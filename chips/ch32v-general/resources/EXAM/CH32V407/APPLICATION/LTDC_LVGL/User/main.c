/********************************** (C) COPYRIGHT *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/12/01
* Description        : Main program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note
  LTDC routine
*/

#include "string.h"
#include "debug.h"
#include "lvgl.h"
#include "lv_port_disp.h"
#include "demos\benchmark\lv_demo_benchmark.h"

/* Global define */

/* Global Variable */

/*********************************************************************
 * @fn      LTDC_GPIO_Config
 * 
 * @brief   Configure the GPIO pins
 * 
 * @param   none
 */

void LTDC_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_AFIO, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOA, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOB, ENABLE);
    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOC, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_LTDC, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_Disable, ENABLE);

    // LTDC_CLK PA10
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_HSYNC PB8
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_VSYNC PB9
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_DE PA9
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_R0 PC0
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // LTDC_R1 PC1
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // LTDC_R2 PC2
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // LTDC_R3 PC3
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // LTDC_R4 PA1
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_R5 PA2
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_R6 PA3
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_R7 PA7
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_G0 PC4
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // LTDC_G1 PC5
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // LTDC_G2 PB0
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_G3 PB1
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_G4 PB3
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_G5 PA4
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_4;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_G6 PB12
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_G7 PB13
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_B0 PB14
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_B1 PB15
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_B2 PB11
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_B3 PB10
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // LTDC_B4 PA8
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_B5 PA13
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_B6 PA14
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // LTDC_B7 PA15
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // BL
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOE, &GPIO_InitStructure);

    GPIO_SetBits(GPIOE, GPIO_Pin_9);
}

#define HBP               (43)
#define VBP               (12)
#define HSW               (4)
#define VSW               (4)
#define HFP               (8)
#define VFP               (8)

#define LCD_Width         (480)
#define LCD_Height        (272)

#define layer1_w          (240)
#define layer1_h          (240)

#define layer1_color_mode LTDC_Pixelformat_RGB565

#define layer1_pixel_size 2

uint8_t layer1[layer1_w * layer1_h * layer1_pixel_size];

/*********************************************************************
 * @fn      argb8888
 *
 * @brief   Combine the four color  into a 32-bit
 * 
 * @param   a - alpha value
 *           r - red value  
 *           g - green value
 *           b - blue value
 * 
 * @return  32-bit color value
 */
static inline uint32_t argb8888(uint8_t a, uint8_t r, uint8_t g, uint8_t b)
{
    return (a << 24) | (r << 16) | (g << 8) | b;
}

/*********************************************************************
 * @fn      rgb_to_rgb565
 * 
 * @brief   Convert 32-bit RGB color to 16-bit RGB565 color
 * 
 * @param   color - 32-bit RGB color
 * 
 * @return  16-bit RGB565 color
 */
static inline uint16_t rgb_to_rgb565(uint32_t color)
{
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | ((b & 0xF8) >> 3);
}

/*********************************************************************
 * @fn      Drawrect_rgb565
 * 
 * @brief   Draw a rectangle with RGB565 color on the image
 * 
 * @param   color - 32-bit RGB color
 *           x - x coordinate of the top-left point of the rectangle
 *           y - y coordinate of the top-left point of the rectangle
 *           w - width of the rectangle
 *           h - height of the rectangle
 *           img_w - width of the image
 *           img_h - height of the image
 *           img - pointer to the image buffer
 * 
 * @return  none
 */
void Drawrect_rgb565(uint32_t color, uint32_t x, uint32_t y, uint32_t w, uint32_t h, uint32_t img_w,
                     uint32_t img_h, uint8_t* img)
{

    uint32_t end_x = (x + w > img_w) ? img_w : x + w;
    uint32_t end_y = (y + h > img_h) ? img_h : y + h;

    uint16_t rgb565_color = rgb_to_rgb565(color);
    uint8_t  rgb565_l     = (rgb565_color >> 8) & 0xFF;
    uint8_t  rgb565_h     = rgb565_color & 0xFF;

    for (uint32_t j = y; j < end_y; ++j)
    {
        for (uint32_t i = x; i < end_x; ++i)
        {
            uint32_t index = (j * img_w + i) * 2;

            img[index]     = rgb565_h;
            img[index + 1] = rgb565_l;
        }
    }
}

/*********************************************************************
 * @fn      argb_to_argb4444
 * 
 * @brief   Convert 32-bit ARGB color to 16-bit ARGB4444 color
 * 
 * @param   color - 32-bit ARGB color
 * 
 * @return  16-bit ARGB4444 color
 */
uint16_t argb_to_argb4444(uint32_t color)
{
    uint8_t a = (color >> 24) & 0xFF;
    uint8_t r = (color >> 16) & 0xFF;
    uint8_t g = (color >> 8) & 0xFF;
    uint8_t b = color & 0xFF;
    return ((a >> 4) << 12) | ((r >> 4) << 8) | ((g >> 4) << 4) | (b >> 4);
}

/*********************************************************************
 * @fn      Drawrect_argb4444
 * 
 * @brief   Draw a rectangle with ARGB4444 color on the image
 * 
 * @param   color - 32-bit ARGB color
 *           x - x coordinate of the top-left point of the rectangle
 *           y - y coordinate of the top-left point of the rectangle
 *           w - width of the rectangle
 *           h - height of the rectangle
 *           img_w - width of the image
 *           img_h - height of the image
 *           img - pointer to the image buffer
 * 
 * @return  none
 */
void Drawrect_argb4444(uint32_t color, uint32_t x, uint32_t y, uint32_t w, uint32_t h,
                       uint32_t img_w, uint32_t img_h, uint8_t* img)
{

    uint32_t end_x = (x + w > img_w) ? img_w : x + w;
    uint32_t end_y = (y + h > img_h) ? img_h : y + h;

    uint16_t argb4444_color = argb_to_argb4444(color);

    uint8_t argb4444_h = (argb4444_color >> 8) & 0xFF;
    uint8_t argb4444_l = argb4444_color & 0xFF;

    for (uint32_t j = y; j < end_y; ++j)
    {
        for (uint32_t i = x; i < end_x; ++i)
        {
            uint32_t index = (j * img_w + i) * 2;

            img[index]     = argb4444_l;
            img[index + 1] = argb4444_h;
        }
    }
}

/*********************************************************************
 * @fn      LCD_Config
 * 
 * @brief   LCD configuration.
 *
 * @return  none
 */
static void LCD_Config(void)
{

    LTDC_InitTypeDef       LTDC_InitStruct        = {0};
    LTDC_Layer_InitTypeDef LTDC_Layer_1InitStruct = {0};

    RCC_HBPeriphClockCmd(RCC_HBPeriph_LTDC, ENABLE);

    LTDC_InitStruct.LTDC_HSPolarity = LTDC_HSPolarity_AH;
    LTDC_InitStruct.LTDC_VSPolarity = LTDC_VSPolarity_AL;
    LTDC_InitStruct.LTDC_DEPolarity = LTDC_DEPolarity_AH;
    LTDC_InitStruct.LTDC_PCPolarity = LTDC_PCPolarity_IIPC;

    LTDC_InitStruct.LTDC_HorizontalSync     = HSW - 1;
    LTDC_InitStruct.LTDC_VerticalSync       = VSW - 1;
    LTDC_InitStruct.LTDC_AccumulatedHBP     = HBP + HSW - 1;
    LTDC_InitStruct.LTDC_AccumulatedVBP     = VBP + VSW - 1;
    LTDC_InitStruct.LTDC_AccumulatedActiveW = LCD_Width + HSW + HBP - 1;
    LTDC_InitStruct.LTDC_AccumulatedActiveH = LCD_Height + VSW + VBP - 1;
    LTDC_InitStruct.LTDC_TotalWidth         = LCD_Width + HSW + HBP + HFP - 1;
    LTDC_InitStruct.LTDC_TotalHeigh         = LCD_Height + VSW + VBP + VFP - 1;

    LTDC_InitStruct.LTDC_BackgroundRedValue   = 0xff;
    LTDC_InitStruct.LTDC_BackgroundGreenValue = 0;
    LTDC_InitStruct.LTDC_BackgroundBlueValue  = 0;

    LTDC_Init(&LTDC_InitStruct);

    // layer1
    LTDC_Layer_1InitStruct.LTDC_HorizontalStart = 0;
    LTDC_Layer_1InitStruct.LTDC_HorizontalStop  = layer1_w;
    LTDC_Layer_1InitStruct.LTDC_VerticalStart   = 0;
    LTDC_Layer_1InitStruct.LTDC_VerticalStop    = layer1_h;

    LTDC_Layer_1InitStruct.LTDC_PixelFormat = layer1_color_mode;

    LTDC_Layer_1InitStruct.LTDC_DefaultColorBlue  = 0;
    LTDC_Layer_1InitStruct.LTDC_DefaultColorGreen = 0;
    LTDC_Layer_1InitStruct.LTDC_DefaultColorRed   = 0;
    LTDC_Layer_1InitStruct.LTDC_DefaultColorAlpha = 0;

    LTDC_Layer_1InitStruct.LTDC_CFBStartAdress = (uint32_t)layer1;

    LTDC_Layer_1InitStruct.LTDC_CFBLineLength = ((layer1_w * layer1_pixel_size) + 31);

    LTDC_Layer_1InitStruct.LTDC_CFBPitch = (layer1_w * layer1_pixel_size);

    LTDC_Layer_1InitStruct.LTDC_CFBLineNumber = layer1_h;

    LTDC_LayerInit(LTDC_Layer, &LTDC_Layer_1InitStruct);

    LTDC_LayerCmd(LTDC_Layer, ENABLE);

    LTDC_ReloadConfig(LTDC_IMReload);

    LTDC_Cmd(ENABLE);
}

/*********************************************************************
 * @fn      ETH_PLL_CLK_Enalbe
 * 
 * @brief   PLL configuration.
 *
 * @return  none
 */
void ETH_PLL_CLK_Enalbe(void)
{

    RCC->CTLR &= ~(1 << 22);
    RCC->CTLR |= (1 << 22);
    while ((RCC->CTLR & (uint32_t)RCC_ETHPLLRDY) != (uint32_t)RCC_ETHPLLRDY)
    {
    }
}

/*********************************************************************
 * @fn      TIM4_init
 * 
 * @brief   TIM configuration.
 *
 * @return  none
 */
void TIM4_init()
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure = {0};

    RCC_PB1PeriphClockCmd(RCC_PB1Periph_TIM4, ENABLE);
    TIM_Cmd(TIM4, DISABLE);

    TIM_TimeBaseStructure.TIM_Period        = 500;
    TIM_TimeBaseStructure.TIM_Prescaler     = SystemCoreClock / 100000;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode   = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);

    TIM_ClearFlag(TIM4, TIM_FLAG_Update);
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);

    NVIC_EnableIRQ(TIM4_IRQn);
    TIM_Cmd(TIM4, ENABLE);
}

int main(void)
{

    SystemCoreClockUpdate();
    USART_Printf_Init(115200);
    Delay_Init();
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());
    printf("LTDC_Display\n");
    printf(__TIME__ "\n");

    Delay_Ms(500);
    Delay_Ms(500);
    Delay_Ms(500);

    LTDC_GPIO_Config();

    // LTDC CLK Settings
    // In this routine, the CLK required for the screen is 10M and the PLL is 500M, so divide by 50

    RCC->CFGR2 &= ~(0x7F);

    ETH_PLL_CLK_Enalbe();
    RCC->CFGR2 |= (1 << 6);  // use eth_pll
    RCC->CFGR2 |= (55);      // div 50

    LCD_Config();

    TIM4_init();
  
    lv_init();
    lv_port_disp_init();

    lv_demo_benchmark();
    // lv_demo_widgets();

    while (1)
    {
        lv_task_handler();
    }
}

void TIM4_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void TIM4_IRQHandler(void)
{
    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    lv_tick_inc(5);
}