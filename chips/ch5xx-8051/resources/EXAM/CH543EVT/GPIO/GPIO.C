/********************************** (C) COPYRIGHT *******************************
* File Name          : GPIO.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 GPIO相关函数
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


/******************************************************************************/
/* 头文件包含 */
#include ".\GPIO\GPIO.H"
#pragma  NOAREGS

/*******************************************************************************
* Function Name  : GPIO_Init
* Description    : GPIO端口初始化函数
* Input          : PORTx:1、3
*                  PINx:位域,每个位对应该Port的一个引脚
*                  MODEx:
*                        0: 高阻输入模式,引脚没有上拉电阻
*                        1: 推挽输出模式,具有对称驱动能力,可以输出或者吸收较大电流
*                        2: 开漏输出,支持高阻输入,引脚没有上拉电阻
*                        3: 准双向模式(标准 8051),开漏输出,支持输入,引脚有上拉电阻(默认模式)
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_Init( UINT8 PORTx, UINT8 PINx, UINT8 MODEx )
{
    UINT8 Px_DIR_PU,Px_MOD_OC;
    
    switch( PORTx )                                                               /* 读出初始值 */
    {
        case PORT1:
            Px_MOD_OC = P1_MOD_OC;
            Px_DIR_PU = P1_DIR_PU;
            break;
        case PORT3:
            Px_MOD_OC = P3_MOD_OC;
            Px_DIR_PU = P3_DIR_PU;
            break;
        default :
            break;
    }
    switch( MODEx )
    {
        /* 高阻输入模式，引脚没有上拉电阻 */
        case MODE0:
            Px_MOD_OC &= ~PINx;
            Px_DIR_PU &= ~PINx;
            break;
        /* 推挽输出模式，具有对称驱动能力，可以输出或者吸收较大电流 */
        case MODE1:
            Px_MOD_OC &= ~PINx;
            Px_DIR_PU |= PINx;
            break;
        /* 开漏输出，支持高阻输入，引脚没有上拉电阻 */
        case MODE2:
            Px_MOD_OC |= PINx;
            Px_DIR_PU &= ~PINx;
            break;
        /* 准双向模式(标准 8051)，开漏输出，支持输入，引脚有上拉电阻 */
        case MODE3:
            Px_MOD_OC |= PINx;
            Px_DIR_PU |= PINx;
            break;
        default :
            break;
    }
    switch( PORTx )                                                               /* 回写 */
    {
        case PORT1:
            P1_MOD_OC = Px_MOD_OC;
            P1_DIR_PU = Px_DIR_PU;
            break;
        case PORT3:
            P3_MOD_OC = Px_MOD_OC;
            P3_DIR_PU = Px_DIR_PU;
            break;
        default :
            break;
    }
}

/*******************************************************************************
* Function Name  : GPIO_INT_Init
* Description    : 可设置 P17_L、P15_L、P14_L、P13_L、RST_H、P31_L、RXD0_L 扩展引
*                  脚的外部中断,同时还包含兼容C51的 INT1_L、INT0_L 的外部中断触发
*                  (RXD0_L具体是哪个引脚取决于引脚是否映射)
* Input          : IntSrc:共9个外部中断,按位域表示,具体定义见GPIO.H
*                  Mode：0：电平模式   1：边沿模式 (注意扩展引脚的中断模式是统一配置的)
*                  NewState：0：关闭对应外部中断使能  1：开启对应外部中断
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_INT_Init( UINT16 IntSrc,UINT8 Mode,UINT8 NewState )
{
    /* 中断触发模式设置 */
    if( Mode == INT_EDGE )                                                      /* 边沿触发模式 */
    {
        if( IntSrc & 0x7F )                                                     /* 存在扩展中断 */
        {
            SAFE_MOD = 0x55;
            SAFE_MOD = 0xAA;
            GPIO_IE |= bIE_IO_EDGE;
            SAFE_MOD = 0x00;
        }
        if(IntSrc & INT_INT0_L)                                                 /* 存在外部中断0 */
        {
            IT0 = 1;
        }
        if(IntSrc & INT_INT1_L)                                                 /* 存在外部中断1 */
        {
            IT1 = 1;
        }
    }
    else                                                                        /* 电平触发模式 */
    {
        if( IntSrc & 0x7F )                                                     /* 存在扩展中断 */
        {
            SAFE_MOD = 0x55;
            SAFE_MOD = 0xAA;
            GPIO_IE &= ~bIE_IO_EDGE;
            SAFE_MOD = 0x00;
        }
        if( IntSrc & INT_INT0_L )                                               /* 存在外部中断0 */
        {
            IT0 = 1;
        }
        if( IntSrc & INT_INT1_L )                                               /* 存在外部中断1 */
        {
            IT1 = 1;
        }
    }
    /* 中断使能状态 */
    if( NewState == Enable )                                                    /* 开启外部中断 */
    {
        SAFE_MOD = 0x55;
        SAFE_MOD = 0xAA;
        GPIO_IE |= ( (UINT8)IntSrc & 0x7F );
        SAFE_MOD = 0x00;
        if( IntSrc & INT_INT0_L )                                               /* 存在外部中断0 */
        {
            EX0 = 1;
        }
        if( IntSrc & INT_INT1_L )                                               /* 存在外部中断1 */
        {
            EX1 = 1;
        }
        if( GPIO_IE & 0x7F )
        {
            IE_GPIO = 1;                                                        /* 开启扩展GPIO中断 */
        }
        EA = 1;                                                                 /* 开启总中断 */
    }
    else                                                                        /* 关闭对应外部中断 */
    {
        SAFE_MOD = 0x55;
        SAFE_MOD = 0xAA;
        GPIO_IE &= ~( (UINT8)IntSrc & 0x7F );
        SAFE_MOD = 0x00;
        if( IntSrc & INT_INT0_L )                                               /* 存在外部中断0 */
        {
            EX0 = 0;
        }
        if(IntSrc & INT_INT1_L)                                                 /* 存在外部中断1 */
        {
            EX1 = 0;
        }
        if( ( GPIO_IE & 0x7F ) == 0 )
        {
            IE_GPIO = 0;                                                        /* 关闭扩展GPIO中断 */
        }
    }
}

/*******************************************************************************
* Function Name  : GPIO_ISR
* Description    : P17、P15、P14、P13、RST、P31、RXD0 引脚外部中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_EXT_ISR( void ) interrupt INT_NO_GPIO
{
    mDelaymS( 10 );
    printf("GPIO int\n");
}

/*******************************************************************************
* Function Name  : GPIO_STD0_ISR
* Description    : INT0(P32) 引脚外部中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_STD0_ISR( void ) interrupt INT_NO_INT0
{
    mDelaymS( 10 );
    printf("P32 Falling\n");

}

/*******************************************************************************
* Function Name  : GPIO_STD1_ISR
* Description    : INT1(P33) 引脚外部中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_STD1_ISR( void ) interrupt INT_NO_INT1
{
    mDelaymS( 10 );
    printf("P33 Falling\n");

}