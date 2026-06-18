/********************************** (C) COPYRIGHT ******************************
* File Name          : EXTest.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : 
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include "SYSFREQ.H"
#include "..\PARA\PARA_TFT\LCD\LCD.H"
#include "EXTEST.H"

/*******************************************************************************
* Function Name  : TEST_GRAPH
* Description    : 纯色填充示例
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TEST_COLOUR( void )
{
#if 0
    UINT16 c;    
    //65536色填充示例
    for( c=0;c<0xFFFF;c++ ){
        LCD_Clear(c);
    }
#endif

    LCD_Clear(BLACK);
    Delay_ms(800);
    LCD_Clear(BLUE);
    Delay_ms(800);
    LCD_Clear(GREEN);
    Delay_ms(800);
    LCD_Clear(RED);
    Delay_ms(800);
    LCD_Clear(GREEN);
    Delay_ms(800);
    LCD_Clear(GRAY);
    Delay_ms(800);
}

/*******************************************************************************
* Function Name  : TEST_GRAPH
* Description    : 演示图形显示
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TEST_GRAPH( void )
{
    LCD_DrawLine(lcddev.width>>1,0,lcddev.width>>1,lcddev.height);
    Delay_ms(500);
    LCD_DrawLine(0,lcddev.height>>1,lcddev.width,lcddev.height>>1);
    Delay_ms(500);
    LCD_DrawRectangle( lcddev.width>>3,lcddev.height>>3,(lcddev.width>>1)-
        (lcddev.width>>3),(lcddev.height>>1)-(lcddev.height>>3) );
    Delay_ms(500);
    Draw_Circle((lcddev.width>>1)+(lcddev.width>>2),lcddev.height>>2,50);
    Delay_ms(500);
    LCD_Fill(0,(lcddev.height>>1)+1,(lcddev.width>>1)-1,lcddev.height,GREEN);
    Delay_ms(500);
    LCD_Color_Fill((lcddev.width>>1)+1,(lcddev.height>>1)+1,lcddev.width,lcddev.height);
    Delay_ms(500);
}

/*******************************************************************************
* Function Name  : EX_TEST
* Description    : 扩展的演示
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void  EX_TEST( void )
{
    TEST_COLOUR( );
    TEST_GRAPH( );
}

/*********************************** endfile **********************************/
