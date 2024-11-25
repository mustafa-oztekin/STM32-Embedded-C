#include <stdio.h>
#include <stdint.h>
#include "system.h"
#include "io.h"
#include "1-Wire.h"
#include "crc8.h"

enum { False, True };


#define OneWire_SetByte(n)	OneWire_Byte(n)
#define OneWire_GetByte()	OneWire_Byte(0xFF)

void OneWire_Init(void)
{
	// DQ hattını open-drain 1 (High-Z) olarak başlatacağız
	IO_WRITE(IOP_DQ, 1);	// başlangıçta idle durum 1-High
	IO_Init(IOP_DQ, IO_MODE_OUTPUT_OD);
}


// 1-Wire bus reset pulse oluşturur, slave(s) presence pulse
// cevabına geri döner
// True: Slave var ve hazır
// False: Slave yok ya da hazır değil
int OneWire_Reset(void)
{
	int timeOut;

	// Critical Section Yönetimi yapılmadan 1-Wire Bus yapılmaz
	// 1 us bekleme yapmamız gerekiyor ama araya kesme girerse bekleme 50 us olur
	// ve hattaki verilerimiz bozulur.
	Sys_EnterCritical();	// Kesmeler kapalı

	IO_WRITE(IOP_DQ, 0);	// DQ = 0
	DelayUs(500);			// Reset pulse

	// Hattı bırakacağız
	// Open-Drain Output'ta 1 yazmak demek High-Z yapmak demektir
	// Dışarıdaki pull-up'tan dolayı 1'e çıkacak
	// aslında ben hattı bırakmış oluyorum
	IO_WRITE(IOP_DQ, 1);	// DQ = High-Z -> pull-up 1

	DelayUs(10);

	timeOut = 250;
	do {
		if(!IO_Read(IOP_DQ))
			break;

		DelayUs(1);
	} while(--timeOut);

	if(timeOut == 0)
		goto END;

	timeOut = 250;
	do {
		if(IO_Read(IOP_DQ))
			break;

		DelayUs(1);
	} while(--timeOut);

END:
	Sys_ExitCritical();
	return (timeOut != 0);
}

// 1-Wire Bus üzerinden 1 bit veri gönderir
// Master örnekleme değerine (okuma) geri döner
int OneWire_Bit(int val)
{
	Sys_EnterCritical();

	IO_WRITE(IOP_DQ, 0);	// DQ = 0
	DelayUs(1);				// Clock Pulse
	// yaklaşık 100 cycle (instruction) bekleme, böyle bir beklemeyi
	// ya timer'ı polling yaparak ya da DelayUs gibi bekleme fonksiyonu ile yapılabilir!!!
	// kesme ile böyle bir bekleme yapılamaz
	// çünkü kesmeye girip çıkarken flag kontolü yaparken bu süreyi aşabiliriz!!!

	if(val)
		IO_WRITE(IOP_DQ, 1);	// DQ = High-Z

	DelayUs(13);

	val = IO_Read(IOP_DQ);		// Master Samples

	DelayUs(46);

	IO_WRITE(IOP_DQ, 1);		// DQ = High-Z

	DelayUs(1);

	Sys_ExitCritical();

	return val;
}


unsigned char OneWire_Byte(unsigned char val)
{
	int i, b;

	for(i = 0; i < 8; ++i) {
		b = OneWire_Bit(val & 1);	// en düşük anlamlı bitin durumu
		val >>= 1;

		if(b)
			val |= 0x80;
	}

	return val;
}


int DS_ReadRom(unsigned char *buf)
{
	int i;

	if(!OneWire_Reset())
		return False;

	OneWire_SetByte(0x33);		// Read ROM

	// Slave 8-byte unique ID gönderir
	for(i = 0; i < 8; ++i)
		buf[i] = OneWire_GetByte();

	OneWire_Reset();
	return True;
}


/////////////////////////////////////////////////

// Koşul: Bus üzerinde tek DS18B20 var
int DS_ReadTemp(int *pTemp)
{
	unsigned char sp[9];
	int i, timeOut;

	if(pTemp == NULL)
		return False;

	/////////////////////////
	// 1. cümle

	if(!OneWire_Reset())
		return False;

	// ROM komutu
	OneWire_SetByte(0xCC);		// Skip ROM

	// İşlev komutu
	OneWire_SetByte(0x44);		// Conver T

	timeOut = 1000;

	do {
		if(IO_Read(IOP_DQ))
			break;
	} while(--timeOut);

	if(timeOut == 0) {
		OneWire_Reset();		// gerekli olmayabilir
		return False;
	}

	/////////////////////////
	// 2. cümle

	if(!OneWire_Reset())
		return False;

	// ROM komutu
	OneWire_SetByte(0xCC);		// Skip ROM

	// İşlev komutu
	OneWire_SetByte(0xBE);		// Read scratchpad

	for(i = 0; i < 9; ++i)
		sp[i] = OneWire_GetByte();

	OneWire_Reset();

	// Hata denetleme
	if(CRC8(sp, 9))
		return False;

	*pTemp = (int)(*(int16_t *)sp);
	return True;
}









