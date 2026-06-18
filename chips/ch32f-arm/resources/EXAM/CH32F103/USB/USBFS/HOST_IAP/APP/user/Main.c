/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2022/08/20
 * Description        : Main program body.
 *********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
 * U disk file system routine:
 *USBFSM(PB6)\USBFSP(PB7).
 *This routine demonstrates U disk file system, reading and writing files in bytes, including file creation, 
 *deletion and modification.Property to modify the file name.
 * Note:File system support FAT12/FAT16/FAT32.
  ***Note***
 *The difference between this code and EXAM1: The matching APP code has modified."Options for target"-"Target"-
 *"Read/Only Memory Areas"-"on-chip"IROM1 of the region, change Start be"0x805000",
 * size is 0xB000,The rest remains unchanged. To be used as a companion APP for IAP. The generated 
 *  Hex file needs to be converted into a bin file,You can use the HexToBin.exe tool under the HOST_IAP 
 *  folder and modify the file name to APP.bin.
  ***Back to IAP***
 * 1. Global define, Macro Definitions/APP CODE ADDR Setting/Verify CODE ADDR Setting/
 * Flash Operation Key Setting,All items in Function Statement, in Flash Operation Key
 *All items are required. 
 * 2. To return to the IAP program, you need to erase the IAP check code, call IAP_VerifyCode_Erase (), and 
 *then perform soft reset. Single-Chip MicrocomputerAfter reset, IAP will be run first, and IAP code will first 
 *check whether IAP check code exists, and if it does not exist, it will enter the IAP upgrade program.
 * 3. For the convenience of demonstration, this code initializes PA0 as a condition to trigger the erasure of 
 *IAP check code. When PA0 is low, it is triggered, which is the default state No trigger. 
*/

#include "debug.h"
#include "string.h"

/* Global define */
/*******************************************************************************/
/* Macro Definitions */
#define DEF_CORE_RV                       0x01
#define DEF_CORE_CM3                      0x10
#define DEF_CORE_TYPE                     DEF_CORE_CM3
#define DEF_FLASH_PAGE_SIZE               0x80

/* APP CODE ADDR Setting */
#define DEF_APP_CODE_START_ADDR           0x08005000
#define DEF_APP_CODE_END_ADDR             0x08010000
#define DEF_APP_CODE_MAXLEN               (DEF_APP_CODE_END_ADDR-DEF_APP_CODE_START_ADDR)

/* Verify CODE ADDR Setting */
#define DEF_VERIFY_CODE_START_ADDR        0x08004F00
#define DEF_VERIFY_CODE_END_ADDR          0x08005000
#define DEF_VERIFY_CODE_MAXLEN            (DEF_VERIFY_CODE_END_ADDR-DEF_VERIFY_CODE_START_ADDR)
#define DEF_VERIFY_CODE_LEN               0x10

/* Flash Operation Key Setting */
#define DEF_FLASH_OPERATION_KEY_CODE_0    0x1A86FF00
#define DEF_FLASH_OPERATION_KEY_CODE_1    0x55AA55AA

/*******************************************************************************/
/* Global Variable */
uint8_t Com_Buffer[ 128 ];

/* Flash Operation Key */
volatile uint32_t Flash_Operation_Key0;
volatile uint32_t Flash_Operation_Key1;

/*******************************************************************************/
/* Function Statement */
void    GPIO_Cfg_init( void );
uint8_t PA0_Check( void );
uint8_t IAP_VerifyCode_Erase( void );
uint8_t IAP_Flash_Erase( uint32_t address, uint32_t length );

/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main()
{
    uint8_t ret;
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
    SystemCoreClockUpdate();
    Delay_Init();
    Delay_Ms(100);
    USART_Printf_Init(115200);
    printf("SystemClk:%d\r\n", SystemCoreClock);;
    printf("Application for IAP\r\n");

    Flash_Operation_Key0 = DEF_FLASH_OPERATION_KEY_CODE_0;

    while(1)
    {
        //Check If Jump to IAP
        ret = PA0_Check();
        if(ret)
        {
            //If want to jump to IAP Code, first Erase Verifycode, and than reset system
            printf("Reset Chip, prepare to Jump To IAP\r\n");
            printf("Erase Verify-Code\r\n");
            IAP_VerifyCode_Erase( );
            NVIC_SystemReset( );
        }
		
	    Delay_Ms( 200 );
        printf( "USER Code\r\n" );
    }
}

/*********************************************************************
 * @fn      IAP_Flash_Erase
 *
 * @brief   Erase Flash In page(256 bytes),Specify length & address,
 *          Based On Fast Flash Operation,
 *          With address protection and program runaway protection.
 *
 * @return  0: Operation Success
 *          See notes for other errors
 */
uint8_t IAP_Flash_Erase(uint32_t address, uint32_t length)
{
    uint32_t i;
    uint32_t erase_addr;
    uint32_t erase_len;
    volatile uint32_t page_cnts;
    volatile uint32_t page_addr;

    /* Set initial value */
    erase_addr = address;
    page_addr = address;
    erase_len = length;
    if((erase_len % DEF_FLASH_PAGE_SIZE) == 0)
    {
        page_cnts = erase_len / DEF_FLASH_PAGE_SIZE;
    }
    else
    {
        page_cnts = (erase_len / DEF_FLASH_PAGE_SIZE) + 1;
    }

    /* Verify Keys, No flash operation if keys are not correct */
    if((Flash_Operation_Key0 != DEF_FLASH_OPERATION_KEY_CODE_0) || (Flash_Operation_Key1 != DEF_FLASH_OPERATION_KEY_CODE_1))
    {
        /* ERR: Risk of code running away */
        return 0xFF;
    }
    /* Verify Address, No flash operation if the address is out of range */
    if(((erase_addr >= DEF_APP_CODE_START_ADDR) && (erase_addr <= DEF_APP_CODE_END_ADDR)) || ((erase_addr >= DEF_VERIFY_CODE_START_ADDR) && (erase_addr <= DEF_VERIFY_CODE_END_ADDR)))
    {
        for(i = 0; i < page_cnts; i++)
        {
            /* Verify Keys, No flash operation if keys are not correct */
            if(Flash_Operation_Key0 != DEF_FLASH_OPERATION_KEY_CODE_0)
            {
                /* ERR: Risk of code running away */
                return 0xFF;
            }
            FLASH_Unlock_Fast();
            FLASH_ErasePage_Fast(page_addr);
            page_addr += DEF_FLASH_PAGE_SIZE;
        }
    }

    return 0;
}

/*********************************************************************
 * @fn      IAP_VerifyCode_Erase
 *
 * @brief   Erase IAP VerifyCode, Based On Fast Flash Operation.
 *          With address protection and program runaway protection.
 *
 * @return  ret : The meaning of 'ret' can be found in the notes of the
 *          corresponding function.
 */
uint8_t  IAP_VerifyCode_Erase(void)
{
    uint8_t ret;

    /* Verify Code Erase */
    Flash_Operation_Key1 = DEF_FLASH_OPERATION_KEY_CODE_1;
    ret = IAP_Flash_Erase(DEF_VERIFY_CODE_START_ADDR, DEF_VERIFY_CODE_MAXLEN);
    Flash_Operation_Key1 = 0;
    if(ret != 0)
    {
        return ret;
    }

    return 0;
}

/*********************************************************************
 * @fn      GPIO_Cfg_init
 *
 * @brief   GPIO init
 *
 * @return  none
 */
void GPIO_Cfg_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      PA0_Check
 *
 * @brief   Check PA0 state
 *
 * @return  0 - not Press Down
 *          1 - Press Down
 */
uint8_t PA0_Check(void)
{
    u8 i;
    GPIO_Cfg_init();
    i = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
    if(i == 0)
    {
        Delay_Ms(20);
        i = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0);
        if(i == 0)
        {
            return 1;
        }
    }
    return 0;
}




