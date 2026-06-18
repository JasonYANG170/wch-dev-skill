/********************************** (C) COPYRIGHT ******************************
* File Name          : PING.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563NET库PING功能函数
*                      (1)、CH563 Examples by KEIL3_V3.53;
*                      (2)、串口0输出监控信息,115200bps;
*                      (3)、ARM9-CH563单片机C语言的PING功能演示程序
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CH563BAS.H"
#include "CH563NET.H"
#include "PING.H"

/* 变量定义 */
UINT8  SENDERR_COUNT;
UINT8  UNREACH_COUNT;
UINT8  TIMOUT_COUNT;
UINT8  SUCRECV_COUNT;
UINT8  SEND_BUF[100];
UINT8  IcmpCont=0;
UINT8  IcmpSeq=0;
UINT8  IcmpSuc=0;
UINT8  icmp_tmp=0;
UINT32 TimeCount;

/**********************************************************************************
* Function Name  : InitParameter
* Description    : 变量初始化
* Input          : None
* Output         : None
* Return         : None
**********************************************************************************/
void InitParameter( void )
{
    UNREACH_COUNT=0;
    TIMOUT_COUNT=0;
    SUCRECV_COUNT=0;
    IcmpSuc = ICMP_KEEP_NO;
}

/**********************************************************************************
* Function Name  : InitPing
* Description    : Ping初始化
* Input          : None
* Output         : None
* Return         : None
**********************************************************************************/
void InitPing( void )
{
    IcmpHeader head;
    UINT32 check_sum=0;
    UINT8 i;

     IcmpCont++;
    IcmpSeq += 10;
    head.i_type = ICMP_HEAD_TYPE;
    head.i_code = ICMP_HEAD_CODE;
    head.i_id   = ICMP_HEAD_ID;
    head.i_seq  = ICMP_HEAD_SEQ+IcmpSeq; 
    memset(head.i_data,0,sizeof(head.i_data));
    for( i=0;i<ICMP_DATA_BYTES;i++ ){
        if(i<26) head.i_data[i] = i + 'a';
        else head.i_data[i] = i + 'a' - 26;
        if(i%2==0) check_sum += head.i_data[i]<<8;
        else check_sum += head.i_data[i]; 
    }
    check_sum += head.i_type<<8;
    check_sum += head.i_code;
    check_sum += head.i_id;
    check_sum += head.i_seq;
    head.i_cksum = check_sum>>16;
    head.i_cksum += check_sum&0xffff;
    head.i_cksum = 0xffff - head.i_cksum;
    memset(SEND_BUF,0,sizeof(SEND_BUF));
    memcpy(SEND_BUF,&head,sizeof(head));
    SEND_BUF[2] = head.i_cksum>>8;
    SEND_BUF[3] = head.i_cksum&0xff;
    SEND_BUF[4] = head.i_id>>8;
    SEND_BUF[5] = head.i_id&0xff;
    SEND_BUF[6] = head.i_seq>>8;
    SEND_BUF[7] = head.i_seq&0xff;
}

/**********************************************************************************
* Function Name  : Respond_Ping
* Description    : 初始化回应PING包的数据
* Input          : None
* Output         : None
* Return         : None
**********************************************************************************/
void Respond_Ping( UINT8 *pDat )
{
    IcmpHeader head;
    UINT32 check_sum=0;
    UINT8 i;

    head.i_type = ICMP_HEAD_REPLY;
    head.i_code = pDat[1];
    head.i_id   = pDat[4]<<8 + pDat[5];
    head.i_seq  = pDat[6]<<8 + pDat[7]; 

    check_sum += head.i_type<<8;
    check_sum += head.i_code;
    check_sum += head.i_id;
    check_sum += head.i_seq;
    for( i=0;i<32;i++ ){
        head.i_data[i] = pDat[i+8];
        if(i%2==0) check_sum += head.i_data[i]<<8;
        else check_sum += head.i_data[i]; 
    }
    head.i_cksum = check_sum>>16;
    head.i_cksum += check_sum&0xffff;
    head.i_cksum = 0xffff - head.i_cksum;
    memset(SEND_BUF,0,sizeof(SEND_BUF));
    memcpy(SEND_BUF,&head,sizeof(head));
    SEND_BUF[2] = head.i_cksum>>8;
    SEND_BUF[3] = head.i_cksum&0xff;
    SEND_BUF[4] = head.i_id>>8;
    SEND_BUF[5] = head.i_id&0xff;
    SEND_BUF[6] = head.i_seq>>8;
    SEND_BUF[7] = head.i_seq&0xff;
}

/**********************************************************************************
* Function Name  : CH563NET_IcmpRecvData
* Description    : 接收数据
* Input          : None
* Output         : None
* Return         : None
**********************************************************************************/
void CH563NET_IcmpRecvData( UINT32 len, UINT8 *pDat )
{
    UINT16 tmp=0;

    icmp_tmp = IcmpSuc; 
    IcmpSuc = 3;
    if( len == 40 ){
        if( pDat[0] == ICMP_HEAD_REPLY ){
            if( pDat[1] == ICMP_HEAD_CODE ){
                tmp = pDat[4];
                tmp = tmp<<8; 
                tmp += pDat[5]; 
                if( tmp == ICMP_HEAD_ID ){
                    tmp = pDat[6];
                    tmp = (tmp<<8); 
                    tmp += pDat[7] - IcmpSeq; 
                    if( tmp == ICMP_HEAD_SEQ ){
                        IcmpSuc = 4;
                    }
                }
            }
        }
        if( pDat[0] == ICMP_HEAD_TYPE ){
            if( pDat[1] == ICMP_HEAD_CODE ){
                Respond_Ping(pDat);
                IcmpSuc = 6;
            }
        }
    }
    else{
        if( pDat[0] == 3 ){
            if( pDat[1] == 1 ){
                IcmpSuc = 5;
            }
        }
    }
}
/**********************************************************************************
* Function Name  : CH563NET_PINGCmd
* Description    : 查询状态执行相应命令
* Input          : None
* Output         : None
* Return         : None
**********************************************************************************/
void CH563NET_PINGCmd( void )                                          
{
    if( IcmpSuc<ICMP_KEEP_NO ){
        switch(IcmpSuc){
            case ICMP_SOKE_CON:
                IcmpSuc = 1;
                TimeCount = 0;
                CH563NET_PINGSendData( SEND_BUF,40,0);
                printf( "\nPinging %d.%d.%d.%d with %d bytes of data:\n\r\n",(UINT16)DestIP[0],(UINT16)DestIP[1],\
                    (UINT16)DestIP[2],(UINT16)DestIP[3],(UINT16)ICMP_DATA_BYTES );
                break;
            case ICMP_SEND_ERR:
                if(SENDERR_COUNT>10){ 
                    IcmpSuc = ICMP_KEEP_NO;
                    SENDERR_COUNT = 0;
                }
                if(TimeCount>250){
                    printf("send data fail!\n");
                    TimeCount = 0;
                    SENDERR_COUNT++;
                    CH563NET_PINGSendData(SEND_BUF,40,0);
                }
                break;
            case ICMP_SEND_SUC:    
                if( TimeCount>1000 ){
                    printf("Request time out.\n");
                    TIMOUT_COUNT++;
                    if(IcmpCont < 4){
                        IcmpSuc = 1;
                        InitPing( );
                        TimeCount = 0;
                        CH563NET_PINGSendData(SEND_BUF,40,0);
                    }
                    else{
                        printf("\r\nPing statistics for %d.%d.%d.%d:\n    Packets: Sent = 4,Received = %d,Lost = %d<%d%% loss>.\n\r\n",\
                            (UINT16)DestIP[0],(UINT16)DestIP[1],(UINT16)DestIP[2],(UINT16)DestIP[3],(UINT16)(4-TIMOUT_COUNT),\
                            (UINT16)TIMOUT_COUNT,(UINT16)(TIMOUT_COUNT*25));
                        IcmpSuc = ICMP_KEEP_NO;
                    }
                }
                break;
            case ICMP_RECV_ERR: // error unknow
                printf("recevie data unknow.\n\r\n");
                IcmpSuc = ICMP_KEEP_NO;
                break;
            case ICMP_RECV_SUC:    // sucess
                printf( "Reply from %d.%d.%d.%d: bytes=%d time<4ms\n",(UINT16)DestIP[0],(UINT16)DestIP[1],(UINT16)DestIP[2],\
                    (UINT16)DestIP[3],(UINT16)ICMP_DATA_BYTES );
                SUCRECV_COUNT++;
                if(IcmpCont < 4){
                    IcmpSuc = 1;
                    InitPing( );
                    TimeCount = 0;
                    CH563NET_PINGSendData(SEND_BUF,40,0);
                }
                else{
                    printf("\r\nPing statistics for %d.%d.%d.%d:\n    Packets: Sent = 4,Received = %d,Lost = %d<%d%% loss>.\n\r\n",(UINT16)DestIP[0],\
                        (UINT16)DestIP[1],(UINT16)DestIP[2],(UINT16)DestIP[3],(UINT16)SUCRECV_COUNT,(UINT16)(4-SUCRECV_COUNT),\
                        (UINT16)((4-SUCRECV_COUNT)*25));
                    IcmpSuc = ICMP_KEEP_NO;
                }
                break;
            case ICMP_UNRECH: // unreachable
                printf("Reply from %d.%d.%d.%d: Destination host unreachable.\n",(UINT16)DestIP[0],(UINT16)DestIP[1],\
                    (UINT16)DestIP[2],(UINT16)DestIP[3]);
                UNREACH_COUNT++;
                if(IcmpCont < 4){
                    IcmpSuc = 1;
                    InitPing( );
                    TimeCount = 0;
                    CH563NET_PINGSendData(SEND_BUF,40,0);
                }
                else{
                    printf("\r\nPing statistics for %d.%d.%d.%d:\n    Packets: Sent = 4,Received = %d,Lost = %d<%d%% loss>.\n\r\n",(UINT16)DestIP[0],\
                        (UINT16)DestIP[1],(UINT16)DestIP[2],(UINT16)DestIP[3],(UINT16)UNREACH_COUNT,(UINT16)(4-UNREACH_COUNT),\
                        (UINT16)((4-UNREACH_COUNT)*25));
                    IcmpSuc = ICMP_KEEP_NO;
                }
                break;
            case ICMP_REPLY: // icmp ping 包
                CH563NET_PINGSendData( SEND_BUF,40,0 );
                break;
            case ICMP_REPLY_SUC: // icmp ping 包
                printf( "Reply ping.\n\r\n" );
                IcmpSuc = icmp_tmp;
                break;
            case ICMP_KEEP_NO:
                break;
            default:
                break;
        }
    }
}

/*********************************** endfile **********************************/
