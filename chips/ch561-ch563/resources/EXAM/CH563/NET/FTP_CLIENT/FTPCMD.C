/********************************** (C) COPYRIGHT *********************************
* File Name          : FTPCMD.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563芯片FTP命令代码
*                    : MDK3.36@ARM966E-S,Thumb,小端
**********************************************************************************/



/******************************************************************************/
FTP   ftp;
char  send_buff[536];    // 发送数据缓存区
UINT32 ListNum = 0;

/*******************************************************************************
* Function Name  : CH563NET_FTPLoginUser
* Description    : 认证用户名   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPLoginUser( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "USER %s\r\n", pUserName);
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPLoginPass
* Description    : 认证口令   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPLoginPass( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "PASS %s\r\n", pPassword);
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPSyst
* Description    : 查询服务器系统类型   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPSyst( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "SYST\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPList
* Description    : 获取文件列表，文件名及时间   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPList( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "LIST\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPNlst
* Description    : 获取文件列表   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPNlst( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "NLST\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPType
* Description    : 设置文件传输类型   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPType( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "TYPE A\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPSize
* Description    : 获取文件信息   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPSize( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "SIZE %s\r\n",FileName);
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPRetr
* Description    : 下载文件   
* Input          : None
* Output         : None                      
* Return         : None
*******************************************************************************/
void CH563NET_FTPRetr( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "RETR %s\r\n",FileName);
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCdup
* Description    : 返回上一级目录   
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void CH563NET_FTPCdup( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "CDUP\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPMkd
* Description    : 创建目录   
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void CH563NET_FTPMkd(UINT8 ReName )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    if(ReName){
        ListNum++;
        sprintf( send_buff, "MKD %s%d\r\n",ListName,ListNum);
    }
    else sprintf( send_buff, "MKD %s\r\n",ListName);
    sprintf( ListMkd,&send_buff[4] );                                            /* 保存目录名 */
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPRmd
* Description    : 删除目录   
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void CH563NET_FTPRmd( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "RMD %s\r\n",ListName); 
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPAppe
* Description    : 上传文件   
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void CH563NET_FTPAppe( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "APPE %s\r\n",FileName);
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPAllo
* Description    : 下载文件   
* Input          : None
* Output         : None               
* Return         : None
*******************************************************************************/
void CH563NET_FTPAllo( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "ALLO 500000\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPNoop
* Description    : 发送空指令   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPNoop( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "NOOP\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPPasv
* Description    : 被动获取连接端口号   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPPasv( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "PASV\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl);
}

/*******************************************************************************
* Function Name  : CH563NET_FTPPort
* Description    : 设定连接端口号   
* Input          : port - 端口号
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPPort( UINT16 port)
{
    memset( send_buff, '\0', sizeof(send_buff));
    sprintf( send_buff, "PORT %s%d,%d\r\n",CharIP,(UINT8)(port/256),(UINT8)(port%256));
#if DEBUG
    printf("PORT :%s\n", send_buff);
#endif
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCwd
* Description    : 打开目录   
* Input          : index -0为当前目录，1为下一级目录
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPCwd( UINT8 index )
{                                                     
    memset( send_buff, '\0', sizeof(send_buff) );
    if(index==1) sprintf( send_buff, "CWD /%s\r\n",ListMkd);
    else sprintf( send_buff, "CWD /%s\r\n",ListName);
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPQuit
* Description    : 退出登录   
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPQuit( void )
{
    memset( send_buff, '\0', sizeof(send_buff) );
    sprintf( send_buff, "QUIT\r\n");
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.SocketCtl );
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCheckLogin
* Description    : 登陆握手信号
* Input          : recv_buff- 握手信息
                   check_type-检测的类型
* Output         : None
* Return         : 函数执行状态
*******************************************************************************/
UINT8 CH563NET_FTPCheckLogin( char *recv_buff )
{
    if( strncmp("220", recv_buff, 3) == 0 ){                                    /* 连接成功 */
        CH563NET_FTPLoginUser( );
    }
    else if( strncmp("331", recv_buff, 3) == 0 ){                               /* 用户名正确 */
        CH563NET_FTPLoginPass( );                   
    }
    else if( strncmp("230", recv_buff, 3) == 0 ){                               /* 登录成功 */
        ftp.CmdStatus = FTP_MACH_LOGINSUC;
    }
    else if( strncmp("530", recv_buff, 3) == 0 ){                               /* 登陆失败 */
        CH563NET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );
    }
    else if( strncmp("221", recv_buff, 3) == 0 ){                               /* 退出登陆 */
        CH563NET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );                 /* 关闭连接 */
    }
    else return FTP_ERR_LOGIN;
    return FTP_CHECK_SUCCESS; 
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCheckSearch
* Description    : 搜查文件握手信号
* Input          : recv_buff- 握手信息
                   check_type-检测的类型
* Output         : None
* Return         : 函数执行状态
*******************************************************************************/
UINT8 CH563NET_FTPCheckSearch( char *recv_buff )
{
    if( strncmp("200 Type", recv_buff, 8) == 0 ){                               /* 格式类型 */ 
        CH563NET_FTPDatServer( &ftp.SouPort );
        CH563NET_FTPPort( ftp.SouPort );
    }
    else if( strncmp("200 PORT", recv_buff, 8 ) == 0){                          /* 目的IP地址及端口号 */ 
        CH563NET_FTPList( );
    }
    else if( strncmp("150", recv_buff, 3) == 0 ){                               /* 获取列表成功 */
        ftp.CmdDataS = FTP_MACH_RECEDATA;
    }
    else if( strncmp("226", recv_buff, 3) == 0 ){                               /* 列表信息结束 */
        if(ftp.InterCmdS == FTP_MACH_FINDFILE) ftp.CmdStatus = FTP_MACH_SEARCHSUC;
        ftp.InterCmdS = 0;
    }
    else if( strncmp("425", recv_buff, 3) == 0 ){                               /* 无法建立数据连接 */
        CH563NET_FTPQuit( );
    }                                                            
    else if( strncmp("250", recv_buff, 3) == 0 ){                               /* 打开目录 */
        ftp.InterCmdS = FTP_MACH_FINDFILE;
        CH563NET_FTPType( );
    }
    else if( strncmp("221", recv_buff, 3) == 0 ){                               /* 退出登陆 */
        CH563NET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );                 /* 关闭连接 */
    }
    else return FTP_ERR_SEARCH;
    return FTP_CHECK_SUCCESS;
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCheckGetfile
* Description    : 下载文件握手信号
* Input          : recv_buff- 握手信息
                   check_type-检测的类型
* Output         : None
* Return         : 函数执行状态
*******************************************************************************/
UINT8 CH563NET_FTPCheckGetfile( char *recv_buff )
{            
    if( strncmp("200 Type", recv_buff, 8) == 0 ){                               /* 格式类型 */ 
        CH563NET_FTPSize( );
    }
    else if( strncmp("213", recv_buff, 3) == 0 ){                               /* 文件信息 */ 
        CH563NET_FTPDatServer( &ftp.SouPort );
        CH563NET_FTPPort( ftp.SouPort );
    }
    else if( strncmp("200 PORT", recv_buff, 8) == 0 ){                          /* 目的IP地址及端口号 */ 
        CH563NET_FTPRetr( );
    }
    else if( strncmp("150", recv_buff, 3) == 0 ){                               /* 获取文件命令成功 */ 
        ftp.CmdDataS = FTP_MACH_RECEDATA;
        memset( send_buff, '\0', sizeof(send_buff) );
    }
    else if(strncmp("550", recv_buff, 3) == 0 ){                                /* 没有找到文件 */
        CH563NET_FTPQuit( );
    }
    else if(strncmp("226", recv_buff,3) == 0 ){                                 /* 传输完成 */
        ftp.CmdStatus = FTP_MACH_GETSUC;                     
    }
    else if( strncmp("221", recv_buff, 3) == 0 ){                               /* 退出登陆 */
        CH563NET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );                 /* 关闭连接 */
    }
    else return FTP_ERR_GETFILE;
    return (FTP_CHECK_SUCCESS);

}

/*******************************************************************************
* Function Name  : CH563NET_FTPCheckPutfile
* Description    : 验证上传文件握手信号
* Input          : recv_buff- 握手信息
                   check_type-检测的类型
* Output         : None
* Return         : 函数执行状态
*******************************************************************************/
UINT8 CH563NET_FTPCheckPutfile( char *recv_buff )
{
    if( strncmp("250", recv_buff, 3) == 0 ){                                    /* 返回上一级目录 */
        if(ftp.InterCmdS == FTP_MACH_MKDLIST){
            CH563NET_FTPMkd( 0 );
        }
        else if(ftp.InterCmdS == FTP_MACH_PUTFILE){
            CH563NET_FTPType( );
        }
    }
    else if( strncmp("257", recv_buff, 3) == 0 ){                               /* 创建成功 */
        ftp.InterCmdS = FTP_MACH_PUTFILE;                                       /* 上传文件 */
        CH563NET_FTPCwd( 1 );                                                   /* 打开目录 */
    }
    else if( strncmp("550", recv_buff, 3) == 0 ){                               /* 创建的目录名存在 */
#if    1
        CH563NET_FTPMkd( 1 );                                                   /* 该目录名存在则创建另外一个目录名（在原先目录名后累加数字如：TXET1）*/
#else
        CH563NET_FTPCwd( 0 );                                                   /* 该目录名存在则直接打开 */
#endif                                          
    }
    if( strncmp("200 Type", recv_buff, 8) == 0 ){                               /* 格式类型 */ 
        CH563NET_FTPDatServer( &ftp.SouPort );
        CH563NET_FTPPort( ftp.SouPort );
    }
    else if( strncmp("200 PORT", recv_buff, 8 ) == 0){                          /* 目的IP地址及端口号 */
        CH563NET_FTPAppe( );                                    
    }
    else if( strncmp("150", recv_buff, 3) == 0 ){                               /* 请求上传文件命令成功 */
        ftp.CmdDataS = FTP_MACH_SENDDATA;                                       /* 需要发送数据 */
    }
    else if( strncmp("226", recv_buff, 3) == 0 ){                               /* 上传结束 */
        ftp.CmdStatus = FTP_MACH_PUTSUC;             
    }
    else if( strncmp("221", recv_buff, 3) == 0 ){                               /* 退出登陆 */
        CH563NET_SocketClose( ftp.SocketCtl,TCP_CLOSE_NORMAL );                 /* 关闭连接 */
    }
    else return FTP_ERR_PUTFILE;
    return (FTP_CHECK_SUCCESS);
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCheckRespond
* Description    : 验证握手信号
* Input          : recv_buff- 握手信息
                   check_type-检测的类型
* Output         : None
* Return         : 函数执行状态
*******************************************************************************/
UINT8 CH563NET_FTPCheckRespond( char *recv_buff,UINT8 check_type )
{
    UINT8 s;
    
    if( strncmp("421", recv_buff, 3) == 0 ){                                    /* 服务器意外断开 */
        return (FTP_CHECK_SUCCESS);
    }
    switch(check_type){
        case FTP_CMD_LOGIN:                                                     /* 核对登陆命令过程中的返回信息 */
            s = CH563NET_FTPCheckLogin( recv_buff );
            return s;
        case FTP_CMD_SEARCH:                                                    /* 核对搜查文件过程中的返回信息 */
            s = CH563NET_FTPCheckSearch( recv_buff );
            return s;
        case FTP_CMD_GETFILE:                                                    /* 核对下载文件过程中的返回信息 */
            s = CH563NET_FTPCheckGetfile( recv_buff );
            return s;
        case FTP_CMD_PUTFILE:                                                    /* 核对上传文件过程中的返回信息 */
            s = CH563NET_FTPCheckPutfile( recv_buff );
            return s;
        default:
            return FTP_ERR_UNKW;
    }
}

/*******************************************************************************
* Function Name  : CH563NET_FTPFindList
* Description    : 核对目录名字
* Input          : None
* Output         : None
* Return         : 函数执行状态
*******************************************************************************/
UINT8 CH563NET_FTPFindList( char *pReceiveData ) 
{
    UINT32 len,i,j;

    len = ftp.RecDatLen;
    for(i=0;i<len;i++){
        if(strncmp(ListName,&pReceiveData[i],strlen(ListName))==0){
            j = i;
            while(strncmp("<DIR>",&pReceiveData[j],strlen("<DIR>"))){
                j--;
                if( pReceiveData[j]== 'M' ) return FALSE;
            }
#if DEBUG
    printf("*********\nfind list\n*********\n");
#endif
            return FTP_CHECK_SUCCESS;                                            /* 找到了指定的文件名 */
        } 
    }    
    return FTP_ERR_UNLIST;
}

/*******************************************************************************
* Function Name  : CH563NET_FTPFindFile
* Description    : 核对文件名字
* Input          : None
* Output         : None
* Return         : 函数执行状态
*******************************************************************************/
UINT8 CH563NET_FTPFindFile( char *pReceiveData ) 
{
    UINT32 len,i;
    len = ftp.RecDatLen;
    for(i=0;i<len;i++){
        if(strncmp(FileName,&pReceiveData[i],strlen(FileName))==0){
#if DEBUG
    printf("*********\nfind file\n*********\n");
#endif
            return FTP_CHECK_SUCCESS;                                            /* 找到了指定的文件名 */
        } 
    }    
    return FTP_ERR_UNFILE;
}

/*******************************************************************************
* Function Name  : ch563ftp_loginstart
* Description    : 登陆
* Input          : None
* Output         : None
* Return         : 函数执行状态
*******************************************************************************/
UINT8 CH563NET_FTPLogin( void )
{
    if(ftp.CmdFirtS == 0){
        ftp.CmdFirtS  = 1;
        ftp.FileCmd = FTP_CMD_LOGIN;                                             /* 进入搜查文件状态 */
        CH563NET_FTPCtlClient( );
    }
    if( ftp.CmdStatus == FTP_MACH_LOGINSUC ){                                    /* 登陆成功 */
#if DEBUG
    printf("************\nlogin success\n*********\n");
#endif
        ftp.CmdFirtS = 0;
        ftp.CmdStatus = 0;
        ftp.FileCmd = 0;      
        return FTP_COMMAND_SUCCESS;    
    }    
    return FTP_COMMAND_CONTINUE;    
}

/*******************************************************************************
* Function Name  : ch563ftp_searchfile
* Description    : 搜查文件
* Input          : pListNAme - 需要搜查的文件所在目录名
                   pFileName - 该目录下的文件名
* Output         : None
* Return         : 函数执行状态
*******************************************************************************/
UINT8 CH563NET_FTPSearch(char *pListNAme,char *pFileName )
{
    if( ftp.CmdFirtS == 0 ){
        ftp.CmdFirtS = 1;
        ftp.FileCmd = FTP_CMD_SEARCH;                                           /* 进入搜查文件状态 */
        ftp.InterCmdS = FTP_MACH_FINDLIST;                                      /* 查找目录 */
        memset((void *)ListName,0,sizeof(ListName));        
        sprintf( ListName,pListNAme );                                          /* 输入文件夹名字 */
        memset((void *)FileName,'\0',sizeof(FileName));               
        sprintf( FileName,pFileName );                                          /* 输入搜查文件的名字 */
        CH563NET_FTPType( );
    }
    if(ftp.TcpStatus == FTP_MACH_DISCONT){
        if(ftp.CmdStatus == FTP_MACH_SEARCHSUC){                                /* 搜查文件完成 */
#if DEBUG
    printf("**********\nsearch success\n*********\n");
#endif
            ftp.CmdFirtS = 0;
            ftp.CmdStatus = 0;
            ftp.FileCmd = 0;     
            return FTP_COMMAND_SUCCESS;
        }
    }
    return FTP_COMMAND_CONTINUE; 
}

/*******************************************************************************
* Function Name  : ch563ftp_getfile
* Description    : 下载文件
* Input          : pFileName - 下载的文件名
* Output         : None
* Return         : None
*******************************************************************************/
UINT8 CH563NET_FTPGetFile( char *pFileName )
{
    if(ftp.CmdFirtS == 0){
        ftp.CmdFirtS  = 1;
        ftp.FileCmd = FTP_CMD_GETFILE;                                          /* 进入下载文件状态 */
        ftp.InterCmdS = FTP_MACH_GETFILE;                                       /* 下载文件 */
        memset((void *)FileName,'\0',sizeof(FileName));               
        sprintf( FileName,pFileName );                                          /* 输入下载文件的名字 */
        CH563NET_FTPType( );
    }
    if(ftp.TcpStatus == FTP_MACH_DISCONT){
        if(ftp.CmdStatus == FTP_MACH_GETSUC){                                   /* 下载文件成功 */
#if DEBUG
    printf("*********\ngetfile success\n*********\n");
#endif
            ftp.CmdFirtS = 0;
            ftp.CmdStatus = 0;
            ftp.FileCmd = 0;       
            return FTP_COMMAND_SUCCESS;
        }
    }    
    return FTP_COMMAND_CONTINUE; 
}

/*******************************************************************************
* Function Name  : ch563ftp_getfile
* Description    : 上传文件
* Input          : pListNAme - 上传文件所需保存的目录名
                   pFileName - 上传文件所需保存的文件名
* Output         : None
* Return         : 函数执行状态
*******************************************************************************/
UINT8 CH563NET_FTPPutFile(char *pListNAme, char *pFileName)
{
    if(ftp.CmdFirtS ==0){
        ftp.CmdFirtS = 1;
        ftp.FileCmd = FTP_CMD_PUTFILE;                                          /* 进行上传文件状态 */
        ftp.InterCmdS = FTP_MACH_MKDLIST;                                       /* 创建目录 */
        memset((void *)ListName,'\0',sizeof(ListName));               
        sprintf( ListName,pListNAme );                                          /* 输入上传文件的名字 */
        memset((void *)FileName,'\0',sizeof(FileName));               
        sprintf( FileName,pFileName );                                          /* 输入上传文件的名字 */
        CH563NET_FTPCdup( );
    }
    if(ftp.TcpStatus == FTP_MACH_DISCONT){
        if(ftp.CmdStatus == FTP_MACH_PUTSUC){                                   /* 上传文件成功 */
#if DEBUG
    printf("*********\nputfile success\n*********\n");
#endif
            ftp.CmdFirtS = 0;
            ftp.CmdStatus = 0;
            ftp.FileCmd = 0;
            return FTP_COMMAND_SUCCESS;
        }
    }    
    return FTP_COMMAND_CONTINUE;     
}

/*********************************** endfile **********************************/
