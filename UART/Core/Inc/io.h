#ifndef INC_IO_H_
#define INC_IO_H_

#include "stm32f4xx.h"

enum {
	IO_PORT_A,
	IO_PORT_B,
	IO_PORT_C,
	IO_PORT_D,
	IO_PORT_E,
	IO_PORT_F,
	IO_PORT_G,
};


enum {
  IO_MODE_INPUT = GPIO_MODE_INPUT,
  IO_MODE_OUTPUT = GPIO_MODE_OUTPUT_PP,
  IO_MODE_ALTERNATE = GPIO_MODE_AF_PP,
  IO_MODE_ANALOG = GPIO_MODE_ANALOG,

};



typedef struct {
	int port;
	int pin;
} IO_PIN;


enum {
	// board led
	IOP_LED,	// örnek bir led

	// lcd modul
	IOP_LCD_RS,
	IOP_LCD_E,
	IOP_LCD_DB4,
	IOP_LCD_DB5,
	IOP_LCD_DB6,
	IOP_LCD_DB7,

	// uart pinleri
	IOP_U1_RX,
	IOP_U1_TX,
	IOP_U2_RX,
	IOP_U2_TX,
	IOP_U6_RX,
	IOP_U6_TX,
};

#ifdef _IOS_
IO_PIN _ios[] = {
		{ IO_PORT_D, 15 },

		// LCD MODUL
		{ IO_PORT_A, 0 },
		{ IO_PORT_A, 1 },
		{ IO_PORT_A, 2 },
		{ IO_PORT_A, 3 },
		{ IO_PORT_A, 4 },
		{ IO_PORT_A, 5 },

		// UART I/O Pinleri
		{ IO_PORT_A, 10 },
		{ IO_PORT_A, 9 },
		{ IO_PORT_A, 3 },
		{ IO_PORT_A, 2 },
		{ IO_PORT_C, 7 },
		{ IO_PORT_C, 6 },
};

GPIO_TypeDef	*_GPIO_Ports[] = {
		GPIOA,
		GPIOB,
		GPIOC,
		GPIOD,
		GPIOE,
};
#endif

void IO_Init(int idx, int mode);
void IO_WRITE(int idx, int val);
int IO_Read(int idx);


#endif /* INC_IO_H_ */
