/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.c
* Author             : WCH
* Version            : V1.0
* Date               : 2024/06/19
* Description 		 : 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 *GPIO routine
 *
 */

#include "CH56x_common.h"

#define  FREQ_SYS   80000000

void GPIO_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/* Low power consumption Mode Definition */
#define  IDLE_MODE  0
#define  HALT_MODE  1
#define  SlEEP_MODE 2
//#define  LP_MODE  IDLE_MODE
//#define  LP_MODE  HALT_MODE
#define  LP_MODE  SlEEP_MODE

/* Execute with WFI or WFE Definition */
//#define  Enter_WFI  1
#define  Enter_WFE  1

/*******************************************************************************
 * @fn       DebugInit
 *
 * @brief    Initializes the UART1 peripheral.
 *
 * @param    baudrate: UART1 communication baud rate.
 *
 * @return   None
 */
void DebugInit(UINT32 baudrate)
{
	UINT32 x;
	UINT32 t = FREQ_SYS;
	
	x = 10 * t * 2 / 16 / baudrate;
	x = ( x + 5 ) / 10;
	R8_UART1_DIV = 1;
	R16_UART1_DL = x;
	R8_UART1_FCR = RB_FCR_FIFO_TRIG | RB_FCR_TX_FIFO_CLR | RB_FCR_RX_FIFO_CLR | RB_FCR_FIFO_EN;
	R8_UART1_LCR = RB_LCR_WORD_SZ;
	R8_UART1_IER = RB_IER_TXD_EN;
	R32_PA_SMT |= (1<<8) |(1<<7);
	R32_PA_DIR |= (1<<8);
}

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main()
{
	SystemInit(FREQ_SYS);
	Delay_Init(FREQ_SYS);
    GPIOA_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU_NSMT);
    GPIOB_ModeCfg(GPIO_Pin_All, GPIO_ModeIN_PU_NSMT);
/* Configure serial debugging */
	DebugInit(115200);
	PRINT("Start @ChipID=%02X\r\n", R8_CHIP_ID);
	DelayMs(100);

#if Enter_WFI
	PFIC_SetPriority(GPIO_IRQn, 0x1 << 4);
    PFIC_EnableIRQ(GPIO_IRQn);  //Enable wake-up source corresponding interrupt.
#endif
    GPIOA_ITModeCfg(GPIO_Pin_2,GPIO_ITMode_LowLevel);  //Configure wake-up source mode.
    PWR_PeriphWakeUpCfg(ENABLE,RB_SLP_GPIO_WAKE);     //Enable the required wake-up source.

#if Enter_WFI
    #if(LP_MODE == IDLE_MODE)
        PRINT("IDLE_MODE Test-WFI \r\n");
        DelayMs(10);
        LowPower_Idle( );
    #elif(LP_MODE == HALT_MODE)
        PRINT("HALT_MODE Test-WFI \r\n");
        DelayMs(10);
        LowPower_Halt( );
    #elif(LP_MODE == SlEEP_MODE)
        PRINT("SlEEP_MODE Test-WFI \r\n");
        DelayMs(10);
        LowPower_Sleep( );
    #endif
#endif

#if Enter_WFE
    #if(LP_MODE == IDLE_MODE)
        PRINT("IDLE_MODE Test-WFE \r\n");
        DelayMs(10);
        LowPower_Idle_WFE( );
    #elif(LP_MODE == HALT_MODE)
        PRINT("HALT_MODE Test-WFE \r\n");
        DelayMs(10);
        LowPower_Halt_WFE( );
    #elif(LP_MODE == SlEEP_MODE)
        PRINT("SlEEP_MODE Test-WFE \r\n");
        DelayMs(10);
        LowPower_Sleep_WFE( );
    #endif
    #if(LP_MODE != IDLE_MODE)
    R8_SAFE_ACCESS_SIG = 0x57;
    R8_SAFE_ACCESS_SIG = 0xA8;
    R8_SLP_POWER_CTRL &= ~RB_SLP_USBHS_PWRDN; // Power on USBHS module
    USBSS->LINK_CFG |= 0x1 << 12;             // Recovery USB PHY
    R8_SAFE_ACCESS_SIG = 0x00;
    #endif
#endif

    PRINT("wake up!!!\r\n");
	while(1)
	{
	    PRINT("Run in main\r\n");
	    DelayMs(1000);
	}
}

/*******************************************************************************
 * @fn      GPIO_IRQHandler
 *
 * @brief   Interruption function
 *
 * @return  None
 */
#if Enter_WFI
void GPIO_IRQHandler(void)
{
	if(GPIOA_2_ReadITFlagBit( ))
	{
      #if(LP_MODE != IDLE_MODE)
	    R8_SAFE_ACCESS_SIG = 0x57;
	    R8_SAFE_ACCESS_SIG = 0xA8;
	    R8_SLP_POWER_CTRL &= ~RB_SLP_USBHS_PWRDN; // Power on USBHS module
	    USBSS->LINK_CFG |= 0x1 << 12;             // Recovery USB PHY
	    R8_SAFE_ACCESS_SIG = 0x00;
      #endif
	    PRINT(" get a interruption!\r\n");
	}
	GPIOA_2_ClearITFlagBit( );
}
#endif
