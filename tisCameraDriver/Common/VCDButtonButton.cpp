// VCDButtonButton.cpp : implementation file
//

#include "stdafx.h"
#include "VCDButtonButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVCDButtonButton

CVCDButtonButton::CVCDButtonButton()
{
}

CVCDButtonButton::~CVCDButtonButton()
{
}


BEGIN_MESSAGE_MAP(CVCDButtonButton, CButton)
	//{{AFX_MSG_MAP(CVCDButtonButton)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVCDButtonButton message handlers

//////////////////////////////////////////////////////////////////////////
/*! Initialize the control
*/
void	CVCDButtonButton::setupCtrl( DShowLib::tIVCDButtonPropertyPtr pItf, tControlVectorPtr pGroup )
{
	m_pButtonItf = pItf;	
	update();
}

//////////////////////////////////////////////////////////////////////////
/*! Set the button's availability
*/
void	CVCDButtonButton::update()
{
	EnableWindow( m_pButtonItf->getAvailable() );
}

//////////////////////////////////////////////////////////////////////////
/*! Invoke the push method on the controlled interface
*/
void CVCDButtonButton::OnClicked() 
{
	try
	{
		m_pButtonItf->push();
	}
	catch( HRESULT )
	{
		AfxMessageBox( TEXT("Error Setting Property") );
	}
}
