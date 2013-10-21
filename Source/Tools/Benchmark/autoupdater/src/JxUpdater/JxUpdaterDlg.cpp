// JxUpdaterDlg.cpp : implementation file
//

#include "stdafx.h"
#include "JxUpdater.h"
#include "JxUpdaterDlg.h"
#include "httpdownload.h"
#include "KGPublic.h"
#include "UpdateDefine.h"
#include <direct.h>
#include "UpdateExtractor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// KJxUpdaterDlg dialog

KJxUpdaterDlg::KJxUpdaterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(KJxUpdaterDlg::IDD, pParent)
{
	m_nVersion = 0;
	m_nNewVersion = 0;
	m_bNeedUpdate = false;

	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void KJxUpdaterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LOGO, m_CtrlLogo);
	DDX_Control(pDX, IDC_PROGRESS1, m_CtrlProgress);
	DDX_Control(pDX, IDC_BUTTON1, m_CtrlCheckUpdate);
	DDX_Control(pDX, IDC_BUTTON2, m_CtrlQuit);
	DDX_Control(pDX, IDC_STATUS, m_CtrlStatus);
	DDX_Control(pDX, IDC_XSJ_LOGO, m_CtrlXsjLogo);
}

BEGIN_MESSAGE_MAP(KJxUpdaterDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(MSG_HTTPDOWNLOAD_STATUS, &KJxUpdaterDlg::OnDownloaderMsg)
	ON_MESSAGE(WM_STARTUP_MSG, &KJxUpdaterDlg::OnStartUpMsg)
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BUTTON1, &KJxUpdaterDlg::OnBnClickedCheckUpdate)
	ON_BN_CLICKED(IDC_BUTTON2, &KJxUpdaterDlg::OnBnClickedQuit)
END_MESSAGE_MAP()


// KJxUpdaterDlg message handlers

BOOL KJxUpdaterDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	Startup();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void KJxUpdaterDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

HCURSOR KJxUpdaterDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

LRESULT KJxUpdaterDlg::OnDownloaderMsg(WPARAM wParam, LPARAM lParam)
{
	PHTTPDOWNLOADSTATUS pDownloadStatus = NULL;
	if (!lParam)
		return FALSE;

	pDownloadStatus = (PHTTPDOWNLOADSTATUS)lParam;

	if (pDownloadStatus->nStatusType == HTTP_STATUS_FILEDOWNLOADEDSIZE)
	{
		CString str;
		int nPos = (pDownloadStatus->dwFileDownloadedSize * 100) / pDownloadStatus->dwFileSize;
		m_CtrlProgress.SetPos(nPos);
		str.Format(MSG_DOWNLOADING, nPos);	
		m_CtrlStatus.SetWindowText(str);
	}

	return TRUE;
}

int KJxUpdaterDlg::DownloadFile(const char* szFilename, bool bNotify, bool bCovner)
{
	int nRetCode = false;
	int nResult  = false;
	CHttpDownload httper;

	KG_PROCESS_ERROR(szFilename);

	httper.SetTimeout(30000, 30000, 30000);
	httper.InitSocket(2, 2);
	if (bNotify)
		httper.SetNotifyWnd(m_hWnd, MSG_HTTPDOWNLOAD_STATUS);
	
	nRetCode = httper.Download(szFilename, m_szUpdatePath, bCovner);
	KG_PROCESS_ERROR(nRetCode == 0);

	httper.UnInitSocket();

	nResult = true;
Exit0:
	if (!nResult)
		httper.UnInitSocket();

	return nResult;	
}

int KJxUpdaterDlg::LoadConfig()
{
	int nRetCode = false;
	int nResult  = false;
	char szConfig[MAX_PATH];
	
	_sntprintf(szConfig, MAX_PATH, "%s%s", m_szAppPath, CONFIG_FILE_NAME);

 	nRetCode = GetPrivateProfileString(
 		CONFIG_SERVER,
 		CONFIG_SERVER_URL,
		"",
 		m_szServerUrl,
 		MAX_PATH,
 		szConfig
 		);
	KG_PROCESS_ERROR(nRetCode);
	
	m_nVersion = GetPrivateProfileInt(
		CONFIG_VERSION, 
		CONFIG_VERSION,
		0,
		szConfig
		);
	KG_PROCESS_ERROR(m_nVersion);

	nResult = true;
Exit0:
	return nResult;
}

void KJxUpdaterDlg::CheckUpdate()
{
	int nRetCode = false;
	int nResult  = false;

	::GetCurrentDirectory(MAX_PATH, m_szAppPath);
	
	_sntprintf(m_szUpdatePath, MAX_PATH, "%s%s%s", m_szAppPath, 
		CONFIG_PATH, "\\");

	nRetCode = CreateLogPath(m_szUpdatePath);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = ClearFile(m_szUpdatePath);
	KG_PROCESS_ERROR(nRetCode);
	InitParam();
// 	nRetCode = LoadConfig();
// 	KG_PROCESS_ERROR(nRetCode);
// 	nRetCode = CheckVersion();
// 	KG_PROCESS_ERROR(nRetCode);

	if (m_bNeedUpdate)
	{
		nRetCode = Update();
		KG_PROCESS_ERROR(nRetCode);
		m_CtrlStatus.SetWindowText(MSG_SUCCESS);
	}
	else
	{
		//m_CtrlStatus.SetWindowText(MSG_ISNEW);
		//OnBnClickedQuit();
		EndDialog(TRUE);
	}

	nResult = true;
Exit0:
	if (!nResult)
		m_CtrlStatus.SetWindowText(MSG_ERROR);
	return;
}

int KJxUpdaterDlg::CheckVersion()
{
	int nRetCode = false;
	int nResult  = false;
	int nTempVer = 0;
	char szConfig[MAX_PATH];
	char szUpdateFileName[MAX_PATH];

	_sntprintf(szUpdateFileName, MAX_PATH, "%s%s%s", m_szServerUrl, 
		SERVER_ROOT, SERVER_FILE_NAME);
	_sntprintf(szConfig, MAX_PATH, "%s%s", m_szUpdatePath, 
		SERVER_FILE_NAME);

	nRetCode = DownloadFile(szUpdateFileName, false, true);
	KG_PROCESS_ERROR(nRetCode);

	nTempVer = GetPrivateProfileInt(
		SERVER_VERSION, 
		SERVER_VERSION,
		0,
		szConfig
		);
	KG_PROCESS_ERROR(nTempVer);

	if (nTempVer > m_nVersion)
	{
		m_nNewVersion = nTempVer;
		m_bNeedUpdate = true;
	}
	
	_tremove(szConfig);
	nResult= true;
Exit0:
	return nResult;
}

int KJxUpdaterDlg::Update()
{
	int nRetCode  = false;
	int nResult   = false;
	int nStrLen   = 0;
	int nLocation = 0;
	CString strPack;
	//char szUpdateFileName[MAX_PATH];

	nRetCode = DownloadFile(m_szUpdateFileName);
	KG_PROCESS_ERROR(nRetCode);
	
	nLocation = strlen(m_szUpdateFileName);

	while (nLocation > 2)
	{
		if (
			m_szUpdateFileName[nLocation] == '\\' || 
			m_szUpdateFileName[nLocation] == '/'
		)
		{
			break;
		}
		nLocation--;
	}

	KG_PROCESS_ERROR(nLocation > 2);
	nLocation++; //skip '/'

	strPack.Format("%s%s", m_szUpdatePath, m_szUpdateFileName + nLocation);
	m_CtrlQuit.EnableWindow(false);
	nRetCode = UpdateExtract(strPack);
	m_CtrlQuit.EnableWindow(true);
	KG_PROCESS_ERROR(nRetCode);

	_tremove(strPack);
	nResult= true;
Exit0:
	return nResult;
}
void KJxUpdaterDlg::OnBnClickedCheckUpdate()
{
	int nRetCode = FALSE;
	STARTUPINFO StartInfo = {sizeof(STARTUPINFO)};
	PROCESS_INFORMATION ProcessInfo;
	CHAR szAppPath[MAX_PATH];
	CString strApp;

	//strApp = (m_szAppPath);
	//strApp += MAIN_EXE_NAME;
	strApp.Format("%s\\%s", m_szAppPath, MAIN_EXE_NAME);

	try
	{
		GetStartupInfo(&StartInfo);
	}
	catch (...)
	{
		goto Exit0;
	}

	nRetCode = CreateProcess(
		NULL,
		(LPTSTR)strApp.GetString(),
		NULL, 
		NULL,
		FALSE,
		NORMAL_PRIORITY_CLASS,
		NULL,
		NULL, 
		&StartInfo, 
		&ProcessInfo
		);
	KG_PROCESS_ERROR(nRetCode);

	CloseHandle(ProcessInfo.hThread);
	CloseHandle(ProcessInfo.hProcess);

Exit0:
	OnBnClickedQuit();
	return;
}

void KJxUpdaterDlg::OnBnClickedQuit()
{
	MSG msg;
	// TODO: Add your control notification handler code here
	if (!m_CtrlCheckUpdate.IsWindowEnabled())
	{
		if (MessageBox(MSG_QUIT, MSG_MSG, MB_OKCANCEL) == IDCANCEL)	
			goto Exit0;

	}

	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);            
	}
	EndDialog(IDOK);
Exit0:
	return;
}

//*********************Extract*************>>>
int KJxUpdaterDlg::UpdateExtract(const CString &strPack)
{
	int nRetCode        = FALSE;
	int nResult         = FALSE;
	int nTotalFile      = 0;
	int nUpdatedFile    = 0;
	int nExceptDirCount = 0;
	MSG msg;
	HExtract hExtract = NULL;
	CString strUpdateInfo;

	hExtract = UpdateExtractor_Init(strPack, m_szAppPath);
	KG_PROCESS_ERROR(hExtract);

	nTotalFile = UpdateExtractor_GetFileCount(hExtract);
	KG_PROCESS_ERROR(nRetCode <= 0);

	m_CtrlProgress.SetRange(0, nTotalFile);

	while (true)
	{
		nRetCode = UpdateExtractor_ExtractNext(hExtract);
		KG_PROCESS_SUCCESS(2 == nRetCode);   //2 is Success
		if (nRetCode == 0)
		{
			strUpdateInfo.Format(MSG_UPDATEING, (nUpdatedFile * 100) / nTotalFile);
			m_CtrlStatus.SetWindowText(strUpdateInfo);
			m_CtrlProgress.SetPos(nUpdatedFile);
			++nUpdatedFile;
		}

		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);            
		}
	}

Exit1:
	m_CtrlProgress.SetPos(nTotalFile);    
	nResult = TRUE;
Exit0:
	if (hExtract)
	{
		UpdateExtractor_Uninit(hExtract);
		hExtract = NULL;
	}

	return nResult;
}
//<<<******************Extract****************

int KJxUpdaterDlg::ClearFile(const CString &strPath)
{
	int nRetCode = FALSE;
	CString strPathName;
	CString strFullPathFile;
	CString strFile;
	WIN32_FIND_DATA FindFileData;
	HANDLE hFindFile  = NULL;

	strPathName = strPath + _T("*.*");

	hFindFile = FindFirstFile(strPathName.GetString(), &FindFileData);
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		while (true)
		{
			strFile = FindFileData.cFileName;
			if (
				(strFile != _T(".")) &&
				(strFile != _T(".."))
				)
			{
				strFullPathFile = strPath + strFile;
				if (!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
				{
					_tremove(strFullPathFile.GetString());
				}
				else
				{
					strFullPathFile += _T("\\");
					ClearFile(strFullPathFile);
				}
			}

			nRetCode = FindNextFile(hFindFile, &FindFileData);
			if (!nRetCode)
			{
				if (GetLastError() == ERROR_NO_MORE_FILES)
					break;
			}
		}
	}

	return TRUE;
}

int KJxUpdaterDlg::CreateLogPath(const char* szPath)
{
	int nRetCode = false;
	int nResult  = false;
	int i        = 3;
	int nPathLen = 0;
	char szTemp[MAX_PATH];

	KG_PROCESS_ERROR(szPath);

	nPathLen = strlen(szPath);
	while (i <= nPathLen)
	{
		for (i; i <= nPathLen; i++)
		{
			if (szPath[i] == '\\') 
			{
				strncpy(szTemp, szPath, i + 1);
				szTemp[i] = '\0';
				nRetCode = _mkdir(szTemp);
				if (errno == ENOENT)
					break;
			}
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

LRESULT KJxUpdaterDlg::OnStartUpMsg(WPARAM wParam, LPARAM lParam)
{
	// TODO: Add your control notification handler code here
	m_CtrlCheckUpdate.EnableWindow(false);
	CheckUpdate();
	m_CtrlCheckUpdate.EnableWindow(true);
	m_CtrlQuit.EnableWindow(true);

	return 0;
}

DWORD WINAPI KJxUpdaterDlg::ShowWebThreadFunc(LPVOID lpParam)
{
	KJxUpdaterDlg *pThis = reinterpret_cast<KJxUpdaterDlg*>(lpParam);
	ASSERT(pThis);

	if (pThis)
		pThis->_ShowWebThreadFunc();

	return 0;
}

INT KJxUpdaterDlg::_ShowWebThreadFunc()
{
	// TODO: Add your control notification handler code here
	m_CtrlCheckUpdate.EnableWindow(false);
	CheckUpdate();
	m_CtrlCheckUpdate.EnableWindow(true);

	return true;
}

void KJxUpdaterDlg::Startup()
{
	HANDLE hThread;
	DWORD dwThreadId = 0;
	m_BmpLogo.LoadBitmap(IDB_JX3LOGO);
	m_BmpXsjLogo.LoadBitmap(IDB_KS_XSJ_LOGO);
	m_CtrlXsjLogo.SetBitmap(m_BmpXsjLogo);
	m_CtrlLogo.SetBitmap(m_BmpLogo);

	hThread = ::CreateThread( 
		NULL,                       
		0,                          
		ShowWebThreadFunc,          
		this,               
		0,                          
		&dwThreadId);               

	if ( hThread )
	{
		::CloseHandle( hThread );
		hThread = NULL;
	}
}

void KJxUpdaterDlg::InitParam()
{
	if (theApp.m_lpCmdLine[0] == '\0')
		goto Exit0;

	strncpy_s(m_szUpdateFileName, theApp.m_lpCmdLine, MAX_PATH);
	m_szUpdateFileName[strlen(m_szUpdateFileName)] = '\0';
	m_bNeedUpdate = true;

Exit0:
	return;
}