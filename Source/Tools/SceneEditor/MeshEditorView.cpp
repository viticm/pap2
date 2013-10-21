// MeshEditorView.cpp : 实现文件
//

#include "stdafx.h"
#include "KGPublic.h"
#include "SceneEditor.h"
#include "MeshEditorView.h"
#include "mesheditordoc.h"
#include "ChildFrm.h"
#include "MainFrm.h"
#include "KSceneModelEditorSFXNewDialog.h"
#include "KSceneEditBaseSelectDialog.h"
#include "mesheditorview.h"
#include "KSceneModelEditorEffectSetDialog.h"
#include "IEKG3DScene.h"
#include "KSceneSettingDialog.h"
//#include "kdlglightproperty.h"
#include "KModelEditorPlayerToolsDialog.h"
#include "KDlgMDLCheckTool.h"
#include "IEEditor.h"//by dengzhihui Refactor 2006年11月24日
#include "IEKG3DAnimationTag.h"
#include "IEKG3DModelTable.h"
#include "KSceneModelEditorAnimationPanel.h"
#include "KSceneModelEditorFrameWnd.h"
#include "KSceneModelEditorLoadAni.h"
#include "IEKG3DTextureTable.h"
#include "KDlgFilterFileView.h"
#include "KDlgLogicSaveProgress.h"
#include "KGFileFind.h"
#include "KDlgAnimationTagBatchTool.h"
#include "KDlgTagProcessFrontEnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(KSceneModelEditorView, CBaseView)

#define GETEDITOR() \
	IEKG3DSceneModelEditor* pEditor = GetDocScene();\
	_ASSERTE(pEditor);

static BOOL s_bEnableHairLOD = FALSE;
const static TCHAR *s_strBatchToolConfigInI = "data\\public\\PlayerCheckTool\\BatchTool.ini";
const static TCHAR *s_strBatchToolConfigSectionPath = "Path";

//(by dengzhihui 2006年11月1日 17:05:54
CModelSampleSink::CModelSampleSink()
{
	m_pDialogSpeedTree = NULL;
}

void CModelSampleSink::SinkFunc(INT nSinkID)
{
	KSceneModelEditorSFXSpeedTreeDialog::OnUpdateUI((IEKG3DModelSpeedTree*)(PVOID)(INT_PTR)nSinkID, m_pDialogSpeedTree);
}
//)

KSceneModelEditorView::KSceneModelEditorView()
{
	m_lpDialogSword2Check = NULL;
	m_pDialogSpeedTree = NULL;
	m_nOutputWindowID = -1;
	m_lpDialogPlayerTools = NULL;
	m_pMDLCheckTool = NULL;
	m_pPlayerCheckTool = NULL;
	m_lpDialogSkillAnimation = NULL;
	m_bBindAll = FALSE;
	m_pFilterFileDlg = NULL;
	memset(m_bInited, 0, sizeof(bool) * s_NumTypes);
}

KSceneModelEditorView::~KSceneModelEditorView()
{
	SAFE_DELETE(m_pPlayerCheckTool);
	SAFE_DELETE(m_lpDialogPlayerTools);
	SAFE_DELETE(m_lpDialogSword2Check);
	SAFE_DELETE(m_pMDLCheckTool);
	SAFE_DELETE(m_lpDialogSkillAnimation);
	SAFE_DELETE(m_pFilterFileDlg);
}

BEGIN_MESSAGE_MAP(KSceneModelEditorView, CBaseView)
	//{{AFX_MSG_MAP(KSceneEditorView)
	ON_WM_SETFOCUS()
    ON_WM_SIZE()
	ON_COMMAND(ID_MODELOPENADD, OnModelopenadd)
	ON_COMMAND(ID_SFXNEW,OnSFXNew)
	ON_COMMAND(ID_SFXCONVERTSPEEDTREE, &KSceneModelEditorView::OnConvertSpeedTree)
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	ON_COMMAND(ID_LOAD_MODEL, OnLoadModel)
	ON_COMMAND(ID_PLAY_ALL, OnPlayAll)
	ON_COMMAND(ID_SHOWBONE, OnShowbone)
	ON_COMMAND(ID_SHOWSOCKET, OnShowsocket)
	ON_COMMAND(ID_SHOWGRID, OnShowgrid)
	ON_UPDATE_COMMAND_UI(ID_SHOWGRID, OnUpdateShowgrid)
	ON_UPDATE_COMMAND_UI(ID_SHOWBONE, OnUpdateShowbone)
	ON_UPDATE_COMMAND_UI(ID_SHOWSOCKET, OnUpdateShowsocket)
	ON_COMMAND(ID_SWORD2CHECK, OnSword2check)
	ON_COMMAND(ID_MDLCHECKTOOL, OnMDLCheckTool)
	ON_COMMAND(ID_PLAYERCHECKTOOL, &KSceneModelEditorView::OnPlayerCheckTool)
	ON_COMMAND(ID_MOTIONPROCESS, OnMotionProcess)
	ON_UPDATE_COMMAND_UI(ID_PERSFECTCAMER, OnUpdatePersfectcamer)
	ON_COMMAND(ID_EDITPROPERTY, OnEditproperty)
	ON_UPDATE_COMMAND_UI(ID_EDITPROPERTY, OnUpdateEditproperty)
	ON_COMMAND(ID_SETEFFECT, OnSeteffect)
    ON_COMMAND(ID_SCENE_SETTING, OnSceneSetting)
	ON_WM_TIMER()
	ON_COMMAND(ID_MOVE, OnPopMenuMove)
	ON_COMMAND(ID_ROTATE, OnPopMenuRotate)
	ON_COMMAND(ID_SCALE, OnPopMenuScale)
	ON_COMMAND(ID_ACTION_UP, &KSceneModelEditorView::OnActionUp)
	ON_COMMAND(ID_ACTION_DOWN, &KSceneModelEditorView::OnActionDown)
	ON_COMMAND(ID_ACTION_LEFT, &KSceneModelEditorView::OnActionLeft)
	ON_COMMAND(ID_ACTION_RIGHT, &KSceneModelEditorView::OnActionRight)
	ON_COMMAND(ID_SPEEDTREE_PANEL, &KSceneModelEditorView::OnSpeedtreePanel)
	ON_COMMAND(ID_DELOBJ, &KSceneModelEditorView::OnDeleteSelectedObject)
	ON_COMMAND(ID_ISOLATE, &KSceneModelEditorView::OnIsolate)
	ON_COMMAND(ID_CHANGEMOVINGSTATE, &KSceneModelEditorView::OnChangeMovingMode)
    ON_MESSAGE(WM_SFXEDITORCLOSE_INVOKERCALLBACK, &KSceneModelEditorView::OnSFXEditorCloseInvoke)
	ON_COMMAND(ID_REFRESHSHADER, &KSceneModelEditorView::OnRefreshshader)
	ON_COMMAND(ID_SKILLANIPANEL, &KSceneModelEditorView::OnSkillAnimationPanel)
	//ON_COMMAND(ID_BILLCLOUD, &KSceneModelEditorView::OnBillcloud)
	ON_COMMAND(ID_PLAYERNORMALTOOLS, &KSceneModelEditorView::OnPlayernormaltools)
	ON_COMMAND(ID_BIND_ALL, &KSceneModelEditorView::OnBindAll)
	ON_COMMAND(ID_MODELEDITOR_FILTEROPEN, &KSceneModelEditorView::OnFilterOpen)
	ON_COMMAND(ID_HAIRLOD, &KSceneModelEditorView::OnEnableHairLOD)
	ON_COMMAND(ID_MODELEDITORSHOWGRID, &KSceneModelEditorView::OnShowGrid)
	ON_UPDATE_COMMAND_UI(ID_MODELEDITORSHOWGRID, &KSceneModelEditorView::OnUpdateShowGrid)
	ON_UPDATE_COMMAND_UI(ID_HAIRLOD, &KSceneModelEditorView::OnUpdateHairLOD)
	ON_WM_ACTIVATE()
END_MESSAGE_MAP()

#define GET_SCENE() \
	KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();\
	IEKG3DScene* pScene = pDoc->GetScene();\
	if(!pScene) return;

#define GET_EDITOR() \
	IEKG3DSceneModelEditor* pEditor = NULL;\
	pScene->GetSceneModelEditor(&pEditor); \
	if(!pEditor)	\
	return;

#define GET_BASE()\
	IEKG3DSceneEditorBase* pBase = NULL;\
	pEditor->GetSceneEditorBase(&pBase);\
	_ASSERTE(pBase);\
	if (!pBase) return;

// KSceneModelEditorView 绘图
void KSceneModelEditorView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: 在此添加绘制代码
}

// KSceneModelEditorView 诊断
#ifdef _DEBUG
void KSceneModelEditorView::AssertValid() const
{
	CBaseView::AssertValid();
}

void KSceneModelEditorView::Dump(CDumpContext& dc) const
{
	CBaseView::Dump(dc);
}
#endif //_DEBUG

void KSceneModelEditorView::OnSkillAnimationPanel()
{
	if (!m_lpDialogSkillAnimation)
	{
		const TCHAR *strPath = NULL;
		TCHAR strFileName[MAX_PATH];
		IEKG3DModel *pModel = NULL;
		GET_SCENE();
		GET_EDITOR();
		pEditor->GetCurModel(&pModel);
		KG_PROCESS_ERROR(pModel);
		
		pModel->GetName(&strPath);
		strcpy_s(strFileName, MAX_PATH, strPath);
		for (size_t i = 0; i < sizeof(g_ModelAnimationKeywords) /sizeof(const TCHAR*); i++)
		{
			TCHAR *strModify = strstr(strFileName, g_ModelAnimationKeywords[i]);
			if (strModify)
			{
				strModify[0] = '\0';
				strcat(strModify, "动作\\");
			}
		}
		m_lpDialogSkillAnimation = new KSceneModelEditorLoadAni;
		KG_PROCESS_ERROR(m_lpDialogSkillAnimation);

		m_lpDialogSkillAnimation->SetCurDir(strFileName, ".ani");
		m_lpDialogSkillAnimation->SetModel(pModel);
		m_lpDialogSkillAnimation->m_pParentView = this;
		m_lpDialogSkillAnimation->Create(IDD_LOAD_ANI, this);
		m_lpDialogSkillAnimation->ShowWindow(SW_SHOW);
	}
Exit0:
	;
}

void KSceneModelEditorView::OnSceneSetting()
{
    static KSceneSettingDialog* dlg = NULL;
    
    if (!dlg)
    {
        dlg = new KSceneSettingDialog;
        dlg->Create(KSceneSettingDialog::IDD, this);
    }

    dlg->ShowWindow(SW_SHOW);
}

// KSceneModelEditorView 消息处理程序
void KSceneModelEditorView::OnSetFocus(CWnd* pOldWnd)
{

	CBaseView::OnSetFocus(pOldWnd);

	KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();
	if(pDoc)
	{
		IEKG3DScene* pScene = pDoc->GetScene();
		if(pScene)
		{
			g_pEngineManager->SetFocusScene(pScene);//by dengzhihui Refactor 2006年11月6日
		}

		IEKG3DTextureTable *piTable = NULL;
		IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

		pManager->GetIEKG3DTextureTable(&piTable);
		piTable->ReloadFromFile();
	}

}
void KSceneModelEditorView::OnSize(UINT nType, int cx, int cy)
{
	CBaseView::OnSize(nType, cx, cy);

	GET_SCENE();
	pScene->OnResizeWindow(NULL);
}


LRESULT KSceneModelEditorView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	LRESULT hr = CBaseView::WindowProc(message, wParam, lParam);

	return 1;
	

}

void KSceneModelEditorView::OnModelopenadd()
{
	char bigBuff[800] = "";
	char szFilter[] ="Mesh Files (*.Mesh)|*.Mesh|X Files (*.x)| *.x|SpeedTree Files (*.STree)| *.STree|体积雾 (*.lvf)| *.lvf|闪电 (*.dlf)| *.dlf||";
	KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
	if (!pDoc)
    {
        return;
    }

	CFileDialog dlg(
        true, NULL, NULL,
		OFN_HIDEREADONLY,
        szFilter
    );
	dlg.m_ofn.lpstrFile = bigBuff;
	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	if (dlg.DoModal() == IDOK)
	{
		TCHAR path_buffer[256];
		TCHAR drive[256];
		TCHAR dir[256];
		TCHAR fname[256];
		TCHAR ext[256];

		CString Vs = dlg.GetPathName();

		wsprintf(path_buffer, "%s", Vs);
		_splitpath(path_buffer, drive, dir, fname, ext);

		TCHAR filename[256];

	    IEKG3DModel* pModel = NULL;
        IEKG3DModelTable *pModelTable = NULL;

		DWORD NewModelID  = 0;
		CString sExt = dlg.GetFileExt();
		sExt.MakeLower();
		//.x File
	    if(sExt == "x")
		{
	//		wsprintf(filename, "%s%s%s.x",drive, dir, fname);
	//		if (FAILED(g_cMeshTable.LoadFromFile(&NewMeshID,filename)))
	//			return ;
	//		if (FAILED(g_cMeshTable.GetXMesh(&pMesh,NewMeshID)))
	//			return ;
		}
		else if(sExt == "stree")
		{
			wsprintf(filename, "%s%s%s.STree",drive, dir, fname);
            //Get3DEngineInterface((void **)(&pManager));

            pManager->GetIEKG3DModelTable(&pModelTable);
            if (FAILED(pModelTable->IELoadResourceFromFile(filename, 0, 0,
				(IKG3DResourceBase **)&pModel)))
            {
				return ;
            }
        }
		else if(sExt == "lvf")
		{
	//		wsprintf(filename, "%s%s%s.lvf",drive, dir, fname);
	//		if (FAILED(g_cMeshTable.LoadFromFile(&NewMeshID,filename)))
	//			return ;
	//		if (FAILED(g_cMeshTable.GetXMesh(&pMesh,NewMeshID)))
	//			return ;
		}
		else if(sExt == "dlf")
		{
	//		wsprintf(filename, "%s%s%s.dlf",drive, dir, fname);
	//		if (FAILED(g_cMeshTable.LoadFromFile(&NewMeshID,filename)))
	//			return ;
	//		if (FAILED(g_cMeshTable.GetXMesh(&pMesh,NewMeshID)))
	//			return ;
		}
		else//MeshFile
		{
	//		wsprintf(filename,"%s%s%s.Mesh",drive,dir,fname);
	//		if (FAILED(g_cMeshTable.LoadFromFile(&NewMeshID,filename)))
	//			return ;

	//		if (FAILED(g_cMeshTable.GetXMesh(&pMesh,NewMeshID)))
	//			return ;

	//		wsprintf(filename,"%s%s%s.Mtl",drive,dir,fname);
	//		pMesh->LoadMaterial(filename);
		}

		ASSERT(pModel);
        //KG3DSceneModelEditor* pScene = (KG3DSceneModelEditor*)pDoc->GetScene();
		//by dengzhihui Refactor 2006年11月24日
		GET_SCENE();
		GET_EDITOR();
		pEditor->AddRenderEntity(pModel, 0, TRUE);

		/*IEKG3DSceneSelectBase* pSelector = NULL;
		pEditor->GetSceneSelector(&pSelector);
		_ASSERTE(pSelector);
		pSelector->AddSelectableEntity(SCENEENTITY_MODEL, pModel);*/

		IEKG3DSceneEditorBase* pEditBase = NULL;
		pEditor->GetSceneEditorBase(&pEditBase);
		pEditBase->IEInsertEntitySelectable(SCENEENTITY_MODEL, pModel, TRUE);//<Selectable Refactor>
	}
}

void KSceneModelEditorView::OnPlayerTools()
{
	GET_SCENE();
	GET_EDITOR();
	if(!m_lpDialogPlayerTools)
	{
		m_lpDialogPlayerTools = new KModelEditorPlayerToolsDialog;
		if(!m_lpDialogPlayerTools)
			return;
		m_lpDialogPlayerTools->Create(KModelEditorPlayerToolsDialog::IDD);
		m_lpDialogPlayerTools->SetModelEditor(pEditor);
	}
	m_lpDialogPlayerTools->ShowWindow(TRUE);

}
void KSceneModelEditorView::OnSFXNew()
{
	GET_SCENE();
	GET_EDITOR();

	KSceneModelEditorSFXNewDialog Dlg;
	Dlg.SetModelEditor(pEditor,pDoc);

	Dlg.DoModal();

}

void KSceneModelEditorView::OnSelect()
{
	GET_SCENE();
	GET_EDITOR();
	GET_BASE();

	KSceneEditBaseSelectDialog Dlg;
	Dlg.SetEditor(pBase);
	Dlg.DoModal();

	/*
	KSceneEntity Entity;

	if(SUCCEEDED(pDoc->m_lpMeshEditorScene->GetFirstSelectedEntity(Entity)))
	{
		switch(Entity.Type)
		{
		case SCENEENTITY_MODEL:
			{
				LPMODEL pModel = (LPMODEL) Entity.Pointer;
				pDoc->m_lpMeshEditorScene->SetCurMesh(pModel);

				break;
			}
		}
	}
	else
	{
		pDoc->m_lpMeshEditorScene->SetCurMesh(NULL);
	}*/
// Exit0:
// 	return;
}
void KSceneModelEditorView::OnShowbone()
{
	//// TODO: Add your command handler code here
	//KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();

	//if(!pDoc)
	//	return;
	//if(!pDoc->m_lpMeshEditorScene)
	//	return;

	//pDoc->m_lpMeshEditorScene->m_bShowBone = !pDoc->m_lpMeshEditorScene->m_bShowBone;
}

void KSceneModelEditorView::OnShowsocket()
{
	// TODO: Add your command handler code here
	///*KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();

	//if(!pDoc)
	//	return;
	//if(!pDoc->m_lpMeshEditorScene)
	//	return;

	//pDoc->m_lpMeshEditorScene->m_bShowSocket = !pDoc->m_lpMeshEditorScene->m_bShowSocket;*/
}

void KSceneModelEditorView::OnShowgrid()
{
	// TODO: Add your command handler code here
	/*KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();

	if(!pDoc)
		return;
	if(!pDoc->m_lpMeshEditorScene)
		return;
*/
	//pDoc->m_lpMeshEditorScene->m_bShowGrid = !pDoc->m_lpMeshEditorScene->m_bShowGrid;
}

void KSceneModelEditorView::OnUpdateShowgrid(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	/*KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();

	if(!pDoc)
		return;
	if(!pDoc->m_lpMeshEditorScene)
		return;
	pCmdUI->SetCheck(pDoc->m_lpMeshEditorScene->m_bShowGrid);*/
}

void KSceneModelEditorView::OnUpdateShowbone(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	/*KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();

	if(!pDoc)
		return;
	if(!pDoc->m_lpMeshEditorScene)
		return;
	pCmdUI->SetCheck(pDoc->m_lpMeshEditorScene->m_bShowBone);*/
}

void KSceneModelEditorView::OnUpdateShowsocket(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	/*KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();

	if(!pDoc)
		return;
	if(!pDoc->m_lpMeshEditorScene)
		return;
	pCmdUI->SetCheck(pDoc->m_lpMeshEditorScene->m_bShowSocket);*/
}

void KSceneModelEditorView::OnSword2check()
{
	KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();
	IEKG3DScene* pCurScene = NULL;
	if(!pDoc)
		goto Exit0;

	pCurScene = pDoc->GetScene();
	if(!pCurScene)
		goto Exit0;

	//如果还没有Check Dialog, 就整一个出来
	if(!m_lpDialogSword2Check)
	{
		m_lpDialogSword2Check = new KSceneModelEditorSword2CheckTool();
		m_lpDialogSword2Check->Create(IDD_MODELEDITOR_CHECKTOOL,NULL);
	}

	GETEDITOR();
	m_lpDialogSword2Check->SetModelEditor(pEditor);
	m_lpDialogSword2Check->ShowWindow(TRUE);

Exit0:
	return;
}


void KSceneModelEditorView::OnUpdatePersfectcamer(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
	//KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();

	//if(!pDoc)
	//	return;
	//if(!pDoc->m_lpMeshEditorScene)
	//	return;

	///*KSceneTable* pSceneTable = (KSceneTable*) pDoc->m_lpMeshEditorScene->m_pSceneTable;
	//KEngineManager* pManager = (KEngineManager*) pSceneTable->m_pManager;
	//KGraphicsEngine* pGEngine = (KGraphicsEngine*) &pManager->g_GraphiceEngine;
	//GraphicsStruct::KCamera* pCamera = &pGEngine->m_cCamera; */

	//pCmdUI->SetCheck(m_Camera.m_bPerspect);
}

void KSceneModelEditorView::OnEditproperty()
{

}

void KSceneModelEditorView::OnUpdateEditproperty(CCmdUI *pCmdUI)
{
	// TODO: Add your command update UI handler code here
}

void KSceneModelEditorView::OnPlayAll()
{
	/*KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();

	if(!pDoc)
		return;
	if(!pDoc->m_lpMeshEditorScene)
		return;

	list<KSceneEntity>::iterator it = pDoc->m_lpMeshEditorScene->m_RenderEntityList.begin();
	while ( it != pDoc->m_lpMeshEditorScene->m_RenderEntityList.end() )
	{

		KSceneEntity& Entity = *it;
		if ( Entity.Type == SCENEENTITY_MODEL )
		{
			KModel* pModel = reinterpret_cast<KModel*>(Entity.Pointer);
			if ( (int)pModel->m_dNumAnimation > pModel->m_nCurAnimationIndex )
			{
				pModel->PlayAnimation(pModel->m_nCurAnimationIndex, pModel->m_dCurAnimationMethod, pModel->m_fCurAnimationSpeed );
			}
		}
		it++;
	}*/
}

void KSceneModelEditorView::OnLoadModel()
{
	/*KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();

	if(!pDoc)
		return;
	if(!pDoc->m_lpMeshEditorScene)
		return;

	CFileDialog dlgOpen(TRUE, "mesh", "NoName", OFN_OVERWRITEPROMPT, "模型文件(*.mesh) |*.mesh||", AfxGetMainWnd());
	if( dlgOpen.DoModal() == IDOK )
	{
		CString strPathName = dlgOpen.GetPathName();
		KModel* pNewModel = NULL;
		g_cMeshTable.Get1NewXMesh( &pNewModel );
		LPSTR strFileName = new TCHAR[strPathName.GetLength() + 1 ];
		strcpy(strFileName, strPathName);
		if ( SUCCEEDED( pNewModel->LoadMesh( strFileName ) ) )
		{
			strFileName[ strPathName.GetLength() - 4 ] = '\0';
			strcat(strFileName, "mtl");
			pNewModel->LoadMaterial(strFileName);
			pDoc->m_lpMeshEditorScene->AddRenderMesh( pNewModel );
			pDoc->m_lpMeshEditorScene->AddSelectableMesh( pNewModel );
		}
		SAFE_DELETE_ARRAY( strFileName );
	}*/
}


void KSceneModelEditorView::OnSeteffect()
{
	// TODO: Add your command handler code here
	/*KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();

	if(!pDoc)
		return;
	if(!pDoc->m_lpMeshEditorScene)
		return;

	KSceneModelEditorEffectSetDialog Dlg;

	Dlg.DoModal();*/

}



void KSceneModelEditorView::OnDestroy()
{
	KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();
	KGLOG_PROCESS_ERROR(pDoc);

	IEKG3DScene* pScene = pDoc->GetScene();
	KG_PROCESS_ERROR(pScene);

	pScene->RemoveOutputWindow(m_nOutputWindowID);
	m_nOutputWindowID = -1;

Exit0:
	CBaseView::OnDestroy();
}

//(by dengzhihui 2006年9月15日 11:55:57
void KSceneModelEditorView::OnInitialUpdate()
{
	IEKG3DModel* pModel = NULL;
    GETEDITOR()
    CBaseView::OnInitialUpdate();

	GET_SCENE();
	KGLOG_PROCESS_ERROR(pScene);

	static PROXYKEYMAP KEYMAP[] =
	{	//           C  S  A  L  M  R  ?    Trigger     ID  Prev Ig Exclusive
		{VK_RBUTTON, 0, 0, 0, 0, 0, 1, 0, WM_MOUSEMOVE , EXEACTION_ROTATE_CAMERA,  0, 0, 0},	//按住右键不放，就转动摄像机
		//{VK_MENU   , 0, 0, 1, 0, 1, 0, 0, WM_MOUSEMOVE,   EXEACTION_ROTATE_CAMERA,  0, 0, 0},	//alt+MBT, 旋转摄像机镜头
		{VK_CONTROL, 1, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   EXEACTION_ZOOM_VIEW,  0, 0, 0},	//ctrl+MBT, 快速缩放视角
		{VK_MBUTTON, 0, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   EXEACTION_PAN_VIEW,  0, 0, 0},	//MBT, PAN视图
		{VK_LBUTTON, 1, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_INVERT_ACTION,  0, 0, 0},	//CTRL＋LBT，反向当前操作

		{VK_LBUTTON, 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_LEFE_KEY_DOWN,  0, 0, 0},	//左键按下，可以作选择、改变地形等很多动作
		{VK_LBUTTON, 1, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_SELECTION_ADD,  0, 0, 0},	//在当前选择范围中，再增加选择内容
		{VK_LBUTTON, 0, 0, 1, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_SELECTION_SUTRACT,  0, 0, 0},	//在当前选择范围中，再减少选择内容

		{0		   , 0, 0, 0, 0, 0, 0, 0, 0             , 0,  0, 0},	//结束
	};

    GET_BASE();

    m_InputProxy.Init(pBase, this, KEYMAP);
    
Exit0:
	;
}

//)

void KSceneModelEditorView::OnPopMenuMove()
{
	KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();
	//KG3DSceneModelEditor* pScene =
	//	static_cast<KG3DSceneModelEditor*>(pDoc->GetScene());
	//if(!pScene)
	//	return;
	GETEDITOR();
	GET_BASE();

	pBase->SetEditState(SCENESTATE_MOVE);//m_dwEditState = SCENESTATE_MOVE;

	IEKG3DSceneSelectBase* pSelector = NULL;
	pEditor->GetSceneSelector(&pSelector);
	_ASSERTE(pSelector);
	//pSelector->OnToggleStateMove();//<OnToggleStateMove Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
}


void KSceneModelEditorView::OnPopMenuRotate()
{
	GET_SCENE();
	GET_EDITOR();
	GET_BASE();

	pBase->SetEditState(SCENESTATE_ROTATE);
	//pScene->m_dwEditState = SCENESTATE_ROTATE;

	/*IEKG3DSceneSelectBase* pSelector = NULL;
	pEditor->GetSceneSelector(&pSelector);
	pSelector->OnToggleStateRotation();*///<OnToggleStateRotation Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
}

void KSceneModelEditorView::OnPopMenuScale()
{
	GET_SCENE();
	GET_EDITOR();
	GET_BASE();
	pBase->SetEditState(SCENESTATE_SCALE);
	/*IEKG3DSceneSelectBase* pSelector = NULL;
	pEditor->GetSceneSelector(&pSelector);
	pEditor->OnToggleStateScaling();*///<OnToggleStateScaling Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
}

void KSceneModelEditorView::OnActionUp()
{
	GETEDITOR();
	if (pEditor)
		pEditor->OnCommand(ctUp);
}

void KSceneModelEditorView::OnActionDown()
{
	GETEDITOR();
	if (pEditor)
		pEditor->OnCommand(ctDown);
}

void KSceneModelEditorView::OnActionLeft()
{
	GETEDITOR();
	if (pEditor)
		pEditor->OnCommand(ctLeft);
}

void KSceneModelEditorView::OnActionRight()
{
	GETEDITOR();
	if (pEditor)
		pEditor->OnCommand(ctRight);
}

IEKG3DSceneModelEditor* KSceneModelEditorView::GetDocScene()
{
	KSceneModelEditorDoc* pDoc =  static_cast<KSceneModelEditorDoc*>(GetDocument());
	_ASSERTE(pDoc);
	if (pDoc)
	{
		IEKG3DScene* p3DScene = pDoc->GetScene();
		IEKG3DSceneModelEditor* pEditor = NULL;
		p3DScene->GetSceneModelEditor(&pEditor);//by dengzhihui Refactor 2006年11月24日
		_ASSERTE(pEditor);
		return pEditor;
	}
	return NULL;
}

void KSceneModelEditorView::OnSpeedtreePanel()
{
	HRESULT hr = E_FAIL;

	IEKG3DModel* pModel = NULL;
	IEKG3DModelSpeedTree* pSpeedTree = NULL;
	GET_SCENE();
	GET_EDITOR();

	hr = pDoc->GetCurrentModel(&pModel);
	KG_COM_PROCESS_ERROR(hr);

    DWORD dwType = 0;
    hr = pModel->GetType(&dwType);
    KGLOG_COM_PROCESS_ERROR(hr);

	KG_PROCESS_ERROR(dwType == MESHTYPE_SPEEDTREE);

	pModel->GetIEKG3DModelSpeedTree(&pSpeedTree);

	if (!m_pDialogSpeedTree)
	{
		m_pDialogSpeedTree =  new KSceneModelEditorSFXSpeedTreeDialog;
		m_pDialogSpeedTree->SetSpeedTree(pSpeedTree,pEditor);
		m_pDialogSpeedTree->Create(KSceneModelEditorSFXSpeedTreeDialog::IDD, this);
	}
	m_pDialogSpeedTree->ShowWindow(TRUE);

	pEditor->SetSpeedTreeUIFunction(KSceneModelEditorSFXSpeedTreeDialog::OnUpdateUI, m_pDialogSpeedTree);
Exit0:
	return;
}

void KSceneModelEditorView::Zoom()
{
	//CBaseView::OnZoomToObj();
	GETEDITOR();
	KG_PROCESS_ERROR(pEditor);
	pEditor->ZoomToObj();
Exit0:
	;
}



void KSceneModelEditorView::OnDeleteSelectedObject()
{
	IEKG3DSpeedTreeModifier* pSpdTreeModifier = NULL;
	GETEDITOR();
	KG_PROCESS_ERROR(pEditor);

	pEditor->GetSpeedTreeModifier(&pSpdTreeModifier);
	if (pSpdTreeModifier)
	{
		pSpdTreeModifier->DeleteSelectedLeaf();
	}
Exit0:
	return;
}

void KSceneModelEditorView::OnIsolate()
{
	GETEDITOR();
	KG_PROCESS_ERROR(pEditor);
	pEditor->IsolateCurrentSelection();
	
	if (m_pDialogSpeedTree)
	{
		if (pEditor->GetIsolateMode())
		{
			static_cast<CButton*>(m_pDialogSpeedTree->GetDlgItem(IDC_CHECK_ISOLATE))->SetCheck(BST_CHECKED);
		}
		else
		{
			static_cast<CButton*>(m_pDialogSpeedTree->GetDlgItem(IDC_CHECK_ISOLATE))->SetCheck(BST_UNCHECKED);
		}
	}
Exit0:
	return;
}

void KSceneModelEditorView::OnMDLCheckTool()
{
	if (!m_pMDLCheckTool)
	{
		m_pMDLCheckTool = new KDlgMDLCheckTool(this);
		m_pMDLCheckTool->Create(KDlgMDLCheckTool::IDD, this);
	}
	GETEDITOR();
	m_pMDLCheckTool->SetModelEditor(pEditor);
	m_pMDLCheckTool->ShowWindow(SW_SHOW);
}

void KSceneModelEditorView::OnPlayerCheckTool()
{
	if (!m_pPlayerCheckTool)
	{
		KSceneModelEditorDoc* pDoc =  static_cast<KSceneModelEditorDoc*>(GetDocument());
		KG_PROCESS_ERROR(pDoc);

		IEKG3DScene* pScene = pDoc->GetScene();
		KG_PROCESS_ERROR(pScene);

		m_pPlayerCheckTool = new KDlgPlayerCheckTool;
		KG_PROCESS_ERROR(m_pPlayerCheckTool);

		m_pPlayerCheckTool->SetScene(pScene);
		m_pPlayerCheckTool->Create(KDlgPlayerCheckTool::IDD, NULL);
		//m_pPlayerCheckTool->SetParent(AfxGetMainWnd());
	}
	m_pPlayerCheckTool->ShowWindow(SW_SHOW);
Exit0:
	;
}

void KSceneModelEditorView::OnChangeMovingMode()
{
	g_pEngineManager->ChangeMovingState();
}

LRESULT KSceneModelEditorView::OnSFXEditorCloseInvoke(WPARAM wParam, LPARAM lParam)
{
    KSceneModelEditorAnimationPanel* pAnimationPanel = NULL;
    CWnd* p1 = GetParent();
    CWnd* p2 = p1->GetParent();
    CWnd* p3 = p2->GetParent();
    KSceneModelEditorFrameWnd* pParent = static_cast<KSceneModelEditorFrameWnd*>(p3);
    KG_PROCESS_ERROR(pParent);
    pAnimationPanel = pParent->GetAnimationSeekBar();
    KG_PROCESS_ERROR(pAnimationPanel);
    pAnimationPanel->OnSFXEditorClose();
Exit0:
    return 0;
}
void KSceneModelEditorView::OnRefreshshader()
{
	if (g_pGraphicsTool)
	{
		g_pGraphicsTool->RefreshShader();
	}
}
void KSceneModelEditorView::OnPlayernormaltools()
{
	OnPlayerTools();
}

static DWORD GetDefaultTypeByModelName(LPCSTR strFileName)
{
	DWORD dwReturn = s_NumTypes - 1;
	TCHAR strLower[MAX_PATH];
	strcpy_s(strLower, strFileName);
	_strlwr_s(strLower, MAX_PATH);
	for (size_t i = 0; i < s_NumTypes - 1; i++)
	{
		if (strstr(strLower, s_strDefaultBindSocketType[i]))
		{
			dwReturn = static_cast<DWORD>(i);
			break;
		}
	}
	return dwReturn;
}

void KSceneModelEditorView::InitDefaultBindSocket(DWORD dwType)
{
	TCHAR *strSectionName = NULL;
	IIniFile *pConfigFile = NULL;
	int nNumSocket = 0;
	_ASSERT(dwType < s_NumTypes);
	KG_PROCESS_SUCCESS(m_bInited[dwType]);

	m_bInited[dwType] = true;
	pConfigFile = g_OpenIniFile("data\\public\\PlayerCheckTool\\DefaultBindSocket.ini");
	KG_PROCESS_ERROR(pConfigFile);

	strSectionName = s_strDefaultBindSocketType[dwType];

	pConfigFile->GetInteger(strSectionName, "NumSocket", 0, &nNumSocket);
	for (int i = 0; i < nNumSocket; i++)
	{
		TCHAR strSocketKey[MAX_PATH];
		TCHAR strMeshKey[MAX_PATH];
		TCHAR strSocketName[MAX_PATH];
		TCHAR strDefaultMeshName[MAX_PATH];
		sprintf_s(strSocketKey,
			MAX_PATH,
			"Socket%d",
			i);
		sprintf_s(strMeshKey,
			MAX_PATH,
			"Mesh%d",
			i);
		pConfigFile->GetString(strSectionName, strSocketKey, "", strSocketName, MAX_PATH);
		pConfigFile->GetString(strSectionName, strMeshKey, "", strDefaultMeshName, MAX_PATH);
		BindPair Pair;
		Pair.strMesh = strDefaultMeshName;
		Pair.strSocket = strSocketName;
		Pair.pModel = NULL;
		g_GetModelManager()->LoadResourceFromFile(strDefaultMeshName, 
			0, 
			0, 
			(IKG3DResourceBase**)&(Pair.pModel));
		m_DefaultBindMesh[dwType].push_back(Pair);
	}
Exit1:
Exit0:
	SAFE_RELEASE(pConfigFile);
	;
}

void KSceneModelEditorView::OnBindAll()
{
	HRESULT hResult = E_FAIL;
	IEKG3DModel *pCurrentModel = NULL;
	LPCTSTR strFileName = NULL;
	GET_SCENE();
	GET_EDITOR();

	hResult = pDoc->GetCurrentModel(&pCurrentModel);
	KG_COM_PROCESS_ERROR(hResult);

	pCurrentModel->GetName(&strFileName);
	DWORD dwType = GetDefaultTypeByModelName(strFileName);
	InitDefaultBindSocket(dwType);
	
	if (!m_bBindAll)
	{
		for (size_t i = 0; i < m_DefaultBindMesh[dwType].size(); i++)
		{
			BindPair &CurrentPair = m_DefaultBindMesh[dwType][i];
			if (CurrentPair.pModel)
			{
				CurrentPair.pModel->BindToSocket(pCurrentModel, m_DefaultBindMesh[dwType][i].strSocket.c_str());
				if (strstr(m_DefaultBindMesh[dwType][i].strSocket.c_str(), "s_face"))
				{
					CurrentPair.pModel->SetFaceMotionMode();
				}
			}
		}
	}
	else
	{
		for (size_t i = 0; i < m_DefaultBindMesh[dwType].size(); i++)
		{
			BindPair &CurrentPair = m_DefaultBindMesh[dwType][i];
			if (CurrentPair.pModel)
			{
				CurrentPair.pModel->UnBindFromOther();
			}
		}
	}
Exit0:
	m_bBindAll = !m_bBindAll;
}
static void OnAnimationFileOpen(LPCSTR strFileName, void *pWnd)
{
	KSceneModelEditorView *pView = static_cast<KSceneModelEditorView*>(pWnd);
	KG_PROCESS_ERROR(pView);

	pView->OnFilterOpenFile(strFileName);
Exit0:
	;
}

void KSceneModelEditorView::OnFilterOpenFile(LPCSTR strFileName)
{
	TCHAR strExt[MAX_PATH];
	KG_PROCESS_ERROR(strFileName);

	_splitpath_s(strFileName,
		NULL,
		0,
		NULL,
		0,
		NULL,
		0,
		strExt,
		MAX_PATH);

	KSceneModelEditorFrameWnd *pWnd = static_cast<KSceneModelEditorFrameWnd*>(GetModelFrameWnd());
	KG_PROCESS_ERROR(pWnd);

	KSceneModelEditorAnimationPanel *pPanel = static_cast<KSceneModelEditorAnimationPanel*>(pWnd->GetAnimationSeekBar());
	KG_PROCESS_ERROR(pPanel);

	if (!strcmp(strExt, ".ani"))
	{
		//ani文件
		pPanel->SetAnimation(strFileName);

	}
	else if (!strcmp(strExt, ".tani"))
	{
		//tani文件
		pPanel->OnLoadTagAnimation(strFileName);
	}

Exit0:
	;
}

static const TCHAR *s_strPresetKeyword[] = 
{
	"\\部件",
	"\\模型"
};

void KSceneModelEditorView::OnFilterOpen()
{
	HRESULT hResult = E_FAIL;
	LPCTSTR strFileName = NULL;
	IEKG3DModel *pCurrentModel = NULL;
	
	if (!m_pFilterFileDlg)
	{
		TCHAR strDriver[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		GET_SCENE();
		GET_EDITOR();

		m_pFilterFileDlg = new KDlgFilterFileView;
		KG_PROCESS_ERROR(m_pFilterFileDlg);
		
		m_pFilterFileDlg->Create(KDlgFilterFileView::IDD, this);
		
		hResult = pDoc->GetCurrentModel(&pCurrentModel);
		KG_COM_PROCESS_ERROR(hResult);

		pCurrentModel->GetName(&strFileName);
		_splitpath_s(strFileName,
			strDriver,
			MAX_PATH,
			strPath,
			MAX_PATH,
			NULL,
			0,
			NULL,
			0);
		for (size_t i = 0; i < sizeof(s_strPresetKeyword) / sizeof(const TCHAR*); i++)
		{
			TCHAR *pFind = strstr(strPath, s_strPresetKeyword[i]);
			if (pFind)
			{
				pFind[0] = '\0';
				break;
			}
		}

		if (strDriver[0] == '\0')
		{
			//如果没有Driver需要加成全路径
			m_pFilterFileDlg->m_strInitDir.Format("%s%s\\动作\\*.*ani", 
				g_szDefExeDirectory,
				strPath);
		}
		else
		{
			m_pFilterFileDlg->m_strInitDir.Format("%s%s\\动作\\*.*ani", 
				strDriver,
				strPath);
		}
		m_pFilterFileDlg->UpdateData(FALSE);
		m_pFilterFileDlg->OnBnClickedButtonRefresh();
		m_pFilterFileDlg->OnOK();
		m_pFilterFileDlg->m_pfCallBack = OnAnimationFileOpen;
	}
	m_pFilterFileDlg->ShowWindow(SW_SHOW);
Exit0:
	;
}

void KSceneModelEditorView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: Add your specialized code here and/or call the base class
	KSceneModelEditorDoc* pDoc = (KSceneModelEditorDoc*) GetDocument();
	KGLOG_PROCESS_ERROR(pDoc);

	IEKG3DScene* pScene = pDoc->GetScene();
	KG_PROCESS_ERROR(pScene);

	g_GetEngineManager()->SetSoundFocusScene(pScene);

Exit0:
	CBaseView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void KSceneModelEditorView::OnMotionProcess()
{
	const static DWORD s_dwSourceInfoCount = 100;
	HRESULT hResult = E_FAIL;
	BOOL bClear = TRUE;
	TagSourceInfo SourceInfo[s_dwSourceInfoCount];
	IEKG3DAnimationTagContainer *pTagContainer = NULL;
	IEKG3DSceneModelEditor *pScene = GetDocScene();
	KDlgTagProcessFrontEnd dlgFrontEnd;
	KDlgLogicSaveProgress *pProgressBar = NULL;
	KG_PROCESS_ERROR(pScene);

	KG_PROCESS_SUCCESS(dlgFrontEnd.DoModal() != IDOK);

	pTagContainer = pScene->GetTagContainer();
	KG_PROCESS_ERROR(pTagContainer);

	pProgressBar = new KDlgLogicSaveProgress(AfxGetMainWnd());
	KG_PROCESS_ERROR(pProgressBar);

	pProgressBar->m_progressModel.SetPos(0);
	pProgressBar->CenterWindow();
	pProgressBar->ShowWindow(SW_NORMAL);

	g_AnimationTagBatchTool.m_pProgressDlg = pProgressBar;
	
	//LoadBatchToolConfig(pTagContainer);
	
	for (size_t i = 0; i < dlgFrontEnd.m_Paths.size(); i++)
	{
		CString strPath;
		g_AnimationTagBatchTool.Init(dlgFrontEnd.m_Paths[i], pTagContainer, bClear);
		bClear = FALSE;
	}
	
	KDlgAnimationTagBatchTool *pTagBatchTool = new KDlgAnimationTagBatchTool(AfxGetMainWnd(), pTagContainer);
	KG_PROCESS_ERROR(pTagBatchTool);
Exit1:
Exit0:
	if (pProgressBar)
	{
		pProgressBar->DestroyWindow();
		SAFE_DELETE(pProgressBar);
	}
	;
}

void KSceneModelEditorView::OnEnableHairLOD()
{
	IEKG3DSceneModelEditor *pScene = GetDocScene();
	KG_PROCESS_ERROR(pScene);

	s_bEnableHairLOD = !s_bEnableHairLOD;
	pScene->EnableLOD(s_bEnableHairLOD);
Exit0:
	;
}

void KSceneModelEditorView::OnUpdateHairLOD(CCmdUI *pCmdUI)
{
	pCmdUI->SetCheck(s_bEnableHairLOD);
}

HRESULT KSceneModelEditorView::LoadBatchToolConfig(IEKG3DAnimationTagContainer *pTagContainer)
{
	HRESULT hResult = E_FAIL;
	int nNumPath = 0;
	IIniFile *pConfig = g_OpenIniFile(s_strBatchToolConfigInI);
	BOOL bClear = TRUE;
	KG_PROCESS_ERROR(pConfig);

	pConfig->GetInteger(s_strBatchToolConfigSectionPath, "NumPath", 0, &nNumPath);
	for (int i = 0; i < nNumPath; i++)
	{
		TCHAR strPath[MAX_PATH];
		TCHAR strPathKey[MAX_PATH];

		sprintf_s(strPathKey,
			MAX_PATH,
			"Path%d",
			i);

		pConfig->GetString(s_strBatchToolConfigSectionPath, strPathKey, "", strPath, MAX_PATH);

		hResult = g_AnimationTagBatchTool.Init(strPath, pTagContainer, bClear);
		KG_COM_PROCESS_ERROR(hResult);

		bClear = FALSE;
	}

	hResult = S_OK;
Exit0:
	SAFE_RELEASE(pConfig);
	return hResult;
}

void KSceneModelEditorView::OnShowGrid()
{
	IEKG3DSceneModelEditor *pScene = GetDocScene();
	KG_PROCESS_ERROR(pScene);

	pScene->SetDrawGound(!pScene->IsDrawGound());
Exit0:
	;
}

void KSceneModelEditorView::OnUpdateShowGrid(CCmdUI *pCmdUI)
{
	IEKG3DSceneModelEditor *pScene = GetDocScene();
	KG_PROCESS_ERROR(pScene);

	pCmdUI->SetCheck(pScene->IsDrawGound());
Exit0:
	;
}


void KSceneModelEditorView::OnConvertSpeedTree() 
{
	HRESULT hrResult  = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	IEKG3DModel *pModel = NULL;
	IEKG3DModelTable *pModelTable = NULL;
	IEKG3DModelSpeedTree* pModelTree = NULL;
	IEKG3DModel *pModelTreeModel = NULL;
	INT nNumBones = 0;
	//TCHAR strPartFileName[MAX_PATH];
	POSITION ViewPos;
	DWORD dwType;
	CString strSpdTreeName;
	LPCSTR strBuffer;

	KSceneModelEditorFrameWnd *pFrameWnd = NULL;
	int nReturn = MessageBox("要把现有模型转换成speedtree吗?", "提示", MB_YESNO);
	KG_PROCESS_SUCCESS(nReturn != IDYES);
	GET_SCENE();
		
	hrRetCode = pDoc->GetCurrentModel(&pModel);
	KG_COM_PROCESS_ERROR (hrRetCode);

	pModel->GetType(&dwType);
	KG_PROCESS_ERROR(dwType == MESHTYPE_DEFAULT);

	hrRetCode = pModel->GetNumBones(&nNumBones);
	KG_COM_PROCESS_ERROR(hrRetCode);
	if (!nNumBones)
	{
		MessageBox("转化为SpeedTree失败，确认是带骨胳的模型.");
		KG_PROCESS_ERROR(nNumBones);
	}

	hrRetCode = g_pEngineManager->GetIEKG3DModelTable(&pModelTable);
	KG_COM_PROCESS_ERROR (hrRetCode);

	hrRetCode = pModelTable->IEGet1NewResourse(&pModelTreeModel, MESHTYPE_SPEEDTREE);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = pModelTreeModel->GetIEKG3DModelSpeedTree(&pModelTree);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);				

	hrRetCode = pModelTree->Init(pModel);
	if (FAILED(hrRetCode))
	{
		MessageBox("转化为SpeedTree失败");
	}
	KG_COM_PROCESS_ERROR(hrRetCode);

	pModel->GetName(&strBuffer);
	strSpdTreeName = strBuffer;
	strSpdTreeName.MakeLower();

	GETEDITOR();
	pEditor->OpenModel(pModelTreeModel, FALSE);
	ViewPos = pDoc->GetFirstViewPosition();
	if (ViewPos)
	{
		KSceneModelEditorView* pView = static_cast<KSceneModelEditorView*>(pDoc->GetNextView(ViewPos));
		if (pView)
		{
			pView->Zoom();
		}
	}


	int nStartPos = strSpdTreeName.Find(".mesh", 0);
	strSpdTreeName.Insert(nStartPos, "_ST");

	pDoc->SetPathName(strSpdTreeName + ".stree");

	CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
	KG_PROCESS_ERROR(pMainFrame);

	pFrameWnd = (KSceneModelEditorFrameWnd*)pMainFrame->GetActiveFrame();
	KG_PROCESS_ERROR(pFrameWnd);

	pFrameWnd->OnModelChange(0, 0);

	OnSpeedtreePanel();

Exit1:
	hrResult = S_OK;
Exit0:
	//if (FAILED(hrResult))
	//{
		//SAFE_RELEASE(pModel);
	//}
	;
}