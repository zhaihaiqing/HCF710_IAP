#ifndef __IFLASH_H__
#define __IFLASH_H__

#include "stm32l1xx.h"

//�û������Լ�����Ҫ����

#define STM32L1XX_MD

#define ABS_RETURN(x,y)         (x < y) ? (y-x) : (x-y)





void Read_NByte_From_Flash(unsigned int Addr,unsigned char *pBuffer,unsigned int Len);			//���ֽڶ�������
void Read_2NByte_From_Flash(unsigned int Addr,unsigned short *pBuffer,unsigned short Len);	//�����ֶ�������
void Read_4NByte_From_Flash(unsigned int Addr,unsigned int *pBuffer,unsigned int Len); 			//���ֶ�������

void FLASH_If_Init(void);
unsigned int FLASH_If_Erase(unsigned int StartAddr);
unsigned int FLASH_Page_Erase(uint16_t sector,uint16_t Page);

unsigned int Write_4NByte_to_Flash(unsigned int FlashAddress, unsigned int* Pr ,unsigned short DataLength);

void FLASH_Test(void);

#endif

















