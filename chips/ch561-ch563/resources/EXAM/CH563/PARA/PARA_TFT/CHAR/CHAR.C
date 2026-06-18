/********************************** (C) COPYRIGHT ******************************
* File Name          : CHAR.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : 字符显示
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "..\PARA\PARA_TFT\LCD\LCD.H"
#include "CHAR.H"

/*******************************************************************************
* Function Name  : show_pic
* Description    : 显示字符串
* Input          : x,y:起点坐标
*                  width,height:区域大小  
*                  size:字体大小
*                  p:字符串起始地址    
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_ShowString(UINT16 x,UINT16 y,UINT16 width,UINT16 height,UINT8 size,UINT8 *p)
{         
    UINT8 x0=x;
    width+=x;
    height+=y;
    while((*p<='~')&&(*p>=' ')){                                                /* 判断是不是非法字符 */
        if(x>=width){x=x0;y+=size;}
        if(y>=height) break;                                                    /* 退出 */
        LCD_ShowChar(x,y,*p,size,0);
        x+=size/2;
        p++;
    }  
}

/*******************************************************************************
* Function Name  : show_pic
* Description    : 字符串测试程序
* Input          : None    
* Output         : None
* Return         : None
*******************************************************************************/
void TEST_CHAR( void )
{
    UINT8 lcd_id[12];                                                           /* 存放LCD ID字符串 */
    UINT16 c; 

    POINT_COLOR=BLACK;
    BACK_COLOR=WHITE;
    c = 40;
    LCD_ShowString(c,50,200,16,16,"3.5' TFT Test");
    sprintf((char *)lcd_id,"LCD ID:0x%x\r\n",lcddev.id);  
    LCD_ShowString(c,70,200,16,16,lcd_id);                                      /* 显示LCD ID */    
    LCD_ShowString(c,90,200,16,16,"2013/9/2");
    LCD_ShowString(c,110,200,16,16,"CH563 demo...");    
    Delay_ms(500);
    LCD_ShowString(c,150,200,16,16,"Mail: ");
    Delay_ms(500);
    c += 8*6;
    LCD_ShowString(c,150,200,16,16,"tech@wch.cn");
    Delay_ms(500);
    c = 40;
    LCD_ShowString(c,170,200,16,16,"Web : ");
    Delay_ms(500);
    c += 8*6;
    LCD_ShowString(c,170,200,16,16,"http://wch.cn");
    Delay_ms(500);
    c = 40;
    LCD_ShowString(c,190,200,16,16,"Tel : ");
    c += 8*6;
    Delay_ms(500);
    LCD_ShowString(c,190,200,16,16,"+86-");
    c += 8*4;
    Delay_ms(300);
    LCD_ShowString(c,190,200,16,16,"025-");
    c += 8*4;
    Delay_ms(300);
    LCD_ShowString(c,190,200,16,16,"5");
    c += 8;
    Delay_ms(300);
    LCD_ShowString(c,190,200,16,16,"2");
    c += 8;
    Delay_ms(300);
    LCD_ShowString(c,190,200,16,16,"6");
    c += 8;
    Delay_ms(300);
    LCD_ShowString(c,190,200,16,16,"3");
    c += 8;
    Delay_ms(300);
    LCD_ShowString(c,190,200,16,16,"8");
    c += 8;
    Delay_ms(300);
    LCD_ShowString(c,190,200,16,16,"370");
    c = 40;
    Delay_ms(500);
    LCD_ShowString(c,210,200,16,16,"Fax : ");
    c += 8*6;
    Delay_ms(500);
    LCD_ShowString(c,210,200,16,16,"+86-025-84730778"); 
}

/*********************************** endfile **********************************/
