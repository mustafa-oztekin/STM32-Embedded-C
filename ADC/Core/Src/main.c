#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "system.h"
#include "uart.h"
#include "io.h"
#include "tim_driver.h"
#include "adc.h"

#define SYS_CLOCK_FREQ	72000000

typedef uint32_t clock_t;

int g_PWMPeriod;

void Task_LED(void);
void Task_Print(void);
void Task_ADC(void);
void Task_ADC_Int(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // LED başlangıç
  IO_Init(IOP_LED, IO_MODE_OUTPUT);

  Sys_ConsoleInit();

  g_PWMPeriod = PWM_Init(10000, 50);

  IADC_IoInit(IOP_AIN_CH0);
  IADC_Init(0);
  IADC_IntConfig();

  IADC_Start();		// Conversion software start

  // Görev Çevrimi (Task Loop)
  // Co-Operative Multitasking (Yardımlaşmalı Çoklu Görev)
  while (1)
  {
	  Task_LED();
	  Task_ADC_Int();
	  // Task_ADC();
  }
}


void Task_ADC_Int(void)
{
	int result;

	if(g_bEOC) {

		g_bEOC = 0;

		result = IADC_Result();

		UART_printf("\rADC= %5d", result);

		// bir sonraki dönüşümü başlatıyoruz
		// eğer sürekli modda isek gerek olmayacak
		IADC_Start();
	}
}

void Task_ADC(void)
{
	int duty;
	float result;

	result = IADC_Convert();

	duty = (result / 4095) * 100;

	PWM_SetDuty(g_PWMPeriod, duty);

	UART_printf("\rADC result= %5d", (int)result);

	// duty = g_PWMPeriod * (result / 4095);
	// PWM_SetDuty(duty);
}




void Task_LED(void)
{
	static enum {
		I_LED_OFF,
		S_LED_OFF,
		I_LED_ON,
		S_LED_ON,
	} state = I_LED_OFF;
	static clock_t t0;	// duruma ilk geçiş saati
	clock_t t1;			// güncel saat değeri
	t1 = HAL_GetTick();
	switch(state) {
	case I_LED_OFF:
		t0 = t1;
		IO_WRITE(IOP_LED, 0);
		state = S_LED_OFF;
		break;
	case S_LED_OFF:
		if(t1 >= t0 + 900) {
			state = I_LED_ON;
		}
		break;
	case I_LED_ON:
		t0 = t1;
		IO_WRITE(IOP_LED, 1);
		state = S_LED_ON;
		break;
	case S_LED_ON:
		if(t1 >= t0 + 100) {
			state = I_LED_OFF;
		}
		break;
	}
}

void Task_Print(void)
{
	static unsigned count;

#ifdef USING_LCD
	printf("SAYI:%10u\r", ++count);
#else
	UART_printf("\rCount: %10lu", ++count);
#endif
}



#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{}
#endif /* USE_FULL_ASSERT */
