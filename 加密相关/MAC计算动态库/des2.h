// des.h : main header file for the DES DLL
//
/*
#if !defined(AFX_DES_H__48495758_93AF_44D5_889E_B97C7240BF22__INCLUDED_)
#define AFX_DES_H__48495758_93AF_44D5_889E_B97C7240BF22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
//	#error include 'stdafx.h' before including this file for PCH
#endif
*/
//#include "resource.h"		// main symbols

/*DES����
dat������ǰ������ �� ���ܺ������ ,����8�ֽڣ�
key1:��Կ������8�ֽ�
mode:����ģʽ  0���ܣ�1���ܣ�
*/
void des(BYTE *dat, BYTE *key1, BYTE mode);	


/*TDES����
dat������ǰ������ �� ���ܺ�����ݣ�����8�ֽ�
key1:��Կǰ8�ֽڣ�
key2:��Կ��8�ֽڣ�
mode:����ģʽ  0���ܣ�1���ܣ�
*/
void tri_des(BYTE *dat, BYTE *key1, BYTE *key2, BYTE mode);

/////////////////////////////////////////////////////////////////////////////
// CDesApp
// See des.cpp for the implementation of this class
/*

class CDesApp : public CWinApp
{
public:
	CDesApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDesApp)
	//}}AFX_VIRTUAL

	//{{AFX_MSG(CDesApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
*/

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

//#endif // !defined(AFX_DES_H__48495758_93AF_44D5_889E_B97C7240BF22__INCLUDED_)
