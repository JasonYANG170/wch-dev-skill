/********************************** (C) COPYRIGHT  *******************************
 * File Name          : ch32m030_flash.h
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/04/16
 * Description        : This file contains all the functions prototypes for the FLASH
 *                      firmware library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32M030_FLASH_H
#define __CH32M030_FLASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32m030.h"

/* FLASH Status */
typedef enum
{
    FLASH_BUSY = 1,
    FLASH_ERROR_PG,
    FLASH_ERROR_WRP,
    FLASH_COMPLETE,
    FLASH_TIMEOUT,
    FLASH_RDP,
    FLASH_OP_RANGE_ERROR = 0xFD,
    FLASH_ALIGN_ERROR = 0xFE,
    FLASH_ADR_RANGE_ERROR = 0xFF,
} FLASH_Status;

/* Flash_Latency */
#define FLASH_Latency_0                  ((uint32_t)0x00000000) /* FLASH 0 Latency cycle */
#define FLASH_Latency_1                  ((uint32_t)0x00000001) /* FLASH 1 Latency cycle */
#define FLASH_Latency_2                  ((uint32_t)0x00000002) /* FLASH 2 Latency cycles */
#define FLASH_Latency_3                  ((uint32_t)0x00000003) /* FLASH 3 Latency cycles */

/* Values to be used with CH32M030 devices (1page = 1KB)*/
#define FLASH_WRProt_Pages0to3           ((uint32_t)0x00000001) /*  Write protection of page 0 to 3 */
#define FLASH_WRProt_Pages4to7           ((uint32_t)0x00000002) /*  Write protection of page 4 to 7 */
#define FLASH_WRProt_Pages8to11          ((uint32_t)0x00000004) /*  Write protection of page 8 to 11 */
#define FLASH_WRProt_Pages12to15         ((uint32_t)0x00000008) /*  Write protection of page 12 to 15 */
#define FLASH_WRProt_Pages16to19         ((uint32_t)0x00000010) /*  Write protection of page 16 to 19 */
#define FLASH_WRProt_Pages20to23         ((uint32_t)0x00000020) /*  Write protection of page 20 to 23 */
#define FLASH_WRProt_Pages24to27         ((uint32_t)0x00000040) /*  Write protection of page 24 to 27 */
#define FLASH_WRProt_Pages28to31         ((uint32_t)0x00000080) /*  Write protection of page 28 to 31 */
#define FLASH_WRProt_Pages32to35         ((uint32_t)0x00000100) /*  Write protection of page 32 to 35 */
#define FLASH_WRProt_Pages36to39         ((uint32_t)0x00000200) /*  Write protection of page 36 to 39 */
#define FLASH_WRProt_Pages40to43         ((uint32_t)0x00000400) /*  Write protection of page 40 to 43 */
#define FLASH_WRProt_Pages44to47         ((uint32_t)0x00000800) /*  Write protection of page 44 to 47 */
#define FLASH_WRProt_Pages48to51         ((uint32_t)0x00001000) /*  Write protection of page 48 to 51 */
#define FLASH_WRProt_Pages52to55         ((uint32_t)0x00002000) /*  Write protection of page 52 to 55 */
#define FLASH_WRProt_Pages56to59         ((uint32_t)0x00004000) /*  Write protection of page 56 to 59 */
#define FLASH_WRProt_Pages60to63         ((uint32_t)0x00008000) /*  Write protection of page 60 to 63 */

#define FLASH_WRProt_AllPages            ((uint32_t)0xFFFFFFFF) /* Write protection of all Pages */

/* Option_Bytes_nRST_STOP */
#define OB_STOP_NoRST                    ((uint16_t)0x0002) /* No reset generated when entering in STOP */
#define OB_STOP_RST                      ((uint16_t)0x0000) /* Reset generated when entering in STOP */

/* Option_Bytes_nRST_STDBY  */
#define OB_STDBY_NoRST                   ((uint16_t)0x0004) /* No reset generated when entering in STANDBY */
#define OB_STDBY_RST                     ((uint16_t)0x0000) /* Reset generated when entering in STANDBY */

/* Option_Bytes_RST_ENandDT */
#define OB_RST_NoEN                      ((uint16_t)0x0018) /* Reset IO disable */
#define OB_RST_EN_DT1ms                  ((uint16_t)0x0010) /* Reset IO enable and  Ignore delay time 1ms */
#define OB_RST_EN_DT512us                ((uint16_t)0x0008) /* Reset IO enable and  Ignore delay time 512us */
#define OB_RST_EN_DT128us                ((uint16_t)0x0000) /* Reset IO enable and  Ignore delay time 128us */

/* Option_Bytes_RST_PIN */
#define OB_RST_PIN_PA15                  ((uint16_t)0x0020) /* PA15 of reset pin */
#define OB_RST_PIN_PC0                   ((uint16_t)0x0000) /* PC0 of reset pin */

/* Option_Bytes_OV_CFG */
#define OB_OV_CFG_NoEN                   ((uint16_t)0x0080) /* No enable over voltage protection*/
#define OB_OV_CFG_EN                     ((uint16_t)0x0000) /* Enable over voltage protection*/

/* FLASH_Interrupts */
#define FLASH_IT_ERROR                   ((uint32_t)0x00000400) /* FPEC error interrupt source */
#define FLASH_IT_EOP                     ((uint32_t)0x00001000) /* End of FLASH Operation Interrupt source */

/* FLASH_Flags */
#define FLASH_FLAG_BSY                   ((uint32_t)0x00000001) /* FLASH Busy flag */
#define FLASH_FLAG_EOP                   ((uint32_t)0x00000020) /* FLASH End of Operation flag */
#define FLASH_FLAG_WRPRTERR              ((uint32_t)0x00000010) /* FLASH Write protected error flag */
#define FLASH_FLAG_OPTERR                ((uint32_t)0x80000001) /* FLASH Option Byte error flag */

#define FLASH_FLAG_BANK1_BSY             FLASH_FLAG_BSY       /* FLASH BANK1 Busy flag*/
#define FLASH_FLAG_BANK1_EOP             FLASH_FLAG_EOP       /* FLASH BANK1 End of Operation flag */
#define FLASH_FLAG_BANK1_WRPRTERR        FLASH_FLAG_WRPRTERR  /* FLASH BANK1 Write protected error flag */

/* ECC_Flags */
#define FLASH_FLAG_ECC_CKEFail_RECFail   ((uint32_t)0x00020000) /* ECC error check fail and error recovery fail */
#define FLASH_FLAG_ECC_CKEFail_RECSuc    ((uint32_t)0x00040000) /* ECC error check fail and error recovery success */
#define FLASH_FLAG_ECC_CKESuc            ((uint32_t)0x00080000) /* ECC error check success */
#define FLASH_FLAG_ECC_CKEFail           ((uint32_t)0x00100000) /* ECC error check fail */

/*Functions used for all CH32M030 devices*/
void         FLASH_SetLatency(uint32_t FLASH_Latency);
void         FLASH_Unlock(void);
void         FLASH_Lock(void);
FLASH_Status FLASH_ErasePage(uint32_t Page_Address);
FLASH_Status FLASH_EraseAllPages(void);
FLASH_Status FLASH_EraseOptionBytes(void);
FLASH_Status FLASH_EnableWriteProtection(uint32_t FLASH_Pages);
FLASH_Status FLASH_EnableReadOutProtection(void);
FLASH_Status FLASH_UserOptionByteConfig(uint16_t OB_IWDG, uint16_t OB_STOP, uint16_t OB_RST, uint16_t OB_RST_PIN, uint16_t OB_OV_CFG);
uint32_t     FLASH_GetUserOptionByte(void);
uint32_t     FLASH_GetWriteProtectionOptionByte(void);
FlagStatus   FLASH_GetReadOutProtectionStatus(void);
void         FLASH_ITConfig(uint32_t FLASH_IT, FunctionalState NewState);
FlagStatus   FLASH_GetFlagStatus(uint32_t FLASH_FLAG);
void         FLASH_ClearFlag(uint32_t FLASH_FLAG);
FLASH_Status FLASH_GetStatus(void);
FLASH_Status FLASH_WaitForLastOperation(uint32_t Timeout);
void         FLASH_Unlock_Fast(void);
void         FLASH_Lock_Fast(void);
void         FLASH_BufReset(void);
void         FLASH_BufLoad(uint32_t Address, uint32_t Data0, uint32_t Data1);
void         FLASH_ErasePage_Fast(uint32_t Page_Address);
void         FLASH_ProgramPage_Fast(uint32_t Page_Address);

/* New function used for all CH32M030 devices */
void         FLASH_UnlockBank1(void);
void         FLASH_LockBank1(void);
FLASH_Status FLASH_EraseAllBank1Pages(void);
FLASH_Status FLASH_GetBank1Status(void);
FLASH_Status FLASH_WaitForLastBank1Operation(uint32_t Timeout);
FLASH_Status FLASH_ROM_ERASE(uint32_t StartAddr, uint32_t Length);
FLASH_Status FLASH_ROM_WRITE(uint32_t StartAddr, uint32_t *pbuf, uint32_t Length);
void         FLASH_ECCERR_ITCmd(FunctionalState NewState);
void         FLASH_ECCERR_Recovery_SetErrFlagCmd(FunctionalState NewState);
FlagStatus   FLASH_GetECCFlagStatus(uint32_t FLASH_ECC_FLAG);
void         FLASH_ClearECCFlag(uint32_t FLASH_ECC_FLAG);

#ifdef __cplusplus
}
#endif

#endif /* __CH32M030_FLASH_H */
