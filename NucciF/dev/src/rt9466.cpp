/*!****************************************************************************
 * @file		rt9466.cpp
 * @author		d_el
 * @version		V1.0
 * @date		Oct 3, 2025
 * @copyright	License (MIT). Copyright (c) 2024 Storozhenko Roman
 * @brief
 */

/*!****************************************************************************
 * Include
 */
#include "rt9466.h"
#include <plog.h>

#define LOG_LOCAL_LEVEL P_LOG_INFO
static const char *logTag = "rt9466";

#define CONFIG_CHARGER_RT9466

/* Registers for rt9466, rt9467 */
#if defined(CONFIG_CHARGER_RT9466) || defined(CONFIG_CHARGER_RT9467)
#define RT946X_REG_CORECTRL0		0x00
#define RT946X_REG_CORECTRL_RST		RT946X_REG_CORECTRL0
#define RT946X_REG_CHGCTRL1		0x01
#define RT946X_REG_CHGCTRL2		0x02
#define RT946X_REG_CHGCTRL3		0x03
#define RT946X_REG_CHGCTRL4		0x04
#define RT946X_REG_CHGCTRL5		0x05
#define RT946X_REG_CHGCTRL6		0x06
#define RT946X_REG_CHGCTRL7		0x07
#define RT946X_REG_CHGCTRL8		0x08
#define RT946X_REG_CHGCTRL9		0x09
#define RT946X_REG_CHGCTRL10		0x0A
#define RT946X_REG_CHGCTRL11		0x0B
#define RT946X_REG_CHGCTRL12		0x0C
#define RT946X_REG_CHGCTRL13		0x0D
#define RT946X_REG_CHGCTRL14		0x0E
#define RT946X_REG_CHGCTRL15		0x0F
#define RT946X_REG_CHGCTRL16		0x10
#define RT946X_REG_CHGADC		0x11
#ifdef CONFIG_CHARGER_RT9467
#define RT946X_REG_DPDM1		0x12
#define RT946X_REG_DPDM2		0x13
#define RT946X_REG_DPDM3		0x14
#endif
#define RT946X_REG_CHGCTRL19		0x18
#define RT946X_REG_CHGCTRL17		0x19
#define RT946X_REG_CHGCTRL18		0x1A
#define RT946X_REG_CHGHIDDENCTRL2	0x21
#define RT946X_REG_CHGHIDDENCTRL4	0x23
#define RT946X_REG_CHGHIDDENCTRL6	0x25
#define RT946X_REG_CHGHIDDENCTRL7	0x26
#define RT946X_REG_CHGHIDDENCTRL8	0x27
#define RT946X_REG_CHGHIDDENCTRL9	0x28
#define RT946X_REG_CHGHIDDENCTRL15	0x2E
#define RT946X_REG_DEVICEID		0x40
#define RT946X_REG_CHGSTAT		0x42
#define RT946X_REG_CHGNTC		0x43
#define RT946X_REG_ADCDATAH		0x44
#define RT946X_REG_ADCDATAL		0x45
#define RT946X_REG_CHGSTATC		0x50
#define RT946X_REG_CHGFAULT		0x51
#define RT946X_REG_TSSTATC		0x52
#define RT946X_REG_CHGIRQ1		0x53
#define RT946X_REG_CHGIRQ2		0x54
#define RT946X_REG_CHGIRQ3		0x55
#ifdef CONFIG_CHARGER_RT9467
#define RT946X_REG_DPDMIRQ		0x56
#endif
#define RT946X_REG_CHGSTATCCTRL		0x60
#define RT946X_REG_CHGFAULTCTRL		0x61
#define RT946X_REG_TSSTATCCTRL		0x62
#define RT946X_REG_CHGIRQ1CTRL		0x63
#define RT946X_REG_CHGIRQ2CTRL		0x64
#define RT946X_REG_CHGIRQ3CTRL		0x65
#ifdef CONFIG_CHARGER_RT9467
#define RT946X_REG_DPDMIRQCTRL		0x66
#endif
#elif defined(CONFIG_CHARGER_MT6370)
/* Registers for mt6370 */
#define RT946X_REG_DEVICEID		0x00
#define RT946X_REG_CORECTRL1		0x01
#define RT946X_REG_CORECTRL2		0x02
#define RT946X_REG_CORECTRL_RST		RT946X_REG_CORECTRL2
#define MT6370_REG_RSTPASCODE1		0x03
#define MT6370_REG_RSTPASCODE2		0x04
#define MT6370_REG_HIDDENPASCODE1	0x07
#define MT6370_REG_HIDDENPASCODE2	0x08
#define MT6370_REG_HIDDENPASCODE3	0x09
#define MT6370_REG_HIDDENPASCODE4	0x0A
#define MT6370_REG_IRQIND		0x0B
#define MT6370_REG_IRQMASK		0x0C
#define MT6370_REG_OSCCTRL		0x10
#define RT946X_REG_CHGCTRL1		0x11
#define RT946X_REG_CHGCTRL2		0x12
#define RT946X_REG_CHGCTRL3		0x13
#define RT946X_REG_CHGCTRL4		0x14
#define RT946X_REG_CHGCTRL5		0x15
#define RT946X_REG_CHGCTRL6		0x16
#define RT946X_REG_CHGCTRL7		0x17
#define RT946X_REG_CHGCTRL8		0x18
#define RT946X_REG_CHGCTRL9		0x19
#define RT946X_REG_CHGCTRL10		0x1A
#define RT946X_REG_CHGCTRL11		0x1B
#define RT946X_REG_CHGCTRL12		0x1C
#define RT946X_REG_CHGCTRL13		0x1D
#define RT946X_REG_CHGCTRL14		0x1E
#define RT946X_REG_CHGCTRL15		0x1F
#define RT946X_REG_CHGCTRL16		0x20
#define RT946X_REG_CHGADC		0x21
#define MT6370_REG_DEVICETYPE		0x22
#define RT946X_REG_DPDM1		MT6370_REG_DEVICETYPE
#define MT6370_REG_USBSTATUS1		0x27
#define MT6370_REG_QCSTATUS2		0x29
#define RT946X_REG_CHGCTRL17		0X2B
#define RT946X_REG_CHGCTRL18		0X2C
#define RT946X_REG_CHGHIDDENCTRL7	0x36
#define MT6370_REG_CHGHIDDENCTRL15	0x3E
#define RT946X_REG_CHGSTAT		0X4A
#define RT946X_REG_CHGNTC		0X4B
#define RT946X_REG_ADCDATAH		0X4C
#define RT946X_REG_ADCDATAL		0X4D
/* FLED */
#define MT6370_REG_FLEDEN		0x7E
/* LDO */
#define MT6370_REG_LDOCFG		0X80
#define MT6370_REG_LDOVOUT		0X81
/* RGB led */
#define MT6370_REG_RGBDIM_BASE		0x81
#define MT6370_REG_RGB1DIM		0x82
#define MT6370_REG_RGB2DIM		0x83
#define MT6370_REG_RGB3DIM		0x84
#define MT6370_REG_RGBEN		0x85
#define MT6370_REG_RGBISNK_BASE		0x85
#define MT6370_REG_RGB1ISNK		0x86
#define MT6370_REG_RGB2ISNK		0x87
#define MT6370_REG_RGB3ISNK		0x88
#define MT6370_REG_RGBCHRINDDIM		0x92
#define MT6370_REG_RGBCHRINDCTRL	0x93
/* backlight */
#define MT6370_BACKLIGHT_BLEN		0xA0
#define MT6370_BACKLIGHT_BLPWM		0xA2
#define MT6370_BACKLIGHT_BLDIM2		0xA4
#define MT6370_BACKLIGHT_BLDIM		0xA5
/* Display bias */
#define MT6370_REG_DBCTRL1		0XB0
#define MT6370_REG_DBCTRL2		0XB1
#define MT6370_REG_DBVBST		0XB2
#define MT6370_REG_DBVPOS		0XB3
#define MT6370_REG_DBVNEG		0XB4
#define MT6370_REG_CHGIRQ1		0xC0
#define RT946X_REG_DPDMIRQ		0xC6
/* status event */
#define MT6370_REG_CHGSTAT1		0xD0
#define RT946X_REG_CHGSTATC		MT6370_REG_CHGSTAT1
#define MT6370_REG_CHGSTAT2		0xD1
#define RT946X_REG_CHGFAULT		MT6370_REG_CHGSTAT2
#define MT6370_REG_CHGSTAT3		0xD2
#define MT6370_REG_CHGSTAT4		0xD3
#define MT6370_REG_CHGSTAT5		0xD4
#define MT6370_REG_CHGSTAT6		0xD5
#define MT6370_REG_DPDMSTAT		0xD6
#define MT6370_REG_DICHGSTAT		0xD7
#define MT6370_REG_OVPCTRLSTAT		0xD8
#define MT6370_REG_FLEDSTAT1		0xD9
#define MT6370_REG_FLEDSTAT2		0xDA
#define MT6370_REG_BASESTAT		0xDB
#define MT6370_REG_LDOSTAT		0xDC
#define MT6370_REG_RGBSTAT		0xDD
#define MT6370_REG_BLSTAT		0xDE
#define MT6370_REG_DBSTAT		0xDF
/* irq mask */
#define MT6370_REG_CHGMASK1		0xE0
#define RT946X_REG_CHGSTATCCTRL		MT6370_REG_CHGMASK1
#define MT6370_REG_CHGMASK2		0xE1
#define MT6370_REG_CHGMASK3		0xE2
#define MT6370_REG_CHGMASK4		0xE3
#define MT6370_REG_CHGMASK5		0xE4
#define MT6370_REG_CHGMASK6		0xE5
#define MT6370_REG_DPDMMASK1		0xE6
#define MT6370_REG_DICHGMASK		0xE7
#define MT6370_REG_OVPCTRLMASK		0xE8
#define MT6370_REG_FLEDMASK1		0xE9
#define MT6370_REG_FLEDMASK2		0xEA
#define MT6370_REG_BASEMASK		0xEB
#define MT6370_REG_LDOMASK		0xEC
#define MT6370_REG_RGBMASK		0xED
#define MT6370_REG_BLMASK		0xEE
#define MT6370_REG_DBMASK		0xEF
#define MT6370_REG_TM_PAS_CODE1		0xF0
#define MT6370_REG_BANK			0xFF
/* TM REGISTER */
#define MT6370_TM_REG_BL3		0x34
#define MT6370_TM_REG_DSV1		0x37
#else
#error "No suitable charger option defined"
#endif

/* EOC current */
#define RT946X_IEOC_MIN		100
#define RT946X_IEOC_MAX		850
#define RT946X_IEOC_STEP	50
/* Minimum Input Voltage Regulator */
#define RT946X_MIVR_MIN		3900
#define RT946X_MIVR_MAX		13400
#define RT946X_MIVR_STEP	100
/* Boost voltage */
#define RT946X_BOOST_VOLTAGE_MIN	4425
#define RT946X_BOOST_VOLTAGE_MAX	5825
#define RT946X_BOOST_VOLTAGE_STEP	25
/* IR compensation resistor */
#define RT946X_IRCMP_RES_MIN	0
#define RT946X_IRCMP_RES_MAX	175
#define RT946X_IRCMP_RES_STEP	25
/* IR compensation voltage clamp */
#define RT946X_IRCMP_VCLAMP_MIN		0
#define RT946X_IRCMP_VCLAMP_MAX		224
#define RT946X_IRCMP_VCLAMP_STEP	32
/* Pre-charge mode threshold voltage */
#define RT946X_VPREC_MIN		2000
#define RT946X_VPREC_MAX		3500
#define RT946X_VPREC_STEP		100
/* Pre-charge current */
#define RT946X_IPREC_MIN		100
#define RT946X_IPREC_MAX		850
#define RT946X_IPREC_STEP		50
/* AICLVTH */
#define RT946X_AICLVTH_MIN	4100
#define RT946X_AICLVTH_MAX	4800
#define RT946X_AICLVTH_STEP	100
/* NTC */
#define RT946X_BATTEMP_NORMAL	0x00
#define RT946X_BATTEMP_WARM	0x02
#define RT946X_BATTEMP_COOL	0x03
#define RT946X_BATTEMP_COLD	0x05
#define RT946X_BATTEMP_HOT	0x06
/* LDO voltage */
#define MT6370_LDO_MIN		1600
#define MT6370_LDO_MAX		4000
#define MT6370_LDO_STEP		200
/* ========== CORECTRL0 0x00 ============ */
#define RT946X_SHIFT_RST	7
#define RT946X_SHIFT_CHG_RST	6
#define RT946X_SHIFT_FLED_RST	5
#define RT946X_SHIFT_LDO_RST	4
#define RT946X_SHIFT_RGB_RST	3
#define RT946X_SHIFT_BL_RST	2
#define RT946X_SHIFT_DB_RST	1
#define RT946X_SHIFT_REG_RST	0
#define RT946X_MASK_RST		BIT(RT946X_SHIFT_RST)
#define RT946X_MASK_CHG_RST	BIT(RT946X_SHIFT_CHG_RST)
#define RT946X_MASK_FLED_RST	BIT(RT946X_SHIFT_FLED_RST)
#define RT946X_MASK_LDO_RST	BIT(RT946X_SHIFT_LDO_RST)
#define RT946X_MASK_RGB_RST	BIT(RT946X_SHIFT_RGB_RST)
#define RT946X_MASK_BL_RST	BIT(RT946X_SHIFT_BL_RST)
#define RT946X_MASK_DB_RST	BIT(RT946X_SHIFT_DB_RST)
#define RT946X_MASK_REG_RST	BIT(RT946X_SHIFT_REG_RST)
#define RT946X_MASK_SOFT_RST                                                   \
	(RT946X_MASK_CHG_RST | RT946X_MASK_FLED_RST | RT946X_MASK_LDO_RST |    \
	 RT946X_MASK_RGB_RST | RT946X_MASK_BL_RST | RT946X_MASK_DB_RST |       \
	 RT946X_MASK_REG_RST)
/* ========== CHGCTRL1 0x01 ============ */
#define RT946X_SHIFT_OPA_MODE   0
#define RT946X_SHIFT_HZ_EN      2
#define RT946X_SHIFT_STAT_EN	4
#define RT946X_MASK_OPA_MODE	BIT(RT946X_SHIFT_OPA_MODE)
#define RT946X_MASK_HZ_EN	BIT(RT946X_SHIFT_HZ_EN)
#define RT946X_MASK_STAT_EN	BIT(RT946X_SHIFT_STAT_EN)
/* ========== CHGCTRL2 0x02 ============ */
#define RT946X_SHIFT_SHIP_MODE	7
#define RT946X_SHIFT_TE		4
#define RT946X_SHIFT_ILMTSEL	2
#define RT946X_SHIFT_CFO_EN	1
#define RT946X_SHIFT_CHG_EN	0
#define RT946X_MASK_SHIP_MODE	BIT(RT946X_SHIFT_SHIP_MODE)
#define RT946X_MASK_TE		BIT(RT946X_SHIFT_TE)
#define RT946X_MASK_ILMTSEL	(0x3 << RT946X_SHIFT_ILMTSEL)
#define RT946X_MASK_CFO_EN	BIT(RT946X_SHIFT_CFO_EN)
#define RT946X_MASK_CHG_EN	BIT(RT946X_SHIFT_CHG_EN)
/* ========== RSTPASCODE1 0x03 (mt6370) ============ */
#define MT6370_MASK_RSTPASCODE1	0xA9
/* ========== CHGCTRL3 0x03 ============ */
#define RT946X_SHIFT_AICR	2
#define RT946X_SHIFT_ILIMEN	0
#define RT946X_MASK_AICR	(0x3F << RT946X_SHIFT_AICR)
#define RT946X_MASK_ILIMEN	BIT(RT946X_SHIFT_ILIMEN)
/*
 * The accuracy of AICR is 7%.  So if AICR = 2150,
 * then Max=2150, Typ=2000, Min=1860. And plus 25 since the AICR
 * is 50ma a step.
 */
#define RT946X_AICR_TYP2MAX(x)	((x) * 107 / 100 + 25)
/* ========== RSTPASCODE2 0x04 (mt6370) ============ */
#define MT6370_MASK_RSTPASCODE2	0x96
/* ========== CHGCTRL4 0x04 ============ */
#define RT946X_SHIFT_CV	1
#define RT946X_MASK_CV	0xFE
/* ========== CHGCTRL5 0x05 ============ */
#define RT946X_SHIFT_BOOST_VOLTAGE	2
#define RT946X_MASK_BOOST_VOLTAGE	0xFC
/* ========== CHGCTRL6 0x06 ============ */
#define RT946X_SHIFT_MIVR	1
#define RT946X_MASK_MIVR	(0x7F << RT946X_SHIFT_MIVR)
/* ========== CHGCTRL7 0x07 ============ */
#define RT946X_SHIFT_ICHG	2
#define RT946X_MASK_ICHG	(0x3F << RT946X_SHIFT_ICHG)
/* ========== CHGCTRL8 0x08 ============ */
#define RT946X_SHIFT_VPREC	4
#define RT946X_SHIFT_IPREC	0
#define RT946X_MASK_VPREC	(0xF << RT946X_SHIFT_VPREC)
#define RT946X_MASK_IPREC	(0xF << RT946X_SHIFT_IPREC)
/* ========== CHGCTRL9 0x09 ============ */
#define RT946X_SHIFT_EOC	3
#define RT946X_SHIFT_IEOC	4
#define RT946X_MASK_EOC		BIT(RT946X_SHIFT_EOC)
#define RT946X_MASK_IEOC	(0xF << RT946X_SHIFT_IEOC)
/* ========== CHGCTRL10 0x0A ============ */
#define RT946X_SHIFT_BOOST_CURRENT	0
#define RT946X_MASK_BOOST_CURRENT	0x07
/* ========== CHGCTRL12 0x0C ============ */
#define RT946X_SHIFT_TMR_EN	1
#define MT6370_IRQ_MASK_ALL	0xFE
#define RT946X_MASK_TMR_EN	BIT(RT946X_SHIFT_TMR_EN)
/* ========== CHGCTRL13 0x0D ============ */
#define RT946X_SHIFT_WDT_EN	7
#define RT946X_MASK_WDT_EN	BIT(RT946X_SHIFT_WDT_EN)
/* ========== CHGCTRL14 0x0E ============ */
#define RT946X_SHIFT_AICLMEAS	7
#define RT946X_SHIFT_AICLVTH	0
#define RT946X_MASK_AICLMEAS	BIT(RT946X_SHIFT_AICLMEAS)
#define RT946X_MASK_AICLVTH	0x07
/* ========== CHGCTRL16 0x10 ============ */
#define RT946X_SHIFT_JEITA_EN	4
#define RT946X_MASK_JEITA_EN	BIT(RT946X_SHIFT_JEITA_EN)
/* ========== CHGADC 0x11 ============ */
#define RT946X_SHIFT_ADC_IN_SEL	4
#define RT946X_SHIFT_ADC_START	0
#define RT946X_MASK_ADC_IN_SEL	(0xF << RT946X_SHIFT_ADC_IN_SEL)
#define RT946X_MASK_ADC_START	BIT(RT946X_SHIFT_ADC_START)
/* ========== CHGDPDM1 0x12 (rt946x) DEVICETYPE 0x22 (mt6370) ============ */
#define RT946X_SHIFT_USBCHGEN	7
#define RT946X_SHIFT_DCDTIMEOUT	6
#define RT946X_SHIFT_DCP	2
#define RT946X_SHIFT_CDP	1
#define RT946X_SHIFT_SDP	0
#define RT946X_MASK_USBCHGEN	BIT(RT946X_SHIFT_USBCHGEN)
#define RT946X_MASK_DCDTIMEOUT	BIT(RT946X_SHIFT_DCDTIMEOUT)
#define RT946X_MASK_DCP		BIT(RT946X_SHIFT_DCP)
#define RT946X_MASK_CDP		BIT(RT946X_SHIFT_CDP)
#define RT946X_MASK_SDP		BIT(RT946X_SHIFT_SDP)
#define RT946X_MASK_BC12_TYPE	(RT946X_MASK_DCP | \
				 RT946X_MASK_CDP | \
				 RT946X_MASK_SDP)
/* ========== USBSTATUS1 0x27 (mt6370) ============ */
#define MT6370_SHIFT_DCD_TIMEOUT 	2
#define MT6370_SHIFT_USB_STATUS		4
#define MT6370_MASK_USB_STATUS	0x70
#define MT6370_CHG_TYPE_NOVBUS			0
#define MT6370_CHG_TYPE_BUSY			1
#define MT6370_CHG_TYPE_SDP			2
#define MT6370_CHG_TYPE_SDPNSTD			3
#define MT6370_CHG_TYPE_DCP			4
#define MT6370_CHG_TYPE_CDP			5
#define MT6370_CHG_TYPE_SAMSUNG_CHARGER		6
#define MT6370_CHG_TYPE_APPLE_0_5A_CHARGER	7
#define MT6370_CHG_TYPE_APPLE_1_0A_CHARGER	8
#define MT6370_CHG_TYPE_APPLE_2_1A_CHARGER	9
#define MT6370_CHG_TYPE_APPLE_2_4A_CHARGER	10
#define MT6370_MASK_DCD_TIMEOUT		BIT(MT6370_SHIFT_DCD_TIMEOUT)
/* ========== QCSTATUS2 0x29 (mt6370) ============ */
#define MT6370_SHIFT_APP_OUT		5
#define MT6370_SHIFT_SS_OUT		4
#define MT6370_SHIFT_APP_REF		3
#define MT6370_SHIFT_APP_DPDM_IN	2
#define MT6370_SHIFT_APP_SS_PL		1
#define MT6370_SHIFT_APP_SS_EN		0
#define MT6370_MASK_APP_OUT	BIT(MT6370_SHIFT_APP_OUT)
#define MT6370_MASK_SS_OUT	BIT(MT6370_SHIFT_SS_OUT)
#define MT6370_MASK_APP_REF	BIT(MT6370_SHIFT_APP_REF)
#define MT6370_MASK_APP_DPDM_IN	BIT(MT6370_SHIFT_APP_DPDM_IN)
#define MT6370_MASK_APP_SS_PL	BIT(MT6370_SHIFT_APP_SS_PL)
#define MT6370_MASK_APP_SS_EN	BIT(MT6370_SHIFT_APP_SS_EN)
#define MT6360_MASK_CHECK_DPDM	(MT6370_MASK_APP_SS_EN | \
				MT6370_MASK_APP_SS_PL | \
				MT6370_MASK_APP_DPDM_IN | \
				MT6370_MASK_APP_REF)
/* ========= CHGHIDDENCTRL7 0x36 (mt6370) ======== */
#define RT946X_ENABLE_VSYS_PROTECT		0x40
#define RT946X_SHIFT_HIDDENCTRL7_VSYS_PROTECT	5
#define RT946X_MASK_HIDDENCTRL7_VSYS_PROTECT \
	(0x3 << RT946X_SHIFT_HIDDENCTRL7_VSYS_PROTECT)
/* ========== CHGCTRL18 0x1A ============ */
#define RT946X_SHIFT_IRCMP_RES		3
#define RT946X_SHIFT_IRCMP_VCLAMP	0
#define RT946X_MASK_IRCMP_RES		(0x7 << RT946X_SHIFT_IRCMP_RES)
#define RT946X_MASK_IRCMP_VCLAMP	(0x7 << RT946X_SHIFT_IRCMP_VCLAMP)
/* ========== HIDDEN CTRL15 0x3E ============ */
#define MT6370_SHIFT_ADC_TS_AUTO	0
#define MT6370_MASK_ADC_TS_AUTO		BIT(MT6370_SHIFT_ADC_TS_AUTO)
/* ========== DEVICE_ID 0x40 ============ */
#define RT946X_MASK_VENDOR_ID	0xF0
#define RT946X_MASK_CHIP_REV	0x0F
/* ========== CHGSTAT 0x42 ============ */
#define RT946X_SHIFT_CHG_STAT	6
#define RT946X_SHIFT_ADC_STAT	0
#define RT946X_MASK_CHG_STAT	(0x3 << RT946X_SHIFT_CHG_STAT)
#define RT946X_MASK_ADC_STAT	BIT(RT946X_SHIFT_ADC_STAT)
/* ========== CHGNTC 0x43 ============ */
#define RT946X_SHIFT_BATNTC_FAULT	4
#define RT946X_MASK_BATNTC_FAULT	0x70
/* ========== CHGSTATC 0x50 (rt946x) ============ */
#define RT946X_SHIFT_PWR_RDY    7
#define RT946X_MASK_PWR_RDY     BIT(RT946X_SHIFT_PWR_RDY)
/* ========== CHGFAULT 0x51 (rt946x) ============ */
#if defined(CONFIG_CHARGER_RT9466) || defined(CONFIG_CHARGER_RT9467)
#define RT946X_SHIFT_CHG_VSYSUV	4
#define RT946X_SHIFT_CHG_VSYSOV	5
#define RT946X_SHIFT_CHG_VBATOV	6
#define RT946X_SHIFT_CHG_VBUSOV	7
#define RT946X_MASK_CHG_VSYSUV	BIT(RT946X_SHIFT_CHG_VSYSUV)
#define RT946X_MASK_CHG_VSYSOV	BIT(RT946X_SHIFT_CHG_VSYSOV)
#define RT946X_MASK_CHG_VBATOV	BIT(RT946X_SHIFT_CHG_VBATOV)
#define RT946X_MASK_CHG_VBUSOV	BIT(RT946X_SHIFT_CHG_VBUSOV)
#endif

/* ========== Variant-specific configuration ============ */
#if defined(CONFIG_CHARGER_RT9466)
	#define RT946X_CHARGER_NAME	"rt9466"
	#define RT946X_VENDOR_ID	0x80
	#define RT946X_ADDR_FLAGS	0x53
#elif defined(CONFIG_CHARGER_RT9467)
	#define RT946X_CHARGER_NAME	"rt9467"
	#define RT946X_VENDOR_ID	0x90
	#define RT946X_ADDR_FLAGS	0x5B
#elif defined(CONFIG_CHARGER_MT6370)
	#define RT946X_CHARGER_NAME	"mt6370"
	#define RT946X_VENDOR_ID	0xE0
	#define RT946X_ADDR_FLAGS	0x34
#else
	#error "No suitable charger option defined"
#endif

/*!****************************************************************************
 * @brief
 */
Rt9466::Rt9466(i2cRead_t _read, i2cWrite_t _write):
m_read(_read), m_write(_write){}

/*!****************************************************************************
*/
void Rt9466::setI2c(i2cRead_t _read, i2cWrite_t _write){
	m_read = _read;
	m_write = _write;
}

bool Rt9466::read_reg(uint8_t reg, uint8_t* val){
	if(!m_write(0xA6, &reg, 1, false, 200)){
		P_LOGW(logTag, "Error i2c write");
		return false;
	}
	uint8_t rx[2] = {};
	if(!m_read(0xA7, rx, 2, 200)){
		P_LOGW(logTag, "Error i2c read");
		return false;
	}
	*val = rx[0];
	return true;
}

bool Rt9466::write_reg(uint8_t reg, uint8_t val){
	uint8_t tx[2] = {reg, val};
	if(!m_write(0xA7, &tx[0], 2, false, 200)){
		P_LOGW(logTag, "Error i2c write");
		return false;
	}
	return true;
}

bool Rt9466::init(void){
	P_LOGD(logTag, "RT9466 init");
	uint8_t deviceid;
	if(!read_reg(RT946X_REG_DEVICEID, &deviceid)){
		return false;
	}
	uint8_t vendor_id = (deviceid & RT946X_MASK_VENDOR_ID) >> 4;
	uint8_t chip_rev = deviceid & RT946X_MASK_CHIP_REV;
	P_LOGI(logTag, "vendor_id %u, chip_rev %u", vendor_id, chip_rev);
	if(vendor_id != 8){
		P_LOGW(logTag, "Wrong chip ID");
		return false;
	}
	return true;
}

bool Rt9466::get_status(Rt9466::ch_status_t *chgstat){
	if(!read_reg(RT946X_REG_CHGSTAT, (uint8_t*)chgstat)){
		return false;
	}
	P_LOGD(logTag, "CHGSTAT:\n"
					"\t chg_stat %u\n"
					"\t vbat_lvl %u\n"
					"\t vbus_ovp %u\n"
					"\t adc_in_conversion %u",
					chgstat->chg_stat, chgstat->vbat_lvl, chgstat->vbus_ovp, chgstat->adc_in_conversion);
	return true;
}

bool Rt9466::adc_start(Rt9466::adc_ch ch){
	uint8_t val = (ch << 4) | 1;
	if(!write_reg(RT946X_REG_CHGADC, val)){
		return false;
	}
	return true;
}

bool Rt9466::adc_read(uint16_t* val){
	ch_status_t ch_status;
	if(!get_status(&ch_status) || ch_status.adc_in_conversion != 0){
		return false;
	}

	uint8_t adc_h;
	if(!read_reg(RT946X_REG_ADCDATAH, &adc_h)){
		return false;
	}
	uint8_t adc_l;
	if(!read_reg(RT946X_REG_ADCDATAL, &adc_l)){
		return false;
	}
	*val = (adc_h << 8) | adc_l;
	return true;
}

bool Rt9466::batteryRegulationVoltageSet(uint16_t voltage){
	if(voltage < 390 || voltage > 471){
		return false;
	}
	if(!write_reg(RT946X_REG_CHGCTRL4, (voltage - 390) << 1)){
		return false;
	}
	return true;
}

bool Rt9466::chargingRegulationCurrentSet(uint8_t current){
	if(current < 1 || current > 50){
		return false;
	}
	if(!write_reg(RT946X_REG_CHGCTRL7, (current - 1) << 2)){
		return false;
	}
	return true;
}

/******************************** END OF FILE ********************************/
