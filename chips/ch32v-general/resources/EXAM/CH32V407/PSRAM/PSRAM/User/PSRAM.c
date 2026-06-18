/********************************** (C) COPYRIGHT  *******************************
* File Name          : ch32v4x7_psram.c
* Author             : WCH
* Version            : V1.0.0
* Date               : 2025/12/01
* Description        : This file provides all the PSRAM firmware functions.
*********************************************************************************
* Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
* Attention: This software (modified or not) and binary are used for 
* microcontroller manufactured by Nanjing Qinheng Microelectronics.
*******************************************************************************/
#include "PSRAM.h"
#include "debug.h"


/*********************************************************************
 * @fn      Wait_Busy_0
 *
 * @brief   PSRAM write register.
 *
 * @param   none
 *          
 * @return  none
*/
void Wait_Busy_0()
{
    while ((PSRAM->ADDR&CMD_BUSY)==1)   //wait busy clear
    {  
    }
}
/*********************************************************************
 * @fn      PSRAMWriteReg
 *
 * @brief   PSRAM write register.
 *
 * @param   Addr -  register address
 *                  CMD_MR_ADDR_0        Mode Register 0
 *                  CMD_MR_ADDR_1        Mode Register 1
 *                  CMD_MR_ADDR_2        Mode Register 2
 *                  CMD_MR_ADDR_4        Mode Register 4
 *                  CMD_MR_ADDR_6        Mode Register 6
 *                  CMD_MR_ADDR_8        Mode Register 8
 *          Data -  data
 * @return  none
*/
void PSRAMWriteReg(uint32_t Addr, uint32_t Data)
{
    Wait_Busy_0();
    PSRAMSetData(Data);
    PSRAM_Set_MR_ADDR(Addr);
    PSRAM_Set_CMD(CMD_MR_WRITE);
    PSRAM_Set_Busy(ENABLE);
    PSRAM_Set_MW(CMD_MR_WRITE);
    Wait_Busy_0();
}

/*********************************************************************
 * @fn      PSRAMReadReg
 *
 * @brief   PSRAM read register.
 *
 * @param   Addr - register address
 *                  CMD_MR_ADDR_0        Mode Register 0
 *                  CMD_MR_ADDR_1        Mode Register 1
 *                  CMD_MR_ADDR_2        Mode Register 2
 *                  CMD_MR_ADDR_4        Mode Register 4
 *                  CMD_MR_ADDR_6        Mode Register 6
 *                  CMD_MR_ADDR_8        Mode Register 8
 *
 * @return  temp - register data
 */
uint32_t PSRAMReadReg(uint32_t Addr)
{
    uint32_t temp=0;
    PSRAM_Set_MR_ADDR(Addr);
    PSRAM_Set_CMD(CMD_MR_READ);
    PSRAM_Set_Busy(ENABLE);
    PSRAM_Set_MW(CMD_MW_Read);
    Wait_Busy_0();
    temp=PSRAM->DATA;
    return temp;
}

/*********************************************************************
 * @fn      PSRAMSetWrLatency
 *
 * @brief   PSRAM set write Latency
 *
 * @param   PSRAM_Fre -  MR4_Write_66M    
                         MR4_Write_109M      
                         MR4_Write_133M      
                         MR4_Write_166M       
                         MR4_Write_200M     
                         MR4_Write_225M       
                         MR4_Write_250M       
                         MR4_Write_300M      
                         MR4_Write_333M       
                         MR4_Write_400M      
                       
 *          Write_Latency - Write Latency.
 *
 * @return  none
 */
void SetWrLatency(uint32_t PSRAM_Fre, uint32_t Write_Latency)
{
    PSRAMSetData(PSRAM_Fre<<5);
    PSRAMSetWrLatency(Write_Latency);
    PSRAM_Set_MR_ADDR(CMD_MR_ADDR_4);
    PSRAM_Set_CMD(CMD_MR_WRITE);
    PSRAM_Set_Busy(ENABLE);
    PSRAM_Set_MW(ENABLE);
    Wait_Busy_0();

    if((PSRAM_Fre==MR4_Write_333M)|(PSRAM_Fre==MR4_Write_400M))
    {
    //High frequency latency enable for 333Mhz/400MHz .
    PSRAMSetData(Hfreq_En<<5);
    PSRAM_Set_MR_ADDR(CMD_MR_ADDR_8);
    PSRAM_Set_CMD(CMD_MR_WRITE);
    PSRAM_Set_Busy(ENABLE);
    PSRAM_Set_MW(ENABLE);
    Wait_Busy_0();
    }
}

/*********************************************************************
 * @fn      PSRAMSetRdLatency
 *
 * @brief   PSRAM set read Latency
 *
 * @param   PSRAM_Fre - MR0_Read_66M       
                        MR0_Read_109M        
                        MR0_Read_133M     
                        MR0_Read_166M       
                        MR0_Read_200M        
                        MR0_Read_225M 
                        MR0_Read_250M       
                        MR0_Read_300M      
                        MR0_Read_333M       
                        MR0_Read_400M        
 *          Write_Latency - read Latency.
 *
 * @return  none
 */
void SetRdLatency(uint32_t PSRAM_Fre, uint32_t Read_Latency,uint32_t Read_Latency_Type)
{
    PSRAMSetData(PSRAM_Fre<<2);
    PSRAMSetRdLatency(Read_Latency);
    PSRAM_Set_MR_ADDR(CMD_MR_ADDR_0);
    PSRAM_Set_CMD(CMD_MR_WRITE);
    PSRAM_Set_Busy(ENABLE);
    PSRAM_Set_MW(ENABLE);
    Wait_Busy_0();
//High frequency latency enable for 333Mhz .
   if((PSRAM_Fre==MR0_Read_333M)|(PSRAM_Fre==MR0_Read_400M))
   {
    PSRAMSetData(Hfreq_En<<5);
    PSRAM_Set_MR_ADDR(CMD_MR_ADDR_8);
    PSRAM_Set_CMD(CMD_MR_WRITE);
    PSRAM_Set_Busy(ENABLE);
    PSRAM_Set_MW(ENABLE);
    Wait_Busy_0();
    }
    if(Read_Latency_Type==Read_Fixed)
    {
    PSRAMSetData(Read_Fixed<<5);
    PSRAMSetRdLatency(Read_Latency);
    PSRAM_Set_MR_ADDR(CMD_MR_ADDR_0);
    PSRAM_Set_CMD(CMD_MR_WRITE);
    PSRAM_Set_Busy(ENABLE);
    PSRAM_Set_MW(CMD_MW_Write);
    Wait_Busy_0();

    }
}

/*********************************************************************
 * @fn      PSRAMDMAWrite
 *
 * @brief   PSRAM write data by DMA 
 *
 * @param   pBuffer - data
 *          WriteAddr - PSRAM address.
 *          dataWidth - PSRAM_MEMORYSIZE_16bit:16bit
 *                      PSRAM_MEMORYSIZE_32bit:32bit
 *          ONEBL  -    DMA_BRST_NUM2: 2 times Burst Length     
                        DMA_BRST_NUM4: 4 times Burst Length       
                        DMA_BRST_NUM6: 6 times Burst Length          
                        DMA_BRST_NUM8: 8 times Burst Length         
                        DMA_BRST_NUM16:16 times Burst Length         
 *          TOWBL_TOUT- DMA_PAUSE_TIM0       pause 0 times
                        DMA_PAUSE_TIM1       pause 1 times
                        DMA_PAUSE_TIM2       pause 2 times
                        DMA_PAUSE_TIM3       pause 3 times
                        DMA_PAUSE_TIM4       pause 4 times
                        DMA_PAUSE_TIM5       pause 5 times
                        DMA_PAUSE_TIM6       pause 6 times
                        DMA_PAUSE_TIM7       pause 7 times
                        DMA_PAUSE_TIM8       pause 8 times
                        DMA_PAUSE_TIM9       pause 9 times
                        DMA_PAUSE_TIM10      pause 10 times
                        DMA_PAUSE_TIM11      pause 11 times
                        DMA_PAUSE_TIM12      pause 12 times
                        DMA_PAUSE_TIM13      pause 13 times
                        DMA_PAUSE_TIM14      pause 14 times
                        DMA_PAUSE_TIM15      pause 15 times
 *
 * @return  none
 */
void PSRAMDMAWrite(uint32_t *pBuffer,uint32_t WriteAddr,uint32_t n,uint32_t dataWidth,uint32_t ONEBL,uint32_t TOWBL_TOUT)
{
    PSRAMDMATypeDef PSRAMDMAStruct={0};
    PSRAMDMAStruct.PSRAM_DMA_DAT_DIR=DMA_DIR_PSRAM;
    PSRAMDMAStruct.PSRAM_DMA_DATA_NUM=n;
    PSRAMDMAStruct.PSRAM_DMA_DMA_ONEBL=ONEBL;
    PSRAMDMAStruct.PSRAM_DMA_MEMORY_SIZE=dataWidth;
    PSRAMDMAStruct.PSRAM_DMA_TOWBL_TOUT=TOWBL_TOUT;
    PSRAMDMASet(pBuffer,WriteAddr,&PSRAMDMAStruct);
    PSRAM_DMA_Cmd(ENABLE);
    while (((PSRAM->ISR)&0x00000002)==0 ){
    }
    PSRAM_ClearITPendingBit(PSRAM_DMATF);      
    PSRAM_DMA_Cmd(DISABLE);
}

/*********************************************************************
 * @fn      PSRAMDMARead
 *
 * @brief   PSRAM write data by DMA 
 *
 * @param   pBuffer - data
 *          WriteAddr - PSRAM address.
 *          dataWidth-  Data bit width
 *          ONEBL  -    DMA_BRST_NUM2: 2 times Burst Length     
                        DMA_BRST_NUM4: 4 times Burst Length       
                        DMA_BRST_NUM6: 6 times Burst Length          
                        DMA_BRST_NUM8: 8 times Burst Length         
                        DMA_BRST_NUM16:16 times Burst Length         
 *          TOWBL_TOUT- DMA_PAUSE_TIM0       pause 0 times
                        DMA_PAUSE_TIM1       pause 1 times
                        DMA_PAUSE_TIM2       pause 2 times
                        DMA_PAUSE_TIM3       pause 3 times
                        DMA_PAUSE_TIM4       pause 4 times
                        DMA_PAUSE_TIM5       pause 5 times
                        DMA_PAUSE_TIM6       pause 6 times
                        DMA_PAUSE_TIM7       pause 7 times
                        DMA_PAUSE_TIM8       pause 8 times
                        DMA_PAUSE_TIM9       pause 9 times
                        DMA_PAUSE_TIM10      pause 10 times
                        DMA_PAUSE_TIM11      pause 11 times
                        DMA_PAUSE_TIM12      pause 12 times
                        DMA_PAUSE_TIM13      pause 13 times
                        DMA_PAUSE_TIM14      pause 14 times
                        DMA_PAUSE_TIM15      pause 15 times
 *
 * @return  none
 */
void PSRAMDMARead(uint32_t *pBuffer,uint32_t ReadAddr,uint32_t n,uint32_t dataWidth,uint32_t ONEBL,uint32_t TOWBL_TOUT)
{
    PSRAMDMATypeDef PSRAMDMAStruct={0};
    PSRAMDMAStruct.PSRAM_DMA_DAT_DIR=DMA_DIR_MEM;
    PSRAMDMAStruct.PSRAM_DMA_DATA_NUM=n;
    PSRAMDMAStruct.PSRAM_DMA_DMA_ONEBL=ONEBL;
    PSRAMDMAStruct.PSRAM_DMA_MEMORY_SIZE=dataWidth;
    PSRAMDMAStruct.PSRAM_DMA_TOWBL_TOUT=TOWBL_TOUT;
    PSRAMDMASet(pBuffer,ReadAddr,&PSRAMDMAStruct);
    PSRAM_DMA_Cmd(ENABLE);
    while (((PSRAM->ISR)&0x00000002)==0 ){};
    PSRAM_ClearITPendingBit(PSRAM_DMATF);      
    PSRAM_DMA_Cmd(DISABLE);
}

/*********************************************************************
 * @fn      RDReg_Cmd_Set
 *
 * @brief   Set Read write command.
 *
 * @param    CMD-command

 * @return  none
 */
void Global_RST_Set()
{
    PSRAM_Set_CMD(CMD_MR_RESET);
    PSRAM_Set_Busy(ENABLE);
    Delay_Us(20);                                  //RESET atleast 2US
}