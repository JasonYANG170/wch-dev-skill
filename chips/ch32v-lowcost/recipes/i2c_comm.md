# Recipe: I2C Communication

## Scenario
Configure I2C for master/slave communication -- 7-bit polling, 7-bit interrupt, DMA transfer, and EEPROM read/write.

## Pin Mapping

| Signal | CH32V003 Pin | CH32L103 Pin |
|--------|-------------|-------------|
| I2C1_SCL | PC2 | PB6 (default) / PC2 (remap) |
| I2C1_SDA | PC1 | PB7 (default) / PC3 (remap) |

> I2C pins must be configured as **alternate-function open-drain** (`GPIO_Mode_AF_OD`).

## API Quick Reference

### I2C Init Structure
```c
typedef struct
{
    uint32_t I2C_ClockSpeed;          // Clock speed in Hz (100000 = 100kHz, 400000 = 400kHz)
    uint16_t I2C_Mode;                // I2C_Mode_I2C
    uint16_t I2C_DutyCycle;           // I2C_DutyCycle_2 (standard) or I2C_DutyCycle_16_9 (fast)
    uint16_t I2C_OwnAddress1;         // Own address (7-bit or 10-bit)
    uint16_t I2C_Ack;                 // I2C_Ack_Enable or I2C_Ack_Disable
    uint16_t I2C_AcknowledgedAddress; // I2C_AcknowledgedAddress_7bit or I2C_AcknowledgedAddress_10bit
} I2C_InitTypeDef;
```

### Key Functions
```c
void       I2C_Init(I2C_TypeDef *I2Cx, I2C_InitTypeDef *I2C_InitStruct);
void       I2C_Cmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_GenerateSTART(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_GenerateSTOP(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_Send7bitAddress(I2C_TypeDef *I2Cx, uint8_t Address, uint8_t I2C_Direction);
void       I2C_SendData(I2C_TypeDef *I2Cx, uint8_t Data);
uint8_t    I2C_ReceiveData(I2C_TypeDef *I2Cx);
void       I2C_AcknowledgeConfig(I2C_TypeDef *I2Cx, FunctionalState NewState);
void       I2C_ITConfig(I2C_TypeDef *I2Cx, uint16_t I2C_IT, FunctionalState NewState);
void       I2C_DMACmd(I2C_TypeDef *I2Cx, FunctionalState NewState);
FlagStatus I2C_GetFlagStatus(I2C_TypeDef *I2Cx, uint32_t I2C_FLAG);
ITStatus   I2C_CheckEvent(I2C_TypeDef *I2Cx, uint32_t I2C_EVENT);
void       I2C_NACKPositionConfig(I2C_TypeDef *I2Cx, uint16_t I2C_NACKPosition);
```

### Key Events
```c
I2C_EVENT_MASTER_MODE_SELECT                   // Start sent, ready for address
I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED     // Address+Write ACKed
I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED        // Address+Read ACKed
I2C_EVENT_MASTER_BYTE_TRANSMITTED              // Byte sent, ACK received
I2C_EVENT_MASTER_BYTE_RECEIVED                 // Byte received
I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED       // Slave: address matched (write)
```

### Key Flags
```c
I2C_FLAG_BUSY   // Bus busy
I2C_FLAG_TXE    // Data register empty (ready to transmit)
I2C_FLAG_RXNE   // Data register not empty (data received)
I2C_FLAG_SB     // Start bit generated
I2C_FLAG_ADDR   // Address sent/matched
I2C_FLAG_BTF    // Byte transfer finished
I2C_FLAG_TRA    // Transmitter/receiver mode
```

## Complete Call Chains

### I2C Master -- 7-bit Polling
```c
void IIC_Init(u32 bound, u16 address)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef I2C_InitTSturcture = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    // SCL -- PC2
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_30MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // SDA -- PC1
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    I2C_InitTSturcture.I2C_ClockSpeed = bound;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_2;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitTSturcture);

    I2C_Cmd(I2C1, ENABLE);
}

// Master transmit: send N bytes to slave at `slave_addr`
void I2C_Master_Send(u8 slave_addr, u8 *data, u8 len)
{
    u8 i;

    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET);

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Transmitter);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    for(i = 0; i < len; i++)
    {
        while(I2C_GetFlagStatus(I2C1, I2C_FLAG_TXE) == RESET);
        I2C_SendData(I2C1, data[i]);
    }

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C1, ENABLE);
}

// Master receive: read N bytes from slave at `slave_addr`
void I2C_Master_Recv(u8 slave_addr, u8 *buf, u8 len)
{
    u8 i;

    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET);

    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));

    I2C_Send7bitAddress(I2C1, slave_addr, I2C_Direction_Receiver);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    for(i = 0; i < len; i++)
    {
        if(i == len - 1)
        {
            I2C_AcknowledgeConfig(I2C1, DISABLE);  // NACK last byte
            I2C_GenerateSTOP(I2C1, ENABLE);
        }
        while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
        buf[i] = I2C_ReceiveData(I2C1);
    }

    I2C_AcknowledgeConfig(I2C1, ENABLE);  // Re-enable ACK for next transfer
}
```

### I2C Slave -- 7-bit Polling
```c
// Slave init (same IIC_Init, but with own address)
void IIC_Init(u32 bound, u16 address)
{
    // ... same GPIO and I2C config as master ...
    // address = slave's own address (e.g., 0x02)
}

// Slave receive loop
u8 RxData[6];
u8 i = 0;

while(!I2C_CheckEvent(I2C1, I2C_EVENT_SLAVE_RECEIVER_ADDRESS_MATCHED));
while(i < 6)
{
    if(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) != RESET)
    {
        RxData[i] = I2C_ReceiveData(I2C1);
        i++;
    }
}
```

### I2C Master -- Interrupt Mode
```c
volatile uint8_t master_state = 0;
volatile uint16_t master_send_len = 0;
volatile uint16_t master_recv_len = 0;

void IIC_Interrupt_Init(u32 bound, u16 address)
{
    GPIO_InitTypeDef GPIO_InitStructure = {0};
    I2C_InitTypeDef I2C_InitTSturcture = {0};
    NVIC_InitTypeDef NVIC_InitStructure = {0};

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    I2C_InitTSturcture.I2C_ClockSpeed = bound;
    I2C_InitTSturcture.I2C_Mode = I2C_Mode_I2C;
    I2C_InitTSturcture.I2C_DutyCycle = I2C_DutyCycle_16_9;
    I2C_InitTSturcture.I2C_OwnAddress1 = address;
    I2C_InitTSturcture.I2C_Ack = I2C_Ack_Enable;
    I2C_InitTSturcture.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_Init(I2C1, &I2C_InitTSturcture);
    I2C_Cmd(I2C1, ENABLE);

    // NVIC for I2C event and error interrupts
    NVIC_InitStructure.NVIC_IRQChannel = I2C1_EV_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);

    NVIC_InitStructure.NVIC_IRQChannel = I2C1_ER_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_Init(&NVIC_InitStructure);

    // Enable I2C interrupts
    I2C_ITConfig(I2C1, I2C_IT_BUF, ENABLE);
    I2C_ITConfig(I2C1, I2C_IT_EVT, ENABLE);
    I2C_ITConfig(I2C1, I2C_IT_ERR, ENABLE);
}

void I2C1_EV_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void I2C1_EV_IRQHandler(void)
{
    // Handle I2C events: SB, ADDR, TXE, RXNE, BTF, STOPF
    // Use I2C_GetITStatus() to check which event fired
    // Use I2C_Send7bitAddress(), I2C_SendData(), I2C_ReceiveData() in handler
}

void I2C1_ER_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));
void I2C1_ER_IRQHandler(void)
{
    // Handle errors: AF (ack failure), BERR, OVR
    if(I2C_GetITStatus(I2C1, I2C_IT_AF))
    {
        I2C_ClearITPendingBit(I2C1, I2C_IT_AF);
    }
}
```

### I2C with DMA
```c
// I2C Master TX via DMA (Channel 6)
void I2C_DMA_Tx_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel6);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&I2C1->DATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)TxData;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 6;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel6, &DMA_InitStructure);
}

// Enable I2C DMA before starting transfer
I2C_DMACmd(I2C1, ENABLE);

// After address phase, enable DMA channel
DMA_Cmd(DMA1_Channel6, ENABLE);

// I2C Slave RX via DMA (Channel 7)
void I2C_DMA_Rx_Init(void)
{
    DMA_InitTypeDef DMA_InitStructure = {0};

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(DMA1_Channel7);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&I2C1->DATAR;
    DMA_InitStructure.DMA_MemoryBaseAddr = (u32)RxData;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 6;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_High;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DMA1_Channel7, &DMA_InitStructure);
}
```

### EEPROM (AT24Cxx) Read/Write
```c
void AT24CXX_WriteOneByte(u16 WriteAddr, u8 DataToWrite)
{
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET);
    I2C_GenerateSTART(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, 0xA0, I2C_Direction_Transmitter);  // EEPROM write address
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, (u8)(WriteAddr & 0x00FF));  // 8-bit address
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    I2C_SendData(I2C1, DataToWrite);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
    I2C_GenerateSTOP(I2C1, ENABLE);
}

u8 AT24CXX_ReadOneByte(u16 ReadAddr)
{
    u8 temp;

    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY) != RESET);
    I2C_GenerateSTART(I2C1, ENABLE);

    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, 0xA0, I2C_Direction_Transmitter);  // Write address first
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));

    I2C_SendData(I2C1, (u8)(ReadAddr & 0x00FF));
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));

    // Repeated start for read
    I2C_GenerateSTART(I2C1, ENABLE);
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));
    I2C_Send7bitAddress(I2C1, 0xA0, I2C_Direction_Receiver);  // Read address
    while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED));

    I2C_AcknowledgeConfig(I2C1, DISABLE);  // NACK single byte
    while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET);
    temp = I2C_ReceiveData(I2C1);
    I2C_GenerateSTOP(I2C1, ENABLE);

    return temp;
}

// Usage:
AT24CXX_WriteOneByte(100, 0x55);
Delay_Ms(5);  // EEPROM write cycle time
u8 val = AT24CXX_ReadOneByte(100);  // returns 0x55
```

## Common Errors

1. **GPIO not AF_OD** -- I2C pins must be `GPIO_Mode_AF_OD`, not `GPIO_Mode_Out_PP`
2. **AFIO clock not enabled** -- Must enable `RCC_APB2Periph_AFIO` for I2C pin mapping
3. **Missing busy check** -- Always check `I2C_FLAG_BUSY` before generating START
4. **NACK not sent on last byte** -- Call `I2C_AcknowledgeConfig(I2C1, DISABLE)` before receiving the last byte
5. **EEPROM write timing** -- AT24Cxx needs ~5ms write cycle time; add `Delay_Ms(5)` after write
6. **10-bit EEPROM address** -- Use two `I2C_SendData()` calls for 16-bit address EEPROMs
