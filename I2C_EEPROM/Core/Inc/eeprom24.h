#ifndef INC_EEPROM24_H_
#define INC_EEPROM24_H_

#define I2C_ADDR_24XX	0xA0

#define PGSIZEMAX		512
#define ADSIZEMAX		4

void E24_Config(int adSize, int pgSize);

int E24_WritePage(int chipAddr, unsigned long offset, const void *buffer, int size);


int E24_Read(int chipAddr, unsigned long offset, void *buffer, int size);
int E24_Write(int chipAddr, unsigned long offset, const void *buffer, int size);

int E24_WriteString(int chipAddr, unsigned long offset, const void *buffer);




#endif /* INC_EEPROM24_H_ */
