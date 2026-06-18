/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32v4x7_it.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/12/01
* Description        : Main Interrupt Service Routines.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32v4x7_it.h"

void NMI_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void HardFault_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TAMPER_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      NMI_Handler
 *
 * @brief   This function handles NMI exception.
 *
 * @return  none
 */
void NMI_Handler(void)
{
  while (1)
  {
  }
}

/*********************************************************************
 * @fn      HardFault_Handler
 *
 * @brief   This function handles Hard Fault exception.
 *
 * @return  none
 */
void HardFault_Handler(void)
{
  NVIC_SystemReset();
  while (1)
  {
  }
}

/*********************************************************************
 * @fn      TAMPER_IRQHandler
 *
 * @brief   This function handles SysTick Handler.
 *
 * @return  none
 */
void TAMPER_IRQHandler(void)
{
  if( BKP_GetITStatus() == SET )
  {
#if 1
    printf("TAMPER_IRQHandler\r\n");
	  printf( "BKP_DR1:%08x\r\n", BKP->DATAR1 );
	  printf( "BKP_DR2:%08x\r\n", BKP->DATAR2 );
	  printf( "BKP_DR3:%08x\r\n", BKP->DATAR3 );
	  printf( "BKP_DR4:%08x\r\n", BKP->DATAR4 );
	  printf( "BKP_DR5:%08x\r\n", BKP->DATAR5 );
	  printf( "BKP_DR6:%08x\r\n", BKP->DATAR6 );
	  printf( "BKP_DR7:%08x\r\n", BKP->DATAR7 );
	  printf( "BKP_DR8:%08x\r\n", BKP->DATAR8 );
	  printf( "BKP_DR9:%08x\r\n", BKP->DATAR9 );
	  printf( "BKP_DR10:%08x\r\n", BKP->DATAR10);
	  printf( "BKP_DR11:%08x\r\n", BKP->DATAR11);
	  printf( "BKP_DR12:%08x\r\n", BKP->DATAR12);
	  printf( "BKP_DR13:%08x\r\n", BKP->DATAR13);
	  printf( "BKP_DR14:%08x\r\n", BKP->DATAR14);
	  printf( "BKP_DR15:%08x\r\n", BKP->DATAR15);
	  printf( "BKP_DR16:%08x\r\n", BKP->DATAR16);
	  printf( "BKP_DR17:%08x\r\n", BKP->DATAR17);
	  printf( "BKP_DR18:%08x\r\n", BKP->DATAR18);
	  printf( "BKP_DR19:%08x\r\n", BKP->DATAR19);
	  printf( "BKP_DR20:%08x\r\n", BKP->DATAR20);
	  printf( "BKP_DR21:%08x\r\n", BKP->DATAR21);
	  printf( "BKP_DR22:%08x\r\n", BKP->DATAR22);
	  printf( "BKP_DR23:%08x\r\n", BKP->DATAR23);
	  printf( "BKP_DR24:%08x\r\n", BKP->DATAR24); 
	  printf( "BKP_DR25:%08x\r\n", BKP->DATAR25);
	  printf( "BKP_DR26:%08x\r\n", BKP->DATAR26);
	  printf( "BKP_DR27:%08x\r\n", BKP->DATAR27);
	  printf( "BKP_DR28:%08x\r\n", BKP->DATAR28);
	  printf( "BKP_DR29:%08x\r\n", BKP->DATAR29);
	  printf( "BKP_DR30:%08x\r\n", BKP->DATAR30);
	  printf( "BKP_DR31:%08x\r\n", BKP->DATAR31);
	  printf( "BKP_DR32:%08x\r\n", BKP->DATAR32);
	  printf( "BKP_DR33:%08x\r\n", BKP->DATAR33);
	  printf( "BKP_DR34:%08x\r\n", BKP->DATAR34);
	  printf( "BKP_DR35:%08x\r\n", BKP->DATAR35);
	  printf( "BKP_DR36:%08x\r\n", BKP->DATAR36);
	  printf( "BKP_DR37:%08x\r\n", BKP->DATAR37);
	  printf( "BKP_DR38:%08x\r\n", BKP->DATAR38);
	  printf( "BKP_DR39:%08x\r\n", BKP->DATAR39);
	  printf( "BKP_DR40:%08x\r\n", BKP->DATAR40);
	  printf( "BKP_DR41:%08x\r\n", BKP->DATAR41);
	  printf( "BKP_DR42:%08x\r\n", BKP->DATAR42);

#endif
  }

  BKP_ClearITPendingBit();
}
