#if !defined(AFX_VCDRANGESLIDER_H__3FD0D140_704B_4724_A4C4_D5D6336686EA__INCLUDED_)
#define AFX_VCDRANGESLIDER_H__3FD0D140_704B_4724_A4C4_D5D6336686EA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// VCDRangeSlider.h : header file
//

#include "VCDPropertyCtrl.h"

/////////////////////////////////////////////////////////////////////////////
// CVCDRangeSlider window

class CVCDRangeSlider
	:	public CSliderCtrl,
		public IVCDPropertyCtrl
{
// Construction
public:
	CVCDRangeSlider();

// Attributes
public:	

// Operations
public:
	void			setupCtrl( DShowLib::tIVCDRangePropertyPtr pItf );

	virtual void	update();
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCDRangeSlider)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CVCDRangeSlider();

protected:
	DShowLib::tIVCDRangePropertyPtr	m_pRangeItf;

	void	initialUpdate();
	void	scrollUpdate();

	// Generated message map functions
protected:
	//{{AFX_MSG(CVCDRangeSlider)
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_VCDRANGESLIDER_H__3FD0D140_704B_4724_A4C4_D5D6336686EA__INCLUDED_)
