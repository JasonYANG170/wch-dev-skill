# EXAM 目录结构（CH583/CH585/CH592/CH595）

本文档覆盖 CH58x/CH59x 全系列 EXAM 目录结构。各芯片差异用注释标注。

## 公共例程（CH583/CH585/CH592/CH595 均包含）

```
EXAM/
├── ADC                             ADC采样例程
│   ├── TouchKey                    触摸按键检测
│   └── ...                         ADC单通道/差分/温度/电池电压采样
├── BLE                             蓝牙例程
│   ├── HAL/                        硬件抽象层（共享）
│   ├── LIB/                        BLE协议栈库（libCH58xBLE.a / libCH59xBLE.a）
│   ├── Peripheral                  BLE从机例程（广播、GATT服务、通知）
│   ├── Central                     BLE主机例程（扫描、连接、读写特征值）
│   ├── Broadcaster                 广播者角色例程（仅广播，不可连接）
│   ├── Observer                    观察者角色例程（仅扫描，不连接）
│   ├── CentPeri                    主从一体例程（Central+Peripheral双角色）
│   ├── MultiCentral                多连接主机例程（同时连接多个从机）
│   ├── MultiCentPeri               多连接主从一体例程
│   ├── HeartRate                   心率Profile例程（SIG标准Heart Rate Service）
│   ├── CyclingSensor               骑行传感器Profile例程
│   ├── RunningSensor               跑步传感器Profile例程
│   ├── SpeedTest_Central           BLE速率测试（主机端，统计接收速率）
│   ├── SpeedTest_Peripheral        BLE速率测试（从机端，持续发送通知）
│   ├── HID_Keyboard                HID键盘例程
│   ├── HID_Mouse                   HID鼠标例程
│   ├── HID_Consumer                HID消费者控制例程
│   ├── HID_Touch                   HID触摸例程
│   ├── RemoteController            遥控器例程
│   ├── BLE_UART                    BLE转串口透传例程（FIFO缓冲，MTU自适应）
│   ├── BLE_USB                     BLE转USB例程（RingMem缓冲中转）
│   ├── Direct_Test_Mode            BLE DTM射频测试例程（UART/USB接口，符合SIG规范）
│   ├── RF_PHY                      非标射频物理层例程（2.4G直接收发）
│   ├── RF_PHY_Hop                  跳频例程
│   ├── SYNC_ADV                    同步广播例程
│   ├── SYNC_SCAN                   同步扫描例程
│   ├── IoCHub_NET                  IoCHub组网例程
│   ├── LWNS                        轻量无线网络栈例程
│   ├── peripheral_ancs_client      ANCS客户端例程（接收iOS通知）
│   ├── BackupUpgrade_IAP           备份升级IAP例程
│   ├── BackupUpgrade_JumpIAP       备份升级跳转IAP例程
│   ├── BackupUpgrade_OTA           备份升级OTA例程
│   ├── OnlyUpdateApp_IAP           仅更新APP的IAP例程
│   ├── OnlyUpdateApp_JumpIAP       仅更新APP的跳转IAP例程
│   ├── OnlyUpdateApp_Peripheral    仅更新APP的从机OTA例程
│   ├── RemoteController_IAP        遥控器IAP例程
│   ├── RemoteController_JumpIAP    遥控器跳转IAP例程
│   └── MESH                        蓝牙Mesh例程
│       ├── adv_ali_light           阿里智能灯模型
│       ├── adv_proxy               广播代理
│       ├── adv_vendor              厂商模型
│       └── provisioner_vendor      配网器厂商模型
├── FLASH                           Flash读写例程
├── I2C                             I2C通信例程
├── IAP                             在线编程例程
│   ├── UART_IAP                    串口IAP例程
│   ├── USB_IAP                     USB IAP例程
│   └── APP                         IAP应用程序例程
├── LCD                             LCD显示例程（CH585/CH592/CH595）
├── PM                              电源管理例程
│   ├── Sleep                       睡眠模式例程
│   ├── Halt                        Halt模式例程
│   └── Shutdown                    关机模式例程
├── PWMX                            PWM输出例程
├── SPI                             SPI通信例程
│   ├── SPI0_Master                 SPI0主机例程
│   └── SPI0_Slave                  SPI0从机例程
├── TMR                             定时器例程
│   ├── Timer                       定时中断例程
│   ├── PWM                         定时器PWM输出例程
│   └── Capture                     输入捕获例程
├── TOUCH                           触摸按键例程
├── UART1                           串口通信例程
│   ├── UART1_Printf                串口打印例程
│   └── UART1_INT                   串口中断例程
├── USB                             USB例程
│   ├── Device                      USB设备例程
│   │   ├── COM                     USB虚拟串口例程
│   │   ├── HID                     USB HID设备例程
│   │   └── Vendor                  USB自定义设备例程
│   └── Host                        USB主机例程
│       ├── HostEnum                USB主机枚举例程
│       ├── HostAOA                 USB AOA例程
│       └── U_DISK                  U盘文件系统例程
├── FreeRTOS/                       FreeRTOS RTOS例程
├── HarmonyOS/                      HarmonyOS例程
└── RT-Thread/                      RT-Thread RTOS例程
```

## CH585 独有例程

```
├── NFCA                            NFC-A读卡器例程（CH585 only）
│   ├── NFCA_LIB                    NFC-A协议库
│   │   └── CH58x_NFCA_LIB.h       NFC-A API头文件（PCD/PICC/CRYPTO1/SOFT_PCD）
│   ├── PCD                         硬件PCD（Proximity Coupling Device）读卡器
│   │   ├── PCD_COMMON/             PCD公共BSP和配置
│   │   ├── MifareClassic/          Mifare Classic读卡例程
│   │   └── src/main.c
│   ├── PCD_SOFT                    软件PCD读卡器（使用TMR0中断解码）
│   │   ├── PCD_SOFT_COMMON/        软件PCD公共BSP和配置
│   │   ├── SoftPCDMifareClassic/   软件PCD Mifare Classic例程
│   │   └── src/main.c
│   └── PICC                        PICC（Proximity Inductive Coupling Card）模拟
│       ├── PICC_COMMON/            PICC公共BSP和配置
│       ├── PICC_M1/                Mifare Classic卡片模拟例程
│       ├── PICC_T2T/               NFC Type 2 Tag模拟例程
│       └── src/main.c
├── LED                             LED驱动例程（CH585/CH595）—— DMA驱动多通道LED输出
└── CH585 NFC读卡器应用指南.pdf     NFC-A硬件设计指南
```

## CH595 独有例程

```
├── ENCODER                         旋转编码器例程（CH595 only）
│   └── src/Main.c                  编码器计数、方向检测、睡眠唤醒
├── KEYSCAN                         按键扫描例程（CH595 only）
│   └── src/Main.c                  矩阵键盘扫描、按键中断、睡眠唤醒
└── LED                             LED驱动例程（CH585/CH595）—— DMA驱动多通道LED输出
```

## 芯片差异汇总

| 特性 | CH583 | CH585 | CH592 | CH595 |
|------|-------|-------|-------|-------|
| Flash | 448KB | 448KB | 448KB | 240KB |
| RAM | 32KB | 128KB | 26KB | 32KB |
| NFCA (NFC-A) | - | **Y** | - | - |
| ENCODER | - | - | - | **Y** |
| KEYSCAN | - | - | - | **Y** |
| LED控制器 | - | **Y** | - | **Y** |
| LCD | - | **Y** | **Y** | **Y** |
| BLE | Y | Y | Y | Y |
| Dual USB | Y | Y | - | - |
