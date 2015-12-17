// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__AF970B1B_410C_4BC1_B5FC_69D56E068DFC__INCLUDED_)
#define AFX_STDAFX_H__AF970B1B_410C_4BC1_B5FC_69D56E068DFC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers

#include <windows.h>
#include "Application.h"

#define AN_style  0x01
#define CN_style  0x0F
#define B_style	  0xFF

#define PSAM_Slot3 0x13
#define PSAM_Slot2 0x12

extern int  PSAM_Slot;
extern int  USERCard;
extern bool moneyFlag;
extern long g_handle;

extern unsigned char _CityCode[33];//城市代码
extern unsigned char _ATR[20];//ATR分散因子
extern unsigned char _Random[10];

#include "math.h"

#include <iostream>
#include <fstream>
using namespace std;
// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__AF970B1B_410C_4BC1_B5FC_69D56E068DFC__INCLUDED_)
