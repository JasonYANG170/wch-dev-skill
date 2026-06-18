/********************************** (C) COPYRIGHT *********************************
* File Name          : HTTPS.H
* Author             : WCH
* Version            : V1.0
* Date               : 2013/11/15
* Description        : CH563NET库-HTTPS
*                    : MDK3.36@ARM966E-S,Thumb
**********************************************************************************/
#ifndef	__HTTPS_H__
#define	__HTTPS_H__

#define Basic_Cfg_Address  0X1000
#define Port_Cfg_Address  0x2000
#define Login_Cfg_Address  0x3000 




#define HTTP_SERVER_PORT		80		

/* http请求方法 */
#define		METHOD_ERR		0		
#define		METHOD_GET		1		
#define		METHOD_HEAD		2		
#define		METHOD_POST		3		

/* http请求类型 */
#define		PTYPE_ERR		  0		
#define		PTYPE_HTML	  1		
#define		PTYPE_PNG		  2				
#define		PTYPE_CSS		  3	
#define   PTYPE_GIF     4

/*563网页通信协议定义*/
#define MODE_TcpServer  0
#define MODE_TcpClient  1




/* HTML Doc. for ERROR */
     
#define RES_HTMLHEAD_OK	"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n"      /*HTML类型的响应报文，最后两个\r\n必须要有*/

#define RES_PNGHEAD_OK	"HTTP/1.1 200 OK\r\nContent-Type: image/png\r\n\r\n"      /*PNG类型的响应报文，最后两个\r\n必须要有*/
  
#define RES_CSSHEAD_OK  "HTTP/1.1 200 OK\r\nContent-Type: text/css\r\n\r\n"       /*CSS类型的响应报文，最后两个\r\n必须要有*/

#define RES_GIFHEAD_OK  "HTTP/1.1 200 OK\r\nContent-Type: image/gif\r\n\r\n"      /*GIF类型的响应报文，最后两个\r\n必须要有*/
 /*-----------------*/


#define MAX_URI_SIZE	128	




#define Basic_Cfg_Len  40
#define Port_Cfg_Len   40    
#define Login_Cfg_Len  40

typedef struct Basic_Cfg        //基础设置配置参数
{
	UINT8 flag[2];                //配置信息验证码0x57,0xab
	UINT8 mac[6];
  UINT8 ip[4];
	UINT8 mask[4];
	UINT8 gateway[4];
} *Basic_Cfg;


typedef struct Port_Cfg        //端口设置配置参数
{
	UINT8 flag[2];               //配置信息验证码0x57,0xab
	UINT8 mode;
  UINT8 src_port[2];
	UINT8 des_ip[4];
	UINT8 des_port[2];
} *Port_Cfg;


typedef struct Login_Cfg        //登录设置配置参数
{
	UINT8  flag[2];               //配置信息验证码0x57,0xab
	UINT8  user[10];
	UINT8  pass[10];
} *Login_Cfg;


typedef struct _st_http_request  //浏览器请求信息
{
	char	METHOD;					
	char	TYPE;					
	char	URI[MAX_URI_SIZE];			
} st_http_request;

typedef struct Para_Tab          //配置信息参数表
{
	char *para;                    //配置项名称
	char value[20];                //配置项数值
}Parameter;
	
extern UINT8  basicbuf[Basic_Cfg_Len],portbuf[Port_Cfg_Len],loginbuf[Login_Cfg_Len];

extern st_http_request *http_request;
  
extern UINT8 httpweb[200] ;                                                               /*用来保存http响应报文*/

extern UINT8 Basic_Default[Basic_Cfg_Len];
extern UINT8 Login_Default[Login_Cfg_Len];
extern UINT8 Port_Default[Port_Cfg_Len];

extern UINT8 RecvBuffer[]; 

 
extern  UINT8 flag;                                    

extern  UINT8 socket;


extern Basic_Cfg Basic_CfgBuf;
extern Login_Cfg Login_CfgBuf;
extern Port_Cfg  Port_CfgBuf;

extern void ParseHttpRequest(st_http_request *, char *);	

extern void ParseUriType(char *, char *);			

extern void MakeHttpResponse(unsigned char *, char);			

extern  char* GetUriName(char* uri);

extern PINT8 DataLocate(PINT8 buf,PINT8 name);

extern void Copy_Flash(const char *);

extern void Init_Para_Tab(void) ;

extern void CH563_RESET(void);

extern void Web_Server(void);

#endif	
