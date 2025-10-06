/*!****************************************************************************
 * @file		footer.c
 * @author		d_el
 * @version		V1.0
 * @date		Dec 20, 2023
 * @copyright	License (MIT). Copyright (c) 2023 Storozhenko Roman
 * @brief
 */

/*!****************************************************************************
 * Include
 */
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include "plog.h"
#include "ui.h"
#include "rtc.h"
#include "beep.h"
#include "display.h"
#include "graphics.h"
#include "systemTSK.h"
#include <prmSystem.h>

/*!****************************************************************************
 * @brief
 */
void printFooter(void){
	char str[36];

	//Print line
	grf_line(0, 107, 159, 107, halfLightGray);

	disp_setColor(black, white);

	char vstr[16];
	Prm::ch_vbatMeas.tostring(vstr, sizeof(vstr));
	snprintf(str, sizeof(str), "VBAT %sV", vstr);
	disp_putStr(0, 110, &font6x8, 0, str);

	//snprintf(str, sizeof(str), "CP   %09" PRIu32, Prm::odoPulseCount.val);
	//disp_putStr(0, 120, &font6x8, 0, str);

	snprintf(str, sizeof(str), "VHV  %03" PRIu16 "V", Prm::vhv_meas.val);
	disp_putStr(0, 120, &font6x8, 0, str);

	//Print time
	struct tm tm;
	time_t unixTime = time(NULL);
	localtime_r(&unixTime, &tm);
	strftime(str, sizeof(str), "%H:%M:%S", &tm);
	disp_putStr(110, 110, &font6x8, 0, str);
	strftime(str, sizeof(str), "%d.%m.%y", &tm);
	disp_putStr(110, 120, &font6x8, 0, str);

	//Print LAN status
	static TickType_t xTime;
	static uint8_t ledon = 0;
	if(fp.state.lanLink != 0){
		if(fp.state.httpactiv != 0){
			xTime = xTaskGetTickCount();
			ledon = 1;
			fp.state.httpactiv = 0;
		}
		if((ledon != 0)&&((xTaskGetTickCount() - xTime) >= 200)){
			ledon = 0;
		}

		if(ledon == 0){
			disp_setColor(black, white);
		}else{
			disp_setColor(black, red);
		}

		snprintf(str, sizeof(str), "LAN");
		disp_putStr(63, 110, &font6x8, 0, str);
	}

	extern uint8_t chargerStatus;
	snprintf(str, sizeof(str), "CHS %u", chargerStatus);
	disp_putStr(63, 120, &font6x8, 0, str);
}

/******************************** END OF FILE ********************************/
