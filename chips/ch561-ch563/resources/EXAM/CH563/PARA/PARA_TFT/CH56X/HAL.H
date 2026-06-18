/********************************** (C) COPYRIGHT ******************************
* File Name          : HAL.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : define for PARA_HW.C
*******************************************************************************/



/******************************************************************************/
#ifndef __HAL_H
#define __HAL_H		

#include "CH563BAS.H"
#include "CH563SFR.H"

void   CH563_PORT_INIT( void );  	                                        /* CH563通讯接口初始化 */
void   LCD_WR_REG( UINT16 CMD );
void   LCD_WR_DATA( UINT16 data );
UINT16 LCD_RD_DATA( void  );
void   LCD_WriteReg(UINT16 LCD_Reg, UINT16 LCD_RegValue);
UINT16 LCD_ReadReg(UINT16 LCD_Reg);
void   LCD_WriteRAM_Prepare(void);
void   LCD_WriteRAM(UINT16 RGB_Code);

#endif 

/*********************************** endfile **********************************/
