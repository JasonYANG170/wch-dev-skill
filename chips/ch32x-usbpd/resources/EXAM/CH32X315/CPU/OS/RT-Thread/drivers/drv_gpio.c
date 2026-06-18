/*
 * File      : drv_gpio.c
 * This file is part of RT-Thread RTOS
 * COPYRIGHT (C) 2015, RT-Thread Development Team
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rt-thread.org/license/LICENSE
 *
 * Change Logs:
 * Date           Author            Notes
 * 2026-04-11     ZYH            the first version
 */

#include <rthw.h>
#include <rtdevice.h>
#include <board.h>
#include <rtthread.h>
#include "drivers/pin.h"


#ifdef RT_USING_PIN
#define __CH32_PIN(index, gpio, gpio_index) {index, GPIO##gpio##_CLK_ENABLE, GPIO##gpio, GPIO_Pin_##gpio_index}
#define __CH32_PIN_DEFAULT  {-1, 0, 0, 0}


static void GPIOA_CLK_ENABLE(void)
{
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOA, ENABLE);
}

static void GPIOB_CLK_ENABLE(void)
{
     RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOB, ENABLE);
}

static void GPIOC_CLK_ENABLE(void)
{
     RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOC, ENABLE);
}

static void GPIOD_CLK_ENABLE(void)
{
    RCC_HB2PeriphClockCmd(RCC_HB2Periph_GPIOD, ENABLE);
}

/* CH32 GPIO driver */
struct pin_index
{
    int index;
    void (*rcc)(void);
    GPIO_TypeDef *gpio;
    uint32_t pin;
};

static const struct pin_index pins[] =
{
#if (CH32X3xx_CHIP ==  CH32X315MCU6_CHIP)
    __CH32_PIN_DEFAULT,
    __CH32_PIN(1, D, 0),
    __CH32_PIN(2, D, 1),    
    __CH32_PIN(3, D, 2), 
    __CH32_PIN(4, D, 14),
    __CH32_PIN(5, D, 15),
    __CH32_PIN(6, C, 0),
    __CH32_PIN(7, C, 1),
    __CH32_PIN(8, C, 2),
    __CH32_PIN(9, C, 3),
    __CH32_PIN(10, C, 4),
    __CH32_PIN(11, C, 5),
    __CH32_PIN(12, C, 6),
    __CH32_PIN(13, C, 7),
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN(18, A, 0),
    __CH32_PIN(19, A, 1),
    __CH32_PIN(20, A, 2),
    __CH32_PIN(21, A, 3),
    __CH32_PIN(22, A, 4),
    __CH32_PIN(23, A, 5),
    __CH32_PIN(24, A, 6),
    __CH32_PIN(25, A, 7),
    __CH32_PIN(26, B, 0),
    __CH32_PIN(27, B, 1),
    __CH32_PIN(28, B, 2),
    __CH32_PIN(29, B, 3),
    __CH32_PIN(30, B, 4),
    __CH32_PIN(31, B, 5),
    __CH32_PIN(32, B, 6),
    __CH32_PIN(33, B, 7),
    __CH32_PIN(34, C, 8),
    __CH32_PIN(35, C, 9),
    __CH32_PIN(36, C, 10),
    __CH32_PIN(37, C, 11), 
    __CH32_PIN_DEFAULT,
    __CH32_PIN(39, C, 12),
    __CH32_PIN(40, C, 13),
    __CH32_PIN(41, C, 14),
    __CH32_PIN(42, C, 15), 
    __CH32_PIN(43, B, 8),
    __CH32_PIN(44, B, 9),
    __CH32_PIN(45, B, 10), 
    __CH32_PIN(46, B, 11),
    __CH32_PIN(47, D, 8),
    __CH32_PIN(48, D, 9),
    __CH32_PIN(49, D, 10),
    __CH32_PIN(50, D, 11),
    __CH32_PIN(51, D, 12),   
    __CH32_PIN(52, D, 13),
    __CH32_PIN(53, A, 8),
    __CH32_PIN(54, A, 9),        
    __CH32_PIN(55, A, 10),
    __CH32_PIN(56, A, 11), 
    __CH32_PIN(57, A, 12), 
    __CH32_PIN_DEFAULT, 
    __CH32_PIN(59, A, 13),
    __CH32_PIN(60, A, 14),
    __CH32_PIN(61, A, 15), 
    __CH32_PIN(62, B, 12),
    __CH32_PIN(63, B, 13),
    __CH32_PIN(64, B, 14), 
    __CH32_PIN(65, B, 15), 
    __CH32_PIN(66, D, 3),       
    __CH32_PIN(67, D, 4),
    __CH32_PIN(68, D, 5),
    __CH32_PIN(69, D, 6),    
    __CH32_PIN(70, D, 7),
    __CH32_PIN_DEFAULT, 
    __CH32_PIN_DEFAULT,        
    __CH32_PIN_DEFAULT, 
    __CH32_PIN_DEFAULT,      
    __CH32_PIN_DEFAULT, 
    __CH32_PIN_DEFAULT,

#endif
#if ( CH32X3xx_CHIP ==  CH32X315WCU6_CHIP)
    __CH32_PIN_DEFAULT,
    __CH32_PIN(1, D, 0),
    __CH32_PIN(2, D, 1),
    __CH32_PIN(3, D, 14),
    __CH32_PIN(4, D, 15),
    __CH32_PIN(5, C, 0),
    __CH32_PIN(6, C, 1),
    __CH32_PIN(7, C, 2),
    __CH32_PIN(8, C, 3),
    __CH32_PIN(9, C, 4),
    __CH32_PIN(10, C, 5),
    __CH32_PIN(11, C, 6),
    __CH32_PIN(12, C, 7),
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN(15, A, 0),
    __CH32_PIN(16, A, 1),
    __CH32_PIN(17, A, 2),
    __CH32_PIN(18, A, 3),
    __CH32_PIN(19, A, 4),
    __CH32_PIN(20, A, 5),
    __CH32_PIN(21, A, 6),
    __CH32_PIN(22, A, 7),
    __CH32_PIN(23, B, 0),
    __CH32_PIN(24, B, 1),
    __CH32_PIN(25, B, 2),
    __CH32_PIN(26, B, 3),
    __CH32_PIN(27, B, 4),
    __CH32_PIN(28, B, 5),
    __CH32_PIN(29, B, 6),
    __CH32_PIN(30, B, 7),
    __CH32_PIN(31, C, 8),
    __CH32_PIN(32, C, 9),
    __CH32_PIN(33, C, 10),
    __CH32_PIN(34, C, 11),
    __CH32_PIN_DEFAULT,
    __CH32_PIN(36, C, 12),
    __CH32_PIN(37, C, 13),
    __CH32_PIN(38, C, 14),
    __CH32_PIN(39, C, 15),
    __CH32_PIN(40, B, 8),
    __CH32_PIN(41, B, 9),
    __CH32_PIN(42, B, 10),
    __CH32_PIN(43, B, 11),
    __CH32_PIN(44, D, 8),
    __CH32_PIN(45, D, 9),
    __CH32_PIN(46, D, 10),
    __CH32_PIN(47, D, 11),
    __CH32_PIN(48, D, 12),
    __CH32_PIN(49, D, 13),
    __CH32_PIN(50, A, 8),
    __CH32_PIN(51, A, 9),
    __CH32_PIN(52, A, 10),
    __CH32_PIN(53, A, 11),
    __CH32_PIN(54, A, 12),
    __CH32_PIN(55, A, 13),
    __CH32_PIN(56, A, 14),
    __CH32_PIN(57, A, 15),
    __CH32_PIN(58, D, 3),
    __CH32_PIN(59, D, 4),
    __CH32_PIN(60, D, 5),
    __CH32_PIN(61, D, 6),
    __CH32_PIN(62, D, 7),
    __CH32_PIN_DEFAULT, 
    __CH32_PIN_DEFAULT, 
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,

#endif

#if (CH32X3xx_CHIP ==  CH32X315CCU6_CHIP)
    __CH32_PIN_DEFAULT,
    __CH32_PIN(1, D, 0),
    __CH32_PIN(2, D, 1),
    __CH32_PIN(3, D, 2),
    __CH32_PIN(4, C, 4),
    __CH32_PIN(5, C, 5),
    __CH32_PIN(6, C, 6),
    __CH32_PIN(7, C, 7),
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN(10, A, 0),
    __CH32_PIN(11, A, 1),
    __CH32_PIN(12, A, 2),
    __CH32_PIN(13, A, 3),
    __CH32_PIN(14, A, 4),
    __CH32_PIN(15, A, 5),
    __CH32_PIN(16, A, 6),
    __CH32_PIN(17, A, 7),
    __CH32_PIN(18, B, 4),
    __CH32_PIN(19, B, 5),
    __CH32_PIN(20, B, 6),
    __CH32_PIN(21, B, 7),
    __CH32_PIN(22, C, 8),
    __CH32_PIN(23, C, 9),
    __CH32_PIN(24, B, 8),
    __CH32_PIN(25, B, 9),
    __CH32_PIN(26, B, 10),
    __CH32_PIN(27, B, 11),
    __CH32_PIN(28, D, 12),
    __CH32_PIN(29, D, 13),
    __CH32_PIN(30, A, 8),
    __CH32_PIN(31, A, 9),
    __CH32_PIN(32, A, 10),
    __CH32_PIN(33, A, 11),
    __CH32_PIN(34, A, 12),
    __CH32_PIN(35, A, 13),
    __CH32_PIN(36, A, 14),
    __CH32_PIN(37, A, 15),
    __CH32_PIN(38, D, 3),
    __CH32_PIN(39, D, 4),
    __CH32_PIN(40, D, 5),
    __CH32_PIN(41, D, 6),
    __CH32_PIN(42, D, 7),   
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,    

#endif

#if (CH32X3xx_CHIP ==  CH32X305RCT6_CHIP)
    __CH32_PIN_DEFAULT,
    __CH32_PIN(1, D, 0),
    __CH32_PIN(2, D, 1),
    __CH32_PIN(3, D, 2),
    __CH32_PIN(4, C, 0),
    __CH32_PIN(5, C, 1),
    __CH32_PIN(6, C, 2),
    __CH32_PIN(7, C, 3),
    __CH32_PIN(8, C, 4),
    __CH32_PIN(9, C, 5),
    __CH32_PIN(10, C, 6),
    __CH32_PIN(11, C, 7),
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN(17, A, 0),
    __CH32_PIN(18, A, 1),
    __CH32_PIN(19, A, 2),
    __CH32_PIN(20, A, 3),
    __CH32_PIN(21, A, 4),
    __CH32_PIN(22, A, 5),
    __CH32_PIN(23, A, 6),
    __CH32_PIN(24, A, 7),
    __CH32_PIN(25, B, 0),
    __CH32_PIN(26, B, 4),
    __CH32_PIN(27, B, 5),
    __CH32_PIN(28, B, 6),
    __CH32_PIN(29, B, 7),
    __CH32_PIN(30, C, 8),
    __CH32_PIN(31, C, 9),
    __CH32_PIN(32, C, 10),
    __CH32_PIN(33, C, 11),
    __CH32_PIN_DEFAULT,
    __CH32_PIN(35, C, 12),
    __CH32_PIN(36, C, 13),
    __CH32_PIN(37, B, 8),
    __CH32_PIN(38, B, 9),
    __CH32_PIN(39, B, 10),
    __CH32_PIN(40, B, 11),
    __CH32_PIN(41, D, 10),
    __CH32_PIN(42, D, 11),
    __CH32_PIN(43, D, 12),
    __CH32_PIN(44, D, 13),
    __CH32_PIN(45, A, 8),
    __CH32_PIN(46, A, 9),
    __CH32_PIN(47, A, 10),
    __CH32_PIN(48, A, 11),
    __CH32_PIN(49, A, 12),
    __CH32_PIN(50, A, 13),
    __CH32_PIN(51, A, 14),
    __CH32_PIN(52, A, 15),
    __CH32_PIN(53, B, 12),
    __CH32_PIN(54, B, 13),
    __CH32_PIN(55, B, 14),
    __CH32_PIN(56, B, 15),
    __CH32_PIN(57, D, 3), 
    __CH32_PIN(58, D, 4), 
    __CH32_PIN(59, D, 5),
    __CH32_PIN(60, D, 6),
    __CH32_PIN(61, D, 7),                             
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,
    __CH32_PIN_DEFAULT,

#endif

};

struct pin_irq_map
{
    rt_uint16_t pinbit;
    IRQn_Type irqno;
};
static const struct pin_irq_map pin_irq_map[] =
{
    {GPIO_Pin_0, EXTI7_0_IRQn},
    {GPIO_Pin_1, EXTI7_0_IRQn},
    {GPIO_Pin_2, EXTI7_0_IRQn},
    {GPIO_Pin_3, EXTI7_0_IRQn},
    {GPIO_Pin_4, EXTI7_0_IRQn},
    {GPIO_Pin_5, EXTI7_0_IRQn},
    {GPIO_Pin_6, EXTI7_0_IRQn},
    {GPIO_Pin_7, EXTI7_0_IRQn},
    {GPIO_Pin_8, EXTI15_8_IRQn},
    {GPIO_Pin_9, EXTI15_8_IRQn},
    {GPIO_Pin_10, EXTI15_8_IRQn},
    {GPIO_Pin_11, EXTI15_8_IRQn},
    {GPIO_Pin_12, EXTI15_8_IRQn},
    {GPIO_Pin_13, EXTI15_8_IRQn},
    {GPIO_Pin_14, EXTI15_8_IRQn},
    {GPIO_Pin_15, EXTI15_8_IRQn},
};
struct rt_pin_irq_hdr pin_irq_hdr_tab[] =
{
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
    { -1, 0, RT_NULL, RT_NULL},
};

#define ITEM_NUM(items) sizeof(items) / sizeof(items[0])
const struct pin_index *get_pin(uint8_t pin)
{
    const struct pin_index *index;
    if (pin < ITEM_NUM(pins))
    {
        index = &pins[pin];
        if (index->index == -1)
            index = RT_NULL;
    }
    else
    {
        index = RT_NULL;
    }
    return index;
};

void ch32_pin_write(rt_device_t dev, rt_base_t pin, rt_base_t value)
{
    const struct pin_index *index;
    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return;
    }
    GPIO_WriteBit(index->gpio, index->pin, (BitAction)value);
}

int ch32_pin_read(rt_device_t dev, rt_base_t pin)
{
    int value;
    const struct pin_index *index;
    value = PIN_LOW;
    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return value;
    }
    value = GPIO_ReadInputDataBit(index->gpio, index->pin);
    return value;
}

void ch32_pin_mode(rt_device_t dev, rt_base_t pin, rt_base_t mode)
{
    const struct pin_index *index;
    GPIO_InitTypeDef GPIO_InitStruct;
    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return;
    }
    /* GPIO Periph clock enable */
    index->rcc();
    /* Configure GPIO_InitStructure */
    GPIO_InitStruct.GPIO_Pin = index->pin;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Very_High;
    if (mode == PIN_MODE_OUTPUT)
    {
        /* output setting */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    }
    else if (mode == PIN_MODE_INPUT_AIN)
    {
        /* input setting: analog input. */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
    }
    else if (mode == PIN_MODE_INPUT)
    {
        /* input setting: float input. */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    }
    else if (mode == PIN_MODE_INPUT_PULLDOWN)
    {
        /* input setting: pull down. */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
    }
    else if (mode == PIN_MODE_INPUT_PULLUP)
    {
        /* input setting: pull up. */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
    }
    else if (mode == PIN_MODE_OUTPUT_OD)
    {
        /* output setting: od. */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_OD;
    }
    else if (mode == PIN_MODE_OUTPUT_AF_OD)
    {
        /* output setting: af od. */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
    }
    else if (mode == PIN_MODE_OUTPUT_AF_PP)
    {
        /* output setting: af pp. */
        GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    }
    GPIO_Init(index->gpio, &GPIO_InitStruct);
}


rt_inline rt_int32_t bit2bitno(rt_uint32_t bit)
{
    int i;
    for (i = 0; i < 32; i++)
    {
        if ((0x01 << i) == bit)
        {
            return i;
        }
    }
    return -1;
}

rt_inline const struct pin_irq_map *get_pin_irq_map(uint32_t pinbit)
{
    rt_int32_t mapindex = bit2bitno(pinbit);
    if (mapindex < 0 || mapindex >= ITEM_NUM(pin_irq_map))
    {
        return RT_NULL;
    }
    return &pin_irq_map[mapindex];
};
rt_err_t ch32_pin_attach_irq(struct rt_device *device, rt_int32_t pin,
                              rt_uint32_t mode, void (*hdr)(void *args), void *args)
{
    const struct pin_index *index;
    rt_base_t level;
    rt_int32_t irqindex = -1;
    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return RT_ENOSYS;
    }
    irqindex = bit2bitno(index->pin);
    if (irqindex < 0 || irqindex >= ITEM_NUM(pin_irq_map))
    {
        return RT_ENOSYS;
    }
    level = rt_hw_interrupt_disable();
    if (pin_irq_hdr_tab[irqindex].pin == pin &&
            pin_irq_hdr_tab[irqindex].hdr == hdr &&
            pin_irq_hdr_tab[irqindex].mode == mode &&
            pin_irq_hdr_tab[irqindex].args == args)
    {
        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }
    if (pin_irq_hdr_tab[irqindex].pin != -1)
    {
        rt_hw_interrupt_enable(level);
        return RT_EBUSY;
    }
    pin_irq_hdr_tab[irqindex].pin = pin;
    pin_irq_hdr_tab[irqindex].hdr = hdr;
    pin_irq_hdr_tab[irqindex].mode = mode;
    pin_irq_hdr_tab[irqindex].args = args;
    rt_hw_interrupt_enable(level);
    return RT_EOK;
}

rt_err_t ch32_pin_dettach_irq(struct rt_device *device, rt_int32_t pin)
{
    const struct pin_index *index;
    rt_base_t level;
    rt_int32_t irqindex = -1;
    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return RT_ENOSYS;
    }
    irqindex = bit2bitno(index->pin);
    if (irqindex < 0 || irqindex >= ITEM_NUM(pin_irq_map))
    {
        return RT_ENOSYS;
    }
    level = rt_hw_interrupt_disable();
    if (pin_irq_hdr_tab[irqindex].pin == -1)
    {
        rt_hw_interrupt_enable(level);
        return RT_EOK;
    }
    pin_irq_hdr_tab[irqindex].pin = -1;
    pin_irq_hdr_tab[irqindex].hdr = RT_NULL;
    pin_irq_hdr_tab[irqindex].mode = 0;
    pin_irq_hdr_tab[irqindex].args = RT_NULL;
    rt_hw_interrupt_enable(level);
    return RT_EOK;
}

rt_err_t ch32_pin_irq_enable(struct rt_device *device, rt_base_t pin,
                              rt_uint32_t enabled)
{
    const struct pin_index *index;
    const struct pin_irq_map *irqmap;
    rt_base_t level;
    rt_int32_t irqindex = -1;
    rt_uint8_t gpio_port_souce=0;
    GPIO_InitTypeDef GPIO_InitStruct;
    EXTI_InitTypeDef EXTI_InitStructure;

    index = get_pin(pin);
    if (index == RT_NULL)
    {
        return RT_ENOSYS;
    }
    if (enabled == PIN_IRQ_ENABLE)
    {
        irqindex = bit2bitno(index->pin);
        if (irqindex < 0 || irqindex >= ITEM_NUM(pin_irq_map))
        {
            return RT_ENOSYS;
        }
        level = rt_hw_interrupt_disable();
        if (pin_irq_hdr_tab[irqindex].pin == -1)
        {
            rt_hw_interrupt_enable(level);
            return RT_ENOSYS;
        }
        irqmap = &pin_irq_map[irqindex];
        /* GPIO Periph clock enable */
        index->rcc();
        RCC_HB2PeriphClockCmd(RCC_HB2Periph_AFIO,ENABLE);

        /* Configure GPIO_InitStructure */
        GPIO_InitStruct.GPIO_Pin = index->pin;
        GPIO_InitStruct.GPIO_Speed = GPIO_Speed_Very_High;

        EXTI_InitStructure.EXTI_Line=index->pin;
        EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
        EXTI_InitStructure.EXTI_LineCmd = ENABLE;
        switch (pin_irq_hdr_tab[irqindex].mode)
        {
        case PIN_IRQ_MODE_RISING:
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
            EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
            break;
        case PIN_IRQ_MODE_FALLING:
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
            EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
            break;
        case PIN_IRQ_MODE_RISING_FALLING:
            GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
            EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
            break;
        }

        GPIO_Init(index->gpio, &GPIO_InitStruct);

        if(index->gpio==GPIOA) gpio_port_souce=GPIO_PortSourceGPIOA;
        if(index->gpio==GPIOB) gpio_port_souce=GPIO_PortSourceGPIOB;
        if(index->gpio==GPIOC) gpio_port_souce=GPIO_PortSourceGPIOC;
        if(index->gpio==GPIOD) gpio_port_souce=GPIO_PortSourceGPIOD;
        GPIO_EXTILineConfig(gpio_port_souce,(rt_uint8_t)irqindex);

        EXTI_Init(&EXTI_InitStructure);
        NVIC_SetPriority(irqmap->irqno,5<<4);
        NVIC_EnableIRQ( irqmap->irqno );

        rt_hw_interrupt_enable(level);
    }
    else if (enabled == PIN_IRQ_DISABLE)
    {
        irqmap = get_pin_irq_map(index->pin);
        if (irqmap == RT_NULL)
        {
            return RT_ENOSYS;
        }
        NVIC_DisableIRQ(irqmap->irqno);
    }
    else
    {
        return RT_ENOSYS;
    }
    return RT_EOK;
}

const static struct rt_pin_ops _ch32_pin_ops =
{
    ch32_pin_mode,
    ch32_pin_write,
    ch32_pin_read,
    ch32_pin_attach_irq,
    ch32_pin_dettach_irq,
    ch32_pin_irq_enable,
};

int rt_hw_pin_init(void)
{
    int result;
    result = rt_device_pin_register("pin", &_ch32_pin_ops, RT_NULL);
    return result;
}
INIT_BOARD_EXPORT(rt_hw_pin_init);

rt_inline void pin_irq_hdr(int irqno)
{
    if (pin_irq_hdr_tab[irqno].hdr)
    {
        pin_irq_hdr_tab[irqno].hdr(pin_irq_hdr_tab[irqno].args);
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    pin_irq_hdr(bit2bitno(GPIO_Pin));
}


void EXTI7_0_IRQHandler(void) __attribute__((interrupt()));
void EXTI15_8_IRQHandler(void) __attribute__((interrupt()));

void EXTI7_0_IRQHandler(void)
{
    GET_INT_SP();
    rt_interrupt_enter();

    if(EXTI_GetITStatus(EXTI_Line0)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_0);
        EXTI_ClearITPendingBit(EXTI_Line0);
    }
    if(EXTI_GetITStatus(EXTI_Line1)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_1);
        EXTI_ClearITPendingBit(EXTI_Line1);
    }
    if(EXTI_GetITStatus(EXTI_Line2)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_2);
        EXTI_ClearITPendingBit(EXTI_Line2);
    }
    if(EXTI_GetITStatus(EXTI_Line3)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_3);
        EXTI_ClearITPendingBit(EXTI_Line3);
    }
    if(EXTI_GetITStatus(EXTI_Line4)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_4);
        EXTI_ClearITPendingBit(EXTI_Line4);
    }
    if(EXTI_GetITStatus(EXTI_Line5)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_5);
        EXTI_ClearITPendingBit(EXTI_Line5);
    }
    if(EXTI_GetITStatus(EXTI_Line6)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_6);
        EXTI_ClearITPendingBit(EXTI_Line6);
    }
    if(EXTI_GetITStatus(EXTI_Line7)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_7);
        EXTI_ClearITPendingBit(EXTI_Line7);
    }        

    rt_interrupt_leave();
    FREE_INT_SP();
}

void EXTI15_8_IRQHandler(void)
{
     GET_INT_SP();
    rt_interrupt_enter();

    if(EXTI_GetITStatus(EXTI_Line8)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_8);
        EXTI_ClearITPendingBit(EXTI_Line8);
    }
    if(EXTI_GetITStatus(EXTI_Line9)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_9);
        EXTI_ClearITPendingBit(EXTI_Line9);
    }
    if(EXTI_GetITStatus(EXTI_Line10)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_10);
        EXTI_ClearITPendingBit(EXTI_Line10);
    }
    if(EXTI_GetITStatus(EXTI_Line11)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_11);
        EXTI_ClearITPendingBit(EXTI_Line11);
    }
    if(EXTI_GetITStatus(EXTI_Line12)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_12);
        EXTI_ClearITPendingBit(EXTI_Line12);
    }
    if(EXTI_GetITStatus(EXTI_Line13)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_13);
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
    if(EXTI_GetITStatus(EXTI_Line14)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_14);
        EXTI_ClearITPendingBit(EXTI_Line14);
    }
    if(EXTI_GetITStatus(EXTI_Line15)!=RESET)
    {
        HAL_GPIO_EXTI_Callback(GPIO_Pin_15);
        EXTI_ClearITPendingBit(EXTI_Line15);
    }        

    rt_interrupt_leave();
    FREE_INT_SP();
}


#endif
