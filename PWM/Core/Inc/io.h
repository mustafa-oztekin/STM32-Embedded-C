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


	// push button pinleri
	IOP_BTN_SET,
	IOP_BTN_UP,
	IOP_BTN_DOWN,

	// pwm
	IOP_TIM2_CH2,
};

#ifdef _IOS_
IO_PIN _ios[] = {
		{ IO_PORT_D, 15 },

		// LCD MODUL
		{ IO_PORT_D, 0 },
		{ IO_PORT_D, 1 },
		{ IO_PORT_D, 2 },
		{ IO_PORT_D, 3 },
		{ IO_PORT_D, 4 },
		{ IO_PORT_D, 6 },

		// push buttons
		{ IO_PORT_A, 8 },
		{ IO_PORT_A, 15 },
		{ IO_PORT_A, 10 },

		// pwm port, pin
		{ IO_PORT_A, 1 },
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
void IO_Toggle(int idx);


#endif /* INC_IO_H_ */
