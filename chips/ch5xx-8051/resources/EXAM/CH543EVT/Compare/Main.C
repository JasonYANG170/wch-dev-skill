/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 CP端外部通道CH4(P1.4)和CM端外部通道CH11(P1.3)比较,输出低或3.3V
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


/******************************************************************************/
/* 头文件包含 */	
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"
#include ".\Compare\Compare.H"
#pragma  NOAREGS

/*******************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main()
{
    /*****************************系统相关初始化*******************************/
    CfgFsys( );                                                                 /* CH543时钟选择配置 */
    mDelaymS( 20 );                                                              /* 稍加延时等待延时稳定 */
    mInitSTDIO( );                                                              /* 串口0初始化 */
    
    printf("CMP demo start ...\n");
    
    /* 比较器通道初始化 */
    CMP_Init( );                                                                /* 比较器通道初始化 */
    
    /*****************************主循环***************************************/
    while( 1 );
}
