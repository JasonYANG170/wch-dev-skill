/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 11通道触摸按键演示
*                      CH0~CH2,分别对应P10 P11 P32
*                      CH4~CH11,分别对应P14~P17 P36 P37 P12 P13
                       注意包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/



/*******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"
#include ".\TouchKey\TouchKey.H"
#pragma  NOAREGS

/*******************************************************************************/
/* 变量定义 */
UINT16 PowerValue[ 16 ];                                             			/* 保存触摸按键上电未触摸值 */
volatile UINT16 Press_Flag = 0;                                                	/* 按下标志位 */
UINT8C CPW_Table[ 16 ] = { 30, 30, 30, 30, 30, 30, 30, 30,                      /* 与板间电容有关的参数,分别对应每个按键 */
						   30, 30, 30, 30, 30, 30, 30, 30, };
						 
/********************************************************************************
* Function Name  : ABS
* Description    : 求两个数差值的绝对值
* Input          : a,b
* Output         : None
* Return         : 差值绝对值
********************************************************************************/
UINT16 ABS( UINT16 a, UINT16 b )
{
    if( a > b )
    {
        return ( a - b );
    }
    else
    {
        return ( b - a );
    }
}

/********************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
********************************************************************************/
void main( void )
{
    UINT8 i = 100;
    UINT8 ch;
    UINT16 value;
    UINT16 err;																	/* 触摸模拟变化差值 */
    
    memset( KeyBuf, 0, sizeof( KeyBuf ) );	
    CfgFsys( );                                                                	/* CH543时钟选择配置 */
    mDelaymS( 20 );																/* 配置时钟后,建议延时稳定时钟 */
    mInitSTDIO( );                                                             	/* 串口0初始化 */
    
	printf("TouchKey demo start ...\n");
    TouchKey_Init( );
    Press_Flag = 0;                                                            	/* 无按键按下 */
    
    mDelaymS( 100 );	
        
    /* 获取按键初值 */
    for( ch = 4; ch != 8; ch++ )
    {
        PowerValue[ ch ] = Default_TouchKey( ch, CPW_Table[ ch ] );
        printf("Y%d",PowerValue[ ch ]);
    }
    printf("\n");
	
    while( 1 )
    {
        while( i )																/* 填充初始值,不可省 */
        {
            for( ch = 4; ch != 8; ch++ )
            {
                TouchKeySelect( ch, CPW_Table[ ch ] );
                i--;
            }
        }
		
		/* 按键检测 */
        for( ch = 4; ch != 8; ch++ )
        {
            value = TouchKeySelect( ch, CPW_Table[ ch ]  );
            err = ABS( PowerValue[ ch ], value );
            
			if( err > DOWM_THRESHOLD_VALUE )									/* 差值大于阈值，认为按下 */
            {
                if( ( Press_Flag & ( 1 << ch ) ) == 0 )                         /* 说明是第一次按下 */
                {
                    printf("ch %d pressed,value:%d\n",(UINT16)ch, value);
                }
                Press_Flag |= ( 1 << ch );
            }
            else if( err < UP_THRESHOLD_VALUE )                                	/* 说明抬起或者未按下 */
            {
                if( Press_Flag & ( 1 << ch ) )									/* 刚抬起 */
                {
                    Press_Flag &= ~( 1 << ch );
                    printf("ch %d up,value:%d\n",(UINT16)ch, value);
                }
            }
        }
    }
}