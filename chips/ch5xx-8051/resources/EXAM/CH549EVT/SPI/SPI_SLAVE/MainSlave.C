/********************************** (C) COPYRIGHT *******************************
* File Name          : MainSlave.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/23
* Description        : CH549 SPI设备例子演示，连接SPI主机进行数据收发，从机获取主机的数据取反
                       然后发送给主机
                       注意包含DEBUG.C/SPI.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\SPI\SPI.H"
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
    mDelaymS(5);                                                               //修改系统主频，建议稍加延时等待主频稳定
    mInitSTDIO( );                                                             //串口0初始化
    printf("SPI Slave start ...\n");
    SPISlvModeSet( );                                                          //SPI从机模式设置
    while(1)
    {
#ifndef SPI_INTERRUPT
        ret = CH549SPISlvRead();                                               //主机保持CS=0
        CH549SPISlvWrite(ret^0xFF);                                            //SPI等待主机把数据取走,SPI 主机每次读之前先将CS=0，读完后CS=1
        printf("Read#%02x\n",(UINT16)ret);
#endif
    }
}
