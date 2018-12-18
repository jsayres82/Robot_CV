#if !defined(AFX_VCDMAPSTRINGSCOMBO_H__E9950616_80B5_44CB_97EF_1210D67BD408__INCLUDED_)
#define AFX_VCDMAPSTRINGSCOMBO_H__E9950616_80B5_44CB_97EF_1210D67BD408__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VCDMapStringsCombo.h : header file
//

#include "VCDPropertyCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// VCDMapStringsCombo window

class CVCDMapStringsCombo
	:	public CComboBox,
		public IVCDPropertyCtrl
{
// Construction
public:
	CVCDMapStringsCombo();

// Attributes
public:

// Operations
public:
	void			setupCtrl( DShowLib::tIVCDMapStringsPropertyPtr pItf );

	virtual void	update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCDMapStringsCombo)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVCDMapStringsCombo();

protected:
	DShowLib::tIVCDMapStringsPropertyPtr	m_pMapStringsItf;

	void	initialUpdate();
	void	scrollUpdate();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVCDMapStringsCombo)
	afx_msg void OnSelchange();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCDMAPSTRINGSCOMBO_H__E9950616_80B5_44CB_97EF_1210D67BD408__INCLUDED_)
