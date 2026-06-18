/********************************** (C) COPYRIGHT *********************************
* File Name          : WAVE.H
* Author             : WCH
* Version            : V1.0
* Date               : 2017/3/8
* Description        : DEFINE FOR WAVE.C
**********************************************************************************/
#ifndef __WAVE_H
#define __WAVE_H

#include <stdio.h>
#include <string.h>
#include "CH561SFR.H"
#include "SYSFREQ.H"



void filehead_load(UINT8  *pfile);
UINT16 fetch2(UINT8  *paddr);
UINT32 fetch4(UINT8  *paddr);

#endif	//__WAVE_H
