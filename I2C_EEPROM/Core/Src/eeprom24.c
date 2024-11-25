#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "system.h"
#include "io.h"
#include "hI2C.h"
#include "eeprom24.h"

static int _AdSize = 2;
static int _PgSize = 8;

static unsigned char _I2CBuf[PGSIZEMAX + ADSIZEMAX];

enum { False, True };


void E24_Config(int adSize, int pgSize)
{
	if(adSize <= ADSIZEMAX)
		_AdSize = adSize;

	if(pgSize <= PGSIZEMAX)
		_PgSize = pgSize;
}

int E24_WritePage(int chipAddr, unsigned long offset, const void *buffer, int size)
{
	int i;

	if(size > _PgSize)
		return False;

	chipAddr <<= 1;

	for(i = _AdSize - 1; i >= 0; --i) {
		_I2CBuf[i] = (unsigned char)offset;
		offset >>= 8;
	}

	memcpy(_I2CBuf + _AdSize, buffer, size);

	if(!HI2C_Write(I2C1, I2C_ADDR_24XX + chipAddr, _I2CBuf, size + _AdSize))
		return False;

	// DelayMs(5);

	// Acknowledge Polling
	while(!HI2C_Write(I2C1, I2C_ADDR_24XX + chipAddr, NULL , 0)) ;

	return True;
}


int E24_Write(int chipAddr, unsigned long offset, const void *buffer, int size)
{
	int bSize;
	const unsigned char *ptr = (const unsigned char *)buffer;

	bSize = _PgSize - (offset % _PgSize);

	while(size > 0) {
		if(bSize > size)
			bSize = size;

		if(!E24_WritePage(chipAddr, offset, ptr, bSize))
			return False;

		offset += bSize;
		ptr += bSize;
		size -= bSize;

		bSize = _PgSize;
	}

	return True;
}




int E24_WriteString(int chipAddr, unsigned long offset, const void *buffer)
{
	int size;
	int bSize;
	const unsigned char *ptr = (const unsigned char *)buffer;

	// string uzunluğu hesaplama
	for(size = 0; ptr[size] != '\0'; ++size)
		;

	bSize = _PgSize - (offset % _PgSize);

	while(size > 0) {
		if(bSize > size)
			bSize = size;

		if(!E24_WritePage(chipAddr, offset, ptr, bSize))
			return False;

		offset += bSize;
		ptr += bSize;
		size -= bSize;

		bSize = _PgSize;
	}

	return offset;
}





int E24_Read(int chipAddr, unsigned long offset, void *buffer, int size)
{
	int i;

	for(i = _AdSize - 1; i >= 0; --i) {
		_I2CBuf[i] = (unsigned char)offset;
		offset >>= 8;
	}

	chipAddr <<= 1;

	if(!HI2C_Write(I2C1, I2C_ADDR_24XX + chipAddr, _I2CBuf, _AdSize))
		return False;

	if(!HI2C_Read(I2C1, I2C_ADDR_24XX + chipAddr, buffer, size))
		return False;

	return True;
}

