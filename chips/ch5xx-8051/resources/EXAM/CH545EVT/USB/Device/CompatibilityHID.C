/********************************** (C) COPYRIGHT *******************************
* File Name          : CompatibilityHID.C
* Author             : WCH
* Version            : V1.5
* Date               : 2023/05/31
* Description        : CH545模拟HID兼容设备，支持中断上下传，支持控制端点上下传，支持全速传输
                       注意包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "DEBUG.H"

#define THIS_ENDP0_SIZE         64
#define ENDP2_IN_SIZE           64
#define ENDP2_OUT_SIZE          64

#pragma NOAREGS

UINT8X  Ep0Buffer[MIN(64,THIS_ENDP0_SIZE+2)]  _at_ 0x0000;                 //端点0 OUT&IN缓冲区,必须是偶地址
UINT8X  Ep2Buffer[MIN(64,ENDP2_IN_SIZE+2)+MIN(64,ENDP2_OUT_SIZE+2)]  _at_ MIN(64,THIS_ENDP0_SIZE+2); //端点2 IN&OUT缓冲区,必须是偶地址
UINT8   Ready,UsbConfig;

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
    0x07,0x05,0x82,0x03,ENDP2_IN_SIZE,0x00,0x01,              //端点描述符
    0x07,0x05,0x02,0x03,ENDP2_OUT_SIZE,0x00,0x01,              //端点描述符
};

// 语言描述符
UINT8C  MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };
// 厂家信息
UINT8C  MyManuInfo[] = { 0x0E, 0x03, 'w', 0, 'c', 0, 'h', 0, '.', 0, 'c', 0, 'n', 0 };
// 产品信息
UINT8C  MyProdInfo[] = { 0x0C, 0x03, 'C', 0, 'H', 0, '5', 0, '4', 0, '5', 0 };                                //字符串描述符
UINT8X UserEp2Buf[64];                                            //用户数据定义
UINT8 Endp2Busy = 0;
#define UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)

sbit Ep2InKey = P1^4;                                             //K1按键

/*******************************************************************************
* Function Name  : USB_DeviceInterrupt
* Description    : USB中断服务程序
* Input          : None
* Return         : None
*******************************************************************************/
void	USB_DeviceInterrupt(void) interrupt INT_NO_USB using 1	
{
	UINT8	i, errflag;
    UINT16  len;
	static	UINT8	SetupReqCode;
    static	UINT16	SetupLen;
	static	PUINT8	pDescr;

	if(UIF_TRANSFER){  // USB传输完成
USB_DevIntNext:      
		switch (USB_INT_ST & ( bUIS_SETUP_ACT | MASK_UIS_TOKEN | MASK_UIS_ENDP )){  // 分析操作令牌和端点号
			case UIS_TOKEN_OUT | 2:  // endpoint 2# 批量端点下传
			case bUIS_SETUP_ACT | UIS_TOKEN_OUT | 2:
				if(U_TOG_OK){  // 不同步的数据包将丢弃
				    UEP2_CTRL ^= bUEP_R_TOG;  // 手动翻转
					len = USB_RX_LEN;
					for(i = 0; i < len; i ++){
						Ep2Buffer[MAX_PACKET_SIZE+i] = Ep2Buffer[i] ^ 0xFF;  // OUT数据取反到IN由计算机验证
					}
					UEP2_T_LEN = len;
					UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;  // 允许上传
				}
				break;
			case UIS_TOKEN_IN | 2:  // endpoint 2# 批量端点上传
			case bUIS_SETUP_ACT | UIS_TOKEN_IN | 2:
					UEP2_T_LEN = 0;                                                     //预使用发送长度一定要清空
			        UEP2_CTRL ^= bUEP_T_TOG;                                            //手动翻转
					Endp2Busy = 0 ;
					UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK
				break;
			case UIS_TOKEN_IN | 0:  // endpoint 0# IN
			case bUIS_SETUP_ACT | UIS_TOKEN_IN | 0:
				switch(SetupReqCode){
					case USB_GET_DESCRIPTOR:
					case HID_GET_REPORT:
						len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;  // 本次传输长度
						memcpy( Ep0Buffer, pDescr, len );  /* 加载上传数据 */
						SetupLen -= len;
						pDescr += len;
						UEP0_T_LEN = len;
						UEP0_CTRL ^= bUEP_T_TOG;  // 翻转
						break;
					case USB_SET_ADDRESS:
						USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
						UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
						break;
					default:
						UEP0_T_LEN = 0;  // 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
						UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
						break;
				}
				break;
			case UIS_TOKEN_OUT | 0:  // endpoint 0# OUT
			case bUIS_SETUP_ACT | UIS_TOKEN_OUT | 0:
				len = USB_RX_LEN;
				UEP0_CTRL ^= bUEP_R_TOG;                                                //同步标志位翻转
				break;
			default:
				if(( USB_INT_ST & ( bUIS_SETUP_ACT | MASK_UIS_TOKEN ) ) == ( bUIS_SETUP_ACT | UIS_TOKEN_FREE )){  // endpoint 0# SETUP
					UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;
					SetupLen = ((UINT16)UsbSetupBuf->wLengthH<<8) + UsbSetupBuf->wLengthL;                                                                      // Ĭ��Ϊ�ɹ������ϴ�0����
					SetupReqCode = UsbSetupBuf->bRequest;
					len = 0;  // 默认为成功并且上传0长度
                    errflag = 0;
					if(( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD){  
						switch(SetupReqCode){							 
							case 0x01:                                                  //GetReport
								pDescr = UserEp2Buf;                                    //控制端点上传输据
								if(SetupLen >= THIS_ENDP0_SIZE)                         //大于端点0大小，需要特殊处理
								{
									len = THIS_ENDP0_SIZE;
								}
								else
								{
									len = SetupLen;
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
						if(SetupLen > len)
						{
							SetupLen = len;    //限制总长度
						}
						len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;   //本次传输长度
						memcpy(Ep0Buffer,pDescr,len);                                     //加载上传数据
						SetupLen -= len;
						pDescr += len;
					}
					else{  // 标准请求						
						switch(SetupReqCode){  // 请求码
							case USB_GET_DESCRIPTOR:
								switch(UsbSetupBuf->wValueH){
									case 1:  // 设备描述符
										pDescr = (PUINT8)(&DevDesc[0]);
										len = sizeof(DevDesc);
										break;
									case 2:  // 配置描述符
										pDescr = (PUINT8)(&CfgDesc[0]);
										len = sizeof(CfgDesc);
										break;
									case 3:  // 字符串描述符
										switch(UsbSetupBuf->wValueL){
											case 1:
												pDescr = (PUINT8)(&MyManuInfo[0]);
												len = sizeof(MyManuInfo);
												break;
											case 2:
												pDescr = (PUINT8)(&MyProdInfo[0]);
												len = sizeof(MyProdInfo);
												break;
											case 0:
												pDescr = (PUINT8)(&MyLangDescr[0]);
												len = sizeof(MyLangDescr);
												break;
											default:
												errflag = 0xFF;  // 不支持的字符串描述符
												break;
										}
										break;
									case 0x22:                                               //报表描述符
										pDescr = HIDRepDesc;                                 //数据准备上传
										len = sizeof(HIDRepDesc);
									break;
									default:
										errflag = 0xFF;  // 不支持的描述符类型
										break;
								}
								if(SetupLen > len) SetupLen = len;  // 限制总长度
								len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;  // 本次传输长度
								memcpy(Ep0Buffer, pDescr, len);  /* 加载上传数据 */
								SetupLen -= len;
								pDescr += len;
								break;
							case USB_SET_ADDRESS:
								SetupLen = UsbSetupBuf->wValueL;  // 暂存USB设备地址
								break;
							case USB_GET_CONFIGURATION:
								Ep0Buffer[0] = UsbConfig;
								if (SetupLen >= 1) len = 1;
								break;
							case USB_SET_CONFIGURATION:
								UsbConfig = UsbSetupBuf->wValueL;
								if (UsbConfig) Ready = 1;									
								break;
							case USB_CLEAR_FEATURE:
								if ((UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP){  // 端点
									switch(UsbSetupBuf->wIndexL){
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
											errflag = 0xFF;  // 不支持的端点
											break;
									}
								}
								else errflag = 0xFF;  // 不是端点不支持
								break;
							case USB_SET_FEATURE:                                       /* Set Feature */
								if((UsbSetupBuf->bRequestType & 0x1F ) == 0x00)        /* 设置设备 */
								{
									if(((( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01)
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
								else if((UsbSetupBuf->bRequestType & 0x1F ) == 0x02)   /* 设置端点 */
								{
									if((((UINT16)UsbSetupBuf->wValueH << 8) | UsbSetupBuf->wValueL) == 0x00)
									{
										switch(((UINT16)UsbSetupBuf->wIndexH << 8) | UsbSetupBuf->wIndexL){
											case 0x82:
												UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点2 IN STALL */
												break;
											case 0x02:
												UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* 设置端点2 OUT STALL */
												break;
											case 0x81:
												UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点1 IN STALL */
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
								Ep0Buffer[0] = 0x00;
								if (SetupLen >= 1) len = 1;
								break;
							case USB_GET_STATUS:
								Ep0Buffer[0] = 0x00;
								Ep0Buffer[1] = 0x00;
								if (SetupLen >= 2) len = 2;
								else len = SetupLen;
								break;
							default:
								errflag = 0xFF;  // 操作失败
								printf("ErrEp0ReqCode=%02X\n",(UINT16)SetupReqCode);
								break;
						}
					}
					if(errflag == 0xFF){  // 操作失败
						UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;  // STALL
					}
					else if(len <= THIS_ENDP0_SIZE){  // 上传数据或者状态阶段返回0长度包
						UEP0_T_LEN = len;
						UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;  // 默认数据包是DATA1
					}
					else{  // 下传数据或其它
						UEP0_T_LEN = 0;  // 虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
						UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;  // 默认数据包是DATA1
					}
					break;
				}
				else {
					printf("ErrEndpInt\n");
					break;
				}
		}
		UIF_TRANSFER = 0;  // 清中断标志
	}
	else if(UIF_BUS_RST){  // USB总线复位
		printf("rst\n");  // 睡眠状态
		UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		UEP2_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
		USB_DEV_AD = 0x00;
		UIF_SUSPEND = 0;
		UIF_TRANSFER = 0;
		Endp2Busy = 0;
		Ready = 0;
		UIF_BUS_RST = 0;                                                            //清中断标志
	}
	else if(UIF_SUSPEND){  // USB总线挂起/唤醒完成
		UIF_SUSPEND = 0;
		if(USB_MIS_ST & bUMS_SUSPEND){  // 挂起
			printf("sleep\n");  // 睡眠状态
		}
		else{  // 唤醒
      printf("weak\n");  // 睡眠状态
			
		}
	}
	else{  // 意外的中断,不可能发生的情况
		printf("UnknownERR\n");
		USB_INT_FG = 0xFF;  // 清中断标志
	}
	if(UIF_TRANSFER) goto USB_DevIntNext;
	
}

/*******************************************************************************
* Function Name  : Enp2BlukIn
* Description    : USB设备模式端点2的批量上传
* Input          : None
* Return         : None
*******************************************************************************/
void Enp2BlukIn( UINT8 *buf,UINT8 len)
{
	memcpy( Ep2Buffer+MAX_PACKET_SIZE, buf, len);                              //加载上传数据
    if( Ready )
    {
        UEP2_T_LEN = len;                                                          //上传最大包长度
        UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                  //有数据时上传数据并应答ACK
        Endp2Busy = 1;                                                            
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
	
	USB_CTRL = 0x00;  // 先设定模式
	UEP2_3_MOD |= bUEP2_RX_EN | bUEP2_TX_EN;  // 端点2下传OUT和上传IN
	UEP2_3_MOD &= ~bUEP2_BUF_MOD;                                              //端点2收发各64字节缓冲区
	UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN);                                //端点0单64字节收发缓冲区
	UEP2_T_LEN = 0;                                                            //预使用发送长度一定要清空
	UEP0_DMA = Ep0Buffer;
	UEP2_DMA = Ep2Buffer;
	USB_DEV_AD = 0x00;
	UDEV_CTRL &= ~ bUD_PD_EN;  // 禁止DP/DM下拉电阻

	UDEV_CTRL &= ~bUD_LOW_SPEED;                                               //选择全速12M模式，默认方式
	USB_CTRL &= ~bUC_LOW_SPEED;


	USB_CTRL |= bUC_DEV_PU_EN | bUC_INT_BUSY;  // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
	UDEV_CTRL |= bUD_PORT_EN;  // 允许USB端口
	USB_INT_FG = 0xFF;  // 清中断标志
	USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
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
	CfgFsys( );                                                            //CH545时钟选择配置
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
			while( Endp2Busy )
			{				
					;    //如果忙（上一包数据没有传上去），则等待。
			}
			Enp2BlukIn( UserEp2Buf,THIS_ENDP0_SIZE );
		}
		mDelaymS( 100 );                                                   //模拟单片机做其它事	 
	}
}
