/********************************** (C) COPYRIGHT  *******************************
 * File Name          : ch32m030_gpio.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2025/09/05
 * Description        : This file provides all the GPIO firmware functions.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "ch32m030_gpio.h"
#include "ch32m030_rcc.h"

/* MASK */
#define LSB_MASK                  ((uint16_t)0xFFFF)
#define DBGAFR_POSITION_MASK      ((uint32_t)0x000F0000)
#define DBGAFR_SDI_MASK           ((uint32_t)0xF8FFFFFF)
#define DBGAFR_LOCATION_MASK      ((uint32_t)0x00200000)
#define DBGAFR_NUMBITS_MASK       ((uint32_t)0x00100000)

/*********************************************************************
 * @fn      GPIO_DeInit
 *
 * @brief   Deinitializes the GPIOx peripheral registers to their default
 *        reset values.
 *
 * @param   GPIOx - where x can be (A..C) to select the GPIO peripheral.
 *
 * @return  none
 */
void GPIO_DeInit(GPIO_TypeDef *GPIOx)
{
    if(GPIOx == GPIOA)
    {
        RCC_PB2PeriphResetCmd(RCC_PB2Periph_GPIOA, ENABLE);
        RCC_PB2PeriphResetCmd(RCC_PB2Periph_GPIOA, DISABLE);
    }
    else if(GPIOx == GPIOB)
    {
        RCC_PB2PeriphResetCmd(RCC_PB2Periph_GPIOB, ENABLE);
        RCC_PB2PeriphResetCmd(RCC_PB2Periph_GPIOB, DISABLE);
    }
    else if(GPIOx == GPIOC)
    {
        RCC_PB2PeriphResetCmd(RCC_PB2Periph_GPIOC, ENABLE);
        RCC_PB2PeriphResetCmd(RCC_PB2Periph_GPIOC, DISABLE);
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
    RCC_PB2PeriphResetCmd(RCC_PB2Periph_AFIO, ENABLE);
    RCC_PB2PeriphResetCmd(RCC_PB2Periph_AFIO, DISABLE);
}

/*********************************************************************
 * @fn      GPIO_Init
 *
 * @brief   GPIOx - where x can be (A..C) to select the GPIO peripheral.
 *          When needing to configure PB0,PB1 input pull-down or input pull-up,
 *          EXTEN->CTLR1 registers must be configured to take effect.
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
        currentmode |= (uint32_t)GPIO_InitStruct->GPIO_Speed;
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
    GPIO_InitStruct->GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStruct->GPIO_Mode = GPIO_Mode_IN_FLOATING;
}

/*********************************************************************
 * @fn      GPIO_ReadInputDataBit
 *
 * @brief   GPIOx - where x can be (A..C) to select the GPIO peripheral.
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
 * @param   GPIOx: where x can be (A..C) to select the GPIO peripheral.
 *
 * @return  The input port pin value.
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
 * @param   GPIOx - where x can be (A..C) to select the GPIO peripheral.
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
 * @param   GPIOx - where x can be (A..C) to select the GPIO peripheral.
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
 * @param   GPIOx - where x can be (A..C) to select the GPIO peripheral.
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
 * @param   GPIOx - where x can be (A..C) to select the GPIO peripheral.
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
 * @param   GPIOx - where x can be (A..C) to select the GPIO peripheral.
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
 * @param   GPIOx - where x can be (A..C) to select the GPIO peripheral.
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
 *            GPIO_PartialRemap1_I2C1 - I2C1 Partial1 Alternate Function mapping
 *            GPIO_PartialRemap2_I2C1 - I2C1 Partial2 Alternate Function mapping
 *            GPIO_FullRemap_I2C1 - I2C1 Full Alternate Function mapping
 *            GPIO_PartialRemap1_USART1 - USART1 Partial1 Alternate Function mapping
 *            GPIO_PartialRemap2_USART1 - USART1 Partial2 Alternate Function mapping
 *            GPIO_PartialRemap3_USART1 - USART1 Partial3 Alternate Function mapping
 *            GPIO_PartialRemap4_USART1 - USART1 Partial4 Alternate Function mapping
 *            GPIO_FullRemap_USART1 - USART1 Full Alternate Function mapping
 *            GPIO_PartialRemap1_SPI1 - SPI1 Partial1 Alternate Function mapping
 *            GPIO_PartialRemap2_SPI1 - SPI1 Partial2 Alternate Function mapping
 *            GPIO_FullRemap_SPI1 - SPI1 Full Alternate Function mapping
 *            GPIO_PartialRemap1_TIM1 - TIM1 Partial1 Alternate Function mapping
 *            GPIO_PartialRemap2_TIM1 - TIM1 Partial2 Alternate Function mapping
 *            GPIO_PartialRemap3_TIM1 - TIM1 Partial3 Alternate Function mapping
 *            GPIO_FullRemap_TIM1 - TIM1 Full Alternate Function mapping
 *            GPIO_PartialRemap1_TIM2 - TIM2 Partial1 Alternate Function mapping
 *            GPIO_PartialRemap2_TIM2 - TIM2 Partial2 Alternate Function mapping
 *            GPIO_FullRemap_TIM2 - TIM2 Full Alternate Function mapping
 *            GPIO_PartialRemap1_TIM3 - TIM3 Partial1 Alternate Function mapping
 *            GPIO_PartialRemap2_TIM3 - TIM3 Partial2 Alternate Function mapping
 *            GPIO_PartialRemap3_TIM3 - TIM3 Partial3 Alternate Function mapping
 *            GPIO_FullRemap_TIM3 - TIM3 Full Alternate Function mapping
 *            GPIO_Remap_SPI1 - SPI1 Alternate Function mapping
 *            GPIO_Remap_I2C1 - I2C1 Alternate Function mapping
 *            GPIO_Remap_UART1 - UART1 Alternate Function mapping
 *            GPIO_Remap_ADC1_ETRG - ADC1 External Trigger mapping
 *            GPIO_Remap_PB5_6 - PB5 and PB6 Alternate Function mapping
 *            GPIO_Remap_HO_DIO_PB9 - PB9 Alternate Function mapping
 *            GPIO_Remap_HO_DIO_PB11 - PB11 Alternate Function mapping
 *            GPIO_Remap_HO_DIO_PB13 - PB13 Alternate Function mapping
 *            GPIO_Remap_HO_DIO_PB15 - PB15 Alternate Function mapping
 *            GPIO_Remap_SDI_Disable - Full SDI Disabled
 *
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void GPIO_PinRemapConfig(uint32_t GPIO_Remap, FunctionalState NewState)
{
    uint32_t tmp = 0x00, tmp1 = 0x00, tmpreg = 0x00, tmpmask = 0x00;

    tmpreg = AFIO->PCFR1;

    tmpmask = (GPIO_Remap & DBGAFR_POSITION_MASK) >> 0x10;
    tmp = GPIO_Remap & LSB_MASK;

    if((GPIO_Remap & (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK)) == (DBGAFR_LOCATION_MASK | DBGAFR_NUMBITS_MASK)) /* [26:24] SDI */
    {
        tmpreg &= DBGAFR_SDI_MASK;
        AFIO->PCFR1 &= DBGAFR_SDI_MASK;
    }
    else if((GPIO_Remap & 0x08000000) == 0x08000000) /* [15:0] 3bit */
    {
        tmp1 = ((uint32_t)0x07) << tmpmask;
        tmpreg &= ~tmp1;
    }
    else if((GPIO_Remap & DBGAFR_NUMBITS_MASK) == DBGAFR_NUMBITS_MASK) /* [15:0] 2bit */
    {
        tmp1 = ((uint32_t)0x03) << tmpmask;
        tmpreg &= ~tmp1;
    }
    else /* [31:0] 1bit */
    {
        tmpreg &= ~(tmp << (((GPIO_Remap & 0x00FFFFFF)>> 0x15) * 0x10));
    }

    if(NewState != DISABLE)
    {
        tmpreg |= (tmp << (((GPIO_Remap & 0x00FFFFFF)>> 0x15) * 0x10));
    }


    AFIO->PCFR1 = tmpreg;
}

/*********************************************************************
 * @fn      GPIO_EXTILineConfig
 *
 * @brief   Selects the GPIO pin used as EXTI Line.
 *
 * @param   GPIO_PortSource - selects the GPIO port to be used as source for EXTI lines.
 *            This parameter can be GPIO_PortSourceGPIOx where x can be (A..C).
 *          GPIO_PinSource - specifies the EXTI line to be configured.
 *            This parameter can be GPIO_PinSourcex where x can be (0..15).
 *
 * @return  none
 */
void GPIO_EXTILineConfig(uint8_t GPIO_PortSource, uint8_t GPIO_PinSource)
{
    uint32_t tmp = 0x00;

    tmp = ((uint32_t)0x03) << (GPIO_PinSource<<1);
    AFIO->EXTICR &= ~tmp;
    AFIO->EXTICR |= ((uint32_t)(GPIO_PortSource<<(GPIO_PinSource<<1)));
}

/*********************************************************************
 * @fn      GPIO_IPD_Unused
 *
 * @brief   Configure unused GPIO as input pull-up.
 *
 * @param   none
 *
 * @return  none
 */
void GPIO_IPD_Unused(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    uint32_t chip = 0;
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA | RCC_PB2Periph_GPIOB | RCC_PB2Periph_GPIOC , ENABLE);
    chip =  *( uint32_t * )0x1FFFF384 & (~0x000000F0);
    switch(chip)
    {
        case 0x03B20800:     //CH32M030K8U
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_11\
                                          |GPIO_Pin_12|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOA, &GPIO_InitStructure);
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_2|GPIO_Pin_3\
                                          |GPIO_Pin_4;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOC, &GPIO_InitStructure);
            break;
        }
        case 0x03BB0800:     //CH32M030G8R
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5\
                                          |GPIO_Pin_6|GPIO_Pin_12\
                                          |GPIO_Pin_14|GPIO_Pin_15;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOA, &GPIO_InitStructure);
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2|GPIO_Pin_3;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOB, &GPIO_InitStructure);
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1\
                                          |GPIO_Pin_2|GPIO_Pin_3\
                                          |GPIO_Pin_4;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOC, &GPIO_InitStructure);

            break;
        }
        case 0x03B30800:     //CH32M030C8U3
        {
            GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
            GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
            GPIO_Init(GPIOC, &GPIO_InitStructure);
            break;
        }
        case 0x03B10800:     //CH32M030C8T
        {
            break;
        }
        case 0x03B00800:     //CH32M030C8U7
        {
            break;
        }
        default:
        {
            break;
        }
    }

}



