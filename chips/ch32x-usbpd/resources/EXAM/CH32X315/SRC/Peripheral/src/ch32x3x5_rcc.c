/********************************** (C) COPYRIGHT *******************************
* File Name          : ch32x3x5_rcc.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : This file provides all the RCC firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32x3x5_rcc.h"

/* RCC registers bit mask */
/* CTLR register bit mask */
#define CTLR_HSEBYP_Reset          ((uint32_t)0xFFFBFFFF)
#define CTLR_HSEBYP_Set            ((uint32_t)0x00040000)
#define CTLR_HSEON_Reset           ((uint32_t)0xFFFEFFFF)
#define CTLR_HSEON_Set             ((uint32_t)0x00010000)
#define CTLR_HSITRIM_Mask          ((uint32_t)0xFFFFFF07)
#define CTLR_LPM_Mask              ((uint32_t)0xFFFFFF7F)

#define CFGR0_SWS_Mask             ((uint32_t)0x0000000C)
#define CFGR0_SW_Mask              ((uint32_t)0xFFFFFFFC)
#define CFGR0_HPRE_Reset_Mask      ((uint32_t)0xFFFFFF0F)
#define CFGR0_PPRE2_Reset_Mask     ((uint32_t)0xFFFFC7FF)
#define CFGR0_ADCPRE_Reset_Mask    ((uint32_t)0xFFFF3FFF)
#define CFGR0_HPRE_Set_Mask        ((uint32_t)0x000000F0)
#define CFGR0_FPRE_Set_Mask        ((uint32_t)0x00000100) 
#define CFGR0_PPRE2_Set_Mask       ((uint32_t)0x00003800) 
#define CFGR0_ADCPRE_Set_Mask      ((uint32_t)0x0000C000) 

/* RSTSCKR register bit mask */
#define RSTSCKR_RMVF_Set           ((uint32_t)0x01000000)

/* CFGR2 register bit mask */
#define CFGR2_ADCPRE_Reset_Mask    ((uint32_t)0xF0000000)
#define CFGR2_ADCSRC_Set_Mask      ((uint32_t)0x01000000) 
#define CFGR2_ADCPLLSRC_Set_Mask   ((uint32_t)0x08000000) 
#define CFGR2_ADCCLKDIV_Set_Mask   ((uint32_t)0xF0000000)

/* RCC Flag Mask */
#define FLAG_Mask                  ((uint8_t)0x1F)

/* INTR register byte 2 (Bits[15:8]) base address */
#define INTR_BYTE2_ADDRESS         ((uint32_t)0x40021009)

/* INTR register byte 3 (Bits[23:16]) base address */
#define INTR_BYTE3_ADDRESS         ((uint32_t)0x4002100A)

/* CFGR0 register byte 4 (Bits[31:24]) base address */
#define CFGR0_BYTE4_ADDRESS        ((uint32_t)0x40021007)

#define PLLHS_SRC_Mask             ((uint32_t)0x30000000)
#define PLLDIV_Mask                ((uint32_t)0x1E0000)

static __I uint8_t PLLDIV[16] = {6, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
static __I uint8_t ADCCLKDIV[16] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
static __I uint8_t HPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};
static __I uint8_t ADCPrescTable[4] = {2, 4, 6, 8};
static __I uint8_t PPrescTable[8] = {1, 1, 1, 1, 2, 4, 8, 16};

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
    while ((RCC->CTLR & (uint32_t)RCC_HSIRDY) != (uint32_t)RCC_HSIRDY)
    {
    }
    RCC->CFGR0 &= (uint32_t)0xFFFFFFFC;
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x00)
    {
    }
    RCC->CFGR0 &= (uint32_t)0x00000003;
    RCC->CTLR &= (uint32_t)0x7FFEFFFF;
    RCC->CTLR &= (uint32_t)0xEAF3FEFF;

    RCC->INTR = 0x008D0000;
    RCC->CFGR2 &= 0x00C0FC00;
    RCC->CFGR2 |= 0x10000000;
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
 *          Note-
 *            HSE can not be stopped if it is used directly or through the PLL as system clock.
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
 * @brief   Configures the PLL clock source and divider factor.
 *
 * @param   RCC_PLLSource - specifies the PLL entry clock source.
 *            RCC_PLLSource_USBHS(480M) - USBHS clock selected as PLL clock entry.
 *            RCC_PLLSource_USBSS(625MHz) - USBSS clock selected as PLL clock entry.
 *            RCC_PLLSource_PLLHS(480M-625MHz-357MHz-125MHz) - PLLHS clock selected
 *        as PLL clock entry.
 *          RCC_PLLDiv - specifies the PLL Division factor.
 *            This parameter can be RCC_PLLDiv_x where x:[2,16].
 *          Should close PLLGATE before use this function
 *
 * @return  none
 */
void RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t RCC_PLLDiv)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;
    tmpreg &= ~((3<<21) | (0xF<<17));
    tmpreg |= RCC_PLLSource | RCC_PLLDiv;

    RCC->CFGR0 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_PLLHSConfig
 *
 * @brief   Configures the PLLHS clock source a.
 *
 * @param   RCC_PLLSource - specifies the PLL entry clock source.
 *            RCC_PLLHSSource_USBHS_480M(480MHz) - USBHS clock selected as PLL clock entry.
 *            RCC_PLLHSSource_USBSS_625M(625MHz) - USBSS 625M clock selected as PLL clock entry.
 *            RCC_PLLHSSource_USBSS_357M(357MHz) - USBSS 357M clock selected as PLL clock entry.
 *            RCC_PLLHSSource_USBSS_125M(125MHz) - USBSS 125M clock selected as PLL clock entry.
 *          Should close PLLGATE before use this function
 *          Tips: RCC_PLLHSSource_USBSS_625M - only applicable to chips with a batch number greater than zero in the fifth digit.
 *
 * @return  none
 */
void RCC_PLLHSConfig(uint32_t RCC_PLLHSSource)
{
    uint32_t tmpreg = 0;
    tmpreg = RCC->CFGR0;
    tmpreg &= ~(3<<21);
    tmpreg |= RCC_PLLHSSource;
    RCC->CFGR0 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_PLLGATECmd
 *
 * @brief   ENABLE or DISABLE PLL clock  gate.
 *          
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_PLLGATECmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CTLR &= ~(1 << 31);
    }
    else
    {
        RCC->CTLR |= (1 << 31);
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
 * @fn      RCC_CoreCLKConfig
 *
 * @brief   Configures the Core clock.
 *
 * @param   RCC_SYSCLK_Div - defines the HB clock divider. This clock is derived from
 *        the system clock (SYSCLK).
 *            RCC_SYSCLK_Div1 - Core clock = SYSCLK.
 *            RCC_SYSCLK_Div2 - Core clock = SYSCLK/2.
 *            RCC_SYSCLK_Div4 - Core clock = SYSCLK/4.
 *            RCC_SYSCLK_Div8 - Core clock = SYSCLK/8.
 *            RCC_SYSCLK_Div16 - Core clock = SYSCLK/16.
 *            RCC_SYSCLK_Div64 - Core clock = SYSCLK/64.
 *            RCC_SYSCLK_Div128 - Core clock = SYSCLK/128.
 *            RCC_SYSCLK_Div256 - Core clock = SYSCLK/256.
 *            RCC_SYSCLK_Div512 - Core clock = SYSCLK/512.
 *
 * @return  none
 */
void RCC_CoreCLKConfig(uint32_t RCC_SYSCLK_Div)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;
    tmpreg &= CFGR0_HPRE_Reset_Mask;
    tmpreg |= RCC_SYSCLK_Div;
    RCC->CFGR0 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_HCLKConfig
 *
 * @brief   Configures the HB clock (HCLK).
 *
 * @param   RCC_CoreCLK_Div - defines the Core clock divider. This clock is derived from
 *        the Core clock.
 *            RCC_CoreCLK_Div1 - HB clock = CoreCLK.
 *            RCC_CoreCLK_Div2 - HB clock = SYSCoreCLK/2.
 *
 * @return  none
 */
void RCC_HCLKConfig(uint32_t RCC_CoreCLK_Div)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;
    tmpreg &= ~(1<<8);
    tmpreg |= RCC_CoreCLK_Div;
    RCC->CFGR0 = tmpreg;
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
 * @param   RCC_ADCPR - This clock is derived fromthe ADC clock . 
 *            RCC_ADCPRE_Div2 - ADC clock/2.
 *            RCC_ADCPRE_Div4 - ADC clock/4.
 *            RCC_ADCPRE_Div6 - ADC clock/6.
 *            RCC_ADCPRE_Div8 - ADC clock/8.
 *          RCC_PPR2 - This clock is derived fromthe ADC clock .
 *            RCC_PPRE2_Div1 - ADC clock/1.
 *            RCC_PPRE2_Div2 - ADC clock/2.
 *            RCC_PPRE2_Div4 - ADC clock/4.
 *            RCC_PPRE2_Div8 - ADC clock/8.
 *            RCC_PPRE2_Div16 - ADC clock/16.

 *
 * @return  none
 */
void RCC_ADCCLKConfig(uint32_t RCC_ADCPR, uint32_t RCC_PPR2)
{
    uint32_t tmpreg = 0;

    tmpreg = RCC->CFGR0;
    tmpreg &= CFGR0_ADCPRE_Reset_Mask|CFGR0_PPRE2_Reset_Mask;   
    tmpreg |= RCC_ADCPR | RCC_PPR2;
    RCC->CFGR0 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_LSICmd
 *
 * @brief   Enables or disables the Internal Low Speed oscillator (LSI).
 *          Note-
 *          LSI can not be disabled if the IWDG is running.
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
    uint32_t tmp = 0,tmp1 = 0, tmp2 = 0, presc = 0, presc1 = 0;

    tmp = RCC->CFGR0 & CFGR0_SWS_Mask;
    tmp1 = RCC->CFGR0 & RCC_PLLSRC;

    switch(tmp)
    {
        case 0x00:
            RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
            break;

        case 0x04:
            RCC_Clocks->SYSCLK_Frequency = HSE_VALUE;
            break;

        case 0x08:
            switch(tmp1)
            { 
                case RCC_PLLSRC_USBHSPLL_480M:
                    presc = ((RCC->CFGR0 & PLLDIV_Mask)>>17);
                    RCC_Clocks->SYSCLK_Frequency = 480000000/PLLDIV[presc];
                    break;
                case RCC_PLLSRC_USBSSPLL_625M:
                    presc = ((RCC->CFGR0 & PLLDIV_Mask)>>17);                
                    RCC_Clocks->SYSCLK_Frequency = 625000000/PLLDIV[presc];
                    break;
              case RCC_PLLSRC_HS:
              case RCC_PLLSRC_HS_1:
                    tmp = RCC->CFGR0 & PLLHS_SRC_Mask;
                    switch(tmp)
                    {
                        case RCC_PLLHS_SRC_USBSSPLL_125M:
                            RCC_Clocks->SYSCLK_Frequency = 125000000;
                            break;
                        case RCC_PLLHS_SRC_USBSSPLL_357M:    
                            RCC_Clocks->SYSCLK_Frequency = 375000000;
                            break;
                        case RCC_PLLHS_SRC_USBSSPLL_625M:    
                            RCC_Clocks->SYSCLK_Frequency = 625000000;
                            break; 
                        case RCC_PLLHS_SRC_USBHSPLL_480M:    
                            RCC_Clocks->SYSCLK_Frequency = 480000000;
                            break; 
                        default:
                            RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
                            break;                                   
                    }
                    break;    

                default:
                    RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
                    break;
            }  
            break;
        default:
            RCC_Clocks->SYSCLK_Frequency = HSI_VALUE;
            break;
    }
    
    tmp = (RCC->CFGR0 & CFGR0_HPRE_Set_Mask) >> 4;
    presc1 = HPrescTable[tmp];
    RCC_Clocks->Core_Frequency = RCC_Clocks->SYSCLK_Frequency >> presc1;

    tmp = (RCC->CFGR0 & CFGR0_FPRE_Set_Mask) >> 8;
    RCC_Clocks->HCLK_Frequency = RCC_Clocks->Core_Frequency >> tmp;
 
    tmp = RCC->CFGR2 & CFGR2_ADCSRC_Set_Mask;
    tmp1 = RCC->CFGR2 & CFGR2_ADCPLLSRC_Set_Mask;

    if((tmp == CFGR2_ADCSRC_Set_Mask))
    {
        presc = (RCC->CFGR2 & CFGR2_ADCCLKDIV_Set_Mask) >> 28;
        if(tmp1 == CFGR2_ADCPLLSRC_Set_Mask)
        {  
            tmp2 = 625000000/ADCCLKDIV[presc];
        }
        else 
        {
            tmp2 = 480000000/ADCCLKDIV[presc];
        } 
    }
    else
    {
        tmp2 = RCC_Clocks->HCLK_Frequency;
    }

    tmp = (RCC->CFGR0 &  CFGR0_PPRE2_Set_Mask) >> 11;
    tmp1 = RCC->CFGR0 &  CFGR0_ADCPRE_Set_Mask >> 14;
    RCC_Clocks->ADCCLK_Frequency = tmp2 /ADCPrescTable[tmp1]/PPrescTable[tmp];    
}

/*********************************************************************
 * @fn      RCC_HBPeriphClockCmd
 *
 * @brief   Enables or disables the HB peripheral clock.
 *
 * @param   RCC_HBPeriph - specifies the HB peripheral to gates its clock.
 *            RCC_HBPeriph_DMA1.
 *            RCC_HBPeriph_CRC.
 *            RCC_HBPeriph_USBHS
 *            RCC_HBPeriph_USBPD 
 *            RCC_HBPeriph_ARGB
 *            RCC_HBPeriph_USBSS
 *          Note-
 *          SRAM clock can be disabled only during sleep mode.
 *          NewState: ENABLE or DISABLE.
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
 * @fn      RCC_HB2PeriphClockCmd
 *
 * @brief   Enables or disables the High Speed HB (HB2) peripheral clock.
 *
 * @param   RCC_HB2Periph - specifies the HB2 peripheral to gates its clock.
 *            RCC_HB2Periph_AFIO.
 *            RCC_HB2Periph_GPIOA.
 *            RCC_HB2Periph_GPIOB.
 *            RCC_HB2Periph_GPIOC.
 *            RCC_HB2Periph_GPIOD.
 *            RCC_HB2Periph_ADC1.
 *            RCC_HB2Periph_ADC2
 *            RCC_HB2Periph_TIM1.
 *            RCC_HB2Periph_SPI1.
 *            RCC_HB2Periph_USART1.
 *            RCC_HB2Periph_ADC3
 *            RCC_HB2Periph_ADC4
 *          NewState - ENABLE or DISABLE
 *
 * @return  none
 */
void RCC_HB2PeriphClockCmd(uint32_t RCC_HB2Periph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->HB2PCENR|= RCC_HB2Periph;
    }
    else
    {
         RCC->HB2PCENR &= ~RCC_HB2Periph;
    }
}

/*********************************************************************
 * @fn      RCC_HB1PeriphClockCmd
 *
 * @brief   Enables or disables the Low Speed HB (HB1) peripheral clock.
 *
 * @param   RCC_HB1Periph - specifies the HB1 peripheral to gates its clock.
 *            RCC_HB1Periph_TIM2.
 *            RCC_HB1Periph_TIM3.
 *            RCC_HB1Periph_TIM4.
 *            RCC_HB1Periph_WWDG.
 *            RCC_HB1Periph_SPI2.
 *            RCC_HB1Periph_SPI3.
 *            RCC_HB1Periph_USART2.
 *            RCC_HB1Periph_USART3.
 *            RCC_HB1Periph_USART4
 *            RCC_HB1Periph_I2C1.
 *            RCC_HB1Periph_I2C2.
 *            RCC_HB1Periph_PWR.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_HB1PeriphClockCmd(uint32_t RCC_HB1Periph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
         RCC->HB1PCENR |= RCC_HB1Periph;
    }
    else
    {
         RCC->HB1PCENR &= ~RCC_HB1Periph;
    }
}

/*********************************************************************
 * @fn      RCC_HB2PeriphResetCmd
 *
 * @brief   Forces or releases High Speed HB (HB2) peripheral reset.
 *
 * @param   RCC_HB2Periph - specifies the HB2 peripheral to reset.
 *            RCC_HB2Periph_AFIO.
 *            RCC_HB2Periph_GPIOA.
 *            RCC_HB2Periph_GPIOB.
 *            RCC_HB2Periph_GPIOC.
 *            RCC_HB2Periph_GPIOD.
 *            RCC_HB2Periph_ADC1.
 *            RCC_HB2Periph_ADC2
 *            RCC_HB2Periph_TIM1.
 *            RCC_HB2Periph_SPI1.
 *            RCC_HB2Periph_USART1.
 *            RCC_HB2Periph_ADC3
 *            RCC_HB2Periph_ADC4
 *          NewState - ENABLE or DISABLE
 *
 * @return  none
 */
void RCC_HB2PeriphResetCmd(uint32_t RCC_HB2Periph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
         RCC->HB2PRSTR |= RCC_HB2Periph;
    }
    else
    {
         RCC->HB2PRSTR &= ~RCC_HB2Periph;
    }
}

/*********************************************************************
 * @fn      RCC_HB1PeriphResetCmd
 *
 * @brief   Forces or releases Low Speed HB (HB1) peripheral reset.
 *
 * @param   RCC_HB1Periph - specifies the HB1 peripheral to reset.
 *            RCC_HB1Periph_TIM2.
 *            RCC_HB1Periph_TIM3.
 *            RCC_HB1Periph_TIM4.
 *            RCC_HB1Periph_WWDG.
 *            RCC_HB1Periph_SPI2.
 *            RCC_HB1Periph_SPI3.
 *            RCC_HB1Periph_USART2.
 *            RCC_HB1Periph_USART3.
 *            RCC_HB1Periph_USART4
 *            RCC_HB1Periph_I2C1.
 *            RCC_HB1Periph_I2C2.
 *            RCC_HB1Periph_PWR.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_HB1PeriphResetCmd(uint32_t RCC_HB1Periph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->HB1PRSTR |= RCC_HB1Periph;
    }
    else
    {
        RCC->HB1PRSTR &= ~RCC_HB1Periph;
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
 *            RCC_MCO_PLLCLK_Div8 - PLL clock divided by 8 selected.
 *
 * @return  none
 */
void RCC_MCOConfig(uint8_t RCC_MCO)
{
    *(__IO uint8_t *)CFGR0_BYTE4_ADDRESS &= ~0xF;
    *(__IO uint8_t *)CFGR0_BYTE4_ADDRESS |= RCC_MCO;
}

/*********************************************************************
 * @fn      RCC_GetFlagStatus
 *
 * @brief   Checks whether the specified RCC flag is set or not.
 *
 * @param   RCC_FLAG - specifies the flag to check.
 *            RCC_FLAG_HSIRDY - HSI oscillator clock ready.
 *            RCC_FLAG_HSERDY - HSE oscillator clock ready.
 *            RCC_FLAG_USBHSPLLRDY - USBHS PLL clock ready.
 *            RCC_FLAG_USBSSPLLRDY - USBSS PLL clock ready.
 *            RCC_FLAG_XI_STAT - XI_XO ready.
 *            RCC_FLAG_LSIRDY - LSI oscillator clock ready.
 *            RCC_FLAG_LKUPRST - Lock up reset.
 *            RCC_FLAG_ADC3AWDRST - ADC3 Watchdog reset.
 *            RCC_FLAG_ADC4AWDRST - ADC4 Watchdog reset.
 *            RCC_FLAG_USBPDRST - USBPD reset.
 *            RCC_FLAG_ADC1AWDRST - ADC1 Watchdog reset.
 *            RCC_FLAG_ADC2AWDRST - ADC2 Watchdog reset.
 *            RCC_FLAG_PINRST - Pin reset.
 *            RCC_FLAG_PORRST - POR/PDR reset.
 *            RCC_FLAG_SFTRST - Software reset.
 *            RCC_FLAG_IWDGRST - Independent Watchdog reset.
 *            RCC_FLAG_WWDGRST - Window Watchdog reset.
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
 *          The reset flags are: RCC_FLAG_PINRST, RCC_FLAG_PORRST, RCC_FLAG_SFTRST,
 *        RCC_FLAG_IWDGRST, RCC_FLAG_WWDGRST, RCC_FLAG_LKUPRST,RCC_FLAG_ADC1AWDRST,
 *        RCC_FLAG_ADC2AWDRST,RCC_FLAG_ADC3AWDRST,RCC_FLAG_ADC4AWDRST;  
 *  
 * @return  none
 */
void RCC_ClearFlag(void)
{
    RCC->RSTSCKR |= RSTSCKR_RMVF_Set;
    __NOP();
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
 *            RCC_IT_CSSF - Clock Security System interrupt. 
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
 *            RCC_IT_CSSF - Clock Security System interrupt.
 *
 * @return  none
 */
void RCC_ClearITPendingBit(uint8_t RCC_IT)
{
    *(__IO uint8_t *)INTR_BYTE3_ADDRESS = RCC_IT;
}

/*********************************************************************
 * @fn      RCC_HBPeriphResetCmd
 *
 * @brief   Forces or releases HB peripheral reset.
 *
 * @param   RCC_HBPeriph - specifies the HB peripheral to reset.
 *            RCC_HBPeriph_USBHS
 *            RCC_HBPeriph_USBPD 
 *            RCC_HBPeriph_ARGB
 *            RCC_HBPeriph_USBSS
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_HBPeriphResetCmd(uint32_t RCC_HBPeriph, FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        RCC->HBRSTR |= RCC_HBPeriph;
    }
    else
    {
        RCC->HBRSTR &= ~RCC_HBPeriph;
    }
}

/*********************************************************************
 * @fn      RCC_ADCCLKSourceConfig
 *
 * @brief   Configures the ADC clock source.
 *
 * @param   RCC_ADCCLKSource - specifies the RNG clock source.
 *            RCC_ADCCLKSource_HCLK -  HCLK clock selected as ADC clock entry
 *            RCC_ADCCLKSource_USBHSPLL - USBHS PLL clock(480M) selected as ADC clock entry
 *            RCC_ADCCLKSource_USBSSPLL - USBSS PLL clock(625M)  selected as ADC clock entry
 *
 * @return  none
 */
void RCC_ADCCLKSourceConfig(uint32_t RCC_ADCCLKSource)
{
    uint32_t tmp = 0;

    tmp = RCC->CFGR2;
    tmp &= ~RCC_ADCCLKSource_USBSSPLL;
    tmp |= RCC_ADCCLKSource;
    RCC->CFGR2 = tmp;
}

/*********************************************************************
 * @fn      RCC_ADCCLKDutyCycleConfig
 *
 * @brief   Configures the ADC clock high level duty cycle.
 *
 * @param   RCC_DutyCycle - high level duty cycle.
 *            RCC_DutyCycle_50 - ADC clock 50% duty cycle .
 *            RCC_DutyCycle_62_5 -ADC clock 62_5% duty cycle .
 *            RCC_DutyCycle_75 -ADC clock 75% duty cycle .
 *
 * @return  none
 */
void RCC_ADCCLKDutyCycleConfig(uint32_t RCC_DutyCycle)
{
    RCC->CFGR2 &= ~(3<<25);
    RCC->CFGR2 |= RCC_DutyCycle;
}

/*********************************************************************
 * @fn      RCC_ADCCLKSourceUSBConfig
 *
 * @brief   Configures the ADC clock (ADCCLK).
 *
 * @param   RCC_ADCPLLCLK_DIV - defines the ADC clock divider. This clock is derived from
 *        the ADCPLL clock .
 *          RCC_ADCPLLCLK_DIV1 - ADC clock = ADCPLL.
 *          RCC_ADCPLLCLK_DIV2 - ADC clock = ADCPLL/2.
 *          RCC_ADCPLLCLK_DIV3 - ADC clock = ADCPLL/3.
 *          RCC_ADCPLLCLK_DIV4 - ADC clock = ADCPLL/4.
 *          RCC_ADCPLLCLK_DIV5 - ADC clock = ADCPLL/5.
 *          RCC_ADCPLLCLK_DIV6 - ADC clock = ADCPLL/6.
 *          RCC_ADCPLLCLK_DIV7 - ADC clock = ADCPLL/7.
 *          RCC_ADCPLLCLK_DIV8 - ADC clock = ADCPLL/8.
 *          RCC_ADCPLLCLK_DIV9 - ADC clock = ADCPLL/9.
 *          RCC_ADCPLLCLK_DIV10 - ADC clock = ADCPLL/10.
 *          RCC_ADCPLLCLK_DIV11 - ADC clock = ADCPLL/11.
 *          RCC_ADCPLLCLK_DIV12 - ADC clock = ADCPLL/12.
 *          RCC_ADCPLLCLK_DIV13 - ADC clock = ADCPLL/13.
 *          RCC_ADCPLLCLK_DIV14 - ADC clock = ADCPLL/14.
 *          RCC_ADCPLLCLK_DIV15 - ADC clock = ADCPLL/15.
 *          RCC_ADCPLLCLK_DIV16 - ADC clock = ADCPLL/16.
 *
 * @return  none
 */
void RCC_ADCClockSourceUSBConfig(uint32_t RCC_ADCPLLCLK_DIV)
{
    uint32_t tmpreg = 0;
    tmpreg = RCC->CFGR2;
    tmpreg &= ~CFGR2_ADCPRE_Reset_Mask;
    tmpreg |= RCC_ADCPLLCLK_DIV;
    RCC->CFGR2 = tmpreg;
}

/*********************************************************************
 * @fn      RCC_USBHS_PLLCmd
 *
 * @brief   ENABLE or DISABLE USBHS PLL Clock.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_USBHS_PLLCmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CTLR |= RCC_USBHSPLLON;
    }
    else
    {
        RCC->CTLR &= ~RCC_USBHSPLLON;
    }
}

/*********************************************************************
 * @fn      RCC_USBSS_PLLCmd
 *
 * @brief   ENABLE or DISABLE USBSS PLL Clock.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_USBSS_PLLCmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CTLR |= RCC_USBSSPLLON;
    }
    else
    {
        RCC->CTLR &= ~RCC_USBSSPLLON;
    }
}

/*********************************************************************
 * @fn      RCC_PIPEcmd
 *
 * @brief   ENABLE or DISABLE PIPE.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_PIPECmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CTLR |= RCC_PIPEON;
    }
    else
    {
        RCC->CTLR &= ~RCC_PIPEON;
    }
}

/*********************************************************************
 * @fn      RCC_USBHS_USBSSPLLCLKConfig
 *
 * @brief   Configures the USBHS/USBSS PLL clock source.
 *
 * @param   RCC_USBHS_USBSSPLLSource - the USBHS clock source.
 *            RCC_USBHSPLLSource_HSE - HSE clock selected as USBHS PLL clock entry
 *            RCC_USBHSPLLSource_HSI - HSI clock selected as USBHS PLL clock entry
 *            RCC_USBHSPLLSource_USBSS_Div25 - USBSS_Div25 selected as USBHS PLL clock entry
 *         RCC_USBSSPLLSource - the USBHS clock source.
 *            RCC_USBSSPLLSource_HSE - HSE clock selected as USBSS PLL clock entry
 *            RCC_USBSSPLLSource_HSI - HSI clock selected as USBSS PLL clock entry
 *            RCC_USBSSPLLSource_USBHS_Div24 - USBHS_Div24 selected as USBSS PLL clock entry
 *         note: RCC_USBHSPLLSource_USBSS_Div25 and RCC_USBSSPLLSource_USBHS_Div24 cannot be used simultaneously
 * @return  none
 */
void RCC_USBHS_USBSSPLLCLKConfig(uint32_t RCC_USBHSPLLSource, uint32_t RCC_USBSSPLLSource)
{
    if((RCC_USBHSPLLSource == RCC_USBHSPLLSource_USBSS_Div25)&&(RCC_USBSSPLLSource == RCC_USBSSPLLSource_USBHS_Div24))return;
    
    RCC->CFGR2 &= ~(0xF<<6);
    if(RCC_USBHSPLLSource == RCC_USBHSPLLSource_HSE )
    {
        RCC->CFGR2 |= RCC_USBHSPLLSource;
    }
    else if(RCC_USBHSPLLSource == RCC_USBHSPLLSource_HSI )
    {
        RCC->CFGR2 |= RCC_USBHSPLLSource;
    }
    else if(RCC_USBHSPLLSource == RCC_USBHSPLLSource_USBSS_Div25 )
    {
        RCC->CFGR2 |= RCC_USBHSPLLSource;
    }
    if(RCC_USBSSPLLSource == RCC_USBSSPLLSource_HSE)
    {
        RCC->CFGR2 |= RCC_USBSSPLLSource;
    }
    else if(RCC_USBSSPLLSource == RCC_USBSSPLLSource_HSI)
    {
        RCC->CFGR2 |= RCC_USBSSPLLSource;
    }
    else if(RCC_USBSSPLLSource == RCC_USBSSPLLSource_USBHS_Div24)
    {
        RCC->CFGR2 |= RCC_USBSSPLLSource;
    }     
}

/*********************************************************************
 * @fn      RCC_USBHSPLLReferConfig
 *
 * @brief   Configures the USBHS PLL refer clock.
 *
 * @param   RCC_USBHSPLLRefer - the USBHS PLL refer clock.
 *            RCC_USBHSPLLRefer_12M - USBHS PLL Refer clock 12M
 *            RCC_USBHSPLLRefer_20M - USBHS PLL Refer clock 20M
 *            RCC_USBHSPLLRefer_24M  - USBHS PLL Refer clock 24M
 *            RCC_USBHSPLLRefer_25M  - USBHS PLL Refer clock 25M
 *            RCC_USBHSPLLRefer_32M - USBHS PLL Refer clock 32M
 *
 * @return  none
 */
void RCC_USBHSPLLReferConfig(uint32_t RCC_USBHSPLLRefer)
{
    RCC->CFGR2 &= ~(7<<0);
    RCC->CFGR2 |= RCC_USBHSPLLRefer;
}

/*********************************************************************
 * @fn      RCC_USBSSPLLReferConfig
 *
 * @brief   Configures the USBSS PLL refer clock.
 *
 * @param   RCC_USBSSPLLRefer - the USBSS PLL refer clock.
 *            RCC_USBSSPLLRefer_20M - USBSS PLL Refer clock 20M
 *            RCC_USBSSPLLRefer_24M - USBSS PLL Refer clock 24M
 *            RCC_USBSSPLLRefer_25M - USBSS PLL Refer clock 25M
 *            RCC_USBSSPLLRefer_30M - USBSS PLL Refer clock 30M
 *            RCC_USBSSPLLRefer_32M - USBSS PLL Refer clock 32M
 *            RCC_USBSSPLLRefer_40M - USBSS PLL Refer clock 40M
 *            RCC_USBSSPLLRefer_60M - USBSS PLL Refer clock 60M
 *            RCC_USBSSPLLRefer_80M - USBSS PLL Refer clock 80M
 *
 * @return  none
 */

void RCC_USBSSPLLReferConfig(uint32_t RCC_USBSSPLLRefer)
{
    RCC->CFGR2 &= ~(7<<3);
    RCC->CFGR2 |= RCC_USBSSPLLRefer;
}

/*********************************************************************
 * @fn      RCC_UTMICmd
 *
 * @brief   ENABLE or DISABLE UTMI.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_UTMICmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CFGR0 |= RCC_UTMION;
    }
    else
    {
        RCC->CFGR0 &= ~RCC_UTMION;
    }
}

/*********************************************************************
 * @fn      RCC_LPMCmd
 *
 * @brief   ENABLE or DISABLE PLM clock gate.
 *          
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void RCC_LPMCmd(FunctionalState NewState)
{
    if(NewState)
    {
        RCC->CTLR &= ~(1 << 8);
    }
    else
    {
        RCC->CTLR |= (1 << 8);
    }
}

/*********************************************************************
 * @fn      RCC_HSERDY_TimeConfig
 *
 * @brief   Configures the time of HSE ready.
 *
 * @param   RCC_HSERDY_Time_Mode - the time mode of HSE ready.
 *            RCC_HSERDY_Time_Mode0 - the time mode0 of HSE ready.
 *            RCC_HSERDY_Time_Mode1 - the time mode1 of HSE ready.
 *            RCC_HSERDY_Time_Mode2 - the time mode2 of HSE ready.
 *            RCC_HSERDY_Time_Mode3 - the time mode3 of HSE ready.
 *            
 * @return  none
 */
void RCC_HSERDY_TimeConfig(uint32_t RCC_HSERDY_Time_Mode)
{
    RCC->CFGR2 &= ~(3<<16);
    RCC->CFGR2 |= (RCC_HSERDY_Time_Mode << 16);
}

/*********************************************************************
 * @fn      RCC_USBHSRDY_TimeConfig
 *
 * @brief   Configures the time of USBHS PLL ready.
 *
 * @param   RCC_USBHSRDY_Time_Mode - the time mode of USBHS PLL ready.
 *            RCC_USBHSRDY_Time_Mode0 - the time mode0 of USBHS PLL ready.
 *            RCC_USBHSRDY_Time_Mode1 - the time mode1 of USBHS PLL ready.
 *            RCC_USBHSRDY_Time_Mode2 - the time mode2 of USBHS PLL ready.
 *            RCC_USBHSRDY_Time_Mode3 - the time mode3 of USBHS PLL ready.
 *            
 * @return  none
 */
void RCC_USBHSRDY_TimeConfig(uint32_t RCC_USBHSRDY_Time_Mode)
{
    RCC->CFGR2 &= ~(3<<18);
    RCC->CFGR2 |= (RCC_USBHSRDY_Time_Mode << 18);
}

/*********************************************************************
 * @fn      RCC_USBSSRDY_TimeConfig
 *
 * @brief   Configures the time of USBSS PLL ready.
 *
 * @param   RCC_USBSSRDY_Time_Mode - the time mode of USBSS PLL ready.
 *            RCC_USBSSRDY_Time_Mode0 - the time mode0 of USBSS PLL ready.
 *            RCC_USBSSRDY_Time_Mode1 - the time mode1 of USBSS PLL ready.
 *            RCC_USBSSRDY_Time_Mode2 - the time mode2 of USBSS PLL ready.
 *            RCC_USBSSRDY_Time_Mode3 - the time mode3 of USBSS PLL ready.
 *            
 * @return  none
 */
void RCC_USBSSRDY_TimeConfig(uint32_t RCC_USBSSRDY_Time_Mode)
{
    RCC->CFGR2 &= ~(3<<20);
    RCC->CFGR2 |= (RCC_USBSSRDY_Time_Mode << 20);
}