/*!****************************************************************************
 * @file		enco.c
 * @author		d_el
 * @version		V2.4
 * @date		16.12.2018
 * @copyright	The MIT License (MIT). Copyright (c) 2018 Storozhenko Roman
 * @brief		encoder driver
 */

/*!****************************************************************************
 * Include
 */
#include "stm32f4xx.h"
#include "gpio.h"
#include "enco.h"

/*!****************************************************************************
 * @brief  Initialize peripheral for incremental encoder
 */
void enco_init(void){}

/*!****************************************************************************
 * @brief
 */
int16_t enco_read(void){
	return 0;
}

/*!****************************************************************************
 */
int32_t enco_update(void){
	return 0;
}

/*!****************************************************************************
 */
void enco_settic(uint16_t n){ (void)n; }

/******************************** END OF FILE ********************************/
