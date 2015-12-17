#ifndef MHC_CARDINTERFACE_H
#define MHC_CARDINTERFACE_H

/*
#ifdef DLL_FILE
#define MHC_CARDINTERFACE_API  extern "C" __declspec(dllexport)
#else
#define MHC_CARDINTERFACE_API  extern "C" __declspec(dllimport)
#endif 
*/
#define MHC_CARDINTERFACE_API  extern "C" __declspec(dllexport)



#define RD_OK			0
#define RD_CONNECT_ERROR	1
#define RD_NO_ICC		2			//�޿�
#define RD_IC_NO_RESP		4		//��Ƭ��Ӧ��
#define RD_ERROR		5			//�ӿ��豸����
#define IRV_OK			0			//����
#define IRV_PARA_ERR	101			//���������
#define IRV_NO_FILE		102			//�ļ�������
#define IRV_RD_RCDERR	103			//����¼ʧ��
#define IRV_CARD_ERR	104			//��Ƭ��Ӧ�쳣
#define IRV_PSAM_ERR	105			//PSAM��Ƭ��Ӧ�쳣
#define IRV_IRK_VERR	106			//�ڲ���֤ʧ��
#define IRV_RD_BINERR	107			//��BINʧ��
#define IRV_WR_RCDERR	108			//д��¼ʧ��
#define IRV_WR_BINERR	109			//дBINʧ��
#define IRV_W_DF03EF05ERR	110			//дDF03EF05ʧ��
#define IRV_ER_DF03EF05ERR	111			//����DF03EF05ʧ��
#define IRV_RCINVALID	112			//��¼������(ef ��Ч���Ϊ ff)
#define IRV_DDF1EF07_DATAERR	113			//��Ƭ�ļ������쳣

#define NO_style			0x00

#define AN_style			0x01
#define CN_style			0x0F
#define B_style				0xFF

#define SLOT_USER			0x01
#define SLOT_PSAM			0x11
#define SLOT_CARD			0x12	

//#include "Application.h"
/*
typedef struct st_itemselect_
{
	WORD	wItemFlag;				  // ���������
	WORD	wID;					  // ��¼�ļ�ID
	VOID	*pValue;				  // ���ݵ�ַ 
	WORD	wValueLen;				  // ���ݳ���
	DWORD	*pdwErrCode;	
}ITEMSELECT, *P_ITEMSELECT;	
*/
/***********************************************************************/
/* �����ӿ�����                                                        */
/************************************************************************/

//�����豸
MHC_CARDINTERFACE_API int __stdcall iDOpenPort(void);

//�Ͽ��豸
MHC_CARDINTERFACE_API int __stdcall iDClosePort(void);

//�ϵ縴λ
// slot = SLOT_USER(0x01) �û����ϵ�
// slot = SLOT_PSAM(0x11) SAM���ϵ�
// ATR��Ҫ����40+1���ϵ��ַ��ռ�
MHC_CARDINTERFACE_API int __stdcall PowerOn(HANDLE hDev,int slot, char * ATR) ; //slot 1  User; slot 0x11 SAM1

MHC_CARDINTERFACE_API int __stdcall iVerifyPIN(HANDLE hDev,char * PIN) ;//У��PSAM��PIN

MHC_CARDINTERFACE_API int __stdcall iChange_Pin(int hDev,
												  char *oldPin, 
												  char *newPin);  

MHC_CARDINTERFACE_API int __stdcall iReader_SAM_Public(int hDev,
														 char* info);//�ú���ȡ��SAM��Ƭ������Ϣ�ļ�

MHC_CARDINTERFACE_API int __stdcall iReader_SAM_Terminal(int hDev,
														   char* info);//�ú���ȡ��SAM��Ƭ���ն���Ϣ�ļ�

MHC_CARDINTERFACE_API int __stdcall  iReader_SAM_PublicApp(int hDev,
															 char* info);//�ú���ȡ��SAM��Ƭ�ľ��񽡿���Ӧ�õĹ�����Ϣ�ļ�

MHC_CARDINTERFACE_API int __stdcall  iReader_SAM_OrgCertificate(int hDev,
															 char* info);//�ú���ȡ��SAM��Ƭ�ľ��񽡿�����������֤���ļ�

MHC_CARDINTERFACE_API int __stdcall  iReader_SAM_Certificate(int hDev,
															 char* info);//�ú���ȡ��SAM��Ƭ��SAM������֤��

//�����������������ļ�
MHC_CARDINTERFACE_API int __stdcall iR_DDF1EF05Info(HANDLE hDev,
							  char * KLB,             //��������                                                                                        
							  char * GFBB,            //�淶�汾                                                                                          
							  char * FKJGMC,          //������������                                                                                      
							  char * FKJGDM,          //������������ 
							  char * FKJGZS,			//��������֤��
							  char * FKSJ,            //����ʱ�䣬YYYYMMDD��ʽ                                                                            
							  char * KH,              //���ţ���������֤�Ż���֤������һ��
							  char * AQM,             //��ȫ��                                                                                            
							  char * XPXLH,           //оƬ���к�
							  char * YYCSDM				//Ӧ�ó��д���
							  );

//�����������������ļ�
MHC_CARDINTERFACE_API int __stdcall iW_DDF1EF05Info(HANDLE hDev,
							  char * KLB,             //��������                                                                                        
							  char * GFBB,            //�淶�汾                                                                                          
							  char * FKJGMC,          //������������                                                                                      
							  char * FKJGDM,          //������������ 
							  char * FKJGZS,			//��������֤��
							  char * FKSJ,            //����ʱ�䣬YYYYMMDD��ʽ                                                                            
							  char * KH,              //���ţ���������֤�Ż���֤������һ��
							  char * AQM,             //��ȫ��                                                                                            
							  char * XPXLH,           //оƬ���к�
							  char * YYCSDM				//Ӧ�ó��д���
							  );

//�ֿ��˻�����Ϣ�����ļ�
MHC_CARDINTERFACE_API int __stdcall iR_DDF1EF06Info(HANDLE hDev,
							  char * XM,              //����
							  char * XB,              //�Ա����
							  char * MZ,              //�������
							  char * CSRQ,            //�������ڣ�YYYYMMDD��ʽ
							  char * SFZH            //�������֤��
							  );

MHC_CARDINTERFACE_API int __stdcall iW_DDF1EF06Info(HANDLE hDev,
							  char * XM,              //����
							  char * XB,              //�Ա����
							  char * MZ,              //�������
							  char * CSRQ,            //�������ڣ�YYYYMMDD��ʽ
							  char * SFZH            //�������֤��
							  );
//��Ƭ
MHC_CARDINTERFACE_API int __stdcall iR_DDF1EF07Info(HANDLE hDev,
							  BYTE * ZHAOPIAN        //��Ƭ												  												  
							  );

MHC_CARDINTERFACE_API int __stdcall iW_DDF1EF07Info(HANDLE hDev,
							  BYTE * ZHAOPIAN        //��Ƭ												  												  
							  );

//EF08
MHC_CARDINTERFACE_API int __stdcall iR_DDF1EF08Info(HANDLE hDev,
							  char * KYXQ,			//����Ч��
							  char * BRDH1,           //���˵绰1
							  char * BRDH2,           //���˵绰2
							  char * YLFYZFFS1,		  //ҽ�Ʒ���֧����ʽ1
							  char * YLFYZFFS2,		  //ҽ�Ʒ���֧����ʽ2
							  char * YLFYZFFS3		  //ҽ�Ʒ���֧����ʽ3		
							  );

//EF08
MHC_CARDINTERFACE_API int __stdcall iW_DDF1EF08Info(HANDLE hDev,
							  char * KYXQ,			//����Ч��
							  char * BRDH1,           //���˵绰1
							  char * BRDH2,           //���˵绰2
							  char * YLFYZFFS1,		  //ҽ�Ʒ���֧����ʽ1
							  char * YLFYZFFS2,		  //ҽ�Ʒ���֧����ʽ2
							  char * YLFYZFFS3		  //ҽ�Ʒ���֧����ʽ3		
							  );


//��ַ��Ϣ�ļ�
MHC_CARDINTERFACE_API int __stdcall iR_DF01EF05Info(HANDLE hDev,
							  char * DZLB1,             //��ַ���1
							  char * DZ1,				//��ַ1
							  char * DZLB2,             //��ַ���2
							  char * DZ2				//��ַ2
							  );

MHC_CARDINTERFACE_API int __stdcall iW_DF01EF05Info(HANDLE hDev,
							  char * DZLB1,             //��ַ���1
							  char * DZ1,				//��ַ1
							  char * DZLB2,             //��ַ���2
							  char * DZ2				//��ַ2
							  );
//��ϵ����Ϣ�ļ�
MHC_CARDINTERFACE_API int __stdcall iR_DF01EF06Info(HANDLE hDev,
							  char * LXRXM1,          //��ϵ������1                                                                                       
							  char * LXRGX1,          //��ϵ�˹�ϵ����1                                                                                   
							  char * LXRDH1,          //��ϵ�˵绰1                                                                                       
							  char * LXRXM2,          //��ϵ������2                                                                                       
							  char * LXRGX2,          //��ϵ�˹�ϵ����2                                                                                   
							  char * LXRDH2,          //��ϵ�˵绰2                                                                                       
							  char * LXRXM3,          //��ϵ������3                                                                                       
							  char * LXRGX3,          //��ϵ�˹�ϵ����3                                                                                   
							  char * LXRDH3			  //��ϵ�˵绰3
							  );

MHC_CARDINTERFACE_API int __stdcall iW_DF01EF06Info(HANDLE hDev,
							  char * LXRXM1,          //��ϵ������1                                                                                       
							  char * LXRGX1,          //��ϵ�˹�ϵ����1                                                                                   
							  char * LXRDH1,          //��ϵ�˵绰1                                                                                       
							  char * LXRXM2,          //��ϵ������2                                                                                       
							  char * LXRGX2,          //��ϵ�˹�ϵ����2                                                                                   
							  char * LXRDH2,          //��ϵ�˵绰2                                                                                       
							  char * LXRXM3,          //��ϵ������3                                                                                       
							  char * LXRGX3,          //��ϵ�˹�ϵ����3                                                                                   
							  char * LXRDH3			  //��ϵ�˵绰3
							  
							  );
//ְҵ������Ϣ�ļ�
MHC_CARDINTERFACE_API int __stdcall iR_DF01EF07Info(HANDLE hDev,
							  char * WHCD,          //�Ļ��̶ȴ���
							  char * HYZK,			//����״������
							  char * ZY             //ְҵ����
							  );

MHC_CARDINTERFACE_API int __stdcall iW_DF01EF07Info(HANDLE hDev,
							  char * WHCD,          //�Ļ��̶ȴ���
							  char * HYZK,			//����״������
							  char * ZY             //ְҵ����
							  );
//֤����¼��Ϣ�ļ�
MHC_CARDINTERFACE_API int __stdcall iR_DF01EF08Info(HANDLE hDev,
							  char * ZJLB,            //֤�������룬�������֤�ź�֤��������һ�����
							  char * ZJHM,            //֤������
							  char * JKDAH,           //����������
							  char * XNHZH            //��ũ��֤��
							  );

MHC_CARDINTERFACE_API int __stdcall iW_DF01EF08Info(HANDLE hDev,
							  char * ZJLB,            //֤�������룬�������֤�ź�֤��������һ�����
							  char * ZJHM,            //֤������
							  char * JKDAH,           //����������
							  char * XNHZH            //��ũ��֤��
							  );
//�ٴ����������ļ�
MHC_CARDINTERFACE_API int __stdcall iR_DF02EF05Info(HANDLE hDev,
							  char * ABOXX,           //ABOѪ�ʹ���
							  char * RHXX,            //RHѪ�ʹ���
							  char * XCBZ,            //������־
							  char * XZBBZ,           //���ಡ��־
							  char * XNXGBBZ,         //����Ѫ�ܲ���־
							  char * DXBBZ,           //��ﲡ��־
							  char * NXWLBZ,          //��Ѫ���ұ�־
							  char * TNBBZ,           //���򲡱�־
							  char * QGYBZ,           //����۱�־
							  char * TXBZ,            //͸����־
							  char * QGYZBZ,          //������ֲ��־
							  char * QGQSBZ,          //����ȱʧ��־
							  char * KZXYZBZ,         //��װж��֫��־
							  char * XZQBQBZ,         //����������־
							  char * QTYXJSMC         //����ҽѧ��ʾ����
							  );

MHC_CARDINTERFACE_API int __stdcall iW_DF02EF05Info(HANDLE hDev,
							  char * ABOXX,           //ABOѪ�ʹ���
							  char * RHXX,            //RHѪ�ʹ���
							  char * XCBZ,            //������־
							  char * XZBBZ,           //���ಡ��־
							  char * XNXGBBZ,         //����Ѫ�ܲ���־
							  char * DXBBZ,           //��ﲡ��־
							  char * NXWLBZ,          //��Ѫ���ұ�־
							  char * TNBBZ,           //���򲡱�־
							  char * QGYBZ,           //����۱�־
							  char * TXBZ,            //͸����־
							  char * QGYZBZ,          //������ֲ��־
							  char * QGQSBZ,          //����ȱʧ��־
							  char * KZXYZBZ,         //��װж��֫��־
							  char * XZQBQBZ,         //����������־
							  char * QTYXJSMC        //����ҽѧ��ʾ����
							  );
//������Ϣ�����ļ�
MHC_CARDINTERFACE_API int __stdcall iR_DF02EF06Info(HANDLE hDev,
							  char * JSBBZ           //���񲡱�־
							  );


MHC_CARDINTERFACE_API int __stdcall iW_DF02EF06Info(HANDLE hDev,
							  char * JSBBZ           //���񲡱�־
							  );



//�������������ļ�
//nRecorderNo ��¼��
MHC_CARDINTERFACE_API int __stdcall iR_DF02EF07Info(HANDLE hDev, int nRecorderNo, char* GMWZMC, char* GMFY);

//������ѭ����¼����д���������Զ�ǵ�һ������
MHC_CARDINTERFACE_API int __stdcall iW_DF02EF07Info(HANDLE hDev,
							  char * GMWZMC,        //������������
							  char * GMFY			//�������ʷ�Ӧ
							  );
//���߻��������ļ�
MHC_CARDINTERFACE_API int __stdcall iR_DF02EF08Info(HANDLE hDev, int nRecorderNo, char* MYJZMC, char* MYJZSJ);
MHC_CARDINTERFACE_API int __stdcall iW_DF02EF08Info(HANDLE hDev,
							  char* MYJZMC,//���߽������� 
							  char* MYJZSJ //���߽���ʱ��
							  );

//סԺ��Ϣ�����ļ�
MHC_CARDINTERFACE_API int __stdcall iR_DF03EF05Info(HANDLE hDev,
							  char * ZYJLBS1,             //סԺ��¼��Ч��־
							  char * ZYJLBS2,
							  char * ZYJLBS3
							  );
//��סԺ��Ч��ʶ����Ϊ0x00
MHC_CARDINTERFACE_API int __stdcall iW_DF03EF05Info(HANDLE hDev,
							  int nRecorderNo
							  );

//��סԺ��Ч��ʶ����Ϊ0xff
MHC_CARDINTERFACE_API int __stdcall iErase_DF03EF05Info(HANDLE hDev,
								  int nRecorderNo
								  );


//������Ϣ�����ļ�
MHC_CARDINTERFACE_API int __stdcall iR_DF03EF06Info(HANDLE hDev,
							  char * MZJLBS1, //�����¼��Ч��ʶ
							  char * MZJLBS2,
							  char * MZJLBS3,
							  char * MZJLBS4,
							  char * MZJLBS5
							  );
//д0x00
MHC_CARDINTERFACE_API int __stdcall iW_DF03EF06Info(HANDLE hDev,
							  int nRecorderNo
							  );
//д0xff
MHC_CARDINTERFACE_API int __stdcall iErase_DF03EF06Info(HANDLE hDev,
								  int nRecorderNo
								  );


//סԺ��Ϣ�ļ�

//1. nStyle����ȡ����4��ֵ��

// #define NO_style			0x00
// 
// #define AN_style			0x01
// #define CN_style			0x0F
// #define B_style			0xFF

//2.���ȫ��ȫд�������д����EE01�����ݣ���ô nStyle=NO_style, pos=0, Len=1639(����Ҫ����EEXX�ļ������2��)
//	�ļ����2��Ϊ��ǩ��������ʱ��д������ݡ�

//3.�����д���������pos\len\style��ȡֵ��ο�����ĵ���

//4.pos\lenȡֵ�ĵ�λ�ǿ��ڲ��洢�ռ���ֽڵĴ�С

//5.������ڴ洢���ݵ�������CN_style��B_style�� szData��size��Ҫ����(���ڴ洢�ռ��2��+1)


MHC_CARDINTERFACE_API int __stdcall iR_DF03EEInfo(HANDLE hDev,
							  int	 nRecorderNo,
							  char * szData,											  
							  int	 nPos,
							  int	 nLen,
							  int	 nStyle
							  );

MHC_CARDINTERFACE_API int __stdcall iR_DF03EE_ALLInfo(HANDLE hDev,
							  int	 nRecorderNo,
							  char * szData											  
							  );//ȫ��

MHC_CARDINTERFACE_API int __stdcall iW_DF03EEInfo(HANDLE hDev,
							  int	 nRecorderNo,
							  char * szData,											  
							  int	 nPos,
							  int	 nLen,
							  int	 nStyle
							  );

MHC_CARDINTERFACE_API int __stdcall iW_DF03EE_ALLInfo(HANDLE hDev,
							  int	 nRecorderNo,
							  char * szData										  
							  );//ȫд


//������Ϣ�ļ�
MHC_CARDINTERFACE_API int __stdcall iR_DF03EDInfo(HANDLE hDev,
							  int	 nRecorderNo,
							  char * szData,											  
							  int	 nPos,
							  int	 nLen,
							  int	 nStyle
							  );

MHC_CARDINTERFACE_API int __stdcall iR_DF03ED_ALLInfo(HANDLE hDev,
							  int	 nRecorderNo,
							  char * szData											  
							  );//ȫ��

MHC_CARDINTERFACE_API int __stdcall iW_DF03EDInfo(HANDLE hDev,
							  int	 nRecorderNo,
							  char * szData,											  
							  int	 nPos,
							  int	 nLen,
							  int	 nStyle
							  );

MHC_CARDINTERFACE_API int __stdcall iW_DF03ED_ALLInfo(HANDLE hDev,
							  int	 nRecorderNo,
							  char * szData										  
							  );//ȫд


//����������Ŷ�����
MHC_CARDINTERFACE_API DWORD __stdcall wsc_GetCardInfo(HANDLE hCard,void *pvSelect,int nItem);
//�����������д����
MHC_CARDINTERFACE_API DWORD __stdcall wsc_PutCardInfo(HANDLE hCard,void *pvSelect,int nItem);

MHC_CARDINTERFACE_API HANDLE __stdcall OpenDevice(int port);

MHC_CARDINTERFACE_API DWORD __stdcall	CloseDevice(HANDLE hCard);

MHC_CARDINTERFACE_API DWORD  __stdcall SendAPDU (
				HANDLE hDev,
				unsigned char bySlot,
				unsigned char * pbyC_Command,
				unsigned long byLen,
				unsigned char * pbyR_Command,
				int * pnRes );

/*
������wsc_SM3Digest
���ܣ����������ݽ���SM3��ϣ����
	����˵����
	hCard		: [in] �豸���
	pbData		: [in] ����ϣ����
	nDataLen	: [in] ����ϣ���ݳ���
	pbHash		: [out]��ϣ���
	pbHashLen	: [out]ָ���ϣ������ȵ�ָ��
	����ֵ������ɹ�������0��ʧ�ܷ�����Ӧ�Ĵ����롣
*/
MHC_CARDINTERFACE_API DWORD __stdcall SM3Digest(HANDLE hCard, BYTE *pbData, unsigned int nDataLen,  BYTE *pbHash, BYTE *pbHashLen);


/*
����:wsc_SM2SignHash
���ܣ����������ݽ���SM2ǩ������
	����˵����
	hCard		: [in] �豸���
	pbData		: [in] ��ǩ�����ݣ�ԭʼ���ݵĹ�ϣ���
	bLen		: [in] ��ǩ����ϣ���ݳ���
	pbSignedData: [out]ǩ�����
	pbLength	: [out]ָ���ϣ������ȵ�ָ��
	����ֵ������ɹ�������0��ʧ�ܷ�����Ӧ�Ĵ����롣

˵������ִ��ǩ��ǰ����Ҫwsc_VerifyPin��֤PIN��ȡ��Ӧ��Ȩ��
*/
DWORD __stdcall SM2SignHash(HANDLE hCard,
								BYTE *pbData,
								BYTE bLen,
								BYTE *pbSignedData,
								BYTE *pbLength);


#endif