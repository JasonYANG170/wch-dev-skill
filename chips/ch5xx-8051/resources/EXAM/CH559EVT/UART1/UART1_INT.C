/********************************** (C) COPYRIGHT *******************************
* File Name          : UART1_INT.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : 提供UART1的配置函数，提供中断方式进行数据收发，以及串口FIFO的使用注意事项    
                       中断方式数据收发
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/                                            
#include "..\DEBUG.C"                                                         //调试信息打印
#include "..\DEBUG.H"

#pragma  NOAREGS

#define CH559UART1_BPS    4800                                               /*定义CH559串口1通讯波特率*/
#define CH559UART1_FIFO_EN   1                                               //使能CH559串口1接收FIFO(接收发送各8字节)

#if CH559UART1_FIFO_EN
UINT8 CH559UART1_FIFO_CNT = 0;
UINT8 UART1SendLen = 0;                                                       //数据发送计数
#define  CH559UART1_FIFO_TRIG  7                                              //FIFO满7字节触发中断(1、2、4、7可选)
#endif

UINT8 Str[] = {"hello world!"};
UINT8 buffer[20];
UINT8 Num;                                                                    //接收计数

/*******************************************************************************
* Function Name  : UART1RegCfgValue()
* Description    : CH559UART1可读寄存器正确配置后的值
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  UART1RegCfgValue( )
{
    printf("SER1_IER %02X\n",(UINT16)SER1_IER);                              //0x27/0x17/0x37都是可能的
    printf("SER1_IIR %02X\n",(UINT16)SER1_IIR);                              //0xc1/0xC2无中断或者空中断，"C"表示FIFO开启
    printf("SER1_LCR %02X\n",(UINT16)SER1_LCR);                              //0x03数据格式的配置，表示无线路间隔，无校验，1位停止位，8位数据位
    printf("SER1_MCR %02X\n",(UINT16)SER1_MCR);                              //0x08中断输出使能，不包括流控开启等其他功能
    printf("SER1_LSR %02X\n",(UINT16)SER1_LSR);                              //0x60，FIFO和线路状态
    printf("SER1_MSR %02X\n",(UINT16)SER1_MSR);
}

/*******************************************************************************
* Function Name  : ResetUART1( )
* Description    : CH559UART1端口软复位
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void ResetUART1( )
{
    SER1_IER |= bIER_RESET;                                                   //该位可以自动清空，复位串口寄存器
}

/*******************************************************************************
* Function Name  : CH559UART1Init(UINT8 DIV,UINT8 mode,UINT8 pin)
* Description    : CH559 UART1初始化设置
* Input          : 
                   UINT8 DIV设置分频系数，时钟频率=Fsys/DIV,DIV不能为0
                   UINT8 mode，模式选择，1：普通串口模式；0:485模式
                   UINT8 pin，串口引脚选择；
                   当mode=1时
                   0：RXD1=P4.0,TXD1关闭；
                   1：RXD1&TXD1=P4.0&P4.4；
                   2：RXD1&TXD1=P2.6&P2.7；
                   3：RXD1&TXD1&TNOW=P2.6&P2.7&P2.5；
                   当mode=0时
                   0：无意义
                   1：P5.4&P5.5连接485,TNOW=P4.4；
                   2：P5.4&P5.5连接485；
                   3：P5.4&P5.5连接485,TNOW=P2.5；
* Output         : None
* Return         : None
*******************************************************************************/
void CH559UART1Init(UINT8 DIV,UINT8 mode,UINT8 pin)
{
    UINT32 x;
    UINT8 x2; 

    SER1_LCR |= bLCR_DLAB;                                                    // DLAB位置1，写DLL、DLM和DIV寄存器
    SER1_DIV = DIV;                                                           // 预分频
    x = 10 * FREQ_SYS *2 / DIV / 16 / CH559UART1_BPS;                             
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x ++;                                                      //四舍五入
    SER1_DLM = x>>8;
    SER1_DLL = x&0xff;
    SER1_LCR &= ~bLCR_DLAB;                                                   //DLAB位置0,防止修改UART1波特率和时钟
    if(mode == 1)                                                             //关闭RS485模式 RS485_EN = 0,不能省略
    {
	      XBUS_AUX |=  bALE_CLK_EN;                                     
    }
    else if(mode == 0)                                                        //开启RS485模式 RS485_EN = 1;
    {
		    UHUB1_CTRL |= bUH1_DISABLE;                                   
		    PIN_FUNC &= ~bXBUS_CS_OE;
		    PIN_FUNC |= bXBUS_AL_OE;
		    XBUS_AUX &= ~bALE_CLK_EN;	
		    SER1_MCR |= bMCR_HALF;                                                //485模式只能使用半双工模式	    
    }
    SER1_LCR |= MASK_U1_WORD_SZ;                                              //线路控制
    SER1_LCR &= ~(bLCR_PAR_EN | bLCR_STOP_BIT);                               //无线路间隔，无校验，1位停止位，8位数据位

    SER1_MCR &= ~bMCR_TNOW;
    SER1_IER |= bIER_EN_MODEM_O;
    SER1_IER |= ((pin << 4) & MASK_U1_PIN_MOD);                                //串口模式配置
    SER1_IER |= /*bIER_MODEM_CHG | */bIER_LINE_STAT | /*bIER_THR_EMPTY |*/ bIER_RECV_RDY;//中断使能配置
 
#if  CH559UART1_FIFO_EN
    SER1_FCR |= MASK_U1_FIFO_TRIG | bFCR_T_FIFO_CLR | bFCR_R_FIFO_CLR | bFCR_FIFO_EN;//FIFO控制器
                                                                               //清空接收、发送FIFO，7字节接收触发，FIFO使能
#endif
    SER1_MCR |= bMCR_OUT2;                                                     //MODEM控制寄存器
    x2 = SER1_IIR;                                                             //读IIR清中断
                                                                               //中断请求输出，不产生实际中断
    SER1_ADDR |= 0xff;                                                         //关闭多机通信
}

/*******************************************************************************
* Function Name  : CH559UART1RcvByte()
* Description    : CH559UART1接收一个字节
* Input          : None
* Output         : None
* Return         : 正确：UINT8 Rcvdat;接收数据
*******************************************************************************/
UINT8  CH559UART1RcvByte( )
{
    while((SER1_LSR & bLSR_DATA_RDY) == 0);                                   //等待数据准备好
    return SER1_RBR;
}

/*******************************************************************************
* Function Name  : CH559UART1SendByte(UINT8 SendDat)
* Description    : CH559UART1发送一个字节
* Input          : UINT8 SendDat；要发送的数据
* Output         : None
* Return         : None
*******************************************************************************/
void CH559UART1SendByte(UINT8 SendDat)
{
#if CH559UART1_FIFO_EN
    while(1)
    {
        if(SER1_LSR & bLSR_T_FIFO_EMP) CH559UART1_FIFO_CNT=8;//FIFO空可以填入最多8字节
        if ( CH559UART1_FIFO_CNT!=0 ) 
        {
            SER1_THR = SendDat;
            CH559UART1_FIFO_CNT--;//FIFO计数
            break;
        }
        while ( (SER1_LSR & bLSR_T_FIFO_EMP) == 0 );//发现FIFO满，只能等待前面1字节发送完成
		}         
#else
    while ( (SER1_LSR & bLSR_T_ALL_EMP) == 0 );//没开FIFO，等待1字节发送完成
    SER1_THR = SendDat;
#endif
}

/*******************************************************************************
* Function Name  : CH559UART1INTSendStr(PUINT8 SendStr)
* Description    : CH559UART1发送空中断发送多个字节
* Input          : UINT8 SendStr ；要发送的数据的首地址
* Output         : None
* Return         : None
*******************************************************************************/
void CH559UART1INTSendStr(PUINT8 SendStr)
{
    PUINT8  tmp;
    tmp = SendStr+UART1SendLen;
	
#if CH559UART1_FIFO_EN
	
    CH559UART1_FIFO_CNT=CH559UART1_FIFO_TRIG;//FIFO空可以填入最多7字节
    while( CH559UART1_FIFO_CNT!=0 ) 
    {
        if( *tmp == '\0' )//发送完毕
        {
            UART1SendLen = 0;
            break;
        }
        SER1_THR = *tmp++;
        UART1SendLen++;
        CH559UART1_FIFO_CNT--;//FIFO计数
    }
		
#else
		
    if( *tmp != '\0' )//继续发送
    {
        SER1_THR = *tmp++;
        UART1SendLen += 1;  
    }
    else//发送完毕
 		{
        UART1SendLen = 0;        
		}
		
#endif	
}

/*******************************************************************************
* Function Name  : UART1Interrupt(void)
* Description    : UART1中断服务程序 
*******************************************************************************/
void	UART1Interrupt( void ) interrupt INT_NO_UART1 using 1                 //UART1中断服务程序,使用寄存器组1
{
    UINT8 InterruptStatus,i; 
    InterruptStatus = SER1_IIR & 0x0f;                                        //获取中断状态 
//     printf("InterruptStatus %02X\n",(UINT16)InterruptStatus);
    switch(InterruptStatus)
    {
        case U1_INT_RECV_RDY:                                                 //接收数据可用中断，可以先读取指定字节数触发中断的数据个数
#if CH559UART1_FIFO_EN
			       for(i = 0;i < CH559UART1_FIFO_TRIG;i++)                          //直接读取触发点的字节数
			       {
                  if(Num >= 19)
                  {
                      Num = 19;
                  }								 
				          buffer[Num] = SER1_RBR;										
				          Num++;
			       }
#else
             if(Num >= 19)
             {
                 Num = 19;
             }						 
             buffer[Num] = SER1_RBR;                                          //FIFO没开，取1个字节
				     Num++;
#endif
			   break;				
        case U1_INT_RECV_TOUT:                                                //接收超时中断
             if(Num >= 19)
             {
                 Num = 19;
             }					
             buffer[Num] = CH559UART1RcvByte( );
				     Num++;
			    break;		
        case U1_INT_LINE_STAT:                                                //线路状态中断
			   break;		
        case U1_INT_SLV_ADDR:                                                 //设备地址match中断
			   break;			
        case U1_INT_NO_INTER:                                                 //无中断
          break;		
        case U1_INT_MODEM_CHG:                                                //MODEM中断
		         i = SER1_MSR;
         break;	
        case U1_INT_THR_EMPTY:                                                //发送空中断，可以启动下次发送或者等待接收
             CH559UART1INTSendStr(Str);
             if(UART1SendLen == 0)//发送完成
             {
                 SER1_IER &= ~bIER_THR_EMPTY;                                     //缓冲区满，手动使能发送空中断，可以进行数据发送	
             }
			   break;
        default:
         break;
	}                                                                    
}

main( ) 
{
    UINT8 i;
//  CfgFsys( );                                                                //时钟配置   
//  mDelaymS(5);                                                              //等待外部晶振稳定 	 		
    
    mInitSTDIO( );                                                             //串口0,可以用于调试
    printf("start ...\n");

/*中断方式*/
    IE_UART1 = 1;                                                              //UART1中断使能
    EA = 1;                                                                    //开全局中断
    Num = 0;
	
    CH559UART1Init(13,1,2);
    UART1RegCfgValue( );                                                       //UART1寄存器配置
    P4_DIR |= 0x10;                                                            //使用P4口时，一定要设置方向,TXD1置为输出

    while(1)
    {
        if(Num >= 19)
        {
            for(i = 0;i < 20;i++)
            {
                printf("%02X  ",(UINT16)buffer[i]);				 
            }
            SER1_IER |= bIER_THR_EMPTY;                                       //缓冲区满，手动使能发送空中断，可以进行数据发送	
            Num = 0;
				}
    }
}
