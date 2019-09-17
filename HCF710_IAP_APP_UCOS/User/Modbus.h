#ifndef __MODBUS_H
#define __MODBUS_H

//��С��ת��
#define htons(n) ((((n) & 0x00ff) << 8) | (((n) & 0xff00) >> 8))
#define ntohs(n) htons(n)
#define htonl(n) ((((n) & 0x000000ff) << 24)|  \
				 (((n) & 0x0000ff00) << 8)  |  \
				 (((n) & 0x00ff0000) >> 8)  |  \
				 (((n) & 0xff000000) >> 24))
#define ntohl(n) htonl(n)
#define htond(n) (htonl(n & 0xffffffff) << 32) | htonl(n >> 32)
#define ntohd(n) htond(n)

typedef union{													 //float�����ݴ�С�˸�ʽת��
	float f;
	char c[4];
}FLOAT_CONV;


/* Exported types ------------------------------------------------------------*/
//modbus ������
enum ModbusFunction_t
{
	ReadKeepRegistor=0x03,        //��ȡ���ּĴ���
	ReadInputRegistor=0x04,       //��ȡ����Ĵ���
	WriteSingleRegistor=0x06,     //Ԥ�õ��Ĵ���
	SoftWare_Reset	= 0x09,
	WriteSomeRegistor=0x10,       //Ԥ�ö�Ĵ���
	Get_SNInfo=0x40,              //��ȡSN��Ϣ
	AutoSetInitalValue=0x41,	  //�Զ����ó�ֵ
	StartSample	=0x42,			  //��������
	FactoryCalibration0=0x43,	  //����У׼��0
	FactoryCalibration1=0x44,	  //����У׼��1
	FactoryCalibration2=0x45,	  //����У׼��2
	FactoryCalibration3=0x46,	  //����У׼��3
	FactoryCalibration4=0x47,	  //����У׼��4
	FactoryCalibration5=0x48,	  //����У׼��5
	Clear_temp_corr    =0x49,     //����¶�����ϵ��
	ModeSwitch		   =0x66,     //ģʽ�л�
	FactoryParameterReset=0x68,   //�ָ���������
};

enum err_num  {
	err_Fu=1,           //��֧�ֵĹ��ܣ��������쳣��
	err_add,            //�Ĵ�����ַ����ȷ
	err_Re_VOR,         //�Ĵ���ֵ������Χ
	err_OE,             //��Ч���������쳣
	err_mode,
	//err_CON,          //ȷ��
	//err_Fu_NUM,       //���ܺ���Ч
	//err_busy          //�豸æ
};


//���ּĴ����ṹ��
typedef struct __attribute__ ((__packed__))   //�����ṹ��������ṹ����뷽ʽΪ�ֽڶ���
{
	unsigned short DeviceAddress;				//�豸��ַ
	unsigned short DeviceGroupNum;				//�豸���
	float	 OriginaLiquidAltitude_After;		//Һλ��ʼ�߶ȸ�λ
	float	 OriginaLiquidAltitude_Befor;		//Һ���ܶ�
	unsigned short Sensor_Range;				//���������̣�KPa��
	unsigned short Liquid_Sec;					//Һ��ѡ��
	float    LocalAccelerationOfGravity;		//�����������ٶ�ֵ����λ��
	float    Sensor_FS_Val;                 	//�ڲ�OEMѹ�����������������ֵ���ο�ֵ��
	float    MV[6];                         	//���У׼�����6����
	float    Temp_T0;                       	//��ʼ�¶�ֵ
	float    LTC0[5];           				// 0����������ֵ-�¶�����ϵ����4��     Load_Temp_Curve
	float    LTC1[5];           				// 1/5����������ֵ-�¶�����ϵ����4��
	float    LTC2[5];           				// 2/5����������ֵ-�¶�����ϵ����4��
	float    LTC3[5];           				// 3/5����������ֵ-�¶�����ϵ����4��
	float    LTC4[5];           				// 4/5����������ֵ-�¶�����ϵ����4��
	float    LTC5[5];           				// ������������ֵ-�¶�����ϵ����4��
	unsigned short	Average_num;
	unsigned short  bps;
	
}KeepRegister_type;

//����Ĵ����ṹ��
typedef struct __attribute__ ((__packed__))
{
	unsigned short DeviceType;						//�豸����
	unsigned short SoftwareVersion;					//����汾
	unsigned short Reserve1;						//����1
	unsigned short Reserve2;						//����2
	unsigned short   SN[8];							//SN��
	unsigned short Reserve3;						//����3
	unsigned short Reserve4;						//����4
	unsigned short Reserve5;						//����5
	unsigned short Reserve6;						//����6
	unsigned int SystemWorkTime;					//ϵͳ����ʱ��
	unsigned short SystemWorkStatus;				//ϵͳ����״̬
	float    Temperature;							//�¶�ֵ
	float    ADCOriginalValue;						//ADCԭʼ��������
	float    PA;
	float    LiquidAltitudeAbsoluteValue_Befor;			//Һλ����ֵ�߶�ֵ
	float    AltitudeDifference_Befor;					//����ֵҺλ�߶Ȳ�
	float    LiquidAltitudeAbsoluteValue_After;			//Һλ����ֵ�߶�ֵ
	float    AltitudeDifference_After;					//����ֵҺλ�߶Ȳ�
	float    MainPower_V;
}InputRegister_type;



//modbus ��Ϣ���սṹ��
typedef struct __attribute__ ((__packed__))
{
	unsigned char DataFlag;						//���ݱ��,�Ƿ�������
	unsigned char DataLen;						//���ݳ���
	unsigned char dat[UART1_RBUF_SIZE];	    	//���ݻ���
	
}ModbusDataPackage_type;


extern volatile KeepRegister_type		KeepRegister;		//���屣�ּĴ���
extern volatile InputRegister_type		InputRegister;		//��������Ĵ���
extern volatile ModbusDataPackage_type ModbusDataPackage;	//����modbus���ջ���

extern unsigned char SuperMode_Flag;



extern volatile unsigned char Modbus_BusyFlag;
extern volatile unsigned char Modbus_FreeFlag;
static float __ltobf(float data);


void InstructionTask(void);
char Get_SNInfo_Fun(void);
uint16_t CRC16_Check(uint8_t *Pushdata,uint8_t length);
char CTC(unsigned char RX_Len);
char FactoryCalibration(unsigned char RX_Len);
char ModbusFactoryParameterReset(unsigned char RX_Len);
char ModbusReadInputRegistor(unsigned char RX_Len);
char ModbusReadKeepRegistor(unsigned char RX_Len);
char ModbusSetInitalValue(unsigned char RX_Len);
char ModbusSwitchMode(unsigned char RX_Len);
char ModbusWriteSingleRegistor(unsigned char RX_Len);
char ModbusWriteSomeRegistor(void);
void ModbusReturnAckInfo(uint8_t err);
void Instruction_Process_Subfunction(unsigned short RX_Len);
char ResetSoftware(unsigned short RX_Len);















#endif


