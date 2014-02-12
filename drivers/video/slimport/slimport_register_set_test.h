/*
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

#ifndef _SLIMPORT_REGISTER_SET_TEST_H_
#define _SLIMPORT_REGISTER_SET_TEST_H_

#include <linux/device.h>

//#define SP_REGISTER_SET_TEST

#ifdef CONFIG_SLIMPORT_REGISTER_SET_TEST
extern unchar val_SP_TX_LT_CTRL_REG0;
extern unchar val_SP_TX_LT_CTRL_REG1;
extern unchar val_SP_TX_LT_CTRL_REG2;
extern unchar val_SP_TX_LT_CTRL_REG3;
extern unchar val_SP_TX_LT_CTRL_REG4;
extern unchar val_SP_TX_LT_CTRL_REG5;
extern unchar val_SP_TX_LT_CTRL_REG6;
extern unchar val_SP_TX_LT_CTRL_REG7;
extern unchar val_SP_TX_LT_CTRL_REG8;
extern unchar val_SP_TX_LT_CTRL_REG9;
extern unchar val_SP_TX_LT_CTRL_REG10;
extern unchar val_SP_TX_LT_CTRL_REG11;
extern unchar val_SP_TX_LT_CTRL_REG12;
extern unchar val_SP_TX_LT_CTRL_REG13;
extern unchar val_SP_TX_LT_CTRL_REG14;
extern unchar val_SP_TX_LT_CTRL_REG15;
extern unchar val_SP_TX_LT_CTRL_REG16;
extern unchar val_SP_TX_LT_CTRL_REG17;
extern unchar val_SP_TX_LT_CTRL_REG18;
extern unchar val_SP_TX_LT_CTRL_REG19;

extern ssize_t ctrl_reg0_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg0_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg1_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg1_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg2_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg2_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg3_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg3_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg4_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg4_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg5_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg5_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg6_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg6_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg7_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg7_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg8_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg8_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg9_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg9_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg10_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg10_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg11_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg11_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg12_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg12_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg13_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg13_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg14_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg14_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg15_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg15_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg16_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg16_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg17_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg17_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg18_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg18_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);

extern ssize_t ctrl_reg19_show(struct device *dev, struct device_attribute *attr,char *buf);
extern ssize_t ctrl_reg19_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count);


extern void anx7808_i2c_probe_reg_init(void);
extern void slimport_tx_link_phy_init_reg_set_print(void);

#endif

#endif /* _SLIMPORT_REGISTER_SET_TEST_H_ */
