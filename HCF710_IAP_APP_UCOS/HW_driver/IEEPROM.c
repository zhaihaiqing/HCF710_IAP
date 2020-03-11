/* Includes ------------------------------------------------------------------*/
#include "main.h"

#define IEEPROM_TimeOut		0xfffff


void Write_EEPROM(unsigned int addr,unsigned char data)
{
	unsigned int address=0;
	address=EEPROM_START_ADDR+addr;
	
	if(IS_FLASH_DATA_ADDRESS(address))
	{
		DATA_EEPROM_Unlock();
		while(FLASH_GetStatus()!=FLASH_COMPLETE);
		DATA_EEPROM_ProgramByte(address,data);
		DATA_EEPROM_Lock();
	}
}


unsigned char Read_EEPROM(unsigned int addr)
{
	unsigned int address=0;
	unsigned char tmp=0;
	address=EEPROM_START_ADDR+addr;
	
	
	if(IS_FLASH_DATA_ADDRESS(address))
	{
		DATA_EEPROM_Unlock();
		while(FLASH_GetStatus()==FLASH_BUSY);
		tmp=*(__IO unsigned char *)address;
		DATA_EEPROM_Lock();
	}
	return tmp;
}


void EERead(unsigned short addr, unsigned char *pbuff,unsigned short length)
{
	unsigned int timeout=0;
	unsigned char *waddr=NULL;
	waddr=(unsigned char *)(EEPROM_START_ADDR+addr);
	
	//DATA_EEPROM_Unlock();
	timeout = IEEPROM_TimeOut;
	while( (FLASH_GetStatus()==FLASH_BUSY) && (timeout--));
	while(length--)
	{
		*pbuff++=*waddr++;
	}
	//DATA_EEPROM_Lock();
}

void EERead_Z(unsigned short addr, unsigned char *pbuff,unsigned short length)
{
	unsigned char *waddr=NULL;
	waddr=(unsigned char *)(0x08080000+addr);
	
	DATA_EEPROM_Unlock();
	while(FLASH_GetStatus()==FLASH_BUSY);
	while(length--)
	{
		*pbuff++=*waddr++;
	}
	DATA_EEPROM_Lock();
}


void EEWrite(unsigned short addr, unsigned char *pbuff,unsigned short length)
{
	//unsigned int i;
	unsigned int waddr=0;
	CPU_SR_ALLOC();
	
	waddr=EEPROM_START_ADDR+addr;
	
	DATA_EEPROM_Unlock();
	while( (FLASH_GetStatus()!=FLASH_COMPLETE));
	while(length--)
	{
		OS_CRITICAL_ENTER();
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		DATA_EEPROM_ProgramByte(waddr++,*pbuff++); 
		OS_CRITICAL_EXIT();
	}
    DATA_EEPROM_Lock();	
}

//void EEWrite(unsigned short addr, unsigned char *pbuff,unsigned short length)
//{
//	unsigned char *waddr=NULL;
//	waddr=(unsigned char *)(EEPROM_START_ADDR+addr);
//	
//	//if(length>128)length=128;
//	
//	DATA_EEPROM_Unlock();
//	while(FLASH_GetStatus()!=FLASH_COMPLETE);
//	while(length--)
//	{
//		*waddr++=*pbuff++;
//		while(FLASH_GetStatus()==FLASH_BUSY);
//	}
//	DATA_EEPROM_Lock();
//}

/*******************************************************************************
* Function Name  : EEErase
* Description    : EEPROM²Á³ýº¯Êý£¬Ð´0
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EEErase(unsigned short addr, unsigned short length)
{
	unsigned char Data_temp[512]={0x00};
	if(length>512)length=512;
	EEWrite(addr,(void *)&Data_temp,length);
}




















