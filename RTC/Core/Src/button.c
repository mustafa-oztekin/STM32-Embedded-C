#include <stdio.h>
#include <stdint.h>

#include "button.h"
#include "io.h"

int g_dbMax = 50;

int _bScan = 0;			// Tarama başlangıç flag değeri

static BTN_PIN _bts[] = {
		{ IOP_BTN_SET, 1, 0, 0 },
		{ IOP_BTN_UP, 1, 0, 0 },
		{ IOP_BTN_DOWN, 1, 0, 0 },

};

#define		N_BUTTONS		(sizeof(_bts) / sizeof(BTN_PIN))

unsigned g_Buttons[N_BUTTONS];		// Button Semaphore (binary / counting)


// Her "button timer" tick ile çağrılır
// Tek bir butonun tarama işlemini yapar
static void BTN_Scan(int btIdx)
{
	int r; // pin okuma değeri

	r = IO_Read(_bts[btIdx].ioIdx);

	if(r != _bts[btIdx].cState) {
		if(++_bts[btIdx].dbCount >= g_dbMax) {
			// başarı sayısına ulaşıldı: durum değiştir
			_bts[btIdx].cState = r;

			_bts[btIdx].dbCount = 0;

			if(_bts[btIdx].cState == _bts[btIdx].aState) {
				// Signal !!!!
				// g_Buttons[btIdx] = 1;	// binary semaphore
				++g_Buttons[btIdx];			// counting semaphore
			}
		}
	}
	else {
		// max başarı sayısına ulaşılamadan hata geldi
		// sayacı sıfırlıyoruz
		_bts[btIdx].dbCount = 0;
	}
}

void BTN_InitButtons(void)
{
	int i;
	for(i = 0; i < N_BUTTONS; ++i)
		IO_Init(_bts[i].ioIdx, IO_MODE_INPUT, GPIO_PULLUP);

	_bScan = 1;
}

void BTN_ScanButtons(void)
{
	if(!_bScan)
		return;

	int i;
	for(i = 0; i < N_BUTTONS; ++i)
		BTN_Scan(i);
}

