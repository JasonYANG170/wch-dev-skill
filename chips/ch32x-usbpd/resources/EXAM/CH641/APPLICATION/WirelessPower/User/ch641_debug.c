/********************************** (C) COPYRIGHT  *******************************
 * File Name          : ch641_debug.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2024/07/08
 * Description        : This file contains all the functions prototypes for UART
 *                      Printf , Delay functions.
 *********************************************************************************
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for 
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/
#include <ch641_debug.h>

TMR_PARAM timer;
uint8_t volatile tmr_ms_cnt;
uint16_t volatile decode_tick;


/*********************************************************************
 * @fn      Delay_Us
 *
 * @brief   Microsecond Delay Time.
 *
 * @param   n - Microsecond number.
 *
 * @return  None
 */
void mDelayuS(uint16_t n)
{
    uint16_t i,k1,k2,dk;

    k1 = (uint16_t)SysTick->CNT;
    i = (n-1) * 6;
    while(1)
    {
        k2 = (uint16_t)SysTick->CNT;
        if(k2 >= k1)
        {
            dk = k2-k1;
        }
        else {
            dk = k2+6000-k1;
        }
        if(i > dk)
        {
            i = i-dk;
        }
        else {
            break;
        }
        k1 = k2;
    }
}

/*********************************************************************
 * @fn      Delay_Ms
 *
 * @brief   Millisecond Delay Time.
 *
 * @param   n - Millisecond number.
 *
 * @return  None
 */
void mDelaymS(uint16_t n)
{
    while ( n )
    {
        mDelayuS( 1000 );
        -- n;
    }
}

extern void QI_Recv_Tick_Proc( void );

void SysTick_Handler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
/*********************************************************************
 * @fn      SysTick_Handler
 *
 * @brief   Interrupt callback function.
 *
 * @return  None
 */
void SysTick_Handler( void )
{
    SysTick->SR = 0;
    decode_tick += 6000;
    tmr_ms_cnt++;
    QI_Recv_Tick_Proc();                    //Qi decoding timeout handling
    Tmr_ISR_Callback();                     //interrupt callback function
}

/*********************************************************************
 * @fn      Timer_Init
 *
 * @brief   Timer initialization.
 *
 * @return  None
 */
void Timer_Init( void )
{
    SysTick->CNT = 0;
    SysTick->CMP = 6000-1;
    SysTick->SR = 0;
    SysTick->CTLR = (1<<3) | (1<<1) | (1<<0);         //Auto-reload; HCLK/8; turn on interrupt; start counter
    NVIC_SetPriority(SysTick_IRQn, 0x80 | 0x60);      //Only bit7 bit6 is valid, lowest priority. Non-preemptive and same priority as QII decoding.
    NVIC_EnableIRQ(SysTick_IRQn);
}

#if DE_PRINTF
/*********************************************************************
 * @fn      USART_Printf_Init
 *
 * @brief   Initializes the USARTx peripheral.
 *
 * @param   baudrate - USART communication baud rate.
 *
 * @return  None
 */
void USART_Printf_Init(uint32_t baudrate)
{
    GPIOB->CFGLR |= 0xF0;           //PB1
    AFIO->PCFR1 &= ~(7<<2);
    AFIO->PCFR1 |= (1<<2);

    USART1->BRR = 417;             //BRR = DIV_M+(DIV_F/16) = 48000000/16/115200=26.041666,The result of the calculation is then multiplied by 16 and fed into the registers
    USART1->CTLR1 = 0x2008;        //UE=uart enable    TE=transmit enable
}
#endif


#if (DE_PRINTF == 1)
void uart_send_dat(uint8_t udat)
{
    while((USART1->STATR & USART_FLAG_TXE) == 0 );
    USART1->DATAR = udat;
}

void uart_send_buf(uint8_t *ubuf,uint8_t ulen)
{
    uint8_t i;
    for(i = 0;i!=ulen;i++)
    {
        uart_send_dat(ubuf[i]);
    }
}

#endif
#if (DE_PRINTF == 2)
/*********************************************************************
 * @fn      _write
 *
 * @brief   Support Printf Function
 *
 * @param   *buf - UART send Data.
 *          size - Data length.
 *
 * @return  size - Data length
 */
__attribute__((used)) 
int _write(int fd, char *buf, int size)
{
    int i;

    for(i = 0; i < size; i++){
        while((USART1->STATR & USART_FLAG_TXE) == 0);
        USART1->DATAR = *buf++;
    }

    return size;
}
#endif

/*********************************************************************
 * @fn      _sbrk
 *
 * @brief   Change the spatial position of data segment.
 *
 * @return  size: Data length
 */
__attribute__((used)) 
void *_sbrk(ptrdiff_t incr)
{
    extern char _end[];
    extern char _heap_end[];
    static char *curbrk = _end;

    if ((curbrk + incr < _end) || (curbrk + incr > _heap_end))
    return NULL - 1;

    curbrk += incr;
    return curbrk -incr;
}


