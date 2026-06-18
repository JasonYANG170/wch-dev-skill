
#ifndef _TFTP_H_
#define _TFTP_H_

#include "CH563BAS.H"


/*******************TFTP报文操作码定义*****************/
#define TFTP_RRQ          01                                //读请求
#define TFTP_WRQ          02                                //写请求
#define TFTP_DATA         03                                //数据包
#define TFTP_ACK          04                                //ACK包
#define TFTP_ERROR        05                                //错误码
#define TFTP_OACK         06                                //带选项的OACK包



extern char *downfile;                                      //文件名
extern char *upfile; 
extern char *trmode;                                        //文件传输格式
extern  char *option;                                       //请求选项，文件大小
extern char filesize[10];
extern UINT16 datasize ;
extern const UINT8 DESIP[];

extern UINT8 desip[4];  
extern UINT8 ReqBuffer[50];                                 //用于保存请求包
extern UINT8 ACKBuffer[20];                                 //用于保存ACK包
extern UINT8 DATABuffer[600];                               //保存发送的数据包
extern UINT8 errorcode;                                     //错误码
extern UINT8  oack_flag;                                    //接收到OACK标志位；
extern UINT8  opcode;
extern UINT16 block_num;
extern UINT16 sendport;
extern UINT8 Create_Request_Package(UINT8 opcode,const char *filename);

extern UINT8 Create_Ack_Package(UINT16 block_num);

extern UINT16 Create_Data_Package(UINT16 block_num,UINT16 datasize,char *sendbuf);

extern void Parse_Recv_Package(UINT8 *buf);


#endif
