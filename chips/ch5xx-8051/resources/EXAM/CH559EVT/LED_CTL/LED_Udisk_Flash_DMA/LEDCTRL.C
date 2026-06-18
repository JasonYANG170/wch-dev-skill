
/********************************** (C) COPYRIGHT *********************************
* File Name          : LEDCTRL.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : CH559的DMA方式实现LED控制，可以使用U盘更新屏显内容
                       支持U盘热插拔
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

/*********************************头文件包含**************************************/
#include <stdio.h>
#include <string.h>
#include "../../CH559.H"                                                    //CH559头文件
#include "../../DEBUG.H"                                                    //串口模块
#include "../../DEBUG.C"
#include "ROOT_UDISKIF.H"                                                   //U盘接口模块
#include "ROOT_UDISKIF.C"

#pragma  NOAREGS

/*********************************头文件包含**************************************/
#define   ScreenLength      1024                                            //屏幕的长度，即列数(bit)
#define   ScreenWidth       16                                              //屏幕的宽度，即行数(bit)
#define   UnicodeSize       2048                                            //ScreenLength*ScreenWidth/8 从U盘中读取的字符数
#define   LargeUnicodeSize  1024                                            //字节
#define   SingleSendSize    128                                             //ScreenLength/8 LED的DMA单次发送的数据长度

#ifndef DEBUG                                                               //打印调试信息
#define DEBUG 1
#endif

// #if LargeUnicodeSize>=UnicodeSize
// UINT8X LEDBuffer[CountUnicode]  _at_ 0x0000;                             //LED DMA发送缓冲区
// #else
UINT8X LEDBuffer[LargeUnicodeSize]  _at_ 0x0000;                            //LED DMA发送缓冲区
UINT8X LEDBuffer1[LargeUnicodeSize]  _at_ (0x0000+LargeUnicodeSize);        //LED DMA发送缓冲区
// #endif

sbit LA   = P2^0;
sbit LB   = P2^1;
sbit LC   = P2^2;
sbit LD   = P2^3;
sbit EN   = P2^4;
sbit STB  = P2^5;
#define EN_L( )       { EN  = 0; }                             
#define EN_H( )       { EN  = 1; }  
#define STB_L( )      { STB = 0; }                            
#define STB_H( )      { STB = 1; }
#define LINE0         { LD=0;LC=0;LB=0;LA=0; } 
#define LINE1         { LD=0;LC=0;LB=0;LA=1; }
#define LINE2         { LD=0;LC=0;LB=1;LA=0; }
#define LINE3         { LD=0;LC=0;LB=1;LA=1; }
#define LINE4         { LD=0;LC=1;LB=0;LA=0; }
#define LINE5         { LD=0;LC=1;LB=0;LA=1; }
#define LINE6         { LD=0;LC=1;LB=1;LA=0; }
#define LINE7         { LD=0;LC=1;LB=1;LA=1; }
#define LINE8         { LD=1;LC=0;LB=0;LA=0; }
#define LINE9         { LD=1;LC=0;LB=0;LA=1; }
#define LINE10        { LD=1;LC=0;LB=1;LA=0; }
#define LINE11        { LD=1;LC=0;LB=1;LA=1; }
#define LINE12        { LD=1;LC=1;LB=0;LA=0; }
#define LINE13        { LD=1;LC=1;LB=0;LA=1; }
#define LINE14        { LD=1;LC=1;LB=1;LA=0; }
#define LINE15        { LD=1;LC=1;LB=1;LA=1; }


/******************************SPI0 Flash**************************************/
/*******************************************************************************
* Function Name  : InitSPI_Host( void )
* Description    : SPI0主机模式初始化
* Input          : None 
* Output         : None
* Return         : None
*******************************************************************************/
void	InitSPI_Host( void )
{
    SPI0_SETUP &=~(bS0_MODE_SLV | bS0_BIT_ORDER) ;                            // 设置成主机模式
    SPI0_CTRL = bS0_SCK_OE | bS0_MOSI_OE;                                     //主机写，默认不启动写传输，如果使能bS0_DATA_DIR，
                                                                              //那么发送数据后自动产生一个字节的时钟，用于快速数据收发	
    P1_DIR |= (bMOSI | bSCK | bSCS| bPWM3 );			                            //bMOSI 、bSCK 、bSCS置为输出方向
    P1_DIR &= ~bMISO;
    SPI0_CK_SE = 0x02;	                                                      //分频为6M
//	SPI0_STAT = 0xFF;                                                         // 清中断标志
//	IE_SPI0 = 1;
}

/*******************************************************************************
* Function Name  : SPI0_MASTER_Trans
* Description    : 发送一字节数据
* Input          : buffer -待发送数据 
* Output         : None
* Return         : None
*******************************************************************************/
void SPI0_MASTER_Trans( UINT8 dat )
{
    SPI0_DATA = dat;
    while( S0_FREE == 0 );                                                    //等待数据发送完成
}

/*******************************************************************************
* Function Name  : SPI0_MASTER_Recv
* Description    : 接收一字节数据
* Input          : None
* Output         : None
* Return         : 接收到数据
*******************************************************************************/
UINT8 SPI0_MASTER_Recv( void )
{
    SPI0_DATA = 0xFF;
    while(  S0_FREE == 0 );                                                   //等待数据回来	
    return SPI0_DATA;
}

/*******************************************************************************
* Function Name  : Read_Status_Register
* Description    : 用来读取状态寄存器,并返回状态寄存器的值,为保证速度，所以不调用函数
* Input          : None
* Output         : None
* Return         : SPI0_DATA -寄存器状态值
*******************************************************************************/
UINT8 Read_Status_Register( void )   
{   
    UINT8 byte = 0;
    SCS = 0 ;                                                                  //使能设备 
    SPI0_DATA = 0x05;                                                          //发送读状态寄存器的命令 
    while( S0_FREE == 0 );                                                     //等待数据发送完成  
    SPI0_DATA = 0xFF; 
    while(  S0_FREE == 0 );                                                    //读取状态寄存器  
    SCS = 1 ;                                                                  //禁止设备    
    return SPI0_DATA;   
}
   
/*******************************************************************************
* Function Name  : Wait_Busy
* Description    : 等待芯片空闲(在执行Byte-Program, Sector-Erase, Block-Erase, Chip-Erase操作后)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Wait_Busy( void )   
{   
    while ((Read_Status_Register())&0x01 == 0x01 )                              //waste time until not busy   
          Read_Status_Register( );   
}

/*******************************************************************************
* Function Name  : WRSR
* Description    : 往状态寄存器里写一个字节
* Input          : byte -写入的数据
* Output         : None
* Return         : None
*******************************************************************************/
void WRSR( UINT8 byte )   
{   
    SCS = 0 ;                                                                  //使能设备 
    SPI0_MASTER_Trans(0x01);                                                   //发送写状态寄存器   
    SPI0_MASTER_Trans(byte);                                                   //改变寄存器里BPx或者BPL (只有2,3,4,5,7位可以改写)   
    SCS = 1 ;                                                                  //禁止设备  
}
 
/*******************************************************************************
* Function Name  : WREN
* Description    : 写使能,同样可以用于使能写状态寄存器 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WREN( void )   
{   																	                                         //SCS与下载冲突使用AIN3替代
   SCS = 0 ;         
   SPI0_MASTER_Trans(0x06);                                                    //发送WREN命令  
   SCS = 1 ;             
}

/*******************************************************************************
* Function Name  : WREN_Check
* Description    : 检查擦写操作前WEL位是否为1 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void WREN_Check( void )   
{   
    UINT8 byte;   
    byte = Read_Status_Register( );                                             //读取状态register   
    if ((byte&0x02) != 0x02)                                                    //检查WEL位置位   
    {   
        WREN( );                                                                //如果未置1进行相应处理,如对其进行写使得操作 
    }   
}

/*******************************************************************************
* Function Name  : Chip_Erase
* Description    : 擦除
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Chip_Erase( void )   
{                          
    WREN_Check();   
    SCS = 0 ;            
    SPI0_MASTER_Trans(0x60);                                                    //发送 Chip Erase命令 (60h or C7h)   
    SCS = 1 ;  
    Wait_Busy();   
}

/*******************************************************************************
* Function Name  : FlashRead
* Description    : 读取起始地址内SingleSendSize个字节的数据.返回读取的数据,高速读 
* Input          : Dst -Destination Address 000000H - 1FFFFFH
                   buffer 接收缓冲区起始地址
* Output         : None
* Return         : None
*******************************************************************************/
void FlashRead(UINT32 Dst,PUINT8 buffer)    
{   
    UINT8 i;    
    SCS = 0 ;                                                                   //enable device  
    SPI0_DATA = 0x03;                                                           //read command 
    while( S0_FREE == 0 );                                                      //等待数据发送完成 
    SPI0_DATA = ((Dst & 0xFFFFFF) >> 16);                                       //send 3 address bytes
    while( S0_FREE == 0 );                                                      //等待数据发送完成 
    SPI0_DATA = ((Dst & 0xFFFF) >> 8);                                       
    while( S0_FREE == 0 );                                                      	
    SPI0_DATA = Dst & 0xFF;                                       
    while( S0_FREE == 0 ); 
    for(i=0;i<SingleSendSize;i++)
    {                                                                           //等待数据返回 
        SPI0_DATA = 0xFF;
        while(  S0_FREE == 0 );                                                  
        *(buffer+i) = SPI0_DATA;			
    }				 
    SCS = 1 ;                                                                   //disable device   
} 

/*******************************************************************************
* Function Name  : Byte_Program
* Description    : 写数据
* Input          : Dst  -Destination Address 000000H - 1FFFFFH
*                  byte -要写入的数据
* Output         : None
* Return         : None
*******************************************************************************/
void Byte_Program(UINT32 Dst, UINT8 byte)
{
    WREN();
    SCS = 0 ;                                                                    //芯片使能 
    SPI0_MASTER_Trans(0x02);                                                     //发送写操作指令 
    SPI0_MASTER_Trans(((Dst & 0xFFFFFF) >> 16));                                 //发送3字节地址 
    SPI0_MASTER_Trans(((Dst & 0xFFFF) >> 8));
    SPI0_MASTER_Trans(Dst & 0xFF);
    SPI0_MASTER_Trans(byte);                                                     //发送要写的数据
    SCS = 1 ;
    Wait_Busy();
}

/*******************************************************************************
* Function Name  : CopyData2Flash(UINT16 Num,UINT16 Addr)
* Description    : 读出汉字对应的字码存入Flash
* Input          : UINT16 Num 本次写入Flash的字节数
                   UINT16 Addr 本次写入Flash的起始地址
* Output         : None
* Return         : None
*******************************************************************************/
void CopyData2Flash(UINT16 Num,UINT16 Addr)
{
		UINT16 i;

		for(i=0;i<Num;i++)
		{
            Byte_Program( Addr+i ,LEDBuffer1[i] );                                  
		}
}	
/****************************SPI0 Flash END*************************************/

/*****************************行选和IO配置**************************************/
/*******************************************************************************
* Function Name  : InitLED(void)
* Description    : LED
* Input          : None 
* Output         : None
* Return         : None
*******************************************************************************/
void InitLED() 
{
    P2_DIR = 0xff;                                                    
    P3_DIR = 0xff;                                                      
    //P4_DIR = 0xff;                                                     
    PORT_CFG |= bP2_DRV | bP3_DRV;
    EN_H( );
    STB_L( );
    LED_CTRL  =  0x00|bLED_OUT_EN |bLED_BIT_ORDER;      
    LED_CK_SE=0x06;
}

/*******************************************************************************
* Function Name  : Showline(UINT8 LineNum) 
* Description    : LED
* Input          : UINT8 LineNum 
* Output         : None
* Return         : None
*******************************************************************************/
void Showline(UINT8 LineNum)                                     
{
    switch(LineNum)                                                     
	  {
         case  0: LINE0;break;
         case  1: LINE1;break;
         case  2: LINE2;break;
         case  3: LINE3;break;
         case  4: LINE4;break;
         case  5: LINE5;break;
         case  6: LINE6;break;
         case  7: LINE7;break;
         case  8: LINE8;break;
         case  9: LINE9;break;
         case 10:LINE10;break;
         case 11:LINE11;break;
         case 12:LINE12;break;
         case 13:LINE13;break;
         case 14:LINE14;break;
         case 15:LINE15;break;
         default:break;
    }
}
/******************************IO end*******************************************/

/*******************************************************************************
* Function Name  : ReadDisplayFile()
* Description    : 读取U盘中需要显示的汉字字符(每个汉字对应2个字符)，存储至Flash
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ReadDisplayFile( )        
{
    UINT8 s,i;
    UINT16 len;
	
	WREN( );                                                                     //FLASH写使能
	WRSR( 0x00 );                                                                //写寄存器 
	Chip_Erase( );                                                               //FLASH整片擦除

    strcpy( mCmdParam.Open.mPathName, "/COMMAND.TXT" );                          //设置需要打开的文件名 
    s = CH559FileOpen( );  
    if ( s == ERR_MISS_DIR || s == ERR_MISS_FILE )    
    {
        printf( "Miss File,Please Check the file name...\n" );
    }
    else    
    {
      mCmdParam.ByteLocate.mByteOffset = 602;                                  //设置偏移指针
      s = CH559ByteLocate();
      mStopIfError( s );
      for(i=0;i<(UnicodeSize/LargeUnicodeSize);i++)                            //长度大于LargeUnicodeSize
      {                                                                        //每次读LargeUnicodeSize
          mCmdParam.ByteRead.mByteCount = LargeUnicodeSize;  
          mCmdParam.ByteRead.mByteBuffer = LEDBuffer1;                         //设置读文件缓冲区地址
          s = CH559ByteRead( );
          mStopIfError( s );
          printf("%02X  \n",(UINT16)i);
#if DEBUG
          for(len=0;len<LargeUnicodeSize;len++)
          {
              printf("%02X  ",(UINT16)LEDBuffer1[len]);
          }	
#endif					
          CopyData2Flash(LargeUnicodeSize,i*LargeUnicodeSize);                 //写入Flash         
      }	
      mCmdParam.ByteRead.mByteCount = UnicodeSize%LargeUnicodeSize;            //读剩余字节  
      mCmdParam.ByteRead.mByteBuffer = LEDBuffer1;                             //设置读文件缓冲区地址
      s = CH559ByteRead( );
      mStopIfError( s );
#if DEBUG
      for(len=0;len<(UnicodeSize%LargeUnicodeSize);len++)
      {
          printf("%02X  ",(UINT16)LEDBuffer1[len]);
      }	
#endif			
      CopyData2Flash(UnicodeSize%LargeUnicodeSize,i*LargeUnicodeSize);         //写入Flash
  } 	
  mCmdParam.Close.mUpdateLen = 0;                                              //禁止更新文件长度
  s = CH559FileClose( );                                                       //关闭文件
  mStopIfError( s );		    
}	

/*******************************************************************************
* Function Name  : SendLeddata(UINT8 lineNum)
* Description    : DMA方式发送列数据
* Input          : UINT8 lineNum
* Output         : None
* Return         : None
*******************************************************************************/	
void SendLeddata(UINT8 lineNum)  
{
    UINT8 i,len;
    len = SingleSendSize/2;
    i=0;
	
    if(lineNum%2 == 0)
    {
        LED_DMA = LEDBuffer1; 
        LED_DMA_CN = len;                                                      //由于DMA是双字的，所以这里除以2
        LED_CTRL |=  bLED_DMA_EN;  
        			
        FlashRead((UINT16)SingleSendSize*((lineNum+1)%ScreenWidth),LEDBuffer); //从FLASH读出数据	
    }
    else
    {
        LED_DMA = LEDBuffer; 
        LED_DMA_CN = len;                                                       //由于DMA是双字的，所以这里除以2 
        LED_CTRL |=  bLED_DMA_EN;	  

        FlashRead((UINT16)SingleSendSize*((lineNum+1)%ScreenWidth),LEDBuffer1);//从FLASH读出数据				
    }
    while(LED_FIFO_CN||!(LED_STAT&bLED_FIFO_EMPTY));		
    LED_CTRL &= ~ bLED_DMA_EN ;
#if 0
    for(i=0;i<SingleSendSize;i++)
    {
        if(lineNum%2 == 0)
        {
            printf("%02X  ",(UINT16)LEDBuffer[i]);
        }
        else
        {
            printf("%02X  ",(UINT16)LEDBuffer1[i]);            
        }	
    }				
    printf("\n");	
#endif		
}


/*******************************************************************************
* Function Name  : Leddisplay(void)
* Description    : 屏幕显示函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/	
void Leddisplay( )  
{
    UINT8 i;
#if 0 	
    printf("benin\n");	
#endif
    for(i=0;i<16;i++)
    { 		
		STB_H();
		mDelayuS(2);
		STB_L(); 
		SendLeddata(i);
		EN_L();		 
		Showline(i);
		mDelayuS(500);
        EN_H();			
    } 
}

/**********************************主函数***************************************/
void main( )  
{
    UINT8 s,i;
    mDelaymS(30);                                                              //上电延时,等待内部晶振稳定,必加          
    mInitSTDIO( );                                                             //为了让计算机通过串口监控演示过程
    printf("Start LED contol....\n");
    InitUSB_Host( );
    InitLED( );
    InitSPI_Host( );
    CH559LibInit( );                                                            //初始化CH559程序库以支持U盘文件
    FoundNewDev = 0;  
    UIF_DETECT=0;
    FlashRead(0,LEDBuffer1);                                                    //从FLASH读出第一行的数据
    while(1)
    {
        if ( UIF_DETECT )                                                       //检测U口设备插拔
        {
            UIF_DETECT = 0;                                                     //清中断标志
            s = AnalyzeRootHub( );                                              //分析ROOT-HUB状态
            if( s == ERR_USB_CONNECT )
            {
                FoundNewDev = 1;
            }
            if( FoundNewDev || s == ERR_USB_CONNECT )                           //有新的USB设备插入
            {
                FoundNewDev = 0;
                mDelaymS( 200 );                                                //由于USB设备刚插入尚未稳定,故等待USB设备数百毫秒,消除插拔抖动
                s = InitRootDevice( );                                          //初始化USB设备
                if( s == ERR_SUCCESS )
                {
                    // U盘操作流程：USB总线复位、U盘连接、获取设备描述符和设置USB地址、可选的获取配置描述符，之后到达此处，由CH559子程序库继续完成后续工作
                    CH559DiskStatus = DISK_USB_ADDR;
                    for( i = 0; i != 10; i ++ )
                    {
                        s = CH559DiskReady( );
                        if ( s == ERR_SUCCESS )
                        {
                            break;
                        }
                        mDelaymS( 50 );
                    }
                    if( CH559DiskStatus >= DISK_MOUNTED )                       //U盘准备好
                    {
                        printf("Read Command File....\n");
                        ReadDisplayFile();
                        printf("Finish Reading File\n");
                    }
                    else
                    {
                        printf( "U_Disk not ready ERR =%02X\n", (UINT16)s );
                    }
                 }
                 else
                 {
                     printf("U_Disk Init Failed,Please retry again\n");
                 }
             } 
             SetUsbSpeed( 1 );                                                  // 默认为全速	
        }
  		Leddisplay( );                                                          //静态显示函数
    }
}




