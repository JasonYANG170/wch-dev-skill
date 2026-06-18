/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.5
* Date               : 2023/05/31
* Description        : CH549 FlashRom操作演示示例
                       1、DataFlash读写操作
                                 2、CodeFlash读写操作
                                 3、配置信息读取
                                 4、设备唯一ID读取
                                 5、OTP区读写
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\FlashRom\FlashRom.H"
#pragma  NOAREGS
UINT8X FlashBuf[64] _at_ 0x0000;                                               //对应于Flash页写的一个页,需要64字节对齐
UINT8X temp[64];
void main()
{
    UINT8 s,i,len;
    CfgFsys( );                                                                //CH549时钟选择配置
    mDelaymS(20);
    mInitSTDIO( );                                                             //串口0初始化
    printf("FlashRom demo start ...\n");
    for(i=0; i!=64; i++)
    {
        FlashBuf[i]=i;
    }
    /* 1、DataFlash 操作演示 */
    Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;
    Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
    memset(temp,0,64);
    printf("DataFlash Erase...\n");
    s = FlashErasePage( 0xF000 ) ;                                             //擦除DataFlash的第一页
    if(s!=0x00)
    {
        printf("%02x \n",(UINT16)s);
    }
    printf("DataFlash Write...\n");
    s = FlashProgPage( 0xF000, FlashBuf,64 );
    if(s!=0x00)
    {
        printf("%02x \n",(UINT16)s);
    }
    Flash_Op_Check_Byte1 = 0x00;
    Flash_Op_Check_Byte2 = 0x00;
    printf("DataFlash Read...\n");
    len = FlashReadBuf(0xF000,temp,64);
    printf("real len:%d\n",(UINT16)len);
    for(i=0; i!=len; i++)
    {
        printf("%02x ",(UINT16)temp[i]);
        if(temp[i]!=FlashBuf[i])
        {
            printf("Err Offset:%02x\n",i);
            while(1)
            {
                ;
            }
        }
    }
    printf("\n\n");
    
    /* 2、Code Flash 操作演示(BOOT区域用户程序无读写权限) */
    Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;
    Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
    memset(temp,0,64);
    printf("CodeFlash Erase...\n");
    s = FlashErasePage( 0xEFC0 ) ;                                             //擦除CodeFlash 0xEFC0所在页
    if(s!=0x00)
    {
        printf("%02x \n",(UINT16)s);
    }
    printf("CodeFlash Write...\n");
    s = FlashProgPage( 0xEFC0, &FlashBuf[0],64 );
    if(s!=0x00)
    {
        printf("%02x \n",(UINT16)s);
    }
    Flash_Op_Check_Byte1 = 0x00;
    Flash_Op_Check_Byte2 = 0x00;
    printf("CodeFlash Read...\n");
    len = FlashReadBuf(0xEFC0,temp,64);
    printf("real len:%d\n",(UINT16)len);
    for(i=0; i!=len; i++)
    {
        printf("%02x ",(UINT16)temp[i]);
        if(temp[i]!=FlashBuf[i])
        {
            printf("Err Offset:%02x\n",i);
            while(1)
            {
                ;
            }
        }
    }
    printf("\n\n");
    
    /* 3、16字节配置位读取 */
    printf("Config Information Read...\n");
    len = FlashReadBuf(0xFFFE,temp,2);
    printf("real len:%d\n",(UINT16)len);
    printf("cfg:%02x %02x\n",(UINT16)temp[1],(UINT16)temp[0]);
    
    /* 4、读取设备唯一ID（只读区地址 10H~15H） 16h~17H是校验和 */
    printf("Unique ID...\n");
    printf("0x%lx ",FlashReadOTPword( 0x14 ));                                 //17H,16H,15H,14H
    printf("0x%lx\n",FlashReadOTPword( 0x10 ));                                //13H,12H,11H,10H
    
#if 0
    /* 5、OTP 操作(只能0变成1,且不可擦除) */
    printf("OTP Read:");
    printf("0x%lx \n",FlashReadOTPword( 0x20 ));                               //读取0x20开始的四字节
    Flash_Op_Check_Byte1 = DEF_FLASH_OP_CHECK1;
    Flash_Op_Check_Byte2 = DEF_FLASH_OP_CHECK2;
    printf("OTP Write ...\n");
    s = FlashProgOTPbyte( 0x20, 0x4f );                                        //从0x20地址开始写入
    if(s!=0x00)
    {
        printf("s = %02x \n",(UINT16)s);
    }
    s = FlashProgOTPbyte( 0x21, 0x58 );
    if(s!=0x00)
    {
        printf("s = %02x \n",(UINT16)s);
    }
    s = FlashProgOTPbyte( 0x22, 0x54 );
    if(s!=0x00)
    {
        printf("s = %02x \n",(UINT16)s);
    }
    s = FlashProgOTPbyte( 0x23, 0x63 );
    if(s!=0x00)
    {
        printf("s = %02x \n",(UINT16)s);
    }
    Flash_Op_Check_Byte1 = 0x00;
    Flash_Op_Check_Byte2 = 0x00;
    printf("OTP Read:");
    printf("0x%lx \n",FlashReadOTPword( 0x20 ));                               //再次读取0x20开始的四字节
#endif
    while(1)
    {
    }
}
