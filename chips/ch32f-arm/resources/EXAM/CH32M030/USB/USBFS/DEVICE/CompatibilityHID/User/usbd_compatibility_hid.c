/********************************** (C) COPYRIGHT *******************************
 * File Name  :usbd_compatibility_hid.c
 * Author     :OWNER
 * Version    : v0.01
 * Date       : 2022骞�7鏈�8鏃�
 * Description:
*******************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include <ch32m030_usbfs_device.h>
#include "debug.h"
#include "string.h"
#include "usbd_compatibility_hid.h"

void TIM2_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

__attribute__ ((aligned(4))) uint8_t UART1_RxBuffer[DEF_UART1_BUF_SIZE];  // UART1 Rx Buffer
__attribute__ ((aligned(4))) uint8_t  HID_Report_Buffer[64];              // HID Report Buffer
volatile uint8_t HID_Set_Report_Flag = SET_REPORT_DEAL_OVER;               // HID SetReport flag

volatile uint16_t UART1_TimeOut;                                           // UART1 RX timeout flag
volatile uint8_t  UART1_Tx_Flag = 0;                                       // UART1 TX flag

volatile uint16_t UART1_RX_CurCnt = 0;                                     // UART1 DMA current remain count
volatile uint16_t UART1_RX_LastCnt = 0;                                    // UART1 DMA last remain count
volatile uint16_t UART1_Rx_RemainLen = 0;                                  // UART1 RX data remain len
volatile uint16_t UART1_Rx_Deal_Ptr = 0;                                   // UART1 RX data deal pointer

/*********************************************************************
 * @fn      TIM2_Init
 *
 * @brief   Timer2 100us initialisation
 *
 * @return  none
 */
void TIM2_Init( void )
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure = {0};

    RCC_PB1PeriphClockCmd(RCC_PB1Periph_TIM2, ENABLE);
    TIM_TimeBaseStructure.TIM_Period = 71;
    TIM_TimeBaseStructure.TIM_Prescaler =100;
    TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE );
    NVIC_EnableIRQ(TIM2_IRQn);
    TIM_Cmd(TIM2, ENABLE);
}

/*********************************************************************
 * @fn      TIM2_IRQHandler
 *
 * @brief   TIM2 IRQ handler
 *
 * @return  none
 */
void TIM2_IRQHandler(void)
{
    UART1_TimeOut++;
    TIM_ClearITPendingBit(TIM2,TIM_IT_Update);
}

/*********************************************************************
 * @fn      UART1_DMA_Init
 *
 * @brief   UART1 DMA initialization
 *
 * @return  none
 */
void UART1_DMA_Init( void )
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_HBPeriphClockCmd( RCC_HBPeriph_DMA1, ENABLE );

    /* UART1 Tx DMA initialization */
    DMA_Cmd( DMA1_Channel6, DISABLE );
    DMA_DeInit( DMA1_Channel6 );
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&USART1->DATAR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)Data_Buffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = DEF_USBD_FS_PACK_SIZE;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init( DMA1_Channel6, &DMA_InitStructure );

    /* UART1 Rx DMA initialization */
    DMA_Cmd( DMA1_Channel7, DISABLE );
    DMA_DeInit( DMA1_Channel7 );
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)UART1_RxBuffer;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = DEF_UART1_BUF_SIZE;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;
    DMA_Init( DMA1_Channel7, &DMA_InitStructure );
    DMA_Cmd( DMA1_Channel7, ENABLE );

    USART_DMACmd(USART1, USART_DMAReq_Rx, ENABLE);
}


/*********************************************************************
 * @fn      UART1_Init
 *
 * @brief   UART1 DMA initialization
 *
 * @return  none
 */
void UART1_Init( void )
{
    GPIO_InitTypeDef  GPIO_InitStructure = {0};
    USART_InitTypeDef USART_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_GPIOC | RCC_PB2Periph_USART1, ENABLE);

    /* UART1 GPIO Init */
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    /* UART1 Init */
    USART_InitStructure.USART_BaudRate = 115200;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_Init(USART1, &USART_InitStructure);

    USART_ClearFlag( USART1, USART_FLAG_TC );
    USART_Cmd(USART1, ENABLE);
}

void UART1_DMA_Tx(uint8_t *pbuf,uint16_t len)
{
    USART_ClearFlag(USART1, USART_FLAG_TC);
    DMA_Cmd( DMA1_Channel6, DISABLE );
    DMA1_Channel6->MADDR = (uint32_t)pbuf;
    DMA1_Channel6->CNTR = (uint32_t)len;
    DMA_Cmd( DMA1_Channel6, ENABLE );
    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
}

/*********************************************************************
 * @fn      UART1_Rx_Service
 *
 * @brief   UART1 rx service routine that sends the data received by
 *          UART1 via USB-HID.
 *
 * @return  none
 */
void UART1_Rx_Service( void )
{
    uint16_t pkg_len;
    uint16_t u16_temp;
    uint16_t remain_len;

    /* Get UART1 rx data */
    UART1_RX_CurCnt = DMA_GetCurrDataCounter(DMA1_Channel7);                              // Get DMA remaining count
    if (UART1_RX_LastCnt != UART1_RX_CurCnt)
    {
        if (UART1_RX_LastCnt > UART1_RX_CurCnt)
        {
            u16_temp = UART1_RX_LastCnt - UART1_RX_CurCnt;
        }
        else
        {
            u16_temp = UART1_RX_LastCnt + ( DEF_UART1_BUF_SIZE - UART1_RX_CurCnt );
        }

        UART1_RX_LastCnt = UART1_RX_CurCnt;

        if ((UART1_Rx_RemainLen + u16_temp) > DEF_UART1_BUF_SIZE )
        {
            printf("UART1 RX_buffer overflow\r\n");                                           // overflow: New data overwrites old data
        }
        else
        {
            UART1_Rx_RemainLen += u16_temp;
        }
        UART1_TimeOut = 0;
    }

    /* Pass-through UART1 data to USB-HID  */
    if (UART1_Rx_RemainLen)
    {
        /* Determine if the USB endpoint is allowed to upload */
        if (USBFS_Endp_Busy[DEF_UEP2] == 0x00)
        {
            /* Calculate the length of this packet upload via USB. */
            remain_len = UART1_Rx_RemainLen;
            pkg_len = 0x00;
            if (remain_len >= ( DEF_USBD_FS_PACK_SIZE - 1 ))
            {
                remain_len = ( DEF_USBD_FS_PACK_SIZE - 1 );
                pkg_len = remain_len;
            }
            else
            {
                if (UART1_TimeOut > DEF_UART1_TOUT_TIME)
                {
                    pkg_len = remain_len;
                }
            }

            if ( pkg_len > ( DEF_UART1_BUF_SIZE - UART1_Rx_Deal_Ptr )  )
            {
                pkg_len = ( DEF_UART1_BUF_SIZE - UART1_Rx_Deal_Ptr );                       // The data at the end of the buffer will be sent directly.
            }

            /* Upload packet via USB. */
            if (pkg_len)
            {
                USBFSD->UEP2_DMA = (uint16_t)(uint32_t)USBFS_EP2_Buf;
                USBFS_EP2_Buf[0] = pkg_len;
                memcpy(USBFS_EP2_Buf + 1,&UART1_RxBuffer[UART1_Rx_Deal_Ptr],pkg_len);
                USBFSD->UEP2_TX_LEN = pkg_len + 1;
                USBFS_Endp_Busy[DEF_UEP2] = 1;
                USBFSD->UEP2_TX_CTRL = (USBFSD->UEP2_TX_CTRL & ~USBFS_UEP_T_RES_MASK) | USBFS_UEP_T_RES_ACK;            // Start Upload
                UART1_Rx_RemainLen -= pkg_len;
                UART1_Rx_Deal_Ptr += pkg_len;

                if (UART1_Rx_Deal_Ptr >= DEF_UART1_BUF_SIZE)
                {
                    UART1_Rx_Deal_Ptr = 0x00;
                }

            }
        }
    }

}

/*********************************************************************
 * @fn      UART1_Tx_Service
 *
 * @brief   UART1 tx service routine that sends the data received by
 *          USB-HID via UART1.
 *
 * @return  none
 */
void UART1_Tx_Service( void )
{
    uint16_t pkg_len = 0;
    uint8_t *pbuf;
    if (UART1_Tx_Flag)
    {
        if (USART_GetFlagStatus(USART1, USART_FLAG_TC))                                  // Check whether UART1 has finished sending.
        {
            USART_ClearFlag(USART1, USART_FLAG_TC);
            USART_DMACmd(USART1, USART_DMAReq_Tx, DISABLE);
            UART1_Tx_Flag = 0;
            NVIC_DisableIRQ(USBFS_IRQn);                                                  // Disable USB interrupts
            RingBuffer_Comm.RemainPack--;
            RingBuffer_Comm.DealPtr++;
            if(RingBuffer_Comm.DealPtr == DEF_Ring_Buffer_Max_Blks)
            {
                RingBuffer_Comm.DealPtr = 0;
            }
            NVIC_EnableIRQ(USBFS_IRQn);                                                   // Enable USB interrupts
        }
    }
    else
    {
        /* Pass-through USB-HID data to UART1 */
        if(RingBuffer_Comm.RemainPack)
        {
            pkg_len = Data_Buffer[(RingBuffer_Comm.DealPtr) * DEF_USBD_FS_PACK_SIZE];      // Get the valid data length
            if (pkg_len)
            {
                if (pkg_len > ( DEF_USBD_FS_PACK_SIZE - 1 ) )
                {
                    pkg_len = DEF_USBD_FS_PACK_SIZE - 1;                                   // Limit the length of this transmission
                }
                pbuf = &Data_Buffer[(RingBuffer_Comm.DealPtr) * DEF_USBD_FS_PACK_SIZE] + 1;
                UART1_DMA_Tx( pbuf, pkg_len );
                UART1_Tx_Flag = 1;
            }
            else
            {
                /* drop out */
                NVIC_DisableIRQ(USBFS_IRQn);                                                  // Disable USB interrupts
                RingBuffer_Comm.RemainPack--;
                RingBuffer_Comm.DealPtr++;
                if(RingBuffer_Comm.DealPtr == DEF_Ring_Buffer_Max_Blks)
                {
                    RingBuffer_Comm.DealPtr = 0;
                }
                NVIC_EnableIRQ(USBFS_IRQn);                                                   // Enable USB interrupts
            }
        }
    }

    /* Monitor whether the remaining space is available for further downloads */
    if(RingBuffer_Comm.RemainPack < (DEF_Ring_Buffer_Max_Blks - DEF_RING_BUFFER_RESTART))
    {
        if(RingBuffer_Comm.StopFlag)
        {
            printf("USB ring buffer full, stop receiving further data.\r\n");
            RingBuffer_Comm.StopFlag = 0;
            USBFSD->UEP1_RX_CTRL = (USBFSD->UEP1_RX_CTRL & ~USBFS_UEP_R_RES_MASK) | USBFS_UEP_R_RES_ACK;
        }
    }

}

/*********************************************************************
 * @fn      HID_Set_Report_Deal
 *
 * @brief   print hid set report data
 *
 * @return  none
 */
void HID_Set_Report_Deal()
{
    if (HID_Set_Report_Flag == SET_REPORT_WAIT_DEAL)
    {
        UART1_DMA_Tx( HID_Report_Buffer, 64 );
        HID_Set_Report_Flag = SET_REPORT_DEAL_OVER;
        USBFSD->UEP0_TX_LEN  = 0;
        USBFSD->UEP0_TX_CTRL =  USBFS_UEP_T_RES_ACK | USBFS_UEP_T_TOG;

    }
}




