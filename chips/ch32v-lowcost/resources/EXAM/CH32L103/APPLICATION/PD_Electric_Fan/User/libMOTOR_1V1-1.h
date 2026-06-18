/*
 * libCH32L103_MOTOR.h
 *
 *  Created on: May 20, 2024
 *      Author: zcc
 */

#ifndef USBPD_LIBMOTOR_1V1_1_H_
#define USBPD_LIBMOTOR_1V1_1_H_

#include "ch32l103_usbpd.h"

typedef struct {
    union {
        u16 Data;
        struct {
            u16 MsgType:5;
            u16 PortDataRole:1;
            u16 SpecRevision:2;
            u16 PortPwrRole:1;
            u16 MsgID:3;
            u16 NDO:3;
            u16 Extended:1;
        };
    };
} st_Prot_Header;




typedef struct {
    union {
        u32 Data;
        struct {
            u32 MaxCurrent:10;
            u32 Current:10;
            u32 Reserved:2;
            u32 EPRMode:1;
            u32 UnchunkedExtended:1;
            u32 NoUSBSuspend:1;
            u32 USBComm:1;
            u32 CapbilityMismatch:1;
            u32 GiveBack:1;
            u32 ObjectPos:4;
        };
    };
} st_Request_Fixed;



typedef struct {

    void (*pDevChk)();
    void (*pUserUnattached)();      //连接成功调用用户程序
    void (*pUserAttached)();        //连接移除调用用户程序

    u8 DevRole;     //Sink   Src   DRP   DRP.TrySink   DRP.TrySrc
    u8 DevStat;     //0:Sink   1:Src

    u8 ConnectStat;


    u16 Cnt;
    u16 TryCnt;
    u16 Timeout;

    uint16_t *PortCC;       //使用的CC的端口控制寄存器
    uint32_t GpioCC;        //使用的CC的GPIO

    uint16_t *PortVconn;        //使用的CC的端口控制寄存器
    uint32_t GpioVconn;     //使用的CC的GPIO

} st_PD_DEVICE;



typedef struct {

                //  SOP     SOP'    SOP"    HRST    CRST
    u8 RxSop;   //  01      10      11      10      11
    u8 TxSop;   //  0x00    0x50    0x44    0xFF    0x73
    u8 Rxfact;

    u8 TxMsgID;
    u8 TxEmarkMsgID;
    u8 RxMsgID;
    u8 RxEmarkMsgID;
    u8 RetryCnt;
    u8 PDExist;             //首包消息不检查MsgID
    u8 PDVconnExist;
    u16 IdleCnt;                //PD Idle计数



    st_Prot_Header Header;      //当前Header默认值，包括DRP状态

    u8 WaitTxGcrc;
    u8 WaitRxGcrc;

    u8 WaitMsgTx;       //等待发送消息标志
    u16 MsgTxCnt;       //消息发送计时ms

    u8 WaitMsgRx;       //等待接收消息标志
    u16 MsgRxCnt;       //消息接收超时ms

    u8 VoltChanging;        //调压标志位
    u8 tSrcTransition;      //等待开始调压    25-35
    u16 tPSTransition;      //调压阶段      450-550

    void (*pRxFinish)();        //接收成功：接收到消息并已回复GoodCRC
    void (*pRxTimeout)();       //接收超时：未能在定时时间内收取到消息，不判断收取消息的类型
    void (*pTxFinish)();        //发送成功：发送消息成功并已收取到GoodCRC
    void (*pTxTimeout)();       //发送超时：重试次数内未能收取到正确的GoodCRC
    void (*pRxHRST)();          //接收到HRST，及发送完HRST

    u8 RxFinish;
    u8 RxHRST;
    u8 TxFinish;


    u32 rxSrcCap[7];
    u8  rxSrcCapCnt;
    st_Request_Fixed savedRequest;

    u8 RequestType;
    u8 PPS_success;
} st_PD_PHY;


extern st_PD_PHY PD_PHY;
extern st_PD_DEVICE PD_DEVICE;



void pProt_TX_Request(void);
void PD_Lib_Check_Version(void);
void Set_DevChk(u8 Role);
void PD_Init(void);
void pProt_TX_PS_RDY(void);
void PD_TX_GCRC(void);
void PD_TX_MSG(void);

#endif /* USBPD_LIBMOTOR_1V1_1_H_ */
