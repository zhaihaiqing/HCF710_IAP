
#include "main.h"

//适用于AT24C系列 ,根据器件容量大小，修改I2C_PAGESIZE即可
#define EEPROM_ADDR						0xA0			//EEPROM器件地址，格式：1 0 1 0  A2 A1 A0 R/W
#define I2C1_SLAVE_ADDRESS7   0x30     //自身地址
#define ClockSpeed            100000   //速率
#define I2C_PAGESIZE					32    		//AT24C64C每页32byte

#define I2C_FLAG_TimeOut  		0x7fff     //超时常量 0x5000
#define I2C_LONG_TimeOut  		(10 * I2C_FLAG_TimeOut)

#define MMA8451_ADDR			(0x1C<<1)			//EEPROM器件地址，格式：0 0 1 1 1 0 0 R/W




void I2C1_Configuration(void)
{
	I2C_InitTypeDef I2C_InitStructure;
	
	//定义GPIO结构体，定义I2C结构体
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* Enable I2C1 和GPIO clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Reset I2C1 peripheral */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1,  ENABLE);
	/* Release reset signal of I2C1 IP */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
	
//	//将外设连接到对应端口上
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
//	
//	/* Configure I2C1 pins: SCL and SDA --GPIO8 and GPIO9 */
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//外设复用模式
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//开漏输出
//	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//无上拉
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//外设复用模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//开漏输出
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//无上拉
	GPIO_Init(GPIOA, &GPIO_InitStructure);

    I2C_DeInit(I2C1);
    //I2C_SoftwareResetCmd(I2C1, ENABLE);
    I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
    I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitStructure.I2C_OwnAddress1 = 0x00;
    I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed =  ClockSpeed;

    I2C_Init(I2C1, &I2C_InitStructure);
    I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);
    I2C_Cmd(I2C1, ENABLE);
}


/****************************************************
**函数名:I2C_Standby_24C
**功能:24C是否准备好再写入的判断
**注意事项:本函数可以理解为:判忙
****************************************************/
void I2C_Standby_24C(void)      
{
  unsigned short int SR1_Tmp=0;
  unsigned int timeout=0;
  timeout = I2C_LONG_TimeOut;//设置超时变量
  do
  {
    /*起始位*/
    I2C_GenerateSTART(I2C1, ENABLE);
    /*读SR1*/
    SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
    /*器件地址(写)*/
    I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
		if(timeout == (I2C_LONG_TimeOut - 1000))I2C1_Configuration();
  }while((!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002)) && (timeout--));

  /**/
  I2C_ClearFlag(I2C1, I2C_FLAG_AF);
  /*发送停止位*/    
  I2C_GenerateSTOP(I2C1, ENABLE);
}

/****************************************************
**函数名:I2C_Read_Byte
**功能:
**I2C标准驱动
****************************************************/
unsigned char I2C1_Read_Byte(unsigned char Device_Addr,unsigned char Reg_Addr)
{
  unsigned int timeout;
  unsigned char Reg_Value;
  timeout = I2C_FLAG_TimeOut;                //设置超时变量
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && (timeout--));
  I2C_AcknowledgeConfig(I2C1, ENABLE);       //允许1字节1应答模式

  I2C_GenerateSTART(I2C1, ENABLE);//发送起始位 
  timeout = I2C_FLAG_TimeOut;     //设置超时变量  
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));//EV5,主模式

  I2C_Send7bitAddress(I2C1,  Device_Addr, I2C_Direction_Transmitter);//发送器件地址(写)
  timeout = I2C_FLAG_TimeOut;//设置超时变量  
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));

  I2C_SendData(I2C1, Reg_Addr); //发送寄存器地址,
  timeout = I2C_FLAG_TimeOut;   //设置超时变量
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));//数据已发送

  I2C_GenerateSTART(I2C1, ENABLE);//产生起始位
  timeout = I2C_FLAG_TimeOut;     //设置超时变量
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));

  I2C_Send7bitAddress(I2C1, Device_Addr, I2C_Direction_Receiver);//发送器件地址，器件读
  timeout = I2C_FLAG_TimeOut;    //设置超时变量
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && (timeout--));
	
  I2C_AcknowledgeConfig(I2C1, DISABLE);	    //最后一位后要关闭应答的
  I2C_GenerateSTOP(I2C1, ENABLE);			//发送停止位

  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) && (timeout--)); /* EV7 */
  Reg_Value = I2C_ReceiveData(I2C1);
		
  I2C_AcknowledgeConfig(I2C1, ENABLE);//再次允许应答模式
  
  return Reg_Value;
}

/*************************************************
**函数名:I2C_Write_Byte
**功能:
**I2C标准驱动
*************************************************/
void I2C_Write_Byte(unsigned char Device_Addr,unsigned char Reg_Addr,unsigned char Reg_Value)
{
  unsigned int timeout;
  
  I2C_GenerateSTART(I2C1, ENABLE);//起始位
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));	

  I2C_Send7bitAddress(I2C1, Device_Addr, I2C_Direction_Transmitter);//器件地址(写)
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));

  I2C_SendData(I2C1, Reg_Addr); //发送高8位地址
  timeout = I2C_FLAG_TimeOut;//设置超时变量  
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));//数据已发送
  
  I2C_SendData(I2C1, Reg_Value); //发送需要写入的数据
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));

  I2C_GenerateSTOP(I2C1, ENABLE);//停止位
}

/*************************************************
**函数名:I2C_PageWrite_24C
**功能:写入一页(以内)  最大32byte
*************************************************/
unsigned char I2C_PageWrite_24C(unsigned short addr,unsigned char* pBuffer, unsigned char Length)
{
  unsigned int timeout;
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && (timeout--));
	if(!timeout)return ERROR;

  /*起始位*/
  I2C_GenerateSTART(I2C1, ENABLE);
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));
	if(!timeout)return ERROR;	

  /*器件地址(写)*/
  I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));
	if(!timeout)return ERROR;

  /*写地址值*/
  I2C_SendData(I2C1, (addr & 0xff00) >> 8); //发送高8位地址
  timeout = I2C_FLAG_TimeOut;//设置超时变量  
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));/*数据已发送*/
	if(!timeout)return ERROR;
  I2C_SendData(I2C1, addr & 0x00ff); //发送低8位地址
  timeout = I2C_FLAG_TimeOut;//设置超时变量  
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));/*数据已发送*/
	if(!timeout)return ERROR;

  while(Length--)  
  {
    I2C_SendData(I2C1, *pBuffer); 
    pBuffer++; 
    timeout = I2C_FLAG_TimeOut;//设置超时变量
    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));
		if(!timeout)return ERROR;
  }
  /*停止位*/
  I2C_GenerateSTOP(I2C1, ENABLE);
	return SUCCESS;
}
/*************************************************
**函数名:I2C_WriteS_24C
**功能:页写入24C
*************************************************/
unsigned char EE_Write(unsigned short addr,unsigned char* pBuffer,  unsigned short Length)
{
	unsigned char temp;
	//1.先把页不对齐的部分写入
	temp=addr % I2C_PAGESIZE;
	I2C_Standby_24C(); //判断是否忙
	if(temp)
	{
    temp=I2C_PAGESIZE-temp;
    if(Length >= temp)
    {
      I2C_PageWrite_24C(addr,pBuffer,  temp);
      Length-=temp;
      addr+=temp;
      pBuffer+=temp;
    }
    else
    {
      if(I2C_PageWrite_24C(addr,pBuffer,  Length) == ERROR )return ERROR;
      Length = 0;
    }
		I2C_Standby_24C(); //判断是否忙
	}
	//2.从页对齐的地方开始写
	while(Length)
	{
		if(Length>=I2C_PAGESIZE)
		{
			if(I2C_PageWrite_24C(addr,pBuffer,  I2C_PAGESIZE) == ERROR )return ERROR; //
			Length-=I2C_PAGESIZE;
			addr+=I2C_PAGESIZE;
			pBuffer+=I2C_PAGESIZE;
			I2C_Standby_24C();  //判断是否忙
		}
		else
		{
			if(I2C_PageWrite_24C(addr,pBuffer,  Length) == ERROR )return ERROR;
			Length=0;
			I2C_Standby_24C(); //判断是否忙
		}
	}
	return SUCCESS;
}

/*******************************************************************************
* Function Name  : EE_Read
* Description    : EEPROM读操作函数
* Input          : 地址、返回指针、读取长度
* Output         : None
* Return         : None
*******************************************************************************/
void EE_Read(unsigned short addr ,unsigned char * pBuffer,unsigned short Length)
{
  unsigned int timeout;
  if(Length==0)return;  //长度为0直接返回
  timeout = I2C_FLAG_TimeOut;//设置超时变量
	I2C_Standby_24C();
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && (timeout--));

  /*允许1字节1应答模式*/
  I2C_AcknowledgeConfig(I2C1, ENABLE);

  /* 发送起始位 */
  I2C_GenerateSTART(I2C1, ENABLE);
  timeout = I2C_FLAG_TimeOut;//设置超时变量  
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));/*EV5,主模式*/

  /*发送器件地址(写)*/
  I2C_Send7bitAddress(I2C1,  EEPROM_ADDR, I2C_Direction_Transmitter);
  timeout = I2C_FLAG_TimeOut;//设置超时变量  
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));

  /*发送地址,,地址分两部分，高地址是字节地址，低地址是页地址*/
  I2C_SendData(I2C1, (addr & 0xff00) >> 8); //发送高8位地址
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));/*数据已发送*/
  I2C_SendData(I2C1, addr & 0x00ff); //发送低8位地址
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));/*数据已发送*/

  /*起始位*/
  I2C_GenerateSTART(I2C1, ENABLE);
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));

  /*器件读*/
  I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Receiver);
  timeout = I2C_FLAG_TimeOut;//设置超时变量
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && (timeout--));
  while (Length)
  {
    if(Length==1)
		{
			I2C_AcknowledgeConfig(I2C1, DISABLE);	//最后一位后要关闭应答的
			I2C_GenerateSTOP(I2C1, ENABLE);			//发送停止位
		}

		timeout = I2C_FLAG_TimeOut;//设置超时变量
		while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) && (timeout--)); /* EV7 */
		*pBuffer = I2C_ReceiveData(I2C1);
		pBuffer++;
		/* Decrement the read bytes counter */
		Length--;
  }
  //再次允许应答模式
  I2C_AcknowledgeConfig(I2C1, ENABLE);
}
/*******************************************************************************
* Function Name  : EEPROM_Erase
* Description    : EEPROM全片擦除程序，
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EE_EraseFullChip(void)
{
	unsigned char  DATA_temp=0x00;
	EE_Write(0x0000,&DATA_temp,16384);
	log_info("Erase EEPROM OK!\r\n");
}

/*******************************************************************************
* Function Name  : EEPROM_Check
* Description    : EEPROM校验程序，向EEPROM中写入8个数据，读出，检查是否一致
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EE_Check(void)
{
	unsigned char counti=0,countj=0;
	unsigned char I2C_WriteData[8]={0x00,0x01,0x02,0x03,0x04,0x05,0x07,0x08};
	unsigned char I2C_ReadData[8];
	log_info("Write EEPROM!\r\n");
//	for(counti=0;counti<8;counti++)
//	{
//		log_info("0x%x\r\n",I2C_WriteData[counti]);
//	}
	EE_Write(0x0000,I2C_WriteData,8);
	//Delay(200);
	log_info("Read EEPROM!\r\n");
	EE_Read(0x0000,I2C_ReadData,8);
//	for(counti=0;counti<8;counti++)
//	{
//		log_info("0x%x\r\n",I2C_ReadData[counti]);
//	}
	for(counti=0;counti<8;counti++)
	{
		if(I2C_ReadData[counti]==I2C_WriteData[counti])
		{
			countj++;
		}
	}
	if(countj==8)
	{log_info("EEPROM is OK !\r\n");}
	else
	{log_info("EEPROM Data wrong !\r\n");}
}




/****************************************************/
/****************************************************/
/*        MMA8451      */

unsigned char MMA8454_Init(void)
{
	unsigned char tem_data=0;
	tem_data=I2C1_Read_Byte(MMA8451_ADDR,MMA8451_WHO_AM_I);
	if(tem_data != 0x1a)return 0;
	//log_info("MMA8451_ID:0x%x\r\n",tem_data);
	//I2C_Write_Byte(MMA8451_ADDR,MMA8451_CTRL_REG1,0x00);
	//I2C_Write_Byte(MMA8451_ADDR,MMA8451_XYZ_DATA_CFG,0x00);
	I2C_Write_Byte(MMA8451_ADDR,MMA8451_CTRL_REG1,0x01);
	return 1;
}

void MMA8454_Get_Status(void)
{
	unsigned char tem_data=0;
	tem_data=I2C1_Read_Byte(MMA8451_ADDR,MMA8451_WHO_AM_I);
	log_info("MMA8451_ID:0x%x\r\n",tem_data);
}


MMA8451_DATA_g MMA8454_Read_DATA(void)
{
	unsigned char tem_data=0;
	unsigned short x=0,y=0,z=0;
	float data_x_g=0,data_y_g=0,data_z_g=0;
	MMA8451_DATA_g Val={0};
	
	x   =I2C1_Read_Byte(MMA8451_ADDR,MMA8451_OUT_X_MSB)<<8;
	x  |=I2C1_Read_Byte(MMA8451_ADDR,MMA8451_OUT_X_LSB);
	x >>=2;
	
	y   =I2C1_Read_Byte(MMA8451_ADDR,MMA8451_OUT_Y_MSB)<<8;
	y  |=I2C1_Read_Byte(MMA8451_ADDR,MMA8451_OUT_Y_LSB);
	y >>=2;
	
	z   =I2C1_Read_Byte(MMA8451_ADDR,MMA8451_OUT_Z_MSB)<<8;
	z  |=I2C1_Read_Byte(MMA8451_ADDR,MMA8451_OUT_Z_LSB);
	z >>=2;
	
	
	//转换为加速度,同时将其对应到设备的笛卡尔坐标系中
	if(x & 0x2000) data_x_g = 0 - (((~x)+1)&0x3fff)*2.0/8.192; 
	else 					 data_x_g = x*2.0/8.192;
	
	if(y & 0x2000) data_y_g = 0 - (((~y)+1)&0x3fff)*2.0/8.192; 
	else 					 data_y_g = y*2.0/8.192;
	
	if(z & 0x2000) data_z_g = 0 - (((~z)+1)&0x3fff)*2.0/8.192; 
	else 					 data_z_g = z*2.0/8.192;
	
	Val.x=data_y_g/1000;
	Val.y=-data_x_g/1000;
	Val.z=data_z_g/1000;
	
	log_info("MMA8451_X:%fg,Y:%fg,Z:%fg\r\n",Val.x,Val.y,Val.z);
	
}








