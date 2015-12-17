/////////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2013,���󼯳�
// All rights reserved.
// 
// �ļ�����: SiCard.h
// ģ������: ����ʡͳһ�籣��ר�ú����ӿ�
// ��Ŀ���ƣ� 
// ��������: ��CPU�����ܺ�������Ҫ��Ľӿڽ��з�װ
// ��������: Winxp/VC++6.0
// ��������: ŷ���� 
// �������ڣ� 2013-9
// QQ:505933474  250796165
/////////////////////////////////////////////////////////////////////////////////
#ifndef SICARD_H
#define SICARD_H
#include <windows.h>

#define SB_ERR_OK	       0       //�ɹ�
#define SB_ERR_UNKNOWN	   1       //��������
#define SB_ERR_CONNECT	   101     //�򿪶�д���˿�ʧ��
#define SB_ERR_UNCONNECT   102     //��д��δ����
#define SB_ERR_COM	       103     //���ڱ�ռ��
#define SB_ERR_CLOSEPORT   104     //�رն�д��ʧ��
#define SB_ERR_CARDMISS	   105     //���������޿�
#define SB_ERR_AUTH	       106	   //����ԿУ�����
#define SB_ERR_VERIFYKEY   107     //������У��ʧ��
#define SB_ERR_UNSUPPORTED 108	   //��д�����봮��ʽ����
#define SB_ERR_OPNUMOVER   109     //��д������Ĳ�����Խ��
#define SB_ERR_OPNUMERROR  110     //��д������Ĳ������Ƿ�
#define SB_ERR_READINFO    111     //��ȡ������Ϣʧ��
#define SB_ERR_WRITEINFO   112     //��Ϣд�뿨��ʧ��
#define SB_ERR_CLOSE       113     //�رն˿�ʧ��
#define SB_ERR_OTHERERROR  114     //�����쳣����

#define SB_ERR_CARDTYPE    301     //�û������Ͳ�ƥ��
#define SB_ERR_AUTHPIN	   303	   //����ԿУ�����
#define SB_ERR_PSAMCARD	   202     //PSAM���ϵ�ʧ��
#define SB_ERR_CPUCARD	   203     //PSAM���ϵ�ʧ��

#ifdef __cplusplus
extern "C" {
#endif
//*****4428
int WINAPI ic_init(int port, long baud);   //�򿪶˿ں���  port=1��baud=9600

int WINAPI ic_exit(int icdev);			   //�رն˿ں���

int WINAPI srd_4428(int icdev, int offset, int len, unsigned char* r_string );	//��4428������

int WINAPI swr_4428(int icdev, int offset, int len, unsigned char* w_string );	//д4428������

int	WINAPI get_status(int icdev,int *state);	//����4428���Ƿ���ڶ������ϣ�state=0 �޿� state=1 �п�

int WINAPI csc_4428(int icdev, int len, unsigned char* p_string); //У��4428��������

int WINAPI wsc_4428(int icdev, int len,unsigned char* p_string);  //�޸�4428��������

int WINAPI chk_card(int icdev);  //�б𿨵�����

void WINAPI getErrorMsg(int icdev,char* ErrorMsg);  //��ȡ������Ϣ


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//******CPU
int  WINAPI  iOpenPort(char* pErrMsg);//�򿪶����� ���������û�д򿪣���򿪶����������򲻽����κβ�����1

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
					  char     *      pErrMsg);//�˻�����   ���ǿպ�����

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
						  char     *    pErrMsg);//�˻�����  ���ǿպ�����


int WINAPI iCardControl (const char* szHosSvrForSiIP,const char* szHosSvrForSiPort,const char * pCtrlType, const char * pCtrlInfo,char*  pErrMsg);

int WINAPI iReadCardFlag(char *nCardFlag , char* pErrMsg);

int WINAPI iGetCardStatus(char* pCardCardStatus, char* pErrMsg);

int WINAPI  iGetDllDependentCity(char* pCityName);


#ifdef __cplusplus
}
#endif

#endif
