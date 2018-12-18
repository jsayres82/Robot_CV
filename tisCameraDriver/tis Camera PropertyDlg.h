// VCD Simple PropertyDlg.h : header file
//

#if !defined(AFX_IDD_TISCAMERAPROPERTYDLG_H__FE179C98_4D80_4DFD_BE48_89A5E3BE32BF__INCLUDED_)
#define AFX_IDD_TISCAMERAPROPERTYDLG_H__FE179C98_4D80_4DFD_BE48_89A5E3BE32BF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Common/SimplePropertyAccess.h"
/////////////////////////////////////////////////////////////////////////////
// CVCDSimplePropertyDlg dialog

class CVCDSimplePropertyDlg : public CDialog
{
// Construction
public:
	bool selectDevice();
	CVCDSimplePropertyDlg(CWnd* pParent = NULL);	// standard constructor



// Dialog Data
	//{{AFX_DATA(CVCDSimplePropertyDlg)
	enum { IDD = IDD_TISCAMERAPROPERTY_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CVCDSimplePropertyDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	DShowLib::Grabber m_Grabber;

	// An instance of the helper class
	CSimplePropertyAccess m_VCDProp;

	// The controls
	CSliderCtrl* m_pBrightnessSlider;
	CStatic* m_pBrightnessStatic;
	CButton* m_pBrightnessCheck;

	CSliderCtrl* m_pWbBlueSlider;
	CStatic* m_pWbBlueStatic;
	CSliderCtrl* m_pWbRedSlider;
	CStatic* m_pWbRedStatic;
	CButton* m_pWbCheck;
	CButton* m_pWbOnePush;

	// Generated message map functions
	//{{AFX_MSG(CVCDSimplePropertyDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBrightnessAuto();
	afx_msg void OnWbAuto();
	afx_msg void OnWbOnepush();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_IDD_TISCAMERAPROPERTYDLG_H__FE179C98_4D80_4DFD_BE48_89A5E3BE32BF__INCLUDED_)
