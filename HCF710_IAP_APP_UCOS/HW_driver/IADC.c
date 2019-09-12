#include "main.h"
#include "includes.h"

#define ADC_Wait_TimeOut  		0xffff     //超时常量 0x5000		
/*******************************************************************************
* Function Name  : IADC_Configuration
* Description    : 配置ADC
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/		
void  IADC_Configuration(void)
{ 	
	unsigned short timeout;
	ADC_InitTypeDef ADC_InitStructure; 
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_HSICmd(ENABLE);  // Enable The HSI (16Mhz)
	                 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;				//PB1 作为模拟通道输入引脚        
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;												//模拟输入引脚
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStructure);	
	
	timeout=ADC_Wait_TimeOut;
	while((RCC_GetFlagStatus(RCC_FLAG_HSIRDY) == RESET) && (timeout--));   	// Check that HSI oscillator is ready
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	//Enable ADC1 clock
	
	
	ADC_StructInit(&ADC_InitStructure);
	ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStructure.ADC_ScanConvMode = DISABLE;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfConversion = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_384Cycles);	//ADC1 regular channe configuration
	
	ADC_DelaySelectionConfig(ADC1, ADC_DelayLength_Freeze);		//Define delay between ADC1 conversions
	ADC_PowerDownCmd(ADC1, ADC_PowerDown_Idle_Delay, ENABLE);	//Enable ADC1 Power Down during Delay
	
	ADC_Cmd(ADC1, ENABLE);										//Enable ADC1
	timeout=ADC_Wait_TimeOut;
	while ((ADC_GetFlagStatus(ADC1, ADC_FLAG_ADONS) == RESET) && (timeout--));	//Wait until ADC1 ON status

	/* Start ADC1 Software Conversion */ 
	//ADC_SoftwareStartConv(ADC1);								//Start ADC1 Software Conversion
	//while (ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC) == RESET)	//Wait until ADC Channel end of conversion
	//{
	//}															
}				  
/*******************************************************************************
* Function Name  : Get_Adc
* Description    : 获取ADC数据
* Input          : 通道
* Output         : ADC返回值
* Return         : None
*******************************************************************************/
unsigned short Get_Adc()   
{
	unsigned short timeout;
	timeout=ADC_Wait_TimeOut;
  //设置指定ADC的规则组通道，一个序列，采样时间
	ADC_SoftwareStartConv(ADC1);//ADC转换开始  			    	
	while((!ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC )) && (timeout--));//等待转换结束
	return ADC_GetConversionValue(ADC1);	//返回最近一次ADC1规则组的转换结果
}
/*******************************************************************************
* Function Name  : Get_Adc_Average
* Description    : 获取ADC平均数据
* Input          : 通道，平均次数
* Output         : ADC返回值
* Return         : None
*******************************************************************************/
float Get_Adc_Average(unsigned char times)
{
	OS_ERR err;
	OS_OPT opt; 
	
	unsigned int adc_val=0;
	unsigned char t;
	float temp=0;
	CPU_SR_ALLOC();
	for(t=0;t<times;t++)
	{
		OS_CRITICAL_ENTER();	//临界区
		adc_val+=Get_Adc();
		OS_CRITICAL_EXIT();	//临界区
		OSTimeDly(5,opt,&err);
	}
	
	adc_val=adc_val/times;
	temp=11.2857*3.3*adc_val/4096;
	
	return temp;
} 	 


