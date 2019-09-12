#ifndef __I2C_H
#define __I2C_H

void I2C1_Configuration(void);

void EE_Read(unsigned short addr ,unsigned char * pBuffer,unsigned short Length);
unsigned char EE_Write(unsigned short addr,unsigned char* pBuffer,  unsigned short Length);
void EE_EraseFullChip(void);
void EE_Check(void);




/*****************************************************************/
/*****************************************************************/
//MMA8451

/*      �Ĵ����б� ��Ϊ8bits		*/
enum
{
    MMA8451_STATUS_REG = 0x00,              //״̬�Ĵ���
    MMA8451_OUT_X_MSB, 											//���ݼĴ���
		MMA8451_OUT_X_LSB, 
		MMA8451_OUT_Y_MSB, 
		MMA8451_OUT_Y_LSB, 
		MMA8451_OUT_Z_MSB, 
		MMA8451_OUT_Z_LSB,
    
    MMA8451_F_SETUP = 0x09, 								//FIFO Setup Register
		MMA8451_TRIG_CFG, 											//�������� only available on the MMA8451 variant
    
    MMA8451_SYSMOD = 0x0B,									//ϵͳģʽ����
    MMA8451_INT_SOURCE, 										//�ж�״̬�Ĵ���
		MMA8451_WHO_AM_I, 											//�豸ID
		MMA8451_XYZ_DATA_CFG, 									//XYZ�������üĴ���
		MMA8451_HP_FILTER_CUTOFF, 							//��ͨ�˲�����ֹƵ��
	
		/*	Portrait/Landscape Embedded Function Registers	*/
		MMA8451_PL_STATUS,											//Portrait/Landscape״̬�Ĵ���
    MMA8451_PL_CFG, 												//Portrait/Landscape���üĴ���
		MMA8451_PL_COUNT, 											//Portrait/Landscapeȥ��������
		MMA8451_PL_BF_ZCOMP, 										//��/ǰ��Z�����Ĵ���
		MMA8451_PL_THS_REG, 										//����/������ֵ���ͺ�Ĵ���
		
		/*	Motion and Freefall Embedded Function Registers	*/
		MMA8451_FF_MT_CFG, 											//Freefall/�˶����üĴ���
		MMA8451_FF_MT_SRC,											//Freefall/�˶�����Դ�Ĵ���
    MMA8451_FF_MT_THS, 											//Freefall/�˶���ֵ�Ĵ���
		MMA8451_FF_MT_COUNT,										//Freefall/�˶��������Ĵ���
    
		/*	Transient (HPF) Acceleration Detection	*/
    MMA8451_TRANSIENT_CFG = 0x1D,						//˲̬���üĴ���
    MMA8451_TRANSIENT_SRC, 									//˲̬Դ���üĴ���
		MMA8451_TRANSIENT_THS, 									//˲̬��ֵ�Ĵ���
		MMA8451_TRANSIENT_COUNT, 								//˲̬�������Ĵ���
		
		/*	Single, Double and Directional Tap Detection Registers	*/
		MMA8451_PULSE_CFG, 											//�������üĴ���
		MMA8451_PULSE_SRC,											//����Դ���üĴ���
    MMA8451_PULSE_THSX, 										//X��������ֵ���üĴ���
		MMA8451_PULSE_THSY, 										//Y��������ֵ���üĴ���
		MMA8451_PULSE_THSZ, 										//Z��������ֵ���üĴ���
		MMA8451_PULSE_TMLT, 										//����ʱ�䴰�ڼĴ���
		MMA8451_PULSE_LTCY, 										//�����ӳٶ�ʱ���Ĵ���
		MMA8451_PULSE_WIND,											//�ڶ�����ʱ�䴰�ڼĴ���
		
		/*	Auto-WAKE/SLEEP Detection	*/
    MMA8451_ASLP_COUNT, 										//�Զ�����/���߼Ĵ���
		
		/*	Control Registers	*/
		MMA8451_CTRL_REG1 = 0x2A, 											//ϵͳ����1�Ĵ���
		MMA8451_CTRL_REG2,  										//ϵͳ����2�Ĵ���
		MMA8451_CTRL_REG3,  										//ϵͳ����3�Ĵ���
		MMA8451_CTRL_REG4,  										//ϵͳ����4�Ĵ���
		MMA8451_CTRL_REG5, 											//ϵͳ����5�Ĵ���
		
		/*	User Offset Correction Registers	*/
    MMA8451_OFF_X, 													//X��ƫ�����Ĵ���
		MMA8451_OFF_Y, 													//Y��ƫ�����Ĵ��� 
		MMA8451_OFF_Z 													//Z��ƫ�����Ĵ���
};

typedef struct
{
    float x;
    float y;
    float z;
}MMA8451_DATA_g;





unsigned char MMA8454_Init(void);
void MMA8454_Get_Status(void);
MMA8451_DATA_g MMA8454_Read_DATA(void);

void EE_Check(void);


#endif
















