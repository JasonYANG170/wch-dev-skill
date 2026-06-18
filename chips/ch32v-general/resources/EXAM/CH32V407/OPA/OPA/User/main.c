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
 *@Note
 OPA1 is used as voltage follower output
 OPA1_CHP1--PB12
 OPA1_CHN1--PB13
 OPA1_OUT--PB1

  In this example, PB1 and PB13 are short-circuited, and PB12 is connected to input voltage.
  If the negative feedback is connected to a resistor, it can also form an
  operational amplifier.
*/

#include "debug.h"

/* OPA Mode Definition*/
#define OPA_Diff_Mode   0
#define OPA_Sign_Mode     1

/* OPA Mode Selection*/
//#define OPA_Mode   OPA_Diff_Mode
#define OPA_Mode   OPA_Sign_Mode

/* OPA_HS Enable Definition */
#define OPA_HS_ENABLE   0
#define OPA_HS_DISABLE  1

/* OPA_HS  Enable Selection */
#define OPA_HS_Mode   OPA_HS_ENABLE
//#define OPA_HS_Mode   OPA_HS_DISABLE


/*********************************************************************
 * @fn      OPA_Init
 *
 * @brief   Initializes OPA collection.
 *
 * @return  none
 */
void OPA1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    OPA_InitTypeDef  OPA_InitStructure={0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    OPA_InitStructure.PSEL=CHP0; 
    OPA_InitStructure.Mode=OUT_IO_OUT0;
#if  (OPA_Mode == OPA_Sign_Mode)
    OPA_InitStructure.NSEL=CHN0;
    OPA_InitStructure.PGADIF=PGADIF_GND;
    OPA_InitStructure.FB=FB_OFF; 
#else
    OPA_InitStructure.NSEL=CHN_PGA_8xIN;
    OPA_InitStructure.PGADIF=PGADIF_IO;
    OPA_InitStructure.FB=FB_ON;
#endif

#if  (OPA_HS_Mode == OPA_HS_ENABLE)
     OPA_InitStructure.HS=HS_ON;
#else
     OPA_InitStructure.HS=HS_OFF;
#endif
   
    OPA_Init(&OPA_InitStructure);
    OPA_Cmd(ENABLE);

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
    SystemCoreClockUpdate();
    Delay_Init();
    USART_Printf_Init(115200);	
    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

    printf("OPA TEST\r\n");
    OPA1_Init(); 

    while(1);
}
