#include <stdio.h>
#include <stdint.h>
#include "system.h"
#include "io.h"
#include "hI2C.h"
#include "LCDM.h"


void LCD_SendCmd(unsigned char cmd)
{
	unsigned char data_u, data_l;

	data_u = (cmd & 0xF0);
	data_l = ((cmd << 4) & 0xF0);

	uint8_t data_t[4];

	data_t[0] = data_u | 0x0C;	//en=1, rs=0 -> bxxxx1100
	data_t[1] = data_u | 0x08;	//en=0, rs=0 -> bxxxx1000
	data_t[2] = data_l | 0x0C;	//en=1, rs=0 -> bxxxx1100
	data_t[3] = data_l | 0x08;	//en=0, rs=0 -> bxxxx1000

	HI2C_Write(I2C1, LCD_ADDRESS, data_t, 4);
}


void LCD_SendCmdL(unsigned char cmd)
{
	unsigned char data_l;

	data_l = ((cmd << 4) & 0xF0);

	uint8_t data_t[2];

	data_t[0] = data_l | 0x0C;	//en=1, rs=0 -> bxxxx1100
	data_t[1] = data_l | 0x08;	//en=0, rs=0 -> bxxxx1000

	HI2C_Write(I2C1, LCD_ADDRESS, data_t, 2);
}


void LCD_DisplayOn(unsigned char mode)
{
	LCD_SendCmd(0x08 | mode);
}

// Ekranı temizle
void LCD_Clear(void)
{
	LCD_SendCmd(0x01);		// 0000-0001 -> display clear
	DelayMs(5);
}

void LCD_SetCoursor(unsigned char pos)
{
	LCD_SendCmd(0x80 | pos);
}

void LCD_Putch(unsigned char c)
{
	switch(c) {
	case '\r':
		LCD_SetCoursor(0);
		break;

	case '\n':
		LCD_SetCoursor(0x40);
		break;

	case '\f':
		LCD_Clear();
		break;

	default:
		LCD_SendData(c);
		break;
	}
}



int LCD_Puts(const char *str)
{
	int i = 0;

	while(str[i])
		LCD_Putch(str[i++]);

	return i;
}



void LCD_Init(void)
{
	// 4-bit initialization
	DelayMs(50);

	LCD_SendCmdL(0x03);
	DelayMs(5);
	LCD_SendCmdL(0x03);
	LCD_SendCmd(0x32);

	// display initialization
	LCD_SendCmd(0x28);

	LCD_DisplayOn(0x00);
	LCD_Clear();
	LCD_SendCmd(0x06);
}


void LCD_SendData(unsigned char data)
{
	unsigned char data_u, data_l;

	data_u = (data & 0xF0);
	data_l = ((data << 4) & 0xF0);

	uint8_t data_t[4];

	data_t[0] = data_u | 0x0D;  //en=1, rs=1 -> bxxxx1101
	data_t[1] = data_u | 0x09;  //en=0, rs=1 -> bxxxx1001
	data_t[2] = data_l | 0x0D;  //en=1, rs=1 -> bxxxx1101
	data_t[3] = data_l | 0x09;  //en=0, rs=1 -> bxxxx1001

	HI2C_Write(I2C1, LCD_ADDRESS, data_t, 4);
}

