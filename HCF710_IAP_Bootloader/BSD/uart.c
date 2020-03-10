/* Includes ------------------------------------------------------------------*/
#include "main.h"

#ifdef __GNUC__
  /* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
     set to 'Yes') calls __io_putchar() */
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

#define UART_FLAG_TimeOut  0x50000   //��ʱ����

volatile unsigned char USART1_RX_Len=0;     //���ڽ���ָ���

unsigned char Uart1Flag;
UART1_RBUF_ST	uart1_rbuf	=	{ 0, 0, };


/**********************************************************************************
* Function Name  : UART1_Configuration
* ����1��ʼ��
* ��ڲ���
* baudrate:������
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
	
	//��GPIO��USART1��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);  //ʹ�ܴ���1ʱ��
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_USART1);//�˿���ӳ��
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1);
	
	/* ��USART1_Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*  ��USART1_Rx��GPIO����Ϊ��������ģʽ
  	����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
  	���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò��� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;		//GPIO_Pin_7
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baudrate;//���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//��������λΪ8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//����ֹͣλΪ1
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������λ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//���ͺͽ���
	USART_Init(USART1, &USART_InitStructure);
	
	//�ж�ʹ��
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
	USART_ClearFlag(USART1, USART_FLAG_TC);     /* �巢�ͱ�־��Transmission Complete flag */
	
	//ʹ��
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
	
	//��GPIO��USART1��ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);  //ʹ�ܴ���1ʱ��
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource10,GPIO_AF_USART3);//�˿���ӳ��
	//GPIO_PinAFConfig(GPIOB,GPIO_PinSource11,GPIO_AF_USART3);
	
	/* ��USART1_Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;     
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/*  ��USART1_Rx��GPIO����Ϊ��������ģʽ
  	����CPU��λ��GPIOȱʡ���Ǹ�������ģʽ���������������費�Ǳ����
  	���ǣ��һ��ǽ�����ϱ����Ķ������ҷ�ֹ�����ط��޸���������ߵ����ò��� */
//	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;		//GPIO_Pin_7
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	USART_InitStructure.USART_BaudRate = baudrate;//���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//��������λΪ8
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//����ֹͣλΪ1
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������λ
	USART_InitStructure.USART_Mode =   USART_Mode_Tx;//���ͺͽ���
	USART_Init(USART3, &USART_InitStructure);
	
	//�ж�ʹ��
//	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
//	USART_ITConfig(USART3, USART_IT_IDLE, ENABLE);
//	USART_ClearFlag(USART3, USART_FLAG_TC);     /* �巢�ͱ�־��Transmission Complete flag */
	
	//ʹ��
	USART_Cmd(USART3, ENABLE);
	
}
#endif
/**********************************************************************************
* Function Name  : USART_PutChar
* ���ڷ���һ���ַ�
* ��ڲ���
* USARTx:���ں�
* ch:����
**********************************************************************************/
void USART_PutChar(USART_TypeDef* USARTx,unsigned char ch)
{
	unsigned int timeout = 0;
	USART_GetFlagStatus(USARTx, USART_FLAG_TC);//USARTx->SR  ����豸�������ϵ磬���ڵ�һ�η����������ֽڶ�ʧ����
											//��ȡһ��SR�Ĵ��������TC��־λ��
	USART_SendData(USARTx, (uint8_t) ch);
	while((USART_GetFlagStatus(USARTx, USART_FLAG_TC) == RESET) && (timeout++ < UART_FLAG_TimeOut));
}
/**********************************************************************************
* Function Name  : USART_PutData
* ���ڷ������ɸ��ַ�
* ��ڲ���
* USARTx:���ں�
* ch:����
* len:���ݳ���
**********************************************************************************/
void USART_PutData(USART_TypeDef* USARTx,unsigned char *dat,unsigned  int len)
{
	unsigned  int i;
	for(i = 0;i < len;i++)USART_PutChar(USARTx,(uint8_t)* (dat++));
}
/**********************************************************************************
* Function Name  : USART_PutS
* ���ڷ����ַ���
* ��ڲ���
* USARTx:���ں�
* *s:����ָ��
**********************************************************************************/
void USART_PutS(USART_TypeDef* USARTx,unsigned char *s)
{
	while(*s != '\0')USART_PutChar(USARTx,*(s++));
}

/**********************************************************************************
* ����1�����ַ�����������ģʽ�����ջ���������ȡ��
**********************************************************************************/
uint16_t USART1_GetCharBlock(uint16_t timeout)
{
	UART1_RBUF_ST *p = &uart1_rbuf;
	uint16_t to = timeout;	
	while(p->out == p->in)if(!(--to))return TIMEOUT;
	return (p->buf [(p->out++) & (UART1_RBUF_SIZE - 1)]);
}

/**********************************************************************************
* ����1�����ַ�������������ģʽ�����ջ���������ȡ��
**********************************************************************************/
uint16_t USART1_GetChar(void)
{
	UART1_RBUF_ST *p = &uart1_rbuf;			
	if(p->out == p->in) //������������
		return EMPTY;
	return USART1_GetCharBlock(1000);
}


/**********************************************************************************
printf���ܶ��壬(�����б�ʾ���ĸ�����������ݼ���������)
**********************************************************************************/
PUTCHAR_PROTOTYPE
{
	USART_PutChar(USART3,(uint8_t) ch);
	return ch;
}

/**********************************************************************************
�������1���ձ�־λ����֡����
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






















