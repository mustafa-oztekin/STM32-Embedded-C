#include <stdio.h>
#include <stdint.h>
#include "system.h"
#include "irtc.h"
#include "uart.h"
#include "stm32f4xx.h"


RTC_HandleTypeDef hrtc;
volatile int g_Time = 0;
volatile int g_RtcAlarm = 0;
volatile int g_RtcChanged = 0;


void MX_RTC_Init(void)
{
  RTC_TimeTypeDef sTime = {0};
  RTC_DateTypeDef sDate = {0};

  hrtc.Instance = RTC;
  hrtc.Init.HourFormat = RTC_HOURFORMAT_24;
  hrtc.Init.AsynchPrediv = 127;
  hrtc.Init.SynchPrediv = 255;
  hrtc.Init.OutPut = RTC_OUTPUT_DISABLE;
  hrtc.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  hrtc.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  if (HAL_RTC_Init(&hrtc) != HAL_OK)
  {
    Error_Handler();
  }

  sTime.Hours = 0;
  sTime.Minutes = 0;
  sTime.Seconds = 0;
  sTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sTime.StoreOperation = RTC_STOREOPERATION_RESET;
  if (HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
  sDate.WeekDay = RTC_WEEKDAY_MONDAY;
  sDate.Month = RTC_MONTH_OCTOBER;
  sDate.Date = 9;
  sDate.Year = 24;

  if (HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD) != HAL_OK)
  {
    Error_Handler();
  }
}

uint32_t get_TimeCount(void)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	HAL_RTC_GetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	HAL_RTC_GetDate(&hrtc, &sDate, RTC_FORMAT_BCD);

	// UART_printf("Time: %02d:%02d:%02d\r", sTime.Hours, sTime.Minutes, sTime.Seconds);

	// Geçen süreyi hesapla (saniye cinsinden)
	//uint32_t elapsedSeconds = (sTime.Hours * 3600 + sTime.Minutes * 60 + sTime.Seconds) -
	// 							(startTime.Hours * 3600 + startTime.Minutes * 60 + startTime.Seconds);

	// UART_printf("Elapsed time: %lu seconds\n", elapsedSeconds);

	uint32_t totalSeconds = (sTime.Hours * 3600 + sTime.Minutes * 60 + sTime.Seconds);
	return totalSeconds;
}



void set_TimeCount(uint32_t tmVal)
{
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	sTime.Hours = (tmVal / 3600) % 24;
	sTime.Minutes = (tmVal / 60) % 60;

	HAL_RTC_SetTime(&hrtc, &sTime, RTC_FORMAT_BCD);
	HAL_RTC_SetDate(&hrtc, &sDate, RTC_FORMAT_BCD);


	// uint32_t totalSeconds = (sTime.Hours * 3600 + sTime.Minutes * 60 + sTime.Seconds);
	// return totalSeconds;
}

// RTC ve Wakeup Timer'ı başlat
void IRTC_WakeUpIntConfig(void)
{
	if (HAL_RTCEx_SetWakeUpTimer_IT(&hrtc, 1999, RTC_WAKEUPCLOCK_RTCCLK_DIV16) != HAL_OK)
	{
		Error_Handler();
	}
	HAL_NVIC_SetPriority(RTC_WKUP_IRQn, 0x0F, 1);
	HAL_NVIC_EnableIRQ(RTC_WKUP_IRQn);
}

// Wakeup Timer kesme fonksiyonu (callback)
void RTC_WKUP_IRQHandler(void)
{
	g_RtcChanged = 1;
	HAL_RTCEx_WakeUpTimerIRQHandler(&hrtc);
}


void IRTC_AlrmIntConfig(void)
{
	RTC_AlarmTypeDef sAlarm = {0};

	// Enable the Alarm A
	sAlarm.AlarmTime.Hours = 0;
	sAlarm.AlarmTime.Minutes = 0;
	sAlarm.AlarmTime.Seconds = 20;
	sAlarm.AlarmTime.SubSeconds = 0;
	sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
	sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
	sAlarm.AlarmMask = RTC_ALARMMASK_HOURS | RTC_ALARMMASK_MINUTES;
	sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;
	sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
	sAlarm.AlarmDateWeekDay = 9;
	sAlarm.Alarm = RTC_ALARM_A;
	if (HAL_RTC_SetAlarm_IT(&hrtc, &sAlarm, RTC_FORMAT_BCD) != HAL_OK)
	{
	Error_Handler();
	}
	HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0x03, 0);
	HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}


void RTC_Alarm_IRQHandler(void)
{
	g_RtcAlarm = 1;
	HAL_RTC_AlarmIRQHandler(&hrtc);
}
