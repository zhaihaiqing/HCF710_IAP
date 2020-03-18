#ifndef __Temp_H
#define __Temp_H


//DS18B20数据线   PB12
#define DQ_OUT_H					GPIOB->BSRRL = GPIO_Pin_12
#define DQ_OUT_L					GPIOB->BSRRH = GPIO_Pin_12
#define DQ_Read()					(GPIOB->IDR & GPIO_Pin_12)

//设置DS18B20数据线方向
#define SET_DS18B20_DQ_IN     GPIOB->MODER   &= 0xfcffffff;\
							  GPIOB->PUPDR   &= 0xfcffffff;GPIOB->PUPDR   |= 0x01000000
															//设置输入功能
															//设置上拉
#define SET_DS18B20_DQ_OUT    GPIOB->MODER   &= 0xfcffffff;GPIOB->MODER   |= 0x01000000;\
							  GPIOB->OTYPER  &= 0xffffefff;GPIOB->OTYPER  |= 0x00001000;\
							  GPIOB->OSPEEDR &= 0xfcffffff;GPIOB->OSPEEDR |= 0x03000000;\
							  GPIOB->PUPDR   &= 0xfcffffff;GPIOB->PUPDR   |= 0x01000000
							//设置为输出功能
							//设置输出类型为开漏
							//设置输出速度为40MHz
							//设置上拉

extern volatile unsigned char IS_UART_RX_IN_DS18B20;


float ADT7301_ReadTemp(void);


void DS18B20_delayus(unsigned int us);
unsigned char DS18B20_Rst(void);
unsigned char DS18B20_Read_Bit(void);
unsigned char DS18B20_Read_Byte(void);															
void DS18B20_Write_Byte(unsigned char dat);															
void DS18B20_ReadROM(void);														
float DS18B20_TEMP(void);
void DS18B20_SendConV_Command(void);
float DS18B20_get_TEMP(void);


#endif

