/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32x3x5_rcc.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : This file contains all the functions prototypes for the 
*                      RCC firmware library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH32X3x5_RCC_H
#define __CH32X3x5_RCC_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "ch32x3x5.h"

/* RCC_Exported_Types */
typedef struct
{
  uint32_t SYSCLK_Frequency;  /* returns SYSCLK clock frequency expressed in Hz */
  uint32_t HCLK_Frequency;    /* returns HCLK clock frequency expressed in Hz */
  uint32_t Core_Frequency;   /* returns Core clock frequency expressed in Hz */
  uint32_t ADCCLK_Frequency;  /* returns ADCCLK clock frequency expressed in Hz */
}RCC_ClocksTypeDef;

/* HSE_configuration */
#define RCC_HSE_OFF                      ((uint32_t)0x00000000)
#define RCC_HSE_ON                       ((uint32_t)0x00010000)
#define RCC_HSE_Bypass                   ((uint32_t)0x00040000)

/* RCC_PLLSource */
#define RCC_PLLSource_USBHS              ((uint32_t)0x00000000)
#define RCC_PLLSource_PLLHS              ((uint32_t)0x00200000)
#define RCC_PLLSource_USBSS              ((uint32_t)0x00400000)

/* RCC_PLLHSSource */
#define RCC_PLLHSSource_USBSS_125M       ((uint32_t)0x00000000)
#define RCC_PLLHSSource_USBHS_357M       ((uint32_t)0x10000000)
#define RCC_PLLHSSource_USBSS_625M       ((uint32_t)0x20000000) /* Only applicable to chips with a batch number greater than zero in the fifth digit */
#define RCC_PLLHSSource_USBHS_480M       ((uint32_t)0x30000000)

/* RCC_PLLDiv */
#define RCC_PLLDiv_2                     ((uint32_t)0x00020000)
#define RCC_PLLDiv_3                     ((uint32_t)0x00040000)
#define RCC_PLLDiv_4                     ((uint32_t)0x00060000)
#define RCC_PLLDiv_5                     ((uint32_t)0x00080000)
#define RCC_PLLDiv_6                     ((uint32_t)0x000A0000)
#define RCC_PLLDiv_7                     ((uint32_t)0x000C0000)
#define RCC_PLLDiv_8                     ((uint32_t)0x000E0000)
#define RCC_PLLDiv_9                     ((uint32_t)0x00100000)
#define RCC_PLLDiv_10                    ((uint32_t)0x00120000)
#define RCC_PLLDiv_11                    ((uint32_t)0x00140000)
#define RCC_PLLDiv_12                    ((uint32_t)0x00160000)
#define RCC_PLLDiv_13                    ((uint32_t)0x00180000)
#define RCC_PLLDiv_14                    ((uint32_t)0x001A0000)
#define RCC_PLLDiv_15                    ((uint32_t)0x001C0000)
#define RCC_PLLDiv_16                    ((uint32_t)0x001E0000)

/* System_clock_source */
#define RCC_SYSCLKSource_HSI             ((uint32_t)0x00000000)
#define RCC_SYSCLKSource_HSE             ((uint32_t)0x00000001)
#define RCC_SYSCLKSource_PLLCLK          ((uint32_t)0x00000002)

/* HB_clock_source */
#define RCC_SYSCLK_Div1                  ((uint32_t)0x00000000)
#define RCC_SYSCLK_Div2                  ((uint32_t)0x00000080)
#define RCC_SYSCLK_Div4                  ((uint32_t)0x00000090)
#define RCC_SYSCLK_Div8                  ((uint32_t)0x000000A0)
#define RCC_SYSCLK_Div16                 ((uint32_t)0x000000B0)
#define RCC_SYSCLK_Div64                 ((uint32_t)0x000000C0)
#define RCC_SYSCLK_Div128                ((uint32_t)0x000000D0)
#define RCC_SYSCLK_Div256                ((uint32_t)0x000000E0)
#define RCC_SYSCLK_Div512                ((uint32_t)0x000000F0)

/* RCC_CoreCLK_Div */
#define RCC_CoreCLK_Div1                  ((uint32_t)0x00000000)
#define RCC_CoreCLK_Div2                  ((uint32_t)0x00000800)


/* RCC_Interrupt_source */
#define RCC_IT_LSIRDY                    ((uint8_t)0x01)
#define RCC_IT_HSIRDY                    ((uint8_t)0x04)
#define RCC_IT_HSERDY                    ((uint8_t)0x08)
#define RCC_IT_CSSF                      ((uint8_t)0x80)

/* RCC_ADCPRE_Div */
#define RCC_ADCPRE_Div2                  ((uint32_t)0x00000000)
#define RCC_ADCPRE_Div4                  ((uint32_t)0x00004000)
#define RCC_ADCPRE_Div6                  ((uint32_t)0x00008000)
#define RCC_ADCPRE_Div8                  ((uint32_t)0x0000C000)

/* RCC_PPRE2_Div */
#define RCC_PPRE2_Div1                   ((uint32_t)0x00000000)
#define RCC_PPRE2_Div2                   ((uint32_t)0x00002000)
#define RCC_PPRE2_Div4                   ((uint32_t)0x00002800)
#define RCC_PPRE2_Div8                   ((uint32_t)0x00003000)
#define RCC_PPRE2_Div16                  ((uint32_t)0x00003800)

/* HB_peripheral */
#define RCC_HBPeriph_DMA1                ((uint32_t)0x00000001)
#define RCC_HBPeriph_CRC                 ((uint32_t)0x00000040)
#define RCC_HBPeriph_USBHS               ((uint32_t)0x00000800)
#define RCC_HBPeriph_USBPD               ((uint32_t)0x00001000)
#define RCC_HBPeriph_ARGB                ((uint32_t)0x00004000)
#define RCC_HBPeriph_USBSS               ((uint32_t)0x00008000)

/* HB2_peripheral */
#define RCC_HB2Periph_AFIO               ((uint32_t)0x00000001)
#define RCC_HB2Periph_GPIOA              ((uint32_t)0x00000004)
#define RCC_HB2Periph_GPIOB              ((uint32_t)0x00000008)
#define RCC_HB2Periph_GPIOC              ((uint32_t)0x00000010)
#define RCC_HB2Periph_GPIOD              ((uint32_t)0x00000020)
#define RCC_HB2Periph_ADC1               ((uint32_t)0x00000200)
#define RCC_HB2Periph_ADC2               ((uint32_t)0x00000400)
#define RCC_HB2Periph_TIM1               ((uint32_t)0x00000800)
#define RCC_HB2Periph_SPI1               ((uint32_t)0x00001000)
#define RCC_HB2Periph_USART1             ((uint32_t)0x00004000)
#define RCC_HB2Periph_ADC3               ((uint32_t)0x00008000)
#define RCC_HB2Periph_ADC4               ((uint32_t)0x00010000)

/* HB1_peripheral */
#define RCC_HB1Periph_TIM2               ((uint32_t)0x00000001)
#define RCC_HB1Periph_TIM3               ((uint32_t)0x00000002)
#define RCC_HB1Periph_TIM4               ((uint32_t)0x00000004)
#define RCC_HB1Periph_WWDG               ((uint32_t)0x00000800)
#define RCC_HB1Periph_SPI2               ((uint32_t)0x00004000)
#define RCC_HB1Periph_SPI3               ((uint32_t)0x00008000)
#define RCC_HB1Periph_USART2             ((uint32_t)0x00020000)
#define RCC_HB1Periph_USART3             ((uint32_t)0x00040000)
#define RCC_HB1Periph_USART4             ((uint32_t)0x00080000)
#define RCC_HB1Periph_I2C1               ((uint32_t)0x00200000)
#define RCC_HB1Periph_I2C2               ((uint32_t)0x00400000)
#define RCC_HB1Periph_PWR                ((uint32_t)0x10000000)

/* Clock_source_to_output_on_MCO_pin */
#define RCC_MCO_NoClock                  ((uint8_t)0x00)
#define RCC_MCO_SYSCLK                   ((uint8_t)0x04)
#define RCC_MCO_HSI                      ((uint8_t)0x05)
#define RCC_MCO_HSE                      ((uint8_t)0x06)
#define RCC_MCO_PLLCLK_Div8              ((uint8_t)0x07)

/* RCC_Flag */
#define RCC_FLAG_HSIRDY                  ((uint8_t)0x21)
#define RCC_FLAG_HSERDY                  ((uint8_t)0x31)
#define RCC_FLAG_USBHSPLLRDY             ((uint8_t)0x39)
#define RCC_FLAG_USBSSPLLRDY             ((uint8_t)0x3B)
#define RCC_FLAG_XI_STAT                 ((uint8_t)0x3D)
#define RCC_FLAG_LSIRDY                  ((uint8_t)0x41)
#define RCC_FLAG_LKUPRST                 ((uint8_t)0x52)
#define RCC_FLAG_ADC3AWDRST              ((uint8_t)0x53)
#define RCC_FLAG_ADC4AWDRST              ((uint8_t)0x54)
#define RCC_FLAG_USBPDRST                ((uint8_t)0x55)
#define RCC_FLAG_ADC1AWDRST              ((uint8_t)0x56)
#define RCC_FLAG_ADC2AWDRST              ((uint8_t)0x57)
#define RCC_FLAG_PINRST                  ((uint8_t)0x5A)
#define RCC_FLAG_PORRST                  ((uint8_t)0x5B)
#define RCC_FLAG_SFTRST                  ((uint8_t)0x5C)
#define RCC_FLAG_IWDGRST                 ((uint8_t)0x5D)
#define RCC_FLAG_WWDGRST                 ((uint8_t)0x5E)

/* SysTick_clock_source */
#define SysTick_CLKSource_HCLK_Div8      ((uint32_t)0xFFFFFFFB)
#define SysTick_CLKSource_HCLK           ((uint32_t)0x00000004)

/* ADC_clock_source */
#define RCC_ADCCLKSource_HCLK            ((uint32_t)0x00000000)
#define RCC_ADCCLKSource_USBHSPLL        ((uint32_t)0x01000000)
#define RCC_ADCCLKSource_USBSSPLL        ((uint32_t)0x09000000)

/* ADC_clock_Duty_Cycle */
#define RCC_DutyCycle_50                 ((uint32_t)0x00000000)
#define RCC_DutyCycle_75                 ((uint32_t)0x02000000)
#define RCC_DutyCycle_62_5               ((uint32_t)0x04000000)

/* RCC_ADCPLLCLK_DIV */
#define RCC_ADCPLLCLK_DIV1               ((uint32_t)0x00000000)
#define RCC_ADCPLLCLK_DIV2               ((uint32_t)0x10000000)
#define RCC_ADCPLLCLK_DIV3               ((uint32_t)0x20000000)
#define RCC_ADCPLLCLK_DIV4               ((uint32_t)0x30000000)
#define RCC_ADCPLLCLK_DIV5               ((uint32_t)0x40000000)
#define RCC_ADCPLLCLK_DIV6               ((uint32_t)0x50000000)
#define RCC_ADCPLLCLK_DIV7               ((uint32_t)0x60000000)
#define RCC_ADCPLLCLK_DIV8               ((uint32_t)0x70000000)
#define RCC_ADCPLLCLK_DIV9               ((uint32_t)0x80000000)
#define RCC_ADCPLLCLK_DIV10              ((uint32_t)0x90000000)
#define RCC_ADCPLLCLK_DIV11              ((uint32_t)0xA0000000)
#define RCC_ADCPLLCLK_DIV12              ((uint32_t)0xB0000000)
#define RCC_ADCPLLCLK_DIV13              ((uint32_t)0xC0000000)
#define RCC_ADCPLLCLK_DIV14              ((uint32_t)0xD0000000)
#define RCC_ADCPLLCLK_DIV15              ((uint32_t)0xE0000000)
#define RCC_ADCPLLCLK_DIV16              ((uint32_t)0xF0000000)

/* RCC_USBHSPLLSource */
#define  RCC_USBHSPLLSource_HSE          ((uint32_t)0x00000000)
#define  RCC_USBHSPLLSource_HSI          ((uint32_t)0x00000100)
#define  RCC_USBHSPLLSource_USBSS_Div25  ((uint32_t)0x00000040)

/* RCC_USBHSPLLRefer */
#define  RCC_USBHSPLLRefer_12M           ((uint32_t)0x00000000)
#define  RCC_USBHSPLLRefer_20M           ((uint32_t)0x00000001)  
#define  RCC_USBHSPLLRefer_24M           ((uint32_t)0x00000002) 
#define  RCC_USBHSPLLRefer_25M           ((uint32_t)0x00000003) 
#define  RCC_USBHSPLLRefer_32M           ((uint32_t)0x00000004) 

/* RCC_USBSSPLLRefer */
#define  RCC_USBSSPLLRefer_20M           ((uint32_t)0x00000000)
#define  RCC_USBSSPLLRefer_24M           ((uint32_t)0x00000008)
#define  RCC_USBSSPLLRefer_25M           ((uint32_t)0x00000010)
#define  RCC_USBSSPLLRefer_30M           ((uint32_t)0x00000018)
#define  RCC_USBSSPLLRefer_32M           ((uint32_t)0x00000020)
#define  RCC_USBSSPLLRefer_40M           ((uint32_t)0x00000028)
#define  RCC_USBSSPLLRefer_60M           ((uint32_t)0x00000030)
#define  RCC_USBSSPLLRefer_80M           ((uint32_t)0x00000038)

/* RCC_USBSSPLLSource */
#define  RCC_USBSSPLLSource_HSE          ((uint32_t)0x00000000)
#define  RCC_USBSSPLLSource_HSI          ((uint32_t)0x00000200)
#define  RCC_USBSSPLLSource_USBHS_Div24  ((uint32_t)0x00000080)

/* RCC_HSERDY_Time_Mode */
#define RCC_HSERDY_Time_Mode0            ((uint32_t)0x00000000)
#define RCC_HSERDY_Time_Mode1            ((uint32_t)0x00000001)
#define RCC_HSERDY_Time_Mode2            ((uint32_t)0x00000002)
#define RCC_HSERDY_Time_Mode3            ((uint32_t)0x00000003)

/* RCC_USBHSPLLRDY_Time_Mode */
#define RCC_USBHSPLLRDY_Time_Mode0       ((uint32_t)0x00000000)
#define RCC_USBHSPLLRDY_Time_Mode1       ((uint32_t)0x00000001)
#define RCC_USBHSPLLRDY_Time_Mode2       ((uint32_t)0x00000002)
#define RCC_USBHSPLLRDY_Time_Mode3       ((uint32_t)0x00000003)

/* RCC_USBSSPLLRDY_Time_Mode */
#define RCC_USBSSPLLRDY_Time_Mode0       ((uint32_t)0x00000000)
#define RCC_USBSSPLLRDY_Time_Mode1       ((uint32_t)0x00000001)
#define RCC_USBSSPLLRDY_Time_Mode2       ((uint32_t)0x00000002)
#define RCC_USBSSPLLRDY_Time_Mode3       ((uint32_t)0x00000003)

void RCC_DeInit(void);
void RCC_HSEConfig(uint32_t RCC_HSE);
ErrorStatus RCC_WaitForHSEStartUp(void);
void RCC_HSICmd(FunctionalState NewState);
void RCC_PLLConfig(uint32_t RCC_PLLSource, uint32_t RCC_PLLDiv);
void RCC_PLLHSConfig(uint32_t RCC_PLLHSSource);
void RCC_PLLGATECmd(FunctionalState NewState);
void RCC_SYSCLKConfig(uint32_t RCC_SYSCLKSource);
uint8_t RCC_GetSYSCLKSource(void);
void RCC_CoreCLKConfig(uint32_t RCC_SYSCLK_Div);
void RCC_HCLKConfig(uint32_t RCC_SYSCLK);
void RCC_ITConfig(uint8_t RCC_IT, FunctionalState NewState);
void RCC_ADCCLKConfig(uint32_t RCC_ADCE, uint32_t RCC_PPR2);
void RCC_LSICmd(FunctionalState NewState);
void RCC_GetClocksFreq(RCC_ClocksTypeDef *RCC_Clocks);
void RCC_HBPeriphClockCmd(uint32_t RCC_HBPeriph, FunctionalState NewState);
void RCC_HB2PeriphClockCmd(uint32_t RCC_HB2Periph, FunctionalState NewState);
void RCC_HB1PeriphClockCmd(uint32_t RCC_HB1Periph, FunctionalState NewState);
void RCC_HB2PeriphResetCmd(uint32_t RCC_HB2Periph, FunctionalState NewState);
void RCC_HB1PeriphResetCmd(uint32_t RCC_HB1Periph, FunctionalState NewState);
void RCC_ClockSecuritySystemCmd(FunctionalState NewState);
void RCC_MCOConfig(uint8_t RCC_MCO);
FlagStatus RCC_GetFlagStatus(uint8_t RCC_FLAG);
void RCC_ClearFlag(void);
ITStatus RCC_GetITStatus(uint8_t RCC_IT);
void RCC_ClearITPendingBit(uint8_t RCC_IT);
void RCC_HBPeriphResetCmd(uint32_t RCC_HBPeriph, FunctionalState NewState);
void RCC_ADCCLKSourceConfig(uint32_t RCC_ADCCLKSource);
void RCC_ADCCLKDutyCycleConfig(uint32_t RCC_DutyCycle);
void RCC_ADCClockSourceUSBConfig(uint32_t RCC_ADCPLLCLK_DIV);
void RCC_USBHS_PLLCmd(FunctionalState NewState);
void RCC_USBSS_PLLCmd(FunctionalState NewState);
void RCC_PIPECmd(FunctionalState NewState);
void RCC_USBHS_USBSSPLLCLKConfig(uint32_t RCC_USBHSPLLSource,uint32_t RCC_USBSSPLLSource);
void RCC_USBHSPLLReferConfig(uint32_t RCC_USBHSPLLRefer);
void RCC_USBSSPLLReferConfig(uint32_t RCC_USBSSPLLRefer);
void RCC_UTMICmd(FunctionalState NewState);
void RCC_LPMCmd(FunctionalState NewState);
void RCC_HSERDY_TimeConfig(uint32_t RCC_HSERDY_Time_Mode);
void RCC_USBHSRDY_TimeConfig(uint32_t RCC_USBHSRDY_Time_Mode);
void RCC_USBSSRDY_TimeConfig(uint32_t RCC_USBSSRDY_Time_Mode);

#ifdef __cplusplus
}
#endif

#endif 





