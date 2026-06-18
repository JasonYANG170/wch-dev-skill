/********************************** (C) COPYRIGHT  *******************************
* File Name          : main.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/03/18
* Description        : Main program body
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

/*
 *@Note WS2812B LED output routine:
*/
#include "debug.h"

#include "math.h"

/* Global define */
#define ARGB_MS2HBTICK(ms) ((uint32_t)((float)SystemCoreClock / 1000.0f * (float)ms))
#define ARGB_US2HBTICK(us) ((uint32_t)((float)SystemCoreClock / 1000000.0f * (float)us))
#define ARGB_NS2HBTICK(ns) ((uint32_t)((float)SystemCoreClock / 1000000000.0f * (float)ns))

#define ARGB_WS2812_NUM 64

#pragma pack(push, 1)

typedef union
{
    struct
    {
        uint8_t G;
        uint8_t R;
        uint8_t B;
    } ColorDecompose;

    uint8_t Data[3];
} WS2812_RGBData_u;

#pragma pack(pop)

typedef struct
{
    float H;
    float S;
    float V;
} WS2812_HSVData_t;

/* Global Variable */
float WS2812_BaseHue = 0.0, WS2812_BaseStep = 1.0, WS2812_BaseValue = 0.069;
WS2812_RGBData_u WS2812_Data[ARGB_WS2812_NUM];

/*********************************************************************
 * @fn      hsv2rgb
 *
 * @brief   The function hsv2rgb converts HSV color data to RGB color data.
 *
 * @param   hsv - HSV color data structure
 *
 * @return  RGB color data structure
 */
WS2812_RGBData_u hsv2rgb(WS2812_HSVData_t hsv)
{
    WS2812_RGBData_u rgb;  // RGB color data structure

    // Check if value is 0, set RGB to black
    if (hsv.V <= 0.0f) {
        rgb.ColorDecompose.R = rgb.ColorDecompose.G = rgb.ColorDecompose.B = 0;
        return rgb;
    }
    // Clamp value to maximum of 1.0
    else if (hsv.V > 1.0f) {
        hsv.V = 1.0f;
    }

    // Normalize hue value to 0-360 range
    while (hsv.H >= 360.0f)
        hsv.H -= 360.0f;
    while (hsv.H < 0.0f)
        hsv.H += 360.0f;

    // Check if saturation is 0, create grayscale color
    if (hsv.S <= 0.0f) {
        rgb.ColorDecompose.R = rgb.ColorDecompose.G = rgb.ColorDecompose.B = (uint8_t)(hsv.V * 255.0f);
        return rgb;
    }
    // Clamp saturation to maximum of 1.0
    else if (hsv.S > 1.0f) {
        hsv.S = 1.0f;
    }

    // Calculate HSV to RGB conversion parameters
    float C = hsv.V * hsv.S;                                          // Chroma
    float X = C * (1.0f - fabsf(fmodf(hsv.H / 60.0f, 2.0f) - 1.0f));  // Second largest component
    float M = hsv.V - C;                                              // Value adjustment

    // Convert HSV to RGB based on hue sector
    if (hsv.H < 60.0f)  // Red to Yellow
    {
        rgb.ColorDecompose.R = (uint8_t)((C + M) * 255.0f);
        rgb.ColorDecompose.G = (uint8_t)((X + M) * 255.0f);
        rgb.ColorDecompose.B = (uint8_t)(M * 255.0f);
    }
    else if (hsv.H < 120.0f)  // Yellow to Green
    {
        rgb.ColorDecompose.R = (uint8_t)((X + M) * 255.0f);
        rgb.ColorDecompose.G = (uint8_t)((C + M) * 255.0f);
        rgb.ColorDecompose.B = (uint8_t)(M * 255.0f);
    }
    else if (hsv.H < 180.0f)  // Green to Cyan
    {
        rgb.ColorDecompose.R = (uint8_t)(M * 255.0f);
        rgb.ColorDecompose.G = (uint8_t)((C + M) * 255.0f);
        rgb.ColorDecompose.B = (uint8_t)((X + M) * 255.0f);
    }
    else if (hsv.H < 240.0f)  // Cyan to Blue
    {
        rgb.ColorDecompose.R = (uint8_t)(M * 255.0f);
        rgb.ColorDecompose.G = (uint8_t)((X + M) * 255.0f);
        rgb.ColorDecompose.B = (uint8_t)((C + M) * 255.0f);
    }
    else if (hsv.H < 300.0f)  // Blue to Magenta
    {
        rgb.ColorDecompose.R = (uint8_t)((X + M) * 255.0f);
        rgb.ColorDecompose.G = (uint8_t)(M * 255.0f);
        rgb.ColorDecompose.B = (uint8_t)((C + M) * 255.0f);
    }
    else  // Magenta to Red
    {
        rgb.ColorDecompose.R = (uint8_t)((C + M) * 255.0f);
        rgb.ColorDecompose.G = (uint8_t)(M * 255.0f);
        rgb.ColorDecompose.B = (uint8_t)((X + M) * 255.0f);
    }

    return rgb;
}

void ARGB_GPIO_Init()
{
    GPIO_InitTypeDef GPIO_InitStructure;

    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/*********************************************************************
 * @fn      ARGB_WS2812B_DMA_Init
 *
 * @brief   The function ARGB_WS2812B_DMA_Init initializes the DMA for WS2812B LED control.
 *
 * @param   ARGB_DMA_Buffer - Pointer to the ARGB data buffer
 *          buffer_size - Size of the ARGB data buffer
 *
 * @return  none
 */
void ARGB_WS2812B_DMA_Init(uint8_t *ARGB_DMA_Buffer, size_t buffer_size)
{
    RCC_HBPeriphClockCmd(RCC_HBPeriph_DMA1, ENABLE);

    DMA_InitTypeDef DMA_InitStructure;

    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ARGB->DATAR);
    DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t)ARGB_DMA_Buffer;
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize         = buffer_size;
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority           = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M                = DMA_M2M_Disable;
    DMA_InitStructure.DMA_BufferMode         = DMA_SingleBufferMode;
    DMA_Init(DMA1_Channel10, &DMA_InitStructure);
    DMA_Cmd(DMA1_Channel10, ENABLE);
}

/*********************************************************************
 * @fn      ARGB_WS2812B_Init
 *
 * @brief   The function ARGB_WS2812B_Init initializes the ARGB peripheral for WS2812B LED control.
 *
 * @param   ARGB_DMA_Buffer - Pointer to the ARGB data buffer
 *          buffer_size - Size of the ARGB data buffer
 *
 * @return  none
 */
void ARGB_WS2812B_Init(uint8_t *ARGB_DMA_Buffer, size_t buffer_size)
{
    // Enable the clock for ARGB peripheral
    RCC_HBPeriphClockCmd(RCC_HBPeriph_ARGB, ENABLE);

    // Initialize ARGB configuration structure
    ARGB_InitTypeDef ARGB_InitStruct;

    // Configure ARGB parameters
    ARGB_InitStruct.ARGB_Length     = buffer_size - 1;         // Set buffer length (minus 1 for zero-based indexing)
    ARGB_InitStruct.ARGB_T1H        = ARGB_US2HBTICK(0.9f);    // Set high time for '1' bit (0.9��s)
    ARGB_InitStruct.ARGB_T0H        = ARGB_US2HBTICK(0.3f);    // Set high time for '0' bit (0.3��s)
    ARGB_InitStruct.ARGB_DataPeriod = ARGB_US2HBTICK(1.45f);   // Set data bit period (1.45��s)
    ARGB_InitStruct.ARGB_RSTPeriod  = ARGB_US2HBTICK(80.0f);   // Set reset period (80��s)
    ARGB_InitStruct.ARGB_Mode       = ARGB_Mode_SendRSTFirst;  // Set mode to send reset first
    ARGB_InitStruct.ARGB_Endian     = ARGB_Endian_MSB;         // Set most significant bit first
    ARGB_Init(&ARGB_InitStruct);                          // Apply ARGB configuration

    // Enable ARGB interrupt for frame reception complete
    ARGB_ITConfig(ARGB_IT_FRH, ENABLE);
    // Enable ARGB interrupt in the NVIC
    NVIC_EnableIRQ(ARGB_IRQn);

    ARGB_WS2812B_DMA_Init((void *)ARGB_DMA_Buffer, buffer_size);
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
    printf("SystemClk:%d\r\n", SystemCoreClock);
    printf("ChipID:%08x\r\n", DBGMCU_GetCHIPID());

    printf("WS2812 Test\r\n");
    ARGB_GPIO_Init();
    // Initialize WS2812B LED strip with data buffer size
    ARGB_WS2812B_Init((void *)WS2812_Data, sizeof(WS2812_Data));

    // Enable DMA and ARGB controller
    ARGB_DMACmd(ENABLE);
    ARGB_Cmd(ENABLE);
    
    while (1) {
    }
}

void __attribute__((interrupt("WCH-Interrupt-fast"))) ARGB_IRQHandler(void)
{
    // Check if the "FreshDone" interrupt flag is set
    if (ARGB_GetITStatus(ARGB_IT_FRH) != RESET) {
        // Clear the "FreshDone" interrupt flag
        ARGB_ClearITPendingBit(ARGB_IT_FRH);
        // Increment the base hue for color shifting effect
        WS2812_BaseHue += 1.0f;
        if (WS2812_BaseHue > 360.0f)
            WS2812_BaseHue = 0.0f;
        // Create HSV color structure
        WS2812_HSVData_t hsv;
        // Update each LED's color in the strip
        for (size_t i = 0; i < ARGB_WS2812_NUM; i++) {
            // Calculate hue based on position with radial pattern
            hsv.H          = WS2812_BaseHue + 360.0f / (float)(ARGB_WS2812_NUM) * (float)i;
            // Set saturation and value
            hsv.S          = WS2812_BaseStep;

            hsv.V          = WS2812_BaseValue;
            // Convert HSV to RGB and store in data array
            WS2812_Data[i] = hsv2rgb(hsv);
        }

        // Refresh DMA transfer with new data
        DMA_Cmd(DMA1_Channel10, DISABLE);                             // Disable DMA
        DMA_SetCurrDataCounter(DMA1_Channel10, sizeof(WS2812_Data));  // Set data size
        DMA_Cmd(DMA1_Channel10, ENABLE);                              // Enable DMA
    }
}
