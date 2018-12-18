// VCD Simple Property.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "tis Camera PropertyDlg.h"
#include "tisCameraDriver.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVCDSimplePropertyApp

BEGIN_MESSAGE_MAP(CVCDSimplePropertyApp, CWinApp)
	//{{AFX_MSG_MAP(CVCDSimplePropertyApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVCDSimplePropertyApp construction

CVCDSimplePropertyApp::CVCDSimplePropertyApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CVCDSimplePropertyApp object

CVCDSimplePropertyApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CVCDSimplePropertyApp initialization

BOOL CVCDSimplePropertyApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.


	DShowLib::InitLibrary();



		CVCDSimplePropertyDlg dlg;

	if( !dlg.selectDevice() )
	{
		AfxMessageBox( TEXT("No device was selected.") );
		return FALSE;
	}

	m_pMainWnd = &dlg;
	dlg.DoModal();
		// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
