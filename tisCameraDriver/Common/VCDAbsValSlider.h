#if !defined(AFX_VCDABSVALSLIDER_H__76935363_A394_4ACF_8602_27B5ABADB5A1__INCLUDED_)
#define AFX_VCDABSVALSLIDER_H__76935363_A394_4ACF_8602_27B5ABADB5A1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VCDAbsValSlider.h : header file
//

#include "VCDPropertyCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CVCDAbsValSlider window

class CVCDAbsValSlider
	:	public CSliderCtrl,
		public IVCDPropertyCtrl
{
// Construction
public:
	CVCDAbsValSlider();

// Attributes
public:

// Operations
public:
	void			setupCtrl( DShowLib::tIVCDAbsoluteValuePropertyPtr pItf );

	virtual void	update();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCDAbsValSlider)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVCDAbsValSlider();

protected:
	DShowLib::tIVCDAbsoluteValuePropertyPtr	m_pAbsValItf;

	void	initialUpdate();
	void	scrollUpdate();


	int GetSliderPos();
	double GetAbsVal();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVCDAbsValSlider)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCDABSVALSLIDER_H__76935363_A394_4ACF_8602_27B5ABADB5A1__INCLUDED_)
