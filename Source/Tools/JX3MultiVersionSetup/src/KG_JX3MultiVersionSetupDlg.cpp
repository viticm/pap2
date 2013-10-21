////////////////////////////////////////////////////////////////////////////////
//  Copyright(c) Kingsoft
//
//  FileName    : KG_JX3MultiVersionSetupDlg.cpp
//  Version     : 1.0
//  Creator     : Hu Hao
//  Create Date : 2009-10-22  16:58:18
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_JX3MultiVersionSetupApp.h"
#include "KG_JX3MultiVersionSetupDlg.h"
#include "KG_CompareVersionNumber.h"
#include "unzip.h"
#include <Psapi.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

////////////////////////////////////////////////////////////////////////////////////////////////////
const TCHAR KG_BASED_MAIN_VERSION_TYPE[]            = _T("zhcn");
const TCHAR KG_NEW_VERSION_SUB_TYPE[]               = _T("exp");
const TCHAR KG_NEW_VERSION_UPDATE_PAK_PREFIX[]      = _T("exp_");
const TCHAR KG_BASED_VERSION_NUMBER[]               = _T("1.3.3.1753");

////////////////////////////////////////////////////////////////////////////////////////////////////
KG_JX3MultiVersionSetupDlg::KG_JX3MultiVersionSetupDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KG_JX3MultiVersionSetupDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void KG_JX3MultiVersionSetupDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_PROGRESS, m_ProcessCtrl);
}

BEGIN_MESSAGE_MAP(KG_JX3MultiVersionSetupDlg, CDialog)
	ON_WM_PAINT()
    ON_WM_DESTROY()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
    ON_BN_CLICKED(IDC_BUTTON_START, &KG_JX3MultiVersionSetupDlg::OnBnClickedButtonStart)
    ON_BN_CLICKED(IDC_BUTTON_QUIT,  &KG_JX3MultiVersionSetupDlg::OnCancel)
END_MESSAGE_MAP()

// KG_JX3MultiVersionSetupDlg message handlers

BOOL KG_JX3MultiVersionSetupDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    m_nRunningFlag = false;
    m_nFinishFlag  = false;

    // Bold the notice title.
    m_ContextTitleFont.CreateFont(
        0, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
        CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, NULL
    );
    GetDlgItem(IDC_STATIC_TITLE)->SetFont(&m_ContextTitleFont, TRUE);
    GetDlgItem(IDC_STATIC_TITLE)->SetWindowText(_T("欢迎使用剑侠情缘网络版叁体验服务器安装程序"));

    GetDlgItem(IDC_STATIC_CONTEXT)->SetWindowText(
        _T("欢迎使用《剑侠情缘网络版叁》体验服务器安装程序。\n\n")
        _T("这个安装程序将指引您完成《剑侠情缘网络版叁》的体验服务器安装过程。\n\n")
        _T("开始安装之前，建议先关闭其他应用程序。\n\n")
        _T("单击 [ 开始安装 ] 执行。\n\n")
        _T("整个安装过程会花费您少许时间，请耐心等待。完成后，单击 [ 完成 ] 正常退出。\n\n")
        _T("安装完成后，您可以在启动游戏时选择进入体验服务器。\n\n")
        _T("建议：请将游戏正式版客户端升级到最新后安装。\n\n")
    );

    GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(_T("准备安装..."));

	return TRUE;  // return TRUE  unless you set the focus to a control
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void KG_JX3MultiVersionSetupDlg::OnPaint()
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

void KG_JX3MultiVersionSetupDlg::OnCancel()
{
    if (m_nRunningFlag) // Not allowed to cancel if running.
        return;

    m_WorkThread.Destroy();
    CDialog::OnCancel();
}

void KG_JX3MultiVersionSetupDlg::OnDestroy()
{
    m_ContextTitleFont.DeleteObject();
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR KG_JX3MultiVersionSetupDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void KG_JX3MultiVersionSetupDlg::OnBnClickedButtonStart()
{
     if (m_nFinishFlag)
     {
         _LoadJX3Launcher();
         OnCancel();
     }
     else
     {
         m_WorkThread.Destroy();
         m_WorkThread.Create(_WorkThreadFunction, this);
     }
}

void KG_JX3MultiVersionSetupDlg::_WorkThreadFunction(void *pvParam)
{
    KG_JX3MultiVersionSetupDlg *pThis = (KG_JX3MultiVersionSetupDlg *)pvParam;
    pThis->_ThreadFunction();
}

int KG_JX3MultiVersionSetupDlg::_ThreadFunction()
{
    int nResult  = false;
    int nRetCode = false;
    
    m_nRunningFlag = true;
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(false);  // Enable after install. 
    // Not allow quit if running.
    GetDlgItem(IDC_BUTTON_QUIT)->EnableWindow(false);  
    GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
    
    m_strErrorInfo.clear();
    nRetCode = _SetProcessPos(0);
    KGLOG_PROCESS_ERROR(nRetCode && "_SetProcessPos()");

    GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(_T("正在安装 - 检测正式版本客户端..."));
    nRetCode = _CheckBasedClientVersion();
    if (nRetCode == false)
    {
        if (m_strErrorInfo.empty())
            m_strErrorInfo = _T("安装失败 - 检测正式版本客户端失败，请先检查正式版本客户端完整性。");
        KGLOG_PROCESS_ERROR(false && "_CheckBasedClientVersion()");
    }

    GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(_T("正在安装 - 检查正式版本客户端是否正在运行..."));
    nRetCode = _CheckBasedVersionClientNotRunning();
    if (nRetCode == false)
    {
        if (m_strErrorInfo.empty())       
            m_strErrorInfo = _T("安装失败 - 检查正式版本客户端是否正在运行失败，请稍候重试。");
        KGLOG_PROCESS_ERROR(false && "_CheckBasedVersionClientNotRunning()");
    }

    GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(_T("正在安装 - 创建体验版本客户端目录..."));
    nRetCode = _CreateNewVersionDirectory();
    if (nRetCode == false)
    {
        if (m_strErrorInfo.empty())       
            m_strErrorInfo = _T("安装失败 - 创建体验版本客户端目录失败。");
        KGLOG_PROCESS_ERROR(false && "_CreateNewVersionDirectory()");
    }

    GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(_T("正在安装 - 准备拷贝文件..."));
    nRetCode = _CopyAllFiles();
    if (nRetCode == false)
    {
        if (m_strErrorInfo.empty())
            m_strErrorInfo = _T("安装失败 - 拷贝文件失败，请先检查正式版客户端完整性。");
        KGLOG_PROCESS_ERROR(false && "_CopyAllFiles()");
    }

    GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(_T("正在安装 - 修改体验版本配置文件..."));
    nRetCode = _EditNewVersionConfigureFiles();
    if (nRetCode == false)
    {
        if (m_strErrorInfo.empty())
            m_strErrorInfo = _T("安装失败 - 修改配置文件失败。");
        KGLOG_PROCESS_ERROR(false && "_CopyGameUpdaterIniFile()");
    }

    GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(_T("正在安装 - 解压剑网三加载器..."));
    nRetCode = _UnzipJX3Launcher();
    if (nRetCode == false)
    {
        if (m_strErrorInfo.empty())
            m_strErrorInfo = _T("安装失败 - 解压剑网三加载器失败。");
        KGLOG_PROCESS_ERROR(false && "_UnzipJX3Launcher()");
    }

    GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(_T("正在安装 - 修改加载器配置文件..."));
    nRetCode = _EditJX3LauncherIniFile();
    if (nRetCode == false)
    {
        if (m_strErrorInfo.empty())
            m_strErrorInfo = _T("安装失败 - 修改加载器配置文件失败。");
        KGLOG_PROCESS_ERROR(false && "_EditJX3LauncherIniFile()");
    }

    nRetCode = _SetProcessPos(95);
    KGLOG_PROCESS_ERROR(nRetCode && "_SetProcessPos()");

    GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(_T("正在安装 - 修改剑网三快捷方式..."));
    nRetCode = _EditJX3Shortcut();
    if (nRetCode == false)
    {
        if (m_strErrorInfo.empty())
            m_strErrorInfo = _T("安装失败 - 修改快捷方式失败。");
        KGLOG_PROCESS_ERROR(false && "_EditJX3Shortcut()");
    }

    GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(_T("正在安装 - 添加注册表信息..."));
    nRetCode = _WriteRegForNewVersion();
    // KGLOG_PROCESS_ERROR(nRetCode); // Setup had been done and ignore the result of writing register.

    nRetCode = _SetProcessPos(100);
    KGLOG_PROCESS_ERROR(nRetCode && "_SetProcessPos()");

    m_nFinishFlag = true;
    GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(_T("安装完成，您可以在启动游戏时选择进入体验服务器。"));
    GetDlgItem(IDC_BUTTON_START)->SetWindowText(_T("启动游戏"));
    GetDlgItem(IDC_BUTTON_QUIT)->SetWindowText(_T("完成"));

    nResult = true;
Exit0:
    m_nRunningFlag = false;
    if (!nResult)
    {
        if (m_strErrorInfo.empty())
            m_strErrorInfo = _T("安装失败 - 安装出现了错误，请稍候重试。");
        GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(m_strErrorInfo.c_str());
    }
    GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE, MF_BYCOMMAND | MF_ENABLED);
    GetDlgItem(IDC_BUTTON_QUIT)->EnableWindow(true);
    GetDlgItem(IDC_BUTTON_START)->EnableWindow(true);
    return nResult;
}

int KG_JX3MultiVersionSetupDlg::_SetProcessPos(int nPos)
{
	int nResult  = false;
	int nRetCode = false;
    TCHAR szTitle[128];

    KGLOG_PROCESS_ERROR(nPos >= 0);
    KGLOG_PROCESS_ERROR(nPos <= 100);

    nRetCode = _sntprintf(szTitle, _countof(szTitle), _T("%d%% - 剑侠情缘网络版叁 体验版安装"), nPos);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szTitle));
    CDialog::SetWindowText(szTitle);

    m_ProcessCtrl.SetPos(nPos);

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_CheckBasedClientVersion()
{
	int nResult  = false;
	int nRetCode = false;
    tstring strVersionCfgFileName;
    TCHAR szBuffer[MAX_PATH];
    int nCompareResult = 0;

    nRetCode = _ReadBasedVersionPathFromReg();
    if (nRetCode == false)
    {
        nRetCode = _AssumeAppPathAsBaseVersionPath();
        KGLOG_PROCESS_ERROR(nRetCode && "_AssumeAppPathAsBaseVersionPath()");
    }

    nRetCode = _IsNecessaryFilesExist();
    if (nRetCode == false)
    {
        m_strErrorInfo = _T("安装失败 - 请先检查正式版本客户端完整性，并将此程序放于正式版本客户端目录下再运行。");
        KGLOG_PROCESS_ERROR(false && "version.cfg not found.");
    }

    strVersionCfgFileName = m_strBasedVersionPath + _T("version.cfg");

    nRetCode = GetPrivateProfileString(
        _T("Version"), _T("Sword3.VersionLineName"), NULL, szBuffer, _countof(szBuffer),
        strVersionCfgFileName.c_str()
    );
    if (nRetCode == 0)
    {   // Before KG_BASED_VERSION_NUMBER, version.cfg was not ini file format.
        m_strErrorInfo = _T("安装失败 - 正式版本客户端版本太低，请先升级到最新版本。");
        KGLOG_PROCESS_ERROR(false && "Failed to load [Version]Sword3.VersionLineName");
    }

    nRetCode = _tcsicmp(szBuffer, KG_BASED_MAIN_VERSION_TYPE);
    if (nRetCode)
    {
        m_strErrorInfo = _T("安装失败 - 客户端版本不匹配，请将此程序放于正式版本客户端目录下再运行。");
        KGLOG_PROCESS_ERROR(false);
    }

    nRetCode = GetPrivateProfileString(
        _T("Version"), _T("Sword3.versionex"), NULL, szBuffer, _countof(szBuffer),
        strVersionCfgFileName.c_str()
    );
    if (nRetCode > 0)
    {
        m_strErrorInfo = _T("安装失败 - 客户端版本不匹配，请将此程序放于正式版本客户端目录下再运行。");
        KGLOG_PROCESS_ERROR(false);
    }

    nRetCode = GetPrivateProfileString(
        _T("Version"), _T("Sword3.version"), NULL, szBuffer, _countof(szBuffer),
        strVersionCfgFileName.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to load [Version]Sword3.version");

    nRetCode = KG_CompareVersionNumber(szBuffer, KG_BASED_VERSION_NUMBER, &nCompareResult);
    KGLOG_PROCESS_ERROR(nRetCode && "KG_CompareVersionNumber()");

    if (nCompareResult < 0)
    {
        m_strErrorInfo = _T("安装失败 - 正式版本客户端版本太低，请先升级到最新版本。");
        KGLOG_PROCESS_ERROR(false && "Error version number");
    }

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_ReadBasedVersionPathFromReg()
{
    int nResult  = false;
    int nRetCode = false;
    HKEY hKey = NULL;
    tstring strSubKey;
    TCHAR szBuffer[MAX_PATH];
    DWORD dwDataSizeInBytes = 0;

    strSubKey = tstring(_T("SOFTWARE\\Kingsoft\\JX3\\")) + KG_BASED_MAIN_VERSION_TYPE;

    nRetCode = RegOpenKeyEx(HKEY_LOCAL_MACHINE, strSubKey.c_str(), 0, KEY_READ, &hKey);
    KGLOG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

    dwDataSizeInBytes = sizeof(szBuffer);
    nRetCode = RegQueryValueEx(
        hKey,_T("installPath"), NULL, NULL, (LPBYTE)szBuffer, &dwDataSizeInBytes
    );
    KGLOG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

    m_strBasedVersionPath = szBuffer;

    // append '\\' if need.
    nRetCode = (int)m_strBasedVersionPath.length();
    KGLOG_PROCESS_ERROR(nRetCode > 0);

    if (m_strBasedVersionPath[nRetCode - 1] != _T('\\') && m_strBasedVersionPath[nRetCode - 1] != _T('/'))
        m_strBasedVersionPath += _T('\\');

    nResult = true;
Exit0:
    if (hKey)
    {
        RegCloseKey(hKey);
        hKey = NULL;
    }
    return nResult;
}

int KG_JX3MultiVersionSetupDlg::_AssumeAppPathAsBaseVersionPath()
{
    int nResult  = false;
    int nRetCode = false;
    TCHAR szApplicationName[MAX_PATH];
    tstring strApplicationPath;

    nRetCode = GetModuleFileName(NULL, szApplicationName, _countof(szApplicationName));
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szApplicationName));

    strApplicationPath = szApplicationName;
    nRetCode = (int)strApplicationPath.find_last_of(_T('\\'));
    KGLOG_PROCESS_ERROR(nRetCode != (int)tstring::npos);
    strApplicationPath.erase(nRetCode + 1);

    m_strBasedVersionPath = strApplicationPath;

    nResult = true;
Exit0:
    return nResult;
}

int KG_JX3MultiVersionSetupDlg::_IsNecessaryFilesExist()
{
	int nResult  = false;
	int nRetCode = false;
    tstring strFileName;
    const TCHAR *NECESSARY_FILE_LIST[] = {
        _T("version.cfg"),
        _T("gameupdater.ini"),
        _T("package.ini"),
        _T("config.ini")
    };

    for (int i = 0; i < _countof(NECESSARY_FILE_LIST); i++)
    {
        strFileName = m_strBasedVersionPath + NECESSARY_FILE_LIST[i];
        nRetCode = _taccess(strFileName.c_str(), 0);
        KG_PROCESS_ERROR(nRetCode == 0);
    }    

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_CheckBasedVersionClientNotRunning()
{
    int nResult  = false;
    int nRetCode = false;
    DWORD dwPIDArray[1024];
    DWORD dwPIDCountBytes = 0;
    unsigned uProcessCount = 0;
    HANDLE hCurrentProcess = NULL;
    TCHAR szProcessName[MAX_PATH];
    tstring strBasedExeFileFullName;
    const TCHAR *BASED_CLIENT_EXE_FILE_LIST[] = {
        _T("gameupdater.exe"),
        _T("JX3Client.exe"),
        _T("GameConfig.exe")
    };

    nRetCode = EnumProcesses(dwPIDArray, sizeof(dwPIDArray), &dwPIDCountBytes);
    KGLOG_PROCESS_ERROR(nRetCode && "EnumProcesses()");
    uProcessCount = dwPIDCountBytes / sizeof(DWORD);

    for (unsigned i = 0; i < uProcessCount; i++)
    {
        if (hCurrentProcess)
        {
            CloseHandle(hCurrentProcess);
            hCurrentProcess = NULL;
        }

        hCurrentProcess = OpenProcess(
            PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, dwPIDArray[i]
        );
        if (hCurrentProcess == NULL)    // Ignore the process that can't open.
            continue;

        nRetCode = GetModuleFileNameEx(hCurrentProcess, NULL, szProcessName, _countof(szProcessName));
        if (nRetCode <= 0)  // Ignore the process that can't get name.
            continue;

        for (int i = 0; i < _countof(BASED_CLIENT_EXE_FILE_LIST); i++)
        {
            strBasedExeFileFullName = m_strBasedVersionPath + BASED_CLIENT_EXE_FILE_LIST[i];

            nRetCode = _tcsicmp(szProcessName, strBasedExeFileFullName.c_str());
            if (nRetCode == 0)
            {
                m_strErrorInfo = _T("安装失败 - 请先关闭正在运行的客户端或更新程序后重试。");
                KGLOG_PROCESS_ERROR(false && "The exe file in the based version path is running");
            }
        }
    }

    nResult = true;
Exit0:
    if (hCurrentProcess)
    {
        CloseHandle(hCurrentProcess);
        hCurrentProcess = NULL;
    }
    return nResult;
}

int KG_JX3MultiVersionSetupDlg::_CreateNewVersionDirectory()
{
	int nResult  = false;
	int nRetCode = false;

    m_strNewVersionType = tstring(KG_BASED_MAIN_VERSION_TYPE) + _T('_') + KG_NEW_VERSION_SUB_TYPE;

    m_strNewVersionPath = m_strBasedVersionPath + m_strNewVersionType + _T('\\');
    
    nRetCode = CreateDirectory(m_strNewVersionPath.c_str(), NULL);
    if (nRetCode == 0)
    {
        nRetCode = GetLastError();
        KGLOG_PROCESS_ERROR(nRetCode == ERROR_ALREADY_EXISTS && "Create new version path.");
    }

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to create directory `%s`, Error code: %d",
            m_strNewVersionPath.c_str(), GetLastError()
        );
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_CopyAllFiles()
{
	int nResult  = false;
	int nRetCode = false;
    LONGLONG llAvailableDiskSize;
    const LONGLONG RESERVE_DISK_SIZE = 1024 * 1024; // Because edit files later, so reserve 1M space more.

    nRetCode = _GetClientFilesInfo();
    KGLOG_PROCESS_ERROR(nRetCode && "_GetClientFilesInfo()");

    nRetCode = _GetPakFilePath();
    KGLOG_PROCESS_ERROR(nRetCode && "_GetPakFilePath()");

    nRetCode = _GetBasedUpdatePakPrefix();
    KGLOG_PROCESS_ERROR(nRetCode && "_GetBasedUpdatePakPrefix()");

    nRetCode = _GetUpdatePakFilesInfo();
    KGLOG_PROCESS_ERROR(nRetCode && "_GetUpdatePakFilesInfo()");

    nRetCode = _RemoveExistNewVersionFiles();
    KGLOG_PROCESS_ERROR(nRetCode && "_RemoveExistNewVersionFiles()");

    nRetCode = _RemoveExistNewUpdatePakFiles();
    KGLOG_PROCESS_ERROR(nRetCode && "_RemoveExistNewUpdatePakFiles()");

    nRetCode = GetDiskFreeSpaceEx(
        m_strBasedVersionPath.c_str(), (PULARGE_INTEGER)&llAvailableDiskSize, NULL, NULL
    );
    KGLOG_PROCESS_ERROR(nRetCode && "GetDiskFreeSpaceEx()");

    if (llAvailableDiskSize < m_llCopyFileTotalSize + m_llUpdatePakTotalSize + RESERVE_DISK_SIZE)
    {
        m_strErrorInfo = _T("安装失败 - 磁盘空间不足。");
        KGLOG_PROCESS_ERROR(false && "Disk space not enough.");
    }

    nRetCode = _CopyClientFiles();
    KGLOG_PROCESS_ERROR(nRetCode && "_CopyClientFiles()");

    nRetCode = _CopyUpdatePakFiles();
    KGLOG_PROCESS_ERROR(nRetCode && "_CopyUpdatePakFiles()");    

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_GetClientFilesInfo()
{
	int nResult  = false;
	int nRetCode = false;
    int nShouldCopyFlag = false;
    tstring strFindFileName;
    HANDLE hFileHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA FindData;

    strFindFileName = m_strBasedVersionPath + _T("*");

    hFileHandle = FindFirstFile(strFindFileName.c_str(), &FindData);
    KGLOG_PROCESS_ERROR(hFileHandle != INVALID_HANDLE_VALUE);

    m_ClientFileList.clear();
    m_llCopyFileTotalSize = 0;
    while (true)
    {
        nRetCode = _CheckFileShouldBeCopied(FindData, &nShouldCopyFlag);
        KGLOG_PROCESS_ERROR(nRetCode && "_CheckFileShouldBeCopied()");

        if (nShouldCopyFlag)
        {
            m_ClientFileList.push_back(FindData.cFileName);
            m_llCopyFileTotalSize += ((LONGLONG)MAXDWORD + 1) * FindData.nFileSizeHigh + FindData.nFileSizeLow;            
        }

        nRetCode = FindNextFile(hFileHandle, &FindData);
        if (nRetCode == false)
        {
            nRetCode = GetLastError();
            if (nRetCode == ERROR_NO_MORE_FILES)
                break;

            KGLOG_PROCESS_ERROR(false && "FindNextFile()");
        }
    }

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(KGLOG_DEBUG, "Failed to get based version files, Error code: %d", GetLastError());
    }
    if (hFileHandle != INVALID_HANDLE_VALUE)
    {
        FindClose(hFileHandle);
        hFileHandle = INVALID_HANDLE_VALUE;
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_CheckFileShouldBeCopied(
    const WIN32_FIND_DATA &crFindData, int *pnRetCopyFlag
)
{
	int nResult  = false;
	int nRetCode = false;
    int nIsSelfFlag = false;

    ASSERT(pnRetCopyFlag);

    *pnRetCopyFlag = false;

    // the directories and the files in directories not be copied.
    nRetCode = crFindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;
    KG_PROCESS_SUCCESS(nRetCode);

    // The JX3MultiVersionSetup.exe not be copied.
    nRetCode = _CheckIsSelfExeFile(crFindData.cFileName, &nIsSelfFlag);
    KGLOG_PROCESS_ERROR(nRetCode && "_CheckIsSelfExeFile()");
    KG_PROCESS_SUCCESS(nIsSelfFlag);

    // The files be ignored not be copied.
    nRetCode = _IsIgnoreCopyFile(crFindData.cFileName);
    KG_PROCESS_SUCCESS(nRetCode);
    
    // here indicate this file should be copied.
    *pnRetCopyFlag = true;
Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_CheckIsSelfExeFile(const TCHAR cszFileName[], int *pnRetIsSelfFlag)
{
	int nResult  = false;
	int nRetCode = false;
    TCHAR szApplicationName[MAX_PATH];
    tstring strAppliacationName;

    ASSERT(cszFileName);
    ASSERT(cszFileName[0]);
    ASSERT(pnRetIsSelfFlag);

    nRetCode = GetModuleFileName(NULL, szApplicationName, _countof(szApplicationName));
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szApplicationName));

    strAppliacationName = szApplicationName;
    nRetCode = (int)strAppliacationName.find_last_of(_T('\\'));
    KGLOG_PROCESS_ERROR(nRetCode != (int)tstring::npos);
    strAppliacationName.erase(0, nRetCode + 1);

    nRetCode = _tcsicmp(strAppliacationName.c_str(), cszFileName);
    if (nRetCode == 0)
        *pnRetIsSelfFlag = true;
    else
        *pnRetIsSelfFlag = false;

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_IsIgnoreCopyFile(const TCHAR cszFileName[])
{
    int nResult  = false;
    int nRetCode = false;
    const TCHAR *IGNORE_COPY_FILE_LIST[] = {
        _T("JX3Launcher.exe"),
        _T("JX3LauncherD.exe"),
        _T("JX3Launcher.ini"),
        _T("uninst.exe"),
    };

    ASSERT(cszFileName);
    ASSERT(cszFileName[0]);

    for (int i = 0; i < _countof(IGNORE_COPY_FILE_LIST); i++)
    {
        nRetCode = _tcsicmp(cszFileName, IGNORE_COPY_FILE_LIST[i]);
        KG_PROCESS_SUCCESS(nRetCode == 0);
    }

    KG_PROCESS_ERROR(false);    // Here indicate not ignore file.

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KG_JX3MultiVersionSetupDlg::_GetPakFilePath()
{
	int nResult  = false;
	int nRetCode = false;
    tstring strPackageIniFileName;
    TCHAR szBuffer[MAX_PATH];

    strPackageIniFileName = m_strBasedVersionPath + _T("package.ini");

    nRetCode = GetPrivateProfileString(
        _T("SO3Client"), _T("Path"), NULL, szBuffer, _countof(szBuffer), strPackageIniFileName.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to load [SO3Client]Path");

    m_strPakFilePath = m_strBasedVersionPath;
    nRetCode = _tcsncicmp(szBuffer, _T(".\\"), _tcslen(_T(".\\")));
    if (nRetCode == 0)
        m_strPakFilePath += szBuffer + _tcslen(_T(".\\"));
    else
        m_strPakFilePath += szBuffer;

    // Append '\\' if need
    nRetCode = (int)m_strPakFilePath.length();
    ASSERT(nRetCode > 0);
    if (m_strPakFilePath[nRetCode - 1] != _T('\\') && m_strPakFilePath[nRetCode - 1] != _T('/'))
        m_strPakFilePath += _T('\\');

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_GetBasedUpdatePakPrefix()
{
	int nResult  = false;
	int nRetCode = false;
    tstring strPackageIniFileName;
    TCHAR szBuffer[MAX_PATH];

    strPackageIniFileName = m_strBasedVersionPath + _T("package.ini");

    // prefix of [SO3Client]0 is based update pak prefix
    nRetCode = GetPrivateProfileString(
        _T("SO3Client"), _T("0"), NULL, szBuffer, _countof(szBuffer), strPackageIniFileName.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szBuffer));

    m_strBasedUpdatePakPrefix = szBuffer;
    nRetCode = (int)m_strBasedUpdatePakPrefix.find_last_of(_T('_'));
    KGLOG_PROCESS_ERROR(nRetCode != (int)tstring::npos);
    m_strBasedUpdatePakPrefix.erase(nRetCode + 1);

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_GetUpdatePakFilesInfo()
{
	int nResult  = false;
	int nRetCode = false;
    tstring strPackageIniFileName;
    tstring strNewPakFileName;
    tstring strUpdatePakFileFullName;
    int nIndex = 0;
    TCHAR szIndex[32];
    TCHAR szBuffer[MAX_PATH];
    LONGLONG llFileSize;
    std::pair<_UPDATE_PAK_MAP::iterator, bool> InsertResult;
    _UPDATE_PAK_MAP::iterator it;

    strPackageIniFileName = m_strBasedVersionPath + _T("package.ini");

    // Get pak files with with same prefix with based update pak prefix.
    m_UpdatePakMap.clear();
    nIndex = 0;
    while (true)
    {
        nRetCode = _sntprintf(szIndex, _countof(szIndex), _T("%d"), nIndex);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        KGLOG_PROCESS_ERROR(nRetCode < _countof(szIndex));

        nRetCode = GetPrivateProfileString(
            _T("SO3Client"), szIndex, NULL, szBuffer, _countof(szBuffer),
            strPackageIniFileName.c_str()
        );
        if (nRetCode == 0)
            break;

        nRetCode = _tcsncicmp(
            szBuffer, m_strBasedUpdatePakPrefix.c_str(), m_strBasedUpdatePakPrefix.length()
        );
        if (nRetCode == 0)
        {
            strNewPakFileName  = KG_NEW_VERSION_UPDATE_PAK_PREFIX;
            strNewPakFileName += szBuffer + m_strBasedUpdatePakPrefix.length();

            InsertResult = m_UpdatePakMap.insert(make_pair(tstring(szBuffer), strNewPakFileName));
            KGLOG_PROCESS_ERROR(InsertResult.second);
        }

        nIndex++;
    }

    m_llUpdatePakTotalSize = 0;
    for (it = m_UpdatePakMap.begin(); it != m_UpdatePakMap.end(); ++it)
    {
        strUpdatePakFileFullName = m_strPakFilePath + it->first;

        // Although update_0.pak not exist if new setup, After 1.0.10.1579, all pak should exist
        nRetCode = _GetFileSize(strUpdatePakFileFullName.c_str(), &llFileSize);
        KGLOG_PROCESS_ERROR(nRetCode && "_GetFileSize()");

        m_llUpdatePakTotalSize += llFileSize;
    }

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_GetFileSize(const TCHAR cszFileName[], LONGLONG *pllRetSize)
{
	int nResult  = false;
    HANDLE hFileHandle = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA FindData;

    ASSERT(cszFileName);
    ASSERT(cszFileName[0]);
    ASSERT(pllRetSize);

    hFileHandle = FindFirstFile(cszFileName, &FindData);
    KGLOG_PROCESS_ERROR(hFileHandle != INVALID_HANDLE_VALUE);

    *pllRetSize = ((LONGLONG)MAXDWORD + 1) * FindData.nFileSizeHigh + FindData.nFileSizeLow;            
    
	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to get size of `%s`, Error code: %d", cszFileName, GetLastError()
        );
    }
    if (hFileHandle)
    {
        FindClose(hFileHandle);
        hFileHandle = INVALID_HANDLE_VALUE;
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_RemoveExistNewVersionFiles()
{
	int nResult  = false;
	int nRetCode = false;
    tstring strFindFileName;
    tstring strFullPakFileName;
    HANDLE hFindFile = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA FileData;

    strFindFileName = m_strNewVersionPath + _T('*');

    hFindFile = FindFirstFile(strFindFileName.c_str(), &FileData);
    KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);

    while (true)
    {
        strFullPakFileName = m_strNewVersionPath + FileData.cFileName;

        SetFileAttributes(strFullPakFileName.c_str(), FILE_ATTRIBUTE_NORMAL);
        _tremove(strFullPakFileName.c_str());

        nRetCode = FindNextFile(hFindFile, &FileData);
        if (nRetCode == false)
        {
            nRetCode = GetLastError();
            if (nRetCode == ERROR_NO_MORE_FILES)
                break;

            KGLogPrintf(KGLOG_DEBUG, "Failed to find next file, Error code: %d", GetLastError());
            KGLOG_PROCESS_ERROR(false);
        }
    }

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to remove exist new update pak file, Error code: %d", GetLastError()
        );
    }
    if (hFindFile != INVALID_HANDLE_VALUE)
    {
        FindClose(hFindFile);
        hFindFile = INVALID_HANDLE_VALUE;
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_RemoveExistNewUpdatePakFiles()
{
	int nResult  = false;
	int nRetCode = false;
    tstring strFindFileName;
    tstring strFullPakFileName;
    HANDLE hFindFile = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA FileData;

    strFindFileName = m_strPakFilePath + _T('*');

    hFindFile = FindFirstFile(strFindFileName.c_str(), &FileData);
    KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);

    while (true)
    {
        nRetCode = _tcsnicmp(
            FileData.cFileName, KG_NEW_VERSION_UPDATE_PAK_PREFIX, 
            _tcslen(KG_NEW_VERSION_UPDATE_PAK_PREFIX)
        );
        if (nRetCode == 0)
        {
            strFullPakFileName = m_strPakFilePath + FileData.cFileName;

            SetFileAttributes(strFullPakFileName.c_str(), FILE_ATTRIBUTE_NORMAL);
            _tremove(strFullPakFileName.c_str());
        }

        nRetCode = FindNextFile(hFindFile, &FileData);
        if (nRetCode == false)
        {
            nRetCode = GetLastError();
            if (nRetCode == ERROR_NO_MORE_FILES)
                break;

            KGLogPrintf(KGLOG_DEBUG, "Failed to find next file, Error code: %d", GetLastError());
            KGLOG_PROCESS_ERROR(false);
        }
    }

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to remove exist new update pak file, Error code: %d", GetLastError()
        );
    }
    if (hFindFile != INVALID_HANDLE_VALUE)
    {
        FindClose(hFindFile);
        hFindFile = INVALID_HANDLE_VALUE;
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_CopyClientFiles()
{
    int nResult  = false;
    int nRetCode = false;
    tstring strSourceFileName;
    tstring strDestineFileName;
    tstring strShowText;
    float fCurrentProgress = 0.0f;
    _FILE_NAME_LIST::const_iterator cit;
    
    fCurrentProgress = 0.0f;
    for (cit = m_ClientFileList.begin(); cit != m_ClientFileList.end(); ++cit)
    {
        strShowText = tstring(_T("正在安装 - 拷贝文件: ")) + *cit;
        GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(strShowText.c_str());

        strSourceFileName  = m_strBasedVersionPath + *cit;
        strDestineFileName = m_strNewVersionPath  +  *cit;

        SetFileAttributes(strDestineFileName.c_str(), FILE_ATTRIBUTE_NORMAL);
        _tremove(strDestineFileName.c_str());

        nRetCode = CopyFile(strSourceFileName.c_str(), strDestineFileName.c_str(), false);
        KGLOG_PROCESS_ERROR(nRetCode && "CopyFile()");

        fCurrentProgress += 50.0f / m_ClientFileList.size();
        nRetCode = _SetProcessPos((int)fCurrentProgress);
        KGLOG_PROCESS_ERROR(nRetCode && "_SetProcessPos()");
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to copy file `%s` to `%s`, Error code: %d",
            strSourceFileName.c_str(), strDestineFileName.c_str(), GetLastError()
        );
    }
    return nResult;
}

int KG_JX3MultiVersionSetupDlg::_CopyUpdatePakFiles()
{
	int nResult  = false;
	int nRetCode = false;
    tstring strSourceFileName;
    tstring strDestineFileName;
    tstring strShowText;
    float fCurrentProgress = 0.0f;
    _UPDATE_PAK_MAP::iterator it;

    fCurrentProgress = 50.0f;
    for (it = m_UpdatePakMap.begin(); it != m_UpdatePakMap.end(); ++it)
    {
        strShowText = tstring(_T("正在安装 - 拷贝文件: ")) + it->first;
        GetDlgItem(IDC_STATIC_STATUS)->SetWindowText(strShowText.c_str());

        strSourceFileName  = m_strPakFilePath + it->first;
        strDestineFileName = m_strPakFilePath + it->second;
        
        nRetCode = CopyFile(strSourceFileName.c_str(), strDestineFileName.c_str(), false);
        KGLOG_PROCESS_ERROR(nRetCode && "CopyFile()");

        fCurrentProgress += 40.0f / m_strPakFilePath.size();
        nRetCode = _SetProcessPos((int)fCurrentProgress);
        KGLOG_PROCESS_ERROR(nRetCode && "_SetProcessPos()");
    }   

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to copy file `%s` to `%s`, Error code: %d",
            strSourceFileName.c_str(), strDestineFileName.c_str(), GetLastError()
        );
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_EditNewVersionConfigureFiles()
{
	int nResult  = false;
	int nRetCode = false;

    nRetCode = _EditNewPackageIniFile();
    KGLOG_PROCESS_ERROR(nRetCode && "_EditNewPackageIniFile()");

    nRetCode = _EditNewGameUpdaterIniFile();
    KGLOG_PROCESS_ERROR(nRetCode && "_EditNewGameUpdaterIniFile()");
    
    nRetCode = _EditNewVersionCfgFile();
    KGLOG_PROCESS_ERROR(nRetCode && "_EditNewVersionCfgFile()");

    nRetCode  = _EditNewConfigIniFile();
    KGLOG_PROCESS_ERROR(nRetCode && "_EditNewConfigIniFile()");

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_EditNewPackageIniFile()
{
	int nResult  = false;
	int nRetCode = false;
    tstring strFileName;
    int nIndex = 0;
    TCHAR szIndex[32];
    TCHAR szBuffer[MAX_PATH];
    _UPDATE_PAK_MAP::iterator it;

    strFileName = m_strNewVersionPath + _T("package.ini");
    
    // Remove read-only attribute.
    SetFileAttributes(strFileName.c_str(), FILE_ATTRIBUTE_NORMAL);

    nIndex = 0;
    while (true)
    {
        nRetCode = _sntprintf(szIndex, _countof(szIndex), _T("%d"), nIndex);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        KGLOG_PROCESS_ERROR(nRetCode < _countof(szIndex));

        nRetCode = GetPrivateProfileString(
            _T("SO3Client"), szIndex, NULL, szBuffer, _countof(szBuffer), strFileName.c_str()
        );
        if (nRetCode == 0)
            break;

        it = m_UpdatePakMap.find(tstring(szBuffer));
        if (it != m_UpdatePakMap.end())
        {
            nRetCode = WritePrivateProfileString(
                _T("SO3Client"), szIndex, (it->second).c_str(), strFileName.c_str()
            );
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        nIndex++;        
    }    

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_EditNewGameUpdaterIniFile()
{
	int nResult  = false;
	int nRetCode = false;
    tstring strFileName;
    const TCHAR NEW_VERSIONURL[]       = _T("http://jx3.autoupdate.kingsoft.com/jx3/");
    const TCHAR NEW_VERSION_BULLETIN[] = _T("http://jx3.autoupdate.kingsoft.com/jx3/zhcn_exp/login_web/config_updater.xml");

    strFileName = m_strNewVersionPath + _T("gameupdater.ini");

    // Remove read-only attribute.
    SetFileAttributes(strFileName.c_str(), FILE_ATTRIBUTE_NORMAL);

    nRetCode = WritePrivateProfileString(
        _T("server_1"), _T("ip"), NEW_VERSIONURL, strFileName.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to write [server_1]ip");

    nRetCode = WritePrivateProfileString(
        _T("server_1"), _T("bulletin"), NEW_VERSION_BULLETIN, strFileName.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to write [server_1]bulletin");

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to edit `%s`, Error code:", strFileName.c_str(), GetLastError()
        );
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_EditNewVersionCfgFile()
{
	int nResult  = false;
	int nRetCode = false;
    tstring strFileName;
    const TCHAR NEW_VERSION_SERVER_LIST_URL[] = _T("http://jx3.autoupdate.kingsoft.com/jx3/zhcn_exp/serverlist/serverlist.ini");
    const TCHAR NEW_VERSION_NAME[] = _T("体验服务器");

    strFileName = m_strNewVersionPath + _T("version.cfg");

    // Remove read-only attribute.
    SetFileAttributes(strFileName.c_str(), FILE_ATTRIBUTE_NORMAL);

    nRetCode = WritePrivateProfileString(
        _T("Version"), _T("Sword3.version"), KG_BASED_VERSION_NUMBER, strFileName.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to write [Version]Sword3.version");    

    nRetCode = WritePrivateProfileString(
        _T("Version"), _T("Sword3.SvrListUrl"), NEW_VERSION_SERVER_LIST_URL, strFileName.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to write [Version]Sword3.SvrListUrl");

    nRetCode = WritePrivateProfileString(
        _T("Version"), _T("Sword3.versionex"), KG_NEW_VERSION_SUB_TYPE, strFileName.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to write [Version]Sword3.versionex");

    nRetCode = WritePrivateProfileString(
        _T("Version"), _T("Sword3.versionName"), NEW_VERSION_NAME, strFileName.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to write [Version]Sword3.versionName");

    nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to edit `%s`, Error code: %d", strFileName.c_str(), GetLastError()
        );
    }
    return nResult;
}

int KG_JX3MultiVersionSetupDlg::_EditNewConfigIniFile()
{
	int nResult  = false;
	int nRetCode = false;
    tstring strFileName;
    const TCHAR lAUNCH_PRIOR_EXE_ROOT_PATH_NEW_VALUE[] = _T("1");

    strFileName = m_strNewVersionPath + _T("config.ini");

    // Remove read-only attribute.
    SetFileAttributes(strFileName.c_str(), FILE_ATTRIBUTE_NORMAL);

    nRetCode = WritePrivateProfileString(
        _T("Launch"), _T("PriorExeRootPath"), lAUNCH_PRIOR_EXE_ROOT_PATH_NEW_VALUE, strFileName.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to write [Launch]PriorExeRootPath");    

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to edit `%s`, Error code: %d", strFileName.c_str(), GetLastError()
        );
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_UnzipJX3Launcher()
{
	int nResult  = false;
	int nRetCode = false;
    TCHAR szApplicationName[MAX_PATH];
    HZIP hZip = NULL;
    ZRESULT zResult = ZR_FLATE;
    int nItemIndex = 0;
    ZIPENTRY ZipEntry;
    tstring strUnzipFileName;

    nRetCode = GetModuleFileName(NULL, szApplicationName, _countof(szApplicationName));
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szApplicationName));

    hZip = OpenZip(szApplicationName, NULL);
    KGLOG_PROCESS_ERROR(hZip);

    zResult = FindZipItem(hZip, _T("JX3Launcher.exe"), false, &nItemIndex, &ZipEntry);
    KGLOG_PROCESS_ERROR(zResult == ZR_OK);
    KGLOG_PROCESS_ERROR(nItemIndex >= 0);

    strUnzipFileName = m_strBasedVersionPath + _T("JX3Launcher.exe");
    
    // Remove old JX3Launcher if it existed.
    SetFileAttributes(strUnzipFileName.c_str(), FILE_ATTRIBUTE_NORMAL);
    _tremove(strUnzipFileName.c_str());

    zResult = UnzipItem(hZip, nItemIndex, strUnzipFileName.c_str());
    KGLOG_PROCESS_ERROR(zResult == ZR_OK);

	nResult = true;
Exit0:
    if (hZip)
    {
        CloseZip(hZip);
        hZip = NULL;
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_EditJX3LauncherIniFile()
{
	int nResult  = false;
	int nRetCode = false;
    tstring strIniFileName;

    strIniFileName = m_strBasedVersionPath + _T("JX3Launcher.ini");

    // Remove read-only attribute.
    nRetCode = SetFileAttributes(strIniFileName.c_str(), FILE_ATTRIBUTE_NORMAL);

    // Remove error file if exist.
    nRetCode = _MakeSureSection0AsBasedVersion(strIniFileName.c_str());
    KGLOG_PROCESS_ERROR(nRetCode && "_MakeSureSection0AsBasedVersion()");

    nRetCode = _AddNewVersionSectionIfNotExist(strIniFileName.c_str());
    KGLOG_PROCESS_ERROR(nRetCode && "_AddNewVersionSectionIfNotExist()");

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_MakeSureSection0AsBasedVersion(const TCHAR cszIniFileName[])
{
	int nResult  = false;
	int nRetCode = false;
    TCHAR BASED_VERSION_LAUNCH_NAME[] = _T("正式版服务器");
    TCHAR BASED_VERSION_LAUNCH_CLUE[] = _T("正式运营（含公测不删档）服务器，无需申请激活，版本稳定。");

    ASSERT(cszIniFileName);
    ASSERT(cszIniFileName[0]);

    nRetCode = WritePrivateProfileString(_T("0"), _T("Name"), BASED_VERSION_LAUNCH_NAME, cszIniFileName);
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to write [0]Name");

    nRetCode = WritePrivateProfileString(_T("0"), _T("Path"), _T(".\\"), cszIniFileName);
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to write [0]Path");

    nRetCode = WritePrivateProfileString(_T("0"), _T("Clue"), BASED_VERSION_LAUNCH_CLUE, cszIniFileName);
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to write [0]Clue");

	nResult = true;
Exit0:
    if (!nRetCode)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to set section 0 as based version, Error code: %d.", GetLastError()
        );
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_AddNewVersionSectionIfNotExist(const TCHAR cszIniFileName[])
{
	int nResult  = false;
	int nRetCode = false;
    tstring strNewVersionPath;
    int nIndex = 0;
    TCHAR szIndex[32];
    TCHAR szBuffer[MAX_PATH];
    TCHAR NEW_VERSION_LAUNCH_NAME[] = _T("体验版服务器");
    TCHAR NEW_VERSION_LAUNCH_CLUE[] = _T("优先体验新内容服务器，需申请激活。");

    ASSERT(cszIniFileName);
    ASSERT(cszIniFileName[0]);

    strNewVersionPath  = tstring(_T(".\\")) + m_strNewVersionType + _T('\\');

    nIndex = 0;
    while (true)
    {
        nRetCode = _sntprintf(szIndex, _countof(szIndex), _T("%d"), nIndex);
        KGLOG_PROCESS_ERROR(nRetCode > 0);
        KGLOG_PROCESS_ERROR(nRetCode < _countof(szIndex));

        nRetCode = GetPrivateProfileString(
            szIndex, _T("Path"), NULL, szBuffer, _countof(szBuffer), cszIniFileName
        );
        if (nRetCode == 0)  // Not found, write new version section into the end.
            break;

        nRetCode = _tcsicmp(szBuffer, strNewVersionPath.c_str());
        if (nRetCode == 0)  // Found, rewrite data and make sure right name and clue.
            break;

        nIndex++;
    }

    // Add new version section.
    nRetCode = _sntprintf(szIndex, _countof(szIndex), _T("%d"), nIndex);
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szIndex));

    nRetCode = WritePrivateProfileString(szIndex, _T("Name"), NEW_VERSION_LAUNCH_NAME, cszIniFileName);
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to write [New]Name");

    nRetCode = WritePrivateProfileString(szIndex, _T("Path"), strNewVersionPath.c_str(), cszIniFileName);
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to write [New]Path");

    nRetCode = WritePrivateProfileString(szIndex, _T("Clue"), NEW_VERSION_LAUNCH_CLUE, cszIniFileName);
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to write [New]Clue");

	nResult = true;
Exit0:
    if (!nRetCode)
    {
        KGLogPrintf(KGLOG_DEBUG, "Failed to add new version section, Error code: %d.", GetLastError());
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_EditJX3Shortcut()
{
	int nResult  = false;
	int nRetCode = false;
    int nCoInitFlag = false;
    int nDesktopShortcutExistedFlag = false;
    HRESULT hRetCode = S_FALSE;
    LPITEMIDLIST pItemIDList = NULL;
    TCHAR szDir[MAX_PATH];
    tstring strCurrentQuickLaunchPath;

    hRetCode = CoInitialize(NULL);
    KGLOG_PROCESS_ERROR(hRetCode == S_OK);
    nCoInitFlag = true;

    // Desktop of all users.
    hRetCode = SHGetSpecialFolderLocation(NULL, CSIDL_COMMON_DESKTOPDIRECTORY, &pItemIDList);
    KGLOG_PROCESS_ERROR(hRetCode == S_OK && "SHGetSpecialFolderLocation()");

    hRetCode = SHGetPathFromIDList(pItemIDList, szDir);
    KGLOG_PROCESS_ERROR(hRetCode && "SHGetPathFromIDList()");

    nRetCode = _EditShortcutInDir(szDir);
    KGLOG_PROCESS_ERROR(nRetCode && "_EditShortcutInDir()");

    nRetCode = _IsIncludeJX3Shortcut(szDir);
    if (nRetCode)
        nDesktopShortcutExistedFlag = true;

    // Desktop of current users.
    hRetCode = SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOPDIRECTORY, &pItemIDList);
    KGLOG_PROCESS_ERROR(hRetCode == S_OK && "SHGetSpecialFolderLocation()");

    hRetCode = SHGetPathFromIDList(pItemIDList, szDir);
    KGLOG_PROCESS_ERROR(hRetCode && "SHGetPathFromIDList()");

    nRetCode = _EditShortcutInDir(szDir);
    KGLOG_PROCESS_ERROR(nRetCode && "_EditShortcutInDir()");

    nRetCode = _IsIncludeJX3Shortcut(szDir);
    if (nRetCode)
        nDesktopShortcutExistedFlag = true;

    if (nDesktopShortcutExistedFlag == false)
    {
        nRetCode = _CreateJX3DesktopShortcut(szDir);
        // KGLOG_PROCESS_ERROR(nRetCode && "_CreateJX3DesktopShortcut()");
    }

    // Start menu of all users.
    hRetCode = SHGetSpecialFolderLocation(NULL, CSIDL_COMMON_STARTMENU, &pItemIDList);
    KGLOG_PROCESS_ERROR(hRetCode == S_OK && "SHGetSpecialFolderLocation()");

    hRetCode = SHGetPathFromIDList(pItemIDList, szDir);
    KGLOG_PROCESS_ERROR(hRetCode && "SHGetPathFromIDList()");

    nRetCode = _EditShortcutInDir(szDir);
    KGLOG_PROCESS_ERROR(nRetCode && "_EditShortcutInDir()");

    // Start menu of current users.
    hRetCode = SHGetSpecialFolderLocation(NULL, CSIDL_STARTMENU, &pItemIDList);
    KGLOG_PROCESS_ERROR(hRetCode == S_OK && "SHGetSpecialFolderLocation()");

    hRetCode = SHGetPathFromIDList(pItemIDList, szDir);
    KGLOG_PROCESS_ERROR(hRetCode && "SHGetPathFromIDList()");

    nRetCode = _EditShortcutInDir(szDir);
    KGLOG_PROCESS_ERROR(nRetCode && "_EditShortcutInDir()");

    // Quick launch of current user.
    hRetCode = SHGetSpecialFolderLocation(NULL, CSIDL_APPDATA, &pItemIDList);
    KGLOG_PROCESS_ERROR(hRetCode == S_OK && "SHGetSpecialFolderLocation()");

    hRetCode = SHGetPathFromIDList(pItemIDList, szDir);
    KGLOG_PROCESS_ERROR(hRetCode && "SHGetPathFromIDList()");

    strCurrentQuickLaunchPath = tstring(szDir) + _T("\\Microsoft\\Internet Explorer\\Quick Launch");
    
    nRetCode = _EditShortcutInDir(strCurrentQuickLaunchPath.c_str());
    KGLOG_PROCESS_ERROR(nRetCode && "_EditShortcutInDir()");

	nResult = true;
Exit0:
    if (nCoInitFlag)
    {
        CoUninitialize();
        nCoInitFlag = false;
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_EditShortcutInDir(const TCHAR cszDir[])
{
	int nResult  = false;
	int nRetCode = false;    
    tstring strDirPath;
    tstring strSubDirPath;
    tstring strFindFileName;
    tstring strFullFileName;
    HANDLE hFindFile = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA FileData;
    const TCHAR IGNORE_DIR_PREFIX[] = _T(".");  // Ignore repeat visit.

    ASSERT(cszDir);
    ASSERT(cszDir[0]);

    strDirPath = cszDir;
    // Append '\\' if need
    nRetCode = (int)strDirPath.length();
    ASSERT(nRetCode > 0);
    if (strDirPath[nRetCode - 1] != _T('\\') && strDirPath[nRetCode - 1] != _T('/'))
        strDirPath += _T('\\');

    strFindFileName = strDirPath + _T('*');
    hFindFile = FindFirstFile(strFindFileName.c_str(), &FileData);
    KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);    

    while (true)
    {
        if (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
        {
            nRetCode = _tcsnicmp(FileData.cFileName, IGNORE_DIR_PREFIX, _tcslen(IGNORE_DIR_PREFIX));
            if (nRetCode)
            {
                strSubDirPath = strDirPath + FileData.cFileName;
                nRetCode = _EditShortcutInDir(strSubDirPath.c_str());
                // Ignore the directories that could not traversed, such as reparse point, etc.
                // KGLOG_PROCESS_ERROR(nRetCode && "_EditShortcutInDir()");
            }
        }
        else
        {
            strFullFileName = strDirPath + FileData.cFileName;
            nRetCode = _EditShortcutIfLinkedToJX3(strFullFileName.c_str());
            // Ignore the file that could not edit.
            // KGLOG_PROCESS_ERROR(nRetCode && "_EditShortcutIfLinkedToJX3()");
        }

        nRetCode = FindNextFile(hFindFile, &FileData);
        if (nRetCode == false)
        {
            nRetCode = GetLastError();
            if (nRetCode == ERROR_NO_MORE_FILES)
                break;

            KGLOG_PROCESS_ERROR(false && "FindNextFile()");
        }
    }

	nResult = true;
Exit0:
    if (!nResult)
    {
        KGLogPrintf(
            KGLOG_DEBUG, "Failed to edit shortcut in directory `%s`, Error code: %d",
            cszDir, GetLastError()
        );
    }
    if (hFindFile != INVALID_HANDLE_VALUE)
    {
        FindClose(hFindFile);
        hFindFile = NULL;
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_IsIncludeJX3Shortcut(const TCHAR cszDir[])
{
	int nResult  = false;
	int nRetCode = false;
    tstring strDir;
    tstring strFindFileName;
    tstring strFileFullName;
    HANDLE hFindFile = INVALID_HANDLE_VALUE;
    WIN32_FIND_DATA FileData;

    ASSERT(cszDir);
    ASSERT(cszDir[0]);

    strDir = cszDir;
    // Append '\\' if need
    nRetCode = (int)strDir.length();
    ASSERT(nRetCode > 0);
    if (strDir[nRetCode - 1] != _T('\\') && strDir[nRetCode - 1] != _T('/'))
        strDir += _T('\\');

    strFindFileName = strDir + _T('*');
    hFindFile = FindFirstFile(strFindFileName.c_str(), &FileData);
    KG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);

    while (true)
    {
        if ((FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == false)
        {
            strFileFullName = strDir + FileData.cFileName;
            nRetCode = _IsJX3DesktopShortcut(strFileFullName.c_str());
            KG_PROCESS_SUCCESS(nRetCode);   // Shortcut existed.
        }

        nRetCode = FindNextFile(hFindFile, &FileData);
        if (nRetCode == false)
        {
            nRetCode = GetLastError();
            if (nRetCode == ERROR_NO_MORE_FILES)
                break;

            KG_PROCESS_ERROR(false && "FindNextFile()");
        }
    }

    KG_PROCESS_ERROR(false);    // Here indicate no jx3 shortcut.

Exit1:
	nResult = true;
Exit0:
    if (hFindFile != INVALID_HANDLE_VALUE)
    {
        FindClose(hFindFile);
        hFindFile = NULL;
    }
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_EditShortcutIfLinkedToJX3(const TCHAR cszLinkFileFullName[])
{
    int nResult  = false;
    int nRetCode = false;
    tstring strGameUpdaterOldLink;
    tstring strGameConfigOldLink;
    tstring strNewLinkedFileName;
    tstring strOldLinkedFileName;
    tstring strOldArgument;

    ASSERT(cszLinkFileFullName);
    ASSERT(cszLinkFileFullName[0]);

    strGameUpdaterOldLink = m_strBasedVersionPath + _T("gameupdater.exe");
    strGameConfigOldLink  = m_strBasedVersionPath + _T("GameConfig.exe");

    strNewLinkedFileName = m_strBasedVersionPath + _T("JX3Launcher.exe");

    nRetCode = _GetShortcutLinkedFileName(
        cszLinkFileFullName, &strOldLinkedFileName, &strOldArgument
    );
    KGLOG_PROCESS_ERROR(nRetCode && "_GetShortcutLinkedFileName()");

    nRetCode = _tcsicmp(strOldLinkedFileName.c_str(), strGameUpdaterOldLink.c_str());
    if (nRetCode == 0)
    {
        nRetCode = _SetShortcutLinkedFileName(
            cszLinkFileFullName, strNewLinkedFileName.c_str(), _T("")
        );
        KGLOG_PROCESS_ERROR(nRetCode && "_SetShortcutLinkedFileName()");
    }

    nRetCode = _tcsicmp(strOldLinkedFileName.c_str(), strGameConfigOldLink.c_str());
    if (nRetCode == 0)
    {
        nRetCode = _SetShortcutLinkedFileName(
            cszLinkFileFullName, strNewLinkedFileName.c_str(), _T("-c")
        );  // -c indicate JX3Launcher.exe load GameConfig.exe
        KGLOG_PROCESS_ERROR(nRetCode && "_SetShortcutLinkedFileName()");
    }

    nResult = true;
Exit0:
    return nResult;
}


int KG_JX3MultiVersionSetupDlg::_IsJX3DesktopShortcut(const TCHAR cszFileName[])
{
	int nResult  = false;
	int nRetCode = false;
    tstring strJX3LauncherFileName;
    tstring strLinkedFileName;
    tstring strArgument;

    ASSERT(cszFileName);
    ASSERT(cszFileName[0]);

    strJX3LauncherFileName = m_strBasedVersionPath + _T("JX3Launcher.exe");

    nRetCode = _GetShortcutLinkedFileName(cszFileName, &strLinkedFileName, &strArgument);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = _tcsicmp(strLinkedFileName.c_str(), strJX3LauncherFileName.c_str());
    KG_PROCESS_ERROR(nRetCode == 0);

    nRetCode = strArgument.empty();
    KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_CreateJX3DesktopShortcut(const TCHAR cszDir[])
{
	int nResult  = false;
	int nRetCode = false;
    tstring strDir;
    tstring strDesktopShortcutFileName;
    tstring strJX3LauncherFileName;

    ASSERT(cszDir);
    ASSERT(cszDir[0]);

    strDir = cszDir;
    // Append '\\' if need
    nRetCode = (int)strDir.length();
    ASSERT(nRetCode > 0);
    if (strDir[nRetCode - 1] != _T('\\') && strDir[nRetCode - 1] != _T('/'))
        strDir += _T('\\');

    strDesktopShortcutFileName = strDir + _T("剑侠情缘网络版叁.lnk");
    strJX3LauncherFileName = m_strBasedVersionPath + _T("JX3Launcher.exe");
    
    nRetCode = _SetShortcutLinkedFileName(
        strDesktopShortcutFileName.c_str(), strJX3LauncherFileName.c_str(), _T("")
    );
    KGLOG_PROCESS_ERROR(nRetCode && "_SetShortcutLinkedFileName()");

	nResult = true;
Exit0:
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_GetShortcutLinkedFileName(
    const TCHAR cszShortcutFileName[], tstring *pstrRetLinkedFileName, tstring *pstrRetArgument
)
{
	int nResult  = false;
    HRESULT hRetCode = S_FALSE;
    IShellLink *piShellLink = NULL;
    IPersistFile *piPersistFile = NULL;
    TCHAR szLinkedFileName[MAX_PATH];
    TCHAR szArgument[MAX_PATH];

    ASSERT(cszShortcutFileName);
    ASSERT(cszShortcutFileName[0]);
    ASSERT(pstrRetLinkedFileName);
    ASSERT(pstrRetArgument);

    hRetCode = CoCreateInstance(
        CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&piShellLink
    );
    KGLOG_PROCESS_ERROR(hRetCode == S_OK);

    hRetCode = piShellLink->QueryInterface(IID_IPersistFile, (LPVOID *)&piPersistFile);
    KGLOG_PROCESS_ERROR(hRetCode == S_OK);

    hRetCode = piPersistFile->Load(cszShortcutFileName, STGM_READ);
    KGLOG_PROCESS_ERROR(hRetCode == S_OK);

    hRetCode = piShellLink->GetPath(szLinkedFileName, _countof(szLinkedFileName), NULL, SLGP_RAWPATH);
    KGLOG_PROCESS_ERROR(hRetCode == S_OK);

    *pstrRetLinkedFileName = szLinkedFileName;

    hRetCode = piShellLink->GetArguments(szArgument, _countof(szArgument));
    KGLOG_PROCESS_ERROR(hRetCode == S_OK);

    *pstrRetArgument = szArgument;
    
	nResult = true;
Exit0:
    KG_COM_RELEASE(piPersistFile);
    KG_COM_RELEASE(piShellLink);
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_SetShortcutLinkedFileName(
    const TCHAR cszShortcutFileName[], const TCHAR cszLinkedFileName[], const TCHAR cszArgument[]
)
{
	int nResult  = false;
    HRESULT hRetCode = S_FALSE;
    IShellLink *piShellLink = NULL;
    IPersistFile *piPersistFile = NULL;

    ASSERT(cszShortcutFileName);
    ASSERT(cszShortcutFileName[0]);
    ASSERT(cszLinkedFileName);
    ASSERT(cszLinkedFileName[0]);
    ASSERT(cszArgument);     
    // ASSERT(cszArgument[0]);  // Argument could be empty, which indicate no argument.

    hRetCode = CoCreateInstance(
        CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&piShellLink
    );
    KGLOG_PROCESS_ERROR(hRetCode == S_OK);

    hRetCode = piShellLink->QueryInterface(IID_IPersistFile, (LPVOID *)&piPersistFile);
    KGLOG_PROCESS_ERROR(hRetCode == S_OK);

    hRetCode = piShellLink->SetPath(cszLinkedFileName);
    KGLOG_PROCESS_ERROR(hRetCode == S_OK);

    if (cszArgument[0])
    {
        hRetCode = piShellLink->SetArguments(cszArgument);
        KGLOG_PROCESS_ERROR(hRetCode == S_OK);
    }

    // Remove read-only attribute.
    SetFileAttributes(cszShortcutFileName, FILE_ATTRIBUTE_NORMAL);

    hRetCode = piPersistFile->Save(cszShortcutFileName, false);
    KGLOG_PROCESS_ERROR(hRetCode == S_OK);

	nResult = true;
Exit0:
    KG_COM_RELEASE(piPersistFile);
    KG_COM_RELEASE(piShellLink);
	return nResult;
}

int KG_JX3MultiVersionSetupDlg::_WriteRegForNewVersion()
{
    int nResult  = false;
    int nRetCode = false;
    tstring strSubKey;
    tstring strVersionFileName;
    TCHAR szVersionNumber[128];
    HKEY hKey = NULL;
    BYTE *pbyData = NULL;
    DWORD dwDataSizeInBytes = 0;

    strSubKey  = tstring(_T("SOFTWARE\\Kingsoft\\JX3\\")) + m_strNewVersionType;

    nRetCode = RegCreateKeyEx(
        HKEY_LOCAL_MACHINE, strSubKey.c_str(), 0, NULL,
        REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL
    );
    KGLOG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

    pbyData = (BYTE *)m_strNewVersionPath.c_str();
    dwDataSizeInBytes = (DWORD)m_strNewVersionPath.length() * sizeof(m_strNewVersionPath[0]);
    nRetCode = RegSetValueEx(hKey, _T("installPath"), 0, REG_SZ, pbyData, dwDataSizeInBytes);
    KGLOG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

    strVersionFileName = m_strNewVersionPath + _T("version.cfg");
    nRetCode = GetPrivateProfileString(
        _T("Version"), _T("Sword3.version"), NULL, szVersionNumber, _countof(szVersionNumber),
        strVersionFileName.c_str()
    );
    KGLOG_PROCESS_ERROR(nRetCode && "Failed to load [Version]Sword3.version");

    pbyData = (BYTE *)szVersionNumber;
    dwDataSizeInBytes = (DWORD)_tcslen(szVersionNumber) * sizeof(szVersionNumber[0]);
    nRetCode = RegSetValueEx(hKey, _T("version"), 0, REG_SZ, pbyData, dwDataSizeInBytes);
    KGLOG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

    pbyData = (BYTE *)szVersionNumber;
    dwDataSizeInBytes = (DWORD)_tcslen(szVersionNumber) * sizeof(szVersionNumber[0]);
    nRetCode = RegSetValueEx(hKey, _T("setupVersion"), 0, REG_SZ, pbyData, dwDataSizeInBytes);
    KGLOG_PROCESS_ERROR(nRetCode == ERROR_SUCCESS);

    nResult = true;
Exit0:
    if (hKey)
    {
        RegCloseKey(hKey);
        hKey = NULL;
    }
    return nResult;
}

int KG_JX3MultiVersionSetupDlg::_LoadJX3Launcher()
{
    int nResult  = false;
    int nRetCode = false;
    tstring strLauncherFileName;
    STARTUPINFO StartupInfo;
    PROCESS_INFORMATION ProcessInfo = {0};

    strLauncherFileName = m_strBasedVersionPath + _T("JX3Launcher.exe");

    ZeroMemory(&StartupInfo, sizeof(StartupInfo));
    StartupInfo.cb = sizeof(StartupInfo);

    nRetCode = CreateProcess(
        strLauncherFileName.c_str(), NULL, NULL, NULL, false, 0, NULL, NULL,
        &StartupInfo, &ProcessInfo
    );
    KGLOG_PROCESS_ERROR(nRetCode && "CreateProcess");

    nResult = true;
Exit0:    
    if (ProcessInfo.hThread)
    {
        CloseHandle(ProcessInfo.hThread);
        ProcessInfo.hThread = NULL;
    }
    if (ProcessInfo.hProcess)
    {
        CloseHandle(ProcessInfo.hProcess);
        ProcessInfo.hProcess = NULL;
    }
    return nResult;
}
