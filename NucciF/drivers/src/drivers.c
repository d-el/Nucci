/*!****************************************************************************
 * @file		drivers.с
 * @author		d_el
 * @version		V1.0
 * @date		05.07.2013
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 */

/*!****************************************************************************
 * Include
 */
#include "pvd.h"
#include "stm32f4x7_eth_bsp.h"
#include "beep.h"
#include "st7735.h"
#include "uart.h"
#include "enco.h"
#include "clock.h"
#include "ledpwm.h"
#include "rtc.h"
#include "sysTimeMeas.h"
#include "gpio.h"
#include "rng.h"
#include "systemTSK.h"
#include "uniqueDeviceId.h"
#include "i2c.h"
#include "htimer.h"

/*!****************************************************************************
 * MCU peripheral initialization
 */
void hardInit(void){
	clockState_type clockState = clock_init();
	if(clockState == clockOk){
		fp.state.mainOscillatorError = 0;
	}else{
		fp.state.mainOscillatorError = 1;
	}
	htimer_init();
	gpio_init();
	sysTimeMeasEnable();
	beep_init();
	ledPwm_init();
	if(rtc_init()){
		fp.state.rtcOscillatorError = 0;
	}else{
		fp.state.rtcOscillatorError = 1;
	}
	rng_init();
	//pvd_init();
}

/******************************** END OF FILE ********************************/
