


#include "main.h"

/**********************************************
* Function Name  : EXTI9_5_Config
* Description    : 
* Input          : None
* Output         : None
* Return         : None


//具有同级抢占式优先级的中断不能执行中断嵌套，高抢占式优先级的可以抢占低优先级的程序

//**********************************************/
//void TIM3_Int_Init(unsigned short  arr,unsigned short psc)
//{
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///使能TIM3时钟
//	
//	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//自动重装载值
//	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //定时器分频
//	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
//	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
//	
//	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//初始化TIM3
//	
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //允许定时器3更新中断
//	TIM_Cmd(TIM3,ENABLE); //使能定时器3
//	
//	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //定时器3中断
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //抢占优先级1
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //子优先级3
//	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	
//}

////定时器3中断服务函数
//void TIM3_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //溢出中断
//	{
//		
//	}
//	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //清除中断标志位
//}

//定时周期100ms

void tmr1_callback(void)
{
	static unsigned short tmr1_num;
	tmr1_num++;
	
	if(tmr1_num>=KeepRegister.Average_num*10)
	{
		tmr1_num=0;
		AD779x_Sampling_Complete_Flag0=1;
	}
}





