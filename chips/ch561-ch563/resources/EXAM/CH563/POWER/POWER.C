/********************************** (C) COPYRIGHT *******************************
* File Name          : POWER.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 USB SLEEP DEMO
*                      (1)、CH563 Examples by KEIL;    
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、通过串口0发送相应命令演示关闭、开启部分功能，睡眠，唤醒等
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include <string.h>
#include "CH563SFR.H"
#include "POWER.H"                                                              /* 芯片USB寄存器相关头文件 */
#include "SYSFREQ.H"

UINT16    my_buffer[ 0x2000 ];

/* 连接一个LED用于监控演示程序的进度,低电平LED亮 */
#define LED                     1<<3

#define LED_OUT_INIT(  )     { R32_PB_OUT |= LED; R32_PB_DIR |= LED; }         /* LED 高电平为输出方向 */
#define LED_OUT_ACT(  )      { R32_PB_CLR |= LED; }                            /* LED 低电平驱动LED显示 */
#define LED_OUT_INACT(  )    { R32_PB_OUT |= LED; }                            /* LED 高电平关闭LED显示 */

/*******************************************************************************
* Function Name  : IRQ_Handler
* Description    : IRQ中断处理函数
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
* Description    : FIQ中断处理函数
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

    x = 10 * FREQ_SYS * 2 / 16 / 115200;                                        /* 115200bps */
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
* Function Name  : USBHost_ModeSet
* Description    : 设置成USB主机模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBHost_ModeSet( void )
{ 
    R8_MISC_CTRL_USB = R8_MISC_CTRL_USB & ( ~RB_MISC_USB_ID_EN );
    R8_MISC_CTRL_USB = R8_MISC_CTRL_USB & ( ~RB_MISC_USB_ID_ST );
    OTG_CONTROL_STATUS = 0x000E0190;
}

/*******************************************************************************
* Function Name  : USBDev_ModeSet
* Description    : 设置成USB设备模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBDev_ModeSet( void )
{ 
    R8_MISC_CTRL_USB = R8_MISC_CTRL_USB & ( ~RB_MISC_USB_ID_EN );               /* USB OTG ID pin enable */                                    
    R8_MISC_CTRL_USB = R8_MISC_CTRL_USB | RB_MISC_USB_ID_ST;                    /* 设置成USB设备 */
}

/*******************************************************************************
* Function Name  : WakeUP_PB
* Description    : PB[8]唤醒配置初始化 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void WakeUP_PB( void )
{
    R32_PB_DIR &= 0xffffff00;                                                   /* GPIO B方向设置为输入 , direction: 0=in, 1=out */
    R32_PB_PU  |= 0x000000ff;                                                   /* GPIO B上拉设置，置1表示上拉 */ 
    Delay_us(10);
    R32_INT_ENABLE_PB |= 0x000000ff;                                            /* GPIO B组各位中断使能 ： 1-使能，0-禁止 */
    R32_INT_POLAR_PB  &= 0;                                                     /* PB[8]低电平唤醒 */      
}

/*******************************************************************************
* Function Name  : Test_Sleep
* Description    : 测试
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Test_Sleep( void )
{
    UINT32    i;

    if ( R8_UART0_LSR & RB_LSR_DATA_RDY ) {                                     /* 通过串口输入0x00001234类似的32位地址值 */
        Delay_ms( 20 );
        for ( i=0; i<10; i++ ){
            if ( R8_UART0_LSR & RB_LSR_DATA_RDY ) {
                my_buffer[i] = R8_UART0_RBR;
                if ( my_buffer[i] >= 'a' && my_buffer[i] <= 'z' ) my_buffer[i] -= 'a' - 'A';
                if ( i >= 2 ){
                    if ( my_buffer[i] >= '0' && my_buffer[i] <= '9' ) my_buffer[i] -= '0';
                    else if ( my_buffer[i] >= 'A' && my_buffer[i] <= 'F' ) my_buffer[i] = my_buffer[i] - 'A' + 10;
                    else i = 99;
                }
            }
            else break;
        }
        if ( i == 10 && my_buffer[0] == '0' && my_buffer[1] == 'X' ) {
            i = ( my_buffer[2] << 12 ) | ( my_buffer[3] << 8 ) | ( my_buffer[4] << 4 ) | my_buffer[5];
            i <<= 16;
            i |= ( my_buffer[6] << 12 ) | ( my_buffer[7] << 8 ) | ( my_buffer[8] << 4 ) | my_buffer[9];
            printf("recv = %8lX\n",i);
            i = *((PUINT32)i);
            printf("Data = %8lX\n",i);
        }
        else { 
            printf("my_buffer=%x\n",my_buffer[0]);      
            switch (my_buffer[0]){
                case 1:                                                         /* 寄存器写入0x95，将使能PLL进入全局休眠状态(包括CPU)，仅支持部分
                                                                                   GPIO 或者USB 或者ETH-WOL 唤醒 */
                    printf("SLEEP\n");
                    R8_SAFE_ACCESS_SIG = 0x57;                                  /* unlock step 1 */
                    R8_SAFE_ACCESS_SIG = 0xA8;                                  /* unlock step 2 */
                    R8_SLP_CTRL_PLL = 0x95;                                     /* 写0x95 */
                    R8_SLP_WAKE_CTRL  |= RB_SLP_PB_WAKE;                        /* PB端口唤醒使能 */
                    R8_SAFE_ACCESS_SIG = 0;                                     /* 保护，禁止读写 */
                    break;

                case 2:                                                         /* 寄存器写入0x6A，将使能CPU进入待机状态，此时PLL仍然在工作，CPU
                                                                                   可以被已使能的各种中断快速唤醒 */
                    printf("CPU \n");
                    R8_SAFE_ACCESS_SIG = 0x57 ;                                 /* unlock step 1 */
                    R8_SAFE_ACCESS_SIG = 0xA8 ;                                 /* unlock step 2 */
                    R8_SLP_CTRL_PLL = 0x6A;                                     /* 写0x6A */
                    R8_SLP_WAKE_CTRL  |= RB_SLP_PB_WAKE;                        /* PB端口唤醒使能 */
                    R8_SAFE_ACCESS_SIG = 0;                                     /* 保护，禁止读写 */
                    break;

                case 3:                                                         /* 以太网模块上电 */
                    printf("ETH PWR ON\n");
                    R8_SAFE_ACCESS_SIG = 0x57;                                  /* unlock step 1 */
                    R8_SAFE_ACCESS_SIG = 0xA8;                                  /* unlock step 2 */
                    R8_SLP_WAKE_CTRL  &= 0x0b;                                  /* 上电 */
                    R8_SAFE_ACCESS_SIG = 0;                                     /* 保护，禁止读写 */
                    break;

                case 4:                                                         /* 以太网模块下电 */
                    printf("ETH PWR OFF\n");
                    R8_SAFE_ACCESS_SIG = 0x57;                                  /* unlock step 1 */
                    R8_SAFE_ACCESS_SIG = 0xA8;                                  /* unlock step 2 */
                    R8_SLP_WAKE_CTRL  |= 0x80;                                  /* 下电 */
                    R8_SAFE_ACCESS_SIG = 0;                                     /* 保护，禁止读写 */
                    break;

                case 5:                                                         /* 打开ADC的PHY */
                    printf("open ADC PHY\n");
                    R8_SAFE_ACCESS_SIG = 0x57 ;                                 /* unlock step 1 */
                    R8_SAFE_ACCESS_SIG = 0xA8 ;                                 /* unlock step 2 */    
                    R8_SLP_CLK_OFF1 &= ~RB_SLP_CLK_ADC;                
                    R8_SAFE_ACCESS_SIG = 0;                                     /* 保护，禁止读写 */
                    R8_ADC_CTRL_MOD |= RB_ADC_POWER_ON;                         /* ADC功能模块使能 */
                    break;

                case 6:                                                         /* 关闭ADC的PHY */
                    printf("close ADC PHY\n");
                    R8_SAFE_ACCESS_SIG = 0x57 ;                                 /* unlock step 1 */
                    R8_SAFE_ACCESS_SIG = 0xA8 ;                                 /* unlock step 2 */    
                    R8_SLP_CLK_OFF1 |= RB_SLP_CLK_ADC;                
                    R8_SAFE_ACCESS_SIG = 0;                                     /* 保护，禁止读写 */
                    R8_ADC_CTRL_MOD &= ~RB_ADC_POWER_ON;                        /* ADC功能模块禁止 */
                    break;

                case 7:                                                         /* 关闭所有时钟 */
                    printf("close clk\n");
                    R8_SAFE_ACCESS_SIG = 0x57 ;                                 /* unlock step 1 */
                    R8_SAFE_ACCESS_SIG = 0xA8 ;                                 /* unlock step 2 */
                    R8_SLP_CLK_OFF0 |= ( RB_SLP_CLK_UART1 | RB_SLP_CLK_UART0 | 
                                       RB_SLP_CLK_TMR0 | RB_SLP_CLK_TMR1 |
                                       RB_SLP_CLK_TMR2 | RB_SLP_CLK_TMR3 | 
                                       RB_SLP_CLK_SPI0 | RB_SLP_CLK_SPI1 );     /* 关闭时钟控制寄存器0 */
                    R8_SLP_CLK_OFF1 |= ( RB_SLP_CLK_ADC | RB_SLP_CLK_GPIO | 
                                       RB_SLP_CLK_USB | RB_SLP_CLK_ETH );       /* 关闭时钟控制寄存器1 */
                    R8_SAFE_ACCESS_SIG = 0;                                     /* 保护，禁止读写 */
                    Delay_ms( 1000 );
                    R8_SAFE_ACCESS_SIG = 0x57 ;                                 /* unlock step 1 */
                    R8_SAFE_ACCESS_SIG = 0xA8 ;                                 /* unlock step 2 */
                    R8_SLP_CLK_OFF0 &= ~RB_SLP_CLK_UART0;                       /* 1s后开启串口0时钟 */
                    R8_SAFE_ACCESS_SIG = 0;                                     /* 保护，禁止读写 */
                    Delay_us( 10 );
                    printf("\nOK\n");
                   break;    

                case 8:                                                         /* 开启所有时钟 */
                    printf("open clk\n");
                    R8_SAFE_ACCESS_SIG = 0x57 ;                                 /* unlock step 1 */
                    R8_SAFE_ACCESS_SIG = 0xA8 ;                                 /* unlock step 2 */
                    R8_SLP_CLK_OFF0 &= 0x00;                                    /* 开启时钟控制寄存器0 */
                    R8_SLP_CLK_OFF1 &= 0x0f;                                    /* 开启时钟控制寄存器1 */
                    R8_SAFE_ACCESS_SIG = 0;                                     /* 保护，禁止读写 */
                    break;    

                case 9:                                                         /* 关闭USB */
                    printf("close usb\n");
                    USBHost_ModeSet( );    
                    R8_SAFE_ACCESS_SIG = 0x57 ;                                 /* unlock step 1 */
                    R8_SAFE_ACCESS_SIG = 0xA8 ;                                 /* unlock step 2 */                     
//                  USB_DEV_CONTROL |= RB_GOSUSP;                               /* 设备模式PHY挂起 */
                    HC_MISCELLANEOUS |= (1<<6);                                 /* 主机模式收发器挂起 */
                    R8_SLP_CLK_OFF1 |= RB_SLP_CLK_USB;                          /* 关闭USB时钟 */
                    R8_SAFE_ACCESS_SIG = 0;                                     /* 保护，禁止读写 */
                    break;

                 case 10:                                                       /* 启动USB主机模式 */
                    printf("USB Host\n");
                    R8_SAFE_ACCESS_SIG = 0x57 ;                                 /* unlock step 1 */
                    R8_SAFE_ACCESS_SIG = 0xA8 ;                                 /* unlock step 2 */                     
                    R8_SLP_CLK_OFF1 &= ~RB_SLP_CLK_USB;                         /* 开启USB时钟 */
                    HC_MISCELLANEOUS &= ~(1<<6);
                    R8_SAFE_ACCESS_SIG = 0;                                     /* 保护，禁止读写 */
                    USBHost_ModeSet( );    
                    break;

                 default:
                    printf("!!!! input error !!!\n");
                    printf(" 1................PLL进入全局休眠状态,PB[8]低电平唤醒\r\n");
                    printf(" 2................CPU 进入待机状态，PB[8]低电平唤醒\r\n");
                    printf(" 3................以太网模块上电\r\n");
                    printf(" 4................以太网模块下电\r\n");
                    printf(" 5................打开ADC的PHY\r\n");
                    printf(" 6................关闭ADC的PHY\r\n");
                    printf(" 7................关闭所有时钟\r\n");
                    printf(" 8................开启所有时钟\r\n");
                    printf(" 9................关闭USB（时钟和PHY）\r\n");
                    printf(" A................启动USB主机模式\r\n");
                    break;
            } 
        }
        while ( R8_UART0_LSR & RB_LSR_DATA_RDY ) i = R8_UART0_RBR;
    }
    for ( i=0; i!=5; i++ ) {
        LED_OUT_ACT( );
        Delay_ms( 100 );
        LED_OUT_INACT( );
        Delay_ms( 100 );
        if ( R8_UART0_LSR & RB_LSR_DATA_RDY ) break;
    }
}

/*******************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
int main( void ) 
{ 
    LED_OUT_INIT( );
    LED_OUT_ACT( );                                                             /* 开机后LED亮一下以示工作 */
    Delay_ms( 100 );
    LED_OUT_INACT( );
    mInitSTDIO( );                                                              /* 为了让计算机通过串口监控演示过程 */
    LED_OUT_ACT( );
    LED_OUT_INACT( );
    printf( "Start test\n" );
    printf(" 1................PLL进入全局休眠状态,PB[8]低电平唤醒\r\n");
    printf(" 2................CPU 进入待机状态，PB[8]低电平唤醒\r\n");
    printf(" 3................以太网模块上电\r\n");
    printf(" 4................以太网模块下电\r\n");
    printf(" 5................打开ADC的PHY\r\n");
    printf(" 6................关闭ADC的PHY\r\n");
    printf(" 7................关闭所有时钟\r\n");
    printf(" 8................开启所有时钟\r\n");
    printf(" 9................关闭USB（时钟和PHY）\r\n");
    printf(" A................启动usb主机模式\r\n");
    WakeUP_PB( );
    while(1){
        Test_Sleep( );
    }
}

/*********************************** endfile **********************************/
