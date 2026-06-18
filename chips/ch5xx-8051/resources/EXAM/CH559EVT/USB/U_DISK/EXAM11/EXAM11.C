
/********************************** (C) COPYRIGHT *******************************
* File Name          :EXAM11.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        :
 CH559 C语言的U盘文件枚举例子
 查找/C51/CH559HFT.C文件，如果没有找到则枚举根目录下不超过10000个文件包括目录(实际不限制上限)
 支持: FAT12/FAT16/FAT32
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#include <CH559.H>
#include <stdio.h>
#include <string.h>
#include "..\..\..\DEBUG.H"
#include "..\..\..\DEBUG.C"

//#define DISK_BASE_BUF_LEN     512 /* 默认的磁盘数据缓冲区大小为512字节(可以选择为2048甚至4096以支持某些大扇区的U盘),为0则禁止在本文件中定义缓冲区并由应用程序在pDISK_BASE_BUF中指定 */
#define FOR_ROOT_UDISK_ONLY      1// 只用于DP/DM端口的U盘文件操作(使用子程序库CH559UFI/X),不支持HUB下U盘操作

//还需要添加LIB库文件
//#define NO_DEFAULT_ACCESS_SECTOR  1       /* 禁止默认的磁盘扇区读写子程序,下面用自行编写的程序代替它 */
//#define NO_DEFAULT_DISK_CONNECT       1       /* 禁止默认的检查磁盘连接子程序,下面用自行编写的程序代替它 */
//#define NO_DEFAULT_FILE_ENUMER        1       /* 禁止默认的文件名枚举回调程序,下面用自行编写的程序代替它 */
#include "../../../USB_LIB/CH559UFI.H"
#include "../../../USB_LIB/CH559UFI.C"

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

UINT8X  RxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0000 ;  // IN, must even address
UINT8X  TxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0040 ;  // OUT, must even address
#define pSetupReq   ((PXUSB_SETUP_REQ)TxBuffer)
struct _RootHubDev{
	UINT8	DeviceStatus;			// 设备状态,0-无设备,1-有设备但尚未初始化,2-有设备但初始化枚举失败,3-有设备且初始化枚举成功
	UINT8	DeviceAddress;		// 设备被分配的USB地址
	UINT8	DeviceSpeed;			// 0为低速,非0为全速
	UINT8	DeviceType;				// 设备类型
	UINT8	GpVar;					  // 通用变量
} xdata RootHubDev[2];

bit     FoundNewDev;
bit		  RootHubId;		    // 当前正在操作的root-hub端口号:0=HUB0,1=HUB1
#pragma NOAREGS

void    mDelayuS( UINT16 n );                        // 以uS为单位延时
void    mDelaymS( UINT16 n );                        // 以mS为单位延时
void    DisableRootHubPort( UINT8 RootHubIndex );    // 关闭端口,实际上硬件已经自动关闭,此处只是清除一些结构状态
UINT8   AnalyzeRootHub( void );                      // 分析端口状态,处理ROOT-HUB端口的设备插拔事件
// 返回ERR_SUCCESS为没有情况,返回ERR_USB_CONNECT为检测到新连接,返回ERR_USB_DISCON为检测到断开
void    SetHostUsbAddr( UINT8 addr );                // 设置USB主机当前操作的USB设备地址
void    ResetRootHubPort( UINT8 RootHubIndex );      // 检测到设备后,复位总线,为枚举设备准备,设置为默认为全速
UINT8   EnableRootHubPort( UINT8 RootHubIndex );     // 使能端口,相应的bUH_PORT_EN置1开启端口,设备断开可能导致返回失败
void	  SelectHubPort( UINT8 RootHubIndex);          // 选择操作指定的ROOT-HUB端口
UINT8   WaitUSB_Interrupt( void );                   // 等待USB中断
// CH559传输事务,输入目的端点地址/PID令牌,同步标志,以20uS为单位的NAK重试总时间(0则不重试,0xFFFF无限重试),返回0成功,超时/出错重试
UINT8   USBHostTransact( UINT8 endp_pid, UINT8 tog, UINT16 timeout );  // endp_pid: 高4位是token_pid令牌, 低4位是端点地址
UINT8   HostCtrlTransfer( PUINT8X DataBuf, PUINT8I RetLen );  // 执行控制传输,8字节请求码在pSetupReq中,DataBuf为可选的收发缓冲区
// 如果需要接收和发送数据,那么DataBuf需指向有效缓冲区用于存放后续数据,实际成功收发的总长度返回保存在ReqLen指向的字节变量中
void    CopySetupReqPkg( PUINT8C pReqPkt );          // 复制控制传输的请求包
UINT8   CtrlGetDeviceDescr( void );                  // 获取设备描述符,返回在TxBuffer中
UINT8   CtrlGetConfigDescr( void );                  // 获取配置描述符,返回在TxBuffer中
UINT8   CtrlSetUsbAddress( UINT8 addr );             // 设置USB设备地址
UINT8   CtrlSetUsbConfig( UINT8 cfg );               // 设置USB设备配置
UINT8   CtrlClearEndpStall( UINT8 endp );            // 清除端点STALL
UINT8   InitRootDevice( UINT8 RootHubIndex );        // 初始化USB设备
void    mInitSTDIO( void );                          //为printf和getkey输入输出初始化串口
void    InitUSB_Host( void );                        // 初始化USB主机
void    mStopIfError( UINT8 iError );                //检查操作状态,如果错误则显示错误代码并停机

/*******************************************************************************
* Function Name  : DisableRootHubPort(UINT8 RootHubIndex)
* Description    : 关闭端口,实际上硬件已经自动关闭,此处只是清除一些结构状态
* Input          : UINT8 RootHubIndex  HUB口
* Output         : None
* Return         : None
*******************************************************************************/
void  DisableRootHubPort( UINT8 RootHubIndex )
{
	RootHubDev[ RootHubIndex ].DeviceStatus = ROOT_DEV_DISCONNECT;
	RootHubDev[ RootHubIndex ].DeviceAddress = 0x00;
	if ( RootHubIndex == 1 ) UHUB1_CTRL = 0x00;  // 清除有关HUB1的控制数据,实际上不需要清除
	else UHUB0_CTRL = 0x00;                      // 清除有关HUB0的控制数据,实际上不需要清除
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
	UINT8	s = ERR_SUCCESS;
	if ( USB_HUB_ST & bUHS_H0_ATTACH ) {                      // 设备存在
		if ( RootHubDev[0].DeviceStatus == ROOT_DEV_DISCONNECT  // 检测到有设备插入
			|| ( UHUB0_CTRL & bUH_PORT_EN ) == 0x00 ) {           // 检测到有设备插入,但尚未允许,说明是刚插入
			DisableRootHubPort( 0 );                              // 关闭端口
			RootHubDev[0].DeviceSpeed = USB_HUB_ST & bUHS_DM_LEVEL ? 0 : 1;
			RootHubDev[0].DeviceStatus = ROOT_DEV_CONNECTED;     //置连接标志
			printf( "HUB 0 dev in\n" );
			s = ERR_USB_CONNECT;
		}
	}
	else if ( RootHubDev[0].DeviceStatus >= ROOT_DEV_CONNECTED ) {  //检测到设备拔出
		DisableRootHubPort( 0 );                                // 关闭端口
		printf( "HUB 0 dev out\n" );
		if ( s == ERR_SUCCESS ) s = ERR_USB_DISCON;
	}
	if ( USB_HUB_ST & bUHS_H1_ATTACH ) {                      // 设备存在
		if ( RootHubDev[1].DeviceStatus == ROOT_DEV_DISCONNECT  // 检测到有设备插入
			|| ( UHUB1_CTRL & bUH_PORT_EN ) == 0x00 ) {           // 检测到有设备插入,但尚未允许,说明是刚插入
			DisableRootHubPort( 1 );                              // 关闭端口
			RootHubDev[1].DeviceSpeed = USB_HUB_ST & bUHS_HM_LEVEL ? 0 : 1;
			RootHubDev[1].DeviceStatus = ROOT_DEV_CONNECTED;      //置连接标志
			printf( "HUB 1 dev in\n" );
			s = ERR_USB_CONNECT;
		}
	}
	else if ( RootHubDev[1].DeviceStatus >= ROOT_DEV_CONNECTED ) {  //检测到设备拔出
		DisableRootHubPort( 1 );                                 // 关闭端口
		printf( "HUB 1 dev out\n" );
		if ( s == ERR_SUCCESS ) s = ERR_USB_DISCON;
	}
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
* Function Name  : ResetRootHubPort
* Description    : 检测到设备后,复位总线,为枚举设备准备,设置为默认为全速
* Input          : UINT8 RootHubIndex
* Output         : None
* Return         : None
*******************************************************************************/
void    ResetRootHubPort( UINT8 RootHubIndex )
{
	UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;  //USB设备的端点0的最大包尺寸
	SetHostUsbAddr( 0x00 );
	SetUsbSpeed( 1 );                      // 默认为全速
	if ( RootHubIndex == 1 ) {
		UHUB1_CTRL = UHUB1_CTRL & ~ bUH_LOW_SPEED | bUH_BUS_RESET;// 默认为全速,开始复位
		mDelaymS( 15 );                      // 复位时间10mS到20mS
		UHUB1_CTRL = UHUB1_CTRL & ~ bUH_BUS_RESET;// 结束复位
	}
	else {
		UHUB0_CTRL = UHUB0_CTRL & ~ bUH_LOW_SPEED | bUH_BUS_RESET;// 默认为全速,开始复位
		mDelaymS( 15 );                     // 复位时间10mS到20mS
		UHUB0_CTRL = UHUB0_CTRL & ~ bUH_BUS_RESET; // 结束复位
	}
	mDelayuS( 250 );
	UIF_DETECT = 0;                       // 清中断标志
}
/*******************************************************************************
* Function Name  : EnableRootHubPort
* Description    : 使能端口,相应的bUH_PORT_EN置1开启端口,设备断开可能导致返回失败
* Input          : UINT8 RootHubIndex
* Output         : None
* Return         : 返回ERR_SUCCESS为检测到新连接,返回ERR_USB_DISCON为无连接
*******************************************************************************/
UINT8   EnableRootHubPort( UINT8 RootHubIndex )
{
	if ( RootHubDev[ RootHubIndex ].DeviceStatus < ROOT_DEV_CONNECTED ) RootHubDev[ RootHubIndex ].DeviceStatus = ROOT_DEV_CONNECTED;
	if ( RootHubIndex == 1 ) {
		if ( USB_HUB_ST & bUHS_H1_ATTACH ) {  // 有设备
			if ( ( UHUB1_CTRL & bUH_PORT_EN ) == 0x00 ) {  // 尚未使能
				RootHubDev[1].DeviceSpeed = USB_HUB_ST & bUHS_HM_LEVEL ? 0 : 1;
				if ( RootHubDev[1].DeviceSpeed == 0 ) UHUB1_CTRL |= bUH_LOW_SPEED;  // 低速
			}
			UHUB1_CTRL |= bUH_PORT_EN;  //使能HUB端口
			return( ERR_SUCCESS );
		}
	}
	else {
		if ( USB_HUB_ST & bUHS_H0_ATTACH ) {  // 有设备
			if ( ( UHUB0_CTRL & bUH_PORT_EN ) == 0x00 ) {  // 尚未使能
				RootHubDev[0].DeviceSpeed = USB_HUB_ST & bUHS_DM_LEVEL ? 0 : 1;
				if ( RootHubDev[0].DeviceSpeed == 0 ) UHUB0_CTRL |= bUH_LOW_SPEED;  // 低速
			}
			UHUB0_CTRL |= bUH_PORT_EN;  //使能HUB端口
			return( ERR_SUCCESS );
		}
	}
	return( ERR_USB_DISCON );
}
/*******************************************************************************
* Function Name  : SelectHubPort
* Description    : 选择操作指定的ROOT-HUB端口
* Input          : UINT8 RootHubIndex
* Output         : None
* Return         : 返回ERR_SUCCESS为检测到新连接,返回ERR_USB_DISCON为无连接
*******************************************************************************/
void	SelectHubPort( UINT8 RootHubIndex ) 
{                                                                       // 选择操作指定的ROOT-HUB端口
	SetHostUsbAddr( RootHubDev[RootHubIndex].DeviceAddress );             // 设置USB主机当前操作的USB设备地址
	SetUsbSpeed( RootHubDev[RootHubIndex].DeviceSpeed );                  // 设置当前USB速度
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
//  UINT8   TransRetry;
#define TransRetry  UEP0_T_LEN                         // 节约内存
    UINT8   s, r;
    UINT16  i;
    UH_RX_CTRL = UH_TX_CTRL = tog;
    TransRetry = 0;
    do
    {
//      LED_TMP = 0;
        UH_EP_PID = endp_pid;                           // 指定令牌PID和目的端点号
        UIF_TRANSFER = 0;                               // 允许传输
//      s = WaitUSB_Interrupt( );
        for ( i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i -- )
        {
            ;
        }
        UH_EP_PID = 0x00;                               // 停止USB传输
//      LED_TMP = 1;
//      if ( s != ERR_SUCCESS ) return( s );            // 中断超时,可能是硬件异常
        if ( UIF_TRANSFER == 0 )
        {
            return( ERR_USB_UNKNOWN );
        }
        if ( UIF_DETECT )                               // USB设备插拔事件
        {
//          mDelayuS( 200 );                            // 等待传输完成
            UIF_DETECT = 0;                             // 清中断标志
            s = AnalyzeRootHub( );                      // 分析ROOT-HUB状态
            if ( s == ERR_USB_CONNECT )
            {
                FoundNewDev = 1;
            }
            if ( CH559DiskStatus == DISK_DISCONNECT )
            {
                return( ERR_USB_DISCON );               // USB设备断开事件
            }
            if ( CH559DiskStatus == DISK_CONNECT )
            {
                return( ERR_USB_CONNECT );              // USB设备连接事件
            }
//          if ( ( USB_HUB_ST & bUHS_H0_ATTACH ) == 0x00 ) return( ERR_USB_DISCON );// USB设备断开事件
            mDelayuS( 200 );                            // 等待传输完成
        }
        if ( UIF_TRANSFER )                             // 传输完成
        {
            if ( U_TOG_OK )
            {
                return( ERR_SUCCESS );
            }
#ifdef DEBUG_NOW
            printf("endp_pid=%02X\n",(UINT16)endp_pid);
            printf("USB_INT_FG=%02X\n",(UINT16)USB_INT_FG);
            printf("USB_INT_ST=%02X\n",(UINT16)USB_INT_ST);
            printf("USB_MIS_ST=%02X\n",(UINT16)USB_MIS_ST);
            printf("USB_RX_LEN=%02X\n",(UINT16)USB_RX_LEN);
            printf("UH_TX_LEN=%02X\n",(UINT16)UH_TX_LEN);
            printf("UH_RX_CTRL=%02X\n",(UINT16)UH_RX_CTRL);
            printf("UH_TX_CTRL=%02X\n",(UINT16)UH_TX_CTRL);
            printf("UHUB0_CTRL=%02X\n",(UINT16)UHUB0_CTRL);
            printf("UHUB1_CTRL=%02X\n",(UINT16)UHUB1_CTRL);
#endif
            r = USB_INT_ST & MASK_UIS_H_RES;              // USB设备应答状态
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
                        return( r | ERR_USB_TRANSFER );    // 不是超时/出错,意外应答
                    }
                    break;                                 // 超时重试
                case USB_PID_IN:
//                  if ( U_TOG_OK ) return( ERR_SUCCESS );
//                  if ( tog ? r == USB_PID_DATA1 : r == USB_PID_DATA0 ) return( ERR_SUCCESS );
//                  if ( r == USB_PID_STALL || r == USB_PID_NAK ) return( r | ERR_USB_TRANSFER );
                    if ( r == USB_PID_DATA0 && r == USB_PID_DATA1 )// 不同步则需丢弃后重试
                        {}                                     // 不同步重试
                    else if ( r )
                    {
                        return( r | ERR_USB_TRANSFER );    // 不是超时/出错,意外应答
                    }
                    break;                                 // 超时重试
                default:
                    return( ERR_USB_UNKNOWN );             // 不可能的情况
                    break;
                }
        }
        else                                               // 其它中断,不应该发生的情况
        {
            USB_INT_FG = 0xFF;                             // 清中断标志
        }
        mDelayuS( 15 );
    }
    while ( ++ TransRetry < 3 );
    return( ERR_USB_TRANSFER );                            // 应答超时
}
/*******************************************************************************
* Function Name  : HostCtrlTransfer
* Description    : 执行控制传输,8字节请求码在pSetupReq中,DataBuf为可选的收发缓冲区
* Input          : PUINT8X DataBuf 如果需要接收和发送数据,那么DataBuf需指向有效缓冲区用于存放后续数据
                   PUINT8I RetLen  实际成功收发的总长度保存在RetLen指向的字节变量中
* Output         : None
* Return         : ERR_USB_BUF_OVER IN状态阶段出错
                   ERR_SUCCESS      数据交换成功
                   其他错误状态
*******************************************************************************/
UINT8   HostCtrlTransfer( PUINT8X DataBuf, PUINT8I RetLen )
{
    UINT8   s, RemLen, RxLen, RxCnt, TxCnt;
    PUINT8X xdata   pBuf;
    PUINT8I xdata   pLen;
    pBuf = DataBuf;
    pLen = RetLen;
    mDelayuS( 200 );
    if ( pLen )
    {
        *pLen = 0;                                          // 实际成功收发的总长度
    }
    UH_TX_LEN = sizeof( USB_SETUP_REQ );
    s = USBHostTransact( USB_PID_SETUP << 4 | 0x00, 0x00, 200000/20 );// SETUP阶段,200mS超时
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    UH_RX_CTRL = UH_TX_CTRL = bUH_R_TOG | bUH_R_AUTO_TOG | bUH_T_TOG | bUH_T_AUTO_TOG;// 默认DATA1
    UH_TX_LEN = 0x01;                                      // 默认无数据故状态阶段为IN
    RemLen = pSetupReq -> wLengthH ? 0xFF : pSetupReq -> wLengthL;
    if ( RemLen && pBuf )                                  // 需要收发数据
    {
        if ( pSetupReq -> bRequestType & USB_REQ_TYP_IN )  // 收
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
                    *pLen += RxLen;                        // 实际成功收发的总长度
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
                    break;                                 // 短包
                }
            }
            UH_TX_LEN = 0x00;                              // 状态阶段为OUT
        }
        else                                               // 发
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
                    *pLen += UH_TX_LEN;                     // 实际成功收发的总长度
                }
            }
//          UH_TX_LEN = 0x01;                               // 状态阶段为IN
        }
    }
    mDelayuS( 200 );
    s = USBHostTransact( ( UH_TX_LEN ? USB_PID_IN << 4 | 0x00: USB_PID_OUT << 4 | 0x00 ), bUH_R_TOG | bUH_T_TOG, 200000/20 );// STATUS阶段
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( UH_TX_LEN == 0 )
    {
        return( ERR_SUCCESS );                              // 状态OUT
    }
    if ( USB_RX_LEN == 0 )
    {
        return( ERR_SUCCESS );                              // 状态IN,检查IN状态返回数据长度
    }
    return( ERR_USB_BUF_OVER );                             // IN状态阶段错误
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
    UINT8D  len;
    UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;
    CopySetupReqPkg( SetupGetDevDescr );
    s = HostCtrlTransfer( TxBuffer, &len );                     // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    UsbDevEndp0Size = ( (PXUSB_DEV_DESCR)TxBuffer ) -> bMaxPacketSize0;// 端点0最大包长度,这是简化处理,正常应该先获取前8字节后立即更新UsbDevEndp0Size再继续
    if ( len < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL )
    {
        return( ERR_USB_BUF_OVER );                             // 描述符长度错误
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
    UINT8D  len;
    CopySetupReqPkg( SetupGetCfgDescr );
    s = HostCtrlTransfer( TxBuffer, &len );                      // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < ( (PUSB_SETUP_REQ)SetupGetCfgDescr ) -> wLengthL )
    {
        return( ERR_USB_BUF_OVER );                              // 返回长度错误
    }
    len = ( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthL;
    if ( len > MAX_PACKET_SIZE )
    {
        return( ERR_USB_BUF_OVER );                              // 返回长度错误
    }
    CopySetupReqPkg( SetupGetCfgDescr );
    pSetupReq -> wLengthL = len;                                 // 完整配置描述符的总长度
    s = HostCtrlTransfer( TxBuffer, &len );                      // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < ( (PUSB_SETUP_REQ)SetupGetCfgDescr ) -> wLengthL || len < ( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthL )
    {
        return( ERR_USB_BUF_OVER );                              // 描述符长度错误
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
UINT8   CtrlSetUsbConfig( UINT8 cfg )                           // 设置USB设备配置
{
    CopySetupReqPkg( SetupSetUsbConfig );
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
    for ( i = 0; i < ( (PXUSB_CFG_DESCR)buf ) -> wTotalLengthL; i += l )// 搜索中断端点描述符,跳过配置描述符和接口描述符
    {
        if ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bDescriptorType == USB_DESCR_TYP_ENDP// 是端点描述符
                && ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bmAttributes & USB_ENDP_TYPE_MASK ) == USB_ENDP_TYPE_INTER // 是中断端点
                && ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_DIR_MASK ) )// 是IN端点
        {
            s = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;// 中断端点的地址
            break;                                             // 可以根据需要保存wMaxPacketSize和bInterval
        }
        l = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bLength;          // 当前描述符长度,跳过
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
* Input          : UINT8 RootHubIndex  内置HUB端口号0/1
* Output         : None
* Return         :
*******************************************************************************/
UINT8   InitRootDevice( UINT8 RootHubIndex ) 
{
    UINT8   i, s, cfg, dv_cls, if_cls;
    printf( "Reset host port\n" );
    ResetRootHubPort( RootHubIndex );                           // 检测到设备后,复位相应端口的USB总线
    for ( i = 0, s = 0; i < 100; i ++ )                         // 等待USB设备复位后重新连接,100mS超时
    {
        mDelaymS( 1 );
        if ( EnableRootHubPort( RootHubIndex ) == ERR_SUCCESS ) // 使能端口
        {
            i = 0;
            s ++;                                               // 计时等待USB设备连接后稳定
            if ( s > 20 )
            {
                break;                                          // 已经稳定连接20mS
            }
        }
    }
    if ( i )                                                    // 复位后设备没有连接
    {
        DisableRootHubPort( RootHubIndex );
        printf( "Disable host port because of disconnect\n" );
        return( ERR_USB_DISCON );
    }
	  SelectHubPort( RootHubIndex);		
    printf( "GetDevDescr: " );
    s = CtrlGetDeviceDescr( );                                  // 获取设备描述符
    if ( s == ERR_SUCCESS )
    {
        for ( i = 0; i < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL; i ++ )
        {
            printf( "x%02X ", (UINT16)( TxBuffer[i] ) );
        }
        printf( "\n" );                                          // 显示出描述符
        dv_cls = ( (PXUSB_DEV_DESCR)TxBuffer ) -> bDeviceClass;  // 设备类代码
		    s = CtrlSetUsbAddress( RootHubIndex + ( (PUSB_SETUP_REQ)SetupSetUsbAddr ) -> wValueL );//设置USB设备地址,加上RootHubIndex可以保证2个HUB端口分配不同的地址
        if ( s == ERR_SUCCESS )
        {
			      RootHubDev[RootHubIndex].DeviceAddress = RootHubIndex + ( (PUSB_SETUP_REQ)SetupSetUsbAddr ) -> wValueL;// 保存USB地址
            printf( "GetCfgDescr: " );
            s = CtrlGetConfigDescr( );                           // 获取配置描述符
            if ( s == ERR_SUCCESS )
            {
                cfg = ( (PXUSB_CFG_DESCR)TxBuffer ) -> bConfigurationValue;
                for ( i = 0; i < ( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthL; i ++ )
                {
                    printf( "x%02X ", (UINT16)( TxBuffer[i] ) );
                }
                printf("\n");
                /* 分析配置描述符,获取端点数据/各端点地址/各端点大小等,更新变量endp_addr和endp_size等 */
                if_cls = ( (PXUSB_CFG_DESCR_LONG)TxBuffer ) -> itf_descr.bInterfaceClass;// 接口类代码
                if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE ) // 是USB存储类设备,基本上确认是U盘
                {
					          s = CtrlSetUsbConfig( cfg );  // 设置USB设备配置									
										if ( s == ERR_SUCCESS ) {
											RootHubDev[RootHubIndex].DeviceStatus = ROOT_DEV_SUCCESS;
											RootHubDev[RootHubIndex].DeviceType = USB_DEV_CLASS_STORAGE;
											printf( "USB-Disk Ready\n" );
											SetUsbSpeed( 1 );  // 默认为全速
											return( ERR_SUCCESS );
										}
                }
             }
         }
	  }
    printf( "InitRootDev Err = %02X\n", (UINT16)s );
    CH559DiskStatus = DISK_CONNECT;
    SetUsbSpeed( 1 );                                            // 默认为全速
    return( s );
}

/*******************************************************************************
* Function Name  : EnumAllRootDevice
* Description    : 枚举所有ROOT-HUB端口的USB设备
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT8	EnumAllRootDevice( void )  
{
	UINT8	s, RootHubIndex;
	printf( "EnumAllRootDev\n" );
	for ( RootHubIndex = 0; RootHubIndex != 2; RootHubIndex ++ ) {
		if ( RootHubDev[RootHubIndex].DeviceStatus == ROOT_DEV_CONNECTED ) {   // 刚插入设备尚未初始化
			s = InitRootDevice( RootHubIndex );                                  // 初始化/枚举指定HUB端口的USB设备
			if ( s != ERR_SUCCESS ) return( s );
		}
	}
	return( ERR_SUCCESS );
}
/*******************************************************************************
* Function Name  : SearchTypeDevice
* Description    : 在ROOT-HUB上搜索指定类型的设备所在的端口号,输出端口号为0xFFFF则未搜索到
                   输出高8位为ROOT-HUB端口号,低8位为外部HUB的端口号,低8位为0则设备直接在ROOT-HUB端口上
                   当然也可以根据USB的厂商VID产品PID进行搜索(事先要记录各设备的VID和PID),以及指定搜索序号
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT16	SearchTypeDevice( UINT8 type )  
{
	UINT8	RootHubIndex;
	for ( RootHubIndex = 0; RootHubIndex != 2; RootHubIndex ++ ) {  // 现时搜索可以避免设备中途拔出而某些信息未及时更新的问题
		if ( RootHubDev[RootHubIndex].DeviceType == type && RootHubDev[RootHubIndex].DeviceStatus >= ROOT_DEV_SUCCESS ) return( (UINT16)RootHubIndex << 8 );  // 类型匹配且枚举成功,在ROOT-HUB端口上
	}
	return( 0xFFFF );
}
/*******************************************************************************
* Function Name  : InitUSB_Host
* Description    : 初始化USB主机
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void    InitUSB_Host( void )                                    // 初始化USB主机
{
    IE_USB = 0;
//  LED_CFG = 1;
//  LED_RUN = 0;
    USB_CTRL = bUC_HOST_MODE;                                   // 先设定模式
    USB_DEV_AD = 0x00;
    UH_EP_MOD = bUH_EP_TX_EN | bUH_EP_RX_EN ;
    UH_RX_DMA = RxBuffer;
    UH_TX_DMA = TxBuffer;
    UH_RX_CTRL = 0x00;
    UH_TX_CTRL = 0x00;
    USB_CTRL = bUC_HOST_MODE | bUC_INT_BUSY | bUC_DMA_EN;       // 启动USB主机及DMA,在中断标志未清除前自动暂停
//  UHUB0_CTRL = 0x00;
//  UHUB1_CTRL = 0x00;
    UH_SETUP = bUH_SOF_EN;
    USB_INT_FG = 0xFF;                                          // 清中断标志
    DisableRootHubPort(0);                                      // 清空
    DisableRootHubPort(1);                                      // 清空	
    USB_INT_EN = bUIE_TRANSFER | bUIE_DETECT;
//  IE_USB = 1;  // 查询方式
}
/*******************************************************************************
* Function Name  : mStopIfError
* Description    : 检查操作状态,如果错误则显示错误代码并停机
* Input          : UINT8 iError
* Output         : None
* Return         : None
*******************************************************************************/
void    mStopIfError( UINT8 iError )
{
    if ( iError == ERR_SUCCESS )
    {
        return;                                                // 操作成功
    }
    printf( "Error: %02X\n", (UINT16)iError );                 // 显示错误
    /* 遇到错误后,应该分析错误码以及CH559DiskStatus状态,例如调用CH559DiskReady查询当前U盘是否连接,如果U盘已断开那么就重新等待U盘插上再操作,
       建议出错后的处理步骤:
       1、调用一次CH559DiskReady,成功则继续操作,例如Open,Read/Write等
       2、如果CH559DiskReady不成功,那么强行将从头开始操作(等待U盘连接，CH559DiskReady等) */
    while ( 1 )
    {
//      LED_TMP=0;                                             // LED闪烁
//      mDelaymS( 100 );
//      LED_TMP=1;
//      mDelaymS( 100 );
    }
}
void main( )
{
    UINT8   s,i;
    UINT8   *pCodeStr;
    UINT16 j,loc;
    CfgFsys( );                                                //使能外部晶振
    mDelaymS(30);                                              //上电延时,等待内部晶振稳定,必加
    mInitSTDIO( );                                             //为了让计算机通过串口监控演示过程
    printf( "Start @ChipID=%02X\n", (UINT16)CHIP_ID );
    InitUSB_Host( );
    CH559LibInit( );                                           //初始化CH559程序库以支持U盘文件
    FoundNewDev = 0;
    printf( "Wait Device In\n" );
    while ( 1 )
    {
       s = ERR_SUCCESS;
        if ( UIF_DETECT )                                      //如果有USB主机检测中断则处理
        {
            UIF_DETECT = 0;                                    //清中断标志
            s = AnalyzeRootHub( );                             //分析ROOT-HUB状态
            if ( s == ERR_USB_CONNECT )
            {
                FoundNewDev = 1;
            }
        }
        if (FoundNewDev)  // 有新的USB设备插入
        {
            FoundNewDev = 0;
            mDelaymS( 200 );                                   // 由于USB设备刚插入尚未稳定,故等待USB设备数百毫秒,消除插拔抖动
			      s = EnumAllRootDevice( );                          // 枚举所有ROOT-HUB端口的USB设备					
            loc = SearchTypeDevice( USB_DEV_CLASS_STORAGE );   // 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号
				    if ( loc != 0xFFFF ) {  // 找到了					
								printf( "Start UDISK_demo @CH559UFI library\n" );
								// U盘操作流程：USB总线复位、U盘连接、获取设备描述符和设置USB地址、可选的获取配置描述符，之后到达此处，由CH559子程序库继续完成后续工作
								i = (UINT8)( loc >> 8 );
								SelectHubPort(i);                             // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址		
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
                if ( CH559DiskStatus >= DISK_MOUNTED )           //U盘准备好
                {
                    /* 读取原文件 */
                    printf( "Open\n" );
                    strcpy( mCmdParam.Open.mPathName, "/C51/CH559HFT.C" );//文件名,该文件在C51子目录下
                    s = CH559FileOpen( );                        //打开文件
                    /* 列出文件 */
                    if ( s == ERR_MISS_DIR )
                    {
                        printf("不存在该文件则列出所有文件\n");  //C51子目录不存在则列出根目录下的所有文件
                        pCodeStr = "/*";
                    }
                    else
                    {
                        pCodeStr = "/C51/*";                     //* CH559HFT.C文件不存在则列出\C51子目录下的以CH559开头的文件
                    }
                    printf( "List file %s\n", pCodeStr );
                    for ( j = 0; j < 10000; j ++ )               //最多搜索前10000个文件,实际上没有限制 
                    {
                        strcpy( mCmdParam.Open.mPathName, pCodeStr );//搜索文件名,*为通配符,适用于所有文件或者子目录
                        i = strlen( mCmdParam.Open.mPathName );
                        mCmdParam.Open.mPathName[ i ] = 0xFF;    //根据字符串长度将结束符替换为搜索的序号,从0到254,如果是0xFF即255则说明搜索序号在CH559vFileSize变量中 
                        CH559vFileSize = j;                      //指定搜索/枚举的序号
                        i = CH559FileOpen( );                    //打开文件,如果文件名中含有通配符*,则为搜索文件而不打开
                        /* CH559FileEnum 与 CH559FileOpen 的唯一区别是当后者返回ERR_FOUND_NAME时那么对应于前者返回ERR_SUCCESS */
                        if ( i == ERR_MISS_FILE )
                        {
                            break;                                //再也搜索不到匹配的文件,已经没有匹配的文件名
                        }
                        if ( i == ERR_FOUND_NAME )
                        {
                            /* 搜索到与通配符相匹配的文件名,文件名及其完整路径在命令缓冲区中 */
                            printf( "  match file %04d#: %s\n", (unsigned int)j, mCmdParam.Open.mPathName );//显示序号和搜索到的匹配文件名或者子目录名
                            continue;                             //继续搜索下一个匹配的文件名,下次搜索时序号会加1
                        }
                        else
                        {
                            /* 出错 */
                            mStopIfError( i );
                            break;
                        }
                    }
                    printf( "Close\n" );
                    i = CH559FileClose( );                         //关闭文件
                    printf( "U盘演示完成\n" );
                }
                else
                {
                    printf( "U盘没有准备好 ERR =%02X\n", (UINT16)s );
                }
            }
            else
            {
                printf("初始化U盘失败，请拔下U盘重试\n");
            }
        }
        mDelaymS( 100 );                                          // 模拟单片机做其它事
        SetUsbSpeed( 1 );                                         // 默认为全速
    }
}
