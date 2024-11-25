#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>

#include "stm32f4xx.h"
#include "system.h"
#include "io.h"
#include "uart.h"

UART_HandleTypeDef		huart2;

void UART_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* Peripheral clock enable */
	__HAL_RCC_USART2_CLK_ENABLE();

	/**USART2 GPIO Configuration
	PA2     ------> USART2_TX
	PA3     ------> USART2_RX
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF7_USART2;

	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// UART2 için başlangıç parametreleri
	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;

	if (HAL_UART_Init(&huart2) != HAL_OK) {
		Error_Handler();
	}
}





void UART_Send(unsigned char val)
{
	// 1) TSR dolu olduğu müddetçe bekle
	while((USART2->SR & USART_SR_TXE) == 0)
		;
	// 2) Yüklemeyi yap
	USART2->DR = val;
}


int UART_DataReady()
{
	return (USART2->SR & USART_SR_RXNE);
}


unsigned char UART_Recv(void)
{
	while(!UART_DataReady())
		;

	return (unsigned char)USART2->DR;
}

void UART_putch(unsigned char c)
{
	if(c == '\n')
		UART_Send('\r');

	UART_Send(c);
}

int UART_puts(const char *str)
{
	int i = 0;

	while(str[i])
		UART_putch(str[i++]);

	return i;
}

int UART_printf(const char *fmt, ...)
{
	va_list args;
	char str[SZ_PRNBUF];

	va_start(args, fmt);
	vsnprintf(str, SZ_PRNBUF, fmt, args);

	return UART_puts(str);
}



