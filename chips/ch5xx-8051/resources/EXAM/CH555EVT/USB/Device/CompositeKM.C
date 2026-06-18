/********************************** (C) COPYRIGHT *******************************
* File Name          : CompositeKM.C
* Author             : WCH
* Version            : V1.5
* Date               : 2023/05/31
* Description        : CH555模拟USB键鼠复合设备,支持类命令,支持唤醒
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
#include "DEBUG.H"

#define THIS_ENDP0_SIZE         8

#define ENDP1_IN_SIZE           8                                              //键盘端点数据包大小
#define ENDP2_IN_SIZE           4                                              //鼠标端点数据包大小

UINT8   SetupReq,Ready,UsbConfig;

#pragma NOAREGS

volatile UINT8	D0SetupReqCode = 0xFF;											/* USB Setup包请求码 */
volatile UINT16	D0SetupLen = 0x00;												/* USB Setup包长度 */
volatile PUINT8	pD0Descr;

/*设备描述符*/
UINT8C DevDesc[] = { 0x12,0x01,0x10,0x01,0x00,0x00,0x00,THIS_ENDP0_SIZE,
                     0x86,0x1a,0xe1,0xe6,0x00,0x01,0x01,0x02,
                     0x00,0x01
                   };
/*字符串描述符*/
UINT8C  MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };                                    // 语言描述符
UINT8C  MyManuInfo[] = { 0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0 }; // 厂家信息
UINT8C  MyProdInfo[] = { 0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '5', 0, '5', 0 };         // 产品信息
/*HID类报表描述符*/
UINT8C KeyRepDesc[] =
{
    0x05,0x01,0x09,0x06,0xA1,0x01,0x05,0x07,
    0x19,0xe0,0x29,0xe7,0x15,0x00,0x25,0x01,
    0x75,0x01,0x95,0x08,0x81,0x02,0x95,0x01,
    0x75,0x08,0x81,0x01,0x95,0x03,0x75,0x01,
    0x05,0x08,0x19,0x01,0x29,0x03,0x91,0x02,
    0x95,0x05,0x75,0x01,0x91,0x01,0x95,0x06,
    0x75,0x08,0x26,0xff,0x00,0x05,0x07,0x19,
    0x00,0x29,0x91,0x81,0x00,0xC0
};
UINT8C MouseRepDesc[] =
{
    0x05,0x01,0x09,0x02,0xA1,0x01,0x09,0x01,
    0xA1,0x00,0x05,0x09,0x19,0x01,0x29,0x03,
    0x15,0x00,0x25,0x01,0x75,0x01,0x95,0x03,
    0x81,0x02,0x75,0x05,0x95,0x01,0x81,0x01,
    0x05,0x01,0x09,0x30,0x09,0x31,0x09,0x38,
    0x15,0x81,0x25,0x7f,0x75,0x08,0x95,0x03,
    0x81,0x06,0xC0,0xC0
};
/*配置描述符*/
UINT8C CfgDesc[] =
{
    0x09,0x02,0x3b,0x00,0x02,0x01,0x00,0xA0,0x32,                    //配置描述符

    0x09,0x04,0x00,0x00,0x01,0x03,0x01,0x01,0x00,                    //接口描述符,键盘
    0x09,0x21,0x11,0x01,0x00,0x01,0x22,sizeof(KeyRepDesc)&0xFF,sizeof(KeyRepDesc)>>8,//HID类描述符
    0x07,0x05,0x81,0x03,ENDP1_IN_SIZE,0x00,0x0a,                       //端点描述符

    0x09,0x04,0x01,0x00,0x01,0x03,0x01,0x02,0x00,                    //接口描述符,鼠标
    0x09,0x21,0x10,0x01,0x00,0x01,0x22,sizeof(MouseRepDesc)&0xFF,sizeof(MouseRepDesc)>>8,//HID类描述符
    0x07,0x05,0x82,0x03,ENDP2_IN_SIZE,0x00,0x0a                        //端点描述符
};
/*键盘数据*/
UINT8 HIDKey[ENDP1_IN_SIZE];
/*鼠标数据*/
UINT8 HIDMouse[ENDP2_IN_SIZE];
UINT8 Endp1Busy = 0;                                                 //传输完成控制标志位
UINT8 Endp2Busy = 0;
UINT8 WakeUpEnFlag = 0;                                              //远程唤醒使能标志

/*******************************************************************************
* Function Name  : CH555USBDevWakeup
* Description    : CH555设备唤醒主机
* Input          : None
* Return         : None
*******************************************************************************/
void CH555USBDevWakeup(void)
{
	UINT8  save;
	
	save = USB_CTRL;
	USB_CTRL ^= bUX_DP_PU_EN | bUX_DM_PU_EN;
	mDelaymS( 10 );  
	USB_CTRL = save;
}

/*******************************************************************************
* Function Name  : Enp1IntIn
* Description    : USB设备端点1的中断上传
* Input          : *buf: 上传数据
*                  len:上传数据长度
* Return         : None
*******************************************************************************/
void Enp1IntIn( UINT8 *buf,UINT8 len )
{
	memcpy( pUSB_BUF_DEV0 + UX_EP1_ADDR, buf, len );                           //加载上传数据
    if( Ready )
    {
        D0_EP1T_L = len;                                                           //上传数据长度
        D0_EP1RES = D0_EP1RES & ~ MASK_UEP_X_RES | UEP_X_RES_ACK;                  //有数据时上传数据并应答ACK
        Endp1Busy = 1;                                                           
    }
}
/*******************************************************************************
* Function Name  : Enp2IntIn
* Description    : USB设备端点2的中断上传
* Input          : *buf: 上传数据
*                  len:上传数据长度
* Return         : None
*******************************************************************************/
void Enp2IntIn( UINT8 *buf,UINT8 len )
{
	memcpy( pUSB_BUF_DEV0 + UX_EP2_ADDR, buf, len );                           //加载上传数据
    if( Ready )
    {
        D0_EP2T_L = len;                                                           //上传数据长度
        D0_EP2RES = D0_EP2RES & ~ MASK_UEP_X_RES | UEP_X_RES_ACK;                  //有数据时上传数据并应答ACK
        Endp2Busy = 1;                                                             
    }
}

/*******************************************************************************
* Function Name  : USB_DeviceInterrupt
* Description    : USB中断服务程序
* Input          : None
* Return         : None
*******************************************************************************/
void	USB_DeviceInterrupt(void) interrupt INT_NO_USB using 1	
{
	UINT8	us, errflag;
	UINT16 len;

	us = USB_IF; 
	if( us & bUX_IF_D0_TRANS )            
	{
USB_DevIntNext:  
		/* D0数据传输完成 */
		switch (D0_STATUS & ( bUXS_SETUP_ACT | MASK_UXS_TOKEN | MASK_UXS_ENDP )){  // 分析操作令牌和端点号   或者D0_STATUS
			case UXS_TOKEN_IN | 2:  // endpoint 2# 批量端点上传
			case bUXS_SETUP_ACT | UXS_TOKEN_IN | 2:
					D0_EP2T_L = 0;                                                     //预使用发送长度一定要清空
					D0_EP2RES ^= bUEP_X_TOG;                                            //手动翻转
					Endp2Busy = 0 ;
					D0_EP2RES = D0_EP2RES & ~ MASK_UEP_X_RES | UEP_X_RES_NAK;           //默认应答NAK
				break;
			
			case UXS_TOKEN_IN | 1:  // endpoint 1# 批量端点上传
			case bUXS_SETUP_ACT | UXS_TOKEN_IN | 1:
				D0_EP1T_L = 0;                                                     //预使用发送长度一定要清空
				D0_EP1RES ^= bUEP_X_TOG;                                            //手动翻转
				Endp1Busy = 0 ;
				D0_EP1RES = D0_EP1RES & ~ MASK_UEP_X_RES | UEP_X_RES_NAK;           //默认应答NAK
			break;
			
			case UXS_TOKEN_IN | 0:  // endpoint 0# IN
			case bUXS_SETUP_ACT | UXS_TOKEN_IN | 0:
				switch(D0SetupReqCode){
					case USB_GET_DESCRIPTOR:
						len = D0SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : D0SetupLen;  // 本次传输长度
						memcpy( pD0_EP0_BUF, pD0Descr, len );  /* 加载上传数据 */
						D0SetupLen -= len;
						pD0Descr += len;
						D0_EP0T_L = len;
						D0_EP0RES ^= bUEP_T_TOG;  // 翻转
						break;
					case USB_SET_ADDRESS:
						D0_ADDR = D0SetupLen;
						D0_EP0RES = UEP_R_RES_ACK | UEP_T_RES_NAK;
						break;
					default:
						D0_EP0T_L = 0;  // 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
						D0_EP0RES = UEP_R_RES_ACK | UEP_T_RES_NAK;
						break;
				}
				break;
				
			case UXS_TOKEN_OUT | 0:  // endpoint 0# OUT
			case bUXS_SETUP_ACT | UXS_TOKEN_OUT | 0:
				len = D0_RX_LEN;
				if(D0SetupReqCode == 0x09)
				{
					if(pD0_EP0_BUF[0])
					{
						printf("Light on Num Lock LED!\n");
					}
					else if(pD0_EP0_BUF[0] == 0)
					{
						printf("Light off Num Lock LED!\n");
					}
				}		
				D0_EP0RES ^= bUEP_R_TOG;                                                //同步标志位翻转
				break;
			
			default:
				if(( D0_STATUS & ( bUXS_SETUP_ACT | MASK_UXS_TOKEN ) ) == ( bUXS_SETUP_ACT | UXS_TOKEN_FREE )){  // endpoint 0# SETUP				 
					D0_EP0RES = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK; 
					D0SetupLen = ((UINT16)pD0_SETUP_REQ->wLengthH<<8) + pD0_SETUP_REQ->wLengthL;
					D0SetupReqCode = pD0_SETUP_REQ->bRequest;
					len = 0;  // 默认为成功并且上传0长度
                    errflag = 0;                    
					if(( pD0_SETUP_REQ->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD){  
						switch(D0SetupReqCode){							 
							case 0x01:                                                  //GetReport
								break;
							case 0x02:                                                   //GetIdle
								break;
							case 0x03:                                                   //GetProtocol
								break;
							case 0x09:                                                   //SetReport
								break;
							case 0x0A:                                                   //SetIdle
								break;
							case 0x0B:                                                   //SetProtocol
								break;
							default:
								errflag = 0xFF;                                                    /*命令不支持*/
								break;
						}
					}
					else{  // 标准请求	
							
						switch(D0SetupReqCode){  // 请求码
							case USB_GET_DESCRIPTOR:
								switch(pD0_SETUP_REQ->wValueH){
									case 1:  // 设备描述符
										pD0Descr = (PUINT8)(&DevDesc[0]);
										len = sizeof(DevDesc);
										break;
									case 2:  // 配置描述符
										pD0Descr = (PUINT8)(&CfgDesc[0]);
										len = sizeof(CfgDesc);
										break;
									case 3:  // 字符串描述符
										switch(pD0_SETUP_REQ->wValueL){
											case 1:
												pD0Descr = (PUINT8)(&MyManuInfo[0]);
												len = sizeof(MyManuInfo);
												break;
											case 2:
												pD0Descr = (PUINT8)(&MyProdInfo[0]);
												len = sizeof(MyProdInfo);
												break;
											case 0:
												pD0Descr = (PUINT8)(&MyLangDescr[0]);
												len = sizeof(MyLangDescr);
												break;
											default:
												errflag = 0xFF;  // 不支持的字符串描述符
												break;
										}
										break;
									case USB_DESCR_TYP_REPORT:
										if(pD0_SETUP_REQ->wIndexL == 0)                  //接口0报表描述符
										{
											pD0Descr = (PUINT8)(&KeyRepDesc[0]);                         //数据准备上传
											len = sizeof(KeyRepDesc);
										}
										else if(pD0_SETUP_REQ->wIndexL == 1)               //接口1报表描述符
										{
											pD0Descr = (PUINT8)(&MouseRepDesc[0]);                         //数据准备上传
											len = sizeof(MouseRepDesc);
										}
										else
										{
											errflag = 0xFF;                                 //本程序只有2个接口，这句话正常不可能执行
										}
										break;
										default:
										errflag = 0xFF;  // 不支持的描述符类型
										break;
								}
								
								if(D0SetupLen > len) D0SetupLen = len;  // 限制总长度
								len = D0SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : D0SetupLen;  // 本次传输长度
								memcpy(pD0_EP0_BUF, pD0Descr, len);  /* 加载上传数据 */
								D0SetupLen -= len;
								pD0Descr += len;
								break;
								
							case USB_SET_ADDRESS:
								D0SetupLen = pD0_SETUP_REQ->wValueL;  // 暂存USB设备地址
								break;
							
							case USB_GET_CONFIGURATION:
								pD0_EP0_BUF[0] = UsbConfig;
								if (D0SetupLen >= 1) len = 1;
								break;
							
							case USB_SET_CONFIGURATION:
								UsbConfig = pD0_SETUP_REQ->wValueL;
								if(UsbConfig)
								{
									printf("SET CONFIG.\n");
									Ready = 1;                                                   //set config命令一般代表usb枚举完成的标志
								}
								break;
							
							case USB_CLEAR_FEATURE:
								if ((pD0_SETUP_REQ->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP){  // 端点
									switch(pD0_SETUP_REQ->wIndexL){
										case 0x82:
											D0_EP3RES = D0_EP3RES & ~ ( bUEP_X_TOG | MASK_UEP_X_RES ) | UEP_X_RES_NAK;
											break;
										case 0x02:
											D0_EP2RES = D0_EP2RES & ~ ( bUEP_X_TOG | MASK_UEP_X_RES ) | UEP_X_RES_ACK;
											break;
										case 0x81:
											D0_EP1RES = D0_EP1RES & ~ ( bUEP_X_TOG | MASK_UEP_X_RES ) | UEP_X_RES_NAK;
											break;
										case 0x01:
											D0_EP1RES = D0_EP1RES & ~ ( bUEP_X_TOG | MASK_UEP_X_RES ) | UEP_X_RES_ACK;
											break;
										default:
											errflag = 0xFF;  // 不支持的端点
											break;
									}
								}
								else if((pD0_SETUP_REQ->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_DEVICE)// 设备
								{
									WakeUpEnFlag &= ~0x01;
									printf("Wake\n");
									break;
								}
								else errflag = 0xFF;  // 不是端点不支持
								break;
							case USB_SET_FEATURE:                                       /* Set Feature */
								if((pD0_SETUP_REQ->bRequestType & 0x1F ) == 0x00)        /* 设置设备 */
								{
									if(((( UINT16 )pD0_SETUP_REQ->wValueH << 8 ) | pD0_SETUP_REQ->wValueL ) == 0x01)
									{
										if(CfgDesc[7] & 0x20)
										{
											WakeUpEnFlag |= 0x01;                                   /* 设置唤醒使能标志 */
											printf("Enable Remote Wakeup.\n");
										}
										else
										{
											errflag = 0xFF;                                  /* 操作失败 */
										}
									}
									else
									{
										errflag = 0xFF;                                      /* 操作失败 */
									}
								}
								else if((pD0_SETUP_REQ->bRequestType & 0x1F ) == 0x02)   /* 设置端点 */
								{
									if((((UINT16)pD0_SETUP_REQ->wValueH << 8) | pD0_SETUP_REQ->wValueL) == 0x00)
									{
										switch(((UINT16)pD0_SETUP_REQ->wIndexH << 8) | pD0_SETUP_REQ->wIndexL){
											case 0x82:
												D0_EP3RES = D0_EP3RES & (~bUEP_X_TOG) | UEP_X_RES_STALL;/* 设置端点2 IN STALL */
												break;
											case 0x02:
												D0_EP2RES = D0_EP2RES & (~bUEP_X_TOG) | UEP_X_RES_STALL;/* 设置端点2 OUT STALL */
												break;
											case 0x81:
												D0_EP1RES = D0_EP1RES & (~bUEP_X_TOG) | UEP_X_RES_STALL;/* 设置端点1 IN STALL */
												break;
											default:
												errflag = 0xFF;                                  /* 操作失败 */
												break;
										}
									}
									else
									{
										errflag = 0xFF;                                       /* 操作失败 */
									}
								}
								else
								{
									errflag = 0xFF;                                           /* 操作失败 */
								}
								break;
							case USB_GET_INTERFACE:
								pD0_EP0_BUF[0] = 0x00;
								if (D0SetupLen >= 1) len = 1;
								break;
							case USB_GET_STATUS:
								pD0_EP0_BUF[0] = 0x00;
								pD0_EP0_BUF[1] = 0x00;
								if (D0SetupLen >= 2) len = 2;
								else len = D0SetupLen;
								break;
							default:
								errflag = 0xFF;  // 操作失败
								printf("ErrEp0ReqCode=%02X\n",(UINT16)D0SetupReqCode);
								break;
						}
					}

					if(errflag == 0xFF){  // 操作失败
						D0_EP0RES = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;  // STALL
					}
					else if(len <= THIS_ENDP0_SIZE){  // 上传数据或者状态阶段返回0长度包
						D0_EP0T_L = len;
						D0_EP0RES = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;  // 默认数据包是DATA1
					}
					else{  // 下传数据或其它
						D0_EP0T_L = 0;  // 虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
						D0_EP0RES = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;  // 默认数据包是DATA1
					}
					break;
				}		
			else {
				printf("Err\n");
				break;
			}
		}
		D0_STATUS =0;
	}
	else if(us & bUX_IF_BUS_RST)
	{  // USB总线复位
		printf("rst\n");
		
		D0_EP0RES = UEP_R_RES_ACK | UEP_T_RES_NAK;
		D0_EP1RES = UEP_X_RES_NAK;
		D0_EP2RES = UEP_X_RES_NAK; 
		D0_EP_MOD = bUX_DEV_EN; 
		
		D0_ADDR = 0;
		HB_ADDR = 0x7F;             
		USB_IF = 0xFF;
		Ready = 0;
        WakeUpEnFlag = 0;
        Endp1Busy = 0;
        Endp2Busy = 0;
		
		USB_IE = bUX_IE_SUSPEND | bUX_IE_TRANSFER | bUX_IE_BUS_RST;
		
		USB_IF = bUX_IF_BUS_RST;		                                                         //清中断标志
	}
	else if( us & bUX_IF_SUSPEND ){  // USB总线挂起/唤醒完成
		USB_IF = bUX_IF_SUSPEND;
		D0_STATUS = 0;
		if( us & bUX_SUSPEND){  // 挂起
			WakeUpEnFlag |= 0x02;
			printf("sleep\n");
		}
	else{  // 唤醒
			WakeUpEnFlag &= ~0x02;
			printf("weak\n"); 
		}
	}
	else{  // 意外的中断,不可能发生的情况
		printf("UnknownInt\n");
		D0_STATUS = 0;
	}
		/* 再次判断是否还有USB中断需要处理 */
	if( USB_IF & bUX_IF_D0_TRANS ) 
	{
		goto USB_DevIntNext;
	} 
	
}

/*******************************************************************************
* Function Name  : InitUSB_Device
* Description    : 初始化USB设备
* Input          : None
* Return         : None
*******************************************************************************/
void	InitUSB_Device( void )  
{	
	IE_USB = 0;
	USB_CTRL = 0;
		
	D0_EP_MOD = bUX_DEV_EN;
	
  D0_EP1T_L = 0;                           							//预使用发送长度一定要清空 	
	D0_EP2T_L = 0;
	D0_ADDR = 0;
	HB_ADDR = 0x7F;             												/* 设置该地址,将收到的数据直接转发到D0 */
	USB_IF = 0xFF;
	USB_IE = bUX_IE_SUSPEND | bUX_IE_TRANSFER | bUX_IE_BUS_RST;
	USB_CTRL = bUX_DP_PU_EN;
	IE_USB = 1;
	
}

/*******************************************************************************
* Function Name  : HIDValueHandle
* Description    : HID键值处理
* Input          : None
* Return         : None
*******************************************************************************/
void HIDValueHandle()
{
	UINT8 i;
	i = _getkey( );
	printf( "%c", (UINT8)i );
	if( WakeUpEnFlag == 0x03 )                                                   //主机已休眠
	{
		CH555USBDevWakeup();                                               //唤醒主机
	}
    
    switch(i)
    {
			//鼠标数据上传示例
			case 'L':                                                        //左键
            HIDMouse[0] = 0x01;
            while( Endp2Busy )
            {
						;    //如果忙（上一包数据没有传上去），则等待。
            }
            Enp2IntIn(HIDMouse,sizeof(HIDMouse));
				HIDMouse[0] = 0;                                               //抬起
            while( Endp2Busy )
            {
						;    //如果忙（上一包数据没有传上去），则等待。
            }
            Enp2IntIn(HIDMouse,sizeof(HIDMouse));
            break;
			case 'R':                                                        //右键
            HIDMouse[0] = 0x02;
            while( Endp2Busy )
            {
						;    //如果忙（上一包数据没有传上去），则等待。
            }
            Enp2IntIn(HIDMouse,sizeof(HIDMouse));
				HIDMouse[0] = 0;                                             //抬起
            while( Endp2Busy )
            {
						;    //如果忙（上一包数据没有传上去），则等待。
            }
            Enp2IntIn(HIDMouse,sizeof(HIDMouse));
            break;
            
			//键盘数据上传示例
			case 'A':                                                         //A键
				HIDKey[2] = 0x04;                                             //按键开始
            while( Endp1Busy )
            {
						;    //如果忙（上一包数据没有传上去），则等待。
            }
            Enp1IntIn(HIDKey,sizeof(HIDKey));
				HIDKey[2] = 0;                                                //按键结束
            while( Endp1Busy )
            {
						;    //如果忙（上一包数据没有传上去），则等待。
            }
            Enp1IntIn(HIDKey,sizeof(HIDKey));
            break;
			case 'Q':                                                         //CAP键
            HIDKey[2] = 0x39;
            while( Endp1Busy )
            {
						;    //如果忙（上一包数据没有传上去），则等待。
            }
            Enp1IntIn(HIDKey,sizeof(HIDKey));
				HIDKey[2] = 0;                                                //按键结束
            while( Endp1Busy )
            {
						;    //如果忙（上一包数据没有传上去），则等待。
            }
            Enp1IntIn(HIDKey,sizeof(HIDKey));
            break;
			default:                                                          //其他
            break;
    }
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Return         : None
*******************************************************************************/
void main(void)
{
	CfgFsys( );                                                            //CH555时钟选择配置
	mDelaymS(20);                                                          //修改主频等待内部晶振稳定,必加
	mInitSTDIO( );                                                         //串口0初始化
	printf("KM Device start ...\n");
	InitUSB_Device();                                                       //USB设备模式初始化
	EA = 1;                                                                //允许单片机中断
	memset(HIDKey,0,sizeof(HIDKey));                                     //清空缓冲区
	memset(HIDMouse,0,sizeof(HIDMouse));
	
	while(1)
	{
		if(Ready)
		{
			HIDValueHandle();                                                 //串口0,程序会停在getkey函数等待接收一个字符
		}
	}
}
