
#ifndef __UART_H
#define __UART_H

#define EMPTY 	0xFFFF
#define TIMEOUT 0xFFFF

#define COM_UART_bps    38400
#define LOG_UART		USART3
#define LOG_UART_bps    115200
	
	

#define Default_MODBUS_INTERVAL_TIME 3  //���崮�����������ռ��ʱ������ֵ,3ms
#define Default_USART2_INTERVAL_TIME 3  //���崮�����������ռ��ʱ������ֵ,3ms
#define Default_USART3_INTERVAL_TIME 3  //���崮�����������ռ��ʱ������ֵ,3ms

#define UART1_RBUF_SIZE   1024		//Ҫ��2 ��������
#if UART1_RBUF_SIZE < 2
#error UART1_RBUF_SIZE is too small.  It must be larger than 1.
#elif ((UART1_RBUF_SIZE & (UART1_RBUF_SIZE-1)) != 0)
#error UART1_RBUF_SIZE must be a power of 2.
#endif

//#define UART2_RBUF_SIZE   256		//Ҫ��2 ��������
//#if UART2_RBUF_SIZE < 2
//#error UART2_RBUF_SIZE is too small.  It must be larger than 1.
//#elif ((UART2_RBUF_SIZE & (UART2_RBUF_SIZE-1)) != 0)
//#error UART2_RBUF_SIZE must be a power of 2.
//#endif

//#define UART3_RBUF_SIZE   256		//Ҫ��2 ��������
//#if UART3_RBUF_SIZE < 2
//#error UART3_RBUF_SIZE is too small.  It must be larger than 1.
//#elif ((UART3_RBUF_SIZE & (UART3_RBUF_SIZE-1)) != 0)
//#error UART3_RBUF_SIZE must be a power of 2.
//#endif

typedef struct uart1_rbuf_st
{
	unsigned int in;							//����
	unsigned int out;							//���
	unsigned char  buf [UART1_RBUF_SIZE];		//�������ռ�
}UART1_RBUF_ST;

//typedef struct uart2_rbuf_st
//{
//	unsigned int in;							//����
//	unsigned int out;							//���
//	unsigned char  buf [UART2_RBUF_SIZE];		//�������ռ�
//}UART2_RBUF_ST;

//typedef struct uart3_rbuf_st
//{
//	unsigned int in;							//����
//	unsigned int out;							//���
//	unsigned char  buf [UART3_RBUF_SIZE];		//�������ռ�
//}UART3_RBUF_ST;

extern unsigned char    Uart1Flag;
extern UART1_RBUF_ST	uart1_rbuf;

//extern unsigned char    Uart2Flag;
//extern UART2_RBUF_ST	uart2_rbuf;

//extern unsigned char    Uart3Flag;
//extern UART3_RBUF_ST	uart3_rbuf;

extern volatile unsigned char ModbusIntervalTime;

extern volatile unsigned char USART1_INTERVAL_TIME;
extern volatile unsigned char USART1_RX_Len;
//extern volatile unsigned char USART2_INTERVAL_TIME;
//extern volatile unsigned char USART2_RX_Len;
//extern volatile unsigned char USART3_INTERVAL_TIME;
//extern volatile unsigned char USART3_RX_Len;

//extern unsigned char USART1_RX[256];
//extern unsigned char USART2_RX[256];
//extern unsigned char USART3_RX[256];	

//void UART1TX_DMA_Configuration( void );
//void UART1_DMA_SendData(unsigned char *BufAddress,unsigned int Length);
void UART1_Configuration(unsigned int baudrate);
//void UART2_Configuration(unsigned int baudrate);
void UART3_Configuration(unsigned int baudrate);
void NVIC_UART_Configuration(void);
void USART_PutChar(USART_TypeDef* USARTx,unsigned char ch);
void USART_PutData(USART_TypeDef* USARTx,unsigned char *dat,unsigned  int len);
void USART_PutS(USART_TypeDef* USARTx,unsigned char *s);
unsigned short int USART1_GetCharBlock(unsigned short int timeout);
unsigned short int USART1_GetChar(void);
//unsigned short int USART2_GetCharBlock(unsigned short int timeout);
//unsigned short int USART2_GetChar(void);
void USART1_ClearBuf_Flag(void);
//void USART2_ClearBuf_Flag(void);
//void USART3_ClearBuf_Flag(void);

void U485SendData(unsigned char *dat,unsigned short len);
void U485_2SendData(unsigned char *dat,unsigned short len);




#endif


