/********************************** (C) COPYRIGHT *******************************
* File Name          : USBHOST.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24 
* Description        :
 USB host example for CH559, start USB device under DP/DM port
 USB主机应用例子,初始化和枚举DP/DM端口连接的USB设备,可以操作U盘或者操作USB鼠标,不包含HID类命令处理
 如果操作U盘，需要添加CH559UFX.LIB/CH559UFI.LIB文件系统库文件
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#define FOR_ROOT_UDISK_ONLY     0   // 只用于DP/DM端口的U盘文件操作(使用子程序库CH559UFI/X),否则本程序还简单演示HID操作
#include <CH559.H>
#include <stdio.h>
#include <string.h>
#include "../../USB_LIB/CH559UFI.H"
#include "../../USB_LIB/CH559UFI.C"

#pragma  NOAREGS

// 各子程序返回状态码
#define ERR_SUCCESS         0x00    // 操作成功
#define ERR_USB_CONNECT     0x15    /* 检测到USB设备连接事件,已经连接 */
#define ERR_USB_DISCON      0x16    /* 检测到USB设备断开事件,已经断开 */
#define ERR_USB_BUF_OVER    0x17    /* USB传输的数据有误或者数据太多缓冲区溢出 */
#define ERR_USB_DISK_ERR    0x1F    /* USB存储器操作失败,在初始化时可能是USB存储器不支持,在读写操作中可能是磁盘损坏或者已经断开 */
#define ERR_USB_TRANSFER    0x20    /* NAK/STALL等更多错误码在0x20~0x2F */
#define ERR_USB_UNSUPPORT   0xFB
#define ERR_USB_UNKNOWN     0xFE
// 获取设备描述符
UINT8C  SetupGetDevDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_DEVICE, 0x00, 0x00, sizeof( USB_DEV_DESCR ), 0x00 };
// 获取配置描述符
UINT8C  SetupGetCfgDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_CONFIG, 0x00, 0x00, 0x04, 0x00 };
// 设置USB地址
UINT8C  SetupSetUsbAddr[] = { USB_REQ_TYP_OUT, USB_SET_ADDRESS, USB_DEVICE_ADDR, 0x00, 0x00, 0x00, 0x00, 0x00 };
// 设置USB配置
UINT8C  SetupSetUsbConfig[] = { USB_REQ_TYP_OUT, USB_SET_CONFIGURATION, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// 清除端点STALL
UINT8C  SetupClrEndpStall[] = { USB_REQ_TYP_OUT | USB_REQ_RECIP_ENDP, USB_CLEAR_FEATURE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
UINT8X  UsbDevEndp0Size;             /* USB设备的端点0的最大包尺寸 */
//USB设备相关信息表,CH559最多支持2个设备
#define ROOT_DEV_DISCONNECT     0
#define ROOT_DEV_CONNECTED      1
#define ROOT_DEV_FAILED         2
#define ROOT_DEV_SUCCESS        3
#if !FOR_ROOT_UDISK_ONLY
#define DEV_TYPE_KEYBOARD   ( USB_DEV_CLASS_HID | 0x20 )
#define DEV_TYPE_MOUSE      ( USB_DEV_CLASS_HID | 0x30 )
struct _RootHubDev
{
    UINT8   DeviceStatus;                    // 设备状态,0-无设备,1-有设备但尚未初始化,2-有设备但初始化枚举失败,3-有设备且初始化枚举成功
//  UINT8   DeviceAddress;                   // 设备被分配的USB地址
    UINT8   DeviceSpeed;                     // 0为低速,非0为全速
    UINT8   DeviceType;                      // 设备类型
//  union {
//      struct MOUSE {
//          UINT8   MouseInterruptEndp;      // 鼠标中断端点号
//          UINT8   MouseIntEndpTog;         // 鼠标中断端点的同步标志
//          UINT8   MouseIntEndpSize;        // 鼠标中断端点的长度
//      }
//      struct PRINT {
//      }
//  }
//.....    struct  _Endp_Attr   Endp_Attr[4];//端点的属性,最多支持4个端点
    UINT8   GpVar;                           // 通用变量
} xdata ThisUsbDev;
#endif
#define WAIT_USB_TOUT_200US     200          // 等待USB中断超时时间200uS@Fsys=12MHz
/*
约定: USB设备地址分配规则(参考USB_DEVICE_ADDR)
地址值  设备位置
0x02    内置Root-HUB0下的USB设备或外部HUB
0x03    内置Root-HUB1下的USB设备或外部HUB
0x1x    内置Root-HUB0下的外部HUB的端口x下的USB设备,x为1~n
0x2x    内置Root-HUB1下的外部HUB的端口x下的USB设备,x为1~n
*/
UINT8X  RxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0000 ;  // IN, must even address
UINT8X  TxBuffer[ MAX_PACKET_SIZE * 8 ] _at_ 0x0800 ;  // OUT, must even address
#define pSetupReq   ((PXUSB_SETUP_REQ)TxBuffer)
bit     FoundNewDev;
#pragma NOAREGS
void    mDelayuS( UINT16 n );                // 以uS为单位延时
void    mDelaymS( UINT16 n );                // 以mS为单位延时
void    DisableRootHubPort( void );          // 关闭端口,实际上硬件已经自动关闭,此处只是清除一些结构状态
UINT8   AnalyzeRootHub( void );              // 分析端口状态,处理ROOT-HUB端口的设备插拔事件
// 返回ERR_SUCCESS为没有情况,返回ERR_USB_CONNECT为检测到新连接,返回ERR_USB_DISCON为检测到断开
void    SetHostUsbAddr( UINT8 addr );        // 设置USB主机当前操作的USB设备地址
#if FOR_ROOT_UDISK_ONLY
#define SetUsbSpeed( x )
#else
void    SetUsbSpeed( UINT8 FullSpeed );      // 设置当前USB速度
#endif
UINT8   ResetRootHubPort( void );            // 检测到设备后,复位总线,为枚举设备准备,设置为默认为全速
UINT8   EnableRootHubPort( void );           // 使能端口,相应的bUH_PORT_EN置1开启端口,设备断开可能导致返回失败
UINT8   WaitUSB_Interrupt( void );           // 等待USB中断
// CH559传输事务,输入目的端点地址/PID令牌,同步标志,以20uS为单位的NAK重试总时间(0则不重试,0xFFFF无限重试),返回0成功,超时/出错重试
UINT8   USBHostTransact( UINT8 endp_pid, UINT8 tog, UINT16 timeout );  // endp_pid: 高4位是token_pid令牌, 低4位是端点地址
UINT8   HostCtrlTransfer( PUINT8 pBuf, PUINT16 pLen );  // 执行控制传输,8字节请求码在pSetupReq中,DataBuf为可选的收发缓冲区
// 如果需要接收和发送数据,那么DataBuf需指向有效缓冲区用于存放后续数据,实际成功收发的总长度返回保存在ReqLen指向的字节变量中
void    CopySetupReqPkg( PUINT8C pReqPkt );  // 复制控制传输的请求包
UINT8   CtrlGetDeviceDescr( void );          // 获取设备描述符,返回在TxBuffer中
UINT8   CtrlGetConfigDescr( void );          // 获取配置描述符,返回在TxBuffer中
UINT8   CtrlSetUsbAddress( UINT8 addr );     // 设置USB设备地址
UINT8   CtrlSetUsbConfig( UINT8 cfg );       // 设置USB设备配置
UINT8   CtrlClearEndpStall( UINT8 endp );    // 清除端点STALL
#if !FOR_ROOT_UDISK_ONLY
UINT8   AnalyzeHidIntEndp( PUINT8X buf );    // 从描述符中分析出HID中断端点的地址
#endif
UINT8   InitRootDevice( void );              // 初始化USB设备
/* 为printf和getkey输入输出初始化串口 */
void    mInitSTDIO( void );
void    InitUSB_Host( void );                // 初始化USB主机
//#define   FREQ_SYS    12000000             // 系统主频12MHz
/*******************************************************************************
* Function Name  : mDelayus(UNIT16 n)
* Description    : us延时函数
* Input          : UNIT16 n
* Output         : None
* Return         : None
*******************************************************************************/
void    mDelayuS( UINT16 n )                                 // 以uS为单位延时
{
    while ( n )                              // total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
    {
        ++ SAFE_MOD;                         // 2 Fsys cycles, for higher Fsys, add operation here
#ifdef  FREQ_SYS
#if     FREQ_SYS >= 14000000
        ++ SAFE_MOD;
#endif
#if     FREQ_SYS >= 16000000
        ++ SAFE_MOD;
#endif
#if     FREQ_SYS >= 18000000
        ++ SAFE_MOD;
#endif
#if     FREQ_SYS >= 20000000
        ++ SAFE_MOD;
#endif
#if     FREQ_SYS >= 22000000
        ++ SAFE_MOD;
#endif
#if     FREQ_SYS >= 24000000
        ++ SAFE_MOD;
#endif
#if     FREQ_SYS >= 26000000
        ++ SAFE_MOD;
#endif
#if     FREQ_SYS >= 28000000
        ++ SAFE_MOD;
#endif
#if     FREQ_SYS >= 30000000
        ++ SAFE_MOD;
#endif
#if     FREQ_SYS >= 32000000
        ++ SAFE_MOD;
#endif
#if     FREQ_SYS >= 34000000
        ++ SAFE_MOD;
#endif
#if     FREQ_SYS >= 36000000
        ++ SAFE_MOD;
#endif
#if     FREQ_SYS >= 38000000
        ++ SAFE_MOD;
#endif
#if     FREQ_SYS >= 40000000
        ++ SAFE_MOD;
#endif
#endif
        -- n;
    }
}
/*******************************************************************************
* Function Name  : mDelayms(UNIT16 n)
* Description    : ms延时函数
* Input          : UNIT16 n
* Output         : None
* Return         : None
*******************************************************************************/
void    mDelaymS( UINT16 n )                                 // 以mS为单位延时
{
    while ( n )
    {
        mDelayuS( 1000 );
        -- n;
    }
}
/*******************************************************************************
* Function Name  : DisableRootHubPort(void)
* Description    : 关闭HUB端口
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void    DisableRootHubPort( void )                           // 关闭端口,实际上硬件已经自动关闭,此处只是清除一些结构状态
{
    CH559DiskStatus = DISK_DISCONNECT;
#ifndef FOR_ROOT_UDISK_ONLY
    ThisUsbDev.DeviceStatus = ROOT_DEV_DISCONNECT;
//  ThisUsbDev.DeviceAddress = 0x00;
#endif
    UHUB0_CTRL = 0x00;                                       // 清除有关HUB0的控制数据,实际上不需要清除
}
/*******************************************************************************
* Function Name  : AnalyzeRootHub(void)
* Description    : 分析端口状态,处理端口的设备插拔事件
                   处理端口的插拔事件,如果设备拔出,函数中调用DisableRootHubPort()函数,将端口关闭,插入事件,置相应端口的状态位
* Input          : None
* Output         : None
* Return         : 返回ERR_SUCCESS为没有情况,返回ERR_USB_CONNECT为检测到新连接,返回ERR_USB_DISCON为检测到断开
*******************************************************************************/
UINT8   AnalyzeRootHub( void )
{
    UINT8   s;
    s = ERR_SUCCESS;
    if ( USB_HUB_ST & bUHS_H0_ATTACH )                       // 设备存在
    {
#if FOR_ROOT_UDISK_ONLY
        if ( CH559DiskStatus == DISK_DISCONNECT
#else
        if ( ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT  // 检测到有设备插入
#endif
                || ( UHUB0_CTRL & bUH_PORT_EN ) == 0x00 )    // 检测到有设备插入,但尚未允许,说明是刚插入
        {
            DisableRootHubPort( );                           // 关闭端口
#if FOR_ROOT_UDISK_ONLY
            CH559DiskStatus = DISK_CONNECT;
#else
//          ThisUsbDev.DeviceSpeed = USB_HUB_ST & bUHS_DM_LEVEL ? 0 : 1;
            ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED;    //置连接标志
#endif
            printf( "USB dev in\n" );
            s = ERR_USB_CONNECT;
        }
    }
#if FOR_ROOT_UDISK_ONLY
    else if ( CH559DiskStatus >= DISK_CONNECT )
    {
#else
    else if ( ThisUsbDev.DeviceStatus >= ROOT_DEV_CONNECTED ) //检测到设备拔出
    {
#endif
        DisableRootHubPort( );                                // 关闭端口
        printf( "USB dev out\n" );
        if ( s == ERR_SUCCESS )
        {
            s = ERR_USB_DISCON;
        }
    }
//  UIF_DETECT = 0;                                           // 清中断标志
    return( s );
}
/*******************************************************************************
* Function Name  : SetHostUsbAddr
* Description    : 设置USB主机当前操作的USB设备地址
* Input          : UINT8 addr
* Output         : None
* Return         : None
*******************************************************************************/
void  SetHostUsbAddr( UINT8 addr )
{
    USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | addr & 0x7F;
}
/*******************************************************************************
* Function Name  : SetUsbSpeed
* Description    : 设置当前USB速度
* Input          : UINT8 FullSpeed
* Output         : None
* Return         : None
*******************************************************************************/
#if !FOR_ROOT_UDISK_ONLY
void    SetUsbSpeed( UINT8 FullSpeed )                          //非U盘操作
{
    if ( FullSpeed )                                            // 全速
    {
        USB_CTRL &= ~ bUC_LOW_SPEED;                            // 全速
        UH_SETUP &= ~ bUH_PRE_PID_EN;                           // 禁止PRE PID
    }
    else
    {
        USB_CTRL |= bUC_LOW_SPEED;                              // 低速
    }
}
#endif
/*******************************************************************************
* Function Name  : ResetRootHubPort
* Description    : 检测到设备后,复位总线,为枚举设备准备,设置为默认为全速
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8   ResetRootHubPort( void )
{
    UINT16 n = 30000;
	UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;                        //USB设备的端点0的最大包尺寸
    SetHostUsbAddr( 0x00 );			
    SetUsbSpeed( 1 );                                            // 默认为全速
    UHUB0_CTRL = UHUB0_CTRL & ~ bUH_LOW_SPEED | bUH_BUS_RESET;// 默认为全速,开始复位
    while( n ) 
	{  
		++ SAFE_MOD;  												
		++ SAFE_MOD;
		++ SAFE_MOD;
		++ SAFE_MOD;
		++ SAFE_MOD;
		++ SAFE_MOD;
		++ SAFE_MOD;
		++ SAFE_MOD;
		++ SAFE_MOD;
		if( USB_HUB_ST & ( bUHS_DP_PIN | bUHS_DM_PIN ) )
		{
			UHUB0_CTRL = UHUB0_CTRL & ~bUH_BUS_RESET;  						/* 结束复位 */
			return( 0x01 );
		}
		-- n;
	}
    UHUB0_CTRL = UHUB0_CTRL & ~ bUH_BUS_RESET;               // 结束复位
    mDelayuS( 200 );
    UIF_DETECT = 0;											     // 清连接中断标志
	return ( 0x00 );
}
/*******************************************************************************
* Function Name  : EnableRootHubPort
* Description    : 使能端口,相应的bUH_PORT_EN置1开启端口,设备断开可能导致返回失败
* Input          : None
* Output         : None
* Return         : 返回ERR_SUCCESS为检测到新连接,返回ERR_USB_DISCON为无连接
*******************************************************************************/
UINT8   EnableRootHubPort( void )
{
#if FOR_ROOT_UDISK_ONLY
    if ( CH559DiskStatus < DISK_CONNECT )
    {
        CH559DiskStatus = DISK_CONNECT;
    }
#else
    if ( ThisUsbDev.DeviceStatus < ROOT_DEV_CONNECTED )
    {
        ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED;
    }
#endif
    if ( USB_HUB_ST & bUHS_H0_ATTACH )                           // 有设备
    {
#if !FOR_ROOT_UDISK_ONLY
        if ( ( UHUB0_CTRL & bUH_PORT_EN ) == 0x00 )              // 尚未使能
        {
            ThisUsbDev.DeviceSpeed = USB_HUB_ST & bUHS_DM_LEVEL ? 0 : 1;
            if ( ThisUsbDev.DeviceSpeed == 0 )
            {
                UHUB0_CTRL |= bUH_LOW_SPEED;                     // 低速
            }
        }
#endif
        UHUB0_CTRL |= bUH_PORT_EN;                               //使能HUB端口
        return( ERR_SUCCESS );
    }
    return( ERR_USB_DISCON );
}
/*******************************************************************************
* Function Name  : WaitUSB_Interrupt
* Description    : 等待USB中断
* Input          : None
* Output         : None
* Return         : 返回ERR_SUCCESS 数据接收或者发送成功
                   ERR_USB_UNKNOWN 数据接收或者发送失败
*******************************************************************************/
UINT8   WaitUSB_Interrupt( void )
{
    UINT16  i;
    for ( i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i -- )
    {
        ;
    }
    return( UIF_TRANSFER ? ERR_SUCCESS : ERR_USB_UNKNOWN );
}
/*******************************************************************************
* Function Name  : USBHostTransact
* Description    : CH559传输事务,输入目的端点地址/PID令牌,同步标志,以20uS为单位的NAK重试总时间(0则不重试,0xFFFF无限重试),返回0成功,超时/出错重试
                   本子程序着重于易理解,而在实际应用中,为了提供运行速度,应该对本子程序代码进行优化
* Input          : UINT8 endp_pid 令牌和地址  endp_pid: 高4位是token_pid令牌, 低4位是端点地址
                   UINT8 tog      同步标志
                   UINT16 timeout 超时时间
* Output         : None
* Return         : ERR_USB_UNKNOWN 超时，可能硬件异常
                   ERR_USB_DISCON  设备断开
                   ERR_USB_CONNECT 设备连接
                   ERR_SUCCESS     传输完成
*******************************************************************************/
UINT8   USBHostTransact( UINT8 endp_pid, UINT8 tog, UINT16 timeout )
{
//  UINT8   TransRetry;
#define TransRetry  UEP0_T_LEN                                     // 节约内存
    UINT8   s, r;
    UINT16  i;
    UH_RX_CTRL = UH_TX_CTRL = tog;
    TransRetry = 0;
    do
    {
        UH_EP_PID = endp_pid;                                       // 指定令牌PID和目的端点号
        UIF_TRANSFER = 0;                                           // 允许传输
//      s = WaitUSB_Interrupt( );
        for ( i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i -- )
        {
            ;
        }
        UH_EP_PID = 0x00;                                           // 停止USB传输
//      if ( s != ERR_SUCCESS ) return( s );                        // 中断超时,可能是硬件异常
        if ( UIF_TRANSFER == 0 )
        {
            return( ERR_USB_UNKNOWN );
        }
        if ( UIF_DETECT )                                           // USB设备插拔事件
        {
//          mDelayuS( 200 );                                        // 等待传输完成
            UIF_DETECT = 0;                                         // 清中断标志
            s = AnalyzeRootHub( );                                  // 分析ROOT-HUB状态
            if ( s == ERR_USB_CONNECT )
            {
                FoundNewDev = 1;
            }
#if FOR_ROOT_UDISK_ONLY
            if ( CH559DiskStatus == DISK_DISCONNECT )
            {
                return( ERR_USB_DISCON );                           // USB设备断开事件
            }
            if ( CH559DiskStatus == DISK_CONNECT )
            {
                return( ERR_USB_CONNECT );                          // USB设备连接事件
            }
#else
            if ( ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT )
            {
                return( ERR_USB_DISCON );                           // USB设备断开事件
            }
            if ( ThisUsbDev.DeviceStatus == ROOT_DEV_CONNECTED )
            {
                return( ERR_USB_CONNECT );                          // USB设备连接事件
            }
#endif
//          if ( ( USB_HUB_ST & bUHS_H0_ATTACH ) == 0x00 ) return( ERR_USB_DISCON );  // USB设备断开事件
            mDelayuS( 200 );                                        // 等待传输完成
        }
        if ( UIF_TRANSFER )                                         // 传输完成
        {
            if ( U_TOG_OK )
            {
                return( ERR_SUCCESS );
            }
            r = USB_INT_ST & MASK_UIS_H_RES;                         // USB设备应答状态
            if ( r == USB_PID_STALL )
            {
                return( r | ERR_USB_TRANSFER );
            }
            if ( r == USB_PID_NAK )
            {
                if ( timeout == 0 )
                {
                    return( r | ERR_USB_TRANSFER );
                }
                if ( timeout < 0xFFFF )
                {
                    timeout --;
                }
                -- TransRetry;
            }
            else switch ( endp_pid >> 4 )
                {
                case USB_PID_SETUP:
                case USB_PID_OUT:
//                  if ( U_TOG_OK ) return( ERR_SUCCESS );
//                  if ( r == USB_PID_ACK ) return( ERR_SUCCESS );
//                  if ( r == USB_PID_STALL || r == USB_PID_NAK ) return( r | ERR_USB_TRANSFER );
                    if ( r )
                    {
                        return( r | ERR_USB_TRANSFER );                 // 不是超时/出错,意外应答
                    }
                    break;                                              // 超时重试
                case USB_PID_IN:
//                  if ( U_TOG_OK ) return( ERR_SUCCESS );
//                  if ( tog ? r == USB_PID_DATA1 : r == USB_PID_DATA0 ) return( ERR_SUCCESS );
//                  if ( r == USB_PID_STALL || r == USB_PID_NAK ) return( r | ERR_USB_TRANSFER );
                    if ( r == USB_PID_DATA0 && r == USB_PID_DATA1 )     // 不同步则需丢弃后重试
                    {
                    }                                                   // 不同步重试
                    else if ( r )
                    {
                        return( r | ERR_USB_TRANSFER );                 // 不是超时/出错,意外应答
                    }
                    break;                                              // 超时重试
                default:
                    return( ERR_USB_UNKNOWN );                          // 不可能的情况
                    break;
                }
        }
        else                                                            // 其它中断,不应该发生的情况
        {
            USB_INT_FG = 0xFF;                                          /* 清中断标志 */
        }
        mDelayuS( 15 );
    }
    while ( ++ TransRetry < 3 );
    return( ERR_USB_TRANSFER );                                         // 应答超时
}
/*******************************************************************************
* Function Name  : HostCtrlTransfer
* Description    : 执行控制传输,8字节请求码在pSetupReq中,DataBuf为可选的收发缓冲区
* Input          : PUINT8X DataBuf 如果需要接收和发送数据,那么DataBuf需指向有效缓冲区用于存放后续数据
                   PUINT8I RetLen  实际成功收发的总长度保存在RetLen指向的字节变量中
* Output         : None
* Return         : ERR_USB_BUF_OVER IN状态阶段出错
                   ERR_SUCCESS     数据交换成功
                   其他错误状态
*******************************************************************************/
UINT8   HostCtrlTransfer( PUINT8 pBuf, PUINT16 pLen )
{
	UINT8   s;
    UINT16  RemLen, RxLen, RxCnt, TxCnt;
	
    mDelayuS( 200 );
    if ( pLen )
    {
        *pLen = 0;                                                        // 实际成功收发的总长度
    }
    UH_TX_LEN = sizeof( USB_SETUP_REQ );
    s = USBHostTransact( USB_PID_SETUP << 4 | 0x00, 0x00, 200000/20 );    // SETUP阶段,200mS超时
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    UH_RX_CTRL = UH_TX_CTRL = bUH_R_TOG | bUH_R_AUTO_TOG | bUH_T_TOG | bUH_T_AUTO_TOG;  // 默认DATA1
    UH_TX_LEN = 0x01;                                                     // 默认无数据故状态阶段为IN
    RemLen = ((UINT16)pSetupReq -> wLengthH <<8 ) | pSetupReq -> wLengthL;
    if ( RemLen && pBuf )                                                 // 需要收发数据
    {
        if ( pSetupReq -> bRequestType & USB_REQ_TYP_IN )                 // 收
        {
            while ( RemLen )
            {
                mDelayuS( 200 );
                s = USBHostTransact( USB_PID_IN << 4 | 0x00, UH_RX_CTRL, 200000/20 );  // IN数据
                if ( s != ERR_SUCCESS )
                {
                    return( s );
                }
                RxLen = USB_RX_LEN < RemLen ? USB_RX_LEN : RemLen;
                RemLen -= RxLen;
                if ( pLen )
                {
                    *pLen += RxLen;                                        // 实际成功收发的总长度
                }
//              memcpy( pBuf, RxBuffer, RxLen );
//              pBuf += RxLen;
                for ( RxCnt = 0; RxCnt != RxLen; RxCnt ++ )
                {
                    *pBuf = RxBuffer[ RxCnt ];
                    pBuf ++;
                }
                if ( USB_RX_LEN == 0 || ( USB_RX_LEN & ( UsbDevEndp0Size - 1 ) ) )
                {
                    break;                                                 // 短包
                }
            }
            UH_TX_LEN = 0x00;                                              // 状态阶段为OUT
        }
        else                                                               // 发
        {
            while ( RemLen )
            {
                mDelayuS( 200 );
                UH_TX_LEN = RemLen >= UsbDevEndp0Size ? UsbDevEndp0Size : RemLen;
//              memcpy( TxBuffer, pBuf, UH_TX_LEN );
//              pBuf += UH_TX_LEN;
                for ( TxCnt = 0; TxCnt != UH_TX_LEN; TxCnt ++ )
                {
                    TxBuffer[ TxCnt ] = *pBuf;
                    pBuf ++;
                }
                s = USBHostTransact( USB_PID_OUT << 4 | 0x00, UH_TX_CTRL, 200000/20 );// OUT数据
                if ( s != ERR_SUCCESS )
                {
                    return( s );
                }
                RemLen -= UH_TX_LEN;
                if ( pLen )
                {
                    *pLen += UH_TX_LEN;                                      // 实际成功收发的总长度
                }
            }
//          UH_TX_LEN = 0x01;                                                // 状态阶段为IN
        }
    }
    mDelayuS( 200 );
    s = USBHostTransact( ( UH_TX_LEN ? USB_PID_IN << 4 | 0x00: USB_PID_OUT << 4 | 0x00 ), bUH_R_TOG | bUH_T_TOG, 200000/20 );  // STATUS阶段
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( UH_TX_LEN == 0 )
    {
        return( ERR_SUCCESS );                                                // 状态OUT
    }
    if ( USB_RX_LEN == 0 )
    {
        return( ERR_SUCCESS );                                                // 状态IN,检查IN状态返回数据长度
    }
    return( ERR_USB_BUF_OVER );                                               // IN状态阶段错误
}
/*******************************************************************************
* Function Name  : CopySetupReqPkg
* Description    : 复制控制传输的请求包
* Input          : PUINT8C pReqPkt 控制请求包地址
* Output         : None
* Return         : None
*******************************************************************************/
void    CopySetupReqPkg( PUINT8C pReqPkt )
{
    UINT8   i;
    for ( i = 0; i != sizeof( USB_SETUP_REQ ); i ++ )
    {
        ((PUINT8X)pSetupReq)[ i ] = *pReqPkt;
        pReqPkt ++;
    }
}
/*******************************************************************************
* Function Name  : CtrlGetDeviceDescr
* Description    : 获取设备描述符,返回在TxBuffer中
* Input          : None
* Output         : None
* Return         : ERR_USB_BUF_OVER 描述符长度错误
                   ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8   CtrlGetDeviceDescr( void )
{
    UINT8   s;
    UINT16  len;
    UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;
    CopySetupReqPkg( SetupGetDevDescr );
    s = HostCtrlTransfer( TxBuffer, &len );                                    // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    UsbDevEndp0Size = ( (PXUSB_DEV_DESCR)TxBuffer ) -> bMaxPacketSize0;        // 端点0最大包长度,这是简化处理,正常应该先获取前8字节后立即更新UsbDevEndp0Size再继续
    if ( len < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL )
    {
        return( ERR_USB_BUF_OVER );                                            // 描述符长度错误
    }
    return( ERR_SUCCESS );
}
/*******************************************************************************
* Function Name  : CtrlGetConfigDescr
* Description    : 获取配置描述符,返回在TxBuffer中
* Input          : None
* Output         : None
* Return         : ERR_USB_BUF_OVER 描述符长度错误
                   ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8   CtrlGetConfigDescr( void )
{
    UINT8   s;
    UINT16  len;
    CopySetupReqPkg( SetupGetCfgDescr );
    s = HostCtrlTransfer( TxBuffer, &len );                                    // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < ( (PUSB_SETUP_REQ)SetupGetCfgDescr ) -> wLengthL )
    {
        return( ERR_USB_BUF_OVER );                                             // 返回长度错误
    }
    len = (UINT16)(( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthH<<8)|( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthL;
//    if ( len > MAX_PACKET_SIZE )
//    {
//        return( ERR_USB_BUF_OVER );                                             // 返回长度错误
//    }
    CopySetupReqPkg( SetupGetCfgDescr );
    pSetupReq -> wLengthL = (UINT8)len;                                                // 完整配置描述符的总长度
	pSetupReq -> wLengthH = (UINT8)(len>>8);
    s = HostCtrlTransfer( TxBuffer, &len );                                     // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < ( ((UINT16)((PUSB_SETUP_REQ)SetupGetCfgDescr ) -> wLengthH<<8)|( (PUSB_SETUP_REQ)SetupGetCfgDescr ) -> wLengthL ) || 
		 len < ( ((UINT16)((PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthH<<8)|((PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthL)  )
    {
        return( ERR_USB_BUF_OVER );                                              // 描述符长度错误
    }
    return( ERR_SUCCESS );
}
/*******************************************************************************
* Function Name  : CtrlSetUsbAddress
* Description    : 设置USB设备地址
* Input          : UINT8 addr 设备地址
* Output         : None
* Return         : ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8   CtrlSetUsbAddress( UINT8 addr )
{
    UINT8   s;
    CopySetupReqPkg( SetupSetUsbAddr );
    pSetupReq -> wValueL = addr;                                                 // USB设备地址
    s = HostCtrlTransfer( NULL, NULL );                                          // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    SetHostUsbAddr( addr );                                                      // 设置USB主机当前操作的USB设备地址
    mDelaymS( 10 );                                                              // 等待USB设备完成操作
    return( ERR_SUCCESS );
}
/*******************************************************************************
* Function Name  : CtrlSetUsbConfig
* Description    : 设置USB设备配置
* Input          : UINT8 cfg       配置值
* Output         : None
* Return         : ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8   CtrlSetUsbConfig( UINT8 cfg )
{
    CopySetupReqPkg( SetupSetUsbConfig );
    pSetupReq -> wValueL = cfg;                                                  // USB设备配置
    return( HostCtrlTransfer( NULL, NULL ) );                                    // 执行控制传输
}
/*******************************************************************************
* Function Name  : CtrlClearEndpStall
* Description    : 清除端点STALL
* Input          : UINT8 endp       端点地址
* Output         : None
* Return         : ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8   CtrlClearEndpStall( UINT8 endp )
{
    CopySetupReqPkg( SetupClrEndpStall );                                        // 清除端点的错误
    pSetupReq -> wIndexL = endp;                                                 // 端点地址
    return( HostCtrlTransfer( NULL, NULL ) );                                    /* 执行控制传输 */
}
/*******************************************************************************
* Function Name  : AnalyzeHidIntEndp
* Description    : 从描述符中分析出HID中断端点的地址
* Input          : PUINT8X buf       待分析数据缓冲区地址
* Output         : None
* Return         : 中断端点地址
*******************************************************************************/
UINT8   AnalyzeHidIntEndp( PUINT8X buf )
{
    UINT8   i, s, l;
    s = 0;
    for ( i = 0; i < ( (PXUSB_CFG_DESCR)buf ) -> wTotalLengthL; i += l )         // 搜索中断端点描述符,跳过配置描述符和接口描述符
    {
        if ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bDescriptorType == USB_DESCR_TYP_ENDP  // 是端点描述符
                && ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bmAttributes & USB_ENDP_TYPE_MASK ) == USB_ENDP_TYPE_INTER  // 是中断端点
                && ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_DIR_MASK ) )// 是IN端点
        {
            s = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;// 中断端点的地址
            break;                                                               // 可以根据需要保存wMaxPacketSize和bInterval
        }
        l = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bLength;                            // 当前描述符长度,跳过
        if ( l > 16 )
        {
            break;
        }
    }
    return( s );
}
/*******************************************************************************
* Function Name  : InitRootDevice
* Description    : 初始化USB设备
* Input          : None
* Output         : None
* Return         :
*******************************************************************************/
UINT8   InitRootDevice( void )
{
    UINT8   s, cfg, dv_cls, if_cls;
	UINT16  i;
    printf( "Reset host port\n" );
    ResetRootHubPort( );                                                         // 检测到设备后,复位相应端口的USB总线
    for ( i = 0, s = 0; i < 100; i ++ )                                          // 等待USB设备复位后重新连接,100mS超时
    {
        mDelaymS( 1 );
        if ( EnableRootHubPort( ) == ERR_SUCCESS )                               // 使能端口
        {
            i = 0;
            s ++;                                                                // 计时等待USB设备连接后稳定
            if ( s > 15 )
            {
                break;                                                           // 已经稳定连接15mS
            }
        }
    }
    if ( i )                                                                     // 复位后设备没有连接
    {
        DisableRootHubPort( );
        printf( "Disable host port because of disconnect\n" );
        return( ERR_USB_DISCON );
    }
    SetUsbSpeed( ThisUsbDev.DeviceSpeed );                                       // 设置当前USB速度
    printf( "GetDevDescr: " );    
	s = CtrlGetDeviceDescr( );                                                   // 获取设备描述符
    if ( s == ERR_SUCCESS )
    {
        for ( i = 0; i < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL; i ++ )
        {
            printf( "x%02X ", (UINT16)( TxBuffer[i] ) );
        }
        printf( "\n" );                                                           // 显示出描述符
        dv_cls = ( (PXUSB_DEV_DESCR)TxBuffer ) -> bDeviceClass;                   // 设备类代码
        s = CtrlSetUsbAddress( ( (PUSB_SETUP_REQ)SetupSetUsbAddr ) -> wValueL );  // 设置USB设备地址
        if ( s == ERR_SUCCESS )
        {
//          ThisUsbDev.DeviceAddress = ( (PUSB_SETUP_REQ)SetupSetUsbAddr ) -> wValueL;// 保存USB地址
            printf( "GetCfgDescr: " );
            s = CtrlGetConfigDescr( );                                            // 获取配置描述符
            if ( s == ERR_SUCCESS )
            {
                cfg = ( (PXUSB_CFG_DESCR)TxBuffer ) -> bConfigurationValue;
                for ( i = 0; i < (((UINT16)( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthH<<8)|( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthL); i ++ )
                {
                    printf( "x%02X ", (UINT16)( TxBuffer[i] ) );
                }
                printf("\n");
                /* 分析配置描述符,获取端点数据/各端点地址/各端点大小等,更新变量endp_addr和endp_size等 */
                if_cls = ( (PXUSB_CFG_DESCR_LONG)TxBuffer ) -> itf_descr.bInterfaceClass;  // 接口类代码
                if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE )          // 是USB存储类设备,基本上确认是U盘
                {
#if FOR_ROOT_UDISK_ONLY
                    CH559DiskStatus = DISK_USB_ADDR;
                    return( ERR_SUCCESS );
                }
                else
                {
                    return( ERR_USB_UNSUPPORT );
                }
#else
                    s = CtrlSetUsbConfig( cfg );                                   // 设置USB设备配置
                    if ( s == ERR_SUCCESS )
                    {
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsbDev.DeviceType = USB_DEV_CLASS_STORAGE;
                        printf( "USB-Disk Ready\n" );
                        SetUsbSpeed( 1 );                                          // 默认为全速
                        return( ERR_SUCCESS );
                    }
                }
                else if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_PRINTER && ( (PXUSB_CFG_DESCR_LONG)TxBuffer ) -> itf_descr.bInterfaceSubClass == 0x01 )    // 是打印机类设备
                {
                    s = CtrlSetUsbConfig( cfg );                                   // 设置USB设备配置
                    if ( s == ERR_SUCCESS )
                    {
                        //需保存端点信息以便主程序进行USB传输
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsbDev.DeviceType = USB_DEV_CLASS_PRINTER;
                        printf( "USB-Print Ready\n" );
                        SetUsbSpeed( 1 );                                          // 默认为全速
                        return( ERR_SUCCESS );
                    }
                }
                else if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_HID && ( (PXUSB_CFG_DESCR_LONG)TxBuffer ) -> itf_descr.bInterfaceSubClass <= 0x01 )    // 是HID类设备,键盘/鼠标等
                {
                    s = AnalyzeHidIntEndp( TxBuffer );                              // 从描述符中分析出HID中断端点的地址
                    ThisUsbDev.GpVar = s & USB_ENDP_ADDR_MASK ;                     // 保存中断端点的地址,位7用于同步标志位,清0
                    if_cls = ( (PXUSB_CFG_DESCR_LONG)TxBuffer ) -> itf_descr.bInterfaceProtocol;
                    s = CtrlSetUsbConfig( cfg );                                    // 设置USB设备配置
                    if ( s == ERR_SUCCESS )
                    {
//                      Set_Idle( );
                        //需保存端点信息以便主程序进行USB传输
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        if ( if_cls == 1 )
                        {
                            ThisUsbDev.DeviceType = DEV_TYPE_KEYBOARD;
                            //进一步初始化,例如设备键盘指示灯LED等
                            printf( "USB-Keyboard Ready\n" );
                            SetUsbSpeed( 1 );                                       // 默认为全速
                            return( ERR_SUCCESS );
                        }
                        else if ( if_cls == 2 )
                        {
                            ThisUsbDev.DeviceType = DEV_TYPE_MOUSE;
                            //为了以后查询鼠标状态,应该分析描述符,取得中断端口的地址,长度等信息
                            printf( "USB-Mouse Ready\n" );
                            SetUsbSpeed( 1 );                                       // 默认为全速
                            return( ERR_SUCCESS );
                        }
                        s = ERR_USB_UNSUPPORT;
                    }
                }
                else                                                                // 可以进一步分析
                {
                    s = CtrlSetUsbConfig( cfg );                                    // 设置USB设备配置
                    if ( s == ERR_SUCCESS )
                    {
                        //需保存端点信息以便主程序进行USB传输
                        ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
                        ThisUsbDev.DeviceStatus = dv_cls ? dv_cls : if_cls;
                        SetUsbSpeed( 1 );                                           // 默认为全速
                        return( ERR_SUCCESS );                                      /* 未知设备初始化成功 */
                    }
                }
#endif
            }
        }
    }
    printf( "InitRootDev Err = %02X\n", (UINT16)s );
#if FOR_ROOT_UDISK_ONLY
    CH559DiskStatus = DISK_CONNECT;
#else
    ThisUsbDev.DeviceStatus = ROOT_DEV_FAILED;
#endif
    SetUsbSpeed( 1 );                                                                // 默认为全速
    return( s );
}

/*******************************************************************************
* Function Name  : mInitSTDIO
* Description    : 为printf和getkey输入输出初始化串口
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void    mInitSTDIO( void )
{
//  SCON = 0x50;
    SM0 = 0;
    SM1 = 1;
    SM2 = 0;
    REN = 1;
    PCON |= SMOD;
    TMOD = TMOD & ~ bT1_GATE & ~ bT1_CT & ~ MASK_T1_MOD | bT1_M1;                    // 0X20
    T2MOD = T2MOD | bTMR_CLK | bT1_CLK;                                              // *12
    TH1 = 0-13;//0xF3;                                                               /* 12MHz晶振, 4800bps*12=57600bps */
    TR1 = 1;
    TI = 1;
}
/*******************************************************************************
* Function Name  : InitUSB_Host
* Description    : 初始化USB主机
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void    InitUSB_Host( void )
{
    IE_USB = 0;
//  LED_CFG = 1;
//  LED_RUN = 0;
    USB_CTRL = bUC_HOST_MODE;                                                        // 先设定模式
    USB_DEV_AD = 0x00;
    UH_EP_MOD = bUH_EP_TX_EN | bUH_EP_RX_EN ;
    UH_RX_DMA = RxBuffer;
    UH_TX_DMA = TxBuffer;
    UH_RX_CTRL = 0x00;
    UH_TX_CTRL = 0x00;
    USB_CTRL = bUC_HOST_MODE | bUC_INT_BUSY | bUC_DMA_EN;                            // 启动USB主机及DMA,在中断标志未清除前自动暂停
//  UHUB0_CTRL = 0x00;
//  UHUB1_CTRL = 0x00;
    UH_SETUP = bUH_SOF_EN;
    USB_INT_FG = 0xFF;                                                               // 清中断标志
    DisableRootHubPort( );                                                           // 清空
    USB_INT_EN = bUIE_TRANSFER | bUIE_DETECT;
//  IE_USB = 1;                                                                      // 查询方式
}
UINT8X  buf[ 100 ];
/*******************************************************************************
* Function Name  : UDISK_demo
* Description    : U盘操作流程：USB总线复位、U盘连接、获取设备描述符和设置USB地址、可选的获取配置描述符，之后到达此处，由CH559子程序库继续完成后续工作
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8   UDISK_demo( void )
{
    UINT8   i, s;
    printf( "Start UDISK_demo @CH559UFI library\n" );
    CH559DiskStatus = DISK_USB_ADDR;
    for ( i = 0; i != 10; i ++ )
    {
        printf( "Wait DiskReady\n" );
        s = CH559DiskReady( );
        if ( s == ERR_SUCCESS )
        {
            break;
        }
        mDelaymS( 50 );
    }
    if ( CH559DiskStatus >= DISK_MOUNTED )                                            /* U盘准备好 */
    {
        printf( "FileCreate\n" );
        strcpy( mCmdParam.Open.mPathName, "/MY_ADC.TXT" );                            /* 文件名,该文件在根目录下 */
        s = CH559FileCreate( );                                                       /* 新建文件并打开,如果文件已经存在则先删除后再新建 */
        if ( s == ERR_SUCCESS )
        {
            printf( "ByteWrite\n" );
            i = sprintf( buf, "本次ADC数据=%d,时间计数=%u\xd\xa", ADC_FIFO, (TH1<<8)|TL1 );/* ADC数据演示 */
            mCmdParam.ByteWrite.mByteCount = i;                                       /* 指定本次写入的字节数 */
            mCmdParam.ByteWrite.mByteBuffer = buf;                                    /* 指向缓冲区 */
            s = CH559ByteWrite( );                                                    /* 以字节为单位向文件写入数据 */
            if ( s == ERR_SUCCESS )
            {
                /* 默认情况下,如果字节数mCmdParam.ByteWrite.mByteCount不为0那么CH559ByteWrite只负责写入数据而不修改文件长度,
                   如果长时间不写入数据则应该更新文件长度,防止突然断电后前面写入的数据与文件长度不相符,
                   如果需要写完数据后立即修改/更新文件长度,那么可以置字节数mCmdParam.ByteWrite.mByteCount为0后调用CH559ByteWrite强行更新文件长度,
                   如果确定不会突然断电或者后面很快有数据不断写入则不必更新文件长度,可以提高速度并减少U盘损耗(U盘内部的内存寿命有限,不宜频繁改写) */
                /*              mCmdParam.ByteWrite.mByteCount = 0;  如果指定写入0字节,则用于刷新文件的长度
                                CH559ByteWrite( );  以字节为单位向文件写入数据,因为是0字节写入,所以只用于更新文件的长度,当阶段性写入数据后,可以用这种办法更新文件长度 */
                printf( "FileClose with updating size\n" );
                mCmdParam.Close.mUpdateLen = 1;                                        /* 自动计算文件长度,以字节为单位写文件,建议让程序库关闭文件以便自动更新文件长度 */
                s = CH559FileClose( );                                                 /* 关闭文件 */
                if ( s == ERR_SUCCESS )
                {
                    printf( "File Create..Write..Close OK\n" );
                }
                else
                {
                    printf( "ErrorClose=%02X\n", (UINT16)s );
                }
            }
            else
            {
                printf( "FileClose\n" );
                printf( "ErrorWrite=%02X\n", (UINT16)s );
                mCmdParam.Close.mUpdateLen = 0;                                         /* 不自动计算文件长度 */
                CH559FileClose( );                                                      /* 关闭文件 */
            }
        }
        else
        {
            printf( "ErrorCreate=%02X\n", (UINT16)s );
        }
    }
    else
    {
        printf( "Disk not ready=%02X\n", (UINT16)s );
    }
    printf( "Finished UDISK_demo\n" );
    return( s );
}
main( )
{
    UINT8   i, s;
#if !FOR_ROOT_UDISK_ONLY
    UINT8   len, endp;
#endif
//  SAFE_MOD = 0x55;
//  SAFE_MOD = 0xAA;
//  CLOCK_CFG |= bOSC_EN_XT;
    mInitSTDIO( );                                                                       /* 为了让计算机通过串口监控演示过程 */
    printf( "Start @ChipID=%02X\n", (UINT16)CHIP_ID );
    InitUSB_Host( );
	CH559LibInit( );                                                                     /* 初始化CH559程序库以支持U盘文件 */
	FoundNewDev = 0;
    printf( "Wait Device In\n" );
    while ( 1 )
    {
        s = ERR_SUCCESS;
        if ( UIF_DETECT )                                                                // 如果有USB主机检测中断则处理
        {
            UIF_DETECT = 0;                                                              // 清中断标志
            s = AnalyzeRootHub( );                                                       // 分析ROOT-HUB状态
            if ( s == ERR_USB_CONNECT )
            {
                FoundNewDev = 1;
            }
        }
//      if ( s == ERR_USB_CONNECT ) {
//      }
        if ( FoundNewDev || s == ERR_USB_CONNECT )                                        // 有新的USB设备插入
        {
            FoundNewDev = 0;
            mDelaymS( 200 );                                                              // 由于USB设备刚插入尚未稳定,故等待USB设备数百毫秒,消除插拔抖动
            s = InitRootDevice( );                                                        // 初始化USB设备
            if ( s != ERR_SUCCESS )
            {
                return( s );
            }
        }
//      other work for each device
        mDelaymS( 100 );                                                                   // 模拟单片机做其它事
        if ( RI == 0 )
        {
            continue;
        }
        i = getkey( );
        printf( "%c", (UINT8)i );
#if FOR_ROOT_UDISK_ONLY
        if ( i == 'D' && CH559DiskStatus >= DISK_USB_ADDR )
        {
#else
        if ( i == 'D' && ThisUsbDev.DeviceType == USB_DEV_CLASS_STORAGE && ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS )// 模拟主观请求,对某USB设备进行操作
        {
#endif
            printf( "Access USB-disk\n" );
//          SetHostUsbAddr( ThisUsbDev.DeviceAddress );                                    // 设置USB主机当前操作的USB设备地址
            SetUsbSpeed( ThisUsbDev.DeviceSpeed );                                         // 设置当前USB速度
            // 对U盘进行操作,调用CH559UFI或者HostCtrlTransfer,USBHostTransact等
            s = UDISK_demo( );
            if ( s )
            {
                printf( "ErrCode=%02X\n", (UINT16)s );
            }
            SetUsbSpeed( 1 );                                                              // 默认为全速
        }
#if !FOR_ROOT_UDISK_ONLY
        else if ( i == 'M' && ThisUsbDev.DeviceType == DEV_TYPE_MOUSE && ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS )// 模拟主观需求,需要操作鼠标
        {
            printf( "Query Mouse\n" );
//          SetHostUsbAddr( ThisUsbDev.DeviceAddress );                                    // 设置USB主机当前操作的USB设备地址
            SetUsbSpeed( ThisUsbDev.DeviceSpeed );                                         // 设置当前USB速度
//          对鼠标进行操作
//          s = GetDeviceDescr( );                                                         // 获取设备描述符
//          if ( s == USB_INT_SUCCESS ) {
//              for ( s = 0; s < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL; s ++ ) printf( "x%02X ", (UINT16)( TxBuffer[s] ) );
//              printf( "\n" );                                                            // 显示出描述符
//          }
            endp = ThisUsbDev.GpVar;                                                       // 中断端点的地址,位7用于同步标志位
            if ( endp & USB_ENDP_ADDR_MASK )                                               // 端点有效
            {
                s = USBHostTransact( USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0 );  // CH559传输事务,获取数据,NAK不重试
                if ( s == ERR_SUCCESS )
                {
                    endp ^= 0x80;                                                          // 同步标志翻转
                    ThisUsbDev.GpVar = endp;                                               // 保存同步标志位
                    len = USB_RX_LEN;                                                      // 接收到的数据长度
                    if ( len )
                    {
                        printf("Mouse data: ");
                        for ( i = 0; i < len; i ++ )
                        {
                            printf("x%02X ",(UINT16)(RxBuffer[i]) );
                        }
                        printf("\n");
                    }
                }
                else if ( s != ( USB_PID_NAK | ERR_USB_TRANSFER ) )
                {
                    printf("Mouse error %02x\n",(UINT16)s);                                // 可能是断开了
                }
            }
            else
            {
                printf("Mouse no interrupt endpoint\n");
            }
            SetUsbSpeed( 1 );                                                               // 默认为全速
        }
        else
        {
        }
#endif
    }
}
