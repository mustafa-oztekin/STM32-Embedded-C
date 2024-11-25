#ifndef INC_SYSTEM_H_
#define INC_SYSTEM_H_

#include <stddef.h>


extern void __delay(unsigned long ncy);

#define CLOCK_FREQ      100000000
#define INSTR_FREQ      (CLOCK_FREQ * 4 / 3)
#define US_CYCLES       (INSTR_FREQ / 1000000)
#define MS_CYCLES       (INSTR_FREQ / 1000)

#define DelayUs(us)     __delay((us) * US_CYCLES)
#define DelayMs(ms)     __delay((ms) * MS_CYCLES)

// extern UART_HandleTypeDef huart2;

void Sys_EnterCritical(void);
void Sys_ExitCritical(void);

void _putch(unsigned char c);
size_t _write(int handle, const unsigned char *buffer, size_t size);

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);
void Sys_ConsoleInit(void);
void Error_Handler(void);



#endif /* INC_SYSTEM_H_ */
