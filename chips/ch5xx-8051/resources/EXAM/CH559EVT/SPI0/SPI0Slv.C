
/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI0Slv.C
* Author             : WCH
* Version            : V1.3
* Date               : 2019/07/22
* Description        : CH559提供SPI0从机模式操作接口函数 
注：片选有效时，从机会自动加载SPI0_S_PRE的预置值到发送移位缓冲区，所以最好可以在片选
有效前向SPI0_S_PRE寄存器写入预发值，或者在主机端丢弃首个接收字节，发送时注意主机会先
取走SPI0_S_PRE里面的值产生一个S0_IF_BYTE中断。
如果片选从无效到有效，从机首先进行发送的话，最好把输出的首字节放到SPI0_S_PRE寄存器中；
如果已经处于片选有效的话，数据数据使用SPI0_DATA就可以了。
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "..\DEBUG.C"                                                          //调试信息打印
#include "..\DEBUG.H"
#pragma  NOAREGS                                        

/*硬件接口定义*/
/******************************************************************************
使用CH559 硬件SPI接口 
         CH559        DIR       
         P1.4        <==>       SCS
         P1.5        <==>       MOSI
         P1.6        <==>       MISO
         P1.7        <==>       SCK
*******************************************************************************/





/*******************************************************************************
* Function Name  : CH559SPI0SlvWrite(UINT8 dat)
* Description    : CH559硬件SPI写数据，从机模式
* Input          : UINT8 dat   数据
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI0SlvWrite(UINT8 dat)
{
    SPI0_DATA = dat;
    while(S0_FREE==0)
    {
        ;
    }
}
/*******************************************************************************
* Function Name  : CH559SPI0SlvRead( )
* Description    : CH559硬件SPI0读数据，从机模式
* Input          : None
* Output         : None
* Return         : UINT8 ret
*******************************************************************************/
UINT8 CH559SPI0SlvRead()
{
    while(S0_FREE == 0)
    {
        ;
    }
    return SPI0_DATA;
}
/*******************************************************************************
* Function Name  : CH559SPI0InterruptInit()
* Description    : CH559SPI0中断初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI0InterruptInit()
{
    //IP_EX |= bIP_SPI0;                                                       //SPI0中断优先级设置
    //SPI0_SETUP |= bS0_IE_FIRST;                                              //使能首字节接收中断	
    //一般用于首字节是命令码
    SPI0_SETUP |=  bS0_IE_BYTE;                                                //使能接收1字节中断，使能FIFO溢出中断
    SPI0_CTRL |= bS0_AUTO_IF;                                                  //自动清S0_IF_BYTE中断标志
    SPI0_STAT |= 0xff;                                                         //清空SPI0中断标志
    IE_SPI0 = 1;                                                               //使能SPI0中断
	 EA  = 1;                                                                  //开启总中断
}

/*******************************************************************************
* Function Name  : CH559SPI0Init()
* Description    : CH559SPI0初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI0Init(void)
{
    SPI0_S_PRE = 0x59;                                                        //预置数据，用于SPI主机发出首字节命令码后得到的交换数据
    PORT_CFG &= ~bP1_OC;                                                 
    P1_DIR &= 0x0F;
    P1_PU &=  0x0F;                                                           //SCS,MISO,MOSI,SCK 引脚置为高阻输入,片选有效MISO自动使能输出
    SPI0_SETUP |= bS0_MODE_SLV;                                               //设置为设备模式，
    SPI0_SETUP &= ~	(bS0_BIT_ORDER);                                          //字节顺序为大端模式	
    SPI0_CTRL &= ~(bS0_MOSI_OE | bS0_SCK_OE);                                  //关闭SCK和MOSI输出
    SPI0_CTRL |=  bS0_MISO_OE;                                              
    SPI0_CTRL &= ~(bS0_MST_CLK | bS0_DATA_DIR | bS0_2_WIRE);                                
    SPI0_CTRL &= ~bS0_CLR_ALL;                                                //清空SPI0的FIFO,默认是1，必须置零才能发送数据
}

#ifdef SPI0Interrupt
/*******************************************************************************S
* Function Name  : SPI0HostInterrupt(void)
* Description    : SPI0 从机模式中断服务程序
* Input          : None
* Output         : None
* Return         : UINT8 ret   
*******************************************************************************/
void	SPI0HostInterrupt( void ) interrupt INT_NO_SPI0                //* SPI0中断服务程序,使用寄存器组1
{
	UINT8 dat;
    dat = CH559SPI0SlvRead();
    CH559SPI0SlvWrite(dat^0xFF);
    printf("Read#%02x\n",(UINT16)dat);
}
#endif

main( ) 
{
    UINT8 ret;
    mDelaymS(10);                                                              //上电延时,等待内部晶振稳定
//  CfgFsys( );  
    mInitSTDIO( );                                                      //串口0,可以用于调试
    printf("start ...\n");  
    CH559SPI0Init();
#ifdef SPI0Interrupt 
    CH559SPI0InterruptInit(); 
#endif 
	
    while(1)                                                             
    {                                                                     
#ifndef SPI0Interrupt
        ret = CH559SPI0SlvRead();                                               //主机保持CS=0
        CH559SPI0SlvWrite(ret^0xFF);                                            //SPI等待主机把数据取走,SPI 主机每次读之前先将CS=0，读完后CS=1
        printf("Read#%02x\n",(UINT16)ret);
#endif
    }			
}