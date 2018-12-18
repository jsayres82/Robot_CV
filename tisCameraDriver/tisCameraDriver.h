// VCD Simple Property.h : main header file for the VCD SIMPLE PROPERTY application
//

#if !defined(AFX_TISCAMERADRIVER_H__5838EBB8_448E_4E5A_A068_853E1EEB3C22__INCLUDED_)
#define AFX_TISCAMERADRIVER_H__5838EBB8_448E_4E5A_A068_853E1EEB3C22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CVCDSimplePropertyApp:
// See VCD Simple Property.cpp for the implementation of this class
//

class CVCDSimplePropertyApp : public CWinApp
{
public:
	CVCDSimplePropertyApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCDSimplePropertyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CVCDSimplePropertyApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TISCAMERADRIVER_H__5838EBB8_448E_4E5A_A068_853E1EEB3C22__INCLUDED_)
