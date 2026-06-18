/********************************** (C) COPYRIGHT *******************************
* File Name          : MAIN.C
* Author             : WCH
* Version            : V1.0
* Date               : 2021/07/16
* Description        : CH555 RGB演示 
  注意：包含 MAIN.C\RGB.C\RGBLOAD.ASM
	      在编译器A51选项中包含 CH555.INC
				
				评估板引脚连接：
           COMA   —— P36
					 COMB   —— P37
					 RED0   —— P40
					 GREEN0 —— P20
					 BLUE0  —— P10
					 RED1   —— P41
					 GREEN1 —— P21
					 BLUE1  —— P11
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "DEBUG.H"
#include "RGB.H"

/* Global Variable */ 
 
extern UINT16D	 BufferAddr;
extern UINT8X	RGBdataBuffer[128][4]; 
extern UINT8X	RGBdataBuffer1[128][4];

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Return         : None
*******************************************************************************/
void main(void)
{
	UINT8 i,RGB_stauts;
	signed char data incre=0;
	
	CfgFsys();
	mDelaymS(20);

	InitRGBLED();
	LED_RED_ADJ = 0;
	LED_GRE_ADJ = 0;
	LED_BLU_ADJ = 0;
	EA = 1;
	/*变量初始化*/     
	for(i=0; i<128; i++){
		RGBdataBuffer1[i][0]=0;
		RGBdataBuffer1[i][1]=0;//R
		RGBdataBuffer1[i][2]=0;//G
		RGBdataBuffer1[i][3]=0;//B
	}    
	
	mDelaymS(200);
	BufferAddr = (UINT16)(&RGBdataBuffer1[0]);

	//R
	for(i=0; i<128; i++){
		RGBdataBuffer[i][0]=127;
		RGBdataBuffer[i][1]=127;//R
		RGBdataBuffer[i][2]=0;//G
		RGBdataBuffer[i][3]=0;//B
	}  
	mDelaymS(200);
	BufferAddr = (UINT16)(&RGBdataBuffer[0]); 
	mDelaymS(200);  
	BufferAddr = (UINT16)(&RGBdataBuffer1[0]);  
	mDelaymS(200);  
	
	//G
	for(i=0; i<128; i++){
		RGBdataBuffer[i][0]=127;
		RGBdataBuffer[i][1]=0;//R
		RGBdataBuffer[i][2]=127;//G
		RGBdataBuffer[i][3]=0;//B
	}
	BufferAddr = (UINT16)(&RGBdataBuffer[0]); 
	mDelaymS(200); 
	BufferAddr = (UINT16)(&RGBdataBuffer1[0]);  
	mDelaymS(200); 
	
	//B
	for(i=0; i<128; i++){
		RGBdataBuffer[i][0]=127;
		RGBdataBuffer[i][1]=0;//R
		RGBdataBuffer[i][2]=0;//G
		RGBdataBuffer[i][3]=127;//B
	}
	BufferAddr = (UINT16)(&RGBdataBuffer[0]); 
	mDelaymS(200); 
	BufferAddr = (UINT16)(&RGBdataBuffer1[0]);  
	mDelaymS(200); 
	
	while(1){
		BufferAddr = (UINT16)(&RGBdataBuffer[0]);            
		if(incre >= 127)  RGB_stauts = 1;
		if(incre <= -127) RGB_stauts = 0;
		LED_RED_ADJ = RGB_stauts?(incre--):(incre++);
		LED_GRE_ADJ = RGB_stauts?(incre--):(incre++);
		LED_BLU_ADJ = RGB_stauts?(incre--):(incre++);
		mDelaymS(10);
	}
}