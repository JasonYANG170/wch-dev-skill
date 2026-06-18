/********************************** (C) COPYRIGHT *******************************
* File Name          : USBH_AOA.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/01
* Description        :
 USB host example for CH559, start USB device under DP/DM and HP/HM port
 支持安卓手机的配件模式（AOA）与通讯，配置手机APP实现双向数据收发，支持普通设备的枚举。
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include <CH559.H>
#include <stdio.h>
#include <string.h>

#pragma  NOAREGS
// 各子程序返回状态码
#define ERR_SUCCESS         0x00    // 操作成功
#define ERR_USB_CONNECT     0x15    /* 检测到USB设备连接事件,已经连接 */
#define ERR_USB_DISCON      0x16    /* 检测到USB设备断开事件,已经断开 */
#define ERR_USB_BUF_OVER    0x17    /* USB传输的数据有误或者数据太多缓冲区溢出 */
#define ERR_USB_DISK_ERR    0x1F    /* USB存储器操作失败,在初始化时可能是USB存储器不支持,在读写操作中可能是磁盘损坏或者已经断开 */
#define ERR_USB_TRANSFER    0x20    /* NAK/STALL等更多错误码在0x20~0x2F */
#define ERR_USB_UNSUPPORT   0xFB    /*不支持的USB设备*/
#define ERR_USB_UNKNOWN     0xFE    /*设备操作出错*/
#define ERR_AOA_PROTOCOL    0x41    /*协议版本出错 */ 

/*获取设备描述符*/
UINT8C  SetupGetDevDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_DEVICE, 0x00, 0x00, sizeof( USB_DEV_DESCR ), 0x00 };
/*获取配置描述符*/
UINT8C  SetupGetCfgDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_CONFIG, 0x00, 0x00, 0x04, 0x00 };
/*设置USB地址*/
UINT8C  SetupSetUsbAddr[] = { USB_REQ_TYP_OUT, USB_SET_ADDRESS, USB_DEVICE_ADDR, 0x00, 0x00, 0x00, 0x00, 0x00 };
/*设置USB配置*/
UINT8C  SetupSetUsbConfig[] = { USB_REQ_TYP_OUT, USB_SET_CONFIGURATION, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
/*设置USB IDLE*/
UINT8C  SetupSetUsbIDLE[] = { 0x21, HID_SET_IDLE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
/*设置USB接口配置*/
UINT8C  SetupSetUsbInterface[] = { USB_REQ_RECIP_INTERF, USB_SET_INTERFACE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
/*清除端点STALL*/
UINT8C  SetupClrEndpStall[] = { USB_REQ_TYP_OUT | USB_REQ_RECIP_ENDP, USB_CLEAR_FEATURE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
/*获取HUB描述符*/
UINT8C  SetupGetHubDescr[] = { HUB_GET_HUB_DESCRIPTOR, HUB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_HUB, 0x00, 0x00, sizeof( USB_HUB_DESCR ), 0x00 };
/*获取HID设备报表描述符*/
UINT8C  SetupGetHIDDevReport[] = { 0x81, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_REPORT, 0x00, 0x00, 0x41, 0x00 };
//AOA获取协议版本
UINT8C  GetProtocol[] = { 0xc0,0x33,0x00,0x00,0x00,0x00,0x02,0x00 };
//启动配件模式
UINT8C  TouchAOAMode[] = { 0x40,0x35,0x00,0x00,0x00,0x00,0x00,0x00 };
/* AOA相关数组定义 */
UINT8C  Sendlen[]= {0,4,16,35,39,53,67};
//字符串ID,与手机APP相关的字符串信息
UINT8C  StringID[] = {'W','C','H',0x00,                                                                                //manufacturer name
                      'W','C','H','U','A','R','T','D','e','m','o',0x00,                                   //model name
                      0x57,0x43,0x48,0x20,0x41,0x63,0x63,0x65,0x73,0x73,0x6f,0x72,0x79,0x20,0x54,0x65,0x73,0x74,0x00,     //description
                      '1','.','0',0x00 ,                                                                       //version
                      0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x63,0x68,0x2e,0x63,0x6e,0,//URI
                      0x57,0x43,0x48,0x41,0x63,0x63,0x65,0x73,0x73,0x6f,0x72,0x79,0x31,0x00                               //serial number
                     };  
//应用索引字符串命令
UINT8C  SetStringID[]= {0x40,0x34,0x00,0x00,0x00,0x00,0x04,0x00,
                        0x40,0x34,0x00,0x00,0x01,0x00,12,0x00,
                        0x40,0x34,0x00,0x00,0x02,0x00,19,0x00,
                        0x40,0x34,0x00,0x00,0x03,0x00,4,0x00,
                        0x40,0x34,0x00,0x00,0x04,0x00,0x0E,0x00,
                        0x40,0x34,0x00,0x00,0x05,0x00,0x0E,0x00
                       };

UINT8X  UsbDevEndp0Size;              /* USB设备的端点0的最大包尺寸 */
/*USB设备相关信息表,CH559最多支持2个设备*/
#define ROOT_DEV_DISCONNECT     0
#define ROOT_DEV_CONNECTED      1
#define ROOT_DEV_FAILED         2
#define ROOT_DEV_SUCCESS        3
#define DEV_TYPE_KEYBOARD   ( USB_DEV_CLASS_HID | 0x20 )
#define DEV_TYPE_MOUSE      ( USB_DEV_CLASS_HID | 0x30 )
#define DEF_AOA_DEVICE          0xF0

struct _RootHubDev
{
    UINT8   DeviceStatus;           // 设备状态,0-无设备,1-有设备但尚未初始化,2-有设备但初始化枚举失败,3-有设备且初始化枚举成功
    UINT8   DeviceAddress;          // 设备被分配的USB地址
    UINT8   DeviceSpeed;            // 0为低速,非0为全速
    UINT8   DeviceType;             // 设备类型
	UINT16  DeviceVID;              // 设备VID
	UINT16  DevicePID;              // 设备PID
    UINT8   GpVar[4];               // 通用变量
} xdata RootHubDev[2];
#define HUB_MAX_PORTS   4
/*
约定: USB设备地址分配规则(参考USB_DEVICE_ADDR)
地址值  设备位置
0x02    内置Root-HUB0下的USB设备或外部HUB
0x03    内置Root-HUB1下的USB设备或外部HUB
0x1x    内置Root-HUB0下的外部HUB的端口x下的USB设备,x为1~n
0x2x    内置Root-HUB1下的外部HUB的端口x下的USB设备,x为1~n
*/
struct _DevOnHubPort
{
    UINT8   DeviceStatus;           // 设备状态,0-无设备,1-有设备但尚未初始化,2-有设备但初始化枚举失败,3-有设备且初始化枚举成功
    UINT8   DeviceAddress;          // 设备被分配的USB地址
    UINT8   DeviceSpeed;            // 0为低速,非0为全速
    UINT8   DeviceType;             // 设备类型
//.....    struct  _Endp_Attr   Endp_Attr[4];   //端点的属性,最多支持4个端点
    UINT8   GpVar;                  // 通用变量
} xdata DevOnHubPort[2][HUB_MAX_PORTS];  // 假定:不超过2个外部HUB,每个外部HUB不超过HUB_MAX_PORTS个端口(多了不管)

UINT8X  RxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0000 ;  // IN, must even address
UINT8X  TxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0040 ;  // OUT, must even address
#define pSetupReq   ((PXUSB_SETUP_REQ)TxBuffer)
UINT8X  COM_BUF[200];                              //开辟共享缓冲区，存放超过64字节的描述符
bit     RootHubId;                 // 当前正在操作的root-hub端口号:0=HUB0,1=HUB1
bit     FoundNewDev;
#pragma NOAREGS
#define WAIT_USB_TOUT_200US     200  // 等待USB中断超时时间200uS@Fsys=12MHz
void    mDelayuS( UINT16 n );                          // 以uS为单位延时
void    mDelaymS( UINT16 n );                          // 以mS为单位延时
void    DisableRootHubPort( UINT8 RootHubIndex );      // 关闭指定的ROOT-HUB端口,实际上硬件已经自动关闭,此处只是清除一些结构状态
UINT8   AnalyzeRootHub( void );                        // 分析ROOT-HUB状态,处理ROOT-HUB端口的设备插拔事件
// 返回ERR_SUCCESS为没有情况,返回ERR_USB_CONNECT为检测到新连接,返回ERR_USB_DISCON为检测到断开
void    SetHostUsbAddr( UINT8 addr );                  // 设置USB主机当前操作的USB设备地址
void    SetUsbSpeed( UINT8 FullSpeed );                // 设置当前USB速度
UINT8   ResetRootHubPort( UINT8 RootHubIndex );        // 检测到设备后,复位相应端口的总线,为枚举设备准备,设置为默认为全速
UINT8   EnableRootHubPort( UINT8 RootHubIndex );       // 使能ROOT-HUB端口,相应的bUH_PORT_EN置1开启端口,设备断开可能导致返回失败
void    SelectHubPort( UINT8 RootHubIndex, UINT8 HubPortIndex );// HubPortIndex=0选择操作指定的ROOT-HUB端口,否则选择操作指定的ROOT-HUB端口的外部HUB的指定端口
UINT8   WaitUSB_Interrupt( void );                     // 等待USB中断
// CH559传输事务,输入目的端点地址/PID令牌,同步标志,以20uS为单位的NAK重试总时间(0则不重试,0xFFFF无限重试),返回0成功,超时/出错重试
UINT8   USBHostTransact( UINT8 endp_pid, UINT8 tog, UINT16 timeout );  // endp_pid: 高4位是token_pid令牌, 低4位是端点地址
UINT8   HostCtrlTransfer( PUINT8 pBuf, PUINT16 pLen );  // 执行控制传输,8字节请求码在pSetupReq中,DataBuf为可选的收发缓冲区
// 如果需要接收和发送数据,那么DataBuf需指向有效缓冲区用于存放后续数据,实际成功收发的总长度返回保存在ReqLen指向的字节变量中
void    CopySetupReqPkg( PUINT8C pReqPkt );            // 复制控制传输的请求包
UINT8   CtrlGetDeviceDescr( void );                    // 获取设备描述符,返回在TxBuffer中
UINT8   CtrlGetConfigDescr( void );                    // 获取配置描述符,返回在TxBuffer中
UINT8   CtrlSetUsbAddress( UINT8 addr );               // 设置USB设备地址
UINT8   CtrlSetUsbConfig( UINT8 cfg );                 // 设置USB设备配置
UINT8   CtrlSetUsbIntercace( UINT8 cfg );              // 设置USB设备接口
UINT8   CtrlClearEndpStall( UINT8 endp );              // 清除端点STALL
UINT8   CtrlGetHubDescr( void );                       // 获取HUB描述符,返回在TxBuffer中
UINT8   HubGetPortStatus( UINT8 HubPortIndex );        // 查询HUB端口状态,返回在TxBuffer中
UINT8   HubSetPortFeature( UINT8 HubPortIndex, UINT8 FeatureSelt );  // 设置HUB端口特性
UINT8   HubClearPortFeature( UINT8 HubPortIndex, UINT8 FeatureSelt );  // 清除HUB端口特性
UINT8   AnalyzeHidIntEndp( PUINT8X buf );              // 从描述符中分析出HID中断端点的地址
UINT8   AnalyzeBulkEndp( PUINT8X buf,UINT8 RootHubIndex ) ;//分析出RootHUB下的批量端点
UINT8   TouchStartAOA(void);                           // 尝试启动AOA模式
UINT8   InitRootDevice( UINT8 RootHubIndex );          // 初始化指定ROOT-HUB端口的USB设备
// 输入: 内置HUB端口号0/1
UINT8   EnumAllRootDevice( void );                     // 枚举所有ROOT-HUB端口的USB设备
UINT8   InitDevOnHub( UINT8 RootHubIndex, UINT8 HubPortIndex );  // 初始化枚举外部HUB后的二级USB设备
UINT8   EnumHubPort( UINT8 RootHubIndex );             // 枚举指定ROOT-HUB端口上的外部HUB集线器的各个端口,检查各端口有无连接或移除事件并初始化二级USB设备
UINT8   EnumAllHubPort( void );                        // 枚举所有ROOT-HUB端口下外部HUB后的二级USB设备
UINT16  SearchTypeDevice( UINT8 type );                // 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号,输出端口号为0xFFFF则未搜索到
// 输出高8位为ROOT-HUB端口号,低8位为外部HUB的端口号,低8位为0则设备直接在ROOT-HUB端口上
void    mInitSTDIO( void );                            // 为printf和getkey输入输出初始化串口
void    InitUSB_Host( void );                          // 初始化USB主机
//#define   FREQ_SYS    12000000                       // 系统主频12MHz
/*******************************************************************************
* Function Name  : mDelayus(UNIT16 n)
* Description    : us延时函数
* Input          : UNIT16 n
* Output         : None
* Return         : None
*******************************************************************************/
void    mDelayuS( UINT16 n )                           // 以uS为单位延时
{
    while ( n )                                        // total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
    {
        ++ SAFE_MOD;                                   // 2 Fsys cycles, for higher Fsys, add operation here
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
void    mDelaymS( UINT16 n )                              // 以mS为单位延时
{
    while ( n )
    {
        mDelayuS( 1000 );
        -- n;
    }
}
/*******************************************************************************
* Function Name  : DisableRootHubPort(UINT8 RootHubIndex)
* Description    : 关闭HUB端口
* Input          : UINT8 RootHubIndex 指定ROOT_HUB口
* Output         : None
* Return         : None
*******************************************************************************/
void    DisableRootHubPort( UINT8 RootHubIndex )          // 关闭指定的ROOT-HUB端口,实际上硬件已经自动关闭,此处只是清除一些结构状态
{
    RootHubDev[ RootHubIndex ].DeviceStatus = ROOT_DEV_DISCONNECT;
    RootHubDev[ RootHubIndex ].DeviceAddress = 0x00;
    if ( RootHubIndex == 1 )
    {
        UHUB1_CTRL = 0x00;                                // 清除有关HUB1的控制数据,实际上不需要清除
    }
    else
    {
        UHUB0_CTRL = 0x00;                                // 清除有关HUB0的控制数据,实际上不需要清除
    }
}
/*******************************************************************************
* Function Name  : AnalyzeRootHub(void)
* Description    : 分析ROOT-HUB状态,处理ROOT-HUB端口的设备插拔事件
                   如果设备拔出,函数中调用DisableRootHubPort()函数,将端口关闭,插入事件,置相应端口的状态位
* Input          : None
* Output         : None
* Return         : 返回ERR_SUCCESS为没有情况,返回ERR_USB_CONNECT为检测到新连接,返回ERR_USB_DISCON为检测到断开
*******************************************************************************/
UINT8   AnalyzeRootHub( void )
{
    UINT8   s;
    s = ERR_SUCCESS;
    if ( USB_HUB_ST & bUHS_H0_ATTACH )                         // 设备存在，HUB0
    {
        if ( RootHubDev[0].DeviceStatus == ROOT_DEV_DISCONNECT // 检测到有设备插入
                || ( UHUB0_CTRL & bUH_PORT_EN ) == 0x00 )      // 检测到有设备插入,但尚未允许,说明是刚插入
        {
            DisableRootHubPort( 0 );                           // 关闭端口
//          RootHubDev[0].DeviceSpeed = USB_HUB_ST & bUHS_DM_LEVEL ? 0 : 1;
            RootHubDev[0].DeviceStatus = ROOT_DEV_CONNECTED;   //置连接标志
            printf( "HUB 0 dev in\n" );
            s = ERR_USB_CONNECT;
        }
    }
    else if ( RootHubDev[0].DeviceStatus >= ROOT_DEV_CONNECTED )//检测到设备拔出
    {
        DisableRootHubPort( 0 );                               // 关闭端口
        printf( "HUB 0 dev out\n" );
        //UH_SETUP &= ~bUH_SOF_EN;
        if ( s == ERR_SUCCESS )
        {
            s = ERR_USB_DISCON;                                //设置设备断开标志
        }
    }
    if ( USB_HUB_ST & bUHS_H1_ATTACH )                         // 设备存在，HUB1
    {
        if ( RootHubDev[1].DeviceStatus == ROOT_DEV_DISCONNECT // 检测到有设备插入
                || ( UHUB1_CTRL & bUH_PORT_EN ) == 0x00 )      // 检测到有设备插入,但尚未允许,说明是刚插入
        {
            DisableRootHubPort( 1 );                           // 关闭端口
//          RootHubDev[1].DeviceSpeed = USB_HUB_ST & bUHS_HM_LEVEL ? 0 : 1;
            RootHubDev[1].DeviceStatus = ROOT_DEV_CONNECTED;   //置连接标志
            printf( "HUB 1 dev in\n" );
            s = ERR_USB_CONNECT;
        }
    }
    else if ( RootHubDev[1].DeviceStatus >= ROOT_DEV_CONNECTED )//检测到设备拔出
    {
        DisableRootHubPort( 1 );                                // 关闭端口
        //UH_SETUP &= ~bUH_SOF_EN;
        printf( "HUB 1 dev out\n" );
        if ( s == ERR_SUCCESS )
        {
            s = ERR_USB_DISCON;
        }
    }
//  UIF_DETECT = 0;                                             // 清连接中断标志
    return( s );
}
/*******************************************************************************
* Function Name  : SetHostUsbAddr
* Description    : 设置USB主机当前操作的USB设备地址
* Input          : UINT8 addr
* Output         : None
* Return         : None
*******************************************************************************/
void    SetHostUsbAddr( UINT8 addr )
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
void    SetUsbSpeed( UINT8 FullSpeed )
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
/*******************************************************************************
* Function Name  : ResetRootHubPort( UINT8 RootHubIndex )
* Description    : 检测到设备后,复位总线,为枚举设备准备,设置为默认为全速
* Input          : UINT8 RootHubIndex 指定端口
* Output         : None
* Return         : None
*******************************************************************************/
UINT8   ResetRootHubPort( UINT8 RootHubIndex )
{
    UINT16 n = 30000;
	UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;                        //USB设备的端点0的最大包尺寸
    SetHostUsbAddr( 0x00 );
	RootHubDev[ RootHubIndex ].DeviceAddress = 0x00;
	if ( RootHubIndex == 1 )
	{ 
		UHUB1_CTRL = 0x00;                                      // 清除有关HUB1的控制数据,实际上不需要清除
	}
	else
	{
		UHUB0_CTRL = 0x00;                                       // 清除有关HUB0的控制数据,实际上不需要清除
	}
			
    SetUsbSpeed( 1 );                                            // 默认为全速
    if ( RootHubIndex == 1 )
    {
        UHUB1_CTRL = UHUB1_CTRL & ~ bUH_LOW_SPEED | bUH_BUS_RESET;// 默认为全速,开始复位
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
			if( USB_HUB_ST & ( bUHS_HP_PIN | bUHS_HM_PIN ) )
			{
				UHUB1_CTRL = UHUB1_CTRL & ~bUH_BUS_RESET;  						/* 结束复位 */
				return( 0x01 );
			}
			--n;
		}
        UHUB1_CTRL = UHUB1_CTRL & ~ bUH_BUS_RESET;               // 结束复位
    }
    else
    {
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
    }
    mDelayuS( 200 );
    UIF_DETECT = 0;											     // 清连接中断标志
	return ( 0x00 );
}
/*******************************************************************************
* Function Name  : EnableRootHubPort( UINT8 RootHubIndex )
* Description    : 使能ROOT-HUB端口,相应的bUH_PORT_EN置1开启端口,设备断开可能导致返回失败
* Input          : UINT8 RootHubIndex 指定端口
* Output         : None
* Return         : 返回ERR_SUCCESS为检测到新连接,返回ERR_USB_DISCON为无连接
*******************************************************************************/
UINT8   EnableRootHubPort( UINT8 RootHubIndex )
{
    if ( RootHubDev[ RootHubIndex ].DeviceStatus < ROOT_DEV_CONNECTED )
    {
        RootHubDev[ RootHubIndex ].DeviceStatus = ROOT_DEV_CONNECTED;
    }
    if ( RootHubIndex == 1 )
    {
        if ( USB_HUB_ST & bUHS_H1_ATTACH )                       // HUB1有设备
        {
            if ( ( UHUB1_CTRL & bUH_PORT_EN ) == 0x00 )          // 尚未使能
            {
                RootHubDev[1].DeviceSpeed = USB_HUB_ST & bUHS_HM_LEVEL ? 0 : 1;
                if ( RootHubDev[1].DeviceSpeed == 0 )
                {
                    UHUB1_CTRL |= bUH_LOW_SPEED;                 // 低速
                }
            }
            UHUB1_CTRL |= bUH_PORT_EN;                           //使能HUB1端口
            return( ERR_SUCCESS );
        }
    }
    else
    {
        if ( USB_HUB_ST & bUHS_H0_ATTACH )                      // HUB0有设备
        {
            if ( ( UHUB0_CTRL & bUH_PORT_EN ) == 0x00 )         // 尚未使能
            {
                RootHubDev[0].DeviceSpeed = USB_HUB_ST & bUHS_DM_LEVEL ? 0 : 1;
                if ( RootHubDev[0].DeviceSpeed == 0 )
                {
                    UHUB0_CTRL |= bUH_LOW_SPEED;                // 低速
                }
            }
            UHUB0_CTRL |= bUH_PORT_EN;                          //使能HUB0端口
            return( ERR_SUCCESS );
        }
    }
    return( ERR_USB_DISCON );
}
/*******************************************************************************
* Function Name  : SelectHubPort( UINT8 RootHubIndex, UINT8 HubPortIndex )
* Description    : 选定需要操作的HUB口
* Input          : UINT8 RootHubIndex HubPortIndex=0选择操作指定的ROOT-HUB端口,否则选择操作指定的ROOT-HUB端口的外部HUB的指定端口
                   UINT8 HubPortIndex 选择操作指定的ROOT-HUB端口的外部HUB的指定端口
* Output         : None
* Return         : None
*******************************************************************************/
void    SelectHubPort( UINT8 RootHubIndex, UINT8 HubPortIndex )
{
    if ( HubPortIndex )                                          // 选择操作指定的ROOT-HUB端口的外部HUB的指定端口
    {
        SetHostUsbAddr( DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceAddress );// 设置USB主机当前操作的USB设备地址
        SetUsbSpeed( DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceSpeed );// 设置当前USB速度
        if ( DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceSpeed == 0 )// 通过外部HUB与低速USB设备通讯需要前置ID
        {
            UH_SETUP |= bUH_PRE_PID_EN;                          // 启用PRE PID
            mDelayuS(100);
        }
    }
    else                                                         // 选择操作指定的ROOT-HUB端口
    {
        SetHostUsbAddr( RootHubDev[RootHubIndex].DeviceAddress );// 设置USB主机当前操作的USB设备地址
        SetUsbSpeed( RootHubDev[RootHubIndex].DeviceSpeed );     // 设置当前USB速度
    }
    RootHubId = RootHubIndex ? 1 : 0;
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
    UINT8   TransRetry;
// #define TransRetry  UEP0_T_LEN                                    // 节约内存
    UINT8  r;
    UINT16  i;
    UH_RX_CTRL = tog;
    UH_TX_CTRL =tog ;
    TransRetry = 0;
    do
    {
//      LED_TMP = 0;
        UH_EP_PID = endp_pid;                                    // 指定令牌PID和目的端点号
        UIF_TRANSFER = 0;                                        // 允许传输
//      s = WaitUSB_Interrupt( );
        for ( i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i -- )
        {
            ;
        }
        UH_EP_PID = 0x00;                                        // 停止USB传输
//      LED_TMP = 1;
//      if ( s != ERR_SUCCESS ) return( s );                     // 中断超时,可能是硬件异常
        if ( UIF_TRANSFER == 0 )
        {
            return( ERR_USB_UNKNOWN );
        }
//         if ( UIF_DETECT )                                        // USB设备插拔事件
//         {
// //          mDelayuS( 200 );                                     // 等待传输完成
// //             UIF_DETECT = 0;                                      // 清中断标志
//             s = AnalyzeRootHub( );                               // 分析ROOT-HUB状态
//             if ( s == ERR_USB_CONNECT )
//             {
//                 FoundNewDev = 1;
//             }
//             if ( (RootHubDev[RootHubId].DeviceStatus == ROOT_DEV_DISCONNECT)||(CH559DiskStatus == DISK_DISCONNECT) )
//             {
//                 return( ERR_USB_DISCON );                        // USB设备断开事件
//             }
//             if ( (RootHubDev[RootHubId].DeviceStatus == ROOT_DEV_CONNECTED)||(CH559DiskStatus == DISK_CONNECT) )
//             {
//                 return( ERR_USB_CONNECT );                       // USB设备连接事件
//             }
// //          if ( ( USB_HUB_ST & bUHS_H0_ATTACH ) == 0x00 ) return( ERR_USB_DISCON );// USB设备断开事件
//             mDelayuS( 200 );                                     // 等待传输完成
//         }
        if ( UIF_TRANSFER )                                      // 传输完成
        {
            if ( U_TOG_OK )
            {
                return( ERR_SUCCESS );
            }
            r = USB_INT_ST & MASK_UIS_H_RES;                     // USB设备应答状态
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
                    if ( U_TOG_OK )
                    {
                        return( ERR_SUCCESS );
                    }
                    if ( r == USB_PID_ACK )
                    {
                        return( ERR_SUCCESS );
                    }
                    if ( r == USB_PID_STALL || r == USB_PID_NAK )
                    {
                        return( r | ERR_USB_TRANSFER );
                    }
                    if ( r )
                    {
                        return( r | ERR_USB_TRANSFER );          // 不是超时/出错,意外应答
                    }
                    break;                                       // 超时重试
                case USB_PID_IN:
                    if ( U_TOG_OK )
                    {
                        return( ERR_SUCCESS );
                    }
                    if ( tog ? r == USB_PID_DATA1 : r == USB_PID_DATA0 )
                    {
                        return( ERR_SUCCESS );
                    }
                    if ( r == USB_PID_STALL || r == USB_PID_NAK )
                    {
                        return( r | ERR_USB_TRANSFER );
                    }
                    if ( r == USB_PID_DATA0 && r == USB_PID_DATA1 )// 不同步则需丢弃后重试
                    {
                    }                                            // 不同步重试
                    else if ( r )
                    {
                        return( r | ERR_USB_TRANSFER );          // 不是超时/出错,意外应答
                    }
                    break;                                       // 超时重试
                default:
                    return( ERR_USB_UNKNOWN );                   // 不可能的情况
                    break;
                }
        }
        else                                                     // 其它中断,不应该发生的情况
        {
            USB_INT_FG = 0xFF;                                   //清中断标志
        }
        mDelayuS( 15 );
    }
    while ( ++ TransRetry < 3 );
    return( ERR_USB_TRANSFER );                                  // 应答超时
}
/*******************************************************************************
* Function Name  : HostCtrlTransfer
* Description    : 执行控制传输,8字节请求码在pSetupReq中,DataBuf为可选的收发缓冲区
* Input          : PUINT8X DataBuf 如果需要接收和发送数据,那么DataBuf需指向有效缓冲区用于存放后续数据
                   PUINT8 RetLen  实际成功收发的总长度保存在RetLen指向的字节变量中
* Output         : None
* Return         : ERR_USB_BUF_OVER IN状态阶段出错
                   ERR_SUCCESS     数据交换成功
                   其他错误状态
*******************************************************************************/
UINT8  HostCtrlTransfer( PUINT8 pBuf, PUINT16 pLen )
{
    UINT8  s;
    UINT16 RemLen, RxLen, RxCnt, TxCnt;

    mDelayuS( 200 );
    if ( pLen )
    {
        *pLen = 0;                                                // 实际成功收发的总长度
    }
    UH_TX_LEN = sizeof( USB_SETUP_REQ );
    s = USBHostTransact( USB_PID_SETUP << 4 | 0x00, 0x00, 200000/20 );// SETUP阶段,200mS超时
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    UH_RX_CTRL = UH_TX_CTRL = bUH_R_TOG | bUH_R_AUTO_TOG | bUH_T_TOG | bUH_T_AUTO_TOG;// 默认DATA1
    UH_TX_LEN = 0x01;                                            // 默认无数据故状态阶段为IN
    RemLen = (pSetupReq -> wLengthH << 8)|( pSetupReq -> wLengthL);
    if ( RemLen && pBuf )                                        // 需要收发数据
    {
        if ( pSetupReq -> bRequestType & USB_REQ_TYP_IN )        // 收
        {
            while ( RemLen )
            {
                mDelayuS( 200 );
                s = USBHostTransact( USB_PID_IN << 4 | 0x00, UH_RX_CTRL, 200000/20 );// IN数据
                if ( s != ERR_SUCCESS )
                {
                    return( s );
                }
                RxLen = USB_RX_LEN < RemLen ? USB_RX_LEN : RemLen;
                RemLen -= RxLen;
                if ( pLen )
                {
                    *pLen += RxLen;                             // 实际成功收发的总长度
                }

                for ( RxCnt = 0; RxCnt != RxLen; RxCnt ++ )
                {
                    *pBuf = RxBuffer[ RxCnt ];
                    pBuf ++;
                }
                if ( USB_RX_LEN == 0 || ( USB_RX_LEN & ( UsbDevEndp0Size - 1 ) ) )
                {
                    break;                                      // 短包
                }
            }
            UH_TX_LEN = 0x00;                                   // 状态阶段为OUT
        }
        else                                                    // 发
        {
            while ( RemLen )
            {
                mDelayuS( 200 );
                UH_TX_LEN = RemLen >= UsbDevEndp0Size ? UsbDevEndp0Size : RemLen;

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
                    *pLen += UH_TX_LEN;                        // 实际成功收发的总长度
                }
            }
//          UH_TX_LEN = 0x01;                                  // 状态阶段为IN
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
        return( ERR_SUCCESS );                                  // 状态OUT
    }
    if ( USB_RX_LEN == 0 )
    {
        return( ERR_SUCCESS );                                  // 状态IN,检查IN状态返回数据长度
    }
    return( ERR_USB_BUF_OVER );                                 // IN状态阶段错误
}
/*******************************************************************************
* Function Name  : CopySetupReqPkg
* Description    : 复制控制传输的请求包
* Input          : PUINT8C pReqPkt 控制请求包地址
* Output         : None
* Return         : None
*******************************************************************************/
void    CopySetupReqPkg( PUINT8C pReqPkt )                      // 复制控制传输的请求包
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
    s = HostCtrlTransfer( COM_BUF, &len );                    // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    UsbDevEndp0Size = ( (PXUSB_DEV_DESCR)COM_BUF ) -> bMaxPacketSize0;// 端点0最大包长度,这是简化处理,正常应该先获取前8字节后立即更新UsbDevEndp0Size再继续
    if ( len < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL )
    {
        return( ERR_USB_BUF_OVER );                            // 描述符长度错误
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
    s = HostCtrlTransfer( COM_BUF, &len );                     // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < ( (PUSB_SETUP_REQ)SetupGetCfgDescr ) -> wLengthL )
    {
        return( ERR_USB_BUF_OVER );                             // 返回长度错误
    }
    len = ((UINT16)( (PXUSB_CFG_DESCR)COM_BUF ) -> wTotalLengthH<<8)|( (PXUSB_CFG_DESCR)COM_BUF ) -> wTotalLengthL;
    CopySetupReqPkg( SetupGetCfgDescr );
	pSetupReq -> wLengthH = (UINT8)(len>>8);
    pSetupReq -> wLengthL = (UINT8)len;                                // 完整配置描述符的总长度
    s = HostCtrlTransfer( COM_BUF, &len );                      // 执行控制传输（此时可能超64字节，存放在COM_BUF中）
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < ( (PUSB_SETUP_REQ)SetupGetCfgDescr ) -> wLengthL || len < ( (PXUSB_CFG_DESCR)COM_BUF ) -> wTotalLengthL )
    {
        return( ERR_USB_BUF_OVER );                             // 描述符长度错误
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
    pSetupReq -> wValueL = addr;                                 // USB设备地址
    s = HostCtrlTransfer( NULL, NULL );                          // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    SetHostUsbAddr( addr );                                      // 设置USB主机当前操作的USB设备地址
    mDelaymS( 10 );                                              // 等待USB设备完成操作
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
    pSetupReq -> wValueL = cfg;                                 // USB设备配置
    return( HostCtrlTransfer( NULL, NULL ) );                   // 执行控制传输
}
/*******************************************************************************
* Function Name  : CtrlSetUsbIDLE
* Description    : 设置USB设备配置
* Input          : UINT8 cfg       配置值
* Output         : None
* Return         : ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8   CtrlSetUsbIDLE( UINT8 idle )
{
    CopySetupReqPkg( SetupSetUsbIDLE );
    pSetupReq -> wIndexL = idle;                                 // USB设备配置
    return( HostCtrlTransfer( NULL, NULL ) );                   // 执行控制传输
}
/*******************************************************************************
* Function Name  : CtrlSetUsbIntercace
* Description    : 设置USB设备接口
* Input          : UINT8 cfg       配置值
* Output         : None
* Return         : ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8   CtrlSetUsbIntercace( UINT8 cfg )
{
    CopySetupReqPkg( SetupSetUsbInterface );
    pSetupReq -> wValueL = cfg;                                 // USB设备配置
    return( HostCtrlTransfer( NULL, NULL ) );                   // 执行控制传输
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
    CopySetupReqPkg( SetupClrEndpStall );                       // 清除端点的错误
    pSetupReq -> wIndexL = endp;                                // 端点地址
    return( HostCtrlTransfer( NULL, NULL ) );                   // 执行控制传输
}
/*******************************************************************************
* Function Name  : CtrlGetHIDDeviceReport
* Description    : 获取HID设备报表描述符,返回在TxBuffer中
* Input          : None
* Output         : None
* Return         : ERR_SUCCESS 成功
                   其他        错误
*******************************************************************************/
UINT8  CtrlGetHIDDeviceReport( void )
{
    UINT8   s;
    UINT16   len;
    UINT8 tmp[]= {0x21,0x0a,0x00,0x00,0x00,0x00,0x00,0x00};
    for ( s = 0; s != sizeof( tmp ); s ++ )
    {
        ((PUINT8X)pSetupReq)[ s ] = tmp[s];
    }
    s = HostCtrlTransfer( COM_BUF, &len );                     // 执行控制传输
//    if ( s != ERR_SUCCESS )
//    {
//        return( s );
//    }
    CopySetupReqPkg( SetupGetHIDDevReport );
    s = HostCtrlTransfer( COM_BUF, &len );                     // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL )
    {
        return( ERR_USB_BUF_OVER );                             // 描述符长度错误
    }
    return( ERR_SUCCESS );
}
/*******************************************************************************
* Function Name  : CtrlGetHubDescr
* Description    : 获取HUB描述符,返回在TxBuffer中
* Input          : None
* Output         : None
* Return         : ERR_SUCCESS 成功
                   ERR_USB_BUF_OVER 长度错误
*******************************************************************************/
UINT8   CtrlGetHubDescr( void )
{
    UINT8   s;
    UINT16  len;
    CopySetupReqPkg( SetupGetHubDescr );
    s = HostCtrlTransfer( TxBuffer, &len );                      // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < ( (PUSB_SETUP_REQ)SetupGetHubDescr ) -> wLengthL )
    {
        return( ERR_USB_BUF_OVER );                             // 描述符长度错误
    }
//  if ( len < 4 ) return( ERR_USB_BUF_OVER );                  // 描述符长度错误
    return( ERR_SUCCESS );
}
/*******************************************************************************
* Function Name  : HubGetPortStatus
* Description    : 查询HUB端口状态,返回在TxBuffer中
* Input          : UINT8 HubPortIndex
* Output         : None
* Return         : ERR_SUCCESS 成功
                   ERR_USB_BUF_OVER 长度错误
*******************************************************************************/
UINT8   HubGetPortStatus( UINT8 HubPortIndex )
{
    UINT8   s;
    UINT16  len;
    pSetupReq -> bRequestType = HUB_GET_PORT_STATUS;
    pSetupReq -> bRequest = HUB_GET_STATUS;
    pSetupReq -> wValueL = 0x00;
    pSetupReq -> wValueH = 0x00;
    pSetupReq -> wIndexL = HubPortIndex;
    pSetupReq -> wIndexH = 0x00;
    pSetupReq -> wLengthL = 0x04;
    pSetupReq -> wLengthH = 0x00;
    s = HostCtrlTransfer( TxBuffer, &len );                     // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < 4 )
    {
        return( ERR_USB_BUF_OVER );                             // 描述符长度错误
    }
    return( ERR_SUCCESS );
}
/*******************************************************************************
* Function Name  : HubSetPortFeature
* Description    : 设置HUB端口特性
* Input          : UINT8 HubPortIndex                           //HUB端口
                   UINT8 FeatureSelt                            //HUB端口特性
* Output         : None
* Return         : ERR_SUCCESS 成功
                   其他        错误
*******************************************************************************/
UINT8   HubSetPortFeature( UINT8 HubPortIndex, UINT8 FeatureSelt )
{
    pSetupReq -> bRequestType = HUB_SET_PORT_FEATURE;
    pSetupReq -> bRequest = HUB_SET_FEATURE;
    pSetupReq -> wValueL = FeatureSelt;
    pSetupReq -> wValueH = 0x00;
    pSetupReq -> wIndexL = HubPortIndex;
    pSetupReq -> wIndexH = 0x00;
    pSetupReq -> wLengthL = 0x00;
    pSetupReq -> wLengthH = 0x00;
    return( HostCtrlTransfer( NULL, NULL ) );  // 执行控制传输
}
/*******************************************************************************
* Function Name  : HubClearPortFeature
* Description    : 清除HUB端口特性
* Input          : UINT8 HubPortIndex                           //HUB端口
                   UINT8 FeatureSelt                            //HUB端口特性
* Output         : None
* Return         : ERR_SUCCESS 成功
                   其他        错误
*******************************************************************************/
UINT8   HubClearPortFeature( UINT8 HubPortIndex, UINT8 FeatureSelt )
{
    pSetupReq -> bRequestType = HUB_CLEAR_PORT_FEATURE;
    pSetupReq -> bRequest = HUB_CLEAR_FEATURE;
    pSetupReq -> wValueL = FeatureSelt;
    pSetupReq -> wValueH = 0x00;
    pSetupReq -> wIndexL = HubPortIndex;
    pSetupReq -> wIndexH = 0x00;
    pSetupReq -> wLengthL = 0x00;
    pSetupReq -> wLengthH = 0x00;
    return( HostCtrlTransfer( NULL, NULL ) );  // 执行控制传输
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
    for ( i = 0; i < ( (PXUSB_CFG_DESCR)buf ) -> wTotalLengthL; i += l )             // 搜索中断端点描述符,跳过配置描述符和接口描述符
    {
        if ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bDescriptorType == USB_DESCR_TYP_ENDP  // 是端点描述符
                && ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bmAttributes & USB_ENDP_TYPE_MASK ) == USB_ENDP_TYPE_INTER// 是中断端点
                && ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_DIR_MASK ) )// 是IN端点
        {
            s = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;// 中断端点的地址
            break;                                                                   // 可以根据需要保存wMaxPacketSize和bInterval
        }
        l = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bLength;                                // 当前描述符长度,跳过
        if ( l > 16 )
        {
            break;
        }
    }
    return( s );
}

/*******************************************************************************
* Function Name  : AnalyzeBulkEndp,仅用于分析RootHub
* Description    : 分析出批量端点,GpVar[0]、GpVar[1]存放上传端点。GpVar[2]、GpVar[3]存放下传端点
* Input          : buf：待分析数据缓冲区地址   HubPortIndex：0表示根HUB，非0表示外部HUB下的端口号
* Output         : None
* Return         : 0
*******************************************************************************/
UINT8   AnalyzeBulkEndp( PUINT8X buf,UINT8 RootHubIndex ) 
{
    UINT8   i, s1,s2, l;
    s1 = 0;s2 = 2;

	memset( RootHubDev[RootHubIndex].GpVar,0,sizeof(RootHubDev[RootHubIndex].GpVar) );  //清空数组

    for ( i = 0; i < ( (PXUSB_CFG_DESCR)buf ) -> wTotalLengthL; i += l )       // 搜索中断端点描述符,跳过配置描述符和接口描述符
    {
        if ( (( (PXUSB_ENDP_DESCR)(buf+i) ) -> bDescriptorType == USB_DESCR_TYP_ENDP)     // 是端点描述符
                && ((( (PXUSB_ENDP_DESCR)(buf+i) ) -> bmAttributes & USB_ENDP_TYPE_MASK ) == USB_ENDP_TYPE_BULK))  // 是批量

        {

			if(( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_DIR_MASK )
				RootHubDev[RootHubIndex].GpVar[s1++] = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
			else
				RootHubDev[RootHubIndex].GpVar[s2++] = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
	
			if(s1 == 2) s1 = 1;
			if(s2 == 4) s2 = 3;			
		}
        l = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bLength;                          // 当前描述符长度,跳过
        if ( l > 16 )
        {
            break;
        }
    }
    return( 0 );
}

//尝试启动AOA模式
UINT8 TouchStartAOA(void)
{
	UINT8 s,i,Num;
	UINT16 len;
    //获取协议版本号
    CopySetupReqPkg( GetProtocol );
    s = HostCtrlTransfer( COM_BUF, &len );  // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
	if(COM_BUF[0]<2) return  ERR_AOA_PROTOCOL;

    //输出字符串
    for(i=0; i<6; i++)
    {
        Num=Sendlen[i];
        CopySetupReqPkg(&SetStringID[8*i]);
        s = HostCtrlTransfer( &StringID[Num], &len );  // 执行控制传输
        if ( s != ERR_SUCCESS )
        {
            return( s );
        }
    }	

    CopySetupReqPkg(TouchAOAMode);
    s = HostCtrlTransfer( COM_BUF, &len );  // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    return ERR_SUCCESS;	
}

/*******************************************************************************
* Function Name  : InitRootDevice
* Description    : 初始化指定ROOT-HUB端口的USB设备
* Input          : UINT8 RootHubIndex 指定端口，内置HUB端口号0/1
* Output         : None
* Return         :
*******************************************************************************/
UINT8   InitRootDevice( UINT8 RootHubIndex )
{
    UINT8   i, s, cfg, dv_cls, if_cls;
	UINT8  touchaoatm = 0;
    UINT8  retry=0;
    printf( "Reset root hub %1d# port\n", (UINT16)RootHubIndex );
    while(retry<10)
    {
        mDelaymS( 100*retry );
        retry++;
        ResetRootHubPort( RootHubIndex );                    // 检测到设备后,复位相应端口的USB总线
        for ( i = 0, s = 0; i < 100; i ++ )                            // 等待USB设备复位后重新连接,100mS超时
        {
            mDelaymS( 1 );
            if ( EnableRootHubPort( RootHubIndex ) == ERR_SUCCESS )    // 使能ROOT-HUB端口
            {
                i = 0;
                s ++;                                                  // 计时等待USB设备连接后稳定
                if ( s > 10*retry )
                {
                    break;                                             // 已经稳定连接15mS
                }
            }
        }
        if ( i )                                                       // 复位后设备没有连接
        {
            DisableRootHubPort( RootHubIndex );
            printf( "Disable root hub %1d# port because of disconnect\n", (UINT16)RootHubIndex );
//         return( ERR_USB_DISCON );
            continue;
        }
        SelectHubPort( RootHubIndex, 0 );
        printf( "GetDevDescr: " );
        s = CtrlGetDeviceDescr( );                                     // 获取设备描述符
        if ( s == ERR_SUCCESS )
        {
            for ( i = 0; i < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL; i ++ )
            {
                printf( "x%02X ", (UINT16)( COM_BUF[i] ) );
            }
            printf( "\n" );                                            // 显示出描述符
			
			RootHubDev[RootHubIndex].DeviceVID=(((UINT16)((PXUSB_DEV_DESCR)COM_BUF)->idVendorH)<<8 ) + ((PXUSB_DEV_DESCR)COM_BUF)->idVendorL; //保存VID PID信息
			RootHubDev[RootHubIndex].DevicePID=(((UINT16)((PXUSB_DEV_DESCR)COM_BUF)->idProductH)<<8 ) + ((PXUSB_DEV_DESCR)COM_BUF)->idProductL;
            dv_cls = ( (PXUSB_DEV_DESCR)COM_BUF ) -> bDeviceClass;    // 设备类代码
            s = CtrlSetUsbAddress( RootHubIndex + ( (PUSB_SETUP_REQ)SetupSetUsbAddr ) -> wValueL );// 设置USB设备地址,加上RootHubIndex可以保证2个HUB端口分配不同的地址
            if ( s == ERR_SUCCESS )
            {
                RootHubDev[RootHubIndex].DeviceAddress = RootHubIndex + ( (PUSB_SETUP_REQ)SetupSetUsbAddr ) -> wValueL;  // 保存USB地址
                printf( "GetCfgDescr: " );
                s = CtrlGetConfigDescr( );                             // 获取配置描述符
                if ( s == ERR_SUCCESS )
                {
                    cfg = ( (PXUSB_CFG_DESCR)COM_BUF ) -> bConfigurationValue;
                    for ( i = 0; i < ( (PXUSB_CFG_DESCR)COM_BUF ) -> wTotalLengthL; i ++ )
                    {
                        printf( "x%02X ", (UINT16)( COM_BUF[i] ) );
                    }
                    printf("\n");
                    //分析配置描述符,获取端点数据/各端点地址/各端点大小等,更新变量endp_addr和endp_size等
                    if_cls = ( (PXUSB_CFG_DESCR_LONG)COM_BUF ) -> itf_descr.bInterfaceClass;  // 接口类代码
                    if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE )// 是USB存储类设备,基本上确认是U盘
                    {
                        s = CtrlSetUsbConfig( cfg );                   // 设置USB设备配置
                        if ( s == ERR_SUCCESS )
                        {
                            RootHubDev[RootHubIndex].DeviceStatus = ROOT_DEV_SUCCESS;
                            RootHubDev[RootHubIndex].DeviceType = USB_DEV_CLASS_STORAGE;
                            printf( "USB-Disk Ready\n" );
                            SetUsbSpeed( 1 );                          // 默认为全速
                            return( ERR_SUCCESS );
                        }
                    }
                    else if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_HID && ( (PXUSB_CFG_DESCR_LONG)COM_BUF ) -> itf_descr.bInterfaceSubClass <= 0x01 )// 是HID类设备,键盘/鼠标等
                    {
                        s = AnalyzeHidIntEndp( COM_BUF );             // 从描述符中分析出HID中断端点的地址
                        RootHubDev[RootHubIndex].GpVar[0] = s & USB_ENDP_ADDR_MASK ;// 保存中断端点的地址,位7用于同步标志位,清0
                        if_cls = ( (PXUSB_CFG_DESCR_LONG)COM_BUF ) -> itf_descr.bInterfaceProtocol;
                        s = CtrlSetUsbConfig( cfg );                   // 设置USB设备配置
                        printf( "HID: %02x",(UINT16)s );
                        if ( s == ERR_SUCCESS )
                        {
                            printf( "GetHIDReport: " );
                            s = CtrlGetHIDDeviceReport();              //获取报表描述符
                            if(s == ERR_SUCCESS)
                            {
                                for ( i = 0; i < 64; i++ )
                                {
                                    printf( "x%02X ", (UINT16)( COM_BUF[i] ) );
                                }
                                printf("\n");
                            }
                            //Set_Idle( );
                            //需保存端点信息以便主程序进行USB传输
                            RootHubDev[RootHubIndex].DeviceStatus = ROOT_DEV_SUCCESS;
                            CtrlSetUsbIDLE(0x0a);
                            if ( if_cls == 1 )
                            {
                                RootHubDev[RootHubIndex].DeviceType = DEV_TYPE_KEYBOARD;
                                //进一步初始化,例如设备键盘指示灯LED等
                                printf( "USB-Keyboard Ready\n" );
                                SetUsbSpeed( 1 );                      // 默认为全速
                                return( ERR_SUCCESS );
                            }
                            else if ( if_cls == 2 )
                            {
                                RootHubDev[RootHubIndex].DeviceType = DEV_TYPE_MOUSE;
                                //为了以后查询鼠标状态,应该分析描述符,取得中断端口的地址,长度等信息
                                printf( "USB-Mouse Ready\n" );
                                SetUsbSpeed( 1 );                      // 默认为全速
                                return( ERR_SUCCESS );
                            }
                            s = ERR_USB_UNSUPPORT;
                        }
                    }
                    else if ( dv_cls == USB_DEV_CLASS_HUB )           // 是HUB类设备,集线器等
                    {
                        s = AnalyzeHidIntEndp( COM_BUF );            // 从描述符中分析出HID中断端点的地址
                        RootHubDev[RootHubIndex].GpVar[1] = s & USB_ENDP_ADDR_MASK ;// 保存中断端点的地址,位7用于同步标志位,清0
                        printf( "GetHubDescr: ");
                        s = CtrlGetHubDescr( );
                        if ( s == ERR_SUCCESS )
                        {
                            for( i = 0; i < TxBuffer[0]; i++ )
                            {
                                printf( "x%02X ",(UINT16)(TxBuffer[i]) );
                            }
                            printf("\n");
                            RootHubDev[RootHubIndex].GpVar[0] = ( (PXUSB_HUB_DESCR)TxBuffer ) -> bNbrPorts;// 保存HUB的端口数量
                            if ( RootHubDev[RootHubIndex].GpVar[0] > HUB_MAX_PORTS )
                            {
                                RootHubDev[RootHubIndex].GpVar[0] = HUB_MAX_PORTS;// 因为定义结构DevOnHubPort时人为假定每个HUB不超过HUB_MAX_PORTS个端口
                            }
                            //if ( ( (PXUSB_HUB_DESCR)TxBuffer ) -> wHubCharacteristics[0] & 0x04 ) printf("带有集线器的复合设备\n");
                            //else printf("单一的集线器产品\n");
                            s = CtrlSetUsbConfig( cfg );               // 设置USB设备配置
                            if ( s == ERR_SUCCESS )
                            {
                                RootHubDev[RootHubIndex].DeviceStatus = ROOT_DEV_SUCCESS;
                                RootHubDev[RootHubIndex].DeviceType = USB_DEV_CLASS_HUB;
                                //需保存端点信息以便主程序进行USB传输,本来中断端点可用于HUB事件通知,但本程序使用查询状态控制传输代替
                                //给HUB各端口上电,查询各端口状态,初始化有设备连接的HUB端口,初始化设备
                                for ( i = 1; i <= RootHubDev[RootHubIndex].GpVar[0]; i ++ )// 给HUB各端口都上电
                                {
                                    DevOnHubPort[RootHubIndex][i-1].DeviceStatus = ROOT_DEV_DISCONNECT;  // 清外部HUB端口上设备的状态
                                    s = HubSetPortFeature( i, HUB_PORT_POWER );
                                    if ( s != ERR_SUCCESS )
                                    {
                                        printf( "Ext-HUB Port_%1d# power on error\n",(UINT16)i );// 端口上电失败
                                    }
                                }
//                             for ( i = 1; i <= RootHubDev[RootHubIndex].GpVar[0]; i ++ ) // 清除HUB各端口连接状态
//                             {
//                                 s = HubClearPortFeature( i, HUB_C_PORT_CONNECTION );
//                                 if ( s != ERR_SUCCESS )
//                                 {
//                                     printf( "Ext-HUB Port_%1d#  clear connection error\n",(UINT16)i );// 端口连接状态清除失败
//                                 }
//                             }
                                for ( i = 1; i <= RootHubDev[RootHubIndex].GpVar[0]; i ++ ) // 查询HUB各端口连接状态
                                {
                                    s = HubGetPortStatus( i );  // 获取端口状态
                                    if ( s != ERR_SUCCESS )
                                    {
                                        printf( "Ext-HUB Port_%1d#	clear connection error\n",(UINT16)i );    // 端口连接状态清除失败
                                    }
                                }
                                SetUsbSpeed( 1 );                                        // 默认为全速
                                return( ERR_SUCCESS );
                            }
                        }
                    }
                    else                                                                 // 可以进一步分析,可能是手机
                    {
                        printf("dv_cls=%02x,if_cls =%02x\n",(UINT16)dv_cls,(UINT16)if_cls);
						AnalyzeBulkEndp(COM_BUF , RootHubIndex );
						for(i=0;i!=4;i++)
						{
							printf("%02x ",(UINT16)RootHubDev[RootHubIndex].GpVar[i] );
						}
						printf("\n");						

						s = CtrlSetUsbConfig( cfg );                                     // 设置USB设备配置
                        if ( s == ERR_SUCCESS )
                        {
							printf("vid pid:%02x %02x\n",(UINT16)RootHubDev[RootHubIndex].DeviceVID,(UINT16)RootHubDev[RootHubIndex].DevicePID);
						
							if((RootHubDev[RootHubIndex].DeviceVID==0x18D1)&&(RootHubDev[RootHubIndex].DevicePID&0xff00)==0x2D00)   //如果是AOA配件
							{
								printf("AOA Mode\n");
								RootHubDev[RootHubIndex].DeviceStatus = ROOT_DEV_SUCCESS;
								RootHubDev[RootHubIndex].DeviceType = DEF_AOA_DEVICE;        //这只是自定义的变量类，不属于USB协议类
								SetUsbSpeed( 1 );                                            // 默认为全速
								return( ERR_SUCCESS );
							}
							else   //如果不是AOA 配件模式，尝试启动配件模式.
							{
								s = TouchStartAOA();
								if( s == ERR_SUCCESS ) 
								{
									if(touchaoatm<3)         //尝试AOA启动次数限制
									{
										touchaoatm++;
										mDelaymS(500);      //部分安卓设备自动断开重连，所以此处最好有延时
										continue;           //其实这里可以不用跳转，AOA协议规定，设备会自动重新接入总线。
									}
									//执行到这，说明可能不支持AOA，或是其他设备
									RootHubDev[RootHubIndex].DeviceType = dv_cls ? dv_cls : if_cls;
									RootHubDev[RootHubIndex].DeviceStatus = ROOT_DEV_SUCCESS;
									SetUsbSpeed( 1 );                                            // 默认为全速
									return( ERR_SUCCESS );                                       // 未知设备初始化成功									
								}							
							}							

                        }
                    }
                }
            }
        }
        printf( "InitRootDev Err = %02X\n", (UINT16)s );
        RootHubDev[RootHubIndex].DeviceStatus = ROOT_DEV_FAILED;
        SetUsbSpeed( 1 );                                                                 // 默认为全速
        continue;
    }
    return( s );
}
/*******************************************************************************
* Function Name  : EnumAllRootDevice
* Description    : 枚举所有ROOT-HUB端口的USB设备
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8   EnumAllRootDevice( void )
{
    UINT8I   s, RootHubIndex;
    printf( "EnumAllRootDev\n" );
    for ( RootHubIndex = 0; RootHubIndex < 2; RootHubIndex ++ )
    {
        printf( "RootHubIndex %02x\n",(UINT16)RootHubIndex );
        if ( RootHubDev[RootHubIndex].DeviceStatus == ROOT_DEV_CONNECTED )           // 刚插入设备尚未初始化
        {
            s = InitRootDevice( RootHubIndex );                                      // 初始化/枚举指定HUB端口的USB设备
            if ( s != ERR_SUCCESS )
            {
                return( s );
            }
        }
    }
    return( ERR_SUCCESS );
}
/*******************************************************************************
* Function Name  : InitDevOnHub
* Description    : 初始化枚举外部HUB后的二级USB设备
* Input          : UINT8 RootHubIndex  指定ROOT_HUB
                   UINT8 HubPortIndex  指定外部HUB
* Output         : None
* Return         : ERR_SUCCESS 成功
                   ERR_USB_UNKNOWN 未知设备
*******************************************************************************/
UINT8   InitDevOnHub( UINT8 RootHubIndex, UINT8 HubPortIndex )
{
    UINT8   i, s, cfg, dv_cls, if_cls;
    printf( "Init dev @ExtHub-port_%1d ", (UINT16)HubPortIndex );
    printf( "@Root_%1d\n", (UINT16)RootHubIndex );
    if ( HubPortIndex == 0 )
    {
        return( ERR_USB_UNKNOWN );
    }
    SelectHubPort( RootHubIndex, HubPortIndex );                                      // 选择操作指定的ROOT-HUB端口的外部HUB的指定端口,选择速度
    printf( "UH_SETUP = GetDevDescr: %02x",(UINT16)UH_SETUP );
    s = CtrlGetDeviceDescr( );                                                        // 获取设备描述符
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    dv_cls = ( (PXUSB_DEV_DESCR)COM_BUF ) -> bDeviceClass;                           // 设备类代码
    cfg = ( (RootHubIndex+1)<<4 ) + HubPortIndex;                                     // 计算出一个USB地址,避免地址重叠
    s = CtrlSetUsbAddress( cfg );                                                     // 设置USB设备地址
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceAddress = cfg;                   // 保存分配的USB地址
    printf( "GetCfgDescr: " );
    s = CtrlGetConfigDescr( );                                                        // 获取配置描述符
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    cfg = ( (PXUSB_CFG_DESCR)COM_BUF ) -> bConfigurationValue;
    for ( i = 0; i < ( (PXUSB_CFG_DESCR)COM_BUF ) -> wTotalLengthL; i ++ )
    {
        printf( "x%02X ", (UINT16)( COM_BUF[i] ) );
    }
    printf("\n");
    /* 分析配置描述符,获取端点数据/各端点地址/各端点大小等,更新变量endp_addr和endp_size等 */
    if_cls = ( (PXUSB_CFG_DESCR_LONG)COM_BUF ) -> itf_descr.bInterfaceClass;         // 接口类代码
    if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE )                          // 是USB存储类设备,基本上确认是U盘
    {
        s = CtrlSetUsbConfig( cfg );                                                  // 设置USB设备配置
        if ( s == ERR_SUCCESS )
        {
            DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceStatus = ROOT_DEV_SUCCESS;
            DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceType = USB_DEV_CLASS_STORAGE;
            printf( "USB-Disk Ready\n" );
            SetUsbSpeed( 1 );                                                         // 默认为全速
            return( ERR_SUCCESS );
        }
    }
    else if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_HID && ( (PXUSB_CFG_DESCR_LONG)COM_BUF ) -> itf_descr.bInterfaceSubClass <= 0x01 )    // 是HID类设备,键盘/鼠标等
    {
        s = AnalyzeHidIntEndp( COM_BUF );                                            // 从描述符中分析出HID中断端点的地址
        DevOnHubPort[RootHubIndex][HubPortIndex-1].GpVar = s;                         // 保存中断端点的地址,位7用于同步标志位,清0
        if_cls = ( (PXUSB_CFG_DESCR_LONG)COM_BUF ) -> itf_descr.bInterfaceProtocol;
        s = CtrlSetUsbConfig( cfg );                                                  // 设置USB设备配置
        if ( s == ERR_SUCCESS )
        {
            //需保存端点信息以便主程序进行USB传输
            DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceStatus = ROOT_DEV_SUCCESS;
            if ( if_cls == 1 )
            {
                DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceType = DEV_TYPE_KEYBOARD;
                //进一步初始化,例如设备键盘指示灯LED等
                printf( "USB-Keyboard Ready\n" );
                SetUsbSpeed( 1 );                                                     // 默认为全速
                return( ERR_SUCCESS );
            }
            else if ( if_cls == 2 )
            {
                DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceType = DEV_TYPE_MOUSE;
                //为了以后查询鼠标状态,应该分析描述符,取得中断端口的地址,长度等信息
                printf( "USB-Mouse Ready\n" );
                SetUsbSpeed( 1 );                                                     // 默认为全速
                return( ERR_SUCCESS );
            }
            s = ERR_USB_UNSUPPORT;
        }
    }
    else if ( dv_cls == USB_DEV_CLASS_HUB )                                           // 是HUB类设备,集线器等
    {
        DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceType = USB_DEV_CLASS_HUB;
        printf( "This program don't support Level 2 HUB\n");                         // 需要支持多级HUB级联请参考本程序进行扩展
        s = HubClearPortFeature( i, HUB_PORT_ENABLE );                               // 禁止HUB端口
        if ( s != ERR_SUCCESS )
        {
            return( s );
        }
        s = ERR_USB_UNSUPPORT;
    }
    else                                                                             // 可以进一步分析
    {
        s = CtrlSetUsbConfig( cfg );                                                 // 设置USB设备配置
        if ( s == ERR_SUCCESS )
        {
            //需保存端点信息以便主程序进行USB传输
            DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceStatus = ROOT_DEV_SUCCESS;
            DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceType = dv_cls ? dv_cls : if_cls;
            SetUsbSpeed( 1 );                                                        // 默认为全速
            return( ERR_SUCCESS );                                                   //未知设备初始化成功
        }
    }
    printf( "InitDevOnHub Err = %02X\n", (UINT16)s );
    DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceStatus = ROOT_DEV_FAILED;
    SetUsbSpeed( 1 );                                                                // 默认为全速
    return( s );
}
/*******************************************************************************
* Function Name  : EnumHubPort
* Description    : 枚举指定ROOT-HUB端口上的外部HUB集线器的各个端口,检查各端口有无连接或移除事件并初始化二级USB设备
* Input          : UINT8 RootHubIndex ROOT_HUB0和ROOT_HUB1
* Output         : None
* Return         : ERR_SUCCESS 成功
                   其他        失败
*******************************************************************************/
UINT8   EnumHubPort( UINT8 RootHubIndex )
{
    UINT8   i, s;
    printf( "EnumHubPort\n" );
    for ( i = 1; i <= RootHubDev[RootHubIndex].GpVar[0]; i ++ )                          // 查询集线器的端口是否有变化
    {
        SelectHubPort( RootHubIndex, 0 );                                             // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
        s = HubGetPortStatus( i );                                                    // 获取端口状态
        if ( s != ERR_SUCCESS )
        {
            return( s );                                                              // 可能是该HUB断开了
        }
        if ( (( TxBuffer[0]&(1<<(HUB_PORT_CONNECTION&0x07)) ) && ( TxBuffer[2]&(1<<(HUB_C_PORT_CONNECTION&0x07)) ))||(TxBuffer[2] == 0x10) )
        {
            // 发现有设备连接
            DevOnHubPort[RootHubIndex][i-1].DeviceStatus = ROOT_DEV_CONNECTED;        // 有设备连接
            DevOnHubPort[RootHubIndex][i-1].DeviceAddress = 0x00;
            s = HubGetPortStatus( i );                                                // 获取端口状态
            if ( s != ERR_SUCCESS )
            {
                return( s );                                                          // 可能是该HUB断开了
            }
            DevOnHubPort[RootHubIndex][i-1].DeviceSpeed = TxBuffer[1] & (1<<(HUB_PORT_LOW_SPEED&0x07)) ? 0 : 1;// 低速还是全速
            if ( DevOnHubPort[RootHubIndex][i-1].DeviceSpeed )
            {
                printf( "Found full speed device on port %1d\n", (UINT16)i );
            }
            else
            {
                printf( "Found low speed device on port %1d\n", (UINT16)i );
            }
            mDelaymS( 200 );                                                          // 等待设备上电稳定
            s = HubSetPortFeature( i, HUB_PORT_RESET );                               // 对有设备连接的端口复位
            if ( s != ERR_SUCCESS )
            {
                return( s );                                                          // 可能是该HUB断开了
            }
            printf( "Reset port and then wait in\n" );
            do                                                                        // 查询复位端口,直到复位完成,把完成后的状态显示出来
            {
                mDelaymS( 1 );
                s = HubGetPortStatus( i );
                if ( s != ERR_SUCCESS )
                {
                    return( s );                                                      // 可能是该HUB断开了
                }
            }
            while ( TxBuffer[0] & (1<<(HUB_PORT_RESET&0x07)) );                       // 端口正在复位则等待
            mDelaymS( 100 );
            s = HubClearPortFeature( i, HUB_C_PORT_RESET );                           // 清除复位完成标志
//             s = HubSetPortFeature( i, HUB_PORT_ENABLE );                              // 启用HUB端口
            s = HubClearPortFeature( i, HUB_C_PORT_CONNECTION );                      // 清除连接或移除变化标志
            if ( s != ERR_SUCCESS )
            {
                return( s );
            }
            s = HubGetPortStatus( i );                                                // 再读取状态,复查设备是否还在
            if ( s != ERR_SUCCESS )
            {
                return( s );
            }
            if ( ( TxBuffer[0]&(1<<(HUB_PORT_CONNECTION&0x07)) ) == 0 )
            {
                DevOnHubPort[RootHubIndex][i-1].DeviceStatus = ROOT_DEV_DISCONNECT;   // 设备不在了
            }
            s = InitDevOnHub( RootHubIndex, i );                                      // 初始化二级USB设备
            if ( s != ERR_SUCCESS )
            {
                return( s );
            }
            SetUsbSpeed( 1 );                                                         // 默认为全速
        }
        else if (TxBuffer[2]&(1<<(HUB_C_PORT_ENABLE&0x07))  )                         // 设备连接出错
        {
            HubClearPortFeature( i, HUB_C_PORT_ENABLE );                              // 清除连接错误标志
            printf( "Device on port error\n" );
            s = HubSetPortFeature( i, HUB_PORT_RESET );                               // 对有设备连接的端口复位
            if ( s != ERR_SUCCESS )
            {
                return( s );    // 可能是该HUB断开了
            }
            do // 查询复位端口,直到复位完成,把完成后的状态显示出来
            {
                mDelaymS( 1 );
                s = HubGetPortStatus( i );
                if ( s != ERR_SUCCESS )
                {
                    return( s );    // 可能是该HUB断开了
                }
            }
            while ( TxBuffer[0] & (1<<(HUB_PORT_RESET&0x07)) );                       // 端口正在复位则等待
        }
        else if ( ( TxBuffer[0]&(1<<(HUB_PORT_CONNECTION&0x07)) ) == 0 )              // 设备已经断开
        {
            if ( DevOnHubPort[RootHubIndex][i-1].DeviceStatus >= ROOT_DEV_CONNECTED )
            {
                printf( "Device on port %1d removed\n", (UINT16)i );
            }
            DevOnHubPort[RootHubIndex][i-1].DeviceStatus = ROOT_DEV_DISCONNECT;       // 有设备连接
            if ( TxBuffer[2]&(1<<(HUB_C_PORT_CONNECTION&0x07)) )
            {
                HubClearPortFeature( i, HUB_C_PORT_CONNECTION );                      // 清除移除变化标志
            }
        }
    }
    return( ERR_SUCCESS );                                                            // 返回操作成功
}
/*******************************************************************************
* Function Name  : EnumAllHubPort
* Description    : 枚举所有ROOT-HUB端口下外部HUB后的二级USB设备
* Input          : None
* Output         : None
* Return         : ERR_SUCCESS 成功
                   其他        失败
*******************************************************************************/
UINT8   EnumAllHubPort( void )
{
    UINT8   s, RootHubIndex;
    printf( "EnumAllHubPort\n" );
    for ( RootHubIndex = 0; RootHubIndex != 2; RootHubIndex ++ )
    {
        if ( RootHubDev[RootHubIndex].DeviceStatus >= ROOT_DEV_SUCCESS && RootHubDev[RootHubIndex].DeviceType == USB_DEV_CLASS_HUB )// HUB枚举成功
        {
            SelectHubPort( RootHubIndex, 0 );                                         // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
            //做点什么?  给HUB各端口上电,查询各端口状态,初始化有设备连接的HUB端口,初始化设备
//             for ( i = 1; i <= RootHubDev[RootHubIndex].GpVar[0]; i ++ ){                 // 初始化HUB各端口
//               s = HubSetPortFeature( i, HUB_PORT_POWER );                             // 给HUB各端口上电
//               if ( s != ERR_SUCCESS )
//               {
//                 return( s );                                                          // 可能是该HUB断开了
//               }
//             }
            s = EnumHubPort( RootHubIndex );                                          // 枚举指定ROOT-HUB端口上的外部HUB集线器的各个端口,检查各端口有无连接或移除事件
            if ( s != ERR_SUCCESS )                                                   // 可能是HUB断开了
            {
                printf( "EnumAllHubPort err = %02X\n", (UINT16)s );
            }
            SetUsbSpeed( 1 );                                                         // 默认为全速
        }
    }
    return( ERR_SUCCESS );
}
/*******************************************************************************
* Function Name  : SearchTypeDevice
* Description    : 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号,输出端口号为0xFFFF则未搜索到
* Input          : UINT8 type 搜索的设备类型
* Output         : None
* Return         : 输出高8位为ROOT-HUB端口号,低8位为外部HUB的端口号,低8位为0则设备直接在ROOT-HUB端口上
                   当然也可以根据USB的厂商VID产品PID进行搜索(事先要记录各设备的VID和PID),以及指定搜索序号
*******************************************************************************/
UINT16  SearchTypeDevice( UINT8 type )
{
    UINT8   RootHubIndex, HubPortIndex;
    for ( RootHubIndex = 0; RootHubIndex != 2; RootHubIndex ++ )                      // 现时搜索可以避免设备中途拔出而某些信息未及时更新的问题
    {
        if ( RootHubDev[RootHubIndex].DeviceType == type && RootHubDev[RootHubIndex].DeviceStatus >= ROOT_DEV_SUCCESS )
        {
            return( (UINT16)RootHubIndex << 8 );                                      // 类型匹配且枚举成功,在ROOT-HUB端口上
        }
        if ( RootHubDev[RootHubIndex].DeviceType == USB_DEV_CLASS_HUB && RootHubDev[RootHubIndex].DeviceStatus >= ROOT_DEV_SUCCESS )// 外部集线器HUB且枚举成功
        {
            for ( HubPortIndex = 1; HubPortIndex <= RootHubDev[RootHubIndex].GpVar[0]; HubPortIndex ++ )// 搜索外部HUB的各个端口
            {
                if ( DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceType == type && DevOnHubPort[RootHubIndex][HubPortIndex-1].DeviceStatus >= ROOT_DEV_SUCCESS )
                {
                    return( ( (UINT16)RootHubIndex << 8 ) | HubPortIndex );           // 类型匹配且枚举成功
                }
            }
        }
    }
    return( 0xFFFF );
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
    UINT8   i;
    IE_USB = 0;
//  LED_CFG = 1;
//  LED_RUN = 0;
    USB_CTRL = bUC_HOST_MODE;                                                       // 先设定模式
    USB_DEV_AD = 0x00;
    UH_EP_MOD = bUH_EP_TX_EN | bUH_EP_RX_EN ;
    UH_RX_DMA = RxBuffer;
    UH_TX_DMA = TxBuffer;
    UH_RX_CTRL = 0x00;
    UH_TX_CTRL = 0x00;
    USB_CTRL = bUC_HOST_MODE | bUC_INT_BUSY | bUC_DMA_EN;                           // 启动USB主机及DMA,在中断标志未清除前自动暂停
//  UHUB0_CTRL = 0x00;
//  UHUB1_CTRL = 0x00;
    UH_SETUP = bUH_SOF_EN;
    USB_INT_FG = 0xFF;                                                              // 清中断标志
    for ( i = 0; i != 2; i ++ )
    {
        DisableRootHubPort( i );                                                    // 清空
    }
    USB_INT_EN = bUIE_TRANSFER | bUIE_DETECT;
//  IE_USB = 1;                                                                     // 查询方式
}


main( )
{
    UINT8   i,k,s, len, endp,HUBFlag;
    UINT16  loc;
    HUBFlag = 0;
    mInitSTDIO( );                                                                  //为了让计算机通过串口监控演示过程
    printf( "Start @ChipID=%02X\n", (UINT16)CHIP_ID );
    InitUSB_Host( );
    FoundNewDev = 0;
    printf( "Wait Device In\n" );
    while ( 1 )
    {
        s = ERR_SUCCESS;
        if ( UIF_DETECT )                                                           // 如果有USB主机检测中断则处理
        {
            UIF_DETECT = 0;                                                         // 清中断标志
            s = AnalyzeRootHub( );                                                  // 分析ROOT-HUB状态
            if ( s == ERR_USB_CONNECT )
            {
                FoundNewDev = 1;
            }
        }
        if ( FoundNewDev && s == ERR_USB_CONNECT )                                  // 有新的USB设备插入
        {
            FoundNewDev = 0;
            mDelaymS( 200 );                                                        // 由于USB设备刚插入尚未稳定,故等待USB设备数百毫秒,消除插拔抖动
            s = EnumAllRootDevice( );                                               // 枚举所有ROOT-HUB端口的USB设备
            if ( s != ERR_SUCCESS )
            {
                printf( "EnumAllRootDev err = %02X\n", (UINT16)s );
            }
        }
        loc = SearchTypeDevice( DEF_AOA_DEVICE );                               // 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号
        if ( loc != 0xFFFF )                                                    // 找到了,如果有两个MOUSE如何处理?
        {
            i = (UINT8)( loc >> 8 );
            len = (UINT8)loc;
            SelectHubPort( i, len );                                            // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
            endp = len ? DevOnHubPort[i][len-1].GpVar : RootHubDev[i].GpVar[0];    // 找到IN端点

            if ( endp & USB_ENDP_ADDR_MASK )                                    // 端点有效
            {
                s = USBHostTransact( USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0 );// CH559传输事务,获取数据,NAK不重试
                if ( s == ERR_SUCCESS )
                {
                    endp ^= 0x80;                                               // 同步标志翻转
                    if ( len )
                    {
                        DevOnHubPort[i][len-1].GpVar = endp;                    // 保存同步标志位
                    }
                    else
                    {
                        RootHubDev[i].GpVar[0] = endp;
                    }
                    len = USB_RX_LEN;                                           // 接收到的数据长度
                    if ( len )
                    {
                        printf("recv data: ");
                        for ( k = 0; k < len; k ++ )
                        {
                            printf("x%02X ",(UINT16)(RxBuffer[k]) );
                        }
                        printf("\n");
                    }
					
					//回传数据
					memcpy(TxBuffer,RxBuffer,len);                            //回传
					endp = RootHubDev[i].GpVar[2];                            //下传端点发OUT包
					UH_TX_LEN = len; 
					s = USBHostTransact( USB_PID_OUT << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0xffff ); //无限次重试下传
					if(s == ERR_SUCCESS)
					{
						endp ^= 0x80;                                       // 同步标志翻转  
						RootHubDev[i].GpVar[2] = endp;                         // 保存同步标志位						
						printf("send back\n");
					}					

                }
                else if ( s != ( USB_PID_NAK | ERR_USB_TRANSFER ) )
                {
                    printf("transmit error %02x\n",(UINT16)s);                      // 可能是断开了
                }
            }
            else
            {
                printf("no interrupt endpoint\n");
            }
            SetUsbSpeed( 1 );                                                    // 默认为全速
        }

    }
}
