/********************************** (C) COPYRIGHT ******************************
* File Name          : KEY.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : define for KEY.C
*******************************************************************************/



/******************************************************************************/
#ifndef __KEY_H
#define __KEY_H	 


/* KEY */
#define KEY_00             (1<<18)
#define KEY_11             (1<<19)

#define KEY1               (1<<20)
#define KEY2               (1<<21)
#define KEY3               (1<<22)
#define KEY4               (1<<23)



#define KEY_OUT_HIGH( KEY )     { R32_PD_OUT |= KEY; }                          /* KEY 输出高电平 */
#define KEY_OUT_LOW( KEY )      { R32_PD_OUT &= ~KEY; }                          /* KEY 输出低电平 */
#define KEY_OUT_DIR( KEY )      { R32_PD_DIR |= KEY; }                          /* KEY 方向为输出 */


void KEY_SCAN( void );
void KEY_INIT( void );

#endif
