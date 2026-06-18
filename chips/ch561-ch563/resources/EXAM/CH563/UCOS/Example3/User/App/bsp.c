/********************************** (C) COPYRIGHT *******************************
* File Name          : BSP.C
* Author             : WCH
* Version            : V1.0
* Date               : 2015/02/05
* Description        : Board support package of ucos on CH563.
*******************************************************************************/

/******************************************************************************/
/* 头文件包含 */
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "BSP.H"
#include "includes.h"
#include "OS_CPU.h"
#include <stdio.h>


/******************************************************************************/
/* 全局变量定义 */
OS_STK  TaskStartStk[TASK_STK_SIZE];                                            /* 启动任务的栈 */
OS_STK  Task1Stk[TASK_STK_SIZE];                                                /* 任务1的栈 */
OS_STK  Task2Stk[TASK_STK_SIZE];                                                /* 任务2的栈 */


/******************************************************************************/
/* 内部函数声明 */
void TIM0_INIT( UINT32 ticksPerSec );
void IRQCTimer0( void );
void Task1 (void *pdata);
void Task2( void *pdata );
void TaskStart (void *pdata);
void BSP_init(void);
void TIM1_INIT( void );
void Uart0_Init( UINT32  baud );


/*******************************************************************************
* @function     TIM0_INIT
* @param        ticksPerSec 每秒钟中断的触发次数
* @retval       None
* @brief        TIM0定时器的初始化函数，该定时器用来作为UCOS的系统时钟
*******************************************************************************/
void TIM0_INIT( UINT32 ticksPerSec ) 
{
    /* 外部函数作用域扩展 */
    extern void IRQASMTimer0(void);                                             

    /* 中断映射设置，将TIM0的IRQ中断映射成向量中断 */
    R8_SAFE_ACCESS_SIG = 0x57 ;  /* unlock step 1 */
    R8_SAFE_ACCESS_SIG = 0xA8 ;  /* unlock step 2 */
    
    R32_INT_VEC_TMR0 = MAKE_INT_VEC_IRQ( IRQASMTimer0 );                        /* 设置TIM0 IRQ中断触发跳转地址 */
    R8_INT_VEC_IRQ |= (RB_IV_IRQ_TMR0 );		                                /* 重新映射TIM0的IRQ为向量中断 */
    
    R8_SAFE_ACCESS_SIG = 0x00 ;  /* lock, to prevent unexpected writing */

    /* 设置TIM0，设置触发时间，中断类型 */
    R32_TMR0_CNT_END = 100000000/ticksPerSec;	            				    /* 设置定时触发的时间 */
    R8_TMR0_CTRL_MOD &= ~RB_TMR_ALL_CLEAR;
    R8_TMR0_INT_FLAG = 0xff;                                                    /* 清除所有的中断标志 */                                                               
    R8_TMR0_INTER_EN = RB_TMR_IE_CYC_END;      				                    /* 定时器时钟周期结束中断使能，PWM 结束中断使能 */

    /* 使能中断 */
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR0;                                          /* 允许定时器0相关中断产生 */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* 开启FIQ全局中断 */

    /* 系统时钟开始运行 */
    R8_TMR0_CTRL_MOD |= RB_TMR_COUNT_EN;										/* 开始计数 */    
	
}

/*******************************************************************************
* @function     IRQ_C_Timer0
* @param        None
* @retval       None
* @brief        TIM0向量中断服务函数，该函数不要使用__irq修饰指明为中断函数，
*               也不要在应用程序中调用该函数。
*******************************************************************************/
void IRQ_C_Timer0( void )
{
	#if OS_CRITICAL_METHOD == 3                                
    OS_CPU_SR  cpu_sr;
	#endif

    OS_ENTER_CRITICAL();
    
    /* 清楚定时器中断标志，以及将计数值清零 */
    R8_INT_EN_IRQ_0 &= ~RB_IE_IRQ_TMR0;		                                    /* 关闭定时器中断 */
    R8_TMR0_CTRL_MOD |= RB_TMR_ALL_CLEAR;
    R8_TMR0_CTRL_MOD &= ~RB_TMR_ALL_CLEAR;
    R8_TMR0_INT_FLAG |= 0xFF;
    R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR0;		                                    /* 开启定时器中断 */

    OS_EXIT_CRITICAL();

    OSTimeTick();
}

/*******************************************************************************
* @function     Task1
* @param        pdata
* @retval       None
* @brief        任务1的函数
*******************************************************************************/
void  Task1 (void *pdata) 
{
	pdata = pdata;                                                              /* 消除警告 */

	for(;;) 
    {
        printf("task1 print CPU Usage:%d\r\n",OSCPUUsage);                      /* 打印CPU使用率信息 */
        
        OSTimeDlyHMSM(0,0,4,0);                                                 /* 延时4秒钟 */
    }
}


/*******************************************************************************
* @function     Task2
* @param        pdata
* @retval       None
* @brief        任务2的函数
*******************************************************************************/
void Task2( void *pdata )
{   
    pdata = pdata;                                                              /* 消除警告 */
    
    OSTimeSet(  1000000000 );                                                   /* 设置系统时间 */
        
    for(;;)
    {    
        printf("task2 OSTime:%d\r\n",OSTimeGet());                              /* 打印系统时间 */
        
        OSTimeDlyHMSM(0,0,2,0);                                                 /* 延时2秒钟 */
    }
}

/*******************************************************************************
* @function     TaskStart
* @param        pdata
* @retval       None
* @brief        启动任务的函数
*******************************************************************************/
void TaskStart(void *pdata)
{
    pdata = pdata;                                                              /* Prevent compiler warning                 */
    
    BSP_init();  
    TIM1_INIT();    

    /* 初始化cpu使用率统计任务，如果需要使用该统计功能，该函数必须要调用 */
    OSStatInit();                                                               /* cpu使用率统计任务初始化 */

    /* 创建用户任务 */
    OSTaskCreate(Task1, (void *)0, &Task1Stk[TASK_STK_SIZE - 1], 1);
    OSTaskCreate(Task2, (void *)0, &Task2Stk[TASK_STK_SIZE - 1], 5);
    
    OSTaskDel(0);                                                               /* 该起始任务删除自己 */  
}


/*******************************************************************************
* @function     BSP_init
* @param        None
* @retval       None
* @brief        BSP初始化函数
*******************************************************************************/
void BSP_init(void) 
{    
    ///可此添加其他需要的驱动，也可以在用户任务中

    Uart0_Init( 115200 );                                                       
    printf("stdio init ok.\r\n");

    /* TIM0初始化，TIIM0作为UCOS的系统时钟，该函数必须调用 */
    TIM0_INIT( OS_TICKS_PER_SEC );
}

/*******************************************************************************
* @function     TIM1_INIT
* @param        None
* @retval       None
* @brief        TIM1的初始化函数，设置为每秒触发一次中断
*******************************************************************************/
void TIM1_INIT( void )
{
    /* 设置TIM1，设置触发时间，中断类型 */
    R32_TMR1_CNT_END = 100000000/1;	            				                /* 设置定时触发的时间 */
    R8_TMR1_CTRL_MOD &= ~RB_TMR_ALL_CLEAR;
    R8_TMR1_INT_FLAG = 0xff;                                                    /* 清除所有的中断标志 */                                                               
    R8_TMR1_INTER_EN = RB_TMR_IE_CYC_END;      				                    /* 定时器时钟周期结束中断使能，PWM 结束中断使能 */

    /* 使能中断 */
    R8_INT_EN_FIQ_0 |= RB_IE_FIQ_TMR1;                                          /* 允许定时器1相关IRQ中断产生 */
    R8_INT_EN_FIQ_GLOB |= RB_IE_FIQ_GLOB;                                       /* 开启FIQ全局中断 */

    /* 系统时钟开始运行 */
    R8_TMR1_CTRL_MOD |= RB_TMR_COUNT_EN;										/* 开始计数 */    

}

/*******************************************************************************
* @function     Uart0_Init
* @param        baud-串口波特率，最高为主频1/8
* @retval       None
* @brief        串口0初始化
*******************************************************************************/
void Uart0_Init( UINT32  baud )
{
    UINT32 x;

    x = 10 * FREQ_SYS/ 8 / baud;                                                /* 设置串口0波特率 */
    x += 5;                                                                     /* 四舍五入 */
    x /= 10;
    R8_UART0_LCR = RB_LCR_DLAB;                                                 /* DLAB位置1 */
    R8_UART0_DIV = 1;                                                           /* 预分频 */
    R8_UART0_DLM = x>>8;
    R8_UART0_DLL = x&0xff;              
    R8_UART0_LCR = RB_LCR_WORD_SZ ;                                             /* 设置字节长度为8 */
    R8_UART0_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR |    
                   RB_FCR_FIFO_EN ;                                             /* 设置FIFO触发点为14，清发送和接收FIFO，FIFO使能 */
    R8_UART0_IER = RB_IER_TXD_EN;                                               /* TXD enable */
    R8_UART0_MCR = RB_MCR_OUT1;
    R32_PB_SMT |= RXD0|TXD0;                                                    /* RXD0 schmitt input, TXD0 slow rate */
    R32_PB_PU  |= RXD0;                                                         /* disable pulldown for RXD0, keep pullup */
    R32_PB_DIR |= TXD0;                                                         /* TXD0 方向置为输出 */
}


/*******************************************************************************
* @function     fputc
* @param        c  -writes the character specified by c 
* @param        *f -the output stream pointed to by *f
* @retval       None
* @brief        通过串口输出监控信息
*******************************************************************************/
int fputc( int c, FILE *f )
{
    R8_UART0_THR = c;                                                           /* 发送数据 */
    while( ( R8_UART0_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                        /* 等待数据发送 */
    return( c );
}

/***************************End of file.***************************************/


