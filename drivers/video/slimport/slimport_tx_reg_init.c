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

#include <linux/kernel.h>

#include "slimport_register_set_test.h"
#include "slimport_tx_reg.h"

extern int sp_write_reg(uint8_t slave_addr, uint8_t offset, uint8_t value);

#if defined (CONFIG_MACH_MSM8974_Z_SKT) || defined(CONFIG_MACH_MSM8974_Z_LGU)

static void slimport_tx_link_phy_init_for_z_skt_lgu(void)
{
	pr_err("%s: slimport tx init \n", __func__);
    
	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG0, 0x19);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG4, 0x1b);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG7, 0x22);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG9, 0x23);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG14, 0x09);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG17, 0x16);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG19, 0x1F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG1, 0x26);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG5, 0x28);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG8, 0x2F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG15, 0x10);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG18, 0x1F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG2, 0x3F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG12, 0x0C); //0x10);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG6, 0x3C); //0x3B);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG16, 0x1C); //0x1F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG3, 0x3F);
}

#elif defined (CONFIG_MACH_MSM8974_Z_ATT_US)

static void slimport_tx_link_phy_init_for_z_att_us(void)
{
	pr_err("%s: slimport tx init [131127]\n", __func__);
    
	sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG0, 0x19);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG4, 0x1b);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG7, 0x22);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG9, 0x23);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG14, 0x09);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG17, 0x16);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG19, 0x1F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG1, 0x26);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG5, 0x28);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG8, 0x2F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG15, 0x10);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG18, 0x1F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG2, 0x3F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG12, 0x0D); //10);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG6, 0x3B);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG16, 0x1F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG3, 0x3F);
}

#elif defined (CONFIG_MACH_MSM8974_Z_TMO_US)

static void slimport_tx_link_phy_init_for_z_tmo_us(void)
{
	pr_err("%s: slimport tx init [131127]  \n", __func__);
    
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG0, 0x19);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG4, 0x1b);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG7, 0x22);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG9, 0x23);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG14, 0x09);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG17, 0x16);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG19, 0x1F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG1, 0x26);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG5, 0x28);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG8, 0x2F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG15, 0x10);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG18, 0x1F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG2, 0x3F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG12, 0x0d);//0x10
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG6, 0x3B);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG16, 0x1F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG3, 0x3F);
}

#else

/* PHY parameter for CTS for msm8974 base */
static void slimport_tx_link_phy_init_for_msm8974_base(void)
{
	pr_err("%s: slimport tx init \n", __func__);
    
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG0, 0x19);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG4, 0x1b);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG7, 0x22);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG9, 0x23);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG14, 0x09);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG17, 0x16);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG19, 0x1F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG1, 0x26);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG5, 0x28);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG8, 0x2F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG15, 0x10);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG18, 0x1F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG2, 0x3F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG12, 0x10);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG6, 0x3B);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG16, 0x1F);
    sp_write_reg(TX_P1, SP_TX_LT_CTRL_REG3, 0x3F);
}

#endif

void slimport_tx_link_phy_init(void)
{
#if defined (CONFIG_MACH_MSM8974_Z_SKT) || defined(CONFIG_MACH_MSM8974_Z_LGU)
	slimport_tx_link_phy_init_for_z_skt_lgu();
#elif defined (CONFIG_MACH_MSM8974_Z_ATT_US)
	slimport_tx_link_phy_init_for_z_att_us();
#elif defined (CONFIG_MACH_MSM8974_Z_TMO_US)
	slimport_tx_link_phy_init_for_z_tmo_us();
#else 
	slimport_tx_link_phy_init_for_msm8974_base();
#endif
}
