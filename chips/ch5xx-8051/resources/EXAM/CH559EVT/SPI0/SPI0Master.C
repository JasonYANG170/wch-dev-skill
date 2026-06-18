
/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI0Mater.C
* Author             : WCH
* Version            : V1.3
* Date               : 2019/07/22
* Description        : CH559提供SPI0主机模式操作接口函数             				   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "..\DEBUG.C"                                                          //调试信息打印
#include "..\DEBUG.H"

#define SPI0Interrupt   0                                                      //设定SPI0数据收发中断方式或者查询方式
UINT8 flag;
UINT8 TmpBuf;

#pragma  NOAREGS
#define SET_SPI0_CK( d )   { SPI0_CK_SE = d; }                                 //d>=2


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
* Function Name  : CH559SPI0HostInit()
* Description    : CH559SPI0初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI0HostInit(void)
{
    PORT_CFG &= ~bP1_OC;
    P1_DIR |= (bSCK | bMOSI | bSCS);
    P1_IE |= bMISO;                                                            //引脚配置
	
    SPI0_SETUP &= ~(bS0_MODE_SLV | bS0_BIT_ORDER);                             //设置为主机模式，字节顺序为大端模式		
    SPI0_CTRL |=  bS0_MOSI_OE  | bS0_SCK_OE ;                                  //MISO输出使能，SCK输出使能
    SPI0_CTRL &= ~(bS0_MST_CLK | bS0_2_WIRE);
    SPI0_CTRL &=  ~(bS0_DATA_DIR);                                             //主机写，默认不启动写传输，如果使能bS0_DATA_DIR，
	                                                                             //那么发送数据后自动产生一个字节的时钟，用于快速数据收发	
    SET_SPI0_CK(6);                                                              //6分频
    SPI0_CTRL &= ~bS0_CLR_ALL;                                                 //清空SPI0的FIFO,默认是1，必须置零才能发送数据
}

/*******************************************************************************
* Function Name  : CH559SPI0Write(UINT8 dat)
* Description    : CH559硬件SPI写数据
* Input          : UINT8 dat   数据
* Output         : None
* Return         : None
*******************************************************************************/
void CH559SPI0Write(UINT8 dat)
{
    SPI0_DATA = dat;                                                           
    while(S0_FREE == 0);													   //等待传输完成		
//如果bS0_DATA_DIR为1，此处可以直接读取一个字节的数据用于快速读写	
}

/*******************************************************************************
* Function Name  : CH559SPI0Read( )
* Description    : CH559硬件SPI0读数据
* Input          : None
* Output         : None
* Return         : UINT8 ret   
*******************************************************************************/
UINT8 CH559SPI0Read()
{
    SPI0_DATA = 0xff;
    while(S0_FREE == 0);
    return SPI0_DATA;
}



void main()
{
    UINT8 ret,i=0;
    mDelaymS(30);                                                              //上电延时,等待内部晶振稳定,必加 
//  CfgFsys( );     
    mInitSTDIO( );                                                             //串口0,可以用于调试
    printf("start ...\n");  
    CH559SPI0HostInit();                                                       //SPI0主机模式初始化
	mDelaymS(100);
    while(1)
    {   
	     SCS = 0;                                                               //SPI主机发送数据
        CH559SPI0Write(i);
        mDelaymS(5);
        ret = CH559SPI0Read();                                            //接收SPI从机返回的数据，取反返回
        SCS = 1;
        if(ret != (i^0xff))
        {
            printf("Err: %02X  %02X  \n",(UINT16)i,(UINT16)ret);               //如果不等于发送数据的取反，打印错误信息
        }
        else
        {
            printf("success %02x\n",(UINT16)i);                               
        }
        i = i+1;
        mDelaymS(50);
    }
}