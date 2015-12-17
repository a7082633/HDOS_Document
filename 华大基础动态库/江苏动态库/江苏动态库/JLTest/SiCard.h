/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013,华大集成
// All rights reserved.
// 
// 文件名称: SiCard.h
// 模块名称: 江苏省统一社保卡专用函数接口
// 项目名称： 
// 功能描述: 将CPU卡功能函数，按要求的接口进行封装
// 开发环境: Winxp/VC++6.0
// 作者姓名: 欧阳俊 
// 开发日期： 2013-9
// QQ:505933474  250796165
/////////////////////////////////////////////////////////////////////////////////
#ifndef SICARD_H
#define SICARD_H
#include <windows.h>

#define SB_ERR_OK	       0       //成功
#define SB_ERR_UNKNOWN	   1       //其他错误
#define SB_ERR_CONNECT	   101     //打开读写器端口失败
#define SB_ERR_UNCONNECT   102     //读写器未连接
#define SB_ERR_COM	       103     //串口被占用
#define SB_ERR_CLOSEPORT   104     //关闭读写器失败
#define SB_ERR_CARDMISS	   105     //读卡器内无卡
#define SB_ERR_AUTH	       106	   //卡密钥校验错误
#define SB_ERR_VERIFYKEY   107     //卡密码校验失败
#define SB_ERR_UNSUPPORTED 108	   //读写卡输入串格式错误
#define SB_ERR_OPNUMOVER   109     //读写卡输入的操作数越界
#define SB_ERR_OPNUMERROR  110     //读写卡输入的操作数非法
#define SB_ERR_READINFO    111     //读取卡上信息失败
#define SB_ERR_WRITEINFO   112     //信息写入卡上失败
#define SB_ERR_CLOSE       113     //关闭端口失败
#define SB_ERR_OTHERERROR  114     //其他异常错误

#define SB_ERR_CARDTYPE    301     //用户卡类型不匹配
#define SB_ERR_AUTHPIN	   303	   //卡密钥校验错误
#define SB_ERR_PSAMCARD	   202     //PSAM卡上电失败
#define SB_ERR_CPUCARD	   203     //PSAM卡上电失败

#ifdef __cplusplus
extern "C" {
#endif
//*****4428
int WINAPI ic_init(int port, long baud);   //打开端口函数  port=1，baud=9600

int WINAPI ic_exit(int icdev);			   //关闭端口函数

int WINAPI srd_4428(int icdev, int offset, int len, unsigned char* r_string );	//读4428卡函数

int WINAPI swr_4428(int icdev, int offset, int len, unsigned char* w_string );	//写4428卡函数

int	WINAPI get_status(int icdev,int *state);	//测试4428卡是否插在读卡器上，state=0 无卡 state=1 有卡

int WINAPI csc_4428(int icdev, int len, unsigned char* p_string); //校验4428卡的密码

int WINAPI wsc_4428(int icdev, int len,unsigned char* p_string);  //修改4428卡的密码

int WINAPI chk_card(int icdev);  //判别卡的类型

void WINAPI getErrorMsg(int icdev,char* ErrorMsg);  //获取错误信息


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//******CPU
int  WINAPI  iOpenPort(char* pErrMsg);//打开读卡器 如果读卡器没有打开，则打开读卡器，否则不进行任何操作。1

int	WINAPI iClosePort(char* pErrMsg);

int WINAPI iReadCard(const char* pInputPin, const char* pFileAddr, char* pOutDataBuff);

int WINAPI iReadCardNo(const char* pInputPin, char* pCardNo, char* pErrMsg);

int WINAPI iWriteCard(const char* pInputPin, const char* pFileAddr, char* pWriteDataBuff,char* pErrMsg);

int WINAPI iAcctLoad (
					  const  char* 	pInputPin ,
					  const  char*	pHosSvrForSiIP, 
					  const  char*	pHosSvrForSiPort,
					  const  char*	pTradeType,
					  const  unsigned  char* 	puTradeAmnt,
					  const  char*	pTradeTime,
					  const  char*	pCardNO,
					  char	*		pTerminalID,
					  char	*		pCardTradeNo, 
					  unsigned  char*	puCardAmntTradePre, 
					  char	*  		pTradeTac, 
					  char     *      pErrMsg);//账户划入   “是空函数”

int WINAPI iAcctConsume (  
						 const  char* 	pInputPin ,
						 const  char* 	pTransType,
						 const unsigned char* puConsumAmt, 
						 const  char* 	pTransTime,
						 const  char* 	pCardNo,
						 char* 		    pTerminalID,
						 char*  		pTransNo, 
						 unsigned char * puPreTransAmt,
						 char*  		pTradeTac,
						  char     *    pErrMsg);//账户消费  “是空函数”


int WINAPI iCardControl (const char* szHosSvrForSiIP,const char* szHosSvrForSiPort,const char * pCtrlType, const char * pCtrlInfo,char*  pErrMsg);

int WINAPI iReadCardFlag(char *nCardFlag , char* pErrMsg);

int WINAPI iGetCardStatus(char* pCardCardStatus, char* pErrMsg);

int WINAPI  iGetDllDependentCity(char* pCityName);


#ifdef __cplusplus
}
#endif

#endif
