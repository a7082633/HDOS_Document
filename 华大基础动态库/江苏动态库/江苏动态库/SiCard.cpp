#include "stdafx.h"
#include "SSSE32.h"
#include <Winuser.h> 
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <string>
using namespace std;
#include <mbstring.h>
#include "WAPP.h"
#include "SiCard.h"
#include "Application.h"
#include <locale.h>

long g_handle =0;

long cardtype=0;//1:CPU卡  2:4428卡 ......
long fileType=0;// 1:记录文件  2：B-TLV 3：循环文件

char EffectDFEF[20]={0};
char EffectDF[20]={0};
char EffectDFEF1[20]={0};
char EffectDF1[20]={0};
bool fVerify= false;

char pErrMsg[512]={0};
int flag=0;
int pReaderHandle=0;

char * ChFflag(int flag);


int iReadCardFlagX(char *nCardFlag , char* pErrMsg);
int iReadCardX(const char* pInputPin, const char* pFileAddr, char* pOutDataBuff , char*pErrMsg);
int iWriteCardX(const char* pInputPin, const char* pFileAddr, char* pWriteDataBuff,char* pErrMsg);
int iCardControlX (const char* szHosSvrForSiIP,const char* szHosSvrForSiPort,const char * pCtrlType, const char * pCtrlInfo,char*  pErrMsg);
int iCardReadAtr(int pReaderHandle, char* pCardAtr, char* pErrMsg);
int iCardReadAtrX (char* pCardAtr, char* pErrMsg);
int iReadCardNoX(const char* pInputPin, char* pCardNo, char* pErrMsg);
int iReadPin(int pReaderHandle,const char * pCtrlType,char *pInputPin, char* pErrMsg);
int iReadPinX (const char * pCtrlType,char *pInputPin, char* pErrMsg);
int iReadVer (char *pVerInfo, char* pErrMsg);
int iReadCardEX(const char* pInputPin, const char* pFileAddr, char* pOutDataBuff,char* pErrMsg);
int iWriteCardEX(const char* pInputPin, const char* pFileAddr, char* pWriteDataBuff,char* pErrMsg);
int iReadIDMsg(int pReaderPort, const char* pBmpFile, char *pName, char *pSex, char *pNation, char *pBirth, char *pAddress, char *pCertNo, char *pDepartment , char *pExpire,char * pErrMsg);


int chartoint1(char c)
{
	switch (c)
	{
	case '0':
		return 0;break;
	case '1':
		return 1;break;
	case '2':
		return 2;break;
	case '3':
		return 3;break;
	case '4':
		return 4;break;
	case '5':
		return 5;break;
	case '6':
		return 6;break;
	case '7':
		return 7;break;
	case '8':
		return 8;break;
	case '9':
		return 9;break;
	case 'A':
	case 'a':
		return 10;break;
	case 'B':
	case 'b':
		return 11;break;
	case 'C':
	case 'c':
		return 12;break;
	case 'D':
	case 'd':
		return 13;break;
	case 'E':
	case 'e':
		return 14;break;
	case 'F':
	case 'f':
		return 15;break;
	default:return 1000;
		break;
	}

	return 0;
}

void StrToHex(unsigned char *Src,unsigned char *Des)
{
	int len = strlen((char*)Src);
	len = len/2;

	for(int i=0;i<len;i++)
	{
		Des[i]=chartoint1(Src[2*i])*16+chartoint1(Src[2*i+1]);
	}
}

void HexToStr(unsigned char *Src,unsigned char *Des,int len )
{
	char a[3]={0};
	for(int i=0;i<len;i++)
	{
		sprintf(a,"%02X",Src[i]);
		if (i==0)
			strcpy((char*)Des,a);
		else
			strcat((char*)Des,a);
	}
}
int getCount(char *strSrc,char ch)
{
	int len = strlen(strSrc);
	if(len <=0)
		return -1;

	int n=0;
	for(int i=0;i<len;i++ )
	{
		if(strSrc[i]==ch)
			n++;
	}
	return n;
}

int findChar(char *strSrc,int start,char ch)
{
	int len = strlen(strSrc);
	if(start > len)
		return -1;
	for(int i=start;i<len;i++)
	{
		if(strSrc[i]==ch)
			return i;
	}
	return -1;	
}

bool IsOtherChar(char* data)//1~0 A_Z a_z | $ :
{
	int len = strlen(data);
	for(int i=0;i<len;i++)
	{
		if((data[i]>=0x30 && data[i]<=0x39) || (data[i]>='a' && data[i]<='f') || (data[i]>='A' && data[i]<='F') || data[i]=='|' || data[i]=='$')
		{
			
		}
		else
			return false;
	}
	return true;
}

bool IsEffectChar(char *data)
{
	int len = strlen(data);
	for(int i=0;i<len;i++)
	{
		if((data[i]>=0x30 && data[i]<=0x39) || (data[i]>='a' && data[i]<='f') || (data[i]>='A' && data[i]<='F'))
		{
			
		}
		else
			return false;
	}
	return true;
}

bool IsDec(char *data)
{
	int len = strlen(data);
	for(int i=0;i<len;i++)
	{
		if((data[i]>=0x30 && data[i]<=0x39))
		{
			
		}
		else
			return false;
	}
	return true;
}

int VerifyPIN(const char* PIN,char* pErrMsg)
{

	unsigned char Response[300]={0};
	unsigned char cmd[200]={0};
	
	int len = strlen(PIN);
	if(len>16 || len<=0 || (len%2)!=0)
	{
		strcpy(pErrMsg,"输入的密码位数不对，或者没有输入密码!");
		return SB_ERR_UNSUPPORTED;
	}

	memset(cmd,0,50);
	memset(Response,0,128);
	memcpy(cmd, "\x00\x20\x00\x00\x06",5);
	cmd[4] = len/2;

	for(int i=0;i<len/2;i++)
	{
		cmd[5+i]=chartoint1(PIN[2*i])*16+chartoint1(PIN[2*i+1]);
	}

	int re = ICC_Reader_Application(g_handle,USERCard,5+len/2,cmd,Response);//校验密钥
	if(re<=0 || Response[re-2]!=0x90)
	{
		if(Response[0]==0x63)
		{
			int i = Response[1]%0x10;
			char s[50]={0};
			sprintf(s,"密钥校验失败，还剩%d次认证次数!",i);
			strcpy(pErrMsg,s);
		}
		else
			strcpy(pErrMsg,"密钥校验失败!");
		return SB_ERR_AUTHPIN;
	}

	return SB_ERR_OK;
}


/*********************************44428**************************************/

int WINAPI ic_init(int port, long baud)
{
	unsigned char data[4];
	if(port==1 && baud==9600)
	{
		pReaderHandle =ICC_Reader_Open("USB1");
		ICC_Reader_4442_PowerOn(pReaderHandle,data);
		if (pReaderHandle<=0)
		{
			flag=201;
			return 201;
		}
		else
		{
			flag=0;
			return pReaderHandle;
		}
	}
	else
		return -1;
	
}
int WINAPI ic_exit(int icdev)
{
	if(icdev==pReaderHandle)
	{
		int ret=ICC_Reader_Close(icdev);
		if(ret!=0)
		{
			flag=213;
			return 213;
		}
	}
	else
	{
		flag=214;
		return 214;
	}
	
	return 0;
}
int WINAPI srd_4428(int icdev, int offset, int len, unsigned char* r_string )
{
	unsigned char data[1024]={0};
	int ret=ICC_Reader_4428_Read(icdev,offset,len,data);
	if(ret!=0)
	{
		flag=211;
		return 211;
	}
	for (int i=0;i<len;i++)
	{
		r_string[i]=data[i];
	}
	return 0;
}
int WINAPI swr_4428(int icdev, int offset, int len, unsigned char* w_string )
{
	int ret;
	ret=ICC_Reader_4428_Write(icdev,offset,len,w_string);
	if(ret!=0)
	{
		flag=212;
		return 212;
	}
	return 0;	
}
int	WINAPI get_status(int icdev,int *state)
{
	unsigned char data[1024];
	int ret=ICC_Reader_4428_PowerOn(icdev,data);
	if(ret!=0)
	{
		flag=204;
		state[0]=0;
	}
	else
		state[0]=1;
	return 0;
}
int WINAPI csc_4428(int icdev, int len, unsigned char* p_string)
{
	int ret;
	char str[1024];
	int slen;
	int i=0;
	while(p_string[i]!='\0')
	{
		str[i]=p_string[i];
		i++;
	}
	slen=strlen(str)/2;
	
	if (slen!=len)
	{
		flag=200;
		return 200;
	}
	ret=ICC_Reader_4428_Verify(icdev,p_string);
	if (ret!=0)
	{
		flag=207;
		return 207;
	}
	return 0;
}
int WINAPI wsc_4428(int icdev, int len,unsigned char* p_string)
{
	int ret;
	char str[1024];
	int slen;
	int i=0;
	while(p_string[i]!='\0')
	{
		str[i]=p_string[i];
		i++;
	}
	slen=strlen(str)/2;
	
	if (slen!=len)
	{
		flag=200;
		return 200;
	}
	ret=ICC_Reader_4428_Change(icdev,p_string); 
	if (ret!=0)
	{
		flag=-206;
		return -206;
	}
	return 0;
}
int WINAPI chk_card(int icdev)
{
	unsigned char Response[128]={0};
	int re;
	unsigned char data[1024];
	int reVal;
	int ret;
	
	ret=ICC_Reader_4428_Read(pReaderHandle,0,1,data);
	reVal=data[0];
	if(reVal==146)
		return 31;
	else
	{
		ret=ICC_Reader_4442_Read(pReaderHandle,0,1,data);
		if(reVal==162 || reVal==163)
			return 21;
	}
	
	ret=ICC_Reader_pre_PowerOn(pReaderHandle,USERCard,Response);
	if(ret>0)
	{
		return 64;
	}

	return 0;
}

char * ChFflag(int flag)
{
	switch (flag)
	{
	case 200:
		return "IC卡其他错误";
	case 201:
		return "IC卡打开端口失败";
	case 202:
		return "IC卡读卡器未连接";
	case 203:
		return "IC卡串口被占用";
	case 204:
		return "IC卡读卡器内无卡";
	case 205:
		return "IC卡卡类型不匹配";
	case 206:
		return "IC卡卡密钥校验失败";
	case 207:
		return "IC卡卡密码校验失败";
	case 208:
		return "读写卡输入串格式错误";
	case 209:
		return "读写卡输入操作数越界";
	case 210:
		return "读写卡输入操作数非法";
	case 211:
		return "IC卡读取卡上信息失败";
	case 212:
		return "IC卡信息写入卡上失败";
	case 213:
		return "IC卡关闭端口失败";
	case 214:
		return "IC卡其他异常错误";
	case -206:
		return "IC卡修改密码失败";
	default:
		return "还没出错";
	}
}

void WINAPI getErrorMsg(int icdev,char* ErrorMsg)
{
	if (icdev!=pReaderHandle)
	{
		flag=200;
	}

	char str[1024]={0};

	for (int j=0;;j++)
	{
		str[j]=ChFflag(flag)[j];
		if(str[j]=='\0')
			break;
	}

	strcpy(ErrorMsg,ChFflag(flag));

	int i=0;
	while (1)
	{
		ErrorMsg[i]=str[i];
		if(ErrorMsg[i]==0)
			break;
		i++;
	}

}


/********************江苏的从这里开始*************************************/



int WINAPI iOpenPort( char * pErrMsg)
{

/*	if(g_handle>0)
	{
		strcpy(pErrMsg,"读写器已打开");
		return SB_ERR_CONNECT;
	}
*/
	int pReaderPort=0;
	if(pReaderPort==0)
	{
		int port;
		port = GetPrivateProfileInt("CARD","PORT",NULL,"./JsSiCard.ini");
		if(port!=99)
		{
			strcpy(pErrMsg,"配置文件错误 card-port");
			return SB_ERR_UNKNOWN;
		}
		port = GetPrivateProfileInt("ID","PORT",NULL,"./JsSiCard.ini");
		if(port!=99)
		{
			strcpy(pErrMsg,"配置文件错误 id-port");
			return SB_ERR_UNKNOWN;
		}

		int user = GetPrivateProfileInt("SlotNo","USER",NULL,"./JsSiCard.ini");
		if(user!=0x01)
		{
			strcpy(pErrMsg,"配置文件错误 slotNo- user");
			return SB_ERR_UNKNOWN;
		}

		int sam = GetPrivateProfileInt("SlotNo","SAM",NULL,"./JsSiCard.ini");
		if(sam<0x11 || sam>0x14)
		{
			strcpy(pErrMsg,"配置文件错误 slotNo-sam");
			return SB_ERR_UNKNOWN;
		}
		PSAM_Slot = sam;

		int keytype = GetPrivateProfileInt("KEY","KEYTYPE",NULL,"./JsSiCard.ini");
		if(keytype!=1)
		{
			strcpy(pErrMsg,"配置文件错误 key-keytype");
			return SB_ERR_UNKNOWN;
		}

		port = GetPrivateProfileInt("KEY","PORT",NULL,"./JsSiCard.ini");
		if(port!=1)
		{
			strcpy(pErrMsg,"配置文件错误 key-port");
			return SB_ERR_UNKNOWN;
		}
	}
	else
	{
		if(pReaderPort!=99 || pErrMsg==NULL )
		{
			strcpy(pErrMsg,"入参错误");
			return SB_ERR_UNKNOWN;
		}
	}
	
	long read = ICC_Reader_Open("USB1");
	if(read<=0)
	{
		strcpy(pErrMsg,"连接读写器失败,请插入读写器");
		return SB_ERR_CONNECT;
	}
	pReaderHandle = read;
	g_handle = read;
	return SB_ERR_OK;
}

int	WINAPI iClosePort(char* pErrMsg)
{

	if(pReaderHandle<=0 || pErrMsg==NULL )
		return SB_ERR_CLOSEPORT;
	ICC_Reader_Close(pReaderHandle);
	g_handle =-1;

	return SB_ERR_OK;
}

int WINAPI iReadCardFlag(char *nCardFlag , char* pErrMsg)//判断是省统一社保卡还是本地其他社保卡
{
	if(pReaderHandle<=0)
	{
		strcpy(pErrMsg,"读写器未连接");
		return SB_ERR_UNCONNECT;
	}
	if(g_handle!=pReaderHandle)
	{
		strcpy(pErrMsg,"句柄值无效");
		return SB_ERR_UNCONNECT;
	}

	unsigned char Response[128]={0};
	ICC_Reader_pre_PowerOn(pReaderHandle,0x11, Response);
	int re = ICC_Reader_pre_PowerOn(pReaderHandle,USERCard,Response);
	if(re<=0)//CPU上电失败，有可能是4428卡,进一步进行判断
	{
		strcpy(pErrMsg,"上电失败，请检查卡片");
		strcpy(nCardFlag,"31");
		return SB_ERR_CARDMISS;
	}

	memset(Response,0,128);
	unsigned char cmd[200]={0};
	memset(cmd,0,50);
	memset(Response,0,128);
	memcpy(cmd, "\x00\xA4\x04\x00\x0F\x73\x78\x31\x2E\x73\x68\x2E\xC9\xE7\xBB\xE1\xB1\xA3\xD5\xCF",20);
	//00A404000F7378312E73682EC9E7BBE1B1A3D5CF
	 re = ICC_Reader_Application(g_handle,USERCard,20,cmd,Response);//X选择MF文件
	if(re<=0)
	{
		strcpy(nCardFlag,"其他卡型-00A4命令执行失败");
		return SB_ERR_CARDTYPE;
		//strcpy(pErrMsg,"卡类型错误-DDF1!");
		//return SB_ERR_CARDTYPE;
	}
	if(Response[re-2]!=0x61 && Response[re-2]!=0x90 )
	{
		strcpy(nCardFlag,"其他卡型-00A4");
		unsigned char temp[10]={0};
		HexToStr(Response+(re-2),temp,2);
		strcat(nCardFlag,(char*)temp);
		return SB_ERR_CARDTYPE;
	}

	memset(cmd,0,50);
	memset(Response,0,128);
	memcpy(cmd, "\x00\xA4\x00\x00\x02\xEF\x05",7);
	re = ICC_Reader_Application(g_handle,USERCard,7,cmd,Response);//X选择EF05文件
	if(re<=0 || Response[re-2]!=0x90)
	{
		strcpy(nCardFlag,"其他卡型00A4-EF05");
		unsigned char temp[10]={0};
		HexToStr(Response+(re-2),temp,2);
		strcat(nCardFlag,(char*)temp);
		return SB_ERR_CARDTYPE;
	}

	memset(cmd,0,50);
	memset(Response,0,128);
	memcpy(cmd, "\x00\xB2\x03\x04\x06",5);
	re = ICC_Reader_Application(g_handle,USERCard,5,cmd,Response);//读取规范版本
	if(re<=0 || Response[re-2]!=0x90)
	{
		strcpy(nCardFlag,"其他卡型-00B2");
		unsigned char temp[10]={0};
		HexToStr(Response+(re-2),temp,2);
		strcat(nCardFlag,(char*)temp);
		return SB_ERR_CARDTYPE;
	}
	char iType[5]={0};
	memcpy(iType,Response+2,4);//获取规范版本
	if(strcmp(iType,"2.00")==0)
	{
		strcpy(nCardFlag,"11");
	}
	else
	{
		strcpy(nCardFlag,"其他卡型");
	}

	return SB_ERR_OK;
}

int iReadCardFlagX(char *nCardFlag , char* pErrMsg)
{
	if(g_handle>0)
	{
		strcpy(pErrMsg,"读写器已打开，请选关闭读写器");
		return SB_ERR_CONNECT;
	}

	int pReaderHandle=0;
	int re = iOpenPort(pErrMsg);
	if(re!=0)
		return re;
	if(GetAtrCityCode(pErrMsg)!=0)
	{
		return SB_ERR_CARDTYPE;
	}
	re = iReadCardFlag(nCardFlag , pErrMsg);
	if(re!=0)
	{
		iClosePort(pErrMsg);
		return re;
	}
	re = iClosePort(pErrMsg);
	if(re!=0)
		return re;
	return SB_ERR_OK;
}

int WINAPI iReadCard(const char* pInputPin, const char* pFileAddr, char* pOutDataBuff)
{//file1|data1|…$file2|data1|data2…$file3:1|data1…$file3:2data1|…$......$file3:A|data1$......$
	char nCardFlag[512];
	int re=GetAtrCityCode(pErrMsg);

	strcpy(EffectDF1,"");
	strcpy(EffectDFEF1,"");
	if(pReaderHandle<=0)
	{
		strcpy(pErrMsg,"读写器未连接");
		return SB_ERR_UNCONNECT;
	}	
	if(g_handle!=pReaderHandle)
	{
		strcpy(pErrMsg,"句柄值无效");
		return SB_ERR_CONNECT;
	}
	
	char data[9000]={0};//存放读取后的数据
	int filelen = strlen(pFileAddr);
	if(filelen<8)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED;
	}
	int ad = findChar((char*)pFileAddr,0,':');
	int addr = findChar((char*)pFileAddr,0,'|');//先对字符串格式进行判断
	if(addr<0 && ad<0)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED;
	}
	int count = getCount((char*)pFileAddr,'$');//获取需要循环的次数
	if(count<0)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED;
	}
	if(getCount((char*)pFileAddr,' ')>0)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED;
	}

	char temp[300]={0};
	char data_item[0x300]={0};//存放数据项
	char data_DFEF[0x100]={0};//存放DF和EF
	char DDF[10]={0};
	char EEF[10]={0};

	int addr_data=0;//记录$的位置 EF的分隔符
	int addr_item =0;//记录|的位置  记录的分隔符  
	addr =0;
	int addr1=0,addr2=0;
	int ret;
/*	
	if(GetAtrCityCode(pErrMsg)!=0)
	{
		return SB_ERR_TYPEERROR;
	}
	*/
	//MFEF05|07|$MFEF06|08|09|0A|$
	for(int i=0;i<count;i++)//DF范围内循环
	{
		addr1 = findChar((char*)pFileAddr,addr_data,'$');// pFileAddr = MFEF05|07|$MFEF06|08|09|0A|$
		memset(temp,0,100);
		memcpy(temp,pFileAddr+addr_data,addr1-addr_data);//获取DF和EF以及数据项  temp = MFEF05|07|08|
		addr_data=addr1+1;
		int len,iCount;

		ad = findChar((char*)temp,0,':');
		addr = findChar((char*)temp,0,'|');//先对字符串格式进行判断

		if(ad>0 && addr>0 && addr<ad) //B-TLV格式
		{
			addr2 = findChar((char*)temp,0,'|');
			memset(data_DFEF,0,0x100);
			memcpy(data_DFEF,temp,addr2);//获取DF和EF  data_DFEF = DF02EF07

			len = strlen(data_DFEF);
			memset(data_item,0,0x300);
			memcpy(data_item,temp+addr2+1,addr1-addr2-1);// data_item = 99:01|99:02|
			if(strlen(data_item)<6)
			{
				fileType =0;
				strcpy(pErrMsg,"输入字符串格式错误");
				return SB_ERR_UNSUPPORTED; 
			}
			iCount = getCount((char*)data_item,'|');//获取|数量
			fileType =2;
		}
		else if(ad>0 && addr<0)//循环文件 DF02EF07:01$DF02EF07:02$DF02EF07:03$DF02EF07:04$
		{
			addr2 = findChar((char*)temp,0,':');
			memset(data_DFEF,0,0x100);
			memcpy(data_DFEF,temp,addr2);//获取DF和EF  data_DFEF = DF02EF07

			len = strlen(data_DFEF);
			memset(data_item,0,0x300);
			memcpy(data_item,temp+addr2+1,addr1-addr2-1);// data_item = 01|

			fileType = 3;
		}
		else if(ad<0 && addr>0)
		{
			addr2 = findChar((char*)temp,0,'|');
			memset(data_DFEF,0,0x100);
			memcpy(data_DFEF,temp,addr2);//获取DF和EF  data_DFEF = MFEF05
			len = strlen(data_DFEF);
			memset(data_item,0,0x300);
			memcpy(data_item,temp+addr2+1,addr1-addr2-1);// data_item = 07|08|
			iCount = getCount((char*)data_item,'|');//获取|数量
			fileType =1;
		}
		else
		{
			strcpy(pErrMsg,"输入字符串格式错误");
			return SB_ERR_UNSUPPORTED; 
		}
		if(len==6)//说明是MF
		{
			memset(DDF,0,10);
			memcpy(DDF,data_DFEF,2);
			memcpy(EEF,data_DFEF+2,4);
		}
		else if(len==8)
		{
			memcpy(DDF,data_DFEF,4);
			memcpy(EEF,data_DFEF+4,4);
		}
		else
		{
			strcpy(pErrMsg,"输入字符串格式错误");
			return SB_ERR_UNSUPPORTED;
		}

		if (strcmp(DDF,"MF")==0 || strcmp(DDF,"DDF1")==0)
		{
			if(strcmp(EffectDF,DDF)!=0)//data_DFEF 获取的DF/EF  
			{
				if(SelectFile("DDF1"))
				{
					strcpy(pErrMsg,"卡未上电");
					return SB_ERR_UNSUPPORTED; 
				}
				strcpy(EffectDF,DDF);
			//	strcpy(EffectDFEF,data_DFEF);
			}/**/
			if (strcmp(EEF,"EF05")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 1:ret = iReadRecord("EF05",recno,0x10,CN_style,result);break;
					case 2:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
					case 3:ret = iReadRecord("EF05",recno,0x04,AN_style,result);break;
					case 4:ret = iReadRecord("EF05",recno,0x0C,CN_style,result);break;
					case 5:ret = iReadRecord("EF05",recno,0x04,CN_style,result);break;
					case 6:ret = iReadRecord("EF05",recno,0x04,CN_style,result);break;
					case 7:ret = iReadRecord("EF05",recno,0x09,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					//case 8:ret = iReadRecord("EF05",recno,0x09,AN_style,result);break;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x08:ret = iReadRecord("EF06",recno,0x12,AN_style,result);break;
					case 0x09:ret = iReadRecord("EF06",recno,0x1E,AN_style,result);break;
					case 0x4E:ret = iReadRecord("EF06",recno,0x14,AN_style,result);break;
					case 0x0A:ret = iReadRecord("EF06",recno,0x01,AN_style,result);break;
					case 0x0B:ret = iReadRecord("EF06",recno,0x01,CN_style,result);break;
					case 0x0C:ret = iReadRecord("EF06",recno,0x03,CN_style,result);break;
					case 0x0D:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{
				char result[0x700]={0};
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x0A,0x0A};
				if(strcmp(EffectDFEF,data_DFEF)!=0)//data_DFEF 获取的DF/EF  
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"RKSSSE认证失败!");
						strcat(pErrMsg,err);
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF,data_DFEF);
					strcpy(EffectDF,DDF);
				}
				
				ret = iReadBinary("EF07",00,0x300,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
				strcpy(EffectDF,DDF);
				strcpy(EffectDFEF,data_DFEF);
			}
			else if (strcmp(EEF,"EF08")==0)
			{
				char result[0x2002]={0};
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x0A,0x0A};
				if(strcmp(EffectDFEF,data_DFEF)!=0)
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"RKSSSE认证失败!");
						strcat(pErrMsg,err);
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF,data_DFEF);
					strcpy(EffectDF,DDF);
				}
				ret = iReadBinary("EF08",00,0x1000,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
				strcpy(EffectDF,DDF);
				strcpy(EffectDFEF,data_DFEF);
			}
			else if (strcmp(EEF,"EF0D")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[113]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0xEA:ret = iReadRecord("EF0D",recno,0x0C,AN_style,result);break;
					case 0xEB:ret = iReadRecord("EF0D",recno,0x03,CN_style,result);break;
					case 0xEC:ret = iReadRecord("EF0D",recno,0x01,AN_style,result);break;
					case 0xED:ret = iReadRecord("EF0D",recno,0x01,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF0D结束符
			else
			{
				strcpy(pErrMsg,"输入字符串格式错误");
				return SB_ERR_UNSUPPORTED; 
			}

		}//DDF结束符
		else if(strcmp(DDF,"DF01")==0)
		{
			/*if(strcmp(EffectDF,DDF)!=0)
			{
				if(SelectFile("DDF1"))
				{
					strcpy(pErrMsg,"卡未上电");
					return SB_ERR_UNSUPPORTED; 
				}
			}*/
			if(strlen(pInputPin)!=6)
			{
				strcpy(pErrMsg,"输入PIN长度错误");
				return SB_ERR_OPNUMERROR;
			}
			if(!IsDec((char*)pInputPin))
			{
				strcpy(pErrMsg,"输入PIN有非法字符");
				return SB_ERR_OPNUMERROR;
			}
			if(strcmp(EffectDF,DDF)!=0)//如果是从别的DF转过来
			{
				if(SelectFile("DF01"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					return SB_ERR_UNSUPPORTED; 
				}
				ret = VerifyPIN(pInputPin,pErrMsg);
				if(ret!=0)
				{
					strcpy(EffectDF,"");
					strcpy(EffectDFEF,"");
					return ret;
				}
				strcpy(EffectDF,DDF);
				strcpy(EffectDFEF,data_DFEF);
			}
			if (strcmp(EEF,"EF05")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x20:ret = iReadRecord("EF05",recno,0x02,AN_style,result);break;
					case 0x21:ret = iReadRecord("EF05",recno,0x50,AN_style,result);break;
					case 0x0E:ret = iReadRecord("EF05",recno,0x03,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
				
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x23:ret = iReadRecord("EF06",recno,0x50,AN_style,result);break;
					case 0x24:ret = iReadRecord("EF06",recno,0x03,CN_style,result);break;
					case 0x28:ret = iReadRecord("EF06",recno,0x0F,AN_style,result);break;
					case 0x2C:ret = iReadRecord("EF06",recno,0x32,AN_style,result);break;
					case 0x2D:ret = iReadRecord("EF06",recno,0x0F,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;

			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x29:ret = iReadRecord("EF07",recno,0x01,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF07结束符
			else if (strcmp(EEF,"EF08")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x2B:ret = iReadRecord("EF08",recno,0x01,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF08结束符
			else if (strcmp(EEF,"EF09")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x2E:ret = iReadRecord("EF09",recno,0x46,AN_style,result);break;
					case 0x30:ret = iReadRecord("EF09",recno,0x09,AN_style,result);break;
					case 0x32:ret = iReadRecord("EF09",recno,0x02,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF09结束符
			else if (strcmp(EEF,"EF0A")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x37:ret = iReadRecord("EF0A",recno,0x03,AN_style,result);break;
					case 0x38:ret = iReadRecord("EF0A",recno,0x02,AN_style,result);break;
					case 0x39:ret = iReadRecord("EF0A",recno,0x04,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF0A结束符
			else if (strcmp(EEF,"EF15")==0)//BTLV
			{	
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						switch(recno)
						{
						case 0x2A:ret = iReadRecord("EF15",recno,0x01,CN_style,result);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,result);
							return ret;
						}
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"40")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[3]={{"57",0x03,AN_style},{"58",0x03,CN_style},{"59",0x28,AN_style}};
							ret = iReadBTLV("EF15",recno+1,0x34,tlv,3,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;

				}//循环结束
				addr_item =0;

			}//EF15结束符
			else if (strcmp(EEF,"EF16")==0)
			{
				char result[0x300]={0};
				
				ret = iReadBinary("EF16",00,0x128,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
			}
			else if (strcmp(EEF,"EF17")==0)
			{
				char result[0x300]={0};
				
				ret = iReadBinary("EF17",00,0x128,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
			}
			else if (strcmp(EEF,"EF18")==0)
			{
				char result[0x300]={0};
				
				ret = iReadBinary("EF18",00,0x128,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
			}
			else if (strcmp(EEF,"EF19")==0)
			{
				char result[0x300]={0};
				
				ret = iReadBinary("EF19",00,0x128,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
			}
			else if (strcmp(EEF,"EF1A")==0)
			{
				char result[0x300]={0};
				
				ret = iReadBinary("EF1A",00,0x128,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
			}
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF01结束
		else if(strcmp(DDF,"DF02")==0)
		{
			if(strlen(pInputPin)!=6)
			{
				strcpy(pErrMsg,"输入PIN长度错误");
				return SB_ERR_OPNUMERROR;
			}
			if(!IsDec((char*)pInputPin))
			{
				strcpy(pErrMsg,"输入PIN有非法字符");
				return SB_ERR_OPNUMERROR;
			}
			if(strcmp(EffectDF,DDF)!=0)//如果是从别的DF转过来
			{
				if(SelectFile("DF02"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					return SB_ERR_UNSUPPORTED; 
				}
				ret = VerifyPIN(pInputPin,pErrMsg);
				if(ret!=0)
				{
					strcpy(EffectDF,"");
					strcpy(EffectDFEF,"");
					return ret;
				}
				strcpy(EffectDF,DDF);
				strcpy(EffectDFEF,data_DFEF);
			}
			if (strcmp(EEF,"EF05")==0)//BTLV
			{	
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						switch(recno)
						{
						case 0x42:ret = iReadRecord("EF05",recno,0x03,AN_style,result);break;
						case 0x41:ret = iReadRecord("EF05",recno,0x03,AN_style,result);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,result);
							return ret;
						}
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"43")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[5]={{"5A",0x07,AN_style},{"5B",0x01,AN_style},{"5C",0x46,AN_style},{"5D",0x04,CN_style},{"5E",0x10,AN_style}};
							ret = iReadBTLV("EF05",recno+2,0x6C,tlv,5,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else if(strcmp(a,"44")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[4]={{"33",0x03,AN_style},{"34",0x46,AN_style},{"35",0x04,CN_style},{"36",0x10,AN_style}};
							ret = iReadBTLV("EF05",recno+4,0x65,tlv,4,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
						
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;

				}//循环结束
				addr_item =0;

			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x4C:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0x4B:ret = iReadRecord("EF06",recno,0x05,AN_style,result);break;
					case 0x4D:ret = iReadRecord("EF06",recno,0x03,CN_style,result);break;
					case 0x60:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0x4F:ret = iReadRecord("EF06",recno,0x06,AN_style,result);break;
					case 0x50:ret = iReadRecord("EF06",recno,0x03,CN_style,result);break;
					case 0x3A:ret = iReadRecord("EF06",recno,0x07,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)//循环文件
			{	
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
					char a[13]={0};
					char result[0x400]={0};
					memcpy(a,data_item,strlen(data_item));
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					if(!IsEffectChar(a))
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					if(recno>4)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					CYC cy[4]={{0x07,AN_style},{0x04,CN_style},{0x04,CN_style},{0x46,AN_style}};
					ret = iReadXHFile("EF07",recno,0x55,cy,4,result);
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");

			}//EF07结束符
			else if (strcmp(EEF,"EF09")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x55:ret = iReadRecord("EF09",recno,0x10,AN_style,result);break;
					case 0x56:ret = iReadRecord("EF09",recno,0x46,AN_style,result);break;
					case 0x96:ret = iReadRecord("EF09",recno,0x04,CN_style,result);break;
					case 0x97:ret = iReadRecord("EF09",recno,0x03,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF09结束符
			else if (strcmp(EEF,"EF15")==0)//BTLV
			{	
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						switch(recno)
						{
						case 0x0F:ret = iReadRecord("EF15",recno,0x04,CN_style,result);break;
						case 0x1F:ret = iReadRecord("EF15",recno,0x03,CN_style,result);break;
							default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,result);
							return ret;
						}
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"99")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[5]={{"10",0x03,AN_style},{"11",0x04,CN_style},{"12",0x03,CN_style}};
							ret = iReadBTLV("EF15",recno,0x10,tlv,3,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
						
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;

				}//循环结束
				addr_item =0;
			}//EF15结束符
			else if (strcmp(EEF,"EF16")==0)//循环文件
			{	
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char a[13]={0};
				char result[0x100]={0};
				memcpy(a,data_item,strlen(data_item));
				if(strlen(a)!=2)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(a))
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				int recno = chartoint1(a[0])*16+chartoint1(a[1]);
				if(recno>4)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				CYC cy[5]={{0x03,AN_style},{0x04,CN_style},{0x04,CN_style},{0x04,CN_style},{0x03,CN_style}};
				ret = iReadXHFile("EF16",recno,0x12,cy,5,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
				strcat(data,"|");
			}//EF16结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF02结束

		else if(strcmp(DDF,"DF03")==0)
		{
			if(strlen(pInputPin)!=6)
			{
				strcpy(pErrMsg,"输入PIN长度错误");
				return SB_ERR_OPNUMERROR;
			}
			if(!IsDec((char*)pInputPin))
			{
				strcpy(pErrMsg,"输入PIN有非法字符");
				return SB_ERR_OPNUMERROR;
			}

			if(strcmp(EffectDF,DDF)!=0)//如果是从别的DF转过来
			{
				if(SelectFile("DF03"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					return SB_ERR_UNSUPPORTED; 
				}
				ret = VerifyPIN(pInputPin,pErrMsg);
				if(ret!=0)
				{
					strcpy(EffectDF,"");
					strcpy(EffectDFEF,"");
					return ret;
				}
				strcpy(EffectDF,DDF);
				strcpy(EffectDFEF,data_DFEF);
			}
			if (strcmp(EEF,"EF05")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x61:ret = iReadRecord("EF05",recno,0x03,CN_style,result);break;
				//	case 0x60:ret = iReadRecord("EF05",recno,0x04,CN_style,result);break;
					case 0x98:ret = iReadRecord("EF05",recno,0x04,CN_style,result);break;
					case 0x62:ret = iReadRecord("EF05",recno,0x04,CN_style,result);break;
					case 0x63:ret = iReadRecord("EF05",recno,0x03,AN_style,result);break;
					case 0x64:ret = iReadRecord("EF05",recno,0x03,AN_style,result);break;
					case 0x65:ret = iReadRecord("EF05",recno,0x03,AN_style,result);break;
					case 0x66:ret = iReadRecord("EF05",recno,0x03,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;	
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)//BTLV
			{	
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误21");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						switch(recno)
						{
						case 0x45:ret = iReadRecord("EF06",recno,0x14,AN_style,result);break;
						case 0x46:ret = iReadRecord("EF06",recno,0x02,AN_style,result);break;
						case 0x47:ret = iReadRecord("EF06",recno,0x02,AN_style,result);break;
						case 0x48:ret = iReadRecord("EF06",recno,0x01,AN_style,result);break;
						case 0x49:ret = iReadRecord("EF06",recno,0x01,AN_style,result);break;
						case 0x67:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
						case 0x6B:ret = iReadRecord("EF06",recno,0x3C,AN_style,result);break;
						default: sprintf(data,"%x",recno);strcpy(pErrMsg,"输入字符串格式错误22 ");strcat(pErrMsg,data_item); return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,result);
							return ret;
						}
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"4A")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[5]={{"13",0x02,AN_style},{"14",0x01,AN_style},{"15",0x14,AN_style},{"16",0x04,CN_style},{"17",0x3C,AN_style}};
							ret = iReadBTLV("EF06",recno+7,0x61,tlv,5,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误25");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误26");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;

				}//循环结束
				addr_item =0;	
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x70:ret = iReadRecord("EF07",recno,0x03,AN_style,result);break;
					case 0x71:ret = iReadRecord("EF07",recno,0x03,CN_style,result);break;
					case 0x6E:ret = iReadRecord("EF07",recno,0x04,CN_style,result);break;
					case 0x6C:ret = iReadRecord("EF07",recno,0x04,CN_style,result);break;
					case 0x6F:ret = iReadRecord("EF07",recno,0x04,CN_style,result);break;
					case 0x73:ret = iReadRecord("EF07",recno,0x04,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF07结束符
			else if (strcmp(EEF,"EF15")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x7A:ret = iReadRecord("EF15",recno,0x03,CN_style,result);break;
					case 0x7B:ret = iReadRecord("EF15",recno,0x04,CN_style,result);break;
					case 0x7C:ret = iReadRecord("EF15",recno,0x04,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF15结束符
			else if (strcmp(EEF,"EF16")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x51:ret = iReadRecord("EF16",recno,0x03,CN_style,result);break;
					case 0x5F:ret = iReadRecord("EF16",recno,0x01,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF16结束符
			else if (strcmp(EEF,"EF17")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x3B:ret = iReadRecord("EF17",recno,0x14,AN_style,result);break;
					case 0x3C:ret = iReadRecord("EF17",recno,0x46,AN_style,result);break;
					case 0x3D:ret = iReadRecord("EF17",recno,0x01,AN_style,result);break;
					case 0x3E:ret = iReadRecord("EF17",recno,0x04,CN_style,result);break;
					case 0x3F:ret = iReadRecord("EF17",recno,0x3C,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF17结束符
			else if (strcmp(EEF,"EF18")==0)//BTLV
			{	
			
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x74:ret = iReadRecord("EF18",recno,0x03,AN_style,result);break;
					case 0x75:ret = iReadRecord("EF18",recno,0x03,CN_style,result);break;
					case 0x76:ret = iReadRecord("EF18",recno,0x01,CN_style,result);break;
					case 0x77:ret = iReadRecord("EF18",recno,0x04,CN_style,result);break;
					case 0x78:ret = iReadRecord("EF18",recno,0x04,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF18结束符
			else if (strcmp(EEF,"EF19")==0)//BTLV
			{	
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"79")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);
							moneyFlag=true;
							TLV tlv[7]={{"18",0x03,AN_style},{"19",0x03,CN_style},{"1A",0x04,CN_style},{"1B",0x04,CN_style},{"1C",0x04,CN_style},{"1D",0x03,AN_style},{"1E",0x04,CN_style}};
							ret = iReadBTLV("EF19",recno,0x27,tlv,7,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;

				}//循环结束
				addr_item =0;		
			}//EF19结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF03结束
		else if(strcmp(DDF,"DF04")==0)
		{
			if(strlen(pInputPin)!=6)
			{
				strcpy(pErrMsg,"输入PIN长度错误");
				return SB_ERR_OPNUMERROR;
			}
			if(!IsDec((char*)pInputPin))
			{
				strcpy(pErrMsg,"输入PIN有非法字符");
				return SB_ERR_OPNUMERROR;
			}
			if(strcmp(EffectDF,DDF)!=0)//如果是从别的DF转过来
			{
				if(SelectFile("DF04"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					return SB_ERR_UNSUPPORTED; 
				}
				ret = VerifyPIN(pInputPin,pErrMsg);
				if(ret!=0)
				{
					strcpy(EffectDF,"");
					strcpy(EffectDFEF,"");
					return ret;
				}
				strcpy(EffectDF,DDF);
				strcpy(EffectDFEF,data_DFEF);
			}
			if (strcmp(EEF,"EF05")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x81:ret = iReadRecord("EF05",recno,0x08,AN_style,result);break;
					case 0x84:ret = iReadRecord("EF05",recno,0x03,CN_style,result);break;
					case 0x87:ret = iReadRecord("EF05",recno,0x11,AN_style,result);break;
					case 0x8C:ret = iReadRecord("EF05",recno,0x01,CN_style,result);break;
					case 0x80:ret = iReadRecord("EF05",recno,0x04,CN_style,result);break;
					case 0x8B:ret = iReadRecord("EF05",recno,0x1D,AN_style,result);break;
					case 0x8A:ret = iReadRecord("EF05",recno,0x0F,AN_style,result);break;
					case 0x83:ret = iReadRecord("EF05",recno,0x09,AN_style,result);break;
					case 0x86:ret = iReadRecord("EF05",recno,0x09,AN_style,result);break;
					case 0x89:ret = iReadRecord("EF05",recno,0x09,AN_style,result);break;
					case 0x7D:ret = iReadRecord("EF05",recno,0x09,AN_style,result);break;
					case 0x7E:ret = iReadRecord("EF05",recno,0x09,AN_style,result);break;
					case 0x7F:ret = iReadRecord("EF05",recno,0x09,AN_style,result);break;
					case 0x8D:ret = iReadRecord("EF05",recno,0x09,AN_style,result);break;
					case 0x8E:ret = iReadRecord("EF05",recno,0x09,AN_style,result);break;
					case 0x8F:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;	
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				unsigned char wordkey[4]={0x67,0x2D,0x84};
				char err[20]={0};
				if(ExterAuthen(wordkey,1,err))
				{
					strcpy(pErrMsg,"RK1DF04认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x90:ret = iReadRecord("EF06",recno,0x01,AN_style,result);break;
					case 0x92:{moneyFlag = true;ret = iReadRecord("EF06",recno,0x04,CN_style,result);}break;
					case 0x93:ret = iReadRecord("EF06",recno,0x02,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF08")==0)//循环文件
			{	
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char a[13]={0};
				char result[100]={0};
				memcpy(a,data_item,strlen(data_item));
				if(strlen(a)!=2)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(a))
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				int recno = chartoint1(a[0])*16+chartoint1(a[1]);
				if(recno>30)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				CYC cy[7]={{0x02,CN_style},{0x01,AN_style},{0x06,CN_style},{0x07,CN_style},{0x04,CN_style},{0x04,CN_style},{0x04,CN_style}};
				ret = iReadXHFile("EF08",recno,0x1C,cy,7,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
				strcat(data,"|");
				
			}//EF08结束符
			else if (strcmp(EEF,"EF15")==0)//循环文件
			{	
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char a[13]={0};
				char result[100]={0};
				memcpy(a,data_item,strlen(data_item));
				if(strlen(a)!=2)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(a))
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				int recno = chartoint1(a[0])*16+chartoint1(a[1]);
				if(recno>8)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				CYC cy[2]={{0x02,CN_style},{0x01,AN_style}};
				ret = iReadXHFile("EF15",recno,0x03,cy,2,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
				strcat(data,"|");
			}//EF15结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF04结束
		else if(strcmp(DDF,"DF05")==0)
		{
			if(strlen(pInputPin)!=6)
			{
				strcpy(pErrMsg,"输入PIN长度错误");
				return SB_ERR_OPNUMERROR;
			}
			if(!IsDec((char*)pInputPin))
			{
				strcpy(pErrMsg,"输入PIN有非法字符");
				return SB_ERR_OPNUMERROR;
			}
			if(strcmp(EffectDF,DDF)!=0)//如果是从别的DF转过来
			{
				if(SelectFile("DF05"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					return SB_ERR_UNSUPPORTED; 
				}
				ret = VerifyPIN(pInputPin,pErrMsg);
				if(ret!=0)
				{
					strcpy(EffectDF,"");
					strcpy(EffectDFEF,"");
					return ret;
				}
				strcpy(EffectDF,DDF);
				strcpy(EffectDFEF,data_DFEF);
			}

			if (strcmp(EEF,"EF05")==0)
			{
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						switch(recno)
						{
						case 0xA0:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xA1:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xB9:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xA2:ret = iReadRecord("EF05",recno,0x04,AN_style,result);break;
						case 0xA9:ret = iReadRecord("EF05",recno,0x01,CN_style,result);break;

						case 0xBA:ret = iReadRecord("EF05",recno,0x14,AN_style,result);break;
						case 0xBB:ret = iReadRecord("EF05",recno,0x64,AN_style,result);break;
						case 0xAA:ret = iReadRecord("EF05",recno,0x14,AN_style,result);break;
						case 0xAB:ret = iReadRecord("EF05",recno,0x04,CN_style,result);break;
						case 0xAC:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;

						case 0xAD:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xAE:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xAF:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xB0:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xB1:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						
						case 0xB2:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xB3:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xB4:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xB5:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xB6:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xB7:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xBC:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
						case 0xB8:ret = iReadRecord("EF05",recno,0x28,AN_style,result);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,result);
							return ret;
						}
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"A3")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[2]={{"A5",0x10,AN_style},{"A6",0x04,AN_style}};
							ret = iReadBTLV("EF05",recno+5,0x18,tlv,2,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else if(strcmp(a,"A4")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[2]={{"A7",0x10,AN_style},{"A8",0x03,AN_style}};
							ret = iReadBTLV("EF05",recno+10,0x17,tlv,2,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}//循环结束
				addr_item =0;
			}
			else 
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}
		else if(strcmp(DDF,"DF06")==0)
		{
			if(strcmp(EffectDF,DDF)!=0)//如果是从别的DF转过来
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x52,0x83};

				if(SelectFile("DF06"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"RK1DF06认证失败!");
					strcat(pErrMsg,err);
					strcpy(EffectDF,"");
					strcpy(EffectDFEF,"");
					return SB_ERR_AUTH;
				}
				strcpy(EffectDF,DDF);
				strcpy(EffectDFEF,data_DFEF);
			}
			if (strcmp(EEF,"EF05")==0)
			{
			//	int file=0;
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						switch(recno)
						{
						case 0xC0:ret = iReadRecord("EF05",recno,0x46,AN_style,result);break;
						case 0xC2:ret = iReadRecord("EF05",recno,0x09,AN_style,result);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,result);
							return ret;
						}
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						char fileflag[3]={0};
						strcpy(fileflag,a);
						if(strcmp(a,"C3")==0 || strcmp(a,"C4")==0 ||strcmp(a,"C5")==0  )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);
							moneyFlag = true;
							TLV tlv[5]={{"E3",0x01,CN_style},{"E4",0x04,CN_style},{"E5",0x04,CN_style},{"E6",0x04,CN_style},{"E7",0x03,CN_style}};
							ret = iReadBTLV("EF05",fileflag[1]-0x30,0x1A,tlv,5,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;

				}//循环结束
				addr_item =0;
			}
			else if(strcmp(EEF,"EF06")==0)
			{
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
			/*	char err[20]={0};
				unsigned char wordkey[4]={0x48,0x52,0x83};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"RK1DF06认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}*/
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0xC6:ret = iReadRecord("EF06",recno,0x46,AN_style,result);break;
					case 0xC8:ret = iReadRecord("EF06",recno,0x09,AN_style,result);break;
					case 0xC9:ret = iReadRecord("EF06",recno,0x01,CN_style,result);break;
					case 0xCA:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0xCB:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;

					case 0xCC:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0xCD:ret = iReadRecord("EF06",recno,0x02,CN_style,result);break;
					case 0xCE:ret = iReadRecord("EF06",recno,0x01,CN_style,result);break;
					case 0xCF:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0xD0:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;

					case 0xD1:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0xD2:ret = iReadRecord("EF06",recno,0x03,CN_style,result);break;
					case 0xD3:ret = iReadRecord("EF06",recno,0x01,CN_style,result);break;
					case 0xD4:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0xD5:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0xD6:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;

					case 0xD7:ret = iReadRecord("EF06",recno,0x03,CN_style,result);break;
					case 0xD8:ret = iReadRecord("EF06",recno,0x01,CN_style,result);break;
					case 0xD9:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0xDA:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0xDB:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0xDC:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0xDD:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0xDE:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0xDF:ret = iReadRecord("EF06",recno,0x03,CN_style,result);break;
					case 0xE0:ret = iReadRecord("EF06",recno,0x03,CN_style,result);break;
					case 0xE1:ret = iReadRecord("EF06",recno,0x02,CN_style,result);break;
					case 0xE2:ret = iReadRecord("EF06",recno,0x02,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;	
			}
			else 
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}
		else if(strcmp(DDF,"DF07")==0)
		{
			if(strlen(pInputPin)!=6)
			{
				strcpy(pErrMsg,"输入PIN长度错误");
				return SB_ERR_OPNUMERROR;
			}
			if(!IsDec((char*)pInputPin))
			{
				strcpy(pErrMsg,"输入PIN有非法字符");
				return SB_ERR_OPNUMERROR;
			}
			if(strcmp(EffectDF,DDF)!=0)//如果是从别的DF转过来
			{
				if(SelectFile("DF07"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					return SB_ERR_UNSUPPORTED; 
				}
				ret = VerifyPIN(pInputPin,pErrMsg);
				if(ret!=0)
				{
					strcpy(EffectDF,"");
					strcpy(EffectDFEF,"");
					return ret;
				}
				strcpy(EffectDF,DDF);
				strcpy(EffectDFEF,data_DFEF);
			}

			if (strcmp(EEF,"EF05")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x3D,0x83};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"RK1DF07认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0xF0:ret = iReadRecord("EF05",recno,0x02,AN_style,result);break;
					case 0xF1:ret = iReadRecord("EF05",recno,0x01,AN_style,result);break;
					case 0xF2:ret = iReadRecord("EF05",recno,0x04,CN_style,result);break;
					case 0xF3:ret = iReadRecord("EF05",recno,0x03,AN_style,result);break;
					case 0xF4:ret = iReadRecord("EF05",recno,0x04,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;	
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x3F,0x84};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"RK2DF04认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0xF5:ret = iReadRecord("EF06",recno,0x03,AN_style,result);break;
					case 0xF6:ret = iReadRecord("EF06",recno,0x04,CN_style,result);break;
					case 0xF7:ret = iReadRecord("EF06",recno,0x46,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0xB1,0x85};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"RK3DF04认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0xF8:ret = iReadRecord("EF07",recno,0x04,CN_style,result);break;
					case 0xF9:ret = iReadRecord("EF07",recno,0x01,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
				
			}//EF07结束符	
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF07结束
		else 
		{
			strcpy(pErrMsg,"输入DF字符串错误");
			return SB_ERR_UNSUPPORTED; 
		}
	}//$循环结束符
	fileType =0;
	strcpy(pOutDataBuff,data);
	return SB_ERR_OK;
}

int iReadCardX(const char* pInputPin, const char* pFileAddr, char* pOutDataBuff , char*pErrMsg)
{
	if(g_handle>0)
	{
		strcpy(pErrMsg,"读写器已打开，请选关闭读写器");
		return SB_ERR_CONNECT;
	}
	int pReaderHandle=0;
	int re = iOpenPort( pErrMsg);
	if(re!=0)
		return re;
	if(GetAtrCityCode(pErrMsg)!=0)
	{
		return SB_ERR_CARDTYPE;
	}
	strcpy(EffectDF,"");
	strcpy(EffectDFEF,"");
	strcpy(EffectDF1,"");
	strcpy(EffectDFEF1,"");
	re = iReadCard( pInputPin, pFileAddr, pOutDataBuff);
	if(re!=0)
	{
		iClosePort(pErrMsg);
		return re;
	}
	re = iClosePort(pErrMsg);
	if(re!=0)
		return re;
	return SB_ERR_OK;
}

int WINAPI iWriteCard(const char* pInputPin, const char* pFileAddr, char* pWriteDataBuff,char* pErrMsg)
{
	int re=GetAtrCityCode(pErrMsg);

	strcpy(EffectDF,"");
	strcpy(EffectDFEF,"");
	
	if(pReaderHandle<=0)
	{
		strcpy(pErrMsg,"读写器未连接");
		return SB_ERR_UNCONNECT;
	}
	if(g_handle!=pReaderHandle)
	{
		strcpy(pErrMsg,"句柄值无效");
		return SB_ERR_CONNECT;
	}
	if(strlen(pInputPin)!=6)
	{
		strcpy(pErrMsg,"输入PIN长度错误");
		return SB_ERR_OPNUMERROR;
	}
	int filelen = strlen(pFileAddr);
	if(filelen<8)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED+10;
	}
	int ad = findChar((char*)pFileAddr,0,':');
	int addr = findChar((char*)pFileAddr,0,'|');//先对字符串格式进行判断
	if(addr<0 && ad<0)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED+20;
	}
	int count = getCount((char*)pFileAddr,'$');//获取需要循环的次数
	if(count<0)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED+30;
	}
	if(getCount((char*)pFileAddr,' ')>0 || getCount((char*)pWriteDataBuff,' ')>0 )
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED+40;
	}

	char temp[556]={0};
//	char tempw[256]={0};

	char data_item[0x100]={0};//存放数据项
	char data_DFEF[100]={0};//存放DF和EF
	char data_w[9000]={0};  //存放要写入的数据
	strcpy(data_w,pWriteDataBuff);


	char DDF[10]={0};
	char EEF[10]={0};

	int addr_data=0;//记录$的位置 EF的分隔符
	int addr_item =0;//记录|的位置  记录的分隔符  

//	int addr_dataw=0;//记录$的位置 EF的分隔符
	int addr_itemw =0;//记录|的位置  记录的分隔符 
	
	addr =0;
	int addr1=0,addr2=0;
	int addr3=0,addr4=0;
	int ret;

	unsigned char Tag[50]={0};
	unsigned char datalength[50]={0};
	unsigned char style[50]={0};

	int ct = getCount((char*)pFileAddr,'|');//获取|数量
	int ct1 = getCount((char*)pFileAddr,'$');//获取|数量

	int ct2 = getCount((char*)pWriteDataBuff,'|');//获取|数量

	if(ct2!=(ct-ct1))
	{
		if(ct==0 && ct1==ct2)
		{
		}
		else if(strcmp(pFileAddr,"MFEF08|00|$")==0 || strcmp(pFileAddr,"MFEF07|00|$")==0 || strcmp(pFileAddr,"DF01EF16|00|$")==0 ||strcmp(pFileAddr,"DF01EF17|00|$")==0 || strcmp(pFileAddr,"DF01EF18|00|$")==0 || strcmp(pFileAddr,"DF01EF19|00|$")==0 ||strcmp(pFileAddr,"DF01EF1A|00|$")==0)
		{}
		else
		{
			strcpy(pErrMsg,"输入字符串非法");
			return SB_ERR_UNSUPPORTED+50; 
		}
	}
/*
	if(GetAtrCityCode(pErrMsg)!=0)
	{
		return SB_ERR_TYPEERROR;
	}*/
	//MFEF05|07|$MFEF06|08|09|0A|$
	for(int i=0;i<count;i++)//DF范围内循环
	{
		addr1 = findChar((char*)pFileAddr,addr_data,'$');// pFileAddr = MFEF05|07|$MFEF06|08|09|0A|$
		memset(temp,0,100);
		memcpy(temp,pFileAddr+addr_data,addr1-addr_data);//获取DF和EF以及数据项  temp = MFEF05|07|08|
		addr_data=addr1+1;
	
		ad = findChar((char*)temp,0,':');
		addr = findChar((char*)temp,0,'|');//先对字符串格式进行判断

		int len,iCount,iCountw;
	//	iCount = getCount((char*)data_item,'|');//获取|数量
		if(ad>0 && addr>0 && addr<ad) //B-TLV格式
		{
			addr2 = findChar((char*)temp,0,'|');
			memset(data_DFEF,0,0x100);
			memcpy(data_DFEF,temp,addr2);//获取DF和EF  data_DFEF = DF02EF07

			len = strlen(data_DFEF);
			memset(data_item,0,0x300);
			memcpy(data_item,temp+addr2+1,addr1-addr2-1);// data_item = 99:01|99:02|
			if(strlen(data_item)<6)
			{
				fileType =0;
				strcpy(pErrMsg,"输入字符串非法");
				return SB_ERR_UNSUPPORTED+60; 
			}
			iCount = getCount((char*)data_item,'|');//获取|数量
			fileType =2;
		}
		else if(ad>0 && addr<0)//循环文件 DF02EF07:01$DF02EF07:02$DF02EF07:03$DF02EF07:04$
		{
			addr2 = findChar((char*)temp,0,':');
			memset(data_DFEF,0,0x100);
			memcpy(data_DFEF,temp,addr2);//获取DF和EF  data_DFEF = DF02EF07

			len = strlen(data_DFEF);
			memset(data_item,0,0x300);
			memcpy(data_item,temp+addr2+1,addr1-addr2-1);// data_item = 01|
			fileType = 3;
		}
		else if(ad<0 && addr>0)
		{
			addr2 = findChar((char*)temp,0,'|');
			memset(data_DFEF,0,0x100);
			memcpy(data_DFEF,temp,addr2);//获取DF和EF  data_DFEF = MFEF05
		//	strcat(data,data_DFEF);
		//	strcat(data,"|");
			len = strlen(data_DFEF);
			memset(data_item,0,0x300);
			memcpy(data_item,temp+addr2+1,addr1-addr2-1);// data_item = 07|08|
			iCount = getCount((char*)data_item,'|');//获取|数量
			iCountw = getCount((char*)data_w,'|');//获取|数量
			fileType =1;
		}
		else
		{
			strcpy(pErrMsg,"输入字符串非法");
			return SB_ERR_UNSUPPORTED+70; 
		}
		if(len==6)//说明是MF
		{
			memset(DDF,0,10);
			memcpy(DDF,data_DFEF,2);
			memcpy(EEF,data_DFEF+2,4);
		}
		else if(len==8)
		{
			memcpy(DDF,data_DFEF,4);
			memcpy(EEF,data_DFEF+4,4);
		}
		else
		{
			strcpy(pErrMsg,"输入字符串格式错误");
			return SB_ERR_UNSUPPORTED+80;
		}

		if (strcmp(DDF,"MF")==0 || strcmp(DDF,"DDF1")==0)
		{
			if(strcmp(EffectDF1,DDF)!=0)
			{
				if(SelectFile("DDF1"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					return SB_ERR_UNSUPPORTED+90; 
				}
				strcpy(EffectDF1,DDF);
			}
			if (strcmp(EEF,"EF05")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x12,0x04};

				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UKSSSE认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
	
				memcpy(Tag,"\x01\x02\x03\x04\x05\x06\x07",7);
				memcpy(datalength,"\x10\x01\x04\x0C\x04\x04\x09",7);
				memcpy(style,"\x0F\x01\x01\x0F\x0F\x0F\x01",7);
				
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
	
					switch(recno)
					{
					case 0x01: ret = iWriteRecordFile("EF05",Tag,0,datalength,style,data);break;
					case 0x02: ret = iWriteRecordFile("EF05",Tag,1,datalength,style,data);break;
					case 0x03: ret = iWriteRecordFile("EF05",Tag,2,datalength,style,data);break;
					case 0x04: ret = iWriteRecordFile("EF05",Tag,3,datalength,style,data);break;
					case 0x05: ret = iWriteRecordFile("EF05",Tag,4,datalength,style,data);break;
					case 0x06: ret = iWriteRecordFile("EF05",Tag,5,datalength,style,data);break;
					case 0x07: ret = iWriteRecordFile("EF05",Tag,6,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x12,0x04};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UKSSSE认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x08\x09\x4E\x0A\x0B\x0C\x0D",8);
				memcpy(datalength,"\x00\x12\x1E\x14\x01\x01\x03\x04",8);
				memcpy(style,"\x00\x01\x01\x01\x01\x0F\x0F\x0F",8);
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x08: ret = iWriteRecordFile("EF06",Tag,1,datalength,style,data);break;
					case 0x09: ret = iWriteRecordFile("EF06",Tag,2,datalength,style,data);break;
					case 0x4E: ret = iWriteRecordFile("EF06",Tag,3,datalength,style,data);break;
					case 0x0A: ret = iWriteRecordFile("EF06",Tag,4,datalength,style,data);break;
					case 0x0B: ret = iWriteRecordFile("EF06",Tag,5,datalength,style,data);break;
					case 0x0C: ret = iWriteRecordFile("EF06",Tag,6,datalength,style,data);break;
					case 0x0D: ret = iWriteRecordFile("EF06",Tag,7,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x12,0x04};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UKSSSE认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				int len = strlen(pWriteDataBuff);
				if(len%2!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(pWriteDataBuff))
				{
					strcpy(pErrMsg,"输入的操作数非法");
					return SB_ERR_OPNUMERROR; 
				}
				if(len==0)
				{
					len = 2*0x300;
				}
				ret = iWriteBinary("EF07",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}
			else if (strcmp(EEF,"EF08")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x12,0x04};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UKSSSE认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				int len = strlen(pWriteDataBuff);
				if(len%2!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(pWriteDataBuff))
				{
					strcpy(pErrMsg,"输入的操作数非法");
					return SB_ERR_OPNUMERROR; 
				}
				if(len==0)
				{
					len = 2*0x300;
				}
				ret = iWriteBinary("EF08",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}
			else if (strcmp(EEF,"EF0D")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x13,0x0D};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UKSSSE认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\xEA\xEB\xEC\xED",5);
				memcpy(datalength,"\x00\x0C\x03\x01\x01",5);
				memcpy(style,"\x00\x01\x0F\x01\x01",5);
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0xEA: ret = iWriteRecordFile("EF0D",Tag,1,datalength,style,data);break;
					case 0xEB: ret = iWriteRecordFile("EF0D",Tag,2,datalength,style,data);break;
					case 0xEC: ret = iWriteRecordFile("EF0D",Tag,3,datalength,style,data);break;
					case 0xED: ret = iWriteRecordFile("EF0D",Tag,4,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF0D结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}

		}//DDF结束符
		else if(strcmp(DDF,"DF01")==0)
		{
			if(strcmp(EffectDF1,DDF)!=0)
			{
				if(SelectFile("DF01"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					return SB_ERR_UNSUPPORTED; 
				}
				strcpy(EffectDF1,DDF);
			}
			if (strcmp(EEF,"EF05")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x42,0x83};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK1DF01认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x20\x21\x0E",4);
				memcpy(datalength,"\x00\x02\x50\x03",4);
				memcpy(style,"\x00\x01\x01\x0F",4);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x20: ret = iWriteRecordFile("EF05",Tag,1,datalength,style,data);break;
					case 0x21: ret = iWriteRecordFile("EF05",Tag,2,datalength,style,data);break;
					case 0x0E: ret = iWriteRecordFile("EF05",Tag,3,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;	
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x25,0x86};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK4DF01认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x23\x24\x28\x2C\x2D",6);
				memcpy(datalength,"\x00\x50\x03\x0F\x32\x0F",6);
				memcpy(style,"\x00\x01\x0F\x01\x01\x01",6);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x23: ret = iWriteRecordFile("EF06",Tag,1,datalength,style,data);break;
					case 0x24: ret = iWriteRecordFile("EF06",Tag,2,datalength,style,data);break;
					case 0x28: ret = iWriteRecordFile("EF06",Tag,3,datalength,style,data);break;
					case 0x2C: ret = iWriteRecordFile("EF06",Tag,4,datalength,style,data);break;
					case 0x2D: ret = iWriteRecordFile("EF06",Tag,5,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x43,0x84};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK2DF01认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x29",2);
				memcpy(datalength,"\x00\x01",2);
				memcpy(style,"\x00\x01",2);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x29: ret = iWriteRecordFile("EF07",Tag,1,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF07结束符
			else if (strcmp(EEF,"EF08")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x24,0x85};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK3DF01认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x2B",2);
				memcpy(datalength,"\x00\x01",2);
				memcpy(style,"\x00\x01",2);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x2B: ret = iWriteRecordFile("EF08",Tag,1,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF08结束符
			else if (strcmp(EEF,"EF09")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x43,0x84};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK2DF01认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x2E\x30\x32",4);
				memcpy(datalength,"\x00\x46\x09\x02",4);
				memcpy(style,"\x00\x01\x01\x01",4);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x2E: ret = iWriteRecordFile("EF09",Tag,1,datalength,style,data);break;
					case 0x30: ret = iWriteRecordFile("EF09",Tag,2,datalength,style,data);break;
					case 0x32: ret = iWriteRecordFile("EF09",Tag,3,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF09结束符
			else if (strcmp(EEF,"EF0A")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x44,0x87};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK5DF01认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x37\x38\x39",4);
				memcpy(datalength,"\x00\x03\x02\x04",4);
				memcpy(style,"\x00\x01\x01\x0F",4);


				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x37: ret = iWriteRecordFile("EF0A",Tag,1,datalength,style,data);break;
					case 0x38: ret = iWriteRecordFile("EF0A",Tag,2,datalength,style,data);break;
					case 0x39: ret = iWriteRecordFile("EF0A",Tag,3,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF0A结束符
			else if (strcmp(EEF,"EF15")==0)//BTLV
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x2F,0x89};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK6DF01认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				TLV tlv[3]={{"57",0x03,AN_style},{"58",0x03,CN_style},{"59",0x28,AN_style}};
				memcpy(Tag,"\x00\x2A",2);
				memcpy(datalength,"\x00\x01",2);
				memcpy(style,"\x00\x0F",2);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						int addw = findChar((char*)data_w,addr_itemw,'|');
						memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
						
						switch(recno)
						{
						case 0x2A: ret = iWriteRecordFile("EF15",Tag,1,datalength,style,data);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,data);
							return ret;
						}
						addr_item=add+1;
						addr_itemw=addw+1;
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"40")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							if(getCount(data,',')!=3)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED; 
							}
							ret = iWriteBTLV("EF15",recno+1,0x34,tlv,3,data,0x40);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;

						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}

				}//循环结束
				addr_item =0;
	
			}//EF15结束符
			else if (strcmp(EEF,"EF16")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x30,0x8A};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK7DF01认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				int len = strlen(pWriteDataBuff);
				if(len%2!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(pWriteDataBuff))
				{
					strcpy(pErrMsg,"输入的操作数非法");
					return SB_ERR_OPNUMERROR; 
				}
				if(len==0)
				{
					len = 2*0x128;
				}
				ret = iWriteBinary("EF16",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}//EF16结束
			else if (strcmp(EEF,"EF17")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x31,0x8B};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK8DF01认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				int len = strlen(pWriteDataBuff);
				if(len%2!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(pWriteDataBuff))
				{
					strcpy(pErrMsg,"输入的操作数非法");
					return SB_ERR_OPNUMERROR; 
				}
				if(len==0)
				{
					len = 2*0x128;
				}
				ret = iWriteBinary("EF17",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}
			else if (strcmp(EEF,"EF18")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x32,0x8C};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK9DF01认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				int len = strlen(pWriteDataBuff);
				if(len%2!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(pWriteDataBuff))
				{
					strcpy(pErrMsg,"输入的操作数非法");
					return SB_ERR_OPNUMERROR; 
				}
				if(len==0)
				{
					len = 2*0x128;
				}
				ret = iWriteBinary("EF18",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}
			else if (strcmp(EEF,"EF19")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x33,0x8D};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UKADF01认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				int len = strlen(pWriteDataBuff);
				if(len%2!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(pWriteDataBuff))
				{
					strcpy(pErrMsg,"输入的操作数非法");
					return SB_ERR_OPNUMERROR; 
				}
				if(len==0)
				{
					len = 2*0x128;
				}
				ret = iWriteBinary("EF19",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}
			else if (strcmp(EEF,"EF1A")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x34,0x8E};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UKBDF01认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				int len = strlen(pWriteDataBuff);
				if(len%2!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(pWriteDataBuff))
				{
					strcpy(pErrMsg,"输入的操作数非法");
					return SB_ERR_OPNUMERROR; 
				}
				if(len==0)
				{
					len = 2*0x128;
				}
				ret = iWriteBinary("EF1A",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}

		}//DF01结束
		else if(strcmp(DDF,"DF02")==0)
		{
			if(strcmp(EffectDF1,DDF)!=0)
			{
				if(SelectFile("DF02"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					strcpy(EffectDF1,"");
					return SB_ERR_UNSUPPORTED; 
				}
				strcpy(EffectDF1,DDF);
			}
			if (strcmp(EEF,"EF05")==0)//BTLV
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x26,0x83};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK1DF02认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x42\x41",3);
				memcpy(datalength,"\x00\x03\x03",3);
				memcpy(style,"\x00\x01\x01",3);
				TLV tlv[5]={{"5A",0x07,AN_style},{"5B",0x01,AN_style},{"5C",0x46,AN_style},{"5D",0x04,CN_style},{"5E",0x10,AN_style}};
				TLV tlv1[4]={{"33",0x03,AN_style},{"34",0x46,AN_style},{"35",0x04,CN_style},{"36",0x10,AN_style}};
				
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						int addw = findChar((char*)data_w,addr_itemw,'|');
						memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
						
						switch(recno)
						{
						case 0x42: ret = iWriteRecordFile("EF05",Tag,1,datalength,style,data);break;
						case 0x41: ret = iWriteRecordFile("EF05",Tag,2,datalength,style,data);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,data);
							return ret;
						}
						addr_item=add+1;
						addr_itemw=addw+1;
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"43")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							if(getCount(data,',')!=5)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED; 
							}
							ret = iWriteBTLV("EF05",recno+2,0x6C,tlv,5,data,0x43);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;
						}
						else if(strcmp(a,"44")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							if(getCount(data,',')!=4)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED; 
							}
							ret = iWriteBTLV("EF05",recno+4,0x65,tlv1,4,data,0x44);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
				}//循环结束
				addr_item =0;	
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x45,0x84};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK2DF02认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x4C\x4B\x4D\x60\x4F\x50\x3A",8);
				memcpy(datalength,"\x00\x04\x05\x03\x04\x06\x03\x07",8);
				memcpy(style,"\x00\x0F\x01\x0F\x0F\x01\x0F\x01",8);


				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x4C: ret = iWriteRecordFile("EF06",Tag,1,datalength,style,data);break;
					case 0x4B: ret = iWriteRecordFile("EF06",Tag,2,datalength,style,data);break;
					case 0x4D: ret = iWriteRecordFile("EF06",Tag,3,datalength,style,data);break;
					case 0x60: ret = iWriteRecordFile("EF06",Tag,4,datalength,style,data);break;
					case 0x4F: ret = iWriteRecordFile("EF06",Tag,5,datalength,style,data);break;
					case 0x50: ret = iWriteRecordFile("EF06",Tag,6,datalength,style,data);break;
					case 0x3A: ret = iWriteRecordFile("EF06",Tag,7,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)//循环文件
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x46,0x85};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK3DF02认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
					char a[3]={0};
					char data[512]={0};
					char result[100]={0};
					memcpy(a,data_item,strlen(data_item));
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					if(!IsEffectChar(a))
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					if(recno!=0)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					if(getCount(data,',')!=4)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					CYC cy[4]={{0x07,AN_style},{0x04,CN_style},{0x04,CN_style},{0x46,AN_style}};
					ret = iWriteXHFile("EF07",recno,0x55,cy,4,data);
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
				addr_itemw=addw+1;
				addr_item=0;

			}//EF07结束符
			else if (strcmp(EEF,"EF09")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x27,0x86};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK4DF02认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x55\x56\x96\x97",5);
				memcpy(datalength,"\x00\x10\x46\x04\x03",5);
				memcpy(style,"\x00\x01\x01\x0F\x0F",5);


				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x55: ret = iWriteRecordFile("EF09",Tag,1,datalength,style,data);break;
					case 0x56: ret = iWriteRecordFile("EF09",Tag,2,datalength,style,data);break;
					case 0x96: ret = iWriteRecordFile("EF09",Tag,3,datalength,style,data);break;
					case 0x97: ret = iWriteRecordFile("EF09",Tag,4,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF09结束符
			else if (strcmp(EEF,"EF15")==0)//BTLV
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x28,0x87};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK5DF02认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				TLV tlv[3]={{"10",0x03,AN_style},{"11",0x04,CN_style},{"12",0x03,CN_style}};
				memcpy(Tag,"\x00\x00\x00\x00\x00\x0F\x1F",7);
				memcpy(datalength,"\x00\x00\x00\x00\x00\x04\x03",7);
				memcpy(style,"\x00\x00\x00\x00\x00\x0F\x0F",7);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						int addw = findChar((char*)data_w,addr_itemw,'|');
						memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
						switch(recno)
						{
						case 0x0F: ret = iWriteRecordFile("EF15",Tag,5,datalength,style,data);break;
						case 0x1F: ret = iWriteRecordFile("EF15",Tag,6,datalength,style,data);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,data);
							return ret;
						}
						addr_item=add+1;
						addr_itemw=addw+1;
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"99")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							if(getCount(data,',')!=3)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED; 
							}
							ret = iWriteBTLV("EF15",recno,0x10,tlv,3,data,0x99);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
				}//循环结束
				addr_item =0;

			}//EF15结束符
			else if (strcmp(EEF,"EF16")==0)//循环文件
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x36,0x89};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK6DF02认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
					char a[3]={0};
					char data[512]={0};
					char result[100]={0};
					memcpy(a,data_item,strlen(data_item));
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					if(!IsEffectChar(a))
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					if(recno!=0)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					if(getCount(data,',')!=5)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					CYC cy[5]={{0x03,AN_style},{0x04,CN_style},{0x04,CN_style},{0x04,CN_style},{0x03,CN_style}};
					ret = iWriteXHFile("EF16",recno,0x12,cy,5,data);
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
			//	addr_item=add+1;
				addr_itemw=addw+1;
				addr_item=0;
			}//EF16结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF02结束

		else if(strcmp(DDF,"DF03")==0)
		{
			if(strcmp(EffectDF1,DDF)!=0)
			{
				if(SelectFile("DF03"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					strcpy(EffectDF1,"");
					return SB_ERR_UNSUPPORTED; 
				}
				strcpy(EffectDF1,DDF);
			}
			if (strcmp(EEF,"EF05")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x47,0x86};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK1DF03认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x61\x98\x62\x63\x64\x65\x66",8);
				memcpy(datalength,"\x00\x03\x04\x04\x03\x03\x03\x03",8);
				memcpy(style,"\x00\x0F\x0F\x0F\x01\x01\x01\x01",8);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x61: ret = iWriteRecordFile("EF05",Tag,1,datalength,style,data);break;
					case 0x98: ret = iWriteRecordFile("EF05",Tag,2,datalength,style,data);break;
					case 0x62: ret = iWriteRecordFile("EF05",Tag,3,datalength,style,data);break;
					case 0x63: ret = iWriteRecordFile("EF05",Tag,4,datalength,style,data);break;
					case 0x64: ret = iWriteRecordFile("EF05",Tag,5,datalength,style,data);break;
					case 0x65: ret = iWriteRecordFile("EF05",Tag,6,datalength,style,data);break;
					case 0x66: ret = iWriteRecordFile("EF05",Tag,7,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)//BTLV
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x29,0x87};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK2DF03认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				TLV tlv[5]={{"13",0x02,AN_style},{"14",0x01,AN_style},{"15",0x14,AN_style},{"16",0x04,CN_style},{"17",0x3C,AN_style}};
				memcpy(Tag,"\x00\x45\x46\x47\x48\x49\x67\x6B",8);
				memcpy(datalength,"\x00\x14\x02\x02\x01\x01\x04\x3C",8);
				memcpy(style,"\x00\x01\x01\x01\x01\x01\x0F\x01",8);
				
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						int addw = findChar((char*)data_w,addr_itemw,'|');
						memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
						
						switch(recno)
						{
						case 0x45: ret = iWriteRecordFile("EF06",Tag,1,datalength,style,data);break;
						case 0x46: ret = iWriteRecordFile("EF06",Tag,2,datalength,style,data);break;
						case 0x47: ret = iWriteRecordFile("EF06",Tag,3,datalength,style,data);break;
						case 0x48: ret = iWriteRecordFile("EF06",Tag,4,datalength,style,data);break;
						case 0x49: ret = iWriteRecordFile("EF06",Tag,5,datalength,style,data);break;
						case 0x67: ret = iWriteRecordFile("EF06",Tag,6,datalength,style,data);break;
						case 0x6B: ret = iWriteRecordFile("EF06",Tag,7,datalength,style,data);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,data);
							return ret;
						}
						addr_item=add+1;
						addr_itemw=addw+1;
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"4A")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							if(getCount(data,',')!=5)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED; 
							}
							ret = iWriteBTLV("EF06",recno+7,0x61,tlv,5,data,0x4A);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;

						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}

				}//循环结束
				addr_item =0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x48,0x88};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK3DF03认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x70\x71\x6E\x6C\x6F\x73",7);
				memcpy(datalength,"\x00\x03\x03\x04\x04\x04\x04",7);
				memcpy(style,"\x00\x01\x0F\x0F\x0F\x0F\x0F",7);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x70: ret = iWriteRecordFile("EF07",Tag,1,datalength,style,data);break;
					case 0x71: ret = iWriteRecordFile("EF07",Tag,2,datalength,style,data);break;
					case 0x6E: ret = iWriteRecordFile("EF07",Tag,3,datalength,style,data);break;
					case 0x6C: ret = iWriteRecordFile("EF07",Tag,4,datalength,style,data);break;
					case 0x6F: ret = iWriteRecordFile("EF07",Tag,5,datalength,style,data);break;
					case 0x73: ret = iWriteRecordFile("EF07",Tag,6,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;

			}//EF07结束符
			else if (strcmp(EEF,"EF15")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x37,0x89};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK4DF03认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x7A\x7B\x7C",4);
				memcpy(datalength,"\x00\x03\x04\x04",4);
				memcpy(style,"\x00\x0F\x0F\x0F",4);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x7A: ret = iWriteRecordFile("EF15",Tag,1,datalength,style,data);break;
					case 0x7B: ret = iWriteRecordFile("EF15",Tag,2,datalength,style,data);break;
					case 0x7C: ret = iWriteRecordFile("EF15",Tag,3,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF15结束符
			else if (strcmp(EEF,"EF16")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x38,0x8A};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK5DF03认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}

				memcpy(Tag,"\x00\x51\x5F",3);
				memcpy(datalength,"\x00\x03\x01",3);
				memcpy(style,"\x00\x0F\x01",3);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					switch(recno)
					{
					case 0x51: ret = iWriteRecordFile("EF16",Tag,1,datalength,style,data);break;
					case 0x5F: ret = iWriteRecordFile("EF16",Tag,2,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF16结束符
			else if (strcmp(EEF,"EF17")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x39,0x8B};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK6DF03认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}

				memcpy(Tag,"\x00\x3B\x3C\x3D\x3E\x3F",6);
				memcpy(datalength,"\x00\x14\x46\x01\x04\x3C",6);
				memcpy(style,"\x00\x01\x01\x01\x0F\x01",6);


				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					switch(recno)
					{
					case 0x3B: ret = iWriteRecordFile("EF17",Tag,1,datalength,style,data);break;
					case 0x3C: ret = iWriteRecordFile("EF17",Tag,2,datalength,style,data);break;
					case 0x3D: ret = iWriteRecordFile("EF17",Tag,3,datalength,style,data);break;
					case 0x3E: ret = iWriteRecordFile("EF17",Tag,4,datalength,style,data);break;
					case 0x3F: ret = iWriteRecordFile("EF17",Tag,5,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF17结束符
			else if (strcmp(EEF,"EF18")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x3A,0x8C};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK7DF03认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x74\x75\x76\x77\x78",6);
				memcpy(datalength,"\x00\x03\x03\x01\x04\x04",6);
				memcpy(style,"\x00\x01\x0F\x0F\x0F\x0F",6);


				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x74: ret = iWriteRecordFile("EF18",Tag,1,datalength,style,data);break;
					case 0x75: ret = iWriteRecordFile("EF18",Tag,2,datalength,style,data);break;
					case 0x76: ret = iWriteRecordFile("EF18",Tag,3,datalength,style,data);break;
					case 0x77: ret = iWriteRecordFile("EF18",Tag,4,datalength,style,data);break;
					case 0x78: ret = iWriteRecordFile("EF18",Tag,5,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF18结束符
			else if (strcmp(EEF,"EF19")==0)//BTLV
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x3B,0x8D};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK8DF03认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}

				TLV tlv[7]={{"18",0x03,AN_style},{"19",0x03,CN_style},{"1A",0x04,CN_style},{"1B",0x04,CN_style},{"1C",0x04,CN_style},{"1D",0x03,AN_style},{"1E",0x04,CN_style}};
				moneyFlag = true;
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"79")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							if(getCount(data,',')!=7)
							{ 
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED; 
							}
							moneyFlag = true;
							ret = iWriteBTLV("EF19",recno,0x27,tlv,7,data,0x79);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}

				}//循环结束
				addr_item =0;
			}//EF19结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF03结束
		else if(strcmp(DDF,"DF04")==0)
		{
			if(strcmp(EffectDF1,DDF)!=0)
			{
				if(SelectFile("DF04"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					strcpy(EffectDF1,"");
					return SB_ERR_UNSUPPORTED; 
				}	
				strcpy(EffectDF1,DDF);
			}
			if (strcmp(EEF,"EF05")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x49,0x85};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK1DF04认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x81\x84\x87\x8C\x80\x8B\x8A\x83\x86\x89\x7D\x7E\x7F\x8D\x8E\x8F",17);
				memcpy(datalength,"\x00\x08\x03\x11\x01\x04\x1D\x0F\x09\x09\x09\x09\x09\x09\x09\x09\x01",17);
				memcpy(style,"\x00\x01\x0F\x01\x0F\x0F\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01",17);
				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x81: ret = iWriteRecordFile("EF05",Tag,1,datalength,style,data);break;
					case 0x84: ret = iWriteRecordFile("EF05",Tag,2,datalength,style,data);break;
					case 0x87: ret = iWriteRecordFile("EF05",Tag,3,datalength,style,data);break;
					case 0x8C: ret = iWriteRecordFile("EF05",Tag,4,datalength,style,data);break;
					case 0x80: ret = iWriteRecordFile("EF05",Tag,5,datalength,style,data);break;
					case 0x8B: ret = iWriteRecordFile("EF05",Tag,6,datalength,style,data);break;
					case 0x8A: ret = iWriteRecordFile("EF05",Tag,7,datalength,style,data);break;
					case 0x83: ret = iWriteRecordFile("EF05",Tag,8,datalength,style,data);break;
					case 0x86: ret = iWriteRecordFile("EF05",Tag,9,datalength,style,data);break;
					case 0x89: ret = iWriteRecordFile("EF05",Tag,10,datalength,style,data);break;
					case 0x7D: ret = iWriteRecordFile("EF05",Tag,11,datalength,style,data);break;
					case 0x7E: ret = iWriteRecordFile("EF05",Tag,12,datalength,style,data);break;
					case 0x7F: ret = iWriteRecordFile("EF05",Tag,13,datalength,style,data);break;
					case 0x8D: ret = iWriteRecordFile("EF05",Tag,14,datalength,style,data);break;
					case 0x8E: ret = iWriteRecordFile("EF05",Tag,15,datalength,style,data);break;
					case 0x8F: ret = iWriteRecordFile("EF05",Tag,16,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
				
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x2A,0x86};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK2DF04认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\x90\x92\x93",4);
				memcpy(datalength,"\x00\x01\x04\x02",4);
				memcpy(style,"\x00\x01\x0F\x01",4);


				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x90: ret = iWriteRecordFile("EF06",Tag,1,datalength,style,data);break;
					case 0x92:{ moneyFlag = true;ret = iWriteRecordFile("EF06",Tag,2,datalength,style,data);}break;
					case 0x93: ret = iWriteRecordFile("EF06",Tag,3,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF08")==0)//循环文件
			{	
				strcpy(pErrMsg,"该文件不允许写数据");
				return SB_ERR_UNSUPPORTED;
			}//EF08结束符
			else if (strcmp(EEF,"EF15")==0)//循环文件
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x2A,0x86};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK2DF04认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char a[3]={0};
				char data[512]={0};
				char result[100]={0};
				memcpy(a,data_item,strlen(data_item));
				if(strlen(a)!=2)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(a))
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				int recno = chartoint1(a[0])*16+chartoint1(a[1]);
				if(recno!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				int addw = findChar((char*)data_w,addr_itemw,'|');
				memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
				if(getCount(data,',')!=2)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				CYC cy[2]={{0x02,CN_style},{0x01,AN_style}};
				ret = iWriteXHFile("EF15",recno,0x03,cy,2,data);
				if(ret!=0)
				{
					strcpy(pErrMsg,data);
					return ret;
				}
				addr_itemw=addw+1;
				addr_item=0;
				
			}//EF15结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF04结束
		else if(strcmp(DDF,"DF05")==0)
		{
			if(strcmp(EffectDF1,DDF)!=0)
			{
				if(SelectFile("DF05"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					strcpy(EffectDF1,"");
					return SB_ERR_UNSUPPORTED; 
				}
				strcpy(EffectDF1,DDF);
			}
			
			if(strcmp(EEF,"EF05")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0xB4,0x83};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK1DF05认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}

				TLV tlv1[2]={{"A5",0x10,AN_style},{"A6",0x04,AN_style}};
				TLV tlv2[2]={{"A7",0x10,AN_style},{"A8",0x03,AN_style}};
				
				memcpy(Tag,"\x00\xA0\xA1\xB9\xA2\xA9\xBA\xBB\xAA\xAB\xAC\xAD\xAE\xAF\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7\xBC\xB8",24);
				memcpy(datalength,"\x00\x01\x01\x01\x04\x01\x14\x64\x14\x04\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x28",24);
				memcpy(style,"\x00\x01\x01\x01\x01\x0F\x01\x01\x01\x0F\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01",24);
				
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						int addw = findChar((char*)data_w,addr_itemw,'|');
						memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
						
						switch(recno)
						{
						case 0xA0: ret = iWriteRecordFile("EF05",Tag,1,datalength,style,data);break;
						case 0xA1: ret = iWriteRecordFile("EF05",Tag,2,datalength,style,data);break;
						case 0xB9: ret = iWriteRecordFile("EF05",Tag,3,datalength,style,data);break;
						case 0xA2: ret = iWriteRecordFile("EF05",Tag,4,datalength,style,data);break;
						case 0xA9: ret = iWriteRecordFile("EF05",Tag,5,datalength,style,data);break;
						case 0xBA: ret = iWriteRecordFile("EF05",Tag,6,datalength,style,data);break;
						case 0xBB: ret = iWriteRecordFile("EF05",Tag,7,datalength,style,data);break;
						case 0xAA: ret = iWriteRecordFile("EF05",Tag,8,datalength,style,data);break;
						case 0xAB: ret = iWriteRecordFile("EF05",Tag,9,datalength,style,data);break;
						case 0xAC: ret = iWriteRecordFile("EF05",Tag,10,datalength,style,data);break;
						case 0xAD: ret = iWriteRecordFile("EF05",Tag,11,datalength,style,data);break;
						case 0xAE: ret = iWriteRecordFile("EF05",Tag,12,datalength,style,data);break;
						case 0xAF: ret = iWriteRecordFile("EF05",Tag,13,datalength,style,data);break;
						case 0xB0: ret = iWriteRecordFile("EF05",Tag,14,datalength,style,data);break;
						case 0xB1: ret = iWriteRecordFile("EF05",Tag,15,datalength,style,data);break;		
						case 0xB2: ret = iWriteRecordFile("EF05",Tag,16,datalength,style,data);break;
						case 0xB3: ret = iWriteRecordFile("EF05",Tag,17,datalength,style,data);break;			
						case 0xB4: ret = iWriteRecordFile("EF05",Tag,18,datalength,style,data);break;
						case 0xB5: ret = iWriteRecordFile("EF05",Tag,19,datalength,style,data);break;	
						case 0xB6: ret = iWriteRecordFile("EF05",Tag,20,datalength,style,data);break;
						case 0xB7: ret = iWriteRecordFile("EF05",Tag,21,datalength,style,data);break;
						case 0xBC: ret = iWriteRecordFile("EF05",Tag,22,datalength,style,data);break;
						case 0xB8: ret = iWriteRecordFile("EF05",Tag,23,datalength,style,data);break;
						
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,data);
							return ret;
						}
						addr_item=add+1;
						addr_itemw=addw+1;
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"A3")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							if(getCount(data,',')!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED; 
							}
							ret = iWriteBTLV("EF05",recno+5,0x18,tlv1,2,data,0xA3);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;

						}
						else if(strcmp(a,"A4")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							if(getCount(data,',')!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED; 
							}
							ret = iWriteBTLV("EF05",recno+10,0x17,tlv2,2,data,0xA4);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入EF字符串错误");
						return SB_ERR_UNSUPPORTED; 
					}
				}//循环结束
				addr_item =0;
			}
		}//DF05
		else if(strcmp(DDF,"DF06")==0)
		{
			if(strcmp(EffectDF1,DDF)!=0)
			{
				if(SelectFile("DF06"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					strcpy(EffectDF1,"");
					return SB_ERR_UNSUPPORTED; 
				}
				strcpy(EffectDF1,DDF);
			}
			if (strcmp(EEF,"EF05")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x53,0x85};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK1DF06认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				TLV tlv[5]={{"E3",0x01,CN_style},{"E4",0x04,CN_style},{"E5",0x04,CN_style},{"E6",0x04,CN_style},{"E7",0x03,CN_style}};
				memcpy(Tag,"\x00\xC0\xC2",3);
				memcpy(datalength,"\x00\x46\x09",3);
				memcpy(style,"\x00\x01\x01",3);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};

					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						int addw = findChar((char*)data_w,addr_itemw,'|');
						memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据	
						switch(recno)
						{
						case 0xC0: ret = iWriteRecordFile("EF05",Tag,1,datalength,style,data);break;
						case 0xC2: ret = iWriteRecordFile("EF05",Tag,2,datalength,style,data);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,data);
							return ret;
						}
						addr_item=add+1;
						addr_itemw=addw+1;
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						char fileflag[3]={0};
						strcpy(fileflag,a);
						if(strcmp(a,"C3")==0 || strcmp(a,"C4")==0||strcmp(a,"C5")==0  )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							if(getCount(data,',')!=5)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return SB_ERR_UNSUPPORTED; 
							}
							ret = iWriteBTLV("EF05",fileflag[1]-0x30,0x1A,tlv,5,data,chartoint1(a[0])*16+chartoint1(a[1]));
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
				}//循环结束
				addr_item =0;
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x54,0x86};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK2DF06认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\xC6\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xDB\xDC\xDD\xDE\xDF\xE0\xE1\xE2",29);
				memcpy(datalength,"\x00\x46\x09\x01\x04\x04\x04\x02\x01\x04\x04\x04\x03\x01\x04\x04\x04\x03\x01\x04\x04\x04\x04\x04\x04\x03\x03\x02\x02",29);
				memcpy(style,"\x00\x01\x01\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F",29);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0xC6:ret = iWriteRecordFile("EF06",Tag,1,datalength,style,data);break;
					case 0xC8:ret = iWriteRecordFile("EF06",Tag,2,datalength,style,data);break;
					case 0xC9:ret = iWriteRecordFile("EF06",Tag,3,datalength,style,data);break;
					case 0xCA:ret = iWriteRecordFile("EF06",Tag,4,datalength,style,data);break;
					case 0xCB:ret = iWriteRecordFile("EF06",Tag,5,datalength,style,data);break;

					case 0xCC:ret = iWriteRecordFile("EF06",Tag,6,datalength,style,data);break;
					case 0xCD:ret = iWriteRecordFile("EF06",Tag,7,datalength,style,data);break;
					case 0xCE:ret = iWriteRecordFile("EF06",Tag,8,datalength,style,data);break;
					case 0xCF:ret = iWriteRecordFile("EF06",Tag,9,datalength,style,data);break;
					case 0xD0:ret = iWriteRecordFile("EF06",Tag,10,datalength,style,data);break;

					case 0xD1:ret = iWriteRecordFile("EF06",Tag,11,datalength,style,data);break;
					case 0xD2:ret = iWriteRecordFile("EF06",Tag,12,datalength,style,data);break;
					case 0xD3:ret = iWriteRecordFile("EF06",Tag,13,datalength,style,data);break;
					case 0xD4:ret = iWriteRecordFile("EF06",Tag,14,datalength,style,data);break;
					case 0xD5:ret = iWriteRecordFile("EF06",Tag,15,datalength,style,data);break;
					case 0xD6:ret = iWriteRecordFile("EF06",Tag,16,datalength,style,data);break;

					case 0xD7:ret = iWriteRecordFile("EF06",Tag,17,datalength,style,data);break;
					case 0xD8:ret = iWriteRecordFile("EF06",Tag,18,datalength,style,data);break;
					case 0xD9:ret = iWriteRecordFile("EF06",Tag,19,datalength,style,data);break;
					case 0xDA:ret = iWriteRecordFile("EF06",Tag,20,datalength,style,data);break;
					case 0xDB:ret = iWriteRecordFile("EF06",Tag,21,datalength,style,data);break;
					case 0xDC:ret = iWriteRecordFile("EF06",Tag,22,datalength,style,data);break;
					case 0xDD:ret = iWriteRecordFile("EF06",Tag,23,datalength,style,data);break;
					case 0xDE:ret = iWriteRecordFile("EF06",Tag,24,datalength,style,data);break;
					case 0xDF:ret = iWriteRecordFile("EF06",Tag,25,datalength,style,data);break;
					case 0xE0:ret = iWriteRecordFile("EF06",Tag,26,datalength,style,data);break;
					case 0xE1:ret = iWriteRecordFile("EF06",Tag,27,datalength,style,data);break;
					case 0xE2:ret = iWriteRecordFile("EF06",Tag,28,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}
			else 
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF06结束
		else if(strcmp(DDF,"DF07")==0)
		{
			if(strcmp(EffectDF1,DDF)!=0)
			{
				if(SelectFile("DF07"))
				{
					strcpy(pErrMsg,"输入DF字符串错误");
					strcpy(EffectDF1,"");
					return SB_ERR_UNSUPPORTED; 
				}	
				strcpy(EffectDF1,DDF);
			}
			if (strcmp(EEF,"EF05")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x3E,0x86};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK1DF07认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\xF0\xF1\xF2\xF3\xF4",6);
				memcpy(datalength,"\x00\x02\x01\x04\x03\x04",6);
				memcpy(style,"\x00\x01\x01\x0F\x01\x0F",6);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0xF0: ret = iWriteRecordFile("EF05",Tag,1,datalength,style,data);break;
					case 0xF1: ret = iWriteRecordFile("EF05",Tag,2,datalength,style,data);break;
					case 0xF2: ret = iWriteRecordFile("EF05",Tag,3,datalength,style,data);break;
					case 0xF3: ret = iWriteRecordFile("EF05",Tag,4,datalength,style,data);break;
					case 0xF4: ret = iWriteRecordFile("EF05",Tag,5,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0xB0,0x87};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK2DF07认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\xF5\xF6\xF7",4);
				memcpy(datalength,"\x00\x03\x04\x46",4);
				memcpy(style,"\x00\x01\x0F\x01",4);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0xF5: ret = iWriteRecordFile("EF06",Tag,1,datalength,style,data);break;
					case 0xF6: ret = iWriteRecordFile("EF06",Tag,2,datalength,style,data);break;
					case 0xF7: ret = iWriteRecordFile("EF06",Tag,3,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)//循环文件
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0xB2,0x88};
				if(strcmp(EffectDFEF1,data_DFEF)!=0)//如果是从别的DFEF转过来
				{
					if(ExterAuthen(wordkey,2,err))
					{
						strcpy(pErrMsg,"UK3DF07认证失败!");
						strcat(pErrMsg,err);
						strcpy(EffectDFEF1,"");
						return SB_ERR_AUTH;
					}
					strcpy(EffectDFEF1,data_DFEF);
				}
				memcpy(Tag,"\x00\xF8\xF9",3);
				memcpy(datalength,"\x00\x04\x01",3);
				memcpy(style,"\x00\x0F\x01",3);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0xF8: ret = iWriteRecordFile("EF07",Tag,1,datalength,style,data);break;
					case 0xF9: ret = iWriteRecordFile("EF07",Tag,2,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
				
			}//EF07结束符	
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF07结束
		else 
		{
			strcpy(pErrMsg,"输入DF字符串错误");
			return SB_ERR_UNSUPPORTED; 
		}

	}//$循环结束符
	
	return SB_ERR_OK;
}

int iWriteCardX(const char* pInputPin, const char* pFileAddr, char* pWriteDataBuff,char* pErrMsg)
{
	if(g_handle>0)
	{
		strcpy(pErrMsg,"读写器已打开，请选关闭读写器");
		return SB_ERR_CONNECT;
	}
	int pReaderHandle=0;
	int re = iOpenPort(pErrMsg);
	if(re!=0)
		return re;
	if(GetAtrCityCode(pErrMsg)!=0)
	{
		return SB_ERR_CARDTYPE;
	}
	strcpy(EffectDF,"");
	strcpy(EffectDFEF,"");
	strcpy(EffectDF1,"");
	strcpy(EffectDFEF1,"");
	re = iWriteCard(pInputPin, pFileAddr, pWriteDataBuff,pErrMsg);
	if(re!=0)
	{
		iClosePort(pErrMsg);
		return re;
	}
	re = iClosePort(pErrMsg);
	if(re!=0)
		return re;
	return SB_ERR_OK;
}

bool numCheck(char *data,int len,char c)
{
	for(int i=0;i<len;i++)
	{
		if(c==0)
		{
			if(data[i]<0x30 || data[i]>0x39)
				return false;
		}
		else
		{
			if((data[i]<0x30  || data[i]>0x39) && data[i]!=c)
				return false;	
		}
	}
	return true;
}

int WINAPI iCardControl (const char* szHosSvrForSiIP,const char* szHosSvrForSiPort,const char * pCtrlType, const char * pCtrlInfo,char*  pErrMsg)
{
	if(pReaderHandle<=0)
	{
		strcpy(pErrMsg,"读写器未连接");
		return SB_ERR_UNCONNECT;
	}
	if(g_handle != pReaderHandle)
	{
		strcpy(pErrMsg,"句柄值无效");
		return SB_ERR_UNCONNECT;
	}
	char pin[100]={0};
//	strcpy(pin,pCtrlInfo);
	unsigned char Response[128]={0};

	int ret =0;
	unsigned char cmd[200]={0};
	memset(cmd,0,50);
	memset(Response,0,128);
	memcpy(cmd, "\x00\xA4\x04\x00\x0F\x73\x78\x31\x2E\x73\x68\x2E\xC9\xE7\xBB\xE1\xB1\xA3\xD5\xCF",20);
	//00A404000F7378312E73682EC9E7BBE1B1A3D5CF
	int re = ICC_Reader_Application(g_handle,USERCard,20,cmd,Response);//X选择MF文件
	if(re<=0)
	{
		strcpy(pErrMsg,"卡类型错误!");
		return SB_ERR_CARDTYPE;
	}
	if(Response[re-2]!=0x61 && Response[re-2]!=0x90 )
	{
		strcpy(pErrMsg,"卡类型错误!");
		return SB_ERR_CARDTYPE;
	}
	if(strcmp(pCtrlType,"01")==0)//更改密码
	{
//		char pinInfo[100]={0};
		if((pCtrlInfo[0]==0x00) || (pCtrlInfo==NULL))
		{
			ret = iReadPin (g_handle,pCtrlType,pin, pErrMsg);
			if(ret!=0)
				return ret;
		}
		else
			strcpy(pin,pCtrlInfo);
		if(!numCheck((char*)pin,13,'|'))
		{
			strcpy(pErrMsg,"输入的密码字符有未能识别!");
			return SB_ERR_UNSUPPORTED;
		}
		memset(cmd,0,50);
		memset(Response,0,128);
		memcpy(cmd,"\x80\x5E\x01\x00\x07",5);

		int datalen = strlen(pin);
		if(datalen!=13)
		{
			strcpy(pErrMsg,"输入的密码位数不对，或者没有输入密码!");
			return SB_ERR_UNSUPPORTED;
		}

		char oldPassword[10]={0};
		char newPassword[10]={0};
		for(int i=0;i<datalen;i++)
		{
			if(pin[i]=='|')
				break;
		}
		memcpy(oldPassword,pin,i);//i==6
		memcpy(newPassword,pin+i+1,datalen-i-1);
		int oldpin = strlen(oldPassword);
		if(oldpin>16 || oldpin<=0 || (oldpin%2)!=0)
		{
			strcpy(pErrMsg,"输入的旧密码位数不对，或者没有输入密码!");
			return SB_ERR_UNSUPPORTED;
		}
		int newpin = strlen(newPassword);
		if(newpin>16 || newpin<=0 || (newpin%2)!=0)
		{
			strcpy(pErrMsg,"输入的新密码位数不对，或者没有输入密码!");
			return SB_ERR_UNSUPPORTED;
		}

		cmd[4] = oldpin/2+newpin/2+1;
		for(i=0;i<oldpin/2;i++)
		{
			cmd[5+i]=chartoint1(oldPassword[2*i])*16+chartoint1(oldPassword[2*i+1]);
		}
		cmd[5+oldpin/2]=0xFF;
		for(i=0;i<newpin/2;i++)
		{
			cmd[5+i+oldpin/2+1]=chartoint1(newPassword[2*i])*16+chartoint1(newPassword[2*i+1]);
		}

		memset(Response,0,30);
		re = ICC_Reader_Application(g_handle,USERCard,6+oldpin/2+newpin/2,cmd,Response);//
		if(Response[0]!=0x90)
		{
			switch(Response[1])
			{
			case 0xC6:strcpy(pErrMsg,"修改失败，还剩6次认证机会!");return SB_ERR_AUTH;
			case 0xC5:strcpy(pErrMsg,"修改失败，还剩5次认证机会!");return SB_ERR_AUTH;
			case 0xC4:strcpy(pErrMsg,"修改失败，还剩4次认证机会!");return SB_ERR_AUTH;
			case 0xC3:strcpy(pErrMsg,"修改失败，还剩3次认证机会!");return SB_ERR_AUTH;
			case 0xC2:strcpy(pErrMsg,"修改失败，还剩2次认证机会!");return SB_ERR_AUTH;
			case 0xC1:strcpy(pErrMsg,"修改失败，还剩1次认证机会!");return SB_ERR_AUTH;
			case 0xC0:strcpy(pErrMsg,"修改失败，卡片被锁!");return SB_ERR_AUTH;
			}
			return SB_ERR_AUTH;
		}
	}
	else if(strcmp(pCtrlType,"02")==0)//重装
	{
		if((pCtrlInfo[0]==0x00) || (pCtrlInfo==NULL))
		{
			ret = iReadPin (g_handle,pCtrlType,pin, pErrMsg);
			if(ret!=0)
				return ret;
		}
		else
			strcpy(pin,pCtrlInfo);
		if(!numCheck((char*)pin,6,0))
		{
			strcpy(pErrMsg,"输入的密码字符有误!");
			return SB_ERR_UNSUPPORTED;
		}
		unsigned char resp[100]={0};
		unsigned char cmd[260]={0};
		unsigned char newPassword[10]={0};

		long re;
		memcpy(cmd, "\x00\xA4\x04\x00\x0F\x73\x78\x31\x2E\x73\x68\x2E\xC9\xE7\xBB\xE1\xB1\xA3\xD5\xCF",20);
		//00A404000F7378312E73682EC9E7BBE1B1A3D5CF
		re = ICC_Reader_Application(g_handle,USERCard,20,cmd,resp);//X选择MF文件
		int len = strlen(pin);
		if(len!=6)
		{
			strcpy(pErrMsg,"输入的密码位数不对，或者没有输入密码!");
			return SB_ERR_UNSUPPORTED;
		}

		for(int i=0;i<len;i++)
		{
			newPassword[i]=(pin[2*i]-0x30)*16+pin[2*i+1]-0x30;
		}

		memcpy(cmd, "\x00\xA4\x00\x00\x02\xEF\x05",7);
		re = ICC_Reader_Application(g_handle,USERCard,7,cmd,resp);
		memcpy(cmd, "\x00\xB2\x01\x04\x12",5);
		re = ICC_Reader_Application(g_handle,USERCard,5,cmd,resp);

		if(re<0 || resp[re-2]!=0x90)
		{
			strcpy(pErrMsg,"卡片为空卡");
			return SB_ERR_UNSUPPORTED;
		}
		char sdisp[3]={0};
		char saccm[40]={0};
		for(int n=0;n<re;n++)
		{
			sprintf(sdisp,"%02x",resp[n]);
			strcat(saccm,sdisp);
		}
		memcpy(_CityCode,saccm+4,32);
		 
		char err[20]={0};
		unsigned char wordkey[4]={0x48,0x10,0x06};
		if(ExterAuthen(wordkey,2,err))
		{
			strcpy(pErrMsg,"PUK认证失败!");
			strcat(pErrMsg,err);
			return SB_ERR_UNSUPPORTED;
		}
		GetRandom();
		memset(cmd,0,50);
		memcpy(cmd, "\x00\xA4\x00\x00\x02\xDF\x01",7);
		int ret = ICC_Reader_Application(g_handle,PSAM_Slot,7,cmd,resp);
		if(ret<2 || (resp[ret-2]!=0x90 && resp[ret-2]!=0x61))
		{
			strcpy(pErrMsg,"PSAM卡类型不对!");
			return SB_ERR_UNSUPPORTED;
		}
		memcpy(cmd, "\xBF\xDE\x48\x09\x18",5); //STK 
		cmd[5]=_ATR[0];
		cmd[6]=_ATR[1];
		cmd[7]=_ATR[2];
		cmd[8]=_ATR[3];
		cmd[9]=_ATR[4];
		cmd[10]=_ATR[5];
		cmd[11]=_ATR[6];
		cmd[12]=_ATR[7];

		cmd[13]=_CityCode[0];
		cmd[14]=_CityCode[1];
		cmd[15]=_CityCode[2];
		cmd[16]=_CityCode[3];
		cmd[17]=_CityCode[4];
		cmd[18]=_CityCode[5];
		cmd[19]=0x73;
		cmd[20]=0x78;
		cmd[21]=_Random[0];
		cmd[22]=_Random[1];
		cmd[23]=_Random[2];
		cmd[24]=_Random[3];
		cmd[25]=_Random[4];
		cmd[26]=_Random[5];
		cmd[27]=_Random[6];
		cmd[28]=_Random[7];
		ret = ICC_Reader_Application(g_handle,PSAM_Slot,29,cmd,resp);
		if(ret<2 || (resp[ret-2]!=0x90))
		{
			strcpy(pErrMsg,"PSAM卡类型不对!");
			return SB_ERR_UNSUPPORTED;
		}

		memset(cmd,0,50);
		memset(resp,0,30);
		memcpy(cmd, "\x80\xFA\x00\x00\x08",5);//对鉴别因子加密
		cmd[5]=3;
		for(i=0;i<3;i++)
		{
			cmd[6+i]=newPassword[i];
		}
		cmd[9]=0x80;
		for(i = 0;i<3;i++)
			cmd[10+i]=0x00;

		ret = ICC_Reader_Application(g_handle,PSAM_Slot,13,cmd,resp);
		if(ret<2 || (resp[ret-2]!=0x61))
		{
			strcpy(pErrMsg,"PSAM卡类型不对!");
			return SB_ERR_UNSUPPORTED;
		}
		memset(cmd,0,50);
		memset(resp,0,30);
		memcpy(cmd, "\x00\xC0\x00\x00\x08",5);
		ret = ICC_Reader_Application(g_handle,PSAM_Slot,5,cmd,resp);//得到密文
		if(ret<2 || (resp[ret-2]!=0x90))
		{
			strcpy(pErrMsg,"PSAM卡类型不对!");
			return SB_ERR_UNSUPPORTED;
		}
		unsigned char EncData[20]={0};
		memcpy(EncData,resp,8);
		
		memset(cmd,0,50);
		memset(resp,0,30);
		memcpy(cmd, "\xBF\xDE\x48\x09\x18",5); //一级
		cmd[5]=_ATR[0];
		cmd[6]=_ATR[1];
		cmd[7]=_ATR[2];
		cmd[8]=_ATR[3];
		cmd[9]=_ATR[4];
		cmd[10]=_ATR[5];
		cmd[11]=_ATR[6];
		cmd[12]=_ATR[7];

		cmd[13]=_CityCode[0];
		cmd[14]=_CityCode[1];
		cmd[15]=_CityCode[2];
		cmd[16]=_CityCode[3];
		cmd[17]=_CityCode[4];
		cmd[18]=_CityCode[5];
		cmd[19]=0x73;
		cmd[20]=0x78;
		cmd[21]=_Random[0];
		cmd[22]=_Random[1];
		cmd[23]=_Random[2];
		cmd[24]=_Random[3];
		cmd[25]=_Random[4];
		cmd[26]=_Random[5];
		cmd[27]=_Random[6];
		cmd[28]=_Random[7];
		ret = ICC_Reader_Application(g_handle,PSAM_Slot,29,cmd,resp);
		if(ret<2 || (resp[ret-2]!=0x90))
		{
			strcpy(pErrMsg,"PSAM卡类型不对!");
			return SB_ERR_UNSUPPORTED;
		}

		memset(cmd,0,50);
		memset(resp,0,30);
		memcpy(cmd, "\x80\xFA\x05\x00\x18",5);//计算MAC
		memcpy(cmd+5,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
		memcpy(cmd+13,"\x84\x24\x00\x01\x0C",5);
		memcpy(cmd+18,EncData,8);
		memcpy(cmd+26,"\x80\x00\x00",3);
		ret = ICC_Reader_Application(g_handle,PSAM_Slot,29,cmd,resp);
		if(ret<2 || (resp[ret-2]!=0x61))
		{
			strcpy(pErrMsg,"PSAM卡类型不对!");
			return SB_ERR_UNSUPPORTED;
		}
		memset(cmd,0,50);
		memset(resp,0,30);
		memcpy(cmd, "\x00\xc0\x00\x00\x08",5);
		ret = ICC_Reader_Application(g_handle,PSAM_Slot,5,cmd,resp);
		if(ret<2 || (resp[ret-2]!=0x90))
		{
			strcpy(pErrMsg,"PSAM卡类型不对!");
			return SB_ERR_UNSUPPORTED;
		}
		unsigned char MacData[10]={0};
		memcpy(MacData,resp,4);//得到MAC

		memset(cmd,0,50);
		memcpy(cmd,"\x84\x24\x00\x01\x0C",5);
		memcpy(cmd+5,EncData,8);
		memcpy(cmd+13,MacData,4);

		ret = ICC_Reader_Application(g_handle,USERCard,17,cmd,resp);
		if(resp[0]!=0x90 || ret<2)
		{
			strcpy(pErrMsg,"重置PIN码出错");
			return SB_ERR_UNSUPPORTED;
		}
	}
	else if(strcmp(pCtrlType,"03")==0)//校验
	{
		if((pCtrlInfo[0]==0x00) || (pCtrlInfo==NULL))
		{
			ret = iReadPin (g_handle,pCtrlType,pin, pErrMsg);
			if(ret!=0)
				return ret;
		}
		else
			strcpy(pin,pCtrlInfo);
		if(!numCheck((char*)pin,6,0))
		{
			strcpy(pErrMsg,"输入的密码字符有误!");
			return SB_ERR_UNSUPPORTED;
		}
		int len = strlen(pin);
		if(len!=6)
		{
			strcpy(pErrMsg,"输入的密码位数不对，或者没有输入密码!");
			return SB_ERR_UNSUPPORTED;
		}

		memset(cmd,0,50);
		memset(Response,0,128);
		memcpy(cmd, "\x00\x20\x00\x00\x06",5);
		cmd[4] = len/2;

		for(int i=0;i<len/2;i++)
		{
			cmd[5+i]=chartoint1(pin[2*i])*16+chartoint1(pin[2*i+1]);
		}

		re = ICC_Reader_Application(g_handle,USERCard,5+len/2,cmd,Response);//校验密钥
		if(re<=0 || Response[re-2]!=0x90)
		{
			if(Response[0]==0x63)
			{
				int i = Response[1]%0x10;
				char s[50]={0};
				sprintf(s,"密钥校验失败，还剩%d次认证次数!",i);
				strcpy(pErrMsg,s);
			}
			else
				strcpy(pErrMsg,"密钥校验失败!");
			return SB_ERR_AUTH;
		}
	}
	else
	{
		strcpy(pErrMsg,"输入的密钥操作类型非法!");
		return SB_ERR_OPNUMERROR;
	}

	return SB_ERR_OK;
}

int iCardControlX (const char* szHosSvrForSiIP,const char* szHosSvrForSiPort,const char * pCtrlType, const char * pCtrlInfo,char*  pErrMsg)
{
	if(g_handle>0)
	{
		strcpy(pErrMsg,"读写器已打开，请选关闭读写器");
		return SB_ERR_CONNECT;
	}
	int pReaderHandle=0;
	int re = iOpenPort( pErrMsg);
	if(re!=0)
		return re;
	re = iCardControl(szHosSvrForSiIP,szHosSvrForSiPort,pCtrlType,pCtrlInfo,pErrMsg);
	if(re!=0)
	{
		iClosePort(pErrMsg);
		return re;
	}
	re = iClosePort(pErrMsg);
	if(re!=0)
		return re;
	return SB_ERR_OK;
}

int iCardReadAtr(int pReaderHandle, char* pCardAtr, char* pErrMsg)
{
	strcpy(EffectDF,"");
	strcpy(EffectDFEF,"");
	strcpy(EffectDF1,"");
	strcpy(EffectDFEF1,"");
	if(pReaderHandle<=0)
	{
		strcpy(pErrMsg,"读写器未连接");
		return SB_ERR_UNCONNECT;
	}	
	if(g_handle != pReaderHandle)
	{
		strcpy(pErrMsg,"句柄值无效");
		return SB_ERR_UNCONNECT;
	}
	unsigned char Response[128]={0};
	strcpy(EffectDFEF,"");
	strcpy(EffectDF,"");
	int re = ICC_Reader_pre_PowerOn(g_handle,USERCard,Response);
	if(re<=0)
	{
		strcpy(pErrMsg,"卡片上电失败!");
		return SB_ERR_CARDMISS;
	}

	char a[3]={0};
	char temp[100]={0};
	for(int i=0;i<re;i++)
	{
		sprintf(a,"%02X",Response[i]);
		strcat(temp,a);
	}
	strcpy(pCardAtr,temp);
	for(int n=re-8;n<re;n++)
	{	
		_ATR[n-re+8] =  Response[n] ;
	}
	unsigned char cmd[100]={0};
	unsigned char resp[100]={0};
	memcpy(cmd, "\x00\xA4\x04\x00\x0F\x73\x78\x31\x2E\x73\x68\x2E\xC9\xE7\xBB\xE1\xB1\xA3\xD5\xCF",20);
	//00A404000F7378312E73682EC9E7BBE1B1A3D5CF
	re = ICC_Reader_Application(g_handle,USERCard,20,cmd,resp);//X选择MF文件
	if(resp[re-2]!=0x61 && resp[re-2]!=0x90)
	{
		strcpy(pErrMsg,"卡类型不对");
		return 105;
	}
	memcpy(cmd, "\x00\xA4\x00\x00\x02\xEF\x05",7);
	re = ICC_Reader_Application(g_handle,USERCard,7,cmd,resp);
	memcpy(cmd, "\x00\xB2\x01\x04\x12",5);
	re = ICC_Reader_Application(g_handle,USERCard,5,cmd,resp);

	if(re<0 || resp[re-2]!=0x90)
	{
		strcpy(pErrMsg,"卡片为空卡");
		return 105;
	}
	char sdisp[3]={0};
	char saccm[40]={0};
	for(n=0;n<re;n++)
	{
		sprintf(sdisp,"%02x",resp[n]);
		strcat(saccm,sdisp);
	}
	memcpy(_CityCode,saccm+4,32);
	return SB_ERR_OK;
}

int iCardReadAtrX (char* pCardAtr, char* pErrMsg)
{
	if(g_handle>0)
	{
		strcpy(pErrMsg,"读写器已打开，请选关闭读写器");
		return SB_ERR_CONNECT;
	}
	int pReaderHandle=0;
	int re = iOpenPort( pErrMsg);
	if(re!=0)
		return re;
	re = iCardReadAtr(pReaderHandle, pCardAtr, pErrMsg);
	if(re!=0)
	{
		iClosePort(pErrMsg);
		return re;
	}
	re = iClosePort(pErrMsg);
	if(re!=0)
		return re;

	return SB_ERR_OK;
}

int WINAPI iReadCardNo(const char* pInputPin, char* pCardNo, char* pErrMsg)
{
	if(pReaderHandle<=0)
	{
		strcpy(pErrMsg,"读写器未连接");
		return SB_ERR_UNCONNECT;
	}	
	if(g_handle != pReaderHandle)
	{
		strcpy(pErrMsg,"句柄值无效");
		return SB_ERR_UNCONNECT;
	}
	unsigned char Response[127]={0};
/*	int re = ICC_Reader_pre_PowerOn(pReaderHandle,USERCard,Response);
	if(re<=0)
	{
		strcpy(pErrMsg,"卡片上电失败!");
		return SB_ERR_CARDMISS;
	}
*/
	memset(Response,0,128);
	unsigned char cmd[200]={0};
	memset(cmd,0,50);
	memset(Response,0,128);
	memcpy(cmd, "\x00\xA4\x04\x00\x0F\x73\x78\x31\x2E\x73\x68\x2E\xC9\xE7\xBB\xE1\xB1\xA3\xD5\xCF",20);
	//00A404000F7378312E73682EC9E7BBE1B1A3D5CF
	int re = ICC_Reader_Application(pReaderHandle,USERCard,20,cmd,Response);//X选择MF文件
	if(re<=0)
	{
		strcpy(pErrMsg,"卡类型错误!");
		return SB_ERR_CARDTYPE;
	}
	if(Response[re-2]!=0x61 && Response[re-2]!=0x90 )
	{
		strcpy(pErrMsg,"卡类型错误!");
		return SB_ERR_CARDTYPE;
	}

	memset(cmd,0,50);
	memset(Response,0,128);
	memcpy(cmd, "\x00\xA4\x00\x00\x02\xEF\x05",7);
	re = ICC_Reader_Application(pReaderHandle,USERCard,7,cmd,Response);//选择EF05文件
	if(re<=0 || Response[re-2]!=0x90)
	{
		strcpy(pErrMsg,"卡类型错误!");
		return SB_ERR_CARDTYPE;
	}

	memset(cmd,0,50);
	memset(Response,0,128);
	memcpy(cmd, "\x00\xB2\x01\x04\x12",5);
	re = ICC_Reader_Application(pReaderHandle,USERCard,5,cmd,Response);//读取社保卡号
	if(re<=0 || Response[re-2]!=0x90)
	{
		unsigned char err[10]={0};
		HexToStr(Response+re-2,err,2);
		strcpy(pErrMsg,"卡类型错误!");
		strcat(pErrMsg,(char*)err);
		return SB_ERR_CARDTYPE;
	}
	unsigned char temp[100]={0};
	HexToStr(Response+2,temp,re-4);

	strcpy(pCardNo,(char*)temp);
	strcat(pCardNo,"|");

	memset(cmd,0,50);
	memset(Response,0,128);
	memcpy(cmd, "\x00\xB2\x03\x04\x06",5);
	re = ICC_Reader_Application(pReaderHandle,USERCard,5,cmd,Response);//读取社保卡号
	if(re<=0 || Response[re-2]!=0x90)
	{
		strcpy(pErrMsg,"卡类型错误!");
		return SB_ERR_CARDTYPE;
	}
	memset(temp,0,100);
	memcpy(temp,Response+2,4);//获取卡号
	strcat(pCardNo,(char*)temp);
	return SB_ERR_OK;
}

int iReadCardNoX(const char* pInputPin, char* pCardNo, char* pErrMsg)
{
	if(g_handle>0)
	{
		strcpy(pErrMsg,"读写器已打开，请选关闭读写器");
		return SB_ERR_CONNECT;
	}
	
	int pReaderHandle=0;
	int re = iOpenPort( pErrMsg);
	if(re!=0)
		return re;
	if(GetAtrCityCode(pErrMsg)!=0)
	{
		return SB_ERR_CARDTYPE;
	}
	re = iReadCardNo(pInputPin, pCardNo, pErrMsg);
	if(re!=0)
	{
		iClosePort(pErrMsg);
		return re;
	}
	re = iClosePort(pErrMsg);
	if(re!=0)
		return re;
	return SB_ERR_OK;
}

int iReadPin(int pReaderHandle,const char * pCtrlType,char *pInputPin, char* pErrMsg)
{
	if(pReaderHandle<=0)
	{
		strcpy(pErrMsg,"读写器未连接");
		return SB_ERR_UNCONNECT;
	}	
	if(g_handle != pReaderHandle)
	{
		strcpy(pErrMsg,"句柄值无效");
		return SB_ERR_UNCONNECT;
	}
	unsigned char rlen;
	unsigned char cpass[100]={0};
	unsigned char cpass1[100]={0};
	if(strcmp(pCtrlType,"01")==0)//更改
	{
		int re = ICC_DispSound(pReaderHandle,13,1);//输入旧密码
		if(re!=0)
		{
			strcpy(pErrMsg,"获取密码失败");
			return SB_ERR_OTHERERROR;
		}
		re = ICC_GetInputPass(pReaderHandle, 20, &rlen,1,1, cpass);//使用外置
		if(re!=0)
		{
			strcpy(pErrMsg,"获取密码失败");
			return SB_ERR_OTHERERROR;
		}
		for(int i=0;i<rlen;i++)
		{
			cpass[i] = cpass[i]+0x30;
		}
		strcpy(pInputPin,(char*)cpass);
		strcat(pInputPin,"|");
		
		memset(cpass,0,100);
		re = ICC_DispSound(pReaderHandle,12,1);//输入新密码
		if(re!=0)
		{
			strcpy(pErrMsg,"获取密码失败");
			return SB_ERR_OTHERERROR;
		}
		re = ICC_GetInputPass(pReaderHandle, 20, &rlen,1,1, cpass);//使用外置
		if(re!=0)
		{
			strcpy(pErrMsg,"获取密码失败");
			return SB_ERR_OTHERERROR;
		}
		for(i=0;i<rlen;i++)
		{
			cpass[i] = cpass[i]+0x30;
		}

		re = ICC_DispSound(pReaderHandle,14,1);//确认新密码
		if(re!=0)
		{
			strcpy(pErrMsg,"获取密码失败");
			return SB_ERR_OTHERERROR;
		}
		re = ICC_GetInputPass(pReaderHandle, 20, &rlen,1,1, cpass1);//使用外置
		if(re!=0)
		{
			strcpy(pErrMsg,"获取密码失败");
			return SB_ERR_OTHERERROR;
		}
		for(i=0;i<rlen;i++)
		{
			cpass1[i] = cpass1[i]+0x30;
		}
		if(strcmp((char*)cpass,(char*)cpass1)!=0)
		{
			strcpy(pErrMsg,"两次输入新密码有误");
			return SB_ERR_OTHERERROR;
		}
		strcat(pInputPin,(char*)cpass);


	}
	else if(strcmp(pCtrlType,"02")==0)//重置
	{
		int re = ICC_DispSound(pReaderHandle,12,1);//输入新密码
		if(re!=0)
		{
			strcpy(pErrMsg,"获取密码失败");
			return SB_ERR_OTHERERROR;
		}
		re = ICC_GetInputPass(pReaderHandle, 20, &rlen,1,1, cpass);//使用外置
		if(re!=0)
		{
			strcpy(pErrMsg,"获取密码失败");
			return SB_ERR_OTHERERROR;
		}
		for(int i=0;i<rlen;i++)
		{
			cpass[i] = cpass[i]+0x30;
		}

		re = ICC_DispSound(pReaderHandle,14,1);//确认新密码
		if(re!=0)
		{
			strcpy(pErrMsg,"获取密码失败");
			return SB_ERR_OTHERERROR;
		}
		re = ICC_GetInputPass(pReaderHandle, 20, &rlen,1,1, cpass1);//使用外置
		if(re!=0)
		{
			strcpy(pErrMsg,"获取密码失败");
			return SB_ERR_OTHERERROR;
		}
		for(i=0;i<rlen;i++)
		{
			cpass1[i] = cpass1[i]+0x30;
		}
		if(strcmp((char*)cpass,(char*)cpass1)!=0)
		{
			strcpy(pErrMsg,"两次输入新密码有误");
			return SB_ERR_OTHERERROR;
		}
		strcpy(pInputPin,(char*)cpass);
	}
	else if(strcmp(pCtrlType,"03")==0)//校验
	{
		int re = ICC_DispSound(pReaderHandle,4,1);//使用外置 
		if(re!=0)
		{
			strcpy(pErrMsg,"获取密码失败");
			return SB_ERR_OTHERERROR;
		}
		re = ICC_GetInputPass(pReaderHandle, 20, &rlen,1,1, cpass);//使用外置
		if(re!=0)
		{
			strcpy(pErrMsg,"获取密码失败");
			return SB_ERR_OTHERERROR;
		}
		for(int i=0;i<rlen;i++)
		{
			cpass[i] = cpass[i]+0x30;
		}
		strcpy(pInputPin,(char*)cpass);
	}
	else 
	{
		strcpy(pErrMsg,"操作类型有误");
		return SB_ERR_OTHERERROR;
	}
	/*	1、请插卡 2、请刷卡 3、读卡错误 4、请输入密码  5、密码错误  6、操作成功 7、操作超时 8、操作失败 9、请取回卡 10、请重新输入密码 11、请再次输入密码 12、请输入新密码 13、请输入旧密码 14、请确认新密码
*/
	return SB_ERR_OK;
}

int iReadPinX (const char * pCtrlType,char *pInputPin, char* pErrMsg)
{
	if(g_handle>0)
	{
		strcpy(pErrMsg,"读写器已打开，请选关闭读写器");
		return SB_ERR_CONNECT;
	}
	
	int pReaderHandle=0;
	int re = iOpenPort( pErrMsg);
	if(re!=0)
		return re;
	re = iReadPin(pReaderHandle,pCtrlType, pInputPin, pErrMsg);
	if(re!=0)
	{
		iClosePort(pErrMsg);
		return re;
	}
	re = iClosePort(pErrMsg);
	if(re!=0)
		return re;
	return SB_ERR_OK;
}

int iReadVer (char *pVerInfo, char* pErrMsg)
{
	strcpy(pVerInfo,"HD100-V1.00");
	return SB_ERR_OK;
}

int WINAPI iReadCardE(int pReaderHandle,const char* pInputPin, const char* pFileAddr, char* pOutDataBuff,char* pErrMsg)
{
	if(pReaderHandle<=0)
	{
		strcpy(pErrMsg,"读写器未连接");
		return SB_ERR_UNCONNECT;
	}	
	if(g_handle!=pReaderHandle)
	{
		strcpy(pErrMsg,"句柄值无效");
		return SB_ERR_CONNECT;
	}
	if(strlen(pInputPin)!=6)
	{
		strcpy(pErrMsg,"输入PIN长度错误");
		return SB_ERR_OPNUMERROR;
	}
	
	char data[9000]={0};//存放读取后的数据
	int filelen = strlen(pFileAddr);
	if(filelen<8)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED;
	}
	int ad = findChar((char*)pFileAddr,0,':');
	int addr = findChar((char*)pFileAddr,0,'|');//先对字符串格式进行判断
	if(addr<0 && ad<0)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED;
	}
	int count = getCount((char*)pFileAddr,'$');//获取需要循环的次数
	if(count<0)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED;
	}
	if(getCount((char*)pFileAddr,' ')>0)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED;
	}

	char temp[1000]={0};
	char data_item[1000]={0};//存放数据项
	char data_DFEF[100]={0};//存放DF和EF
	char DDF[10]={0};
	char EEF[10]={0};

	int addr_data=0;//记录$的位置 EF的分隔符
	int addr_item =0;//记录|的位置  记录的分隔符  
	addr =0;
	int addr1=0,addr2=0;
	int ret;
/*	
	if(GetAtrCityCode(pErrMsg)!=0)
	{
		return SB_ERR_TYPEERROR;
	}*/
	//MFEF05|07|$MFEF06|08|09|0A|$
	for(int i=0;i<count;i++)//DF范围内循环
	{
		addr1 = findChar((char*)pFileAddr,addr_data,'$');// pFileAddr = MFEF05|07|$MFEF06|08|09|0A|$
		memset(temp,0,1000);
		memcpy(temp,pFileAddr+addr_data,addr1-addr_data);//获取DF和EF以及数据项  temp = MFEF05|07|08|
		addr_data=addr1+1;
		int len,iCount;

		ad = findChar((char*)temp,0,':');
		addr = findChar((char*)temp,0,'|');//先对字符串格式进行判断

		if(ad>0 && addr>0 && addr<ad) //B-TLV格式
		{
			addr2 = findChar((char*)temp,0,'|');
			memcpy(data_DFEF,temp,addr2);//获取DF和EF  data_DFEF = DF02EF07

			len = strlen(data_DFEF);
			memcpy(data_item,temp+addr2+1,addr1-addr2-1);// data_item = 99:01|99:02|
			if(strlen(data_item)<6)
			{
				fileType =0;
				strcpy(pErrMsg,"输入字符串非法");
				return SB_ERR_UNSUPPORTED; 
			}
			iCount = getCount((char*)data_item,'|');//获取|数量
			fileType =2;
		}
		else if(ad>0 && addr<0)//循环文件 DF02EF07:01$DF02EF07:02$DF02EF07:03$DF02EF07:04$
		{
			addr2 = findChar((char*)temp,0,':');
			memcpy(data_DFEF,temp,addr2);//获取DF和EF  data_DFEF = DF02EF07

			len = strlen(data_DFEF);
			memcpy(data_item,temp+addr2+1,addr1-addr2-1);// data_item = 01|

			fileType = 3;
		}
		else if(ad<0 && addr>0)
		{
			addr2 = findChar((char*)temp,0,'|');
			memcpy(data_DFEF,temp,addr2);//获取DF和EF  data_DFEF = MFEF05
		//	strcat(data,data_DFEF);
		//	strcat(data,"|");
		
			len = strlen(data_DFEF);
			memcpy(data_item,temp+addr2+1,addr1-addr2-1);// data_item = 07|08|
			iCount = getCount((char*)data_item,'|');//获取|数量
			fileType =1;
		}
		else
		{
			strcpy(pErrMsg,"输入字符串非法");
			return SB_ERR_UNSUPPORTED; 
		}
		if(len==6)//说明是MF
		{
			memset(DDF,0,10);
			memcpy(DDF,data_DFEF,2);
			memcpy(EEF,data_DFEF+2,4);
		}
		else if(len==8)
		{
			memcpy(DDF,data_DFEF,4);
			memcpy(EEF,data_DFEF+4,4);
		}
		else
		{
			strcpy(pErrMsg,"输入字符串格式错误");
			return SB_ERR_UNSUPPORTED;
		}

		if (strcmp(DDF,"MF")==0 || strcmp(DDF,"DDF1")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}
			if (strcmp(EEF,"EF05")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 1:ret = iReadRecordEX("EF05",recno,0x10,CN_style,result);break;
					case 2:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
					case 3:ret = iReadRecordEX("EF05",recno,0x04,AN_style,result);break;
					case 4:ret = iReadRecordEX("EF05",recno,0x0C,CN_style,result);break;
					case 5:ret = iReadRecordEX("EF05",recno,0x04,CN_style,result);break;
					case 6:ret = iReadRecordEX("EF05",recno,0x04,CN_style,result);break;
					case 7:ret = iReadRecordEX("EF05",recno,0x09,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					//case 8:ret = iReadRecord("EF05",recno,0x09,AN_style,result);break;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x08:ret = iReadRecordEX("EF06",recno,0x12,AN_style,result);break;
					case 0x09:ret = iReadRecordEX("EF06",recno,0x1E,AN_style,result);break;
					case 0x4E:ret = iReadRecordEX("EF06",recno,0x14,AN_style,result);break;
					case 0x0A:ret = iReadRecordEX("EF06",recno,0x01,AN_style,result);break;
					case 0x0B:ret = iReadRecordEX("EF06",recno,0x01,CN_style,result);break;
					case 0x0C:ret = iReadRecordEX("EF06",recno,0x03,CN_style,result);break;
					case 0x0D:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{
				char result[0x700]={0};
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x0A,0x0A};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"RKSSSE认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}

				ret = iReadBinary("EF07",00,0x300,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
			}
			else if (strcmp(EEF,"EF08")==0)
			{
				char result[0x2002]={0};
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x0A,0x0A};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"RKSSSE认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}

				ret = iReadBinary("EF08",00,0x1000,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
			}
			else if (strcmp(EEF,"EF0D")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[113]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0xEA:ret = iReadRecordEX("EF0D",recno,0x0C,AN_style,result);break;
					case 0xEB:ret = iReadRecordEX("EF0D",recno,0x03,CN_style,result);break;
					case 0xEC:ret = iReadRecordEX("EF0D",recno,0x01,AN_style,result);break;
					case 0xED:ret = iReadRecordEX("EF0D",recno,0x01,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF0D结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}

		}//DDF结束符
		else if(strcmp(DDF,"DF01")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}
			if(SelectFile("DF01"))
			{
				strcpy(pErrMsg,"输入DF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
			if(strlen(pInputPin)!=6)
			{
				strcpy(pErrMsg,"输入PIN长度错误");
				return SB_ERR_OPNUMERROR;
			}
			if(!IsDec((char*)pInputPin))
			{
				strcpy(pErrMsg,"输入PIN有非法字符");
				return SB_ERR_OPNUMERROR;
			}
			ret = VerifyPIN(pInputPin,pErrMsg);
			if(ret!=0)
				return ret;
			if (strcmp(EEF,"EF05")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x20:ret = iReadRecordEX("EF05",recno,0x02,AN_style,result);break;
					case 0x21:ret = iReadRecordEX("EF05",recno,0x50,AN_style,result);break;
					case 0x0E:ret = iReadRecordEX("EF05",recno,0x03,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;	
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x23:ret = iReadRecordEX("EF06",recno,0x50,AN_style,result);break;
					case 0x24:ret = iReadRecordEX("EF06",recno,0x03,CN_style,result);break;
					case 0x28:ret = iReadRecordEX("EF06",recno,0x0F,AN_style,result);break;
					case 0x2C:ret = iReadRecordEX("EF06",recno,0x32,AN_style,result);break;
					case 0x2D:ret = iReadRecordEX("EF06",recno,0x0F,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x29:ret = iReadRecordEX("EF07",recno,0x01,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF07结束符
			else if (strcmp(EEF,"EF08")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x2B:ret = iReadRecordEX("EF08",recno,0x01,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF08结束符
			else if (strcmp(EEF,"EF09")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x2E:ret = iReadRecordEX("EF09",recno,0x46,AN_style,result);break;
					case 0x30:ret = iReadRecordEX("EF09",recno,0x09,AN_style,result);break;
					case 0x32:ret = iReadRecordEX("EF09",recno,0x02,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF09结束符
			else if (strcmp(EEF,"EF0A")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x37:ret = iReadRecordEX("EF0A",recno,0x03,AN_style,result);break;
					case 0x38:ret = iReadRecordEX("EF0A",recno,0x02,AN_style,result);break;
					case 0x39:ret = iReadRecordEX("EF0A",recno,0x04,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF0A结束符
			else if (strcmp(EEF,"EF15")==0)//BTLV
			{	
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						switch(recno)
						{
						case 0x2A:ret = iReadRecordEX("EF15",recno,0x01,CN_style,result);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,result);
							return ret;
						}
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"40")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[3]={{"57",0x03,AN_style},{"58",0x03,CN_style},{"59",0x28,AN_style}};
							ret = iReadBTLVEX("EF15",recno+1,0x34,tlv,3,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;

				}//循环结束
				addr_item =0;

			}//EF15结束符
			else if (strcmp(EEF,"EF16")==0)
			{
				char result[0x300]={0};
				
				ret = iReadBinary("EF16",00,0x128,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
			}
			else if (strcmp(EEF,"EF17")==0)
			{
				char result[0x300]={0};
				
				ret = iReadBinary("EF17",00,0x128,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
			}
			else if (strcmp(EEF,"EF18")==0)
			{
				char result[0x300]={0};
				
				ret = iReadBinary("EF18",00,0x128,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
			}
			else if (strcmp(EEF,"EF19")==0)
			{
				char result[0x300]={0};
				
				ret = iReadBinary("EF19",00,0x128,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
			}
			else if (strcmp(EEF,"EF1A")==0)
			{
				char result[0x300]={0};
				
				ret = iReadBinary("EF1A",00,0x128,CN_style,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
			}
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF01结束
		else if(strcmp(DDF,"DF02")==0)
		{
		/*	if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}*/
			if(SelectFile("DF02"))
			{
				strcpy(pErrMsg,"输入DF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
			if(strlen(pInputPin)!=6)
			{
				strcpy(pErrMsg,"输入PIN长度错误");
				return SB_ERR_OPNUMERROR;
			}
			if(!IsDec((char*)pInputPin))
			{
				strcpy(pErrMsg,"输入PIN有非法字符");
				return SB_ERR_OPNUMERROR;
			}
			ret = VerifyPIN(pInputPin,pErrMsg);
			if(ret!=0)
				return ret;
			if (strcmp(EEF,"EF05")==0)//BTLV
			{	
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						switch(recno)
						{
						case 0x42:ret = iReadRecordEX("EF05",recno,0x03,AN_style,result);break;
						case 0x41:ret = iReadRecordEX("EF05",recno,0x03,AN_style,result);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,result);
							return ret;
						}
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"43")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[5]={{"5A",0x07,AN_style},{"5B",0x01,CN_style},{"5C",0x46,AN_style},{"5D",0x04,CN_style},{"5E",0x10,AN_style}};
							ret = iReadBTLVEX("EF05",recno+2,0x6C,tlv,5,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else if(strcmp(a,"44")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[4]={{"33",0x03,AN_style},{"34",0x46,AN_style},{"35",0x04,CN_style},{"36",0x10,AN_style}};
							ret = iReadBTLVEX("EF05",recno+4,0x65,tlv,4,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
						
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;

				}//循环结束
				addr_item =0;

			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x4C:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0x4B:ret = iReadRecordEX("EF06",recno,0x05,AN_style,result);break;
					case 0x4D:ret = iReadRecordEX("EF06",recno,0x03,CN_style,result);break;
					case 0x60:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0x4F:ret = iReadRecordEX("EF06",recno,0x06,AN_style,result);break;
					case 0x50:ret = iReadRecordEX("EF06",recno,0x03,CN_style,result);break;
					case 0x3A:ret = iReadRecordEX("EF06",recno,0x07,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)//循环文件
			{	
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
					char a[13]={0};
					char result[0x100]={0};
					memcpy(a,data_item,strlen(data_item));
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					if(!IsEffectChar(a))
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					CYC cy[4]={{0x07,AN_style},{0x04,CN_style},{0x04,CN_style},{0x46,AN_style}};
					ret = iReadXHFileEX("EF07",recno,0x55,cy,4,result);
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");

			}//EF07结束符
			else if (strcmp(EEF,"EF09")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x55:ret = iReadRecordEX("EF09",recno,0x10,AN_style,result);break;
					case 0x56:ret = iReadRecordEX("EF09",recno,0x46,AN_style,result);break;
					case 0x96:ret = iReadRecordEX("EF09",recno,0x04,CN_style,result);break;
					case 0x97:ret = iReadRecordEX("EF09",recno,0x03,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF09结束符
			else if (strcmp(EEF,"EF15")==0)//BTLV
			{	
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						switch(recno)
						{
						case 0x0F:ret = iReadRecordEX("EF15",recno,0x04,CN_style,result);break;
						case 0x1F:ret = iReadRecordEX("EF15",recno,0x03,CN_style,result);break;
							default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,result);
							return ret;
						}
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"99")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[5]={{"10",0x03,AN_style},{"11",0x04,CN_style},{"12",0x03,CN_style}};
							ret = iReadBTLVEX("EF15",recno,0x10,tlv,3,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
						
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;

				}//循环结束
				addr_item =0;
			}//EF15结束符
			else if (strcmp(EEF,"EF16")==0)//循环文件
			{	
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char a[13]={0};
				char result[0x100]={0};
				memcpy(a,data_item,strlen(data_item));
				if(strlen(a)!=2)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(a))
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				int recno = chartoint1(a[0])*16+chartoint1(a[1]);
				
				CYC cy[5]={{0x03,AN_style},{0x04,CN_style},{0x04,CN_style},{0x04,CN_style},{0x03,CN_style}};
				ret = iReadXHFileEX("EF16",recno,0x12,cy,5,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
				strcat(data,"|");
			}//EF16结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF02结束

		else if(strcmp(DDF,"DF03")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}
			if(SelectFile("DF03"))
			{
				strcpy(pErrMsg,"输入DF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
			if(strlen(pInputPin)!=6)
			{
				strcpy(pErrMsg,"输入PIN长度错误");
				return SB_ERR_OPNUMERROR;
			}
			if(!IsDec((char*)pInputPin))
			{
				strcpy(pErrMsg,"输入PIN有非法字符");
				return SB_ERR_OPNUMERROR;
			}
			ret = VerifyPIN(pInputPin,pErrMsg);
			if(ret!=0)
				return ret;

			if (strcmp(EEF,"EF05")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x61:ret = iReadRecordEX("EF05",recno,0x03,CN_style,result);break;
				//	case 0x60:ret = iReadRecord("EF05",recno,0x04,CN_style,result);break;
					case 0x98:ret = iReadRecordEX("EF05",recno,0x04,CN_style,result);break;
					case 0x62:ret = iReadRecordEX("EF05",recno,0x04,CN_style,result);break;
					case 0x63:ret = iReadRecordEX("EF05",recno,0x03,AN_style,result);break;
					case 0x64:ret = iReadRecordEX("EF05",recno,0x03,AN_style,result);break;
					case 0x65:ret = iReadRecordEX("EF05",recno,0x03,AN_style,result);break;
					case 0x66:ret = iReadRecordEX("EF05",recno,0x03,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;	
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)//BTLV
			{	
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						switch(recno)
						{
						case 0x45:ret = iReadRecordEX("EF06",recno,0x14,AN_style,result);break;
						case 0x46:ret = iReadRecordEX("EF06",recno,0x02,AN_style,result);break;
						case 0x47:ret = iReadRecordEX("EF06",recno,0x02,AN_style,result);break;
						case 0x48:ret = iReadRecordEX("EF06",recno,0x01,AN_style,result);break;
						case 0x49:ret = iReadRecordEX("EF06",recno,0x01,AN_style,result);break;
						case 0x67:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
						case 0x6B:ret = iReadRecordEX("EF06",recno,0x3C,AN_style,result);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,result);
							return ret;
						}
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"4A")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[5]={{"13",0x02,AN_style},{"14",0x01,AN_style},{"15",0x14,AN_style},{"16",0x04,CN_style},{"17",0x3C,AN_style}};
							ret = iReadBTLVEX("EF06",recno+7,0x61,tlv,5,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;

				}//循环结束
				addr_item =0;	
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x70:ret = iReadRecordEX("EF07",recno,0x03,AN_style,result);break;
					case 0x71:ret = iReadRecordEX("EF07",recno,0x03,CN_style,result);break;
					case 0x6E:ret = iReadRecordEX("EF07",recno,0x04,CN_style,result);break;
					case 0x6C:ret = iReadRecordEX("EF07",recno,0x04,CN_style,result);break;
					case 0x6F:ret = iReadRecordEX("EF07",recno,0x04,CN_style,result);break;
					case 0x73:ret = iReadRecordEX("EF07",recno,0x04,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF07结束符
			else if (strcmp(EEF,"EF15")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x7A:ret = iReadRecordEX("EF15",recno,0x03,CN_style,result);break;
					case 0x7B:ret = iReadRecordEX("EF15",recno,0x04,CN_style,result);break;
					case 0x7C:ret = iReadRecordEX("EF15",recno,0x04,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF15结束符
			else if (strcmp(EEF,"EF16")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x51:ret = iReadRecordEX("EF16",recno,0x03,CN_style,result);break;
					case 0x5F:ret = iReadRecordEX("EF16",recno,0x01,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF16结束符
			else if (strcmp(EEF,"EF17")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x3B:ret = iReadRecordEX("EF17",recno,0x14,AN_style,result);break;
					case 0x3C:ret = iReadRecordEX("EF17",recno,0x46,AN_style,result);break;
					case 0x3D:ret = iReadRecordEX("EF17",recno,0x01,AN_style,result);break;
					case 0x3E:ret = iReadRecordEX("EF17",recno,0x04,CN_style,result);break;
					case 0x3F:ret = iReadRecordEX("EF17",recno,0x3C,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF17结束符
			else if (strcmp(EEF,"EF18")==0)//BTLV
			{	
			
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x74:ret = iReadRecordEX("EF18",recno,0x03,AN_style,result);break;
					case 0x75:ret = iReadRecordEX("EF18",recno,0x03,CN_style,result);break;
					case 0x76:ret = iReadRecordEX("EF18",recno,0x01,CN_style,result);break;
					case 0x77:ret = iReadRecordEX("EF18",recno,0x04,CN_style,result);break;
					case 0x78:ret = iReadRecordEX("EF18",recno,0x04,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF18结束符
			else if (strcmp(EEF,"EF19")==0)//BTLV
			{	
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"79")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[7]={{"18",0x03,AN_style},{"19",0x03,CN_style},{"1A",0x04,CN_style},{"1B",0x04,CN_style},{"1C",0x04,CN_style},{"1D",0x03,AN_style},{"1E",0x04,CN_style}};
							ret = iReadBTLV("EF19",recno,0x27,tlv,7,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;

				}//循环结束
				addr_item =0;		
			}//EF19结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF03结束
		else if(strcmp(DDF,"DF04")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}
			if(SelectFile("DF04"))
			{
				strcpy(pErrMsg,"输入DF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
			if(strlen(pInputPin)!=6)
			{
				strcpy(pErrMsg,"输入PIN长度错误");
				return SB_ERR_OPNUMERROR;
			}
			if(!IsDec((char*)pInputPin))
			{
				strcpy(pErrMsg,"输入PIN有非法字符");
				return SB_ERR_OPNUMERROR;
			}
			ret = VerifyPIN(pInputPin,pErrMsg);
			if(ret!=0)
				return ret;

			if (strcmp(EEF,"EF05")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x81:ret = iReadRecordEX("EF05",recno,0x08,AN_style,result);break;
					case 0x84:ret = iReadRecordEX("EF05",recno,0x03,CN_style,result);break;
					case 0x87:ret = iReadRecordEX("EF05",recno,0x11,AN_style,result);break;
					case 0x8C:ret = iReadRecordEX("EF05",recno,0x01,CN_style,result);break;
					case 0x80:ret = iReadRecordEX("EF05",recno,0x04,CN_style,result);break;
					case 0x8B:ret = iReadRecordEX("EF05",recno,0x1D,AN_style,result);break;
					case 0x8A:ret = iReadRecordEX("EF05",recno,0x0F,AN_style,result);break;
					case 0x83:ret = iReadRecordEX("EF05",recno,0x09,AN_style,result);break;
					case 0x86:ret = iReadRecordEX("EF05",recno,0x09,AN_style,result);break;
					case 0x89:ret = iReadRecordEX("EF05",recno,0x09,AN_style,result);break;
					case 0x7D:ret = iReadRecordEX("EF05",recno,0x09,AN_style,result);break;
					case 0x7E:ret = iReadRecordEX("EF05",recno,0x09,AN_style,result);break;
					case 0x7F:ret = iReadRecordEX("EF05",recno,0x09,AN_style,result);break;
					case 0x8D:ret = iReadRecordEX("EF05",recno,0x09,AN_style,result);break;
					case 0x8E:ret = iReadRecordEX("EF05",recno,0x09,AN_style,result);break;
					case 0x8F:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;	
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				unsigned char wordkey[4]={0x67,0x2D,0x84};
				char err[20]={0};
				if(ExterAuthen(wordkey,1,err))
				{
					strcpy(pErrMsg,"RK1DF04认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0x90:ret = iReadRecordEX("EF06",recno,0x01,AN_style,result);break;
					case 0x92:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0x93:ret = iReadRecordEX("EF06",recno,0x02,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF08")==0)//循环文件
			{	
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char a[13]={0};
				char result[100]={0};
				memcpy(a,data_item,strlen(data_item));
				if(strlen(a)!=2)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(a))
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				int recno = chartoint1(a[0])*16+chartoint1(a[1]);
				
				CYC cy[7]={{0x02,CN_style},{0x01,AN_style},{0x06,CN_style},{0x07,CN_style},{0x04,CN_style},{0x04,CN_style},{0x04,CN_style}};
				ret = iReadXHFileEX("EF08",recno,0x1C,cy,7,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
				strcat(data,"|");
				
			}//EF08结束符
			else if (strcmp(EEF,"EF15")==0)//循环文件
			{	
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char a[13]={0};
				char result[100]={0};
				memcpy(a,data_item,strlen(data_item));
				if(strlen(a)!=2)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(a))
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				int recno = chartoint1(a[0])*16+chartoint1(a[1]);
				CYC cy[2]={{0x02,CN_style},{0x01,AN_style}};
				ret = iReadXHFileEX("EF15",recno,0x03,cy,2,result);
				if(ret!=0)
				{
					strcpy(pErrMsg,result);
					return ret;
				}
				strcat(data,result);
				strcat(data,"|");
			}//EF15结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF04结束
		else if(strcmp(DDF,"DF05")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}
			if(SelectFile("DF05"))
			{
				strcpy(pErrMsg,"输入DF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
			if(strlen(pInputPin)!=6)
			{
				strcpy(pErrMsg,"输入PIN长度错误");
				return SB_ERR_OPNUMERROR;
			}
			if(!IsDec((char*)pInputPin))
			{
				strcpy(pErrMsg,"输入PIN有非法字符");
				return SB_ERR_OPNUMERROR;
			}
			ret = VerifyPIN(pInputPin,pErrMsg);
			if(ret!=0)
				return ret;

			if (strcmp(EEF,"EF05")==0)
			{
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						switch(recno)
						{
						case 0xA0:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xA1:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xB9:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xA2:ret = iReadRecordEX("EF05",recno,0x04,AN_style,result);break;
						case 0xA9:ret = iReadRecordEX("EF05",recno,0x01,CN_style,result);break;

						case 0xBA:ret = iReadRecordEX("EF05",recno,0x14,AN_style,result);break;
						case 0xBB:ret = iReadRecordEX("EF05",recno,0x64,AN_style,result);break;
						case 0xAA:ret = iReadRecordEX("EF05",recno,0x14,AN_style,result);break;
						case 0xAB:ret = iReadRecordEX("EF05",recno,0x04,CN_style,result);break;
						case 0xAC:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;

						case 0xAD:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xAE:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xAF:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xB0:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xB1:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						
						case 0xB2:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xB3:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xB4:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xB5:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xB6:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xB7:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xBC:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
						case 0xB8:ret = iReadRecordEX("EF05",recno,0x28,AN_style,result);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,result);
							return ret;
						}
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"A3")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[2]={{"A5",0x10,AN_style},{"A6",0x04,AN_style}};
							ret = iReadBTLVEX("EF05",recno+5,0x18,tlv,2,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else if(strcmp(a,"A4")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[2]={{"A7",0x10,AN_style},{"A8",0x03,AN_style}};
							ret = iReadBTLVEX("EF05",recno+10,0x17,tlv,2,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;

				}//循环结束
				addr_item =0;
			}
			else 
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}
		else if(strcmp(DDF,"DF06")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}
			if(SelectFile("DF06"))
			{
				strcpy(pErrMsg,"输入DF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
			char err[20]={0};
			unsigned char wordkey[4]={0x48,0x52,0x83};
			if(ExterAuthen(wordkey,2,err))
			{
				strcpy(pErrMsg,"RK1DF06认证失败!");
				strcat(pErrMsg,err);
				return SB_ERR_AUTH;
			}
			if (strcmp(EEF,"EF05")==0)
			{
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						switch(recno)
						{
						case 0xC0:ret = iReadRecordEX("EF05",recno,0x46,AN_style,result);break;
						case 0xC2:ret = iReadRecordEX("EF05",recno,0x09,AN_style,result);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,result);
							return ret;
						}
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"C3")==0 || strcmp(a,"C4")==0 ||strcmp(a,"C5")==0  )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							TLV tlv[5]={{"E3",0x01,CN_style},{"E4",0x04,CN_style},{"E5",0x04,CN_style},{"E6",0x04,CN_style},{"E7",0x03,CN_style}};
							ret = iReadBTLVEX("EF05",recno+2,0x1A,tlv,5,result);
							if(ret!=0)
							{
								strcpy(pErrMsg,result);
								return ret;
							}
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;

				}//循环结束
				addr_item =0;
			}
			else if(strcmp(EEF,"EF06")==0)
			{
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x52,0x83};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"RK1DF06认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0xC6:ret = iReadRecordEX("EF06",recno,0x46,AN_style,result);break;
					case 0xC8:ret = iReadRecordEX("EF06",recno,0x09,AN_style,result);break;
					case 0xC9:ret = iReadRecordEX("EF06",recno,0x01,CN_style,result);break;
					case 0xCA:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0xCB:ret = iReadRecordEX("EF06",recno,0x05,CN_style,result);break;

					case 0xCC:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0xCD:ret = iReadRecordEX("EF06",recno,0x02,CN_style,result);break;
					case 0xCE:ret = iReadRecordEX("EF06",recno,0x01,CN_style,result);break;
					case 0xCF:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0xD0:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;

					case 0xD1:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0xD2:ret = iReadRecordEX("EF06",recno,0x03,CN_style,result);break;
					case 0xD3:ret = iReadRecordEX("EF06",recno,0x01,CN_style,result);break;
					case 0xD4:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0xD5:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0xD6:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;

					case 0xD7:ret = iReadRecordEX("EF06",recno,0x03,CN_style,result);break;
					case 0xD8:ret = iReadRecordEX("EF06",recno,0x01,CN_style,result);break;
					case 0xD9:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0xDA:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0xDB:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0xDC:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0xDD:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0xDE:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0xDF:ret = iReadRecordEX("EF06",recno,0x03,CN_style,result);break;
					case 0xE0:ret = iReadRecordEX("EF06",recno,0x03,CN_style,result);break;
					case 0xE1:ret = iReadRecordEX("EF06",recno,0x02,CN_style,result);break;
					case 0xE2:ret = iReadRecordEX("EF06",recno,0x02,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;	
			}
			else 
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}
		else if(strcmp(DDF,"DF07")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}
			if(SelectFile("DF07"))
			{
				strcpy(pErrMsg,"输入DF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
			if(strlen(pInputPin)!=6)
			{
				strcpy(pErrMsg,"输入PIN长度错误");
				return SB_ERR_OPNUMERROR;
			}
			if(!IsDec((char*)pInputPin))
			{
				strcpy(pErrMsg,"输入PIN有非法字符");
				return SB_ERR_OPNUMERROR;
			}
			ret = VerifyPIN(pInputPin,pErrMsg);
			if(ret!=0)
				return ret;

			if (strcmp(EEF,"EF05")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x3D,0x83};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"RK1DF07认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0xF0:ret = iReadRecordEX("EF05",recno,0x02,AN_style,result);break;
					case 0xF1:ret = iReadRecordEX("EF05",recno,0x01,AN_style,result);break;
					case 0xF2:ret = iReadRecordEX("EF05",recno,0x04,CN_style,result);break;
					case 0xF3:ret = iReadRecordEX("EF05",recno,0x03,AN_style,result);break;
					case 0xF4:ret = iReadRecordEX("EF05",recno,0x04,CN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;	
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x3F,0x84};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"RK2DF04认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0xF5:ret = iReadRecordEX("EF06",recno,0x03,AN_style,result);break;
					case 0xF6:ret = iReadRecordEX("EF06",recno,0x04,CN_style,result);break;
					case 0xF7:ret = iReadRecordEX("EF06",recno,0x46,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0xB1,0x85};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"RK3DF04认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					switch(recno)
					{
					case 0xF8:ret = iReadRecordEX("EF07",recno,0x04,CN_style,result);break;
					case 0xF9:ret = iReadRecordEX("EF07",recno,0x01,AN_style,result);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,result);
						return ret;
					}
					strcat(data,result);
					strcat(data,"|");
					addr_item=add+1;
				}
				addr_item=0;
				
			}//EF07结束符	
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF07结束
		else 
		{
			strcpy(pErrMsg,"输入DF字符串错误");
			return SB_ERR_UNSUPPORTED; 
		}
	}//$循环结束符
	fileType =0;
	strcpy(pOutDataBuff,data);
	
	return SB_ERR_OK;
}


int WINAPI iWriteCardE(int pReaderHandle,const char* pInputPin, const char* pFileAddr, char* pWriteDataBuff,char* pErrMsg)
{
	
	if(pReaderHandle<=0)
	{
		strcpy(pErrMsg,"读写器未连接");
		return SB_ERR_UNCONNECT;
	}	
	if(g_handle!=pReaderHandle)
	{
		strcpy(pErrMsg,"句柄值无效");
		return SB_ERR_CONNECT;
	}
	if(strlen(pInputPin)!=6)
	{
		strcpy(pErrMsg,"输入PIN长度错误");
		return SB_ERR_OPNUMERROR;
	}
	int filelen = strlen(pFileAddr);
	if(filelen<8)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED;
	}
	int ad = findChar((char*)pFileAddr,0,':');
	int addr = findChar((char*)pFileAddr,0,'|');//先对字符串格式进行判断
	if(addr<0 && ad<0)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED;
	}
	int count = getCount((char*)pFileAddr,'$');//获取需要循环的次数
	if(count<0)
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED;
	}
	if(getCount((char*)pFileAddr,' ')>0 || getCount((char*)pWriteDataBuff,' ')>0 )
	{
		strcpy(pErrMsg,"输入字符串格式错误");
		return SB_ERR_UNSUPPORTED;
	}

	char temp[256]={0};
//	char tempw[256]={0};

	char data_item[100]={0};//存放数据项
	char data_DFEF[100]={0};//存放DF和EF
	char data_w[9000]={0};  //存放要写入的数据
	strcpy(data_w,pWriteDataBuff);


	char DDF[10]={0};
	char EEF[10]={0};

	int addr_data=0;//记录$的位置 EF的分隔符
	int addr_item =0;//记录|的位置  记录的分隔符  

//	int addr_dataw=0;//记录$的位置 EF的分隔符
	int addr_itemw =0;//记录|的位置  记录的分隔符 
	
	addr =0;
	int addr1=0,addr2=0;
	int addr3=0,addr4=0;
	int ret;

	unsigned char Tag[50]={0};
	unsigned char datalength[50]={0};
	unsigned char style[50]={0};

	int ct = getCount((char*)pFileAddr,'|');//获取|数量
	int ct1 = getCount((char*)pFileAddr,'$');//获取|数量

	int ct2 = getCount((char*)pWriteDataBuff,'|');//获取|数量

	if(ct2!=(ct-ct1))
	{
		if(ct==0 && ct1==ct2)
		{
		}
		else if(strcmp(pFileAddr,"MFEF08|00|$")==0 || strcmp(pFileAddr,"MFEF07|00|$")==0 || strcmp(pFileAddr,"DF01EF16|00|$")==0 ||strcmp(pFileAddr,"DF01EF17|00|$")==0 || strcmp(pFileAddr,"DF01EF18|00|$")==0 || strcmp(pFileAddr,"DF01EF19|00|$")==0 ||strcmp(pFileAddr,"DF01EF1A|00|$")==0)
		{}
		else
		{
			strcpy(pErrMsg,"输入字符串非法");
			return SB_ERR_UNSUPPORTED; 
		}
	}
/*
	if(GetAtrCityCode(pErrMsg)!=0)
	{
		return SB_ERR_TYPEERROR;
	}*/
	//MFEF05|07|$MFEF06|08|09|0A|$
	for(int i=0;i<count;i++)//DF范围内循环
	{
		addr1 = findChar((char*)pFileAddr,addr_data,'$');// pFileAddr = MFEF05|07|$MFEF06|08|09|0A|$
		memset(temp,0,100);
		memcpy(temp,pFileAddr+addr_data,addr1-addr_data);//获取DF和EF以及数据项  temp = MFEF05|07|08|
		addr_data=addr1+1;
	
		ad = findChar((char*)temp,0,':');
		addr = findChar((char*)temp,0,'|');//先对字符串格式进行判断

		int len,iCount,iCountw;
	//	iCount = getCount((char*)data_item,'|');//获取|数量
		if(ad>0 && addr>0 && addr<ad) //B-TLV格式
		{
			addr2 = findChar((char*)temp,0,'|');
			memcpy(data_DFEF,temp,addr2);//获取DF和EF  data_DFEF = DF02EF07

			len = strlen(data_DFEF);
			memcpy(data_item,temp+addr2+1,addr1-addr2-1);// data_item = 99:01|99:02|
			if(strlen(data_item)<6)
			{
				fileType =0;
				strcpy(pErrMsg,"输入字符串非法");
				return SB_ERR_UNSUPPORTED; 
			}
			iCount = getCount((char*)data_item,'|');//获取|数量
			fileType =2;
		}
		else if(ad>0 && addr<0)//循环文件 DF02EF07:01$DF02EF07:02$DF02EF07:03$DF02EF07:04$
		{
			addr2 = findChar((char*)temp,0,':');
			memcpy(data_DFEF,temp,addr2);//获取DF和EF  data_DFEF = DF02EF07

			len = strlen(data_DFEF);
			memcpy(data_item,temp+addr2+1,addr1-addr2-1);// data_item = 01|

			fileType = 3;
		}
		else if(ad<0 && addr>0)
		{
			addr2 = findChar((char*)temp,0,'|');
			memcpy(data_DFEF,temp,addr2);//获取DF和EF  data_DFEF = MFEF05
		//	strcat(data,data_DFEF);
		//	strcat(data,"|");
			len = strlen(data_DFEF);
			memcpy(data_item,temp+addr2+1,addr1-addr2-1);// data_item = 07|08|
			iCount = getCount((char*)data_item,'|');//获取|数量
			iCountw = getCount((char*)data_w,'|');//获取|数量
			fileType =1;
		}
		else
		{
			strcpy(pErrMsg,"输入字符串非法");
			return SB_ERR_UNSUPPORTED; 
		}
		if(len==6)//说明是MF
		{
			memset(DDF,0,10);
			memcpy(DDF,data_DFEF,2);
			memcpy(EEF,data_DFEF+2,4);
		}
		else if(len==8)
		{
			memcpy(DDF,data_DFEF,4);
			memcpy(EEF,data_DFEF+4,4);
		}
		else
		{
			strcpy(pErrMsg,"输入字符串格式错误");
			return SB_ERR_UNSUPPORTED;
		}

		if (strcmp(DDF,"MF")==0 || strcmp(DDF,"DDF1")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}

			if (strcmp(EEF,"EF05")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x12,0x04};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UKSSSE认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x01\x02\x03\x04\x05\x06\x07",7);
				memcpy(datalength,"\x10\x01\x04\x0C\x04\x04\x09",7);
				memcpy(style,"\x0F\x01\x01\x0F\x0F\x0F\x01",7);
				
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
	
					switch(recno)
					{
					case 0x01: ret = iWriteRecordFileEX("EF05",Tag,0,datalength,style,data);break;
					case 0x02: ret = iWriteRecordFileEX("EF05",Tag,1,datalength,style,data);break;
					case 0x03: ret = iWriteRecordFileEX("EF05",Tag,2,datalength,style,data);break;
					case 0x04: ret = iWriteRecordFileEX("EF05",Tag,3,datalength,style,data);break;
					case 0x05: ret = iWriteRecordFileEX("EF05",Tag,4,datalength,style,data);break;
					case 0x06: ret = iWriteRecordFileEX("EF05",Tag,5,datalength,style,data);break;
					case 0x07: ret = iWriteRecordFileEX("EF05",Tag,6,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				if(fileType!=1)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x12,0x04};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UKSSSE认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x08\x09\x4E\x0A\x0B\x0C\x0D",8);
				memcpy(datalength,"\x00\x12\x1E\x14\x01\x01\x03\x04",8);
				memcpy(style,"\x00\x01\x01\x01\x01\x0F\x0F\x0F",8);
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x08: ret = iWriteRecordFileEX("EF06",Tag,1,datalength,style,data);break;
					case 0x09: ret = iWriteRecordFileEX("EF06",Tag,2,datalength,style,data);break;
					case 0x4E: ret = iWriteRecordFileEX("EF06",Tag,3,datalength,style,data);break;
					case 0x0A: ret = iWriteRecordFileEX("EF06",Tag,4,datalength,style,data);break;
					case 0x0B: ret = iWriteRecordFileEX("EF06",Tag,5,datalength,style,data);break;
					case 0x0C: ret = iWriteRecordFileEX("EF06",Tag,6,datalength,style,data);break;
					case 0x0D: ret = iWriteRecordFileEX("EF06",Tag,7,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x12,0x04};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UKSSSE认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				int len = strlen(pWriteDataBuff);
				if(len%2!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				ret = iWriteBinary("EF07",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}
			else if (strcmp(EEF,"EF08")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x12,0x04};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UKSSSE认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				int len = strlen(pWriteDataBuff);
				if(len%2!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				ret = iWriteBinary("EF08",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}
			else if (strcmp(EEF,"EF0D")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x13,0x0D};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UKSSSE认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\xEA\xEB\xEC\xED",5);
				memcpy(datalength,"\x00\x0C\x03\x01\x01",5);
				memcpy(style,"\x00\x01\x0F\x01\x01",5);
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0xEA: ret = iWriteRecordFileEX("EF0D",Tag,1,datalength,style,data);break;
					case 0xEB: ret = iWriteRecordFileEX("EF0D",Tag,2,datalength,style,data);break;
					case 0xEC: ret = iWriteRecordFileEX("EF0D",Tag,3,datalength,style,data);break;
					case 0xED: ret = iWriteRecordFileEX("EF0D",Tag,4,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF0D结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}

		}//DDF结束符
		else if(strcmp(DDF,"DF01")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}
			if(SelectFile("DF01"))
			{
				strcpy(pErrMsg,"输入DF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}

			if (strcmp(EEF,"EF05")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x42,0x83};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK1DF01认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x20\x21\x0E",4);
				memcpy(datalength,"\x00\x02\x50\x03",4);
				memcpy(style,"\x00\x01\x01\x0F",4);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x20: ret = iWriteRecordFileEX("EF05",Tag,1,datalength,style,data);break;
					case 0x21: ret = iWriteRecordFileEX("EF05",Tag,2,datalength,style,data);break;
					case 0x0E: ret = iWriteRecordFileEX("EF05",Tag,3,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;	
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x25,0x86};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK4DF01认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x23\x24\x28\x2C\x2D",6);
				memcpy(datalength,"\x00\x50\x03\x0F\x32\x0F",6);
				memcpy(style,"\x00\x01\x0F\x01\x01\x01",6);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x23: ret = iWriteRecordFileEX("EF06",Tag,1,datalength,style,data);break;
					case 0x24: ret = iWriteRecordFileEX("EF06",Tag,2,datalength,style,data);break;
					case 0x28: ret = iWriteRecordFileEX("EF06",Tag,3,datalength,style,data);break;
					case 0x2C: ret = iWriteRecordFileEX("EF06",Tag,4,datalength,style,data);break;
					case 0x2D: ret = iWriteRecordFileEX("EF06",Tag,5,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x43,0x84};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK2DF01认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x29",2);
				memcpy(datalength,"\x00\x01",2);
				memcpy(style,"\x00\x01",2);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x29: ret = iWriteRecordFileEX("EF07",Tag,1,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF07结束符
			else if (strcmp(EEF,"EF08")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x24,0x85};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK3DF01认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x2B",2);
				memcpy(datalength,"\x00\x01",2);
				memcpy(style,"\x00\x01",2);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x2B: ret = iWriteRecordFileEX("EF08",Tag,1,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF08结束符
			else if (strcmp(EEF,"EF09")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x43,0x84};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK2DF01认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x2E\x30\x32",4);
				memcpy(datalength,"\x00\x46\x09\x02",4);
				memcpy(style,"\x00\x01\x01\x01",4);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x2E: ret = iWriteRecordFileEX("EF09",Tag,1,datalength,style,data);break;
					case 0x30: ret = iWriteRecordFileEX("EF09",Tag,2,datalength,style,data);break;
					case 0x32: ret = iWriteRecordFileEX("EF09",Tag,3,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF09结束符
			else if (strcmp(EEF,"EF0A")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x44,0x87};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK5DF01认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x37\x38\x39",4);
				memcpy(datalength,"\x00\x03\x02\x04",4);
				memcpy(style,"\x00\x01\x01\x0F",4);


				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x37: ret = iWriteRecordFileEX("EF0A",Tag,1,datalength,style,data);break;
					case 0x38: ret = iWriteRecordFileEX("EF0A",Tag,2,datalength,style,data);break;
					case 0x39: ret = iWriteRecordFileEX("EF0A",Tag,3,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF0A结束符
			else if (strcmp(EEF,"EF15")==0)//BTLV
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x2F,0x89};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK6DF01认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				TLV tlv[3]={{"57",0x03,AN_style},{"58",0x03,CN_style},{"59",0x28,AN_style}};
				memcpy(Tag,"\x00\x2A",2);
				memcpy(datalength,"\x00\x01",2);
				memcpy(style,"\x00\x0F",2);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						int addw = findChar((char*)data_w,addr_itemw,'|');
						memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
						
						switch(recno)
						{
						case 0x2A: ret = iWriteRecordFileEX("EF15",Tag,1,datalength,style,data);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,data);
							return ret;
						}
						addr_item=add+1;
						addr_itemw=addw+1;
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"40")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							
							ret = iWriteBTLVEX("EF15",recno+1,0x34,tlv,3,data,0x40);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;

						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}

				}//循环结束
				addr_item =0;
	
			}//EF15结束符
			else if (strcmp(EEF,"EF16")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x30,0x8A};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK7DF01认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				int len = strlen(pWriteDataBuff);
				if(len%2!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				ret = iWriteBinary("EF16",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}//EF16结束
			else if (strcmp(EEF,"EF17")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x31,0x8B};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK8DF01认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				int len = strlen(pWriteDataBuff);

				ret = iWriteBinary("EF17",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}
			else if (strcmp(EEF,"EF18")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x32,0x8C};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK9DF01认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				int len = strlen(pWriteDataBuff);
				if(len%2!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				ret = iWriteBinary("EF18",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}
			else if (strcmp(EEF,"EF19")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x33,0x8D};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UKADF01认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				int len = strlen(pWriteDataBuff);
				if(len%2!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				ret = iWriteBinary("EF19",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}
			else if (strcmp(EEF,"EF1A")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x34,0x8E};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UKBDF01认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				int len = strlen(pWriteDataBuff);
				if(len%2!=0)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				ret = iWriteBinary("EF1A",00,len/2,CN_style,pWriteDataBuff);
				if(ret!=0)
				{
					strcpy(pErrMsg,pWriteDataBuff);
					return ret;
				}
				addr_item=0;
			}
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}

		}//DF01结束
		else if(strcmp(DDF,"DF02")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}
			if(SelectFile("DF02"))
			{
				strcpy(pErrMsg,"输入DF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}

			if (strcmp(EEF,"EF05")==0)//BTLV
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x26,0x83};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK1DF02认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x42\x41",3);
				memcpy(datalength,"\x00\x03\x03",3);
				memcpy(style,"\x00\x01\x01",3);
				TLV tlv[5]={{"5A",0x07,AN_style},{"5B",0x01,CN_style},{"5C",0x46,AN_style},{"5D",0x04,CN_style},{"5E",0x10,AN_style}};
				TLV tlv1[4]={{"33",0x03,AN_style},{"34",0x46,AN_style},{"35",0x04,CN_style},{"36",0x10,AN_style}};
				
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						int addw = findChar((char*)data_w,addr_itemw,'|');
						memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
						
						switch(recno)
						{
						case 0x42: ret = iWriteRecordFileEX("EF05",Tag,1,datalength,style,data);break;
						case 0x41: ret = iWriteRecordFileEX("EF05",Tag,2,datalength,style,data);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,data);
							return ret;
						}
						addr_item=add+1;
						addr_itemw=addw+1;
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"43")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							
							ret = iWriteBTLVEX("EF05",recno+2,0x6C,tlv,5,data,0x43);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;
						}
						else if(strcmp(a,"44")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							
							ret = iWriteBTLVEX("EF05",recno+4,0x65,tlv1,4,data,0x44);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
				}//循环结束
				addr_item =0;	
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x45,0x84};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK2DF02认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x4C\x4B\x4D\x60\x4F\x50\x3A",8);
				memcpy(datalength,"\x00\x04\x05\x03\x04\x06\x03\x07",8);
				memcpy(style,"\x00\x0F\x01\x0F\x0F\x01\x0F\x01",8);


				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x4C: ret = iWriteRecordFileEX("EF06",Tag,1,datalength,style,data);break;
					case 0x4B: ret = iWriteRecordFileEX("EF06",Tag,2,datalength,style,data);break;
					case 0x4D: ret = iWriteRecordFileEX("EF06",Tag,3,datalength,style,data);break;
					case 0x60: ret = iWriteRecordFileEX("EF06",Tag,4,datalength,style,data);break;
					case 0x4F: ret = iWriteRecordFileEX("EF06",Tag,5,datalength,style,data);break;
					case 0x50: ret = iWriteRecordFileEX("EF06",Tag,6,datalength,style,data);break;
					case 0x3A: ret = iWriteRecordFileEX("EF06",Tag,7,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)//循环文件
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x46,0x85};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK3DF02认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
					char a[3]={0};
					char data[512]={0};
					char result[100]={0};
					memcpy(a,data_item,strlen(data_item));
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					if(!IsEffectChar(a))
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					if(getCount(data,',')!=4)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					CYC cy[4]={{0x07,AN_style},{0x04,CN_style},{0x04,CN_style},{0x46,AN_style}};
					ret = iWriteXHFileEX("EF07",recno,0x55,cy,4,data);
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
				addr_itemw=addw+1;
				addr_item=0;

			}//EF07结束符
			else if (strcmp(EEF,"EF09")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x27,0x86};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK4DF02认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x55\x56\x96\x97",5);
				memcpy(datalength,"\x00\x10\x46\x04\x03",5);
				memcpy(style,"\x00\x01\x01\x0F\x0F",5);


				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);
					
					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0x55: ret = iWriteRecordFileEX("EF09",Tag,1,datalength,style,data);break;
					case 0x56: ret = iWriteRecordFileEX("EF09",Tag,2,datalength,style,data);break;
					case 0x96: ret = iWriteRecordFileEX("EF09",Tag,3,datalength,style,data);break;
					case 0x97: ret = iWriteRecordFileEX("EF09",Tag,4,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF09结束符
			else if (strcmp(EEF,"EF15")==0)//BTLV
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x28,0x87};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK5DF02认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				TLV tlv[3]={{"10",0x03,AN_style},{"11",0x04,CN_style},{"12",0x03,CN_style}};
				memcpy(Tag,"\x00\x00\x00\x00\x00\x0F\x1F",7);
				memcpy(datalength,"\x00\x00\x00\x00\x00\x04\x03",7);
				memcpy(style,"\x00\x00\x00\x00\x00\x0F\x0F",7);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						int addw = findChar((char*)data_w,addr_itemw,'|');
						memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
						
						switch(recno)
						{
						case 0x0F: ret = iWriteRecordFileEX("EF15",Tag,5,datalength,style,data);break;
						case 0x1F: ret = iWriteRecordFileEX("EF15",Tag,6,datalength,style,data);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,data);
							return ret;
						}
						addr_item=add+1;
						addr_itemw=addw+1;
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"99")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							
							ret = iWriteBTLVEX("EF15",recno,0x10,tlv,3,data,0x99);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;

						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}

				}//循环结束
				addr_item =0;

			}//EF15结束符
			else if (strcmp(EEF,"EF16")==0)//循环文件
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x36,0x89};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK6DF02认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
					char a[3]={0};
					char data[512]={0};
					char result[100]={0};
					memcpy(a,data_item,strlen(data_item));
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					if(!IsEffectChar(a))
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					if(getCount(data,',')!=5)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					CYC cy[5]={{0x03,AN_style},{0x04,CN_style},{0x04,CN_style},{0x04,CN_style},{0x03,CN_style}};
					ret = iWriteXHFileEX("EF16",recno,0x12,cy,5,data);
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
			//	addr_item=add+1;
				addr_itemw=addw+1;
				addr_item=0;
			}//EF16结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF02结束

		else if(strcmp(DDF,"DF03")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}
			if(SelectFile("DF03"))
			{
				strcpy(pErrMsg,"输入DF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}

			if (strcmp(EEF,"EF05")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x47,0x86};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK1DF03认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x61\x98\x62\x63\x64\x65\x66",8);
				memcpy(datalength,"\x00\x03\x04\x04\x03\x03\x03\x03",8);
				memcpy(style,"\x00\x0F\x0F\x0F\x01\x01\x01\x01",8);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x61: ret = iWriteRecordFileEX("EF05",Tag,1,datalength,style,data);break;
					case 0x98: ret = iWriteRecordFileEX("EF05",Tag,2,datalength,style,data);break;
					case 0x62: ret = iWriteRecordFileEX("EF05",Tag,3,datalength,style,data);break;
					case 0x63: ret = iWriteRecordFileEX("EF05",Tag,4,datalength,style,data);break;
					case 0x64: ret = iWriteRecordFileEX("EF05",Tag,5,datalength,style,data);break;
					case 0x65: ret = iWriteRecordFileEX("EF05",Tag,6,datalength,style,data);break;
					case 0x66: ret = iWriteRecordFileEX("EF05",Tag,7,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)//BTLV
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x29,0x87};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK2DF03认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				TLV tlv[5]={{"13",0x02,AN_style},{"14",0x01,AN_style},{"15",0x14,AN_style},{"16",0x04,CN_style},{"17",0x3C,AN_style}};
				memcpy(Tag,"\x00\x45\x46\x47\x48\x49\x67\x6B",8);
				memcpy(datalength,"\x00\x14\x02\x02\x01\x01\x04\x3C",8);
				memcpy(style,"\x00\x01\x01\x01\x01\x01\x0F\x01",8);
				
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						int addw = findChar((char*)data_w,addr_itemw,'|');
						memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
						
						switch(recno)
						{
						case 0x45: ret = iWriteRecordFileEX("EF06",Tag,1,datalength,style,data);break;
						case 0x46: ret = iWriteRecordFileEX("EF06",Tag,2,datalength,style,data);break;
						case 0x47: ret = iWriteRecordFileEX("EF06",Tag,3,datalength,style,data);break;
						case 0x48: ret = iWriteRecordFileEX("EF06",Tag,4,datalength,style,data);break;
						case 0x49: ret = iWriteRecordFileEX("EF06",Tag,5,datalength,style,data);break;
						case 0x67: ret = iWriteRecordFileEX("EF06",Tag,6,datalength,style,data);break;
						case 0x6B: ret = iWriteRecordFileEX("EF06",Tag,7,datalength,style,data);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,data);
							return ret;
						}
						addr_item=add+1;
						addr_itemw=addw+1;
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"4A")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							
							ret = iWriteBTLVEX("EF06",recno+7,0x61,tlv,5,data,0x4A);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;

						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}

				}//循环结束
				addr_item =0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x48,0x88};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK3DF03认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x70\x71\x6E\x6C\x6F\x73",7);
				memcpy(datalength,"\x00\x03\x03\x04\x04\x04\x04",7);
				memcpy(style,"\x00\x01\x0F\x0F\x0F\x0F\x0F",7);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x70: ret = iWriteRecordFileEX("EF07",Tag,1,datalength,style,data);break;
					case 0x71: ret = iWriteRecordFileEX("EF07",Tag,2,datalength,style,data);break;
					case 0x6E: ret = iWriteRecordFileEX("EF07",Tag,3,datalength,style,data);break;
					case 0x6C: ret = iWriteRecordFileEX("EF07",Tag,4,datalength,style,data);break;
					case 0x6F: ret = iWriteRecordFileEX("EF07",Tag,5,datalength,style,data);break;
					case 0x73: ret = iWriteRecordFileEX("EF07",Tag,6,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;

			}//EF07结束符
			else if (strcmp(EEF,"EF15")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x37,0x89};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK4DF03认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x7A\x7B\x7C",4);
				memcpy(datalength,"\x00\x03\x04\x04",4);
				memcpy(style,"\x00\x0F\x0F\x0F",4);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x7A: ret = iWriteRecordFileEX("EF15",Tag,1,datalength,style,data);break;
					case 0x7B: ret = iWriteRecordFileEX("EF15",Tag,2,datalength,style,data);break;
					case 0x7C: ret = iWriteRecordFileEX("EF15",Tag,3,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF15结束符
			else if (strcmp(EEF,"EF16")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x38,0x8A};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK5DF03认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x51\x5F",3);
				memcpy(datalength,"\x00\x03\x01",3);
				memcpy(style,"\x00\x0F\x01",3);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x51: ret = iWriteRecordFileEX("EF16",Tag,1,datalength,style,data);break;
					case 0x5F: ret = iWriteRecordFileEX("EF16",Tag,2,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF16结束符
			else if (strcmp(EEF,"EF17")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x39,0x8B};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK6DF03认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x3B\x3C\x3D\x3E\x3F",6);
				memcpy(datalength,"\x00\x14\x46\x01\x04\x3C",6);
				memcpy(style,"\x00\x01\x01\x01\x0F\x01",6);


				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x3B: ret = iWriteRecordFileEX("EF17",Tag,1,datalength,style,data);break;
					case 0x3C: ret = iWriteRecordFileEX("EF17",Tag,2,datalength,style,data);break;
					case 0x3D: ret = iWriteRecordFileEX("EF17",Tag,3,datalength,style,data);break;
					case 0x3E: ret = iWriteRecordFileEX("EF17",Tag,4,datalength,style,data);break;
					case 0x3F: ret = iWriteRecordFileEX("EF17",Tag,5,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF17结束符
			else if (strcmp(EEF,"EF18")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x3A,0x8C};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK7DF03认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x74\x75\x76\x77\x78",6);
				memcpy(datalength,"\x00\x03\x03\x01\x04\x04",6);
				memcpy(style,"\x00\x01\x0F\x0F\x0F\x0F",6);


				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x74: ret = iWriteRecordFileEX("EF18",Tag,1,datalength,style,data);break;
					case 0x75: ret = iWriteRecordFileEX("EF18",Tag,2,datalength,style,data);break;
					case 0x76: ret = iWriteRecordFileEX("EF18",Tag,3,datalength,style,data);break;
					case 0x77: ret = iWriteRecordFileEX("EF18",Tag,4,datalength,style,data);break;
					case 0x78: ret = iWriteRecordFileEX("EF18",Tag,5,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF18结束符
			else if (strcmp(EEF,"EF19")==0)//BTLV
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x3B,0x8D};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK8DF03认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				TLV tlv[7]={{"18",0x03,AN_style},{"19",0x03,CN_style},{"1A",0x04,CN_style},{"1B",0x04,CN_style},{"1C",0x04,CN_style},{"1D",0x03,AN_style},{"1E",0x04,CN_style}};

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"79")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							
							ret = iWriteBTLVEX("EF19",recno,0x27,tlv,7,data,0x79);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;

						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}

				}//循环结束
				addr_item =0;
			}//EF19结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF03结束
		else if(strcmp(DDF,"DF04")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}
			if(SelectFile("DF04"))
			{
				strcpy(pErrMsg,"输入DF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}

			if (strcmp(EEF,"EF05")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x49,0x85};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK1DF04认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x81\x84\x87\x8C\x80\x8B\x8A\x83\x86\x89\x7D\x7E\x7F\x8D\x8E\x8F",17);
				memcpy(datalength,"\x00\x08\x03\x11\x01\x04\x1D\x0F\x09\x09\x09\x09\x09\x09\x09\x09\x01",17);
				memcpy(style,"\x00\x01\x0F\x01\x0F\x0F\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01",17);
				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x81: ret = iWriteRecordFileEX("EF05",Tag,1,datalength,style,data);break;
					case 0x84: ret = iWriteRecordFileEX("EF05",Tag,2,datalength,style,data);break;
					case 0x87: ret = iWriteRecordFileEX("EF05",Tag,3,datalength,style,data);break;
					case 0x8C: ret = iWriteRecordFileEX("EF05",Tag,4,datalength,style,data);break;
					case 0x80: ret = iWriteRecordFileEX("EF05",Tag,5,datalength,style,data);break;
					case 0x8B: ret = iWriteRecordFileEX("EF05",Tag,6,datalength,style,data);break;
					case 0x8A: ret = iWriteRecordFileEX("EF05",Tag,7,datalength,style,data);break;
					case 0x83: ret = iWriteRecordFileEX("EF05",Tag,8,datalength,style,data);break;
					case 0x86: ret = iWriteRecordFileEX("EF05",Tag,9,datalength,style,data);break;
					case 0x89: ret = iWriteRecordFileEX("EF05",Tag,10,datalength,style,data);break;
					case 0x7D: ret = iWriteRecordFileEX("EF05",Tag,11,datalength,style,data);break;
					case 0x7E: ret = iWriteRecordFileEX("EF05",Tag,12,datalength,style,data);break;
					case 0x7F: ret = iWriteRecordFileEX("EF05",Tag,13,datalength,style,data);break;
					case 0x8D: ret = iWriteRecordFileEX("EF05",Tag,14,datalength,style,data);break;
					case 0x8E: ret = iWriteRecordFileEX("EF05",Tag,15,datalength,style,data);break;
					case 0x8F: ret = iWriteRecordFileEX("EF05",Tag,16,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
				
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x2A,0x86};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK2DF04认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\x90\x92\x93",4);
				memcpy(datalength,"\x00\x01\x04\x02",4);
				memcpy(style,"\x00\x01\x0F\x01",4);


				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0x90: ret = iWriteRecordFileEX("EF06",Tag,1,datalength,style,data);break;
					case 0x92: ret = iWriteRecordFileEX("EF06",Tag,2,datalength,style,data);break;
					case 0x93: ret = iWriteRecordFileEX("EF06",Tag,3,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF08")==0)//循环文件
			{	
				strcpy(pErrMsg,"输入字符串格式错误");
				return SB_ERR_UNSUPPORTED;
			}//EF08结束符
			else if (strcmp(EEF,"EF15")==0)//循环文件
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x2A,0x86};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK2DF04认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				if(fileType!=3)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				char a[3]={0};
				char data[512]={0};
				char result[100]={0};
				memcpy(a,data_item,strlen(data_item));
				if(strlen(a)!=2)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				if(!IsEffectChar(a))
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				int recno = chartoint1(a[0])*16+chartoint1(a[1]);

				int addw = findChar((char*)data_w,addr_itemw,'|');
				memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
				if(getCount(data,',')!=2)
				{
					strcpy(pErrMsg,"输入字符串格式错误");
					return SB_ERR_UNSUPPORTED; 
				}
				CYC cy[2]={{0x02,CN_style},{0x01,AN_style}};
				ret = iWriteXHFileEX("EF15",recno,0x03,cy,2,data);
				if(ret!=0)
				{
					strcpy(pErrMsg,data);
					return ret;
				}
				addr_itemw=addw+1;
				addr_item=0;
				
			}//EF15结束符
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF04结束
		else if(strcmp(DDF,"DF05")==0)
		{
			if(strcmp(EEF,"EF05")==0)
			{
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0xB4,0x83};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK1DF05认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				TLV tlv1[2]={{"A5",0x10,AN_style},{"A6",0x04,AN_style}};
				TLV tlv2[2]={{"A7",0x10,AN_style},{"A8",0x03,AN_style}};
				
				memcpy(Tag,"\x00\xA0\xA1\xB9\xA2\xA9\xBA\xBB\xAA\xAB\xAC\AD\xAE\xAF\xB0\xB1\xB2\xB3\xB4\xB5\xB6\xB7\xBC\xB8",24);
				memcpy(datalength,"\x00\x01\x01\x01\x04\x01\x14\x64\x14\x04\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x28",24);
				memcpy(style,"\x00\x01\x01\x01\x01\x0F\x01\x01\x01\x0F\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01\x01",8);
				
				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						int addw = findChar((char*)data_w,addr_itemw,'|');
						memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
						
						switch(recno)
						{
						case 0xA0: ret = iWriteRecordFileEX("EF05",Tag,1,datalength,style,data);break;
						case 0xA1: ret = iWriteRecordFileEX("EF05",Tag,2,datalength,style,data);break;
						case 0xB9: ret = iWriteRecordFileEX("EF05",Tag,3,datalength,style,data);break;
						case 0xA2: ret = iWriteRecordFileEX("EF05",Tag,4,datalength,style,data);break;
						case 0xA9: ret = iWriteRecordFileEX("EF05",Tag,5,datalength,style,data);break;
						case 0xBA: ret = iWriteRecordFileEX("EF05",Tag,6,datalength,style,data);break;
						case 0xBB: ret = iWriteRecordFileEX("EF05",Tag,7,datalength,style,data);break;
						case 0xAA: ret = iWriteRecordFileEX("EF05",Tag,8,datalength,style,data);break;
						case 0xAB: ret = iWriteRecordFileEX("EF05",Tag,9,datalength,style,data);break;
						case 0xAC: ret = iWriteRecordFileEX("EF05",Tag,10,datalength,style,data);break;
						case 0xAD: ret = iWriteRecordFileEX("EF05",Tag,11,datalength,style,data);break;
						case 0xAE: ret = iWriteRecordFileEX("EF05",Tag,12,datalength,style,data);break;
						case 0xAF: ret = iWriteRecordFileEX("EF05",Tag,13,datalength,style,data);break;
						case 0xB0: ret = iWriteRecordFileEX("EF05",Tag,14,datalength,style,data);break;
						case 0xB1: ret = iWriteRecordFileEX("EF05",Tag,15,datalength,style,data);break;		
						case 0xB2: ret = iWriteRecordFileEX("EF05",Tag,16,datalength,style,data);break;
						case 0xB3: ret = iWriteRecordFileEX("EF05",Tag,17,datalength,style,data);break;			
						case 0xB4: ret = iWriteRecordFileEX("EF05",Tag,18,datalength,style,data);break;
						case 0xB5: ret = iWriteRecordFileEX("EF05",Tag,19,datalength,style,data);break;	
						case 0xB6: ret = iWriteRecordFileEX("EF05",Tag,20,datalength,style,data);break;
						case 0xB7: ret = iWriteRecordFileEX("EF05",Tag,21,datalength,style,data);break;
						case 0xBC: ret = iWriteRecordFileEX("EF05",Tag,22,datalength,style,data);break;
						case 0xB8: ret = iWriteRecordFileEX("EF05",Tag,23,datalength,style,data);break;
						
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,data);
							return ret;
						}
						addr_item=add+1;
						addr_itemw=addw+1;
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"A3")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
							ret = iWriteBTLVEX("EF05",recno+5,0x18,tlv1,2,data,0xA3);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;

						}
						if(strcmp(a,"A4")==0 )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

							ret = iWriteBTLVEX("EF05",recno+10,0x17,tlv2,2,data,0xA4);
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入EF字符串错误");
						return SB_ERR_UNSUPPORTED; 
					}
				}//循环结束
				addr_item =0;
			}
		}//DF05
		else if(strcmp(DDF,"DF06")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}
			if(SelectFile("DF06"))
			{
				strcpy(pErrMsg,"输入DF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
			if (strcmp(EEF,"EF05")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x53,0x85};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK1DF06认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				TLV tlv[5]={{"E3",0x01,CN_style},{"E4",0x04,CN_style},{"E5",0x04,CN_style},{"E6",0x04,CN_style},{"E7",0x03,CN_style}};
				memcpy(Tag,"\x00\xC0\xC2",3);
				memcpy(datalength,"\x00\x46\x09",3);
				memcpy(style,"\x00\x01\x01",3);

				for(int n=0;n<iCount;n++)//|范围内循环
				{
					char a[13]={0};
					char data[256]={0};
					char result[0x100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memset(a,0,13);
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)==2)
					{
						int recno = chartoint1(a[0])*16+chartoint1(a[1]);
						int addw = findChar((char*)data_w,addr_itemw,'|');
						memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据	
						switch(recno)
						{
						case 0xC0: ret = iWriteRecordFileEX("EF05",Tag,1,datalength,style,data);break;
						case 0xC2: ret = iWriteRecordFileEX("EF05",Tag,2,datalength,style,data);break;
						default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
						}
						if(ret!=0)
						{
							strcpy(pErrMsg,data);
							return ret;
						}
						addr_item=add+1;
						addr_itemw=addw+1;
					}
					else if(strlen(a)==5)
					{
						add = findChar((char*)data_item,addr_item,':');
						if(add<0)
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return ret;
						}
						memset(a,0,13);
						memcpy(a,data_item+addr_item,add-addr_item);
						if(strcmp(a,"C3")==0 || strcmp(a,"C4")==0||strcmp(a,"C5")==0  )
						{
							addr_item+=3;
							add = findChar((char*)data_item,addr_item,'|');
							memset(a,0,13);
							memcpy(a,data_item+addr_item,add-addr_item);
							if(strlen(a)!=2)
							{
								strcpy(pErrMsg,"输入字符串格式错误");
								return ret;
							}
							int recno = chartoint1(a[0])*16+chartoint1(a[1]);

							int addw = findChar((char*)data_w,addr_itemw,'|');
							memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

							ret = iWriteBTLVEX("EF05",recno+2,0x1A,tlv,5,data,chartoint1(a[0])*16+chartoint1(a[1]));
							if(ret!=0)
							{
								strcpy(pErrMsg,data);
								return ret;
							}
							addr_itemw=addw+1;
							addr_item=add+1;
						}
						else
						{
							strcpy(pErrMsg,"输入字符串格式错误");
							return SB_ERR_UNSUPPORTED; 
						}
					}
					else
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
				}//循环结束
				addr_item =0;
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x48,0x53,0x85};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK1DF06认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\xC6\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF\xD0\xD1\xD2\xD3\xD4\xD5\xD6\xD7\xD8\xD9\xDA\xD8\xDC\xDD\xDE\xDF\xE0\xE1\xE2",29);
				memcpy(datalength,"\x46\x09\x01\x04\x04\x04\x02\x01\x04\x04\x04\x03\x01\x04\x04\x04\x03\x01\x04\x04\x04\x04\x04\x04\x03\x03\x02\x02",29);
				memcpy(style,"\x00\x01\x01\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\x0F\0F\x0F",29);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据
					
					switch(recno)
					{
					case 0xC6:ret = iWriteRecordFileEX("EF06",Tag,1,datalength,style,data);break;
					case 0xC8:ret = iWriteRecordFileEX("EF06",Tag,2,datalength,style,data);break;
					case 0xC9:ret = iWriteRecordFileEX("EF06",Tag,3,datalength,style,data);break;
					case 0xCA:ret = iWriteRecordFileEX("EF06",Tag,4,datalength,style,data);break;
					case 0xCB:ret = iWriteRecordFileEX("EF06",Tag,5,datalength,style,data);break;

					case 0xCC:ret = iWriteRecordFileEX("EF06",Tag,6,datalength,style,data);break;
					case 0xCD:ret = iWriteRecordFileEX("EF06",Tag,7,datalength,style,data);break;
					case 0xCE:ret = iWriteRecordFileEX("EF06",Tag,8,datalength,style,data);break;
					case 0xCF:ret = iWriteRecordFileEX("EF06",Tag,9,datalength,style,data);break;
					case 0xD0:ret = iWriteRecordFileEX("EF06",Tag,10,datalength,style,data);break;

					case 0xD1:ret = iWriteRecordFileEX("EF06",Tag,11,datalength,style,data);break;
					case 0xD2:ret = iWriteRecordFileEX("EF06",Tag,12,datalength,style,data);break;
					case 0xD3:ret = iWriteRecordFileEX("EF06",Tag,13,datalength,style,data);break;
					case 0xD4:ret = iWriteRecordFileEX("EF06",Tag,14,datalength,style,data);break;
					case 0xD5:ret = iWriteRecordFileEX("EF06",Tag,15,datalength,style,data);break;
					case 0xD6:ret = iWriteRecordFileEX("EF06",Tag,16,datalength,style,data);break;

					case 0xD7:ret = iWriteRecordFileEX("EF06",Tag,17,datalength,style,data);break;
					case 0xD8:ret = iWriteRecordFileEX("EF06",Tag,18,datalength,style,data);break;
					case 0xD9:ret = iWriteRecordFileEX("EF06",Tag,19,datalength,style,data);break;
					case 0xDA:ret = iWriteRecordFileEX("EF06",Tag,20,datalength,style,data);break;
					case 0xDB:ret = iWriteRecordFileEX("EF06",Tag,21,datalength,style,data);break;
					case 0xDC:ret = iWriteRecordFileEX("EF06",Tag,22,datalength,style,data);break;
					case 0xDD:ret = iWriteRecordFileEX("EF06",Tag,23,datalength,style,data);break;
					case 0xDE:ret = iWriteRecordFileEX("EF06",Tag,24,datalength,style,data);break;
					case 0xDF:ret = iWriteRecordFileEX("EF06",Tag,25,datalength,style,data);break;
					case 0xE0:ret = iWriteRecordFileEX("EF06",Tag,26,datalength,style,data);break;
					case 0xE1:ret = iWriteRecordFileEX("EF06",Tag,27,datalength,style,data);break;
					case 0xE2:ret = iWriteRecordFileEX("EF06",Tag,28,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}
			else 
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF06结束
		else if(strcmp(DDF,"DF07")==0)
		{
			if(SelectFile("DDF1"))
			{
				strcpy(pErrMsg,"卡未上电");
				return SB_ERR_UNSUPPORTED; 
			}
			if(SelectFile("DF07"))
			{
				strcpy(pErrMsg,"输入DF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}

			if (strcmp(EEF,"EF05")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0x3E,0x86};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK1DF07认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\xF0\xF1\xF2\xF3\xF4",6);
				memcpy(datalength,"\x00\x02\x01\x04\x03\x04",6);
				memcpy(style,"\x00\x01\x01\x0F\x01\x0F",6);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0xF0: ret = iWriteRecordFileEX("EF05",Tag,1,datalength,style,data);break;
					case 0xF1: ret = iWriteRecordFileEX("EF05",Tag,2,datalength,style,data);break;
					case 0xF2: ret = iWriteRecordFileEX("EF05",Tag,3,datalength,style,data);break;
					case 0xF3: ret = iWriteRecordFileEX("EF05",Tag,4,datalength,style,data);break;
					case 0xF4: ret = iWriteRecordFileEX("EF05",Tag,5,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF05结束符 
			else if (strcmp(EEF,"EF06")==0)
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0xB0,0x87};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK2DF07认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\xF5\xF6\xF7",4);
				memcpy(datalength,"\x00\x03\x04\x46",4);
				memcpy(style,"\x00\x01\x0F\x01",4);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0xF5: ret = iWriteRecordFileEX("EF06",Tag,1,datalength,style,data);break;
					case 0xF6: ret = iWriteRecordFileEX("EF06",Tag,2,datalength,style,data);break;
					case 0xF7: ret = iWriteRecordFileEX("EF06",Tag,3,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
			}//EF06结束符 
			else if (strcmp(EEF,"EF07")==0)//循环文件
			{	
				char err[20]={0};
				unsigned char wordkey[4]={0x47,0xB2,0x88};
				if(ExterAuthen(wordkey,2,err))
				{
					strcpy(pErrMsg,"UK3DF07认证失败!");
					strcat(pErrMsg,err);
					return SB_ERR_AUTH;
				}
				memcpy(Tag,"\x00\xF8\xF9",3);
				memcpy(datalength,"\x00\x04\x01",3);
				memcpy(style,"\x00\x0F\x01",3);

				for(int n=0;n<iCount;n++)//范围内循环
				{
					char a[3]={0};
					char data[256]={0};
					char result[100]={0};
					int add = findChar((char*)data_item,addr_item,'|');
					if(add<2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					memcpy(a,data_item+addr_item,add-addr_item);
					if(strlen(a)!=2)
					{
						strcpy(pErrMsg,"输入字符串格式错误");
						return SB_ERR_UNSUPPORTED; 
					}
					int recno = chartoint1(a[0])*16+chartoint1(a[1]);

					int addw = findChar((char*)data_w,addr_itemw,'|');
					memcpy(data,data_w+addr_itemw,addw-addr_itemw); //获取到写入卡片的数据

					switch(recno)
					{
					case 0xF8: ret = iWriteRecordFileEX("EF07",Tag,1,datalength,style,data);break;
					case 0xF9: ret = iWriteRecordFileEX("EF07",Tag,2,datalength,style,data);break;
					default:strcpy(pErrMsg,"输入字符串格式错误");return SB_ERR_UNSUPPORTED;
					}
					if(ret!=0)
					{
						strcpy(pErrMsg,data);
						return ret;
					}
					addr_item=add+1;
					addr_itemw=addw+1;
				}
				addr_item=0;
				
			}//EF07结束符	
			else
			{
				strcpy(pErrMsg,"输入EF字符串错误");
				return SB_ERR_UNSUPPORTED; 
			}
		}//DF07结束
		else 
		{
			strcpy(pErrMsg,"输入DF字符串错误");
			return SB_ERR_UNSUPPORTED; 
		}

	}//$循环结束符
	return SB_ERR_OK;
}

int UnicodeToAnsi(unsigned short *Src, char *Des, int start, int len)
{
	wchar_t wsztemp[70]={0};
//	char szStr[100];
	for(int i=0;i<len/2;i++)
	{
		wsztemp[i]=Src[2*i]+(Src[2*i+1]<<8);		
	}
	sprintf(Des, "%S", wsztemp);
	return 0;
}
int DecodeToBmpFile(unsigned char * wltFile, const char * bmpFile)
{

	ofstream out(bmpFile, ios_base::binary);
	out.write((char *)wltFile, 38862);
	out.close();

	return 0;
}

int iReadIDMsg(int pReaderPort, const char* pBmpFile, char *pName, char *pSex, char *pNation, char *pBirth, char *pAddress, char *pCertNo, char *pDepartment , char *pExpire,char * pErrMsg)
{
	if(g_handle>0)
	{
		strcpy(pErrMsg,"读写器已打开，请选关闭读写器");
		return SB_ERR_CONNECT;
	}
	
	int pReaderHandle=0;
	int re = iOpenPort( pErrMsg);
	if(re!=0)
		return re;

	char Response1[1300]={0};
	unsigned short Response[1300]={0};

	re= PICC_Reader_ID_Request((long)pReaderHandle);//身份证寻卡
	if(re!=0)
	{
		strcpy(pErrMsg,"无卡");
		return SB_ERR_CARDMISS;
	}

	re =  PICC_Reader_ID_Select((long)pReaderHandle);//身份证选卡
	if(re!=0)
		return IFD_ICC_NoResponse;

	re = PICC_Reader_ID_Read((long)pReaderHandle,(unsigned char*)Response1);//身份证读卡
	if(re<0)
		return IFD_ICC_NoResponse;	

	for(int i=0;i<1300;i++)
	{
		Response[i]=Response1[i];
		Response[i] = Response[i]<<8;
		Response[i] = Response[i]>>8;
	}

	setlocale(LC_CTYPE, "chs"); 

	char name[16]={0};
	UnicodeToAnsi(Response,name,0,30);
	for(i=0;i<strlen(name);i++)
	{
		if(name[i]==' ')
		{
			name[i]=0;
			break;
		}
	}
	strcpy(pName,name);

	char sex[3]={0};
	UnicodeToAnsi(Response+30,sex,0,2);
	if(sex[0]==0x31)
		strcpy(pSex,"男");
	else if(sex[0]==0x32)
		strcpy(pSex,"女");
	else
		strcpy(pSex,"其他");

	char nation[5]={0};
	UnicodeToAnsi(Response+32,nation,0,4);
	int na = atoi(nation);
	string m_nation;
	switch(na)
	{
		case 01:m_nation = "汉";break;
		case 02:m_nation = "蒙古";break;
		case 03:m_nation = "回";break;
		case 04:m_nation = "藏";break;
		case 05:m_nation = "维吾尔";break;
		case 06:m_nation = "苗";break;
		case 07:m_nation = "彝";break;
		case 8:m_nation = "壮";break;
		case 9:m_nation = "布依";break;
		case 10:m_nation = "朝鲜";break;
		case 11:m_nation = "满";break;
		case 12:m_nation = "侗";break;
		case 13:m_nation = "瑶";break;
		case 14:m_nation = "白";break;
		case 15:m_nation = "土家";break;
		case 16:m_nation = "哈尼";break;
		case 17:m_nation = "哈萨克";break;
		case 18:m_nation = "傣";break;
		case 19:m_nation = "黎";break;
		case 20:m_nation = "傈僳";break;
		case 21:m_nation = "佤";break;
		case 22:m_nation = "畲";break;
		case 23:m_nation = "高山";break;
		case 24:m_nation = "拉祜";break;
		case 25:m_nation = "水";break;
		case 26:m_nation = "东乡";break;
		case 27:m_nation = "纳西";break;
		case 28:m_nation = "景颇";break;
		case 29:m_nation = "柯尔克孜";break;
		case 30:m_nation = "土";break;
		case 31:m_nation = "达斡尔";break;
		case 32:m_nation = "仫佬";break;
		case 33:m_nation = "羌";break;
		case 34:m_nation = "布朗";break;
		case 35:m_nation = "撒拉";break;
		case 36:m_nation = "毛南";break;
		case 37:m_nation = "仡佬";break;
		case 38:m_nation = "锡伯";break;
		case 39:m_nation = "阿昌";break;
		case 40:m_nation = "普米";break;
		case 41:m_nation = "塔吉克";break;
		case 42:m_nation = "怒";break;
		case 43:m_nation = "乌孜别克";break;
		case 44:m_nation = "俄罗斯";break;
		case 45:m_nation = "鄂温克";break;
		case 46:m_nation = "德昂";break;
		case 47:m_nation = "保安";break;
		case 48:m_nation = "裕固";break;
		case 49:m_nation = "京";break;
		case 50:m_nation = "塔塔尔";break;
		case 51:m_nation = "独龙";break;
		case 52:m_nation = "鄂伦春";break;
		case 53:m_nation = "赫哲";break;
		case 54:m_nation = "门巴";break;
		case 55:m_nation = "珞巴";break;
		case 56:m_nation = "基诺";break;

		default:break;

	}
	strcpy((char*)pNation,(char*)m_nation.c_str());

	char born[17]={0};
	UnicodeToAnsi(Response+36,born,0,16);
	strcpy(pBirth,born);


	char address[71]={0};
	UnicodeToAnsi(Response+52,address,0,70);
	strcpy(pAddress,address);

	char ID[37]={0};
	UnicodeToAnsi(Response+122,ID,0,36);
	strcpy(pCertNo,ID);

	char organ[31]={0};
	UnicodeToAnsi(Response+158,organ,0,30);
	strcpy(pDepartment,organ);

	char ITime[17]={0};
	UnicodeToAnsi(Response+188,ITime,0,16);
//	m_effectedDate = ITime;
//	m_effectedDate.Remove(' ');

    char NTime[17]={0};
	UnicodeToAnsi(Response+204,NTime,0,16);
	strcpy(pExpire,NTime);
//	m_expiredDate = NTime;
//	m_expiredDate.Remove(' ');

	char Naddr[37]={0};
	UnicodeToAnsi(Response+220,Naddr,0,36);


	unsigned char tupian[1300]={0};
	memcpy(tupian,Response1+256,1024);
	
	if(pBmpFile==NULL)
		return 0;

	TCHAR szExePath[MAX_PATH];
	TCHAR* m_lpMove;
	typedef int(PASCAL *lUnpack)(UCHAR *wlt, UCHAR *bmp);
	lUnpack Unpack;
	HINSTANCE WLTdllHandle;

	WLTdllHandle=LoadLibrary(".\\UnPack.dll");
	if (WLTdllHandle!=NULL)
	{
		Unpack=(lUnpack)GetProcAddress(WLTdllHandle,"Unpack");
		UCHAR *bmp = new UCHAR[38862];//解码后bmp数据待存入的数组
//		UCHAR *wlt = new UCHAR[1024];//wlt数据写入此数组中 
	//	tupian[0]=0x01;
		int ret = Unpack(tupian ,bmp); 
		
		char bmpfile[300]={0};
		int len = strlen(pBmpFile);
		int j=0;
		for(int i=0;i<len;i++)
		{
			if(pBmpFile[i]=='/')
			{
				bmpfile[j++] ='\\';
				bmpfile[j++] ='\\';
			}
			else if(pBmpFile[i]=='\\')
			{
				bmpfile[j++] =pBmpFile[i];
				if(pBmpFile[i+1]=='\\')
					bmpfile[j++] =pBmpFile[i];
				else
					bmpfile[j++] ='\\';
			}
			else
			{
				bmpfile[j++] =pBmpFile[i]; 
			}
		}
//		strcpy((char*)bmpfile,"D:\\zp.bmp");
		if(pBmpFile!=NULL)
			ret = DecodeToBmpFile(bmp, bmpfile);
	}
	FreeLibrary(WLTdllHandle);
//	strcpy((char*)sBmpFilepath,"D:\\zp.bmp");

	return SB_ERR_OK;
}

int iReadCardEX(const char* pInputPin, const char* pFileAddr, char* pOutDataBuff,char* pErrMsg)
{
	if(g_handle>0)
	{
		strcpy(pErrMsg,"读写器已打开，请先关闭读写器");
		return SB_ERR_CONNECT;
	}
	
	int pReaderHandle=0;
	int re = iOpenPort( pErrMsg);
	if(re!=0)
		return re;
	if(GetAtrCityCode(pErrMsg)!=0)
	{
		return SB_ERR_CARDTYPE;
	}
	re = iReadCardE(pReaderHandle,pInputPin, pFileAddr, pOutDataBuff,pErrMsg);
	if(re!=0)
	{
		iClosePort(pErrMsg);
		return re;
	}
	re = iClosePort(pErrMsg);
	if(re!=0)
		return re;
}

int iWriteCardEX(const char* pInputPin, const char* pFileAddr, char* pWriteDataBuff,char* pErrMsg)
{
	if(g_handle>0)
	{
		strcpy(pErrMsg,"读写器已打开，请选关闭读写器");
		return SB_ERR_CONNECT;
	}
	
	int pReaderHandle=0;
	int re = iOpenPort( pErrMsg);
	if(re!=0)
		return re;
	if(GetAtrCityCode(pErrMsg)!=0)
	{
		return SB_ERR_CARDTYPE;
	}
	re = iWriteCardE(pReaderHandle,pInputPin,  pFileAddr,  pWriteDataBuff,pErrMsg);
	if(re!=0)
	{
		iClosePort(pErrMsg);
		return re;
	}
	re = iClosePort(pErrMsg);
	if(re!=0)
		return re;
}

int WINAPI iAcctLoad (const  char* 	pInputPin ,
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
					  char     *      pErrMsg)
{
	return 0;
}

int WINAPI iAcctConsume ( const  char* 	pInputPin ,
						 const  char* 	pTransType,
						 const unsigned char* puConsumAmt, 
						 const  char* 	pTransTime,
						 const  char* 	pCardNo,
						 char* 		    pTerminalID,
						 char*  		pTransNo, 
						 unsigned char * puPreTransAmt,
						 char*  		pTradeTac,
						  char     *    pErrMsg)
{
	return 0;
}

int WINAPI iGetCardStatus(char* pCardCardStatus, char* pErrMsg)
{
	unsigned char Response[128]={0};
	int re;
	unsigned char data[1024];
	int reVal;
	int ret;
		
	ICC_Reader_4442_PowerOn(pReaderHandle,data);
	ret=ICC_Reader_4428_Read(pReaderHandle,0,1,data);
	reVal=data[0];
	if(reVal==146)
	{
		strcpy(pCardCardStatus,"卡在读卡器中");
		return 0;
	}
	else
	{
		ret=ICC_Reader_4442_Read(pReaderHandle,0,1,data);
		if(reVal==162 || reVal==163)
		{
			strcpy(pCardCardStatus,"卡在读卡器中");
			return 0;
		}
	}
	
	ret=ICC_Reader_pre_PowerOn(pReaderHandle,USERCard,Response);
	if(ret>0)
	{
		strcpy(pCardCardStatus,"卡在读卡器中");
		return 0;
	}
	else
	{
		strcpy(pErrMsg,"卡不在读卡器中或插反");
		return 105;
	}
	
	return 0;
}

int WINAPI  iGetDllDependentCity(char* pCityName)
{
	strcpy(pCityName,"JiangSu-HuaiAn");
	return 0;
}