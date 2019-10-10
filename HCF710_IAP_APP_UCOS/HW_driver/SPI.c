
#include "main.h"

#define SPI_Wait_TimeOut  		0xffff     //超时常量 0x5000	

/*******************************************************************************
* Function Name  : SPI1_Configuration
* Description    : STM32 SPI1配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI1_Configuration(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
  
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1,ENABLE);													//打开外设时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA | RCC_AHBPeriph_GPIOB  , ENABLE);
	
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);                     						//设置为外设功能，此处不能合并
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;											//设置端口模式，PA13=SCK2,PA14=MISO2,PA15=MOSI2
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                                								//外设复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																//推挽模式
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_DOWN;																//默认下拉
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;															//GPIO翻转速度为40MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);																		//将配置写入对应的GPIO寄存器中

	//AD_CS
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4;																	//设置SPI片选
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																//端口输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																//推挽模式
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;																//默认上拉，禁止AD片选
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;															//GPIO翻转速度为2MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);																		//将配置写入对应的GPIO寄存器中 

	//Temp_CS
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_0 ;																	//Temp_CS、AD_Start
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;																//
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;																//
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;																//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;															//
	GPIO_Init(GPIOB, &GPIO_InitStructure);																		//
	
	/* SPI1 configuration */
	SPI_Cmd(SPI1, DISABLE); 
	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;//串行同步时钟的第1个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;//定义波特率预分频的值:波特率预分频值为2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	SPI_Cmd(SPI1, ENABLE); 
}
void SPI1_Mode_Configuration(unsigned short SPI1_CPOL,unsigned short SPI1_CPHA)
{
	SPI_InitTypeDef  SPI_InitStructure;
	/* SPI1 configuration */
	SPI_Cmd(SPI1, DISABLE); 
	SPI_I2S_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;//设置SPI单向或者双向的数据模式:SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;							//设置SPI工作模式:设置为主SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;//设置SPI的数据大小:SPI发送接收8位帧结构
	SPI_InitStructure.SPI_CPOL = SPI1_CPOL;//串行同步时钟的空闲状态为高电平
	SPI_InitStructure.SPI_CPHA = SPI1_CPHA;//串行同步时钟的第1个跳变沿（上升或下降）数据被采样
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;//NSS信号由硬件（NSS管脚）还是软件（使用SSI位）管理:内部NSS信号有SSI位控制
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;//定义波特率预分频的值:波特率预分频值为2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;//指定数据传输从MSB位还是LSB位开始:数据传输从MSB位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;	//CRC值计算的多项式
	SPI_Init(SPI1, &SPI_InitStructure);  //根据SPI_InitStruct中指定的参数初始化外设SPIx寄存器
	
	SPI_Cmd(SPI1, ENABLE); 
}

/*******************************************************************************
* Function Name  : SPI1_ReadWriteByte
* Description    : 从SPI总线上读取或写一个字节数据
* Input          : 写字节
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
* Function Name  : SPI1_CS_Select(ADC_CS、ADX_CS、TEMP_CS)
* Description    : SPI片选选择
* Input          : 片选位
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
* rw:读操作=1，写操作=2
* reg：寄存器地址
* cread：是否连续读 1=连续读
* AD779x reset
*******************************************************************************/
static void AD779x_Comm(unsigned char reg, unsigned char rw, unsigned char cread)
{	
	SPI1_ReadWriteByte((rw ? 0x40 : 0x00) | (reg << 3) | (cread ? 0x04 : 0x00));
}

/*******************************************************************************
* Function Name  : AD779x_StatusRegisterRead
* 读取AD7798状态寄存器
*******************************************************************************/
unsigned char AD779x_StatusRegisterRead(void)
{ 
	unsigned char Status; 
 
	//AD7798_Reset(); //复位
	//AD_SPI_CS_LOW;
	AD779x_Comm(AD779x_STATUS_REG,1,0);  //读寄存器操作
	Status=SPI1_ReadWriteByte(Dummy_Byte);  //读取
	return Status;     
}

/*******************************************************************************
* Function Name  : AD779x_ReadOffset
* 读取AD779x校准寄存器
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
* 读取AD779x满度校准寄存器
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
* 设置AD779x工作模式
* mode:模式选择
* psw:电源开关
* rate:转换速率
*******************************************************************************/
void AD779x_SetMode(unsigned char mode, unsigned char psw, unsigned char rate)
{
    AD779x_Comm(AD779x_MODE_REG, 0, 0);
    SPI1_ReadWriteByte(mode << 5 | (psw ? 0x10 : 0x00));
    SPI1_ReadWriteByte(rate);
}

/*******************************************************************************
* Function Name  : AD779x_WriteConfig
* 写AD779x配置寄存器
* burnout:电流使能位
* UB:单/双极选择位
* gain:增益
* ref_det:参考保护使能
* buf:配置ADC是否使用内部缓冲
* channel:通道选择
*******************************************************************************/
void AD779x_WriteConfig(unsigned char burnout, unsigned char UB, unsigned char gain,unsigned char ref_det, unsigned char buf, unsigned char channel)
{
	AD779x_Comm(AD779x_CONFIG_REG, 0, 0);
	SPI1_ReadWriteByte((burnout ? 0x20 : 0x00) | (UB ? 0x10 : 0x00) | gain);
	SPI1_ReadWriteByte((ref_det ? 0x20 : 0x00) | (buf ? 0x10 : 0x00) | channel);
}

/*******************************************************************************
* Function Name  : AD779x_DataReady
* 检查AD779x是否准备好
*******************************************************************************/
unsigned char AD779x_DataReady(void)
{
	return (!AD779x_RDY);
}

/*********************************************************************************
**                          以上部分为配置函数                                  **
**********************************************************************************/

/*******************************************************************************
* Function Name  : AD779x_RequestData
* 设置是否连续读数据
* continuous:1=连续读,0=读一次
*******************************************************************************/
void AD779x_RequestData(unsigned char continuous)
{
	AD779x_Comm(AD779x_DATA_REG, 1, continuous);
}

/*******************************************************************************
* Function Name  : AD779x_Calibrate
* 校正
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
		 OSTimeDlyHMSM(0,0,0,2,OS_OPT_TIME_HMSM_STRICT,&err); //延时
		 if(!(--timeout))return 0;
	}
	offset = AD779x_ReadOffset();
	return offset;
}

/*******************************************************************************
* Function Name  : AD779x_Init
* 初始化AD779x
*******************************************************************************/
unsigned char AD779x_Init(void)
{
	//unsigned char Status=0,ID=0;
	unsigned int buf;
	
	SPI1_CS_Select(AD779x);				//选择AD779x的片选信号
	
	__nop();__nop();__nop();__nop();
	AD779x_Reset();								//复位AD779x
	//__nop();__nop();__nop();__nop();
	
	
	AD779x_Comm(AD779x_ID_REG,1,0);  //读寄存器操作
	ad7799_ID=SPI1_ReadWriteByte(0x00);  //读取
	//if((ID & 0x09) != 0x09)return 0;
	
	ad7799_status = AD779x_StatusRegisterRead();//获取AD779x状态寄存器内容
	
	buf=AD779x_ReadScale();
	AD779x_Calibrate();				//对AD779x进行校准

	AD779x_Set(1);
	
	
	//log1_info("1 AD779x ID:0x%x,Status:0x%x\r\n",ID,Status);
	//log1_info("2 AD779x Status:0x%x\r\n",Status);
	//log1_info("AD779x OK!\r\n");
	return 0;
}

/*******************************************************************************
* Function Name  : AD779x_ReadData
* 读数据
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
* Description    : 对采集到的数据进行一阶滞后滤波算法，
									 系数Filter_K称为灵敏度，表示当前数据在总结果中所占的权重，
									 系数越大，灵敏度越高，但数据平滑度越低。
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
	OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时10ms
	AD779x_SetMode(AD779x_CONTINUOUS_CONVERSION_MODE,0,AD779x_33_2_HZ);
	//AD779x_WriteConfig(unsigned char burnout, unsigned char UB, unsigned char gain,unsigned char ref_det, unsigned char buf, unsigned char channel)
	AD779x_WriteConfig(0,0,AD779x_32_GAIN,0,1,channel);
}

/*******************************************************************************
* Function Name  : AD779x_ContinuousReadData 持续单次采样
* 读数据
* channel:通道
* *databuf：数据指针
* number：读取数量
* 返回：0：超时，1：成功
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
		OS_CRITICAL_ENTER();	//进入临界区
		
		AD779x_RequestData(0);
		val = SPI1_ReadWriteByte(0);
		val <<= 8;
		val |= SPI1_ReadWriteByte(0);
		val <<= 8;
		val |= SPI1_ReadWriteByte(0);
		
		OS_CRITICAL_EXIT();	//进入临界区
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
//* Function Name  : AD779x_ContinuousReadData 持续单次采样
//* 读数据
//* channel:通道
//* *databuf：数据指针
//* number：读取数量
//* 返回：0：超时，1：成功
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
//		OS_CRITICAL_ENTER();	//进入临界区
//		
//		AD779x_RequestData(0);
//		val = SPI1_ReadWriteByte(0);
//		val <<= 8;
//		val |= SPI1_ReadWriteByte(0);
//		val <<= 8;
//		val |= SPI1_ReadWriteByte(0);
//		
//		OS_CRITICAL_EXIT();	//进入临界区
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
* Function Name  : AD779x_ContinuousReadData  按次数采样，平均
* 读数据
* channel:通道
* *databuf：数据指针
* number：读取数量
* 返回：0：超时，1：成功
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
//	float adc_Average=0;//ADC采样值
//	
//	float dataMV=0;
//	//float dataPA=0;
//	CPU_SR_ALLOC();
//	
//	SPI1_CS_Select(AD779x);
//	
//	if(!AD779x_RDY)
//	{
//		OS_CRITICAL_ENTER();	//进入临界区
//		AD779x_RequestData(0);
//		val = SPI1_ReadWriteByte(0);
//		val <<= 8;
//		val |= SPI1_ReadWriteByte(0);
//		val <<= 8;
//		val |= SPI1_ReadWriteByte(0);
//		
//		OS_CRITICAL_EXIT();	//进入临界区
//		
//		ADC_VAL[AD779x_Sampling_Count1] = (int)val - 0x800000;
//		//log_info("ADC_VAL[%d]:0x%x\r\n",AD779x_Sampling_Count1,ADC_VAL[AD779x_Sampling_Count1]);
//		AD779x_Sampling_Count1++;
//		AD_CS_H();
//		if(AD779x_Sampling_Count1>=Size)
//		{
//			AD779x_Sampling_Count1=0;
//			
//			//冒泡排序，采样Size次，从小到大排序，去中间20个数据，求均值
//			for(i=0;i<Size;i++)
//			{
//				for(j=i+1;j<Size;j++)
//				{
//					if(ADC_VAL[i]>ADC_VAL[j])//从小到大，改为"<"变为从大到小
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








