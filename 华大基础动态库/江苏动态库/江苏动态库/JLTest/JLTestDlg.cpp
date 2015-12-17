// JLTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JLTest.h"
#include "JLTestDlg.h"
#include "SiCard.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJLTestDlg dialog

CJLTestDlg::CJLTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CJLTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CJLTestDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CJLTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CJLTestDlg)
	DDX_Control(pDX, IDC_RICHEDIT1, m_rich);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CJLTestDlg, CDialog)
	//{{AFX_MSG_MAP(CJLTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_OPENPORT, OnOpenport)
	ON_BN_CLICKED(IDC_CHECKTYPE, OnChecktype)
	ON_BN_CLICKED(IDC_CLEAR, OnClear)
	ON_BN_CLICKED(IDC_GCARDNO, OnGcardno)
	ON_BN_CLICKED(IDC_PERSNO, OnPersno)
	ON_BN_CLICKED(IDC_GETATR, OnGetatr)
	ON_BN_CLICKED(IDC_TERSN, OnTersn)
	ON_BN_CLICKED(IDC_OK, OnOk)
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CJLTestDlg message handlers

BOOL CJLTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CJLTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CJLTestDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CJLTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CJLTestDlg::OnOpenport() 
{
	// TODO: Add your control notification handler code here
	char err[100]={0};
	int pReaderHandle=0;
	char pErrMsg[100]={0};
	char nCardFlag[100]={0};
	char pInputPin[20]={0};
	int re;
	re = iOpenPort(err);

	char pCardAtr[100]={0};
/*	re = iCardReadAtr(pReaderHandle, pCardAtr, pErrMsg);
	if(re!=0)
	{
		m_rich.GetWindowText(temp);
		m_rich.SetWindowText(temp+"\r"+"获取ATR失败" + pErrMsg);
	//	return;
	}*/
//	re = iCardControl (pReaderHandle,"02",NULL ,pErrMsg);//NULL
	
//	iCardControl(pReaderHandle,const char * pCtrlType, const char * pCtrlInfo,char*  pErrMsg);
//	re = iReadPin(pReaderHandle,"01",pInputPin,pErrMsg);
/*
	char pCardNo[100]={0};
	re = iReadCardNoX(pCardNo, pErrMsg);
	if(re!=0)
	{
		m_rich.GetWindowText(temp);
		m_rich.SetWindowText(temp+"\r"+"获取卡号失败" + pErrMsg);
		return;
	}

	char pCardAtr[100]={0};
	re = iCardReadAtrX(pInputPin,  pCardAtr, pErrMsg);
	if(re!=0)
	{
		m_rich.GetWindowText(temp);
		m_rich.SetWindowText(temp+"\r"+"获取ATR失败" + pErrMsg);
		return;
	}

	re = iCardControl (pReaderHandle,"01", "123455|123456",pErrMsg);
	if(re!=0)
	{
		m_rich.GetWindowText(temp);
		m_rich.SetWindowText(temp+"\r"+"密钥控制失败" + pErrMsg);
		return;
	}

	re = iReadCardFlag(pReaderHandle,nCardFlag , pErrMsg);
	if(re!=0)
	{
		m_rich.GetWindowText(temp);
		m_rich.SetWindowText(temp+"\r"+"读取卡类型失败");
		return;
	}
	m_rich.GetWindowText(temp);
	m_rich.SetWindowText(temp+"\r"+"卡类型："+nCardFlag);
*/
//	re = iReadCardFlagX(nCardFlag , pErrMsg);
/*	char pBmpFile[100]={0};
	char pName[100]={0};
	char pSex[100]={0}; 
	char pNation[100]={0};
	char pBirth[100]={0};
	char pAddress[100]={0};
	char pCertNo[100]={0};
	char pDepartment[100]={0};
	char pExpire[100]={0};
	strcpy((char*)pBmpFile,"D:/zp.bmp");
	re = iReadIDMsg(pReaderHandle, pBmpFile, pName, pSex, pNation, pBirth, pAddress, pCertNo, pDepartment , pExpire,pErrMsg);
	m_rich.GetWindowText(temp);
	m_rich.SetWindowText(temp+"\r"+pName);
	m_rich.GetWindowText(temp);
	m_rich.SetWindowText(temp+"\r"+pSex);
*/

	char pFileAddr[600]={0};
	char pOutDataBuff[0x2002]={0};
	strcpy(pInputPin,"123456");

//	strcpy(pFileAddr,"DF02EF07:01$DF02EF07:02$DF02EF07:03$DF02EF07:04$");//DF02EF15|99:03|99:04|$DF02EF07:03$DF02EF07:04$
//	strcpy(pFileAddr,"DF02EF15|99:01|99:02|0F|1F|$");
	strcpy(pFileAddr,"MFEF06|08|09|0A|0D|$");//MFEF05|01|02|03|04|05|06|07|$
//	strcpy(pFileAddr,"DF02EF16:03$DF02EF16:04$DF02EF16:01$DF02EF16:02$");
//	strcpy(pFileAddr,"DF03EF05|61|98|62|63|64|65|66|$");
//	strcpy(pFileAddr,"DF03EF06|4A:01|45|46|4A:02|47|48|49|67|6B|$DF03EF06|4A:03|$");
//	strcpy(pFileAddr,"DF03EF19|79:01|$");
//	strcpy(pFileAddr,"DF04EF05|81|84|87|8C|80|8B|8A|83|86|89|7D|7E|7F|8D|8E|8F|$");
//	strcpy(pFileAddr,"DF04EF06|90|92|93|$");
//  strcpy(pFileAddr,"DF04EF08:01$DF04EF08:02$DF04EF08:03$DF04EF08:04$DF04EF08:05$DF04EF08:06$DF04EF08:07$DF04EF08:08$DF04EF08:09$DF04EF08:0A$");
//	strcpy(pFileAddr,"DF04EF15:01$DF04EF15:02$DF04EF15:03$DF04EF15:04$DF04EF15:05$DF04EF15:06$DF04EF15:07$DF04EF15:08$");
//	strcpy(pFileAddr,"DF07EF05|F0|F1|F2|F3|F4|$");
//	strcpy(pFileAddr,"MFEF0D|EA|EB|EC|ED|$MFEF06|09|0A|0D|$");
//	strcpy(pFileAddr,"DF01EF05|20|21|0E|$DF01EF06|23|24|28|2C|2D|$");
//	strcpy(pFileAddr,"DF01EF07|29|$DF01EF08|2B|$DF01EF09|2E|30|32|$DF01EF0A|37|38|39|$");
//	strcpy(pFileAddr,"DF01EF15|40:01|40:02|2A|$DF01EF15|40:03|$");
//	strcpy(pFileAddr,"DF02EF05|42|41|43:01|43:02|$DF02EF05|44:01|44:02|$");
//	strcpy(pFileAddr,"DF02EF06|4C|4B|4D|60|4F|50|3A|$");
//	strcpy(pFileAddr,"DF02EF09|55|56|96|97|$");
//	strcpy(pFileAddr,"DF02EF15|99:01|99:02|$DF02EF15|99:03|99:04|$DF02EF15|0F|1F|$");
//	strcpy(pFileAddr,"DF02EF16:01$DF02EF16:02$DF02EF16:03$DF02EF16:04$");
//	strcpy(pFileAddr,"DF03EF07|70|71|6E|6C|6F|73|$");
//	strcpy(pFileAddr,"DF03EF15|7A|7B|7C|$");
//	strcpy(pFileAddr,"DF03EF16|51|5F|$");
//	strcpy(pFileAddr,"DF03EF17|3B|3C|3D|3E|3F|$");
//	strcpy(pFileAddr,"DF03EF18|74|75|76|77|78|$");
//	strcpy(pFileAddr,"DF07EF06|F5|F6|F7|$");
//	strcpy(pFileAddr,"DF07EF07|F8|F9|$");

//	strcpy(pFileAddr,"MFEF06|08|$DF01EF06|23|$MFEF06|09|$DF01EF06|24|$MFEF06|4E|$DF01EF06|28|$MFEF06|0A|$DF01EF06|2C|$MFEF06|0B|$DF01EF06|2D|$MFEF06|0C|$MFEF06|0D|$"); 

//	strcpy(pFileAddr,"DF03EF06|45|$DF03EF06|46|$DF03EF06|47|$DF03EF06|48|$DF03EF06|49|$DF03EF06|67|$DF03EF06|6B|$DF03EF06|4A:01|$DF03EF06|4A:02|$");
//	strcpy(pFileAddr,"DF05EF05|A0|$DF05EF05|A1|$DF05EF05|B9|$DF05EF05|A2|$DF05EF05|A9|$DF05EF05|A3:01|$DF05EF05|A3:02|$DF05EF05|A3:03|$DF05EF05|BA|$");
//	strcpy(pFileAddr,"DF06EF05|C0|$DF06EF05|C2|$DF06EF05|C3:01|$DF06EF05|C4:01|$DF06EF05|C5:01|$");
//	strcpy(pFileAddr,"DF06EF06|C6|$DF06EF06|C8|$DF06EF06|C9|$DF06EF06|CA|$DF06EF06|CB|$DF06EF06|CC|$DF06EF06|CD|$DF06EF06|CE|$DF06EF06|CF|$");
/*	
	CString ddt;
	GetDlgItemText(IDC_EDIT2,ddt);*/
//	strcpy(pFileAddr,"DF01EF05|20|$MFEF05|02|$DF01EF05|21|$MFEF05|03|$DF01EF05|0E|$MFEF05|04|$MFEF05|05|$MFEF05|06|$MFEF05|07|$");
//	strcpy(pFileAddr,"MFEF05|02|03|04|05|06|07|$");
//	strcpy(pFileAddr,"DF06EF06|C6|$");
//	strcpy(pFileAddr,"DF01EF16|01|$");
//	strcpy(pFileAddr,"DF04EF05|81|$DF04EF05|84|$DF04EF05|87|$DF04EF05|8C|$DF04EF05|80|$DF04EF05|8B|$DF04EF05|8A|$DF04EF05|83|$DF04EF05|86|$DF04EF05|89|$DF04EF05|7D|$DF04EF05|7E|$DF04EF05|7F|$DF04EF05|8D|$DF04EF05|8E|$DF04EF05|8F|$");
//	strcpy(pFileAddr,"DF04EF15:02$");
//	strcpy(pFileAddr,"DF03EF06|45|$DF03EF06|46|$DF03EF06|47|$DF03EF06|48|$DF03EF06|49|$DF03EF06|67|$DF03EF06|6B|$DF03EF06|4A:01|$DF03EF06|4A:02|$DF03EF06|4A:03|$");//
//	strcpy(pFileAddr,"DF06EF06|C6|C8|C9|CA|CB|CC|CD|CE|CF|D0|D1|D2|D3|D4|D5|D6|D7|D8|D9|DA|DB|DC|DD|DE|DF|E0|E1|E2|$");
//
//	strcpy(pFileAddr,"DF06EF06|C6|$DF06EF06|C8|$DF06EF06|C9|$DF06EF06|CA|$DF06EF06|CB|$DF06EF06|CC|$DF06EF06|CD|$DF06EF06|CE|$DF06EF06|CF|$DF06EF06|D0|$DF06EF06|D1|$DF06EF06|D2|$DF06EF06|D3|$DF06EF06|D4|$DF06EF06|D5|$DF06EF06|D6|$DF06EF06|D7|$DF06EF06|D8|$DF06EF06|D9|$DF06EF06|DA|$DF06EF06|DB|$DF06EF06|DC|$DF06EF06|DD|$DF06EF06|DE|$DF06EF06|DF|$DF06EF06|E0|$DF06EF06|E1|$DF06EF06|E2|$");
	strcpy(pFileAddr,"MFEF05|06|$");
	re = iReadCard(pInputPin, pFileAddr, pOutDataBuff);
	if(re!=0)
	{
		m_rich.GetWindowText(temp);
		m_rich.SetWindowText(temp+"\r"+pErrMsg);
	}
	m_rich.GetWindowText(temp);
	m_rich.SetWindowText(temp+'\r'+pOutDataBuff);
/**/
	char pWriteDataBuff[1512]={0};

//	strcpy(pWriteDataBuff,"环卫,20130630,20130630,南京市卫生局,|清洁,20130630,20130630,南京市卫生局,|门卫,20130630,20130630,南京市卫生局,|保安,20130630,20130630,南京市卫生局,|");//没有循环
//	strcpy(pWriteDataBuff,"3601231999|201306|1|1|张三|1|19850111|");
//	strcpy(pWriteDataBuff,"1|江苏南京|010203|江苏南京珠江路|000102|123123|李四|12345678|");
//	strcpy(pWriteDataBuff,"1|0|江苏省卫生厅|12345678|01|086|11|20130629|");
//	strcpy(pWriteDataBuff,"123,112233,南京大学,|001,321,江苏大学,|11|002,345,南京理工,|");
//	strcpy(pWriteDataBuff,"001|002|1234567,2,江苏省人社厅,20130630,1234567890,|2234566,1,江苏省南京市社保局,20130630,1234567891,|002,江苏省人社厅,20130630,1234567890,|001,江苏省人社厅,20130630,1234567890,|");
//	strcpy(pWriteDataBuff,"20130630|12345|010203|20130630|000102|123123|清洁工|");
//	strcpy(pWriteDataBuff,"20130630|江苏省南京市社保局|20130630|000|");
//	strcpy(pWriteDataBuff,"001,20130701,112233,|002,20130701,112233,|003,20130701,112233,|004,20130701,112233,|20130801|223344|");
//	strcpy(pWriteDataBuff,"001,20130701,20130701,20130701,112233,|002,20130701,20130701,20130701,112233,|003,20130701,20130701,20130701,112233,|004,20130701,20130701,20130701,112233,|");
//	strcpy(pWriteDataBuff,"112233|20121129|20121129|015|015|086|086|");
//	strcpy(pWriteDataBuff,"1234567890123|02|02|1|0|20121129|南京市劳动局|21,2,曾亚义,20120111,南京市卫生局,|11,1,曾亚义1,20120111,南京市卫生局1,|23,3,曾亚义2,20120111,南京市卫生局2,|");
//	strcpy(pWriteDataBuff,"123|201211|20121129|20121129|20121129|20121129|");
//	strcpy(pWriteDataBuff,"201211|20121129|20121129|");
//	strcpy(pWriteDataBuff,"201211|1|");
//	strcpy(pWriteDataBuff,"12345678901234|江苏省南京市3街道办|1|20120312|江苏省南京市劳动局|");
//	strcpy(pWriteDataBuff,"123|112233|11|20120312|20120312|");
//	strcpy(pWriteDataBuff,"123,112233,20120312,20120312,212.5,123,20120312,|");
//	strcpy(pWriteDataBuff,"12345678|201211|36012312345|11|20121129|1234578|20121129|123456789|123456789|123456789|123456789|123456789|123456789|123456789|123456789|1|");
//	strcpy(pWriteDataBuff,"1|12.123|12|");
//	strcpy(pWriteDataBuff,"0001,2,|0002,2,|0003,3,|0004,4,|0005,5,|0006,6,|0007,7,|0008,8,|");
//	strcpy(pWriteDataBuff,"12|1|12345678|345|87654321|");
//	strcpy(pWriteDataBuff,"123|20130101|南京市卫生局|");

//	strcpy(pWriteDataBuff,"4c0420130630|4b053132333435|4d03010203|600420130630|4f06303030313032|5003123123|3a07c7e5bde0b9a400|");
//	strcpy(pWriteDataBuff,"551032303133303633300000000000000000|5646bdadcbd5caa1c4cfbea9cad0c9e7b1a3bed600000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000011|960420130630|9703000077|");
//	strcpy(pWriteDataBuff,"4034570331323358031122335928c4cfbea9b4f3d1a70000000000000000000000000000000000000000000000000000000000000000|4034570330303158033210005928bdadcbd5b4f3d1a70000000000000000000000000000000000000000000000000000000000000000|2a0111|4034570330303258033450005928c4cfbea9c0edb9a40000000000000000000000000000000000000000000000000000000000000000|");
//	strcpy(pWriteDataBuff,"4203303031|4103303032|436c5a07313233343536375b01205c46bdadcbd5caa1c8cbc9e7ccfc000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005d04201306305e1031323334353637383930000000000000|436c5a07323233343536365b01105c46bdadcbd5caa1c4cfbea9cad0c9e7b1a3bed6000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000005d04201306305e1031323334353637383931000000000000|446533033030323446bdadcbd5caa1c8cbc9e7ccfc00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000350420130630361031323334353637383930000000000000|446533033030313446bdadcbd5caa1c8cbc9e7ccfc00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000350420130630361031323334353637383930000000000000|");

//	strcpy(pWriteDataBuff,"南京市卫生局|1251332|12,11223344,22334455,123,223344,|12,11223344,22334455,123,223344,|12,11223344,22334455,123,223344,|");

//	strcpy(pWriteDataBuff,"12,,2245,12345678,,12,2223,|");

//	strcpy(pWriteDataBuff,"|");

	strcpy(pFileAddr,"MFEF05|06|$");
	strcpy(pWriteDataBuff,"20141010|$");
						 //  Uv3|Xy5|ZYZG001,A,职业资格证书发证机构名称职业资格证书发证机构名称职业资格证书发证机构01,19910101,ZYZGZS0001,|ZYZG002,B,职业资格证书发证机构名称职业资格证书发证机构名称职业资格证书发证机构02,19920202,ZYZGZS0002,|ZY1,职业资格注册登记机构名称职业资格注册登记机构名称职业资格注册登记机构01,19930303,KFABC00001,|ZY2,职业资格注册登记机构名称职业资格注册登记机构名称职业资格注册登记机构02,19940404,KFABC00002,|
	re = iWriteCard(pInputPin, pFileAddr, pWriteDataBuff,pErrMsg);
	if(re!=0)
	{
		m_rich.GetWindowText(temp);
		m_rich.SetWindowText(temp+"\r"+pErrMsg);
	}

// 	strcpy(pFileAddr,"DF04EF06|92|$");
// 	strcpy(pWriteDataBuff,"298.196|");
// 
// 	re = iWriteCard(pInputPin, pFileAddr, pWriteDataBuff,pErrMsg);
// 	if(re!=0)
// 	{
// 		m_rich.GetWindowText(temp);
// 		m_rich.SetWindowText(temp+"\r"+pErrMsg);
// 	}
/**/
//	strcpy(pFileAddr,"MFEF07|00|$");
//	strcpy(pWriteDataBuff,"");

/*	re = iWriteCard(pReaderHandle,pInputPin, pFileAddr, pWriteDataBuff,pErrMsg);
	if(re!=0)
	{
		m_rich.GetWindowText(temp);
		m_rich.SetWindowText(temp+"\r"+pErrMsg);
	}



//	strcpy(pFileAddr,"MFEF05|02|03|04|05|06|07|$");
	re = iReadCard(pReaderHandle,pInputPin, pFileAddr, pOutDataBuff, pErrMsg);
	if(re!=0)
	{
		m_rich.GetWindowText(temp);
		m_rich.SetWindowText(temp+"\r"+pErrMsg);
	}
	m_rich.GetWindowText(temp);
	m_rich.SetWindowText(temp+'\r'+pOutDataBuff);
*/
/*	char pCardNo[100]={0};
	re = iReadCardNo(pReaderHandle, pCardNo,  pErrMsg);
	re = iClosePort(pReaderHandle,  pErrMsg);
	re = iReadCardNoX(pCardNo, pErrMsg);
*/
}

void CJLTestDlg::OnChecktype() 
{
	// TODO: Add your control notification handler code her

}

void CJLTestDlg::OnClear() 
{
	// TODO: Add your control notification handler code here
	m_rich.SetWindowText("");
}
#include "SSSE32.H"
#pragma comment(lib,"SSSE32.lib")

void CJLTestDlg::OnGcardno() 
{
	long ReaderHandle=ICC_Reader_Open("USB1");
	unsigned char rlen;
	unsigned char cpass[7]={0};
	ICC_GetInputPass(ReaderHandle, 20, &rlen, 1,1,cpass);//获取密码，带超时 start==0用内置键盘，start==1用外置键盘

	CString str,str1,str2;
	for (int i=0;i<rlen;i++)
	{
		str.Format("%d",cpass[i]);
		str2+=str;
	}
	str1.Format("%d",rlen);

	MessageBox(str2);
	MessageBox("rlen= "+str1);
}

void CJLTestDlg::OnPersno() 
{
	
}

void CJLTestDlg::OnGetatr() 
{

}

void CJLTestDlg::OnTersn() 
{
	// TODO: Add your control notification handler code here
		

}

void CJLTestDlg::OnOk() 
{
	// TODO: Add your control notification handler code here
	
}

void CJLTestDlg::OnButton1() 
{
	// TODO: Add your control notification handler code here
	long Rhandle = ICC_Reader_Open("USB1");
	
}
