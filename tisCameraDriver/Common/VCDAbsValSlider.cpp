// VCDAbsValSlider.cpp : implementation file
//

#include "stdafx.h"
#include "VCDAbsValSlider.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include <cmath>

/////////////////////////////////////////////////////////////////////////////
// CVCDAbsValSlider

CVCDAbsValSlider::CVCDAbsValSlider()
{
}

CVCDAbsValSlider::~CVCDAbsValSlider()
{
}


BEGIN_MESSAGE_MAP(CVCDAbsValSlider, CSliderCtrl)
	//{{AFX_MSG_MAP(CVCDAbsValSlider)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVCDAbsValSlider message handlers

using namespace DShowLib;

//////////////////////////////////////////////////////////////////////////
/*! Initialize the control
*/
void	CVCDAbsValSlider::setupCtrl( tIVCDAbsoluteValuePropertyPtr pItf )
{
	m_pAbsValItf = pItf;
	initialUpdate();
}

//////////////////////////////////////////////////////////////////////////
/*! Set the slider's range to fixed 0..100
*/
void	CVCDAbsValSlider::initialUpdate()
{
	SetRange( 0, 100, FALSE );
	update();
}

//////////////////////////////////////////////////////////////////////////
/*! Set the slider's availability
*/
void	CVCDAbsValSlider::update()
{
	EnableWindow( m_pAbsValItf->getAvailable() );
	scrollUpdate();
}

//////////////////////////////////////////////////////////////////////////
/*! Set the slider's position and the buddy's text to the value provided
	by the controlled interface
*/
void	CVCDAbsValSlider::scrollUpdate()
{
	SetPos( GetSliderPos() );

	CWnd* pBuddy = GetBuddy( FALSE );
	if( pBuddy )
	{
		CString txt;
		txt.Format( TEXT("%.4f"), m_pAbsValItf->getValue() );
		pBuddy->SetWindowText( txt );
	}
}

//////////////////////////////////////////////////////////////////////////
/*! Convert the slider's position to a value for the controlled interface
*/
void CVCDAbsValSlider::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	// Get the new absolute value depending on the slider position
	// and set the new value to the interface
	if( !m_pAbsValItf->getReadOnly() )
	{
		try
		{
			m_pAbsValItf->setValue( GetAbsVal() );
		}
		catch( HRESULT )
		{
			AfxMessageBox( TEXT("Error Setting Property") );
		}
	}
	
	// Read the new value back from the absolute value interface
	// This has to be done because we do not know the granularity of the absolute values
	// and the value that has really been set normally differs from the value we
	// assigned to the property
	scrollUpdate();
	
	CSliderCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}



//////////////////////////////////////////////////////////////////////////
/*! Calculate the optimal slider position for the current absolute value
	of the controlled interface
*/
int CVCDAbsValSlider::GetSliderPos()
{
	double rmin = 0;
	double rmax = 0;
	double absval = 0;
	double rangelen = 0;
	double p = 0;

	// Get the property data from the interface
	rmin = m_pAbsValItf->getRangeMin();
	rmax = m_pAbsValItf->getRangeMax();
	absval = m_pAbsValItf->getValue();

	// Do calculation depending of the dimension function of the property
	if( m_pAbsValItf->getDimFunction() == IVCDAbsoluteValueProperty::eLog )
	{
		rangelen = log(rmax) - log(rmin);
		p = 100 / rangelen * (log(absval) - log(rmin));
	}
	else // m_pAbsValItf->getDimFunction() == IVCDAbsoluteValueProperty::eLinear
	{
		rangelen = rmax - rmin;
		p = 100 / rangelen * (absval - rmin);
	}

	// Round to integer
	return (int)(p+0.5);
}

//////////////////////////////////////////////////////////////////////////
/*! Calculate the new absolute value based on the slider's position
*/
double CVCDAbsValSlider::GetAbsVal()
{
	double rmin = 0;
	double rmax = 0;
	double rangelen = 0;
	double value = 0;

	// Get the property data from the interface
	rmin = m_pAbsValItf->getRangeMin();
	rmax = m_pAbsValItf->getRangeMax();

	// Do calculation depending of the dimension function of the property
	if( m_pAbsValItf->getDimFunction() == IVCDAbsoluteValueProperty::eLog )
	{

		rangelen = log(rmax) - log(rmin);
		value = exp( log(rmin) + rangelen / 100 * GetPos() );

	}
	else // m_pAbsValItf->getDimFunction() == IVCDAbsoluteValueProperty::eLinear
	{

		rangelen = rmax - rmin;
		value = rmin + rangelen / 100 * GetPos();
	}

	// Correct the value if it is out of bounds
	if (value > rmax)
	{
		value = rmax;
	}
	if (value < rmin)
	{
		value = rmin;
	}

	return value;
}

