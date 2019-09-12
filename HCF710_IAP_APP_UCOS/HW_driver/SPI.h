#ifndef __SPI_H
#define __SPI_H


#define AD779x      0x01
#define ADS124x     0x01					//连接至ADS124x
#define ADT7301     0x02					//连接至ADT7301高精度温度传感器

#define  SPI2_CLK_H()	 		GPIO_SetBits(GPIOB, GPIO_Pin_13)
#define  SPI2_CLK_L()	 		GPIO_ResetBits(GPIOB, GPIO_Pin_13)


#define  SPI2_MOSI_H()	 		GPIO_SetBits(GPIOB, GPIO_Pin_15)
#define  SPI2_MOSI_L()	 		GPIO_ResetBits(GPIOB, GPIO_Pin_15)
#define  Read_SPI_MISO    		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_14)

#define  ADS1247_DRDY    		GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)

#define  AD_CS_H()	 			GPIO_SetBits(GPIOA, GPIO_Pin_4)
#define  AD_CS_L()	 			GPIO_ResetBits(GPIOA, GPIO_Pin_4)

#define  TEMP_CS_H()	 		GPIO_SetBits(GPIOB, GPIO_Pin_0)
#define  TEMP_CS_L()	 		GPIO_ResetBits(GPIOB, GPIO_Pin_0)


/********************************************/
/* ADS1248 register map   寄存器地址        */
/*******************************************/
#define ADS_MUX0						0x00
#define ADS_VBIAS     					0x01
#define ADS_MUX1      					0x02
#define ADS_SYS0      					0x03
#define ADS_OFC0						0x04
#define ADS_OFC1						0x05
#define ADS_OFC2						0x06
#define ADS_FSC0						0x07
#define ADS_FSC1						0x08
#define ADS_FSC2						0x09
#define ADS_IDAC0						0x0a
#define ADS_IDAC1						0x0b
#define ADS_GPIOCFG						0x0c
#define ADS_GPIODIR						0x0d
#define ADS_GPIODAT						0x0e

/******************************************/
/* ADS1248 SPI Commands     SPI命令        */
/*******************************************/
#define ADS_WAKEUP						0x00
#define ADS_SLEEP						0x02
#define ADS_SYNC						0x04
#define ADS_RESET						0x06
#define ADS_NOP							0xff
#define ADS_RDATA						0x12
#define ADS_RDATAC						0x14
#define ADS_SDATAC						0x16
#define ADS_ReadREG						0x20
#define ADS_WriteREG					0x40
#define ADS_SYSOCAL						0x60
#define ADS_SYSGCAL						0x61
#define ADS_SELFOCAL					0x62

/************************************************************************/
/* ads1248 macroinstruction                                             */
/************************************************************************/
// about MUX0: Multiplexer Control Register 0
#define  P_AIN0                    0x00
#define  P_AIN1                    0x08
#define  P_AIN2                    0x10
#define  P_AIN3                    0x18
#define  P_AIN4                    0x20
#define  P_AIN5                    0x28
#define  P_AIN6                    0x30
#define  P_AIN7                    0x38
#define  N_AIN0                    0x00
#define  N_AIN1                    0x01
#define  N_AIN2                    0x02
#define  N_AIN3                    0x03
#define  N_AIN4                    0x04
#define  N_AIN5                    0x05
#define  N_AIN6                    0x06
#define  N_AIN7                    0x07

// about MUX1: Multiplexer Control Register 1
#define  CLK_Inter                  0x00
#define  CLK_Exter                  0x80
#define  REF_Inter_AlwaysOn         0x20  // selecte internal reference and always open  
#define  REF_Inter_AlwaysOff        0x00  // selecte internal reference and always off 
#define  SELT_REF0                  0x00                                
#define  SELT_REF1                  0x08
#define  SELT_Inter                 0x10
#define  SELT_Inter_REF0            0x18

// about SYS0 : System Control Register 0
#define         PGAGAIN_1           0x00
#define         PGAGAIN_2           0x10
#define         PGAGAIN_4           0x20
#define         PGAGAIN_8           0x30
#define         PGAGAIN_16          0x40
#define         PGAGAIN_32          0x50
#define         PGAGAIN_64          0x60
#define         PGAGAIN_128         0x70
#define         DATARATE_5          0x00
#define        	DATARATE_10         0x01
#define         DATARATE_20         0x02
#define         DATARATE_40         0x03
#define         DATARATE_80         0x04
#define         DATARATE_160        0x05
#define         DATARATE_320        0x06
#define         DATARATE_640        0x07
#define         DATARATE_1000       0x08
#define         DATARATE_2000       0x09

// about IDAC0: IDAC Control Register 0
#define  DRDY_MODE_EN               0x08
#define  DRDY_MODE_DIS              0x00

// the magnitude of the excitation current.
// The IDACs require the internal reference to be on.
#define  		IMAG_Off                 0x00
#define  		IMAG_50                  0x01
#define 		IMAG_100                 0x02
#define 		IMAG_250                 0x03
#define  		IMAG_500                 0x04
#define  		IMAG_750                 0x05
#define  		IMAG_1000                0x06
#define  		IMAG_1500                0x07

// about IDAC1: IDAC Control Register 1
// select the output pin for the first current source DAC.
#define         IDAC1_AIN0           0x00
#define         IDAC1_AIN1           0x10
#define         IDAC1_AIN2           0x20
#define         IDAC1_AIN3           0x30
#define         IDAC1_AIN4           0x40
#define         IDAC1_AIN5           0x50
#define         IDAC1_AIN6           0x60
#define         IDAC1_AIN7           0x70
#define         IDAC1_IEXT1          0x80
#define         IDAC1_IEXT2          0x90
#define         IDAC1_OFF            0xC0

//select the output pin for the second current source DAC.
#define         IDAC2_AIN0           0x00
#define         IDAC2_AIN1           0x01
#define         IDAC2_AIN2           0x02
#define         IDAC2_AIN3           0x03
#define         IDAC2_AIN4           0x04
#define         IDAC2_AIN5           0x05
#define         IDAC2_AIN6           0x06
#define         IDAC2_AIN7           0x07
#define         IDAC2_IEXT1          0x08
#define         IDAC2_IEXT2          0x09
#define         IDAC2_OFF            0x0C



#define ADS124X_DRDY    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)


#define Dummy_Byte                  0x00  				//不考虑的数据
//定义AD779x DRDY引脚
#define AD779x_RDY        			(GPIOA->IDR & GPIO_Pin_6)

#define AD779x_STATUS_RDY       0x80
#define AD779x_STATUS_ERROR     0x40
#define AD779x_STATUS_NOREF     0x20
#define AD779x_STATUS_CHAN_MASK 0x07

/*******************   AD779x寄存器列表   ***************************************/
#define AD779x_COMM_REG 	0x0			//通信寄存器，8bits，只写，所有的寄存器读写都是通过该寄存器完成的，CR7:Write EN,CR6:R/W,CR5-CR3:RS Add,CR2:CRead,CR1-CR0:0
#define AD779x_STATUS_REG 	0x0			//状态寄存器，8bits，Reset:0x80(AD7798)/0x88(AD7799)               SR7:RDY,SR6:Error bit,SR5:NOREF,SR4:0,SR3:0/1,SR2-SR0:CHx On converted
#define AD779x_MODE_REG   	0x1			//模式寄存器，16bits 												MR15-MR13:Mode Select,MR12:PSW,MR11-MR4:0,MR3-MR0:Filter Update Rate Select
#define AD779x_CONFIG_REG 	0x2			//配置寄存器，16bits  CON15-CON14:0,CON12:U/B,CON11:0,CON10-CON8:Gain,CON7-CON6:0,CON5:REF_DET,CON4:BUF,CON3:0,CON2-CON0:Channel Select
#define AD779x_DATA_REG   	0x3			//数据寄存器，			 Reset:0x0000(AD7798)/0x000000(AD7799)
#define AD779x_ID_REG     	0x4			//ID寄存器，	8bits，Reset:0xx8(AD7798)/0xx9(AD7799)
#define AD779x_IO_REG     	0x5			//IO寄存器，	8bits                                                IO7:0,IO6:IOEN,IO5:IO2DAT,IO4:IO1DAT,IO3-IO0:0
#define AD779x_OFFSET_REG 	0x6			//Offset寄存器，		 Reset:0x8000(AD7798)/0x800000(AD7799)
#define AD779x_SCALE_REG  	0x7			//Full_Scale寄存器， Reset:0x5xxx(AD7798)/0x5xxx00(AD7799)

enum {  AD779x_CONTINUOUS_CONVERSION_MODE,  
        AD779x_SINGLE_CONVERSION_MODE,
        AD779x_IDLE_MODE, 
        AD779x_POWERDOWN_MODE, 
        AD779x_INTERNAL_OFFSET_CAL_MODE,
        AD779x_INTERNAL_SCALE_CAL_MODE,  
        AD779x_SYSTEM_OFFSET_CAL_MODE,
        AD779x_SYSTEM_SCALE_CAL_MODE
      };

enum {  AD779x_470_HZ = 1, 
        AD779x_242_HZ, 
        AD779x_123_HZ, 
        AD779x_62_HZ,
        AD779x_50_HZ, 
        AD779x_39_HZ, 
        AD779x_33_2_HZ, 
        AD779x_19_6_HZ,
        AD779x_16_7_1_HZ, 
        AD779x_16_7_2_HZ, 
        AD779x_12_5_HZ, 
        AD779x_10_HZ,
        AD779x_8_33_HZ, 
        AD779x_6_25_HZ, 
        AD779x_4_17_HZ 
     };
enum {  AD779x_1_GAIN, 
        AD779x_2_GAIN, 
        AD779x_4_GAIN, 
        AD779x_8_GAIN,
        AD779x_16_GAIN, 
        AD779x_32_GAIN, 
        AD779x_64_GAIN, 
        AD779x_128_GAIN 
      };
enum {  AD779x_AIN1_CHAN, 
        AD779x_AIN2_CHAN, 
        AD779x_AIN3_CHAN,
        AD779x_AIN11_CHAN
      };
unsigned char AD779x_Init(void);
int AD779x_ReadData(char channel);
//unsigned char AD779x_ContinuousReadData(char channel,unsigned int Size);			
			
unsigned int AD779x_Calibrate(void);
void AD779x_RequestData(unsigned char continuous);
unsigned char AD779x_DataReady(void);
void AD779x_WriteConfig(unsigned char burnout, unsigned char UB, unsigned char gain,unsigned char ref_det, unsigned char buf, unsigned char channel);
void AD779x_SetMode(unsigned char mode, unsigned char psw, unsigned char rate);
unsigned int AD779x_ReadScale(void);
unsigned int AD779x_ReadOffset(void);
unsigned char AD779x_StatusRegisterRead(void);
static void AD779x_Comm(unsigned char reg, unsigned char rw, unsigned char cread);
static void AD779x_Reset(void);
float AD779x_ContinuousReadData(char channel,unsigned int Size); 
	  
void AD779x_Set(char channel);	  
void AD779x_ContinuousReadData1(unsigned short Average_num);	  
void AD779x_ContinuousReadDataN(char channel,unsigned int Size);

enum{
	WRITE = 0,
	READ = 1
};

extern unsigned char AD779x_Sampling_Complete_Flag0;
extern unsigned char AD779x_Sampling_Complete_Flag1;





void SPI1_Configuration(void);
unsigned char SPI1_ReadWriteByte(unsigned char byte);
unsigned char ADS124x_ReadREG(unsigned char Reg_addr);
void ADS124x_WriteReg(unsigned char Reg_addr,unsigned char data);
void ADS124x_WriteCommand(unsigned char data);
unsigned char SPI1_CS_Select(unsigned char CS);



#endif


