/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/02/08
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "debug.h"
#include "lcd.h"
#include "ov5640.h"
#include "ov2640.h"
/*
 *@Note
  DVP routine to operate ov2640\ov5640 camera video mode to display image on TFTLCD:
  DVP--PIN:
    D10--PD6
    D11--PD2
    D8--PC10
    D9--PC12
    DPWDN--PE5
    DPCLK--PA6
    D7--PB9
    D6--PB8
    D5--PB6
    D4--PC11
    D3--PC9
    D2--PC8
    D1--PA10
    D0--PA9
    RESET--PE4
    SCL--PE2
    SDA--PE3
    HSYNC--PA4
    VSYNC--PA5
  FSMC routine to operate TFTLCD-LCD--PIN:
    PD7--NE1(CS)
    PD12--RS
    PD5 --FSMC_NEW(WR)
    PD4 --FSMC_NOE(RD)
    PA15 --LCDRST#
    PD14--FSMC_D0
    PD15--FSMC_D1
    PD0 --FSMC_D2
    PD1--FSMC_D3
    PE7--FSMC_D4
    PE8 --FSMC_D5
    PE9 --FSMC_D6
    PE10--FSMC_D7
    PE11--FSMC_D8
    PB11--FSMC_D9
    PB12--FSMC_D10
    PB13--FSMC_D11
    PB14--FSMC_D12
    PD8 --FSMC_D13
    PD9 --FSMC_D14
    PD10--FSMC_D15
    PB1--BL
 */

/* Camera module selection */
#define OV2640_Module  0
#define OV5640_Module  1
#define CAMERA_SELECT  OV5640_Module

#if(CAMERA_SELECT == OV2640_Module)
uint16_t ImageDataOv2640[2 * OV2640_RGB565_WIDTH] = {0};
#elif(CAMERA_SELECT == OV5640_Module)
uint16_t ImageDataOv5640[2 * OV5640_RGB565_WIDTH] = {0};
#endif

volatile UINT32 frame_cnt = 0;
volatile UINT32 href_cnt = 0;

void DVP_IRQHandler(void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      DMA_SRAMLCD_Enable
 *
 * @brief   Enable SRAMLCD DMA transmission
 *
 * @return  none
 */
void DMA_SRAMLCD_Enable(void)
{
    DMA_Cmd(DMA2_Channel5, DISABLE );
    DMA_SetCurrDataCounter(DMA2_Channel5,lcddev.width);
    DMA_Cmd(DMA2_Channel5, ENABLE);
}

/*********************************************************************
 * @fn      LCD_Reset_GPIO_Init
 *
 * @brief   Init LCD reset GPIO.
 *
 * @return  none
 */
void LCD_Reset_GPIO_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure={0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_SetBits(GPIOA,GPIO_Pin_15);
}

/*********************************************************************
 * @fn      DMA_SRAMLCD_Init
 *
 * @brief   Init SRAMLCD DMA
 *
 * @param   ddr: DVP data memory base addr.
 *
 * @return  none
 */
void DMA_SRAMLCD_Init(u32 ddr)
{
    DMA_InitTypeDef DMA_InitStructure={0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA2, ENABLE);

    DMA_DeInit(DMA2_Channel5);

    DMA_InitStructure.DMA_PeripheralBaseAddr = ddr;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)&LCD->LCD_RAM;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Enable;
    DMA_Init(DMA2_Channel5, &DMA_InitStructure);
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
#if(CAMERA_SELECT == OV2640_Module)
    DVP_InitStructure.DVP_COL_NUM = OV2640_RGB565_WIDTH * 2;
    DVP_InitStructure.DVP_ROW_NUM = OV2640_RGB565_HEIGHT;
#elif(CAMERA_SELECT == OV5640_Module)
    DVP_InitStructure.DVP_COL_NUM = OV5640_RGB565_WIDTH * 2;
    DVP_InitStructure.DVP_ROW_NUM = OV5640_RGB565_HEIGHT;
#endif
    DVP_InitStructure.DVP_HCLK_P = DVP_Hclk_P_Rising;
    DVP_InitStructure.DVP_HSYNC_P = DVP_Hsync_P_High;
    DVP_InitStructure.DVP_VSYNC_P = DVP_Vsync_P_High;
#if(CAMERA_SELECT == OV2640_Module)
    DVP_InitStructure.DVP_DMA_BUF0_Addr = (uint32_t)(ImageDataOv2640);
    DVP_InitStructure.DVP_DMA_BUF1_Addr = (uint32_t)(ImageDataOv2640 + OV2640_RGB565_WIDTH);
#elif(CAMERA_SELECT == OV5640_Module)
    DVP_InitStructure.DVP_DMA_BUF0_Addr = (uint32_t)(ImageDataOv5640);
    DVP_InitStructure.DVP_DMA_BUF1_Addr = (uint32_t)(ImageDataOv5640 + OV5640_RGB565_WIDTH);
#endif
    DVP_InitStructure.DVP_FrameCapRate = DVP_FrameCapRate_100P;
    DVP_InitStructure.DVP_JPEGMode = DISABLE;
    DVP_InitStructure.DVP_CaptureMode = DVP_CaptureMode_Continuous;
    DVP_Init(&DVP_InitStructure);

    DVP_ReceiveCircuitResetCmd(DISABLE);
    DVP_FIFO_ResetCmd(DISABLE);

    NVIC_EnableIRQ(DVP_IRQn);
    NVIC_SetPriority(DVP_IRQn, 0);

    DVP_ITConfig(DVP_IT_STR_FRM | DVP_IT_ROW_DONE | DVP_IT_FRM_DONE | DVP_IT_FIFO_OV | DVP_IT_STP_FRM, ENABLE);

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
    if(DVP_GetITStatus(DVP_IT_ROW_DONE) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_ROW_DONE);

        if(href_cnt % 2)
        {
            /* Send DVP data to LCD */
            DMA_Cmd(DMA2_Channel5, DISABLE);    
#if(CAMERA_SELECT == OV2640_Module)
            DMA_SetCurrDataCounter(DMA2_Channel5, OV2640_RGB565_WIDTH * 2);
            DMA2_Channel5->PADDR = (uint32_t)(ImageDataOv2640);
#elif(CAMERA_SELECT == OV5640_Module)
            DMA_SetCurrDataCounter(DMA2_Channel5, OV5640_RGB565_WIDTH * 2);
            DMA2_Channel5->PADDR = (uint32_t)(ImageDataOv5640);
#endif     
            DMA_Cmd(DMA2_Channel5, ENABLE);
        }
        href_cnt++;
    }

    if(DVP_GetITStatus(DVP_IT_FRM_DONE) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_FRM_DONE);
        href_cnt = 0;
    }

    if(DVP_GetITStatus(DVP_IT_STR_FRM) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_STR_FRM);
        frame_cnt++;
    }

    if(DVP_GetITStatus(DVP_IT_STP_FRM) != RESET)
    {
        DVP_ClearITPendingBit(DVP_IT_STP_FRM);
    }

    if(DVP_GetITStatus(DVP_IT_FIFO_OV) != RESET)
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
	USART_Printf_Init(115200);	
	printf("SystemClk:%d\r\n",SystemCoreClock);
	printf( "ChipID:%08x\r\n", DBGMCU_GetCHIPID() );

#if(CAMERA_SELECT == OV2640_Module)
    LCD_Reset_GPIO_Init();
    LCD_Init();   
   while(OV2640_Init())
    {
        printf("Camera Initialize failed.\r\n");
        Delay_Ms(1000);
    }
    printf("Camera Initialize Success.\r\n");

    Delay_Ms(1000);

    OV2640_RGB565_Mode_Init();
    Delay_Ms(1000);   

    printf("RGB565 Mode...\r\n");

    DMA_SRAMLCD_Init((u32)ImageDataOv2640);
    DVP_Function_Init();
#endif

#if(CAMERA_SELECT == OV5640_Module)
    uint8_t tempstr[50]; 
    LCD_Reset_GPIO_Init();
    LCD_Init();   
    LCD_Display_Dir(1);
    
    sprintf ((char*)tempstr,"height: %d  width: %d  id: %#x\n", lcddev.height, lcddev.width, lcddev.id);
    LCD_ShowString(0,120-12,320,48,16,tempstr);

    LCD_SetCursor (0, 0);
    LCD_WriteRAM_Prepare();

    while(OV5640_Init()) {
        printf("Camera Initialize failed.\r\n");
        Delay_Ms(1000);
    }
    printf("Camera Initialize Success.\r\n");

    OV5640_RGB565_Mode_Init();
    Delay_Ms(500);
    printf("RGB565 Mode...\r\n");
    DMA_SRAMLCD_Init((uint32_t)ImageDataOv5640);
    DVP_Function_Init();
#endif

    while(1);
}



