/*
Module Name:Main.cpp
*****************************************
**  Copyright  (C)  WCH  2001-2015     **
**  Web:  http://wch.cn                **
*****************************************
Environment:
    user mode only,VC6.0
Revision History:
    9/10/2015: TECH
Descriptor:
    WCH MCU IAP操作代码，支持CH56X，CH55X芯片
*/

#include "resource.h"
#include "Main.h"
#include "IAP.H"

#define MAX_FILE_SIZE 60*1024 //下载文件最大长度

extern BOOL IsDownloading,IsDeviceChanged;
extern ULONG AfxDnInterface;
extern DnDevInforS AfxDnDev[]; //设备列表
extern HANDLE AfxDnDnDevH;
extern HWND AfxMainHwnd;

//HEX格式文件转换成BIN格式
BOOL HexToBin( PVOID Hexbuf,     //Hexbuf为HEX文件缓冲区
			  ULONG iHexBufLen,  //iHexBufLen为HEX缓冲区长度
			  PVOID Binbuf,      //Binbuf为BIN文件缓冲区
			  PULONG iBinLen );   //iBinLen为转换后BIN的有效长度
//通过CMD_IAP_PROM(ISP编程命令)将数据写入FLASH内
BOOL WCHMcuIAP_PROM(HANDLE DnDevH,PUCHAR DnDataBuf,ULONG DnDataLen);
//CMD_IAP_VERIFY(ISP校验命令)
BOOL WCHMcuIAP_VERIFY(HANDLE DnDevH,PUCHAR DnDataBuf,ULONG DnDataLen);
//CMD_IAP_END(ISP结束标志):
BOOL WCHMcuIAP_END(HANDLE DnDevH);
//CMD_IAP_ERASE和通过CMD_IAP_END请求操作函数，两个命令处理方法相同
BOOL WCHMcuIAP_ERASE(HANDLE DnDevH,
					USHORT StartAddr); //地址必须是1024的位数
//关闭串口下载接口
VOID DnInterface_Close(HANDLE DnDevH);
//打开串口下载接口
HANDLE DnInterface_Open();
//从下载文件中取出全部数据至缓冲区内.若是hex文件要转换成bin文件;下载数据以4K对齐，不满4096的部分填写0XFF
BOOL ReadDataFromDnFile(PUCHAR DnDataBuf, //下载文件内容缓冲区
						ULONG *DnDataLen);  //下载文件内容字节数
//CMD_IAP_VERIFY和通过CMD_IAP_PROM请求操作函数，两个命令处理方法相同
BOOL WCHMcuIAP_PROM_VERIFY(HANDLE DnDevH,
						  UCHAR CmdCode,
						  USHORT StartAddr,
						  PUCHAR DnDataBuf,
						  ULONG DnDataLen);
//发送IAP命令数据
BOOL WCHMcuIAP_SendCmd(HANDLE DnDevH,PUCHAR CmdPktBuf,PULONG oLen);
//接收IAP命令返回
BOOL WCHMcuIAP_RecvCmdRet(HANDLE DnDevH,PUCHAR CmdPktBuf,PULONG iLen);



//通过USB接口和MCU的IAP固件，下载IMCU用户程序
DWORD WINAPI IAPFlashDownloadThread(LPVOID lpParameter)
{
	HANDLE DnDevH;
	UCHAR* buffer = NULL;												//数据缓冲区指针
	BOOL RetVal = FALSE;
	PUCHAR DnDataBuf = NULL; //下载文件内容缓冲区
	ULONG DnDataLen = 0;      //下载文件内容字节数

	EnableWindow(GetDlgItem(AfxMainHwnd,IDC_Download),FALSE); //防止重复点击下载
	IsDownloading = TRUE;

	if( AfxDnInterface == 0 ) //USB接口下载
		DbgPrint("\r\n****开始通过USB接口进行IAP下载:");
	else
		DbgPrint("\r\n****开始通过串口接口进行IAP下载:");

	//1.读取下载文件内的数据至缓冲区内,如是HEX文件，将转成BIN文件
	DbgPrint("1.读取下载文件内容>>");
	DnDataLen = MAX_FILE_SIZE; //最大长度60K
	DnDataBuf = (PUCHAR)malloc(DnDataLen);
	ZeroMemory(DnDataBuf,DnDataLen);
    if( !ReadDataFromDnFile(DnDataBuf,    //下载文件内容缓冲区
		&DnDataLen) )   //下载文件内容字节数	
	{
		DbgPrint("  读下载文件失败");
		goto ExitDownload;
	}
	else
		DbgPrint("  已读取下载数据%d字节",DnDataLen);
	
	//2.打开设备下载接口	
	DbgPrint("2.打开下载接口>>");;
	DnDevH = DnInterface_Open();
	if( DnDevH == INVALID_HANDLE_VALUE )
		goto ExitDownload;
			
	//第1步：发送擦除命令	
	DbgPrint("3.擦除FLASH>>");
	if( !WCHMcuIAP_ERASE(DnDevH,0) )
	{
		//isTrue = FALSE;
		DbgPrint("  擦除错误.");
		goto ExitDownload;
	}
	else
		DbgPrint("  OK");
	//第2步：发送编程命令，并进行编程	
	DbgPrint("4.写FLASH>>");
	if( !WCHMcuIAP_PROM(DnDevH,DnDataBuf,DnDataLen) )
	{
		//isTrue = FALSE;
		DbgPrint("  失败.");
		goto ExitDownload;
	}
	else
		DbgPrint("  成功");
	//第3步：发送校验命令
	//校验命令的填充和发送
	DbgPrint("5.下载检验>>");
	if (!WCHMcuIAP_VERIFY(DnDevH,DnDataBuf,DnDataLen))
		
	{
		DbgPrint("  出错.");
		goto ExitDownload;
	}
	DbgPrint("  成功");	
	RetVal = TRUE;
	
ExitDownload:
	DbgPrint("6.下载结束>>");
	if( WCHMcuIAP_END(DnDevH) )
		DbgPrint("  设置成功");
	else
		DbgPrint("  设置失败");

	DbgPrint("7.关闭下载接口>>");
	DnInterface_Close(DnDevH);
	DnDevH = INVALID_HANDLE_VALUE;

	if( RetVal )
		DbgPrint("****IAP下载成功.");
	else
		DbgPrint("****IAP下载失败!\r\n   1.检查通讯接口以及连线是否正确;\r\n    2.MCU内有没有烧入IAP程序;\r\n    3.MCU有没有进入到IAP下载模式;");	

	if( DnDataBuf != NULL )
	{		
		free(DnDataBuf);
		DnDataBuf = NULL;
	}

	EnableWindow(GetDlgItem(AfxMainHwnd,IDC_Download),TRUE); //恢复下载按钮状态
	IsDownloading = FALSE;
	if( IsDeviceChanged ) //下载期间有设备插拔
	{
		IsDeviceChanged = FALSE;
		PostMessage(GetDlgItem(AfxMainHwnd,IDC_ScanDev),BM_CLICK,0,0); //刷新设备列表
	}
	return 0;	
}


//通过CMD_IAP_PROM(ISP编程命令)将数据写入FLASH内
BOOL WCHMcuIAP_PROM(HANDLE DnDevH,PUCHAR DnDataBuf,ULONG DnDataLen)
{
	return WCHMcuIAP_PROM_VERIFY(DnDevH,CMD_IAP_PROM,0,DnDataBuf,DnDataLen);
}

//CMD_IAP_VERIFY(ISP校验命令)
BOOL WCHMcuIAP_VERIFY(HANDLE DnDevH,PUCHAR DnDataBuf,ULONG DnDataLen)
{
	return WCHMcuIAP_PROM_VERIFY(DnDevH,CMD_IAP_VERIFY,0,DnDataBuf,DnDataLen);
}

//CMD_IAP_ERASE和通过CMD_IAP_END请求操作函数，两个命令处理方法相同
BOOL WCHMcuIAP_ERASE(HANDLE DnDevH,
					USHORT StartAddr) //地址必须是1024的位数
{
	USHORT FlashAddr;
	IAPCMDS IAPCmd = {0};
	ULONG  IAPCmdLen;
	UCHAR CmdDataLen;
	BOOL RetVal = FALSE;
	
	FlashAddr = StartAddr;	//每次下传时的缓冲区的偏移地址,从0x00000000开始
	CmdDataLen = sizeof(FlashAddr); //无后续数据
	
	//填充命令包
	IAPCmd.program.cmd = CMD_IAP_ERASE;          //命令码
	IAPCmd.program.len = CmdDataLen;       //无后续数据
	*((PUSHORT)&IAPCmd.program.addr[0]) = FlashAddr; //写FLASH的首地址
	
	IAPCmdLen = CmdDataLen + 2; //本次下传数据的总长度		
	
	//发送IAP命令包
	if( !WCHMcuIAP_SendCmd(DnDevH,(PUCHAR)&IAPCmd,&IAPCmdLen) )
	{
		DbgPrint("  WCHMcuIAP_ERASE send data failure.[%X>%dB].",FlashAddr,CmdDataLen);
		goto Exit_WCHMcuIAP_ERASE;
	}
	//接受操作结果2字节:
	IAPCmdLen = 2;
	memset(&IAPCmd,0xFF,5);
	if( !WCHMcuIAP_RecvCmdRet(DnDevH,(PUCHAR)&IAPCmd,&IAPCmdLen) )
	{
		DbgPrint("  WCHMcuIAP_ERASE read status failure.[%X>%dB].",FlashAddr,CmdDataLen);
		goto Exit_WCHMcuIAP_ERASE;
	}
	if( (IAPCmd.other.buf[0] != 0x00) || (IAPCmd.other.buf[1] != 0x00) )//状态显示错误
	{
		DbgPrint("  WCHMcuIAP_ERASE failure.[%X.%X].",IAPCmd.other.buf[0],IAPCmd.other.buf[1]);
		goto Exit_WCHMcuIAP_ERASE;
	}	
	else
		RetVal = TRUE;

Exit_WCHMcuIAP_ERASE:
	return RetVal;
}

//CMD_IAP_END(ISP结束标志):
BOOL WCHMcuIAP_END(HANDLE DnDevH)
{
	IAPCMDS IAPCmd = {0};
	ULONG  IAPCmdLen;
	UCHAR CmdDataLen;
	BOOL RetVal = FALSE;
	
	CmdDataLen = sizeof(IAPCmd.end.status); //无后续数据
	
	//填充命令包
	IAPCmd.end.cmd = CMD_IAP_END;    //命令码
	IAPCmd.end.len = CmdDataLen;     //后续数据为status数据
	IAPCmd.end.status[0] = IAPCmd.end.status[1] = 0;
	
	IAPCmdLen = 1 + 1+ CmdDataLen; //命令码+长度+后续数据status
	
	//发送IAP命令包
	if( !WCHMcuIAP_SendCmd(DnDevH,(PUCHAR)&IAPCmd,&IAPCmdLen) )
	{
		DbgPrint("  WCHMcuIAP_END send data failure");
		goto Exit_WCHMcuIAP_END;
	}
	else 
		RetVal = TRUE;
	//无返回数据	

Exit_WCHMcuIAP_END:
	return RetVal;
}


//CMD_IAP_VERIFY和通过CMD_IAP_PROM请求操作函数，两个命令处理方法相同
BOOL WCHMcuIAP_PROM_VERIFY(HANDLE DnDevH,
						  UCHAR CmdCode,
						  USHORT StartAddr,
						  PUCHAR DnDataBuf,
						  ULONG DnDataLen)
{
	USHORT FlashAddr;

	ULONG RemainLen = DnDataLen;
	IAPCMDS IAPCmd = {0};
	ULONG  IAPCmdLen;
	UCHAR CmdDataLen;
	
	FlashAddr = StartAddr;	//每次下传时的缓冲区的偏移地址,从0x00000000开始
	RemainLen = DnDataLen;
	
	while (RemainLen > 0)
	{		
		if (RemainLen > mMAX_CMDBUF_LENGTH )
			CmdDataLen = mMAX_CMDBUF_LENGTH;  //满包
		else
			CmdDataLen = (UCHAR)RemainLen;           //零头包
		//填充命令包
		IAPCmd.program.cmd = CmdCode;          //命令码
		IAPCmd.program.len = CmdDataLen;       //写入FLASH的字节数
		*((PUSHORT)&IAPCmd.program.addr[0]) = FlashAddr; //写FLASH的首地址			
		memcpy(IAPCmd.program.buf,&DnDataBuf[FlashAddr],CmdDataLen);	//写入FLASH的数据

		IAPCmdLen = CmdDataLen + 4; //本次下传数据的总长度		

		//发送IAP命令包
		if( !WCHMcuIAP_SendCmd(DnDevH,(PUCHAR)&IAPCmd,&IAPCmdLen) )
		{			
			DbgPrint("    发送命令包失败.[%X>%dB].",FlashAddr,CmdDataLen);
			break;
		}
		//接受操作结果2字节:
		IAPCmdLen = 2;
		memset(&IAPCmd,0xFF,5);
		if( !WCHMcuIAP_RecvCmdRet(DnDevH,(PUCHAR)&IAPCmd,&IAPCmdLen) )
		{
			DbgPrint("  Read status failure.[%X>%dB].",FlashAddr,CmdDataLen);
			break;
		}
		if( (IAPCmd.other.buf[0] != 0x00) || (IAPCmd.other.buf[1] != 0x00) )//状态显示错误
		{
			DbgPrint("  状态码返回错误.[%X.%X].",IAPCmd.other.buf[0],IAPCmd.other.buf[1]);
			break;
		}

		RemainLen -= CmdDataLen;                   //剩余数据长度
		FlashAddr += CmdDataLen;                   //下次写FLASH时的首地址
	}	
	return (RemainLen == 0);
}


//打开串口下载接口
HANDLE DnInterface_Open()
{
	ULONG InterfSel;
	BOOL RetVal = FALSE;
	HANDLE DnDnDevH = INVALID_HANDLE_VALUE;

	InterfSel = SendDlgItemMessage(AfxMainHwnd,IDC_DeviceList,CB_GETCURSEL,0,0);
	if( InterfSel == CB_ERR )
	{
		DbgPrint("    没有选择下载设备");
		return DnDnDevH;
	}	
	if( AfxDnInterface == 0 )//USB接口
	{
		//DbgPrint("打开USB下载接口:");

		DnDnDevH = CreateFile( AfxDnDev[InterfSel].DevName, GENERIC_READ | GENERIC_WRITE,  // 打开设备,不支持重叠操作
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );  // 共享
		
		if (INVALID_HANDLE_VALUE == DnDnDevH)
		{
			DbgPrint("    %s打开失败",AfxDnDev[InterfSel].DevName);
			return DnDnDevH;
		}
		else
		{
			DbgPrint("    %s打开成功",AfxDnDev[InterfSel].DevName);
		}

		CH375SetExclusive((ULONG)DnDnDevH,1);
		CH375SetTimeout((ULONG)DnDnDevH,1000,1000);		
	}
	else //串口
	{
		CHAR portname[64] = {0};
		DCB dcb;
		COMMTIMEOUTS TimeOuts;
		
		sprintf(portname, "\\\\.\\%s",AfxDnDev[InterfSel].DevName);		
		
		//打开串口
		DnDnDevH = CreateFile(portname,GENERIC_READ|GENERIC_WRITE,
			0,NULL,OPEN_EXISTING,
			NULL,
			NULL);
		if (INVALID_HANDLE_VALUE == DnDnDevH)
		{
			//串口打开失败
			//DbgPrint("            失败.");
			DbgPrint("    %s打开失败",AfxDnDev[InterfSel].DevName);
			ShowLastError("打开串口:");
			return DnDnDevH;
		}
		else
		{
			DbgPrint("    %s打开成功",AfxDnDev[InterfSel].DevName);
		}
		SetupComm(DnDnDevH,8192,8192);
		GetCommTimeouts(DnDnDevH,&TimeOuts);
		//设定超时
		TimeOuts.ReadIntervalTimeout = 0;
		TimeOuts.ReadTotalTimeoutMultiplier=1000;
		TimeOuts.ReadTotalTimeoutConstant=2000;
		TimeOuts.WriteTotalTimeoutMultiplier=50;
		TimeOuts.WriteTotalTimeoutConstant=500;
		SetCommTimeouts(DnDnDevH,&TimeOuts);//设置超时
		PurgeComm(DnDnDevH,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);   //清空输入、输出缓冲区
		
		GetCommState(DnDnDevH,&dcb);
		GetCommState(DnDnDevH,&dcb);
		dcb.BaudRate = IAP_UART_BAUDRATE;
		dcb.ByteSize = 8;					//设置数据位
		dcb.Parity = NOPARITY;				//设置校验位
		dcb.StopBits = ONESTOPBIT;			//设置停止位		
		SetCommState(DnDnDevH,&dcb);	
	}
	return DnDnDevH;
}

//关闭串口下载接口
VOID DnInterface_Close(HANDLE DnDevH)
{
	//DbgPrint("关闭下载接口:");
	if( DnDevH != INVALID_HANDLE_VALUE )
	{
		if( AfxDnInterface == 0 ) //USB接口下载
		{
		}
		else
		{
		}
		CloseHandle(DnDevH);
		DnDevH = INVALID_HANDLE_VALUE;		
		DbgPrint("    已关闭。");
	}
	else
		DbgPrint("    下载接口未打开，无需关闭");
}

//接收IAP命令返回
BOOL WCHMcuIAP_RecvCmdRet(HANDLE DnDevH,PUCHAR CmdPktBuf,PULONG iLen)
{
	ULONG DataLen;
	BOOL RetVal = FALSE;

	DataLen = *iLen;
	if( AfxDnInterface == 0 ) //USB下载
	{
		if( !CH375ReadData( (ULONG)DnDevH,CmdPktBuf,&DataLen) )
		{
			*iLen = 0;
			DbgPrint("    USB读失败.");
		}
		else
		{
			if( DataLen == *iLen )
				RetVal = TRUE;
			else
				DbgPrint("    USB读操作返回长度错误(%d-%d).",DataLen,*iLen);
			*iLen = DataLen;
		}
	}
	else //串口下载
	{
		if( !ReadFile(DnDevH,CmdPktBuf,DataLen,&DataLen,NULL) )
		{
			*iLen = 0;
			DbgPrint("    串口读失败.");
		}
		else
		{
			if( DataLen == *iLen )
				RetVal = TRUE;	
			else
				DbgPrint("    串口读操作返回长度错误(%d-%d).",DataLen,*iLen);

			*iLen = DataLen;
		}		
	}
	return RetVal;
}

//发送IAP命令数据
BOOL WCHMcuIAP_SendCmd(HANDLE DnDevH,PUCHAR CmdPktBuf,PULONG oLen)
{
	BOOL RetVal = FALSE;
	ULONG DataLen = *oLen;

	if( AfxDnInterface == 0 ) //USB下载
	{
		if( !CH375WriteData((ULONG)DnDevH,CmdPktBuf,&DataLen) )
		{		
			*oLen = 0;
			DbgPrint("    USB写数据失败");

		}
		else
		{
			if( DataLen != *oLen )
			{
				DbgPrint("    USB写超时.(%d-%d) ",DataLen,*oLen);				
			}
			else
				RetVal = TRUE;
			*oLen = DataLen;
		}
	}
	else //串口下载
	{//因串口较USB无数据校验功能，给串口数据包增加3个字节：0x57+0xAB+数据+检验和。
		PUCHAR NewCmdBuf;
		UCHAR CheckSum = 0;
		ULONG i;

		PurgeComm(DnDevH,PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);   //清空输入、输出缓冲区

		DataLen += 3;
		NewCmdBuf = (PUCHAR)malloc(DataLen);
		ZeroMemory(NewCmdBuf,DataLen);
		for(i=0;i<*oLen;i++)
		{
			CheckSum += CmdPktBuf[i];
		}
		NewCmdBuf[0] = Uart_Sync_Head1; //串口前导码1
		NewCmdBuf[1] = Uart_Sync_Head2; //串口前导码2
		memcpy(&NewCmdBuf[2],CmdPktBuf,*oLen);
		NewCmdBuf[2+*oLen] = CheckSum; //数据校验和
		
		//通过串口写数据
		if( !WriteFile(DnDevH,NewCmdBuf,DataLen,&DataLen,NULL) ) //写失败
		{
			ShowLastError("WriteFile");
			DbgPrint("    串口写失败");
			*oLen = 0;
		}
		else
		{
			if( DataLen == (*oLen+3) ) //写成功
				RetVal = TRUE;
			else
			{
				DbgPrint("    串口写超时(%d-%d)",DataLen,*oLen);
			}
			*oLen = DataLen;
		}
		free(NewCmdBuf);
	}
	return RetVal;
}

//从下载文件中取出全部数据至缓冲区内.若是hex文件要转换成bin文件;下载数据以4K对齐，不满4096的部分填写0XFF
BOOL ReadDataFromDnFile(PUCHAR DnDataBuf, //下载文件内容缓冲区
						ULONG *DnDataLen)  //下载文件内容字节数
{
	HANDLE FileH;
	ULONG FileSize;
	CHAR FileName[MAX_PATH] = "";
	PUCHAR FileBuf = NULL;
	BOOL RetVal = FALSE;
	
	GetDlgItemText(AfxMainHwnd,IDC_DownloadFile,FileName,sizeof(FileName)); //下载文件名
	FileH = CreateFile(FileName,GENERIC_READ,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_ARCHIVE,NULL);
	if(FileH==INVALID_HANDLE_VALUE)
	{
		DbgPrint("    无法打开下载文件%s",FileName);
		goto ExitReadDataFromDnFile;
	}
	FileSize = GetFileSize(FileH,NULL);
	if( FileSize == 0 ) //文件为空
	{
		DbgPrint("    空白下载文件%s",FileName);
		goto ExitReadDataFromDnFile;
	}
	
	FileBuf = (PUCHAR)malloc(FileSize);		
	ZeroMemory(FileBuf,FileSize);

	if( !ReadFile(FileH,FileBuf,FileSize,&FileSize,NULL) )
	{
		ShowLastError("    读下载文件 ");
		goto ExitReadDataFromDnFile;
	}
	
	CharUpper(FileName); //文件名全部大写
	if( (strstr(FileName,".HEX") != NULL) ) //是HEX格式下载文件,要转换成BIN文件
	{
		DbgPrint("    HEX格式转换成BIN格式.");
		if (!HexToBin(FileBuf, FileSize, DnDataBuf, &FileSize))
		{
			DbgPrint("    无效的HEX文件.");
			goto ExitReadDataFromDnFile;
		}
		if (FileSize > *DnDataLen)
		{
			DbgPrint("    下载文件不能超过%d字节",MAX_FILE_SIZE);
			goto ExitReadDataFromDnFile;
		}
		else
			*DnDataLen = FileSize;
	}
	else //BIN文件
	{
		if (FileSize > MAX_FILE_SIZE)
		{
			DbgPrint("    下载文件不能超过%d字节",MAX_FILE_SIZE);
			goto ExitReadDataFromDnFile;
		}
		
		memcpy(DnDataBuf,FileBuf,FileSize);  //复制下载数据
		*DnDataLen = FileSize;
	}	
		
	if( (*DnDataLen %4) ) //下载数据以4对齐，不足部分补0
	{	
		ULONG FillLen;

		FillLen = 4-(*DnDataLen%4);		
		memset(&DnDataBuf[*DnDataLen],0,FillLen);
		*DnDataLen += FillLen; //整理后的长度
	}
	RetVal = TRUE;

ExitReadDataFromDnFile:
	if( FileH != INVALID_HANDLE_VALUE )
		CloseHandle(FileH);	
	if( FileBuf )
		free(FileBuf);

	return RetVal;
}

//把两个字符转成一个字节的十六进制数值
BOOL Char2ToHex(PCHAR  pInChar,  //待转换的2个字符缓冲区
			   PUCHAR pOutChar) //转换后的十六进制数值
{
	UCHAR h,l;	

	h = pInChar[0]; //高4位
	l = pInChar[1]; //低4位
	if(l>='0' && l<='9')
	{
		l = l - '0';	
	}
	else if(l>='a' && l<='f')
	{
		l = l - 'a' + 0xa;
	}
	else if(l>='A' && l<='F')
	{
		l = l - 'A' + 0xa;
	}
	else
	{	
		return FALSE;
	}	
	if(h>='0' && h<='9')
	{
		h = h - '0';
	}
	else if(h>='a' && h<='f')
	{
		h = h - 'a' + 0xa;
	}
	else if(h>='A' &&h <='F')
	{
		h = h - 'A' + 0xa;
	}
	else
	{
		return FALSE;
	}	
	h <<= 4;
	h |= l;
	*pOutChar = h;
	return TRUE;
}

//HEX格式文件转换成BIN格式
BOOL HexToBin( PVOID Hexbuf,     //Hexbuf为HEX文件缓冲区
			  ULONG iHexBufLen,  //iHexBufLen为HEX缓冲区长度
			  PVOID Binbuf,      //Binbuf为BIN文件缓冲区
			  PULONG iBinLen )   //iBinLen为转换后BIN的有效长度
{
	PUCHAR hp;              // 暂存Hexbuf指针
	ULONG StartPos;         // 记录节开始位置
	UCHAR DataBuf[256];     // 临时数据存放缓冲
	USHORT ExtAdr;          // 扩展地址
	USHORT SegAdr;          // 扩展段地址
	USHORT OfsAdr;          // 偏移地址
	ULONG WriteAdr;         // 写入地址

	__try
	{						// 冲区置0XFF
		memset( Binbuf, 0xff, iHexBufLen);
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return FALSE;   // 访问发生异常,转换失败
	}
	// 检查HEX文件缓冲区是否可读
	if( IsBadReadPtr( Hexbuf, iHexBufLen ) )
		return FALSE;
	
	StartPos = 0;
	ExtAdr = 0;
	SegAdr = 0;
	hp = (PUCHAR)Hexbuf;   //暂存HEX缓冲区开始指针
	*iBinLen = 0; 

	//LINE 数据格式：0x3A(1个字节) + 后续数据长度(2个字节) + 数据偏移址址(4个字节) + 数据类型(1个字节) + 数据内容(2个字节为单位) + 检验和(2个字节) + 0x0D + 0x0A

	while( 1 )
	{
		if( (StartPos + 2) > iHexBufLen ) //包内数据得至少得包含3个字节 //if( (StartPos + 2) > iHexBufLen ) //包内数据得至少得包含3个字节
			return FALSE;
		
		//取行数据头标志0x3A
		if( hp[StartPos] == ':' )
		{
			UCHAR i;
			ULONG Len = 0;          // 有效数据长度
			UCHAR CheckSum;         // 校验和		

			//取出HEX行的数据长度,2个字节
			Char2ToHex((char*)(hp + StartPos + 1),(PUCHAR)&Len); 
			if( (StartPos + Len*2 + 13) > iHexBufLen ) //一个HEX行数据长度至少13字节
				return FALSE;                          //防止非法访问内存

			//LINE OFFSET3-4-5-6:起始地址，4字节
			//LINE OFFSET7...数据内容,2的倍数			
			for( CheckSum=(UCHAR)Len, i=0; i < (Len+4); ++i ) //标志头后面和检验和之间的数据值的相加，最终与累加和相加，结果为0x100的整数倍，否则检验和出错
			{// 将节数据转换到缓冲区里
				Char2ToHex((char *)(hp+StartPos+3+i+i),(PUCHAR)(DataBuf+i)); //DataBuf存放从起始地址开始的转换成数值的数据
				CheckSum += DataBuf[i];
			}
			if( CheckSum != 0 )   // 校验和失败,错误的HEX文件
				return FALSE;

			//数据类型分析
			switch ( DataBuf[2] ) 
			{
			case  0: // 数据记录
				{
					OfsAdr = DataBuf[0]*256 + DataBuf[1];
					WriteAdr = ExtAdr*65536 + SegAdr*16 + OfsAdr;

					if ((WriteAdr + Len) > iHexBufLen)
						return FALSE;

					memcpy(&(((PUCHAR)Binbuf)[WriteAdr]),&DataBuf[3],Len);
					StartPos += Len*2 + 13;         // 到下一个行的开始

					if( (WriteAdr + Len) > *iBinLen )
						*iBinLen = WriteAdr + Len;  // 得到最大结束地址
				}
				break;				
			case  2: // 扩展段地址记录
				{
					SegAdr = DataBuf[3]*256 + DataBuf[4];
					StartPos += 17;
				}
				break;				
			case  4: // 扩展线性地址记录
				{
					ExtAdr = DataBuf[3]*256 + DataBuf[4];
					StartPos += 17;
				}
				break;				
			case 5:  //开始线性地址记录
				{
					StartPos += 21;
				}
				break;				
			case 3: //开始段地址记录
				{
					StartPos += 21;
				}
				break;				
			case  1:  // HEX文件结束标志
				return TRUE;				
			default:
				return FALSE;
			}
		}
		else
		{
			return FALSE;                 // 不可能出现
		}
	}
}