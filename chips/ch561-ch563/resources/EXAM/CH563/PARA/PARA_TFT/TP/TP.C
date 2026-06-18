/********************************** (C) COPYRIGHT ******************************
* File Name          : TP.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : 触摸测试
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <stdio.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"
#include "tp.h"
#include "..\PARA\PARA_TFT\LCD\LCD.H"

#define SPI_SCK                ( 1 << 13 )                                    /* PB13 */
#define SPI_MOSI               ( 1 << 14 )                                    /* PB14 */
#define SPI_MISO               ( 1 << 15 )                                    /* PB15 */
#define SPI_SCS                ( 1 << 6 )                                     /* PB6 */
#define SPI_INT                ( 1 << 2 )                                     /* PB2 */

#define SPI_SCK_LOW( )      { R32_PB_CLR |= SPI_SCK; }  
#define SPI_SCK_HIGH( )     { R32_PB_OUT |= SPI_SCK; }  

#define SPI_MISO_PIN( )     ( R32_PB_PIN&SPI_MISO ) 

#define SPI_MOSI_LOW( )     { R32_PB_CLR |= SPI_MOSI; } 
#define SPI_MOSI_HIGH( )    { R32_PB_OUT |= SPI_MOSI; }  

#define SPI_SCS_LOW( )      { R32_PB_CLR |= SPI_SCS; }  
#define SPI_SCS_HIGH( )     { R32_PB_OUT |= SPI_SCS; }  

#define SPI_INT_PIN( )      ( R32_PB_PIN&SPI_INT )

#define SPI_START_CMD( )     { R32_PB_OUT |= SPI_SCS; } 
#define SPI_END_CMD( )       { R32_PB_OUT |= SPI_SCS; } 

/*******************************************************************************
* Function Name  : SPI_MASTER_INIT
* Description    : SPI初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_MASTER_INIT ( void )
{
    R32_PB_DIR &= ~(SPI_INT);
    R32_PB_PU  |= (SPI_INT);

    R32_PB_DIR &= ~(SPI_MISO);
    R32_PB_PU  |= (SPI_MISO);
    R32_PB_DIR |= (SPI_SCK|SPI_SCS|SPI_MOSI); 
    R32_PB_PU  |= (SPI_SCK|SPI_SCS|SPI_MOSI);
}

/*******************************************************************************
* Function Name  : SPI_TP_Write
* Description    : 写
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SPI_TP_Write( UINT8 cmd )
{
    UINT8 i;

    SPI_SCS_LOW( );
    Delay_us(5);
    for(i=0;i<8;i++){
        SPI_SCK_LOW( );
        if( (cmd&0x80)==0 ){
            SPI_MOSI_LOW( );
        }
        else{  
            SPI_MOSI_HIGH();    
        }
        SPI_SCK_HIGH( );
        cmd=cmd<<1;
    }
    Delay_us(5);
}

/*******************************************************************************
* Function Name  : SPI_TP_Read
* Description    : 读
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT16 SPI_TP_Read( void )
{
    UINT8   i;
    UINT16     val=0;

    for(i=0;i<12;i++){
        SPI_SCK_LOW( );
        val = val<<1;
        val = val | (SPI_MISO_PIN()?1:0);
        SPI_SCK_HIGH( );
    }
    return val;
}

/*******************************************************************************
* Function Name  : TP_GetPosition
* Description    : 获取触摸坐标值
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT16 TP_GetPosition( UINT8 cmd )
{

#define   READ_DATA_COUNT  24
    UINT8   i,j,k;
    UINT16  temp;
    UINT16  TP[READ_DATA_COUNT];


    i = 0;
    
    SPI_START_CMD( ); 
    while((i<READ_DATA_COUNT)&&(!SPI_INT_PIN( ))){
        SPI_TP_Write(cmd);
        TP[i]=SPI_TP_Read( );
        i++;
    }
    SPI_END_CMD( );
    if(i==READ_DATA_COUNT){
        for(j=0;j<i;j++){
            for(k=0;k<i-j;k++){
                if (*(TP+k)>*(TP+k+1)){
                    temp=*(TP+k);
                    *(TP+k)=*(TP+k+1);
                    *(TP+k+1)=temp;
                }
            }
        }
        return (TP[i>>1]);
    }
    else{
        return 0;
    }
}

/*******************************************************************************
* Function Name  : TP_DrawLine_TEST
* Description    : 触摸测试子程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void TP_DrawLine_TEST(void)
{
    UINT16   gx,gy;                   // coordinate on glass
    UINT16   tx,ty;                   // coordinate value read from TP
    UINT16   tx_tmp=0,ty_tmp=0;       // coordinate value read from TP
    UINT32   count=0;


    PRINT("TFT  TP Test!\n");
    SPI_MASTER_INIT( );
    POINT_COLOR = RED;
    Draw_Circle(20,460,2);
    Draw_Circle(300,460,2);
    Draw_Circle(300,20,2);
    Draw_Circle(20,20,2);
    Draw_Circle(160,240,2);
    POINT_COLOR = WHITE;
    R8_PB_PU_0 |= 1<<7;                                                         /* PB7上拉 */
    R8_PB_DIR_0 &= ~(1<<7);                                                     /* PB7输入 */
    while(1){
        count++;
        Delay_ms(1);
        if( (R8_PB_PIN_0&(1<<7)) == 0 ) LCD_Clear(BLACK);                
        if(count>50){
            count = 0;
            tx_tmp = ty_tmp = 0; 
        }
        while(!SPI_INT_PIN( )){
            Delay_ms(1);
            if(!SPI_INT_PIN( )){
				count = 0;
                tx=TP_GetPosition(0xD0);//x-position measure
                if(tx_tmp) tx_tmp = tx_tmp>tx?(tx_tmp-tx):(tx-tx_tmp); 
                if(tx_tmp>100) break; 
                tx_tmp = tx;
				tx = tx-130;
                gx = tx*5/28+tx%6;
                ty=TP_GetPosition(0x90);//y-position measure
                if(ty_tmp) ty_tmp = ty_tmp>ty?(ty_tmp-ty):(ty-ty_tmp); 
                if(ty_tmp>100) break; 
                ty_tmp = ty;
				ty = ty - 80;
				ty = ty - ty/20;
				gy = ty*4/15+ty%4;
                if((gx<lcddev.width)&&(gy<lcddev.height)){
       				LCD_Fill(gx-1,gy-1,gx+1,gy+1,POINT_COLOR);            
//				    POINT_COLOR--;
                }
            }
        }
    }
}

