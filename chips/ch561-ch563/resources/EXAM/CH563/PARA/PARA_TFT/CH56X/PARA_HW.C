/********************************** (C) COPYRIGHT ******************************
* File Name          : PARA_HW.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/09/2
* Description        : 总线接口函数
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include    "HAL.H"

PUINT16V        pXbusPt;

void CH563_PORT_INIT(void)
{

    R32_PA_DIR = (1<<21)|( 1<<20 )|( 1<<19 )|(1<<16);                           /* 置地址、WR、RD、CS信号输出 */ 
    R32_PA_PU  = ((1<<21)|( 1<<20 )|( 1<<19 ))&(~(1<<16));                      /* GPIO A上拉设置,置1表示上拉 */

    R8_SAFE_ACCESS_SIG = 0x57;                                                  /* unlock step 1 */
    R8_SAFE_ACCESS_SIG = 0xA8;                                                  /* unlock step 2 */
    R8_XBUS_CONFIG = RB_XBUS_ENABLE | RB_XBUS_ADDR_OE | RB_XBUS_EN_32BIT;       /* 外部总线使能 */
    R8_XBUS_SETUP_HOLD = 0x04;                                                  /* 1 setup clocks */
    R8_SAFE_ACCESS_SIG = 0x00;                                                  /* lock, to prevent unexpected writing */
}

/* 写寄存器函数 */
/* CMD:寄存器值 */
void  LCD_WR_REG( UINT16 reg )          
{
    pXbusPt = ( PUINT16V )0x00C00000;
    *pXbusPt = reg;    
}

/* 写LCD数据 */
/* data:要写入的值 */
void  LCD_WR_DATA( UINT16 mData )          
{
    pXbusPt = ( PUINT16V )0x00C10000;
    *pXbusPt = mData;
}

/* 读LCD数据 */
/* 返回值:读到的值 */
UINT16 LCD_RD_DATA( void  )          
{
    UINT16 mData;

    pXbusPt = ( PUINT16V )0x00C10000;
    mData = *pXbusPt ;
    return (mData);
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
    LCD_WR_DATA( RGB_Code );                                                    /* 写十六位GRAM */
}

/*********************************** endfile **********************************/
