/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 外部通道和内部参考电压比较；外部通道和外部通道比较；
                       内部参考电压和外部通道比较
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "CH549.H"
#include "DEBUG.H"
#include "Compare.H"
#pragma  NOAREGS
void main()
{
    CfgFsys( );                                                                  //CH549时钟选择配置
    mDelaymS(20);
    mInitSTDIO( );                                                               //串口0初始化
    printf("CMP demo start ...\n");
    CMP_Init();                                                                  //比较器通道初始化
    while(1)
    {
#if (EN_ADC_INT==0)
        if(ADC_CTRL&bCMPO)
        {
            printf("+ > -\n");
        }
        else
        {
            printf("+ < -\n");
        }
        mDelaymS(1000);
#endif
    }
}
