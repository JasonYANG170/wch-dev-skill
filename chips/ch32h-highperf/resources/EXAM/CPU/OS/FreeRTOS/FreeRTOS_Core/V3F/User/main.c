/********************************** (C) COPYRIGHT *******************************
 * File Name          : main_v3f.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/03/01
 * Description        : Main program body for V3F.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*
 *@Note
 *task1 and task2 alternate printing
 */

#include "debug.h"
#include "FreeRTOS.h"
#include "task.h"
#include "hardware.h"
#include "shared.h"

/* Global define */
#define TASK1_TASK_PRIO     5
#define TASK1_STK_SIZE      256
#define TASK2_TASK_PRIO     5
#define TASK2_STK_SIZE      256

/* Global Variable */
TaskHandle_t Task1Task_Handler;
TaskHandle_t Task2Task_Handler;


/*********************************************************************
 * @fn      GPIO_Toggle_INIT
 *
 * @brief   Initializes GPIOA.0/1
 *
 * @return  none
 */
void GPIO_Toggle_INIT(void)
{
  GPIO_InitTypeDef  GPIO_InitStructure={0};

  RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA,ENABLE);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0|GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed=GPIO_Speed_High;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}


/*********************************************************************
 * @fn      task1_task
 *
 * @brief   task1 program.
 *
 * @param  *pvParameters - Parameters point of task1
 *
 * @return  none
 */
void task1_task(void *pvParameters)
{
    while(1)
    {
		if(HSEM_FastTake(HSEM_ID0) == READY)
		{
		for(u8 t=0; t<4; t++)
		{
			printf("Buffer_Sharing[%d] - %d\r\n",t,Buffer_Sharing[t]);
			Buffer_Sharing[t] += 1;
			for(int zF=0;zF<100;zF++)
			{
				__NOP();
			}

		}
		printf("Data_Sharing - %x\r\n",Data_Sharing);
		Data_Sharing += 1;
		HSEM_ReleaseOneSem(HSEM_ID0, 0);
		for(int z=0;z<100;z++)
		{
			__NOP();
		}
		}
    }
}

/*********************************************************************
 * @fn      task2_task
 *
 * @brief   task2 program.
 *
 * @param  *pvParameters - Parameters point of task2
 *
 * @return  none
 */
void task2_task(void *pvParameters)
{
    while(1)
    {
        printf("------task2 entry\r\n");
        GPIO_ResetBits(GPIOA, GPIO_Pin_1);
        vTaskDelay(500);
        GPIO_SetBits(GPIOA, GPIO_Pin_1);
        vTaskDelay(500);
    }
}

/*********************************************************************
 * @fn      Reset_Sharing_Data
 *
 * @brief   Initializes Sharing_Data.
 *
 * @return  none
 */
void Reset_Sharing_Data(void)
{
	for(int i = 0;i<4;i++)
	{
		Buffer_Sharing[i] = i;
	}
	Data_Sharing = 0xFFFF0000;
}


/*********************************************************************
 * @fn      main
 *
 * @brief   Main program.
 *
 * @return  none
 */
int main(void)
{
	SystemInit();
	SystemAndCoreClockUpdate();

	USART_Printf_Init(115200);		
	printf("1SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );


#if (Run_Core == Run_Core_V3FandV5F)
	NVIC_WakeUp_V5F(Core_V5F_StartAddr);//wake up V5
	HSEM_ITConfig(HSEM_ID0, ENABLE);
    NVIC->SCTLR |= 1<<4;
	RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR,ENABLE);
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
	HSEM_ClearFlag(HSEM_ID0);
	printf("V3F wake up\r\n");
	Reset_Sharing_Data();
	printf("FreeRTOS Kernel Version:%s\r\n",tskKERNEL_VERSION_NUMBER);

    GPIO_Toggle_INIT();
    /* create two task */
    xTaskCreate((TaskFunction_t )task2_task,
                        (const char*    )"task2",
                        (uint16_t       )TASK2_STK_SIZE,
                        (void*          )NULL,
                        (UBaseType_t    )TASK2_TASK_PRIO,
                        (TaskHandle_t*  )&Task2Task_Handler);

    xTaskCreate((TaskFunction_t )task1_task,
                    (const char*    )"task1",
                    (uint16_t       )TASK1_STK_SIZE,
                    (void*          )NULL,
                    (UBaseType_t    )TASK1_TASK_PRIO,
                    (TaskHandle_t*  )&Task1Task_Handler);
    vTaskStartScheduler();

    while(1)
    {
        printf("shouldn't run at here!!\n");

    }	

#elif (Run_Core == Run_Core_V3F)

#elif (Run_Core == Run_Core_V5F)
	NVIC_WakeUp_V5F(Core_V5F_StartAddr);//wake up V5
	PWR_EnterSTOPMode(PWR_Regulator_ON, PWR_STOPEntry_WFE);
	printf("V3F wake up\r\n");
#endif

	while(1)
	{
		printf("V3F running...\r\n");
		Delay_Ms(1000);
	}

}
