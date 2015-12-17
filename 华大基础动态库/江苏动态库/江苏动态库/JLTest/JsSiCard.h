/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2012,���󼯳�
// All rights reserved.
// 
// �ļ�����: JsSiCard.h
// ģ������: ����ʡͳһ�籣��ר�ú����ӿ�
// ��Ŀ���ƣ� 
// ��������: ��CPU�����ܺ�������Ҫ��Ľӿڽ��з�װ
// ��������: Winxp/VC++6.0
// ��������: ŷ����
// �������ڣ� 2013-6
// QQ:505933474
/////////////////////////////////////////////////////////////////////////////////
#ifndef JSSICARD_H
#define JSSICARD_H
#include <windows.h>

#define SB_ERR_OK	       0       //�ɹ�
#define SB_ERR_UNKNOWN	   1       //��������
#define SB_ERR_CONNECT	   101     //�򿪶�д���˿�ʧ��
#define SB_ERR_UNCONNECT   102     //��д��δ����
#define SB_ERR_COM	       103     //���ڱ�ռ��
#define SB_ERR_CLOSEPORT   104     //�رն�д��ʧ��
#define SB_ERR_CARDMISS	   201     //���������޿�
#define SB_ERR_PSAMCARD	   202     //PSAM���ϵ�ʧ��
#define SB_ERR_CPUCARD	   203     //PSAM���ϵ�ʧ��
#define SB_ERR_CARDTYPE    301     //�û������Ͳ�ƥ��
#define SB_ERR_AUTH	       302	   //����ԿУ�����
#define SB_ERR_AUTHPIN	   303	   //����ԿУ�����
#define SB_ERR_UNSUPPORTED 304	   //��д�����봮��ʽ����
#define SB_ERR_OPNUMOVER   305     //��д������Ĳ�����Խ��
#define SB_ERR_OPNUMERROR  306     //��д������Ĳ������Ƿ�
#define SB_ERR_READINFO    307     //��ȡ������Ϣʧ��
#define SB_ERR_WRITEINFO   308     //��Ϣд�뿨��ʧ��
#define SB_ERR_OTHERERROR  999     //�����쳣����

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
int  WINAPI  iOpenPort(char* pErrMsg);//�򿪶����� ���������û�д򿪣���򿪶����������򲻽����κβ�����1

int  WINAPI  iClosePort(char* pErrMsg);//�رն�����2

int  WINAPI  iCheckType(char* iType, char* CityCode, char* pErrMsg);//���Կ�����3

int WINAPI  iReadCard(const char* pInputPin, const char* pFileAddr, char* pOutDataBuff, char* pErrMsg);//����(��ȡ������Ϣ)4

int WINAPI  iReadCardNo(const char* pInputPin , char* pCardNo , char* pErrMsg);//���籣����5

int WINAPI  iWriteCard(const char* pInputPin, const char* pFileAddr, char* pWriteDataBuff, char* pErrMsg);//д��(��ȡ������Ϣ)6

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
				char     *      pErrMsg);//�˻�����7

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
						  char     *    pErrMsg);//�˻�����8

int  WINAPI  iCardControl (
						   const  char* 	szHosSvrForSiIP,	
						   const char*	szHosSvrForSiPort,
						   const char* pCtrlType, 
						   const char* pCtrlInfo,
						   char     *  pErrMsg);//������9

int  WINAPI  iReadPsamNo(char* szTerminal, char* pErrMsg);//��PSAM���ն˻����10

int  WINAPI  iGetCardATR(char* szCardATR, char* pErrMsg);//��ÿ�ƬATR��λ��Ϣ11

int WINAPI  iReadPersonNo (const char* pInputPin, char* pPersonNo, char* pErrMsg);//������ҽ�Ʊ��ո����˺�12
*/

#ifdef __cplusplus
}
#endif

#endif
