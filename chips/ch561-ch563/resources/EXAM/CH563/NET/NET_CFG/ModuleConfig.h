
/********************************** (C) COPYRIGHT *********************************
* File Name          : Moduleconfig.H
* Description        :上位机通信数据结构定义
*                      
**********************************************************************************/

#include "CH563SFR.H"

#ifndef __MODULECONFIG_H__
#define __MODULECONFIG_H__

#include "CH563SFR.H"
#define NET_MODULE_DATA_LENGTH   60               //与模块通信时数据区的最大长度      
                                                                                      
#define MODULE_CFG_LEN           44                                                  
                                                                                      
//通信命令码                                                                          
#define NET_MODULE_CMD_SET      0X01              //配置网络中的模块                  
#define NET_MODULE_CMD_GET      0X02              //获取某个模块的配置                
#define NET_MODULE_CMD_RESET    0X03              //获取某个模块的配置                
#define NET_MODULE_CMD_SEARCH   0X04              //搜索网络中的模块                  
                                                                                      
//应答命令码                                                                          
#define NET_MODULE_ACK_SET      0X81              //回应配置命令码                    
#define NET_MODULE_ACK_GET      0X82              //回应获取命令码                    
#define NET_MODULE_ACK_RESET    0X83              //获取某个模块的配置                
#define NET_MODULE_ACK_SEARCH   0X84              //回应所搜命令码                    
                                                                                      
#define NET_MODULE_FLAG        "CH395NET_MODULE"  //用来标识上位机通信                   
#define checkcode1              0X11              //用来标识配置信息
#define checkcode2              0x22

//网络通信结构体
typedef struct NET_COMM {
  unsigned char flag[16];                         //通信标识，因为都是用广播方式进行通信的，所以这里加一个固定值
  unsigned char cmd;                              //命令头
  unsigned char id[6];                            //标识，CH563MAC地址
  unsigned char len;                              //数据区长度
  unsigned char dat[NET_MODULE_DATA_LENGTH];      //数据区缓冲区
}net_comm,*pnet_comm;

//模块标识
#define  NET_MODULE_TYPE_TCP_S    0X00            //模块作为TCP SERVER                    
#define  NET_MODULE_TYPE_TCP_C    0X01            //模块作为TCP CLIENT                    
#define  NET_MODULE_TYPE_UDP_S    0X02            //模块作为UDP SERVER                    
#define  NET_MODULE_TYPE_UDP_C    0X03            //模块作为UDP CLIENT                    
#define  NET_MODULE_TYPE_NONE     0X04            //模块的默认配置，不开启任何模式       



//模块的配置结构
typedef struct MODULE_CFG {
  unsigned char module_name[21];                  //模块本身的IP地址                                  
  unsigned char type;                             //标识模块处于那模式(TCP/UDP server/client)         
  unsigned char src_ip[4];                        //模块本身的IP地址                                  
  unsigned char mask[4];                          //模块本身的子网掩码                                
  unsigned char getway[4];                        //模块对应的网关地址                                
  unsigned char src_port[2];                      //模块源端口                                        
  unsigned char dest_ip[4];                       //目的IP地址                                        
  unsigned char dest_port[2];                     //目的端口 
  unsigned char cfg_flag[2];                      //用于读取eeprom时验证配置信息  
}module_cfg,*pmodule_cfg;


extern UINT8 Default_cfg[MODULE_CFG_LEN];
extern pmodule_cfg CFG;
extern UINT8   ParseConfigbuf(UINT8 *buf);
#endif
