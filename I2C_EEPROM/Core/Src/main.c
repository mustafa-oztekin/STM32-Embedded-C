#include <stdio.h>
#include <stdint.h>
#include "main.h"
#include "system.h"
#include "io.h"
#include "hI2C.h"
#include "LCDM.h"
#include "eeprom24.h"
#include "uart.h"

#define SYS_CLOCK_FREQ	72000000

enum { False, True };

typedef uint32_t clock_t;


void I2C_Test(void);
void Task_Print(void);
void Task_LED(void);


int main(void)
{
  char str[256] = { 0 };

  // int adres = 0;

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();

  // LED başlangıç
  IO_Init(IOP_LED, IO_MODE_OUTPUT);

  // I2C Init
  HI2C_Init(100000);

  // UART2 Init
  UART_Init();

  // I2C Test
  // I2C_Test();

  // EEPROM - 24LC512
  E24_Config(2, 128);

  // LCD - I2C Init
  Sys_ConsoleInit();

  LCD_DisplayOn(LCD_MODE_ON | LCD_MODE_CURSOR);
  DelayMs(3);

  // LCD_Write(0, "mustafa");

  /*
  adres = E24_WriteString(0, 0, "SON BYTE: 14/");

  if(adres)
	  LCD_Puts("yazma basarili\n");
  else
	  LCD_Puts("yazma hatasi\n");
	  */

  /*  EEPROM kullanırken başlangıçta okuyup hangi Byte'ta kaldığımızı görelim  */
  if(!E24_Read(0, 0, str, 13))
	  LCD_Write(0, "E24 okunamadi!\n");
  else {
	  LCD_Write(0, "EEPROM okundu!\n");
	  str[13] = 0;
	  LCD_Write(0, str);
  }

  // Görev Çevrimi (Task Loop)
  // Co-Operative Multitasking (Yardımlaşmalı Çoklu Görev)
  while (1)
  {
	  Task_LED();
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
	static unsigned count;
	printf("SAYI:%10u\n", ++count);
}



void I2C_Test(void)
{
	int status;

	if(HI2C_Start(I2C1, 0xA0)) {	// EEPROM - 24LC512
	  HI2C_Stop(I2C1);
	  status = True;
	}
	else {
	  status = False;
	}

	if(HI2C_Start(I2C1, 0x4E)) {	// LCD Display - PCF8574
	  HI2C_Stop(I2C1);
	  status = True;
	}
	else {
	  status = False;
	}
	/*
	// I2C cihazlarının adreslerinin bulunması
	for(uint8_t i = 0; i < 128; ++i) {
		if(HI2C_Start(I2C1, i << 1)) {
			HI2C_Stop();
			status = True;
		} else {
			status = False;
		}
		DelayMs(200);
	}
	*/
}


#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{}
#endif /* USE_FULL_ASSERT */
