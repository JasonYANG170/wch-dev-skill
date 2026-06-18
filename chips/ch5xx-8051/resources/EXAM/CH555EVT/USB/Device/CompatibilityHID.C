/********************************** (C) COPYRIGHT *******************************
* File Name          : CompatibilityHID.C
* Author             : WCH
* Version            : V1.5
* Date               : 2023/05/31
* Description        : CH555模拟HID兼容设备，支持中断独立上下传，支持控制端点上下传，支持全速
                       注意包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
/*硬件接口定义*/
/******************************************************************************
使用CH555 硬件按键接口
         CH555
         P4.0——KEY_L
*******************************************************************************/

#include "DEBUG.H"

#define THIS_ENDP0_SIZE         32                                                 //端点0最大包长度为32

#pragma NOAREGS

UINT8   Ready,UsbConfig;

volatile UINT8	D0SetupReqCode = 0xFF;											/* USB Setup包请求码 */
volatile UINT16	D0SetupLen = 0x00;												/* USB Setup包长度 */
volatile PUINT8	pD0Descr;

/*设备描述符*/
UINT8C DevDesc[] = {
	0x12,0x01,0x00,0x01,0x00,0x00,0x00,THIS_ENDP0_SIZE,
  0x86,0x1A,0x10,0xE0,0x00,0x00,0x01,0x02,
  0x00,0x01                  
};

/*HID类报表描述符*/
UINT8C HIDRepDesc[] =
{
    0x06, 0xA0,0xff,
    0x09, 0x01,
    0xa1, 0x01,                                                   //集合开始
    0x09, 0x01,                                                   //Usage Page  用法
    0x15, 0x00,                                                   //Logical  Minimun
    0x26, 0x00,0xff,                                              //Logical  Maximun
    0x75, 0x08,                                                   //Report Size
    0x95, THIS_ENDP0_SIZE,                                        //Report Counet
    0x81, 0x02,                                                   //Input
    0x09, 0x02,                                                   //Usage Page  用法
    0x75, 0x08,                                                   //Report Size
    0x95, THIS_ENDP0_SIZE,                                        //Report Counet
    0x91, 0x02,                                                   //Output
    0x09, 0x03,
    0x75, 0x08,
    0x95, THIS_ENDP0_SIZE,
    0xB1, 0x02,
    0xC0
};

UINT8C CfgDesc[] =
{
    0x09,0x02,0x29,0x00,0x01,0x01,0x04,0xA0,0x23,               //配置描述符
    0x09,0x04,0x00,0x00,0x02,0x03,0x00,0x00,0x00,               //接口描述符
    0x09,0x21,0x00,0x01,0x00,0x01,0x22,sizeof(HIDRepDesc)&0xFF,sizeof(HIDRepDesc)>>8,               //HID类描述符
    0x07,0x05,0x83,0x03,THIS_ENDP0_SIZE,0x00,0x01,              //端点描述符
    0x07,0x05,0x02,0x03,THIS_ENDP0_SIZE,0x00,0x01,              //端点描述符
};

// 语言描述符
UINT8C  MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };
// 厂家信息
UINT8C  MyManuInfo[] = { 0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0 };
// 产品信息
UINT8C  MyProdInfo[] = { 0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '5', 0, '5', 0 };                                //字符串描述符
UINT8X UserEp2Buf[64];                                            //用户数据定义
UINT8 EndpBusy = 0;

sbit Ep2InKey = P4^0;                                             //KEY_L(K5)按键

/*******************************************************************************
* Function Name  : USB_DeviceInterrupt
* Description    : USB中断服务程序
* Input          : None
* Return         : None
*******************************************************************************/
void	USB_DeviceInterrupt(void) interrupt INT_NO_USB using 1	
{
	UINT8  i, us, errflag;
	UINT16 len;

	us = USB_IF; 
	if( us & bUX_IF_D0_TRANS )            
	{
USB_DevIntNext:  
		/* D0数据传输完成 */
		switch (D0_STATUS & ( bUXS_SETUP_ACT | MASK_UXS_TOKEN | MASK_UXS_ENDP )){  // 分析操作令牌和端点号   或者D0_STATUS
			case UXS_TOKEN_OUT | 2:  // endpoint 2# 批量端点下传
			case bUXS_SETUP_ACT | UXS_TOKEN_OUT | 2:
				if( D0_STATUS & bUXS_TOG_OK ){  // 不同步的数据包将丢弃
						D0_EP2RES ^= bUEP_X_TOG;  // 手动翻转
					len = D0_RX_LEN;
					for(i = 0; i < len; i ++){
						( pUSB_BUF_DEV0 + UX_EP3_ADDR )[i] = ( pUSB_BUF_DEV0 + UX_EP2_ADDR )[i] ^ 0xFF;  // OUT数据取反到IN由计算机验证
					}
					D0_EP3T_L = len;
					D0_EP3RES = D0_EP3RES & ~ MASK_UEP_X_RES | UEP_X_RES_ACK;  // 允许端点3上传
				}
				break;

			case UXS_TOKEN_IN | 3:  // endpoint 3# 批量端点上传
			case bUXS_SETUP_ACT | UXS_TOKEN_IN | 3:
					D0_EP3T_L = 0;                                                     //预使用发送长度一定要清空
					D0_EP3RES ^= bUEP_X_TOG;                                            //手动翻转
					EndpBusy = 0 ;
					D0_EP3RES = D0_EP3RES & ~ MASK_UEP_X_RES | UEP_X_RES_NAK;           //默认应答NAK
				break;
			
			case UXS_TOKEN_IN | 0:  // endpoint 0# IN
			case bUXS_SETUP_ACT | UXS_TOKEN_IN | 0:
				switch(D0SetupReqCode){
					case USB_GET_DESCRIPTOR:
					case HID_GET_REPORT:
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
				D0_EP0RES ^= bUEP_R_TOG;                                                //同步标志位翻转
				break;
			
			default:
				if(( D0_STATUS & ( bUXS_SETUP_ACT | MASK_UXS_TOKEN ) ) == ( bUXS_SETUP_ACT | UXS_TOKEN_FREE )){  // endpoint 0# SETUP				 
					D0_EP0RES = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_NAK | UEP_T_RES_NAK; 
					D0SetupLen = ((UINT16)pD0_SETUP_REQ->wLengthH<<8) + pD0_SETUP_REQ->wLengthL;                                                                      // Ĭ��Ϊ�ɹ������ϴ�0����
					D0SetupReqCode = pD0_SETUP_REQ->bRequest;
					len = 0;  // 默认为成功并且上传0长度	
                    errflag = 0;                
					if(( pD0_SETUP_REQ->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD){  
						switch(D0SetupReqCode){							 
							case 0x01:                                                  //GetReport
								pD0Descr = UserEp2Buf;                                    //控制端点上传输据
								if(D0SetupLen >= THIS_ENDP0_SIZE)                         //大于端点0大小，需要特殊处理
								{
									len = THIS_ENDP0_SIZE;
								}
								else
								{
									len = D0SetupLen;
								}
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
						if(D0SetupLen > len)
						{
							D0SetupLen = len;    //限制总长度
						}
						len = D0SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : D0SetupLen;   //本次传输长度
						memcpy(pD0_EP0_BUF,pD0Descr,len);                                     //加载上传数据
						D0SetupLen -= len;
						pD0Descr += len;
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
									case 0x22:                                               //报表描述符
										pD0Descr = (PUINT8)( &HIDRepDesc[0] );                                 //数据准备上传
										len = sizeof(HIDRepDesc);
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
								if (UsbConfig) Ready = 1;									
								break;
							
							case USB_CLEAR_FEATURE:
								if ((pD0_SETUP_REQ->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP){  // 端点
									switch(pD0_SETUP_REQ->wIndexL){
										case 0x83:
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
								else errflag = 0xFF;  // 不是端点不支持
								break;
							case USB_SET_FEATURE:                                       /* Set Feature */
								if((pD0_SETUP_REQ->bRequestType & 0x1F ) == 0x00)        /* 设置设备 */
								{
									if(((( UINT16 )pD0_SETUP_REQ->wValueH << 8 ) | pD0_SETUP_REQ->wValueL ) == 0x01)
									{
										if(CfgDesc[7] & 0x20)
										{
												/* 设置唤醒使能标志 */
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
											case 0x83:
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
		printf("rst\n");  // 睡眠状态
		D0_EP0RES = UEP_R_RES_ACK | UEP_T_RES_NAK;
		D0_EP2RES = UEP_X_RES_ACK;
		D0_EP3RES = UEP_X_RES_NAK; 
		D0_EP_MOD = bUX_EP2O_EN|bUX_DEV_EN; 
		
		D0_ADDR = 0;
		HB_ADDR = 0x7F;             
		USB_IF = 0xFF;
		EndpBusy = 0;
		Ready = 0;
		
		USB_IE = bUX_IE_SUSPEND | bUX_IE_TRANSFER | bUX_IE_BUS_RST;
		
		USB_IF = bUX_IF_BUS_RST;		                                                         //清中断标志
	}
	else if( us & bUX_IF_SUSPEND ){  // USB总线挂起/唤醒完成
		USB_IF = bUX_IF_SUSPEND;
		D0_STATUS = 0;
		if( us & bUX_SUSPEND){  // 挂起
		printf("sleep\n");  // 睡眠状态
		}
	else{  // 唤醒
    printf("weak\n");  // 睡眠状态	
		}
	}
	else{  // 意外的中断,不可能发生的情况
		printf("UnknownERR\n");
		D0_STATUS = 0;
	}
		/* 再次判断是否还有USB中断需要处理 */
	if( USB_IF & bUX_IF_D0_TRANS ) 
	{
		goto USB_DevIntNext;
	} 
	
}

/*******************************************************************************
* Function Name  : Enp3BlukIn
* Description    : USB设备模式端点3的批量上传
* Input          : None
* Return         : None
*******************************************************************************/
void Enp3BlukIn( UINT8 *buf,UINT8 len)
{
	memcpy(pUSB_BUF_DEV0 + UX_EP3_ADDR, buf, len); 					
    if( Ready )
    {
        D0_EP3T_L = len;                                                          //上传最大包长度
        D0_EP3RES = D0_EP3RES & ~ MASK_UEP_X_RES | UEP_X_RES_ACK;                 //有数据时上传数据并应答ACK
        EndpBusy = 1;                                                          
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
		
	
	D0_EP_MOD = bUX_EP2O_EN | bUX_DEV_EN;	
  D0_EP3T_L = 0;                           							//预使用发送长度一定要清空 	
	D0_ADDR = 0;
	HB_ADDR = 0x7F;             												/* 设置该地址,将收到的数据直接转发到D0 */
	USB_IF = 0xFF;
	USB_IE = bUX_IE_SUSPEND | bUX_IE_TRANSFER | bUX_IE_BUS_RST;
	USB_CTRL = bUX_DP_PU_EN;
	IE_USB = 1;
	
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Return         : None
*******************************************************************************/
void main(void)
{
	UINT8 i;
	CfgFsys( );                                                            //CH555时钟选择配置
	mDelaymS(20);                                                          //修改主频等待内部晶振稳定,必加
	mInitSTDIO( );                                                         //串口0初始化
	printf("start ...\n");

	for(i=0; i<64; i++)                                                    //准备演示数据
	{
		UserEp2Buf[i] = i;
	}
	InitUSB_Device();                                                       //USB设备模式初始化
	EA = 1;                                                                //允许单片机中断
	
	while(1)
	{
		if((Ep2InKey==0))
		{				
			while( EndpBusy )
			{				
					;    //如果忙（上一包数据没有传上去），则等待。
			}
			Enp3BlukIn( UserEp2Buf,THIS_ENDP0_SIZE );
		}
		mDelaymS( 100 );                                                   //模拟单片机做其它事	 
	}
}
