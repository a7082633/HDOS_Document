// JLTest.h : main header file for the JLTEST application
//

#if !defined(AFX_JLTEST_H__17F450A1_C136_4D8D_B3F1_ED488EB77725__INCLUDED_)
#define AFX_JLTEST_H__17F450A1_C136_4D8D_B3F1_ED488EB77725__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CJLTestApp:
// See JLTest.cpp for the implementation of this class
//

class CJLTestApp : public CWinApp
{
public:
	CJLTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CJLTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CJLTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_JLTEST_H__17F450A1_C136_4D8D_B3F1_ED488EB77725__INCLUDED_)
