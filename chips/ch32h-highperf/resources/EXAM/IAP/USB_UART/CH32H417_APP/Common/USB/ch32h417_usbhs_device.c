/********************************** (C) COPYRIGHT *******************************
 * File Name          : ch32h417_usbhs_device.c
 * Author             : WCH
 * Version            : V1.0.1
 * Date               : 2026/04/10
 * Description        : This file provides all the USBHS firmware functions.
 *********************************************************************************
 * Copyright (c) 2025 Nanjing Qinheng Microelectronics Co., Ltd.
 * Attention: This software (modified or not) and binary are used for
 * microcontroller manufactured by Nanjing Qinheng Microelectronics.
 *******************************************************************************/

#include "ch32h417_usbhs_device.h"
#include "usb_desc.h"
#include "usb_inf.h"

/******************************************************************************/
/* Variable Definition */
/* Global */
const uint8_t *pUSBHS_Descr;

/* Setup Request */
volatile uint8_t USBHS_SetupReqCode;
volatile uint8_t USBHS_SetupReqType;
volatile uint16_t USBHS_SetupReqValue;
volatile uint16_t USBHS_SetupReqIndex;
volatile uint16_t USBHS_SetupReqLen;

/* USB Device Status */
volatile uint8_t USBHS_DevConfig;
volatile uint8_t USBHS_DevAddr;
volatile uint16_t USBHS_DevMaxPackLen;
volatile uint8_t USBHS_DevSpeed;
volatile uint8_t USBHS_DevSleepStatus;
volatile uint8_t USBHS_DevEnumStatus;

/* HID Class Command */
volatile uint8_t USBHS_HidIdle;

/* Endpoint Buffer */
__attribute__((aligned(4))) uint8_t USBHS_EP0_Buf[DEF_USBD_UEP0_SIZE];
__attribute__((aligned(4))) uint8_t USBHS_EP2_Rx_Buf[DEF_IAP_USB_PACK_SIZE];

void USBHS_IRQHandler(void) __attribute__((interrupt("WCH-Interrupt-fast")));

/*********************************************************************
 * @fn      USBHS_Device_Endp_Init
 *
 * @brief   Initializes USB device endpoints.
 *
 * @return  none
 */
void USBHS_Device_Endp_Init(void)
{
    USBHSD->UEP_TX_EN = USBHS_UEP0_T_EN | USBHS_UEP2_T_EN;
    USBHSD->UEP_RX_EN = USBHS_UEP0_R_EN | USBHS_UEP2_R_EN;

    USBHSD->UEP0_MAX_LEN = DEF_USBD_UEP0_SIZE;
    USBHSD->UEP1_MAX_LEN = DEF_IAP_USB_PACK_SIZE;
    USBHSD->UEP2_MAX_LEN = DEF_IAP_USB_PACK_SIZE;

    USBHSD->UEP0_DMA = (uint32_t)(uint8_t *)USBHS_EP0_Buf;
    USBHSD->UEP2_RX_DMA = (uint32_t)(uint8_t *)USBHS_EP2_Rx_Buf;

    USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_RES_NAK;
    USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_RES_ACK;

    USBHSD->UEP2_TX_CTRL = USBHS_UEP_T_RES_NAK;
    USBHSD->UEP2_RX_CTRL = USBHS_UEP_R_RES_ACK;
}

/*********************************************************************
 * @fn      USBHS_RCC_Init
 *
 * @brief   Initializes USB high-speed rcc.
 *
 * @return  none
 */
void USBHS_RCC_Init(FunctionalState sta)
{
    if(sta)
    {
        if((RCC->PLLCFGR & RCC_SYSPLL_SEL) != RCC_SYSPLL_USBHS)
        {
            /* Initialize USBHS 480M PLL */
            RCC_USBHS_PLLCmd(DISABLE);
            RCC_USBHSPLLCLKConfig(RCC_USBHSPLLSource_HSE);
            RCC_USBHSPLLReferConfig(RCC_USBHSPLLRefer_25M);
            RCC_USBHSPLLClockSourceDivConfig(RCC_USBHSPLL_IN_Div1);
            RCC_USBHS_PLLCmd(ENABLE);
            while (!(RCC->CTLR & RCC_USBHS_PLLRDY));
        }
        /* Enable UTMI Clock */
        RCC_UTMIcmd(ENABLE);
        /* Enable USBHS Clock */
        RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS, ENABLE);
    }
    else
    {
        RCC_HBPeriphClockCmd(RCC_HBPeriph_USBHS, DISABLE);
        RCC_UTMIcmd(DISABLE);
        if((RCC->PLLCFGR & RCC_SYSPLL_SEL) != RCC_SYSPLL_USBHS)
        {
            RCC_USBHS_PLLCmd(DISABLE);
        }
    }
}

/*********************************************************************
 * @fn      USBHS_Device_Init
 *
 * @brief   Initializes USB high-speed device.
 *
 * @return  none
 */
void USBHS_Device_Init(FunctionalState sta)
{
    if(sta)
    {
        USBHS_RCC_Init(ENABLE);
        USBHSD->CONTROL = USBHS_UD_RST_LINK | USBHS_UD_PHY_SUSPENDM;
        USBHSD->INT_EN = USBHS_UDIE_BUS_RST | USBHS_UDIE_SUSPEND | USBHS_UDIE_BUS_SLEEP | USBHS_UDIE_LPM_ACT | USBHS_UDIE_TRANSFER | USBHS_UDIE_LINK_RDY;
        USBHS_Device_Endp_Init();
        USBHSD->BASE_MODE = USBHS_UD_SPEED_HIGH;
        USBHSD->CONTROL = USBHS_UD_DEV_EN | USBHS_UD_DMA_EN | USBHS_UD_LPM_EN | USBHS_UD_PHY_SUSPENDM;
        NVIC_EnableIRQ(USBHS_IRQn);
    }
    else
    {
        USBHSD->CONTROL = USBHS_UD_RST_SIE | USBHS_UD_RST_LINK;
        NVIC_DisableIRQ(USBHS_IRQn);
        USBHS_RCC_Init(DISABLE);
    }
}

/*********************************************************************
 * @fn      USBHS_IRQHandler
 *
 * @brief   This function handles USBHS exception.
 *
 * @return  none
 */
void USBHS_IRQHandler(void)
{
    uint8_t intflag, intst, errflag;
    uint16_t len;
    uint8_t endp_num;

    intflag = USBHSD->INT_FG;
    intst = USBHSD->INT_ST;

    if(intflag & USBHS_UDIF_TRANSFER)
    {
        endp_num = intst & USBHS_UDIS_EP_ID_MASK;
        if(!(intst & USBHS_UDIS_EP_DIR))  // SETUP/OUT Transaction
        {
            switch(endp_num)
            {
            case DEF_UEP0:
                USBHSD->UEP0_RX_CTRL &= ~USBHS_UEP_R_DONE;
                if((USBHSD->UEP0_RX_CTRL & USBHS_UEP_R_SETUP_IS) && !(USBHSD->UEP0_RX_CTRL & USBHS_UEP_R_DONE))
                {
                    /* Store All Setup Values */
                    USBHS_SetupReqType = pUSBHS_SetupReqPak->bRequestType;
                    USBHS_SetupReqCode = pUSBHS_SetupReqPak->bRequest;
                    USBHS_SetupReqLen = pUSBHS_SetupReqPak->wLength;
                    USBHS_SetupReqValue = pUSBHS_SetupReqPak->wValue;
                    USBHS_SetupReqIndex = pUSBHS_SetupReqPak->wIndex;

                    len = 0;
                    errflag = 0;
                    if((USBHS_SetupReqType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD)
                    {
                        /* usb non-standard request processing */
                        if((USBHS_SetupReqType & USB_REQ_TYP_MASK) == USB_REQ_TYP_CLASS)
                        {
                            /* Class request */
                        }
                        else if(USBHS_SetupReqType & USB_REQ_TYP_VENDOR)
                        {
                            /* Manufacturer request */
                        }
                        else
                        {
                            errflag = 0xFF;
                        }
                    }
                    else
                    {
                        /* usb standard request processing */
                        switch(USBHS_SetupReqCode)
                        {
                        /* get device/configuration/string/report/... descriptors */
                        case USB_GET_DESCRIPTOR:
                            switch((uint8_t)(USBHS_SetupReqValue >> 8))
                            {
                            /* get usb device descriptor */
                            case USB_DESCR_TYP_DEVICE:
                                pUSBHS_Descr = MyDevDescr;
                                len = DEF_USBD_DEVICE_DESC_LEN;
                                break;

                            /* get usb configuration descriptor */
                            case USB_DESCR_TYP_CONFIG:
                                pUSBHS_Descr = MyCfgDescr;
                                len = DEF_USBD_CONFIG_DESC_LEN;
                                break;

                            /* get usb string descriptor */
                            case USB_DESCR_TYP_STRING:
                                switch((uint8_t)(USBHS_SetupReqValue & 0xFF))
                                {
                                /* Descriptor 0, Language descriptor */
                                case DEF_STRING_DESC_LANG:
                                    pUSBHS_Descr = MyLangDescr;
                                    len = DEF_USBD_LANG_DESC_LEN;
                                    break;

                                /* Descriptor 1, Manufacturers String descriptor */
                                case DEF_STRING_DESC_MANU:
                                    pUSBHS_Descr = MyManuInfo;
                                    len = DEF_USBD_MANU_DESC_LEN;
                                    break;

                                /* Descriptor 2, Product String descriptor */
                                case DEF_STRING_DESC_PROD:
                                    pUSBHS_Descr = MyProdInfo;
                                    len = DEF_USBD_PROD_DESC_LEN;
                                    break;

                                /* Descriptor 3, Serial-number String descriptor */
                                case DEF_STRING_DESC_SERN:
                                    pUSBHS_Descr = MySerNumInfo;
                                    len = DEF_USBD_SN_DESC_LEN;
                                    break;

                                default:
                                    errflag = 0xFF;
                                    break;
                                }
                                break;
#if (DEF_USB_APP_MODE == DEF_USB_APP_MODE_HID)
                            case USB_DESCR_TYP_REPORT:
                                pUSBHS_Descr = MyHIDReportDesc;
                                len = DEF_USBD_REPORT_DESC_LEN;
                                break;
#endif
                            case USB_DESCR_TYP_HID:
                                if(USBHS_SetupReqIndex == 0x00)
                                {
                                    pUSBHS_Descr = &MyCfgDescr[18];
                                    len = 9;
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                                break;

                            default:
                                errflag = 0xFF;
                                break;
                            }
                            /* Copy Descriptors to Endp0 DMA buffer */
                            if(USBHS_SetupReqLen > len)
                            {
                                USBHS_SetupReqLen = len;
                            }
                            len = (USBHS_SetupReqLen >= DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : USBHS_SetupReqLen;
                            memcpy(USBHS_EP0_Buf, pUSBHS_Descr, len);
                            pUSBHS_Descr += len;
                            break;

                        /* Set usb address */
                        case USB_SET_ADDRESS:
                            USBHS_DevAddr = (uint16_t)(USBHS_SetupReqValue & 0xFF);
                            break;

                        /* Get usb configuration now set */
                        case USB_GET_CONFIGURATION:
                            USBHS_EP0_Buf[0] = USBHS_DevConfig;
                            if(USBHS_SetupReqLen > 1)
                            {
                                USBHS_SetupReqLen = 1;
                            }
                            break;

                        /* Set usb configuration to use */
                        case USB_SET_CONFIGURATION:
                            USBHS_DevConfig = (uint8_t)(USBHS_SetupReqValue & 0xFF);
                            USBHS_DevEnumStatus = 0x01;
                            break;

                        /* Clear or disable one usb feature */
                        case USB_CLEAR_FEATURE:
                            if((USBHS_SetupReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE)
                            {
                                /* clear one device feature */
                                if((uint8_t)(USBHS_SetupReqValue & 0xFF) == 0x01)
                                {
                                    /* clear usb sleep status, device not prepare to sleep */
                                    USBHS_DevSleepStatus &= ~0x01;
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                            }
                            else if((USBHS_SetupReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
                            {
                                /* Set End-point Feature */
                                if((uint8_t)(USBHS_SetupReqValue & 0xFF) == USB_REQ_FEAT_ENDP_HALT)
                                {
                                    /* Clear End-point Feature */
                                    switch((uint8_t)(USBHS_SetupReqIndex & 0xFF))
                                    {
                                    case(DEF_UEP2 | DEF_UEP_IN):
                                        /* Set End-point 2 IN NAK */
                                        USBHSD->UEP2_TX_CTRL = USBHS_UEP_T_RES_NAK;
                                        break;

                                    case(DEF_UEP2 | DEF_UEP_OUT):
                                        /* Set End-point 2 OUT ACK */
                                        USBHSD->UEP2_RX_CTRL = USBHS_UEP_R_RES_ACK;
                                        break;

                                    default:
                                        errflag = 0xFF;
                                        break;
                                    }
                                }
                                else
                                {
                                    errflag = 0xFF;
                                }
                            }
                            else
                            {
                                errflag = 0xFF;
                            }
                            break;

                        /* set or enable one usb feature */
                        case USB_SET_FEATURE:
                            if((USBHS_SetupReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE)
                            {
                                errflag = 0xFF;
                            }
                            else if((USBHS_SetupReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
                            {
                                /* Set End-point Feature */
                                if((uint8_t)(USBHS_SetupReqValue & 0xFF) == USB_REQ_FEAT_ENDP_HALT)
                                {
                                    /* Set end-points status stall */
                                    switch((uint8_t)(USBHS_SetupReqIndex & 0xFF))
                                    {
                                    case(DEF_UEP2 | DEF_UEP_IN):
                                        /* Set End-point 2 IN STALL */
                                        USBHSD->UEP2_TX_CTRL = (USBHSD->UEP2_TX_CTRL & ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_STALL;
                                        break;

                                    case(DEF_UEP2 | DEF_UEP_OUT):
                                        /* Set End-point 2 OUT STALL */
                                        USBHSD->UEP2_RX_CTRL = (USBHSD->UEP2_RX_CTRL & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_STALL;
                                        break;
                                    default:
                                        errflag = 0xFF;
                                        break;
                                    }
                                }
                            }
                            break;

                        /* This request allows the host to select another setting for the specified interface  */
                        case USB_GET_INTERFACE:
                            USBHS_EP0_Buf[0] = 0x00;
                            if(USBHS_SetupReqLen > 1)
                            {
                                USBHS_SetupReqLen = 1;
                            }
                            break;

                        case USB_SET_INTERFACE:
                            break;

                        /* host get status of specified device/interface/end-points */
                        case USB_GET_STATUS:
                            USBHS_EP0_Buf[0] = 0x00;
                            USBHS_EP0_Buf[1] = 0x00;
                            if((USBHS_SetupReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_ENDP)
                            {
                                switch((uint8_t)(USBHS_SetupReqIndex & 0xFF))
                                {
                                case(DEF_UEP2 | DEF_UEP_IN):
                                    if(((USBHSD->UEP2_TX_CTRL) & USBHS_UEP_T_RES_MASK) == USBHS_UEP_T_RES_STALL)
                                    {
                                        USBHS_EP0_Buf[0] = 0x01;
                                    }
                                    break;

                                case(DEF_UEP2 | DEF_UEP_OUT):
                                    if(((USBHSD->UEP2_RX_CTRL) & USBHS_UEP_R_RES_MASK) == USBHS_UEP_R_RES_STALL)
                                    {
                                        USBHS_EP0_Buf[0] = 0x01;
                                    }
                                    break;
                                    break;
                                default:
                                    errflag = 0xFF;
                                    break;
                                }
                            }
                            else if((USBHS_SetupReqType & USB_REQ_RECIP_MASK) == USB_REQ_RECIP_DEVICE)
                            {
                                if(USBHS_DevSleepStatus & 0x01)
                                {
                                    USBHS_EP0_Buf[0] = 0x02;
                                }
                            }

                            if(USBHS_SetupReqLen > 2)
                            {
                                USBHS_SetupReqLen = 2;
                            }
                            break;

                        default:
                            errflag = 0xFF;
                            break;
                        }
                    }

                    /* errflag = 0xFF means a request not support or some errors occurred, else correct */
                    if(errflag == 0xFF)
                    {
                        /* if one request not support, return stall */
                        USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_STALL;
                        USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_STALL;
                    }
                    else
                    {
                        /* end-point 0 data Tx/Rx */
                        if(USBHS_SetupReqType & DEF_UEP_IN)
                        {
                            /* tx */
                            len = (USBHS_SetupReqLen > DEF_USBD_UEP0_SIZE) ? DEF_USBD_UEP0_SIZE : USBHS_SetupReqLen;
                            USBHS_SetupReqLen -= len;
                            USBHSD->UEP0_TX_LEN = len;
                            USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_ACK;
                        }
                        else
                        {
                            /* rx */
                            if(USBHS_SetupReqLen == 0)
                            {
                                USBHSD->UEP0_TX_LEN = 0;
                                USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_ACK;
                            }
                            else
                            {
                                USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_ACK;
                            }
                        }
                    }
                }
                /* end-point 0 data out interrupt */
                else
                {

                    USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_RES_NAK;  // clear
                    len = USBHSD->UEP0_RX_LEN;

                    /* if any processing about rx, set it here */
                    if((USBHS_SetupReqType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD)
                    {

						USBHS_SetupReqLen = 0;
                    }
                    else
                    {
                        /* Standard request end-point 0 Data download */
                        USBHS_SetupReqLen = 0;
                    }

                    if(USBHS_SetupReqLen == 0)
                    {
                        USBHSD->UEP0_TX_LEN = 0;
                        USBHSD->UEP0_TX_CTRL = USBHS_UEP_T_TOG_DATA1 | USBHS_UEP_T_RES_ACK;
                    }
                }
                
                break;
            /* end-point 2 data out interrupt */
            case DEF_UEP2:
                USBHSD->UEP2_RX_CTRL &= ~USBHS_UEP_R_DONE;
                if(USBHSD->UEP2_RX_CTRL & USBHS_UEP_R_TOG_MATCH)
                {
                    USB_Pack_Out_Callback(DEF_USB_PORT_HS, USBHSD->UEP2_RX_LEN & 0xFF);
                    USBHSD->UEP2_RX_CTRL ^= USBHS_UEP_R_TOG_DATA1;
                    USBHSD->UEP2_RX_CTRL = ((USBHSD->UEP2_RX_CTRL) & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK;
                }
                else
                {
                    USBHSD->UEP2_RX_CTRL = ((USBHSD->UEP2_RX_CTRL) & ~USBHS_UEP_R_RES_MASK) | USBHS_UEP_R_RES_ACK;
                }
                break;

            default:
                errflag = 0xFF;
                break;
            }
        }
        else
        {
            /* data-in stage processing */
            switch(endp_num)
            {
            /* end-point 0 data in interrupt */
            case DEF_UEP0:
                USBHSD->UEP0_TX_CTRL &= ~USBHS_UEP_T_DONE;
                if(USBHS_SetupReqLen == 0)
                {
                    USBHSD->UEP0_RX_CTRL = USBHS_UEP_R_TOG_DATA1 | USBHS_UEP_R_RES_ACK;
                }

                if((USBHS_SetupReqType & USB_REQ_TYP_MASK) != USB_REQ_TYP_STANDARD)
                {
                    /* Non-standard request endpoint 0 Data upload */
                }
                else
                {
                    /* Standard request endpoint 0 Data upload */
                    switch(USBHS_SetupReqCode)
                    {
                    case USB_GET_DESCRIPTOR:
                        len = USBHS_SetupReqLen >= DEF_USBD_UEP0_SIZE ? DEF_USBD_UEP0_SIZE : USBHS_SetupReqLen;
                        memcpy(USBHS_EP0_Buf, pUSBHS_Descr, len);
                        USBHS_SetupReqLen -= len;
                        pUSBHS_Descr += len;
                        USBHSD->UEP0_TX_LEN = len;
                        USBHSD->UEP0_TX_CTRL ^= USBHS_UEP_T_TOG_DATA1;
                        USBHSD->UEP0_TX_CTRL = (USBHSD->UEP0_TX_CTRL & ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_ACK;  // clear
                        break;

                    case USB_SET_ADDRESS:
                        USBHSD->DEV_AD = USBHS_DevAddr;
                        break;

                    default:
                        USBHSD->UEP0_TX_LEN = 0;
                        break;
                    }
                }
                break;

            /* end-point 2 data in interrupt */
            case DEF_UEP2:
                USBHSD->UEP2_TX_CTRL &= ~USBHS_UEP_T_DONE;
                USBHSD->UEP2_TX_CTRL = (USBHSD->UEP2_TX_CTRL & ~USBHS_UEP_T_RES_MASK) | USBHS_UEP_T_RES_NAK;
                USBHSD->UEP2_TX_CTRL ^= USBHS_UEP_T_TOG_DATA1;
                
                break;
            default:
                break;
            }
        }
    }    
	else if(intflag & USBHS_UDIF_LINK_RDY)
    {
        USBHSD->INT_FG = USBHS_UDIF_LINK_RDY;
    }
    else if(intflag & USBHS_UDIF_SUSPEND)
    {
        USBHSD->INT_FG = USBHS_UDIF_SUSPEND;
        /* usb suspend interrupt processing */
        if(USBHSD->MIS_ST & USBHS_UDMS_SUSPEND)
        {
            USBHS_DevSleepStatus |= 0x02;
            if(USBHS_DevSleepStatus == 0x03)
            {
                /* Handling usb sleep here */
            }
        }
        else
        {
            USBHS_DevSleepStatus &= ~0x02;
        }

    }
    else if(intflag & USBHS_UDIF_BUS_RST)
    {
        /* usb reset interrupt processing */
        USBHS_DevConfig = 0;
        USBHS_DevAddr = 0;
        USBHS_DevSleepStatus = 0;
        USBHS_DevEnumStatus = 0;

        USBHSD->DEV_AD = 0;
        USBHS_Device_Endp_Init();
        USBHSD->INT_FG = USBHS_UDIF_BUS_RST;
    }
    else
    {
        /* other interrupts */
        USBHSD->INT_FG = intflag;
    }
}
