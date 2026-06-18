/********************************** (C) COPYRIGHT *******************************
* File Name          : system_ch32x3x5.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/01
* Description        : CH32X3x5 Device Peripheral Access Layer System Source File.
*                      For HSE = 20Mhz
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "ch32x3x5.h" 

/* 
* Uncomment the line corresponding to the desired System clock (SYSCLK) frequency (after 
* reset the HSI is used as SYSCLK source).
* If none of the define below is enabled, the HSI is used as System clock source. 
*/

// #define SYSCLK_CoreCLK_HCLK_HSE    HSE_VALUE
// #define SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSE    240000000
#define SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSE    240000000
// #define SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSE    312500000
// #define SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSE    480000000
// #define SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSI    240000000
// #define SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSI    240000000
// #define SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSI    312500000
// #define SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSI    480000000

/* Clock Definitions */
#ifdef SYSCLK_CoreCLK_HCLK_HSE
uint32_t SystemClock         = HSE_VALUE;              /* System Clock Frequency */
uint32_t SystemCoreClock     = HSE_VALUE;
uint32_t HCLKClock           = HSE_VALUE;
#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSE
uint32_t SystemClock         = SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSE;              /* System Clock Frequency */
uint32_t SystemCoreClock     = SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSE;
uint32_t HCLKClock           = SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSE >> 1;
#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSE
uint32_t SystemClock         = SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSE;              /* System Clock Frequency */
uint32_t SystemCoreClock     = SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSE;
uint32_t HCLKClock           = SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSE;
#elif defined SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSE
uint32_t SystemClock         = SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSE;        /* System Clock Frequency */
uint32_t SystemCoreClock     = SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSE;
uint32_t HCLKClock           = SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSE;
#elif defined SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSE
uint32_t SystemClock         = SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSE;              /* System Clock Frequency */
uint32_t SystemCoreClock     = SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSE;
uint32_t HCLKClock           = SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSE >> 1;
#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSI
uint32_t SystemClock         = SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSI;              /* System Clock Frequency */
uint32_t SystemCoreClock     = SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSI;
uint32_t HCLKClock           = SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSI >> 1;
#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSI
uint32_t SystemClock         = SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSI;              /* System Clock Frequency */
uint32_t SystemCoreClock     = SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSI;
uint32_t HCLKClock           = SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSI;
#elif defined SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSI
uint32_t SystemClock         = SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSI;        /* System Clock Frequency */
uint32_t SystemCoreClock     = SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSI;
uint32_t HCLKClock           = SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSI;
#elif defined SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSI
uint32_t SystemClock         = SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSI;              /* System Clock Frequency */
uint32_t SystemCoreClock     = SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSI;
uint32_t HCLKClock           = SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSI >> 1;
#else
uint32_t SystemClock         = HSI_VALUE;              /* System Clock Frequency */
uint32_t SystemCoreClock     = HSI_VALUE;
uint32_t HCLKClock           = HSI_VALUE;
#endif

static __I uint8_t PLLDIV[16] = {6, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
static __I uint8_t HPrescTable[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4, 6, 7, 8, 9};

/* system_private_function_proto_types */
static void SetSysClock(void);

#ifdef SYSCLK_CoreCLK_HCLK_HSE
static void SetSYSCLK_HCLK_HSE();
#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSE
static void SetSYSCLK_240M_CoreCLK_240M_HCLK_120M_HSE( void );
#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSE
static void SetSYSCLK_240M_CoreCLK_240M_HCLK_240M_HSE( void );
#elif defined SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSE
static void SetSYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSE( void );
#elif defined SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSE
static void SetSYSCLK_480M_CoreCLK_480M_HCLK_240M_HSE( void );
#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSI
static void SetSYSCLK_240M_CoreCLK_240M_HCLK_120M_HSI( void );
#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSI
static void SetSYSCLK_240M_CoreCLK_240M_HCLK_240M_HSI( void );
#elif defined SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSI
static void SetSYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSI( void );
#elif defined SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSI
static void SetSYSCLK_480M_CoreCLK_480M_HCLK_240M_HSI( void );

#endif


/*********************************************************************
 * @fn      SystemInit
 *
 * @brief   Setup the microcontroller system Initialize the Embedded Flash Interface,
 *        the PLL and update the SystemCoreClock variable.
 *
 * @return  none
 */
void SystemInit (void)
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

  FLASH->KEYR = ((uint32_t)0x45670123);
  FLASH->KEYR = ((uint32_t)0xCDEF89AB);
  FLASH->MODEKEYR = ((uint32_t)0x45670123);
  FLASH->MODEKEYR = ((uint32_t)0xCDEF89AB);
  FLASH->ACTLR |= ((uint32_t)0x00000800);
  FLASH->CTLR |= ((uint32_t)0x00008080);

  SetSysClock();
}


/*********************************************************************
 * @fn      SystemCoreClockUpdate
 *
 * @brief   Update SystemCoreClock variable according to Clock Register Values.
 *
 * @return  none
 */
void SystemCoreClockUpdate (void)
{
  uint32_t tmp = 0,tmp1 = 0, presc = 0, presc1 = 0;

  tmp = RCC->CFGR0 & RCC_SWS;
  tmp1 = RCC->CFGR0 & RCC_PLLSRC;

  switch(tmp)
  {
      case 0x00:
          SystemClock = HSI_VALUE;
          break;

      case 0x04:
          SystemClock = HSE_VALUE;
          break;

      case 0x08:
          switch(tmp1)
          { 
              case RCC_PLLSRC_USBHSPLL_480M:
                  presc = ((RCC->CFGR0 & RCC_PLLDIV)>>17);
                  SystemClock = 480000000/PLLDIV[presc];
                  break;
              case RCC_PLLSRC_USBSSPLL_625M:
                  presc = ((RCC->CFGR0 & RCC_PLLDIV)>>17);                
                  SystemClock = 625000000/PLLDIV[presc];
                  break;
              case RCC_PLLSRC_HS:
              case RCC_PLLSRC_HS_1:
                  tmp = RCC->CFGR0 & RCC_PLLHS_SRC;
                  switch(tmp)
                  {
                      case RCC_PLLHS_SRC_USBSSPLL_125M:
                          SystemClock = 125000000;
                          break;
                      case RCC_PLLHS_SRC_USBSSPLL_357M:    
                          SystemClock = 375000000;
                          break;
                      case RCC_PLLHS_SRC_USBSSPLL_625M:    
                          SystemClock = 625000000;
                          break; 
                      case RCC_PLLHS_SRC_USBHSPLL_480M:    
                          SystemClock = 480000000;
                          break; 
                      default:
                          SystemClock = HSI_VALUE;
                          break;                                   
                  }
                  break;    

              default:
                  SystemClock = HSI_VALUE;
                  break;
          }  
          break;
      default:
          SystemClock = HSI_VALUE;
          break;
  }
  
  tmp = (RCC->CFGR0 & RCC_HPRE) >> 4;
  presc1 = HPrescTable[tmp];
  SystemCoreClock = SystemClock >> presc1;

  tmp = (RCC->CFGR0 & RCC_FPRE) >> 8;
  HCLKClock = SystemCoreClock >> tmp;
}

/*********************************************************************
 * @fn      SetSysClock
 *
 * @brief   Configures the System clock frequency, HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSysClock(void)
{
    GPIO_IPD_Unused();
#ifdef SYSCLK_CoreCLK_HCLK_HSE
    SetSYSCLK_HCLK_HSE();
#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSE
    SetSYSCLK_240M_CoreCLK_240M_HCLK_120M_HSE();
#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSE
    SetSYSCLK_240M_CoreCLK_240M_HCLK_240M_HSE();
#elif defined SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSE
    SetSYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSE();
#elif defined SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSE
    SetSYSCLK_480M_CoreCLK_480M_HCLK_240M_HSE();
#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSI
    SetSYSCLK_240M_CoreCLK_240M_HCLK_120M_HSI();
#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSI
    SetSYSCLK_240M_CoreCLK_240M_HCLK_240M_HSI();
#elif defined SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSI
    SetSYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSI();
#elif defined SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSI
    SetSYSCLK_480M_CoreCLK_480M_HCLK_240M_HSI();

#endif
 
 /* If none of the define above is enabled, the HSI is used as System clock
  * source (default after reset) 
    */
}


#ifdef SYSCLK_CoreCLK_HCLK_HSE

/*********************************************************************
 * @fn      SetSYSCLK_HCLK_HSE
 *
 * @brief   Sets HSE as System clock source and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSYSCLK_HCLK_HSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0, FLASH_Temp = 0;
   
  RCC->CTLR |= ((uint32_t)RCC_HSEON);
 
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* CoreClock = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;      
    /* HCLK = CoreClock */
    RCC->CFGR0 |= (uint32_t)RCC_FPRE_DIV1;
    
    /* Select FLASH clock frequency*/
    FLASH_Temp = FLASH->ACTLR;
    FLASH_Temp &= ~FLASH_ACTLR_SCK_CFG;
    FLASH_Temp |= FLASH_ACTLR_LATENCY_HCLK_DIV1;
    FLASH->ACTLR = FLASH_Temp;

    /* Select HSE as system clock source */
    RCC->CFGR0 &= (uint32_t)((uint32_t)~(RCC_SW));
    RCC->CFGR0 |= (uint32_t)RCC_SW_HSE;    

    /* Wait till HSE is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x04)
    {
    }
  }
  else
  { 
        /* If HSE fails to start-up, the application will have wrong clock
     * configuration. User can add here some code to deal with this error 
         */
  }  
}

#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSE

/*********************************************************************
 * @fn      SetSYSCLK_240M_CoreCLK_240M_HCLK_120M_HSE
 *
 * @brief   Sets System clock frequency to 120MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSYSCLK_240M_CoreCLK_240M_HCLK_120M_HSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0, FLASH_Temp = 0;
     
  RCC->CTLR |= ((uint32_t)RCC_HSEON);
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* CoreClock = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;    
    /* HCLK = CoreClock / 2*/
    RCC->CFGR0 |= (uint32_t)RCC_FPRE_DIV2;

    RCC->CFGR2 |= (uint32_t)(RCC_USBHSPLLSRCC_HSE | RCC_USBHSPLLREFSEL_20MHz);

    RCC->CTLR |= RCC_USBHSPLLON;
    /* Wait till USBHSPLL is ready */
    while((RCC->CTLR & RCC_USBHSPLLRDY) == 0)
    {
    }

    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_USBHSPLL_480M | RCC_PLLDIV2);

    /* Select FLASH clock frequency*/
    FLASH_Temp = FLASH->ACTLR;
    FLASH_Temp &= ~FLASH_ACTLR_SCK_CFG;
    FLASH_Temp |= FLASH_ACTLR_LATENCY_HCLK_DIV2;
    FLASH->ACTLR = FLASH_Temp;

    /* Select PLL as system clock source */
    RCC->CTLR &= (uint32_t)(~RCC_PLLGATE);
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;    
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { 
        /*
         * If HSE fails to start-up, the application will have wrong clock
     * configuration. User can add here some code to deal with this error 
         */
  } 
}

#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSE

/*********************************************************************
 * @fn      SetSYSCLK_240M_CoreCLK_240M_HCLK_240M_HSE
 *
 * @brief   Sets System clock frequency to 240MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSYSCLK_240M_CoreCLK_240M_HCLK_240M_HSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0, FLASH_Temp = 0;

  RCC->CTLR |= ((uint32_t)RCC_HSEON);
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* CoreClock = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;    
    /* HCLK = CoreClock */
    RCC->CFGR0 |= (uint32_t)RCC_FPRE_DIV1;

    RCC->CFGR2 |= (uint32_t)(RCC_USBHSPLLSRCC_HSE | RCC_USBHSPLLREFSEL_20MHz);

    RCC->CTLR |= RCC_USBHSPLLON;
    /* Wait till USBHSPLL is ready */
    while((RCC->CTLR & RCC_USBHSPLLRDY) == 0)
    {
    }

    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_USBHSPLL_480M | RCC_PLLDIV2);

    /* Select FLASH clock frequency*/
    FLASH_Temp = FLASH->ACTLR;
    FLASH_Temp &= ~FLASH_ACTLR_SCK_CFG;
    FLASH_Temp |= FLASH_ACTLR_LATENCY_HCLK_DIV4;
    FLASH->ACTLR = FLASH_Temp;

    /* Select PLL as system clock source */
    RCC->CTLR &= (uint32_t)(~RCC_PLLGATE);
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;    
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { 
        /*
         * If HSE fails to start-up, the application will have wrong clock
     * configuration. User can add here some code to deal with this error 
         */
  } 
}

#elif defined SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSE

/*********************************************************************
 * @fn      SetSYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSE
 *
 * @brief   Sets System clock frequency to 312_5MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0, FLASH_Temp = 0;

  RCC->CTLR |= ((uint32_t)RCC_HSEON);
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* CoreClock = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;    
    /* HCLK = CoreClock */
    RCC->CFGR0 |= (uint32_t)RCC_FPRE_DIV1;

    RCC->CFGR2 |= (uint32_t)(RCC_USBSSPLLSRCC_HSE | RCC_USBSSPLLREFSEL_20MHz);

    RCC->CTLR |= RCC_USBSSPLLON;
    /* Wait till USBSSPLL is ready */
    while((RCC->CTLR & RCC_USBSSPLLRDY) == 0)
    {
    }

    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_USBSSPLL_625M | RCC_PLLDIV2);

    /* Select FLASH clock frequency*/
    FLASH_Temp = FLASH->ACTLR;
    FLASH_Temp &= ~FLASH_ACTLR_SCK_CFG;
    FLASH_Temp |= FLASH_ACTLR_LATENCY_HCLK_DIV8;
    FLASH->ACTLR = FLASH_Temp;

    /* Select PLL as system clock source */
    RCC->CTLR &= (uint32_t)(~RCC_PLLGATE);
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;    
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { 
        /*
         * If HSE fails to start-up, the application will have wrong clock
     * configuration. User can add here some code to deal with this error 
         */
  } 
}

#elif defined SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSE

/*********************************************************************
 * @fn      SetSYSCLK_480M_CoreCLK_480M_HCLK_240M_HSE
 *
 * @brief   Sets System clock frequency to 48MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSYSCLK_480M_CoreCLK_480M_HCLK_240M_HSE(void)
{
  __IO uint32_t StartUpCounter = 0, HSEStatus = 0, FLASH_Temp = 0;

  RCC->CTLR |= ((uint32_t)RCC_HSEON);
  /* Wait till HSE is ready and if Time out is reached exit */
  do
  {
    HSEStatus = RCC->CTLR & RCC_HSERDY;
    StartUpCounter++;  
  } while((HSEStatus == 0) && (StartUpCounter != HSE_STARTUP_TIMEOUT));

  if ((RCC->CTLR & RCC_HSERDY) != RESET)
  {
    HSEStatus = (uint32_t)0x01;
  }
  else
  {
    HSEStatus = (uint32_t)0x00;
  }  

  if (HSEStatus == (uint32_t)0x01)
  {
    /* CoreClock = SYSCLK */
    RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;    
    /* HCLK = CoreClock / 2 */
    RCC->CFGR0 |= (uint32_t)RCC_FPRE_DIV2;

    RCC->CFGR2 |= (uint32_t)(RCC_USBHSPLLSRCC_HSE | RCC_USBHSPLLREFSEL_20MHz);

    RCC->CTLR |= RCC_USBHSPLLON;
    /* Wait till USBHSPLL is ready */
    while((RCC->CTLR & RCC_USBHSPLLRDY) == 0)
    {
    }

    RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HS|RCC_PLLHS_SRC_USBHSPLL_480M);

    /* Select FLASH clock frequency*/
    FLASH_Temp = FLASH->ACTLR;
    FLASH_Temp &= ~FLASH_ACTLR_SCK_CFG;
    FLASH_Temp |= FLASH_ACTLR_LATENCY_HCLK_DIV4;
    FLASH->ACTLR = FLASH_Temp;

    /* Select PLL as system clock source */
    RCC->CTLR &= (uint32_t)(~RCC_PLLGATE);
    RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;    
    /* Wait till PLL is used as system clock source */
    while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
    {
    }
  }
  else
  { 
        /*
         * If HSE fails to start-up, the application will have wrong clock
     * configuration. User can add here some code to deal with this error 
         */
  } 
}

#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_120M_HSI

/*********************************************************************
 * @fn      SetSYSCLK_240M_CoreCLK_240M_HCLK_120M_HSI
 *
 * @brief   Sets System clock frequency to 120MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSYSCLK_240M_CoreCLK_240M_HCLK_120M_HSI(void)
{
  __IO uint32_t FLASH_Temp = 0;

  /* CoreClock = SYSCLK */
  RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;    
  /* HCLK = CoreClock / 2 */
  RCC->CFGR0 |= (uint32_t)RCC_FPRE_DIV2;

  RCC->CFGR2 |= (uint32_t)(RCC_USBHSPLLSRCC_HSI | RCC_USBHSPLLREFSEL_20MHz);

  RCC->CTLR |= RCC_USBHSPLLON;
  /* Wait till USBHSPLL is ready */
  while((RCC->CTLR & RCC_USBHSPLLRDY) == 0)
  {
  }

  RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_USBHSPLL_480M | RCC_PLLDIV2);

  /* Select FLASH clock frequency*/
  FLASH_Temp = FLASH->ACTLR;
  FLASH_Temp &= ~FLASH_ACTLR_SCK_CFG;
  FLASH_Temp |= FLASH_ACTLR_LATENCY_HCLK_DIV2;
  FLASH->ACTLR = FLASH_Temp;

  /* Select PLL as system clock source */
  RCC->CTLR &= (uint32_t)(~RCC_PLLGATE);
  RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;    
  /* Wait till PLL is used as system clock source */
  while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
  {
  }
}

#elif defined SYSCLK_240M_CoreCLK_240M_HCLK_240M_HSI

/*********************************************************************
 * @fn      SetSYSCLK_480M_CoreCLK_480M_HCLK_240M_HSI
 *
 * @brief   Sets System clock frequency to 480MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSYSCLK_240M_CoreCLK_240M_HCLK_240M_HSI(void)
{
  __IO uint32_t FLASH_Temp = 0;

  /* CoreClock = SYSCLK */
  RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;    
  /* HCLK = CoreClock */
  RCC->CFGR0 |= (uint32_t)RCC_FPRE_DIV1;

  RCC->CFGR2 |= (uint32_t)(RCC_USBHSPLLSRCC_HSI | RCC_USBHSPLLREFSEL_20MHz);

  RCC->CTLR |= RCC_USBHSPLLON;
  /* Wait till USBHSPLL is ready */
  while((RCC->CTLR & RCC_USBHSPLLRDY) == 0)
  {
  }

  RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_USBHSPLL_480M | RCC_PLLDIV2);

  /* Select FLASH clock frequency*/
  FLASH_Temp = FLASH->ACTLR;
  FLASH_Temp &= ~FLASH_ACTLR_SCK_CFG;
  FLASH_Temp |= FLASH_ACTLR_LATENCY_HCLK_DIV4;
  FLASH->ACTLR = FLASH_Temp;

  /* Select PLL as system clock source */
  RCC->CTLR &= (uint32_t)(~RCC_PLLGATE);
  RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;    
  /* Wait till PLL is used as system clock source */
  while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
  {
  }
}

#elif defined SYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSI

/*********************************************************************
 * @fn      SetSYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSI
 *
 * @brief   Sets System clock frequency to 312_5MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSYSCLK_312_5M_CoreCLK_312_5M_HCLK_312_5M_HSI(void)
{
  __IO uint32_t FLASH_Temp = 0;

  /* CoreClock = SYSCLK */
  RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;    
  /* HCLK = CoreClock */
  RCC->CFGR0 |= (uint32_t)RCC_FPRE_DIV1;

  RCC->CFGR2 |= (uint32_t)(RCC_USBSSPLLSRCC_HSI | RCC_USBSSPLLREFSEL_20MHz);

  RCC->CTLR |= RCC_USBSSPLLON;
  /* Wait till USBSSPLL is ready */
  while((RCC->CTLR & RCC_USBSSPLLRDY) == 0)
  {
  }

  RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_USBSSPLL_625M | RCC_PLLDIV2);

  /* Select FLASH clock frequency*/
  FLASH_Temp = FLASH->ACTLR;
  FLASH_Temp &= ~FLASH_ACTLR_SCK_CFG;
  FLASH_Temp |= FLASH_ACTLR_LATENCY_HCLK_DIV8;
  FLASH->ACTLR = FLASH_Temp;

  /* Select PLL as system clock source */
  RCC->CTLR &= (uint32_t)(~RCC_PLLGATE);
  RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;    
  /* Wait till PLL is used as system clock source */
  while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
  {
  }
}

#elif defined SYSCLK_480M_CoreCLK_480M_HCLK_240M_HSI

/*********************************************************************
 * @fn      SetSYSCLK_480M_CoreCLK_480M_HCLK_240M_HSI
 *
 * @brief   Sets System clock frequency to 480MHz and configure HCLK, PCLK2 and PCLK1 prescalers.
 *
 * @return  none
 */
static void SetSYSCLK_480M_CoreCLK_480M_HCLK_240M_HSI(void)
{
  __IO uint32_t FLASH_Temp = 0;

  /* CoreClock = SYSCLK */
  RCC->CFGR0 |= (uint32_t)RCC_HPRE_DIV1;    
  /* HCLK = CoreClock / 2 */
  RCC->CFGR0 |= (uint32_t)RCC_FPRE_DIV2;

  RCC->CFGR2 |= (uint32_t)(RCC_USBHSPLLSRCC_HSI | RCC_USBHSPLLREFSEL_20MHz);

  RCC->CTLR |= RCC_USBHSPLLON;
  /* Wait till USBHSPLL is ready */
  while((RCC->CTLR & RCC_USBHSPLLRDY) == 0)
  {
  }

  RCC->CFGR0 |= (uint32_t)(RCC_PLLSRC_HS|RCC_PLLHS_SRC_USBHSPLL_480M);

  /* Select FLASH clock frequency*/
  FLASH_Temp = FLASH->ACTLR;
  FLASH_Temp &= ~FLASH_ACTLR_SCK_CFG;
  FLASH_Temp |= FLASH_ACTLR_LATENCY_HCLK_DIV4;
  FLASH->ACTLR = FLASH_Temp;

  /* Select PLL as system clock source */
  RCC->CTLR &= (uint32_t)(~RCC_PLLGATE);
  RCC->CFGR0 |= (uint32_t)RCC_SW_PLL;    
  /* Wait till PLL is used as system clock source */
  while ((RCC->CFGR0 & (uint32_t)RCC_SWS) != (uint32_t)0x08)
  {
  }
}

#endif
