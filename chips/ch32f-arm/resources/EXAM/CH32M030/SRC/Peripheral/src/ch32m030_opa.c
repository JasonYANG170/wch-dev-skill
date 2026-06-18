/********************************** (C) COPYRIGHT  *******************************
 * File Name          : ch32m030_opa.c
 * Author             : WCH
 * Version            : V1.0.2
 * Date               : 2025/01/07
 * Description        : This file provides all the OPA firmware functions.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "ch32m030_opa.h"


/*********************************************************************
 * @fn      QII1_AE_Cmd
 *
 * @brief   Enables or disables the specified OPA1 and CMP1 peripheral.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void QII1_AE_Cmd(FunctionalState NewState)
{
    if(NewState == ENABLE)
    {
        OPA->QII_CFGR |= (uint32_t)(1 << 0);
    }
    else
    {
        OPA->QII_CFGR &= ~(uint32_t)(1 << 0);
    }
}

/*********************************************************************
 * @fn      QII2_AE_Cmd
 *
 * @brief   Enables or disables the specified CMP2 peripheral.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void QII2_AE_Cmd(FunctionalState NewState)
{
    if(NewState == ENABLE)
    {
        OPA->QII_CFGR |= (uint32_t)(1 << 3);
    }
    else
    {
        OPA->QII_CFGR &= ~(uint32_t)(1 << 3);
    }
}

/*********************************************************************
 * @fn      QII2_OPA2_Cmd
 *
 * @brief   Enables or disables the specified OPA2 peripheral.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void QII2_OPA2_Cmd(FunctionalState NewState)
{
    if(NewState == ENABLE)
    {
        OPA->QII_CFGR |= (uint32_t)(1 << 4);
    }
    else
    {
        OPA->QII_CFGR &= ~(uint32_t)(1 << 4);
    }
}

/*********************************************************************
 * @fn      CMP3_Cmd
 *
 * @brief   Enables or disables the specified CMP3 peripheral.
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void CMP3_Cmd(FunctionalState NewState)
{
    if(NewState == ENABLE)
    {
        OPA->CMP3_CFGR |= (uint32_t)(1 << 0);
    }
    else
    {
        OPA->CMP3_CFGR &= ~(uint32_t)(1 << 0);
    }
}

/*********************************************************************
 * @fn      OPA_ISP_Cmd
 *
 * @brief   Enables or disables the specified OPA3_ISP1 or OPA4_ISP2 peripheral.
 *
 * @param   ISPx - the specified OPA3_ISP1 or OPA4_ISP2.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void OPA_ISP_Cmd(OPA_ISP_Num_TypeDef ISPx,FunctionalState NewState)
{
    if(NewState == ENABLE)
    {
        OPA->ISP_CTLR |= (uint32_t)(1 << (0+16*ISPx));
    }
    else
    {
        OPA->ISP_CTLR &= ~(uint32_t)(1 << (0+16*ISPx));
    }
}

/*********************************************************************
 * @fn      OPA_ISP_Init
 *
 * @brief   Initializes the OPA3 or OPA4 peripheral according to the specified
 *        parameters in the OPA_ISP_InitStruct.
 *
 * @param   ISPx - the specified OPA3 or OPA4.
 *          OPA_ISP_InitStruct - pointer to a OPA_ISP_InitTypeDef structure
 *
 * @return  none
 */
void OPA_ISP_Init(OPA_ISP_Num_TypeDef ISPx,OPA_ISP_InitTypeDef *OPA_ISP_InitStruct)
{
    uint32_t tmp0 = 0;
    uint32_t tmpmask = 0x1FE;
    uint8_t NUM = 0;
    tmp0 = OPA->ISP_CTLR;

    if(ISPx == OPA3_ISP1)
    {
        tmp0 &= (~tmpmask);
    }
    else if(ISPx == OPA4_ISP2)
    {
        tmp0 &= (~(tmpmask << 16));
    }
    NUM = ISPx;
    tmp0 |= (OPA_ISP_InitStruct->OPA_ISP_GAIN)<< (1+16*NUM)\
         | (OPA_ISP_InitStruct->OPA_ISP_VFBIAS)<< (3+16*NUM) | (OPA_ISP_InitStruct->OPA_ISP_NESL)<< (4+16*NUM)\
         | (OPA_ISP_InitStruct->OPA_ISP_SEL_IO)<< (5+16*NUM) | (OPA_ISP_InitStruct->OPA_ISP_QDET)<< (6+16*NUM)\
         | (OPA_ISP_InitStruct->OPA_ISP_QDET_PD30K)<< (7+16*NUM)\
         | (OPA_ISP_InitStruct->OPA_ISP_QDET_VBSEL)<< (8+16*NUM);

    OPA->ISP_CTLR = tmp0;
}

/*********************************************************************
 * @fn      OPA_ISP_StructInit
 *
 * @brief   Fills each OPA_ISP_InitStruct member with its default value.
 *
 * @param   OPA_ISP_InitStruct - pointer to an OPA_ISP_InitTypeDef structure that
 *        contains the configuration information for the specified OPA
 *        peripheral.
 *
 * @return  none
 */
void OPA_ISP_StructInit(OPA_ISP_InitTypeDef *OPA_ISP_InitStruct)
{
    OPA_ISP_InitStruct->OPA_ISP_GAIN = OPA_ISP_GAIN_4;
    OPA_ISP_InitStruct->OPA_ISP_VFBIAS = OPA_ISP_VFBIAS_0_55V;
    OPA_ISP_InitStruct->OPA_ISP_NESL = OPA_ISP_NESL_VSS;
    OPA_ISP_InitStruct->OPA_ISP_SEL_IO = OPA_ISP_SEL_TO_ADC_OFF;
    OPA_ISP_InitStruct->OPA_ISP_QDET =OPA_ISP_QDET_EN_OFF;
    OPA_ISP_InitStruct->OPA_ISP_QDET_PD30K = OPA_ISP_QDET_PD30K_OFF;
    OPA_ISP_InitStruct->OPA_ISP_QDET_VBSEL = OPA_ISP_QDET_VBSEL_OFF;
}

/*********************************************************************
 * @fn      OPA1_Init
 *
 * @brief   Initializes the OPA1 peripheral according to the specified
 *        parameters in the OPA1_InitStruct.
 * @param   OPA1_InitStruct - pointer to a OPA1_InitTypeDef structure.
 *
 * @return  none
 */
void OPA1_Init(OPA1_InitTypeDef *OPA1_InitStruct)
{
    uint32_t tmp0 = 0;

    tmp0 = OPA->QII_CFGR;

    tmp0 &= 0xFFFFFFFB;

    tmp0 |= (OPA1_InitStruct->QII1_AVSEL << 2);

    OPA->QII_CFGR = tmp0;
}

/*********************************************************************
 * @fn      OPA1_StructInit
 *
 * @brief   Fills each OPA1_InitStruct member with its default value.
 *
 * @param   OPA1_InitStruct - pointer to an OPA1_InitStruct structure that
 *        contains the configuration information for the specified OPA
 *        peripheral.
 *
 * @return  none
 */
void OPA1_StructInit(OPA1_InitTypeDef *OPA1_InitStruct)
{
    OPA1_InitStruct->QII1_AVSEL = OPA1_QII1_AVSEL_20;
}

/*********************************************************************
 * @fn      OPA2_Init
 *
 * @brief   Initializes the OPA2 peripheral according to the specified
 *        parameters in the OPA2_InitStruct.
 * @param   OPA2_InitStruct - pointer to a OPA2_InitTypeDef structure.
 *
 * @return  none
 */
void OPA2_Init(OPA2_InitTypeDef *OPA2_InitStruct)
{
    uint32_t tmp0 = 0;

    tmp0 = OPA->QII_CFGR;

    tmp0 &= 0xFFFFFF1F;

    tmp0 |= (OPA2_InitStruct->QII2_SESEL << 5) | (OPA2_InitStruct->QII2_AVSEL << 6);

    OPA->QII_CFGR = tmp0;
}

/*********************************************************************
 * @fn      OPA2_StructInit
 *
 * @brief   Fills each OPA2_InitStruct member with its default value.
 *
 * @param   OPA2_InitStruct - pointer to an OPA2_InitStruct structure that
 *        contains the configuration information for the specified OPA
 *        peripheral.
 *
 * @return  none
 */
void OPA2_StructInit(OPA2_InitTypeDef *OPA2_InitStruct)
{
    OPA2_InitStruct->QII2_SESEL = OPA2_QII2_SESEL_MODE_QII;
    OPA2_InitStruct->QII2_AVSEL = OPA2_QII2_AVSEL_5;
}

/*********************************************************************
 * @fn      CMP1_Init
 *
 * @brief   Initializes the CMP1 peripheral according to the specified
 *        parameters in the CMP1_InitStruct.
 * @param   CMP1_InitStruct - pointer to a CMP1_InitTypeDef structure.
 *
 * @return  none
 */
void CMP1_Init(CMP1_InitTypeDef *CMP1_InitStruct)
{
    uint32_t tmp0 = 0,tmp1;

    tmp0 = OPA->QII_CFGR;
    tmp1 = OPA->CMP_CTLR;

    tmp0 &= 0xFFFFFFFD;
    tmp1 &= 0xDFFFFFC0;

    tmp0 |= (CMP1_InitStruct->QII1_HYPSEL << 1);
    tmp1 |= (CMP1_InitStruct->CMP1_FILT_EN << 0) | (CMP1_InitStruct->CMP1_FILT_CFG <<1)\
            | (CMP1_InitStruct->CMP1_PT_IO << 29);

    OPA->QII_CFGR = tmp0;
    OPA->CMP_CTLR = tmp1;
}

/*********************************************************************
 * @fn      CMP1_StructInit
 *
 * @brief   Fills each CMP1_StructInit member with its default value.
 *
 * @param   CMP1_StructInit - pointer to an CMP1_StructInit structure that
 *        contains the configuration information for the specified CMP
 *        peripheral.
 *
 * @return  none
 */
void CMP1_StructInit(CMP1_InitTypeDef *CMP1_InitStruct)
{
    CMP1_InitStruct->QII1_HYPSEL = CMP1_QII1_HYPSEL_100mv;
    CMP1_InitStruct->CMP1_FILT_EN = CMP1_FILT_EN_OFF;
    CMP1_InitStruct->CMP1_FILT_CFG = 0;
    CMP1_InitStruct->CMP1_PT_IO = CMP1_PT_IO_PA12_OFF;
}

/*********************************************************************
 * @fn      CMP2_Init
 *
 * @brief   Initializes the CMP2 peripheral according to the specified
 *        parameters in the CMP2_InitStruct.
 * @param   CMP2_InitStruct - pointer to a CMP2_InitTypeDef structure.
 *
 * @return  none
 */
void CMP2_Init(CMP2_InitTypeDef *CMP2_InitStruct)
{
    uint32_t tmp0 = 0,tmp1;

    tmp0 = OPA->QII_CFGR;
    tmp1 = OPA->CMP_CTLR;

    tmp0 &= 0xFFC208FF;
    tmp1 &= 0xEFFFC03F;

    tmp0 |= (CMP2_InitStruct->QII2_HYPSEL << 8 ) | (CMP2_InitStruct->CMP2_QII2_DACEN << 12)\
            | (CMP2_InitStruct->CMP2_QII2_DAC <<13) | (CMP2_InitStruct->QII2_CHNSEL << 18)\
            | (CMP2_InitStruct->QII2_CHPSEL << 20);

    tmp1 |= (CMP2_InitStruct->CMP2_FILT_EN << 6) | (CMP2_InitStruct->CMP2_FILT_CFG <<7)\
            | (CMP2_InitStruct->CMP2_BK_EN << 12)| (CMP2_InitStruct->CMP2_INT_EN << 13)\
            |(CMP2_InitStruct->CMP2_PT_IO << 28);

    OPA->QII_CFGR = tmp0;
    OPA->CMP_CTLR = tmp1;
}

/*********************************************************************
 * @fn      CMP2_StructInit
 *
 * @brief   Fills each CMP2_InitStruct member with its default value.
 *
 * @param   CMP2_InitStruct - pointer to an CMP2_InitStruct structure that
 *        contains the configuration information for the specified CMP
 *        peripheral.
 *
 * @return  none
 */
void CMP2_StructInit(CMP2_InitTypeDef *CMP2_InitStruct)
{
    CMP2_InitStruct->QII2_HYPSEL = CMP2_QII2_HYPSEL_0mv;
    CMP2_InitStruct->CMP2_QII2_DACEN = CMP2_QII2_DACEN_OFF;
    CMP2_InitStruct->CMP2_QII2_DAC = 0;
    CMP2_InitStruct->QII2_CHNSEL = CMP2_QII2_CHNSEL_PB6;
    CMP2_InitStruct->QII2_CHPSEL = CMP2_QII2_CHPSEL_PB5;
    CMP2_InitStruct->CMP2_FILT_EN = CMP2_FILT_EN_OFF;
    CMP2_InitStruct->CMP2_FILT_CFG = 0;
    CMP2_InitStruct->CMP2_BK_EN = CMP2_BK_EN_TIM1BKIN_OFF;
    CMP2_InitStruct->CMP2_INT_EN = CMP2_INT_EN_OFF;
    CMP2_InitStruct->CMP2_PT_IO = CMP2_PT_IO_PA13_OFF;
}

/*********************************************************************
 * @fn      CMP3_Init
 *
 * @brief   Initializes the CMP3 peripheral according to the specified
 *        parameters in the CMP3_InitStruct.
 * @param   CMP3_InitStruct - pointer to a CMP3_InitTypeDef structure.
 *
 * @return  none
 */
void CMP3_Init(CMP3_InitTypeDef *CMP3_InitStruct)
{
    uint32_t tmp0 = 0,tmp1,tmp2;

    tmp0 = OPA->CMP_CTLR;
    tmp1 = OPA->CMP3_CFGR;
    tmp2 = OPA->CMP3_SW;

    tmp0 &= 0xB0043FFF;
    tmp1 &= 0xFFFE0001;
    tmp2 &= 0xFFFFFFC0;


    tmp0 |= (CMP3_InitStruct->CMP3_TRG_SRC << 14) | (CMP3_InitStruct->CMP3_TRG_GATE << 17)\
            | (CMP3_InitStruct->CMP3_COM_EN << 19) | (CMP3_InitStruct->CMP3_FILT_EN << 20)\
            | (CMP3_InitStruct->CMP3_FILT_CFG <<21)\
            | (CMP3_InitStruct->CMP3_BK_EN <<26) | (CMP3_InitStruct->CMP3_INT_EN <<27)\
            | (CMP3_InitStruct->CMP3_CAP);

    tmp1 |= (CMP3_InitStruct->CMP3_PSEL << 1) | (CMP3_InitStruct->CMP3_NSEL << 4)\
            | (CMP3_InitStruct->CMP3_HYS << 7) | (CMP3_InitStruct->CMP3_RMID << 9)\
            | (CMP3_InitStruct->CMP3_DACDAT << 10) | (CMP3_InitStruct->CMP3_AT_IO << 14)\
            | (CMP3_InitStruct->CMP3_PT_IO << 15) | (CMP3_InitStruct->CMP3_PT_IO_SEL << 16);

    tmp2 |= (CMP3_InitStruct->CMP3_CH_SW_NUM << 0) | (CMP3_InitStruct->CMP3_OUT_CH1_POL << 2)\
            | (CMP3_InitStruct->CMP3_OUT_CH2_POL << 3) | (CMP3_InitStruct->CMP3_OUT_CH3_POL << 4)\
            | (CMP3_InitStruct->CMP3_OUT_CH4_POL << 5);

     OPA->CMP_CTLR = tmp0 ;
     OPA->CMP3_CFGR = tmp1 ;
     OPA->CMP3_SW = tmp2;
}

/*********************************************************************
 * @fn      CMP3_StructInit
 *
 * @brief   Fills each CMP3_InitStruct member with its default value.
 *
 * @param   CMP3_InitStruct - pointer to an CMP3_InitStruct structure that
 *        contains the configuration information for the specified CMP
 *        peripheral.
 *
 * @return  none
 */
void CMP3_StructInit(CMP3_InitTypeDef *CMP3_InitStruct)
{
    CMP3_InitStruct->CMP3_TRG_SRC = CMP3_TRG_SRC_T1_CC5;
    CMP3_InitStruct->CMP3_TRG_GATE = CMP3_TRG_GATE_ON;
    CMP3_InitStruct->CMP3_COM_EN = CMP3_COM_MODE_OFF;
    CMP3_InitStruct->CMP3_FILT_EN = CMP3_FILT_EN_OFF;
    CMP3_InitStruct->CMP3_FILT_CFG = 0;
    CMP3_InitStruct->CMP3_BK_EN = CMP3_BK_EN_TIM1BKIN_OFF;
    CMP3_InitStruct->CMP3_INT_EN = CMP3_INT_EN_OFF;
    CMP3_InitStruct->CMP3_CAP = CMP3_CAP_EN_T2_Channels_OFF;
    CMP3_InitStruct->CMP3_PSEL = CMP3_PSEL_PA3;
    CMP3_InitStruct->CMP3_NSEL = CMP3_NSEL_PA5;
    CMP3_InitStruct->CMP3_HYS = CMP3_HYS_0mv;
    CMP3_InitStruct->CMP3_RMID = CMP3_RMID_EN_OFF;
    CMP3_InitStruct->CMP3_DACDAT = 0;
    CMP3_InitStruct->CMP3_AT_IO = CMP3_AT_IO_OFF;
    CMP3_InitStruct->CMP3_PT_IO = CMP3_PT_IO_OFF;
    CMP3_InitStruct->CMP3_PT_IO_SEL = CMP3_PT_IO_PB4;
    CMP3_InitStruct->CMP3_CH_SW_NUM = CMP3_CH_SW_NUM_0;
    CMP3_InitStruct->CMP3_OUT_CH1_POL = CMP3_OUT_CH1_POL_HIGH;
    CMP3_InitStruct->CMP3_OUT_CH2_POL = CMP3_OUT_CH2_POL_HIGH;
    CMP3_InitStruct->CMP3_OUT_CH3_POL = CMP3_OUT_CH3_POL_HIGH;
    CMP3_InitStruct->CMP3_OUT_CH4_POL = CMP3_OUT_CH4_POL_HIGH;

}

/*********************************************************************
 * @fn      CMP_GetOutStatus
 *
 * @brief   Obtain the output value of the comparator.
 *
 * @param   CMP_OUT - Select CMP.
 *          CMP1_OUT - The value of CMP1 output with FILT OFF.
 *          CMP1_OUT_FILT - The value of CMP1 output with FILT ON.
 *          CMP2_OUT - The value of CMP2 output with FILT OFF.
 *          CMP2_OUT_FILT - The value of CMP2 output with FILT ON.
 *          CMP3_OUT -The value of CMP3 output with FILT OFF.
 *          CMP3_OUT_FILT - The value of CMP3 output with FILT ON.
 *
 * @return  FlagStatus: SET or RESET.
 */
FlagStatus CMP_GetOutStatus(uint32_t CMP_OUT)
{
    FlagStatus bitstatus = RESET;

    if((OPA->CMP_STATR & CMP_OUT) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      CMP_GetFlagStatus
 *
 * @brief   Checks whether the CMP flag is set or not.
 *
 * @param   CMP_FLAG - specifies the CMP flag to check.
 *          CMP2_FLAG_OUTHIGH - CMP2 High level flag.
 *          CMP3_FLAG_CHOUT0 - CMP3 channel 1 High level flag.
 *          CMP3_FLAG_CHOUT1 - CMP3 channel 2 High level flag.
 *          CMP3_FLAG_CHOUT2 - CMP3 channel 3 High level flag.
 *          CMP3_FLAG_CHOUT3 - CMP3 channel 4 High level flag.
 *
 * @return  FlagStatus: SET or RESET.
 */
FlagStatus CMP_GetFlagStatus(uint32_t CMP_FLAG)
{
    FlagStatus bitstatus = RESET;

    if((OPA->CMP_STATR & CMP_FLAG) != (uint32_t)RESET)
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }

    return bitstatus;
}

/*********************************************************************
 * @fn      CMP_ClearITPendingBit
 *
 * @brief   Clears the CMP flag .
 *
 * @param   CMP_FLAG - specifies the CMP flag to clear.
 *          CMP2_FLAG_OUTHIGH - CMP2 High level flag.
 *          CMP3_FLAG_CHOUT0 - CMP3 channel 1 High level flag.
 *          CMP3_FLAG_CHOUT1 - CMP3 channel 2 High level flag.
 *          CMP3_FLAG_CHOUT2 - CMP3 channel 3 High level flag.
 *          CMP3_FLAG_CHOUT3 - CMP3 channel 4 High level flag.
 *
 * @return  none
 */
void CMP_ClearFlag( uint32_t CMP_FLAG)
{
    OPA->CMP_STATR = CMP_FLAG;
}

/*********************************************************************
 * @fn      QII_OutToTIM3Cap_Cmd
 *
 * @brief   Enables or disables QII internal out to TIM3 capture channel.
 *          QII1 connect to TIM3_CH1
 *          QII2 connect to TIM3_CH2
 *
 * @param   NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void QII_OutToTIM3Cap_Cmd(FunctionalState NewState)
{
    if(NewState != DISABLE)
    {
        OPA->CMP_CTLR |= (1<<18);
    }
    else
    {
        OPA->CMP_CTLR &= ~(1<<18);
    }
}

/*********************************************************************
 * @fn      ISP_OutToADC_CMD
 *
 * @brief   Enables or disables the ISP module output to ADC channel.
 *
 * @param   ISPx - the specified OPA3_ISP1 or OPA4_ISP2.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void ISP_OutToADC_CMD(OPA_ISP_Num_TypeDef ISPx,FunctionalState NewState)
{
    uint8_t tmp;
    tmp = 16*ISPx + 5;

    if(NewState != DISABLE)
    {
        OPA->ISP_CTLR |= (1 << tmp);
    }
    else
    {
        OPA->ISP_CTLR &= ~(1 << tmp);
    }
}

/*********************************************************************
 * @fn      ISP_QDET_CMD
 *
 * @brief   Enables or disables the ISP module Q value detection.
 *
 * @param   ISPx - the specified OPA3_ISP1 or OPA4_ISP2.
 *          NewState - ENABLE or DISABLE.
 *
 * @return  none
 */
void ISP_QDET_CMD(OPA_ISP_Num_TypeDef ISPx,FunctionalState NewState)
{
    uint8_t tmp;
    tmp = 16*ISPx + 6;

    if(NewState != DISABLE)
    {
        OPA->ISP_CTLR |= (1 << tmp);
    }
    else
    {
        OPA->ISP_CTLR &= ~(1 << tmp);
    }
}
