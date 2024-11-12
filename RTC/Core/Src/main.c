#include "main.h"
#include <stdint.h>
#include <stdio.h>
#include "io.h"
#include "modul.h"
#include "uart.h"
#include "system.h"
#include "button.h"
#include "irtc.h"


typedef uint32_t clock_t;

extern RTC_HandleTypeDef hrtc;


// Başlangıç zamanı saklanacak yapı
// RTC_TimeTypeDef startTime;
// RTC_DateTypeDef startDate;

// RTC setup timeout değeri
#define RTC_TIMEOUT		30000

enum { IRTC_WORK, IRTC_SETUP };
int g_RtcMode = IRTC_WORK;

void Task_LED(void);
void Task_Print(void);
void Task_Buttons(void);
void Task_Time(void);
void Task_RTC(void);
void DisplayTime(uint32_t tmVal);
void Task_RTC_Time(void);
void Task_RTC_Alarm(void);

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
  // UART_puts("\nMerhaba Dunya!\n\n");

  MX_RTC_Init();
  // IRTC_WakeUpIntConfig();
  IRTC_AlrmIntConfig();

  // Başlangıç zamanını al
  // HAL_RTC_GetTime(&hrtc, &startTime, RTC_FORMAT_BCD);
  // HAL_RTC_GetDate(&hrtc, &startDate, RTC_FORMAT_BCD);

  // Görev Çevrimi (Task Loop)
  // Co-Operative Multitasking (Yardımlaşmalı Çoklu Görev)
  while (1)
  {
	  Task_LED();
	  // Task_Print();
	  // Task_Buttons();
	  // Task_Time();
	  // Task_RTC_Time();
	  Task_RTC();
	  Task_RTC_Alarm();
  }
}


void Task_RTC(void)
{
	// static olmasının nedeni, bu fonksiyona her girdiğinde önceki değeri koruması gerekiyor
	// eğer static olmasaydı fonksiyona her girildiğinde fonksiyon local olduğu için
	// state değeri yeni değerden başlayacaktı.
	// state machine kurgusunda static önemli çünkü her fonksiyon çağrıldığında önceki değeri bilmeli
	static enum {
		I_WORK,
		S_WORK,
		I_SETHOUR,
		S_SETHOUR,
		I_SETMIN,
		S_SETMIN,
		S_INCHOUR,
		S_DECHOUR,
		S_INCMIN,
		S_DECMIN,
		S_UPDATE,
		S_CANCEL,
	} state = I_WORK;

	static clock_t t0, t1;
	static uint32_t tm0, tm1;
	static int hour, min;

	t1 = HAL_GetTick();		// güncel saat değeri

	switch(state) {
	case I_WORK:
			g_RtcMode = IRTC_WORK;

#ifdef USING_LCD
			LCD_DisplayOn(LCD_ON);
#endif

			state = S_WORK;

	case S_WORK:
			tm1 = get_TimeCount();
			if(tm1 != tm0) {
				DisplayTime(tm1);
				tm0 = tm1;
			}
			if(g_Buttons[BTN_SET]) {
				g_Buttons[BTN_SET] = 0;

				// clear false signals
				g_Buttons[BTN_UP] = 0;
				g_Buttons[BTN_DOWN] = 0;

				hour = (tm1 / 3600) % 24;
				min = (tm1 / 60) % 60;
				state = I_SETHOUR;
			}
			break;

	case I_SETHOUR:
			g_RtcMode = IRTC_SETUP;

#ifdef USING_LCD
			LCD_DisplayOn(LCD_ON | LCD_BLINK);
#endif

			DisplayTime(hour * 3600 + min * 60);

#ifdef USING_LCD
			LCD_SetCoursor(1);
#endif

			t0 = t1;
			state = S_SETHOUR;
			break;

	case S_SETHOUR:
			if(t1 - t0 >= RTC_TIMEOUT) {
				state = S_CANCEL;
			}
			if(g_Buttons[BTN_SET]) {
				g_Buttons[BTN_SET] = 0;
				state = I_SETMIN;
			}
			else if(g_Buttons[BTN_UP]) {
				g_Buttons[BTN_UP] = 0;
				state = S_INCHOUR;
			}
			else if(g_Buttons[BTN_DOWN]) {
				g_Buttons[BTN_DOWN] = 0;
				state = S_DECHOUR;
			}
			break;

	case I_SETMIN:
			g_RtcMode = IRTC_SETUP;
			DisplayTime(hour * 3600 + min * 60);

#ifdef USING_LCD
			LCD_SetCoursor(4);
#endif

			t0 = t1;
			state = S_SETMIN;
			break;

	case S_SETMIN:
			if(t1 - t0 >= RTC_TIMEOUT) {
				state = S_CANCEL;
			}
			if(g_Buttons[BTN_SET]) {
				g_Buttons[BTN_SET] = 0;
				state = S_UPDATE;
			}
			else if(g_Buttons[BTN_UP]) {
				g_Buttons[BTN_UP] = 0;
				state = S_INCMIN;
			}
			else if(g_Buttons[BTN_DOWN]) {
				g_Buttons[BTN_DOWN] = 0;
				state = S_DECMIN;
			}
			break;

	case S_INCHOUR:
			if(++hour >= 24)
				hour = 0;

			state = I_SETHOUR;
			break;

	case S_DECHOUR:
			if(--hour < 0)
				hour = 23;

			state = I_SETHOUR;
			break;

	case S_INCMIN:
			if(++min >= 60)
				min = 0;

			state = I_SETMIN;
			break;

	case S_DECMIN:
			if(--min < 0)
				min = 59;

			state = I_SETMIN;
			break;

	case S_UPDATE:
			set_TimeCount(hour * 3600 + min * 60);
			state = I_WORK;
			break;

	case S_CANCEL:
			state = I_WORK;
			break;
	}
}


void DisplayTime(uint32_t tmVal)
{
	int hour, min, sec;

	hour = (tmVal / 3600) % 24;
	min = (tmVal / 60) % 60;
	sec = tmVal % 60;

#ifdef USING_LCD
	printf("\r%02d:%02d:%02d", hour, min, sec);
#else
	UART_printf("\r%02d:%02d:%02d", hour, min, sec);
#endif
}


void Task_Time(void)
{
	RTC_TimeTypeDef currentTime = {0};
	RTC_DateTypeDef currentDate = {0};

	HAL_RTC_GetTime(&hrtc, &currentTime, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &currentDate, RTC_FORMAT_BCD);

	UART_printf("\r%02d:%02d:%02d", currentTime.Hours, currentTime.Minutes, currentTime.Seconds);

	// Geçen süreyi hesapla (saniye cinsinden)
	// uint32_t elapsedSeconds = (currentTime.Hours * 3600 + currentTime.Minutes * 60 + currentTime.Seconds) -
	// 							(startTime.Hours * 3600 + startTime.Minutes * 60 + startTime.Seconds);

	// UART_printf("Elapsed time: %lu seconds\r", elapsedSeconds);
}

void Task_RTC_Time(void)
{
	if((g_RtcMode != IRTC_SETUP) && g_RtcChanged) {
		// printf("\nTime:%9d", ++g_Time);
		UART_printf("  Time:%9d\n", ++g_Time);
		g_RtcChanged = 0;
	}
}

void Task_RTC_Alarm(void)
{
	static unsigned nAlarms;
	if(g_RtcAlarm) {
		UART_printf("\n(%u)\n", ++nAlarms);
		g_RtcAlarm = 0;
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

#ifdef USING_LCD
	printf("SAYI:%10u\r", ++count);
#else
	UART_printf("\rCount: %10lu", ++count);
#endif
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t *file, uint32_t line)
{
}
#endif /* USE_FULL_ASSERT */
