/********************************** (C) COPYRIGHT *******************************
* File Name          : SD_INC.H
* Author             : WCH
* Version            : V1.10
* Date               : 2013/05/15
* Description        : SD卡相关定义头文件
*******************************************************************************/



#ifndef	__SD_INC_H__
#define __SD_INC_H__

/******************************************************************************/
/* SD卡状态结构体的定义 */
struct _SD_STAU
{
	UINT8 SD_Inster; 
	UINT8 SD_Write_Pro;
	UINT8 SD_Type;												 /* 0:第一版;1:第二版 */
	UINT8 SD_Cap_Type;											 /* 0:小容量;1:大容量  */
};

/*******************************************************************************/
/* SD卡信息结构体定义(the information structure variable of SD Card) */
typedef __packed struct SD_STRUCT
{	
	UINT8  card_type;
	UINT32 block_num;				 			  				 /* 卡中块的数量 */
	UINT16 block_len;				 							 /* 卡的块长度(单位:字节) */
	UINT16 erase_unit;				 							 /* 一次可擦除的块个数 */
	UINT32 SD_Capability;           							 /* SD卡容量,单位：字节*/
	UINT8  timeout_read;			 							 /* 读块超时时间(单位: 8 SPI clock) */
	UINT8  timeout_write;			 							 /* 写块超时时间(单位: 8 SPI clock) */
	UINT8  timeout_erase;			 							 /* 擦块超时时间(单位: 8 SPI clock) */
}sd_struct;

/******************************************************************************/
/* 以下为SD卡SPI模式下命令集 */
/******************************************************************************/

/******************************************************************************/
/* 应答方式 */
#define R1    							0x01
#define R1B   							0x02
#define R2    							0x03
#define R3    							0x04

/******************************************************************************/
/* R1和R2高字节错误码(R1 and upper byte of R2 error code) */
#define MSK_IDLE          		  		0x01
#define MSK_ERASE_RST     		  		0x02
#define MSK_ILL_CMD       		  		0x04
#define MSK_CRC_ERR       		  		0x08
#define MSK_ERASE_SEQ_ERR 		  		0x10
#define MSK_ADDR_ERR      		  		0x20
#define MSK_PARAM_ERR     		  		0x40

/******************************************************************************/
/* R2低字节错误码(lower byte of R2 error code) */
#define MSK_TOK_ERROR             		0x01
#define MSK_TOK_CC_ERROR          		0x02
#define MSK_TOK_ECC_FAILED        		0x04
#define MSK_TOK_CC_OUTOFRANGE     		0x08
#define MSK_TOK_CC_LOCKED         		0x10

/******************************************************************************/
/* 数据令牌(Data Tokens) */
#define SD_TOK_READ_STARTBLOCK    		0xFE
#define SD_TOK_WRITE_STARTBLOCK   		0xFE
#define SD_TOK_READ_STARTBLOCK_M  		0xFE
#define SD_TOK_WRITE_STARTBLOCK_M 		0xFC
#define SD_TOK_STOP_MULTI         		0xFD

/******************************************************************************/
/* 数据响应令牌(Data Response Tokens) */
#define SD_RESP_DATA_MSK		  		0x0F					 /* 数据响应掩码 */
#define SD_RESP_DATA_ACCETPTED	  		0x05					 /* 数据被接受 */
#define SD_RESP_DATA_REJECT_CRC	  		0x0B      				 /* 由于CRC错误而被拒绝 */
#define SD_RESP_DATA_REJECT_WRITE 		0x0D					 /* 由于写错误而被拒绝 */

/******************************************************************************/
/* 等待类型(Wait Type) */
#define SD_WAIT_READ			  		0x00					 /* 读等待 */
#define SD_WAIT_WRITE			  		0x01					 /* 写等待 */
#define SD_WAIT_ERASE		 	  		0x02					 /* 擦除等待 */

/******************************** 基本命令集  ********************************/
/* 复位SD 卡 */
#define CMD0 							0
#define CMD0_R 							R1

/* 激活该卡的初始化处理 */
#define CMD1 							1
#define CMD1_R 							R1

/* 第二版接口识别 */
#define CMD8 							8
#define CMD8_R 							R3

/* 读CSD寄存器 */
#define CMD9 							9
#define CMD9_R 							R1

/* 读CID寄存器 */
#define CMD10 							10
#define CMD10_R 						R1

/* 停止读多块时的数据传输  */
#define CMD12 							12
#define CMD12_R 						R1B

/* 读 Card_Status 寄存器  */
#define CMD13 							13
#define CMD13_R 						R2

/************************************ 块读命令集********************************/
/* 设置块的长度 */
#define CMD16 							16
#define CMD16_R 						R1

/* 读单块*/
#define CMD17 							17
#define CMD17_R 						R1

/* 读多块,直至主机发送CMD12为止 */
#define CMD18 							18
#define CMD18_R 						R1

/*************************************块写命令集 ********************************/
/* 写单块  */
#define CMD24 							24
#define CMD24_R 						R1

/* 写多块 */
#define CMD25 							25
#define CMD25_R 						R1

/* 写CSD寄存器 */
#define CMD27 							27
#define CMD27_R 						R1

/************************************ 写保护 ***********************************/
#define CMD28 							28
#define CMD28_R 						R1B

#define CMD29 							29
#define CMD29_R 						R1B

#define CMD30 							30
#define CMD30_R 						R1

/************************************* 擦除命令 *******************************/
/* 设置擦除块的起始地址(只用于SD卡) */
#define CMD32 							32
#define CMD32_R 						R1

/* 设置擦除块的终止地址(只用于SD卡)  */
#define CMD33 							33
#define CMD33_R 						R1

/* 设置擦除块的起始地址(只用于MMC卡) */
#define CMD35 							35
#define CMD35_R 						R1

/* 设置擦除块的终止地址(只用于MMC卡) */
#define CMD36 							36
#define CMD36_R 						R1

/* 擦除所选择的块 */
#define CMD38 							38
#define CMD38_R 						R1B

/************************************ 锁卡命令 **********************************/
/* 设置/复位密码或上锁/解锁卡 */
#define CMD42 							42
#define CMD42_R							R1B

/************************************ 应用命令  ********************************/
/* 通知下一个命令为应用命令 */
#define CMD55 							55
#define CMD55_R 						R1

/* 向卡发送或从卡接收数据块来获取通用/特殊命令集 */
#define CMD56 							56
#define CMD56_R 						R1

/* 读OCR寄存器 */
#define CMD58 							58
#define CMD58_R 						R3

/* 使能或禁止CRC */
#define CMD59 							59
#define CMD59_R 						R1

/************************************ 应用命令  ********************************/
/* 获取 SD Status寄存器 */
#define ACMD13 							13
#define ACMD13_R 						R2

/* 获取已写入卡中的块的个数 */
#define ACMD22 							22
#define ACMD22_R 						R1

/* 在写之前,设置预先擦除的块的个数  */
#define ACMD23 							23
#define ACMD23_R 						R1

/* 启动此卡的初始化过程  */
#define ACMD41 							41
#define ACMD41_R 						R1

/* 连接/断开CD/DATA[3]引脚上的上拉电阻  */
#define ACMD42 							42
#define ACMD42_R 						R1

/* 读取该卡的配置寄存器SCR  */
#define ACMD51 							51
#define ACMD51_R 						R1

/******************************************************************************/
/* 相关错误码定义 */
		
/* 错误码(error code) */
#define SD_NO_ERR			     		0x00					 /* 函数执行成功 */
#define SD_ERR_NO_CARD		 			0x60					 /* 卡没有完全插入到卡座中 */
#define SD_ERR_USER_PARAM      			0x61					 /* 用户使用API函数时，入口参数有错误 */
#define SD_ERR_CARD_PARAM		 		0x62					 /* 卡中参数有错误（与本模块不兼容） */
#define	SD_ERR_VOL_NOTSUSP        		0x63					 /* 卡不支持3.3V供电 */
#define SD_ERR_OVER_CARDRANGE			0x64					 /* 操作超出卡容量范围 */
#define SD_ERR_UNKNOWN_CARD       		0x65					 /* 无法识别卡型 */
#define SD_ERR_WP_PROTECT         		0x66            		 /* 写保护 */
#define SD_CARD_BUSY              		0x67            		 /* 在读取OCR寄存器时检测到BIT31为0表示忙 */
#define SD_BLOCK_ERROR            		0x68            		 /* 块大小一定要大于等于512,否则无法操作 */

/* SD命令可能返回的错误码 */
#define SD_ERR_CMD_RESPTYPE	 			0x69					 /* 命令类型错误 */
#define SD_ERR_CMD_TIMEOUT     			0x6A					 /* 卡命令响应超时 */
#define SD_ERR_CMD_RESP		 			0x6B					 /* 卡命令响应错误 */
#define SD_Power_Up_Busy          		0x6C					 /* 上电忙 */

/* 数据流错误码 */
#define SD_ERR_DATA_CRC16      			0x6D					 /* 数据流CRC16校验不通过 */
#define SD_ERR_DATA_START_TOK			0x6E					 /* 读单块或多块时,数据开始令牌不正确 */
#define	SD_ERR_DATA_RESP		 		0x6F					 /* 写单块或多块时,卡数据响应令牌不正确 */

/* 等待错误码 */
#define SD_ERR_TIMEOUT_WAIT    			0x70					 /* 写或擦操作时,发生超时错误 */
#define SD_ERR_TIMEOUT_READ    			0x71					 /* 读操作超时错误 */
#define	SD_ERR_TIMEOUT_WRITE	 		0x72					 /* 写操作超时错误 */
#define SD_ERR_TIMEOUT_ERASE   			0x73					 /* 擦除操作超时错误 */
#define	SD_ERR_TIMEOUT_WAITIDLE 		0x74					 /* 初始化卡时,等待卡退出空闲状态超时错误 */

/* 写操作可能返回的错误码 */
#define	SD_ERR_WRITE_BLK				0x75					 /* 写块数据错误 */
#define	SD_ERR_WRITE_BLKNUMS      		0x76				 	 /* 写多块时,想要写入的块与正确写入的块数不一致 */
#define SD_ERR_WRITE_PROTECT			0x77					 /* 卡外壳的写保护开关打在写保护位置 */

/* 使用UCOS-II时可能出现的错误 */
#define SD_ERR_CREATE_SEMSD				0xA0					 /* 创建SD/MMC卡访问信号量失败 */



/********************************************************************************/
/* 其它宏定义 */	
/********************************************************************************/
#define	CARDTYPE_SD						0x00					 /* 卡型为SD  卡 */
#define CARDTYPE_MMC					0x01					 /* 卡型为MMC 卡 */
#define SD_UCOSII_SMALLWAIT				0xFF					 /* 运行于UCOS-II时的小等待时间 */
#define SD_READREG_TIMEOUT				0x08					 /* 读寄存器的超时时间 */
		    	    	    
/* Mask off the bits in the OCR corresponding to voltage range 3.2V to 3.4V, OCR bits 20 and 21 */
#define MSK_OCR_33			 			0xC0

/* 定义在初始化阶段,等待SD卡退出空闲状态的重试次数 */
/* Number of tries to wait for the card to go idle during initialization */
#define SD_IDLE_WAIT_MAX     			10000

/* SD卡命令超时时间(timeout of command) */
#define SD_CMD_TIMEOUT   	 			0x0FFFF
	
/* 100ms 相当的SPI时钟数(单位 unit: 8 clocks) */
/* 100ms correspond to SPI clock(unit: 8 clocks)*/
#define READ_TIMEOUT_100MS      		100 * SPI_CLOCK / 1000 / 8

/* 250ms 相当的SPI时钟数(单位 unit: 8 clocks) */
/* 250ms correspond to SPI clock(unit: 8 clocks)*/
#define WRITE_TIMEOUT_250MS				250 * SPI_CLOCK / 1000 / 8    


/* CSD中一些域的字节位置(高字节在前) */
#define TAAC_POS 						1						 /* TACC */
#define NSAC_POS						2						 /* NSAC */
#define READ_BL_LEN_POS					5						 /* READ_BL_LEN */
#define C_SIZE_POS1						6						 /* C_SIZE upper  2-bit */
#define C_SIZE_POS2						7						 /* C_SIZE middle 8-bit */
#define C_SIZE_POS3						8						 /* C_SIZE lower	2-bit */
#define C_SIZE_MULT_POS1				9						 /* C_SIZE_MULT upper 2-bit */
#define C_SIZE_MULT_POS2				10						 /* C_SIZE_MULT lower 1-bit */	
#define SECTOR_SIZE_POS1				10						 /* SECTOR_SIZE upper 5-bit */
#define SECTOR_SIZE_POS2				11						 /* SECTOR_SIZE lower 2-bit */
#define R2WFACTOR_POS 					12						 /* R2WFACTOR_POS */

/* MMC擦除相关定义 */
#define ERASE_GRP_SIZE_POS   			10						 /* MMC卡 ERASE_GRP_SIZE 5-bit */
#define ERASE_GRP_MULTI_POS1 			10						 /* MMC卡 ERASE_GRP_MULTI 2-bit */
#define ERASE_GRP_MULTI_POS2 			11						 /* MMC卡 ERASE_GRP_MULTI 3-bit */

/* CSD中一些域的掩码 */
#define TAAC_MSK						0x07					 /* TACC 域掩码 */
#define NSAC_MSK						0x78					 /* NSAC 域掩码 */
#define READ_BL_LEN_MSK				 	0x0F				 	 /* READ_BL_LEN 的掩码 */
#define C_SIZE_MSK1						0x03					 /* C_SIZE 高2位掩码 */
#define C_SIZE_MSK3						0xC0					 /* C_SIZE 低2位掩码 */
#define C_SIZE_MULT_MSK1 				0x03					 /* C_SIZE_MULT 的高2位掩码 */
#define C_SIZE_MULT_MSK2 				0x80					 /* C_SIZE_MULT 的低2位掩码 */
#define R2WFACTOR_MSK					0x1C					 /* R2WFACTOR 掩码 */
#define SECTOR_SIZE_MSK1				0x3F					 /* SECTOR_SIZE 的高5位 */
#define SECTOR_SIZE_MSK2				0x80					 /* SECTOR_SIZE 的低2位 */
#define ERASE_GRP_SIZE_MSK 				0x7C					 /* MMC卡 ERASE_GRP_SIZE 掩码 */
#define ERASE_GRP_MULTI_MSK1 			0x03					 /* MMC卡 ERASE_GRP_MULTI 高2位掩码 */
#define ERASE_GRP_MULTI_MSK2 			0xE0				  	 /* MMC卡 ERASE_GRP_NULTI 低3位掩码 */
#define SD_BLOCK_LEN       				512						 /* 设置SD块大小 */


void SD_Port_Init( void );

UINT8 SD_Init( void );

UINT8 SD_ReadMultiBlock( UINT32 startadress, UINT8 *buf, UINT32 blockmun );

UINT8 SD_WriteMultiBlock( UINT32 startadress, UINT8 *buf, UINT32 blockmun );

#endif
/*********************************** endfile **********************************/
