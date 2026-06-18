/********************************** (C) COPYRIGHT *******************************
* File Name          : KEYBOARD.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/05/15
* Description        : CH563 USB_Device Example
*******************************************************************************/

#ifndef __KEYBOARD_H
#define __KEYBOARD_H

extern volatile UINT8 KEY_SendFlag;

void KEY_SendKey(UINT8 num);
void KEY_SendBlank(UINT8 num);

#endif
