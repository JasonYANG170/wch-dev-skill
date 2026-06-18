/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH59x_flash.c
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2025/9/17
 * Description
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "CH59x_common.h"

/*********************************************************************
 * @fn      FLASH_PAGE_ERASE
 *
 * @brief   erase page 256B, address must be page aligned (256B)
 *
 * @param   address     - erash address
 *
 * @return  none
 */
__HIGH_CODE
uint8_t FLASH_PAGE_ERASE( uint32_t address )
{
    R16_SLP_WAKE_CFG &= ~RB_ACAUTO_ENABLE;
    sys_safe_access_enable();
    //Open the erase programming channel
    R32_GLOBAL_CONFIG = 0xe1;
    R8_FLASH_CTRL2 = 0;
    // 1. Pre-program the area to be programmed
    // 2. Erase the area to be programmed
    R8_FLASH_CTRL2 |= 3<<4;
    R32_FLASH_ADDR = address;
    R8_PREPG_CTRL = 0;
    R8_PREPG_CTRL = (1<<4);// prepg_start
    while((R32_FLASH_STATUS & (1<<31) ) != 0);
    while((R32_FLASH_STATUS & (1<<19) ) == 0);
    R8_ERASE_CTRL |= (1<<4);
    while((R32_FLASH_STATUS & (1<<31) ) != 0);
    while((R32_FLASH_STATUS & (1<<19) ) == 0);
    R8_ERASE_CTRL = 0;
    R16_SLP_WAKE_CFG |= RB_ACAUTO_ENABLE;
    sys_safe_access_disable();
    //Operate flash once to restore it
    *((PUINT32V)ROM_CFG_ADDR) = 0X00;
    return 0;
}

/*********************************************************************
 * @fn      FLASH_SECTOR_ERASE
 *
 * @brief   erase sector 2K, address must be sector aligned (2K)
 *
 * @param   address     - erash address
 *
 * @return  none
 */
__HIGH_CODE
uint8_t FLASH_SECTOR_ERASE( uint32_t address )
{
    R16_SLP_WAKE_CFG &= ~RB_ACAUTO_ENABLE;
    sys_safe_access_enable();
    //Open the erase programming channel
    R32_GLOBAL_CONFIG = 0xe1;
    R8_FLASH_CTRL2 = 0;
    // 1. Pre-program the area to be programmed
    // 2. Erase the area to be programmed
    R8_FLASH_CTRL2 |= 3<<4;
    R32_FLASH_ADDR = address;
    R8_PREPG_CTRL = (1<<1);
    R8_PREPG_CTRL |= (1<<4);// prepg_start
    while((R32_FLASH_STATUS & (1<<31) ) != 0);
    while((R32_FLASH_STATUS & (1<<19) ) == 0);
    R8_ERASE_CTRL |= (1<<0)|(1<<4);
    while((R32_FLASH_STATUS & (1<<31) ) != 0);
    while((R32_FLASH_STATUS & (1<<19) ) == 0);
    R8_ERASE_CTRL = 0;
    R16_SLP_WAKE_CFG |= RB_ACAUTO_ENABLE;
    sys_safe_access_disable();
    //Operate flash once to restore it
    *((PUINT32V)ROM_CFG_ADDR) = 0X00;
    return 0;
}

/*********************************************************************
 * @fn      FLASH_BANK_ERASE
 *
 * @brief   erase bank 64K, address must be bank aligned (64K)
 *
 * @param   address     - erash address
 *
 * @return  none
 */
__HIGH_CODE
uint8_t FLASH_BANK_ERASE( uint32_t address )
{
    R16_SLP_WAKE_CFG &= ~RB_ACAUTO_ENABLE;
    sys_safe_access_enable();
    //Open the erase programming channel
    R32_GLOBAL_CONFIG = 0xe1;
    R8_FLASH_CTRL2 = 0;
    // 1. Pre-program the area to be programmed
    // 2. Erase the area to be programmed
    R8_FLASH_CTRL2 |= 3<<4;
    R32_FLASH_ADDR = address;
    R8_PREPG_CTRL = (2<<1);
    R8_PREPG_CTRL |= (1<<4);// prepg_start
    while((R32_FLASH_STATUS & (1<<31) ) != 0);
    while((R32_FLASH_STATUS & (1<<19) ) == 0);
    R8_ERASE_CTRL |= (1<<1)|(1<<4);
    while((R32_FLASH_STATUS & (1<<31) ) != 0);
    while((R32_FLASH_STATUS & (1<<19) ) == 0);
    R8_ERASE_CTRL = 0;
    R16_SLP_WAKE_CFG |= RB_ACAUTO_ENABLE;
    sys_safe_access_disable();
    //Operate flash once to restore it
    *((PUINT32V)ROM_CFG_ADDR) = 0X00;
    return 0;
}

/*********************************************************************
 * @fn      FLASH_ERASE
 *
 * @brief   FLASH erase
 *
 * @param   address     - erash address
 * @param   len         - write len
 *
 * @return  none
 */
uint8_t FLASH_ERASE(uint32_t address, uint32_t len)
{
    uint8_t erase_sta;
    uint32_t current_addr = address;
    uint32_t end_addr = address + len;

    // 长度为0不执行擦除
    if (len == 0) return 1;

    // 提升擦除效率
    while (current_addr < end_addr)
    {
        // 检查是否可以执行64KB擦除
        if ((end_addr - current_addr >= FLASH_BANK_SIZE) && (current_addr % FLASH_BANK_SIZE == 0))
        {
            erase_sta = FLASH_BANK_ERASE(current_addr);
            if (erase_sta) return 2;
            current_addr += FLASH_BANK_SIZE;
            continue;
        }

        // 检查是否可以执行2KB擦除
        if ((end_addr - current_addr >= FLASH_SECTOR_SIZE) && (current_addr % FLASH_SECTOR_SIZE == 0))
        {
            erase_sta = FLASH_SECTOR_ERASE(current_addr);
            if (erase_sta) return 2;
            current_addr += FLASH_SECTOR_SIZE;
            continue;
        }

        // 最后使用256字节页擦除
        if (current_addr % FLASH_PAGE_SIZE == 0)
        {
            erase_sta = FLASH_PAGE_ERASE(current_addr);
            if (erase_sta) return 2;
            current_addr += FLASH_PAGE_SIZE;
        }
        else
        {
            uint32_t aligned_addr = (current_addr / FLASH_PAGE_SIZE) * FLASH_PAGE_SIZE;
            erase_sta = FLASH_PAGE_ERASE(aligned_addr);
            if (erase_sta) return 2;
            current_addr = aligned_addr + FLASH_PAGE_SIZE;
        }
    }

    return 0;
}

/*********************************************************************
 * @fn      FLASH_WRITE
 *
 * @brief   Flash program, address must be page aligned (256B)
 *
 * @param   address     - write address
 * @param   pbuf        - write buffer
 * @param   len         - write len
 *
 * @return  none
 */
__HIGH_CODE
uint8_t FLASH_WRITE( uint32_t address, void *pbuf, uint32_t len )
{
    uint32_t i;
    uint8_t *pdata;
    uint32_t val;
    uint32_t w_len=0;
    uint32_t p_len=0;
    R16_SLP_WAKE_CFG &= ~RB_ACAUTO_ENABLE;
    sys_safe_access_enable();
    R32_GLOBAL_CONFIG = 0xe1;
    R8_FLASH_CTRL2 = 0;
    R8_FLASH_CTRL2 |= 3<<4;
    while(len)
    {
        //3. Clear the page latch.
        R8_BUF_CTRL = 1<<0;
        while((R32_FLASH_STATUS & (1<<31) ) != 0);
        while((R32_FLASH_STATUS & (1<<19) ) == 0);
        //4. Write the data that needs to be programmed into the page latch.
        R8_FLASH_CTRL2 |= (1<<2);
        R8_BUF_CTRL = (1<<5);
        R32_FLASH_ADDR = address+w_len;
        pdata = pbuf+w_len;
        p_len = (len>256)?256:len;
        for( i=0; i<p_len; i+=4 )
        {
            val = *pdata++;
            val |= (*pdata++)<<8;
            val |= (*pdata++)<<16;
            val |= (*pdata++)<<24;
            while((R32_FLASH_STATUS & (1<<31) ) != 0);
            while((R32_FLASH_STATUS & (1<<19) ) == 0);
            R32_FLASH_DATA = val;
        }
        while((R32_FLASH_STATUS & (1<<31) ) != 0);
        while((R32_FLASH_STATUS & (1<<19) ) == 0);
        //5. Program the contents of the page latch into the page at the specified address.
        R32_FLASH_ADDR = address+w_len;
        R8_PROGRAM_CTRL = (1<<0);
        len -= p_len;
        w_len += p_len;
        while((R32_FLASH_STATUS & (1<<31) ) != 0);
        while((R32_FLASH_STATUS & (1<<19) ) == 0);
    }
    R16_SLP_WAKE_CFG |= RB_ACAUTO_ENABLE;
    sys_safe_access_disable();
    //Operate flash once to restore it
    *((PUINT32V)ROM_CFG_ADDR) = 0X00;
    return 0;
}

/*********************************************************************
 * @fn      FLASH_READ
 *
 * @brief   Read falsh
 *
 * @param   address     - read address
 * @param   pbuf        - read buffer
 * @param   len         - read len
 *
 * @return  none
 */
uint8_t FLASH_READ( uint32_t address, void *pbuf, uint32_t len )
{
    uint32_t i;
    uint8_t *pdata;
    pdata = pbuf;
    for(i = 0; i < len; i++)
    {
        pdata[i] = ((uint8_t *)address)[i];
    }
    return 0;
}
/*********************************************************************
 * @fn      GET_UNIQUE_ID
 *
 * @brief   get 64 bit unique ID
 *
 * @param   Buffer      - Pointer to the buffer where data should be stored, Must be aligned to 4 bytes.
 *
 * @return  0-SUCCESS  (!0)-FAILURE
 */
void GET_UNIQUE_ID(uint8_t *Buffer)
{
    uint16_t temp;
    FLASH_READ( ROM_CFG_MAC_ADDR, Buffer, 8 );
    temp = (Buffer[0]|(Buffer[1]<<8)) + (Buffer[2]|(Buffer[3]<<8)) + (Buffer[4]|(Buffer[5]<<8));
    Buffer[6] = temp&0xFF;
    Buffer[7] = (temp>>8)&0xFF;
}
