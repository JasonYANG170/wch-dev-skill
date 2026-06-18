/********************************** (C) COPYRIGHT *******************************
* File Name          : Host.C
* Author             : WCH
* Version            : V1.0
* Date               : 2021/02/25
* Description        : 可实现键盘、鼠标、U盘枚举、HID兼容、Android配置模式通讯
                       注意包含文件USBHOST.C/DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "USBHOST.H"

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Return         : None
*******************************************************************************/
void main(void)
{
	UINT8	i, s, len, endp;
	UINT16 loc;
	
	CfgFsys();
	mInitSTDIO();  /* 为了让计算机通过串口监控演示过程 */
	printf("Start @ChipID=%02X\n", (UINT16)CHIP_ID);
	InitUSB_Host();

	FoundNewDev = 0;
	printf("Wait Device In\n");
	
	while(1){			
    s = ERR_SUCCESS;
		if(UIF_DETECT){  					// 如果有USB主机检测中断则处理				
			UIF_DETECT = 0;  						// 清中断标志
			s = AnalyzeRootHub();   // 分析ROOT-HUB状态
			if(s == ERR_USB_CONNECT) FoundNewDev = 1;
		}
		if(FoundNewDev || s == ERR_USB_CONNECT){  // 有新的USB设备插入				
			FoundNewDev = 0;
			mDelaymS( 200 );  // 由于USB设备刚插入尚未稳定,故等待USB设备数百毫秒,消除插拔抖动
			s = EnumAllRootDevice();  // 枚举所有ROOT-HUB端口的USB设备
			if(s != ERR_SUCCESS) printf("EnumAllRootDev err = %02X\n", (UINT16)s);
		}
		mDelaymS(100);  // 模拟单片机做其它事
    loc = SearchTypeDevice(DEV_TYPE_MOUSE);  // 在ROOT-HUB以及外部HUB各端口上搜索指定类型的设备所在的端口号
		
		if (loc != 0xFFFF){  // 找到了,如果有两个MOUSE如何处理?				
			i = (UINT8)(loc >> 8);
			len = (UINT8)loc;
			SelectHubPort(len);  // 选择操作指定的ROOT-HUB端口,设置当前USB速度以及被操作设备的USB地址
			endp = len ? DevOnHubPort[len-1].GpVar[0] : ThisUsbDev.GpVar[0];  // 中断端点的地址,位7用于同步标志位
			if(endp & USB_ENDP_ADDR_MASK){  // 端点有效
				s = USBHostTransact(USB_PID_IN << 4 | endp & 0x7F, endp & 0x80 ? bUH_R_TOG | bUH_T_TOG : 0, 0);  // CH557传输事务,获取数据,NAK不重试
				if(s == ERR_SUCCESS) {
					endp ^= 0x80;  // 同步标志翻转
					if (len) DevOnHubPort[len-1].GpVar[0] = endp; // 保存同步标志位
					else ThisUsbDev.GpVar[0] = endp;
					len = USB_RX_LEN;  // 接收到的数据长度
					if(len){
						printf("Mouse data: ");
						for(i = 0; i < len; i ++) printf("x%02X ",(UINT16)(RxBuffer[i]));
						printf("\n");
					}
				}
				else if(s != ( USB_PID_NAK | ERR_USB_TRANSFER )) printf("Mouse error %02x\n",(UINT16)s);  // 可能是断开了
			}
			else printf("Mouse no interrupt endpoint\n");
			SetUsbSpeed(1);  // 默认为全速
		}
  }
}








