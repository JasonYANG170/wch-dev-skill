/********************************** (C) COPYRIGHT ******************************
* File Name          : FTP_CLIENT.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563芯片FTP接口命令
*                    : MDK3.36@ARM966E-S,Thumb,小端
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "CH563BAS.H"
#include "CH563NET.H"
#include "FTPINC.H"
#include "FTPCMD.C"
#include "FTPFILE.C"

/*******************************************************************************
* Function Name  : CH563NET_FTPProcessReceDat
* Description    : 读取下载数据   
* Input          : recv_buff  -563接收到的数据
*                  check_type -当前命令类型
*                  socketid   -socket索引
* Output         : None               
* Return         : None
*******************************************************************************/
void CH563NET_FTPProcessReceDat( char *recv_buff,UINT8 check_type,UINT8 socketid )
{
    UINT8 S;
    if( socketid == ftp.DatTransfer ){
        if(ftp.CmdDataS == FTP_MACH_RECEDATA){                                  /* 接受文件数据 */
            if( ftp.InterCmdS == FTP_MACH_GETFILE ){ 
                S = CH563NET_FTPFileWrite(recv_buff,strlen(recv_buff));
            }
            else if(ftp.InterCmdS == FTP_MACH_FINDLIST ){
                S = CH563NET_FTPFindList( recv_buff );                          /* 验证传输数据，用于查找指定的目录名 */
                if( S == FTP_CHECK_SUCCESS ) ftp.FindList = 1;                  /* 查到指定的目录名 */
            }
            else if(ftp.InterCmdS == FTP_MACH_FINDFILE ){
                S = CH563NET_FTPFindFile( recv_buff );                          /* 查找文件 */    
                if( S == FTP_CHECK_SUCCESS ) ftp.FindFile = 1;                  /* 找到文件 */
            }
        }
    }
    else if( socketid == ftp.SocketCtl ){                                       /* 接收为命令应答 */            
        S = CH563NET_FTPCheckRespond(recv_buff,check_type);
    }
}

/*******************************************************************************
* Function Name  : CH395_FTPSendFile
* Description    : 发送数据
* Input          : NONE
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPSendFile( void )
{
    UINT8 S;    
    
    S = CH563NET_FTPFileOpen( FileName );
    if(S == FTP_CHECK_SUCCESS)    CH563NET_FTPFileRead( );
    CH563NET_FTPSendData( send_buff, strlen(send_buff),ftp.DatTransfer );
    if(ftp.CmdDataS == FTP_MACH_DATAOVER){
        CH563NET_SocketClose( ftp.DatTransfer,TCP_CLOSE_NORMAL );
    }    
}

/*******************************************************************************
* Function Name  : CH563NET_FTPCheckRespond
* Description    : 查询状态，执行相应命令
* Input          : recv_buff  -握手信息
                   check_type -检测的类型
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPClientCmd( void )
{
    if(ftp.CmdDataS == FTP_MACH_SENDDATA){
        if(ftp.TcpStatus == FTP_MACH_CONNECT) CH563NET_FTPSendFile( );          /* 向服务器发送数据 */
        return ;     
    }
    if(ftp.CmdDataS == FTP_MACH_DATAOVER){
        if(ftp.FindList == 1){                                                  /* 找到目录名 */
            ftp.FindList = 0;
            CH563NET_FTPCwd( 0 );
        }
    }
    if(ftp.FileCmd){
        CH563NET_FTPInterCmd( );                                                /* 执行对应的接口命令 */
    }
}

/*******************************************************************************
* Function Name  : CH563NET_FTPInterCmd
* Description    : 执行对应的命令，可自行调整执行顺序
* Input          : NONE
* Output         : None
* Return         : None
*******************************************************************************/
void CH563NET_FTPInterCmd( void )
{
    switch(ftp.FileCmd){
        case FTP_CMD_LOGIN:                                                     /* 登陆 */
            if( CH563NET_FTPLogin( ) == FTP_COMMAND_SUCCESS ){                  /* 登陆成功,可进行其他操作 */
                CH563NET_FTPSearch("USER","FILELIST.txt" );                     /* 开始查询指定的目录 */            
            }
            break;
        case FTP_CMD_SEARCH:                                                    /* 搜查文件（参数：目录名，文件名）*/
            if( CH563NET_FTPSearch("USER","FILELIST.txt" ) == FTP_COMMAND_SUCCESS ){        
                if( ftp.FindFile )CH563NET_FTPGetFile("FILELIST.txt" );         /* 搜查到指定的目录下的文件则开始下载文件 */
                else CH563NET_FTPQuit( );                                       /* 没搜查到指定的目录下的文件则退出（也可进行其他操作，如上传）*/
            }
            break;
        case FTP_CMD_GETFILE:                                                   /* 下载文件（参数：文件名）*/ 
            if(CH563NET_FTPGetFile("FILELIST.txt" ) == FTP_COMMAND_SUCCESS ){   /* 下载文件成功，可进行其他操作 */
                CH563NET_FTPPutFile("TEXT","abc.txt");                          /* 上传文件 */
            }
            break;
        case FTP_CMD_PUTFILE:                                                   /* 上传文件（参数：目录名，文件名）*/
            if( CH563NET_FTPPutFile("TEXT","abc.txt")== FTP_COMMAND_SUCCESS ){  /* 上传文件成功，可进行其他操作 */
                CH563NET_FTPQuit( );                                            /* 退出 */
            }
            break;
        default:
            break;
    }
}

/*********************************** endfile **********************************/
