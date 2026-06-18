/********************************** (C) COPYRIGHT ******************************
* File Name          : CHINESE.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : define for CHINESE.C
*******************************************************************************/



/******************************************************************************/
#ifndef __CHINESE_H
#define __CHINESE_H	 
#include "CH563SFR.H"

void TEST_FONT(void);
void Test_Show_CH_Font16(UINT16 x,UINT16 y,UINT8 *s,UINT16 color);
void Test_Show_CH_Font24(UINT16 x,UINT16 y,UINT8 *s,UINT16 color);
		  				    
#endif
