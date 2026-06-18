/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2024/06/06
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "debug.h"
#include "ov2640.h"
/*
 *@Note
 * DVP routine to operate OV2640 camera JPEG mode:
 *   Output picture data through UART2 (PA2), display pictures through serial port image software,
 *   or take 0xFF from a frame of data, starting with 0xD8;0xFF, data at the end of 0xD9, modify the
 *   file format to display pictures.
 * DVP--PIN:
 *   D10--PD6
 *   D11--PD2
 *   D8--PC10
 *   D9--PC12
 *   DPWDN--PE5
 *   DPCLK--PA6
 *   D7--PB9
 *   D6--PB8
 *   D5--PB6
 *   D4--PC11
 *   D3--PC9
 *   D2--PC8
 *   D1--PA10
 *   D0--PA9
 *   RESET--PE4
*    SCL--PE2
 *   SDA--PE3
 *   HSYNC--PA4
 *   VSYNC--PA5
 *
 *  Use UART2 (PA2) serial port output, #define DEBUG DEBUG_UART2 in debug.h
 *  UART1(PA9) is occupied by DVP
 *
 */

/* DVP Work Mode JPEG_MODE */
UINT32  JPEG_DVPDMAaddr0 = 0x20005000;
UINT32  JPEG_DVPDMAaddr1 = 0x20005000 + OV2640_JPEG_WIDTH;

volatile uint32_t frame_cnt = 0;
volatile uint32_t addr_cnt = 0;
volatile uint32_t href_cnt = 0;

void DVP_IRQHandler (void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      UART2_Send_Byte
 *
 * @brief   UART2 send one byte data.
 *
 * @param   t - UART send Data.
 *
 * @return  none
 */
void UART2_Send_Byte(u8 t)
{
    while (USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    USART_SendData(USART2, t);
}

/*********************************************************************
 * @fn      DVP_Init
 *
 * @brief   Init DVP
 *
 * @return  none
 */
void DVP_Function_Init(void)
{
    DVP_InitTypeDef DVP_InitStructure = {0};
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DVP, ENABLE);
    DVP_DeInit();

    DVP_InitStructure.DVP_DataSize = DVP_DataSize_8b;
    DVP_InitStructure.DVP_COL_NUM = OV2640_JPEG_WIDTH;
    DVP_InitStructure.DVP_HCLK_P = DVP_Hclk_P_Rising;
    DVP_InitStructure.DVP_HSYNC_P = DVP_Hsync_P_High;
    DVP_InitStructure.DVP_VSYNC_P = DVP_Vsync_P_High;
    DVP_InitStructure.DVP_DMA_BUF0_Addr = JPEG_DVPDMAaddr0;
    DVP_InitStructure.DVP_DMA_BUF1_Addr = JPEG_DVPDMAaddr1;
    DVP_InitStructure.DVP_FrameCapRate = DVP_FrameCapRate_25P;
    DVP_InitStructure.DVP_JPEGMode = ENABLE;
    DVP_Init(&DVP_InitStructure);

    DVP_ReceiveCircuitResetCmd(DISABLE);
    DVP_FIFO_ResetCmd(DISABLE);

    NVIC_EnableIRQ(DVP_IRQn);
    NVIC_SetPriority(DVP_IRQn, 0);

    DVP_ITConfig(DVP_IT_STR_FRM|DVP_IT_ROW_DONE|DVP_IT_FRM_DONE|DVP_IT_FIFO_OV|DVP_IT_STP_FRM, ENABLE);
    
    DVP_DMACmd(ENABLE);
    DVP_Cmd(ENABLE);
}

/*********************************************************************
 * @fn      DVP_IRQHandler
 *
 * @brief   This function handles DVP exception.
 *
 * @return  none
 */
void DVP_IRQHandler(void)
{
    if (DVP_GetITStatus(DVP_IT_ROW_DONE) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_ROW_DONE);

        href_cnt++;
        if (addr_cnt%2)     /* buf1 done */
        {
            addr_cnt++;
            DVP->DMA_BUF1 += OV2640_JPEG_WIDTH *2;
        }
        else                /* buf0 done */
        {
            addr_cnt++;
            DVP->DMA_BUF0 += OV2640_JPEG_WIDTH *2;
        }
    }

    if (DVP_GetITStatus(DVP_IT_FRM_DONE) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_FRM_DONE);

        DVP_Cmd(DISABLE);
        /* send JPEG data */
        {
            vu32 i;
            vu32 val;
            href_cnt = href_cnt*OV2640_JPEG_WIDTH;
            for(i=0; i<href_cnt; i++){
                val = *(UINT8*)(0x20005000+i);
                UART2_Send_Byte(val);
            }
        }
        DVP_Cmd(ENABLE);

        DVP->DMA_BUF0 = JPEG_DVPDMAaddr0;  /* DMA addr0 */
        DVP->DMA_BUF1 = JPEG_DVPDMAaddr1;  /* DMA addr1 */
        href_cnt = 0;
        addr_cnt =0;       
    }

    if (DVP_GetITStatus(DVP_IT_STR_FRM) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_STR_FRM);
        frame_cnt++;
    }

    if (DVP_GetITStatus(DVP_IT_STP_FRM) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_STP_FRM);
    }

    if (DVP_GetITStatus(DVP_IT_FIFO_OV) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_FIFO_OV);
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
	SystemCoreClockUpdate();
	Delay_Init();
	USART_Printf_Init(921600);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

  while(OV2640_Init())
  {
      printf("Camera Model Error\r\n");
      Delay_Ms(1000);
  }
    printf("JPEG Mode\r\n");
    OV2640_JPEG_Mode_Init();
    Delay_Ms(1000);

    DVP_Function_Init();
    while(1);
}







