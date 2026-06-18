/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch641_i2c.h
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/07/08
* Description        : This file contains all the functions prototypes for the
*                      I2C firmware library.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#ifndef __CH641_I2C_H
#define __CH641_I2C_H

#ifdef __cplusplus
extern "C" {
#endif

#include <ch641.h>

/* I2C Init structure definition  */
typedef struct
{
    uint16_t I2C_OwnAddress1; /* Specifies the first device own address.
                                 This parameter can be a 7-bit or 10-bit address. */

    uint16_t I2C_Ack; /* Enables or disables the acknowledgement.
                         This parameter can be a value of @ref I2C_acknowledgement */

    uint16_t I2C_AcknowledgedAddress; /* Specifies if 7-bit or 10-bit address is acknowledged.
                                         This parameter can be a value of @ref I2C_acknowledged_address */
} I2C_InitTypeDef;

/* I2C_acknowledgement */
#define I2C_Ack_Enable                                       ((uint16_t)0x0400)
#define I2C_Ack_Disable                                      ((uint16_t)0x0000)

/* I2C_transfer_direction */
#define I2C_Direction_Transmitter                            ((uint8_t)0x00)
#define I2C_Direction_Receiver                               ((uint8_t)0x01)

/* I2C_acknowledged_address */
#define I2C_AcknowledgedAddress_7bit                         ((uint16_t)0x4000)
#define I2C_AcknowledgedAddress_10bit                        ((uint16_t)0xC000)

/* I2C_registers */
#define I2C_Register_CTLR1                                   ((uint8_t)0x00)
#define I2C_Register_CTLR2                                   ((uint8_t)0x04)
#define I2C_Register_OADDR1                                  ((uint8_t)0x08)
#define I2C_Register_OADDR2                                  ((uint8_t)0x0C)
#define I2C_Register_DATAR                                   ((uint8_t)0x10)
#define I2C_Register_STAR1                                   ((uint8_t)0x14)
#define I2C_Register_STAR2                                   ((uint8_t)0x18)

/* I2C_PEC_position */
#define I2C_PECPosition_Next                                 ((uint16_t)0x0800)
#define I2C_PECPosition_Current                              ((uint16_t)0xF7FF)

/* I2C_NACK_position */
#define I2C_NACKPosition_Next                                ((uint16_t)0x0800)
#define I2C_NACKPosition_Current                             ((uint16_t)0xF7FF)

/* I2C_interrupts_definition */
#define I2C_IT_BUF                                           ((uint16_t)0x0400)
#define I2C_IT_EVT                                           ((uint16_t)0x0200)
#define I2C_IT_ERR                                           ((uint16_t)0x0100)

/* I2C_interrupts_definition */
#define I2C_IT_PECERR                                        ((uint32_t)0x01001000)
#define I2C_IT_OVR                                           ((uint32_t)0x01000800)
#define I2C_IT_AF                                            ((uint32_t)0x01000400)
#define I2C_IT_BERR                                          ((uint32_t)0x01000100)
#define I2C_IT_TXE                                           ((uint32_t)0x06000080)
#define I2C_IT_RXNE                                          ((uint32_t)0x06000040)
#define I2C_IT_STOPF                                         ((uint32_t)0x02000010)
#define I2C_IT_BTF                                           ((uint32_t)0x02000004)
#define I2C_IT_ADDR                                          ((uint32_t)0x02000002)

/* STAR2 register flags  */
#define I2C_FLAG_DUALF                                       ((uint32_t)0x00800000)
#define I2C_FLAG_GENCALL                                     ((uint32_t)0x00100000)
#define I2C_FLAG_TRA                                         ((uint32_t)0x00040000)
#define I2C_FLAG_BUSY                                        ((uint32_t)0x00020000)

/* STAR1 register flags */
#define I2C_FLAG_PECERR                                      ((uint32_t)0x10001000)
#define I2C_FLAG_OVR                                         ((uint32_t)0x10000800)
#define I2C_FLAG_AF                                          ((uint32_t)0x10000400)
#define I2C_FLAG_BERR                                        ((uint32_t)0x10000100)
#define I2C_FLAG_TXE                                         ((uint32_t)0x10000080)
#define I2C_FLAG_RXNE                                        ((uint32_t)0x10000040)
#define I2C_FLAG_STOPF                                       ((uint32_t)0x10000010)
#define I2C_FLAG_BTF                                         ((uint32_t)0x10000004)
#define I2C_FLAG_ADDR                                        ((uint32_t)0x10000002)

/******************I2C Slave Events (Events grouped in order of communication)******************/

/******************************************************************************************************************** 
  * @brief  Start Communicate events
  * 
  * Wait on one of these events at the start of the communication. It means that 
  * the I2C peripheral detected a start condition of master device generate on the bus.  
  * If the acknowledge feature is enabled through function I2C_AcknowledgeConfig()),The peripheral generates an ACK condition on the bus. 
  *    
  * a) In normal case (only one address managed by the slave), when the address 
  *   sent by the master matches the own address of the peripheral (configured by 
  *   I2C_OwnAddress1 field) the I2C_EVENT_SLAVE_XXX_ADDRESS_MATCHED event is set 
  *   (where XXX could be TRANSMITTER or RECEIVER).
  *    
  * b) In case the address sent by the master matches the second address of the 
  *   peripheral (configured by the function I2C_OwnAddress2Config() and enabled 
  *   by the function I2C_DualAddressCmd()) the events I2C_EVENT_SLAVE_XXX_SECONDADDRESS_MATCHED 
  *   (where XXX could be TRANSMITTER or RECEIVER) are set.
  *   
  * c) In case the address sent by the master is General Call (address 0x00) and 
  *   if the General Call is enabled for the peripheral (using function I2C_GeneralCallCmd()) 
  *   the following event is set I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED.   
  * 
  */

/* EVT1 */   
/* a) Case of One Single Address managed by the slave */
#define  I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED          ((uint32_t)0x00020002) /* BUSY and ADDR flags */
#define  I2C_EVENT_SLAVE_TRANSMITTER_ADDRESS_MATCHED       ((uint32_t)0x00060082) /* TRA, BUSY, TXE and ADDR flags */

/* b) Case of Dual address managed by the slave */
#define  I2C_EVENT_SLAVE_RECEIVER_SECONDADDRESS_MATCHED    ((uint32_t)0x00820000)  /* DUALF and BUSY flags */
#define  I2C_EVENT_SLAVE_TRANSMITTER_SECONDADDRESS_MATCHED ((uint32_t)0x00860080)  /* DUALF, TRA, BUSY and TXE flags */

/* c) Case of General Call enabled for the slave */
#define  I2C_EVENT_SLAVE_GENERALCALLADDRESS_MATCHED        ((uint32_t)0x00120000)  /* GENCALL and BUSY flags */

/******************************************************************************************************************** 
  * @brief  Communication events
  * 
  * Wait on one of these events when EVT1 has already been checked -
  * 
  * - Slave Receiver mode -
  *     - EVT2--The device is expecting to receive a data byte . 
  *     - EVT4--The device is expecting the end of the communication -
  *       master sends a stop condition and data transmission is stopped.
  *    
  * - Slave Transmitter mode -
  *    - EVT3--When a byte has been transmitted by the slave and the Master is expecting 
  *      the end of the byte transmission. The two events I2C_EVENT_SLAVE_BYTE_TRANSMITTED and
  *      I2C_EVENT_SLAVE_BYTE_TRANSMITTING are similar. If the user software doesn't guarantee 
  *      the EVT3 is managed before the current byte end of transfer The second one can optionally
  *      be used. 
  *    - EVT3_2--When the master sends a NACK  to tell slave device that data transmission 
  *      shall end . The slave device has to stop sending 
  *      data bytes and wait a Stop condition from bus.
  *      
  * Note -
  *  If the  user software does not guarantee that the event 2 is 
  *  managed before the current byte end of transfer, User may check on I2C_EVENT_SLAVE_BYTE_RECEIVED 
  *  and I2C_FLAG_BTF flag at the same time .
  *  In this case the communication will be slower.
  *
  */

/* Slave Receiver mode*/ 
/* EVT2 */
#define  I2C_EVENT_SLAVE_BYTE_RECEIVED                     ((uint32_t)0x00020040)  /* BUSY and RXNE flags */
/* EVT4  */
#define  I2C_EVENT_SLAVE_STOP_DETECTED                     ((uint32_t)0x00000010)  /* STOPF flag */

/* Slave Transmitter mode -----------------------*/
/* EVT3 */
#define  I2C_EVENT_SLAVE_BYTE_TRANSMITTED                  ((uint32_t)0x00060084)  /* TRA, BUSY, TXE and BTF flags */
#define  I2C_EVENT_SLAVE_BYTE_TRANSMITTING                 ((uint32_t)0x00060080)  /* TRA, BUSY and TXE flags */
/* EVT3_2 */
#define  I2C_EVENT_SLAVE_ACK_FAILURE                       ((uint32_t)0x00000400)  /* AF flag */


void     I2C_DeInit(I2C_TypeDef *I2Cx);
void     I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef *I2C_InitStruct);
void     I2C_StructInit(I2C_InitTypeDef *I2C_InitStruct);
void     I2C_Cmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void     I2C_DMACmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void     I2C_AcknowledgeConfig(I2C_TypeDef *I2Cx, FunctionalState NewState);
void     I2C_OwnAddress2Config(I2C_TypeDef *I2Cx, uint8_t Address);
void     I2C_DualAddressCmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void     I2C_GeneralCallCmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void     I2C_ITConfig(I2C_TypeDef *I2Cx, uint16_t I2C_IT, FunctionalState NewState);
void     I2C_SendData(I2C_TypeDef *I2Cx, uint8_t Data);
uint8_t  I2C_ReceiveData(I2C_TypeDef *I2Cx);
void     I2C_Send7bitAddress(I2C_TypeDef *I2Cx, uint8_t Address, uint8_t I2C_Direction);
uint16_t I2C_ReadRegister(I2C_TypeDef *I2Cx, uint8_t I2C_Register);
void     I2C_SoftwareResetCmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void     I2C_NACKPositionConfig(I2C_TypeDef *I2Cx, uint16_t I2C_NACKPosition);
void     I2C_TransmitPEC(I2C_TypeDef *I2Cx, FunctionalState NewState);
void     I2C_PECPositionConfig(I2C_TypeDef *I2Cx, uint16_t I2C_PECPosition);
void     I2C_CalculatePEC(I2C_TypeDef *I2Cx, FunctionalState NewState);
uint8_t  I2C_GetPEC(I2C_TypeDef *I2Cx);
void     I2C_ARPCmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void     I2C_StretchClockCmd(I2C_TypeDef *I2Cx, FunctionalState NewState);


/*****************************************************************************************
 *
 *                         I2C State Monitoring Functions
 *                       
 ****************************************************************************************   
 * This I2C driver provides three different ways for I2C state monitoring
 *  profit the application requirements and constraints -
 *        
 *  
 * a) First way -
 *    Using I2C_CheckEvent() function -
 *    It compares the status registers (STARR1 and STAR2) content to a given event
 *    (can be the combination of more flags).
 *    If the current status registers includes the given flags  will return SUCCESS.
 *    and  if the current status registers miss flags will returns ERROR.
 *    - When to use -
 *      - This function is suitable for most applications as well as for startup 
 *      activity since the events are fully described in the product reference manual 
 *      (CH32V03RM).
 *      - It is also suitable for users who need to define their own events.
 *    - Limitations -
 *      - If an error occurs besides to the monitored error,
 *        the I2C_CheckEvent() function may return SUCCESS despite the communication
 *       in corrupted state.  it is suggeted to use error interrupts to monitor the error
 *        events and handle them in IRQ handler.
 *
 *        
 *      Note -
 *        The following functions are recommended for error management:
 *          - I2C_ITConfig() main function of configure and enable the error interrupts.
 *          - I2Cx_ER_IRQHandler() will be called when the error interrupt happen.
 *            Where x is the peripheral instance
 *          -  I2Cx_ER_IRQHandler() will call I2C_GetFlagStatus() or I2C_GetITStatus() functions
 *            to determine which error occurred.
 *          - I2C_ClearFlag() \ I2C_ClearITPendingBit() \ I2C_SoftwareResetCmd()
 *            \ I2C_GenerateStop() will be use to clear the error flag and source,
 *            and return to correct communication status.
 *            
 *
 *  b) Second way -
 *     Using the function to get a single word(uint32_t) composed of status register 1 and register 2. 
 *     (Status Register 2 value is shifted left by 16 bits and concatenated to Status Register 1).
 *     - When to use -
 *
 *       - This function is suitable for the same applications above but it 
 *         don't have the limitations of I2C_GetFlagStatus() function .
 *         The returned value could be compared to events already defined in the 
 *         library (ch641_i2c.h) or to custom values defined by user.
 *       - This function can be used to monitor the status of multiple flags simultaneously.
 *       - Contrary to the I2C_CheckEvent () function, this function can choose the time to
 *         accept the event according to the user's needs (when all event flags are set and  
 *         no other flags are set, or only when the required flags are set) 
 *     
 *     - Limitations -
 *       - User may need to define his own events.
 *       - Same remark concerning the error management is applicable for this 
 *         function if user decides to check only regular communication flags (and 
 *         ignores error flags).
 *     
 *
 *  c) Third way -
 *     Using the function I2C_GetFlagStatus() get the status of 
 *     one single flag . 
 *     - When to use -
 *        - This function could be used for specific applications or in debug phase.
 *        - It is suitable when only one flag checking is needed . 
 *          
 *     - Limitations -
 *        - Call this function to access the status register. Some flag bits may be cleared.           
 *       - Function may need to be called twice or more in order to monitor one single event. 
 */
            
 

/*********************************************************
 * 
 *  a) Basic state monitoring(First way)
 ********************************************************
 */
ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
/*********************************************************
 * 
 *  b) Advanced state monitoring(Second way:)
 ********************************************************
 */
uint32_t I2C_GetLastEvent(I2C_TypeDef* I2Cx);
/*********************************************************
 * 
 *  c) Flag-based state monitoring(Third way)
 *********************************************************
 */
FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);

void     I2C_ClearFlag(I2C_TypeDef *I2Cx, uint32_t I2C_FLAG);
ITStatus I2C_GetITStatus(I2C_TypeDef *I2Cx, uint32_t I2C_IT);
void     I2C_ClearITPendingBit(I2C_TypeDef *I2Cx, uint32_t I2C_IT);

#ifdef __cplusplus
}
#endif

#endif
