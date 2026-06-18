/********************************** (C) COPYRIGHT ******************************
* File Name          : CHINESE.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : 中文显示
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include "SYSFREQ.H"
#include "CHINESE.H"
#include "CHFONT.H"
#include "..\PARA\PARA_TFT\LCD\LCD.H"       
            
/*******************************************************************************
* Function Name  : LCD_Draw_Point
* Description    : 在LCD上的（x，y）处画点
* Input          : x,y:坐标
*                  color:点的颜色
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Draw_Point(UINT16 x,UINT16 y,UINT16 color)
{
    UINT16 temp;

    temp=POINT_COLOR;
    POINT_COLOR=color;
    LCD_DrawPoint(x,y);
    POINT_COLOR=temp;
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font16
* Description    : 在指定位置 显示1个16*16的汉字
* Input          : x,y:汉字显示的位置
*                  s  :汉字字符串首地址
*                  color:汉字的颜色
* Output         : None
* Return         : None
*******************************************************************************/
void Test_Show_CH_Font16(UINT16 x,UINT16 y,UINT8 *s,UINT16 color)
{                   
    UINT8 temp,t,t1,k;
    UINT16 y0=y;    
    UINT16 HZnum;
    HZnum=sizeof(tfont16)/sizeof(typFNT_GB16);    
    while(*s){ 
        for (k=0;k<HZnum;k++){ 
            if ((tfont16[k].Index[0]==*(s))&&(tfont16[k].Index[1]==*(s+1))){
                for(t=0;t<32;t++){                                              /* 每个16*16的汉字点阵 有32个字节 */
                    temp=tfont16[k].Msk[t];                    
                    for(t1=0;t1<8;t1++){
                        if(temp&0x80)LCD_Draw_Point(x,y,color);                 /* 画实心点 */
                        else LCD_Draw_Point(x,y,BACK_COLOR);                    /* 画空白点（使用背景色） */
                        temp<<=1;
                        y++;
                        if((y-y0)==16){
                            y=y0;
                            x++;
                            break;
                        }
                    }       
                }   
            }
        }
        s+=2;
    }            
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font24
* Description    : 在指定位置 显示1个24*24的汉字
* Input          : x,y:汉字显示的位置
*                  s  :汉字字符串首地址
*                  color:汉字的颜色
* Output         : None
* Return         : None
*******************************************************************************/
void Test_Show_CH_Font24(UINT16 x,UINT16 y,UINT8 *s,UINT16 color)
{                   
    UINT8 temp,t,t1,k;
    UINT16 y0=y;    
    UINT16 HZnum;
    HZnum=sizeof(tfont24)/sizeof(typFNT_GB24);
    while(*s){ 
        for (k=0;k<HZnum;k++){ 
            if ((tfont24[k].Index[0]==*(s))&&(tfont24[k].Index[1]==*(s+1))){
                for(t=0;t<72;t++){                                              /* 每个24*24的汉字点阵 有72个字节 */
                    temp=tfont24[k].Msk[t];                                     /* 前24个字节 */
                    for(t1=0;t1<8;t1++){
                        if(temp&0x80)LCD_Draw_Point(x,y,color);                 /* 画实心点 */
                        else LCD_Draw_Point(x,y,BACK_COLOR);                    /* 画空白点（使用背景色） */
                        temp<<=1;
                        y++;
                        if((y-y0)==24){
                            y=y0;
                            x++;
                            break;
                        }
                    }       
                } 
            }
        }    
        s+=2;
    }
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font32
* Description    : 在指定位置 显示1个32*32的汉字
* Input          : x,y:汉字显示的位置
*                  s  :汉字字符串首地址
*                  color:汉字的颜色
* Output         : None
* Return         : None
*******************************************************************************/
void Test_Show_CH_Font32(UINT16 x,UINT16 y,UINT8 *s,UINT16 color)
{                   
    UINT8 temp,t,t1,k;
    UINT16 y0=y;    
    UINT16 HZnum;
    HZnum=sizeof(tfont32)/sizeof(typFNT_GB32);
    while(*s){ 
        for (k=0;k<HZnum;k++){ 
            if ((tfont32[k].Index[0]==*(s))&&(tfont32[k].Index[1]==*(s+1))){
                for(t=0;t<128;t++){                                             /* 每个32*32的汉字点阵 有128个字节 */
                    temp=tfont32[k].Msk[t];           
                    for(t1=0;t1<8;t1++){
                        if(temp&0x80)LCD_Draw_Point(x,y,color);                 /* 画实心点 */
                        else LCD_Draw_Point(x,y,BACK_COLOR);                    /* 画空白点（使用背景色） */
                        temp<<=1;
                        y++;
                        if((y-y0)==32){
                            y=y0;
                            x++;
                            break;
                        }
                    }       
                } 
            }
        }    
        s+=2;
    }
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font48
* Description    : 在指定位置 显示1个48*48的汉字
* Input          : x,y:汉字显示的位置
*                  s  :汉字字符串首地址
*                  color:汉字的颜色
* Output         : None
* Return         : None
*******************************************************************************/
void Test_Show_CH_Font48(UINT16 x,UINT16 y,UINT8 *s,UINT16 color)
{                   
    UINT8 temp,t,t1,k;
    UINT16 y0=y;    
    UINT16 HZnum;
    HZnum=sizeof(tfont48)/sizeof(typFNT_GB48);
    while(*s){ 
        for (k=0;k<HZnum;k++){ 
            if ((tfont48[k].Index[0]==*(s))&&(tfont48[k].Index[1]==*(s+1))){
                for(t=0;t<228;t++){                                             /* 每个48*48的汉字点阵 有228个字节 */
                    temp=tfont48[k].Msk[t];           
                    for(t1=0;t1<8;t1++){
                        if(temp&0x80)LCD_Draw_Point(x,y,color);                 /* 画实心点 */
                        else LCD_Draw_Point(x,y,BACK_COLOR);                    /* 画空白点（使用背景色） */
                        temp<<=1;
                        y++;
                        if((y-y0)==48){
                            y=y0;
                            x++;
                            break;
                        }
                    }       
                } 
            }
        }    
        s+=2;
    }
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font64
* Description    : 在指定位置 显示1个64*64的汉字
* Input          : x,y:汉字显示的位置
*                  s  :汉字字符串首地址
*                  color:汉字的颜色
* Output         : None
* Return         : None
*******************************************************************************/
void Test_Show_CH_Font64(UINT16 x,UINT16 y,UINT8 *s,UINT16 color)
{                   
    UINT8 temp,t,t1,k;
    UINT16 y0=y;    
    UINT16 HZnum;

    HZnum=sizeof(tfont64)/sizeof(typFNT_GB64);
    while(*s){ 
        for (k=0;k<HZnum;k++){ 
            if ((tfont64[k].Index[0]==*(s))&&(tfont64[k].Index[1]==*(s+1))){
                for(t=0;t<512;t++){                                             /* 每个64*64的汉字点阵 有512个字节 */
                    temp=tfont64[k].Msk[t];           
                    for(t1=0;t1<8;t1++){
                        if(temp&0x80)LCD_Draw_Point(x,y,color);                 /* 画实心点 */
                        else LCD_Draw_Point(x,y,BACK_COLOR);                    /* 画空白点（使用背景色）*/
                        temp<<=1;
                        y++;
                        if((y-y0)==64){
                             y=y0;
                            x++;
                            break;
                        }
                    }       
                } 
            }
        }    
        s+=2;
    }
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font_EX
* Description    : 在指定位置 显示1个32*32的汉字
* Input          : x,y:汉字显示的位置
*                  s  :汉字字符串首地址
*                  color:汉字的颜色
* Output         : None
* Return         : None
*******************************************************************************/
void Test_Show_CH_Font_EX(UINT16 x,UINT16 y,UINT8 num,UINT16 color)
{                   
    UINT8 temp,t,t1;
    UINT16 y0=y;    

    for(t=0;t<128;t++){                                                        /* 每个32*32的汉字点阵 有128个字节 */
        temp=tfont_ex[num].Msk[t];           
        for(t1=0;t1<8;t1++){
            if(temp&0x80)LCD_Draw_Point(x,y,color);                            /* 画实心点 */
            else LCD_Draw_Point(x,y,BACK_COLOR);                               /* 画空白点（使用背景色）*/
            temp<<=1;
            y++;
            if((y-y0)==32){
                y=y0;
                x++;
                break;
            }
        }    
    }
}

/*******************************************************************************
* Function Name  : Test_Show_CH_Font64
* Description    : 测试汉字显示函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TEST_FONT(void)
{
    BACK_COLOR=WHITE;
    POINT_COLOR = BLUE; 
    LCD_ShowString(50,20,200,16,16,"WCH-CH563...");    
    POINT_COLOR = BLACK; 
    LCD_ShowString(50,40,200,16,16,"WCH-CH563...");    
    Delay_ms(500);
    Test_Show_CH_Font24(50,70,"汉字测试",BLUE);
    Test_Show_CH_Font24(50,100,"汉字测试",BLACK);
    Delay_ms(500);
    Test_Show_CH_Font32(10,140,"操作液晶屏演示",BLUE);
    Test_Show_CH_Font32(10,180,"操作液晶屏演示",BLACK);
    Delay_ms(500);
    Test_Show_CH_Font_EX(260,30,0,RED);
    Delay_ms(500);
    Test_Show_CH_Font_EX(260,70,1,GREEN);
    Delay_ms(50);
    Test_Show_CH_Font_EX(260,110,2,BLACK);
    Delay_ms(50);
    Test_Show_CH_Font_EX(260,150,3,GRAY);
    Delay_ms(50);
    Test_Show_CH_Font_EX(260,190,4,GRAY75);
    Delay_ms(50);
    Test_Show_CH_Font_EX(260,230,5,GRAY50);
    Delay_ms(50);
}

/*********************************** endfile **********************************/
