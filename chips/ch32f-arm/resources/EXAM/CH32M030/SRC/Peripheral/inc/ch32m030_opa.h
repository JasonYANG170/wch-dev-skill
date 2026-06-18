/********************************** (C) COPYRIGHT  *******************************
 * File Name          : ch32m030_opa.h
 * Author             : WCH
 * Version            : V1.0.2
 * Date               : 2025/01/13
 * Description        : This file contains all the functions prototypes for the
 *                      OPA firmware library.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#ifndef __CH32M030_OPA_H
#define __CH32M030_OPA_H

#ifdef __cplusplus
extern "C" {
#endif

#include "ch32m030.h"

/* CMP1_QII1_HYPSEL_enumeration */
typedef enum
{
    CMP1_QII1_HYPSEL_100mv = 0,
    CMP1_QII1_HYPSEL_200mv,
} CMP1_QII1_HYPSEL_TypeDef;

/* OPA1_QII1_AVSEL_enumeration */
typedef enum
{
    OPA1_QII1_AVSEL_20 = 0,
    OPA1_QII1_AVSEL_40,
} OPA1_QII1_AVSEL_TypeDef;

/* OPA_QII2_SESEL_MODE_enumeration */
typedef enum
{
    OPA2_QII2_SESEL_MODE_QII = 0,
    OPA2_QII2_SESEL_MODE_Amplification,
} OPA2_QII2_SESEL_MODE_TypeDef;

/* OPA2_QII2_AVSEL_enumeration */
typedef enum
{
    OPA2_QII2_AVSEL_5 = 0,
    OPA2_QII2_AVSEL_10,
    OPA2_QII2_AVSEL_20,
    OPA2_QII2_AVSEL_40
} OPA2_QII2_AVSEL_TypeDef;

/* CMP2_QII2_HYPSEL_enumeration */
typedef enum
{
    CMP2_QII2_HYPSEL_0mv = 0,
    CMP2_QII2_HYPSEL_10mv,
    CMP2_QII2_HYPSEL_20mv,
    CMP2_QII2_HYPSEL_40mv,
    CMP2_QII2_HYPSEL_60mv,
    CMP2_QII2_HYPSEL_80mv,
    CMP2_QII2_HYPSEL_100mv,
    CMP2_QII2_HYPSEL_200mv
} CMP2_QII2_HYPSEL_TypeDef;

/* CMP2_QII2_DACEN_enumeration */
typedef enum
{
    CMP2_QII2_DACEN_OFF = 0,
    CMP2_QII2_DACEN_ON
} CMP2_QII2_DACEN_TypeDef;

/* CMP2_QII2_CHNSEL_enumeration */
typedef enum
{
    CMP2_QII2_CHNSEL_PB6 = 0,
    CMP2_QII2_CHNSEL_VB,
    CMP2_QII2_CHNSEL_DAC1,
    CMP2_QII2_CHNSEL_VSS
} CMP2_QII2_CHNSEL_TypeDef;

/* CMP2_QII2_CHPSEL_enumeration */
typedef enum
{
    CMP2_QII2_CHPSEL_PB5 = 0,
    CMP2_QII2_CHPSEL_OPA2_Output,
    CMP2_QII2_CHPSEL_ISP1_Output,
    CMP2_QII2_CHPSEL_VSS
} CMP2_QII2_CHPSEL_TypeDef;

/* OPA1_Init Structure definition */
typedef struct
{
    OPA1_QII1_AVSEL_TypeDef       QII1_AVSEL;        /* Selection of operational amplifier gain multiples for OPA1 */
} OPA1_InitTypeDef;

/* OPA2_Init Structure definition */
typedef struct
{
    OPA2_QII2_SESEL_MODE_TypeDef  QII2_SESEL;         /* Selection of operation mode for OPA2 */
    OPA2_QII2_AVSEL_TypeDef       QII2_AVSEL;         /* Selection of operational amplifier gain multiples for OPA2 */
}OPA2_InitTypeDef;

/* OPA_ISP_Num_enumeration */
typedef enum
{
    OPA3_ISP1 = 0,
    OPA4_ISP2
} OPA_ISP_Num_TypeDef;

/* OPA_ISP_GAIN_SEL_enumeration */
typedef enum
{
    OPA_ISP_GAIN_4 = 0,
    OPA_ISP_GAIN_8,
    OPA_ISP_GAIN_16,
    OPA_ISP_GAIN_55
}OPA_ISP_GAIN_SEL_TypeDef;

/* OPA_ISP1_VFBIAS_SEL_enumeration */
typedef enum
{
    OPA_ISP_VFBIAS_0_55V = 0,
    OPA_ISP_VFBIAS_1_6V
} OPA_ISP_VFBIAS_SEL_TypeDef;

/* OPA_ISP_NESL_SEL_enumeration */
typedef enum
{
    OPA_ISP_NESL_ISN = 0,
    OPA_ISP_NESL_VSS
} OPA_ISP_NESL_SEL_TypeDef;

/* OPA_ISP_SEL_IO_enumeration */
typedef enum
{
    OPA_ISP_SEL_TO_ADC_OFF = 0,
    OPA_ISP_SEL_TO_ADC_ON
} OPA_ISP_SEL_IO_TypeDef;

/* OPA_ISP_QDET_EN_enumeration */
typedef enum
{
    OPA_ISP_QDET_EN_OFF = 0,
    OPA_ISP_QDET_EN_ON
} OPA_ISP_QDET_EN_TypeDef;

/* OPA_ISP_QDET_PD30K_enumeration */
typedef enum
{
    OPA_ISP_QDET_PD30K_OFF = 0,
    OPA_ISP_QDET_PD30K_ON
} OPA_ISP_QDET_PD30K_TypeDef;

/* OPA_ISP_QDET_VBSEL_enumeration */
typedef enum
{
    OPA_ISP_QDET_VBSEL_OFF = 0,
    OPA_ISP_QDET_VBSEL_ON
} OPA_ISP_QDET_VBSEL_TypeDef;

/* OPA_ISP_Init Structure definition */
typedef struct
{
    OPA_ISP_GAIN_SEL_TypeDef      OPA_ISP_GAIN;       /* Selection of operational amplifier gain multiples for OPA3 or OPA4 */
    OPA_ISP_VFBIAS_SEL_TypeDef    OPA_ISP_VFBIAS;     /* Selection of reference voltage for OPA3 or OPA4 */
    OPA_ISP_NESL_SEL_TypeDef      OPA_ISP_NESL;       /* Selection of negative terminal for OPA3 or OPA4*/
    OPA_ISP_SEL_IO_TypeDef        OPA_ISP_SEL_IO;     /* Selection of output to ADC for OPA3 or OPA4*/
    OPA_ISP_QDET_EN_TypeDef       OPA_ISP_QDET;       /* Enable Q value detection of OPA3 or OPA4*/
    OPA_ISP_QDET_PD30K_TypeDef    OPA_ISP_QDET_PD30K; /* Enable Q-value detection with 30K resistor for OPA3 or OPA4*/
    OPA_ISP_QDET_VBSEL_TypeDef    OPA_ISP_QDET_VBSEL; /* Enable Q-value detection reference voltage for OPA3 or OPA4*/

} OPA_ISP_InitTypeDef;

/* CMP1_FILT_EN_enumeration */
typedef enum
{
    CMP1_FILT_EN_OFF = 0,
    CMP1_FILT_EN_ON
} CMP1_FILT_EN_TypeDef;

/* CMP2_FILT_EN_enumeration */
typedef enum
{
    CMP2_FILT_EN_OFF = 0,
    CMP2_FILT_EN_ON
} CMP2_FILT_EN_TypeDef;

/* CMP2_BK_EN_enumeration */
typedef enum
{
    CMP2_BK_EN_TIM1BKIN_OFF = 0,
    CMP2_BK_EN_TIM1BKIN_ON
} CMP2_BK_EN_TypeDef;

/* CMP2_INT_EN_enumeration */
typedef enum
{
    CMP2_INT_EN_OFF = 0,
    CMP2_INT_EN_ON
} CMP2_INT_EN_TypeDef;

/* CMP3_TRG_SRC_SEL_enumeration */
typedef enum
{
    CMP3_TRG_SRC_T1_CC4 = 0,
    CMP3_TRG_SRC_T1_CC5,
    CMP3_TRG_SRC_T2_CC1,
    CMP3_TRG_SRC_T2_CC2,
    CMP3_TRG_SRC_T2_CC3,
    CMP3_TRG_SRC_T2_CC4,
    CMP3_TRG_SRC_T3_CC1,
    CMP3_TRG_SRC_T3_CC2,
} CMP3_TRG_SRC_SEL_TypeDef;

/* CMP3_TRG_GATE_enumeration */
typedef enum
{
    CMP3_TRG_GATE_ON = 0,
    CMP3_TRG_GATE_OFF
} CMP3_TRG_GATE_TypeDef;

/* CMP3_COM_MODE_enumeration */
typedef enum
{
    CMP3_COM_MODE_OFF = 0,
    CMP3_COM_MODE_ON
} CMP3_COM_MODE_TypeDef;

/* CMP3_FILT_EN_enumeration */
typedef enum
{
    CMP3_FILT_EN_OFF = 0,
    CMP3_FILT_EN_ON
} CMP3_FILT_EN_TypeDef;

/* CMP3_BK_EN_enumeration */
typedef enum
{
    CMP3_BK_EN_TIM1BKIN_OFF = 0,
    CMP3_BK_EN_TIM1BKIN_ON
} CMP3_BK_EN_TypeDef;

/* CMP3_INT_EN_enumeration */
typedef enum
{
    CMP3_INT_EN_OFF = 0,
    CMP3_INT_EN_ON
} CMP3_INT_EN_TypeDef;

/* CMP2_PT_IO_enumeration */
typedef enum
{
    CMP2_PT_IO_PA13_OFF = 0,
    CMP2_PT_IO_PA13_ON
} CMP2_PT_IO_TypeDef;

/* CMP1_PT_IO_enumeration */
typedef enum
{
    CMP1_PT_IO_PA12_OFF = 0,
    CMP1_PT_IO_PA12_ON
} CMP1_PT_IO_TypeDef;

/* CMP3_CAP_EN_enumeration */
typedef enum
{
    CMP3_CAP_EN_T2_Channels_OFF = 0,
    CMP3_CAP_EN_T2_Channels_ON
} CMP3_CAP_EN_TypeDef;

/* CMP1_Init Structure definition */
typedef struct
{
    CMP1_QII1_HYPSEL_TypeDef      QII1_HYPSEL;        /* Selection of Hysteresis for CMP1 */
    CMP1_FILT_EN_TypeDef          CMP1_FILT_EN;       /* Enable the output digital filtering for CMP1 */
    uint8_t                       CMP1_FILT_CFG;      /* CMP1 output digital filter length configuration
                                                         The CMP1_FILT_CFG value should set 0x0~0x1F  */
    CMP1_PT_IO_TypeDef            CMP1_PT_IO;         /* Selection of CMP1 push-pull output to IO */
} CMP1_InitTypeDef;

/* CMP2_Init Structure definition */
typedef struct
{
    CMP2_QII2_HYPSEL_TypeDef      QII2_HYPSEL;        /* Selection of Hysteresis for CMP2 */
    CMP2_QII2_CHNSEL_TypeDef      QII2_CHNSEL;        /* Selection of negative terminal for CMP2 */
    CMP2_QII2_CHPSEL_TypeDef      QII2_CHPSEL;        /* Selection of positive terminal for CMP2 */
    CMP2_FILT_EN_TypeDef          CMP2_FILT_EN;       /* Enable the output digital filtering for CMP2 */
    uint8_t                       CMP2_FILT_CFG;      /* CMP2 output digital filter length configuration
                                                         The CMP2_FILT_CFG value should set 0x0~0x1F*/
    CMP2_QII2_DACEN_TypeDef       CMP2_QII2_DACEN;    /* Enable DAC output */
    uint8_t                       CMP2_QII2_DAC;      /* Set the analog value of DAC output
                                                         The CMP2_QII2_DAC value should set 0x0~0xF*/

    CMP2_PT_IO_TypeDef            CMP2_PT_IO;         /* Enable CMP2 push-pull to IO*/
    CMP2_BK_EN_TypeDef            CMP2_BK_EN;         /* Enable CMP2 output to TIM1 brake input */
    CMP2_INT_EN_TypeDef           CMP2_INT_EN;        /* Enable CMP2 interrupt*/

} CMP2_InitTypeDef;

/* CMP3_EN_enumeration */
typedef enum
{
    CMP3_EN_OFF = 0,
    CMP3_EN_ON
} CMP3_EN_TypeDef;

/* CMP3_PSEL_enumeration */
typedef enum
{
    CMP3_PSEL_PA3 = 0,
    CMP3_PSEL_PB0,
    CMP3_PSEL_PB1,
    CMP3_PSEL_PB3,
    CMP3_PSEL_ISP1,
    CMP3_PSEL_ISP2
} CMP3_PSEL_TypeDef;

/* CMP3_NSEL_enumeration */
typedef enum
{
    CMP3_NSEL_PA5 = 0,
    CMP3_NSEL_PA6,
    CMP3_NSEL_PA7,
    CMP3_NSEL_PB2,
    CMP3_NSEL_DAC,
} CMP3_NSEL_TypeDef;

/* CMP3_HYS_enumeration */
typedef enum
{
    CMP3_HYS_0mv,
    CMP3_HYS_10mv,
    CMP3_HYS_20mv,
    CMP3_HYS_40mv
} CMP3_HYS_TypeDef;

/* CMP3_RMID_EN_enumeration */
typedef enum
{
    CMP3_RMID_EN_OFF,
    CMP3_RMID_EN_ON
} CMP3_RMID_EN_TypeDef;

/* CMP3_Analog_output_to_IO_enumeration */
typedef enum
{
    CMP3_AT_IO_OFF = 0,
    CMP3_AT_IO_ON
} CMP3_AT_IO_TypeDef;

/* CMP3_Push_Pull_Output_to_IO_enumeration */
typedef enum
{
    CMP3_PT_IO_OFF = 0,
    CMP3_PT_IO_ON
} CMP3_PT_IO_TypeDef;

/* CMP3_Push_Pull_Output_IO_Selection_enumeration */
typedef enum
{
    CMP3_PT_IO_PB4 = 0,
    CMP3_PT_IO_PB5
} CMP3_PT_IO_SEL_TypeDef;

/* CMP3_CH_SW_NUM_enumeration */
typedef enum
{
    CMP3_CH_SW_NUM_0 = 0,
    CMP3_CH_SW_NUM_2,
    CMP3_CH_SW_NUM_3,
    CMP3_CH_SW_NUM_4
} CMP3_CH_SW_NUM_TypeDef;

/* CMP3_OUT_CH1_POL_enumeration */
typedef enum
{
    CMP3_OUT_CH1_POL_HIGH = 0,
    CMP3_OUT_CH1_POL_LOW
} CMP3_OUT_CH1_POL_TypeDef;

/* CMP3_OUT_CH2_POL_enumeration */
typedef enum
{
    CMP3_OUT_CH2_POL_HIGH = 0,
    CMP3_OUT_CH2_POL_LOW
} CMP3_OUT_CH2_POL_TypeDef;

/* CMP3_OUT_CH3_POL_enumeration */
typedef enum
{
    CMP3_OUT_CH3_POL_HIGH = 0,
    CMP3_OUT_CH3_POL_LOW
} CMP3_OUT_CH3_POL_TypeDef;

/* CMP3_OUT_CH1_POL_enumeration */
typedef enum
{
    CMP3_OUT_CH4_POL_HIGH = 0,
    CMP3_OUT_CH4_POL_LOW
} CMP3_OUT_CH4_POL_TypeDef;

/* CMP3_Init Structure definition */
typedef struct
{
    CMP3_TRG_SRC_SEL_TypeDef      CMP3_TRG_SRC;        /* Selection of the trigger source for CMP3 */
    CMP3_TRG_GATE_TypeDef         CMP3_TRG_GATE;       /* Enable hardware triggered channel switching */
    CMP3_COM_MODE_TypeDef         CMP3_COM_EN;         /* Enable the continuous compare function for CMP3*/
    CMP3_FILT_EN_TypeDef          CMP3_FILT_EN;        /* Enable the output digital filtering for CMP3 */
    uint8_t                       CMP3_FILT_CFG;       /* CMP3 output digital filter length configuration
                                                          The CMP3_FILT_CFG value should set 0x0~0x1F*/
    CMP3_BK_EN_TypeDef            CMP3_BK_EN;          /* Enable CMP3 output to TIM1 brake input */
    CMP3_INT_EN_TypeDef           CMP3_INT_EN;         /* Enable CMP3 interrupt*/
    CMP3_CAP_EN_TypeDef           CMP3_CAP;            /* Enable CMP3 compare output to TIM2 channels*/

    CMP3_PSEL_TypeDef             CMP3_PSEL;           /* Selection of positive terminal for CMP3 */
    CMP3_NSEL_TypeDef             CMP3_NSEL;           /* Selection of negative terminal for CMP3 */
    CMP3_HYS_TypeDef              CMP3_HYS;            /* Selection of Hysteresis voltage */
    CMP3_RMID_EN_TypeDef          CMP3_RMID;           /* Enable the virtual center point for CMP3*/
    uint8_t                       CMP3_DACDAT;         /* Set the analog value of DAC output
                                                          The CMP3_DACDAT value should set 0x0~0xF*/
    CMP3_AT_IO_TypeDef            CMP3_AT_IO;          /* Enable CMP3 analog output to IO */
    CMP3_PT_IO_TypeDef            CMP3_PT_IO;          /* Enable CMP3 push-pull output to IO */
    CMP3_PT_IO_SEL_TypeDef        CMP3_PT_IO_SEL;      /* Selection of the IO for the CMP3 push-pull output */

    CMP3_CH_SW_NUM_TypeDef        CMP3_CH_SW_NUM;      /* Selection the number of channel switching*/
    CMP3_OUT_CH1_POL_TypeDef      CMP3_OUT_CH1_POL;    /* Comparison output polarity configuration bit for switching the first channel of CMP3 channel*/
    CMP3_OUT_CH2_POL_TypeDef      CMP3_OUT_CH2_POL;    /* Comparison output polarity configuration bit for switching the first channel of CMP3 channe2*/
    CMP3_OUT_CH3_POL_TypeDef      CMP3_OUT_CH3_POL;    /* Comparison output polarity configuration bit for switching the first channel of CMP3 channe3*/
    CMP3_OUT_CH4_POL_TypeDef      CMP3_OUT_CH4_POL;    /* Comparison output polarity configuration bit for switching the first channel of CMP3 channe4*/

} CMP3_InitTypeDef;

/*CMP output definition*/
#define CMP1_OUT                  (uint32_t)0x00000001
#define CMP1_OUT_FILT             (uint32_t)0x00000002
#define CMP2_OUT                  (uint32_t)0x00000100
#define CMP2_OUT_FILT             (uint32_t)0x00000202
#define CMP3_OUT                  (uint32_t)0x00010000
#define CMP3_OUT_FILT             (uint32_t)0x00020000

/*CMP_FLAG definition*/
#define CMP2_FLAG_OUTHIGH         (uint32_t)0x00000400

#define CMP3_FLAG_CHOUT0          (uint32_t)0x00040000
#define CMP3_FLAG_CHOUT1          (uint32_t)0x00080000
#define CMP3_FLAG_CHOUT2          (uint32_t)0x00100000
#define CMP3_FLAG_CHOUT3          (uint32_t)0x00200000

void         QII1_AE_Cmd(FunctionalState NewState);
void         QII2_AE_Cmd(FunctionalState NewState);
void         QII2_OPA2_Cmd(FunctionalState NewState);
void         CMP3_Cmd(FunctionalState NewState);
void         OPA_ISP_Cmd(OPA_ISP_Num_TypeDef ISPx,FunctionalState NewState);
void         OPA_ISP_Init(OPA_ISP_Num_TypeDef ISPx,OPA_ISP_InitTypeDef *OPA_ISP_InitStruct);
void         OPA_ISP_StructInit(OPA_ISP_InitTypeDef *OPA_ISP_InitStruct);
void         OPA1_Init(OPA1_InitTypeDef *OPA1_InitStruct);
void         OPA1_StructInit(OPA1_InitTypeDef *OPA1_InitStruct);
void         OPA2_Init(OPA2_InitTypeDef *OPA2_InitStruct);
void         OPA2_StructInit(OPA2_InitTypeDef *OPA2_InitStruct);
void         CMP1_Init(CMP1_InitTypeDef *CMP1_InitStruct);
void         CMP1_StructInit(CMP1_InitTypeDef *CMP1_InitStruct);
void         CMP2_Init(CMP2_InitTypeDef *CMP2_InitStruct);
void         CMP2_StructInit(CMP2_InitTypeDef *CMP2_InitStruct);
void         CMP3_Init(CMP3_InitTypeDef *CMP3_InitStruct);
void         CMP3_StructInit(CMP3_InitTypeDef *CMP3_InitStruct);
FlagStatus   CMP_GetOutStatus(uint32_t CMP_OUT);
FlagStatus   CMP_GetFlagStatus(uint32_t CMP_FLAG);
void         CMP_ClearFlag( uint32_t CMP_FLAG);
void         QII_OutToTIM3Cap_Cmd(FunctionalState NewState);
void         ISP_OutToADC_CMD(OPA_ISP_Num_TypeDef ISPx,FunctionalState NewState);
void         ISP_QDET_CMD(OPA_ISP_Num_TypeDef ISPx,FunctionalState NewState);

#ifdef __cplusplus
}
#endif

#endif





















