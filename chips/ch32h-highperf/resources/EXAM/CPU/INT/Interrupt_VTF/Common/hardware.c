/********************************** (C) COPYRIGHT  *******************************
* File Name          : hardware.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/03/01
* Description        : This file provides all the hardware firmware functions.
*********************************************************************************
* Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "hardware.h"

/* Global Variable */ 
volatile uint32_t time0=0;
volatile uint32_t time1=0;


#if Func_Run_V3F 
void SysTick0_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SysTick0_Handler(void)
{
    time0=SysTick0->CNT;
    SysTick0->CTLR=0;
    SysTick0->ISR &= ~(1<<0);
    printf("delta time:%d\r\n",time0-0x20);
}
#endif

#if Func_Run_V5F
void SysTick1_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void SysTick1_Handler(void)
{
    time1=SysTick1->CNT;
    SysTick1->CTLR=0;
    SysTick0->ISR &= ~(1<<1);
    printf("delta time:%d\r\n",time1-0x20);
}
#endif

/*********************************************************************
 * @fn      Systick0_Init
 *
 * @brief   Initializes Systick0.
 *
 * @return  none
 */
void Systick0_Init(void)
{
    SysTick0->ISR &= ~(1 << 0);
    SysTick0->CNT=0;
    SysTick0->CMP=0x20;
    SysTick0->CTLR=0x7;
}
/*********************************************************************
 * @fn      Systick1_Init
 *
 * @brief   Initializes Systick1.
 *
 * @return  none
 */
void Systick1_Init(void)
{
    SysTick0->ISR &= ~(1 << 1);
    SysTick1->CNT=0;
    SysTick1->CMP=0x20;
    SysTick1->CTLR=0x7;
}
/*********************************************************************
 * @fn      Interrupt_VTF_Init
 *
 * @brief   Initializes VTF.
 *
 * @return  none
 */
void Interrupt_VTF_Init(u32 SysTK,IRQn_Type IRQn,uint8_t num,FunctionalState NewState)
{
    SetVTFIRQ((u32)SysTK,IRQn,0,NewState);
	NVIC_EnableIRQ(IRQn);
}

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   Resets the CRC Data register (DR).
 *
 * @return  none
 */
void Hardware(void)
{

#if defined (Core_V3F)
    Interrupt_VTF_Init((u32)SysTick0_Handler,SysTick0_IRQn,0,ENABLE);
    Systick0_Init();
#endif
#if defined (Core_V5F)
    Interrupt_VTF_Init((u32)SysTick1_Handler,SysTick1_IRQn,1,ENABLE);
    Systick1_Init();
#endif

}
