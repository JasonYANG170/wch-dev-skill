/********************************** (C) COPYRIGHT  *******************************
 * File Name          : hardware.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/03/30
 * Description        : This file provides all the hardware firmware functions.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include "hardware.h"
#include "ov5640.h"
#include "ov2640.h"
#include "string.h"

/* Camera module selection */
#define OV2640_Module 0
#define OV5640_Module 1
#define CAMERA_SELECT OV5640_Module

#if ( Func_Run_V3F )
#if (CAMERA_SELECT == OV2640_Module)
__attribute__ ((aligned (32))) uint16_t ImageDataOv2640_a[OV2640_RGB565_WIDTH] = {0};
__attribute__ ((aligned (32))) uint16_t ImageDataOv2640_b[OV2640_RGB565_WIDTH] = {0};
#elif (CAMERA_SELECT == OV5640_Module)
__attribute__ ((aligned (32))) uint16_t ImageDataOv5640_a[OV5640_RGB565_WIDTH] = {0};
__attribute__ ((aligned (32))) uint16_t ImageDataOv5640_b[OV5640_RGB565_WIDTH] = {0};
#endif
#endif

/* LTDC definition */
#define HBP (43)
#define VBP (12)
#define HSW (4)
#define VSW (4)
#define HFP (8)
#define VFP (8)

#define LCD_Width  (480)
#define LCD_Height (272)

#define layer1_w (480)
#define layer1_h (272)

#define layer1_color_mode LTDC_Pixelformat_RGB565
#define layer1_pixel_size 2

volatile UINT32 frame_cnt = 0;
volatile UINT32 href_cnt = 0;
volatile UINT32 addr_cnt = 0;
volatile UINT32 line_cnt = 0;
volatile UINT32 addrx = 0;

__attribute__ ((aligned (32))) uint8_t layer1[(layer1_w * layer1_h * layer1_pixel_size)];

void DVP_IRQHandler (void) __attribute__ ((interrupt ("WCH-Interrupt-fast")));


#if ( Func_Run_V3F )
/*********************************************************************
 * @fn      LTDC_GPIO_Config
 *
 * @brief   Configure the LTDC GPIO pins
 *
 * @param   none
 */
static void LTDC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_HB1PeriphClockCmd(RCC_HB1Periph_PWR, ENABLE);
    PWR_VIO18ModeCfg(PWR_VIO18CFGMODE_SW);
    PWR_VIO18LevelCfg(PWR_VIO18Level_MODE3);

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO,  ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOD, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOE, ENABLE);
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOF, ENABLE);

    // CLK PF1(AF14)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource1, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // HSYNC PC10(AF15)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource10, GPIO_AF15);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // VSYNC PC11(AF15)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource11, GPIO_AF15);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // DE PC5(AF14)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource5, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // R[3] PD0(AF15)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource0, GPIO_AF15);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // R[4] PA5(AF14)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // R[5] PA9(AF14)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // R[6] PA8(AF14)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource8, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // R[7] PD4(AF15)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource4, GPIO_AF15);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // G[2] PA6(AF14)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // G[3] PD6(AF15)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource6, GPIO_AF15);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // G[4] PB10(AF14)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // G[5] PC1(AF14)
    GPIO_PinAFConfig(GPIOC, GPIO_PinSource1, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // G[6] PF4(AF15)
    GPIO_PinAFConfig(GPIOF, GPIO_PinSource4, GPIO_AF15);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOF, &GPIO_InitStructure);

    // G[7] PB15(AF14)
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource15, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // B[3] PD7(AF14)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource7, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // B[4] PE0(AF9)
    GPIO_PinAFConfig(GPIOE, GPIO_PinSource0, GPIO_AF9);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    // B[5] PA3(AF14)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // B[6] PA15(AF14)
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource15, GPIO_AF14);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // B[7] PD2(AF9)
    GPIO_PinAFConfig(GPIOD, GPIO_PinSource2, GPIO_AF9);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    // BL
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Very_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    GPIO_SetBits(GPIOB, GPIO_Pin_5);
}
#endif

#if ( Func_Run_V3F )
/*********************************************************************
 * @fn      LCD_Config
 *
 * @brief   LCD configuration.
 *
 * @return  none
 */
static void LCD_Config(void)
{
    LTDC_InitTypeDef LTDC_InitStruct = {0};
    LTDC_Layer_InitTypeDef LTDC_Layer_1InitStruct = {0};

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_LTDC, ENABLE);

    LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AH;
    LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;
    LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AH;
    LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IIPC;
    LTDC_InitStruct.LTDC_HorizontalSync = HSW - 1;
    LTDC_InitStruct.LTDC_VerticalSync = VSW - 1;
    LTDC_InitStruct.LTDC_AccumulatedHBP = HBP + HSW - 1;
    LTDC_InitStruct.LTDC_AccumulatedVBP = VBP + VSW - 1;
    LTDC_InitStruct.LTDC_AccumulatedActiveW = LCD_Width + HSW + HBP - 1;
    LTDC_InitStruct.LTDC_AccumulatedActiveH = LCD_Height + VSW + VBP - 1;
    LTDC_InitStruct.LTDC_TotalWidth = LCD_Width + HSW + HBP + HFP - 1;
    LTDC_InitStruct.LTDC_TotalHeigh = LCD_Height + VSW + VBP + VFP - 1;
    LTDC_InitStruct.LTDC_BackgroundRedValue = 0xff;
    LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;
    LTDC_InitStruct.LTDC_BackgroundBlueValue = 0;
    LTDC_Init(&LTDC_InitStruct);

    // layer1
    LTDC_Layer_1InitStruct.LTDC_HorizontalStart = 0;
    LTDC_Layer_1InitStruct.LTDC_HorizontalStop = layer1_w;
    LTDC_Layer_1InitStruct.LTDC_VerticalStart = 0;
    LTDC_Layer_1InitStruct.LTDC_VerticalStop = layer1_h;
    LTDC_Layer_1InitStruct.LTDC_PixelFormat = layer1_color_mode;
    LTDC_Layer_1InitStruct.LTDC_ConstantAlpha = 0xff;

    LTDC_Layer_1InitStruct.LTDC_DefaultColorBlue = 0;
    LTDC_Layer_1InitStruct.LTDC_DefaultColorGreen = 0;
    LTDC_Layer_1InitStruct.LTDC_DefaultColorRed = 0;
    LTDC_Layer_1InitStruct.LTDC_DefaultColorAlpha = 0;

    LTDC_Layer_1InitStruct.LTDC_BlendingFactor_1 = LTDC_BlendingFactor1_CA;
    LTDC_Layer_1InitStruct.LTDC_BlendingFactor_2 = LTDC_BlendingFactor2_CA;
    LTDC_Layer_1InitStruct.LTDC_CFBStartAdress = (uint32_t)layer1;
    LTDC_Layer_1InitStruct.LTDC_CFBLineLength = ((layer1_w * layer1_pixel_size) + 31);
    LTDC_Layer_1InitStruct.LTDC_CFBPitch = (layer1_w * layer1_pixel_size);
    LTDC_Layer_1InitStruct.LTDC_CFBLineNumber = layer1_h;
    LTDC_LayerInit(LTDC_Layer1, &LTDC_Layer_1InitStruct);

    LTDC_LayerCmd(LTDC_Layer1, ENABLE);
    LTDC_ReloadConfig(LTDC_IMReload);
    LTDC_Cmd(ENABLE);

}
#endif

#if ( Func_Run_V3F )
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
#if (CAMERA_SELECT == OV2640_Module)
    DVP_InitStructure.DVP_COL_NUM = OV2640_RGB565_WIDTH * 2;
    DVP_InitStructure.DVP_ROW_NUM = OV2640_RGB565_HEIGHT;
#elif (CAMERA_SELECT == OV5640_Module)
    DVP_InitStructure.DVP_COL_NUM = OV5640_RGB565_WIDTH * 2;
    DVP_InitStructure.DVP_ROW_NUM = OV5640_RGB565_HEIGHT;
#endif
    DVP_InitStructure.DVP_HCLK_P = DVP_Hclk_P_Rising;
    DVP_InitStructure.DVP_HSYNC_P = DVP_Hsync_P_High;
    DVP_InitStructure.DVP_VSYNC_P = DVP_Vsync_P_High;
#if (CAMERA_SELECT == OV2640_Module)
    DVP_InitStructure.DVP_DMA_BUF0_Addr = (uint32_t)ImageDataOv2640_a;
    DVP_InitStructure.DVP_DMA_BUF1_Addr = (uint32_t)ImageDataOv2640_b;
#elif (CAMERA_SELECT == OV5640_Module)
    DVP_InitStructure.DVP_DMA_BUF0_Addr = (uint32_t)ImageDataOv5640_a;
    DVP_InitStructure.DVP_DMA_BUF1_Addr = (uint32_t)ImageDataOv5640_b;
#endif
    DVP_InitStructure.DVP_FrameCapRate = DVP_FrameCapRate_100P;
    DVP_InitStructure.DVP_JPEGMode = DISABLE;
    DVP_Init(&DVP_InitStructure);

    DVP_ReceiveCircuitResetCmd(DISABLE);
    DVP_FIFO_ResetCmd(DISABLE);

    NVIC_EnableIRQ(DVP_IRQn);
    NVIC_SetPriority(DVP_IRQn, 0);

    DVP_ITConfig(DVP_IT_STR_FRM | DVP_IT_ROW_DONE | DVP_IT_FRM_DONE | DVP_IT_FIFO_OV | DVP_IT_STP_FRM, ENABLE);

    DVP_DMACmd(ENABLE);
    DVP_Cmd(ENABLE);
}
#endif

/*********************************************************************
 * @fn      Hardware
 *
 * @brief   dvp function
 *
 * @return  none
 */
void Hardware(void) 
{
#if ( Func_Run_V3F )
#if (CAMERA_SELECT == OV2640_Module)
    
    LTDC_GPIO_Config();
    RCC_LTDCCLKConfig (RCC_LTDCClockSource_PLL);
    RCC_LTDCClockSourceDivConfig (RCC_LTDCClockSource_Div40);
    LCD_Config();

    while(OV2640_Init()) {
        printf("Camera Initialize failed.\r\n");
        Delay_Ms(1000);
    }
    printf("Camera Initialize Success.\r\n");
    OV2640_RGB565_Mode_Init();
    Delay_Ms(500);
    printf("RGB565 Mode...\r\n");
    DVP_Function_Init();
#endif

#if (CAMERA_SELECT == OV5640_Module)

    LTDC_GPIO_Config();
    RCC_LTDCCLKConfig(RCC_LTDCClockSource_PLL);
    RCC_LTDCClockSourceDivConfig(RCC_LTDCClockSource_Div40);
    LCD_Config();

    while(OV5640_Init()) {
        printf("Camera Initialize failed.\r\n");
        Delay_Ms(1000);
    }
    printf("Camera Initialize Success.\r\n");

    OV5640_RGB565_Mode_Init();
    Delay_Ms(500);
    printf("RGB565 Mode...\r\n");

    DVP_Function_Init();
#endif

    while(1);

#endif
}

#if ( Func_Run_V3F )
/*********************************************************************
 * @fn      DVP_IRQHandler
 *
 * @brief   This function handles DVP exception.
 *
 * @return  none
 */
void DVP_IRQHandler(void)
{
    if (DVP_GetITStatus(DVP_IT_ROW_DONE) != RESET) {
        DVP_ClearITPendingBit(DVP_IT_ROW_DONE);

        addrx = layer1_w * layer1_pixel_size * line_cnt;

        if (addr_cnt % 2) {
            addr_cnt++;
#if(CAMERA_SELECT == OV2640_Module)
            memcpy(&layer1[addrx], (void *)ImageDataOv2640_a, layer1_w * layer1_pixel_size);
#elif(CAMERA_SELECT == OV5640_Module)
            memcpy(&layer1[addrx], (void *)ImageDataOv5640_a, layer1_w * layer1_pixel_size);
#endif
        } else {
            addr_cnt++;
#if(CAMERA_SELECT == OV2640_Module)
            memcpy(&layer1[addrx], (void *)ImageDataOv2640_b, layer1_w * layer1_pixel_size);
#elif(CAMERA_SELECT == OV5640_Module)
            memcpy(&layer1[addrx], (void *)ImageDataOv5640_b, layer1_w * layer1_pixel_size);
#endif
        }  
              line_cnt++;

        if (line_cnt > (layer1_h-1 )) {
            line_cnt = 0;
        }
        href_cnt++;
    }

    if (DVP_GetITStatus(DVP_IT_FRM_DONE) != RESET) {
        DVP_ClearITPendingBit(DVP_IT_FRM_DONE);
        addr_cnt = 0;
        href_cnt = 0;
    }

    if (DVP_GetITStatus(DVP_IT_STR_FRM) != RESET) {
        DVP_ClearITPendingBit(DVP_IT_STR_FRM);
        frame_cnt++;
    }

    if (DVP_GetITStatus(DVP_IT_STP_FRM) != RESET) {
        DVP_ClearITPendingBit(DVP_IT_STP_FRM);
    }

    if (DVP_GetITStatus(DVP_IT_FIFO_OV) != RESET) {
        DVP_ClearITPendingBit(DVP_IT_FIFO_OV);
    }
}
#endif