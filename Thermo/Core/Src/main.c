#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "system.h"
#include "uart.h"
#include "io.h"
#include "1-Wire.h"
#include "crc8.h"

#define SYS_CLOCK_FREQ	100000000

typedef uint32_t clock_t;

void Task_LED(void);
void Task_Print(void);
void Task_ThermoB(void);

int main(void)
{
  unsigned char dsID[8];
  int i;
  unsigned char crc;

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // LED başlangıç
  IO_Init(IOP_LED, IO_MODE_OUTPUT);

  Sys_ConsoleInit();

  // 1-Wire Bus başlangıç
  OneWire_Init();

  if(DS_ReadRom(dsID)) {
	  UART_puts("\nID: ");
	  for(i = 7; i >= 0; --i)
		  UART_printf("%02X", dsID[i]);
	  UART_putch('\n');

	  crc = CRC8(dsID, 8);
	  UART_printf("CRC: %02X", crc);
	  UART_putch('\n');
  }
  else {
	  UART_puts("Hata: DS yok!");
	  UART_putch('\n');
  }

  // Görev Çevrimi (Task Loop)
  // Co-Operative Multitasking (Yardımlaşmalı Çoklu Görev)
  while (1)
  {
	  Task_LED();
	  Task_ThermoB();
  }
}


void Task_ThermoB(void)
{
	int temp;

	if(!DS_ReadTemp(&temp))
		UART_puts("\rERROR          ");
	else
		UART_printf("\rTemp:% 6.1f", (float)temp / 16);
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
