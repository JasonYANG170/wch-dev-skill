/********************************** (C) COPYRIGHT ******************************
* File Name          : PICTURE.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : 图片显示
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include "..\PARA\PARA_TFT\LCD\LCD.H"
#include "..\PARA\PARA_TFT\CH56X\HAL.H"

/*******************************************************************************
* Function Name  : show_pic
* Description    : 16位垂直扫描，高位在前
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#if 0
void show_pic(const unsigned char *p)
{
    int i,j;
    unsigned char picH,picL;
    LCD_WriteReg(0x36,0x02);
    LCD_SetWindows(0,0,240-1,320-1);
    for(i=0;i<120;i++){
        for(j=0;j<320;j++){
             picH=*p++;
            picL=*p++;        
            POINT_COLOR=picH<<8|picL;
            
            LCD_WR_DATA( POINT_COLOR );
        }    
    }
    LCD_WriteReg(0x36,0x31);
}
#else

void show_pic(unsigned short *p)
{
    UINT32 i,j;
    UINT16 *head;

    head = p;

    /* 第1象限 */
    LCD_Display_Dir( 0 );
	printf("%8d %8d\n",lcddev.width,lcddev.height);
//    LCD_SetWindows(0,0,160-1,240-1);
    LCD_SetWindows(0,0,((lcddev.width>>1)-1),((lcddev.height>>1)-1));
    for(i=0;i<240;i++){
        for(j=0;j<160;j++){
            LCD_WR_DATA( *p );
            p++;
        }    
    }
    /* 第4象限 */
    LCD_WriteReg(0x36,0xc2);
//  LCD_SetWindows(160,240,320-1,480-1);
    LCD_SetWindows((lcddev.width>>1),(lcddev.height>>1),(lcddev.width-1),lcddev.height-1);
	p = head;
    for(i=0;i<240;i++){
        for(j=0;j<160;j++){
            LCD_WR_DATA( *p );
            p++;
        }    
    }
    /* 第3象限 */
   LCD_WriteReg(0x36,0x82);
//   LCD_SetWindows(0,240,160-1,480-1);
    LCD_SetWindows(0,(lcddev.height>>1),((lcddev.width>>1)-1),(lcddev.height-1));
	p = head;
    for(i=0;i<240;i++){
        for(j=0;j<160;j++){
            LCD_WR_DATA( *p );
            p++;
        }    
    }
    /* 第2象限 */
    LCD_WriteReg(0x36,0x42);
//    LCD_SetWindows(160,0,320-1,240-1);
    LCD_SetWindows((lcddev.width>>1),0,(lcddev.width-1),((lcddev.height>>1)-1));
	p = head;
    for(i=0;i<240;i++){
        for(j=0;j<160;j++){
            LCD_WR_DATA( *p );
            p++;
        }    
    }
    LCD_Display_Dir( lcddev.dir );
} 
#endif

/*********************************** endfile **********************************/
