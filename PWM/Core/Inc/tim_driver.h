#ifndef INC_TIM_DRIVER_H_
#define INC_TIM_DRIVER_H_


#include <stdint.h>

extern volatile unsigned long g_T1Count;
extern volatile unsigned long g_T2Count;

void Timer1_Init(unsigned prescale, unsigned period);
void Timer1_Start(void);

void Timer2_Init(unsigned prescale, unsigned period);
void Timer2_Start(void);
void Timer2_Stop(void);

void PWM_Init(double freq, double duty);


#endif /* INC_TIM_DRIVER_H_ */
