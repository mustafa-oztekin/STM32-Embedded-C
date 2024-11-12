#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "tim_driver.h"
#include "io.h"
#include "system.h"


////////////   timer 1

void Timer1_Init(unsigned prescale, unsigned period)
{
	__HAL_RCC_TIM1_CLK_ENABLE();	// timer1 clock 100 MHz

	// timer clock = 100 MHz / 50000 = 2000 Hz (0.5 ms period)
	// timer'ın herbir adımı 0.5 ms'ye denk geliyor
	TIM1->PSC = prescale - 1;

	// reload in every 100 ms
	// timer her 100 değerine geldiğinde resetlenecek
	TIM1->ARR = period - 1;

	// Enable Timer Update Interrupt
	TIM1->DIER |= TIM_DIER_UIE;

	// Enable TIM10 Interrupt on NVIC
	NVIC_EnableIRQ(TIM1_UP_TIM10_IRQn);
	NVIC_SetPriority(TIM1_UP_TIM10_IRQn,3);



	// Timer1_Reset();
}

void Timer1_Start(void)
{
	TIM1->CR1 |= TIM_CR1_CEN;
}




/////////////   timer 2



void Timer2_Init(unsigned prescale, unsigned period)
{
	__HAL_RCC_TIM2_CLK_ENABLE();	// timer2 clock 100 MHz

	// timer clock = 100 MHz / 50000 = 2000 Hz (0.5 ms period)
	// timer'ın herbir adımı 0.5 ms'ye denk geliyor
	TIM2->PSC = prescale - 1;

	// reload in every 100 ms
	// timer her 100 değerine geldiğinde resetlenecek
	TIM2->ARR = period - 1;

	// Enable Timer Update Interrupt
	TIM2->DIER |= TIM_DIER_UIE;

	// Enable TIM10 Interrupt on NVIC
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn,4);
}

void Timer2_Start(void)
{
	TIM2->CR1 |= TIM_CR1_CEN;
}


// Timer1 Irq Handler
volatile unsigned long g_T1Count;

void TIM1_UP_TIM10_IRQHandler(void)
{
	uint16_t flag = TIM1->SR;

	if((flag & TIM_SR_UIF) == SET) {
		// flag bitini sıfırlıyoruz
		TIM1->SR &= ~(TIM_SR_UIF);

		++g_T1Count;
		IO_Toggle(IOP_LED);
	}
}

// Timer2 Irq Handler
volatile unsigned long g_T2Count;

void TIM2_IRQHandler(void)
{
	uint16_t flag = TIM2->SR;

	if((flag & TIM_SR_UIF) == SET) {
		// flag bitini sıfırlıyoruz
		TIM2->SR &= ~(TIM_SR_UIF);

		++g_T2Count;
		IO_Toggle(IOP_LED1);
	}
}
