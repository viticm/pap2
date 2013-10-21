// SceneEditorView.h : interface of the KSceneEditorView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_SCENEEDITORVIEW_H__8D364B5D_649C_479C_9E7D_86649A621095__INCLUDED_)
#define AFX_SCENEEDITORVIEW_H__8D364B5D_649C_479C_9E7D_86649A621095__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
#include "SceneEditorDoc.h"
#include "KSceneSceneEditorPanelDialog.h"
//#include "KDlgLightProperty.h"
#include "KSceneSceneEditorDialogPaintTexture.h"
#include "KSceneEditorTopViewDialog.h"
#include "KSceneEditorLayerDialog.h"
#include "KSceneEditorVisibleSetDialog.h"
#include "BaseView.h"
#include "KGDlgSnapTool.h"
#include "KGNpcPropertyEditerDlg.h"


//(by dengzhihui 2006年7月6日 18:40:00
#include <XTToolkitPro.h> // Xtreme Toolkit Pro component library
//)

#define EDITOR_CONFIG_FILE "Config.ini"

class KSceneEditorCntrItem;
class KSceneSceneEditorDialogObjectMgr;
class KSceneSceneEditorDialogDirMgr;
class IEKG3DSceneSceneEditor;
class KSceneSettingDialog;
class KSceneEditorPVSSetupDialog;
class KSceneEditorAntiPortalSetupDialog;
class KSceneEditorRoadSetup;
class KSceneEditorDetialMapDialog;
class KSceneEditorWallSetupDialog;
class KSceneEditorRiver;
class KGShortcutKeyDlg;
//没有检查到任何引用
//struct PvsSelectFilter : public ISelectFilter //<ISelectFilter Refactor>
//{
//    void Attach(IEKG3DRepresentPVS* pPvs);
//    virtual bool operator() (IEKG3DSceneEntity* pEntity);
//    IEKG3DRepresentPVS* m_pPvs;
//};

class KSceneEditorView : public CBaseView
{

protected: // create from serialization only
	KSceneEditorView();
	DECLARE_DYNCREATE(KSceneEditorView)

// Attributes
public:
	KSceneEditorDoc* GetDocument();
	// m_pSelection holds the selection to the current KSceneEditorCntrItem.
	// For many applications, such a member variable isn't adequate to
	//  represent a selection, such as a multiple selection or a selection
	//  of objects that are not KSceneEditorCntrItem objects.  This selection
	//  mechanism is provided just to help you get started.

	// TODO: replace this selection mechanism with one appropriate to your app.
	KSceneEditorCntrItem* m_pSelection;

	int	 m_nOutputWindowID;
// Operations
public:
	CMenu                    m_MenuPop;
	BOOL                     m_bAutoSave;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KSceneEditorView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	void OnRButtonUp(UINT nFlags, CPoint point) ;
    void OnLButtonUp(UINT nFlags, CPoint point) ;

    void OnModifyPvs(IEKG3DSceneEntity* pEngity);

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~KSceneEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(KSceneEditorView)
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnDestroy();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnShowPanel();
	afx_msg void OnUpdateShowPanel(CCmdUI* pCmdUI);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnDetalground();
	afx_msg void OnUpdateDetalground(CCmdUI* pCmdUI);
	afx_msg void OnGroup();
	afx_msg void OnUpdateGroup(CCmdUI* pCmdUI);
	afx_msg void OnUngroup();
	afx_msg void OnUpdateUngroup(CCmdUI* pCmdUI);
	afx_msg void OnMotionblur();
	afx_msg void OnUpdateMotionblur(CCmdUI* pCmdUI);
	afx_msg void OnAutosave();
	afx_msg void OnUpdateAutosave(CCmdUI* pCmdUI);
	afx_msg void OnOptimizesmallentity();
	afx_msg void OnUpdateOptimizesmallentity(CCmdUI* pCmdUI);
	afx_msg void OnScenelighting();
	afx_msg void OnUpdateScenelighting(CCmdUI* pCmdUI);
	afx_msg void OnRefreshTerrain();
	afx_msg void OnOutputSmallMap();
	afx_msg void OnRefreshTexture();
	afx_msg void OnSelect();
	void OnModifySoundBall(IEKG3DRepresentObject *pSoundBall);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnRenderSoundBallRange();
    afx_msg void OnLock();
    afx_msg void OnUnLock();
	afx_msg void OnScenesetting();
	afx_msg void OnLargeterrain();
	afx_msg void OnShowobscale();
	afx_msg void OnUpdateShowobscale(CCmdUI *pCmdUI);
	afx_msg void OnShwoproperties();
	afx_msg void OnEditTrafficPoint();
    afx_msg void OnDoodaScript();
    afx_msg void OnSelNpcRepresnet();
    afx_msg void OnEditObjPropertys();
    afx_msg void OnSetPointColor();
    afx_msg LRESULT OnColorChanged(WPARAM wParam, LPARAM lParam);
    afx_msg void OnConverPvs();
    afx_msg void OnAddPortal();
    afx_msg void OnSavePvs();
    afx_msg void OnSaveAsPvs();
	afx_msg void OnPopMenuMove();
	afx_msg void OnPopMenuSelect();
	afx_msg void OnPopMenuRotate();
	afx_msg void OnPopMenuScale();
	afx_msg void OnPopMenuSelectByName();
	afx_msg void OnRefreshshader();
	afx_msg void OnEngineCommand();


public:
	//KDlgLightProperty* m_pLightProperty;
	KSceneSceneEditorPanelDialog	m_DialogPanel;
	KSceneSceneEditorDialogObjectMgr* m_lpDialogObjectMgr;
	//KSceneSceneEditorDialogPaintTexture	m_PaintTextureDialog;
	KSceneSceneEditorDialogDirMgr*   m_lpDialogDirMgr;

	KSceneEditorTopViewDialog m_DialogTopView;
    KGDlgSnapTool m_DialogSnapTool;
	KSceneEditorLayerDialog m_DialogLayer;
	KSceneEditorVisibleSetDialog m_DialogVisibleSet;

	KSceneEditorPVSSetupDialog* m_lpDialogPVS;
	KSceneEditorAntiPortalSetupDialog* m_lpDialogAntiPortal;
	KSceneEditorRoadSetup* m_lpDialogRoad;//道路设置工具
	KSceneEditorRiver*     m_lpDialogRiver;//河流工具
	KSceneEditorDetialMapDialog* m_lpDialogDetailMap;
	KSceneEditorWallSetupDialog* m_lpDialogWall;
//	KGNpcPropertyEditerDlg* m_lpDialogNpcProperty;
	KGShortcutKeyDlg* m_lpDialogKey;
    D3DCOLORVALUE m_ColorPointLightEdit;
private:
	CDialog*	m_pSceneSettingDialog;//改成无模式对话框 Last modify by Chen Tianhong: 2007-6-6 16:04:37

public:
	IEKG3DSceneSceneEditor* GetDocScene();    
	HRESULT FrameMove();
	//void	ONLBUTTONDOWN (WPARAM wParam, LPARAM lParam);

	DWORD   m_dwState;
	DWORD   m_dwSubState;
	BOOL	m_bEditing;	
	afx_msg void OnNnwMap();
	afx_msg void OnUpdateModifyModel(CCmdUI* pCmdUI);
	afx_msg void OnModifyModel();
	afx_msg void OnModifyLight();
    afx_msg void OnShowBsp();

public:
	afx_msg void OnUnitSeclectedEntity();
	afx_msg void OnDirMgr();
	afx_msg void OnUpdateModelunit(CCmdUI *pCmdUI);
	afx_msg void OnModelununit();
	afx_msg void OnUpdateModelununit(CCmdUI *pCmdUI);
	afx_msg void OnPaintTexture();
    afx_msg void OnShockwaveEntity();

	//(by dengzhihui 2006年7月6日 18:35:04
	afx_msg void OnEditUndo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUndoListBoxControlSelChange(NMHDR* pNMHDR, LRESULT* pRes);
	afx_msg void OnUndoListBoxControlPoup(NMHDR* pNMHDR, LRESULT* pRes);
	afx_msg void OnUpdateEditUndo(CCmdUI *pCmdUI);
	afx_msg void OnEditUndo();

	afx_msg void OnEditRedo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnRedoListBoxControlSelChange(NMHDR* pNMHDR, LRESULT* pRes);
	afx_msg void OnRedoListBoxControlPoup(NMHDR* pNMHDR, LRESULT* pRes);
	afx_msg void OnUpdateEditRedo(CCmdUI *pCmdUI);
	afx_msg void OnEditRedo();
	
	CXTPControlStatic* FindInfoControl(CXTPControl* pControl);

	LRESULT OnDockingPaneNotify(WPARAM wParam, LPARAM lParam);
	LRESULT OnActionExected(WPARAM wParam, LPARAM lParam);
	CXTPDockingPaneManager m_paneManager;
	CXTPDockingPane* m_pPane;
	BOOL m_preFullScrIsClosed;
	INT m_nOldXPos, m_nOldYPos;

	inline DWORD GetEditState(IEKG3DSceneSceneEditor* pEditor);
    afx_msg void OnZoomCoordBig();
    afx_msg void OnZoomCoordLit();

    afx_msg void OnSetCoord();
    afx_msg void OnSetLink();

	afx_msg void OnNormalMapGen();
	afx_msg void OnViewRightbat();
	afx_msg void OnUpdateViewRightbat(CCmdUI *pCmdUI);
	afx_msg void OnTopview();
	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnAddSelTexture();
	afx_msg void OnRemoveSelTexture();
	//////////////////////////////////////////////////////////////////////////
	afx_msg void OnShowprofile();
    afx_msg void OnShowVideoInfo();
    afx_msg void OnOpenAPTPlayObjMode();
	afx_msg void OnUpdateShowprofile(CCmdUI *pCmdUI);
    afx_msg void OnShowsnaptool();
	afx_msg void OnLayertool();
	afx_msg void OnModifyvisibleset();
	afx_msg void OnAddmeasuretool();
	afx_msg void OnUpdateAddmeasuretool(CCmdUI *pCmdUI);
    afx_msg void OnResetGroupPose();
	afx_msg void OnGroupsaveas();

	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	afx_msg void OnStartrecord();
	afx_msg void OnUpdateStartrecord(CCmdUI *pCmdUI);
	afx_msg void OnStoprecord();
	afx_msg void OnUpdateStoprecord(CCmdUI *pCmdUI);

	afx_msg void OnPlayerMode();
	afx_msg void OnPlayerMode(CCmdUI *pCmdUI);
    
    afx_msg void OnScreenShot();

	afx_msg void OnPlayerSpeedUp();
	afx_msg void OnPlayerSpeedDown();

	afx_msg void OnPlayerModeUpdate(CCmdUI *pCmdUI);

	afx_msg void OnModeSelectUpdate(CCmdUI *pCmdUI);
	afx_msg void OnModeScaleUpdate(CCmdUI *pCmdUI);
	afx_msg void OnModeMoveUpdate(CCmdUI *pCmdUI);
	afx_msg void OnModeRotateUpdate(CCmdUI *pCmdUI);

	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();

	afx_msg void OnPlayCamareAni();	
	afx_msg void OnDeleteSelectedObject();
	
	//afx_msg BOOL OnMouseWheel(UINT, short, CPoint);

	afx_msg void OnActionUp();	
	afx_msg void OnActionDown();	
	afx_msg void OnActionLeft();	
	afx_msg void OnActionRight();	
	afx_msg void OnActionPageUp();	
	afx_msg void OnActionPageDown();	
	
	afx_msg void OnActionAdd();	//'['
	afx_msg void OnActionDec();	//']'

	afx_msg void OnHideObject();	
	afx_msg void OnHideTerrain();	
	afx_msg void OnHideGrass();	
	afx_msg void OnHideFrutex();
	afx_msg void OnHideNPCAttackRange();

	afx_msg void OnShowObjectManager();
	afx_msg void OnShowLightProperty();
	
	afx_msg void OnHome();	
	afx_msg void OnEnd();	
	
	afx_msg void OnAddKeyFrame();	
	afx_msg void OnDropObj();	

	afx_msg void OnGetSelectedObjInHand();	
public:
	afx_msg void OnLogicalpoly();
	afx_msg void OnAddmap();
	afx_msg void OnLoadDefaultMap();
	afx_msg void OnSoundballedit();
	afx_msg void OnPlayscenesound();
	afx_msg void OnUpdatePlayscenesound(CCmdUI *pCmdUI);
	afx_msg void OnRendersound();
	afx_msg void OnUpdateRenderSoundBallRange(CCmdUI *pCmdUI);
	afx_msg void OnUpdateRendersound(CCmdUI *pCmdUI);
	afx_msg void OnEnablePVS();
	afx_msg void OnIsRayUseBoneBox();
	//afx_msg void OnPvssetup();
	afx_msg void OnAntiPortalSetup();
	afx_msg void OnTerrainselect();
	afx_msg void OnRoad();
	afx_msg void OnDetailMap();
	afx_msg void OnWallsetup();
	afx_msg void OnRiver();
	afx_msg void OnMenuentitynum();
	afx_msg void OnMoveNPC();
protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
public:
	afx_msg void OnShortcutkey();
};



#ifndef _DEBUG  // debug version in SceneEditorView.cpp
inline KSceneEditorDoc* KSceneEditorView::GetDocument()
   { return (KSceneEditorDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SCENEEDITORVIEW_H__8D364B5D_649C_479C_9E7D_86649A621095__INCLUDED_)

