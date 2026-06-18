/********************************** (C) COPYRIGHT *******************************
* File Name          : SYSFREQ.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : application system clock configuration, before main program or on the startup program
*******************************************************************************/



/******************************************************************************/
#include "CH563SFR.H"

//#define FREQ_SYS            100000000                 // define frequency requested by application, then auto calculation by this file
// support: 50000000,62500000,75000000,83333333,100000000,110000000,112500000,116666666,125000000,137500000,150000000,etc.

#include "SYSFREQ.H"

/* PLL config */

void SysFreq( void )
{
    R8_SAFE_ACCESS_SIG = 0x57 ;  // unlock step 1
    R8_SAFE_ACCESS_SIG = 0xA8 ;  // unlock step 2
#if ( FREQ_OSC < 20000000 || FREQ_ETH_XT )
    R8_PLL_EXT_CFG = 0x40 | ( FREQ_OSC >= 20000000 ? RB_PLL_XT_FREQ : 0 ) | ( FREQ_ETH_XT ? RB_PLL_ETH_OSC : 0 ) ;
#endif
#if ( FREQ_VCO_MUL != 25 )
    R8_PLL_LOOP_DIV = FREQ_VCO_MUL ;
#endif
#if ( FREQ_OSC_DIV != 3 )
    R8_PLL_PRE_DIV = FREQ_OSC_DIV ;
#endif
#if ( FREQ_SYS_DIV != 4 || FREQ_VCO / 25000000 != 10 )
    R8_PLL_OUT_DIV = 0x80 | ( FREQ_SYS_DIV << 4 ) & RB_PLL_SYS_DIV | ( FREQ_VCO / 25000000 ) & RB_PLL_ETH_DIV ;
#endif
#if ( MEM_DATA != 32 )
    R8_GLOB_MEM_CFG = 0x80 | ( MEM_DATA == 96 ? 0x03 : ( MEM_DATA == 64 ? 0x00 : 0x02 ) ) ;
#endif
    R8_SAFE_ACCESS_SIG = 0x00 ;  // lock, to prevent unexpected writing
}

/*******************************************************************************
* Function Name  : Delay_us
* Description    : 延时函数(时钟为100MHZ时,延时精确)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define	DELAY_US_CLK_DIV	4100000
void Delay_us( int n )
{
	int i;
	
	while( n -- ) 
	{
		for( i = FREQ_SYS / DELAY_US_CLK_DIV; i != 0; -- i );
//		for( i = 0; i != 20; i++ );
	}
}

/*******************************************************************************
* Function Name  : Delay_ms
* Description    : 延时函数(时钟为100MHZ时,延时精确)
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void Delay_ms( int n )
{
	int i;
	
	while( n -- ) 
	{
		for( i = FREQ_SYS / ( DELAY_US_CLK_DIV / 1000 ); i != 0; -- i );
//		for( i = 0; i != 20000; i++ );
	}
}
