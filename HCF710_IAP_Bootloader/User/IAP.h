
#ifndef __IAP_H
#define __IAP_H



typedef  void (*iapfun)(void);				//����һ���������͵Ĳ���.

void iap_load_app(unsigned int appxaddr);			//ִ��flash�����app����
unsigned char Update_Firmware(void);
void iap_goto_app(unsigned int appxaddr);
uint8_t Wait_Commend(void);


#endif


