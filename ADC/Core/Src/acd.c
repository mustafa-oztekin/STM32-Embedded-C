#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"
#include "system.h"
#include "io.h"


volatile int g_bEOC;		// End of Conversion flag, like semaphore

void IADC_IoInit(int idx)
{
	IO_Init(idx, IO_MODE_ANALOG);
}

// nConv:	Toplam kanal sayısı (listedeki)
// cmode:	True/False sürekli mod
// smode:	True/False tarama modu
void IADC_Init(int ch)
{
	// 1) ADC clock setup, GPIO
	__HAL_RCC_ADC1_CLK_ENABLE();

	// 2) Prescaler, Common Control Register (CCR)
	ADC->CCR |= (1 << 16);		// PCLK2 divided by 4

	// 3) Set the Resolution, Scan Mode, Continuous Conversion, EOC, Data Align in CR1, CR2
	ADC1->CR1 &= ~(3 << 24);	// 12-bit resolution
	ADC1->CR1 |= (0 << 8);  	// Scan Mode disabled

	ADC1->CR2 |= (0 << 1);		// Continuous Conversion mode disabled
	ADC1->CR2 &= ~(1 << 11);	// Data Alignment Right

	// Set the Sampling Time for the channels in ADC_SMPRx
	if(ch <= 9)
		ADC1->SMPR2 |= (4 << (3 * ch));  // Sampling time of 84 cycles for channels 0-9
	else
		ADC1->SMPR1 |= (4 << (3 * ch));  // Sampling time of 84 cycles for channels 10-16

	// SQR1-2-3 ==> conversion
	// 1st conversion in regular sequence
	ADC1->SQR3 |= (ch << 0);

	// 6. Set the Regular channel sequence length in ADC_SQR1
	ADC1->SQR1 &= ~(15 << 20);  // SQR1_L = 0000  for 1 conversions

	uint32_t delay = 10000;
	while (delay--);

	ADC1->CR2 |= (1 << 0);         // ADC etkinleştir (ADON bit)
}


// Software trigger
// ADC işlemini yazılımsal olarak başlatır
void IADC_Start(void)
{
	// Start the Conversion by Setting the SWSTART bit in CR2
	ADC1->CR2 |= (1 << 30);		// SWSTART bitini ayarla, dönüş başlat
}



float IADC_Convert(void)
{
	IADC_Start();

	while(!(ADC1->SR & (1 << 1)))
		;
	return ADC1->DR;	// Read the Data Register
}

// ADC tamamlandığında sonuç Register değerini geri döner
int IADC_Result(void)
{
	return ADC1->DR;	// Read the Data Register
}


// ADC EOC (End of Conversion) kesme yapılandırması
void IADC_IntConfig(void)
{
	// Çevresel bölge ayarları
	// Clear the false Interrupt
	ADC1->SR &= ~ADC_SR_EOC;

	// Interrupt enable for EOC
	ADC1->CR1 |= ADC_CR1_EOCIE;

	// Çekirdek NVIC ayarları
	NVIC_SetPriority(ADC_IRQn, 2);
	NVIC_EnableIRQ(ADC_IRQn);
}


void ADC_IRQHandler(void)
{
	// birden fazla sebepten dolayı gelme olabilir
	// o yüzden mutlaka hangi sebepten gelmesini istediysek o sebepleri if bloğunda kontrol et
	if((ADC1->SR & ADC_SR_EOC) != 0) {
		g_bEOC = 1;
		ADC1->SR &= ~ADC_SR_EOC;
	}
}

