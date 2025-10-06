/*!****************************************************************************
 * @file    	adcTSK.h
 * @author  	Storozhenko Roman - D_EL
 * @version 	V1.0
 * @date    	04-10-2025
 * @copyright 	The MIT License (MIT). Copyright (c) 2025 Storozhenko Roman
 */
#ifndef ADC_TSK_H
#define ADC_TSK_H

/*!****************************************************************************
 * Include
 */

/*!****************************************************************************
 * User typedef
 */
typedef struct {
	struct {
		uint16_t vhv;
	}filtered;
	uint16_t		vhvoffset;
	uint16_t		targetcurrentlsb;
	bool 			overload;
} adcTaskStct_type;

/*!****************************************************************************
 * External variables
 */
extern adcTaskStct_type adcTaskStct;

/*!****************************************************************************
 * Prototypes for the functions
 */
void adcTSK(void *pPrm);

#endif //ADC_TSK_H
/******************************** END OF FILE ********************************/
