# CH57x BLE Stack API Quick Reference

## BLE Library Initialization

```c
// Header: CH57xBLE_LIB.h

// Initialize BLE library (must be called before any BLE API)
void BLE_LibInit(bleConfig_t *cfg);

// Configuration structure
typedef struct {
    uint32_t MEMAddr;        // Memory heap address (MEM_BUF)
    uint32_t MEMLen;         // Memory heap size (BLE_MEMHEAP_SIZE)
    uint32_t BufMaxLen;      // Max ATT payload length + 4
    uint32_t BufNumber;      // Number of BLE buffers
    uint32_t TxNumEvent;     // TX events per connection event
    uint32_t TxPower;        // TX power level
    uint32_t SNVAddr;        // SNV (Simple NV) Flash address
    uint32_t SelRTCClock;    // RTC clock source (CLK_OSC32K)
    uint32_t ConnectNumber;  // Packed: (peri & 3) | (centr << 2)
    pfnSrandCB   srandCB;    // Random seed callback
    pfnTsCB      tsCB;       // Temperature sensor callback
    pfnRCB       rcCB;        // RC calibration callback
    pfnSleepCB   sleepCB;    // Sleep callback (CH57X_LowPower)
} bleConfig_t;

// TX Power levels
LL_TX_POWEER_0_DBM       // 0 dBm (default)
LL_TX_POWEER_MINUS_6_DBM // -6 dBm
LL_TX_POWEER_MINUS_20_DBM // -20 dBm (low power)

// RTC clock sources
CLK_OSC32K               // Internal 32kHz (default)
CLK_OSC32768             // Internal 32.768kHz
```

## GAP (Generic Access Profile)

```c
// Set GAP parameters
void GAP_SetParamValue(uint16_t param, uint16_t value);

// Common parameters
TGAP_DISC_ADV_INT_MIN    // Min advertising interval (in 0.625ms units)
TGAP_DISC_ADV_INT_MAX    // Max advertising interval
TGAP_CONN_EST_INT_MIN    // Min connection interval
TGAP_CONN_EST_INT_MAX    // Max connection interval
TGAP_CONN_EST_LATENCY    // Connection latency
TGAP_CONN_EST_SUPERV_TIMEOUT // Supervision timeout

// Scan parameters
TGAP_SCAN_RSP_DATA_MAX   // Max scan response data length
```

## GAP Peripheral Role

```c
// Header: peripheral.h (or generated)

// Initialize peripheral role
void GAPRole_PeripheralInit(void);

// Set parameters
void GAPRole_SetParameter(uint16_t param, uint8_t len, void *pValue);

// Parameters
GAPROLE_ADVERT_ENABLED     // uint8_t: 1=enable, 0=disable
GAPROLE_ADVERT_DATA        // uint8_t[31]: advertising data
GAPROLE_SCAN_RSP_DATA      // uint8_t[31]: scan response data
GAPROLE_MIN_CONN_INTERVAL  // uint16_t: min connection interval (1.25ms units)
GAPROLE_MAX_CONN_INTERVAL  // uint16_t: max connection interval
GAPROLE_SLAVE_LATENCY      // uint16_t: slave latency
GAPROLE_TIMEOUT_MULTIPLIER // uint16_t: supervision timeout (10ms units)

// Start device (registers callbacks, starts advertising)
void GAPRole_PeripheralStartDevice(uint8_t taskId, gapBondCBs_t *pCBs, gapRolesCBs_t *pAppCallbacks);

// Callback structure
typedef struct {
    gapRolesStateNotify_t pfnStateChange;  // State change callback
    gapRolesRssiRead_t    pfnRssiRead;     // RSSI read callback
    gapRolesParamUpdateCB_t pfnParamUpdate; // Param update callback
} gapRolesCBs_t;

// State notification callback signature
void StateNotificationCB(gapRole_States_t newState, gapRoleEvent_t *pEvent);

// Connection parameter update request
void GAPRole_PeripheralConnParamUpdateReq(uint16_t connHandle,
    uint16_t minInterval, uint16_t maxInterval,
    uint16_t latency, uint16_t timeout,
    uint8_t taskId);
```

## GAP Central Role

```c
// Initialize central role
void GAPRole_CentralInit(void);

// Start discovery (scanning)
void GAPRole_CentralStartDiscovery(uint8_t mode, uint8_t activeScan, uint8_t whiteList);
// mode: DEVDISC_MODE_ALL, DEVDISC_MODE_GENERAL, DEVDISC_MODE_LIMITED

// Cancel discovery
void GAPRole_CentralCancelDiscovery(void);

// Establish connection
void GAPRole_CentralEstablishLink(uint8_t highDutyCycle, uint8_t whiteList,
                                   uint8_t addrType, uint8_t *peerAddr);

// Terminate connection
void GAPRole_TerminateLink(uint16_t connHandle);

// Update link parameters
void GAPRole_UpdateLink(uint16_t connHandle,
                         uint16_t intervalMin, uint16_t intervalMax,
                         uint16_t latency, uint16_t timeout);
```

## GAP Broadcaster / Observer

```c
// Broadcaster (advertising only, no connections)
void GAPRole_BroadcasterInit(void);
void GAPRole_BroadcasterStartDevice(void);

// Observer (scanning only, no connections)
void GAPRole_ObserverInit(void);
void GAPRole_ObserverStartDiscovery(uint8_t mode, uint8_t activeScan);
```

## GATT Operations

```c
// Header: CH57xBLE_LIB.h

// Service registration
void GGS_AddService(uint32_t services);       // GAP service
void GATTServApp_AddService(uint32_t services); // GATT service
void DevInfo_AddService(void);                 // Device Information Service

// Register service
uint8_t GATTServApp_RegisterService(gattAttribute_t *attrs, uint16_t numAttrs,
                                     uint8_t encKeySize, gattServiceCBs_t *pCBs);

// Callback structure
typedef struct {
    pfnGATTReadAttrCB_t   pfnReadAttrCB;
    pfnGATTWriteAttrCB_t  pfnWriteAttrCB;
    pfnGATTAuthorizeAttrCB_t pfnAuthorizeAttrCB;
} gattServiceCBs_t;

// Notification
uint8_t GATT_Notification(uint16_t connHandle, attHandleValueNoti_t *pNoti, uint8_t authenticated);

// Indication
uint8_t GATT_Indication(uint16_t connHandle, attHandleValueInd_t *pInd,
                         uint8_t authenticated, uint8_t taskId);

// Read/write (for central)
uint8_t GATT_ReadCharValue(uint16_t connHandle, attReadReq_t *pReq, uint8_t taskId);
uint8_t GATT_WriteCharValue(uint16_t connHandle, attWriteReq_t *pReq, uint8_t taskId);
uint8_t GATT_WriteNoRsp(uint16_t connHandle, attWriteReq_t *pReq);

// Service discovery
uint8_t GATT_DiscAllPrimaryServices(uint16_t connHandle, uint8_t taskId);
uint8_t GATT_DiscAllChars(uint16_t connHandle, uint16_t startHandle, uint16_t endHandle, uint8_t taskId);
uint8_t GATT_DiscAllCharDescs(uint16_t connHandle, uint16_t startHandle, uint16_t endHandle, uint8_t taskId);

// Exchange MTU
uint8_t GATT_ExchangeMTU(uint16_t connHandle, attExchangeMTUReq_t *pReq, uint8_t taskId);
```

## TMOS (Task Management OS)

```c
// Register event processing function
uint8_t TMOS_ProcessEventRegister(pfnTaskEventHandler callback);

// Message handling
uint8_t *tmos_msg_receive(uint8_t taskId);
uint8_t tmos_msg_send(uint8_t taskId, uint8_t *pMsg);
uint8_t tmos_msg_deallocate(uint8_t *pMsg);

// Timer
tmosTimer_t *tmos_mem_alloc(uint16_t size);
void tmos_mem_free(void *ptr);
tmos_start_task(uint8_t taskId, uint16_t events, uint16_t timeout);

// System process (must be called in main loop)
uint16_t TMOS_SystemProcess(void);

// Events
#define SYS_EVENT_MSG    0x8000  // System message event
```

## GATT Attribute Table Definition

```c
// Attribute table entry
typedef struct {
    attAttrType_t type;      // UUID type and value
    uint8_t permissions;     // Read/write permissions
    uint16_t handle;         // Attribute handle (auto-assigned)
    uint8_t *pValue;         // Pointer to attribute value
} gattAttribute_t;

// Permissions
GATT_PERMIT_READ             0x01
GATT_PERMIT_WRITE            0x02
GATT_PERMIT_AUTHEN_READ      0x04
GATT_PERMIT_AUTHEN_WRITE     0x08
GATT_PERMIT_ENCRYPT_READ     0x40
GATT_PERMIT_ENCRYPT_WRITE    0x80

// Characteristic properties
GATT_PROP_BCAST              0x01
GATT_PROP_READ               0x02
GATT_PROP_WRITE_NO_RSP       0x04
GATT_PROP_WRITE              0x08
GATT_PROP_NOTIFY             0x10
GATT_PROP_INDICATE           0x20

// UUID sizes
ATT_BT_UUID_SIZE             2   // 16-bit Bluetooth UUID
ATT_UUID_SIZE                16  // 128-bit custom UUID
```

## Advertising Data Format

```c
// Advertising data type IDs
GAP_ADTYPE_FLAGS                    0x01
GAP_ADTYPE_16BIT_MORE               0x02
GAP_ADTYPE_16BIT_COMPLETE           0x03
GAP_ADTYPE_LOCAL_NAME_SHORT         0x08
GAP_ADTYPE_LOCAL_NAME_COMPLETE      0x09
GAP_ADTYPE_POWER_LEVEL              0x0A
GAP_ADTYPE_SLAVE_CONN_INTERVAL_RANGE 0x12

// Example advertising data
static uint8_t advertData[] = {
    0x02, GAP_ADTYPE_FLAGS, GAP_ADTYPE_GENERAL_DISCOVERABLE | GAP_ADTYPE_BREDR_NOT_SUPPORTED,
    0x07, GAP_ADTYPE_LOCAL_NAME_COMPLETE, 'C', 'H', '5', '7', '3', 'x',
};

// Max advertising data length
#define B_MAX_ADV_LEN    31
```

## Constants

```c
#define ATT_MTU_SIZE           23    // Default ATT MTU
#define ATT_MAX_MTU_SIZE       247   // Maximum ATT MTU
#define B_ADDR_LEN             6     // BLE address length
#define INVALID_CONNHANDLE     0xFFFF
#define GATT_ALL_SERVICES      0xFFFFFFFF
```

## Role States

```c
typedef enum {
    GAPROLE_INIT = 0,
    GAPROLE_STARTED,
    GAPROLE_ADVERTISING,
    GAPROLE_WAITING,
    GAPROLE_CONNECTED,
    GAPROLE_CONNECTED_ADV,
    GAPROLE_ERROR
} gapRole_States_t;
```

## GAP Event Types

```c
#define GAP_DEVICE_INIT_DONE_EVENT      0x00
#define GAP_DEVICE_DISCOVERY_EVENT      0x01
#define GAP_ADV_DATA_UPDATE_EVENT       0x02
#define GAP_MAKE_DISCOVERABLE_DONE_EVENT 0x03
#define GAP_END_DISCOVERABLE_DONE_EVENT  0x04
#define GAP_LINK_ESTABLISHED_EVENT      0x05
#define GAP_LINK_TERMINATED_EVENT       0x06
#define GAP_LINK_PARAM_UPDATE_EVENT     0x07
```
