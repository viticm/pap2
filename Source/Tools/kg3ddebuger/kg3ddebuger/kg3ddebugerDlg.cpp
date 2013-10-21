// kg3ddebugerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "kg3ddebuger.h"
#include "kg3ddebugerDlg.h"
#include "Psapi.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#pragma comment(lib, "Psapi.lib")

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// Ckg3ddebugerDlg dialog




Ckg3ddebugerDlg::Ckg3ddebugerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(Ckg3ddebugerDlg::IDD, pParent)
    , m_nCpuNum(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Ckg3ddebugerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_SLIDER_ZFAR, m_SliderZFar);
    DDX_Control(pDX, IDC_SPIN_CPU_NUM, m_SpinCpuNum);
    DDX_Text(pDX, IDC_EDIT_CPU_NUM, m_nCpuNum);
    DDV_MinMaxInt(pDX, m_nCpuNum, 1, 8);
    DDX_Control(pDX, IDC_SLIDER_ANGEL, m_SlidAngel);
}

BEGIN_MESSAGE_MAP(Ckg3ddebugerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_COMMAND_RANGE(IDC_CHECK_TERRAIN, IDC_CHECK_F_CLIENTOBJ, &Ckg3ddebugerDlg::OnCheckNotity)
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    ON_WM_TIMER()
	ON_BN_CLICKED(IDC_CHECK_TERRAIN, &Ckg3ddebugerDlg::OnBnClickedCheckTerrain)
END_MESSAGE_MAP()


// Ckg3ddebugerDlg message handlers

BOOL Ckg3ddebugerDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here

    ASSERT(theApp.m_hHookProcess);

    SHFILEINFO siinfo;
    if (SHGetFileInfo(theApp.m_strHookName, 0, &siinfo, sizeof(siinfo), SHGFI_LARGEICON | SHGFI_ICON) > 0)
    {
        CStatic* pIconCtl = (CStatic*)GetDlgItem(IDC_ICON_AP);
        
        if (pIconCtl)
        {
            pIconCtl->SetIcon(siinfo.hIcon);
        }
    }
    
    CWnd* pNameWnd = GetDlgItem(IDC_STATIC_NAME);

    if (pNameWnd)
        pNameWnd->SetWindowText(CString("Current Attach Process :") + theApp.m_strHookName);

    CheckDlgButton(IDC_CHECK_TERRAIN, theApp.GetRenderOption(RENDER_TERRAIN));
    CheckDlgButton(IDC_CHECK_GRASS, theApp.GetRenderOption(RENDER_GRASS));
    CheckDlgButton(IDC_CHECK_ENVIRONMENT, theApp.GetRenderOption(RENDER_WATER));
    CheckDlgButton(IDC_CHECK_WATER, theApp.GetRenderOption(RENDER_TERRAIN));
    CheckDlgButton(IDC_CHECK_TREE, theApp.GetRenderOption(RENDER_TREE));
    CheckDlgButton(IDC_CHECK_SFX, theApp.GetRenderOption(RENDER_SFX));
    CheckDlgButton(IDC_CHECK_CLIENTOBJ, theApp.GetRenderOption(RENDER_CLIENT_OBJ));
    CheckDlgButton(IDC_CHECK_ENTITY, theApp.GetRenderOption(RENDER_ENTITY));
    CheckDlgButton(IDC_CHECK_SKIN, theApp.GetRenderOption(RENDER_SKIN_MESH));
    CheckDlgButton(IDC_CHECK_NORL, theApp.GetRenderOption(RENDER_NORL_MESH));
    CheckDlgButton(IDC_CHECK_WATER_FR, theApp.GetRenderOption(RENDER_WATER_REFLECTION));
    CheckDlgButton(IDC_CHECK_CBBILLBOARD, theApp.GetRenderOption(RENDER_DRAWABLE_BILLBOARD));
    CheckDlgButton(IDC_CHECK_CPU_MODEL_PROCESS, theApp.GetRenderOption(RENDER_CPU_MODEL_PROCESS));

    CheckDlgButton(IDC_CHECK_F_SKIN, theApp.GetFrameMoveOption(FRAMEMOVE_SKIN_MESH));
    CheckDlgButton(IDC_CHECK_F_NORL, theApp.GetFrameMoveOption(FRAMEMOVE_NORL_MESH));
    CheckDlgButton(IDC_CHECK_STREE, theApp.GetFrameMoveOption(FRAMEMOVE_SPEEDTREE));
    CheckDlgButton(IDC_CHECK_F_SFX, theApp.GetFrameMoveOption(FRAMEMOVE_SFX));
    CheckDlgButton(IDC_CHECK_F_PHYSICS, theApp.GetFrameMoveOption(FRAMEMOVE_PHYSICS));
    CheckDlgButton(IDC_CHECK_F_ENVIRONMENT, theApp.GetFrameMoveOption(FRAMEMOVE_ENVIRONMENT));
    CheckDlgButton(IDC_CHECK_F_CLIENTOBJ, theApp.GetFrameMoveOption(FRAMEMOVE_CLIENT_OBJ));

    if (!theApp.GetRenderOption(RENDER_ENTITY))
    {
        GetDlgItem(IDC_CHECK_SKIN)->EnableWindow(FALSE);
        GetDlgItem(IDC_CHECK_NORL)->EnableWindow(FALSE);
    }

    if (!theApp.GetRenderOption(RENDER_WATER))
    {
        GetDlgItem(IDC_CHECK_WATER_FR)->EnableWindow(FALSE);
    }

    if (!theApp.GetRenderOption(RENDER_SFX))
    {
        GetDlgItem(IDC_CHECK_CBBILLBOARD)->EnableWindow(FALSE);
    }

    if (!theApp.GetRenderOption(RENDER_CPU_MODEL_PROCESS))
    {
        GetDlgItem(IDC_EDIT_CPU_NUM)->EnableWindow(FALSE);
        m_SpinCpuNum.EnableWindow(FALSE);
    }

    int zFar = (int)theApp.GetRenderParam(RENDER_PARAM_ZFAR) / 100;
    
    m_SliderZFar.SetRange(1, 5000, TRUE);
    m_SliderZFar.SetPos(zFar);
    
    CString strZfar;
    strZfar.Format(TEXT("视距 : %d 米"), zFar);
    SetDlgItemText(IDC_STATIC_ZFAR, strZfar);

    DWORD angle = (DWORD)theApp.GetRenderParam(RENDER_PARAM_CAMERA_ANGEL);

    m_SlidAngel.SetRange(1, 180, TRUE);
    m_SlidAngel.SetPos(angle);

    CString strAngle;
    strAngle.Format(TEXT("视角 : %u 度"), angle);
    SetDlgItemText(IDC_STATIC_ANGLE, strAngle);

    m_SpinCpuNum.SetRange(1, 8);
    m_SpinCpuNum.SetBuddy(GetDlgItem(IDC_EDIT_CPU_NUM));

    int cpuNum = (int)theApp.GetRenderParam(RENDER_PARAM_MP_CPU_NUM);
    m_SpinCpuNum.SetPos(cpuNum);
    m_nCpuNum = cpuNum;
    UpdateData(FALSE);

    SetTimer(100, 10, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void Ckg3ddebugerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void Ckg3ddebugerDlg::OnPaint()
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
HCURSOR Ckg3ddebugerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void Ckg3ddebugerDlg::OnCheckNotity(UINT id)
{
    BOOL bCheck = IsDlgButtonChecked(id);

    switch (id)
    {   
        // Render
    case IDC_CHECK_TERRAIN :
            theApp.SetRenderOption(RENDER_TERRAIN, bCheck);
        break;
    case IDC_CHECK_GRASS :
        theApp.SetRenderOption(RENDER_GRASS, bCheck);
        break;
    case IDC_CHECK_ENVIRONMENT :
        theApp.SetRenderOption(RENDER_ENVIRONMENT, bCheck);
        break;
    case IDC_CHECK_WATER :
        theApp.SetRenderOption(RENDER_WATER, bCheck);
        GetDlgItem(IDC_CHECK_WATER_FR)->EnableWindow(bCheck);
        break;
    case IDC_CHECK_TREE :
        theApp.SetRenderOption(RENDER_TREE, bCheck);
        break;
    case IDC_CHECK_SFX :
        theApp.SetRenderOption(RENDER_SFX, bCheck);
        GetDlgItem(IDC_CHECK_CBBILLBOARD)->EnableWindow(bCheck);
        break;
    case IDC_CHECK_CLIENTOBJ :
        theApp.SetRenderOption(RENDER_CLIENT_OBJ, bCheck);
        break;
    case IDC_CHECK_ENTITY :
        theApp.SetRenderOption(RENDER_ENTITY, bCheck);
        GetDlgItem(IDC_CHECK_SKIN)->EnableWindow(bCheck);
        GetDlgItem(IDC_CHECK_NORL)->EnableWindow(bCheck);
        break;
    case IDC_CHECK_SKIN :
        theApp.SetRenderOption(RENDER_SKIN_MESH, bCheck);
        break;
    case IDC_CHECK_NORL :
        theApp.SetRenderOption(RENDER_NORL_MESH, bCheck);
        break;
    case IDC_CHECK_WATER_FR :
        theApp.SetRenderOption(RENDER_WATER_REFLECTION, bCheck);
        break;
    case IDC_CHECK_CBBILLBOARD :
        theApp.SetRenderOption(RENDER_DRAWABLE_BILLBOARD, bCheck);
        break;
    case IDC_CHECK_CPU_MODEL_PROCESS :
        theApp.SetRenderOption(RENDER_CPU_MODEL_PROCESS, bCheck);
        GetDlgItem(IDC_EDIT_CPU_NUM)->EnableWindow(bCheck);
        m_SpinCpuNum.EnableWindow(bCheck);
        break;

        // FrameMove
    case IDC_CHECK_F_SKIN :
        theApp.SetFrameMoveOption(FRAMEMOVE_SKIN_MESH, bCheck);
        break;
    case IDC_CHECK_F_NORL :
        theApp.SetFrameMoveOption(FRAMEMOVE_NORL_MESH, bCheck);
        break;
    case IDC_CHECK_STREE :
        theApp.SetFrameMoveOption(FRAMEMOVE_SPEEDTREE, bCheck);
        break;
    case IDC_CHECK_F_SFX :
        theApp.SetFrameMoveOption(FRAMEMOVE_SFX, bCheck);
        break;
    case IDC_CHECK_F_PHYSICS :
        theApp.SetFrameMoveOption(FRAMEMOVE_PHYSICS, bCheck);
        break;
    case IDC_CHECK_F_ENVIRONMENT :
        theApp.SetFrameMoveOption(FRAMEMOVE_ENVIRONMENT, bCheck);
        break;
    case IDC_CHECK_F_CLIENTOBJ :
        theApp.SetFrameMoveOption(FRAMEMOVE_CLIENT_OBJ, bCheck);
        break;

    default :
        break;
    }

}

void Ckg3ddebugerDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if (pScrollBar == (CWnd*)&m_SliderZFar)
    {
        int zFar = m_SliderZFar.GetPos();

        CString strZfar;
        strZfar.Format(TEXT("视距 : %d 米"), zFar);
        SetDlgItemText(IDC_STATIC_ZFAR, strZfar);

        theApp.SetRenderParam(RENDER_PARAM_ZFAR, (DWORD)zFar * 100);
        
    }
    else if (pScrollBar == (CWnd*)&m_SlidAngel)
    {
        int angle = m_SlidAngel.GetPos();

        CString strAngle;
        strAngle.Format(TEXT("视角 : %d 度"), angle);
        SetDlgItemText(IDC_STATIC_ANGLE, strAngle);

        theApp.SetRenderParam(RENDER_PARAM_CAMERA_ANGEL, (DWORD)angle);

    }
    else
        CDialog::OnHScroll(nSBCode, nPos, pScrollBar);
}

void Ckg3ddebugerDlg::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    if (pScrollBar == (CWnd*)&m_SpinCpuNum)
    {
        int Num = m_SpinCpuNum.GetPos();
        theApp.SetRenderParam(RENDER_PARAM_MP_CPU_NUM, (DWORD)Num);

    }
    else
        CDialog::OnVScroll(nSBCode, nPos, pScrollBar);
}

void Ckg3ddebugerDlg::OnTimer(UINT_PTR nIDEvent)
{
    if (nIDEvent == 100)
    {
        FILETIME fileTimeCreate;
        FILETIME fileTimeExit;
        FILETIME fileTimeKenel;
        FILETIME fileTimeUser;
        
        GetProcessTimes(theApp.m_hHookProcess, &fileTimeCreate, &fileTimeExit, &fileTimeKenel, &fileTimeUser);
        
        if (fileTimeExit.dwHighDateTime != 0 || fileTimeExit.dwLowDateTime != 0)
        {
            KillTimer(100);
            MessageBox(TEXT("The Attach Process is Exit :("), TEXT("Alert"), MB_OK);
            PostQuitMessage(0);
        }
    }

    CDialog::OnTimer(nIDEvent);
}

void Ckg3ddebugerDlg::OnBnClickedCheckTerrain()
{
	// TODO: Add your control notification handler code here
}
