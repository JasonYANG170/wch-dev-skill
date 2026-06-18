/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 * @Note
 * FLASH erase/read/write, and fast programming:
 * Includes Standard Erase and Program, Fast Erase and Program.
 * Note ：It is recommended to enable continuous read when programming/erasing/read FLASH (FLASH->ACTLR bit11).
*/

#include "debug.h"

/* Global define */
typedef enum
{
    FAILED = 0,
    PASSED = !FAILED
} TestStatus;
#define PAGE_WRITE_START_ADDR            ((uint32_t)0x0800F000) /* Start from 60K */
#define PAGE_WRITE_END_ADDR              ((uint32_t)0x08010000) /* End at 63K */
#define FLASH_PAGE_SIZE                  4096
#define FLASH_PAGES_TO_BE_PROTECTED      FLASH_WRProt_Pages60to63

/* Fast Mode define */
#define FAST_FLASH_PROGRAM_START_ADDR    ((uint32_t)0x08010000)
#define FAST_FLASH_PROGRAM_END_ADDR      ((uint32_t)0x08020000)
#define FAST_FLASH_SIZE                  (64 * 1024)

/* Global Variable */
uint32_t              EraseCounter = 0x0, Address = 0x0;
uint16_t              Data = 0xAAAA;
uint32_t              WRPR_Value = 0xFFFFFFFF, ProtectedPages = 0x0;
uint32_t              NbrOfPage;
volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
volatile TestStatus MemoryProgramStatus = PASSED;
volatile TestStatus MemoryEraseStatus = PASSED;

#define Fadr    (0x08020000)
#define Fsize   ((((256*4))))
u32 buf[Fsize];

/*********************************************************************
 * @fn      Flash_Test
 *
 * @brief   Flash Program Test.
 *
 * @return  none
 */
void Flash_Test(void)
{
    printf("FLASH Test\n");

    FLASH_Unlock();

    NbrOfPage = (PAGE_WRITE_END_ADDR - PAGE_WRITE_START_ADDR) / FLASH_PAGE_SIZE;

    FLASH_ClearFlag(FLASH_FLAG_BSY | FLASH_FLAG_EOP | FLASH_FLAG_WRPRTERR);

    for(EraseCounter = 0; (EraseCounter < NbrOfPage) && (FLASHStatus == FLASH_COMPLETE); EraseCounter++)
    {
        FLASHStatus = FLASH_ErasePage(PAGE_WRITE_START_ADDR + (FLASH_PAGE_SIZE * EraseCounter)); //Erase 4KB

        if(FLASHStatus != FLASH_COMPLETE)
        {
            printf("FLASH Erase Fail\r\n");
            return;
        }
        printf("FLASH Erase Suc\r\n");
    }

    Address = PAGE_WRITE_START_ADDR;
    printf("Programing...\r\n");
    while((Address < PAGE_WRITE_END_ADDR) && (FLASHStatus == FLASH_COMPLETE))
    {
        FLASHStatus = FLASH_ProgramHalfWord(Address, Data);
        Address = Address + 2;
    }

    Address = PAGE_WRITE_START_ADDR;

    printf("Program Cheking...\r\n");
    while((Address < PAGE_WRITE_END_ADDR) && (MemoryProgramStatus != FAILED))
    {
        if((*(__IO uint16_t *)Address) != Data)
        {
            MemoryProgramStatus = FAILED;
        }
        Address += 2;
    }

    if(MemoryProgramStatus == FAILED)
    {
        printf("Memory Program FAIL!\r\n");
    }
    else
    {
        printf("Memory Program PASS!\r\n");
    }

    FLASH_Lock();

}

/*********************************************************************
 * @fn      Flash_Test_Fast
 *
 * @brief   Flash Fast Program Test.
 *
 * @return  none
 */
void Flash_Test_Fast(void)
{
    u32 i;
    u8 Verify_Flag = 0;
    FLASH_Status s;

    for(i = 0; i < Fsize; i++){
        buf[i] = i;
    }

    printf("Read flash\r\n");
    for(i=0; i<Fsize; i++){
        printf("adr-%08x v-%08x\r\n", Fadr +4*i, *(u32*)(Fadr +4*i));
    }

    s = FLASH_ROM_ERASE(Fadr, Fsize*4);
    if(s != FLASH_COMPLETE)
    {
        printf("check FLASH_ADR_RANGE_ERROR FLASH_ALIGN_ERROR or FLASH_OP_RANGE_ERROR\r\n");
        return;
    }

    printf("Erase flash\r\n");
    for(i=0; i<Fsize; i++){
        printf("adr-%08x v-%08x\r\n", Fadr +4*i, *(u32*)(Fadr +4*i));
    }

    s = FLASH_ROM_WRITE(Fadr,  buf, Fsize*4);
    if(s != FLASH_COMPLETE)
    {
        printf("check FLASH_ADR_RANGE_ERROR FLASH_ALIGN_ERROR or FLASH_OP_RANGE_ERROR\r\n");
        return;
    }

    printf("Write flash\r\n");
    for(i=0; i<Fsize; i++){
        printf("adr-%08x v-%08x\r\n", Fadr +4*i, *(u32*)(Fadr +4*i));
    }

    for(i = 0; i < Fsize; i++){
        if(buf[i] == *(u32 *)(Fadr + 4 * i))
        {
            Verify_Flag = 0;
        }
        else
        {
            Verify_Flag = 1;
            break;
        }
    }

    if(Verify_Flag)
        printf("%d Byte Verify Fail\r\n", (Fsize*4));
    else
        printf("%d Byte Verify Suc\r\n", (Fsize*4));
}
#define addr  (0x08019000)
uint32_t pbuf[64] = {0};
void FLASH_RW(void)
{
    FLASH_Unlock_Fast();
    FLASH_ErasePage(0x08019000);
    for(int i = 0;i<64;i++)
    {
        pbuf[i] = i;
    }
    FLASH_ProgramPage_Fast(addr,pbuf);
    FLASH_ProgramPage_Fast(addr+256,pbuf);
    FLASH_ProgramPage_Fast(addr+256*2,pbuf);
    for(int j = 0;j<64*3;j++)
    {
        printf("%08x - %08x \r\n",(vu32)(addr+4*j),*(vu32*)(addr+4*j));
    }
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
    USART_Printf_Init(921600);
    printf("SystemClk:%d\r\n", SystemClock);
    printf("SystemCoreClk:%d\r\n", SystemCoreClock);
    printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );
    printf("Flash Program Test\r\n");
    printf("FLASH->CTLR - %08x\r\n",FLASH->CTLR);

    Flash_Test();
    Flash_Test_Fast();

    while(1)
        ;
}
