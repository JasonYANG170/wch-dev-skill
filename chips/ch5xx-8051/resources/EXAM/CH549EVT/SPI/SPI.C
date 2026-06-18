/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/23
* Description        : CH549 SPI主、从模式接口函数
注：片选有效时，从机会自动加载SPI0_S_PRE的预置值到发送移位缓冲区，所以最好可以在片选
有效前向SPI0_S_PRE寄存器写入预发值，或者在主机端丢弃首个接收字节，发送时注意主机会先
取走SPI0_S_PRE里面的值产生一个S0_IF_BYTE中断。
如果片选从无效到有效，从机首先进行发送的话，最好把输出的首字节放到SPI0_S_PRE寄存器中；
如果已经处于片选有效的话，数据数据使用SPI0_DATA就可以
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\SPI\SPI.H"
#pragma  NOAREGS
/*******************************************************************************
* Function Name  : SPIMasterModeSet( UINT8 mode )
* Description    : SPI主机模式初始化
* Input          : UINT8 mode
* Output         : None
* Return         : None
*******************************************************************************/
void SPIMasterModeSet(UINT8 mode)
{
    SCS = 1;
    P1_MOD_OC &= ~(bSCS|bMOSI|bSCK);
    P1_DIR_PU |= (bSCS|bMOSI|bSCK);                                            //SCS,MOSI,SCK设推挽输出
    P1_MOD_OC |= bMISO;                                                        //MISO 上拉输入
    P1_DIR_PU |= bMISO;
    SPI0_SETUP = 0;                                                           //Master模式,高位在前
    if(mode == 0)
    {
        SPI0_CTRL = (bS0_MOSI_OE|bS0_SCK_OE);                                   //模式0
    }
    else if(mode == 3)
    {
        SPI0_CTRL = (bS0_MOSI_OE|bS0_SCK_OE|bS0_MST_CLK);                       //模式3
    }
}
/*******************************************************************************
* Function Name  : CH549SPIMasterWrite(UINT8 dat)
* Description    : CH549硬件SPI写数据,主机模式
* Input          : UINT8 dat   数据
* Output         : None
* Return         : None
*******************************************************************************/
void CH549SPIMasterWrite(UINT8 dat)
{
    SPI0_DATA = dat;
    while(S0_FREE == 0)
    {
        ;    //等待传输完成
    }
}
/*******************************************************************************
* Function Name  : CH549SPIMasterRead( )
* Description    : CH549硬件SPI0读数据，主机模式
* Input          : None
* Output         : None
* Return         : UINT8 ret
*******************************************************************************/
UINT8 CH549SPIMasterRead()
{
    SPI0_DATA = 0xff;
    while(S0_FREE == 0)
    {
        ;
    }
    return SPI0_DATA;
}
/*******************************************************************************
* Function Name  : SPISlvModeSet( )
* Description    : SPI从机模式初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPISlvModeSet( )
{
    P1_MOD_OC &= ~(bSCS|bMOSI|bSCK);                                          //SCS,MOSI,SCK 浮空输入
    P1_DIR_PU &= ~(bSCS|bMOSI|bSCK);
    P1_MOD_OC &= ~bMISO;                                                      //MISO推挽输出
    P1_DIR_PU |= bMISO;
    SPI0_S_PRE = 0x66;                                                        //预置值,任意值
    SPI0_SETUP = bS0_MODE_SLV;                                                //Slv模式,高位在前
    SPI0_CTRL = bS0_MISO_OE;                                                  //MISO 输出使能
#ifdef SPI_INTERRUPT
    SPI0_SETUP |= bS0_IE_FIRST | bS0_IE_BYTE;
    IE_SPI0 = 1;
    EA = 1;
#endif
}
/*******************************************************************************
* Function Name  : CH549SPISlvWrite(UINT8 dat)
* Description    : CH549硬件SPI写数据，从机模式
* Input          : UINT8 dat   数据
* Output         : None
* Return         : None
*******************************************************************************/
void CH549SPISlvWrite(UINT8 dat)
{
    SPI0_DATA = dat;
    while(S0_FREE==0)
    {
        ;
    }
}
/*******************************************************************************
* Function Name  : CH549SPISlvRead( )
* Description    : CH549硬件SPI0读数据，从机模式
* Input          : None
* Output         : None
* Return         : UINT8 ret
*******************************************************************************/
UINT8 CH549SPISlvRead()
{
    while(S0_FREE == 0)
    {
        ;
    }
    return SPI0_DATA;
}
#ifdef SPI_INTERRUPT
/*******************************************************************************
* Function Name  : SPIInterrupt(void)
* Description    : SPI 中断服务程序
*******************************************************************************/
void SPIInterrupt( void ) interrupt INT_NO_SPI0 using 1        //SPI中断服务程序,使用寄存器组1
{
    UINT8 dat;
    dat = CH549SPISlvRead();
    CH549SPISlvWrite(dat^0xFF);
#if DE_PRINTF
    printf("Read#%02x\n",(UINT16)dat);
#endif
}
#endif