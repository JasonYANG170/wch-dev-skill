/********************************** (C) COPYRIGHT *******************************
 * File Name          : main.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2025/03/01
 * Description        : Main program body.
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

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4|GPIO_Pin_5;
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
                    for(int zT=0;zT<100;zT++)
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
        printf("-------------task5 entry\r\n");
        GPIO_ResetBits(GPIOA, GPIO_Pin_5);
        vTaskDelay(500);
        GPIO_SetBits(GPIOA, GPIO_Pin_5);
        vTaskDelay(500);
    }
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
	SystemAndCoreClockUpdate();
	USART_Printf_Init(115200);
	printf("V5F SystemCoreClk:%d\r\n", SystemCoreClock);

#if (Run_Core == Run_Core_V3FandV5F)
	HSEM_FastTake(HSEM_ID0);
	HSEM_ReleaseOneSem(HSEM_ID0, 0);

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
	// Hardware();
 
#endif


	while(1)
	{
		printf("V5F running...\r\n");
		// Delay_Ms(1000);
	}
}
