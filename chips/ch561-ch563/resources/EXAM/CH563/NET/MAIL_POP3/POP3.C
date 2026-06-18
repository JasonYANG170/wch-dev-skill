/********************************** (C) COPYRIGHT ******************************
* File Name          : CH563POP3.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563芯片用于接收邮件
*                    : MDK3.36@ARM966E-S,Thumb,小端
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CH563BAS.H"
#include "CH563NET.H"
#include "POP3.H"
#include "SYSFREQ.H"

const char *base64_map = 
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnoprstuvwxyz0123456789+/";
const char base64_decode_map[256] = 
{
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255,  62, 255, 255, 255,  63,  52,  53,  54,  55,  56,  57,  58,  59,  60,  61, 255, 255,
     255,   0, 255, 255, 255,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14,
      15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25, 255, 255, 255, 255, 255, 255,  26,  27,  28,
      29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,  48,
      49,  50,  51, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,
     255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255
};
/* POP3 CMD Codes */
const char  POP3_CLIENT_CMD[12][5] = 
{
    /* basic POP3 commands */
    "QUIT",                                                                     /* 0 退出 */
    "USER",                                                                     /* 1 用户名 */
    "PASS",                                                                     /* 2 密码 */
    "STAT",                                                                     /* 3 邮箱统计资料 */
    "LIST",                                                                     /* 4 返回指定邮件的大小 */
    "RETR",                                                                     /* 5 邮件的全部文本 */
    "DELE",                                                                     /* 6 标记删除 */
    "RSET",                                                                     /* 7 撤销所有的DELE命令 */
    "NOOP",                                                                     /* 8 返回一个肯定的响应 */
    /* alternative POP3 commands */
    "APOP",                                                                     /* 9  认证一种安全传输口令的办法，执行成功导致状态转换 */
    "TOP" ,                                                                     /* 10 处理返回n号邮件的前m行内容，m必须是自然数 */
    "UIDL"                                                                      /* 11 返回用于该指定邮件的唯一标识 */
};
/******************************************************************************/
POP   m_pop3;
POP   *p_pop3;
char  R_argv[3][32];
char  send_buff[512];                                                           /* 发送数据缓存区 */
char  EncodeHostName[32];                                                       /* 保存编码后发件人名字 */
char  MailBodyData[128]= "Demonstration test ch563 mail function wch.cn";       /* 邮件正文内容，用于演示（回复时用于暂存解码的正文内容）*/
char  AttachmentData[attach_max_len]= "0123456789abcdefghijklmnopqrstuvwxyz";   /* 附件内容，用于演示（回复时用于暂存解码后的附件内容）*/

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
        UINT32 i = 0, j = 0;
        
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
* Function Name  : Base64Decode
* Description    : base64解码
* Input          : src     -需要解码的字符串
                   src_len -需要解码字符串的长度
                   dst     -解码后的字符串
* Output         : None
* Return         : None
*******************************************************************************/
void Base64Decode(char *src, UINT16 src_len, char *dst)
{
        UINT32 i = 0, j = 0;
        
        for (; i < src_len; i += 4) {
            if(strncmp( &src[i], "\r\n", 2 ) == 0) i += 2;
            dst[j++] = base64_decode_map[src[i]] << 2 |
                    base64_decode_map[src[i + 1]] >> 4;
            dst[j++] = base64_decode_map[src[i + 1]] << 4 |
                    base64_decode_map[src[i + 2]] >> 2;
            dst[j++] = base64_decode_map[src[i + 2]] << 6 |
                    base64_decode_map[src[i + 3]];
        }
        if(src_len%4 == 3) {
            dst[strlen(dst)-1] = '\0'; 
        }
        else if(src_len%4 == 2) {
            dst[strlen((char *)dst)-1] = '\0'; 
            dst[strlen((char *)dst)-1] = '\0'; 
        }
        else dst[j] = '\0';
}

/*******************************************************************************
* Function Name  : QuotedPrintableEncode
* Description    : quotedprintable编码
* Input          : pSrc需要编码的字符串
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
* Function Name  : QuotedPrintableEncode
* Description    : quotedprintable解码
* Input          : pSrc    -需要解码的字符串
                   nSrcLen -需要编码字符串的长度
                   pDst    -解码后的字符串
* Output         : None
* Return         : None
*******************************************************************************/
void QuotedPrintableDecode( char *pSrc, char *pDst, UINT16 nSrcLen )
{
    UINT16 i = 0;
    UINT16 nDstLen = 0;

    while( i < nSrcLen ){
        if( strncmp( pSrc, "=\r\n", 3 ) == 0 ){
            pSrc += 3;
            i += 3;
        }
        else{
            if( *pSrc == '=' ){
                sscanf( pSrc, "=%02x",*pDst);
                pDst++;
                pSrc += 3;
                i += 3;
            }
            else{
                *pDst++ = *pSrc++;
                i++;
            }
            nDstLen++;
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
* Function Name  : ch563mail_pop3init
* Description    : 接收邮件初始化
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_POP3Init( void )
{
    p_pop3 = &m_pop3;
    memset( p_pop3, '\0', sizeof(POP) );
    strcpy( p_pop3->pPop3Server,   p_Server );                                  /* 服务器名称 */
    strcpy( p_pop3->pPop3UserName, p_UserName );                                /* 用户名 */
    strcpy( p_pop3->pPop3PassWd,   p_PassWord );                                /* 密码    */
}

/*******************************************************************************
* Function Name  : CH563NET_POP3User
* Description    : 认证用户名  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_POP3User( void )                          
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf(send_buff, "%s %s\r\n",POP3_CLIENT_CMD[1], p_pop3->pPop3UserName);
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_USER ,p_pop3->Socket); 
}

/*******************************************************************************
* Function Name  : CH563NET_POP3Pass
* Description    : 密码  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_POP3Pass( void )                          
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %s\r\n", POP3_CLIENT_CMD[2], p_pop3->pPop3PassWd );
    CH563NET_SendData(  send_buff, strlen(send_buff), POP_CHECK_PASS,p_pop3->Socket );    
}

/*******************************************************************************
* Function Name  : CH563NET_POP3Stat
* Description    : 回送邮箱统计资料  
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_POP3Stat( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff,"%s\r\n", POP3_CLIENT_CMD[3] );
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_STAT,p_pop3->Socket );
}

/*******************************************************************************
* Function Name  : CH563NET_POP3List
* Description    : 处理server返回指定邮件的大小 
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_POP3List( void )
{
#if    0                                                                        /* 如果需指定某封邮件则置1 */
    char num;
    num = '1';                                                                  /* 根据需要修改邮件号 */
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c\r\n", POP3_CLIENT_CMD[4],num );
#else
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s\r\n", POP3_CLIENT_CMD[4] );
#endif
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_LIST,p_pop3->Socket);  
}

/*******************************************************************************
* Function Name  : CH563NET_POP3Retr
* Description    : 处理server邮件的全部文本 
* Input          : num-邮件号
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef    POP_RTER 
void CH563NET_POP3Retr( UINT8 num )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c\r\n", POP3_CLIENT_CMD[5], num );
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_RETR,p_pop3->Socket );
}
#endif

/*******************************************************************************
* Function Name  : CH563NET_POP3Dele
* Description    : 处理server标记删除 
* Input          : num-邮件号
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef    POP_DELE
void CH563NET_POP3Dele( UINT8 num )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c\r\n", POP3_CLIENT_CMD[6], num );
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_DELE,p_pop3->Socket );
}
#endif

/*******************************************************************************
* Function Name  : CH563NET_POP3Rset
* Description    : 处理server撤销删除
* Input          : num-邮件号
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef    POP_RSET
void CH563NET_POP3Rset( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s \r\n", POP3_CLIENT_CMD[7]);
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_RSET,p_pop3->Socket );
}
#endif

/*******************************************************************************
* Function Name  : CH563NET_POP3Top
* Description    : 返回n号邮件的前m行内容
* Input          : num-邮件号
                   m-行数
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef    POP_TOP
void CH563NET_POP3Top( char num ,char m  )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c %c\r\n", POP3_CLIENT_CMD[10],num,m);
    CH563NET_SendData( send_buff, strlen(send_buff), POP_CHECK_TOP,p_pop3->Socket );
}
#endif

/*******************************************************************************
* Function Name  : CH563NET_POP3Uidl
* Description    : 处理server返回用于该指定邮件的唯一标识
* Input          : num-邮件号
* Output         : None
* Return         : None
*******************************************************************************/
#ifdef    POP_UIDL
void CH563NET_POP3Uidl( char num )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "%s %c\r\n", POP3_CLIENT_CMD[11], num);
    CH563NET_SendData( send_buff, sizeof(send_buff), POP_CHECK_UIDL,p_pop3->Socket );
}
#endif
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
    sprintf( send_buff, "%s\r\n", POP3_CLIENT_CMD[0]);
    if(index==p_pop3->Socket)    CH563NET_SendData( send_buff, strlen(send_buff),POP_CHECK_QUIT,index );
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
        case POP_RECEIVE_USER: 
            CH563NET_POP3User(  );
            break;
        case POP_RECEIVE_PASS: 
            CH563NET_POP3Pass(  );
            break;
        case POP_RECEIVE_STAT: 
            CH563NET_POP3Stat(  );
            break;
        case POP_RECEIVE_LIST: 
            CH563NET_POP3List(  );
            break;
        case POP_ERR_CHECK:
            CH563NET_Quit( p_pop3->Socket );
            CheckType = uncheck;
            i = CH563NET_SocketClose( p_pop3->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
            break;
        case POP_RECEIVE_QUIT:
            CH563NET_Quit( p_pop3->Socket );
            break;
        case POP_CLOSE_SOCKET:
            CheckType = uncheck;
            i = CH563NET_SocketClose( p_pop3->Socket,TCP_CLOSE_NORMAL );
            mStopIfError(i);
            break;
        default:
            ReceDatFlag = 1; 
            break;
    }
}
/*******************************************************************************
* Function Name  : CH563NET_CheckResponse
* Description    : 验证握手信号
* Input          : recv_buff- 握手信息
                   check_type-检测的类型
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH563NET_CheckResponse( char *recv_buff,UINT8 checktype )
{
    switch(checktype){
        case POP_CHECK_CNNT:                                                    /* 连接 */
            if(strncmp("+OK", recv_buff, 3) == 0) {
                OrderType = POP_RECEIVE_USER;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_CNNT;
        case POP_CHECK_USER:                                                    /* 用户名 */
            if(strncmp("+OK", recv_buff, 3) == 0) {
                OrderType = POP_RECEIVE_PASS;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_USER;
        case POP_CHECK_PASS:                                                    /* 密码 */
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_STAT;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_PASS;
        case POP_CHECK_STAT:                                                    /* 总邮件信息 */
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_LIST;
                return (CHECK_SUCCESS);    
            } 
            return POP_ERR_STAT;
        case POP_CHECK_LIST:                                                    /* 邮件列表 */
            if((strncmp("+OK", recv_buff, 3) == 0)){
                OrderType = NOOP;
                return (CHECK_SUCCESS);
            }
            if((strncmp(".", recv_buff, 1) == 0)){
                p_pop3->RefreshTime = 1;
                OrderType = POP_RECEIVE_QUIT;
                return (CHECK_SUCCESS);
            }
            if((strncmp("1", recv_buff, 1) == 0)){
                p_pop3->RefreshTime = 0;
                OrderType = POP_RECEIVE_RTER;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_LIST;
        case POP_CHECK_QUIT:                                                    /* 退出登陆 */
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_CLOSE_SOCKET;
                return (CHECK_SUCCESS);
            }
            return POP_ERR_QUIT;
        case POP_CHECK_RETR:                                                    /* 读取邮件内容 */ 
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = NOOP;    
                memset(AttachmentData,'\0',sizeof(AttachmentData));
                memset(R_argv,'\0',sizeof(R_argv));
                return (CHECK_SUCCESS);
            } 
            else if(strncmp("-ERROR", recv_buff, 6) != 0){
                if( strncmp("\r\n.\r\n", &recv_buff[ReceLen-5], 5) == 0 ){
                    p_pop3->AnalyMailDat = 1;                                   /* 接收邮件完成标志 */
                    CH563NET_AnalyseMailData( recv_buff );                      /* 分析邮件内容 */
                    OrderType = POP_RECEIVE_QUIT;    
               }
                else{
                     p_pop3->AnalyMailDat = 0;
                       CH563NET_AnalyseMailData( recv_buff );
                }
                return (CHECK_SUCCESS);
            } 
            return POP_ERR_RETR;
        case POP_CHECK_DELE:                                                    /* 删除 */
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* 执行完后不执行其他命令，如需可自行选择下一条命令 */
                return (CHECK_SUCCESS);
            }
            return POP_ERR_DELE;
        case POP_CHECK_RSET:                                                    /* 撤销删除 */ 
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* 执行完后不执行其他命令，如需可自行选择下一条命令 */
                return (CHECK_SUCCESS);
            }
            return POP_ERR_RSET;
        case POP_CHECK_TOP:        // 
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* 执行完后不执行其他命令，如需可自行选择下一条命令 */
                return (CHECK_SUCCESS);
            }
            if(strncmp("Return", recv_buff, 6) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* 执行完后不执行其他命令，如需可自行选择下一条命令 */
                return (CHECK_SUCCESS);
            } 
            return POP_ERR_TOP;
        case POP_CHECK_UIDL:    //
            if(strncmp("+OK", recv_buff, 3) == 0){
                OrderType = POP_RECEIVE_QUIT;                                   /* 执行完后不执行其他命令，如需可自行选择下一条命令 */
                return (CHECK_SUCCESS);
            }
            return POP_ERR_UIDL;
        default:
            return CHECK_ERROR;
    }
}
/*******************************************************************************
* Function Name  : CH563NET_AnalyseMailData
* Description    : 分析接收到的邮件
* Input          : recv_buff-邮件内容
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_AnalyseMailData( char *recv_buff ) 
{
     UINT16    i=0,j=0;

    if(p_pop3->ReceFlag){
        i = 0;
        if(p_pop3->EncodeType == 2) goto type2;                                 /*  跳转到附件接收 编码方式--quoted-printable */
        if(p_pop3->EncodeType == 1) goto type1;                                 /*  跳转到附件接收 编码方式--base64 */
        if(p_pop3->EncodeType == 0) goto type0;                                 /*  跳转到附件接收 编码方式--others    */
    }
    for(i=0;i<ReceLen;i++){
        /* 发件人名字/地址 */                 
        if( strncmp("\nFrom: ", &recv_buff[i], 7) == 0 ){
            i += 7;
            if(recv_buff[i] == '"') i++;
            if(recv_buff[i] == '='){
                while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;     /* 发件人经过编码 */
                i += 3;
                j = 0;
                memset( send_buff, '\0', sizeof(send_buff) );
                /* 发件人名字 */
                while((recv_buff[i] != '=')&&(recv_buff[i] != '?')&&(i < ReceLen)){
                    send_buff[j] = recv_buff[i];
                    j++;
                    i++;        
                }
                memset( p_pop3->DecodeRName, '\0', sizeof(p_pop3->DecodeRName) );
                Base64Decode(send_buff,  strlen(send_buff),p_pop3->DecodeRName);
            }
            else{                                                               /* 未编码 */
                j = 0;
                while((recv_buff[i] != '"')&&(i < ReceLen)){                    /* 发件人名字 */
                    p_pop3->DecodeRName[j] = recv_buff[i];
                    j++;
                    i++;        
                }
            }                       
            while((recv_buff[i] != '<')&&(i < ReceLen)) i++;
            i++;
            j = 0;
            while((recv_buff[i] != '>')&&(i < ReceLen)){                        /* 发件人地址 */
                R_argv[0][j] = recv_buff[i];
                j++;
                i++; 
            }
        }
        /* 抄送 */
        if( strncmp("\nCc: ", &recv_buff[i], 5) == 0 ){
            i += 5;
            while(recv_buff[i] != '<')    i++;
            i++;
            j = 0;
            while(recv_buff[i] != '>'&&(i < ReceLen)){                          /* 抄送人地址 */
                p_pop3->Ccname[j] = recv_buff[i];
                j++;
                i++;        
            }
        }
        /* 日期 */
        if( strncmp("\nDate: ", &recv_buff[i], 7) == 0 ){
            i += 7;
            j = 0;
            while((recv_buff[i] != '\r')&&(i < ReceLen)){                       /* 发送邮件日期 */
                p_pop3->sBufTime[j] = recv_buff[i];
                j++;
                i++;        
            }
        }
        /* 主题 */
        if( strncmp("\nSubject: ", &recv_buff[i], 10) == 0 ){
            i += 10;
            if(recv_buff[i] == '[') {
                while(recv_buff[i] == ']'&&(i < ReceLen)) i++;
                i += 2;
            }
            if(recv_buff[i] == '='){                                            /* 主题经过编码 */
                while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;
                i += 3;
                j = 0;
                memset( send_buff, '\0', sizeof(send_buff) );
                /* 主题 */
                while((recv_buff[i] != '=')&&(recv_buff[i] != '?')&&(i < ReceLen)){
                    send_buff[j] = recv_buff[i];
                    j++;
                    i++;      
                }
                Base64Decode(send_buff,strlen(send_buff),R_argv[1]);
            }
            else{                                                               /* 未编码 */
                j = 0;
                while(recv_buff[i] != '\r'&&(i < ReceLen)){                     /* 主题 */
                    R_argv[1][j] = recv_buff[i];
                    j++;
                    i++;      
                }
            }
        }
        /* 附件的编码方式、名字、内容 */
        if( strncmp("name=", &recv_buff[i], 5) == 0 ){
            i += 5;
            while(strncmp("Content-Transfer-Encoding: ", &recv_buff[i], 27)&&strncmp("filename=", &recv_buff[i], 9)&&(i < ReceLen)) i++;
            if(strncmp("Content-Transfer-Encoding: ", &recv_buff[i], 27) == 0){ /* 查询附件编码方式 */
/* 编码方式 */
                i += 27;
                if(strncmp("base64", &recv_buff[i], 6) == 0 ){
                    i += 6;
                    p_pop3->EncodeType = 1;
                }
                else if(strncmp("quoted-printable", &recv_buff[i], 16) == 0 ) {
                    i += 16;
                    p_pop3->EncodeType = 2;
                }
                else p_pop3->EncodeType = 0;  
/* 附件名字 */
                while(strncmp("filename=", &recv_buff[i], 9)) i++;
                i += 9;
                while(recv_buff[i] != '"'&&i<ReceLen) i++;
                i++;
                if(recv_buff[i] == '='){                                        /* 附件名字经过编码 */
                    while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;
                    i += 3;
                    j = 0;
                    memset(send_buff,'\0', sizeof(send_buff) );
                    /* 附件名字 */
                    while((recv_buff[i] != '=')&&(recv_buff[i] != '?')&&(i < ReceLen)){             
                        send_buff[j] = recv_buff[i];
                        j++;
                        i++;      
                    }
                    Base64Decode(send_buff,  strlen(send_buff),R_argv[2]);
                }
                else{                                                           /* 未编码 */
                    j = 0;
                    while(recv_buff[i] != '"'&&(i < ReceLen)){                  /* 附件名字 */
                        R_argv[2][j] = recv_buff[i];
                        j++;
                        i++;        
                    }
                }
            }
/* 附件名字 */
            else if( strncmp("filename=", &recv_buff[i], 9) == 0 ){
                i += 9;
                while(recv_buff[i] != '"'&&i<ReceLen) i++;
                i++;
                if(recv_buff[i] == '='){
                    while(strncmp("?B?", &recv_buff[i], 3)&&(i < ReceLen)) i++;
                    i += 3;
                    j = 0;
                    memset(send_buff,'\0', sizeof(send_buff) );
                    while((recv_buff[i] != '=')&&(recv_buff[i] != '?')&&(i < ReceLen)){
                        send_buff[j] = recv_buff[i];
                        j++;
                        i++;      
                    }
                    Base64Decode(send_buff,  strlen(send_buff),R_argv[2]);
                }
                else{
                    j = 0;
                    while(recv_buff[i] != '"'&&(i < ReceLen)){
                        R_argv[2][j] = recv_buff[i];
                        j++;
                        i++;        
                    }
                }
/* 附件编码方式 */
                while(strncmp("Content-Transfer-Encoding: ", &recv_buff[i], 27)) i++;
                i += 27;
                if(strncmp("base64", &recv_buff[i], 6) == 0 ){
                    i += 6;
                    p_pop3->EncodeType = 1;
                }
                else if(strncmp("quoted-printable", &recv_buff[i], 16) == 0 ) {
                    i += 16;
                    p_pop3->EncodeType = 2;
                }
                else p_pop3->EncodeType = 0;  
            }
/* 附件内容 */
            while( strncmp("\n\r\n", &recv_buff[i], 3) != 0 &&(i < ReceLen)) i++;
            i += 3;
            if(p_pop3->EncodeType==1){    /* base64 */
                j = 0;
                memset(send_buff,'\0', sizeof(send_buff) );
type1:            while((recv_buff[i] != '=')&&strncmp("\r\n--", &recv_buff[i], 4)&&strncmp("\r\n\r\n", &recv_buff[i], 4)&&(i < ReceLen)&&\
                    (j<attach_max_len)){
                    send_buff[j] = recv_buff[i];
                    j++;
                    i++;        
                }
                if(i>=ReceLen-1) p_pop3->ReceFlag = 1;
                else {
                    p_pop3->ReceFlag = 0;
                    Base64Decode(send_buff,  strlen(send_buff),AttachmentData);
                }
            }
            else if(p_pop3->EncodeType==2){     /* quoted-printable */
                j = 0;
                memset(send_buff,'\0', sizeof(send_buff));
type2:            while(strncmp("\r\n.\r\n", &recv_buff[i], 5)&&strncmp("\r\n--", &recv_buff[i], 4)&&(i < ReceLen)&&(j<attach_max_len)){
                    send_buff[j] = recv_buff[i];
                    j++;
                    i++;        
                }
                if(i>=ReceLen-1) p_pop3->ReceFlag = 1;
                else {
                    p_pop3->ReceFlag = 0;
                    QuotedPrintableDecode(send_buff,(char *)AttachmentData, strlen(send_buff));
                }
            }
            else{    /* 7bit or others */ 
                j = 0;
type0:            while(recv_buff[i] != '\r'&&(i < ReceLen)&&(j<attach_max_len)){
                    AttachmentData[j] = recv_buff[i];
                    j++;
                    i++;        
                }
                if(i>=ReceLen-1) p_pop3->ReceFlag = 1;
                else p_pop3->ReceFlag = 0; 
            }
        }
    }
    if(p_pop3->AnalyMailDat){
#if DEBUG
        printf("addr:\n %s\n",R_argv[0]);                                       /* 发送人的地址    */ 
        printf("send name:\n %s\n",p_pop3->DecodeRName);                        /* 发送人名字 */
        printf("subject:\n %s\n",R_argv[1]);                                    /* 邮件主题 */
        printf("attach name:\n%s\n",R_argv[2]);                                 /* 附件名字 */
        printf("send time:\n %s\n",p_pop3->sBufTime);                           /* 发送时间    */
        printf("attach text:\n%s\n",AttachmentData);                            /* 附件内容    */
#endif
    }
}

/*******************************************************************************
* Function Name  : CH563NET_MailQuery
* Description    : 状态查询
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_MailQuery( void )
{
    if( p_pop3->DiscnntFlag == 1){
        p_pop3->DiscnntFlag = 0;
#ifdef    POP_REFRESH
        if(p_pop3->RefreshTime){
            p_pop3->RefreshTime = 0;
            Delay_ms(200);
            CH563NET_CreatTcpPop3( ); 
        } 
#endif
    }
    if(ReceDatFlag){
#ifdef    POP_RTER                                                                                                               
        if(OrderType == POP_RECEIVE_RTER ){
            ReceDatFlag = 0;    
            CH563NET_POP3Retr( '1' );                                           /* 处理server邮件的全部文本内容    （输入邮件号） */
        }
#endif
#ifdef    POP_DELE 
        if(OrderType == POP_RECEIVE_DELE ){
            ReceDatFlag = 0;    
            CH563NET_POP3Dele( '1' );                                           /* 处理server标记删除，QUIT命令执行时才真正删除（输入邮件号） */
        }
#endif
#ifdef    POP_RSET 
        if(OrderType == POP_RECEIVE_RSET ){ 
            ReceDatFlag = 0;    
            CH563NET_POP3Rset(  );                                              /* 处理撤销所有的DELE命令 */
        }
#endif
#ifdef    POP_TOP 
        if(OrderType == POP_RECEIVE_TOP ){
            ReceDatFlag = 0;    
            CH563NET_POP3Top( '1','3' );                                        /* 返回n号邮件的前m行内容(输入邮件号,行号(必须为自然数)) */
        }
#endif
#ifdef    POP_UIDL 
        if(OrderType == POP_RECEIVE_UIDL ){
            ReceDatFlag = 0;    
            CH563NET_POP3Uidl( '1' );                                           /* 处理server返回用于该指定邮件的唯一标识，如果没有指定，返回所有的。（输入邮件号）*/
        }
#endif
    }

}

/*********************************** endfile **********************************/
