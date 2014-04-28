#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/err.h>
#include <linux/gpio.h>
#include <linux/irq.h>
#include <linux/if.h>
#include <linux/random.h>
#include <linux/io.h>
#ifdef CONFIG_WIFI_CONTROL_FUNC
#include <linux/skbuff.h>
#include <linux/wlan_plat.h>
#endif
#include <mach/board_lge.h> /* add for hw revision check by hayun.kim */
#include <linux/pm_qos.h>

#define WLAN_POWER    26
#define WLAN_HOSTWAKE 44

static int gpio_wlan_power = WLAN_POWER; /* add for hw revision check by hayun.kim */
static int gpio_wlan_hostwake = WLAN_HOSTWAKE; /* add for hw revision check by hayun.kim */

static unsigned wlan_wakes_msm[] = {
	    GPIO_CFG(WLAN_HOSTWAKE, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA) };

/* for wifi power supply */
static unsigned wifi_config_power_on[] = {
	    GPIO_CFG(WLAN_POWER, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA) };

#if defined(CONFIG_BCM4335BT)
extern int bcm_bt_lock(int cookie);
extern void bcm_bt_unlock(int cookie);
static int lock_cookie_wifi = 'W' | 'i'<<8 | 'F'<<16 | 'i'<<24; /* cookie is "WiFi" */
#endif /* defined(CONFIG_BCM4335BT) */

/* For broadcom */
#ifdef CONFIG_BROADCOM_WIFI_RESERVED_MEM

#define WLAN_STATIC_SCAN_BUF0		5
#define WLAN_STATIC_SCAN_BUF1		6
#define PREALLOC_WLAN_SEC_NUM		4
#define PREALLOC_WLAN_BUF_NUM		160
#define PREALLOC_WLAN_SECTION_HEADER		24

#define WLAN_SECTION_SIZE_0	(PREALLOC_WLAN_BUF_NUM * 128)
#define WLAN_SECTION_SIZE_1	(PREALLOC_WLAN_BUF_NUM * 128)
#define WLAN_SECTION_SIZE_2	(PREALLOC_WLAN_BUF_NUM * 512)
#define WLAN_SECTION_SIZE_3	(PREALLOC_WLAN_BUF_NUM * 1024)

#define DHD_SKB_HDRSIZE			336
#define DHD_SKB_1PAGE_BUFSIZE	((PAGE_SIZE*1)-DHD_SKB_HDRSIZE)
#define DHD_SKB_2PAGE_BUFSIZE	((PAGE_SIZE*2)-DHD_SKB_HDRSIZE)
#define DHD_SKB_4PAGE_BUFSIZE	((PAGE_SIZE*4)-DHD_SKB_HDRSIZE)

#define WLAN_SKB_BUF_NUM	17

#define LGE_BCM_WIFI_DMA_QOS_CONTROL

static struct sk_buff *wlan_static_skb[WLAN_SKB_BUF_NUM];

struct wlan_mem_prealloc {
	void *mem_ptr;
	unsigned long size;
};

static struct wlan_mem_prealloc wlan_mem_array[PREALLOC_WLAN_SEC_NUM] = {
	{ NULL, (WLAN_SECTION_SIZE_0 + PREALLOC_WLAN_SECTION_HEADER) },
	{ NULL, (WLAN_SECTION_SIZE_1 + PREALLOC_WLAN_SECTION_HEADER) },
	{ NULL, (WLAN_SECTION_SIZE_2 + PREALLOC_WLAN_SECTION_HEADER) },
	{ NULL, (WLAN_SECTION_SIZE_3 + PREALLOC_WLAN_SECTION_HEADER) }
};

void *wlan_static_scan_buf0;
void *wlan_static_scan_buf1;
static void *brcm_wlan_mem_prealloc(int section, unsigned long size)
{
	if (section == PREALLOC_WLAN_SEC_NUM)
		return wlan_static_skb;
	if (section == WLAN_STATIC_SCAN_BUF0)
		return wlan_static_scan_buf0;
	if (section == WLAN_STATIC_SCAN_BUF1)
		return wlan_static_scan_buf1;
	if ((section < 0) || (section > PREALLOC_WLAN_SEC_NUM))
		return NULL;

	if (wlan_mem_array[section].size < size)
		return NULL;

	return wlan_mem_array[section].mem_ptr;
}

static int brcm_init_wlan_mem(void)
{
	int i;
	int j;

	for (i = 0; i < 8; i++) {
		wlan_static_skb[i] = dev_alloc_skb(DHD_SKB_1PAGE_BUFSIZE);
		if (!wlan_static_skb[i])
			goto err_skb_alloc;
	}

	for (; i < 16; i++) {
		wlan_static_skb[i] = dev_alloc_skb(DHD_SKB_2PAGE_BUFSIZE);
		if (!wlan_static_skb[i])
			goto err_skb_alloc;
	}

	wlan_static_skb[i] = dev_alloc_skb(DHD_SKB_4PAGE_BUFSIZE);
	if (!wlan_static_skb[i])
		goto err_skb_alloc;

	for (i = 0; i < PREALLOC_WLAN_SEC_NUM; i++) {
		wlan_mem_array[i].mem_ptr =
				kmalloc(wlan_mem_array[i].size, GFP_KERNEL);

		if (!wlan_mem_array[i].mem_ptr)
			goto err_mem_alloc;
}
	wlan_static_scan_buf0 = kmalloc(65536, GFP_KERNEL);
	if (!wlan_static_scan_buf0)
		goto err_mem_alloc;
	wlan_static_scan_buf1 = kmalloc(65536, GFP_KERNEL);
	if (!wlan_static_scan_buf1)
		goto err_mem_alloc;

	printk("%s: WIFI MEM Allocated\n", __func__);
	return 0;

 err_mem_alloc:
	pr_err("Failed to mem_alloc for WLAN\n");
	for (j = 0; j < i; j++)
		kfree(wlan_mem_array[j].mem_ptr);

	i = WLAN_SKB_BUF_NUM;

 err_skb_alloc:
	pr_err("Failed to skb_alloc for WLAN\n");
	for (j = 0; j < i; j++)
		dev_kfree_skb(wlan_static_skb[j]);

	return -ENOMEM;
}
#endif /* CONFIG_BROADCOM_WIFI_RESERVED_MEM */

static unsigned int g_wifi_detect;
static void *sdc2_dev;
void (*sdc2_status_cb)(int card_present, void *dev);

int sdc2_status_register(void (*cb)(int card_present, void *dev), void *dev)
{

	printk(KERN_ERR "%s: Dubugging Point 1\n", __func__);

	if (sdc2_status_cb)
		return -EINVAL;
	sdc2_status_cb = cb;
	sdc2_dev = dev;
	return 0;
}

unsigned int sdc2_status(struct device *dev)
{
	printk("J:%s> g_wifi_detect = %d\n", __func__, g_wifi_detect);
	return g_wifi_detect;
}

#ifdef LGE_BCM_WIFI_DMA_QOS_CONTROL
static int wifi_dma_state; /* 0 : INATIVE, 1:INIT, 2:IDLE, 3:ACTIVE */
static struct pm_qos_request wifi_dma_qos;
static struct delayed_work req_dma_work;
static uint32_t packet_transfer_cnt;

static void bcm_wifi_req_dma_work(struct work_struct *work)
{
	switch (wifi_dma_state) {
		case 2: /* IDLE State */
			if (packet_transfer_cnt < 100) {
				/* IDLE -> INIT */
				wifi_dma_state = 1;
				/* printk(KERN_ERR "%s: schedule work : %d : (IDLE -> INIT)\n", __func__, packet_transfer_cnt); */
			} else {
				/* IDLE -> ACTIVE */
				wifi_dma_state = 3;
				pm_qos_update_request(&wifi_dma_qos, 7);
				schedule_delayed_work(&req_dma_work, msecs_to_jiffies(50));
				/* printk(KERN_ERR "%s: schedule work : %d : (IDLE -> ACTIVE)\n", __func__, packet_transfer_cnt); */
			}
			break;

		case 3: /* ACTIVE State */
			if (packet_transfer_cnt < 10) {
				/* ACTIVE -> IDLE */
				wifi_dma_state = 2;
				pm_qos_update_request(&wifi_dma_qos, PM_QOS_DEFAULT_VALUE);
				schedule_delayed_work(&req_dma_work, msecs_to_jiffies(1000));
				/* printk(KERN_ERR "%s: schedule work : %d : (ACTIVE -> IDLE)\n", __func__, packet_transfer_cnt); */
			} else {
				/* Keep ACTIVE */
				schedule_delayed_work(&req_dma_work, msecs_to_jiffies(50));
				/* printk(KERN_ERR "%s: schedule work : %d :  (ACTIVE -> ACTIVE)\n", __func__, packet_transfer_cnt); */
			}
			break;

		default:
			break;

	}

	packet_transfer_cnt = 0;
}

void bcm_wifi_req_dma_qos(int vote)
{
	if (vote)
		packet_transfer_cnt++;

	/* INIT -> IDLE */
	if (wifi_dma_state == 1 && vote) {
		wifi_dma_state = 2; /* IDLE */
		schedule_delayed_work(&req_dma_work, msecs_to_jiffies(1000));
		/* printk(KERN_ERR "%s: schedule work (INIT -> IDLE)\n", __func__); */
	}
}
#endif

int bcm_wifi_reinit_gpio(void)
{
	int rc = 0;

	int hw_rev = HW_REV_A;

	/* set gpio value */
	hw_rev = lge_get_board_revno();

	gpio_wlan_hostwake	= 44;
	gpio_wlan_power	= 26;

	printk(KERN_ERR "%s: rev=%d, gpio_power=%d, gpio_hostwakeup=%d\n", __func__, hw_rev, gpio_wlan_power, gpio_wlan_hostwake);

	/* COMMON */
	wlan_wakes_msm[0] = GPIO_CFG(gpio_wlan_hostwake, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA);
	wifi_config_power_on[0] = GPIO_CFG(gpio_wlan_power, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA);

	/* WLAN_POWER */
	if (gpio_tlmm_config(wifi_config_power_on[0], GPIO_CFG_ENABLE))
		printk(KERN_ERR "%s: Failed to configure WLAN_POWER\n", __func__);

	/* HOST_WAKEUP */
	rc = gpio_tlmm_config(wlan_wakes_msm[0], GPIO_CFG_ENABLE);
	if (rc)
		printk(KERN_ERR "%s: Failed to configure wlan_wakes_msm = %d\n", __func__, rc);

	if (gpio_direction_input(gpio_wlan_hostwake))
		printk(KERN_ERR "%s: WL_HOSTWAKE failed direction in\n", __func__);

	return 0;
}

int bcm_wifi_set_power(int enable)
{
	int ret = 0;
	static int is_initialized;

	if (is_initialized == 0) {
		bcm_wifi_reinit_gpio();
		mdelay(10);
		is_initialized = 1;
	}

#if defined(CONFIG_BCM4335BT)
	printk(KERN_INFO "%s: trying to acquire BT lock\n", __func__);
	if (bcm_bt_lock(lock_cookie_wifi) != 0)
		printk(KERN_INFO "%s:** WiFi: timeout in acquiring bt lock**\n", __func__);
	else
		printk(KERN_INFO "%s: btlock acquired\n", __func__);
#endif /* defined(CONFIG_BCM4335BT) */

	if (enable) {
		ret = gpio_direction_output(gpio_wlan_power, 1);
		if (ret) {
			printk(KERN_ERR "%s: WL_REG_ON  failed to pull up (%d)\n", \
					__func__, ret);
			ret = -EIO;
			goto out;
		}

		/* WLAN chip to reset */
		mdelay(150); /* for booting time save */
		printk(KERN_INFO "J:%s: applied delay. 150ms\n", __func__);
		printk(KERN_ERR "%s: wifi power successed to pull up\n", __func__);

	} else {
		ret = gpio_direction_output(gpio_wlan_power, 0);
		if (ret) {
			printk(KERN_ERR "%s:  WL_REG_ON  failed to pull down (%d)\n",
					__func__, ret);
			ret = -EIO;
			goto out;
		}

		/* WLAN chip down */
		mdelay(100);/* for booting time save */
		printk(KERN_ERR "%s: wifi power successed to pull down\n", __func__);
	}

#if defined(CONFIG_BCM4335BT)
	bcm_bt_unlock(lock_cookie_wifi);
#endif /* defined(CONFIG_BCM4335BT) */

	return ret;

out:
#if defined(CONFIG_BCM4335BT)
	/* For a exceptional case, release btlock */
	printk(KERN_INFO "%s: exceptional bt_unlock\n", __func__);
	bcm_bt_unlock(lock_cookie_wifi);
#endif /* defined(CONFIG_BCM4335BT) */

	return ret;
}

int __init bcm_wifi_init_gpio_mem(struct platform_device *platdev)
{
	int rc = 0;

	/* add for hw revision check by hayun.kim, START */
	int hw_rev = HW_REV_A;

	hw_rev = lge_get_board_revno();

	gpio_wlan_hostwake	= 44;
	gpio_wlan_power	= 26;

	printk(KERN_ERR "%s: rev=%d, gpio_power=%d, gpio_hostwakeup=%d\n", __func__, hw_rev, gpio_wlan_power, gpio_wlan_hostwake);

	/* COMMON */
	wlan_wakes_msm[0] = GPIO_CFG(gpio_wlan_hostwake, 0, GPIO_CFG_INPUT, GPIO_CFG_PULL_DOWN, GPIO_CFG_2MA);
	wifi_config_power_on[0] = GPIO_CFG(gpio_wlan_power, 0, GPIO_CFG_OUTPUT, GPIO_CFG_NO_PULL, GPIO_CFG_2MA);
	/* add for hw revision check by hayun.kim, END */

	/* WLAN_POWER */
	if (gpio_tlmm_config(wifi_config_power_on[0], GPIO_CFG_ENABLE))
		printk(KERN_ERR "%s: Failed to configure WLAN_POWER\n", __func__);

	if (gpio_request(gpio_wlan_power, "WL_REG_ON"))
		printk(KERN_ERR "Failed to request gpio %d for WL_REG_ON\n", gpio_wlan_power);

	if (gpio_direction_output(gpio_wlan_power, 0))
		printk(KERN_ERR "%s: WL_REG_ON  failed direction out\n", __func__);

	/* HOST_WAKEUP */
	rc = gpio_tlmm_config(wlan_wakes_msm[0], GPIO_CFG_ENABLE);
	if (rc)
		printk(KERN_ERR "%s: Failed to configure wlan_wakes_msm = %d\n", __func__, rc);
	if (gpio_request(gpio_wlan_hostwake, "wlan_wakes_msm"))
		printk(KERN_ERR "Failed to request gpio %d for wlan_wakes_msm\n", gpio_wlan_hostwake);
	if (gpio_direction_input(gpio_wlan_hostwake))
		printk(KERN_ERR "%s: WL_HOSTWAKE failed direction in\n", __func__);


	/* For MSM8974_S */
	if (platdev != NULL) {
		struct resource *resource = platdev->resource;
		if (resource != NULL) {
			resource->start = gpio_to_irq(gpio_wlan_hostwake);
			resource->end = gpio_to_irq(gpio_wlan_hostwake);

			printk(KERN_INFO "J:%s> resource->start = %d\n", __func__, resource->start);
		}
	}
	/* For MSM8974_E */

#ifdef CONFIG_BROADCOM_WIFI_RESERVED_MEM
	brcm_init_wlan_mem();
#endif

	printk(KERN_INFO "bcm_wifi_init_gpio_mem successfully\n");

	return 0;
}

static int bcm_wifi_reset(int on)
{
	return 0;
}

static int bcm_wifi_carddetect(int val)
{
	g_wifi_detect = val;
	if (sdc2_status_cb)
		sdc2_status_cb(val, sdc2_dev);
	else
		printk(KERN_INFO "%s:There is no callback for notify\n", __func__);
	return 0;
}

static int bcm_wifi_get_mac_addr(unsigned char *buf)
{
	uint rand_mac;
	static unsigned char mymac[6] = {0,};
	const unsigned char nullmac[6] = {0,};
	pr_debug("%s: %p\n", __func__, buf);

	if (buf == NULL)
		return -EAGAIN;

	if (memcmp( mymac, nullmac, 6) != 0 ) {
		/* Mac displayed from UI are never updated..
		   So, mac obtained on initial time is used */
		memcpy(buf, mymac, 6);
		return 0;
	}

	srandom32((uint)jiffies);
	rand_mac = random32();
	buf[0] = 0x00;
	buf[1] = 0x90;
	buf[2] = 0x4c;
	buf[3] = (unsigned char)rand_mac;
	buf[4] = (unsigned char)(rand_mac >> 8);
	buf[5] = (unsigned char)(rand_mac >> 16);

	memcpy(mymac, buf, 6);

	printk(KERN_INFO "[%s] Exiting. MyMac :  %x : %x : %x : %x : %x : %x", \
		__func__ , buf[0], buf[1], buf[2], buf[3], buf[4], buf[5]);

	return 0;
}

#define COUNTRY_BUF_SZ	4
struct cntry_locales_custom {
	char iso_abbrev[COUNTRY_BUF_SZ];
	char custom_locale[COUNTRY_BUF_SZ];
	int custom_locale_rev;
};

/* Customized Locale table */
const struct cntry_locales_custom bcm_wifi_translate_custom_table[] = {
/* Table should be filled out based on custom platform regulatory requirement */
	/* 4330/4334/43241 */
	{"AR", "AR", 1},
	{"AT", "AT", 1},
	{"AU", "AU", 2},
	{"BE", "BE", 1},
	{"BG", "BG", 1},
	{"BN", "BN", 1},
	{"CA", "CA", 2},
	{"CH", "CH", 1},
	{"CY", "CY", 1},
	{"CZ", "CZ", 1},
	{"DE", "DE", 3},
	{"DK", "DK", 1},
	{"EE", "EE", 1},
	{"ES", "ES", 1},
	{"FI", "FI", 1},
	{"FR", "FR", 1},
	{"GB", "GB", 1},
	{"GR", "GR", 1},
	{"HR", "HR", 1},
	{"HU", "HU", 1},
	{"IE", "IE", 1},
	{"IS", "IS", 1},
	{"IT", "IT", 1},
	{"JP", "JP", 5},
	{"KR", "KR", 24},
	{"KW", "KW", 1},
	{"LI", "LI", 1},
	{"LT", "LT", 1},
	{"LU", "LU", 1},
	{"LV", "LV", 1},
	{"MT", "MT", 1},
	{"NL", "NL", 1},
	{"NO", "NO", 1},
	{"PL", "PL", 1},
	{"PT", "PT", 1},
	{"PY", "PY", 1},
	{"RO", "RO", 1},
	{"RU", "RU", 13},
	{"SE", "SE", 1},
	{"SI", "SI", 1},
	{"SK", "SK", 1},
	{"TW", "TW", 2},
	/* 4334/43241 */
	{"",   "XZ", 11},	/* Universal if Country code is unknown or empty */
	{"IR", "XZ", 11},	/* Universal if Country code is IRAN, (ISLAMIC REPUBLIC OF) */
	{"SD", "XZ", 11},	/* Universal if Country code is SUDAN */
	{"SY", "XZ", 11},	/* Universal if Country code is SYRIAN ARAB REPUBLIC */
	{"GL", "XZ", 11},	/* Universal if Country code is GREENLAND */
	{"PS", "XZ", 11},	/* Universal if Country code is PALESTINIAN TERRITORY, OCCUPIED */
	{"TL", "XZ", 11},	/* Universal if Country code is TIMOR-LESTE (EAST TIMOR) */
	{"MH", "XZ", 11},	/* Universal if Country code is MARSHALL ISLANDS */
	{"SG", "SG", 4},
	{"US", "US", 46},
	{"UA", "UA", 8},
	{"CO", "CO", 4},
	{"ID", "ID", 1},
	{"LA", "LA", 1},
	{"LB", "LB", 2},
	{"VN", "VN", 4},
	{"MA", "MA", 1},
	{"TR", "TR", 7},
	{"AE", "AE", 1},
	{"MX", "MX", 1},
};

static void *bcm_wifi_get_country_code(char *ccode)
{
	int size, i;
	static struct cntry_locales_custom country_code;

	size = ARRAY_SIZE(bcm_wifi_translate_custom_table);

	if ((size == 0) || (ccode == NULL))
		return NULL;

	for (i = 0; i < size; i++) {
		if (strcmp(ccode, bcm_wifi_translate_custom_table[i].iso_abbrev) == 0)
			return (void *)&bcm_wifi_translate_custom_table[i];
	}

	memset(&country_code, 0, sizeof(struct cntry_locales_custom));
	strlcpy(country_code.custom_locale, ccode, COUNTRY_BUF_SZ);

	return (void *)&country_code;
}

static struct wifi_platform_data bcm_wifi_control = {
#ifdef CONFIG_BROADCOM_WIFI_RESERVED_MEM
	.mem_prealloc	= brcm_wlan_mem_prealloc,
#endif /* CONFIG_BROADCOM_WIFI_RESERVED_MEM */
	.set_power	= bcm_wifi_set_power,
	.set_reset      = bcm_wifi_reset,
	.set_carddetect = bcm_wifi_carddetect,
	.get_mac_addr   = bcm_wifi_get_mac_addr,
	.get_country_code = bcm_wifi_get_country_code, 
};

static struct resource wifi_resource[] = {
	[0] = {
		.name = "bcmdhd_wlan_irq",
		.start = 0,  /* assigned later */
		.end   = 0,  /* assigned later */
		/* .flags = IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHLEVEL | IORESOURCE_IRQ_SHAREABLE, // for HW_OOB */
		.flags = IORESOURCE_IRQ | IORESOURCE_IRQ_HIGHEDGE | IORESOURCE_IRQ_SHAREABLE, /* for SW_OOB */
	},
};

static struct platform_device bcm_wifi_device = {
	.name           = "bcmdhd_wlan",
	.id             = 1,
	.num_resources  = ARRAY_SIZE(wifi_resource),
	.resource       = wifi_resource,
	.dev            = {
		.platform_data = &bcm_wifi_control,
	},
};

void __init init_bcm_wifi(void)
{
#ifdef CONFIG_WIFI_CONTROL_FUNC

#ifdef LGE_BCM_WIFI_DMA_QOS_CONTROL
	INIT_DELAYED_WORK(&req_dma_work, bcm_wifi_req_dma_work);
	pm_qos_add_request(&wifi_dma_qos, PM_QOS_CPU_DMA_LATENCY, PM_QOS_DEFAULT_VALUE);
	wifi_dma_state = 1; /* INIT */
	printk("%s: wifi_dma_qos is added\n", __func__);
#endif

	bcm_wifi_init_gpio_mem(&bcm_wifi_device);
	platform_device_register(&bcm_wifi_device);
#endif
}

