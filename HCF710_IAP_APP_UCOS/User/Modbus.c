
#include "main.h"

volatile unsigned char Modbus_BusyFlag;
volatile unsigned char Modbus_FreeFlag;

unsigned char  FactoryResetWord[4]    = {0x33,0x55,0x77,0x99};//��������ָ�����
unsigned char  FactorySetValueWord[4] = {0x44,0x66,0x88,0xaa};//��������ָ�����
unsigned char  FactorySetValueWord2[4] = {0x40,0x66,0x88,0xaa};//��������ָ�����
unsigned char  SuperModeValueWord2[4]  = {0x40,0x60,0x80,0xa0};//�߼�ģʽ����

unsigned char SuperMode_Flag	= 0;

volatile KeepRegister_type				KeepRegister;		//���屣�ּĴ���
volatile KeepRegister_type				KeepRegisterTemp;	//���屣�ּĴ�������
volatile InputRegister_type	 			InputRegister;		//��������Ĵ���
volatile InputRegister_type	 			InputRegisterTemp;	//��������Ĵ�������
volatile ModbusDataPackage_type 		ModbusDataPackage;	//����modbus���ջ���

/*******************************************************************************
* Function Name  : __ltobf
* Description    : float�����ݴ�С�˸�ʽת��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
static float __ltobf(float data)
{
	FLOAT_CONV d1, d2;

	d1.f = data;

	d2.c[0] = d1.c[3];
	d2.c[1] = d1.c[2];
	d2.c[2] = d1.c[1];
	d2.c[3] = d1.c[0];
	return d2.f;
}
/*******************************************************************************
* Function Name  : CRC16_Check
* Description    : CRCУ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
uint16_t CRC16_Check(uint8_t *Pushdata,uint8_t length)
{
	uint16_t Reg_CRC=0xffff;
	uint8_t i,j;
	for( i = 0; i<length; i ++)
	{
		Reg_CRC^= *Pushdata++;
		for (j = 0; j<8; j++)
		{
			if (Reg_CRC & 0x0001)

			Reg_CRC=Reg_CRC>>1^0xA001;
			else
			Reg_CRC >>=1;
		}
	}
	return   Reg_CRC;
}

/*******************************************************************************
* Function Name  : ModbusReturnAckInfo
* Description    : modbus�����쳣����Ϣ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ModbusReturnAckInfo(uint8_t err)
{
	uint8_t ErrDat[5];
	uint16_t crc;
	//log_info("err:0x%x\r\n",err);
	if(err)//�쳣��
	{
		ErrDat[0] = KeepRegister.DeviceAddress;//��ֵ�豸��ַ
		ErrDat[1] = ModbusDataPackage.dat[1] | 0x80;//��ֵ�쳣������,��������+0x80
		ErrDat[2] = err;//��ֵ�쳣��
		crc = CRC16_Check(ErrDat,3);
		ErrDat[3] = (crc & 0xff);//У���8λ
		ErrDat[4] = (crc >> 8);//У���8λ
		
		U485SendData(ErrDat,5);//��485��������
		//err=0;
	}
}

/*******************************************************************************
* Function Name  : KeepRegistorDataHton
* Description    : ���ּĴ������ݴ�С��ת��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void KeepRegistorDataHton(void)
{
	uint8_t i;
	KeepRegisterTemp.DeviceAddress =htons(KeepRegister.DeviceAddress);
	KeepRegisterTemp.DeviceGroupNum      =htons(KeepRegister.DeviceGroupNum);
	KeepRegisterTemp.OriginaLiquidAltitude_After =__ltobf(KeepRegister.OriginaLiquidAltitude_After);
	KeepRegisterTemp.OriginaLiquidAltitude_Befor =__ltobf(KeepRegister.OriginaLiquidAltitude_Befor);
	KeepRegisterTemp.Sensor_Range      =htons(KeepRegister.Sensor_Range);
	KeepRegisterTemp.Liquid_Sec      =htons(KeepRegister.Liquid_Sec);
	KeepRegisterTemp.LocalAccelerationOfGravity =__ltobf(KeepRegister.LocalAccelerationOfGravity);
	KeepRegisterTemp.Sensor_FS_Val =__ltobf(KeepRegister.Sensor_FS_Val);
	for(i=0;i<6;i++)KeepRegisterTemp.MV[i] = __ltobf(KeepRegister.MV[i]);
	KeepRegisterTemp.Temp_T0 = __ltobf(KeepRegister.Temp_T0);
	for(i=0;i<5;i++)KeepRegisterTemp.LTC0[i] = __ltobf(KeepRegister.LTC0[i]);
	for(i=0;i<5;i++)KeepRegisterTemp.LTC1[i] = __ltobf(KeepRegister.LTC1[i]);
	for(i=0;i<5;i++)KeepRegisterTemp.LTC2[i] = __ltobf(KeepRegister.LTC2[i]);
	for(i=0;i<5;i++)KeepRegisterTemp.LTC3[i] = __ltobf(KeepRegister.LTC3[i]);
	for(i=0;i<5;i++)KeepRegisterTemp.LTC4[i] = __ltobf(KeepRegister.LTC4[i]);
	for(i=0;i<5;i++)KeepRegisterTemp.LTC5[i] = __ltobf(KeepRegister.LTC5[i]);	
	KeepRegisterTemp.Average_num =htons(KeepRegister.Average_num);
	KeepRegisterTemp.bps =htons(KeepRegister.bps);
}
/*******************************************************************************
* Function Name  : InputRegistorDataHton
* Description    : ����Ĵ������ݴ�С��ת��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InputRegistorDataHton(void)
{
	uint8_t i;
	InputRegisterTemp.DeviceType =htons(InputRegister.DeviceType);
	InputRegisterTemp.SoftwareVersion =htons(InputRegister.SoftwareVersion);
	InputRegisterTemp.SystemWorkTime =htonl(InputRegister.SystemWorkTime);
	InputRegisterTemp.SystemWorkStatus =htons(InputRegister.SystemWorkStatus);
	InputRegisterTemp.AltitudeDifference_After =__ltobf(InputRegister.AltitudeDifference_After);
	InputRegisterTemp.Temperature =__ltobf(InputRegister.Temperature);
	InputRegisterTemp.LiquidAltitudeAbsoluteValue_After =__ltobf(InputRegister.LiquidAltitudeAbsoluteValue_After);
	InputRegisterTemp.ADCOriginalValue =__ltobf(InputRegister.ADCOriginalValue);
	InputRegisterTemp.PA               =__ltobf(InputRegister.PA);
	for(i=0;i<8;i++)InputRegisterTemp.SN[i] =	htons(InputRegister.SN[i]);
	InputRegisterTemp.AltitudeDifference_Befor =__ltobf(InputRegister.AltitudeDifference_Befor);
	InputRegisterTemp.LiquidAltitudeAbsoluteValue_Befor =__ltobf(InputRegister.LiquidAltitudeAbsoluteValue_Befor);
	InputRegisterTemp.MainPower_V =__ltobf(InputRegister.MainPower_V);
	
}
/********************************
�����ּĴ���
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x03)+��ʼ��ַ(2�ֽ�)+��ȡ�Ĵ�������(2�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x03)+���ݳ���(�ֽ���,1�ֽ�)+����(n�ֽ�)+У��(2�ֽ�)
*********************************/
char ModbusReadKeepRegistor(unsigned char RX_Len)
{
	unsigned char err = 0;
	uint8_t temp[255];
	uint16_t crc;
	
 	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3]; //��ȡ��ʼ��ַ
 	uint16_t RegVal = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];		//��ȡҪ��ȡ�ļĴ�������
 	uint16_t StopAddress = StartAddress + RegVal - 1;									//��ȡ������ַ
 	uint16_t KeepRegistorSize = sizeof(KeepRegister) / 2;								//���㱣�ּĴ���������
 	uint16_t bytes = RegVal*2;															//����Ҫ��ȡ���ֽ���
	
 	if(RX_Len != 8)err = err_OE;										//��Ч���������쳣
 	if(!RegVal)err = err_add;															//�Ĵ�����ַ����ȷ,��ȡ�����������1
 	if( (  (StartAddress == 0x03) || (StartAddress == 0x05) || (StartAddress == 0x09) || (StartAddress == 0x0b)  || (StartAddress == 0x0d) || (StartAddress == 0x0f)  || \
	       (StartAddress == 0x11) || (StartAddress == 0x13) || (StartAddress == 0x15) || (StartAddress == 0x17)  || (StartAddress == 0x19) || (StartAddress == 0x1b)  || (StartAddress == 0x1d) || (StartAddress == 0x1f) || \
		   (StartAddress == 0x21) || (StartAddress == 0x23) || (StartAddress == 0x25) || (StartAddress == 0x27)  || (StartAddress == 0x29) || (StartAddress == 0x2b)  || (StartAddress == 0x2d) || (StartAddress == 0x2f) || \
		   (StartAddress == 0x31) || (StartAddress == 0x33) || (StartAddress == 0x35) || (StartAddress == 0x37)  || (StartAddress == 0x39) || (StartAddress == 0x3b)  || (StartAddress == 0x3d) || (StartAddress == 0x3f) || \
		   (StartAddress == 0x41) || (StartAddress == 0x43) || (StartAddress == 0x45) || (StartAddress == 0x47)  || (StartAddress == 0x49) || (StartAddress == 0x4b)  || (StartAddress == 0x4d) || (StartAddress == 0x4f) || \
		   (StartAddress == 0x51) || (StartAddress == 0x53) || (StartAddress == 0x55)  ) )err = err_add;//�Ĵ�����ַ����ȷ,���ֽ�����Ҫ��ͷ����
 	if( (  (StopAddress == 0x02)  || (StopAddress == 0x04)  || (StopAddress == 0x08)  || (StopAddress == 0x0a)   || (StopAddress == 0x0c)  || (StopAddress == 0x0e)   || \
	       (StopAddress == 0x10)  || (StopAddress == 0x12)  || (StopAddress == 0x14)  || (StopAddress == 0x16)   || (StopAddress == 0x18)  || (StopAddress == 0x1a)   || (StopAddress == 0x1c)  || (StopAddress == 0x1e)  || \
		   (StopAddress == 0x20)  || (StopAddress == 0x22)  || (StopAddress == 0x24)  || (StopAddress == 0x26)   || (StopAddress == 0x28)  || (StopAddress == 0x2a)   || (StopAddress == 0x2c)  || (StopAddress == 0x2e)  || \
		   (StopAddress == 0x30)  || (StopAddress == 0x32)  || (StopAddress == 0x34)  || (StopAddress == 0x36)   || (StopAddress == 0x38)  || (StopAddress == 0x3a)   || (StopAddress == 0x3c)  || (StopAddress == 0x3e)  || \
		   (StopAddress == 0x40)  || (StopAddress == 0x42)  || (StopAddress == 0x44)  || (StopAddress == 0x46)   || (StopAddress == 0x48)  || (StopAddress == 0x4a)   || (StopAddress == 0x4c)  || (StopAddress == 0x4e)  || \
		   (StopAddress == 0x50)  || (StopAddress == 0x52)  || (StopAddress == 0x54)   ) )err = err_add;//�Ĵ�����ַ����ȷ,���ֽ�����û��ȫ������
 	if(StopAddress > (KeepRegistorSize - 1))err = err_add;							//�Ĵ�����ַ����ȷ
 	if(StartAddress > (KeepRegistorSize - 1))err = err_add;							//�Ĵ�����ַ����ȷ
	 
	 
// 	U485TX;Delay_ms(10);
// 	printf("StartAddress:%d\r\n",StartAddress);
// 	printf("StopAddress:%d\r\n",StopAddress);
// 	printf("KeepRegistorSize:%d\r\n",KeepRegistorSize);
// 	printf("err:%d\r\n",err);
// 	printf("ModbusDataPackage.dat[0]:%d\r\n",ModbusDataPackage.dat[0]);
// 	printf("\r\n");
// 	U485RX;Delay_ms(100);
	
	if(  err != 0  )							//�����쳣����Ϣ
	{
		ModbusReturnAckInfo(err);														//��485�����쳣����Ϣ
		return ERROR;
	}
 	
 	KeepRegistorDataHton();																//��С�����ݴ���,���ڻ�����
 	/*��ȡ���ּĴ������ݲ���485��������*/
  	
	temp[0] = KeepRegister.DeviceAddress;								//��ֵ�豸��ַ
  	temp[1] = ModbusDataPackage.dat[1];									//��ֵ������
  	temp[2] = bytes;													//��ֵ���ݳ���(�ֽ���)
  	memcpy(&temp[3],(uint8_t *)&KeepRegisterTemp+StartAddress*2,bytes);	//�ַ���copy
  	
  	crc=CRC16_Check(temp,bytes+3);										//ִ��crcУ��
  	temp[bytes+3]=crc & 0xff;
  	temp[bytes+4]=crc>>8;
 	if(ModbusDataPackage.dat[0]) U485SendData(temp,bytes+5);			//��������
	
	return SUCCESS;	
}

/********************************
������Ĵ���
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x04)+��ʼ��ַ(2�ֽ�)+��ȡ�Ĵ�������(2�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x04)+���ݳ���(�ֽ���,1�ֽ�)+����(n�ֽ�)+У��(2�ֽ�)
*********************************/
char ModbusReadInputRegistor(unsigned char RX_Len)
{
	uint8_t err=0;
	uint8_t temp[255];
	uint16_t crc;
	
	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3];	//��ȡ��ʼ��ַ
	uint16_t RegVal = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];		//��ȡҪ��ȡ�ļĴ�������
	uint16_t StopAddress = StartAddress + RegVal - 1;									//��ȡ������ַ
	uint16_t InputRegistorSize = sizeof(InputRegister) / 2;								//���㱣�ּĴ���������
	uint16_t bytes = RegVal*2;															//����Ҫ��ȡ���ֽ���
	
	if(RX_Len != 8)err = err_OE;										//��Ч���������쳣
	if(!RegVal)err = err_add;											//�Ĵ�����ַ����ȷ,��ȡ�����������1
	
	if( ((StartAddress == 0x11) || (StartAddress == 0x14) || (StartAddress == 0x16) || (StartAddress == 0x18) || (StartAddress == 0x1a) || (StartAddress == 0x1c) || \
		 (StartAddress == 0x1e) || (StartAddress == 0x20) || (StartAddress == 0x22) ) )err = err_add;//�Ĵ�����ַ����ȷ,���ֽ�����Ҫ��ͷ����
	if( ((StopAddress == 0x10 ) || (StopAddress == 0x13 ) || (StopAddress == 0x15 ) || (StopAddress == 0x17 ) || (StopAddress == 0x19 ) || (StopAddress == 0x1b ) || \
		 (StopAddress == 0x1d ) || (StopAddress == 0x1f ) || (StopAddress == 0x21 ) ) )err = err_add;//�Ĵ�����ַ����ȷ,���ֽ�����û��ȫ������
	
	if(StopAddress > (InputRegistorSize - 1))err = err_add;	//�Ĵ�����ַ����ȷ
	if(StartAddress > (InputRegistorSize - 1))err = err_add;	//�Ĵ�����ַ����ȷ
	if(  err != 0 )						//�����쳣����Ϣ
	{
		ModbusReturnAckInfo(err);								//��485�����쳣����Ϣ
		return ERROR;
	}
	
	
	InputRegistorDataHton();									//��С�����ݴ���,���ڻ�����
	//��ȡ����Ĵ������ݲ���485��������
	
	temp[0] = KeepRegister.DeviceAddress;						//��ֵ�豸��ַ
	temp[1] = ModbusDataPackage.dat[1];							//��ֵ������
	temp[2] = bytes;											//��ֵ���ݳ���(�ֽ���)
	memcpy(&temp[3],(uint8_t *)&InputRegisterTemp+StartAddress*2,bytes);
	
	crc=CRC16_Check(temp,bytes+3);
	temp[bytes+3]=crc & 0xff;
	temp[bytes+4]=crc>>8;
	if(ModbusDataPackage.dat[0]) U485SendData(temp,bytes+5);
	
	return SUCCESS;	
}

/********************************
д�����Ĵ���,���ּĴ���
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x06)+�Ĵ�����ַ(2�ֽ�)+�Ĵ�����ֵ(2�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x06)+�Ĵ�����ַ(2�ֽ�)+�Ĵ�����ֵ(2�ֽ�)+У��(2�ֽ�)
*********************************/
char ModbusWriteSingleRegistor(unsigned char RX_Len)
{
	uint8_t err=0;
	uint8_t temp[10];
	uint8_t flag=0;
	uint16_t crc,dat;
	//��ȡ��ز���
	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3];	//��ȡ��ʼ��ַ
	dat = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];					//��ȡҪд�������
	
	//�����Ϸ����
	if(RX_Len != 8)err = err_OE;	//��Ч���������쳣
	if((StartAddress != 0) && (StartAddress != 1) && (StartAddress != 6) && (StartAddress != 7) && (StartAddress != 0x56) && (StartAddress != 0x57))err = err_add;			//�쳣��,�Ĵ�����ʼ��ַ����ȷ,���ֽ����ݲ����ô˹�����
	//������Ч��Χ�жϲ�д��
	
	
	
	//if(StopAddress > 0x09)flag=1;		//�ж�����Σ�ռĴ�����־λ
	if(StartAddress > 0x09)flag=1;		//�ж�����Σ�ռĴ�����־λ
	if( (SuperMode_Flag==0) && (flag==1) ){ModbusReturnAckInfo(err_mode);return ERROR;}		//�������ͨģʽ��ͬʱ������Σ�ռĴ������򷵻ش���
	
	
	switch(StartAddress)
	{
		case 0:
			if((dat == 0) || (dat > 247)){ModbusReturnAckInfo(3);	return ERROR;}	//��ַ���ݳ�����Χ,�����쳣������,�Ĵ���ֵ������Χ
			KeepRegister.DeviceAddress = dat;
			EEWrite(KREEPROM_BASEADDR,(void *)&dat,2);//��������
			break;
		case 1:
			KeepRegister.DeviceGroupNum = dat;
			EEWrite(KREEPROM_BASEADDR+2,(void *)&dat,2);//��������
			break;
		case 6:
			if( ((dat == 0x14) || (dat == 0x64)) ==0 ){ModbusReturnAckInfo(3);	return ERROR;}
			KeepRegister.Sensor_Range = dat;
			EEWrite(KREEPROM_BASEADDR+12,(void *)&dat,2);//��������
			break;
		case 7:
			if( ( ((dat >>8) == 0x01) || ((dat >>8) == 0x02) || ((dat >>8) ==0x03) || ((dat >>8) ==0x04) || ((dat >>8)==0x05) ) ==0  ){ModbusReturnAckInfo(3);	return ERROR;}   //����������ض�ֵ�����ش���
			//if( ( ((dat & 0x00FF)==0x00) || ((dat & 0x00FF)==0x01))  ==0  ){ModbusReturnAckInfo(3);	return ERROR;}
			KeepRegister.Liquid_Sec = dat;
			EEWrite(KREEPROM_BASEADDR+14,(void *)&dat,2);//��������
			break;
		case 0x56:
			if(  (dat < 0x00) || (dat >256)  ){ModbusReturnAckInfo(3);	return ERROR;}   //����������ض�ֵ�����ش���
			KeepRegister.Average_num = dat;
			EEWrite(KREEPROM_BASEADDR+172,(void *)&dat,2);//��������
			break;
		case 0x57:
			if(  (dat < 0x01) || (dat >0x07)  ){ModbusReturnAckInfo(3);	return ERROR;}   //����������ض�ֵ�����ش���
			KeepRegister.Average_num = dat;
			EEWrite(KREEPROM_BASEADDR+174,(void *)&dat,2);//��������
			break;
		default:
			ModbusReturnAckInfo(3);	
			return ERROR;
			break;	
	}			
	
//	if(StartAddress == 0)							//���д�豸��ַ����
//	{
//		if((dat == 0) || (dat > 247)){ModbusReturnAckInfo(3);	return ERROR;}	//��ַ���ݳ�����Χ,�����쳣������,�Ĵ���ֵ������Χ
//		KeepRegister.DeviceAddress = dat;
//		EEWrite(KREEPROM_BASEADDR,(void *)&dat,2);//��������
//	}
//	
//	if(StartAddress == 1)							//���д�豸���
//	{	
//		KeepRegister.DeviceGroupNum = dat;
//		EEWrite(KREEPROM_BASEADDR+2,(void *)&dat,2);//��������
//	}
//	
//	if(StartAddress == 6)							//���д����������
//	{
//		if( ((dat == 0x14) || (dat == 0x64)) ==0 ){ModbusReturnAckInfo(3);	return ERROR;}
//		KeepRegister.Sensor_Range = dat;
//		EEWrite(KREEPROM_BASEADDR+12,(void *)&dat,2);//��������
//	}
//	
//	if(StartAddress == 7)							//���
//	{
//		if( ( ((dat >>8) == 0x01) || ((dat >>8) == 0x02) || ((dat >>8) ==0x03) || ((dat >>8) ==0x04) || ((dat >>8)==0x05) ) ==0  ){ModbusReturnAckInfo(3);	return ERROR;}   //����������ض�ֵ�����ش���
//		//if( ( ((dat & 0x00FF)==0x00) || ((dat & 0x00FF)==0x01))  ==0  ){ModbusReturnAckInfo(4);	return ERROR;}
//		KeepRegister.Liquid_Sec = dat;
//		EEWrite(KREEPROM_BASEADDR+14,(void *)&dat,2);//��������
//	}
//	
//	if(StartAddress == 0x56)							//ƽ������
//	{
//		if(  (dat < 0x00) || (dat >256)  ){ModbusReturnAckInfo(3);	return ERROR;}   //����������ض�ֵ�����ش���
//		KeepRegister.Average_num = dat;
//		EEWrite(KREEPROM_BASEADDR+172,(void *)&dat,2);//��������
//	}
//	if(StartAddress == 0x57)							//ƽ������
//	{
//		if(  (dat < 0x01) || (dat >0x07)  ){ModbusReturnAckInfo(3);	return ERROR;}   //����������ض�ֵ�����ش���
//		KeepRegister.Average_num = dat;
//		EEWrite(KREEPROM_BASEADDR+174,(void *)&dat,2);//��������
//	}
	
	if(  err != 0 )			//�����쳣����Ϣ
	{
		ModbusReturnAckInfo(err);					//��485�����쳣����Ϣ
		return ERROR;
	}
	//ָ���
	
	temp[0] = KeepRegister.DeviceAddress;			//��ֵ�豸��ַ
	temp[1] = ModbusDataPackage.dat[1];				//��ֵ������
	memcpy(&temp[2],(uint8_t *)&ModbusDataPackage.dat[2],4);
	//У��
	crc = CRC16_Check(temp,6);						//crcУ��
	temp[6] = crc & 0xff;							//crc��λ��ǰ
	temp[7] = crc >> 8;								//��λ�ں�
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);//��������
	
	return SUCCESS;
}
/********************************
д����Ĵ���,���ּĴ���
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x10)+�Ĵ�����ַ(2�ֽ�)+�Ĵ�������(2�ֽ�)+�ֽ���(1�ֽ�)+�Ĵ�����ֵ(N������)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x10)+�Ĵ�����ַ(2�ֽ�)+�Ĵ�������(2�ֽ�)+У��(2�ֽ�)
*********************************/
char ModbusWriteSomeRegistor(void)
{
	uint8_t err=0;
	uint8_t temp[10];
	//uint8_t t[180];
	uint8_t flag=0;
	uint16_t crc;
	//��ȡ��ز���
	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3];	//��ȡ��ʼ��ַ
	uint16_t RegVal = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];		//��ȡҪд��ļĴ�������
	uint8_t bytes = ModbusDataPackage.dat[6];											//��ȡ�ֽ���
	uint16_t StopAddress = StartAddress + RegVal - 1;									//��ȡ������ַ
	uint16_t KeepRegistorSize = sizeof(KeepRegister) / 2;								//���㱣�ּĴ���������
	
	//U485TX;Delay_ms(10);
	
	//printf("����ֵ:mm  ������߶�ֵ:mm\r\n");
	//Delay_ms(100);U485RX;Delay_ms(10);
	
	//�����Ϸ����
	if(!RegVal || !bytes)err = err_add;													//�Ĵ�����ַ����ȷ,��ȡ�����������1
	if( (  (StartAddress == 0x03) || (StartAddress == 0x05) || (StartAddress == 0x09) || (StartAddress == 0x0b)  || (StartAddress == 0x0d) || (StartAddress == 0x0f)  || \
		   (StartAddress == 0x11) || (StartAddress == 0x13) || (StartAddress == 0x15) || (StartAddress == 0x17)  || (StartAddress == 0x19) || (StartAddress == 0x1b)  || (StartAddress == 0x1d) || (StartAddress == 0x1f) || \
	       (StartAddress == 0x21) || (StartAddress == 0x23) || (StartAddress == 0x25) || (StartAddress == 0x27)  || (StartAddress == 0x29) || (StartAddress == 0x2b)  || (StartAddress == 0x2d) || (StartAddress == 0x2f) || \
	       (StartAddress == 0x31) || (StartAddress == 0x33) || (StartAddress == 0x35) || (StartAddress == 0x37)  || (StartAddress == 0x39) || (StartAddress == 0x3b)  || (StartAddress == 0x3d) || (StartAddress == 0x3f) || \
	       (StartAddress == 0x41) || (StartAddress == 0x43) || (StartAddress == 0x45) || (StartAddress == 0x47)  || (StartAddress == 0x49) || (StartAddress == 0x4b)  || (StartAddress == 0x4d) || (StartAddress == 0x4f) || \
	       (StartAddress == 0x51) || (StartAddress == 0x53) || (StartAddress == 0x55)  ) )err = err_add;//�Ĵ�����ַ����ȷ,���ֽ�����Ҫ��ͷ����
	if( (  (StopAddress == 0x02)  || (StopAddress == 0x04)  || (StopAddress == 0x08)  || (StopAddress == 0x0a)   || (StopAddress == 0x0c)  || (StopAddress == 0x0e)   || \
	       (StopAddress == 0x10)  || (StopAddress == 0x12)  || (StopAddress == 0x14)  || (StopAddress == 0x16)   || (StopAddress == 0x18)  || (StopAddress == 0x1a)   || (StopAddress == 0x1c)  || (StopAddress == 0x1e)  || \
	       (StopAddress == 0x20)  || (StopAddress == 0x22)  || (StopAddress == 0x24)  || (StopAddress == 0x26)   || (StopAddress == 0x28)  || (StopAddress == 0x2a)   || (StopAddress == 0x2c)  || (StopAddress == 0x2e)  || \
	       (StopAddress == 0x30)  || (StopAddress == 0x32)  || (StopAddress == 0x34)  || (StopAddress == 0x36)   || (StopAddress == 0x38)  || (StopAddress == 0x3a)   || (StopAddress == 0x3c)  || (StopAddress == 0x3e)  || \
	       (StopAddress == 0x40)  || (StopAddress == 0x42)  || (StopAddress == 0x44)  || (StopAddress == 0x46)   || (StopAddress == 0x48)  || (StopAddress == 0x4a)   || (StopAddress == 0x4c)  || (StopAddress == 0x4e)  || \
	       (StopAddress == 0x50)  || (StopAddress == 0x52)  || (StopAddress == 0x54)   ) )err = err_add;//�Ĵ�����ַ����ȷ,���ֽ�����û��ȫ������
	if(StopAddress > (KeepRegistorSize - 1))err = err_add;		//�Ĵ�����ַ����ȷ
	if(StartAddress > (KeepRegistorSize - 1))err = err_add;		//�Ĵ�����ַ����ȷ
	
	if(StopAddress > 0x09)flag=1;		//�ж�����Σ�ռĴ�����־λ
	if(StartAddress > 0x09)flag=1;		//�ж�����Σ�ռĴ�����־λ
	
	
	if( (SuperMode_Flag==0) && (flag==1) )err=err_mode;		//�������ͨģʽ��ͬʱ������Σ�ռĴ������򷵻ش���
	
	if(  err != 0 )							//�����쳣����Ϣ
	{
		ModbusReturnAckInfo(err);									//��485�����쳣����Ϣ
		return ERROR;
	}
	
	memcpy((uint8_t *)&KeepRegisterTemp,(uint8_t *)&KeepRegister,sizeof(KeepRegister));					//�Ƚ���ǰ�Ĵ������ݿ�����������
	memcpy((uint8_t *)&KeepRegisterTemp + StartAddress*2,(uint8_t *)&ModbusDataPackage.dat[7],bytes);	//Ȼ�󽫽��յ����ݿ������Ĵ���������
	err = 0;
	//��С��ת��
	if(StartAddress == 0x00)KeepRegisterTemp.DeviceAddress = htons(KeepRegisterTemp.DeviceAddress);														//���豸��ַ���ݴ�С��ת��
	if((StartAddress <= 0x01) && (StopAddress >= 0x01))KeepRegisterTemp.DeviceGroupNum = htons(KeepRegisterTemp.DeviceGroupNum );										//����������1���ݴ�С��ת��
	if((StartAddress <= 0x02) && (StopAddress >= 0x03))KeepRegisterTemp.OriginaLiquidAltitude_After = __ltobf(KeepRegisterTemp.OriginaLiquidAltitude_After);				//��Һλ��ʼ�߶����ݴ�С��ת��
	if((StartAddress <= 0x04) && (StopAddress >= 0x05))KeepRegisterTemp.OriginaLiquidAltitude_Befor = __ltobf(KeepRegisterTemp.OriginaLiquidAltitude_Befor );							//��Һ���ܶ����ݴ�С��ת��
	if((StartAddress <= 0x06) && (StopAddress >= 0x06))KeepRegisterTemp.Sensor_Range = htons(KeepRegisterTemp.Sensor_Range );									//����������2���ݴ�С��ת��
	if((StartAddress <= 0x07) && (StopAddress >= 0x07))KeepRegisterTemp.Liquid_Sec = htons(KeepRegisterTemp.Liquid_Sec );											//����������3���ݴ�С��ת��
	if((StartAddress <= 0x08) && (StopAddress >= 0x09))KeepRegisterTemp.LocalAccelerationOfGravity = __ltobf(KeepRegisterTemp.LocalAccelerationOfGravity );	//�������������ٶ����ݴ�С��ת��
	if((StartAddress <= 0x0a) && (StopAddress >= 0x0b))KeepRegisterTemp.Sensor_FS_Val = __ltobf(KeepRegisterTemp.Sensor_FS_Val );							//
	if((StartAddress <= 0x0c) && (StopAddress >= 0x0d))KeepRegisterTemp.MV[0] = __ltobf(KeepRegisterTemp.MV[0] );
	if((StartAddress <= 0x0e) && (StopAddress >= 0x0f))KeepRegisterTemp.MV[1] = __ltobf(KeepRegisterTemp.MV[1] );
	if((StartAddress <= 0x10) && (StopAddress >= 0x11))KeepRegisterTemp.MV[2] = __ltobf(KeepRegisterTemp.MV[2] );
	if((StartAddress <= 0x12) && (StopAddress >= 0x13))KeepRegisterTemp.MV[3] = __ltobf(KeepRegisterTemp.MV[3] );
	if((StartAddress <= 0x14) && (StopAddress >= 0x15))KeepRegisterTemp.MV[4] = __ltobf(KeepRegisterTemp.MV[4] );
	if((StartAddress <= 0x16) && (StopAddress >= 0x17))KeepRegisterTemp.MV[5] = __ltobf(KeepRegisterTemp.MV[5] );
	if((StartAddress <= 0x18) && (StopAddress >= 0x19))KeepRegisterTemp.Temp_T0 = __ltobf(KeepRegisterTemp.Temp_T0 );
	if((StartAddress <= 0x1a) && (StopAddress >= 0x1b))KeepRegisterTemp.LTC0[0] = __ltobf(KeepRegisterTemp.LTC0[0] );
	if((StartAddress <= 0x1c) && (StopAddress >= 0x1d))KeepRegisterTemp.LTC0[1] = __ltobf(KeepRegisterTemp.LTC0[1] );
	if((StartAddress <= 0x1e) && (StopAddress >= 0x1f))KeepRegisterTemp.LTC0[2] = __ltobf(KeepRegisterTemp.LTC0[2] );
	if((StartAddress <= 0x20) && (StopAddress >= 0x21))KeepRegisterTemp.LTC0[3] = __ltobf(KeepRegisterTemp.LTC0[3] );
	if((StartAddress <= 0x22) && (StopAddress >= 0x23))KeepRegisterTemp.LTC0[4] = __ltobf(KeepRegisterTemp.LTC0[4] );
	
	if((StartAddress <= 0x24) && (StopAddress >= 0x25))KeepRegisterTemp.LTC1[0] = __ltobf(KeepRegisterTemp.LTC1[0] );
	if((StartAddress <= 0x26) && (StopAddress >= 0x27))KeepRegisterTemp.LTC1[1] = __ltobf(KeepRegisterTemp.LTC1[1] );
	if((StartAddress <= 0x28) && (StopAddress >= 0x29))KeepRegisterTemp.LTC1[2] = __ltobf(KeepRegisterTemp.LTC1[2] );
	if((StartAddress <= 0x2a) && (StopAddress >= 0x2b))KeepRegisterTemp.LTC1[3] = __ltobf(KeepRegisterTemp.LTC1[3] );
	if((StartAddress <= 0x2c) && (StopAddress >= 0x2d))KeepRegisterTemp.LTC1[4] = __ltobf(KeepRegisterTemp.LTC1[4] );
	
	if((StartAddress <= 0x2e) && (StopAddress >= 0x2f))KeepRegisterTemp.LTC2[0] = __ltobf(KeepRegisterTemp.LTC2[0] );
	if((StartAddress <= 0x30) && (StopAddress >= 0x31))KeepRegisterTemp.LTC2[1] = __ltobf(KeepRegisterTemp.LTC2[1] );
	if((StartAddress <= 0x32) && (StopAddress >= 0x33))KeepRegisterTemp.LTC2[2] = __ltobf(KeepRegisterTemp.LTC2[2] );
	if((StartAddress <= 0x34) && (StopAddress >= 0x35))KeepRegisterTemp.LTC2[3] = __ltobf(KeepRegisterTemp.LTC2[3] );
	if((StartAddress <= 0x36) && (StopAddress >= 0x37))KeepRegisterTemp.LTC2[4] = __ltobf(KeepRegisterTemp.LTC2[4] );
	
	if((StartAddress <= 0x38) && (StopAddress >= 0x39))KeepRegisterTemp.LTC3[0] = __ltobf(KeepRegisterTemp.LTC3[0] );
	if((StartAddress <= 0x3a) && (StopAddress >= 0x3b))KeepRegisterTemp.LTC3[1] = __ltobf(KeepRegisterTemp.LTC3[1] );
	if((StartAddress <= 0x3c) && (StopAddress >= 0x3d))KeepRegisterTemp.LTC3[2] = __ltobf(KeepRegisterTemp.LTC3[2] );
	if((StartAddress <= 0x3e) && (StopAddress >= 0x3f))KeepRegisterTemp.LTC3[3] = __ltobf(KeepRegisterTemp.LTC3[3] );
	if((StartAddress <= 0x40) && (StopAddress >= 0x41))KeepRegisterTemp.LTC3[4] = __ltobf(KeepRegisterTemp.LTC3[4] );
	
	if((StartAddress <= 0x42) && (StopAddress >= 0x43))KeepRegisterTemp.LTC4[0] = __ltobf(KeepRegisterTemp.LTC4[0] );
	if((StartAddress <= 0x44) && (StopAddress >= 0x45))KeepRegisterTemp.LTC4[1] = __ltobf(KeepRegisterTemp.LTC4[1] );
	if((StartAddress <= 0x46) && (StopAddress >= 0x47))KeepRegisterTemp.LTC4[2] = __ltobf(KeepRegisterTemp.LTC4[2] );
	if((StartAddress <= 0x48) && (StopAddress >= 0x49))KeepRegisterTemp.LTC4[3] = __ltobf(KeepRegisterTemp.LTC4[3] );
	if((StartAddress <= 0x4a) && (StopAddress >= 0x4b))KeepRegisterTemp.LTC4[4] = __ltobf(KeepRegisterTemp.LTC4[4] );
	
	if((StartAddress <= 0x4c) && (StopAddress >= 0x4d))KeepRegisterTemp.LTC5[0] = __ltobf(KeepRegisterTemp.LTC5[0] );
	if((StartAddress <= 0x4e) && (StopAddress >= 0x4f))KeepRegisterTemp.LTC5[1] = __ltobf(KeepRegisterTemp.LTC5[1] );
	if((StartAddress <= 0x50) && (StopAddress >= 0x51))KeepRegisterTemp.LTC5[2] = __ltobf(KeepRegisterTemp.LTC5[2] );
	if((StartAddress <= 0x52) && (StopAddress >= 0x53))KeepRegisterTemp.LTC5[3] = __ltobf(KeepRegisterTemp.LTC5[3] );
	if((StartAddress <= 0x54) && (StopAddress >= 0x55))KeepRegisterTemp.LTC5[4] = __ltobf(KeepRegisterTemp.LTC5[4] );
	if((StartAddress <= 0x56) && (StopAddress >= 0x56))KeepRegisterTemp.Average_num = htons(KeepRegisterTemp.Average_num );
	if((StartAddress <= 0x57) && (StopAddress >= 0x57))KeepRegisterTemp.bps = htons(KeepRegisterTemp.bps );
	
	//�ж�������Ч��
	if((KeepRegisterTemp.DeviceAddress == 0) || (KeepRegisterTemp.DeviceAddress > 247))	err = err_Re_VOR;	//��ַ���ݳ�����Χ,�����쳣������,�Ĵ���ֵ������Χ
	if((KeepRegisterTemp.Average_num < 0) || (KeepRegisterTemp.Average_num > 256))		err = err_Re_VOR;
	if((KeepRegisterTemp.bps< 0x01) || (KeepRegisterTemp.bps > 0x07))					err = err_Re_VOR;
	if((KeepRegisterTemp.LocalAccelerationOfGravity< 9.78) || (KeepRegisterTemp.LocalAccelerationOfGravity > 10.0))err = err_Re_VOR;
	if(!err)																						//����޴���,�򽫻�������ݿ������Ĵ�����
	{
		memcpy((uint8_t *)&KeepRegister,(uint8_t *)&KeepRegisterTemp,sizeof(KeepRegister));
		EEWrite(KREEPROM_BASEADDR,(void *)&KeepRegisterTemp,sizeof(KeepRegister));					//�������ݣ����������Ĵ�����
		
//		EEWrite(KREEPROM_BASEADDR,(void *)&KeepRegister.DeviceAddress,24);//����EEPROM
//		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
//		EEWrite(KREEPROM_BASEADDR+24,(void *)&KeepRegister.MV[0],28);
//		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
//	
//		EEWrite(KREEPROM_BASEADDR+52,(void *)&KeepRegister.LTC0[0],120);
//		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
//		
//		EEWrite(KREEPROM_BASEADDR+172,(void *)&KeepRegister.Average_num,4);//��������
//		GPIO_PinReverse(GPIOA,GPIO_Pin_1);

	}
	else if(ModbusDataPackage.dat[0])//������ݷ�Χ����
	{
		ModbusReturnAckInfo(err);	 //��485�����쳣����Ϣ
		return ERROR;
	}
	
	//ָ���
	temp[0] = KeepRegister.DeviceAddress;					//��ֵ�豸��ַ
	temp[1] = ModbusDataPackage.dat[1];						//��ֵ������
	memcpy(&temp[2],(uint8_t *)&ModbusDataPackage.dat[2],4);//��ֵ�Ĵ�����ַ������,��4�ֽ�
	//У��
	crc = CRC16_Check(temp,6);								//crcУ��
	temp[6] = crc & 0xff;									//crc��λ��ǰ
	temp[7] = crc >> 8;										//��λ�ں�
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);		//��������
	
	return SUCCESS;
}



/********************************
�趨��ֵ����ɺ�д���ּĴ���
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x41)+������(4�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x41)+���޸ļĴ����׵�ַ(2�ֽ�)+���޸ļĴ�������(2�ֽ�)+У��(2�ֽ�)
������:0x44,0x66,0x88,0xaa
*********************************/
char ModbusSetInitalValue(unsigned char RX_Len)
{
	uint8_t err=0;
	uint8_t temp[10];
	uint16_t crc;
	//float OriginaAltitudeTemp=0;
	//SenPow_ON;Delay_ms(200);
	if(RX_Len !=8 )err = err_OE;
	if( strncmp(FactorySetValueWord,(unsigned char *)&ModbusDataPackage.dat[2],4) !=0 )err=err_OE;
	if(  (err != 0)  &&  (ModbusDataPackage.dat[0] != 0) )
	{
		ModbusReturnAckInfo(err);
		return ERROR;
	}
	
	KeepRegister.OriginaLiquidAltitude_Befor = InputRegister.LiquidAltitudeAbsoluteValue_Befor;
	KeepRegister.OriginaLiquidAltitude_After = InputRegister.LiquidAltitudeAbsoluteValue_After;    //ͬʱ�����ݱ�����EEPROM
	KeepRegister.Temp_T0 = InputRegister.Temperature;
	
	//log_info("%f,%f,%f\r\n",KeepRegister.OriginaLiquidAltitude_Befor,KeepRegister.OriginaLiquidAltitude_After,KeepRegister.Temp_T0);
	
	EEWrite(KREEPROM_BASEADDR+4,(void *)&KeepRegister.OriginaLiquidAltitude_After,4);
	EEWrite(KREEPROM_BASEADDR+8,(void *)&KeepRegister.OriginaLiquidAltitude_Befor,4);
	EEWrite(KREEPROM_BASEADDR+0x18*2,(void *)&KeepRegister.Temp_T0,4);
	
	temp[0] = KeepRegister.DeviceAddress;
	temp[1] = ModbusDataPackage.dat[1];
	temp[2] = 0x00;
	temp[3] = 0x02;
	temp[4] = 0x00;
	temp[5] = 0x04;
	
	crc = CRC16_Check(temp,6);
	
	temp[6] = crc &0xff;
	temp[7] = crc >> 8;
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);
	
	//SenPow_OFF;
	return SUCCESS;
}

/********************************
������У׼��У׼��ɺ�д���ּĴ���
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x43)+������(4�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x43)+���޸ļĴ����׵�ַ(2�ֽ�)+���޸ļĴ�������(2�ֽ�)+У��(2�ֽ�)
�����룺����������Ϊ��0x43��0x44��0x45��0x46��0x47��0x48
������:0x44,0x66,0x88,0xaa:�û�У׼
������:0x40,0x66,0x88,0xaa:����У׼
*********************************/
char FactoryCalibration(unsigned char RX_Len)
{
	uint8_t i,err=0;
	uint8_t temp[10];
	uint8_t function_code=0x00;
	uint16_t crc;
	float ADCValue=0;
	if(RX_Len !=8 )err = err_OE;
	if( (strncmp(FactorySetValueWord,(unsigned char *)&ModbusDataPackage.dat[2],4) !=0) && (strncmp(FactorySetValueWord2,(unsigned char *)&ModbusDataPackage.dat[2],4) !=0)  )err=err_OE;
	if(SuperMode_Flag==0)err=err_mode;//�������ͨģʽ���򷵻ش���
	if(  err != 0 )
	{
		ModbusReturnAckInfo(err);
		return ERROR;
	}
	
	function_code=ModbusDataPackage.dat[1]; //��ȡ������
	
	ADCValue = InputRegister.ADCOriginalValue;//ȡn��ƽ����mV
	
	switch (function_code)   //���ݹ��������Ӧ��У׼�����У׼
	{
		case FactoryCalibration0: KeepRegister.MV[0]=ADCValue;
								  EEWrite(KREEPROM_BASEADDR+24,(void *)&KeepRegister.MV[0],4);
								  KeepRegister.Temp_T0=InputRegister.Temperature;
								  EEWrite(KREEPROM_BASEADDR+48,(void *)&KeepRegister.Temp_T0,4);
								  log_info("FactoryCalibration0 Over!\r\n");
								  break;//���У׼��0��ͬʱ�����ݱ�����EEPROM
		case FactoryCalibration1: KeepRegister.MV[1]=ADCValue; EEWrite(KREEPROM_BASEADDR+28,(void *)&KeepRegister.MV[1],4);log_info("FactoryCalibration1 Over!\r\n");break;//���У׼��1��ͬʱ�����ݱ�����EEPROM
		case FactoryCalibration2: KeepRegister.MV[2]=ADCValue; EEWrite(KREEPROM_BASEADDR+32,(void *)&KeepRegister.MV[2],4);log_info("FactoryCalibration2 Over!\r\n");break;//���У׼��2��ͬʱ�����ݱ�����EEPROM
		case FactoryCalibration3: KeepRegister.MV[3]=ADCValue; EEWrite(KREEPROM_BASEADDR+36,(void *)&KeepRegister.MV[3],4);log_info("FactoryCalibration3 Over!\r\n");break;//���У׼��3��ͬʱ�����ݱ�����EEPROM
		case FactoryCalibration4: KeepRegister.MV[4]=ADCValue; EEWrite(KREEPROM_BASEADDR+40,(void *)&KeepRegister.MV[4],4);log_info("FactoryCalibration4 Over!\r\n");break;//���У׼��4��ͬʱ�����ݱ�����EEPROM
		case FactoryCalibration5: KeepRegister.MV[5]=ADCValue; EEWrite(KREEPROM_BASEADDR+44,(void *)&KeepRegister.MV[5],4);log_info("FactoryCalibration5 Over!\r\n");break;//���У׼��5��ͬʱ�����ݱ�����EEPROM
		default: break;
	}
	
	
	temp[0] = KeepRegister.DeviceAddress;
	temp[1] = ModbusDataPackage.dat[1];
	temp[2] = 0x00;
	temp[3] = 0x0a;
	temp[4] = 0x00;
	temp[5] = 0x02;
	
	crc = CRC16_Check(temp,6);
	
	temp[6] = crc &0xff;
	temp[7] = crc >> 8;
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);
	
	//SenPow_OFF;
	return SUCCESS;
}

/***********************************************************************
��ȡSN��Ϣ
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x40)+������(4�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x40)+��������(1�ֽ�)+SN��Ϣ(15�ֽ�)+У��(2�ֽ�)
������:0x33,0x55,0x77,0x99
************************************************************************/
//uint16_t TOS_NODE_ID=111;
//const uint8_t __progmem_smartbow_start[64] = {"&&SUPERHHH111401&&2"};
char Get_SNInfo_Fun(void)
{
	unsigned char i;
	//unsigned char err=0;
	//uint16_t crc;
	unsigned char SN_Info[16]={0};
	//unsigned char temp[19]={0};
	//volatile unsigned char reservalData = TOS_NODE_ID;
	
//	if(ModbusDataPackage.DataLen !=8 )err = err_OE;
//	if( strncmp(FactorySetValueWord,(unsigned char *)&ModbusDataPackage.dat[2],4) !=0 )err=err_OE;
//	
//	if(  (err != 0)  && (ModbusDataPackage.dat[0] != 0) )//���ж�
//	{
//		ModbusReturnAckInfo(err);
//		return ERROR;
//	}
	
	
	
//	
//	for(i=0; i<19; i++)
//	{
//		temp[i] = __progmem_smartbow_start[i];
//	}
//	
//	for(i=0; i<14; i++)
//	{
//		SN_Info[i] = temp[i+2];
//	}
//	SN_Info[14]=temp[18];
//	SN_Info[15]=0;
	
	EERead_Z(EEPROM_SHARE_DATA_ADDR,SN_Info,16);//��ȡEEPROM�й����ֶ��е�SN��Ϣ
	
	for(i=0;i<8;i++)
	{
		InputRegister.SN[i]=SN_Info[i*2]<<8 | SN_Info[i*2+1];
		//InputRegisterTemp.SN[i]=htons(InputRegisterTemp.SN[i]);
	}
	
	//memcpy((uint8_t *)&InputRegister,(uint8_t *)&InputRegisterTemp,sizeof(InputRegister));
	

	
//	RS485_TX();
//	for(i=0;i<8;i++)
//	{
//		InputRegisterTemp.SN[i]=SN_Info[i*2]<<8 | SN_Info[i*2+1];
//		InputRegisterTemp.SN[i]=htons(InputRegisterTemp.SN[i]);
//	}
//	log_info("SN_Info:%s\r\n",&SN_Info);
//	log_info("SN_Infi:%s\r\n",&InputRegisterTemp.SN[0]);
//	RS485_RX();
	
//	temp[0] = KeepRegister.DeviceAddress;
//	temp[1] = ModbusDataPackage.dat[1];
//	temp[2] = 0x0F;
//	
//	for(i=0;i<15;i++)
//	{
//		temp[i+3]=SN_Info[i];
//	}
//	
//	crc = CRC16_Check(temp,18);
//	
//	temp[18] = crc &0xff;
//	temp[19] = crc >> 8;
//	if(ModbusDataPackage.dat[0]) U485SendData(temp,20);
	
	return SUCCESS;
}

/********************************
ģʽ�л�
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x66)+������(4�ֽ�)+У��(2�ֽ�)

������:0x44,0x66,0x88,0xaa�л�Ϊ��ͨģʽ    ���ظ�ʽ:������ַ(1�ֽ�)+������(0x66)+0x44+0x66+0x88+0xaa+У��(2�ֽ�)
������:0x40,0x60,0x80,0xa0�л�Ϊ����ģʽ    ���ظ�ʽ:������ַ(1�ֽ�)+������(0x66)+0x40+0x60+0x80+0xa0+У��(2�ֽ�)
*********************************/
char ModbusSwitchMode(unsigned char RX_Len)
{
	uint8_t err=0;
	uint8_t temp[10];
	uint16_t crc;
	uint8_t temp2=3;
	
	if(RX_Len !=8 )err = err_OE;
	if( strncmp(SuperModeValueWord2,(unsigned char *)&ModbusDataPackage.dat[2],4) ==0 )//�л�Ϊ����ģʽ
	{
		temp2=1;		
	}
	else if( strncmp(FactorySetValueWord,(unsigned char *)&ModbusDataPackage.dat[2],4) ==0 )//�л�Ϊ��ͨģʽ
	{
		temp2=2;
	}
	else err=err_OE;
	
	if(  (err != 0)  &&  (ModbusDataPackage.dat[0] != 0) )
	{
		ModbusReturnAckInfo(err);
		return ERROR;
	}
	
		
	if( temp2==1 )//�л�Ϊ����ģʽ
	{
		SuperMode_Flag=1;
		temp[0] = 0x34;
		temp[1] = 0x12;
		EEWrite(SUPERMODE_FLAG_BASEADDR,temp,2);
		InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0x00ff)|0x0100;
		
		temp[0] = KeepRegister.DeviceAddress;
		temp[1] = ModbusDataPackage.dat[1];
		temp[2] = 0x40;
		temp[3] = 0x60;
		temp[4] = 0x80;
		temp[5] = 0xa0;
	}
	
	else if( temp2==2 )//�л�Ϊ��ͨģʽ
	{
		SuperMode_Flag=0;
		temp[0] = 0x00;
		temp[1] = 0x00;
		EEWrite(SUPERMODE_FLAG_BASEADDR,temp,2);
		InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0x00ff)|0x0200;
		
		temp[0] = KeepRegister.DeviceAddress;
		temp[1] = ModbusDataPackage.dat[1];
		temp[2] = 0x44;
		temp[3] = 0x66;
		temp[4] = 0x88;
		temp[5] = 0xaa;
	}
	
	crc = CRC16_Check(temp,6);
	
	temp[6] = crc &0xff;
	temp[7] = crc >> 8;
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);
	
	return SUCCESS;
}

/********************************
����¶�����ϵ�������¶�����ϵ��ȫ������
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x49)+������(4�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x49)+���޸ļĴ����׵�ַ(2�ֽ�)+���޸ļĴ�������(2�ֽ�)+У��(2�ֽ�)
������:0x44,0x66,0x88,0xaa
*********************************/
char CTC(unsigned char RX_Len)
{
	uint8_t err=0;
	uint8_t i=0;
	uint8_t temp[10];
	uint16_t crc;

	if(RX_Len !=8 )err = err_OE;
	if( strncmp(FactorySetValueWord,(unsigned char *)&ModbusDataPackage.dat[2],4) !=0 )err=err_OE;
	if(SuperMode_Flag==0)err=err_mode;
	if(  (err != 0)  && (ModbusDataPackage.dat[0] != 0) )//���ж�
	{
		ModbusReturnAckInfo(err);
		return ERROR;
	}
	
	//��ϵ��ȫ������
	for(i=0;i<5;i++) KeepRegister.LTC0[i]=0;
	for(i=0;i<5;i++) KeepRegister.LTC1[i]=0;
	for(i=0;i<5;i++) KeepRegister.LTC2[i]=0;
	for(i=0;i<5;i++) KeepRegister.LTC3[i]=0;
	for(i=0;i<5;i++) KeepRegister.LTC4[i]=0;
	for(i=0;i<5;i++) KeepRegister.LTC5[i]=0;
	
	EEWrite(KREEPROM_BASEADDR+52,(void *)&KeepRegister.LTC0[0],120);
	
	temp[0] = KeepRegister.DeviceAddress;
	temp[1] = ModbusDataPackage.dat[1];
	temp[2] = 0x00;
	temp[3] = 0x1A;
	temp[4] = 0x00;
	temp[5] = 0x3C;
	
	crc = CRC16_Check(temp,6);
	temp[6] = crc &0xff;
	temp[7] = crc >> 8;
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);
	
	return SUCCESS;
}

/********************************
�ָ���������
���ݸ�ʽ:������ַ(0)+������(0x68)+������(4�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x68)+�Ĵ�����ַ(2�ֽ�)+�Ĵ�������(2�ֽ�)+�ֽ���(1�ֽ�)+���б��ּĴ�������(N�ֽ�)+У��(2�ֽ�)
������:0x33,0x55,0x77,0x99
*********************************/
char ModbusFactoryParameterReset(unsigned char RX_Len)
{
	uint8_t err=0;
	uint8_t temp[255];
	uint16_t crc;
	if(RX_Len != 8)err = err_OE;//��Ч���������쳣
	if(strncmp(FactoryResetWord,(char *)&ModbusDataPackage.dat[2],4) != 0)err = err_OE;//��Ч���������쳣
	if(SuperMode_Flag==0)err=err_mode;
	if(  (err != 0)  && (ModbusDataPackage.dat[0] != 0) )	  //�����쳣����Ϣ
	{
		ModbusReturnAckInfo(err);			  //��485�����쳣����Ϣ
		return ERROR;
	}
	
	//KeepRegister_Num=0;
	//KeepRegister_Byte_Num=0;
	
	
	
	
//	EEErase(KREEPROM_BASEADDR,12);           //����EEPROMǰ12���ֽڣ�������ַ����š���ʼ�߶ȡ��ܶȡ��������ٶ�
//	EEErase(KREEPROM_BASEADDR+16,4);         //�����������ٶȡ�
//	
//	EEErase(FPOWERONFLAG_BASEADDR,2);        //�����״��ϵ��־λ
	
	KeepRegistorDataHton();				 //��С�����ݴ���,���ڻ�����
										 
	temp[0] = KeepRegister.DeviceAddress;//��ֵ�豸��ַ
	temp[1] = ModbusDataPackage.dat[1];	 //��ֵ������
	temp[2] = 0x00;
	temp[3] = 0x00;
	temp[4] = 0x00;
	temp[5] = KeepRegister_Num;          
	temp[6] = KeepRegister_Byte_Num;
	
	memcpy(&temp[7],(uint8_t *)&KeepRegisterTemp,KeepRegister_Byte_Num);//��ȡ����
	
	//У��
	crc = CRC16_Check(temp,KeepRegister_Byte_Num+7); //crcУ��
	temp[KeepRegister_Byte_Num+7] = crc & 0xff;		//crc��λ��ǰ
	temp[KeepRegister_Byte_Num+8] = crc >> 8;		//��λ�ں�
	
	
	KeepRegister.DeviceAddress              = DefaultDeviceADDR;
	KeepRegister.OriginaLiquidAltitude_After      = 0;
	KeepRegister.OriginaLiquidAltitude_Befor      = 0;
	KeepRegister.Liquid_Sec                = (Liquid_FD01_25<<8) | 0x00;
	KeepRegister.LocalAccelerationOfGravity = DefaultAccelerationOfGravity;
	KeepRegister.DeviceGroupNum = 0;
	KeepRegister.Sensor_Range = DefaultSensor_Range;
	KeepRegister.Average_num	=	3;
	KeepRegister.bps			=   3;
	
	EEWrite(KREEPROM_BASEADDR,(void *)&KeepRegister.DeviceAddress,20);//����EEPROM
	EEWrite(KREEPROM_BASEADDR+172,(void *)&KeepRegister.Average_num,2);//��������
	
	
	if(ModbusDataPackage.dat[0]) U485SendData(temp,KeepRegister_Byte_Num+9);//��������,����=��ȡ�ֽ���+ǰ�������ֽ�+����У�飻����ǹ㲥�����򲻻ظ���
	UART1_Configuration(KeepRegister.bps);
	//SystemResetSoft();
	//while(1);//�ȴ����Ź���λ��������ɻָ���������
	
	return SUCCESS;
}


/********************************
�趨��ֵ����ɺ�д���ּĴ���
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x41)+������(4�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x41)+���޸ļĴ����׵�ַ(2�ֽ�)+���޸ļĴ�������(2�ֽ�)+У��(2�ֽ�)
������:0x33,0x55,0x77,0x99
*********************************/
char ResetSoftware(unsigned short RX_Len)
{
	uint8_t err=0;
	uint8_t temp[10];
	uint16_t crc;
	float OriginaAltitudeTemp=0;
	if(RX_Len !=8 )err = err_OE;
	if( strncmp(FactoryResetWord,(unsigned char *)&ModbusDataPackage.dat[2],4) !=0 )err=err_OE;
	if(  (err != 0)  &&  (ModbusDataPackage.dat[0] != 0) )
	{
		ModbusReturnAckInfo(err);
		return ERROR;
	}
	
	temp[0] = KeepRegister.DeviceAddress;
	temp[1] = ModbusDataPackage.dat[1];
	temp[2] = 0x00;
	temp[3] = 0x00;
	temp[4] = 0x00;
	temp[5] = 0x00;
	
	crc = CRC16_Check(temp,6);
	
	temp[6] = crc &0xff;
	temp[7] = crc >> 8;
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);
	
	SystemResetSoft();
	while(1);
	
	
	return SUCCESS;
}




void Instruction_Process_Subfunction(unsigned short RX_Len)
{
	if(ModbusDataPackage.dat[0] == KeepRegister.DeviceAddress)//�Ǳ���ָ��
	{
		switch(ModbusDataPackage.dat[1])
		{
			case ReadKeepRegistor:			//�����ּĴ���
						ModbusReadKeepRegistor(RX_Len);
						//log_info("ModbusReadKeepRegistor\r\n");
						//InputRegister.SystemWorkStatus=0x01;
						break;
			case ReadInputRegistor:			//������Ĵ���
						ModbusReadInputRegistor(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x02;
						//log1_info("ReadInputRegistor\r\n");
						break;
			case WriteSingleRegistor:		//д�������ּĴ���
						ModbusWriteSingleRegistor(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x03;
						//log_info("WriteSingleRegistor\r\n");
						break;
			case SoftWare_Reset:
						ResetSoftware(RX_Len);
						break;
			case WriteSomeRegistor:			//д������ּĴ���
						ModbusWriteSomeRegistor();
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x04;
						//log_info("WriteSomeRegistor\r\n");
						break;
			case AutoSetInitalValue:		//�Զ����ó�ֵ
						ModbusSetInitalValue(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x05;
						//log_info("AutoSetInitalValue\r\n");
						break;
			case ModeSwitch:
						ModbusSwitchMode(RX_Len);		//ģʽ�л�
						break;
			case FactoryCalibration0:
			case FactoryCalibration1:
			case FactoryCalibration2:
			case FactoryCalibration3:
			case FactoryCalibration4:
			case FactoryCalibration5:		
						FactoryCalibration(RX_Len);			
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x07;
						break;//����У׼
			case Clear_temp_corr:                //���У׼ϵ��
						CTC(RX_Len);
						//log_info("Clear_temp_corr\r\n");
						break;
			case FactoryParameterReset:		//�ָ���������
						ModbusFactoryParameterReset(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x09;
						//log_info("FactoryParameterReset\r\n");
						break;
			default:
						ModbusReturnAckInfo(err_Fu);//��485�����쳣����Ϣ,���ܺ���Ч
						break;
		}
	}
	/********************************
	�㲥ָ����ظ�
	���ݸ�ʽ:������ַ(1�ֽ�)+������(0x)+������(4�ֽ�)+У��(2�ֽ�) 
	�ֶ�:0x33,0x55,0x77,0x99
	*********************************/
	else if (ModbusDataPackage.dat[0] == 0x00)//�ǹ㲥ָ��
	{
		switch(ModbusDataPackage.dat[1])
		{
			case AutoSetInitalValue:
						ModbusSetInitalValue(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x11;
						break;
			case ModeSwitch:
						ModbusSwitchMode(RX_Len);		//ģʽ�л�
						break;
			case FactoryCalibration0:
			case FactoryCalibration1:
			case FactoryCalibration2:
			case FactoryCalibration3:
			case FactoryCalibration4:
			case FactoryCalibration5:
						FactoryCalibration(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x07;
						break;//����У׼		
			case FactoryParameterReset:	//�ָ���������
						ModbusFactoryParameterReset(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x13;
						break;
			default:
						break;
		} 
	}
}













