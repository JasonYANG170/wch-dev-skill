# UART 串口通信

> **适用摘要**: 配置 CH58x/CH59x UART 串口的初始化、发送、接收和中断

## 触发意图

- "配置 UART 串口"
- "UART1 发送数据"
- "串口中断接收"
- "修改串口波特率"
- "printf 重定向到串口"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CH58x_common.h` 或 `CH59x_common.h` |
| GPIO | 串口引脚已配置 |

## 调用链

```
Step 1: 配置 GPIO 引脚 (ModeCfg)
Step 2: 初始化 UART (DefInit 或 BaudRateCfg)
Step 3: (可选) 配置中断 (INTCfg)
Step 4: 发送/接收数据
```

## 分步说明

### Step 1: UART1 默认初始化（调试串口）

```c
// GPIO 配置
GPIOA_ModeCfg(GPIO_Pin_8, GPIO_ModeIN_PU);       // RX 上拉输入
GPIOA_ModeCfg(GPIO_Pin_9, GPIO_ModeOut_PP_5mA);   // TX 推挽输出

// 默认初始化 (115200, 8N1)
UART1_DefInit();

// 发送字符串
UART1_SendString("Hello CH58x\n", 12);
```

### Step 2: 自定义波特率

```c
// 先默认初始化
UART1_DefInit();

// 再修改波特率
UART1_BaudRateCfg(9600);
```

### Step 3: 中断接收

```c
// 配置中断
UART1_INTCfg(ENABLE, UART_IT_RX_BYTE);

// 中断服务程序
__attribute__((interrupt("WCH-Interrupt-fast")))
__attribute__((section(".highcode")))
void UART1_IRQHandler(void) {
    uint8_t flag = UART1_GetITFlag();

    if ((flag & UART_II_MASK) == UART_II_RECV_RDY) {
        uint8_t data = UART1_RecvByte();
        // 处理接收到的数据
        UART1_SendByte(data);  // 回显
    }

    if ((flag & UART_II_MASK) == UART_II_RECV_TOUT) {
        // 接收超时，读取剩余数据
        while (UART1_RecvByte(&data) == SUCCESS) {
            // 处理
        }
    }
}
```

### Step 4: printf 重定向

```c
// 重写 _write 函数 (GCC)
int _write(int fd, char *buf, int len) {
    for (int i = 0; i < len; i++) {
        UART1_SendByte(buf[i]);
    }
    return len;
}
```

## UART 实例

| UART | TX 引脚 | RX 引脚 | 说明 |
|---|---|---|---|
| UART0 | PA4 | PA5 | 通用串口 |
| UART1 | PA9 | PA8 | 默认调试串口 |
| UART2 | PA20 | PA21 | 通用串口 |
| UART3 | PA13 | PA12 | 通用串口 |

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 无输出 | GPIO 未配置 | 先配置 ModeCfg 再初始化 UART |
| 乱码 | 波特率不匹配 | 检查两端波特率设置 |
| 丢数据 | 中断未开启或 FIFO 溢出 | 开启中断并及时读取数据 |

## 参考项目

- `resources/EXAM/UART1/UART1_Printf/` — 串口打印
- `resources/EXAM/UART1/UART1_INT/` — 串口中断
