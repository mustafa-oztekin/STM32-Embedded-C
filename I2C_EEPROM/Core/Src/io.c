#include <stdio.h>
#include <stdint.h>

#include "stm32f4xx.h"

#define _IOS_
#include "io.h"



void IO_Init(int idx, int mode)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	int port;

	GPIO_InitStructure.Mode = mode;
	GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_HIGH;
	GPIO_InitStructure.Pin = (1 << _ios[idx].pin);

	port = _ios[idx].port;
	HAL_GPIO_Init(_GPIO_Ports[port], &GPIO_InitStructure);
}

void IO_WRITE(int idx, int val)
{
	int port;

	port = _ios[idx].port;
	if(val)
		_GPIO_Ports[port]->BSRR |= (1 << _ios[idx].pin);
	else
		_GPIO_Ports[port]->BSRR |= ((1 << _ios[idx].pin) << 16);

}


int IO_Read(int idx)
{
	int port;

	port = _ios[idx].port;

	return _GPIO_Ports[port]->IDR & (1 << _ios[idx].pin);
}


void IO_Toggle(int idx)
{
	int port;
	port = _ios[idx].port;

	_GPIO_Ports[port]->ODR ^= (1 << _ios[idx].pin);
}