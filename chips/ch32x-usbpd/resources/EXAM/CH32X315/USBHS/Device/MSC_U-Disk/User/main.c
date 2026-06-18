/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/01/19
 * Description        : 
 *********************************************************************************
 * Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/* @Note
 * UDisk Example:
 * This program provides examples of UDisk.
 *  */


#include "debug.h"
#include "ch32X315_usbhs_device.h"
#include "SPI_FLASH.h"
#include "SW_UDISK.h"
#include "Internal_Flash.h"

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
    USART_Printf_Init(921600);
    
#if (STORAGE_MEDIUM == MEDIUM_SPI_FLASH)
    printf("USBD UDisk Demo\r\nStorage Medium: SPI FLASH \r\n");
    /* SPI flash init */
    FLASH_Port_Init( );
    /* FLASH ID check */
    FLASH_IC_Check( );
#elif (STORAGE_MEDIUM == MEDIUM_INTERAL_FLASH)
    printf("USBD UDisk Demo\r\nStorage Medium: INTERNAL FLASH \r\n");
    Flash_Sector_Count = IFLASH_UDISK_SIZE  / DEF_UDISK_SECTOR_SIZE;
    Flash_Sector_Size = DEF_UDISK_SECTOR_SIZE;
#endif

    /* Enable Udisk */
    Udisk_Capability = Flash_Sector_Count;
    Udisk_Status |= DEF_UDISK_EN_FLAG;

    USBHS_Device_Init(ENABLE);

    while(1)
    {
        ;
    }

}
