/********************************** (C) COPYRIGHT *******************************
* File Name          : ROOT_UDISKIF.H
* Author             : WCH
* Version            : V1.1
* Date               : 2015/10/13
* Description        : CH559 Interface                                  				   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#ifndef __UINTERFACE_H__
#define	__UINTERFACE_H__


//#define DISK_BASE_BUF_LEN     512 /* 默认的磁盘数据缓冲区大小为512字节(可以选择为2048甚至4096以支持某些大扇区的U盘),为0则禁止在本文件中定义缓冲区并由应用程序在pDISK_BASE_BUF中指定 */
#define FOR_ROOT_UDISK_ONLY      1// 只用于DP/DM端口的U盘文件操作(使用子程序库CH559UFI/X),不支持HUB下U盘操作

// 各子程序返回状态码
#define ERR_SUCCESS         0x00    // 操作成功
#define ERR_USB_CONNECT     0x15    /* 检测到USB设备连接事件,已经连接 */
#define ERR_USB_DISCON      0x16    /* 检测到USB设备断开事件,已经断开 */
#define ERR_USB_BUF_OVER    0x17    /* USB传输的数据有误或者数据太多缓冲区溢出 */
#define ERR_USB_DISK_ERR    0x1F    /* USB存储器操作失败,在初始化时可能是USB存储器不支持,在读写操作中可能是磁盘损坏或者已经断开 */
#define ERR_USB_TRANSFER    0x20    /* NAK/STALL等更多错误码在0x20~0x2F */
#define ERR_USB_UNSUPPORT   0xFB
#define ERR_USB_UNKNOWN     0xFE
#define WAIT_USB_TOUT_200US     200  // 等待USB中断超时时间200uS@Fsys=12MHz
#define SetUsbSpeed( x )
UINT8X  UsbDevEndp0Size;    /* USB设备的端点0的最大包尺寸 */


//USB设备相关信息表,CH559最多支持2个设备
#define ROOT_DEV_DISCONNECT     0
#define ROOT_DEV_CONNECTED      1
#define ROOT_DEV_FAILED         2
#define ROOT_DEV_SUCCESS        3
#define pSetupReq   ((PXUSB_SETUP_REQ)TxBuffer)
bit     FoundNewDev;
#pragma NOAREGS
void    mDelayuS( UINT16 n );  // 以uS为单位延时
void    mDelaymS( UINT16 n );  // 以mS为单位延时
void    DisableRootHubPort( void );  // 关闭端口,实际上硬件已经自动关闭,此处只是清除一些结构状态
UINT8   AnalyzeRootHub( void );   // 分析端口状态,处理ROOT-HUB端口的设备插拔事件
// 返回ERR_SUCCESS为没有情况,返回ERR_USB_CONNECT为检测到新连接,返回ERR_USB_DISCON为检测到断开
void    SetHostUsbAddr( UINT8 addr );  // 设置USB主机当前操作的USB设备地址
void    ResetRootHubPort( void );  // 检测到设备后,复位总线,为枚举设备准备,设置为默认为全速
UINT8   EnableRootHubPort( void );  // 使能端口,相应的bUH_PORT_EN置1开启端口,设备断开可能导致返回失败
UINT8   WaitUSB_Interrupt( void );  // 等待USB中断
// CH559传输事务,输入目的端点地址/PID令牌,同步标志,以20uS为单位的NAK重试总时间(0则不重试,0xFFFF无限重试),返回0成功,超时/出错重试
UINT8   USBHostTransact( UINT8 endp_pid, UINT8 tog, UINT16 timeout );  // endp_pid: 高4位是token_pid令牌, 低4位是端点地址
UINT8   HostCtrlTransfer( PUINT8X DataBuf, PUINT8I RetLen );  // 执行控制传输,8字节请求码在pSetupReq中,DataBuf为可选的收发缓冲区
// 如果需要接收和发送数据,那么DataBuf需指向有效缓冲区用于存放后续数据,实际成功收发的总长度返回保存在ReqLen指向的字节变量中
void    CopySetupReqPkg( PUINT8C pReqPkt );  // 复制控制传输的请求包
UINT8   CtrlGetDeviceDescr( void );  // 获取设备描述符,返回在TxBuffer中
UINT8   CtrlGetConfigDescr( void );  // 获取配置描述符,返回在TxBuffer中
UINT8   CtrlSetUsbAddress( UINT8 addr );  // 设置USB设备地址
UINT8   CtrlSetUsbConfig( UINT8 cfg );  // 设置USB设备配置
UINT8   CtrlClearEndpStall( UINT8 endp );  // 清除端点STALL
UINT8   InitRootDevice( void );  // 初始化USB设备
/* 为printf和getkey输入输出初始化串口 */
void    mInitSTDIO( void );
void    InitUSB_Host( void );  // 初始化USB主机
#endif