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
//#include <string.h>
//#include <math.h>
#include "stm32l1xx.h"
#include "uart.h"
#include "gpio.h"
#include "Modbus.h"

#include "IEEPROM.h"
#include "IFLASH.h"
#include "IAP.h"
#include "IAP_CONFIG.h"


#pragma pack(2)  //ָ�����ֽڶ���


//����log_info
//#define debug
#ifdef  debug
		//#define log_info(...)             printf(__VA_ARGS__);
		 #define log_info(...)          GPIO_SetBits(GPIOA,GPIO_Pin_4);printf(__VA_ARGS__);GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#else
		#define log_info(...)
#endif


#define SystemResetSoft()							*((uint32_t *)0xe000ed0c)=0x05fa0004; //ʵ��ϵͳ�����λ
#define SystemDisableIRQ()							__disable_irq()//�ر����ж�{ __ASM volatile ("cpsid i"); }
#define SystemEnableIRQ()							__enable_irq() //�������ж�{ __ASM volatile ("cpsie i"); }




#define ERROR			              	    0		   //ʧ��
#define SUCCESS			            	    1		   //�ɹ�

#define DEVICETYPE					        0x02c6	   //HCF1100�豸����,��ʮ����1100ת��Ϊʮ������
#define SOFTWAREVERSION			        	0x0400     //����汾�Ŷ�����򣺰汾�Ź���λ��ʾ������1.2.3����ʾ����Ϊ���߰�λ��ʾ1���Ͱ�λ��ʾ23,���еͰ�λ����ʾ99

#define DefaultDeviceADDR			        0x01	   
	   

#define KeepRegister_Num	               (sizeof(KeepRegister)/2)
#define KeepRegister_Byte_Num         		sizeof(KeepRegister)

//BL���ݽṹ��
typedef struct __attribute__ ((__packed__))
{
	unsigned short 	 DeviceAddress;				//�豸��485��ַ
	unsigned short 	 DeviceType;				//�豸����
	unsigned short   SoftwareVersion;			//BL����汾
	
	unsigned short   SN[8];
	
	unsigned short 	Program_Version;	//APP����汾
	unsigned int   	Program_Size;		//APP�����ֽ���
	unsigned short 	Totle_Frame;		//��������������
	
	unsigned short 	Totle_Frame_Count;	//�����ѽ�������������������
	unsigned char 	Start_sector;		//��ʼλ��Ϊ0x08008000,λ�ڵ�8����
	unsigned char 	End_sector;			//��������
	unsigned char 	Start_Page;			//��ʼҳ
	unsigned char 	End_Page;			//����ҳ
	unsigned int 	APP_FlashAddr;		//
	
}BL_Data_type;


/* Exported constants --------------------------------------------------------*/
extern volatile unsigned int SysTick_Count;
extern volatile unsigned int TimingDelay;

extern volatile BL_Data_type BL_Data;

/* Exported functions ------------------------------------------------------- */
void Delay(volatile unsigned int nTime);

void Init_SysTick(void);
void Init_Devices(void);
void Init_Parameter(void);


//extern unsigned short Device_ADDR;
extern const unsigned char  FactorySetValueWord[4];
extern unsigned int Update_Firmware_Flag;














#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/







