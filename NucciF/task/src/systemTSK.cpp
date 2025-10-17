/*!****************************************************************************
 * @file		systemTSK.c
 * @author		d_el
 * @version		V1.0
 * @date		04.10.2025
 * @copyright	The MIT License (MIT). Copyright (c) 2025 Storozhenko Roman
 * @brief		System control task
 */

/*!****************************************************************************
 * Include
 */
#include <inttypes.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <plog.h>
#include <prmSystem.h>
#include <sntp.h>
#include <stm32f4x7_eth_bsp.h>
#include <stm32f4x7_eth.h>
#include <ethernetif.h>
#include <lwip/tcpip.h>
#include <lwip/dns.h>
#include <display.h>
#include <beep.h>
#include <rtc.h>
#include <pvd.h>
#include <i2c.h>
#include <ledpwm.h>
#include <board.h>
#include <ui.h>
#include <startupTSK.h>
#include <settingTSK.h>
#include <baseTSK.h>
#include <monitorTSK.h>
#include <24AAxx.h>
#include <rt9466.h>
#include <adcTSK.h>
#include "systemTSK.h"
#include <write.h>
#include "httpServerTSK.h"
#include "radmonClient.h"
#include <modbusServerTSK.h>
#include <specificMath.h>

#include <semphr.h>

/*!****************************************************************************
 * Memory
 */
frontPanel_type fp;					///< Data structure front panel
static TaskHandle_t windowTskHandle;	///< Program task handler
static struct netif xnetif; 			///< Network interface structure
static Eep24AA eep24AA08;
static Rt9466 rt9466;
static SemaphoreHandle_t lowPowerSem;
static TickType_t countTime;
static uint32_t sparkOffset;
static uint16_t cnt[60];
static bool rt9466reconfig = true;

/*!****************************************************************************
 * Local prototypes for the functions
 */
static void loadParameters(void);
static void shutdown(void);
static void LwIP_Init(const uint32_t ipaddr, const uint32_t netmask, const uint32_t gateway);
extern "C" int _write(int fd, const void *buf, size_t count);

/**
 * SYS_DEBUG_LEVEL: Enable debugging for system task
 */
#define TASK_MONITOR_EN	0
#define LOG_LOCAL_LEVEL P_LOG_INFO
static const char *logTag = "SYS";

/*!****************************************************************************
 * @brief
 */
void cntIntHandled(void){
	Prm::odoPulseCount.val++;
	size_t index = (xTaskGetTickCount() / 1000) % 60;
	static size_t indexPrev;
	if(indexPrev != index){
		indexPrev = index;
		cnt[index] = 0;
	}
	cnt[index]++;
	BeepTime(2, 2000);
}

/*!****************************************************************************
 * @brief
 */
void meter_clear(void){
	Prm::countTime.val = 0;
	sparkOffset = Prm::odoPulseCount.val;
	countTime = xTaskGetTickCount();
	for(size_t i = 0; i < sizeof(cnt)/sizeof(cnt[0]); i++){
		cnt[i] = 0;
	}
}

/*!****************************************************************************
 * @brief
 */
uint8_t chargerStatus;
void ch_stat(){
	Rt9466::ch_status_t ch_status;
	if(rt9466.get_status(&ch_status)){
		chargerStatus = ch_status.chg_stat;
	}

	rt9466.adc_start(Rt9466::adc_ch::VBAT);
	vTaskDelay(20);
	uint16_t bat_adc = 0;
	if(rt9466.adc_read(&bat_adc)){
		//const char* ch_stat_str[4] = { "ready", "in_progress", "charge_done", "fault" };
		Prm::ch_vbatMeas.val = (bat_adc * 5) / 10;
		//P_LOGI(logTag, "ch: stat %s, V %umV", ch_stat_str[ch_status.chg_stat], Prm::ch_vbatMeas.val);
	}

	if(rt9466reconfig){
		bool result = rt9466.batteryRegulationVoltageSet(Prm::ch_chvbat.val);
		result = result && rt9466.chargingRegulationCurrentSet(Prm::ch_chibat.val);
		rt9466reconfig = false;
	}
}

/*!****************************************************************************
 * @brief
 */
void setupI2Cdevices(void){
	static SemaphoreHandle_t i2cTcSem;
	vSemaphoreCreateBinary(i2cTcSem);
	xSemaphoreTake(i2cTcSem, portMAX_DELAY);
	assert(i2cTcSem != NULL);

	auto i2cTxHook = [](i2c_type* i2c){
		(void)i2c;
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		xSemaphoreGiveFromISR(i2cTcSem, &xHigherPriorityTaskWoken);
		portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
	};

	auto i2cRead = [](uint8_t devAddr, uint8_t* dst, size_t len, uint16_t timeout){
		//P_LOGI(logTag, "i2c_read %u", len);
		i2c_read(i2c1, dst, len, devAddr);
		return pdTRUE == xSemaphoreTake(i2cTcSem, pdMS_TO_TICKS(timeout));
	};

	auto i2cWrite = [](uint8_t devAddr, const uint8_t* src, size_t len, bool needstop, uint16_t timeout){
		//P_LOGI(logTag, "i2c_write %u", len);
		i2c_write(i2c1, src, len, devAddr, needstop ? i2cNeedStop : i2cWithoutStop);
		return pdTRUE == xSemaphoreTake(i2cTcSem, pdMS_TO_TICKS(timeout));
	};

	i2c_init(i2c1);
	i2c_setCallback(i2c1, i2cTxHook);

	eep24AA08.setI2c(i2cRead, i2cWrite);
	rt9466.setI2c(i2cRead, i2cWrite);

	rt9466.init();
	Rt9466::ch_status_t ch_status;
	rt9466.get_status(&ch_status);
}

/*!****************************************************************************
 * @brief
 */
void systemTSK(void *pPrm){
	(void)pPrm;
	selWindow_type 	selWindowPrev = noneWindow;

	//Init log system
	plog_setWrite(_write);
	plog_setTimestamp([]() -> uint32_t { return xTaskGetTickCount(); });

	P_LOGI(logTag, "\n\nStarted systemTSK");

	setupI2Cdevices();
	loadParameters();												// Load panel settings and user parameters
	timezoneUpdate(Prm::timezone.val);
	disp_init();
	ETH_BSP_Config();			//Configure Ethernet (GPIOs, clocks, MAC, DMA)
	LwIP_Init(Prm::ipadr.val, Prm::netmask.val, Prm::gateway.val);	// Initialize the LwIP stack
	uint64_t mac = 0;
	memcpy(&mac, xnetif.hwaddr, xnetif.hwaddr_len);
	Prm::mac0.val = mac;
	sntp_init();													// Initialize service SNTP

	gppin_initCntInt(cntIntHandled);
	countTime = xTaskGetTickCount();

	lowPowerSem = xSemaphoreCreateBinary();
	assert(lowPowerSem != NULL);

	BaseType_t osres = xTaskCreate(adcTSK, "adcTSK", ADC_TSK_SZ_STACK, NULL, ADC_TSK_PRIO, NULL);
	assert(osres == pdTRUE);
	P_LOGI(logTag, "Started adcTSK");

	osres = xTaskCreate(radmonClientTSK, "radmonTSK", RADMON_TSK_SZ_STACK, NULL, RADMON_TSK_PRIO, NULL);
	assert(osres == pdTRUE);
	P_LOGI(logTag, "Started radmonClientTSK");

	osres = xTaskCreate(httpServerTSK, "httpServerTSK", HTTP_TSK_SZ_STACK, NULL, HTTP_TSK_PRIO, NULL);
	assert(osres == pdTRUE);
	P_LOGI(logTag, "Started httpServerTSK");

	osres = xTaskCreate(modbusServerTSK, "modbusServerTSK", HTTP_TSK_SZ_STACK, NULL, HTTP_TSK_PRIO, NULL);
	assert(osres == pdTRUE);
	P_LOGI(logTag, "Started modbusServerTSK");

#if(TASK_MONITOR_EN > 0)
	osres = xTaskCreate(monitorTSK, "monitorTSK", OSMONITOR_TSK_SZ_STACK, NULL, OSMONITOR_TSK_PRIO, NULL);
	assert(osres == pdTRUE);
#endif
	(void)osres;

	osres = pdTRUE;
	uint32_t odoPulseCountPrev = Prm::odoPulseCount.val;
	selWindow(startupWindow);
	TickType_t batStat_time = 0;
	while(1){
		Prm::pulseCount.val = Prm::odoPulseCount.val - sparkOffset;
		if(Prm::odoPulseCount.val - odoPulseCountPrev > 100){
			odoPulseCountPrev = Prm::odoPulseCount.val;
			saveparametersUser();
		}

		uint32_t cpm = 0;
		for(size_t i = 0; i < sizeof(cnt)/sizeof(cnt[0]); i++){
			cpm += cnt[i];
		}
		Prm::pulseCountpm.val = cpm;
		Prm::radVal_uRph.val = (cpm * 10 * 60/*min in hour*/ * 10) / 3624;
		Prm::radVal_uSvph.val = (cpm * 10 * 60/*min in hour*/) / 3624;

		Prm::rad_uR.val = Prm::pulseCount.val * 100 / 3624;
		Prm::rad_uSv.val = Prm::pulseCount.val * 10 / 3624;

		if(xTaskGetTickCount() - countTime >= 1000){
			countTime += 1000;
			Prm::countTime.val++;
		}

		if(selWindowPrev != fp.currentSelWindow){
			if(windowTskHandle != NULL){
				assert(osres == pdTRUE);	//Fail windowTskHandle
				P_LOGI(logTag, "Stopped %s", pcTaskGetName(windowTskHandle));
				vTaskDelete(windowTskHandle);
			}

			switch(fp.currentSelWindow){
				case noneWindow:
					break;
				case startupWindow:
					osres = xTaskCreate(startupTSK, "startupTSK", STARTUP_TSK_SZ_STACK, NULL, STARTUP_TSK_PRIO, &windowTskHandle);
					break;
				case settingWindow:
					osres = xTaskCreate(settingTSK, "settingTSK", SETT_TSK_SZ_STACK, NULL, SETT_TSK_PRIO, &windowTskHandle);
					break;
				case baseWindow:
					osres = xTaskCreate(baseTSK, "baseTSK", BASE_TSK_SZ_STACK, NULL, BASE_TSK_PRIO, &windowTskHandle);
					break;
				default:
					osres = pdTRUE;
					assert(!"Fail selector");
			}
			assert(osres == pdTRUE);	//Fail windowTskHandle
			P_LOGI(logTag, "Started %s", pcTaskGetName(windowTskHandle));
			selWindowPrev = fp.currentSelWindow;
		}

		/*
		 * Led blink
		 */
		static uint8_t ledCount = 0;
		if(ledCount++ == 200){
			LED_ON();
			ledCount = 0;
		}
		if(ledCount == 20){
			LED_OFF();
		}
		if((ledCount == 40) && false){
			LED_ON();
		}
		if(ledCount == 60){
			LED_OFF();
		}

		static uint32_t linkRequest = 0;
		if(linkRequest != httpServer.numberRequest){
			fp.state.httpactiv = 1;
			linkRequest = httpServer.numberRequest;
		}

		if(batStat_time < xTaskGetTickCount()){
			batStat_time = xTaskGetTickCount() + 200;
			ch_stat();
			adcTaskStct.targetcurrentlsb = s32iq_lerp(0, 0, 390, Prm::vhv_calGain.val, Prm::vhv_set.val);
			Prm::vhv_meas.val = s32iq_lerp(0, 0, Prm::vhv_calGain.val, 390, adcTaskStct.filtered.vhv);
			adcTaskStct.hvEnable = true; //Prm::rad_unit.val != 0;

			if(Prm::ch_vbatMeas.val != 0 && Prm::ch_vbatMeas.val < Prm::vbamMin.val * 10){
				shutdown();
			}
		}

		if(Prm::reboot.val == Prm::mask_reboot::reboot_do){
			vTaskDelay(pdMS_TO_TICKS(2));
			NVIC_SystemReset();
		}

		vTaskDelay(SYSTEM_TSK_PERIOD);
	}
}

/*!****************************************************************************
 * @brief	Load parameters from memory
 */
static void loadParameters(void){
	size_t size = Prm::getSerialSize(Prm::Save::savesys);
	if(size){
		uint8_t buffer[size] = {1,2,3};
		const uint16_t systemSettingsAddress = 0;
		if(eep24AA08.read(buffer, systemSettingsAddress, size)){
			if(!Prm::deserialize(Prm::Save::savesys, buffer, size)){
				P_LOGW(logTag, "System settings load error");
				fp.state.sysSettingLoadDefault = 1;
			}
		}
	}

	size = Prm::getSerialSize(Prm::Save::saveuse);
	if(size){
		uint8_t buffer[size];
		if(rtc_backupRead(0, buffer, size)){
			if(!Prm::deserialize(Prm::Save::saveuse, buffer, size)){
				P_LOGW(logTag, "User settings load error");
				fp.state.userSettingLoadDefault = 1;
			}
		}
	}
}

/*!****************************************************************************
 * @brief	Save parameters to memory
 */
void saveparametersSystem(void){
	const uint16_t systemSettingsAddress = 0;
	size_t size = Prm::getSerialSize(Prm::Save::savesys);
	if(size){
		uint8_t buffer[size];
		Prm::serialize(Prm::Save::savesys, buffer);
		if(!eep24AA08.write(systemSettingsAddress, buffer, size)){
			P_LOGW(logTag, "saveparametersSystem, error save data");
		}

	}
}

/*!****************************************************************************
 * @brief	Save parameters to memory
 */
void saveparametersUser(void){
	size_t size = Prm::getSerialSize(Prm::Save::saveuse);
	if(size){
		uint8_t buffer[size];
		Prm::serialize(Prm::Save::saveuse, buffer);
		rtc_backupWrite((uint32_t*)buffer, 0, (size + 3) / 4);
	}
}

/*!****************************************************************************
 * @brief
 */
bool chargerConfig(void){
	rt9466reconfig = true;
	return true;
}

/*!****************************************************************************
 * Shutdown
 */
static void shutdown(void){
	setLcdBrightness(0);
	LED_OFF();
	ETH_BSP_Deinit();
	NVIC_SystemReset();
}

/*!****************************************************************************
 * @param ip:		 Internet Protocol address
 * @param netmask:
 * @param gateway
 */
void LwIP_Init(const uint32_t ipaddr, const uint32_t netmask, const uint32_t gateway){
	ip_addr_t l_ipaddr;
	ip_addr_t l_netmask;
	ip_addr_t l_gateway;

	//With convert 32-bits host order to network order
	l_ipaddr.addr = htonl(ipaddr);
	l_netmask.addr = htonl(netmask);
	l_gateway.addr = htonl(gateway);

	/* Create tcp_ip stack thread */
	tcpip_init( NULL, NULL);

	/*
	 * Adds your network interface to the netif_list. Allocate a struct
	 * netif and pass a pointer to this structure as the first argument.
	 * Give pointers to cleared ip_addr structures when using DHCP,
	 * or fill them with sane numbers otherwise. The state pointer may be NULL.
	 *
	 * The init function pointer must point to a initialization function for
	 * your ethernet netif interface. The following code illustrates it's use.
	 */
	P_LOGI(logTag, "set IP: %s", ipaddr_ntoa(&l_ipaddr) );
	netif_add(&xnetif, &l_ipaddr, &l_netmask, &l_gateway, NULL, &ethernetif_init, &tcpip_input);

	/*  Registers the default network interface. */
	netif_set_default(&xnetif);

	/*
	 * Set DNS server address
	 */
	ip_addr_t ipaddrs;
	IP4_ADDR(&ipaddrs, 8, 8, 8, 8);
	dns_setserver(0, &ipaddrs);
	IP4_ADDR(&ipaddrs, 1, 1, 1, 1);
	dns_setserver(1, &ipaddrs);

	auto linkcallback = [](struct netif *netif){
		fp.state.lanLink = netif_is_link_up(netif) ? 1 : 0;
		P_LOGI(logTag, "LAN link %s", fp.state.lanLink ? "Up" : "Down");
	};
	netif_set_link_callback(&xnetif, linkcallback);
	netif_set_up(&xnetif);
}

/*!****************************************************************************
 */
void netSettingUpdate(void){
	ip_addr_t l_ipaddr = { htonl(Prm::ipadr.val) };
	ip_addr_t l_netmask = { htonl(Prm::netmask.val) };
	ip_addr_t l_gateway = { htonl(Prm::gateway.val) };
	P_LOGI(logTag, "update IP: %s", ipaddr_ntoa(&l_ipaddr) );
	netif_set_addr(&xnetif, &l_ipaddr, &l_netmask, &l_gateway);
}

/*!****************************************************************************
 * @brief	Select window task & wait selected
 * 			This function need call from current GUI window
 * @param 	window	window task identifier
 */
void selWindow(selWindow_type window){
	fp.currentSelWindow = window;
	while(windowTskHandle != NULL){
		 vTaskSuspend(NULL);	//Suspend current window
	}
}

/*!****************************************************************************
 */
void timezoneUpdate(int8_t timezone){
	char str[100];
	if(Prm::dstOnOff.val){
		snprintf(str, sizeof(str), "TZ=CEST%iCET%i,M%u.%u.%u/%02u:%02u:%02u,M%u.%u.%u/%02u:%02u:%02u",
				//									| month
				//										| Week
				//											| Day
				//												| Time
				timezone, timezone - 1,
				Prm::DSTSMon.val,
				Prm::DSTSWeek.val,
				Prm::DSTSDay.val,
				Prm::DSTSHour.val,
				Prm::DSTSMin.val,
				Prm::DSTSSec.val,
				Prm::DSTEMon.val,
				Prm::DSTEWeek.val,
				Prm::DSTEDay.val,
				Prm::DSTEHour.val,
				Prm::DSTEMin.val,
				Prm::DSTESec.val
				);
	}else{
		snprintf(str, sizeof(str), "TZ=GMT%i", timezone);
	}
	putenv(str);
	tzset();
}

/*!****************************************************************************
 * @brief Init operating system
 */
void OSinit(void){
	BaseType_t Result = pdTRUE;

	Result &= xTaskCreate(systemTSK, "systemTSK", SYSTEM_TSK_SZ_STACK, NULL, SYSTEM_TSK_PRIO, NULL);
	assert(Result == pdTRUE);
	vTaskStartScheduler();
}

/******************************** END OF FILE ********************************/
