/********************************** (C) COPYRIGHT *******************************
* File Name          : Main.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/09
* Description        : CH549 GPIO
                       需要包含DEBUG.C
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
********************************************************************************/
#include ".\Public\CH549.H"
#include ".\Public\DEBUG.H"
#include ".\GPIO\GPIO.H"
#pragma  NOAREGS
sbit LED2 = P2^2;
sbit LED3 = P2^3;
sbit LED4 = P2^4;
sbit LED5 = P2^5;
void main()
{
    CfgFsys( );                                                               //CH549Ê±ÖÓÑ¡ÔñÅäÖÃ
    mDelaymS(20);
    mInitSTDIO( );                                                            //����0��ʼ��
    printf("GPIO demo start ...\n");
    /* ����GPIO */
    GPIO_Init( PORT1,PIN0,MODE3);                                              //P1.0��������
    GPIO_Init( PORT1,PIN4,MODE1);                                              //P1.4�������
    /* �����ⲿ�ж� */
    GPIO_Init( PORT0,PIN3,MODE3);                                              //P03��������
    GPIO_Init( PORT1,PIN5,MODE3);                                              //P15��������
    GPIO_Init( PORT3,PIN2,MODE3);                                              //P32(INT0)��������
    GPIO_Init( PORT3,PIN3,MODE3);                                              //P33(INT1)��������
    GPIO_INT_Init( (INT_P03_L|INT_P15_L|INT_INT0_L|INT_INT1_L),INT_EDGE,Enable); //�ⲿ�ж�����
    while(1)
    {
        LED2 = ~LED2;
        LED3 = ~LED3;
        LED4 = ~LED4;
        LED5 = ~LED5;
        mDelaymS(100);
    }
}
