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
	__HAL_RCC_TIM2_CLK_ENABLE();	// timer2 clock 72 MHz

	// PWM için güncelleme
	// timer clock = 72 MHz / 36 = 2 MHz
	TIM2->PSC = prescale - 1;

	// period ==> (2 MHz / 200) = 10 kHz
	TIM2->ARR = period - 1;

	// Enable Timer Update Interrupt
	// TIM2->DIER |= TIM_DIER_UIE;

	// Enable TIM10 Interrupt on NVIC
	// NVIC_EnableIRQ(TIM2_IRQn);
	// NVIC_SetPriority(TIM2_IRQn,4);
}

void Timer2_Start(void)
{
	TIM2->CR1 |= TIM_CR1_CEN;
}

void Timer2_Stop(void)
{
	TIM2->CR1 &= ~TIM_CR1_CEN;
}


///////////////////////////////////////////////////////////////////
// TIM2 CH2 kullanılacak
// freq: PWM frekansı
// duty: PWM duty cycle % olarak
int PWM_Init(float freq, float duty)
{
	// SystemCoreClock
	uint32_t period, prescale;

	// 1) Çıkış kanalının I/O ayarları
	IO_Init(IOP_TIM2_CH2, IO_MODE_ALTERNATE);
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

	// 4) Timer'ı çalıştır
	TIM2->EGR |= (1 << 0);
	Timer2_Start();

	return period;
}

void PWM_SetDuty(int period, float duty)
{
	// set duty cycle
	TIM2->CCR2 = period * (duty / 100);
}



///////////////////////////////////////////////////////////////////
// Timer1 Irq Handler
volatile unsigned long g_T1Count;
void TIM1_UP_TIM10_IRQHandler(void)
{
	uint16_t flag = TIM1->SR;

	if((flag & TIM_SR_UIF) == SET) {
		// flag bitini sıfırlıyoruz
		TIM1->SR &= ~(TIM_SR_UIF);

		++g_T1Count;
		// IO_Toggle(IOP_LED);
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

		// ++g_T2Count;
	}
}
