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
 * CD-ROM Example:
 * 1.This program provides examples of CD-ROM.The iso file TEST.iso used for testing is 
 * located in the TEST_ISO folder. 
 * 2.User needs to place the ISO at address 0 of the external Flash.The ISO file is in 
 * ISO 9660 Joliet Level 1 format and is suitable for a sector size of 512B. If other 
 * file systems are used, the sector size DEF_CFG_DISK_SEC_SIZE in the program needs to 
 * be adjusted.
 * 3.This Code Only Supports Winboard W25 Series flash, if user needs to use other type
 * of flash, you can add flash type by yourself in SPI_FLASH.c in user floder.
 * 4.External flash chip needs to be attached to SPI_2.
 * 5.CD_ROM iso file 'TEST.iso' contains wch serial-port driver, and autorun.inf.
 *  */


#include "debug.h"
#include "ch32x315_usbhs_device.h"
#include "SPI_FLASH.h"
#include "SW_CDROM.h"
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

    printf("USBD CD-ROM Demo\r\n");
    /* SPI flash init */
    FLASH_Port_Init( );
    /* FLASH ID check */
    FLASH_IC_Check( );

    /* Enable Udisk */
    CDROM_Capability = Flash_Sector_Count;
    CDROM_Status |= DEF_CDROM_EN_FLAG;

    USBHS_Device_Init(ENABLE);

    while(1)
    {
        ;
    }

}
