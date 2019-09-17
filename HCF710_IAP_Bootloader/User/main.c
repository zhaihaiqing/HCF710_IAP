/**
  ******************************************************************************
  * @file    main.c 
  * @author  Դ��ۺ�-�Ժ���
  * @version V4.0.0
  * @date    10-9-2018
  * @brief   Main program body
  ******************************************************************************
  */

#include "main.h"

volatile unsigned int SysTick_Count = 0;   //Systick����
volatile unsigned int TimingDelay = 0;     //��ʱ��������
const unsigned char  FactorySetValueWord[4] = {0x44,0x66,0x88,0xaa};//��������ָ�����

unsigned int First_PO_Flag = 0;     				//�״��ϵ��־λ			�洢λ�ã�EEPROM_BL_FLAG_ADDR+0

//�̼����±�־λ������4λ��ֵΪ0x08080808ʱִ�и��£�ֵΪ0x05050505ʱ��ʾ�������
unsigned int Update_Firmware_Flag = 0;     	//APP������±�־λ		�洢λ�ã�EEPROM_BL_FLAG_ADDR+4

volatile BL_Data_type BL_Data;

RCC_ClocksTypeDef  RCC_Clocks1;

/*******************************************************************************
* Function Name  : Delay function
* Description    : ��ʱ����������ʱ����sleepģʽ
* Input          : ��ʱʱ����ms��
* Output         : None
* Return         : None
*******************************************************************************/
void Delay(volatile unsigned int nTime)
{ 
	TimingDelay = nTime;
	while(TimingDelay != 0);//__WFI;//�ȴ������н���sleepģʽ
}

/*******************************************************************************
* Function Name  : SysTick init function
* Description    : ���ö�ʱ���ȼ�SysTick���ȼ�
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_SysTick(void)
{
	SysTick_Config(RCC_Clocks1.SYSCLK_Frequency / 1000); //���ö�ʱ���ȣ�1ms
	NVIC_SetPriority(SysTick_IRQn, 0x0);    //SysTick�ж����ȼ�����
}

void Init_Hardware(void)
{
	//���Ӳ����ʼ��
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);//�ж����ȼ�����
	Init_SysTick();//����SysTick��ʱ��
	GPIO_Configuration();
	UART1_Configuration(9600);

	log_info("Run in BootLoader,Hardware has init!\r\n");
	
	RS485_RX();
	Delay(5);
}

void Init_Parameter(void)
{
	uint8_t	tem[12]={0};
	
	Delay(5);								//������ʱ
	
	BL_Data.APP_FlashAddr = FLASH_APP_ADDRESS;										  //APP������ʼ��ַ
	BL_Data.Start_sector  = (FLASH_APP_ADDRESS-FLASH_START_ADDRESS)/FLASH_SECTOR_SIZE;//����APP������ʼ����
	
	//��ȡBL��־λ
	EERead(First_PO_Flag_Addr,tem,8);
	First_PO_Flag			=		tem[3]<<24 | tem[2]<<16 | tem[1]<<8 | tem[0];//�״ο�����־λ
	Update_Firmware_Flag	=		tem[7]<<24 | tem[6]<<16 | tem[5]<<8 | tem[4];//�̼�������־λ
	
	BL_Data.DeviceType=DEVICETYPE;
	BL_Data.SoftwareVersion=SOFTWAREVERSION;
	
	Get_SNInfo_Fun();		//��ȡSN��Ϣ��ͬʱ��ȡEEPROM��SN��Ϣ���ж�һ���ԣ���һ������дEEPROM
	
	ClC_WatchDogTask();//�ȴ�MT��Ϣ
	
	//�����Ҫ���£���ȡ��APP��ص�����
	EERead(EEPROM_APP_DATA_ADDR+KREEPROM_BASEADDR,tem,2);		//��ȡ�豸485ͨѶ��ַ
	BL_Data.DeviceAddress= tem[0];
	if( (BL_Data.DeviceAddress<1) || (BL_Data.DeviceAddress>247) )	
	{
		BL_Data.DeviceAddress=1;
	}
	
	log_info("Parameter has init! Device_ADDR:%d\r\n",BL_Data.DeviceAddress);
	log_info("First_PO_Flag:0x%x \r\n",First_PO_Flag);
	log_info("Update_Firmware_Flag:0x%x \r\n",Update_Firmware_Flag);
	log_info("DeviceType:0x%x \r\n",BL_Data.DeviceType);
	log_info("SoftwareVersion:0x%x \r\n",BL_Data.SoftwareVersion);
}



/*******************************************************************************
* Function Name  : main
* Description    : ������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main(void)
{		
	RCC_GetClocksFreq(&RCC_Clocks1);
	Init_Hardware();	//���Ӳ��������ʼ��
	Init_Parameter();	//��ɲ�����ʼ��
	
	while(1)
	{
		//�����������ɱ�־λ==0x08080808��ֱ����ת��APP����
		if( Update_Firmware_Flag == 0x08080808 )
		{
			ClC_WatchDogTask();	//�ȴ�MT��Ϣ
			Wait_Commend();		//����תǰ5���յ�����ָ�ִ��Updata����ֹ��������ʧ��
			
			if( (Update_Firmware_Flag == 0x08080808) && (((*(volatile unsigned int*)(FLASH_APP_ADDRESS + 4)) & 0xFF000000) == 0x08000000) )//APP�������λ����ȷ��ִ����ת
			{
				//ִ��IAP��ת
				log_info("No Update Task , Go To APP ....\r\n");
				ClC_WatchDogTask();	  
				iap_load_app(FLASH_APP_ADDRESS);  //��ת��APP�����
			}
			else 
			{
				log_info("Start APP Failed!\r\n");//�����תʧ�ܣ��ȴ����Ź���λ
				Update_Firmware();
				SystemResetSoft();//�̼�����ʧ��ֱ������
				while(1);
			}
		}
		//����ִ������
		else
		{
			log_info("Update_Firmware...\r\n");
			
			Update_Firmware();										//ִ�й̼�����
			SystemResetSoft();//�̼�����ʧ��ֱ������
			while(1);
		}
	}
}


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
