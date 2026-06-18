/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2019/07/22
* Description        : CH549 16通道触摸按键演示
*                      CH0~CH15 分别对应引脚 P1.0~P1.7  P0.0~P0.7
                       注意包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\TouchKey\TouchKey.H"
#pragma  NOAREGS
UINT16 PowerValue[16];                                                         //保存触摸按键上电未触摸值
volatile UINT16 Press_Flag = 0;                                                //按下标志位
UINT8C CPW_Table[16] = { 30,30,30,30, 30,30,30,30,                             //与板间电容有关的参数，分别对应每个按键
                         30,30,30,30, 30,30,30,30,
                       };

/*******************************************************************************
* Function Name  : ABS
* Description    : 求两个数差值的绝对值
* Input          : a,b
* Output         : None
* Return         : 差值绝对值
*******************************************************************************/
UINT16 ABS(UINT16 a,UINT16 b)
{
    if(a>b)
    {
        return (a-b);
    }
    else
    {
        return (b-a);
    }
}
/*******************************************************************************
* Function Name  : LED_Port_Init
* Description    : LED引脚初始化,推挽输出
*                  P22~P25
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LED_Port_Init(void)
{
    P2 |= (0xF<<2);                                                          //默认熄灭
    P2_MOD_OC &= ~(0xF<<2);
    P2_DIR_PU |= (0xF<<2);
}
/*******************************************************************************
* Function Name  : LED_Control
* Description    : 点灯控制
* Input          : LEDx: 0~3 分别对应P22~P25四个LED灯（低驱）
*                  status: 0:灭  1：亮
* Output         : None
* Return         : None
*******************************************************************************/
void LED_Control(UINT8 LEDx,UINT8 status)
{
    if(LEDx>3)
    {
        return;
    }
    if(status)                                                                //点亮
    {
        P2 &= ~(1<<(2+LEDx));
    }
    else                                                                      //熄灭
    {
        P2 |= (1<<(2+LEDx));
    }
}
//主函数
void main()
{
	UINT8 i = 100;
    UINT8 ch;
    UINT16 value;
    UINT16 err;                                                                //触摸模拟变化差值
    memset(KeyBuf,0,sizeof(KeyBuf));	
    CfgFsys( );                                                                //CH549时钟选择配置
    mDelaymS(20);
    mInitSTDIO( );                                                             //串口0初始化
    printf("TouchKey demo start ...\n");
    LED_Port_Init();
    TouchKey_Init();
    Press_Flag = 0;                                                            //无按键按下
    /* 获取按键初值 */
    for(ch = 8; ch!=12; ch++)
    {
        PowerValue[ch] = Default_TouchKey( ch,CPW_Table[ch] );
        printf(" Y%d ",PowerValue[ch] );
    }
    printf("\n");
    while(1)
    {
		while(i)                                                             //填充初始值，不可省
		{
			 for(ch = 8; ch!=12; ch++)
			{
				TouchKeySelect( ch,CPW_Table[ch] );
				i--;
			}
		}
        /* 按键检测 */
        for(ch = 8; ch!=12; ch++)
        {
            value = TouchKeySelect( ch,CPW_Table[ch] );
            err = ABS(PowerValue[ch],value);
            if( err > DOWM_THRESHOLD_VALUE )                                   //差值大于阈值，认为按下
            {
                if((Press_Flag & (1<<ch)) == 0)                                //说明是第一次按下
                {
                    printf("ch %d pressed,value:%d\n",(UINT16)ch, value);
                    /* 点灯处理 */
                    LED_Control(ch-8,1);
                }
                Press_Flag |= (1<<ch);
            }
            else if( err < UP_THRESHOLD_VALUE )                                //说明抬起或者未按下
            {
                if(Press_Flag & (1<<ch))                                       //刚抬起
                {
                    Press_Flag &= ~(1<<ch);
                    printf("ch %d up,value:%d\n",(UINT16)ch, value);
                    /* 灭灯处理 */
                    LED_Control(ch-8,0);
                }
            }
        }
    }
}