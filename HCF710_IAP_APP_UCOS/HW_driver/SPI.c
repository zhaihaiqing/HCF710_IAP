
#include "main.h"

#define SPI_Wait_TimeOut  		0xffff     //��ʱ���� 0x5000	

/*******************************************************************************
* Function Name  : SPI1_Configuration
* Description    : STM32 SPI1����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_Configuration(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);													//������ʱ��
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB  , ENABLE);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);                     						//����Ϊ���蹦�ܣ��˴����ܺϲ�
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;											//���ö˿�ģʽ��PA13=SCK2,PA14=MISO2,PA15=MOSI2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                                								//���踴��ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																//����ģʽ
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;																//Ĭ������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;															//GPIO��ת�ٶ�Ϊ40MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);																		//������д���Ӧ��GPIO�Ĵ�����

	//AD_CS
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;																	//����SPIƬѡ
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																//�˿����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																//����ģʽ
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;																//Ĭ����������ֹADƬѡ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;															//GPIO��ת�ٶ�Ϊ2MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);																		//������д���Ӧ��GPIO�Ĵ����� 

	//Temp_CS
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;																	//Temp_CS��AD_Start
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																//
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																//
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;																//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;															//
	GPIO_Init(GPIOB, &GPIO_InitStructure);																		//
	
	/* SPI1 configuration */
	SPI_Cmd(SPI1, DISABLE); 
	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//����ͬ��ʱ�ӵĵ�1�������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	
	SPI_Cmd(SPI1, ENABLE); 
}
void SPI1_Mode_Configuration(unsigned short SPI1_CPOL,unsigned short SPI1_CPHA)
{
	SPI_InitTypeDef  SPI_InitStructure;
	/* SPI1 configuration */
	SPI_Cmd(SPI1, DISABLE); 
	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//����SPI�������˫�������ģʽ:SPI����Ϊ˫��˫��ȫ˫��
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							//����SPI����ģʽ:����Ϊ��SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//����SPI�����ݴ�С:SPI���ͽ���8λ֡�ṹ
	SPI_InitStructure.SPI_CPOL = SPI1_CPOL;//����ͬ��ʱ�ӵĿ���״̬Ϊ�ߵ�ƽ
	SPI_InitStructure.SPI_CPHA = SPI1_CPHA;//����ͬ��ʱ�ӵĵ�1�������أ��������½������ݱ�����
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//NSS�ź���Ӳ����NSS�ܽţ����������ʹ��SSIλ������:�ڲ�NSS�ź���SSIλ����
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;//���岨����Ԥ��Ƶ��ֵ:������Ԥ��ƵֵΪ2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//ָ�����ݴ����MSBλ����LSBλ��ʼ:���ݴ����MSBλ��ʼ
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRCֵ����Ķ���ʽ
	SPI_Init(SPI1, &SPI_InitStructure);  //����SPI_InitStruct��ָ���Ĳ�����ʼ������SPIx�Ĵ���
	
	SPI_Cmd(SPI1, ENABLE); 
}

/*******************************************************************************
* Function Name  : SPI1_ReadWriteByte
* Description    : ��SPI�����϶�ȡ��дһ���ֽ�����
* Input          : д�ֽ�
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char SPI1_ReadWriteByte(unsigned char byte)
{
  unsigned int timeout = SPI_Wait_TimeOut;
  /*!< Loop while DR register in not emplty */
  while ((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) && (timeout--));

  /*!< Send byte through the SPI2 peripheral */
  SPI_I2S_SendData(SPI1, byte);
  
   /*!< Wait to receive a byte */
  timeout = SPI_Wait_TimeOut;
  while ((SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) && (timeout--));

  /*!< Return the byte read from the SPI bus */
  return SPI_I2S_ReceiveData(SPI1);

}

/*******************************************************************************
* Function Name  : SPI1_CS_Select(ADC_CS��ADX_CS��TEMP_CS)
* Description    : SPIƬѡѡ��
* Input          : Ƭѡλ
* Output         : SUCCESS/ERROR
* Return         : None
*******************************************************************************/
unsigned char SPI1_CS_Select(unsigned char CS)
{
	if(CS==AD779x)
	{
		TEMP_CS_H();
		__nop();
		AD_CS_L();
		return SUCCESS;
	}
	else if(CS==ADT7301)
	{
		AD_CS_H();
		__nop();
		TEMP_CS_L();
		return SUCCESS;
	}
	else
		return ERROR;
}

/*******************************************************************************
* Function Name  : AD779x_Reset
* AD779x reset
*******************************************************************************/
static void AD779x_Reset(void)
{
	char i;
	for(i = 0; i<4; i++)SPI1_ReadWriteByte(0xff);
	__nop();__nop();__nop();__nop();
}

/*******************************************************************************
* Function Name  : AD779x_Comm
* rw:������=1��д����=2
* reg���Ĵ�����ַ
* cread���Ƿ������� 1=������
* AD779x reset
*******************************************************************************/
static void AD779x_Comm(unsigned char reg, unsigned char rw, unsigned char cread)
{	
	SPI1_ReadWriteByte((rw ? 0x40 : 0x00) | (reg << 3) | (cread ? 0x04 : 0x00));
}

/*******************************************************************************
* Function Name  : AD779x_StatusRegisterRead
* ��ȡAD7798״̬�Ĵ���
*******************************************************************************/
unsigned char AD779x_StatusRegisterRead(void)
{ 
	unsigned char Status; 
 
	//AD7798_Reset(); //��λ
	//AD_SPI_CS_LOW;
	AD779x_Comm(AD779x_STATUS_REG,1,0);  //���Ĵ�������
	Status=SPI1_ReadWriteByte(Dummy_Byte);  //��ȡ
	return Status;     
}

/*******************************************************************************
* Function Name  : AD779x_ReadOffset
* ��ȡAD779xУ׼�Ĵ���
*******************************************************************************/
unsigned int AD779x_ReadOffset(void)
{
	unsigned int val = 0; 
	AD779x_Comm(AD779x_OFFSET_REG, 1, 0);
	val = (unsigned int)SPI1_ReadWriteByte(Dummy_Byte);
	val=val<<8;
	val |= (unsigned int)SPI1_ReadWriteByte(Dummy_Byte);
	val=val<<8;
	val |= (unsigned int)SPI1_ReadWriteByte(Dummy_Byte);
	return val;
}

/*******************************************************************************
* Function Name  : AD779x_ReadScale
* ��ȡAD779x����У׼�Ĵ���
*******************************************************************************/
unsigned int AD779x_ReadScale(void)
{
	unsigned int val = 0;  
	AD779x_Comm(AD779x_SCALE_REG, 1, 0);
	val = (unsigned int)SPI1_ReadWriteByte(Dummy_Byte);
	val=val<<8;
	val |= (unsigned int)SPI1_ReadWriteByte(Dummy_Byte);
	val=val<<8;
	val |= (unsigned int)SPI1_ReadWriteByte(Dummy_Byte);
	return val;
}

/*******************************************************************************
* Function Name  : AD779x_SetMode
* ����AD779x����ģʽ
* mode:ģʽѡ��
* psw:��Դ����
* rate:ת������
*******************************************************************************/
void AD779x_SetMode(unsigned char mode, unsigned char psw, unsigned char rate)
{
    AD779x_Comm(AD779x_MODE_REG, 0, 0);
    SPI1_ReadWriteByte(mode << 5 | (psw ? 0x10 : 0x00));
    SPI1_ReadWriteByte(rate);
}

/*******************************************************************************
* Function Name  : AD779x_WriteConfig
* дAD779x���üĴ���
* burnout:����ʹ��λ
* UB:��/˫��ѡ��λ
* gain:����
* ref_det:�ο�����ʹ��
* buf:����ADC�Ƿ�ʹ���ڲ�����
* channel:ͨ��ѡ��
*******************************************************************************/
void AD779x_WriteConfig(unsigned char burnout, unsigned char UB, unsigned char gain,unsigned char ref_det, unsigned char buf, unsigned char channel)
{
	AD779x_Comm(AD779x_CONFIG_REG, 0, 0);
	SPI1_ReadWriteByte((burnout ? 0x20 : 0x00) | (UB ? 0x10 : 0x00) | gain);
	SPI1_ReadWriteByte((ref_det ? 0x20 : 0x00) | (buf ? 0x10 : 0x00) | channel);
}

/*******************************************************************************
* Function Name  : AD779x_DataReady
* ���AD779x�Ƿ�׼����
*******************************************************************************/
unsigned char AD779x_DataReady(void)
{
	return (!AD779x_RDY);
}

/*********************************************************************************
**                          ���ϲ���Ϊ���ú���                                  **
**********************************************************************************/

/*******************************************************************************
* Function Name  : AD779x_RequestData
* �����Ƿ�����������
* continuous:1=������,0=��һ��
*******************************************************************************/
void AD779x_RequestData(unsigned char continuous)
{
	AD779x_Comm(AD779x_DATA_REG, 1, continuous);
}

/*******************************************************************************
* Function Name  : AD779x_Calibrate
* У��
*******************************************************************************/
unsigned int AD779x_Calibrate(void)
{
	OS_ERR      err;
	unsigned int offset;
	unsigned int timeout=500;
	offset  = AD779x_ReadOffset();
	//log_info("Before Calibrate offset=0x%d\r\n",offset); 
	/* cal */
	AD779x_SetMode(AD779x_INTERNAL_OFFSET_CAL_MODE, 0, AD779x_16_7_1_HZ);
	while(!AD779x_DataReady())
	{
		 OSTimeDlyHMSM(0,0,0,2,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ
		 if(!(--timeout))return 0;
	}
	offset = AD779x_ReadOffset();
	return offset;
}

/*******************************************************************************
* Function Name  : AD779x_Init
* ��ʼ��AD779x
*******************************************************************************/
unsigned char AD779x_Init(void)
{
	//unsigned char Status=0,ID=0;
	unsigned int buf;
	
	SPI1_CS_Select(AD779x);				//ѡ��AD779x��Ƭѡ�ź�
	
	__nop();__nop();__nop();__nop();
	AD779x_Reset();								//��λAD779x
	//__nop();__nop();__nop();__nop();
	
	
	AD779x_Comm(AD779x_ID_REG,1,0);  //���Ĵ�������
	ad7799_ID=SPI1_ReadWriteByte(0x00);  //��ȡ
	//if((ID & 0x09) != 0x09)return 0;
	
	ad7799_status = AD779x_StatusRegisterRead();//��ȡAD779x״̬�Ĵ�������
	
	buf=AD779x_ReadScale();
	AD779x_Calibrate();				//��AD779x����У׼

	AD779x_Set(1);
	
	
	//log1_info("1 AD779x ID:0x%x,Status:0x%x\r\n",ID,Status);
	//log1_info("2 AD779x Status:0x%x\r\n",Status);
	//log1_info("AD779x OK!\r\n");
	return 0;
}

/*******************************************************************************
* Function Name  : AD779x_ReadData
* ������
*******************************************************************************/
//int AD779x_ReadData(char channel)
//{
//	unsigned int val = 0;
//	unsigned int timeout=500;
//	int result=0;
//	SPI1_CS_Select(AD779x);
//	Delay(5);	
//	AD779x_SetMode(AD779x_SINGLE_CONVERSION_MODE,0,AD779x_16_7_1_HZ);
//	AD779x_WriteConfig(0,0,AD779x_32_GAIN,0,1,channel);
//	AD779x_RequestData(0);
//	while(!AD779x_DataReady())
//	{
//		Delay(2);
//		if(!(--timeout)){AD_CS_H();return 0;}
//	}
//	val = SPI1_ReadWriteByte(0);
//	val <<= 8;
//	val |= SPI1_ReadWriteByte(0);
//	val <<= 8;
//	val |= SPI1_ReadWriteByte(0);
//	AD_CS_H();
//	result = (int)val - 0x800000;
//	//log_info("ADC Result:0x%x\r\n",result);
//	return result;
//}

/*******************************************************************************
* Function Name  : 
* Description    : �Բɼ��������ݽ���һ���ͺ��˲��㷨��
									 ϵ��Filter_K��Ϊ�����ȣ���ʾ��ǰ�������ܽ������ռ��Ȩ�أ�
									 ϵ��Խ��������Խ�ߣ�������ƽ����Խ�͡�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
float PA_MV_Data_Filter=0;
void FO_Filter(unsigned char Filter_K,float data)
{
	
	if(data<PA_MV_Data_Filter)
	{
		data=PA_MV_Data_Filter-(((PA_MV_Data_Filter-data)*Filter_K)/256.0);
		PA_MV_Data_Filter=data;
	}
	else if(data>PA_MV_Data_Filter)
	{
		data=PA_MV_Data_Filter+(((data-PA_MV_Data_Filter)*Filter_K)/256.0);
		PA_MV_Data_Filter=data;
	}
	else 
	{
		PA_MV_Data_Filter=data;
	}
}

void AD779x_Set(char channel)
{
	OS_ERR err;
	
	SPI1_CS_Select(AD779x);
	OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ10ms
	AD779x_SetMode(AD779x_CONTINUOUS_CONVERSION_MODE,0,AD779x_33_2_HZ);
	//AD779x_WriteConfig(unsigned char burnout, unsigned char UB, unsigned char gain,unsigned char ref_det, unsigned char buf, unsigned char channel)
	AD779x_WriteConfig(0,0,AD779x_32_GAIN,0,1,channel);
}

/*******************************************************************************
* Function Name  : AD779x_ContinuousReadData �������β���
* ������
* channel:ͨ��
* *databuf������ָ��
* number����ȡ����
* ���أ�0����ʱ��1���ɹ�
*******************************************************************************/

unsigned char AD779x_Sampling_Complete_Flag0=0;
unsigned char AD779x_Sampling_Complete_Flag1=0;

void AD779x_ContinuousReadData1(unsigned short Average_num)
{
	unsigned short i,j;
	unsigned int val = 0;
	//int adc_buf=0; 
	float dataMV=0;
	static unsigned int AD779x_Sampling_Count1;
	static long int	ADC_VAL;
	
	CPU_SR_ALLOC();
	
	SPI1_CS_Select(AD779x);
	
	if(!AD779x_RDY)
	{
		OS_CRITICAL_ENTER();	//�����ٽ���
		
		AD779x_RequestData(0);
		val = SPI1_ReadWriteByte(0);
		val <<= 8;
		val |= SPI1_ReadWriteByte(0);
		val <<= 8;
		val |= SPI1_ReadWriteByte(0);
		
		OS_CRITICAL_EXIT();	//�����ٽ���
		AD_CS_H();
		
		ADC_VAL += ((int)val - 0x800000);
		AD779x_Sampling_Count1++;
		
		if( AD779x_Sampling_Complete_Flag0 || (Average_num==0) )
		{
			
			dataMV=5000.0*ADC_VAL/AD779x_Sampling_Count1/32/8388608;//mV
			
			InputRegister.ADCOriginalValue = dataMV;
			//log1_info("dataMV=%fmV,Average_num=%d,Sampling_Time:%ds\r\n",dataMV,AD779x_Sampling_Count1,Average_num);
			
			
			ADC_VAL=0;
			AD779x_Sampling_Complete_Flag0=0;
			AD779x_Sampling_Complete_Flag1=1;
			AD779x_Sampling_Count1=0;
		}	
	}
	AD_CS_H();
}



///*******************************************************************************
//* Function Name  : AD779x_ContinuousReadData �������β���
//* ������
//* channel:ͨ��
//* *databuf������ָ��
//* number����ȡ����
//* ���أ�0����ʱ��1���ɹ�
//*******************************************************************************/
//unsigned int AD779x_Sampling_Count1=0;
//unsigned char AD779x_Sampling_Complete_Flag1=0;
//int	ADC_VAL=0;
//void AD779x_ContinuousReadData1(unsigned short Average_num)
//{
//	unsigned short i,j;
//	unsigned int val = 0;
//	//int adc_buf=0; 
//	float dataMV=0;
//	
//	CPU_SR_ALLOC();
//	
//	SPI1_CS_Select(AD779x);
//	
//	if(!AD779x_RDY)
//	{
//		OS_CRITICAL_ENTER();	//�����ٽ���
//		
//		AD779x_RequestData(0);
//		val = SPI1_ReadWriteByte(0);
//		val <<= 8;
//		val |= SPI1_ReadWriteByte(0);
//		val <<= 8;
//		val |= SPI1_ReadWriteByte(0);
//		
//		OS_CRITICAL_EXIT();	//�����ٽ���
//		AD_CS_H();
//		
//		ADC_VAL += ((int)val - 0x800000);
//		AD779x_Sampling_Count1++;
//		if(AD779x_Sampling_Count1>=Average_num)
//		{
//			AD779x_Sampling_Complete_Flag1=1;
//			AD779x_Sampling_Count1=0;
//			dataMV=5000.0*ADC_VAL/Average_num/32/8388608;//mV
//			InputRegister.ADCOriginalValue = dataMV;
//			ADC_VAL=0;
//			log1_info("dataMV=%fmV,Average_num=%d\r\n",dataMV,Average_num);	
//		}	
//	}
//	AD_CS_H();
//}


/*******************************************************************************
* Function Name  : AD779x_ContinuousReadData  ������������ƽ��
* ������
* channel:ͨ��
* *databuf������ָ��
* number����ȡ����
* ���أ�0����ʱ��1���ɹ�
*******************************************************************************/
//unsigned int AD779x_Sampling_Count1=0;
//unsigned char AD779x_Sampling_Complete_Flag1=0;
//int ADC_VAL[128]={0};
//void AD779x_ContinuousReadDataN(char channel,unsigned int Size)
//{
//	unsigned short i,j;
//	unsigned int val = 0;
//	int adc_buf=0; 
//	
//	float adc_Average=0;//ADC����ֵ
//	
//	float dataMV=0;
//	//float dataPA=0;
//	CPU_SR_ALLOC();
//	
//	SPI1_CS_Select(AD779x);
//	
//	if(!AD779x_RDY)
//	{
//		OS_CRITICAL_ENTER();	//�����ٽ���
//		AD779x_RequestData(0);
//		val = SPI1_ReadWriteByte(0);
//		val <<= 8;
//		val |= SPI1_ReadWriteByte(0);
//		val <<= 8;
//		val |= SPI1_ReadWriteByte(0);
//		
//		OS_CRITICAL_EXIT();	//�����ٽ���
//		
//		ADC_VAL[AD779x_Sampling_Count1] = (int)val - 0x800000;
//		//log_info("ADC_VAL[%d]:0x%x\r\n",AD779x_Sampling_Count1,ADC_VAL[AD779x_Sampling_Count1]);
//		AD779x_Sampling_Count1++;
//		AD_CS_H();
//		if(AD779x_Sampling_Count1>=Size)
//		{
//			AD779x_Sampling_Count1=0;
//			
//			//ð�����򣬲���Size�Σ���С��������ȥ�м�20�����ݣ����ֵ
//			for(i=0;i<Size;i++)
//			{
//				for(j=i+1;j<Size;j++)
//				{
//					if(ADC_VAL[i]>ADC_VAL[j])//��С���󣬸�Ϊ"<"��Ϊ�Ӵ�С
//					{
//						adc_buf=ADC_VAL[i];
//						ADC_VAL[i]=ADC_VAL[j];
//						ADC_VAL[j]=adc_buf;
//					}
//				}
//			}
//			
//			adc_buf=0;
//			for(i=Size/2-8;i<Size/2+8;i++)
//			{
//				adc_buf+=ADC_VAL[i];
//			}
//			
//			adc_Average=1.0*adc_buf/16;
//			
//			dataMV=5000.0*adc_Average/32/8388608;//mV
//			//dataPA=20000*dataMV/50;
//			AD779x_Sampling_Complete_Flag1=1;
//			InputRegister.ADCOriginalValue = dataMV;
//			
//			//log_info("channel(%d)dataMV_Average=%fmV\r\n",channel,dataMV);		
//		}
//	}
//	AD_CS_H();
//}








