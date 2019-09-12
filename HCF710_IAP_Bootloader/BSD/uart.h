
#ifndef __UART_H
#define __UART_H

#define EMPTY 	0xFFFF
#define TIMEOUT 0xFFFF

#define UART1_RBUF_SIZE   512		//要求：2 的整次幂
#if UART1_RBUF_SIZE < 2
#error UART1_RBUF_SIZE is too small.  It must be larger than 1.
#elif ((UART1_RBUF_SIZE & (UART1_RBUF_SIZE-1)) != 0)
#error UART1_RBUF_SIZE must be a power of 2.
#endif


typedef struct uart1_rbuf_st
{
	unsigned int in;							//输入
	unsigned int out;							//输出
	unsigned char  buf [UART1_RBUF_SIZE];		//缓冲区空间
}UART1_RBUF_ST;


extern unsigned char    Uart1Flag;
extern UART1_RBUF_ST	uart1_rbuf;


extern volatile unsigned char USART1_INTERVAL_TIME;
extern volatile unsigned char USART1_RX_Len;

void UART1_Configuration(unsigned int baudrate);

void NVIC_UART_Configuration(void);
void USART_PutChar(USART_TypeDef* USARTx,unsigned char ch);
void USART_PutData(USART_TypeDef* USARTx,unsigned char *dat,unsigned  int len);
void USART_PutS(USART_TypeDef* USARTx,unsigned char *s);
unsigned short int USART1_GetCharBlock(unsigned short int timeout);
unsigned short int USART1_GetChar(void);

void USART1_ClearBuf_Flag(void);

void U485SendData(unsigned char *dat,unsigned short len);
void U485_2SendData(unsigned char *dat,unsigned short len);


#endif


