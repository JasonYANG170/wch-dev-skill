/********************************** (C) COPYRIGHT *********************************
* File Name          : SD_HW.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/05/15
* Description        : DEFINE FOR SDCARD.C
**********************************************************************************/
#ifndef __SD_HW_H__
#define __SD_HW_H__

#define SD_SPI1_SENDBYTE( d )    { R8_SPI1_FIFO = d;R16_SPI1_TOTAL_CNT = 0x01;\
                                   while( R8_SPI1_FIFO_COUNT != 0 ); }

#define SD_SPI1_RECVBYTE( d )    { R8_SPI1_FIFO = 0xFF;R16_SPI1_TOTAL_CNT = 0x01;\
                                   while( R8_SPI1_FIFO_COUNT != 0 );\
                                   d = R8_SPI1_BUFFER; }

#define		PIN_SD_SPI_SCS		( 1<<16 )		//PB16
#define		PIN_SD_SPI_SCK	    ( 1<<17 )		//PB17
#define		PIN_SD_SPI_MOSI		( 1<<18 )		//PB18
#define 	PIN_SD_SPI_MISO		( 1<<19 )		//PB19
#define		PIN_SD_INSERT		( 1<<1  )		//PB1	SD卡插入检测引脚
#define	    PIN_SD_WP	    	( 1<<14 )		//PD14	SD卡写保护检测引脚
//#define		PIN_SD_WP		    ( 1<<2  )

#define		SD_INSERT_PIN_IN( )	( R32_PB_PIN&0x02 )
#define	    SD_WP_PIN_IN( )		( R8_PD_PIN_1&0x40 )
//#define	SD_WP_PIN_IN( )		( R32_PB_PIN&0x04 )
#define		SD_CS_PIN_LOW( )	( R32_PB_CLR|=(1<<16) )
#define 	SD_CS_PIN_HIGH( )	( R32_PB_OUT|=(1<<16) )


/*以下为SD卡函数声明*/
extern void SD_Port_Init( void );				//SPI初始化
extern UINT8 SD_Init( void );					//SD卡初始化
extern UINT8 SD_WriteMultiBlock( UINT32 startadress, UINT8 *buf, UINT32 blockmun );	//写扇区函数
extern UINT8 SD_ReadMultiBlock( UINT32 startadress, UINT8 *buf, UINT32 blockmun );
extern UINT8 SD_Check_Insert( void );
extern volatile sd_struct  sds;										 				/* SD卡信息结构体定义 */

//extern UINT8 data_buf[48*1024];					//全局缓冲区


extern UINT8 SD_TYPE( void );
#endif
