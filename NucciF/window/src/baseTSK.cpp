/*!****************************************************************************
 * @file		base.h
 * @author		d_el - Storozhenko Roman
 * @version		V1.0
 * @date		01.01.2015
 * @copyright	The MIT License (MIT). Copyright (c) 2020 Storozhenko Roman
 * @brief		This task is base GUI
 */

/*!****************************************************************************
 * Include
 */
#include <inttypes.h>
#include <string.h>
#include <stdio.h>
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include "plog.h"
#include "ui.h"
#include "rtc.h"
#include "beep.h"
#include "key.h"
#include "display.h"
#include "graphics.h"
#include "systemTSK.h"
#include "sysTimeMeas.h"
#include "baseTSK.h"
#include <prmSystem.h>
#include <enco.h>
#include "footer.h"
#include <ledpwm.h>

enum {
	VAR_VOLT = 0,
	VAR_CURR,
	VAR_NUMBER
};

static uint16_t valmas[160];

/******************************************************************************
 * Base task
 */
void baseTSK(void *pPrm){
	(void)pPrm;
	TickType_t xLastWakeTime = xTaskGetTickCount();
	TickType_t lastActiv = xTaskGetTickCount();
	uint8_t varParam = VAR_VOLT;
	char str[30];

	struct BaeParameter{
		union{
			struct{
				const Prm::Val<uint16_t> *voltage;
				const Prm::Val<uint16_t> *current;
				const Prm::Val<uint8_t> *mode;
			};
			Prm::IVal *p[3];
		};
	};

	disp_setColor(black, red);
	disp_fillScreen(black);
	//ksSet(30, 5, 0, 0, 0);
	enco_settic(3);

	uint8_t vmode = 0;

	while(1){

		/**************************************
		 * Key process
		 */
		if(keyProc() != 0){
			if(0){
				BeepTime(ui.beep.error.time, ui.beep.error.freq);
			}else{
				BeepTime(ui.beep.key.time, ui.beep.key.freq);
				if(keyState(kNext)){
					varParam++;
					if(varParam >= VAR_NUMBER){
						varParam = VAR_VOLT;
					}
				}/*else if(keyState(kMode)){
					if(!regenable){
						selWindow(chargerWindow);
					}else{
						BeepTime(ui.beep.error.time, ui.beep.error.freq);
					}
				}*/else if(keyState(kFunc)){
					if(keyDin(kNext)){
						selWindow(settingWindow);
					}
					vmode ^= 1;
				}/*else if(keyState(kOnOff)){
					uint8_t result = 0;
					if(!regenable){

					}else{

					}
					if(result != 0){
						disp_putStr(0, 0, &arial, 0, "Error Connect");
						vTaskDelay(1000);
					}
				}*/else if(keyState(kUp)){
					//params[Prm::basepreset.val].p[varParam]->bigstep(1);
				}else if(keyState(kDown)){
					meter_clear();
				}
			}

			lastActiv = xTaskGetTickCount();
		}

		uint8_t time_s = (xTaskGetTickCount() / 300) % (sizeof(valmas)/sizeof(valmas[0]));
		valmas[time_s] = Prm::pulseCountpm.val;

		if(vmode == 0){
			if(Prm::brightnessTime > 0 && (xTaskGetTickCount() - lastActiv) > Prm::brightnessTime * 60000){
				setLcdBrightness(0);
			}else{
				setLcdBrightness(Prm::brightness.val * 10);
			}

			auto encoval = enco_update();
			if(encoval != 0){
			}

			/**************************************
			 * Output data to display
			 */
			disp_setColor(black, ui.color.cursor);

			// Level
			auto& radph = Prm::rad_unit.val == Prm::mask_unit::uR ? Prm::radVal_uRph : Prm::radVal_uSvph;
			if(Prm::countTime.val >= 60){
				if(Prm::rad_unit.val == Prm::mask_unit::uSv){
					snprintf(str, sizeof(str), "%03" PRIi32 ".%02" PRIi32, radph.val / 100, radph.val % 100);
				}else{
					snprintf(str, sizeof(str), "%04" PRIi32 ".%01" PRIi32, radph.val / 10, radph.val % 10);
				}

			}else{
				snprintf(str, sizeof(str), "----");
			}
			disp_putStr(0, 0, &dSegBold, 6, str);
			disp_putStr(130, 5, &font8x12, 0, Prm::rad_unit.val == Prm::mask_unit::uR ? "uR" : "uSv");
			disp_putStr(130, 17, &font8x12, 0, "ph");

			auto& rad = Prm::rad_unit.val == Prm::mask_unit::uR ? Prm::rad_uR : Prm::rad_uSv;
			uint32_t urad = rad.val;
			if(urad <= 99999){
				snprintf(str, sizeof(str), "%04" PRIi32 ".%01" PRIi32, urad / 10, urad % 10);
			}else{
				snprintf(str, sizeof(str), "%04" PRIi32, urad / 10);
			}
			disp_putStr(0, 34, &dSegBold, 6, str);
			disp_putStr(130, 51, &font8x12, 0, rad.getunit());

			if(Prm::countTime.val >= 60){
				snprintf(str, sizeof(str), "CPM: %" PRIu32, Prm::pulseCountpm.val);
			}else{
				snprintf(str, sizeof(str), "CPM: -");
			}
			disp_putStr(0, 72, &arial, 0, str);

			uint32_t view_time_s = Prm::countTime.val;
			if(view_time_s < 60){
				snprintf(str, sizeof(str), "Time: %" PRIu32 "s", view_time_s);
			}else if(view_time_s < 3600){
				snprintf(str, sizeof(str), "Time: %" PRIu32 "m %02" PRIu32 "s", view_time_s / 60, view_time_s % 60);
			}else{
				snprintf(str, sizeof(str), "Time: %02" PRIu32 "h%02" PRIu32 "m%02" PRIu32 "s", view_time_s / 3600, view_time_s / 60 % 60, view_time_s % 60);
			}
			disp_putStr(0, 90, &arial, 0, str);
		}
		else{
			uint16_t offset_x = 0;
			uint16_t offset_y = 7;
			uint16_t h = 100;

			uint16_t valmax = 0;
			for(size_t i = 0; i < sizeof(valmas)/sizeof(valmas[0]); i++){
				if(valmas[i] > valmax){
					valmax = valmas[i];
				}
			}
			uint16_t divFactor = (valmax + h) / h;

			uint16_t prev_y = offset_y + h - valmas[0] / divFactor;
			for(size_t i = 0; i < sizeof(valmas)/sizeof(valmas[0]); i++){
				//disp_setPixel(i, offset_y + h - valmas[i] / divFactor, white);
				uint16_t y = offset_y + h - valmas[i] / divFactor;
				grf_line(i, prev_y, i, y, red);
				if(valmas[i] > 0){
					grf_line(i, offset_y + h, i, y + 1, 0x5AE7);
				}
				prev_y = y;
			}

			disp_keepBackgroundColor(true);

			uint16_t max_y = offset_y + h - valmax / divFactor;
			max_y = max_y < 0 + font6x8.chars->image->h ? 0 + font6x8.chars->image->h : max_y;
			max_y = max_y > offset_y + h - font6x8.chars->image->h * 2 ? offset_y + h - font6x8.chars->image->h * 2 : max_y;
			snprintf(str, sizeof(str), "%u", valmax);
			disp_putStr(offset_x, max_y, &font6x8, 0, str);


			disp_setContentColor(white);
			snprintf(str, sizeof(str), "%ucpm", h * divFactor);
			disp_putStr(offset_x, 0, &font6x8, 0, str);
			snprintf(str, sizeof(str), "0cpm");
			disp_putStr(offset_x, offset_y + h - font6x8.chars->image->h, &font6x8, 0, str);
			disp_keepBackgroundColor(false);
		}

		//Print status bar
		printFooter();
		disp_flushfill(&ui.color.background);
		//Cyclic delay
		vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(BASE_TSK_PERIOD));
	}
}

/******************************** END OF FILE ********************************/
