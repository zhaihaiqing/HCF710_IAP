/* Includes ------------------------------------------------------------------*/
#include "main.h"

/*******************************************************************************
* Function Name  : ADS124x_Init1
* Description    : ADS124x��ʼ�����������ò��������в��ɸ��ĵļĴ���
									 ADS124x��ʼ�����������������͡�����Դ�������ٶ�
									 ADS124x�ڿ�������ʱ�����ݵ���������������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
//void ADS124x_Set_Par()
//{                  
//	ADS124x_WriteCommand(ADS_SDATAC);//����ֹͣת��ָ�� ADS_RESET
//	//������ѡ��AIN0��������ѡ��AIN1�����ɸ���
//	//ADS124x_WriteReg(ADS_MUX0,0x01);  //00 001 000,Bit7-6:����������Դ��ⲻʹ�ã�Bit5-3:������ΪAIN0��Bit2-0:������ΪAIN1
//	//ADS124x_WriteReg(ADS_VBIAS,0x00); //00000000,Bit7-0:ƫ�õ�ѹѡ��رգ�Ĭ�ϣ�
//	//�̶�ѡ���ⲿREF1��Ϊ�ο���ѹԴ�����ɸ���
//	ADS124x_WriteReg(ADS_MUX1,0x30); //000 00 000,Bit7�������ڲ�����Bit6-5:�ڲ���׼һֱ�رգ�Bit4-3:�����׼Ϊ�ⲿREF1��Bit2-0:��ͨģʽ
//	ADS124x_WriteReg(ADS_SYS0,PGAGAIN_32 | DATARATE_5); //0000 0000��Bit7:��������Ϊ0��Bit6-4��ѡ�����棺1-128����Bit3-0��ѡ������ٶ� ��5-2000SPS
//	//ADS124x_WriteReg(ADS_IDAC0,0x05); //0000 0000,Bit7-4:ֻ����Bit3:DRDY/DOUT���ܸ���Dout�� ��Bit2-0:�ر��ڲ�����Դ 	0x06,����Ϊ1mA
//	//ADS124x_WriteReg(ADS_IDAC1,0x2f); //1111 1111,Bit7-4      ����Ϊ0x8f ����1mA������ADS1248��IEXC1���
////	//�̶������ɸ���
////	ADS124x_WriteReg(ADS_OFC0,0x00); //00000000
////	ADS124x_WriteReg(ADS_OFC1,0x00); //00000000
////	ADS124x_WriteReg(ADS_OFC2,0x00); //00000000
////	ADS124x_WriteReg(ADS_FSC0,0x00); //00000000
////	ADS124x_WriteReg(ADS_FSC1,0x00); //00000000
////	ADS124x_WriteReg(ADS_FSC2,0x00); //00000000
//	
//	//GPIO��Ӧ�˿ڳ�ʼ������������Ϊ��������˿ڣ����ɸ���
//	//ADS124x_WriteReg(ADS_GPIOCFG,0xec); //11101100,Bit7-0:���ö�Ӧ�˿�Ϊģ��ڻ���GPIO����1��ΪGPIO
//	//ADS124x_WriteReg(ADS_GPIODIR,0x13); //0001 0011,Bit7-0:���ö�ӦGPIO�˿�Ϊ���뻹�������0Ϊ�����1Ϊ����
//	//����Դ���ã���ѹ������1mA��������
//	
//	//��������Ӧ�����������ٶȿ�ѡ��5��10�� 20��40��80��160��320��640��1000��2000SPS
//	//GPIO�˿ڲ��������õ������ͣ�����
//	//ADS124x_WriteReg(ADS_GPIODAT,Bridge_Type); //1000 0100,Bit7-0:GPIO��д�ڣ�������������1/4��
//	ADS124x_WriteCommand(ADS_RDATAC);//��������ת��ָ��
//	Delay(110);
//}

//unsigned char ADS124x_Check_Par()
//{           
//		unsigned char temp=0;
//	
//	
//		temp=ADS124x_ReadREG(ADS_SYS0);//����ֹͣת��ָ�� ADS_RESET
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
* Description    : ADS1248�ײ������������
* Input          : None
* Output         : None
* Return         : ADC����ֵ���з��ų�����
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
//	adcVal=(signed long)( result[0] << 24 | result[1] << 16 | result[2] << 8 );//��������
//	
//	if(adcVal>0x7fffffff)//����λת��
//	{
//		adcVal=(~adcVal)+0x01;
//	}
//	adcVal=adcVal>>8;
//	adcmV=((float)adcVal/0x7fffff)*2048.0/32;	//������ѹֵ�任
//	
//	//log_info("adcmV:%fmV\r\n",adcmV);
//	
//	return adcmV;
//}


/*******************************************************************************
* Function Name  : ADCSmaple
* Description    : ѹ���źŲɼ�������������ת��Ϊ��ѹ�ź�
* Input          : None
* Output         : None
* Return         : ��ѹ�źţ�mV��
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
//		SPI1_CS_Select(ADS124x);//Ƭѡ�ź�ѡ��ADS_124X
//		ADC_DADY_Flag=0;
//		ADS_buff[Sampling_Count]=ADS124x_ReadData();
//		log_info("ADS[%d]:%fmV\r\n",Sampling_Count,ADS_buff[Sampling_Count]);
//		Sampling_Count++;
//		AD_CS_H();
//		
//		if(Sampling_Count>=10)
//		{
//			Sampling_Count=0;
//			//ð�����򣬲���10�Σ���С��������ȡ�м�5���������ֵ
//			for(i=0;i<ADCBUFF_LENGTH;i++)
//			{
//				for(j=i+1;j<ADCBUFF_LENGTH;j++)
//				{
//					if(ADS_buff[i]>ADS_buff[j])//��С���󣬸�Ϊ"<"��Ϊ�Ӵ�С
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
* Description    : ���ɼ����ĵ�ѹ�ź�ת��Ϊѹǿֵ������һ��ת��Ϊ�߶�ֵ��ͬʱ������Ư����
* Input          : None
* Output         : ��������Ĵ���ֵ
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
	double Liquid_B=0;//Һ���ܶ�
	double LD_V[4]={0};
		
	/******************************************************************
	**����Һ���ܶ�
	******************************************************************/
	//����Һ�����ͣ�����Һ���ܶȲ���
 	if((KeepRegister.Liquid_Sec&0x0F00) == 0x0100)      {LD_V[0] =  0.00000001874 ; LD_V[1] = -0.000006231;  LD_V[2] =  0.0000262; LD_V[3] = 1     ;}       //����ˮ
 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0200) {LD_V[0] =  0.000000002806; LD_V[1] = -0.000002759;  LD_V[2] = -0.0003734; LD_V[3] = 1.075 ;}       //����FD-1�� -25�����Һ
 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0300) {LD_V[0] = -0.00000003493;  LD_V[1] =  0.000001245;  LD_V[2] = -0.0005355; LD_V[3] = 1.096; }       //����YF-2A�� -45�����Һ
 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0400) {LD_V[0] = -0.00000003079;  LD_V[1] =  0.0000006097; LD_V[2] = -0.0004351; LD_V[3] = 1.078; }       //����OAT -30�����Һ
 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0500) {LD_V[0] = -0.00000002093;  LD_V[1] = -0.0000001951; LD_V[2] = -0.0004336; LD_V[3] = 1.09;  }       //����OAT -45��
		
//	if((KeepRegister.Liquid_Sec&0x0F00) == 0x0100)      Liquid_B = 1     ;       //����ˮ
// 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0200) Liquid_B = 1.075 ;       //����FD-1�� -25�����Һ
// 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0300) Liquid_B = 1.096 ;        //����YF-2A�� -45�����Һ
// 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0400) Liquid_B = 1.078 ;       //����OAT -30�����Һ
// 	else if((KeepRegister.Liquid_Sec&0x0F00) == 0x0500) Liquid_B = 1.09  ;        //����OAT -45��
	
 	T_d=20;
	Liquid_B=LD_V[0]*T_d*T_d*T_d + LD_V[1]*T_d*T_d + LD_V[2]*T_d + LD_V[3];  //������20���µ�Һ���ܶ�
//	//log_info("Liquid_B:%f\r\n",Liquid_B);
//		
	T_d=ADT_temp;  //��ȡ��ǰ�¶�
	Liquid_D=LD_V[0]*T_d*T_d*T_d + LD_V[1]*T_d*T_d + LD_V[2]*T_d + LD_V[3];  //����Һ���ܶ� 
	//Liquid_D = Liquid_B;
//	//log_info("Liquid_D:%f\r\n",Liquid_D);
	
	/************************************************************************
	**�������У׼                                                           
	************************************************************************/
	for(i=0;i<6;i++) SV[i]= i*(float)KeepRegister.Sensor_Range*1000/5;       //�������У׼��׼��
	
	
	ADCValue = ADS_R;	                             //����
		
	//���������У׼��ͬʱ�����źţ�mV��ת��Ϊѹ���źţ�Pa��
	if     (ADCValue < KeepRegister.MV[1]){PA=(ADCValue-KeepRegister.MV[0])*(SV[1]-SV[0])/(KeepRegister.MV[1]-KeepRegister.MV[0])+SV[0];}
	else if(ADCValue < KeepRegister.MV[2]){PA=(ADCValue-KeepRegister.MV[1])*(SV[2]-SV[1])/(KeepRegister.MV[2]-KeepRegister.MV[1])+SV[1];}
	else if(ADCValue < KeepRegister.MV[3]){PA=(ADCValue-KeepRegister.MV[2])*(SV[3]-SV[2])/(KeepRegister.MV[3]-KeepRegister.MV[2])+SV[2];}
	else if(ADCValue < KeepRegister.MV[4]){PA=(ADCValue-KeepRegister.MV[3])*(SV[4]-SV[3])/(KeepRegister.MV[4]-KeepRegister.MV[3])+SV[3];}
	else                                  {PA=(ADCValue-KeepRegister.MV[4])*(SV[5]-SV[4])/(KeepRegister.MV[5]-KeepRegister.MV[4])+SV[4];}
	
	/*       ������Ư����ǰ�ĸ߶�����               */
	OriginaAltitudeTemp= ( PA/(Liquid_B*1000*KeepRegister.LocalAccelerationOfGravity) );   //p=��gh+�˵صĴ���ѹ ��:ˮ��1.0X1000 Kg/m3    g��9.8N/kg     �߶�:m
	OriginaAltitudeTemp =OriginaAltitudeTemp*1000;//ת����mm
	InputRegister.LiquidAltitudeAbsoluteValue_Befor = OriginaAltitudeTemp;
	InputRegister.AltitudeDifference_Befor = InputRegister.LiquidAltitudeAbsoluteValue_Befor - KeepRegister.OriginaLiquidAltitude_Befor;
	
	/*       ������Ư������ĸ߶�����               */
	OriginaAltitudeTemp= ( PA/(Liquid_D*1000*KeepRegister.LocalAccelerationOfGravity) );   //p=��gh+�˵صĴ���ѹ ��:ˮ��1.0X1000 Kg/m3    g��9.8N/kg     �߶�:m
	OriginaAltitudeTemp =OriginaAltitudeTemp*1000;//ת����mm
	
	
	
	/************************************************************************
	**�ฺ���¶�����                                                         
	************************************************************************/
//	if( (KeepRegister.Liquid_Sec & 0x0F) == 0x01)
//	{
		for(i=0;i<6;i++)SV_m[i]=SV[i]/(Liquid_D*KeepRegister.LocalAccelerationOfGravity);   //�����׼ѹ���µ�mmҺ����
		//6��У׼���ߣ��¶�-ѹǿ������
		//��1000����ȡ��ʵϵ��
		for(i=0;i<5;i++)P0[i]=((double)KeepRegister.LTC0[i])/1000;
		for(i=0;i<5;i++)P1[i]=((double)KeepRegister.LTC1[i])/1000;
		for(i=0;i<5;i++)P2[i]=((double)KeepRegister.LTC2[i])/1000;
		for(i=0;i<5;i++)P3[i]=((double)KeepRegister.LTC3[i])/1000;
		for(i=0;i<5;i++)P4[i]=((double)KeepRegister.LTC4[i])/1000;
		for(i=0;i<5;i++)P5[i]=((double)KeepRegister.LTC5[i])/1000;
		
		//6���¶�����
		//���ݵ�ǰ�¶ȣ���ȡ6�������ϵ�ѹǿ��У׼�㣬
		//�¶�������ʽ���Ľ�����
		TEMP_Cor_VAL[0] = P0[4]*pow(T_d,4) + P0[3]*pow(T_d,3) + P0[2]*T_d*T_d + P0[1]*T_d + P0[0];   //  �㸺�����¶�-����ֵ����
		TEMP_Cor_VAL[1] = P1[4]*pow(T_d,4) + P1[3]*pow(T_d,3) + P1[2]*T_d*T_d + P1[1]*T_d + P1[0];   //  1/5�������¶�-����ֵ����
		TEMP_Cor_VAL[2] = P2[4]*pow(T_d,4) + P2[3]*pow(T_d,3) + P2[2]*T_d*T_d + P2[1]*T_d + P2[0];   //  1/5�������¶�-����ֵ����
		TEMP_Cor_VAL[3] = P3[4]*pow(T_d,4) + P3[3]*pow(T_d,3) + P3[2]*T_d*T_d + P3[1]*T_d + P3[0];   //  1/5�������¶�-����ֵ����
		TEMP_Cor_VAL[4] = P4[4]*pow(T_d,4) + P4[3]*pow(T_d,3) + P4[2]*T_d*T_d + P4[1]*T_d + P4[0];   //  1/5�������¶�-����ֵ����
		TEMP_Cor_VAL[5] = P5[4]*pow(T_d,4) + P5[3]*pow(T_d,3) + P5[2]*T_d*T_d + P5[1]*T_d + P5[0];   //  ���������¶�-����ֵ����
	
		//�����¶�ѹǿ�����ϵ�����ѹǿ��У׼�㣬ѹǿ
		for(i=0;i<6;i++)TEMP_Cor_VAL[i]=TEMP_Cor_VAL[i]/(KeepRegister.LocalAccelerationOfGravity*Liquid_D);        //��ѹǿת��ΪmmҺ����ֵ
		
		if     (ADCValue < KeepRegister.MV[1]){Cor_result=(OriginaAltitudeTemp-SV_m[0])*(TEMP_Cor_VAL[1]-TEMP_Cor_VAL[0])/(SV_m[1]-SV_m[0])+TEMP_Cor_VAL[0];}
		else if(ADCValue < KeepRegister.MV[2]){Cor_result=(OriginaAltitudeTemp-SV_m[1])*(TEMP_Cor_VAL[2]-TEMP_Cor_VAL[1])/(SV_m[2]-SV_m[1])+TEMP_Cor_VAL[1];}
		else if(ADCValue < KeepRegister.MV[3]){Cor_result=(OriginaAltitudeTemp-SV_m[2])*(TEMP_Cor_VAL[3]-TEMP_Cor_VAL[2])/(SV_m[3]-SV_m[2])+TEMP_Cor_VAL[2];}
		else if(ADCValue < KeepRegister.MV[4]){Cor_result=(OriginaAltitudeTemp-SV_m[3])*(TEMP_Cor_VAL[4]-TEMP_Cor_VAL[3])/(SV_m[4]-SV_m[3])+TEMP_Cor_VAL[3];}
		else                                  {Cor_result=(OriginaAltitudeTemp-SV_m[4])*(TEMP_Cor_VAL[5]-TEMP_Cor_VAL[4])/(SV_m[5]-SV_m[4])+TEMP_Cor_VAL[4];}
//	}
//	else 
//		Cor_result=0;
	
	OriginaAltitudeTemp=OriginaAltitudeTemp - Cor_result; 

	//InputRegister.ADCOriginalValue = ADCValue;													//��ADCԭʼ����ֵ��mV����������Ĵ���
	InputRegister.PA  =  PA;
	InputRegister.LiquidAltitudeAbsoluteValue_After = OriginaAltitudeTemp;							//���ɼ�����Һλ���Ը߶�ֵ��������Ĵ���
	InputRegister.AltitudeDifference_After= OriginaAltitudeTemp - KeepRegister.OriginaLiquidAltitude_After;	//����߶Ȳ֮�����ݴ�������Ĵ���
}













