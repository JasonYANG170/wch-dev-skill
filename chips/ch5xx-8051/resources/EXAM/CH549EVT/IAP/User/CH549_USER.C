/********************************** (C) COPYRIGHT ******************************
* File Name          : CH549_USER.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/24
* Description        : 用户程序。演示用户程序运行，当P46输入低电平时，程序跳转至IAP
*                      程序区，进行用户程序升级。
                       D2慢闪
                       注意包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#pragma NOAREGS
#define IAP_StartAddr     0xE000                            //IAP程序存放的起始地址，该地址至少要比实际的IAP地址小4字节
sbit    EnableIAP  =      P4^6;                             //IAP跳转检测引脚
sbit    D2 = P2^2;
typedef void (*Function)( void );
Function Jump2IAPAddr;
/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main( void )
{
    UINT32 i=0;
    CfgFsys();
    mDelaymS(5);
    mInitSTDIO( );
    printf("User Demo start...\n");
    Jump2IAPAddr = (Function)IAP_StartAddr;                //函数指针赋值,指定IAP程序地址
    while(1)
    {
        if( EnableIAP == 0 )                               //检测P46引脚是否为低
        {
            printf("Jump to IAP..\n");
            Jump2IAPAddr( );                               //跳转至IAP程序区
        }
        i++;
        if(i== 200000)
        {
            printf("User APP Wait..\n");
            D2 = ~D2;
            i = 0;
        }
    }
}
