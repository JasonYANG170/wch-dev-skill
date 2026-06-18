/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/27
* Description        : CH549 Type-C PD使用
*                      工作在DFP模式，自动检测UFP设备正反插，控制对应的CC脚PD通讯
*                      作为PD通讯的供电端
*                      注意：该程序测试条件：32Mhz,+5V VDD,38400bps
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "CH549.H"
#include "DEBUG.H"
#include "PD.H"
#pragma  NOAREGS
/* UFP连接、断开状态
 * 0:断开
 * 1:连接
 * 2:PD通讯完成 */
UINT8X Connect_Status = 0;                                                      //UFP连接状态
#if (POWER_ROLE == SOURCE)
/*******************************************************************************
* Function Name  : main
* Description    : 程序主入口
* Input          : None
* Return         : None
*******************************************************************************/
void main( )
{
    UINT8 status;
    UINT8 rev_flag = 0;
    UINT16 j = 0;
    CfgFsys( );                                                                //CH549时钟选择配置
    mDelaymS(20);                                                              //修改主频建议稍加延时等待芯片供电稳定
    mInitSTDIO( );                                                             //串口0初始化
    printf("PD Source start ...\n");
    Timer0Init();                                                              //Timer0初始化 3.33us定时用于查询发送
    PD_Source_Init();                                                          //PD接口初始化
    while(1)
    {
        status = DFP_Insert();
        if( status )                                                           //UFP连接
        {
            if(Connect_Status == 0)
            {
                printf("connected\n");
                Connect_Status = 1;
                CCSel = status;                                                //CC脚选择
            }
        }
        else
        {
            if(Connect_Status)                                                 //清除状态
            {
                printf("disconnected\n");
                Connect_Status = 0;
                CCSel = 0;
                VBUSG = 1;                                                     //关闭VBUS电源
            }
        }
        if(Connect_Status == 1)                                                //已连接
        {
            VBUSG = 0;                                                         //打开VBUS电源
            mDelaymS(300);
            printf("CC%d Start Communication\n",(UINT16)CCSel);
            /* 1、填充Header,准备发送 Source Send Capbility指令 */
            ResetSndHeader();
//          Union_Header->HeaderStruct.NDO = 2;                                //3 Byte数据（最大7）
//          Union_Header->HeaderStruct.MsgType = SourceSendCap;
//          /* 2、填充两组Sink可选择的电压电流值 */
//          memcpy(&SndDataBuf[2],SrcCap_Table,8);
            /* 3、调用底层发送函数(添加PRE、SOF、CRC32、EOP、4B5B编码、BMC编码) */
            Config_SourceCap();
            if(SendHandle(CCSel) == 0)                                         //发送成功,准备接收
            {
                while(1)
                {
                    rev_flag = ReceiveHandle ( CCSel );
                    if(rev_flag)                                               //有新数据到达
                    {
                        if (GetMsgType() == SINK_SEND_REQUEST)
                        {
                            ResetSndHeader();                                  //回复 Accept
                            SetMsgType(SOURCE_SEND_ACCEPT);
                            if(SendHandle(CCSel)==0)
                            {
                                mDelaymS(150);
                                /* 准备电压调整 */
                                ResetSndHeader();                               //回复 PS_RDY
                                SetMsgType(PS_RDY);                             //回复 Accept
                                if(SendHandle(CCSel)==0)
                                {
                                    printf("PD PS_RDY Success.\n");
                                    Connect_Status = 2;                         //通讯结束
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
#else
void main( )
{
    UINT8 index;
    UINT8 status;
    UINT8 msgType;
    UINT16 Voltage;
    CfgFsys();                                                                  /* CH549时钟选择配置 */
    mDelaymS(20);                                                               /* 修改主频建议稍加延时等待芯片供电稳定 */
    mInitSTDIO();                                                               /* 串口0初始化 */
    printf("PD Sink start ...\n");
    Timer0Init();                                                                           /* Timer0初始化 3.33us定时用于查询发送 */
    PD_Sink_Init();                                                             /* PD接口初始化 */
    Voltage = 14800;                                                                                          /* 请求电压，单位mV */
    while(1)
    {
        /* 检测DFP的连接，确定CC通讯管脚 */
        status = Connect_Check();
        if( status != DFP_DISCONNECT )                                            /* DFP连接 */
        {
            if(Connect_Status == 0)
            {
                printf("cc:%d,connected:%d\n",(UINT16)CCSel,(UINT16)status);
                Connect_Status = DFP_STAT_CONNECT;
            }
        }
        else                                                                     /* DFP未连接 */
        {
            if(Connect_Status)                                                     /* 清除状态 */
            {
                printf("disconnected:%d\n",(UINT16)status);
                Connect_Status = DFP_STAT_DISCONNECT;
                VBUSG = 0;                                                           /* 关闭VBUS电源 */
            }
        }
        while(Connect_Status)                                                    /* 已连接，此处数据接收处理 */
        {
            status = ReceiveHandle ( CCSel );
            if(status==0)                                                          /* 有新数据到达 */
            {
                msgType = GetMsgType();
                switch (msgType)
                {
                case SOURCE_SEND_CAP:
                    index = SearchVoltage ( Voltage );                                 /* 电压选择,单位 mV */
                    if(index != 0xFF)
                    {
                        printf("Matched Volt: %dmV\n", Voltage);
                        PrepareReqData(index);
                        SendHandle(CCSel);                                                             /* 发送请求电源消息 */
                    }
                    else
                    {
                        printf("No Matched Volt.\n");
                    }
                    break;
                case SOURCE_SEND_ACCEPT:
                    printf("Accept\n");
                    break;
                case PS_RDY:
                    printf("Ready\n");
                    VBUSG = 0;                                                       /* PD通讯完成， 状态标志置位 */
                    break;
                case REJECT:
                    printf("Reject\n");
                    break;
                default :
                    printf("MsgType: %02x\n",(UINT16)msgType);
                    break;
                }
            }
//          status = Connect_Check();
        }
    }
}
#endif
