/**
  ******************************************************************************
  * @file    Project/STM32L1xx_StdPeriph_Templates/main.h 
  * @author  MCD Application Team
  * @version V1.2.0
  * @date    16-May-2014
  * @brief   Header for main.c module
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "stm32l1xx.h"
#include "uart.h"
#include "gpio.h"
#include "SPI.h"
//#include "I2C.h"
#include "IEEPROM.h"
#include "Modbus.h"
#include "IADC.h"
#include "PAS.h"
#include "Timer.h"
#include "Temp.h"
#include "includes.h"
#include "stm32l1xx_it.h"


//����log_info
//#define debug
//#define debug2

#ifdef debug
		 #define log_info(...)    	GPIO_SetBits(GPIOB,GPIO_Pin_5); printf(__VA_ARGS__);GPIO_ResetBits(GPIOB,GPIO_Pin_5);
#else
		#define log_info(...)
#endif

//����log_info


#ifdef debug2
		 #define log1_info(...)     GPIO_SetBits(GPIOB,GPIO_Pin_5); printf(__VA_ARGS__);GPIO_ResetBits(GPIOB,GPIO_Pin_5);
#else
		#define log1_info(...)
#endif


#define SystemResetSoft()				*((uint32_t *)0xe000ed0c)=0x05fa0004; //ʵ��ϵͳ�����λ


#define KREEPROM_BASEADDR			  	0x32	 		//���ּĴ����洢����ַ
#define FPOWERONFLAG_BASEADDR		  	0x16	   		//�״ο�����־λ

#define SUPERMODE_FLAG_BASEADDR       	0x24	   		//�߼�ģʽ��־λ��ŵ�ַ

#define ERROR			              	0		   		//ʧ��
#define SUCCESS			            	1		   		//�ɹ�

#define DEVICETYPE					  	0x02c6	   //HCF710�豸����,��ʮ����710ת��Ϊʮ������
#define SOFTWAREVERSION			  		0x0401     //����汾�Ŷ�����򣺰汾�Ź���λ��ʾ������1.2.3����ʾ����Ϊ���߰�λ��ʾ1���Ͱ�λ��ʾ23,���еͰ�λ����ʾ99

#define DefaultDeviceADDR			  	0x01	   	//Ĭ��������ַ
#define DefaultLiquidDensity		  	1.0	   		//Ĭ��Һ���ܶ�
#define DefaultAltitude				  	0.0	  		//Ĭ�ϳ�ʼ�߶�
#define DefaultAccelerationOfGravity  	9.8015   //Ĭ�ϵ����������ٶȣ�������
#define DefaultSensor_Range			  	20       //Ĭ�ϴ���������Ϊ20KPa

#define Liquid_Water					0x01     //liquid selecet code
#define Liquid_FD01_25					0x02
#define Liquid_YF2A_45					0x03
#define Liquid_OAT_30					0x04
#define Liquid_OAT_45					0x05

#define KeepRegister_Num	    (sizeof(KeepRegister)/2)
#define KeepRegister_Byte_Num    sizeof(KeepRegister)


/* Below this define I/O ports with CPU*/
#define LED0_ON()        GPIOB->BSRRH = GPIO_Pin_9//
#define LED0_OFF()    	 GPIOB->BSRRL = GPIO_Pin_9//

#define LED1_ON()        GPIOB->BSRRH = GPIO_Pin_8//
#define LED1_OFF()    	 GPIOB->BSRRL = GPIO_Pin_8//
#define LED1_COM()		 GPIOB->ODR ^= GPIO_Pin_8;

#define RS485_RX()    	 GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define RS485_TX()    	 GPIO_SetBits(GPIOB,GPIO_Pin_5)

#define SenPower_ON()    GPIO_SetBits(GPIOA,GPIO_Pin_3)
#define SenPower_OFF()   GPIO_ResetBits(GPIOA,GPIO_Pin_3)


typedef struct __attribute__ ((__packed__))
{
	float	   	XACC;								//ACC X  ���ٶ�ֵ
	float	   	YACC;								//ACC Y
	float	   	ZACC;								//ACC Z
	float	   	TACC;								//ACC T
	float    	ang_x;
	float    	ang_y;
	float    	ang_z;
	float	   	XMAG;								//MAG Z��
	float	   	YMAG;								//MAG Z��
	float	   	ZMAG;								//MAG Z��
	float  	 	azimuth;
	float    	pitch;
	float    	roll;
}IMU_Data_type;

/* Exported constants --------------------------------------------------------*/

extern volatile IMU_Data_type IMU_Data;
extern volatile unsigned int   WorkTime;
extern volatile unsigned char  WorkTimeOutFlag;
extern volatile unsigned char  Temp_Flag;  
extern volatile unsigned char  MainPower_Flag;
extern volatile unsigned char  MMA8451_Flag;
extern volatile unsigned int   Time_Count;	

extern volatile unsigned int SysTick_Count;
extern volatile unsigned int SysTick_Count1;
extern volatile unsigned int TimingDelay;


/* Exported functions ------------------------------------------------------- */
void Delay(volatile unsigned int nTime);

void Init_SysTick(void);
void Init_Devices(void);
void Init_Parameter(void);


void ClC_WatchDogTask(void);

















#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/







