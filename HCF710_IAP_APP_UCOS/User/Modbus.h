#ifndef __MODBUS_H
#define __MODBUS_H

//大小端转换
#define htons(n) ((((n) & 0x00ff) << 8) | (((n) & 0xff00) >> 8))
#define ntohs(n) htons(n)
#define htonl(n) ((((n) & 0x000000ff) << 24)|  \
				 (((n) & 0x0000ff00) << 8)  |  \
				 (((n) & 0x00ff0000) >> 8)  |  \
				 (((n) & 0xff000000) >> 24))
#define ntohl(n) htonl(n)
#define htond(n) (htonl(n & 0xffffffff) << 32) | htonl(n >> 32)
#define ntohd(n) htond(n)

typedef union{													 //float型数据大小端格式转换
	float f;
	char c[4];
}FLOAT_CONV;


/* Exported types ------------------------------------------------------------*/
//modbus 功能码
enum ModbusFunction_t
{
	ReadKeepRegistor=0x03,        //读取保持寄存器
	ReadInputRegistor=0x04,       //读取输入寄存器
	WriteSingleRegistor=0x06,     //预置单寄存器
	SoftWare_Reset	= 0x09,
	WriteSomeRegistor=0x10,       //预置多寄存器
	Get_SNInfo=0x40,              //获取SN信息
	AutoSetInitalValue=0x41,	  //自动设置初值
	StartSample	=0x42,			  //采样命令
	FactoryCalibration0=0x43,	  //出厂校准点0
	FactoryCalibration1=0x44,	  //出厂校准点1
	FactoryCalibration2=0x45,	  //出厂校准点2
	FactoryCalibration3=0x46,	  //出厂校准点3
	FactoryCalibration4=0x47,	  //出厂校准点4
	FactoryCalibration5=0x48,	  //出厂校准点5
	Clear_temp_corr    =0x49,     //清除温度修正系数
	ModeSwitch		   =0x66,     //模式切换
	FactoryParameterReset=0x68,   //恢复出厂设置
};

enum err_num  {
	err_Fu=1,           //非支持的功能（功能码异常）
	err_add,            //寄存器地址不正确
	err_Re_VOR,         //寄存器值超出范围
	err_OE,             //有效操作发生异常
	err_mode,
	//err_CON,          //确认
	//err_Fu_NUM,       //功能号无效
	//err_busy          //设备忙
};


//保持寄存器结构体
typedef struct __attribute__ ((__packed__))   //声明结构体变量，结构体对齐方式为字节对齐
{
	unsigned short DeviceAddress;				//设备地址
	unsigned short DeviceGroupNum;				//设备组号
	float	 OriginaLiquidAltitude_After;		//液位初始高度高位
	float	 OriginaLiquidAltitude_Befor;		//液体密度
	unsigned short Sensor_Range;				//传感器量程（KPa）
	unsigned short Liquid_Sec;					//液体选择
	float    LocalAccelerationOfGravity;		//当地重力加速度值（高位）
	float    Sensor_FS_Val;                 	//内部OEM压力传感器满量程输出值（参考值）
	float    MV[6];                         	//五阶校准所需的6个点
	float    Temp_T0;                       	//初始温度值
	float    LTC0[5];           				// 0负载下修正值-温度曲线系数，4阶     Load_Temp_Curve
	float    LTC1[5];           				// 1/5负载下修正值-温度曲线系数，4阶
	float    LTC2[5];           				// 2/5负载下修正值-温度曲线系数，4阶
	float    LTC3[5];           				// 3/5负载下修正值-温度曲线系数，4阶
	float    LTC4[5];           				// 4/5负载下修正值-温度曲线系数，4阶
	float    LTC5[5];           				// 满负载下修正值-温度曲线系数，4阶
	unsigned short	Average_num;
	unsigned short  bps;
	
}KeepRegister_type;

//输入寄存器结构体
typedef struct __attribute__ ((__packed__))
{
	unsigned short DeviceType;						//设备类型
	unsigned short SoftwareVersion;					//软件版本
	unsigned short Reserve1;						//保留1
	unsigned short Reserve2;						//保留2
	unsigned short   SN[8];							//SN号
	unsigned short Reserve3;						//保留3
	unsigned short Reserve4;						//保留4
	unsigned short Reserve5;						//保留5
	unsigned short Reserve6;						//保留6
	unsigned int SystemWorkTime;					//系统工作时间
	unsigned short SystemWorkStatus;				//系统工作状态
	float    Temperature;							//温度值
	float    ADCOriginalValue;						//ADC原始采样度数
	float    PA;
	float    LiquidAltitudeAbsoluteValue_Befor;			//液位绝对值高度值
	float    AltitudeDifference_Befor;					//减初值液位高度差
	float    LiquidAltitudeAbsoluteValue_After;			//液位绝对值高度值
	float    AltitudeDifference_After;					//减初值液位高度差
	float    MainPower_V;
}InputRegister_type;



//modbus 消息接收结构体
typedef struct __attribute__ ((__packed__))
{
	unsigned char DataFlag;						//数据标记,是否有数据
	unsigned char DataLen;						//数据长度
	unsigned char dat[UART1_RBUF_SIZE];	    	//数据缓存
	
}ModbusDataPackage_type;


extern volatile KeepRegister_type		KeepRegister;		//定义保持寄存器
extern volatile InputRegister_type		InputRegister;		//定义输入寄存器
extern volatile ModbusDataPackage_type ModbusDataPackage;	//定义modbus接收缓存

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


