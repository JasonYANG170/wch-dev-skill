# BLE 直接测试模式 (Direct Test Mode / DTM)

> **适用摘要**: 实现 CH58x/CH59x BLE Direct Test Mode，符合蓝牙 SIG DTM 规范，支持通过 UART 或 USB 接口接收 HCI DTM 命令进行射频 TX/RX 测试

## 触发意图

- "BLE DTM 测试"
- "直接测试模式"
- "BLE 射频认证"
- "Direct Test Mode"
- "蓝牙 DTM 命令"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | `CONFIG.h`, `HAL.h`, `CH58xBLE_LIB.h`, `RF_dtm.h`, `app_usb.h`, `RingMem.h` |
| config.h | `BLE_MEMHEAP_SIZE` >= 4KB |
| 库文件 | `libCH58xBLE.a` 或 `libCH59xBLE.a` |
| 外部工具 | 蓝牙测试仪或 DTM 上位机软件 |

## 调用链

```
Step 1: 配置 config.h
Step 2: 初始化 BLE (CH58X_BLEInit)
Step 3: 初始化 HAL, RF Role
Step 4: 初始化 UART (接收 DTM 命令) 和 USB (可选)
Step 5: 初始化 RF 配置
Step 6: 在 Main_Circulation 中轮询处理 DTM 命令
Step 7: 解析 HCI DTM 命令并执行 TX/RX 测试
Step 8: 返回 DTM 测试结果
```

## 分步说明

### Step 1: main.c 入口

```c
#include <RF_dtm.h>
#include "CONFIG.h"
#include "HAL.h"
#include "app_usb.h"

int main(void) {
    SetSysClock(CLK_SOURCE_PLL_60MHz);

#ifdef DEBUG
    GPIOB_SetBits(bTXD0);
    GPIOB_ModeCfg(bTXD0, GPIO_ModeOut_PP_5mA);
    UART0_DefInit();
    GPIOA_SetBits(bTXD1);
    GPIOA_ModeCfg(bTXD1, GPIO_ModeOut_PP_5mA);
    GPIOA_ModeCfg(bRXD1, GPIO_ModeIN_PU);
    UART1_DefInit();
#endif

    CH58X_BLEInit();
    HAL_Init();
    RF_RoleInit();

    // 初始化 UART1 接收 DTM 命令
    UART1_ByteTrigCfg(UART_4BYTE_TRIG);
    UART1_INTCfg(ENABLE, RB_IER_RECV_RDY);
    PFIC_EnableIRQ(UART1_IRQn);

    app_usb_init();  // USB 接口 (可选)
    RF_Init();       // RF 配置

    Main_Circulation();
    return 0;
}
```

### Step 2: Main_Circulation 中处理 DTM

```c
__HIGH_CODE
void Main_Circulation() {
    while(1) {
        TMOS_SystemProcess();
        DtmProcess();  // 轮询处理 DTM 命令
    }
}
```

### Step 3: DTM 命令格式

```
HCI DTM 命令格式: [0x01, OCF, OGF, param_len, params...]

支持的命令:
- Reset:              01 03 0C 00
- Receiver Test:      01 1D 20 01 <channel>
- Transmitter Test:   01 1E 20 03 <channel> <length> <type>
- Test End:           01 1F 20 00
- Set TX Power:       01 01 FF 01 <power>
- Set PF:             01 02 FF 01 <pf_value>
- Single Carrier:     01 03 FF 01 <channel>
- Receiver 2M:        01 33 <channel> <phy>
- Transmitter 2M:     01 34 <channel> <length> <type> <phy>
```

### Step 4: RF 配置

```c
void RF_Init(void) {
    RingMemInit(&uartRingParm, uartRingMemBuff, sizeof(uartRingMemBuff));
    RingMemInit(&usbRingParm, usbRingMemBuff, sizeof(usbRingMemBuff));

    taskID = TMOS_ProcessEventRegister(RF_ProcessEvent);
    rf_Config.accessAddress = 0x71764129;
    rf_Config.CRCInit = 0x555555;
    rf_Config.Channel = 39;
    rf_Config.Frequency = 2480000;
    rf_Config.LLEMode = LLE_MODE_BASIC | LLE_WHITENING_OFF;
    rf_Config.rfStatusCB = RF_2G4StatusCallBack;
    rf_Config.RxMaxlen = 251;

    RF_Config(&rf_Config);
}
```

### Step 5: 通道转换 (DTM 通道 -> BLE 通道)

```c
uint8_t Choose_CH(uint8_t cch) {
    if(cch == 0)  return 37;
    if(cch == 12) return 38;
    if(cch == 39) return 39;
    if(cch < 12)  return cch - 1;
    if(cch > 12)  return cch - 2;
    return 0;
}
```

### Step 6: TX 测试执行

```c
// TX 测试数据模式
void TX_DATA(uint8_t *buf, uint8_t len) {
    switch(buf[0]) {
        case 0: PRBS9_Get(&buf[2], len); break;   // PRBS9
        case 1: memset(&buf[2], 0xF0, len); break; // 11110000
        case 2: memset(&buf[2], 0xAA, len); break; // 10101010
        case 3: PRBS15_Get(&buf[2], len); break;   // PRBS15
        case 4: memset(&buf[2], 0xFF, len); break; // 全1
        case 5: memset(&buf[2], 0x00, len); break; // 全0
        case 6: memset(&buf[2], 0x0F, len); break; // 00001111
        case 7: memset(&buf[2], 0x55, len); break; // 01010101
    }
}

// TX 完成后自动重发
void RF_2G4StatusCallBack(uint8_t sta, uint8_t crc, uint8_t *rxBuf) {
    case TX_MODE_TX_FINISH:
        gTxCount++;
        ttflag = 1;  // 标记需要重发
        break;
}

// 在 DtmProcess 中重发
void DtmProcess(void) {
    UART_Process_Data();
    USB_Process_Data();
    if(ttflag) {
        ttflag = 0;
        TX_DATA(TxBuf, TxBuf[1]);
        rf_tx_start(TxBuf);
    }
}
```

### Step 7: DTM 响应格式

```c
// 命令完成事件
uint8_t cmdCompleteEvt[23] = {0x04, 0x0E};

// Test End 响应包含收发计数
cmdCompleteEvt[7] = (gTxCount) & 0xFF;
cmdCompleteEvt[8] = ((gTxCount) >> 8) & 0xFF;
m_UART_SendString(cmdCompleteEvt, 9);
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 命令解析失败 | 数据格式不对 | 确保首字节为 0x01，长度字段正确 |
| TX 计数为 0 | 通道未正确配置 | 检查 `Choose_CH` 转换结果 |
| UART 数据丢失 | 接收缓冲区太小 | 增大 RingMem 缓冲区 |
| 单载波不工作 | 未调用 `LL_SingleChannel` | 先调用 `LL_SingleChannel(ch)` 再发送 |

## 参考项目

- `resources/EXAM/BLE/Direct_Test_Mode/` — DTM 射频测试例程 (支持 UART 和 USB 接口)
