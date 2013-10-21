// BaseView.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "BaseView.h"
#include "KEditorDocumentBase.h"
//#include "kg3dsceneeditorbase.h"
#include "IEEditor.h"
#include "KSceneEditBaseSelectDialog.h"
#include "IEEditor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


#define GETEDITOR() \
	IEKG3DSceneEditorBase* pEditor = GetDocScene();\
	_ASSERTE(pEditor); \
	if(!pEditor) return;

#define GETEDITORBOOL() \
	IEKG3DSceneEditorBase* pEditor = GetDocScene();\
	_ASSERTE(pEditor); \
	if(!pEditor) return FALSE;

// CBaseView

IMPLEMENT_DYNCREATE(CBaseView, CView)

CBaseView::CBaseView()
{

}

CBaseView::~CBaseView()
{
}

BEGIN_MESSAGE_MAP(CBaseView, CView)
	ON_COMMAND(ID_MOVE, OnPopMenuMove)
	ON_COMMAND(ID_ROTATE, OnPopMenuRotate)
	ON_COMMAND(ID_SCALE, OnPopMenuScale)
	ON_COMMAND(ID_SELECT,OnPopMenuSelect)
	ON_UPDATE_COMMAND_UI(ID_SELECT, &CBaseView::OnModeSelectUpdate)
	ON_UPDATE_COMMAND_UI(ID_SCALE, &CBaseView::OnModeScaleUpdate)
	ON_UPDATE_COMMAND_UI(ID_MOVE, &CBaseView::OnModeMoveUpdate)
	ON_UPDATE_COMMAND_UI(ID_ROTATE, &CBaseView::OnModeRotateUpdate)

	ON_COMMAND(ID_ZOOMTO_OBJ, &CBaseView::OnZoomToObj)
	ON_UPDATE_COMMAND_UI(ID_ZOOMTO_OBJ, &CBaseView::OnEditorModeUpdate)

	ON_COMMAND(ID_SHOW_OBJECTSELECTDLG, &CBaseView::OnShowObjectSelectDlg)
	ON_UPDATE_COMMAND_UI(ID_SHOW_OBJECTSELECTDLG, &CBaseView::OnEditorModeUpdate)
	//ON_WM_MOUSEWHEEL()

END_MESSAGE_MAP()
 

// CBaseView drawing

void CBaseView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}


// CBaseView diagnostics

#ifdef _DEBUG
void CBaseView::AssertValid() const
{
	CView::AssertValid();
}
#endif //_DEBUG

BOOL CBaseView::PreTranslateMessage(MSG* pMsg)
{
	if (m_InputProxy.CheckInputEvent(pMsg))
	{
		return TRUE;
	}

	return CView::PreTranslateMessage(pMsg);
}

void CBaseView::OnEditorModeUpdate(CCmdUI *pCmdUI)
{
	IEKG3DSceneEditorBase* pEditor = GetDocScene();
	if (pEditor)
	{
		pCmdUI->Enable(pEditor->GetEditing());
	}
}

IEKG3DSceneEditorBase* CBaseView::GetDocScene()
{
	KEditorDocumentBase* pDoc =  dynamic_cast<KEditorDocumentBase*>(GetDocument());
	_ASSERTE(pDoc);
	if (pDoc)
	{
		IEKG3DScene* p3DScene = pDoc->GetScene();
		IEKG3DSceneEditorBase* pEditor = NULL;//reinterpret_cast<IEKG3DSceneEditorBase*> (p3DScene);
		p3DScene->GetSceneEditorBase(&pEditor);
		_ASSERTE(pEditor);
		return pEditor;
	}
	return NULL;
}
// CBaseView message handlers

void CBaseView::OnPopMenuMove()
{
	GETEDITOR();
	pEditor->SetEditState(SCENESTATE_MOVE); //m_dwEditState = SCENESTATE_MOVE;
	IEKG3DSceneSelectBase* pSelector = NULL;
	//pEditor->GetSceneSelector(&pSelector);
	//_ASSERTE(pSelector);
	//pSelector->OnToggleStateMove();//<OnToggleStateMove Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
}
void CBaseView::OnPopMenuRotate()
{
	GETEDITOR();
	pEditor->SetEditState(SCENESTATE_ROTATE);//m_dwEditState = SCENESTATE_ROTATE;
	//IEKG3DSceneSelectBase* pSelector = NULL;
	//pEditor->GetSceneSelector(&pSelector);
	//_ASSERTE(pSelector);
	//pSelector->OnToggleStateRotation();//<OnToggleStateRotation Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
}
void CBaseView::OnPopMenuScale()
{
	GETEDITOR();
	pEditor->SetEditState(SCENESTATE_SCALE);//pEditor->m_dwEditState = SCENESTATE_SCALE;
   /* IEKG3DSceneSelectBase* pSelector = NULL;
    pEditor->GetSceneSelector(&pSelector);
    _ASSERTE(pSelector);
	pSelector->OnToggleStateScaling();*///<OnToggleStateScaling Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
}

void CBaseView::OnPopMenuSelect()
{
	GETEDITOR();
	pEditor->SetEditState(SCENESTATE_SELECT);//pEditor->m_dwEditState = SCENESTATE_SELECT;
	//pEditor->OnToggleStateSelect();
}

inline DWORD GetEditState(IEKG3DSceneEditorBase* pBase){\
DWORD dwState = 0;\
pBase->GetEditState(&dwState);\
return dwState;\
}

void CBaseView::OnModeSelectUpdate(CCmdUI *pCmdUI)
{
	IEKG3DSceneEditorBase* pEditor = GetDocScene();
	_ASSERTE(pEditor);
	if (pEditor)
	{
		pCmdUI->SetCheck(GetEditState(pEditor) == SCENESTATE_SELECT);// && pEditor->GetEditing()
		pCmdUI->Enable(pEditor->GetEditing());
	}
}
void CBaseView::OnModeScaleUpdate(CCmdUI *pCmdUI)
{
	IEKG3DSceneEditorBase* pEditor = GetDocScene();
	_ASSERTE(pEditor);
	if (pEditor)
	{
		pCmdUI->SetCheck(GetEditState(pEditor) == SCENESTATE_SCALE);
		pCmdUI->Enable(pEditor->GetEditing());
	}
}
void CBaseView::OnModeMoveUpdate(CCmdUI *pCmdUI)
{
	IEKG3DSceneEditorBase* pEditor = GetDocScene();
	_ASSERTE(pEditor);
	if (pEditor)
	{
		pCmdUI->SetCheck(GetEditState(pEditor) == SCENESTATE_MOVE);
		pCmdUI->Enable(pEditor->GetEditing());
	}
}
void CBaseView::OnModeRotateUpdate(CCmdUI *pCmdUI)
{
	IEKG3DSceneEditorBase* pEditor = GetDocScene();
	_ASSERTE(pEditor);
	if (pEditor)
	{
		pCmdUI->SetCheck(GetEditState(pEditor) == SCENESTATE_ROTATE);
		pCmdUI->Enable(pEditor->GetEditing());
	}
}

void CBaseView::OnZoomToObj()
{
	IEKG3DSceneEditorBase* pEditor = GetDocScene();
	_ASSERTE(pEditor);
	if(pEditor)
		pEditor->ExecAction(EXEACTION_ZOOM_TO_OBJECT, 1, 0, 0);
}
//显示物件选择对话框2
void CBaseView::OnShowObjectSelectDlg()
{
	GETEDITOR();
	KSceneEditBaseSelectDialog Dlg;
	Dlg.SetEditor(pEditor);
	Dlg.DoModal();
}

//BOOL CBaseView::OnMouseWheel(UINT, short nDelta, CPoint pt)
//{
//	GETEDITORBOOL();
//	pEditor->ZoomView(nDelta, pt);
//	return TRUE;
//}