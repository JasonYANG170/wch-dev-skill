
/********************************** (C) COPYRIGHT *******************************
* File Name          : SPI0Flash.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : CH559 SPI0 读写外部Flash
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\DEBUG.C"                                                          //调试信息打印
#include ".\DEBUG.H"

#pragma  NOAREGS

sbit CHIP_SELECT = P1^4;
#define SENDBYTE_SPI( d )    {  SPI0_DATA = d;while(S0_FREE == 0); }
#define RECVBYTE_SPI( d )    { SPI0_DATA = 0xff;while(S0_FREE == 0);d = SPI0_DATA;}

UINT8 buf[50]; 

/*******************************************************************************
* Function Name  : InitHostSPI0( void )
* Description    : SPI0主机模式初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void    InitHostSPI0( void )
{
    SPI0_SETUP &=~(bS0_MODE_SLV | bS0_BIT_ORDER);                              /*设置成主机模式*/
    SPI0_CTRL = bS0_SCK_OE | bS0_MOSI_OE;                                      /*主机写，默认不启动写传输，如果使能bS0_DATA_DIR*/
                                                                               /*那么发送数据后自动产生一个字节的时钟，用于快速数据收发*/
    P1_DIR |= (bMOSI | bSCK | bSCS| bPWM3 );                                   /*bMOSI 、bSCK 、bSCS置为输出方向*/
    P1_DIR &= ~bMISO;
    SPI0_CK_SE = 0x02;                                                         /*分频为6M*/
//  SPI0_STAT = 0xFF;                                                          /*清中断标志*/
//  IE_SPI0 = 1;
}

/*******************************************************************************
* Function Name  : ReadExternalFlashStatusReg_SPI
* Description    : 用来读取状态寄存器,并返回状态寄存器的值
* Input          : None
* Output         : None
* Return         : ExFlashRegStatus
*******************************************************************************/
UINT8 ReadExternalFlashStatusReg_SPI( void )
{
    UINT8 ExFlashRegStatus;
    CHIP_SELECT = 0;
    SENDBYTE_SPI(0x05);                                                        /*发送读状态寄存器的命令 */
    RECVBYTE_SPI(ExFlashRegStatus);                                            /*读取状态寄存器*/
    CHIP_SELECT = 1 ;
    return ExFlashRegStatus;
}
   
/*******************************************************************************
* Function Name  : WaitExternalFlashIfBusy
* Description    : 等待芯片空闲(在执行Byte-Program, Sector-Erase, Block-Erase, Chip-Erase操作后)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WaitExternalFlashIfBusy( void )
{
    while ((ReadExternalFlashStatusReg_SPI())&0x01 == 0x01 )                   /*等待直到Flash空闲*/
    {
        ReadExternalFlashStatusReg_SPI( );
    }
}

/*******************************************************************************
* Function Name  : WriteExternalFlashStatusReg_SPI
* Description    : 往状态寄存器里写一个字节
* Input          : status -写入的数据
* Output         : None
* Return         : None
*******************************************************************************/
void WriteExternalFlashStatusReg_SPI( UINT8 status )
{
    CHIP_SELECT = 0 ;
    SENDBYTE_SPI(0x01);                                                       /*发送写状态寄存器*/
    SENDBYTE_SPI(status);                                                     /*改变寄存器里BPx或者BPL (只有2,3,4,5,7位可以改写)*/
    CHIP_SELECT = 1 ;
}
 
/*******************************************************************************
* Function Name  : WriteExternalFlashEnable_SPI
* Description    : 写使能,同样可以用于使能写状态寄存器
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WriteExternalFlashEnable_SPI( void )
{
    CHIP_SELECT = 0 ;
    SENDBYTE_SPI(0x06);                                                       /*发送写使能命令*/
    CHIP_SELECT = 1 ;
}

/*******************************************************************************
* Function Name  : CheckExternalFlashWriteEnable_SPI
* Description    : 检查擦写操作前WEL位是否为1
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CheckExternalFlashWriteEnable_SPI( void )
{
    UINT8 WRENStatus;
    WRENStatus = ReadExternalFlashStatusReg_SPI();                            /*读取状态register*/
    if ((WRENStatus&0x02) != 0x02)                                            /*检查WEL位置位*/
    {
        WriteExternalFlashEnable_SPI( );                                      /*如果未置1进行相应处理,如对其进行写使能操作*/
    }
}

/*******************************************************************************
* Function Name  : EraseExternalFlash_SPI
* Description    : 擦除外部Flash
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void EraseExternalFlash_SPI( void )
{
    CheckExternalFlashWriteEnable_SPI();
    CHIP_SELECT = 0 ;
    SENDBYTE_SPI(0x60);                                                        /*发送 Chip Erase命令 (60h or C7h)*/
    CHIP_SELECT = 1 ;
    WaitExternalFlashIfBusy();
}

/*******************************************************************************
* Function Name  : ByteReadExternalFlash_SPI
* Description    : 读取一个地址内一个字节的数据.返回读取的数据 
* Input          : UINT32 StarAddr -Destination Address 000000H - 1FFFFFH
* Output         : None
* Return         : byte -读取的数据
*******************************************************************************/
UINT8 ByteReadExternalFlash_SPI(UINT32 StarAddr)    
{   
    UINT8 dat = 0;    
    CHIP_SELECT = 0 ;                                                           //enable device  
    SENDBYTE_SPI(0x03);                                                         //read command 
    SENDBYTE_SPI(((StarAddr & 0xFFFFFF) >> 16));                                //send 3 address bytes  
    SENDBYTE_SPI(((StarAddr & 0xFFFF) >> 8));
    SENDBYTE_SPI(StarAddr & 0xFF);
    RECVBYTE_SPI(dat);   
    CHIP_SELECT = 1 ;                                                           //disable device   
    return dat;                                                                 //return one byte read
} 

/*******************************************************************************
* Function Name  : ByteWriteExternalFlash_SPI
* Description    : 写数据
* Input          : StarAddr  -Destination Address 000000H - 1FFFFFH
*                  dat -要写入的数据
* Output         : None
* Return         : None
*******************************************************************************/
void ByteWriteExternalFlash_SPI(UINT32 StarAddr, UINT8 dat)
{
    WriteExternalFlashEnable_SPI();
    CHIP_SELECT = 0 ;                                                          //芯片使能 
    SENDBYTE_SPI(0x02);                                                        //发送写操作指令 
    SENDBYTE_SPI(((StarAddr & 0xFFFFFF) >> 16));                                    //发送3字节地址 
    SENDBYTE_SPI(((StarAddr & 0xFFFF) >> 8));
    SENDBYTE_SPI(StarAddr & 0xFF);
    SENDBYTE_SPI(dat);                                                         //发送要写的数据
    CHIP_SELECT = 1 ;
    WaitExternalFlashIfBusy();
}

/*******************************************************************************
* Function Name  : BlukReadExternalFlash_SPI
* Description    : 读取起始地址(StarAddr)内多个字节(Len)的数据.存入缓冲区RcvBuffer中
* Input          : StarAddr -Destination Address 000000H - 1FFFFFH
                   Len 读取数据长度
                   RcvBuffer 接收缓冲区起始地址
* Output         : None
* Return         : None
*******************************************************************************/
void BlukReadExternalFlash_SPI(UINT32 StarAddr,UINT16 Len,PUINT8 RcvBuffer)
{
    UINT16 i;
    for(i=0; i<Len; i++)                                                        /*接收数据*/
    {
        RcvBuffer[i] = ByteReadExternalFlash_SPI(StarAddr);
        StarAddr++;                                                             /*读取下一地址*/
    }
}

/*******************************************************************************
* Function Name  : BlukWriteExternalFlash_SPI
* Description    : 将数据写入外部Flash
* Input          : StarAddr  -Destination Address 000000H - 1FFFFFH
                   Len 发送数据长度
*                  SendBuffer -发送数据缓冲区
* Output         : None
* Return         : None
*******************************************************************************/
void BlukWriteExternalFlash_SPI(UINT32 StarAddr,UINT16 Len,PUINT8 SendBuffer)
{
    UINT16 i;
    for(i=0; i<Len; i++)                                                        /*发送要写的数据*/
    {
        ByteWriteExternalFlash_SPI(StarAddr,*(SendBuffer+i)); 
        StarAddr++;			
    }
}

void main( ) 
{
    UINT32  i;
//  mDelaymS(30);                                                                  //上电延时,等待内部晶振稳定
    mInitSTDIO( );                                                                 /* 为了让计算机通过串口监控演示过程 */
    printf( "Start SPI FLASH @ChipID=%02X\n", (UINT16)CHIP_ID );

    InitHostSPI0( );
#if  0	
    printf("Address(0xF8) = %02x\n",(UINT16)SPI0_STAT);
    printf("Address(0xF9) = %02x\n",(UINT16)SPI0_DATA);
    printf("Address(0xFA) = %02x\n",(UINT16)SPI0_CTRL);
    printf("Address(0xFB) = %02x\n",(UINT16)SPI0_CK_SE);
    printf("Address(0xFC) = %02x\n",(UINT16)SPI0_SETUP);	
#endif
    WriteExternalFlashEnable_SPI( );                                               //FLASH写使能
    WriteExternalFlashStatusReg_SPI( 0x00 );                                       //写寄存器 
    EraseExternalFlash_SPI( );                                                     //FLASH整片擦除
    printf("Chip_Erase over\n");
    for(i = 0 ;i < 50 ;i ++)
    {
        buf[i] = i;                                                
    }
    BlukWriteExternalFlash_SPI(0,50,&buf[0]);                                      //在FLASH地址0x00000000写入数据
    printf("Write over\n");
    BlukReadExternalFlash_SPI(0,50,&buf[0]);                                       //从FLASH地址0x00000000读出数据
    for(i = 0 ;i < 50 ;i ++)
    {
        printf(" %02x  ",(UINT16)buf[i]);                                               
    }
	  while(1);
}
