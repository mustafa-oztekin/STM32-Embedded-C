#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "system.h"
#include "io.h"
#include "hI2C.h"
#include "LCDM.h"


static unsigned char _LcdBuf[LCD_BUFFER_SIZE];

enum { False, True };


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


void LCD_SetCoursor(unsigned char pos)
{
	LCD_SendCmd(0x80 | pos);
}


void LCD_SetCursor_forWrite(unsigned char cmd, int *k)
{
	unsigned char data_u, data_l;

	data_u = (cmd & 0xF0);
	data_l = ((cmd << 4) & 0xF0);

    _LcdBuf[*k]     = data_u | 0x0C;  // en=1, rs=0
    _LcdBuf[*k + 1] = data_u | 0x08;  // en=0, rs=0
    _LcdBuf[*k + 2] = data_l | 0x0C;  // en=1, rs=0
    _LcdBuf[*k + 3] = data_l | 0x08;  // en=0, rs=0

    *k += 4;  // Buffer indeksini 4 artır
}


void LCD_SendData_forWrite(unsigned char data, int *k)
{
	unsigned char data_u, data_l;

	data_u = (data & 0xF0);
	data_l = ((data << 4) & 0xF0);

	_LcdBuf[*k]		=	data_u | 0x0D;	//en=1, rs=1
	_LcdBuf[*k + 1]	=	data_u | 0x09;	//en=0, rs=1
	_LcdBuf[*k + 2]	=	data_l | 0x0D;	//en=1, rs=1
	_LcdBuf[*k + 3]	=	data_l | 0x09;	//en=0, rs=1

    *k += 4;  // Buffer indeksini 4 artır
}


// LCD için tek cümle yazma kalıbı
// 16x2 LCD için 16 karakter yazar
// Veri ve komut yazar
// LCD_Putch fonksiyonunun güncel versiyonu
// LCD_Putch 1 byte gönderirken sürekli start ve stop ekliyordu
// bu fonksiyonda ise tüm verileri tek bir bufferda dizip tek seferde gönderdim
void LCD_Write(int chipAddr, const void *buffer)
{
	int size, i, k = 0;
	unsigned char data_u, data_l;
	const unsigned char *ptr = (const unsigned char *)buffer;
	// string uzunluğu hesaplama		size = strlen((const unsigned char *)buffer);
	for(size = 0; ptr[size] != '\0'; ++size)
		;

	for(i = 0; i < size; ++i) {
		switch(ptr[i]) {
		case '\r':
			LCD_SetCursor_forWrite(0x80, &k);
			break;

		case '\n':
			LCD_SetCursor_forWrite(0xC0, &k);
			break;

		default:
			LCD_SendData_forWrite(ptr[i], &k);
			break;
		}
	}

	// LCD veya buffer taşma kontrolü yapılabilir
	// ama 16x2 LCD Display'e yazdığım için maks uzunluğu 16 olarak giriyorum
	HI2C_Write(I2C1, LCD_ADDRESS, _LcdBuf, k);
}
