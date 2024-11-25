#ifndef INC_1_WIRE_H_
#define INC_1_WIRE_H_


void OneWire_Init(void);
int OneWire_Reset(void);
int OneWire_Bit(int val);
int DS_ReadRom(unsigned char *buf);
int DS_ReadTemp(int *pTemp);


#endif /* INC_1_WIRE_H_ */
