#include "modul.h"
#include "io.h"

#include "stm32f4xx_hal.h"



// LCD modüle 4-bit senkron veri gönderir
// veri: karakter kodu veya komut kodu
// gönderilecek veri parametrenin düşük anlamlı 4-bitinde
static void LCD_SendDataL(unsigned char c)
{
	// 1) Data Setup (verinin hazırlanması)
	IO_WRITE(IOP_LCD_DB4, (c & 1) != 0);
	IO_WRITE(IOP_LCD_DB5, (c & 2) != 0);
	IO_WRITE(IOP_LCD_DB6, (c & 4) != 0);
	IO_WRITE(IOP_LCD_DB7, (c & 8) != 0);

	// 2) Clock Generation (saat / onay işareti gönderme)
	IO_WRITE(IOP_LCD_E, 1);
	DelayUs(3);
	IO_WRITE(IOP_LCD_E, 0);
	DelayUs(100);
}

// LCD modüle 4-bit senkron komut gönderir
static void LCD_SendCmdL(unsigned char c)
{
	IO_WRITE(IOP_LCD_RS, 0); // RS = 0 (komut)

	LCD_SendDataL(c); // 4-bit veri gönder
}

// LCD modüle 8-bit veri gönderir
// 2 adımda, önce yüksek 4-bit, sonra düşük 4-bit
static void LCD_SendData(unsigned char c)
{
	LCD_SendDataL(c >> 4); // yüksek anlamlı 4-bit
	// = operatörü kullanmadığımız için c'nin değeri değişmez!
	LCD_SendDataL(c);
}

// LCD modüle 8-bit komut gönderir
// 2 adımda, önce yüksek 4-bit, sonra düşük 4-bit
void LCD_SendCmd(unsigned char c)
{
	IO_WRITE(IOP_LCD_RS, 0); // RS = 0 (komut)
	LCD_SendData(c);
}

// LCD modüle 8-bit karakter gönderir
// 2 adımda, önce yüksek 4-bit, sonra düşük 4-bit
void LCD_PutChar(unsigned char c)
{
	IO_WRITE(IOP_LCD_RS, 1); // RS = 1 (karakter)
	LCD_SendData(c);		// 8-bit veri gönderiyoruz
}

// Ekranı temizle
void LCD_Clear(void)
{
	LCD_SendCmd(0x01);		// 0000-0001 -> display clear
	DelayMs(5);
}

void LCD_DisplayOn(unsigned char mode)
{
	LCD_SendCmd(0x08 | mode);
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
		LCD_PutChar(c);
		break;
	}
}


void LCD_Init(void)
{
	IO_Init(IOP_LCD_RS, IO_MODE_OUTPUT, GPIO_NOPULL);

	IO_WRITE(IOP_LCD_E, 0); // başlangıçta 0 olması için
	IO_Init(IOP_LCD_E, IO_MODE_OUTPUT, GPIO_NOPULL);

	IO_Init(IOP_LCD_DB4, IO_MODE_OUTPUT, GPIO_NOPULL);
	IO_Init(IOP_LCD_DB5, IO_MODE_OUTPUT, GPIO_NOPULL);
	IO_Init(IOP_LCD_DB6, IO_MODE_OUTPUT, GPIO_NOPULL);
	IO_Init(IOP_LCD_DB7, IO_MODE_OUTPUT, GPIO_NOPULL);

	DelayMs(100);

	// 4-bit arayüz ile başlatma
	// Function Set
	LCD_SendCmdL(0x03);		// 0011
	DelayMs(5);
	LCD_SendCmdL(0x03);		// 0011
	LCD_SendCmd(0x32);
	LCD_SendCmd(0x28);

	LCD_DisplayOn(0x00);		// Display On-Off
	LCD_Clear();
	LCD_SendCmd(0x06);
	LCD_DisplayOn(LCD_MODE_ON);
	DelayMs(2);
}



