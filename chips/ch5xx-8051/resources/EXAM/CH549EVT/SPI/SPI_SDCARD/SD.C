/********************************** (C) COPYRIGHT *******************************
* File Name          : SD.C
* Author             : WCH
* Version            : V1.0
* Date               : 2018/08/28
* Description        : CH549 SD卡操作读写等基本函数
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
/********************************* 引脚定义 ************************************
*    P14  <===========>  SCS
*    P15  <===========>  DI/MOSI
*    P16  <===========>  DO/MISO
*    P17  <===========>  SCK
*    P13  <===========>  C/D
*******************************************************************************/
#include ".\SPI\SPI_SDCARD\SD.H"
SD_INFO xdata sd_info;                                                         //SD卡信息
UINT8X param[4];                                                               //SD卡命令所带四字节参数
UINT8X resp[16];                                                               //SD卡相应
UINT32X tr;                                                                    //32位全局变量
/*******************************************************************************
* Function Name  : SPI_TransByte(UINT8 dat)
* Description    : SPI 收发一个字节
* Input          : UINT8 dat 写数据
* Output         : None
* Return         : 交换读出
*******************************************************************************/
UINT8 SPI_TransByte(UINT8 dat)
{
    SPI0_DATA = dat;
    while(S0_FREE == 0)
    {
        ;
    }
    return SPI0_DATA;
}
/*******************************************************************************
* Function Name  : SD_SendCmd
* Description    : 发送命令与响应
* Input          : cmd 命令码
*                  param 指向四字节参数
* Output         : resp 指向命令返回数据
* Return         : 操作状态
*******************************************************************************/
UINT8 SD_SendCmd(UINT8 cmd,UINT8X *param,UINT8X *resp )
{
    UINT8X s;
    SPI_TransByte(0x40 | (cmd&0x3f));
    SPI_TransByte(param[0]);
    SPI_TransByte(param[1]);
    SPI_TransByte(param[2]);
    SPI_TransByte(param[3]);
    if(cmd == CMD0)
    {
        SPI_TransByte(0x95);
    }
    else if(cmd == CMD8)
    {
        SPI_TransByte(0x87);
    }
    else
    {
        SPI_TransByte(0xFF);
    }
    if(cmd == CMD12)
    {
        SPI_TransByte(0xFF);    //发送完cmd12之后读到第一个字节需要丢弃
    }
    tr = 0;
    do
    {
        s = SPI_TransByte(0xff);
        tr++;
    }
    while(( ( s & 0x80 ) != 0x00 ) && ( tr < SD_CMD_TIMEOUT ));
    if(tr>=SD_CMD_TIMEOUT)
    {
        return (SD_ERR_CMD_TIMEOUT);                                           //响应超时
    }
    *resp = s;
    if(s&0x04)
    {
        return SD_CMD_UNSURPORT;                                               //不支持的命令
    }
    else if(s&0x40)
    {
        return SD_ERR_CMD_PARAM;    //参数错误
    }
    else if(s&0x08)
    {
        return SD_ERR_CMD_CRC;    //CRC错误
    }
    return SD_ERR_CMD_OK;
}
/*******************************************************************************
* Function Name  : POWER_ON
* Description    : SD卡上电,发送80个时钟
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void POWER_ON(void)
{
    SCS = 1;
    for(tr=0; tr!=10; tr++)
    {
        SPI_TransByte(0xFF);
    }
}
/*******************************************************************************
* Function Name  : SD_Reset
* Description    : SD卡复位命令 CMD0
* Input          : None
* Output         : resp 指向命令返回数据
* Return         : 命令执行状态
*******************************************************************************/
UINT8 SD_Reset(UINT8X *resp)
{
    UINT8X s;
    memset(param,0,4);
    SCS = 0;
    s = SD_SendCmd(CMD0,param,resp);
    SCS = 1;
    return s;
}
/*******************************************************************************
* Function Name  : SD_CHECK_VOLTAGE
* Description    : 对于初始化高容量卡，强制加入CMD8，用于验证操作条件
* Input          : None
* Output         : resp 指向命令返回数据
* Return         : 命令执行状态
*******************************************************************************/
UINT8 SD_CHECK_VOLTAGE(UINT8X *resp)
{
    UINT8X s;
    UINT8X res_r0;
    param[0] = 0;
    param[1] = 0;
    param[2] = 1;
    param[3] = 0xAA;
    SCS = 0;
    s = SD_SendCmd(CMD8,param,&res_r0);
    if(s==SD_ERR_CMD_OK)
    {
        for(tr=0; tr<4; tr++)
        {
            resp[tr]= SPI_TransByte(0xff);
        }
    }
    SCS = 1;
    return s;
}
/*******************************************************************************
* Function Name  : SD_GET_OCR
* Description    : 获取OCR，初始化SD卡，完成后，从Idle进入Ready状态
* Input          : None
* Output         : resp 指向命令返回数据
* Return         : 命令执行状态
*******************************************************************************/
UINT8 SD_GET_OCR(UINT8X *resp)
{
    UINT8X  s;
    UINT8X res_r0;
    memset(param,0,4);
    for(tr=0; tr!=SD_CMD_TIMEOUT; tr++)
    {
        memset(param,0,4);
        SCS = 0;
        s = SD_SendCmd(CMD55,param,&res_r0);
        SCS = 1;
        if(s==SD_ERR_CMD_OK)
        {
            if(sd_info.type==0x02)                                              //HCS(BIT30)需要置1
            {
                param[0]=0x40;
            }
            else                                                                //HCS应该设置为0
            {
                param[0]=0x00;
            }
            param[1]=0xff;
            param[2]=0x80;
            param[3]=0x00;
            SCS = 0;
            s = SD_SendCmd(ACMD41,param,&res_r0);
            SCS = 1;
            if(s==SD_ERR_CMD_OK)
            {
                if((res_r0 & 0x01)==0)                                          //退出Idle
                {
                    break;
                }
            }
            else if(s==SD_CMD_UNSURPORT)
            {
                if(sd_info.type==0x01)
                {
                    sd_info.type=0x00;                                         //可能是MMC卡
                    for(tr=0; tr!=SD_CMD_TIMEOUT; tr++)
                    {
                        memset(param,0,4);
                        SCS = 0;
                        s = SD_SendCmd(CMD1,param,&res_r0);                    //mmc卡的初始化从cmd1开始
                        SCS = 1;
                        if(s==SD_ERR_CMD_OK)
                        {
                            if((res_r0 & 0x01)==0)                             //退出Idle
                            {
                                break;
                            }
                        }
                    }
                }
                break;
            }
            else
            {
                return s;
            }
        }
        else
        {
            return s;
        }
    }
    memset(param,0,4);
    SCS = 0;
    s = SD_SendCmd(CMD58,param,&res_r0);                                       //执行CMD58获取OCR
    if(s == SD_ERR_CMD_OK)
    {
        for(tr=0; tr<4; tr++)
        {
            resp[tr]= SPI_TransByte(0xff);
        }
        if(resp[0]&0x80)
        {
            if(resp[0]&0x40)                                                   //分析CCS位，即只要type不是3，那CCS就是0
            {
                sd_info.type = 3;    //高容量卡
            }
        }
    }
    SCS = 1;
    return s;
}
/*******************************************************************************
* Function Name  : SD_GET_CSD
* Description    : 读取CSD寄存器(提供关于访问卡内容的信息)
* Input          : None
* Output         : resp 指向命令返回数据
* Return         : 命令执行状态
*******************************************************************************/
UINT8 SD_GET_CSD(UINT8X *resp)
{
    UINT8X  s;
    UINT8X res_r0;
    memset(param,0,4);
    SCS = 0;
    s = SD_SendCmd(CMD9,param,&res_r0);
    if(s!=SD_ERR_CMD_OK)
    {
        SCS = 1;
        return s;
    }
    tr= 0;
    do
    {
        s = SPI_TransByte(0xff);
        tr++;
    }
    while(( s != 0xFE ) && ( tr < SD_CMD_TIMEOUT ));
    if(tr>=SD_CMD_TIMEOUT)
    {
        SCS = 1;
        return (SD_ERR_CMD_TIMEOUT);                                          //响应超时
    }
    for(tr=0; tr<16; tr++)
    {
        resp[tr]= SPI_TransByte(0xff);
    }
    SPI_TransByte(0xff);                                                      //两个字节的CRC
    SPI_TransByte(0xff);
    SCS = 1;
    return SD_ERR_CMD_OK;
}
/*******************************************************************************
* Function Name  : SET_BLOCK_LEN
* Description    : 设置块长度512字节(注意高容量卡，强制为512字节)
* Input          : len 设置块长度
* Output         : None
* Return         : 命令执行状态
*******************************************************************************/
UINT8 SET_BLOCK_LEN(UINT32 len)
{
    UINT8X  s;
    UINT8X res_r0;
    param[0]=(UINT8)(len>>24&0xff);
    param[1]=(UINT8)(len>>16&0xff);
    param[2]=(UINT8)(len>>8&0xff);
    param[3]=(UINT8)(len&0xff);
    SCS = 0;
    s = SD_SendCmd(CMD16,param,&res_r0);
    if(res_r0!=0)
    {
        s = res_r0;    //返回res_r0
    }
    SCS = 1;
    return s;
}
/*******************************************************************************
* Function Name  : SD_HIGH_SPEED
* Description    : 切换到高速通讯模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SD_HIGH_SPEED(void)
{
    SPI_CK_SET(6);
}
/*******************************************************************************
* Function Name  : SD_Check_Insert
* Description    : 检测SD卡是否插入,插入则返回TRUE; 否则返回FALSE
* Input          : None
* Output         : None
* Return         : FALSE or TRUE
*******************************************************************************/
UINT8 SD_Check_Insert( void )
{
    if( SD_INSERT_PIN_IN( ) )                                                 //SD卡未插入
    {
        sd_info.insert = 0x00;
        return( FALSE );
    }
    else
    {
        mDelayuS( 1 );
        if( !SD_INSERT_PIN_IN( ) )                                            //SD卡插入
        {
            sd_info.insert = 0x01;
            return( TRUE );
        }
        else                                                                  //SD卡未插入
        {
            return( FALSE );
        }
    }
}
/*******************************************************************************
* Function Name  : SD_Init
* Description    : SD卡初始化
* Input          : None
* Output         : None
* Return         : SD_ERR_CMD_OK 成功； 其他
*******************************************************************************/
UINT8 SD_Init(void)
{
    UINT8X s;
    UINT16X tmp;
    if( SD_Check_Insert() == FALSE )
    {
        return SD_ERR_INSERT;
    }
    memset(&sd_info,0,sizeof(SD_INFO));
    SPIMasterModeSet(3);                                                      //SPI模式3，端口初始化
    SPI_CK_SET(64);                                                           //SPI分频,SD卡初始化时钟不超过400Khz
    POWER_ON();                                                               //上电
    /* 发送CMD0 */
#if DE_PRINTF
    printf("CMD0:\n");
#endif
    SD_Reset(&resp[0]);                                                       //卡复位,返回R0
    if(resp[0]!=0x01)
    {
        return SD_ERR_INIT;
    }
    /* 发送CMD8（验证 SD 卡接口操作条件）*/
#if DE_PRINTF
    printf("CMD8:\n");
#endif
    s = SD_CHECK_VOLTAGE(&resp[0]);                                           //返回长度4
    if(s==SD_CMD_UNSURPORT)
    {
        sd_info.type = 0x01;    //v1卡或mmc v3卡
    }
    else if(s==SD_ERR_CMD_OK)
    {
#if DE_PRINTF
        printf("%02x %02x %02x %02x\n",(UINT16)resp[0],(UINT16)resp[1],(UINT16)resp[2],(UINT16)resp[3]);
#endif
        if(resp[2]==0x01 && resp[3]==0xaa)                                    //电压必须是2.7-3.6
        {
            sd_info.type = 0x02;
        }
        else
        {
            sd_info.type = 0x04;
        }
    }
    else
    {
        return s;
    }
    /* 获取OCR寄存器（发完ACMD41之后进入Ready状态，以上都是Idle），初始化 */
#if DE_PRINTF
    printf("GET OCR:\n");
#endif
    SD_GET_OCR(&resp[0]);                                                    //返回4字节OCR
#if DE_PRINTF
    printf("Card Type:0x%02x\n",(UINT16)sd_info.type);
#endif
    /* 读取CSD（可选）CMD9 */
#if DE_PRINTF
    printf("GET CSD:\n");
#endif
    s = SD_GET_CSD(&resp[0]);                                                //返回16字节CSD
    if(s!=SD_ERR_CMD_OK)
    {
        return s;
    }
#if DE_PRINTF
    for(s=0; s!=16; s++)
    {
        printf("%02x ",(UINT16)resp[s]);
    }
    printf("\n");
#endif
    /* 对于SD卡容量的处理,第一版、第二版不一样 */
    if( ( resp[ 0 ] & 0x80 ) == 0x80 )                                       //CSD数据错误,容量清0
    {
        return( SD_ERR_VOL_NOTSUSP );
    }
    else
    {
        /* READ_BL_LEN: 位83--80: 1001------512byte */
        sd_info.block_len = 1 << ( resp[ 5 ] & 0x0F );
        if( sd_info.block_len < 512 )
        {
            return ( SD_BLOCK_ERROR );
        }
        if( ( resp[ 0 ] & 0x40 ) == 0x40  )                                  //CSD结构版本:第二版
        {
            /* 对于第二版计算容量方法:
               位83--80:------READ_BL_LEN:
               位69--48:------C_SIZE
               容量 = READ_BL_LEN * ( C_SIZE + 1 ) */
#if DE_PRINTF
            printf("csd struct v2\n");
#endif
            sd_info.block_num = ( ( UINT32 )( resp[ 7 ] & 0x3F ) ) << 16 ;
            sd_info.block_num += ( UINT32 )resp[ 8 ] << 8;
            sd_info.block_num += ( UINT32 )resp[ 9 ];
            sd_info.block_num += 1;
            sd_info.block_num = sd_info.block_num << 10;
            /* 计算总容量 */
            sd_info.capability = ( ( UINT32 ) sd_info.block_len ) * ( ( UINT32 )( sd_info.block_num / 1024 ) );
        }
        else                                                                 //CSD结构版本:第一版
        {
            /* 对于第一版计算容量方法:
               位83--80:------READ_BL_LEN:
               位73--62:------C_SIZE
               位49--47:------C_SIZE_MULT
               BLOCKNR = ( C_SIZE + 1 ) * MULT
               MULT = ( C_SIZE_MULT < 8 ) * 2 ^ ( C_SIZE_MULT + 2 )
               BLOCK_LEN = ( READ_BL_LEN < 12 ) * 2 ^ ( READ_BL_LEN )
               存储器容量 = BLOCKNR * BLOCK_LEN */
            /* 计算C_SIZE_MULT */
#if DE_PRINTF
            printf("csd struct v1\n");
#endif
            tmp  = ( resp[ 9 ] & 0x03 ) << 1;
            tmp += ( ( resp[ 10 ]& 0x80 ) >> 7 ) + 2;
            /* 计算C_SIZE */
            sd_info.block_num  = ( resp[ 6 ] & 0x03 ) << 10;
            sd_info.block_num += ( resp[ 7 ] << 2 );
            sd_info.block_num += ( ( resp[ 8 ] & 0xc0 ) >> 6 );
            /* 获得卡中块的数量 */
            sd_info.block_num = ( sd_info.block_num + 1 ) * ( 1 << tmp );
            /* 计算总容量 */
            sd_info.capability = ( ( UINT32 ) sd_info.block_len ) * ( ( UINT32 ) ( sd_info.block_num / 1024 ) );
        }
#if DE_PRINTF
        printf("sd_info.block_len = %d\n",(UINT16)sd_info.block_len);
        printf("sd_info.block_num = %ld\n",( UINT32 )sd_info.block_num );
        printf("capability =  %f G\n",( float )( ( ( float )( sd_info.capability / 1024 ) / 1024 )  )  );
#endif
    }
    /* 设置块长度CMD16 */
    s = SET_BLOCK_LEN(512);
    if(s!=SD_ERR_CMD_OK)
    {
        return s;
    }
    SD_HIGH_SPEED();                                                         //进入高速模式
    sd_info.init_success = 1;                                                //初始化完成
    return SD_ERR_CMD_OK;
}
/*******************************************************************************
* Function Name  : SD_READ_SECTOR
* Description    : 读一个扇区,对于“大容量”参数是扇区号，对于“小容量”参数是物理地址。
*                  为了统一，该函数输入的参数一律为扇区号，函数内分析是大卡还是小卡。
*                  根据是否支持CMD8来判断。
* Input          : sector_num 扇区号
* Output         : datbuf 指向读出缓冲区
* Return         : SD_ERR_CMD_OK 成功； 其他
*******************************************************************************/
UINT8 SD_READ_SECTOR(UINT8X *datbuf,UINT32 sector_num)
{
    UINT8X  s;
    UINT8X res_r0;
    if(sd_info.type!=3)
    {
        sector_num = sector_num<<9;    //扩大512倍
    }
    param[0]=(UINT8)(sector_num>>24&0xff);
    param[1]=(UINT8)(sector_num>>16&0xff);
    param[2]=(UINT8)(sector_num>>8&0xff);
    param[3]=(UINT8)(sector_num&0xff);
    SCS = 0;
    s = SD_SendCmd(CMD17,param,&res_r0);
    if(s!=SD_ERR_CMD_OK)
    {
        SCS = 1;
        return s;
    }
    tr= 0;
    do
    {
        s = SPI_TransByte(0xff);
        tr++;
    }
    while(( s != 0xFE ) && ( tr < SD_CMD_TIMEOUT ));
    if(tr>=SD_CMD_TIMEOUT)
    {
        SCS = 1;
        return (SD_ERR_CMD_TIMEOUT);                                         //响应超时
    }
    for(tr=0; tr<512; tr++)
    {
        *datbuf++= SPI_TransByte(0xff);
    }
    SPI_TransByte(0xff);                                                     //两个字节的CRC
    SPI_TransByte(0xff);
    SCS = 1;
    return SD_ERR_CMD_OK;
}
/*******************************************************************************
* Function Name  : SD_READ_MULT_SECTOR
* Description    : 读多个扇区，对于“大容量”参数是扇区号，对于“小容量”参数是物理地址。
*                  为了统一，该函数输入的参数一律为扇区号，函数内分析是大卡还是小卡。
* Input          : sector_num 扇区号
*                  blocks 扇区数
* Output         : datbuf 指向读出缓冲区
* Return         : SD_ERR_CMD_OK 成功； 其他
*******************************************************************************/
UINT8 SD_READ_MULT_SECTOR(UINT8X *datbuf,UINT32 sector_num,UINT16 blocks )
{
    UINT8X  s;
    UINT8X res_r0;
    if(sd_info.type!=3)
    {
        sector_num = sector_num<<9;    //扩大512倍
    }
    param[0]=(UINT8)(sector_num>>24&0xff);
    param[1]=(UINT8)(sector_num>>16&0xff);
    param[2]=(UINT8)(sector_num>>8&0xff);
    param[3]=(UINT8)(sector_num&0xff);
    SCS = 0;
    s = SD_SendCmd(CMD18,param,&res_r0);
    if(s!=SD_ERR_CMD_OK)
    {
        SCS = 1;
        return s;
    }
    while(blocks--)
    {
        tr= 0;
        do
        {
            s = SPI_TransByte(0xff);
            tr++;
        }
        while(( s != 0xFE ) && ( tr < SD_CMD_TIMEOUT ));
        if(tr>=SD_CMD_TIMEOUT)
        {
            SCS = 1;
            return (SD_ERR_CMD_TIMEOUT);                                     //响应超时
        }
        for(tr=0; tr<512; tr++)
        {
            *datbuf++= SPI_TransByte(0xff);
        }
        SPI_TransByte(0xff);                                                 //两个字节的CRC
        SPI_TransByte(0xff);
    }
    /* 发送CMD12终止传输 */
    memset(param,0,4);
    s = SD_SendCmd(CMD12,param,&res_r0);
    if(s!=SD_ERR_CMD_OK)
    {
        SCS =1;
        return s;
    }
    tr= 0;                                                                   //busy检测，DO信号线忙会持续低电平
    do
    {
        s = SPI_TransByte(0xff);
        tr++;
    }
    while(( s !=0xFF ) && ( tr < SD_CMD_TIMEOUT ));
    if(tr>=SD_CMD_TIMEOUT)
    {
        SCS = 1;
        return (SD_ERR_CMD_TIMEOUT);                                        //响应超时
    }
    SCS = 1;
    return SD_ERR_CMD_OK;
}
/*******************************************************************************
* Function Name  : SD_WRITE_SECTOR
* Description    : 写一个扇区，对于“大容量”参数是扇区号，对于“小容量”参数是物理地址。
*                  为了统一，该函数输入的参数一律为扇区号，函数内分析是大卡还是小卡。
* Input          : sector_num 扇区号
*                  datbuf 指向写入缓冲区
* Output         : None
* Return         : SD_ERR_CMD_OK 成功； 其他
*******************************************************************************/
UINT8 SD_WRITE_SECTOR(UINT8X *datbuf,UINT32 sector_num)
{
    UINT8X  s;
    UINT8X res_r0;
    if(sd_info.type!=3)
    {
        sector_num = sector_num<<9;    //扩大512倍
    }
    param[0]=(UINT8)(sector_num>>24&0xff);
    param[1]=(UINT8)(sector_num>>16&0xff);
    param[2]=(UINT8)(sector_num>>8&0xff);
    param[3]=(UINT8)(sector_num&0xff);
    SCS = 0;
    s = SD_SendCmd(CMD24,param,&res_r0);
    if(s!=SD_ERR_CMD_OK)
    {
        SCS =1;
        return s;
    }
    SPI_TransByte(0xFF);                                                       //至少发一个空闲数据，提供时钟，使卡准备好
    SPI_TransByte(0xFE);
    for(tr=0; tr!=32; tr++)
    {
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
        SPI_TransByte(*datbuf++);
    }
    SPI_TransByte(0xFF);                                                       //两个字节的CRC
    SPI_TransByte(0xFF);
    s = SPI_TransByte(0xFF);                                                   //Data Resp,不同于CMD Resp  XXX0(010)1:Accep  XXX0(101)1:CRC ERR  XXX0(110)1:WRITE ERR
    if((s&0x1F)!=5)                                                            //数据写入不正确
    {
        SCS = 1;
        return s;
    }
    tr=0;
    do                                                                         //忙等待
    {
        s = SPI_TransByte(0xFF);
        tr++;
    }
    while(( s !=0xFF ) && ( tr < SD_CMD_TIMEOUT ));
    if(tr>=SD_CMD_TIMEOUT)
    {
        SCS = 1;
        return (SD_ERR_CMD_TIMEOUT);                                           //响应超时
    }
    SCS = 1;
    return SD_ERR_CMD_OK;
}
/*******************************************************************************
* Function Name  : SD_WRITE_SECTOR
* Description    : 写多个扇区，对于“大容量”参数是扇区号，对于“小容量”参数是物理地址。
*                  为了统一，该函数输入的参数一律为扇区号，函数内分析是大卡还是小卡。
* Input          : sector_num 扇区号
*                  datbuf 指向写入缓冲区
*                  blocks 写入块数
* Output         : None
* Return         : SD_ERR_CMD_OK 成功； 其他
*******************************************************************************/
UINT8 SD_WRITE_MULT_SECTOR( UINT8X *datbuf,UINT32 sector_num,UINT16 blocks )
{
    UINT8X  s;
    UINT8X res_r0;
    if(sd_info.type!=3)
    {
        sector_num = sector_num<<9;    //扩大512倍
    }
    param[0]=(UINT8)(sector_num>>24&0xff);
    param[1]=(UINT8)(sector_num>>16&0xff);
    param[2]=(UINT8)(sector_num>>8&0xff);
    param[3]=(UINT8)(sector_num&0xff);
    SCS = 0;
    s = SD_SendCmd(CMD25,param,&res_r0);
    if(s!=SD_ERR_CMD_OK)
    {
        SCS = 1;
        return s;
    }
    SPI_TransByte(0xFF);                                                       //至少发一个空闲数据，提供时钟，使卡准备好
    while(blocks--)
    {
        SPI_TransByte(0xFC);
        for(tr=0; tr!=32; tr++)
        {
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
            SPI_TransByte(*datbuf++);
        }
        SPI_TransByte(0xFF);                                                   //两个字节的CRC
        SPI_TransByte(0xFF);
        s = SPI_TransByte(0xFF);                                               //Data Resp,不同于CMD Resp  XXX0(010)1:Accep  XXX0(101)1:CRC ERR  XXX0(110)1:WRITE ERR
        if((s&0x1F)!=5)                                                        //数据写入不正确
        {
            SCS = 1;
            return s;
        }
        tr=0;
        do                                                                     //忙等待
        {
            s = SPI_TransByte(0xFF);
            tr++;
        }
        while(( s !=0xFF ) && ( tr < SD_CMD_TIMEOUT ));
        if(tr>=SD_CMD_TIMEOUT)
        {
            SCS = 1;
            return (SD_ERR_CMD_TIMEOUT);                                       //响应超时
        }
    }
    SPI_TransByte(0xFD);                                                       //停止传输
    SPI_TransByte(0xFF);                                                       //多发8个时钟
    tr=0;
    do                                                                         //忙等待
    {
        s = SPI_TransByte(0xFF);
        tr++;
    }
    while(( s !=0xFF ) && ( tr < SD_CMD_TIMEOUT ));
    if(tr>=SD_CMD_TIMEOUT)
    {
        SCS = 1;
        return (SD_ERR_CMD_TIMEOUT);                                           //响应超时
    }
    SCS = 1;
    return SD_ERR_CMD_OK;
}
