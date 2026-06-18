/********************************** (C) COPYRIGHT *******************************
* File Name          : USB_DEVICE.C
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563 USB_Device Example
*******************************************************************************/



/******************************************************************************/
/* 头文件包含 */
#include  <stdio.h>
#include  <string.h>
#include  "CH563BAS.H"                                                          /* 芯片类型定义相关头文件 */    
#include  "CH563SFR.H"                                                          /* 芯片寄存器相关头文件 */
#include  "SYSFREQ.H"                                                           /* 系统配置相关头文件 */
#include  "CH563USBSFR.H"                                                       /* 芯片USB寄存器相关头文件 */    
#include  "CH563USB.H"                                                          /* USB常用定义相关头文件 */    
#include  "USB_DEVICE.H"                                                        /* USB设备定义相关头文件 */
#include  "PRINTF.H"                                                            /* 串口打印输出控制头文件 */
#include  "AUDIO.H"


UINT32 MyBuf[1024];
UINT32 count=0;
/******************************************************************************/
/* 设备描述符 */
const UINT8 MyDevDescr[ ] = {
 0x12,            //Length            
 0x01,            //DescriptorType:Descriptor
 0x00, 0x02,      //USB2.0
 0x00,            //bDeviceClass
 0x00,            //bDeviceSubClass    
 0x00,            //bDeviceProtocol
 0x40,            //MaxPacketSize 64
 0x54, 0x68,      //ID Vendor
 0x54, 0x57,      //ID product    
 0x00, 0x01,      //bcdDevice
 0x00,                
 0x00,
 0x00,
 0x01,            //bNumConfigurations 
};

/* 全速配置描述符 */
const UINT8 My_FS_CfgDescr[ ] = {
//配置描述符                  
  //Configuration1  
 0x09,           //Length                            
 0x02,           //DescriptorType : ConfigDescriptor 
 0xFE, 0x00,     //TotalLength               
 0x04,           //NumInterfaces:4                 
 0x01,           //ConfigurationValue                
 0x00,           //Configuration String              
 0x80,           //Attributes:Bus Power              
 0x32,           //MaxPower = 0x32*2ma  

//接口描述符
 //standard interface AC         descriptor(Interface 0, Alternate Setting 0):
 0x09,           //Length                     
 0x04,           //DescriptorType:Inerface    
 0x00,           //InterfaceNum:0             
 0x00,           //AlternateSetting:0   (identified)      
 0x00,           //NumEndpoint:0              
 0x01,           //InterfaceClass:audio       
 0x01,           //InterfaceSubClass:audio control
 0x00,           //          
 0x00,           //       

 //class-specific AC interface descriptor
 0x0A,           //Length                                                                                        
 0x24,           //DescriptorType:audio interface descriptor                                                     
 0x01,           //DescriptorSubType:audio control header                                                        
 0x00,0x01,      //bcdADC:audio Device Class 1.0       
 0x6B,0x00,      //TotalLength of class-specific AC interface descriptors                                                                            
 0x02,           //InCollection:2 //the number of AudioStreaming interface                                                       
 0x01,           //InterfaceNr(1)   
 0x02,           // 
             
 //USB Streaming ITD    ID=01
 0x0C,           //Length                                    
 0x24,           //DescriptorType:audio interface descriptor 
 0x02,           //DescriptorSubType:Input Terminal          
 0x01,           //Terminal   ID:0x01                           
 0x01,0x01,      //TerminalType:USB Streaming              
 0x00,           //bAssocTerminal                            
 0x02,           //NrChannels:2 channel                              
 0x03,0x00,      //ChannelConfig:Left Front,Right Front,     
 0x00,           //ChannelName String                        
 0x00,           //Terminal String                           
 //mono mic ITD     ID=04
 0x0C,           //Length                                    
 0x24,           //DescriptorType:audio interface descriptor 
 0x02,           //DescriptorSubType:Input Terminal          
 0x04,           //Terminal   ID:0x04                           
 0x01,0x02,      //TerminalType:microphone              
 0x00,           //bAssocTerminal                            
 0x01,           //NrChannels:1 channel                              
 0x01,0x00,      //ChannelConfig:center Front,  //0001     
 0x00,           //ChannelName String                        
 0x00,           //Terminal String  

//SU 
 0x07,           //Length                                    
 0x24,           //DescriptorType:audio interface descriptor 
 0x05,           //DescriptorSubType:SU
 0x09,           //ID
 0x01,           //bNrInPins
 0x05,           //SID
 0x00,

//Audio Feature Unit Descriptor FU1      ID=2
 //SourceId 0x08,ControlSize 0x02, Mute,Volume,  
 0x0D,          //Length                                   
 0x24,          //DescriptorType:audio interface descriptor
 0x06,          //DescriptorSubType:Audio Feature Unit     
 0x02,          //UnitID:0x02                             
 0x08,          //SID:8 MU                  
 0x02,          //ControlSize:n=2byte                       
 0x01,0x00,     //Controls(0):Volume supported for master channel
                             //D0: Mute
                            //D1: Volume
                            //D2: Bass
                            //D3: Mid
                            //D4: Treble
                            //D5: Graphic Equalizer
                            //D6: Automatic Gain
                            //D7: Delay
                            //D8: Bass Boost
                            //D9: Loudness                     
 0x02,0x00,     //Controls(1):supported for logical channel 1.
 0x02,0x00,     //Controls(2):supported for logical channel 2.
 0x00,          //Feature String

 //SourceId 0x04,ControlSize 0x02,Mute,Volume,   FU2    ID=05
 0x0B,         //Length                                   
 0x24,         //DescriptorType:audio interface descriptor
 0x06,         //DescriptorSubType:Audio Feature Unit     
 0x05,         //UnitID:0x05                             
 0x04,         //SourceID:4    #USB mic IT                
 0x02,         //ControlSize:n=2byte                       
 0x03,0x00,    //Controls(0):Volume supported for master channel
                             //D0: Mute
                            //D1: Volume
                            //D2: Bass
                            //D3: Mid
                            //D4: Treble
                            //D5: Graphic Equalizer
                            //D6: Automatic Gain
                            //D7: Delay
                            //D8: Bass Boost
                            //D9: Loudness                     
 0x00,0x00,     //Controls(1):supported for logical channel 1.
 0x00,          //Feature String 
 //SourceId 0x04,ControlSize 0x02,Mute,Volume,    FU3    ID=07
 0x0B,          //Length                                   
 0x24,          //DescriptorType:audio interface descriptor
 0x06,          //DescriptorSubType:Audio Feature Unit     
 0x07,          //UnitID:0x07                             
 0x04,          //SourceID:4    #USB mic IT                
 0x02,          //ControlSize:n=2byte                       
 0x03,0x00,     //Controls(0):Volume supported for master channel
                             //D0: Mute
                            //D1: Volume
                            //D2: Bass
                            //D3: Mid
                            //D4: Treble
                            //D5: Graphic Equalizer
                            //D6: Automatic Gain
                            //D7: Delay
                            //D8: Bass Boost
                            //D9: Loudness                     
 0x00,0x00,     //Controls(1):    
 0x00,          //Feature String 

//MU 
 //SourceId 0x01,0x07,MU  
 0x0D,          //Length                                   
 0x24,          //DescriptorType:audio interface descriptor
 0x04,          //DescriptorSubType:Audio MU     
 0x08,          //UnitID:0x08                             
 0x02,          //TWO InPinS
 0x01,          //SourceID:1    #USB  IT 
 0x07,          //SourceID:7    #USB  FU3            
 0x02,          //two channel                      
 0x03,0x00,     //left,right
 0x00,          //
 0x00,          //
 0x00,          //

 //USB Speaker    OTD    ID=03
 0x09,          //Length                                    
 0x24,          //DescriptorType:audio interface descriptor 
 0x03,          //DescriptorSubTYpe:Output Terminal         
 0x03,          //Terminal      ID:0x03                           
 0x01, 0x03,    //TerminalType:Speaker                
 0x00,      //AssocTerminal: 00                      
 0x02,      //SourceID:2   
 0x00,      //Terminal String
  //USB_IN    OTD      ID=06
 0x09,      //Length                                    
 0x24,      //DescriptorType:audio interface descriptor 
 0x03,      //DescriptorSubTYpe:Output Terminal         
 0x06,      //Terminal      ID:0x06                           
 0x01, 0x01,//TerminalType: USB Streaming               
 0x04,      //AssocTerminal: 04                      
 0x09,      //SourceID:9   
 0x00,      //Terminal String 

//-------------------Speaker  interface---------------------//
 //standard interface AS descriptor(Interface 1, Alternate Setting 0):
 0x09,       //Length                           
 0x04,       //DescriptorType:Interface         
 0x01,       //InterfaceNum:1                   
 0x00,       //AlternateSetting:0               
 0x00,       //NumEndpoint:0                    
 0x01,       //InterfaceClass:audio             
 0x02,       //InterfaceSubClass:audio streaming
 0x00,       //InterfaceProtocol                
 0x00,       //Interface String                 

 //standard interface AS descriptor(Interface 1, Alternate Setting 1):                        
 0x09,       //Length                           
 0x04,       //DescriptorType:Interface         
 0x01,       //InterfaceNum:1                   
 0x01,       //AlternateSetting:1               
 0x01,       //NumEndpoint:1                    
 0x01,       //InterfaceClass:audio             
 0x02,       //InterfaceSubClass:audio streaming
 0x00,       //InterfaceProtocol                
 0x00,       //Interface String                
 //Audio Streaming Interface Descriptor:AS_GENERAL(0x01),
 0x07,       //Length                                   
 0x24,       //DescriptorType:audio interface descriptor
 0x01,       //DescriptorSubType:AS_GENERAL             
 0x01,       //TerminalLink    
 0x01,       //Delay:0                                  
 0x01, 0x00, //FormatTag:PCM  0x0001
 
 //Class-Specific AS Format Type Descriptor
 0x0B,              //Length                                   
 0x24,              //DescriptorType:audio interface descriptor
 0x02,              //DescriptorSubType:Format_type            
 0x01,              //FormatType:Format type 1                 
 0x02,              //NumberOfChannel:2                       
 0x02,              //SubframeSize:2byte The number of bytes occupied by one audio subframe. Can be 1, 2, 3 or 4.                      
 0x10,              //BitsResolution:16bit   The number of effectively used bits from the available bits in an audio subframe                  
 0x01,              //SampleFreqType:One sampling frequency.   
 0x40,0x1f,0x00,    //8K(0x001f40)12k(0x002ee0) 16k(0x003e80)  32k(0x007300) 44.1k(0x00ac44) 48k(0x00bb80) 
 //Endpoint 1 - Standard Descriptor:Output Endpoint                  
 //Isochronous,Synchronization Type(Asynchronous)
 0x09,       //Length                                                                
 0x05,       //DescriptorType:endpoint descriptor                                    
 0x02,       //EndpointAddress:Output                                       
 0x09,       // D3..2: Synchronization type    01 = Asynchronous  10 = Adaptive  11 = Synchronous
             //    D1..0: Transfer type        01 = Isochronous   10 = bulk      11 = interrupt
 0x40,0x00,  //MaxPacketSize 64                                       
 0x01,       //delay 1                              
 0x00,       //bRefresh      
 0x00,       //bSynchAddress
 //Endpoint - Audio Streaming Descriptor
 //Audio Endpoint descriptor,General
 0x07,      //Length                                     
 0x25,      //DescriptorType:audio endpoint descriptor   
 0x01,      //DescriptorSubType:audio endpiont general   
 0x00,      //Attributes:0x00.............               
 0x01,      //LockDelayUnits                             
 0x01,
 0x00, 
//------------------mono-mic---------------------//
 //standard interface AS descriptor(Interface 2, Alternate Setting 0):
 0x09,       //Length                           
 0x04,       //DescriptorType:Interface         
 0x02,       //InterfaceNum:2                   
 0x00,       //AlternateSetting:0               
 0x00,       //NumEndpoint:0                    
 0x01,       //InterfaceClass:audio             
 0x02,       //InterfaceSubClass:audio streaming
 0x00,       //InterfaceProtocol                
 0x00,       //Interface String                 

 //standard interface AS descriptor(Interface 2, Alternate Setting 1):                        
 0x09,       //Length                           
 0x04,       //DescriptorType:Interface         
 0x02,       //InterfaceNum:2                   
 0x01,       //AlternateSetting:1               
 0x01,       //NumEndpoint:1                    
 0x01,       //InterfaceClass:audio             
 0x02,       //InterfaceSubClass:audio streaming
 0x00,       //InterfaceProtocol                
 0x00,       //Interface String 

//Audio Streaming Interface Descriptor:AS_GENERAL(0x01),
 0x07,       //Length                                   
 0x24,       //DescriptorType:audio interface descriptor
 0x01,       //DescriptorSubType:AS_GENERAL             
 0x06,       //TerminalLink    
 0x01,       //Delay:0                                  
 0x01, 0x00, //FormatTag:PCM  0x0001
 
 //Class-Specific AS Format Type Descriptor
 0x0B,              //Length                                   
 0x24,              //DescriptorType:audio interface descriptor
 0x02,              //DescriptorSubType:Format_type            
 0x01,              //FormatType:Format type 1                 
 0x01,              //NumberOfChannel:1                       
 0x02,              //SubframeSize:2byte The number of bytes occupied by one audio subframe. Can be 1, 2, 3 or 4.                      
 0x10,              //BitsResolution:16bit   The number of effectively used bits from the available bits in an audio subframe                  
 0x01,              //SampleFreqType:One sampling frequency.   
 0x40,0x1F,0x00,    //8K(0x001f40) 16k(0x003e80)  32k(0x007300)  48k(0x00bb80)    
 //Endpoint 2 - Standard Descriptor:Input Endpoint                  
 //Isochronous,Synchronization Type(Asynchronous)
 0x09,       //Length                                                                
 0x05,       //DescriptorType:endpoint descriptor                                    
 0x82,       //EndpointAddress:Input                                       
 0x05,       // D3..2: Synchronization type    01 = Asynchronous  10 = Adaptive  11 = Synchronous
             //    D1..0: Transfer type        01 = Isochronous   10 = bulk      11 = interrupt
 0x40,0x00,  //MaxPacketSize 64                                       
 0x01,       //delay 1                              
 0x00,       //bRefresh      
 0x00,       //bSynchAddress

 //Endpoint - Audio Streaming Descriptor
 //Audio Endpoint descriptor,General
 0x07,      //Length                                     
 0x25,      //DescriptorType:audio endpoint descriptor   
 0x01,      //DescriptorSubType:audio endpiont general   
 0x00,      //Attributes:0x00.............               
 0x00,      //LockDelayUnits                             
 0x00,
 0x00,   

//------------------HID--------------------//
 //interface HID descriptor(Interface 3, Alternate Setting 0):
 0x09,       //Length                           
 0x04,       //DescriptorType:Interface         
 0x03,       //InterfaceNum:3                   
 0x00,       //AlternateSetting:0               
 0x01,       //NumEndpoint:1                    
 0x03,       //InterfaceClass: HID            
 0x00,       //InterfaceSubClass:not used
 0x00,       //InterfaceProtocol                
 0x00,       //Interface String 
 //
 0x09,       //Length
 0x21,       //DescriptorType:HID
 0x00,0x01,  //bcdHID
 0x00,       //bCountyCode
 0x01,       //bNumHID
 0x22,       //bDescriptorType:
 0x32,       //TotalLength
 0x00,       //
 //Endpoint 3 - Standard Descriptor: Endpoint                  
 0x07,       //Length                                                                
 0x05,       //DescriptorType:endpoint descriptor                                    
 0x81,       //EndpointAddress:Output                                       
 0x03,       // D3..2: Synchronization type    01 = Asynchronous  10 = Adaptive  11 = Synchronous
             //    D1..0: Transfer type        01 = Isochronous   10 = bulk      11 = interrupt
 0x04,0x00,  //MaxPacketSize 4                                       
 0x08,       //delay 8               
};

/* 高速配置描述符 */         
const UINT8 My_HS_CfgDescr[ ] = { 0x09, 0x02, 0x27, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
                                  0x09, 0x04, 0x00, 0x00, 0x03, 0xFF, 0x80, 0x37, 0x00,
                                  0x07, 0x05, 0x82, 0x02, 0x00, 0x02, 0x00,
                                  0x07, 0x05, 0x02, 0x02, 0x00, 0x02, 0x00,
                                  0x07, 0x05, 0x81, 0x03, 0x40, 0x00, 0x0A };

/* 设备限定描述符 */
const UINT8    My_QueDescr[ ] = { 0x0A, 0x06, 0x00, 0x02, 0xFF, 0x00, 0xFF, 0x40, 0x01, 0x00 };

/* 全速模式，其他速度配置描述符 */
const UINT8    My_FS_OscDescr[ ] = { 0x09, 0x07, 0x27, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
                                  0x09, 0x04, 0x00, 0x00, 0x03, 0xFF, 0x80, 0x37, 0x00,
                                  0x07, 0x05, 0x82, 0x02, 0x00, 0x02, 0x00,
                                  0x07, 0x05, 0x02, 0x02, 0x00, 0x02, 0x00,
                                  0x07, 0x05, 0x81, 0x03, 0x40, 0x00, 0x0A };

/* 高速模式，其他速度配置描述符 */
const UINT8    My_HS_OscDescr[ ] = { 0x09, 0x07, 0x27, 0x00, 0x01, 0x01, 0x00, 0x80, 0x32,
                                  0x09, 0x04, 0x00, 0x00, 0x03, 0xFF, 0x80, 0x37, 0x00,
                                  0x07, 0x05, 0x82, 0x02, 0x40, 0x00, 0x00,
                                  0x07, 0x05, 0x02, 0x02, 0x40, 0x00, 0x00,
                                  0x07, 0x05, 0x81, 0x03, 0x08, 0x00, 0x0A };

//报告描述符
const UINT8 MyReportDescr[] = 
{
0x05, 0x0C,               // Usage page(consumer device)
0x09, 0x01,               // Usage(remote control)
0xa1, 0x01,               // Cellection(application)
    0x15, 0x00,           // L_Min(0)
    0x25, 0x01,           // L_Max(1)
    0x09, 0xE9,           // Usage(volume up)
    0x09, 0xEA,           // Usage(volume down)
    0x75, 0x01,           // R_Size(1 bit)
    0x95, 0x02,           // R_Count(2)
    0x81, 0x02,           // Input(data,var,abs)

    0x09, 0xE2,           // Usage(mute)
    0x09, 0x00,           // Usage(unassigned)
    0x81, 0x06,           // Input(data,var,rel)

    0x09, 0x00,           // Usage
    0x95, 0x04,           // R_Count
    0x81, 0x02,           // Input

    0x26, 0xFF, 0x00,     // L_Max(2byte)（255）
    0x09, 0x00,           // Usage
    0x75, 0x08,           // R_Size(8bit)
    0x95, 0x03,           // R_Count(3)
    0x81, 0x02,           // Input

    0x09, 0x00,           // Usage
    0x95, 0x04,           // R_Count
    0x91, 0x02,           // Output
0xc0                      // end
};

// 语言描述符
const    UINT8 MyLangDescr[] = { 0x04, 0x03, 0x09, 0x04 };

// 厂家信息
const    UINT8 MyManuInfo[] = { 0x0e, 0x03, 0x77, 0x00, 0x63, 0x00, 0x68, 0x00, 0x2e, 0x00, 0x63, 0x00, 0x6e, 0x00 };

// 产品信息
const    UINT8 MyProdInfo[16] = { 0x10, 0x03, 'h', 0, 'e', 0, 'a', 0, 'd', 0, 's', 0, 'e', 0, 't', 0};

UINT8    cla81_buf[12] = { 0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00,0x00, 0x00, 0x00, 0x00 };
UINT8    cla82_buf[12] = { 0x00, 0x00, 0x00, 0x80,0x00, 0x00, 0x00, 0x80,0x00, 0x00, 0x00, 0x00 };



/*******************************************************************************/
/* 常、变量定义 */
UINT8V  gUsbConfig;                                                             /* USB设备配置标志 */
UINT8V  gSetupReq;                                                              /* USB控制传输命令码 */    
UINT16V gSetupLen;                                                              /* USB控制传输传输长度 */
UINT8V  gUsbSpeed;                                                              /* 当前USB速度(默认0: 高速;1:全速) */    
UINT8V  gUsbInterCfg;                                                           /* USB设备接口配置 */
UINT8V  gUsbFlag;                                                               /* USB设备各种操作标志,位0=总线复位,位1=获取设备描述符,位2=设置地址,位3=获取配置描述符,位4=设置配置 */
UINT8   *pDescr;                                                                /* 描述符指针 */        
UINT32  USB_Recv_Buf[ 512 ];                                                    /* USB设备数据接收缓冲区 */
UINT32  USB_Tran_Buf[ 512 ];                                                    /* USB设备数据发送缓冲区 */
UINT32V gRecvLen;                                                               /* 当前包接收长度 */
AUDIO_PWM pwm;

/*******************************************************************************
* Function Name  : USBDev_ModeSet
* Description    : 设置成USB设备模式
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_ModeSet( void )
{ 
    R8_MISC_CTRL_USB &= ~RB_MISC_USB_ID_EN;                                     /* USB OTG ID pin enable */                                    
    R8_MISC_CTRL_USB |= RB_MISC_USB_ID_ST;                                      /* 设置成USB设备 */
}

/*******************************************************************************
* Function Name  : USBDev_SpeedSet
* Description    : USB设备速度设置
* Input          : speed---USB_DEV_SPEED_HS: 高速模式
*                          USB_DEV_SPEED_FS: 全速模式
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_SpeedSet( UINT8 speed )
{
    if( speed == USB_DEV_SPEED_HS )
    {
        USB_DEV_CONTROL &= ~RB_FORCE_FS;                                        /* 高速模式(默认) */
    }
    else
    {
        USB_DEV_CONTROL |= RB_FORCE_FS;                                         /* 全速模式 */
    }    
}

/*******************************************************************************
* Function Name  : USBDev_SpeedCheck
* Description    : USB设备速度检测
* Input          : None
* Output         : None
* Return         : 返回USB_DEV_SPEED_HS: 高速模式; USB_DEV_SPEED_FS: 全速模式
*******************************************************************************/

UINT8 USBDev_SpeedCheck( void )
{
     if( USB_DEV_CONTROL & RB_HS_EN )
    {
#if  MY_DEBUG_PRINTF         
        printf("Current Device is High Speed!\n");    
#endif                                    
        gUsbSpeed = USB_DEV_SPEED_HS;                                           /* 当前USB设备速度为高速 */
        return( USB_DEV_SPEED_HS );    
    }
    else
    {
#if  MY_DEBUG_PRINTF         
          printf("Current Device is Full Speed!\n");
#endif          
        gUsbSpeed = USB_DEV_SPEED_FS;                                           /* 当前USB设备速度为全速 */
        return( USB_DEV_SPEED_FS );    
    }
}

/*******************************************************************************
* Function Name  : USBDev_UsbInt_Enable
* Description    : USB设备相关USB中断使能
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_UsbInt_Enable( void )  
{
    /* 开启USB中断 */
    R8_INT_EN_IRQ_1 |= RB_IE_IRQ_USB;                                           /* USB中断使能 */
    R8_INT_EN_IRQ_GLOB |= RB_IE_IRQ_GLOB;                                       /* 全局中断使能 */
}

/*******************************************************************************
* Function Name  : USBDev_Init
* Description    : USB设备初始化
*                  包括主要寄存器的设置，端点、FIFO配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_Init( void )
{
    /* 模拟CH372设备 */
    /* ENDP1_IN 为中断端点，占8个字节，使用FIFO0(IN) */
    /* ENDP2_IN 为批量端点，占64个字节，使用FIFO1(IN) */
    /* ENDP2_OUT为批量端点，占64个字节，使用FIFO2(OUT) */

    /* 复位USB设备 */
    USB_DEV_CONTROL = RB_SFRST;
    while( USB_DEV_CONTROL & 0x10 );                                            /* 等待复位完成 */

    /* 复位USB设备相关FIFO */
    USB_DEV_FIFO0_INFO = RB_FFRST;                                              /* FIFO0 Reset */
    USB_DEV_FIFO1_INFO = RB_FFRST;                                              /* FIFO1 Reset */
    USB_DEV_FIFO2_INFO = RB_FFRST;                                              /* FIFO2 Reset */
    USB_DEV_FIFO3_INFO = RB_FFRST;                                              /* FIFO3 Reset */
    Delay_us( 10 );    
    
#if 0
    /* 一、各端点以及相关FIFO配置(寄存器直接配置方式) */
    USB_DEV_FIFO_CONFIG    = 0x00222223;                                        /* R(0x1AC) */
    USB_DEV_EP1_4_MAP      = 0x33332130;                                        /* R(0x1A0) FIFO0(ENDP1_IN)、FIFO1(ENDP2_IN)、FIFO2(ENDP2_OUT) */         
    USB_DEV_FIFO_MAP       = 0x0F021211;                                        /* R(0x1A8) */                                          
    USB_DEV_EP1_IN_MAXPKS  = 0x00000008;                                        /* R(0x160) EP1_IN_MAXPKS  = 8 bytes */
    USB_DEV_EP2_IN_MAXPKS  = 0x00000040;                                        /* R(0x164) EP2_IN_MAXPKS  = 64 bytes */
    USB_DEV_EP2_OUT_MAXPKS = 0x00000040;                                        /* R(0x184) EP2_OUT_MAXPKS = 64 bytes */ 
#endif

#if 1        
    /* 二、各端点以及相关FIFO配置(函数间接配置方式) */

    /* 端点1配置 */
    USBDev_EPx_Init( ENDP1, 
                     ENDP_TYPE_IN,                                              /* 配置ENDP1 IN端点 */
                     FIFO_TYPE_INTERRUPT,                                       /* 端点类型为中断端点 */
                     FIFO_DIR_IN,                                               /* FIFO方向为为IN */
                     FIFO_BK_NUM_SINGLE,                                        /* FIFO块数为单块,最大512字节 */    
                     FIFO_BK_SIZE_0,                                            /* FIFO块大小小于等于512字节 */
                     FIFO_NUM_0,                                                /* 该端点对应的FIFO为FIFO 0 */
                     FIFO_ENABLE,                                               /* 使能该端点对应的FIFO */
                     USB_EP1_IN_MAX_PKT );                                      /* 设置该端点最大包大小 */
    
    /* 端点2配置 */    
    USBDev_EPx_Init( ENDP2,     
                     ENDP_TYPE_IN,                                              /* 配置ENDP2 IN端点 */
                     FIFO_TYPE_ISO,                                             /* 端点类型为批量端点 */
                     FIFO_DIR_IN,                                               /* FIFO方向为为IN */
                     FIFO_BK_NUM_SINGLE,                                        /* FIFO块数为单块,最大512字节 */
                     FIFO_BK_SIZE_0,                                            /* FIFO块大小小于等于512字节 */    
                     FIFO_NUM_1,                                                /* 该端点对应的FIFO为FIFO 1 */
                     FIFO_ENABLE,                                               /* 使能该端点对应的FIFO */    
                     USB_EP2_IN_MAX_PKT );                                      /* 设置该端点最大包大小 */

    /* 端点2配置 */
    USBDev_EPx_Init( ENDP2, 
                     ENDP_TYPE_OUT,                                             /* 配置ENDP2 OUT端点 */
                     FIFO_TYPE_ISO,                                             /* 端点类型为批量端点 */
                     FIFO_DIR_OUT,                                              /* FIFO方向为为OUT */
                     FIFO_BK_NUM_SINGLE,                                        /* FIFO块数为单块,最大512字节 */
                     FIFO_BK_SIZE_0,                                            /* FIFO块大小小于等于512字节 */    
                     FIFO_NUM_2,                                                /* 该端点对应的FIFO为FIFO 2 */
                     FIFO_ENABLE,                                               /* 使能该端点对应的FIFO */    
                     USB_EP2_OUT_MAX_PKT );                                     /* 设置该端点最大包大小 */        
#endif        

    /* USB设备各种中断使能设置 */
    /* 该程序使能了除FIFO空中断外的所有中断,可以根据需要关闭相应中断 */
    USB_DEV_MASK_INT_GROP  = 0x00000000;                                        /* Enable USB Device interrupt of source group 0、1、2*/
    USB_DEV_MASK_INT_GROP0 = 0x00000000;
    USB_DEV_MASK_INT_GROP1 = RB_F3_IN_INT | RB_F2_IN_INT | RB_F1_IN_INT | 
        RB_F0_IN_INT;                                                           /* 禁止FIFO上传空中断 */
    USB_DEV_MASK_INT_GROP2 = 0x00000000;        

    /* 使能芯片并连接(根据需要可以强制设置全速模式) */
    USB_DEV_CONTROL = RB_CHIP_EN | RB_GLINT_EN;                                 /* 使能芯片、使能全局中断、使能高速 */                  
    USBDev_SpeedSet( USB_DEV_SPEED_FS );                                        /* 强制设置全速模式 */
    Delay_ms( 1 );                                                
    USB_PHY_TEST_MODE &= ~RB_UNPLUG;                                            /* 寄存器(0x114)位0清0,通知主机连接 */
} 

/*******************************************************************************
* Function Name  : USB_Dev_SetAddress
* Description    : 设置USB设备地址
* Input          : addre---设备地址
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_SetAddress( UINT8 addre )
{
    USB_DEV_ADDRESS = ( USB_DEV_ADDRESS & RB_AFT_CONF ) | addre;                /* 清地址寄存器低7位 */    
}

/*******************************************************************************
* Function Name  : USBDev_SetConfig
* Description    : 设置USB设备配置标志位
*                  注意: 必须在处理SET_CONFIGURATION命令后设置该标志位,否则不会响应
*                        非控制端点的传输
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_SetConfig( void )
{
    USB_DEV_ADDRESS |= RB_AFT_CONF;                                             /* 设置配置标志位 */     
    
#if  MY_DEBUG_PRINTF         
    printf("USB Enumerate Success!\n ");
#endif
}

/*******************************************************************************
* Function Name  : USBDev_EP0_Stall
* Description    : USB设备控制设置STALL
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EP0_Stall( void )
{
    USB_DEV_CX_CONF_FIFO_STATUS |= RB_CX_STL;                                   /* reg=0x120 */    
}

/*******************************************************************************
* Function Name  : USBDev_EPx_IN_Stall
* Description    : USB设备EPx的IN端点设置STALL
* Input          : epx_num---端点号(范围为:1---8)
*                  mode------模式(1为设置STALL, 0为清除STALL)
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_IN_Stall( UINT8 epx_num, UINT8 mode )
{
    UINT32 addr;
    UINT32 reg;    
    
    addr = USB_BASE_ADDR + 0x160 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));    
    if( mode )
    {
        (*((PUINT32V)addr)) = reg | RB_STL_IEPx;        
    }
    else
    {
        (*((PUINT32V)addr)) = reg & ( ~RB_STL_IEPx );    
    }                
}

/*******************************************************************************
* Function Name  : USBDev_EPx_OUT_Stall
* Description    : USB设备EPx的OUT端点设置STALL
* Input          : epx_num---端点号(范围为:1---8)
*                  mode------模式(1：为设置STALL, 0为清除STALL)
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_OUT_Stall( UINT8 epx_num, UINT8 mode )
{
    UINT32 addr;
    UINT32 reg;    
    
    addr = USB_BASE_ADDR + 0x180 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));

    if( mode )
    {
        (*((PUINT32V)addr)) = reg | RB_STL_OEPx;        
    }
    else
    {
        (*((PUINT32V)addr)) = reg & ( ~RB_STL_OEPx );    
    }
}

/*******************************************************************************
* Function Name  : USBDev_EPx_IN_ResetToggle
* Description    : USB设备EPx的IN端点复位同步位
* Input          : epx_num---端点号(范围为:1---8)
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_IN_ResetToggle( UINT8 epx_num )
{
    UINT32 addr;
    UINT32 reg;    
    
    addr = USB_BASE_ADDR + 0x160 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));
    (*((PUINT32V)addr)) = reg | RB_RSTG_IEPx;
    Delay_us( 5 );
    (*((PUINT32V)addr)) = reg & ( ~RB_RSTG_IEPx );            
}

/*******************************************************************************
* Function Name  : USBDev_EPx_OUT_ResetToggle
* Description    : USB设备EPx的OUT端点复位同步位
* Input          : epx_num---端点号(范围为:1---8)
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_OUT_ResetToggle( UINT8 epx_num )
{
    UINT32 addr;
    UINT32 reg;    
    
    addr = USB_BASE_ADDR + 0x180 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));
    (*((PUINT32V)addr)) = reg | RB_RSTG_OEPx;
    Delay_us( 5 );
    (*((PUINT32V)addr)) = reg & ( ~RB_RSTG_OEPx );                
}

/*******************************************************************************
* Function Name  : USBDev_EPx_IN_SetMaxPKS
* Description    : 设置USB设备EPx的IN端点最大包长度
* Input          : epx_num---端点号(范围为:1---8)
*                  maxpks_len---最大包大小 
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_IN_SetMaxPKS( UINT8 epx_num, UINT16 maxpks_len )
{
    UINT32 addr;
    UINT32 reg;    
    
    maxpks_len = maxpks_len & 0x7FF;
    addr = USB_BASE_ADDR + 0x160 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));
    (*((PUINT32V)addr)) = ( reg & 0xFFFFFC00 ) | maxpks_len;        
}

/*******************************************************************************
* Function Name  : USBDev_EPx_OUT_SetMaxPKS
* Description    : 设置USB设备EPx的OUT端点最大包长度
* Input          : epx_num---端点号(范围为:1---8)
*                  maxpks_len---最大包大小 
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_OUT_SetMaxPKS( UINT8 epx_num, UINT16 maxpks_len )
{
    UINT32 addr;
    UINT32 reg;    
    
    maxpks_len = maxpks_len & 0x7FF;
    addr = USB_BASE_ADDR + 0x180 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));
    (*((PUINT32V)addr)) = ( reg & 0xFFFFFC00 ) | maxpks_len;        
}

/*******************************************************************************
* Function Name  : USBDev_EPx_Init
* Description    : USB设备端点配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EPx_Init( UINT8 epx_num, 
                      UINT8 epx_type,
                      UINT8 fifo_type, 
                      UINT8 fifo_dir, 
                      UINT8 fifo_bk_num, 
                      UINT8 fifo_bk_size, 
                      UINT8 fifo_num, 
                      UINT8 fifo_en,
                      UINT16 maxpks_len )
{    
    UINT32 addr;
    UINT32 reg1, reg2, mask;    
    
    /* 设置该端点IN或OUT包的最大包大小 */
    if( epx_type == ENDP_TYPE_IN )
    {    
        USBDev_EPx_IN_SetMaxPKS( epx_num, maxpks_len );                         /* 设置EPx_IN端点大小 */
    }
    else if( epx_type == ENDP_TYPE_OUT )
    {
        USBDev_EPx_OUT_SetMaxPKS( epx_num, maxpks_len );                        /* 设置EPx_OUT端点大小 */
    }        

    /* 设置该端点IN或OUT对应的FIFO号 */
    /* 配置Endpoint1-4 Map Register(Addr=0X1A0)以及Endpoint5-8 Map Register(Addr=0X1A4)*/    
    if( epx_num <= 4 )
    {
        addr = USB_BASE_ADDR + 0x1A0;    
    }
    else
    {
        addr = USB_BASE_ADDR + 0x1A4;            
        epx_num = epx_num - 4;
    }    
    reg1 = (*((PUINT32V)addr));                                                 /* 读取寄存器0X1A0或0X1A4原配置值 */

    mask = 0x00000003;
    reg2 = fifo_num;
    if( epx_type == ENDP_TYPE_OUT )
    {    
        mask = mask << 4;
        reg2 = reg2 << 4;
    }
    mask = mask << ( ( epx_num - 1 ) * 8 );    
    reg2 = reg2 << ( ( epx_num - 1 ) * 8 );                
    reg1 = ( reg1 & ( ~mask ) ) | reg2;    
    reg1 = reg1 & 0x33333333;
    (*((PUINT32V)addr)) = reg1;        

    /* 配置FIFO Map Register(Addr=0X1A8) */
    mask = 0x000000FF;
    reg2 = ( ( fifo_dir & 0x03 ) << 4 ) | epx_num;
    mask = mask << ( fifo_num * 8 );    
    reg2 = reg2 << ( fifo_num * 8 );    
    reg1 = USB_DEV_FIFO_MAP;    
    USB_DEV_FIFO_MAP = ( reg1 & ( ~mask ) ) | reg2;        
    
    /* 配置FIFO Configuration Register(Addr=0X1AC) */    
    mask = 0x000000FF;
    reg2 = fifo_type | ( ( fifo_bk_num & 0x03 ) << 2 ) | ( fifo_bk_size << 4 ) | ( fifo_en << 5 );        
    mask = mask << ( fifo_num * 8 );    
    reg2 = reg2 << ( fifo_num * 8 );    
    reg1 = USB_DEV_FIFO_CONFIG;    
    USB_DEV_FIFO_CONFIG = ( reg1 & ( ~mask ) ) | reg2; 
}

/*******************************************************************************
* Function Name  : USBDev_RD_FIFOx
* Description    : 从FIFO中读取USB数据
* Input          : fifonum---FIFO号
*                  len-------传输数据长度
*                  *pbuf-----数据存储缓冲区    
* Output         : None
* Return         : len---实际读取长度
*******************************************************************************/

UINT32 USBDev_RD_FIFOx( UINT8 fifonum, UINT32 len, UINT8 *pbuf )
{
    UINT32 i = 0;
        
    if( ( fifonum != USB_DMA_ACC_F0 ) && ( fifonum != USB_DMA_ACC_F1 ) &&
        ( fifonum != USB_DMA_ACC_F2 ) && ( fifonum != USB_DMA_ACC_F3 ) &&
        ( fifonum != USB_DMA_ACC_CXF ) )
    {
        USB_DEV_DMA_TARGET_FIFO = USB_DMA_ACC_NO;                               /* 设置DMA不对应任何FIFO */
        return( 0x00 );
    }
    
#if  0 
    printf("USBDev_RD_FIFOx:\n ");
#endif    
    USB_DEV_DMA_TARGET_FIFO = fifonum;                                          /* 设置DMA对应的FIFO */
    if( fifonum == USB_DMA_ACC_CXF )                                            /* SETUP包数据直接读取 */
    {
        if( len == 8 ) 
        {
            *(PUINT32)pbuf = USB_DEV_SETUP_CMD_RPORT;
            *(PUINT32)( pbuf + 4 ) = USB_DEV_SETUP_CMD_RPORT;
        }
        else 
        {
            len = 0;
        }            
    }
    else
    {
        if( USB_DEV_DMA_SET1 & RB_DMA_ABORT )                                   /* 判断是否有DMA_ABORT */
        {
            USB_DEV_DMA_SET1 |= RB_CLRFIFO_DMAABORT;    
            len = 0x00;                            
        }
        else
        {        
            /* 从FIFO x Instruction and Byte Count Register(0X1B0--0X1BC)中读取对应FIFO数据的长度 */
            if( fifonum == USB_DMA_ACC_F0 )
            {
                i = USB_DEV_FIFO0_INFO & 0x7FF;                                            
            }
            else if( fifonum == USB_DMA_ACC_F1 )
            {
                i = USB_DEV_FIFO1_INFO & 0x7FF;                                        
            }            
            else if( fifonum == USB_DMA_ACC_F2 )
            {
                i = USB_DEV_FIFO2_INFO & 0x7FF;                                        
            }            
            else if( fifonum == USB_DMA_ACC_F3 )
            {
                i = USB_DEV_FIFO3_INFO & 0x7FF;
            }    
            if( len > i )
            {
                len = i;                                                        /* 读取长度大于实际长度,则取实际长度 */
            }
#if 0 
            printf("rd_len:%04x\n",(UINT32)len);
#endif
            /* 通过DMA将数据从RAM传给FIFO */
            USB_DEV_DMA_SET1 = len << 8;                                        /* 设置USB DMA通信长度 */
            USB_DEV_DMA_SET2 = (UINT32)pbuf;                                    /* 设置USB DMA通信缓冲区起始地址 */    
            USB_DEV_DMA_SET1 = USB_DEV_DMA_SET1 | RB_DMA_START;                 /* 启动USB DMA进行FIFO数据处理 */
            while( USB_DEV_DMA_SET1 & RB_DMA_START );                           /* 等待DMA传输结束 */
            USB_DEV_INT_GROP2 = USB_DEV_INT_GROP2 | RB_DMA_CMPLT;               /* 清DMA完成标志 */        
        }
    }
    USB_DEV_DMA_TARGET_FIFO = USB_DMA_ACC_NO;                                   /* 设置DMA不对应任何FIFO */
    return( len );
}

/*******************************************************************************
* Function Name  : USBDev_WR_FIFOx
* Description    : 向FIFO中写入USB数据
* Input          : fifonum---FIFO号
*                  len-------传输数据长度
*                  *pbuf-----数据存储缓冲区    
* Output         : None
* Return         : len---写入长度
*******************************************************************************/

UINT32 USBDev_WR_FIFOx( UINT8 fifonum, UINT32 len, UINT8 *pbuf )
{
    if( ( fifonum != USB_DMA_ACC_F0 ) && ( fifonum != USB_DMA_ACC_F1 ) &&
        ( fifonum != USB_DMA_ACC_F2 ) && ( fifonum != USB_DMA_ACC_F3 ) &&
        ( fifonum != USB_DMA_ACC_CXF ) )
    {
        USB_DEV_DMA_TARGET_FIFO = USB_DMA_ACC_NO;                               /* 设置DMA不对应任何FIFO */
        return( 0x00 );
    }

#if  MY_DEBUG_PRINTF 
    printf("USBDev_WR_FIFOx:\n ");
#endif
    if( USB_DEV_DMA_SET1 & RB_DMA_ABORT )                                       /* 判断是否有DMA_ABORT */
    {
        USB_DEV_DMA_SET1 = RB_CLRFIFO_DMAABORT;    
        return( 0x00 );                            
    }
    else
    {        
#if  MY_DEBUG_PRINTF 
        printf("wr_len:%04x\n",(UINT32)len);
#endif

        /* 通过DMA将数据从FIFO传给RAM */
        USB_DEV_DMA_TARGET_FIFO = fifonum;                                      /* 设置USB DMA对应的FIFO */                                                 
        USB_DEV_DMA_SET1 = ( len  << 8 ) | RB_DMA_TYPE;                         /* 设置USB DMA通信长度 */
        
        /* 如果长度不为0则进行DMA数据处理 */
        if( len )
        {
            USB_DEV_DMA_SET2 = ( UINT32 )pbuf;                                  /* 设置USB DMA通信缓冲区起始地址 */    
            USB_DEV_DMA_SET1 = USB_DEV_DMA_SET1 | RB_DMA_START;                 /* 启动USB DMA进行FIFO数据处理 */
            while( ( USB_DEV_DMA_SET1 & RB_DMA_START ) );                       /* 等待DMA传输结束 */
        }
        USB_DEV_INT_GROP2 = USB_DEV_INT_GROP2 | RB_DMA_CMPLT;                   /* 清DMA完成标志 */    
        USB_DEV_DMA_TARGET_FIFO = USB_DMA_ACC_NO;                               /* 设置DMA不对应任何FIFO */
        return( len );        
    }        
}

/*******************************************************************************
* Function Name  : USBDev_EP0_Setup_Deal
* Description    : USB控制端点Setup包处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EP0_Setup_Deal( void )
{
    UINT32 len;
    UINT32 status;    
    UINT8  buf[ 8 ];
    USB_SETUP_REQ SetupReqBuf;

    /* 获取SETUP包 */    
    len = USBDev_RD_FIFOx( USB_DMA_ACC_CXF, 8, (PUINT8)&SetupReqBuf );          /* 从FIFO4中读取SETUP包 */
    if( len == sizeof( USB_SETUP_REQ ) )                                        /* 判断长度是否是Setup包长度 */
    {
        gSetupLen = ( SetupReqBuf.wLengthH << 8 ) + SetupReqBuf.wLengthL;       /* 保存传输长度 */
        gSetupReq = SetupReqBuf.bRequest;                                       /* 保存当前请求码 */

        /* 分析并处理当前SETUP包 */
        len = 0;                                                                /* 默认上传长度为0 */
        status = 0x00;                                                          /* 默认操作为成功状态 */    
        if( ( SetupReqBuf.bReqType & USB_REQ_TYPE_MASK ) != USB_REQ_TYPE_STAND ) 
        {  
            /* 其它请求，如类请求，产商请求等 */
                        switch(gSetupReq){
                            case DEF_USB_SET_CUR:
                                switch(SetupReqBuf.wIndexH){
                                    case DEF_USB_CLA_SU:                        //SU control
                                        break;
                                    case DEF_USB_CLA_FU1:                       //FU
                                    case DEF_USB_CLA_FU2:
                                    case DEF_USB_CLA_FU3:
                                        switch( SetupReqBuf.wValueH ){
                                            case DEF_USB_CLA_VOL:               //volume
                                                break;
                                             case DEF_USB_CLA_MUT:              //mute
                                                break;
                                            default:
                                                status = 0xFF;                  // 操作失败
                                                break;                            
                                        }
                                        break;
                                     default:
                                        status = 0xFF;                          // 操作失败
                                        break;    
                                }
                                break;

                            case DEF_USB_SET_UnK:
                                break;

                            case DEF_USB_GET_CUR:            
                                switch(SetupReqBuf.wIndexH){      
                                    case DEF_USB_CLA_MU:                        /* MU control */
                                        pDescr = (PUINT8)( &cla81_buf[0] );
                                        len = sizeof( cla81_buf );
                                        break;                                           
                                    case DEF_USB_CLA_SU:                        /* SU control */
                                        buf[0] = 0x01;
                                        pDescr = buf;
                                        if ( gSetupLen >= 1 ) len = 1;
                                        else len = gSetupLen;
                                        break;                            
                                    case DEF_USB_CLA_FU1:                       /* FU */
                                    case DEF_USB_CLA_FU2:
                                    case DEF_USB_CLA_FU3:
                                        switch( SetupReqBuf.wValueH ){
                                            case DEF_USB_CLA_VOL:               /* volume */
                                                buf[0] = 0x00;
                                                buf[1] = 0xf9;
                                                pDescr = buf;
                                                if ( gSetupLen >= 2 ) len = 2;
                                                else len = gSetupLen;                                                
                                                break;
                                             case DEF_USB_CLA_MUT:              /* mute */
                                                buf[1] = 0x00;
                                                pDescr = buf;
                                                if ( gSetupLen >= 1 ) len = 1;
                                                else len = gSetupLen;
                                                break;
                                            default:
                                                status = 0xFF;                  /* 操作失败 */
                                                break;                            
                                        }
                                        break;
                                }
                                break;
                            
                            case DEF_USB_GET_MIN:        
                                switch(SetupReqBuf.wIndexH){
                                    case DEF_USB_CLA_MU:
                                        pDescr = (PUINT8)( &cla82_buf[0] );
                                        len = sizeof( cla82_buf );
                                        break;
                                    case DEF_USB_CLA_FU1:                       /* FU */
                                    case DEF_USB_CLA_FU2:
                                    case DEF_USB_CLA_FU3:
                                        switch( SetupReqBuf.wValueH ){
                                            case DEF_USB_CLA_VOL:               /* volume */
                                                buf[0] = 0xa0;
                                                buf[1] = 0xe3;
                                                pDescr = buf;
                                                if ( gSetupLen >= 2 ) len = 2;
                                                else len = gSetupLen;
                                                break;
                                            default:
                                                status = 0xFF;                  /* 操作失败 */
                                                break;
                                        }
                                        break;
                                }
                                break;            
                            
                            case DEF_USB_GET_MAX:
                                switch(SetupReqBuf.wIndexH){
                                     case DEF_USB_CLA_FU1:                      /* FU */
                                     case DEF_USB_CLA_FU2:
                                     case DEF_USB_CLA_FU3:
                                        switch( SetupReqBuf.wValueH ){
                                            case DEF_USB_CLA_VOL:               /* volume */
                                                buf[0] = 0xf0;
                                                buf[1] = 0xff;
                                                pDescr = buf;
                                                if ( gSetupLen >= 2 ) len = 2;
                                                else len = gSetupLen;                                            
                                                break;
                                            default:
                                                status = 0xFF;                  /* 操作失败 */
                                                break;
                                        }
                                        break;
                                }
                                break;            
                            
                            case DEF_USB_GET_RES:            
                                switch(SetupReqBuf.wIndexH){
                                    case DEF_USB_CLA_FU1:                       /* FU */
                                    case DEF_USB_CLA_FU2:
                                    case DEF_USB_CLA_FU3:
                                        switch( SetupReqBuf.wValueH ){
                                            case DEF_USB_CLA_VOL:               /* volume */
                                                buf[0] = 0x30;
                                                buf[1] = 0x00;
                                                pDescr = buf;
                                                if ( gSetupLen >= 2 ) len = 2;
                                                else len = gSetupLen;
                                                break;
                                            default:
                                                status = 0xFF;                  /* 操作失败 */
                                                break;
                                        }
                                        break;
                                    default:
                                        status = 0xFF;
                                        break;
                                }
                                break;
                        }
        }
        else 
        {  
            /* 标准请求 */
            switch( gSetupReq ) 
            {
                case USB_REQ_GET_DESCR:                                         /* CMD: GET_DESCR */
#if  MY_DEBUG_PRINTF                     
                    printf("USB_REQ_GET_DESCR:\n ");
#endif
                    switch( SetupReqBuf.wValueH ) 
                    {
                        case 1:
                            gUsbFlag |= DEF_BIT_USB_DEV_DESC;                   /* 设置获取设备描述符标志 */
                            pDescr = (PUINT8)( &MyDevDescr[ 0 ] );
                            len = sizeof( MyDevDescr );
                            USBDev_SpeedCheck(  );                              /* 判断当前USB设备速度 */
                            break;
                        
                        case 2:
                            gUsbFlag |= DEF_BIT_USB_CFG_DESC;                   /* 设置获取配置描述符标志 */
                            if( gUsbSpeed == USB_DEV_SPEED_HS )                 /* 高速模式 */
                            {
                                gUsbFlag |= DEF_BIT_USB_HS;                     /* 设置高速标志 */
                                pDescr = (PUINT8)( &My_FS_CfgDescr[ 0 ] );
                                len = sizeof( My_FS_CfgDescr );
                                
                                /* 重新设置相应端点大小 */                                
                                USBDev_EPx_IN_SetMaxPKS( ENDP1, 64 );
                                USBDev_EPx_IN_SetMaxPKS( ENDP2, 512 );
                                USBDev_EPx_OUT_SetMaxPKS( ENDP2, 512 );                                
                            }
                            else                                                /* 全速模式 */        
                            {
                                gUsbFlag &= ~DEF_BIT_USB_HS;                    /* 设置全速标志 */
                                pDescr = (PUINT8)( &My_FS_CfgDescr[ 0 ] );
                                len = sizeof( My_FS_CfgDescr );
                            }
                            break;
                        
                        case 3:                            
                            switch( SetupReqBuf.wValueL ) 
                            {
                                case 1:
                                    pDescr = (PUINT8)( &MyManuInfo[ 0 ] );      /* 获取厂商字符串描述符 */
                                    len = sizeof( MyManuInfo );
                                    break;
                                    
                                case 2:
                                    pDescr = (PUINT8)( &MyProdInfo[ 0 ] );      /* 获取产品字符串描述符 */
                                    len = sizeof( MyProdInfo );
                                    break;
                                    
                                case 0:
                                    pDescr = (PUINT8)( &MyLangDescr[ 0 ] );     /* 获取语言ID字符串描述符 */
                                    len = sizeof( MyLangDescr );
                                    break;
                                
                                default:
                                    status = 0xFF;                              /* 操作失败 */
                                    break;
                            }
                            break;
                        
                        case 6:                                                 /* 设备限定描述符 */
                            pDescr = ( PUINT8 )( &My_QueDescr[ 0 ] );
                            len = sizeof( My_QueDescr );
                            break;
                        
                        case 7:                                                 /* 其他速度配置描述符 */
                            if( gUsbSpeed == USB_DEV_SPEED_HS )                 /* 高速模式 */
                            {
                                pDescr = ( PUINT8 )( &My_HS_OscDescr[ 0 ] );
                                len = sizeof( My_HS_OscDescr );                                
                            }
                            else
                            {
                                pDescr = ( PUINT8 )( &My_FS_OscDescr[ 0 ] );
                                len = sizeof( My_FS_OscDescr );    
                            }
                            break;
                        case 0x22:
                            pDescr = ( PUINT8 )( &MyReportDescr[ 0 ] );
                            len = sizeof( MyReportDescr );    
                            break;                                               
                        default:
                            status = 0xFF;                                      /* 操作失败 */
                            break;
                    }
                    break;
                                
                case USB_REQ_SET_ADDRESS:                                       /* CMD: SET_ADDRESS */    
                    gUsbFlag |= DEF_BIT_USB_ADDRESS;                            /* 设置设置地址标志 */            
                    if( SetupReqBuf.wValueL < 0x80 )
                    {
                        USBDev_SetAddress( SetupReqBuf.wValueL );               /* 设置地址: 注：感觉芯片在下一个SETUP包时该地址才生效 */                            
                    }
                    else
                    {
                        status = 0xFF;                                          /* 操作失败 */    
                    }
                    break;
                                
                case USB_REQ_GET_CONFIG:                                        /* CMD: GET_CONFIG */
                    buf[ 0 ] = gUsbConfig;
                    len = 0x01;
                    pDescr = buf;
                    break;
                                
                case USB_REQ_SET_CONFIG:                                        /* CMD: SET_CONFIG */
                    gUsbFlag |= DEF_BIT_USB_SET_CFG;                            /* 设置设置配置标志 */
                    gUsbConfig = SetupReqBuf.wValueL;
                    USBDev_SetConfig( );                                        /* 设置USB配置位 */
                    break;
                                
                case USB_REQ_CLR_FEATURE:                                       /* Clear Feature */
                    if( ( SetupReqBuf.bReqType & 0x1F ) == 0x00 )               /* 清除设备 */ 
                    {                                         
                        if( ( ( ( UINT16 )SetupReqBuf.wValueH << 8 ) | SetupReqBuf.wValueL ) == 0x01 )
                        {
                            if( gUsbSpeed == USB_DEV_SPEED_HS )                 /* 高速模式 */
                            {
                                if( My_HS_CfgDescr[ 7 ] & 0x20 ) 
                                {
                                    gUsbFlag &= ~DEF_BIT_USB_WAKE;              /* 设置唤醒禁止标志 */
                                }
                                else 
                                {
                                    status = 0xFF;                              /* 操作失败 */
                                }
                            }
                            else
                            {
                                if( My_FS_CfgDescr[ 7 ] & 0x20 ) 
                                {    
                                    gUsbFlag &= ~DEF_BIT_USB_WAKE;              /* 设置唤醒禁止标志 */
                                }
                                else 
                                {
                                    status = 0xFF;                              /* 操作失败 */
                                }
                            }
                        }
                        else 
                        {
                            status = 0xFF;                                      /* 操作失败 */
                        }    
                    }                    
                    else if( ( SetupReqBuf.bReqType & 0x1F ) == 0x02 )          /* 清除端点 */
                    {      
                        switch( ( ( UINT16 )SetupReqBuf.wIndexH << 8 ) | SetupReqBuf.wIndexL ) 
                        {
                            case 0x82:
                                USBDev_EPx_IN_Stall( 0x02, 0x00 );              /* 清除STALL */
                                USBDev_EPx_IN_ResetToggle( 0x02 );              /* 复位同步标志 */
                                break;
                            
                            case 0x02:
                                USBDev_EPx_OUT_Stall( 0x02, 0x00 );             /* 清除STALL */
                                USBDev_EPx_OUT_ResetToggle( 0x02 );             /* 复位同步标志 */
                                break;
                            
                            case 0x81:
                                USBDev_EPx_IN_Stall( 0x01, 0x00 );              /* 清除STALL */
                                USBDev_EPx_IN_ResetToggle( 0x01 );              /* 复位同步标志 */
                                break;
                            
                            case 0x01:
                                USBDev_EPx_OUT_Stall( 0x01, 0x00 );             /* 清除STALL */
                                USBDev_EPx_OUT_ResetToggle( 0x01 );             /* 复位同步标志 */
                                break;
                            
                            default:
                                status = 0xFF;                                  /* 操作失败 */
                                break;
                        }
                    }
                    else
                    {
                        status = 0xFF;                                          /* 操作失败 */
                    }
                    break;

                case USB_REQ_SET_FEATURE:                                       /* Set Feature */
                    if( ( SetupReqBuf.bReqType & 0x1F ) == 0x00 )               /* 设置设备 */
                    {
                        if( ( ( ( UINT16 )SetupReqBuf.wValueH << 8 ) | SetupReqBuf.wValueL ) == 0x01 )
                        {
                            if( gUsbSpeed == USB_DEV_SPEED_HS )                 /* 高速模式 */
                            {
                                if( My_HS_CfgDescr[ 7 ] & 0x20 ) 
                                {
                                    gUsbFlag |= DEF_BIT_USB_WAKE;               /* 设置唤醒使能标志 */
                                }
                                else 
                                {
                                    status = 0xFF;                              /* 操作失败 */
                                }
                            }
                            else
                            {
                                if( My_HS_CfgDescr[ 7 ] & 0x20 ) 
                                {
                                    gUsbFlag |= DEF_BIT_USB_WAKE;               /* 设置唤醒使能标志 */
                                }
                                else 
                                {    
                                    status = 0xFF;                              /* 操作失败 */
                                }
                            }
                        }
                        else 
                        {
                            status = 0xFF;                                      /* 操作失败 */
                        }
                    }
                    else if( ( SetupReqBuf.bReqType & 0x1F ) == 0x02 )          /* 设置端点 */
                    {
                        if( ( ( ( UINT16 )SetupReqBuf.wValueH << 8 ) | SetupReqBuf.wValueL ) == 0x00 )
                        {
                            switch( ( ( UINT16 )SetupReqBuf.wIndexH << 8 ) | SetupReqBuf.wIndexL ) 
                            {
                                case 0x82:
                                    USBDev_EPx_IN_Stall( 0x02, 0x01 );          /* 设置端点2 IN STALL */
                                    break;

                                case 0x02:
                                    USBDev_EPx_OUT_Stall( 0x02, 0x01 );         /* 设置端点2 OUT Stall */
                                    break;

                                case 0x81:
                                    USBDev_EPx_IN_Stall( 0x01, 0x01 );          /* 设置端点1 IN STALL */
                                    break;

                                default:
                                    status = 0xFF;                              /* 操作失败 */
                                    break;
                            }
                        }
                        else 
                        {
                            status = 0xFF;                                      /* 操作失败 */
                        }
                    }
                    else 
                    {
                        status = 0xFF;                                          /* 操作失败 */
                    }    
                    break;

                case USB_REQ_SET_INTERF:                                        /* Set Interface */
                    gUsbInterCfg = SetupReqBuf.wIndexL;                         /* 保存接口配置 */
#if 1
                    if( gUsbInterCfg == 1 ){
                        if( SetupReqBuf.wValueL == 1 ) 
                            USB_DEV_FIFO_CONFIG = USB_DEV_FIFO_CONFIG & (~(1<<17)) | (1<<16);                        
                                                                                /* FIFO2  OUT ISO */
                       else USB_DEV_FIFO_CONFIG = USB_DEV_FIFO_CONFIG & (~(1<<17)) & (~(1<<16));                        
                                                                                /* FIFO2  OUT RESV */

                    }
                    if( gUsbInterCfg == 2){
                        if( SetupReqBuf.wValueL == 1 )
                            USB_DEV_FIFO_CONFIG = USB_DEV_FIFO_CONFIG & (~(1<<9)) | (1<<8);                        
                                                                                /* FIFO1  IN  ISO */
                        else USB_DEV_FIFO_CONFIG = USB_DEV_FIFO_CONFIG & (~(1<<9)) & (~(1<<8));
                                                                                /* FIFO1  IN  RESV */
                    } 
#endif
                    break;
                
                case USB_REQ_GET_INTERF:                                        /* CMD: GET_INTERF */
                    buf[ 0 ] = gUsbInterCfg;
                    pDescr = buf;
                    len = 1;                                        
                    break;
                
                case USB_REQ_GET_STATUS:                                        /* Get Status */
                    if( ( SetupReqBuf.bReqType & 0x1F ) == 0x00 )               /* 获取设备状态 */
                    {
                        if( ( ( UINT16 )SetupReqBuf.wValueH << 8 | SetupReqBuf.wValueL ) == 0x00 && 
                            ( ( UINT16 )SetupReqBuf.wIndexH << 8 | SetupReqBuf.wIndexL ) == 0x00 )
                        {
                            buf[ 0 ] = ( gUsbFlag & DEF_BIT_USB_WAKE ) ? 0x02 : 0x00;
                            buf[ 1 ] = 0x00;
                            pDescr = buf;
                            len = 2;
                        }
                        else 
                        {
                            status = 0xFF;                                      /* 操作失败 */    
                        }
                    }
                    else if( ( SetupReqBuf.bReqType & 0x1F ) == 0x02 )          /* 获取端点状态 */
                    {
                        if( ( ( UINT16 )SetupReqBuf.wValueH << 8 | SetupReqBuf.wValueL ) == 0x00 )
                        {
                            switch( ( UINT16 )SetupReqBuf.wIndexH << 8 | SetupReqBuf.wIndexL )   
                            {
                                case 0x82:
                                    buf[ 0 ] = ( USB_DEV_EP2_IN_MAXPKS & RB_STL_IEPx ) ? 0x01 : 0x00;
                                    break;

                                case 0x02:
                                    buf[ 0 ] = ( USB_DEV_EP2_OUT_MAXPKS & RB_STL_OEPx ) ? 0x01 : 0x00;
                                    break;

                                case 0x81:
                                    buf[ 0 ] = ( USB_DEV_EP1_IN_MAXPKS & RB_STL_IEPx ) ? 0x01 : 0x00;
                                    break;

                                default:
                                    status = 0xFF;                              /* 操作失败 */
                                    break;
                            }
                            buf[ 1 ] = 0x00;
                            len = 2;
                            pDescr = buf;                            
                        }
                        else 
                        {
                            status = 0xFF;                                      /* 操作失败 */
                        }
                    }
                    else 
                    {
                        status = 0xFF;                                          /* 操作失败 */
                    }
                    break;
                                
                default:
                    status = 0xFF;                                              /* 操作失败 */
                    break;
            }
        }
    }
    else 
    {
        status = 0xFF;                                                          /* 操作失败 */
    }
    
    if( status == 0xFF ) 
    {  
        /* 操作失败 */
        USBDev_EP0_Stall( );                                                    /* 端点0返回STALL */
    }
    else
    {
        gSetupLen = gSetupLen < len ? gSetupLen : len;                          /* 限制总长度 */
        if( gSetupLen )
        {                
            if( SetupReqBuf.bReqType & 0x80 )                                   /* 上传 */
            {                
                len = gSetupLen>=USB_EP0_MAX_PKT ? USB_EP0_MAX_PKT : gSetupLen; /* 计算本次传输长度 */                    
                memcpy( USB_Tran_Buf, pDescr, len );                            /* 将FlashROM中的const数据复制到RAM中，以便DMA存取 */ 
                  USBDev_WR_FIFOx( USB_DMA_ACC_CXF, len, (UINT8 *)USB_Tran_Buf ); /* 加载上传数据到FIFO 4 */
                gSetupLen -= len;                                               /* 计算传输长度 */
                pDescr += len;                                                  /* 计算指针偏移 */
                if( gSetupLen == 0 )
                {
                    USB_DEV_CX_CONF_FIFO_STATUS |= RB_DONE;                     /* 全部上传结束,设置端点0数据传输结束 */
                }
            }        
        }
        else                                            
        {
            USB_DEV_CX_CONF_FIFO_STATUS |= RB_DONE;                             /* 设置端点0数据传输结束 */    
        }    
    }    
}

/*******************************************************************************
* Function Name  : USBDev_EP0_IN_Deal
* Description    : USB控制端点IN包处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EP0_IN_Deal( void )
{
    UINT16 len;
        
    switch( gSetupReq ) 
    {
        case USB_REQ_GET_DESCR:        
            len = gSetupLen >= USB_EP0_MAX_PKT ? USB_EP0_MAX_PKT : gSetupLen;   /* 本次传输长度 */
            memcpy( USB_Tran_Buf, pDescr, len );                                /* 将FlashROM中的const数据复制到RAM中，以便DMA存取 */
            USBDev_WR_FIFOx( USB_DMA_ACC_CXF, len, (UINT8 *)USB_Tran_Buf );     /* 加载上传数据 */
            gSetupLen -= len;
            pDescr += len;                    
            if( len < USB_EP0_MAX_PKT )                                         /* 判断数据是否全部上传结束 */    
            {
                USB_DEV_CX_CONF_FIFO_STATUS |= RB_DONE;                         /* 设置端点0数据传输结束 */    
            }                
            break;
            
        default:
            break;
    }
}

/*******************************************************************************
* Function Name  : USBDev_EP0_OUT_Deal
* Description    : USB控制端点OUT包处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EP0_OUT_Deal( void )
{
    
}

/*******************************************************************************
* Function Name  : USBDev_EP2_IN_Deal
* Description    : USB端点2 IN包处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EP2_IN_Deal( void )
{
#if  MY_DEBUG_PRINTF         
    printf("USBDev_EP2_IN_Deal...\n");
#endif

    /* 上传缓冲区中的数据 */
    if( gUsbSpeed == USB_DEV_SPEED_HS )                                         /* 高速模式 */
    {
        USBDev_WR_FIFOx( USB_DMA_ACC_F1, gRecvLen, (PUINT8)USB_Tran_Buf );
    }
    else
    {
        memset(USB_Tran_Buf,0x55,16);
        gRecvLen = 16;
        USBDev_WR_FIFOx( USB_DMA_ACC_F1, gRecvLen, (PUINT8)USB_Tran_Buf );
    }
}

/*******************************************************************************
* Function Name  : USBDev_EP2_OUT_Deal
* Description    : USB端点2 OUT包处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/

void USBDev_EP2_OUT_Deal( void )
{
    UINT32    len,i;

    /* 从下传缓冲区中提取数据 */
    if( gUsbSpeed == USB_DEV_SPEED_HS )                                         /* 高速模式 */
    {
        len = USBDev_RD_FIFOx( USB_DMA_ACC_F2, 512, (PUINT8)USB_Recv_Buf );     /* 读取EP2_OUT包 */                
    }
    else
    {
        len = USBDev_RD_FIFOx( USB_DMA_ACC_F2, 64, (PUINT8)USB_Recv_Buf );      /* 读取EP2_OUT包 */
    }
    pwm.DatLen = len>>2;
    for( i=0;i<pwm.DatLen;i++ ){
        pwm.DatBuf[i] = ((USB_Recv_Buf[i]>>8) + 0x80)&0xff;
        pwm.DatBuf[i] = pwm.DatBuf[i]*6;
    }
    pwm.DatBuf[i] = pwm.DatBuf[i-1]; 
    Star_PWM_DMA( );
}

/*******************************************************************************
* Function Name  : USBDev_EPx_Up_0BytePack
* Description    : USB设备EPx的IN端点上传0长度包
* Input          : epx_num---端点号(范围为:1---8)
* Output         : None
* Return         : None
*******************************************************************************/
void USBDev_EPx_Up_0BytePack( UINT8 epx_num )
{
    UINT32 addr;
    UINT32 reg;    
    
    addr = USB_BASE_ADDR + 0x160 + 4 * ( epx_num - 1 );
    reg  = (*((PUINT32V)addr));
    (*((PUINT32V)addr)) = reg | RB_TX0BYTE_IEPx;        
}
                            
/*******************************************************************************
* Function Name  : USBDev_IRQHandler
* Description    : USB中断处理
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USBDev_IRQHandler( void )
{
    UINT32 grp_int, intstatus;

    /* 判断是否有USB设备中断产生,有则处理 */    
    if( USB_GL_INT_STATUS & RB_DEV_INT )                                        /* 判断是否有USB设备中断产生 */
    {
        USB_GL_INT_STATUS = USB_GL_INT_STATUS | RB_DEV_INT;                     /* 该位写1清0 */  

        grp_int = USB_DEV_INT_GROP;                                             /* 获取产生USB中断的中断源 */
        if( grp_int & 0x01 )                                                    /* grop0: control transfer interrupt */
        {
            intstatus = USB_DEV_INT_GROP0;             
            if( intstatus & RB_CX_COMABORT_INT  )                               /* R(0x140)位5： 控制传输失败 */
            {        
                USB_DEV_INT_GROP0 = intstatus | RB_CX_COMABORT_INT;             /* 清USB端点0 OUT中断(写1清0) */                        
            }    
            if( intstatus & RB_CX_COMFAIL_INT )                                 /* R(0x140)位4：控制端点存在IN、OUT失败 */
            {
                /* 该中断位只读,由硬件清0 */                
                USBDev_EP0_Stall( );                                            /* 端点0返回STALL */
            }
            if( intstatus & RB_CX_COMEND )                                      /* R(0x140)位3：进入状态阶段 */
            {
                /* 控制传输进入状态阶段 */
                /* 该中断位只读,由硬件清0 */
            }
            if( intstatus & RB_CX_OUT_INT )                                     /* R(0x140)位2：控制端点OUT中断 */
            {                
                /* 控制端点OUT中断处理 */                                    
                USBDev_EP0_OUT_Deal( );        
                        
                /* 该中断位只读,由硬件清0 */
            }
            if( intstatus & RB_CX_IN_INT )                                      /* R(0x140)位1：控制端点IN中断 */
            {
                /* 控制端点IN中断处理 */                                
                USBDev_EP0_IN_Deal( );
                
                /* 该中断位只读,由硬件清0 */
            }
            if( intstatus & RB_CX_SETUP_INT )                                   /* R(0x140)位0：控制端点SETUP中断 */
            {
                /* 控制端点SETUP中断处理 */
                USBDev_EP0_Setup_Deal( );
                
                /* 该中断位只读,由硬件清0 */
            }                        
        }    
        if( grp_int & 0x02 )                                                    /* grop1: bulk/ISO/Interrupt interrupt */
        {
            intstatus = USB_DEV_INT_GROP1;         

            /* 该寄存器所有中断位为只读,由硬件清0 */
            if( intstatus & RB_F3_IN_INT )                                      /* FIFO3 IN interrupt */
            {                
                USB_DEV_MASK_INT_GROP1 |= RB_MF3_IN_INT;                        /* 禁止FIFO3空中断 */
                /* 自行添加处理程序 */                                                
            }
            if( intstatus & RB_F2_IN_INT )                                      /* FIFO2 IN interrupt */
            {                
                USB_DEV_MASK_INT_GROP1 |= RB_MF2_IN_INT;                        /* 禁止FIFO2空中断 */
                /* 自行添加处理程序 */    
            }
            if( intstatus & RB_F1_IN_INT )                                      /* FIFO1 IN interrupt */
            {
                USB_DEV_MASK_INT_GROP1 |= RB_MF1_IN_INT;                        /* 禁止FIFO1空中断 */
                /* 自行添加处理程序 */                    
            }
            if( intstatus & RB_F0_IN_INT )                                      /* FIFO0 IN interrupt */
            {    
                USB_DEV_MASK_INT_GROP1 |= RB_MF0_IN_INT;                        /* 禁止FIFO0空中断 */
                /* 自行添加处理程序 */                                    
            }
            if( intstatus & RB_F3_SPK_INT )                                     /* FIFO3 Short OUT interrupt */
            {                
            }
            if( intstatus & RB_F3_OUT_INT )                                     /* FIFO3 OUT interrupt */
            {                
                /* 自行添加处理程序 */
            }
            if( intstatus & RB_F2_SPK_INT )                                     /* FIFO2 Short OUT interrupt */
            {                
            }
            if( intstatus & RB_F2_OUT_INT )                                     /* FIFO2 OUT interrupt */
            {                
                USBDev_EP2_OUT_Deal( );                                         /* 数据下传处理 */
            }
            if( intstatus & RB_F1_SPK_INT )                                     /* FIFO1 Short OUT interrupt */
            {                
            }
            if( intstatus & RB_F1_OUT_INT )                                     /* FIFO1 OUT interrupt */
            {
                /* 自行添加处理程序 */                            
            }
            if( intstatus & RB_F1_IN_INT )                                     /* FIFO1 IN interrupt */
            {
                USBDev_EP2_IN_Deal( );                                            
            }
            if( intstatus & RB_F0_SPK_INT )                                     /* FIFO0 Short OUT interrupt */
            {                
            }
            if( intstatus & RB_F0_OUT_INT )                                     /* FIFO0 OUT interrupt */
            {                
                /* 自行添加处理程序 */
            }
        }    
        if( grp_int & 0x04 )                                                    /* grop2: bulk/ISO/Interrupt interrupt */ 
        {
            intstatus = USB_DEV_INT_GROP2; 
            
            if( intstatus & RB_DEV_WAKEUP_BYBUS )                               /* Dev_Wakeup_byVBUS interrupt */
            {                
                /* 设备由VBUS唤醒中断 */
                /* 该中断位只读,由硬件清0 */                
            }            
            if( intstatus & RB_DEV_IDLE )                                       /* Dev_Idle interrupt */
            {                
                /* 设备空闲中断 */
                /* 该中断位只读,由硬件清0 */                
            }            
            if( intstatus & RB_DMA_ERROR )                                      /* DMA Error interrupt */
            {                
                /* DMA错误中断 */
                USB_DEV_INT_GROP2 = RB_DMA_ERROR;                               /* 清DMA错误中断(写1清0) */                                
            }            
            if( intstatus & RB_DMA_CMPLT )                                      /* DMA Completion interrupt */
            {                
                /* DMA传输结束中断 */
                USB_DEV_INT_GROP2 = RB_DMA_CMPLT;                               /* 清DMA传输结束中断(写1清0) */                
            }            
            if( intstatus & RB_RX0BYTE_INT )                                    /* Received Zero-length Data Packet interrupt */
            {                
                /* 接收0长度数据包中断 */
                USB_DEV_INT_GROP2 |= RB_RX0BYTE_INT;                            /* 清接收0长度数据包中断(写1清0) */
                USB_DEV_RECV_0_LEN_PACK = 0x00;
            
                /* 自行添加处理,实际需要根据USB_DEV_RECV_0_LEN_PACK判断下传0长度包的端点,再进行处理 */
            }            
            if( intstatus & RB_TX0BYTE_INT )                                    /* Transferred Zero-length Data Packet interrupt */
            {                
                /* 发送0长度数据包中断 */
                USB_DEV_INT_GROP2 |= RB_TX0BYTE_INT;                            /* 清发送0长度数据包中断(写1清0) */
                USB_DEV_SEND_0_LEN_PACK = 0x00;                                 /* 清中断标志 */
                
                /* 自行添加处理,实际需要根据USB_DEV_SEND_0_LEN_PACK判断上传0长度包的端点,再进行处理 */                            
            }            
            if( intstatus & RB_ISO_SEQ_ABORT_INT )                              /* ISO Sequential Abort interrupt */
            {                                
                /* USB ISO异常中断 */
                USB_DEV_INT_GROP2 = RB_ISO_SEQ_ABORT_INT;                       /* 清USB ISO异常中断(写1清0) */            
            }            
            if( intstatus & RB_ISO_SEQ_ERR_INT )                                /* ISO Sequential Error interrupt */
            {            
                /* USB ISO错误中断 */
                USB_DEV_INT_GROP2 = RB_ISO_SEQ_ERR_INT;                         /* 清USB ISO错误中断(写1清0) */            
            }            
            if( intstatus & RB_RESMQ_INT )                                      /* Resume interrupt */
            {                
                /* USB总线恢复中断 */
                USB_DEV_INT_GROP2 = RB_RESMQ_INT;                               /* 清USB总线恢复中断(写1清0) */
                gUsbFlag &= ~DEF_BIT_USB_SUPD;                                  /* 清总线挂起标志 */
                USB_DEV_CONTROL &= ~RB_GOSUSP;                                  /* 清PHY挂起 */            
            }            
            if( intstatus & RB_SUSP_INT )                                       /* Suspend interrupt */
            {                
                /* USB总线挂起中断 */
                USB_DEV_INT_GROP2 = RB_SUSP_INT;                                /* 清USB总线挂起中断(写1清0) */    
                gUsbFlag |= DEF_BIT_USB_SUPD;                                   /* 置总线挂起标志 */
                USB_DEV_CONTROL |= RB_GOSUSP;                                   /* 置PHY挂起 */        
            }            
            if( intstatus & RB_USBRST_INT )                                     /* USB Reset interrupt */
            {    
                USB_DEV_INT_GROP2 = RB_USBRST_INT;            
                gUsbFlag = DEF_BIT_USB_RESET;                                   /*  清其它标志,设置复位标志 */
        
                /* USB总线复位,自动清地址 */                                                                     
                USB_DEV_ADDRESS = 0;                                            /* 清USB设备地址(实际复位时自动清0) */
            }
        }
    }
    else 
    {
        USB_GL_INT_STATUS = USB_GL_INT_STATUS | RB_HC_INT | RB_OTG_INT;         /* 该位写1清0 */  
    }        
}

/*********************************** endfile **********************************/
