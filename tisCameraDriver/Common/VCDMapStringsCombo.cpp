// VCDMapStringsCombo.cpp : implementation file
//

#include "stdafx.h"
#include "VCDMapStringsCombo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVCDMapStringsCombo

CVCDMapStringsCombo::CVCDMapStringsCombo()
{
}

CVCDMapStringsCombo::~CVCDMapStringsCombo()
{
}


BEGIN_MESSAGE_MAP(CVCDMapStringsCombo, CComboBox)
	//{{AFX_MSG_MAP(CVCDMapStringsCombo)
	ON_CONTROL_REFLECT(CBN_SELCHANGE, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVCDMapStringsCombo message handlers

using namespace DShowLib;

//////////////////////////////////////////////////////////////////////////
/*! Initialize the control
*/
void	CVCDMapStringsCombo::setupCtrl( tIVCDMapStringsPropertyPtr pItf )
{
	m_pMapStringsItf = pItf;
	initialUpdate();
}

//////////////////////////////////////////////////////////////////////////
/*! Fill the available strings into the combo box
*/
void	CVCDMapStringsCombo::initialUpdate()
{
	IVCDMapStringsProperty::tStringVec strings = m_pMapStringsItf->getStrings();

	SetRedraw( FALSE );
	for( unsigned int i = 0; i < strings.size(); ++i )
	{
		AddString( CString( strings[i].c_str() ) );
	}
	SetRedraw( TRUE );
	
	update();
}

//////////////////////////////////////////////////////////////////////////
/*! Set the combo's availability to the property's availability
*/
void	CVCDMapStringsCombo::update()
{
	EnableWindow( m_pMapStringsItf->getAvailable() );

	scrollUpdate();
}

//////////////////////////////////////////////////////////////////////////
/*! Select the current string in the combo box
*/
void	CVCDMapStringsCombo::scrollUpdate()
{
	SelectString( -1, CString( m_pMapStringsItf->getString().c_str() ) );
}

//////////////////////////////////////////////////////////////////////////
/*! Set the currently selected string at the interface
*/
void CVCDMapStringsCombo::OnSelchange() 
{
	try
	{
		CString txt;
		GetWindowText( txt );
		m_pMapStringsItf->setString( (LPCTSTR)txt );

	}
	catch( HRESULT )
	{
		AfxMessageBox( TEXT("Error Setting Property") );
	}
}
