


#include "main.h"

/**********************************************
* Function Name  : EXTI9_5_Config
* Description    : 
* Input          : None
* Output         : None
* Return         : None


//����ͬ����ռʽ���ȼ����жϲ���ִ���ж�Ƕ�ף�����ռʽ���ȼ��Ŀ�����ռ�����ȼ��ĳ���

//**********************************************/
//void TIM3_Int_Init(unsigned short  arr,unsigned short psc)
//{
//	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStructure;
//	NVIC_InitTypeDef NVIC_InitStructure;
//	
//	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);  ///ʹ��TIM3ʱ��
//	
//	TIM_TimeBaseInitStructure.TIM_Period = arr; 	//�Զ���װ��ֵ
//	TIM_TimeBaseInitStructure.TIM_Prescaler=psc;  //��ʱ����Ƶ
//	TIM_TimeBaseInitStructure.TIM_CounterMode=TIM_CounterMode_Up; //���ϼ���ģʽ
//	TIM_TimeBaseInitStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
//	
//	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseInitStructure);//��ʼ��TIM3
//	
//	TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE); //����ʱ��3�����ж�
//	TIM_Cmd(TIM3,ENABLE); //ʹ�ܶ�ʱ��3
//	
//	NVIC_InitStructure.NVIC_IRQChannel=TIM3_IRQn; //��ʱ��3�ж�
//	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0x01; //��ռ���ȼ�1
//	NVIC_InitStructure.NVIC_IRQChannelSubPriority=0x03; //�����ȼ�3
//	NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
//	NVIC_Init(&NVIC_InitStructure);
//	
//}

////��ʱ��3�жϷ�����
//void TIM3_IRQHandler(void)
//{
//	if(TIM_GetITStatus(TIM3,TIM_IT_Update)==SET) //����ж�
//	{
//		
//	}
//	TIM_ClearITPendingBit(TIM3,TIM_IT_Update);  //����жϱ�־λ
//}

//��ʱ����100ms

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





