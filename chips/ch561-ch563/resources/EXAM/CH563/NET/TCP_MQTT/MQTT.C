/********************************** (C) COPYRIGHT ******************************
* File Name		: MQTT.C
* Author		: WCH
* Version		: V1.0
* Date			: 2018/4/15
* Description	:基于TCP/IP的MQTT协议通讯例程，实现以MQTT方式，通过百度、阿里云物联网服务器实现设备互通；使用前需设置帐号和密码
*					(1)、CH563 Examples by KEIL;
*					(2)、串口0输出监控信息,115200bps;
*					(3)、本程序用于演示基于TCP/IP的MQTT协议通讯，
					     单片机连接以太网、MQTT服务器后将会发布一个主题，
					     再订阅这个主题，并向这个主题发布消息，
						 最后接收到自己发送的消息。
*******************************************************************************/



/******************************************************************************/
/* 头文件包含*/
#include <stdio.h>							 
#include <string.h>
#include "CH563SFR.H"
#include "SYSFREQ.H"

#include "CH563NET.H"
#include "ISPXT56X.H"
#include "MQTTPacket.H"

#define CH563NET_DBG		1			 
#define KEEPLIVE_ENABLE		1														/* 开启KEEPLIVE功能 */


/* 下面的缓冲区和全局变量必须要定义，库中调用 */
__align(16)UINT8		CH563MACRxDesBuf[(RX_QUEUE_ENTRIES )*16];					/* MAC接收描述符缓冲区，16字节对齐 */
__align(4) UINT8		CH563MACRxBuf[RX_QUEUE_ENTRIES*RX_BUF_SIZE];				/* MAC接收缓冲区，4字节对齐 */
__align(4) SOCK_INF SocketInf[CH563NET_MAX_SOCKET_NUM];								/* Socket信息表，4字节对齐 */
const UINT16 MemNum[8] = {CH563NET_NUM_IPRAW,
												 CH563NET_NUM_UDP,
												 CH563NET_NUM_TCP,
												 CH563NET_NUM_TCP_LISTEN,
												 CH563NET_NUM_TCP_SEG,
												 CH563NET_NUM_IP_REASSDATA,
												 CH563NET_NUM_PBUF,
												 CH563NET_NUM_POOL_BUF
												 };
const UINT16 MemSize[8] = {CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_IPRAW_PCB),
													CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_UDP_PCB),
													CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_TCP_PCB),
													CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_TCP_PCB_LISTEN),
													CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_TCP_SEG),
													CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_IP_REASSDATA),
													CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_PBUF) + CH563NET_MEM_ALIGN_SIZE(0),
													CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_PBUF) + CH563NET_MEM_ALIGN_SIZE(CH563NET_SIZE_POOL_BUF)
												 };
__align(4)UINT8 Memp_Memory[CH563NET_MEMP_SIZE];
__align(4)UINT8 Mem_Heap_Memory[CH563NET_RAM_HEAP_SIZE];
__align(4)UINT8 Mem_ArpTable[CH563NET_RAM_ARP_TABLE_SIZE];
/******************************************************************************/
/* 本演示程序的相关宏 */
#define RECE_BUF_LEN	536					/* 接收缓冲区的大小 */
/* CH563NET库TCP的MSS长度为536字节，即一个TCP包里的数据部分最长为536字节 */
/* TCP协议栈采用滑动窗口进行流控，窗口最大值为socket的接收缓冲区长度。在设定 */
/* RX_QUEUE_ENTRIES时要考虑MSS和窗口之间的关系，例如窗口值为4*MSS，则远端一次会发送 */
/* 4个TCP包，如果RX_QUEUE_ENTRIES小于4，则必然会导致数据包丢失，从而导致通讯效率降低 */
/* 建议RX_QUEUE_ENTRIES要大于( 窗口/MSS ),如果多个socket同时进行大批量发送数据，则 */
/* 建议RX_QUEUE_ENTRIES要大于(( 窗口/MSS )*socket个数) 在多个socket同时进行大批数据收发时 */
/* 为了节约RAM，请将接收缓冲区的长度设置为MSS */




/*使用前需要设置对应服务器平台的帐号和密码*/
UINT8 MACAddr[6] = {0x84,0xc2,0xe4,0x05,0x06,0x17};			/* CH563MAC地址 */
UINT8 IPAddr[4] = {192,168,10,75};							/* CH563IP地址 */
UINT8 GWIPAddr[4] = {192,168,10,1};							/* CH563网关 */
UINT8 IPMask[4] = {255,255,255,0};							/* CH563子网掩码 */
UINT16 aport=1000;											/* CH563源端口 */

UINT16 DESPORT= 1883;										/* MQTT服务器端口 */
UINT8 DESIP[4] = {163,177,150,12};							/* MQTT服务器IP地址 */
char *username = "test1/device1";							/* 设备名，每个设备唯一，可用”/“做分级 */
char *password = "password";								/* 服务器登陆密码 */
char *sub_topic = "topic1";									/* 订阅的会话名，为了自发自收，应与发布的会话名相同 */
char *pub_topic = "topic1";									/* 发布的会话名 */


/* CH563相关定义 */
UINT8 SocketId;												/* 保存socket索引，可以不用定义 */
UINT8 SocketRecvBuf[RECE_BUF_LEN];							/* socket接收缓冲区 */
UINT8 MyBuf[RECE_BUF_LEN];									/* 定义一个临时缓冲区 */
UINT32 a=0;

UINT8 con_flag=0;											/* 已连接MQTT服务器标志位 */
UINT8 pub_flag=1;											/* 已发布会话消息标志位 */
UINT8 sub_flag=0;											/* 已订阅会话标志位 */
UINT8 tout_flag=0;											/* 超时标志位 */
UINT16 packetid=0;											/* 包ID */


/*******************************************************************************
* Function Name	: SysTimeInit
* Description	: 系统定时器初始化，CH563@100MHZ TIME0 10ms，根据CH563NETTIMEPERIOD
*				  来初始化定时器。
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void SysTimeInit(void)
{
		R8_TMR0_CTRL_MOD = RB_TMR_ALL_CLEAR;
		R32_TMR0_COUNT = 0x00000000; 
		R32_TMR0_CNT_END = 0x186a0 * CH563NETTIMEPERIOD;		/* 设置为10MS定时 */
		R8_TMR0_INTER_EN |= RB_TMR_IE_CYC_END;
		R8_TMR0_CTRL_MOD = RB_TMR_COUNT_EN;
}


/*******************************************************************************
* Function Name	: InitSysHal
* Description	: 硬件初始化操作，开启TIM0，ETH中断
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void InitSysHal(void)
{
		R8_INT_EN_IRQ_0 |= RB_IE_IRQ_TMR0;			/* 开启TIM0中断 */
		R8_INT_EN_IRQ_1 |= RB_IE_IRQ_ETH;			/* 开启ETH中断 */
		R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;		/* 开启IRQ全局中断 */
}


/*******************************************************************************
* Function Name	: mStopIfError
* Description	: 调试使用，显示错误代码
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void mStopIfError(UINT8 iError)
{
		if (iError == CH563NET_ERR_SUCCESS) return;			/* 操作成功 */
#if CH563NET_DBG
		printf("Error: %02X\n", (UINT16)iError);			/* 显示错误 */
#endif		
}


/*******************************************************************************
* Function Name	: Transport_Open
* Description	: 创建TCP连接
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
UINT8 Transport_Open()
{
	UINT8 i;																														 
	SOCK_INF TmpSocketInf;									/* 创建临时socket变量 */

	memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));		/* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
	memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);			/* 设置目的IP地址 */
	TmpSocketInf.DesPort = 1883;							/* 设置目的端口 */
	TmpSocketInf.SourPort = aport++;						/* 设置源端口 */
	TmpSocketInf.ProtoType = PROTO_TYPE_TCP;				/* 设置socekt类型 */
	TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf;	/* 设置接收缓冲区的接收缓冲区 */
	TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;				/* 设置接收缓冲区的接收长度 */
	i = CH563NET_SocketCreat(&SocketId,&TmpSocketInf);		/* 创建socket，将返回的socket索引保存在SocketId中 */
	mStopIfError(i);										/* 检查错误 */
	
	i = CH563NET_SocketConnect(SocketId);					/* TCP连接 */
	mStopIfError(i);										/* 检查错误 */
	return SocketId;
}


/*******************************************************************************
* Function Name	: Transport_Close
* Description	: 关闭TCP连接
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
UINT8 Transport_Close()
{
	UINT8 i;
	i=CH563NET_SocketClose(SocketId,TCP_CLOSE_NORMAL);
	mStopIfError(i);
	return i;
}


/*******************************************************************************
* Function Name	: Transport_SendPacket
* Description	: 以太网发送数据
* Input			: UINT8 *buf 发送数据的首字节地址
				  UINT32 len 发送数据的长度
* Output		: None
* Return		: None
*******************************************************************************/
void Transport_SendPacket(UINT8 *buf,UINT32 len)
{
	UINT32 totallen;
	UINT8 *p=buf;
	
	totallen=len;
	while(1)
	{
		len = totallen;
		CH563NET_SocketSend(SocketId,p,&len);					/* 将MyBuf中的数据发送 */
		totallen -= len;					 					/* 将总长度减去以及发送完毕的长度 */
		p += len;												/* 将缓冲区指针偏移*/
		if(totallen)continue;									/* 如果数据未发送完毕，则继续发送*/
		break;													/* 发送完毕，退出 */
	}
}


/*******************************************************************************
* Function Name	: MQTT_Connect
* Description	: 创建MQTT连接
* Input			: char *username 设备名
				  char *password 服务器连接密码
* Output		: None
* Return		: None
*******************************************************************************/
void MQTT_Connect(char *username,char *password)
{
	MQTTPacket_connectData data = MQTTPacket_connectData_initializer;
	UINT32 len;
	UINT8 buf[200];

	data.clientID.cstring = "11";
	data.keepAliveInterval = 20;
	data.cleansession = 1;
	data.username.cstring = username;																		
	data.password.cstring = password;
	 
	len=MQTTSerialize_connect(buf,sizeof(buf),&data);											
	Transport_SendPacket(buf,len);						/*建立MQTT连接*/
}


/*******************************************************************************
* Function Name	: MQTT_Subscribe
* Description	: MQTT订阅一个主题
* Input			: char *topic 订阅的主题名
* Output		: None
* Return		: None
*******************************************************************************/
void MQTT_Subscribe( char *topic)
{
	MQTTString topicString = MQTTString_initializer;
	int req_qos=0;
	UINT32 len;
	UINT32 msgid=1;
	UINT8 buf[200];
	
	topicString.cstring=topic;
	len=MQTTSerialize_subscribe(buf,sizeof(buf),0,msgid,1,&topicString,&req_qos);
	Transport_SendPacket(buf,len);
}


/*******************************************************************************
* Function Name	: MQTT_Unsubscribe
* Description	: MQTT取消订阅一个主题
* Input			: char *topic 取消订阅的主题名
* Output		: None
* Return		: None
*******************************************************************************/
void MQTT_Unsubscribe(char *topic)
{
	MQTTString topicString = MQTTString_initializer;
	UINT32 len;
	UINT32 msgid=1;
	UINT8 buf[200];
	
	topicString.cstring=topic;
	len=MQTTSerialize_unsubscribe(buf,sizeof(buf),0,msgid,1,&topicString);
	Transport_SendPacket(buf,len);
}


/*******************************************************************************
* Function Name	: MQTT_Subscribe
* Description	: MQTT订阅一个主题
* Input			: char *topic 订阅的主题名
* Output		: None
* Return		: None
*******************************************************************************/
void MQTT_Publish(char *topic,char *payload)
{
	MQTTString topicString = MQTTString_initializer;
	UINT32 payloadlen;
	UINT32 len;
	UINT8 buf[1024];
	
	topicString.cstring=topic; 
	payloadlen=strlen(payload);
	len= MQTTSerialize_publish(buf,sizeof(buf),0,0,0,packetid++,topicString,payload,payloadlen);	
	Transport_SendPacket(buf,len);
}


/*******************************************************************************
* Function Name	: MQTT_Pingreq
* Description	: MQTT发送心跳包
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void MQTT_Pingreq()
{
	UINT32 len;
	UINT8 buf[200];
	
	len=MQTTSerialize_pingreq(buf,sizeof(buf));
	Transport_SendPacket(buf,len);
}


/*******************************************************************************
* Function Name	: MQTT_Disconnect
* Description	: 断开MQTT连接
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void MQTT_Disconnect()
{
	UINT32 len;
	UINT8 buf[50];
	len=MQTTSerialize_disconnect(buf,sizeof(buf));
	Transport_SendPacket(buf,len);
}


/*******************************************************************************
* Function Name	: net_initkeeplive
* Description	: keeplive初始化
* Input			: None			
* Output		: None
* Return		: None
*******************************************************************************/
#ifdef	KEEPLIVE_ENABLE
void net_initkeeplive(void)
{
		struct _KEEP_CFG	klcfg;

		klcfg.KLIdle = 20000;				/* 空闲 */
		klcfg.KLIntvl = 10000;				/* 间隔 */
		klcfg.KLCount = 5;					/* 次数 */
		CH563NET_ConfigKeepLive(&klcfg);
}
#endif

/*******************************************************************************
* Function Name	: CH563NET_LibInit
* Description	: 库初始化操作
* Input			: ip			ip地址指针
*				: gwip		网关ip地址指针
*				: mask		掩码指针
*				: macaddr MAC地址指针 
* Output		: None
* Return		: 执行状态
*******************************************************************************/
UINT8 CH563NET_LibInit(const UINT8 *ip,const UINT8 *gwip,const UINT8 *mask,const UINT8 *macaddr)
{
		UINT8 i;
		struct _CH563_CFG cfg;

		if(CH563NET_GetVer() != CH563NET_LIB_VER)return 0xfc;			/* 获取库的版本号，检查是否和头文件一致 */
		CH563NETConfig = LIB_CFG_VALUE;									/* 将配置信息传递给库的配置变量 */
		cfg.RxBufSize = RX_BUF_SIZE; 
		cfg.TCPMss	 = CH563NET_TCP_MSS;
		cfg.HeapSize = CH563_MEM_HEAP_SIZE;
		cfg.ARPTableNum = CH563NET_NUM_ARP_TABLE;
		cfg.MiscConfig0 = CH563NET_MISC_CONFIG0;
		CH563NET_ConfigLIB(&cfg);
		i = CH563NET_Init(ip,gwip,mask,macaddr);
#ifdef	KEEPLIVE_ENABLE
		net_initkeeplive( );
#endif
		return (i);											/* 库初始化 */
}

/*******************************************************************************
* Function Name	: CH563NET_CreatTcpSocket
* Description	: 创建TCP Client socket
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void CH563NET_CreatTcpSocket(void)
{
	 UINT8 i;																														 
	 SOCK_INF TmpSocketInf;											/* 创建临时socket变量 */

	 memset((void *)&TmpSocketInf,0,sizeof(SOCK_INF));				/* 库内部会将此变量复制，所以最好将临时变量先全部清零 */
	 memcpy((void *)TmpSocketInf.IPAddr,DESIP,4);					/* 设置目的IP地址 */
	 TmpSocketInf.DesPort = 1000;									/* 设置目的端口 */
	 TmpSocketInf.SourPort = 2000;									/* 设置源端口 */
	 TmpSocketInf.ProtoType = PROTO_TYPE_TCP;						/* 设置socekt类型 */
	 TmpSocketInf.RecvStartPoint = (UINT32)SocketRecvBuf;			/* 设置接收缓冲区的接收缓冲区 */
	 TmpSocketInf.RecvBufLen = RECE_BUF_LEN ;						/* 设置接收缓冲区的接收长度 */
	 i = CH563NET_SocketCreat(&SocketId,&TmpSocketInf);				/* 创建socket，将返回的socket索引保存在SocketId中 */
	 mStopIfError(i);												/* 检查错误 */

#ifdef	KEEPLIVE_ENABLE
	 CH563NET_SocketSetKeepLive( SocketId, 1 );						/* 开启socket的KEEPLIVE功能（V06版本支持） */
#endif

	 i = CH563NET_SocketConnect(SocketId);							/* TCP连接 */
	 mStopIfError(i);												/* 检查错误 */
}

/*******************************************************************************
* Function Name	: CH563NET_HandleSockInt
* Description	: Socket中断处理函数
* Input			: sockeid	socket索引
*				: initstat 中断状态
* Output		: None
* Return		: None
*******************************************************************************/
void CH563NET_HandleSockInt(UINT8 sockeid,UINT8 initstat)
{
	UINT32 len;
	UINT8 i;

	unsigned char* dup;
	unsigned short* packetid;

	int* qos;
	unsigned char* retained;
	MQTTString* topicName;
	unsigned char* payload;
	int payloadlen;
	unsigned char *p=payload;

	if(initstat & SINT_STAT_RECV)							/* 接收中断 */
	{
		len = CH563NET_SocketRecvLen(sockeid,NULL);			/* 查询长度 */
		CH563NET_SocketRecv(sockeid,MyBuf,&len);			/* 将接收缓冲区的数据读到MyBuf中*/
		switch(MyBuf[0]>>4)
		{
			case FLAG_CONNACK:
				printf("connack\r\n");
				con_flag=1;
				break;

			case FLAG_PUBLISH:
				MQTTDeserialize_publish(dup,qos,retained,packetid,topicName,&payload,&payloadlen,MyBuf,len);
				printf("payloadlen=%d\r\n",(UINT16)payloadlen);
				p=payload;
				for(i=0;i<payloadlen;i++)
				{
					printf("%c ",(UINT16)*p);
					p++;
				}
				printf("\r\n");
				break;
				
			case FLAG_SUBACK:
				sub_flag=1;
				printf("suback\r\n");
				break;

			default:

				break;
		}
	}
	if(initstat & SINT_STAT_CONNECT)					/* TCP连接中断 */
	{
#if CH563NET_DBG										/* 产生此中断表示TCP已经连接，可以进行收发数据 */
		printf("TCP Connect Success\n");													 
#endif
		MQTT_Connect(username, password);

	}
	if(initstat & SINT_STAT_DISCONNECT)					/* TCP断开中断 */
	{
#if CH563NET_DBG										/* 产生此中断，CH563库内部会将此socket清除，置为关闭*/
		printf("TCP Disconnect\n");						/* 应用曾需可以重新创建连接 */
#endif												 
		CH563NET_CreatTcpSocket();
	}
	if(initstat & SINT_STAT_TIM_OUT)					/* TCP超时中断 */
	{
#if CH563NET_DBG										/* 产生此中断，CH563库内部会将此socket清除，置为关闭*/
		printf("TCP Timout\n");							/* 应用曾需可以重新创建连接 */
#endif
		Transport_Open();
	}
}

/*******************************************************************************
* Function Name	: CH563NET_HandleGloableInt
* Description	: 全局中断处理函数
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void CH563NET_HandleGlobalInt(void)
{
	UINT8 initstat;
	UINT8 i;
	UINT8 socketinit;
	
	initstat = CH563NET_GetGlobalInt();										/* 读全局中断状态并清除 */
	if(initstat & GINT_STAT_UNREACH)										/* 不可达中断 */
	{
#if CH563NET_DBG
		printf("UnreachCode ：%d\n",CH563Inf.UnreachCode);					/* 查看不可达代码 */
		printf("UnreachProto ：%d\n",CH563Inf.UnreachProto);				/* 查看不可达协议类型 */
		printf("UnreachPort ：%d\n",CH563Inf.UnreachPort);					/* 查询不可达端口 */
#endif			 
	}
	if(initstat & GINT_STAT_IP_CONFLI)										/* IP冲突中断 */
	{
	 
	}
	if(initstat & GINT_STAT_PHY_CHANGE)										/* PHY改变中断 */
	{
		i = CH563NET_GetPHYStatus();										/* 获取PHY状态 */
#if CH563NET_DBG
		printf("GINT_STAT_PHY_CHANGE %02x\n",i);
#endif	 
	}
	if(initstat & GINT_STAT_SOCKET)											/* Socket中断 */
	{
		for(i = 0; i < CH563NET_MAX_SOCKET_NUM; i ++)										 
		{
			socketinit = CH563NET_GetSocketInt(i);							/* 读socket中断并清零 */
			if(socketinit) CH563NET_HandleSockInt(i,socketinit);			/* 如果有中断则清零 */
		}		
	}
}



/*******************************************************************************
* Function Name	: mInitSTDIO
* Description	: 为printf和getkey输入输出初始化串口
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
void mInitSTDIO( void )
{
	UINT32		x, x2;

	x = 10 * FREQ_SYS * 2 / 16 / 115200;									/* 115200bps */
	x2 = x % 10;
	x /= 10;
	if ( x2 >= 5 ) x ++;													/* 四舍五入 */
	R8_UART0_LCR = 0x80;													/* DLAB位置1 */
	R8_UART0_DIV = 1;														/* 预分频 */
	R8_UART0_DLM = x>>8;
	R8_UART0_DLL = x&0xff;

	R8_UART0_LCR = RB_LCR_WORD_SZ ;													/* 设置字节长度为8 */
	R8_UART0_FCR = RB_FCR_FIFO_TRIG|RB_FCR_TX_FIFO_CLR|RB_FCR_RX_FIFO_CLR |		
								 RB_FCR_FIFO_EN ;									/* 设置FIFO触发点为14，清发送和接收FIFO，FIFO使能 */
	R8_UART0_IER = RB_IER_TXD_EN;													/* TXD enable */
	R32_PB_SMT |= RXD0|TXD0;														/* RXD0 schmitt input, TXD0 slow rate */
	R32_PB_PD &= ~ RXD0;															/* disable pulldown for RXD0, keep pullup */
	R32_PB_DIR |= TXD0;																/* TXD0 output enable */
}

/*******************************************************************************
* Function Name	: fputc
* Description	: 通过串口输出监控信息
* Input			: c-- writes the character specified by c 
*				  *f--the output stream pointed to by *f
* Output		: None
* Return		: None
*******************************************************************************/
int fputc( int c, FILE *f )
{
		R8_UART0_THR = c;													/* 发送数据 */
		while( ( R8_UART0_LSR & RB_LSR_TX_FIFO_EMP ) == 0 );				/* 等待数据发送 */
		return( c );
}


__irq void IRQ_Handler( void )
{
	if(R32_INT_FLAG & 0x8000)												/* 以太网中断 */
	{																		/* 以太网中断中断服务函数 */
			CH563NET_ETHIsr();
	}
	if(R32_INT_FLAG & RB_IF_TMR0)											/* 定时器中断 */
	{
		CH563NET_TimeIsr(CH563NETTIMEPERIOD);								/* 定时器中断服务函数 */
		R8_TMR0_INT_FLAG |= 0xff;											/* 清除定时器中断标志 */
		a++;
	 }
}

__irq void FIQ_Handler( void )
{
	while(1);
}

/*******************************************************************************
* Function Name	: main
* Description	: 主函数
* Input			: None
* Output		: None
* Return		: None
*******************************************************************************/
int main(void) 
{
	UINT32 i = 0;
	UINT16 TimeDelay=0;
	char payload[500];
	
	for(i=0;i<500;i++)
	payload[i]='a';

	mInitSTDIO( );															/* 为了让计算机通过串口监控演示过程 */
	i = CH563NET_LibInit(IPAddr,GWIPAddr,IPMask,MACAddr);					/* 库初始化 */
	mStopIfError(i);														/* 检查错误 */
#if CH563NET_DBG
	printf("CH563NETLibInit Success\n");
#endif		
	SysTimeInit();															/* 系统定时器初始化 */
	InitSysHal();															/* 初始化中断 */
	Transport_Open();	

	while(1)
	{
		CH563NET_MainTask();												/* CH563NET库主任务函数，需要在主循环中不断调用 */
		if (CH563NET_QueryGlobalInt()) CH563NET_HandleGlobalInt();			/* 查询中断，如果有中断，则调用全局中断处理函数 */
		Delay_ms(1);
		TimeDelay++;
		if (TimeDelay>500) {
			TimeDelay=0;
			if(con_flag) MQTT_Publish(pub_topic,payload);
		}
	}
}

/*********************************** endfile **********************************/
