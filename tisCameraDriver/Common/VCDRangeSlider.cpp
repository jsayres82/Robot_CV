// VCDRangeSlider.cpp : implementation file
//

#include "stdafx.h"
#include "VCDRangeSlider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVCDRangeSlider

CVCDRangeSlider::CVCDRangeSlider()
{
}

CVCDRangeSlider::~CVCDRangeSlider()
{
}


BEGIN_MESSAGE_MAP(CVCDRangeSlider, CSliderCtrl)
	//{{AFX_MSG_MAP(CVCDRangeSlider)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVCDRangeSlider message handlers

//////////////////////////////////////////////////////////////////////////
/*! Initialize the control
*/
void	CVCDRangeSlider::setupCtrl( DShowLib::tIVCDRangePropertyPtr pItf )
{
	m_pRangeItf = pItf;
	initialUpdate();
}

//////////////////////////////////////////////////////////////////////////
/*! Set the slider's range to the property's minimum and maximum
*/
void	CVCDRangeSlider::initialUpdate()
{
	int min = m_pRangeItf->getRangeMin();
	int max = m_pRangeItf->getRangeMax();
	int delta = m_pRangeItf->getDelta();

	SetRange( min/delta, max/delta, FALSE );
	update();
}

//////////////////////////////////////////////////////////////////////////
/*! Adjust enabled state to availability
*/
void	CVCDRangeSlider::update()
{
	EnableWindow( m_pRangeItf->getAvailable() );

	scrollUpdate();
}

//////////////////////////////////////////////////////////////////////////
/*! Set the slider's position the the property's state
*/
void	CVCDRangeSlider::scrollUpdate()
{
	int pos = m_pRangeItf->getValue();
	int delta = m_pRangeItf->getDelta();

	SetPos( pos/delta );

	CWnd* pBuddy = GetBuddy( FALSE );
	if( pBuddy )
	{
		CString txt;
		txt.Format( TEXT("%d"), pos/delta );	
		pBuddy->SetWindowText( txt );
	}
}

//////////////////////////////////////////////////////////////////////////
/*! Try to set the slider's value to the controlled property
*/
void CVCDRangeSlider::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int p = GetPos();

	try
	{
		m_pRangeItf->setValue( p * m_pRangeItf->getDelta() );
	}
	catch( HRESULT )
	{
		AfxMessageBox( TEXT("Error Setting Property") );
	}

	CWnd* pBuddy = GetBuddy( FALSE );
	if( pBuddy )
	{
		CString txt;
		txt.Format( TEXT("%d"), p );
		pBuddy->SetWindowText( txt );
	}
}
