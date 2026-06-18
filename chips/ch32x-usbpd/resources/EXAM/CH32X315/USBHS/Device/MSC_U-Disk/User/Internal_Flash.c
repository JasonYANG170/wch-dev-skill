/********************************** (C) COPYRIGHT *******************************
 * File Name          : Internal_Flash.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/01/19
 * Description        : Internal Flash program
*********************************************************************************
* Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32X3X5.h"
#include "Internal_Flash.h"
#include "ch32x3x5_flash.h"

/*********************************************************************
 * @fn      FLASH_BufLoad
 *
 * @brief   Flash Buffer load(4Byte).
 *
 * @param   Address - specifies the address to be programmed.
 *          Data0 - specifies the data0 to be programmed.
 *
 * @return  none
 */
void IFlash_Prog_4K(uint32_t address, uint32_t *pbuf)
{
    if(address < IFLASH_UDISK_START_ADDR || (address + 4095) > IFLASH_UDISK_END_ADDR)
    {
        printf("Error Address %x\n", address);
        return;
    }
    address &= 0x00FFFFFF;
    address |= 0x08000000;

    FLASH_ROM_ERASE(address, 4096);
    FLASH_ROM_WRITE(address, pbuf, 4096);
}
