/**
  ******************************************************************************
  * @file    main.c 
  * @author  翟海青
  * @version V4.0.0
  * @date    10-9-2018
  * @brief   Main program body
  ******************************************************************************
  */

#include "main.h"

static  CPU_BOOLEAN     AppSw;

static	OS_TCB	AppTaskStartTCB;
__align(8) static  CPU_STK  AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE];
static  void  AppTaskStart          (void     *p_arg);


OS_TMR	tmr1;	//创建一个软件定时器，连续模式，初始延时5ms，定时周期100ms


/*****************************************************************
*
*	关于优先级的说明：
*	系统保留有5个任务，这五个任务占了5个优先级，用户不得使用这5个优先级
*	优先级0：中断服务管理任务：OS_IntQTask();
*	优先级1：时钟节拍任务：OS_TickTask();
*	优先级2：定时任务：OS_TmrTask();
*	优先级OS_CFG_PRIO_MAX-2(倒数第二个优先级)：统计任务：OS_StatTask();
*	优先级OS_CFG_PRIO_MAX-1(倒数第一个优先级)：空闲任务：OS_IdleTask();
*
******************************************************************/

#define	Instruction_TASK_PRIO	3			//任务优先级
#define Instruction_STK_SIZE	512			//任务堆栈空间
OS_TCB	InstructionTaskTCB;					//任务控制块
__align(8) static CPU_STK	Instruction_TASK_STK[Instruction_STK_SIZE];	//任务堆栈
void	Instruction_Task(void *p_arg);			//任务函数

#define	CLCWDT_TASK_PRIO	4			//任务优先级
#define CLCWDT_STK_SIZE		64			//任务堆栈空间
OS_TCB	CLCWDTTaskTCB;					//任务控制块
__align(8) static CPU_STK	CLCWDT_TASK_STK[CLCWDT_STK_SIZE];	//任务堆栈
void	CLCWDT_Task(void *p_arg);			//任务函数

#define	PAS_TASK_PRIO	5			//任务优先级
#define PAS_STK_SIZE	256			//任务堆栈空间,注意空间大小，256个字=256X4=1024字节
OS_TCB	PASTaskTCB;					//任务控制块
__align(8) static CPU_STK	PAS_TASK_STK[PAS_STK_SIZE];	//任务堆栈
void	PAS_Task(void *p_arg);			//任务函数

#define	TEMP_TASK_PRIO	6			//任务优先级
#define TEMP_STK_SIZE	128			//任务堆栈空间
OS_TCB	TEMPTaskTCB;					//任务控制块
__align(8) static CPU_STK	TEMP_TASK_STK[TEMP_STK_SIZE];	//任务堆栈
void	TEMP_Task(void *p_arg);			//任务函数

#define	MAINPOWER_TASK_PRIO	7			//任务优先级
#define MAINPOWER_STK_SIZE	128			//任务堆栈空间
OS_TCB	MAINPOWERTaskTCB;					//任务控制块
__align(8) static CPU_STK	MAINPOWER_TASK_STK[MAINPOWER_STK_SIZE];//任务堆栈
void	MAINPOWER_Task(void *p_arg);			//任务函数



#ifdef debug
#define	LOGINFO_TASK_PRIO	8			//任务优先级
#define LOGINFO_STK_SIZE	128			//任务堆栈空间
OS_TCB	LOGINFOTaskTCB;					//任务控制块
__align(8) static CPU_STK	LOGINFO_TASK_STK[LOGINFO_STK_SIZE];	//任务堆栈
void	LOGINFO_Task(void *p_arg);			//任务函数
#endif

#define	LED0_TASK_PRIO		9			//任务优先级
#define LED0_STK_SIZE		64			//任务堆栈空间
OS_TCB	LED0TaskTCB;					//任务控制块
__align(8) static CPU_STK	LEDO_TASK_STK[LED0_STK_SIZE];	//任务堆栈
void	LED0_Task(void *p_arg);			//任务函数

#define	LED1_TASK_PRIO		10			//任务优先级
#define LED1_STK_SIZE		64			//任务堆栈空间
OS_TCB	LED1TaskTCB;					//任务控制块
__align(8) static CPU_STK	LED1_TASK_STK[LED1_STK_SIZE];	//任务堆栈
void	LED1_Task(void *p_arg);			//任务函数



//__align(8) 

float MKx=0,MKy=0,MKz=0;										//定义磁力计椭圆修正系数

volatile unsigned int SysTick_Count = 0;   //Systick计数
volatile unsigned int SysTick_Count1 = 0;  //Systick计数
volatile unsigned int TimingDelay = 0;     //延时函数计数

volatile unsigned int   WorkTime = 0;			//工作时间计时，最大900S，区别输入寄存器中的系统工作时间
volatile unsigned char  WorkTimeOutFlag = 0;	//工作时间溢出标志位，超过900S未收到指令，置1该位，同时不再喂狗，等待复位
volatile unsigned char  Temp_Flag=1;            //温度采样标志位，3s采一次
volatile unsigned char  MainPower_Flag=1;       //主电源电压采样标志位，3s采一次
volatile unsigned char  MMA8451_Flag=1;       	//主电源电压采样标志位，3s采一次
volatile unsigned int   Time_Count=0;			//采样计数器

volatile IMU_Data_type IMU_Data;

/*******************************************************************************
* Function Name  : Init_Parameter function
* Description    : 完成相应的参数初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_Parameter(void)
{
	uint8_t temp[2]={0},i=0;
	
	//初始化保持寄存器参数
	//EEErase(FPOWERONFLAG_BASEADDR,2);
	EERead(FPOWERONFLAG_BASEADDR,temp,2);//执行首次开机标志位判断，确认是否是首次开机
	if( (temp[0] != 0x12) && (temp[1] != 0x34) )
	{
		//是首次开机
		//log1_info("Device powers on for the first time,and Init Default Parameter. . .\r\n");
		//EERead(KREEPROM_BASEADDR,(void *)&KeepRegister.DeviceAddress,20);//读取EEPROM中存储的保持寄存器值，恢复以下几项
		
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		
		//初始化设备相关参数
		KeepRegister.DeviceAddress              		= DefaultDeviceADDR;
		KeepRegister.DeviceGroupNum 					= 0;
		KeepRegister.OriginaLiquidAltitude_After      	= 0;
		KeepRegister.OriginaLiquidAltitude_Befor      	= 0;
		KeepRegister.Sensor_Range 						= DefaultSensor_Range;
		KeepRegister.Liquid_Sec                			= (Liquid_FD01_25<<8) | 0x00;
		KeepRegister.LocalAccelerationOfGravity 		= DefaultAccelerationOfGravity;
		KeepRegister.Sensor_FS_Val						= 0;
		
		
		EEWrite(KREEPROM_BASEADDR,(void *)&KeepRegister.DeviceAddress,24);//更新EEPROM
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		
		
		//初始化线性修正相关参数
		for(i=0;i<6;i++) KeepRegister.MV[i]=i*10;
		KeepRegister.Temp_T0 = 20;
		EEWrite(KREEPROM_BASEADDR+24,(void *)&KeepRegister.MV[0],28);
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		
		
		//将温漂修正系数全部清零
		for(i=0;i<5;i++) KeepRegister.LTC0[i]=0;
		for(i=0;i<5;i++) KeepRegister.LTC1[i]=0;
		for(i=0;i<5;i++) KeepRegister.LTC2[i]=0;
		for(i=0;i<5;i++) KeepRegister.LTC3[i]=0;
		for(i=0;i<5;i++) KeepRegister.LTC4[i]=0;
		for(i=0;i<5;i++) KeepRegister.LTC5[i]=0;
		EEWrite(KREEPROM_BASEADDR+52,(void *)&KeepRegister.LTC0[0],120);
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		
		
		//写设备参数
		KeepRegister.Average_num	=	3;
		KeepRegister.bps			=	3;
		EEWrite(KREEPROM_BASEADDR+172,(void *)&KeepRegister.Average_num,4);//保存数据
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		
		//写操作模式
		SuperMode_Flag=1;										//首次开机设置模式为超级模式
		InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0x00ff)|0x0100;
		temp[0] = 0x12;
		temp[1] = 0x34;
		EEWrite(FPOWERONFLAG_BASEADDR,temp,2);
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		
		//写首次开机标志位
		temp[0] = 0x34;
		temp[1] = 0x12;
		EEWrite(SUPERMODE_FLAG_BASEADDR,temp,2);
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
	}
	
	//读取工作模式
	temp[0]=0;
	temp[1]=0;
	EERead(SUPERMODE_FLAG_BASEADDR,temp,2);
	if((temp[0] == 0x34) && (temp[1] == 0x12) )
	{
		SuperMode_Flag=1;		//根据标志位进行模式判断
		InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0x00ff)|0x0100;
	}
	else 
	{
		SuperMode_Flag=0;
		InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0x00ff)|0x0200;
	}
	
	//log1_info("time1:%ds\r\n",KeepRegister.Average_num);
	
	EERead(KREEPROM_BASEADDR,(void *)&KeepRegister,sizeof(KeepRegister));
	
	//log1_info("time2:%ds\r\n",KeepRegister.Average_num);
	
	if((KeepRegister.Average_num<0)||(KeepRegister.Average_num>256))KeepRegister.Average_num=3;
	if((KeepRegister.DeviceAddress<1)||(KeepRegister.DeviceAddress>247))KeepRegister.DeviceAddress=1;
	if((KeepRegister.bps<1)||(KeepRegister.bps>7))KeepRegister.bps=3;
	InputRegister.DeviceType      = DEVICETYPE;
	InputRegister.SoftwareVersion = SOFTWAREVERSION;
	
	GPIO_PinReverse(GPIOA,GPIO_Pin_1);
	Get_SNInfo_Fun();
	
	//log1_info("Device Parameter has Init!,time:%ds\r\n",KeepRegister.Average_num);
}

int main(void)
{
    OS_ERR  err;
	CPU_SR_ALLOC();												//向CPU申请一个变量
	
	SCB->VTOR = FLASH_BASE | 0x8000;      						//设置向量表偏移值，Bootloader起始地址为0x0800 0000，共32KB空间,APP程序起始地址为0x0800 8000，
		
    BSP_IntDisAll();                                            /* Disable all interrupts.                              */
    
    OSInit(&err);                                               /* Init uC/OS-III.必须要先初始化                        */
	OS_CRITICAL_ENTER();//进入临界区
																/* Create the start task  必须创建                      */
	OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,             	//任务控制块
                 (CPU_CHAR     *)"App Task Start",				//任务名字
                 (OS_TASK_PTR   )AppTaskStart, 					//任务函数
                 (void         *)0,								//传递给任务的参数
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,		//任务优先级
                 (CPU_STK      *)&AppTaskStartStk[0],			//任务堆栈基地址
                 (CPU_STK_SIZE  )AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10],		//任务堆栈深度限位
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,							//任务堆栈大小
                 (OS_MSG_QTY    )0,														//任务内部消息队列能够接收的最大消息数目，0表示禁止接收
                 (OS_TICK       )0,														//当使能时间片轮转时的时间片长度，为0时为默认长度，
                 (void         *)0,														//用户补充的存储区
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),			//任务选项
                 (OS_ERR       *)&err);													//存放该函数错误时的返回值

	OS_CRITICAL_EXIT();	//退出临界区
    OSStart(&err);		//开始UCOSIII,将CPU控制权移交给UCOS                        		/* Start multitasking (i.e. give control to uC/OS-III). */
    
    (void)&err;
    
    return (0);
}

/**********************************************
*		任务开始函数，该函数是UCOS必须创建的
*
*
*
**********************************************/
static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
	
	CPU_SR_ALLOC();												/*           定义一个CPU_SR变量         */
   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */	//时钟设置
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */
    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts         = cpu_clk_freq                                 /* Determine nbr SysTick increments                     */
                 / (CPU_INT32U)OSCfg_TickRate_Hz;  

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    //Mem_Init();                                               /* Initialize memory managment module                   */
    //Math_Init();                                              /* Initialize mathematical module                       */
	
/**			以下是操作系统功能剪裁		**/
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);               /* Compute CPU capacity with no task running  统计任务          */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN					//如果使能了测量中断关闭时间
    CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  				//当使用时间片轮转的时候
												//使能时间片轮转调度功能,时间片长度为1个系统时钟节拍，既1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
    
#if (APP_CFG_SERIAL_EN == DEF_ENABLED)    
    App_SerialInit();                                           /* Initialize Serial communication for application ...  */
#endif
    
		/**			外设初始化			**/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);//中断优先级分组
	GPIO_Configuration();
	
	Init_Parameter();
	
	UART1_Configuration(KeepRegister.bps);
	SPI1_Configuration();
	AD779x_Init();
	
	
	
	log1_info("cpu_clk_freq:%dHz\r\n",cpu_clk_freq);
	log1_info("NVIC_PriorityGroup_3\r\n");
	log1_info("GPIO has Init!\r\n");
	log1_info("UART has Init:%dbps!\r\n",COM_UART_bps);
	log1_info("Hardware has Init!\r\n");
	
	
	OSTmrCreate((OS_TMR*  	)&tmr1,		//创建软件定时器tmr1
				(CPU_CHAR*	)"tmr1",
				(OS_TICK	) 0,		//初始延时5ms
				(OS_TICK	) 10 ,		//定时周期ms，定时器时钟节拍*次数,当前为10ms
				(OS_OPT		) OS_OPT_TMR_PERIODIC,//周期模式
				(OS_TMR_CALLBACK_PTR)tmr1_callback,
				(void *		)0,
				(OS_ERR*	)&err);
	
    /* Create Application tasks                   */
	OS_CRITICAL_ENTER();	//进入临界区
			 
	OSTaskCreate((OS_TCB 	* )&CLCWDTTaskTCB,		//压力传感器采样任务
			 (CPU_CHAR	* )"CLCWDT Task", 		
			 (OS_TASK_PTR )CLCWDT_Task, 			
			 (void		* )0,					
			 (OS_PRIO	  )CLCWDT_TASK_PRIO,     
			 (CPU_STK   * )&CLCWDT_TASK_STK[0],	
			 (CPU_STK_SIZE)CLCWDT_STK_SIZE/10,	
			 (CPU_STK_SIZE)CLCWDT_STK_SIZE,		
			 (OS_MSG_QTY  )0,					
			 (OS_TICK	  )0,					
			 (void   	* )0,					
			 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
			 (OS_ERR 	* )&err);
			 
	OSTaskCreate((OS_TCB 	* )&InstructionTaskTCB,		//压力传感器采样任务
			 (CPU_CHAR	* )"Instruction Task", 		
			 (OS_TASK_PTR )Instruction_Task, 			
			 (void		* )0,					
			 (OS_PRIO	  )Instruction_TASK_PRIO,     
			 (CPU_STK   * )&Instruction_TASK_STK[0],	
			 (CPU_STK_SIZE)Instruction_STK_SIZE/10,	
			 (CPU_STK_SIZE)Instruction_STK_SIZE,		
			 (OS_MSG_QTY  )0,					
			 (OS_TICK	  )0,					
			 (void   	* )0,					
			 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
			 (OS_ERR 	* )&err);
	
	OSTaskCreate((OS_TCB 	* )&PASTaskTCB,		//压力传感器采样任务
			 (CPU_CHAR	* )"PAS Task", 		
			 (OS_TASK_PTR )PAS_Task, 			
			 (void		* )0,					
			 (OS_PRIO	  )PAS_TASK_PRIO,     
			 (CPU_STK   * )&PAS_TASK_STK[0],	
			 (CPU_STK_SIZE)PAS_STK_SIZE/10,	
			 (CPU_STK_SIZE)PAS_STK_SIZE,		
			 (OS_MSG_QTY  )0,					
			 (OS_TICK	  )0,					
			 (void   	* )0,					
			 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
			 (OS_ERR 	* )&err);
	
	OSTaskCreate((OS_TCB 	* )&MAINPOWERTaskTCB,		//主电源电压采样任务
			 (CPU_CHAR	* )"MAINPOWER Task", 		
			 (OS_TASK_PTR )MAINPOWER_Task, 			
			 (void		* )0,					
			 (OS_PRIO	  )MAINPOWER_TASK_PRIO,     
			 (CPU_STK   * )&MAINPOWER_TASK_STK[0],	
			 (CPU_STK_SIZE)MAINPOWER_STK_SIZE/10,	
			 (CPU_STK_SIZE)MAINPOWER_STK_SIZE,		
			 (OS_MSG_QTY  )0,					
			 (OS_TICK	  )0,					
			 (void   	* )0,					
			 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
			 (OS_ERR 	* )&err);	
	
	OSTaskCreate((OS_TCB 	* )&TEMPTaskTCB,		//温度传感器采样任务
			 (CPU_CHAR	* )"TEMP Task", 		
			 (OS_TASK_PTR )TEMP_Task, 			
			 (void		* )0,					
			 (OS_PRIO	  )TEMP_TASK_PRIO,     
			 (CPU_STK   * )&TEMP_TASK_STK[0],	
			 (CPU_STK_SIZE)TEMP_STK_SIZE/10,	
			 (CPU_STK_SIZE)TEMP_STK_SIZE,		
			 (OS_MSG_QTY  )0,					
			 (OS_TICK	  )0,					
			 (void   	* )0,					
			 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
			 (OS_ERR 	* )&err);	
#ifdef debug	
	OSTaskCreate((OS_TCB 	* )&LOGINFOTaskTCB,		//参数打印任务
			 (CPU_CHAR	* )"LOGINFO Task", 		
			 (OS_TASK_PTR )LOGINFO_Task, 			
			 (void		* )0,					
			 (OS_PRIO	  )LOGINFO_TASK_PRIO,     
			 (CPU_STK   * )&LOGINFO_TASK_STK[0],	
			 (CPU_STK_SIZE)LOGINFO_STK_SIZE/10,	
			 (CPU_STK_SIZE)LOGINFO_STK_SIZE,		
			 (OS_MSG_QTY  )0,					
			 (OS_TICK	  )0,					
			 (void   	* )0,					
			 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
			 (OS_ERR 	* )&err);
#endif			 
	
	OSTaskCreate((OS_TCB 	* )&LED0TaskTCB,		//LED0任务
			 (CPU_CHAR	* )"LED0 Task", 		
			 (OS_TASK_PTR )LED0_Task, 			
			 (void		* )0,					
			 (OS_PRIO	  )LED0_TASK_PRIO,     
			 (CPU_STK   * )&LEDO_TASK_STK[0],	
			 (CPU_STK_SIZE)LED0_STK_SIZE/10,	
			 (CPU_STK_SIZE)LED0_STK_SIZE,		
			 (OS_MSG_QTY  )0,					
			 (OS_TICK	  )0,					
			 (void   	* )0,					
			 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
			 (OS_ERR 	* )&err);
			 
	OSTaskCreate((OS_TCB 	* )&LED1TaskTCB,	   //LED1任务
			 (CPU_CHAR	* )"LED1 Task", 		
			 (OS_TASK_PTR )LED1_Task, 			
			 (void		* )0,					
			 (OS_PRIO	  )LED1_TASK_PRIO,     
			 (CPU_STK   * )&LED1_TASK_STK[0],	
			 (CPU_STK_SIZE)LED1_STK_SIZE/10,	
			 (CPU_STK_SIZE)LED1_STK_SIZE,		
			 (OS_MSG_QTY  )0,					
			 (OS_TICK	  )0,					
			 (void   	* )0,					
			 (OS_OPT      )OS_OPT_TASK_STK_CHK|OS_OPT_TASK_STK_CLR,
			 (OS_ERR 	* )&err);
	
	//log1_info("LED0_Task has Create，prio:%d!\r\n",LED0_TASK_PRIO);
	//log1_info("LED1_Task has Create，prio:%d!\r\n",LED1_TASK_PRIO);
	//log1_info("LOGINFO_Task has Create，prio:%d!\r\n",LOGINFO_TASK_PRIO);
	//log1_info("TEMP_Task has Create，prio:%d!\r\n",TEMP_TASK_PRIO);
	//log1_info("MAINPOWER_Task has Create，prio:%d!\r\n",MAINPOWER_TASK_PRIO);
	log1_info("OS has Init!\r\n");
		 
	//OS_TaskSuspend((OS_TCB*)&AppTaskStartTCB,&err);		//挂起开始任务	
	OS_CRITICAL_EXIT();	//临界区
	while(DEF_TRUE)
	{
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1000ms
		InputRegister.SystemWorkTime++;//系统工作时间++
	}
	
}



void Instruction_Task(void *p_arg)
{
	OS_ERR err;
	unsigned char ix,RX_Len;
	unsigned short crc;
	p_arg = p_arg;
	RS485_RX();
	while(1)
	{
//		if(ModbusIntervalTime) ModbusIntervalTime--;
//		else
//		{
//			if(ModbusDataPackage.DataLen && !ModbusDataPackage.DataFlag)
//			ModbusDataPackage.DataFlag = 1;
//			Modbus_FreeFlag=0;
//		}
		if(ModbusDataPackage.DataFlag)		  //数据包已接收完成
		{ 	
			for(ix=0;ix<ModbusDataPackage.DataLen;ix++) ModbusDataPackage.dat[ix] =USART1_GetChar();//将串口数据放到指定buf
			RX_Len=ModbusDataPackage.DataLen;
			
			USART1_ClearBuf_Flag();				 //清空串口接收缓存
			ModbusDataPackage.DataLen = 0;  //先清空长度，注意清空顺序
			ModbusDataPackage.DataFlag = 0; //清空标记位
			
			crc = CRC16_Check((uint8_t *)ModbusDataPackage.dat,RX_Len-2 );
			
			if( (( crc == ( (ModbusDataPackage.dat[RX_Len - 1] << 8) |    ModbusDataPackage.dat[RX_Len - 2])        )) \
			||  ( ( (ModbusDataPackage.dat[RX_Len - 1]) == 0xff    ) && ((ModbusDataPackage.dat[RX_Len - 2]) == 0xff))  )				
			{
				 Instruction_Process_Subfunction(RX_Len);
			}
		}
		OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err); //延时1ms
	}
}

/*******************************************************************************
* Function Name  : Temp_ReadTASK
* Description    : 温度测量任务
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
float Old_Temp=0.0;
unsigned int Temp_Count=0;
unsigned char Temp_Err_Count=0;
void TEMP_Task(void *p_arg)
{
	OS_ERR err;
	unsigned char i=0;
	float temp[5]={0},buff=0;
	float ADT7301_Temp,DS18B20_Temp,ABStemp;
	
	//CPU_SR_ALLOC();
	p_arg = p_arg;
	
	GPIO_Configuration();
	
	OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
	OSSchedLock(&err);//关闭任务调度
	DS18B20_Temp=DS18B20_TEMP();
	OSSchedUnlock(&err);//恢复调度

	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
	OSSchedLock(&err);//关闭任务调度
	DS18B20_Temp=DS18B20_TEMP();
	OSSchedUnlock(&err);//恢复调度 	
	Old_Temp=DS18B20_Temp;
	
	while(1)
	{
		InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x21;	
		
		OSSchedLock(&err);//关闭任务调度
		DS18B20_Temp=DS18B20_TEMP();
		//log1_info("DS18B20_temp1:%.3f℃\r\n",DS18B20_Temp);
		OSSchedUnlock(&err);//恢复调度  
		
		ABStemp = (DS18B20_Temp>Old_Temp)?(DS18B20_Temp-Old_Temp):(Old_Temp-DS18B20_Temp);
		if( (DS18B20_Temp>=-40) && (DS18B20_Temp<=85) )
		{
			if( (ABStemp<3) || (Temp_Count<3) )
			{
				Temp_Count++;
				InputRegister.Temperature=DS18B20_Temp;
				Old_Temp=DS18B20_Temp;
				//log1_info("DS18B20_temp2:%.3f℃\r\n",DS18B20_Temp);
			}
			else
			{
				Temp_Err_Count++;
				if(Temp_Err_Count>5)
				{
					Temp_Err_Count=0;
					Temp_Count=0;
				}
				
			}
			OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时1000ms，每各1S进行依次温度采样
		}
		else
		{
			OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时300ms	
		}
	}	
	
}

/*******************************************************************************
* Function Name  : MainPower_ReadTASK
* Description    : 主电源电压监测任务
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAINPOWER_Task(void *p_arg)
{
	OS_ERR err;
	
	p_arg = p_arg;
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时2000ms
	while(1)
	{
		MainPower_Flag=0;
		InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x22;
		IADC_Configuration();
		//OSTimeDlyHMSM(0,0,0,2,OS_OPT_TIME_HMSM_STRICT,&err);
		InputRegister.MainPower_V=Get_Adc_Average(5);
		//OSTimeDlyHMSM(0,0,0,2,OS_OPT_TIME_HMSM_STRICT,&err);
		
		//关闭时钟
		RCC_HSICmd(DISABLE);  // Enable The HSI (16Mhz)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);	//Enable ADC1 clock
		
		//log1_info("MainPower:%.3fV\r\n",InputRegister.MainPower_V);
		
		OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时2000ms
		
	}
}

/*******************************************************************************
* Function Name  : PAS_TASK
* Description    : 液位高度信号测量函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PAS_Task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	
	
	OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); //延时2000ms
	OSTmrStart(&tmr1,&err);			//开启软件定时器tmr1
	while(1)
	{
		AD779x_ContinuousReadData1(KeepRegister.Average_num);//
		
		if(AD779x_Sampling_Complete_Flag1)
		{
			AD779x_Sampling_Complete_Flag1=0;
			Level_height_conversion(InputRegister.ADCOriginalValue,InputRegister.Temperature);
		
			//log1_info("Average_num:%d,mV:%fmV\r\n",KeepRegister.Average_num,InputRegister.ADCOriginalValue);
	
			//log_info("Befor_AbsoluteValue:%fmm,Befor_DiffAltitude:%fmm\r\n",InputRegister.LiquidAltitudeAbsoluteValue_Befor,InputRegister.AltitudeDifference_Befor);
			//log_info("After_AbsoluteValue:%fmm,After_DiffAltitude:%fmm\r\n",InputRegister.LiquidAltitudeAbsoluteValue_After,InputRegister.AltitudeDifference_After);
		}
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //延时
	}

}

void CLCWDT_Task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,750,OS_OPT_TIME_HMSM_STRICT,&err); //延时1000ms
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		
		
		
	}
}

void LED0_Task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	
	while(1)
	{
		LED0_ON();
		OSTimeDlyHMSM(0,0,0,40,OS_OPT_TIME_HMSM_STRICT,&err); //延时40ms
		LED0_OFF();
		OSTimeDlyHMSM(0,0,1,388,OS_OPT_TIME_HMSM_STRICT,&err); //延时1388ms，每分钟42次
	}
}

void LED1_Task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	
	while(1)
	{
		//LED1_ON();
		//OSTimeDlyHMSM(0,0,0,333,OS_OPT_TIME_HMSM_STRICT,&err); //延时200ms
		LED1_OFF();
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //延时500ms
	}
}

#ifdef debug
void LOGINFO_Task(void *p_arg)
{
	OS_ERR err;
	unsigned char i=0;
	
	p_arg = p_arg;
	
	log_info("KeepRegister.DeviceAddress:0x%x\r\n",KeepRegister.DeviceAddress);
	log_info("KeepRegister.DeviceGroupNum:0x%x\r\n",KeepRegister.DeviceGroupNum);
	log_info("KeepRegister.OriginaLiquidAltitude_After:%f\r\n",KeepRegister.OriginaLiquidAltitude_After);
	log_info("KeepRegister.OriginaLiquidAltitude_Befor:%f\r\n",KeepRegister.OriginaLiquidAltitude_Befor);
	log_info("KeepRegister.Sensor_Range:%dKPa\r\n",KeepRegister.Sensor_Range);
	log_info("KeepRegister.Liquid_Sec:0x%x\r\n",KeepRegister.Liquid_Sec);
	log_info("KeepRegister.LocalAccelerationOfGravity:%f\r\n",KeepRegister.LocalAccelerationOfGravity);
	log_info("KeepRegister.Sensor_FS_Val:%f\r\n",KeepRegister.Sensor_FS_Val);
	for(i=0;i<6;i++)log_info("KeepRegister.MV[%d]:%f\r\n",i,KeepRegister.MV[i]);
	log_info("KeepRegister.Temp_T0:%f\r\n",KeepRegister.Temp_T0);
	for(i=0;i<5;i++)log_info("KeepRegister.LTC0[%d]:%f\r\n",i,KeepRegister.LTC0[i]);
	for(i=0;i<5;i++)log_info("KeepRegister.LTC1[%d]:%f\r\n",i,KeepRegister.LTC1[i]);
	for(i=0;i<5;i++)log_info("KeepRegister.LTC2[%d]:%f\r\n",i,KeepRegister.LTC2[i]);
	for(i=0;i<5;i++)log_info("KeepRegister.LTC3[%d]:%f\r\n",i,KeepRegister.LTC3[i]);
	for(i=0;i<5;i++)log_info("KeepRegister.LTC4[%d]:%f\r\n",i,KeepRegister.LTC4[i]);
	for(i=0;i<5;i++)log_info("KeepRegister.LTC5[%d]:%f\r\n",i,KeepRegister.LTC5[i]);
	
	log_info("InputRegister.DeviceType:0x%x\r\n",InputRegister.DeviceType);
	log_info("InputRegister.SoftwareVersion:0x%x\r\n",InputRegister.SoftwareVersion);
	log_info("InputRegister.SystemWorkTime:0x%x\r\n",InputRegister.SystemWorkTime);
	log_info("InputRegister.SystemWorkStatus:0x%x\r\n",InputRegister.SystemWorkStatus);
	
	log_info("InputRegister.AltitudeDifference_After:%f\r\n",InputRegister.AltitudeDifference_After);
	log_info("InputRegister.Temperature:%f\r\n",InputRegister.Temperature);
	log_info("InputRegister.LiquidAltitudeAbsoluteValue_After:%f\r\n",InputRegister.LiquidAltitudeAbsoluteValue_After);
	log_info("InputRegister.ADCOriginalValue:%f\r\n",InputRegister.ADCOriginalValue);
	log_info("InputRegister.PA:%f\r\n",InputRegister.PA);
	log_info("InputRegister.AltitudeDifference_Befor:%f\r\n",InputRegister.AltitudeDifference_Befor);
	log_info("InputRegister.LiquidAltitudeAbsoluteValue_Befor:%f\r\n",InputRegister.LiquidAltitudeAbsoluteValue_Befor);
	log_info("InputRegister.MainPower_V:%f\r\n",InputRegister.MainPower_V);
	
	OS_TaskSuspend((OS_TCB*)&LOGINFOTaskTCB,&err);		//挂起该任务
	
	//return ;
}
#endif


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
