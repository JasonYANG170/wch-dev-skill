/********************************** (C) COPYRIGHT *********************************
* File Name          : SMTP.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : Define for CH563SMTP.C
**********************************************************************************/
#ifndef __SMTP_H__
#define __SMTP_H__
//================================================================================
extern  UINT8 OrderType;                          // 命令类型
extern  UINT8 CheckType;        
extern  UINT8 ReceDatFlag;                        // 收到数据标志位
extern  UINT16 ReceLen;    
//=================================================================================
#define attach_max_len              512           // 发送附件最大长度，默认为512   
#define POP3_SERVER_PORT            110           // 收邮件目的端点号
#define SMTP_SERVER_PORT            25            // 发邮件目的端点号
#define DEBUG                        1            // 调试    -- 波特率115200
typedef struct
{
    UINT8 Socket;                
    UINT8 g_MIME;                                 // 有无附件标志位
    char  m_strFile[48];                          // 附件名字
    char  m_strSendFrom[48];                      // 发件人地址
    char  m_strSendTo[48];                        // 收件人地址
    char  m_strSMTPServer[32];                    // 服务器名称
    char  m_strUSERID[32];                        // 用户名
    char  m_strPASSWD[32];                        // 密码
    char  m_strSubject[32];                       // 主题
    char  m_strSenderName[32];                    // 发送人名字
}SMTP;
/* smtp发送邮件相关参数 */
extern const char *m_Server;                      // 服务器名称
extern const char *m_UserName;                    // 用户名
extern const char *m_PassWord;                    // 密码
extern const char *m_SendFrom;                    // 发件人地址
extern const char *m_SendName;                    // 发送人名字
extern const char *m_SendTo;                      // 收件人地址
extern const char *m_Subject;                     // 主题
extern const char *m_FileName;                    // 附件名字(如果不发送附件,则置为"\0")
/* CH563相关定义 */                                                                   
extern UINT8 MACAddr[6];                          // CH563MAC地址 
extern const UINT8 IPAddr[4];                     // CH563IP地址 
extern const UINT8 GWIPAddr[4];                   // CH563网关 
extern const UINT8 IPMask[4];                     // CH563子网掩码 
/* socketsmtp 相关定义,SMTP发送邮件    */                                          
extern const UINT8  SocketsmtpIP[4];              // Socket smtp目的IP地址 
extern const UINT16  SmtpSourPrt;                 // Socket smtp源端口号
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
//SMTP SEND CODE
#define SMTP_SEND_HELO            0x10    // 发送hello命令
#define SMTP_SEND_AUTH            0x11    // 发送登陆命令
#define SMTP_SEND_USER            0x12    // 发送用户名
#define SMTP_SEND_PASS            0x13    // 发送密码
#define SMTP_SEND_MAIL            0x14    // 发送发送者地址
#define SMTP_SEND_RCPT            0x15    // 发送接收者地址
#define SMTP_SEND_DATA            0x16    // 发送发送数据命令
#define SMTP_DATA_OVER            0x17    // 发送邮件内容
#define SMTP_SEND_QUIT            0x18    // 退出登陆
#define SMTP_CLOSE_SOCKET         0x19    // 关闭smtp的socket
#define SMTP_SEND_START           0x1E    // 启动发送邮件
#define SMTP_ERR_CHECK            0x1F    // smtp握手出错退出登陆并关闭socket
/* *********************************************************************************************************************
* 握手信号核对错误代码
************************************************************************************************************************/
//SMTP ERR CHECK CODE
#define SMTP_ERR_CNNT            0x31     // smtp连接错误-------------可能是端口号，服务器IP等设置错误
#define SMTP_ERR_HELO            0x32     // 发送helo命令失败---------HELO命令码错误，或者主机名格式错误
#define SMTP_ERR_AUTH            0x33     // 请求登陆命令失败---------AUTH命令码错误
#define SMTP_ERR_USER            0x34     // 发送用户名未识别---------用户名格式不对，或USER命令码错误
#define SMTP_ERR_PASS            0x35     // 登陆失败-----------------用户名或密码错误，或PASS命令码错误
#define SMTP_ERR_MAIL            0x36     // 发件人地址发送失败-------发件人地址不正确，或MAIL命令码错误
#define SMTP_ERR_RCPT            0x37     // 发送接收地址出错---------收件人地址不正确，或RCPT命令码错误
#define SMTP_ERR_DATA            0x38     // 请求发送数据-------------DATA命令码错误
#define SMTP_ERR_DATA_END        0x39     // 发送数据过程出错
#define SMTP_ERR_QUIT            0x3A     //    退出登陆失败
#define SMTP_ERR_UNKNOW          0x3F     // smtp错误未知
//
#define send_data_timeout        0x10     // 发送数据超时
#define send_data_success        0x14     // 发送数据超时
/***********************************************************************************************************************
* 核对握手信息的错误类型 
* 成功返回CHECK_SUCCESS。并命令操作码置为下一条需操作的命令
* 错误返回对应的错误代码 
* 部分命令无握手信号，则不需要核对
***********************************************************************************************************************/
#define CHECK_SUCCESS            0x00
#define uncheck                    0x00
//SMTP CHECK CODE
#define SMTP_CHECK_CNNT            0x60
#define SMTP_CHECK_HELO            0x61
#define SMTP_CHECK_AUTH            0x62
#define SMTP_CHECK_USER            0x63
#define SMTP_CHECK_PASS            0x64
#define SMTP_CHECK_MAIL            0x65
#define SMTP_CHECK_RCPT            0x66
#define SMTP_CHECK_DATA            0x67
#define SMTP_CHECK_DATA_END        0x68
#define SMTP_CHECK_QUIT            0x69

#define g_strBoundary    "18ac0781-9ae4-4a2a-b5f7-5479635efb6b"                      /* 边界 */
#define g_strEncode      "base64"                                                    /* 编码方式 */
#define g_strcharset     "gb2312"                                                    /* windows格式 (linux格式"utf-8") */
#define g_xMailer        "X-Mailer: X-WCH-Mail Client Sender\r\n"                    /* X-Mailer内容     */
#define g_Encoding       "Content-Transfer-Encoding: quoted-printable\r\nReply-To: " /* Encoding 内容 */
#define g_Custom         "X-Program: CSMTPMessageTester"                             /* X-Program内同，可修改 */
#define g_FormatMail     "This is a multi-part message in MIME format."              /* 邮件有多种内容，一般是有附件 */
#define g_AttachHead     "\r\nContent-Transfer-Encoding: quoted-printable\r\n" 
#define g_AttachHedType  "text/plain"
#define g_MailHedType    "multipart/mixed"

//================================================================================
void CH563NET_SendData( char *PSend, UINT32 Len,UINT8 type,UINT8 index  );        // 发送数据

void CH563NET_MAILInit(void);                                                     // 563初始化

void CH563NET_SMTPInit( void );                                                   // 发邮件初始化

void CH563NET_MailCmd( UINT8 choiceorder);                                        // 根据命令类型执行对应的命令，自动完成整个收发过程

UINT8 CH563NET_CheckResponse( char *recv_buff,UINT8 check_type );                 // 检查应答信息

void CH563NET_Quit(UINT8 index );                                                 // 退出邮箱登陆

void CH563NET_CreatTcpSmtp( void );                                               // 创建smtp连接

void CH563NET_ReplyMailBody( void );                                              // 回复邮件的正文内容

void CH563NET_MailQuery( void );

void mStopIfError(UINT8 iError);

void CH563NET_HandleGloableInt( void );

//================================================================================
#endif
