#ifndef INC_UART_H_
#define INC_UART_H_

enum {
	UART_1,
	UART_2,
	UART_6,
};

#define SZ_PRNBUF	256

void UART_Send(unsigned char val);
unsigned char UART_Recv(void);

int UART_DataReady(void);

void UART_putch(unsigned char c);
int UART_puts(const char *str);
int UART_printf(const char *fmt, ...);


#endif /* INC_UART_H_ */
