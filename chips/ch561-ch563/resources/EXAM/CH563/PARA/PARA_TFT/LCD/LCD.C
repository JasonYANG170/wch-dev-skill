/********************************** (C) COPYRIGHT ******************************
* File Name          : LCD.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : LCD初始化及显示
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include "LCD.H"
#include "FONT.H" 
#include "SYSFREQ.H"
#include "..\PARA\PARA_TFT\CH56X\HAL.H"
                 
/* LCD的画笔颜色和背景色 */       
UINT16 POINT_COLOR=0x0000;                                                      /* 画笔颜色 */
UINT16 BACK_COLOR=0xFFFF;                                                       /* 背景色 */ 
_lcd_dev lcddev;                                                                /* 管理LCD重要参数 */
    
/*******************************************************************************
* Function Name  : LCD_BGR2RGB
* Description    : 从ILI93xx读出的数据为GBR格式，而我们写入的时候为RGB格式,通过该函数转换
* Input          : c- GBR格式的颜色值 
* Output         : None
* Return         : RGB格式的颜色值
*******************************************************************************/
UINT16 LCD_BGR2RGB(UINT16 c)
{
    UINT16  r,g,b,rgb;   
    b=(c>>0)&0x1f;
    g=(c>>5)&0x3f;
    r=(c>>11)&0x1f;     
    rgb=(b<<11)+(g<<5)+(r<<0);         
    return(rgb);
}

/*******************************************************************************
* Function Name  : LCD_ReadPoint
* Description    : 读取个某点的颜色值
* Input          : x,y:坐标 
* Output         : None
* Return         : 此点的颜色
*******************************************************************************/
UINT16 LCD_ReadPoint(UINT16 x,UINT16 y)
{
    UINT16 r=0,g=0,b=0;
    if(x>=lcddev.width||y>=lcddev.height)return 0;                              /* 超过了范围,直接返回 */           
    LCD_SetCursor(x,y);        
    if(lcddev.id==0X9341||lcddev.id==0X6804)LCD_WR_REG(0X2E);                   /* 9341/6804 发送读GRAM指令 */
    else LCD_WR_REG(R34);                                                       /* 其他IC发送读GRAM指令 */
    if(lcddev.id==0X9320)Delay_us(2);                                           /* FOR 9320,延时2us */        
    if( LCD_RD_DATA( ) )r=0;                                                    /* dummy Read */       
    Delay_us(2);      
    r=LCD_RD_DATA( );                                                           /* 实际坐标颜色 */
    if(lcddev.id==0X9341){                                                      /* 9341要分2次读出 */
        Delay_us(2);      
        b=LCD_RD_DATA( ); 
        g=r&0XFF;                                                               /* 对于9341,第一次读取的是RG的值,R在前,G在后,各占8位 */
        g<<=8;
    }
    else if(lcddev.id==0X6804)r=LCD_RD_DATA( );                                 /* 6804第二次读取的才是真实值 */
    if(lcddev.id==0X9325||lcddev.id==0X4535||lcddev.id==0X4531||lcddev.id==0X8989||lcddev.id==0XB505){
        return r;                                                               /* 这几种IC直接返回颜色值 */
    }
    else if(lcddev.id==0X9341)return (((r>>11)<<11)|((g>>10)<<5)|(b>>11));      /* ILI9341需要公式转换一下 */
    else return LCD_BGR2RGB(r);                                                 /* 其他IC */
}

/*******************************************************************************
* Function Name  : LCD_DisplayOn
* Description    : LCD开启显示
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOn( void )
{                       
    if(lcddev.id==0X9341||lcddev.id==0X6804)LCD_WR_REG(0X29);                   /* 开启显示 */
    else LCD_WriteReg(R7,0x0173);                                               /* 开启显示 */
}

/*******************************************************************************
* Function Name  : LCD_DisplayOn
* Description    : LCD关闭显示
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DisplayOff(void)
{       
    if(lcddev.id==0X9341||lcddev.id==0X6804)LCD_WR_REG(0X28);                   /* 关闭显示 */
    else LCD_WriteReg(R7,0x0);//关闭显示 
}

/*******************************************************************************
* Function Name  : LCD_DisplayOn
* Description    : 设置光标位置
* Input          : Xpos:横坐标
*                   Ypos:纵坐标
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetCursor(UINT16 Xpos, UINT16 Ypos)
{              
    LCD_WR_REG(lcddev.setxcmd); 
    LCD_WR_DATA(Xpos>>8); 
    LCD_WR_DATA(Xpos&0XFF);     
    LCD_WR_REG(lcddev.setycmd); 
    LCD_WR_DATA(Ypos>>8); 
    LCD_WR_DATA(Ypos&0XFF);
} 

/*******************************************************************************
* Function Name  : LCD_SetWindows
* Description    : 设置lcd显示窗口
* Input          : xStar- 起始x位置
*                  yStar- 起始y位置
*                  xEnd - x结束位置
*                  yEnd - y结束位置
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_SetWindows(UINT16 xStar, UINT16 yStar,UINT16 xEnd,UINT16 yEnd)
{              
    LCD_WR_REG(lcddev.setxcmd); 
 
    LCD_WR_DATA( xStar>>8);
    LCD_WR_DATA( xStar&0xFF);
    LCD_WR_DATA(xEnd>>8); 
    LCD_WR_DATA(xEnd&0XFF);     

    LCD_WR_REG(lcddev.setycmd); 
 
    LCD_WR_DATA( yStar>>8 );
    LCD_WR_DATA( yStar&0xFF );
    LCD_WR_DATA( yEnd>>8 ); 
    LCD_WR_DATA( yEnd&0XFF );
#if 1
    LCD_WR_REG( 0x3A );    // 16/18 bits
    LCD_WR_DATA( 0x55 );
#endif
    LCD_WR_REG( lcddev.wramcmd );
}

/*******************************************************************************
* Function Name  : LCD_DrawPoint
* Description    : 画点
* Input          : x,y:坐标
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawPoint(UINT16 x,UINT16 y)
{
    LCD_SetCursor(x,y);                                                         /* 设置光标位置 */ 
    LCD_WriteRAM_Prepare();                                                     /* 开始写入GRAM */
    LCD_WR_DATA( POINT_COLOR ); 
}

/*******************************************************************************
* Function Name  : LCD_Fast_DrawPoint
* Description    : 快速画点
* Input          : x,y:坐标
*                   color:颜色
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Fast_DrawPoint(UINT16 x,UINT16 y,UINT16 color)
{       
    if(lcddev.id==0X9341||lcddev.id==0X6804){
        LCD_WR_REG(lcddev.setxcmd); 
        LCD_WR_DATA(x>>8); 
        LCD_WR_DATA(x&0XFF);     
        LCD_WR_REG(lcddev.setycmd); 
        LCD_WR_DATA(y>>8); 
        LCD_WR_DATA(y&0XFF);
    }
    else{
        if(lcddev.dir==1)x=lcddev.width-1-x;                                   /* 横屏其实就是调转x,y坐标 */
        LCD_WriteReg(lcddev.setxcmd,x);
        LCD_WriteReg(lcddev.setycmd,y);
    }             
    LCD_WR_REG( lcddev.wramcmd ); 
    LCD_WR_DATA( color );
}     

/*******************************************************************************
* Function Name  : LCD_Display_Dir
* Description    : 设置LCD显示方向
* Input          : dir:0,竖屏；1,横屏
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Display_Dir(UINT8 dir)
{
    if(dir==0){     /* 竖屏 */
        lcddev.width=320;
        lcddev.height=480;
        lcddev.wramcmd=0X2C;
        lcddev.setxcmd=0X2A;
        lcddev.setycmd=0X2B; 
        LCD_WriteReg(0x36,0x02); //从左到右,从上到下
    }
    else{          /* 横屏 */		  
        lcddev.width=480;
        lcddev.height=320;
        lcddev.wramcmd=0X2C;
        lcddev.setxcmd=0X2A;
        lcddev.setycmd=0X2B;     
        LCD_WriteReg(0x36,0x23); //从下到上,从右到左
    } 
}

/*******************************************************************************
* Function Name  : LCD_RESET
* Description    : 用于复位lcd
* Input          : s-(0:低电平,1:高电平)
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_RESET( UINT8 s )
{
#if 1
    R32_PB_DIR |= (1<<2); 
    R32_PB_PU |= (1<<2); 
    if(s) R32_PB_OUT |= (1<<2);
    else  R32_PB_CLR |= (1<<2); 
    R32_PB_DIR &= ~(1<<2); 

#else
    R32_PD_DIR |= led_control; 
    R32_PD_PU |= led_control; 
    if(s) R32_PD_OUT |= led_control;
    else  R32_PD_CLR |= led_control; 
    R32_PD_DIR &= ~led_control; 
#endif
}

/*******************************************************************************
* Function Name  : LCD_Init
* Description    : 初始化lcd
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Init(void)
{                                       
    Delay_ms(50);     
    LCD_RESET( 0 );
    Delay_ms(200);                                                              /* reset */
    LCD_RESET( 1 );
    Delay_ms(800);  
    PRINT("LCD INIT...\r\n");
 
    lcddev.id=0x6804;             
    PRINT(" LCD ID:%x\r\n",lcddev.id);                                          /* 打印LCD ID */

    LCD_WR_REG(0x0011);
    Delay_ms(20);
    LCD_WR_REG(0x00D0);    
    LCD_WR_DATA(0x0007); 
    LCD_WR_DATA(0x0041); 
    LCD_WR_DATA(0x001C); 
     
    LCD_WR_REG(0x00D1);    
    LCD_WR_DATA(0x0000); 
    LCD_WR_DATA(0x0036);   // 30 vcm
    LCD_WR_DATA(0x001B);   // 15 vdv
                                                                     
    LCD_WR_REG(0x00D2);
    LCD_WR_DATA(0x0001);   // AP0[2:0]
    LCD_WR_DATA(0x0011);   // DC10[2:0],DC00[2:0]
      
    LCD_WR_REG(0x00C0);    //*************
    LCD_WR_DATA(0x0010);   // REV & SM & GS
    LCD_WR_DATA(0x003B);   // NL[5:0]
    LCD_WR_DATA(0x0000);   // SCN[6:0]
    LCD_WR_DATA(0x0012);   // 02 NDL , PTS[2:0]
    LCD_WR_DATA(0x0001);   // 11 PTG , ISC[3:0]  
      
    LCD_WR_REG(0x00C5);    
    LCD_WR_DATA(0x0003);
    
    LCD_WR_REG(0x00C8);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0057);
    LCD_WR_DATA(0x0033);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0044);
    LCD_WR_DATA(0x0002);
    LCD_WR_DATA(0x0077);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0000);
      
    LCD_WR_REG(0x00F8);
    LCD_WR_DATA(0x0001);
      
    LCD_WR_REG(0x00FE);
    LCD_WR_DATA(0x0000);
    LCD_WR_DATA(0x0002);
      
    LCD_WR_REG(0x0036); 
    LCD_WR_DATA(0x000A); 
    Delay_ms(20);
    LCD_WR_REG(0x003a);  
    LCD_WR_DATA(0x0005);
    Delay_ms(20);  
    
    LCD_WR_REG(0x0029); 
    LCD_WR_REG(0x002c);

    lcddev.dir=0;                                                               /* 横屏 */
    LCD_Display_Dir( lcddev.dir );
    LCD_Clear(BLACK);
    Delay_ms(800);  
} 

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : 清屏函数
* Input          : color-要清屏的填充色
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Clear(UINT16 color)
{
    UINT32 index=0;      
    UINT32 totalpoint=lcddev.width;    
    totalpoint*=lcddev.height;                                                  /* 得到总点数 */
    LCD_SetWindows(0,0,lcddev.width-1,lcddev.height-1);
    LCD_WriteRAM_Prepare( );                                                    /* 开始写入GRAM */
    for(index=0;index<totalpoint;index++)
    {
        LCD_WR_DATA( color );
    }
}

/*******************************************************************************
* Function Name  : LCD_Fill
* Description    : 在指定区域内填充单个颜色
* Input          : (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1) 
*                  color:要填充的颜色
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Fill(UINT16 sx,UINT16 sy,UINT16 ex,UINT16 ey,UINT16 color)
{          
    UINT16 i,j;
    UINT16 xlen=0;
    xlen=ex-sx+1;       
    for(i=sy;i<=ey;i++)
    {
        LCD_SetCursor(sx,i);                                                    /* 设置光标位置 */ 
        LCD_WriteRAM_Prepare();                                                 /* 开始写入GRAM    */  
        for(j=0;j<xlen;j++) LCD_WR_DATA(color);                                 /* 设置光标位置 */         
    }
}

/*******************************************************************************
* Function Name  : LCD_Color_Bulk_Fill
* Description    : 在指定区域内填充指定颜色块
* Input          : (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
                   color:要填充的颜色
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Color_Bulk_Fill(UINT16 sx,UINT16 sy,UINT16 ex,UINT16 ey,UINT16 *color)
{  
    UINT16 height,width;
    UINT16 i,j;

    width=ex-sx+1;                                                              /* 得到填充的宽度 */
    height=ey-sy+1;                                                             /* 高度 */
    for(i=0;i<height;i++){
        LCD_SetCursor(sx,sy+i);                                                 /* 设置光标位置 */ 
        LCD_WriteRAM_Prepare();                                                 /* 开始写入GRAM */
        for(j=0;j<width;j++) LCD_WR_DATA( color[i*height+j] );                  /* 写入数据 */
    }      
}

/*******************************************************************************
* Function Name  : LCD_Color_Fill
* Description    : 在指定区域内填充随机颜色
* Input          : (sx,sy),(ex,ey):填充矩形对角坐标,区域大小为:(ex-sx+1)*(ey-sy+1)
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_Color_Fill(UINT16 sx,UINT16 sy,UINT16 ex,UINT16 ey)
{  
    UINT16 height,width;
    UINT16 i,j;

    width=ex-sx+1;                                                              /* 得到填充的宽度 */
    height=ey-sy+1;                                                             /* 高度 */
    for(i=0;i<height;i++)
    {
        LCD_SetCursor(sx,sy+i);                                                 /* 设置光标位置 */ 
        LCD_WriteRAM_Prepare();                                                 /* 开始写入GRAM */
        for(j=0;j<width;j++) LCD_WR_DATA( i*height+j*width );                   /* 写入数据 */ 
    }      
}

/*******************************************************************************
* Function Name  : LCD_DrawLine
* Description    : 画线
* Input          : x1,y1:起点坐标
*                  x2,y2:终点坐标 
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawLine(UINT16 x1, UINT16 y1, UINT16 x2, UINT16 y2)
{
    UINT16 t; 
    int xerr=0,yerr=0,delta_x,delta_y,distance; 
    int incx,incy,uRow,uCol; 

    delta_x=x2-x1;                                                              /* 计算坐标增量 */ 
    delta_y=y2-y1; 
    uRow=x1; 
    uCol=y1; 
    if(delta_x>0)incx=1;                                                        /* 设置单步方向 */
    else if(delta_x==0)incx=0;                                                  /* 垂直线 */ 
    else {incx=-1;delta_x=-delta_x;} 
    if(delta_y>0)incy=1; 
    else if(delta_y==0)incy=0;                                                  /* 水平线 */
    else{incy=-1;delta_y=-delta_y;} 
    if( delta_x>delta_y)distance=delta_x;                                       /* 选取基本增量坐标轴 */ 
    else distance=delta_y; 
    for(t=0;t<=distance+1;t++ ){                                                /* 画线输出 */ 
        LCD_DrawPoint(uRow,uCol);                                               /* 画点 */ 
        xerr+=delta_x ; 
        yerr+=delta_y ; 
        if(xerr>distance){ 
            xerr-=distance; 
            uRow+=incx; 
        } 
        if(yerr>distance){ 
            yerr-=distance; 
            uCol+=incy; 
        } 
    }  
}

/*******************************************************************************
* Function Name  : LCD_DrawRectangle
* Description    : 画矩形
* Input          : x1,y1:起点坐标
*                  x2,y2:终点坐标 
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_DrawRectangle(UINT16 x1, UINT16 y1, UINT16 x2, UINT16 y2)
{
    LCD_DrawLine(x1,y1,x2,y1);
    LCD_DrawLine(x1,y1,x1,y2);
    LCD_DrawLine(x1,y2,x2,y2);
    LCD_DrawLine(x2,y1,x2,y2);
}

/*******************************************************************************
* Function Name  : Draw_Circle
* Description    : 在指定位置画一个指定大小的圆
* Input          : (x,y):中心点
*                  r    :半径
* Output         : None
* Return         : None
*******************************************************************************/
void Draw_Circle(UINT16 x0,UINT16 y0,UINT8 r)
{
    int a,b;
    int di;
    a=0;b=r;      
    di=3-(r<<1);                                                                /* 判断下个点位置的标志 */
    while(a<=b)
    {
        LCD_DrawPoint(x0+a,y0-b);             //5
        LCD_DrawPoint(x0+b,y0-a);             //0           
        LCD_DrawPoint(x0+b,y0+a);             //4               
        LCD_DrawPoint(x0+a,y0+b);             //6 
        LCD_DrawPoint(x0-a,y0+b);             //1       
        LCD_DrawPoint(x0-b,y0+a);             
        LCD_DrawPoint(x0-a,y0-b);             //2             
        LCD_DrawPoint(x0-b,y0-a);             //7                  
        a++;
        /* 使用Bresenham算法画圆 */     
        if(di<0) di +=4*a+6;      
        else{
            di+=10+4*(a-b);   
            b--;
        }                             
    }
}

/*******************************************************************************
* Function Name  : LCD_ShowChar
* Description    : 在指定位置显示一个字符
* Input          : x,y:起始坐标
*                  num:要显示的字符:" "--->"~"
*                  size:字体大小 12/16
*                  mode:叠加方式(1)还是非叠加方式(0)
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_ShowChar(UINT16 x,UINT16 y,UINT8 num,UINT8 size,UINT8 mode)
{                                
    UINT8 temp,t1,t;
    UINT16 y0=y;
    UINT16 colortemp=POINT_COLOR;                       
    /* 设置窗口 */           
    num=num-' ';                                                                /* 得到偏移后的值 */
    if(!mode){                                                                   /* 非叠加方式 */
        for(t=0;t<size;t++){
            if(size==12)temp=asc2_1206[num][t];                                 /* 调用1206字体 */
            else temp=asc2_1608[num][t];                                        /* 调用1608字体 */                              
            for(t1=0;t1<8;t1++){
                if(temp&0x80)POINT_COLOR=colortemp;
                else POINT_COLOR=BACK_COLOR;
                LCD_DrawPoint(x,y);    
                temp<<=1;
                y++;
                if(x>=lcddev.width){                                            /* 超区域了 */
                    POINT_COLOR=colortemp;
                    return;
                }              
                if((y-y0)==size){
                    y=y0;
                    x++;
                    if(x>=lcddev.width){                                         /* 超区域了 */
                        POINT_COLOR=colortemp;
                        return;
                    }         
                    break;
                }
            }
                   
        }    
    }
    else{                                                                       /* 叠加方式 */
        for(t=0;t<size;t++){
            if(size==12)temp=asc2_1206[num][t];                                 /* 调用1206字体 */
            else temp=asc2_1608[num][t];                                        /* 调用1608字体 */                               
            for(t1=0;t1<8;t1++){
                if(temp&0x80)LCD_DrawPoint(x,y); 
                temp<<=1;
                y++;
                if(x>=lcddev.height){                                           /* 超区域了 */
                    POINT_COLOR=colortemp;
                    return;
                }
                if((y-y0)==size){
                    y=y0;
                    x++;
                    if(x>=lcddev.width){                                        /* 超区域了 */
                        POINT_COLOR=colortemp;
                        return;
                    }
                    break;
                }
            }       
        }     
    }
    POINT_COLOR=colortemp;                          
}

/*******************************************************************************
* Function Name  : LCD_Pow
* Description    : 清屏函数
* Input          : m^n函数
* Output         : None
* Return         : 返回值:m^n次方.
*******************************************************************************/
UINT32 LCD_Pow(UINT8 m,UINT8 n)
{
    UINT32 result=1;     
    while(n--)result*=m;    
    return result;
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : 显示数字,高位为0,则不显示
* Input          : x,y :起点坐标     
*                  len :数字的位数
*                  size:字体大小
*                  color:颜色 
*                  num:数值(0~4294967295);     
* Output         : None
* Return         : 返回值:m^n次方.
*******************************************************************************/
void LCD_ShowNum(UINT16 x,UINT16 y,UINT32 num,UINT8 len,UINT8 size)
{             
    UINT8 t,temp;
    UINT8 enshow=0;                           
    for(t=0;t<len;t++){
        temp=(num/LCD_Pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1)){
            if(temp==0){
                LCD_ShowChar(x+(size/2)*t,y,' ',size,0);
                continue;
            }
            else 
                enshow=1; 
        }
        LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,0); 
    }
}

/*******************************************************************************
* Function Name  : LCD_Clear
* Description    : 显示数字,高位为0,还是显示
* Input          : x,y :起点坐标     
*                  len :数字的位数
*                  size:字体大小
*                  color:颜色 
*                  num:数值(0~999999999)
*                  mode:[7]:0,不填充;1,填充0.
                        [6:1]:保留
                        [0]:0,非叠加显示;1,叠加显示.
* Output         : None
* Return         : 返回值:m^n次方.
*******************************************************************************/
void LCD_ShowxNum(UINT16 x,UINT16 y,UINT32 num,UINT8 len,UINT8 size,UINT8 mode)
{  
    UINT8 t,temp;
    UINT8 enshow=0;                           
    for(t=0;t<len;t++){
        temp=(num/LCD_Pow(10,len-t-1))%10;
        if(enshow==0&&t<(len-1)){
            if(temp==0){
                if(mode&0X80)LCD_ShowChar(x+(size/2)*t,y,'0',size,mode&0X01);  
                else LCD_ShowChar(x+(size/2)*t,y,' ',size,mode&0X01);  
                continue;
            }
            else 
                enshow=1; 
        }
        LCD_ShowChar(x+(size/2)*t,y,temp+'0',size,mode&0X01); 
    }
} 

/*********************************** endfile **********************************/
