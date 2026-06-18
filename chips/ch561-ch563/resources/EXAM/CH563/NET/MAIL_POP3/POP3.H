/********************************** (C) COPYRIGHT *********************************
* File Name          : POP3.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : Define for ch563pop3.c
**********************************************************************************/
#ifndef __POP3_H__
#define __POP3_H__
//================================================================================
extern  UINT8 OrderType;                       // 命令类型
extern  UINT8 CheckType;      
extern  UINT8 ReceDatFlag;                     // 收到数据标志位
extern  UINT16 ReceLen;    
//=================================================================================
#define attach_max_len              512        // 发送附件最大长度，默认为512   
#define POP3_SERVER_PORT            110        // 收邮件目的端点号
#define SMTP_SERVER_PORT            25         // 发邮件目的端点号
#define DEBUG                        1         // 调试    -- 波特率115200
typedef struct
{
    UINT8 Socket;               
    UINT8 EncodeType;                          // 邮件使用的编码方式
    UINT8 AnalyMailDat;                        // 分析邮件结束标志位
    UINT8 identitycheck;                       // 验证附件内容标志位
    UINT8 RefreshTime;                         // 没搜查到邮件继续搜索标志位
    UINT8 DiscnntFlag;
    UINT8 ReceFlag;
    char  sBufTime[40];                        // 保存接收邮件的时间
    char  Ccname[48];                          // 抄送的名字
    char  DecodeRName[32];                     // 保存接收到邮件解码后的发件人名字
    char  pPop3Server[32];                     // POP服务器
    char  pPop3UserName[32];                   // POP登陆用户名
    char  pPop3PassWd[32];                     // POP登陆密码
}POP;
/* pop接收邮件相关 */
extern const char *p_Server;                   // POP服务器
extern const char *p_UserName;                 // POP登陆用户名
extern const char *p_PassWord;                 // POP登陆密码
/* CH563相关定义 */                                                                   
extern UINT8 MACAddr[6];                       // CH563MAC地址 
extern const UINT8 IPAddr[4];                  // CH563IP地址 
extern const UINT8 GWIPAddr[4];                // CH563网关 
extern const UINT8 IPMask[4];                  // CH563子网掩码 
/* socketpop3 相关定义, POP收邮件*/                                              
extern const UINT8  Socketpop3IP[4];           // Socket pop3目的IP地址
extern const UINT16  Pop3SourPrt;              // Socket pop3源端口号

/************************************************************************************************************************
* POP可选命令
************************************************************************************************************************/
#define POP_RTER        1                      // 1 处理server邮件的全部文本
#define POP_DELE        1                      // 2 标记删除
//    #define POP_RSET    1                    // 3 处理撤销所有的DELE命令
//    #define POP_TOP        1                 // 4 返回n号邮件的前m行内容
//    #define POP_UIDL    1                    // 5 处理server返回用于该指定邮件的唯一标识，如果没有指定，返回所有的。
#define POP_REFRESH        1                   // 接收邮件过程中没收到邮件继续查询标志 
/************************************************************************************************************************
* 命令代码 
* 0x01-0x0f 为接收邮件命令码
****************** 0x01-0x07 为常用命令
****************** 0x09-0x0e 为P可选命令
****************** 0x01-0x02-0x03-0x06 为确认状态
****************** 0x04-0x05 0x09-0x0e 为操作状态
* 0x10-0x1f 为发送邮件命令码
****************** 0x10-0x17 需按顺序执行
****************** 后面的为可选择操作
* 0x00         不做任何操作
************************************************************************************************************************/
#define NOOP                    0x00
//POP RECEIVE CODE
#define POP_RECEIVE_USER        0x01    // 认证用户
#define POP_RECEIVE_PASS        0x02    // 认证密码
#define POP_RECEIVE_STAT        0x03    // 邮箱统计资料
#define POP_RECEIVE_LIST        0x04    // 返回指定邮件的大小
#define POP_RECEIVE_RTER        0x05    // 读取邮件的全部文本
#define POP_RECEIVE_QUIT        0x06    // 退出登陆
#define POP_CLOSE_SOCKET        0x07    // 关闭pop的socket
#define POP_RECEIVE_START       0x08    // 启动接收邮件
#define POP_RECEIVE_DELE        0x09    // 标记删除
#define POP_RECEIVE_RSET        0x0a    // 撤销所有的DELE命令
#define POP_RECEIVE_TOP         0x0b    // 处理返回n号邮件的前m行内容，m必须是自然数
#define POP_RECEIVE_UIDL        0x0e    // 返回用于该指定邮件的唯一标识
#define POP_ERR_CHECK           0x0F    // POP握手出错退出登陆并关闭socket
/* *********************************************************************************************************************
* 握手信号核对错误代码
************************************************************************************************************************/
//POP ERR CHECK CODE
#define POP_ERR_CNNT            0x20     // pop连接错误--------------可能是端口号，服务器IP等设置错误
#define POP_ERR_USER            0x21     // 认证用户名检测错误-------用户名格式不对，或USER命令码错误（命令码错误建议查看发送的命令码核对是否正确，后同）
#define POP_ERR_PASS            0x22     // 登陆失败-----------------用户名或密码错误，或PASS命令码错误
#define POP_ERR_STAT            0x23     // 获取邮件信息错误---------STAT命令码错误，如出现失败重新登陆，如还是失败检查其他问题
#define POP_ERR_LIST            0x24     // 获取邮件信息列表---------LIST命令码错误
#define POP_ERR_RETR            0x25     // 读取指定邮件号信息错误---读取的邮件不存在，或RETR命令码错误，或输入的参数不正确
#define POP_ERR_DELE            0x26     // 删除某封邮件失败---------删除的邮件不存在，或RETR命令码错误，或输入的参数不正确
#define POP_ERR_QUIT            0x59     // 退出登陆失败-------------QUIT命令码错误
#define POP_ERR_RSET            0x28     // 恢复删除的邮件-----------RSET命令码错误（命令需在退出之前执行才能有效）
#define POP_ERR_TOP             0x2B     // 读取某封邮件前m行失败----读取的邮件不存在，或RETR命令码错误，或输入的参数不正确
#define POP_ERR_UIDL            0x2C     // 获取某封邮件ID失败-------邮件不存在，或UIDL命令码错误，或输入的参数不正确
#define POP_ERR_UNKW            0x2F     // pop错误未知--------------错误未知
//
#define send_data_timeout       0x10     // 发送数据超时
#define send_data_success       0x14     // 发送数据超时

/***********************************************************************************************************************
* 常用命令代码 
***********************************************************************************************************************/
//POP3  CODE
#define OPEN_SOCKET_RECEIVE     0xA0     // 打开接收socket
#define LOGIN_MAILBOX_RECEIVE   0xA1     // 登陆接收邮箱
#define READ_MAIL_STAT          0xA2     // 读取邮件总列表
#define READ_MAIL_LIST          0xA3     // 读取邮件列表
#define READ_MAIL_MAILBODYDATA  0xA4     // 读取邮件正文内容
#define READ_MAIL_ATTACHMENT    0xA5     // 读取邮件附件内容
#define READ_MAIL_ADDRESS       0xA6     // 读取发件人地址
#define READ_MAIL_TIME          0xA7     // 读取发送时间
#define READ_MAIL_SUBJECT       0xA8     // 读取邮件主题
#define QUIT_RECEIVE_MAIL       0xA9     // 退出登陆并关闭socket
//SMTP  CODE
#define OPEN_SOCKET_SEND        0xB0     // 打开发送socket
#define LOGIN_MAILBOX_SEND      0xB1     // 登陆发送邮箱
#define SEND_MAIL_UNATTACH      0xB2     // 发送邮件不包含附件
#define SEND_MAIL_CONTATTACH    0xB3     // 发送邮件包含附件
#define QUIT_SEND_MAIL          0xB4     // 退出登陆并关闭socket
/***********************************************************************************************************************
* 核对握手信息的错误类型 
* 成功返回CHECK_SUCCESS。并命令操作码置为下一条需操作的命令
* 错误返回对应的错误代码 
* 部分命令无握手信号，则不需要核对
***********************************************************************************************************************/
#define CHECK_SUCCESS              0
#define uncheck                    0
#define CHECK_ERROR                1
//POP CHECK CODE
// 检查应答信息，正确返回“+0k” ，错误返回“-ERR”
#define POP_CHECK_CNNT            0x50    
#define POP_CHECK_USER            0x51    
#define POP_CHECK_PASS            0x52    
#define POP_CHECK_STAT            0x53
#define POP_CHECK_LIST            0x54    
#define POP_CHECK_RETR            0x55
#define POP_CHECK_DELE            0x56
#define POP_CHECK_SEAR            0x57
#define POP_CHECK_QUIT            0x58
#define POP_CHECK_RSET            0x59
#define POP_CHECK_APOP            0x5A
#define POP_CHECK_TOP             0x5B
#define POP_CHECK_UIDL            0x5C
//==============================================================================
void CH563NET_SendData( char *PSend, UINT32 Len,UINT8 type,UINT8 index  );      // 发送数据

void CH563NET_MAILInit(void);                                                   // 563初始化

void CH563NET_POP3Init( void );                                                 // 收邮件初始化

void CH563NET_MailCmd( UINT8 choiceorder);                                      // 根据命令类型执行对应的命令，自动完成整个收发过程

void CH563NET_POP3Retr( UINT8 num );                                            // 读取某封邮件

void CH563NET_POP3Dele( UINT8 num );                                            // 删除某封邮件

void CH563NET_POP3Rset( void );                                                 // 撤销所有的删除命令

void CH563NET_POP3Top( UINT8 num ,UINT8 m  );                                   // 读取某封邮件前m行

void CH563NET_POP3Uidl( UINT8 num );                                            // 获取某封邮件的ID

UINT8 CH563NET_CheckResponse( char *recv_buff,UINT8 check_type );               // 检查应答信息

void CH563NET_Quit(UINT8 index );                                               // 退出邮箱登陆

void CH563NET_CreatTcpPop3( void );                                             // 创建pop3连接

void CH563NET_ReplyMailBody( void );                                            // 回复邮件的正文内容

void CH563NET_MailQuery( void );

void CH563NET_AnalyseMailData( char *recv_buff );

void mStopIfError(UINT8 iError);

void CH563NET_HandleGloableInt( void );

//================================================================================
#endif
