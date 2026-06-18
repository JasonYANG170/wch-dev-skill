/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/28
* Description        : CH549 SD卡文件系统操作演示
工程需添加文件：DEBUG.C\SD.C\Main.C\SPI.C\CH549UFI.LIB
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\SPI\SPI_SDCARD\SD.H"
#pragma  NOAREGS
#ifdef MAX_PACKET_SIZE
#undef MAX_PACKET_SIZE
#define MAX_PACKET_SIZE                 1       /* 不用RxBuffer,TxBuffer,只需定义小一点即可，但是必须定义,因为U盘操作时文件系统库有引用 */
#define DISK_BASE_BUF_LEN               512     /* 默认的磁盘数据缓冲区大小为512字节(可以选择为2048甚至4096以支持某些大扇区的U盘),为0则禁止在本文件中定义缓冲区并由应用程序在pDISK_BASE_BUF中指定 */
#define NO_DEFAULT_ACCESS_SECTOR        1       /* 禁止默认的磁盘扇区读写子程序,下面用自行编写的程序代替它 */
#define NO_DEFAULT_DISK_CONNECT         1       /* 禁止默认的检查磁盘连接子程序,下面用自行编写的程序代替它 */
//#define NO_DEFAULT_FILE_ENUMER        1       /* 禁止默认的文件名枚举回调程序,下面用自行编写的程序代替它 */
#include ".\USB\USB_LIB\CH549UFI.H"
#include ".\USB\USB_LIB\CH549UFI.C"
/********************** 以下定义为解决库编译警告问题 **************************/
UINT8X RxBuffer[ MAX_PACKET_SIZE ];
UINT8X TxBuffer[ MAX_PACKET_SIZE ];
UINT8 USBHostTransact( UINT8 endp_pid, UINT8 tog, UINT16 timeout )
{
    endp_pid = endp_pid;
    tog = tog;
    timeout = timeout;
    return 0;
}
UINT8 HostCtrlTransfer( PUINT8X DataBuf, PUINT8I RetLen )
{
    DataBuf = DataBuf;
    RetLen = RetLen;
    return 0;
}
void CopySetupReqPkg( PUINT8C pReqPkt )
{
    pReqPkt = pReqPkt;
}
UINT8 CtrlGetConfigDescr( void )
{
    return 0;
}
UINT8 CtrlSetUsbConfig( UINT8 cfg )
{
    cfg = cfg;
    return 0;
}
UINT8 CtrlClearEndpStall( UINT8 endp )
{
    endp = endp;
    return 0;
}
/******************以上保留,仅兼容U盘库,SD卡时无他用*****************************/
/******************以下为SD卡底层与库函数接口处理部分 ***************************/
#ifdef NO_DEFAULT_DISK_CONNECT
/*******************************************************************************
* Function Name  : CH549DiskConnect
* Description    : 磁盘连接检测（重定义）
* Input          : None
* Output         : None
* Return         : ERR_SUCCESS / ERR_USB_DISCON
*******************************************************************************/
UINT8 CH549DiskConnect( void )
{
    if(SD_Check_Insert() == TRUE)                                              /* 卡已插入 */
    {
        if(CH549DiskStatus < DISK_CONNECT)
        {
            CH549DiskStatus = DISK_CONNECT;
        }
        return( ERR_SUCCESS );
    }
    else                                                                       /* 卡未插入 */
    {
        CH549DiskStatus = DISK_DISCONNECT;
        return( ERR_USB_DISCON );
    }
}
#endif
#ifdef NO_DEFAULT_ACCESS_SECTOR
/*******************************************************************************
* Function Name  : CH549ReadSector
* Description    : 扇区读取（重定义）,由文件系统库调用
* Input          : SectCount 读取扇区数
*                  DataBuf 指向读取缓冲区
*                  CH549vLbaCurrent 操作起始扇区
* Output         : None
* Return         : 操作状态
*******************************************************************************/
UINT8 CH549ReadSector( UINT8 SectCount, PUINT8X DataBuf )
{
#if 0
    UINT16X i;
    printf("read sector 0x%08lx\n",CH549vLbaCurrent);
#endif
    CH549IntStatus = SD_READ_MULT_SECTOR(DataBuf,CH549vLbaCurrent,SectCount );
#if 0
    for( i=0; i!=DISK_BASE_BUF_LEN; i++)
    {
        printf("%02x ",(UINT16)DataBuf[i]);
    }
    printf("\n");
#endif
    if ( CH549IntStatus == SD_ERR_CMD_OK )
    {
        CH549IntStatus = ERR_SUCCESS;
    }
    else
    {
        CH549IntStatus = ERR_USB_DISK_ERR;
    }
    return( CH549IntStatus );
}
/*******************************************************************************
* Function Name  : CH549WriteSector
* Description    : 扇区写（重定义）,由文件系统库调用
* Input          : SectCount 写扇区数
*                  DataBuf 指向写缓冲区
*                  CH549vLbaCurrent 操作起始扇区
* Output         : None
* Return         : 操作状态
*******************************************************************************/
#ifdef  EN_DISK_WRITE
UINT8 CH549WriteSector( UINT8 SectCount, PUINT8X DataBuf )  /* 将缓冲区中的多个扇区的数据块写入磁盘 */
{
#if 0
    UINT16X i;
    printf("write sector 0x%08lx\n",CH549vLbaCurrent);
#endif
    CH549IntStatus = SD_WRITE_MULT_SECTOR( DataBuf,CH549vLbaCurrent,SectCount );
#if 0
    for( i=0; i!=DISK_BASE_BUF_LEN; i++)
    {
        printf("%02x ",(UINT16)DataBuf[i]);
    }
    printf("\n");
#endif
    if ( CH549IntStatus == SD_ERR_CMD_OK )
    {
        CH549IntStatus = ERR_SUCCESS;
    }
    else
    {
        CH549IntStatus = ERR_USB_DISK_ERR;
    }
    return( CH549IntStatus );
}
#endif
#endif
/*******************************************************************************
* Function Name  : CH549DiskInit
* Description    : SD卡初始化，并分析文件系统,类似于U盘操作时函数CH549DiskReady
*                  卡初始化调用一次,找到DBR地址
* Input          : None
* Output         : 更新 CH549DiskStatus CH549vLbaCurrent
* Return         : 操作状态
*******************************************************************************/
UINT8 CH549DiskInit( void )
{
    UINT8X s;
    s = SD_Init();                                                             //SD卡初始化
    if(s!=SD_ERR_CMD_OK)
    {
        return s;
    }
    CH549vLbaCurrent = 0;                                                      //分析DBR
    s = CH549ReadSector(1,pDISK_BASE_BUF);                                     //读取0扇区的数据信息
    if((pDISK_BASE_BUF[0] == 0xeb)&&(pDISK_BASE_BUF[2] == 0x90))               //DBR的物理扇区号是0
    {
        CH549vLbaCurrent = 0;
    }
    else
    {
        CH549vLbaCurrent = pDISK_BASE_BUF[454] + (((UINT32)pDISK_BASE_BUF[455])<<8) + (((UINT32)pDISK_BASE_BUF[456])<<16) + (((UINT32)pDISK_BASE_BUF[457])<<24);
        if(CH549vLbaCurrent)                                                  //扇区偏移非0，找到DBR
        {
#if DE_PRINTF
            printf("The active part is PART1!\r\n");
#endif
        }
        else
        {
            CH549vLbaCurrent = pDISK_BASE_BUF[470] + (((UINT32)pDISK_BASE_BUF[471])<<8) + (((UINT32)pDISK_BASE_BUF[472])<<16) + (((UINT32)pDISK_BASE_BUF[473])<<24);
            if(CH549vLbaCurrent)                                              //扇区偏移非0，找到DBR
            {
#if DE_PRINTF
                printf("The active part is PART2!\r\n");
#endif
            }
            else
            {
                CH549vLbaCurrent = pDISK_BASE_BUF[486] + (((UINT32)pDISK_BASE_BUF[487])<<8) + (((UINT32)pDISK_BASE_BUF[488])<<16) + (((UINT32)pDISK_BASE_BUF[489]<<24));
                if(CH549vLbaCurrent)                                          //扇区偏移非0，找到DBR
                {
#if DE_PRINTF
                    printf("The active part is PART3!\r\n");
#endif
                }
                else
                {
                    CH549vLbaCurrent = pDISK_BASE_BUF[502] + (((UINT32)pDISK_BASE_BUF[503])<<8) + (((UINT32)pDISK_BASE_BUF[504])<<16) + (((UINT32)pDISK_BASE_BUF[505])<<24);
                    if(CH549vLbaCurrent)                                      //扇区偏移非0，找到DBR
                    {
#if DE_PRINTF
                        printf("The active part is PART4!\r\n");
#endif
                    }
                    else
                    {
                        return ERR_MBR_ERROR;
                    }
                }
            }
        }
    }
    CH549DiskStatus = DISK_MOUNTED;
    return ERR_SUCCESS;
}
/*******************************************************************************
* Function Name  : mStopIfError
* Description    : 检查操作状态,如果错误则显示错误代码并停机
* Input          : iError
* Output         : None
* Return         : None
*******************************************************************************/
void mStopIfError( UINT8 iError )
{
    if ( iError == ERR_SUCCESS )
    {
        return;    //操作成功
    }
    printf( "Error: %02X\n", (UINT16)iError );  /* 显示错误 */
    while ( 1 )
    {
//      LED_TMP=0;                                                            //LED闪烁
//      mDelaymS( 100 );
//      LED_TMP=1;
//      mDelaymS( 100 );
    }
}
UINT8X buf[100];
/*******************************************************************************
* Function Name  : main
* Description    : 程序入口
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main()
{
    UINT8X i,s,c;
    CfgFsys( );                                                                //CH549时钟选择配置
    mDelaymS(20);
#if DE_PRINTF
    mInitSTDIO( );                                                             //串口0初始化
    printf("SD Card demo start ...\n");
#endif
    CH549LibInit();                                                            //库初始化
#if DE_PRINTF
    printf("Wait SD Insert...\n");
#endif
    while(1)
    {
        CH549DiskConnect();
        if( CH549DiskStatus >= DISK_CONNECT )                                  //磁盘连接
        {
            if(CH549DiskStatus == DISK_CONNECT)                                //说明刚刚连接，需初始化卡
            {
#if DE_PRINTF
                printf("Detect the Card\n");
#endif
                mDelaymS( 50 );                                                //插入后延时
                for ( i = 0; i != 10; i ++ )
                {
#if DE_PRINTF
                    printf( "Wait DiskReady\n" );
#endif
                    s = CH549DiskInit();                                       //等待SD准备好
                    if ( s == ERR_SUCCESS )
                    {
                        break;
                    }
                    else
                    {
#if DE_PRINTF
                        printf("%02x\n",(UINT16)s);
#endif
                    }
                    mDelaymS( 50 );
                }
            }
            if( CH549DiskStatus >= DISK_MOUNTED )                              //可以进行文件操作
            {
                /* 创建文件演示 */
#if DE_PRINTF
                printf( "Create\n" );
#endif
                strcpy( mCmdParam.Create.mPathName, "/NEWFILE.TXT" );          //新文件名,在根目录下,中文文件名
                s = CH549FileCreate( );                                        //新建文件并打开,如果文件已经存在则先删除后再新建
                mStopIfError( s );
                /* 写文件演示 */
#if DE_PRINTF
                printf( "ByteWrite\n" );
#endif
                //实际应该判断写数据长度和定义缓冲区长度是否相符，如果大于缓冲区长度则需要多次写入
                i = sprintf( buf,"Note: \xd\xa这个程序是以字节为单位进行U盘文件读写,549简单演示功能。\xd\xa");
                for(c=0; c<10; c++)
                {
                    mCmdParam.ByteWrite.mByteCount = i;                        //指定本次写入的字节数
                    mCmdParam.ByteWrite.mByteBuffer = buf;                     //指向缓冲区
                    s = CH549ByteWrite( );                                     //以字节为单位向文件写入数据
                    mStopIfError( s );
#if DE_PRINTF
                    printf("成功写入 %02X次\n",(UINT16)c);
#endif
                }
                /* 关闭并保存文件 */
#if DE_PRINTF
                printf( "Close\n" );
#endif
                mCmdParam.Close.mUpdateLen = 1;                                //自动计算文件长度,以字节为单位写文件,建议让程序库关闭文件以便自动更新文件长度
                i = CH549FileClose( );
                mStopIfError( i );
                /* 等待卡移除 */
                while(1)
                {
                    CH549DiskConnect();
                    if( CH549DiskStatus == DISK_DISCONNECT )
                    {
                        break;
                    }
                }
            }
        }
    }
}