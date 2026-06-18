/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/12/01
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

  /*
  FSMC routine to operate TFTLCD-:
  LCD--PIN:
    PD7--NE1(CS)
    PD12--RS
    PD5 --FSMC_NEW(WR)
    PD4 --FSMC_NOE(RD)
    PA15 --LCDRST#
    PD14--FSMC_D0
    PD15--FSMC_D1
    PD0 --FSMC_D2
    PD1--FSMC_D3
    PE7--FSMC_D4
    PE8 --FSMC_D5
    PE9 --FSMC_D6
    PE10--FSMC_D7
    PE11--FSMC_D8
    PB11--FSMC_D9
    PB12--FSMC_D10
    PB13--FSMC_D11
    PB14--FSMC_D12
    PD8 --FSMC_D13
    PD9 --FSMC_D14
    PD10--FSMC_D15
    PB1--BL
*/


#include "debug.h"
#include "lcd.h"
/* Global define */

/* Global Variable */


/*********************************************************************
 * @fn      LCD_Reset_GPIO_Init
 *
 * @brief   Init LCD reset GPIO.
 *
 * @return  none
 */
void LCD_Reset_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure={0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_15);
}
/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	u8 x=0;
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);	
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    printf("LCD TEST\r\n");
	LCD_Reset_GPIO_Init();
	//LCD reset
	GPIO_ResetBits(GPIOA,GPIO_Pin_15);
	Delay_Ms(100);
	GPIO_SetBits(GPIOA,GPIO_Pin_15);

  LCD_Init();
	POINT_COLOR=RED;		
	 
    while(1) 
	{		 
		switch(x)
		{
			case 0:LCD_Clear(WHITE);break;
			case 1:LCD_Clear(BLACK);break;
			case 2:LCD_Clear(BLUE);break;
			case 3:LCD_Clear(RED);break;
			case 4:LCD_Clear(MAGENTA);break;
			case 5:LCD_Clear(GREEN);break;
			case 6:LCD_Clear(CYAN);break;

			case 7:LCD_Clear(YELLOW);break;
			case 8:LCD_Clear(BRRED);break;
			case 9:LCD_Clear(GRAY);break;
			case 10:LCD_Clear(LGRAY);break;
			case 11:LCD_Clear(BROWN);break;
		}
		POINT_COLOR=RED;	  
		LCD_ShowString(30,40,210,24,24,"CH32V307");
		LCD_ShowString(30,70,200,16,16,"TFTLCD TEST");
		LCD_ShowString(30,90,200,16,16,"WCH");
	    x++;
		if(x==12)x=0;			   		 
		Delay_Ms(1000);	

	} 

}
