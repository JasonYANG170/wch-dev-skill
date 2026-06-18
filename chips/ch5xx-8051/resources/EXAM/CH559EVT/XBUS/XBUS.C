/********************************** (C) COPYRIGHT *******************************
* File Name          : XBUS.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : CH559的总线操作
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#include ".\DEBUG.C"                                                          //调试信息打印
#include ".\DEBUG.H"

#pragma  NOAREGS

#define CS1 0
#define CS0 1

#if CS0                                                                       //向指定地址写入数据
#define XBUSWrite( addr, dat ) { *((volatile PUINT8X)(XDATA_XBUS_CS0 | addr)) = dat; }
#define XBUSRead( addr ) *((volatile PUINT8X)(XDATA_XBUS_CS0 | addr))         //读总线数据
#endif
#if CS1                                                                       //向指定地址写入数据
#define XBUSWrite( addr, dat ) { *((volatile PUINT8X)(XDATA_XBUS_CS1 | addr)) = dat; }
#define XBUSRead( addr ) *((volatile PUINT8X)(XDATA_XBUS_CS1 | addr))          //读总线数据
#endif


/*******************************************************************************
* Function Name  : CH559XBUSDirectInit()
* Description    : CH559 XBUS直接地址方式初始化
                   P3.6  <==>  #WR
                   P3.7  <==>  #RD
                   P3.3  <==>  #CS1  或  P3.4  <==>  #CS0
                   P0    <==>  D0-D7
                   P4.0-P4.5&P3.5&P2.7<==>  A0-A7
                   P2    <==>  A8-A15
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH559XBUSDirectInit()
{
    PORT_CFG |= bP0_OC | bP2_OC |bP3_OC;
    P2_DIR |= 0xff;
    P0_DIR |= 0xff;                                                            
    P3_DIR |= bRD | bWR| bDA6;
    P0_PU |= 0xff;
    P2_PU |= 0xff;															                               //P0和P2设置双向引脚
    P3_PU |= bRD | bWR | bDA6;										                             //WR\RD\CS1\A6设置为双向引脚
    P4_DIR |= 0x3f;                                                            //P4.0-P4.5允许输出
    PIN_FUNC |= bXBUS_EN | bXBUS_AH_OE | bXBUS_AL_OE;                          //使能直接总线方式	
    PIN_FUNC |= bXBUS_CS_OE;                                                   //CS使能
}

/*******************************************************************************
* Function Name  : CH559XBUSALEInit()
* Description    : CH559 XBUS ALE锁存方式初始化
                   P3.6  <==>  #WR
                   P3.7  <==>  #RD
                   P3.3  <==>  #CS1或P3.4  <==>  #CS0
                   P0    <==>  D0-D7
                   P0    <==>  A0-A7
                   P2    <==>  A8-A15
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH559XBUSALEInit()
{
    PORT_CFG |= bP0_OC | bP2_OC |bP3_OC;
    P2_DIR |= 0xff;
    P0_DIR |= 0xff;                                                            
    P3_DIR |= bRD | bWR;                                        
    P0_PU |= 0xff;
    P2_PU |= 0xff;															                               //P0和P2设置双向引脚
    P3_PU |= bRD | bWR;										                                     //WR\RD\CS1设置为双向引脚
    PIN_FUNC |= bXBUS_EN | bXBUS_AH_OE;                                        //P2作为地址高位
    PIN_FUNC &= ~bXBUS_AL_OE;                                                  //P0口为地址低位和数据位复用	
    PIN_FUNC |= bXBUS_CS_OE;                                                   //CS使能
}

/*******************************************************************************
* Function Name  : XBUSSpeedSetup( )
* Description    : XBUS速度设置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void XBUSSpeedSetup( )
{
#if CS1                                                                      //CS1作为片选
    	XBUS_SPEED |= bXBUS1_SETUP;                                            //总线建立时间，为0则2个时钟，为1则3个时钟
    	XBUS_SPEED |= bXBUS1_HOLD;                                             //总线保持时间，为0则1个时钟，为1则2个时钟
    	XBUS_SPEED |= bXBUS1_WIDTH1 | bXBUS1_WIDTH0;                           //总线读写有效脉宽，00\01\10\11分别对应2\4\8\16个时钟周期
#endif
#if CS0                                                                      //CS0作为片选
    	XBUS_SPEED |= bXBUS0_SETUP;                                            //总线建立时间，为0则2个时钟，为1则3个时钟
    	XBUS_SPEED |= bXBUS0_HOLD;                                             //总线保持时间，为0则1个时钟，为1则2个时钟
    	XBUS_SPEED |= bXBUS0_WIDTH1 | bXBUS0_WIDTH0;                           //总线读写有效脉宽，00\01\10\11分别对应2\4\8\16个时钟周期    	
#endif
}

main( ) 
{
    UINT16 j;
    	
//  CfgFsys( );                                                                //CH559时钟选择配置    
    mDelaymS(5);                                                               //等待外部晶振稳定		
    mInitSTDIO();                                                              //串口0,可以用于调试
    printf("start ...\n");

/* 直接地址方式 */
#if 1
    CH559XBUSDirectInit();                                                     //初始化直接总线方式
    XBUSSpeedSetup( );                                                         //总线速度设置
#endif

    printf("Write ExternalRAM ...\n");                                                 //向RAM写数据
    for(j=0;j<0x200;j++)
    { 
    	  XBUSWrite(j,j%256);  
    	  if((j%256) == 0)
        {
        	  printf("Addr: %02X   \n",j);
        }		
    }
    printf("\n");

    mDelaymS(500);		
    printf("Read ExternalRAM ...\n");                                                  //读RAM中数据    
    for(j=0;j<0x200;j++)
    {
        printf("%02X  ",(UINT16)XBUSRead(j));	
    }
    while(1);	 
}
