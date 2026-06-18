/********************************** (C) COPYRIGHT *******************************
* File Name          : USBHOST.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/03
* Description        : CH557 USB 主机接口函数
*       U_DISK例程需包含 CH557UFI.H
*       Host例程不需包含 CH557UFI.H
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "USBHOST.H"
//#include "CH557UFI.H"     //U盘例程使用，主机例程注释掉,不然HOST程序造成复位

UINT8X  Com_Buffer[ COM_BUF_SIZE ];   // 定义用户临时缓冲区,枚举时用于处理描述符,枚举结束也可以用作普通临时缓冲区
UINT8X	RxBuffer[ MAX_PACKET_SIZE ] _at_ 0x00;  // IN, must even address
UINT8X	TxBuffer[ MAX_PACKET_SIZE ] _at_ MAX_PACKET_SIZE;  // OUT, must even address

#define	pSetupReq	((PXUSB_SETUP_REQ)TxBuffer)
bit		FoundNewDev;
UINT8X	UsbDevEndp0Size;	/* USB设备的端点0的最大包尺寸 */
_RootHubDev   xdata ThisUsbDev;
_DevOnHubPort xdata DevOnHubPort[HUB_MAX_PORTS];
UINT8 Set_Port;
// 获取设备描述符
UINT8C	SetupGetDevDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_DEVICE, 0x00, 0x00, sizeof( USB_DEV_DESCR ), 0x00 };
// 获取配置描述符
UINT8C	SetupGetCfgDescr[] = { USB_REQ_TYP_IN, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_CONFIG, 0x00, 0x00, 0x04, 0x00 };
// 设置USB地址
UINT8C	SetupSetUsbAddr[] = { USB_REQ_TYP_OUT, USB_SET_ADDRESS, USB_DEVICE_ADDR, 0x00, 0x00, 0x00, 0x00, 0x00 };
// 设置USB配置
UINT8C	SetupSetUsbConfig[] = { USB_REQ_TYP_OUT, USB_SET_CONFIGURATION, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
/*设置USB接口配置*/
UINT8C  SetupSetUsbInterface[] = { USB_REQ_RECIP_INTERF, USB_SET_INTERFACE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// 清除端点STALL
UINT8C	SetupClrEndpStall[] = { USB_REQ_TYP_OUT | USB_REQ_RECIP_ENDP, USB_CLEAR_FEATURE, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
// 获取HUB描述符
UINT8C	SetupGetHubDescr[] = { HUB_GET_HUB_DESCRIPTOR, HUB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_HUB, 0x00, 0x00, sizeof( USB_HUB_DESCR ), 0x00 };
/*设置 HID空闲不发包 */
UINT8C  SetupSetHIDIdle[]= { 0x21,HID_SET_IDLE,0x00,0x00,0x00,0x00,0x00,0x00 };
/*获取HID设备报表描述符*/
UINT8C  SetupGetHIDDevReport[] = { 0x81, USB_GET_DESCRIPTOR, 0x00, USB_DESCR_TYP_REPORT, 0x00, 0x00, 0xFF, 0x00 };
/*打印机类命令*/
UINT8C  XPrinterReport[] = { 0xA1, 0, 0x00, 0, 0x00, 0x00, 0xF1, 0x03 };
//AOA获取协议版本
UINT8C  GetProtocol[] = { 0xc0,0x33,0x00,0x00,0x00,0x00,0x02,0x00 };
//启动配件模式
UINT8C  TouchAOAMode[] = { 0x40,0x35,0x00,0x00,0x00,0x00,0x00,0x00 };
/* AOA相关数组定义 */
UINT8C  Sendlen[] = {0,4,16,35,39,53,67};
//字符串ID,与手机APP相关的字符串信息
UINT8C  StringID[] = {
	'W','C','H',0x00,                                                                                   //manufacturer name
	'W','C','H','U','A','R','T','D','e','m','o',0x00,                                                   //model name
	0x57,0x43,0x48,0x20,0x41,0x63,0x63,0x65,0x73,0x73,0x6f,0x72,0x79,0x20,0x54,0x65,0x73,0x74,0x00,     //description
	'1','.','0',0x00 ,                                                                                  //version
	0x68,0x74,0x74,0x70,0x3a,0x2f,0x2f,0x77,0x63,0x68,0x2e,0x63,0x6e,0,                                 //URI
	0x57,0x43,0x48,0x41,0x63,0x63,0x65,0x73,0x73,0x6f,0x72,0x79,0x31,0x00                               //serial number
};

//应用索引字符串命令
UINT8C  SetStringID[] = {
	0x40,0x34,0x00,0x00,0x00,0x00,0x04,0x00,
	0x40,0x34,0x00,0x00,0x01,0x00,12,0x00,
	0x40,0x34,0x00,0x00,0x02,0x00,19,0x00,
	0x40,0x34,0x00,0x00,0x03,0x00,4,0x00,
	0x40,0x34,0x00,0x00,0x04,0x00,0x0E,0x00,
	0x40,0x34,0x00,0x00,0x05,0x00,0x0E,0x00
};

/*******************************************************************************
* Function Name  : DisableRootHubPort
* Description    : 关闭HUB端口
* Input          : None
* Return         : None
*******************************************************************************/
void DisableRootHubPort(void)          
{
#ifdef	FOR_ROOT_UDISK_ONLY
	CH549DiskStatus = DISK_DISCONNECT;
#endif
	
#ifndef	DISK_BASE_BUF_LEN
	ThisUsbDev.DeviceStatus = ROOT_DEV_DISCONNECT;
  ThisUsbDev.DeviceAddress = 0x00;
#endif
}

/*******************************************************************************
* Function Name  : AnalyzeRootHub
* Description    : 分析ROOT-HUB状态,处理ROOT-HUB端口的设备插拔事件
                   如果设备拔出,函数中调用DisableRootHubPort()函数,将端口关闭,插入事件,置相应端口的状态位
* Input          : None
* Return         : 返回ERR_SUCCESS为没有情况,返回ERR_USB_CONNECT为检测到新连接,返回ERR_USB_DISCON为检测到断开
*******************************************************************************/
UINT8 AnalyzeRootHub(void)
{ 
	UINT8	s;
	
	s = ERR_SUCCESS;
	if(USB_MIS_ST & bUMS_DEV_ATTACH){                                        // 设备存在
#ifdef DISK_BASE_BUF_LEN
		if(CH549DiskStatus == DISK_DISCONNECT
#else
		if(ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT                        // 检测到有设备插入
#endif
			|| (UHOST_CTRL & bUH_PORT_EN) == 0x00){                              // 检测到有设备插入,但尚未允许,说明是刚插入
			DisableRootHubPort();                                                   // 关闭端口
#ifdef DISK_BASE_BUF_LEN
			CH549DiskStatus = DISK_CONNECT;
#else
//		ThisUsbDev.DeviceSpeed = USB_HUB_ST & bUHS_DM_LEVEL ? 0 : 1;
			ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED;                            //置连接标志
#endif
#ifdef DEBUG
			printf("USB dev in\n");
#endif
			s = ERR_USB_CONNECT;
		}
	}
#ifdef DISK_BASE_BUF_LEN
	else if(CH549DiskStatus >= DISK_CONNECT){
#else
	else if(ThisUsbDev.DeviceStatus >= ROOT_DEV_CONNECTED){                  //检测到设备拔出
#endif
		DisableRootHubPort();                                                     // 关闭端口
#ifdef DEBUG	
		printf("USB dev out\n");
#endif
		if (s == ERR_SUCCESS) s = ERR_USB_DISCON;
	}

	return(s);
}

/*******************************************************************************
* Function Name  : SetHostUsbAddr
* Description    : 设置USB主机当前操作的USB设备地址
* Input          : UINT8 addr
* Return         : None
*******************************************************************************/
void SetHostUsbAddr(UINT8 addr)
{
	USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | addr & 0x7F;
}

/*******************************************************************************
* Function Name  : SetUsbSpeed
* Description    : 设置当前USB速度
* Input          : UINT8 FullSpeed
* Return         : None
*******************************************************************************/
void SetUsbSpeed(UINT8 FullSpeed)  
{
	if (FullSpeed)                                                           // 全速
	{
		USB_CTRL &= ~ bUC_LOW_SPEED;                                           // 全速
		UH_SETUP &= ~ bUH_PRE_PID_EN;                                          // 禁止PRE PID
	}
	else
	{
		USB_CTRL |= bUC_LOW_SPEED;                                             // 低速		
	}
}

/*******************************************************************************
* Function Name  : ResetRootHubPort
* Description    : 检测到设备后,复位总线,为枚举设备准备,设置为默认为全速
* Input          : None   
* Return         : None
*******************************************************************************/
void ResetRootHubPort(void)
{
	UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;                                      //USB设备的端点0的最大包尺寸
	
#ifndef DISK_BASE_BUF_LEN	
	memset(&ThisUsbDev,0,sizeof(ThisUsbDev));                                 //清空结构体
#endif	
	
	SetHostUsbAddr(0x00);
	UHOST_CTRL &= ~bUH_PORT_EN;                                                // 关掉端口
	SetUsbSpeed(1);                                                            // 默认为全速
	UHOST_CTRL = UHOST_CTRL & ~ bUH_LOW_SPEED | bUH_BUS_RESET;                   // 默认为全速,开始复位
	mDelaymS(20);                                                            // 复位时间10mS到20mS
	UHOST_CTRL = UHOST_CTRL & ~ bUH_BUS_RESET;                                 // 结束复位
	while ((USB_MIS_ST & bUMS_DEV_ATTACH) == 0);
	mDelaymS(1);
	UIF_DETECT = 0;                                                            // 清中断标志
}

/*******************************************************************************
* Function Name  : EnableRootHubPort
* Description    : 使能ROOT-HUB端口,相应的bUH_PORT_EN置1开启端口,设备断开可能导致返回失败
* Input          : None
* Return         : 返回ERR_SUCCESS为检测到新连接,返回ERR_USB_DISCON为无连接
*******************************************************************************/
UINT8 EnableRootHubPort(void)
{
#ifdef DISK_BASE_BUF_LEN
	if(CH549DiskStatus < DISK_CONNECT) CH549DiskStatus = DISK_CONNECT;
#else
	if (ThisUsbDev.DeviceStatus < ROOT_DEV_CONNECTED) ThisUsbDev.DeviceStatus = ROOT_DEV_CONNECTED;
#endif
	
	if(USB_MIS_ST & bUMS_DEV_ATTACH){                                        // 有设备
#ifndef DISK_BASE_BUF_LEN
		if ((UHOST_CTRL & bUH_PORT_EN) == 0x00){                              // 尚未使能
			ThisUsbDev.DeviceSpeed = USB_MIS_ST & bUMS_DM_LEVEL ? 0 : 1;
			if (ThisUsbDev.DeviceSpeed == 0) UHOST_CTRL |= bUH_LOW_SPEED;          // 低速
		}
#endif
		
		UH_SETUP = bUH_SOF_EN;		
		UHOST_CTRL |= bUH_PORT_EN;                                                 //使能HUB端口
		return(ERR_SUCCESS);
	}
	return(ERR_USB_DISCON);
}

/*******************************************************************************
* Function Name  : SelectHubPort
* Description    : 选定需要操作的HUB口
* Input          : UINT8 HubPortIndex 选择操作指定的ROOT-HUB端口的外部HUB的指定端口
* Return         : None
*******************************************************************************/
void SelectHubPort(UINT8 HubPortIndex)  
{ 
	if(HubPortIndex)                                                         // 选择操作指定的ROOT-HUB端口的外部HUB的指定端口
	{
		SetHostUsbAddr(DevOnHubPort[HubPortIndex-1].DeviceAddress);          // 设置USB主机当前操作的USB设备地址
		SetUsbSpeed(DevOnHubPort[HubPortIndex-1].DeviceSpeed);               // 设置当前USB速度
		if(DevOnHubPort[HubPortIndex-1].DeviceSpeed == 0)                   // 通过外部HUB与低速USB设备通讯需要前置ID
		{
			UH_SETUP |= bUH_PRE_PID_EN;                                        // 启用PRE PID
			mDelayuS(100);
		}
	}
	else                                                                       
	{      			
		SetHostUsbAddr(ThisUsbDev.DeviceAddress);                            // 设置USB主机当前操作的USB设备地址
		SetUsbSpeed(ThisUsbDev.DeviceSpeed);                                 // 设置USB设备的速度
	}
}

/*******************************************************************************
* Function Name  : WaitUSB_Interrupt
* Description    : 等待USB中断
* Input          : None
* Return         : 返回ERR_SUCCESS 数据接收或者发送成功
                   ERR_USB_UNKNOWN 数据接收或者发送失败
*******************************************************************************/
UINT8 WaitUSB_Interrupt(void)
{
	UINT16  i;
	
	for(i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i --){;}
	return(UIF_TRANSFER ? ERR_SUCCESS : ERR_USB_UNKNOWN);
}

/*******************************************************************************
* Function Name  : USBHostTransact
* Description    : CH557传输事务,输入目的端点地址/PID令牌,同步标志,以20uS为单位的NAK重试总时间(0则不重试,0xFFFF无限重试),返回0成功,超时/出错重试
                   本子程序着重于易理解,而在实际应用中,为了提供运行速度,应该对本子程序代码进行优化
* Input          : UINT8 endp_pid 令牌和地址  endp_pid: 高4位是token_pid令牌, 低4位是端点地址
                   UINT8 tog      同步标志
                   UINT16 timeout 超时时间
* Return         : ERR_USB_UNKNOWN 超时，可能硬件异常
                   ERR_USB_DISCON  设备断开
                   ERR_USB_CONNECT 设备连接
                   ERR_SUCCESS     传输完成
*******************************************************************************/
UINT8 USBHostTransact(UINT8 endp_pid, UINT8 tog, UINT16 timeout)
{
//	UINT8	TransRetry;
#define	TransRetry	UEP0_T_LEN	                                               // 节约内存
	UINT8	s, r;
	UINT16	i;
	
	UH_RX_CTRL = UH_TX_CTRL = tog;
	TransRetry = 0;
	
	do{
		UH_EP_PID = endp_pid;                                                      // 指定令牌PID和目的端点号
		UIF_TRANSFER = 0;                                                          // 允许传输
//  s = WaitUSB_Interrupt( );
		for(i = WAIT_USB_TOUT_200US; i != 0 && UIF_TRANSFER == 0; i --);
		UH_EP_PID = 0x00;                                                          // 停止USB传输
//	if ( s != ERR_SUCCESS ) return( s );  // 中断超时,可能是硬件异常
		if(UIF_TRANSFER == 0) return(ERR_USB_UNKNOWN);
		if(UIF_DETECT){                                                        // USB设备插拔事件
//			mDelayuS( 200 );                                                       // 等待传输完成
			UIF_DETECT = 0;                                                          // 清中断标志
			s = AnalyzeRootHub();                                                   // 分析ROOT-HUB状态
			if(s == ERR_USB_CONNECT) FoundNewDev = 1;

#ifdef DISK_BASE_BUF_LEN
			if(CH549DiskStatus == DISK_DISCONNECT) return(ERR_USB_DISCON);      // USB设备断开事件
			if(CH549DiskStatus == DISK_CONNECT) return(ERR_USB_CONNECT);        // USB设备连接事件
#else
			if(ThisUsbDev.DeviceStatus == ROOT_DEV_DISCONNECT) return(ERR_USB_DISCON);// USB设备断开事件
			if(ThisUsbDev.DeviceStatus == ROOT_DEV_CONNECTED) return(ERR_USB_CONNECT);// USB设备连接事件
#endif
			
			mDelayuS(200);  // 等待传输完成
		}
		if(UIF_TRANSFER){  // 传输完成
			if(U_TOG_OK) return(ERR_SUCCESS);
			r = USB_INT_ST & MASK_UIS_H_RES;  // USB设备应答状态
			if(r == USB_PID_STALL) return(r | ERR_USB_TRANSFER);
			if(r == USB_PID_NAK){
				if(timeout == 0) return(r | ERR_USB_TRANSFER);
				if(timeout < 0xFFFF) timeout --;
				-- TransRetry;
			}
			else switch(endp_pid >> 4){
				case USB_PID_SETUP:
				case USB_PID_OUT:
//					if ( U_TOG_OK ) return( ERR_SUCCESS );
//					if ( r == USB_PID_ACK ) return( ERR_SUCCESS );
//					if ( r == USB_PID_STALL || r == USB_PID_NAK ) return( r | ERR_USB_TRANSFER );
					if (r) return(r | ERR_USB_TRANSFER);  // 不是超时/出错,意外应答
					break;  // 超时重试
				case USB_PID_IN:
//					if ( U_TOG_OK ) return( ERR_SUCCESS );
//					if ( tog ? r == USB_PID_DATA1 : r == USB_PID_DATA0 ) return( ERR_SUCCESS );
//					if ( r == USB_PID_STALL || r == USB_PID_NAK ) return( r | ERR_USB_TRANSFER );
					if (r == USB_PID_DATA0 && r == USB_PID_DATA1){  // 不同步则需丢弃后重试
					}  // 不同步重试
					else if(r)return(r | ERR_USB_TRANSFER);  // 不是超时/出错,意外应答
					break;  // 超时重试
				default:
					return(ERR_USB_UNKNOWN);  // 不可能的情况
					break;
			}
		}
		else {  // 其它中断,不应该发生的情况
			USB_INT_FG = 0xFF;  /* 清中断标志 */
		}
		mDelayuS(15);	
	} while (++ TransRetry < 3);
	
	return(ERR_USB_TRANSFER);  // 应答超时
}

/*******************************************************************************
* Function Name  : HostCtrlTransfer
* Description    : 执行控制传输,8字节请求码在pSetupReq中,DataBuf为可选的收发缓冲区
* Input          : PUINT8X DataBuf 如果需要接收和发送数据,那么DataBuf需指向有效缓冲区用于存放后续数据
                   PUINT8 RetLen  实际成功收发的总长度保存在RetLen指向的字节变量中
* Return         : ERR_USB_BUF_OVER IN状态阶段出错
                   ERR_SUCCESS     数据交换成功
                   其他错误状态
*******************************************************************************/
UINT8 HostCtrlTransfer(PUINT8 pBuf, PUINT16 pLen)  
{
	UINT8    s;
	UINT16   RemLen, RxLen, RxCnt, TxCnt;

	mDelayuS(200);
	
	if(pLen)
	{
		*pLen = 0;                                                              // 实际成功收发的总长度
	}
	UH_TX_LEN = sizeof(USB_SETUP_REQ);
	s = USBHostTransact(USB_PID_SETUP << 4 | 0x00, 0x00, 200000/20);          // SETUP阶段,200mS超时
	if(s != ERR_SUCCESS)
	{
		return(s);
	}
	UH_RX_CTRL = UH_TX_CTRL = bUH_R_TOG | bUH_R_AUTO_TOG | bUH_T_TOG | bUH_T_AUTO_TOG;// 默认DATA1
	UH_TX_LEN = 0x01;                                                           // 默认无数据故状态阶段为IN
	RemLen = (pSetupReq -> wLengthH << 8)|( pSetupReq -> wLengthL);
	if(RemLen && pBuf)                                                       // 需要收发数据
	{
		if(pSetupReq -> bRequestType & USB_REQ_TYP_IN)                       // 收
		{
			while(RemLen)
			{
				mDelayuS(200);
				s = USBHostTransact(USB_PID_IN << 4 | 0x00, UH_RX_CTRL, 200000/20);// IN数据
				if(s != ERR_SUCCESS)
				{
					return(s);
				}
				RxLen = USB_RX_LEN < RemLen ? USB_RX_LEN : RemLen;
				RemLen -= RxLen;
				if(pLen)
				{
					*pLen += RxLen;                                              // 实际成功收发的总长度
				}
//              memcpy( pBuf, RxBuffer, RxLen );
//              pBuf += RxLen;
				for(RxCnt = 0; RxCnt != RxLen; RxCnt ++)
				{
					*pBuf = RxBuffer[RxCnt];
					pBuf ++;
				}
				if(USB_RX_LEN == 0 || ( USB_RX_LEN & ( UsbDevEndp0Size - 1 )))
				{
					break;                                                       // 短包
				}
			}
			UH_TX_LEN = 0x00;                                                    // 状态阶段为OUT
		}
		else                                                                     // 发
		{
			while(RemLen)
			{
				mDelayuS(200);
				UH_TX_LEN = RemLen >= UsbDevEndp0Size ? UsbDevEndp0Size : RemLen;
//              memcpy( TxBuffer, pBuf, UH_TX_LEN );
//              pBuf += UH_TX_LEN;
#ifndef DISK_BASE_BUF_LEN
				if(pBuf[1] == 0x09)                                              //HID类命令处理
				{
					Set_Port = Set_Port^1;
					*pBuf = Set_Port;
#ifdef DEBUG							
					printf("SET_PORT  %02X  %02X ",(UINT16)(*pBuf),(UINT16)(Set_Port));
#endif									
				}
#endif
				for(TxCnt = 0; TxCnt != UH_TX_LEN; TxCnt ++)
				{
					TxBuffer[TxCnt] = *pBuf;
					pBuf ++;
				}
				s = USBHostTransact(USB_PID_OUT << 4 | 0x00, UH_TX_CTRL, 200000/20);// OUT数据
				if(s != ERR_SUCCESS)
				{
					return(s);
				}
				RemLen -= UH_TX_LEN;
				if(pLen)
				{
					*pLen += UH_TX_LEN;                                           // 实际成功收发的总长度
				}
			}
//          UH_TX_LEN = 0x01;                                                     // 状态阶段为IN
		}
	}
	mDelayuS(200);
	s = USBHostTransact((UH_TX_LEN ? USB_PID_IN << 4 | 0x00: USB_PID_OUT << 4 | 0x00 ), bUH_R_TOG | bUH_T_TOG, 200000/20);  // STATUS阶段
	if(s != ERR_SUCCESS)
	{
		return(s);
	}
	if(UH_TX_LEN == 0)
	{
		return(ERR_SUCCESS);                                                    // 状态OUT
	}
	if(USB_RX_LEN == 0)
	{
		return(ERR_SUCCESS);                                                    // 状态IN,检查IN状态返回数据长度
	}
	return(ERR_USB_BUF_OVER);                                                   // IN状态阶段错误
}

/*******************************************************************************
* Function Name  : CopySetupReqPkg
* Description    : 复制控制传输的请求包
* Input          : PUINT8C pReqPkt 控制请求包地址
* Return         : None
*******************************************************************************/
void CopySetupReqPkg(PUINT8C pReqPkt)                                        // 复制控制传输的请求包
{
	UINT8 i;
	
	for(i = 0; i != sizeof(USB_SETUP_REQ); i ++)
	{
		((PUINT8X)pSetupReq)[i] = *pReqPkt;
		pReqPkt++;
	}			
}

/*******************************************************************************
* Function Name  : CtrlGetDeviceDescr
* Description    : 获取设备描述符,返回在TxBuffer中
* Input          : None
* Return         : ERR_USB_BUF_OVER 描述符长度错误
                   ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8 CtrlGetDeviceDescr(void)  
{
	UINT8 s;
	UINT16 len;
	
	UsbDevEndp0Size = DEFAULT_ENDP0_SIZE;
	CopySetupReqPkg(SetupGetDevDescr);
	s = HostCtrlTransfer(Com_Buffer, &len);                            // 执行控制传输
	if(s != ERR_SUCCESS)
	{
		return(s);
	}
	UsbDevEndp0Size = ((PXUSB_DEV_DESCR)Com_Buffer) -> bMaxPacketSize0;        // 端点0最大包长度,这是简化处理,正常应该先获取前8字节后立即更新UsbDevEndp0Size再继续
	if(len < ((PUSB_SETUP_REQ)SetupGetDevDescr) -> wLengthL)
	{
		return(ERR_USB_BUF_OVER);                                              // 描述符长度错误
	}
	return(ERR_SUCCESS);
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
UINT8 CtrlGetConfigDescr(void)
{
	UINT8 s;
	UINT16 len;
	
	CopySetupReqPkg(SetupGetCfgDescr);
	s = HostCtrlTransfer(Com_Buffer, &len);                            // 执行控制传输
	if(s != ERR_SUCCESS)
	{
		return(s);
	}

	len = ((UINT16)((PXUSB_CFG_DESCR)Com_Buffer) -> wTotalLengthH<<8)|((PXUSB_CFG_DESCR)Com_Buffer) -> wTotalLengthL;
	CopySetupReqPkg(SetupGetCfgDescr);
	pSetupReq -> wLengthL = (UINT8)len;                                                 // 完整配置描述符的总长度		
	pSetupReq -> wLengthH = (UINT8)(len>>8);
	s = HostCtrlTransfer(Com_Buffer, &len);                            // 执行控制传输
	if(s != ERR_SUCCESS)
	{
		return(s);
	}
	
#ifdef DISK_BASE_BUF_LEN
	if(len>64) len = 64;
	memcpy(TxBuffer,Com_Buffer,len);                                             //U盘操作时，需要拷贝到TxBuffer
#endif
	
	return(ERR_SUCCESS);
}

/*******************************************************************************
* Function Name  : CtrlSetUsbAddress
* Description    : 设置USB设备地址
* Input          : UINT8 addr 设备地址
* Output         : None
* Return         : ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8 CtrlSetUsbAddress(UINT8 addr) 
{
	UINT8 s;
	
	CopySetupReqPkg(SetupSetUsbAddr);	
	pSetupReq -> wValueL = addr;                                                // USB设备地址	
	s = HostCtrlTransfer(NULL, NULL);                                         // 执行控制传输
	if(s != ERR_SUCCESS)
	{
			return(s);
	}
	SetHostUsbAddr(addr);                                                     // 设置USB主机当前操作的USB设备地址
	mDelaymS(10);                                                             // 等待USB设备完成操作
	return(ERR_SUCCESS);
}

/*******************************************************************************
* Function Name  : CtrlSetUsbConfig
* Description    : 设置USB设备配置
* Input          : UINT8 cfg       配置值
* Return         : ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8 CtrlSetUsbConfig(UINT8 cfg)                   
{
	CopySetupReqPkg(SetupSetUsbConfig);	
	pSetupReq->wValueL = cfg;                                                // USB设备配置		
	return(HostCtrlTransfer(NULL, NULL));                                  // 执行控制传输
}

/*******************************************************************************
* Function Name  : CtrlClearEndpStall
* Description    : 清除端点STALL
* Input          : UINT8 endp       端点地址
* Return         : ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8 CtrlClearEndpStall(UINT8 endp)  
{
	CopySetupReqPkg(SetupClrEndpStall);                                      // 清除端点的错误
	pSetupReq -> wIndexL = endp;                                               // 端点地址	
	return(HostCtrlTransfer(NULL, NULL));                          // 执行控制传输
}

#ifndef DISK_BASE_BUF_LEN
/*******************************************************************************
* Function Name  : CtrlSetUsbIntercace
* Description    : 设置USB设备接口
* Input          : UINT8 cfg       配置值
* Return         : ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8 CtrlSetUsbIntercace(UINT8 cfg)                   
{
	CopySetupReqPkg(SetupSetUsbInterface);		
	pSetupReq -> wValueL = cfg;                                              // USB设备配置	
	return(HostCtrlTransfer(NULL, NULL));                           // 执行控制传输
}

/*******************************************************************************
* Function Name  : CtrlGetHIDDeviceReport
* Description    : 获取HID设备报表描述符,返回在TxBuffer中
* Input          : None
* Return         : ERR_SUCCESS 成功
                   其他        错误
*******************************************************************************/
UINT8 CtrlGetHIDDeviceReport(UINT8 infc,PUINT16 pLen)  
{
	UINT8 s;

	CopySetupReqPkg(SetupSetHIDIdle);		
	pSetupReq -> wIndexL = infc;
	s = HostCtrlTransfer(Com_Buffer, pLen);                            // 执行控制传输
	if(s != ERR_SUCCESS)
	{
		return(s);
	}	
	
	CopySetupReqPkg(SetupGetHIDDevReport);	
	pSetupReq -> wIndexL = infc;	
	s = HostCtrlTransfer(Com_Buffer, pLen);                            // 执行控制传输
	if(s != ERR_SUCCESS)
	{
		return(s);
	}

	return(ERR_SUCCESS);
}

/*******************************************************************************
* Function Name  : CtrlGetHubDescr
* Description    : 获取HUB描述符,返回在TxBuffer中
* Input          : None
* Return         : ERR_SUCCESS 成功
                   ERR_USB_BUF_OVER 长度错误
*******************************************************************************/
UINT8 CtrlGetHubDescr(void)  
{
	UINT8 s;
	UINT16 len;
	
	CopySetupReqPkg(SetupGetHubDescr);
	s = HostCtrlTransfer(Com_Buffer, &len);                          // 执行控制传输
	if(s != ERR_SUCCESS)
	{
		return(s);
	}
	if(len < ((PUSB_SETUP_REQ)SetupGetHubDescr) -> wLengthL)
	{
		return(ERR_USB_BUF_OVER);                                            // 描述符长度错误
	}
//  if ( len < 4 ) return( ERR_USB_BUF_OVER );                                 // 描述符长度错误
	return(ERR_SUCCESS);
}

/*******************************************************************************
* Function Name  : HubGetPortStatus
* Description    : 查询HUB端口状态,返回在TxBuffer中
* Input          : UINT8 HubPortIndex 
* Return         : ERR_SUCCESS 成功
                   ERR_USB_BUF_OVER 长度错误
*******************************************************************************/
UINT8 HubGetPortStatus(UINT8 HubPortIndex)   
{
	UINT8 s;
	UINT16 len;
	
	pSetupReq -> bRequestType = HUB_GET_PORT_STATUS;
	pSetupReq -> bRequest = HUB_GET_STATUS;
	pSetupReq -> wValueL = 0x00;
	pSetupReq -> wValueH = 0x00;
	pSetupReq -> wIndexL = HubPortIndex;
	pSetupReq -> wIndexH = 0x00;
	pSetupReq -> wLengthL = 0x04;
	pSetupReq -> wLengthH = 0x00;
	s = HostCtrlTransfer(Com_Buffer, &len);                           // 执行控制传输
	if(s != ERR_SUCCESS)
	{
		return(s);
	}
	if(len < 4)
	{
		return(ERR_USB_BUF_OVER);                                             // 描述符长度错误
	}
	return(ERR_SUCCESS);
}

/*******************************************************************************
* Function Name  : HubSetPortFeature
* Description    : 设置HUB端口特性
* Input          : UINT8 HubPortIndex    //HUB端口
                   UINT8 FeatureSelt     //HUB端口特性
* Return         : ERR_SUCCESS 成功
                   其他        错误
*******************************************************************************/
UINT8 HubSetPortFeature(UINT8 HubPortIndex, UINT8 FeatureSelt) 
{
	pSetupReq -> bRequestType = HUB_SET_PORT_FEATURE;
	pSetupReq -> bRequest = HUB_SET_FEATURE;
	pSetupReq -> wValueL = FeatureSelt;
	pSetupReq -> wValueH = 0x00;
	pSetupReq -> wIndexL = HubPortIndex;
	pSetupReq -> wIndexH = 0x00;
	pSetupReq -> wLengthL = 0x00;
	pSetupReq -> wLengthH = 0x00;
	return(HostCtrlTransfer(NULL, NULL));                                 // 执行控制传输
}

/*******************************************************************************
* Function Name  : HubClearPortFeature
* Description    : 清除HUB端口特性
* Input          : UINT8 HubPortIndex                                         //HUB端口
                   UINT8 FeatureSelt                                          //HUB端口特性
* Return         : ERR_SUCCESS 成功
                   其他        错误
*******************************************************************************/
UINT8 HubClearPortFeature(UINT8 HubPortIndex, UINT8 FeatureSelt) 
{
	pSetupReq -> bRequestType = HUB_CLEAR_PORT_FEATURE;
	pSetupReq -> bRequest = HUB_CLEAR_FEATURE;
	pSetupReq -> wValueL = FeatureSelt;
	pSetupReq -> wValueH = 0x00;
	pSetupReq -> wIndexL = HubPortIndex;
	pSetupReq -> wIndexH = 0x00;
	pSetupReq -> wLengthL = 0x00;
	pSetupReq -> wLengthH = 0x00;
	return(HostCtrlTransfer(NULL, NULL));                                // 执行控制传输
}

/*******************************************************************************
* Function Name  : CtrlGetXPrinterReport1
* Description    : 打印机类命令
* Input          : None
* Return         : ERR_USB_BUF_OVER 描述符长度错误
                   ERR_SUCCESS      成功
                   其他
*******************************************************************************/
UINT8 CtrlGetXPrinterReport1(void)  
{
	UINT8 s;
	UINT16 len;
	
	CopySetupReqPkg(XPrinterReport);
	s = HostCtrlTransfer(Com_Buffer, &len);                         // 执行控制传输
	if(s != ERR_SUCCESS)
	{
		return(s);
	}
//     if ( len < ( (XPrinterReport[7]<<8)|(XPrinterReport[6]) ))
//     {
//         return( ERR_USB_BUF_OVER );                                        // 描述符长度错误
//     }
	return(ERR_SUCCESS);
}

/*******************************************************************************
* Function Name  : AnalyzeHidIntEndp
* Description    : 从描述符中分析出HID中断端点的地址,如果HubPortIndex是0保存到ROOTHUB，如果是非零值则保存到HUB下结构体
* Input          : PUINT8X buf ： 待分析数据缓冲区地址 HubPortIndex：0表示根HUB，非0表示外部HUB下的端口号
* Output         : None
* Return         : 端点数
*******************************************************************************/
UINT8 AnalyzeHidIntEndp(PUINT8X buf, UINT8 HubPortIndex) 
{
	UINT8 i, s, l;
	s = 0;

	if(HubPortIndex)
		memset(DevOnHubPort[HubPortIndex-1].GpVar,0,sizeof(DevOnHubPort[HubPortIndex-1].GpVar)); //清空数组
	else
		memset(ThisUsbDev.GpVar,0,sizeof(ThisUsbDev.GpVar));                     //清空数组

	for(i = 0; i < ( (PXUSB_CFG_DESCR)buf ) -> wTotalLengthL; i += l)       // 搜索中断端点描述符,跳过配置描述符和接口描述符
	{
		if(( (PXUSB_ENDP_DESCR)(buf+i) ) -> bDescriptorType == USB_DESCR_TYP_ENDP  // 是端点描述符
						&& ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bmAttributes & USB_ENDP_TYPE_MASK ) == USB_ENDP_TYPE_INTER// 是中断端点
						&& ( ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_DIR_MASK ) )// 是IN端点
		{           // 保存中断端点的地址,位7用于同步标志位,清0
			if(HubPortIndex)
			DevOnHubPort[HubPortIndex-1].GpVar[s] = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
			else
			ThisUsbDev.GpVar[s] = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bEndpointAddress & USB_ENDP_ADDR_MASK;// 中断端点的地址，可以根据需要保存wMaxPacketSize和bInterval                                                          
#ifdef DEBUG			
	printf("%02x ",(UINT16)ThisUsbDev.GpVar[s]);
#endif
			s++;
			if(s >= 4) break;	//只分析4个端点
	
		}
		l = ( (PXUSB_ENDP_DESCR)(buf+i) ) -> bLength;                          // 当前描述符长度,跳过
		if ( l > 16 )
		{
			break;
		}
	}
	
#ifdef DEBUG
	printf("\n");
#endif	
	
	return( s );
}

/*******************************************************************************
* Function Name  : AnalyzeBulkEndp
* Description    : 分析出批量端点,GpVar[0]、GpVar[1]存放上传端点。GpVar[2]、GpVar[3]存放下传端点
* Input          : buf：待分析数据缓冲区地址   HubPortIndex：0表示根HUB，非0表示外部HUB下的端口号
* Return         : 0
*******************************************************************************/
UINT8 AnalyzeBulkEndp(PUINT8X buf, UINT8 HubPortIndex) 
{
	UINT8   i, s1,s2, l;
	s1 = 0;s2 = 2;
	
	if(HubPortIndex)
		memset(DevOnHubPort[HubPortIndex-1].GpVar,0,sizeof(DevOnHubPort[HubPortIndex-1].GpVar)); //清空数组
	else
		memset(ThisUsbDev.GpVar,0,sizeof(ThisUsbDev.GpVar));                     //清空数组
	
	for(i = 0; i < ((PXUSB_CFG_DESCR)buf) -> wTotalLengthL; i += l)       // 搜索中断端点描述符,跳过配置描述符和接口描述符
	{
		if((((PXUSB_ENDP_DESCR)(buf+i)) -> bDescriptorType == USB_DESCR_TYP_ENDP)     // 是端点描述符
						&& ((((PXUSB_ENDP_DESCR)(buf+i) ) -> bmAttributes & USB_ENDP_TYPE_MASK ) == USB_ENDP_TYPE_BULK))  // 是中断端点
		{
			if(HubPortIndex)
			{
				if(((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_DIR_MASK)
					DevOnHubPort[HubPortIndex-1].GpVar[s1++] = ((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
				else
					DevOnHubPort[HubPortIndex-1].GpVar[s2++] = ((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
			}
			else
			{
				if(((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_DIR_MASK)
					ThisUsbDev.GpVar[s1++] = ((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
				else
					ThisUsbDev.GpVar[s2++] = ((PXUSB_ENDP_DESCR)(buf+i)) -> bEndpointAddress & USB_ENDP_ADDR_MASK;
			}
		
			if(s1 == 2) s1 = 1;
			if(s2 == 4) s2 = 3;			
		}
		
		l = ((PXUSB_ENDP_DESCR)(buf+i)) -> bLength;                          // 当前描述符长度,跳过
		if(l > 16)
		{
			break;
		}
	}
	return(0);
}

//尝试启动AOA模式
UINT8 TouchStartAOA(void)
{
	UINT8 s,i,Num;
	UINT16 len,cp_len;
	
	//获取协议版本号
	CopySetupReqPkg(GetProtocol);
	s = HostCtrlTransfer(Com_Buffer, &len);  // 执行控制传输
	if(s != ERR_SUCCESS)
	{
		return(s);
	}
	if(Com_Buffer[0]<2) return ERR_AOA_PROTOCOL;

	//输出字符串
	for(i=0; i<6; i++)
	{
		Num=Sendlen[i];
		CopySetupReqPkg(&SetStringID[8*i]);
		cp_len = (pSetupReq -> wLengthH << 8)|( pSetupReq -> wLengthL);
		memcpy(Com_Buffer,&StringID[Num],cp_len);
		s = HostCtrlTransfer(Com_Buffer, &len);  // 执行控制传输
		if(s != ERR_SUCCESS)
		{
			return(s);
		}
	}	

	CopySetupReqPkg(TouchAOAMode);
	s = HostCtrlTransfer(Com_Buffer, &len);  // 执行控制传输
	if(s != ERR_SUCCESS)
	{
		return(s);
	}
	
	return ERR_SUCCESS;	
}

/*******************************************************************************
* Function Name  : InitRootDevice
* Description    : 初始化指定ROOT-HUB端口的USB设备
* Input          : UINT8 RootHubIndex 指定端口，内置HUB端口号0/1
* Return         :
*******************************************************************************/
UINT8 InitRootDevice( void ) 
{
	UINT8 t, s, cfg, dv_cls, if_cls,ifc;
	UINT8 touchaoatm = 0;
	UINT16 i,len;
	t = 0;
	
#ifdef DEBUG	
	printf("Reset USB Port\n");
#endif
	
USBDevEnum:
	for(i=0;i<t;i++)
	{
		mDelaymS(100);	
		if(t>10) return(s);			
	}
	ResetRootHubPort();                                                    // 检测到设备后,复位相应端口的USB总线
	for(i = 0, s = 0; i < 100; i ++)                                     // 等待USB设备复位后重新连接,100mS超时
	{
		mDelaymS(1);
		if(EnableRootHubPort() == ERR_SUCCESS)                          // 使能ROOT-HUB端口
		{
			i = 0;
			s ++;                                                           // 计时等待USB设备连接后稳定
			if(s > (20+t))
			{
				break;                                                      // 已经稳定连接15mS
			}
		}
	}	
	if(i)                                                                // 复位后设备没有连接
	{
		DisableRootHubPort();
		
#ifdef DEBUG
		printf("Disable usb port because of disconnect\n");
#endif	
		
		return(ERR_USB_DISCON);
	}
	SelectHubPort( 0 );
#ifdef DEBUG		
	printf("GetDevDescr: ");
#endif
	s = CtrlGetDeviceDescr();                                               // 获取设备描述符
	if(s == ERR_SUCCESS)
	{
#ifdef DEBUG	
		for(i = 0; i < ( (PUSB_SETUP_REQ)SetupGetDevDescr ) -> wLengthL; i ++)
		{
			printf("x%02X ", (UINT16)(Com_Buffer[i]));				
		}
		printf( "\n" );                                                      // 显示出描述符
#endif	
		ThisUsbDev.DeviceVID = (((UINT16)((PXUSB_DEV_DESCR)Com_Buffer)->idVendorH)<<8) + ((PXUSB_DEV_DESCR)Com_Buffer)->idVendorL; //保存VID PID信息
		ThisUsbDev.DevicePID = (((UINT16)((PXUSB_DEV_DESCR)Com_Buffer)->idProductH)<<8) + ((PXUSB_DEV_DESCR)Com_Buffer)->idProductL;
		dv_cls = ((PXUSB_DEV_DESCR)Com_Buffer) -> bDeviceClass;               // 设备类代码			
		s = CtrlSetUsbAddress(((PUSB_SETUP_REQ)SetupSetUsbAddr) -> wValueL);// 设置USB设备地址,加上RootHubIndex可以保证2个HUB端口分配不同的地址
		if(s == ERR_SUCCESS)
		{
				ThisUsbDev.DeviceAddress = ((PUSB_SETUP_REQ)SetupSetUsbAddr) -> wValueL;  // 保存USB地址
			
#ifdef DEBUG						
				printf("GetCfgDescr: ");
#endif					
				s = CtrlGetConfigDescr();                                       // 获取配置描述符
				if(s == ERR_SUCCESS)
				{
							cfg = ( (PXUSB_CFG_DESCR)Com_Buffer ) -> bConfigurationValue;
							ifc = ( (PXUSB_CFG_DESCR)Com_Buffer ) -> bNumInterfaces;					
#ifdef DEBUG							
							for ( i = 0; i < (((UINT16)( (PXUSB_CFG_DESCR)Com_Buffer ) -> wTotalLengthH<<8)|( (PXUSB_CFG_DESCR)Com_Buffer ) -> wTotalLengthL); i ++ )
							{
									printf( "x%02X ", (UINT16)( Com_Buffer[i] ) );
							}
							printf("\n");
#endif								
																																						//分析配置描述符,获取端点数据/各端点地址/各端点大小等,更新变量endp_addr和endp_size等
							if_cls = ( (PXUSB_CFG_DESCR_LONG)Com_Buffer ) -> itf_descr.bInterfaceClass;  // 接口类代码								
							if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_STORAGE )      // 是USB存储类设备,基本上确认是U盘
							{
								AnalyzeBulkEndp(Com_Buffer , 0 );
#ifdef DEBUG
								for(i=0;i!=4;i++)
								{
									printf("%02x ",(UINT16)ThisUsbDev.GpVar[i] );
								}
								printf("\n");
#endif					
								s = CtrlSetUsbConfig( cfg );                              // 设置USB设备配置
								if ( s == ERR_SUCCESS )
								{
									ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
									ThisUsbDev.DeviceType = USB_DEV_CLASS_STORAGE;
#ifdef DEBUG											
									printf( "USB-Disk Ready\n" );
#endif										
									SetUsbSpeed( 1 );                                     // 默认为全速
									return( ERR_SUCCESS );
								}
							}
							else if ( dv_cls == 0x00 && if_cls == USB_DEV_CLASS_PRINTER && ( (PXUSB_CFG_DESCR_LONG)Com_Buffer ) -> itf_descr.bInterfaceSubClass == 0x01 )// 是打印机类设备
							{
#ifdef DEBUG										
	printf( "USB-Print OK\n" );
#endif									
								if((Com_Buffer[19] == 5)&&(Com_Buffer[20]&&0x80)){
									 ThisUsbDev.GpVar[0] = Com_Buffer[20];                     //IN 端点											
								}
								else if((Com_Buffer[19] == 5)&&((Com_Buffer[20]&&0x80) == 0)){
									 ThisUsbDev.GpVar[1] = Com_Buffer[20];                    //OUT 端点												
								}		
								if((Com_Buffer[26] == 5)&&(Com_Buffer[20]&&0x80)){
									 ThisUsbDev.GpVar[0] = Com_Buffer[27];                     //IN 端点											
								}
								else if((Com_Buffer[26] == 5)&&((Com_Buffer[20]&&0x80) == 0)){
									 ThisUsbDev.GpVar[1] = Com_Buffer[27];                    //OUT 端点												
								}										
//                  ThisUsbDev.GpVar = ( (PUSB_CFG_DESCR_LONG)Com_Buffer ) -> endp_descr[0].bEndpointAddress;// 保存批量传输端点
								s = CtrlSetUsbConfig(cfg);                            // 设置USB设备配置
								if(s == ERR_SUCCESS)
								{									
									s = CtrlSetUsbIntercace(cfg);
//                         if(s == ERR_SUCCESS){
																																		 //需保存端点信息以便主程序进行USB传输
									s = CtrlGetXPrinterReport1();                    //打印机类命令
									if(s == ERR_SUCCESS){													 
									 ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
									 ThisUsbDev.DeviceType = USB_DEV_CLASS_PRINTER;
#ifdef DEBUG														 
	printf( "USB-Print Ready\n" );
#endif													 
									 SetUsbSpeed( 1 );                               // 默认为全速
									 return( ERR_SUCCESS );
									}													 
//                         }
								}
							}
							else if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_HID) && (( (PXUSB_CFG_DESCR_LONG)Com_Buffer ) -> itf_descr.bInterfaceSubClass <= 0x01))// 是HID类设备,键盘/鼠标等
							{ 									
									s = AnalyzeHidIntEndp(Com_Buffer,0);                    // 从描述符中分析出HID中断端点的地址								
#ifdef DEBUG														 
									printf("AnalyzeHidIntEndp %02x\n",(UINT16)s);
#endif		                    
									if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceProtocol;
#ifdef DEBUG														 
									printf("CtrlSetUsbConfig %02x\n",(UINT16)cfg);
#endif		
									s = CtrlSetUsbConfig(cfg);                          // 设置USB设备配置								
									if(s == ERR_SUCCESS)
									{
#ifdef DEBUG												
										printf("GetHIDReport: ");
#endif			
										for(dv_cls=0;dv_cls<ifc;dv_cls++)
										{											
											s = CtrlGetHIDDeviceReport(dv_cls,&len);                    //获取报表描述符
											if(s == ERR_SUCCESS)
											{
#ifdef DEBUG														
											for (i = 0; i < len; i++)
											{
												printf("x%02X ", (UINT16)(Com_Buffer[i]));
											}
											printf("\n");
#endif														
										}
									}
											//Set_Idle( );
																																			 //需保存端点信息以便主程序进行USB传输
											ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
											if(if_cls == 1)
											{
												ThisUsbDev.DeviceType = DEV_TYPE_KEYBOARD;
																																		 //进一步初始化,例如设备键盘指示灯LED等
												if(ifc > 1)
												{
#ifdef DEBUG														
						printf("USB_DEV_CLASS_HID Ready\n");
#endif																
						ThisUsbDev.DeviceType = USB_DEV_CLASS_HID;//复合HID设备															
												}																												
#ifdef DEBUG														
												printf("USB-Keyboard Ready\n");
#endif													
												SetUsbSpeed(1);                            // 默认为全速

												return(ERR_SUCCESS);
											}
											else if(if_cls == 2)
											{
												ThisUsbDev.DeviceType = DEV_TYPE_MOUSE;
																																		 //为了以后查询鼠标状态,应该分析描述符,取得中断端口的地址,长度等信息
												if(ifc > 1)
												{
#ifdef DEBUG														
													printf("USB_DEV_CLASS_HID Ready\n");
#endif																
													ThisUsbDev.DeviceType = USB_DEV_CLASS_HID;//复合HID设备															
												}															
#ifdef DEBUG													
												printf("USB-Mouse Ready\n");
#endif													
												SetUsbSpeed(1);                            // 默认为全速
																									
												return(ERR_SUCCESS);
											}
											s = ERR_USB_UNSUPPORT;
									}
							}
							else if(dv_cls == USB_DEV_CLASS_HUB)                   // 是HUB类设备,集线器等
							{
								s = AnalyzeHidIntEndp(Com_Buffer,0);                    // 从描述符中分析出HID中断端点的地址
#ifdef DEBUG		
								printf("AnalyzeHidIntEndp %02x\n",(UINT16)s);
#endif	                   			
#ifdef DEBUG										
								printf("GetHubDescr:");
#endif									
								s = CtrlGetHubDescr();
								if(s == ERR_SUCCESS)
								{
#ifdef DEBUG												
									for(i = 0; i < Com_Buffer[0]; i++)
									{
										printf("x%02X ",(UINT16)(Com_Buffer[i]));
									}                       
			printf("\n");
#endif												
									ThisUsbDev.GpHUBPortNum = ( (PXUSB_HUB_DESCR)Com_Buffer ) -> bNbrPorts;// 保存HUB的端口数量
									if ( ThisUsbDev.GpHUBPortNum > HUB_MAX_PORTS )
									{
										ThisUsbDev.GpHUBPortNum = HUB_MAX_PORTS;// 因为定义结构DevOnHubPort时人为假定每个HUB不超过HUB_MAX_PORTS个端口
									}
									//if ( ( (PXUSB_HUB_DESCR)Com_Buffer ) -> wHubCharacteristics[0] & 0x04 ) printf("带有集线器的复合设备\n");
									//else printf("单一的集线器产品\n");
									s = CtrlSetUsbConfig( cfg );                     // 设置USB设备配置
									if ( s == ERR_SUCCESS )
									{
										ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
										ThisUsbDev.DeviceType = USB_DEV_CLASS_HUB;
										//需保存端点信息以便主程序进行USB传输,本来中断端点可用于HUB事件通知,但本程序使用查询状态控制传输代替
										//给HUB各端口上电,查询各端口状态,初始化有设备连接的HUB端口,初始化设备
										for(i = 1; i <= ThisUsbDev.GpHUBPortNum; i ++)// 给HUB各端口都上电
										{
											DevOnHubPort[i-1].DeviceStatus = ROOT_DEV_DISCONNECT;  // 清外部HUB端口上设备的状态
											s = HubSetPortFeature( i, HUB_PORT_POWER );
											if(s != ERR_SUCCESS)
											{
#ifdef DEBUG																		
													printf("Ext-HUB Port_%1d# power on error\n",(UINT16)i);// 端口上电失败
#endif																	
											}
										}
//							for ( i = 1; i <= ThisUsbDev.GpVar[0]; i ++ )            // 查询HUB各端口连接状态
//							{
//								s = HubGetPortStatus( i );                           // 获取端口状态
//								if ( s != ERR_SUCCESS ) 
//#ifdef DEBUG																	
//								printf( "Ext-HUB Port_%1d#	clear connection error\n",(UINT16)i );	// 端口连接状态清除失败
//#endif															
//							}														
											SetUsbSpeed(1);                                        // 默认为全速
											return(ERR_SUCCESS);
									}
								}
							}
							else                                                                 //其他设备
							{			
#ifdef DEBUG														 
								printf("dv_cls %02x\n",(UINT16)dv_cls);
								printf("if_cls %02x\n",(UINT16)if_cls);
								printf("if_subcls %02x\n",(UINT16)((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceSubClass);									
#endif				
								AnalyzeBulkEndp(Com_Buffer , 0);                                  //分析出批量端点
#ifdef DEBUG
								for(i=0;i!=4;i++)
								{
									printf("%02x ",(UINT16)ThisUsbDev.GpVar[i]);
								}
								printf("\n");
#endif
								s = CtrlSetUsbConfig( cfg );                                     // 设置USB设备配置
								if ( s == ERR_SUCCESS ) 
								{
#ifdef DEBUG						
					printf("%02x %02x\n",(UINT16)ThisUsbDev.DeviceVID,(UINT16)ThisUsbDev.DevicePID);
#endif						
//						if((ThisUsbDev.DeviceVID==0x18D1)&&(ThisUsbDev.DevicePID&0xff00)==0x2D00)   //如果是AOA配件
//						{
//							printf("AOA Mode\n");
//							ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
//							ThisUsbDev.DeviceType = DEF_AOA_DEVICE;                      //这只是自定义的变量类，不属于USB协议类
//							SetUsbSpeed( 1 );                                            // 默认为全速
//							return( ERR_SUCCESS );
//						}
//						else   //如果不是AOA 配件模式，尝试启动配件模式.
//						{
//							s = TouchStartAOA();
//							if( s == ERR_SUCCESS ) 
//							{
//								if(touchaoatm<3)         //尝试AOA启动次数限制
//								{
//									touchaoatm++;
//									mDelaymS(500);      //部分安卓设备自动断开重连，所以此处最好有延时
//									goto USBDevEnum;    //其实这里可以不用跳转，AOA协议规定，设备会自动重新接入总线。
//								}
//								//执行到这，说明可能不支持AOA，或是其他设备
//								ThisUsbDev.DeviceType = dv_cls ? dv_cls : if_cls;
//								ThisUsbDev.DeviceStatus = ROOT_DEV_SUCCESS;
//								SetUsbSpeed( 1 );                                            // 默认为全速
//								return( ERR_SUCCESS );                                       // 未知设备初始化成功									
//							}							
//						}
						return( ERR_SUCCESS ); 
					}
				}
			}
		}
	}
	
#ifdef DEBUG			
	printf( "InitRootDev Err = %02X\n", (UINT16)s );
#endif		
	
	ThisUsbDev.DeviceStatus = ROOT_DEV_FAILED;
	SetUsbSpeed( 1 );                                                                 // 默认为全速
	t++;
	goto USBDevEnum;		
}

/*******************************************************************************
* Function Name  : EnumAllRootDevice
* Description    : 枚举所有ROOT-HUB端口的USB设备
* Input          : None
* Return         : None
*******************************************************************************/
UINT8 EnumAllRootDevice(void)   
{
	UINT8I s;
	
#ifdef DEBUG
	printf( "EnumUSBDev\n" );
#endif
	
	if(ThisUsbDev.DeviceStatus == ROOT_DEV_CONNECTED)                        // 刚插入设备尚未初始化
	{
		s = InitRootDevice();                                      // 初始化/枚举指定HUB端口的USB设备
		if(s != ERR_SUCCESS)
		{
			return(s);
		}
	}
	return(ERR_SUCCESS);
}

/*******************************************************************************
* Function Name  : SearchTypeDevice
* Description    : 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号,输出端口号为0xFFFF则未搜索到
* Input          : UINT8 type 搜索的设备类型
* Return         : 输出高8位为ROOT-HUB端口号,低8位为外部HUB的端口号,低8位为0则设备直接在ROOT-HUB端口上
                   当然也可以根据USB的厂商VID产品PID进行搜索(事先要记录各设备的VID和PID),以及指定搜索序号
*******************************************************************************/
UINT16 SearchTypeDevice(UINT8 type)   
{
	UINT8 RootHubIndex;                                                          //CH557只有一个USB口,RootHubIndex = 0,只需看返回值的低八位即可
  UINT8 HubPortIndex;
	
	RootHubIndex = 0;
	if((ThisUsbDev.DeviceType == USB_DEV_CLASS_HUB) && (ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS))// 外部集线器HUB且枚举成功
	{
		for(HubPortIndex = 1; HubPortIndex <= ThisUsbDev.GpHUBPortNum; HubPortIndex ++)// 搜索外部HUB的各个端口
		{
			if(DevOnHubPort[HubPortIndex-1].DeviceType == type && DevOnHubPort[HubPortIndex-1].DeviceStatus >= ROOT_DEV_SUCCESS)
			{
				return(((UINT16)RootHubIndex << 8) | HubPortIndex);           // 类型匹配且枚举成功
			}
		}
	}
	if((ThisUsbDev.DeviceType == type) && (ThisUsbDev.DeviceStatus >= ROOT_DEV_SUCCESS))
	{
		return((UINT16)RootHubIndex << 8);                                      // 类型匹配且枚举成功,在ROOT-HUB端口上
	} 
	
	return(0xFFFF);
}
#endif


#ifdef DISK_BASE_BUF_LEN

UINT8 InitRootDevice(void)                                                       // 初始化USB设备
{
	UINT8	s, cfg, dv_cls, if_cls,t=0;
	UINT16  i;
#ifdef DEBUG	
	printf("Reset host port\n");
#endif
USBDevEnum:
	for(i=0;i<t;i++)
	{
		mDelaymS(100);	
		if(t>10) return(s);			
	}
	ResetRootHubPort();                                                            // 检测到设备后,复位相应端口的USB总线
	for (i = 0, s = 0; i < 100; i ++){                                           // 等待USB设备复位后重新连接,100mS超时
		mDelaymS( 1 );
		if (EnableRootHubPort() == ERR_SUCCESS){                                  // 使能端口
			i = 0;
			s ++;                                                                       // 计时等待USB设备连接后稳定
			if (s > 100) break;                                                       // 已经稳定连接100mS
		}
	}
	if(i){                                                                      // 复位后设备没有连接
		DisableRootHubPort();
#ifdef DEBUG			
		printf("Disable host port because of disconnect\n");
#endif		
		return(ERR_USB_DISCON);
	}
	SetUsbSpeed(1);                                                              // 设置当前USB速度
	s = CtrlGetDeviceDescr();                                                     // 获取设备描述符
	if(s == ERR_SUCCESS){
#ifdef DEBUG			
	  printf("GetDevDescr: ");		
		for(i = 0; i < ((PUSB_SETUP_REQ)SetupGetDevDescr) -> wLengthL; i ++) printf("x%02X ", (UINT16)( Com_Buffer[i]));
		printf("\n");                                                             // 显示出描述符
#endif
		dv_cls = ((PXUSB_DEV_DESCR)Com_Buffer) -> bDeviceClass;                     // 设备类代码
		s = CtrlSetUsbAddress(((PUSB_SETUP_REQ)SetupSetUsbAddr) -> wValueL);    // 设置USB设备地址
		if(s == ERR_SUCCESS){
			s = CtrlGetConfigDescr();                                                // 获取配置描述符
			if(s == ERR_SUCCESS){
				cfg = ((PXUSB_CFG_DESCR)Com_Buffer) -> bConfigurationValue;
#ifdef DEBUG					
			  printf("GetCfgDescr: ");				
				for(i = 0; i < (((UINT16)((PXUSB_CFG_DESCR)Com_Buffer) -> wTotalLengthH<<8)|((PXUSB_CFG_DESCR)Com_Buffer) -> wTotalLengthL); i ++) 
					printf("x%02X ", (UINT16)( Com_Buffer[i]));
				printf("\n");
#endif				
/* 分析配置描述符,获取端点数据/各端点地址/各端点大小等,更新变量endp_addr和endp_size等 */
				if_cls = ((PXUSB_CFG_DESCR_LONG)Com_Buffer) -> itf_descr.bInterfaceClass; // 接口类代码
				if((dv_cls == 0x00) && (if_cls == USB_DEV_CLASS_STORAGE)){            // 是USB存储类设备,基本上确认是U盘					
//					s = CtrlSetUsbConfig( cfg );                                     // 设置USB设备配置
//                    if ( s == ERR_SUCCESS )
					{
						CH549DiskStatus = DISK_USB_ADDR;
						return(ERR_SUCCESS);
					}
//					else return( ERR_USB_UNSUPPORT );
				}
				else{
					return(ERR_USB_UNSUPPORT);
				}
			}
		}
	}
#ifdef DEBUG		
	printf("InitRootDev Err = %02X\n", (UINT16)s);
#endif
	CH549DiskStatus = DISK_CONNECT;
	SetUsbSpeed(1); // 默认为全速
	t++;
	goto USBDevEnum;	
 
}

#endif

/*******************************************************************************
* Function Name  : InitUSB_Host
* Description    : 初始化USB主机
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitUSB_Host(void)
{
	IE_USB = 0;

	USB_DEV_AD = 0x00;
	UH_EP_MOD = bUH_EP_TX_EN | bUH_EP_RX_EN ;		//端点使能
	UH_RX_DMA = RxBuffer;
	UH_TX_DMA = TxBuffer;
	UH_RX_CTRL = 0x00;
	UH_TX_CTRL = 0x00;
	USB_CTRL = bUC_HOST_MODE | bUC_INT_BUSY;		// 设定模式,在中断标志未清除前自动暂停

	USB_INT_FG = 0xFF;                          // 清中断标志
	
	DisableRootHubPort( );                      // 关闭端口
	
	USB_INT_EN = bUIE_TRANSFER | bUIE_DETECT;		//中断使能
	//IE_USB = 1;                           		// 查询方式
}


