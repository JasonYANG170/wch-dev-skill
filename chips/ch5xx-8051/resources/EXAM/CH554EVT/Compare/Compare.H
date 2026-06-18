/********************************** (C) COPYRIGHT *******************************
* File Name          : COMPARE.H
* Author             : WCH
* Version            : V1.00
* Date               : 2017-11-18
* Description        : 
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#ifndef CMPARE_H
#define CMPARE_H

typedef enum 
{	
	IN0, 													/* P11 */			
	IN1, 				                  /* P14 */
	IN2, 	                        /* P15 */
	IN3, 	                        /* P32 */
} _cmp_channel;



#define NO_INVERT_CHAN0					(0X00)
#define NO_INVERT_CHAN1					(0x01)
#define NO_INVERT_CHAN2					(0X02)
#define NO_INVERT_CHAN3					(0X03)

#define INVERT_IN						(0X08)

#define IE_CMP					IE_ADC
#define	CMP_START				ADC_START				

extern UINT8	CMP_Flag;

extern void CMP_Enable(void);
extern void CMP_Disable(void);
extern UINT8 CMP_Init(_cmp_channel pos, _cmp_channel neg);
extern void CMP_Interrupt_Enable();
extern void CMP_Interrupt_Disable();
extern UINT8 CMP_Out(void);

#endif
/* END OF FILE */
