
/********************************** (C) COPYRIGHT *******************************
* File Name          : DataFlash.C
* Author             : WCH
* Version            : V1.7
* Date               : 2023/05/31
* Description        : CH554 DataFlash字节读写函数定义   
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#include "..\Public\CH554.H"                                                         
#include "..\Public\Debug.H"
#include "DataFlash.H"

/*
 * @Note
 * Whenever programming on board or downloading program via UART or USB, 5V supply  
 * voltage must be used temporarily. 
 */

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
* Function Name  : WriteDataFlash(UINT8 Addr,PUINT8 buf,UINT8 len)
* Description    : DataFlashд
* Input          : UINT8 Addr��PUINT16 buf,UINT8 len
* Output         : None
* Return         : UINT8 i ����д�볤��
*******************************************************************************/
UINT8 WriteDataFlash(UINT8 Addr,PUINT8 buf,UINT8 len)
{
    UINT8 i;
    
    if( Flash_Op_Unlock( bDATA_WE ) )                                         
    {
        return( 0xFF );
    }
    
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    Addr <<= 1;
    for(i=0;i<len;i++)
	{
        ROM_ADDR_L = Addr + i*2;
        ROM_DATA_L = *(buf+i);			
        if ( ROM_STATUS & bROM_ADDR_OK ) {                                     // 操作地址有效
           ROM_CTRL = ROM_CMD_WRITE;                                           // 写入
        }
        if((ROM_STATUS ^ bROM_ADDR_OK) > 0) return i;                          // 返回状态,0x00=success,  0x02=unknown command(bROM_CMD_ERR)
	}
    Flash_Op_Lock( bDATA_WE );
    
    return i;		
}

/*******************************************************************************
* Function Name  : ReadDataFlash(UINT8 Addr,UINT8 len,PUINT8 buf)
* Description    : 读DataFlash
* Input          : UINT8 Addr UINT8 len PUINT8 buf
* Output         : None
* Return         : UINT8 i 返回写入长度
*******************************************************************************/
UINT8 ReadDataFlash(UINT8 Addr,UINT8 len,PUINT8 buf)
{
    UINT8 i;
    ROM_ADDR_H = DATA_FLASH_ADDR >> 8;
    Addr <<= 1;
    for(i=0;i<len;i++){
	  ROM_ADDR_L = Addr + i*2;                                                   //Addr必须为偶地址
	  ROM_CTRL = ROM_CMD_READ;
//     if ( ROM_STATUS & bROM_CMD_ERR ) return( 0xFF );                        // unknown command
	  *(buf+i) = ROM_DATA_L;
		}
    return i;
}