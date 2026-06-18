/********************************** (C) COPYRIGHT *******************************
* File Name          : SYSFREQ.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : application system clock configuration, before main program or on the startup program
*******************************************************************************/



/******************************************************************************/
#include "CH563SFR.H"

#ifndef __SYSFREQ_H__
#define __SYSFREQ_H__

#ifdef __cplusplus
extern "C" {
#endif

//#define FREQ_SYS            100000000                 // define frequency requested by application, then auto calculation by this file
// support: 50000000,62500000,75000000,83333333,100000000,110000000,112500000,116666666,125000000,137500000,150000000,etc.

#ifndef MEM_DATA
#define MEM_DATA            64                        // memory configuration for data SRAM, 64=64K, 32=32K, 96=96K
#endif

extern void Delay_us( int n );  // 微秒级延时函数
extern void Delay_ms( int n );  // 毫秒级延时函数


/* PLL config auto calculation *************************************************************************************** */

/* FREQ_OSC                                              = external crystal frequency, 30MHz or 12MHz, default is 30MHz
            / FREQ_OSC_DIV                               = REF_FREQ, from 5MHz to 30MHz, default is 10MHz
                           * FREQ_VCO_MUL                = VCO_FREQ, from 100MHz to 330MHz, default is 250MHz
                                          / FREQ_SYS_DIV = SYS_FREQ, from 20MHz to 150MHz, default is 62.5MHz
                                          / n            = ETH_FREQ, 25MHz, not used if FREQ_ETH_XT=1
                                          / 3*n          = NRZI_CLK_FREQ, see R8_TMR0_NRZI_CK_DIV or R8_TMR1_NRZI_CK_DIV
*/

#ifndef FREQ_OSC
#define FREQ_OSC            30000000                  // default external crystal frequency, 30MHz
//#define FREQ_OSC            12000000                  // external crystal frequency, 30MHz or 12MHz if used USB, else from 10MHz to 30MHz
#endif
#define FREQ_OSC_30M        ( FREQ_OSC == 30000000 ? 1 : 0 )
#ifndef FREQ_SYS
//#define FREQ_SYS            62500000                  // default system clock frequency, 62.5MHz
#define FREQ_SYS            100000000                 // system clock frequency, 100MHz
#endif
#ifndef FREQ_ETH_XT
#define FREQ_ETH_XT         0                         // enanble external crystal for ethernet clock, else PLL
#endif

#if     FREQ_SYS == 25000000 || FREQ_SYS >= 33000000 && FREQ_SYS <= 34000000 // 25MHz, 33.33MHz
#ifndef FREQ_OSC_DIV
#define FREQ_OSC_DIV        3                         // PLL input frequency divisor, for 10MHz/4MHz
#endif
#ifndef FREQ_VCO_MUL
#define FREQ_VCO_MUL        (FREQ_OSC_30M?10:25)      // PLL VCO frequency multiplier, for 100MHz
#endif
#if     FREQ_SYS != 25000000
#ifndef FREQ_SYS_DIV
#define FREQ_SYS_DIV        3                         // PLL system frequency divisor
#endif
#endif
#endif

#if     FREQ_SYS == 31250000 || FREQ_SYS >= 41000000 && FREQ_SYS <= 42000000 // 31.25MHz, 41.67MHz
#ifndef FREQ_OSC_DIV
#define FREQ_OSC_DIV        6                         // PLL input frequency divisor, for 5MHz
#endif
#ifndef FREQ_VCO_MUL
#define FREQ_VCO_MUL        (FREQ_OSC_30M?25:0)       // PLL VCO frequency multiplier, for 125MHz
#endif
#if     FREQ_SYS != 31250000
#ifndef FREQ_SYS_DIV
#define FREQ_SYS_DIV        3                         // PLL system frequency divisor
#endif
#endif
#endif

#if     FREQ_SYS == 37500000 || FREQ_SYS == 50000000  // 37.5MHz, 50MHz
#ifndef FREQ_OSC_DIV
#define FREQ_OSC_DIV        2                         // PLL input frequency divisor, for 15MHz/6MHz
#endif
#ifndef FREQ_VCO_MUL
#define FREQ_VCO_MUL        (FREQ_OSC_30M?10:25)      // PLL VCO frequency multiplier, for 150MHz
#endif
#endif

#if     FREQ_SYS == 62500000 || FREQ_SYS >= 83000000 && FREQ_SYS <= 84000000 || FREQ_SYS == 125000000 // 62.5MHz, 83.33MHz, 125MHz
#ifndef FREQ_OSC_DIV
#define FREQ_OSC_DIV        3                         // PLL input frequency divisor, for 10MHz
#endif
#ifndef FREQ_VCO_MUL
#define FREQ_VCO_MUL        (FREQ_OSC_30M?25:0)       // PLL VCO frequency multiplier, for 250MHz
#endif
#if     FREQ_SYS != 62500000 && FREQ_SYS != 125000000
#ifndef FREQ_SYS_DIV
#define FREQ_SYS_DIV        3                         // PLL system frequency divisor
#endif
#endif
#endif

#if     FREQ_SYS == 75000000 || FREQ_SYS == 100000000 || FREQ_SYS == 150000000 // 75MHz, 100MHz, 150MHz
#ifndef FREQ_OSC_DIV
#define FREQ_OSC_DIV        2                         // PLL input frequency divisor, for 15MHz/6MHz
#endif
#ifndef FREQ_VCO_MUL
#define FREQ_VCO_MUL        (FREQ_OSC_30M?20:50)      // PLL VCO frequency multiplier, for 300MHz
#endif
#endif

#if     FREQ_SYS == 68750000 || FREQ_SYS >= 91000000 && FREQ_SYS <= 92000000 || FREQ_SYS == 137500000 // 68.75MHz, 91.67MHz, 137.5MHz
#ifndef FREQ_OSC_DIV
#define FREQ_OSC_DIV        6                         // PLL input frequency divisor, for 5MHz
#endif
#ifndef FREQ_VCO_MUL
#define FREQ_VCO_MUL        (FREQ_OSC_30M?55:0)       // PLL VCO frequency multiplier, for 275MHz
#endif
#if     FREQ_SYS != 68750000 && FREQ_SYS != 137500000
#ifndef FREQ_SYS_DIV
#define FREQ_SYS_DIV        3                         // PLL system frequency divisor
#endif
#endif
#endif

#if     FREQ_SYS == 56250000 || FREQ_SYS == 112500000 // 56.25MHz, 112.5MHz
#ifndef FREQ_OSC_DIV
#define FREQ_OSC_DIV        2                         // PLL input frequency divisor, for 15MHz
#endif
#ifndef FREQ_VCO_MUL
#define FREQ_VCO_MUL        (FREQ_OSC_30M?15:0)       // PLL VCO frequency multiplier, for 225MHz
#endif
#endif

#if     FREQ_SYS == 87500000 || FREQ_SYS >= 116000000 && FREQ_SYS <= 117000000 // 87.5MHz, 116.67MHz
#ifndef FREQ_OSC_DIV
#define FREQ_OSC_DIV        3                         // PLL input frequency divisor, for 10MHz
#endif
#ifndef FREQ_VCO_MUL
#define FREQ_VCO_MUL        (FREQ_OSC_30M?35:0)       // PLL VCO frequency multiplier, for 350MHz
#endif
#if     FREQ_SYS != 87500000
#ifndef FREQ_SYS_DIV
#define FREQ_SYS_DIV        3                         // PLL system frequency divisor
#endif
#endif
#endif

#if     FREQ_SYS == 82500000 || FREQ_SYS == 110000000 // 82.5MHz, 110MHz
#ifndef FREQ_OSC_DIV
#define FREQ_OSC_DIV        (FREQ_OSC_30M?3:2)        // PLL input frequency divisor, for 10MHz/6MHz
#endif
#ifndef FREQ_VCO_MUL
#define FREQ_VCO_MUL        (FREQ_OSC_30M?33:55)      // PLL VCO frequency multiplier, for 330MHz
#endif
#endif

#if     FREQ_VCO_MUL == 0
ERROR_"PLL not supported, OSC_VCO"
#endif

#ifndef FREQ_VCO
#define FREQ_VCO            (FREQ_OSC/FREQ_OSC_DIV*FREQ_VCO_MUL) // PLL VCO frequency, from 100MHz to 300MHz, a multiple of 25MHz if used ethernet and EN_ETH_XT=0
#endif

#ifndef FREQ_SYS_DIV
#define FREQ_SYS_DIV        (FREQ_VCO/FREQ_SYS) // PLL system frequency divisor
#endif

#if     FREQ_OSC_DIV >= 8 || FREQ_OSC/FREQ_OSC_DIV < 3000000 || FREQ_OSC/FREQ_OSC_DIV > 30000000
ERROR_"PLL not supported, OSC_DIV"
#endif

#if     FREQ_VCO_MUL >= 64 || FREQ_VCO < 100000000 || FREQ_VCO > 360000000
ERROR_"PLL not supported, VCO_MUL"
#endif

#if     FREQ_SYS_DIV > 4 || FREQ_SYS < 20000000 || FREQ_SYS > 150000000
ERROR_"PLL not supported, SYS_FREQ"
#endif

#ifdef __cplusplus
}
#endif

#endif
