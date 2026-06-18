/********************************** (C) COPYRIGHT *******************************
* File Name          : CompositeKM.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543模拟USB键鼠复合设备,支持类命令,支持唤醒
                       演示键盘鼠标简单操作。其他键值，参考 HID USAGE TABLE协议文档
                       串口0接收字符：
                       ‘L’：鼠标左键
                       ‘R’: 鼠标右键
                       ‘A’: 键盘‘A’键
                       ‘Q’: 键盘‘Caps’键
                    任意字符：主机睡眠状态下,设备远程唤醒主机（注意设备一般需自供电,因为主机休眠可能USB口也会掉电）
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


/******************************************************************************/
/* 头文件包含 */
#include "DEBUG.H"

/******************************************************************************/
/* 常、变量定义 */
//#define Fullspeed
#ifdef  Fullspeed
#define THIS_ENDP0_SIZE         64
#else
#define THIS_ENDP0_SIZE         8                                               /* 低速USB，中断传输、控制传输最大包长度为8 */
#endif

#define ENDP1_IN_SIZE           8                                               /* 键盘端点数据包大小 */
#define ENDP2_IN_SIZE           4                                               /* 鼠标端点数据包大小 */

/* 端点0 OUT&IN缓冲区，必须是偶地址 */
UINT8X  Ep0Buffer[ MIN( 64, THIS_ENDP0_SIZE + 2 ) ] _at_ 0x0000;
/* 端点1 IN缓冲区,必须是偶地址 */
UINT8X  Ep1Buffer[ MIN( 64, ENDP1_IN_SIZE + 2 ) ]   _at_ MIN( 64, THIS_ENDP0_SIZE + 2 );
/* 端点2 IN缓冲区,必须是偶地址 */
UINT8X  Ep2Buffer[ MIN( 64, ENDP2_IN_SIZE + 2 ) ]   _at_ ( MIN( 64, THIS_ENDP0_SIZE + 2 ) + MIN( 64, ENDP1_IN_SIZE + 2 ) );

/*键盘数据*/
UINT8 HIDKey[ ENDP1_IN_SIZE ];
/*鼠标数据*/
UINT8 HIDMouse[ ENDP2_IN_SIZE ];

UINT8 Endp1Busy = 0;                                                             /* 传输完成控制标志位 */
UINT8 Endp2Busy = 0;
UINT8 WakeUpEnFlag = 0;                                                          /* 远程唤醒使能标志 */
UINT8   SetupReq,Ready,UsbConfig;

#define UsbSetupBuf     ( (PUSB_SETUP_REQ)Ep0Buffer )
#pragma NOAREGS

/******************************************************************************/
/* USB设备描述符 */
UINT8C DevDesc[ ] =
{
    0x12, 0x01, 0x10, 0x01, 0x00, 0x00, 0x00, THIS_ENDP0_SIZE,
    0x86, 0x1a, 0xe1, 0xe6, 0x00, 0x01, 0x01, 0x02,
    0x00, 0x01
};

/******************************************************************************/
/* 语言字符串描述符 */
UINT8C  MyLangDescr[ ] = { 0x04, 0x03, 0x09, 0x04 };

/******************************************************************************/
/* 厂商字符串描述符 */
UINT8C  MyManuInfo[ ] = { 0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0 };

/******************************************************************************/
/* 产品字符串描述符 */
UINT8C  MyProdInfo[ ] = { 0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '4', 0, '3', 0 };

/******************************************************************************/
/*HID类报表描述符*/
UINT8C KeyRepDesc[ ] =
{
    0x05, 0x01, 0x09, 0x06, 0xA1, 0x01, 0x05, 0x07,
    0x19, 0xe0, 0x29, 0xe7, 0x15, 0x00, 0x25, 0x01,
    0x75, 0x01, 0x95, 0x08, 0x81, 0x02, 0x95, 0x01,
    0x75, 0x08, 0x81, 0x01, 0x95, 0x03, 0x75, 0x01,
    0x05, 0x08, 0x19, 0x01, 0x29, 0x03, 0x91, 0x02,
    0x95, 0x05, 0x75, 0x01, 0x91, 0x01, 0x95, 0x06,
    0x75, 0x08, 0x26, 0xff, 0x00, 0x05, 0x07, 0x19,
    0x00, 0x29, 0x91, 0x81, 0x00, 0xC0
};

UINT8C MouseRepDesc[ ] =
{
    0x05, 0x01, 0x09, 0x02, 0xA1, 0x01, 0x09, 0x01,
    0xA1, 0x00, 0x05, 0x09, 0x19, 0x01, 0x29, 0x03,
    0x15, 0x00, 0x25, 0x01, 0x75, 0x01, 0x95, 0x03,
    0x81, 0x02, 0x75, 0x05, 0x95, 0x01, 0x81, 0x01,
    0x05, 0x01, 0x09, 0x30, 0x09, 0x31, 0x09, 0x38,
    0x15, 0x81, 0x25, 0x7f, 0x75, 0x08, 0x95, 0x03,
    0x81, 0x06, 0xC0, 0xC0
};

/******************************************************************************/
/* USB配置描述符 */
UINT8C CfgDesc[ ] =
{
    0x09, 0x02, 0x3b, 0x00, 0x02, 0x01, 0x00, 0xA0, 0x32,                       /* 配置描述符 */

    0x09, 0x04, 0x00, 0x00, 0x01, 0x03, 0x01, 0x01, 0x00,                       /* 接口描述符,键盘 */
    0x09, 0x21, 0x11, 0x01, 0x00, 0x01, 0x22, sizeof( KeyRepDesc ) & 0xFF, sizeof( KeyRepDesc ) >> 8,   /* HID类描述符 */
    0x07, 0x05, 0x81, 0x03, ENDP1_IN_SIZE, 0x00,0x0a,                           /* 端点描述符 */

    0x09, 0x04, 0x01, 0x00, 0x01, 0x03, 0x01, 0x02,0x00,                        /* 接口描述符,鼠标 */
    0x09, 0x21, 0x10, 0x01, 0x00, 0x01, 0x22, sizeof( MouseRepDesc ) & 0xFF, sizeof( MouseRepDesc ) >> 8,/* HID类描述符 */
    0x07, 0x05, 0x82, 0x03, ENDP2_IN_SIZE, 0x00, 0x0a                           /* 端点描述符 */
};

/*******************************************************************************
* Function Name  : CH543USBDevWakeup
* Description    : CH543设备模式唤醒主机，发送K信号
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH554USBDevWakeup( void )
{
#ifdef Fullspeed
    UDEV_CTRL |= bUD_LOW_SPEED;
    mDelaymS( 2 );
    UDEV_CTRL &= ~bUD_LOW_SPEED;
#else
    UDEV_CTRL &= ~bUD_LOW_SPEED;
    mDelaymS( 2 );
    UDEV_CTRL |= bUD_LOW_SPEED;
#endif
}

/*******************************************************************************
* Function Name  : Enp1IntIn
* Description    : USB设备模式端点1的中断上传
* Input          : *buf: 上传数据
*                  len:上传数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void Enp1IntIn( UINT8 *buf, UINT8 len )
{

    memcpy( Ep1Buffer, buf, len );                                              /* 加载上传数据 */
    if( Ready )
    {
        UEP1_T_LEN = len;                                                           /* 上传数据长度 */
        UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                   /* 有数据时上传数据并应答ACK */
        Endp1Busy = 1;                                                              
    }
}

/*******************************************************************************
* Function Name  : Enp2IntIn
* Description    : USB设备模式端点2的中断上传
* Input          : *buf: 上传数据
*                  len:上传数据长度
* Output         : None
* Return         : None
*******************************************************************************/
void Enp2IntIn( UINT8 *buf, UINT8 len )
{
    memcpy( Ep2Buffer, buf, len );                                                   /* 加载上传数据 */
    if( Ready )
    {
        UEP2_T_LEN = len;                                                            /* 上传数据长度 */
        UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                    /* 有数据时上传数据并应答ACK */
        Endp2Busy = 1;                                                               
    }
}

/*******************************************************************************
* Function Name  : USB_DeviceInterrupt
* Description    : USB中断服务程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_DeviceInterrupt( void ) interrupt INT_NO_USB using 1	
{
	UINT8  errflag;
    UINT16 len;
	static	UINT8 SetupReqCode;
    static  UINT16 SetupLen;
	static	PUINT8	pDescr;
    
	if( UIF_TRANSFER )                                                          /* USB传输完成 */
    {
 USB_DevIntNext:
		switch( USB_INT_ST & ( bUIS_SETUP_ACT | MASK_UIS_TOKEN | MASK_UIS_ENDP ) )  /* 分析操作令牌和端点号 */
        {
			case UIS_TOKEN_IN | 2:                                              /* endpoint 2# 批量端点上传 */
			case bUIS_SETUP_ACT | UIS_TOKEN_IN | 2:
					UEP2_T_LEN = 0;                                             /* 预使用发送长度一定要清空 */
					UEP2_CTRL ^= bUEP_T_TOG;                                    /* 手动翻转 */
					Endp2Busy = 0;
					UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;   /* 默认应答NAK */
				break;
			case UIS_TOKEN_IN | 1:                                              /* endpoint 1# 中断端点上传 */
			case bUIS_SETUP_ACT | UIS_TOKEN_IN | 1:
					UEP1_T_LEN = 0;                                             /* 预使用发送长度一定要清空 */
					UEP1_CTRL ^= bUEP_T_TOG;                                    /* 手动翻转 */
					Endp1Busy = 0;
					UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;   /* 默认应答NAK */
				break;
			case UIS_TOKEN_IN | 0:                                              /* endpoint 0# IN */
			case bUIS_SETUP_ACT | UIS_TOKEN_IN | 0:
				switch( SetupReqCode )
                {
					case USB_GET_DESCRIPTOR:
						len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;     /* 本次传输长度 */
						memcpy( Ep0Buffer, pDescr, len );                       /* 加载上传数据 */
						SetupLen -= len;
						pDescr += len;
						UEP0_T_LEN = len;
						UEP0_CTRL ^= bUEP_T_TOG;                                /* 翻转 */
						break;
					case USB_SET_ADDRESS:
						USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
						UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
						break;
					default:
                        /* 状态阶段完成中断或者是强制上传0长度数据包结束控制传输 */
						UEP0_T_LEN = 0;
						UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
						break;
				}
				break;
			case UIS_TOKEN_OUT | 0:                                             /* endpoint 0# OUT */
			case bUIS_SETUP_ACT | UIS_TOKEN_OUT | 0:
				len = USB_RX_LEN;
				if(SetupReqCode == 0x09)
				{
					if(Ep0Buffer[ 0 ])
					{
						printf("Light on Num Lock LED!\n");
					}
					else if(Ep0Buffer[ 0 ] == 0)
					{
						printf("Light off Num Lock LED!\n");
					}
				}
				UEP0_CTRL ^= bUEP_R_TOG;                                        /* 同步标志位翻转 */
				break;
			default:
			if ( ( USB_INT_ST & ( bUIS_SETUP_ACT | MASK_UIS_TOKEN ) ) == ( bUIS_SETUP_ACT | UIS_TOKEN_FREE ) ){     /* endpoint 0# SETUP */
				UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_NAK | UEP_T_RES_NAK; 
				SetupLen = ( (UINT16)UsbSetupBuf->wLengthH << 8 ) + UsbSetupBuf->wLengthL;
				SetupReqCode = UsbSetupBuf->bRequest;
				len = 0;                                                        /* 默认为成功并且上传0长度 */
                errflag = 0;
				if ( ( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )
                {  
					switch( SetupReqCode )
                    {
						case 0x01:                                              /* GetReport */
							break;
						case 0x02:                                              /* GetIdle */
							break;
						case 0x03:                                              /* GetProtocol */
							break;
						case 0x09:                                               /* SetReport */
							break;
						case 0x0A:                                              /* SetIdle */
							break;
						case 0x0B:                                              /* SetProtocol */
							break;
						default:
							errflag = 0xFF;                                       /* 命令不支持 */
							break;
					}
				}
				else                                                            /* 标准请求 */
                {
					switch( SetupReqCode )                                      /* 请求码 */
                    {
						case USB_GET_DESCRIPTOR:
							switch( UsbSetupBuf->wValueH )
                            {
								case 1:                                         /* 设备描述符 */
									pDescr = (PUINT8)( &DevDesc[ 0 ] );
									len = sizeof( DevDesc );
									break;
								case 2:                                         /* 配置描述符 */
									pDescr = (PUINT8)( &CfgDesc[ 0 ] );
									len = sizeof( CfgDesc );
									break;
								case 3:                                         /* 字符串描述符 */
									switch( UsbSetupBuf->wValueL )
                                    {
										case 1:
											pDescr = (PUINT8)( &MyManuInfo[ 0 ] );
											len = sizeof( MyManuInfo );
											break;
										case 2:
											pDescr = (PUINT8)( &MyProdInfo[ 0 ] );
											len = sizeof( MyProdInfo );
											break;
										case 0:
											pDescr = (PUINT8)( &MyLangDescr[ 0 ] );
											len = sizeof( MyLangDescr );
											break;
										default:
											errflag = 0xFF;                       /* 不支持的字符串描述符 */
											break;
									}
									break;
								case USB_DESCR_TYP_REPORT:
									if( UsbSetupBuf->wIndexL == 0 )             /* 接口0报表描述符 */
									{
										pDescr = KeyRepDesc;                    /* 数据准备上传 */
										len = sizeof( KeyRepDesc );
									}
									else if( UsbSetupBuf->wIndexL == 1 )        /* 接口1报表描述符 */
									{
										pDescr = MouseRepDesc;                  /* 数据准备上传 */
										len = sizeof( MouseRepDesc );
									}
									else
									{
										errflag = 0xFF;                           /* 本程序只有2个接口，这句话正常不可能执行 */
									}
									break;
								default:
									errflag = 0xFF;                               /* ֧支持的描述符类型 */
									break;
							}
							if( SetupLen > len ) SetupLen = len;                /* 限制总长度 */
							len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;     /* 本次传输长度 */
							memcpy( Ep0Buffer, pDescr, len );                   /* 加载上传数据 */
							SetupLen -= len;
							pDescr += len;
							break;                              																
						case USB_SET_ADDRESS:
							SetupLen = UsbSetupBuf->wValueL;                    /* 暂存USB设备地址 */
							break;
						case USB_GET_CONFIGURATION:
							Ep0Buffer[ 0 ] = UsbConfig;
							if ( SetupLen >= 1 ) len = 1;
							break;
						case USB_SET_CONFIGURATION:
							UsbConfig = UsbSetupBuf->wValueL;
							if( UsbConfig )
							{
								printf("SET CONFIG.\n");
								Ready = 1;                                      /* set config命令一般代表usb枚举完成的标志 */
							}
							break;
						case USB_CLEAR_FEATURE:
							if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )       /* 端点 */
                            {
								switch( UsbSetupBuf->wIndexL )
                                {
									case 0x82:
										UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
										break;
									case 0x02:
										UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
										break;
									case 0x81:
										UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
										break;
									case 0x01:
										UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
										break;
									default:
										errflag = 0xFF;                           /* 不支持的端点 */
										break;
								}
							}
							else if( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE )  /* 设备 */
							{
								WakeUpEnFlag &= ~0x01;
								printf("Wake\n");
								break;
							}
							else
							{
								errflag = 0xFF;                                   /* 不是端点不支持 */
							}
							break;
						case USB_SET_FEATURE:                                   /* Set Feature */
							if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x00)   /* 设置设备 */
							{
								if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01)
								{
									if( CfgDesc[ 7 ] & 0x20 )
									{
										WakeUpEnFlag |= 0x01;                       /* 设置唤醒使能标志 */
										printf("Enable Remote Wakeup.\n");
									}
									else
									{
										errflag = 0xFF;                           /* 操作失败 */
									}
								}
								else
								{
									errflag = 0xFF;                               /* 操作失败 */
								}
							}
							else if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x02 )        /* 设置端点 */
							{
								if( ( ( (UINT16)UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL) == 0x00 )
								{
									switch( ( (UINT16)UsbSetupBuf->wIndexH << 8 ) | UsbSetupBuf->wIndexL )
                                    {
										case 0x82:
											UEP2_CTRL = UEP2_CTRL & ( ~bUEP_T_TOG ) | UEP_T_RES_STALL;  /* 设置端点2 IN STALL */
											break;
										case 0x02:
											UEP2_CTRL = UEP2_CTRL & ( ~bUEP_R_TOG ) | UEP_R_RES_STALL;  /* 设置端点2 OUT Stall */
											break;
										case 0x81:
											UEP1_CTRL = UEP1_CTRL & ( ~bUEP_T_TOG ) | UEP_T_RES_STALL;  /* 设置端点1 IN STALL */
											break;
										default:
											errflag = 0xFF;                       /* 操作失败 */
											break;
									}
								}
								else
								{
									errflag = 0xFF;                               /* 操作失败 */
								}
							}
							else
							{
								errflag = 0xFF;                                   /* 操作失败 */
							}
							break;
						case USB_GET_INTERFACE:
							Ep0Buffer[ 0 ] = 0x00;
							if(SetupLen >= 1) len = 1;
							break;
						case USB_GET_STATUS:
							Ep0Buffer[ 0 ] = 0x00;
							Ep0Buffer[ 1 ] = 0x00;
							if ( SetupLen >= 2 ) len = 2;
							else len = SetupLen;
							break;
						default:
							errflag = 0xFF;                                       /* 操作失败 */
							printf("ErrEp0ReqCode=%02X\n",(UINT16)SetupReqCode);
							break;
					}
				}
				if(errflag == 0xFF)                                               /* 操作失败 */
                {
					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;    /* STALL */
				}
				else if(len <= THIS_ENDP0_SIZE)                                 /* 上传数据或者状态阶段返回0长度包 */
                {
					UEP0_T_LEN = len;
					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;    /* 默认数据包是DATA1 */
				}
				else                                                            /* 下传数据或其它 */
                {
                    /* 虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段 */
					UEP0_T_LEN = 0;
					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;    /* 默认数据包是DATA1 */
				}
				break;
			}
			else
            {
				printf("ErrEndpInt\n");
				break;
			}
		}

		UIF_TRANSFER = 0;                                                       /* 清中断标志 */
	}
	else if( UIF_BUS_RST )                                                      /* USB总线复位 */
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP1_CTRL = UEP_T_RES_NAK;
        UEP2_CTRL = UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        Ready = 0;
        WakeUpEnFlag = 0;
        Endp1Busy = 0; 
        Endp2Busy = 0; 
        UIF_BUS_RST = 0;                                                        /* 清中断标志 */
	}
	else if( UIF_SUSPEND )                                                      /* USB总线挂起/唤醒完成 */
    {
		UIF_SUSPEND = 0;
		if( USB_MIS_ST & bUMS_SUSPEND )                                         /* 挂起 */
        {
			WakeUpEnFlag |= 0x02;
			if( WakeUpEnFlag == 0x03 )
			{
				printf( "Sleep\n" );

			}
		}
		else                                                                    /* 唤醒 */
        {
			WakeUpEnFlag &= ~0x02; 
			
		}
	}
	else                                                                        /* 意外的中断,不可能发生的情况 */
    {
		printf("UnknownInt\n");
		USB_INT_FG = 0xFF;                                                      /* 清中断标志 */
	}
	if( UIF_TRANSFER ) goto USB_DevIntNext;
	
}

/*******************************************************************************
* Function Name  : InitUSB_Device
* Description    : USB设备初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
/*--------------------------------------------------------------
          | bUDP_PUE | bUDM_PUE | bUD_LOW_SPEED | bUC_LOW_SPEED
Low  Speed|    0     |    1     |       1       |       1        
Full Speed|    1     |    0     |       0       |       0
---------------------------------------------------------------*/
void InitUSB_Device( void )
{
	IE_USB = 0;
	USB_CTRL = 0x00;                                                            /* 先设定模式 */
	UEP1_T_LEN = 0;                                                             /* 预使用发送长度一定要清空 */
	UEP2_T_LEN = 0;                                                             /* 预使用发送长度一定要清空 */
	UEP4_1_MOD = bUEP1_TX_EN;                                                   /* 端点1上传IN */
	UEP2_3_MOD = bUEP2_TX_EN;                                                   /* 端点2上传IN */
	UEP0_DMA_L = Ep0Buffer;
	UEP1_DMA_L = Ep1Buffer;
	UEP2_DMA_L = Ep2Buffer;
	USB_DEV_AD = 0x00;
	USB_PULL   = 0x00;                                                          /* 禁止DP/DM下拉电阻 */
	
#ifndef Fullspeed
	UDEV_CTRL |= bUD_LOW_SPEED;                                                 /* 选择低速1.5M模式 */
	USB_CTRL |= bUC_LOW_SPEED;
    USB_PULL = USB_PULL & ~bUDP_PUE | bUDM_PUE;                                 /* Low  Speed */
#else
	UDEV_CTRL &= ~bUD_LOW_SPEED;                                                /* 选择全速12M模式，默认方式 */
	USB_CTRL &= ~bUC_LOW_SPEED;
    USB_PULL = bUDP_PUE;                                                        /* Full Speed */
#endif

	USB_CTRL |= bUC_DEV_EN | bUC_INT_BUSY;                                      /* 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK */
    
    
	UDEV_CTRL |= bUD_PORT_EN;                                                   /* 允许USB端口 */
	USB_INT_FG = 0xFF;                                                          /* 清中断标志 */
	USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
	IE_USB = 1;
}

/*******************************************************************************
* Function Name  : HIDValueHandle
* Description    : HID键值处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void HIDValueHandle()
{
	UINT8 i;
    
	i = _getkey( );
	printf( "%c", (UINT8)i );
	if( WakeUpEnFlag == 0x03 )                                                  /* 主机已休眠 */
	{
		CH554USBDevWakeup( );                                                   /* 唤醒主机 */
	}

    switch( i )
    {
			/* 鼠标数据上传示例 */
			case 'L':                                                           /* 左键 */
            HIDMouse[ 0 ] = 0x01;
            while( Endp2Busy )
            {
                    /* 如果忙（上一包数据没有传上去），则等待 */
                ;
            }
            Enp2IntIn( HIDMouse, sizeof( HIDMouse ) );
				HIDMouse[ 0 ] = 0;                                              /* 抬起 */
            while( Endp2Busy )
            {
                    /* 如果忙（上一包数据没有传上去），则等待 */
                ; 
            }
            Enp2IntIn( HIDMouse, sizeof( HIDMouse ) );
            break;
			case 'R':                                                           /* 右键 */
            HIDMouse[ 0 ] = 0x02;
            while( Endp2Busy )
            {
                    /* 如果忙（上一包数据没有传上去），则等待 */
                ;
            }
            Enp2IntIn( HIDMouse, sizeof( HIDMouse ) );
				HIDMouse[ 0 ] = 0;                                              /* 抬起 */
            while( Endp2Busy )
            {
                    /* 如果忙（上一包数据没有传上去），则等待 */
                ;
            }
            Enp2IntIn( HIDMouse, sizeof( HIDMouse ) );
            break;
            
			/* 键盘数据上传示例 */
			case 'A':                                                           /* A键 */
				HIDKey[ 2 ] = 0x04;                                             /* 按键开始 */
            while( Endp1Busy )
            {
                    /* 如果忙（上一包数据没有传上去），则等待 */
                ;
            }
            Enp1IntIn( HIDKey, sizeof( HIDKey ) );
				HIDKey[2] = 0;                                                  /* 按键结束 */
            while( Endp1Busy )
            {
                    /* 如果忙（上一包数据没有传上去），则等待 */
                ;
            }
            Enp1IntIn( HIDKey, sizeof( HIDKey ) );
            break;
			case 'Q':                                                           /* CAP键 */
            HIDKey[ 2 ] = 0x39;
            while( Endp1Busy )
            {
                    /* 如果忙（上一包数据没有传上去），则等待 */
                ;
            }
            Enp1IntIn( HIDKey,sizeof( HIDKey ) );
				HIDKey[ 2 ] = 0;                                                /* 按键结束 */
            while( Endp1Busy )
            {
                    /* 如果忙（上一包数据没有传上去），则等待 */
                ;
            }
            Enp1IntIn( HIDKey, sizeof( HIDKey ) );
            break;
			default:                                                            /* 其他 */
            break;
    }
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main(void)
{
    /*****************************系统相关初始化*******************************/
	CfgFsys( );                                                                 /* CH543时钟选择配置 */
	mDelaymS( 20 );                                                             /* 修改主频等待内部晶振稳定,必加 */
	mInitSTDIO( );                                                              /* 串口0初始化 */
    printf("KM Device start ...\n");
    
    /* USB设备初始化 */    
	InitUSB_Device( );                                                          /* USB设备模式初始化 */
	EA = 1;                                                                     /* 允许单片机中断 */
    
	memset( HIDKey, 0, sizeof( HIDKey ) );                                      /* 清空缓冲区 */
	memset( HIDMouse, 0, sizeof( HIDMouse ) );
	
    /*****************************主循环***************************************/
	while(1)
	{
		if( Ready )
		{
            /* 串口0,程序会停在getkey函数等待接收一个字符 */
			HIDValueHandle( );
		}
	}
}
