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
    WCH MCU IAP Windows下载示例界面代码
	支持CH56X，CH55X单片机
*/

#include "resource.h"
#include <stdio.h>
#include "Main.h"
#include "IAP.H"
#include "CH375DLL.H"
#pragma comment (lib,"CH375DLL")

//全局变量
HWND AfxMainHwnd; //主窗体句柄
HINSTANCE AfxMainIns; //进程实例

DEFINE_GUID(GUID_DEVINTERFACE_COMPORT,                0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);

#define mDnDev_MAX_NUMBER 16
DnDevInforS AfxDnDev[mDnDev_MAX_NUMBER] = {0}; //设备列表
UCHAR AfxDnDevCnt;
ULONG AfxDnInterface;
BOOL IsDownloading,IsDeviceChanged;

//函数声明
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

//应用程序入口
int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow)
{
	AfxMainIns = hInstance;
	return 	DialogBox(hInstance, (LPCTSTR)IDD_IAPDEMO_DIALOG, 0, (DLGPROC)WndProc);
}

//查找下载设备接口，并显示
ULONG ScanDnDeviceAndShow()
{
	ULONG i,DevID;
	CHAR FmtStr[128] = "";			

	AfxDnDevCnt = 0;	
	if( AfxDnInterface == 0 ) //USB接口
	{
		for (i = 0;i < mDnDev_MAX_NUMBER;++i)
		{
			if( CH375OpenDevice(i) != INVALID_HANDLE_VALUE )//找到一个设备
			{
				DevID = CH375GetUsbID(i);
				if( ( LOWORD(DevID) == 0x4348 ) || //下载的USB设备VID号，与MCU对应
					( HIWORD(DevID) == 0x55E0 ) )  //下载的USB设备PID号，与MCU对应
				{
					strcpy(&AfxDnDev[AfxDnDevCnt].DevName[0],(PCHAR)CH375GetDeviceName(i));
					if( strlen(AfxDnDev[AfxDnDevCnt].DevName) == 0 ) //无效设备名称
						continue;
					AfxDnDev[AfxDnDevCnt].iIndex = AfxDnDevCnt;
					AfxDnDevCnt++;
				}
				CH375CloseDevice(i);
			}
		}
		if( AfxDnDevCnt )
			DbgPrint("已查找到%d个USB设备",AfxDnDevCnt);
		else
			DbgPrint("未找到USB设备，请检查MCU是否已下载IAP代码，以及MCU是否已进入IAP模式",AfxDnDevCnt);
	}
	else //枚举串口
	{	
		SP_DEVINFO_DATA DeviceInfoData;
		HDEVINFO COMDeviceInfoSet = INVALID_HANDLE_VALUE;
		CHAR PortName[24];
		DWORD dwType, dwComNameLen;
		HKEY hDeviceKey;
		
		COMDeviceInfoSet = SetupDiGetClassDevs(&GUID_DEVINTERFACE_COMPORT,0,AfxMainHwnd, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT ); // COM Port class present on system		
		//Enumerate through all Devices.        
		DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
		for (i=0;SetupDiEnumDeviceInfo(COMDeviceInfoSet,i,&DeviceInfoData);i++)
		{
			PortName[0] = 0;
			hDeviceKey = SetupDiOpenDevRegKey(COMDeviceInfoSet,
				&DeviceInfoData,DICS_FLAG_GLOBAL,0,DIREG_DEV,KEY_ALL_ACCESS);
			if (hDeviceKey != INVALID_HANDLE_VALUE)
			{
				dwComNameLen = sizeof(PortName);
				RegQueryValueEx(hDeviceKey,REGSTR_VAL_PORTNAME,0,&dwType,(PBYTE)PortName,&dwComNameLen);
				RegCloseKey( hDeviceKey );
			}
			if( strlen(PortName) ) //找到一个串口
			{
				strcpy(&AfxDnDev[AfxDnDevCnt].DevName[0],PortName);
				AfxDnDev[AfxDnDevCnt].iIndex = AfxDnDevCnt;
				AfxDnDevCnt++;
			}			
		}
		if( COMDeviceInfoSet != INVALID_HANDLE_VALUE )
			SetupDiDestroyDeviceInfoList(COMDeviceInfoSet);
		DbgPrint("已查找到%d个串口",AfxDnDevCnt);
	}

	//清空设备列表内容
	SendDlgItemMessage(AfxMainHwnd,IDC_DeviceList,CB_RESETCONTENT,0,0);
	for(i=0;i<AfxDnDevCnt;i++)
	{
		if( AfxDnInterface == 0 )
			sprintf(FmtStr,"%d号设备",i); //显示名称可自已定义
		else
			sprintf(FmtStr,"%s",AfxDnDev[i].DevName); //显示名称可自已定义
		SendDlgItemMessage(AfxMainHwnd,IDC_DeviceList,CB_ADDSTRING,0,(LPARAM)(LPCTSTR)FmtStr);
	}
	SendDlgItemMessage(AfxMainHwnd,IDC_DeviceList,CB_SETCURSEL,0,0);

	return AfxDnDevCnt;
}

// 监视USB设备的插拔
VOID CALLBACK NotifyUsbDnDeviceRoutine(ULONG	iEventStatus)  // 设备事件和当前状态(在下行定义): 0=设备拔出事件, 3=设备插入事件
{
	if( AfxDnInterface != 0 ) //非USB方式下载，不用更新设备列表
		return;
	if( ( iEventStatus == CH375_DEVICE_ARRIVAL ) || ( iEventStatus == CH375_DEVICE_REMOVE) )//设备插入
	{
		if( IsDownloading ) //正在下载程序，暂停刷新设备列表
			IsDeviceChanged = TRUE; //下载完成后刷新设备列表
		else
		{
			IsDeviceChanged = FALSE;
			PostMessage(GetDlgItem(AfxMainHwnd,IDC_ScanDev),BM_CLICK,0,0); //刷新设备列表
		}
	}
}

//主窗体进程
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	AfxMainHwnd = hWnd;

	switch (message)
	{
	case WM_INITDIALOG:
		//初始化下载方式
		SendDlgItemMessage(hWnd,IDC_DnInterface,CB_ADDSTRING,0,(LPARAM)(LPCTSTR)"USB");
		SendDlgItemMessage(hWnd,IDC_DnInterface,CB_ADDSTRING,0,(LPARAM)(LPCTSTR)"串口");
		SendDlgItemMessage(hWnd,IDC_DnInterface,CB_SETCURSEL,0,0);
		AfxDnInterface = SendDlgItemMessage(hWnd,IDC_DnInterface,CB_GETCURSEL,0,0);

		CH375SetDeviceNotify(0,NULL,NotifyUsbDnDeviceRoutine);	//启动设备插拔监视
		IsDownloading = IsDeviceChanged = FALSE;

		PostMessage(GetDlgItem(AfxMainHwnd,IDC_ScanDev),BM_CLICK,0,0); //刷新设备列表
		break;
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDC_DnInterface: 
			if( (wmEvent == CBN_SELCHANGE) ) //更新接口设备列表
			{
				//刷新设备列表
				AfxDnInterface = SendDlgItemMessage(hWnd,IDC_DnInterface,CB_GETCURSEL,0,0);
				ScanDnDeviceAndShow(); //刷新设备列表
			}
			break;
		case IDC_SelectFile: //选择下载文件
			
			{// 获取将要发送的文件名		
				CHAR FmtStr[256] = "",FileName[512] = "";
				OPENFILENAME mOpenFile={0};					

				sprintf(FmtStr,"选择下载文件");			
				// Fill in the OPENFILENAME structure to support a template and hook.
				mOpenFile.lStructSize = sizeof(OPENFILENAME);
				mOpenFile.hwndOwner         = AfxMainHwnd;
				mOpenFile.hInstance         = AfxMainIns;		
				mOpenFile.lpstrFilter       = "*.HEX\0*.HEX\0*.BIN\0*.BIN\0";		
				mOpenFile.lpstrCustomFilter = NULL;
				mOpenFile.nMaxCustFilter    = 0;
				mOpenFile.nFilterIndex      = 0;
				mOpenFile.lpstrFile         = FileName;
				mOpenFile.nMaxFile          = sizeof(FileName);
				mOpenFile.lpstrFileTitle    = NULL;
				mOpenFile.nMaxFileTitle     = 0;
				mOpenFile.lpstrInitialDir   = NULL;
				mOpenFile.lpstrTitle        = FmtStr;
				
				mOpenFile.nFileOffset       = 0;
				mOpenFile.nFileExtension    = 0;
				mOpenFile.lpstrDefExt       = NULL;
				mOpenFile.lCustData         = 0;
				mOpenFile.lpfnHook 		   = NULL;
				mOpenFile.lpTemplateName    = NULL;
				mOpenFile.Flags             = OFN_SHOWHELP | OFN_EXPLORER | OFN_READONLY | OFN_FILEMUSTEXIST;
				if (!GetOpenFileName(&mOpenFile))
				{ 			
					DbgPrint("选择发送文件出错");				
				}
				else
				{
					SetDlgItemText(AfxMainHwnd,IDC_DownloadFile,FileName); //保存下载文件
					SendDlgItemMessage(AfxMainHwnd,IDC_DownloadFile,EM_SETSEL,0xFFFFFFFE,0xFFFFFFFE);//将插入符移到文本框的末尾
				}
			}
			break;
		case IDC_ScanDev: //扫描设备
			ScanDnDeviceAndShow();
			break;
		case IDC_ClearResult: //清空结果信息
			SetDlgItemText(AfxMainHwnd,IDC_ResultShow,"");
			break;
		case IDC_Download: //下载文件
			DWORD ThreadID;

			if( SendDlgItemMessage(hWnd,IDC_DeviceList,CB_GETCURSEL,0,0) == CB_ERR )
			{
				MessageBox(AfxMainHwnd,"请先选择下载设备或者当前没有找到下载设备","IAP DEMO",MB_ICONERROR);
				DbgPrint("请先选择下载设备或者当前没有找到下载设备.");
				break;
			}
			{//判断下载文件是否存在
				CHAR FileName[MAX_PATH] = "";
				OFSTRUCT lpReOpenBuff = {0};

				if( GetDlgItemText(AfxMainHwnd,IDC_DownloadFile,FileName,sizeof(FileName)) < 1 )
				{
					MessageBox(AfxMainHwnd,"请先选择下载文件","IAP DEMO",MB_ICONERROR);
					DbgPrint("下载失败!请先选择下载文件");
					break;
				}				
				if ( OpenFile(FileName,&lpReOpenBuff,OF_EXIST) ==  HFILE_ERROR )
				{
					MessageBox(AfxMainHwnd,"下载文件不存在或正在使用","IAP DEMO",MB_ICONERROR);
					DbgPrint("下载失败!下载文件不存在或正在使用");
					break;
				}
			}
			CloseHandle(CreateThread(NULL,0,IAPFlashDownloadThread,NULL,0,&ThreadID)); //开始USB下载

			break;	
		case WM_DESTROY:
			CH375SetDeviceNotify(0,NULL,NULL); //取消设备插拔检测
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;		
		case WM_DESTROY:
			PostQuitMessage(0);
			break;		
	}
	return 0;
}