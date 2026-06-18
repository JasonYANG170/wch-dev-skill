/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32x3x5.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : CH32X3x5 Device Peripheral Access Layer Header File.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/  
#ifndef __CH32X3x5_H
#define __CH32X3x5_H

#ifdef __cplusplus
 extern "C" {
#endif 
  
#define __MPU_PRESENT             0 /* Other CH32 devices does not provide an MPU */
#define __Vendor_SysTickConfig    0 /* Set to 1 if different SysTick Config is used */	 
	 
#ifndef HSE_VALUE
#define HSE_VALUE    ((uint32_t)20000000) /* Value of the External oscillator in Hz */
#endif

/* In the following line adjust the External High Speed oscillator (HSE) Startup Timeout value */
#define HSE_STARTUP_TIMEOUT   ((uint16_t)0x1000000) /* Time out for HSE start up */

#define HSI_VALUE    ((uint32_t)20000000) /* Value of the Internal oscillator in Hz */

/* CH32X3x5 Standard Peripheral Library version number */
#define __CH32X3x5_STDPERIPH_VERSION_MAIN   (0x01) /* [15:8] main version */
#define __CH32X3x5_STDPERIPH_VERSION_SUB    (0x00) /* [7:0] sub version */
#define __CH32X3x5_STDPERIPH_VERSION        ( (__CH32X3x5_STDPERIPH_VERSION_MAIN << 8)\
                                             |(__CH32X3x5_STDPERIPH_VERSION_SUB << 0))


/* Interrupt Number Definition, according to the selected device */	 
typedef enum IRQn
{
 /******  RISC-V Processor Exceptions Numbers *******************************************************/
  NonMaskableInt_IRQn         = 2,       /* 2 Non Maskable Interrupt                             */
  HardFault_IRQn              = 3,       /* 3 Exception Interrupt                                */
  Ecall_M_Mode_IRQn           = 5,       /* 5 Ecall M Mode Interrupt                             */
  Ecall_U_Mode_IRQn           = 8,       /* 8 Ecall U Mode Interrupt                             */
  Break_Point_IRQn            = 9,       /* 9 Break Point Interrupt                              */
  SysTick_IRQn                = 12,      /* 12 System timer Interrupt                            */
  Software_IRQn               = 14,      /* 14 software Interrupt                                */

 /******  RISC-V specific Interrupt Numbers *********************************************************/
  WWDG_IRQn                   = 16,      /* Window WatchDog Interrupt                            */
  EXTI15_8_IRQn               = 17,      /* EXTI Line[15:8] Interrupt                            */
  FLASH_IRQn                  = 18,      /* FLASH global Interrupt                               */
  RCC_IRQn                    = 19,      /* RCC global Interrupt                                 */
  EXTI7_0_IRQn                = 20,      /* EXTI Line[7:0] Interrupt                             */
  SPI2_IRQn                   = 21,      /* SPI2 global Interrupt                                */
  DMA1_Channel1_IRQn          = 22,      /* DMA1 Channel 1 global Interrupt                      */
  DMA1_Channel2_IRQn          = 23,      /* DMA1 Channel 2 global Interrupt                      */
  DMA1_Channel3_IRQn          = 24,      /* DMA1 Channel 3 global Interrupt                      */
  DMA1_Channel4_IRQn          = 25,      /* DMA1 Channel 4 global Interrupt                      */
  DMA1_Channel5_IRQn          = 26,      /* DMA1 Channel 5 global Interrupt                      */
  DMA1_Channel6_IRQn          = 27,      /* DMA1 Channel 6 global Interrupt                      */
  DMA1_Channel7_IRQn          = 28,      /* DMA1 Channel 7 global Interrupt                      */
  DMA1_Channel8_IRQn          = 29,      /* DMA1 Channel 8 global Interrupt                      */
  DMA1_Channel9_IRQn          = 30,      /* DMA1 Channel 9 global Interrupt                      */
  DMA1_Channel10_IRQn         = 31,      /* DMA1 Channel 10 global Interrupt                     */
  DMA1_Channel11_IRQn         = 32,      /* DMA1 Channel 11 global Interrupt                     */
  USART2_IRQn                 = 33,      /* USART2 global Interrupt                              */
  I2C1_EV_IRQn                = 34,      /* I2C1 Event Interrupt                                 */
  I2C1_ER_IRQn                = 35,      /* I2C1 Error Interrupt                                 */
  USART1_IRQn                 = 36,      /* USART1 global Interrupt                              */
  SPI1_IRQn                   = 37,      /* SPI1 global Interrupt                                */
  TIM1_UP_IRQn                = 38,      /* TIM1 Update Interrupt                                */
  TIM1_CC_IRQn                = 39,      /* TIM1 Capture Compare Interrupt                       */
  TIM1_TRG_COM_IRQn           = 40,      /* TIM1 Trigger and Commutation Interrupt               */
  TIM1_BRK_IRQn               = 41,      /* TIM1 Break Interrupt                                 */
  TIM2_IRQn                   = 42,      /* TIM2 global Interrupt                                */
  I2C2_EV_IRQn                = 43,      /* I2C2 Event Interrupt                                 */
  I2C2_ER_IRQn                = 44,      /* I2C2 Error Interrupt                                 */
  USBPD_IRQn                  = 45,      /* USBPD global Interrupt                               */
  USBPDWakeUP_IRQn            = 46,      /* USBPD wakeup Interrupt                               */
  ARGB_IRQn                   = 47,      /* ARGB global Interrupt                                */
  TIM3_IRQn                   = 48,      /* TIM3 global Interrupt                                */
  TIM4_IRQn                   = 49,      /* TIM4 global Interrupt                                */
  SPI3_IRQn                   = 50,      /* SPI3 global Interrupt                                */
  USART3_IRQn                 = 51,      /* USART3 global Interrupt                              */
  USART4_IRQn                 = 52,      /* USART4 global Interrupt                              */
  USBSS_IRQn                  = 53,      /* USBSS global Interrupt                               */
  USBSS_LINK_IRQn             = 54,      /* USBSS Link global Interrupt                          */
  USBHS_IRQn                  = 55,      /* USBHS global Interrupt                               */
  USBHSWakeup_IRQn            = 56,      /* USBHS wakeup Interrupt                               */
  USBSSWakeup_IRQn            = 57,      /* USBSS wakeup Interrupt                               */
  PVD_IRQn                    = 58,      /* PVD Interrupt                                        */
  ADC1_IRQn                   = 59,      /* ADC1 global Interrupt                                */
  ADC2_IRQn                   = 60,      /* ADC2 global Interrupt                                */
  ADC3_IRQn                   = 61,      /* ADC3 global Interrupt                                */
  RTCAlarm_IRQn               = 62,      /* RTCAlarm_IRQn global Interrupt                       */
  RTC_IRQn                    = 63,      /* RTC global Interrupt                                 */
  USART_WKUP_IRQn             = 64,      /* USART wakeup Interrupt                               */
  ADC4_IRQn                   = 65,      /* ADC4 global Interrupt                                */
} IRQn_Type;

#include <stdint.h>
#include "core_riscv.h"
#include "system_ch32x3x5.h"

/* Standard Peripheral Library old definitions (maintained for legacy purpose) */

/* Analog to Digital Converter */
typedef struct
{
  __IO uint32_t STATR;
  __IO uint32_t CTLR1;
  __IO uint32_t CTLR2;
  __IO uint32_t SAMPTR1;
  __IO uint32_t SAMPTR2;
  __IO uint32_t IOFR1;
  __IO uint32_t IOFR2;
  __IO uint32_t IOFR3;
  __IO uint32_t IOFR4;
  __IO uint32_t WDHTR;
  __IO uint32_t WDLTR;
  __IO uint32_t RSQR1;
  __IO uint32_t RSQR2;
  __IO uint32_t RSQR3;
  __IO uint32_t ISQR;
  __IO uint32_t IDATAR1;
  __IO uint32_t IDATAR2;
  __IO uint32_t IDATAR3;
  __IO uint32_t IDATAR4;
  __IO uint32_t RDATAR;
  __IO uint32_t SMPMD;
  __IO uint32_t UDP;
  __IO uint32_t CONCFG;
  __IO uint32_t SCANCNT;
} ADC_TypeDef;

/* Addressable RGB */
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint32_t DAT_ARR;
  __IO uint32_t RST_ARR;
  __IO uint32_t CCRL;
  __IO uint32_t CCRH;
  __IO uint32_t DAT_CYC;
  __IO uint32_t DATAR;
  __IO uint32_t STATR;
  __IO uint32_t CNTR;
} ARGB_TypeDef;

/* CRC Calculation Unit */
typedef struct
{
  __IO uint32_t DATAR;
  __IO uint8_t  IDATAR;
  uint8_t   RESERVED0;
  uint16_t  RESERVED1;
  __IO uint32_t CTLR;
} CRC_TypeDef;

/* DMA Channel Controller */
typedef struct
{
  __IO uint32_t CFGR;
  __IO uint32_t CNTR;
  __IO uint32_t PADDR;
  __IO uint32_t MADDR;
  __IO uint32_t M1ADDR;
} DMA_Channel_TypeDef;

/* DMA Controller */
typedef struct
{
  __IO uint32_t INTFR;
  __IO uint32_t INTFCR;
} DMA_TypeDef;

/* External Interrupt/Event Controller */
typedef struct
{
  __IO uint32_t INTENR; 
  __IO uint32_t EVENR;   
  __IO uint32_t RTENR;   
  __IO uint32_t FTENR;   
  __IO uint32_t SWIEVR;  
  __IO uint32_t INTFR;   
} EXTI_TypeDef;

/* FLASH Registers */
typedef struct
{
  __IO uint32_t ACTLR;
  __IO uint32_t KEYR;
  __IO uint32_t OBKEYR;
  __IO uint32_t STATR;
  __IO uint32_t CTLR;
  __IO uint32_t ADDR;
  uint32_t RESERVED;
  __IO uint32_t OBR;
  __IO uint32_t WPR;
  __IO uint32_t MODEKEYR;
  __IO uint32_t BOOT_MODEKEYR;
} FLASH_TypeDef;

/* Option Bytes Registers */  
typedef struct
{
  __IO uint16_t RDPR;
  __IO uint16_t USER;
  __IO uint16_t Data0;
  __IO uint16_t Data1;
  __IO uint16_t WRPR0;
  __IO uint16_t WRPR1;
  __IO uint16_t WRPR2;
  __IO uint16_t WRPR3;
} OB_TypeDef;

/* General Purpose I/O */
typedef struct
{
  __IO uint32_t CFGLR;
  __IO uint32_t CFGHR;
  __IO uint32_t INDR;
  __IO uint32_t OUTDR;
  __IO uint32_t BSHR;
  __IO uint32_t BCR;
  __IO uint32_t LCKR;
  __IO uint32_t SPEED;
} GPIO_TypeDef;

/* Alternate Function I/O */
typedef struct
{
  uint32_t RESERVED0;
  __IO uint32_t PCFR1;
  __IO uint32_t EXTICR[4];
  __IO uint32_t CR;
  uint32_t RESERVED1;
  __IO uint32_t GPIOA_AFLR;
  __IO uint32_t GPIOA_AFHR;
  __IO uint32_t GPIOB_AFLR;
  __IO uint32_t GPIOB_AFHR;
  __IO uint32_t GPIOC_AFLR;
  __IO uint32_t GPIOC_AFHR;
  __IO uint32_t GPIOD_AFLR;
  __IO uint32_t GPIOD_AFHR; 
} AFIO_TypeDef;

/* Inter Integrated Circuit Interface */
typedef struct
{
  __IO uint16_t CTLR1;
  uint16_t  RESERVED0;
  __IO uint16_t CTLR2;
  uint16_t  RESERVED1;
  __IO uint16_t OADDR1;
  uint16_t  RESERVED2;
  __IO uint16_t OADDR2;
  uint16_t  RESERVED3;
  __IO uint16_t DATAR;
  uint16_t  RESERVED4;
  __IO uint16_t STAR1;
  uint16_t  RESERVED5;
  __IO uint16_t STAR2;
  uint16_t  RESERVED6;
  __IO uint16_t CKCFGR;
  uint16_t  RESERVED7;
} I2C_TypeDef;

/* Independent WatchDog */
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint32_t PSCR;
  __IO uint32_t RLDR;
  __IO uint32_t STATR;
} IWDG_TypeDef;

/* Power Control */
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint32_t CSR;
} PWR_TypeDef;

/* Reset and Clock Control */
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint32_t CFGR0;
  __IO uint32_t INTR;
  __IO uint32_t HB2PRSTR;
  __IO uint32_t HB1PRSTR;
  __IO uint32_t HBPCENR;
  __IO uint32_t HB2PCENR;
  __IO uint32_t HB1PCENR;
  uint32_t  RESERVED0;
  __IO uint32_t RSTSCKR;
  __IO uint32_t HBRSTR;
  __IO uint32_t CFGR2;
} RCC_TypeDef;

/* Real-Time Clock */
typedef struct
{
  __IO uint16_t CTLRH;
  uint16_t  RESERVED0;
  __IO uint32_t CTLRL;
  __IO uint16_t PSCRH;
  uint16_t  RESERVED1;
  __IO uint16_t PSCRL;
  uint16_t  RESERVED2;
  __IO uint16_t DIVH;
  uint16_t  RESERVED3;
  __IO uint16_t DIVL;
  uint16_t  RESERVED4;
  __IO uint16_t CNTH;
  uint16_t  RESERVED5;
  __IO uint16_t CNTL;
  uint16_t  RESERVED6;
  __IO uint16_t ALRMH;
  uint16_t  RESERVED7;
  __IO uint16_t ALRML;
  uint16_t  RESERVED8;
} RTC_TypeDef;

/* Serial Peripheral Interface */
typedef struct
{
  __IO uint16_t CTLR1;
  uint16_t  RESERVED0;
  __IO uint16_t CTLR2;
  uint16_t  RESERVED1;
  __IO uint16_t STATR;
  uint16_t  RESERVED2;
  __IO uint16_t DATAR;
  uint16_t  RESERVED3;
  __IO uint16_t CRCR;
  uint16_t  RESERVED4;
  __IO uint16_t RCRCR;
  uint16_t  RESERVED5;
  __IO uint16_t TCRCR;
  uint16_t  RESERVED6[5];
  __IO uint16_t HSCR;
  uint16_t  RESERVED9;
} SPI_TypeDef;

/* TIM */
typedef struct
{
  __IO uint16_t CTLR1;
  uint16_t  RESERVED0;
  __IO uint16_t CTLR2;
  uint16_t  RESERVED1;
  __IO uint16_t SMCFGR;
  uint16_t  RESERVED2;
  __IO uint16_t DMAINTENR;
  uint16_t  RESERVED3;
  __IO uint16_t INTFR;
  uint16_t  RESERVED4;
  __IO uint16_t SWEVGR;
  uint16_t  RESERVED5;
  __IO uint16_t CHCTLR1;
  uint16_t  RESERVED6;
  __IO uint16_t CHCTLR2;
  uint16_t  RESERVED7;
  __IO uint16_t CCER;
  uint16_t  RESERVED8;
  union
  {
       __IO uint32_t CNT_TIM4;
      struct
      {
          __IO uint16_t CNT;
          uint16_t      RESERVED9;
      };
  };
  __IO uint16_t PSC;
  uint16_t  RESERVED10;
  union
  {
      __IO uint32_t ATRLR_TIM4;
      struct
      {
          __IO uint16_t ATRLR;
          uint16_t      RESERVED11;
      };
  };
  __IO uint16_t RPTCR;
  uint16_t  RESERVED12;
  union
  {
      __IO uint32_t CH1CVR_TIM4;
      __IO uint32_t CH1CVR;
  };
  union
  {
      __IO uint32_t CH2CVR_TIM4;
      __IO uint32_t CH2CVR;
  };
  union
  {
      __IO uint32_t CH3CVR_TIM4;
      __IO uint32_t CH3CVR;
  };
  union
  {
      __IO uint32_t CH4CVR_TIM4;
      __IO uint32_t CH4CVR;
  };
  __IO uint16_t BDTR;
  uint16_t  RESERVED17;
  __IO uint16_t DMACFGR;
  uint16_t  RESERVED18;
  union
  {
      __IO uint32_t DMAADR_R32;
      struct
      {
          __IO uint16_t DMAADR;
          uint16_t      RESERVED19;
      };
  };
} TIM_TypeDef;

/* Universal Synchronous Asynchronous Receiver Transmitter */
typedef struct
{
  __IO uint16_t STATR;
  uint16_t  RESERVED0;
  __IO uint16_t DATAR;
  uint16_t  RESERVED1;
  __IO uint32_t BRR;
  __IO uint16_t CTLR1;
  uint16_t  RESERVED2;
  __IO uint16_t CTLR2;
  uint16_t  RESERVED3;
  __IO uint16_t CTLR3;
  uint16_t  RESERVED4;
  __IO uint16_t GPR;
  uint16_t  RESERVED5;
  __IO uint16_t CTLR4;
  uint16_t  RESERVED6;
} USART_TypeDef;

/* Window WatchDog */
typedef struct
{
  __IO uint32_t CTLR;
  __IO uint32_t CFGR;
  __IO uint32_t STATR;
} WWDG_TypeDef;

/* PD Registers */
typedef struct
{
    union
    {
        __IO uint32_t USBPD_CONFIG;
        struct
        {
            __IO uint16_t CONFIG;
            __IO uint16_t BMC_CLK_CNT;
        };
    };
    union
    {
        __IO uint32_t USBPD_CONTROL;
        struct
        {
            union
            {
                __IO uint16_t R16_CONTROL;
                struct
                {
                    __IO uint8_t  CONTROL;
                    __IO uint8_t  TX_SEL;
                };
            };
            __IO uint16_t BMC_TX_SZ;
        };
    };
    union
    {
        __IO uint32_t USBPD_STATUS;
        struct
        {
            union
            {
                __IO uint16_t R16_STATUS;
                struct
                {
                    __IO uint8_t  DATA_BUF;
                    __IO uint8_t  STATUS;
                };
            };
            __IO uint16_t BMC_BYTE_CNT;
        };
    };
    union
    {
        __IO uint32_t USBPD_PORT;
        struct
        {
            __IO uint16_t PORT_CC1;
            __IO uint16_t PORT_CC2;
        };
    };
    __IO uint32_t USBPD_DMA;
} USBPD_TypeDef;

/* USBSS Deveice Registers */
typedef struct 
{
  __IO uint8_t  UEP_TX_CFG;             // 0x0
  __IO uint8_t  UEP_TX_CR;              // 0x1
  __IO uint8_t  UEP_TX_SEQ;             // 0x2
  __IO uint8_t  UEP_TX_ST;              // 0x3
  __IO uint8_t  UEP_TX_CHAIN_CR;        // 0x4
  __IO uint8_t  UEP_TX_CHAIN_ST;        // 0x5
  __IO uint16_t UEP_TX_CHAIN_LEN;       // 0x6
  __IO uint8_t  UEP_TX_CHAIN_EXP_NUMP;  // 0x8
  __IO uint8_t  UEP_TX_CHAIN_NUMP;      // 0x9
  __IO uint16_t UEP_TX_DMA_OFS;         // 0xA
  __IO uint32_t UEP_TX_DMA;             // 0xC
}USBSS_EP_TX_TypeDef;

typedef struct 
{
  __IO uint8_t  UEP_RX_CFG;             // 0x0
  __IO uint8_t  UEP_RX_CR;              // 0x1
  __IO uint8_t  UEP_RX_SEQ;             // 0x2
  __IO uint8_t  UEP_RX_ST;              // 0x3
  __IO uint8_t  UEP_RX_CHAIN_CR;        // 0x4
  __IO uint8_t  UEP_RX_CHAIN_ST;        // 0x5
  __IO uint16_t UEP_RX_CHAIN_LEN;       // 0x6
  __IO uint8_t  UEP_RX_CHAIN_MAX_NUMP;  // 0x8
  __IO uint8_t  UEP_RX_CHAIN_NUMP;      // 0x9
  __IO uint16_t UEP_RX_DMA_OFS;         // 0xA
  __IO uint32_t UEP_RX_DMA;             // 0xC
}USBSS_EP_RX_TypeDef;

typedef struct
{
    __IO uint32_t LINK_CFG;          
    __IO uint32_t LINK_CTRL;         
    __IO uint32_t LINK_INT_CTRL;     
    __IO uint32_t LINK_INT_FLAG;     
    __IO uint32_t LINK_STATUS;       
    uint8_t Reserved0[3];           
    __IO uint8_t LINK_ITP_PRE;       
    uint8_t Reserved1[5];           
    __IO uint8_t LINK_U2_INACT_TIMER;
    uint8_t Reserved2[10];          
    __IO uint8_t LINK_U1_WKUP_FILTER;
    uint8_t Reserved3[1];     
    __IO uint16_t LINK_U1_WKUP_TMR;           
    __IO uint8_t LINK_U2_WKUP_FILTER;
    uint8_t Reserved4[3];           
    __IO uint8_t LINK_U3_WKUP_FILTER;
    uint8_t Reserved5[1];  
    __IO uint16_t LINK_U3_WKUP_TMR;     
    uint8_t Reserved6[12];          
    __IO uint16_t LINK_ISO_DLY;      
    uint8_t Reserved7[14];          
    __IO uint16_t LINK_LPM_CR;       
    uint8_t Reserved8[2];                 
    __IO uint32_t LINK_LMP_PORT_CAP; 
    __IO uint32_t LINK_LMP_RX_DATA0; 
    __IO uint32_t LINK_LMP_RX_DATA1; 
    __IO uint32_t LINK_LMP_RX_DATA2; 
    __IO uint32_t LINK_LMP_TX_DATA0; 
    __IO uint32_t LINK_LMP_TX_DATA1; 
    __IO uint32_t LINK_LMP_TX_DATA2; 
    __IO uint32_t USB_CONTROL;
    __IO uint32_t USB_STATUS;
    __IO uint32_t USB_ITP;
    __IO uint32_t USB_ITP_ADJ;
    __IO uint16_t UEP_TX_EN;
    __IO uint16_t UEP_RX_EN;
    __IO uint32_t UEP0_TX_CTRL;
    __IO uint32_t UEP0_RX_CTRL;
    __IO uint32_t UEP0_TX_DMA;
    __IO uint32_t UEP0_RX_DMA;
    __IO uint32_t UEP0_TX_DMA_OFS;
    __IO uint32_t UEP0_RX_DMA_OFS;
    uint8_t Reserved9[36]; 
    __IO USBSS_EP_TX_TypeDef EP1_TX;
    __IO USBSS_EP_RX_TypeDef EP1_RX;
    __IO USBSS_EP_TX_TypeDef EP2_TX;
    __IO USBSS_EP_RX_TypeDef EP2_RX;
    __IO USBSS_EP_TX_TypeDef EP3_TX;
    __IO USBSS_EP_RX_TypeDef EP3_RX;
    __IO USBSS_EP_TX_TypeDef EP4_TX;
    __IO USBSS_EP_RX_TypeDef EP4_RX;
    __IO USBSS_EP_TX_TypeDef EP5_TX;
    __IO USBSS_EP_RX_TypeDef EP5_RX;
    __IO USBSS_EP_TX_TypeDef EP6_TX;
    __IO USBSS_EP_RX_TypeDef EP6_RX;
    __IO USBSS_EP_TX_TypeDef EP7_TX;
    __IO USBSS_EP_RX_TypeDef EP7_RX;
}USBSSD_TypeDef;

/* USBHS Deveice Registers */
typedef struct
{
  __IO uint8_t  CONTROL;
  __IO uint8_t  BASE_MODE;
  __IO uint8_t  INT_EN;
  __IO uint8_t  DEV_AD;
  __IO uint8_t  WAKE_CTRL;
  __IO uint8_t  TEST_MODE;
  __IO uint16_t LPM_DATA;

  __IO uint8_t  INT_FG;
  __IO uint8_t  INT_ST;
  __IO uint8_t  MIS_ST;
  uint8_t  RESERVED0;

  __IO uint16_t FRAME_NO;
  __IO uint16_t BUS;

  __IO uint16_t UEP_TX_EN;
  __IO uint16_t UEP_RX_EN;
  __IO uint16_t UEP_TX_TOG_AUTO;
  __IO uint16_t UEP_RX_TOG_AUTO;

  __IO uint8_t  UEP_TX_BURST;
  __IO uint8_t  UEP_TX_BURST_MODE;
  __IO uint8_t  UEP_RX_BURST;
  __IO uint8_t  UEP_RX_RES_MODE;

  __IO uint32_t UEP_AF_MODE;
  __IO uint32_t UEP0_DMA;
  __IO uint32_t UEP1_RX_DMA;
  __IO uint32_t UEP2_RX_DMA;
  __IO uint32_t UEP3_RX_DMA;
  __IO uint32_t UEP4_RX_DMA;
  __IO uint32_t UEP5_RX_DMA;
  __IO uint32_t UEP6_RX_DMA;
  __IO uint32_t UEP7_RX_DMA;
  __IO uint32_t UEP1_TX_DMA;
  __IO uint32_t UEP2_TX_DMA;
  __IO uint32_t UEP3_TX_DMA;
  __IO uint32_t UEP4_TX_DMA;
  __IO uint32_t UEP5_TX_DMA;
  __IO uint32_t UEP6_TX_DMA;
  __IO uint32_t UEP7_TX_DMA;
  __IO uint32_t UEP0_MAX_LEN;
  __IO uint32_t UEP1_MAX_LEN;
  __IO uint32_t UEP2_MAX_LEN;
  __IO uint32_t UEP3_MAX_LEN;
  __IO uint32_t UEP4_MAX_LEN;
  __IO uint32_t UEP5_MAX_LEN;
  __IO uint32_t UEP6_MAX_LEN;
  __IO uint32_t UEP7_MAX_LEN;

  __IO uint16_t UEP0_RX_LEN;
  uint16_t RESERVED1;
  __IO uint16_t UEP1_RX_LEN;
  __IO uint16_t UEP1_RX_SIZE;
  __IO uint16_t UEP2_RX_LEN;
  __IO uint16_t UEP2_RX_SIZE;
  __IO uint16_t UEP3_RX_LEN;
  __IO uint16_t UEP3_RX_SIZE;
  __IO uint16_t UEP4_RX_LEN;
  __IO uint16_t UEP4_RX_SIZE;
  __IO uint16_t UEP5_RX_LEN;
  __IO uint16_t UEP5_RX_SIZE;
  __IO uint16_t UEP6_RX_LEN;
  __IO uint16_t UEP6_RX_SIZE;
  __IO uint16_t UEP7_RX_LEN;
  __IO uint16_t UEP7_RX_SIZE;
  __IO uint16_t UEP0_TX_LEN;
  __IO uint8_t  UEP0_TX_CTRL;
  __IO uint8_t  UEP0_RX_CTRL;

  __IO uint16_t UEP1_TX_LEN;
  __IO uint8_t  UEP1_TX_CTRL;
  __IO uint8_t  UEP1_RX_CTRL;
  __IO uint16_t UEP2_TX_LEN;
  __IO uint8_t  UEP2_TX_CTRL;
  __IO uint8_t  UEP2_RX_CTRL;
  __IO uint16_t UEP3_TX_LEN;
  __IO uint8_t  UEP3_TX_CTRL;
  __IO uint8_t  UEP3_RX_CTRL;
  __IO uint16_t UEP4_TX_LEN;
  __IO uint8_t  UEP4_TX_CTRL;
  __IO uint8_t  UEP4_RX_CTRL;
  __IO uint16_t UEP5_TX_LEN;
  __IO uint8_t  UEP5_TX_CTRL;
  __IO uint8_t  UEP5_RX_CTRL;
  __IO uint16_t UEP6_TX_LEN;
  __IO uint8_t  UEP6_TX_CTRL;
  __IO uint8_t  UEP6_RX_CTRL;
  __IO uint16_t UEP7_TX_LEN;
  __IO uint8_t  UEP7_TX_CTRL;
  __IO uint8_t  UEP7_RX_CTRL;

  __IO uint16_t UEP_TX_ISO;
  __IO uint16_t UEP_RX_ISO;

  __IO uint32_t UEP1_RX_FIFO;
  __IO uint32_t UEP2_RX_FIFO;
  __IO uint32_t UEP3_RX_FIFO;
  __IO uint32_t UEP4_RX_FIFO;
  __IO uint32_t UEP5_RX_FIFO;
  __IO uint32_t UEP6_RX_FIFO;
  __IO uint32_t UEP7_RX_FIFO;
  __IO uint32_t UEP1_TX_FIFO;
  __IO uint32_t UEP2_TX_FIFO;
  __IO uint32_t UEP3_TX_FIFO;
  __IO uint32_t UEP4_TX_FIFO;
  __IO uint32_t UEP5_TX_FIFO;
  __IO uint32_t UEP6_TX_FIFO;
  __IO uint32_t UEP7_TX_FIFO;
} USBHSD_TypeDef;

/* USBHS Host Registers */
typedef struct  __attribute__((packed))
{
  __IO uint8_t  CFG;
  uint8_t  RESERVED0;
  __IO uint8_t  INT_EN;
  __IO uint8_t  DEV_ADDR;
  __IO uint32_t CONTROL;

  __IO uint8_t  INT_FLAG;
  __IO uint8_t  INT_ST;
  __IO uint8_t  MIS_ST;
  uint8_t  RESERVED1;

  __IO uint32_t LPM;
  __IO uint32_t SPLIT;
  __IO uint32_t FRAME;
  __IO uint32_t TX_LEN;
  __IO uint32_t RX_LEN;
  __IO uint32_t RX_MAX_LEN;
  __IO uint32_t RX_DMA;
  __IO uint32_t TX_DMA;
  __IO uint32_t PORT_CTRL;
  __IO uint8_t  PORT_CFG;
  uint8_t  RESERVED2;
  __IO uint8_t  PORT_INT_EN;
  __IO uint8_t  PORT_TEST_CT;

  __IO uint16_t PORT_STATUS;
  __IO uint8_t  PORT_STATUS_CHG;
  uint8_t  RESERVED3[5];
  __IO uint32_t ROOT_BC_CTRL;
} USBHSH_TypeDef;



/* USBHS Deveice Registers */
typedef struct
{
  __IO uint32_t  HSI_CAL_CR;
} USBHSI_TypeDef;
/* Enhanced Registers */
typedef struct
{
  __IO uint32_t EXTEN_CTR;
} EXTEN_TypeDef;

/* Peripheral memory map */
#define FLASH_BASE            ((uint32_t)0x08000000) /* FLASH base address in the alias region */
#define SRAM_BASE             ((uint32_t)0x20000000) /* SRAM base address in the alias region */
#define PERIPH_BASE           ((uint32_t)0x40000000) /* Peripheral base address in the alias region */

#define HBPERIPH_BASE         (PERIPH_BASE)

#define TIM2_BASE             (HBPERIPH_BASE + 0x00000)
#define TIM3_BASE             (HBPERIPH_BASE + 0x00400)
#define TIM4_BASE             (HBPERIPH_BASE + 0x00800)
#define RTC_BASE              (HBPERIPH_BASE + 0x02800)
#define WWDG_BASE             (HBPERIPH_BASE + 0x02C00)
#define IWDG_BASE             (HBPERIPH_BASE + 0x03000)
#define SPI2_BASE             (HBPERIPH_BASE + 0x03800)
#define SPI3_BASE             (HBPERIPH_BASE + 0x03C00)
#define USART2_BASE           (HBPERIPH_BASE + 0x04400)
#define USART3_BASE           (HBPERIPH_BASE + 0x04800)
#define USART4_BASE           (HBPERIPH_BASE + 0x04C00)
#define I2C1_BASE             (HBPERIPH_BASE + 0x05400)
#define I2C2_BASE             (HBPERIPH_BASE + 0x05800)
#define PWR_BASE              (HBPERIPH_BASE + 0x07000)

#define AFIO_BASE             (HBPERIPH_BASE + 0x10000)
#define EXTI_BASE             (HBPERIPH_BASE + 0x10400)
#define GPIOA_BASE            (HBPERIPH_BASE + 0x10800)
#define GPIOB_BASE            (HBPERIPH_BASE + 0x10C00)
#define GPIOC_BASE            (HBPERIPH_BASE + 0x11000)
#define GPIOD_BASE            (HBPERIPH_BASE + 0x11400)
#define ADC1_BASE             (HBPERIPH_BASE + 0x12400)
#define ADC2_BASE             (HBPERIPH_BASE + 0x12800)
#define TIM1_BASE             (HBPERIPH_BASE + 0x12C00)
#define SPI1_BASE             (HBPERIPH_BASE + 0x13000)
#define USART1_BASE           (HBPERIPH_BASE + 0x13800)
#define ADC3_BASE             (HBPERIPH_BASE + 0x13C00)
#define ADC4_BASE             (HBPERIPH_BASE + 0x14000)

#define DMA1_BASE             (HBPERIPH_BASE + 0x20000)
#define DMA1_Channel1_BASE    (HBPERIPH_BASE + 0x20008)
#define DMA1_Channel2_BASE    (HBPERIPH_BASE + 0x2001C)
#define DMA1_Channel3_BASE    (HBPERIPH_BASE + 0x20030)
#define DMA1_Channel4_BASE    (HBPERIPH_BASE + 0x20044)
#define DMA1_Channel5_BASE    (HBPERIPH_BASE + 0x20058)
#define DMA1_Channel6_BASE    (HBPERIPH_BASE + 0x2006C)
#define DMA1_Channel7_BASE    (HBPERIPH_BASE + 0x20080)
#define DMA1_Channel8_BASE    (HBPERIPH_BASE + 0x20094)
#define DMA1_Channel9_BASE    (HBPERIPH_BASE + 0x200A8)
#define DMA1_Channel10_BASE   (HBPERIPH_BASE + 0x200BC)
#define DMA1_Channel11_BASE   (HBPERIPH_BASE + 0x200D0)

#define DMA1_EXTEN_BASE       (HBPERIPH_BASE + 0x200E4)
#define RCC_BASE              (HBPERIPH_BASE + 0x21000)
#define FLASH_R_BASE          (HBPERIPH_BASE + 0x22000)
#define CRC_BASE              (HBPERIPH_BASE + 0x23000)
#define ARGB_BASE             (HBPERIPH_BASE + 0x23400)
#define USBPD_BASE            (HBPERIPH_BASE + 0x24400)

#define USBHS_BASE            (HBPERIPH_BASE + 0x30000)
#define USBHSD_BASE           (USBHS_BASE)
#define USBHSH_BASE           (USBHS_BASE + 0x100)
#define USBHSI_BASE           (USBHS_BASE + 0x200)

#define USBSS_BASE            (HBPERIPH_BASE + 0x38000)

#define OB_BASE               ((uint32_t)0x1FFFF800)

#define EXTEN_BASE            ((uint32_t)0x400220C0)

#define Reg_BASE              ((uint32_t)0x4002202C)
#define Cfg_BASE              ((uint32_t)0x1FFFE008)

/* Peripheral declaration */
#define TIM2                  ((TIM_TypeDef *) TIM2_BASE)
#define TIM3                  ((TIM_TypeDef *) TIM3_BASE)
#define TIM4                  ((TIM_TypeDef *) TIM4_BASE)
#define RTC                   ((RTC_TypeDef *) RTC_BASE)
#define WWDG                  ((WWDG_TypeDef *) WWDG_BASE)
#define IWDG                  ((IWDG_TypeDef *) IWDG_BASE)
#define SPI2                  ((SPI_TypeDef *) SPI2_BASE)
#define SPI3                  ((SPI_TypeDef *) SPI3_BASE)
#define USART2                ((USART_TypeDef *) USART2_BASE)
#define USART3                ((USART_TypeDef *) USART3_BASE)
#define USART4                ((USART_TypeDef *) USART4_BASE)
#define I2C1                  ((I2C_TypeDef *) I2C1_BASE)
#define I2C2                  ((I2C_TypeDef *) I2C2_BASE)
#define PWR                   ((PWR_TypeDef *) PWR_BASE)
#define AFIO                  ((AFIO_TypeDef *) AFIO_BASE)
#define EXTI                  ((EXTI_TypeDef *) EXTI_BASE)
#define GPIOA                 ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB                 ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC                 ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD                 ((GPIO_TypeDef *) GPIOD_BASE)
#define ADC1                  ((ADC_TypeDef *) ADC1_BASE)
#define ADC2                  ((ADC_TypeDef *) ADC2_BASE)
#define TIM1                  ((TIM_TypeDef *) TIM1_BASE)
#define SPI1                  ((SPI_TypeDef *) SPI1_BASE)
#define USART1                ((USART_TypeDef *) USART1_BASE)
#define ADC3                  ((ADC_TypeDef *) ADC3_BASE)
#define ADC4                  ((ADC_TypeDef *) ADC4_BASE)
#define DMA1                  ((DMA_TypeDef *) DMA1_BASE)
#define DMA1_EXTEN            ((DMA_TypeDef *) DMA1_EXTEN_BASE)
#define DMA1_Channel1         ((DMA_Channel_TypeDef *) DMA1_Channel1_BASE)
#define DMA1_Channel2         ((DMA_Channel_TypeDef *) DMA1_Channel2_BASE)
#define DMA1_Channel3         ((DMA_Channel_TypeDef *) DMA1_Channel3_BASE)
#define DMA1_Channel4         ((DMA_Channel_TypeDef *) DMA1_Channel4_BASE)
#define DMA1_Channel5         ((DMA_Channel_TypeDef *) DMA1_Channel5_BASE)
#define DMA1_Channel6         ((DMA_Channel_TypeDef *) DMA1_Channel6_BASE)
#define DMA1_Channel7         ((DMA_Channel_TypeDef *) DMA1_Channel7_BASE)
#define DMA1_Channel8         ((DMA_Channel_TypeDef *) DMA1_Channel8_BASE)
#define DMA1_Channel9         ((DMA_Channel_TypeDef *) DMA1_Channel9_BASE)
#define DMA1_Channel10        ((DMA_Channel_TypeDef *) DMA1_Channel10_BASE)
#define DMA1_Channel11        ((DMA_Channel_TypeDef *) DMA1_Channel11_BASE)

#define RCC                   ((RCC_TypeDef *) RCC_BASE)
#define FLASH                 ((FLASH_TypeDef *) FLASH_R_BASE)
#define CRC                   ((CRC_TypeDef *) CRC_BASE)
#define ARGB                  ((ARGB_TypeDef *) ARGB_BASE)
#define USBPD                 ((USBPD_TypeDef *) USBPD_BASE)

#define USBHSD                ((USBHSD_TypeDef *) USBHSD_BASE)
#define USBHSH                ((USBHSH_TypeDef *) USBHSH_BASE)
#define USBSSD                ((USBSSD_TypeDef *) USBSS_BASE)
#define USBHSI                ((USBHSI_TypeDef *) USBHSI_BASE)

#define OB                    ((OB_TypeDef *) OB_BASE)
#define EXTEN                 ((EXTEN_TypeDef *) EXTEN_BASE)

/******************************************************************************/
/*                         Peripheral Registers Bits Definition               */
/******************************************************************************/

/******************************************************************************/
/*                        Analog to Digital Converter                         */
/******************************************************************************/

/********************  Bit definition for ADC_STATR register  ********************/
#define  ADC_AWD                                     ((uint8_t)0x01)               /* Analog watchdog flag */
#define  ADC_EOC                                     ((uint8_t)0x02)               /* End of conversion */
#define  ADC_JEOC                                    ((uint8_t)0x04)               /* Injected channel end of conversion */
#define  ADC_JSTRT                                   ((uint8_t)0x08)               /* Injected channel Start flag */
#define  ADC_STRT                                    ((uint8_t)0x10)               /* Regular channel Start flag */
#define  ADC_RSTF                                    ((uint16_t)0x8000)            /* Reset flag */
#define  ADC_DELTA                                   ((uint32_t)0x07FF0000)        /* ADC_DELTA[10:0] bits */

/*******************  Bit definition for ADC_CTLR1 register  ********************/
#define  ADC_AWDCH                                   ((uint32_t)0x0000001F)        /* AWDCH[4:0] bits (Analog watchdog channel select bits) */
#define  ADC_AWDCH_0                                 ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_AWDCH_1                                 ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_AWDCH_2                                 ((uint32_t)0x00000004)        /* Bit 2 */
#define  ADC_AWDCH_3                                 ((uint32_t)0x00000008)        /* Bit 3 */
#define  ADC_AWDCH_4                                 ((uint32_t)0x00000010)        /* Bit 4 */

#define  ADC_EOCIE                                   ((uint32_t)0x00000020)        /* Interrupt enable for EOC */
#define  ADC_AWDIE                                   ((uint32_t)0x00000040)        /* Analog Watchdog interrupt enable */
#define  ADC_JEOCIE                                  ((uint32_t)0x00000080)        /* Interrupt enable for injected channels */
#define  ADC_SCAN                                    ((uint32_t)0x00000100)        /* Scan mode */
#define  ADC_AWDSGL                                  ((uint32_t)0x00000200)        /* Enable the watchdog on a single channel in scan mode */
#define  ADC_JAUTO                                   ((uint32_t)0x00000400)        /* Automatic injected group conversion */
#define  ADC_DISCEN                                  ((uint32_t)0x00000800)        /* Discontinuous mode on regular channels */
#define  ADC_JDISCEN                                 ((uint32_t)0x00001000)        /* Discontinuous mode on injected channels */

#define  ADC_DISCNUM                                 ((uint32_t)0x0000E000)        /* DISCNUM[2:0] bits (Discontinuous mode channel count) */
#define  ADC_DISCNUM_0                               ((uint32_t)0x00002000)        /* Bit 0 */
#define  ADC_DISCNUM_1                               ((uint32_t)0x00004000)        /* Bit 1 */
#define  ADC_DISCNUM_2                               ((uint32_t)0x00008000)        /* Bit 2 */

#define  ADC_DUALMOD                                 ((uint32_t)0x000F0000)        /* DUALMOD[3:0] bits (Dual mode selection) */
#define  ADC_DUALMOD_0                               ((uint32_t)0x00010000)        /* Bit 0 */
#define  ADC_DUALMOD_1                               ((uint32_t)0x00020000)        /* Bit 1 */
#define  ADC_DUALMOD_2                               ((uint32_t)0x00040000)        /* Bit 2 */
#define  ADC_DUALMOD_3                               ((uint32_t)0x00080000)        /* Bit 3 */

#define  ADC_JAWDEN                                  ((uint32_t)0x00400000)        /* Analog watchdog enable on injected channels */
#define  ADC_AWDEN                                   ((uint32_t)0x00800000)        /* Analog watchdog enable on regular channels */

#define  ADC_BUFEN                                   ((uint32_t)0x04000000)        
 
#define  ADC_AWDRST                                  ((uint32_t)0x20000000) 
#define  ADC_ANARST                                  ((uint32_t)0x40000000) 
#define  ADC_SWPRE                                   ((uint32_t)0x80000000) 
     
/*******************  Bit definition for ADC_CTLR2 register  ********************/
#define  ADC_ADON                                    ((uint32_t)0x00000001)        /* A/D Converter ON / OFF */
#define  ADC_CONT                                    ((uint32_t)0x00000002)        /* Continuous Conversion */
#define  ADC_CAL                                     ((uint32_t)0x00000004)        /* A/D Calibration */
#define  ADC_RSTCAL                                  ((uint32_t)0x00000008)        /* Reset Calibration */

#define  ADC_CALVOL                                  ((uint32_t)0x00000030)        /* ADC_CALVOL[2:0] bits */
#define  ADC_CALVOL_0                                ((uint32_t)0x00000010) 
#define  ADC_CALVOL_1                                ((uint32_t)0x00000020) 

#define  ADC_CALVOL0                                 ((uint32_t)0x00000000) 
#define  ADC_CALVOL1                                 ((uint32_t)0x00000010) 
#define  ADC_CALVOL2                                 ((uint32_t)0x00000020) 
#define  ADC_CALVOL3                                 ((uint32_t)0x00000030) 

#define  ADC_CALOUT                                  ((uint32_t)0x00000040)

#define  ADC_DMA                                     ((uint32_t)0x00000100)        /* Direct Memory access mode */
#define  ADC_ALIGN                                   ((uint32_t)0x00000800)        /* Data Alignment */

#define  ADC_JEXTSEL                                 ((uint32_t)0x00007000)        /* JEXTSEL[2:0] bits (External event select for injected group) */
#define  ADC_JEXTSEL_0                               ((uint32_t)0x00001000)        /* Bit 0 */
#define  ADC_JEXTSEL_1                               ((uint32_t)0x00002000)        /* Bit 1 */
#define  ADC_JEXTSEL_2                               ((uint32_t)0x00004000)        /* Bit 2 */

#define  ADC_JEXTTRIG                                ((uint32_t)0x00008000)        /* External Trigger Conversion mode for injected channels */

#define  ADC_EXTSEL                                  ((uint32_t)0x000E0000)        /* EXTSEL[2:0] bits (External Event Select for regular group) */
#define  ADC_EXTSEL_0                                ((uint32_t)0x00020000)        /* Bit 0 */
#define  ADC_EXTSEL_1                                ((uint32_t)0x00040000)        /* Bit 1 */
#define  ADC_EXTSEL_2                                ((uint32_t)0x00080000)        /* Bit 2 */

#define  ADC_EXTTRIG                                 ((uint32_t)0x00100000)        /* External Trigger Conversion mode for regular channels */
#define  ADC_JSWSTART                                ((uint32_t)0x00200000)        /* Start Conversion of injected channels */
#define  ADC_SWSTART                                 ((uint32_t)0x00400000)        /* Start Conversion of regular channels */
#define  ADC_VREFE                                   ((uint32_t)0x00800000)        /* VREFINT Enable */

#define  ADC_ENHANCE                                 ((uint32_t)0x03000000)  
#define  ADC_ENHANCE_0                               ((uint32_t)0x01000000) 
#define  ADC_ENHANCE_1                               ((uint32_t)0x02000000) 

#define  ADC_ENHANCE_12BitMODE                       ((uint32_t)0x00000000)  
#define  ADC_ENHANCE_13BitMODE                       ((uint32_t)0x01000000)  
#define  ADC_ENHANCE_14BitMODE                       ((uint32_t)0x02000000)  
#define  ADC_ENHANCE_15BitMODE                       ((uint32_t)0x03000000)  

#define  ADC_REGUCON                                 ((uint32_t)0x04000000)  
#define  ADC_INJECON                                 ((uint32_t)0x08000000) 

#define  ADC_CONTS                                   ((uint32_t)0x30000000)       /* ADC_CONTS[1:0] bits */
#define  ADC_CONTS_0                                 ((uint32_t)0x10000000) 
#define  ADC_CONTS_1                                 ((uint32_t)0x20000000) 

#define  ADC_CONTS_ADC1                              ((uint32_t)0x00000000) 
#define  ADC_CONTS_ADC2                              ((uint32_t)0x10000000) 
#define  ADC_CONTS_ADC3                              ((uint32_t)0x20000000) 
#define  ADC_CONTS_ADC4                              ((uint32_t)0x30000000) 

/******************  Bit definition for ADC_SAMPTR1 register  *******************/
#define  ADC_SMP10                                   ((uint32_t)0x00000007)        /* SMP10[2:0] bits (Channel 10 Sample time selection) */
#define  ADC_SMP10_0                                 ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_SMP10_1                                 ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_SMP10_2                                 ((uint32_t)0x00000004)        /* Bit 2 */

#define  ADC_SMP11                                   ((uint32_t)0x00000038)        /* SMP11[2:0] bits (Channel 11 Sample time selection) */
#define  ADC_SMP11_0                                 ((uint32_t)0x00000008)        /* Bit 0 */
#define  ADC_SMP11_1                                 ((uint32_t)0x00000010)        /* Bit 1 */
#define  ADC_SMP11_2                                 ((uint32_t)0x00000020)        /* Bit 2 */

#define  ADC_SMP12                                   ((uint32_t)0x000001C0)        /* SMP12[2:0] bits (Channel 12 Sample time selection) */
#define  ADC_SMP12_0                                 ((uint32_t)0x00000040)        /* Bit 0 */
#define  ADC_SMP12_1                                 ((uint32_t)0x00000080)        /* Bit 1 */
#define  ADC_SMP12_2                                 ((uint32_t)0x00000100)        /* Bit 2 */

#define  ADC_SMP13                                   ((uint32_t)0x00000E00)        /* SMP13[2:0] bits (Channel 13 Sample time selection) */
#define  ADC_SMP13_0                                 ((uint32_t)0x00000200)        /* Bit 0 */
#define  ADC_SMP13_1                                 ((uint32_t)0x00000400)        /* Bit 1 */
#define  ADC_SMP13_2                                 ((uint32_t)0x00000800)        /* Bit 2 */

/******************  Bit definition for ADC_SAMPTR2 register  *******************/
#define  ADC_SMP0                                    ((uint32_t)0x00000007)        /* SMP0[2:0] bits (Channel 0 Sample time selection) */
#define  ADC_SMP0_0                                  ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_SMP0_1                                  ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_SMP0_2                                  ((uint32_t)0x00000004)        /* Bit 2 */

#define  ADC_SMP1                                    ((uint32_t)0x00000038)        /* SMP1[2:0] bits (Channel 1 Sample time selection) */
#define  ADC_SMP1_0                                  ((uint32_t)0x00000008)        /* Bit 0 */
#define  ADC_SMP1_1                                  ((uint32_t)0x00000010)        /* Bit 1 */
#define  ADC_SMP1_2                                  ((uint32_t)0x00000020)        /* Bit 2 */

#define  ADC_SMP2                                    ((uint32_t)0x000001C0)        /* SMP2[2:0] bits (Channel 2 Sample time selection) */
#define  ADC_SMP2_0                                  ((uint32_t)0x00000040)        /* Bit 0 */
#define  ADC_SMP2_1                                  ((uint32_t)0x00000080)        /* Bit 1 */
#define  ADC_SMP2_2                                  ((uint32_t)0x00000100)        /* Bit 2 */

#define  ADC_SMP3                                    ((uint32_t)0x00000E00)        /* SMP3[2:0] bits (Channel 3 Sample time selection) */
#define  ADC_SMP3_0                                  ((uint32_t)0x00000200)        /* Bit 0 */
#define  ADC_SMP3_1                                  ((uint32_t)0x00000400)        /* Bit 1 */
#define  ADC_SMP3_2                                  ((uint32_t)0x00000800)        /* Bit 2 */

#define  ADC_SMP4                                    ((uint32_t)0x00007000)        /* SMP4[2:0] bits (Channel 4 Sample time selection) */
#define  ADC_SMP4_0                                  ((uint32_t)0x00001000)        /* Bit 0 */
#define  ADC_SMP4_1                                  ((uint32_t)0x00002000)        /* Bit 1 */
#define  ADC_SMP4_2                                  ((uint32_t)0x00004000)        /* Bit 2 */

#define  ADC_SMP5                                    ((uint32_t)0x00038000)        /* SMP5[2:0] bits (Channel 5 Sample time selection) */
#define  ADC_SMP5_0                                  ((uint32_t)0x00008000)        /* Bit 0 */
#define  ADC_SMP5_1                                  ((uint32_t)0x00010000)        /* Bit 1 */
#define  ADC_SMP5_2                                  ((uint32_t)0x00020000)        /* Bit 2 */

#define  ADC_SMP6                                    ((uint32_t)0x001C0000)        /* SMP6[2:0] bits (Channel 6 Sample time selection) */
#define  ADC_SMP6_0                                  ((uint32_t)0x00040000)        /* Bit 0 */
#define  ADC_SMP6_1                                  ((uint32_t)0x00080000)        /* Bit 1 */
#define  ADC_SMP6_2                                  ((uint32_t)0x00100000)        /* Bit 2 */

#define  ADC_SMP7                                    ((uint32_t)0x00E00000)        /* SMP7[2:0] bits (Channel 7 Sample time selection) */
#define  ADC_SMP7_0                                  ((uint32_t)0x00200000)        /* Bit 0 */
#define  ADC_SMP7_1                                  ((uint32_t)0x00400000)        /* Bit 1 */
#define  ADC_SMP7_2                                  ((uint32_t)0x00800000)        /* Bit 2 */

#define  ADC_SMP8                                    ((uint32_t)0x07000000)        /* SMP8[2:0] bits (Channel 8 Sample time selection) */
#define  ADC_SMP8_0                                  ((uint32_t)0x01000000)        /* Bit 0 */
#define  ADC_SMP8_1                                  ((uint32_t)0x02000000)        /* Bit 1 */
#define  ADC_SMP8_2                                  ((uint32_t)0x04000000)        /* Bit 2 */

#define  ADC_SMP9                                    ((uint32_t)0x38000000)        /* SMP9[2:0] bits (Channel 9 Sample time selection) */
#define  ADC_SMP9_0                                  ((uint32_t)0x08000000)        /* Bit 0 */
#define  ADC_SMP9_1                                  ((uint32_t)0x10000000)        /* Bit 1 */
#define  ADC_SMP9_2                                  ((uint32_t)0x20000000)        /* Bit 2 */

/******************  Bit definition for ADC_IOFR1 register  *******************/
#define  ADC_JOFFSET1                                ((uint16_t)0x7FFF)            /* Data offset for injected channel 1 */

/******************  Bit definition for ADC_IOFR2 register  *******************/
#define  ADC_JOFFSET2                                ((uint16_t)0x7FFF)            /* Data offset for injected channel 2 */

/******************  Bit definition for ADC_IOFR3 register  *******************/
#define  ADC_JOFFSET3                                ((uint16_t)0x7FFF)            /* Data offset for injected channel 3 */

/******************  Bit definition for ADC_IOFR4 register  *******************/
#define  ADC_JOFFSET4                                ((uint16_t)0x7FFF)            /* Data offset for injected channel 4 */

/*******************  Bit definition for ADC_WDHTR register  ********************/
#define  ADC_HT                                      ((uint16_t)0x7FFF)            /* Analog watchdog high threshold */

/*******************  Bit definition for ADC_WDLTR register  ********************/
#define  ADC_LT                                      ((uint16_t)0x7FFF)            /* Analog watchdog low threshold */

/*******************  Bit definition for ADC_RSQR1 register  *******************/
#define  ADC_SQ13                                    ((uint32_t)0x0000001F)        /* SQ13[4:0] bits (13th conversion in regular sequence) */
#define  ADC_SQ13_0                                  ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_SQ13_1                                  ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_SQ13_2                                  ((uint32_t)0x00000004)        /* Bit 2 */
#define  ADC_SQ13_3                                  ((uint32_t)0x00000008)        /* Bit 3 */
#define  ADC_SQ13_4                                  ((uint32_t)0x00000010)        /* Bit 4 */

#define  ADC_SQ14                                    ((uint32_t)0x000003E0)        /* SQ14[4:0] bits (14th conversion in regular sequence) */
#define  ADC_SQ14_0                                  ((uint32_t)0x00000020)        /* Bit 0 */
#define  ADC_SQ14_1                                  ((uint32_t)0x00000040)        /* Bit 1 */
#define  ADC_SQ14_2                                  ((uint32_t)0x00000080)        /* Bit 2 */
#define  ADC_SQ14_3                                  ((uint32_t)0x00000100)        /* Bit 3 */
#define  ADC_SQ14_4                                  ((uint32_t)0x00000200)        /* Bit 4 */

#define  ADC_SQ15                                    ((uint32_t)0x00007C00)        /* SQ15[4:0] bits (15th conversion in regular sequence) */
#define  ADC_SQ15_0                                  ((uint32_t)0x00000400)        /* Bit 0 */
#define  ADC_SQ15_1                                  ((uint32_t)0x00000800)        /* Bit 1 */
#define  ADC_SQ15_2                                  ((uint32_t)0x00001000)        /* Bit 2 */
#define  ADC_SQ15_3                                  ((uint32_t)0x00002000)        /* Bit 3 */
#define  ADC_SQ15_4                                  ((uint32_t)0x00004000)        /* Bit 4 */

#define  ADC_SQ16                                    ((uint32_t)0x000F8000)        /* SQ16[4:0] bits (16th conversion in regular sequence) */
#define  ADC_SQ16_0                                  ((uint32_t)0x00008000)        /* Bit 0 */
#define  ADC_SQ16_1                                  ((uint32_t)0x00010000)        /* Bit 1 */
#define  ADC_SQ16_2                                  ((uint32_t)0x00020000)        /* Bit 2 */
#define  ADC_SQ16_3                                  ((uint32_t)0x00040000)        /* Bit 3 */
#define  ADC_SQ16_4                                  ((uint32_t)0x00080000)        /* Bit 4 */

#define  ADC_L                                       ((uint32_t)0x00F00000)        /* L[3:0] bits (Regular channel sequence length) */
#define  ADC_L_0                                     ((uint32_t)0x00100000)        /* Bit 0 */
#define  ADC_L_1                                     ((uint32_t)0x00200000)        /* Bit 1 */
#define  ADC_L_2                                     ((uint32_t)0x00400000)        /* Bit 2 */
#define  ADC_L_3                                     ((uint32_t)0x00800000)        /* Bit 3 */

/*******************  Bit definition for ADC_RSQR2 register  *******************/
#define  ADC_SQ7                                     ((uint32_t)0x0000001F)        /* SQ7[4:0] bits (7th conversion in regular sequence) */
#define  ADC_SQ7_0                                   ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_SQ7_1                                   ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_SQ7_2                                   ((uint32_t)0x00000004)        /* Bit 2 */
#define  ADC_SQ7_3                                   ((uint32_t)0x00000008)        /* Bit 3 */
#define  ADC_SQ7_4                                   ((uint32_t)0x00000010)        /* Bit 4 */

#define  ADC_SQ8                                     ((uint32_t)0x000003E0)        /* SQ8[4:0] bits (8th conversion in regular sequence) */
#define  ADC_SQ8_0                                   ((uint32_t)0x00000020)        /* Bit 0 */
#define  ADC_SQ8_1                                   ((uint32_t)0x00000040)        /* Bit 1 */
#define  ADC_SQ8_2                                   ((uint32_t)0x00000080)        /* Bit 2 */
#define  ADC_SQ8_3                                   ((uint32_t)0x00000100)        /* Bit 3 */
#define  ADC_SQ8_4                                   ((uint32_t)0x00000200)        /* Bit 4 */

#define  ADC_SQ9                                     ((uint32_t)0x00007C00)        /* SQ9[4:0] bits (9th conversion in regular sequence) */
#define  ADC_SQ9_0                                   ((uint32_t)0x00000400)        /* Bit 0 */
#define  ADC_SQ9_1                                   ((uint32_t)0x00000800)        /* Bit 1 */
#define  ADC_SQ9_2                                   ((uint32_t)0x00001000)        /* Bit 2 */
#define  ADC_SQ9_3                                   ((uint32_t)0x00002000)        /* Bit 3 */
#define  ADC_SQ9_4                                   ((uint32_t)0x00004000)        /* Bit 4 */

#define  ADC_SQ10                                    ((uint32_t)0x000F8000)        /* SQ10[4:0] bits (10th conversion in regular sequence) */
#define  ADC_SQ10_0                                  ((uint32_t)0x00008000)        /* Bit 0 */
#define  ADC_SQ10_1                                  ((uint32_t)0x00010000)        /* Bit 1 */
#define  ADC_SQ10_2                                  ((uint32_t)0x00020000)        /* Bit 2 */
#define  ADC_SQ10_3                                  ((uint32_t)0x00040000)        /* Bit 3 */
#define  ADC_SQ10_4                                  ((uint32_t)0x00080000)        /* Bit 4 */

#define  ADC_SQ11                                    ((uint32_t)0x01F00000)        /* SQ11[4:0] bits (11th conversion in regular sequence) */
#define  ADC_SQ11_0                                  ((uint32_t)0x00100000)        /* Bit 0 */
#define  ADC_SQ11_1                                  ((uint32_t)0x00200000)        /* Bit 1 */
#define  ADC_SQ11_2                                  ((uint32_t)0x00400000)        /* Bit 2 */
#define  ADC_SQ11_3                                  ((uint32_t)0x00800000)        /* Bit 3 */
#define  ADC_SQ11_4                                  ((uint32_t)0x01000000)        /* Bit 4 */

#define  ADC_SQ12                                    ((uint32_t)0x3E000000)        /* SQ12[4:0] bits (12th conversion in regular sequence) */
#define  ADC_SQ12_0                                  ((uint32_t)0x02000000)        /* Bit 0 */
#define  ADC_SQ12_1                                  ((uint32_t)0x04000000)        /* Bit 1 */
#define  ADC_SQ12_2                                  ((uint32_t)0x08000000)        /* Bit 2 */
#define  ADC_SQ12_3                                  ((uint32_t)0x10000000)        /* Bit 3 */
#define  ADC_SQ12_4                                  ((uint32_t)0x20000000)        /* Bit 4 */

/*******************  Bit definition for ADC_RSQR3 register  *******************/
#define  ADC_SQ1                                     ((uint32_t)0x0000001F)        /* SQ1[4:0] bits (1st conversion in regular sequence) */
#define  ADC_SQ1_0                                   ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_SQ1_1                                   ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_SQ1_2                                   ((uint32_t)0x00000004)        /* Bit 2 */
#define  ADC_SQ1_3                                   ((uint32_t)0x00000008)        /* Bit 3 */
#define  ADC_SQ1_4                                   ((uint32_t)0x00000010)        /* Bit 4 */

#define  ADC_SQ2                                     ((uint32_t)0x000003E0)        /* SQ2[4:0] bits (2nd conversion in regular sequence) */
#define  ADC_SQ2_0                                   ((uint32_t)0x00000020)        /* Bit 0 */
#define  ADC_SQ2_1                                   ((uint32_t)0x00000040)        /* Bit 1 */
#define  ADC_SQ2_2                                   ((uint32_t)0x00000080)        /* Bit 2 */
#define  ADC_SQ2_3                                   ((uint32_t)0x00000100)        /* Bit 3 */
#define  ADC_SQ2_4                                   ((uint32_t)0x00000200)        /* Bit 4 */

#define  ADC_SQ3                                     ((uint32_t)0x00007C00)        /* SQ3[4:0] bits (3rd conversion in regular sequence) */
#define  ADC_SQ3_0                                   ((uint32_t)0x00000400)        /* Bit 0 */
#define  ADC_SQ3_1                                   ((uint32_t)0x00000800)        /* Bit 1 */
#define  ADC_SQ3_2                                   ((uint32_t)0x00001000)        /* Bit 2 */
#define  ADC_SQ3_3                                   ((uint32_t)0x00002000)        /* Bit 3 */
#define  ADC_SQ3_4                                   ((uint32_t)0x00004000)        /* Bit 4 */

#define  ADC_SQ4                                     ((uint32_t)0x000F8000)        /* SQ4[4:0] bits (4th conversion in regular sequence) */
#define  ADC_SQ4_0                                   ((uint32_t)0x00008000)        /* Bit 0 */
#define  ADC_SQ4_1                                   ((uint32_t)0x00010000)        /* Bit 1 */
#define  ADC_SQ4_2                                   ((uint32_t)0x00020000)        /* Bit 2 */
#define  ADC_SQ4_3                                   ((uint32_t)0x00040000)        /* Bit 3 */
#define  ADC_SQ4_4                                   ((uint32_t)0x00080000)        /* Bit 4 */

#define  ADC_SQ5                                     ((uint32_t)0x01F00000)        /* SQ5[4:0] bits (5th conversion in regular sequence) */
#define  ADC_SQ5_0                                   ((uint32_t)0x00100000)        /* Bit 0 */
#define  ADC_SQ5_1                                   ((uint32_t)0x00200000)        /* Bit 1 */
#define  ADC_SQ5_2                                   ((uint32_t)0x00400000)        /* Bit 2 */
#define  ADC_SQ5_3                                   ((uint32_t)0x00800000)        /* Bit 3 */
#define  ADC_SQ5_4                                   ((uint32_t)0x01000000)        /* Bit 4 */

#define  ADC_SQ6                                     ((uint32_t)0x3E000000)        /* SQ6[4:0] bits (6th conversion in regular sequence) */
#define  ADC_SQ6_0                                   ((uint32_t)0x02000000)        /* Bit 0 */
#define  ADC_SQ6_1                                   ((uint32_t)0x04000000)        /* Bit 1 */
#define  ADC_SQ6_2                                   ((uint32_t)0x08000000)        /* Bit 2 */
#define  ADC_SQ6_3                                   ((uint32_t)0x10000000)        /* Bit 3 */
#define  ADC_SQ6_4                                   ((uint32_t)0x20000000)        /* Bit 4 */

/*******************  Bit definition for ADC_ISQR register  *******************/
#define  ADC_JSQ1                                    ((uint32_t)0x0000001F)        /* JSQ1[4:0] bits (1st conversion in injected sequence) */  
#define  ADC_JSQ1_0                                  ((uint32_t)0x00000001)        /* Bit 0 */
#define  ADC_JSQ1_1                                  ((uint32_t)0x00000002)        /* Bit 1 */
#define  ADC_JSQ1_2                                  ((uint32_t)0x00000004)        /* Bit 2 */
#define  ADC_JSQ1_3                                  ((uint32_t)0x00000008)        /* Bit 3 */
#define  ADC_JSQ1_4                                  ((uint32_t)0x00000010)        /* Bit 4 */

#define  ADC_JSQ2                                    ((uint32_t)0x000003E0)        /* JSQ2[4:0] bits (2nd conversion in injected sequence) */
#define  ADC_JSQ2_0                                  ((uint32_t)0x00000020)        /* Bit 0 */
#define  ADC_JSQ2_1                                  ((uint32_t)0x00000040)        /* Bit 1 */
#define  ADC_JSQ2_2                                  ((uint32_t)0x00000080)        /* Bit 2 */
#define  ADC_JSQ2_3                                  ((uint32_t)0x00000100)        /* Bit 3 */
#define  ADC_JSQ2_4                                  ((uint32_t)0x00000200)        /* Bit 4 */

#define  ADC_JSQ3                                    ((uint32_t)0x00007C00)        /* JSQ3[4:0] bits (3rd conversion in injected sequence) */
#define  ADC_JSQ3_0                                  ((uint32_t)0x00000400)        /* Bit 0 */
#define  ADC_JSQ3_1                                  ((uint32_t)0x00000800)        /* Bit 1 */
#define  ADC_JSQ3_2                                  ((uint32_t)0x00001000)        /* Bit 2 */
#define  ADC_JSQ3_3                                  ((uint32_t)0x00002000)        /* Bit 3 */
#define  ADC_JSQ3_4                                  ((uint32_t)0x00004000)        /* Bit 4 */

#define  ADC_JSQ4                                    ((uint32_t)0x000F8000)        /* JSQ4[4:0] bits (4th conversion in injected sequence) */
#define  ADC_JSQ4_0                                  ((uint32_t)0x00008000)        /* Bit 0 */
#define  ADC_JSQ4_1                                  ((uint32_t)0x00010000)        /* Bit 1 */
#define  ADC_JSQ4_2                                  ((uint32_t)0x00020000)        /* Bit 2 */
#define  ADC_JSQ4_3                                  ((uint32_t)0x00040000)        /* Bit 3 */
#define  ADC_JSQ4_4                                  ((uint32_t)0x00080000)        /* Bit 4 */

#define  ADC_JL                                      ((uint32_t)0x00300000)        /* JL[1:0] bits (Injected Sequence length) */
#define  ADC_JL_0                                    ((uint32_t)0x00100000)        /* Bit 0 */
#define  ADC_JL_1                                    ((uint32_t)0x00200000)        /* Bit 1 */

/*******************  Bit definition for ADC_IDATAR1 register  *******************/
#define  ADC_IDATAR1_JDATA                           ((uint16_t)0xFFFF)            /* Injected data */

/*******************  Bit definition for ADC_IDATAR2 register  *******************/
#define  ADC_IDATAR2_JDATA                           ((uint16_t)0xFFFF)            /* Injected data */

/*******************  Bit definition for ADC_IDATAR3 register  *******************/
#define  ADC_IDATAR3_JDATA                           ((uint16_t)0xFFFF)            /* Injected data */

/*******************  Bit definition for ADC_IDATAR4 register  *******************/
#define  ADC_IDATAR4_JDATA                           ((uint16_t)0xFFFF)            /* Injected data */

/********************  Bit definition for ADC_RDATAR register  ********************/
#define  ADC_RDATAR_DATA                             ((uint32_t)0x0000FFFF)        /* Regular data */
#define  ADC_RDATAR_ADC2DATA                         ((uint32_t)0xFFFF0000)        /* ADC2 data */

/********************  Bit definition for ADC_SMPMD register  ********************/
#define  ADC_SMPMD0                                  ((uint32_t)0x00000001)        /* SMPMD0 */
#define  ADC_SMPMD1                                  ((uint32_t)0x00000002)        /* SMPMD1 */
#define  ADC_SMPMD2                                  ((uint32_t)0x00000004)        /* SMPMD2 */
#define  ADC_SMPMD3                                  ((uint32_t)0x00000008)        /* SMPMD3 */
#define  ADC_SMPMD4                                  ((uint32_t)0x00000010)        /* SMPMD4 */
#define  ADC_SMPMD5                                  ((uint32_t)0x00000020)        /* SMPMD5 */
#define  ADC_SMPMD6                                  ((uint32_t)0x00000040)        /* SMPMD6 */
#define  ADC_SMPMD7                                  ((uint32_t)0x00000080)        /* SMPMD7 */
#define  ADC_SMPMD8                                  ((uint32_t)0x00000100)        /* SMPMD8 */
#define  ADC_SMPMD9                                  ((uint32_t)0x00000200)        /* SMPMD9 */
#define  ADC_SMPMD10                                 ((uint32_t)0x00000400)        /* SMPMD10 */
#define  ADC_SMPMD11                                 ((uint32_t)0x00000800)        /* SMPMD11 */
#define  ADC_SMPMD12                                 ((uint32_t)0x00001000)        /* SMPMD12 */
#define  ADC_SMPMD13                                 ((uint32_t)0x00002000)        /* SMPMD13 */

/********************  Bit definition for ADC_UDP register  ********************/
#define  ADC_SMPUDP                                  ((uint32_t)0x000000FF)        /* ADC_SMPUDP[7:0] bits */
#define  ADC_SMPCAL                                  ((uint32_t)0x0000FF00)        /* ADC_SMPCAL[7:0] bits */

/********************  Bit definition for ADC_CONCFG register  ********************/
#define  ADC_REGUDLY                                 ((uint32_t)0x0000003F)        /* ADC_REGUDLY[5:0] bits */
#define  ADC_INJEDLY                                 ((uint32_t)0x00003F00)        /* ADC_INJEDLY[5:0] bits */

#define  ADC_INJESEQ                                 ((uint32_t)0x00030000)        /* ADC_INJESEQ[1:0] bits */
#define  ADC_INJESEQ_1                               ((uint32_t)0x00010000) 
#define  ADC_INJESEQ_2                               ((uint32_t)0x00020000) 

#define  ADC_INJENUM                                 ((uint32_t)0x000C0000)        /* ADC_INJENUM[1:0] bits */
#define  ADC_INJENUM_1                               ((uint32_t)0x00040000) 
#define  ADC_INJENUM_2                               ((uint32_t)0x00080000) 

/********************  Bit definition for ADC_SCANCNT register  ********************/
#define  ADC_SCSCFG                                  ((uint32_t)0x00000001)      
#define  ADC_SCANOE0                                 ((uint32_t)0x00000002)     
#define  ADC_SCANOE1                                 ((uint32_t)0x00000004)        
#define  ADC_SCANOE2                                 ((uint32_t)0x00000008)        

/******************************************************************************/
/*                          CRC Calculation Unit                              */
/******************************************************************************/

/*******************  Bit definition for CRC_DATAR register  *********************/
#define  CRC_DATAR_DR                                ((uint32_t)0xFFFFFFFF) /* Data register bits */

/*******************  Bit definition for CRC_IDATAR register  ********************/
#define  CRC_IDR_IDATAR                              ((uint8_t)0xFF)        /* General-purpose 8-bit data register bits */

/********************  Bit definition for CRC_CTLR register  ********************/
#define  CRC_CTLR_RESET                              ((uint8_t)0x01)        /* RESET bit */


/******************************************************************************/
/*                             DMA Controller                                 */
/******************************************************************************/

/*******************  Bit definition for DMA_INTFR register  ********************/
#define  DMA_GIF1                                    ((uint32_t)0x00000001)        /* Channel 1 Global interrupt flag */
#define  DMA_TCIF1                                   ((uint32_t)0x00000002)        /* Channel 1 Transfer Complete flag */
#define  DMA_HTIF1                                   ((uint32_t)0x00000004)        /* Channel 1 Half Transfer flag */
#define  DMA_TEIF1                                   ((uint32_t)0x00000008)        /* Channel 1 Transfer Error flag */
#define  DMA_GIF2                                    ((uint32_t)0x00000010)        /* Channel 2 Global interrupt flag */
#define  DMA_TCIF2                                   ((uint32_t)0x00000020)        /* Channel 2 Transfer Complete flag */
#define  DMA_HTIF2                                   ((uint32_t)0x00000040)        /* Channel 2 Half Transfer flag */
#define  DMA_TEIF2                                   ((uint32_t)0x00000080)        /* Channel 2 Transfer Error flag */
#define  DMA_GIF3                                    ((uint32_t)0x00000100)        /* Channel 3 Global interrupt flag */
#define  DMA_TCIF3                                   ((uint32_t)0x00000200)        /* Channel 3 Transfer Complete flag */
#define  DMA_HTIF3                                   ((uint32_t)0x00000400)        /* Channel 3 Half Transfer flag */
#define  DMA_TEIF3                                   ((uint32_t)0x00000800)        /* Channel 3 Transfer Error flag */
#define  DMA_GIF4                                    ((uint32_t)0x00001000)        /* Channel 4 Global interrupt flag */
#define  DMA_TCIF4                                   ((uint32_t)0x00002000)        /* Channel 4 Transfer Complete flag */
#define  DMA_HTIF4                                   ((uint32_t)0x00004000)        /* Channel 4 Half Transfer flag */
#define  DMA_TEIF4                                   ((uint32_t)0x00008000)        /* Channel 4 Transfer Error flag */
#define  DMA_GIF5                                    ((uint32_t)0x00010000)        /* Channel 5 Global interrupt flag */
#define  DMA_TCIF5                                   ((uint32_t)0x00020000)        /* Channel 5 Transfer Complete flag */
#define  DMA_HTIF5                                   ((uint32_t)0x00040000)        /* Channel 5 Half Transfer flag */
#define  DMA_TEIF5                                   ((uint32_t)0x00080000)        /* Channel 5 Transfer Error flag */
#define  DMA_GIF6                                    ((uint32_t)0x00100000)        /* Channel 6 Global interrupt flag */
#define  DMA_TCIF6                                   ((uint32_t)0x00200000)        /* Channel 6 Transfer Complete flag */
#define  DMA_HTIF6                                   ((uint32_t)0x00400000)        /* Channel 6 Half Transfer flag */
#define  DMA_TEIF6                                   ((uint32_t)0x00800000)        /* Channel 6 Transfer Error flag */
#define  DMA_GIF7                                    ((uint32_t)0x01000000)        /* Channel 7 Global interrupt flag */
#define  DMA_TCIF7                                   ((uint32_t)0x02000000)        /* Channel 7 Transfer Complete flag */
#define  DMA_HTIF7                                   ((uint32_t)0x04000000)        /* Channel 7 Half Transfer flag */
#define  DMA_TEIF7                                   ((uint32_t)0x08000000)        /* Channel 7 Transfer Error flag */
#define  DMA_GIF8                                    ((uint32_t)0x10000000)        /* Channel 8 Global interrupt flag */
#define  DMA_TCIF8                                   ((uint32_t)0x20000000)        /* Channel 8 Transfer Complete flag */
#define  DMA_HTIF8                                   ((uint32_t)0x40000000)        /* Channel 8 Half Transfer flag */
#define  DMA_TEIF8                                   ((uint32_t)0x80000000)        /* Channel 8 Transfer Error flag */

#define  DMA_GIF9                                    ((uint32_t)0x00000001)        /* Channel 9 Global interrupt flag */
#define  DMA_TCIF9                                   ((uint32_t)0x00000002)        /* Channel 9 Transfer Complete flag */
#define  DMA_HTIF9                                   ((uint32_t)0x00000004)        /* Channel 9 Half Transfer flag */
#define  DMA_TEIF9                                   ((uint32_t)0x00000008)        /* Channel 9 Transfer Error flag */
#define  DMA_GIF10                                   ((uint32_t)0x00000010)        /* Channel 10 Global interrupt flag */
#define  DMA_TCIF10                                  ((uint32_t)0x00000020)        /* Channel 10 Transfer Complete flag */
#define  DMA_HTIF10                                  ((uint32_t)0x00000040)        /* Channel 10 Half Transfer flag */
#define  DMA_TEIF10                                  ((uint32_t)0x00000080)        /* Channel 10 Transfer Error flag */
#define  DMA_GIF11                                   ((uint32_t)0x00000100)        /* Channel 11 Global interrupt flag */
#define  DMA_TCIF11                                  ((uint32_t)0x00000200)        /* Channel 11 Transfer Complete flag */
#define  DMA_HTIF11                                  ((uint32_t)0x00000400)        /* Channel 11 Half Transfer flag */
#define  DMA_TEIF11                                  ((uint32_t)0x00000800)        /* Channel 11 Transfer Error flag */

/*******************  Bit definition for DMA_INTFCR register  *******************/
#define  DMA_CGIF1                                   ((uint32_t)0x00000001)        /* Channel 1 Global interrupt clear */
#define  DMA_CTCIF1                                  ((uint32_t)0x00000002)        /* Channel 1 Transfer Complete clear */
#define  DMA_CHTIF1                                  ((uint32_t)0x00000004)        /* Channel 1 Half Transfer clear */
#define  DMA_CTEIF1                                  ((uint32_t)0x00000008)        /* Channel 1 Transfer Error clear */
#define  DMA_CGIF2                                   ((uint32_t)0x00000010)        /* Channel 2 Global interrupt clear */
#define  DMA_CTCIF2                                  ((uint32_t)0x00000020)        /* Channel 2 Transfer Complete clear */
#define  DMA_CHTIF2                                  ((uint32_t)0x00000040)        /* Channel 2 Half Transfer clear */
#define  DMA_CTEIF2                                  ((uint32_t)0x00000080)        /* Channel 2 Transfer Error clear */
#define  DMA_CGIF3                                   ((uint32_t)0x00000100)        /* Channel 3 Global interrupt clear */
#define  DMA_CTCIF3                                  ((uint32_t)0x00000200)        /* Channel 3 Transfer Complete clear */
#define  DMA_CHTIF3                                  ((uint32_t)0x00000400)        /* Channel 3 Half Transfer clear */
#define  DMA_CTEIF3                                  ((uint32_t)0x00000800)        /* Channel 3 Transfer Error clear */
#define  DMA_CGIF4                                   ((uint32_t)0x00001000)        /* Channel 4 Global interrupt clear */
#define  DMA_CTCIF4                                  ((uint32_t)0x00002000)        /* Channel 4 Transfer Complete clear */
#define  DMA_CHTIF4                                  ((uint32_t)0x00004000)        /* Channel 4 Half Transfer clear */
#define  DMA_CTEIF4                                  ((uint32_t)0x00008000)        /* Channel 4 Transfer Error clear */
#define  DMA_CGIF5                                   ((uint32_t)0x00010000)        /* Channel 5 Global interrupt clear */
#define  DMA_CTCIF5                                  ((uint32_t)0x00020000)        /* Channel 5 Transfer Complete clear */
#define  DMA_CHTIF5                                  ((uint32_t)0x00040000)        /* Channel 5 Half Transfer clear */
#define  DMA_CTEIF5                                  ((uint32_t)0x00080000)        /* Channel 5 Transfer Error clear */
#define  DMA_CGIF6                                   ((uint32_t)0x00100000)        /* Channel 6 Global interrupt clear */
#define  DMA_CTCIF6                                  ((uint32_t)0x00200000)        /* Channel 6 Transfer Complete clear */
#define  DMA_CHTIF6                                  ((uint32_t)0x00400000)        /* Channel 6 Half Transfer clear */
#define  DMA_CTEIF6                                  ((uint32_t)0x00800000)        /* Channel 6 Transfer Error clear */
#define  DMA_CGIF7                                   ((uint32_t)0x01000000)        /* Channel 7 Global interrupt clear */
#define  DMA_CTCIF7                                  ((uint32_t)0x02000000)        /* Channel 7 Transfer Complete clear */
#define  DMA_CHTIF7                                  ((uint32_t)0x04000000)        /* Channel 7 Half Transfer clear */
#define  DMA_CTEIF7                                  ((uint32_t)0x08000000)        /* Channel 7 Transfer Error clear */
#define  DMA_CGIF8                                   ((uint32_t)0x10000000)        /* Channel 8 Global interrupt clear */
#define  DMA_CTCIF8                                  ((uint32_t)0x20000000)        /* Channel 8 Transfer Complete clear */
#define  DMA_CHTIF8                                  ((uint32_t)0x40000000)        /* Channel 8 Half Transfer clear */
#define  DMA_CTEIF8                                  ((uint32_t)0x80000000)        /* Channel 8 Transfer Error clear */

/*******************  Bit definition for            A_CFGR1 register  *******************/
#define  DMA_CFGR1_EN                                ((uint16_t)0x0001)            /* Channel enable*/
#define  DMA_CFGR1_TCIE                              ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR1_HTIE                              ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR1_TEIE                              ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR1_DIR                               ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR1_CIRC                              ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR1_PINC                              ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR1_MINC                              ((uint16_t)0x0080)            /* Memory increment mode */

#define  DMA_CFGR1_PSIZE                             ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR1_PSIZE_0                           ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR1_PSIZE_1                           ((uint16_t)0x0200)            /* Bit 1 */

#define  DMA_CFGR1_MSIZE                             ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR1_MSIZE_0                           ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR1_MSIZE_1                           ((uint16_t)0x0800)            /* Bit 1 */

#define  DMA_CFGR1_PL                                ((uint16_t)0x3000)            /* PL[1:0] bits(Channel Priority level) */
#define  DMA_CFGR1_PL_0                              ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR1_PL_1                              ((uint16_t)0x2000)            /* Bit 1 */

#define  DMA_CFGR1_MEM2MEM                           ((uint16_t)0x4000)            /* Memory to memory mode */
#define  DMA_CFGR1_DOUBLE_MODE                       ((uint16_t)0x8000) 
#define  DMA_CFGR1_FLAG_CUR_MEM                      ((uint32_t)0x00010000) 

/*******************  Bit definition for DMA_CFGR2 register  *******************/
#define  DMA_CFGR2_EN                                ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFGR2_TCIE                              ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR2_HTIE                              ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR2_TEIE                              ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR2_DIR                               ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR2_CIRC                              ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR2_PINC                              ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR2_MINC                              ((uint16_t)0x0080)            /* Memory increment mode */

#define  DMA_CFGR2_PSIZE                             ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR2_PSIZE_0                           ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR2_PSIZE_1                           ((uint16_t)0x0200)            /* Bit 1 */

#define  DMA_CFGR2_MSIZE                             ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR2_MSIZE_0                           ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR2_MSIZE_1                           ((uint16_t)0x0800)            /* Bit 1 */

#define  DMA_CFGR2_PL                                ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFGR2_PL_0                              ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR2_PL_1                              ((uint16_t)0x2000)            /* Bit 1 */

#define  DMA_CFGR2_MEM2MEM                           ((uint16_t)0x4000)            /* Memory to memory mode */
#define  DMA_CFGR2_DOUBLE_MODE                       ((uint16_t)0x8000) 
#define  DMA_CFGR2_FLAG_CUR_MEM                      ((uint32_t)0x00010000) 

/*******************  Bit definition for DMA_CFGR3 register  *******************/
#define  DMA_CFGR3_EN                                ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFGR3_TCIE                              ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR3_HTIE                              ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR3_TEIE                              ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR3_DIR                               ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR3_CIRC                              ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR3_PINC                              ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR3_MINC                              ((uint16_t)0x0080)            /* Memory increment mode */

#define  DMA_CFGR3_PSIZE                             ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR3_PSIZE_0                           ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR3_PSIZE_1                           ((uint16_t)0x0200)            /* Bit 1 */

#define  DMA_CFGR3_MSIZE                             ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR3_MSIZE_0                           ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR3_MSIZE_1                           ((uint16_t)0x0800)            /* Bit 1 */

#define  DMA_CFGR3_PL                                ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFGR3_PL_0                              ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR3_PL_1                              ((uint16_t)0x2000)            /* Bit 1 */

#define  DMA_CFGR3_MEM2MEM                           ((uint16_t)0x4000)            /* Memory to memory mode */
#define  DMA_CFGR3_DOUBLE_MODE                       ((uint16_t)0x8000) 
#define  DMA_CFGR3_FLAG_CUR_MEM                      ((uint32_t)0x00010000) 

/*******************  Bit definition for DMA_CFGR4 register  *******************/
#define  DMA_CFGR4_EN                                ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFGR4_TCIE                              ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR4_HTIE                              ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR4_TEIE                              ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR4_DIR                               ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR4_CIRC                              ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR4_PINC                              ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR4_MINC                              ((uint16_t)0x0080)            /* Memory increment mode */

#define  DMA_CFGR4_PSIZE                             ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR4_PSIZE_0                           ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR4_PSIZE_1                           ((uint16_t)0x0200)            /* Bit 1 */
             
#define  DMA_CFGR4_MSIZE                             ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR4_MSIZE_0                           ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR4_MSIZE_1                           ((uint16_t)0x0800)            /* Bit 1 */
             
#define  DMA_CFGR4_PL                                ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFGR4_PL_0                              ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR4_PL_1                              ((uint16_t)0x2000)            /* Bit 1 */
             
#define  DMA_CFGR4_MEM2MEM                           ((uint16_t)0x4000)            /* Memory to memory mode */
#define  DMA_CFGR4_DOUBLE_MODE                       ((uint16_t)0x8000) 
#define  DMA_CFGR4_FLAG_CUR_MEM                      ((uint32_t)0x00010000) 

/******************  Bit definition for DMA_CFGR5 register  *******************/
#define  DMA_CFGR5_EN                                ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFGR5_TCIE                              ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR5_HTIE                              ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR5_TEIE                              ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR5_DIR                               ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR5_CIRC                              ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR5_PINC                              ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR5_MINC                              ((uint16_t)0x0080)            /* Memory increment mode */
             
#define  DMA_CFGR5_PSIZE                             ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR5_PSIZE_0                           ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR5_PSIZE_1                           ((uint16_t)0x0200)            /* Bit 1 */
             
#define  DMA_CFGR5_MSIZE                             ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR5_MSIZE_0                           ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR5_MSIZE_1                           ((uint16_t)0x0800)            /* Bit 1 */
             
#define  DMA_CFGR5_PL                                ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFGR5_PL_0                              ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR5_PL_1                              ((uint16_t)0x2000)            /* Bit 1 */
             
#define  DMA_CFGR5_MEM2MEM                           ((uint16_t)0x4000)            /* Memory to memory mode enable */
#define  DMA_CFGR5_DOUBLE_MODE                       ((uint16_t)0x8000) 
#define  DMA_CFGR5_FLAG_CUR_MEM                      ((uint32_t)0x00010000) 

/*******************  Bit definition for DMA_CFGR6 register  *******************/
#define  DMA_CFGR6_EN                                ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFGR6_TCIE                              ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR6_HTIE                              ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR6_TEIE                              ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR6_DIR                               ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR6_CIRC                              ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR6_PINC                              ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR6_MINC                              ((uint16_t)0x0080)            /* Memory increment mode */
             
#define  DMA_CFGR6_PSIZE                             ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR6_PSIZE_0                           ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR6_PSIZE_1                           ((uint16_t)0x0200)            /* Bit 1 */
             
#define  DMA_CFGR6_MSIZE                             ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR6_MSIZE_0                           ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR6_MSIZE_1                           ((uint16_t)0x0800)            /* Bit 1 */
             
#define  DMA_CFGR6_PL                                ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFGR6_PL_0                              ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR6_PL_1                              ((uint16_t)0x2000)            /* Bit 1 */
             
#define  DMA_CFGR6_MEM2MEM                           ((uint16_t)0x4000)            /* Memory to memory mode */
#define  DMA_CFGR6_DOUBLE_MODE                       ((uint16_t)0x8000) 
#define  DMA_CFGR6_FLAG_CUR_MEM                      ((uint32_t)0x00010000) 

/*******************  Bit definition for DMA_CFGR7 register  *******************/
#define  DMA_CFGR7_EN                                ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFGR7_TCIE                              ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR7_HTIE                              ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR7_TEIE                              ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR7_DIR                               ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR7_CIRC                              ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR7_PINC                              ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR7_MINC                              ((uint16_t)0x0080)            /* Memory increment mode */
             
#define  DMA_CFGR7_PSIZE                             ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR7_PSIZE_0                           ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR7_PSIZE_1                           ((uint16_t)0x0200)            /* Bit 1 */
             
#define  DMA_CFGR7_MSIZE                             ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR7_MSIZE_0                           ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR7_MSIZE_1                           ((uint16_t)0x0800)            /* Bit 1 */
             
#define  DMA_CFGR7_PL                                ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFGR7_PL_0                              ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR7_PL_1                              ((uint16_t)0x2000)            /* Bit 1 */
             
#define  DMA_CFGR7_MEM2MEM                           ((uint16_t)0x4000)            /* Memory to memory mode enable */
#define  DMA_CFGR7_DOUBLE_MODE                       ((uint16_t)0x8000) 
#define  DMA_CFGR7_FLAG_CUR_MEM                      ((uint32_t)0x00010000) 

/*******************  Bit definition for DMA_CFGR8 register  *******************/
#define  DMA_CFGR8_EN                                ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFGR8_TCIE                              ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR8_HTIE                              ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR8_TEIE                              ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR8_DIR                               ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR8_CIRC                              ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR8_PINC                              ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR8_MINC                              ((uint16_t)0x0080)            /* Memory increment mode */
             
#define  DMA_CFGR8_PSIZE                             ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR8_PSIZE_0                           ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR8_PSIZE_1                           ((uint16_t)0x0200)            /* Bit 1 */
             
#define  DMA_CFGR8_MSIZE                             ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR8_MSIZE_0                           ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR8_MSIZE_1                           ((uint16_t)0x0800)            /* Bit 1 */
             
#define  DMA_CFGR8_PL                                ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFGR8_PL_0                              ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR8_PL_1                              ((uint16_t)0x2000)            /* Bit 1 */
             
#define  DMA_CFGR8_MEM2MEM                           ((uint16_t)0x4000)            /* Memory to memory mode enable */
#define  DMA_CFGR8_DOUBLE_MODE                       ((uint16_t)0x8000) 
#define  DMA_CFGR8_FLAG_CUR_MEM                      ((uint32_t)0x00010000) 

/*******************  Bit definition for DMA_CFGR9 register  *******************/
#define  DMA_CFGR9_EN                                ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFGR9_TCIE                              ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR9_HTIE                              ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR9_TEIE                              ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR9_DIR                               ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR9_CIRC                              ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR9_PINC                              ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR9_MINC                              ((uint16_t)0x0080)            /* Memory increment mode */
             
#define  DMA_CFGR9_PSIZE                             ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR9_PSIZE_0                           ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR9_PSIZE_1                           ((uint16_t)0x0200)            /* Bit 1 */
             
#define  DMA_CFGR9_MSIZE                             ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR9_MSIZE_0                           ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR9_MSIZE_1                           ((uint16_t)0x0800)            /* Bit 1 */
             
#define  DMA_CFGR9_PL                                ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFGR9_PL_0                              ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR9_PL_1                              ((uint16_t)0x2000)            /* Bit 1 */
             
#define  DMA_CFGR9_MEM2MEM                           ((uint16_t)0x4000)            /* Memory to memory mode enable */
#define  DMA_CFGR9_DOUBLE_MODE                       ((uint16_t)0x8000) 
#define  DMA_CFGR9_FLAG_CUR_MEM                      ((uint32_t)0x00010000) 

/*******************  Bit definition for DMA_CFGR10 register  *******************/
#define  DMA_CFGR10_EN                               ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFGR10_TCIE                             ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR10_HTIE                             ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR10_TEIE                             ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR10_DIR                              ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR10_CIRC                             ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR10_PINC                             ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR10_MINC                             ((uint16_t)0x0080)            /* Memory increment mode */
             
#define  DMA_CFGR10_PSIZE                            ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR10_PSIZE_0                          ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR10_PSIZE_1                          ((uint16_t)0x0200)            /* Bit 1 */
             
#define  DMA_CFGR10_MSIZE                            ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR10_MSIZE_0                          ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR10_MSIZE_1                          ((uint16_t)0x0800)            /* Bit 1 */
             
#define  DMA_CFGR10_PL                               ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFGR10_PL_0                             ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR10_PL_1                             ((uint16_t)0x2000)            /* Bit 1 */
             
#define  DMA_CFGR10_MEM2MEM                          ((uint16_t)0x4000)            /* Memory to memory mode enable */
#define  DMA_CFGR10_DOUBLE_MODE                      ((uint16_t)0x8000) 
#define  DMA_CFGR10_FLAG_CUR_MEM                     ((uint32_t)0x00010000) 

/*******************  Bit definition for DMA_CFGR11 register  *******************/
#define  DMA_CFGR11_EN                               ((uint16_t)0x0001)            /* Channel enable */
#define  DMA_CFGR11_TCIE                             ((uint16_t)0x0002)            /* Transfer complete interrupt enable */
#define  DMA_CFGR11_HTIE                             ((uint16_t)0x0004)            /* Half Transfer interrupt enable */
#define  DMA_CFGR11_TEIE                             ((uint16_t)0x0008)            /* Transfer error interrupt enable */
#define  DMA_CFGR11_DIR                              ((uint16_t)0x0010)            /* Data transfer direction */
#define  DMA_CFGR11_CIRC                             ((uint16_t)0x0020)            /* Circular mode */
#define  DMA_CFGR11_PINC                             ((uint16_t)0x0040)            /* Peripheral increment mode */
#define  DMA_CFGR11_MINC                             ((uint16_t)0x0080)            /* Memory increment mode */
             
#define  DMA_CFGR11_PSIZE                            ((uint16_t)0x0300)            /* PSIZE[1:0] bits (Peripheral size) */
#define  DMA_CFGR11_PSIZE_0                          ((uint16_t)0x0100)            /* Bit 0 */
#define  DMA_CFGR11_PSIZE_1                          ((uint16_t)0x0200)            /* Bit 1 */
             
#define  DMA_CFGR11_MSIZE                            ((uint16_t)0x0C00)            /* MSIZE[1:0] bits (Memory size) */
#define  DMA_CFGR11_MSIZE_0                          ((uint16_t)0x0400)            /* Bit 0 */
#define  DMA_CFGR11_MSIZE_1                          ((uint16_t)0x0800)            /* Bit 1 */
             
#define  DMA_CFGR11_PL                               ((uint16_t)0x3000)            /* PL[1:0] bits (Channel Priority level) */
#define  DMA_CFGR11_PL_0                             ((uint16_t)0x1000)            /* Bit 0 */
#define  DMA_CFGR11_PL_1                             ((uint16_t)0x2000)            /* Bit 1 */
             
#define  DMA_CFGR11_MEM2MEM                          ((uint16_t)0x4000)            /* Memory to memory mode enable */
#define  DMA_CFGR11_DOUBLE_MODE                      ((uint16_t)0x8000) 
#define  DMA_CFGR11_FLAG_CUR_MEM                     ((uint32_t)0x00010000) 

/******************  Bit definition for DMA_CNTR1 register  ******************/
#define  DMA_CNTR1_NDT                               ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR2 register  ******************/
#define  DMA_CNTR2_NDT                               ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR3 register  ******************/
#define  DMA_CNTR3_NDT                               ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR4 register  ******************/
#define  DMA_CNTR4_NDT                               ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR5 register  ******************/
#define  DMA_CNTR5_NDT                               ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR6 register  ******************/
#define  DMA_CNTR6_NDT                               ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR7 register  ******************/
#define  DMA_CNTR7_NDT                               ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR8 register  ******************/
#define  DMA_CNTR8_NDT                               ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR9 register  ******************/
#define  DMA_CNTR9_NDT                               ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR10 register  ******************/
#define  DMA_CNTR10_NDT                              ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_CNTR11 register  ******************/
#define  DMA_CNTR11_NDT                              ((uint16_t)0xFFFF)            /* Number of data to Transfer */

/******************  Bit definition for DMA_PADDR1 register  *******************/
#define  DMA_PADDR1_PA                               ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR2 register  *******************/
#define  DMA_PADDR2_PA                               ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR3 register  *******************/
#define  DMA_PADDR3_PA                               ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR4 register  *******************/
#define  DMA_PADDR4_PA                               ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR5 register  *******************/
#define  DMA_PADDR5_PA                               ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR6 register  *******************/
#define  DMA_PADDR6_PA                               ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR7 register  *******************/
#define  DMA_PADDR7_PA                               ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR8 register  *******************/
#define  DMA_PADDR8_PA                               ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR9 register  *******************/
#define  DMA_PADDR9_PA                               ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR10 register  *******************/
#define  DMA_PADDR10_PA                              ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_PADDR11 register  *******************/
#define  DMA_PADDR11_PA                              ((uint32_t)0xFFFFFFFF)        /* Peripheral Address */

/******************  Bit definition for DMA_MADDR1 register  *******************/
#define  DMA_MADDR1_MA                               ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR2 register  *******************/
#define  DMA_MADDR2_MA                               ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR3 register  *******************/
#define  DMA_MADDR3_MA                               ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR4 register  *******************/
#define  DMA_MADDR4_MA                               ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR5 register  *******************/
#define  DMA_MADDR5_MA                               ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR6 register  *******************/
#define  DMA_MADDR6_MA                               ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR7 register  *******************/
#define  DMA_MADDR7_MA                               ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR8 register  *******************/
#define  DMA_MADDR8_MA                               ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR9 register  *******************/
#define  DMA_MADDR9_MA                               ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR10 register  *******************/
#define  DMA_MADDR10_MA                              ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_MADDR11 register  *******************/
#define  DMA_MADDR11_MA                              ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_M1ADDR1 register  *******************/
#define  DMA_M1ADDR1_MA                              ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_M1ADDR2 register  *******************/
#define  DMA_M1ADDR2_MA                              ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_M1ADDR3 register  *******************/
#define  DMA_M1ADDR3_MA                              ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_M1ADDR4 register  *******************/
#define  DMA_M1ADDR4_MA                              ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_M1ADDR5 register  *******************/
#define  DMA_M1ADDR5_MA                              ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_M1ADDR6 register  *******************/
#define  DMA_M1ADDR6_MA                              ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_M1ADDR7 register  *******************/
#define  DMA_M1ADDR7_MA                              ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_M1ADDR8 register  *******************/
#define  DMA_M1ADDR8_MA                              ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_M1ADDR9 register  *******************/
#define  DMA_M1ADDR9_MA                              ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_M1ADDR10 register  *******************/
#define  DMA_M1ADDR10_MA                             ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_M1ADDR11 register  *******************/
#define  DMA_M1ADDR11_MA                             ((uint32_t)0xFFFFFFFF)        /* Memory Address */

/******************  Bit definition for DMA_EXTEM_INTFR register  *******************/
#define  DMA_GIF9                                    ((uint32_t)0x00000001)        /* Channel 9 Global interrupt flag */
#define  DMA_TCIF9                                   ((uint32_t)0x00000002)        /* Channel 9 Transfer Complete flag */
#define  DMA_HTIF9                                   ((uint32_t)0x00000004)        /* Channel 9 Half Transfer flag */
#define  DMA_TEIF9                                   ((uint32_t)0x00000008)        /* Channel 9 Transfer Error flag */
#define  DMA_GIF10                                   ((uint32_t)0x00000010)        /* Channel 10 Global interrupt flag */
#define  DMA_TCIF10                                  ((uint32_t)0x00000020)        /* Channel 10 Transfer Complete flag */
#define  DMA_HTIF10                                  ((uint32_t)0x00000040)        /* Channel 10 Half Transfer flag */
#define  DMA_TEIF10                                  ((uint32_t)0x00000080)        /* Channel 10 Transfer Error flag */
#define  DMA_GIF11                                   ((uint32_t)0x00000100)        /* Channel 11 Global interrupt flag */
#define  DMA_TCIF11                                  ((uint32_t)0x00000200)        /* Channel 11 Transfer Complete flag */
#define  DMA_HTIF11                                  ((uint32_t)0x00000400)        /* Channel 11 Half Transfer flag */
#define  DMA_TEIF11                                  ((uint32_t)0x00000800)        /* Channel 11 Transfer Error flag */

/******************  Bit definition for DMA_EXTEM_INTFCR register  *******************/
#define  DMA_CGIF9                                   ((uint32_t)0x00000001)        /* Channel 9 Global interrupt clear */
#define  DMA_CTCIF9                                  ((uint32_t)0x00000002)        /* Channel 9 Transfer Complete clear */
#define  DMA_CHTIF9                                  ((uint32_t)0x00000004)        /* Channel 9 Half Transfer clear */
#define  DMA_CTEIF9                                  ((uint32_t)0x00000008)        /* Channel 9 Transfer Error clear */
#define  DMA_CGIF10                                  ((uint32_t)0x00000010)        /* Channel 10 Global interrupt clear */
#define  DMA_CTCIF10                                 ((uint32_t)0x00000020)        /* Channel 10 Transfer Complete clear */
#define  DMA_CHTIF10                                 ((uint32_t)0x00000040)        /* Channel 10 Half Transfer clear */
#define  DMA_CTEIF10                                 ((uint32_t)0x00000080)        /* Channel 10 Transfer Error clear */
#define  DMA_CGIF11                                  ((uint32_t)0x00000100)        /* Channel 11 Global interrupt clear */
#define  DMA_CTCIF11                                 ((uint32_t)0x00000200)        /* Channel 11 Transfer Complete clear */
#define  DMA_CHTIF11                                 ((uint32_t)0x00000400)        /* Channel 11 Half Transfer clear */
#define  DMA_CTEIF11                                 ((uint32_t)0x00000800)        /* Channel 11 Transfer Error clear */

/******************************************************************************/
/*                    External Interrupt/Event Controller                     */
/******************************************************************************/

/*******************  Bit definition for EXTI_INTENR register  *******************/
#define  EXTI_INTENR_MR0                             ((uint32_t)0x00000001)        /* Interrupt Mask on line 0 */
#define  EXTI_INTENR_MR1                             ((uint32_t)0x00000002)        /* Interrupt Mask on line 1 */
#define  EXTI_INTENR_MR2                             ((uint32_t)0x00000004)        /* Interrupt Mask on line 2 */
#define  EXTI_INTENR_MR3                             ((uint32_t)0x00000008)        /* Interrupt Mask on line 3 */
#define  EXTI_INTENR_MR4                             ((uint32_t)0x00000010)        /* Interrupt Mask on line 4 */
#define  EXTI_INTENR_MR5                             ((uint32_t)0x00000020)        /* Interrupt Mask on line 5 */
#define  EXTI_INTENR_MR6                             ((uint32_t)0x00000040)        /* Interrupt Mask on line 6 */
#define  EXTI_INTENR_MR7                             ((uint32_t)0x00000080)        /* Interrupt Mask on line 7 */
#define  EXTI_INTENR_MR8                             ((uint32_t)0x00000100)        /* Interrupt Mask on line 8 */
#define  EXTI_INTENR_MR9                             ((uint32_t)0x00000200)        /* Interrupt Mask on line 9 */
#define  EXTI_INTENR_MR10                            ((uint32_t)0x00000400)        /* Interrupt Mask on line 10 */
#define  EXTI_INTENR_MR11                            ((uint32_t)0x00000800)        /* Interrupt Mask on line 11 */
#define  EXTI_INTENR_MR12                            ((uint32_t)0x00001000)        /* Interrupt Mask on line 12 */
#define  EXTI_INTENR_MR13                            ((uint32_t)0x00002000)        /* Interrupt Mask on line 13 */
#define  EXTI_INTENR_MR14                            ((uint32_t)0x00004000)        /* Interrupt Mask on line 14 */
#define  EXTI_INTENR_MR15                            ((uint32_t)0x00008000)        /* Interrupt Mask on line 15 */
#define  EXTI_INTENR_MR16                            ((uint32_t)0x00010000)        /* Interrupt Mask on line 16 */
#define  EXTI_INTENR_MR17                            ((uint32_t)0x00020000)        /* Interrupt Mask on line 17 */
#define  EXTI_INTENR_MR18                            ((uint32_t)0x00040000)        /* Interrupt Mask on line 18 */
#define  EXTI_INTENR_MR19                            ((uint32_t)0x00080000)        /* Interrupt Mask on line 19 */
#define  EXTI_INTENR_MR20                            ((uint32_t)0x00100000)        /* Interrupt Mask on line 20 */
#define  EXTI_INTENR_MR21                            ((uint32_t)0x00200000)        /* Interrupt Mask on line 21 */

/*******************  Bit definition for EXTI_EVENR register  *******************/
#define  EXTI_EVENR_MR0                              ((uint32_t)0x00000001)        /* Event Mask on line 0 */
#define  EXTI_EVENR_MR1                              ((uint32_t)0x00000002)        /* Event Mask on line 1 */
#define  EXTI_EVENR_MR2                              ((uint32_t)0x00000004)        /* Event Mask on line 2 */
#define  EXTI_EVENR_MR3                              ((uint32_t)0x00000008)        /* Event Mask on line 3 */
#define  EXTI_EVENR_MR4                              ((uint32_t)0x00000010)        /* Event Mask on line 4 */
#define  EXTI_EVENR_MR5                              ((uint32_t)0x00000020)        /* Event Mask on line 5 */
#define  EXTI_EVENR_MR6                              ((uint32_t)0x00000040)        /* Event Mask on line 6 */
#define  EXTI_EVENR_MR7                              ((uint32_t)0x00000080)        /* Event Mask on line 7 */
#define  EXTI_EVENR_MR8                              ((uint32_t)0x00000100)        /* Event Mask on line 8 */
#define  EXTI_EVENR_MR9                              ((uint32_t)0x00000200)        /* Event Mask on line 9 */
#define  EXTI_EVENR_MR10                             ((uint32_t)0x00000400)        /* Event Mask on line 10 */
#define  EXTI_EVENR_MR11                             ((uint32_t)0x00000800)        /* Event Mask on line 11 */
#define  EXTI_EVENR_MR12                             ((uint32_t)0x00001000)        /* Event Mask on line 12 */
#define  EXTI_EVENR_MR13                             ((uint32_t)0x00002000)        /* Event Mask on line 13 */
#define  EXTI_EVENR_MR14                             ((uint32_t)0x00004000)        /* Event Mask on line 14 */
#define  EXTI_EVENR_MR15                             ((uint32_t)0x00008000)        /* Event Mask on line 15 */
#define  EXTI_EVENR_MR16                             ((uint32_t)0x00010000)        /* Event Mask on line 16 */
#define  EXTI_EVENR_MR17                             ((uint32_t)0x00020000)        /* Event Mask on line 17 */
#define  EXTI_EVENR_MR18                             ((uint32_t)0x00040000)        /* Event Mask on line 18 */
#define  EXTI_EVENR_MR19                             ((uint32_t)0x00080000)        /* Event Mask on line 19 */
#define  EXTI_EVENR_MR20                             ((uint32_t)0x00100000)        /* Event Mask on line 20 */
#define  EXTI_EVENR_MR21                             ((uint32_t)0x00200000)        /* Event Mask on line 21 */

/******************  Bit definition for EXTI_RTENR register  *******************/
#define  EXTI_RTENR_TR0                              ((uint32_t)0x00000001)        /* Rising trigger event configuration bit of line 0 */
#define  EXTI_RTENR_TR1                              ((uint32_t)0x00000002)        /* Rising trigger event configuration bit of line 1 */
#define  EXTI_RTENR_TR2                              ((uint32_t)0x00000004)        /* Rising trigger event configuration bit of line 2 */
#define  EXTI_RTENR_TR3                              ((uint32_t)0x00000008)        /* Rising trigger event configuration bit of line 3 */
#define  EXTI_RTENR_TR4                              ((uint32_t)0x00000010)        /* Rising trigger event configuration bit of line 4 */
#define  EXTI_RTENR_TR5                              ((uint32_t)0x00000020)        /* Rising trigger event configuration bit of line 5 */
#define  EXTI_RTENR_TR6                              ((uint32_t)0x00000040)        /* Rising trigger event configuration bit of line 6 */
#define  EXTI_RTENR_TR7                              ((uint32_t)0x00000080)        /* Rising trigger event configuration bit of line 7 */
#define  EXTI_RTENR_TR8                              ((uint32_t)0x00000100)        /* Rising trigger event configuration bit of line 8 */
#define  EXTI_RTENR_TR9                              ((uint32_t)0x00000200)        /* Rising trigger event configuration bit of line 9 */
#define  EXTI_RTENR_TR10                             ((uint32_t)0x00000400)        /* Rising trigger event configuration bit of line 10 */
#define  EXTI_RTENR_TR11                             ((uint32_t)0x00000800)        /* Rising trigger event configuration bit of line 11 */
#define  EXTI_RTENR_TR12                             ((uint32_t)0x00001000)        /* Rising trigger event configuration bit of line 12 */
#define  EXTI_RTENR_TR13                             ((uint32_t)0x00002000)        /* Rising trigger event configuration bit of line 13 */
#define  EXTI_RTENR_TR14                             ((uint32_t)0x00004000)        /* Rising trigger event configuration bit of line 14 */
#define  EXTI_RTENR_TR15                             ((uint32_t)0x00008000)        /* Rising trigger event configuration bit of line 15 */
#define  EXTI_RTENR_TR16                             ((uint32_t)0x00010000)        /* Rising trigger event configuration bit of line 16 */
#define  EXTI_RTENR_TR17                             ((uint32_t)0x00020000)        /* Rising trigger event configuration bit of line 17 */
#define  EXTI_RTENR_TR18                             ((uint32_t)0x00040000)        /* Rising trigger event configuration bit of line 18 */
#define  EXTI_RTENR_TR19                             ((uint32_t)0x00080000)        /* Rising trigger event configuration bit of line 19 */
#define  EXTI_RTENR_TR20                             ((uint32_t)0x00100000)        /* Rising trigger event configuration bit of line 20 */
#define  EXTI_RTENR_TR21                             ((uint32_t)0x00200000)        /* Rising trigger event configuration bit of line 21 */

/******************  Bit definition for EXTI_FTENR register  *******************/
#define  EXTI_FTENR_TR0                              ((uint32_t)0x00000001)        /* Falling trigger event configuration bit of line 0 */
#define  EXTI_FTENR_TR1                              ((uint32_t)0x00000002)        /* Falling trigger event configuration bit of line 1 */
#define  EXTI_FTENR_TR2                              ((uint32_t)0x00000004)        /* Falling trigger event configuration bit of line 2 */
#define  EXTI_FTENR_TR3                              ((uint32_t)0x00000008)        /* Falling trigger event configuration bit of line 3 */
#define  EXTI_FTENR_TR4                              ((uint32_t)0x00000010)        /* Falling trigger event configuration bit of line 4 */
#define  EXTI_FTENR_TR5                              ((uint32_t)0x00000020)        /* Falling trigger event configuration bit of line 5 */
#define  EXTI_FTENR_TR6                              ((uint32_t)0x00000040)        /* Falling trigger event configuration bit of line 6 */
#define  EXTI_FTENR_TR7                              ((uint32_t)0x00000080)        /* Falling trigger event configuration bit of line 7 */
#define  EXTI_FTENR_TR8                              ((uint32_t)0x00000100)        /* Falling trigger event configuration bit of line 8 */
#define  EXTI_FTENR_TR9                              ((uint32_t)0x00000200)        /* Falling trigger event configuration bit of line 9 */
#define  EXTI_FTENR_TR10                             ((uint32_t)0x00000400)        /* Falling trigger event configuration bit of line 10 */
#define  EXTI_FTENR_TR11                             ((uint32_t)0x00000800)        /* Falling trigger event configuration bit of line 11 */
#define  EXTI_FTENR_TR12                             ((uint32_t)0x00001000)        /* Falling trigger event configuration bit of line 12 */
#define  EXTI_FTENR_TR13                             ((uint32_t)0x00002000)        /* Falling trigger event configuration bit of line 13 */
#define  EXTI_FTENR_TR14                             ((uint32_t)0x00004000)        /* Falling trigger event configuration bit of line 14 */
#define  EXTI_FTENR_TR15                             ((uint32_t)0x00008000)        /* Falling trigger event configuration bit of line 15 */
#define  EXTI_FTENR_TR16                             ((uint32_t)0x00010000)        /* Falling trigger event configuration bit of line 16 */
#define  EXTI_FTENR_TR17                             ((uint32_t)0x00020000)        /* Falling trigger event configuration bit of line 17 */
#define  EXTI_FTENR_TR18                             ((uint32_t)0x00040000)        /* Falling trigger event configuration bit of line 18 */
#define  EXTI_FTENR_TR19                             ((uint32_t)0x00080000)        /* Falling trigger event configuration bit of line 19 */
#define  EXTI_FTENR_TR20                             ((uint32_t)0x00100000)        /* Falling trigger event configuration bit of line 20 */
#define  EXTI_FTENR_TR21                             ((uint32_t)0x00200000)        /* Falling trigger event configuration bit of line 21 */

/******************  Bit definition for EXTI_SWIEVR register  ******************/
#define  EXTI_SWIEVR_SWIEVR0                         ((uint32_t)0x00000001)        /* Software Interrupt on line 0 */
#define  EXTI_SWIEVR_SWIEVR1                         ((uint32_t)0x00000002)        /* Software Interrupt on line 1 */
#define  EXTI_SWIEVR_SWIEVR2                         ((uint32_t)0x00000004)        /* Software Interrupt on line 2 */
#define  EXTI_SWIEVR_SWIEVR3                         ((uint32_t)0x00000008)        /* Software Interrupt on line 3 */
#define  EXTI_SWIEVR_SWIEVR4                         ((uint32_t)0x00000010)        /* Software Interrupt on line 4 */
#define  EXTI_SWIEVR_SWIEVR5                         ((uint32_t)0x00000020)        /* Software Interrupt on line 5 */
#define  EXTI_SWIEVR_SWIEVR6                         ((uint32_t)0x00000040)        /* Software Interrupt on line 6 */
#define  EXTI_SWIEVR_SWIEVR7                         ((uint32_t)0x00000080)        /* Software Interrupt on line 7 */
#define  EXTI_SWIEVR_SWIEVR8                         ((uint32_t)0x00000100)        /* Software Interrupt on line 8 */
#define  EXTI_SWIEVR_SWIEVR9                         ((uint32_t)0x00000200)        /* Software Interrupt on line 9 */
#define  EXTI_SWIEVR_SWIEVR10                        ((uint32_t)0x00000400)        /* Software Interrupt on line 10 */
#define  EXTI_SWIEVR_SWIEVR11                        ((uint32_t)0x00000800)        /* Software Interrupt on line 11 */
#define  EXTI_SWIEVR_SWIEVR12                        ((uint32_t)0x00001000)        /* Software Interrupt on line 12 */
#define  EXTI_SWIEVR_SWIEVR13                        ((uint32_t)0x00002000)        /* Software Interrupt on line 13 */
#define  EXTI_SWIEVR_SWIEVR14                        ((uint32_t)0x00004000)        /* Software Interrupt on line 14 */
#define  EXTI_SWIEVR_SWIEVR15                        ((uint32_t)0x00008000)        /* Software Interrupt on line 15 */
#define  EXTI_SWIEVR_SWIEVR16                        ((uint32_t)0x00010000)        /* Software Interrupt on line 16 */
#define  EXTI_SWIEVR_SWIEVR17                        ((uint32_t)0x00020000)        /* Software Interrupt on line 17 */
#define  EXTI_SWIEVR_SWIEVR18                        ((uint32_t)0x00040000)        /* Software Interrupt on line 18 */
#define  EXTI_SWIEVR_SWIEVR19                        ((uint32_t)0x00080000)        /* Software Interrupt on line 19 */
#define  EXTI_SWIEVR_SWIEVR20                        ((uint32_t)0x00100000)        /* Software Interrupt on line 20 */
#define  EXTI_SWIEVR_SWIEVR21                        ((uint32_t)0x00200000)        /* Software Interrupt on line 21 */

/*******************  Bit definition for EXTI_INTFR register  ********************/
#define  EXTI_INTF_INTF0                             ((uint32_t)0x00000001)        /* Pending bit for line 0 */
#define  EXTI_INTF_INTF1                             ((uint32_t)0x00000002)        /* Pending bit for line 1 */
#define  EXTI_INTF_INTF2                             ((uint32_t)0x00000004)        /* Pending bit for line 2 */
#define  EXTI_INTF_INTF3                             ((uint32_t)0x00000008)        /* Pending bit for line 3 */
#define  EXTI_INTF_INTF4                             ((uint32_t)0x00000010)        /* Pending bit for line 4 */
#define  EXTI_INTF_INTF5                             ((uint32_t)0x00000020)        /* Pending bit for line 5 */
#define  EXTI_INTF_INTF6                             ((uint32_t)0x00000040)        /* Pending bit for line 6 */
#define  EXTI_INTF_INTF7                             ((uint32_t)0x00000080)        /* Pending bit for line 7 */
#define  EXTI_INTF_INTF8                             ((uint32_t)0x00000100)        /* Pending bit for line 8 */
#define  EXTI_INTF_INTF9                             ((uint32_t)0x00000200)        /* Pending bit for line 9 */
#define  EXTI_INTF_INTF10                            ((uint32_t)0x00000400)        /* Pending bit for line 10 */
#define  EXTI_INTF_INTF11                            ((uint32_t)0x00000800)        /* Pending bit for line 11 */
#define  EXTI_INTF_INTF12                            ((uint32_t)0x00001000)        /* Pending bit for line 12 */
#define  EXTI_INTF_INTF13                            ((uint32_t)0x00002000)        /* Pending bit for line 13 */
#define  EXTI_INTF_INTF14                            ((uint32_t)0x00004000)        /* Pending bit for line 14 */
#define  EXTI_INTF_INTF15                            ((uint32_t)0x00008000)        /* Pending bit for line 15 */
#define  EXTI_INTF_INTF16                            ((uint32_t)0x00010000)        /* Pending bit for line 16 */
#define  EXTI_INTF_INTF17                            ((uint32_t)0x00020000)        /* Pending bit for line 17 */
#define  EXTI_INTF_INTF18                            ((uint32_t)0x00040000)        /* Pending bit for line 18 */
#define  EXTI_INTF_INTF19                            ((uint32_t)0x00080000)        /* Pending bit for line 19 */
#define  EXTI_INTF_INTF20                            ((uint32_t)0x00100000)        /* Pending bit for line 20 */
#define  EXTI_INTF_INTF21                            ((uint32_t)0x00200000)        /* Pending bit for line 21 */

/******************************************************************************/
/*                      FLASH and Option Bytes Registers                      */
/******************************************************************************/

/*******************  Bit definition for FLASH_ACTLR register  ******************/
#define  FLASH_ACTLR_SCK_CFG                         ((uint32_t)0x00000003)
#define  FLASH_ACTLR_SCK_CFG_0                       ((uint32_t)0x00000001)
#define  FLASH_ACTLR_SCK_CFG_1                       ((uint32_t)0x00000002)

#define  FLASH_ACTLR_LATENCY_HCLK_DIV1               ((uint32_t)0x00000000)
#define  FLASH_ACTLR_LATENCY_HCLK_DIV2               ((uint32_t)0x00000001)
#define  FLASH_ACTLR_LATENCY_HCLK_DIV4               ((uint32_t)0x00000002)
#define  FLASH_ACTLR_LATENCY_HCLK_DIV8               ((uint32_t)0x00000003)

#define  FLASH_ACTLR_ENHANCE_STATUS                  ((uint32_t)0x00000040)
#define  FLASH_ACTLR_EHMOD                           ((uint32_t)0x00000080)
      
#define  FLASH_ACTLR_LP                              ((uint32_t)0x00000100)
#define  FLASH_ACTLR_RD_MD                           ((uint32_t)0x00000800)
#define  FLASH_ACTLR_READY                           ((uint32_t)0x00004000)
#define  FLASH_ACTLR_ST                              ((uint32_t)0x00008000)

/******************  Bit definition for FLASH_KEYR register  ******************/
#define  FLASH_KEYR_FKEYR                            ((uint32_t)0xFFFFFFFF)        /* FPEC Key */
#define  FLASH_KEYR_KEY1                             ((uint32_t)0x45670123)
#define  FLASH_KEYR_KEY2                             ((uint32_t)0xCDEF89AB)

/*****************  Bit definition for FLASH_OBKEYR register  ****************/
#define  FLASH_OBKEYR_OBKEYR                         ((uint32_t)0xFFFFFFFF)        /* Option Byte Key */

/******************  Bit definition for FLASH_STATR register  *******************/
#define  FLASH_STATR_BSY                             ((uint8_t)0x01)               /* Busy */
#define  FLASH_STATR_WRBSY                           ((uint8_t)0x02)               
#define  FLASH_STATR_WRPRTERR                        ((uint8_t)0x10)               /* Write Protection Error */
#define  FLASH_STATR_EOP                             ((uint8_t)0x20)               /* End of operation */             

#define  FLASH_STATR_BOOT_LOADER                     ((uint16_t)0x0800)
#define  FLASH_STATR_BOOT_AVA                        ((uint16_t)0x1000)
#define  FLASH_STATR_BOOT_STATUS                     ((uint16_t)0x2000)
#define  FLASH_STATR_BOOT_MODE                       ((uint16_t)0x4000)
#define  FLASH_STATR_BOOT_LOCK                       ((uint16_t)0x8000)

/*******************  Bit definition for FLASH_CTLR register  *******************/
#define  FLASH_CTLR_PG                               ((uint32_t)0x00000001)        /* Programming */
#define  FLASH_CTLR_PER                              ((uint32_t)0x00000002)        /* Sector Erase 4K */
#define  FLASH_CTLR_MER                              ((uint32_t)0x00000004)        /* Mass Erase */
#define  FLASH_CTLR_OPTPG                            ((uint32_t)0x00000010)        /* Option Byte Programming */
#define  FLASH_CTLR_OPTER                            ((uint32_t)0x00000020)        /* Option Byte Erase */
#define  FLASH_CTLR_STRT                             ((uint32_t)0x00000040)        /* Start */
#define  FLASH_CTLR_LOCK                             ((uint32_t)0x00000080)        /* Lock */
#define  FLASH_CTLR_OPTWRE                           ((uint32_t)0x00000200)        /* Option Bytes Write Enable */
#define  FLASH_CTLR_ERRIE                            ((uint32_t)0x00000400)        /* Error Interrupt Enable */
#define  FLASH_CTLR_EOPIE                            ((uint32_t)0x00001000)        /* End of operation interrupt enable */
#define  FLASH_CTLR_FAST_LOCK                        ((uint32_t)0x00008000)        /* Fast Lock */
#define  FLASH_CTLR_PAGE_PG                          ((uint32_t)0x00010000)        /* Page Programming 256Byte */
#define  FLASH_CTLR_PAGE_ER                          ((uint32_t)0x00020000)        /* Page Erase 256Byte */
#define  FLASH_CTLR_PAGE_BER32                       ((uint32_t)0x00040000)        /* Block Erase 32K */
#define  FLASH_CTLR_PG_STRT                          ((uint32_t)0x00200000)        /* Page Programming Start */
#define  FLASH_CTLR_RSENACT                          ((uint32_t)0x00400000)

/*******************  Bit definition for FLASH_ADDR register  *******************/
#define  FLASH_ADDR_FAR                              ((uint32_t)0xFFFFFFFF)        /* Flash Address */

/******************  Bit definition for FLASH_OBR register  *******************/
#define  FLASH_OBR_OPTERR                            ((uint16_t)0x0001)            /* Option Byte Error */
#define  FLASH_OBR_RDPRT                             ((uint16_t)0x0002)            /* Read protection */

#define  FLASH_OBR_USER                              ((uint16_t)0x0024)            /* User Option Bytes */
#define  FLASH_OBR_WDG_SW                            ((uint16_t)0x0004)            /* WDG_SW */
#define  FLASH_OBR_RST_MODE                          ((uint16_t)0x0020)           /* RST_MODE */

#define  FLASH_OBR_DATA0                             ((uint32_t)0x0003FC00)
#define  FLASH_OBR_DATA1                             ((uint32_t)0x03FC0000)

/******************  Bit definition for FLASH_WPR register  ******************/
#define  FLASH_WPR_WRP                               ((uint32_t)0xFFFFFFFF)        /* Write Protect */

/******************  Bit definition for FLASH_MODEKEYR register  ******************/
#define  FLASH_MODEKEYR_KEY1                         ((uint32_t)0x45670123)        
#define  FLASH_MODEKEYR_KEY2                         ((uint32_t)0xCDEF89AB)        

/******************  Bit definition for FLASH_RDPR register  *******************/
#define  FLASH_RDPR_RDPR                             ((uint32_t)0x000000FF)        /* Read protection option byte */
#define  FLASH_RDPR_nRDPR                            ((uint32_t)0x0000FF00)        /* Read protection complemented option byte */

/******************  Bit definition for FLASH_USER register  ******************/
#define  FLASH_USER_USER                             ((uint32_t)0x00FF0000)        /* User option byte */
#define  FLASH_USER_nUSER                            ((uint32_t)0xFF000000)        /* User complemented option byte */

/******************  Bit definition for FLASH_Data0 register  *****************/
#define  FLASH_Data0_Data0                           ((uint32_t)0x000000FF)        /* User data storage option byte */
#define  FLASH_Data0_nData0                          ((uint32_t)0x0000FF00)        /* User data storage complemented option byte */

/******************  Bit definition for FLASH_Data1 register  *****************/
#define  FLASH_Data1_Data1                           ((uint32_t)0x00FF0000)        /* User data storage option byte */
#define  FLASH_Data1_nData1                          ((uint32_t)0xFF000000)        /* User data storage complemented option byte */

/******************  Bit definition for FLASH_WRPR0 register  ******************/
#define  FLASH_WRPR0_WRPR0                           ((uint32_t)0x000000FF)        /* Flash memory write protection option bytes */
#define  FLASH_WRPR0_nWRPR0                          ((uint32_t)0x0000FF00)        /* Flash memory write protection complemented option bytes */

/******************  Bit definition for FLASH_WRPR1 register  ******************/
#define  FLASH_WRPR1_WRPR1                           ((uint32_t)0x00FF0000)        /* Flash memory write protection option bytes */
#define  FLASH_WRPR1_nWRPR1                          ((uint32_t)0xFF000000)        /* Flash memory write protection complemented option bytes */

/******************  Bit definition for FLASH_WRPR2 register  ******************/
#define  FLASH_WRPR2_WRPR2                           ((uint32_t)0x000000FF)        /* Flash memory write protection option bytes */
#define  FLASH_WRPR2_nWRPR2                          ((uint32_t)0x0000FF00)        /* Flash memory write protection complemented option bytes */

/******************  Bit definition for FLASH_WRPR3 register  ******************/
#define  FLASH_WRPR3_WRPR3                           ((uint32_t)0x00FF0000)        /* Flash memory write protection option bytes */
#define  FLASH_WRPR3_nWRPR3                          ((uint32_t)0xFF000000)        /* Flash memory write protection complemented option bytes */

/******************************************************************************/
/*                General Purpose and Alternate Function I/O                  */
/******************************************************************************/

/*******************  Bit definition for GPIO_CFGLR register  *******************/
#define  GPIO_CFGLR_MODE                             ((uint32_t)0x33333333)        /* Port x mode bits */

#define  GPIO_CFGLR_MODE0                            ((uint32_t)0x00000003)        /* MODE0[1:0] bits (Port x mode bits, pin 0) */
#define  GPIO_CFGLR_MODE0_0                          ((uint32_t)0x00000001)        /* Bit 0 */
#define  GPIO_CFGLR_MODE0_1                          ((uint32_t)0x00000002)        /* Bit 1 */

#define  GPIO_CFGLR_MODE1                            ((uint32_t)0x00000030)        /* MODE1[1:0] bits (Port x mode bits, pin 1) */
#define  GPIO_CFGLR_MODE1_0                          ((uint32_t)0x00000010)        /* Bit 0 */
#define  GPIO_CFGLR_MODE1_1                          ((uint32_t)0x00000020)        /* Bit 1 */

#define  GPIO_CFGLR_MODE2                            ((uint32_t)0x00000300)        /* MODE2[1:0] bits (Port x mode bits, pin 2) */
#define  GPIO_CFGLR_MODE2_0                          ((uint32_t)0x00000100)        /* Bit 0 */
#define  GPIO_CFGLR_MODE2_1                          ((uint32_t)0x00000200)        /* Bit 1 */

#define  GPIO_CFGLR_MODE3                            ((uint32_t)0x00003000)        /* MODE3[1:0] bits (Port x mode bits, pin 3) */
#define  GPIO_CFGLR_MODE3_0                          ((uint32_t)0x00001000)        /* Bit 0 */
#define  GPIO_CFGLR_MODE3_1                          ((uint32_t)0x00002000)        /* Bit 1 */

#define  GPIO_CFGLR_MODE4                            ((uint32_t)0x00030000)        /* MODE4[1:0] bits (Port x mode bits, pin 4) */
#define  GPIO_CFGLR_MODE4_0                          ((uint32_t)0x00010000)        /* Bit 0 */
#define  GPIO_CFGLR_MODE4_1                          ((uint32_t)0x00020000)        /* Bit 1 */

#define  GPIO_CFGLR_MODE5                            ((uint32_t)0x00300000)        /* MODE5[1:0] bits (Port x mode bits, pin 5) */
#define  GPIO_CFGLR_MODE5_0                          ((uint32_t)0x00100000)        /* Bit 0 */
#define  GPIO_CFGLR_MODE5_1                          ((uint32_t)0x00200000)        /* Bit 1 */

#define  GPIO_CFGLR_MODE6                            ((uint32_t)0x03000000)        /* MODE6[1:0] bits (Port x mode bits, pin 6) */
#define  GPIO_CFGLR_MODE6_0                          ((uint32_t)0x01000000)        /* Bit 0 */
#define  GPIO_CFGLR_MODE6_1                          ((uint32_t)0x02000000)        /* Bit 1 */

#define  GPIO_CFGLR_MODE7                            ((uint32_t)0x30000000)        /* MODE7[1:0] bits (Port x mode bits, pin 7) */
#define  GPIO_CFGLR_MODE7_0                          ((uint32_t)0x10000000)        /* Bit 0 */
#define  GPIO_CFGLR_MODE7_1                          ((uint32_t)0x20000000)        /* Bit 1 */

#define  GPIO_CFGLR_CNF                              ((uint32_t)0xCCCCCCCC)        /* Port x configuration bits */

#define  GPIO_CFGLR_CNF0                             ((uint32_t)0x0000000C)        /* CNF0[1:0] bits (Port x configuration bits, pin 0) */
#define  GPIO_CFGLR_CNF0_0                           ((uint32_t)0x00000004)        /* Bit 0 */
#define  GPIO_CFGLR_CNF0_1                           ((uint32_t)0x00000008)        /* Bit 1 */

#define  GPIO_CFGLR_CNF1                             ((uint32_t)0x000000C0)        /* CNF1[1:0] bits (Port x configuration bits, pin 1) */
#define  GPIO_CFGLR_CNF1_0                           ((uint32_t)0x00000040)        /* Bit 0 */
#define  GPIO_CFGLR_CNF1_1                           ((uint32_t)0x00000080)        /* Bit 1 */

#define  GPIO_CFGLR_CNF2                             ((uint32_t)0x00000C00)        /* CNF2[1:0] bits (Port x configuration bits, pin 2) */
#define  GPIO_CFGLR_CNF2_0                           ((uint32_t)0x00000400)        /* Bit 0 */
#define  GPIO_CFGLR_CNF2_1                           ((uint32_t)0x00000800)        /* Bit 1 */

#define  GPIO_CFGLR_CNF3                             ((uint32_t)0x0000C000)        /* CNF3[1:0] bits (Port x configuration bits, pin 3) */
#define  GPIO_CFGLR_CNF3_0                           ((uint32_t)0x00004000)        /* Bit 0 */
#define  GPIO_CFGLR_CNF3_1                           ((uint32_t)0x00008000)        /* Bit 1 */

#define  GPIO_CFGLR_CNF4                             ((uint32_t)0x000C0000)        /* CNF4[1:0] bits (Port x configuration bits, pin 4) */
#define  GPIO_CFGLR_CNF4_0                           ((uint32_t)0x00040000)        /* Bit 0 */
#define  GPIO_CFGLR_CNF4_1                           ((uint32_t)0x00080000)        /* Bit 1 */

#define  GPIO_CFGLR_CNF5                             ((uint32_t)0x00C00000)        /* CNF5[1:0] bits (Port x configuration bits, pin 5) */
#define  GPIO_CFGLR_CNF5_0                           ((uint32_t)0x00400000)        /* Bit 0 */
#define  GPIO_CFGLR_CNF5_1                           ((uint32_t)0x00800000)        /* Bit 1 */

#define  GPIO_CFGLR_CNF6                             ((uint32_t)0x0C000000)        /* CNF6[1:0] bits (Port x configuration bits, pin 6) */
#define  GPIO_CFGLR_CNF6_0                           ((uint32_t)0x04000000)        /* Bit 0 */
#define  GPIO_CFGLR_CNF6_1                           ((uint32_t)0x08000000)        /* Bit 1 */

#define  GPIO_CFGLR_CNF7                             ((uint32_t)0xC0000000)        /* CNF7[1:0] bits (Port x configuration bits, pin 7) */
#define  GPIO_CFGLR_CNF7_0                           ((uint32_t)0x40000000)        /* Bit 0 */
#define  GPIO_CFGLR_CNF7_1                           ((uint32_t)0x80000000)        /* Bit 1 */

/*******************  Bit definition for GPIO_CFGHR register  *******************/
#define  GPIO_CFGHR_MODE                             ((uint32_t)0x33333333)        /* Port x mode bits */

#define  GPIO_CFGHR_MODE8                            ((uint32_t)0x00000003)        /* MODE8[1:0] bits (Port x mode bits, pin 8) */
#define  GPIO_CFGHR_MODE8_0                          ((uint32_t)0x00000001)        /* Bit 0 */
#define  GPIO_CFGHR_MODE8_1                          ((uint32_t)0x00000002)        /* Bit 1 */

#define  GPIO_CFGHR_MODE9                            ((uint32_t)0x00000030)        /* MODE9[1:0] bits (Port x mode bits, pin 9) */
#define  GPIO_CFGHR_MODE9_0                          ((uint32_t)0x00000010)        /* Bit 0 */
#define  GPIO_CFGHR_MODE9_1                          ((uint32_t)0x00000020)        /* Bit 1 */

#define  GPIO_CFGHR_MODE10                           ((uint32_t)0x00000300)        /* MODE10[1:0] bits (Port x mode bits, pin 10) */
#define  GPIO_CFGHR_MODE10_0                         ((uint32_t)0x00000100)        /* Bit 0 */
#define  GPIO_CFGHR_MODE10_1                         ((uint32_t)0x00000200)        /* Bit 1 */

#define  GPIO_CFGHR_MODE11                           ((uint32_t)0x00003000)        /* MODE11[1:0] bits (Port x mode bits, pin 11) */
#define  GPIO_CFGHR_MODE11_0                         ((uint32_t)0x00001000)        /* Bit 0 */
#define  GPIO_CFGHR_MODE11_1                         ((uint32_t)0x00002000)        /* Bit 1 */

#define  GPIO_CFGHR_MODE12                           ((uint32_t)0x00030000)        /* MODE12[1:0] bits (Port x mode bits, pin 12) */
#define  GPIO_CFGHR_MODE12_0                         ((uint32_t)0x00010000)        /* Bit 0 */
#define  GPIO_CFGHR_MODE12_1                         ((uint32_t)0x00020000)        /* Bit 1 */

#define  GPIO_CFGHR_MODE13                           ((uint32_t)0x00300000)        /* MODE13[1:0] bits (Port x mode bits, pin 13) */
#define  GPIO_CFGHR_MODE13_0                         ((uint32_t)0x00100000)        /* Bit 0 */
#define  GPIO_CFGHR_MODE13_1                         ((uint32_t)0x00200000)        /* Bit 1 */

#define  GPIO_CFGHR_MODE14                           ((uint32_t)0x03000000)        /* MODE14[1:0] bits (Port x mode bits, pin 14) */
#define  GPIO_CFGHR_MODE14_0                         ((uint32_t)0x01000000)        /* Bit 0 */
#define  GPIO_CFGHR_MODE14_1                         ((uint32_t)0x02000000)        /* Bit 1 */

#define  GPIO_CFGHR_MODE15                           ((uint32_t)0x30000000)        /* MODE15[1:0] bits (Port x mode bits, pin 15) */
#define  GPIO_CFGHR_MODE15_0                         ((uint32_t)0x10000000)        /* Bit 0 */
#define  GPIO_CFGHR_MODE15_1                         ((uint32_t)0x20000000)        /* Bit 1 */

#define  GPIO_CFGHR_CNF                              ((uint32_t)0xCCCCCCCC)        /* Port x configuration bits */

#define  GPIO_CFGHR_CNF8                             ((uint32_t)0x0000000C)        /* CNF8[1:0] bits (Port x configuration bits, pin 8) */
#define  GPIO_CFGHR_CNF8_0                           ((uint32_t)0x00000004)        /* Bit 0 */
#define  GPIO_CFGHR_CNF8_1                           ((uint32_t)0x00000008)        /* Bit 1 */

#define  GPIO_CFGHR_CNF9                             ((uint32_t)0x000000C0)        /* CNF9[1:0] bits (Port x configuration bits, pin 9) */
#define  GPIO_CFGHR_CNF9_0                           ((uint32_t)0x00000040)        /* Bit 0 */
#define  GPIO_CFGHR_CNF9_1                           ((uint32_t)0x00000080)        /* Bit 1 */

#define  GPIO_CFGHR_CNF10                            ((uint32_t)0x00000C00)        /* CNF10[1:0] bits (Port x configuration bits, pin 10) */
#define  GPIO_CFGHR_CNF10_0                          ((uint32_t)0x00000400)        /* Bit 0 */
#define  GPIO_CFGHR_CNF10_1                          ((uint32_t)0x00000800)        /* Bit 1 */

#define  GPIO_CFGHR_CNF11                            ((uint32_t)0x0000C000)        /* CNF11[1:0] bits (Port x configuration bits, pin 11) */
#define  GPIO_CFGHR_CNF11_0                          ((uint32_t)0x00004000)        /* Bit 0 */
#define  GPIO_CFGHR_CNF11_1                          ((uint32_t)0x00008000)        /* Bit 1 */

#define  GPIO_CFGHR_CNF12                            ((uint32_t)0x000C0000)        /* CNF12[1:0] bits (Port x configuration bits, pin 12) */
#define  GPIO_CFGHR_CNF12_0                          ((uint32_t)0x00040000)        /* Bit 0 */
#define  GPIO_CFGHR_CNF12_1                          ((uint32_t)0x00080000)        /* Bit 1 */

#define  GPIO_CFGHR_CNF13                            ((uint32_t)0x00C00000)        /* CNF13[1:0] bits (Port x configuration bits, pin 13) */
#define  GPIO_CFGHR_CNF13_0                          ((uint32_t)0x00400000)        /* Bit 0 */
#define  GPIO_CFGHR_CNF13_1                          ((uint32_t)0x00800000)        /* Bit 1 */

#define  GPIO_CFGHR_CNF14                            ((uint32_t)0x0C000000)        /* CNF14[1:0] bits (Port x configuration bits, pin 14) */
#define  GPIO_CFGHR_CNF14_0                          ((uint32_t)0x04000000)        /* Bit 0 */
#define  GPIO_CFGHR_CNF14_1                          ((uint32_t)0x08000000)        /* Bit 1 */

#define  GPIO_CFGHR_CNF15                            ((uint32_t)0xC0000000)        /* CNF15[1:0] bits (Port x configuration bits, pin 15) */
#define  GPIO_CFGHR_CNF15_0                          ((uint32_t)0x40000000)        /* Bit 0 */
#define  GPIO_CFGHR_CNF15_1                          ((uint32_t)0x80000000)        /* Bit 1 */

/*******************  Bit definition for GPIO_INDR register  *******************/
#define  GPIO_INDR_IDR0                              ((uint16_t)0x0001)            /* Port input data, bit 0 */
#define  GPIO_INDR_IDR1                              ((uint16_t)0x0002)            /* Port input data, bit 1 */
#define  GPIO_INDR_IDR2                              ((uint16_t)0x0004)            /* Port input data, bit 2 */
#define  GPIO_INDR_IDR3                              ((uint16_t)0x0008)            /* Port input data, bit 3 */
#define  GPIO_INDR_IDR4                              ((uint16_t)0x0010)            /* Port input data, bit 4 */
#define  GPIO_INDR_IDR5                              ((uint16_t)0x0020)            /* Port input data, bit 5 */
#define  GPIO_INDR_IDR6                              ((uint16_t)0x0040)            /* Port input data, bit 6 */
#define  GPIO_INDR_IDR7                              ((uint16_t)0x0080)            /* Port input data, bit 7 */
#define  GPIO_INDR_IDR8                              ((uint16_t)0x0100)            /* Port input data, bit 8 */
#define  GPIO_INDR_IDR9                              ((uint16_t)0x0200)            /* Port input data, bit 9 */
#define  GPIO_INDR_IDR10                             ((uint16_t)0x0400)            /* Port input data, bit 10 */
#define  GPIO_INDR_IDR11                             ((uint16_t)0x0800)            /* Port input data, bit 11 */
#define  GPIO_INDR_IDR12                             ((uint16_t)0x1000)            /* Port input data, bit 12 */
#define  GPIO_INDR_IDR13                             ((uint16_t)0x2000)            /* Port input data, bit 13 */
#define  GPIO_INDR_IDR14                             ((uint16_t)0x4000)            /* Port input data, bit 14 */
#define  GPIO_INDR_IDR15                             ((uint16_t)0x8000)            /* Port input data, bit 15 */

/*******************  Bit definition for GPIO_OUTDR register  *******************/
#define  GPIO_OUTDR_ODR0                             ((uint16_t)0x0001)            /* Port output data, bit 0 */
#define  GPIO_OUTDR_ODR1                             ((uint16_t)0x0002)            /* Port output data, bit 1 */
#define  GPIO_OUTDR_ODR2                             ((uint16_t)0x0004)            /* Port output data, bit 2 */
#define  GPIO_OUTDR_ODR3                             ((uint16_t)0x0008)            /* Port output data, bit 3 */
#define  GPIO_OUTDR_ODR4                             ((uint16_t)0x0010)            /* Port output data, bit 4 */
#define  GPIO_OUTDR_ODR5                             ((uint16_t)0x0020)            /* Port output data, bit 5 */
#define  GPIO_OUTDR_ODR6                             ((uint16_t)0x0040)            /* Port output data, bit 6 */
#define  GPIO_OUTDR_ODR7                             ((uint16_t)0x0080)            /* Port output data, bit 7 */
#define  GPIO_OUTDR_ODR8                             ((uint16_t)0x0100)            /* Port output data, bit 8 */
#define  GPIO_OUTDR_ODR9                             ((uint16_t)0x0200)            /* Port output data, bit 9 */
#define  GPIO_OUTDR_ODR10                            ((uint16_t)0x0400)            /* Port output data, bit 10 */
#define  GPIO_OUTDR_ODR11                            ((uint16_t)0x0800)            /* Port output data, bit 11 */
#define  GPIO_OUTDR_ODR12                            ((uint16_t)0x1000)            /* Port output data, bit 12 */
#define  GPIO_OUTDR_ODR13                            ((uint16_t)0x2000)            /* Port output data, bit 13 */
#define  GPIO_OUTDR_ODR14                            ((uint16_t)0x4000)            /* Port output data, bit 14 */
#define  GPIO_OUTDR_ODR15                            ((uint16_t)0x8000)            /* Port output data, bit 15 */

/******************  Bit definition for GPIO_BSHR register  *******************/
#define  GPIO_BSHR_BS0                               ((uint32_t)0x00000001)        /* Port x Set bit 0 */
#define  GPIO_BSHR_BS1                               ((uint32_t)0x00000002)        /* Port x Set bit 1 */
#define  GPIO_BSHR_BS2                               ((uint32_t)0x00000004)        /* Port x Set bit 2 */
#define  GPIO_BSHR_BS3                               ((uint32_t)0x00000008)        /* Port x Set bit 3 */
#define  GPIO_BSHR_BS4                               ((uint32_t)0x00000010)        /* Port x Set bit 4 */
#define  GPIO_BSHR_BS5                               ((uint32_t)0x00000020)        /* Port x Set bit 5 */
#define  GPIO_BSHR_BS6                               ((uint32_t)0x00000040)        /* Port x Set bit 6 */
#define  GPIO_BSHR_BS7                               ((uint32_t)0x00000080)        /* Port x Set bit 7 */
#define  GPIO_BSHR_BS8                               ((uint32_t)0x00000100)        /* Port x Set bit 8 */
#define  GPIO_BSHR_BS9                               ((uint32_t)0x00000200)        /* Port x Set bit 9 */
#define  GPIO_BSHR_BS10                              ((uint32_t)0x00000400)        /* Port x Set bit 10 */
#define  GPIO_BSHR_BS11                              ((uint32_t)0x00000800)        /* Port x Set bit 11 */
#define  GPIO_BSHR_BS12                              ((uint32_t)0x00001000)        /* Port x Set bit 12 */
#define  GPIO_BSHR_BS13                              ((uint32_t)0x00002000)        /* Port x Set bit 13 */
#define  GPIO_BSHR_BS14                              ((uint32_t)0x00004000)        /* Port x Set bit 14 */
#define  GPIO_BSHR_BS15                              ((uint32_t)0x00008000)        /* Port x Set bit 15 */

#define  GPIO_BSHR_BR0                               ((uint32_t)0x00010000)        /* Port x Reset bit 0 */
#define  GPIO_BSHR_BR1                               ((uint32_t)0x00020000)        /* Port x Reset bit 1 */
#define  GPIO_BSHR_BR2                               ((uint32_t)0x00040000)        /* Port x Reset bit 2 */
#define  GPIO_BSHR_BR3                               ((uint32_t)0x00080000)        /* Port x Reset bit 3 */
#define  GPIO_BSHR_BR4                               ((uint32_t)0x00100000)        /* Port x Reset bit 4 */
#define  GPIO_BSHR_BR5                               ((uint32_t)0x00200000)        /* Port x Reset bit 5 */
#define  GPIO_BSHR_BR6                               ((uint32_t)0x00400000)        /* Port x Reset bit 6 */
#define  GPIO_BSHR_BR7                               ((uint32_t)0x00800000)        /* Port x Reset bit 7 */
#define  GPIO_BSHR_BR8                               ((uint32_t)0x01000000)        /* Port x Reset bit 8 */
#define  GPIO_BSHR_BR9                               ((uint32_t)0x02000000)        /* Port x Reset bit 9 */
#define  GPIO_BSHR_BR10                              ((uint32_t)0x04000000)        /* Port x Reset bit 10 */
#define  GPIO_BSHR_BR11                              ((uint32_t)0x08000000)        /* Port x Reset bit 11 */
#define  GPIO_BSHR_BR12                              ((uint32_t)0x10000000)        /* Port x Reset bit 12 */
#define  GPIO_BSHR_BR13                              ((uint32_t)0x20000000)        /* Port x Reset bit 13 */
#define  GPIO_BSHR_BR14                              ((uint32_t)0x40000000)        /* Port x Reset bit 14 */
#define  GPIO_BSHR_BR15                              ((uint32_t)0x80000000)        /* Port x Reset bit 15 */

/*******************  Bit definition for GPIO_BCR register  *******************/
#define  GPIO_BCR_BR0                                ((uint16_t)0x0001)            /* Port x Reset bit 0 */
#define  GPIO_BCR_BR1                                ((uint16_t)0x0002)            /* Port x Reset bit 1 */
#define  GPIO_BCR_BR2                                ((uint16_t)0x0004)            /* Port x Reset bit 2 */
#define  GPIO_BCR_BR3                                ((uint16_t)0x0008)            /* Port x Reset bit 3 */
#define  GPIO_BCR_BR4                                ((uint16_t)0x0010)            /* Port x Reset bit 4 */
#define  GPIO_BCR_BR5                                ((uint16_t)0x0020)            /* Port x Reset bit 5 */
#define  GPIO_BCR_BR6                                ((uint16_t)0x0040)            /* Port x Reset bit 6 */
#define  GPIO_BCR_BR7                                ((uint16_t)0x0080)            /* Port x Reset bit 7 */
#define  GPIO_BCR_BR8                                ((uint16_t)0x0100)            /* Port x Reset bit 8 */
#define  GPIO_BCR_BR9                                ((uint16_t)0x0200)            /* Port x Reset bit 9 */
#define  GPIO_BCR_BR10                               ((uint16_t)0x0400)            /* Port x Reset bit 10 */
#define  GPIO_BCR_BR11                               ((uint16_t)0x0800)            /* Port x Reset bit 11 */
#define  GPIO_BCR_BR12                               ((uint16_t)0x1000)            /* Port x Reset bit 12 */
#define  GPIO_BCR_BR13                               ((uint16_t)0x2000)            /* Port x Reset bit 13 */
#define  GPIO_BCR_BR14                               ((uint16_t)0x4000)            /* Port x Reset bit 14 */
#define  GPIO_BCR_BR15                               ((uint16_t)0x8000)            /* Port x Reset bit 15 */

/******************  Bit definition for GPIO_LCKR register  *******************/
#define  GPIO_LCK0                                   ((uint32_t)0x00000001)        /* Port x Lock bit 0 */
#define  GPIO_LCK1                                   ((uint32_t)0x00000002)        /* Port x Lock bit 1 */
#define  GPIO_LCK2                                   ((uint32_t)0x00000004)        /* Port x Lock bit 2 */
#define  GPIO_LCK3                                   ((uint32_t)0x00000008)        /* Port x Lock bit 3 */
#define  GPIO_LCK4                                   ((uint32_t)0x00000010)        /* Port x Lock bit 4 */
#define  GPIO_LCK5                                   ((uint32_t)0x00000020)        /* Port x Lock bit 5 */
#define  GPIO_LCK6                                   ((uint32_t)0x00000040)        /* Port x Lock bit 6 */
#define  GPIO_LCK7                                   ((uint32_t)0x00000080)        /* Port x Lock bit 7 */
#define  GPIO_LCK8                                   ((uint32_t)0x00000100)        /* Port x Lock bit 8 */
#define  GPIO_LCK9                                   ((uint32_t)0x00000200)        /* Port x Lock bit 9 */
#define  GPIO_LCK10                                  ((uint32_t)0x00000400)        /* Port x Lock bit 10 */
#define  GPIO_LCK11                                  ((uint32_t)0x00000800)        /* Port x Lock bit 11 */
#define  GPIO_LCK12                                  ((uint32_t)0x00001000)        /* Port x Lock bit 12 */
#define  GPIO_LCK13                                  ((uint32_t)0x00002000)        /* Port x Lock bit 13 */
#define  GPIO_LCK14                                  ((uint32_t)0x00004000)        /* Port x Lock bit 14 */
#define  GPIO_LCK15                                  ((uint32_t)0x00008000)        /* Port x Lock bit 15 */
#define  GPIO_LCKK                                   ((uint32_t)0x00010000)        /* Lock key */

/******************  Bit definition for GPIO_SPEED register  *******************/
#define  GPIO_SPEED0                                 ((uint32_t)0x00000003)
#define  GPIO_SPEED1                                 ((uint32_t)0x0000000C)  
#define  GPIO_SPEED2                                 ((uint32_t)0x00000030)  
#define  GPIO_SPEED3                                 ((uint32_t)0x000000C0)  
#define  GPIO_SPEED4                                 ((uint32_t)0x00000300)  
#define  GPIO_SPEED5                                 ((uint32_t)0x00000C00)  
#define  GPIO_SPEED6                                 ((uint32_t)0x00003000)  
#define  GPIO_SPEED7                                 ((uint32_t)0x0000C000)  
#define  GPIO_SPEED8                                 ((uint32_t)0x00030000)  
#define  GPIO_SPEED9                                 ((uint32_t)0x000C0000)  
#define  GPIO_SPEED10                                ((uint32_t)0x00300000)  
#define  GPIO_SPEED11                                ((uint32_t)0x00C00000)  
#define  GPIO_SPEED12                                ((uint32_t)0x03000000)  
#define  GPIO_SPEED13                                ((uint32_t)0x0C000000)  
#define  GPIO_SPEED14                                ((uint32_t)0x30000000)    
#define  GPIO_SPEED15                                ((uint32_t)0xC0000000)  

/******************  Bit definition for AFIO_PCFR1 register  *******************/
#define  AFIO_PCFR1_PD0_1_REMAP                      ((uint32_t)0x00008000)

#define  AFIO_PCFR1_SWJ_CFG                          ((uint32_t)0x07000000)        /* SWJ_CFG[2:0] bits */
#define  AFIO_PCFR1_SWJ_CFG_0                        ((uint32_t)0x01000000)        /* Bit 0 */
#define  AFIO_PCFR1_SWJ_CFG_1                        ((uint32_t)0x02000000)        /* Bit 1 */
#define  AFIO_PCFR1_SWJ_CFG_2                        ((uint32_t)0x04000000)        /* Bit 2 */

/*****************  Bit definition for AFIO_EXTICR1 register  *****************/
#define  AFIO_EXTICR1_EXTI0                          ((uint16_t)0x000F)            /* EXTI 0 configuration */
#define  AFIO_EXTICR1_EXTI1                          ((uint16_t)0x00F0)            /* EXTI 1 configuration */
#define  AFIO_EXTICR1_EXTI2                          ((uint16_t)0x0F00)            /* EXTI 2 configuration */
#define  AFIO_EXTICR1_EXTI3                          ((uint16_t)0xF000)            /* EXTI 3 configuration */

#define  AFIO_EXTICR1_EXTI0_PA                       ((uint16_t)0x0000)            /* PA[0] pin */
#define  AFIO_EXTICR1_EXTI0_PB                       ((uint16_t)0x0001)            /* PB[0] pin */
#define  AFIO_EXTICR1_EXTI0_PC                       ((uint16_t)0x0002)            /* PC[0] pin */
#define  AFIO_EXTICR1_EXTI0_PD                       ((uint16_t)0x0003)            /* PD[0] pin */

#define  AFIO_EXTICR1_EXTI1_PA                       ((uint16_t)0x0000)            /* PA[1] pin */
#define  AFIO_EXTICR1_EXTI1_PB                       ((uint16_t)0x0010)            /* PB[1] pin */
#define  AFIO_EXTICR1_EXTI1_PC                       ((uint16_t)0x0020)            /* PC[1] pin */
#define  AFIO_EXTICR1_EXTI1_PD                       ((uint16_t)0x0030)            /* PD[1] pin */
 
#define  AFIO_EXTICR1_EXTI2_PA                       ((uint16_t)0x0000)            /* PA[2] pin */
#define  AFIO_EXTICR1_EXTI2_PB                       ((uint16_t)0x0100)            /* PB[2] pin */
#define  AFIO_EXTICR1_EXTI2_PC                       ((uint16_t)0x0200)            /* PC[2] pin */
#define  AFIO_EXTICR1_EXTI2_PD                       ((uint16_t)0x0300)            /* PD[2] pin */

#define  AFIO_EXTICR1_EXTI3_PA                       ((uint16_t)0x0000)            /* PA[3] pin */
#define  AFIO_EXTICR1_EXTI3_PB                       ((uint16_t)0x1000)            /* PB[3] pin */
#define  AFIO_EXTICR1_EXTI3_PC                       ((uint16_t)0x2000)            /* PC[3] pin */
#define  AFIO_EXTICR1_EXTI3_PD                       ((uint16_t)0x3000)            /* PD[3] pin */

/*****************  Bit definition for AFIO_EXTICR2 register  *****************/
#define  AFIO_EXTICR2_EXTI4                          ((uint16_t)0x000F)            /* EXTI 4 configuration */
#define  AFIO_EXTICR2_EXTI5                          ((uint16_t)0x00F0)            /* EXTI 5 configuration */
#define  AFIO_EXTICR2_EXTI6                          ((uint16_t)0x0F00)            /* EXTI 6 configuration */
#define  AFIO_EXTICR2_EXTI7                          ((uint16_t)0xF000)            /* EXTI 7 configuration */

#define  AFIO_EXTICR2_EXTI4_PA                       ((uint16_t)0x0000)            /* PA[4] pin */
#define  AFIO_EXTICR2_EXTI4_PB                       ((uint16_t)0x0001)            /* PB[4] pin */
#define  AFIO_EXTICR2_EXTI4_PC                       ((uint16_t)0x0002)            /* PC[4] pin */
#define  AFIO_EXTICR2_EXTI4_PD                       ((uint16_t)0x0003)            /* PD[4] pin */

#define  AFIO_EXTICR2_EXTI5_PA                       ((uint16_t)0x0000)            /* PA[5] pin */
#define  AFIO_EXTICR2_EXTI5_PB                       ((uint16_t)0x0010)            /* PB[5] pin */
#define  AFIO_EXTICR2_EXTI5_PC                       ((uint16_t)0x0020)            /* PC[5] pin */
#define  AFIO_EXTICR2_EXTI5_PD                       ((uint16_t)0x0030)            /* PD[5] pin */

#define  AFIO_EXTICR2_EXTI6_PA                       ((uint16_t)0x0000)            /* PA[6] pin */
#define  AFIO_EXTICR2_EXTI6_PB                       ((uint16_t)0x0100)            /* PB[6] pin */
#define  AFIO_EXTICR2_EXTI6_PC                       ((uint16_t)0x0200)            /* PC[6] pin */
#define  AFIO_EXTICR2_EXTI6_PD                       ((uint16_t)0x0300)            /* PD[6] pin */

#define  AFIO_EXTICR2_EXTI7_PA                       ((uint16_t)0x0000)            /* PA[7] pin */
#define  AFIO_EXTICR2_EXTI7_PB                       ((uint16_t)0x1000)            /* PB[7] pin */
#define  AFIO_EXTICR2_EXTI7_PC                       ((uint16_t)0x2000)            /* PC[7] pin */
#define  AFIO_EXTICR2_EXTI7_PD                       ((uint16_t)0x3000)            /* PD[7] pin */

/*****************  Bit definition for AFIO_EXTICR3 register  *****************/
#define  AFIO_EXTICR3_EXTI8                          ((uint16_t)0x000F)            /* EXTI 8 configuration */
#define  AFIO_EXTICR3_EXTI9                          ((uint16_t)0x00F0)            /* EXTI 9 configuration */
#define  AFIO_EXTICR3_EXTI10                         ((uint16_t)0x0F00)            /* EXTI 10 configuration */
#define  AFIO_EXTICR3_EXTI11                         ((uint16_t)0xF000)            /* EXTI 11 configuration */

#define  AFIO_EXTICR3_EXTI8_PA                       ((uint16_t)0x0000)            /* PA[8] pin */
#define  AFIO_EXTICR3_EXTI8_PB                       ((uint16_t)0x0001)            /* PB[8] pin */
#define  AFIO_EXTICR3_EXTI8_PC                       ((uint16_t)0x0002)            /* PC[8] pin */
#define  AFIO_EXTICR3_EXTI8_PD                       ((uint16_t)0x0003)            /* PD[8] pin */

#define  AFIO_EXTICR3_EXTI9_PA                       ((uint16_t)0x0000)            /* PA[9] pin */
#define  AFIO_EXTICR3_EXTI9_PB                       ((uint16_t)0x0010)            /* PB[9] pin */
#define  AFIO_EXTICR3_EXTI9_PC                       ((uint16_t)0x0020)            /* PC[9] pin */
#define  AFIO_EXTICR3_EXTI9_PD                       ((uint16_t)0x0030)            /* PD[9] pin */
 
#define  AFIO_EXTICR3_EXTI10_PA                      ((uint16_t)0x0000)            /* PA[10] pin */
#define  AFIO_EXTICR3_EXTI10_PB                      ((uint16_t)0x0100)            /* PB[10] pin */
#define  AFIO_EXTICR3_EXTI10_PC                      ((uint16_t)0x0200)            /* PC[10] pin */
#define  AFIO_EXTICR3_EXTI10_PD                      ((uint16_t)0x0300)            /* PD[10] pin */

#define  AFIO_EXTICR3_EXTI11_PA                      ((uint16_t)0x0000)            /* PA[11] pin */
#define  AFIO_EXTICR3_EXTI11_PB                      ((uint16_t)0x1000)            /* PB[11] pin */
#define  AFIO_EXTICR3_EXTI11_PC                      ((uint16_t)0x2000)            /* PC[11] pin */
#define  AFIO_EXTICR3_EXTI11_PD                      ((uint16_t)0x3000)            /* PD[11] pin */

/*****************  Bit definition for AFIO_EXTICR4 register  *****************/
#define  AFIO_EXTICR4_EXTI12                         ((uint16_t)0x000F)            /* EXTI 12 configuration */
#define  AFIO_EXTICR4_EXTI13                         ((uint16_t)0x00F0)            /* EXTI 13 configuration */
#define  AFIO_EXTICR4_EXTI14                         ((uint16_t)0x0F00)            /* EXTI 14 configuration */
#define  AFIO_EXTICR4_EXTI15                         ((uint16_t)0xF000)            /* EXTI 15 configuration */

#define  AFIO_EXTICR4_EXTI12_PA                      ((uint16_t)0x0000)            /* PA[12] pin */
#define  AFIO_EXTICR4_EXTI12_PB                      ((uint16_t)0x0001)            /* PB[12] pin */
#define  AFIO_EXTICR4_EXTI12_PC                      ((uint16_t)0x0002)            /* PC[12] pin */
#define  AFIO_EXTICR4_EXTI12_PD                      ((uint16_t)0x0003)            /* PD[12] pin */

#define  AFIO_EXTICR4_EXTI13_PA                      ((uint16_t)0x0000)            /* PA[13] pin */
#define  AFIO_EXTICR4_EXTI13_PB                      ((uint16_t)0x0010)            /* PB[13] pin */
#define  AFIO_EXTICR4_EXTI13_PC                      ((uint16_t)0x0020)            /* PC[13] pin */
#define  AFIO_EXTICR4_EXTI13_PD                      ((uint16_t)0x0030)            /* PD[13] pin */

#define  AFIO_EXTICR4_EXTI14_PA                      ((uint16_t)0x0000)            /* PA[14] pin */
#define  AFIO_EXTICR4_EXTI14_PB                      ((uint16_t)0x0100)            /* PB[14] pin */
#define  AFIO_EXTICR4_EXTI14_PC                      ((uint16_t)0x0200)            /* PC[14] pin */
#define  AFIO_EXTICR4_EXTI14_PD                      ((uint16_t)0x0300)            /* PD[14] pin */

#define  AFIO_EXTICR4_EXTI15_PA                      ((uint16_t)0x0000)            /* PA[15] pin */
#define  AFIO_EXTICR4_EXTI15_PB                      ((uint16_t)0x1000)            /* PB[15] pin */
#define  AFIO_EXTICR4_EXTI15_PC                      ((uint16_t)0x2000)            /* PC[15] pin */
#define  AFIO_EXTICR4_EXTI15_PD                      ((uint16_t)0x3000)            /* PD[15] pin */

/******************  Bit definition for AFIO_CR register  *******************/
#define  AFIO_CR_USBPDHVT                            ((uint32_t)0x00000001)   

#define  AFIO_CR_USBPDRISE                           ((uint32_t)0x00000006)        /* USBPDRISE[1:0] bit*/
#define  AFIO_CR_USBPDRISE_0                         ((uint32_t)0x00000002) 
#define  AFIO_CR_USBPDRISE_1                         ((uint32_t)0x00000004) 

/******************  Bit definition for AFIO_AFLR register  *******************/
#define  AFIO_AFLR_AFR0                              ((uint32_t)0x0000000F)   
#define  AFIO_AFLR_AFR1                              ((uint32_t)0x000000F0)
#define  AFIO_AFLR_AFR2                              ((uint32_t)0x00000F00)
#define  AFIO_AFLR_AFR3                              ((uint32_t)0x0000F000)
#define  AFIO_AFLR_AFR4                              ((uint32_t)0x000F0000)
#define  AFIO_AFLR_AFR5                              ((uint32_t)0x00F00000)
#define  AFIO_AFLR_AFR6                              ((uint32_t)0x0F000000)
#define  AFIO_AFLR_AFR7                              ((uint32_t)0xF0000000)

/******************  Bit definition for AFIO_AFHR register  *******************/
#define  AFIO_AFHR_AFR8                              ((uint32_t)0x0000000F)   
#define  AFIO_AFHR_AFR9                              ((uint32_t)0x000000F0)
#define  AFIO_AFHR_AFR10                             ((uint32_t)0x00000F00)
#define  AFIO_AFHR_AFR11                             ((uint32_t)0x0000F000)
#define  AFIO_AFHR_AFR12                             ((uint32_t)0x000F0000)
#define  AFIO_AFHR_AFR13                             ((uint32_t)0x00F00000)
#define  AFIO_AFHR_AFR14                             ((uint32_t)0x0F000000)
#define  AFIO_AFHR_AFR15                             ((uint32_t)0xF0000000)

/******************************************************************************/
/*                           Independent WATCHDOG                             */
/******************************************************************************/

/*******************  Bit definition for IWDG_CTLR register  ********************/
#define  IWDG_KEY                                    ((uint16_t)0xFFFF)            /* Key value (write only, read 0000h) */

/*******************  Bit definition for IWDG_PSCR register  ********************/
#define  IWDG_PR                                     ((uint8_t)0x07)               /* PR[2:0] (Prescaler divider) */
#define  IWDG_PR_0                                   ((uint8_t)0x01)               /* Bit 0 */
#define  IWDG_PR_1                                   ((uint8_t)0x02)               /* Bit 1 */
#define  IWDG_PR_2                                   ((uint8_t)0x04)               /* Bit 2 */

/*******************  Bit definition for IWDG_RLDR register  *******************/
#define  IWDG_RL                                     ((uint16_t)0x0FFF)            /* Watchdog counter reload value */

/*******************  Bit definition for IWDG_STATR register  ********************/
#define  IWDG_PVU                                    ((uint8_t)0x01)               /* Watchdog prescaler value update */
#define  IWDG_RVU                                    ((uint8_t)0x02)               /* Watchdog counter reload value update */

/******************************************************************************/
/*                      Inter-integrated Circuit Interface                    */
/******************************************************************************/

/*******************  Bit definition for I2C_CTLR1 register  ********************/
#define  I2C_CTLR1_PE                                ((uint16_t)0x0001)            /* Peripheral Enable */
#define  I2C_CTLR1_ENPEC                             ((uint16_t)0x0020)            /* PEC Enable */
#define  I2C_CTLR1_ENGC                              ((uint16_t)0x0040)            /* General Call Enable */
#define  I2C_CTLR1_NOSTRETCH                         ((uint16_t)0x0080)            /* Clock Stretching Disable (Slave mode) */
#define  I2C_CTLR1_START                             ((uint16_t)0x0100)            /* Start Generation */
#define  I2C_CTLR1_STOP                              ((uint16_t)0x0200)            /* Stop Generation */
#define  I2C_CTLR1_ACK                               ((uint16_t)0x0400)            /* Acknowledge Enable */
#define  I2C_CTLR1_POS                               ((uint16_t)0x0800)            /* Acknowledge/PEC Position (for data reception) */
#define  I2C_CTLR1_PEC                               ((uint16_t)0x1000)            /* Packet Error Checking */
#define  I2C_CTLR1_SWRST                             ((uint16_t)0x8000)            /* Software Reset */

/*******************  Bit definition for I2C_CTLR2 register  ********************/
#define  I2C_CTLR2_FREQ                              ((uint16_t)0x003F)            /* FREQ[5:0] bits (Peripheral Clock Frequency) */
#define  I2C_CTLR2_FREQ_0                            ((uint16_t)0x0001)            /* Bit 0 */
#define  I2C_CTLR2_FREQ_1                            ((uint16_t)0x0002)            /* Bit 1 */
#define  I2C_CTLR2_FREQ_2                            ((uint16_t)0x0004)            /* Bit 2 */
#define  I2C_CTLR2_FREQ_3                            ((uint16_t)0x0008)            /* Bit 3 */
#define  I2C_CTLR2_FREQ_4                            ((uint16_t)0x0010)            /* Bit 4 */
#define  I2C_CTLR2_FREQ_5                            ((uint16_t)0x0020)            /* Bit 5 */

#define  I2C_CTLR2_ITERREN                           ((uint16_t)0x0100)            /* Error Interrupt Enable */
#define  I2C_CTLR2_ITEVTEN                           ((uint16_t)0x0200)            /* Event Interrupt Enable */
#define  I2C_CTLR2_ITBUFEN                           ((uint16_t)0x0400)            /* Buffer Interrupt Enable */
#define  I2C_CTLR2_DMAEN                             ((uint16_t)0x0800)            /* DMA Requests Enable */
#define  I2C_CTLR2_LAST                              ((uint16_t)0x1000)            /* DMA Last Transfer */

/*******************  Bit definition for I2C_OADDR1 register  *******************/
#define  I2C_OADDR1_ADD0                             ((uint16_t)0x0001)
#define  I2C_OADDR1_ADD1_7                           ((uint16_t)0x00FE)            /* Interface Address */
#define  I2C_OADDR1_ADD8_9                           ((uint16_t)0x0300)            /* Interface Address */

#define  I2C_OADDR1_ADD0                             ((uint16_t)0x0001)            /* Bit 0 */
#define  I2C_OADDR1_ADD1                             ((uint16_t)0x0002)            /* Bit 1 */
#define  I2C_OADDR1_ADD2                             ((uint16_t)0x0004)            /* Bit 2 */
#define  I2C_OADDR1_ADD3                             ((uint16_t)0x0008)            /* Bit 3 */
#define  I2C_OADDR1_ADD4                             ((uint16_t)0x0010)            /* Bit 4 */
#define  I2C_OADDR1_ADD5                             ((uint16_t)0x0020)            /* Bit 5 */
#define  I2C_OADDR1_ADD6                             ((uint16_t)0x0040)            /* Bit 6 */
#define  I2C_OADDR1_ADD7                             ((uint16_t)0x0080)            /* Bit 7 */
#define  I2C_OADDR1_ADD8                             ((uint16_t)0x0100)            /* Bit 8 */
#define  I2C_OADDR1_ADD9                             ((uint16_t)0x0200)            /* Bit 9 */

#define  I2C_OADDR1_ADDMODE                          ((uint16_t)0x8000)            /* Addressing Mode (Slave mode) */

/*******************  Bit definition for I2C_OADDR2 register  *******************/
#define  I2C_OADDR2_ENDUAL                           ((uint8_t)0x01)               /* Dual addressing mode enable */
#define  I2C_OADDR2_ADD2                             ((uint8_t)0xFE)               /* Interface address */

/********************  Bit definition for I2C_DATAR register  ********************/
#define  I2C_DR_DATAR                                ((uint8_t)0xFF)               /* 8-bit Data Register */

/*******************  Bit definition for I2C_STAR1 register  ********************/
#define  I2C_STAR1_SB                                ((uint16_t)0x0001)            /* Start Bit (Master mode) */
#define  I2C_STAR1_ADDR                              ((uint16_t)0x0002)            /* Address sent (master mode)/matched (slave mode) */
#define  I2C_STAR1_BTF                               ((uint16_t)0x0004)            /* Byte Transfer Finished */
#define  I2C_STAR1_ADD10                             ((uint16_t)0x0008)            /* 10-bit header sent (Master mode) */
#define  I2C_STAR1_STOPF                             ((uint16_t)0x0010)            /* Stop detection (Slave mode) */
#define  I2C_STAR1_RXNE                              ((uint16_t)0x0040)            /* Data Register not Empty (receivers) */
#define  I2C_STAR1_TXE                               ((uint16_t)0x0080)            /* Data Register Empty (transmitters) */
#define  I2C_STAR1_BERR                              ((uint16_t)0x0100)            /* Bus Error */
#define  I2C_STAR1_ARLO                              ((uint16_t)0x0200)            /* Arbitration Lost (master mode) */
#define  I2C_STAR1_AF                                ((uint16_t)0x0400)            /* Acknowledge Failure */
#define  I2C_STAR1_OVR                               ((uint16_t)0x0800)            /* Overrun/Underrun */
#define  I2C_STAR1_PECERR                            ((uint16_t)0x1000)            /* PEC Error in reception */

/*******************  Bit definition for I2C_STAR2 register  ********************/
#define  I2C_STAR2_MSL                               ((uint16_t)0x0001)            /* Master/Slave */
#define  I2C_STAR2_BUSY                              ((uint16_t)0x0002)            /* Bus Busy */
#define  I2C_STAR2_TRA                               ((uint16_t)0x0004)            /* Transmitter/Receiver */
#define  I2C_STAR2_GENCALL                           ((uint16_t)0x0010)            /* General Call Address (Slave mode) */
#define  I2C_STAR2_DUALF                             ((uint16_t)0x0080)            /* Dual Flag (Slave mode) */
#define  I2C_STAR2_PEC                               ((uint16_t)0xFF00)            /* Packet Error Checking Register */

/*******************  Bit definition for I2C_CKCFGR register  ********************/
#define  I2C_CKCFGR_CCR                              ((uint16_t)0x0FFF)            /* Clock Control Register in Fast/Standard mode (Master mode) */
#define  I2C_CKCFGR_DUTY                             ((uint16_t)0x4000)            /* Fast Mode Duty Cycle */
#define  I2C_CKCFGR_FS                               ((uint16_t)0x8000)            /* I2C Master Mode Selection */

/******************************************************************************/
/*                             Power Control                                  */
/******************************************************************************/

/********************  Bit definition for PWR_CTLR register  ********************/
#define  PWR_CTLR_LPDS                               ((uint16_t)0x0001)     /* Low-Power Deepsleep */
#define  PWR_CTLR_PVDE                               ((uint16_t)0x0010)     /* Power Voltage Detector Enable */

#define  PWR_CTLR_PLS                                ((uint16_t)0x0060)     /* PLS[1:0] bits (PVD Level Selection) */
#define  PWR_CTLR_PLS_0                              ((uint16_t)0x0020)     /* Bit 0 */
#define  PWR_CTLR_PLS_1                              ((uint16_t)0x0040)     /* Bit 1 */

#define  PWR_CTLR_PLS_MODE0                          ((uint16_t)0x0000)     
#define  PWR_CTLR_PLS_MODE1                          ((uint16_t)0x0020)     
#define  PWR_CTLR_PLS_MODE2                          ((uint16_t)0x0040)     
#define  PWR_CTLR_PLS_MODE3                          ((uint16_t)0x0060)     

#define  PWR_CTLR_PLAN                               ((uint16_t)0x0080)     

#define  PWR_CTLR_LDO_VDDK                           ((uint16_t)0x0700)   
#define  PWR_CTLR_LDO_VDD12A                         ((uint16_t)0x3800)

/*******************  Bit definition for PWR_CSR register  ********************/
#define  PWR_CSR_PVDO                                ((uint16_t)0x0001)     /* PVD Output */

/******************************************************************************/
/*                         Reset and Clock Control                            */
/******************************************************************************/

/********************  Bit definition for RCC_CTLR register  ********************/
#define  RCC_HSION                                   ((uint32_t)0x00000001)        /* Internal High Speed clock enable */
#define  RCC_HSIRDY                                  ((uint32_t)0x00000002)        /* Internal High Speed clock ready flag */
#define  RCC_LPMSRC                                  ((uint32_t)0x00000100)  
#define  RCC_LPMGATE                                 ((uint32_t)0x00000200)
#define  RCC_HSEON                                   ((uint32_t)0x00010000)        /* External High Speed clock enable */
#define  RCC_HSERDY                                  ((uint32_t)0x00020000)        /* External High Speed clock ready flag */
#define  RCC_HSEBYP                                  ((uint32_t)0x00040000)        /* External High Speed clock Bypass */
#define  RCC_CSSON                                   ((uint32_t)0x00080000)        /* Clock Security System enable */

#define  RCC_USBHSPLLON                              ((uint32_t)0x01000000)        /* PLL enable */
#define  RCC_USBHSPLLRDY                             ((uint32_t)0x02000000)        /* PLL clock ready flag */
#define  RCC_USBSSPLLON                              ((uint32_t)0x04000000)
#define  RCC_USBSSPLLRDY                             ((uint32_t)0x08000000)

#define  RCC_PIPEON                                  ((uint32_t)0x10000000)
#define  RCC_PIXISTAT                                ((uint32_t)0x20000000)
#define  RCC_PLLGATE                                 ((uint32_t)0x80000000)

/*******************  Bit definition for RCC_CFGR0 register  *******************/
#define  RCC_SW                                      ((uint32_t)0x00000003)        /* SW[1:0] bits (System clock Switch) */
#define  RCC_SW_0                                    ((uint32_t)0x00000001)        /* Bit 0 */
#define  RCC_SW_1                                    ((uint32_t)0x00000002)        /* Bit 1 */

#define  RCC_SW_HSI                                  ((uint32_t)0x00000000)        /* HSI selected as system clock */
#define  RCC_SW_HSE                                  ((uint32_t)0x00000001)        /* HSE selected as system clock */
#define  RCC_SW_PLL                                  ((uint32_t)0x00000002)        /* PLL selected as system clock */

#define  RCC_SWS                                     ((uint32_t)0x0000000C)        /* SWS[1:0] bits (System Clock Switch Status) */
#define  RCC_SWS_0                                   ((uint32_t)0x00000004)        /* Bit 0 */
#define  RCC_SWS_1                                   ((uint32_t)0x00000008)        /* Bit 1 */

#define  RCC_SWS_HSI                                 ((uint32_t)0x00000000)        /* HSI oscillator used as system clock */
#define  RCC_SWS_HSE                                 ((uint32_t)0x00000004)        /* HSE oscillator used as system clock */
#define  RCC_SWS_PLL                                 ((uint32_t)0x00000008)        /* PLL used as system clock */

#define  RCC_HPRE                                    ((uint32_t)0x000000F0)        /* HPRE[3:0] bits (HB prescaler) */
#define  RCC_HPRE_0                                  ((uint32_t)0x00000010)        /* Bit 0 */
#define  RCC_HPRE_1                                  ((uint32_t)0x00000020)        /* Bit 1 */
#define  RCC_HPRE_2                                  ((uint32_t)0x00000040)        /* Bit 2 */
#define  RCC_HPRE_3                                  ((uint32_t)0x00000080)        /* Bit 3 */

#define  RCC_HPRE_DIV1                               ((uint32_t)0x00000000)        /* SYSCLK not divided */
#define  RCC_HPRE_DIV2                               ((uint32_t)0x00000080)        /* SYSCLK divided by 2 */
#define  RCC_HPRE_DIV4                               ((uint32_t)0x00000090)        /* SYSCLK divided by 4 */
#define  RCC_HPRE_DIV8                               ((uint32_t)0x000000A0)        /* SYSCLK divided by 8 */
#define  RCC_HPRE_DIV16                              ((uint32_t)0x000000B0)        /* SYSCLK divided by 16 */
#define  RCC_HPRE_DIV64                              ((uint32_t)0x000000C0)        /* SYSCLK divided by 64 */
#define  RCC_HPRE_DIV128                             ((uint32_t)0x000000D0)        /* SYSCLK divided by 128 */
#define  RCC_HPRE_DIV256                             ((uint32_t)0x000000E0)        /* SYSCLK divided by 256 */
#define  RCC_HPRE_DIV512                             ((uint32_t)0x000000F0)        /* SYSCLK divided by 512 */

#define  RCC_FPRE                                    ((uint32_t)0x00000100)        /* FPRE bits (HCLK source prescaler) */

#define  RCC_FPRE_DIV1                               ((uint32_t)0x00000000)
#define  RCC_FPRE_DIV2                               ((uint32_t)0x00000100)

#define  RCC_PPRE2                                   ((uint32_t)0x00003800)        /* PRE2[2:0] bits (PB2 prescaler) */
#define  RCC_PPRE2_0                                 ((uint32_t)0x00000800)        /* Bit 0 */
#define  RCC_PPRE2_1                                 ((uint32_t)0x00001000)        /* Bit 1 */
#define  RCC_PPRE2_2                                 ((uint32_t)0x00002000)        /* Bit 2 */

#define  RCC_PPRE2_DIV1                              ((uint32_t)0x00000000)        /* HCLK not divided */
#define  RCC_PPRE2_DIV2                              ((uint32_t)0x00002000)        /* HCLK divided by 2 */
#define  RCC_PPRE2_DIV4                              ((uint32_t)0x00002800)        /* HCLK divided by 4 */
#define  RCC_PPRE2_DIV8                              ((uint32_t)0x00003000)        /* HCLK divided by 8 */
#define  RCC_PPRE2_DIV16                             ((uint32_t)0x00003800)        /* HCLK divided by 16 */

#define  RCC_ADCPRE                                  ((uint32_t)0x0000C000)        /* ADCPRE[1:0] bits (ADC prescaler) */
#define  RCC_ADCPRE_0                                ((uint32_t)0x00004000)        /* Bit 0 */
#define  RCC_ADCPRE_1                                ((uint32_t)0x00008000)        /* Bit 1 */

#define  RCC_ADCPRE_DIV2                             ((uint32_t)0x00000000)        /* PCLK2 divided by 2 */
#define  RCC_ADCPRE_DIV4                             ((uint32_t)0x00004000)        /* PCLK2 divided by 4 */
#define  RCC_ADCPRE_DIV6                             ((uint32_t)0x00008000)        /* PCLK2 divided by 6 */
#define  RCC_ADCPRE_DIV8                             ((uint32_t)0x0000C000)        /* PCLK2 divided by 8 */

#define  RCC_PLLDIV                                  ((uint32_t)0x001E0000)        /* PLLDIV[3:0] bits (PLL multiplication factor) */
#define  RCC_PLLDIV_0                                ((uint32_t)0x00020000)        /* Bit 0 */
#define  RCC_PLLDIV_1                                ((uint32_t)0x00040000)        /* Bit 1 */
#define  RCC_PLLDIV_2                                ((uint32_t)0x00080000)        /* Bit 2 */
#define  RCC_PLLDIV_3                                ((uint32_t)0x00100000)        /* Bit 3 */
 
#define  RCC_PLLDIV2                                 ((uint32_t)0x00020000)
#define  RCC_PLLDIV3                                 ((uint32_t)0x00040000)
#define  RCC_PLLDIV4                                 ((uint32_t)0x00060000)
#define  RCC_PLLDIV5                                 ((uint32_t)0x00080000)
#define  RCC_PLLDIV6                                 ((uint32_t)0x000A0000)
#define  RCC_PLLDIV7                                 ((uint32_t)0x000C0000)
#define  RCC_PLLDIV8                                 ((uint32_t)0x000E0000)
#define  RCC_PLLDIV9                                 ((uint32_t)0x00100000)
#define  RCC_PLLDIV10                                ((uint32_t)0x00120000)
#define  RCC_PLLDIV11                                ((uint32_t)0x00140000)
#define  RCC_PLLDIV12                                ((uint32_t)0x00160000)
#define  RCC_PLLDIV13                                ((uint32_t)0x00180000)
#define  RCC_PLLDIV14                                ((uint32_t)0x001A0000)
#define  RCC_PLLDIV15                                ((uint32_t)0x001C0000)
#define  RCC_PLLDIV16                                ((uint32_t)0x001E0000)

#define  RCC_PLLSRC                                  ((uint32_t)0x00600000) 
#define  RCC_PLLSRC_1                                ((uint32_t)0x00200000) 
#define  RCC_PLLSRC_2                                ((uint32_t)0x00400000) 

#define  RCC_PLLSRC_USBHSPLL_480M                    ((uint32_t)0x00000000)        /* USBHSPLL 480M clock selected as PLL entry clock source */
#define  RCC_PLLSRC_HS                               ((uint32_t)0x00600000)        /* PLL entry clock source is determined by PLLHS_SRC */
#define  RCC_PLLSRC_HS_1                             ((uint32_t)0x00200000)
#define  RCC_PLLSRC_USBSSPLL_625M                    ((uint32_t)0x00400000)        /* USBSSPLL 625M clock selected as PLL entry clock source */

#define  RCC_UTMION                                  ((uint32_t)0x00800000) 

#define  RCC_CFGR0_MCO                               ((uint32_t)0x0F000000)        /* MCO[3:0] bits (Microcontroller Clock Output) */
#define  RCC_MCO_0                                   ((uint32_t)0x01000000)        /* Bit 0 */
#define  RCC_MCO_1                                   ((uint32_t)0x02000000)        /* Bit 1 */
#define  RCC_MCO_2                                   ((uint32_t)0x04000000)        /* Bit 2 */
#define  RCC_MCO_3                                   ((uint32_t)0x08000000)        /* Bit 3 */

#define  RCC_MCO_NOCLOCK                             ((uint32_t)0x00000000)        /* No clock */
#define  RCC_CFGR0_MCO_SYSCLK                        ((uint32_t)0x04000000)        /* System clock selected as MCO source */
#define  RCC_CFGR0_MCO_HSI                           ((uint32_t)0x05000000)        /* HSI clock selected as MCO source */
#define  RCC_CFGR0_MCO_HSE                           ((uint32_t)0x06000000)        /* HSE clock selected as MCO source  */
#define  RCC_CFGR0_MCO_PLLD8                         ((uint32_t)0x07000000)        /* PLL clock divided by 8 selected as MCO source */

#define  RCC_PLLHS_SRC                               ((uint32_t)0x30000000)
#define  RCC_PLLHS_SRC_0                             ((uint32_t)0x10000000)
#define  RCC_PLLHS_SRC_1                             ((uint32_t)0x20000000)

#define  RCC_PLLHS_SRC_USBSSPLL_125M                 ((uint32_t)0x00000000)
#define  RCC_PLLHS_SRC_USBSSPLL_357M                 ((uint32_t)0x10000000)
#define  RCC_PLLHS_SRC_USBSSPLL_625M                 ((uint32_t)0x20000000)        /* Only applicable to chips with a batch number greater than zero in the fifth digit */
#define  RCC_PLLHS_SRC_USBHSPLL_480M                 ((uint32_t)0x30000000)

/*******************  Bit definition for RCC_INTR register  ********************/
#define  RCC_LSIRDYF                                 ((uint32_t)0x00000001)        /* LSI Ready Interrupt flag */
#define  RCC_LSERDYF                                 ((uint32_t)0x00000002)        /* LSE Ready Interrupt flag */
#define  RCC_HSIRDYF                                 ((uint32_t)0x00000004)        /* HSI Ready Interrupt flag */
#define  RCC_HSERDYF                                 ((uint32_t)0x00000008)        /* HSE Ready Interrupt flag */
#define  RCC_CSSF                                    ((uint32_t)0x00000080)        /* Clock Security System Interrupt flag */
#define  RCC_LSIRDYIE                                ((uint32_t)0x00000100)        /* LSI Ready Interrupt Enable */
#define  RCC_LSERDYIE                                ((uint32_t)0x00000200)        /* LSE Ready Interrupt Enable */
#define  RCC_HSIRDYIE                                ((uint32_t)0x00000400)        /* HSI Ready Interrupt Enable */
#define  RCC_HSERDYIE                                ((uint32_t)0x00000800)        /* HSE Ready Interrupt Enable */

/*****************  Bit definition for RCC_HB2PRSTR register  *****************/
#define  RCC_AFIORST                                 ((uint32_t)0x00000001)        /* Alternate Function I/O reset */
#define  RCC_IOPARST                                 ((uint32_t)0x00000004)        /* I/O port A reset */
#define  RCC_IOPBRST                                 ((uint32_t)0x00000008)        /* I/O port B reset */
#define  RCC_IOPCRST                                 ((uint32_t)0x00000010)        /* I/O port C reset */
#define  RCC_IOPDRST                                 ((uint32_t)0x00000020)        /* I/O port D reset */
#define  RCC_ADC1RST                                 ((uint32_t)0x00000200)        /* ADC 1 interface reset */
#define  RCC_ADC2RST                                 ((uint32_t)0x00000400)        /* ADC 2 interface reset */
#define  RCC_TIM1RST                                 ((uint32_t)0x00000800)        /* TIM1 Timer reset */
#define  RCC_SPI1RST                                 ((uint32_t)0x00001000)        /* SPI 1 reset */
#define  RCC_USART1RST                               ((uint32_t)0x00004000)        /* USART1 reset */
#define  RCC_ADC3RST                                 ((uint32_t)0x00008000)        /* ADC 3 interface reset */
#define  RCC_ADC4RST                                 ((uint32_t)0x00010000)        /* ADC 4 interface reset */

/*****************  Bit definition for RCC_HB1PRSTR register  *****************/
#define  RCC_TIM2RST                                 ((uint32_t)0x00000001)        /* Timer 2 reset */
#define  RCC_TIM3RST                                 ((uint32_t)0x00000002)        /* Timer 3 reset */
#define  RCC_TIM4RST                                 ((uint32_t)0x00000004)        /* Timer 4 reset */
#define  RCC_WWDGRST                                 ((uint32_t)0x00000800)        /* Window Watchdog reset */
#define  RCC_SPI2RST                                 ((uint32_t)0x00004000)        /* SPI 2 reset */
#define  RCC_SPI3RST                                 ((uint32_t)0x00008000)        /* SPI 3 reset */
#define  RCC_USART2RST                               ((uint32_t)0x00020000)        /* USART 2 reset */
#define  RCC_USART3RST                               ((uint32_t)0x00040000)        /* USART 3 reset */
#define  RCC_USART4RST                               ((uint32_t)0x00080000)        /* USART 4 reset */
#define  RCC_I2C1RST                                 ((uint32_t)0x00200000)        /* I2C 1 reset */
#define  RCC_I2C2RST                                 ((uint32_t)0x00400000)        /* I2C 2 reset */ 
#define  RCC_PWRRST                                  ((uint32_t)0x10000000)        /* Power interface reset */

/******************  Bit definition for RCC_HBPCENR register  ******************/
#define  RCC_DMA1EN                                  ((uint16_t)0x0001)            /* DMA1 clock enable */
#define  RCC_CRCEN                                   ((uint16_t)0x0040)            /* CRC clock enable */
#define  RCC_USBHSEN                                 ((uint16_t)0x0800)
#define  RCC_USBPDEN                                 ((uint16_t)0x1000)
#define  RCC_ARGBEN                                  ((uint16_t)0x4000)
#define  RCC_USBSSEN                                 ((uint16_t)0x8000)

/******************  Bit definition for RCC_HB2PCENR register  *****************/
#define  RCC_AFIOEN                                  ((uint32_t)0x00000001)         /* Alternate Function I/O clock enable */
#define  RCC_IOPAEN                                  ((uint32_t)0x00000004)         /* I/O port A clock enable */
#define  RCC_IOPBEN                                  ((uint32_t)0x00000008)         /* I/O port B clock enable */
#define  RCC_IOPCEN                                  ((uint32_t)0x00000010)         /* I/O port C clock enable */
#define  RCC_IOPDEN                                  ((uint32_t)0x00000020)         /* I/O port D clock enable */

#define  RCC_ADC1EN                                  ((uint32_t)0x00000200)         /* ADC 1 interface clock enable */
#define  RCC_ADC2EN                                  ((uint32_t)0x00000400)         /* ADC 2 interface clock enable */
#define  RCC_TIM1EN                                  ((uint32_t)0x00000800)         /* TIM1 Timer clock enable */
#define  RCC_SPI1EN                                  ((uint32_t)0x00001000)         /* SPI 1 clock enable */
#define  RCC_USART1EN                                ((uint32_t)0x00004000)         /* USART1 clock enable */
#define  RCC_ADC3EN                                  ((uint32_t)0x00008000)         /* ADC 3 interface clock enable */
#define  RCC_ADC4EN                                  ((uint32_t)0x00010000)         /* ADC 4 interface clock enable */

/*****************  Bit definition for RCC_HB1PCENR register  ******************/
#define  RCC_TIM2EN                                  ((uint32_t)0x00000001)        /* Timer 2 clock enabled*/
#define  RCC_TIM3EN                                  ((uint32_t)0x00000002)        /* Timer 3 clock enable */
#define  RCC_TIM4EN                                  ((uint32_t)0x00000004)
#define  RCC_WWDGEN                                  ((uint32_t)0x00000800)        /* Window Watchdog clock enable */
#define  RCC_SPI2EN                                  ((uint32_t)0x00004000)
#define  RCC_SPI3EN                                  ((uint32_t)0x00008000)
#define  RCC_USART2EN                                ((uint32_t)0x00020000)        /* USART 2 clock enable */
#define  RCC_USART3EN                                ((uint32_t)0x00040000)
#define  RCC_USART4EN                                ((uint32_t)0x00080000)
#define  RCC_I2C1EN                                  ((uint32_t)0x00200000)        /* I2C 1 clock enable */
#define  RCC_I2C2EN                                  ((uint32_t)0x00400000)
#define  RCC_PWREN                                   ((uint32_t)0x10000000)        /* Power interface clock enable */

/*******************  Bit definition for RCC_RSTSCKR register  ********************/  
#define  RCC_LSION                                   ((uint32_t)0x00000001)        /* Internal Low Speed oscillator enable */
#define  RCC_LSIRDY                                  ((uint32_t)0x00000002)        /* Internal Low Speed oscillator Ready */

#define  RCC_LKUPRSTF                                ((uint32_t)0x00040000)        /* LOCKUP reset flag */
#define  RCC_ADC3AWDRSTF                             ((uint32_t)0x00080000)        /* ADC3 AWD reset flag */
#define  RCC_ADC4AWDRSTF                             ((uint32_t)0x00100000)        /* ADC4 AWD reset flag */
#define  RCC_USBPDRSTF                               ((uint32_t)0x00200000)        /* USBPD reset flag */
#define  RCC_ADC1AWDRSTF                             ((uint32_t)0x00400000)        /* ADC1 AWD reset flag */
#define  RCC_ADC2AWDRSTF                             ((uint32_t)0x00800000)        /* ADC2 AWD reset flag */
#define  RCC_RMVF                                    ((uint32_t)0x01000000)        /* Remove reset flag */
#define  RCC_PINRSTF                                 ((uint32_t)0x04000000)        /* PIN reset flag */
#define  RCC_PORRSTF                                 ((uint32_t)0x08000000)        /* POR/PDR reset flag */
#define  RCC_SFTRSTF                                 ((uint32_t)0x10000000)        /* Software Reset flag */
#define  RCC_IWDGRSTF                                ((uint32_t)0x20000000)        /* Independent Watchdog reset flag */
#define  RCC_WWDGRSTF                                ((uint32_t)0x40000000)        /* Window watchdog reset flag */

/*******************  Bit definition for RCC_HBRSTR register  ********************/  
#define  RCC_USBHSRST                                ((uint32_t)0x00000800)    
#define  RCC_USBPDRST                                ((uint32_t)0x00001000)        
#define  RCC_ARGBRST                                 ((uint32_t)0x00004000)        
#define  RCC_USBSSRST                                ((uint32_t)0x00008000)      

/*******************  Bit definition for RCC_CFGR2 register  ********************/  
#define  RCC_USBHSPLLREFSEL                          ((uint32_t)0x00000007)   
#define  RCC_USBHSPLLREFSEL_0                        ((uint32_t)0x00000001) 
#define  RCC_USBHSPLLREFSEL_1                        ((uint32_t)0x00000002) 
#define  RCC_USBHSPLLREFSEL_2                        ((uint32_t)0x00000004) 

#define  RCC_USBHSPLLREFSEL_12MHz                    ((uint32_t)0x00000000) 
#define  RCC_USBHSPLLREFSEL_20MHz                    ((uint32_t)0x00000001)
#define  RCC_USBHSPLLREFSEL_24MHz                    ((uint32_t)0x00000002)
#define  RCC_USBHSPLLREFSEL_25MHz                    ((uint32_t)0x00000003)
#define  RCC_USBHSPLLREFSEL_32MHz                    ((uint32_t)0x00000004)

#define  RCC_USBSSPLLREFSEL                          ((uint32_t)0x00000038)
#define  RCC_USBSSPLLREFSEL_0                        ((uint32_t)0x00000008)
#define  RCC_USBSSPLLREFSEL_1                        ((uint32_t)0x00000010)
#define  RCC_USBSSPLLREFSEL_2                        ((uint32_t)0x00000020)

#define  RCC_USBSSPLLREFSEL_20MHz                    ((uint32_t)0x00000000)
#define  RCC_USBSSPLLREFSEL_24MHz                    ((uint32_t)0x00000008)
#define  RCC_USBSSPLLREFSEL_25MHz                    ((uint32_t)0x00000010)
#define  RCC_USBSSPLLREFSEL_30MHz                    ((uint32_t)0x00000018)
#define  RCC_USBSSPLLREFSEL_32MHz                    ((uint32_t)0x00000020)
#define  RCC_USBSSPLLREFSEL_40MHz                    ((uint32_t)0x00000028)
#define  RCC_USBSSPLLREFSEL_60MHz                    ((uint32_t)0x00000030)
#define  RCC_USBSSPLLREFSEL_80MHz                    ((uint32_t)0x00000038)

#define  RCC_USBPLLSRCCFG                            ((uint32_t)0x000000C0)
#define  RCC_USBPLLSRCCFG_0                          ((uint32_t)0x00000040)
#define  RCC_USBPLLSRCCFG_1                          ((uint32_t)0x00000080)

#define  RCC_USBHSPLLSRC_USBHSPLLSRC                 ((uint32_t)0x00000000)
#define  RCC_USBHSPLLSRC_USBSSPLL_625M_DIV25         ((uint32_t)0x00000040)
#define  RCC_USBSSPLLSRC_USBHSPLL_480M_DIV24         ((uint32_t)0x00000080)
#define  RCC_USBSSPLLSRC_USBSSPLLSRC                 ((uint32_t)0x000000C0)

#define  RCC_USBHSPLLSRCC                            ((uint32_t)0x00000100)
#define  RCC_USBHSPLLSRCC_HSE                        ((uint32_t)0x00000000)
#define  RCC_USBHSPLLSRCC_HSI                        ((uint32_t)0x00000100)

#define  RCC_USBSSPLLSRCC                            ((uint32_t)0x00000200)
#define  RCC_USBSSPLLSRCC_HSE                        ((uint32_t)0x00000000)
#define  RCC_USBSSPLLSRCC_HSI                        ((uint32_t)0x00000200)

#define  RCC_HSERDYCFG                               ((uint32_t)0x00030000)
#define  RCC_HSERDYCFG_0                             ((uint32_t)0x00010000)
#define  RCC_HSERDYCFG_1                             ((uint32_t)0x00020000)

#define  RCC_HSERDYCFG0                              ((uint32_t)0x00000000)
#define  RCC_HSERDYCFG1                              ((uint32_t)0x00010000)
#define  RCC_HSERDYCFG2                              ((uint32_t)0x00020000)
#define  RCC_HSERDYCFG3                              ((uint32_t)0x00030000)

#define  RCC_USBHSPLLRDYCFG                          ((uint32_t)0x000C0000)
#define  RCC_USBHSPLLRDYCFG_0                        ((uint32_t)0x00040000)
#define  RCC_USBHSPLLRDYCFG_1                        ((uint32_t)0x00080000)

#define  RCC_USBHSPLLRDYCFG0                         ((uint32_t)0x00000000)
#define  RCC_USBHSPLLRDYCFG1                         ((uint32_t)0x00040000)
#define  RCC_USBHSPLLRDYCFG2                         ((uint32_t)0x00080000)
#define  RCC_USBHSPLLRDYCFG3                         ((uint32_t)0x000C0000)

#define  RCC_USBSSPLLRDYCFG                          ((uint32_t)0x00300000)
#define  RCC_USBSSPLLRDYCFG_0                        ((uint32_t)0x00100000)
#define  RCC_USBSSPLLRDYCFG_1                        ((uint32_t)0x00200000)

#define  RCC_USBSSPLLRDYCFG0                         ((uint32_t)0x00000000)
#define  RCC_USBSSPLLRDYCFG1                         ((uint32_t)0x00100000)
#define  RCC_USBSSPLLRDYCFG2                         ((uint32_t)0x00200000)
#define  RCC_USBSSPLLRDYCFG3                         ((uint32_t)0x00300000)

#define  RCC_ADCSRC                                  ((uint32_t)0x01000000)

#define  RCC_ADCSRC_HCLK                             ((uint32_t)0x00000000)
#define  RCC_ADCSRC_USBPLL                           ((uint32_t)0x01000000)

#define  RCC_ADCDUTYMD                               ((uint32_t)0x06000000)
#define  RCC_ADCDUTYMD_0                             ((uint32_t)0x02000000)
#define  RCC_ADCDUTYMD_1                             ((uint32_t)0x04000000)

#define  RCC_ADCDUTYMD0                              ((uint32_t)0x00000000)
#define  RCC_ADCDUTYMD1                              ((uint32_t)0x02000000)
#define  RCC_ADCDUTYMD2                              ((uint32_t)0x04000000)
#define  RCC_ADCDUTYMD3                              ((uint32_t)0x06000000)

#define  RCC_ADCPLLSRC                               ((uint32_t)0x08000000)

#define  RCC_ADCPLLSRC_USBHSPLL_480MHz               ((uint32_t)0x00000000)
#define  RCC_ADCPLLSRC_USBSSPLL_625MHz               ((uint32_t)0x08000000)

#define  RCC_ADCCLKDIV                               ((uint32_t)0xF0000000)
#define  RCC_ADCCLKDIV_0                             ((uint32_t)0x10000000)
#define  RCC_ADCCLKDIV_1                             ((uint32_t)0x20000000)
#define  RCC_ADCCLKDIV_2                             ((uint32_t)0x40000000)
#define  RCC_ADCCLKDIV_3                             ((uint32_t)0x80000000)

#define  RCC_ADCCLKDIV1                              ((uint32_t)0x00000000)
#define  RCC_ADCCLKDIV2                              ((uint32_t)0x10000000)
#define  RCC_ADCCLKDIV3                              ((uint32_t)0x20000000)
#define  RCC_ADCCLKDIV4                              ((uint32_t)0x30000000)
#define  RCC_ADCCLKDIV5                              ((uint32_t)0x40000000)
#define  RCC_ADCCLKDIV6                              ((uint32_t)0x50000000)
#define  RCC_ADCCLKDIV7                              ((uint32_t)0x60000000)
#define  RCC_ADCCLKDIV8                              ((uint32_t)0x70000000)
#define  RCC_ADCCLKDIV9                              ((uint32_t)0x80000000)
#define  RCC_ADCCLKDIV10                             ((uint32_t)0x90000000)
#define  RCC_ADCCLKDIV11                             ((uint32_t)0xA0000000)
#define  RCC_ADCCLKDIV12                             ((uint32_t)0xB0000000)
#define  RCC_ADCCLKDIV13                             ((uint32_t)0xC0000000)
#define  RCC_ADCCLKDIV14                             ((uint32_t)0xD0000000)
#define  RCC_ADCCLKDIV15                             ((uint32_t)0xE0000000)
#define  RCC_ADCCLKDIV16                             ((uint32_t)0xF0000000)

/******************************************************************************/
/*                             Real-Time Clock                                */
/******************************************************************************/

/*******************  Bit definition for RTC_CTLRH register  ********************/
#define  RTC_CTLRH_SECIE                             ((uint8_t)0x01)               /* Second Interrupt Enable */
#define  RTC_CTLRH_ALRIE                             ((uint8_t)0x02)               /* Alarm Interrupt Enable */
#define  RTC_CTLRH_OWIE                              ((uint8_t)0x04)               /* OverfloW Interrupt Enable */

/*******************  Bit definition for RTC_CTLRL register  ********************/
#define  RTC_CTLRL_SECF                              ((uint8_t)0x01)               /* Second Flag */
#define  RTC_CTLRL_ALRF                              ((uint8_t)0x02)               /* Alarm Flag */
#define  RTC_CTLRL_OWF                               ((uint8_t)0x04)               /* OverfloW Flag */
#define  RTC_CTLRL_RSF                               ((uint8_t)0x08)               /* Registers Synchronized Flag */
#define  RTC_CTLRL_CNF                               ((uint8_t)0x10)               /* Configuration Flag */
#define  RTC_CTLRL_RTOFF                             ((uint8_t)0x20)               /* RTC operation OFF */
#define  RTC_CTLRL_RTCEN                             ((uint8_t)0x40) 
#define  RTC_CTLRL_RTCSEL                            ((uint8_t)0x80) 
#define  RTC_CTLRL_CCO                               ((uint16_t)0x100) 
#define  RTC_CTLRL_ASOE                              ((uint16_t)0x200) 
#define  RTC_CTLRL_ASOS                              ((uint16_t)0x400) 

#define  RTC_CTLRL_RTCCAL                            ((uint32_t)0x0003F800) 

/*******************  Bit definition for RTC_PSCH register  *******************/
#define  RTC_PSCH_PRL                                ((uint16_t)0x000F)            /* RTC Prescaler Reload Value High */

/*******************  Bit definition for RTC_PRLL register  *******************/
#define  RTC_PSCL_PRL                                ((uint16_t)0xFFFF)            /* RTC Prescaler Reload Value Low */

/*******************  Bit definition for RTC_DIVH register  *******************/
#define  RTC_DIVH_RTC_DIV                            ((uint16_t)0x000F)            /* RTC Clock Divider High */

/*******************  Bit definition for RTC_DIVL register  *******************/
#define  RTC_DIVL_RTC_DIV                            ((uint16_t)0xFFFF)            /* RTC Clock Divider Low */

/*******************  Bit definition for RTC_CNTH register  *******************/
#define  RTC_CNTH_RTC_CNT                            ((uint16_t)0xFFFF)            /* RTC Counter High */

/*******************  Bit definition for RTC_CNTL register  *******************/
#define  RTC_CNTL_RTC_CNT                            ((uint16_t)0xFFFF)            /* RTC Counter Low */

/*******************  Bit definition for RTC_ALRMH register  *******************/
#define  RTC_ALRMH_RTC_ALRM                          ((uint16_t)0xFFFF)            /* RTC Alarm High */

/*******************  Bit definition for RTC_ALRML register  *******************/
#define  RTC_ALRML_RTC_ALRM                          ((uint16_t)0xFFFF)            /* RTC Alarm Low */

/******************************************************************************/
/*                        Serial Peripheral Interface                         */
/******************************************************************************/

/*******************  Bit definition for SPI_CTLR1 register  ********************/
#define  SPI_CTLR1_CPHA                              ((uint16_t)0x0001)            /* Clock Phase */
#define  SPI_CTLR1_CPOL                              ((uint16_t)0x0002)            /* Clock Polarity */
#define  SPI_CTLR1_MSTR                              ((uint16_t)0x0004)            /* Master Selection */

#define  SPI_CTLR1_BR                                ((uint16_t)0x0038)            /* BR[2:0] bits (Baud Rate Control) */
#define  SPI_CTLR1_BR_0                              ((uint16_t)0x0008)            /* Bit 0 */
#define  SPI_CTLR1_BR_1                              ((uint16_t)0x0010)            /* Bit 1 */
#define  SPI_CTLR1_BR_2                              ((uint16_t)0x0020)            /* Bit 2 */

#define  SPI_CTLR1_SPE                               ((uint16_t)0x0040)            /* SPI Enable */
#define  SPI_CTLR1_LSBFIRST                          ((uint16_t)0x0080)            /* Frame Format */
#define  SPI_CTLR1_SSI                               ((uint16_t)0x0100)            /* Internal slave select */
#define  SPI_CTLR1_SSM                               ((uint16_t)0x0200)            /* Software slave management */
#define  SPI_CTLR1_RXONLY                            ((uint16_t)0x0400)            /* Receive only */
#define  SPI_CTLR1_DFF                               ((uint16_t)0x0800)            /* Data Frame Format */
#define  SPI_CTLR1_CRCNEXT                           ((uint16_t)0x1000)            /* Transmit CRC next */
#define  SPI_CTLR1_CRCEN                             ((uint16_t)0x2000)            /* Hardware CRC calculation enable */
#define  SPI_CTLR1_BIDIOE                            ((uint16_t)0x4000)            /* Output enable in bidirectional mode */
#define  SPI_CTLR1_BIDIMODE                          ((uint16_t)0x8000)            /* Bidirectional data mode enable */

/*******************  Bit definition for SPI_CTLR2 register  ********************/
#define  SPI_CTLR2_RXDMAEN                           ((uint8_t)0x01)               /* Rx Buffer DMA Enable */
#define  SPI_CTLR2_TXDMAEN                           ((uint8_t)0x02)               /* Tx Buffer DMA Enable */
#define  SPI_CTLR2_SSOE                              ((uint8_t)0x04)               /* SS Output Enable */
#define  SPI_CTLR2_ERRIE                             ((uint8_t)0x20)               /* Error Interrupt Enable */
#define  SPI_CTLR2_RXNEIE                            ((uint8_t)0x40)               /* RX buffer Not Empty Interrupt Enable */
#define  SPI_CTLR2_TXEIE                             ((uint8_t)0x80)               /* Tx buffer Empty Interrupt Enable */

/********************  Bit definition for SPI_STATR register  ********************/
#define  SPI_STATR_RXNE                              ((uint8_t)0x01)               /* Receive buffer Not Empty */
#define  SPI_STATR_TXE                               ((uint8_t)0x02)               /* Transmit buffer Empty */
#define  SPI_STATR_CHSIDE                            ((uint8_t)0x04)               /* Channel side */
#define  SPI_STATR_UDR                               ((uint8_t)0x08)               /* Underrun flag */
#define  SPI_STATR_CRCERR                            ((uint8_t)0x10)               /* CRC Error flag */
#define  SPI_STATR_MODF                              ((uint8_t)0x20)               /* Mode fault */
#define  SPI_STATR_OVR                               ((uint8_t)0x40)               /* Overrun flag */
#define  SPI_STATR_BSY                               ((uint8_t)0x80)               /* Busy flag */

/********************  Bit definition for SPI_DATAR register  ********************/
#define  SPI_DATAR_DR                                ((uint16_t)0xFFFF)            /* Data Register */

/*******************  Bit definition for SPI_CRCR register  ******************/
#define  SPI_CRCR_CRCPOLY                            ((uint16_t)0xFFFF)            /* CRC polynomial register */

/******************  Bit definition for SPI_RCRCR register  ******************/
#define  SPI_RCRCR_RXCRC                             ((uint16_t)0xFFFF)            /* Rx CRC Register */

/******************  Bit definition for SPI_TCRCR register  ******************/
#define  SPI_TCRCR_TXCRC                             ((uint16_t)0xFFFF)            /* Tx CRC Register */

/******************  Bit definition for SPI_HSCR register  *******************/
#define  SPI_HSCR_HSRXEN                             ((uint16_t)0x0001)            
#define  SPI_HSCR_HSRXEN2                            ((uint16_t)0x0004)            

/******************************************************************************/
/*                                    TIM                                     */
/******************************************************************************/

/*******************  Bit definition for TIM_CTLR1 register  ********************/
#define  TIM_CEN                                     ((uint16_t)0x0001)            /* Counter enable */
#define  TIM_UDIS                                    ((uint16_t)0x0002)            /* Update disable */
#define  TIM_URS                                     ((uint16_t)0x0004)            /* Update request source */
#define  TIM_OPM                                     ((uint16_t)0x0008)            /* One pulse mode */
#define  TIM_DIR                                     ((uint16_t)0x0010)            /* Direction */

#define  TIM_CMS                                     ((uint16_t)0x0060)            /* CMS[1:0] bits (Center-aligned mode selection) */
#define  TIM_CMS_0                                   ((uint16_t)0x0020)            /* Bit 0 */
#define  TIM_CMS_1                                   ((uint16_t)0x0040)            /* Bit 1 */

#define  TIM_ARPE                                    ((uint16_t)0x0080)            /* Auto-reload preload enable */

#define  TIM_CTLR1_CKD                               ((uint16_t)0x0300)            /* CKD[1:0] bits (clock division) */
#define  TIM_CKD_0                                   ((uint16_t)0x0100)            /* Bit 0 */
#define  TIM_CKD_1                                   ((uint16_t)0x0200)            /* Bit 1 */

#define  TIM_CAPOV                                   ((uint16_t)0x4000) 
#define  TIM_CAPLVL                                  ((uint16_t)0x8000) 

/*******************  Bit definition for TIM_CTLR2 register  ********************/
#define  TIM_CCPC                                    ((uint16_t)0x0001)            /* Capture/Compare Preloaded Control */
#define  TIM_CCUS                                    ((uint16_t)0x0004)            /* Capture/Compare Control Update Selection */
#define  TIM_CCDS                                    ((uint16_t)0x0008)            /* Capture/Compare DMA Selection */

#define  TIM_MMS                                     ((uint16_t)0x0070)            /* MMS[2:0] bits (Master Mode Selection) */
#define  TIM_MMS_0                                   ((uint16_t)0x0010)            /* Bit 0 */
#define  TIM_MMS_1                                   ((uint16_t)0x0020)            /* Bit 1 */
#define  TIM_MMS_2                                   ((uint16_t)0x0040)            /* Bit 2 */

#define  TIM_TI1S                                    ((uint16_t)0x0080)            /* TI1 Selection */
#define  TIM_OIS1                                    ((uint16_t)0x0100)            /* Output Idle state 1 (OC1 output) */
#define  TIM_OIS1N                                   ((uint16_t)0x0200)            /* Output Idle state 1 (OC1N output) */
#define  TIM_OIS2                                    ((uint16_t)0x0400)            /* Output Idle state 2 (OC2 output) */
#define  TIM_OIS2N                                   ((uint16_t)0x0800)            /* Output Idle state 2 (OC2N output) */
#define  TIM_OIS3                                    ((uint16_t)0x1000)            /* Output Idle state 3 (OC3 output) */
#define  TIM_OIS3N                                   ((uint16_t)0x2000)            /* Output Idle state 3 (OC3N output) */
#define  TIM_OIS4                                    ((uint16_t)0x4000)            /* Output Idle state 4 (OC4 output) */

/*******************  Bit definition for TIM_SMCFGR register  *******************/
#define  TIM_SMS                                     ((uint16_t)0x0007)            /* SMS[2:0] bits (Slave mode selection) */
#define  TIM_SMS_0                                   ((uint16_t)0x0001)            /* Bit 0 */
#define  TIM_SMS_1                                   ((uint16_t)0x0002)            /* Bit 1 */
#define  TIM_SMS_2                                   ((uint16_t)0x0004)            /* Bit 2 */

#define  TIM_TS                                      ((uint16_t)0x0070)            /* TS[2:0] bits (Trigger selection) */
#define  TIM_TS_0                                    ((uint16_t)0x0010)            /* Bit 0 */
#define  TIM_TS_1                                    ((uint16_t)0x0020)            /* Bit 1 */
#define  TIM_TS_2                                    ((uint16_t)0x0040)            /* Bit 2 */

#define  TIM_MSM                                     ((uint16_t)0x0080)            /* Master/slave mode */
 
#define  TIM_ETF                                     ((uint16_t)0x0F00)            /* ETF[3:0] bits (External trigger filter) */
#define  TIM_ETF_0                                   ((uint16_t)0x0100)            /* Bit 0 */
#define  TIM_ETF_1                                   ((uint16_t)0x0200)            /* Bit 1 */
#define  TIM_ETF_2                                   ((uint16_t)0x0400)            /* Bit 2 */
#define  TIM_ETF_3                                   ((uint16_t)0x0800)            /* Bit 3 */

#define  TIM_ETPS                                    ((uint16_t)0x3000)            /* ETPS[1:0] bits (External trigger prescaler) */
#define  TIM_ETPS_0                                  ((uint16_t)0x1000)            /* Bit 0 */
#define  TIM_ETPS_1                                  ((uint16_t)0x2000)            /* Bit 1 */
 
#define  TIM_ECE                                     ((uint16_t)0x4000)            /* External clock enable */
#define  TIM_ETP                                     ((uint16_t)0x8000)            /* External trigger polarity */

/*******************  Bit definition for TIM_DMAINTENR register  *******************/
#define  TIM_UIE                                     ((uint16_t)0x0001)            /* Update interrupt enable */
#define  TIM_CC1IE                                   ((uint16_t)0x0002)            /* Capture/Compare 1 interrupt enable */
#define  TIM_CC2IE                                   ((uint16_t)0x0004)            /* Capture/Compare 2 interrupt enable */
#define  TIM_CC3IE                                   ((uint16_t)0x0008)            /* Capture/Compare 3 interrupt enable */
#define  TIM_CC4IE                                   ((uint16_t)0x0010)            /* Capture/Compare 4 interrupt enable */
#define  TIM_COMIE                                   ((uint16_t)0x0020)            /* COM interrupt enable */
#define  TIM_TIE                                     ((uint16_t)0x0040)            /* Trigger interrupt enable */
#define  TIM_BIE                                     ((uint16_t)0x0080)            /* Break interrupt enable */
#define  TIM_UDE                                     ((uint16_t)0x0100)            /* Update DMA request enable */
#define  TIM_CC1DE                                   ((uint16_t)0x0200)            /* Capture/Compare 1 DMA request enable */
#define  TIM_CC2DE                                   ((uint16_t)0x0400)            /* Capture/Compare 2 DMA request enable */
#define  TIM_CC3DE                                   ((uint16_t)0x0800)            /* Capture/Compare 3 DMA request enable */
#define  TIM_CC4DE                                   ((uint16_t)0x1000)            /* Capture/Compare 4 DMA request enable */
#define  TIM_COMDE                                   ((uint16_t)0x2000)            /* COM DMA request enable */
#define  TIM_TDE                                     ((uint16_t)0x4000)            /* Trigger DMA request enable */

/********************  Bit definition for TIM_INTFR register  ********************/
#define  TIM_UIF                                     ((uint16_t)0x0001)            /* Update interrupt Flag */
#define  TIM_CC1IF                                   ((uint16_t)0x0002)            /* Capture/Compare 1 interrupt Flag */
#define  TIM_CC2IF                                   ((uint16_t)0x0004)            /* Capture/Compare 2 interrupt Flag */
#define  TIM_CC3IF                                   ((uint16_t)0x0008)            /* Capture/Compare 3 interrupt Flag */
#define  TIM_CC4IF                                   ((uint16_t)0x0010)            /* Capture/Compare 4 interrupt Flag */
#define  TIM_COMIF                                   ((uint16_t)0x0020)            /* COM interrupt Flag */
#define  TIM_TIF                                     ((uint16_t)0x0040)            /* Trigger interrupt Flag */
#define  TIM_BIF                                     ((uint16_t)0x0080)            /* Break interrupt Flag */
#define  TIM_CC1OF                                   ((uint16_t)0x0200)            /* Capture/Compare 1 Overcapture Flag */
#define  TIM_CC2OF                                   ((uint16_t)0x0400)            /* Capture/Compare 2 Overcapture Flag */
#define  TIM_CC3OF                                   ((uint16_t)0x0800)            /* Capture/Compare 3 Overcapture Flag */
#define  TIM_CC4OF                                   ((uint16_t)0x1000)            /* Capture/Compare 4 Overcapture Flag */

/*******************  Bit definition for TIM_SWEVGR register  ********************/
#define  TIM_UG                                      ((uint8_t)0x01)               /* Update Generation */
#define  TIM_CC1G                                    ((uint8_t)0x02)               /* Capture/Compare 1 Generation */
#define  TIM_CC2G                                    ((uint8_t)0x04)               /* Capture/Compare 2 Generation */
#define  TIM_CC3G                                    ((uint8_t)0x08)               /* Capture/Compare 3 Generation */
#define  TIM_CC4G                                    ((uint8_t)0x10)               /* Capture/Compare 4 Generation */
#define  TIM_COMG                                    ((uint8_t)0x20)               /* Capture/Compare Control Update Generation */
#define  TIM_TG                                      ((uint8_t)0x40)               /* Trigger Generation */
#define  TIM_BG                                      ((uint8_t)0x80)               /* Break Generation */

/******************  Bit definition for TIM_CHCTLR1 register  *******************/
#define  TIM_CC1S                                    ((uint16_t)0x0003)            /* CC1S[1:0] bits (Capture/Compare 1 Selection) */
#define  TIM_CC1S_0                                  ((uint16_t)0x0001)            /* Bit 0 */
#define  TIM_CC1S_1                                  ((uint16_t)0x0002)            /* Bit 1 */

#define  TIM_OC1FE                                   ((uint16_t)0x0004)            /* Output Compare 1 Fast enable */
#define  TIM_OC1PE                                   ((uint16_t)0x0008)            /* Output Compare 1 Preload enable */

#define  TIM_OC1M                                    ((uint16_t)0x0070)            /* OC1M[2:0] bits (Output Compare 1 Mode) */
#define  TIM_OC1M_0                                  ((uint16_t)0x0010)            /* Bit 0 */
#define  TIM_OC1M_1                                  ((uint16_t)0x0020)            /* Bit 1 */
#define  TIM_OC1M_2                                  ((uint16_t)0x0040)            /* Bit 2 */

#define  TIM_OC1CE                                   ((uint16_t)0x0080)            /* Output Compare 1Clear Enable */

#define  TIM_CC2S                                    ((uint16_t)0x0300)            /* CC2S[1:0] bits (Capture/Compare 2 Selection) */
#define  TIM_CC2S_0                                  ((uint16_t)0x0100)            /* Bit 0 */
#define  TIM_CC2S_1                                  ((uint16_t)0x0200)            /* Bit 1 */

#define  TIM_OC2FE                                   ((uint16_t)0x0400)            /* Output Compare 2 Fast enable */
#define  TIM_OC2PE                                   ((uint16_t)0x0800)            /* Output Compare 2 Preload enable */

#define  TIM_OC2M                                    ((uint16_t)0x7000)            /* OC2M[2:0] bits (Output Compare 2 Mode) */
#define  TIM_OC2M_0                                  ((uint16_t)0x1000)            /* Bit 0 */
#define  TIM_OC2M_1                                  ((uint16_t)0x2000)            /* Bit 1 */
#define  TIM_OC2M_2                                  ((uint16_t)0x4000)            /* Bit 2 */

#define  TIM_OC2CE                                   ((uint16_t)0x8000)            /* Output Compare 2 Clear Enable */


#define  TIM_IC1PSC                                  ((uint16_t)0x000C)            /* IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define  TIM_IC1PSC_0                                ((uint16_t)0x0004)            /* Bit 0 */
#define  TIM_IC1PSC_1                                ((uint16_t)0x0008)            /* Bit 1 */

#define  TIM_IC1F                                    ((uint16_t)0x00F0)            /* IC1F[3:0] bits (Input Capture 1 Filter) */
#define  TIM_IC1F_0                                  ((uint16_t)0x0010)            /* Bit 0 */
#define  TIM_IC1F_1                                  ((uint16_t)0x0020)            /* Bit 1 */
#define  TIM_IC1F_2                                  ((uint16_t)0x0040)            /* Bit 2 */
#define  TIM_IC1F_3                                  ((uint16_t)0x0080)            /* Bit 3 */

#define  TIM_IC2PSC                                  ((uint16_t)0x0C00)            /* IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
#define  TIM_IC2PSC_0                                ((uint16_t)0x0400)            /* Bit 0 */
#define  TIM_IC2PSC_1                                ((uint16_t)0x0800)            /* Bit 1 */

#define  TIM_IC2F                                    ((uint16_t)0xF000)            /* IC2F[3:0] bits (Input Capture 2 Filter) */
#define  TIM_IC2F_0                                  ((uint16_t)0x1000)            /* Bit 0 */
#define  TIM_IC2F_1                                  ((uint16_t)0x2000)            /* Bit 1 */
#define  TIM_IC2F_2                                  ((uint16_t)0x4000)            /* Bit 2 */
#define  TIM_IC2F_3                                  ((uint16_t)0x8000)            /* Bit 3 */

/******************  Bit definition for TIM_CHCTLR2 register  *******************/
#define  TIM_CC3S                                    ((uint16_t)0x0003)            /* CC3S[1:0] bits (Capture/Compare 3 Selection) */
#define  TIM_CC3S_0                                  ((uint16_t)0x0001)            /* Bit 0 */
#define  TIM_CC3S_1                                  ((uint16_t)0x0002)            /* Bit 1 */

#define  TIM_OC3FE                                   ((uint16_t)0x0004)            /* Output Compare 3 Fast enable */
#define  TIM_OC3PE                                   ((uint16_t)0x0008)            /* Output Compare 3 Preload enable */

#define  TIM_OC3M                                    ((uint16_t)0x0070)            /* OC3M[2:0] bits (Output Compare 3 Mode) */
#define  TIM_OC3M_0                                  ((uint16_t)0x0010)            /* Bit 0 */
#define  TIM_OC3M_1                                  ((uint16_t)0x0020)            /* Bit 1 */
#define  TIM_OC3M_2                                  ((uint16_t)0x0040)            /* Bit 2 */

#define  TIM_OC3CE                                   ((uint16_t)0x0080)            /* Output Compare 3 Clear Enable */

#define  TIM_CC4S                                    ((uint16_t)0x0300)            /* CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define  TIM_CC4S_0                                  ((uint16_t)0x0100)            /* Bit 0 */
#define  TIM_CC4S_1                                  ((uint16_t)0x0200)            /* Bit 1 */

#define  TIM_OC4FE                                   ((uint16_t)0x0400)            /* Output Compare 4 Fast enable */
#define  TIM_OC4PE                                   ((uint16_t)0x0800)            /* Output Compare 4 Preload enable */

#define  TIM_OC4M                                    ((uint16_t)0x7000)            /* OC4M[2:0] bits (Output Compare 4 Mode) */
#define  TIM_OC4M_0                                  ((uint16_t)0x1000)            /* Bit 0 */
#define  TIM_OC4M_1                                  ((uint16_t)0x2000)            /* Bit 1 */
#define  TIM_OC4M_2                                  ((uint16_t)0x4000)            /* Bit 2 */

#define  TIM_OC4CE                                   ((uint16_t)0x8000)            /* Output Compare 4 Clear Enable */


#define  TIM_IC3PSC                                  ((uint16_t)0x000C)            /* IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define  TIM_IC3PSC_0                                ((uint16_t)0x0004)            /* Bit 0 */
#define  TIM_IC3PSC_1                                ((uint16_t)0x0008)            /* Bit 1 */

#define  TIM_IC3F                                    ((uint16_t)0x00F0)            /* IC3F[3:0] bits (Input Capture 3 Filter) */
#define  TIM_IC3F_0                                  ((uint16_t)0x0010)            /* Bit 0 */
#define  TIM_IC3F_1                                  ((uint16_t)0x0020)            /* Bit 1 */
#define  TIM_IC3F_2                                  ((uint16_t)0x0040)            /* Bit 2 */
#define  TIM_IC3F_3                                  ((uint16_t)0x0080)            /* Bit 3 */

#define  TIM_IC4PSC                                  ((uint16_t)0x0C00)            /* IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define  TIM_IC4PSC_0                                ((uint16_t)0x0400)            /* Bit 0 */
#define  TIM_IC4PSC_1                                ((uint16_t)0x0800)            /* Bit 1 */

#define  TIM_IC4F                                    ((uint16_t)0xF000)            /* IC4F[3:0] bits (Input Capture 4 Filter) */
#define  TIM_IC4F_0                                  ((uint16_t)0x1000)            /* Bit 0 */
#define  TIM_IC4F_1                                  ((uint16_t)0x2000)            /* Bit 1 */
#define  TIM_IC4F_2                                  ((uint16_t)0x4000)            /* Bit 2 */
#define  TIM_IC4F_3                                  ((uint16_t)0x8000)            /* Bit 3 */

/*******************  Bit definition for TIM_CCER register  *******************/
#define  TIM_CC1E                                    ((uint16_t)0x0001)            /* Capture/Compare 1 output enable */
#define  TIM_CC1P                                    ((uint16_t)0x0002)            /* Capture/Compare 1 output Polarity */
#define  TIM_CC1NE                                   ((uint16_t)0x0004)            /* Capture/Compare 1 Complementary output enable */
#define  TIM_CC1NP                                   ((uint16_t)0x0008)            /* Capture/Compare 1 Complementary output Polarity */
#define  TIM_CC2E                                    ((uint16_t)0x0010)            /* Capture/Compare 2 output enable */
#define  TIM_CC2P                                    ((uint16_t)0x0020)            /* Capture/Compare 2 output Polarity */
#define  TIM_CC2NE                                   ((uint16_t)0x0040)            /* Capture/Compare 2 Complementary output enable */
#define  TIM_CC2NP                                   ((uint16_t)0x0080)            /* Capture/Compare 2 Complementary output Polarity */
#define  TIM_CC3E                                    ((uint16_t)0x0100)            /* Capture/Compare 3 output enable */
#define  TIM_CC3P                                    ((uint16_t)0x0200)            /* Capture/Compare 3 output Polarity */
#define  TIM_CC3NE                                   ((uint16_t)0x0400)            /* Capture/Compare 3 Complementary output enable */
#define  TIM_CC3NP                                   ((uint16_t)0x0800)            /* Capture/Compare 3 Complementary output Polarity */
#define  TIM_CC4E                                    ((uint16_t)0x1000)            /* Capture/Compare 4 output enable */
#define  TIM_CC4P                                    ((uint16_t)0x2000)            /* Capture/Compare 4 output Polarity */

/*******************  Bit definition for TIM_CNT register  ********************/
#define  TIM_CNT                                     ((uint16_t)0xFFFF)            /* Counter Value */

/*******************  Bit definition for TIM_PSC register  ********************/
#define  TIM_PSC                                     ((uint16_t)0xFFFF)            /* Prescaler Value */

/*******************  Bit definition for TIM_ATRLR register  ********************/
#define  TIM_ARR                                     ((uint16_t)0xFFFF)            /* actual auto-reload Value */

/*******************  Bit definition for TIM_RPTCR register  ********************/
#define  TIM_REP                                     ((uint8_t)0xFF)               /* Repetition Counter Value */

/*******************  Bit definition for TIM_CH1CVR register  *******************/
#define  TIM_CCR1                                    ((uint16_t)0xFFFF)            /* Capture/Compare 1 Value */

/*******************  Bit definition for TIM_CH2CVR register  *******************/
#define  TIM_CCR2                                    ((uint16_t)0xFFFF)            /* Capture/Compare 2 Value */

/*******************  Bit definition for TIM_CH3CVR register  *******************/
#define  TIM_CCR3                                    ((uint16_t)0xFFFF)            /* Capture/Compare 3 Value */

/*******************  Bit definition for TIM_CH4CVR register  *******************/
#define  TIM_CCR4                                    ((uint16_t)0xFFFF)            /* Capture/Compare 4 Value */

/*******************  Bit definition for TIM_BDTR register  *******************/
#define  TIM_DTG                                     ((uint16_t)0x00FF)            /* DTG[0:7] bits (Dead-Time Generator set-up) */
#define  TIM_DTG_0                                   ((uint16_t)0x0001)            /* Bit 0 */
#define  TIM_DTG_1                                   ((uint16_t)0x0002)            /* Bit 1 */
#define  TIM_DTG_2                                   ((uint16_t)0x0004)            /* Bit 2 */
#define  TIM_DTG_3                                   ((uint16_t)0x0008)            /* Bit 3 */
#define  TIM_DTG_4                                   ((uint16_t)0x0010)            /* Bit 4 */
#define  TIM_DTG_5                                   ((uint16_t)0x0020)            /* Bit 5 */
#define  TIM_DTG_6                                   ((uint16_t)0x0040)            /* Bit 6 */
#define  TIM_DTG_7                                   ((uint16_t)0x0080)            /* Bit 7 */

#define  TIM_LOCK                                    ((uint16_t)0x0300)            /* LOCK[1:0] bits (Lock Configuration) */
#define  TIM_LOCK_0                                  ((uint16_t)0x0100)            /* Bit 0 */
#define  TIM_LOCK_1                                  ((uint16_t)0x0200)            /* Bit 1 */
 
#define  TIM_OSSI                                    ((uint16_t)0x0400)            /* Off-State Selection for Idle mode */
#define  TIM_OSSR                                    ((uint16_t)0x0800)            /* Off-State Selection for Run mode */
#define  TIM_BKE                                     ((uint16_t)0x1000)            /* Break enable */
#define  TIM_BKP                                     ((uint16_t)0x2000)            /* Break Polarity */
#define  TIM_AOE                                     ((uint16_t)0x4000)            /* Automatic Output enable */
#define  TIM_MOE                                     ((uint16_t)0x8000)            /* Main Output enable */

/*******************  Bit definition for TIM_DMACFGR register  ********************/
#define  TIM_DBA                                     ((uint16_t)0x001F)            /* DBA[4:0] bits (DMA Base Address) */
#define  TIM_DBA_0                                   ((uint16_t)0x0001)            /* Bit 0 */
#define  TIM_DBA_1                                   ((uint16_t)0x0002)            /* Bit 1 */
#define  TIM_DBA_2                                   ((uint16_t)0x0004)            /* Bit 2 */
#define  TIM_DBA_3                                   ((uint16_t)0x0008)            /* Bit 3 */
#define  TIM_DBA_4                                   ((uint16_t)0x0010)            /* Bit 4 */

#define  TIM_DBL                                     ((uint16_t)0x1F00)            /* DBL[4:0] bits (DMA Burst Length) */
#define  TIM_DBL_0                                   ((uint16_t)0x0100)            /* Bit 0 */
#define  TIM_DBL_1                                   ((uint16_t)0x0200)            /* Bit 1 */
#define  TIM_DBL_2                                   ((uint16_t)0x0400)            /* Bit 2 */
#define  TIM_DBL_3                                   ((uint16_t)0x0800)            /* Bit 3 */
#define  TIM_DBL_4                                   ((uint16_t)0x1000)            /* Bit 4 */

/*******************  Bit definition for TIM_DMAADR register  *******************/
#define  TIM_DMAR_DMAB                               ((uint16_t)0xFFFF)            /* DMA register for burst accesses */

/******************************************************************************/
/*         Universal Synchronous Asynchronous Receiver Transmitter            */
/******************************************************************************/

/*******************  Bit definition for USART_STATR register  *******************/
#define  USART_STATR_PE                              ((uint16_t)0x0001)            /* Parity Error */
#define  USART_STATR_FE                              ((uint16_t)0x0002)            /* Framing Error */
#define  USART_STATR_NE                              ((uint16_t)0x0004)            /* Noise Error Flag */
#define  USART_STATR_ORE                             ((uint16_t)0x0008)            /* OverRun Error */
#define  USART_STATR_IDLE                            ((uint16_t)0x0010)            /* IDLE line detected */
#define  USART_STATR_RXNE                            ((uint16_t)0x0020)            /* Read Data Register Not Empty */
#define  USART_STATR_TC                              ((uint16_t)0x0040)            /* Transmission Complete */
#define  USART_STATR_TXE                             ((uint16_t)0x0080)            /* Transmit Data Register Empty */
#define  USART_STATR_LBD                             ((uint16_t)0x0100)            /* LIN Break Detection Flag */
#define  USART_STATR_CTS                             ((uint16_t)0x0200)            /* CTS Flag */
#define  USART_STATR_RX_BUSY                         ((uint16_t)0x0400)
#define  USART_STATR_MS_ERR                          ((uint16_t)0x0800)
#define  USART_STATR_LPWKUP_ACT_FLAG                 ((uint16_t)0x8000)

/*******************  Bit definition for USART_DATAR register  *******************/
#define  USART_DATAR_DR                              ((uint16_t)0x01FF)            /* Data value */

/******************  Bit definition for USART_BRR register  *******************/
#define  USART_BRR_DIV_Fraction                      ((uint16_t)0x0000000F)            /* Fraction of USARTDIV */
#define  USART_BRR_DIV_Mantissa                      ((uint16_t)0x000FFFF0)            /* Mantissa of USARTDIV */

/******************  Bit definition for USART_CTLR1 register  *******************/
#define  USART_CTLR1_SBK                             ((uint16_t)0x0001)            /* Send Break */
#define  USART_CTLR1_RWU                             ((uint16_t)0x0002)            /* Receiver wakeup */
#define  USART_CTLR1_RE                              ((uint16_t)0x0004)            /* Receiver Enable */
#define  USART_CTLR1_TE                              ((uint16_t)0x0008)            /* Transmitter Enable */
#define  USART_CTLR1_IDLEIE                          ((uint16_t)0x0010)            /* IDLE Interrupt Enable */
#define  USART_CTLR1_RXNEIE                          ((uint16_t)0x0020)            /* RXNE Interrupt Enable */
#define  USART_CTLR1_TCIE                            ((uint16_t)0x0040)            /* Transmission Complete Interrupt Enable */
#define  USART_CTLR1_TXEIE                           ((uint16_t)0x0080)            /* PE Interrupt Enable */
#define  USART_CTLR1_PEIE                            ((uint16_t)0x0100)            /* PE Interrupt Enable */
#define  USART_CTLR1_PS                              ((uint16_t)0x0200)            /* Parity Selection */
#define  USART_CTLR1_PCE                             ((uint16_t)0x0400)            /* Parity Control Enable */
#define  USART_CTLR1_WAKE                            ((uint16_t)0x0800)            /* Wakeup method */
#define  USART_CTLR1_M                               ((uint16_t)0x1000)            /* Word length */
#define  USART_CTLR1_UE                              ((uint16_t)0x2000)            /* USART Enable */
#define  USART_CTLR1_M_EXT_0                         ((uint16_t)0x4000)            
#define  USART_CTLR1_M_EXT_1                         ((uint16_t)0x8000) 
#define  USART_CTLR1_M_EXT5                          ((uint16_t)0xC000) 
#define  USART_CTLR1_M_EXT6                          ((uint16_t)0x8000) 
#define  USART_CTLR1_M_EXT7                          ((uint16_t)0x4000) 

/******************  Bit definition for USART_CTLR2 register  *******************/
#define  USART_CTLR2_ADD                             ((uint16_t)0x000F)            /* Address of the USART node */
#define  USART_CTLR2_LBDL                            ((uint16_t)0x0020)            /* LIN Break Detection Length */
#define  USART_CTLR2_LBDIE                           ((uint16_t)0x0040)            /* LIN Break Detection Interrupt Enable */
#define  USART_CTLR2_LBCL                            ((uint16_t)0x0100)            /* Last Bit Clock pulse */
#define  USART_CTLR2_CPHA                            ((uint16_t)0x0200)            /* Clock Phase */
#define  USART_CTLR2_CPOL                            ((uint16_t)0x0400)            /* Clock Polarity */
#define  USART_CTLR2_CLKEN                           ((uint16_t)0x0800)            /* Clock Enable */

#define  USART_CTLR2_STOP                            ((uint16_t)0x3000)            /* STOP[1:0] bits (STOP bits) */
#define  USART_CTLR2_STOP_0                          ((uint16_t)0x1000)            /* Bit 0 */
#define  USART_CTLR2_STOP_1                          ((uint16_t)0x2000)            /* Bit 1 */

#define  USART_CTLR2_LINEN                           ((uint16_t)0x4000)            /* LIN mode enable */

/******************  Bit definition for USART_CTLR3 register  *******************/
#define  USART_CTLR3_EIE                             ((uint16_t)0x0001)            /* Error Interrupt Enable */
#define  USART_CTLR3_IREN                            ((uint16_t)0x0002)            /* IrDA mode Enable */
#define  USART_CTLR3_IRLP                            ((uint16_t)0x0004)            /* IrDA Low-Power */
#define  USART_CTLR3_HDSEL                           ((uint16_t)0x0008)            /* Half-Duplex Selection */
#define  USART_CTLR3_NACK                            ((uint16_t)0x0010)            /* Smartcard NACK enable */
#define  USART_CTLR3_SCEN                            ((uint16_t)0x0020)            /* Smartcard mode enable */
#define  USART_CTLR3_DMAR                            ((uint16_t)0x0040)            /* DMA Enable Receiver */
#define  USART_CTLR3_DMAT                            ((uint16_t)0x0080)            /* DMA Enable Transmitter */
#define  USART_CTLR3_RTSE                            ((uint16_t)0x0100)            /* RTS Enable */
#define  USART_CTLR3_CTSE                            ((uint16_t)0x0200)            /* CTS Enable */
#define  USART_CTLR3_CTSIE                           ((uint16_t)0x0400)            /* CTS Interrupt Enable */
#define  USART_CTLR3_LPWKUP_EN                       ((uint16_t)0x0800)            /* Low Power Wakeup Enable */
#define  USART_CTLR3_LPWKUP_CK_SRC                   ((uint16_t)0x1000)            /* Low Power Wakeup Clock Source */
#define  USART_CTLR3_LPWKUP_DLY_CFG                  ((uint16_t)0xE000)            /* Low Power Wakeup Delay Configuration */

/******************  Bit definition for USART_GPR register  ******************/
#define  USART_GPR_PSC                               ((uint16_t)0x00FF)            /* PSC[7:0] bits (Prescaler value) */
#define  USART_GPR_PSC_0                             ((uint16_t)0x0001)            /* Bit 0 */
#define  USART_GPR_PSC_1                             ((uint16_t)0x0002)            /* Bit 1 */
#define  USART_GPR_PSC_2                             ((uint16_t)0x0004)            /* Bit 2 */
#define  USART_GPR_PSC_3                             ((uint16_t)0x0008)            /* Bit 3 */
#define  USART_GPR_PSC_4                             ((uint16_t)0x0010)            /* Bit 4 */
#define  USART_GPR_PSC_5                             ((uint16_t)0x0020)            /* Bit 5 */
#define  USART_GPR_PSC_6                             ((uint16_t)0x0040)            /* Bit 6 */
#define  USART_GPR_PSC_7                             ((uint16_t)0x0080)            /* Bit 7 */

#define  USART_GPR_GT                                ((uint16_t)0xFF00)            /* Guard time value */

/******************  Bit definition for USART_CTLR4 register  ******************/
#define  USART_CTLR4_SELF_OD                         ((uint16_t)0x0001)            /* USART Open Drain Mode */
#define  USART_CTLR4_MS_ERRIE                        ((uint16_t)0x0002)            /* USART Space or Mark verification enable */
#define  USART_CTLR4_CHECK_SEL                       ((uint16_t)0x000C)            /* CHECK[1:0] bits (Check mode) */
#define  USART_CTLR4_CHECK_MARKENABLE                ((uint16_t)0x0008)            /* Bit 0 */
#define  USART_CTLR4_CHECK_APACEENABLE               ((uint16_t)0x000C)            /* Bit 1 */
      
/******************************************************************************/
/*                            Window WATCHDOG                                 */
/******************************************************************************/

/*******************  Bit definition for WWDG_CTLR register  ********************/
#define  WWDG_CTLR_T                                 ((uint8_t)0x7F)               /* T[6:0] bits (7-Bit counter (MSB to LSB)) */
#define  WWDG_CTLR_T0                                ((uint8_t)0x01)               /* Bit 0 */
#define  WWDG_CTLR_T1                                ((uint8_t)0x02)               /* Bit 1 */
#define  WWDG_CTLR_T2                                ((uint8_t)0x04)               /* Bit 2 */
#define  WWDG_CTLR_T3                                ((uint8_t)0x08)               /* Bit 3 */
#define  WWDG_CTLR_T4                                ((uint8_t)0x10)               /* Bit 4 */
#define  WWDG_CTLR_T5                                ((uint8_t)0x20)               /* Bit 5 */
#define  WWDG_CTLR_T6                                ((uint8_t)0x40)               /* Bit 6 */

#define  WWDG_CTLR_WDGA                              ((uint8_t)0x80)               /* Activation bit */

/*******************  Bit definition for WWDG_CFGR register  *******************/
#define  WWDG_CFGR_W                                 ((uint16_t)0x007F)            /* W[6:0] bits (7-bit window value) */
#define  WWDG_CFGR_W0                                ((uint16_t)0x0001)            /* Bit 0 */
#define  WWDG_CFGR_W1                                ((uint16_t)0x0002)            /* Bit 1 */
#define  WWDG_CFGR_W2                                ((uint16_t)0x0004)            /* Bit 2 */
#define  WWDG_CFGR_W3                                ((uint16_t)0x0008)            /* Bit 3 */
#define  WWDG_CFGR_W4                                ((uint16_t)0x0010)            /* Bit 4 */
#define  WWDG_CFGR_W5                                ((uint16_t)0x0020)            /* Bit 5 */
#define  WWDG_CFGR_W6                                ((uint16_t)0x0040)            /* Bit 6 */

#define  WWDG_CFGR_WDGTB                             ((uint16_t)0x0180)            /* WDGTB[1:0] bits (Timer Base) */
#define  WWDG_CFGR_WDGTB0                            ((uint16_t)0x0080)            /* Bit 0 */
#define  WWDG_CFGR_WDGTB1                            ((uint16_t)0x0100)            /* Bit 1 */

#define  WWDG_CFGR_EWI                               ((uint16_t)0x0200)            /* Early Wakeup Interrupt */

/*******************  Bit definition for WWDG_STATR register  ********************/
#define  WWDG_STATR_EWIF                       ((uint8_t)0x01)               /* Early Wakeup Interrupt Flag */

/******************************************************************************/
/*                                 ARGB                                     */
/******************************************************************************/
/*******************  Bit definition for ARGB_CTLR register  *******************/
#define  ARGB_TE                                     ((uint32_t)0x00000001)
#define  ARGB_DMAEN                                  ((uint32_t)0x00000002)
#define  ARGB_MODE                                   ((uint32_t)0x00000004)
#define  ARGB_LSB                                    ((uint32_t)0x00000008)

#define  ARGB_FRHIE                                  ((uint32_t)0x00000100)
#define  ARGB_RSTIE                                  ((uint32_t)0x00000200)
#define  ARGB_TCIE                                   ((uint32_t)0x00000400)
#define  ARGB_BYTEIE                                 ((uint32_t)0x00000800)
#define  ARGB_HALFIE                                 ((uint32_t)0x00001000)
#define  ARGB_TXEIE                                  ((uint32_t)0x00002000)

/*******************  Bit definition for ARGB_DAT_ARR register  *******************/
#define  ARGB_DAT_ARR                                ((uint32_t)0x00000FFF)

/*******************  Bit definition for ARGB_RST_ARR register  *******************/
#define  ARGB_RST_ARR                                ((uint32_t)0x0001FFFF)

/*******************  Bit definition for ARGB_CCRL register  *******************/
#define  ARGB_CCRL                                   ((uint32_t)0x00000FFF)

/*******************  Bit definition for ARGB_CCRH register  *******************/
#define  ARGB_CCRH                                   ((uint32_t)0x00000FFF)

/*****************  Bit definition for ARGB_DAT_CYC register  ******************/
#define  ARGB_DAT_CYC                                ((uint32_t)0x0000FFFF)

/****************  Bit definition for ARGB_DAT_DATAR register  *****************/
#define  ARGB_DATA                                   ((uint32_t)0x000000FF)

/****************  Bit definition for ARGB_STATR register  *****************/
#define  ARGB_FRHIF                                  ((uint32_t)0x00000001)
#define  ARGB_RSTIF                                  ((uint32_t)0x00000002)
#define  ARGB_TCIF                                   ((uint32_t)0x00000004)
#define  ARGB_BYTEIF                                 ((uint32_t)0x00000008)
#define  ARGB_HALFIF                                 ((uint32_t)0x00000010)
#define  ARGB_TXEIF                                  ((uint32_t)0x00000020)

#define  ARGB_STATE                                  ((uint32_t)0x000000C0)
#define  ARGB_STATE_0                                ((uint32_t)0x00000040)
#define  ARGB_STATE_1                                ((uint32_t)0x00000080)

#define  ARGB_BIT_CNT                                ((uint32_t)0x00000700)
#define  ARGB_BYTE_CNT                               ((uint32_t)0xFFFF0000)

/****************  Bit definition for ARGB_CNTR register  *****************/
#define  ARGB_CNT                                    ((uint32_t)0x0001FFFF)


#include "ch32x3x5_conf.h"

#ifdef __cplusplus
}
#endif

#endif 




