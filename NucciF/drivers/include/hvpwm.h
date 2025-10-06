/*!****************************************************************************
 * @file		hvpwm.h
 * @author		d_el
 * @version		V1.0
 * @date		03.10.2025
 * @copyright	The MIT License (MIT). Copyright (c) 2025 Storozhenko Roman
 * @brief		HV converter PWM
 */
#ifndef hvpwm_H
#define hvpwm_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"

typedef void (*pwmUpdateCallback_t)(void);

/*!****************************************************************************
 * Prototypes for the functions
 */
void hvPwm_init(void);
void hvPwm_set(uint16_t val);
void hvPwm_UpdateCallbackSet(pwmUpdateCallback_t c);

#ifdef __cplusplus
}
#endif

#endif //hvpwm_H
/******************************** END OF FILE ********************************/
