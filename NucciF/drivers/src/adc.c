/*!****************************************************************************
 * @file		adc.c
 * @author		Storozhenko Roman - D_EL
 * @version 	V1.0
 * @date		03-10-2025
 * @copyright 	The MIT License (MIT). Copyright (c) 2025 Storozhenko Roman
 */

/*!****************************************************************************
* Include
*/
#include <stddef.h>
#include "gpio.h"
#include "board.h"
#include "adc.h"

/*!****************************************************************************
* MEMORY
*/
adcStct_type adcStct = {
	.sampleRate = 10000,	// Default sample Rate
};

#define ADC_DMA_CHANNEL		0

/*!****************************************************************************
* TIM1 -> ADC1 -> DMA2_Channel3 -> DMA1_Channel1_IRQHandler
*/
void adc_init(void){
	adcStct.adc = ADC1;
	adcStct.com = ADC123_COMMON;
	adcStct.tim = TIM3;
	adcStct.dmaSream = DMA2_Stream4;
	adcStct.dma = DMA2;

	/**********************************
	 * IO
	 */
	//Analog Input
	gppin_init(GPIOA, 4, analogMode, pullDisable, 0, 0);

	for(int i = 0; i < 360000; i++) __NOP();

	/**********************************
	 * Clock
	 */
	RCC->APB2ENR |= RCC_APB2ENR_ADC1EN;					// ADC clock Enable
	RCC->AHB2RSTR |= RCC_APB2RSTR_ADCRST;				// ADC reset
	RCC->AHB2RSTR &= ~RCC_APB2RSTR_ADCRST;

	/**********************************
	 * ADC
	 */
	adcStct.com->CCR &= ~ADC_CCR_ADCPRE;
	adcStct.com->CCR |= 3 << ADC_CCR_ADCPRE_Pos;			// PCLK2 divided by 8
	adcStct.adc->CR1 |= ADC_CR1_SCAN;						//Scan mode enabled
	adcStct.adc->CR2 |= ADC_CR2_EXTEN;						//Conversion on external event enabled
	adcStct.adc->CR2 |= 8 << ADC_CR2_EXTSEL_Pos;			// Timer 3 TRGO event
	if(CH_NUMBER == 1){
		adcStct.adc->CR2 |= ADC_CR2_DDS;					// DMA disable selection (for single ADC mode)
	}
	adcStct.adc->CR2 |= ADC_CR2_DMA;						// DMA mode enabled
	adcStct.adc->SMPR2 |= 1 << ADC_SMPR2_SMP4_Pos;			// Sample time x cycles
	adcStct.adc->SQR1 |= 0 << ADC_SQR1_L_Pos;				// Regular channel sequence length 1 conversion
	adcStct.adc->SQR3 |= 4 << ADC_SQR3_SQ1_Pos;				// Channel 4
	adcStct.adc->CR2 |= ADC_CR2_ADON;

//    ADC1->CR2       |= ADC_CR2_RSTCAL;
//    while((ADC1->CR2 & ADC_CR2_RSTCAL) != 0);
//    ADC1->CR2       |= ADC_CR2_CAL;
//    while((ADC1->CR2 & ADC_CR2_RSTCAL) != 0);

	/**********************************
	 * DMA Init
	 */
	RCC->AHB1ENR |= RCC_AHB1ENR_DMA2EN;
	adcStct.dmaSream->CR = 0;
	adcStct.dmaSream->CR |= (uint32_t)((ADC_DMA_CHANNEL & 0x03) << 25);	// Channel selection
	adcStct.dmaSream->CR |= DMA_SxCR_PL_1;								// Priority level High
	adcStct.dmaSream->CR |= DMA_SxCR_MSIZE_0;							// Memory data size half-word (16-bit)
	adcStct.dmaSream->CR |= DMA_SxCR_PSIZE_0;							// Memory data size half-word (16-bit)
	adcStct.dmaSream->CR |= DMA_SxCR_MINC;								// Memory increment mode enabled
	adcStct.dmaSream->CR &= ~DMA_SxCR_PINC;								// Peripheral increment mode disabled
	adcStct.dmaSream->CR |= DMA_SxCR_CIRC;								// Circular mode enable
	adcStct.dmaSream->CR &= ~DMA_SxCR_DIR;								// Direction Peripheral-to-memory
	adcStct.dmaSream->CR |= DMA_SxCR_TCIE;								// Transfer complete interrupt enable
	adcStct.dmaSream->PAR = (uint32_t)&ADC1->DR;						// Peripheral address
	adcStct.dmaSream->M0AR = (uint32_t)&adcStct.adcreg[0];
	adcStct.dmaSream->NDTR = CH_NUMBER;
	NVIC_EnableIRQ(DMA2_Stream4_IRQn);
	NVIC_SetPriority(DMA2_Stream4_IRQn, 15/*Priority*/);
	adcStct.dmaSream->CR |= DMA_SxCR_EN;
}

/*!****************************************************************************
 *
 */
void adc_startSampling(void){
	TIM1->CR1 |= TIM_CR1_CEN;
}

/*!****************************************************************************
 *
 */
void adc_stopSampling(void){
	TIM1->CR1 &= ~TIM_CR1_CEN;
}

/*!****************************************************************************
 *
 */
void adc_setSampleRate(uint16_t us){
	adcStct.sampleRate = us;
	TIM1->ARR = us;
}

/*!****************************************************************************
 *
 */
void adc_setCallback(adcCallback_type tcHoock){
	adcStct.tcHoock = tcHoock;
}

/*!****************************************************************************
*---> DMA for SAADC Interrupt Handler
*/
void DMA2_Stream4_IRQHandler(void){
	if(adcStct.tcHoock != NULL){
		adcStct.tcHoock(&adcStct);
	}
	adcStct.dma->HIFCR = DMA_HIFCR_CTCIF4; //Clear flag
}

/******************************** END OF FILE ********************************/
