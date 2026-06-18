/********************************** (C) COPYRIGHT *******************************
* File Name          : user_proc.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/10/23
* Description        : User profile
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "user_proc.h"
#include "led.h"
#include "adc.h"
#include "logic.h"


USER_CFG usercfg;

/*********************************************************************
 * @fn      Recv_QI_Data_Callback
 *
 * @brief   QI receives data, which is used to output the message.
 *
 * @return  none
 */
void Recv_QI_Data_Callback( void )
{
#if (DE_PRINTF == 1)
    uint8_t i;
    for(i=0;i!=Msg_Proc.Com_Buf_Len;i++)
    {
        uart_send_dat(Msg_Proc.Com_Buf[i]);
    }
#elif (DE_PRINTF == 2)
    uint8_t i;
    for(i=0;i!=Msg_Proc.Com_Buf_Len;i++)
    {
        printf("%02X ",Msg_Proc.Com_Buf[i]);
    }
#endif
}

/*********************************************************************
 * @fn      Get_Cur_Q_Ref
 *
 * @brief   QI receives data, which is used to output the message.
 *          Meas < Expt.q_value_ref. Equipment considered to be present, otherwise not present
 *
 * @return  none
 */
void Get_Cur_Q_Ref(uint16_t meas)
{
#if (DE_PRINTF == 2)
    printf("osc_adc:%d",meas);
#endif
    if(volt_state.bit_fastcharge == 0) Expt.q_value_ref = Q_VALUE_LTHRD;
    else Expt.q_value_ref = Q_VALUE_HTHRD;
    if(volt_state.bit_highvol) Expt.q_value_ref = Q_VALUE_HHTHRD;
}

/*********************************************************************
 * @fn      Get_ISEN_State
 *
 * @brief   Get the current state (you can modify the PING_1_6A_ISEN PING_0_3A_ISEN macro definition, only sampling circuit changes, set to the corresponding value can).
 *
 * @return  none
 */
void Get_ISEN_State( void )
{
    uint16_t adc_temp;
    ADC_Set_ISEN();
    adc_temp = ADC_Start();
//    printf("%d ",adc_temp);

    if( adc_temp > PING_1_6A_ISEN )
    {
        Expt.fod_cnt |= 0x80;
        if(Expt.fod_cnt&0x40) Expt.fod_cnt = 0x80;
        Expt.fod_cnt++;
        if((Expt.fod_cnt & 0x1f) > 1) Expt.fod_cnt |= 0x20;
    }
    else if(adc_temp > PING_0_3A_ISEN)
    {
        Expt.fod_cnt |= 0x40;
        if(Expt.fod_cnt&0x80) Expt.fod_cnt = 0x40;
        if((Expt.fod_cnt & 0x1f) < 0x1f) Expt.fod_cnt++;
        if((Expt.fod_cnt & 0x1f) > 8) Expt.fod_cnt |= 0x20;
    }
}

/*********************************************************************
 * @fn      OSC_Check
 *
 * @brief   OSC Overvoltage Handling
 *          Generally do not need to be modified, it has a certain effect on the charging distance.
 *
 * @return  none
 */
void OSC_Check( uint16_t meas )
{
    if(meas > OSC_WORK_TRIG_HVOLT) Remove_Device();
    else if(meas > OSC_WORK_TRIG_LVOLT)
    {
        pwm_ctrl.bit_power_dir = 1;
        Power_Proc( 1 );
    }
}

/*********************************************************************
 * @fn      Over_ISEN_Check
 *
 * @brief   Overcurrent handling (the macro MAX_ISEN_LIMIT can be modified, no other changes are recommended).
 *
 * @return  none
 */
void Over_ISEN_Check( void )
{
    uint16_t adc_temp;
    ADC_Set_ISEN();
    adc_temp = ADC_Start();
    if(adc_temp > RT_ISEN_LIMIT)
    {
        pwm_ctrl.bit_power_dir = 1;
        Power_Proc( 1 );
    }
}

/*********************************************************************
 * @fn      NTC_Check
 *
 * @brief   Temperature detection (macro definitions can be modified)
 *
 * @return  none
 */
void NTC_Check( void )
{
    uint16_t adc_temp;
    ADC_Set_NTC();
    adc_temp = ADC_Start();
    ADC_NTC_Release();
    if(Expt.bit_ntc_trig)
    {
        if(adc_temp > NTC_RESUM_VALUE)
        {
            Expt.bit_ntc_lock = 0;
            Expt.bit_ntc_trig = 0;
        }
    }
    else
    {
        if(adc_temp < NTC_TRIG_VALUE)
        {
#if (DE_PRINTF == 2)
            printf("ntc trig\n");
#endif
            Expt.bit_ntc_trig = 1;
            Remove_Device();
        }
        else if(adc_temp < NTC_ADJ_VALUE)           //Memorize the current current at the moment of the first trigger, halve it as the adjustment point, until the temperature is lower than this temperature, unlock.
        {
            if(Expt.bit_ntc_lock == 0)
            {
                Expt.bit_ntc_lock = 1;

                ADC_Set_ISEN();
                max_isen = ADC_Sample_X4();
                max_isen >>= 1;                     //Get half the value of the coil current
                if(max_isen < NTC_MIN_ISEN) max_isen = NTC_MIN_ISEN;
            }
        }
        else
        {
            Expt.bit_ntc_lock = 0;                  //Restore the original current limit point
        }
    }
}

/*********************************************************************
 * @fn      VHV_Check
 *
 * @brief   Voltage detection (macro definitions can be modified)
 *
 * @return  none
 */
void VHV_Check( void )
{
    uint16_t adc_temp;
    ADC_Set_VHV();
    adc_temp = ADC_Sample_X4();
    if(Expt.bit_vhv_trig == 0)
    {
        if((adc_temp > VHV_TRIG_HIGH) || (adc_temp < VHV_LTRIG_LOW))
        {
            Expt.bit_vhv_trig = 1;
            Remove_Device();
        }
    }
    else
    {
        if((adc_temp > VHV_LTRIG_HIGH) && (adc_temp < VHV_TRIG_LOW))
        {
            Expt.bit_vhv_trig = 0;
        }
    }
}

/*********************************************************************
 * @fn      Get_Real_ISEN
 *
 * @brief   Get the current real current value (return value in mA)
 *
 * @return  none
 */
uint16_t Get_Real_ISEN( void )
{
    uint16_t adc_temp;
    ADC_Set_ISEN();
    adc_temp = ADC_Sample_X4();
    adc_temp = adc_temp<<2;    //adc_temp*4
    return adc_temp;
}

/*********************************************************************
 * @fn      Get_Real_ISEN
 *
 * @brief   magsafe mode Get power percentage callback Specific implementation, ADC measurement value is converted to power value 0~100
 *
 * @return  none
 */
void Get_Cur_Batpercent( void )
{
    usercfg.var_batpercent = 100;
}

/*********************************************************************
 * @fn      App_Cfg_Init
 *
 * @brief   Initialization of the configuration of common user parameters, called before entering the main program.
 *          Pop-up window settings bit_ap_pop_win_en (0 off, 1 on),bit_lv_ap_pop_win (0, low-voltage does not pop-up window 1, allows low-voltage 5V pop-up window, note the premise is that at the same time you need to turn on the pop-up window enable bit_ap_pop_win_en = 1).
 *          bit_magsafe_pop_mode(0:generic popup 1: magsafe popup (show current power var_batpercent).
 *          Standby low power setting (off for bit_low_power=0, on for bit_low_power=1).
 *          Whether to set timeout bit_fod_notry for static FOD foreign object 0: with timeout 2min exit function 1: dead wait for removal, will not timeout and retry.
 *          12V request (bit_vhv12_prefer=1 for 12V, bit_vhv12_prefer=0 for 9V), note that you need to synchronize the modification of the .H file VHV over-voltage protection value.
 *
 * @return  none
 */
void App_Cfg_Init( void )
{
    usercfg.bit_ap_pop_win_en = 1;
    usercfg.bit_lv_ap_pop_win = 1;

    usercfg.bit_magsafe_pop_mode = 0;

    usercfg.bit_low_power = 0;                  // Low-power standby mode is turned off by default, which can be mainly used for charging treasure and other scenarios.
    usercfg.bit_fod_notry = 0;
    usercfg.bit_vhv12_prefer = 1;               // 12V requested by default

    usercfg.bit_epp20_en = 0;                   //0:epp default 15W protocol 1: enable epp 20W (currently some test stands support this mode).

    if((DBGMCU_GetDEVID() & 0xF0) == 0x10 )     //version B, the P-tube drive capability is doubled
    {
        usercfg.pwm_dead_time = 4;                  // The default value is 11, i.e. 230ns, which is large for MOS compatibility, e.g. small gate capacitance, which can be scaled down.
        usercfg.pwm_dt_forword = 3;                 //This value is set smaller than pwm_dead_time. The library does not set a limit on it, but do not set it arbitrarily.
    }
    else
    {
        usercfg.pwm_dead_time = 5;
        usercfg.pwm_dt_forword = 3;
    }
#if DE_PRINTF
    USART_Printf_Init(115200);
#endif
}

/*********************************************************************
 * @fn      Blind_Callback
 *
 * @brief   Printing and other processing
 *
 * @return  none
 */
void Blind_Callback( void )
{
    uint16_t adc_temp;
#if (DE_PRINTF == 2)
    printf("loss\n");
#endif
    ADC_Set_OSC();
    adc_temp = ADC_Start( );
    if(adc_temp > OSC_BLIND_RMV_VOLT)
    {
        Blind_Cont_Cnt = 0xff;
    }
}

/*********************************************************************
 * @fn      Get_Pingrmv_ISEN
 *
 * @brief   Remove current, if the current is too high during PING, remove the device current
 *
 * @return  none
 */
uint16_t Get_Pingrmv_ISEN( void )
{
    return PING_0_8A_ISEN;
}

/*********************************************************************
 * @fn      User_Loop_Proc
 *
 * @brief   Program segments executed by the main program cycle (you can add your own, but it is not recommended that the segments be too time-consuming).
 *
 * @return  none
 */
void User_Loop_Proc( void )
{
    if(led.soft_breath_en)
    {
        LED_Breath_Ctl();
    }

}

