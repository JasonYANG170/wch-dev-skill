/********************************** (C) COPYRIGHT *******************************
* File Name          : EXAM11.C
* Author             : WCH
* Version            : V1.0
* Date               : 2020/07/03
* Description        : CH545 C语言的U盘目录文件枚举程序
 支持: FAT12/FAT16/FAT32
 注意包含 CH545UFI.LIB/CH545_USB.C/DEBUG.C
 注；使用CH545UFI.LIB注意(编译器内存模式设置成:small)
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "absacc.h"
#include "CH545UFI.C"
#include "DEBUG.H"
#include "USBHOST.H"

#pragma NOAREGS

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
	/* 遇到错误后,应该分析错误码以及CH545DiskStatus状态,例如调用CH545DiskReady查询当前U盘是否连接,如果U盘已断开那么就重新等待U盘插上再操作,
		 建议出错后的处理步骤:
		 1、调用一次CH545DiskReady,成功则继续操作,例如Open,Read/Write等
		 2、如果CH545DiskReady不成功,那么强行将从头开始操作(等待U盘连接，CH545DiskReady等) */
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
	UINT8 s,i;
	UINT8 *pCodeStr;
	UINT16 j;
	
	CfgFsys();
	mDelaymS(5);                                                              //修改主频稍加延时等待主频稳定
	mInitSTDIO();                                                            //初始化串口0为了让计算机通过串口监控演示过程 */
	InitUSB_Host();
	CH549LibInit();                                                          //初始化CH545程序库以支持U盘文件
	FoundNewDev = 0;
	printf( "Wait Device In:%02x\n" ,(UINT16)CH549GetVer());
	while(1)
	{
		s = ERR_SUCCESS;
		if(UIF_DETECT)                                                     // 如果有USB主机检测中断则处理
		{
			UIF_DETECT = 0;                                                     // 清中断标志
			s = AnalyzeRootHub();                                              // 分析ROOT-HUB状态
			if(s == ERR_USB_CONNECT)
			{
				FoundNewDev = 1;
			}
		}
		if(FoundNewDev || s == ERR_USB_CONNECT)
		{
			// 有新的USB设备插入
			FoundNewDev = 0;
			mDelaymS(200);                                                    // 由于USB设备刚插入尚未稳定,故等待USB设备数百毫秒,消除插拔抖动
			s = InitRootDevice();                                              // 初始化USB设备
			if(s == ERR_SUCCESS)
			{
				printf("Start UDISK_demo @CH549UFI library\n");
				// U盘操作流程：USB总线复位、U盘连接、获取设备描述符和设置USB地址、可选的获取配置描述符，之后到达此处，由CH549子程序库继续完成后续工作
				CH549DiskStatus = DISK_USB_ADDR;
				for(i = 0; i != 10; i ++)
				{
					printf("Wait DiskReady\n");
					s = CH549DiskReady();
					if(s == ERR_SUCCESS)
					{
						break;
					}
					mDelaymS(50);
				}
				if(CH549DiskStatus >= DISK_MOUNTED)                           //U盘准备好
				{
					/* 读文件 */ 
					strcpy(mCmdParam.Open.mPathName, "/C51/CH549HFT.C");         //设置要操作的文件名和路径
					s = CH549FileOpen();                                          //打开文件
					printf("Open:%02x\n",(UINT16)s);
					if(s == ERR_MISS_DIR)
					{
						printf("不存在该文件夹则列出根目录所有文件\n");
						pCodeStr = "/*";
					}
					else
					{
						pCodeStr = "/C51/*";    //列出\C51子目录下的的文件
					}
					printf("List file %s\n", pCodeStr);
					for(j = 0; j < 10000; j ++)                                 //限定10000个文件,实际上没有限制
					{
						strcpy(mCmdParam.Open.mPathName, pCodeStr);              //搜索文件名,*为通配符,适用于所有文件或者子目录
						i = strlen(mCmdParam.Open.mPathName);
						mCmdParam.Open.mPathName[i] = 0xFF;                      //根据字符串长度将结束符替换为搜索的序号,从0到254,如果是0xFF即255则说明搜索序号在CH549vFileSize变量中
						CH549vFileSize = j;                                        //指定搜索/枚举的序号
						i = CH549FileOpen();                                      //打开文件,如果文件名中含有通配符*,则为搜索文件而不打开
						/* CH549FileEnum 与 CH549FileOpen 的唯一区别是当后者返回ERR_FOUND_NAME时那么对应于前者返回ERR_SUCCESS */
						if(i == ERR_MISS_FILE)
						{
							break;    //再也搜索不到匹配的文件,已经没有匹配的文件名
						}
						if(i == ERR_FOUND_NAME)                                 //搜索到与通配符相匹配的文件名,文件名及其完整路径在命令缓冲区中
						{
							printf("match file %04d#: %s\n", (unsigned int)j, mCmdParam.Open.mPathName);  /* 显示序号和搜索到的匹配文件名或者子目录名 */
							continue;                                                /* 继续搜索下一个匹配的文件名,下次搜索时序号会加1 */
						}
						else                                                       //出错
						{
							mStopIfError(i);
							break;
						}
					}
					i = CH549FileClose();                                          //关闭文件
					printf("U盘演示完成\n");
				}
				else
				{
					printf("U盘没有准备好 ERR =%02X\n", (UINT16)s);
				}
			}
			else
			{
				printf("初始化U盘失败，请拔下U盘重试\n");
			}
		}
		mDelaymS(100);  // 模拟单片机做其它事
		SetUsbSpeed(1);  // 默认为全速
	}
}