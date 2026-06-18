/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/02/08
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
/*
 *@Note
 * In STANDBY mode, 32K bytes RAM data holding routines:
 * Address range of  32K bytes RAM data holding: 0x20000000 - 0x20000000+32K .
 * This example demonstrates the RAM data holding in standby mode (chip VDD power supply or VBAT power
 * supply,and RAM work at different voltages). Write data to 32K RAM respectively, then
 * execute WFI to enter STANDBY mode, exit STANDBY mode by inputting a high level to the PA0 (wake-up)
 * pin, print RAM data after waking up, and test whether RAM retains data.
 *
 */

#include "debug.h"

#define KEEP_32kRAM  __attribute__((section(".keep_ram")))/* Address space for 32K bytes of RAM. */

/* Power supply mode definition */
#define  VDD_POWER_MODE  0
#define  VBAT_POWER_MODE 1

#define  POWER_MODE  VDD_POWER_MODE

/* RAM low voltage working mode definition */
#define  RAMLV 1

/* Global Variable */
KEEP_32kRAM   u32 DataBuf[512];

/*********************************************************************
 * @fn      TestDataWrite
 *
 * @brief   Write data in 32K RAM.
 *
 * @return  none
 */
void TestDataWrite(void)
{
    uint32_t i=0;
    /* Write data in 32K RAM */
    for( i=0; i<512; i++ )
    {
       DataBuf[i] = 0x5678ABCD;
    }
    printf("Write data in 32K RAM Successfully!\r\n");
}

/*********************************************************************
 * @fn      TestDataRead
 *
 * @brief   Read data written in 32K RAM.
 *
 * @return  none
 */
void TestDataRead(void)
{
    uint32_t i=0;
    printf("Read data in 32K RAM:\r\n");
    for( i=0; i<512; i++ )
    {
         printf("DataBuf#%d = 0x%08x\r\n", i, DataBuf[i]);  
    }
}

/*********************************************************************
 * @fn      All_GPIO_AIN_Init
 *
 * @brief   Before entering standby mode, all I/O pins not used for wake-up functions 
 *          shall be configured to analog input mode.
 * @return  none
 */
void All_GPIO_AIN_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure={0};
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA | RCC_PB2Periph_GPIOB | RCC_PB2Periph_GPIOC |
                          RCC_PB2Periph_GPIOD | RCC_PB2Periph_GPIOE | RCC_PB2Periph_AFIO, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
    GPIO_Init( GPIOA, &GPIO_InitStructure );
    GPIO_Init( GPIOB, &GPIO_InitStructure );
    GPIO_Init( GPIOC, &GPIO_InitStructure );
    GPIO_Init( GPIOD, &GPIO_InitStructure );
    GPIO_Init( GPIOE, &GPIO_InitStructure );
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
    Delay_Ms(500);
    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    TestDataRead();
    TestDataWrite();
    RCC_PB1PeriphClockCmd(RCC_PB1Periph_PWR, ENABLE);
    PWR_WakeUpPinCmd(ENABLE);
    printf("\r\n***Start standby RAM data hold.. \r\n");
    /* Before entering standby mode, all I/O pins not used for wake-up functions 
     shall be configured to analog input mode */
    All_GPIO_AIN_Init();
    Delay_Ms(500);
#if(POWER_MODE == VDD_POWER_MODE)
    #if RAMLV
        PWR_EnterSTANDBYMode_RAM_LV();
    #else
        PWR_EnterSTANDBYMode_RAM();
    #endif
#elif(POWER_MODE == VBAT_POWER_MODE)
    #if RAMLV
        PWR_EnterSTANDBYMode_RAM_LV_VBAT_EN();
    #else
        PWR_EnterSTANDBYMode_RAM_VBAT_EN();
    #endif
#endif
    while(1);
}

