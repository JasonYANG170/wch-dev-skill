/* 2014.09.09
*****************************************
**   Copyright  (C)  W.ch  1999-2014   **
**   Web:      http://wch.cn           **
*****************************************
**  USB-flash File Interface for CH56x **
**  ADS 1.2, Keil 3.11/4.12 @ARM946E-S,v5TE **
*****************************************
*/
/* CH56x U盘主机文件系统接口, 支持: FAT12/FAT16/FAT32 */

//#define DISK_BASE_BUF_LEN		512	/* 默认的磁盘数据缓冲区大小为512字节,建议选择为2048甚至4096以支持某些大扇区的U盘,为0则禁止在本文件中定义缓冲区并由应用程序在pDISK_BASE_BUF中指定 */
/* 如果需要复用磁盘数据缓冲区以节约RAM,那么可将DISK_BASE_BUF_LEN定义为0以禁止在本文件中定义缓冲区,而由应用程序在调用CH56xLibInit之前将与其它程序合用的缓冲区起始地址置入pDISK_BASE_BUF变量 */

//#define NO_DEFAULT_ACCESS_SECTOR	1		/* 禁止默认的磁盘扇区读写子程序,下面用自行编写的程序代替它 */
//#define NO_DEFAULT_FILE_ENUMER		1		/* 禁止默认的文件名枚举回调程序,下面用自行编写的程序代替它 */

//#include "CH56XBAS.H"
#include "CH56XUFI.H"

CMD_PARAM mCmdParam;						/* 命令参数 */
#if DISK_BASE_BUF_LEN > 0
__align(8)	UINT8  DISK_BASE_BUF[ DISK_BASE_BUF_LEN ];	/* 外部RAM的磁盘数据缓冲区,缓冲区长度为一个扇区的长度 */
#endif

/* 以下程序可以根据需要修改 */

#ifndef NO_DEFAULT_ACCESS_SECTOR		/* 在应用程序中定义NO_DEFAULT_ACCESS_SECTOR可以禁止默认的磁盘扇区读写子程序,然后用自行编写的程序代替它 */

//if ( use_external_interface ) {  // 替换U盘扇区底层读写子程序
//    CH374vSectorSize=512;  // 设置实际的扇区大小,该值必须是512的倍数
//    CH56xvSectorSizeB=9;   // 设置实际的扇区大小的位移数,512则对应9,1024对应10,2048对应11
//    CH374DiskStatus=DISK_MOUNTED;  // 强制块设备连接成功(只差分析文件系统)
//}

#ifdef __UDISK_H__

extern BULK_ONLY_CMD	mBOC;

#else

typedef union _BULK_ONLY_CMD0 {
	struct {
		UINT32	mCBW_Sig;
		UINT32	mCBW_Tag;
		UINT32	mCBW_DataLen;			/* 输入: 数据传输长度 */
		UINT8	mCBW_Flag;				/* 输入: 传输方向等标志 */
		UINT8	mCBW_LUN;
		UINT8	mCBW_CB_Len;			/* 输入: 命令块的长度,有效值是1到16 */
		UINT8	mCBW_CB_Buf[16];		/* 输入: 命令块,该缓冲区最多为16个字节 */
	} mCBW;								/* BulkOnly协议的命令块, 输入CBW结构 */
	struct {
		UINT32	mCSW_Sig;
		UINT32	mCSW_Tag;
		UINT32	mCSW_Residue;			/* 返回: 剩余数据长度 */
		UINT8	mCSW_Status;			/* 返回: 命令执行结果状态 */
		UINT8	mReserved;
	} mCSW;								/* BulkOnly协议的命令状态块, 输出CSW结构 */
} BULK_ONLY_CMD0;						/* CMD_BulkOnlyCmd, 执行基于BulkOnly协议的命令, 如果有数据传输那么数据在pDISK_BASE_BUF中 */

extern BULK_ONLY_CMD0	mBOC;

#endif

extern UINT8 MS_ScsiCmd_Process( UINT8 *DataBuf );

UINT8	CH56xReadSector( UINT32 StartLba, UINT32 SectCount, PUINT8 DataBuf )  /* 从磁盘读取多个扇区的数据到缓冲区中 */
{
	UINT8	retry;
//	if ( use_external_interface ) return( extReadSector( StartLba, SectCount, DataBuf ) );  /* 外部接口 */
	for( retry = 0; retry < 3; retry ++ ) {  /* 错误重试 */
		mBOC.mCBW.mCBW_DataLen = SectCount << CH56xvSectorSizeB;
		mBOC.mCBW.mCBW_Flag = 0x80;
		mBOC.mCBW.mCBW_CB_Len = 10;
		mBOC.mCBW.mCBW_CB_Buf[ 0 ] = SPC_CMD_READ10;
		mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[ 2 ] = (UINT8)( StartLba >> 24 );
		mBOC.mCBW.mCBW_CB_Buf[ 3 ] = (UINT8)( StartLba >> 16 );
		mBOC.mCBW.mCBW_CB_Buf[ 4 ] = (UINT8)( StartLba >> 8 );
		mBOC.mCBW.mCBW_CB_Buf[ 5 ] = (UINT8)( StartLba );
		mBOC.mCBW.mCBW_CB_Buf[ 6 ] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[ 7 ] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[ 8 ] = SectCount;
		mBOC.mCBW.mCBW_CB_Buf[ 9 ] = 0x00;
		CH56xIntStatus = MS_ScsiCmd_Process( DataBuf );  /* 执行基于BulkOnly协议的命令 */
		if ( CH56xIntStatus == ERR_SUCCESS ) {
			return( CH56xIntStatus );
		}
		CH56xIntStatus = CH56xAnalyzeError( retry );
		if ( CH56xIntStatus != ERR_SUCCESS ) {
			return( CH56xIntStatus );
		}
	}
	return( CH56xIntStatus = ERR_USB_DISK_ERR );  /* 磁盘操作错误 */
}

#ifdef EN_DISK_WRITE

extern void Delay_us( int n );

UINT8	CH56xWriteSector( UINT32 StartLba, UINT32 SectCount, PUINT8 DataBuf )  /* 将缓冲区中的多个扇区的数据块写入磁盘 */
{
	UINT8	retry;
//	if ( use_external_interface ) return( extWriteSector( StartLba, SectCount, DataBuf ) );  /* 外部接口 */
	for( retry = 0; retry < 3; retry ++ ) {  /* 错误重试 */
		mBOC.mCBW.mCBW_DataLen = SectCount << CH56xvSectorSizeB;
		mBOC.mCBW.mCBW_Flag = 0x00;
		mBOC.mCBW.mCBW_CB_Len = 10;
		mBOC.mCBW.mCBW_CB_Buf[ 0 ] = SPC_CMD_WRITE10;
		mBOC.mCBW.mCBW_CB_Buf[ 1 ] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[ 2 ] = (UINT8)( StartLba >> 24 );
		mBOC.mCBW.mCBW_CB_Buf[ 3 ] = (UINT8)( StartLba >> 16 );
		mBOC.mCBW.mCBW_CB_Buf[ 4 ] = (UINT8)( StartLba >> 8 );
		mBOC.mCBW.mCBW_CB_Buf[ 5 ] = (UINT8)( StartLba );
		mBOC.mCBW.mCBW_CB_Buf[ 6 ] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[ 7 ] = 0x00;
		mBOC.mCBW.mCBW_CB_Buf[ 8 ] = SectCount;
		mBOC.mCBW.mCBW_CB_Buf[ 9 ] = 0x00;
		CH56xIntStatus = MS_ScsiCmd_Process( DataBuf );  /* 执行基于BulkOnly协议的命令 */
		if ( CH56xIntStatus == ERR_SUCCESS ) {
			Delay_us( 200 );  /* 写操作后延时 */
			return( CH56xIntStatus );
		}
		CH56xIntStatus = CH56xAnalyzeError( retry );
		if ( CH56xIntStatus != ERR_SUCCESS ) {
			return( CH56xIntStatus );
		}
	}
	return( CH56xIntStatus = ERR_USB_DISK_ERR );  /* 磁盘操作错误 */
}

#endif
#endif  // NO_DEFAULT_ACCESS_SECTOR

#ifndef NO_DEFAULT_FILE_ENUMER			/* 在应用程序中定义NO_DEFAULT_FILE_ENUMER可以禁止默认的文件名枚举回调程序,然后用自行编写的程序代替它 */
void xFileNameEnumer( void )			/* 文件名枚举回调子程序 */
{
/* 如果指定枚举序号CH56xvFileSize为0xFFFFFFFF后调用FileOpen，那么每搜索到一个文件FileOpen都会调用本回调程序，
   回调程序xFileNameEnumer返回后，FileOpen递减CH56xvFileSize并继续枚举直到搜索不到文件或者目录。建议做法是，
   在调用FileOpen之前定义一个全局变量为0，当FileOpen回调本程序后，本程序由CH56xvFdtOffset得到结构FAT_DIR_INFO，
   分析结构中的DIR_Attr以及DIR_Name判断是否为所需文件名或者目录名，记录相关信息，并将全局变量计数增量，
   当FileOpen返回后，判断返回值如果是ERR_MISS_FILE或ERR_FOUND_NAME都视为操作成功，全局变量为搜索到的有效文件数。
   如果在本回调程序xFileNameEnumer中将CH56xvFileSize置为1，那么可以通知FileOpen提前结束搜索。以下是回调程序例子 */
#if 0
	UINT8			i;
	UINT16			FileCount;
	P_FAT_DIR_INFO	pFileDir;
	PUINT8			NameBuf;
	pFileDir = (P_FAT_DIR_INFO)( pDISK_BASE_BUF + CH56xvFdtOffset );  /* 当前FDT的起始地址 */
	FileCount = (UINT16)( 0xFFFFFFFF - CH56xvFileSize );  /* 当前文件名的枚举序号,CH56xvFileSize初值是0xFFFFFFFF,找到文件名后递减 */
	if ( FileCount < sizeof( FILE_DATA_BUF ) / 12 ) {  /* 检查缓冲区是否足够存放,假定每个文件名需占用12个字节存放 */
		NameBuf = & FILE_DATA_BUF[ FileCount * 12 ];  /* 计算保存当前文件名的缓冲区地址 */
		for ( i = 0; i < 11; i ++ ) NameBuf[ i ] = pFileDir -> DIR_Name[ i ];  /* 复制文件名,长度为11个字符,未处理空格 */
		if ( pFileDir -> DIR_Attr & ATTR_DIRECTORY ) NameBuf[ i ] = 1;  /* 判断是目录名 */
		NameBuf[ i ] = 0;  /* 文件名结束符 */
	}
#endif
}
#endif  // NO_DEFAULT_FILE_ENUMER

UINT8	CH56xLibInit( void )  /* 初始化CH56x程序库和CH56x芯片,操作成功返回0 */
{
	if ( CH56xGetVer( ) < CH56x_LIB_VER ) return( 0xFF );  /* 获取当前子程序库的版本号,版本太低则返回错误 */
#if DISK_BASE_BUF_LEN > 0
	pDISK_BASE_BUF = & DISK_BASE_BUF[0];  /* 指向外部RAM的磁盘数据缓冲区 */
	pDISK_FAT_BUF = & DISK_BASE_BUF[0];  /* 指向外部RAM的磁盘FAT数据缓冲区,可以与pDISK_BASE_BUF合用以节约RAM */
/* 如果希望提高文件存取速度,那么可以在主程序中调用CH56xLibInit之后,将pDISK_FAT_BUF重新指向另一个独立分配的与pDISK_BASE_BUF同样大小的缓冲区 */
#endif
	CH56xDiskStatus = DISK_UNKNOWN;  /* 未知状态 */
	return( ERR_SUCCESS );
}
