/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/27
* Description        : CH549 Type-C使用
主模式(DFP) ：支持检测Typc-C设备的插拔，支持设备的正插和反插检测,
              支持默认供电电流，1.5A供电电流以及3A的供电电流设置;
从模式(UFP) ：检测设备连接上主机，可检测主机供电能力：
                            默认供电电流，1.5A供电电流以及3A供电电流
DFP (Downstream Facing Port) Host端
UFP (Upstream Facing Port)   Dev端
注意包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include "Type_C.H"
#pragma  NOAREGS
void main( )
{
    UINT16 j = 0;
    CfgFsys( );                                                          //CH549时钟选择配置
    mDelaymS(20);                                                        //修改主频建议稍加延时等待芯片供电稳定
    mInitSTDIO( );                                                       //串口0初始化
    printf("Type-C Demo start ...\n");
#ifdef TYPE_C_DFP                                                        //Type-C主机检测正反插，通知设备主机供电能力
    TypeC_DFP_Init(3);                                                     //主机供电3A电流电流输出能力
    while(1)
    {
        j = TypeC_DFP_Insert();
        if( j == UCC_DISCONNECT )
        {
            VBUS = 1;                                                          //关闭VBUS
            printf("UFP disconnect...\n");
        }
        else if( j == UCC1_CONNECT )
        {
            VBUS = 0;                                                          //开启VBUS
            printf("UFP CC1 insertion...\n");                                  //正向插入
        }
        else if( j == UCC2_CONNECT )
        {
            VBUS = 0;                                                          //开启VBUS
            printf("UFP CC2 insertion...\n");                                  //反向插入
        }
        else if( j == UCC_CONNECT )
        {
            VBUS = 0;                                                          //开启VBUS
            printf("UFP connect...\n");
        }
        mDelaymS(500);                                                       //延时无意义，模拟单片机执行其他操作
    }
#endif
#ifdef TYPE_C_UFP
    TypeC_UPF_PDInit();                                                    //UPF初始化
    while(1)
    {
        j = TypeC_UPF_PDCheck();                                             //检测主机的供电能力
        if(j == UPF_PD_Normal)
        {
            printf("DFP defaultPower...\n");
        }
        if(j == UPF_PD_1A5)
        {
            printf("DFP 1.5A...\n");
        }
        if(j == UPF_PD_3A)
        {
            printf("DFP 3.0...\n");
        }
        if(j == UPD_PD_DISCONNECT)
        {
            printf("disconnect...\n");
        }
        mDelaymS( 500 );                                                     //延时无意义，模拟单片机执行其他操作
    }
#endif
}
