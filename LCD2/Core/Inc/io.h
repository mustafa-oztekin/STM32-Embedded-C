#ifndef INC_IO_H_
#define INC_IO_H_

// #include "stm32f4xx_hal_gpio.h"

enum {
	IO_PORT_A,
	IO_PORT_B,
	IO_PORT_C,
	IO_PORT_D,
	IO_PORT_E,
	IO_PORT_F,
	IO_PORT_G,
};

typedef struct {
	int port;
	int pin;
} IO_PIN;


enum {
	IOP_LED,	// örnek bir led
	IOP_LCD_RS,
	IOP_LCD_E,
	IOP_LCD_DB4,
	IOP_LCD_DB5,
	IOP_LCD_DB6,
	IOP_LCD_DB7,
};

#ifdef _IOS_
IO_PIN _ios[] = {
		{ IO_PORT_D, 15 },
		{ IO_PORT_A, 0 },
		{ IO_PORT_A, 1 },
		{ IO_PORT_A, 2 },
		{ IO_PORT_A, 3 },
		{ IO_PORT_A, 4 },
		{ IO_PORT_A, 5 },

};

GPIO_TypeDef	*_GPIO_Ports[] = {
		GPIOA,
		GPIOB,
		GPIOC,
		GPIOD,
		GPIOE,
};
#endif

void IO_Init(int idx);
void IO_WRITE(int idx, int val);
int IO_Read(int idx);


#endif /* INC_IO_H_ */
