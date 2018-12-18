#if !defined(AFX_VCDSWITCHCHECK_H__9DACC7BD_5F85_4894_A172_8AB1324E59F8__INCLUDED_)
#define AFX_VCDSWITCHCHECK_H__9DACC7BD_5F85_4894_A172_8AB1324E59F8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VCDSwitchCheck.h : header file
//

#include "VCDPropertyCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CVCDSwitchCheck window

class CVCDSwitchCheck
	:	public CButton,
		public IVCDPropertyCtrl
{
// Construction
public:
	CVCDSwitchCheck();

// Attributes
public:

// Operations
public:
	void			setupCtrl( DShowLib::tIVCDSwitchPropertyPtr pItf, tControlVectorPtr pGroup );

	virtual void	update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCDSwitchCheck)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVCDSwitchCheck();

protected:
	DShowLib::tIVCDSwitchPropertyPtr	m_pSwitchItf;
	tControlVectorPtr					m_pGroup;

	void	initialUpdate();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVCDSwitchCheck)
	afx_msg void OnClicked();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCDSWITCHCHECK_H__9DACC7BD_5F85_4894_A172_8AB1324E59F8__INCLUDED_)
