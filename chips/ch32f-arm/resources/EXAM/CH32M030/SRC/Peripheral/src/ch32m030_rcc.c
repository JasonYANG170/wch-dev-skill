/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32m030_rcc.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2024/09/01
 * Description        : This file provides all the RCC firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32m030_rcc.h"


/* CTLR register bit mask */
#define CTLR_HSEBYP_Reset          ((uint32_t)0xFFFBFFFF)
#define CTLR_HSEBYP_Set            ((uint32_t)0x00040000)
#define CTLR_HSEON_Reset           ((uint32_t)0xFFFEFFFF)
#define CTLR_HSEON_Set             ((uint32_t)0x00010000)
#define CTLR_HSITRIM_Mask          ((uint32_t)0xFFFFFF07)

/* CFGR0 register bit mask */
#define CFGR0_PLL_Mask             ((uint32_t)0xFFFEFFFF)
#define CFGR0_PLLSRC_Mask          ((uint32_t)0x00010000)
#define CFGR0_SWS_Mask             ((uint32_t)0x0000000C)
#define CFGR0_SW_Mask              ((uint32_t)0xFFFFFFFC)
#define CFGR0_HPRE_Reset_Mask      ((uint32_t)0xFFFFF80F)
#define CFGR0_HPRE_Set_Mask        ((uint32_t)0x000007F0)
#define CFGR0_ADCPRE_Reset_Mask    ((uint32_t)0xFFFF07FF)
#define CFGR0_ADCPRE_Set_Mask      ((uint32_t)0x0000F800)

/* RSTSCKR register bit mask */
#define RSTSCKR_RMVF_Set           ((uint32_t)0x01000000)

/* RCC Flag Mask */
#define FLAG_Mask                  ((uint8_t)0x1F)

/* INTR register byte 2 (Bits[15:8]) base address */
#define INTR_BYTE2_ADDRESS         ((uint32_t)0x40021009)

/* INTR register byte 3 (Bits[23:16]) base address */
#define INTR_BYTE3_ADDRESS         ((uint32_t)0x4002100A)

/* CFGR0 register byte 4 (Bits[31:24]) base address */
#define CFGR0_BYTE4_ADDRESS        ((uint32_t)0x40021007)

static __I uint8_t ADCPrescTable[20] = {2, 4, 6, 8, 4, 8, 12, 16, 8, 16, 24, 32, 16, 32, 48, 64, 32, 64, 96, 128};

/*********************************************************************
 * @fn      RCC_DeInit
 *
 * @brief   Resets the RCC clock configuration to the default reset state.
 *          Note-
 *          HSE can not be stopped if it is used directly or through the PLL as system clock.
 * @return  none
 */
void RCC_DeInit(void)
{
    RCC->CTLR |= (uint32_t)0x00000001;
    RCC->CFGR0 &= (uint32_t)0xFFFFFFFC;
    RCC->CFGR0 &= (uint32_t)0xE8FF0003;
    RCC->CTLR &= (uint32_t)0xFEF6FFFF;
    RCC->CTLR &= (uint32_t)0xFFFBFFFF;
    RCC->CFGR0 &= (uint32_t)0xFFFEFFFF;
    RCC->INTR = 0x009F0000;
    /* Flash 0 wait state */
    FLASH->ACTLR = (uint32_t)FLASH_ACTLR_LATENCY_0;
}

/*********************************************************************
 * @fn      RCC_HSEConfig
 *
 * @brief   Configures the External High Speed oscillator (HSE).
 *
 * @param   RCC_HSE -
 *            RCC_HSE_OFF - HSE oscillator OFF.
 *            RCC_HSE_ON - HSE oscillator ON.
 *            RCC_HSE_Bypass - HSE oscillator bypassed with external clock.
 *            Note-
 *              HSE can not be stopped if it is used directly or through
 *            the PLL as system clock.
 * @return  none
 */
void RCC_HSEConfig(uint32_t RCC_HSE)
{
    RCC->CTLR &= CTLR_HSEON_Reset;
    RCC->CTLR &= CTLR_HSEBYP_Reset;

    switch(RCC_HSE)
    {
        case RCC_HSE_ON:
            RCC->CTLR |= CTLR_HSEON_Set;
            break;

        case RCC_HSE_Bypass:
            RCC->CTLR |= CTLR_HSEBYP_Set | CTLR_HSEON_Set;
            break;

        default:
            break;
    }
}

/*********************************************************************
 * @fn      RCC_WaitForHSEStartUp
 *
 * @brief   Waits for HSE start-up.
 *
 * @return  READY - HSE oscillator is stable and ready to use.
 *          NoREADY - HSE oscillator not yet ready.
 */
ErrorStatus RCC_WaitForHSEStartUp(void)
{
    __IO uint32_t StartUpCounter = 0;

    ErrorStatus status = NoREADY;
    FlagStatus  HSEStatus = RESET;

    do
    {
        HSEStatus = RCC_GetFlagStatus(RCC_FLAG_HSERDY);
        StartUpCounter++;
    } while((StartUpCounter != HSE_STARTUP_TIMEOUT) && (HSEStatus == RESET));

    if(RCC_GetFlagStatus(RCC_FLAG_HSERDY) != RESET)
    {
        status = READY;
    }
    else
    {
        status = NoREADY;
    }

    return (status);
}

/*********************************************************************
 * @fn      RCC_AdjustHSICalibrationValue
 *
 * @brief   Adjusts the Internal High Speed oscillator (HSI) calibration value.
 *
 * @param   HSICalibrationValue - specifies the calibration trimming value.
 *            This parameter must be a number between 0 and 0x1F.
 *
 * @return  none
 */
void RCC_AdjustHSICalibrationValue(uint8_t HSICalibrationValue)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CTLR;
    tmpreg &= CTLR_HSITRIM_Mask;
    tmpreg |= (uint32_t)HSICalibrationValue << 3;
    RCC->CTLR = tmpreg;
}

/*********************************************************************
 * @fn      RCC_HSICmd
 *
 * @brief   Enables or disables the Internal High Speed oscillator (HSI).
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_HSICmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CTLR |= (1 << 0);
    }
    else
    {
        RCC->CTLR &= ~(1 << 0);
    }
}

/*********************************************************************
 * @fn      RCC_PLLConfig
 *
 * @brief   Configures the PLL clock source.
 *
 * @param   RCC_PLLSource - specifies the PLL entry clock source.
 *            RCC_PLLSource_HSI - HSI oscillator clock selected as PLL
 *        clock entry.
 *            RCC_PLLSource_HSE - HSE oscillator clock selected as PLL
 *        clock entry.
 *
 * @return  none
 */
void RCC_PLLConfig(uint32_t RCC_PLLSource)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;
    tmpreg &= CFGR0_PLL_Mask;
    tmpreg |= RCC_PLLSource;
    RCC->CFGR0 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_PLLCmd
 *
 * @brief   Enables or disables the PLL.
 *           Note-The PLL can not be disabled if it is used as system clock.
 *          
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_PLLCmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CTLR |= (1 << 24);
    }
    else
    {
        RCC->CTLR &= ~(1 << 24);
    }
}

/*********************************************************************
 * @fn      RCC_SYSCLKConfig
 *
 * @brief   Configures the system clock (SYSCLK).
 *
 * @param   RCC_SYSCLKSource - specifies the clock source used as system clock.
 *            RCC_SYSCLKSource_HSI - HSI selected as system clock.
 *            RCC_SYSCLKSource_HSE - HSE selected as system clock.
 *            RCC_SYSCLKSource_PLLCLK - PLL selected as system clock.
 *
 * @return  none
 */
void RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;
    tmpreg &= CFGR0_SW_Mask;
    tmpreg |= RCC_SYSCLKSource;
    RCC->CFGR0 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_GetSYSCLKSource
 *
 * @brief   Returns the clock source used as system clock.
 *
 * @return  0x00 - HSI used as system clock.
 *          0x04 - HSE used as system clock.
 *          0x08 - PLL used as system clock.
 */
uint8_t RCC_GetSYSCLKSource(void)
{
    return ((uint8_t)(RCC->CFGR0 & CFGR0_SWS_Mask));
}

/*********************************************************************
 * @fn      RCC_HCLKConfig
 *
 * @brief   Configures the HB clock (HCLK).
 *
 * @param   RCC_SYSCLK_Div_Val - defines the HB clock divider. This clock is derived from
 *        the system clock (SYSCLK).
 *          This parameter must be a number between 0x00 and 0x7F.
 *
 * @return  none
 */
void RCC_HCLKConfig(uint32_t RCC_SYSCLK_Div_Val)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;
    tmpreg &= CFGR0_HPRE_Reset_Mask;
    tmpreg |= RCC_SYSCLK_Div_Val;
    RCC->CFGR0 = tmpreg;
    __NOP(); __NOP(); __NOP(); __NOP();
    __NOP(); __NOP(); __NOP(); __NOP();
}

/*********************************************************************
 * @fn      RCC_ITConfig
 *
 * @brief   Enables or disables the specified RCC interrupts.
 *
 * @param   RCC_IT - specifies the RCC interrupt sources to be enabled or disabled.
 *            RCC_IT_LSIRDY - LSI ready interrupt.
 *            RCC_IT_HSIRDY - HSI ready interrupt.
 *            RCC_IT_HSERDY - HSE ready interrupt.
 *            RCC_IT_PLLRDY - PLL ready interrupt.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_ITConfig(uint8_t RCC_IT, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        *(__IO uint8_t *)INTR_BYTE2_ADDRESS |= RCC_IT;
    }
    else
    {
        *(__IO uint8_t *)INTR_BYTE2_ADDRESS &= (uint8_t)~RCC_IT;
    }
}

/*********************************************************************
 * @fn      RCC_ADCCLKConfig
 *
 * @brief   Configures the ADC clock (ADCCLK).
 *
 * @param   RCC_HB - defines the ADC clock divider. This clock is derived from
 *        the HB clock.
 *          RCC_HB_Div2 - ADC clock = HCLK/2.
 *          RCC_HB_Div4 - ADC clock = HCLK/4.
 *          RCC_HB_Div6 - ADC clock = HCLK/6.
 *          RCC_HB_Div8 - ADC clock = HCLK/8.
 *          RCC_HB_Div12 - ADC clock = HCLK/12.
 *          RCC_HB_Div16 - ADC clock = HCLK/16.
 *          RCC_HB_Div24 - ADC clock = HCLK/24.
 *          RCC_HB_Div32 - ADC clock = HCLK/32.
 *          RCC_HB_Div48 - ADC clock = HCLK/48.
 *          RCC_HB_Div64 - ADC clock = HCLK/64.
 *          RCC_HB_Div96 - ADC clock = HCLK/96.
 *          RCC_HB_Div128 - ADC clock = HCLK/128.
 *
 * @return  none
 */
void RCC_ADCCLKConfig(uint32_t RCC_HB)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;
    tmpreg &= CFGR0_ADCPRE_Reset_Mask;
    tmpreg |= RCC_HB;
    RCC->CFGR0 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_LSICmd
 *
 * @brief   Enables or disables the Internal Low Speed oscillator (LSI).
 *          Note-
 *            LSI can not be disabled if the IWDG is running.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_LSICmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->RSTSCKR |= (1 << 0);
    }
    else
    {
        RCC->RSTSCKR &= ~(1 << 0);
    }
}

/*********************************************************************
 * @fn      RCC_GetClocksFreq
 *
 * @brief   The result of this function could be not correct when using
 *        fractional value for HSE crystal.
 *
 * @param   RCC_Clocks - pointer to a RCC_ClocksTypeDef structure which will hold
 *        the clocks frequencies.
 *
 * @return  none
 */
void RCC_GetClocksFreq(RCC_ClocksTypeDef *RCC_Clocks)
{
    uint32_t tmp = 0, pllsource = 0, presc = 0;

    tmp = RCC->CFGR0 & CFGR0_SWS_Mask;

    switch(tmp)
    {
        case 0x00:
            RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
            break;

        case 0x04:
            RCC_Clocks->SYSCLK_Frequency = HSE_VALUE;
            break;

        case 0x08:
            pllsource = RCC->CFGR0 & CFGR0_PLLSRC_Mask;

            if(pllsource == 0x00)
            {
                RCC_Clocks->SYSCLK_Frequency = HSI_VALUE * 18;
            }
            else
            {
                RCC_Clocks->SYSCLK_Frequency = HSE_VALUE * 18;
            }
            break;

        default:
            RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
            break;
    }

    tmp = RCC->CFGR0 & CFGR0_HPRE_Set_Mask;
    tmp = tmp >> 4;
    presc = tmp + 1;
    RCC_Clocks->HCLK_Frequency = RCC_Clocks->SYSCLK_Frequency / presc;

    tmp = RCC->CFGR0 & CFGR0_ADCPRE_Set_Mask;
    tmp = tmp >> 11;
    tmp = ((tmp & 0x18) >> 3) | ((tmp & 0x7) << 2);

    if((tmp & 0x13) >= 4)
    {
        tmp -= 12;
    }
    else
    {
        tmp &= 0x03;
    }

    presc = ADCPrescTable[tmp];
    RCC_Clocks->ADCCLK_Frequency = RCC_Clocks->HCLK_Frequency / presc;
}

/*********************************************************************
 * @fn      RCC_HBPeriphClockCmd
 *
 * @brief   Enables or disables the HB peripheral clock.
 *
 * @param   RCC_HBPeriph - specifies the HB peripheral to gates its clock.
 *            RCC_HBPeriph_DMA1.
 *            RCC_HBPeriph_SRAM.
 *            RCC_HBPeriph_USBFS.
 *            RCC_HBPeriph_USBPD0.
 *            RCC_HBPeriph_USBPD1.
 *            RCC_HBPeriph_OPCM.
 *
 *          Note-
 *            SRAM  clock can be disabled only during sleep mode.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_HBPeriphClockCmd(uint32_t RCC_HBPeriph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->HBPCENR |= RCC_HBPeriph;
    }
    else
    {
        RCC->HBPCENR &= ~RCC_HBPeriph;
    }
}

/*********************************************************************
 * @fn      RCC_PB2PeriphClockCmd
 *
 * @brief   Enables or disables the High Speed PB (PB2) peripheral clock.
 *
 * @param   RCC_PB2Periph - specifies the PB2 peripheral to gates its clock.
 *            RCC_PB2Periph_AFIO.
 *            RCC_PB2Periph_GPIOA.
 *            RCC_PB2Periph_GPIOB.
 *            RCC_PB2Periph_GPIOC.
 *            RCC_PB2Periph_ADC1.
 *            RCC_PB2Periph_TIM1.
 *            RCC_PB2Periph_SPI1.
 *            RCC_PB2Periph_USART1.
 *          NewState - ENABLE or DISABLE
 *
 * @return  none
 */
void RCC_PB2PeriphClockCmd(uint32_t RCC_PB2Periph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->PB2PCENR |= RCC_PB2Periph;
    }
    else
    {
        RCC->PB2PCENR &= ~RCC_PB2Periph;
    }
}

/*********************************************************************
 * @fn      RCC_PB1PeriphClockCmd
 *
 * @brief   Enables or disables the Low Speed PB (PB1) peripheral clock.
 *
 * @param   RCC_PB1Periph - specifies the PB1 peripheral to gates its clock.
 *            RCC_PB1Periph_TIM2.
 *            RCC_PB1Periph_TIM3.
 *            RCC_PB1Periph_WWDG.
 *            RCC_PB1Periph_I2C1.
 *            RCC_PB1Periph_PWR.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_PB1PeriphClockCmd(uint32_t RCC_PB1Periph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->PB1PCENR |= RCC_PB1Periph;
    }
    else
    {
        RCC->PB1PCENR &= ~RCC_PB1Periph;
    }
}

/*********************************************************************
 * @fn      RCC_PB2PeriphResetCmd
 *
 * @brief   Forces or releases High Speed PB (PB2) peripheral reset.
 *
 * @param   RCC_PB2Periph - specifies the PB2 peripheral to reset.
 *            RCC_PB2Periph_AFIO.
 *            RCC_PB2Periph_GPIOA.
 *            RCC_PB2Periph_GPIOB.
 *            RCC_PB2Periph_GPIOC.
 *            RCC_PB2Periph_ADC1.
 *            RCC_PB2Periph_TIM1.
 *            RCC_PB2Periph_SPI1.
 *            RCC_PB2Periph_USART1.
 *          NewState - ENABLE or DISABLE
 *
 * @return  none
 */
void RCC_PB2PeriphResetCmd(uint32_t RCC_PB2Periph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->PB2PRSTR |= RCC_PB2Periph;
    }
    else
    {
        RCC->PB2PRSTR &= ~RCC_PB2Periph;
    }
}

/*********************************************************************
 * @fn      RCC_PB1PeriphResetCmd
 *
 * @brief   Forces or releases Low Speed PB (PB1) peripheral reset.
 *
 * @param   RCC_PB1Periph - specifies the PB1 peripheral to reset.
 *            RCC_PB1Periph_TIM2.
 *            RCC_PB1Periph_TIM3.
 *            RCC_PB1Periph_WWDG.
 *            RCC_PB1Periph_I2C1.
 *            RCC_PB1Periph_PWR.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_PB1PeriphResetCmd(uint32_t RCC_PB1Periph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->PB1PRSTR |= RCC_PB1Periph;
    }
    else
    {
        RCC->PB1PRSTR &= ~RCC_PB1Periph;
    }
}

/*********************************************************************
 * @fn      RCC_ClockSecuritySystemCmd
 *
 * @brief   Enables or disables the Clock Security System.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_ClockSecuritySystemCmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CTLR |= (1 << 19);
    }
    else
    {
        RCC->CTLR &= ~(1 << 19);
    }
}

/*********************************************************************
 * @fn      RCC_MCOConfig
 *
 * @brief   Selects the clock source to output on MCO pin.
 *
 * @param   RCC_MCO - specifies the clock source to output.
 *            RCC_MCO_NoClock - No clock selected.
 *            RCC_MCO_SYSCLK - System clock selected.
 *            RCC_MCO_HSI - HSI oscillator clock selected.
 *            RCC_MCO_HSE - HSE oscillator clock selected.
 *
 * @return  none
 */
void RCC_MCOConfig(uint8_t RCC_MCO)
{
    *(__IO uint8_t *)CFGR0_BYTE4_ADDRESS = RCC_MCO;
}

/*********************************************************************
 * @fn      RCC_GetFlagStatus
 *
 * @brief   Checks whether the specified RCC flag is set or not.
 *
 * @param   RCC_FLAG - specifies the flag to check.
 *            RCC_FLAG_HSIRDY - HSI oscillator clock ready.
 *            RCC_FLAG_HSERDY - HSE oscillator clock ready.
 *            RCC_FLAG_PLLRDY - PLL clock ready.
 *            RCC_FLAG_LSIRDY - LSI oscillator clock ready.
 *            RCC_FLAG_USBPD0RST - USBPD0 reset.
 *            RCC_FLAG_USBPD1RST - USBPD1 reset.
 *            RCC_FLAG_ADCAWDRST - ADC Watch dog reset.
 *            RCC_FLAG_PINRST - Pin reset.
 *            RCC_FLAG_PORRST - POR/PDR reset.
 *            RCC_FLAG_SFTRST - Software reset.
 *            RCC_FLAG_WWDGRST - Window Watch dog reset.
 *            RCC_FLAG_LPWRRST - Low Power reset.
 *
 * @return  FlagStatus - SET or RESET.
 */
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG)
{
    uint32_t tmp = 0;
    uint32_t statusreg = 0;

    FlagStatus bitstatus = RESET;
    tmp = RCC_FLAG >> 5;

    if(tmp == 1)
    {
        statusreg = RCC->CTLR;
    }
    else
    {
        statusreg = RCC->RSTSCKR;
    }

    tmp = RCC_FLAG & FLAG_Mask;

    if((statusreg & ((uint32_t)1 << tmp)) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      RCC_ClearFlag
 *
 * @brief   Clears the RCC reset flags.
 *          Note-   
 *            The reset flags are - RCC_FLAG_USBPD0RST, RCC_FLAG_USBPD1RST,
 *          RCC_FLAG_ADCAWDRST, RCC_FLAG_PINRST, RCC_FLAG_PORRST, RCC_FLAG_SFTRST,
 *          RCC_FLAG_WWDGRST, RCC_FLAG_LPWRRST.
 *
 * @return  none
 */
void RCC_ClearFlag(void)
{
    RCC->RSTSCKR |= RSTSCKR_RMVF_Set;
}

/*********************************************************************
 * @fn      RCC_GetITStatus
 *
 * @brief   Checks whether the specified RCC interrupt has occurred or not.
 *
 * @param   RCC_IT - specifies the RCC interrupt source to check.
 *            RCC_IT_LSIRDY - LSI ready interrupt.
 *            RCC_IT_HSIRDY - HSI ready interrupt.
 *            RCC_IT_HSERDY - HSE ready interrupt.
 *            RCC_IT_PLLRDY - PLL ready interrupt.
 *            RCC_IT_CSS - Clock Security System interrupt.
 *
 * @return  ITStatus - SET or RESET.
 */
ITStatus RCC_GetITStatus(uint8_t RCC_IT)
{
    ITStatus bitstatus = RESET;

    if((RCC->INTR & RCC_IT) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      RCC_ClearITPendingBit
 *
 * @brief   Clears the RCC's interrupt pending bits.
 *
 * @param   RCC_IT - specifies the interrupt pending bit to clear.
 *            RCC_IT_LSIRDY - LSI ready interrupt.
 *            RCC_IT_HSIRDY - HSI ready interrupt.
 *            RCC_IT_HSERDY - HSE ready interrupt.
 *            RCC_IT_PLLRDY - PLL ready interrupt.
 *            RCC_IT_CSS - Clock Security System interrupt.
 *
 * @return  none
 */
void RCC_ClearITPendingBit(uint8_t RCC_IT)
{
    *(__IO uint8_t *)INTR_BYTE3_ADDRESS = RCC_IT;
}

/*********************************************************************
 * @fn      RCC_ADCCLKDutyCycleConfig
 *
 * @brief   Configures the ADC clock high level duty cycle.
 *
 * @param   RCC_DutyCycle - high level duty cycle.
 *            RCC_ADC_H_Level_Mode0 - ADC clock high-level duty cycle is mode 0.
 *            RCC_ADC_H_Level_Mode1 - ADC clock high-level duty cycle is mode 1.
 *
 * @return  none
 */
void RCC_ADCCLKDutyCycleConfig(uint32_t RCC_DutyCycle)
{
    RCC->CFGR0 &= ~(1<<28);
    RCC->CFGR0 |= RCC_DutyCycle;
}

