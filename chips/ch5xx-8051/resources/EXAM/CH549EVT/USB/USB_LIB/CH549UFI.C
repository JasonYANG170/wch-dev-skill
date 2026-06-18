/* 2014.09.09
*****************************************
**   Copyright  (C)  W.ch  1999-2015   **
**   Web:      http://wch.cn           **
*****************************************
**  USB-flash File Interface for CH549 **
**  KC7.0@MCS51                        **
*****************************************
*/
/* CH549 U盘主机文件系统接口, 支持: FAT12/FAT16/FAT32 */

//#define DISK_BASE_BUF_LEN		512	/* 默认的磁盘数据缓冲区大小为512字节(可以选择为2048甚至4096以支持某些大扇区的U盘),为0则禁止在本文件中定义缓冲区并由应用程序在pDISK_BASE_BUF中指定 */
/* 如果需要复用磁盘数据缓冲区以节约RAM,那么可将DISK_BASE_BUF_LEN定义为0以禁止在本文件中定义缓冲区,而由应用程序在调用CH549LibInit之前将与其它程序合用的缓冲区起始地址置入pDISK_BASE_BUF变量 */

//#define NO_DEFAULT_ACCESS_SECTOR	1		/* 禁止默认的磁盘扇区读写子程序,下面用自行编写的程序代替它 */
//#define NO_DEFAULT_DISK_CONNECT		1		/* 禁止默认的检查磁盘连接子程序,下面用自行编写的程序代替它 */
//#define NO_DEFAULT_FILE_ENUMER		1		/* 禁止默认的文件名枚举回调程序,下面用自行编写的程序代替它 */

//#include "CH549.H"
#include "CH549UFI.H"

CMD_PARAM_I	mCmdParam;						/* 命令参数 */
#if		DISK_BASE_BUF_LEN > 0
UINT8X	DISK_BASE_BUF[ DISK_BASE_BUF_LEN ] _at_ 0x0080;	/* 外部RAM的磁盘数据缓冲区,缓冲区长度为一个扇区的长度 */
#endif

/* 以下程序可以根据需要修改 */

#ifndef	NO_DEFAULT_ACCESS_SECTOR		/* 在应用程序中定义NO_DEFAULT_ACCESS_SECTOR可以禁止默认的磁盘扇区读写子程序,然后用自行编写的程序代替它 */
//if ( use_external_interface ) {  // 替换U盘扇区底层读写子程序
//    CH549vSectorSizeH=512>>8;  // 设置实际的扇区大小,必须是512的倍数,该值是磁盘的扇区大小的高字节,低字节总是0x00,数值2代替0x200
//    CH549vSectorSizeB=9;   // 设置实际的扇区大小的位移数,512则对应9,1024对应10,2048对应11
//    CH549DiskStatus=DISK_MOUNTED;  // 强制块设备连接成功(只差分析文件系统)
//}

UINT8	CH549ReadSector( UINT8 SectCount, PUINT8X DataBuf )  /* 从磁盘读取多个扇区的数据到缓冲区中 */
{
	UINT8	retry;
//	if ( use_external_interface ) return( extReadSector( CH549vLbaCurrent, SectCount, DataBuf ) );  /* 外部接口 */
	for( retry = 0; retry < 3; retry ++ ) {  /* 错误重试 */
		pCBW -> mCBW_DataLen0 = 0;  /* 数据传输长度 */
		pCBW -> mCBW_DataLen1 = SectCount << CH549vSectorSizeB - 8;
		pCBW -> mCBW_DataLen2 = 0;
		pCBW -> mCBW_DataLen3 = 0;
		pCBW -> mCBW_Flag = 0x80;
		pCBW -> mCBW_LUN = CH549vCurrentLun;
		pCBW -> mCBW_CB_Len = 10;
		pCBW -> mCBW_CB_Buf[ 0 ] = SPC_CMD_READ10;
		pCBW -> mCBW_CB_Buf[ 1 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 2 ] = *(PUINT8)&CH549vLbaCurrent;
		pCBW -> mCBW_CB_Buf[ 3 ] = *( (PUINT8)&CH549vLbaCurrent + 1 );
		pCBW -> mCBW_CB_Buf[ 4 ] = *( (PUINT8)&CH549vLbaCurrent + 2 );
		pCBW -> mCBW_CB_Buf[ 5 ] = *( (PUINT8)&CH549vLbaCurrent + 3 );
		pCBW -> mCBW_CB_Buf[ 6 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 7 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 8 ] = SectCount;
		pCBW -> mCBW_CB_Buf[ 9 ] = 0x00;
		CH549BulkOnlyCmd( DataBuf );  /* 执行基于BulkOnly协议的命令 */
		if ( CH549IntStatus == ERR_SUCCESS ) {
			return( ERR_SUCCESS );
		}
		CH549IntStatus = CH549AnalyzeError( retry );
		if ( CH549IntStatus != ERR_SUCCESS ) {
			return( CH549IntStatus );
		}
	}
	return( CH549IntStatus = ERR_USB_DISK_ERR );  /* 磁盘操作错误 */
}

#ifdef	EN_DISK_WRITE
UINT8	CH549WriteSector( UINT8 SectCount, PUINT8X DataBuf )  /* 将缓冲区中的多个扇区的数据块写入磁盘 */
{
	UINT8	retry;
//	if ( use_external_interface ) return( extWriteSector( CH549vLbaCurrent, SectCount, DataBuf ) );  /* 外部接口 */
	for( retry = 0; retry < 3; retry ++ ) {  /* 错误重试 */
		pCBW -> mCBW_DataLen0 = 0;  /* 数据传输长度 */
		pCBW -> mCBW_DataLen1 = SectCount << CH549vSectorSizeB - 8;
		pCBW -> mCBW_DataLen2 = 0;
		pCBW -> mCBW_DataLen3 = 0;
		pCBW -> mCBW_Flag = 0x00;
		pCBW -> mCBW_LUN = CH549vCurrentLun;
		pCBW -> mCBW_CB_Len = 10;
		pCBW -> mCBW_CB_Buf[ 0 ] = SPC_CMD_WRITE10;
		pCBW -> mCBW_CB_Buf[ 1 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 2 ] = *(PUINT8)&CH549vLbaCurrent;
		pCBW -> mCBW_CB_Buf[ 3 ] = *( (PUINT8)&CH549vLbaCurrent + 1 );
		pCBW -> mCBW_CB_Buf[ 4 ] = *( (PUINT8)&CH549vLbaCurrent + 2 );
		pCBW -> mCBW_CB_Buf[ 5 ] = *( (PUINT8)&CH549vLbaCurrent + 3 );
		pCBW -> mCBW_CB_Buf[ 6 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 7 ] = 0x00;
		pCBW -> mCBW_CB_Buf[ 8 ] = SectCount;
		pCBW -> mCBW_CB_Buf[ 9 ] = 0x00;
		CH549BulkOnlyCmd( DataBuf );  /* 执行基于BulkOnly协议的命令 */
		if ( CH549IntStatus == ERR_SUCCESS ) {
			mDelayuS( 200 );  /* 写操作后延时 */
			return( ERR_SUCCESS );
		}
		CH549IntStatus = CH549AnalyzeError( retry );
		if ( CH549IntStatus != ERR_SUCCESS ) {
			return( CH549IntStatus );
		}
	}
	return( CH549IntStatus = ERR_USB_DISK_ERR );  /* 磁盘操作错误 */
}
#endif
#endif  // NO_DEFAULT_ACCESS_SECTOR

#ifndef	NO_DEFAULT_DISK_CONNECT			/* 在应用程序中定义NO_DEFAULT_DISK_CONNECT可以禁止默认的检查磁盘连接子程序,然后用自行编写的程序代替它 */
/* 检查磁盘是否连接 */
UINT8	CH549DiskConnect( void )
{
	USB_DEV_AD &= 0x7F;
	if ( USB_DEV_AD == USB_DEVICE_ADDR ) {  /* 根下USB设备 */
		if ( UHOST_CTRL & bUH_PORT_EN ) {  /* USB设备存在且未插拔 */
			return( ERR_SUCCESS );  /* USB设备已经连接且未插拔 */
		}
		else if ( USB_MIS_ST & bUMS_DEV_ATTACH ) {  /* USB设备存在 */
mDiskConnect:
			CH549DiskStatus = DISK_CONNECT;  /* 曾经断开过 */
			return( ERR_SUCCESS );  /* 外部HUB或USB设备已经连接或者断开后重新连接 */
		}
		else {  /* USB设备断开 */
mDiskDisconn:
			CH549DiskStatus = DISK_DISCONNECT;
			return( ERR_USB_DISCON );
		}
	}
#ifndef	FOR_ROOT_UDISK_ONLY
	else if ( USB_DEV_AD > 0x10 && USB_DEV_AD <= 0x14 ) {  /* 外部HUB的端口下的USB设备 */
		if ( UHOST_CTRL & bUH_PORT_EN ) {  /* 外部HUB存在且未插拔 */
			TxBuffer[ MAX_PACKET_SIZE - 1 ] = USB_DEV_AD;  /* 备份 */
			USB_DEV_AD = USB_DEVICE_ADDR - 1 + ( USB_DEV_AD >> 4 );  /* 设置USB主机端的USB地址指向HUB */
			CH549IntStatus = HubGetPortStatus( TxBuffer[ MAX_PACKET_SIZE - 1 ] & 0x0F );  /* 查询HUB端口状态,返回在TxBuffer中 */
			if ( CH549IntStatus == ERR_SUCCESS ) {
				if ( TxBuffer[2] & (1<<(HUB_C_PORT_CONNECTION-0x10)) ) {  /* 检测到HUB端口上的插拔事件 */
					CH549DiskStatus = DISK_DISCONNECT;  /* 假定为HUB端口上的USB设备断开 */
					HubClearPortFeature( TxBuffer[ MAX_PACKET_SIZE - 1 ] & 0x0F, HUB_C_PORT_CONNECTION );  /* 清除HUB端口连接事件状态 */
				}
				USB_DEV_AD = TxBuffer[ MAX_PACKET_SIZE - 1 ];  /* 设置USB主机端的USB地址指向USB设备 */
				if ( TxBuffer[0] & (1<<HUB_PORT_CONNECTION) ) {  /* 连接状态 */
					if ( CH549DiskStatus < DISK_CONNECT ) {
						CH549DiskStatus = DISK_CONNECT;  /* 曾经断开过 */
					}
					return( ERR_SUCCESS );  /* USB设备已经连接或者断开后重新连接 */
				}
				else {
//					CH549DiskStatus = DISK_DISCONNECT;
//					return( ERR_USB_DISCON );
					CH549DiskStatus = DISK_CONNECT;
					return( ERR_HUB_PORT_FREE );  /* HUB已经连接但是HUB端口尚未连接磁盘 */
				}
			}
			else {
				USB_DEV_AD = TxBuffer[ MAX_PACKET_SIZE - 1 ];  /* 设置USB主机端的USB地址指向USB设备 */
				if ( CH549IntStatus == ERR_USB_DISCON ) {
//					CH549DiskStatus = DISK_DISCONNECT;
//					return( ERR_USB_DISCON );
					goto mDiskDisconn;
				}
				else {
					CH549DiskStatus = DISK_CONNECT;  /* HUB操作失败 */
					return( CH549IntStatus );
				}
			}
		}
		else if ( USB_MIS_ST & bUMS_DEV_ATTACH ) {  /* USB设备存在,外部HUB或USB设备已经连接或者断开后重新连接 */
//			CH549DiskStatus = DISK_CONNECT;  /* 曾经断开过 */
//			return( ERR_SUCCESS );  /* 外部HUB或USB设备已经连接或者断开后重新连接 */
			goto mDiskConnect;
		}
		else {  /* 外部HUB断开 */
			CH549DiskStatus = DISK_DISCONNECT;
		}
	}
#endif
	else {
//		CH549DiskStatus = DISK_DISCONNECT;
//		return( ERR_USB_DISCON );
		goto mDiskDisconn;
	}
}
#endif  // NO_DEFAULT_DISK_CONNECT

#ifndef	NO_DEFAULT_FILE_ENUMER			/* 在应用程序中定义NO_DEFAULT_FILE_ENUMER可以禁止默认的文件名枚举回调程序,然后用自行编写的程序代替它 */
void xFileNameEnumer( void )			/* 文件名枚举回调子程序 */
{
/* 如果指定枚举序号CH549vFileSize为0xFFFFFFFF后调用FileOpen，那么每搜索到一个文件FileOpen都会调用本回调程序，
   回调程序xFileNameEnumer返回后，FileOpen递减CH549vFileSize并继续枚举直到搜索不到文件或者目录。建议做法是，
   在调用FileOpen之前定义一个全局变量为0，当FileOpen回调本程序后，本程序由CH549vFdtOffset得到结构FAT_DIR_INFO，
   分析结构中的DIR_Attr以及DIR_Name判断是否为所需文件名或者目录名，记录相关信息，并将全局变量计数增量，
   当FileOpen返回后，判断返回值如果是ERR_MISS_FILE或ERR_FOUND_NAME都视为操作成功，全局变量为搜索到的有效文件数。
   如果在本回调程序xFileNameEnumer中将CH549vFileSize置为1，那么可以通知FileOpen提前结束搜索。以下是回调程序例子 */
#if		0
	UINT8			i;
	UINT16			FileCount;
	PX_FAT_DIR_INFO	pFileDir;
	PUINT8			NameBuf;
	pFileDir = (PX_FAT_DIR_INFO)( pDISK_BASE_BUF + CH549vFdtOffset );  /* 当前FDT的起始地址 */
	FileCount = (UINT16)( 0xFFFFFFFF - CH549vFileSize );  /* 当前文件名的枚举序号,CH549vFileSize初值是0xFFFFFFFF,找到文件名后递减 */
	if ( FileCount < sizeof( FILE_DATA_BUF ) / 12 ) {  /* 检查缓冲区是否足够存放,假定每个文件名需占用12个字节存放 */
		NameBuf = & FILE_DATA_BUF[ FileCount * 12 ];  /* 计算保存当前文件名的缓冲区地址 */
		for ( i = 0; i < 11; i ++ ) NameBuf[ i ] = pFileDir -> DIR_Name[ i ];  /* 复制文件名,长度为11个字符,未处理空格 */
//		if ( pFileDir -> DIR_Attr & ATTR_DIRECTORY ) NameBuf[ i ] = 1;  /* 判断是目录名 */
		NameBuf[ i ] = 0;  /* 文件名结束符 */
	}
#endif
}
#endif  // NO_DEFAULT_FILE_ENUMER

UINT8	CH549LibInit( void )  /* 初始化CH549程序库,操作成功返回0 */
{
	if ( CH549GetVer( ) < CH549_LIB_VER ) return( 0xFF );  /* 获取当前子程序库的版本号,版本太低则返回错误 */
#if		DISK_BASE_BUF_LEN > 0
	pDISK_BASE_BUF = & DISK_BASE_BUF[0];  /* 指向外部RAM的磁盘数据缓冲区 */
	pDISK_FAT_BUF = & DISK_BASE_BUF[0];  /* 指向外部RAM的磁盘FAT数据缓冲区,可以与pDISK_BASE_BUF合用以节约RAM */
/* 如果希望提高文件存取速度,那么可以在主程序中调用CH549LibInit之后,将pDISK_FAT_BUF重新指向另一个独立分配的与pDISK_BASE_BUF同样大小的缓冲区 */
#endif
	CH549DiskStatus = DISK_UNKNOWN;  /* 未知状态 */
	CH549vSectorSizeB = 9;  /* 默认的物理磁盘的扇区是512B */
	CH549vSectorSizeH = 512 >> 8;  // 默认的物理磁盘的扇区是512B,该值是磁盘的扇区大小的高字节,低字节总是0x00,数值2代替0x200
	CH549vStartLba = 0;  /* 默认为自动分析FDD和HDD */
	return( ERR_SUCCESS );
}
