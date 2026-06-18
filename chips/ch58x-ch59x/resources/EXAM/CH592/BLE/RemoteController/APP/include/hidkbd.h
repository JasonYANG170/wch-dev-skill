/********************************** (C) COPYRIGHT *******************************
 * File Name          : hidkbd.h
 * Author             : WCH
 * Version            : V1.0
 * Date               : 2018/12/10
 * Description        :
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#ifndef HIDKBD_H
#define HIDKBD_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 */

/*********************************************************************
 * CONSTANTS
 */

// Task Events
#define START_DEVICE_EVT          0x0001
#define START_PARAM_UPDATE_EVT    0x0004
#define START_PHY_UPDATE_EVT      0x0008
/*********************************************************************
 * MACROS
 */

/*********************************************************************
 * FUNCTIONS
 */

/*********************************************************************
 * GLOBAL VARIABLES
 */
extern uint16_t hidEmuConnHandle;
extern uint8_t hidEmuTaskId;
/*
 * Task Initialization for the BLE Application
 */
extern void HidEmu_Init(void);

/*
 * Task Event Processor for the BLE Application
 */
extern uint16_t HidEmu_ProcessEvent(uint8_t task_id, uint16_t events);

extern void hidEmu_adv_enable(uint8_t enable);
extern uint8_t hidEmu_class_keyboard_report(uint8_t *pData, uint8_t len);
extern uint8_t hidEmu_mouse_report(uint8_t *pData, uint8_t len);
extern uint8_t hidEmu_consumer_report(uint8_t *pData, uint8_t len);
extern uint8_t hidEmu_sys_ctl_report(uint8_t data);
extern uint8_t hidEmu_all_keyboard_report(uint8_t *pData, uint8_t len);
/*********************************************************************
*********************************************************************/

#ifdef __cplusplus
}
#endif

#endif
