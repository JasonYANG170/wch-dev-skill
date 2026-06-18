/********************************** (C) COPYRIGHT *******************************
* File Name          : EXAM1.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/03
* Description        :
 CH545 C语言的U盘文件字节读写示例程序，文件指针偏移，修改文件属性，删除文件等操作
 支持: FAT12/FAT16/FAT32
 注意包含 CH545UFI.LIB/USBHOST.C/DEBUG.C
 注；使用CH545UFI.LIB注意(编译器内存模式设置成:small)
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "absacc.h"
#include "DEBUG.H"
#include "CH545UFI.C"
#include "USBHOST.H"  

#pragma NOAREGS

UINT8X buf[100];	

/*******************************************************************************
* Function Name  : mStopIfError
* Description    : 检查操作状态,如果错误则显示错误代码并停机
* Input          : iError：错误类型
* Return         : None
*******************************************************************************/
void mStopIfError(UINT8 iError)
{
	if(iError == ERR_SUCCESS)
	{
		return;    /* 操作成功 */
	}
	printf("Error: %02X\n", (UINT16)iError);  /* 显示错误 */
	/* 遇到错误后,应该分析错误码以及CH554DiskStatus状态,例如调用CH549DiskReady查询当前U盘是否连接,如果U盘已断开那么就重新等待U盘插上再操作,
		 建议出错后的处理步骤:
		 1、调用一次CH549DiskReady,成功则继续操作,例如Open,Read/Write等
		 2、如果CH549DiskReady不成功,那么强行将从头开始操作(等待U盘连接，CH554DiskReady等) */
	while(1)
	{
	}
}

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Return         : None
*******************************************************************************/
void main(void)
{
	UINT8 s,c,i;
	UINT16 TotalCount;
	
	CfgFsys();
	mDelaymS(20);                                                              //修改主频，稍加延时等待主频稳定
	mInitSTDIO();                                                            //初始化串口0为了让计算机通过串口监控演示过程
	printf("start ...\n");
	InitUSB_Host();
	CH549LibInit();                                                          //初始化CH549程序库以支持U盘文件
	printf("start ...:%x\n",(UINT16)CH549GetVer());
	FoundNewDev = 0;
	while(1)
	{
		s = ERR_SUCCESS;
		if(UIF_DETECT)                                                   // 如果有USB主机检测中断则处理
		{
			UIF_DETECT = 0;                                                 // 清连接中断标志
			s = AnalyzeRootHub();                                          // 分析ROOT-HUB状态
			if(s == ERR_USB_CONNECT)
			{
				FoundNewDev = 1;
			}
		}
		if(FoundNewDev || s == ERR_USB_CONNECT)                         // 有新的USB设备插入
		{
			FoundNewDev = 0;
			mDelaymS(200);                                               // 由于USB设备刚插入尚未稳定,故等待USB设备数百毫秒,消除插拔抖动
			s = InitRootDevice();                                         // 初始化USB设备
			if (s == ERR_SUCCESS)
			{
				// U盘操作流程：USB总线复位、U盘连接、获取设备描述符和设置USB地址、可选的获取配置描述符，之后到达此处，由CH549子程序库继续完成后续工作
				CH549DiskStatus = DISK_USB_ADDR;
				for(i = 0;i != 10;i ++)
				{
					printf("Wait DiskReady\n");
					s = CH549DiskReady();                                 //等待U盘准备好
					if(s == ERR_SUCCESS)
					{
						break;
					}
					else
					{
						printf("%02x\n",(UINT16)s);
					}
					mDelaymS(50);
				}
				if(CH549DiskStatus >= DISK_MOUNTED)
				{
					//创建文件演示
					printf("Create\n");
					strcpy(mCmdParam.Create.mPathName, "/NEWFILE.TXT");          /* 新文件名,在根目录下,中文文件名 */
					s = CH549FileCreate();                                        /* 新建文件并打开,如果文件已经存在则先删除后再新建 */
					mStopIfError(s);
					printf("ByteWrite\n");
					//实际应该判断写数据长度和定义缓冲区长度是否相符，如果大于缓冲区长度则需要多次写入
					i = sprintf(buf,"Note: \xd\xa这个程序是以字节为单位进行U盘文件读写,549简单演示功能。\xd\xa");  /*演示 */
					for(c=0; c<10; c++)
					{
						mCmdParam.ByteWrite.mByteCount = i;                          /* 指定本次写入的字节数 */
						mCmdParam.ByteWrite.mByteBuffer = buf;                       /* 指向缓冲区 */
						s = CH549ByteWrite();                                       /* 以字节为单位向文件写入数据 */
						mStopIfError(s);
						printf("成功写入 %02X次\n",(UINT16)c);
					}

					printf("Close\n");
					mCmdParam.Close.mUpdateLen = 1;                                  /* 自动计算文件长度,以字节为单位写文件,建议让程序库关闭文件以便自动更新文件长度 */
					i = CH549FileClose();
					mStopIfError(i);
				}
			}
		}
		mDelaymS(100);  // 模拟单片机做其它事
		SetUsbSpeed(1);  // 默认为全速
	}
}