#if !defined(AFX_VCDBUTTONBUTTON_H__B3F78025_1624_4538_B740_6DC2CAD6F666__INCLUDED_)
#define AFX_VCDBUTTONBUTTON_H__B3F78025_1624_4538_B740_6DC2CAD6F666__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VCDButtonButton.h : header file
//

#include "VCDPropertyCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CVCDButtonButton window

class CVCDButtonButton
	:	public CButton,
		public IVCDPropertyCtrl
{
// Construction
public:
	CVCDButtonButton();

// Attributes
public:

// Operations
public:
	void			setupCtrl( DShowLib::tIVCDButtonPropertyPtr pItf, tControlVectorPtr pGroup );

	virtual void	update();


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCDButtonButton)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVCDButtonButton();

protected:
	DShowLib::tIVCDButtonPropertyPtr	m_pButtonItf;
	tControlVectorPtr					m_pGroup;

	// Generated message map functions
protected:
	//{{AFX_MSG(CVCDButtonButton)
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCDBUTTONBUTTON_H__B3F78025_1624_4538_B740_6DC2CAD6F666__INCLUDED_)
