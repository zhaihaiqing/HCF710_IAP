


#include "main.h"

/**********************************************
* Function Name  : EXTI9_5_Config
* Description    : 
* Input          : None
* Output         : None
* Return         : None


具有同级抢占式优先级的中断不能执行中断嵌套，高抢占式优先级的可以抢占低优先级的程序

**********************************************/
void EXTI9_5_Config(void)
{
	EXTI_InitTypeDef   EXTI_InitStructure;
	GPIO_InitTypeDef   GPIO_InitStructure;
	NVIC_InitTypeDef   NVIC_InitStructure;
	/* Enable GPIOA clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	/* Configure PA6 pin as input floating */
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;																//
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;															//
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;													//
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;													//
	GPIO_Init(GPIOA, &GPIO_InitStructure); 

	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	/* Connect EXTI6 Line to PA6 pin */
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource6);

	/* Configure EXTI6 line */
	EXTI_InitStructure.EXTI_Line = EXTI_Line6;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	/* Enable and set EXTI6 Interrupt to the lowest priority */
	NVIC_InitStructure.NVIC_IRQChannel = EXTI9_5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}









