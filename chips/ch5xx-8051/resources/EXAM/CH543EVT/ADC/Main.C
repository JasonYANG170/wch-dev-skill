/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 ADC初始化,ADC中断和查询方式演示外部通道以及内部通道检测
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


/******************************************************************************/
/* 头文件包含 */	
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"
#include ".\ADC\ADC.H"

/******************************************************************************/
/* 常、变量定义 */
volatile UINT8 GetValueFlag = 0;                                                /* ADC中断方式采集完成标志位，1有效 */
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
    UINT8 ch;
    
    /*****************************系统相关初始化*******************************/
    CfgFsys( );                                                                 /* CH543时钟选择 */
    mDelaymS( 5 );                                                              /* 稍加延时等待延时稳定 */
    mInitSTDIO( );                                                              /* 串口0初始化 */
    
    printf("ADC demo start ...\n");
    
    /* ADC初始化 */    
    ADC_Init( 0 );                                                              /* ADC初始化,选择采样时钟 */
    
    /*****************************主循环***************************************/
    while( 1 )
    {
#if ( EN_ADC_INT == 0 )                                                         /* 查询方式演示ADC通道切换和采集 */
        /* 循环采集ADC 0~13 通道 */
        for( ch=0; ch!=14; ch++ )
        {  
            ADC_ChSelect( ch );                                                 /* 选择通道 */
            ADC_StartSample( );                                                 /* 启动采样 */
            while( (ADC_CTRL&bADC_IF) == 0 )                                    /* 等待转换完成 */
            {
            }
            ADC_CTRL |= bADC_IF;                                                /* 清标志 */
            printf("ch%d=%d\n",(UINT16)ch, (UINT16)ADC_DAT);                    /* 输出ADC采样值 */
        }
        printf("\n");
        mDelaymS( 100 );                                                        /* 模拟单片机干其他事情 */
#else                                                                           /* 中断方式演示ADC通道切换和采集，以通道0为例 */
        ADC_StartSample( );                                                     /* 启动采样 */
        mDelaymS( 10 );                                                         /* 模拟单片机干其他事情 */
        if( GetValueFlag )                                                      /* 通道采集完成标志 */
        {
            printf("ch0=%d\n",(UINT16)ADC_DAT);                                 /* 输出ADC采样值 */
        }
        mDelaymS( 100 );                                                        /* 模拟单片机干其他事情 */
#endif
    }
}