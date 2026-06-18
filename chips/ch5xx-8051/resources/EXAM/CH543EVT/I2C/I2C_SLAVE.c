/********************************** (C) COPYRIGHT *******************************
* File Name          : I2C_SLAVE.C
* Author             : WCH
* Version            : V1.0
* Date               : 2022/01/13
* Description        : CH543 I2C Slave 中断例程

主机发送:支持多字节发送数据,时序等同于24CXX系列Page Write,数据地址起始地址0x00
        START + 器件地址(0x56) + 数据地址(0x00) + 数据*(N) + STOP

主机读取:支持多字节读取数据，时序等同于24CXX系列Random Read,数据地址起始地址为0x00
        START + 器件地址(0x56) + 数据地址(0x00) + START + 器件地址(0x57) + 数据*(N) + STOP
  SCL  ->  P1.6
  SDA  ->  P1.7
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/


/******************************************************************************/
/* 头文件包含 */
#include ".\Public\CH543.H"
#include ".\Public\DEBUG.H"
#pragma  NOAREGS

/******************************************************************************/
/* 常、变量定义 */
UINT8X I2CS_Buff[ 32 ] = { 0 };
UINT8X I2CS_Buff_S[ 32 ] = { 0 };
UINT16 Recv_len = 0,I2CS_Flag = 0,Recv_Flag = 0;

/*******************************************************************************
* Function Name  : I2CS_Slave_Init
* Description    : 硬件I2CS初始化函数
* Input          : None
* Return         : None
*******************************************************************************/
void I2CS_Slave_Init( void )
{
    UINT8 i;
    
    for( i = 0; i < 32; i++ )
    {
        I2CS_Buff[i] = 0xFF;
        I2CS_Buff_S[i] = i | 0x80;
    }
    
    P1_MOD_OC |= 0xC0;                                                          /* 开漏输出 */
    P1_DIR_PU |= 0xC0;                                                          /* 使能上拉 */
    I2CS_DEV_A = 0x56;                                                          /* 从机设备地址 */
    I2CS_CTRL = bI2CS_IE_READ | bI2CS_IE_WRITE | bI2CS_IE_ADDR | bI2CS_IE_STASTO | bI2CS_EN;
    IE_I2C = 1;
}

/*******************************************************************************
* Function Name  : I2C_INT( )
* Description    : IIC 中断服务函数
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void I2C_INT( void ) interrupt INT_NO_I2C using 1
{
    if( I2CS_STAT & bI2CS_IF_DEV_A )
    {
        Recv_len = 0;
        Recv_Flag = 0;
        I2CS_Flag = 0;
    }
    if( ( I2CS_STAT & MASK_I2CS_STAT ) == 3)                                    /* 接收到数据单元地址 */    
    {
        I2CS_DATA = *( I2CS_Buff_S + I2CS_ADDR );
    }
    else if( ( I2CS_STAT & MASK_I2CS_STAT ) == 5 )                              /* 接收到数据字节 */   
    {
        I2CS_Buff[ I2CS_ADDR ] = I2CS_DATA;        
        Recv_Flag = 1;
        Recv_len++;
    }
    else if( ( I2CS_STAT & MASK_I2CS_STAT ) == 7 )
    {
        I2CS_DATA = *( I2CS_Buff_S + I2CS_ADDR );
    }
    else if( (I2CS_STAT & MASK_I2CS_STAT) == 0x0C )                             /*接收到stop 一次传输完成*/
    {
        I2CS_Flag = 1; 
    }
    I2CS_STAT |= 0xF0;                                                          /* 清I2CS中断 */
}

/*******************************************************************************
* Function Name  : main
* Description    : 主程序
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void main( void )
{
    UINT8 i;
    
    /*****************************系统相关初始化*******************************/
    CfgFsys( );
    mDelaymS( 20 );                                                             /* 调整主频，建议稍加延时等待内部时钟稳定 */
    mInitSTDIO( );                                                              /* 串口0初始化 */
    
    /* IIC从机初始化 */ 
    I2CS_Slave_Init( );
    printf("I2CS start ...\n");
    
    EA = 1;
    E_DIS = 0;
    
    /*****************************主循环***************************************/
    while( 1 )
    {
        
        /* 中断方式收发,应根据实际应用进行优化 */
		if(( I2CS_Flag == 1 ) && ( Recv_Flag == 1 ) )
		{
			I2CS_Flag = 0;
			Recv_Flag = 0;
            printf("len %d\n",(UINT16)Recv_len);
            for ( i = 0; i < 32; i++ )
            {
                printf("%02x ",(UINT16)I2CS_Buff[i]);
            }
            printf("\n");
            for ( i = 0; i < 32; i++ )
            {
                I2CS_Buff[i] = 0xFF;
            }
		}
    }
}
