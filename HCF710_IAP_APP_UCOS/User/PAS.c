/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*******************************************************************************
* Function Name  : ADS124x_Init1
* Description    : ADS124x初始化函数，配置测量过程中不可更改的寄存器
									 ADS124x初始化函数，配置桥类型、激励源、采样速度
									 ADS124x在开启增益时，数据的噪声度有所降低
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void ADS124x_Set_Par()
//{                  
//	ADS124x_WriteCommand(ADS_SDATAC);//发送停止转换指令 ADS_RESET
//	//正输入选择AIN0，负输入选择AIN1，不可更改
//	//ADS124x_WriteReg(ADS_MUX0,0x01);  //00 001 000,Bit7-6:传感器电流源检测不使用，Bit5-3:正输入为AIN0，Bit2-0:负输入为AIN1
//	//ADS124x_WriteReg(ADS_VBIAS,0x00); //00000000,Bit7-0:偏置电压选择关闭（默认）
//	//固定选择外部REF1作为参考电压源，不可更改
//	ADS124x_WriteReg(ADS_MUX1,0x30); //000 00 000,Bit7：启用内部晶振，Bit6-5:内部基准一直关闭，Bit4-3:输入基准为外部REF1，Bit2-0:普通模式
//	ADS124x_WriteReg(ADS_SYS0,PGAGAIN_32 | DATARATE_5); //0000 0000，Bit7:必须设置为0，Bit6-4：选择增益：1-128倍，Bit3-0：选择测量速度 ：5-2000SPS
//	//ADS124x_WriteReg(ADS_IDAC0,0x05); //0000 0000,Bit7-4:只读，Bit3:DRDY/DOUT功能复用Dout线 ，Bit2-0:关闭内部恒流源 	0x06,设置为1mA
//	//ADS124x_WriteReg(ADS_IDAC1,0x2f); //1111 1111,Bit7-4      设置为0x8f 设置1mA电流从ADS1248的IEXC1输出
////	//固定，不可更改
////	ADS124x_WriteReg(ADS_OFC0,0x00); //00000000
////	ADS124x_WriteReg(ADS_OFC1,0x00); //00000000
////	ADS124x_WriteReg(ADS_OFC2,0x00); //00000000
////	ADS124x_WriteReg(ADS_FSC0,0x00); //00000000
////	ADS124x_WriteReg(ADS_FSC1,0x00); //00000000
////	ADS124x_WriteReg(ADS_FSC2,0x00); //00000000
//	
//	//GPIO对应端口初始化，，，设置为数字输出端口，不可更改
//	//ADS124x_WriteReg(ADS_GPIOCFG,0xec); //11101100,Bit7-0:设置对应端口为模拟口还是GPIO，置1设为GPIO
//	//ADS124x_WriteReg(ADS_GPIODIR,0x13); //0001 0011,Bit7-0:设置对应GPIO端口为输入还是输出，0为输出，1为输入
//	//激励源设置，电压激励或1mA电流激励
//	
//	//增益自适应，，，测量速度可选择5、10、 20、40、80、160、320、640、1000、2000SPS
//	//GPIO端口操作，设置电桥类型，四种
//	//ADS124x_WriteReg(ADS_GPIODAT,Bridge_Type); //1000 0100,Bit7-0:GPIO读写口，，，，两线制1/4桥
//	ADS124x_WriteCommand(ADS_RDATAC);//发送连续转换指令
//	Delay(110);
//}

//unsigned char ADS124x_Check_Par()
//{           
//		unsigned char temp=0;
//	
//	
//		temp=ADS124x_ReadREG(ADS_SYS0);//发送停止转换指令 ADS_RESET
//		
//		if(temp == (PGAGAIN_32 | DATARATE_5) )
//		{
//			return 1;
//		}
//		else 
//		{
//			SenPower_OFF();
//			Delay(500);
//			ClC_WatchDogTask();
//			Delay(200);
//			SenPower_ON();	
//		}	
//}

/*******************************************************************************
* Function Name  : ADS1248_Sample
* Description    : ADS1248底层采样驱动程序
* Input          : None
* Output         : None
* Return         : ADC采样值，有符号长整型
*******************************************************************************/
//float ADS124x_ReadData(void)
//{
//	unsigned char result[3]={0,0,0};
//	signed long adcVal=0;
//	float adcmV=0;
//	
//	result[0]=SPI1_ReadWriteByte(ADS_NOP);
//	result[1]=SPI1_ReadWriteByte(ADS_NOP);
//	result[2]=SPI1_ReadWriteByte(ADS_NOP);
//	
//	adcVal=(signed long)( result[0] << 24 | result[1] << 16 | result[2] << 8 );//数据整合
//	
//	if(adcVal>0x7fffffff)//符号位转换
//	{
//		adcVal=(~adcVal)+0x01;
//	}
//	adcVal=adcVal>>8;
//	adcmV=((float)adcVal/0x7fffff)*2048.0/32;	//采样电压值变换
//	
//	//log_info("adcmV:%fmV\r\n",adcmV);
//	
//	return adcmV;
//}


/*******************************************************************************
* Function Name  : ADCSmaple
* Description    : 压力信号采集函数，并将其转换为电压信号
* Input          : None
* Output         : None
* Return         : 电压信号（mV）
*******************************************************************************/

//#define ADCBUFF_LENGTH 10
//float ADS_buff[ADCBUFF_LENGTH]={0};
//unsigned int Sampling_Count=0;
//unsigned char Sampling_Complete_Flag=0;
//volatile unsigned char ADC_DADY_Flag=0;
//float ADCSmaple(void)
//{
//	unsigned char i,j;
//	float buff=0;
//	float ADCsum=0;
//	float ADS_Result=0;
//	
//	if(ADC_DADY_Flag)
//	{
//		SPI1_CS_Select(ADS124x);//片选信号选择ADS_124X
//		ADC_DADY_Flag=0;
//		ADS_buff[Sampling_Count]=ADS124x_ReadData();
//		log_info("ADS[%d]:%fmV\r\n",Sampling_Count,ADS_buff[Sampling_Count]);
//		Sampling_Count++;
//		AD_CS_H();
//		
//		if(Sampling_Count>=10)
//		{
//			Sampling_Count=0;
//			//冒泡排序，采样10次，从小到大排序，取中间5个数，求均值
//			for(i=0;i<ADCBUFF_LENGTH;i++)
//			{
//				for(j=i+1;j<ADCBUFF_LENGTH;j++)
//				{
//					if(ADS_buff[i]>ADS_buff[j])//从小到大，改为"<"变为从大到小
//					{
//						buff=ADS_buff[i];
//						ADS_buff[i]=ADS_buff[j];
//						ADS_buff[j]=buff;
//					}
//				}
//			}
//			
//			ADCsum=0;
//			for(i=3;i<7;i++)
//			{
//				ADCsum+=ADS_buff[i];
//			}
//			ADS_Result=ADCsum/4;
//			log_info("ADS_Result:%fmV\r\n\r\n",ADS_Result);
//			InputRegister.ADCOriginalValue=ADS_Result;
//			
//			Sampling_Complete_Flag=1;
//		}
//	}
//	return 0;

//}


/*******************************************************************************
* Function Name  : Level_height_conversion
* Description    : 将采集到的电压信号转化为压强值，并进一步转换为高度值，同时进行温漂修正
* Input          : None
* Output         : 更新输入寄存器值
* Return         : None
*******************************************************************************/
void Level_height_conversion(float ADS_R,float ADT_temp)
{
	unsigned char i;
	float SV[6]={0};
	float SV_m[6]={0};
	float OriginaAltitudeTemp=0,PA=0,ADCValue=0;
	double T_d=0;
	double P0[5]={0},P1[5]={0},P2[5]={0},P3[5]={0},P4[5]={0},P5[5]={0};
	double TEMP_Cor_VAL[6]={0};
	double Cor_result=0;
	double Liquid_D=0;
	double Liquid_B=0;//液体密度
	double LD_V[4]={0};
		
	/******************************************************************
	**计算液体密度
	******************************************************************/
	//根据液体类型，计算液体密度参数
 	if((KeepRegister.Liquid_Sec&0x0F00) == 0x0100)      {LD_V[0] =  0.00000001874 ; LD_V[1] = -0.000006231;  LD_V[2] =  0.0000262; LD_V[3] = 1     ;}       //纯净水
 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0200) {LD_V[0] =  0.000000002806; LD_V[1] = -0.000002759;  LD_V[2] = -0.0003734; LD_V[3] = 1.075 ;}       //长城FD-1型 -25℃防冻液
 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0300) {LD_V[0] = -0.00000003493;  LD_V[1] =  0.000001245;  LD_V[2] = -0.0005355; LD_V[3] = 1.096; }       //长城YF-2A型 -45℃防冻液
 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0400) {LD_V[0] = -0.00000003079;  LD_V[1] =  0.0000006097; LD_V[2] = -0.0004351; LD_V[3] = 1.078; }       //壳牌OAT -30℃防冻液
 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0500) {LD_V[0] = -0.00000002093;  LD_V[1] = -0.0000001951; LD_V[2] = -0.0004336; LD_V[3] = 1.09;  }       //壳牌OAT -45℃
		
//	if((KeepRegister.Liquid_Sec&0x0F00) == 0x0100)      Liquid_B = 1     ;       //纯净水
// 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0200) Liquid_B = 1.075 ;       //长城FD-1型 -25℃防冻液
// 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0300) Liquid_B = 1.096 ;        //长城YF-2A型 -45℃防冻液
// 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0400) Liquid_B = 1.078 ;       //壳牌OAT -30℃防冻液
// 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0500) Liquid_B = 1.09  ;        //壳牌OAT -45℃
	
 	T_d=20;
	Liquid_B=LD_V[0]*T_d*T_d*T_d + LD_V[1]*T_d*T_d + LD_V[2]*T_d + LD_V[3];  //计算在20℃下的液体密度
//	//log_info("Liquid_B:%f\r\n",Liquid_B);
//		
	T_d=ADT_temp;  //获取当前温度
	Liquid_D=LD_V[0]*T_d*T_d*T_d + LD_V[1]*T_d*T_d + LD_V[2]*T_d + LD_V[3];  //计算液体密度 
	//Liquid_D = Liquid_B;
//	//log_info("Liquid_D:%f\r\n",Liquid_D);
	
	/************************************************************************
	**多阶线性校准                                                           
	************************************************************************/
	for(i=0;i<6;i++) SV[i]= i*(float)KeepRegister.Sensor_Range*1000/5;       //计算五阶校准标准点
	
	
	ADCValue = ADS_R;	                             //采样
		
	//传感器五阶校准，同时将电信号（mV）转换为压力信号（Pa）
	if     (ADCValue < KeepRegister.MV[1]){PA=(ADCValue-KeepRegister.MV[0])*(SV[1]-SV[0])/(KeepRegister.MV[1]-KeepRegister.MV[0])+SV[0];}
	else if(ADCValue < KeepRegister.MV[2]){PA=(ADCValue-KeepRegister.MV[1])*(SV[2]-SV[1])/(KeepRegister.MV[2]-KeepRegister.MV[1])+SV[1];}
	else if(ADCValue < KeepRegister.MV[3]){PA=(ADCValue-KeepRegister.MV[2])*(SV[3]-SV[2])/(KeepRegister.MV[3]-KeepRegister.MV[2])+SV[2];}
	else if(ADCValue < KeepRegister.MV[4]){PA=(ADCValue-KeepRegister.MV[3])*(SV[4]-SV[3])/(KeepRegister.MV[4]-KeepRegister.MV[3])+SV[3];}
	else                                  {PA=(ADCValue-KeepRegister.MV[4])*(SV[5]-SV[4])/(KeepRegister.MV[5]-KeepRegister.MV[4])+SV[4];}
	
	/*       计算温漂修正前的高度数据               */
	OriginaAltitudeTemp= ( PA/(Liquid_B*1000*KeepRegister.LocalAccelerationOfGravity) );   //p=ρgh+此地的大气压 ρ:水：1.0X1000 Kg/m3    g：9.8N/kg     高度:m
	OriginaAltitudeTemp =OriginaAltitudeTemp*1000;//转换成mm
	InputRegister.LiquidAltitudeAbsoluteValue_Befor = OriginaAltitudeTemp;
	InputRegister.AltitudeDifference_Befor = InputRegister.LiquidAltitudeAbsoluteValue_Befor - KeepRegister.OriginaLiquidAltitude_Befor;
	
	/*       计算温漂修正后的高度数据               */
	OriginaAltitudeTemp= ( PA/(Liquid_D*1000*KeepRegister.LocalAccelerationOfGravity) );   //p=ρgh+此地的大气压 ρ:水：1.0X1000 Kg/m3    g：9.8N/kg     高度:m
	OriginaAltitudeTemp =OriginaAltitudeTemp*1000;//转换成mm
	
	
	
	/************************************************************************
	**多负载温度修正                                                         
	************************************************************************/
//	if( (KeepRegister.Liquid_Sec & 0x0F) == 0x01)
//	{
		for(i=0;i<6;i++)SV_m[i]=SV[i]/(Liquid_D*KeepRegister.LocalAccelerationOfGravity);   //计算标准压力下的mm液体柱
		//6条校准曲线，温度-压强差曲线
		//除1000，获取真实系数
		for(i=0;i<5;i++)P0[i]=((double)KeepRegister.LTC0[i])/1000;
		for(i=0;i<5;i++)P1[i]=((double)KeepRegister.LTC1[i])/1000;
		for(i=0;i<5;i++)P2[i]=((double)KeepRegister.LTC2[i])/1000;
		for(i=0;i<5;i++)P3[i]=((double)KeepRegister.LTC3[i])/1000;
		for(i=0;i<5;i++)P4[i]=((double)KeepRegister.LTC4[i])/1000;
		for(i=0;i<5;i++)P5[i]=((double)KeepRegister.LTC5[i])/1000;
		
		//6条温度曲线
		//根据当前温度，获取6条曲线上的压强差校准点，
		//温度修正公式，四阶修正
		TEMP_Cor_VAL[0] = P0[4]*pow(T_d,4) + P0[3]*pow(T_d,3) + P0[2]*T_d*T_d + P0[1]*T_d + P0[0];   //  零负载下温度-修正值曲线
		TEMP_Cor_VAL[1] = P1[4]*pow(T_d,4) + P1[3]*pow(T_d,3) + P1[2]*T_d*T_d + P1[1]*T_d + P1[0];   //  1/5负载下温度-修正值曲线
		TEMP_Cor_VAL[2] = P2[4]*pow(T_d,4) + P2[3]*pow(T_d,3) + P2[2]*T_d*T_d + P2[1]*T_d + P2[0];   //  1/5负载下温度-修正值曲线
		TEMP_Cor_VAL[3] = P3[4]*pow(T_d,4) + P3[3]*pow(T_d,3) + P3[2]*T_d*T_d + P3[1]*T_d + P3[0];   //  1/5负载下温度-修正值曲线
		TEMP_Cor_VAL[4] = P4[4]*pow(T_d,4) + P4[3]*pow(T_d,3) + P4[2]*T_d*T_d + P4[1]*T_d + P4[0];   //  1/5负载下温度-修正值曲线
		TEMP_Cor_VAL[5] = P5[4]*pow(T_d,4) + P5[3]*pow(T_d,3) + P5[2]*T_d*T_d + P5[1]*T_d + P5[0];   //  满负载下温度-修正值曲线
	
		//六个温度压强曲线上的六个压强差校准点，压强
		for(i=0;i<6;i++)TEMP_Cor_VAL[i]=TEMP_Cor_VAL[i]/(KeepRegister.LocalAccelerationOfGravity*Liquid_D);        //将压强转换为mm液体柱值
		
		if     (ADCValue < KeepRegister.MV[1]){Cor_result=(OriginaAltitudeTemp-SV_m[0])*(TEMP_Cor_VAL[1]-TEMP_Cor_VAL[0])/(SV_m[1]-SV_m[0])+TEMP_Cor_VAL[0];}
		else if(ADCValue < KeepRegister.MV[2]){Cor_result=(OriginaAltitudeTemp-SV_m[1])*(TEMP_Cor_VAL[2]-TEMP_Cor_VAL[1])/(SV_m[2]-SV_m[1])+TEMP_Cor_VAL[1];}
		else if(ADCValue < KeepRegister.MV[3]){Cor_result=(OriginaAltitudeTemp-SV_m[2])*(TEMP_Cor_VAL[3]-TEMP_Cor_VAL[2])/(SV_m[3]-SV_m[2])+TEMP_Cor_VAL[2];}
		else if(ADCValue < KeepRegister.MV[4]){Cor_result=(OriginaAltitudeTemp-SV_m[3])*(TEMP_Cor_VAL[4]-TEMP_Cor_VAL[3])/(SV_m[4]-SV_m[3])+TEMP_Cor_VAL[3];}
		else                                  {Cor_result=(OriginaAltitudeTemp-SV_m[4])*(TEMP_Cor_VAL[5]-TEMP_Cor_VAL[4])/(SV_m[5]-SV_m[4])+TEMP_Cor_VAL[4];}
//	}
//	else 
//		Cor_result=0;
	
	OriginaAltitudeTemp=OriginaAltitudeTemp - Cor_result; 

	//InputRegister.ADCOriginalValue = ADCValue;													//将ADC原始采样值（mV）存入输入寄存器
	InputRegister.PA  =  PA;
	InputRegister.LiquidAltitudeAbsoluteValue_After = OriginaAltitudeTemp;							//将采集到的液位绝对高度值存入输入寄存器
	InputRegister.AltitudeDifference_After= OriginaAltitudeTemp - KeepRegister.OriginaLiquidAltitude_After;	//计算高度差，之后将数据存入输入寄存器
}













