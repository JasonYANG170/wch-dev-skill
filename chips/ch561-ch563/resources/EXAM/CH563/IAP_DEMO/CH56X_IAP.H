#ifndef __CH563_IAP_H__
#define __CH563_IAP_H__



#define Uart_Sync_Head1		0x57
#define Uart_Sync_Head2		0xab

/*以下为ISP下载命令定义*/
#define		CMD_IAP_PROM		0x80		//ISP编程命令
#define		CMD_IAP_ERASE		0x81		//ISP擦除命令
#define		CMD_IAP_VERIFY	    0x82		//ISP校验命令
#define		CMD_IAP_END			0x83		//ISP结束标志

/********************************************************************************/
/*
  命令格式说明:
 1, CMD_IAP_END(IAP结束标志):
    此命令格式为:CMD_IAP_END+0X0000表示结束.
 2, CMD_IAP_VERIFY(IAP校验命令):
 	此命令格式如下:
 		CMD_IAP_VERIFY+2字节数据长度+4字节数据地址
	返回数据:
	正确:0x0000+从地址开始的2字节数据长度数据
	错误:0x0001
 3, CMD_IAP_PROM(IAP编程命令):
 	此命令格式如下:
 		CMD_IAP_PROM+2字节数据长度+4字节数据地址+后续数据.注意此命令发下来的数据个数必须为4字节的整数倍
 	返回数据:
		正确:0x0000
		错误:0x0001 		 	
 4, CMD_IAP_ERASE(IAP擦除命令):
 	此命令格式如下:
 		CMD_IAP_ERASE+2字节开始地址，一般为0x0000，此命令只需要发送一次
	返回数据:
		正确:0x0000
		错误:0x0001 		 	
*/  
#define 	IAP_LEN			64
/*ISP下载程序结构体*/
typedef union _IAP_CMD {
	struct {
		UINT8	cmd;
		UINT8	len;
		UINT8	addr[2];
	} erase;				/*擦除命令*/
	struct {
		UINT8	cmd;
		UINT8	len;
		UINT8	status[2];
	} end;					/*结束命令*/
	struct {				
		UINT8	cmd;
		UINT8	len;			//后续数据长度
		UINT8	addr[2];			//校验地址
		UINT8	buf[IAP_LEN-4];	//校验数据
	} verify;				/*校验命令*/
	struct {				
		UINT8	cmd;
		UINT8	len;			//后续数据长度
		UINT8	addr[2];			//地址
		UINT8	buf[IAP_LEN-4];	//数据
	}program;				 /*编程命令*/
	struct {
		UINT8	buf[IAP_LEN];
	} other;				/*接收命令*/					
}iap_cmd1;



#endif
