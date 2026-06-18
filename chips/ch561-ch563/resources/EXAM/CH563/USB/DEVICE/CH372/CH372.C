/********************************** (C) COPYRIGHT *******************************
* File Name          : USB_DEVICE.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 USB_Device Example
*                      说明: 
*                      (1)、该程序模拟USB设备,实现数据的上传\下传;
*                      (2)、端点2下传一包数据之后,CH563取反后回传;
*                      (3)、自动识别高速\全速模式,也可以初始化后强制设置为全速模式;
*                      (4)、增加数字签名测试; 
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include  <stdio.h>
#include  <string.h>
#include  "CH563BAS.H"                                                          /* 芯片类型定义相关头文件 */    
#include  "CH563SFR.H"                                                          /* 芯片寄存器相关头文件 */
#include  "SYSFREQ.H"                                                           /* 系统配置相关头文件 */
#include  "CH563USBSFR.H"                                                       /* 芯片USB寄存器相关头文件 */    
#include  "CH563USB.H"                                                          /* USB常用定义相关头文件 */    
#include  "USB_DEVICE.H"                                                        /* USB设备定义相关头文件 */
#include  "PRINTF.H"                                                            /* 串口打印输出控制头文件 */

/******************************************************************************/
/* 设备描述符 */
const UINT8 MyDevDescr[ ] = { 0x12, 0x01, 0x00, 0x02, 0xFF, 0x80, 0x37, 0x40, 
                              0x48, 0x43, 0x37, 0x55,                           /* 厂商ID和产品ID */
                              0x00, 0x01, 0x01, 0x02, 0x00, 0x01 };

/* 全速配置描述符 */
const UINT8 My_FS_CfgDescr[ ] = { 0x09, 0x02, 0x27, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
                                  0x09, 0x04, 0x00, 0x00, 0x03, 0xFF, 0x80, 0x37, 0x00,
                                  0x07, 0x05, 0x82, 0x02, 0x40, 0x00, 0x00,
                                  0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00,
                                  0x07, 0x05, 0x81, 0x03, 0x08, 0x00, 0x0A };

/* 高速配置描述符 */         
const UINT8 My_HS_CfgDescr[ ] = { 0x09, 0x02, 0x27, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
                                  0x09, 0x04, 0x00, 0x00, 0x03, 0xFF, 0x80, 0x37, 0x00,
                                  0x07, 0x05, 0x82, 0x02, 0x00, 0x02, 0x00,
                                  0x07, 0x05, 0x02, 0x02, 0x00, 0x02, 0x00,
                                  0x07, 0x05, 0x81, 0x03, 0x40, 0x00, 0x0A };

/* 设备限定描述符 */
const UINT8    My_QueDescr[ ] = { 0x0A, 0x06, 0x00, 0x02, 0xFF, 0x00, 0xFF, 0x40, 0x01, 0x00 };

/* 全速模式，其他速度配置描述符 */
const UINT8    My_FS_OscDescr[ ] = { 0x09, 0x07, 0x27, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
                                  0x09, 0x04, 0x00, 0x00, 0x03, 0xFF, 0x80, 0x37, 0x00,
                                  0x07, 0x05, 0x82, 0x02, 0x00, 0x02, 0x00,
                                  0x07, 0x05, 0x02, 0x02, 0x00, 0x02, 0x00,
                                  0x07, 0x05, 0x81, 0x03, 0x40, 0x00, 0x0A };

/* 高速模式，其他速度配置描述符 */
const UINT8    My_HS_OscDescr[ ] = { 0x09, 0x07, 0x27, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
                                  0x09, 0x04, 0x00, 0x00, 0x03, 0xFF, 0x80, 0x37, 0x00,
                                  0x07, 0x05, 0x82, 0x02, 0x40, 0x00, 0x00,
                                  0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00,
                                  0x07, 0x05, 0x81, 0x03, 0x08, 0x00, 0x0A };

/* 语言描述符 */
const UINT8 MyLangDescr[ ] = { 0x04, 0x03, 0x09, 0x04 };

/* 厂家信息 */
const UINT8 MyManuInfo[ ] = { 0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0 };

/* 产品信息 */
const UINT8 MyProdInfo[ ] = { 0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '6', 0, '3', 0 };

/*******************************************************************************/
/* 常、变量定义 */
UINT8V  gUsbConfig;                                                             /* USB设备配置标志 */
UINT8V  gSetupReq;                                                              /* USB控制传输命令码 */    
UINT16V gSetupLen;                                                              /* USB控制传输传输长度 */
UINT8V  gUsbSpeed;                                                              /* 当前USB速度(默认0: 高速;1:全速) */    
UINT8V  gUsbInterCfg;                                                           /* USB设备接口配置 */
UINT8V  gUsbFlag;                                                               /* USB设备各种操作标志,位0=总线复位,位1=获取设备描述符,位2=设置地址,位3=获取配置描述符,位4=设置配置 */
UINT8   *pDescr;                                                                /* 描述符指针 */        
UINT32  USB_Recv_Buf[ 512/4 ];                                                  /* USB设备数据接收缓冲区 */
UINT32  USB_Tran_Buf[ 512/4 ];                                                  /* USB设备数据发送缓冲区 */
UINT32V gRecvLen;                                                               /* 当前包接收长度 */

/*******************************************************************************
* Function Name  : USBDev_ModeSet
* Description    : 设置成USB设备模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_ModeSet( void )
{ 
    R8_MISC_CTRL_USB &= ~RB_MISC_USB_ID_EN;                                     /* USB OTG ID pin enable */                                    
    R8_MISC_CTRL_USB |= RB_MISC_USB_ID_ST;                                      /* 设置成USB设备 */
}

/*******************************************************************************
* Function Name  : USBDev_SpeedSet
* Description    : USB设备速度设置
* Input          : speed---USB_DEV_SPEED_HS: 高速模式
*                          USB_DEV_SPEED_FS: 全速模式
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_SpeedSet( UINT8 speed )
{
    if( speed == USB_DEV_SPEED_HS )
    {
        USB_DEV_CONTROL &= ~RB_FORCE_FS;                                        /* 高速模式(默认) */
    }
    else
    {
        USB_DEV_CONTROL |= RB_FORCE_FS;                                         /* 全速模式 */
    }    
}

/*******************************************************************************
* Function Name  : USBDev_SpeedCheck
* Description    : USB设备速度检测
* Input          : None
* Output         : None
* Return         : 返回USB_DEV_SPEED_HS: 高速模式; USB_DEV_SPEED_FS: 全速模式
*******************************************************************************/

UINT8 USBDev_SpeedCheck( void )
{
     if( USB_DEV_CONTROL & RB_HS_EN )
    {
#if  MY_DEBUG_PRINTF         
        printf("Current Device is High Speed!\n");    
#endif                                    
        gUsbSpeed = USB_DEV_SPEED_HS;                                           /* 当前USB设备速度为高速 */
        return( USB_DEV_SPEED_HS );    
    }
    else
    {
#if  MY_DEBUG_PRINTF         
          printf("Current Device is Full Speed!\n");
#endif          
        gUsbSpeed = USB_DEV_SPEED_FS;                                           /* 当前USB设备速度为全速 */
        return( USB_DEV_SPEED_FS );    
    }
}

/*******************************************************************************
* Function Name  : USBDev_UsbInt_Enable
* Description    : USB设备相关USB中断使能
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_UsbInt_Enable( void )  
{
    /* 开启USB中断 */
    R8_INT_EN_IRQ_1 |= RB_IE_IRQ_USB;                                           /* USB中断使能 */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* 全局中断使能 */
}

/*******************************************************************************
* Function Name  : USBDev_Init
* Description    : USB设备初始化
*                  包括主要寄存器的设置，端点、FIFO配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_Init( void )
{
    /* 模拟CH372设备 */
    /* ENDP1_IN 为中断端点，占8个字节，使用FIFO0(IN) */
    /* ENDP2_IN 为批量端点，占64个字节，使用FIFO1(IN) */
    /* ENDP2_OUT为批量端点，占64个字节，使用FIFO2(OUT) */

    /* 复位USB设备 */
    USB_DEV_CONTROL = RB_SFRST;
    while( USB_DEV_CONTROL & 0x10 );                                            /* 等待复位完成 */

    /* 复位USB设备相关FIFO */
    USB_DEV_FIFO0_INFO = RB_FFRST;                                              /* FIFO0 Reset */
    USB_DEV_FIFO1_INFO = RB_FFRST;                                              /* FIFO1 Reset */
    USB_DEV_FIFO2_INFO = RB_FFRST;                                              /* FIFO2 Reset */
    USB_DEV_FIFO3_INFO = RB_FFRST;                                              /* FIFO3 Reset */
    Delay_us( 10 );    
    
#if 1
    /* 一、各端点以及相关FIFO配置(寄存器直接配置方式) */
    USB_DEV_FIFO_CONFIG       = 0x00222223;                                        /* R(0x1AC) */
    USB_DEV_EP1_4_MAP      = 0x33332130;                                        /* R(0x1A0) FIFO0(ENDP1_IN)、FIFO1(ENDP2_IN)、FIFO2(ENDP2_OUT) */         
    USB_DEV_FIFO_MAP       = 0x0F021211;                                        /* R(0x1A8) */                                          
    USB_DEV_EP1_IN_MAXPKS  = 0x00000008;                                        /* R(0x160) EP1_IN_MAXPKS  = 8 bytes */
    USB_DEV_EP2_IN_MAXPKS  = 0x00000040;                                        /* R(0x164) EP2_IN_MAXPKS  = 64 bytes */
    USB_DEV_EP2_OUT_MAXPKS = 0x00000040;                                        /* R(0x184) EP2_OUT_MAXPKS = 64 bytes */    
#endif

#if 0        
    /* 二、各端点以及相关FIFO配置(函数间接配置方式) */

    /* 端点1配置 */
    USBDev_EPx_Init( ENDP1, 
                     ENDP_TYPE_IN,                                              /* 配置ENDP1 IN端点 */
                     FIFO_TYPE_INTERRUPT,                                       /* 端点类型为中断端点 */
                     FIFO_DIR_IN,                                               /* FIFO方向为为IN */
                     FIFO_BK_NUM_SINGLE,                                        /* FIFO块数为单块,最大512字节 */    
                     FIFO_BK_SIZE_0,                                            /* FIFO块大小小于等于512字节 */
                     FIFO_NUM_0,                                                /* 该端点对应的FIFO为FIFO 0 */
                     FIFO_ENABLE,                                               /* 使能该端点对应的FIFO */
                     USB_EP1_IN_MAX_PKT );                                      /* 设置该端点最大包大小 */
    
    /* 端点2配置 */    
    USBDev_EPx_Init( ENDP2,     
                     ENDP_TYPE_IN,                                              /* 配置ENDP2 IN端点 */
                     FIFO_TYPE_BULK,                                            /* 端点类型为批量端点 */
                     FIFO_DIR_IN,                                               /* FIFO方向为为IN */
                     FIFO_BK_NUM_SINGLE,                                        /* FIFO块数为单块,最大512字节 */
                     FIFO_BK_SIZE_0,                                            /* FIFO块大小小于等于512字节 */    
                     FIFO_NUM_1,                                                /* 该端点对应的FIFO为FIFO 1 */
                     FIFO_ENABLE,                                               /* 使能该端点对应的FIFO */    
                     USB_EP2_IN_MAX_PKT );                                      /* 设置该端点最大包大小 */

    /* 端点2配置 */
    USBDev_EPx_Init( ENDP2, 
                     ENDP_TYPE_OUT,                                             /* 配置ENDP2 OUT端点 */
                     FIFO_TYPE_BULK,                                            /* 端点类型为批量端点 */
                     FIFO_DIR_OUT,                                              /* FIFO方向为为OUT */
                     FIFO_BK_NUM_SINGLE,                                        /* FIFO块数为单块,最大512字节 */
                     FIFO_BK_SIZE_0,                                            /* FIFO块大小小于等于512字节 */    
                     FIFO_NUM_2,                                                /* 该端点对应的FIFO为FIFO 2 */
                     FIFO_ENABLE,                                               /* 使能该端点对应的FIFO */    
                     USB_EP2_OUT_MAX_PKT );                                     /* 设置该端点最大包大小 */        
#endif        

    /* USB设备各种中断使能设置 */
    /* 该程序使能了除FIFO空中断外的所有中断,可以根据需要关闭相应中断 */
    USB_DEV_MASK_INT_GROP  = 0x00000000;                                        /* Enable USB Device interrupt of source group 0、1、2*/
    USB_DEV_MASK_INT_GROP0 = 0x00000000;
    USB_DEV_MASK_INT_GROP1 = RB_F3_IN_INT | RB_F2_IN_INT | RB_F1_IN_INT | 
        RB_F0_IN_INT;                                                           /* 禁止FIFO上传空中断 */
    USB_DEV_MASK_INT_GROP2 = 0x00000000;        

    /* 使能芯片并连接(根据需要可以强制设置全速模式) */
    USB_DEV_CONTROL = RB_CHIP_EN | RB_GLINT_EN;                                 /* 使能芯片、使能全局中断、使能高速 */                  
//    USBDev_SpeedSet( USB_DEV_SPEED_FS );                                        /* 强制设置全速模式 */
    Delay_ms( 1 );                                                
    USB_PHY_TEST_MODE &= ~RB_UNPLUG;                                            /* 寄存器(0x114)位0清0,通知主机连接 */
} 

/*******************************************************************************
* Function Name  : USB_Dev_SetAddress
* Description    : 设置USB设备地址
* Input          : addre---设备地址
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_SetAddress( UINT8 addre )
{
    USB_DEV_ADDRESS = ( USB_DEV_ADDRESS & RB_AFT_CONF ) | addre;                /* 清地址寄存器低7位 */    
}

/*******************************************************************************
* Function Name  : USBDev_SetConfig
* Description    : 设置USB设备配置标志位
*                  注意: 必须在处理SET_CONFIGURATION命令后设置该标志位,否则不会响应
*                        非控制端点的传输
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_SetConfig( void )
{
    USB_DEV_ADDRESS |= RB_AFT_CONF;                                             /* 设置配置标志位 */     
    
#if  MY_DEBUG_PRINTF         
    printf("USB Enumerate Success!\n ");
#endif
}

/*******************************************************************************
* Function Name  : USBDev_EP0_Stall
* Description    : USB设备控制设置STALL
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EP0_Stall( void )
{
    USB_DEV_CX_CONF_FIFO_STATUS |= RB_CX_STL;                                   /* reg=0x120 */    
}

/*******************************************************************************
* Function Name  : USBDev_EPx_IN_Stall
* Description    : USB设备EPx的IN端点设置STALL
* Input          : epx_num---端点号(范围为:1---8)
*                  mode------模式(1为设置STALL, 0为清除STALL)
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_IN_Stall( UINT8 epx_num, UINT8 mode )
{
    UINT32 addr;
    UINT32 reg;    
    
    addr = USB_BASE_ADDR + 0x160 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));    
    if( mode )
    {
        (*((PUINT32V)addr)) = reg | RB_STL_IEPx;        
    }
    else
    {
        (*((PUINT32V)addr)) = reg & ( ~RB_STL_IEPx );    
    }                
}

/*******************************************************************************
* Function Name  : USBDev_EPx_OUT_Stall
* Description    : USB设备EPx的OUT端点设置STALL
* Input          : epx_num---端点号(范围为:1---8)
*                  mode------模式(1：为设置STALL, 0为清除STALL)
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_OUT_Stall( UINT8 epx_num, UINT8 mode )
{
    UINT32 addr;
    UINT32 reg;    
    
    addr = USB_BASE_ADDR + 0x180 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));

    if( mode )
    {
        (*((PUINT32V)addr)) = reg | RB_STL_OEPx;        
    }
    else
    {
        (*((PUINT32V)addr)) = reg & ( ~RB_STL_OEPx );    
    }
}

/*******************************************************************************
* Function Name  : USBDev_EPx_IN_ResetToggle
* Description    : USB设备EPx的IN端点复位同步位
* Input          : epx_num---端点号(范围为:1---8)
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_IN_ResetToggle( UINT8 epx_num )
{
    UINT32 addr;
    UINT32 reg;    
    
    addr = USB_BASE_ADDR + 0x160 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));
    (*((PUINT32V)addr)) = reg | RB_RSTG_IEPx;
    Delay_us( 5 );
    (*((PUINT32V)addr)) = reg & ( ~RB_RSTG_IEPx );            
}

/*******************************************************************************
* Function Name  : USBDev_EPx_OUT_ResetToggle
* Description    : USB设备EPx的OUT端点复位同步位
* Input          : epx_num---端点号(范围为:1---8)
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_OUT_ResetToggle( UINT8 epx_num )
{
    UINT32 addr;
    UINT32 reg;    
    
    addr = USB_BASE_ADDR + 0x180 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));
    (*((PUINT32V)addr)) = reg | RB_RSTG_OEPx;
    Delay_us( 5 );
    (*((PUINT32V)addr)) = reg & ( ~RB_RSTG_OEPx );                
}

/*******************************************************************************
* Function Name  : USBDev_EPx_IN_SetMaxPKS
* Description    : 设置USB设备EPx的IN端点最大包长度
* Input          : epx_num---端点号(范围为:1---8)
*                  maxpks_len---最大包大小 
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_IN_SetMaxPKS( UINT8 epx_num, UINT16 maxpks_len )
{
    UINT32 addr;
    UINT32 reg;    
    
    maxpks_len = maxpks_len & 0x7FF;
    addr = USB_BASE_ADDR + 0x160 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));
    (*((PUINT32V)addr)) = ( reg & 0xFFFFFC00 ) | maxpks_len;        
}

/*******************************************************************************
* Function Name  : USBDev_EPx_OUT_SetMaxPKS
* Description    : 设置USB设备EPx的OUT端点最大包长度
* Input          : epx_num---端点号(范围为:1---8)
*                  maxpks_len---最大包大小 
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_OUT_SetMaxPKS( UINT8 epx_num, UINT16 maxpks_len )
{
    UINT32 addr;
    UINT32 reg;    
    
    maxpks_len = maxpks_len & 0x7FF;
    addr = USB_BASE_ADDR + 0x180 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));
    (*((PUINT32V)addr)) = ( reg & 0xFFFFFC00 ) | maxpks_len;        
}

/*******************************************************************************
* Function Name  : USBDev_EPx_Init
* Description    : USB设备端点配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_Init( UINT8 epx_num, 
                      UINT8 epx_type,
                      UINT8 fifo_type, 
                      UINT8 fifo_dir, 
                      UINT8 fifo_bk_num, 
                      UINT8 fifo_bk_size, 
                      UINT8 fifo_num, 
                      UINT8 fifo_en,
                      UINT16 maxpks_len )
{    
    UINT32 addr;
    UINT32 reg1, reg2, mask;    
    
    /* 设置该端点IN或OUT包的最大包大小 */
    if( epx_type == ENDP_TYPE_IN )
    {    
        USBDev_EPx_IN_SetMaxPKS( epx_num, maxpks_len );                         /* 设置EPx_IN端点大小 */
    }
    else if( epx_type == ENDP_TYPE_OUT )
    {
        USBDev_EPx_OUT_SetMaxPKS( epx_num, maxpks_len );                        /* 设置EPx_OUT端点大小 */
    }        

    /* 设置该端点IN或OUT对应的FIFO号 */
    /* 配置Endpoint1-4 Map Register(Addr=0X1A0)以及Endpoint5-8 Map Register(Addr=0X1A4)*/    
    if( epx_num <= 4 )
    {
        addr = USB_BASE_ADDR + 0x1A0;    
    }
    else
    {
        addr = USB_BASE_ADDR + 0x1A4;            
        epx_num = epx_num - 4;
    }    
    reg1 = (*((PUINT32V)addr));                                                 /* 读取寄存器0X1A0或0X1A4原配置值 */

    mask = 0x00000003;
    reg2 = fifo_num;
    if( epx_type == ENDP_TYPE_OUT )
    {    
        mask = mask << 4;
        reg2 = reg2 << 4;
    }
    mask = mask << ( ( epx_num - 1 ) * 8 );    
    reg2 = reg2 << ( ( epx_num - 1 ) * 8 );                
    reg1 = ( reg1 & ( ~mask ) ) | reg2;    
    reg1 = reg1 & 0x33333333;
    (*((PUINT32V)addr)) = reg1;        

    /* 配置FIFO Map Register(Addr=0X1A8) */
    mask = 0x000000FF;
    reg2 = ( ( fifo_dir & 0x03 ) << 4 ) | epx_num;
    mask = mask << ( fifo_num * 8 );    
    reg2 = reg2 << ( fifo_num * 8 );    
    reg1 = USB_DEV_FIFO_MAP;    
    USB_DEV_FIFO_MAP = ( reg1 & ( ~mask ) ) | reg2;        
    
    /* 配置FIFO Configuration Register(Addr=0X1AC) */    
    mask = 0x000000FF;
    reg2 = fifo_type | ( ( fifo_bk_num & 0x03 ) << 2 ) | ( fifo_bk_size << 4 ) | ( fifo_en << 5 );        
    mask = mask << ( fifo_num * 8 );    
    reg2 = reg2 << ( fifo_num * 8 );    
    reg1 = USB_DEV_FIFO_CONFIG;    
    USB_DEV_FIFO_CONFIG = ( reg1 & ( ~mask ) ) | reg2;                        
}

/*******************************************************************************
* Function Name  : USBDev_RD_FIFOx
* Description    : 从FIFO中读取USB数据
* Input          : fifonum---FIFO号
*                  len-------传输数据长度
*                  *pbuf-----数据存储缓冲区    
* Output         : None
* Return         : len---实际读取长度
*******************************************************************************/

UINT32 USBDev_RD_FIFOx( UINT8 fifonum, UINT32 len, UINT8 *pbuf )
{
    UINT32 i = 0;
        
    if( ( fifonum != USB_DMA_ACC_F0 ) && ( fifonum != USB_DMA_ACC_F1 ) &&
        ( fifonum != USB_DMA_ACC_F2 ) && ( fifonum != USB_DMA_ACC_F3 ) &&
        ( fifonum != USB_DMA_ACC_CXF ) )
    {
        USB_DEV_DMA_TARGET_FIFO = USB_DMA_ACC_NO;                               /* 设置DMA不对应任何FIFO */
        return( 0x00 );
    }
    
#if  MY_DEBUG_PRINTF 
    printf("USBDev_RD_FIFOx:\n ");
#endif    
    USB_DEV_DMA_TARGET_FIFO = fifonum;                                          /* 设置DMA对应的FIFO */
    if( fifonum == USB_DMA_ACC_CXF )                                            /* SETUP包数据直接读取 */
    {
        if( len == 8 ) 
        {
            *(PUINT32)pbuf = USB_DEV_SETUP_CMD_RPORT;
            *(PUINT32)( pbuf + 4 ) = USB_DEV_SETUP_CMD_RPORT;
        }
        else 
        {
            len = 0;
        }            
    }
    else
    {
        if( USB_DEV_DMA_SET1 & RB_DMA_ABORT )                                   /* 判断是否有DMA_ABORT */
        {
            USB_DEV_DMA_SET1 |= RB_CLRFIFO_DMAABORT;    
            len = 0x00;                            
        }
        else
        {        
            /* 从FIFO x Instruction and Byte Count Register(0X1B0--0X1BC)中读取对应FIFO数据的长度 */
            if( fifonum == USB_DMA_ACC_F0 )
            {
                i = USB_DEV_FIFO0_INFO & 0x7FF;                                            
            }
            else if( fifonum == USB_DMA_ACC_F1 )
            {
                i = USB_DEV_FIFO1_INFO & 0x7FF;                                        
            }            
            else if( fifonum == USB_DMA_ACC_F2 )
            {
                i = USB_DEV_FIFO2_INFO & 0x7FF;                                        
            }            
            else if( fifonum == USB_DMA_ACC_F3 )
            {
                i = USB_DEV_FIFO3_INFO & 0x7FF;
            }    
            if( len > i )
            {
                len = i;                                                        /* 读取长度大于实际长度,则取实际长度 */
            }
#if MY_DEBUG_PRINTF 
            printf("rd_len:%04x\n",(UINT32)len);
#endif
            /* 通过DMA将数据从RAM传给FIFO */
            USB_DEV_DMA_SET1 = len << 8;                                        /* 设置USB DMA通信长度 */
            USB_DEV_DMA_SET2 = (UINT32)pbuf;                                    /* 设置USB DMA通信缓冲区起始地址 */    
            USB_DEV_DMA_SET1 = USB_DEV_DMA_SET1 | RB_DMA_START;                 /* 启动USB DMA进行FIFO数据处理 */
            while( USB_DEV_DMA_SET1 & RB_DMA_START );                           /* 等待DMA传输结束 */
            USB_DEV_INT_GROP2 = USB_DEV_INT_GROP2 | RB_DMA_CMPLT;               /* 清DMA完成标志 */        
        }
    }
    USB_DEV_DMA_TARGET_FIFO = USB_DMA_ACC_NO;                                   /* 设置DMA不对应任何FIFO */
    return( len );
}

/*******************************************************************************
* Function Name  : USBDev_WR_FIFOx
* Description    : 向FIFO中写入USB数据
* Input          : fifonum---FIFO号
*                  len-------传输数据长度
*                  *pbuf-----数据存储缓冲区    
* Output         : None
* Return         : len---写入长度
*******************************************************************************/

UINT32 USBDev_WR_FIFOx( UINT8 fifonum, UINT32 len, UINT8 *pbuf )
{
    if( ( fifonum != USB_DMA_ACC_F0 ) && ( fifonum != USB_DMA_ACC_F1 ) &&
        ( fifonum != USB_DMA_ACC_F2 ) && ( fifonum != USB_DMA_ACC_F3 ) &&
        ( fifonum != USB_DMA_ACC_CXF ) )
    {
        USB_DEV_DMA_TARGET_FIFO = USB_DMA_ACC_NO;                               /* 设置DMA不对应任何FIFO */
        return( 0x00 );
    }

#if  MY_DEBUG_PRINTF 
    printf("USBDev_WR_FIFOx:\n ");
#endif
    if( USB_DEV_DMA_SET1 & RB_DMA_ABORT )                                       /* 判断是否有DMA_ABORT */
    {
        USB_DEV_DMA_SET1 = RB_CLRFIFO_DMAABORT;    
        return( 0x00 );                            
    }
    else
    {        
#if  MY_DEBUG_PRINTF 
        printf("wr_len:%04x\n",(UINT32)len);
#endif

        /* 通过DMA将数据从FIFO传给RAM */
        USB_DEV_DMA_TARGET_FIFO = fifonum;                                      /* 设置USB DMA对应的FIFO */                                                 
        USB_DEV_DMA_SET1 = ( len  << 8 ) | RB_DMA_TYPE;                         /* 设置USB DMA通信长度 */
        
        /* 如果长度不为0则进行DMA数据处理 */
        if( len )
        {
            USB_DEV_DMA_SET2 = ( UINT32 )pbuf;                                  /* 设置USB DMA通信缓冲区起始地址 */    
            USB_DEV_DMA_SET1 = USB_DEV_DMA_SET1 | RB_DMA_START;                 /* 启动USB DMA进行FIFO数据处理 */
            while( ( USB_DEV_DMA_SET1 & RB_DMA_START ) );                       /* 等待DMA传输结束 */
        }
        USB_DEV_INT_GROP2 = USB_DEV_INT_GROP2 | RB_DMA_CMPLT;                   /* 清DMA完成标志 */    
        USB_DEV_DMA_TARGET_FIFO = USB_DMA_ACC_NO;                               /* 设置DMA不对应任何FIFO */
        return( len );        
    }        
}

/*******************************************************************************
* Function Name  : USBDev_EP0_Setup_Deal
* Description    : USB控制端点Setup包处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EP0_Setup_Deal( void )
{
    UINT32 len;
    UINT32 status;    
    UINT8  buf[ 8 ];
    USB_SETUP_REQ SetupReqBuf;

    /* 获取SETUP包 */    
    len = USBDev_RD_FIFOx( USB_DMA_ACC_CXF, 8, (PUINT8)&SetupReqBuf );          /* 从FIFO4中读取SETUP包 */
    if( len == sizeof( USB_SETUP_REQ ) )                                        /* 判断长度是否是Setup包长度 */
    {
        gSetupLen = ( SetupReqBuf.wLengthH << 8 ) + SetupReqBuf.wLengthL;       /* 保存传输长度 */
        gSetupReq = SetupReqBuf.bRequest;                                       /* 保存当前请求码 */

        /* 分析并处理当前SETUP包 */
        len = 0;                                                                /* 默认上传长度为0 */
        status = 0x00;                                                          /* 默认操作为成功状态 */    
        if( ( SetupReqBuf.bReqType & USB_REQ_TYPE_MASK ) != USB_REQ_TYPE_STAND ) 
        {  
            /* 其它请求，如类请求，产商请求等 */
            status = 0xFF;                                                      /* 操作失败 */
        }
        else 
        {  
            /* 标准请求 */
            switch( gSetupReq ) 
            {
                case USB_REQ_GET_DESCR:                                         /* CMD: GET_DESCR */
#if  MY_DEBUG_PRINTF                     
                    printf("USB_REQ_GET_DESCR:\n ");
#endif
                    switch( SetupReqBuf.wValueH ) 
                    {
                        case 1:
                            gUsbFlag |= DEF_BIT_USB_DEV_DESC;                   /* 设置获取设备描述符标志 */
                            pDescr = (PUINT8)( &MyDevDescr[ 0 ] );
                            len = sizeof( MyDevDescr );
                            USBDev_SpeedCheck(  );                              /* 判断当前USB设备速度 */
                            break;
                        
                        case 2:
                            gUsbFlag |= DEF_BIT_USB_CFG_DESC;                   /* 设置获取配置描述符标志 */
                            if( gUsbSpeed == USB_DEV_SPEED_HS )                 /* 高速模式 */
                            {
                                gUsbFlag |= DEF_BIT_USB_HS;                     /* 设置高速标志 */
                                pDescr = (PUINT8)( &My_HS_CfgDescr[ 0 ] );
                                len = sizeof( My_HS_CfgDescr );
                                
                                /* 重新设置相应端点大小 */                                
                                USBDev_EPx_IN_SetMaxPKS( ENDP1, 64 );
                                USBDev_EPx_IN_SetMaxPKS( ENDP2, 512 );
                                USBDev_EPx_OUT_SetMaxPKS( ENDP2, 512 );                                
                            }
                            else                                                /* 全速模式 */        
                            {
                                gUsbFlag &= ~DEF_BIT_USB_HS;                    /* 设置全速标志 */
                                pDescr = (PUINT8)( &My_FS_CfgDescr[ 0 ] );
                                len = sizeof( My_FS_CfgDescr );
                            }
                            break;
                        
                        case 3:                            
                            switch( SetupReqBuf.wValueL ) 
                            {
                                case 1:
                                    pDescr = (PUINT8)( &MyManuInfo[ 0 ] );      /* 获取厂商字符串描述符 */
                                    len = sizeof( MyManuInfo );
                                    break;
                                    
                                case 2:
                                    pDescr = (PUINT8)( &MyProdInfo[ 0 ] );      /* 获取产品字符串描述符 */
                                    len = sizeof( MyProdInfo );
                                    break;
                                    
                                case 0:
                                    pDescr = (PUINT8)( &MyLangDescr[ 0 ] );     /* 获取语言ID字符串描述符 */
                                    len = sizeof( MyLangDescr );
                                    break;
                                
                                default:
                                    status = 0xFF;                              /* 操作失败 */
                                    break;
                            }
                            break;
                        
                        case 6:                                                 /* 设备限定描述符 */
                            pDescr = ( PUINT8 )( &My_QueDescr[ 0 ] );
                            len = sizeof( My_QueDescr );
                            break;
                        
                        case 7:                                                 /* 其他速度配置描述符 */
                            if( gUsbSpeed == USB_DEV_SPEED_HS )                 /* 高速模式 */
                            {
                                pDescr = ( PUINT8 )( &My_HS_OscDescr[ 0 ] );
                                len = sizeof( My_HS_OscDescr );                                
                            }
                            else
                            {
                                pDescr = ( PUINT8 )( &My_FS_OscDescr[ 0 ] );
                                len = sizeof( My_FS_OscDescr );    
                            }
                            break;
                                                
                        default:
                            status = 0xFF;                                      /* 操作失败 */
                            break;
                    }
                    break;
                                
                case USB_REQ_SET_ADDRESS:                                       /* CMD: SET_ADDRESS */    
                    gUsbFlag |= DEF_BIT_USB_ADDRESS;                            /* 设置设置地址标志 */            
                    if( SetupReqBuf.wValueL < 0x80 )
                    {
                        USBDev_SetAddress( SetupReqBuf.wValueL );               /* 设置地址: 注：感觉芯片在下一个SETUP包时该地址才生效 */                            
                    }
                    else
                    {
                        status = 0xFF;                                          /* 操作失败 */    
                    }
                    break;
                                
                case USB_REQ_GET_CONFIG:                                        /* CMD: GET_CONFIG */
                    buf[ 0 ] = gUsbConfig;
                    len = 0x01;
                    pDescr = buf;
                    break;
                                
                case USB_REQ_SET_CONFIG:                                        /* CMD: SET_CONFIG */
                    gUsbFlag |= DEF_BIT_USB_SET_CFG;                            /* 设置设置配置标志 */
                    gUsbConfig = SetupReqBuf.wValueL;
                    USBDev_SetConfig( );                                        /* 设置USB配置位 */
                    break;
                                
                case USB_REQ_CLR_FEATURE:                                       /* Clear Feature */
                    if( ( SetupReqBuf.bReqType & 0x1F ) == 0x00 )               /* 清除设备 */ 
                    {                                         
                        if( ( ( ( UINT16 )SetupReqBuf.wValueH << 8 ) | SetupReqBuf.wValueL ) == 0x01 )
                        {
                            if( gUsbSpeed == USB_DEV_SPEED_HS )                 /* 高速模式 */
                            {
                                if( My_HS_CfgDescr[ 7 ] & 0x20 ) 
                                {
                                    gUsbFlag &= ~DEF_BIT_USB_WAKE;              /* 设置唤醒禁止标志 */
                                }
                                else 
                                {
                                    status = 0xFF;                              /* 操作失败 */
                                }
                            }
                            else
                            {
                                if( My_FS_CfgDescr[ 7 ] & 0x20 ) 
                                {    
                                    gUsbFlag &= ~DEF_BIT_USB_WAKE;              /* 设置唤醒禁止标志 */
                                }
                                else 
                                {
                                    status = 0xFF;                              /* 操作失败 */
                                }
                            }
                        }
                        else 
                        {
                            status = 0xFF;                                      /* 操作失败 */
                        }    
                    }                    
                    else if( ( SetupReqBuf.bReqType & 0x1F ) == 0x02 )          /* 清除端点 */
                    {      
                        switch( ( ( UINT16 )SetupReqBuf.wIndexH << 8 ) | SetupReqBuf.wIndexL ) 
                        {
                            case 0x82:
                                USBDev_EPx_IN_Stall( 0x02, 0x00 );              /* 清除STALL */
                                USBDev_EPx_IN_ResetToggle( 0x02 );              /* 复位同步标志 */
                                break;
                            
                            case 0x02:
                                USBDev_EPx_OUT_Stall( 0x02, 0x00 );             /* 清除STALL */
                                USBDev_EPx_OUT_ResetToggle( 0x02 );             /* 复位同步标志 */
                                break;
                            
                            case 0x81:
                                USBDev_EPx_IN_Stall( 0x01, 0x00 );              /* 清除STALL */
                                USBDev_EPx_IN_ResetToggle( 0x01 );              /* 复位同步标志 */
                                break;
                            
                            case 0x01:
                                USBDev_EPx_OUT_Stall( 0x01, 0x00 );             /* 清除STALL */
                                USBDev_EPx_OUT_ResetToggle( 0x01 );             /* 复位同步标志 */
                                break;
                            
                            default:
                                status = 0xFF;                                  /* 操作失败 */
                                break;
                        }
                    }
                    else
                    {
                        status = 0xFF;                                          /* 操作失败 */
                    }
                    break;

                case USB_REQ_SET_FEATURE:                                       /* Set Feature */
                    if( ( SetupReqBuf.bReqType & 0x1F ) == 0x00 )               /* 设置设备 */
                    {
                        if( ( ( ( UINT16 )SetupReqBuf.wValueH << 8 ) | SetupReqBuf.wValueL ) == 0x01 )
                        {
                            if( gUsbSpeed == USB_DEV_SPEED_HS )                 /* 高速模式 */
                            {
                                if( My_HS_CfgDescr[ 7 ] & 0x20 ) 
                                {
                                    gUsbFlag |= DEF_BIT_USB_WAKE;               /* 设置唤醒使能标志 */
                                }
                                else 
                                {
                                    status = 0xFF;                              /* 操作失败 */
                                }
                            }
                            else
                            {
                                if( My_HS_CfgDescr[ 7 ] & 0x20 ) 
                                {
                                    gUsbFlag |= DEF_BIT_USB_WAKE;               /* 设置唤醒使能标志 */
                                }
                                else 
                                {    
                                    status = 0xFF;                              /* 操作失败 */
                                }
                            }
                        }
                        else 
                        {
                            status = 0xFF;                                      /* 操作失败 */
                        }
                    }
                    else if( ( SetupReqBuf.bReqType & 0x1F ) == 0x02 )          /* 设置端点 */
                    {
                        if( ( ( ( UINT16 )SetupReqBuf.wValueH << 8 ) | SetupReqBuf.wValueL ) == 0x00 )
                        {
                            switch( ( ( UINT16 )SetupReqBuf.wIndexH << 8 ) | SetupReqBuf.wIndexL ) 
                            {
                                case 0x82:
                                    USBDev_EPx_IN_Stall( 0x02, 0x01 );          /* 设置端点2 IN STALL */
                                    break;

                                case 0x02:
                                    USBDev_EPx_OUT_Stall( 0x02, 0x01 );         /* 设置端点2 OUT Stall */
                                    break;

                                case 0x81:
                                    USBDev_EPx_IN_Stall( 0x01, 0x01 );          /* 设置端点1 IN STALL */
                                    break;

                                default:
                                    status = 0xFF;                              /* 操作失败 */
                                    break;
                            }
                        }
                        else 
                        {
                            status = 0xFF;                                      /* 操作失败 */
                        }
                    }
                    else 
                    {
                        status = 0xFF;                                          /* 操作失败 */
                    }    
                    break;

                case USB_REQ_SET_INTERF:                                        /* Set Interface */
                    gUsbInterCfg = SetupReqBuf.wIndexL;                         /* 保存接口配置 */
                    break;
                
                case USB_REQ_GET_INTERF:                                        /* CMD: GET_INTERF */
                    buf[ 0 ] = gUsbInterCfg;
                    pDescr = buf;
                    len = 1;                                        
                    break;
                
                case USB_REQ_GET_STATUS:                                        /* Get Status */
                    if( ( SetupReqBuf.bReqType & 0x1F ) == 0x00 )               /* 获取设备状态 */
                    {
                        if( ( ( UINT16 )SetupReqBuf.wValueH << 8 | SetupReqBuf.wValueL ) == 0x00 && 
                            ( ( UINT16 )SetupReqBuf.wIndexH << 8 | SetupReqBuf.wIndexL ) == 0x00 )
                        {
                            buf[ 0 ] = ( gUsbFlag & DEF_BIT_USB_WAKE ) ? 0x02 : 0x00;
                            buf[ 1 ] = 0x00;
                            pDescr = buf;
                            len = 2;
                        }
                        else 
                        {
                            status = 0xFF;                                      /* 操作失败 */    
                        }
                    }
                    else if( ( SetupReqBuf.bReqType & 0x1F ) == 0x02 )          /* 获取端点状态 */
                    {
                        if( ( ( UINT16 )SetupReqBuf.wValueH << 8 | SetupReqBuf.wValueL ) == 0x00 )
                        {
                            switch( ( UINT16 )SetupReqBuf.wIndexH << 8 | SetupReqBuf.wIndexL )   
                            {
                                case 0x82:
                                    buf[ 0 ] = ( USB_DEV_EP2_IN_MAXPKS & RB_STL_IEPx ) ? 0x01 : 0x00;
                                    break;

                                case 0x02:
                                    buf[ 0 ] = ( USB_DEV_EP2_OUT_MAXPKS & RB_STL_OEPx ) ? 0x01 : 0x00;
                                    break;

                                case 0x81:
                                    buf[ 0 ] = ( USB_DEV_EP1_IN_MAXPKS & RB_STL_IEPx ) ? 0x01 : 0x00;
                                    break;

                                default:
                                    status = 0xFF;                              /* 操作失败 */
                                    break;
                            }
                            buf[ 1 ] = 0x00;
                            len = 2;
                            pDescr = buf;                            
                        }
                        else 
                        {
                            status = 0xFF;                                      /* 操作失败 */
                        }
                    }
                    else 
                    {
                        status = 0xFF;                                          /* 操作失败 */
                    }
                    break;
                                
                default:
                    status = 0xFF;                                              /* 操作失败 */
                    break;
            }
        }
    }
    else 
    {
        status = 0xFF;                                                          /* 操作失败 */
    }
    
    if( status == 0xFF ) 
    {  
        /* 操作失败 */
        USBDev_EP0_Stall( );                                                    /* 端点0返回STALL */
    }
    else
    {
        gSetupLen = gSetupLen < len ? gSetupLen : len;                          /* 限制总长度 */
        if( gSetupLen )
        {                
            if( SetupReqBuf.bReqType & 0x80 )                                   /* 上传 */
            {                
                len = gSetupLen>=USB_EP0_MAX_PKT ? USB_EP0_MAX_PKT : gSetupLen; /* 计算本次传输长度 */                    
                memcpy( USB_Tran_Buf, pDescr, len );                            /* 将FlashROM中的const数据复制到RAM中，以便DMA存取 */ 
                  USBDev_WR_FIFOx( USB_DMA_ACC_CXF, len, (UINT8 *)USB_Tran_Buf ); /* 加载上传数据到FIFO 4 */
                gSetupLen -= len;                                               /* 计算传输长度 */
                pDescr += len;                                                  /* 计算指针偏移 */
                if( gSetupLen == 0 )
                {
                    USB_DEV_CX_CONF_FIFO_STATUS |= RB_DONE;                     /* 全部上传结束,设置端点0数据传输结束 */
                }
            }        
        }
        else                                            
        {
            USB_DEV_CX_CONF_FIFO_STATUS |= RB_DONE;                             /* 设置端点0数据传输结束 */    
        }    
    }    
}

/*******************************************************************************
* Function Name  : USBDev_EP0_IN_Deal
* Description    : USB控制端点IN包处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EP0_IN_Deal( void )
{
    UINT16 len;
        
    switch( gSetupReq ) 
    {
        case USB_REQ_GET_DESCR:        
            len = gSetupLen >= USB_EP0_MAX_PKT ? USB_EP0_MAX_PKT : gSetupLen;   /* 本次传输长度 */
            memcpy( USB_Tran_Buf, pDescr, len );                                /* 将FlashROM中的const数据复制到RAM中，以便DMA存取 */
            USBDev_WR_FIFOx( USB_DMA_ACC_CXF, len, (UINT8 *)USB_Tran_Buf );     /* 加载上传数据 */
            gSetupLen -= len;
            pDescr += len;                    
            if( len < USB_EP0_MAX_PKT )                                         /* 判断数据是否全部上传结束 */    
            {
                USB_DEV_CX_CONF_FIFO_STATUS |= RB_DONE;                         /* 设置端点0数据传输结束 */    
            }                
            break;
            
        default:
            break;
    }
}

/*******************************************************************************
* Function Name  : USBDev_EP0_OUT_Deal
* Description    : USB控制端点OUT包处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EP0_OUT_Deal( void )
{
    
}

/*******************************************************************************
* Function Name  : USBDev_EP2_IN_Deal
* Description    : USB端点2 IN包处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EP2_IN_Deal( void )
{
#if  MY_DEBUG_PRINTF         
    printf("USBDev_EP2_IN_Deal...\n");
#endif

    /* 上传缓冲区中的数据 */
    if( gUsbSpeed == USB_DEV_SPEED_HS )                                         /* 高速模式 */
    {
        USBDev_WR_FIFOx( USB_DMA_ACC_F1, gRecvLen, (PUINT8)USB_Tran_Buf );
    }
    else
    {
           USBDev_WR_FIFOx( USB_DMA_ACC_F1, gRecvLen, (PUINT8)USB_Tran_Buf );
    }
}

/*******************************************************************************
* Function Name  : USBDev_EP2_OUT_Deal
* Description    : USB端点2 OUT包处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EP2_OUT_Deal( void )
{
    UINT32    len;

#if  MY_DEBUG_PRINTF         
    printf("USBDev_EP2_OUT_Deal...\n");
#endif

    /* 从下传缓冲区中提取数据 */
    if( gUsbSpeed == USB_DEV_SPEED_HS )                                         /* 高速模式 */
    {
        len = USBDev_RD_FIFOx( USB_DMA_ACC_F2, 512, (PUINT8)USB_Recv_Buf );     /* 读取EP2_OUT包 */                
    }
    else
    {
           len = USBDev_RD_FIFOx( USB_DMA_ACC_F2, 64, (PUINT8)USB_Recv_Buf );      /* 读取EP2_OUT包 */                
    }

    /* 将数据取反后通过上传端点返回 */
    gRecvLen = len;
    for( len = 0; len < gRecvLen; len++ )
    {
        USB_Tran_Buf[ len ] = ~USB_Recv_Buf[ len ];    
    }
    USBDev_EP2_IN_Deal( );
}

/*******************************************************************************
* Function Name  : USBDev_EPx_Up_0BytePack
* Description    : USB设备EPx的IN端点上传0长度包
* Input          : epx_num---端点号(范围为:1---8)
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_Up_0BytePack( UINT8 epx_num )
{
    UINT32 addr;
    UINT32 reg;    
    
    addr = USB_BASE_ADDR + 0x160 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));
    (*((PUINT32V)addr)) = reg | RB_TX0BYTE_IEPx;        
}
                            
/*******************************************************************************
* Function Name  : USBDev_IRQHandler
* Description    : USB中断处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_IRQHandler( void )
{
    UINT32 grp_int, intstatus;

    /* 判断是否有USB设备中断产生,有则处理 */    
    if( USB_GL_INT_STATUS & RB_DEV_INT )                                        /* 判断是否有USB设备中断产生 */
    {
        USB_GL_INT_STATUS = USB_GL_INT_STATUS | RB_DEV_INT;                     /* 该位写1清0 */  

        grp_int = USB_DEV_INT_GROP;                                             /* 获取产生USB中断的中断源 */
        if( grp_int & 0x01 )                                                    /* grop0: control transfer interrupt */
        {
            intstatus = USB_DEV_INT_GROP0;             
            if( intstatus & RB_CX_COMABORT_INT  )                               /* R(0x140)位5： 控制传输失败 */
            {        
                USB_DEV_INT_GROP0 = intstatus | RB_CX_COMABORT_INT;             /* 清USB端点0 OUT中断(写1清0) */                        
            }    
            if( intstatus & RB_CX_COMFAIL_INT )                                 /* R(0x140)位4：控制端点存在IN、OUT失败 */
            {
                /* 该中断位只读,由硬件清0 */                
                USBDev_EP0_Stall( );                                            /* 端点0返回STALL */
            }
            if( intstatus & RB_CX_COMEND )                                      /* R(0x140)位3：进入状态阶段 */
            {
                /* 控制传输进入状态阶段 */
                /* 该中断位只读,由硬件清0 */
            }
            if( intstatus & RB_CX_OUT_INT )                                     /* R(0x140)位2：控制端点OUT中断 */
            {                
                /* 控制端点OUT中断处理 */                                    
                USBDev_EP0_OUT_Deal( );        
                        
                /* 该中断位只读,由硬件清0 */
            }
            if( intstatus & RB_CX_IN_INT )                                      /* R(0x140)位1：控制端点IN中断 */
            {
                /* 控制端点IN中断处理 */                                
                USBDev_EP0_IN_Deal( );
                
                /* 该中断位只读,由硬件清0 */
            }
            if( intstatus & RB_CX_SETUP_INT )                                   /* R(0x140)位0：控制端点SETUP中断 */
            {
                /* 控制端点SETUP中断处理 */
                USBDev_EP0_Setup_Deal( );
                
                /* 该中断位只读,由硬件清0 */
            }                        
        }    
        if( grp_int & 0x02 )                                                    /* grop1: bulk/ISO/Interrupt interrupt */
        {
            intstatus = USB_DEV_INT_GROP1;         

            /* 该寄存器所有中断位为只读,由硬件清0 */
            if( intstatus & RB_F3_IN_INT )                                      /* FIFO3 IN interrupt */
            {                
                USB_DEV_MASK_INT_GROP1 |= RB_MF3_IN_INT;                        /* 禁止FIFO3空中断 */
                /* 自行添加处理程序 */                                                
            }
            if( intstatus & RB_F2_IN_INT )                                      /* FIFO2 IN interrupt */
            {                
                USB_DEV_MASK_INT_GROP1 |= RB_MF2_IN_INT;                        /* 禁止FIFO2空中断 */
                /* 自行添加处理程序 */    
            }
            if( intstatus & RB_F1_IN_INT )                                      /* FIFO1 IN interrupt */
            {
                USB_DEV_MASK_INT_GROP1 |= RB_MF1_IN_INT;                        /* 禁止FIFO1空中断 */
                /* 自行添加处理程序 */                    
            }
            if( intstatus & RB_F0_IN_INT )                                      /* FIFO0 IN interrupt */
            {    
                USB_DEV_MASK_INT_GROP1 |= RB_MF0_IN_INT;                        /* 禁止FIFO0空中断 */
                /* 自行添加处理程序 */                                    
            }
            if( intstatus & RB_F3_SPK_INT )                                     /* FIFO3 Short OUT interrupt */
            {                
            }
            if( intstatus & RB_F3_OUT_INT )                                     /* FIFO3 OUT interrupt */
            {                
                /* 自行添加处理程序 */
            }
            if( intstatus & RB_F2_SPK_INT )                                     /* FIFO2 Short OUT interrupt */
            {                
            }
            if( intstatus & RB_F2_OUT_INT )                                     /* FIFO2 OUT interrupt */
            {                
                USBDev_EP2_OUT_Deal( );                                         /* 数据下传处理 */
            }
            if( intstatus & RB_F1_SPK_INT )                                     /* FIFO1 Short OUT interrupt */
            {                
            }
            if( intstatus & RB_F1_OUT_INT )                                     /* FIFO1 OUT interrupt */
            {
                /* 自行添加处理程序 */                            
            }
            if( intstatus & RB_F0_SPK_INT )                                     /* FIFO0 Short OUT interrupt */
            {                
            }
            if( intstatus & RB_F0_OUT_INT )                                     /* FIFO0 OUT interrupt */
            {                
                /* 自行添加处理程序 */
            }
        }    
        if( grp_int & 0x04 )                                                    /* grop2: bulk/ISO/Interrupt interrupt */ 
        {
            intstatus = USB_DEV_INT_GROP2; 
            
            if( intstatus & RB_DEV_WAKEUP_BYBUS )                               /* Dev_Wakeup_byVBUS interrupt */
            {                
                /* 设备由VBUS唤醒中断 */
                /* 该中断位只读,由硬件清0 */                
            }            
            if( intstatus & RB_DEV_IDLE )                                       /* Dev_Idle interrupt */
            {                
                /* 设备空闲中断 */
                /* 该中断位只读,由硬件清0 */                
            }            
            if( intstatus & RB_DMA_ERROR )                                      /* DMA Error interrupt */
            {                
                /* DMA错误中断 */
                USB_DEV_INT_GROP2 = RB_DMA_ERROR;                               /* 清DMA错误中断(写1清0) */                                
            }            
            if( intstatus & RB_DMA_CMPLT )                                      /* DMA Completion interrupt */
            {                
                /* DMA传输结束中断 */
                USB_DEV_INT_GROP2 = RB_DMA_CMPLT;                               /* 清DMA传输结束中断(写1清0) */                
            }            
            if( intstatus & RB_RX0BYTE_INT )                                    /* Received Zero-length Data Packet interrupt */
            {                
                /* 接收0长度数据包中断 */
                USB_DEV_INT_GROP2 |= RB_RX0BYTE_INT;                            /* 清接收0长度数据包中断(写1清0) */
                USB_DEV_RECV_0_LEN_PACK = 0x00;
            
                /* 自行添加处理,实际需要根据USB_DEV_RECV_0_LEN_PACK判断下传0长度包的端点,再进行处理 */
            }            
            if( intstatus & RB_TX0BYTE_INT )                                    /* Transferred Zero-length Data Packet interrupt */
            {                
                /* 发送0长度数据包中断 */
                USB_DEV_INT_GROP2 |= RB_TX0BYTE_INT;                            /* 清发送0长度数据包中断(写1清0) */
                USB_DEV_SEND_0_LEN_PACK = 0x00;                                 /* 清中断标志 */
                
                /* 自行添加处理,实际需要根据USB_DEV_SEND_0_LEN_PACK判断上传0长度包的端点,再进行处理 */                            
            }            
            if( intstatus & RB_ISO_SEQ_ABORT_INT )                              /* ISO Sequential Abort interrupt */
            {                                
                /* USB ISO异常中断 */
                USB_DEV_INT_GROP2 = RB_ISO_SEQ_ABORT_INT;                       /* 清USB ISO异常中断(写1清0) */            
            }            
            if( intstatus & RB_ISO_SEQ_ERR_INT )                                /* ISO Sequential Error interrupt */
            {            
                /* USB ISO错误中断 */
                USB_DEV_INT_GROP2 = RB_ISO_SEQ_ERR_INT;                         /* 清USB ISO错误中断(写1清0) */            
            }            
            if( intstatus & RB_RESMQ_INT )                                      /* Resume interrupt */
            {                
                /* USB总线恢复中断 */
                USB_DEV_INT_GROP2 = RB_RESMQ_INT;                               /* 清USB总线恢复中断(写1清0) */
                gUsbFlag &= ~DEF_BIT_USB_SUPD;                                  /* 清总线挂起标志 */
                USB_DEV_CONTROL &= ~RB_GOSUSP;                                  /* 清PHY挂起 */            
            }            
            if( intstatus & RB_SUSP_INT )                                       /* Suspend interrupt */
            {                
                /* USB总线挂起中断 */
                USB_DEV_INT_GROP2 = RB_SUSP_INT;                                /* 清USB总线挂起中断(写1清0) */    
                gUsbFlag |= DEF_BIT_USB_SUPD;                                   /* 置总线挂起标志 */
                USB_DEV_CONTROL |= RB_GOSUSP;                                   /* 置PHY挂起 */        
            }            
            if( intstatus & RB_USBRST_INT )                                     /* USB Reset interrupt */
            {    
                USB_DEV_INT_GROP2 = RB_USBRST_INT;            
                gUsbFlag = DEF_BIT_USB_RESET;                                   /*  清其它标志,设置复位标志 */
        
                /* USB总线复位,自动清地址 */                                                                     
                USB_DEV_ADDRESS = 0;                                            /* 清USB设备地址(实际复位时自动清0) */
            }
        }
    }
    else 
    {
        USB_GL_INT_STATUS = USB_GL_INT_STATUS | RB_HC_INT | RB_OTG_INT;         /* 该位写1清0 */  
    }        
}

/*********************************** endfile **********************************/
