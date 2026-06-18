/********************************** (C) COPYRIGHT *******************************
* File Name          : FLASH.C
* Author             : WCH
* Version            : V1.5
* Date               : 2023/05/31
* Description        : CH555 Flash字节读写函数定义
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "Flash.H"

#pragma  NOAREGS

/*
 * @Note
 * The following Flash Operation Flags need to be assigned specific values before 
 * erasing or programming Flash, and to be cleared after the operations are done, 
 * which can prevent misoperation of Flash.
 */
UINT8 Flash_Op_Check_Byte1 = 0x00;
UINT8 Flash_Op_Check_Byte2 = 0x00;

/*******************************************************************************
* Function Name  : Flash_Op_Unlock
* Description    : Flash��������
* Input          : flash_type: bCODE_WE(Code Flash); bDATA_WE(Data Flash) 
* Output         : None
* Return         : 0xFF(Flash Operation Flags Error)/0x00(Flash Operation Flags Correct)
*******************************************************************************/
UINT8 Flash_Op_Unlock( UINT8 flash_type )
{
    bit ea_sts;
    
    /* Check the Flash operation flags to prevent Flash misoperation. */
    if( ( Flash_Op_Check_Byte1 != DEF_FLASH_OP_CHECK1 ) ||
        ( Flash_Op_Check_Byte2 != DEF_FLASH_OP_CHECK2 ) )
    {
        return 0xFF;                                                           /* Flash Operation Flags Error */
    }
    
    /* Disable all INTs to prevent writing GLOBAL_CFG from failing in safe mode. */
    ea_sts = EA;                                
    EA = 0;
    
    /* Enable Flash writing operations. */
    SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG |= flash_type;
    SAFE_MOD = 0x00;

    /* Restore all INTs. */
    EA = ea_sts;
    
    return 0x00;
}

/*******************************************************************************
* Function Name  : Flash_Op_Lock
* Description    : Flash��������
* Input          : flash_type: bCODE_WE(Code Flash)/bDATA_WE(Data Flash) 
* Output         : None
* Return         : None
*******************************************************************************/
void Flash_Op_Lock( UINT8 flash_type )
{
    bit ea_sts;
    
    /* Disable all INTs to prevent writing GLOBAL_CFG from failing in safe mode. */
    ea_sts = EA;                                
    EA = 0;
    
    /* Disable Flash writing operations. */
    SAFE_MOD = 0x55;
	SAFE_MOD = 0xAA;
	GLOBAL_CFG &= ~flash_type;
    SAFE_MOD = 0x00;
    
    /* Restore all INTs. */
    EA = ea_sts;
}

/*******************************************************************************
* Function Name  : FlashErasePage
* Description    : 用于页擦除，每64字节为一页。将页内所有数据变为0x00
* Input          : Addr:擦除地址所在页
* Return         : status：返回操作状态
*                   0x00-成功  
*                   0x01-地址无效  
*                   0x02-未知命令或超时
*******************************************************************************/
UINT8 FlashErasePage(UINT16 Addr)
{
	UINT8 status;                                    /* 返回操作状态 */
	UINT8 FlashType;                                 /* Flash 类型标志 */
	
    if( ( Flash_Op_Check_Byte1 != DEF_FLASH_OP_CHECK1 ) ||                     
    ( Flash_Op_Check_Byte2 != DEF_FLASH_OP_CHECK2 ) )
    {
        return( 0xFF );
    }
    
	Addr &= 0xFFC0;                                  /* 64字节对齐 */
	if((Addr>=DATA_FLASH_ADDR) && (Addr<BOOT_LOAD_ADDR))/* DataFlash区域 */
	{
		FlashType = bDATA_WE;
	}
	else                                             /* CodeFlash区域 */
	{
		FlashType = bCODE_WE;
	}
    
    if( Flash_Op_Unlock( FlashType ) )               
    {
        return( 0xFF );
    }
    
	ROM_ADDR = Addr;                                 /* 写入目标地址 */
	ROM_BUF_MOD = bROM_BUF_BYTE;                     /* 选择块擦除模式或单字节编程模式 */
	ROM_DAT_BUF = 0;                                 /* 擦写数据缓冲区寄存器为0 */
	if (ROM_STATUS & bROM_ADDR_OK)                 /* 操作地址有效 */
	{
		ROM_CTRL = ROM_CMD_ERASE;                    /* 启动擦除 */
		if(ROM_STATUS & bROM_CMD_ERR)
		{
			status = 0x02;                           /* 未知命令或超时 */
		}
		else
		{
			status = 0x00;                           /* 操作成功 */
		}
	}
	else
	{
		status = 0x01;                               /* 地址无效 */
	}
    Flash_Op_Lock( FlashType );
	
	return status;
}

/*******************************************************************************
* Function Name  : FlashProgByte
* Description    : Flash 字节编程
* Input          : Addr：写入地址
*                  Data：字节编程的数据
* Return         : status：编程状态返回 
*                   0x00-成功  
*                   0x01-地址无效  
*                   0x02-未知命令或超时
*******************************************************************************/
UINT8 FlashProgByte(UINT16 Addr,UINT8 Data)
{
	UINT8 status;                                    /* 返回操作状态 */
	UINT8 FlashType;                                 /* Flash 类型标志 */
	
	if((Addr>=DATA_FLASH_ADDR) && (Addr<BOOT_LOAD_ADDR))/* DataFlash区域 */
	{
		FlashType = bDATA_WE;
	}
	else                                             /* CodeFlash区域 */
	{
		FlashType = bCODE_WE;
	}

    if( Flash_Op_Unlock( FlashType ) )               /* Flash�������� */
    {
        return( 0xFF );
    }
    
	ROM_ADDR = Addr;                                 /* 写入目标地址 */
	ROM_BUF_MOD = bROM_BUF_BYTE;                     /* 选择块擦除模式或单字节编程模式 */
	ROM_DAT_BUF = Data;                              /* 数据缓冲区寄存器 */
	if (ROM_STATUS & bROM_ADDR_OK)                 /* 操作地址有效 */
	{
		ROM_CTRL = ROM_CMD_PROG ;                    /* 启动编程 */
		if(ROM_STATUS & bROM_CMD_ERR)
		{
			status = 0x02;                           /* 未知命令或超时 */
		}
		else
		{
			status = 0x00;                           /* 操作成功 */
		}
	}
	else
	{
		status = 0x01;                               /* 地址无效 */
	}
    Flash_Op_Lock( FlashType );
	
	return status;
}

/*******************************************************************************
* Function Name  : FlashProgPage
* Description    : 页编程,仅编程当前Addr所在页。
* Input          : Addr：写入地址
*                  Buf：Buf地址的低6位要与Addr地址低6位相等，也就是（Buf地址%64）与页内偏移地址要相同
*                  len: 写入长度，最大64
* Return         : status：编程状态返回 
*                   0x00-成功  
*                   0x01-地址无效  
*                   0x02-未知命令或超时
*                   0xFC-起始地址加上本次写的字节数超出当前页
*                   0xFB-buf地址低6位与起始地址低6位不相等
*******************************************************************************/
UINT8 FlashProgPage(UINT16 Addr, PUINT8X Buf,UINT8 len)
{
	UINT8 status;                                    /* 返回操作状态 */
	UINT8 FlashType;                                 /* Flash 类型标志 */
	UINT8 page_offset;                               /* Addr在当前页的偏移地址 */
	
	if((Addr>=DATA_FLASH_ADDR) && (Addr<BOOT_LOAD_ADDR))/* DataFlash区域 */
	{
		FlashType = bDATA_WE;
	}
	else                                             /* CodeFlash区域 */
	{
		FlashType = bCODE_WE;
	}
    
	page_offset = Addr & MASK_ROM_ADDR;
	if (len > (ROM_PAGE_SIZE - page_offset))
	{
		return 0xFC;                              /* 起始地址加上本次写的字节数不能超出当前页, 每64字节为一页, 单次操作不得超出当前页 */
	}
	if (((UINT8)Buf & MASK_ROM_ADDR) != page_offset)
	{
		return 0xFB;                              /* xdata缓冲区地址低6位必须与起始地址低6位相等 */
	}
    
    if( Flash_Op_Unlock( FlashType ) )            /* Flash�������� */
    {
        return( 0xFF );
    }
    
	ROM_ADDR = Addr;
	ROM_BUF_MOD = page_offset + len - 1;             /* 页编程结束地址低6位，含改地址 */
	DPL = (UINT8)Buf;
	DPH = (UINT8)( (UINT16)Buf >> 8 );
	if (ROM_STATUS & bROM_ADDR_OK)                 /* 操作地址有效 */
	{
		ROM_CTRL = ROM_CMD_PROG ;                    /* 启动编程 */
		if(ROM_STATUS & bROM_CMD_ERR)
		{
				status = 0x02;                           /* 未知命令或超时 */
		}
		else
		{
				status = 0x00;                           /* 操作成功 */
		}
	}
	else
	{
		status = 0x01;                               /* 地址无效 */
	}
    Flash_Op_Lock( FlashType );
	
	return status;
}

/*******************************************************************************
* Function Name  : FlashReadBuf
* Description    : 读Flash（包含data和code）
* Input          : Addr：读出地址
*                  buf：读出数据
*                  len：读出长度
* Return         : i:返回实际读出长度
*******************************************************************************/
UINT8 FlashReadBuf(UINT16 Addr,PUINT8 buf,UINT16 len)
{
	UINT16 i;
    
	for(i=0; i!=len; i++)
	{
		buf[i] = *(PUINT8C)Addr;
		if(Addr==0xFFFF)
		{
			i++;
			break;
		}
		Addr++;
	}
	return i;
}

/*******************************************************************************
* Function Name  : FlashProgOTPbyte
* Description    : 单字节写OTP区域，只能0变成1,且不可擦除
* Input          : Addr：0x20~0x3F
*                  Data:
* Return         : status:操作状态 
*                   0x00-成功  
*                   0x02-未知命令或超时
*******************************************************************************/
UINT8 FlashProgOTPbyte(UINT8 Addr, UINT8 Data)
{
	UINT8 status;                                    /* 返回操作状态 */
	
	if( Flash_Op_Unlock( bDATA_WE ) )                /* Flash�������� */
    {
        return( 0xFF );
    }
	ROM_ADDR = Addr;
	ROM_BUF_MOD = bROM_BUF_BYTE;
	ROM_DAT_BUF = Data;
	ROM_CTRL = ROM_CMD_PG_OTP;
	if(ROM_STATUS & bROM_CMD_ERR)
	{
		status = 0x02;                               /* 未知命令或超时 */
	}
	else
	{
		status = 0x00;                               /* 操作成功 */
	}
    Flash_Op_Lock( bDATA_WE );
	
	return status;
}

/*******************************************************************************
* Function Name  : FlashReadOTPword
* Description    : 4字节为单位读取ReadOnly区或者OTP区
* Input          : Addr：0x00~0x3F
* Return         : temp：读取的四字节数据
*******************************************************************************/
UINT32 FlashReadOTPword(UINT8 Addr)
{
	UINT32 temp;
	
	ROM_ADDR = Addr;
	ROM_CTRL = ROM_CMD_RD_OTP;
	temp = ROM_DATA_HI;
	temp <<= 16;
	temp |= ROM_DATA_LO;
	return temp;
}
