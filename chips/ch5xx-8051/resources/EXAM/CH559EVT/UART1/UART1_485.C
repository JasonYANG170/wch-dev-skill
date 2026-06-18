/********************************** (C) COPYRIGHT *******************************
* File Name          : UART1_485.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        : 提供UART1的配置函数，提供查询方式进行数据收发，以及串口FIFO的使用注意事项 
                       UART1的485模式收发数据，必须设置半双工
											 演示时需要其他串口向CH559的串口输入数据；
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/                                         
#include "..\DEBUG.C"                                                         //调试信息打印
#include "..\DEBUG.H"

#pragma  NOAREGS

#define CH559UART1_BPS    57600                                               /*定义CH559串口1通讯波特率*/
#define CH559UART1_FIFO_EN   1                                                //使能CH559串口1接收FIFO(接收发送各8字节)

#if CH559UART1_FIFO_EN
UINT8 CH559UART1_FIFO_CNT;
#define  CH559UART1_FIFO_TRIG  7                                              //FIFO满7字节触发中断(1、2、4、7可选)
#endif

UINT8 Str[] = {"hello world1234!"};

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
    SER1_IER |= /*bIER_MODEM_CHG | */bIER_LINE_STAT | bIER_THR_EMPTY | bIER_RECV_RDY;//中断使能配置
 
#if  CH559UART1_FIFO_EN
    SER1_FCR |= MASK_U1_FIFO_TRIG | bFCR_T_FIFO_CLR | bFCR_R_FIFO_CLR | bFCR_FIFO_EN;//FIFO控制器
                                                                               //清空接收、发送FIFO，7字节接收触发，FIFO使能
#endif
    SER1_MCR |= bMCR_OUT2;                                                     //MODEM控制寄存器
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


#if CH559UART1_FIFO_EN
/*******************************************************************************
* Function Name  : CH559UART1Rcv(PUINT8 buf,UINT8 len)
* Description    : CH559UART1接收多字节,必须开FIFO
* Input          : PUINT8 buf  数据存储缓冲区
                   UINT8 len   数据预接收长度
* Output         : None
* Return         : None
*******************************************************************************/
void CH559UART1Rcv(PUINT8 buf,UINT8 len)
{
    UINT8 i,j;
    j = 0;
    while(len)
    {
         if(len >= CH559UART1_FIFO_TRIG)//预接收长度超过FIFO接收触发级
         {
             while((SER1_IIR & U1_INT_RECV_RDY) == 0);//等待数据可用中断
             for(i=0;i<CH559UART1_FIFO_TRIG;i++)
             {
                 *(buf+j) = SER1_RBR; 
                 j++;
             }
             len -= CH559UART1_FIFO_TRIG;             
         }
         else
         {
             while((SER1_LSR & bLSR_DATA_RDY) == 0);//等待数据准备好
             *(buf+j) = SER1_RBR; 
             j++;
             len--;              
         }    
    }

}
#endif

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
* Function Name  : CH559UART1SendStr(PUINT8 SendStr)
* Description    : CH559UART1发送多个字节
* Input          : UINT8 SendStr ；要发送的数据的首地址
* Output         : None
* Return         : None
*******************************************************************************/
void CH559UART1SendStr(PUINT8 SendStr)
{
#if CH559UART1_FIFO_EN
    while(1)
    {
        if(SER1_LSR & bLSR_T_FIFO_EMP) CH559UART1_FIFO_CNT=8;//FIFO空可以填入最多8字节
        while( CH559UART1_FIFO_CNT!=0 ) 
        {
            if( *SendStr == '\0' ) break;//发送完毕
            SER1_THR = *SendStr++;
            CH559UART1_FIFO_CNT--;//FIFO计数
        }
        if( *SendStr == '\0' ) break;//发送完毕
        while ( (SER1_LSR & bLSR_T_FIFO_EMP) == 0 );//发现FIFO满，只能等待前面1字节发送完成
		}         
#else
    while( *SendStr != '\0' )
    {
        SER1_THR = *SendStr++;
        while ( (SER1_LSR & bLSR_T_ALL_EMP) == 0 );//没开FIFO，等待1字节发送完成
    }
#endif	
}

main( ) 
{
   UINT8 i,j;
   UINT16 cnt;
   UINT8 buffer[20];
   UINT8 tmp[16]={0x13,0xab,0x5f,0x9d,0x32,0xde,0x56,0xaa,0x23,0x28,0x36,0x48,0x59,0x46,0x96,0xdd};
//  CfgFsys( );                                                                //时钟配置   
//  mDelaymS(5);                                                              //等待外部晶振稳定 	 	
    
    mInitSTDIO( );                                                             //串口0,可以用于调试
    printf("start ...\n");
	
    CH559UART1Init(13,0,2);
//  UART1RegCfgValue( );                                                       //UART1寄存器配置
    P4_DIR |= 0xff;                                                            //使用P4口时，一定要设置方向,TXD1置为输出
    P4_OUT = 0xff;
    cnt = 0;
/*查询方式*/
   while(1)
   {
#if 0
       CH559UART1SendByte(0x13);//数据发送
       CH559UART1SendByte(0xab);
       CH559UART1SendByte(0x5f);
       CH559UART1SendByte(0x9d);
       CH559UART1SendByte(0x32);
       CH559UART1SendByte(0xde);
       CH559UART1SendByte(0x56);
       CH559UART1SendByte(0xAA);
       CH559UART1SendByte(0x23);
       CH559UART1SendByte(0x28);
       CH559UART1SendByte(0x36);
       CH559UART1SendByte(0x48);
       CH559UART1SendByte(0x59);
       CH559UART1SendByte(0x46);
       CH559UART1SendByte(0x96);
       CH559UART1SendByte(0xDD);
//     CH559UART1SendStr(Str);
       mDelaymS(500);
       P4_OUT = ~P4_OUT;
#endif

#if 1
#if CH559UART1_FIFO_EN//数据接收
       CH559UART1Rcv(buffer,16);
       for(j=0;j<16;j++) 
       {
           if(tmp[j] != buffer[j]) 
           {
              cnt++;
              printf("Error CNT: %d     ",cnt);
              for(i = 0;i < 16;i++)
              {
                  printf("%02X  ",(UINT16)buffer[i]);
              }
              printf("\n");
							PWM1_ = 0;//数据出错
              break;
           }
           else
           {
              RXD1_ = ~RXD1_;//P4.0闪正常运行
           }
       }
       for(i = 0;i < 16;i++)
       {
           printf("%02X  ",(UINT16)buffer[i]);				 
       }
       printf("\n");
#else
       for(i=0;i<16;i++)
			 {
           buffer[i] = CH559UART1RcvByte();
       }
       for(i = 0;i < 16;i++)
       {
           printf("%02X  ",(UINT16)buffer[i]);
       }
       printf("\n");
#endif
#endif
   }
}
