/********************************** (C) COPYRIGHT *******************************
* File Name          : eth_driver.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2026/02/04
* Description        : eth program body.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/

#include "string.h"
#include "eth_driver.h"

__attribute__((__aligned__(4))) ETH_DMADESCTypeDef DMARxDscrTab[ETH_RXBUFNB];       /* MAC receive descriptor, 4-byte aligned*/
__attribute__((__aligned__(4))) ETH_DMADESCTypeDef DMATxDscrTab[ETH_TXBUFNB];       /* MAC send descriptor, 4-byte aligned */

__attribute__((__aligned__(4))) uint8_t  MACRxBuf[ETH_RXBUFNB*ETH_RX_BUF_SZE];      /* MAC receive buffer, 4-byte aligned */
__attribute__((__aligned__(4))) uint8_t  MACTxBuf[ETH_TXBUFNB*ETH_TX_BUF_SZE];      /* MAC send buffer, 4-byte aligned */

uint16_t gPHYAddress;
volatile uint32_t LocalTime;
volatile uint8_t PhyWaitNegotiationSuc = 0;
ETH_DMADESCTypeDef *pDMARxSet;
ETH_DMADESCTypeDef *pDMATxSet;

volatile uint8_t LinkSta = 0;  //0:Link down 1:Link up
void ETH_LinkDownCfg(void);
void PHY_FuncInit(void);
/*********************************************************************
 * @fn      WCHNET_GetMacAddr
 *
 * @brief   Get the MAC address
 *
 * @return  none.
 */
void WCHNET_GetMacAddr( uint8_t *p )
{
    uint8_t i;
    uint8_t *macaddr = (uint8_t *)(ROM_CFG_USERADR_ID+5);

    for(i=0;i<6;i++)
    {
        *p = *macaddr;
        p++;
        macaddr--;
    }
}

/*********************************************************************
 * @fn      WCHNET_TimeIsr
 *
 * @brief
 *
 * @return  none.
 */
void WCHNET_TimeIsr( uint16_t timperiod )
{
    LocalTime += timperiod;
}

/*********************************************************************
 * @fn      WCHNET_QueryPhySta
 *
 * @brief   Query external PHY status
 *
 * @return  none.
 */

void WCHNET_QueryPhySta(void)
{
    if(PhyWaitNegotiationSuc)
    {
        ETH_PHYLink();
    }
}

/*********************************************************************
 * @fn      RecDataPolling
 *
 * @brief   process received data.
 *
 * @param   none.
 *
 * @return  none.
 */
void RecDataPolling(void)
{
    uint32_t length, buffer;

    while(!(pDMARxSet->Status & ETH_DMARxDesc_OWN))
    {
        if( !(pDMARxSet->Status & ETH_DMARxDesc_ES) &&\
        (pDMARxSet->Status & ETH_DMARxDesc_LS) &&\
        (pDMARxSet->Status & ETH_DMARxDesc_FS) )
        {
            /* Get the Frame Length of the received packet: substruct 4 bytes of the CRC */
            length = (pDMARxSet->Status & ETH_DMARxDesc_FL) >> 16;
            /* Get the addrees of the actual buffer */
            buffer = pDMARxSet->Buffer1Addr;

            /* Do something*/
            printf("rec data:%d bytes\r\n",length);
            printf("data:%x\r\n",*((uint8_t *)buffer));
        }
        pDMARxSet->Status = ETH_DMARxDesc_OWN;
        pDMARxSet = (ETH_DMADESCTypeDef *)pDMARxSet->Buffer2NextDescAddr;
    }
}

/*********************************************************************
 * @fn      WCHNET_MainTask
 *
 * @brief   library main task function
 *
 * @param   none.
 *
 * @return  none.
 */
void WCHNET_MainTask(void)
{
    // RecDataPolling();
    WCHNET_QueryPhySta();                   /* Query external PHY status */
}

/*********************************************************************
 * @fn      ETH_SetClock
 *
 * @brief   Set the Ethernet related clock
 *
 * @param   none.
 *
 * @return  none.
 */
void ETH_SetClock(void)
{
    /* Only support HSE_VALUE: 25MHz.*/
    RCC->CTLR |= 1<<22;
    while((RCC->CTLR & (1<<23)) == 0);
}

/*********************************************************************
 * @fn      ETH_LinkUpCfg
 *
 * @brief   When the PHY is connected, configure the relevant functions.
 *
 * @param   none.
 *
 * @return  none.
 */
void ETH_LinkUpCfg(void)
{
    uint16_t phy_stat;

    printf("Link up\r\n");
    LinkSta = 1;
    PhyWaitNegotiationSuc = 0;
    ETH_Start( );
    ETH_WritePHYRegister(gPHYAddress, PHY_PAG_SEL, PHY_REG_PAGE0 );
    phy_stat = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_STATUS );
    if(phy_stat & PHY_SPEED_100M_MODE)
    {
        if(phy_stat & PHY_FULL_DUPLEX_MODE)
        {
            ETH->MACCR |= ETH_Mode_FullDuplex;
        }
        else
        {
            ETH->MACCR &= ~ETH_Mode_FullDuplex;
        } 
        ETH->MACCR &= ~(3<<14); 
        ETH->MACCR |= ETH_Speed_100M;
    }
    else {
        if(phy_stat & PHY_FULL_DUPLEX_MODE)
        {
            ETH->MACCR |= ETH_Mode_FullDuplex;
        }
        else
        {
            ETH->MACCR &= ~ETH_Mode_FullDuplex;
        } 
        ETH->MACCR &= ~(3<<14); 
        ETH->MACCR |= ETH_Speed_10M;
    }
}

/*********************************************************************
 * @fn      ETH_LinkDownCfg
 *
 * @brief   When the PHY is disconnected, configure the relevant functions.
 *
 * @param   none.
 *
 * @return  none.
 */
void ETH_LinkDownCfg(void)
{
    printf("Link down\r\n");
    LinkSta = 0;
}

/*********************************************************************
 * @fn      ETH_PHYLink
 *
 * @brief   Configure MAC parameters after the PHY Link is successful.
 *
 * @param   none.
 *
 * @return  none.
 */
void ETH_PHYLink( void )
{
    u32 phy_bsr, phy_anlpar, phy_bcr;

    phy_bsr = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_BSR );
    phy_anlpar = ETH_ReadPHYRegister( PHY_ADDRESS, PHY_ANLPAR);
    phy_bcr = ETH_ReadPHYRegister( gPHYAddress, PHY_BCR);

    if(phy_bsr & PHY_Linked_Status)   //LinkUp
    {
        if(phy_bcr & PHY_AutoNegotiation)
        {
            if(phy_anlpar == 0)
            {
                ETH_LinkUpCfg();
            }
            else {
                if(phy_bsr & PHY_AutoNego_Complete)
                {
                    ETH_LinkUpCfg();
                }
                else{
                    PhyWaitNegotiationSuc = 1;
                }
            }
        }
        else {
            ETH_LinkUpCfg();
        }
    }
    else {                              //LinkDown
        /*Link down*/
        ETH_LinkDownCfg();
    }
    ETH_WritePHYRegister(gPHYAddress, PHY_PAG_SEL, PHY_REG_PAGE0 );
    ETH_ReadPHYRegister( gPHYAddress, PHY_INTERRUPT_IND);   /* Clear the Interrupt status */
}

void PHY_LEDCfg(void)
{
    uint16_t RegValue;
    GPIO_InitTypeDef GPIO_InitStructure = {0};

    RCC_PB2PeriphClockCmd(RCC_PB2Periph_AFIO | RCC_PB2Periph_GPIOD, ENABLE);
    AFIO->PCFR1 |= 1<<31;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, GPIO_Pin_14);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_High;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    GPIO_ResetBits(GPIOD, GPIO_Pin_15);

    ETH_WritePHYRegister(gPHYAddress, PHY_PAG_SEL, PHY_REG_PAGE7 );
    //turn on LED--PHY
    RegValue = ETH_ReadPHYRegister(gPHYAddress, PHY_INTERRUPT_MASK);
    RegValue |= 1<<9;
    ETH_WritePHYRegister(gPHYAddress, PHY_INTERRUPT_MASK, RegValue );

    //Set LED Mode
    // RegValue = ETH_ReadPHYRegister(gPHYAddress, PHY_LED_CTRL);
    // RegValue |= 5<<5;
    // ETH_WritePHYRegister(gPHYAddress, PHY_LED_CTRL, RegValue );
}

/*********************************************************************
 * @fn      PHY_InterruptInit
 *
 * @brief   Configure PHY interrupt function
 *
 * @param   none.
 *
 * @return  none.
 */
void PHY_InterruptInit(void)
{
    uint16_t RegValue;

    ETH_WritePHYRegister(gPHYAddress, PHY_PAG_SEL, PHY_REG_PAGE7 );
    /* Configure interrupt function */
    RegValue = ETH_ReadPHYRegister(gPHYAddress, PHY_INTERRUPT_MASK);
    RegValue |= 0x01 << 13;
    ETH_WritePHYRegister(gPHYAddress, PHY_INTERRUPT_MASK, RegValue );
    /* Clear the Interrupt status */
    ETH_WritePHYRegister(gPHYAddress, PHY_PAG_SEL, PHY_REG_PAGE0 );
    ETH_ReadPHYRegister( gPHYAddress, PHY_INTERRUPT_IND);
}

/*********************************************************************
 * @fn      ETH_RegInit
 *
 * @brief   ETH register initialization.
 *
 * @param   ETH_InitStruct:initialization struct.
 *          PHYAddress:PHY address.
 *
 * @return  Initialization status.
 */
uint32_t ETH_RegInit( ETH_InitTypeDef* ETH_InitStruct, uint16_t PHYAddress )
{
    uint16_t tmpreg = 0;
    /* Set the SMI interface clock, set as the main frequency divided by 42  */
    ETH->MACMIIAR = (uint32_t)ETH_MACMIIAR_CR_Div42;

    /*------------------------ MAC register configuration  -------------------------------------------*/
    ETH->MACCR = (uint32_t)(ETH_InitStruct->ETH_Watchdog |
                  ETH_InitStruct->ETH_Jabber |
                  ETH_InitStruct->ETH_InterFrameGap |
                  ETH_InitStruct->ETH_ChecksumOffload |
                  ETH_InitStruct->ETH_AutomaticPadCRCStrip |
                  ETH_InitStruct->ETH_LoopbackMode);

    ETH->MACFFR = (uint32_t)(ETH_InitStruct->ETH_ReceiveAll |
                          ETH_InitStruct->ETH_SourceAddrFilter |
                          ETH_InitStruct->ETH_PassControlFrames |
                          ETH_InitStruct->ETH_BroadcastFramesReception |
                          ETH_InitStruct->ETH_DestinationAddrFilter |
                          ETH_InitStruct->ETH_PromiscuousMode |
                          ETH_InitStruct->ETH_MulticastFramesFilter |
                          ETH_InitStruct->ETH_UnicastFramesFilter);

    ETH->MACHTHR = (uint32_t)ETH_InitStruct->ETH_HashTableHigh;
    ETH->MACHTLR = (uint32_t)ETH_InitStruct->ETH_HashTableLow;

    ETH->MACFCR = (uint32_t)((ETH_InitStruct->ETH_PauseTime << 16) |
                     ETH_InitStruct->ETH_UnicastPauseFrameDetect |
                     ETH_InitStruct->ETH_ReceiveFlowControl |
                     ETH_InitStruct->ETH_TransmitFlowControl);

    ETH->MACVLANTR = (uint32_t)(ETH_InitStruct->ETH_VLANTagComparison |
                               ETH_InitStruct->ETH_VLANTagIdentifier);

    ETH->DMAOMR = (uint32_t)(ETH_InitStruct->ETH_DropTCPIPChecksumErrorFrame |
                    ETH_InitStruct->ETH_TransmitStoreForward |
                    ETH_InitStruct->ETH_ForwardErrorFrames |
                    ETH_InitStruct->ETH_ForwardUndersizedGoodFrames);

    /* Reset the physical layer */
    tmpreg = ETH_ReadPHYRegister(PHYAddress, PHY_BCR);
    tmpreg |= PHY_Reset;
    ETH_WritePHYRegister(PHYAddress, PHY_BCR, tmpreg);
    return ETH_SUCCESS;
}

/*********************************************************************
 * @fn      ETH_Configuration
 *
 * @brief   Ethernet configure.
 *
 * @return  none
 */
void ETH_Configuration( uint8_t *macAddr )
{
    ETH_InitTypeDef ETH_InitStructure;
    uint16_t timeout = 10000;

    gPHYAddress = PHY_ADDRESS;

    /* Set the Ethernet related clock */
    ETH_SetClock();

    /* Enable Ethernet MAC clock */
    RCC->HBPCENR |= 1<<14;

    /* Reset ETHERNET on AHB Bus */
    ETH_DeInit();

    /* Disable PHY Powerdown */
    ETH->PHY_CR &= ~(1<<30);

    /* Disable PHY Rstn */
    ETH->PHY_CR |= 1<<31;

    /* Software reset */
    ETH_SoftwareReset();

    /* Wait for software reset */
    do{
        Delay_Us(10);
        if( !--timeout )  break;
    }while(ETH->DMABMR & ETH_DMABMR_SR);

    /* ETHERNET Configuration */
    /*------------------------   MAC   -----------------------------------*/
    ETH_InitStructure.ETH_Watchdog = ETH_Watchdog_Enable;
    ETH_InitStructure.ETH_Jabber = ETH_Jabber_Enable;
    ETH_InitStructure.ETH_InterFrameGap = ETH_InterFrameGap_96Bit;
    ETH_InitStructure.ETH_ChecksumOffload = ETH_ChecksumOffload_Disable;
    ETH_InitStructure.ETH_AutomaticPadCRCStrip = ETH_AutomaticPadCRCStrip_Disable;
    ETH_InitStructure.ETH_LoopbackMode = ETH_LoopbackMode_Disable;

    /* Filter function configuration */
    ETH_InitStructure.ETH_ReceiveAll = ETH_ReceiveAll_Enable;
    ETH_InitStructure.ETH_PromiscuousMode = ETH_PromiscuousMode_Enable;
    ETH_InitStructure.ETH_BroadcastFramesReception = ETH_BroadcastFramesReception_Enable;
    ETH_InitStructure.ETH_MulticastFramesFilter = ETH_MulticastFramesFilter_Perfect;
    ETH_InitStructure.ETH_UnicastFramesFilter = ETH_UnicastFramesFilter_Perfect;
    ETH_InitStructure.ETH_PassControlFrames = ETH_PassControlFrames_BlockAll;
    ETH_InitStructure.ETH_DestinationAddrFilter = ETH_DestinationAddrFilter_Normal;
    ETH_InitStructure.ETH_SourceAddrFilter = ETH_SourceAddrFilter_Disable;

    ETH_InitStructure.ETH_HashTableHigh = 0x0;
    ETH_InitStructure.ETH_HashTableLow = 0x0;

    /* VLan function configuration */
    ETH_InitStructure.ETH_VLANTagComparison = ETH_VLANTagComparison_16Bit;
    ETH_InitStructure.ETH_VLANTagIdentifier = 0x0;

    /* Flow Control function configuration */
    ETH_InitStructure.ETH_PauseTime = 0x0;
    ETH_InitStructure.ETH_UnicastPauseFrameDetect = ETH_UnicastPauseFrameDetect_Disable;
    ETH_InitStructure.ETH_ReceiveFlowControl = ETH_ReceiveFlowControl_Disable;
    ETH_InitStructure.ETH_TransmitFlowControl = ETH_TransmitFlowControl_Disable;

    /*------------------------   DMA   -----------------------------------*/
    /* When we use the Checksum offload feature, we need to enable the Store and Forward mode:
    the store and forward guarantee that a whole frame is stored in the FIFO, so the MAC can insert/verify the checksum,
    if the checksum is OK the DMA can handle the frame otherwise the frame is dropped */
    ETH_InitStructure.ETH_DropTCPIPChecksumErrorFrame = ETH_DropTCPIPChecksumErrorFrame_Enable;
    ETH_InitStructure.ETH_TransmitStoreForward = ETH_TransmitStoreForward_Enable;
    ETH_InitStructure.ETH_ForwardErrorFrames = ETH_ForwardErrorFrames_Enable;
    ETH_InitStructure.ETH_ForwardUndersizedGoodFrames = ETH_ForwardUndersizedGoodFrames_Enable;
    /* Configure Ethernet */
    ETH_RegInit( &ETH_InitStructure, gPHYAddress );

    /* Configure MAC address */
    ETH->MACA0HR = (uint32_t)((macAddr[5]<<8) | macAddr[4]);
    ETH->MACA0LR = (uint32_t)(macAddr[0] | (macAddr[1]<<8) | (macAddr[2]<<16) | (macAddr[3]<<24));

    /* Mask the interrupt that Tx good frame count counter reaches half the maximum value */
    ETH->MMCTIMR = ETH_MMCTIMR_TGFM;
    /* Mask the interrupt that Rx good unicast frames counter reaches half the maximum value */
    /* Mask the interrupt that Rx crc error counter reaches half the maximum value */
    ETH->MMCRIMR = ETH_MMCRIMR_RGUFM | ETH_MMCRIMR_RFCEM;

    PHY_InterruptInit();

    ETH_DMAITConfig(ETH_DMA_IT_NIS |\
                ETH_DMA_IT_R |\
                ETH_DMA_IT_T |\
                ETH_DMA_IT_AIS |\
                ETH_DMA_IT_RBU |\
                ETH_DMA_IT_PHYSR,\
                ENABLE);
}

/*********************************************************************
 * @fn      MACRAW_Tx
 *
 * @brief   Ethernet sends data frames in chain mode.
 *
 * @param   buff   send buffer pointer
 *          len    Send data length
 *
 * @return  Send status.
 */
uint32_t MACRAW_Tx(uint8_t *buff, uint16_t len)
{
    /* Check if the descriptor is owned by the ETHERNET DMA (when set) or CPU (when reset) */
    if((DMATxDescToSet->Status & ETH_DMATxDesc_OWN) != (u32)RESET)
    {
        /* Return ERROR: OWN bit set */
        return ETH_ERROR;
    }
    /* Setting the Frame Length: bits[12:0] */
    DMATxDescToSet->ControlBufferSize = (len & ETH_DMATxDesc_TBS1);

    memcpy((uint8_t *)DMATxDescToSet->Buffer1Addr, buff, len);

    /* Setting the last segment and first segment bits (in this case a frame is transmitted in one descriptor) */
    DMATxDescToSet->Status |= ETH_DMATxDesc_LS | ETH_DMATxDesc_FS;

    /* Set Own bit of the Tx descriptor Status: gives the buffer back to ETHERNET DMA */
    DMATxDescToSet->Status |= ETH_DMATxDesc_OWN;

    /* Clear TBUS ETHERNET DMA flag */
    ETH->DMASR = ETH_DMASR_TBUS;
    /* Resume DMA transmission*/
    ETH->DMATPDR = 0;

    /* Update the ETHERNET DMA global Tx descriptor with next Tx descriptor */
    /* Chained Mode */
    /* Selects the next DMA Tx descriptor list for next buffer to send */
    DMATxDescToSet = (ETH_DMADESCTypeDef*) (DMATxDescToSet->Buffer2NextDescAddr);
    /* Return SUCCESS */
    return ETH_SUCCESS;
}

/*********************************************************************
 * @fn      WCHNET_ETHIsr
 *
 * @brief   Ethernet Interrupt Service Routine
 *
 * @return  none
 */
void WCHNET_ETHIsr(void)
{
    uint32_t intStat;

    intStat = ETH->DMASR;

    if (intStat & ETH_DMA_FLAG_AIS)
    {
        if (intStat & ETH_DMA_FLAG_RBU)
        {
            ETH_DMAClearITPendingBit(ETH_DMA_FLAG_RBU);
        }
        ETH_DMAClearITPendingBit(ETH_DMA_FLAG_AIS);
    }

    if( intStat & ETH_DMA_FLAG_NIS )
    {
        if( intStat & ETH_DMA_FLAG_R )
        {
            /*If you don't use the Ethernet library,
             * you can do some data processing operations here*/
            ETH_DMAClearITPendingBit(ETH_DMA_FLAG_R);
        }
        if( intStat & ETH_DMA_FLAG_T )
        {
            ETH_DMAClearITPendingBit(ETH_DMA_FLAG_T);
        }
        if( intStat & ETH_DMA_FLAG_PHYSR)
        {
            ETH_PHYLink();
            ETH_DMAClearITPendingBit(ETH_DMA_FLAG_PHYSR);
        }
        ETH_DMAClearITPendingBit(ETH_DMA_FLAG_NIS);
    }
}

/*********************************************************************
 * @fn      ETH_Init
 *
 * @brief   Ethernet initialization.
 *
 * @return  none
 */
void ETH_Init( uint8_t *macAddr )
{
    ETH_Configuration( macAddr );
    ETH_DMATxDescChainInit(DMATxDscrTab, MACTxBuf, ETH_TXBUFNB);
    ETH_DMARxDescChainInit(DMARxDscrTab, MACRxBuf, ETH_RXBUFNB);
    pDMARxSet = DMARxDscrTab;
    pDMATxSet = DMATxDscrTab;
    NVIC_EnableIRQ(ETH_IRQn);
    NVIC_SetPriority(ETH_IRQn, 0);

    R32_ETH_CFG4 = 0X01818818;
    R32_ETH_CFG10 &= ~0xf;
    R32_ETH_ANA1 = 0x6d4e1026;
}

/******************************** endfile @ eth_driver ******************************/
