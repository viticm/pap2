// KG_D3DMemoryLeakAnalyzerDlg.cpp : implementation file
//

#include "stdafx.h"
#include "KG_D3DMemoryLeakAnalyzer.h"
#include "KG_D3DMemoryLeakAnalyzerDlg.h"
#include "KG_DebugProcess.h"
#include <shlwapi.h>
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


class _StringCache
{
public:
    void Append(LPCTSTR psNewString)
    {
        ::EnterCriticalSection(&m_CriticalSection);

        m_String += psNewString;

        ::LeaveCriticalSection(&m_CriticalSection);;
    };
    void Get(CString *psRetString)
    {
        ASSERT(psRetString);
        ::EnterCriticalSection(&m_CriticalSection);

        *psRetString = m_String;
        m_String.Empty();
        
        ::LeaveCriticalSection(&m_CriticalSection);;
    }
public:
    _StringCache()  {::InitializeCriticalSection(&m_CriticalSection);};
    ~_StringCache() {::DeleteCriticalSection(&m_CriticalSection);    };
private:
    CRITICAL_SECTION m_CriticalSection;
    CString m_String;
};

_StringCache g_StringCache;
unsigned     g_uBreakCount = 0;
unsigned     g_uLastBreakCount = g_uBreakCount;
ALLOC_STAT   g_AllocStat = { 0 };
ALLOC_STAT   g_LastAllocStat = g_AllocStat;

////////////////////////////////////////////////////////////////////////////////

void CALLBACK DebugProcessCallBack(DEBUG_PROCESS_EVENT eEvent, void *pvData, void *pvUserData)
{
    CKG_D3DMemoryLeakAnalyzerDlg *pDlg = (CKG_D3DMemoryLeakAnalyzerDlg *)pvUserData;
    ASSERT(pDlg);

    switch (eEvent)
    {
    case DBG_PROCESS_CREATE:
        g_StringCache.Append(TEXT("process created\n"));
        break;

    case DBG_PROCESS_EXIT:
        g_StringCache.Append(TEXT("\nprocess exited\n"));
        break;

    case DBG_MEM_USE_STATE_CHANGE:
        g_uBreakCount     = 0;
        g_uLastBreakCount = g_uBreakCount;
        ZeroMemory(&g_AllocStat, sizeof(g_AllocStat));
        g_LastAllocStat = g_AllocStat;
        break;

    case DBG_BREAK:
        InterlockedIncrement((LONG *)&g_uBreakCount);
        g_AllocStat = *((ALLOC_STAT *)pvData); // maybe multi thread error, unimportance
        break;

    case DBG_DEBUG_STRING:
        g_StringCache.Append((const char *)pvData);
     //   pDlg->AppendDebugString((const char *)pvData);
        break;
    }
}
////////////////////////////////////////////////////////////////////////////////


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


// CKG_D3DMemoryLeakAnalyzerDlg dialog




CKG_D3DMemoryLeakAnalyzerDlg::CKG_D3DMemoryLeakAnalyzerDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKG_D3DMemoryLeakAnalyzerDlg::IDD, pParent)
    , m_sDebugAppPathName(_T(""))
    , m_nWorkThreadRunningFlag(false)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CKG_D3DMemoryLeakAnalyzerDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_EDIT_APP_PATH_NAME, m_sDebugAppPathName);
    DDX_Control(pDX, IDC_EDIT_DEBUG_OUTPUT, m_ctlDebugOutput);
    DDX_Control(pDX, IDC_REPORT_WND, m_ctlReportWnd);
    DDX_Control(pDX, IDC_ENABLE_MEM_USE_ANALYZE, m_ctlEnableMemUseAnalyze);
}

BEGIN_MESSAGE_MAP(CKG_D3DMemoryLeakAnalyzerDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_BROWSE, &CKG_D3DMemoryLeakAnalyzerDlg::OnBnClickedButtonBrowse)
    ON_BN_CLICKED(IDOK, &CKG_D3DMemoryLeakAnalyzerDlg::OnBnClickedOk)
    ON_BN_CLICKED(IDCANCEL, &CKG_D3DMemoryLeakAnalyzerDlg::OnBnClickedCancel)
    ON_EN_CHANGE(IDC_EDIT_APP_PATH_NAME, &CKG_D3DMemoryLeakAnalyzerDlg::OnEnChangeEditAppPathName)
    ON_BN_CLICKED(IDC_MEM_STATE, &CKG_D3DMemoryLeakAnalyzerDlg::OnBnClickedGetMemRegion)
    ON_WM_TIMER()
    ON_BN_CLICKED(IDC_GET_ALLOC_FUNCTION, &CKG_D3DMemoryLeakAnalyzerDlg::OnBnClickedGetAllocFunction)
    ON_BN_CLICKED(IDC_ENABLE_MEM_USE_ANALYZE, &CKG_D3DMemoryLeakAnalyzerDlg::OnBnClickedEnableMemUseAnalyze)
END_MESSAGE_MAP()


// CKG_D3DMemoryLeakAnalyzerDlg message handlers

BOOL CKG_D3DMemoryLeakAnalyzerDlg::OnInitDialog()
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

    TCHAR szModulePath[MAX_PATH];
    ::GetModuleFileName(NULL, szModulePath, MAX_PATH);
    TCHAR *pchFilePos = pchFilePos = _tcsrchr(szModulePath, TCHAR('\\'));
    if (pchFilePos)
    {
        *pchFilePos = NULL;
    }
    m_sCurProcPath = szModulePath;

    m_sDebugAppPathName = "d:\\Kingsoft\\Game\\Sword3-products\\trunk\\client\\JX3ClientD.exe";
    //m_sDebugAppPathName = "d:\\UserData\\Documents\\Visual Studio 2005\\Projects\\test222\\Debug\\test222.exe";
    UpdateData(FALSE);

    CWnd *pWin = GetDlgItem(IDOK);
    ASSERT(pWin);
    pWin->EnableWindow(!(m_sDebugAppPathName.IsEmpty()));


    m_ctlDebugOutput.SetLimitText(0);
    //m_ctlEnableMemUseAnalyze.SetCheck(TRUE);
    OnBnClickedEnableMemUseAnalyze();

    SetTimer(0, 1000, NULL);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CKG_D3DMemoryLeakAnalyzerDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CKG_D3DMemoryLeakAnalyzerDlg::OnPaint()
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
HCURSOR CKG_D3DMemoryLeakAnalyzerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}
////////////////////////////////////////////////////////////////////////////////
CString CKG_D3DMemoryLeakAnalyzerDlg::_GetDebugAppWorkPath()
{
    int nRetCode = false;

    nRetCode = m_sDebugAppPathName.ReverseFind('\\');
    if (nRetCode > 0)
        return m_sDebugAppPathName.Left(nRetCode);
    else
        return ".";

}
////////////////////////////////////////////////////////////////////////////////
CString CKG_D3DMemoryLeakAnalyzerDlg::_GetSymbolSearchPath()
{
    TCHAR szSymbolPath[4096];
    CString sDebugProcWorkPath;
    
    szSymbolPath[0] = '\0';

    if (0 == ::GetEnvironmentVariable("_NT_SYMBOL_PATH", szSymbolPath, countof(szSymbolPath)))
    {
        _snprintf(
            szSymbolPath, MAX_PATH,
            "srv*%s\\Symbols*http://msdl.microsoft.com/download/symbols",
            m_sCurProcPath
        );
        szSymbolPath[MAX_PATH - 1] = '\0';

        if (!::PathFileExists(m_sCurProcPath + TEXT("\\Symbols")))
        {
            MessageBox(
                TEXT("第一次运行由于需要从微软的服务器上下载系统dll的pdb文件，所以启动过程会有些慢"),
                TEXT("提示")
            );
        }
    }
    sDebugProcWorkPath = _GetDebugAppWorkPath();

    return sDebugProcWorkPath + ";" + szSymbolPath;
}
////////////////////////////////////////////////////////////////////////////////
void CKG_D3DMemoryLeakAnalyzerDlg::OnBnClickedButtonBrowse()
{
    CFileDialog fileDlg (
        TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY, TEXT("*.exe|*.exe|*.*|*.*||"), this
    );

    if(fileDlg.DoModal () == IDOK)
    {
        m_sDebugAppPathName = fileDlg.GetPathName();
        UpdateData(FALSE);

        OnEnChangeEditAppPathName();
        OnBnClickedOk();
    }
}

void CKG_D3DMemoryLeakAnalyzerDlg::OnBnClickedOk()
{
    int nRetCode = false;
    CWinThread *pDebugThread = NULL;
    DWORD dwLoadDebugRuntimeFlag = 0;
    DWORD dwDataType = 0;
    DWORD dwDataSize = 0;

    if (!(GetDlgItem(IDOK)->IsWindowEnabled()))
    {
        return;
    }

    g_uBreakCount     = 0;
    g_uLastBreakCount = g_uBreakCount;
    ZeroMemory(&g_AllocStat, sizeof(g_AllocStat));
    g_LastAllocStat = g_AllocStat;

    // -------------------------- Get AppPathName ------------------------------
    UpdateData(TRUE);
    if (m_sDebugAppPathName.IsEmpty())
    {
        MessageBox(TEXT("请指定可执行程序!"));
        return;
    }
    // -------------------- check runtime environment  -------------------------
    if (
        !::PathFileExists(m_sCurProcPath + "\\symsrv.dll") || 
        !::PathFileExists(m_sCurProcPath + "\\dbghelp.dll")
    )
    {
        nRetCode = _CopyDLlFromVCPath();
        if (!nRetCode)
        {
            MessageBox(
                TEXT(
                    "请从VC的安装目录(如：c:\\Program Files\\Microsoft Visual Studio 8\\Common7\\IDE)"
                    "复制symsrv.dll和dbghelp.dll此文件到本程序目录\r\n"
                    "注意，system32目录下的dbghelp.dll的版本过低，得用VC带的"
                ),
                TEXT("缺少需要的文件")
            );
            return; 
        }
    }

    dwDataType = REG_DWORD;
    dwDataSize = sizeof(dwLoadDebugRuntimeFlag);
    nRetCode = ::SHGetValue(
        HKEY_LOCAL_MACHINE,
        TEXT("SOFTWARE\\Microsoft\\Direct3D"),
        TEXT("LoadDebugRuntime"),
        &dwDataType,
        &dwLoadDebugRuntimeFlag,
        &dwDataSize
    );
    if ((nRetCode != ERROR_SUCCESS) || (dwLoadDebugRuntimeFlag != 1))
    {
        nRetCode = MessageBox(
            TEXT("Direct3D没有使用Debug版本(通过DirectX控制面板设置)，无法检查到Direct3D内存泄露\r\n是否继续？"),
            TEXT("警告"),
            MB_YESNO | MB_ICONWARNING
        );
        KG_PROCESS_ERROR(nRetCode == IDYES);
    }

    // ------------------------------  ------------------------------
    pDebugThread = AfxBeginThread(_WorkThread, this);
    KG_ASSERT_EXIT(pDebugThread);

    pDebugThread->m_bAutoDelete = true;

Exit0:
    return;
}

void CKG_D3DMemoryLeakAnalyzerDlg::OnBnClickedCancel()
{
    // TODO: Add your control notification handler code here
    m_Debugger.Stop();
    while(m_Debugger.IsRunning() && !m_nWorkThreadRunningFlag)
    {
        ::Sleep(10);
    }
    OnCancel();
}

void CKG_D3DMemoryLeakAnalyzerDlg::OnBnClickedEnableMemUseAnalyze()
{
    int nRetCode = false;
    CWnd *pWnd = NULL;
    int nEnableMemUseAnalyze = false;

    nEnableMemUseAnalyze = m_ctlEnableMemUseAnalyze.GetCheck();
    pWnd = GetDlgItem(IDC_GET_ALLOC_FUNCTION);
    ASSERT(pWnd);
    nRetCode = pWnd->EnableWindow(nEnableMemUseAnalyze);
    //KG_ASSERT_EXIT(nRetCode); // return the state before the EnableWindow member function was called

    pWnd = GetDlgItem(IDC_GET_MEM_REGION);
    ASSERT(pWnd);
    nRetCode = pWnd->EnableWindow(nEnableMemUseAnalyze);
    //KG_ASSERT_EXIT(nRetCode);
    
    nRetCode = m_Debugger.EnableMemUseAnalyze(nEnableMemUseAnalyze);
    KG_ASSERT_EXIT(nRetCode);
Exit0:
    return;
}

void CKG_D3DMemoryLeakAnalyzerDlg::OnBnClickedGetMemRegion()
{
    std::string sMemRegion;
    m_Debugger.GetMemRegion(&sMemRegion);
    m_ctlReportWnd.SetWindowText(sMemRegion.c_str());

    m_ctlReportWnd.SetSel(0, -1, TRUE);
    m_ctlReportWnd.Copy();
    m_ctlReportWnd.SetSel(-1, -1, TRUE); // If nStartChar is –1, any current selection is removed
}

void CKG_D3DMemoryLeakAnalyzerDlg::OnBnClickedGetAllocFunction()
{
    std::string sMemAllocFunction;
    m_Debugger.GetMemAllocFunction(&sMemAllocFunction);
    m_ctlReportWnd.SetWindowText(sMemAllocFunction.c_str());

    m_ctlReportWnd.SetSel(0, -1, TRUE);
    m_ctlReportWnd.Copy();
    m_ctlReportWnd.SetSel(-1, -1, TRUE); // If nStartChar is –1, any current selection is removed
}

void CKG_D3DMemoryLeakAnalyzerDlg::OnEnChangeEditAppPathName()
{
    UpdateData(TRUE);

    CWnd *pWin = GetDlgItem(IDOK);
    ASSERT(pWin);
    pWin->EnableWindow(!(m_sDebugAppPathName.IsEmpty()));
}

void CKG_D3DMemoryLeakAnalyzerDlg::OnTimer(UINT_PTR nIDEvent)
{
    CString sMsg;
    int nWindowTextLength;
    unsigned uCurBreakCount = g_uBreakCount;
    ALLOC_STAT CurAllocStat = g_AllocStat;
    char szRealTimeInfo[1024];

    _snprintf(
        szRealTimeInfo, _countof(szRealTimeInfo),
        "BreakCount:%u\r\n"
        "BreakInc  :%u\r\n"
        "\r\n"
        "VMAlloc:%u\r\n"
        "VMFree:%u\r\n"
        "HeapAlloc:%u\r\n"
        "HeapFree :%u\r\n"
        "\r\n"
        "VMAllocInc:%u\r\n"
        "VMFreeInc:%u\r\n"
        "HeapAllocInc:%u\r\n"
        "HeapFreeInc :%u\r\n"
        "\r\n"
        "AllocSize:%.3fM\r\n"
        "AllocSizeInc:%.3fM\r\n"
        ,
        uCurBreakCount,
        uCurBreakCount - g_uLastBreakCount,

        CurAllocStat.uVMAllocCount,
        CurAllocStat.uVMFreeCount,
        CurAllocStat.uHeapAllocCount,
        CurAllocStat.uHeapFreeCount,

        CurAllocStat.uVMAllocCount   - g_LastAllocStat.uVMAllocCount,
        CurAllocStat.uVMFreeCount    - g_LastAllocStat.uVMFreeCount,
        CurAllocStat.uHeapAllocCount - g_LastAllocStat.uHeapAllocCount,
        CurAllocStat.uHeapFreeCount  - g_LastAllocStat.uHeapFreeCount,

        CurAllocStat.uTotalAllocSize / 1024.0f / 1024.0f,
        ((int)CurAllocStat.uTotalAllocSize - (int)g_LastAllocStat.uTotalAllocSize) / 1024.0f / 1024.0f
   );
    szRealTimeInfo[_countof(szRealTimeInfo) - 1] = '\0';
    g_uLastBreakCount = uCurBreakCount;
    g_LastAllocStat   = CurAllocStat;

    SetDlgItemText(IDC_REALTIME_STATE, szRealTimeInfo);

    // -------------------------- Append Debug String --------------------------
    g_StringCache.Get(&sMsg);
    if (sMsg.IsEmpty())
        return;
  
    //{//DEBUG
    //    static HANDLE hFile = ::CreateFile("log.txt", GENERIC_WRITE, FILE_SHARE_READ, NULL,  CREATE_ALWAYS,NULL, NULL);
    //    DWORD dwWriteSize = 0;
    //    ::WriteFile(hFile, sMsg.GetBuffer(), sMsg.GetLength(), &dwWriteSize, 0);
    //    //return;
    //}
    // ----- some program use "\n" for new line, convert to "\r\n" ------------
    sMsg.Replace("\r\n", "\n");
    sMsg.Replace("\n", "\r\n");

    nWindowTextLength = m_ctlDebugOutput.GetWindowTextLength();
    m_ctlDebugOutput.SetSel(nWindowTextLength, nWindowTextLength, TRUE);
    m_ctlDebugOutput.ReplaceSel(sMsg, FALSE);

    CDialog::OnTimer(nIDEvent);
}
////////////////////////////////////////////////////////////////////////////////
int CKG_D3DMemoryLeakAnalyzerDlg::_CopyDLlFromVCPath()
{
    int nResult  = false;
    int nRetCode = false;
    TCHAR szVC8Path[MAX_PATH];
    CString sCopyFromPathName;
    CString sCopyToPathName;
    
    nRetCode = ::GetEnvironmentVariable("VS80COMNTOOLS", szVC8Path, sizeof(szVC8Path) / sizeof(szVC8Path[0]));
    KG_ASSERT_EXIT(nRetCode > 0);

    sCopyFromPathName = szVC8Path;
    nRetCode = sCopyFromPathName.Replace("Tools\\", "IDE\\symsrv.dll");    
    KG_ASSERT_EXIT(nRetCode == 1);

    sCopyToPathName =  m_sCurProcPath + "\\symsrv.dll";
    nRetCode = ::CopyFile(sCopyFromPathName, sCopyToPathName, FALSE);
    KG_ASSERT_EXIT(nRetCode);

    sCopyFromPathName = szVC8Path;
    nRetCode = sCopyFromPathName.Replace("Tools\\", "IDE\\dbghelp.dll");    
    KG_ASSERT_EXIT(nRetCode == 1);

    sCopyToPathName =  m_sCurProcPath + "\\dbghelp.dll";
    nRetCode = ::CopyFile(sCopyFromPathName, sCopyToPathName, FALSE);
    KG_ASSERT_EXIT(nRetCode);

    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
int CKG_D3DMemoryLeakAnalyzerDlg::_ThreadFunction()
{
    int nResult  = false;
    int nRetCode = false;
    CString sDebugAppWorkPath;
    CString sSymbolPath; 
    CWnd *pWnd = NULL;
    std::string sLeakReport;

    m_nWorkThreadRunningFlag = true;

    pWnd = GetDlgItem(IDOK);
    ASSERT(pWnd);
    pWnd->EnableWindow(FALSE);
    m_ctlDebugOutput.SetWindowText("");
    m_ctlReportWnd.SetWindowText("");
    
    sDebugAppWorkPath = _GetDebugAppWorkPath();
    sSymbolPath       = _GetSymbolSearchPath();

    nRetCode = m_Debugger.Create(m_sDebugAppPathName, sDebugAppWorkPath, sSymbolPath);
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = m_Debugger.WaitForExit(DebugProcessCallBack, this);
    KG_ASSERT_EXIT(nRetCode);

    nRetCode = m_Debugger.GetLeakReport(&sLeakReport);
    KG_ASSERT_EXIT(nRetCode);

    m_ctlReportWnd.SetWindowText(sLeakReport.c_str());

    nResult = true;
Exit0:
    if (pWnd)
    {
        pWnd->EnableWindow(TRUE);
    }
    m_nWorkThreadRunningFlag = false;
    return nResult;
}
