
#include "main.h"

volatile ModbusDataPackage_type 	ModbusDataPackage;	//定义modbus接收缓存

/*******************************************************************************
* Function Name  : __ltobf
* Description    : float型数据大小端格式转换
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//static float __ltobf(float data)
//{
//	FLOAT_CONV d1, d2;

//	d1.f = data;

//	d2.c[0] = d1.c[3];
//	d2.c[1] = d1.c[2];
//	d2.c[2] = d1.c[1];
//	d2.c[3] = d1.c[0];
//	return d2.f;
//}
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

/***********************************************************************
获取SN信息
数据格式:器件地址(1字节)+功能码(0x40)+操作码(4字节)+校验(2字节)
返回格式:器件地址(1字节)+功能码(0x40)+数据数量(1字节)+SN信息(15字节)+校验(2字节)
操作码:0x33,0x55,0x77,0x99
************************************************************************/
const char __progmem_smartbow_start[32] = "**TESTHCF7101002**5";
char Get_SNInfo_Fun(void)
{
	unsigned char i;
	unsigned char temp[19]={0};
	unsigned char SN_Program[16]={0};
	unsigned char SN_EEPROM[16]={0};
	unsigned char SN_ERR_FLAG=0;
	
	for(i=0; i<19; i++)
	{
		temp[i] = __progmem_smartbow_start[i];
	}
	
	for(i=0; i<14; i++)
	{
		SN_Program[i] = temp[i+2];
	}
	SN_Program[14]=temp[18];
	SN_Program[15]=0;
	
	for(i=0;i<8;i++)
	{
		BL_Data.SN[i]=SN_Program[i*2]<<8 | SN_Program[i*2+1];
	}
	
	EERead(EEPROM_SHARE_DATA_ADDR,SN_EEPROM,16);//读取EEPROM中共享字段中的SN信息
	
	//循环判断ROM中的SN和Program中的SN是否相等，不相等则重新向EEPROMROM中写入SN信息
	for(i=0;i<16;i++)
	{
		if(SN_EEPROM[i] != SN_Program[i]) SN_ERR_FLAG++;
	}
	if(SN_ERR_FLAG)
	{
		EEWrite(EEPROM_SHARE_DATA_ADDR,SN_Program,16);
	}
	log_info("SN Init OK!SN_ERR_FLAG:%d\r\n",SN_ERR_FLAG);
	
	return SUCCESS;
}














