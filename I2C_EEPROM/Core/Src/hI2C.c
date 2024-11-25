#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "system.h"
#include "io.h"
#include "hI2C.h"



#define HI2C_TIMEOUT    250
// #define HI2C_PGSIZE		1

enum { False, True };

I2C_HandleTypeDef	hi2c1;


void HI2C_Init(int baudRate)
{
	// I2C1 SCL ve SDA Pinleri (PB6 ve PB7) yapılandırılacak
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;		// Open-drain alternatife ayarla
	GPIO_InitStruct.Pull = GPIO_NOPULL;			// Pull-up/down yok
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;	// I2C1 için alternatif fonksiyon

	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// I2C1 için saat sinyalini etkinleştir
	__HAL_RCC_I2C1_CLK_ENABLE();

	// I2C başlangıç parametreleri
	hi2c1.Instance = I2C1;						// I2C1 modülünü kullan
	hi2c1.Init.ClockSpeed = baudRate;			// 100 kHz standart hız
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;		// Duty cycle 1:1
	hi2c1.Init.OwnAddress1 = 0x00;				// Ana cihaz adresi (master mod)
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;	// 7-bit adresleme
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;	// İkili adresleme devre dışı
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;	// Genel çağrı devre dışı
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;		// Clock stretching etkin

	if(HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
}


// I2C olaylarını kontrol etmek için geri dönüşlü bir fonksiyon
uint8_t HI2C_CheckEvent(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT)
{
    uint32_t lastevent = 0;
    uint32_t flag1 = 0, flag2 = 0;

    // Status register 1 ve 2'yi oku
    flag1 = I2Cx->SR1;
    flag2 = I2Cx->SR2;
    flag2 = flag2 << 16;

    // İki register'ı birleştirerek tam olayı elde et
    lastevent = (flag1 | flag2) & 0x00FFFFFF;

    // İstenen olay oluşmuşsa 1, aksi halde 0 döndür
    if((lastevent & I2C_EVENT) == I2C_EVENT) {
        return 1;		// Olay oluşmuş
    } else {
    	return 0;		// Olay oluşmamış
    }
}

static int HI2C_Event(I2C_TypeDef *I2Cx, int event)
{
	unsigned long tmOut = HI2C_TIMEOUT;

	do{
		if (HI2C_CheckEvent(I2Cx, event) == 1)
			break;
	}while(--tmOut);

	return (tmOut != 0);
}

// Start koşulu oluşturur
int HI2C_Start(I2C_TypeDef *I2Cx, unsigned char ctl)
{
	int status;

	I2Cx->CR1 |= I2C_CR1_START;		// start condition

	if(!HI2C_Event(I2Cx, I2C_EVENT_MASTER_MODE_SELECT))	// wait for SB bit to set	(EV5)
		return False;

	I2Cx->DR = ctl;				// send the address (kontrol byte)

	status = HI2C_Event(I2Cx, ctl & 1 ?
	I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED :
	I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);			// wait for the ADDR bit to set	(EV6)

	if (status != 1)
		I2Cx->CR1 |= I2C_CR1_STOP;		// stop condition

	return status;
}


// Stop koşulu oluşturur
void HI2C_Stop(I2C_TypeDef *I2Cx)
{
	I2Cx->CR1 |= I2C_CR1_STOP;		// stop condition
}


// 8-bit veri gönderir
int HI2C_Send(I2C_TypeDef *I2Cx, unsigned char val)
{
	if(!HI2C_Event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTING))	// wait for TXE bit to set	(EV8_1)
		return False;

	I2Cx->DR = val;

	return HI2C_Event(I2Cx, I2C_EVENT_MASTER_BYTE_TRANSMITTED); //wait for TXE, BTF bits to set EV8_2
}


int HI2C_Recv(I2C_TypeDef *I2Cx, unsigned char *pVal)
{
	if(!HI2C_Event(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED))	// wait for RxNE bit to set	(EV7)
		return False;

	*pVal = (uint8_t)I2Cx->DR;
	return True;
}

// Slave'e Ack gönderme veya yapılandırması
// ack: ACK biti lojik değeri 0: olumlu
void HI2C_Acknowledge(I2C_TypeDef *I2Cx, int ack)
{
	if(ack) {
		/* Disable the acknowledgement */
		I2Cx->CR1 &= (uint16_t)~((uint16_t)I2C_CR1_ACK);
	}
	else {
		/* Enable the acknowledgement */
		I2Cx->CR1 |= I2C_CR1_ACK;
	}
}



////////////////////////////////////////////////////////////////////////////////
// GENERIC I2C FUNCTIONS

// I2C Generic Write
// I2C üzerinden yazma cümlesi ile veri gönderir
// I2Cx: I2C Çevresel no
// devAddr: Slave device hardware 7-bit address (high 7-bit)
// buf: Gönderilecek verinin başlangıç adresi
// size: Gönderilecek veri uzunluğu (kontrol byte hariç)
int HI2C_Write(I2C_TypeDef *I2Cx, unsigned char devAddr, const void *buf, int size)
{
	const unsigned char *ptr = (const unsigned char *)buf;

	if(size < 0)
		return False;

	// devAddr &= 0xFE;	// EEPROM - 24LC512

	if(!HI2C_Start(I2Cx, devAddr))
		return False;

	while(size) {
		if(!HI2C_Send(I2Cx, *ptr++))
			break;

		--size;
	}
	HI2C_Stop(I2Cx);

	return !size;
}


// I2C Generic Read
// I2C üzerinden okuma cümlesi ile veri alır
// I2Cx: I2C Çevresel no
// devAddr: Slave device hardware 7-bit address (high 7-bit)
// buf: Alınacak verinin başlangıç adresi
// size: Alınacak veri uzunluğu (kontrol byte hariç)
int HI2C_Read(I2C_TypeDef *I2Cx, unsigned char devAddr, void *buf, int size)
{
	unsigned char *ptr = (unsigned char *)buf;
	int ack;

	if(size <= 0)
		return False;

	if(!HI2C_Start(I2Cx, devAddr | 1))	// okuma için kontrol byte = device address | 1
		return False;

	do{
		ack = (size == 1); // size 1'e kadar ack hep 0, en son size = 1 olduğunda ack = 1
		HI2C_Acknowledge(I2C1, ack);

		if(!HI2C_Recv(I2C1, ptr++))
			break;
	}while(--size);
	HI2C_Stop(I2C1);

	return !size;
}


////////////////////////////////////////////////////////////////////

#define SZ_I2CBUF		256

// 1 Byte kullanan register adres kullanan cihazlar için yazma
int HI2C_WriteA(I2C_TypeDef *I2Cx, unsigned char devAddr, unsigned char regAddr, const void *buf, int size)
{
	unsigned char i2cbuf[SZ_I2CBUF + 1];

	if(size > SZ_I2CBUF)
		return False;

	i2cbuf[0] = regAddr;
	memcpy(i2cbuf + 1, buf, size);

	return HI2C_Write(I2C1, devAddr, i2cbuf, size + 1);
}


// 1 Byte kullanan register adres kullanan cihazlar için okuma
int HI2C_ReadA(I2C_TypeDef *I2Cx, unsigned char devAddr, unsigned char regAddr, void *buf, int size)
{
	if(!HI2C_Write(I2C1, devAddr, &regAddr, 1))
		return False;

	if(!HI2C_Read(I2C1, devAddr, buf, size))
		return False;

	return True;
}




/////////////////////////////////////////////////////////////////////////
// eski koddan kalma silinecek ama önce karşılaştırılacak


// 1-byte veri gönderir
void HI2C_Writex(uint8_t data)
{
	while(!(I2C1->SR1 & (1 << 7)))		// wait for TXE bit to set
		;								// EV8_1
	I2C1->DR = data;
	while(!(I2C1->SR1 & (1 << 2)))		// EV8_2
		;
}

// n-byte veri gönderir
void HI2C_WriteMulti(uint8_t *data, uint8_t size)
{
	while(!(I2C1->SR1 & (1 << 7)))		// wait for TXE bit to set
		;								// EV8_1

	while(size) {
		while(!(I2C1->SR1 & (1 << 7)))		// wait for TXE bit to set
			;								// EV8_1
		I2C1->DR = (volatile uint32_t)*data++;
		size--;
	}

	while(!(I2C1->SR1 & (1 << 2)))		// EV8_2
		;
}
