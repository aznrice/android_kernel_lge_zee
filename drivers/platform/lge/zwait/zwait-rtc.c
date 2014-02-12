/*
 * LG Zero-Wait RTC
 *
 * Copyright (C) 2013 LG Electronics Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#include <linux/device.h>
#include <linux/slab.h>
#include <linux/rtc.h>
#include <linux/delay.h>
#include <linux/reboot.h>
#include <linux/workqueue.h>
#include <linux/fs.h>
#include <linux/zwait.h>
#include "zwait.h"

enum {
	ZW_BATTERY_NORMAL = 0,
	ZW_BATTERY_LOW,
	ZW_BATTERY_VERYLOW,
	ZW_BATTERY_STATUS_MAX
};

struct zw_check_data {
	struct {
		int max;	/* capacity */
		int min;	/* capacity */
	} range;
	int delta;		/* capacity */
	unsigned long timeout;	/* seconds */
};

struct zw_rtc {
	struct list_head entry;
	struct rtc_device *rtc;
	struct rtc_task *task;
};

static LIST_HEAD(zw_rtc_list);
static DEFINE_MUTEX(zw_rtc_list_mtx);

static struct zw_check_data zw_check_table[ZW_BATTERY_STATUS_MAX] = {
	[ZW_BATTERY_NORMAL] = {
		.range = {100, 20},	/* 100% ~ 20% */
		.delta = 5,
		.timeout = (60 * 60UL),	/* 1 hour */
	},
	[ZW_BATTERY_LOW] = {
		.range = {19 , 6},	/* 19% ~ 6% */
		.delta = 3,
		.timeout = (30 * 60UL),	/* 30 minutes */
	},
	[ZW_BATTERY_VERYLOW] = {
		.range = {5, 1},	/* 5% ~ 1% */
		.delta = 0,
		.timeout = (15 * 60UL),	/* 15 minutes */
	}
};

static int zw_batt_capacity;
static int zw_batt_status;
static int zw_rtc_retry = 3;
static int zw_rtc_retry_delay = 200;	/* msecs */

static void zw_rtc_work_func(struct work_struct *work);
static DECLARE_WORK(zw_rtc_work, zw_rtc_work_func);

ssize_t get_zw_timeout(char *buf)
{
	return sprintf(buf, "normal: %lu, low: %lu, very low: %lu\n",
			zw_check_table[ZW_BATTERY_NORMAL].timeout,
			zw_check_table[ZW_BATTERY_LOW].timeout,
			zw_check_table[ZW_BATTERY_VERYLOW].timeout);
}

ssize_t set_zw_timeout(const char *buf, size_t count)
{
	unsigned long normal;
	unsigned long low;
	unsigned long very_low;

	if (sscanf(buf, "%lu %lu %lu", &normal, &low, &very_low) != 3)
		return -EINVAL;

	zw_check_table[ZW_BATTERY_NORMAL].timeout = normal;
	zw_check_table[ZW_BATTERY_LOW].timeout = low;
	zw_check_table[ZW_BATTERY_VERYLOW].timeout = very_low;

	return count;
}

ssize_t get_zw_delta(char *buf)
{
	return sprintf(buf, "normal: %d, low: %d, very low: %d\n",
			zw_check_table[ZW_BATTERY_NORMAL].delta,
			zw_check_table[ZW_BATTERY_LOW].delta,
			zw_check_table[ZW_BATTERY_VERYLOW].delta);
}

ssize_t set_zw_delta(const char *buf, size_t count)
{
	int normal;
	int low;
	int very_low;

	if (sscanf(buf, "%d %d %d", &normal, &low, &very_low) != 3)
		return -EINVAL;

	zw_check_table[ZW_BATTERY_NORMAL].delta = normal;
	zw_check_table[ZW_BATTERY_LOW].delta = low;
	zw_check_table[ZW_BATTERY_VERYLOW].delta = very_low;

	return count;
}

int get_zw_rtc_retry(void)
{
	return zw_rtc_retry;
}

void set_zw_rtc_retry(int max)
{
	zw_rtc_retry = max;
}

int get_zw_rtc_retry_delay(void)
{
	return zw_rtc_retry_delay;
}

void set_zw_rtc_retry_delay(int msec)
{
	zw_rtc_retry_delay = msec;
}

static void zw_rtc_irq_handler(void *private_data)
{
	schedule_work(&zw_rtc_work);
}

static struct rtc_task zw_rtc_task = {
	.func = zw_rtc_irq_handler,
};

static inline void zw_rtc_task_set(void)
{
	struct zw_rtc *data;
	unsigned long flags;

	mutex_lock(&zw_rtc_list_mtx);
	list_for_each_entry(data, &zw_rtc_list, entry) {
		spin_lock_irqsave(&data->rtc->irq_task_lock, flags);
		data->task = data->rtc->irq_task;
		data->rtc->irq_task = NULL;
		spin_unlock_irqrestore(&data->rtc->irq_task_lock, flags);
	}

	data = list_first_entry(&zw_rtc_list, struct zw_rtc, entry);
	spin_lock_irqsave(&data->rtc->irq_task_lock, flags);
	data->rtc->irq_task = &zw_rtc_task;
	spin_unlock_irqrestore(&data->rtc->irq_task_lock, flags);
	mutex_unlock(&zw_rtc_list_mtx);
}

static inline void zw_rtc_task_clean(void)
{
	struct zw_rtc *data;
	unsigned long flags;

	mutex_lock(&zw_rtc_list_mtx);
	list_for_each_entry(data, &zw_rtc_list, entry) {
		spin_lock_irqsave(&data->rtc->irq_task_lock, flags);
		data->rtc->irq_task = data->task;
		data->task = NULL;
		spin_unlock_irqrestore(&data->rtc->irq_task_lock, flags);
	}
	mutex_unlock(&zw_rtc_list_mtx);
}

static inline void __zw_rtc_read_time(struct rtc_device *rtc,
				struct rtc_time *tm)
{
	int i;

	for (i = 0; i < zw_rtc_retry; i++) {
		if (rtc_read_time(rtc, tm) == 0)
			return;

		pr_err("%s: fail to rtc_read_time() (try = %d)",
			__func__, i + 1);
		if (i < (zw_rtc_retry - 1))
			mdelay(zw_rtc_retry_delay);
	}

	zw_emergency_remount();
	kernel_power_off();
}

static inline void __zw_rtc_set_alarm(struct rtc_device *rtc,
				struct rtc_wkalrm *alarm)
{
	int i;

	for (i = 0; i < zw_rtc_retry; i++) {
		if (rtc_set_alarm(rtc, alarm) == 0)
			return;

		pr_err("%s: fail to rtc_set_alarm() (try = %d)",
			__func__, i + 1);
		if (i < (zw_rtc_retry - 1))
			mdelay(zw_rtc_retry_delay);
	}

	zw_emergency_remount();
	kernel_power_off();
}

static inline void zw_rtc_set_alarm(bool enable)
{
	struct zw_rtc *data;
	struct rtc_wkalrm tmp;
	unsigned long time;

	memset(&tmp, 0, sizeof(struct rtc_wkalrm));

	mutex_lock(&zw_rtc_list_mtx);
	data = list_first_entry(&zw_rtc_list, struct zw_rtc, entry);

	__zw_rtc_read_time(data->rtc, &tmp.time);
	rtc_tm_to_time(&tmp.time, &time);

	if (enable) {
		tmp.enabled = 1;
		time += zw_check_table[zw_batt_status].timeout;
	}

	rtc_time_to_tm(time, &tmp.time);
	__zw_rtc_set_alarm(data->rtc, &tmp);

	mutex_unlock(&zw_rtc_list_mtx);
}

static int zw_battery_status(int capacity)
{
	int i;

	if (capacity > 100)
		capacity = 100;

	for (i = 0; i < ZW_BATTERY_STATUS_MAX; i++) {
		if ((capacity <= zw_check_table[i].range.max)
			&& (capacity >= zw_check_table[i].range.min)) {
			return i;
		}
	}

	WARN(1, "%s: out of range(capacity = %d)\n", __func__, capacity);
	return ZW_BATTERY_VERYLOW;
}

static inline void zw_power_off_force(void)
{
	pr_info("%s: force to power off\n", __func__);
	zw_rtc_set_alarm(0);
	zw_rtc_task_clean();
	zw_emergency_remount();
	kernel_power_off();
}

static void zw_rtc_work_func(struct work_struct *work)
{
	int current_capacity;
	int delta = zw_check_table[zw_batt_status].delta;

	pr_info("Timeout in Zero Wait mode!\n");

	current_capacity = zw_psy_get_batt_capacity();
	pr_info("%s: battery capacity = %d\n", __func__, current_capacity);

	if (current_capacity > zw_batt_capacity)
		current_capacity = zw_batt_capacity;

	if ((zw_batt_capacity - current_capacity) >= delta) {
		zw_power_off_force();
	} else if (zw_batt_status == ZW_BATTERY_LOW) {
		if (current_capacity < zw_check_table[zw_batt_status].range.min)
			zw_power_off_force();
	}

	/* set rtc alarm & suspend again */
	zw_rtc_set_alarm(1);
}

void zw_rtc_set(void)
{
	zw_batt_capacity = zw_psy_get_batt_capacity();
	pr_info("%s: battery capacity = %d\n", __func__, zw_batt_capacity);

	zw_batt_status = zw_battery_status(zw_batt_capacity);

	zw_rtc_task_set();
	zw_rtc_set_alarm(1);
}

void zw_rtc_clean(void)
{
	zw_rtc_set_alarm(0);
	zw_rtc_task_clean();
}

int zw_rtc_info_register(struct rtc_device *rtc)
{
	struct zw_rtc *data;

	if (rtc == NULL)
		return -EINVAL;

	data = kzalloc(sizeof(struct zw_rtc), GFP_KERNEL);
	if (data == NULL)
		return -ENOMEM;

	data->rtc = rtc;

	mutex_lock(&zw_rtc_list_mtx);
	list_add_tail(&data->entry, &zw_rtc_list);
	mutex_unlock(&zw_rtc_list_mtx);

	return 0;
}

void zw_rtc_info_unregister(struct rtc_device *rtc)
{
	struct zw_rtc *data;

	if (rtc == NULL)
		return;

	mutex_lock(&zw_rtc_list_mtx);
	list_for_each_entry(data, &zw_rtc_list, entry) {
		if (data->rtc == rtc) {
			list_del(&data->entry);
			data->rtc = NULL;
			data->task = NULL;
			kfree(data);
			break;
		}
	}
	mutex_unlock(&zw_rtc_list_mtx);
}
