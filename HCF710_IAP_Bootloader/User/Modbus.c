
#include "main.h"

volatile ModbusDataPackage_type 	ModbusDataPackage;	//����modbus���ջ���

/*******************************************************************************
* Function Name  : __ltobf
* Description    : float�����ݴ�С�˸�ʽת��
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

/***********************************************************************
��ȡSN��Ϣ
���ݸ�ʽ:������ַ(1�ֽ�)+������(0x40)+������(4�ֽ�)+У��(2�ֽ�)
���ظ�ʽ:������ַ(1�ֽ�)+������(0x40)+��������(1�ֽ�)+SN��Ϣ(15�ֽ�)+У��(2�ֽ�)
������:0x33,0x55,0x77,0x99
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
	
	EERead(EEPROM_SHARE_DATA_ADDR,SN_EEPROM,16);//��ȡEEPROM�й����ֶ��е�SN��Ϣ
	
	//ѭ���ж�ROM�е�SN��Program�е�SN�Ƿ���ȣ��������������EEPROMROM��д��SN��Ϣ
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














