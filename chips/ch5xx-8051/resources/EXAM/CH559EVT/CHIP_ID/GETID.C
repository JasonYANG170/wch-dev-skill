
/********************************** (C) COPYRIGHT *******************************
* File Name          :GETID.C
* Author             : WCH
* Version            : V1.3
* Date               : 2016/06/24
* Description        :获取芯片唯一ID号和ID号校验功能
                      ROM_CHIP_ID_ADDR起始4字节ID号，接下来2字节是ID和校验
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/

#include "..\DEBUG.C"                                                       //调试信息打印
#include "..\DEBUG.H"
#include <stdio.h>
#include <string.h>

#pragma NOAREGS

#define ROM_CHIP_ID_ADDR 0x20

/*******************************************************************************
* Function Name  : GetChipID(void)
* Description    : 获取ID号和ID号和校验
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
UINT32 GetChipID( void )
{
	UINT8	d0, d1;
	UINT16	xl, xh;
	E_DIS = 1;                                                                  //避免进入中断
	d0 = *(PUINT8C)( ROM_CHIP_ID_ADDR + 0 );
	d1 = *(PUINT8C)( ROM_CHIP_ID_ADDR + 1 );                                    //ID号低字
	xl = ( d1 << 8 ) | d0;
	d0 = *(PUINT8C)( ROM_CHIP_ID_ADDR + 2 );
	d1 = *(PUINT8C)( ROM_CHIP_ID_ADDR + 3 );                                    //ID号高字
	xh = ( d1 << 8 ) | d0;
	d0 = *(PUINT8C)( ROM_CHIP_ID_ADDR + 6 );
	d1 = *(PUINT8C)( ROM_CHIP_ID_ADDR + 7 );                                    //ID校验和
	E_DIS = 0;
	if ( (UINT16)( xl + xh ) != (UINT16)( ( d1 << 8 ) | d0 ) ) return( 0xFFFFFFFF );//校验ID号
	return( ( (UINT32)xh << 16 ) | xl );
}

/*******************************************************************************
* Function Name  : CopyChipID(void)
* Description    : 获取ID号，因为Flash双字节访问，低字节在前，使用时要注意
* Input          : PUINT32X buf
* Output         : None
* Return         : None
*******************************************************************************/
void CopyChipID( PUINT32X buf )
{
	E_DIS = 1;
	*( (PUINT16X)buf + 0 ) = *(const unsigned short code *)( ROM_CHIP_ID_ADDR + 0 );
	*( (PUINT16X)buf + 1 ) = *(const unsigned short code *)( ROM_CHIP_ID_ADDR + 2 );
	E_DIS = 0;
}

void main()
{
    UINT32 x;
//  CfgFsys( );    
//  mDelaymS(5);                                                               //等待外部晶振稳定
    
    mInitSTDIO( );                                                             //串口0,可以用于调试
    printf( "Start @ChipID=%02X\n", (UINT16)CHIP_ID );	
    printf("ID+CRC:%lx\n",GetChipID());
    CopyChipID(&x);
    printf("ID:%lx\n",x);	
    while(1);
}