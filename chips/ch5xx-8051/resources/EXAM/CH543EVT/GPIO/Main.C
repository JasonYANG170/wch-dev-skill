/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 GPIO功能演示: 普通输入输出测试; 中断触发模式
                       注意包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


/******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"
#include ".\GPIO\GPIO.H"
#pragma  NOAREGS

/*******************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main( void )
{
    /*****************************系统相关初始化*******************************/
    CfgFsys( );                                                                 /* CH543时钟选择配置 */
    mDelaymS( 20 );
    mInitSTDIO( );                                                              /* 串口0初始化 */
    printf("GPIO demo start ...\n");
    
    /* 配置GPIO */
    GPIO_Init( PORT1,PIN3,MODE3 );                                              /* P13上拉输入 */
    GPIO_Init( PORT1,PIN4,MODE3 );                                              /* P14上拉输入 */
    GPIO_Init( PORT1,PIN5,MODE3 );                                              /* P15上拉输入 */
    GPIO_Init( PORT1,PIN7,MODE3 );                                              /* P17上拉输入 */
    GPIO_Init( PORT3,PIN1,MODE3 );                                              /* P31上拉输入 */
    GPIO_Init( PORT3,PIN2,MODE3 );                                              /* P32(INT0)上拉输入 */
    GPIO_Init( PORT3,PIN3,MODE3 );                                              /* P33(INT1)上拉输入 */
    
    /* 配置外部中断 */
    GPIO_INT_Init( ( INT_P15_L | INT_P17_L | INT_INT0_L | INT_INT1_L ), INT_EDGE, Enable);
    
    /*****************************主循环***************************************/
    while( 1 );
    
}
