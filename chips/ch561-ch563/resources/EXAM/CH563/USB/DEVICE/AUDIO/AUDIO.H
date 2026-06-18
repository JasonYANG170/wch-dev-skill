/********************************** (C) COPYRIGHT *******************************
* File Name          : AUDIO.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 USB_Device Example
*******************************************************************************/

#ifndef __AUDIO_H__
#define __AUDIO_H__

void Star_PWM_DMA( void );
/*类请求*/
#define    DEF_USB_SET_CUR            0x01
#define    DEF_USB_SET_UnK            0x0a
#define    DEF_USB_GET_CUR            0x81
#define    DEF_USB_GET_MIN            0x82
#define    DEF_USB_GET_MAX            0x83
#define    DEF_USB_GET_RES            0x84
/*类请求类型*/
#define    DEF_USB_CLA_FU1            0x02
#define    DEF_USB_CLA_FU2            0x05
#define    DEF_USB_CLA_FU3            0x07
#define    DEF_USB_CLA_MU             0x08
#define    DEF_USB_CLA_SU             0x09
/*类请求FU类型*/
#define    DEF_USB_CLA_MUT            0x01
#define    DEF_USB_CLA_VOL            0x02
#define    DEF_USB_CLA_BAS            0x04
#define    DEF_USB_CLA_MID            0x08
#define    DEF_USB_CLA_TRE            0x10
#define    DEF_USB_CLA_GEQ            0x20
#define    DEF_USB_CLA_AGA            0x40
#define    DEF_USB_CLA_DLY            0x80

typedef struct __USB_AUDIO_PWM 
{
    UINT8  RepeatTim;
    UINT32 DatBuf[64];
    UINT32 SamplRate;
    UINT32 DatLen;
    UINT32 LenCount;
    UINT32 DutyCycle;
    UINT32 EndCount;
}AUDIO_PWM;

extern AUDIO_PWM pwm;
#endif
