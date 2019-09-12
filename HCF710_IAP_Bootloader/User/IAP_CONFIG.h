
#ifndef __IAP_CONFIG_H
#define __IAP_CONFIG_H

#define	BYTES_NUM_EACH_TRAN			256			//����ÿ�������а����ľ����ֽ�����������4��������
/***********          IO����           *************************/

#define ClC_WatchDogTask()			GPIOA->ODR ^= GPIO_Pin_1	//���Ź�ι��

#define RS485_RX()    	 			GPIOB->BSRRH = GPIO_Pin_5;   //GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define RS485_TX()    	 			GPIOB->BSRRL = GPIO_Pin_5;   //GPIO_SetBits(GPIOB,GPIO_Pin_5)

#define LED1_ON()        			GPIOB->BSRRH = GPIO_Pin_9	//GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define LED1_OFF()    	 			GPIOB->BSRRL = GPIO_Pin_9	//GPIO_SetBits(GPIOB,GPIO_Pin_5)

/***********          Flash����           *************************/
//STM32L151CBT6-A  Cat.2
//Flash:����Ϊ0-31��������ÿ����������Ϊ0-15��page��ÿҳ256Bytes


#define STM32_FLASH_SIZE  128 	 					//��ѡSTM32��FLASH������С(��λΪKB)
#define FLASH_SECTOR_SIZE 4096 						//������С��4096���ֽڣ���STM32L151CBT6-A  Cat.2
#define FLASH_PAGE_SIZE   256						//ÿҳ256���ֽ�

#define FLASH_SECTOR_TOTAL_NUM			(STM32_FLASH_SIZE*1024/FLASH_SECTOR_SIZE)
#define FLASH_PAGE_NUM_EACH_SECTOR		(FLASH_SECTOR_SIZE/FLASH_PAGE_SIZE)

//FLASH��ʼ��ַ
#define FLASH_START_ADDRESS 0x08000000 	     		//STM32 FLASH����ʼ��ַ,BootLoader��ʼ��ַ
#define BootLoader_MAXSIZE  0x8000   				//STM32 BOOTLoader������󳤶�32K

#define FLASH_APP_ADDRESS 	0x08008000 				//STM32 FLASH����ʼ��ַ  APP������ʼ��ַ
#define APP_MAXSIZE   		0x18000					//STM32 APP������󳤶�96K

#define FLASH_LAST_PAGE_ADDRESS  		0x0801FF00	//Flash���һҳ����ʼ��ַ
#define FLASH_END_ADDRESS        		0x0801FFFF	//Flash���һ����ַ

#define FLASH_SECTOR_NUMBER  	  ((uint32_t)(ABS_RETURN(FLASH_APP_ADDRESS,FLASH_START_ADDRESS))>>12)
#define FLASH_PROTECTED_SECTORS   ((uint32_t)~((1 << FLASH_SECTOR_NUMBER) - 1))

#define FLASH_SIZE   			 (FLASH_END_ADDRESS - FLASH_APP_ADDRESS + 1)


/***********          IEEPROM����           *************************/

#define KREEPROM_BASEADDR			  	0x32	 //���ּĴ����洢����ַ

#define PEKEY1        0x89ABCDEF                //FLASH_PEKEYR
#define PEKEY2        0x02030405                //FLASH_PEKEYR    ���·ֱ�ʵ�ְ��ֽں��ַ�ʽд�룺

#define EEPROM_START_ADDR 	0x08080000        	//�ڲ�EEPROM��ʼ��ַ
#define EEPROM_SIZE       	0x0FFF			  	//�ڲ�EEPROM�ռ��С 4KB
#define EEPROM_END_ADDR 	0x08080FFF		  	//�ڲ�EEPROM������ַ

#define	EEPROM_BL_FLAG_ADDR		0x00			//����BootLoader��־λ��ʼ��ַ����С
#define	EEPROM_BL_FLAG_SPACE	0x80

#define	EEPROM_BL_DATA_ADDR		0x80			//����BootLoader������ʼ��ַ����С
#define	EEPROM_BL_DATA_SPACE	0x100

#define	EEPROM_SHARE_DATA_ADDR	0x180			//����BL��APP����ռ���ʼ��ַ
#define	EEPROM_SHARE_DATA_SPACE	0x80

#define	EEPROM_APP_FLAG_ADDR	0x200			//����APP��־λ��ʼ��ַ����С
#define	EEPROM_APP_FLAG_SPACE	0x80

#define	EEPROM_APP_DATA_ADDR	0x280			//����APP������ʼ��ַ����С
#define	EEPROM_APP_DATA_SPACE	0x800

#define First_PO_Flag_Addr 				 	EEPROM_BL_FLAG_ADDR+0     	//�״ο�����־λ�洢λ��
#define Update_Firmware_Flag_Addr 		 	EEPROM_BL_FLAG_ADDR+4   	//APP������±�־λ�洢λ��







#endif


