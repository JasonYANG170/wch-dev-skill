/********************************** (C) COPYRIGHT *******************************
* File Name          : adc.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/10/23
* Description        : Adc-related configuration
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "adc.h"

static uint16_t ISEN_Bias;                                 /* Current calibration deviation value */

/*********************************************************************
 * @fn      ADC_User_Init
 *
 * @brief   ADC Initialization
 *          PA6 - NTC (default push-pull, analog for ADC sampling)
 *          PA7 - VOSC ISP
 *
 * @return  none
 */
void ADC_User_Init( void )
{
    RCC->CFGR0 &= ~0x0001F800;
    RCC->CFGR0 |= (0x35<<11);                   //adc clock 500K

    GPIOA->CFGLR &= ~0xF0000000;

    EXTEN->CTLR2 &= ~(0x17<<8);
    EXTEN->CTLR2 |= (0x01<<8);                  //ISP differential inputs, internal bias off

    RCC->APB2PRSTR |= (1<<9);                   //ADC module reset
    RCC->APB2PRSTR &= ~(1<<9);                  //ADC Module Recovery

    ADC1->CTLR2 = (7<<17) | (1<<0);             //Turn on ADC module power

    ADC1->CTLR2 |= CTLR2_RSTCAL_Set;            //Resets the selected ADC calibration registers.
    while( ADC1->CTLR2 & CTLR2_RSTCAL_Set );
    ADC1->CTLR2 |= CTLR2_CAL_Set;
    while( ADC1->CTLR2 & CTLR2_CAL_Set );
    ADC1->SAMPTR2 = 0x02;                       //Sampling time 37 TICK
}

/*********************************************************************
 * @fn      ADC_Set_VHV
 *
 * @brief   Select adc channel as vhv (20% input).
 *
 * @return  none
 */
void ADC_Set_VHV( void )
{
    ADC1->RSQR3 = ADC_Channel_VHV;
}

/*********************************************************************
 * @fn      ADC_Set_NTC
 *
 * @brief   Select adc channel as PA6 ntc.
 *
 * @return  none
 */
void ADC_Set_NTC( void )
{
    GPIOA->CFGLR &= ~0x0F000000;
    ADC1->RSQR3 = ADC_Channel_NTC;
}
/*********************************************************************
 * @fn      ADC_NTC_Release
 *
 * @brief   Select adc channel as PA6 ntc.
 *
 * @return  none
 */
void ADC_NTC_Release( void )
{
    GPIOA->CFGLR |= 0x03000000;
}

/*********************************************************************
 * @fn      ADC_Set_OSC
 *
 * @brief   Select adc channel as PA7 OSC_ADC.
 *
 * @return  none
 */
void ADC_Set_OSC( void )
{
    ADC1->RSQR3 = ADC_Channel_OSC;
}

/*********************************************************************
 * @fn      ADC_Set_ISEN
 *
 * @brief   Select adc channel as ISP.
 *
 * @return  none
 */
void ADC_Set_ISEN( void )
{
    ADC1->RSQR3 = ADC_Channel_ISP;
}

/*********************************************************************
 * @fn      ADC_Start
 *
 * @brief   Start adc, read adc data once 10bit ADC.
 *          At 48M, 1M is about 17.2us.
 *          Takes about 20 ADC clock cycles to complete
 *
 * @return  none
 */
uint16_t ADC_Start( void )
{
    ADC1->CTLR2 |= 1;                           //Start ADC sampling
    while( (ADC1->STATR & 0x02) == 0 );
    ADC1->STATR &= ~0x02;

    if((ADC1->RSQR3 & 0x1F) == ADC_Channel_ISP) //Current measurement channel. Measured values with fixed deviation
    {
        if(ADC1->RDATAR > ISEN_Bias )
            return ( ADC1->RDATAR - ISEN_Bias );
        else
            return 0;
    }
    else
    {
        return ADC1->RDATAR;
    }
}

/*********************************************************************
 * @fn      ADC_Sample_X4
 *
 * @brief   Four consecutive samples were averaged.
 *
 * @return  none
 */
uint16_t ADC_Sample_X4(void)
{
    uint16_t adc_temp;
    adc_temp = ADC_Start();
    adc_temp += ADC_Start();
    adc_temp += ADC_Start();
    adc_temp += ADC_Start();
    adc_temp >>= 2;
    return adc_temp;
}

/*********************************************************************
 * @fn      ISEN_Calibration
 *
 * @brief   Current sampling calibration, be sure to calibrate once when there is no current.
 *
 * @return  none
 */
void ISEN_Calibration( void )
{
    uint8_t i;
    ISEN_Bias = 0;
    ADC1->RSQR3 = ADC_Channel_ISP;
    for(i = 0; i != 4; i++)
    {
        ADC1->CTLR2 |= 1;
        while( (ADC1->STATR & 0x02) == 0 );
        ADC1->STATR &= ~0x02;
        ISEN_Bias += ADC1->RDATAR;
    }
    ISEN_Bias >>= 2;
#if (DE_PRINTF == 2)
    printf("bias:%d ", ISEN_Bias);
#endif
}
