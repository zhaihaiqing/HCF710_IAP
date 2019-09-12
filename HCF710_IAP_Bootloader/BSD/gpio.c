
#include "main.h"

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef        GPIO_InitStructure;
	//ʹ�ܶ˿�ʱ��
	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE);

	/*�������*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				//PA1=WDI,
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure); 
	
	/*�������*/
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure); 
	
	
}

/*******************************************************************************
* Function Name  : GPIO_PinReverse
* Description    : GPIOȡ������
* Input          : GPIOx��GPIO_PIN_x
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_PinReverse(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin)
{
    assert_param(IS_GPIO_ALL_PERIPH(GPIOx));
    assert_param(IS_GPIO_PIN(GPIO_Pin));
    GPIOx->ODR ^= GPIO_Pin;
}
















