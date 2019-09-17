/**
  ******************************************************************************
  * @file    main.c 
  * @author  �Ժ���
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


OS_TMR	tmr1;	//����һ�������ʱ��������ģʽ����ʼ��ʱ5ms����ʱ����100ms


/*****************************************************************
*
*	�������ȼ���˵����
*	ϵͳ������5���������������ռ��5�����ȼ����û�����ʹ����5�����ȼ�
*	���ȼ�0���жϷ����������OS_IntQTask();
*	���ȼ�1��ʱ�ӽ�������OS_TickTask();
*	���ȼ�2����ʱ����OS_TmrTask();
*	���ȼ�OS_CFG_PRIO_MAX-2(�����ڶ������ȼ�)��ͳ������OS_StatTask();
*	���ȼ�OS_CFG_PRIO_MAX-1(������һ�����ȼ�)����������OS_IdleTask();
*
******************************************************************/

#define	Instruction_TASK_PRIO	3			//�������ȼ�
#define Instruction_STK_SIZE	512			//�����ջ�ռ�
OS_TCB	InstructionTaskTCB;					//������ƿ�
__align(8) static CPU_STK	Instruction_TASK_STK[Instruction_STK_SIZE];	//�����ջ
void	Instruction_Task(void *p_arg);			//������

#define	CLCWDT_TASK_PRIO	4			//�������ȼ�
#define CLCWDT_STK_SIZE		64			//�����ջ�ռ�
OS_TCB	CLCWDTTaskTCB;					//������ƿ�
__align(8) static CPU_STK	CLCWDT_TASK_STK[CLCWDT_STK_SIZE];	//�����ջ
void	CLCWDT_Task(void *p_arg);			//������

#define	PAS_TASK_PRIO	5			//�������ȼ�
#define PAS_STK_SIZE	256			//�����ջ�ռ�,ע��ռ��С��256����=256X4=1024�ֽ�
OS_TCB	PASTaskTCB;					//������ƿ�
__align(8) static CPU_STK	PAS_TASK_STK[PAS_STK_SIZE];	//�����ջ
void	PAS_Task(void *p_arg);			//������

#define	TEMP_TASK_PRIO	6			//�������ȼ�
#define TEMP_STK_SIZE	128			//�����ջ�ռ�
OS_TCB	TEMPTaskTCB;					//������ƿ�
__align(8) static CPU_STK	TEMP_TASK_STK[TEMP_STK_SIZE];	//�����ջ
void	TEMP_Task(void *p_arg);			//������

#define	MAINPOWER_TASK_PRIO	7			//�������ȼ�
#define MAINPOWER_STK_SIZE	128			//�����ջ�ռ�
OS_TCB	MAINPOWERTaskTCB;					//������ƿ�
__align(8) static CPU_STK	MAINPOWER_TASK_STK[MAINPOWER_STK_SIZE];//�����ջ
void	MAINPOWER_Task(void *p_arg);			//������



#ifdef debug
#define	LOGINFO_TASK_PRIO	8			//�������ȼ�
#define LOGINFO_STK_SIZE	128			//�����ջ�ռ�
OS_TCB	LOGINFOTaskTCB;					//������ƿ�
__align(8) static CPU_STK	LOGINFO_TASK_STK[LOGINFO_STK_SIZE];	//�����ջ
void	LOGINFO_Task(void *p_arg);			//������
#endif

#define	LED0_TASK_PRIO		9			//�������ȼ�
#define LED0_STK_SIZE		64			//�����ջ�ռ�
OS_TCB	LED0TaskTCB;					//������ƿ�
__align(8) static CPU_STK	LEDO_TASK_STK[LED0_STK_SIZE];	//�����ջ
void	LED0_Task(void *p_arg);			//������

#define	LED1_TASK_PRIO		10			//�������ȼ�
#define LED1_STK_SIZE		64			//�����ջ�ռ�
OS_TCB	LED1TaskTCB;					//������ƿ�
__align(8) static CPU_STK	LED1_TASK_STK[LED1_STK_SIZE];	//�����ջ
void	LED1_Task(void *p_arg);			//������



//__align(8) 

float MKx=0,MKy=0,MKz=0;										//�����������Բ����ϵ��

volatile unsigned int SysTick_Count = 0;   //Systick����
volatile unsigned int SysTick_Count1 = 0;  //Systick����
volatile unsigned int TimingDelay = 0;     //��ʱ��������

volatile unsigned int   WorkTime = 0;			//����ʱ���ʱ�����900S����������Ĵ����е�ϵͳ����ʱ��
volatile unsigned char  WorkTimeOutFlag = 0;	//����ʱ�������־λ������900Sδ�յ�ָ���1��λ��ͬʱ����ι�����ȴ���λ
volatile unsigned char  Temp_Flag=1;            //�¶Ȳ�����־λ��3s��һ��
volatile unsigned char  MainPower_Flag=1;       //����Դ��ѹ������־λ��3s��һ��
volatile unsigned char  MMA8451_Flag=1;       	//����Դ��ѹ������־λ��3s��һ��
volatile unsigned int   Time_Count=0;			//����������

volatile IMU_Data_type IMU_Data;

/*******************************************************************************
* Function Name  : Init_Parameter function
* Description    : �����Ӧ�Ĳ�����ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Init_Parameter(void)
{
	uint8_t temp[2]={0},i=0;
	
	//��ʼ�����ּĴ�������
	//EEErase(FPOWERONFLAG_BASEADDR,2);
	EERead(FPOWERONFLAG_BASEADDR,temp,2);//ִ���״ο�����־λ�жϣ�ȷ���Ƿ����״ο���
	if( (temp[0] != 0x12) && (temp[1] != 0x34) )
	{
		//���״ο���
		//log1_info("Device powers on for the first time,and Init Default Parameter. . .\r\n");
		//EERead(KREEPROM_BASEADDR,(void *)&KeepRegister.DeviceAddress,20);//��ȡEEPROM�д洢�ı��ּĴ���ֵ���ָ����¼���
		
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		
		//��ʼ���豸��ز���
		KeepRegister.DeviceAddress              		= DefaultDeviceADDR;
		KeepRegister.DeviceGroupNum 					= 0;
		KeepRegister.OriginaLiquidAltitude_After      	= 0;
		KeepRegister.OriginaLiquidAltitude_Befor      	= 0;
		KeepRegister.Sensor_Range 						= DefaultSensor_Range;
		KeepRegister.Liquid_Sec                			= (Liquid_FD01_25<<8) | 0x00;
		KeepRegister.LocalAccelerationOfGravity 		= DefaultAccelerationOfGravity;
		KeepRegister.Sensor_FS_Val						= 0;
		
		
		EEWrite(KREEPROM_BASEADDR,(void *)&KeepRegister.DeviceAddress,24);//����EEPROM
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		
		
		//��ʼ������������ز���
		for(i=0;i<6;i++) KeepRegister.MV[i]=i*10;
		KeepRegister.Temp_T0 = 20;
		EEWrite(KREEPROM_BASEADDR+24,(void *)&KeepRegister.MV[0],28);
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		
		
		//����Ư����ϵ��ȫ������
		for(i=0;i<5;i++) KeepRegister.LTC0[i]=0;
		for(i=0;i<5;i++) KeepRegister.LTC1[i]=0;
		for(i=0;i<5;i++) KeepRegister.LTC2[i]=0;
		for(i=0;i<5;i++) KeepRegister.LTC3[i]=0;
		for(i=0;i<5;i++) KeepRegister.LTC4[i]=0;
		for(i=0;i<5;i++) KeepRegister.LTC5[i]=0;
		EEWrite(KREEPROM_BASEADDR+52,(void *)&KeepRegister.LTC0[0],120);
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		
		
		//д�豸����
		KeepRegister.Average_num	=	3;
		KeepRegister.bps			=	3;
		EEWrite(KREEPROM_BASEADDR+172,(void *)&KeepRegister.Average_num,4);//��������
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		
		//д����ģʽ
		SuperMode_Flag=1;										//�״ο�������ģʽΪ����ģʽ
		InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0x00ff)|0x0100;
		temp[0] = 0x12;
		temp[1] = 0x34;
		EEWrite(FPOWERONFLAG_BASEADDR,temp,2);
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
		
		//д�״ο�����־λ
		temp[0] = 0x34;
		temp[1] = 0x12;
		EEWrite(SUPERMODE_FLAG_BASEADDR,temp,2);
		GPIO_PinReverse(GPIOA,GPIO_Pin_1);
	}
	
	//��ȡ����ģʽ
	temp[0]=0;
	temp[1]=0;
	EERead(SUPERMODE_FLAG_BASEADDR,temp,2);
	if((temp[0] == 0x34) && (temp[1] == 0x12) )
	{
		SuperMode_Flag=1;		//���ݱ�־λ����ģʽ�ж�
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
	CPU_SR_ALLOC();												//��CPU����һ������
	
	SCB->VTOR = FLASH_BASE | 0x8000;      						//����������ƫ��ֵ��Bootloader��ʼ��ַΪ0x0800 0000����32KB�ռ�,APP������ʼ��ַΪ0x0800 8000��
		
    BSP_IntDisAll();                                            /* Disable all interrupts.                              */
    
    OSInit(&err);                                               /* Init uC/OS-III.����Ҫ�ȳ�ʼ��                        */
	OS_CRITICAL_ENTER();//�����ٽ���
																/* Create the start task  ���봴��                      */
	OSTaskCreate((OS_TCB       *)&AppTaskStartTCB,             	//������ƿ�
                 (CPU_CHAR     *)"App Task Start",				//��������
                 (OS_TASK_PTR   )AppTaskStart, 					//������
                 (void         *)0,								//���ݸ�����Ĳ���
                 (OS_PRIO       )APP_CFG_TASK_START_PRIO,		//�������ȼ�
                 (CPU_STK      *)&AppTaskStartStk[0],			//�����ջ����ַ
                 (CPU_STK_SIZE  )AppTaskStartStk[APP_CFG_TASK_START_STK_SIZE / 10],		//�����ջ�����λ
                 (CPU_STK_SIZE  )APP_CFG_TASK_START_STK_SIZE,							//�����ջ��С
                 (OS_MSG_QTY    )0,														//�����ڲ���Ϣ�����ܹ����յ������Ϣ��Ŀ��0��ʾ��ֹ����
                 (OS_TICK       )0,														//��ʹ��ʱ��Ƭ��תʱ��ʱ��Ƭ���ȣ�Ϊ0ʱΪĬ�ϳ��ȣ�
                 (void         *)0,														//�û�����Ĵ洢��
                 (OS_OPT        )(OS_OPT_TASK_STK_CHK | OS_OPT_TASK_STK_CLR),			//����ѡ��
                 (OS_ERR       *)&err);													//��Ÿú�������ʱ�ķ���ֵ

	OS_CRITICAL_EXIT();	//�˳��ٽ���
    OSStart(&err);		//��ʼUCOSIII,��CPU����Ȩ�ƽ���UCOS                        		/* Start multitasking (i.e. give control to uC/OS-III). */
    
    (void)&err;
    
    return (0);
}

/**********************************************
*		����ʼ�������ú�����UCOS���봴����
*
*
*
**********************************************/
static  void  AppTaskStart (void *p_arg)
{
    CPU_INT32U  cpu_clk_freq;
    CPU_INT32U  cnts;
    OS_ERR      err;
	
	CPU_SR_ALLOC();												/*           ����һ��CPU_SR����         */
   (void)p_arg;

    BSP_Init();                                                 /* Initialize BSP functions                             */	//ʱ������
    CPU_Init();                                                 /* Initialize the uC/CPU services                       */
    cpu_clk_freq = BSP_CPU_ClkFreq();                           /* Determine SysTick reference freq.                    */
    cnts         = cpu_clk_freq                                 /* Determine nbr SysTick increments                     */
                 / (CPU_INT32U)OSCfg_TickRate_Hz;  

    OS_CPU_SysTickInit(cnts);                                   /* Init uC/OS periodic time src (SysTick).              */

    //Mem_Init();                                               /* Initialize memory managment module                   */
    //Math_Init();                                              /* Initialize mathematical module                       */
	
/**			�����ǲ���ϵͳ���ܼ���		**/
#if OS_CFG_STAT_TASK_EN > 0u
    OSStatTaskCPUUsageInit(&err);               /* Compute CPU capacity with no task running  ͳ������          */
#endif

#ifdef CPU_CFG_INT_DIS_MEAS_EN					//���ʹ���˲����жϹر�ʱ��
    CPU_IntDisMeasMaxCurReset();
#endif

#if	OS_CFG_SCHED_ROUND_ROBIN_EN  				//��ʹ��ʱ��Ƭ��ת��ʱ��
												//ʹ��ʱ��Ƭ��ת���ȹ���,ʱ��Ƭ����Ϊ1��ϵͳʱ�ӽ��ģ���1*5=5ms
	OSSchedRoundRobinCfg(DEF_ENABLED,1,&err);  
#endif	
    
#if (APP_CFG_SERIAL_EN == DEF_ENABLED)    
    App_SerialInit();                                           /* Initialize Serial communication for application ...  */
#endif
    
		/**			�����ʼ��			**/
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_3);//�ж����ȼ�����
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
	
	
	OSTmrCreate((OS_TMR*  	)&tmr1,		//���������ʱ��tmr1
				(CPU_CHAR*	)"tmr1",
				(OS_TICK	) 0,		//��ʼ��ʱ5ms
				(OS_TICK	) 10 ,		//��ʱ����ms����ʱ��ʱ�ӽ���*����,��ǰΪ10ms
				(OS_OPT		) OS_OPT_TMR_PERIODIC,//����ģʽ
				(OS_TMR_CALLBACK_PTR)tmr1_callback,
				(void *		)0,
				(OS_ERR*	)&err);
	
    /* Create Application tasks                   */
	OS_CRITICAL_ENTER();	//�����ٽ���
			 
	OSTaskCreate((OS_TCB 	* )&CLCWDTTaskTCB,		//ѹ����������������
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
			 
	OSTaskCreate((OS_TCB 	* )&InstructionTaskTCB,		//ѹ����������������
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
	
	OSTaskCreate((OS_TCB 	* )&PASTaskTCB,		//ѹ����������������
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
	
	OSTaskCreate((OS_TCB 	* )&MAINPOWERTaskTCB,		//����Դ��ѹ��������
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
	
	OSTaskCreate((OS_TCB 	* )&TEMPTaskTCB,		//�¶ȴ�������������
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
	OSTaskCreate((OS_TCB 	* )&LOGINFOTaskTCB,		//������ӡ����
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
	
	OSTaskCreate((OS_TCB 	* )&LED0TaskTCB,		//LED0����
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
			 
	OSTaskCreate((OS_TCB 	* )&LED1TaskTCB,	   //LED1����
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
	
	//log1_info("LED0_Task has Create��prio:%d!\r\n",LED0_TASK_PRIO);
	//log1_info("LED1_Task has Create��prio:%d!\r\n",LED1_TASK_PRIO);
	//log1_info("LOGINFO_Task has Create��prio:%d!\r\n",LOGINFO_TASK_PRIO);
	//log1_info("TEMP_Task has Create��prio:%d!\r\n",TEMP_TASK_PRIO);
	//log1_info("MAINPOWER_Task has Create��prio:%d!\r\n",MAINPOWER_TASK_PRIO);
	log1_info("OS has Init!\r\n");
		 
	//OS_TaskSuspend((OS_TCB*)&AppTaskStartTCB,&err);		//����ʼ����	
	OS_CRITICAL_EXIT();	//�ٽ���
	while(DEF_TRUE)
	{
		OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1000ms
		InputRegister.SystemWorkTime++;//ϵͳ����ʱ��++
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
		if(ModbusDataPackage.DataFlag)		  //���ݰ��ѽ������
		{ 	
			for(ix=0;ix<ModbusDataPackage.DataLen;ix++) ModbusDataPackage.dat[ix] =USART1_GetChar();//���������ݷŵ�ָ��buf
			RX_Len=ModbusDataPackage.DataLen;
			
			USART1_ClearBuf_Flag();				 //��մ��ڽ��ջ���
			ModbusDataPackage.DataLen = 0;  //����ճ��ȣ�ע�����˳��
			ModbusDataPackage.DataFlag = 0; //��ձ��λ
			
			crc = CRC16_Check((uint8_t *)ModbusDataPackage.dat,RX_Len-2 );
			
			if( (( crc == ( (ModbusDataPackage.dat[RX_Len - 1] << 8) |    ModbusDataPackage.dat[RX_Len - 2])        )) \
			||  ( ( (ModbusDataPackage.dat[RX_Len - 1]) == 0xff    ) && ((ModbusDataPackage.dat[RX_Len - 2]) == 0xff))  )				
			{
				 Instruction_Process_Subfunction(RX_Len);
			}
		}
		OSTimeDlyHMSM(0,0,0,1,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1ms
	}
}

/*******************************************************************************
* Function Name  : Temp_ReadTASK
* Description    : �¶Ȳ�������
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
	
	OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
	OSSchedLock(&err);//�ر��������
	DS18B20_Temp=DS18B20_TEMP();
	OSSchedUnlock(&err);//�ָ�����

	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
	OSSchedLock(&err);//�ر��������
	DS18B20_Temp=DS18B20_TEMP();
	OSSchedUnlock(&err);//�ָ����� 	
	Old_Temp=DS18B20_Temp;
	
	while(1)
	{
		InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x21;	
		
		OSSchedLock(&err);//�ر��������
		DS18B20_Temp=DS18B20_TEMP();
		//log1_info("DS18B20_temp1:%.3f��\r\n",DS18B20_Temp);
		OSSchedUnlock(&err);//�ָ�����  
		
		ABStemp = (DS18B20_Temp>Old_Temp)?(DS18B20_Temp-Old_Temp):(Old_Temp-DS18B20_Temp);
		if( (DS18B20_Temp>=-40) && (DS18B20_Temp<=85) )
		{
			if( (ABStemp<3) || (Temp_Count<3) )
			{
				Temp_Count++;
				InputRegister.Temperature=DS18B20_Temp;
				Old_Temp=DS18B20_Temp;
				//log1_info("DS18B20_temp2:%.3f��\r\n",DS18B20_Temp);
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
			OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1000ms��ÿ��1S���������¶Ȳ���
		}
		else
		{
			OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ300ms	
		}
	}	
	
}

/*******************************************************************************
* Function Name  : MainPower_ReadTASK
* Description    : ����Դ��ѹ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void MAINPOWER_Task(void *p_arg)
{
	OS_ERR err;
	
	p_arg = p_arg;
	OSTimeDlyHMSM(0,0,1,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ2000ms
	while(1)
	{
		MainPower_Flag=0;
		InputRegister.SystemWorkStatus=(InputRegister.SystemWorkStatus & 0xff00)|0x22;
		IADC_Configuration();
		//OSTimeDlyHMSM(0,0,0,2,OS_OPT_TIME_HMSM_STRICT,&err);
		InputRegister.MainPower_V=Get_Adc_Average(5);
		//OSTimeDlyHMSM(0,0,0,2,OS_OPT_TIME_HMSM_STRICT,&err);
		
		//�ر�ʱ��
		RCC_HSICmd(DISABLE);  // Enable The HSI (16Mhz)
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, DISABLE);	//Enable ADC1 clock
		
		//log1_info("MainPower:%.3fV\r\n",InputRegister.MainPower_V);
		
		OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ2000ms
		
	}
}

/*******************************************************************************
* Function Name  : PAS_TASK
* Description    : Һλ�߶��źŲ�������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void PAS_Task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	
	
	OSTimeDlyHMSM(0,0,2,0,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ2000ms
	OSTmrStart(&tmr1,&err);			//���������ʱ��tmr1
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
		OSTimeDlyHMSM(0,0,0,10,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ
	}

}

void CLCWDT_Task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	
	while(1)
	{
		OSTimeDlyHMSM(0,0,0,750,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1000ms
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
		OSTimeDlyHMSM(0,0,0,40,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ40ms
		LED0_OFF();
		OSTimeDlyHMSM(0,0,1,388,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ1388ms��ÿ����42��
	}
}

void LED1_Task(void *p_arg)
{
	OS_ERR err;
	p_arg = p_arg;
	
	while(1)
	{
		//LED1_ON();
		//OSTimeDlyHMSM(0,0,0,333,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ200ms
		LED1_OFF();
		OSTimeDlyHMSM(0,0,0,100,OS_OPT_TIME_HMSM_STRICT,&err); //��ʱ500ms
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
	
	OS_TaskSuspend((OS_TCB*)&LOGINFOTaskTCB,&err);		//���������
	
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
