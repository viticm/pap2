// GameConfigDlg.cpp : 实现文件
//

#include "stdafx.h"
#include <io.h>

#include "GameConfig.h"
#include "GameConfigDlg.h"
#include "KGPublic.h"
#include "kglog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

KGameConfigDlg::KGameConfigDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KGameConfigDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void KGameConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TAB, m_MainTab);
}

BEGIN_MESSAGE_MAP(KGameConfigDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_OK, &KGameConfigDlg::OnBnClickedBtnOk)
	ON_BN_CLICKED(IDC_BTN_QUIT, &KGameConfigDlg::OnBnClickedBtnQuit)
	ON_BN_CLICKED(IDC_BTN_SYSINFO, &KGameConfigDlg::OnBnClickedBtnSysInfo)
	ON_BN_CLICKED(IDC_BTN_APPLY, &KGameConfigDlg::OnBnClickedBtnApply)
END_MESSAGE_MAP()


// KGameConfigDlg 消息处理程序

BOOL KGameConfigDlg::OnInitDialog()
{
    int nResult  = false;
    int nRetCode = false;
    TCHAR szApplicationPath[MAX_PATH];
    TCHAR szConfigFileName[MAX_PATH];
    TCHAR *pchLashSlash = NULL;

    nRetCode = GetModuleFileName(NULL, szApplicationPath, _countof(szApplicationPath));
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szApplicationPath));

    pchLashSlash = _tcsrchr(szApplicationPath, _T('\\'));
    KGLOG_PROCESS_ERROR(pchLashSlash);
    *(pchLashSlash + 1) = _T('\0');

    nRetCode = _sntprintf(
        szConfigFileName, _countof(szConfigFileName), _T("%s%s"), szApplicationPath, _T("config.ini")
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szConfigFileName));

    nRetCode = _taccess(szConfigFileName, 0);
    KGLOG_PROCESS_ERROR(nRetCode == 0 && "config.ini not found.");

    CDialog::OnInitDialog();
    SetIcon(m_hIcon, TRUE);
    SetIcon(m_hIcon, FALSE);

    m_MainTab.Init();

    nResult = true;
Exit0:
    if (!nResult)
    {
        MessageBox(_T("程序初始化失败!"));
        KGLogPrintf(KGLOG_ERR,	"程序初始化失败，退出程序！"); 
        EndDialog(FALSE);
    }	
    return TRUE;
}

void KGameConfigDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标显示。
//
HCURSOR KGameConfigDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

BOOL KGameConfigDlg::PreTranslateMessage(MSG* pMsg)     
{   
	if (pMsg->wParam == VK_RETURN)      
	{     
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg); 

}   

void KGameConfigDlg::OnBnClickedBtnOk()
{
	// TODO: Add your control notification handler code here
	m_MainTab.ConfigSave();
	OnBnClickedBtnQuit();
}

void KGameConfigDlg::OnBnClickedBtnQuit()
{
	// TODO: Add your control notification handler code here
	EndDialog(TRUE);
}

void KGameConfigDlg::OnBnClickedBtnSysInfo()
{
	//::WinExec("msinfo32.exe", NULL);
	//m_InfoPage.Init();
}
void KGameConfigDlg::OnBnClickedBtnApply()
{
	// TODO: Add your control notification handler code here
	m_MainTab.ConfigSave();
}
