#ifndef INC_LCDM_H_
#define INC_LCDM_H_

#define		LCD_ADDRESS			0x4E
#define		LCD_BUFFER_SIZE		64

#define		LCD_MODE_BLINK		1
#define		LCD_MODE_CURSOR		2
#define		LCD_MODE_ON			4

void LCD_SendCmd(unsigned char cmd);
void LCD_SendCmdL(unsigned char cmd);
void LCD_DisplayOn(unsigned char mode);
void LCD_Init(void);
void LCD_Clear(void);
void LCD_SetCoursor(unsigned char pos);
void LCD_SendData(unsigned char data);
void LCD_Putch(unsigned char c);
int LCD_Puts(const char *str);

void LCD_SetCursor_forWrite(unsigned char cmd, int *k);
void LCD_SendData_forWrite(unsigned char data, int *k);

void LCD_Write(int chipAddr, const void *buffer);


#endif /* INC_LCDM_H_ */
