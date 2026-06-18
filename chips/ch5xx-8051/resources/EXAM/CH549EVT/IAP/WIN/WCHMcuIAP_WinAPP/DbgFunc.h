//Copyright (c) 1999-2012  WCH Corporation
//Module Name:DbgFunc.h
/*
Abstract:
    Function for outputing debug information 
Environment:
    user mode only
Notes:
  Copyright (c) 1999-2012 WCH Corporation.  All Rights Reserved.
Revision History:
  6/5/2012: L.s create
--*/

#ifndef		_DEBUGFUNC_H
#define		_DEBUGFUNC_H

VOID  DbgPrint (LPCTSTR lpFormat,...); //显示格式化字符串
void ShowLastError(PCHAR FuncName); //显示上次运行错误
ULONG mStrToBcd(PCHAR str); //将十六进制字符转换成十进制码,数字转换成字符用ltoa()函数
double GetCurrentTimerVal(); //获取硬件计数器已运行时间,ms为单位,比GetTickCount更准确
VOID DelayTime1(double TimerVal); //定时值为ms,其定时误差一般不超过0.5微秒，精度与CPU等机器配置有关
PVOID trim(PVOID str);  //去除字符串中的前后空格
VOID  DbgPrint (LPCTSTR lpFormat,...); //输出格式化字符串,用dbgview软件接收
void ShowLastError(PCHAR FuncName); //显示上次运行错误
double GetCurrentTimerVal(); //获取硬件计数器已运行时间,ms为单位,比GetTickCount更准确
//延时函数,以ms为单位
VOID DelayTime1(
				double TimerVal //定时值为ms,其定时误差一般不超过0.5微秒，精度与CPU等机器配置有关
				);
//输出调试信息到文件里
BOOL LogToFile(PUCHAR Buffer,  //打印信息
				 ULONG OLen,   //打印信息长度
				 BOOL IsChar);  //打印信息以字符形式显示还是以数值形式显示
ULONG mStrToHEX(PCHAR str); //将十六进制字符转换成十进制码,数字转换成字符用ltoa()函数
ULONG mStrToBcd(PCHAR str); //将十进制字符转换成十进制码,数字转换成字符用ltoa()函数
PVOID trim(PVOID str); //去除字符串中的前后空格
VOID  AddStrToEdit (HWND hDlg,ULONG EditID,const char * Format,...); //将格式化字符信息输出到文本框末尾

#endif