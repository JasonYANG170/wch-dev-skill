/********************************** (C) COPYRIGHT *******************************
* File Name          : MAIN.C
* Author             : WCH
* Version            : V1.5
* Date               : 2023/05/31
* Description        : CH545 DataFlash                       
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include "Flash.H"

/* Global Variable */ 
#define Data_Size   128
UINT8X buf[Data_Size] _at_ 0x0000;

/*******************************************************************************
* Function Name  : main
* Description    : Main program
* Input          : None
* Return         : None
*******************************************************************************/
void main(void) 
{
	UINT16 i;

	mDelaymS(50);
	CfgFsys();                                                                    
	mInitSTDIO();                                                             

	printf("CHIP_ID:%02x\n",(UINT16)CHIP_ID); 
	printf("EXAM DataFlash...\n");
	
	/* Erase dataflash */
    Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;
    Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
	for(i=0;i<(Data_Size/64);i++)
			FlashErasePage(DATA_FLASH_ADDR+i*64);
    Flash_Op_Check_Byte1 = 0x00;
    Flash_Op_Check_Byte2 = 0x00;
	
	/* Read dataflash */
	memset(buf,0x00,Data_Size); 
	printf("Read DataFlash\n");    
	FlashReadBuf(DATA_FLASH_ADDR+0,buf,Data_Size);
	for(i=0;i<Data_Size;i++)
			printf("addr:%04x Data:%02x\n",i,(UINT16)buf[i]); 
	
	/* WriteByte dataflash */
    Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;
    Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
	printf("WriteByte DataFlash:\n");
	for(i=0;i<Data_Size;i++){
			if( FlashProgByte(DATA_FLASH_ADDR+i,i&0xff)){
					printf("------------ FlashProgByte error\n");
			}
	}  
    Flash_Op_Check_Byte1 = 0x00;
    Flash_Op_Check_Byte2 = 0x00;

    /* Read dataflash */
	memset(buf,0x00,Data_Size);    
	printf("Read DataFlash\n");
	FlashReadBuf(DATA_FLASH_ADDR+0,buf,Data_Size);
	for(i=0;i<Data_Size;i++)
			printf("addr:%04x Data:%02x\n",i,(UINT16)buf[i]); 
	
	/* Erase dataflash */
    Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;
    Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
	for(i=0;i<(Data_Size/64);i++)
			FlashErasePage(DATA_FLASH_ADDR+i*64);
	
	/* WritePage dataflash */
	printf("WritePage DataFlash:\n");
	memset(buf,0x5A,64);
	if(FlashProgPage(DATA_FLASH_ADDR,buf,64))
	{
			printf("------------ FlashProgByte error:%x\n");
	}
    Flash_Op_Check_Byte1 = 0x00;
    Flash_Op_Check_Byte2 = 0x00;
	
	/* Read dataflash */
	memset(buf,0x00,Data_Size);    
	printf("Read DataFlash\n");
	FlashReadBuf(DATA_FLASH_ADDR+0,buf,Data_Size);
	for(i=0;i<Data_Size;i++)
			printf("addr:%04x Data:%02x\n",i,(UINT16)buf[i]);

	/* Read Config Information */
	printf("Config Information Read...\n");
	i = FlashReadBuf(0xFFFE,buf,2);
	printf("real len:%d\n",(UINT16)i);
	printf("cfg:%02x %02x\n",(UINT16)buf[1],(UINT16)buf[0]);

	/* ��ȡ�豸ΨһID��ֻ������ַ 10H~15H�� 16h~17H��У��� */
	printf("Unique ID...\n");
	printf("0x%lx ",FlashReadOTPword(0x14));                                 //17H,16H,15H,14H
	printf("0x%lx\n",FlashReadOTPword(0x10));   
	
	while(1);
}