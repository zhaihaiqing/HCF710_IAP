/* Includes ------------------------------------------------------------------*/
#include "main.h"

iapfun jump2app; //����һ����������

//����ջ��ָ��
//addr:ջ����ַ
__asm void MSR_MSP(unsigned int addr) 
{
    MSR MSP, r0 		//set Main Stack value
    BX r14
}

/*******************************************************************************
* Function Name  : iap_load_app
* Description    : ��ת������������ת��APP���������С�
* Input          : APP������ʼ��ַ
* Output         : None
* Return         : None
*******************************************************************************/
void iap_load_app(unsigned int appxaddr)
{
	if(((*(volatile unsigned int*)appxaddr)&0x2FFE0000)==0x20000000)	//���ջ����ַ�Ƿ�Ϸ�.
	{ 
		jump2app=(iapfun)*(volatile unsigned int*)(appxaddr+4);					//�û��������ڶ�����Ϊ����ʼ��ַ(��λ��ַ)		
		MSR_MSP(*(volatile unsigned int*)appxaddr);								//��ʼ��APP��ջָ��(�û��������ĵ�һ�������ڴ��ջ����ַ)
		jump2app();																//��ת��APP.
	}
}	

/*******************************************************************************
* Function Name  : ClearUsartBuf
* Description    : ������ڻ���
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void ClearUsartBuf(void)
{
	USART1_ClearBuf_Flag();			//��մ��ڽ��ջ���
	ModbusDataPackage.DataLen = 0;  //����ճ��ȣ�ע�����˳��
	ModbusDataPackage.DataFlag = 0; //��ձ��λ
}

/*******************************************************************************
* Function Name  : Re_BootLoader_Info
* Description    : �ظ���ѯBootLoader��Ϣ
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void Re_BootLoader_Info(void)
{
	uint16_t M_crc=0;
	uint8_t	tem[8]={0};
	
	tem[0]=BL_Data.DeviceAddress;
	tem[1]=0xA2;
	tem[2]=(BL_Data.DeviceType&0xff00)>>8;
	tem[3]= BL_Data.DeviceType&0xff;
	tem[4]=(BL_Data.SoftwareVersion&0xff00)>>8;
	tem[5]= BL_Data.SoftwareVersion&0xff;
	M_crc=CRC16_Check(tem,6);
	tem[6] = (M_crc & 0xff);//У���8λ
	tem[7] = (M_crc >> 8);//У���8λ
	
	U485SendData(tem,8);//��485��������
	
	ClearUsartBuf();
}

/*******************************************************************************
* Function Name  : Re_BootLoader_Info
* Description    : �ظ�������Ϣ
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void Re_BootLoader_Update(void)
{
	uint16_t M_crc=0;
	uint8_t	tem[8]={0};
	
	tem[0]=0x00;tem[1]=0x00;tem[2]=0x00;tem[3]=0x00;
	EEWrite(Update_Firmware_Flag_Addr,tem,4);				//���̼�������ɱ�־λ��Ϊ0x08080808
	Update_Firmware_Flag=0x00;
				
	tem[0]=BL_Data.DeviceAddress;
	tem[1]=0xA3;
	tem[2]=0x44;
	tem[3]=0x66;
	tem[4]=0x88;
	tem[5]=0xaa;
	M_crc=CRC16_Check(tem,6);
	tem[6] = (M_crc & 0xff);//У���8λ
	tem[7] = (M_crc >> 8);//У���8λ
	U485SendData(tem,8);//��485��������
	
	ClearUsartBuf();
}


uint8_t Wait_Commend(void)
{
	uint16_t count=0,i=0,M_crc=0;
	
	count=1000;
	log_info("PowerON Wait Update Firmware 5S...\r\n");
	while(1)
	{
		Delay(5);
		ClC_WatchDogTask();//�ȴ�MT��Ϣ
		count--;
		if(count==0)
		{
			log_info("Wait_Update Timeout!\r\n");
			return 0;
		}
		
		if(ModbusDataPackage.DataFlag)
		{
			for(i=0;i<ModbusDataPackage.DataLen;i++)
			{
				ModbusDataPackage.dat[i] =USART1_GetChar();//���������ݷŵ�ָ��buf
			}
			
			M_crc = CRC16_Check((uint8_t *)ModbusDataPackage.dat,ModbusDataPackage.DataLen-2 );
			if( (( M_crc == ( (ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1] << 8) |    ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2])        )) || \
				(((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1]) == 0xff    ) && ((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2]) == 0xff))  )				
			{
			
				if((ModbusDataPackage.dat[0]==BL_Data.DeviceAddress)&&(ModbusDataPackage.DataLen==8))
				{
					if(ModbusDataPackage.dat[1]==0x22)Re_BootLoader_Info();
					if(ModbusDataPackage.dat[1]==0x23){Re_BootLoader_Update();return 0;};
				}
			}
			ClearUsartBuf();
		}
	}
}

/*******************************************************************************
* Function Name  : Update_Firmware
* Description    : �̼����º���
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

unsigned char Update_Firmware(void)
{
	unsigned char tem[12]={0};
	unsigned int databuf[BYTES_NUM_EACH_TRAN/4]={0};
	unsigned short i=0,j=0,M_crc=0,count=0,datbuf=0;

	//�̼�������Ҫ������
	//��һ�����ȴ�������Ϣ,�ȴ�ʱ��10s
	log_info("\r\nStep 1 :Wait MT ConnectInfo!\r\n");
	while(1)
	{
		i=1000;
		ClC_WatchDogTask();//�ȴ�MT��Ϣ
		while(!ModbusDataPackage.DataFlag)
		{
			Delay(10);
			ClC_WatchDogTask();//�ȴ�MT��Ϣ
			i--;
			if(i==0)
			{
				log_info("\r\nStep 1 :Wait MT Connect Timeout!\r\n");
				return 0;
			}
		}
		for(i=0;i<ModbusDataPackage.DataLen;i++)
		{
			ModbusDataPackage.dat[i] =USART1_GetChar();//���������ݷŵ�ָ��buf
		}
		M_crc = CRC16_Check((uint8_t *)ModbusDataPackage.dat,ModbusDataPackage.DataLen-2 );
		if( (( M_crc == ( (ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1] << 8) |    ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2])        )) || \
			(((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1]) == 0xff    ) && ((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2]) == 0xff))  )				
		{
			if( (ModbusDataPackage.dat[0]==BL_Data.DeviceAddress)&&(ModbusDataPackage.DataLen==8) && (ModbusDataPackage.dat[1]==0x25)  ) 
			{
				log_info("ConnectDevice OK!\r\n");
				tem[0]=BL_Data.DeviceAddress;
				tem[1]=0xA5;
				tem[2]=0x44;
				tem[3]=0x66;
				tem[4]=0x88;
				tem[5]=0xaa;
				M_crc=CRC16_Check(tem,6);
				tem[6] = (M_crc & 0xff);//У���8λ
				tem[7] = (M_crc >> 8);//У���8λ
				U485SendData(tem,8);//��485��������
				
				ClearUsartBuf();
				break;	//������ǰwhile(1)
			}
			else if( (ModbusDataPackage.dat[0]==BL_Data.DeviceAddress)&&(ModbusDataPackage.DataLen==8) && (ModbusDataPackage.dat[1]==0x22) )
			{
				Re_BootLoader_Info();
			}
			else 
			{
				ClearUsartBuf();
				//ModbusReturnAckInfo(02)//�����߷��ʹ���
			}
		}
		ClearUsartBuf();
		
	}
	
	//�ڶ������ȴ���������APP������Ϣ,�ȴ�ʱ��10s
	log_info("\r\nStep 2 :Wait MT APPImageInfo!\r\n");
	while(1)
	{
		i=1000;
		while(!ModbusDataPackage.DataFlag)
		{
			Delay(10);
			ClC_WatchDogTask();//�ȴ�MT��Ϣ
			i--;
			if(i==0)
			{
				log_info("\r\nStep 2 :Wait MT APPImageInfo Timeout!\r\n");
				return 0;
			}
		}
		//log_info("SysTick_Count:%d\r\n",SysTick_Count);
		for(i=0;i<ModbusDataPackage.DataLen;i++)
		{
			ModbusDataPackage.dat[i] =USART1_GetChar();//���������ݷŵ�ָ��buf
		}
		M_crc = CRC16_Check((uint8_t *)ModbusDataPackage.dat,ModbusDataPackage.DataLen-2 );
		if( (( M_crc == ( (ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1] << 8) |    ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2])        )) || \
			(((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1]) == 0xff    ) && ((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2]) == 0xff))  )				
		{
			if( (ModbusDataPackage.dat[0]==BL_Data.DeviceAddress)&&(ModbusDataPackage.DataLen==12) && (ModbusDataPackage.dat[1]==0x26)  ) 
			{
				BL_Data.Program_Version = (ModbusDataPackage.dat[2]<<8) | ModbusDataPackage.dat[3];
				BL_Data.Program_Size    = (ModbusDataPackage.dat[4]<<24) | (ModbusDataPackage.dat[5]<<16)| \
										  (ModbusDataPackage.dat[6]<<8) | ModbusDataPackage.dat[7];	
				BL_Data.Totle_Frame		=  ModbusDataPackage.dat[8]<<8 | ModbusDataPackage.dat[9];
				BL_Data.Totle_Frame_Count=	BL_Data.Totle_Frame;
	
				log_info("App Info-Program_Version:0x%x ,Program_Size:%d,Totle_Frame:%d\r\n",BL_Data.Program_Version,BL_Data.Program_Size,BL_Data.Totle_Frame);
	
				BL_Data.End_sector=BL_Data.Program_Size/4096 + BL_Data.Start_sector+1;	//����APP�������һλ����������
				if(BL_Data.End_sector>31)BL_Data.End_sector=31;
				log_info("CPU Erage Flash Now . . .Start_sector:%d,End_sector:%d\r\n",BL_Data.Start_sector,BL_Data.End_sector);
				for(i=BL_Data.Start_sector;i<=BL_Data.End_sector;i++)				//ѭ������APP����Ҫռ�õ�����
					for(j=BL_Data.Start_Page;j<=FLASH_PAGE_NUM_EACH_SECTOR;j++)
						FLASH_Page_Erase(i,j);										//����Flash
				log_info("Flash  Erage  OK . . .\r\n");
				log_info("Program_Version:%d.%d.%d Program_Size:%d Totle_Frame:%d\r\n",(BL_Data.Program_Version&0xff00)>>8,\
						(BL_Data.Program_Version&0xff)/10,(BL_Data.Program_Version&0xff)%10,BL_Data.Program_Size,BL_Data.Totle_Frame);				
				
				tem[0]=BL_Data.DeviceAddress;
				tem[1]=0xA6;
				tem[2]=0x44;
				tem[3]=0x66;
				tem[4]=0x88;
				tem[5]=0xaa;
				M_crc=CRC16_Check(tem,6);
				tem[6] = (M_crc & 0xff);//У���8λ
				tem[7] = (M_crc >> 8);	//У���8λ
				U485SendData(tem,8);	//��485��������
				
				//while(1);
				//log_info("SysTick_Count:%d\r\n",SysTick_Count);
				break;
			}
			else 
			{
				ClearUsartBuf();
				//ModbusReturnAckInfo(02)//�����߷��ʹ���
			}
		}
		ClearUsartBuf();
	}
	
	//��������ѭ�����վ������ݣ�ÿ�εȴ�ʱ��10s����ʱ�˳�
	log_info("\r\nStep 3 :Wait MT APPData!\r\n");
	ClearUsartBuf();
	while(1)
	{
		i=1000;
		while(!ModbusDataPackage.DataFlag)
		{
			Delay(10);
			ClC_WatchDogTask();//�ȴ�MT��Ϣ
			i--;
			if(i==0)
			{
				log_info("\r\nStep 3 :Wait MT APPData Timeout!\r\n");
				return 0;
			}
		}
		//log_info("SysTick_Count:%d\r\n",SysTick_Count);
		for(i=0;i<ModbusDataPackage.DataLen;i++)
		{
			ModbusDataPackage.dat[i] =USART1_GetChar();//���������ݷŵ�ָ��buf
		}
		M_crc = CRC16_Check((uint8_t *)ModbusDataPackage.dat,ModbusDataPackage.DataLen-2 );
		if( (( M_crc == ( (ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1] << 8) |    ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2])        )) || \
			(((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 1]) == 0xff    ) && ((ModbusDataPackage.dat[ModbusDataPackage.DataLen - 2]) == 0xff))  )				
		{
			if( (ModbusDataPackage.dat[0]==BL_Data.DeviceAddress)&&(ModbusDataPackage.DataLen==262) && (ModbusDataPackage.dat[1]==0x27)  ) 
			{
				//�������ϣ���flash,���һ�����ݣ����������Ĳ�0,ע���С�˸�ʽ
				memset(databuf,0,BYTES_NUM_EACH_TRAN/4);//�������
				for(i=0;i<BYTES_NUM_EACH_TRAN/4;i++)	//���ֽھ�ƴ���֣�4B��
				{
					databuf[i]=(ModbusDataPackage.dat[i*4+7]<<24) | (ModbusDataPackage.dat[i*4+6]<<16) | (ModbusDataPackage.dat[i*4+5]<<8) | ModbusDataPackage.dat[i*4+4];
					//log_info("databuf[%d]:0x%x\r\n",i,databuf[i]);
				}
				count++;
				log_info("Write to Flash %d. . .\r\n",count);
				Write_4NByte_to_Flash(BL_Data.APP_FlashAddr,databuf,BYTES_NUM_EACH_TRAN/4);	//��Flash��д������
				BL_Data.APP_FlashAddr = BL_Data.APP_FlashAddr+BYTES_NUM_EACH_TRAN;			//��ַ��256
				BL_Data.Totle_Frame--;										//ʣ����������--
				ClearUsartBuf();
				
				if(BL_Data.Totle_Frame)//���ݽ���δ���
				{
					tem[0]=BL_Data.DeviceAddress;
					tem[1]=0xA7;
					tem[2]=(count&0xff00)>>8;
					tem[3]=	count&0x00ff;
					tem[4]=0x88;
					tem[5]=0xaa;
					M_crc=CRC16_Check(tem,6);
					tem[6] = (M_crc & 0xff);//У���8λ
					tem[7] = (M_crc >> 8);//У���8λ
					U485SendData(tem,8);//��485��������
					
					//log_info("SysTick_Count:%d\r\n",SysTick_Count);
				}
				else	//���ݽ������
				{
					//���Ȼָ���������
					
					
					datbuf=0x03;
					EEWrite(0x280+0x32+174,(void *)&datbuf,2);//�޸Ĳ�����Ϊ9600
					
					
					tem[0]=BL_Data.DeviceAddress;
					tem[1]=0xB7;
					tem[2]=	 (BL_Data.Program_Size&0xff000000)>>24;
					tem[3]=	 (BL_Data.Program_Size&0x00ff0000)>>16;
					tem[4]=	 (BL_Data.Program_Size&0x0000ff00)>>8;
					tem[5]=	  BL_Data.Program_Size&0x000000ff;
					
					M_crc=CRC16_Check(tem,6);
					tem[6] = (M_crc & 0xff);//У���8λ
					tem[7] = (M_crc >> 8);//У���8λ
					U485SendData(tem,8);//��485��������
					
					//log_info("SysTick_Count:%d\r\n",SysTick_Count);
					
					tem[0]=0x08;tem[1]=0x08;tem[2]=0x08;tem[3]=0x08;
					EEWrite(Update_Firmware_Flag_Addr,tem,4);				//���̼�������ɱ�־λ��Ϊ0x08080808
					if(((*(volatile unsigned int*)(FLASH_APP_ADDRESS + 4)) & 0xFF000000) == 0x08000000)//APP�������λ����ȷ��ִ����ת
					{
						log_info("Complete Firmware Update!Go To APP...\r\n");
						iap_load_app(FLASH_APP_ADDRESS);  //��ת��APP�����
					}
					else 
					{
						log_info("Start APP Failed!\r\n");//�����תʧ�ܣ���ִ����תָ���ת��Э�鴦��
						SystemResetSoft();
						while(1);
					}
				}
			}
			else 
			{
				ClearUsartBuf();
				//ModbusReturnAckInfo(02)//�����߷��ʹ���
			}
		}
		ClearUsartBuf();
	}
	//return 0;
}

















