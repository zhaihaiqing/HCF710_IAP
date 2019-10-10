/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "includes.h"



/*******************************************************************************
* Function Name  : ADT7301_ReadTemp
* Description    : ADT7301温度获取，并转换为摄氏度
* Input          : None
* Output         : None
* Return         : 温度值
*******************************************************************************/
float ADT7301_ReadTemp(void)
{
	unsigned int TempVal_temp;
	unsigned char MSByte,LSByte;
	float TempVal,tt;
	
	SPI1_CS_Select(ADT7301); 
	__nop();
	__nop();
	
	MSByte=SPI1_ReadWriteByte(0x00);//读高位
	LSByte=SPI1_ReadWriteByte(0x00);//读低位
	
	TempVal_temp = (MSByte<<8) | LSByte;
	__nop();
	__nop();
	__nop();
	__nop();	
	
	TEMP_CS_H();
	
	TempVal_temp = (MSByte<<8) | LSByte;
	TempVal      = (float)TempVal_temp;
	if((TempVal_temp & 0x2000) == 0x2000) tt = (TempVal-16384)/32;
	else 
		tt = (TempVal/32);
	return tt;
}





unsigned char DS18B20_Rom_Addr[8];

void DS18B20_delayus(unsigned int us)
{	
	unsigned int  i=0;
	for(i=0;i<us;i++)
	{__nop();}
}
//cpu_clk=7.3728MHz
#define ds18b20_t_538us	480
#define ds18b20_t_69us	60
#define ds18b20_t_66us	58
#define ds18b20_t_63us	55
#define ds18b20_t_6us	2
#define ds18b20_t_4us	1
#define ds18b20_t_2us	0


//cpu_clk=7.3728MHz
//#define ds18b20_t_538us	480
//#define ds18b20_t_69us	60
//#define ds18b20_t_66us	58
//#define ds18b20_t_63us	55
//#define ds18b20_t_6us	2
//#define ds18b20_t_4us	1
//#define ds18b20_t_2us	0

//cpu_clk=29.4912MHz
//#define ds18b20_t_538us	2000
//#define ds18b20_t_69us	250
//#define ds18b20_t_66us	240
//#define ds18b20_t_63us	230
//#define ds18b20_t_6us		20
//#define ds18b20_t_4us		11
//#define ds18b20_t_2us		4

unsigned char DS18B20_Rst(void)			//复位DS18B20
{
	unsigned char dat;
	SET_DS18B20_DQ_OUT;
	DQ_OUT_L;//拉低
	DS18B20_delayus(480);//538us
	DQ_OUT_H;
	SET_DS18B20_DQ_IN;
	DS18B20_delayus(55);//60us
	
	dat=DQ_Read();
	
	DS18B20_delayus(480);//524us
	SET_DS18B20_DQ_OUT;
	DQ_OUT_H;
	return dat;	
}

unsigned char DS18B20_Read_Byte(void)
{
	unsigned char i,dat=0;
	
	for(i=0;i<8;i++)
	{
		dat>>=1;
		SET_DS18B20_DQ_OUT;//设置为输出
		
		DQ_OUT_L;					//拉低总线
		__nop();__nop();
		SET_DS18B20_DQ_IN;//设置成输入，由外部上拉电阻将总线拉高，释放总线
		__nop();
		if(DQ_Read())//获取端口值
		dat|=0x80;
		DS18B20_delayus(58);//66us
	}
	SET_DS18B20_DQ_OUT;
	return dat;
}



void DS18B20_Write_Byte(unsigned char dat)
{
	unsigned char i;
	SET_DS18B20_DQ_OUT;
	for(i=0;i<8;i++)
	{
		if(dat&0x01)
		{	
			DQ_OUT_L;//写时间空隙总是从总线的低电平开始
			DS18B20_delayus(2);//15us内拉高
			DQ_OUT_H;
			DS18B20_delayus(58);//69us//整个写1时隙不低于60us
		}
		else
		{
			DQ_OUT_L;
			DS18B20_delayus(60);//保持在60-120us之间
			DQ_OUT_H;
			DS18B20_delayus(2);//
		}
		dat>>=1;
	}
}

void DS18B20_ReadROM(void)
{
	unsigned char i;
	
	DS18B20_Rst();
	
	DS18B20_Write_Byte(0x33);//发送读rom命令
	
	for(i=0;i<8;i++)				//读rom
	{
		DS18B20_Rom_Addr[i]=DS18B20_Read_Byte();
	}

	log1_info("Rom_Addr: ");
	for(i=0;i<8;i++)
	{
		log1_info("0x%x ",DS18B20_Rom_Addr[i]);
	}
	log1_info("\r\n");

}


void DS18B20_SendConV_Command(void)
{
	DS18B20_Rst();

	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0x44);
}

float DS18B20_TEMP(void)
{
	float tt,result;
	unsigned short temp;
	unsigned char a,b;
	
	CPU_SR_ALLOC();
	
//	if(DS18B20_Rst());
//	DS18B20_ReadROM();
	
	OS_CRITICAL_ENTER();
	
	if(DS18B20_Rst())
	{
		return -85;
	}
	
	
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0x44);
	
	if(DS18B20_Rst())
	{
		return -85;
	}
	DS18B20_Write_Byte(0xcc);
	DS18B20_Write_Byte(0xbe);
	a=DS18B20_Read_Byte();
	b=DS18B20_Read_Byte();
	
	OS_CRITICAL_EXIT();
	
	
	
	
	temp=(b<<8) | a ;
	//log1_info("DS18B20_1:0x%x   ",temp);
	
	
	if(temp & 0xf800)
	{
		temp=~temp+1;
		tt=(temp&0x07ff)*0.0625;
		//tt=tt+0.5;//温度数据，+5是四舍五入
		result= 0-tt;
		
		//log1_info("DS18B20_1:%.2f\r\n",result);
	}
	else 
	{
		tt=(temp&0x07ff)*0.0625;
		//tt=tt+0.5;//温度数据，+5是四舍五入
		result = tt;
		//log_info("DS18B20_2:%.2f\r\n",result);
	}

	//log1_info("DS18B20:%.2f\r\n",result);
	return result;
}








