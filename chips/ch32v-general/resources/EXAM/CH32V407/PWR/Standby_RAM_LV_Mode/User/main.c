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
 *    when LV is enabled in standby mode, RAM 32K low-power data holding routines:
 *    This routine demonstrates writing data at the specified location of 32K RAM,
 *    then WFI enters STANDBY sleep mode and turns on LV,Input high level through PA0 (wakeup) pin
 *    to exit standby mode, print RAM data after waking up, and test whether RAM holds data.
 */
#include "debug.h"

/*********************************************************************
 * @fn      TestDataWrite
 *
 * @brief   Write data to certain address of 32K RAM.
 *
 * @return  none
 */
void TestDataWrite(void)
{
    uint32_t myAddr=0x20000000+4*1024;

    uint32_t i=0;
    for(i=0;i<5120;i++)//Write 32K RAM
    {
        *(uint32_t volatile *)(myAddr+(i<<2)) = 0x5678ABCD;
    }
    for(i=0;i<5120;i++)//Check 32K RAM
    {
        if((*(uint32_t volatile *)(myAddr+(i<<2))) != 0x5678ABCD)
        {
            printf("Write 32K RAM Error!\r\n");
            break;
        }
    }
    printf("Write 32K RAM Successfully!\r\n");
}

/*********************************************************************
 * @fn      TestDataRead
 *
 * @brief   Print the data of certain address of 32K RAM.
 *
 * @return  none
 */
void TestDataRead(void)
{
    uint32_t myAddr=0x20000000+4*1024;
    uint32_t i=0;
   
    printf("32K RAM:\r\n");
    for(i=0;i<5120;i++)
    {
        printf("0x%08x=0x%08x\r\n",myAddr+(i<<2),*(uint32_t volatile *)(myAddr+(i<<2)));
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
    printf("32K RAM low-voltage mode, power in STANDBY\r\n");
    /* Before entering standby mode, all I/O pins not used for wake-up functions 
     shall be configured to analog input mode */
    All_GPIO_AIN_Init();
    Delay_Ms(500);
    PWR_EnterSTANDBYMode_RAM_LV();

    while(1);
}
