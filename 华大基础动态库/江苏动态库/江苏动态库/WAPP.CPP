#include "stdafx.h"
#include "SSSE32.H"
#include "WAPP.H"
#include <string>
using namespace std;
#include <mbstring.h>
#include <math.h>

string strdata1,strdata,sDisp;
char data1[2];
char sAccm;
long Rhand;
int cardtype=0;//1：新卡，2：新一批老卡，3：老一批老卡
unsigned char ucATR[20]={0};

int KeyBoard=0;//0：自己键盘，1：韵达通

unsigned char _CityCode[33]={0};//城市代码
unsigned char _ATR[20]={0};//ATR分散因子
unsigned char _Random[10]={0};

bool moneyFlag = false;

int  PSAM_Slot = 0x11;
int  USERCard =0x01;

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


#pragma data_seg("_CSP_SHARED")
HANDLE g_hEvent = NULL;
#pragma data_seg()
#pragma comment(linker,"/section:_CSP_SHARED,RWS")

bool IsDec1(char *data)
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
bool IsEffectChar1(char *data)
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
int chartoint(char c)
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
	default:
		break;

	}
	return 0;
}
int findChar(int StartAddr,char * data,char c)
{
	int len = strlen(data);
	for(int i=StartAddr;i<len;i++)
	{
		if(data[i]==c)
		{
			return i;
		}
		else
			continue;
	}
	if(i==len)
		return -1;
	return 0;
}

void StrToHex1(unsigned char *Src,unsigned char *Des)
{
	int len = strlen((char*)Src);
	len = len/2;

	for(int i=0;i<len;i++)
	{
		Des[i]=chartoint(Src[2*i])*16+chartoint(Src[2*i+1]);
	}
}

void HexToStr1(unsigned char *Src,unsigned char *Des,int len )
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
void password_setup(unsigned char *instr,unsigned char *outstr,int len)
{
	int i; 
	memcpy(outstr,instr,len);

	for(i=len-1; i >= 0; i--)
	{
		outstr[i] = char(outstr[i]-693-'m'-'t');
	}
}

long iReadRecord(char *EFid,int recNO,int datalength,int style,char *result)//读取记录文件 
{
	Rhand = g_handle;
	unsigned char resp[256];
	unsigned char cmd[260];
	int n=0;
	memset(resp,0,256);
	long ret;
	memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
	cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
	cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
	ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
	if(ret<2 || resp[0]!=0x90)
	{
		//strcpy(result,"选择文件失败!");
		strcpy(result,"读取文件失败--");
		strcat(result,"00A4-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return SB_ERR_READINFO;
	}

//	int id=0;
//	id=chartoint(EFid[2])*16+chartoint(EFid[3]);
	memcpy(cmd, "\x00\xB2\x00\x00\x00",5);
	cmd[2]=recNO;
	cmd[4]=datalength+2;

	memset(resp,0,256);
	ret = ICC_Reader_Application(Rhand,USERCard,5,cmd,resp);
	if (ret<0 || (resp[ret-2]!=0x90))
	{
		strcpy(result,"读取文件失败--");
		strcat(result,"00B2-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return SB_ERR_READINFO;
	}
	char temp[300]={0};
	if(style==AN_style)
	{
		for(  n=0;n<(ret-2);n++)
		{
			temp[n]=resp[n];		
		}
		sDisp = (char*)temp;
		int len = sDisp.length();
		if(len<2)
			len=2;
		memcpy(result,temp+2,len-2);			
	}
	else if(style == CN_style)
	{
		char data1[3]={0};
		for( n=2;n<(ret-2);n++)
		{
			sprintf(data1,"%02X",resp[n]);
			strcat(temp,data1);  
		}
		int len = strlen(temp);

		for(int x=(len-1);x>-1;)
		{
			if(temp[x]=='F')
				temp[x]=0x00;
			x--;
		}

		if(recNO==0x92 || recNO==0xCC || recNO==0xD1 || recNO==0xD6 || recNO==0xDB ||recNO==0xDC || recNO==0xDD || recNO ==0xDE)//本地个人实际缴费金额★  cn	'04'
		{
			char gwei[100]={0};
			char dwei[100]={0};
			int len = strlen(temp);
			if(len>2)
			{
				memcpy(gwei,temp,len-2);
				memcpy(dwei,temp+len-2,2);
			}
			else if(len>0 && len<=2)
			{
				strcpy(dwei,temp);
				strcpy(gwei,"0");
			}
			else
			{
				strcpy(dwei,"00");
				strcpy(gwei,"0");
			}
			
			strcpy(result,gwei);
			strcat(result,".");
			strcat(result,dwei);
			moneyFlag = false;
		}
		else
		{
			strcpy(result,temp);
		}
	}
	return 0;		
}

long iReadRecordEX(char *EFid,int recNO,int datalength,int style,char *result)//读取记录文件 
{
	Rhand = g_handle;
	unsigned char resp[256];
	unsigned char cmd[260];
	int n=0;
	memset(resp,0,256);
	long ret;
	memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
	cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
	cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
	ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
	if(ret<2 || resp[0]!=0x90)
	{
		strcpy(result,"00A4-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return 1;
	}

	memcpy(cmd, "\x00\xB2\x00\x00\x00",5);
	cmd[2]=recNO;
	cmd[4]=datalength+2;

	memset(resp,0,256);
	ret = ICC_Reader_Application(Rhand,USERCard,5,cmd,resp);
	if (ret<0 || (resp[ret-2]!=0x90))
	{
		strcpy(result,"00B2-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return 1;
	}
	char temp[500]={0};
	
	char data1[3]={0};
	for( n=0;n<(ret-2);n++)
	{
		sprintf(data1,"%02x",resp[n]);
		strcat(temp,data1);  
	}
	int len = strlen(temp);

	memcpy(result,temp,len);

	return 0;		
}

void UINTToBin(unsigned int ul , char* hex)
{
	unsigned char t[3];
	
	t[0] = (unsigned char) (ul / 256);
	t[1] = (unsigned char) (ul % 256);
	t[2] = 0;

	memcpy(hex , t , 2);
}

long iWriteBinary(char *EFid,int offset,int datalength,int style,char *result)
{
	if(!IsEffectChar1(result))
	{
		strcpy(result,"非法字符");
		return SB_ERR_OPNUMERROR;
	}
	if((2*datalength)!=strlen(result) && strlen(result)!=0)
	{
		strcpy(result,"操作数越界");
		return SB_ERR_OPNUMOVER;	
	}

	Rhand = g_handle;
	unsigned char resp[256];
	unsigned char cmd[260]={0x00,0xd6};
	
	char *data = new char [0x1002];
	memset(data,0,0x1002);

	int n=0;
	memset(resp,0,100);
	long ret;
	ret=offset;
	int len = strlen(result);
	if(len==0)
	{

	}
	else
	{
		if(style==AN_style)
		{
			memcpy(data,result,datalength);
		}
		else if(style == CN_style)
		{
			if(len!=2*datalength || (len%2)!=0)
			{
				strcpy(result,"操作数越界");
				delete []data;
				data = NULL;
				return SB_ERR_OPNUMOVER;
			}
			for( n=0;n<datalength;n++)
			{
				data[n] = chartoint(result[2*n])*16+chartoint(result[2*n+1]);  
			}
		}
	}
	

	memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
	cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
	cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
	ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
	if(ret<2 || resp[0]!=0x90)
	{
		//strcpy(result,"选择文件失败!");
		strcpy(result,"写入文件失败--");
		strcat(result,"00A4-");
		strcat(result,EFid);
		char a1[15]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a1,2);
		strcat(result,a1);
		delete []data;
		data = NULL;
		return SB_ERR_WRITEINFO;
	}

	int a = (datalength - datalength % 0x80) / 0x80 ;
	if(datalength%0x80==0)
		a=a-1;
	for (int i = 0 ; i < a ; i++ )
	{
		memcpy (cmd, "\x00\xD6\x00\x00\x80" , 5);
		ret = i * 0x80+offset;
		UINTToBin(ret , (char*)cmd + 2);
		memcpy(cmd+5,data+ret-offset,0x80);
		memset(resp,0,256);
		ret = ICC_Reader_Application(Rhand,USERCard,5+0x80,cmd,resp);
		if (ret<0 || (resp[ret-2]!=0x90))
		{
			strcpy(result,"写入文件失败--");
			strcat(result,"00D6-");
			strcat(result,EFid);
			char a1[15]={0};
			if(ret>1)
				HexToStr1(&resp[ret-2],(unsigned char*)a1,2);
			strcat(result,a1);
			delete []data;
			data = NULL;
			return SB_ERR_WRITEINFO;
		}
	}
	//last block
	memcpy (cmd, "\x00\xD6\x00\x00\x80" , 5);
	ret = i * 0x80+offset;
	UINTToBin(ret , (char*)cmd + 2);
	cmd[4] = (datalength % 0x80)  ;
	if(datalength%0x80==0)
		cmd[4]=0x80;
	memset(resp,0,256);
	memcpy(cmd+5,data+i*0x80,cmd[4]);
	ret = ICC_Reader_Application(Rhand,USERCard,5+cmd[4],cmd,resp);
	if (ret<0 || (resp[ret-2]!=0x90))
	{
		strcpy(result,"写入文件失败--");
		strcat(result,"00D6-");
		strcat(result,EFid);
		char a1[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a1,2);
		strcat(result,a1);
		delete []data;
		data = NULL;
		return SB_ERR_WRITEINFO;
	}
	delete []data;
	data = NULL;
	return 0;		
}
long iReadBinary(char *EFid,int offset,int datalength,int style,char *result)
{
	Rhand = g_handle;
	unsigned char resp[256];
	unsigned char cmd[260]={0x00,0xb0};
	unsigned char data[0x1000]={0};
	int n=0;
	int re=0;
	
	memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
	cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
	cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
	int ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
	if(ret<2 || resp[0]!=0x90)
	{
		//strcpy(result,"选择文件失败!");
		strcpy(result,"选择文件--");
		strcat(result," 00A4-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return SB_ERR_READINFO;
	}

	memset(resp,0,100);
	ret=offset;
	int a = (datalength - datalength % 0xf0) / 0xf0 ;
	if(datalength%0xf0==0)
		a=a-1;
	for (int i = 0 ; i < a ; i++ )
	{
		memcpy (cmd, "\x00\xb0\x00\x00\xf0" , 5);
		ret = i * 0xf0+offset;
		UINTToBin(ret , (char*)cmd + 2);
		memset(resp,0,256);
		re = ICC_Reader_Application(Rhand,USERCard,5,cmd,resp);
		if (re<0 || (resp[re-2]!=0x90))
		{
			strcpy(result,"读取文件失败--");
			strcat(result,"00B0-");
			strcat(result,EFid);
			char a[5]={0};
			if(ret>1)
				HexToStr1(&resp[ret-2],(unsigned char*)a,2);
			strcat(result,a);
			return SB_ERR_READINFO;
		}
		memcpy(data+ret-offset,resp,0xf0);
	}
	//last block
	memcpy (resp, "\x00\xb0\x00\x00\xf0" , 5);
	ret = i * 0xf0+offset;
	UINTToBin(ret , (char*)cmd + 2);
	cmd[4] = datalength % 0xf0 ;
	if(datalength%0xf0 == 0)
		cmd[4]=0xf0;
	memset(resp,0,256);
	re = ICC_Reader_Application(Rhand,USERCard,5,cmd,resp);
	if (re<0 || (resp[re-2]!=0x90))
	{
		strcpy(result,"读取文件失败--");
		strcat(result,"00B0-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return SB_ERR_READINFO;
	}
	memcpy(data+ret-offset,resp,cmd[4]);

	ret = i*0xf0;
	ret += cmd[4];

	char temp[0x2002]={0};
	if(style==AN_style)
	{
		memcpy(result,data,datalength);			
	}
	else if(style == CN_style)
	{
		char data1[3]={0};
		for( n=0;n<(ret);n++)
		{
			sprintf(data1,"%02X",data[n]);
			strcat(temp,data1);  
		}
		int len = strlen(temp);
		memcpy(result,temp,len);
	}
	
	return 0;		
}

long SelectFile(char *FileId)
{
	Rhand = g_handle;
	unsigned char resp[256];
	unsigned char cmd[260];
	int n=0;
	memset(resp,0,100);
	long ret;

	if(strcmp(FileId,"DDF1")==0)
	{
		memcpy(cmd, "\x00\xA4\x04\x00\x0F\x73\x78\x31\x2E\x73\x68\x2E\xC9\xE7\xBB\xE1\xB1\xA3\xD5\xCF",20);
		ret = ICC_Reader_Application(g_handle,USERCard,20,cmd,resp);//X选择MF文件
		if(resp[ret-2]!=0x61 && resp[ret-2]!=0x90)
		{
			return 1;
		}
	}
	else
	{
		memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
		cmd[5]=chartoint(FileId[0])*16+chartoint(FileId[1]);
		cmd[6]=chartoint(FileId[2])*16+chartoint(FileId[3]);	
		ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
		if(ret<2 || (resp[ret-2]!=0x61 && resp[ret-2]!=0x90))
		{
			return 1;
		}
	}
	
	return 0;
}

long GetRandom()
{
	Rhand = g_handle;
	unsigned char resp[256]={0};
	unsigned char cmd[260]={0};

	long ret=0;

	memcpy(cmd, "\x00\x84\x00\x00\x08",5);
	
	ret = ICC_Reader_Application(Rhand,0x01,5,cmd,resp);//X选择MF文件
	if(ret<2 || (resp[ret-2]!=0x90))
	{
		return 1;
	}
	memcpy(_Random,resp,8);

	return 0;	
}
static DWORD WINAPI SendProc(PVOID p)
{
	unsigned char cpass[20]={0};
	WaitForSingleObject(g_hEvent, INFINITE);
	unsigned char len ;
	long re = ICC_GetInputPass(Rhand, 20,&len,0,0, cpass);
	SetEvent(g_hEvent);

	return 0;
}


long GetPassWord(char*data,char*err)
{

	Rhand = g_handle;

	unsigned char cpass[20]={0};
	unsigned char len ;

	if(KeyBoard ==0)
	{
	//	ICC_DispSound(Rhand,4);
		long re = ICC_GetInputPass(Rhand, 10,&len,0,0, cpass);
		if(re==0)
		{
			KeyBoard =0;
			for(int i=0;i<len;i++)
				cpass[i]+=0x30;
			strcpy(data,(char*)cpass);

			return 0;
		}
		else
			KeyBoard =1;
	}

	if(KeyBoard ==1)
	{

	}
	return 0;
}

long PerPINVerify(char*data,char*err)
{
	unsigned char resp[100]={0};
	unsigned char cmd[260]={0};
	unsigned char temp[100]={0};
	strdata="";
	sDisp="";
	long ret;
	memcpy(cmd, "\x00\xA4\x04\x00\x0F\x73\x78\x31\x2E\x73\x68\x2E\xC9\xE7\xBB\xE1\xB1\xA3\xD5\xCF",20);
	//00A404000F7378312E73682EC9E7BBE1B1A3D5CF
	ret = ICC_Reader_Application(Rhand,0x01,20,cmd,resp);//X选择MF文件
	
	strdata = data;
	long EndLen = strdata.find("<",2);
	long StartLen = strdata.find(">",0); 
	sDisp = strdata.substr(StartLen+1,EndLen-StartLen-1);
	

	memcpy(cmd, "\x00\x20\x00\x00",5);
	int len = sDisp.length()/2;
	cmd[4]=len;
	strcpy((char*)temp,sDisp.c_str());
	for(int i=0;i<len;i++)
	{
		cmd[5+i]=(temp[2*i]-0x30)*16+temp[2*i+1]-0x30;
	}
	memset(resp,0,100);
	ret = ICC_Reader_Application(Rhand,0x01,len+5,cmd,resp);
	if (ret<0)
	{
		sprintf(err,"%d",ret);
		strcpy(data,"<DATA>99</DATA>");
		return 1;
	}

	if(resp[0]!=0x90 && resp[1]!=0x00)
	{
		switch(resp[1])
		{
			case 0xc5:strcpy(err,"密钥认证错误,剩余5次");break;
			case 0xc4:strcpy(err,"密钥认证错误,剩余4次");break;
			case 0xc3:strcpy(err,"密钥认证错误,剩余3次");break;
			case 0xc2:strcpy(err,"密钥认证错误,剩余2次");break;
			case 0xc1:strcpy(err,"密钥认证错误,剩余1次");break;
			case 0xc0:strcpy(err,"密钥认证错误,剩余0次");break;
			default:strcpy(err,"密码被锁！");break;
		}

		strcpy(data,"<DATA>99</DATA>");
		return 1;
	}

	strcpy(data,"<DATA>00</DATA>");
	return 0;
}

long VerifyPIN(char*pin,char*err)
{
	Rhand = g_handle;
	unsigned char resp[100]={0};
	unsigned char cmd[260]={0};
	unsigned char temp[100]={0};

	long ret;

	memcpy(cmd, "\x00\x20\x00\x00",5);
	int len = strlen(pin)/2;
	cmd[4]=len;

	for(int i=0;i<len;i++)
	{
		cmd[5+i]=(pin[2*i]-0x30)*16+pin[2*i+1]-0x30;
	}
	memset(resp,0,100);
	ret = ICC_Reader_Application(Rhand,0x01,len+5,cmd,resp);
	if (ret<0)
	{
		strcpy(err,"命令执行失败");
		return SB_ERR_OTHERERROR ;
	}

	if(resp[0]!=0x90 && resp[1]!=0x00)
	{
		switch(resp[1])
		{
			case 0xc5:strcpy(err,"密钥认证错误,剩余5次");break;
			case 0xc4:strcpy(err,"密钥认证错误,剩余4次");break;
			case 0xc3:strcpy(err,"密钥认证错误,剩余3次");break;
			case 0xc2:strcpy(err,"密钥认证错误,剩余2次");break;
			case 0xc1:strcpy(err,"密钥认证错误,剩余1次");break;
			case 0xc0:strcpy(err,"密钥认证错误,剩余0次");break;
			default:strcpy(err,"密码被锁！");break;
		}
		return SB_ERR_AUTHPIN;
	}
	return 0;
}

long ExterAutenKey(unsigned char *keyval)
{
	Rhand = g_handle;
	unsigned char cmd[100]={0};
	unsigned char resp[100]={0};
	GetRandom();
	memset(cmd,0,50);
	memcpy(cmd, "\x00\x82\x00\x00\x08",5);
	unsigned char encode[20]={0};

	memcpy(encode,keyval,16);

	short relen;
	HD_3DES_EncryptByte(_Random,8,encode,&relen);
	
	memcpy(cmd+5,encode,8);

	int ret = ICC_Reader_Application(Rhand,0x01,13,cmd,resp);
	if(ret<2 || (resp[ret-2]!=0x90 && resp[ret-2]!=0x61))
	{
		return -1;
	}
	return 0;
}

long ExterAuthen(unsigned char *WorkKey,int level,char *err)
{
	Rhand = g_handle;
	unsigned char cmd[100]={0};
	unsigned char resp[100]={0};

	GetRandom();
	int re = ICC_Reader_pre_PowerOn(Rhand,PSAM_Slot,resp);
	memset(cmd,0,50);
	memcpy(cmd, "\x00\xA4\x00\x00\x02\xDF\x01",7);
	int ret = ICC_Reader_Application(Rhand,PSAM_Slot,7,cmd,resp);
	if(ret<2 || (resp[ret-2]!=0x90 && resp[ret-2]!=0x61))
	{
		strcpy(err,"00A4-");
		strcat(err,"DF01");
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(err,a);
		return -1;
	}
	memcpy(cmd, "\xBF\xDE\x67\x08\x20",5); //一级
	cmd[2]=WorkKey[0];
	cmd[3]=WorkKey[1];

	if(level==1)//一级
	{
		cmd[4]=0x20;
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

		cmd[21]=_CityCode[0];
		cmd[22]=_CityCode[1];
		cmd[23]=0x30;
		cmd[24]=0x30;
		cmd[25]=0x30;
		cmd[26]=0x30;
		cmd[27]=0x73;
		cmd[28]=0x68;
		
		cmd[29]=_Random[0];
		cmd[30]=_Random[1];
		cmd[31]=_Random[2];
		cmd[32]=_Random[3];
		cmd[33]=_Random[4];
		cmd[34]=_Random[5];
		cmd[35]=_Random[6];
		cmd[36]=_Random[7];

		ret = ICC_Reader_Application(Rhand,PSAM_Slot,37,cmd,resp);
		if(ret<2 || (resp[ret-2]!=0x90))
		{
			strcpy(err,"BFDE-");

			char a[5]={0};
			if(ret>1)
				HexToStr1(&resp[ret-2],(unsigned char*)a,2);
			strcat(err,a);
			return -1;
		}
	}
	else if(level ==2)//二级
	{
		cmd[4]=0x18;
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
		ret = ICC_Reader_Application(Rhand,PSAM_Slot,29,cmd,resp);
		if(ret<2 || (resp[ret-2]!=0x90))
		{
			strcpy(err,"BFDE-");

			char a[5]={0};
			if(ret>1)
				HexToStr1(&resp[ret-2],(unsigned char*)a,2);
			strcat(err,a);
			return -1;
		}
	}
	else if(level == 3)//三级
	{
		cmd[4]=0x10;
		cmd[5]=_ATR[0];
		cmd[6]=_ATR[1];
		cmd[7]=_ATR[2];
		cmd[8]=_ATR[3];
		cmd[9]=_ATR[4];
		cmd[10]=_ATR[5];
		cmd[11]=_ATR[6];
		cmd[12]=_ATR[7];

		cmd[13]=_Random[0];
		cmd[14]=_Random[1];
		cmd[15]=_Random[2];
		cmd[16]=_Random[3];
		cmd[17]=_Random[4];
		cmd[18]=_Random[5];
		cmd[19]=_Random[6];
		cmd[20]=_Random[7];
		ret = ICC_Reader_Application(Rhand,PSAM_Slot,21,cmd,resp);
		if(ret<2 || (resp[ret-2]!=0x90))
		{
			strcpy(err,"BFDE-");

			char a[5]={0};
			if(ret>1)
				HexToStr1(&resp[ret-2],(unsigned char*)a,2);
			strcat(err,a);
			return -1;
		}
	}
	else
	{
		return -1;
	}

	memset(cmd,0,50);
	memcpy(cmd, "\x80\xFA\x00\x00\x08\x01\x02\x03\x04\x05\x06\x07\x08",13);//对鉴别因子加密
	ret = ICC_Reader_Application(Rhand,PSAM_Slot,13,cmd,resp);
	if(ret<2 || (resp[ret-2]!=0x61))
	{
		strcpy(err,"80FA-");

		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(err,a);
		return -1;
	}
	memset(cmd,0,50);
	memcpy(cmd, "\x00\xc0\x00\x00\x08",5);
	ret = ICC_Reader_Application(Rhand,PSAM_Slot,5,cmd,resp);
	if(ret<2 || (resp[ret-2]!=0x90))
	{
		strcpy(err,"00C0-");

		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(err,a);
		return -1;
	}

	memset(cmd,0,50);
	memcpy(cmd,"\x00\x82\x00\x00\x10",5);
	cmd[3]=WorkKey[2];
	memcpy(cmd+5,resp,8);
	memcpy(cmd+13,"\x01\x02\x03\x04\x05\x06\x07\x08",8);

	ret = ICC_Reader_Application(Rhand,0x01,21,cmd,resp);
	if(resp[0]!=0x90 || ret<2)
	{
		strcpy(err,"0082-");

		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(err,a);
		return -1;
	}

	return 0;
}

long iReadBTLV(char *EFid,int recNO,int datalength,TLV *tl,int size,char *result) //recNO 记录号  size 数据项
{
	Rhand = g_handle;
	unsigned char resp[256];
	unsigned char cmd[260];
	sDisp="";
	int n=0;
//	strdata1=data;
	memset(resp,0,256);
	long ret;
	memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
	cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
	cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
	ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
	if(ret<2 || resp[0]!=0x90)
	{
		strcpy(result,"读取文件失败--");
		strcat(result,"00A4-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return SB_ERR_READINFO;
	}
	memcpy(cmd, "\x00\xB2\x00\x04\x00",5);
	cmd[2]=recNO;
	cmd[4]=datalength+2;
	
	memset(resp,0,256);
	ret = ICC_Reader_Application(Rhand,USERCard,5,cmd,resp);
	if (ret<0 || (resp[ret-2]!=0x90))
	{
		strcpy(result,"读取文件失败--");
		strcat(result,"00B2-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return SB_ERR_READINFO;
	}

	unsigned char tempdata[256]={0};
	int total_len=0;
	for(int i=0;i<size;i++)
	{
		memset(tempdata,0,256);

		memcpy(tempdata,resp+total_len+2,tl[i].length+2);
		
		if(tl[i].datastyle==AN_style)
		{
			strcat(result,(char*)tempdata+2);
			strcat(result,",");
		}
		else if(tl[i].datastyle == CN_style)
		{
			
			char data1[3]={0};
			char data2[300]={0};
			for( n=2;n<tl[i].length+2;n++)
			{
				sprintf(data1,"%02X",tempdata[n]);
				strcat(data2,data1);  
			}

			int len = strlen(data2);//去掉FF
			for(int x=(len-1);x>-1;)
			{
				if(data2[x]=='F')
					data2[x]=0x00;
				x--;
			}
			
			if(strcmp(tl[i].flag,"1C")==0 || strcmp(tl[i].flag,"E6")==0)//本地个人实际缴费金额★  cn	'04'
			{
				char gwei[100]={0};
				char dwei[100]={0};
				int len = strlen(data2);
				if(len>2)
				{
					memcpy(gwei,data2,len-2);
					memcpy(dwei,data2+len-2,2);
				}
				else if(len>0 && len<=2)
				{
					strcpy(dwei,data2);
					strcpy(gwei,"0");
				}
				else
				{
					strcpy(dwei,"00");
					strcpy(gwei,"0");
				}
				
				strcat(result,gwei);
				strcat(result,".");
				strcat(result,dwei);
				strcat(result,",");
				moneyFlag = false;
			}
			else
			{
				strcat(result,data2);
				strcat(result,",");
			}
		}
		total_len+=2;
		total_len+=tl[i].length;
	}
	return 0;		 
}

long iReadBTLVEX(char *EFid,int recNO,int datalength,TLV *tl,int size,char *result) //recNO 记录号  size 数据项
{
	Rhand = g_handle;
	unsigned char resp[256];
	unsigned char cmd[260];
	int n=0;

	memset(resp,0,256);
	long ret;
	memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
	cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
	cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
	ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
	if(ret<2 || resp[0]!=0x90)
	{
		strcpy(result,"00A4-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return 1;
	}
	memcpy(cmd, "\x00\xB2\x00\x04\x00",5);
	cmd[2]=recNO;
	cmd[4]=datalength+2;
	
	memset(resp,0,256);
	ret = ICC_Reader_Application(Rhand,USERCard,5,cmd,resp);
	if (ret<0 || (resp[ret-2]!=0x90))
	{
		strcpy(result,"00B2-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return 1;
	}
	char data1[3]={0};
	char data2[0x300]={0};
	for( n=0;n<datalength+2;n++)
	{
		sprintf(data1,"%02x",resp[n]);
		strcat(data2,data1);  
	}
	strcat(result,data2);

/*
	unsigned char tempdata[256]={0};
	int total_len=0;
	for(int i=0;i<size;i++)
	{
		memset(tempdata,0,256);

		memcpy(tempdata,resp+total_len,tl[i].length+2);
		
		if(tl[i].datastyle==AN_style)
		{
			char data1[3]={0};
			char data2[300]={0};
			for( n=2;n<tl[i].length+2;n++)
			{
				sprintf(data1,"%02x",tempdata[n]);
				strcat(data2,data1);  
			}
			strcat(result,data2);
//			strcat(result,",");
		}
		else if(tl[i].datastyle == CN_style)
		{
			char data1[3]={0};
			char data2[300]={0};
			for( n=2;n<tl[i].length+2;n++)
			{
				sprintf(data1,"%02x",tempdata[n]);
				strcat(data2,data1);  
			}
			strcat(result,data2);
//			strcat(result,",");
		}

		total_len+=2;
		total_len+=tl[i].length;
	}*/
	return 0;		 
}

long iReadXHFile(char *EFid,int recNO,int datalength,CYC *cy,int size,char *result)//recNO,记录条数，size，数据项数目
{
	Rhand = g_handle;
	unsigned char resp[256];
	unsigned char cmd[260];
	int n=0;
	memset(resp,0,256);
	long ret;
	memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
	cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
	cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
	ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
	if(ret<2 || resp[0]!=0x90)
	{
		strcpy(result,"读取文件失败--");
		strcat(result,"00A4-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return SB_ERR_READINFO;
	}

	memcpy(cmd, "\x00\xB2\x00\x04\x00",5);
	cmd[2]=recNO;
	cmd[4]=datalength;

	memset(resp,0,256);
	ret = ICC_Reader_Application(Rhand,USERCard,5,cmd,resp);
	if (ret<0 || (resp[ret-2]!=0x90))
	{
		strcpy(result,"读取文件失败--");
		strcat(result,"00B2-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return SB_ERR_READINFO;
	}
	char temp[512]={0};
	unsigned char tempdata[256]={0};
	int total_len=0;

	for(int j=0;j<size;j++)
	{
		memset(tempdata,0,256);
		memcpy(tempdata,resp+total_len,cy[j].length);
		if(cy[j].datastyle==AN_style)
		{
			strcat(result,(char*)tempdata);			
		}
		else if(cy[j].datastyle == CN_style)
		{
			char data22[512]={0};
			if(strcmp(EFid,"EF15")==0 && j==0)//交易序号	b	'02'
			{
				char gwei[100]={0};
				int count =tempdata[0]*256+tempdata[1];
				
				sprintf(gwei,"%d",count);
				strcpy(data22,gwei);
			}
			else if(strcmp(EFid,"EF08")==0 && j>3 && j<7)
			{
				char gwei[100]={0};
				int count =tempdata[3]*16777216+tempdata[2]*65536+tempdata[1]*256+tempdata[0];
				sprintf(gwei,"%d",count);
				strcpy(data22,gwei);
			}
			else if(strcmp(EFid,"EF08")==0 && j==0)//交易序号	b	'02'
			{
				char gwei[100]={0};
				int count =tempdata[0]*256+tempdata[1];
				
				sprintf(gwei,"%d",count);
				strcpy(data22,gwei);
			}
			else
			{
				char data1[3]={0};
				memset(data22,0,512);
				for( n=0;n<cy[j].length;n++)
				{
					sprintf(data1,"%02X",tempdata[n]);
					strcat(data22,data1);  
				}
				int len = strlen(data22);
				for(int i=(len-1);i>-1;)
				{
					if(data22[i]=='F')
						data22[i]=0x00;
					i--;
				}
			}
			strcat(result,data22);
		}

		total_len+=cy[j].length;
		strcat(result,",");
	}	
	return 0;
}

long iReadXHFileEX(char *EFid,int recNO,int datalength,CYC *cy,int size,char *result)//recNO,记录条数，size，数据项数目
{
	Rhand = g_handle;
	unsigned char resp[256];
	unsigned char cmd[260];
	int n=0;
	memset(resp,0,256);
	long ret;
	memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
	cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
	cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
	ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
	if(ret<2 || resp[0]!=0x90)
	{
		strcpy(result,"00A4-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return 1;
	}

	memcpy(cmd, "\x00\xB2\x00\x04\x00",5);
	cmd[2]=recNO;
	cmd[4]=datalength;

	memset(resp,0,256);
	ret = ICC_Reader_Application(Rhand,USERCard,5,cmd,resp);
	if (ret<0 || (resp[ret-2]!=0x90))
	{
		strcpy(result,"00B2-");
		strcat(result,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(result,a);
		return 1;
	}
	char temp[512]={0};
	unsigned char tempdata[256]={0};
	int total_len=0;

	for(int j=0;j<size;j++)
	{
		memset(tempdata,0,256);
		memcpy(tempdata,resp+total_len,cy[j].length);
		char data1[3]={0};
		char data2[300]={0};
		for( n=0;n<cy[j].length;n++)
		{
			sprintf(data1,"%02x",tempdata[n]);
			strcat(data2,data1);  
		}
		strcat(result,data2);
		total_len+=cy[j].length;
		strcat(result,",");
	}	
	return 0;
}

long iWriteRecordFile(char *EFid,unsigned char * Tag,int recNO,unsigned char * datalength,unsigned char* style,char *data) //recNO 没有用到  Tag 记录文件每条记录的标签
{
	Rhand = g_handle;
	unsigned char resp[256];
	unsigned char cmd[260];
	unsigned char temp[256]={0};

		memset(resp,0,256);
		long ret;
		memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
		cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
		cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
		ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
		if(ret<2 || resp[0]!=0x90)
		{
			strcpy(data,"写入文件失败--");
			strcat(data,"00A4-");
			strcat(data,EFid);
			char a[5]={0};
			if(ret>1)
				HexToStr1(&resp[ret-2],(unsigned char*)a,2);
			strcat(data,a);
			return SB_ERR_WRITEINFO;
		}
		memset(temp,0,256);
		int len = strlen(data);

		if(style[recNO]==AN_style)
		{
			if(len>datalength[recNO])
			{
				strcpy(data,"传入数据长度越界");
				return SB_ERR_OPNUMOVER;
			}
			for( int n=0;n<(len);n++)
			{
				temp[n]=data[n];		
			}		
		}
		else if(style[recNO] == CN_style)
		{
			if(len!=0)
			{
				if(Tag[recNO]==0x92 || Tag[recNO]==0xCC || Tag[recNO]==0xD1 || Tag[recNO]==0xD6 || Tag[recNO]==0xDB ||Tag[recNO]==0xDC || Tag[recNO]==0xDD || Tag[recNO]==0xDE)//本地个人实际缴费金额★  cn	'04'
				{
					char gwei[100]={0};
					char dwei[100]={0};

					int addr = findChar(0,data,'.');
					int l = strlen(data);
					if(addr<0)//无小数
					{
						strcpy(dwei,"00");
						addr = l;
						memcpy(gwei,data,l);
					}
					else if(addr==0)
					{
						strcat(data,"传入待写入金额格式有误");
						return SB_ERR_OPNUMERROR;
					}
					else
					{
						memcpy(gwei,data,addr);
						memcpy(dwei,data+addr+1,l-addr-1);
					}
					if(!IsDec1(gwei))
					{
						strcat(data,"传入待写入数据有未能识别字符");
						return SB_ERR_OPNUMERROR;
					}
					if(strlen(gwei)>6)
					{
						strcat(data,"传入待写入数据长度越界");
						return SB_ERR_OPNUMOVER;
					}
					if(!IsDec1(dwei))
					{
						strcat(data,"传入待写入数据有未能识别字符");
						return SB_ERR_OPNUMERROR;
					}
					if(strlen(dwei)>2)
					{
						strcat(data,"传入待写入数据长度越界");
						return SB_ERR_OPNUMOVER;
					}
					int gg = atoi(gwei)*100+atoi(dwei);
					sprintf(gwei,"%d",gg);
					strcpy(data,gwei);
					moneyFlag = false;
					len = strlen(data);
				}
			}
			if(!IsEffectChar1(data))
			{
				strcat(data,"传入待写入数据有未能识别字符");
				return SB_ERR_OPNUMERROR;
			}
			memset(temp,0xFF,256);
			if((len%2)!=0)
			{
				strcat(data,"F");
				len+=1;
			}
			if(len>(2*datalength[recNO]))
			{
				strcpy(data,"传入待写入数据长度越界");
				return SB_ERR_OPNUMOVER;
			}
			for(int n=0;n<len/2;n++)
			{
				temp[n] = chartoint(data[2*n])*16+chartoint(data[2*n+1]);  
			}
			for(int j=0;j<(datalength[recNO]-len/2);j++)
				temp[n++]=0xFF;

		}
		else
		{
			return 1;
		}

		memcpy(cmd, "\x00\xDC\x00\x00\x00",5);
		cmd[2]=Tag[recNO];
		cmd[4]=datalength[recNO]+2;
		cmd[5]=Tag[recNO];
		cmd[6]=datalength[recNO];
		for(int i=0;i<datalength[recNO];i++)
		{
			cmd[7+i]=temp[i];
		}

		memset(resp,0,256);
		ret = ICC_Reader_Application(Rhand,USERCard,datalength[recNO]+7,cmd,resp);
		if (ret<0 || (resp[ret-2]!=0x90))
		{
			strcpy(data,"写入文件失败--");
			strcat(data,"00DC-");
			strcat(data,EFid);
			char a[5]={0};
			if(ret>1)
				HexToStr1(&resp[ret-2],(unsigned char*)a,2);
			strcat(data,a);
			return SB_ERR_WRITEINFO;
		}
	
	return 0;		
}

long iWriteRecordFileEX(char *EFid,unsigned char * Tag,int recNO,unsigned char * datalength,unsigned char* style,char *data) //recNO 没有用到  Tag 记录文件每条记录的标签
{
	Rhand = g_handle;
	unsigned char resp[256];
	unsigned char cmd[260];
	unsigned char temp[256]={0};

	memset(resp,0,256);
	long ret;
	memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
	cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
	cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
	ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
	if(ret<2 || resp[0]!=0x90)
	{
		strcpy(data,"00A4-");
		strcat(data,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(data,a);
		return 1;
	}
	memset(temp,0,256);
	int len = strlen(data);
	if(len!=(2*(datalength[recNO]+2)) || (len%2)!=0)
	{
		strcpy(data,"传入数据有误");
		return 1;
	}
	for(int n=0;n<len/2;n++)
	{
		temp[n] = chartoint(data[2*n])*16+chartoint(data[2*n+1]);  
	}
	for(int j=0;j<(datalength[recNO]+2-len/2);j++)
		temp[n++]=0xFF;

	memcpy(cmd, "\x00\xDC\x00\x00\x00",5);
	cmd[2]=Tag[recNO];
	cmd[4]=datalength[recNO]+2;
	cmd[5]=temp[0];
	cmd[6]=temp[1];
	for(int i=0;i<datalength[recNO];i++)
	{
		cmd[7+i]=temp[2+i];
	}

	memset(resp,0,256);
	ret = ICC_Reader_Application(Rhand,USERCard,datalength[recNO]+7,cmd,resp);
	if (ret<0 || (resp[ret-2]!=0x90))
	{
		strcpy(data,"00DC-");
		strcat(data,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(data,a);
		return 1;
	}
	
	return 0;		
}

long iWriteRecord(char *EFid,unsigned char * Tag,int recNO,unsigned char * datalength,unsigned char* style,char *data) //recNO 没有用到  Tag 记录文件每条记录的标签
{
	Rhand = g_handle;
	unsigned char resp[256];
	unsigned char cmd[260];
	unsigned char temp[256]={0};
	sDisp="";
	int n=0;
	string strdata1;
	strdata="";
	strdata1="";
	strdata1 = data;
	long StartLen = 0;
	long EndLen = 0;
	long StartLen1 = 0;
	long EndLen1 = 0;
	
	while(true)
	{
		StartLen = strdata1.find("<",EndLen1);
		if(StartLen<0)
			break;
		EndLen = strdata1.find(">",StartLen); // <C01>

		sDisp = strdata1.substr(StartLen+2,EndLen-StartLen-1);//C01->01

		int rec = atoi(sDisp.c_str());

		StartLen1 = strdata1.find("<",EndLen);// </C
		EndLen1 = strdata1.find(">",StartLen1); // </C01>

		sDisp = strdata1.substr(EndLen+1,StartLen1-EndLen-1);

		memset(resp,0,256);
		long ret;
		memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
		cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
		cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
		ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
		if(ret<2 || resp[0]!=0x90)
		{
			return 1;
		}
		memset(temp,0,256);
		int len = sDisp.length();
	//	int inlen = datalength;
		if(style[rec-1]==AN_style)
		{
			for(  n=0;n<(len);n++)
			{
				temp[n]=sDisp[n];		
			}		
		}
		else if(style[rec-1] == CN_style)
		{
			for( n=0;n<len/2;n++)
			{
				temp[n] = chartoint(sDisp[2*n])*16+chartoint(sDisp[2*n+1]);  
			}
		}
		else
		{
			return 1;
		}

		memcpy(cmd, "\x00\xDC\x00\x04\x00",5);
		cmd[2]=rec;
		cmd[4]=datalength[rec-1]+2;
		cmd[5]=Tag[rec-1];
		cmd[6]=datalength[rec-1];
		for(int i=0;i<datalength[rec-1];i++)
		{
			cmd[7+i]=temp[i];
		}

		memset(resp,0,256);
		ret = ICC_Reader_Application(Rhand,USERCard,datalength[rec-1]+7,cmd,resp);
		if (ret<0 || (resp[ret-2]!=0x90))
		{
	//		return 1;
		}
	}
	return 0;		
}

long iWriteBTLV(char *EFid,int recNO,int datalength,TLV *tl,int size,char *data,unsigned char _flag) //size 数据项
{   
	Rhand = g_handle;
	int ret =0;
	char carddata[256]={0};
	int n=0;
	int len =0;
	int start=0;
	char temp[256]={0};
	for(int i=0;i<size;i++)
	{
		memset(temp,0,256);
		int s = findChar(start,data,',');// 1111|23456|

		memcpy(temp,data+start,s-start);
		
		carddata[n++]=chartoint(tl[i].flag[0])*16+chartoint(tl[i].flag[1]); //添加标志位
		carddata[n++]=tl[i].length;

		if((tl[i].datastyle)==AN_style)
		{
			len = strlen(temp);
			if(len>tl[i].length)	
			{
				strcpy(data,"传入数据长度越界");
				return SB_ERR_OPNUMOVER;
			}
			for( int j=0;j<tl[i].length;j++)
			{
				carddata[n++]=temp[j];		
			}		
		}
		else if((tl[i].datastyle) == CN_style)
		{
			if(strlen(temp)==0)
			{
				
			}
			else
			{
				if(strcmp(tl[i].flag,"1C")==0 || strcmp(tl[i].flag,"E6")==0)//本地个人实际缴费金额★  cn	'04'
				{
					char gwei[100]={0};
					char dwei[100]={0};

					int addr = findChar(0,temp,'.');
					int l = strlen(temp);
					if(addr<0)//无小数
					{
						strcpy(dwei,"00");
						addr = l;
						memcpy(gwei,temp,l);
					}
					else if(addr==0)
					{
						strcpy(data,"传入待写入数据有未能识别字符");
						strcat(data,temp);
						return SB_ERR_OPNUMERROR;
					}
					else
					{
						memcpy(gwei,temp,addr);
						memcpy(dwei,temp+addr+1,l-addr-1);
					}

					if(!IsDec1(gwei))
					{
						strcpy(data,"传入待写入数据有未能识别字符");
						strcat(data,gwei);
						return SB_ERR_OPNUMERROR;
					}
					if(strlen(gwei)>6)
					{
						strcpy(data,"传入数据长度越界");
						strcat(data,gwei);
						return SB_ERR_OPNUMOVER;
					}
					if(!IsDec1(dwei))
					{
						strcpy(data,"传入待写入数据有未能识别字符");
						strcat(data,dwei);
						return SB_ERR_OPNUMERROR;
					}
					if(strlen(dwei)>2)
					{
						strcpy(data,"传入数据长度越界");
						strcat(data,dwei);
						return SB_ERR_OPNUMOVER;
					}
					int gg = atoi(gwei)*100+atoi(dwei);

					sprintf(gwei,"%d",gg);
					strcpy(temp,gwei);
					moneyFlag = false;
				}
			}
			if(!IsEffectChar1(temp))
			{
				strcpy(data,"传入待写入数据有未能识别字符");
				strcat(data,temp);
				return SB_ERR_OPNUMERROR;
			}
			len = strlen(temp);
			if((len%2)!=0)
			{
				strcat(temp,"F");
				len+=1;
			}
			if((len/2)>tl[i].length)
			{
				strcpy(data,"传入数据长度越界");
				return SB_ERR_OPNUMOVER;
			}
			for(int j=0;j<(len/2);j++)
			{
				carddata[n++] = chartoint(temp[2*j])*16+chartoint(temp[2*j+1]);  
			}
			for(j=0;j<(tl[i].length-len/2);j++)
				carddata[n++]=0xFF;
		}
		start=s;
		start+=1;
	}

	unsigned char resp[256];
	unsigned char cmd[300]={0};
	memset(resp,0,256);
//	long ret;
	memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
	cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
	cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
	ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
	if(ret<2 || resp[0]!=0x90)
	{
		strcpy(data,"写入文件失败--");
		strcat(data,"00A4-");
		strcat(data,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(data,a);
		return SB_ERR_WRITEINFO;
	}

	memset(cmd,0,300);
	memcpy(cmd,"\x00\xDC\x00\x04\x00",5);
	cmd[2]=recNO;
	cmd[4]=datalength+2;
	cmd[5]=_flag;
	cmd[6]=datalength;

	for(i=0;i<datalength;i++)
	{
		cmd[7+i]=carddata[i];
	}

	memset(resp,0,256);
	ret = ICC_Reader_Application(Rhand,USERCard,datalength+5+2,cmd,resp);
	if (ret<0 || (resp[ret-2]!=0x90))
	{
		strcpy(data,"写入文件失败--");
		strcat(data,"00DC-");
		strcat(data,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(data,a);
		return SB_ERR_WRITEINFO;
	}

	return 0;
}

long iWriteBTLVEX(char *EFid,int recNO,int datalength,TLV *tl,int size,char *data,unsigned char _flag) //size 数据项
{   
	Rhand = g_handle;
	int ret =0;
	char carddata[0x300]={0};
	int n=0;

	int start=0;
	char temp[256]={0};
	int len = strlen(data);
	if(len!=2*(datalength+2))
	{
		strcpy(data,"传入数据格式长度有误");
		return 1;
	}

	for(int j=0;j<(len/2);j++)
	{
		carddata[n++] = chartoint(data[2*j])*16+chartoint(data[2*j+1]);  
	}

	unsigned char resp[256];
	unsigned char cmd[300]={0};
	memset(resp,0,256);

	memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
	cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
	cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
	ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
	if(ret<2 || resp[0]!=0x90)
	{
		strcpy(data,"00A4-");
		strcat(data,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(data,a);
		return 1;
	}

	memset(cmd,0,300);
	memcpy(cmd,"\x00\xDC\x00\x04\x00",5);
	cmd[2]=recNO;
	cmd[4]=datalength+2;
	cmd[5]=carddata[0];
	cmd[6]=carddata[1];

	for(int i=0;i<datalength;i++)
	{
		cmd[7+i]=carddata[2+i];
	}

	memset(resp,0,256);
	ret = ICC_Reader_Application(Rhand,USERCard,datalength+5+2,cmd,resp);
	if (ret<0 || (resp[ret-2]!=0x90))
	{
		strcpy(data,"00DC-");
		strcat(data,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(data,a);
		return 1;
	}

	return 0;
}

long iWriteXHFileEX(char *EFid,int recNO,int datalength,CYC *cy,int size,char *data) //recNO 没有使用
{	
	Rhand = g_handle;
	int st ;
	int ret =0;
	char carddata[256]={0};
	char temp[256]={0};
	int n=0;
	int len =0;
	int j=0;
	int start = 0;
	for(int i=0;i<size;i++)
	{
		if(i==0)
			n=0;
		else 
		{
			n=0;
			for(int j=0;j<i;j++)
				n += cy[j].length;
		}
		memset(temp,0,256);
		int s = findChar(start,data,',');
		memcpy(temp,data+start,s-start);

		len = strlen(temp);

		if(len!=2*(cy[i].length) || (len%2)!=0)
		{
			strcpy(data,"输入的数据长度有误");
			return 1;
		}
		for( j=0;j<(len/2);j++)
		{
			carddata[n++] = chartoint(temp[2*j])*16+chartoint(temp[2*j+1]);  
		}
		for(j=0;j<(cy[i].length-len/2);j++)
			carddata[n++]=0xFF;

		start = s;
		start+=1;
	}

	unsigned char resp[256];
	unsigned char cmd[300]={0};
	memset(resp,0,256);
//	long ret;
	memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
	cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
	cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
	ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
	if(ret<2 || resp[0]!=0x90)
	{
		strcpy(data,"00A4-");
		strcat(data,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(data,a);
		return 1;
	}

	memset(cmd,0,300);
	memcpy(cmd,"\x00\xDC\x01\x04\x00",5);
	cmd[4]=datalength;
	cmd[2] = recNO;
	for(i=0;i<datalength;i++)
	{
		cmd[5+i]=carddata[i];
	}

	memset(resp,0,256);
	ret = ICC_Reader_Application(Rhand,USERCard,datalength+5,cmd,resp);
	if (ret<0 || (resp[ret-2]!=0x90))
	{
		strcpy(data,"00A4-");
		strcat(data,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(data,a);
		return 1;
	}
	return 0;
}

long iWriteXHFile(char *EFid,int recNO,int datalength,CYC *cy,int size,char *data) //recNO 没有使用
{	
	Rhand = g_handle;
	int st ;
	int ret =0;
	char carddata[256]={0};
	char temp[256]={0};
	int n=0;
	int len =0;
	int j=0;
	int start = 0;
	for(int i=0;i<size;i++)
	{
		if(i==0)
			n=0;
		else 
		{
			n=0;
			for(int j=0;j<i;j++)
				n += cy[j].length;
		}

		memset(temp,0,256);
		int s = findChar(start,data,',');

		memcpy(temp,data+start,s-start);
		if((cy[i].datastyle)==AN_style)
		{
			len = strlen(temp);
			if(len>cy[i].length)
			{
					strcpy(data,"传入数据长度越界");
					return SB_ERR_OPNUMOVER;
			}
			for( int j=0;j<len;j++)
			{
				carddata[n++]=temp[j];		
			}		
		}
		else if((cy[i].datastyle) == CN_style)
		{
			if(strcmp(EFid,"EF15")==0 && i==0)//交易序号	b	'02'
			{
				if(!IsDec1(temp))
				{
					strcat(data,"传入待写入数据有未能识别字符");
					return SB_ERR_OPNUMERROR;
				}
				int count = atoi(temp);
				if(count>65535)
				{
					strcat(data,"传入数据长度越界");
					return SB_ERR_OPNUMOVER;
				}
				char gwei[10]={0};
				sprintf(gwei,"%04x",count);
				for( j=0;j<2;j++)
				{
					carddata[n++] = chartoint(gwei[2*j])*16+chartoint(gwei[2*j+1]);  
				}
			}
			else 
			{
				if(!IsEffectChar1(temp))
				{
					strcat(data,"传入待写入数据有未能识别字符");
					return SB_ERR_OPNUMERROR;
				}
				len = strlen(temp);
				if((len%2)!=0)
				{
					strcat(temp,"F");
					len+=1;
				}
				if(len>2*(cy[i].length))
				{
					strcpy(data,"传入数据长度越界");
					return SB_ERR_OPNUMOVER;
				}
				for( j=0;j<(len/2);j++)
				{
					carddata[n++] = chartoint(temp[2*j])*16+chartoint(temp[2*j+1]);  
				}
				for(j=0;j<(cy[i].length-len/2);j++)
					carddata[n++]=0xFF;
			}
			
		}
		start = s;
		start+=1;
	}

	unsigned char resp[256];
	unsigned char cmd[300]={0};
	memset(resp,0,256);
//	long ret;
	memcpy(cmd, "\x00\xA4\x00\x00\x02",5);
	cmd[5]=chartoint(EFid[0])*16+chartoint(EFid[1]);
	cmd[6]=chartoint(EFid[2])*16+chartoint(EFid[3]);	
	ret = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);//X选择MF文件
	if(ret<2 || resp[0]!=0x90)
	{
		strcpy(data,"写入文件失败--");
		strcat(data,"00A4-");
		strcat(data,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(data,a);
		return SB_ERR_WRITEINFO;
	}

	memset(cmd,0,300);
	memcpy(cmd,"\x00\xE2\x00\x00\x00",5);
	cmd[4]=datalength;
//	cmd[2] = recNO;
	for(i=0;i<datalength;i++)
	{
		cmd[5+i]=carddata[i];
	}

	memset(resp,0,256);
	ret = ICC_Reader_Application(Rhand,USERCard,datalength+5,cmd,resp);
	if (ret<0 || (resp[ret-2]!=0x90))
	{
		strcpy(data,"写入文件失败--");
		strcat(data,"00A4-");
		strcat(data,EFid);
		char a[5]={0};
		if(ret>1)
			HexToStr1(&resp[ret-2],(unsigned char*)a,2);
		strcat(data,a);
		return SB_ERR_WRITEINFO;
	}
	return 0;
}

long PerPINChange(char*data,char*err)
{
		int st ;
	int ret =0;
	char result[256]={0};
	strdata="";
	sDisp="";
	strdata1 = data;
	
	char oldPassword[10]={0};
	char newPassword[10]={0};
//	char newPassword2[10]={0};

	long StartLen = strdata1.find("<",0); // <OLD>
	long EndLen = strdata1.find(">",StartLen); 

	long StartLen1 = strdata1.find("<",EndLen); //</OLD>
	long EndLen1 = strdata1.find(">",StartLen1); 

	long StartLen2 = strdata1.find("<",EndLen1); //<NEW>
	long EndLen2 = strdata1.find(">",StartLen2);
	
	long StartLen3 = strdata1.find("<",EndLen2); //</NEW>
	long EndLen3 = strdata1.find(">",StartLen3); 

	sDisp = strdata1.substr(EndLen+1,StartLen1-EndLen-1); //获取旧密码
	strcpy(oldPassword,sDisp.c_str());
	sDisp = strdata1.substr(EndLen2+1,StartLen3-EndLen2-1); //获取新密码
	strcpy(newPassword,sDisp.c_str());

	unsigned char resp[300]={0};
	unsigned char cmd[200]={0};

	int re = ICC_Reader_pre_PowerOn(Rhand,0x01,resp);
	for(int n=re-8;n<re;n++)
	{	
		_ATR[n-9] =  resp[n] ;
	}
	re = ICC_Reader_pre_PowerOn(Rhand,0x11,resp);
	if(re<0)
	{
		strcpy(err,"请放置SAM卡于SAM1卡槽");
		strcpy(data,"<DATA>99</DATA>");
		return 104;
	}
	memset(cmd,0,50);
	memcpy(cmd, "\x00\xA4\x04\x00\x0F\x73\x78\x31\x2E\x73\x68\x2E\xC9\xE7\xBB\xE1\xB1\xA3\xD5\xCF",20);
	//00A404000F7378312E73682EC9E7BBE1B1A3D5CF
	re = ICC_Reader_Application(Rhand,0x01,20,cmd,resp);//X选择MF文件
	if(resp[0]!=0x61)
	{
		strcpy(err,"卡类型不对");
		strcpy(data,"<DATA>99</DATA>");
		return 105;
	}
	
	memset(cmd,0,50);
	memcpy(cmd,"\x80\x5E\x01\x00\x07",5);

	for(int i=0;i<3;i++)
	{
		cmd[5+i]=chartoint(oldPassword[2*i])*16+chartoint(oldPassword[2*i+1]);
		cmd[9+i]=chartoint(newPassword[2*i])*16+chartoint(newPassword[2*i+1]);
	}
	cmd[8]=0xFF;
	memset(resp,0,30);
	re = ICC_Reader_Application(Rhand,0x01,12,cmd,resp);//X选择MF文件
	if(resp[0]!=0x90)
	{
		//strcpy(pErrMsg,"卡类型不对");
		switch(resp[1])
		{
		case 0xC6:strcpy(err,"还剩6次认证机会!");return 6;
		case 0xC5:strcpy(err,"还剩5次认证机会!");return 5;
		case 0xC4:strcpy(err,"还剩4次认证机会!");return 4;
		case 0xC3:strcpy(err,"还剩3次认证机会!");return 3;
		case 0xC2:strcpy(err,"还剩2次认证机会!");return 2;
		case 0xC1:strcpy(err,"还剩1次认证机会!");return 1;
		case 0xC0:strcpy(err,"还剩0次认证机会!");return -1;
		}
		return 105;
	}
	strcpy(data,"<DATA>00</DATA>");
	return 0;
}
long PerPINUnLock(char*data,char*err)
{

	unsigned char resp[300]={0};
	unsigned char cmd[200]={0};

	int re = ICC_Reader_pre_PowerOn(Rhand,0x01,resp);
	for(int n=re-8;n<re;n++)
	{	
		_ATR[n-9] =  resp[n] ;
	}
	re = ICC_Reader_pre_PowerOn(Rhand,0x11,resp);
	if(re<0)
	{
		strcpy(err,"请放置SAM卡于SAM1卡槽");
		strcpy(data,"<DATA>99</DATA>");
		return 104;
	}
	memset(cmd,0,50);
	memset(resp,0,20);
	memcpy(cmd, "\x00\xA4\x04\x00\x0F\x73\x78\x31\x2E\x73\x68\x2E\xC9\xE7\xBB\xE1\xB1\xA3\xD5\xCF",20);
	//00A404000F7378312E73682EC9E7BBE1B1A3D5CF
	re = ICC_Reader_Application(Rhand,0x01,20,cmd,resp);//X选择MF文件
	if(resp[0]!=0x61)
	{
		strcpy(err,"卡类型不对");
		strcpy(data,"<DATA>99</DATA>");
		return 105;
	}
	memcpy(cmd, "\x00\xA4\x00\x00\x02\xEF\x05",7);
	re = ICC_Reader_Application(Rhand,0x01,7,cmd,resp);
	memcpy(cmd, "\x00\xB2\x01\x04\x12",5);
	re = ICC_Reader_Application(Rhand,0x01,5,cmd,resp);

	if(re<0 || resp[re-2]!=0x90)
	{
		strcpy(err,"卡片为空卡");
		strcpy(data,"<DATA>99</DATA>");
		return 1;
	}
	char sdisp[3]={0};
	char saccm[40]={0};
	for(n=0;n<re;n++)
	{
		sprintf(sdisp,"%02x",resp[n]);
		strcat(saccm,sdisp);
	}
	//char str[33]={0}; 
	memcpy(_CityCode,saccm+4,32);

	unsigned char wordkey[4]={0x48,0x10,0x06};
	if(ExterAuthen(wordkey,2,err))
	{
		strcpy(err,"PUK认证失败!");
		strcpy(data,"<DATA>99</DATA>");
		return 104;
	}

	GetRandom();
	memset(cmd,0,50);
	memcpy(cmd, "\x00\xA4\x00\x00\x02\xDF\x01",7);
	int ret = ICC_Reader_Application(Rhand,0x11,7,cmd,resp);
	if(ret<2 || (resp[ret-2]!=0x90 && resp[ret-2]!=0x61))
	{
		strcpy(err,"PSAM卡类型不对!");
		strcpy(data,"<DATA>99</DATA>");
		return 105;
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
	ret = ICC_Reader_Application(Rhand,0x11,29,cmd,resp);
	if(ret<2 || (resp[ret-2]!=0x90))
	{
		strcpy(err,"PSAM卡类型不对!");
		strcpy(data,"<DATA>99</DATA>");
		return 105;
	}

	memset(cmd,0,50);
	memset(resp,0,30);
	memcpy(cmd, "\x80\xFA\x05\x00\x10",5);//计算MAC
	memcpy(cmd+5,"\x00\x00\x00\x00\x00\x00\x00\x00",8);
	memcpy(cmd+13,"\x84\x24\x00\x00\x04",5);
	memcpy(cmd+18,"\x80\x00\x00",3);
	ret = ICC_Reader_Application(Rhand,0x11,21,cmd,resp);
	if(ret<2 || (resp[ret-2]!=0x61))
	{
		strcpy(err,"PSAM卡类型不对!");
		strcpy(data,"<DATA>99</DATA>");
		return 105;
	}
	memset(cmd,0,50);
	memset(resp,0,30);
	memcpy(cmd, "\x00\xc0\x00\x00\x08",5);
	ret = ICC_Reader_Application(Rhand,0x11,5,cmd,resp);
	if(ret<2 || (resp[ret-2]!=0x90))
	{
		strcpy(err,"PSAM卡类型不对!");
		strcpy(data,"<DATA>99</DATA>");
		return 105;
	}
	unsigned char MacData[10]={0};
	memcpy(MacData,resp,4);//得到MAC

	memset(cmd,0,50);
	memcpy(cmd,"\x84\x24\x00\x00\x04",5);
	memcpy(cmd+5,MacData,4);

	ret = ICC_Reader_Application(Rhand,0x01,9,cmd,resp);
	if(resp[0]!=0x90 || ret<2)
	{
		strcpy(err,"解锁PIN码出错");
		strcpy(data,"<DATA>99</DATA>");
		return 105;
	}
	strcpy(data,"<DATA>00</DATA>");
	return 0;
}

int GetAtrCityCode(char *err)
{
	unsigned char resp[300]={0};
	unsigned char cmd[200]={0};
	Rhand = g_handle;

/*	cardtype=0;
	int re = ICC_Reader_pre_PowerOn(Rhand,USERCard,resp);
	if(re<=0)
	{
		re = ICC_Reader_4428_PowerOn(Rhand,resp);
		if(re!=0 || Response[0]==0xff)
		{
			strcpy(pErrMsg,"卡片上电失败!");
			return SB_ERR_CARDMISS;
		}
		else
			cardtype=2;
		return 0;
	}	*/
	
	int re = ICC_Reader_pre_PowerOn(Rhand,USERCard,resp);
	if(re<0)
	{
		strcpy(err,"无卡请插卡");
		return SB_ERR_CARDMISS;
	}
	for(int n=re-8;n<re;n++)
	{	
		_ATR[n-re+8] =  resp[n] ;
	}

	
	memset(cmd,0,50);
	memset(resp,0,20);
	memcpy(cmd, "\x00\xA4\x04\x00\x0F\x73\x78\x31\x2E\x73\x68\x2E\xC9\xE7\xBB\xE1\xB1\xA3\xD5\xCF",20);
	//00A404000F7378312E73682EC9E7BBE1B1A3D5CF
	re = ICC_Reader_Application(Rhand,USERCard,20,cmd,resp);//X选择MF文件
	if(resp[re-2]!=0x61 && resp[re-2]!=0x90)
	{
		strcpy(err,"卡类型不对");
		return SB_ERR_CARDTYPE ;
	}
	memcpy(cmd, "\x00\xA4\x00\x00\x02\xEF\x05",7);
	re = ICC_Reader_Application(Rhand,USERCard,7,cmd,resp);
	memcpy(cmd, "\x00\xB2\x01\x04\x12",5);
	re = ICC_Reader_Application(Rhand,USERCard,5,cmd,resp);

	if(re<0 || resp[re-2]!=0x90)
	{
		strcpy(err,"卡片为空卡");
		return SB_ERR_CARDTYPE ;
	}
	char sdisp[3]={0};
	char saccm[40]={0};
	for(n=0;n<re;n++)
	{
		sprintf(sdisp,"%02x",resp[n]);
		strcat(saccm,sdisp);
	}

	memcpy(_CityCode,saccm+4,32);
	return 0;
}