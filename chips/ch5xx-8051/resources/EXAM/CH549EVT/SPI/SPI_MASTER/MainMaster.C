/********************************** (C) COPYRIGHT *******************************
* File Name          : MainMaster.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/23
* Description        : CH549 SPI主机和CH549 SPI从机通讯，主机发送数据，从机取反返回
                       从机参考MainSlave.C
                       注意包含DEBUG.C/SPI.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\SPI\SPI.H"
#pragma  NOAREGS
/*硬件接口定义*/
/******************************************************************************
使用CH549 硬件SPI接口
         CH549
         P1.4        =       SCS
         P1.5        =       MOSI
         P1.6        =       MISO
         P1.7        =       SCK
*******************************************************************************/
void main()
{
    UINT8 ret,i=0;
    CfgFsys( );
    mDelaymS(20);                                                              //调整主频，建议稍加延时等待内部时钟稳定
    mInitSTDIO( );                                                             //串口0初始化
    printf("SPI Master start ...\n");
    SPIMasterModeSet(3);                                                       //SPI主机模式设置，模式3
    SPI_CK_SET(12);                                                            //12分频
    while(1)
    {
        SCS = 0;                                                               //SPI主机发送数据
        CH549SPIMasterWrite(i);
        mDelaymS(5);
        ret = CH549SPIMasterRead();                                            //接收SPI从机返回的数据，取反返回
        SCS = 1;
        if(ret != (i^0xff))
        {
            printf("Err: %02X  %02X  \n",(UINT16)i,(UINT16)ret);               //如果不等于发送数据的取反，打印错误信息
        }
        else
        {
            printf("success %02x\n",(UINT16)i);                                //每成功40次打印一次
        }
        i = i+1;
        mDelaymS(50);
    }
}