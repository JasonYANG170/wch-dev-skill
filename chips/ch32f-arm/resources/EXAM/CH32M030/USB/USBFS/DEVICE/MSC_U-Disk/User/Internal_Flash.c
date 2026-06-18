/********************************** (C) COPYRIGHT *******************************
 * File Name          : Internal_Flash.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/01/19
 * Description        : Internal Flash program
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32m030.h"
#include "Internal_Flash.h"
#include "ch32m030_flash.h"

/*********************************************************************
 * @fn      IFlash_Prog_512
 *
 * @brief   IFlash Prog 512 Byte
 *
 * @param   Address - specifies the address to be programmed.
 *          Data0 - specifies the data0 to be programmed.
 *
 * @return  none
 */
void IFlash_Prog_512(uint32_t address,uint32_t *pbuf)
{
    if (address < IFLASH_UDISK_START_ADDR || (address + 511) > IFLASH_UDISK_END_ADDR )
    {
        printf("Error Address %x\n",address);
        return;
    }
    address &= 0x00FFFFFF;
    address |= 0x08000000;
    NVIC_DisableIRQ(USBFS_IRQn);
    FLASH_Unlock_Fast();
    FLASH_ROM_ERASE(address,512);
    FLASH_ROM_WRITE(address, pbuf, 512);
    FLASH_Lock_Fast();
    NVIC_EnableIRQ(USBFS_IRQn);
}

