#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include "io.h"
#include "modul.h"
#include "uart.h"
#include "system.h"
#include "button.h"


typedef uint32_t clock_t;

void Task_LED(void);
void Task_Print(void);
void Task_Buttons(void);

int main(void)
{
  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // LED başlangıç
  IO_Init(IOP_LED, IO_MODE_OUTPUT, GPIO_NOPULL);

  // Konsol başlangıç
  Sys_ConsoleInit();

  // Button başlangıç
  BTN_InitButtons();

  // printf("Hello, World!\n");
  UART_puts("\nMerhaba Dunya!\n");

  // Görev Çevrimi (Task Loop)
  // Co-Operative Multitasking (Yardımlaşmalı Çoklu Görev)
  while (1)
  {
	  Task_LED();
	  // Task_Print();
	  Task_Buttons();
  }
}


void Task_Buttons(void)
{
	static unsigned count = 0;
	if(g_Buttons[BTN_SET]) {
		// SET işle
		UART_printf("SET : (%u)\n", ++count);

		// g_Buttons[BTN_SET] = 0;		// binary semaphore
		--g_Buttons[BTN_SET];			// counting semaphore
	}
	if(g_Buttons[BTN_UP]) {
		// UP işle
		UART_printf("UP : (%u)\n", ++count);

		// g_Buttons[BTN_UP] = 0;
		--g_Buttons[BTN_UP];			// counting semaphore
	}
	if(g_Buttons[BTN_DOWN]) {
		// DOWN işle
		UART_printf("DOWN : (%u)\n", ++count);

		// g_Buttons[BTN_DOWN] = 0;
		--g_Buttons[BTN_DOWN];			// counting semaphore
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
	static unsigned count;

	// printf("SAYI:%10u\r", ++count);
	UART_printf("Count: %10lu\r", ++count);
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
