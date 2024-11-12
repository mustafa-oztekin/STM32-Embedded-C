#ifndef INC_MODUL_H_
#define INC_MODUL_H_

#include <stdint.h>

extern void __delay(uint32_t ncy);

#define CLOCK_FREQ      100000000
#define INSTR_FREQ      (CLOCK_FREQ * 4 / 3)
#define US_CYCLES       (INSTR_FREQ / 1000000)
#define MS_CYCLES       (INSTR_FREQ / 1000)

#define DelayUs(us)     __delay((us) * US_CYCLES)
#define DelayMs(ms)     __delay((ms) * MS_CYCLES)

#define		LCD_MODE_BLINK		1
#define		LCD_MODE_CURSOR		2
#define		LCD_MODE_ON			4



void LCD_Init(void);
void LCD_PutChar(unsigned char c);
void LCD_SetCoursor(unsigned char pos);
void LCD_SendCmd(unsigned char c);
void LCD_DisplayOn(unsigned char mode);
void LCD_Clear(void);

void LCD_Putch(unsigned char c);


#endif /* INC_MODUL_H_ */
