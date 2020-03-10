#include "main.h"

/*****************************************************
* ������Flash��     ��Flash��Կ�Ĵ���FLASH_KEYR��д��KEY1 = 0x45670123
* 					����Flash��Կ�Ĵ���FLASH_KEYR��д��KEY2 = 0xCDEF89AB
* �����ݲ���λ����
* ������������
* ��д�����ݡ�
*****************************************************/

/****************************************************
* �ڲ�Flash���洢���ṹ
* �ڲ�Flash memory��0x0800 0000-0x0801 ffff������128KB
*	���д�0x0800 0000-0x0800 7fff��32KB���ڴ��Bootloader��hex��
*	���д�0x0x0800 8000-0x0801 ffff����96KB���ڴ��Ӧ�ó���(bin)
* ����32��4KB������ÿ��������Ϊ16��256B Page
* S0      PO          256B
*					P1					256B
*					P2          256B
*					P3					256B
*					P4-7        1KB
*					P8-11       1KB
*					P12-15      1KB
* S1									4KB
* S2									4KB
* ...
* S31									4KB
****************************************************/

volatile FLASH_Status FLASHStatus;

/*******************************************************************************
* Function Name  : Read_From_Flash
* Description    : ��Flash�ж�������
* Input          : Flash��ַ���������ָ�롢���ȣ�8λ,���ֽڣ�
* Output         : ��������
* Return         : None
*******************************************************************************/
void Read_NByte_From_Flash(unsigned int Addr,unsigned char *pBuffer,unsigned int Len)   	
{
	unsigned int i;
	for(i=0;i<Len;i++)
	{
		pBuffer[i]=*(volatile unsigned char*)Addr;//��ȡ1���ֽ�.
		Addr++;//ƫ��1���ֽ�.	
	}
}
/*******************************************************************************
* Function Name  : Read_2NByte_From_Flash
* Description    : ��Flash�ж�������
* Input          : Flash��ַ���������ָ�롢���ȣ�16λ���֣�˫�ֽڳ��ȣ�
* Output         : ��������
* Return         : None
*******************************************************************************/
void Read_2NByte_From_Flash(unsigned int Addr,unsigned short *pBuffer,unsigned short Len)   	
{
	unsigned short i;
	for(i=0;i<Len;i++)
	{
		pBuffer[i]=*(volatile unsigned short*)Addr;//��ȡ2���ֽ�.
		Addr+=2;//ƫ��2���ֽ�.	
	}
}

/*******************************************************************************
* Function Name  : Read_4NByte_From_Flash
* Description    : ��Flash�ж�������
* Input          : Flash��ַ���������ָ�롢���ȣ�32λ�֣����ֽڳ��ȣ�
* Output         : ��������
* Return         : None
*******************************************************************************/
void Read_4NByte_From_Flash(unsigned int Addr,unsigned int *pBuffer,unsigned int Len)   	
{
	unsigned int i;
	for(i=0;i<Len;i++)
	{
		pBuffer[i]=*(volatile unsigned int*)Addr;//��ȡ2���ֽ�.
		Addr+=4;//ƫ��2���ֽ�.	
	}
}


/*******************************************************************************
* Function Name  : FLASH_If_Init
* Description    : �ڲ�Flash��ʼ�������������־λ
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void FLASH_If_Init(void)
{ 
  /* Unlock the Program memory */
  FLASH_Unlock();

  /* Clear all FLASH flags */  
  FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                  | FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);   
}
/*******************************************************************************
* Function Name  : FLASH_If_Erase
* Description    : �ڲ�Flash�������ӿ�ʼ��������������Flash
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
unsigned int FLASH_If_Erase(uint32_t StartAddr)
{
  uint32_t flashaddress;
	
  flashaddress = StartAddr;
  SystemDisableIRQ();
  while (flashaddress <= (uint32_t) FLASH_LAST_PAGE_ADDRESS)
  {
    if (FLASH_ErasePage(flashaddress) == FLASH_COMPLETE)
    {
      flashaddress += FLASH_PAGE_SIZE;
    }
    else
    {
      /* Error occurred while page erase */
			SystemEnableIRQ();
      return (1);
    }
  }
	SystemEnableIRQ();
  return (0);
}
/*******************************************************************************
* Function Name  : FLASH_Page_Erase
* Description    : �ڲ�Flashҳ����
* Input          : ����(0-31,����APPռ��8-31)��ҳ(0-15)
* Output         : None
* Return         : None
*******************************************************************************/
unsigned int FLASH_Page_Erase(uint16_t sector,uint16_t Page)
{
  uint32_t flashaddress;
  if(sector<8 || sector>31 || Page>15 )return 1;
	
  flashaddress = FLASH_START_ADDRESS+sector*FLASH_SECTOR_SIZE+Page*FLASH_PAGE_SIZE;
  SystemDisableIRQ();
  FLASH_If_Init();
  FLASH_ErasePage(flashaddress);
  FLASH_Lock();
  SystemEnableIRQ();
  
  return (0);
}

/*******************************************************************************
* Function Name  : FLASH_Write
* Description    : �ڲ�дFlash����4N��ַ��ʼд
* Input          : ��ַ������ָ�롢���ݳ���
* Output         : None
* Return         : None
*******************************************************************************/
unsigned int Write_4NByte_to_Flash(unsigned int FlashAddress, unsigned int* Pr ,unsigned short DataLength)
{
  //FLASH_Status status = FLASH_BUSY;
	unsigned int i;
	unsigned char status=0;
//	unsigned int sector_addr;    	//������ַ
//	unsigned short sector_off;		//����ƫ��
//	unsigned short sector_cre;		//������ʣ���ַ
//	unsigned int   offaddr;				//ȥ��0x0800 0000��ĵ�ַ
	
	if((FlashAddress<FLASH_APP_ADDRESS) || (FlashAddress>=FLASH_END_ADDRESS) )return 2;//��ַ�Ϸ����
	if(FlashAddress % 4)return 2;//��ַ�Ϸ�����飬д��ʼ��ַ������4��������
	
//	offaddr=FlashAddress-FLASH_START_ADDRESS;		//�����0x0800 0000��ƫ����.
//	sector_addr=offaddr/FLASH_SECTOR_SIZE;    //����������ַ
//	
//	sector_off=(offaddr%FLASH_SECTOR_SIZE)/4;	//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
//	sector_cre=FLASH_SECTOR_SIZE/2-sector_off;//����ʣ��ռ��С 
	SystemDisableIRQ();
	FLASH_If_Init();
  /* Write the buffer to the memory */
	for(i=0;i<DataLength;i++)
	{
    if( FLASH_FastProgramWord(FlashAddress, Pr[i]) == FLASH_COMPLETE)
		{
			FlashAddress=FlashAddress+4;
		}
		else 
		{
			status=1;
			break;
		}
	}
	FLASH_Lock();
	SystemEnableIRQ();
  return status;
}


void FLASH_Test(void)
{
		//Flash��д����
	unsigned int buf[512]={0};
	unsigned int buf1[512]={0};
	unsigned int i=0;
	unsigned int test_ADDR=0;
	unsigned int count=0;
	
	
	count=64;
	for(i=0;i<count;i++)
	buf1[i]=i;
	test_ADDR=0x0801Ef00;
	FLASH_Page_Erase(30,15);
	FLASH_Page_Erase(31,0);
	Read_4NByte_From_Flash(test_ADDR,buf,count);  //OK 
	for(i=0;i<count;i++)
	log_info("Flash:%d\r\n",buf[i]);
	Write_4NByte_to_Flash(test_ADDR,buf1,count);
	Read_4NByte_From_Flash(test_ADDR,buf,count);  //OK 
	for(i=0;i<count;i++)
	log_info("AFlash:%d\r\n",buf[i]);
	
}
















