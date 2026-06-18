/****************************ch563TFTP接口函数*****************************/


/***************************************************************************/
/* 头文件包含*/
#include "string.h"
#include "stdio.h"
#include "stdlib.h"
#include "CH563BAS.H"
#include "CH563NET.H"
#include "TFTP.H"

/***************************************************************************/
/* 变量定义 */
char *downfile="111.txt";                                                       //文件名
char *upfile="sendfile"; 
char *trmode="octet";                                                           //文件传输格式
char *option="tsize";                                                           //请求选项，文件大小
char filesize[10];
char *sendfile="123456789ABCDEFGHIJKLMN123456789ABCDEFGHIJKLMN";
               
UINT8  desip[4];
UINT8  ReqBuffer[50];                                                            //用于保存请求包
UINT8  ACKBuffer[20];                                                            //用于保存ACK包
UINT8  DATABuffer[600];
UINT16 datasize ;                                                               //保存从tftp服务器上下载的文件
UINT8  errorcode;                                                               //错误码
UINT8  opcode;
UINT16 block_num;
UINT16 sendport;
UINT32 sendnum=0;
UINT8  RW;                                                                      //读写请求发送完成标志，0-读，1-写
/*************************************************************************
功能：组装客户端请求包

输入：filename—文件名
      trmode—文件传输模式
      option—文件大小选项
      filesize-文件传输长度，读请求下为0，写请求下为实际上传文件长度
      
输出：组装完成后请求包的长度
**************************************************************************/

UINT8 Create_Request_Package(UINT8 opcode,const char *filename )
{
      UINT16 i=0;
      UINT16 pos=0;

      memset(ReqBuffer,0,50);
      ReqBuffer[pos]=0;
      pos++;
      ReqBuffer[pos]=opcode;                                                    //操作码，1为读请求，2为写请求
      pos++;
      for(i=0;i<strlen(filename);i++){                                          //文件名        
          ReqBuffer[pos]=filename[i];
          pos++;
        }
          ReqBuffer[pos]=0;
           pos++;        
      for(i=0;i<strlen(trmode);i++) {                                           //文件传输方式，默认octet。        
          ReqBuffer[pos]=trmode[i];
          pos++;
        }
          ReqBuffer[pos]=0;
           pos++;        
      for(i=0;i<strlen(option);i++){                                            //请求选项：文件大小tsize。        
          ReqBuffer[pos]=option[i];
          pos++;
        }
          ReqBuffer[pos]=0;
          pos++;        
      if(opcode==2) {                                                           //根据读/写请求来设置文件长度，读 ；长度为0；写 ；发送文件实际长度        
          datasize=strlen(sendfile);  
          sprintf(filesize,"%d",datasize);
          RW=1;
        }            
      else{
          RW=0;
          filesize[0]='0';
        }                            
      for(i=0;i<strlen(filesize);i++){                                          //请求选项：文件大小tsize。        
          ReqBuffer[pos]=filesize[i];    
          pos++;
        }
          ReqBuffer[pos]=0;
           pos++;
        
      return pos;                                                               //返回请求报文的长度。
        
}

/*************************************************************************
功能：组装客户端ACK包

输入：block_num:数据包编号
      
输出：组装完成后ACK包的长度
**************************************************************************/
UINT8 Create_Ack_Package(UINT16 block_num)
{
      UINT8 pos=0;
      memset(ACKBuffer,0,20);
      ACKBuffer[pos]=0;
      pos++;
      ACKBuffer[pos]=TFTP_ACK;                                                  //opcode=4;
      pos++;
      ACKBuffer[pos]=(char)(block_num>>8);                                      //块编号
      pos++;
      ACKBuffer[pos]=(char)(block_num);
      pos++;
      
      return pos;
  
}
   
/*************************************************************************
功能：组装客户端data包

输入：block_num:数据包编号
      datasize:一包数据的长度
       
输出：组装完成后data包的长度
**************************************************************************/
UINT16 Create_Data_Package(UINT16 block_num,UINT16 datasize,char *sendbuf)
{
      UINT16 sendlen;                                                           //单包数据长度，最大512
      UINT16 pos=0;
      memset(DATABuffer,0,600);
      DATABuffer[pos]=0;
      pos++;
      DATABuffer[pos]=TFTP_DATA;                                                //opcode=3;
      pos++;
      DATABuffer[pos]=(char)(block_num>>8);                                     //块编号
      pos++;
      DATABuffer[pos]=(char)(block_num);
      pos++;
      if(datasize>=512)
        sendlen=512;
      else 
        sendlen=datasize;
      memcpy(&DATABuffer[pos],sendbuf,sendlen);
      pos+=sendlen;
      
      return pos;
  
}

/*************************************************************************
功能：分析接收到的数据包

输入:  opcode:接收包类型
       block_num:包编号
             
输出：组装完成后data包的长度
**************************************************************************/
void Parse_Recv_Package(UINT8 *buf)
{
  UINT32 len;

  switch(buf[1])                                                                //分析接收包的操作码(类型）
  {
    
    case TFTP_DATA:
           opcode=TFTP_DATA; 
           block_num=buf[3];                                                    //接收到数据包，取出数据包编号          
           printf("blocknum=%-8d  DATA\r\n",block_num);        
           len=Create_Ack_Package(block_num);
           CH563NET_SocketUdpSendTo(0,ACKBuffer,&len,desip,sendport);           //发送相应编号的ack包              
           break;  
    case TFTP_ACK:
           opcode=TFTP_ACK;
           block_num=buf[3];                                                    //接收到ACK包，取出数据包编号
           printf("blocknum=%-8d  ACK\r\n",block_num);          
           if(datasize!=0){
              len=Create_Data_Package(block_num+1, datasize,sendfile);
              datasize-=(len-4);
              CH563NET_SocketUdpSendTo(0,DATABuffer,&len,desip,sendport);       //收到ack包后，发送data包
           }
           else
              printf("\nupload over : len=%s\n",filesize);
           break;    
    case TFTP_ERROR:
           opcode=TFTP_ERROR;
           errorcode=buf[3];                                                    //收到错误包，取出错误码           
           printf("error:%d",errorcode);
           break;    
    case TFTP_OACK:
           opcode=TFTP_OACK;                                                    //收到OACK包          
           block_num=buf[3];
           printf("blocknum=0         OACK\r\n");
           if(RW==0) {                                                          //oack响应的·是读请求        
              len=Create_Ack_Package(0);                        
              CH563NET_SocketUdpSendTo(0,ACKBuffer,&len,desip,sendport);        //收到oack包后，发送编号为0的ACK包
           } 
           if(RW==1) {                                                          //oack响应的是写请求              
              len=Create_Data_Package(1, datasize,sendfile);
              datasize-=(len-4);
              CH563NET_SocketUdpSendTo(0,DATABuffer,&len,desip,sendport);       //收到oack包后，发送编号为0的data包
           }
           break;              
    default:  
           break;
  }      
}  

      
  
    
  
  
  
   
  
  
  
  
  
  
  

