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

/*      寄存器列表 均为8bits		*/
enum
{
    MMA8451_STATUS_REG = 0x00,              //状态寄存器
    MMA8451_OUT_X_MSB, 											//数据寄存器
		MMA8451_OUT_X_LSB, 
		MMA8451_OUT_Y_MSB, 
		MMA8451_OUT_Y_LSB, 
		MMA8451_OUT_Z_MSB, 
		MMA8451_OUT_Z_LSB,
    
    MMA8451_F_SETUP = 0x09, 								//FIFO Setup Register
		MMA8451_TRIG_CFG, 											//触发配置 only available on the MMA8451 variant
    
    MMA8451_SYSMOD = 0x0B,									//系统模式配置
    MMA8451_INT_SOURCE, 										//中断状态寄存器
		MMA8451_WHO_AM_I, 											//设备ID
		MMA8451_XYZ_DATA_CFG, 									//XYZ数据配置寄存器
		MMA8451_HP_FILTER_CUTOFF, 							//高通滤波器截止频率
	
		/*	Portrait/Landscape Embedded Function Registers	*/
		MMA8451_PL_STATUS,											//Portrait/Landscape状态寄存器
    MMA8451_PL_CFG, 												//Portrait/Landscape配置寄存器
		MMA8451_PL_COUNT, 											//Portrait/Landscape去抖计数器
		MMA8451_PL_BF_ZCOMP, 										//后/前和Z补偿寄存器
		MMA8451_PL_THS_REG, 										//纵向/横向阈值和滞后寄存器
		
		/*	Motion and Freefall Embedded Function Registers	*/
		MMA8451_FF_MT_CFG, 											//Freefall/运动配置寄存器
		MMA8451_FF_MT_SRC,											//Freefall/运动动作源寄存器
    MMA8451_FF_MT_THS, 											//Freefall/运动阈值寄存器
		MMA8451_FF_MT_COUNT,										//Freefall/运动计数器寄存器
    
		/*	Transient (HPF) Acceleration Detection	*/
    MMA8451_TRANSIENT_CFG = 0x1D,						//瞬态配置寄存器
    MMA8451_TRANSIENT_SRC, 									//瞬态源配置寄存器
		MMA8451_TRANSIENT_THS, 									//瞬态阈值寄存器
		MMA8451_TRANSIENT_COUNT, 								//瞬态计数器寄存器
		
		/*	Single, Double and Directional Tap Detection Registers	*/
		MMA8451_PULSE_CFG, 											//脉冲配置寄存器
		MMA8451_PULSE_SRC,											//脉冲源配置寄存器
    MMA8451_PULSE_THSX, 										//X轴脉冲阈值配置寄存器
		MMA8451_PULSE_THSY, 										//Y轴脉冲阈值配置寄存器
		MMA8451_PULSE_THSZ, 										//Z轴脉冲阈值配置寄存器
		MMA8451_PULSE_TMLT, 										//脉冲时间窗口寄存器
		MMA8451_PULSE_LTCY, 										//脉冲延迟定时器寄存器
		MMA8451_PULSE_WIND,											//第二脉冲时间窗口寄存器
		
		/*	Auto-WAKE/SLEEP Detection	*/
    MMA8451_ASLP_COUNT, 										//自动唤醒/休眠寄存器
		
		/*	Control Registers	*/
		MMA8451_CTRL_REG1 = 0x2A, 											//系统控制1寄存器
		MMA8451_CTRL_REG2,  										//系统控制2寄存器
		MMA8451_CTRL_REG3,  										//系统控制3寄存器
		MMA8451_CTRL_REG4,  										//系统控制4寄存器
		MMA8451_CTRL_REG5, 											//系统控制5寄存器
		
		/*	User Offset Correction Registers	*/
    MMA8451_OFF_X, 													//X轴偏移量寄存器
		MMA8451_OFF_Y, 													//Y轴偏移量寄存器 
		MMA8451_OFF_Z 													//Z轴偏移量寄存器
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
















