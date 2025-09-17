	/*!****************************************************************************
 * @file		beep.h
 * @author		d_el
 * @version		V1.0
 * @date		19.12.2014
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		Driver beep
 */
#ifndef beep_H
#define beep_H

#ifdef __cplusplus
extern "C" {
#endif

/*!****************************************************************************
 * Include
 */
#include <stdint.h>
#include <stdbool.h>

/*!****************************************************************************
 * Function declaration
 */
void BeepTime(uint16_t time, uint16_t ferq);
void beep_init(void);
void beep_enable(bool en);

#ifdef __cplusplus
}
#endif

#endif //beep_H
/******************************** END OF FILE ********************************/
