/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012,华大集成
// All rights reserved.
// 
// 文件名称: JsSiCard.h
// 模块名称: 江苏省统一社保卡专用函数接口
// 项目名称： 
// 功能描述: 将CPU卡功能函数，按要求的接口进行封装
// 开发环境: Winxp/VC++6.0
// 作者姓名: 欧阳俊
// 开发日期： 2013-6
// QQ:505933474
/////////////////////////////////////////////////////////////////////////////////
#ifndef JSSICARD_H
#define JSSICARD_H
#include <windows.h>

#define SB_ERR_OK	       0       //成功
#define SB_ERR_UNKNOWN	   1       //其他错误
#define SB_ERR_CONNECT	   101     //打开读写器端口失败
#define SB_ERR_UNCONNECT   102     //读写器未连接
#define SB_ERR_COM	       103     //串口被占用
#define SB_ERR_CLOSEPORT   104     //关闭读写器失败
#define SB_ERR_CARDMISS	   201     //读卡器内无卡
#define SB_ERR_PSAMCARD	   202     //PSAM卡上电失败
#define SB_ERR_CPUCARD	   203     //PSAM卡上电失败
#define SB_ERR_CARDTYPE    301     //用户卡类型不匹配
#define SB_ERR_AUTH	       302	   //卡密钥校验错误
#define SB_ERR_AUTHPIN	   303	   //卡密钥校验错误
#define SB_ERR_UNSUPPORTED 304	   //读写卡输入串格式错误
#define SB_ERR_OPNUMOVER   305     //读写卡输入的操作数越界
#define SB_ERR_OPNUMERROR  306     //读写卡输入的操作数非法
#define SB_ERR_READINFO    307     //读取卡上信息失败
#define SB_ERR_WRITEINFO   308     //信息写入卡上失败
#define SB_ERR_OTHERERROR  999     //其他异常错误

#ifdef __cplusplus
extern "C" {
#endif

int WINAPI iOpenPort(int pReaderPort, int *pReaderHandle, char * pErrMsg);

int	WINAPI iClosePort(int pReaderHandle ,char* pErrMsg);

int WINAPI iReadCardFlag(int pReaderHandle,char *nCardFlag , char* pErrMsg);

int WINAPI iReadCardFlagX(char *nCardFlag , char* pErrMsg);

int WINAPI iReadCard(int pReaderHandle,const char* pInputPin, const char* pFileAddr, char* pOutDataBuff , char*pErrMsg);

int WINAPI iReadCardX(const char* pInputPin, const char* pFileAddr, char* pOutDataBuff , char*pErrMsg);

int WINAPI iWriteCard(int pReaderHandle,const char* pInputPin, const char* pFileAddr, char* pWriteDataBuff,char* pErrMsg);

int WINAPI iWriteCardX(const char* pInputPin, const char* pFileAddr, char* pWriteDataBuff,char* pErrMsg);

int WINAPI iCardControl (int pReaderHandle,const char * pCtrlType, const char * pCtrlInfo,char*  pErrMsg);

int WINAPI iCardControlX (const char * pCtrlType, const char * pCtrlInfo,char*  pErrMsg);

int WINAPI iCardReadAtr (int pReaderHandle, char* pCardAtr, char* pErrMsg);

int WINAPI iCardReadAtrX (char* pCardAtr, char* pErrMsg);

int WINAPI iReadCardNo(int pReaderHandle, const char* pInputPin,char* pCardNo, char* pErrMsg);

int WINAPI iReadCardNoX( const char* pInputPin,char* pCardNo, char* pErrMsg);

int WINAPI iReadPin (int pReaderHandle,const char * pCtrlType,char *pInputPin, char* pErrMsg);

int WINAPI iReadPinX (const char * pCtrlType,char *pInputPin, char* pErrMsg);

int WINAPI iReadVer (char *pVerInfo, char* pErrMsg);

int WINAPI iReadCardEX(const char* pInputPin, const char* pFileAddr, char* pOutDataBuff,char* pErrMsg);

int WINAPI iWriteCardEX(const char* pInputPin, const char* pFileAddr, char* pWriteDataBuff,char* pErrMsg);

int WINAPI iReadIDMsg(int pReaderPort, const char* pBmpFile, char *pName, char *pSex, char *pNation, char *pBirth, char *pAddress, char *pCertNo, char *pDepartment , char *pExpire,char * pErrMsg);


/*
int  WINAPI  iOpenPort(char* pErrMsg);//打开读卡器 如果读卡器没有打开，则打开读卡器，否则不进行任何操作。1

int  WINAPI  iClosePort(char* pErrMsg);//关闭读卡器2

int  WINAPI  iCheckType(char* iType, char* CityCode, char* pErrMsg);//测试卡类型3

int WINAPI  iReadCard(const char* pInputPin, const char* pFileAddr, char* pOutDataBuff, char* pErrMsg);//读卡(获取卡上信息)4

int WINAPI  iReadCardNo(const char* pInputPin , char* pCardNo , char* pErrMsg);//读社保卡号5

int WINAPI  iWriteCard(const char* pInputPin, const char* pFileAddr, char* pWriteDataBuff, char* pErrMsg);//写卡(获取卡上信息)6

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
				char     *      pErrMsg);//账户划入7

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
						  char     *    pErrMsg);//账户消费8

int  WINAPI  iCardControl (
						   const  char* 	szHosSvrForSiIP,	
						   const char*	szHosSvrForSiPort,
						   const char* pCtrlType, 
						   const char* pCtrlInfo,
						   char     *  pErrMsg);//卡控制9

int  WINAPI  iReadPsamNo(char* szTerminal, char* pErrMsg);//读PSAM卡终端机编号10

int  WINAPI  iGetCardATR(char* szCardATR, char* pErrMsg);//获得卡片ATR复位信息11

int WINAPI  iReadPersonNo (const char* pInputPin, char* pPersonNo, char* pErrMsg);//读基本医疗保险个人账号12
*/

#ifdef __cplusplus
}
#endif

#endif
