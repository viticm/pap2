// TestTGAPreviewDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TestTGAPreview.h"
#include "TestTGAPreviewDlg.h"
#include "e:\\work\\Sword3\\Sword3\\Source\\Tools\\GameDesignerEditor\\Controls\\TGAPreview\\TGAPreview\\TGAPreview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CTestTGAPreviewDlg dialog

CTestTGAPreviewDlg::CTestTGAPreviewDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTestTGAPreviewDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

CTestTGAPreviewDlg::~CTestTGAPreviewDlg()
{
	//KillTimer(m_nTimer);
}

void CTestTGAPreviewDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTestTGAPreviewDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDCANCEL2, OnChange)
	ON_BN_CLICKED(IDCANCEL3, OnRemove)	
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CTestTGAPreviewDlg::OnRemove()
{
	HWND h1 = (GetDlgItem(IDC_PREVIEW)->GetSafeHwnd());
	HWND h2 = (GetDlgItem(IDC_PREVIEW2)->GetSafeHwnd());

	UnInit(h2);
}

void CTestTGAPreviewDlg::OnChange()
{
	CString strFileNames;
	strFileNames += CString("D:\\b001.TGA") + "\r\n";
	strFileNames += CString("D:\\b002.TGA") + "\r\n";
	strFileNames += CString("D:\\b003.TGA") + "\r\n";
	strFileNames += CString("D:\\b004.TGA") + "\r\n";
	strFileNames += CString("D:\\b005.TGA") + "\r\n";
	strFileNames += CString("D:\\b006.TGA") + "\r\n";
	strFileNames += CString("D:\\b007.TGA") + "\r\n";
	strFileNames += CString("D:\\b008.TGA") + "\r\n";
	strFileNames += CString("D:\\b009.TGA") + "\r\n";
	strFileNames += CString("D:\\b010.TGA") + "\r\n";

	if (Init((GetDlgItem(IDC_PREVIEW2)->GetSafeHwnd())))
		SetFileNames((GetDlgItem(IDC_PREVIEW2)->GetSafeHwnd()), (char*)LPCTSTR(strFileNames));
}

// CTestTGAPreviewDlg message handlers

BOOL CTestTGAPreviewDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	m_nTimer = SetTimer(1, 1000 / 30, 0);
	m_nTimer1 = SetTimer(2, 1000 / 40, 0);

	Init((GetDlgItem(IDC_PREVIEW)->GetSafeHwnd()));
	CString strFileNames;
	strFileNames += CString("D:\\b001.TGA") + "\r\n";
	strFileNames += CString("D:\\b002.TGA") + "\r\n";
	strFileNames += CString("D:\\b003.TGA") + "\r\n";
	strFileNames += CString("D:\\b004.TGA") + "\r\n";
	strFileNames += CString("D:\\b005.TGA") + "\r\n";
	strFileNames += CString("D:\\b006.TGA") + "\r\n";
	strFileNames += CString("D:\\b007.TGA") + "\r\n";
	strFileNames += CString("D:\\b008.TGA") + "\r\n";
	strFileNames += CString("D:\\b009.TGA") + "\r\n";
	strFileNames += CString("D:\\b010.TGA") + "\r\n";

	SetFileNames((GetDlgItem(IDC_PREVIEW)->GetSafeHwnd()), (char*)strFileNames.GetBuffer());
	strFileNames.ReleaseBuffer();


	CString strFileNames1;
	strFileNames1 += CString("D:\\b001.JPG") + "\r\n";
	strFileNames1 += CString("D:\\b002.JPG") + "\r\n";
	strFileNames1 += CString("D:\\b003.JPG") + "\r\n";
	strFileNames1 += CString("D:\\b004.JPG") + "\r\n";
	strFileNames1 += CString("D:\\b005.JPG") + "\r\n";
	strFileNames1 += CString("D:\\b006.JPG") + "\r\n";
	strFileNames1 += CString("D:\\b007.JPG") + "\r\n";
	strFileNames1 += CString("D:\\b008.JPG") + "\r\n";
	strFileNames1 += CString("D:\\b009.JPG") + "\r\n";
	strFileNames1 += CString("D:\\b010.JPG") + "\r\n";

	ExportToFile((GetDlgItem(IDC_PREVIEW)->GetSafeHwnd()), 128, 128, (char*)strFileNames1.GetBuffer());
	strFileNames1.ReleaseBuffer();
	SetFileNames((GetDlgItem(IDC_PREVIEW)->GetSafeHwnd()), (char*)strFileNames1.GetBuffer());

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CTestTGAPreviewDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

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

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CTestTGAPreviewDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTestTGAPreviewDlg::OnOK()
{
	return;
}

void CTestTGAPreviewDlg::OnCancel()
{
	UnInit(NULL);//(GetDlgItem(IDC_PREVIEW)->GetSafeHwnd()));

	CDialog::OnCancel();
}

void CTestTGAPreviewDlg::OnTimer(UINT nIDEvent) 
{
	//MessageBeep(0xFFFFFFFF);   // Beep

	// Call base class handler.
	if (m_nTimer == nIDEvent)
	{
		HWND h1 = (GetDlgItem(IDC_PREVIEW)->GetSafeHwnd());
		//Render(h1);
		FrameMove(h1);
	}
	else
	{
		//HWND h2 = (GetDlgItem(IDC_PREVIEW2)->GetSafeHwnd());
		//FrameMove(h2);
		//Render(h2);
		HWND h1 = (GetDlgItem(IDC_PREVIEW)->GetSafeHwnd());
		Render(h1);
	}
	CDialog::OnTimer(nIDEvent);
}