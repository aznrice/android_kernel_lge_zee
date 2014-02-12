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

#include "slimport_register_set_test.h"
#include "slimport_tx_reg.h"

extern int sp_write_reg(uint8_t slave_addr, uint8_t offset, uint8_t value);

#ifdef CONFIG_SLIMPORT_REGISTER_SET_TEST

unchar val_SP_TX_LT_CTRL_REG0;
unchar val_SP_TX_LT_CTRL_REG1;
unchar val_SP_TX_LT_CTRL_REG2;
unchar val_SP_TX_LT_CTRL_REG3;
unchar val_SP_TX_LT_CTRL_REG4;
unchar val_SP_TX_LT_CTRL_REG5;
unchar val_SP_TX_LT_CTRL_REG6;
unchar val_SP_TX_LT_CTRL_REG7;
unchar val_SP_TX_LT_CTRL_REG8;
unchar val_SP_TX_LT_CTRL_REG9;
unchar val_SP_TX_LT_CTRL_REG10;
unchar val_SP_TX_LT_CTRL_REG11;
unchar val_SP_TX_LT_CTRL_REG12;
unchar val_SP_TX_LT_CTRL_REG13;
unchar val_SP_TX_LT_CTRL_REG14;
unchar val_SP_TX_LT_CTRL_REG15;
unchar val_SP_TX_LT_CTRL_REG16;
unchar val_SP_TX_LT_CTRL_REG17;
unchar val_SP_TX_LT_CTRL_REG18;
unchar val_SP_TX_LT_CTRL_REG19;


/* reg0 */
ssize_t ctrl_reg0_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG0);
}
ssize_t ctrl_reg0_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG0 = val;
	return count;
}

/* reg1 */
ssize_t ctrl_reg1_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG1);
}
ssize_t ctrl_reg1_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG1 = val;
	return count;
}

/* reg2 */
ssize_t ctrl_reg2_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG2);
}
ssize_t ctrl_reg2_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG2 = val;
	return count;
}

/* reg3 */
ssize_t ctrl_reg3_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG3);
}
ssize_t ctrl_reg3_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG3 = val;
	return count;
}

/* reg4 */
ssize_t ctrl_reg4_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG4);
}
ssize_t ctrl_reg4_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG4 = val;
	return count;
}

/* reg5 */
ssize_t ctrl_reg5_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG5);
}
ssize_t ctrl_reg5_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG5 = val;
	return count;
}

/* reg6 */
ssize_t ctrl_reg6_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG6);
}
ssize_t ctrl_reg6_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG6 = val;
	return count;
}

/* reg7 */
ssize_t ctrl_reg7_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG7);
}
ssize_t ctrl_reg7_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG7 = val;
	return count;
}

/* reg8 */
ssize_t ctrl_reg8_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG8);
}
ssize_t ctrl_reg8_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG8 = val;
	return count;
}

/* reg9 */
ssize_t ctrl_reg9_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG9);
}
ssize_t ctrl_reg9_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG9 = val;
	return count;
}

/* reg10 */
ssize_t ctrl_reg10_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG10);
}
ssize_t ctrl_reg10_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG10 = val;
	return count;
}

/* reg11 */
ssize_t ctrl_reg11_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG11);
}
ssize_t ctrl_reg11_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG11 = val;
	return count;
}

/* reg12 */
ssize_t ctrl_reg12_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG12);
}
ssize_t ctrl_reg12_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG12 = val;
	return count;
}

/* reg13 */
ssize_t ctrl_reg13_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG13);
}
ssize_t ctrl_reg13_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG13 = val;
	return count;
}

/* reg14 */
ssize_t ctrl_reg14_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG14);
}
ssize_t ctrl_reg14_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG14 = val;
	return count;
}

/* reg15 */
ssize_t ctrl_reg15_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG15);
}
ssize_t ctrl_reg15_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG15 = val;
	return count;
}

/* reg16 */
ssize_t ctrl_reg16_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG16);
}
ssize_t ctrl_reg16_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG16 = val;
	return count;
}

/* reg17 */
ssize_t ctrl_reg17_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG17);
}
ssize_t ctrl_reg17_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG17 = val;
	return count;
}

/* reg18 */
ssize_t ctrl_reg18_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG18);
}
ssize_t ctrl_reg18_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG18 = val;
	return count;
}

/* reg19 */
ssize_t ctrl_reg19_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "0x%x\n", val_SP_TX_LT_CTRL_REG19);
}
ssize_t ctrl_reg19_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	int ret;
	long val;
	ret = strict_strtol(buf, 10, &val);
	if (ret)
		return ret;
	val_SP_TX_LT_CTRL_REG19 = val;
	return count;
}

void anx7808_i2c_probe_reg_init(void)
{
	val_SP_TX_LT_CTRL_REG0 = 0x19;
	val_SP_TX_LT_CTRL_REG4 = 0x1b;
	val_SP_TX_LT_CTRL_REG7 = 0x22;
	val_SP_TX_LT_CTRL_REG9 = 0x23;
	val_SP_TX_LT_CTRL_REG14 = 0x09;
	val_SP_TX_LT_CTRL_REG17 = 0x16;
	val_SP_TX_LT_CTRL_REG19 = 0x1F;
	val_SP_TX_LT_CTRL_REG1 = 0x26;
	val_SP_TX_LT_CTRL_REG5 = 0x28;
	val_SP_TX_LT_CTRL_REG8 = 0x2F;
	val_SP_TX_LT_CTRL_REG15 = 0x10;
	val_SP_TX_LT_CTRL_REG18 = 0x1F;
	val_SP_TX_LT_CTRL_REG2 = 0x3F;
	val_SP_TX_LT_CTRL_REG12 = 0x10;
	val_SP_TX_LT_CTRL_REG6 = 0x3B;
	val_SP_TX_LT_CTRL_REG16 = 0x1F;
	val_SP_TX_LT_CTRL_REG3 = 0x3F;
}

void slimport_tx_link_phy_init_reg_set_print(void)
{
	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG0, val_SP_TX_LT_CTRL_REG0);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG0 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG0);

	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG4, val_SP_TX_LT_CTRL_REG4);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG4 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG4);
	
	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG7, val_SP_TX_LT_CTRL_REG7);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG7 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG7);
	
	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG9, val_SP_TX_LT_CTRL_REG9);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG9 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG9);

	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG14, val_SP_TX_LT_CTRL_REG14);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG14 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG14);

	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG17, val_SP_TX_LT_CTRL_REG17);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG17 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG17);
	
	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG19, val_SP_TX_LT_CTRL_REG19);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG19 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG19);
	
	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG1, val_SP_TX_LT_CTRL_REG1);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG1 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG1);

	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG5, val_SP_TX_LT_CTRL_REG5);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG5 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG5);

	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG8, val_SP_TX_LT_CTRL_REG8);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG8 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG8);

	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG15, val_SP_TX_LT_CTRL_REG15);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG15 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG15);

	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG18, val_SP_TX_LT_CTRL_REG18);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG18 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG18);
	
	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG2, val_SP_TX_LT_CTRL_REG2); 
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG2 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG2);
	
	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG12, val_SP_TX_LT_CTRL_REG12);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG12 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG12);
	
	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG6, val_SP_TX_LT_CTRL_REG6);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG6 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG6);
	
	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG16, val_SP_TX_LT_CTRL_REG16);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG16 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG16);
	
	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG3, val_SP_TX_LT_CTRL_REG3);
	printk ("%s : set register : val_SP_TX_LT_CTRL_REG3 = 0x%x\n",__func__, val_SP_TX_LT_CTRL_REG3);

}

#endif
