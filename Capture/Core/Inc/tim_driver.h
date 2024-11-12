#ifndef INC_TIM_DRIVER_H_
#define INC_TIM_DRIVER_H_


#include <stdint.h>

extern volatile unsigned long g_T1Count, g_T2Count, g_T4Count;

void Timer1_Init(unsigned prescale, unsigned period);
void Timer1_Start(void);

void Timer2_Init(unsigned prescale, unsigned period);
void Timer2_IntConfig(int priority);
void Timer2_Start(void);

void Timer4_Init(unsigned prescale, unsigned period);
void Timer4_IntConfig(int priority);
void Timer4_Start(void);

void PWM_Init(double freq, double duty);
void Capture_Init(void);


#endif /* INC_TIM_DRIVER_H_ */
