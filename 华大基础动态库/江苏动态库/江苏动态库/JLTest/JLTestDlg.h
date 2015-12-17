// JLTestDlg.h : header file
//

#if !defined(AFX_JLTESTDLG_H__92FF3A1C_052F_43F7_8815_2C7C5CC77686__INCLUDED_)
#define AFX_JLTESTDLG_H__92FF3A1C_052F_43F7_8815_2C7C5CC77686__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CJLTestDlg dialog

class CJLTestDlg : public CDialog
{
// Construction
public:
	CJLTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CJLTestDlg)
	enum { IDD = IDD_JLTEST_DIALOG };
	CRichEditCtrl	m_rich;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJLTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	CString temp;
	// Generated message map functions
	//{{AFX_MSG(CJLTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnOpenport();
	afx_msg void OnChecktype();
	afx_msg void OnClear();
	afx_msg void OnGcardno();
	afx_msg void OnPersno();
	afx_msg void OnGetatr();
	afx_msg void OnTersn();
	afx_msg void OnOk();
	afx_msg void OnButton1();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JLTESTDLG_H__92FF3A1C_052F_43F7_8815_2C7C5CC77686__INCLUDED_)
