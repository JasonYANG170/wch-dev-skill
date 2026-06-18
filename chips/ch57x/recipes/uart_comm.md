# UART 通信

> **适用摘要**: 配置 UART 串口通信，包括初始化、发送、接收和中断驱动。

## 触发意图

- "配置 UART"
- "串口通信"
- "UART 发送接收"
- "printf 调试输出"

## 前置条件

| 条件 | 要求 |
|---|---|
| 参考项目 | `resources/EXAM/UART1/` |

## 分步说明

### UART 引脚分配

| UART | TX 引脚 | RX 引脚 |
|------|---------|---------|
| UART0 | PA4 | PA5 |
| UART1 | PA9 | PA8 |
| UART2 | PA20 | PA21 |
| UART3 | PA13 | PA12 |

### 基本初始化（115200, 8N1）

```c
#include "CH57x_common.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

    // GPIO 配置
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);       // UART1 RX
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);   // UART1 TX

    // UART 初始化（默认 115200）
    UART1_DefInit();

    // 发送字符串
    UART1_SendString("Hello CH57x\n", 12);

    while(1);
}
```

### 自定义波特率

```c
UART1_DefInit();
UART1_BaudRateCfg(9600);   // 设置为 9600 波特率
```

### 中断接收

```c
volatile uint8_t rx_buf[256];
volatile uint16_t rx_cnt = 0;

__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void UART1_IRQHandler(void) {
    uint8_t flag = UART1_GetITFlag();
    switch (flag & UART_II_MASK) {
        case UART_II_RECV_RDY:
            while (R8_UART1_LSR & RB_LSR_DATA_RDY) {
                rx_buf[rx_cnt++] = R8_UART1_RBR;
                if (rx_cnt >= sizeof(rx_buf)) rx_cnt = 0;
            }
            break;
        case UART_II_LINE_STAT:
            (void)R8_UART1_LSR;  // 读 LSR 清除中断
            break;
    }
}

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);
    GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);
    GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);
    UART1_DefInit();

    // 使能接收中断
    UART1_INTCfg(ENABLE, RB_IER_RECV_RDY);
    PFIC_EnableIRQ(UART1_IRQn);

    while(1);
}
```

### printf 重定向

```c
// 重定向 printf 到 UART1（需要链接 libc）
int _write(int fd, char *buf, int len) {
    UART1_SendString((uint8_t *)buf, len);
    return len;
}

// 使用
printf("Value: %d\n", 42);
```

### 调试输出宏

```c
#ifdef DEBUG
  #define PRINT(fmt, ...) printf(fmt, ##__VA_ARGS__)
#else
  #define PRINT(fmt, ...)
#endif

// 在 main() 中初始化
#ifdef DEBUG
    UART1_DefInit();
#endif
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 收不到数据 | GPIO 模式未配置 | 先配置 GPIO 再初始化 UART |
| 乱码 | 波特率不匹配 | 检查两端波特率是否一致 |
| 数据丢失 | 中断优先级太低 | 提高中断优先级或使用 DMA |
| printf 不输出 | 未重定向 _write | 实现 _write 函数 |

## 参考项目

- `resources/EXAM/UART1/` — UART1 收发示例
- `resources/EXAM/BLE/BLE_UART/` — BLE UART 透传（中断驱动）
