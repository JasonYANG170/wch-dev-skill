#ifndef		_MAIN_H
#define		_MAIN_H

// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>

#include <windows.h>
#include <stdio.h>
#include <initguid.h>
#include <regstr.h>
#include <setupapi.h>
#pragma comment(lib,"setupapi")
#include "CH375DLL.H"
//#include <afxdlgs.h>
#pragma comment (lib,"CH375DLL")
#include "DbgFunc.h"


//下载设备信息结构
typedef struct _DnDeviceInfor
{
	UCHAR iIndex;   //设备序号
	CHAR  DevName[128];  //设备名称,用于通讯使用
	CHAR  DevDispName[128]; //在设备管理器内显示的名称
}DnDevInforS,*PDnDevInforS;

#endif		// MAIN_H