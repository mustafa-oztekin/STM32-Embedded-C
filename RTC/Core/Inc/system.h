#ifndef INC_SYSTEM_H_
#define INC_SYSTEM_H_

#include <stddef.h>

#define _STDIN	0
#define _STDOUT	1
#define _STDERR	2


void _putch(unsigned char c);
size_t _write(int handle, const unsigned char *buffer, size_t size);

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
void Sys_ConsoleInit(void);
void Error_Handler(void);



#endif /* INC_SYSTEM_H_ */
