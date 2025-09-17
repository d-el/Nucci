/*!****************************************************************************
 * @file		settingTSK.c
 * @author		d_el - Storozhenko Roman
 * @version		V1.1
 * @date		01.01.2016
 * @copyright	The MIT License (MIT). Copyright (c) 2020 Storozhenko Roman
 * @brief		This task create start screen
 */

/*!****************************************************************************
 * Include
 */
#include <FreeRTOS.h>
#include <task.h>
#include <rtc.h>
#include <enco.h>
#include <beep.h>
#include <lwip/netif.h>
#include <ledpwm.h>
#include <systemTSK.h>
#include "settingTSK.h"
#include "menuSystem.h"

using namespace Menu;

/*!****************************************************************************
 * @brief    set LCD Bright callback
 */
ItemState setBright(const MenuItem* m){
	(void)m;
	setLcdBrightness(Prm::brightness.val * 10);
	return ItemState{ true, "" };
}

/*!****************************************************************************
 * @brief    set Beep Volume callback
 */
ItemState setBeepVol(const MenuItem* m){
	(void)m;
	beep_enable(Prm::beepOnOff.val);
	return ItemState{ true, "" };
}

/*!****************************************************************************
 * @brief    set LCD Bright callback
 */
ItemState updateDST(const MenuItem* m){
	(void)m;
	timezoneUpdate(Prm::timezone.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    Calibrate date unselect callback
 */
ItemState calibrDateUnselect(const MenuItem* m){
	(void)m;

	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    RTC select callback
 */
ItemState rtcSelect(const MenuItem* m){
	(void)m;
	Prm::utcTime.val = time(NULL);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    RTC select callback
 */
ItemState rtcUnselect(const MenuItem* m){
	(void)m;
	rtc_setTimeUnix(Prm::utcTime.val);
	return ItemState { true, "" };
}

/*!****************************************************************************
 * @brief    NET pfUnselect
 */
ItemState netUpdate(const MenuItem* m){
	(void)m;
	netSettingUpdate();
	return ItemState { true, "" };
}

extern const MenuItem
m1,
m2,
	m20,
	m21,
	m22,
	m23,
		m230,
		m231,
		m232,
		m233,
		m234,
		m235,
	m24,
		m240,
		m241,
		m242,
		m243,
		m244,
		m245,
m3,
	m30,
	m31,
	m32,
	m33,
m4,
m5,
m6;

const MenuItem
m1("Unit", &Prm::rad_unit, true, 0, nullptr, nullptr, nullptr, nullptr, &m2, nullptr, nullptr),

m2("Date&Time", nullptr, true, 0, nullptr, nullptr, nullptr, nullptr, &m3, &m1, &m20),
	m20("Clock", &Prm::utcTime, true, 0, nullptr, rtcSelect, rtcUnselect, nullptr, &m21, nullptr, nullptr, clockEditor),
	m21("Time Zone", &Prm::timezone, true, 0, nullptr, nullptr, nullptr, nullptr, &m22, &m20),
	m22("DST", &Prm::dstOnOff, true, 0, nullptr, nullptr, nullptr, nullptr, &m23, &m21),
	m23("Start", nullptr, true, 0, nullptr, nullptr, updateDST, nullptr, &m24, &m22, &m230),
		m230("Mon", &Prm::DSTSMon, true, 0, nullptr, nullptr, nullptr, nullptr, &m231, nullptr),
		m231("Week", &Prm::DSTSWeek, true, 0, nullptr, nullptr, nullptr, nullptr, &m232, &m230),
		m232("Day", &Prm::DSTSDay, true, 0, nullptr, nullptr, nullptr, nullptr, &m233, &m231),
		m233("Hour", &Prm::DSTSHour, true, 0, nullptr, nullptr, nullptr, nullptr, &m234, &m232),
		m234("Min", &Prm::DSTSMin, true, 0, nullptr, nullptr, nullptr, nullptr, &m235, &m233),
		m235("Sec", &Prm::DSTSSec, true, 0, nullptr, nullptr, nullptr, nullptr, nullptr, &m234),
	m24("End", nullptr, true, 0, nullptr, nullptr, updateDST, nullptr, nullptr, &m23, &m240),
		m240("Mon", &Prm::DSTEMon, true, 0, nullptr, nullptr, nullptr, nullptr, &m241, nullptr),
		m241("Week", &Prm::DSTEWeek, true, 0, nullptr, nullptr, nullptr, nullptr, &m242, &m240),
		m242("Day", &Prm::DSTEDay, true, 0, nullptr, nullptr, nullptr, nullptr, &m243, &m241),
		m243("Hour", &Prm::DSTEHour, true, 0, nullptr, nullptr, nullptr, nullptr, &m244, &m242),
		m244("Min", &Prm::DSTEMin, true, 0, nullptr, nullptr, nullptr, nullptr, &m245, &m243),
		m245("Sec", &Prm::DSTESec, true, 0, nullptr, nullptr, nullptr, nullptr, nullptr, &m244),

m3("LAN", nullptr, true, 0, nullptr, nullptr, netUpdate, nullptr, &m4, &m2, &m30),
	m30("IP address", &Prm::ipadr, true, 0, nullptr, nullptr, nullptr, nullptr, &m31, nullptr, nullptr, ipAddressEditor),
	m31("subnet mask", &Prm::netmask, true, 0, nullptr, nullptr, nullptr, nullptr, &m32, &m30, nullptr, ipAddressEditor),
	m32("gateway", &Prm::gateway, true, 0, nullptr, nullptr, nullptr, nullptr, &m33, &m31, nullptr, ipAddressEditor),
	m33("MAC", &Prm::mac0, false, 0, nullptr, nullptr, nullptr, nullptr, nullptr, &m32, nullptr, ipMacEditor),

m4("Bright", &Prm::brightness, true, 0, setBright, nullptr, nullptr, nullptr, &m5, &m3),
m5("BrightTime", &Prm::brightnessTime, true, 0, nullptr, nullptr, nullptr, nullptr, &m6, &m4),
m6("BeepVol", &Prm::beepOnOff, true, 0, setBeepVol, nullptr, nullptr, nullptr, nullptr, &m5);

/*!****************************************************************************
 * @brief    Setting system task
 */
void settingTSK(void *pPrm){
	(void)pPrm;
	run(&m1);
	saveparametersSystem();
	selWindow(baseWindow);
	vTaskSuspend(NULL);
}

/******************************** END OF FILE ********************************/
