/********************************** (C) COPYRIGHT ******************************
* File Name          : CH563SMTP.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/05/15
* Description        : CH563芯片用于发送邮件
*                    : MDK3.36@ARM966E-S,Thumb,小端
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CH563BAS.H"
#include "CH563NET.H"
#include "SMTP.H"
#include "SYSFREQ.H"

const char base64_map[64] = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
/* SMTP CMD Codes */
const  char  SMTP_CLIENT_CMD[6][11] = 
{
    "EHLO",                                                                     /* 0 退出 */
    "AUTH LOGIN",                                                               /* 1 登陆 */
    "MAIL FROM:",                                                               /* 2 发件人地址 */
    "RCPT TO:",                                                                 /* 3 收件人地址 */
    "DATA",                                                                     /* 4 开始发送数据命令 */
    "QUIT"                                                                      /* 5 退出 */
};
/******************************************************************************/
SMTP    m_smtp;
SMTP    *p_smtp;
char     send_buff[512];                                                        /* 发送数据缓存区 */
char     EncodeHostName[32];                                                    /* 保存编码后发件人名字 */
char     MailBodyData[128]= "Demonstration test ch563 mail function wch.cn";    /* 邮件正文内容，用于演示（回复时用于暂存解码的正文内容）*/
char     AttachmentData[attach_max_len]= "0123456789abcdefghijklmnopqrstuvwxyz";/* 附件内容，用于演示（回复时用于暂存解码后的附件内容）*/

/*******************************************************************************
* Function Name  : Base64Encode
* Description    : base64编码
* Input          : src     -需要编码的字符串
                   src_len -需要编码字符串的长度
                   dst     -编码后的字符串
* Output         : None
* Return         : None
*******************************************************************************/
void Base64Encode(char *src, UINT16 src_len, char *dst)
{
        UINT16 i = 0,j = 0;
        
        for (; i < src_len - src_len % 3; i += 3) {
                dst[j++] = base64_map[(src[i] >> 2) & 0x3f];
                dst[j++] = base64_map[((src[i] << 4) | (src[i + 1] >> 4)) & 0x3f];
                dst[j++] = base64_map[((src[i + 1] << 2) | (src[i + 2] >> 6 )) & 0x3f];
                dst[j++] = base64_map[src[i + 2] & 0x3f];
        }
        
        if (src_len % 3 == 1) {
                 dst[j++] = base64_map[(src[i] >> 2) & 0x3f];
                 dst[j++] = base64_map[(src[i] << 4) & 0x3f];
                 dst[j++] = '=';
                 dst[j++] = '=';
        }
        else if (src_len % 3 == 2) {
                dst[j++] = base64_map[(src[i] >> 2) & 0x3f];
                dst[j++] = base64_map[((src[i] << 4) | (src[i + 1] >> 4)) & 0x3f];
                dst[j++] = base64_map[(src[i + 1] << 2) & 0x3f];
                dst[j++] = '=';
        }
        dst[j] = '\0';
}

/*******************************************************************************
* Function Name  : QuotedPrintableEncode
* Description    : quotedprintable编码
* Input          : pSrc    -需要编码的字符串
                   pDst    -编码后的字符串
                   nSrcLen -需要编码字符串的长度
                   MaxLine -最大行数
* Output         : None
* Return         : None
*******************************************************************************/
void QuotedPrintableEncode( char *pSrc, char *pDst, UINT16 nSrcLen, UINT16 MaxLine )
{
    UINT16 nDstLen  = 0;
    UINT16 nLineLen = 0;
    UINT16 i = 0;

    for(i = 0; i < nSrcLen; i++, pSrc++ ){        
        if( (*pSrc >= '!') && (*pSrc <= '~') && (*pSrc != '=') ){
            *pDst++ = *pSrc;
            nDstLen++;
            nLineLen++;
        }
        else{
            sprintf(pDst, "=%02x", *pSrc);
            pDst += 3; 
            nDstLen += 3;
            nLineLen += 3;
        }
        if( nLineLen >= MaxLine - 3 ){    
            sprintf(pDst,"=\r\n");
            pDst += 3;
            nDstLen += 3;
            nLineLen = 0;
        }
    }
    *pDst = '\0';
}

/*******************************************************************************
* Function Name  : CH563NET_XToChar
* Description    : 16进制转字符串
* Input          : dat -要转换的十六进制数据
                   p   -转换后对应的字符串
                   len -要转换的长度
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_XToChar( char  *dat,char  *p,char len)
{
    char k;
    for(k=0;k<len;k++){
        *p = (((dat[k]&0xf0)>>4)/10)?(((dat[k]&0xf0)>>4)+'A'-10):(((dat[k]&0xf0)>>4)+'0');
        p++;
        *p = ((dat[k]&0x0f)/10)?((dat[k]&0x0f)+'A'-10):((dat[k]&0x0f)+'0');
        p++;
        if(k<len-1){
            *p = '.';
            p++;        
        }
    }
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPInit
* Description    : 发送邮件初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPInit( void )
{
    p_smtp = &m_smtp;
    p_smtp->g_MIME = 0;
    memset( p_smtp, '\0', sizeof(SMTP) );
    strcpy( p_smtp->m_strSMTPServer, m_Server );                                /* 服务器名称 */
    strcpy( p_smtp->m_strUSERID,m_UserName );                                   /* 用户名 */
    strcpy( p_smtp->m_strPASSWD,m_PassWord );                                   /* 密码 */        
    strcpy( p_smtp->m_strSendFrom,m_SendFrom );                                 /* 发件人地址 */
    strcpy( p_smtp->m_strSenderName, m_SendName );                              /* 发送人名字 */
    strcpy( p_smtp->m_strSendTo,m_SendTo );                                     /* 收件人地址 */
    strcpy( p_smtp->m_strSubject,m_Subject );                                   /* 主题    */
    strcpy( p_smtp->m_strFile,m_FileName );                                     /* 附件名字(如果不发送附件，此处不需初始化) */
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPIsMIME
* Description    : 判断有无附件
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPIsMIME( void )
{
    if( strlen(p_smtp->m_strFile) <= 0 ) p_smtp->g_MIME = 0;
    else p_smtp->g_MIME = 1;
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPAttachHeader
* Description    : 用于组建附件信封 
* Input          : pFileName     -附件名字
                   pAttachHeader -组建好的信封内容
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPAttachHeader(  char *pFileName, char *pAttachHeader )
{
    const char *strContentType1 = "application/octet-stream";
    sprintf(pAttachHeader, "\r\n\r\n--%s\r\nContent-Type: %s;\r\n name=\"%s\"%sContent-Disposition: \
    attachment;\r\n filename=\"%s\"\r\n\r\n", g_strBoundary, strContentType1, pFileName,g_AttachHead, pFileName ); 
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPAttachEnd
* Description    : 组建附件结束内容 
* Input          : EndSize    -附件结束内容大小
*                   pAttachEnd -附件结束内容
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPAttachEnd( UINT16 *EndSize, char *pAttachEnd )
{
    strcat( pAttachEnd, "\r\n--" );
    strcat( pAttachEnd, g_strBoundary );
    strcat( pAttachEnd, "--" );
    *EndSize = strlen(pAttachEnd);
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPMailHeader
* Description    : 邮件信封  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPMailHeader( void )
{
    // "FROM: "
    memset( send_buff, '\0', sizeof(send_buff) );
    strcat( send_buff, "From: \"" );
    strcat( send_buff, p_smtp->m_strSenderName );
    strcat( send_buff, "\" <" );
    strcat( send_buff, p_smtp->m_strSendFrom );
    strcat( send_buff, ">\r\n" );
    // "TO: " 
    strcat( send_buff, "To: <" );
    strcat( send_buff, p_smtp->m_strSendTo );
    strcat( send_buff, ">\r\n" );
    // "Subject: " 
    strcat( send_buff, "Subject: ");
    strcat( send_buff, p_smtp->m_strSubject );
    strcat( send_buff, "\r\n" );
    //"Date: " 
    strcat( send_buff, "Date: ");
//    strcat( send_buff, "" );     /* 时间    */
     strcat( send_buff, "\r\n" );
    /* "X-Mailer: " */
    strcat( send_buff, g_xMailer );
    /* 有附件 */
    if( p_smtp->g_MIME == 1 ){
        strcat( send_buff, "MIME-Version: 1.0\r\nContent-Type: " );
        strcat( send_buff, g_MailHedType );
        strcat( send_buff, ";\r\n\tboundary=\"" );
        strcat( send_buff, g_strBoundary );
        strcat( send_buff, "\"\r\n" );
    }
    /* Encoding information    */
    strcat( send_buff, g_Encoding );
    strcat( send_buff, p_smtp->m_strSenderName );
    strcat( send_buff, " <" );
    strcat( send_buff, p_smtp->m_strSendFrom );
    strcat( send_buff, ">\r\n" );
    /* add custom-tailor */
    strcat( send_buff, g_Custom );
    /* end of mail header */
    strcat( send_buff, "\r\n\r\n" );
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);    
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPSendAttachData
* Description    : 发送附件内容  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPSendAttachData( void )
{
    UINT16  EndSize;

    memset( send_buff, '\0', sizeof(send_buff) );
    CH563NET_SMTPAttachHeader( p_smtp->m_strFile, send_buff);
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);   /* Send attached file header */
/*******************************************************************************
*发送附件内容 
*****************************************************************************/
    memset( send_buff, '\0', sizeof(send_buff) );
    QuotedPrintableEncode( AttachmentData, send_buff, strlen(AttachmentData),200 );
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);
    memset( send_buff, '\0', sizeof(send_buff) );
    CH563NET_SMTPAttachEnd( &EndSize, send_buff );
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);   /* Send attached file end */
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPSendAttachHeader
* Description    : 发送附件信封  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPSendAttachHeader( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, g_FormatMail );
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck,p_smtp->Socket );
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "\r\n--%s\r\nContent-Type: %s;\r\n\tcharset=\"%s\"%s\r\n",\
        g_strBoundary, g_AttachHedType, g_strcharset,g_AttachHead );
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck,p_smtp->Socket);
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPEhlo
* Description    : 进入发送邮件状态  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPEhlo( void )
{
    memset( EncodeHostName, '\0', sizeof(EncodeHostName) );
    QuotedPrintableEncode( (char *)p_smtp->m_strSenderName, EncodeHostName, strlen(p_smtp->m_strSenderName),76 );
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %s\r\n", SMTP_CLIENT_CMD[0],EncodeHostName );
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_HELO ,p_smtp->Socket);
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPAuth
* Description    : 进入发送邮件状态  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPAuth( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n",SMTP_CLIENT_CMD[1]);
    /* send "AUTH LOGIN" command */
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_AUTH ,p_smtp->Socket);
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPUser
* Description    : 认证用户名   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPUser( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    Base64Encode( (char *)p_smtp->m_strUSERID, strlen(p_smtp->m_strUSERID), send_buff );
    sprintf( send_buff, "%s\r\n", send_buff);
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_USER,p_smtp->Socket );
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPPass
* Description    : 登陆密码    
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPPass( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    Base64Encode( (char *)p_smtp->m_strPASSWD, strlen(p_smtp->m_strPASSWD), send_buff);
    sprintf( send_buff, "%s\r\n", send_buff);
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_PASS,p_smtp->Socket );
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPMail
* Description    : 发送发件人名字  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPMail( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s <%s>\r\n", SMTP_CLIENT_CMD[2],p_smtp->m_strSendFrom );
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_MAIL,p_smtp->Socket );
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPRcpt
* Description    : 收件人地址 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPRcpt( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s <%s>\r\n", SMTP_CLIENT_CMD[3],p_smtp->m_strSendTo );    
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_RCPT,p_smtp->Socket );
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPData
* Description    : 发送data命令 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPData( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n",SMTP_CLIENT_CMD[4] );
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_DATA,p_smtp->Socket );
}

/*******************************************************************************
* Function Name  : CH563NET_SMTPSendMail
* Description    : 发送邮件内容  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_SMTPSendMail( void )
{
    CH563NET_SMTPIsMIME( );
    CH563NET_SMTPMailHeader(  );                                                /* Send Mail Header */
    if( p_smtp->g_MIME ==  1 ){
        CH563NET_SMTPSendAttachHeader(  );                                      /* Send MIME Header */
    }
    else {
        CH563NET_SendData("\r\n", strlen("\r\n"),uncheck,p_smtp->Socket);
    }
    memset( send_buff, '\0', sizeof(send_buff) );
     QuotedPrintableEncode( (char *)MailBodyData, send_buff, strlen(MailBodyData),76 );
    CH563NET_SendData( send_buff, strlen(send_buff),uncheck ,p_smtp->Socket);
    if( 1 == p_smtp->g_MIME ) CH563NET_SMTPSendAttachData( );                   /* Send Attached file */
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "\r\n.\r\n" );
    /* Send end flag of Mail */
    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_DATA_END ,p_smtp->Socket);
}

/*******************************************************************************
* Function Name  : CH563NET_Quit
* Description    : 退出登陆
* Input          : index-需退出的socketid
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_Quit( UINT8 index )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n", SMTP_CLIENT_CMD[5]);
    if(index==p_smtp->Socket)    CH563NET_SendData( send_buff, strlen(send_buff),SMTP_CHECK_QUIT,index );
}

/*******************************************************************************
* Function Name  : CH563NET_MailCmd
* Description    : 判断命令进入对应子程序
* Input          : choiceorder-命令类型
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_MailCmd( UINT8 choiceorder )
{
    UINT8 i;

    switch( choiceorder ){
        case SMTP_SEND_HELO: 
            CH563NET_SMTPEhlo( );
            break;
        case SMTP_SEND_AUTH: 
            CH563NET_SMTPAuth( );
            break;
        case SMTP_SEND_USER: 
            CH563NET_SMTPUser( );
            break;
        case SMTP_SEND_PASS: 
            CH563NET_SMTPPass( );
            break;
        case SMTP_SEND_MAIL: 
            CH563NET_SMTPMail( );
            break;
        case SMTP_SEND_RCPT: 
            CH563NET_SMTPRcpt( );
            break;
        case SMTP_SEND_DATA: 
            CH563NET_SMTPData( );
            break;
        case SMTP_DATA_OVER:
            CH563NET_SMTPSendMail( );
            break;
        case SMTP_ERR_CHECK:
            CH563NET_Quit( p_smtp->Socket );
            CheckType = uncheck;
            i = CH563NET_SocketClose( p_smtp->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
            break;
        case SMTP_SEND_QUIT:
            CH563NET_Quit( p_smtp->Socket );
            break;
        case SMTP_CLOSE_SOCKET:
            CheckType = uncheck;
            i = CH563NET_SocketClose( p_smtp->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
            break;
        default: 
            break;
    }
}

/*******************************************************************************
* Function Name  : CH563NET_CheckResponse
* Description    : 验证握手信号
* Input          : recv_buff- 握手信息
                   check_type-检测的类型
* Output         : None
* Return         : 函数执行状态
*******************************************************************************/
UINT8 CH563NET_CheckResponse( char *recv_buff,UINT8 checktype )
{
    switch(checktype){
        case SMTP_CHECK_CNNT:    // 
            if( strncmp("220", recv_buff, 3) == 0 ){
                OrderType = SMTP_SEND_HELO;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_CNNT;
        case SMTP_CHECK_HELO:    // 
            if( strncmp("250", recv_buff, 3) == 0 ){
                OrderType = SMTP_SEND_AUTH;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_HELO;
        case SMTP_CHECK_AUTH:                                                   /* 登陆命令 */
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = NOOP;
                return (CHECK_SUCCESS);
            }
            if(strncmp("334", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_USER;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_AUTH;
        case SMTP_CHECK_USER:                                                   /* 用户名 */
            if(strncmp("334", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_PASS;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_USER;
        case SMTP_CHECK_PASS:                                                   /* 密码 */
            if(strncmp("235", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_MAIL;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_PASS;
        case SMTP_CHECK_MAIL:                                                   /* 发送者 */
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_RCPT;
                return (CHECK_SUCCESS);    
            } 
            return SMTP_ERR_MAIL;
        case SMTP_CHECK_RCPT:                                                   /* 接收 */
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_DATA;
                return (CHECK_SUCCESS);            
            }
            return SMTP_ERR_RCPT;
        case SMTP_CHECK_DATA:                                                   /* data 命令 */
            if(strncmp("354", recv_buff, 3) == 0){
                OrderType = SMTP_DATA_OVER;
                return (CHECK_SUCCESS);
            } 
            return SMTP_ERR_DATA;
        case SMTP_CHECK_DATA_END:                                               /* 数据发送完成 */
            if(strncmp("250", recv_buff, 3) == 0){
                OrderType = SMTP_SEND_QUIT;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_DATA_END;
        case SMTP_CHECK_QUIT:                                                   /* 退出登陆 */
            if(strncmp("220", recv_buff, 3) == 0||strncmp("221", recv_buff, 3) == 0){
                OrderType = SMTP_CLOSE_SOCKET;
                return (CHECK_SUCCESS);
            }
            return SMTP_ERR_QUIT;
        default:
            return SMTP_ERR_UNKNOW;
    }
}

/*********************************** endfile **********************************/
