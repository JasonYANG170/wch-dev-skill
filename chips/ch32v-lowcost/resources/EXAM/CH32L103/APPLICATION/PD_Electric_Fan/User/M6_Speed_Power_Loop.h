/* Define to prevent recursive inclusion ------------------------------------*/
#ifndef __M6_SPEEDLOOP_H
#define __M6_SPEEDLOOP_H

/* Includes -----------------------------------------------------------------*/

/* Exported constants -------------------------------------------------------*/
//转速环使用定时器时钟频率(unit-Hz)
#define TIMER_CLOCK_FREQ_SPEED      96000000uL

//转速环定时器中断频率(unit-Hz)
#define TIMER_FREQ_SPEED_LOOP       2000

//SYS定时器周期值
#define TIMER_PERIDO_SYSTICK     TIMER_CLOCK_FREQ_SPEED / (u32)(TIMER_FREQ_SPEED_LOOP*8)

//转速环计算频率(unit-Hz)
#define SPEED_LOOP_CAL_FREQ         2000

//转速环计算周期(unit-s)
#define SPEED_LOOP_CAL_PERIOD      _IQ(1.0)/SPEED_LOOP_CAL_FREQ

//转速环PID调节器计算周期(x+1)*500usec
#define SPEED_SAMPLE_PID_PERIOD     TIMER_FREQ_SPEED_LOOP/SPEED_LOOP_CAL_FREQ

//PD计算周期
#define PD_PERIOD                        2

/* Exported variables -------------------------------------------------------*/
/* Exported macro -----------------------------------------------------------*/

/* Exported functions -------------------------------------------------------*/
//转速处理定时器初始化
void Systick_Init(void);

//转速处理定时器使能
void SYS_TIM_Cmd(FunctionalState NewState);

#endif
