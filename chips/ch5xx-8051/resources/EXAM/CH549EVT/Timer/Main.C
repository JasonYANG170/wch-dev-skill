/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/21
* Description        : CH549 Time 初始化、定时器、计数器赋值，T2捕捉功能等
                       定时器中断处理
                       注意包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\Timer\Timer.H"
#pragma  NOAREGS
void main( )
{
    CfgFsys( );                                                                //CH549时钟选择配置
    mDelaymS(5);                                                               //修改主频，建议稍加延时等待主频稳定
    mInitSTDIO( );                                                             //串口初始化
    printf("Timer Demo start ...\n");
    P1_MOD_OC &= ~(1<<7);                                                      //SCK测试引脚配置推挽输出
    P1_DIR_PU |= (1<<7);
#ifdef T0_INT
    printf("T0 Test ...\n");
    mTimer0Clk12DivFsys();                                                     //T0定时器时钟设置 FREQ_SYS/12
    mTimer_x_ModInit(0,1);                                                     //T0 定时器模式设置 模式1 16位定时器
    mTimer_x_SetData(0,2000);                                                  //T0定时器赋值 24MHZ 1MS中断
    mTimer0RunCTL(1);                                                          //T0定时器启动
    ET0 = 1;                                                                   //T0定时器中断开启
    EA = 1;
#endif
#ifdef T1_INT                                                                  //注意：DEBUG.C的mInitSTDIO使用T1的模式2作为波特率发生器
    printf("T1 Test ...\n");
    mTimer1Clk12DivFsys( );                                                    //T1定时器时钟设置 FREQ_SYS/12
    mTimer_x_ModInit(1,2);                                                     //T1 定时器模式设置,模式2 8位自动重装
    mTimer_x_SetData(1,0xC7C8);                                                //T1定时器赋值 TH1=TL1=256-200=38H;  100us中断
    mTimer1RunCTL(1);                                                          //T1定时器启动
    ET1 = 1;                                                                   //T1定时器中断开启
    EA = 1;
#endif
#ifdef T2_INT
    printf("T2 Test ...\n");
    mTimer2ClkFsys();                                                          //T2定时器时钟设置  等于FREQ_SYS
    mTimer_x_ModInit(2,0);                                                     //T2 定时器模式设置
    mTimer_x_SetData(2,48000);                                                 //T2定时器赋值 FREQ_SYS=24MHz,2ms中断
    mTimer2RunCTL(1);                                                          //T2定时器启动
    ET2 = 1;                                                                   //T2定时器中断开启
    EA = 1;
#endif
//可以同时开启T0，测SCK引脚输出脉冲宽度
#ifdef T2_CAP
    printf("T2_CAP Test ...\n");
    P1_MOD_OC &= ~(1<<1);                                                     //CAP2 浮空输入
    P1_DIR_PU &= ~(1<<1);
    P1_MOD_OC &= ~(1<<0);                                                     //CAP1 浮空输入
    P1_DIR_PU &= ~(1<<0);
    P3_MOD_OC &= ~(1<<6);                                                     //CAP0 浮空输入
    P3_DIR_PU &= ~(1<<6);
    mTimer2Clk12DivFsys();                                                    //T2定时器时钟设置 FREQ_SYS/12
    mTimer_x_SetData(2,0);                                                    //T2 定时器模式设置捕捉模式
    CAP2Init(1);                                                              //T2 CAP2(P11)设置，任意沿捕捉
    CAP1Init(1);                                                              //T2 CAP1(P10)设置，任意沿捕捉
    CAP0Init(1);                                                              //T2 CAP0(P36)设置，任意沿捕捉
    mTimer2RunCTL(1);                                                         //T2定时器启动
    ET2 = 1;                                                                  //T2定时器中断开启
    EA = 1;
#endif
    while(1)
    {
    }
}
