#ifndef GZCA_H
#define GZCA_H
#include <windows.h>

//状态码
#define IFD_OK	0					 //执行成功
#define IFD_ICC_TypeError	-1		 //卡片类型不对
#define IFD_ICC_NoExist	-2			 //无卡
#define IFD_ICC_NoPower	-3			 //有卡未上电
#define IFD_ICC_NoResponse	-4	     //卡片无应答
#define IFD_SJ_UnConnected	-5	     //加密机连接错误
#define IFD_SJ_Communication	-6	 //加密机通讯错误
#define IFD_ConnectError	-11	     //读卡器连接错
#define IFD_UnConnected	-12	         //未建立连接(没有执行打开设备函数)
#define IFD_BadCommand	-13	         //(动态库)不支持该命令
#define IFD_ParameterError	-14	     //(发给动态库的)命令参数错
#define IFD_CheckSumError	-15	     //信息校验和出错

#ifdef __cplusplus
extern "C" {
#endif

	long WINAPI HD_InitCard(char* IPaddress,int port); 
	long WINAPI HD_MAC(char* initdata,char *key,char *data,int datalen,int keylen,char *Mac); 

#ifdef __cplusplus
}
#endif
#endif