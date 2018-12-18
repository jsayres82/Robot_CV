// VCDSwitchCheck.cpp : implementation file
//

#include "stdafx.h"
#include "VCDSwitchCheck.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVCDSwitchCheck

CVCDSwitchCheck::CVCDSwitchCheck()
{
}

CVCDSwitchCheck::~CVCDSwitchCheck()
{
}


BEGIN_MESSAGE_MAP(CVCDSwitchCheck, CButton)
	//{{AFX_MSG_MAP(CVCDSwitchCheck)
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVCDSwitchCheck message handlers

//////////////////////////////////////////////////////////////////////////
/*! Initialize the control
	Save a pointer to the controlled interface
	Save a pointer to the control group
*/
void	CVCDSwitchCheck::setupCtrl( DShowLib::tIVCDSwitchPropertyPtr pItf, tControlVectorPtr pGroup )
{
	m_pSwitchItf = pItf;
	m_pGroup = pGroup;
	initialUpdate();
}

//////////////////////////////////////////////////////////////////////////
/*! Simply do a complete update
*/
void	CVCDSwitchCheck::initialUpdate()
{
	update();
}

//////////////////////////////////////////////////////////////////////////
/*! Adjust enabled state to availability
	Set check state to the controlled property's switch state
*/
void	CVCDSwitchCheck::update()
{
	EnableWindow( m_pSwitchItf->getAvailable() );

	SetCheck( m_pSwitchItf->getSwitch() ? BST_CHECKED : BST_UNCHECKED );
}

//////////////////////////////////////////////////////////////////////////
/*! Try to set the new check state at the controlled property
*/
void CVCDSwitchCheck::OnClicked() 
{
	if( !m_pSwitchItf->getReadOnly() )
	{
		bool old = GetCheck() == BST_CHECKED;		

		try
		{
			m_pSwitchItf->setSwitch( !old );
		}
		catch( HRESULT )
		{
			AfxMessageBox( TEXT("Error Setting Property") );
		}

		SetCheck( !old ? BST_CHECKED : BST_UNCHECKED );

		if( m_pGroup != 0 )
		{
			for( tControlVector::iterator i = m_pGroup->begin(); i != m_pGroup->end(); ++i )
			{
				(*i)->update();
			}
		}
	}
}
