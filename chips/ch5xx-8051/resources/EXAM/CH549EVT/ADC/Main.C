/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2019/07/22
* Description        : CH549 ADC初始化，ADC中断和查询方式演示外部通道以及内部温度检测
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include "ADC.H"
#pragma  NOAREGS
volatile UINT8 GetValueFlag = 0;                                           //ADC中断方式采集完成标志位，1有效
void main( )
{
    UINT8 ch;
    CfgFsys( );                                                            //CH549时钟选择配置24MHz
    mDelaymS(5);                                                           //稍加延时等待延时稳定
    mInitSTDIO( );                                                         //串口0初始化
    printf("ADC demo start ...\n");
#if 1
    ADC_ExInit( 3 );                                                       //ADC初始化,选择采样时钟
    while(1)
    {
#if (EN_ADC_INT == 0)                                                      //查询方式演示ADC通道切换和采集
        for(ch=0; ch!=16; ch++)
        {
            ADC_ChSelect(ch);                                               //选择通道
            ADC_StartSample();                                                 //启动采样
            while((ADC_CTRL&bADC_IF) == 0)
            {
                ;    //查询等待标志置位
            }
            ADC_CTRL = bADC_IF;                                                //清标志
            printf("ch%d=%d  ",(UINT16)ch,(UINT16)ADC_DAT);                    //输出ADC采样值
        }
        printf("\n");
        printf("\n");
        mDelaymS(1000);
#else                                                                          //中断方式演示ADC通道切换和采集，以通道0为例
       ADC_StartSample();                                                      //启动采样				
       mDelaymS(10);                                                           //模拟单片机干其他事情							
       if(GetValueFlag)                                                        //通道采集完成标志
       {
          printf("ch0  \n",(UINT16)ADC_DAT);                                   //输出ADC采样值		 
       }
       mDelaymS(1000);                                                         //模拟单片机干其他事情					 
#endif
    }
#else                                                                         //温度检测
    ADC_InTSInit();                                                           //初始化温度检测
    while(1)
    {
        ADC_StartSample();                                                    //启动采样
#if EN_ADC_INT
        mDelaymS(20);                                                         //中断函数处理，也可以添加标志位
#else
        while((ADC_CTRL&bADC_IF) == 0)
        {
            ;    //查询等待标志置位
        }
        ADC_CTRL = bADC_IF;                                                   //清标志
#endif
        printf("%d ",(UINT16)ADC_DAT);                                        //输出ADC采样值
        printf("\n");
    }
#endif
}