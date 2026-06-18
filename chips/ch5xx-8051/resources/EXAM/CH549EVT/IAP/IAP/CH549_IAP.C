/********************************** (C) COPYRIGHT ******************************
* File Name          : CH549_IAP.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/24
* Description        : IAP 功能演示例子程序
*                      1，支持USB下载，USB为全速自定义设备
*                      2，支持CodeFlash编程
                       注意包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\IAP\IAP\CH549_IAP.H"
/******************************************************************************
*  注：CH549_IAP.C需要配合CH549_USER.C使用
*  注意：本例子从0xE000开始存放IAP代码,也就是从56K处开始存放IAP,56K-60K位置可以用于存放IAP代码
*  编译前需指定链接地址： Option->Target Code Memery选项的 Eprom起始地址填入:0xE004
*
*  链接地址偏移四字节的说明：IAP放在CodeFlash后面，生成的BIN文件开头的四个字节是跳转到
*  IAPmain的指令。而整合USER和IAP之后，前面区域都是放用户区文件的，所以这四个字节的跳转
*  到IAPmain的指令必须搬到后面某固定地址处，所以IAP链接文件时向后偏移4字节就是为了跳转
*  指令腾地方。
*******************************************************************************/
#define THIS_ENDP0_SIZE         DEFAULT_ENDP0_SIZE               //默认端点0的大小
// 设备描述符
UINT8C MyDevDescr[] = { 0x12, 0x01, 0x10, 0x01,
                        0xFF, 0x80, 0x55, THIS_ENDP0_SIZE,
                        0x48, 0x43, 0xe0, 0x55,
                        0x00, 0x01, 0x00, 0x00,0x00, 0x01
                      };
// 配置描述符
UINT8C MyCfgDescr[] = { 0x09, 0x02, 0x20, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
                        0x09, 0x04, 0x00, 0x00, 0x02, 0xFF, 0x80, 0x55, 0x00,
                        0x07, 0x05, 0x82, 0x02, 0x40, 0x00, 0x00,
                        0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00
                      };
UINT8X UsbConfig = 0;                                            // USB配置标志
UINT8X Ep0Buffer[ THIS_ENDP0_SIZE+2 ] _at_ 0x0000 ;              // OUT&IN, must even address
UINT8X Ep2Buffer[ 2*MAX_PACKET_SIZE+4 ] _at_ THIS_ENDP0_SIZE+2 ; // OUT+IN, must even address
#define   UsbSetupBuf     ((PUSB_SETUP_REQ)Ep0Buffer)
UINT8X SetupReqCode,SetupLen;
PUINT8 pDescr;

iap_cmd1 xdata iap_cmd _at_ 0x00C6;                              //IAP命令
UINT8X uart_bit;                                                 //下载方式全局标志位，1表示为串口，2表示为USB口
UINT16X chip_id,eeprom_len;
PUINT8C pCode;
#pragma NOAREGS
sbit DisableIAP = P1^0;                                         //软复位,返回用户程序触发引脚
sbit D2 = P2^2;
UINT16X cur_block,last_block;                                   //64字节一块，表示当前和前一次的块号，控制块的擦除操作
             
/*
 * @Note
 * The following Flash Operation Flags need to be assigned specific values before 
 * erasing or programming Flash, and to be cleared after the operations are done, 
 * which can prevent misoperation of Flash.
 */
UINT8 Flash_Op_Check_Byte1 = 0x00;
UINT8 Flash_Op_Check_Byte2 = 0x00;

/* Check of Flash Operation */ 
#define DEF_FLASH_OP_CHECK1     0xAA
#define DEF_FLASH_OP_CHECK2     0x55  

/*******************************************************************************
* Function Name  : Flash_Op_Unlock
* Description    : Flash????????
* Input          : flash_type: bCODE_WE(Code Flash); bDATA_WE(Data Flash) 
* Output         : None
* Return         : 0xFF(Flash Operation Flags Error)/0x00(Flash Operation Flags Correct)
*******************************************************************************/
UINT8 Flash_Op_Unlock( UINT8 flash_type )
{
    bit ea_sts;
    
    /* Check the Flash operation flags to prevent Flash misoperation. */
    if( ( Flash_Op_Check_Byte1 != DEF_FLASH_OP_CHECK1 ) ||
        ( Flash_Op_Check_Byte2 != DEF_FLASH_OP_CHECK2 ) )
    {
        return 0xFF;                                                           /* Flash Operation Flags Error */
    }
    
    /* Disable all INTs to prevent writing GLOBAL_CFG from failing in safe mode. */
    ea_sts = EA;                                
    EA = 0;
    
    /* Enable Flash writing operations. */
    SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= flash_type;
    SAFE_MOD = 0x00;

    /* Restore all INTs. */
    EA = ea_sts;
    
    return 0x00;
}

/*******************************************************************************
* Function Name  : Flash_Op_Lock
* Description    : Flash????????
* Input          : flash_type: bCODE_WE(Code Flash)/bDATA_WE(Data Flash) 
* Output         : None
* Return         : None
*******************************************************************************/
void Flash_Op_Lock( UINT8 flash_type )
{
    bit ea_sts;
    
    /* Disable all INTs to prevent writing GLOBAL_CFG from failing in safe mode. */
    ea_sts = EA;                                
    EA = 0;
    
    /* Disable Flash writing operations. */
    SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~flash_type;
    SAFE_MOD = 0x00;
    
    /* Restore all INTs. */
    EA = ea_sts;
}                      
                      
/*******************************************************************************
* Function Name  : ErasePage( UINT16 Addr )
* Description    : 用于页擦除，每64字节为一页。将页内所有数据变为0x00
* Input          : Addr:擦除地址所在页
* Output         : None
* Return         : 返回操作状态，0x00:成功  0x01:地址无效  0x02:未知命令或超时
*******************************************************************************/
UINT8 FlashErasePage( UINT16 Addr )
{
    UINT8 status;                                    /* 返回操作状态 */
    UINT8 FlashType;                                 /* Flash 类型标志 */
    
    if( ( Flash_Op_Check_Byte1 != DEF_FLASH_OP_CHECK1 ) ||                   
        ( Flash_Op_Check_Byte2 != DEF_FLASH_OP_CHECK2 ) )
    {
        return( 0xFF );
    }
    Addr &= 0xFFC0;                                  /* 64字节对齐 */
    if((Addr>0xEFFF) && (Addr<0xF400))               /* DataFlash区域 */
    {
        FlashType = bDATA_WE;
    }
    else                                             /* CodeFlash区域 */
    {
        FlashType = bCODE_WE;
    }
    
    if( Flash_Op_Unlock( FlashType ) )               
    {
        return( 0xFF );
    }
    
    ROM_ADDR = Addr;                                 /* 写入目标地址 */
    ROM_BUF_MOD = bROM_BUF_BYTE;                     /* 选择块擦除模式或单字节编程模式 */
    ROM_DAT_BUF = 0;                                 /* 擦写数据缓冲区寄存器为0 */
    if ( ROM_STATUS & bROM_ADDR_OK )                 /* 操作地址有效 */
    {
        ROM_CTRL = ROM_CMD_ERASE;                    /* 启动擦除 */
        if(ROM_STATUS & bROM_CMD_ERR)
        {
            status = 0x02;    /* 未知命令或超时 */
        }
        else
        {
            status = 0x00;    /* 操作成功 */
        }
    }
    else
    {
        status = 0x01;    /* 地址无效 */
    }
    Flash_Op_Lock( FlashType );
    
    return status;
}
/*******************************************************************************
* Function Name  : FlashProgByte(UINT16 Addr,UINT8 Data)
* Description    : Flash 字节编程
* Input          : Addr：写入地址
*                  Data：字节编程的数据
* Output         : None
* Return         : 编程状态返回 0x00:成功  0x01:地址无效  0x02:未知命令或超时
*******************************************************************************/
UINT8 FlashProgByte( UINT16 Addr,UINT8 Data )
{
    UINT8 status;                                    /* 返回操作状态 */
    UINT8 FlashType;                                 /* Flash 类型标志 */
    
    if( ( Flash_Op_Check_Byte1 != DEF_FLASH_OP_CHECK1 ) || 
        ( Flash_Op_Check_Byte2 != DEF_FLASH_OP_CHECK2 ) )
    {
        return( 0xFF );
    }
    
    if((Addr>0xEFFF) && (Addr<0xF400))               /* DataFlash区域 */
    {
        FlashType = bDATA_WE;
    }
    else                                             /* CodeFlash区域 */
    {
        FlashType = bCODE_WE;
    }
    
    if( Flash_Op_Unlock( FlashType ) )               
    {
        return( 0xFF );
    }
    
    ROM_ADDR = Addr;                                 /* 写入目标地址 */
    ROM_BUF_MOD = bROM_BUF_BYTE;                     /* 选择块擦除模式或单字节编程模式 */
    ROM_DAT_BUF = Data;                              /* 数据缓冲区寄存器 */
    if ( ROM_STATUS & bROM_ADDR_OK )                 /* 操作地址有效 */
    {
        ROM_CTRL = ROM_CMD_PROG ;                    /* 启动编程 */
        if(ROM_STATUS & bROM_CMD_ERR)
        {
            status = 0x02;    /* 未知命令或超时 */
        }
        else
        {
            status = 0x00;    /* 操作成功 */
        }
    }
    else
    {
        status = 0x01;    /* 地址无效 */
    }
    Flash_Op_Lock( FlashType );
    
    return status;
}
/*******************************************************************************
* Function Name  : FlashVerify
* Description    : Flash校验
* Input          : Addr    芯片编程地址地址，地址为偶数地址
                   pData   编程数据，以WORD为基准
                   len     校验长度
* Output         : None
* Return         :         返回校验状态
                    0x00   校验成功
                    0xff   校验失败
*******************************************************************************/
UINT8 FlashVerify( UINT16 Addr, UINT8 *pData, UINT16 len )
{
    UINT16 i;
    pCode = (PUINT8C)( Addr );
    for( i=0; i!=len; i++ )
    {
        if( *pData != *pCode )
        {
            return 0xff;
        }
        pCode++;
        pData++;
    }
    return 0;
}
/*******************************************************************************
* Function Name  : CH549_Respond
* Description    : IAP升级时，芯片应答函数
* Input          : s 有效应答字节
* Output         : None
* Return         : SBUF 串口接收字节
*******************************************************************************/
void CH549_Respond( UINT8 s )
{
    Ep2Buffer[ MAX_PACKET_SIZE ] = s;
    Ep2Buffer[ MAX_PACKET_SIZE+1 ] = 0x00;
    UEP2_T_LEN = 2;
    UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_ACK;  // 允许上传
}
/*******************************************************************************
* Function Name  : CH549_USB_ISPDownload
* Description    : CH549下载函数
*                ：
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH549_IAPDownload( void )
{
    UINT8  s;
    UINT16 i;
    UINT16 len;
    UINT32 addr;
    switch( iap_cmd.other.buf[0] )                                  //分析命令码
    {
    case CMD_IAP_PROM:                                              //ISP 编程命令
        Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;
        Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
        len = iap_cmd.program.len;                                  //待写字节数
        addr = (iap_cmd.program.addr[0] | (UINT16)iap_cmd.program.addr[1]<<8);
        cur_block = (addr+len-1)/64;                                //下传数据的最后一个字节地址所在块
        if(cur_block!=last_block)
        {
            last_block = cur_block;
            s = FlashErasePage(cur_block<<6);                       //擦除
        }
        if(s == 0)
        {
            for(i=0; i!=len; i++)
            {
                s = FlashProgByte( addr+i,iap_cmd.program.buf[i] ); //字节编程,如果需提速，可以改成块编程
                if(s)
                {
                    break;
                }
            }
        }
        Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;
        Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
        CH549_Respond( s );                                         //返回校验
        break;
    case CMD_IAP_ERASE:                                             //ISP 擦除命令（实际操作上在编程时擦除）
        cur_block = 0;                                              //CMD_IAP_ERASE是第一条命令，后面也不会出现,故此处初始化变量
        last_block = 0xFFFF;
        CH549_Respond( 0 );
        break;
    case CMD_IAP_VERIFY:                                             // ISP 校验命令
        addr = (iap_cmd.verify.addr[0] | (UINT16)iap_cmd.verify.addr[1]<<8);
        len = iap_cmd.verify.len;
        s = FlashVerify( addr,&(iap_cmd.verify.buf[0]),iap_cmd.verify.len );
        CH549_Respond( s );
        break;
    case CMD_IAP_END:                                                // ISP 结束命令
        CH549SoftReset();                                            // 执行复位
        break;
    default:
        CH549_Respond( 0xfe );                                       // 未知的命令
        break;
    }
}
/*******************************************************************************
* Function Name  : USB_DeviceInterrupt
* Description    : USB中断查询函数，IAP程序无法使用中断
*                ：
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USB_DeviceInterrupt( void )
{
    UINT8 len;
    if( UIF_TRANSFER )                                                // USB传输完成
    {
        switch ( USB_INT_ST & ( MASK_UIS_TOKEN | MASK_UIS_ENDP ) )// 分析操作令牌和端点号
        {
        case UIS_TOKEN_OUT | 2:                                   // endpoint 2# 批量端点下传
            if ( U_TOG_OK )                                       // 不同步的数据包将丢弃
            {
                len = USB_RX_LEN;
                memcpy( iap_cmd.other.buf,Ep2Buffer,len );
                uart_bit = 2;
                CH549_IAPDownload( );
            }
            break;
        case UIS_TOKEN_IN | 2:                                    // endpoint 2# 批量端点上传
            UEP2_CTRL = UEP2_CTRL & ~ MASK_UEP_T_RES | UEP_T_RES_NAK;// 暂停上传
            break;
        case UIS_TOKEN_SETUP | 0:                                 // endpoint 0# SETUP
            len = USB_RX_LEN;
            if ( len == sizeof( USB_SETUP_REQ ) )                 // SETUP包长度
            {
                SetupLen = UsbSetupBuf->wLengthL;
                if ( UsbSetupBuf->wLengthH || SetupLen > 0x7F )
                {
                    SetupLen = 0x7F;                              // 限制总长度
                }
                len = 0;                                          // 默认为成功并且上传0长度
                if ( ( UsbSetupBuf->bRequestType & USB_REQ_TYP_MASK ) != USB_REQ_TYP_STANDARD )//只支持标准请求
                {
                    len = 0xFF;                                   // 操作失败
                }
                else                                              // 标准请求
                {
                    SetupReqCode = UsbSetupBuf->bRequest;
                    switch( SetupReqCode )                        // 请求码
                    {
                    case USB_GET_DESCRIPTOR:
                        switch( UsbSetupBuf->wValueH )
                        {
                        case 1:                                   // 设备描述符
                            pDescr = (PUINT8)( &MyDevDescr[0] );
                            len = sizeof( MyDevDescr );
                            break;
                        case 2:                                   // 配置描述符
                            pDescr = (PUINT8)( &MyCfgDescr[0] );
                            len = sizeof( MyCfgDescr );
                            break;
                        default:
                            len = 0xFF;                           // 不支持的描述符类型
                            break;
                        }
                        if ( SetupLen > len )
                        {
                            SetupLen = len;                       // 限制总长度
                        }
                        len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;// 本次传输长度
                        memcpy( Ep0Buffer, pDescr, len );         // 加载上传数据
                        SetupLen -= len;
                        pDescr += len;
                        break;
                    case USB_SET_ADDRESS:
                        SetupLen = UsbSetupBuf->wValueL;          // 暂存USB设备地址
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
                    default:
                        len = 0xFF;                               // 操作失败
                        break;
                    }
                }
            }
            else
            {
                len = 0xFF;                                       // SETUP包长度错误
            }
            if ( len == 0xFF )                                    // 操作失败
            {
                SetupReqCode = 0xFF;
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_STALL | UEP_T_RES_STALL;// STALL
            }
            else if ( len <= THIS_ENDP0_SIZE )                    // 上传数据或者状态阶段返回0长度包
            {
                UEP0_T_LEN = len;
                UEP0_CTRL  = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;// 默认数据包是DATA1
            }
            else                                                  // 下传数据或其它
            {
                UEP0_T_LEN = 0;                                   // 虽然尚未到状态阶段，但是提前预置上传0长度数据包以防主机提前进入状态阶段
                UEP0_CTRL = bUEP_R_TOG | bUEP_T_TOG | UEP_R_RES_ACK | UEP_T_RES_ACK;// 默认数据包是DATA1
            }
            break;
        case UIS_TOKEN_IN | 0:                                    // endpoint 0# IN
            switch( SetupReqCode )
            {
            case USB_GET_DESCRIPTOR:
                len = SetupLen >= THIS_ENDP0_SIZE ? THIS_ENDP0_SIZE : SetupLen;// 本次传输长度
                memcpy( Ep0Buffer, pDescr, len );                 // 加载上传数据
                SetupLen -= len;
                pDescr += len;
                UEP0_T_LEN = len;
                UEP0_CTRL ^= bUEP_T_TOG;                          // 翻转
                break;
            case USB_SET_ADDRESS:
                USB_DEV_AD = USB_DEV_AD & bUDA_GP_BIT | SetupLen;
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            default:
                UEP0_T_LEN = 0;                                   // 状态阶段完成中断或者是强制上传0长度数据包结束控制传输
                UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
                break;
            }
            break;
        case UIS_TOKEN_OUT | 0:                                   // endpoint 0# OUT
            UEP0_CTRL ^= bUEP_R_TOG;                              //同步标志位翻转
            break;
        default:
            break;
        }
        UIF_TRANSFER = 0;                                             // 清中断标志
    }
    else if ( UIF_BUS_RST )                                           // USB总线复位
    {
        UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
        UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
        USB_DEV_AD = 0x00;
        UIF_SUSPEND = 0;
        UIF_TRANSFER = 0;
        UIF_BUS_RST = 0;                                              // 清中断标志
    }
    else if ( UIF_SUSPEND )                                           // USB总线挂起/唤醒完成
    {
        UIF_SUSPEND = 0;
    }
}
/*******************************************************************************
* Function Name  : InitUSB_Device()
* Description    : USB设备模式配置,设备模式启动，收发端点配置，中断开启
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitUSB_Device( void )                                         // 初始化USB设备
{
    IE_USB = 0;
    USB_CTRL = 0x00;                                                // 先设定模式
    UEP2_3_MOD = bUEP2_RX_EN | bUEP2_TX_EN;                         // 端点2下传OUT和上传IN
    UEP0_DMA = Ep0Buffer;
    UEP2_DMA = Ep2Buffer;
    UEP0_CTRL = UEP_R_RES_ACK | UEP_T_RES_NAK;
    UEP2_CTRL = bUEP_AUTO_TOG | UEP_R_RES_ACK | UEP_T_RES_NAK;
    USB_DEV_AD = 0x00;
    UDEV_CTRL = bUD_PD_DIS;                                         // 禁止DP/DM下拉电阻
    USB_CTRL = bUC_DEV_PU_EN | bUC_INT_BUSY | bUC_DMA_EN;           // 启动USB设备及DMA，在中断期间中断标志未清除前自动返回NAK
    UDEV_CTRL |= bUD_PORT_EN;                                       // 允许USB端口
    USB_INT_FG = 0xFF;                                              // 清中断标志
    USB_INT_EN = bUIE_SUSPEND | bUIE_TRANSFER | bUIE_BUS_RST;
}
/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main( void )
{
    UINT32 j=0;
    CfgFsys();
    mDelaymS(5);
    mInitSTDIO( );
    printf("IAP Demo start...\n");
    EA = 0;                                                              //关闭中断，IAP方式无法使用中断
    P1_MOD_OC |= (1<<0);
    P1_DIR_PU |= (1<<0);
    uart_bit = 0;
    InitUSB_Device( );                                                   //USB设备模式初始化函数，查询方式
    while(1)
    {
        USB_DeviceInterrupt( );                                          // 查询usb中断,建议不要频繁查询
        /* 退出iap下载 */
        if( DisableIAP == 0 )                                            // 查询P10低电平时执行软复位，从新执行用户程序
        {
            printf("soft reset\n");
            CH549SoftReset();                                            //软复位
        }
        j++;
        if(j == 50000)
        {
            D2 = ~D2;                                                      //D2快闪
            j = 0;
        }
    }
}