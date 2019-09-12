
#include "main.h"

//������AT24Cϵ�� ,��������������С���޸�I2C_PAGESIZE����
#define EEPROM_ADDR						0xA0			//EEPROM������ַ����ʽ��1 0 1 0  A2 A1 A0 R/W
#define I2C1_SLAVE_ADDRESS7   0x30     //�����ַ
#define ClockSpeed            100000   //����
#define I2C_PAGESIZE					32    		//AT24C64Cÿҳ32byte

#define I2C_FLAG_TimeOut  		0x7fff     //��ʱ���� 0x5000
#define I2C_LONG_TimeOut  		(10 * I2C_FLAG_TimeOut)

#define MMA8451_ADDR			(0x1C<<1)			//EEPROM������ַ����ʽ��0 0 1 1 1 0 0 R/W




void I2C1_Configuration(void)
{
	I2C_InitTypeDef I2C_InitStructure;
	
	//����GPIO�ṹ�壬����I2C�ṹ��
	GPIO_InitTypeDef GPIO_InitStructure;
  
	/* Enable I2C1 ��GPIO clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	
	/* Reset I2C1 peripheral */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1,  ENABLE);
	/* Release reset signal of I2C1 IP */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
	
//	//���������ӵ���Ӧ�˿���
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
//	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
//	
//	/* Configure I2C1 pins: SCL and SDA --GPIO8 and GPIO9 */
//	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_8 | GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���踴��ģʽ
//	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//��©���
//	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//������
//	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_11 | GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//���踴��ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//��©���
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;//������
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
**������:I2C_Standby_24C
**����:24C�Ƿ�׼������д����ж�
**ע������:�������������Ϊ:��æ
****************************************************/
void I2C_Standby_24C(void)      
{
  unsigned short int SR1_Tmp=0;
  unsigned int timeout=0;
  timeout = I2C_LONG_TimeOut;//���ó�ʱ����
  do
  {
    /*��ʼλ*/
    I2C_GenerateSTART(I2C1, ENABLE);
    /*��SR1*/
    SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
    /*������ַ(д)*/
    I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
		if(timeout == (I2C_LONG_TimeOut - 1000))I2C1_Configuration();
  }while((!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002)) && (timeout--));

  /**/
  I2C_ClearFlag(I2C1, I2C_FLAG_AF);
  /*����ֹͣλ*/    
  I2C_GenerateSTOP(I2C1, ENABLE);
}

/****************************************************
**������:I2C_Read_Byte
**����:
**I2C��׼����
****************************************************/
unsigned char I2C1_Read_Byte(unsigned char Device_Addr,unsigned char Reg_Addr)
{
  unsigned int timeout;
  unsigned char Reg_Value;
  timeout = I2C_FLAG_TimeOut;                //���ó�ʱ����
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && (timeout--));
  I2C_AcknowledgeConfig(I2C1, ENABLE);       //����1�ֽ�1Ӧ��ģʽ

  I2C_GenerateSTART(I2C1, ENABLE);//������ʼλ 
  timeout = I2C_FLAG_TimeOut;     //���ó�ʱ����  
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));//EV5,��ģʽ

  I2C_Send7bitAddress(I2C1,  Device_Addr, I2C_Direction_Transmitter);//����������ַ(д)
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����  
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));

  I2C_SendData(I2C1, Reg_Addr); //���ͼĴ�����ַ,
  timeout = I2C_FLAG_TimeOut;   //���ó�ʱ����
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));//�����ѷ���

  I2C_GenerateSTART(I2C1, ENABLE);//������ʼλ
  timeout = I2C_FLAG_TimeOut;     //���ó�ʱ����
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));

  I2C_Send7bitAddress(I2C1, Device_Addr, I2C_Direction_Receiver);//����������ַ��������
  timeout = I2C_FLAG_TimeOut;    //���ó�ʱ����
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && (timeout--));
	
  I2C_AcknowledgeConfig(I2C1, DISABLE);	    //���һλ��Ҫ�ر�Ӧ���
  I2C_GenerateSTOP(I2C1, ENABLE);			//����ֹͣλ

  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) && (timeout--)); /* EV7 */
  Reg_Value = I2C_ReceiveData(I2C1);
		
  I2C_AcknowledgeConfig(I2C1, ENABLE);//�ٴ�����Ӧ��ģʽ
  
  return Reg_Value;
}

/*************************************************
**������:I2C_Write_Byte
**����:
**I2C��׼����
*************************************************/
void I2C_Write_Byte(unsigned char Device_Addr,unsigned char Reg_Addr,unsigned char Reg_Value)
{
  unsigned int timeout;
  
  I2C_GenerateSTART(I2C1, ENABLE);//��ʼλ
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));	

  I2C_Send7bitAddress(I2C1, Device_Addr, I2C_Direction_Transmitter);//������ַ(д)
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));

  I2C_SendData(I2C1, Reg_Addr); //���͸�8λ��ַ
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����  
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));//�����ѷ���
  
  I2C_SendData(I2C1, Reg_Value); //������Ҫд�������
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));

  I2C_GenerateSTOP(I2C1, ENABLE);//ֹͣλ
}

/*************************************************
**������:I2C_PageWrite_24C
**����:д��һҳ(����)  ���32byte
*************************************************/
unsigned char I2C_PageWrite_24C(unsigned short addr,unsigned char* pBuffer, unsigned char Length)
{
  unsigned int timeout;
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && (timeout--));
	if(!timeout)return ERROR;

  /*��ʼλ*/
  I2C_GenerateSTART(I2C1, ENABLE);
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));
	if(!timeout)return ERROR;	

  /*������ַ(д)*/
  I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Transmitter);
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));
	if(!timeout)return ERROR;

  /*д��ֵַ*/
  I2C_SendData(I2C1, (addr & 0xff00) >> 8); //���͸�8λ��ַ
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����  
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));/*�����ѷ���*/
	if(!timeout)return ERROR;
  I2C_SendData(I2C1, addr & 0x00ff); //���͵�8λ��ַ
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����  
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));/*�����ѷ���*/
	if(!timeout)return ERROR;

  while(Length--)  
  {
    I2C_SendData(I2C1, *pBuffer); 
    pBuffer++; 
    timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
    while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));
		if(!timeout)return ERROR;
  }
  /*ֹͣλ*/
  I2C_GenerateSTOP(I2C1, ENABLE);
	return SUCCESS;
}
/*************************************************
**������:I2C_WriteS_24C
**����:ҳд��24C
*************************************************/
unsigned char EE_Write(unsigned short addr,unsigned char* pBuffer,  unsigned short Length)
{
	unsigned char temp;
	//1.�Ȱ�ҳ������Ĳ���д��
	temp=addr % I2C_PAGESIZE;
	I2C_Standby_24C(); //�ж��Ƿ�æ
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
		I2C_Standby_24C(); //�ж��Ƿ�æ
	}
	//2.��ҳ����ĵط���ʼд
	while(Length)
	{
		if(Length>=I2C_PAGESIZE)
		{
			if(I2C_PageWrite_24C(addr,pBuffer,  I2C_PAGESIZE) == ERROR )return ERROR; //
			Length-=I2C_PAGESIZE;
			addr+=I2C_PAGESIZE;
			pBuffer+=I2C_PAGESIZE;
			I2C_Standby_24C();  //�ж��Ƿ�æ
		}
		else
		{
			if(I2C_PageWrite_24C(addr,pBuffer,  Length) == ERROR )return ERROR;
			Length=0;
			I2C_Standby_24C(); //�ж��Ƿ�æ
		}
	}
	return SUCCESS;
}

/*******************************************************************************
* Function Name  : EE_Read
* Description    : EEPROM����������
* Input          : ��ַ������ָ�롢��ȡ����
* Output         : None
* Return         : None
*******************************************************************************/
void EE_Read(unsigned short addr ,unsigned char * pBuffer,unsigned short Length)
{
  unsigned int timeout;
  if(Length==0)return;  //����Ϊ0ֱ�ӷ���
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
	I2C_Standby_24C();
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) && (timeout--));

  /*����1�ֽ�1Ӧ��ģʽ*/
  I2C_AcknowledgeConfig(I2C1, ENABLE);

  /* ������ʼλ */
  I2C_GenerateSTART(I2C1, ENABLE);
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����  
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));/*EV5,��ģʽ*/

  /*����������ַ(д)*/
  I2C_Send7bitAddress(I2C1,  EEPROM_ADDR, I2C_Direction_Transmitter);
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����  
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) && (timeout--));

  /*���͵�ַ,,��ַ�������֣��ߵ�ַ���ֽڵ�ַ���͵�ַ��ҳ��ַ*/
  I2C_SendData(I2C1, (addr & 0xff00) >> 8); //���͸�8λ��ַ
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));/*�����ѷ���*/
  I2C_SendData(I2C1, addr & 0x00ff); //���͵�8λ��ַ
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
  while ((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) && (timeout--));/*�����ѷ���*/

  /*��ʼλ*/
  I2C_GenerateSTART(I2C1, ENABLE);
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)) && (timeout--));

  /*������*/
  I2C_Send7bitAddress(I2C1, EEPROM_ADDR, I2C_Direction_Receiver);
  timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
  while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) && (timeout--));
  while (Length)
  {
    if(Length==1)
		{
			I2C_AcknowledgeConfig(I2C1, DISABLE);	//���һλ��Ҫ�ر�Ӧ���
			I2C_GenerateSTOP(I2C1, ENABLE);			//����ֹͣλ
		}

		timeout = I2C_FLAG_TimeOut;//���ó�ʱ����
		while((!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)) && (timeout--)); /* EV7 */
		*pBuffer = I2C_ReceiveData(I2C1);
		pBuffer++;
		/* Decrement the read bytes counter */
		Length--;
  }
  //�ٴ�����Ӧ��ģʽ
  I2C_AcknowledgeConfig(I2C1, ENABLE);
}
/*******************************************************************************
* Function Name  : EEPROM_Erase
* Description    : EEPROMȫƬ��������
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
* Description    : EEPROMУ�������EEPROM��д��8�����ݣ�����������Ƿ�һ��
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
	
	
	//ת��Ϊ���ٶ�,ͬʱ�����Ӧ���豸�ĵѿ�������ϵ��
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








