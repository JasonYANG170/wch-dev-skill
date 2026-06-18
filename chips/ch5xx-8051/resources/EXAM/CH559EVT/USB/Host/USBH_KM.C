/********************************** (C) COPYRIGHT *******************************
* File Name          : USBH_KM.C
* Author             : WCH
* Version            : V1.5
* Date               : 2018/08/01
* Description        :
 USB host example for CH559, start USB device under DP/DM and HP/HM port
 USB主机应用例子,初始化和枚举USB端口连接的设备,同时支持最多2个USB设备,支持一级外部HUB,
 可以操作USB键鼠和HUB,包含HID类命令处理
 不支持U盘操作，如果需要操作U盘，请参考其他例子
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
#define ERR_USB_OVER_IF     0x30    /* 设备接口数超过4个 */
#define ERR_USB_UNSUPPORT   0xFB    /*不支持的USB设备*/
#define ERR_USB_UNKNOWN     0xFE    /*设备操作出错*/
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
/*获取HID设备报表描述符*/
UINT8C  SetupGetHIDDevReport[] = { 0x81, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_REPORT, 0x00, 0x00, 0xFF, 0xFF };
/* set usb device report*/
UINT8C  SetupSetReport[] = {USB_DESCR_TYP_HID,0x09,0x00,0x02,0x00,0x00,0x01,0x00}; 
UINT8X  UsbDevEndp0Size;              /* USB设备的端点0的最大包尺寸 */
/*USB设备相关信息表,CH559最多支持2个设备*/
#define ROOT_DEV_DISCONNECT     0
#define ROOT_DEV_CONNECTED      1
#define ROOT_DEV_FAILED         2
#define ROOT_DEV_SUCCESS        3
#define DEV_TYPE_KEYBOARD   ( USB_DEV_CLASS_HID | 0x20 )
#define DEV_TYPE_MOUSE      ( USB_DEV_CLASS_HID | 0x30 )
UINT8 ReportData = 0;
#define  HUB_INTERFACE_COUNT        4
#define  GLOB_USAGE_PAGE_DEF  		0X05	  // Unsigned integer specifying the current Usage Page.
#define  GLOB_USAGE_PAGE_DESKTOP    0X01	  // generic desktop
#define  LOCAL_USAGE_DEF	      	0X09	  // Usage index for an item usage; represents a suggested usage for the item or collection. 
#define  LOCAL_USAGE_MOUSE     	    0X02	  // MOUSE
#define  LOCAL_USAGE_KEY     		0X06	  // KEY

#define  DEBUG 1

struct _RootHubDev
{
    UINT8   DeviceStatus;           // 设备状态,0-无设备,1-有设备但尚未初始化,2-有设备但初始化枚举失败,3-有设备且初始化枚举成功
    UINT8   DeviceAddress;          // 设备被分配的USB地址
    UINT8   DeviceSpeed;            // 0为低速,非0为全速
    UINT8   DeviceType;             // 设备类型
    UINT8   InfCnt;                 // 设备接口数
    struct interface{
        UINT16  DescrLen;          // hid descripor len
        UINT8   InAddr;            // in endpoint address
        UINT8   OutAddr;           // in endpoint address
    	UINT8   DeviceType;        // device type 
        UINT8   WaitUSB_IN_Interval;      
    }Interface[HUB_INTERFACE_COUNT];	
} xdata RootHubDev[2];
UINT8X  RxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0000 ;  // IN, must even address
UINT8X  TxBuffer[ MAX_PACKET_SIZE*8 ] _at_ 0x0040 ;  // OUT, must even address
#define pSetupReq   ((PXUSB_SETUP_REQ)TxBuffer)
bit     RootHubId;                 // 当前正在操作的root-hub端口号:0=HUB0,1=HUB1
bit     FoundNewDev;                   

UINT16X  T0Count[2][HUB_INTERFACE_COUNT];    //T0计数，单位ms
#pragma NOAREGS
#define WAIT_USB_TOUT_200US     200  // 等待USB中断超时时间200uS@Fsys=12MHz
#define WAIT_USB_IN_COUNT     0x2EE0  // 等待USB中断超时时间200uS@Fsys=12MHz

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
UINT8   AnalyzeHidIntEndp( PUINT8X buf );              // 从描述符中分析出HID中断端点的地址
UINT8   InitRootDevice( UINT8 RootHubIndex );          // 初始化指定ROOT-HUB端口的USB设备
UINT16  SearchTypeDevice( UINT8 type );                // 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号,输出端口号为0xFFFF则未搜索到
// 输出高8位为ROOT-HUB端口号,低8位为外部HUB的端口号,低8位为0则设备直接在ROOT-HUB端口上
void    mInitSTDIO( void );                            // 为printf和getkey输入输出初始化串口
void    InitUSB_Host( void );                          // 初始化USB主机
//#define   FREQ_SYS    12000000                       // 系统主频12MHz

#define T0_START 1
#define T0_STOP 0

#define mTimer0ClkFsys( ) (T2MOD |= bTMR_CLK | bT0_CLK)                     //定时器,时钟=Fsys
//CH559 Timer0 开始(SS=1)/结束(SS=0)
#define mTimer0RunCTL( SS ) (TR0 = SS ? T0_START : T0_STOP)
/*******************************************************************************
* Function Name  : mTimer0ModSetup(UINT8 mode)
* Description    : CH559定时计数器0模式0设置
* Input          : UINT8 mode,Timer0模式选择
                   0：模式0，13位定时器，TL0的高3位无效
                   1：模式1，16位定时器
                   2：模式2，8位自动重装定时器
                   3：模式3，两个8位定时器
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer0ModSetup(UINT8 mode)
{
    TMOD &= 0xf0;
    TMOD |= mode; 
}

/*******************************************************************************
* Function Name  : mTimer0SetData(UINT16 dat)
* Description    : CH559Timer0 TH0和TL0赋值
* Input          : UINT16 dat;定时器赋值
* Output         : None
* Return         : None
*******************************************************************************/
void mTimer0SetData(UINT16 dat)
{
    UINT16 tmp;
    tmp = 65536 - dat;	
    TL0 = tmp & 0xff;
    TH0 = (tmp>>8) & 0xff;
}

/*******************************************************************************
* Function Name  : mTimer0Interrupt()
* Description    : CH559定时计数器0定时计数器中断处理函数
*******************************************************************************/
void	mTimer0Interrupt( void ) //interrupt INT_NO_TMR0 using 1                //timer0中断服务程序,使用寄存器组1
{                                                                             //方式3时，TH0使用Timer1的中断资源
    UINT8 i;
    if(TF0)
    {					
      CAP1 = ~CAP1;			
      mTimer0SetData(WAIT_USB_IN_COUNT);                                     //非自动重载方式需重新给TH0和TL0赋值  
      for(i=0;i<HUB_INTERFACE_COUNT;i++)			
      {
        T0Count[0][i]++;				
        T0Count[1][i]++;	
      }
      TF0 = 0;			
    }
}

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
void  SelectHubPort( UINT8 RootHubIndex, UINT8 HubPortIndex )
{
    UINT8 i;
    i = HubPortIndex;
    SetHostUsbAddr( RootHubDev[RootHubIndex].DeviceAddress );// 设置USB主机当前操作的USB设备地址
    SetUsbSpeed( RootHubDev[RootHubIndex].DeviceSpeed );     // 设置当前USB速度
    RootHubId = RootHubIndex ? 1 : 0;
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
    UINT8   r;
    UINT16  i;
    UH_RX_CTRL = tog;
    UH_TX_CTRL =tog ;
    TransRetry = 0;
    do
    {
        UH_EP_PID = endp_pid;                                    // 指定令牌PID和目的端点号
        UIF_TRANSFER = 0;                                        // 允许传输
        for ( i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i -- )
        {
            ;
        }
        UH_EP_PID = 0x00;                                        // 停止USB传输
        if ( UIF_TRANSFER == 0 )
        {
            return( ERR_USB_UNKNOWN );
        }
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
UINT8   HostCtrlTransfer( PUINT8 pBuf, PUINT16 pLen )
{
    UINT8    s;
    UINT16   RemLen, RxLen, RxCnt, TxCnt;

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
    UINT16   len;
    UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;
    CopySetupReqPkg( SetupGetDevDescr );
    s = HostCtrlTransfer( TxBuffer, &len );                    // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    UsbDevEndp0Size = ( (PXUSB_DEV_DESCR)TxBuffer ) -> bMaxPacketSize0;// 端点0最大包长度,这是简化处理,正常应该先获取前8字节后立即更新UsbDevEndp0Size再继续
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
    s = HostCtrlTransfer( TxBuffer, &len );                     // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < ( (PUSB_SETUP_REQ)SetupGetCfgDescr ) -> wLengthL )
    {
        return( ERR_USB_BUF_OVER );                             // 返回长度错误
    }
    len = ((UINT16)( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthH<<8)|( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthL;
    CopySetupReqPkg( SetupGetCfgDescr );
    pSetupReq -> wLengthL = (UINT8)len;                                // 完整配置描述符的总长度
    pSetupReq -> wLengthH = (UINT8)(len>>8);
	s = HostCtrlTransfer( TxBuffer, &len );                     // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < ( (PUSB_SETUP_REQ)SetupGetCfgDescr ) -> wLengthL || len < ( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthL )
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
* Description    : 获取HID报表描述符
* Input          : UINT8 Len  报表描述符长度
                   UINT8 IfNum 接口编号
* Output         : None
* Return         : ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8	CtrlGetHIDDeviceReport(UINT16 Len,UINT8 IfNum)
{
    CopySetupReqPkg( SetupGetHIDDevReport );                      // 获取接口报表
    ((PUINT8X)pSetupReq)[ 6 ] = (UINT8)Len;                       //长度+0x40
    ((PUINT8X)pSetupReq)[ 7 ] = (UINT8)(Len >>8);                  //
    ((PUINT8X)pSetupReq)[ 4 ] = IfNum;                            //
	return( HostCtrlTransfer( TxBuffer, NULL ) );                   // 执行控制传输
}
/*******************************************************************************
* Function Name  : KM_HostSetReport
* Description    : 设置USB设备配置
* Input          : UINT8 cfg       配置值
* Output         : None
* Return         :
*******************************************************************************/
void	KM_HostSetReport( UINT8 cfg )  // 设置USB设备配置
{
	CopySetupReqPkg( SetupSetReport );
	HostCtrlTransfer( &cfg, NULL );  // 执行控制传输
}

UINT8 KM_HostAnalyseMouseHid( UINT8 index, UINT8 num ) 
{
	if( ((UINT8 *)TxBuffer)[0] == GLOB_USAGE_PAGE_DEF && ((UINT8 *)TxBuffer)[1] == GLOB_USAGE_PAGE_DESKTOP\
	 && ((UINT8 *)TxBuffer)[2] == LOCAL_USAGE_DEF && ((UINT8 *)TxBuffer)[3] == LOCAL_USAGE_MOUSE ){
			RootHubDev[index].Interface[num].DeviceType = DEV_TYPE_MOUSE;
		  return ERR_SUCCESS;		 
		}
	  else if( ((UINT8 *)TxBuffer)[0] == GLOB_USAGE_PAGE_DEF && ((UINT8 *)TxBuffer)[1] == GLOB_USAGE_PAGE_DESKTOP\
	 && ((UINT8 *)TxBuffer)[2] == LOCAL_USAGE_DEF && ((UINT8 *)TxBuffer)[3] == LOCAL_USAGE_KEY ){
			RootHubDev[index].Interface[num].DeviceType = DEV_TYPE_KEYBOARD;
			return ERR_SUCCESS;
		}
	return ERR_USB_UNSUPPORT;
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
    UINT8   s, cfg, dv_cls, if_cls,ep;
    UINT8  retry=0;
	UINT16  i,j;
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
                printf( "x%02X ", (UINT16)( TxBuffer[i] ) );
            }
            printf( "\n" );                                            // 显示出描述符
            dv_cls = ( (PXUSB_DEV_DESCR)TxBuffer ) -> bDeviceClass;    // 设备类代码
            s = CtrlSetUsbAddress( RootHubIndex + ( (PUSB_SETUP_REQ)SetupSetUsbAddr ) -> wValueL );// 设置USB设备地址,加上RootHubIndex可以保证2个HUB端口分配不同的地址
            if ( s == ERR_SUCCESS )
            {
                RootHubDev[RootHubIndex].DeviceAddress = RootHubIndex + ( (PUSB_SETUP_REQ)SetupSetUsbAddr ) -> wValueL;  // 保存USB地址
                printf( "GetCfgDescr: " );
                s = CtrlGetConfigDescr( );                             // 获取配置描述符
				if ( s == ERR_SUCCESS ){
					RootHubDev[RootHubIndex].InfCnt = ((PXUSB_CFG_DESCR)TxBuffer)->bNumInterfaces;//接口数
					if( RootHubDev[RootHubIndex].InfCnt > 4 ) return( ERR_USB_OVER_IF );          //超出4个接口
					cfg = ( (PXUSB_CFG_DESCR)TxBuffer ) -> bConfigurationValue;
#ifdef DEBUG
					printf( "ifnum %02X ", (UINT16)RootHubDev[RootHubIndex].InfCnt );
					for ( i = 0; i < (((UINT16)( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthH<<8)|( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthL); i ++ ){
							printf( "x%02X ", (UINT16)( TxBuffer[i] ) );
					}
					printf("\n");
#endif
					//分析配置描述符,获取端点数据/各端点地址/各端点大小等,更新变量endp_addr和endp_size等
					if_cls = ( (PXUSB_CFG_DESCR_LONG)TxBuffer ) -> itf_descr.bInterfaceClass;  // 接口类代码 03
					if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_HID && ( (PXUSB_CFG_DESCR_LONG)TxBuffer ) -> itf_descr.bInterfaceSubClass <= 0x01 )// 是HID类设备,键盘/鼠标等
					{
						s = 0;									
						for(i=0;i<RootHubDev[RootHubIndex].InfCnt;i++){  
							if(TxBuffer[s+1] == 0x02){//配置描述符										
								s = s	+ TxBuffer[s];
							}										
							if(TxBuffer[s+1] == 0x04){//接口描述符	
								ep = TxBuffer[s+4];											
								s = s	+ TxBuffer[s];								
							}									
							if(TxBuffer[s+1] == 0x21){//HID描述符										
								s = s	+ TxBuffer[s];
								RootHubDev[RootHubIndex].Interface[i].DescrLen = TxBuffer[s-1];
								RootHubDev[RootHubIndex].Interface[i].DescrLen <<= 8;
								RootHubDev[RootHubIndex].Interface[i].DescrLen |= TxBuffer[s-2]; // 保存描述长度
							}			
							if(TxBuffer[s+1] == 0x05){//端点描述符		
								 while(ep--){										
									 if(TxBuffer[s+2]&0x80){//IN
										RootHubDev[RootHubIndex].Interface[i].InAddr = TxBuffer[s+2]&0x0f;	
										RootHubDev[RootHubIndex].Interface[i].WaitUSB_IN_Interval = TxBuffer[s+6];
										if((RootHubDev[RootHubIndex].DeviceSpeed == 0)&&(RootHubDev[RootHubIndex].Interface[i].WaitUSB_IN_Interval<8))
										{
										  RootHubDev[RootHubIndex].Interface[i].WaitUSB_IN_Interval	= 8;														
										}															
#ifdef DEBUG
										printf( "IN_Ep %02X \n", (UINT16)( RootHubDev[RootHubIndex].Interface[i].InAddr ) );
										printf( "IN_Val %02X \n", (UINT16)( RootHubDev[RootHubIndex].Interface[i].WaitUSB_IN_Interval ) );													 
#endif															 
										s = s	+ TxBuffer[s];													 
										continue;											 
									 }											 
									 if((TxBuffer[s+2]&0x80)==0){//OUT
										RootHubDev[RootHubIndex].Interface[i].OutAddr = TxBuffer[s+2]&0x0f;	
										s = s	+ TxBuffer[s];	
										continue;											  
									 }
								}										  
							}					
#ifdef DEBUG
							printf( "hidlen %02X ", (UINT16)( RootHubDev[RootHubIndex].Interface[i].DescrLen ) );
							printf("\n");
#endif											
						}
						s = CtrlSetUsbConfig( cfg );                   // 设置USB设备配置
						if ( s == ERR_SUCCESS )
						{
							for(i=0;i<RootHubDev[RootHubIndex].InfCnt;i++){
								CtrlSetUsbIDLE(i);													
								s = CtrlGetHIDDeviceReport(RootHubDev[RootHubIndex].Interface[i].DescrLen,i);              //获取报表描述符
#ifdef DEBUG	
								printf( "len003 %02X \n", (UINT16)(RootHubDev[RootHubIndex].Interface[i].DescrLen) );
								
								for ( j=0; j<(RootHubDev[RootHubIndex].Interface[i].DescrLen); j++ ){
									printf( "x%02X ", (UINT16)( TxBuffer[j] ) );
								}
								printf("\n");
								printf( "GetHIDReport: %02X ", (UINT16)s );
#endif
								if( s == ERR_SUCCESS ){
									s = KM_HostAnalyseMouseHid( RootHubIndex, i );// 需要分析
								}
								else return s;
							}
							if((RootHubDev[0].Interface[0].DeviceType == DEV_TYPE_KEYBOARD)&&(RootHubDev[1].Interface[0].DeviceType == DEV_TYPE_KEYBOARD)){//接入2个键盘
								KM_HostSetReport(ReportData);//同步LED										
							}	
							RootHubDev[RootHubIndex].DeviceStatus = ROOT_DEV_SUCCESS;												
							return( ERR_SUCCESS );                    
						}
					}
					else                                                                  //其他设备
					{
						s = CtrlSetUsbConfig( cfg );                   // 设置USB设备配置
						if ( s == ERR_SUCCESS )
						{					
							RootHubDev[RootHubIndex].DeviceStatus = ROOT_DEV_SUCCESS;
							RootHubDev[RootHubIndex].DeviceType = dv_cls?dv_cls:if_cls;
							return( ERR_SUCCESS ); 
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
* Description    : 初始化USB主机，查询方式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void    InitUSB_Host( void )
{
    UINT8   i;
    IE_USB = 0;
    USB_CTRL = bUC_HOST_MODE;                                                       // 先设定模式
    USB_DEV_AD = 0x00;
    UH_EP_MOD = bUH_EP_TX_EN | bUH_EP_RX_EN ;
    UH_RX_DMA = RxBuffer;
    UH_TX_DMA = TxBuffer;
    UH_RX_CTRL = 0x00;
    UH_TX_CTRL = 0x00;
    USB_CTRL = bUC_HOST_MODE | bUC_INT_BUSY | bUC_DMA_EN;                           // 启动USB主机及DMA,在中断标志未清除前自动暂停
    UH_SETUP = bUH_SOF_EN;
    USB_INT_FG = 0xFF;                                                              // 清中断标志
    for ( i = 0; i != 2; i ++ )
    {
        DisableRootHubPort( i );                                                    // 清空
    }
    USB_INT_EN = bUIE_TRANSFER | bUIE_DETECT;
}

/*******************************************************************************
* Function Name  : RootHUB_Detect_USB_Plug
* Description    : 检测ROOTHUB端口设备插拔
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RootHUB_Detect_USB_Plug( void )
{
	UINT8 s;
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
}
/*******************************************************************************
* Function Name  : RootHUB_USB_Dev_Enum
* Description    : 枚举ROOT HUB 下dev
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void RootHUB_USB_Dev_Enum( void )
{
	UINT8 s;
	s = ERR_SUCCESS;	
	if ( FoundNewDev )                                                          // 有新的USB设备插入
	{
		FoundNewDev = 0;
		mDelaymS( 200 );                                                        // 由于USB设备刚插入尚未稳定,故等待USB设备数百毫秒,消除插拔抖动
		s = EnumAllRootDevice( );                                               // 枚举所有ROOT-HUB端口的USB设备
		if ( s != ERR_SUCCESS )
		{
			printf( "EnumAllRootDev err = %02X\n", (UINT16)s );
		}
	}	
}

/*******************************************************************************
* Function Name  : RootHUB_Get_Mouse_Data
* Description    : 获取ROOT HUB下键鼠标数据
* Input          : 
                   UINT8 RootHubIndex
                   UINT8 Dev_Type
                   PUINT8 MPort0 HUB0口数据，PUINT8 MPort1 HUB1口数据
                   MPortn 为0代表没有数据
* Output         : None
* Return         : None 
*******************************************************************************/
void RootHUB_Get_Data( UINT8 RootHubIndex,UINT8 Dev_Type,PUINT8 MPort0, PUINT8 MPort1)
{
	UINT8 s,endp,i,flag;
	flag = 0;
	if((Dev_Type == DEV_TYPE_KEYBOARD)||(Dev_Type == DEV_TYPE_MOUSE))
	{
		for(i=0;i<RootHubDev[RootHubIndex].InfCnt;i++)
		{		
			if ( RootHubDev[RootHubIndex].Interface[i].DeviceType == Dev_Type && RootHubDev[RootHubIndex].DeviceStatus >= ROOT_DEV_SUCCESS  && T0Count[RootHubIndex][i] >= RootHubDev[RootHubIndex].Interface[i].WaitUSB_IN_Interval)
			{
				T0Count[RootHubIndex][i] = 0;                                              //清空计时				
				SelectHubPort( RootHubIndex, 0 );                                           // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
				endp = RootHubDev[RootHubIndex].Interface[i].InAddr;                        // 中断端点的地址,位7用于同步标志位
				if ( endp & USB_ENDP_ADDR_MASK )                                            // 端点有效
				{
					s = USBHostTransact( USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0 );// CH559传输事务,获取数据,NAK不重试
					if ( s == ERR_SUCCESS )
					{
						endp ^= 0x80;                                                           // 同步标志翻转
						RootHubDev[RootHubIndex].Interface[i].InAddr = endp;                                                  
						if ( USB_RX_LEN )                                                       // 接收到的数据长度
						{								
							if(RootHubIndex)
							{
								MPort1[0] = USB_RX_LEN;									
								memcpy(MPort1+1,RxBuffer,USB_RX_LEN);                               //HUB1口数据	
							}									
							else
							{
								MPort0[0] = USB_RX_LEN;									
								memcpy(MPort0+1,RxBuffer,USB_RX_LEN);                               //HUB0口数据
							}
							if(Dev_Type == DEV_TYPE_KEYBOARD)
							{									
											if(RxBuffer[2]==0x39){//Caps Lock		
												ReportData ^= 0x02;			
												KM_HostSetReport(ReportData);	
												flag = 0xAA;
											}
											if(RxBuffer[2]==0x53){//Num Lock		
												ReportData ^= 0x01;		
												KM_HostSetReport(ReportData);					
												flag = 0xAA;					
											}		
											if(RxBuffer[2]==0x47){//Scroll Lock		
												ReportData ^= 0x04;		
												KM_HostSetReport(ReportData);					
												flag = 0xAA;					
											}
											if((flag == 0xAA)&&(RootHubDev[0].Interface[0].DeviceType == DEV_TYPE_KEYBOARD)&&(RootHubDev[1].Interface[0].DeviceType == DEV_TYPE_KEYBOARD)){//接入2个键盘	
												s = RootHubIndex ^ 0x01;	
												SelectHubPort(s,0);	// switch hub num			
												KM_HostSetReport(ReportData);							
											}								
						   }							
						   return;                                                               //同一HUB端口复合多个键鼠只取1设备接口数据						
						}
					}			
				 }
			 }
		  }		
	}
}
/*******************************************************************************
* Function Name  : main
* Description    : main函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
main( )
{
    UINT8   i;
    UINT8   RootHub0[20];
    UINT8   RootHub1[20];	
    mInitSTDIO( );                                                                  //为了让计算机通过串口监控演示过程
    printf("CHIP_ID: %02x\n",(UINT16)ID_CH559);
	printf("Host_Keyboard_Mouse_V100:"__DATE__",Debug time:"__TIME__"\n");
    InitUSB_Host( );                                                                //初始化USB主机
    mTimer0ClkFsys( );                                                              //设置定时器0 1ms定时
    mTimer0ModSetup( 1 );
    mTimer0SetData( WAIT_USB_IN_COUNT );
    mTimer0RunCTL(T0_START);	
	
	
    FoundNewDev = 0;                                                                //变量初始化
    memset(RootHubDev,0,sizeof(RootHubDev));
    memset(T0Count,0,sizeof(T0Count));	
	
    while ( 1 )
    {
		RootHUB_Detect_USB_Plug( );                                              //检测设备插拔			
		RootHUB_USB_Dev_Enum( );                                                 //枚举ROOT下设备
		
		RootHubId = 0;					
		memset(RootHub0,0,sizeof(RootHub0));
		memset(RootHub1,0,sizeof(RootHub1));			
		RootHUB_Get_Data( RootHubId,DEV_TYPE_MOUSE,RootHub0,RootHub1);
		if(RootHub0[0])                                                         //HUB0 Mouse数据
		{
			printf("HUB0_Mouse data: ");
			for ( i = 1; i <= RootHub0[0]; i ++ )
			{
					printf("x%02X ",(UINT16)(RootHub0[i]) );
			}
			printf("\n");					
		}
		memset(RootHub0,0,sizeof(RootHub0));
		memset(RootHub1,0,sizeof(RootHub1));						
		RootHUB_Get_Data( RootHubId,DEV_TYPE_KEYBOARD,RootHub0,RootHub1);
		if(RootHub0[0])                                                         //HUB0 Keyboard数据
		{
			printf("HUB0_Keyboard data: ");
			for ( i = 1; i <= RootHub0[0]; i ++ )
			{
					printf("x%02X ",(UINT16)(RootHub0[i]) );
			}
			printf("\n");					
		}					
		RootHubId = 1;					
		memset(RootHub0,0,sizeof(RootHub0));
		memset(RootHub1,0,sizeof(RootHub1));			
		RootHUB_Get_Data( RootHubId,DEV_TYPE_MOUSE,RootHub0,RootHub1);			
		if(RootHub1[0])                                                         //HUB1 Mouse数据
		{
			printf("HUB1_Mouse data: ");
			for ( i = 1; i <= RootHub1[0]; i ++ )
			{
					printf("x%02X ",(UINT16)(RootHub1[i]) );
			}
			printf("\n");					
		}		
		memset(RootHub0,0,sizeof(RootHub0));
		memset(RootHub1,0,sizeof(RootHub1));						
		RootHUB_Get_Data( RootHubId,DEV_TYPE_KEYBOARD,RootHub0,RootHub1);			
		if(RootHub1[0])                                                         //HUB1 Keyboard数据
		{
			printf("HUB1_Keyboard data: ");
			for ( i = 1; i <= RootHub1[0]; i ++ )
			{
					printf("x%02X ",(UINT16)(RootHub1[i]) );
			}
			printf("\n");					
		}		
				
		SetUsbSpeed( 1 );                                                        // 默认为全速		
		mTimer0Interrupt( );                                                     // 1ms定时				
    }
}
