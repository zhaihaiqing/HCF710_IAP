/*
************************************************************************************************************************
*                                                      uC/OS-III
*                                                 The Real-Time Kernel
*
*                                  (c) Copyright 2009-2012; Micrium, Inc.; Weston, FL
*                           All rights reserved.  Protected by international copyright laws.
*
*                                       OS CONFIGURATION (APPLICATION SPECIFICS)
*
* File    : OS_CFG_APP.H
* By      : JJL
* Version : V3.03.00
*
* LICENSING TERMS:
* ---------------
*           uC/OS-III is provided in source form for FREE short-term evaluation, for educational use or 
*           for peaceful research.  If you plan or intend to use uC/OS-III in a commercial application/
*           product then, you need to contact Micrium to properly license uC/OS-III for its use in your 
*           application/product.   We provide ALL the source code for your convenience and to help you 
*           experience uC/OS-III.  The fact that the source is provided does NOT mean that you can use 
*           it commercially without paying a licensing fee.
*
*           Knowledge of the source code may NOT be used to develop a similar product.
*
*           Please help us continue to provide the embedded community with the finest software available.
*           Your honesty is greatly appreciated.
*
*           You can contact us at www.micrium.com, or by phone at +1 (954) 217-2036.
************************************************************************************************************************
*/

#ifndef OS_CFG_APP_H
#define OS_CFG_APP_H

/*
************************************************************************************************************************
*                                                      CONSTANTS
************************************************************************************************************************
*/

                                                            /* --------------------- MISCELLANEOUS ------------------ */
#define  OS_CFG_MSG_POOL_SIZE            100u               /* Maximum number of messages                             */	//消息数量
#define  OS_CFG_ISR_STK_SIZE             256u               /* Stack size of ISR stack (number of CPU_STK elements)   */	//中断任务堆栈大小
#define  OS_CFG_TASK_STK_LIMIT_PCT_EMPTY  10u               /* Stack limit position in percentage to empty            */	//任务堆栈深度


                                                            /* ---------------------- IDLE TASK --------------------- */
#define  OS_CFG_IDLE_TASK_STK_SIZE       128u               /* Stack size (number of CPU_STK elements)                */	//空闲任务


                                                            /* ------------------ ISR HANDLER TASK ------------------ */
#define  OS_CFG_INT_Q_SIZE                10u               /* Size of ISR handler task queue                         */	//中断队列大小
#define  OS_CFG_INT_Q_TASK_STK_SIZE      128u               /* Stack size (number of CPU_STK elements)                */	//中断服务管理任务大小


                                                            /* ------------------- STATISTIC TASK ------------------- */	//统计任务
#define  OS_CFG_STAT_TASK_PRIO     (OS_CFG_PRIO_MAX - 2u)   /* Priority                                               */
#define  OS_CFG_STAT_TASK_RATE_HZ         10u               /* Rate of execution (10 Hz Typ.)                         */
#define  OS_CFG_STAT_TASK_STK_SIZE       128u               /* Stack size (number of CPU_STK elements)                */	//统计任务堆栈


                                                            /* ------------------------ TICKS ----------------------- */
#define  OS_CFG_TICK_RATE_HZ            100u               /* Tick rate in Hertz (10 to 1000 Hz)                     */	//时钟节拍
#define  OS_CFG_TICK_TASK_PRIO             1u               /* Priority                                               */	//时钟节拍任务优先级
#define  OS_CFG_TICK_TASK_STK_SIZE       128u               /* Stack size (number of CPU_STK elements)                */	//时钟节拍任务堆栈大小
#define  OS_CFG_TICK_WHEEL_SIZE           17u               /* Number of 'spokes' in tick wheel                       */


                                                            /* ----------------------- TIMERS ----------------------- */	//定时任务
#define  OS_CFG_TMR_TASK_PRIO      (OS_CFG_PRIO_MAX - 2u)   /* Priority                                               */	//定时任务优先级
#define  OS_CFG_TMR_TASK_RATE_HZ         100u               /* Rate for timers (10 Hz Typ.)                           */	//定时频率
#define  OS_CFG_TMR_TASK_STK_SIZE        128u               /* Stack size (number of CPU_STK elements)                */	//堆栈
#define  OS_CFG_TMR_WHEEL_SIZE            17u               /* Number of 'spokes' in timer wheel                      */	//定时器列表数量

#endif
