// Test3DEngineMfcDlg.cpp : implementation file
//

#include "stdafx.h"
#include "Test3DEngineMfc.h"
#include "Test3DEngineMfcDlg.h"
#include "SceneEditorCommon.h"
#include "Functions.h"
#include "InputProxy.h"
#include "KG3DInterface.h"
#include "IEKG3DScene.h"
#include "IEKG3DSceneOutputWnd.h"
#include "IEKG3DTerrain.h"
#include "IEEditor.h"
#include "d3d9.h"
#include "d3dx9.h"
#include "File.h"
#include "IEKG3DModelTable.h"
#include "IEKG3DTextureTable.h"
#include "IEKG3DSFX.h"
#include "KG3DAnimationStruct.h"
#include "KG3DTextureTable.h"
#include "KSO3World.h"
#include "KG3DSFX.h"


// CTest3DEngine
#define EDITOR_CONFIG_FILE "Config.ini"

#define GETMODELEDITOR() \
	IEKG3DSceneModelEditor* pModelEditor = NULL;\
	m_p3DScenesVec[m_index]->GetSceneModelEditor(&pModelEditor);\
	_ASSERTE(pModelEditor); \
	if(!pModelEditor) return
#define GETSCENEBASE_MODEL() \
	IEKG3DSceneEditorBase* pBase = NULL;\
	m_p3DScenesVec[m_index]->GetSceneEditorBase(&pBase);\
	_ASSERTE(pBase); \
	if(!pBase) return

#define GETEDITOR() \
	IEKG3DSceneSceneEditor* pEditor = NULL;\
	m_p3DScenesVec[m_index]->GetSceneSceneEditor(&pEditor);\
	_ASSERTE(pEditor); \
	if(!pEditor) return
#define GETNPC() \
	IEKG3DRepresentNPC* pNpc = NULL;\
	pEditor->GetRepresentNPC(&pNpc);\
	_ASSERTE(pNpc);\
	if (!pNpc) return
#define GETSCENEBASE() \
	IEKG3DSceneEditorBase* pBase = NULL;\
	pEditor->GetSceneEditorBase(&pBase); \
	_ASSERTE(pBase);\
	if (!pBase) return
#define GETSCENE() \
	IEKG3DScene* pScene = m_p3DScenesVec[m_index];\
	_ASSERTE(pScene); \
	if(!pScene) return
#define GETSELECT() \
	IEKG3DSceneSelectBase* pSelector = NULL; \
	pEditor->GetSceneSelector(&pSelector); \
	_ASSERTE(pSelector); \
	if (!pSelector) \
	return
#define KG_COM_PROCESS_ERROR(Condition) \
	do  \
{   \
	if (FAILED(Condition))  \
	goto Exit0;         \
} while (false)
#define KG_PROCESS_ERROR(Condition) \
	do  \
{   \
	if (!(Condition))   \
	goto Exit0;     \
} while (false)



#ifdef _DEBUG
#define new DEBUG_NEW
#endif

extern IEKG3DEngineManager* g_pEngineManager;

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


// CTest3DEngineMfcDlg dialog




CTest3DEngineMfcDlg::CTest3DEngineMfcDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTest3DEngineMfcDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CTest3DEngineMfcDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CTest3DEngineMfcDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_WM_TIMER()
	ON_BN_CLICKED(IDOK, &CTest3DEngineMfcDlg::OnBnClickedOk)
END_MESSAGE_MAP()

IEKG3DRepresentObject* g_prepresentObj = NULL;
IEKG3DSceneModelEditor* g_pModelEditor = NULL;
IEKG3DScene* g_pScene = NULL;
IEKG3DSceneEditorBase* g_pBaseEditor = NULL;


HRESULT LoadPlayerModel(BSTR mdlfile)
{
	g_prepresentObj->SetModel(0, BSTR_TO_STRING(mdlfile), NULL, NULL, TRUE);
	IKG3DModel *pModel = NULL;
	g_prepresentObj->GetModel(pModel, 0);
	g_pScene->GetSceneModelEditor(&g_pModelEditor);
	g_pModelEditor->OpenModel(dynamic_cast<IEKG3DModel*>(pModel), FALSE);
	return S_OK;
}

HRESULT ToggleRotateModelEditor()
{
	g_pScene->GetSceneEditorBase(&g_pBaseEditor);\
	g_pBaseEditor->SetEditing(!g_pBaseEditor->GetEditing());
	return S_OK;
}

HRESULT FileToImage(BSTR src3DFileName, BSTR desImageFileName)
{
	USES_CONVERSION;	
	HRESULT hr = E_FAIL;

	DWORD width = 256;
	DWORD height = 256;
	DWORD bgcolor = 0x00ffffff;

	STRING str(BSTR_TO_STRING(src3DFileName));
	std::transform(str.begin(), str.end(), str.begin(), std::tolower);

	if (str.find(TEXT(".mdl")) != string::npos || str.find(TEXT(".mesh")) != string::npos)
	{
		LoadPlayerModel(src3DFileName);
		ToggleRotateModelEditor(); // 不旋转

		IKG3DModel* pPlayerModel = NULL;
		g_prepresentObj->GetModel(pPlayerModel, 0);
		if (!pPlayerModel)
		{
			STRING strErr(TEXT("【From atlax:】 该文件无法加载："));
			strErr += BSTR_TO_STRING(src3DFileName);
			strErr += TEXT("。\r\n");
			OutputDebugString(strErr.c_str());
			return S_OK;
		}
		IEKG3DModel* pModel = (IEKG3DModel*)pPlayerModel;

		STRING strOutputFile(BSTR_TO_STRING(desImageFileName));
		strOutputFile += TEXT("001.jpg");
		//strOutputFile += TEXT(".jpg");
		hr = pModel->RenderToPic(strOutputFile.c_str(), width, height, bgcolor);
	} 
	else if (str.find(TEXT(".ani")) != string::npos || str.find(TEXT(".tani")) != string::npos)
	{
		str = str.substr(0, str.find_last_of("\\/") + 1);
		CFileFind file;
		BOOL bContinue = file.FindFile(((STRING)(str + "*.mdl")).c_str()); // 就在同一个目录下的mdl

		if (!bContinue) // 父目录下的“模型”文件夹
		{
			str = str.substr(0, str.length() - 1);
			str = str.substr(0, str.find_last_of("\\/") + 1) + TEXT("模型\\");
			bContinue = file.FindFile(((STRING)(str + "*.mdl")).c_str());
		}
		if (!bContinue) // 父目录下的“部件/Mdl”文件夹（player）
		{
			str = str.substr(0, str.length() - 1);
			str = str.substr(0, str.find_last_of("\\/") + 1) + TEXT("部件\\Mdl\\");
			bContinue = file.FindFile(((STRING)(str + "*.mdl")).c_str());
		}
		if (bContinue)
		{
			file.FindNextFile();
			CString filename = file.GetFilePath();
			LoadPlayerModel(STRING_TO_BSTR(filename));
			ToggleRotateModelEditor(); // 不旋转

			IKG3DModel* pPlayerModel = NULL;
			g_prepresentObj->GetModel(pPlayerModel, 0);
			if (!pPlayerModel) return hr;
			IEKG3DModel* pModel = (IEKG3DModel*)pPlayerModel;

			hr = pModel->RenderAnimationToPicSerial(BSTR_TO_STRING(src3DFileName), BSTR_TO_STRING(desImageFileName),
				width, height, bgcolor);
		}
	}

	return S_OK;
}

// CTest3DEngineMfcDlg message handlers

BOOL CTest3DEngineMfcDlg::OnInitDialog()
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
	HWND hWnd = GetDlgItem(IDC_PREVIEW)->GetSafeHwnd();
	STRING strEnginePath("D:/Kingsoft/Game/sword3-products/trunk/client/");
	STRING strStartUpPath("C:/Documents and Settings/kuangsihao/桌面/Test3DEngine/");
	Init3DEngine(0, KG3DENG_NONE, hWnd, hWnd, STRING_TO_BSTR(strEnginePath.c_str()), STRING_TO_BSTR(strStartUpPath.c_str()));
	g_pEngineManager->NewOneScene(SCENETYPE_MODELEDITOR, (IKG3DScene**)&g_pScene);
	INT windowid = 0;
	g_pScene->AddOutputWindow("SceneMain", (HWND)hWnd, OUTPUTWND_MODELEDITOR_PREVIEW, 0, &windowid);
	g_pScene->SetCurrentOutputWindow(windowid);
	IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
	g_pScene->GetCurOutputWnd(&piSceneCurOutputWnd);
	if (piSceneCurOutputWnd)
		piSceneCurOutputWnd->InitCameraStation(EDITOR_CONFIG_FILE);
	g_pScene->GetSceneEditorBase(&g_pBaseEditor);
	CInputProxy* inputProxy = new CInputProxy();
	InitKeyState(g_pBaseEditor, hWnd, inputProxy);
	g_pEngineManager->NewKG3DRepresentObject(REPRESENTOBJECT_DEFAULT, 0, 0, &g_prepresentObj);

	//LoadPlayerModel(STRING_TO_BSTR("D:/Kingsoft/Game/sword3-products/trunk/client/data/source/NPC_source/E13003/模型/E13003.mdl"));


	SetTimer(1, 33, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CTest3DEngineMfcDlg::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == 1)
	{
		g_pEngineManager->FrameMove();
		g_pEngineManager->Render();
	}

	CDialog::OnTimer(nIDEvent);
}


void CTest3DEngineMfcDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CTest3DEngineMfcDlg::OnPaint()
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
HCURSOR CTest3DEngineMfcDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CTest3DEngineMfcDlg::OnBnClickedOk()
{
	FileToImage(STRING_TO_BSTR("D:/Kingsoft/Game/sword3-products/trunk/client/data/source/NPC_source/E13003/模型/E13003.mdl"), STRING_TO_BSTR("C:/E13003"));
	//OnOK();
}
