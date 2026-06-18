//Copyright (c) 1999-2012  WCH Corporation
//Module Name:Main.cpp
/*
Abstract:
    编程辅助函数
Environment:
    user mode only
Notes:
  Copyright (c) 1999-2012 WCH Corporation.  All Rights Reserved.
Revision History:
  6/5/2012: L.s create
--*/

#include "Main.h"
#include "resource.h"
extern HWND AfxMainHwnd;

//输出格式化字符串,用dbgview软件接收
VOID  DbgPrint (LPCTSTR lpFormat,...)
{   
   CHAR TextBufferTmp[10240]="";    
   
   va_list arglist;
   va_start(arglist, lpFormat);
   vsprintf(TextBufferTmp,lpFormat,arglist);
   va_end(arglist);
   strcat(TextBufferTmp,"\r\n");
   OutputDebugString(TextBufferTmp);

   AddStrToEdit(AfxMainHwnd,IDC_ResultShow,TextBufferTmp);
   return ;
}

/*显示上次运行错误*/
void ShowLastError(PCHAR FuncName) 
{
	DWORD LastResult=0; // pointer to variable to receive error codes	
	char szSysMsg[100],tem[100]="";
	LastResult=GetLastError();
	FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM,0,LastResult,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),szSysMsg,sizeof(szSysMsg),0);	
	DbgPrint("%s:Last Err(%x)%s",FuncName,LastResult,szSysMsg);
}

//获取硬件计数器已运行时间,ms为单位,比GetTickCount更准确
double GetCurrentTimerVal()
{
	LARGE_INTEGER litmp; 
	double dfFreq,QPart1; 
	QueryPerformanceFrequency(&litmp);  //频率以HZ为单位
	dfFreq = (double)litmp.QuadPart;    //获得计数器的时钟频率
	QueryPerformanceCounter(&litmp);
	QPart1 = (double)litmp.QuadPart;        //获得初始值
	return(QPart1 *1000/dfFreq  );  //获得对应的时间值=振荡次数/振荡频率，单位为秒
}

//延时函数,以ms为单位
VOID DelayTime1(
				double TimerVal //定时值为ms,其定时误差一般不超过0.5微秒，精度与CPU等机器配置有关
				)
{
	LARGE_INTEGER litmp; 
	LONGLONG QPart1,QPart2;
	double dfMinus, dfFreq, dfTim,NewTimerVal; 

	NewTimerVal = TimerVal*0.001;       //将ms定时值转成s值
	QueryPerformanceFrequency(&litmp);  //频率以HZ为单位
	dfFreq = (double)litmp.QuadPart;    //获得计数器的时钟频率
	QueryPerformanceCounter(&litmp);
	QPart1 = litmp.QuadPart;            //获得初始值
	do{
		QueryPerformanceCounter(&litmp);
		QPart2 = litmp.QuadPart;        //获得中止值
		dfMinus = (double)(QPart2-QPart1);
		dfTim = dfMinus / dfFreq;       //获得对应的时间值=振荡次数/振荡频率，单位为秒		
	}while(dfTim<NewTimerVal);
	return;
}

//输出调试信息到文件里
BOOL LogToFile(PUCHAR Buffer,  //打印信息
				 ULONG OLen,   //打印信息长度
				 BOOL IsChar)  //打印信息以字符形式显示还是以数值形式显示
{ 
	char tembuf[1000]="";
	CHAR tem[10]="";
	HANDLE DFileHandle;
	DWORD nNumberOfBytesToWrite,i;
	DFileHandle = CreateFile("DbgInfor.txt",GENERIC_READ|GENERIC_WRITE,FILE_SHARE_WRITE|FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_ARCHIVE,NULL);
	if(DFileHandle==INVALID_HANDLE_VALUE)
		return FALSE;
	nNumberOfBytesToWrite=OLen;
	ZeroMemory(tembuf,sizeof(tembuf));
	if(!IsChar){
		for(i=0;i<OLen;i++){
			sprintf(tem,"%02X",Buffer[i]);
			strcat(tembuf,tem);			
		}
		strcat(tembuf,"\r\n");
		nNumberOfBytesToWrite=OLen*2+1;
	}
	else{		
		strcpy(tembuf,(PCHAR)Buffer);
		strcat(tembuf,"\r\n\0");
		nNumberOfBytesToWrite=strlen((PCHAR)Buffer);
	}
	SetFilePointer(DFileHandle,0,NULL,FILE_END);
	WriteFile(DFileHandle,tembuf,nNumberOfBytesToWrite,&nNumberOfBytesToWrite,NULL);
	CloseHandle(DFileHandle);
	return FALSE;
}


/*将十六进制字符转换成十进制码,数字转换成字符用ltoa()函数*/
ULONG mStrToHEX(PCHAR str) 
{  
	char mlen,i=0;
	UCHAR iChar=0,Char[9]="";
	UINT mBCD=0,de=1;
	mlen=strlen(str);
	memcpy(Char,str,mlen);
	for(i=mlen-1;i>=0;i--)
	{	iChar=Char[i];
	if ( iChar >= '0' && iChar <= '9' )
		mBCD = mBCD+(iChar -'0')*de;
	else if ( iChar >= 'A' && iChar <= 'F' ) 
		mBCD =mBCD+ (iChar - 'A' + 0x0a)*de;
	else if ( iChar >= 'a' && iChar <= 'f' )
		mBCD =mBCD+ (iChar - 'a' + 0x0a)*de;
	else return(0);
	de*=16;
	}
	return(mBCD);
}

/*将十进制字符转换成十进制码,数字转换成字符用ltoa()函数*/
ULONG mStrToBcd(PCHAR str) 
{  
	char mlen,i=0;

	UCHAR iChar=0,Char[9]="";
	UINT mBCD=0,de=1;

	mlen=strlen(str);
	memcpy(Char,str,mlen);
	for(i=mlen-1;i>=0;i--)
	{	iChar=Char[i];
	if ( iChar >= '0' && iChar <= '9' )
		mBCD = mBCD+(iChar -'0')*de;
	else return(0);
	de*=10;
	}
	return(mBCD);
}

//去除字符串中的前后空格
PVOID trim(PVOID str)  
{
	PCHAR x;
	char y[1000]="",z[1000]="";
	x=(char *)str;
	if (lstrlen(x)==0) return(NULL) ;
	while(*x==' '|| *x==0x09 )  //trimleft
		x++;
	if (*x==NULL)
		return(NULL);
	else
		strcpy(y,x);
	x=y+lstrlen(y);
	x--;
	while(*x==' '|| *x==0x09) x--;
	memcpy(z,y,x-y+1);
    z[lstrlen(z)]='\0';
	return(&z[0]);
}

//将格式化字符信息输出到文本框末尾
VOID  AddStrToEdit (HWND hDlg,ULONG EditID,const char * Format,...)
{
   va_list arglist;   
   int cb;
   CHAR buffer[512]="";

   va_start(arglist, Format);
   cb = _vsnprintf(&buffer[strlen(buffer)], sizeof(buffer), Format, arglist);
   if (cb == -1) 
   {
      buffer[sizeof(buffer) - 2] = '\n';
   }
   if(strlen(buffer) && buffer[strlen(buffer)-1]!='\n' )
	   strcat(buffer,"\r\n\0");   
   va_end(arglist);
   
   SendDlgItemMessage(hDlg,EditID,EM_SETSEL,0xFFFFFFFE,0xFFFFFFFE);
   SendDlgItemMessage(hDlg,EditID,EM_REPLACESEL,0,(LPARAM)buffer);
   SendDlgItemMessage(hDlg,EditID,EM_SETSEL,0xFFFFFFFE,0xFFFFFFFE);
   return ;
}
