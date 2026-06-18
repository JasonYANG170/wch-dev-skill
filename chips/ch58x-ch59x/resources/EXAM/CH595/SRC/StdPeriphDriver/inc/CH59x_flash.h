/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH59x_flash.h
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef __CH59x_FLASH_H__
#define __CH59x_FLASH_H__

#ifdef __cplusplus
extern "C" {
#endif

#define FLASH_PAGE_SIZE     0x100
#define FLASH_SECTOR_SIZE   0x800
#define FLASH_BANK_SIZE     0x10000

#ifndef FLASH_MAX_SIZE
#define FLASH_MAX_SIZE      0x3C000                   // Flash maximum program size, 240KB
#endif

#define R8_PREPG_CTRL     (*((PUINT8V)0x40001800))
#define R8_BUF_CTRL     (*((PUINT8V)0x40001801))
#define R8_ERASE_CTRL     (*((PUINT8V)0x40001802))
#define R8_PROGRAM_CTRL     (*((PUINT8V)0x40001803))
#define R8_FLASH_RD_CTRL     (*((PUINT8V)0x40001804))
#define R8_FLASH_LP_CTRL     (*((PUINT8V)0x40001805))
#define R8_PREFETCH_CTRL     (*((PUINT8V)0x40001806))
#define R8_FLASH_CTRL2     (*((PUINT8V)0x40001807))
#define R8_RDPRCTDIS_CTRL     (*((PUINT8V)0x40001808))
#define R8_CHIP_STATUS1     (*((PUINT8V)0x40001809))

#define ROM_CFG_MAC_ADDR    0x40218         // address for MAC address information
#define ROM_CFG_CHIP_ID     0x40224

/**
 * @brief   get 6 bytes MAC address
 *
 * @param   Buffer      - Pointer to the buffer where data should be stored, Must in RAM and be aligned to 4 bytes.
 *
 * @return  0-SUCCESS  (!0)-FAILURE
 */
#define GetMACAddress(Buffer)      FLASH_READ( ROM_CFG_MAC_ADDR, Buffer, 6 )
uint8_t FLASH_PAGE_ERASE( uint32_t address );
uint8_t FLASH_SECTOR_ERASE( uint32_t address );
uint8_t FLASH_BANK_ERASE( uint32_t address );
uint8_t FLASH_ERASE( uint32_t address , uint32_t len );
uint8_t FLASH_WRITE( uint32_t address, void *pbuf, uint32_t len );
uint8_t FLASH_READ( uint32_t address, void *pbuf, uint32_t len );

UINT8 UserOptionByteConfig(FunctionalState RESET_EN, FunctionalState BOOT_PIN, FunctionalState UART_NO_KEY_EN,
        uint32_t FLASHProt_Size);

UINT8 UserOptionByteClose_SWD(void);

void UserOptionByte_Active(void);

void GET_UNIQUE_ID(uint8_t *Buffer);
#ifdef __cplusplus
}
#endif

#endif // __CH59x_FLASH_H__
