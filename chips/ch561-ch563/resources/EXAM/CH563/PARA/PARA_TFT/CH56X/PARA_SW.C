/********************************** (C) COPYRIGHT ******************************
* File Name          : PARA_SW.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : 模拟并口
*******************************************************************************/



/******************************************************************************/
#include    "HAL.H"

#define PARA_RS                ( 1 << 16 )                                    /* RS(PA16) */
#define PARA_CS                ( 1 << 19 )                                    /* CS(PA19) */
#define PARA_RD                ( 1 << 20 )                                    /* RD(PA20) */
#define PARA_WR                ( 1 << 21  )                                   /* WR(PA21) */
    

#define PIN_A0_LOW( )     { R32_PA_CLR |= PARA_RS; }  /* 模拟并口A0引脚输出低电平 */
#define PIN_A0_HIGH( )    { R32_PA_OUT |= PARA_RS; }  /* 模拟并口A0引脚输出高电平 */
#define PIN_PCS_LOW( )    { R32_PA_CLR |= PARA_CS; }  /* 模拟并口片选引脚输出低电平 */
#define PIN_PCS_HIGH( )   { R32_PA_OUT |= PARA_CS; }  /* 模拟并口片选引脚输出高电平 */
#define PIN_RD_LOW( )     { R32_PA_CLR |= PARA_RD; }  /* 模拟并口RD引脚输出低电平 */
#define PIN_RD_HIGH( )    { R32_PA_OUT |= PARA_RD; }  /* 模拟并口RD引脚输出高电平 */
#define PIN_WR_LOW( )     { R32_PA_CLR |= PARA_WR; }  /* 模拟并口WR引脚输出低电平 */
#define PIN_WR_HIGH( )    { R32_PA_OUT |= PARA_WR; }  /* 模拟并口WR引脚输出高电平 */


void CH563_PORT_INIT(void)
{
    R32_PD_DIR &= ~(0xFFFF);                                                    /* 禁止数据输出 */                            
    R32_PA_DIR = (1<<21)|( 1<<20 )|( 1<<19 )|(1<<16);                           /* 置地址、WR、RD信号输出 */ 
    R32_PA_PU  = (1<<21)|( 1<<20 )|( 1<<19 )|(1<<16); ;                         /* GPIO A 上拉设置， 置1表示上拉 */ 
    PIN_PCS_HIGH( );
    PIN_A0_HIGH( );
    PIN_WR_HIGH( );
    PIN_RD_HIGH( );
}

/*******************************************************************************
* Function Name  : LCD_WR_REG
* Description    : 写寄存器函数
* Input          : mCmd---要写入的命令
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WR_REG( UINT16 mCmd )  
{
    R32_PD_OUT = mCmd;                                                          /* 输出数据 */    
    R32_PD_DIR |= 0xFFFF;                                                       /* 设置并口方向为输出 */ 

    PIN_PCS_LOW( );
    PIN_A0_LOW( );

    PIN_WR_LOW( );                                                              /* 输出有效写控制信号 */        
    PIN_PCS_LOW( );                                                             /* 该操作无意义,仅作延时 */
    PIN_PCS_LOW( );                                                             /* 该操作无意义,仅作延时 */
    PIN_WR_HIGH( );                                                             /* 输出无效的控制信号 */
 
    PIN_PCS_HIGH( );
    R32_PD_DIR &= ~(0xFFFF);                                                    /* 禁止数据输出 */                            
    Delay_us( 2 );      
}

/*******************************************************************************
* Function Name  : LCD_WR_DATA
* Description    : 写数据
* Input          : mData---写数据
* Output         : None
* Return         : None
*******************************************************************************/
void LCD_WR_DATA( UINT16 mData )     
{
    R32_PD_OUT = mData ;                                                        /* 输出数据 */    
    R32_PD_DIR |= 0xFFFF;

    PIN_PCS_LOW( );    
    PIN_A0_HIGH( );    

    PIN_WR_LOW( );                                                              /* 输出有效写控制信号, 写命令 */        
    PIN_PCS_LOW( );                                                             /* 该操作无意义,仅作延时 */
    PIN_PCS_LOW( );                                                             /* 该操作无意义,仅作延时 */
    PIN_WR_HIGH( );                                                             /* 输出无效的控制信号 */
 
    PIN_PCS_HIGH( );    
    R32_PD_DIR &= ~(0xFFFF);                                                    /* 禁止数据输出 */                            
}

/*******************************************************************************
* Function Name  : LCD_RD_DATA
* Description    : 读数据
* Input          : None
* Output         : None
* Return         : 返回读取的数据
*******************************************************************************/
UINT16 LCD_RD_DATA( void )  
{
    UINT16  mData;

    Delay_us(1);
    R32_PD_DIR &= ~(0xFFFF);                                                    /* 方向为输入 */                            
 
    PIN_PCS_LOW( );
  
    PIN_A0_HIGH( );    

    PIN_RD_LOW( );                                                              /* 输出有效读控制信号, 读数据 */
    PIN_PCS_LOW( );                                                             /* 该操作无意义,仅作延时 */
    PIN_PCS_LOW( );                                                             /* 该操作无意义,仅作延时 */
    mData = (UINT16)( R32_PD_OUT );                                             /* 输入数据 */
    PIN_RD_HIGH( );                                                             /* 输出无效的控制信号 */
    PIN_PCS_HIGH( );
 
    return( mData );
}

/* 写寄存器 */
/* LCD_Reg:寄存器地址 */
/* LCD_RegValue:要写入的数据 */
void  LCD_WriteReg(UINT16 LCD_Reg, UINT16 LCD_RegValue)
{
    LCD_WR_REG( LCD_Reg );
    LCD_WR_DATA( LCD_RegValue );
}

/* 读寄存器 */
/* LCD_Reg:寄存器地址 */
/* 返回值:读到的数据 */
UINT16 LCD_ReadReg(UINT16 LCD_Reg)
{
    LCD_WR_REG( LCD_Reg );
    return( LCD_RD_DATA( ) );
}

/* 开始写GRAM */
void  LCD_WriteRAM_Prepare(void)
{
     LCD_WR_REG(lcddev.wramcmd);      
}
     
/* LCD写GRAM */
/* RGB_Code:颜色值 */
void  LCD_WriteRAM(UINT16 RGB_Code)
{                                
    LCD_WR_DATA( RGB_Code );                                                     /* 写十六位GRAM    */
}

/*******************************************************************************/
