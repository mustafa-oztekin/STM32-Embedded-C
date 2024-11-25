#ifndef INC_HI2C_H_
#define INC_HI2C_H_

#define I2C_WRITE	0
#define I2C_READ	1


/* --EV5 	Communication start */
#define	I2C_EVENT_MASTER_MODE_SELECT	((uint32_t)0x00030001)  /*BUSY, MSL and SB flag*/

/* --EV6 	Address Acknowledge */
#define	I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED	((uint32_t)0x00070082) /*BUSY,MSL,ADDR,TXE,TRA flags*/
#define	I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED		((uint32_t)0x00030002) /*BUSY,MSL,ADDR flags*/


/* Master TRANSMITTER mode --------------------------*/
/* --EV8_1 */
#define	I2C_EVENT_MASTER_BYTE_TRANSMITTING	((uint32_t)0x00070080) /*TRA, BUSY, MSL, TXE flags*/
/* --EV8_2 */
#define	I2C_EVENT_MASTER_BYTE_TRANSMITTED	((uint32_t)0x00070084)  /*TRA, BUSY, MSL, TXE and BTF flags*/

/* Master RECEIVER mode -----------------------------*/
/* --EV7 */
#define	I2C_EVENT_MASTER_BYTE_RECEIVED	((uint32_t)0x00030040)  /* BUSY, MSL and RXNE flags */




void HI2C_Init(int baudRate);
uint8_t HI2C_CheckEvent(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT);
int HI2C_Start(I2C_TypeDef *I2Cx, unsigned char ctl);
void HI2C_Stop(I2C_TypeDef *I2Cx);


// Generic Write - Read
int HI2C_Write(I2C_TypeDef *I2Cx, unsigned char devAddr, const void *buf, int size);
int HI2C_Read(I2C_TypeDef *I2Cx, unsigned char devAddr,void *buf, int size);

// Register Adres'e yazma ve okuma (IMU, RTC, vs.)
int HI2C_WriteA(I2C_TypeDef *I2Cx, unsigned char devAddr, unsigned char regAddr, const void *buf, int size);
int HI2C_ReadA(I2C_TypeDef *I2Cx, unsigned char devAddr, unsigned char regAddr, void *buf, int size);


void HI2C_Writex(uint8_t data);
void HI2C_WriteMulti(uint8_t *data, uint8_t size);



#endif /* INC_HI2C_H_ */
