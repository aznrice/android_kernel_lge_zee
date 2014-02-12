/*
 * LG Zero-Wait IRQs simple control (exclude gpio-keys)
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

#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/zwait.h>

struct zw_irqs {
	struct list_head entry;
	unsigned int irq;
	int wakeup;
};

static LIST_HEAD(zw_irqs_list);
static DEFINE_MUTEX(zw_irqs_list_mtx);

void zw_irqs_set(void)
{
	struct zw_irqs *data;

	mutex_lock(&zw_irqs_list_mtx);
	list_for_each_entry(data, &zw_irqs_list, entry) {
		if (data->wakeup)
			zw_disable_irq_wake(data->irq);
		disable_irq(data->irq);
	}
	mutex_unlock(&zw_irqs_list_mtx);
}

void zw_irqs_clean(void)
{
	struct zw_irqs *data;

	mutex_lock(&zw_irqs_list_mtx);
	list_for_each_entry(data, &zw_irqs_list, entry) {
		enable_irq(data->irq);
		if (data->wakeup)
			zw_enable_irq_wake(data->irq);
	}
	mutex_unlock(&zw_irqs_list_mtx);
}

void zw_irqs_info_register(unsigned int irq, int wakeup)
{
	int match = 0;
	struct zw_irqs *data;

	mutex_lock(&zw_irqs_list_mtx);
	list_for_each_entry(data, &zw_irqs_list, entry) {
		if (data->irq == irq) {
			if (wakeup)
				data->wakeup = wakeup;
			match = 1;
			WARN(1, "%s: irq %d is already registered!\n",
				__func__, irq);
		}
	}

	if (!match) {
		data = kzalloc(sizeof(struct zw_irqs), GFP_KERNEL);
		if (data == NULL)
			return;

		data->irq = irq;
		data->wakeup = wakeup;
		list_add_tail(&data->entry, &zw_irqs_list);
	}
	mutex_unlock(&zw_irqs_list_mtx);
}

void zw_irqs_info_unregister(unsigned int irq)
{
	struct zw_irqs *data;

	mutex_lock(&zw_irqs_list_mtx);
	list_for_each_entry(data, &zw_irqs_list, entry) {
		if (data->irq == irq) {
			list_del(&data->entry);
			kfree(data);
			break;
		}
	}
	mutex_unlock(&zw_irqs_list_mtx);
}
