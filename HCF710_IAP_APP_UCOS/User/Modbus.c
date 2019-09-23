
#include "main.h"

volatile unsigned char Modbus_BusyFlag;
volatile unsigned char Modbus_FreeFlag;

unsigned char  FactoryResetWord[4]    = {0x33,0x55,0x77,0x99};//出厂配置指令代号
unsigned char  FactorySetValueWord[4] = {0x44,0x66,0x88,0xaa};//出厂配置指令代号
unsigned char  FactorySetValueWord2[4] = {0x40,0x66,0x88,0xaa};//出厂配置指令代号
unsigned char  SuperModeValueWord2[4]  = {0x40,0x60,0x80,0xa0};//高级模式代号

unsigned char SuperMode_Flag	= 0;

volatile KeepRegister_type				KeepRegister;		//定义保持寄存器
volatile KeepRegister_type				KeepRegisterTemp;	//定义保持寄存器缓存
volatile InputRegister_type	 			InputRegister;		//定义输入寄存器
volatile InputRegister_type	 			InputRegisterTemp;	//定义输入寄存器缓存
volatile ModbusDataPackage_type 		ModbusDataPackage;	//定义modbus接收缓存

/*******************************************************************************
* Function Name  : __ltobf
* Description    : float型数据大小端格式转换
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
* Description    : CRC校验
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
* Description    : modbus返回异常码信息
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ModbusReturnAckInfo(uint8_t err)
{
	uint8_t ErrDat[5];
	uint16_t crc;
	//log_info("err:0x%x\r\n",err);
	if(err)//异常码
	{
		ErrDat[0] = KeepRegister.DeviceAddress;//赋值设备地址
		ErrDat[1] = ModbusDataPackage.dat[1] | 0x80;//赋值异常功能码,即功能码+0x80
		ErrDat[2] = err;//赋值异常码
		crc = CRC16_Check(ErrDat,3);
		ErrDat[3] = (crc & 0xff);//校验低8位
		ErrDat[4] = (crc >> 8);//校验高8位
		
		U485SendData(ErrDat,5);//向485发送数据
		//err=0;
	}
}

/*******************************************************************************
* Function Name  : KeepRegistorDataHton
* Description    : 保持寄存器数据大小端转换
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
* Description    : 输入寄存器数据大小端转换
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
读保持寄存器
数据格式:器件地址(1字节)+功能码(0x03)+起始地址(2字节)+读取寄存器数量(2字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x03)+数据长度(字节数,1字节)+数据(n字节)+校验(2字节)
*********************************/
char ModbusReadKeepRegistor(unsigned char RX_Len)
{
	unsigned char err = 0;
	uint8_t temp[255];
	uint16_t crc;
	
 	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3]; //获取起始地址
 	uint16_t RegVal = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];		//获取要读取的寄存器数量
 	uint16_t StopAddress = StartAddress + RegVal - 1;									//获取结束地址
 	uint16_t KeepRegistorSize = sizeof(KeepRegister) / 2;								//计算保持寄存器总数量
 	uint16_t bytes = RegVal*2;															//计算要读取的字节数
	
 	if(RX_Len != 8)err = err_OE;										//有效操作发生异常
 	if(!RegVal)err = err_add;															//寄存器地址不正确,读取数量必须大于1
 	if( (  (StartAddress == 0x03) || (StartAddress == 0x05) || (StartAddress == 0x09) || (StartAddress == 0x0b)  || (StartAddress == 0x0d) || (StartAddress == 0x0f)  || \
	       (StartAddress == 0x11) || (StartAddress == 0x13) || (StartAddress == 0x15) || (StartAddress == 0x17)  || (StartAddress == 0x19) || (StartAddress == 0x1b)  || (StartAddress == 0x1d) || (StartAddress == 0x1f) || \
		   (StartAddress == 0x21) || (StartAddress == 0x23) || (StartAddress == 0x25) || (StartAddress == 0x27)  || (StartAddress == 0x29) || (StartAddress == 0x2b)  || (StartAddress == 0x2d) || (StartAddress == 0x2f) || \
		   (StartAddress == 0x31) || (StartAddress == 0x33) || (StartAddress == 0x35) || (StartAddress == 0x37)  || (StartAddress == 0x39) || (StartAddress == 0x3b)  || (StartAddress == 0x3d) || (StartAddress == 0x3f) || \
		   (StartAddress == 0x41) || (StartAddress == 0x43) || (StartAddress == 0x45) || (StartAddress == 0x47)  || (StartAddress == 0x49) || (StartAddress == 0x4b)  || (StartAddress == 0x4d) || (StartAddress == 0x4f) || \
		   (StartAddress == 0x51) || (StartAddress == 0x53) || (StartAddress == 0x55)  ) )err = err_add;//寄存器地址不正确,多字节数据要从头读出
 	if( (  (StopAddress == 0x02)  || (StopAddress == 0x04)  || (StopAddress == 0x08)  || (StopAddress == 0x0a)   || (StopAddress == 0x0c)  || (StopAddress == 0x0e)   || \
	       (StopAddress == 0x10)  || (StopAddress == 0x12)  || (StopAddress == 0x14)  || (StopAddress == 0x16)   || (StopAddress == 0x18)  || (StopAddress == 0x1a)   || (StopAddress == 0x1c)  || (StopAddress == 0x1e)  || \
		   (StopAddress == 0x20)  || (StopAddress == 0x22)  || (StopAddress == 0x24)  || (StopAddress == 0x26)   || (StopAddress == 0x28)  || (StopAddress == 0x2a)   || (StopAddress == 0x2c)  || (StopAddress == 0x2e)  || \
		   (StopAddress == 0x30)  || (StopAddress == 0x32)  || (StopAddress == 0x34)  || (StopAddress == 0x36)   || (StopAddress == 0x38)  || (StopAddress == 0x3a)   || (StopAddress == 0x3c)  || (StopAddress == 0x3e)  || \
		   (StopAddress == 0x40)  || (StopAddress == 0x42)  || (StopAddress == 0x44)  || (StopAddress == 0x46)   || (StopAddress == 0x48)  || (StopAddress == 0x4a)   || (StopAddress == 0x4c)  || (StopAddress == 0x4e)  || \
		   (StopAddress == 0x50)  || (StopAddress == 0x52)  || (StopAddress == 0x54)   ) )err = err_add;//寄存器地址不正确,多字节数据没有全部读出
 	if(StopAddress > (KeepRegistorSize - 1))err = err_add;							//寄存器地址不正确
 	if(StartAddress > (KeepRegistorSize - 1))err = err_add;							//寄存器地址不正确
	 
	 
// 	U485TX;Delay_ms(10);
// 	printf("StartAddress:%d\r\n",StartAddress);
// 	printf("StopAddress:%d\r\n",StopAddress);
// 	printf("KeepRegistorSize:%d\r\n",KeepRegistorSize);
// 	printf("err:%d\r\n",err);
// 	printf("ModbusDataPackage.dat[0]:%d\r\n",ModbusDataPackage.dat[0]);
// 	printf("\r\n");
// 	U485RX;Delay_ms(100);
	
	if(  err != 0  )							//返回异常码信息
	{
		ModbusReturnAckInfo(err);														//向485返回异常码信息
		return ERROR;
	}
 	
 	KeepRegistorDataHton();																//大小端数据处理,放在缓存中
 	/*读取保持寄存器数据并向485返回数据*/
  	
	temp[0] = KeepRegister.DeviceAddress;								//赋值设备地址
  	temp[1] = ModbusDataPackage.dat[1];									//赋值功能码
  	temp[2] = bytes;													//赋值数据长度(字节数)
  	memcpy(&temp[3],(uint8_t *)&KeepRegisterTemp+StartAddress*2,bytes);	//字符串copy
  	
  	crc=CRC16_Check(temp,bytes+3);										//执行crc校验
  	temp[bytes+3]=crc & 0xff;
  	temp[bytes+4]=crc>>8;
 	if(ModbusDataPackage.dat[0]) U485SendData(temp,bytes+5);			//发送数据
	
	return SUCCESS;	
}

/********************************
读输入寄存器
数据格式:器件地址(1字节)+功能码(0x04)+起始地址(2字节)+读取寄存器数量(2字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x04)+数据长度(字节数,1字节)+数据(n字节)+校验(2字节)
*********************************/
char ModbusReadInputRegistor(unsigned char RX_Len)
{
	uint8_t err=0;
	uint8_t temp[255];
	uint16_t crc;
	
	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3];	//获取起始地址
	uint16_t RegVal = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];		//获取要读取的寄存器数量
	uint16_t StopAddress = StartAddress + RegVal - 1;									//获取结束地址
	uint16_t InputRegistorSize = sizeof(InputRegister) / 2;								//计算保持寄存器总数量
	uint16_t bytes = RegVal*2;															//计算要读取的字节数
	
	if(RX_Len != 8)err = err_OE;										//有效操作发生异常
	if(!RegVal)err = err_add;											//寄存器地址不正确,读取数量必须大于1
	
	if( ((StartAddress == 0x11) || (StartAddress == 0x14) || (StartAddress == 0x16) || (StartAddress == 0x18) || (StartAddress == 0x1a) || (StartAddress == 0x1c) || \
		 (StartAddress == 0x1e) || (StartAddress == 0x20) || (StartAddress == 0x22) ) )err = err_add;//寄存器地址不正确,多字节数据要从头读出
	if( ((StopAddress == 0x10 ) || (StopAddress == 0x13 ) || (StopAddress == 0x15 ) || (StopAddress == 0x17 ) || (StopAddress == 0x19 ) || (StopAddress == 0x1b ) || \
		 (StopAddress == 0x1d ) || (StopAddress == 0x1f ) || (StopAddress == 0x21 ) ) )err = err_add;//寄存器地址不正确,多字节数据没有全部读出
	
	if(StopAddress > (InputRegistorSize - 1))err = err_add;	//寄存器地址不正确
	if(StartAddress > (InputRegistorSize - 1))err = err_add;	//寄存器地址不正确
	if(  err != 0 )						//返回异常码信息
	{
		ModbusReturnAckInfo(err);								//向485返回异常码信息
		return ERROR;
	}
	
	
	InputRegistorDataHton();									//大小端数据处理,放在缓存中
	//读取输入寄存器数据并向485返回数据
	
	temp[0] = KeepRegister.DeviceAddress;						//赋值设备地址
	temp[1] = ModbusDataPackage.dat[1];							//赋值功能码
	temp[2] = bytes;											//赋值数据长度(字节数)
	memcpy(&temp[3],(uint8_t *)&InputRegisterTemp+StartAddress*2,bytes);
	
	crc=CRC16_Check(temp,bytes+3);
	temp[bytes+3]=crc & 0xff;
	temp[bytes+4]=crc>>8;
	if(ModbusDataPackage.dat[0]) U485SendData(temp,bytes+5);
	
	return SUCCESS;	
}

/********************************
写单个寄存器,保持寄存器
数据格式:器件地址(1字节)+功能码(0x06)+寄存器地址(2字节)+寄存器数值(2字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x06)+寄存器地址(2字节)+寄存器数值(2字节)+校验(2字节)
*********************************/
char ModbusWriteSingleRegistor(unsigned char RX_Len)
{
	uint8_t err=0;
	uint8_t temp[10];
	uint8_t flag=0;
	uint16_t crc,dat;
	//获取相关参数
	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3];	//获取起始地址
	dat = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];					//获取要写入的数据
	
	//参数合法检查
	if(RX_Len != 8)err = err_OE;	//有效操作发生异常
	if((StartAddress != 0) && (StartAddress != 1) && (StartAddress != 6) && (StartAddress != 7) && (StartAddress != 0x56) && (StartAddress != 0x57))err = err_add;			//异常码,寄存器开始地址不正确,多字节数据不可用此功能码
	//数据有效范围判断并写入
	
	
	
	//if(StopAddress > 0x09)flag=1;		//判定操作危险寄存器标志位
	if(StartAddress > 0x09)flag=1;		//判定操作危险寄存器标志位
	if( (SuperMode_Flag==0) && (flag==1) ){ModbusReturnAckInfo(err_mode);return ERROR;}		//如果是普通模式，同时操作到危险寄存器，则返回错误
	
	
	switch(StartAddress)
	{
		case 0:
			if((dat == 0) || (dat > 247)){ModbusReturnAckInfo(3);	return ERROR;}	//地址数据超出范围,返回异常功能码,寄存器值超出范围
			KeepRegister.DeviceAddress = dat;
			EEWrite(KREEPROM_BASEADDR,(void *)&dat,2);//保存数据
			break;
		case 1:
			KeepRegister.DeviceGroupNum = dat;
			EEWrite(KREEPROM_BASEADDR+2,(void *)&dat,2);//保存数据
			break;
		case 6:
			if( ((dat == 0x14) || (dat == 0x64)) ==0 ){ModbusReturnAckInfo(3);	return ERROR;}
			KeepRegister.Sensor_Range = dat;
			EEWrite(KREEPROM_BASEADDR+12,(void *)&dat,2);//保存数据
			break;
		case 7:
			if( ( ((dat >>8) == 0x01) || ((dat >>8) == 0x02) || ((dat >>8) ==0x03) || ((dat >>8) ==0x04) || ((dat >>8)==0x05) ) ==0  ){ModbusReturnAckInfo(3);	return ERROR;}   //如果不等于特定值，返回错误
			//if( ( ((dat & 0x00FF)==0x00) || ((dat & 0x00FF)==0x01))  ==0  ){ModbusReturnAckInfo(3);	return ERROR;}
			KeepRegister.Liquid_Sec = dat;
			EEWrite(KREEPROM_BASEADDR+14,(void *)&dat,2);//保存数据
			break;
		case 0x56:
			if(  (dat < 0x00) || (dat >256)  ){ModbusReturnAckInfo(3);	return ERROR;}   //如果不等于特定值，返回错误
			KeepRegister.Average_num = dat;
			EEWrite(KREEPROM_BASEADDR+172,(void *)&dat,2);//保存数据
			break;
		case 0x57:
			if(  (dat < 0x01) || (dat >0x07)  ){ModbusReturnAckInfo(3);	return ERROR;}   //如果不等于特定值，返回错误
			KeepRegister.Average_num = dat;
			EEWrite(KREEPROM_BASEADDR+174,(void *)&dat,2);//保存数据
			break;
		default:
			ModbusReturnAckInfo(3);	
			return ERROR;
			break;	
	}			
	
//	if(StartAddress == 0)							//如果写设备地址数据
//	{
//		if((dat == 0) || (dat > 247)){ModbusReturnAckInfo(3);	return ERROR;}	//地址数据超出范围,返回异常功能码,寄存器值超出范围
//		KeepRegister.DeviceAddress = dat;
//		EEWrite(KREEPROM_BASEADDR,(void *)&dat,2);//保存数据
//	}
//	
//	if(StartAddress == 1)							//如果写设备组号
//	{	
//		KeepRegister.DeviceGroupNum = dat;
//		EEWrite(KREEPROM_BASEADDR+2,(void *)&dat,2);//保存数据
//	}
//	
//	if(StartAddress == 6)							//如果写传感器量程
//	{
//		if( ((dat == 0x14) || (dat == 0x64)) ==0 ){ModbusReturnAckInfo(3);	return ERROR;}
//		KeepRegister.Sensor_Range = dat;
//		EEWrite(KREEPROM_BASEADDR+12,(void *)&dat,2);//保存数据
//	}
//	
//	if(StartAddress == 7)							//如果
//	{
//		if( ( ((dat >>8) == 0x01) || ((dat >>8) == 0x02) || ((dat >>8) ==0x03) || ((dat >>8) ==0x04) || ((dat >>8)==0x05) ) ==0  ){ModbusReturnAckInfo(3);	return ERROR;}   //如果不等于特定值，返回错误
//		//if( ( ((dat & 0x00FF)==0x00) || ((dat & 0x00FF)==0x01))  ==0  ){ModbusReturnAckInfo(4);	return ERROR;}
//		KeepRegister.Liquid_Sec = dat;
//		EEWrite(KREEPROM_BASEADDR+14,(void *)&dat,2);//保存数据
//	}
//	
//	if(StartAddress == 0x56)							//平均次数
//	{
//		if(  (dat < 0x00) || (dat >256)  ){ModbusReturnAckInfo(3);	return ERROR;}   //如果不等于特定值，返回错误
//		KeepRegister.Average_num = dat;
//		EEWrite(KREEPROM_BASEADDR+172,(void *)&dat,2);//保存数据
//	}
//	if(StartAddress == 0x57)							//平均次数
//	{
//		if(  (dat < 0x01) || (dat >0x07)  ){ModbusReturnAckInfo(3);	return ERROR;}   //如果不等于特定值，返回错误
//		KeepRegister.Average_num = dat;
//		EEWrite(KREEPROM_BASEADDR+174,(void *)&dat,2);//保存数据
//	}
	
	if(  err != 0 )			//返回异常码信息
	{
		ModbusReturnAckInfo(err);					//向485返回异常码信息
		return ERROR;
	}
	//指令返回
	
	temp[0] = KeepRegister.DeviceAddress;			//赋值设备地址
	temp[1] = ModbusDataPackage.dat[1];				//赋值功能码
	memcpy(&temp[2],(uint8_t *)&ModbusDataPackage.dat[2],4);
	//校验
	crc = CRC16_Check(temp,6);						//crc校验
	temp[6] = crc & 0xff;							//crc低位在前
	temp[7] = crc >> 8;								//高位在后
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);//发送数据
	
	return SUCCESS;
}
/********************************
写多个寄存器,保持寄存器
数据格式:器件地址(1字节)+功能码(0x10)+寄存器地址(2字节)+寄存器数量(2字节)+字节数(1字节)+寄存器数值(N个数据)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x10)+寄存器地址(2字节)+寄存器数量(2字节)+校验(2字节)
*********************************/
char ModbusWriteSomeRegistor(void)
{
	uint8_t err=0;
	uint8_t temp[10];
	//uint8_t t[180];
	uint8_t flag=0;
	uint16_t crc;
	//获取相关参数
	uint16_t StartAddress = (ModbusDataPackage.dat[2] << 8) | ModbusDataPackage.dat[3];	//获取起始地址
	uint16_t RegVal = (ModbusDataPackage.dat[4] << 8) | ModbusDataPackage.dat[5];		//获取要写入的寄存器数量
	uint8_t bytes = ModbusDataPackage.dat[6];											//获取字节数
	uint16_t StopAddress = StartAddress + RegVal - 1;									//获取结束地址
	uint16_t KeepRegistorSize = sizeof(KeepRegister) / 2;								//计算保持寄存器总数量
	
	//U485TX;Delay_ms(10);
	
	//printf("修正值:mm  修正后高度值:mm\r\n");
	//Delay_ms(100);U485RX;Delay_ms(10);
	
	//参数合法检查
	if(!RegVal || !bytes)err = err_add;													//寄存器地址不正确,读取数量必须大于1
	if( (  (StartAddress == 0x03) || (StartAddress == 0x05) || (StartAddress == 0x09) || (StartAddress == 0x0b)  || (StartAddress == 0x0d) || (StartAddress == 0x0f)  || \
		   (StartAddress == 0x11) || (StartAddress == 0x13) || (StartAddress == 0x15) || (StartAddress == 0x17)  || (StartAddress == 0x19) || (StartAddress == 0x1b)  || (StartAddress == 0x1d) || (StartAddress == 0x1f) || \
	       (StartAddress == 0x21) || (StartAddress == 0x23) || (StartAddress == 0x25) || (StartAddress == 0x27)  || (StartAddress == 0x29) || (StartAddress == 0x2b)  || (StartAddress == 0x2d) || (StartAddress == 0x2f) || \
	       (StartAddress == 0x31) || (StartAddress == 0x33) || (StartAddress == 0x35) || (StartAddress == 0x37)  || (StartAddress == 0x39) || (StartAddress == 0x3b)  || (StartAddress == 0x3d) || (StartAddress == 0x3f) || \
	       (StartAddress == 0x41) || (StartAddress == 0x43) || (StartAddress == 0x45) || (StartAddress == 0x47)  || (StartAddress == 0x49) || (StartAddress == 0x4b)  || (StartAddress == 0x4d) || (StartAddress == 0x4f) || \
	       (StartAddress == 0x51) || (StartAddress == 0x53) || (StartAddress == 0x55)  ) )err = err_add;//寄存器地址不正确,多字节数据要从头读出
	if( (  (StopAddress == 0x02)  || (StopAddress == 0x04)  || (StopAddress == 0x08)  || (StopAddress == 0x0a)   || (StopAddress == 0x0c)  || (StopAddress == 0x0e)   || \
	       (StopAddress == 0x10)  || (StopAddress == 0x12)  || (StopAddress == 0x14)  || (StopAddress == 0x16)   || (StopAddress == 0x18)  || (StopAddress == 0x1a)   || (StopAddress == 0x1c)  || (StopAddress == 0x1e)  || \
	       (StopAddress == 0x20)  || (StopAddress == 0x22)  || (StopAddress == 0x24)  || (StopAddress == 0x26)   || (StopAddress == 0x28)  || (StopAddress == 0x2a)   || (StopAddress == 0x2c)  || (StopAddress == 0x2e)  || \
	       (StopAddress == 0x30)  || (StopAddress == 0x32)  || (StopAddress == 0x34)  || (StopAddress == 0x36)   || (StopAddress == 0x38)  || (StopAddress == 0x3a)   || (StopAddress == 0x3c)  || (StopAddress == 0x3e)  || \
	       (StopAddress == 0x40)  || (StopAddress == 0x42)  || (StopAddress == 0x44)  || (StopAddress == 0x46)   || (StopAddress == 0x48)  || (StopAddress == 0x4a)   || (StopAddress == 0x4c)  || (StopAddress == 0x4e)  || \
	       (StopAddress == 0x50)  || (StopAddress == 0x52)  || (StopAddress == 0x54)   ) )err = err_add;//寄存器地址不正确,多字节数据没有全部读出
	if(StopAddress > (KeepRegistorSize - 1))err = err_add;		//寄存器地址不正确
	if(StartAddress > (KeepRegistorSize - 1))err = err_add;		//寄存器地址不正确
	
	if(StopAddress > 0x09)flag=1;		//判定操作危险寄存器标志位
	if(StartAddress > 0x09)flag=1;		//判定操作危险寄存器标志位
	
	
	if( (SuperMode_Flag==0) && (flag==1) )err=err_mode;		//如果是普通模式，同时操作到危险寄存器，则返回错误
	
	if(  err != 0 )							//返回异常码信息
	{
		ModbusReturnAckInfo(err);									//向485返回异常码信息
		return ERROR;
	}
	
	memcpy((uint8_t *)&KeepRegisterTemp,(uint8_t *)&KeepRegister,sizeof(KeepRegister));					//先将当前寄存器数据拷贝到缓存中
	memcpy((uint8_t *)&KeepRegisterTemp + StartAddress*2,(uint8_t *)&ModbusDataPackage.dat[7],bytes);	//然后将接收的数据拷贝到寄存器缓存中
	err = 0;
	//大小端转换
	if(StartAddress == 0x00)KeepRegisterTemp.DeviceAddress = htons(KeepRegisterTemp.DeviceAddress);														//将设备地址数据大小端转换
	if((StartAddress <= 0x01) && (StopAddress >= 0x01))KeepRegisterTemp.DeviceGroupNum = htons(KeepRegisterTemp.DeviceGroupNum );										//将保留数据1数据大小端转换
	if((StartAddress <= 0x02) && (StopAddress >= 0x03))KeepRegisterTemp.OriginaLiquidAltitude_After = __ltobf(KeepRegisterTemp.OriginaLiquidAltitude_After);				//将液位初始高度数据大小端转换
	if((StartAddress <= 0x04) && (StopAddress >= 0x05))KeepRegisterTemp.OriginaLiquidAltitude_Befor = __ltobf(KeepRegisterTemp.OriginaLiquidAltitude_Befor );							//将液体密度数据大小端转换
	if((StartAddress <= 0x06) && (StopAddress >= 0x06))KeepRegisterTemp.Sensor_Range = htons(KeepRegisterTemp.Sensor_Range );									//将保留数据2数据大小端转换
	if((StartAddress <= 0x07) && (StopAddress >= 0x07))KeepRegisterTemp.Liquid_Sec = htons(KeepRegisterTemp.Liquid_Sec );											//将保留数据3数据大小端转换
	if((StartAddress <= 0x08) && (StopAddress >= 0x09))KeepRegisterTemp.LocalAccelerationOfGravity = __ltobf(KeepRegisterTemp.LocalAccelerationOfGravity );	//将当地重力加速度数据大小端转换
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
	
	//判断数据有效性
	if((KeepRegisterTemp.DeviceAddress == 0) || (KeepRegisterTemp.DeviceAddress > 247))	err = err_Re_VOR;	//地址数据超出范围,返回异常功能码,寄存器值超出范围
	if((KeepRegisterTemp.Average_num < 0) || (KeepRegisterTemp.Average_num > 256))		err = err_Re_VOR;
	if((KeepRegisterTemp.bps< 0x01) || (KeepRegisterTemp.bps > 0x07))					err = err_Re_VOR;
	if((KeepRegisterTemp.LocalAccelerationOfGravity< 9.78) || (KeepRegisterTemp.LocalAccelerationOfGravity > 10.0))err = err_Re_VOR;
	if(!err)																						//如果无错误,则将缓存的数据拷贝到寄存器中
	{
		memcpy((uint8_t *)&KeepRegister,(uint8_t *)&KeepRegisterTemp,sizeof(KeepRegister));
		EEWrite(KREEPROM_BASEADDR,(void *)&KeepRegisterTemp,sizeof(KeepRegister));					//保存数据，更新整个寄存器组
		
//		EEWrite(KREEPROM_BASEADDR,(void *)&KeepRegister.DeviceAddress,24);//更新EEPROM
//		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
//		EEWrite(KREEPROM_BASEADDR+24,(void *)&KeepRegister.MV[0],28);
//		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
//	
//		EEWrite(KREEPROM_BASEADDR+52,(void *)&KeepRegister.LTC0[0],120);
//		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
//		
//		EEWrite(KREEPROM_BASEADDR+172,(void *)&KeepRegister.Average_num,4);//保存数据
//		GPIO_PinReverse(GPIOA,GPIO_Pin_1);

	}
	else if(ModbusDataPackage.dat[0])//如果数据范围错误
	{
		ModbusReturnAckInfo(err);	 //向485返回异常码信息
		return ERROR;
	}
	
	//指令返回
	temp[0] = KeepRegister.DeviceAddress;					//赋值设备地址
	temp[1] = ModbusDataPackage.dat[1];						//赋值功能码
	memcpy(&temp[2],(uint8_t *)&ModbusDataPackage.dat[2],4);//赋值寄存器地址和数量,共4字节
	//校验
	crc = CRC16_Check(temp,6);								//crc校验
	temp[6] = crc & 0xff;									//crc低位在前
	temp[7] = crc >> 8;										//高位在后
	if(ModbusDataPackage.dat[0]) U485SendData(temp,8);		//发送数据
	
	return SUCCESS;
}



/********************************
设定初值，完成后写保持寄存器
数据格式:器件地址(1字节)+功能码(0x41)+操作码(4字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x41)+所修改寄存器首地址(2字节)+所修改寄存器数量(2字节)+校验(2字节)
操作码:0x44,0x66,0x88,0xaa
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
	KeepRegister.OriginaLiquidAltitude_After = InputRegister.LiquidAltitudeAbsoluteValue_After;    //同时将数据保存在EEPROM
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
满量程校准，校准完成后写保持寄存器
数据格式:器件地址(1字节)+功能码(0x43)+操作码(4字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x43)+所修改寄存器首地址(2字节)+所修改寄存器数量(2字节)+校验(2字节)
功能码：六个点依次为：0x43、0x44、0x45、0x46、0x47、0x48
操作码:0x44,0x66,0x88,0xaa:用户校准
操作码:0x40,0x66,0x88,0xaa:出厂校准
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
	if(SuperMode_Flag==0)err=err_mode;//如果是普通模式，则返回错误
	if(  err != 0 )
	{
		ModbusReturnAckInfo(err);
		return ERROR;
	}
	
	function_code=ModbusDataPackage.dat[1]; //获取功能码
	
	ADCValue = InputRegister.ADCOriginalValue;//取n次平均，mV
	
	switch (function_code)   //根据功能码对相应的校准点进行校准
	{
		case FactoryCalibration0: KeepRegister.MV[0]=ADCValue;
								  EEWrite(KREEPROM_BASEADDR+24,(void *)&KeepRegister.MV[0],4);
								  KeepRegister.Temp_T0=InputRegister.Temperature;
								  EEWrite(KREEPROM_BASEADDR+48,(void *)&KeepRegister.Temp_T0,4);
								  log_info("FactoryCalibration0 Over!\r\n");
								  break;//五阶校准点0，同时将数据保存在EEPROM
		case FactoryCalibration1: KeepRegister.MV[1]=ADCValue; EEWrite(KREEPROM_BASEADDR+28,(void *)&KeepRegister.MV[1],4);log_info("FactoryCalibration1 Over!\r\n");break;//五阶校准点1，同时将数据保存在EEPROM
		case FactoryCalibration2: KeepRegister.MV[2]=ADCValue; EEWrite(KREEPROM_BASEADDR+32,(void *)&KeepRegister.MV[2],4);log_info("FactoryCalibration2 Over!\r\n");break;//五阶校准点2，同时将数据保存在EEPROM
		case FactoryCalibration3: KeepRegister.MV[3]=ADCValue; EEWrite(KREEPROM_BASEADDR+36,(void *)&KeepRegister.MV[3],4);log_info("FactoryCalibration3 Over!\r\n");break;//五阶校准点3，同时将数据保存在EEPROM
		case FactoryCalibration4: KeepRegister.MV[4]=ADCValue; EEWrite(KREEPROM_BASEADDR+40,(void *)&KeepRegister.MV[4],4);log_info("FactoryCalibration4 Over!\r\n");break;//五阶校准点4，同时将数据保存在EEPROM
		case FactoryCalibration5: KeepRegister.MV[5]=ADCValue; EEWrite(KREEPROM_BASEADDR+44,(void *)&KeepRegister.MV[5],4);log_info("FactoryCalibration5 Over!\r\n");break;//五阶校准点5，同时将数据保存在EEPROM
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
获取SN信息
数据格式:器件地址(1字节)+功能码(0x40)+操作码(4字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x40)+数据数量(1字节)+SN信息(15字节)+校验(2字节)
操作码:0x33,0x55,0x77,0x99
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
//	if(  (err != 0)  && (ModbusDataPackage.dat[0] != 0) )//加判断
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
	
	EERead_Z(EEPROM_SHARE_DATA_ADDR,SN_Info,16);//读取EEPROM中共享字段中的SN信息
	
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
模式切换
数据格式:器件地址(1字节)+功能码(0x66)+操作码(4字节)+校验(2字节)

操作码:0x44,0x66,0x88,0xaa切换为普通模式    返回格式:器件地址(1字节)+功能码(0x66)+0x44+0x66+0x88+0xaa+校验(2字节)
操作码:0x40,0x60,0x80,0xa0切换为超级模式    返回格式:器件地址(1字节)+功能码(0x66)+0x40+0x60+0x80+0xa0+校验(2字节)
*********************************/
char ModbusSwitchMode(unsigned char RX_Len)
{
	uint8_t err=0;
	uint8_t temp[10];
	uint16_t crc;
	uint8_t temp2=3;
	
	if(RX_Len !=8 )err = err_OE;
	if( strncmp(SuperModeValueWord2,(unsigned char *)&ModbusDataPackage.dat[2],4) ==0 )//切换为超级模式
	{
		temp2=1;		
	}
	else if( strncmp(FactorySetValueWord,(unsigned char *)&ModbusDataPackage.dat[2],4) ==0 )//切换为普通模式
	{
		temp2=2;
	}
	else err=err_OE;
	
	if(  (err != 0)  &&  (ModbusDataPackage.dat[0] != 0) )
	{
		ModbusReturnAckInfo(err);
		return ERROR;
	}
	
		
	if( temp2==1 )//切换为超级模式
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
	
	else if( temp2==2 )//切换为普通模式
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
清除温度修正系数，将温度修正系数全部清零
数据格式:器件地址(1字节)+功能码(0x49)+操作码(4字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x49)+所修改寄存器首地址(2字节)+所修改寄存器数量(2字节)+校验(2字节)
操作码:0x44,0x66,0x88,0xaa
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
	if(  (err != 0)  && (ModbusDataPackage.dat[0] != 0) )//加判断
	{
		ModbusReturnAckInfo(err);
		return ERROR;
	}
	
	//将系数全部清零
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
恢复出厂设置
数据格式:器件地址(0)+功能码(0x68)+操作码(4字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x68)+寄存器地址(2字节)+寄存器数量(2字节)+字节数(1字节)+所有保持寄存器数据(N字节)+校验(2字节)
操作码:0x33,0x55,0x77,0x99
*********************************/
char ModbusFactoryParameterReset(unsigned char RX_Len)
{
	uint8_t err=0;
	uint8_t temp[255];
	uint16_t crc;
	if(RX_Len != 8)err = err_OE;//有效操作发生异常
	if(strncmp(FactoryResetWord,(char *)&ModbusDataPackage.dat[2],4) != 0)err = err_OE;//有效操作发生异常
	if(SuperMode_Flag==0)err=err_mode;
	if(  (err != 0)  && (ModbusDataPackage.dat[0] != 0) )	  //返回异常码信息
	{
		ModbusReturnAckInfo(err);			  //向485返回异常码信息
		return ERROR;
	}
	
	//KeepRegister_Num=0;
	//KeepRegister_Byte_Num=0;
	
	
	
	
//	EEErase(KREEPROM_BASEADDR,12);           //擦除EEPROM前12个字节，包括地址、组号、初始高度、密度、重力加速度
//	EEErase(KREEPROM_BASEADDR+16,4);         //擦除重力加速度。
//	
//	EEErase(FPOWERONFLAG_BASEADDR,2);        //擦除首次上电标志位
	
	KeepRegistorDataHton();				 //大小端数据处理,放在缓存中
										 
	temp[0] = KeepRegister.DeviceAddress;//赋值设备地址
	temp[1] = ModbusDataPackage.dat[1];	 //赋值功能码
	temp[2] = 0x00;
	temp[3] = 0x00;
	temp[4] = 0x00;
	temp[5] = KeepRegister_Num;          
	temp[6] = KeepRegister_Byte_Num;
	
	memcpy(&temp[7],(uint8_t *)&KeepRegisterTemp,KeepRegister_Byte_Num);//获取数据
	
	//校验
	crc = CRC16_Check(temp,KeepRegister_Byte_Num+7); //crc校验
	temp[KeepRegister_Byte_Num+7] = crc & 0xff;		//crc低位在前
	temp[KeepRegister_Byte_Num+8] = crc >> 8;		//高位在后
	
	
	KeepRegister.DeviceAddress              = DefaultDeviceADDR;
	KeepRegister.OriginaLiquidAltitude_After      = 0;
	KeepRegister.OriginaLiquidAltitude_Befor      = 0;
	KeepRegister.Liquid_Sec                = (Liquid_FD01_25<<8) | 0x00;
	KeepRegister.LocalAccelerationOfGravity = DefaultAccelerationOfGravity;
	KeepRegister.DeviceGroupNum = 0;
	KeepRegister.Sensor_Range = DefaultSensor_Range;
	KeepRegister.Average_num	=	3;
	KeepRegister.bps			=   3;
	
	EEWrite(KREEPROM_BASEADDR,(void *)&KeepRegister.DeviceAddress,20);//更新EEPROM
	EEWrite(KREEPROM_BASEADDR+172,(void *)&KeepRegister.Average_num,2);//保存数据
	
	
	if(ModbusDataPackage.dat[0]) U485SendData(temp,KeepRegister_Byte_Num+9);//发送数据,长度=读取字节数+前面三个字节+两个校验；如果是广播命令则不回复。
	UART1_Configuration(KeepRegister.bps);
	//SystemResetSoft();
	//while(1);//等待看门狗复位操作，完成恢复出厂设置
	
	return SUCCESS;
}


/********************************
设定初值，完成后写保持寄存器
数据格式:器件地址(1字节)+功能码(0x41)+操作码(4字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x41)+所修改寄存器首地址(2字节)+所修改寄存器数量(2字节)+校验(2字节)
操作码:0x33,0x55,0x77,0x99
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
	if(ModbusDataPackage.dat[0] == KeepRegister.DeviceAddress)//是本机指令
	{
		switch(ModbusDataPackage.dat[1])
		{
			case ReadKeepRegistor:			//读保持寄存器
						ModbusReadKeepRegistor(RX_Len);
						//log_info("ModbusReadKeepRegistor\r\n");
						//InputRegister.SystemWorkStatus=0x01;
						break;
			case ReadInputRegistor:			//读输入寄存器
						ModbusReadInputRegistor(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x02;
						//log1_info("ReadInputRegistor\r\n");
						break;
			case WriteSingleRegistor:		//写单个保持寄存器
						ModbusWriteSingleRegistor(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x03;
						//log_info("WriteSingleRegistor\r\n");
						break;
			case SoftWare_Reset:
						ResetSoftware(RX_Len);
						break;
			case WriteSomeRegistor:			//写多个保持寄存器
						ModbusWriteSomeRegistor();
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x04;
						//log_info("WriteSomeRegistor\r\n");
						break;
			case AutoSetInitalValue:		//自动设置初值
						ModbusSetInitalValue(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x05;
						//log_info("AutoSetInitalValue\r\n");
						break;
			case ModeSwitch:
						ModbusSwitchMode(RX_Len);		//模式切换
						break;
			case FactoryCalibration0:
			case FactoryCalibration1:
			case FactoryCalibration2:
			case FactoryCalibration3:
			case FactoryCalibration4:
			case FactoryCalibration5:		
						FactoryCalibration(RX_Len);			
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x07;
						break;//出厂校准
			case Clear_temp_corr:                //清除校准系数
						CTC(RX_Len);
						//log_info("Clear_temp_corr\r\n");
						break;
			case FactoryParameterReset:		//恢复出厂设置
						ModbusFactoryParameterReset(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x09;
						//log_info("FactoryParameterReset\r\n");
						break;
			default:
						ModbusReturnAckInfo(err_Fu);//向485返回异常码信息,功能号无效
						break;
		}
	}
	/********************************
	广播指令，不回复
	数据格式:器件地址(1字节)+功能码(0x)+操作码(4字节)+校验(2字节) 
	字段:0x33,0x55,0x77,0x99
	*********************************/
	else if (ModbusDataPackage.dat[0] == 0x00)//是广播指令
	{
		switch(ModbusDataPackage.dat[1])
		{
			case AutoSetInitalValue:
						ModbusSetInitalValue(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x11;
						break;
			case ModeSwitch:
						ModbusSwitchMode(RX_Len);		//模式切换
						break;
			case FactoryCalibration0:
			case FactoryCalibration1:
			case FactoryCalibration2:
			case FactoryCalibration3:
			case FactoryCalibration4:
			case FactoryCalibration5:
						FactoryCalibration(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x07;
						break;//出厂校准		
			case FactoryParameterReset:	//恢复出厂设置
						ModbusFactoryParameterReset(RX_Len);
						InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x13;
						break;
			default:
						break;
		} 
	}
}













