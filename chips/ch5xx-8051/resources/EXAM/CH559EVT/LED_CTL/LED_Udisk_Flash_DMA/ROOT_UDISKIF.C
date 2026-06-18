/********************************** (C) COPYRIGHT *******************************
* File Name          : ROOT_UDISKIF.C
* Author             : WCH
* Version            : V1.1
* Date               : 2015/10/13
* Description        : CH559 U盘读写 Interface                                  				   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "../../CH559.H"
//还需要添加LIB库文件
#include "../../USB_LIB/CH559UFI.H"
#include "../../USB_LIB/CH559UFI.C"

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
UINT8X  RxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0000 ;  // IN, must even address
UINT8X  TxBuffer[ MAX_PACKET_SIZE ] _at_ 0x0040 ;  // OUT, must even address


/*******************************************************************************
* Function Name  : DisableRootHubPort( void )
* Description    : 关闭Roothub端口
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void   DisableRootHubPort( void )      // 关闭端口,实际上硬件已经自动关闭,此处只是清除一些结构状态
{
    CH559DiskStatus = DISK_DISCONNECT;
    UHUB0_CTRL = 0x00;                 // 清除有关HUB0的控制数据,实际上不需要清除
}

/*******************************************************************************
* Function Name  : AnalyzeRootHub( void )
* Description    : 分析端口状态
* Input          : None
* Output         : None
* Return         : UINT8 s
*******************************************************************************/
UINT8   AnalyzeRootHub( void )         // 分析端口状态,处理端口的设备插拔事件
// 返回ERR_SUCCESS为没有情况,返回ERR_USB_CONNECT为检测到新连接,返回ERR_USB_DISCON为检测到断开
{
//处理端口的插拔事件,如果设备拔出,函数中调用DisableRootHubPort()函数,将端口关闭,插入事件,置相应端口的状态位
    UINT8   s;
    s = ERR_SUCCESS;
    if ( USB_HUB_ST & bUHS_H0_ATTACH ) // 设备存在
    {
        if ( CH559DiskStatus == DISK_DISCONNECT || ( UHUB0_CTRL & bUH_PORT_EN ) == 0x00 )    // 检测到有设备插入,但尚未允许,说明是刚插入
        {
            DisableRootHubPort( );     // 关闭端口
            CH559DiskStatus = DISK_CONNECT;
            s = ERR_USB_CONNECT;
        }
    }
    else if ( CH559DiskStatus >= DISK_CONNECT )
    {
        DisableRootHubPort( );         // 关闭端口
        if ( s == ERR_SUCCESS )
        {
            s = ERR_USB_DISCON;
        }
    }
    return( s );
}

/*******************************************************************************
* Function Name  : SetHostUsbAddr( UINT8 addr )
* Description    : 设置USB主机当前操作的USB设备地址
* Input          : UINT8 addr
* Output         : None
* Return         : None
*******************************************************************************/
void   SetHostUsbAddr( UINT8 addr )  
{
    USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | addr & 0x7F;
}

/*******************************************************************************
* Function Name  : ResetRootHubPort( void )
* Description    : 检测到设备后,复位总线,为枚举设备准备,设置为默认为全速
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void   ResetRootHubPort( void )        
{
    UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;  /* USB设备的端点0的最大包尺寸 */
    SetHostUsbAddr( 0x00 );
    SetUsbSpeed( 1 );                   // 默认为全速
    UHUB0_CTRL = UHUB0_CTRL & ~ bUH_LOW_SPEED | bUH_BUS_RESET;  // 默认为全速,开始复位
    mDelaymS( 15 );                     // 复位时间10mS到20mS
    UHUB0_CTRL = UHUB0_CTRL & ~ bUH_BUS_RESET;  // 结束复位
    mDelayuS( 250 );
    UIF_DETECT = 0;                     // 清中断标志
}

/*******************************************************************************
* Function Name  : EnableRootHubPort( void )
* Description    : 使能端口,相应的bUH_PORT_EN置1开启端口,设备断开可能导致返回失败
* Input          : None
* Output         : None
* Return         : ERR_USB_DISCON
*******************************************************************************/
UINT8   EnableRootHubPort( void )  
{
    if ( CH559DiskStatus < DISK_CONNECT )
    {
        CH559DiskStatus = DISK_CONNECT;
    }
    if ( USB_HUB_ST & bUHS_H0_ATTACH )    // 有设备
    {
        UHUB0_CTRL |= bUH_PORT_EN;  //使能HUB端口
        return( ERR_SUCCESS );
    }
    return( ERR_USB_DISCON );
}

/*******************************************************************************
* Function Name  : WaitUSB_Interrupt( void )
* Description    : 等待USB中断
* Input          : None
* Output         : None
* Return         : UIF_TRANSFER ? ERR_SUCCESS : ERR_USB_UNKNOWN
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
* Function Name  : USBHostTransact( UINT8 endp_pid, UINT8 tog, UINT16 timeout )
* Description    : CH559传输事务,输入目的端点地址/PID令牌,同步标志,以20uS为单位的NAK重试总时间(0则不重试,0xFFFF无限重试),返回0成功,超时/出错重试
* Input          : UINT8 endp_pid, UINT8 tog, UINT16 timeout
* Output         : None
* Return         : some
*******************************************************************************/ 
UINT8   USBHostTransact( UINT8 endp_pid, UINT8 tog, UINT16 timeout )  // endp_pid: 高4位是token_pid令牌, 低4位是端点地址
{
 // 本子程序着重于易理解,而在实际应用中,为了提供运行速度,应该对本子程序代码进行优化
    #define TransRetry  UEP0_T_LEN  // 节约内存
    UINT8   s, r;
    UINT16  i;
    UH_RX_CTRL = UH_TX_CTRL = tog;
    TransRetry = 0;
    do
    {
        UH_EP_PID = endp_pid;  // 指定令牌PID和目的端点号
        UIF_TRANSFER = 0;  // 允许传输
        for ( i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i -- )
        {
            ;
        }
        UH_EP_PID = 0x00;  // 停止USB传输
        if ( UIF_TRANSFER == 0 )
        {
            return( ERR_USB_UNKNOWN );
        }
        if ( UIF_DETECT )    // USB设备插拔事件
        {
            UIF_DETECT = 0;  // 清中断标志
            s = AnalyzeRootHub( );   // 分析ROOT-HUB状态
            if ( s == ERR_USB_CONNECT )
            {
                FoundNewDev = 1;
            }
            if ( CH559DiskStatus == DISK_DISCONNECT )
            {
                return( ERR_USB_DISCON );    // USB设备断开事件
            }
            if ( CH559DiskStatus == DISK_CONNECT )
            {
                return( ERR_USB_CONNECT );    // USB设备连接事件
            }
            mDelayuS( 200 );  // 等待传输完成
        }
        if ( UIF_TRANSFER )    // 传输完成
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
            r = USB_INT_ST & MASK_UIS_H_RES;  // USB设备应答状态
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
                    if ( r )
                    {
                        return( r | ERR_USB_TRANSFER );    // 不是超时/出错,意外应答
                    }
                    break;  // 超时重试
                case USB_PID_IN:
                    if ( r == USB_PID_DATA0 && r == USB_PID_DATA1 )    // 不同步则需丢弃后重试
                    {
                    }  // 不同步重试
                    else if ( r )
                    {
                        return( r | ERR_USB_TRANSFER );    // 不是超时/出错,意外应答
                    }
                    break;  // 超时重试
                default:
                    return( ERR_USB_UNKNOWN );  // 不可能的情况
                    break;
                }
        }
        else    // 其它中断,不应该发生的情况
        {
            USB_INT_FG = 0xFF;  /* 清中断标志 */
        }
        mDelayuS( 15 );
    }
    while ( ++ TransRetry < 3 );
    return( ERR_USB_TRANSFER );  // 应答超时
}

/*******************************************************************************
* Function Name  : HostCtrlTransfer( PUINT8X DataBuf, PUINT8I RetLen )
* Description    : 执行控制传输,8字节请求码在pSetupReq中,DataBuf为可选的收发缓冲区
* Input          : PUINT8X DataBuf, PUINT8I RetLen
* Output         : None
* Return         : some
*******************************************************************************/
UINT8   HostCtrlTransfer( PUINT8X DataBuf, PUINT8I RetLen )  
// 如果需要接收和发送数据,那么DataBuf需指向有效缓冲区用于存放后续数据,实际成功收发的总长度保存在ReqLen指向的字节变量中
{
    UINT8   s, RemLen, RxLen, RxCnt, TxCnt;
    PUINT8X xdata   pBuf;
    PUINT8I xdata   pLen;
    pBuf = DataBuf;
    pLen = RetLen;
    mDelayuS( 200 );
    if ( pLen )
    {
        *pLen = 0;    // 实际成功收发的总长度
    }
    UH_TX_LEN = sizeof( USB_SETUP_REQ );
    s = USBHostTransact( USB_PID_SETUP << 4 | 0x00, 0x00, 200000/20 );  // SETUP阶段,200mS超时
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    UH_RX_CTRL = UH_TX_CTRL = bUH_R_TOG | bUH_R_AUTO_TOG | bUH_T_TOG | bUH_T_AUTO_TOG;  // 默认DATA1
    UH_TX_LEN = 0x01;  // 默认无数据故状态阶段为IN
    RemLen = pSetupReq -> wLengthH ? 0xFF : pSetupReq -> wLengthL;
    if ( RemLen && pBuf )    // 需要收发数据
    {
        if ( pSetupReq -> bRequestType & USB_REQ_TYP_IN )    // 收
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
                    *pLen += RxLen;    // 实际成功收发的总长度
                }
                for ( RxCnt = 0; RxCnt != RxLen; RxCnt ++ )
                {
                    *pBuf = RxBuffer[ RxCnt ];
                    pBuf ++;
                }
                if ( USB_RX_LEN == 0 || ( USB_RX_LEN & ( UsbDevEndp0Size - 1 ) ) )
                {
                    break;    // 短包
                }
            }
            UH_TX_LEN = 0x00;  // 状态阶段为OUT
        }
        else    // 发
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
                s = USBHostTransact( USB_PID_OUT << 4 | 0x00, UH_TX_CTRL, 200000/20 );  // OUT数据
                if ( s != ERR_SUCCESS )
                {
                    return( s );
                }
                RemLen -= UH_TX_LEN;
                if ( pLen )
                {
                    *pLen += UH_TX_LEN;    // 实际成功收发的总长度
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
        return( ERR_SUCCESS );    // 状态OUT
    }
    if ( USB_RX_LEN == 0 )
    {
        return( ERR_SUCCESS );    // 状态IN,检查IN状态返回数据长度
    }
    return( ERR_USB_BUF_OVER );  // IN状态阶段错误
}

/*******************************************************************************
* Function Name  : CopySetupReqPkg( PUINT8C pReqPkt )
* Description    : 复制控制传输的请求包
* Input          : PUINT8C pReqPkt
* Output         : None
* Return         : None
*******************************************************************************/
void   CopySetupReqPkg( PUINT8C pReqPkt )  
{
    UINT8   i;
    for ( i = 0; i != sizeof( USB_SETUP_REQ ); i ++ )
    {
        ((PUINT8X)pSetupReq)[ i ] = *pReqPkt;
        pReqPkt ++;
    }
}

/*******************************************************************************
* Function Name  : CtrlGetDeviceDescr( void )
* Description    : 获取设备描述符,返回在TxBuffer中
* Input          : None
* Output         : None
* Return         : some
*******************************************************************************/
UINT8   CtrlGetDeviceDescr( void )  
{
    UINT8   s;
    UINT8D  len;
    UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;
    CopySetupReqPkg( SetupGetDevDescr );
    s = HostCtrlTransfer( TxBuffer, &len );  // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    UsbDevEndp0Size = ( (PXUSB_DEV_DESCR)TxBuffer ) -> bMaxPacketSize0;  // 端点0最大包长度,这是简化处理,正常应该先获取前8字节后立即更新UsbDevEndp0Size再继续
    if ( len < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL )
    {
        return( ERR_USB_BUF_OVER );    // 描述符长度错误
    }
    return( ERR_SUCCESS );
}

/*******************************************************************************
* Function Name  : CtrlGetConfigDescr( void )
* Description    : 获取配置描述符,返回在TxBuffer中
* Input          : None
* Output         : None
* Return         : some
*******************************************************************************/
UINT8   CtrlGetConfigDescr( void ) 
{
    UINT8   s;
    UINT8D  len;
    CopySetupReqPkg( SetupGetCfgDescr );
    s = HostCtrlTransfer( TxBuffer, &len );  // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < ( (PUSB_SETUP_REQ)SetupGetCfgDescr ) -> wLengthL )
    {
        return( ERR_USB_BUF_OVER );    // 返回长度错误
    }
    len = ( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthL;
    if ( len > MAX_PACKET_SIZE )
    {
        return( ERR_USB_BUF_OVER );    // 返回长度错误
    }
    CopySetupReqPkg( SetupGetCfgDescr );
    pSetupReq -> wLengthL = len;  // 完整配置描述符的总长度
    s = HostCtrlTransfer( TxBuffer, &len );  // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    if ( len < ( (PUSB_SETUP_REQ)SetupGetCfgDescr ) -> wLengthL || len < ( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthL )
    {
        return( ERR_USB_BUF_OVER );    // 描述符长度错误
    }
    return( ERR_SUCCESS );
}

/*******************************************************************************
* Function Name  : CtrlSetUsbAddress( UINT8 addr )
* Description    : 设置USB设备地址
* Input          : UINT8 addr
* Output         : None
* Return         : some
*******************************************************************************/
UINT8   CtrlSetUsbAddress( UINT8 addr )  // 设置USB设备地址
{
    UINT8   s;
    CopySetupReqPkg( SetupSetUsbAddr );
    pSetupReq -> wValueL = addr;  // USB设备地址
    s = HostCtrlTransfer( NULL, NULL );  // 执行控制传输
    if ( s != ERR_SUCCESS )
    {
        return( s );
    }
    SetHostUsbAddr( addr );  // 设置USB主机当前操作的USB设备地址
    mDelaymS( 10 );  // 等待USB设备完成操作
    return( ERR_SUCCESS );
}

/*******************************************************************************
* Function Name  : CtrlSetUsbConfig( UINT8 cfg )
* Description    : 设置USB设备配置
* Input          : UINT8 cfg
* Output         : None
* Return         : HostCtrlTransfer( NULL, NULL )
*******************************************************************************/
UINT8   CtrlSetUsbConfig( UINT8 cfg )  // 设置USB设备配置
{
    CopySetupReqPkg( SetupSetUsbConfig );
    pSetupReq -> wValueL = cfg;  // USB设备配置
    return( HostCtrlTransfer( NULL, NULL ) );  // 执行控制传输
}

/*******************************************************************************
* Function Name  : CtrlClearEndpStall( UINT8 endp )
* Description    : 清除端点STALL
* Input          : UINT8 endp
* Output         : None
* Return         : HostCtrlTransfer( NULL, NULL )
*******************************************************************************/
UINT8   CtrlClearEndpStall( UINT8 endp )  // 清除端点STALL
{
    CopySetupReqPkg( SetupClrEndpStall );  // 清除端点的错误
    pSetupReq -> wIndexL = endp;  // 端点地址
    return( HostCtrlTransfer( NULL, NULL ) );  /* 执行控制传输 */
}

/*******************************************************************************
* Function Name  : AnalyzeHidIntEndp( PUINT8X buf )
* Description    : 从描述符中分析出HID中断端点的地址
* Input          : PUINT8X buf
* Output         : None
* Return         : UINT8 s
*******************************************************************************/
UINT8   AnalyzeHidIntEndp( PUINT8X buf )  // 从描述符中分析出HID中断端点的地址
{
    UINT8   i, s, l;
    s = 0;
    for ( i = 0; i < ( (PXUSB_CFG_DESCR)buf ) -> wTotalLengthL; i += l )    // 搜索中断端点描述符,跳过配置描述符和接口描述符
    {
        if ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bDescriptorType == USB_DESCR_TYP_ENDP  // 是端点描述符
                && ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bmAttributes & USB_ENDP_TYPE_MASK ) == USB_ENDP_TYPE_INTER  // 是中断端点
                && ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_DIR_MASK ) )    // 是IN端点
        {
            s = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;  // 中断端点的地址
            break;  // 可以根据需要保存wMaxPacketSize和bInterval
        }
        l = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bLength;  // 当前描述符长度,跳过
        if ( l > 16 )
        {
            break;
        }
    }
    return( s );
}

/*******************************************************************************
* Function Name  : InitRootDevice( void )
* Description    : 初始化USB设备
* Input          : None
* Output         : None
* Return         : some
*******************************************************************************/
UINT8   InitRootDevice( void )  // 初始化USB设备
{
    UINT8   i, s, cfg, dv_cls, if_cls;
    ResetRootHubPort( );  // 检测到设备后,复位相应端口的USB总线
    for ( i = 0, s = 0; i < 100; i ++ )    // 等待USB设备复位后重新连接,100mS超时
    {
        mDelaymS( 1 );
        if ( EnableRootHubPort( ) == ERR_SUCCESS )    // 使能端口
        {
            i = 0;
            s ++;  // 计时等待USB设备连接后稳定
            if ( s > 100 )
            {
                break;    // 已经稳定连接100mS
            }
        }
    }
    if ( i )    // 复位后设备没有连接
    {
        DisableRootHubPort( );
        return( ERR_USB_DISCON );
    }
    SetUsbSpeed( ThisUsbDev.DeviceSpeed );  // 设置当前USB速度
    s = CtrlGetDeviceDescr( );  // 获取设备描述符
    if ( s == ERR_SUCCESS )
    {
        for ( i = 0; i < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL; i ++ )
        {
           ;
        }
        dv_cls = ( (PXUSB_DEV_DESCR)TxBuffer ) -> bDeviceClass;  // 设备类代码
        s = CtrlSetUsbAddress( ( (PUSB_SETUP_REQ)SetupSetUsbAddr ) -> wValueL );  // 设置USB设备地址
        if ( s == ERR_SUCCESS )
        {
            s = CtrlGetConfigDescr( );  // 获取配置描述符
            if ( s == ERR_SUCCESS )
            {
                cfg = ( (PXUSB_CFG_DESCR)TxBuffer ) -> bConfigurationValue;
                for ( i = 0; i < ( (PXUSB_CFG_DESCR)TxBuffer ) -> wTotalLengthL; i ++ )
                {
                  ; 
                }
                /* 分析配置描述符,获取端点数据/各端点地址/各端点大小等,更新变量endp_addr和endp_size等 */
                if_cls = ( (PXUSB_CFG_DESCR_LONG)TxBuffer ) -> itf_descr.bInterfaceClass;  // 接口类代码
                if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE )    // 是USB存储类设备,基本上确认是U盘
                {
                    CH559DiskStatus = DISK_USB_ADDR;
                    return( ERR_SUCCESS );
                }
                else
                {
                    return( ERR_USB_UNSUPPORT );
                }
            }
        }
    }
    CH559DiskStatus = DISK_CONNECT;
    SetUsbSpeed( 1 );  // 默认为全速
    return( s );
}

/*******************************************************************************
* Function Name  : InitUSB_Host( void )
* Description    : 初始化USB主机
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void    InitUSB_Host( void )  // 初始化USB主机
{
    IE_USB = 0;
    USB_CTRL = bUC_HOST_MODE;  // 先设定模式
    USB_DEV_AD = 0x00;
    UH_EP_MOD = bUH_EP_TX_EN | bUH_EP_RX_EN ;
    UH_RX_DMA = RxBuffer;
    UH_TX_DMA = TxBuffer;
    UH_RX_CTRL = 0x00;
    UH_TX_CTRL = 0x00;
    USB_CTRL = bUC_HOST_MODE | bUC_INT_BUSY | bUC_DMA_EN;  // 启动USB主机及DMA,在中断标志未清除前自动暂停
    UH_SETUP = bUH_SOF_EN;
    USB_INT_FG |= 0xFF;  // 清中断标志
    DisableRootHubPort( );  // 清空
    USB_INT_EN = bUIE_TRANSFER | bUIE_DETECT;
}

/*******************************************************************************
* Function Name  : mStopIfError( UINT8 iError )
* Description    : 检查操作状态,如果错误则显示错误代码并停机
* Input          : UINT8 iError
* Output         : None
* Return         : None
*******************************************************************************/
void    mStopIfError( UINT8 iError )
{
    if ( iError == ERR_SUCCESS )
    {
        return;    /* 操作成功 */
    }
    printf( "Error: %02X\n", (UINT16)iError );  /* 显示错误 */
    /* 遇到错误后,应该分析错误码以及CH559DiskStatus状态,例如调用CH559DiskReady查询当前U盘是否连接,如果U盘已断开那么就重新等待U盘插上再操作,
       建议出错后的处理步骤:
       1、调用一次CH559DiskReady,成功则继续操作,例如Open,Read/Write等
       2、如果CH559DiskReady不成功,那么强行将从头开始操作(等待U盘连接，CH559DiskReady等) */
    while ( 1 )
    {
//      LED_TMP=0;  /* LED闪烁 */
//      mDelaymS( 100 );
//      LED_TMP=1;
//      mDelaymS( 100 );
    }
}