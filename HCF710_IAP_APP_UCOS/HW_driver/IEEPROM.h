
#ifndef __IEEPROM_H
#define __IEEPROM_H

#define EEPROM_START_ADDR 	0x08080280
#define EEPROM_SIZE       	0x07FF
#define EEPROM_END_ADDR 	0x080807FF

//����BL��APP����ռ���ʼ��ַ
#define	EEPROM_SHARE_DATA_ADDR	0x180
#define	EEPROM_SHARE_DATA_SPACE	0x80

#define PEKEY1        		0x89ABCDEF                //FLASH_PEKEYR
#define PEKEY2        		0x02030405                //FLASH_PEKEYR    ���·ֱ�ʵ�ְ��ֽں��ַ�ʽд�룺



void Write_EEPROM(unsigned int addr,unsigned char data);
unsigned char Read_EEPROM(unsigned int addr);

void EERead(unsigned short addr, unsigned char *pbuff,unsigned short length);
void EEWrite(unsigned short addr, unsigned char *pbuff,unsigned short length);
void EEErase(unsigned short addr, unsigned short length);

#endif


