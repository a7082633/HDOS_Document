#ifndef HDICCARD_H
#define HDICCARD_H
#include <windows.h>


#define JR_OK 0     //ִ�гɹ�

#define IC_UnConnect -1001 //�豸ͨѶ��ʱ
#define IC_CardNoExist -1002 //�޿�
#define IC_CardLock -2001	//��Ƭ�������������
#define IC_CardInit -2002	//��Ƭ��ʼ��ʧ��
#define IC_CardGetCDOL1 -2003	//��Ƭ��ȡCDOL1ʧ��
#define IC_CardARQC  -2004	//��Ƭ��ȡARQCʧ��
#define IC_CardGetCDOL2 -2005	//��ȡ��ƬCDOL2ʧ��
#define IC_CardTC -2006	  //��Ƭ����TCʧ��
#define IC_CardAAC -2007	//��Ƭ����AACʧ��

#define IC_CardIsAuthen -2010	//��Ƭ��������֤ʧ��
#define IC_CardScript -2008	  //��Ƭִ�нű�ʧ��
#define IC_CardList -2009	//��ȡ������ϸʧ��





#ifdef __cplusplus
extern "C" {
#endif

int __stdcall HD_ICC_GetInfo(char *TermType, 
					char *PortType, 
					char *PortNO, 
					char *TransPortNO, 
					char *ICFlag,
					char *Taglist,
					char *AIDList,
					char *DataBuf
);

int __stdcall HD_ICC_GenARQC(char *TermType, 
					char *PortType, 
					char *PortNO,  
					char *TransPortNO, 
					char *ICFlag,
					char *TxData, 
					char *AIDList, 
					char *ARQCLen,
					char *ascARQC,
					char *hexARQC
);

int __stdcall HD_ICC_ARPC_ExeICScript(
					char *TermType, 
					char *PortType, 
					char *PortNO,  
					char *TransPortNO, 
					char *ICFlag,
					char *TxData,
					char *ARPCLen,
					char *hexARPC,
					char *Status,
					char *TC, 
					char *ScriptResult
);

int __stdcall HD_ICC_GetTxDetail(
						char *TermType, 
						char *PortType, 
						char *PortNO,  
						char *TransPortNO, 
						char *ICFlag,
						char *AIDList, 
						char *TxDetail
);






#ifdef __cplusplus
}
#endif

#endif