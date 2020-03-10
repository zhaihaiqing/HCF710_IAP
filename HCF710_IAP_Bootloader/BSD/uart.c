/* Includes ------------------------------------------------------------------*/
#include "main.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#define UART_FLAG_TimeOut  0x50000   //超时常量

volatile unsigned char USART1_RX_Len=0;     //串口接收指令长度

unsigned char Uart1Flag;
UART1_RBUF_ST	uart1_rbuf	=	{ 0, 0, };


/**********************************************************************************
* Function Name  : UART1_Configuration
* 串口1初始化
* 入口参数
* baudrate:波特率
**********************************************************************************/
void UART1_Configuration(unsigned int baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable the USART1 Interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	//打开GPIO和USART1的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);  //使能串口1时钟
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_USART1);//端口重映射
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1);
	
	/* 将USART1_Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*  将USART1_Rx的GPIO配置为浮空输入模式
  	由于CPU复位后，GPIO缺省都是浮空输入模式，因此下面这个步骤不是必须的
  	但是，我还是建议加上便于阅读，并且防止其它地方修改了这个口线的设置参数 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;		//GPIO_Pin_7
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baudrate;//设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//设置数据位为8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//设置停止位为1
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控位
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//发送和接收
	USART_Init(USART1, &USART_InitStructure);
	
	//中断使能
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* 清发送标志，Transmission Complete flag */
	
	//使能
	USART_Cmd(USART1, ENABLE);
	
}

#ifdef  debug
void UART3_Configuration(unsigned int baudrate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* Enable the USART1 Interrupt */
//	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
	
	//打开GPIO和USART1的时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);  //使能串口1时钟
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);//端口重映射
	//GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
	
	/* 将USART1_Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*  将USART1_Rx的GPIO配置为浮空输入模式
  	由于CPU复位后，GPIO缺省都是浮空输入模式，因此下面这个步骤不是必须的
  	但是，我还是建议加上便于阅读，并且防止其它地方修改了这个口线的设置参数 */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		//GPIO_Pin_7
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baudrate;//设置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//设置数据位为8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//设置停止位为1
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件流控位
	USART_InitStructure.USART_Mode =   USART_Mode_Tx;//发送和接收
	USART_Init(USART3, &USART_InitStructure);
	
	//中断使能
//	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
//	USART_ClearFlag(USART3, USART_FLAG_TC);     /* 清发送标志，Transmission Complete flag */
	
	//使能
	USART_Cmd(USART3, ENABLE);
	
}
#endif
/**********************************************************************************
* Function Name  : USART_PutChar
* 串口发送一个字符
* 入口参数
* USARTx:串口号
* ch:数据
**********************************************************************************/
void USART_PutChar(USART_TypeDef* USARTx,unsigned char ch)
{
	unsigned int timeout = 0;
	USART_GetFlagStatus(USARTx, USART_FLAG_TC);//USARTx->SR  解决设备重启或上电，串口第一次发送数据首字节丢失问题
											//读取一次SR寄存器，清除TC标志位。
	USART_SendData(USARTx, (uint8_t) ch);
	while((USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET) && (timeout++ < UART_FLAG_TimeOut));
}
/**********************************************************************************
* Function Name  : USART_PutData
* 串口发送若干个字符
* 入口参数
* USARTx:串口号
* ch:数据
* len:数据长度
**********************************************************************************/
void USART_PutData(USART_TypeDef* USARTx,unsigned char *dat,unsigned  int len)
{
	unsigned  int i;
	for(i = 0;i < len;i++)USART_PutChar(USARTx,(uint8_t)* (dat++));
}
/**********************************************************************************
* Function Name  : USART_PutS
* 串口发送字符串
* 入口参数
* USARTx:串口号
* *s:数据指针
**********************************************************************************/
void USART_PutS(USART_TypeDef* USARTx,unsigned char *s)
{
	while(*s != '\0')USART_PutChar(USARTx,*(s++));
}

/**********************************************************************************
* 串口1接收字符函数，阻塞模式（接收缓冲区中提取）
**********************************************************************************/
uint16_t USART1_GetCharBlock(uint16_t timeout)
{
	UART1_RBUF_ST *p = &uart1_rbuf;
	uint16_t to = timeout;	
	while(p->out == p->in)if(!(--to))return TIMEOUT;
	return (p->buf [(p->out++) & (UART1_RBUF_SIZE - 1)]);
}

/**********************************************************************************
* 串口1接收字符函数，非阻塞模式（接收缓冲区中提取）
**********************************************************************************/
uint16_t USART1_GetChar(void)
{
	UART1_RBUF_ST *p = &uart1_rbuf;			
	if(p->out == p->in) //缓冲区空条件
		return EMPTY;
	return USART1_GetCharBlock(1000);
}


/**********************************************************************************
printf功能定义，(括号中表示从哪个串口输出数据及数据类型)
**********************************************************************************/
PUTCHAR_PROTOTYPE
{
	USART_PutChar(USART3,(uint8_t) ch);
	return ch;
}

/**********************************************************************************
清除串口1接收标志位，按帧进行
**********************************************************************************/
void USART1_ClearBuf_Flag(void)
{
	UART1_RBUF_ST *p = &uart1_rbuf;
	p->out = 0;
	p->in = 0;
	
	Uart1Flag = 0;
	USART1_RX_Len = 0;
}


void U485SendData(unsigned char *dat,unsigned short len)
{
	RS485_TX();
	__nop();
	USART_PutData(USART1,dat,len);
	__nop();
	RS485_RX();
}






















