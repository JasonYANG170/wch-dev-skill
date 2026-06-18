/********************************** (C) COPYRIGHT *******************************
* File Name          : IAP_DEMO.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 IAP DEMO

*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>

#ifndef    DEBUG
#define    DEBUG  1
#endif

#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "ISPXT56X.H"
#include "CH56X_IAP.H"
                              

/* 连接一个LED用于监控演示程序的进度,低电平LED亮 */
#define LED_OUT_INIT( LED )     { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }          /* LED 高电平为输出方向 */
#define LED_OUT_ACT( LED )      { R32_PB_CLR |= LED; }                             /* LED 低电平驱动LED显示 */
#define LED_OUT_INACT( LED )    { R32_PB_OUT |= LED; }                             /* LED 高电平关闭LED显示 */

iap_cmd1  iap_cmd;


/*******************************************************************************
* CH563EVT D4指示灯闪烁程序
* 缓冲区必须四字节对齐（__align(4)）
*******************************************************************************/
__align(4)   CONST UINT8 CH563DEMO_BIN[] ={
    0x0B, 0x00, 0x00, 0xEA, 0x05, 0x00, 0x00, 0xEA, 0x07, 0x00, 0x00, 0xEA, 0x04, 0x00, 0x00, 0xEA, 
    0x04, 0x00, 0x00, 0xEA, 0x42, 0x00, 0x00, 0x00, 0x22, 0x00, 0x00, 0xEA, 0x23, 0x00, 0x00, 0xEA, 
    0xFE, 0xFF, 0xFF, 0xEA, 0xFE, 0xFF, 0xFF, 0xEA, 0xFE, 0xFF, 0xFF, 0xEA, 0xFE, 0xFF, 0xFF, 0xEA, 
    0xFE, 0xFF, 0xFF, 0xEA, 0x50, 0x00, 0x9F, 0xE5, 0xDB, 0xF0, 0x21, 0xE3, 0x00, 0xD0, 0xA0, 0xE1, 
    0x00, 0x00, 0x40, 0xE2, 0xD7, 0xF0, 0x21, 0xE3, 0x00, 0xD0, 0xA0, 0xE1, 0x00, 0x00, 0x40, 0xE2, 
    0xD3, 0xF0, 0x21, 0xE3, 0x00, 0xD0, 0xA0, 0xE1, 0x10, 0x00, 0x40, 0xE2, 0xD2, 0xF0, 0x21, 0xE3, 
    0x00, 0xD0, 0xA0, 0xE1, 0x01, 0x0C, 0x40, 0xE2, 0xD1, 0xF0, 0x21, 0xE3, 0x00, 0xD0, 0xA0, 0xE1, 
    0x01, 0x0C, 0x40, 0xE2, 0x1F, 0xF0, 0x21, 0xE3, 0x00, 0xD0, 0xA0, 0xE1, 0x01, 0xAB, 0x4D, 0xE2, 
    0x2D, 0x00, 0x00, 0xFA, 0x01, 0x00, 0x00, 0xEB, 0xFE, 0xFF, 0xFF, 0xEA, 0x00, 0x00, 0x82, 0x00, 
    0x0C, 0xD0, 0x9F, 0xE5, 0x3E, 0x00, 0x00, 0xFA, 0x00, 0x00, 0x9F, 0xE5, 0x10, 0xFF, 0x2F, 0xE1, 
    0xCD, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x81, 0x00, 0x00, 0x00, 0xA0, 0xE1, 0xFE, 0xFF, 0xFF, 0xEA, 
    0x00, 0x00, 0xA0, 0xE1, 0xFE, 0xFF, 0xFF, 0xEA, 0x1E, 0x4B, 0x18, 0x70, 0xC0, 0x46, 0x1D, 0x4A, 
    0x52, 0x79, 0x20, 0x23, 0x1A, 0x42, 0xFA, 0xD0, 0x70, 0x47, 0xC0, 0x46, 0x1A, 0x48, 0x80, 0x6A, 
    0x38, 0x21, 0x08, 0x43, 0x18, 0x49, 0x88, 0x62, 0x08, 0x00, 0x00, 0x6A, 0x38, 0x21, 0x08, 0x43, 
    0x15, 0x49, 0x08, 0x62, 0x08, 0x00, 0x00, 0x6A, 0x80, 0x21, 0x88, 0x43, 0x12, 0x49, 0x08, 0x62, 
    0x08, 0x00, 0x00, 0x6B, 0x80, 0x21, 0x08, 0x43, 0x0F, 0x49, 0x08, 0x63, 0x18, 0xE0, 0x60, 0x1C, 
    0x04, 0x06, 0x24, 0x0E, 0xC8, 0x2C, 0x00, 0xD1, 0x00, 0x24, 0x00, 0x2C, 0x05, 0xD1, 0x0A, 0x48, 
    0x80, 0x6A, 0x20, 0x21, 0x08, 0x43, 0x08, 0x49, 0x88, 0x62, 0x64, 0x2C, 0x05, 0xD1, 0x06, 0x48, 
    0xC0, 0x6A, 0x20, 0x21, 0x08, 0x43, 0x04, 0x49, 0xC8, 0x62, 0x01, 0x20, 0x00, 0xF0, 0x23, 0xF8, 
    0xE5, 0xE7, 0x00, 0x00, 0x00, 0xD0, 0x40, 0x00, 0x80, 0x00, 0x40, 0x00, 0x57, 0x20, 0x01, 0x21, 
    0x89, 0x05, 0x08, 0x70, 0xA8, 0x20, 0x08, 0x70, 0x14, 0x20, 0xC8, 0x72, 0x02, 0x20, 0x88, 0x72, 
    0xBC, 0x20, 0x48, 0x72, 0x80, 0x20, 0x08, 0x71, 0x00, 0x20, 0x08, 0x70, 0x70, 0x47, 0x01, 0x00, 
    0x04, 0xE0, 0x18, 0x20, 0x00, 0xE0, 0x40, 0x1E, 0x00, 0x28, 0xFC, 0xD1, 0x0A, 0x00, 0x49, 0x1E, 
    0x00, 0x2A, 0xF6, 0xD1, 0x70, 0x47, 0x01, 0x00, 0x04, 0xE0, 0x05, 0x48, 0x00, 0xE0, 0x40, 0x1E, 
    0x00, 0x28, 0xFC, 0xD1, 0x0A, 0x00, 0x49, 0x1E, 0x00, 0x2A, 0xF6, 0xD1, 0x70, 0x47, 0x00, 0x00, 
    0x46, 0x5F, 0x00, 0x00, 0x05, 0x4C, 0x06, 0x4D, 0x03, 0xE0, 0x20, 0x00, 0x0F, 0xC8, 0x98, 0x47, 
    0x10, 0x34, 0xAC, 0x42, 0xF9, 0xD3, 0xFF, 0xF7, 0x78, 0xEF, 0x00, 0x00, 0xB4, 0x01, 0x00, 0x00, 
    0xB4, 0x01, 0x00, 0x00
}; 

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void IRQ_Handler( void )
{
    while(1);
}

/*******************************************************************************
* Function Name  : FIQ_Handler
* Description    : FIQ中断函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
__irq void FIQ_Handler( void )
{
    while(1);
}

/*******************************************************************************
* Function Name  : mInitSTDIO
* Description    : 为printf和getkey输入输出初始化串口
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void mInitSTDIO( void )
{
    UINT32    x, x2;

    x = 10 * FREQ_SYS/ 8 / 115200;                                              /* 115200bps */
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x ++;                                                        /* 四舍五入 */
    R8_UART0_LCR = 0x80;                                                        /* DLAB位置1 */
    R8_UART0_DIV = 1;                                                           /* 预分频 */
    R8_UART0_DLM = x>>8;
    R8_UART0_DLL = x&0xff;

    R8_UART0_LCR = RB_LCR_WORD_SZ ;                                             /* 设置字节长度为8 */
    R8_UART0_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR |    
                   RB_FCR_FIFO_EN ;                                             /* 设置FIFO触发点为14，清发送和接收FIFO，FIFO使能 */
    R8_UART0_IER = RB_IER_TXD_EN;                                               /* TXD enable */
    R32_PB_SMT |= RXD0|TXD0;                                                    /* RXD0 schmitt input, TXD0 slow rate */
    R32_PB_PD &= ~ RXD0;                                                        /* disable pulldown for RXD0, keep pullup */
    R32_PB_DIR |= TXD0;                                                         /* TXD0 output enable */
}

/*******************************************************************************
* Function Name  : fputc
* Description    : 通过串口输出监控信息
* Input          : c-- writes the character specified by c 
*                  *f--the output stream pointed to by *f
* Output         : None
* Return         : None
*******************************************************************************/
int fputc( int c, FILE *f )
{
    R8_UART0_THR = c;                                                           /* 发送数据 */
    while( ( R8_UART0_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );                        /* 等待数据发送 */
    return( c );
}
/*******************************************************************************
* Function Name  : UART_Send
* Description    : 串口1发送字节子程序
* Input          : dat -要发送的数据
* Output         : None
* Return         : None
*******************************************************************************/
void UART_Send( UINT8 dat )   
{        
    R8_UART1_THR  = dat;
    while( ( R8_UART1_LSR & RB_LSR_TX_ALL_EMP ) == 0 );                         /* 等待数据发送 */       
}

/*******************************************************************************
* Function Name  : UART_Receive
* Description    : 串口1接收一字节子程序
* Input          : None
* Output         : None
* Return         : Rcvdat -接收到的数据 
*******************************************************************************/
UINT8 UART_Receive( void )    
{
    UINT8 Rcvdat = 0;
        
    if( !( ( R8_UART1_LSR  ) & ( RB_LSR_OVER_ERR |RB_LSR_PAR_ERR  | RB_LSR_FRAME_ERR |  RB_LSR_BREAK_ERR  ) ) ) {
        while( ( R8_UART1_LSR & RB_LSR_DATA_RDY  ) == 0 );                      /* 等待数据准备好 */ 
        Rcvdat = R8_UART1_RBR;                                                  /* 从接收缓冲寄存器读出数据 */ 
    }
    else{
        R8_UART1_RBR;                                                           /* 有错误清除 */
    }
    return( Rcvdat );
}

/*******************************************************************************
* Function Name  : Uart1_Init
* Description    : 串口1初始化
* Input          : baud-串口波特率，最高为主频1/8
* Output         : None
* Return         : None
*******************************************************************************/
void Uart1_Init( UINT32 baud )
{
    UINT32 x;

    x = 10 * FREQ_SYS/ 8 / baud;                                                /* 115200bps */
    x += 5;                                                                     /* 四舍五入 */
    x /= 10;
    R8_UART1_LCR = RB_LCR_DLAB;                                                 /* DLAB位置1 */
    R8_UART1_DIV = 1;                                                           /* 预分频 */
    R8_UART1_DLM = x>>8;
    R8_UART1_DLL = x&0xff;

    R8_UART1_LCR = RB_LCR_WORD_SZ ;                                             /* 设置字节长度为8 */
    R8_UART1_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN ;
                                                                                /* 设置FIFO触发点为28，清发送和接收FIFO，FIFO使能 */
    R8_UART1_IER = RB_IER_TXD_EN;                                               /* TXD enable */
    R32_PB_SMT |= RXD1|TXD1;                                                    /* RXD1 schmitt input, TXD1 slow rate */
    R32_PB_PU  |= RXD1;                                                         /* disable pulldown for RXD0, keep pullup */
    R32_PB_DIR |= TXD1;                                                         /* TXD1 output enable */
}

/*******************************************************************************
* Function Name  : CH56X_Respond
* Description    : 
* Input          : 
* Output         : None
* Return         : None
*******************************************************************************/
void CH56X_Respond( UINT8 s )
{
   UART_Send( s );
   UART_Send( 0x00 );
}

/*******************************************************************************
* Function Name  : CH56X_IAPDownload
* Description    : CH56x下载函数
*                ：
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH56X_IAPDownload( void )
{
    UINT8  s;
    UINT16 i;
    UINT32 addr;

    switch( iap_cmd.other.buf[0] ){                                                    // 分析命令码
        case CMD_IAP_PROM:                                                             // ISP 编程命令
            addr = (iap_cmd.program.addr[0] | (UINT16)iap_cmd.program.addr[1]<<8);
            s = FLASH_ROM_WRITE( addr, (PVOID)&iap_cmd.program.buf[0], iap_cmd.program.len );       /* 更新用户程序，用户程序从0x00000000地址开始 */                        
            CH56X_Respond( s );
            break;
        case CMD_IAP_ERASE:                                                            // ISP 擦除命令
            FLASH_ROM_LOCK( 0x00 );                                                    // FLASH解锁
            s = FLASH_ROM_ERASE( 0 ,1024*128 );                                        /* 擦除flash */
            CH56X_Respond( s );
            break;    
        case CMD_IAP_VERIFY:                                                           // ISP 校验命令
            addr = (iap_cmd.verify.addr[0] | (UINT16)iap_cmd.verify.addr[1]<<8);
            s = FLASH_ROM_VERIFY( addr, (PVOID)&iap_cmd.verify.buf[0], iap_cmd.verify.len );      /*  校验是否正确 */
            CH56X_Respond( s );
            break;
        case CMD_IAP_END:                                                              // ISP 结束命令
            FLASH_ROM_LOCK( FLASH_ROM_LOCK_AUTO );                                     // 全部LOCK
            R8_SAFE_ACCESS_SIG = 0x57 ;                                                /* unlock step 1 */
            R8_SAFE_ACCESS_SIG = 0xA8 ;                                                /* unlock step 2 */                     
            R8_GLOB_RST_CFG = 0x40 | RB_GLOB_FORCE_RST;                                // reset
            R8_SAFE_ACCESS_SIG = 0;                                                    /* 保护，禁止读写 */
            break;
        default:
            CH56X_Respond( 0xfe );                                                     // 未知的命令
            break;
    }
}

/*******************************************************************************
* Function Name  : UATR_Interrupt
* Description    : 串口接收处理函数
*                ：
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UATR_Handle( void  )    
{
    UINT8 dat,i,len,add_value;

    dat = UART_Receive();
    if( dat == Uart_Sync_Head1 ){
        dat = UART_Receive();
        if( dat == Uart_Sync_Head2 ){
            iap_cmd.other.buf[0] = UART_Receive();                              /*命令码*/
            add_value = 0;
            add_value+=iap_cmd.other.buf[0];
            len = iap_cmd.other.buf[1] = UART_Receive();                        /*后续数据长度*/
            add_value+=iap_cmd.other.buf[1];
            if( iap_cmd.other.buf[0] == CMD_IAP_PROM  || iap_cmd.other.buf[0] == CMD_IAP_VERIFY ){    /*命令码为编程校验需要增加2字节*/
                len+=2;
            }                    
            for( i=0;i!=len;i++ ){
                iap_cmd.other.buf[i+2] = UART_Receive();
                add_value+=iap_cmd.other.buf[i+2];
            }
            i = UART_Receive();
            if( add_value != i ){
                UART_Send( 0x55 );
                UART_Send( 0xaa );                                              /*累加和错误，要求计算机重发*/                                
            }
            else{
                CH56X_IAPDownload(  );
            }
        }
    }
}

/*******************************************************************************
* Function Name  : CH563_FLASH_CODE
* Description    : 更新falsh
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563_FLASH_CODE( void ) 
{
    UINT32 i;
    
    FLASH_ROM_LOCK( 0x00 );                                                     // FLASH解锁
    i = FLASH_ROM_ERASE( 0 ,1024*128 );                                         /* 擦除flash */
    if( i ){
        PRINT("    ***Erase Error***\n");
    }
    else{
        PRINT("    *** erase ok***\n");
    }
    PRINT("flash Write:\n");
    i = FLASH_ROM_WRITE( 0,(PVOID)CH563DEMO_BIN,sizeof(CH563DEMO_BIN) );        /* 更新用户程序，用户程序从0x00000000地址开始 */                        
    if( i ){
        PRINT("    *** Write Error***\n");
    }
    else{
        PRINT("    *** write ok***\n");
    }
    PRINT("flash  verify:\n");
    i = FLASH_ROM_VERIFY( 0,(PVOID)CH563DEMO_BIN,sizeof(CH563DEMO_BIN) );       /*  校验是否正确 */
    if( i ){
        PRINT("    *** verify Error***\n");
        while(1){
            LED_OUT_ACT( (1<<3|1<<4|1<<5) );                                    // 升级失败
            Delay_ms( 500 );
            LED_OUT_INACT( (1<<3|1<<4|1<<5) );        
            Delay_ms( 500 );
        }
    }
    else{
        PRINT("    *** verify ok***\n");
        LED_OUT_ACT( (1<<3|1<<4|1<<5) );                                        // 升级完成
        Delay_ms( 500 );
    }
    FLASH_ROM_LOCK( FLASH_ROM_LOCK_AUTO );                                      // 全部LOCK
    PRINT("\n");
    R8_SAFE_ACCESS_SIG = 0x57 ;                                                 /* unlock step 1 */
    R8_SAFE_ACCESS_SIG = 0xA8 ;                                                 /* unlock step 2 */                     
    R8_GLOB_RST_CFG = 0x40 | RB_GLOB_FORCE_RST;                                 // reset
    R8_SAFE_ACCESS_SIG = 0;                                                     /* 保护，禁止读写 */
}



/*******************************************************************************
* Function Name  : CH563_FLASH_CODE
* Description    : 更新falsh
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563_FLASH_CODE1( void ) 
{
    UINT32 i;
    
    FLASH_ROM_LOCK( 0x00 );        // FLASH解锁
    i = FLASH_ROM_ERASE( 0 ,1024*128 );                                        /* 擦除flash */
    if( i ){
        PRINT("    ***Erase Error***\n");
    }
    else{
        PRINT("    *** erase ok***\n");
    }
    PRINT("flash Write:\n");
    i = FLASH_ROM_WRITE( 0,(PVOID)CH563DEMO_BIN,sizeof(CH563DEMO_BIN) );       /* 更新用户程序，用户程序从0x00000000地址开始 */                        
    if( i ){
        PRINT("    *** Write Error***\n");
    }
    else{
        PRINT("    *** write ok***\n");
    }
    PRINT("flash  verify:\n");
    i = FLASH_ROM_VERIFY( 0,(PVOID)CH563DEMO_BIN,sizeof(CH563DEMO_BIN) );      /*  校验是否正确 */
    if( i ){
        PRINT("    *** verify Error***\n");
        while(1){
            LED_OUT_ACT( (1<<3|1<<4|1<<5) );                                    // 升级失败
            Delay_ms( 500 );
            LED_OUT_INACT( (1<<3|1<<4|1<<5) );        
            Delay_ms( 500 );
        }
    }
    else{
        PRINT("    *** verify ok***\n");
        LED_OUT_ACT( (1<<3|1<<4|1<<5) );                                        // 升级完成
        Delay_ms( 500 );
    }
    FLASH_ROM_LOCK( FLASH_ROM_LOCK_AUTO );                                      // 全部LOCK
    PRINT("\n");
    R8_SAFE_ACCESS_SIG = 0x57 ;                                                 /* unlock step 1 */
    R8_SAFE_ACCESS_SIG = 0xA8 ;                                                 /* unlock step 2 */                     
    R8_GLOB_RST_CFG = 0x40 | RB_GLOB_FORCE_RST;                                 // reset
    R8_SAFE_ACCESS_SIG = 0;                                                     /* 保护，禁止读写 */
}

/*******************************************************************************
* Function Name  : main
* Description    : 主函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void ) 
{
    UINT8 i,my_buffer[8];

    LED_OUT_INIT( (1<<3|1<<4|1<<5) );                                           // 初始化指示灯 
    R32_PB_DIR &=  ~(1<<7);                                                     // PB7输入上拉                                
    R32_PB_PU  |=  (1<<7);                   
    mInitSTDIO( );                                                               
    PRINT("Edit Date and Time is: "__DATE__" , " __TIME__"\n");
    GET_UNIQUE_ID( my_buffer );
    PRINT("chip id: ");
    for(i=0;i<8;i++) PRINT("%-2x ",my_buffer[i]);
    PRINT("\n");
    Uart1_Init( 57600 );
    while(1){
        i++;
        if( i == 200 ) i = 0;
        if( i == 0 )   LED_OUT_INACT( 1<<3 );
        if( i == 100 ) LED_OUT_ACT( 1<<3 );
        if( (R32_PB_PIN & (1<<7) )== 0  ){                                      // 本程序中设置的更新条件
            CH563_FLASH_CODE( );    
        }
        if(  R8_UART1_LSR & RB_LSR_DATA_RDY   ) UATR_Handle( );                   
        Delay_ms( 1 );
    }
}

/*********************************** endfile **********************************/
