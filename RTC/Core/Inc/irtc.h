#ifndef INC_IRTC_H_
#define INC_IRTC_H_

#include <stdint.h>

extern volatile int g_Time;
extern volatile int g_RtcAlarm;
extern volatile int g_RtcChanged;

void MX_RTC_Init(void);
uint32_t get_TimeCount(void);
void set_TimeCount(uint32_t tmval);
void IRTC_WakeUpIntConfig(void);
void IRTC_AlrmIntConfig(void);


#endif /* INC_IRTC_H_ */
