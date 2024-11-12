#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "system.h"
#include "uart.h"
#include "io.h"
#include "tim_driver.h"

#define SYS_CLOCK_FREQ	100000000

typedef uint32_t clock_t;

void Task_LED(void);
void Task_Print(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // LED başlangıç
  IO_Init(IOP_LED, IO_MODE_OUTPUT, GPIO_NOPULL);
  IO_Init(IOP_LED1, IO_MODE_OUTPUT, GPIO_NOPULL);
  IO_Init(IOP_LED2, IO_MODE_OUTPUT, GPIO_NOPULL);
  IO_Init(IOP_LED3, IO_MODE_OUTPUT, GPIO_NOPULL);

  Sys_ConsoleInit();

  UART_puts("Merhaba Dunya!\n");

  Timer1_Init(SYS_CLOCK_FREQ / 2000, 1000);
  Timer2_Init(SYS_CLOCK_FREQ / 2000, 2000);

  Timer1_Start();
  Timer2_Start();


  // Görev Çevrimi (Task Loop)
  // Co-Operative Multitasking (Yardımlaşmalı Çoklu Görev)
  while (1)
  {
	  // Task_LED();
	  // Task_Print();
  }
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
	// static unsigned count;

#ifdef USING_LCD
	printf("SAYI:%10u\r", ++count);
#else
	UART_printf("\rCount: %10lu", g_T1Count);
#endif
}



#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{}
#endif /* USE_FULL_ASSERT */
