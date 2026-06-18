/********************************** (C) COPYRIGHT *******************************
* File Name          :Compound_Dev.C
* Author             : WCH
* Version            : V2.0
* Date               : 2023/05/31
* Description        : CH559模拟USB复合设备，键鼠，支持类命令
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "..\..\DEBUG.C"                                                       //������Ϣ��ӡ
#include "..\..\DEBUG.H"
#include <string.h>
#define THIS_ENDP0_SIZE         DEFAULT_ENDP0_SIZE
#define ENDP1_IN_SIZE           8
#define ENDP2_IN_SIZE           4

UINT8X  Ep0Buffer[MIN(64,THIS_ENDP0_SIZE+2)] _at_ 0x0000;  //端点0 OUT&IN缓冲区，必须是偶地址
UINT8X  Ep1Buffer[MIN(64,ENDP1_IN_SIZE+2)] _at_ MIN(64,THIS_ENDP0_SIZE+2);  //端点1 IN缓冲区,必须是偶地址
UINT8X  Ep2Buffer[MIN(64,ENDP2_IN_SIZE+2)] _at_ (MIN(64,THIS_ENDP0_SIZE+2)+MIN(64,ENDP1_IN_SIZE+2));  //端点2 IN缓冲区,必须是偶地址
UINT8   SetupReq,Ready,UsbConfig;
UINT16  SetupLen;
PUINT8  pDescr;                                                                //USB配置标志
USB_SETUP_REQ   SetupReqBuf;                                                   //暂存Setup包
#define UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)
#define DEBUG 0

#pragma  NOAREGS

/*设备描述符*/
UINT8C DevDesc[18] = {0x12,0x01,0x10,0x01,0x00,0x00,0x00,0x08,
                      0x3d,0x41,0x07,0x21,0x00,0x00,0x00,0x00,
                      0x00,0x01
                     };
UINT8C CfgDesc[59] =
{
    0x09,0x02,0x3b,0x00,0x02,0x01,0x00,0xA0,0x32,             //配置描述符
    0x09,0x04,0x00,0x00,0x01,0x03,0x01,0x01,0x00,             //接口描述符,键盘
    0x09,0x21,0x11,0x01,0x00,0x01,0x22,0x3e,0x00,             //HID类描述符
    0x07,0x05,0x81,0x03,ENDP1_IN_SIZE,0x00,0x0a,                       //端点描述符
    0x09,0x04,0x01,0x00,0x01,0x03,0x01,0x02,0x00,             //接口描述符,鼠标
    0x09,0x21,0x10,0x01,0x00,0x01,0x22,0x34,0x00,             //HID类描述符
    0x07,0x05,0x82,0x03,ENDP2_IN_SIZE,0x00,0x0a                        //端点描述符
};
/*字符串描述符*/
/*HID类报表描述符*/
UINT8C KeyRepDesc[62] =
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
UINT8C MouseRepDesc[52] =
{
    0x05,0x01,0x09,0x02,0xA1,0x01,0x09,0x01,
    0xA1,0x00,0x05,0x09,0x19,0x01,0x29,0x03,
    0x15,0x00,0x25,0x01,0x75,0x01,0x95,0x03,
    0x81,0x02,0x75,0x05,0x95,0x01,0x81,0x01,
    0x05,0x01,0x09,0x30,0x09,0x31,0x09,0x38,
    0x15,0x81,0x25,0x7f,0x75,0x08,0x95,0x03,
    0x81,0x06,0xC0,0xC0
};
/*鼠标数据*/
UINT8 HIDMouse[4] = {0x0,0x0,0x0,0x0};
UINT8 HIDKey[8] = {0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
UINT8 Endp1Busy = 0;
UINT8 Endp2Busy = 0;

/*******************************************************************************
* Function Name  : USBDeviceCfg()
* Description    : USB设备模式配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBDeviceCfg()
{
    USB_CTRL = 0x00;                                                           //清空USB控制寄存器
    USB_CTRL &= ~bUC_HOST_MODE;                                                //该位为选择设备模式
    USB_CTRL |=  bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;                    //USB设备和内部上拉使能,在中断期间中断标志未清除前自动返回NAK
    USB_DEV_AD = 0x00;                                                         //设备地址初始化
    UDEV_CTRL &= ~bUD_RECV_DIS;                                                //使能接收器
    USB_CTRL |= bUC_LOW_SPEED;
    UDEV_CTRL |= bUD_LOW_SPEED;                                                //选择低速1.5M模式
//    USB_CTRL &= ~bUC_LOW_SPEED;
//    UDEV_CTRL &= ~bUD_LOW_SPEED;                                             //选择全速12M模式，默认方式
    UDEV_CTRL |= bUD_DP_PD_DIS | bUD_DM_PD_DIS;                                //禁止DM、DP下拉电阻
    UDEV_CTRL |= bUD_PORT_EN;                                                  //使能物理端口
}
/*******************************************************************************
* Function Name  : USBDeviceIntCfg()
* Description    : USB设备模式中断初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBDeviceIntCfg()
{
    USB_INT_EN |= bUIE_SUSPEND;                                               //使能设备挂起中断
    USB_INT_EN |= bUIE_TRANSFER;                                              //使能USB传输完成中断
    USB_INT_EN |= bUIE_BUS_RST;                                               //使能设备模式USB总线复位中断
    USB_INT_FG |= 0x1F;                                                       //清中断标志
    IE_USB = 1;                                                               //使能USB中断
    EA = 1;                                                                   //允许单片机中断
}
/*******************************************************************************
* Function Name  : USBDeviceEndPointCfg()
* Description    : USB设备模式端点配置，模拟复合设备，除了端点0的控制传输，还包括端点1、2的中断上传
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBDeviceEndPointCfg()
{
    UEP1_DMA = Ep1Buffer;                                                      //端点1数据传输地址
    UEP4_1_MOD |= bUEP1_TX_EN;                                                 //端点1发送使能
    UEP4_1_MOD &= ~bUEP1_RX_EN;                                                //端点1接收禁止
    UEP4_1_MOD &= ~bUEP1_BUF_MOD;                                              //端点1单64字节发送缓冲区
    UEP2_DMA = Ep2Buffer;                                                      //端点2数据传输地址
    UEP2_3_MOD |= bUEP2_TX_EN;                                                 //端点2发送使能
    UEP2_3_MOD &= ~bUEP2_RX_EN;                                                //端点2接收禁止
    UEP2_3_MOD &= ~bUEP2_BUF_MOD;                                              //端点2单64字节发送缓冲区
    UEP0_DMA = Ep0Buffer;                                                      //端点0数据传输地址
    UEP4_1_MOD &= ~(bUEP4_RX_EN | bUEP4_TX_EN);                                //端点0单64字节收发缓冲区
}
/*******************************************************************************
* Function Name  : enp1IntIn()
* Description    : USB设备模式端点1的中断上传
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void enp1IntIn( )
{
    memcpy( Ep1Buffer, HIDKey, sizeof(HIDKey));                              //加载上传数据
    if( Ready )
    {
        UEP1_T_LEN = sizeof(HIDKey);                                             //上传数据长度
        UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                //有数据时上传数据并应答ACK
        Endp1Busy = 1;
    }
}
/*******************************************************************************
* Function Name  : enp2IntIn()
* Description    : USB设备模式端点2的中断上传
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void enp2IntIn( )
{
    memcpy( Ep2Buffer, HIDMouse, sizeof(HIDMouse));                          //加载上传数据
    if( Ready )
    {
        UEP2_T_LEN = sizeof(HIDMouse);                                           //上传数据长度
        UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;                //有数据时上传数据并应答ACK
        Endp2Busy = 1;
    }
}

/*******************************************************************************
* Function Name  : DeviceInterrupt()
* Description    : CH559USB中断处理函数
*******************************************************************************/
void    DeviceInterrupt( void ) interrupt INT_NO_USB using 1                      //USB中断服务程序,使用寄存器组1
{
    UINT8  errflag;
    UINT16 len;
#if DEBUG
    printf("%02X ",(UINT16)USB_INT_FG);
#endif
    if(UIF_TRANSFER)                                                            //USB传输完成标志
    {
        switch (USB_INT_ST & (MASK_UIS_TOKEN | MASK_UIS_ENDP))
        {
        case UIS_TOKEN_IN | 2:                                                  //endpoint 2# 中断端点上传
            UEP2_T_LEN = 0;                                                     //预使用发送长度一定要清空
            UEP2_CTRL ^= bUEP_T_TOG;                                            //手动翻转
            UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK
            Endp2Busy = 0;
            break;
        case UIS_TOKEN_IN | 1:                                                  //endpoint 1# 中断端点上传
            UEP1_T_LEN = 0;                                                     //预使用发送长度一定要清空
            UEP1_CTRL ^= bUEP_T_TOG;                                            //手动翻转
            UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;           //默认应答NAK
            Endp1Busy = 0;
            break;
        case UIS_TOKEN_SETUP | 0:                                               //SETUP事务
            UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_NAK | UEP_T_RES_NAK; 
			len = USB_RX_LEN;
            if(len == (sizeof(USB_SETUP_REQ)))
            {
                SetupLen = ( (UINT16)UsbSetupBuf->wLengthH << 8 ) | UsbSetupBuf->wLengthL;
                len = 0;                                                         // 默认为成功并且上传0长度
                errflag = 0;
                if ( ( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )/* 只支持标准请求 */
                {
//                    errflag = 0xFF;                                                  // 操作失败
//                    printf("ErrEp0ReqType=%02X\n",(UINT16)UsbSetupBuf->bRequestType);
                }
								else{
                //标准请求
                SetupReq = UsbSetupBuf->bRequest;
#if DEBUG
                printf("REQ %02X ",(UINT16)SetupReq);
#endif
                switch(SetupReq)                                                  //请求码
                {
                case USB_GET_DESCRIPTOR:
                    switch(UsbSetupBuf->wValueH)
                    {
                    case 1:                                                       //设备描述符
                        pDescr = DevDesc;                                         //把设备描述符送到要发送的缓冲区
                        len = sizeof(DevDesc);
                        break;
                    case 2:                                                       //配置描述符
                        pDescr = CfgDesc;                                         //把设备描述符送到要发送的缓冲区
                        len = sizeof(CfgDesc);
                        break;
                    case 0x22:                                                    //报表描述符
#if DEBUG
                        printf("RREQ %02X ",(UINT16)SetupReq);
#endif
                        if(UsbSetupBuf->wIndexL == 0)                             //接口0报表描述符
                        {
                            pDescr = KeyRepDesc;                                  //数据准备上传
                            len = sizeof(KeyRepDesc);
                        }
                        else if(UsbSetupBuf->wIndexL == 1)                        //接口1报表描述符
                        {
                            pDescr = MouseRepDesc;                                //数据准备上传
                            len = sizeof(MouseRepDesc);
                            Ready = 1;                                            //如果有更多接口，该标准位应该在最后一个接口配置完成后有效
                        }
                        else
                        {
                            errflag = 0xFF;                                           //本程序只有2个接口，这句话正常不可能执行
                        }
                        break;
                    default:
                        errflag = 0xFF;                                               //不支持的命令或者出错
                        break;
                    }
                    if ( errflag == 0xFF ){
                        break;                                                    //出错或者命令不支持
                    }										
                    if ( SetupLen > len ){
                        SetupLen = len;                                           //限制总长度
                    }
                    len = SetupLen >= 8 ? 8 : SetupLen;                            //本次传输长度
                    memcpy(Ep0Buffer,pDescr,len);                                  //加载上传数据
                    SetupLen -= len;
                    pDescr += len;
                    break;
                case USB_SET_ADDRESS:
                    SetupLen = UsbSetupBuf->wValueL;                              //暂存USB设备地址
                    break;
                case USB_GET_CONFIGURATION:
                    Ep0Buffer[0] = UsbConfig;
                    if ( SetupLen >= 1 )
                    {
                        len = 1;
                    }
                    break;
                case USB_SET_CONFIGURATION:
                    UsbConfig = UsbSetupBuf->wValueL;
                    break;
                case 0x0A:
                    break;
                case USB_CLEAR_FEATURE:                                            //Clear Feature
                    if ( ( UsbSetupBuf->bRequestType & USB_REQ_RECIP_MASK ) == USB_REQ_RECIP_ENDP )// 端点
                    {
                       switch( UsbSetupBuf->wIndexL )
                       {
                          case 0x82:
                               UEP2_CTRL = UEP2_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                               break;
                          case 0x81:
                               UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_T_TOG | MASK_UEP_T_RES ) | UEP_T_RES_NAK;
                               break;
                          case 0x01:
                               UEP1_CTRL = UEP1_CTRL & ~ ( bUEP_R_TOG | MASK_UEP_R_RES ) | UEP_R_RES_ACK;
                               break;
                          default:
                               errflag = 0xFF;                                         // 不支持的端点
                               break;
                        }
                     }
                     else
                     {
                        errflag = 0xFF;                                                // 不是端点不支持
                     }
                    break;
                    case USB_SET_FEATURE:                                          /* Set Feature */
                    if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x00 )                  /* 设置设备 */
                    {
                        if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x01 )
                        {
                            if( CfgDesc[ 7 ] & 0x20 )
                            {
                                 /* 设置唤醒使能标志 */
                            }
                            else
                            {
                                errflag = 0xFF;                                        /* 操作失败 */
                            }
                        }
                        else
                        {
                            errflag = 0xFF;                                            /* 操作失败 */
                        }
                    }
                    else if( ( UsbSetupBuf->bRequestType & 0x1F ) == 0x02 )             /* 设置端点 */
                    {
                        if( ( ( ( UINT16 )UsbSetupBuf->wValueH << 8 ) | UsbSetupBuf->wValueL ) == 0x00 )
                        {
                            switch( ( ( UINT16 )UsbSetupBuf->wIndexH << 8 ) | UsbSetupBuf->wIndexL )
                            {
                                case 0x82:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点2 IN STALL */
                                    break;

                                case 0x02:
                                    UEP2_CTRL = UEP2_CTRL & (~bUEP_R_TOG) | UEP_R_RES_STALL;/* 设置端点2 OUT Stall */
                                    break;

                                case 0x81:
                                    UEP1_CTRL = UEP1_CTRL & (~bUEP_T_TOG) | UEP_T_RES_STALL;/* 设置端点1 IN STALL */
                                    break;

                                default:
                                    errflag = 0xFF;                                    /* 操作失败 */
                                    break;
                            }
                        }
                        else
                        {
                            errflag = 0xFF;                                      /* 操作失败 */
                        }
                    }
                    else
                    {
                        errflag = 0xFF;                                          /* 操作失败 */
                    }
                    break;
                case USB_GET_STATUS:
                     Ep0Buffer[0] = 0x00;
                     Ep0Buffer[1] = 0x00;
                     if ( SetupLen >= 2 )
                     {
                        len = 2;
                     }
                     else
                     {
                        len = SetupLen;
                     }
                     break;
                default:
                    errflag = 0xFF;                                                    //操作失败
                    break;
                }
							}
            }
            else
            {
                errflag = 0xFF;                                                         //包长度错误
            }
            if(errflag == 0xFF)
            {
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;//STALL
            }
            else if(len <= 8)                                                       //上传数据或者状态阶段返回0长度包
            {
                UEP0_T_LEN = len;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1，返回应答ACK
#if DEBUG
                printf("S_U\n");
#endif
            }
            else
            {
                UEP0_T_LEN = 0;  //虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;//默认数据包是DATA1,返回应答ACK
            }
            break;
        case UIS_TOKEN_IN | 0:                                                      //endpoint0 IN
            switch(SetupReq)
            {
            case USB_GET_DESCRIPTOR:
                len = SetupLen >= 8 ? 8 : SetupLen;                                 //本次传输长度
                memcpy( Ep0Buffer, pDescr, len );                                   //加载上传数据
                SetupLen -= len;
                pDescr += len;
                UEP0_T_LEN = len;
                UEP0_CTRL ^= bUEP_T_TOG;                                             //同步标志位翻转
                break;
            case USB_SET_ADDRESS:
                USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            default:
                UEP0_T_LEN = 0;                                                      //状态阶段完成中断或者是强制上传0长度数据包结束控制传输
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            }
            break;
        case UIS_TOKEN_OUT | 0:  // endpoint0 OUT
            len = USB_RX_LEN;
            if(SetupReq == 0x09)
            {
                if(Ep0Buffer[0])
                {
                    printf("Light on Num Lock LED!\n");
                }
                else if(Ep0Buffer[0] == 0)
                {
                    printf("Light off Num Lock LED!\n");
                }
            }
			UEP0_CTRL ^= bUEP_R_TOG;                                     //同步标志位翻转
            break;
        default:
            break;
        }
        UIF_TRANSFER = 0;                                                           //写0清空中断
    }
    else if(UIF_BUS_RST)                                                                 //设备模式USB总线复位中断
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP1_CTRL = UEP_T_RES_NAK;
        UEP2_CTRL = UEP_T_RES_NAK;
        Endp1Busy = 0;
        Endp2Busy = 0;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        UIF_BUS_RST = 0;                                                             //清中断标志
    }
    else if (UIF_SUSPEND)                                                                 //USB总线挂起/唤醒完成
    {
        UIF_SUSPEND = 0;
        if ( USB_MIS_ST & bUMS_SUSPEND )                                             //挂起
        {
#if DEBUG
            printf( "zz" );                                                             //睡眠状态
            while ( XBUS_AUX & bUART0_TX )
            {
                ;    //等待发送完成
            }
#endif
            SAFE_MOD = 0x55;
            SAFE_MOD = 0xAA;
            WAKE_CTRL = bWAK_BY_USB | bWAK_RXD0_LO;                                     //USB或者RXD0有信号时可被唤醒
            PCON |= PD;                                                                 //睡眠
            SAFE_MOD = 0x55;
            SAFE_MOD = 0xAA;
            WAKE_CTRL = 0x00;
        }
    }
    else {                                                                             //意外的中断,不可能发生的情况
        USB_INT_FG = 0xFF;                                                             //清中断标志
//      printf("UnknownInt  N");
    }
}
void HIDValueHandle()
{
    UINT8 i;
    if( RI )
    {
		RI = 0;
		i = SBUF;
        printf( "%c\n", (UINT8)i );
        switch(i)
        {
//鼠标数据上传示例
        case 'L':                                                        //左键
            HIDMouse[0] = 0x01;
            while( Endp2Busy );
            enp2IntIn();
            HIDMouse[0] = 0;
            while( Endp2Busy );
			enp2IntIn();
            break;
        case 'R':                                                        //右键
            HIDMouse[0] = 0x02;
            while( Endp2Busy );
            enp2IntIn();
            HIDMouse[0] = 0;
            while( Endp2Busy );
			enp2IntIn();
            break;
//键盘数据上传示例
        case 'A':                                                         //A键
            HIDKey[2] = 0x04;                                             //按键开始
            while( Endp1Busy );
            enp1IntIn();
			HIDKey[2] = 0;                                                //按键结束
			while( Endp1Busy );                                           /*等待传输完成*/    
			enp1IntIn();		
			break;
        case 'P':                                                         //P键
            HIDKey[2] = 0x13;
            while( Endp1Busy );
            enp1IntIn();
            HIDKey[2] = 0;                                                //按键结束
            while( Endp1Busy );                                           /*等待传输完成*/
            enp1IntIn();			
            break;
        case 'Q':                                                         //Num Lock键
            HIDKey[2] = 0x53;
            while( Endp1Busy );
            enp1IntIn();
            HIDKey[2] = 0;                                                //按键结束
            while( Endp1Busy );                                           /*等待传输完成*/
            enp1IntIn();			
            break;
        default:                                                          //其他
            UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;     //默认应答NAK
            UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;     //默认应答NAK
            break;
        }
    }
    else
    {
        UEP1_CTRL = UEP1_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;        //默认应答NAK
        UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;        //默认应答NAK
    }
}
main()
{
//  CfgFsys( );                                                           //CH559时钟选择配置 
//  mDelaymS(5);                                                          //等待外部晶振稳定 	 			
    mInitSTDIO( );                                                        //串口0,可以用于调试
    printf("start ...\n");
    USBDeviceCfg();                                                       //模拟鼠标
    USBDeviceEndPointCfg();                                               //端点配置
    USBDeviceIntCfg();                                                    //中断初始化
    UEP1_T_LEN = 0;                                                       //预使用发送长度一定要清空
    UEP2_T_LEN = 0;                                                       //预使用发送长度一定要清空
    Ready = 0;
    while(1)
    {
        if(Ready)
        {
			HIDValueHandle();
        }
        mDelaymS( 100 );                                                   //模拟单片机做其它事
    }
}
