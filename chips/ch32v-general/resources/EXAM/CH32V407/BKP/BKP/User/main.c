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
 *BKP routine:
 *Intrusion detection pin (PC13), when an intrusion detection event occurs, all data backup register contents are cleared,
 *And trigger the intrusion detection interrupt.
 *
 */

#include "debug.h"

/*********************************************************************
 * @fn      BKP_Tamper_Init
 *
 * @brief   Initializes the BKP Tamper.
 *
 * @return  none
 */
void BKP_Tamper_Init(void)
{
	RCC_PB1PeriphClockCmd( RCC_PB1Periph_PWR | RCC_PB1Periph_BKP, ENABLE );

	BKP_TamperPinCmd( DISABLE );
    PWR_BackupAccessCmd( ENABLE );
    BKP_ClearFlag();

	BKP_WriteBackupRegister( BKP_DR1, 0xF1F1 );
	BKP_WriteBackupRegister( BKP_DR2, 0x2F2F );
	BKP_WriteBackupRegister( BKP_DR3, 0xABCD );
	BKP_WriteBackupRegister( BKP_DR4, 0x3456 );
	BKP_WriteBackupRegister( BKP_DR5, 0x9880 );
	BKP_WriteBackupRegister( BKP_DR6, 0x5678 );
	BKP_WriteBackupRegister( BKP_DR7, 0xABCD );
	BKP_WriteBackupRegister( BKP_DR8, 0x3456 );
	BKP_WriteBackupRegister( BKP_DR9, 0x9880 );
	BKP_WriteBackupRegister( BKP_DR10, 0x5678 );
	BKP_WriteBackupRegister( BKP_DR11, 0xABCD );
	BKP_WriteBackupRegister( BKP_DR12, 0x3456 );
	BKP_WriteBackupRegister( BKP_DR13, 0x9880 );
	BKP_WriteBackupRegister( BKP_DR14, 0x5678 );
	BKP_WriteBackupRegister( BKP_DR15, 0xABCD );
	BKP_WriteBackupRegister( BKP_DR16, 0x3456 );  
	BKP_WriteBackupRegister( BKP_DR17, 0xF1F1 );
	BKP_WriteBackupRegister( BKP_DR18, 0x2F2F );
	BKP_WriteBackupRegister( BKP_DR19, 0xABCD );
	BKP_WriteBackupRegister( BKP_DR20, 0x3456 );
	BKP_WriteBackupRegister( BKP_DR21, 0x9880 );
	BKP_WriteBackupRegister( BKP_DR22, 0x5678 );
	BKP_WriteBackupRegister( BKP_DR23, 0xABCD );
	BKP_WriteBackupRegister( BKP_DR24, 0x3456 );
	BKP_WriteBackupRegister( BKP_DR25, 0x9880 );
	BKP_WriteBackupRegister( BKP_DR26, 0x5678 );
	BKP_WriteBackupRegister( BKP_DR27, 0xABCD );
	BKP_WriteBackupRegister( BKP_DR28, 0x3456 );
	BKP_WriteBackupRegister( BKP_DR29, 0x9880 );
	BKP_WriteBackupRegister( BKP_DR30, 0x5678 );
	BKP_WriteBackupRegister( BKP_DR31, 0xABCD );
	BKP_WriteBackupRegister( BKP_DR32, 0x3456 );
	BKP_WriteBackupRegister( BKP_DR33, 0xF1F1 );
	BKP_WriteBackupRegister( BKP_DR34, 0x2F2F );
	BKP_WriteBackupRegister( BKP_DR35, 0xABCD );
	BKP_WriteBackupRegister( BKP_DR36, 0x3456 );
	BKP_WriteBackupRegister( BKP_DR37, 0x9880 );
	BKP_WriteBackupRegister( BKP_DR38, 0x5678 );
	BKP_WriteBackupRegister( BKP_DR39, 0xABCD );
	BKP_WriteBackupRegister( BKP_DR40, 0x3456 );
	BKP_WriteBackupRegister( BKP_DR41, 0x9880 );
	BKP_WriteBackupRegister( BKP_DR42, 0x5678 );                    

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
    
	BKP_TamperPinLevelConfig( BKP_TamperPinLevel_High );  //TPAL:0-PC13 set input-pull-down
//	BKP_TamperPinLevelConfig( BKP_TamperPinLevel_Low );	 //TPAL:1-PC13 input-pull-up

    NVIC_SetPriority(TAMPER_IRQn,0x0);
    NVIC_EnableIRQ(TAMPER_IRQn);

	BKP_ITConfig( ENABLE );
	BKP_TamperPinCmd( ENABLE );
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

    USART_Printf_Init(115200);	

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

    SystemCoreClockUpdate();
    Delay_Init();
    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID());
	BKP_Tamper_Init();

	while(1);
}


