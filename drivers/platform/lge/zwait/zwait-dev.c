/*
 * LG Zero-Wait driver
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

#include <linux/module.h>
#include <linux/device.h>
#include <linux/string.h>
#include <linux/seqlock.h>
#include <linux/power_supply.h>
#include <linux/platform_device.h>
#include "zwait.h"

static const char *const map_zw_state[ZW_STATE_MAX] = {
	[ZW_STATE_OFF]		= "off",
	[ZW_STATE_ON_SYSTEM]	= "system",
	[ZW_STATE_ON_USER]	= "user"
};

static ATOMIC_NOTIFIER_HEAD(zw_notifier_list);

static int zw_state = ZW_STATE_OFF;
static DEFINE_SEQLOCK(zw_seqlock);

static int zw_keep_uartcon = 0;
static DEFINE_SEQLOCK(zw_keep_uartcon_seqlock);

static struct class *zw_class;
static struct platform_device *zwait_pdev;

static inline int read_zw_state(void)
{
	int state;
	unsigned seq;

	do {
		seq = read_seqbegin(&zw_seqlock);
		state = zw_state;
	} while (read_seqretry(&zw_seqlock, seq));

	return state;
}

int is_zw_mode(void)
{
	return read_zw_state();
}

static inline int read_zw_keep_uart_console(void)
{
	int val;
	unsigned seq;

	do {
		seq = read_seqbegin(&zw_keep_uartcon_seqlock);
		val = zw_keep_uartcon;
	} while (read_seqretry(&zw_keep_uartcon_seqlock, seq));

	return val;
}

int zw_keep_uart_console(void)
{
	return read_zw_keep_uart_console();
}

int zw_notifier_chain_register(struct notifier_block *nb, void *ptr)
{
	if (nb == NULL)
		return -EINVAL;

	nb->ptr = ptr;

	return atomic_notifier_chain_register(&zw_notifier_list, nb);
}

int zw_notifier_chain_unregister(struct notifier_block *nb)
{
	if (nb == NULL)
		return -EINVAL;

	return atomic_notifier_chain_unregister(&zw_notifier_list, nb);
}

static inline int zw_notifier_call_chain(int state)
{
	return atomic_notifier_call_chain(&zw_notifier_list, state, NULL);
}

static void zw_suspend_handle(int state)
{
	switch (state) {
	case ZW_STATE_OFF:
		power_supply_permit_change_all();
		zw_pwrkey_clean();
		zw_power_pm_clean();
		zw_psy_update();
		zw_notifier_call_chain(state);
		zw_irqs_clean();
		dpm_wakeup_dev_list_clean();
		zw_rtc_clean();
		break;

	case ZW_STATE_ON_SYSTEM:
	case ZW_STATE_ON_USER:
		power_supply_forbid_change_all();
		dpm_wakeup_dev_list_set();
		zw_irqs_set();
		zw_pwrkey_set(state);
		zw_power_pm_set();
		zw_notifier_call_chain(state);
		zw_rtc_set();
		break;

	default:
		pr_err("%s: state(%d) is not supported!\n", __func__, state);
		break;
	}
	pr_debug("%s: state = %s\n", __func__, map_zw_state[state]);
}

static ssize_t zw_state_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	int state = read_zw_state();
	return sprintf(buf, "%s\n", map_zw_state[state]);
}

static ssize_t zw_state_store(struct class *class,
			struct class_attribute *attr,
			const char *buf, size_t count)
{
	int i, len;
	int old_state;
	int ret = -EINVAL;
	char *p;

	p = memchr(buf, '\n', count);
	len = p ? p - buf : count;

	for (i = 0; i < ZW_STATE_MAX; i++) {
		if (strncmp(buf, map_zw_state[i], len) == 0) {
			write_seqlock(&zw_seqlock);
			old_state = zw_state;
			zw_state = i;
			write_sequnlock(&zw_seqlock);

			if (old_state != i)
				zw_suspend_handle(i);

			ret = count;
			break;
		}
	}

	return ret;
}

static CLASS_ATTR(state, 0644, zw_state_show, zw_state_store);

static ssize_t zw_timeout_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	return get_zw_timeout(buf);
}

static ssize_t zw_timeout_store(struct class *class,
			struct class_attribute *attr,
			const char *buf, size_t count)
{
	return set_zw_timeout(buf, count);
}

static CLASS_ATTR(timeout_in_sec, 0644, zw_timeout_show, zw_timeout_store);

static ssize_t zw_delta_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	return get_zw_delta(buf);
}

static ssize_t zw_delta_store(struct class *class,
			struct class_attribute *attr,
			const char *buf, size_t count)
{
	return set_zw_delta(buf, count);
}

static CLASS_ATTR(delta, 0644, zw_delta_show, zw_delta_store);

static ssize_t zw_retry_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", get_zw_rtc_retry());
}

static ssize_t zw_retry_store(struct class *class,
			struct class_attribute *attr,
			const char *buf, size_t count)
{
	int max;

	if (sscanf(buf, "%d", &max) != 1)
		return -EINVAL;

	set_zw_rtc_retry(max);

	return count;
}

static CLASS_ATTR(retry_max, 0644, zw_retry_show, zw_retry_store);

static ssize_t zw_retry_delay_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", get_zw_rtc_retry_delay());
}

static ssize_t zw_retry_delay_store(struct class *class,
			struct class_attribute *attr,
			const char *buf, size_t count)
{
	int msec;

	if (sscanf(buf, "%d", &msec) != 1)
		return -EINVAL;

	set_zw_rtc_retry_delay(msec);

	return count;
}

static CLASS_ATTR(retry_delay_in_ms, 0644,
		zw_retry_delay_show, zw_retry_delay_store);

static ssize_t zw_keep_uart_console_show(struct class *class,
			struct class_attribute *attr, char *buf)
{
	return sprintf(buf, "%d\n", read_zw_keep_uart_console());
}

static ssize_t zw_keep_uart_console_store(struct class *class,
			struct class_attribute *attr,
			const char *buf, size_t count)
{
	int val;

	if (sscanf(buf, "%d", &val) != 1)
		return -EINVAL;

	write_seqlock(&zw_keep_uartcon_seqlock);
	zw_keep_uartcon = val;
	write_sequnlock(&zw_keep_uartcon_seqlock);

	return count;
}

static CLASS_ATTR(keep_uart_console, 0644,
		zw_keep_uart_console_show, zw_keep_uart_console_store);

static struct platform_driver zwait_driver = {
	.driver = {
		.name = "zwait",
	}
};

static int __init zw_init(void)
{
	int ret;

	zw_class = class_create(THIS_MODULE, "zwait");
	if (IS_ERR(zw_class)) {
		ret = PTR_ERR(zw_class);
		goto err_class_create;
	}

	ret = class_create_file(zw_class, &class_attr_state);
	if (ret < 0) {
		goto err_class_create_file1;
	}

	ret = class_create_file(zw_class, &class_attr_timeout_in_sec);
	if (ret < 0) {
		goto err_class_create_file2;
	}

	ret = class_create_file(zw_class, &class_attr_delta);
	if (ret < 0) {
		goto err_class_create_file3;
	}

	ret = class_create_file(zw_class, &class_attr_retry_max);
	if (ret < 0) {
		goto err_class_create_file4;
	}

	ret = class_create_file(zw_class, &class_attr_retry_delay_in_ms);
	if (ret < 0) {
		goto err_class_create_file5;
	}

	ret = class_create_file(zw_class, &class_attr_keep_uart_console);
	if (ret < 0) {
		goto err_class_create_file6;
	}

	zwait_pdev = platform_device_register_simple("zwait", -1, NULL, 0);
	if (IS_ERR(zwait_pdev)) {
		ret = PTR_ERR(zwait_pdev);
		goto err_platform_device_register;
	}

	ret = platform_driver_register(&zwait_driver);
	if (ret < 0) {
		goto err_platform_driver_register;
	}

	return 0;

err_platform_driver_register:
	platform_device_unregister(zwait_pdev);
err_platform_device_register:
	class_remove_file(zw_class, &class_attr_keep_uart_console);
err_class_create_file6:
	class_remove_file(zw_class, &class_attr_retry_delay_in_ms);
err_class_create_file5:
	class_remove_file(zw_class, &class_attr_retry_max);
err_class_create_file4:
	class_remove_file(zw_class, &class_attr_delta);
err_class_create_file3:
	class_remove_file(zw_class, &class_attr_timeout_in_sec);
err_class_create_file2:
	class_remove_file(zw_class, &class_attr_state);
err_class_create_file1:
	class_destroy(zw_class);
err_class_create:
	return ret;
}

static void __exit zw_exit(void)
{
	platform_driver_unregister(&zwait_driver);
	platform_device_unregister(zwait_pdev);
	class_remove_file(zw_class, &class_attr_keep_uart_console);
	class_remove_file(zw_class, &class_attr_retry_delay_in_ms);
	class_remove_file(zw_class, &class_attr_retry_max);
	class_remove_file(zw_class, &class_attr_delta);
	class_remove_file(zw_class, &class_attr_timeout_in_sec);
	class_remove_file(zw_class, &class_attr_state);
	class_destroy(zw_class);
}

module_init(zw_init);
module_exit(zw_exit);

MODULE_DESCRIPTION("LG Zero-Wait driver");
MODULE_AUTHOR("LG Electronics Inc.");
MODULE_LICENSE("GPL");
