/********************************** (C) COPYRIGHT *******************************
 * File Name          : device_host_trans.c
 * Author             : WCH
 * Version            : V1.0.0
 * Date               : 2026/01/19
 * Description        : This file provides all the USB_Device and USB_Host transmit firmware functions.
 *********************************************************************************
 * Copyright (c) 2026 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

/*******************************************************************************/
/* Header File */
#include "device_host_trans.h"
#include "usb_host_config.h"
#include "ch32v407_usbhs_device.h"
#include "usb_host_hid.h"


/*********************************************************************
 * @fn      AnalyzeKeyValue_And_UploadData
 *
 * @brief   Analyze Key Data to Upload Data or Operate Selfdefine Action 
 *
 * @para    id_flag: report id flag
 *          endp: end-point numbers
 *          pubf: data buffer
 *          len: load data length
 *
 * @return  none
 */
void AnalyzeKeyValue_And_UploadData( uint8_t id_flag, uint8_t endp, uint8_t *pbuf, uint16_t len )
{
    uint8_t id;
    uint16_t upload_times = 0;
    static uint8_t s_upload = DEF_STATE_WAIT_WIN; 
    static uint8_t s_selfdefine = 0; 
    uint8_t buf_0[10] = {0};
    uint8_t buf_1[10] = {0x00, 0x00, DEF_KEY_ENTER}; //enter
    uint8_t buf_2[10] = {0x00, 0x00, DEF_KEY_W, DEF_KEY_C, DEF_KEY_H, DEF_KEY_LEFT_POINT, DEF_KEY_C, DEF_KEY_N};//wch.cn 

    id_flag ? (id = 1) : (id = 0);

    if(s_upload == DEF_STATE_WAIT_WIN)
    {
        if(pbuf[id] == DEF_KEY_WIN)
        {
            if(memcmp(pbuf + id + 2, buf_0, len - id - 2))
            {
                s_upload = DEF_STATE_WAIT_WIN; 
            }
            else
            {
                s_upload = DEF_STATE_WAIT_F9;
            }
        }
        else
        {
            s_upload = DEF_STATE_WAIT_WIN; 
        }
    }
    else if(s_upload == DEF_STATE_WAIT_F9)
    { 
        if(pbuf[id + 2] != 0x00)
        {
            if(pbuf[id + 2] == DEF_KEY_F9 && !memcmp(pbuf + id + 3, buf_0, len - id - 3))
            {
                s_upload = DEF_STATE_WAIT_ALL_ZERO;
            }
            else
            {
                s_upload = DEF_STATE_WAIT_WIN; 
            }
        }
    }
    else if (s_upload == DEF_STATE_WAIT_ALL_ZERO ) 
    {
        if(memcmp(pbuf + id, buf_0, len - id))
        {
            s_upload = DEF_STATE_WAIT_WIN; 
        }
        else 
        {
            s_upload = DEF_STATE_UPLOAD_WCHCN;
        }
    }

    if(s_upload == DEF_STATE_UPLOAD_WCHCN)
    {
        USBHS_Endp_DataUp( endp, pbuf, len);//upload data
        while(USBHS_Endp_Busy[ endp ])
        {
            Delay_Us(10);
            if(++upload_times >= UPLOAD_TIME_OUT)
            {
                printf("Upload All Zero Time Out\r\n");
                break;
            }
        }

        Delay_Ms(200);

        s_upload = DEF_STATE_WAIT_WIN; 

        if(id)
        {
            buf_0[0] = pbuf[0]; //report id
            for (uint8_t j = 8; j >= 0; j--) 
            {
                buf_1[j + 1] = buf_1[j];
                buf_2[j + 1] = buf_2[j];
            }
            buf_1[0] = pbuf[0];//report id
            buf_2[0] = pbuf[0];//report id
        }

        USBHS_Endp_DataUp( endp, buf_2, len);//wch.cn
        while(USBHS_Endp_Busy[ endp ])
        {
            Delay_Us(10);
            if(++upload_times >= UPLOAD_TIME_OUT)
            {
                printf("Upload wch.cn Time Out\r\n");
                break;
            }
        }
        upload_times = 0;

        USBHS_Endp_DataUp( endp, buf_0, len);//0
        while(USBHS_Endp_Busy[ endp ])
        {
            Delay_Us(10);
            if(++upload_times >= UPLOAD_TIME_OUT)
            {
                printf("Upload 0 Time Out\r\n");
                break;
            }
        }
        upload_times = 0;

        Delay_Ms(200);

        USBHS_Endp_DataUp( endp, buf_1, len);//Enter
        while(USBHS_Endp_Busy[ endp ])
        {
            Delay_Us(10);
            if(++upload_times >= UPLOAD_TIME_OUT)
            {
                printf("Upload Enter Time Out\r\n");
                break;
            }
        }
        upload_times = 0;

        USBHS_Endp_DataUp( endp, buf_0, len);//0
        while(USBHS_Endp_Busy[ endp ])
        {
            Delay_Us(10);
            if(++upload_times >= UPLOAD_TIME_OUT)
            {
                printf("Upload 0 Time Out\r\n");
                break;
            }
        }
    }
    else
    {
        if(pbuf[id + 2] == DEF_KEY_F2)
        {
            if(memcmp(pbuf + id + 3, buf_0, len - id - 3))
            {
                s_selfdefine = 0;
            }
            else
            {
                s_selfdefine = DEF_KEY_F2;
            }
        }
        if(pbuf[id + 2] == DEF_KEY_F3)
        {
            if(memcmp(pbuf + id + 3, buf_0, len - id - 3))
            {
                s_selfdefine = 0;
            }
            else
            {
                s_selfdefine = DEF_KEY_F3;
            }
        }
        if(pbuf[id + 2] == DEF_KEY_F4)
        {
            if(memcmp(pbuf + id + 3, buf_0, len - id - 3))
            {
                s_selfdefine = 0;
            }
            else
            {
                s_selfdefine = 0;
            }
        }
        if(s_selfdefine == DEF_KEY_F2)
        {
            if((pbuf[id + 2] >= DEF_KEY_LEFT_ONE && pbuf[id + 2] <= DEF_KEY_LEFT_ZERO) || (pbuf[id + 2] >= DEF_KEY_RIGHT_ONE && pbuf[id + 2] <= DEF_KEY_RIGHT_ZERO))
            {
                if(!memcmp(pbuf + id + 3, buf_0, len - id - 3))
                {
                    pbuf[id + 3] = pbuf[id + 2];
                }
            }
        }
        else if(s_selfdefine == DEF_KEY_F3)
        {
            if((pbuf[id + 2] >= DEF_KEY_LEFT_ONE && pbuf[id + 2] <= DEF_KEY_LEFT_ZERO) || (pbuf[id + 2] >= DEF_KEY_RIGHT_ONE && pbuf[id + 2] <= DEF_KEY_RIGHT_ZERO))
            {
                if(!memcmp(pbuf + id + 3, buf_0, len - id - 3))
                {
                    pbuf[id + 3] = pbuf[id + 2];
                    pbuf[id + 4] = pbuf[id + 2];
                }
            }
        }
        USBHS_Endp_DataUp( endp, pbuf, len);//upload data
    }
}

/*********************************************************************
 * @fn      USBH_MainDeal
 *
 * @brief   Provide a simple enumeration process for USB devices and
 *          obtain keyboard and mouse data at regular intervals.
 *
 * @return  none
 */
void USBH_MainDeal( void )
{
    uint8_t  s;
    uint8_t  usb_port;
    uint8_t  index;
    uint8_t  intf_num, in_num;
    uint16_t len;
#if DEF_DEBUG_PRINTF
    uint16_t i;
#endif
    
    for( usb_port = 0; usb_port < DEF_TOTAL_ROOT_HUB; usb_port++ )
    {
        s = USBH_CheckRootHubPortStatus( usb_port ); // Check USB device connection or disconnection
        if( s == ROOT_DEV_CONNECTED )
        {
            DUG_PRINTF( "USB Port%x Dev In.\r\n", usb_port );
            
            /* Set root device state parameters */
            RootHubDev[ usb_port ].bStatus = ROOT_DEV_CONNECTED;
            RootHubDev[ usb_port ].DeviceIndex = usb_port * DEF_ONE_USB_SUP_DEV_TOTAL;

            s = USBH_EnumRootDevice( usb_port ); // Simply enumerate root device
            if( s == ERR_SUCCESS )
            {
                if( RootHubDev[ usb_port ].bType == USB_DEV_CLASS_HID ) // Further enumerate it if this device is a HID device
                {
                    DUG_PRINTF("Root Device Is HID. ");

                    s = USBH_EnumHidDevice( usb_port, RootHubDev[ usb_port ].DeviceIndex, RootHubDev[ usb_port ].bEp0MaxPks );
                    DUG_PRINTF( "Further Enum Result: " );
                    if( s == ERR_SUCCESS )
                    {
                        DUG_PRINTF( "OK\r\n" );
                        
                        /* Set the connection status of the device  */
                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_SUCCESS;

                        USBHS_Device_Init( ENABLE );
                    }
                    else if( s != ERR_USB_DISCON )
                    {
                        DUG_PRINTF( "Err(%02x)\r\n", s );
                        
                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_FAILED;
                    }
                }
                else if( RootHubDev[ usb_port ].bType == USB_DEV_CLASS_HUB )
                {
                    DUG_PRINTF("Root Device Is HUB. ");

                    s = USBH_EnumHubDevice( usb_port, RootHubDev[ usb_port ].bEp0MaxPks );
                    DUG_PRINTF( "Further Enum Result: " );
                    if( s == ERR_SUCCESS )
                    {
                        DUG_PRINTF( "OK\r\n" );

                        /* Set the connection status of the device  */
                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_SUCCESS;
                    }
                    else if( s != ERR_USB_DISCON )
                    {
                        DUG_PRINTF( "Err(%02x)\r\n", s );

                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_FAILED;
                    }
                }
                else // Detect that this device is a Non-HID device
                {
                    DUG_PRINTF( "Root Device Is " );
                    switch( RootHubDev[ usb_port ].bType )
                    {
                        case USB_DEV_CLASS_STORAGE:
                            DUG_PRINTF("Storage. ");
                            break;
                        case USB_DEV_CLASS_PRINTER:
                            DUG_PRINTF("Printer. ");
                            break;
                        case DEF_DEV_TYPE_UNKNOWN:
                            DUG_PRINTF("Unknown. ");
                            break;
                    }
                    DUG_PRINTF( "End Enum.\r\n" );
                    
                    RootHubDev[ usb_port ].bStatus = ROOT_DEV_SUCCESS;
                }
            }
            else if( s != ERR_USB_DISCON )
            {
                /* Enumeration failed */
                DUG_PRINTF( "Enum Fail with Error Code:%x\r\n",s );
                RootHubDev[ usb_port ].bStatus = ROOT_DEV_FAILED;
            }
        }
        else if( s == ROOT_DEV_DISCONNECT )
        {
            DUG_PRINTF( "USB Port%x Dev Out.\r\n", usb_port );
            
            USBHS_Device_Init( DISABLE );

            /* Clear Device parameters */
            KB_LED_Cur_Status = 0x00;
            memset( (void *)USBHS_Endp_Busy, DEF_UEP_FREE, sizeof( USBHS_Endp_Busy ));
            USBHS_DevSleepStatus = 0x00;

            /* Clear Host parameters */
            index = RootHubDev[ usb_port ].DeviceIndex;
            memset( &RootHubDev[ usb_port ].bStatus, 0, sizeof( ROOT_HUB_DEVICE ) );
            memset( &HostCtl[ index ].InterfaceNum, 0, sizeof( HOST_CTL ) );
        }
    }

    /* Get the data of the HID device connected to the USB host port */
    for( usb_port = 0; usb_port < DEF_TOTAL_ROOT_HUB; usb_port++ )
    {
        if( RootHubDev[ usb_port ].bStatus >= ROOT_DEV_SUCCESS )
        {
            index = RootHubDev[ usb_port ].DeviceIndex;
            if( RootHubDev[ usb_port ].bType == USB_DEV_CLASS_HID )
            {
                for( intf_num = 0; intf_num < HostCtl[ index ].InterfaceNum; intf_num++ )
                {
                    for( in_num = 0; in_num < HostCtl[ index ].Interface[ intf_num ].InEndpNum; in_num++ )
                    {   
                        /* Handle keyboard lighting */
                        if( HostCtl[ index ].Interface[ intf_num ].Type == DEC_KEY )
                        {
                            if(HostCtl[ index ].Interface[ intf_num ].SetReport_Value != KB_LED_Cur_Status)
                            {
                                HostCtl[ index ].Interface[ intf_num ].SetReport_Value = KB_LED_Cur_Status;
                                HostCtl[ index ].Interface[ intf_num ].SetReport_Flag = 1;
                            }
                            else 
                            {
                                HostCtl[ index ].Interface[ intf_num ].SetReport_Flag = 0;
                            }
                            if( HostCtl[ index ].Interface[ intf_num ].SetReport_Flag )
                            {
                                KB_SetReport( usb_port, index, RootHubDev[ usb_port ].bEp0MaxPks, intf_num );
                            }
                        }        

                        /* Get endpoint data based on the interval time of the device */
                        if( HostCtl[ index ].Interface[ intf_num ].InEndpTimeCount[ in_num ] >= HostCtl[ index ].Interface[ intf_num ].InEndpInterval[ in_num ] )
                        {
                            HostCtl[ index ].Interface[ intf_num ].InEndpTimeCount[ in_num ] %= HostCtl[ index ].Interface[ intf_num ].InEndpInterval[ in_num ];
               
                            /* Get endpoint data */
                            s = USBH_GetHidData( usb_port, index, intf_num, in_num, Com_Buf, &len );
                            if( s == ERR_SUCCESS )
                            {
                                AnalyzeKeyValue_And_UploadData( HostCtl[ index ].Interface[ intf_num ].IDFlag, HostCtl[ index ].Interface[ intf_num ].InEndpAddr[ in_num ], Com_Buf, len );
#if DEF_DEBUG_PRINTF                        
                                for( i = 0; i < len; i++ )
                                {
                                    DUG_PRINTF( "%02x ", Com_Buf[ i ] );
                                }
                                DUG_PRINTF( "\r\n" );
#endif                           
                            }
                            else if( s == ERR_USB_DISCON )
                            {
                                break;
                            }
                            else if( s == ( USB_PID_STALL | ERR_USB_TRANSFER ) )
                            {
                                /* USB device abnormal event */
                                DUG_PRINTF("Abnormal\r\n");
                                
                                /* Clear endpoint */
                                USBH_ClearEndpStall( usb_port, HostCtl[ index ].Interface[ intf_num ].InEndpAddr[ in_num ] | 0x80 );
                                HostCtl[ index ].Interface[ intf_num ].InEndpTog[ in_num ] = 0x00;
                                
                                /* Judge the number of error */
                                HostCtl[ index ].ErrorCount++;
                                if( HostCtl[ index ].ErrorCount >= 10 )
                                {
                                    /* Re-enumerate the device and clear the endpoint again */
                                    memset( &RootHubDev[ usb_port ].bStatus, 0, sizeof( ROOT_HUB_DEVICE ) );
                                    s = USBH_EnumRootDevice( usb_port );
                                    if( s == ERR_SUCCESS )
                                    {
                                        USBH_ClearEndpStall( usb_port, HostCtl[ index ].Interface[ intf_num ].InEndpAddr[ in_num ] | 0x80 );
                                        HostCtl[ index ].ErrorCount = 0x00;
                                        
                                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_CONNECTED; 
                                        RootHubDev[ usb_port ].DeviceIndex = usb_port * DEF_ONE_USB_SUP_DEV_TOTAL;
                                        
                                        memset( &HostCtl[ index ].InterfaceNum, 0, sizeof( HOST_CTL ) );
                                        s = USBH_EnumHidDevice( usb_port, index, RootHubDev[ usb_port ].bEp0MaxPks );
                                        if( s == ERR_SUCCESS )
                                        {
                                            RootHubDev[ usb_port ].bStatus = ROOT_DEV_SUCCESS; 
                                        }
                                        else if( s != ERR_USB_DISCON )
                                        {
                                            RootHubDev[ usb_port ].bStatus = ROOT_DEV_FAILED; 
                                        }
                                    }
                                    else if( s != ERR_USB_DISCON )
                                    {
                                        RootHubDev[ usb_port ].bStatus = ROOT_DEV_FAILED;
                                    }
                                }
                            }
                        }
                    }

                    if( s == ERR_USB_DISCON )
                    {
                        break;
                    }
                }
            }
        }
    }
}

/*********************************************************************
 * @fn      USB_Sleep_Wakeup_CFG
 *
 * @brief   Configure USB wake up mode
 *
 * @return  none
 */
void USB_Sleep_Wakeup_CFG( void )
{
    EXTI_InitTypeDef EXTI_InitStructure = { 0 };

    EXTI_InitStructure.EXTI_Line = EXTI_DEVICE | EXTI_HOST;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Event;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init( &EXTI_InitStructure );

    EXTI->INTENR |= EXTI_INTENR_MR18 | EXTI_INTENR_MR20;
}

/*********************************************************************
 * @fn      MCU_Sleep_Wakeup_Operate
 *
 * @brief   Perform sleep operation
 *
 * @return  none
 */
void MCU_Sleep_Wakeup_Operate( void )
{
    printf( "Sleep\r\n" );
    __disable_irq( );
    USBHSD->CONTROL &= ~USBHS_UH_PHY_SUSPENDM;
    EXTI_ClearFlag( EXTI_HOST | EXTI_DEVICE );

    HID_SetFeature(RootHubDev[ 0 ].bEp0MaxPks, 0, 0, 1 );
    
    PWR_EnterSTOPMode(PWR_Regulator_LowPower,PWR_STOPEntry_WFE);
    USBHSD->CONTROL |= USBHS_UD_PHY_SUSPENDM;
    SystemInit();
    SystemCoreClockUpdate();
    USBHSH_RCC_Init(ENABLE);
    USBHSD_RCC_Init(ENABLE);

    if( EXTI_GetFlagStatus( EXTI_DEVICE ) != RESET )//pc wake up kb
    {
        EXTI_ClearFlag( EXTI_DEVICE );
    }
    else if( EXTI_GetFlagStatus( EXTI_HOST) != RESET )// kb wake up pc
    {
        EXTI_ClearFlag( EXTI_HOST );
        USBHS_Send_Resume( );
        Delay_Ms(100);//wait kb can receive data
    }

    HID_ClearFeature(RootHubDev[ 0 ].bEp0MaxPks, 0, 0, 1 );

    __enable_irq( );
    printf( "Wake\r\n" );
}
