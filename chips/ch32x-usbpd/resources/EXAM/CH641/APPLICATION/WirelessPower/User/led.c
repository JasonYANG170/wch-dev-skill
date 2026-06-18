/********************************** (C) COPYRIGHT *******************************
* File Name          : led.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2023/10/23
* Description        : led
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "led.h"

LED_PARAM led;
uint8_t const led_breath_trig_buf[21]={0,0,0,0,1, 1,2,2,3,4, 5,6,7,8,10, 11,13,14,16,18, 20};


/*********************************************************************
 * @fn      Tmr_ISR_Callback
 *
 * @brief   Timer interrupt callback function, called once in 1ms. Because it is an interrupt function, do not take up too much time.
 *
 * @return  none
 */
void Tmr_ISR_Callback( void )
{
    if(led.soft_breath_en)
    {
        if(led.led_breath_tick < led.led_breath_trig)        /* trig=0  0%     trig>=20  100% */
        {
            LED_ON_BLUE();
        }
        else
        {
            LED_OFF_BLUE();
        }
        if(led.led_breath_tick > 18)
        {
            led.led_breath_tick = 0;
        }
        else
        {
            led.led_breath_tick++;
        }
    }
    else if(led.bit_led_initok == 0)
    {
        led.led_init_tmr_cnt++;
        if(led.led_init_tmr_cnt>499)
        {
            led.led_init_tmr_cnt = 0;
            if(led.led_init_stage == 0)
            {
                LED_OFF_RED();
                LED_OFF_BLUE();
            }
            else if(led.led_init_stage == 1)
            {
                LED_ON_RED();
                LED_ON_BLUE();
            }
            else if(led.led_init_stage == 2)
            {
                LED_OFF_RED();
                LED_OFF_BLUE();
            }
            else
            {
                led.bit_led_initok = 1;
            }
            led.led_init_stage++;
        }
    }
}

/*********************************************************************
 * @fn      LED_Breath_Ctl
 *
 * @brief   This function is only executed after start_led_breath. It is eventually called by the main program loop call User_Loop_Proc.
 *          Breathing control, including time control, breathing effect, can be modified by yourself.
 *          Default light effect: adjust the brightness every 100ms. The effect is from all out to the brightest, and from the brightest to all out.
 *          This function, controls the breathing change effect. The focus is on controlling the change in led_breath_trig
 *
 * @return  none
 */
void LED_Breath_Ctl( void )
{
    led.tmr_led_breath_cycle += timer.tmr_ms_dlt;
    if(led.tmr_led_breath_cycle > 99)
    {
        led.tmr_led_breath_cycle = 0;
        if( led.led_trig_inc )
        {
            led.led_breath_trig_x++;
            if(led.led_breath_trig_x >=20 )
            {
                led.led_trig_inc = 0;
                led.led_breath_trig_x=20;
            }
        }
        else
        {
            led.led_breath_trig_x--;
            if((led.led_breath_trig_x==0) || (led.led_breath_trig_x>20))
            {
                led.led_trig_inc = 1;
                led.led_breath_trig_x = 0;
            }
        }
        led.led_breath_trig = led_breath_trig_buf[led.led_breath_trig_x];
    }
}

/*********************************************************************
 * @fn      start_led_breath
 *
 * @brief   Start breath.
 *
 * @return  none
 */
static void start_led_breath( void )
{
    if(led.soft_breath_en == 0)
    {
        led.soft_breath_en = 1;
        led.led_breath_tick = 0;
        led.led_breath_trig = 0;
        led.led_breath_trig_x = 0;
    }
}

/*********************************************************************
 * @fn      stop_led_breath
 *
 * @brief   Stop breath.
 *
 * @return  none
 */
static void stop_led_breath( void )
{
    led.soft_breath_en = 0;
}

/*********************************************************************
 * @fn      Pre_Init_Callback
 *
 * @brief   Called once near the entrance to the main loop, it can be used to initialize the lighting finish.
 *
 * @return  none
 */
void Pre_Init_Callback( void )
{
    uint8_t i = 0;
    //Tentatively set a timeout of 5S, if in fact the total time for the light to come on is longer than this, it can be lengthened.
    while(led.bit_led_initok == 0)                             //Wait for the initialization of the light display to complete and continue.
    {
        i++;
        if(i>250) break;
        mDelaymS(20);
    }
    led.bit_led_initok = 1;
}

/*********************************************************************
 * @fn      LED_Init
 *
 * @brief   Just powered up indicator status. When this function is called, the delay function has not been initialized and should not be used yet.
 *
 * @return  none
 */
void LED_Init( void )
{
    LED_ON_RED();
    LED_ON_BLUE();

    GPIOA->CFGLR &= ~0x0F000000;                //PA6
    GPIOA->CFGLR |= 0x03000000;

    GPIOB->CFGLR &= ~0x000000F0;                //PB1
    GPIOB->CFGLR |= 0x00000030;

    stop_led_breath();
    led.bit_led_initok = 0;
    led.led_init_tmr_cnt = 0;
    led.led_init_stage = 0;
}

/*********************************************************************
 * @fn      LED_Standby
 *
 * @brief   standby mode.
 *
 * @return  none
 */
void LED_Standby( void )
{
    stop_led_breath();
    LED_ON_RED();
    LED_OFF_BLUE();
}

/*********************************************************************
 * @fn      LED_Standby
 *
 * @brief   Preparation for entry (single execution).
 *
 * @return  none
 */
void LED_Rdy2work( void )
{
    LED_OFF_RED();
    start_led_breath();
}

/*********************************************************************
 * @fn      LED_Rdy2full
 *
 * @brief   Just received a full pack to handle (not powered down at this time).
 *
 * @return  none
 */
void LED_Rdy2full( void )
{
    Expt.bit_det_wait = 1;       //Changing this bit selects whether or not to turn off the power when it is full 0: no power off 1: power off
    /* If you don't turn off the power, you can selectively turn the lights here. */
    if(Expt.bit_det_wait == 0)
    {

    }
    else
    {
        stop_led_breath();
    }
}

/*********************************************************************
 * @fn      LED_Full
 *
 * @brief   Full indicator processing in power off state.
 *
 * @return  none
 */
void LED_Full( void )
{
    LED_OFF_RED();
    LED_ON_BLUE();
}

/*********************************************************************
 * @fn      LED_Tog
 *
 * @brief   Exception flashing light handling. Included here are common exceptions, ntc, vhv.
 *
 * @return  none
 */
void LED_Tog( void )
{
    led.tmr_LED_Tog_cycle += timer.tmr_ms_dlt;
    if(led.tmr_LED_Tog_cycle > 250)
    {
        led.tmr_LED_Tog_cycle = 0;
        if( (Expt.bit_det_wait && (Expt.bit_det_wait_mode==0)) || Expt.bit_ntc_trig || Expt.bit_vhv_trig )
        {
            stop_led_breath();
            if((GPIOA->OUTDR & BLUE_LED))
            {
                LED_OFF_RED();
                LED_ON_BLUE();
            }
            else {
                LED_ON_RED();
                LED_OFF_BLUE();
            }
        }
    }
}


