#ifndef __MODBUS_H
#define __MODBUS_H

////��С��ת��
//#define htons(n) ((((n) & 0x00ff) << 8) | (((n) & 0xff00) >> 8))
//#define ntohs(n) htons(n)
//#define htonl(n) ((((n) & 0x000000ff) << 24)|  \
//				 (((n) & 0x0000ff00) << 8)  |  \
//				 (((n) & 0x00ff0000) >> 8)  |  \
//				 (((n) & 0xff000000) >> 24))
//#define ntohl(n) htonl(n)
//#define htond(n) (htonl(n & 0xffffffff) << 32) | htonl(n >> 32)
//#define ntohd(n) htond(n)

//typedef union{													 //float�����ݴ�С�˸�ʽת��
//	float f;
//	char c[4];
//}FLOAT_CONV;


//modbus ��Ϣ���սṹ��
typedef struct __attribute__ ((__packed__))
{
	unsigned char DataFlag;					//���ݱ��,�Ƿ�������
	unsigned short DataLen;					//���ݳ���
	unsigned char dat[UART1_RBUF_SIZE];	    //���ݻ���
}ModbusDataPackage_type;

extern volatile ModbusDataPackage_type ModbusDataPackage;	//����modbus���ջ���

static float __ltobf(float data);
unsigned short CRC16_Check(unsigned char *Pushdata,unsigned char length);
void ModbusReturnAckInfo(unsigned char err);

void InstructionTask(void);
char Get_SNInfo_Fun(void);











#endif


