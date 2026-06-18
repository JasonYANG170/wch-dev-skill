/********************************** (C) COPYRIGHT *******************************
* File Name          : Type_C.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/27
* Description        : CH549 Type-C使用
                       主模式 设备正反插检测、充电电流通知从设备
                       从模式 检测主机供电能力
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "Type_C.H"
#pragma  NOAREGS
/********************************************************************************
DFP (Downstream Facing Port) Host端
UFP (Upstream Facing Port)   Dev端
在DFP的CC pin会有上拉电阻Rp,在UFP会有下拉电阻Rd
在DFP和UFP未连接前,DFP的VBUS是没有输出的
CC PIN是用来侦测正反插，从DPF来看，当CC1接到下拉就是正插;如果CC2接到下拉就是反插；
     侦测完正反插之后，就会输出相应的USB信号

DPF在不同的模式下，定义了在CC PIN要供多大的电流或是要用多大的Rp值
********************************************************************************/
#ifdef TYPE_C_DFP
/*******************************************************************************
* Function Name  : TypeC_DFP_Init(UINT8 Power)
* Description    : Type-C UPF检测初始化
* Input          : UINT8 Power
                   0 禁止UCC1&2上拉
                   1 默认电流
                   2 1.5A
                   3 3.0A
* Output         : None
* Return         : NONE
*******************************************************************************/
void TypeC_DFP_Init( UINT8 Power )
{
    P1_MOD_OC &= ~(bUCC2|bUCC1);
    P1_DIR_PU &= ~(bUCC2|bUCC1);                                                     //UCC1 UCC2 设置浮空输入
    VCTL = 1;
    P1_MOD_OC &= ~(bVCTL);                                                           //bVCTL 推挽输出，高电平禁止输出VCTL
    P1_DIR_PU |= (bVCTL);
    if(Power == 0)
    {
        DFP_DisableRpUCC1();                                                         //UCC1禁止
        DFP_DisableRpUCC2();                                                         //UCC2禁止
    }
    else if(Power == 1)
    {
        DFP_DefaultPowerUCC1();                                                      //输出能力默认
        DFP_DefaultPowerUCC2();
    }
    else if(Power == 2)
    {
        DFP_1_5APowerUCC1();                                                         //输出能力1.5A
        DFP_1_5APowerUCC2();
    }
    else if(Power == 3)
    {
        DFP_3_0APowerUCC1();                                                         //输出能力3.0A
        DFP_3_0APowerUCC2();
    }
    ADC_CFG |= bADC_EN | bADC_AIN_EN;                                                //开启ADC模块电源,开启外部通道
    ADC_CFG = ADC_CFG & ~(bADC_CLK0 | bADC_CLK1);                                    //选择ADC参考时钟 750K
    ADC_CTRL = bADC_IF;                                                              //清除ADC转换完成标志，写1清零
    mDelayuS(2);                                                                     //等待ADC电源稳定
}
/*******************************************************************************
* Function Name  : TypeC_DFP_Insert(void)
* Description    : Type-C DPF检测UFP正插，反插以及未插入和已插入
* Input          : NONE
* Output         : None
* Return         : 0   未连接
                   1   正向连接
                   2   反向连接
                   3   连接，无法判定正反
*******************************************************************************/
UINT8 TypeC_DFP_Insert( void )
{
    UINT16 UCC1_Value,UCC2_Value;
    ADC_CHAN = 4;                                                                   //CC1引脚连接至AIN4(P14)
    ADC_CTRL = bADC_START;                                                          //启动采样
    while((ADC_CTRL&bADC_IF) == 0)
    {
        ;    //查询等待标志置位
    }
    ADC_CTRL = bADC_IF;                                                             //清标志
    UCC1_Value = ADC_DAT&0xFFF;
    ADC_CHAN = 5;                                                                   //CC2引脚连接至AIN5(P15)
    ADC_CTRL = bADC_START;                                                          //启动采样
    while((ADC_CTRL&bADC_IF) == 0)
    {
        ;    //查询等待标志置位
    }
    ADC_CTRL = bADC_IF;                                                             //清标志
    UCC2_Value = (ADC_DAT&0xFFF);
    printf("UCC1:%d UCC2:%d\n",(UINT16)UCC1_Value,(UINT16)UCC2_Value);
    if( (UCC1_Value<=UCC_Connect_Vlaue) && (UCC2_Value<=UCC_Connect_Vlaue) )        //双向连接
    {
        return UCC_CONNECT;
    }
    else if( (UCC1_Value<=UCC_Connect_Vlaue) && (UCC2_Value>=UCC_Connect_Vlaue) )   //正向连接
    {
        return UCC1_CONNECT;
    }
    else if( (UCC1_Value>=UCC_Connect_Vlaue) && (UCC2_Value<=UCC_Connect_Vlaue) )   //反向连接
    {
        return UCC2_CONNECT;
    }
    else if( (UCC1_Value>=UCC_Connect_Vlaue) && (UCC2_Value>=UCC_Connect_Vlaue) )   //未连接
    {
        return UCC_DISCONNECT;
    }
    return UCC_DISCONNECT;
}
#endif
/********************************************************************************
UPF,需要检测CC管脚的电压值来获取DFP的电流输出能力
          CC电压Min      CC电压Max
默认电流    0.25V         0.61V
1.5A        0.70V         1.16V
3.0A        1.31V         2.04V
********************************************************************************/
#ifdef TYPE_C_UFP
/*******************************************************************************
* Function Name  : TypeC_UPF_PDInit()
* Description    : Type-C UPF初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TypeC_UPF_PDInit( void )
{
    P1_MOD_OC &= ~(bUCC2|bUCC1);                                                    //UCC1 UCC2 设置浮空输入
    P1_DIR_PU &= ~(bUCC2|bUCC1);
    UPF_CC1RdCfg(1);                                                                //开启UCC1下拉电阻5.1K
    ADC_CFG |= bADC_EN | bADC_AIN_EN;                                               //开启ADC模块电源,开启外部通道
    ADC_CFG = ADC_CFG & ~(bADC_CLK0 | bADC_CLK1);                                   //选择ADC参考时钟 750Khz
    ADC_CTRL = bADC_IF;                                                             //清除ADC转换完成标志，写1清零
    mDelayuS(2);                                                                    //等待ADC电源稳定
}
/*******************************************************************************
* Function Name  : TypeC_UPF_PDCheck()
* Description    : Type-C UPF检测DPF供电能力
* Input          : None
* Output         : None
* Return         : UINT8
                   1 默认电流
                   2 1.5A
                   3 3.0A
                   0xff 悬空
*******************************************************************************/
UINT8 TypeC_UPF_PDCheck()
{
    UINT16 UCC1_Value;
    ADC_CHAN = 4;                                                                   //CC1引脚连接至AIN4(P14)
    ADC_CTRL = bADC_START;                                                          //启动采样
    while((ADC_CTRL&bADC_IF) == 0)
    {
        ;    //查询等待标志置位
    }
    ADC_CTRL = bADC_IF;                                                             //清标志
    UCC1_Value = ADC_DAT&0xFFF;
    printf("UCC1=%d\n",(UINT16)UCC1_Value);
    if((UCC1_Value >= Power3_0AMin)&&(UCC1_Value <= Power3_0AMax))
    {
        return UPF_PD_3A;                                                           //3.0A供电能力
    }
    else if((UCC1_Value >= Power1_5AMin)&&(UCC1_Value <= Power1_5AMax))
    {
        return UPF_PD_1A5;                                                          //1.5A供电能力
    }
    else if((UCC1_Value >= DufaultPowerMin)&&(UCC1_Value <= DufaultPowerMax))
    {
        return UPF_PD_Normal;                                                       //默认供电能力
    }
    return UPD_PD_DISCONNECT;
}
#endif
