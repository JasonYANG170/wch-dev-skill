/********************************** (C) COPYRIGHT *******************************
* File Name          : MAIN.C
* Author             : WCH
* Version            : V1.0
* Date               : 2015/02/05
* Description        : CH563 UART0 UCOS
*                      (1)、CH563 Examples by KEIL4 V4.23
*                      (2)、CH563 UCOS例程
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "includes.h"
#include "OS_CPU.h"
#include "BSP.H"


/******************************************************************************/
/* 宏定义 */


/******************************************************************************/
/* 内部函数声明 */
void TIM0_INIT( UINT32 );                                                       /* TIM0定时器初始化 */
void TaskStart(void *data);                                                     /* 启动任务函数 */
void Task1(void *data);                                                         /* 任务1函数 */
void Task2(void *data);                                                         /* 任务2函数 */


/******************************************************************************/
/* 外部变量作用域扩展 */
extern OS_STK  TaskStartStk[TASK_STK_SIZE];                                     /* 启动任务的栈 */


/*******************************************************************************
* @function     IRQ_C_Handler
* @param        None
* @retval       None
* @brief        IRQ中断函数，不要加__irq进行修饰，在汇编代码中被间接调用
*******************************************************************************/
void IRQ_C_Handler( void )
{
    UINT8 i;
    
    if(R8_INT_FLAG_0&RB_IF_TMR1)
    {
        i= R8_TMR1_INT_FLAG;
        if(i & RB_TMR_IF_CYC_END)                                               /* 捕获超时中断 */
        {                                                                       
            R8_INT_EN_IRQ_0 &= ~RB_IE_IRQ_TMR1;                                 /* 关闭定时器中断 */
            R8_TMR1_CTRL_MOD |= RB_TMR_ALL_CLEAR;
            R8_TMR1_CTRL_MOD &= ~RB_TMR_ALL_CLEAR;
            printf("TIM1 IRQ\r\n");         /* 仅作为测试用，中断中请勿使用打印，耗费时间 */
            R8_TMR1_INT_FLAG |= 0xFF;
            R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR1;                                  /* 开启定时器中断 */
        }
    }
}
  

/*******************************************************************************
* @function     FIQ_C_Handler
* @param        None
* @retval       None
* @brief        FIQ中断函数，不要加__irq进行修饰，在汇编代码中被间接调用
*******************************************************************************/
void FIQ_C_Handler( void )
{
    while(1);
}


/*******************************************************************************
* @Function     main
* @Input        None
* @Output       None
* @retVal       None
* @brief        主函数
*******************************************************************************/
int main( void ) 
{
    /* 延时，待晶振振荡稳定 */
    Delay_ms( 100 );                                                 

    /* 将CPU主导权交给操作系统 */
    OSInit();                                                                   /* 操作系统初始化 */
    OSTaskCreate(TaskStart, (void *)0, &TaskStartStk[TASK_STK_SIZE - 1], 0);    /* 创建起始任务，在起始任务中初始化BSP以及创建其它任务 */
    OSStart();                                                                  /* 任务开始调度 */
}


#if 0
/******************************************************************************/
/* 以下函数为调试时使用函数，暂时不要删除，遇到bug调试时候使用 */
void UndefineInstruction_P( void )
{
    //while(1)
        printf("UndefineInstruction_P\r\n");
}
void PrefetchAbort_P( void )
{
    //while(1)
        printf("PrefetchAbort_P\r\n");
}
void SoftwareInterrupt_P( void )
{
    //while(1)
        printf("SoftwareInterrupt_P\r\n");
}
void DataAbort_P( void )
{
   //while(1)
        printf("DataAbort_P\r\n");
}

#endif

/*********************************** endfile **********************************/
