// VCD Simple PropertyDlg.cpp : implementation file
//

#include "stdafx.h"
#include "tisCameraDriver.h"
#include "tis Camera PropertyDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CVCDSimplePropertyDlg dialog

CVCDSimplePropertyDlg::CVCDSimplePropertyDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CVCDSimplePropertyDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CVCDSimplePropertyDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CVCDSimplePropertyDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CVCDSimplePropertyDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CVCDSimplePropertyDlg, CDialog)
	//{{AFX_MSG_MAP(CVCDSimplePropertyDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BRIGHTNESS_AUTO, OnBrightnessAuto)
	ON_BN_CLICKED(IDC_WB_AUTO, OnWbAuto)
	ON_BN_CLICKED(IDC_WB_ONEPUSH, OnWbOnepush)
	ON_WM_HSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CVCDSimplePropertyDlg message handlers

using namespace DShowLib;

BOOL CVCDSimplePropertyDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// Get references to the controls
	m_pBrightnessSlider = (CSliderCtrl*)GetDlgItem( IDC_BRIGHTNESS_SLIDER );	
	m_pBrightnessStatic = (CStatic*)GetDlgItem( IDC_BRIGHTNESS_STATIC );
	m_pBrightnessCheck = (CButton*)GetDlgItem( IDC_BRIGHTNESS_AUTO );

	m_pWbBlueSlider = (CSliderCtrl*)GetDlgItem( IDC_WBBLUE_SLIDER );
	m_pWbBlueStatic = (CStatic*)GetDlgItem( IDC_WBBLUE_STATIC );
	m_pWbRedSlider = (CSliderCtrl*)GetDlgItem( IDC_WBRED_SLIDER );
	m_pWbRedStatic = (CStatic*)GetDlgItem( IDC_WBRED_STATIC );
	m_pWbCheck = (CButton*)GetDlgItem( IDC_WB_AUTO );
	m_pWbOnePush = (CButton*)GetDlgItem( IDC_WB_ONEPUSH );

    // Initialize the VCDProp object to access the properties of our ICImagingControl
    // object
	m_VCDProp.init( m_Grabber.getAvailableVCDProperties() );

	// Initialize the auto checkboxes and deactivate automation
	if( !m_VCDProp.isAutoAvailable( VCDID_Brightness ) )
	{
		m_pBrightnessCheck->EnableWindow( FALSE );
	}
	else
	{
		m_VCDProp.setAuto( VCDID_Brightness, false );
	}

	if( !m_VCDProp.isAutoAvailable( VCDID_WhiteBalance) )
	{
		m_pWbCheck->EnableWindow( FALSE );
	}
	else
	{
		m_VCDProp.setAuto( VCDID_WhiteBalance, false );
	}

	// Initialize the one push button
	if( !m_VCDProp.isOnePushAvailable( VCDID_WhiteBalance ) )
	{
		m_pWbOnePush->EnableWindow( FALSE );
	}

	// Initialize the brightness slider
	if( !m_VCDProp.isAvailable( VCDID_Brightness ) )
	{
		m_pBrightnessSlider->EnableWindow(FALSE);
	}
	else
	{
		m_pBrightnessSlider->EnableWindow();
		m_pBrightnessSlider->SetRangeMin( m_VCDProp.getRangeMin( VCDID_Brightness ) );
		m_pBrightnessSlider->SetRangeMax( m_VCDProp.getRangeMax( VCDID_Brightness ) );
		int val = m_VCDProp.getValue( VCDID_Brightness );
		m_pBrightnessSlider->SetPos( val );

		CString s;
		s.Format( TEXT("%d"), val );
		m_pBrightnessStatic->SetWindowText( s );
	}

	// Initialize the slider for whitebalance blue
	if( !m_VCDProp.isAvailable( VCDElement_WhiteBalanceBlue ) )
	{
		m_pWbBlueSlider->EnableWindow(FALSE);
	}
	else
	{
		m_pWbBlueSlider->EnableWindow();
		m_pWbBlueSlider->SetRangeMin( m_VCDProp.getRangeMin( VCDElement_WhiteBalanceBlue ) );
		m_pWbBlueSlider->SetRangeMax( m_VCDProp.getRangeMax( VCDElement_WhiteBalanceBlue ) );
		int val = m_VCDProp.getValue( VCDElement_WhiteBalanceBlue );
		m_pWbBlueSlider->SetPos( val );

		CString s;
		s.Format( TEXT("%d"), val );
		m_pWbBlueStatic->SetWindowText( s );
	}

	// Initialize the slider for whitebalance red
	if( !m_VCDProp.isAvailable( VCDElement_WhiteBalanceRed ) )
	{
		m_pWbRedSlider->EnableWindow(FALSE);
	}
	else
	{
		m_pWbRedSlider->EnableWindow();
		m_pWbRedSlider->SetRangeMin( m_VCDProp.getRangeMin( VCDElement_WhiteBalanceRed ) );
		m_pWbRedSlider->SetRangeMax( m_VCDProp.getRangeMax( VCDElement_WhiteBalanceRed ) );
		int val = m_VCDProp.getValue( VCDElement_WhiteBalanceRed );
		m_pWbRedSlider->SetPos( val );

		CString s;
		s.Format( TEXT("%d"), val );
		m_pWbRedStatic->SetWindowText( s );
	}

	// Set output window for live mode
	m_Grabber.setHWND( GetDlgItem( IDC_DISPLAY )->GetSafeHwnd() );
	// Start live mode
	m_Grabber.startLive();
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CVCDSimplePropertyDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CVCDSimplePropertyDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//////////////////////////////////////////////////////////////////////////
/*! Let the user select a device and make initial settings
*/
bool CVCDSimplePropertyDlg::selectDevice()
{
	m_Grabber.showDevicePage( m_hWnd );

	return m_Grabber.isDevValid();
}

//The checkboxes' event handlers
void CVCDSimplePropertyDlg::OnBrightnessAuto() 
{
	if( m_pBrightnessCheck->GetCheck() )
	{
		m_VCDProp.setAuto( VCDID_Brightness, true );
		m_pBrightnessSlider->EnableWindow( FALSE );
	}
	else
	{
		m_VCDProp.setAuto( VCDID_Brightness, false );
		m_pBrightnessSlider->EnableWindow();
	}	
}

void CVCDSimplePropertyDlg::OnWbAuto() 
{
	if( m_pWbCheck->GetCheck() )
	{
		m_VCDProp.setAuto( VCDID_WhiteBalance, true );
		m_pWbBlueSlider->EnableWindow( FALSE );
		m_pWbRedSlider->EnableWindow( FALSE );
		m_pWbOnePush->EnableWindow( FALSE );
	}
	else
	{
		m_VCDProp.setAuto( VCDID_WhiteBalance, false );
		m_pWbBlueSlider->EnableWindow();
		m_pWbRedSlider->EnableWindow();
		m_pWbOnePush->EnableWindow();
	}
}

// Button event handler
void CVCDSimplePropertyDlg::OnWbOnepush() 
{
	m_VCDProp.push( VCDID_WhiteBalance );
}


// The sliders' event handler
void CVCDSimplePropertyDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	int pos = ((CSliderCtrl*) pScrollBar)->GetPos();
	CString posText;
	posText.Format( TEXT("%d"), pos );

	// Check which slider was scrolled
	switch( pScrollBar->GetDlgCtrlID() )
	{
	case IDC_WBBLUE_SLIDER:
		{
			m_VCDProp.setValue( VCDElement_WhiteBalanceBlue, pos );
			m_pWbBlueStatic->SetWindowText( posText );
		}
		break;
	case IDC_WBRED_SLIDER:
		{
			m_VCDProp.setValue( VCDElement_WhiteBalanceRed, pos );
			m_pWbRedStatic->SetWindowText( posText );			
		}
		break;
	case IDC_BRIGHTNESS_SLIDER:
		{
			m_VCDProp.setValue( VCDID_Brightness, pos );
			m_pBrightnessStatic->SetWindowText( posText );
		}
		break;
	}
	
	CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}
