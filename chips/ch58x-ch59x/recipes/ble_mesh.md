# BLE Mesh

> **适用摘要**: 实现 CH58x/CH59x BLE Mesh 组网，包含阿里智能灯、厂商模型

## 触发意图

- "BLE Mesh"
- "蓝牙 Mesh 组网"
- "阿里智能灯"
- "Mesh 配网"

## 前置条件

| 条件 | 要求 |
|---|---|
| 头文件 | BLE + Mesh 相关头文件 |
| config.h | BLE 堆 ≥ 10KB |
| 库文件 | Mesh 库已链接 |

## 调用链

```
Step 1: 配置 Mesh 参数
Step 2: 初始化 BLE + Mesh 协议栈
Step 3: 配网 (Provisioning)
Step 4: 处理 Mesh 消息
```

## 分步说明

### Step 1: Mesh 初始化

```c
// Mesh 初始化在 BLE 初始化之后
CH58X_BLEInit();
HAL_Init();
Mesh_Init();  // Mesh 协议栈初始化
```

### Step 2: 阿里智能灯模型

```c
// 注册阿里灯模型
Ali_Light_Model_Init();

// 处理消息
void Mesh_MessageHandler(mesh_msg_t *msg) {
    switch (msg->opcode) {
        case ALI_LIGHT_ONOFF:
            // 开关控制
            break;
        case ALI_LIGHT_DIM:
            // 亮度调节
            break;
    }
}
```

## 常见错误

| 错误 | 原因 | 解决方法 |
|---|---|---|
| 配网失败 | OOB 数据不匹配 | 检查设备 UUID 和密钥 |
| 消息丢失 | TTL 过短 | 增加消息 TTL 值 |
| 内存不足 | Mesh 堆太小 | 增大 BLE_MEMHEAP_SIZE |

## 参考项目

- `resources/EXAM/BLE/MESH/adv_ali_light/`
- `resources/EXAM/BLE/MESH/adv_vendor/`
- `resources/EXAM/BLE/MESH/provisioner_vendor/`
