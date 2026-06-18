/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32x3x5_gpio.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : This file provides all the GPIO firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32x3x5_gpio.h"
#include "ch32x3x5_rcc.h"

/* MASK */
#define LSB_MASK                  ((uint16_t)0xFFFF)
#define DBGAFR_SWJCFG_MASK        ((uint32_t)0xF0FFFFFF)
#define DBGAFR_LOCATION_MASK      ((uint32_t)0x00200000)
#define DBGAFR_NUMBITS_MASK       ((uint32_t)0x00100000)

/*********************************************************************
 * @fn      GPIO_DeInit
 *
 * @brief   Deinitializes the GPIOx peripheral registers to their default
 *        reset values.
 *
 * @param   GPIOx - where x can be (A..D) to select the GPIO peripheral.
 *
 * @return  none
 */
void GPIO_DeInit(GPIO_TypeDef *GPIOx)
{
    if(GPIOx == GPIOA)
    {
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_GPIOA, ENABLE);
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_GPIOA, DISABLE);
    }
    else if(GPIOx == GPIOB)
    {
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_GPIOB, ENABLE);
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_GPIOB, DISABLE);
    }
    else if(GPIOx == GPIOC)
    {
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_GPIOC, ENABLE);
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_GPIOC, DISABLE);
    }
    else if(GPIOx == GPIOD)
    {
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_GPIOD, ENABLE);
        RCC_HB2PeriphResetCmd(RCC_HB2Periph_GPIOD, DISABLE);
    }
}

/*********************************************************************
 * @fn      GPIO_AFIODeInit
 *
 * @brief   Deinitializes the Alternate Functions (remap, event control
 *        and EXTI configuration) registers to their default reset values.
 *
 * @return  none
 */
void GPIO_AFIODeInit(void)
{
    RCC_HB2PeriphResetCmd(RCC_HB2Periph_AFIO, ENABLE);
    RCC_HB2PeriphResetCmd(RCC_HB2Periph_AFIO, DISABLE);
}

/*********************************************************************
 * @fn      GPIO_Init
 *
 * @brief   GPIOx - where x can be (A..D) to select the GPIO peripheral.
 *
 * @param   GPIO_InitStruct - pointer to a GPIO_InitTypeDef structure that
 *        contains the configuration information for the specified GPIO peripheral.
 *
 * @return  none
 */
void GPIO_Init(GPIO_TypeDef *GPIOx, GPIO_InitTypeDef *GPIO_InitStruct)
{
    uint32_t currentmode = 0x00, currentpin = 0x00, pinpos = 0x00, pos = 0x00;
    uint32_t tmpreg = 0x00, pinmask = 0x00;

    currentmode = ((uint32_t)GPIO_InitStruct->GPIO_Mode) & ((uint32_t)0x0F);

    if((((uint32_t)GPIO_InitStruct->GPIO_Mode) & ((uint32_t)0x10)) != 0x00)
    {
        currentmode |= (uint32_t)0x01;
        
        tmpreg = GPIOx->SPEED;
        for(pinpos = 0x00; pinpos < 0x10; pinpos++)
        {
            pos = ((uint32_t)0x01) << pinpos;
            currentpin = (GPIO_InitStruct->GPIO_Pin) & pos;

            if(currentpin == pos)
            {
                pos = pinpos << 1;
                pinmask = ((uint32_t)0x03) << pos;
                tmpreg &= ~pinmask;
                tmpreg |= (((uint32_t)GPIO_InitStruct->GPIO_Speed) << pos);
            }
        }
        GPIOx->SPEED = tmpreg;
    }

    if(((uint32_t)GPIO_InitStruct->GPIO_Pin & ((uint32_t)0x00FF)) != 0x00)
    {
        tmpreg = GPIOx->CFGLR;

        for(pinpos = 0x00; pinpos < 0x08; pinpos++)
        {
            pos = ((uint32_t)0x01) << pinpos;
            currentpin = (GPIO_InitStruct->GPIO_Pin) & pos;

            if(currentpin == pos)
            {
                pos = pinpos << 2;
                pinmask = ((uint32_t)0x0F) << pos;
                tmpreg &= ~pinmask;
                tmpreg |= (currentmode << pos);

                if(GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IPD)
                {
                    GPIOx->BCR = (((uint32_t)0x01) << pinpos);
                }
                else
                {
                    if(GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IPU)
                    {
                        GPIOx->BSHR = (((uint32_t)0x01) << pinpos);
                    }
                }
            }
        }
        GPIOx->CFGLR = tmpreg;
    }

    if(GPIO_InitStruct->GPIO_Pin > 0x00FF)
    {
        tmpreg = GPIOx->CFGHR;

        for(pinpos = 0x00; pinpos < 0x08; pinpos++)
        {
            pos = (((uint32_t)0x01) << (pinpos + 0x08));
            currentpin = ((GPIO_InitStruct->GPIO_Pin) & pos);

            if(currentpin == pos)
            {
                pos = pinpos << 2;
                pinmask = ((uint32_t)0x0F) << pos;
                tmpreg &= ~pinmask;
                tmpreg |= (currentmode << pos);

                if(GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IPD)
                {
                    GPIOx->BCR = (((uint32_t)0x01) << (pinpos + 0x08));
                }

                if(GPIO_InitStruct->GPIO_Mode == GPIO_Mode_IPU)
                {
                    GPIOx->BSHR = (((uint32_t)0x01) << (pinpos + 0x08));
                }
            }
        }
        GPIOx->CFGHR = tmpreg;
    }
}

/*********************************************************************
 * @fn      GPIO_StructInit
 *
 * @brief   Fills each GPIO_InitStruct member with its default
 *
 * @param   GPIO_InitStruct - pointer to a GPIO_InitTypeDef structure
 *      which will be initialized.
 *
 * @return  none
 */
void GPIO_StructInit(GPIO_InitTypeDef *GPIO_InitStruct)
{
    GPIO_InitStruct->GPIO_Pin = GPIO_Pin_All;
    GPIO_InitStruct->GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStruct->GPIO_Mode = GPIO_Mode_IN_FLOATING;
}

/*********************************************************************
 * @fn      GPIO_ReadInputDataBit
 *
 * @brief   GPIOx - where x can be (A..D) to select the GPIO peripheral.
 *
 * @param    GPIO_Pin - specifies the port bit to read.
 *             This parameter can be GPIO_Pin_x where x can be (0..15).
 *
 * @return  The input port pin value.
 */
uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    uint8_t bitstatus = 0x00;

    if((GPIOx->INDR & GPIO_Pin) != (uint32_t)Bit_RESET)
    {
        bitstatus = (uint8_t)Bit_SET;
    }
    else
    {
        bitstatus = (uint8_t)Bit_RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      GPIO_ReadInputData
 *
 * @brief   Reads the specified GPIO input data port.
 *
 * @param   GPIOx - where x can be (A..D) to select the GPIO peripheral.
 *
 * @return  The output port pin value.
 */
uint16_t GPIO_ReadInputData(GPIO_TypeDef *GPIOx)
{
    return ((uint16_t)GPIOx->INDR);
}

/*********************************************************************
 * @fn      GPIO_ReadOutputDataBit
 *
 * @brief   Reads the specified output data port bit.
 *
 * @param   GPIOx - where x can be (A..D) to select the GPIO peripheral.
 *          GPIO_Pin - specifies the port bit to read.
 *            This parameter can be GPIO_Pin_x where x can be (0..15).
 *
 * @return  none
 */
uint8_t GPIO_ReadOutputDataBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    uint8_t bitstatus = 0x00;

    if((GPIOx->OUTDR & GPIO_Pin) != (uint32_t)Bit_RESET)
    {
        bitstatus = (uint8_t)Bit_SET;
    }
    else
    {
        bitstatus = (uint8_t)Bit_RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      GPIO_ReadOutputData
 *
 * @brief   Reads the specified GPIO output data port.
 *
 * @param   GPIOx - where x can be (A..D) to select the GPIO peripheral.
 *
 * @return  GPIO output port pin value.
 */
uint16_t GPIO_ReadOutputData(GPIO_TypeDef *GPIOx)
{
    return ((uint16_t)GPIOx->OUTDR);
}

/*********************************************************************
 * @fn      GPIO_SetBits
 *
 * @brief   Sets the selected data port bits.
 *
 * @param   GPIOx - where x can be (A..D) to select the GPIO peripheral.
 *          GPIO_Pin - specifies the port bits to be written.
 *            This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
 *
 * @return  none
 */
void GPIO_SetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->BSHR = GPIO_Pin;
}

/*********************************************************************
 * @fn      GPIO_ResetBits
 *
 * @brief   Clears the selected data port bits.
 *
 * @param   GPIOx - where x can be (A..D) to select the GPIO peripheral.
 *          GPIO_Pin - specifies the port bits to be written.
 *            This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
 *
 * @return  none
 */
void GPIO_ResetBits(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    GPIOx->BCR = GPIO_Pin;
}

/*********************************************************************
 * @fn      GPIO_WriteBit
 *
 * @brief   Sets or clears the selected data port bit.
 *
 * @param   GPIO_Pin - specifies the port bit to be written.
 *            This parameter can be one of GPIO_Pin_x where x can be (0..15).
 *          BitVal - specifies the value to be written to the selected bit.
 *            Bit_RESET - to clear the port pin.
 *            Bit_SET - to set the port pin.
 *
 * @return  none
 */
void GPIO_WriteBit(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, BitAction BitVal)
{
    if(BitVal != Bit_RESET)
    {
        GPIOx->BSHR = GPIO_Pin;
    }
    else
    {
        GPIOx->BCR = GPIO_Pin;
    }
}

/*********************************************************************
 * @fn      GPIO_Write
 *
 * @brief   Writes data to the specified GPIO data port.
 *
 * @param   GPIOx - where x can be (A..D) to select the GPIO peripheral.
 *          PortVal - specifies the value to be written to the port output data register.
 *
 * @return  none
 */
void GPIO_Write(GPIO_TypeDef *GPIOx, uint16_t PortVal)
{
    GPIOx->OUTDR = PortVal;
}

/*********************************************************************
 * @fn      GPIO_PinLockConfig
 *
 * @brief   Locks GPIO Pins configuration registers.
 *
 * @param   GPIOx - where x can be (A..D) to select the GPIO peripheral.
 *          GPIO_Pin - specifies the port bit to be written.
 *            This parameter can be any combination of GPIO_Pin_x where x can be (0..15).
 *
 * @return  none
 */
void GPIO_PinLockConfig(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    uint32_t tmp = 0x00010000;

    tmp |= GPIO_Pin;
    GPIOx->LCKR = tmp;
    GPIOx->LCKR = GPIO_Pin;
    GPIOx->LCKR = tmp;
    tmp = GPIOx->LCKR;
    tmp = GPIOx->LCKR;
}

/*********************************************************************
 * @fn      GPIO_PinRemapConfig
 *
 * @brief   Changes the mapping of the specified pin.
 *
 * @param   GPIO_Remap - selects the pin to remap.
 *            GPIO_Remap_PD0PD1 - PD0 and PD1 Alternate Function mapping
 *            GPIO_Remap_SWJ_Disable - SWJ Disable Function mapping
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState)
{
    uint32_t tmp = 0x00, tmpreg = 0x00;

    tmpreg = AFIO->PCFR1;
    tmp = GPIO_Remap & LSB_MASK;

    if((GPIO_Remap & (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK)) == (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK)) /* [26:24] 3bit SWD_JTAG */
    {
        tmpreg &= DBGAFR_SWJCFG_MASK;
        AFIO->PCFR1 &= DBGAFR_SWJCFG_MASK;
    }
    else /* [31:0] 1bit */
    {
        tmpreg &= ~(tmp << ((GPIO_Remap >> 0x15) * 0x10));
        tmpreg |= ~DBGAFR_SWJCFG_MASK;
    }

    /* Set bit */
    if(NewState != DISABLE)
    {
        tmpreg |= (tmp << (((GPIO_Remap & 0x00FFFFFF) >> 0x15) * 0x10));
    }

    AFIO->PCFR1 = tmpreg;
}

/*********************************************************************
 * @fn      GPIO_PinAFConfig
 *
 * @brief   Port multiplexing function.
 *
 * @param   GPIOx where x can be (A..D).
 *          GPIO_PinSource - specifies the pin to be configured.
 *            This parameter can be GPIO_PinSourcex where x can be (0..15).
 *          GPIO_AF - The number of the multiplexed function.
 *            This parameter can refer:CH32X315DS0.pdf.
 *
 * @return  none
 */
void GPIO_PinAFConfig(GPIO_TypeDef *GPIOx, uint8_t GPIO_PinSource, uint8_t GPIO_AF)
{
    uint8_t tmp;

    if(GPIO_PinSource >= 0x08)
    {
        tmp = GPIO_PinSource - 0x08;
    }
    else
    {
        tmp = GPIO_PinSource;
    }

    if(GPIOx == GPIOA)
    {
        if(GPIO_PinSource >= 0x08)
        {
            AFIO->GPIOA_AFHR &= ~(0xF << (tmp << 2));
            AFIO->GPIOA_AFHR |= (GPIO_AF << (tmp << 2));
        }
        else
        {
            AFIO->GPIOA_AFLR &= ~(0xF << (tmp << 2));
            AFIO->GPIOA_AFLR |= (GPIO_AF << (tmp << 2));
        }
    }
    else if(GPIOx == GPIOB)
    {
        if(GPIO_PinSource >= 0x08)
        {
            AFIO->GPIOB_AFHR &= ~(0xF << (tmp << 2));
            AFIO->GPIOB_AFHR |= (GPIO_AF << (tmp << 2));
        }
        else
        {
            AFIO->GPIOB_AFLR &= ~(0xF << (tmp << 2));
            AFIO->GPIOB_AFLR |= (GPIO_AF << (tmp << 2));
        }
    }
    else if(GPIOx == GPIOC)
    {
        if(GPIO_PinSource >= 0x08)
        {
            AFIO->GPIOC_AFHR &= ~(0xF << (tmp << 2));
            AFIO->GPIOC_AFHR |= (GPIO_AF << (tmp << 2));
        }
        else
        {
            AFIO->GPIOC_AFLR &= ~(0xF << (tmp << 2));
            AFIO->GPIOC_AFLR |= (GPIO_AF << (tmp << 2));
        }
    }
    else if(GPIOx == GPIOD)
    {
        if(GPIO_PinSource >= 0x08)
        {
            AFIO->GPIOD_AFHR &= ~(0xF << (tmp << 2));
            AFIO->GPIOD_AFHR |= (GPIO_AF << (tmp << 2));
        }
        else
        {
            AFIO->GPIOD_AFLR &= ~(0xF << (tmp << 2));
            AFIO->GPIOD_AFLR |= (GPIO_AF << (tmp << 2));
        }
    }
  
}

/*********************************************************************
 * @fn      GPIO_EXTILineConfig
 *
 * @brief   Selects the GPIO pin used as EXTI Line.
 *
 * @param   GPIO_PortSource - selects the GPIO port to be used as source for EXTI lines.
 *            This parameter can be GPIO_PortSourceGPIOx where x can be (A..D) and GPIO_PortSourceCMP.
 *          GPIO_PinSource - specifies the EXTI line to be configured.
 *            This parameter can be GPIO_PinSourcex where x can be (0..15).
 *
 * @return  none
 */
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource)
{
    uint32_t tmp = 0x00;

    tmp = ((uint32_t)0x0F) << (0x04 * (GPIO_PinSource & (uint8_t)0x07));
    AFIO->EXTICR[GPIO_PinSource >> 0x02] &= ~tmp;
    AFIO->EXTICR[GPIO_PinSource >> 0x02] |= (((uint32_t)GPIO_PortSource) << (0x04 * (GPIO_PinSource & (uint8_t)0x03)));
}

/*********************************************************************
 * @fn      GPIO_USBPDHVT
 *
 * @brief   Configure USBPDHVT.
 *
 * @param   none
 * @return  none
 */
void GPIO_USBPDHVT(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        AFIO->CR |= 1<<0;
    }
    else
    {
        AFIO->CR  &= ~(uint16_t)(1<<0);
    }
}

/*********************************************************************
 * @fn      GPIO_USBPDRISE
 *
 * @brief   Configure USBPDRISE.
 * @param   GPIO_USBPDRISE_ModeX - the time mode of HSE ready.
 *            GPIO_USBPDRISE_Mode0 - Tr reduce 11%.
 *            GPIO_USBPDRISE_Mode1 - Tr reduce 5%.
 *            GPIO_USBPDRISE_Mode2- Tr reduce 0%.
 *            GPIO_USBPDRISE_Mode3 - Tr improve 7%.
 * @return  none
 */
void GPIO_USBPDRISE(uint32_t GPIO_USBPDRISE)
{
    AFIO->CR &= ~(uint16_t)(3<<1);
    AFIO->CR |= GPIO_USBPDRISE;
}

/*********************************************************************
 * @fn      VCfg_Init
 *
 * @brief   VCfg_Init.
 *
 * @param   none
 *
 * @return  chipID
 */
uint32_t VCfg_Init(void)
{
    vu32 TempReg;
    uint32_t chip = 0;

    chip = (*(vu32*)(Cfg_BASE-4));

    if((chip & 0xFFF00000) == 0x31500000)
    {
        for(u8 i = 0; i < 11; i++)
        {
            TempReg = *(vu32*)(Cfg_BASE+4*i);
            *(vu32*)(Reg_BASE+4*i) = TempReg;
        }
    }
    else
    {
        chip = *(vu32*)0x1FFFF704;
    }

    return chip;
} 

/*********************************************************************
 * @fn      GPIO_IPD_Unused
 *
 * @brief   Configure unused GPIO as input pull-down.
 *
 * @param   none
 *
 * @return  none
 */
void GPIO_IPD_Unused(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    uint32_t chip = 0;
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA | RCC_HB2Periph_GPIOB | RCC_HB2Periph_GPIOC|\
                           RCC_HB2Periph_GPIOD , ENABLE);
                     
    chip = VCfg_Init() & (~0x000000F0);

    switch(chip)
    {
        case 0x31510001:     //CH32X315CCU
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2\
                                          |GPIO_Pin_3|GPIO_Pin_12|GPIO_Pin_13\
                                          |GPIO_Pin_14|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
            GPIO_Init(GPIOB, &GPIO_InitStructure);
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1|GPIO_Pin_2\
                                          |GPIO_Pin_3|GPIO_Pin_10|GPIO_Pin_11\
                                          |GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14\
                                          |GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
            GPIO_Init(GPIOC, &GPIO_InitStructure);
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_10\
                                          |GPIO_Pin_11;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
            GPIO_Init(GPIOD, &GPIO_InitStructure);

            break;
        }
        case 0x31520002:     //CH32X315WCU
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14\
                                          |GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
            GPIO_Init(GPIOB, &GPIO_InitStructure);
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
            GPIO_Init(GPIOD, &GPIO_InitStructure);

            break;
        }
        case 0x31530003:     //CH32X305RCT
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1|GPIO_Pin_2|GPIO_Pin_3;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
            GPIO_Init(GPIOB, &GPIO_InitStructure);
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
            GPIO_Init(GPIOC, &GPIO_InitStructure);
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_14\
                                          |GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
            GPIO_Init(GPIOD, &GPIO_InitStructure);

            break;
        }
        case 0x31500000:     //CH32X315MCU
        {
            break;
        }
        default:
        {
            break;
        }
    }
}





