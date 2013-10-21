#include "stdafx.h"
#include "SceneEditor.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "SceneEditorDoc.h"
#include "SceneEditorView.h"
#include "MeshEditorDoc.h"
#include "MeshEditorView.h"
#include "SwordTestDoc.h"
#include "SwordTestView.h"
#include "KObjectEditorDoc.h"
#include "KSceneObjectEditorView.h"
#include "KSceneObjectFrame.h"
#include "KSceneObjectEditorFrameWnd.h"
#include "KSceneModelEditorFrameWnd.h"
#include "KSceneSceneEditorFrameWnd.h"
#include "kg3Dtypes.h"
#include "KSceneSFXEditorDoc.h"
#include "KSceneSFXEditorFrame.h"
#include "KSceneSFXEditorView.h"
#include "KG3DSceneEditorDirectory.h"
#include "MaterialUniformToolsDlg.h"
#include "KgDlg_SpeedTree_converter.h"
#include "KDlgPlayerCheckTool.h"
#include "KGPhysicsParamEditorDlg.h"
#include "../KG3DEngine/KG3DEngine.h"		   //by dengzhihui Refactor 2006年11月6日
#include "KSO3World.h"
#include "SO3Represent/SO3RepresentInterface.h"
#include "SO3UI/SO3GameWorldUIHandler.h"
#include "SO3UI/ISO3UI.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "SO3World/IKGSO3WorldClient.h"
#include "KSceneDataFlowEditorDoc.h"
#include "KSceneDataFlowEditorView.h"
#include "KScneDataFlowEditorFrameWnd.h"
#include "KPvsEditorDoc.h"
#include "KPvsEditorFrame.h"
#include "KPvsEditorView.h"
#include "KSceneEditorAboutBox.h"
#include "KModelEditorBspGenerateDialog.h"
#include "KGNormalMapCreatorDlg.h"
#include "IKG3DSoundShell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const TCHAR * g_ModelAnimationKeywords[] = 
{
	"模型",
	"部件",
};


BOOL g_bUseSO3Logical = FALSE;

IKSO3RepresentHandler* g_pRepresentHandler = NULL;
IKG3DSoundShell *g_pSoundShell = NULL;
namespace
{
    CDllHolder g_ModuleRepresent;
	CDllHolder g_ModuleUI;

    IKSO3RepresentInterface* g_pRepresent;
	ISO3UI* g_pUI;

    void UnInitRepresent()
    {
        SAFE_RELEASE(g_pRepresent);
        g_ModuleRepresent.UnInit();
    }

    BOOL InitRepresent()
    {
        BOOL bRetCode = FALSE;
        fnCreateSO3Represent pfnCreateSO3Represent = NULL;

        UnInitRepresent();

#ifdef _DEBUG
        g_ModuleRepresent.Init("JX3RepresentD.dll");
#else
        g_ModuleRepresent.Init("JX3Represent.dll");
#endif

        pfnCreateSO3Represent = (fnCreateSO3Represent)GetProcAddress(g_ModuleRepresent.GetDllHandle(), FN_CREATE_SO3REPRESENT);
        KGLOG_PROCESS_ERROR(pfnCreateSO3Represent);

        g_pRepresent = pfnCreateSO3Represent();
        KGLOG_PROCESS_ERROR(g_pRepresent);

        return TRUE;
Exit0:
        return FALSE;
    }


    void UnInitUI()
    {
        if (g_pUI)
        {
            g_pUI->Exit();
            g_pUI = NULL;
        }
        g_ModuleUI.UnInit();
    }

    BOOL InitUI()
    {
        BOOL bRetCode = FALSE;
        fnCreateSO3UI pfnCreateSO3UI = NULL;

        KGLOG_PROCESS_ERROR(g_GetEngineIEInterface() && "init 3d engine first!");
        KGLOG_PROCESS_ERROR(g_pRepresent && "init represent first!");

        UnInitUI();

#ifdef _DEBUG
        g_ModuleUI.Init("JX3UID.dll");
#else
        g_ModuleUI.Init("JX3UI.dll");
#endif

        pfnCreateSO3UI = (fnCreateSO3UI)GetProcAddress(g_ModuleUI.GetDllHandle(), FN_CREATE_SO3UI);
        KGLOG_PROCESS_ERROR(pfnCreateSO3UI);

        g_pUI = pfnCreateSO3UI();
        KGLOG_PROCESS_ERROR(g_pUI);

#if 0
        Param.hInst = m_hInst;
        Param.hWnd = NULL;
        Param.fnAttribute = AttributeStringToID;
        Param.fnRequire = RequireStringToID;
        Param.p3DEngineManager = g_GetEngineIEInterface();
        Param.p3DModelManager = g_GetEngineIEInterface()->Get3DModelTable();
        Param.p3DUI = g_GetEngineIEInterface()->Get3DUI();
        Param.pRepresent = g_pRepresent->GetRepresentHandelr();
        g_pUI->Init(&Param);
#endif

        return TRUE;
Exit0:
        return FALSE;
    }

};


/////////////////////////////////////////////////////////////////////////////
// KSceneEditorApp
namespace MFCFramework
{

BEGIN_MESSAGE_MAP(KSceneEditorApp, CWinApp)
	//{{AFX_MSG_MAP(KSceneEditorApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//ON_COMMAND(ID_RESET, OnReset)
	//}}AFX_MSG_MAP
	// Standard file based document commands
ON_COMMAND(ID_PHYSISCSETTING, &KSceneEditorDoc::OnPhysiscsetting)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_NEWDATAFLOW, OnFileNewDataFlow)
	//ON_COMMAND(ID_FILE_NEW2, OnFileNew2)
    ON_COMMAND(ID_NEW_PVS, OnNewPvs)
//	ON_COMMAND(ID_MESHEDIT, OnMeshEdit)
	ON_COMMAND(ID_FILE_OPEN,CWinApp::OnFileOpen)
	ON_COMMAND(ID_PLAYCHECKTOOL, OnPlayCheckTool)
	ON_COMMAND(ID_NPCCHECKTOOL, OnNpcCheckTool)
    ON_COMMAND(ID_NORMALMAP_GEN, &OnNormalMapGen)
	ON_COMMAND(ID_BSP, OnBsp)
	ON_COMMAND(ID_MODELOPTIMIZE, OnModelOptimize)
	ON_COMMAND(ID_FILE_SAVE,OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS,OnFileSaveAs)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_BN_CLICKED(IDC_SWORDTEST, OnBnClickedSwordtest)
	ON_COMMAND(ID_FILE_NEW_SFX, OnFileNewSfx)
	ON_COMMAND(ID_MATERIALTOOL, OnMaterialTool)
	ON_COMMAND(ID_AMBIENT_ONLY, OnAmbientOnly)
	ON_COMMAND(ID_SPEEDTREEVERSIONCONVERT, OnSpeedTreeConvert)
	ON_COMMAND(ID_TEST, KSceneEditorApp::OnSomeTest)
    ON_COMMAND(ID_D3D9_DEBUG, KSceneEditorApp::OnShowD3D9DebugTool)
	ON_COMMAND(ID_REFRESHSHADER,&KSceneEditorView::OnRefreshshader)
   //{ WM_ACTIVATEAPP, 0, 0, 0, AfxSig_vww, \
   //(AFX_PMSG)(AFX_PMSGW) \
   //(static_cast< void (AFX_MSG_CALL CWnd::*)(BOOL, DWORD) > ( &MFCFramework::KSceneEditorApp::OnActivateApp)) },

END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneEditorApp construction

KSceneEditorApp::KSceneEditorApp()
{ 
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_pdlgCheckTool = NULL;	
	m_hDummperModule = NULL;
    m_bMemoryInitFlag    = false;
    m_bLogInitFlag = false;
    m_bGameWorldOpenFlag = false;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only KSceneEditorApp object

KSceneEditorApp theApp;
/////////////////////////////////////////////////////////////////////////////
// KSceneEditorApp initialization

BOOL KSceneEditorApp::InitInstance()
{
    BOOL                        bResult             = false;
	BOOL                        bRetCode            = false;
	HRESULT                     hr                  = E_FAIL;
    char*                       pszRetCWD           = NULL;
    IRecorderFactory*           piRecorderFactory   = NULL;
    CMultiDocTemplate*          pDocTemplate        = NULL;
    MFCFramework::CMainFrame*   pMainFrame    = NULL;
    KGLOG_PARAM                 LogParam;
    char                        szCWD[MAX_PATH];
    CCommandLineInfo            cmdInfo;

    //使用Dummper
    m_hDummperModule = LoadLibrary(_T("dumper.dll"));

    pszRetCWD = getcwd(szCWD, sizeof(szCWD));
    KGLOG_PROCESS_ERROR(pszRetCWD);

	g_SetRootPath();

    g_SetFindFileMode(TRUE);
    g_SetFilePath("");
    g_LoadPackageFiles("config.ini", "PackFile");

	memset(&LogParam, 0, sizeof(LogParam));
	snprintf(LogParam.szPath,sizeof(LogParam.szPath),"%s\\logs",szCWD);
	LogParam.szPath[sizeof(LogParam.szPath) - 1] = '\0';

	strcpy(LogParam.szIdent, "SceneEditor");
	LogParam.nMaxLineEachFile = 65536;
	LogParam.Options = (KGLOG_OPTIONS)KGLOG_OPTION_FILE;

	bRetCode = KGLogInit(LogParam, NULL);
    KGLOG_PROCESS_ERROR(bRetCode);
    m_bLogInitFlag = true;

    bRetCode = KMemory::Initialize("SceneEditor.memory");
    KGLOG_PROCESS_ERROR(bRetCode);
    m_bMemoryInitFlag = true;

	g_cEditorDirectory.Load();

	{
		TCHAR AppDir[256] = {0};
		GetCurrentDirectory(256, AppDir);
		wsprintf(g_szDefWorkDirectory,"%s\\",AppDir);
		wsprintf(g_szDefExeDirectory,"%s\\",AppDir);
	}

	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	// Change the registry key under which our settings are stored.
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	
	pDocTemplate = new CMultiDocTemplate(
		IDR_SCENEETYPE,
		RUNTIME_CLASS(KSceneEditorDoc),
		RUNTIME_CLASS(CChildFrame), // custom MDI child frame
		RUNTIME_CLASS(KSceneEditorView));
	pDocTemplate->SetContainerInfo(IDR_SCENEETYPE_CNTR_IP);
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_MESHETYPE,
		RUNTIME_CLASS(KSceneModelEditorDoc),
		RUNTIME_CLASS(KSceneModelEditorFrameWnd), // custom MDI child frame
		RUNTIME_CLASS(KSceneModelEditorView));
	pDocTemplate->SetContainerInfo(IDR_SCENEETYPE_CNTR_IP);
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDS_OBJECTTYPE,
		RUNTIME_CLASS(KSceneObjectEditorDoc),
		RUNTIME_CLASS(KSceneObjectEditorFrameWnd), // custom MDI child frame
		RUNTIME_CLASS(KSceneObjectEditorView));
	pDocTemplate->SetContainerInfo(IDR_SCENEETYPE_CNTR_IP);
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_SFXEDITOR,
		RUNTIME_CLASS(KSceneSFXEditorDoc),
		RUNTIME_CLASS(KSceneSFXEditorFrame), // custom MDI child frame
		RUNTIME_CLASS(KSceneSFXEditorView));
	pDocTemplate->SetContainerInfo(IDR_SFXEDITOR);
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_MODELTYPE,
		RUNTIME_CLASS(KSceneModelEditorDoc),
		RUNTIME_CLASS(KSceneModelEditorFrameWnd), // custom MDI child frame
		RUNTIME_CLASS(KSceneModelEditorView));
	pDocTemplate->SetContainerInfo(IDR_SCENEETYPE_CNTR_IP);
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDS_DATAFLOWFILE,
		RUNTIME_CLASS(KSceneDataFlowEditorDoc),
		RUNTIME_CLASS(KScneDataFlowEditorFrameWnd), // custom MDI child frame
		RUNTIME_CLASS(KSceneDataFlowEditorView));
	pDocTemplate->SetContainerInfo(IDR_SCENEETYPE_CNTR_IP);
	AddDocTemplate(pDocTemplate);

    pDocTemplate = new CMultiDocTemplate(
        IDR_PVSTYPE,
        RUNTIME_CLASS(KPvsEditorDoc),
        RUNTIME_CLASS(KPvsEditorFrame), // custom MDI child frame
        RUNTIME_CLASS(KPvsEditorView));
    pDocTemplate->SetContainerInfo(IDR_PVSTYPE);
    AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_MODELTYPE2,
		RUNTIME_CLASS(KSceneModelEditorDoc),
		RUNTIME_CLASS(KSceneModelEditorFrameWnd), // custom MDI child frame
		RUNTIME_CLASS(KSceneModelEditorView));
	pDocTemplate->SetContainerInfo(IDR_SCENEETYPE_CNTR_IP);
	AddDocTemplate(pDocTemplate);
	pDocTemplate = new CMultiDocTemplate(
		IDR_SPEEDTREETYPE,
		RUNTIME_CLASS(KSceneModelEditorDoc),
		RUNTIME_CLASS(KSceneModelEditorFrameWnd), // custom MDI child frame
		RUNTIME_CLASS(KSceneModelEditorView));
	pDocTemplate->SetContainerInfo(IDR_SCENEETYPE_CNTR_IP);
	AddDocTemplate(pDocTemplate);

	pDocTemplate = new CMultiDocTemplate(
		IDR_MODELTYPE3,
		RUNTIME_CLASS(KSceneModelEditorDoc),
		RUNTIME_CLASS(KSceneModelEditorFrameWnd), // custom MDI child frame
		RUNTIME_CLASS(KSceneModelEditorView));
	pDocTemplate->SetContainerInfo(IDR_SCENEETYPE_CNTR_IP);
	AddDocTemplate(pDocTemplate);

	// create main MDI Frame window
	pMainFrame = new MFCFramework::CMainFrame;
    bRetCode = pMainFrame->LoadFrame(IDR_SCENEETYPE);
    KGLOG_PROCESS_ERROR(bRetCode);

    m_pMainWnd = pMainFrame;


	//(by dengzhihui 2006年12月7日 15:48:16
	extern BYTE* g_SceneEditorKeyState;//键盘状态 
	ZeroMemory(g_SceneEditorKeyState,sizeof(BYTE)*256);
	//)

	// Parse command line for standard shell commands, DDE, file open
	
	ParseCommandLine(cmdInfo);

	// Dispatch commands specified on the command line
	//if (!ProcessShellCommand(cmdInfo))
	//	return FALSE;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(SW_MAXIMIZE); 
	pMainFrame->UpdateWindow();
    
    g_pEngineManager->GetUseSO3Logical(&g_bUseSO3Logical);
	if (g_bUseSO3Logical)
	{
        piRecorderFactory = CreateRecorderFactoryInterface(ermNormal);
        KGLOG_PROCESS_ERROR(piRecorderFactory);

        g_pSO3World = new KSO3World;
        KGLOG_PROCESS_ERROR(g_pSO3World);
        
        bRetCode = g_pSO3World->Init(piRecorderFactory);
        KGLOG_PROCESS_ERROR(bRetCode);
        m_bGameWorldOpenFlag = true;

        if (InitRepresent() && InitUI())
        {
            HWND hWnd = NULL;

            g_SetGameWorldUIHandler(&g_pUI->GetGameWorldUIHandler());
            g_SetGameWorldRepresentHandler(g_pRepresent->GetGameWorldRepresentHandler());
            g_pRepresentHandler = g_pRepresent->GetRepresentHandelr();

            g_pRepresent->GetRepresentHandelr()->AttachGameWorld(g_pSO3World, QuerySO3WorldClientInterface());
            g_pRepresent->GetRepresentHandelr()->Attach3DEngine(g_GetEngineIEInterface(), g_pEngineManager->Get3DModelTable(), g_pEngineManager->Get3DUI());
            g_pRepresent->GetRepresentHandelr()->AttachUI(g_pUI, &g_pUI->GetGameWorldUIHandler());

            g_pEngineManager->GetBaseWnd(&hWnd);
            g_pRepresent->GetRepresentHandelr()->InitHWND(hWnd);
            if (!g_pRepresent->Init())
            {
                ::MessageBox(NULL, "表现逻辑初始化失败！", NULL, 0);
            }
        }
	}

	//创建声音
	g_pSoundShell = Create3DSoundShell();
	if (!g_pSoundShell)
	{
		KGLogPrintf(KGLOG_WARNING, "声音创建失败.");
	}
	else
	{
		g_pSoundShell->Init(m_pMainWnd->GetSafeHwnd());
	}

	//在这里把需要使用sound的地方都统一设置上soundshell
	g_pEngineManager->SetSoundShell(g_pSoundShell);
	if (g_pRepresent)
	{
		g_pRepresent->SetSoundShell(g_pSoundShell);
	}

    bResult = true;
Exit0:
    if (!bResult)
    {
        if (m_bGameWorldOpenFlag)
        {
            g_pSO3World->UnInit();
            m_bGameWorldOpenFlag = false;
        }

        KG_DELETE(g_pSO3World);

        if (m_bMemoryInitFlag)
        {
            KMemory::Finalize();
            m_bMemoryInitFlag = false;
        }

        if (m_bLogInitFlag)
        {
            KGLogUnInit(NULL);
            m_bLogInitFlag = false;
        }
    }
    KG_COM_RELEASE(piRecorderFactory);
	return bResult;
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void KSceneEditorApp::OnAppAbout()
{
	KSceneEditorAboutBox aboutDlg;
	aboutDlg.DoModal();
}

void KSceneEditorApp::OnNewPvs()
{
    POSITION Pos = m_pDocManager->GetFirstDocTemplatePosition();
    CDocTemplate* pTemplate = NULL;

    while (Pos != NULL)
    { 
        pTemplate = (CDocTemplate*)MFCFramework::theApp.GetNextDocTemplate(Pos);

        CString strDocName;

        pTemplate->GetDocString(strDocName, CDocTemplate::docName);
        if (strDocName == _T("Pvs编辑"))
        {
            break;
        }
    }

    if (!pTemplate)
        return;

    pTemplate->OpenDocumentFile(NULL);

}

void KSceneEditorApp::OnFileNew(void)
{
	POSITION Pos = m_pDocManager->GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = NULL;
	
	while (Pos != NULL)
	{ 
		pTemplate = (CDocTemplate*)MFCFramework::theApp.GetNextDocTemplate(Pos);

		CString strDocName;

		pTemplate->GetDocString(strDocName, CDocTemplate::docName);
		if (strDocName == _T("场景编辑"))
		{
			break;
		}
	}

	ASSERT(pTemplate != NULL);
	ASSERT_KINDOF(CDocTemplate, pTemplate);

	CDocument* pOrgDoc = pTemplate->OpenDocumentFile(NULL);
	ASSERT_KINDOF(KSceneEditorDoc, pOrgDoc);
	KSceneEditorDoc* pDoc = (KSceneEditorDoc *)pOrgDoc;
	if (pDoc)
	{
		/*KG3DScene* pScene = NULL;
		g_pEngineManager->NewOneScene(SCENETYPE_SCENEEDITOR,(IKG3DScene**)&pScene);
		if(!pScene)
		{
			MessageBox(NULL,"创建场景失败","失败",0);
			return;
		}
		pScene->LoadFromFile("",0,0);
		g_pEngineManager->SetFocusScene(pScene);
		pDoc->SetScene(pScene);*/
	}
	//CWinApp::OnFileNew();
	//this->m_pDocManager->GetOpenDocumentCount();
}

/////////////////////////////////////////////////////////////////////////////
// KSceneEditorApp message handlers
void KSceneEditorApp::OnFileNewDataFlow(void)
{
	POSITION Pos = m_pDocManager->GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = NULL;

	while (Pos != NULL)
	{ 
		pTemplate = (CDocTemplate*)MFCFramework::theApp.GetNextDocTemplate(Pos);

		CString strDocName;

		pTemplate->GetDocString(strDocName, CDocTemplate::docName);
		if (strDocName == _T("数据流编辑"))
		{
			break;
		}
	}

	ASSERT(pTemplate != NULL);
	ASSERT_KINDOF(CDocTemplate, pTemplate);

	CDocument* pOrgDoc = pTemplate->OpenDocumentFile(NULL);
	ASSERT_KINDOF(KSceneDataFlowEditorDoc, pOrgDoc);
	KSceneDataFlowEditorDoc* pDoc = (KSceneDataFlowEditorDoc *)pOrgDoc;
	if (pDoc)
	{
	}
}
//void KSceneEditorApp::OnFileNew2(void)
//{
//	POSITION Pos = m_pDocManager->GetFirstDocTemplatePosition();
//	CDocTemplate* pTemplate = NULL;
//	while (Pos != NULL)
//	{
//		pTemplate = (CDocTemplate*)MFCFramework::theApp.GetNextDocTemplate(Pos);
//
//		CString strDocName;
//
//		pTemplate->GetDocString(strDocName, CDocTemplate::docName);
//		if (strDocName == _T("模型编辑"))
//		{
//			break;
//		}
//	}
////	CDocTemplate* pTemplate = (CDocTemplate*)m_pDocManager->GetNextDocTemplate(Pos);
////	pTemplate = (CDocTemplate*)m_pDocManager->GetNextDocTemplate(Pos);
//
//	ASSERT(pTemplate != NULL);
//	ASSERT_KINDOF(CDocTemplate, pTemplate);
//	CDocument* pOrgDoc = pTemplate->OpenDocumentFile(NULL);
//	ASSERT_KINDOF(KSceneModelEditorDoc, pOrgDoc);
//	KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc *)pOrgDoc;
//	if (pDoc)
//	{
//		MFCFramework::CMainFrame* pMainFrame = (MFCFramework::CMainFrame*) m_pMainWnd;
//		IEKG3DScene* pScene = NULL;
//		g_pEngineManager->NewOneScene(SCENETYPE_MODELEDITOR,(IKG3DScene**)&pScene);//by dengzhihui Refactor 2006年11月6日
//		pDoc->SetScene(pScene);
//		g_pEngineManager->SetFocusScene(pScene);//by dengzhihui Refactor 2006年11月6日
//
//	}
//	//CWinApp::OnFileNew();
//	//this->m_pDocManager->GetOpenDocumentCount();
//}

//void KSceneEditorApp::OnFileNewObject(void)
//{
//	POSITION Pos = m_pDocManager->GetFirstDocTemplatePosition();
//
//	CDocTemplate* pTemplate = NULL;
//	while (Pos != NULL)
//	{
//		pTemplate = (CDocTemplate*)MFCFramework::theApp.GetNextDocTemplate(Pos);
//
//		CString strDocName;
//
//		pTemplate->GetDocString(strDocName, CDocTemplate::docName);
//		if (strDocName == _T("物件编辑"))
//		{
//			break;
//		}
//	}
//	ASSERT(pTemplate != NULL);
//	ASSERT_KINDOF(CDocTemplate, pTemplate);
//
//	CDocument* pOrgDoc = pTemplate->OpenDocumentFile(NULL);
//	ASSERT_KINDOF(KSceneObjectEditorDoc, pOrgDoc);
//	KSceneObjectEditorDoc* pDoc = (KSceneObjectEditorDoc*)pOrgDoc;
//	if (pDoc)
//	{
//		MFCFramework::CMainFrame* pMainFrame = (MFCFramework::CMainFrame*) m_pMainWnd;
//		IEKG3DScene* pScene = NULL;
//		g_pEngineManager->NewOneScene(SCENETYPE_OBJECTEDITOR,(IKG3DScene**)&pScene);//by dengzhihui Refactor 2006年11月6日
//		pDoc->SetScene(pScene);
//		g_pEngineManager->SetFocusScene(pScene);//by dengzhihui Refactor 2006年11月6日
//
//		//KG3DSceneObjectEditor* pEditor = static_cast<KG3DSceneObjectEditor*>(pScene);
//		//pEditor->OpenObject("F_TM_唐门牌楼01.Mesh");
//
//		//pSceneObjectEditor->OnNewObject();
//	}
//}

void KSceneEditorApp::OnFileOpen(void)
{
	// TODO: Add your command handler code here
	char bigBuff[800] = "";  // maximum common dialog buffer size
	char szFilter[] ="Map Files (*.Map)|*.Map|Mesh Files (*.Mesh)|*.Mesh|";


	{
		//CTH 2008-01-22 1727 打包据说要这两句话
		g_SetRootPath();
		g_SetFilePath("");
	}

	CFileDialog dlg(true, NULL, NULL,
		OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, szFilter);
	// Modify OPENFILENAME members directly to point to bigBuff
	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);// Last modify by Wu Caizhong,
	if (dlg.DoModal() == IDOK)
	{
		POSITION ps = dlg.GetStartPosition(); 
		while(ps != NULL)
		{
			CString Vs = dlg.GetNextPathName(ps);
			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];

			wsprintf(path_buffer,"%s",Vs);
			_splitpath( path_buffer, drive, dir, fname, ext );

			CString FileType;
			FileType.Format("%s",ext);
			DWORD NewXMeshID = 0;

			TCHAR filename[256];
			wsprintf(filename,"%s",Vs);

			if ((FileType==".Map")||(FileType==".map"))
			{
				POSITION Pos = GetFirstDocTemplatePosition();
				CDocTemplate* pTemplate = (CDocTemplate*)GetNextDocTemplate(Pos);

				ASSERT(pTemplate != NULL);
				ASSERT_KINDOF(CDocTemplate, pTemplate);
				KSceneEditorDoc* pDoc = (KSceneEditorDoc*) pTemplate->OpenDocumentFile(filename);
				if (pDoc)
				{
					MFCFramework::CMainFrame* pMainFrame = (MFCFramework::CMainFrame*) m_pMainWnd;
					IEKG3DScene* pScene = NULL;
					g_pEngineManager->NewOneScene(SCENETYPE_SCENEEDITOR,(IKG3DScene**)&pScene);//by dengzhihui Refactor 2006年11月6日
					pDoc->SetScene(pScene);
					g_pEngineManager->SetFocusScene(pScene);//by dengzhihui Refactor 2006年11月6日
				
					//KSceneSceneEdit* pSceneEditor = (KSceneSceneEdit*) pScene;
					//pSceneEditor->LoadScene(filename);
				}
			}
			else
			{
				//MessageBox("UnKnown FileType","SceneEditor",0);
			}
			
		}
	}
}

void KSceneEditorApp::OnFileSave(void)
{
	// TODO: Add your command handler code here
	char bigBuff[800] = "";  // maximum common dialog buffer size
	char szFilter[] ="Map Files (*.Map)|*.Map|";

	CFileDialog dlg(true, NULL, NULL,
		OFN_HIDEREADONLY , szFilter);
	// Modify OPENFILENAME members directly to point to bigBuff
	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	if (dlg.DoModal() == IDOK)
	{
		POSITION ps = dlg.GetStartPosition(); 
		while(ps != NULL)
		{
			CString Vs = dlg.GetNextPathName(ps);
			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];

			wsprintf(path_buffer,"%s",Vs);
			_splitpath( path_buffer, drive, dir, fname, ext );

			CString FileType;
			FileType.Format("%s",ext);
			DWORD NewXMeshID = 0;

			TCHAR filename[256];
			wsprintf(filename,"%s",Vs);

			if ((FileType==".Map")||(FileType==".map"))
			{
				POSITION Pos = m_pDocManager->GetFirstDocTemplatePosition();
				CDocTemplate* pTemplate = (CDocTemplate*)m_pDocManager->GetNextDocTemplate(Pos);

				ASSERT(pTemplate != NULL);
				ASSERT_KINDOF(CDocTemplate, pTemplate);
				KSceneEditorDoc* pDoc = (KSceneEditorDoc*) pTemplate->OpenDocumentFile(NULL);
				if (pDoc)
				{
					MFCFramework::CMainFrame* pMainFrame = (MFCFramework::CMainFrame*) m_pMainWnd;
/*					LPSCENE pScene = NULL;
					g_pEngineManager->NewOneScene(SCENETYPE_SCENEEDITOR,(IKG3DScene**)&pScene);
					pDoc->SetScene((KSceneSceneEdit*)pScene);
					KSceneSceneEdit* pSceneEditor = (KSceneSceneEdit*) pScene;
					KModelTerrain* pMap = (KModelTerrain*) pSceneEditor->m_lpMapMesh;
					*///pMap->LoadMap(filename);
				}
			}
			else
			{
				//MessageBox("UnKnown FileType","SceneEditor",0);
			}

		}
	}
}

BOOL KSceneEditorApp::OnIdle(LONG lCount)
{
	CWinApp::OnIdle(lCount);

	g_pEngineManager->FrameMove();
	g_pEngineManager->Render();

	MFCFramework::CMainFrame* pMainFrame = (MFCFramework::CMainFrame*) m_pMainWnd;

	if( pMainFrame->m_bActive )
	{
		extern BYTE* g_SceneEditorKeyState;
		g_SceneEditorKeyState[VK_LBUTTON] = !!(GetAsyncKeyState(VK_LBUTTON) & 0x8000);
		g_SceneEditorKeyState[VK_MBUTTON] = !!(GetAsyncKeyState(VK_MBUTTON) & 0x8000);
		g_SceneEditorKeyState[VK_RBUTTON] = !!(GetAsyncKeyState(VK_RBUTTON) & 0x8000);
		g_SceneEditorKeyState[VK_MENU]    = !!(GetAsyncKeyState(VK_MENU) & 0x8000);
		g_SceneEditorKeyState[VK_CONTROL] = !!(GetAsyncKeyState(VK_CONTROL) & 0x8000);
		g_SceneEditorKeyState[VK_SHIFT]   = !!(GetAsyncKeyState(VK_SHIFT) & 0x8000);

		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void KSceneEditorApp::OnFileSaveAs(void)
{
	// TODO: Add your command handler code here
	char bigBuff[800] = "";  // maximum common dialog buffer size
	char szFilter[] ="Map Files (*.Map)|*.Map|";

	CFileDialog dlg(false, "Map", NULL, 
		OFN_HIDEREADONLY , szFilter);
	// Modify OPENFILENAME members directly to point to bigBuff
	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	if (	dlg.DoModal() == IDOK)
	{
		POSITION ps = dlg.GetStartPosition(); 
		while(ps != NULL)
		{
			CString Vs = dlg.GetNextPathName(ps);
			TCHAR path_buffer[256];
			TCHAR drive[256];
			TCHAR dir[256];
			TCHAR fname[256];
			TCHAR ext[256];

			wsprintf(path_buffer,"%s",Vs);
			_splitpath( path_buffer, drive, dir, fname, ext );

			CString FileType;
			FileType.Format("%s",ext);
			DWORD NewXMeshID = 0;

			TCHAR filename[256];
			wsprintf(filename,"%s",Vs);

			if ((FileType==".Map")||(FileType==".map"))
			{
				POSITION Pos = m_pDocManager->GetFirstDocTemplatePosition();
				CDocTemplate* pTemplate = (CDocTemplate*)m_pDocManager->GetNextDocTemplate(Pos);

				ASSERT(pTemplate != NULL);
				ASSERT_KINDOF(CDocTemplate, pTemplate);
				KSceneEditorDoc* pDoc = (KSceneEditorDoc*) pTemplate->OpenDocumentFile(NULL);
				if (pDoc)
				{
					MFCFramework::CMainFrame* pMainFrame = (MFCFramework::CMainFrame*) m_pMainWnd;
/*					LPSCENE pScene = NULL;
					g_pEngineManager->NewOneScene(SCENETYPE_SCENEEDITOR,(IKG3DScene**)&pScene);
					pDoc->SetScene((KSceneSceneEdit*)pScene);
					KSceneSceneEdit* pSceneEditor = (KSceneSceneEdit*) pScene;
					KModelTerrain* pMap = (KModelTerrain*) pSceneEditor->m_lpMapMesh;
					*///pMap->LoadMap(filename);
				}
			}
			else
			{
				//MessageBox("UnKnown FileType","SceneEditor",0);
			}
		}
	}
}

void KSceneEditorApp::OnBnClickedSwordtest()
{
	// TODO: 在此添加控件通知处理程序代码
	POSITION Pos = GetFirstDocTemplatePosition();
	CDocTemplate* pTemplate = NULL;
	KSwordTestDoc* pDoc = NULL;
	CString str = "外传测试";
	CString TemplateStr;
	while(Pos)
	{
		pTemplate = (CDocTemplate*) GetNextDocTemplate(Pos);
		ASSERT(pTemplate != NULL);
		ASSERT_KINDOF(CDocTemplate, pTemplate);
		pTemplate->GetDocString(TemplateStr,CDocTemplate::docName);
		if (TemplateStr==str)
			break;
	}

	MFCFramework::CMainFrame* pMainFrame = (MFCFramework::CMainFrame*) m_pMainWnd;
	//LPSCENE pScene = NULL;
	//g_pEngineManager->NewOneScene(SCENETYPE_SWORDTEST,(IKG3DScene**)&pScene);

	pDoc = (KSwordTestDoc*) pTemplate->OpenDocumentFile(NULL);
	if (pDoc)
	{
		//pDoc->SetScene((KSceneSwordTest*)pScene);
		pDoc->SendInitialUpdate();
	}
}

void KSceneEditorApp::OnReset() 
{
	// TODO: Add your command handler code here
	if(m_pDocManager)
	{
		m_pDocManager->SaveAllModified();
		m_pDocManager->CloseAllDocuments(TRUE);

		MFCFramework::CMainFrame* pMainFrame = (MFCFramework::CMainFrame*) m_pMainWnd;
		g_pEngineManager->Reset();//by dengzhihui Refactor 2006年11月6日
	}


}

int KSceneEditorApp::ExitInstance() // return app exit code
{
	BOOL bRetCode = FALSE;
    BOOL bUseSO3Logical = FALSE;
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

	g_cEditorDirectory.Save();

    if(pManager)
		pManager->GetUseSO3Logical(&bUseSO3Logical);

	if(bUseSO3Logical)
	{
        if (m_bGameWorldOpenFlag)
        {
            g_pSO3World->UnInit();
            m_bGameWorldOpenFlag = false;
        }

        UnInitUI();
        UnInitRepresent();
        g_SetGameWorldUIHandler(NULL);
        g_SetGameWorldRepresentHandler(NULL);
	}

	if (g_pSoundShell)
	{
		SAFE_RELEASE(g_pSoundShell);
	}
    g_ClearPackageFiles();

    KG_DELETE(g_pSO3World);

Exit0:

	int nRet = CWinApp::ExitInstance();
	
	extern void UnInit3DEngine();
	UnInit3DEngine();

    if (m_bMemoryInitFlag)
    {
        KMemory::Finalize();
        m_bMemoryInitFlag = false;
    }

    if (m_bLogInitFlag)
    {
        KGLogUnInit(NULL);
        m_bLogInitFlag = false;
    }

	if (m_hDummperModule)
	{
		FreeLibrary(m_hDummperModule);
		m_hDummperModule = NULL;
	}
	return nRet;
}
};

void MFCFramework::KSceneEditorApp::OnShowD3D9DebugTool()
{
    char  szToolPath[MAX_PATH];
    g_GetFullPath(szToolPath, "");

    char* psz = NULL;
    
    if (szToolPath[strlen(szToolPath) - 1] == '\\')
    {
        psz = strrchr(szToolPath, '\\');

        if (!psz)
            return;

        *psz = 0;
    }

    psz = strrchr(szToolPath, '\\');

    if (!psz)
        return;

    *psz = 0;
    sprintf(szToolPath, "%s\\tools\\d3d9_debug_tools\\Perky.exe %x", szToolPath, GetCurrentProcessId());
    

    UINT u = WinExec(szToolPath, SW_NORMAL);
    if (u <= 31)
    {
        AfxMessageBox("找不到Perky.exe, 请更新 svm : \\trunk\\tools\\");
    }

}



void MFCFramework::KSceneEditorApp::OnSomeTest()
{
    TCHAR szModelName[MAX_PATH];
    TCHAR szBindName[MAX_PATH];

    g_GetFullPath(szModelName, TEXT("data\\source\\NPC\\E16003\\E16003.mdl"));
    g_GetFullPath(szBindName, TEXT("data\\test.bind"));
    
    IKG3DModel* pModel = NULL;
    IKG3DModel* pBind  = NULL;

    g_GetModelManager()->LoadResourceFromFile(szModelName, 0, 0, (IKG3DResourceBase**)&pModel);
    g_GetModelManager()->LoadResourceFromFile(szBindName,  0, 0, (IKG3DResourceBase**)&pBind);
    
    ((MFCFramework::KSceneEditorApp*)AfxGetApp())->NewSFXEditorToModifyBindInfo(
        pModel, 
        pBind, 
        0, 
        NULL, 
        CString(""),
        g_pGraphicsTool->GetNowTime());
    SAFE_RELEASE(pModel);
    SAFE_RELEASE(pBind);
}

void MFCFramework::KSceneEditorApp::NewSFXEditor(LPCTSTR szModelName, LPCTSTR szBindName)
{
    POSITION Pos = m_pDocManager->GetFirstDocTemplatePosition();
    CDocTemplate* pTemplate = NULL;

    while (Pos != NULL)
    {
        pTemplate = (CDocTemplate*)MFCFramework::theApp.GetNextDocTemplate(Pos);

        CString strDocName;

        pTemplate->GetDocString(strDocName, CDocTemplate::docName);
        if (strDocName == _T("特效编辑"))
        {
            break;
        }
    }

    ASSERT(pTemplate != NULL);
    ASSERT_KINDOF(CDocTemplate, pTemplate);


    KSceneSFXEditorDoc* pDoc = dynamic_cast<KSceneSFXEditorDoc*>(pTemplate->OpenDocumentFile(NULL));
    if (pDoc)
    {
        IEKG3DSceneSFXEditor* pSFXEditor = NULL;
        pDoc->GetSceneSFXEditor(&pSFXEditor);
        if (pSFXEditor)
            pSFXEditor->OpenModel2Bind(szModelName, szBindName);
    }

}

void MFCFramework::KSceneEditorApp::NewSFXEditorToModifyBindInfo(
    IKG3DModel* pModel, 
    IKG3DModel* pBind, 
    int nModifyIndex,
    CWnd *pInvoker,
    const CString& strSFXStartPath,
    DWORD dwNowTime
    )
{
    POSITION Pos = m_pDocManager->GetFirstDocTemplatePosition();
    CDocTemplate* pTemplate = NULL;

    while (Pos != NULL)
    {
        pTemplate = (CDocTemplate*)MFCFramework::theApp.GetNextDocTemplate(Pos);

        CString strDocName;

        pTemplate->GetDocString(strDocName, CDocTemplate::docName);
        if (strDocName == _T("特效编辑"))
        {
            break;
        }
    }

    ASSERT(pTemplate != NULL);
    ASSERT_KINDOF(CDocTemplate, pTemplate);


    KSceneSFXEditorDoc* pDoc = dynamic_cast<KSceneSFXEditorDoc*>(pTemplate->OpenDocumentFile(NULL));
    if (pDoc)
    {
        POSITION Pos = pDoc->GetFirstViewPosition();
        KSceneSFXEditorView* pView = dynamic_cast<KSceneSFXEditorView*>(pDoc->GetNextView(Pos));
    
		if (pView && pView->m_paneSFXModelView)
        {
			pView->m_paneSFXEditerWnd.SetActivePage(4);
            pView->m_paneSFXModelView.m_sheet.m_pageModelView.m_strStartPath = strSFXStartPath;
            pView->m_paneSFXModelView.m_sheet.m_pageModelView.OnBnClickedButtonRefush();
            pView->m_pInvoker = pInvoker;
        }


        IEKG3DSceneSFXEditor* pSFXEditor = NULL;
        pDoc->GetSceneSFXEditor(&pSFXEditor);

		if (pSFXEditor)//pSFXEditor->IsModifyMode())
		{
			pView->m_paneSFXKeyFrame.FillComb();
			pView->m_paneSFXKeyFrame.m_comb.SetCurSel(0);
			pView->m_paneSFXKeyFrame.OnCbnSelchangeComboAni();
            pSFXEditor->ModifyBindInfo(pModel, pBind, nModifyIndex);
        }

        pBind->AddRef();

       




    }
}

void MFCFramework::KSceneEditorApp::OnFileNewSfx()
{
	// TODO: Add your command handler code here
    POSITION Pos = m_pDocManager->GetFirstDocTemplatePosition();
    CDocTemplate* pTemplate = NULL;

    while (Pos != NULL)
    {
        pTemplate = (CDocTemplate*)MFCFramework::theApp.GetNextDocTemplate(Pos);

        CString strDocName;

        pTemplate->GetDocString(strDocName, CDocTemplate::docName);
        if (strDocName == _T("特效编辑"))
        {
            break;
        }
    }

	ASSERT(pTemplate != NULL);
	ASSERT_KINDOF(CDocTemplate, pTemplate);
	pTemplate->OpenDocumentFile(NULL);
}

void MFCFramework::KSceneEditorApp::OnAppExit()
{
	return;
}

void MFCFramework::KSceneEditorApp::OnMaterialTool()
{
	CMaterialUniformToolsDlg dlg;
	dlg.DoModal();
}

void MFCFramework::KSceneEditorApp::OnAmbientOnly()
{
	g_pEngineManager->SetAmibentShaderOnly( !g_pEngineManager->GetAmibentShaderOnly() );
}

BOOL MFCFramework::KSceneEditorApp::PreTranslateMessage(MSG* pMsg)
{
	extern BYTE* g_SceneEditorKeyState;
	if (pMsg->message >= WM_KEYFIRST && pMsg->message <= WM_KEYLAST)
		g_SceneEditorKeyState[pMsg->wParam] = !!(GetAsyncKeyState(INT(pMsg->wParam)) & 0x8000);
	if (pMsg->message >= WM_MOUSEFIRST && pMsg->message <= WM_MOUSELAST && pMsg->message != WM_MOUSEMOVE || pMsg->message == WM_COMMAND)
	{	
		g_SceneEditorKeyState[VK_LBUTTON] = !!(GetAsyncKeyState(VK_LBUTTON) & 0x8000);
		g_SceneEditorKeyState[VK_MBUTTON] = !!(GetAsyncKeyState(VK_MBUTTON) & 0x8000);
		g_SceneEditorKeyState[VK_RBUTTON] = !!(GetAsyncKeyState(VK_RBUTTON) & 0x8000);
	}

	return CWinApp::PreTranslateMessage(pMsg);
}

void MFCFramework::KSceneEditorApp::OnSpeedTreeConvert()
{
	KGDLG_SPEEDTREE_CONVERTER dlgConverter;
	dlgConverter.DoModal();
}

void MFCFramework::KSceneEditorApp::OnPlayCheckTool()
{
	HRESULT hResult = E_FAIL;
	TCHAR strCheckToolModel[MAX_PATH];
	IEKG3DScene* pScene = NULL;
    KSceneModelEditorDoc* pDoc = NULL;
	KSceneModelEditorView *pModelEditor = NULL;
	sprintf_s(strCheckToolModel,
		MAX_PATH,
		"%sdata\\public\\PlayerCheckTool\\male\\Normal\\m2.mdl",
		g_szDefWorkDirectory);
	pDoc = static_cast<KSceneModelEditorDoc*>(OpenDocumentFile(strCheckToolModel));
	KG_PROCESS_ERROR(pDoc);
    
	pModelEditor = static_cast<KSceneModelEditorView*>(GetActiveView());
	KG_PROCESS_ERROR(pModelEditor);
	
	pModelEditor->OnPlayerCheckTool();
	hResult = S_OK;
Exit0:
    if (FAILED(hResult))
	{
		MessageBox(AfxGetMainWnd()->GetSafeHwnd(), "打开初始模型失败，请检查模型是否存在。", "错误", MB_OK);
	}
}

void MFCFramework::KSceneEditorApp::OnNpcCheckTool()
{
	HRESULT hResult = E_FAIL;
	TCHAR strCheckToolModel[MAX_PATH];
	IEKG3DScene* pScene = NULL;
	KSceneModelEditorDoc* pDoc = NULL;
	KSceneModelEditorView *pModelEditor = NULL;
	sprintf_s(strCheckToolModel,
		MAX_PATH,
		"%sdata\\public\\PlayerCheckTool\\male\\Normal\\m2.mdl",
		g_szDefWorkDirectory);
	pDoc = static_cast<KSceneModelEditorDoc*>(OpenDocumentFile(strCheckToolModel));
	KG_PROCESS_ERROR(pDoc);

	pModelEditor = static_cast<KSceneModelEditorView*>(GetActiveView());
	KG_PROCESS_ERROR(pModelEditor);

	pModelEditor->OnMDLCheckTool();
	hResult = S_OK;
Exit0:
	if (FAILED(hResult))
	{
		MessageBox(AfxGetMainWnd()->GetSafeHwnd(), "打开初始模型失败，请检查模型是否存在。", "错误", MB_OK);
	}
}

void MFCFramework::KSceneEditorApp::OnNormalMapGen()
{
    static KGNormalMapCreatorDlg dlg;
    static bool bFirst = true;

    if (bFirst)
        dlg.Create(KGNormalMapCreatorDlg::IDD, AfxGetMainWnd());
    dlg.CenterWindow();

    bFirst = false;

    dlg.ShowWindow(SW_SHOW);
}



void MFCFramework::KSceneEditorApp::OnBsp()
{
	TCHAR strCheckToolModel[MAX_PATH];
	IEKG3DScene* pScene = NULL;
	sprintf_s(strCheckToolModel,
		MAX_PATH,
		"%sdata\\public\\PlayerCheckTool\\male\\Normal\\m2.mdl",
		g_szDefWorkDirectory);
	KSceneModelEditorDoc* pDoc = static_cast<KSceneModelEditorDoc*>(OpenDocumentFile(strCheckToolModel));
	if (!pDoc)
	{
		MessageBox(AfxGetMainWnd()->GetSafeHwnd(), "打开初始模型失败，请检查模型是否存在。", "错误", MB_OK);
		return;
	}
	pScene = pDoc->GetScene();
	KG_PROCESS_ERROR(pScene);

	KModelEditorBspGenerateDialog *pBsp = new KModelEditorBspGenerateDialog;
	ASSERT(pBsp);
	pBsp->SetScene(pScene);
	pBsp->Create(KModelEditorBspGenerateDialog::IDD, NULL);
	pBsp->SetScene(pScene);

	pBsp->ShowWindow(SW_SHOW);
Exit0:
	;
}



void  FindFilesInSubDir(LPCSTR strDirName, 
													   IEKG3DSceneModelEditor *pEditor)
{
	TCHAR strInput[MAX_PATH];
	HRESULT hResult = E_FAIL;
	IEKG3DModel *pModel = NULL;
	CFileFind Find;
	sprintf(strInput, "%s\\*.*", strDirName);
	BOOL bFound = Find.FindFile(strInput);
	while (bFound)
	{
		bFound = Find.FindNextFile();
		if (Find.IsDirectory())
		{
			CString strDirName = Find.GetFileName();
			if (strDirName != "." && strDirName != "..")
			{
				FindFilesInSubDir(Find.GetFilePath(), pEditor);
			}
		}
		else
		{
			CString strFileName = Find.GetFilePath();
			strFileName.MakeLower();
			TCHAR strExt[MAX_PATH];
			_splitpath(strFileName, NULL, NULL, NULL, strExt);
			if (!stricmp(strExt, ".mesh"))
			{
				hResult = pEditor->OpenModel(Find.GetFilePath(), NULL, NULL, FALSE, &pModel);
				KG_COM_PROCESS_ERROR(hResult);

				hResult = pModel->SaveToFile(Find.GetFilePath(), 0);
				KG_COM_PROCESS_ERROR(hResult);
			}
		}
	}
Exit0:
	Find.Close();
}

void MFCFramework::KSceneEditorApp::OnModelOptimize()
{
	CFileFind Find;
	HRESULT hResult = E_FAIL;
	BOOL bWorking = FALSE;
	size_t uLength = 0;
	TCHAR strStartFilePath[MAX_PATH];
	IEKG3DScene *pScene = NULL;
	IEKG3DSceneModelEditor *pEditor = NULL;
	IEKG3DModel *pModel = NULL;

	sprintf_s(strStartFilePath, MAX_PATH, "%sdata\\public\\TagEditor\\Pattern.Mesh", g_szDefWorkDirectory);

	KSceneModelEditorDoc* pDoc = static_cast<KSceneModelEditorDoc*>(OpenDocumentFile(strStartFilePath));
	if (!pDoc)
	{
		MessageBox(AfxGetMainWnd()->GetSafeHwnd(), "打开初始模型失败，请检查模型是否存在。", "错误", MB_OK);
		return;
	}
	pScene = pDoc->GetScene();
	KG_PROCESS_ERROR(pScene);

	pScene->GetSceneModelEditor(&pEditor);
	KG_PROCESS_ERROR(pEditor);
		
	//sprintf_s(strStartFilePath, "%sdata\\source\\maps_source", g_szDefWorkDirectory);
	//FindFilesInSubDir(strStartFilePath, pEditor);
	
	sprintf_s(strStartFilePath, "%sdata\\source\\player", g_szDefWorkDirectory);
	FindFilesInSubDir(strStartFilePath, pEditor);

	
	//sprintf_s(strStartFilePath, "%sdata\\source\\player\\部件\\F2_1000_belt.mesh", g_szDefWorkDirectory);
	//for (int i = 0; i < 100; i++)
	{
	//	pEditor->OpenModel(strStartFilePath, NULL, NULL, FALSE, NULL);
	}
	
Exit0:
	;
}

CWnd* GetModelFrameWnd()
{
	CWnd* pReturn = NULL;
	CMDIFrameWnd* pMainWnd = static_cast<CMDIFrameWnd*>(AfxGetMainWnd());
	KG_PROCESS_ERROR(pMainWnd);
	pReturn = pMainWnd->MDIGetActive();
Exit0:
	return pReturn;
}

CView* MFCFramework::KSceneEditorApp::GetActiveView()
{
    MFCFramework::CMainFrame* pMainFrame = static_cast<MFCFramework::CMainFrame*>(AfxGetMainWnd());
    CFrameWnd* pFrameWnd = pMainFrame->GetActiveFrame();
    return pFrameWnd->GetActiveView();
}

