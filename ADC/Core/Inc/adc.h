#ifndef INC_ADC_H_
#define INC_ADC_H_


extern volatile int g_bEOC;		// End of Conversion flag, like semaphore

void IADC_IoInit(int idx);
void IADC_Init(int ch);
void IADC_Start(void);
float IADC_Convert(void);
int IADC_Result(void);
void IADC_IntConfig(void);



#endif /* INC_ADC_H_ */
