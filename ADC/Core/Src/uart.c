#include <stdio.h>
#include <stdint.h>
#include "io.h"
#include "uart.h"
#include "stm32f4xx.h"
#include <stdarg.h>


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



