#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "stm32f4xx_hal.h"
#include "tim_driver.h"
#include "io.h"
#include "system.h"
#include "stm32f4xx_hal_tim.h"


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


/////////////   timer 2 Init
void Timer2_Init(unsigned prescale, unsigned period)
{
	__HAL_RCC_TIM2_CLK_ENABLE();	// timer2 clock 72 MHz

	// timer clock = 72 MHz
	// pwm init'te psc değeri vermiyoruz ama arr yani period'a 7200 veriyoruz
	// bu sayede 72000000 / 7200 = 10 kHz'lik pwm sinyali üretmiş oluyoruz.
	TIM2->PSC = prescale - 1;

	// reload in every 100 ms
	// timer her 100 değerine geldiğinde resetlenecek
	TIM2->ARR = period - 1;
}




/////////////   timer 2 Interrupt Config
void Timer2_IntConfig(int priority)
{
	// Enable Timer Update Interrupt
	TIM2->DIER |= TIM_DIER_UIE;

	// Enable TIM2 Interrupt on NVIC
	NVIC_EnableIRQ(TIM2_IRQn);
	NVIC_SetPriority(TIM2_IRQn, priority);
}


void Timer2_Start(void)
{
	TIM2->CR1 |= TIM_CR1_CEN;
}

/////////////   timer 4 Init
void Timer4_Init(unsigned prescale, unsigned period)
{
	__HAL_RCC_TIM4_CLK_ENABLE();	// timer2 clock 72 MHz

	// timer clock = 72 MHz
	// pwm init'te psc değeri vermiyoruz ama arr yani period'a 7200 veriyoruz
	// bu sayede 72000000 / 7200 = 10 kHz'lik pwm sinyali üretmiş oluyoruz.
	TIM4->PSC = prescale - 1;

	// reload in every 100 ms
	// timer her 100 değerine geldiğinde resetlenecek
	TIM4->ARR = period - 1;
}

/////////////   timer 4 Interrupt Config
void Timer4_IntConfig(int priority)
{
	// Enable Timer Update Interrupt
	TIM4->DIER |= TIM_DIER_UIE;

	// Enable TIM10 Interrupt on NVIC
	NVIC_EnableIRQ(TIM4_IRQn);
	NVIC_SetPriority(TIM4_IRQn, priority);
}

void Timer4_Start(void)
{
	TIM4->CR1 |= TIM_CR1_CEN;
}




///////////////////////////////////////////////////////////////////
// TIM2 CH2 kullanılacak
// freq: PWM frekansı
// duty: PWM duty cycle % olarak
void PWM_Init(double freq, double duty)
{
	// SystemCoreClock
	uint32_t period, prescale;

	// 1) Çıkış kanalının I/O ayarları
	IO_Init(IOP_TIM2_CH2, IO_MODE_ALTERNATE, GPIO_NOPULL);	// AF1	port/pin 1
	GPIOA->AFR[0] &= ~(1UL << 7);
	GPIOA->AFR[0] &= ~(1UL << 6);
	GPIOA->AFR[0] &= ~(1UL << 5);
	GPIOA->AFR[0] |= (1UL << 4);

	// Timer ayarları,	PWM frekansı belirlenecek
	// ftov = fpwm = ftmr / (period * prescale)
	// Tpwm = (period * prescale) / ftmr

	period = SystemCoreClock / freq;

	if(period >= 65536) {
		prescale = (period / 65536) + 1;	// Prescale'i ayarla.
		period = period / prescale;			// Periodu yeni prescale değerine göre ayarla.
	}
	else
		prescale = 1;			// Prescale'i 1 olarak ayarla.

	Timer2_Init(prescale, period);

	// 3) PWM ayarları
	// Output Compare

	// Output Compare Mode
	// TIM2->CCMR1 &= ~(3UL << 8);	!?

	// CH-2 PWM MODE
	TIM2->CCMR1 |= (1 << 14);
	TIM2->CCMR1 |= (1 << 13);
	TIM2->CCMR1 &= ~(1 << 12);
	TIM2->CCMR1 |= (1 << 11);	// preload

	// Output Polarity : Active High
	TIM2->CCER &= ~(1 << 5);
	// Enable OC2REF Output
	TIM2->CCER |= (1 << 4);

	// set duty cycle
	TIM2->CCR2 = period * (duty / 100);

	// 4) Timer2'yi çalıştır
	TIM2->EGR |= (1 << 0);
	TIM2->CR1 |= TIM_CR1_CEN;
}


void Capture_Init(void)
{
	// 1) I/O ayarı
	IO_Init(IOP_TIM4_CH3, IO_MODE_ALTERNATE, GPIO_NOPULL);	// AF2	port/pin B8
	GPIOB->AFR[1] &= ~(1UL << 3);
	GPIOB->AFR[1] &= ~(1UL << 2);
	GPIOB->AFR[1] |= (1UL << 1);
	GPIOB->AFR[1] &= ~(1UL << 0);

	// 2) Timer'ı başlat		input capture'da timer önce çalıştırılmalı
	Timer4_Init(1, 65536);

	// 3) Input Capture Mode
	// CC3S[1:0] = 01
	TIM4->CCMR2 |= (1UL << 0);
	TIM4->CCMR2 &= ~(1UL << 1);

	// PSC 00
	TIM4->CCMR2 &= ~(3 << 2);
	// filtre
	TIM4->CCMR2 &= ~(0xF << 4);

	// Enable IC3REF Input
	TIM4->CCER |= (1 << 8);		// IC Enable	CC3E

	// 4) Interrupt Config
	TIM4->DIER |= (1 << 3);		// CC3 Interrupt Enable

	// Enable TIM4 Interrupt on NVIC
	NVIC_SetPriority(TIM4_IRQn, 0);
	NVIC_EnableIRQ(TIM4_IRQn);

	// 5) Timer4'ü çalıştır
	TIM4->CR1 |= TIM_CR1_CEN;
}


volatile unsigned long g_T1Count, g_T2Count, g_T4Count;
// Timer1 Irq Handler
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
void TIM2_IRQHandler(void)
{
	uint16_t flag = TIM2->SR;

	if((flag & TIM_SR_UIF) == SET) {
		// flag bitini sıfırlıyoruz
		TIM2->SR &= ~(TIM_SR_UIF);

		++g_T2Count;
	}
}


// Timer4 Irq Handler
void TIM4_IRQHandler(void)
{
	/*
	if((TIM4->SR & TIM_SR_UIF) == SET) {
		++g_T4Count;

		TIM4->SR &= ~(TIM_SR_UIF);

	}
	*/

	if((TIM4->SR & TIM_SR_CC3IF)) {
		IO_Toggle(IOP_LED);

		TIM4->SR &= ~(TIM_SR_CC3IF);
	}
}

