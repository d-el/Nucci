/*!****************************************************************************
 * @file		ledpwm.c
 * @author		d_el
 * @version		V1.0
 * @date		27.12.2015
 * @copyright	The MIT License (MIT). Copyright (c) 2017 Storozhenko Roman
 * @brief		pwm for lcd led
 */

/*!****************************************************************************
 * Include
 */
#include "gpio.h"
#include "hvpwm.h"

/*!****************************************************************************
 * MEMORY
 */
TIM_TypeDef *tim3 = TIM3;
static pwmUpdateCallback_t pwmUpdateCallback;

/*!****************************************************************************
 * @brief Initialize timer
 */
void hvPwm_init(void){
	//TIM3_CH1 - PA6
	gppin_init(GPIOA, 6, alternateFunctionPushPull, pullDisable, 0, 2);

	RCC->APB1ENR |= RCC_APB1ENR_TIM3EN;						// Clock Enable
	RCC->APB1RSTR |= RCC_APB1RSTR_TIM3RST;					// Reset
	RCC->APB1RSTR &= ~RCC_APB1RSTR_TIM3RST;

	tim3->PSC = 1 - 1;										// Set prescaler
	TIM3->CCER |= TIM_CCER_CC1E;							// Channel enable
	TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;		// PWM mode
	TIM3->ARR = 20000;										// Auto-reload value
	TIM3->CCR1 = 0;											// Compare value
	TIM3->CR1 |= TIM_CR1_ARPE;								// TIMx_ARR register is buffered
	TIM3->CR2 |= TIM_CR2_MMS_1;								// Update - The update event is selected as trigger output (TRGO)
	TIM3->BDTR |= TIM_BDTR_MOE;								// Main output enable
	TIM3->DIER |= TIM_DIER_UIE;								// Update interrupt enable

	NVIC_EnableIRQ(TIM3_IRQn);
	NVIC_SetPriority(TIM3_IRQn, 14/*Priority*/);

	TIM3->CR1 |= TIM_CR1_CEN;								// Count enable
}

/*!****************************************************************************
* @brief
*/
void hvPwm_set(uint16_t ccr){
	if(ccr > 120){
		ccr = 120;
	}

	if(ccr > TIM3->ARR){
		ccr = TIM3->ARR;
	}
	TIM3->CCR1 = ccr;
}

void TIM3_IRQHandler(void){
	if(pwmUpdateCallback){
		pwmUpdateCallback();
	}
	TIM3->SR = ~TIM_SR_UIF; // Clear flag
}

void hvPwm_UpdateCallbackSet(pwmUpdateCallback_t c){
	pwmUpdateCallback = c;
}

/******************************** END OF FILE ********************************/
