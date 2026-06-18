/********************************** (C) COPYRIGHT *******************************
* File Name          : logic.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/10/23
* Description        : Power regulation and dynamic FOD
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "logic.h"
#include "user_proc.h"
#include "adc.h"

/******************************************************************************/
/* Constant,Variable Definition */
uint16_t max_isen;                     //Maximum current limit

/*********************************************************************
 * @fn      QI_CE_Handle
 *
 * @brief   CE Message Handling Functions.
 *
 * @param   dat - CE
 *
 * @return  none
 */
void QI_CE_Handle( uint8_t dat )
{
    uint8_t slow_adj;
    uint16_t adc_temp;

    slow_adj = 0;
    pwm_ctrl.bit_power_dir = 0;
    if(dat & 0x80)
    {
        dat = 0-dat;
        pwm_ctrl.bit_power_dir = 1;
    }

    /* Under Voltage Detection */
    ADC_Set_VHV();
    adc_temp = ADC_Sample_X4();
    if(adc_temp < Uvlo_Thrdvalue)
    {
#if (DE_PRINTF == 2)
        printf("low volt");
#endif
        pwm_ctrl.bit_power_dir = 1;
        Power_Proc( 1 );
    }
    else
    {
        if((adc_temp-Uvlo_Thrdvalue) < MIN_VOTL_ERR)                           /* Fast but not undervoltage, slow adjustment near threshold point */
        {
            slow_adj = 1;
        }

        ADC_Set_ISEN();                                                        /* Overcurrent detection */
        adc_temp = ADC_Sample_X4();
        if(Expt.bit_ntc_lock == 0)
        {
            if(pwm_ctrl.bit_127k_mode)
            {
                if(volt_state.bit_fastcharge)
                {
                    max_isen = AP_ISEN_LIMIT;
                }
                else
                {
                    max_isen = MAX_ISEN_LIMIT;
                }
            }
            else max_isen = MAX_ISEN_LIMIT;
        }
        if(adc_temp > max_isen)
        {
#if (DE_PRINTF == 2)
            printf("over IS");
#endif
            pwm_ctrl.bit_power_dir = 1;
            Power_Proc( 1 );
        }
        else
        {
            if((max_isen-adc_temp) < MAX_ISEN_ERR)                             /* Fast over but not overcurrent, slow adjustment near threshold point */
            {
                slow_adj = 1;
            }

            if(dat)                                                            /* Analyze the need for adjustment */
            {
                if(dat == 1) slow_adj = 1;                                      //dat = 1 to slow tune. If necessary, the steady-state slow-tuning deadband can be increased

                if( slow_adj )
                {
                    Power_Proc( 1 );
                }
                else
                {
                    PID_Process( dat );                                        /* PID Handling Functions */
                }

                /* Print Final Frequency */
#if (DE_PRINTF == 2)
                adc_temp = 48000/(TIM1->ATRLR+1);
                printf("freq:%d ",adc_temp);
                if(pwm_ctrl.bit_half_bridge)
                {
                    adc_temp = (TIM1->ATRLR+1) - TIM1->CH1CVR;
                    adc_temp = adc_temp*100/(TIM1->ATRLR+1);
                    printf("duty:%d half",adc_temp);
                }
                else
                {
                    adc_temp = (TIM1->CH3CVR *100)/(TIM1->ATRLR+1);
                    printf("duty:%d full",adc_temp);
                }
#endif
            }
        }
    }
}

/*********************************************************************
 * @fn      QI_RP_Handle
 *
 * @brief   Dynamic FOD Handling Functions.
 *          ADC_V*ADC_I < RP*121.094 *5/efficiency
 *          5W  55%: ADC_V*ADC_I < RP*1101 ----> (ADC_V>>3)*(ADC_I>>1) < RP*69
*           10W 60%: ADC_V*ADC_I < RP*2018 ----> (ADC_V>>3)*(ADC_I>>1) < RP*126
*           15W 70%: ADC_V*ADC_I < RP*2595 ----> (ADC_V>>3)*(ADC_I>>1) < RP*162
 *
 * @param   dat - RP
 *
 * @return  0 - non-triggered
 *          1 - triggered FOD
 */
uint8_t QI_RP_Handle( uint8_t dat )
{
#if DYN_FOD_ENABLE
    uint16_t adc_v,adc_i,rp_mul;

    ADC_Set_ISEN();
    adc_i = ADC_Sample_X4();
#if (DE_PRINTF == 2)
    printf("is:%dmA ",adc_i<<2);
#endif
    if(adc_i > MIN_FOD_ISEN)
    {
        adc_i >>= 1;

        ADC_Set_VHV();
        adc_v = ADC_Sample_X4();
#if (DE_PRINTF == 2)
    printf("volt:%dmV",(adc_v>>3)*129);
#endif
        adc_v >>= 3;
        if(Expt.bit_10w_power == 0)
        {
            rp_mul = dat * 69;
        }
        else
        {
            if(Expt.bit_15w_power == 0)
            {
                rp_mul = dat * 126;
            }
            else
            {
                rp_mul = dat * 162;
            }
        }
        if((adc_i * adc_v) > rp_mul)
        {
#if (DE_PRINTF == 2)
            printf("dyn fod ++\n");
#endif
            Expt.fod_cnt++;
        }
        else
        {
            Expt.fod_cnt = 0;
        }
        if(Expt.fod_cnt >= FOD_CHECK_MAXCNT)
        {
#if (DE_PRINTF == 2)
            printf("dyn fod trig\n");
#endif
            return 1;
        }
    }
    else
    {
        Expt.fod_cnt = 0;
    }
#endif
    return 0;
}
