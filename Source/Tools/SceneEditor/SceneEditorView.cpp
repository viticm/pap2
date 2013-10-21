// SceneEditorView.cpp : implementation of the KSceneEditorView class
//

#include "stdafx.h"
#include "SceneEditor.h"

#include "SceneEditorDoc.h"
#include "CntrItem.h"
#include "SceneEditorView.h"
//#include "kg3denginemanager.h"
//#include "SceneEditorCommon.h"//by dengzhihui Refactor 2006年11月6日

#include "ChildFrm.h"
//(by dengzhihui 2006年7月5日 16:39:38
#include "MainFrm.h"
#include ".\CommonPro.Include\xtpdepreciated.h"
//)
#include <direct.h>
#include "KEditorSettingDialog.h"
#include ".\sceneeditorview.h"
#include "KSceneSettingDialog.h"
#include "SceneEditor.h"
#include "KPvsEditorDoc.h"



#include "SceneEditBase.h"

#include "KSceneSFXEditorDoc.h"

#include "KSceneSceneEditorDialogObjectMgr.h"
#include "MeshEditorDoc.h"

#include "SO3Represent/SO3RepresentInterface.h"
#include "KSceneSceneEditorDialogDirMgr.h"
#include "KScene.h"
#include "Global.h"
#include "KRegion.h"
#include "SO3GlobalDef.h "
#include "SO3Represent/SO3RepresentInterface.h"

#include "KGNpcPropertyEditerDlg.h"
#include "KGMemDateEditDlg.h"
//#include "KG3DRepresentObjectVisibleEntitySet.h"
#include "mesheditorview.h"

#include "IEKG3DSceneOutputWnd.h"
#include "KGSelRepresentNPCDlg.h"

#include "KSceneDialogLogicalPoly.h"

#include "KG3DSceneLogicalEditorBase.h"
#include "KSceneCommonParamDialog.h"
#include "KSceneEditorSoundBallDialog.h"
#include "IEKG3DTextureTable.h"

#include "KSceneEditorPVSSetupDialog.h"
#include "KSceneEditorAntiPortalSetupDialog.h"
#include "KGTrafficPropertyEditerDlg.h"
#include "KSceneEditorRoadSetup.h"
#include "IEKG3DEnvEffLight.h"
#include "iekg3dmesh.h"
#include "IEKG3DSFX.h"
#include "IEEditor.h"
#include "KSceneEditorDetialMapDialog.h"

#include "KSceneEditorWallSetupDialog.h"
#include "KSceneEditorRiver.h"
#include "KGDlgSnapTool.h"
#include "KSceneSettingDialogV2.h"
#include "KSceneHelperFuncs.h"
#include "KGShortcutKeyDlg.h"
#include "KGNormalMapCreatorDlg.h"
#include "KSceneEditorDialogCheckRelive.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// KSceneEditorView

IMPLEMENT_DYNCREATE(KSceneEditorView, CBaseView)

BEGIN_MESSAGE_MAP(KSceneEditorView, CBaseView)
//{{AFX_MSG_MAP(KSceneEditorView)
	ON_WM_DESTROY()
	ON_WM_SETFOCUS()
	ON_WM_SIZE()
	ON_WM_CREATE()
	ON_COMMAND(ID_SHOW_PANEL, OnShowPanel)
	ON_UPDATE_COMMAND_UI(ID_SHOW_PANEL, OnUpdateShowPanel)
	ON_WM_TIMER()
	ON_COMMAND(ID_DETALGROUND, OnDetalground)
	ON_UPDATE_COMMAND_UI(ID_DETALGROUND, OnUpdateDetalground)
	ON_COMMAND(ID_GROUP, OnGroup)
	ON_UPDATE_COMMAND_UI(ID_GROUP, OnUpdateGroup)
	ON_COMMAND(ID_UNGROUP1, OnUngroup)
	ON_UPDATE_COMMAND_UI(ID_UNGROUP1, OnUpdateUngroup)
	ON_COMMAND(ID_MOTIONBLUR, OnMotionblur)
	ON_UPDATE_COMMAND_UI(ID_MOTIONBLUR, OnUpdateMotionblur)
	//ON_COMMAND(ID_AUTOSAVE, OnAutosave)
	//ON_UPDATE_COMMAND_UI(ID_AUTOSAVE, OnUpdateAutosave)
	ON_COMMAND(ID_OPTIMIZESMALLENTITY, OnOptimizesmallentity)
	ON_UPDATE_COMMAND_UI(ID_OPTIMIZESMALLENTITY, OnUpdateOptimizesmallentity)
	ON_COMMAND(ID_SCENELIGHTING, OnScenelighting)
	ON_UPDATE_COMMAND_UI(ID_SCENELIGHTING, OnUpdateScenelighting)
	//ON_COMMAND(ID_REFRESHGROUND,OnRefreshTerrain)
	//ON_COMMAND(ID_OUTSMALLMAP,OnOutputSmallMap)
	ON_COMMAND(ID_RELOADTEXTURE,OnRefreshTexture)
	//ON_COMMAND(ID_SELECT,OnSelect)
	ON_WM_RBUTTONUP()
	//}}AFX_MSG_MAP
	// Standard printing commands
	//ON_COMMAND(ID_FILE_PRINT, CBaseView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_DIRECT, CBaseView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_PREVIEW, CBaseView::OnFilePrintPreview)
	ON_COMMAND(ID_SCENESETTING, OnScenesetting)
    ON_COMMAND(ID_LOCK, OnLock)
    ON_COMMAND(ID_UNLOCK, OnUnLock)
	ON_COMMAND(ID_RENDERSOUNDBALLRANGE, OnRenderSoundBallRange)
	ON_UPDATE_COMMAND_UI(ID_RENDERSOUNDBALLRANGE, &KSceneEditorView::OnUpdateRenderSoundBallRange)
	//ON_COMMAND(ID_LARGETERRAIN, OnLargeterrain)
	ON_COMMAND(ID_SHOWOBSCALE, OnShowobscale)
	ON_UPDATE_COMMAND_UI(ID_SHOWOBSCALE, OnUpdateShowobscale)
	ON_COMMAND(ID_SHWOPROPERTIES, OnShwoproperties)
	ON_COMMAND(ID_TRAFFICPOINT,OnEditTrafficPoint)
    ON_COMMAND(ID_DOODA, OnDoodaScript)
    ON_COMMAND(ID_SEL_NPC_REPRESENT, OnSelNpcRepresnet)
    ON_COMMAND(ID_OBJ_PROPERTYS,  OnEditObjPropertys)
    ON_COMMAND(ID_SET_POINTLIGHT_COLOR, OnSetPointColor)
    ON_MESSAGE(WM_COLORPOINTER_CHANGE, OnColorChanged)
    ON_COMMAND(ID_CONVER_PVS, OnConverPvs)
    ON_COMMAND(ID_ADD_PORTAL, OnAddPortal)
    ON_COMMAND(ID_SAVE_PVS, OnSavePvs)
    ON_COMMAND(ID_SAVE_AS_PVS, OnSaveAsPvs)
	ON_COMMAND(ID_MODIFY, OnModifyModel)
    ON_COMMAND(ID_SHOW_BSP, OnShowBsp)
    ON_COMMAND(ID_SHOT, OnScreenShot)
	ON_UPDATE_COMMAND_UI(ID_MODIFY, OnUpdateModifyModel)
	ON_COMMAND(ID_MODIFYLIGHT, OnModifyLight)
	//ON_COMMAND(ID_MOVE, OnPopMenuMove)
	//ON_COMMAND(ID_ROTATE, OnPopMenuRotate)
	//ON_COMMAND(ID_SCALE, OnPopMenuScale)
	ON_COMMAND(ID_SELECTBYNAME, OnPopMenuSelectByName)
	ON_COMMAND(ID_MODELUNIT, &KSceneEditorView::OnUnitSeclectedEntity)
	ON_UPDATE_COMMAND_UI(ID_MODELUNIT, &KSceneEditorView::OnUpdateModelunit)
	ON_COMMAND(ID_MODELUNUNIT, &KSceneEditorView::OnModelununit)
	ON_UPDATE_COMMAND_UI(ID_MODELUNUNIT, &KSceneEditorView::OnUpdateModelununit)
	ON_COMMAND(ID_33093, &KSceneEditorView::OnPaintTexture)
	ON_COMMAND(ID_DIRMGR, OnDirMgr)

    ON_COMMAND(ID_SHOCKWAVE_ENTITY, &KSceneEditorView::OnShockwaveEntity)

	//(by dengzhihui 2006年7月6日 18:34:11
	ON_XTP_EXECUTE(ID_EDIT_UNDO, OnEditUndo)
	ON_NOTIFY(XTP_LBN_SELCHANGE, ID_EDIT_UNDO, OnUndoListBoxControlSelChange)
	ON_NOTIFY(XTP_LBN_POPUP, ID_EDIT_UNDO, OnUndoListBoxControlPoup)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, &KSceneEditorView::OnUpdateEditUndo)
	ON_COMMAND(ID_EDIT_UNDO, &KSceneEditorView::OnEditUndo)

	ON_XTP_EXECUTE(ID_EDIT_REDO, OnEditRedo)
	ON_NOTIFY(XTP_LBN_SELCHANGE, ID_EDIT_REDO, OnRedoListBoxControlSelChange)
	ON_NOTIFY(XTP_LBN_POPUP, ID_EDIT_REDO, OnRedoListBoxControlPoup)
	ON_UPDATE_COMMAND_UI(ID_EDIT_REDO, &KSceneEditorView::OnUpdateEditRedo)
	ON_COMMAND(ID_EDIT_REDO, &KSceneEditorView::OnEditRedo)
	
	ON_MESSAGE(XTPWM_DOCKINGPANE_NOTIFY, OnDockingPaneNotify)
	ON_MESSAGE(WM_ACTIONEXECUTED, OnActionExected)
	
    ON_COMMAND(ID_NORMALMAP_GEN, &KSceneEditorView::OnNormalMapGen)
	ON_COMMAND(ID_VIEW_RIGHTBAT, &KSceneEditorView::OnViewRightbat)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RIGHTBAT, &KSceneEditorView::OnUpdateViewRightbat)
	ON_COMMAND(ID_TOPVIEW, &KSceneEditorView::OnTopview)
	//////////////////////////////////////////////////////////////////////////
	ON_COMMAND(ID_ADD_TEX, &KSceneEditorView::OnAddSelTexture)
	ON_COMMAND(ID_REMOVE_TEX, &KSceneEditorView::OnRemoveSelTexture)
	//////////////////////////////////////////////////////////////////////////
	ON_COMMAND(ID_SHOWPROFILE, &KSceneEditorView::OnShowprofile)
    ON_COMMAND(ID_SHOW_VIDEO_INFO, &KSceneEditorView::OnShowVideoInfo)
    ON_COMMAND(ID_GATE_APT_PLAY_OBJ, &KSceneEditorView::OnOpenAPTPlayObjMode)
	ON_UPDATE_COMMAND_UI(ID_SHOWPROFILE, &KSceneEditorView::OnUpdateShowprofile)
    ON_COMMAND(ID_COMPUTESCENESNAP, &KSceneEditorView::OnShowsnaptool)
	ON_COMMAND(ID_LAYERTOOL, &KSceneEditorView::OnLayertool)
	ON_COMMAND(ID_MODIFYVISIBLESET, &KSceneEditorView::OnModifyvisibleset)
	ON_COMMAND(ID_ADDMEASURETOOL, &KSceneEditorView::OnAddmeasuretool)
	ON_UPDATE_COMMAND_UI(ID_ADDMEASURETOOL, &KSceneEditorView::OnUpdateAddmeasuretool)
    ON_COMMAND(ID_RESET_GROUP_POSE, &KSceneEditorView::OnResetGroupPose)
	ON_COMMAND(ID_GROUPSAVEAS, &KSceneEditorView::OnGroupsaveas)
	ON_COMMAND(ID_STARTRECORD, &KSceneEditorView::OnStartrecord)
	ON_UPDATE_COMMAND_UI(ID_STARTRECORD, &KSceneEditorView::OnUpdateStartrecord)
	ON_COMMAND(ID_STOPRECORD, &KSceneEditorView::OnStoprecord)
	ON_UPDATE_COMMAND_UI(ID_STOPRECORD, &KSceneEditorView::OnUpdateStoprecord)
	ON_COMMAND(ID_PLAYERMODE, &KSceneEditorView::OnPlayerMode)
	ON_UPDATE_COMMAND_UI(ID_PLAYERMODE, &KSceneEditorView::OnPlayerMode)

	ON_COMMAND(ID_ROLE_SPEEDUP, &KSceneEditorView::OnPlayerSpeedUp)
	ON_UPDATE_COMMAND_UI(ID_ROLE_SPEEDUP, &KSceneEditorView::OnPlayerModeUpdate)
	ON_COMMAND(ID_ROLE_SPEEDDOWN, &KSceneEditorView::OnPlayerSpeedDown)
	ON_UPDATE_COMMAND_UI(ID_ROLE_SPEEDDOWN, &KSceneEditorView::OnPlayerModeUpdate)
	//ON_COMMAND(ID_SETCAMAREDIR_RESET, &KSceneEditorView::OnCamareReset)
	//ON_UPDATE_COMMAND_UI(ID_SETCAMAREDIR_RESET, &KSceneEditorView::OnPlayerModeUpdate)
	//ON_COMMAND(ID_SETCAMAREDIR_BACK, &KSceneEditorView::OnCamareGoBack)
	//ON_UPDATE_COMMAND_UI(ID_SETCAMAREDIR_BACK, &KSceneEditorView::OnPlayerModeUpdate)

	/*ON_UPDATE_COMMAND_UI(ID_SELECT, &KSceneEditorView::OnModeSelectUpdate)
	ON_UPDATE_COMMAND_UI(ID_SCALE, &KSceneEditorView::OnModeScaleUpdate)
	ON_UPDATE_COMMAND_UI(ID_MOVE, &KSceneEditorView::OnModeMoveUpdate)
	ON_UPDATE_COMMAND_UI(ID_ROTATE, &KSceneEditorView::OnModeRotateUpdate)*/
	
	ON_COMMAND(ID_EDIT_COPY, &KSceneEditorView::OnEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, &CBaseView::OnEditorModeUpdate)
	ON_COMMAND(ID_EDIT_PASTE, &KSceneEditorView::OnEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, &CBaseView::OnEditorModeUpdate)	

    ON_COMMAND(ID_COPY, &KSceneEditorView::OnEditCopy)
    ON_UPDATE_COMMAND_UI(ID_COPY, &CBaseView::OnEditorModeUpdate)
    ON_COMMAND(ID_PAST, &KSceneEditorView::OnEditPaste)
    ON_UPDATE_COMMAND_UI(ID_PAST, &CBaseView::OnEditorModeUpdate)	

	//ON_UPDATE_COMMAND_UI(ID_ZOOMTO_OBJ, &CBaseView::OnEditorModeUpdate)	
	
	ON_COMMAND(ID_PLAYCAMAREANI, &KSceneEditorView::OnPlayCamareAni)	
	ON_UPDATE_COMMAND_UI(ID_PLAYCAMAREANI, &CBaseView::OnEditorModeUpdate)
	
	ON_COMMAND(ID_DELOBJ, &KSceneEditorView::OnDeleteSelectedObject)
	ON_UPDATE_COMMAND_UI(ID_DELOBJ, &CBaseView::OnEditorModeUpdate)

    ON_COMMAND(ID_DELETE, &KSceneEditorView::OnDeleteSelectedObject)
    ON_UPDATE_COMMAND_UI(ID_DELETE, &CBaseView::OnEditorModeUpdate)

	ON_WM_MOUSEWHEEL()

	ON_COMMAND(ID_ACTION_UP, &KSceneEditorView::OnActionUp)
	ON_UPDATE_COMMAND_UI(ID_ACTION_UP, &CBaseView::OnEditorModeUpdate)
	ON_COMMAND(ID_ACTION_DOWN, &KSceneEditorView::OnActionDown)
	ON_UPDATE_COMMAND_UI(ID_ACTION_DOWN, &CBaseView::OnEditorModeUpdate)
	
	ON_COMMAND(ID_ACTION_LEFT, &KSceneEditorView::OnActionLeft)	
	ON_UPDATE_COMMAND_UI(ID_ACTION_LEFT, &CBaseView::OnEditorModeUpdate)
	ON_COMMAND(ID_ACTION_RIGHT, &KSceneEditorView::OnActionRight)
	ON_UPDATE_COMMAND_UI(ID_ACTION_RIGHT, &CBaseView::OnEditorModeUpdate)
	
	ON_COMMAND(ID_ACTION_PGUP, &KSceneEditorView::OnActionPageUp)
	ON_UPDATE_COMMAND_UI(ID_ACTION_PGUP, &CBaseView::OnEditorModeUpdate)
	ON_COMMAND(ID_ACTION_PGDN, &KSceneEditorView::OnActionPageDown)
	ON_UPDATE_COMMAND_UI(ID_ACTION_PGDN, &CBaseView::OnEditorModeUpdate)

	ON_COMMAND(ID_ACTION_ADD, &KSceneEditorView::OnActionAdd)
	ON_UPDATE_COMMAND_UI(ID_ACTION_ADD, &CBaseView::OnEditorModeUpdate)
	ON_COMMAND(ID_ACTION_DEC, &KSceneEditorView::OnActionDec)
	ON_UPDATE_COMMAND_UI(ID_ACTION_DEC, &CBaseView::OnEditorModeUpdate)

	ON_COMMAND(ID_HIDE_OBJECT, &KSceneEditorView::OnHideObject)
	ON_UPDATE_COMMAND_UI(ID_HIDE_OBJECT, &CBaseView::OnEditorModeUpdate)
	ON_COMMAND(ID_HIDE_TERRAIN, &KSceneEditorView::OnHideTerrain)
	ON_UPDATE_COMMAND_UI(ID_HIDE_TERRAIN, &CBaseView::OnEditorModeUpdate)
	ON_COMMAND(ID_HIDE_GRASS, &KSceneEditorView::OnHideGrass)
	ON_UPDATE_COMMAND_UI(ID_HIDE_GRASS, &CBaseView::OnEditorModeUpdate)
	ON_COMMAND(ID_HIDE_FRUTEX, &KSceneEditorView::OnHideFrutex)
	ON_UPDATE_COMMAND_UI(ID_HIDE_FRUTEX, &CBaseView::OnEditorModeUpdate)
	ON_COMMAND(ID_HIDE_NPCATTACKRANGE, &KSceneEditorView::OnHideNPCAttackRange)
	ON_UPDATE_COMMAND_UI(ID_HIDE_NPCATTACKRANGE, &CBaseView::OnEditorModeUpdate)

	ON_COMMAND(ID_OBJECT_MANAGER, &KSceneEditorView::OnShowObjectManager)
	ON_UPDATE_COMMAND_UI(ID_OBJECT_MANAGER, &CBaseView::OnEditorModeUpdate)	
	ON_COMMAND(ID_LIGHT_PROPERTY, &KSceneEditorView::OnShowLightProperty)
	ON_UPDATE_COMMAND_UI(ID_LIGHT_PROPERTY, &CBaseView::OnEditorModeUpdate)	

	//ID_SETCAMAREDIR_RESET
	ON_COMMAND(ID_ACTION_HOME, &KSceneEditorView::OnHome)
	ON_COMMAND(ID_ACTION_END, &KSceneEditorView::OnEnd)
	
	ON_COMMAND(ID_ACTION_ADDKEYFRAME, &KSceneEditorView::OnAddKeyFrame)
	ON_UPDATE_COMMAND_UI(ID_ACTION_ADDKEYFRAME, &CBaseView::OnEditorModeUpdate)	
	ON_COMMAND(ID_ACTION_DROPOBJ, &KSceneEditorView::OnDropObj)
	ON_UPDATE_COMMAND_UI(ID_ACTION_DROPOBJ, &CBaseView::OnEditorModeUpdate)	

	ON_COMMAND(ID_GETSELECTEDOBJINHAND, &KSceneEditorView::OnGetSelectedObjInHand)
	ON_UPDATE_COMMAND_UI(ID_GETSELECTEDOBJINHAND, &CBaseView::OnEditorModeUpdate)	
	
	ON_COMMAND(ID_LOGICALPOLY, &KSceneEditorView::OnLogicalpoly)
	ON_COMMAND(ID_ADDMAP, &KSceneEditorView::OnAddmap)
	ON_COMMAND(ID_LOAD_DEFAULT_MAP, &KSceneEditorView::OnLoadDefaultMap)
	ON_COMMAND(ID_SOUNDBALLEDIT, &KSceneEditorView::OnSoundballedit)
	ON_COMMAND(ID_PLAYSCENESOUND, &KSceneEditorView::OnPlayscenesound)
	ON_UPDATE_COMMAND_UI(ID_PLAYSCENESOUND, &KSceneEditorView::OnUpdatePlayscenesound)
	ON_COMMAND(ID_RENDERSOUND, &KSceneEditorView::OnRendersound)
	ON_UPDATE_COMMAND_UI(ID_RENDERSOUND, &KSceneEditorView::OnUpdateRendersound)
	ON_COMMAND(ID_ENABLE_PVS, &KSceneEditorView::OnEnablePVS)
	ON_COMMAND(ID_USEBONEBOX, &KSceneEditorView::OnIsRayUseBoneBox)
	ON_COMMAND(ID_ENGINECOMMAND, &KSceneEditorView::OnEngineCommand)
	ON_COMMAND(ID_REFRESHSHADER,&KSceneEditorView::OnRefreshshader)
	//ON_COMMAND(ID_PVSSETUP, &KSceneEditorView::OnPvssetup)
	ON_COMMAND(ID_40043, &KSceneEditorView::OnAntiPortalSetup)
	ON_COMMAND(ID_TERRAINSELECT, &KSceneEditorView::OnTerrainselect)
	ON_COMMAND(ID_ROAD, &KSceneEditorView::OnRoad)

    ON_COMMAND(ID_ZOOM_BIG, &KSceneEditorView::OnZoomCoordBig)
    ON_COMMAND(ID_ZOOM_LIT, &KSceneEditorView::OnZoomCoordLit)

    ON_COMMAND(ID_COORD, &KSceneEditorView::OnSetCoord)
    ON_COMMAND(ID_COMMAND_SFX_LINK, &KSceneEditorView::OnSetLink)
	ON_COMMAND(ID_DETAILMAP, &OnDetailMap)

	ON_COMMAND(ID_WALLSETUP, &KSceneEditorView::OnWallsetup)
	ON_COMMAND(ID_RIVER, &KSceneEditorView::OnRiver)
	ON_COMMAND(ID_MenuEntityNum, &KSceneEditorView::OnMenuentitynum)
	ON_COMMAND(ID_MOVE_NPC, &KSceneEditorView::OnMoveNPC)
	
	ON_WM_LBUTTONUP()
	ON_WM_SETCURSOR()

	ON_WM_ACTIVATE()
	ON_COMMAND(ID_ShortcutKey, &KSceneEditorView::OnShortcutkey)
	END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// KSceneEditorView construction/destruction

KSceneEditorView::KSceneEditorView() : m_pPane(NULL) , m_preFullScrIsClosed(FALSE)
{
//	m_bAddToScene = FALSE;
	m_pSelection = NULL;
	m_bAutoSave = FALSE;
	//m_pLightProperty = NULL;
	m_lpDialogObjectMgr = NULL;
	m_nOutputWindowID = -1;

	m_lpDialogDirMgr = NULL;
	m_pSceneSettingDialog = NULL;

	m_lpDialogPVS = NULL;
	m_lpDialogAntiPortal = NULL;
	m_lpDialogRoad = NULL;
	m_lpDialogDetailMap = NULL;

	m_lpDialogWall = NULL;
	m_lpDialogRiver = NULL;
	m_lpDialogKey = NULL;

	//m_lpDialogNpcProperty = NULL;
}


KSceneEditorView::~KSceneEditorView()
{
	//SAFE_DELETE(m_pLightProperty);
	SAFE_DELETE(m_lpDialogObjectMgr);
	SAFE_DELETE(m_lpDialogDirMgr);
	if(m_pSceneSettingDialog)///<CTH>
	{
		HWND hWnd = m_pSceneSettingDialog->GetSafeHwnd();
		if (hWnd)
		{
			m_pSceneSettingDialog->DestroyWindow();///无模式对话框，如果这个对话框需要OnDestory的话，就需要主动调这个
		}
		delete m_pSceneSettingDialog;
	}

	SAFE_DELETE(m_lpDialogPVS);
	SAFE_DELETE(m_lpDialogAntiPortal);
	SAFE_DELETE(m_lpDialogRoad);
	SAFE_DELETE(m_lpDialogDetailMap);
	SAFE_DELETE(m_lpDialogWall);
	SAFE_DELETE(m_lpDialogRiver);
	SAFE_DELETE(m_lpDialogKey);
}

void KSceneEditorView::OnSetPointColor()
{
    IEKG3DSceneSceneEditor* pScene = GetDocScene();

    if (!pScene)
        return;

    ULONG nSelCount = 0;
    //pScene->GetSelectedEntityCount(&nSelCount);
	nSelCount = KSF::GetSelectionCount(pScene);
    
    if (nSelCount < 1)
        return;

    IEKG3DSceneEntity* pEntity = NULL;
   /* pScene->GetSelectedEntity(0, &pEntity);

    if (!pEntity)
        return;*/

	pEntity = KSF::GetFirstSelectedEntity(pScene);
	if(NULL == pEntity)
		return;

    SCENEENTITYTYPE dwType;
    pEntity->GetType(&dwType);

    IEKG3DRepresentObject* pObj = NULL;
    if (dwType != SCENEENTITY_OBJECT)
        return;

    pEntity->GetPointer((void**)&pObj);

    if (!pObj)
        return;

    IKG3DModel* pModel = NULL;

    pObj->GetModel(pModel, 0);

    if (!pModel)
        return;

    if (pModel->GetType() != MESHTYPE_POINTLIGHT)
        return;

    pModel->GetModelColor(0, &m_ColorPointLightEdit);
    GetColorPickerDialog()->Init(&m_ColorPointLightEdit, "Point Light Color (Alpha To Control Power)", this, TRUE);
}

LRESULT KSceneEditorView::OnColorChanged(WPARAM wParam, LPARAM lParam)
{
    IEKG3DSceneSceneEditor* pScene = GetDocScene();

    if (!pScene)
        return 0;

	ULONG nSelCount = 0;
	//pScene->GetSelectedEntityCount(&nSelCount);
	nSelCount = KSF::GetSelectionCount(pScene);

    if (nSelCount < 1)
        return 0;

    IEKG3DSceneEntity* pEntity = NULL;
    //pScene->GetSelectedEntity(0, &pEntity);
	pEntity = KSF::GetFirstSelectedEntity(pScene);

    if (!pEntity)
        return 0;

    SCENEENTITYTYPE dwType;
    pEntity->GetType(&dwType);

    IEKG3DRepresentObject* pObj = NULL;
    if (dwType != SCENEENTITY_OBJECT)
        return 0;

    pEntity->GetPointer((void**)&pObj);

    if (!pObj)
        return 0;

    IKG3DModel* pModel = NULL;

    pObj->GetModel(pModel, 0);

    if (!pModel)
        return 0;

    if (pModel->GetType() != MESHTYPE_POINTLIGHT)
        return 0;

    pModel->SetModelColor(0, m_ColorPointLightEdit);
    pScene->NotifyObjInfomationChanged(pObj);


    return 1;
}

void KSceneEditorView::OnSavePvs()
{
    IEKG3DSceneSceneEditor* pScene = GetDocScene();

    if (!pScene)
        return;

	ULONG nSelCount = 0;
	//pScene->GetSelectedEntityCount(&nSelCount);
	nSelCount = KSF::GetSelectionCount(pScene);

    if (nSelCount != 1)
    {
        MessageBox(TEXT("一次只能保存一个Pvs文件"), TEXT("Error"));
        return;
    }

    IEKG3DSceneEntity* pEntity = NULL;
    //pScene->GetSelectedEntity(0, &pEntity);
	pEntity = KSF::GetFirstSelectedEntity(pScene);

    if (!pEntity)
        return;

    SCENEENTITYTYPE dwType;
    pEntity->GetType(&dwType);

    IEKG3DRepresentObject* pObj = NULL;
    if (dwType != SCENEENTITY_OBJECT)
    {
        MessageBox(TEXT("你选中的不是Pvs物件"), TEXT("Error"));
        return;
    }

    pEntity->GetPointer((void**)&pObj);

    IEKG3DRepresentPVS* pPvs = NULL;

    if (pObj->GetType() != REPRESENTOBJECT_PVS)
    {
        MessageBox(TEXT("你选中的不是Pvs物件"), TEXT("Error"));
        return;
    }

    pPvs = dynamic_cast<IEKG3DRepresentPVS*>(pObj);

    ASSERT(pPvs);


    string strSaveNem = pPvs->GetName();

    if (strSaveNem.length() <= 0 || strSaveNem == "temp.pvs")
    {
        char szDefPath[MAX_PATH];
        g_GetFullPath(szDefPath, _T("Pvs\\"));

        CFileDialog fileDlg(
            FALSE, szDefPath, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
            "Pvs文件|*.pvs|所有文件|*.*|", this
            );
        fileDlg.m_ofn.lStructSize = sizeof(fileDlg.m_ofn);
        fileDlg.m_ofn.lpstrTitle  = "保存Pvs文件";
        fileDlg.m_ofn.lpstrInitialDir = szDefPath;

        if (fileDlg.DoModal() != IDOK)
            return;

        strSaveNem = fileDlg.GetPathName().GetBuffer();
        fileDlg.GetPathName().ReleaseBuffer();
    }

    if (FAILED(pPvs->SavePvsFile(strSaveNem.c_str())))
    {
        MessageBox(TEXT("Pvs物件保存失败"), TEXT("Error"));
    }

}

void KSceneEditorView::OnSaveAsPvs()
{
    IEKG3DSceneSceneEditor* pScene = GetDocScene();
    if (!pScene)
        return;

    pScene->AddPvsPortal();
}

void KSceneEditorView::OnAddPortal()
{
    IEKG3DSceneSceneEditor* pScene = GetDocScene();
    if (!pScene)
        return;

    pScene->AddPvsPortal();
}

void KSceneEditorView::OnConverPvs()
{
    IEKG3DSceneSceneEditor* pScene = GetDocScene();
    if (!pScene)
        return;

	ULONG nSelCount = 0;
	//pScene->GetSelectedEntityCount(&nSelCount);
	nSelCount = KSF::GetSelectionCount(pScene);

    if (nSelCount != 1)
    {
        MessageBox(TEXT("一次只能转换一个Obj"), TEXT("Error"));
        return;
    }

    IEKG3DSceneEntity* pEntity = NULL;
    //pScene->GetSelectedEntity(0, &pEntity);
	pEntity = KSF::GetFirstSelectedEntity(pScene);

    if (!pEntity)
        return;

    SCENEENTITYTYPE dwType;
    pEntity->GetType(&dwType);

    IEKG3DRepresentObject* pObj = NULL;
    if (dwType != SCENEENTITY_OBJECT)
    {
        MessageBox(TEXT("只能转换Object"), TEXT("Error"));
        return;
    }
    
    pEntity->GetPointer((void**)&pObj);

    IEKG3DRepresentPVS* pPvs = NULL;

    //if (pObj->GetObjType() != REPRESENT_TYPE_PVS)
	if(pObj->GetType() != REPRESENTOBJECT_PVS)
    {
        if (FAILED(pScene->ConvertCurrentSelectedObjectToPVS(&pPvs)))
        {
            MessageBox(TEXT("Convert Faild!"));
            return;
        }
    }
    else
    {
        pPvs = dynamic_cast<IEKG3DRepresentPVS*>(pObj);
    }

    ASSERT(pPvs);

    m_DialogPanel.ShowDialog(TEXT("Pvs 面板"));
    m_DialogPanel.UpdateUI();
}

void KSceneEditorView::OnSetCoord()
{
    IEKG3DSceneSceneEditor* pScene = GetDocScene();

    CXTPCommandBars* pCommandBars = ((MFCFramework::CMainFrame*)AfxGetMainWnd())->GetCommandBars();
    
    if (!pCommandBars)
        return;
    
    CXTPToolBar* pToolBarStd = pCommandBars->GetToolBar(IDR_MAINFRAME);
    
    if (!pToolBarStd)
        return;

    CXTPControlComboBox* pControlCombo = (CXTPControlComboBox*)pToolBarStd->GetControl(18);
    if (!pControlCombo)
        return;

    int nSel = pControlCombo->GetCurSel();
    switch (nSel)
    {
    case 0 :
        pScene->SetCoordMode(KG3DCOORD_WORLD);
        break;
    case 1 :
        pScene->SetCoordMode(KG3DCOORD_LOCAL);
        break;
    default :
        break;
    }

}

void KSceneEditorView::OnSetLink()
{
    D3DXVECTOR3 v = D3DXVECTOR3(100.f, 100.f, 100.f);
    IKG3DScene* pScene = dynamic_cast<IKG3DScene*>(GetDocScene());
    pScene->AddSceneSigns(v);

    int a = 0;
    a++;
}


void KSceneEditorView::OnZoomCoordBig()
{
    IEKG3DSceneSceneEditor* pEditor = GetDocScene();
    ASSERT(pEditor);
    pEditor->ZoomCoordinate(0.1f);
}

void KSceneEditorView::OnZoomCoordLit()
{
    IEKG3DSceneSceneEditor* pEditor = GetDocScene();
    ASSERT(pEditor);
    pEditor->ZoomCoordinate(-0.1f);
}

BOOL KSceneEditorView::PreCreateWindow(CREATESTRUCT& cs)
{
	//  the CREATESTRUCT cs

	return CBaseView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// KSceneEditorView drawing

void KSceneEditorView::OnDraw(CDC* pDC)
{
	KSceneEditorDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

#define GETEDITOR() \
	IEKG3DSceneSceneEditor* pEditor = GetDocScene();\
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
	IEKG3DScene* pScene = NULL;\
	pEditor->GetScene(&pScene);\
	_ASSERTE(pScene); \
	if(!pScene) return

#define GETSELECT() \
	IEKG3DSceneSelectBase* pSelector = NULL; \
	pEditor->GetSceneSelector(&pSelector); \
	_ASSERTE(pSelector); \
	if (!pSelector) \
		return

BOOL KSceneEditorView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
    IEKG3DSceneSceneEditor* pScene = GetDocScene();
    IEKG3DSceneEditorBase* pEditorBase = NULL;
    pScene->GetSceneEditorBase(&pEditorBase);

    if (pEditorBase && pEditorBase->GetRtsActive())
    {
        DWORD dwState = 0;
        pScene->GetEditState(&dwState);
        switch (dwState)
        {
        case SCENESTATE_MOVE :
            SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_MOVE));
            return TRUE;
        case SCENESTATE_ROTATE :
            SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_ROT));
            return TRUE;
        case SCENESTATE_SCALE :
            SetCursor(AfxGetApp()->LoadCursor(IDC_CURSOR_SCL));
            return TRUE;
        default :
            break;
        }
    }
    return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

void KSceneEditorView::OnInitialUpdate()
{
	CBaseView::OnInitialUpdate();

	KSceneEditorDoc* pDoc =  GetDocument();
	KGLOG_PROCESS_ERROR(pDoc);
	
	IEKG3DScene* p3DScene = pDoc->GetScene();
	KGLOG_PROCESS_ERROR(p3DScene);	

	//KG3DSceneSceneEditor* pEditor = static_cast<KG3DSceneSceneEditor*> (p3DScene);
	GETEDITOR();
	KGLOG_PROCESS_ERROR(pEditor);


	static PROXYKEYMAP KEYMAP[] =
	{	//           C  S  A  L  M  R  ?    Trigger			ID				 Prev Ig Exclusive
		{'X'	   , 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN , EXEACTION_PRESURE,  0, 0, 1},	//压感调整
		{'Z'	   , 0, 1, 0, 1, 0, 0, 0, WM_LBUTTONDOWN , EXEACTION_INNER_CIRCLE,  0, 0, 1},	//内外圈同时调整
		{'Z'	   , 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN , EXEACTION_OUTTER_CIRCLE,  0, 0, 1},	//外圈调整

		{VK_RBUTTON, 0, 0, 0, 0, 0, 1, 0, WM_MOUSEMOVE,   EXEACTION_ROTATE_CAMERA,  0, 1, 0},	//alt+MBT, 旋转摄像机镜头
		{VK_CONTROL, 1, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   EXEACTION_ZOOM_VIEW,  0, 0, 0},	//ctrl+MBT, 快速缩放视角
		{VK_MBUTTON, 0, 0, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   EXEACTION_PAN_VIEW,  0, 0, 0},	//MBT, PAN视图
		{VK_MBUTTON, 0, 1, 0, 0, 1, 0, 0, WM_MOUSEMOVE,   EXEACTION_ROTATE_VIEW/*4*/,  0, 0, 0},	//SHIFT+MBT, 以选定点为中心旋转视图
		{VK_LBUTTON, 1, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_INVERT_ACTION,  0, 0, 0},	//CTRL＋LBT，反向当前操作

		{'W'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_GO_FORWARD,  0, 2, 0},	//前进 忽略鼠标键
		{VK_LBUTTON, 0, 0, 0, 1, 0, 1, 0, WM_MOUSEMOVE  , EXEACTION_GO_FORWARD,  0, 0, 0},	//按住左右键不放，就直走
		{'S'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_GO_BACKWARD,  0, 2, 0},	//后退 忽略鼠标键
		{'A'	   , 0, 0, 0, 1, 0, 0, 0, WM_KEYDOWN    , EXEACTION_TURN_LEFT,  0, 1, 0},	//左转 
		{'D'	   , 0, 0, 0, 1, 0, 0, 0, WM_KEYDOWN    , EXEACTION_TURN_RIGHT,  0, 1, 0},	//右转 
		{'A'	   , 0, 0, 0, 0, 1, 0, 0, WM_KEYDOWN    , EXEACTION_TURN_LEFT,  0, 1, 0},	//左转 
		{'D'	   , 0, 0, 0, 0, 1, 0, 0, WM_KEYDOWN    , EXEACTION_TURN_RIGHT,  0, 1, 0},	//右转 
		{'A'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_MOV_LEFT,  0, 2, 0},	//左移 忽略键盘
		{'D'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN    , EXEACTION_MOV_RIGHT,  0, 2, 0},	//右移 忽略键盘

		{VK_RBUTTON, 0, 0, 0, 0, 0, 1, 0, WM_MOUSEMOVE , EXEACTION_CAM_CIRCLE,  0, 0, 0},	//按住右键不放，就转动摄像机
		{'Q'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , EXEACTION_MOV_UP,  0, 3, 0},	//升高，忽略所有
		{'E'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , EXEACTION_MOV_DOWN,  0, 3, 0},	//下降，忽略所有
		{' '	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , EXEACTION_JUMP,  0, 3, 0},	//跳，忽略所有
		{'A'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , EXEACTION_TURN_LEFT_VIEW_FOLLOWED,  0, 1, 0},	//左转，同时转动摄像机
		{'D'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , EXEACTION_TURN_RIGHT_VIEW_FOLLOWED,  0, 1, 0},	//右转

		{VK_LBUTTON, 0, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_LEFE_KEY_DOWN,  0, 0, 0},	//左键按下，可以作选择、改变地形等很多动作
		{VK_SHIFT,   0, 1, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_SMOOTH_TERRAIN,  0, 0, 0},	//平滑
		
		{VK_ADD,	 0, 0, 0, 0, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_SPEED_UP,  0, 3, 0},	//加速
		{VK_SUBTRACT,0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN,     EXEACTION_SPEED_DOWN,  0, 3, 0},	//减速
		
		{VK_LBUTTON, 1, 0, 0, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_SELECTION_ADD,  0, 0, 0},	//在当前选择范围中，再增加选择内容
		{VK_LBUTTON, 0, 0, 1, 1, 0, 0, 0, WM_LBUTTONDOWN, EXEACTION_SELECTION_SUTRACT,  0, 0, 0},	//在当前选择范围中，再减少选择内容
		
		{'1'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , EXEACTION_SELECT,  0, 3, 0},	//选
		{'2'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , EXEACTION_MOVE,    0, 3, 0},	//移
		{'3'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , EXEACTION_ROTATE,  0, 3, 0},	//转
		{'4'	   , 0, 0, 0, 0, 0, 0, 0, WM_KEYDOWN   , EXEACTION_SCALE,   0, 3, 0},	//放
				
		//{0,			 0, 0, 0, 0, 0, 0, 0, WM_MOUSEMOVE,   EXEACTION_MOUSE_MOVE, 0, 0, 0},	//单纯的检查鼠标移动
		{0		   , 0, 0, 0, 0, 0, 0, 0, 0             , 0,  0, 0, 0},	//结束
		//           C  S  A  L  M  R  ?    Trigger     ID  Prev Ig Exclusive
	};

	GETSCENEBASE();
	m_InputProxy.Init(pBase, this, KEYMAP);
	KG3DSceneLogicalEditorBase* pLogic = (KG3DSceneLogicalEditorBase*)pDoc->GetLogicalScene();
	if(pLogic)
		pLogic->SetInputProxy(&m_InputProxy);

	p3DScene->AddOutputWindow("SceneMain",m_hWnd, OUTPUTWND_SCENEEDITOR_MAIN,0,&m_nOutputWindowID);


    //////////////////////////////////////////////////////////////////////////
    //
    IEKG3DSceneOutputWnd* pWnd = NULL;    
    p3DScene->GetOutputWindow(m_nOutputWindowID, &pWnd);
    ASSERT(pWnd);

    IEKG3DCamera* pCamera = pWnd->GetIECamera();
    ASSERT(pCamera);

    float fFovy = 0.0f;
    float fAspect = 0.0f;
    float fzNear = 0.0f;
    float fzFar = 0.0f;

    pCamera->GetGlobalPerspective(&fFovy, &fAspect, &fzNear, &fzFar);
    fzFar = 40000;
    fFovy = D3DX_PI / 180 * 45;
    pCamera->SetGlobalPerspective(fFovy, fAspect, 10.0, fzFar);
    //
    //////////////////////////////////////////////////////////////////////////



	ASSERT( p3DScene->GetCurrentCamera() );

    IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
    p3DScene->GetCurOutputWnd(&piSceneCurOutputWnd);
    if (piSceneCurOutputWnd)
        piSceneCurOutputWnd->InitCameraStation(EDITOR_CONFIG_FILE);

	/*p3DScene->GetCurrentCamera()->SetPosition( pEditor->m_vCameraPos );
	p3DScene->GetCurrentCamera()->SetLookAtPosition( pEditor->m_vCameraLookAtPos );
	p3DScene->GetCurrentCamera()->SetUpDirection( pEditor->m_vCameraUpDirection );*/


	m_DialogPanel.SetSceneEditor(pEditor,pDoc->m_lpLogicalData,pDoc);
	m_DialogPanel.m_DialogTerrian.SetSceneEditorView(this);
	m_DialogPanel.m_DialogTerrian.SetSceneEditor(pEditor);

	IEKG3DSceneOutputWnd* pwnd = NULL;
    p3DScene->GetOutputWindow(m_nOutputWindowID, &pwnd);
	if(pwnd)
	{
		RECT rect;
		GetWindowRect(&rect);
		DWORD W = rect.right - rect.left;
		DWORD H = rect.bottom -rect.top;
		//PostRender::KG3DPostRenderEffectBase* pEffect =  NULL;
		//pwnd->AddPostRenderEffect(pEffect,PostRender::TYPE_BLOOM);
		//pwnd->EnablePostRenderEffect(TRUE,W,H);
	}


	//SetTimer(0,1000,NULL);

	m_pSelection = NULL;    // initialize selection

	m_DialogPanel.Create(m_DialogPanel.IDD);
	//m_DialogPanel.ShowWindow(SW_SHOW);

    //m_PaintTextureDialog.Create(m_PaintTextureDialog.IDD);

	//(by dengzhihui 2006年7月13日 18:13:14
	m_paneManager.InstallDockingPanes(GetParentFrame());
	m_paneManager.SetTheme(xtpPaneThemeExplorer);

	// Create docking panes.
	m_pPane = m_paneManager.CreatePane(
		IDR_MAINFRAME, CRect(0, 0,260, 120), dockRightOf);
	m_pPane->SetOptions(xtpPaneNoHideable);
	//)

	m_DialogTopView.Create(KSceneEditorTopViewDialog::IDD);
	m_DialogTopView.SetSceneEditor(pEditor);
	m_DialogTopView.ShowWindow(SW_HIDE);//FALSE);

    m_DialogSnapTool.Create(KGDlgSnapTool::IDD);
    m_DialogSnapTool.SetEditor(pEditor);
    m_DialogSnapTool.ShowWindow(SW_HIDE);

	m_DialogLayer.Create(KSceneEditorLayerDialog::IDD);
	m_DialogLayer.SetEditor(pBase);
	m_DialogLayer.ShowWindow(SW_HIDE);

	m_DialogVisibleSet.Create(KSceneEditorVisibleSetDialog::IDD);
	m_DialogVisibleSet.ShowWindow(SW_HIDE);
	m_DialogVisibleSet.CenterWindow();
	//(by dengzhihui 2006年8月4日 13:26:16 保存trap设置信息 
	m_DialogPanel.SetDocument(pDoc, false); //false //load, true = save
	//)
	//(by dengzhihui 2006年8月30日 18:00:26
	POINT pt;
	GetCursorPos(&pt);
	ScreenToClient(&pt);
	m_nOldXPos = pt.x;
	m_nOldYPos = pt.y;
	//)
Exit0:
	return;
}
/////////////////////////////////////////////////////////////////////////////
// KSceneEditorView printing

BOOL KSceneEditorView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void KSceneEditorView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void KSceneEditorView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
}

void KSceneEditorView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used.
	
	KSceneEditorDoc* pDoc =  GetDocument();
	KGLOG_PROCESS_ERROR(pDoc);

	IEKG3DScene* p3DScene = pDoc->GetScene();
	KG_PROCESS_ERROR(p3DScene);

	p3DScene->RemoveOutputWindow(m_nOutputWindowID);
	m_nOutputWindowID = -1;

	//(by dengzhihui 2006年8月4日 13:26:16 保存trap设置信息 
	m_DialogPanel.SetDocument(pDoc, true);
	//)

Exit0:
	CBaseView::OnDestroy();
}


/////////////////////////////////////////////////////////////////////////////
// OLE Client support and commands


// Special handling of OnSetFocus and OnSize are required for a container
//  when an object is being edited in-place.
void KSceneEditorView::OnSetFocus(CWnd* pOldWnd)
{
	CBaseView::OnSetFocus(pOldWnd);
	BOOL bAutoReloadTexture = FALSE;
	IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    KSceneEditorDoc* pDoc =  GetDocument();
	if(pDoc)
	{
		IEKG3DScene* pScene = pDoc->GetScene();
		if(pScene)
		{
			g_pEngineManager->SetFocusScene(pScene);//by dengzhihui Refactor 2006年11月6日
			pScene->SetCurrentOutputWindow(m_nOutputWindowID);
		}
	}
	
	RECT r;
	GetWindowRect(&r);
	float Weigth = (r.right - r.left)*1.0f;
	float Height = (r.bottom - r.top)*1.0f ;
	//m_Camera.Aspect = Weigth/Height;
    //Get3DEngineInterface((void **)(&pManager));
    
    pManager->GetAutoReloadTexture(&bAutoReloadTexture);
	if(bAutoReloadTexture)
    {
        IEKG3DTextureTable *piTable = NULL;
       
        pManager->GetIEKG3DTextureTable(&piTable);
		piTable->AutoReload();
    }
}

void KSceneEditorView::OnSize(UINT nType, int cx, int cy)
{
	CBaseView::OnSize(nType, cx, cy);
/*	COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
	if (pActiveItem != NULL)
		pActiveItem->SetItemRects();
*/	
	RECT r;
	GetWindowRect(&r);
	float Weigth = (r.right - r.left)*1.0f;
	float Height = (r.bottom - r.top)*1.0f ;
	//m_Camera.Aspect = Weigth/Height;

	KSceneEditorDoc* pDoc =  GetDocument();
	if(pDoc)
	{
		IEKG3DScene* pScene = pDoc->GetScene();
		if(pScene)
		{
			pScene->OnResizeWindow(NULL);
		}
	}
	TCHAR str[256];
	wsprintf(str,"%d %d", (r.right - r.left),(r.bottom - r.top));
	this->SetWindowText(str);
}

/////////////////////////////////////////////////////////////////////////////
// KSceneEditorView diagnostics

#ifdef _DEBUG
void KSceneEditorView::AssertValid() const
{
	CBaseView::AssertValid();
}

void KSceneEditorView::Dump(CDumpContext& dc) const
{
	CBaseView::Dump(dc);
}

KSceneEditorDoc* KSceneEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(KSceneEditorDoc)));
	return (KSceneEditorDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// KSceneEditorView message handlers

int KSceneEditorView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CBaseView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//g_pEngineManager->SetRenderWindow(m_hWnd);
	
	CSize Size(3000,3000);
	//SetScrollSizes(MM_TEXT,Size);

	m_MenuPop.LoadMenu(IDR_MENU_OBJECT);

	return 0;
}

LRESULT KSceneEditorView::WindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	LRESULT hr = CBaseView::WindowProc(message, wParam, lParam);
    HRESULT hRetCode = E_FAIL;
	BOOL bRetCode = FALSE;
	static BOOL b = TRUE;
	if (b)
		return 1; //测试功能是否完整
	//(by dengzhihui 2006年8月17日 15:40:29
	//优化鼠标操作感
	return 1;//<CTH>//070316-解决C4715 not all control paths return a value

}

void KSceneEditorView::OnShowPanel() 
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	//pDoc->m_lpEditScene->m_lpDialogPanel->ShowWindow(TRUE);

}

void KSceneEditorView::OnUpdateShowPanel(CCmdUI* pCmdUI) 
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	//pCmdUI->SetCheck(pDoc->m_lpEditScene->m_lpDialogPanel->IsWindowVisible());

	
}

void KSceneEditorView::OnTimer(UINT nIDEvent) 
{
	/*if(m_bAutoSave)
	{
		int K = 0;
		TCHAR Dir[256];
		wsprintf(Dir,"%sAutoSave",g_Def_WorkDirectory);

		mkdir(Dir);
		TCHAR Name[256];
		SYSTEMTIME Time;

		::GetSystemTime(&Time);

		wsprintf(Name,"%s\\%d-%d-%d-%d.Map",Dir,Time.wMonth,Time.wDay,Time.wHour,Time.wMinute);

		KSceneEditorDoc* pDoc =  GetDocument();
		pDoc->m_lpEditScene->SaveScene(Name);
	}*/
	CBaseView::OnTimer(nIDEvent);
}

void KSceneEditorView::OnDetalground() 
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	//if(pDoc->m_lpEditScene)
	//{
	//	if(pDoc->m_lpEditScene->m_lpMapMesh)
	//	{
	//		//在这里改变地面精度
	//	}
	//}
}

void KSceneEditorView::OnUpdateDetalground(CCmdUI* pCmdUI) 
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	//if(pDoc->m_lpEditScene)
	//{
	//	if(pDoc->m_lpEditScene->m_lpMapMesh)
	//	{
	//		//修改地表贴图精度
	//	}
	//}	
}

void KSceneEditorView::OnGroup() 
{
	/*KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	KG3DSceneSceneEditor* pScene = static_cast<KG3DSceneSceneEditor*>(pDoc->GetScene());
	if(pScene)
	{
	pScene->GroupSelectEntity();
	}*/
}

void KSceneEditorView::OnUpdateGroup(CCmdUI* pCmdUI) 
{
	/*KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	KG3DSceneSceneEditor* pScene = static_cast<KG3DSceneSceneEditor*>(pDoc->GetScene());
	if(pScene)
	{
	if(pScene->m_listSelectedEntity.size()>1)
	{
	pCmdUI->Enable(TRUE);
	}
	else
	{
	pCmdUI->Enable(FALSE);
	}
	}*/
}

void KSceneEditorView::OnUngroup() 
{
	/*KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	KG3DSceneSceneEditor* pScene = static_cast<KG3DSceneSceneEditor*>(pDoc->GetScene());
	if(pScene)
	{
	pScene->UnGroupSelectEntity();
	}*/
}

void KSceneEditorView::OnUpdateUngroup(CCmdUI* pCmdUI) 
{
	/*KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	KG3DSceneSceneEditor* pScene = static_cast<KG3DSceneSceneEditor*>(pDoc->GetScene());
	if(pScene)
	{
	if(pScene->m_listSelectedEntity.size()>=1)
	{
	pCmdUI->Enable(TRUE);
	}
	else
	{
	pCmdUI->Enable(FALSE);
	}
	}*/
}

void KSceneEditorView::OnMotionblur() 
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
}

void KSceneEditorView::OnUpdateMotionblur(CCmdUI* pCmdUI) 
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
}

void KSceneEditorView::OnAutosave() 
{
	m_bAutoSave = !m_bAutoSave;
}

void KSceneEditorView::OnUpdateAutosave(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bAutoSave);
}

void KSceneEditorView::OnOptimizesmallentity() 
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	/*if(pDoc->m_lpEditScene)
	{
		pDoc->m_lpEditScene->m_bOptimizeSmallEntity = !pDoc->m_lpEditScene->m_bOptimizeSmallEntity;
	}*/
}

void KSceneEditorView::OnUpdateOptimizesmallentity(CCmdUI* pCmdUI) 
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	/*if(pDoc->m_lpEditScene)
	{
		pCmdUI->SetCheck(pDoc->m_lpEditScene->m_bOptimizeSmallEntity);
	}*/

}

void KSceneEditorView::OnScenelighting() 
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	/*if(pDoc->m_lpEditScene)
	{
		pDoc->m_lpEditScene->m_lpMapMesh->m_bRenderGroundWithEffect = 
			!pDoc->m_lpEditScene->m_lpMapMesh->m_bRenderGroundWithEffect;
	}*/
}

void KSceneEditorView::OnUpdateScenelighting(CCmdUI* pCmdUI) 
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	/*if(pDoc->m_lpEditScene)
	{
		pCmdUI->SetCheck(pDoc->m_lpEditScene->m_lpMapMesh->m_bRenderGroundWithEffect);
	}*/

}

void KSceneEditorView::OnRefreshTerrain() 
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	/*if(pDoc->m_lpEditScene)
	{
		pDoc->m_lpEditScene->m_lpMapMesh->RefreshGround(0);
	}*/
}

void KSceneEditorView::OnOutputSmallMap() 
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	//if(pDoc->m_lpEditScene)
	//{
	//	char bigBuff[800] = "";  // maximum common dialog buffer size
	//	char szFilter[] ="Small Map Files|*.tga|";

	//	CFileDialog dlg(true, NULL, NULL,
	//		OFN_HIDEREADONLY , szFilter);

	//	dlg.m_ofn.lpstrFile = bigBuff;
	//	dlg.m_ofn.nMaxFile = sizeof(bigBuff);
	//	if (	dlg.DoModal() == IDOK)
	//	{	
	//		CString Name;
	//		Name = dlg.GetPathName();

	//		TCHAR FileName[256];
	//		wsprintf(FileName,"%s",Name);

	//		pDoc->m_lpEditScene->OutputSmallMap(FileName);
	//	}
	//	//pDoc->m_lpEditScene->m_lpMapMesh->RefreshGround(0);
	//}
}

void KSceneEditorView::OnRefreshTexture()
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	{
        IEKG3DTextureTable *piTable = NULL;
        IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
        //Get3DEngineInterface((void **)(&pManager));
        pManager->GetIEKG3DTextureTable(&piTable);
		piTable->ReloadFromFile();
	}
}

void KSceneEditorView::OnSelect()
{
	OnPopMenuSelect();
}

/*//没有检查到任何引用
void PvsSelectFilter::Attach(IEKG3DRepresentPVS* pPvs)
{
    m_pPvs = pPvs;
}

bool PvsSelectFilter::operator()(IEKG3DSceneEntity* pEntity)
{
    ASSERT(m_pPvs);

    void* pPointer = NULL;
    pEntity->GetPointer(&pPointer);

    SCENEENTITYTYPE dwType;
    pEntity->GetType(&dwType);

    switch (dwType)
    {
    case SCENEENTITY_OBJECT :
        {
            IEKG3DRepresentObject* pObject = static_cast<IEKG3DRepresentObject*>(pPointer);
            int nNum = m_pPvs->GetInsideObjectCount();

            for (int i = 0; i < nNum; ++i)
            {
                if (m_pPvs->GetInsideObject((DWORD)i) == pObject)
                    return true;
            }
        }
        break;

    case SCENEENTITY_PVS_PORTAL :
        {
            IKG3DPvsPortal* pPortal = static_cast<IKG3DPvsPortal*>(pPointer);
            int nNum = m_pPvs->GetInsideObjectCount();
            nNum = m_pPvs->GetPortalCount();

            for (int i = 0; i < nNum; ++i)
            {
                if (m_pPvs->GetPortal((DWORD)i) == pPortal)
                    return true;
            }

        }
        break;
    default :
        break;
    }

    return false;
}
*/

void KSceneEditorView::OnLock()
{
    IEKG3DSceneSceneEditor* pScene = GetDocScene();
    if (!pScene)
        return;

    HRESULT hr = pScene->LockSelSpaceNode();

    if (FAILED(hr))
    {
        if (hr == E_CDN_NUMBER)
            MessageBox(TEXT("锁定物件过多,只能锁定一个pvs物件!!"), TEXT("Error"));
        else if (hr == E_CDN_TYPE)
            MessageBox(TEXT("锁定物件不是pvs物件!!"), TEXT("Error"));
        else
            MessageBox(TEXT("锁定出错,未知错误!!"), TEXT("Error"));
    }
}

void KSceneEditorView::OnUnLock()
{
    IEKG3DSceneSceneEditor* pScene = GetDocScene();
    if (!pScene)
        return;

    pScene->UnLockSpaceNode();

    //pScene->SetSelFilter(NULL);
}
#define USE_NEW_ENV_DIALOG 0
void KSceneEditorView::OnScenesetting()
{
#if ! USE_NEW_ENV_DIALOG
	if (!m_pSceneSettingDialog)
	{
		m_pSceneSettingDialog = new KSceneSettingDialog;
	}
	if (m_pSceneSettingDialog)
	{
		if(m_pSceneSettingDialog->GetSafeHwnd())
		{
			m_pSceneSettingDialog->ShowWindow(SW_SHOW);
		}
		else
		{
			BOOL bRet = m_pSceneSettingDialog->Create(KSceneSettingDialog::IDD, this);
			if(bRet)
			{
				m_pSceneSettingDialog->ShowWindow(SW_SHOW);
			}
		}
	}
#else
	if (!m_pSceneSettingDialog)
	{
		m_pSceneSettingDialog = new KSceneSettingDialogV2;
	}
	if (m_pSceneSettingDialog)
	{
		if(m_pSceneSettingDialog->GetSafeHwnd())
		{
			m_pSceneSettingDialog->ShowWindow(SW_SHOW);
		}
		else
		{
			BOOL bRet = m_pSceneSettingDialog->Create(KSceneSettingDialogV2::IDD, this);
			if(bRet)
			{
				m_pSceneSettingDialog->ShowWindow(SW_SHOW);
			}
		}
	}
#endif

#if defined(DEBUG) | defined(_DEBUG)
#else
#if USE_NEW_ENV_DIALOG
	NULL
#endif
#endif
}

void KSceneEditorView::OnLargeterrain()
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	//if(pDoc->m_lpEditScene)
	//{
	//	pDoc->m_lpEditScene->ShowDialogLargeTerrian(TRUE);
	//}
}

void KSceneEditorView::OnShowobscale()
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	/*if(pDoc->m_lpEditScene)
	{
		if(pDoc->m_lpEditScene->m_dwShowObscale)
			pDoc->m_lpEditScene->m_dwShowObscale = 0;
		else
			pDoc->m_lpEditScene->m_dwShowObscale = 1;
	}*/
}

void KSceneEditorView::OnUpdateShowobscale(CCmdUI *pCmdUI)
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	/*if(pDoc->m_lpEditScene)
	{
		pCmdUI->SetCheck(pDoc->m_lpEditScene->m_dwShowObscale);
	}*/
}

void KSceneEditorView::OnLButtonUp(UINT nFlags, CPoint point)
{
    m_DialogPanel.UpdateUI();
}

void KSceneEditorView::OnRButtonUp(UINT nFlags, CPoint point) 
{
	
	GETEDITOR();
	DWORD dwState;
	pEditor->GetEditState(&dwState);
	if (dwState == SCENESTATE_TERRAINSELECTIONMODIFY)
	{
		pEditor->ResetSelectTerrain();
		pEditor->SetEditState(SCENESTATE_TERRAINSELECTION);
		return;
	}
	if (dwState == SCENESTATE_TERRAINSELECTION)
		return;

	//if(GetEditState(pEditor)) - -! who add this, 0 meaning the state select
    if (!pEditor->IsPMode())
	{
		CPoint pt; 
		GetCursorPos(&pt); 

		CMenu* pM=m_MenuPop.GetSubMenu(0);

		ULONG nSelCount = 0;
		//pEditor->GetSelectedEntityCount(&nSelCount);
		nSelCount = KSF::GetSelectionCount(pEditor);

		BOOL bSelected = (nSelCount == 0);//pEditor->m_listSelectedEntity.size()==0;
		{
			pM->EnableMenuItem(ID_LOCK,bSelected);
			pM->EnableMenuItem(ID_HIDE,bSelected);
			pM->EnableMenuItem(ID_MOVE,bSelected);
			pM->EnableMenuItem(ID_ROTATE,bSelected);
			pM->EnableMenuItem(ID_SCALE,bSelected);
			pM->EnableMenuItem(ID_DROP,bSelected);
			pM->EnableMenuItem(ID_GROUP,bSelected);
			pM->EnableMenuItem(ID_SHWOPROPERTIES,bSelected);
			pM->EnableMenuItem(ID_MODIFY,bSelected);
			pM->EnableMenuItem(ID_COPY,bSelected);
			pM->EnableMenuItem(ID_DELETE,bSelected);
			pM->EnableMenuItem(ID_CUT,bSelected);
			pM->EnableMenuItem(ID_TRAFFICPOINT,bSelected);
            if (nSelCount == 1)//pEditor->m_listSelectedEntity.size() == 1)
            {
                pM->EnableMenuItem(ID_SHOCKWAVE_ENTITY, FALSE);
                IEKG3DSceneEntity *piSelectedEntity = NULL;//&(*pEditor->m_listSelectedEntity.begin());
				//pEditor->GetSelectedEntity(0, &piSelectedEntity);
				piSelectedEntity = KSF::GetFirstSelectedEntity(pEditor);
                
				SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
				PVOID pEntityPointer = NULL;
				
				if (NULL != piSelectedEntity)
				{					
					piSelectedEntity->GetType(&EntityType);
					piSelectedEntity->GetPointer(&pEntityPointer);
				}
              
				IEKG3DSceneEntity *piRenderEntity = NULL;
				GETSCENE();
				pScene->FindRenderEntity(EntityType, pEntityPointer, &piRenderEntity);
				if (piRenderEntity)
					pM->CheckMenuItem(ID_SHOCKWAVE_ENTITY, TRUE);
				else
					pM->CheckMenuItem(ID_SHOCKWAVE_ENTITY, FALSE);
            }
            else
                pM->EnableMenuItem(ID_SHOCKWAVE_ENTITY, TRUE);
		}
		if (GetAsyncKeyState(VK_CONTROL)&0x8000)
			pM->TrackPopupMenu(TPM_CENTERALIGN,pt.x,pt.y,this);
	}

	CBaseView::OnRButtonUp(nFlags, point);
}

void KSceneEditorView::OnDoodaScript()
{
    int nResult  = false;
    int nRetCode = false;

	DWORD dwEditState = 0;
	INT nSelectedEntityCount = 0;
    IEKG3DSceneSceneEditor* pScene = NULL; 
    KSceneEditorDoc* pDocment = NULL;

    pDocment = (KSceneEditorDoc*)GetDocument();
    KG_PROCESS_ERROR(pDocment);

    
	pScene = dynamic_cast<IEKG3DSceneSceneEditor*>(pDocment->GetScene());
    KG_PROCESS_ERROR(pScene);

    pScene->GetEditState(&dwEditState);
	KG_PROCESS_SUCCESS(dwEditState);

	//pScene->GetSelectedEntityCount(&nSelectedEntityCount);
	nSelectedEntityCount = KSF::GetSelectionCount(pScene);

    KG_PROCESS_ERROR(0 != nSelectedEntityCount);

    {
		KGNpcPropertyEditerDlg doodaPropertyDlg(pDocment->GetScene(),pDocment->m_lpLogicalData, this, PROPERTY_EDIT_TYPE_DOODAD);
		doodaPropertyDlg.DoModal();
		
		/*if (m_lpDialogNpcProperty != NULL)
		{
			m_lpDialogNpcProperty->CloseWindow();
			SAFE_DELETE(m_lpDialogNpcProperty);
		}
		m_lpDialogNpcProperty = new KGNpcPropertyEditerDlg(pDocment->GetScene(),pDocment->m_lpLogicalData, this, PROPERTY_EDIT_TYPE_DOODAD);
		m_lpDialogNpcProperty->Create(KGNpcPropertyEditerDlg::IDD);
		m_lpDialogNpcProperty->ShowWindow(SW_NORMAL);*/
    }

Exit1:
    nResult = true;
Exit0:
    return;
}

void KSceneEditorView::OnSelNpcRepresnet()
{
    KGSelRepresentNPCDlg dlg(this);
    dlg.DoModal();

//Exit0:
    return;
}

void KSceneEditorView::OnShwoproperties()
{
	int nResult  = false;
	int nRetCode = false;

	//KG3DSceneSceneEditor* pScene = NULL; 
	KSceneEditorDoc* pDocment = NULL;

	pDocment = (KSceneEditorDoc*)GetDocument();
	KG_PROCESS_ERROR(pDocment);
	GETEDITOR();
	GETSCENE();
	//pScene = (KG3DSceneSceneEditor*)pDocment->GetScene();
	KG_PROCESS_ERROR(pScene);

	//KG_PROCESS_SUCCESS(!pScene->m_bEditing);

	//KG_PROCESS_ERROR(pScene->m_listSelectedEntity.size());
	{
		KGNpcPropertyEditerDlg npcPropertyDlg(pDocment->GetScene(),pDocment->m_lpLogicalData);
		npcPropertyDlg.DoModal();

		/*if (m_lpDialogNpcProperty != NULL)
		{
			m_lpDialogNpcProperty->CloseWindow();
			SAFE_DELETE(m_lpDialogNpcProperty);
		}
		m_lpDialogNpcProperty = new KGNpcPropertyEditerDlg(pScene,pDocment->m_lpLogicalData);
		m_lpDialogNpcProperty->Create(KGNpcPropertyEditerDlg::IDD);
		m_lpDialogNpcProperty->ShowWindow(SW_NORMAL);*/
	}

//Exit1:
	nResult = true;
Exit0:
	return;
}

void KSceneEditorView::OnEditTrafficPoint()
{
	int nResult  = false;
	int nRetCode = false;

	//KG3DSceneSceneEditor* pScene = NULL; 
	KSceneEditorDoc* pDocment = NULL;
  
	pDocment = (KSceneEditorDoc*)GetDocument();
	KG_PROCESS_ERROR(pDocment);
	GETEDITOR();
	GETSCENE();
	//pScene = (KG3DSceneSceneEditor*)pDocment->GetScene();
	KG_PROCESS_ERROR(pScene);
	KG_PROCESS_ERROR(pEditor);
	{
		GETSCENEBASE();
		IEKG3DSceneEntity* pEntity = NULL;
		SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
		//pEditor->GetSelectedEntity(0,&pEntity);
		pEntity = KSF::GetFirstSelectedEntity(pEditor);

		nRetCode = pEntity->GetType(&EntityType);
		KGLOG_COM_PROCESS_ERROR(nRetCode);
		if(EntityType==SCENEENTITY_OBJECT)
		{
			IEKG3DRepresentObject* pObject = NULL;
			nRetCode = pEntity->GetPointer((VOID **)&pObject);
			KGLOG_COM_PROCESS_ERROR(nRetCode);
			if(pObject->GetType() == REPRESENTOBJECT_TRAFFICPOINT)
			{
				IEKG3DRepresentTrafficPoint* pRepTrafficPoint = pEditor->GetIEKG3DRepresentTrafficPoint(pObject);
				DWORD dwType = 0;
				dwType = pRepTrafficPoint->GetKind();
				if (dwType == 0)
				{
					KGTrafficPropertyEditerDlg trafficPropertyDlg(pScene,pDocment->m_lpLogicalData);//pScene,pDocment->m_lpLogicalData);
					trafficPropertyDlg.DoModal();
				}
			}
		}

	}

	//Exit1:
	nResult = true;
Exit0:
	return;
}

void KSceneEditorView::OnEditObjPropertys()
{
    HRESULT hResult  = false;
    HRESULT hRetCode = false;

	BYTE *pbObj = NULL;
	BYTE *pbModify = NULL;
	IEKG3DMesh *pMesh = NULL;
	IEKG3DModel* pModel = NULL;
	KSceneEditorDoc* pDocment = NULL;
	IEKG3DSceneSceneEditor* pScene = NULL;
	
    KGMEMDATEELEMENT element[] =   
    {
        ELEMENT_TITLE("显示相关(Display)"),
        {ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_BOOL,   _T("是否生成障碍")},
        {ELEMENT_AUTO, sizeof(INT),   MEM_TYPE_BOOL,   _T("是否有镜头碰撞")},
  
        ELEMENT_END()
    };

	//这里的大小要等于上面element包含数据的大小
	size_t uPropertySize = 0;
	int nIndex = 0;
	while (element[nIndex].nOffset != -1)
	{
		uPropertySize += element[nIndex].nSize;
		nIndex++;
	}

	pbModify = new BYTE[uPropertySize];
	KG_PROCESS_ERROR(pbModify);
	
	GETEDITOR();

    pDocment = (KSceneEditorDoc*)GetDocument();
    KG_PROCESS_ERROR(pDocment);

	pScene = pEditor;
	KG_PROCESS_ERROR(pScene);

	GETSCENEBASE();
	pBase->GetFirstSelectedModel(pModel);
	KG_PROCESS_ERROR(pModel);

	DWORD dwType = 0;
	pModel->GetType(&dwType);
	if (dwType == MESHTYPE_SPEEDTREE)
	{
		IEKG3DModelSpeedTree* pSpdTree = NULL;
		hRetCode = pModel->GetIEKG3DModelSpeedTree(&pSpdTree);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		KG_PROCESS_ERROR(pSpdTree);

		pSpdTree->GetObjProperty(&pbObj);
		memcpy_s(pbModify, 
			uPropertySize, 
			pbObj, 
			uPropertySize); 
		
		KGMemDateEditDlg memDateDlg(pbModify, element);
		if (memDateDlg.DoModal() == IDOK)
		{
			if (memcmp(pbObj, pbModify, uPropertySize))
			{
				if (MessageBox("物件属性已经修改， 是否要保存?", "提示", MB_YESNO) == IDYES)
				{
					memcpy_s(pbObj, 
						uPropertySize,
						pbModify,
						uPropertySize);
					pSpdTree->SavePropertyToIni(NULL);
				}
			}
		}
	}
	else
	{
		LPCTSTR pszName = NULL;
		hRetCode = pModel->GetIEKG3DMesh(&pMesh);
		KGLOG_COM_PROCESS_ERROR(hRetCode);

		KG_PROCESS_ERROR(pModel && pMesh);

		hRetCode = pMesh->GetObjProperty(&pbObj);
		KGLOG_COM_PROCESS_ERROR(hRetCode);
		pMesh->GetName(&pszName);
		KGMemDateEditDlg memDateDlg(pbModify, element);
		memDateDlg.SetTitleName(pszName);
		memcpy_s(pbModify, 
			uPropertySize, 
			pbObj, 
			uPropertySize);
		if (memDateDlg.DoModal() == IDOK)
		{
			if (memcmp(pbObj, pbModify, uPropertySize))
			{
				if (MessageBox("物件属性已经修改， 是否要保存?", "提示", MB_YESNO) == IDYES)
				{
					memcpy_s(pbObj, 
						uPropertySize,
						pbModify,
						uPropertySize);
					pMesh->SavePropertyToIni(NULL);
				}
			}
		}
	}
	hRetCode = S_OK;
Exit0:
	SAFE_DELETE_ARRAY(pbModify);
    return;
}

void KSceneEditorView::OnNnwMap()
{
}

void KSceneEditorView::OnUpdateModifyModel(CCmdUI* pCmdUI)
{
// 	KSceneEditorDoc* pDoc =  GetDocument();
// 	if(!pDoc)
// 		return ;
// 	IEKG3DScene* pScene = pDoc->GetScene();
// 	if(!pScene)
// 		return;
// 	KG3DSceneSceneEditor* pEditor = (KG3DSceneSceneEditor*) pScene;
	GETEDITOR();
	GETSELECT();
	
	IEKG3DSceneSelectionTool* pSelTool = NULL;
	pEditor->GetSelectionToolInterface(&pSelTool);
	if(NULL == pSelTool)
		return;

	//if(pSelector->GetSelectedEntitySize() > 0)
	if(pSelTool->GetSelectionCount() > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void KSceneEditorView::OnModifyPvs(IEKG3DSceneEntity* pEngity)
{
    if (!pEngity)
        return;

    void* pt = NULL;
    pEngity->GetPointer(&pt);

    IEKG3DRepresentPVS* pvs = dynamic_cast<IEKG3DRepresentPVS*>(static_cast<IEKG3DRepresentObject*>(pt));

    if (strlen(pvs->GetName()) <= 0)
        return;


    POSITION Pos = MFCFramework::theApp.m_pDocManager->GetFirstDocTemplatePosition();
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

    CDocument* pDoc = pTemplate->OpenDocumentFile(pvs->GetName());
    static_cast<KPvsEditorDoc*>(pDoc)->SetModifyPvs(pvs);
    

}


void KSceneEditorView::OnShowBsp()
{
    GETEDITOR();
    pEditor->EnableShowSelectedBsp(TRUE);
}

void KSceneEditorView::OnModifyModel()
{
    HRESULT hRetCode = E_FAIL;
	GETEDITOR();
	GETSCENEBASE();
	GETSCENE();

    // check modify pvs

    ULONG nCount = 0;
    //pEditor->GetSelectedEntityCount(&nCount);
	nCount = KSF::GetSelectionCount(pEditor);

    if (nCount <= 0)
        return;

    IEKG3DSceneEntity* pEntity = NULL;
    //pEditor->GetSelectedEntity(0, &pEntity);
	pEntity = KSF::GetFirstSelectedEntity(pEditor);

    if (!pEntity)
        return;

    SCENEENTITYTYPE EntityType;
    pEntity->GetType(&EntityType);
    
    if (EntityType == SCENEENTITY_OBJECT)
    {
        void* pt = NULL;
        pEntity->GetPointer(&pt);
        IEKG3DRepresentObject* obj = static_cast<IEKG3DRepresentObject*>(pt);

        if (obj->GetType() == REPRESENTOBJECT_PVS)
            return OnModifyPvs(pEntity);

		if (obj->GetType() == REPRESENTOBJECT_SOUNDBALL)
			return OnModifySoundBall(obj);
    }
   


	//POSITION Pos;
	CView* pOrgView = NULL;
	KSceneModelEditorView* pView = NULL;
	IEKG3DModel* pModel = NULL;
    LPCTSTR pszName = NULL;

	if(FAILED(pBase->GetFirstSelectedModel(pModel)))
		return;
	if(!pModel)
		return;
    
    DWORD dwModelType = 0;
    hRetCode = pModel->GetType(&dwModelType);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

    if (dwModelType == MESHTYPE_SFX)
    {
        char szName[MAX_PATH];
        pModel->GetMeshFileName(szName);
        MFCFramework::theApp.OpenDocumentFile(szName);
    }
    else
    {
	    //pModel->AddRef();

        POSITION Pos = MFCFramework::theApp.GetFirstDocTemplatePosition();
        CDocTemplate* pTemplate = NULL;
        while (Pos != NULL)
        {
	        pTemplate = (CDocTemplate*)MFCFramework::theApp.GetNextDocTemplate(Pos);
    		
	        CString strDocName;

	        pTemplate->GetDocString(strDocName, CDocTemplate::docName);
	        if (strDocName == _T("模型编辑"))
	        {
		        break;
	        }
        }

        ASSERT(pTemplate != NULL);
        ASSERT_KINDOF(CDocTemplate, pTemplate);

        CDocument* pOrgDoc = pTemplate->OpenDocumentFile(NULL);
        ASSERT_KINDOF(KSceneModelEditorDoc, pOrgDoc);
        KSceneModelEditorDoc* pModelDoc = (KSceneModelEditorDoc *)pOrgDoc;
        if (pModelDoc)
        {
            DWORD dwAmbient = 0;
			TCHAR strModelName[MAX_PATH];
	        IEKG3DScene* pScene = pModelDoc->GetScene();
	        g_pEngineManager->SetFocusScene(pScene);//by dengzhihui Refactor 2006年11月6日
	        IEKG3DSceneModelEditor* pModelEditor = NULL;
			IEKG3DScene* pSceneEditor = NULL;

		    pScene->GetSceneModelEditor(&pModelEditor);
	        ASSERT(pModelEditor);
	        pModelEditor->OpenModel(pModel, FALSE);
	        if (dwModelType == MESHTYPE_MODELST)
			{
                hRetCode = pModel->GetName(&pszName);
                KGLOG_COM_PROCESS_ERROR(hRetCode);

				strcpy(strModelName, pszName);
			}
			else
			{
				pModel->GetMeshFileName(strModelName);
			}
	        pModelDoc->SetPathName(strModelName, FALSE);
	        pModelDoc->SetModifiedFlag(TRUE);
            pSceneEditor = dynamic_cast<IEKG3DScene*>(pBase);

			hRetCode = pSceneEditor->GetAmbient(&dwAmbient);
            KGLOG_COM_PROCESS_ERROR(hRetCode);
            hRetCode = pScene->SetAmbient(dwAmbient);
			KGLOG_COM_PROCESS_ERROR(hRetCode);
            
			Pos = pModelDoc->GetFirstViewPosition();
			KG_PROCESS_ERROR(Pos);
			pOrgView = pModelDoc->GetNextView(Pos);
			KG_PROCESS_ERROR(pOrgView);
			pView = static_cast<KSceneModelEditorView*>(pOrgView);
			KG_PROCESS_ERROR(pView);
			pView->Zoom();
			pModelDoc->SendInitialUpdate();
			pModelDoc->SetPanelContainer(pModel);
			if(pScene != pSceneEditor)
			{
				IEKG3DEnvEffLight* pSunLight = NULL;
				IEKG3DEnvEffLight* pOtherLight = NULL;
				hRetCode = pScene->GetEngineIEInterface(KG3DTYPE_ENVEFF_LIGHT, 0, 0, (LPVOID*)&pSunLight);
				KG_COM_PROCESS_ERROR(hRetCode);
				hRetCode = pSceneEditor->GetEngineIEInterface(KG3DTYPE_ENVEFF_LIGHT, 0, 0, (LPVOID*)&pOtherLight);
				KG_COM_PROCESS_ERROR(hRetCode);
				hRetCode = pSunLight->Copy(pOtherLight);
				KG_COM_PROCESS_ERROR(hRetCode);
			}			
        }
    }
Exit0:
	return;
}

void KSceneEditorView::OnModifyLight()
{
	KSceneEditorDoc* pDoc = static_cast<KSceneEditorDoc*>(GetDocument());
	if(!pDoc)
		return ;
	IEKG3DScene* pScene = pDoc->GetScene();
	if(!pScene)
		return ;
	//if(!m_pLightProperty)
	//{
	//	m_pLightProperty = new KDlgLightProperty(NULL,pScene);
	//}
	//m_pLightProperty->ShowWindow(SW_NORMAL);
	//g_GetLightPropertyDlg().Init(pScene);

	MessageBox(_T("现在灯光的相应功能已经移到\"场景参数设定 > 灯光\""));
}

//////////////////////////////////////////////////////////////////////////
//pop menu
void KSceneEditorView::OnPopMenuMove()
{
	GETEDITOR();
	GETSELECT();
	pEditor->SetEditState(SCENESTATE_MOVE);//m_dwEditState = SCENESTATE_MOVE;
	//pSelector->OnToggleStateMove();//<OnToggleStateMove Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
}
void KSceneEditorView::OnPopMenuRotate()
{
	GETEDITOR();
	GETSELECT();
	pEditor->SetEditState(SCENESTATE_ROTATE);//m_dwEditState = SCENESTATE_ROTATE;
	//pSelector->OnToggleStateRotation();//<OnToggleStateRotation Refactor>//SceneSelectBase会监视SetEditState，在内部改变状态
}
void KSceneEditorView::OnPopMenuScale()
{
	GETEDITOR();
	pEditor->SetEditState(SCENESTATE_SCALE);//m_dwEditState = SCENESTATE_SCALE;
	//pEditor->OnToggleStateScale();
}

void KSceneEditorView::OnPopMenuSelect()
{
	GETEDITOR();
	pEditor->SetEditState(SCENESTATE_SELECT);//m_dwEditState = SCENESTATE_SELECT;
	//pEditor->OnToggleStateSelect();
}
void KSceneEditorView::OnPopMenuSelectByName()
{
	CBaseView::OnShowObjectSelectDlg();
}

void KSceneEditorView::OnUnitSeclectedEntity()//将多个模型合并为一个object
{
	GETEDITOR();
	/////add by huangjinshou 2008-08-22/////////////////////////////
	DWORD dwState = 0;
	pEditor->GetEditState(&dwState);//
	if(dwState != SCENESTATE_SELECT)
		return ; 


    if (pEditor->IsSelectedPvs())
    {
        AfxMessageBox(TEXT("不能合并室内物件,thanks :)"));
    }
    else
	    pEditor->UnitSelectEntity();
}

void KSceneEditorView::OnUpdateModelunit(CCmdUI *pCmdUI)
{
// 	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
// 	KG3DSceneSceneEditor* pScene = static_cast<KG3DSceneSceneEditor*>(pDoc->GetScene());
// 	if(!pScene)
// 		return;
	GETEDITOR();
	GETSELECT();

	IEKG3DSceneSelectionTool* pSelTool = NULL;
	pEditor->GetSelectionToolInterface(&pSelTool);
	if(NULL == pSelTool)
		return;

	//if(pSelector->GetSelectedEntitySize() > 1)
	if(pSelTool->GetSelectionCount() > 1)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void KSceneEditorView::OnModelununit()//拆散合并的object为多个模型
{
	GETEDITOR();
	/////add by huangjinshou 2008-08-22/////////////////////////////
	DWORD dwState = 0;
	pEditor->GetEditState(&dwState);//
	if(dwState != SCENESTATE_SELECT)
		return ; 
	
    if (!pEditor->IsSelectedPvs())
	    pEditor->UnUnitSelectEntity();
}

void KSceneEditorView::OnUpdateModelununit(CCmdUI *pCmdUI)
{
// 	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
// 	KG3DSceneSceneEditor* pScene = static_cast<KG3DSceneSceneEditor*>(pDoc->GetScene());
// 	if(!pScene)
// 		return;
	GETEDITOR(); GETSELECT();
	
	//if(pSelector->GetSelectedEntitySize() > 0)
	IEKG3DSceneSelectionTool* pSelTool = NULL;
	pEditor->GetSelectionToolInterface(&pSelTool);
	if(NULL == pSelTool)
		return;
	if(pSelTool->GetSelectionCount() > 0)
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void KSceneEditorView::OnPaintTexture()
{
	/*m_PaintTextureDialog.SetSceneEditor(m_DialogPanel.m_lpSceneEditor);
    IEKG3DTerrain *pTerrainEx = NULL;
    m_DialogPanel.m_lpSceneEditor->GetTerrainEx(&pTerrainEx);
	m_PaintTextureDialog.SetTerrain(pTerrainEx);
	m_PaintTextureDialog.SetDataToTerrainEx();
	m_PaintTextureDialog.ShowWindow(TRUE);*/
}

void KSceneEditorView::OnDirMgr()
{
	GETEDITOR();
	if(!m_lpDialogDirMgr)
	{
		m_lpDialogDirMgr = new KSceneSceneEditorDialogDirMgr;
		m_lpDialogDirMgr->SetSceneEditor(pEditor);
		m_lpDialogDirMgr->Create(KSceneSceneEditorDialogDirMgr::IDD,this);
	}
	m_lpDialogDirMgr->ShowWindow(TRUE);
}

void KSceneEditorView::OnShockwaveEntity()
{
	GETEDITOR();

    pEditor->ShockwaveEntity();
}

//(by dengzhihui 2006年7月6日 18:37:13
void KSceneEditorView::OnEditUndo(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (((NMXTPCONTROL*)pNMHDR)->pControl->GetType() == xtpControlSplitButtonPopup)
	{
		//CRichEditView::OnEditUndo();
		AfxMessageBox("撤消最近动作");
		*pResult = 1; // Handled;
	}

	CXTPControlListBox* pControlListBox = DYNAMIC_DOWNCAST(CXTPControlListBox, ((NMXTPCONTROL*)pNMHDR)->pControl);
	if (pControlListBox)
	{
		CString str;
		str.Format(_T("撤消 最近 %i 动作"), pControlListBox->GetListCtrl()->GetSelCount());

		AfxMessageBox(str);

		*pResult = 1; // Handled;
	}
}

CXTPControlStatic* KSceneEditorView::FindInfoControl(CXTPControl* pControl)
{
	CXTPCommandBar* pCommandBar = pControl->GetParent();

	for (int i = 0; i < pCommandBar->GetControls()->GetCount(); i++)
	{
		CXTPControlStatic* pControlStatic = DYNAMIC_DOWNCAST(CXTPControlStatic, pCommandBar->GetControl(i));
		if (pControlStatic && pControlStatic->GetID() == pControl->GetID())
		{
			return pControlStatic;
		}

	}
	return NULL;
}

void KSceneEditorView::OnUndoListBoxControlSelChange(NMHDR* pNMHDR, LRESULT* pRes)
{
	ASSERT(pNMHDR != NULL);
	ASSERT(pRes != NULL);

	CXTPControlListBox* pControlListBox = DYNAMIC_DOWNCAST(CXTPControlListBox, ((NMXTPCONTROL*)pNMHDR)->pControl);
	if (pControlListBox)
	{

		CXTPControlStatic* pInfo = FindInfoControl(pControlListBox);
		if (pInfo)
		{
			CString str;
			str.Format(_T("撤消 %i 动作"), pControlListBox->GetListCtrl()->GetSelCount());
			pInfo->SetCaption(str);
			pInfo->DelayRedrawParent();
		}



		*pRes = 1;
	}
}

void KSceneEditorView::OnUndoListBoxControlPoup(NMHDR* pNMHDR, LRESULT* pRes)
{
	ASSERT(pNMHDR != NULL);
	ASSERT(pRes != NULL);
	CXTPControlListBox* pControlListBox = DYNAMIC_DOWNCAST(CXTPControlListBox, ((NMXTPCONTROL*)pNMHDR)->pControl);
	if (pControlListBox)
	{
		CListBox* pListBox = pControlListBox->GetListCtrl();
		pListBox->ResetContent();

		int nCount = RAND_S() % 20 + 3;

		for (int i = 0; i < nCount; i++)
		{
			CString str;
			str.Format(_T("撤消 内容 %i"), i + 1);
			pListBox->AddString(str);
		}

		CXTPControlStatic* pInfo = FindInfoControl(pControlListBox);
		if (pInfo)
		{
			CString str;
			pInfo->SetCaption(_T("撤消 0 动作"));
			pInfo->DelayRedrawParent();
		}


		*pRes = 1;
	}
}

void KSceneEditorView::OnUpdateEditUndo(CCmdUI *pCmdUI)
{
	// 占位
}

void KSceneEditorView::OnEditUndo()
{
	// 占位
}

//////////////////////////////////////////////////////////////////////////
void KSceneEditorView::OnEditRedo(NMHDR* pNMHDR, LRESULT* pResult)
{
	if (((NMXTPCONTROL*)pNMHDR)->pControl->GetType() == xtpControlSplitButtonPopup)
	{		
		AfxMessageBox("重作最近动作");

		*pResult = 1; // Handled;
	}

	CXTPControlListBox* pControlListBox = DYNAMIC_DOWNCAST(CXTPControlListBox, ((NMXTPCONTROL*)pNMHDR)->pControl);
	if (pControlListBox)
	{
		CString str;
		str.Format(_T("重做 最近 %i 动作"), pControlListBox->GetListCtrl()->GetSelCount());

		AfxMessageBox(str);

		*pResult = 1; // Handled;
	}
}

void KSceneEditorView::OnRedoListBoxControlSelChange(NMHDR* pNMHDR, LRESULT* pRes)
{
	ASSERT(pNMHDR != NULL);
	ASSERT(pRes != NULL);

	CXTPControlListBox* pControlListBox = DYNAMIC_DOWNCAST(CXTPControlListBox, ((NMXTPCONTROL*)pNMHDR)->pControl);
	if (pControlListBox)
	{

		CXTPControlStatic* pInfo = FindInfoControl(pControlListBox);
		if (pInfo)
		{
			CString str;
			str.Format(("重做 %i 动作"), pControlListBox->GetListCtrl()->GetSelCount());
			pInfo->SetCaption(str);
			pInfo->DelayRedrawParent();
		}
		*pRes = 1;
	}
}

void KSceneEditorView::OnRedoListBoxControlPoup(NMHDR* pNMHDR, LRESULT* pRes)
{
	ASSERT(pNMHDR != NULL);
	ASSERT(pRes != NULL);
	CXTPControlListBox* pControlListBox = DYNAMIC_DOWNCAST(CXTPControlListBox, ((NMXTPCONTROL*)pNMHDR)->pControl);
	if (pControlListBox)
	{
		CListBox* pListBox = pControlListBox->GetListCtrl();
		pListBox->ResetContent();

		int nCount = RAND_S() % 20 + 3;

		for (int i = 0; i < nCount; i++)
		{
			CString str;
			str.Format(_T("重做 内容 %i"), i + 1);
			pListBox->AddString(str);
		}

		CXTPControlStatic* pInfo = FindInfoControl(pControlListBox);
		if (pInfo)
		{
			CString str;
			pInfo->SetCaption(_T("重做 0 动作"));
			pInfo->DelayRedrawParent();
		}


		*pRes = 1;
	}
}

void KSceneEditorView::OnUpdateEditRedo(CCmdUI *pCmdUI)
{
	//占位
}

void KSceneEditorView::OnEditRedo()
{
	//占位
}

LRESULT KSceneEditorView::OnDockingPaneNotify(WPARAM wParam, LPARAM lParam)
{
	if (wParam == XTP_DPN_SHOWWINDOW)
	{
		if(wParam == 1 && (lParam == 1 || lParam == 0)) //全屏消息
		{
			CXTPDockingPaneLayout layoutNormal(&m_paneManager);
			if (lParam == 1) //全屏化
			{
				// Save the current state for docking panes.
				//m_paneManager.GetLayout(&layoutNormal);
				//layoutNormal.Save(_T("fullscr"));
				m_preFullScrIsClosed = m_pPane->IsClosed();
				if (!m_pPane->IsClosed() && !m_pPane->IsFloating())
					OnViewRightbat();
			}
			else
			{
				// Load the previous state for docking panes.
				if (m_pPane->IsClosed() && !m_preFullScrIsClosed)
				{
					OnViewRightbat();
				}
			}
		}
		else	
		{
			CXTPDockingPane* pPane = (CXTPDockingPane*)lParam;

			if (!pPane->IsValid())
			{
				pPane->Attach(&m_DialogPanel);
				m_DialogPanel.ShowWindow(SW_SHOW);
			}
		}
		return TRUE;
	}
	return FALSE;
}

void KSceneEditorView::OnNormalMapGen()
{
    static KGNormalMapCreatorDlg dlg;
    static bool bFirst = true;

    if (bFirst)
        dlg.Create(KGNormalMapCreatorDlg::IDD, this);

    bFirst = false;

    dlg.ShowWindow(SW_SHOW);
}

void KSceneEditorView::OnViewRightbat()
{
	if (m_pPane->IsClosed())
	{
		m_DialogPanel.ShowWindow(SW_HIDE);
		CXTPDockingPane* pPane = m_pPane;
		pPane->Detach();
		m_paneManager.DestroyPane(pPane);
		// Create docking panes.
		m_pPane = m_paneManager.CreatePane(
			IDR_MAINFRAME, CRect(0, 0,230, 120), dockRightOf);
		m_pPane->SetOptions(xtpPaneNoHideable);		
	}
	else
		m_paneManager.CloseAll();
}
void KSceneEditorView::OnUpdateViewRightbat(CCmdUI *pCmdUI)
{	
	pCmdUI->SetCheck(!m_pPane->IsClosed());
}
//)


void KSceneEditorView::OnTopview()
{
	BOOL bShow = m_DialogTopView.IsWindowVisible();
	m_DialogTopView.ShowWindow(bShow ? SW_HIDE : SW_SHOW);
}
//////////////////////////////////////////////////////////////////////////////
//by huangjinshou 2007.11.16
void KSceneEditorView::OnAddSelTexture()
{	
	GETEDITOR();
	BOOL bOpen = pEditor->GetOpenAddTexture();
	pEditor->SetOpenAddTexture(TRUE);	
}

void KSceneEditorView::OnRemoveSelTexture()
{
	GETEDITOR();
	BOOL bOpen = pEditor->GetOpenAddTexture();
	pEditor->SetOpenAddTexture(FALSE);	
}
//////////////////////////////////////////////////////////

void KSceneEditorView::OnShowprofile()
{
	if(NULL == g_pEngineManager)
		return;
	if(g_pEngineManager->GetRenderOption(RENDER_INFO))
	{
		g_pEngineManager->SetRenderOption(RENDER_INFO, FALSE);
	}
	else
	{
		g_pEngineManager->SetRenderOption(RENDER_INFO, TRUE);
	}
}

void KSceneEditorView::OnShowVideoInfo()
{
	if(NULL == g_pEngineManager)
		return;
    if(g_pEngineManager->GetRenderOption(RENDER_VIDEO_MEMERY_INFO))
    {
       g_pEngineManager->SetRenderOption(RENDER_VIDEO_MEMERY_INFO, FALSE);
    }
    else
    {
       g_pEngineManager->SetRenderOption(RENDER_VIDEO_MEMERY_INFO, TRUE);
    }
}


void KSceneEditorView::OnUpdateShowprofile(CCmdUI *pCmdUI)
{
// 	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
// 	KG3DSceneSceneEditor* pScene = static_cast<KG3DSceneSceneEditor*>(pDoc->GetScene());
// 	if(!pScene)
// 		return;
	if(NULL == g_pEngineManager)
		return;
    BOOL b = g_pEngineManager->GetRenderOption(RENDER_INFO);
	pCmdUI->SetCheck(!!b);
}

void KSceneEditorView::OnShowsnaptool()
{
	m_DialogSnapTool.UpdateDataButton();
    m_DialogSnapTool.ShowWindow(TRUE);
    m_DialogSnapTool.CenterWindow(this);
}

void KSceneEditorView::OnLayertool()
{
	m_DialogLayer.ShowWindow(TRUE);
}

void KSceneEditorView::OnModifyvisibleset()
{
	HRESULT hRetCode = E_FAIL;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
    GETEDITOR();
	
	ULONG nCount = 0;
	//pEditor->GetSelectedEntityCount(&nCount);
	nCount = KSF::GetSelectionCount(pEditor);

	if(nCount == 0)
		return;

	IEKG3DSceneEntity *piEntity = NULL;//&(*pEditor->m_listSelectedEntity.begin());
	//pEditor->GetSelectedEntity(0, &piEntity);
	piEntity = KSF::GetFirstSelectedEntity(pEditor);
    
    hRetCode = piEntity->GetType(&EntityType);
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	if(EntityType == SCENEENTITY_OBJECT)
	{
		IEKG3DRepresentObject* pObject = NULL;
        hRetCode = piEntity->GetPointer((VOID **)&pObject);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
		if(pObject->GetType() == REPRESENTOBJECT_VISIBLESET)
		{
			IEKG3DRepresentObjectVisibleEntitySet* pSet = 
				(IEKG3DRepresentObjectVisibleEntitySet*)(pObject);

			m_DialogVisibleSet.SetVisibleEntitySet(pEditor,pSet);
			m_DialogVisibleSet.ShowWindow(SW_SHOW);
		}
	}
Exit0:
    return;
}

void KSceneEditorView::OnAddmeasuretool()
{
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	GETEDITOR();

	pEditor->AddOneMeasureTool();
}

void KSceneEditorView::OnUpdateAddmeasuretool(CCmdUI *pCmdUI)
{
	pCmdUI->Enable(TRUE);
}

void KSceneEditorView::OnResetGroupPose()
{
    GETEDITOR();
    IEKG3DRepresentObject* pObject = NULL;
    pEditor->GetFirstSelectedObject(&pObject);
    if(!pObject)
        return;

    int nModelCount = 0;
    pObject->GetModelCount(&nModelCount);
    if(nModelCount<=1)
        return;

    pObject->ResetGroupPose();
}

void KSceneEditorView::OnGroupsaveas()
{
	GETEDITOR();
	IEKG3DRepresentObject* pObject = NULL;
	pEditor->GetFirstSelectedObject(&pObject);
	if(!pObject)
		return;

	int nModelCount = 0;
	pObject->GetModelCount(&nModelCount);
	if(nModelCount<=1)
		return;

	CFileDialog dlg(false, "Group", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "组合文件 (*.Group)|*.Group||");
	if(dlg.DoModal() != IDOK)
		return;

	TCHAR Name[256];
	wsprintf(Name,"%s",dlg.GetPathName());

	pObject->SaveToIniFile(Name,0);

}

BOOL KSceneEditorView::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	BOOL bRet = CBaseView::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);

	return bRet;
}

void KSceneEditorView::OnStartrecord()
{
	HRESULT hRetCode = E_FAIL;
    // get export file name
	char  szFolder[MAX_PATH] = { 0 };
	wsprintf(szFolder,"%s\\MovieRecord",g_szDefWorkDirectory);
	mkdir(szFolder);

	g_pEngineManager->MovieRecord( szFolder, 1, IPPSPEC_FILTER_NOT, IPPSPEC_QUALITY_CINEMATIC2, IPPSPEC_MPEG2, IPPSPEC_FPS_25);//by dengzhihui Refactor 2006年11月6日
	ASSERT( g_pEngineManager->MovieReady() );//by dengzhihui Refactor 2006年11月6日
	
	//KG3DSceneSceneEditor* pEditor = GetDocScene();
    //KGLOG_PROCESS_ERROR(pEditor);
	GETEDITOR();
    IEKG3DSceneOutputWnd *piPrimaryWnd = NULL;
    hRetCode = pEditor->GetPrimaryWindow(&piPrimaryWnd);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
    {
        RECT rect;
        piPrimaryWnd->GetRect(&rect);
		g_pEngineManager->MovieViewport(rect);//by dengzhihui Refactor 2006年11月6日
    }
Exit0:
    return;
}

void KSceneEditorView::OnUpdateStartrecord(CCmdUI *pCmdUI)
{
	_ASSERTE(pCmdUI);
	if (pCmdUI)
		pCmdUI->Enable(!g_pEngineManager->MovieReady());//by dengzhihui Refactor 2006年11月6日
}

void KSceneEditorView::OnStoprecord()
{
	ASSERT(g_pEngineManager->MovieReady());//by dengzhihui Refactor 2006年11月6日
	g_pEngineManager->MovieStop( );//by dengzhihui Refactor 2006年11月6日
	ASSERT( !g_pEngineManager->MovieReady() );//by dengzhihui Refactor 2006年11月6日
}

void KSceneEditorView::OnUpdateStoprecord(CCmdUI *pCmdUI)
{
	_ASSERTE(pCmdUI);
	if (pCmdUI)
		pCmdUI->Enable(g_pEngineManager->MovieReady());//by dengzhihui Refactor 2006年11月6日
}

IEKG3DSceneSceneEditor* KSceneEditorView::GetDocScene()
{
	KSceneEditorDoc* pDoc =  GetDocument();
	_ASSERTE(pDoc);
	if (pDoc)
	{
		IEKG3DScene* p3DScene = pDoc->GetScene();
		//KG3DSceneSceneEditor* pEditor = static_cast<KG3DSceneSceneEditor*> (p3DScene);
		IEKG3DSceneSceneEditor* pEditor = NULL;
		p3DScene->GetSceneSceneEditor(&pEditor);
		_ASSERTE(pEditor);
		return pEditor;
	}
	return NULL;
}

void KSceneEditorView::OnPlayerMode()
{
	GETEDITOR();
	GETSCENEBASE();
	//pEditor->m_bEditing = !pEditor->m_bEditing;
	pBase->SetEditing(!pBase->GetEditing());

	//(by dengzhihui Refactor 2006年11月6日
	if (!pBase->GetEditing())
		pEditor->SetNPCPosition(TRUE, FALSE, NULL);
	//)
}

void KSceneEditorView::OnPlayerMode(CCmdUI *pCmdUI)
{
	//_ASSERTE(pCmdUI);
	if (pCmdUI)
	{
		GETEDITOR();GETSCENEBASE();
		_ASSERTE(pEditor);
		if (pEditor)
			pCmdUI->SetCheck(!pBase->GetEditing());
	}
}

void KSceneEditorView::OnScreenShot()
{
    IKG3DUI* pUI = NULL;
    g_pEngineManager->GetUI(&pUI);

    if (pUI)
    {
        pUI->ScreenShot("__screen_shot_0.jpg", D3DXIFF_JPG, 100);
    }
}

void KSceneEditorView::OnPlayerSpeedUp()
{
	GETEDITOR();GETNPC();
	//pEditor->m_pRepresentNPC->m_fMoveSpeed += 50.0f;
	//pEditor->m_pRepresentNPC->m_fMoveSpeed = min(10000.0f, pEditor->m_pRepresentNPC->m_fMoveSpeed);
	pNpc->SetMoveSpeed(pNpc->GetMoveSpeed() + 50.0f);
	pNpc->SetMoveSpeed(min(10000.0f, pNpc->GetMoveSpeed()));
}

void KSceneEditorView::OnPlayerSpeedDown()
{
	GETEDITOR();GETNPC();
// 	pEditor->m_pRepresentNPC->m_fMoveSpeed -= 50.0f;
// 	pEditor->m_pRepresentNPC->m_fMoveSpeed = max(100.0f, pEditor->m_pRepresentNPC->m_fMoveSpeed);	
	pNpc->SetMoveSpeed(pNpc->GetMoveSpeed() - 50.0f);
	pNpc->SetMoveSpeed(max(100.0f, pNpc->GetMoveSpeed()));
}

void KSceneEditorView::OnPlayerModeUpdate(CCmdUI *pCmdUI)
{
	GETEDITOR();
	pCmdUI->Enable(!GetEditState(pEditor));//pEditor->m_bEditing);

}


void KSceneEditorView::OnOpenAPTPlayObjMode()
{
    GETEDITOR();
    pEditor->SetUseCrossPlayMode(!pEditor->IsUseCrossPlayMode());
}

void KSceneEditorView::OnModeSelectUpdate(CCmdUI *pCmdUI)
{
	GETEDITOR();
	pCmdUI->SetCheck(GetEditState(pEditor) == SCENESTATE_SELECT);//pEditor->m_dwEditState == SCENESTATE_SELECT);	
}
void KSceneEditorView::OnModeScaleUpdate(CCmdUI *pCmdUI)
{
	GETEDITOR();
	pCmdUI->SetCheck(GetEditState(pEditor) == SCENESTATE_SCALE);//pEditor->m_dwEditState == SCENESTATE_SCALE);
}
void KSceneEditorView::OnModeMoveUpdate(CCmdUI *pCmdUI)
{
	GETEDITOR();
	pCmdUI->SetCheck(GetEditState(pEditor) == SCENESTATE_MOVE);
}
void KSceneEditorView::OnModeRotateUpdate(CCmdUI *pCmdUI)
{
	GETEDITOR();
	pCmdUI->SetCheck(GetEditState(pEditor) == SCENESTATE_ROTATE);
}

void KSceneEditorView::OnEditCopy()
{
	/*GETEDITOR();
	GETSELECT();
	pSelector->CopySelectedEntity();*/

	//2009-1-12 <Copy Refactor>复制改成3dsmax的方式，不需要用Ctrl+C记录选择，而是直接用Ctrl+V就复制出当前的选择。记录选择这个操作需要考虑太多物件相关性的问题，而且也应该尽量向Max的操作看齐
	MessageBox(_T("复制不需要Ctrl+C。直接按Ctrl+V就可以复制选择的物件，请参考3dsmax的操作"));
}

void KSceneEditorView::OnEditPaste()
{
	GETEDITOR();
	GETSELECT();

    if (pSelector->GetSelectableEntitySize() <= 0)
    {
        MessageBox(TEXT("无效复制源?"), TEXT("SceneEditer"), MB_OK);
        return;
    }


    if (MessageBox(TEXT("复制当前选择的，你确定要粘贴吗？粘贴后复制出的物件会被自动选中"), TEXT("SceneEditer"), MB_YESNO) == IDYES)
	{
		//pSelector->ParseCopiedEntity();

		IEKG3DSceneEditorBase* pSceneEditBase = NULL;
		pEditor->GetSceneEditorBase(&pSceneEditBase);
		_ASSERTE(NULL != pSceneEditBase);
		if (NULL != pSceneEditBase)
		{
			pSceneEditBase->PasteSelection();
		}
	}
}

void KSceneEditorView::OnPlayCamareAni()
{
	GETEDITOR();
	GETSCENEBASE();
	pBase->ExecAction(EXEACTION_PLAY_CAM_ANI, 1, 0, 0);
}

void KSceneEditorView::OnDeleteSelectedObject()
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    BOOL bRetCode = FALSE;
    SCENEENTITYTYPE EntityType = SCENEENTITY_NONE;
	DWORD dwState=0;
   
    KSceneEditorDoc* pDoc = static_cast<KSceneEditorDoc*>(GetDocument());
    KG_PROCESS_ERROR(pDoc);

    IEKG3DScene* p3DScene = pDoc->GetScene();
    KG_PROCESS_ERROR(p3DScene);

	//KG3DSceneSceneEditor* pEditor = static_cast<KG3DSceneSceneEditor*>(p3DScene);
	GETEDITOR();
	IEKG3DSceneLogicalEditorBase* pLogicBase = NULL;
	/*pEditor->GetIELogicScene(&pLogicBase);
	_ASSERTE(pLogicBase);
	if (!pLogicBase) return;*/

	//KScene*	pLogicScene = pEditor->GetLogicalScene();	
	/*KScene * LogicScene = NULL;
	pLogicBase->GetLogicScene((LPVOID*)&LogicScene);
	if(!LogicScene)
		return;
	*/
	/////add by huangjinshou 2008-08-20/////////////////////////////
	pEditor->GetEditState(&dwState);//
	if(dwState != SCENESTATE_SELECT)
		return ; 
	///////////////////////////////////////////////////////////////
	// 开始删除引擎3D物件
	pEditor->DeleteSelectedEntity();
	

//Exit1:
    hResult = S_OK;
Exit0:
    return;

}

//当有弹簧键被执行时，会发一个消息给VIEW，wParam 为ActionID，方便更新一些界面上的状态
LRESULT KSceneEditorView::OnActionExected(WPARAM wParam, LPARAM lParam)
{
	switch(wParam)
	{
	case 16:
	case 17:
	case 18:
		if (lParam == 1)
		{
			//m_PaintTextureDialogNew.OnUpdateUI((UINT)wParam);
			m_DialogPanel.m_DialogTerrian.OnUpdateUI((UINT)wParam);
		}
		break;
	/*case 19:
		{
			KG3DSceneSceneEditor* pEditor = GetDocScene();
			_ASSERTE(pEditor);
			if (pEditor)
			{
				if(pEditor->m_dwEditState == SCENESTATE_PLACEUNIT)
				{
					KScene * pLogicalScene = pEditor->GetLogicalScene();
					if(!pLogicalScene)
						return E_FAIL;
					m_DialogPanel.m_DialogUnit.AddUnit(pLogicalScene);
				}
			}
		}
		break;*/
	}

	return 0;
}

//BOOL KSceneEditorView::OnMouseWheel(UINT, short nDelta, CPoint pt)
//{
//	GETEDITOR() FALSE;
//	GETSCENEBASE() FALSE;
//	pBase->ZoomView(nDelta, pt);	
//	return TRUE;
//}

void KSceneEditorView::OnActionUp()
{
	GETEDITOR();
	pEditor->OnCommand(ctUp);
}

void KSceneEditorView::OnActionDown()
{
	GETEDITOR();
	pEditor->OnCommand(ctDown);
}

void KSceneEditorView::OnActionLeft()
{
	GETEDITOR();
	pEditor->OnCommand(ctLeft);
}

void KSceneEditorView::OnActionRight()
{
	GETEDITOR();
	pEditor->OnCommand(ctRight);
}

void KSceneEditorView::OnActionPageUp()
{
	GETEDITOR();
	pEditor->OnCommand(ctPageUp);
}

void KSceneEditorView::OnActionPageDown()
{
	GETEDITOR();
	pEditor->OnCommand(ctPageDown);
}
//缩小笔刷
void KSceneEditorView::OnActionAdd()	//'['
{
	GETEDITOR();
	pEditor->OnCommand(ctInc);
	OnActionExected(16, 0);
}
//放大笔刷
void KSceneEditorView::OnActionDec()	//']'
{
	GETEDITOR();
	pEditor->OnCommand(ctDec);
	OnActionExected(16, 0);
}
//隐藏显示物件
void KSceneEditorView::OnHideObject()
{
	GETEDITOR();
	pEditor->OnCommand(ctHideObject);
}
//隐藏显示地表
void KSceneEditorView::OnHideTerrain()
{
	GETEDITOR();
	pEditor->OnCommand(ctHideTerrain);
}
//隐藏显示草
void KSceneEditorView::OnHideGrass()
{
	GETEDITOR();
	pEditor->OnCommand(ctHideGrass);
}
//隐藏显示灌木
void KSceneEditorView::OnHideFrutex()
{
	GETEDITOR();
	pEditor->OnCommand(ctHideTree);
}
//隐藏NPC攻击范围
void KSceneEditorView::OnHideNPCAttackRange()
{
	GETEDITOR();
	pEditor->OnCommand(ctHideNPCAttackRange);
}
//隐藏显示物件管理器
void KSceneEditorView::OnShowObjectManager()
{
	GETEDITOR();	
	if(!m_lpDialogObjectMgr && pEditor)
	{
		m_lpDialogObjectMgr = new KSceneSceneEditorDialogObjectMgr;
		m_lpDialogObjectMgr->Create(KSceneSceneEditorDialogObjectMgr::IDD,NULL);
		m_lpDialogObjectMgr->SetSceneEditor(pEditor);
	}
	_ASSERTE(m_lpDialogObjectMgr);
	if (m_lpDialogObjectMgr)
		m_lpDialogObjectMgr->ShowWindow(TRUE);
}
//隐藏显示灯光属性
void KSceneEditorView::OnShowLightProperty()
{
	/*
		GETEDITOR();
			GETSCENE();
		    DWORD dwLightCount = 0;
		    pScene->GetLightCount(&dwLightCount);
			if(dwLightCount)
				g_GetLightPropertyDlg().Init(pScene);*/
	MessageBox(_T("这部分功能已经移到\"场景参数设定 > 灯光\""));	
}

void KSceneEditorView::OnHome()
{
	GETEDITOR();
	pEditor->OnCommand(ctHome);
}

void KSceneEditorView::OnEnd()
{
	GETEDITOR();
	pEditor->OnCommand(ctEnd);
}

void KSceneEditorView::OnAddKeyFrame()
{
	GETEDITOR();
	pEditor->OnCommand(ctAddKeyFrame);
}

void KSceneEditorView::OnDropObj()
{
	GETEDITOR();
	pEditor->OnCommand(ctDropObject);
}

//快速移动当前选中的物件
void KSceneEditorView::OnGetSelectedObjInHand()
{
	GETEDITOR();
	pEditor->GetSelectedObjectInHand();
}

inline DWORD KSceneEditorView::GetEditState(IEKG3DSceneSceneEditor* pEditor)
{
	DWORD dwEditing = 0;
	if (pEditor)
	{
		pEditor->GetEditState(&dwEditing);
	}
	return dwEditing;
}

extern DWORD TransToEngineFormat(long lLogicInfo);

void KSceneEditorView::OnLogicalpoly()
{
	KSceneEditorDoc* pDoc = GetDocument();
	GETEDITOR();

	IEKG3DRepresentPoly* pIEPoly = pEditor->GetSelectedPoly();
	if(pIEPoly)
	{
		KSceneDialogLogicalPoly dlg;

		int nCount = pDoc->m_lpLogicalform->get_Count();
		long lLogicInfo = 0;
		for (int i = 0; i < nCount; i ++)
		{
			_bstr_t cbstr = "";
			_bstr_t cbstr2 = "";
			BSTR bstr = cbstr.Detach();
			BSTR bstr2 = cbstr2.Detach();

			pDoc->m_lpLogicalform->GetItem(i,&bstr2, &lLogicInfo, &bstr);

			cbstr.Attach(bstr);
			cbstr2.Attach(bstr2);

			TCHAR Name[MAX_PATH];
			wsprintf(Name,"%S",bstr2);
			TCHAR ScriptName[MAX_PATH];
			wsprintf(ScriptName,"%S",bstr);

			DWORD dwModify = TransToEngineFormat(lLogicInfo);

			dlg.AddLogicalSet(dwModify,i,Name,ScriptName,dwModify);
		}

		dlg.SetPoly(pIEPoly);
		dlg.DoModal();
	}
}

void KSceneEditorView::OnAddmap()
{
	KSceneEditorDoc* pDoc = GetDocument();
	GETEDITOR();

	CFileDialog dlg(true, "Map", 0, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, "剑网3D地图文件(*.Map)|*.Map||");
	if(dlg.DoModal() == IDOK)
	{
		TCHAR FileName[MAX_PATH];
		//TCHAR RetFileName[MAX_PATH];
		wsprintf(FileName,"%s",dlg.GetPathName());
		pEditor->AddOneMap(FileName);
	
	}
}

void KSceneEditorView::OnLoadDefaultMap()
{
	KSceneCommonParamDialog ParamDlg;
	KG3DTERRAIN_LOAD_PARAM LoadParam;
	int nRetParamIndex[13];
	nRetParamIndex[0] = ParamDlg.AddAnParam(_T("开始位置X"), LoadParam.nStart_X,TRUE);
	nRetParamIndex[1] = ParamDlg.AddAnParam(_T("开始位置Y"), LoadParam.nStart_Y,TRUE);
	nRetParamIndex[2] = ParamDlg.AddAnParam(_T("宽度(128*N)(米)"), LoadParam.nWidth,TRUE);
	nRetParamIndex[3] = ParamDlg.AddAnParam(_T("高度(128*N)(米)"), LoadParam.nHeight,TRUE);
	nRetParamIndex[4] = ParamDlg.AddAnParam(_T("创建Section(只埋1或0,下同)"), LoadParam.nSection,TRUE);
	nRetParamIndex[5] = ParamDlg.AddAnParam(_T("创建Region(0为不创建)"), LoadParam.nRegion,TRUE);
	nRetParamIndex[6] = ParamDlg.AddAnParam(_T("创建ZoneTerrainBlock"), LoadParam.nZoneTerrainBlock,TRUE);
	nRetParamIndex[7] = ParamDlg.AddAnParam(_T("创建SectionTerrainBlock"), LoadParam.nSectionTerrainBlock,TRUE);
	nRetParamIndex[8] = ParamDlg.AddAnParam(_T("创建RegionTerrainBlock"), LoadParam.nRegionTerrainBlock,TRUE);
	nRetParamIndex[9] = ParamDlg.AddAnParam(_T("高度图"), LoadParam.strHeightMapName ,TRUE);
	nRetParamIndex[10] = ParamDlg.AddAnParam(_T("高度图宽度"), LoadParam.nHeightMapWidth ,TRUE);
	nRetParamIndex[11] = ParamDlg.AddAnParam(_T("最低高度"), LoadParam.nHeightLowest ,TRUE);
	nRetParamIndex[12] = ParamDlg.AddAnParam(_T("高度差"), LoadParam.nHeightTotal,TRUE);

	if (IDOK != ParamDlg.DoModal())
		return;
	KG3DTERRAIN_LOAD_PARAM DefParam;
	if (! ParamDlg.GetData(nRetParamIndex[0], LoadParam.nStart_X))
		LoadParam.nStart_X = DefParam.nStart_X;
	if(! ParamDlg.GetData(nRetParamIndex[1], LoadParam.nStart_Y))
		LoadParam.nStart_Y = DefParam.nStart_Y;
	if(! ParamDlg.GetData(nRetParamIndex[2], LoadParam.nWidth))
		LoadParam.nWidth = DefParam.nWidth;
	if(! ParamDlg.GetData(nRetParamIndex[3], LoadParam.nHeight))
		LoadParam.nHeight = DefParam.nHeight;
	if(! ParamDlg.GetData(nRetParamIndex[4], LoadParam.nSection))
		LoadParam.nSection = DefParam.nSection;
	if(! ParamDlg.GetData(nRetParamIndex[5], LoadParam.nRegion))
		LoadParam.nRegion = DefParam.nRegion;
	if(! ParamDlg.GetData(nRetParamIndex[6], LoadParam.nZoneTerrainBlock))
		LoadParam.nZoneTerrainBlock = DefParam.nZoneTerrainBlock;
	if(! ParamDlg.GetData(nRetParamIndex[7], LoadParam.nSectionTerrainBlock))
		LoadParam.nSectionTerrainBlock = DefParam.nSectionTerrainBlock;
	if(! ParamDlg.GetData(nRetParamIndex[8], LoadParam.nRegionTerrainBlock))
		LoadParam.nRegionTerrainBlock = DefParam.nRegionTerrainBlock;
	if(! ParamDlg.GetData(nRetParamIndex[9], LoadParam.strHeightMapName))
		LoadParam.strHeightMapName = DefParam.strHeightMapName;
	if(! ParamDlg.GetData(nRetParamIndex[10], LoadParam.nHeightMapWidth))
		LoadParam.nHeightMapWidth = DefParam.nHeightMapWidth;
	if(! ParamDlg.GetData(nRetParamIndex[11], LoadParam.nHeightLowest))
		LoadParam.nHeightLowest = DefParam.nHeightLowest;
	if(! ParamDlg.GetData(nRetParamIndex[12], LoadParam.nHeightTotal))
		LoadParam.nHeightTotal = DefParam.nHeightTotal;
	do 
	{
		GETEDITOR();
		if( LoadParam.nWidth > 3072 || LoadParam.nHeight > 3072)
			MessageBox("创建失败，地图宽度和高度上限值为3072","提示！");
		else
			pEditor->LoadDefaultTerrain(&LoadParam);
	} while(false);
	return;
}
void KSceneEditorView::OnSoundballedit()
{
	KSceneEditorSoundBallDialog dlg;
	dlg.DoModal();

}

void KSceneEditorView::OnEnablePVS()
{
	GETEDITOR();
	BOOL bPVSState = pEditor->GetPVSState();
	pEditor->EnablePVS(!bPVSState);
}



void KSceneEditorView::OnPlayscenesound()
{
	GETEDITOR();
	BOOL b = pEditor->IsSceneSoundEnable();
	pEditor->EnableSceneSound( !b );
}

void KSceneEditorView::OnUpdatePlayscenesound(CCmdUI *pCmdUI)
{
	GETEDITOR();
	BOOL b = pEditor->IsSceneSoundEnable();
	pCmdUI->SetCheck(b);
}

void KSceneEditorView::OnRendersound()
{
	GETEDITOR();
	BOOL b = pEditor->IsSceneSoundVisiable();
	pEditor->VisiableSceneSound( !b );
	if (!b)
	{
		pEditor->VisibleSoundRange(!b);
	}
}

void KSceneEditorView::OnRenderSoundBallRange()
{
	GETEDITOR();
	BOOL b = pEditor->IsSoundRangeVisible();
	pEditor->VisibleSoundRange(!b);
}

void KSceneEditorView::OnUpdateRenderSoundBallRange(CCmdUI *pCmdUI)
{
	GETEDITOR();
	BOOL b = pEditor->IsSoundRangeVisible();
	pCmdUI->SetCheck(b);
}

void KSceneEditorView::OnUpdateRendersound(CCmdUI *pCmdUI)
{
	GETEDITOR();
	BOOL b = pEditor->IsSceneSoundVisiable();
	pCmdUI->SetCheck(b);
}

void KSceneEditorView::OnIsRayUseBoneBox()
{
	GETEDITOR();
	if(pEditor->GetUseBoneBox())
	{
		pEditor->SetUseBoneBox(FALSE);
	}
	else
	{
		pEditor->SetUseBoneBox(TRUE);
	}
}

void KSceneEditorView::OnRefreshshader()
{
	if (g_pGraphicsTool)
	{
		g_pGraphicsTool->RefreshShader();
	}
	
}
//void KSceneEditorView::OnPvssetup()
//{
//	GETEDITOR();
//
//	if(!m_lpDialogPVS)
//	{
//		m_lpDialogPVS = new KSceneEditorPVSSetupDialog();
//		KGLOG_PROCESS_ERROR(m_lpDialogPVS);
//		m_lpDialogPVS->SetEditor(pEditor);
//		m_lpDialogPVS->Create(KSceneEditorPVSSetupDialog::IDD);
//	}
//	m_lpDialogPVS->ShowWindow(SW_SHOW);
//
//Exit0:
//	return;
//}

void KSceneEditorView::OnAntiPortalSetup()
{
	GETEDITOR();

	if(!m_lpDialogAntiPortal)
	{
		m_lpDialogAntiPortal = new KSceneEditorAntiPortalSetupDialog();
		KGLOG_PROCESS_ERROR(m_lpDialogAntiPortal);
		m_lpDialogAntiPortal->SetEditor(pEditor);
		m_lpDialogAntiPortal->Create(KSceneEditorAntiPortalSetupDialog::IDD);
	}
	m_lpDialogAntiPortal->ShowWindow(SW_SHOW);

Exit0:
	return;
}



void KSceneEditorView::OnTerrainselect()
{
	GETEDITOR();
	pEditor->SetEditState(SCENESTATE_TERRAINSELECTION);//pEditor->m_dwEditState = SCENESTATE_SELECT;
	//pEditor->OnToggleStateSelect();
}

void KSceneEditorView::OnRoad()
{
	GETEDITOR();
	// TODO: Add your command handler code here
	if (!m_lpDialogRoad)
	{
		m_lpDialogRoad = new KSceneEditorRoadSetup();
		KGLOG_PROCESS_ERROR(m_lpDialogRoad);

        IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr = pEditor->GetIEKG3DSceneOutDoorSpaceMgr();
        m_lpDialogRoad->SetEditor(pEditor,pOutDoorSpaceMgr);

		m_lpDialogRoad->Create(KSceneEditorRoadSetup::IDD);
	}
	m_lpDialogRoad->ShowWindow(SW_SHOW);
Exit0:

	;
}

void KSceneEditorView::OnDetailMap()
{
	GETEDITOR();
	if(!m_lpDialogDetailMap)
	{
		m_lpDialogDetailMap = new KSceneEditorDetialMapDialog();
		KGLOG_PROCESS_ERROR(m_lpDialogDetailMap);

		IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr = pEditor->GetIEKG3DSceneOutDoorSpaceMgr();
		m_lpDialogDetailMap->SetEditor(pEditor,pOutDoorSpaceMgr);
		m_lpDialogDetailMap->Create(KSceneEditorDetialMapDialog::IDD);
		m_lpDialogDetailMap->SetPotinerToTgaButton();
	}
	m_lpDialogDetailMap->ShowWindow(SW_SHOW);

Exit0:
	;
}

void KSceneEditorView::OnWallsetup()
{
	GETEDITOR();

	if(!m_lpDialogWall)
	{
		m_lpDialogWall = new KSceneEditorWallSetupDialog();
		KGLOG_PROCESS_ERROR(m_lpDialogWall);

		m_lpDialogWall->Create(KSceneEditorWallSetupDialog::IDD);
	}

	IEKG3DWallDefine* pWallDefine = NULL;
	IEKG3DWall* pWall = NULL;
	pWall = pEditor->GetFirstSelectedObjectWall();
	if(pWall)
	{
		IEKG3DWallDefine* pWallDefine = pWall->GetWallDefine();
		if(pWallDefine)
		{
			pWallDefine->AddRef();
		}
		m_lpDialogWall->SetWallDefine(pWallDefine,pEditor,pWall);
		m_lpDialogWall->ShowWindow(SW_SHOW);
	}
	else if(SUCCEEDED(pEditor->GetDefaultWallDefine(&pWallDefine)))
	{
		m_lpDialogWall->SetWallDefine(pWallDefine,pEditor,pWall);
		m_lpDialogWall->ShowWindow(SW_SHOW);
	}

Exit0:
	return;

}

void KSceneEditorView::OnRiver()
{
	GETEDITOR();
	// TODO: Add your command handler code here
	if (!m_lpDialogRiver)
	{
		m_lpDialogRiver = new KSceneEditorRiver();
		KGLOG_PROCESS_ERROR(m_lpDialogRiver);

		IEKG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr = pEditor->GetIEKG3DSceneOutDoorSpaceMgr();
		m_lpDialogRiver->SetEditor(pEditor,pOutDoorSpaceMgr);

		m_lpDialogRiver->Create(KSceneEditorRiver::IDD);
	}
	m_lpDialogRiver->ShowWindow(SW_SHOW);
Exit0:

	;
}

void KSceneEditorView::OnMenuentitynum()
{
	// TODO: Add your command handler code here
	GETEDITOR();
	DWORD dwNum = pEditor->GetEntityNum();
	CString str;
	str.Format("当前场景，Entity数目： %i ", dwNum);

	AfxMessageBox(str);
}


void KSceneEditorView::OnMoveNPC()
{
	
	D3DXVECTOR3 vNPCNewPos(0,0,0);
	std::vector<INT>	LogicPoints;
	KSceneCommonParamDialog Dialog;
	INT nHandle1 = Dialog.AddAnParam(_T("输入跳到的位置(x,y,z): "), vNPCNewPos,TRUE);
	INT nHandle2 = Dialog.AddAnParam(_T("或者输入逻辑坐标(x,z)："), LogicPoints,TRUE);
	if (IDOK != Dialog.DoModal())
		return;	

	BOOL bHasChangeHandle1 = FALSE;
	BOOL bRet = Dialog.GetData(nHandle1, vNPCNewPos, &bHasChangeHandle1);
	if(! bRet)
		return;

	GETEDITOR();
	if (NULL == pEditor)
		return;

	HRESULT hr = E_FAIL;
	if (bHasChangeHandle1)
	{
		hr = pEditor->SetNPCPosition(FALSE, FALSE, &vNPCNewPos);
	}
	else	//如果输入的是逻辑坐标而不是Scene坐标，则使用逻辑坐标
	{
		bRet = Dialog.GetData(nHandle2, LogicPoints);
		if (bRet && LogicPoints.size() >= 2 && LogicPoints.size() <= 3)	//如果输入两个值，就取默认Z，否则就用第三个值
		{
			D3DXVECTOR3 vScene(0,0,0);
			if (LogicPoints.size() == 3)
			{
				D3DXVECTOR3 vLogic((FLOAT)(LogicPoints[0]), (FLOAT)(LogicPoints[1]), (FLOAT)(LogicPoints[2]));
				hr = pEditor->LogicalToScene(&vScene, vLogic.x, vLogic.y, vLogic.z, FALSE);
			}
			else
			{
				D3DXVECTOR3 vLogic((FLOAT)(LogicPoints[0]), (FLOAT)(LogicPoints[1]), 0);
				
				hr = pEditor->LogicalToScene(&vScene, vLogic.x, vLogic.y, vLogic.z, TRUE);

				hr = pEditor->GetGroundHeight(vScene.y, vScene.x, vScene.z);	//如果不指定第三个点，那么用地形高度
			}

			if (SUCCEEDED(hr))
			{
				hr = pEditor->SetNPCPosition(FALSE, FALSE, &vScene);
			}
		}
	}
	if (FAILED(hr))
	{
		MessageBox(_T("参数不正确，跳转失败"));
	}
		
}

void KSceneEditorView::OnEngineCommand()
{
	std::tstring strCommand = _T("");

	KSceneCommonParamDialog dlg;
	INT handleParam = dlg.AddAnParam(_T("输入命令："), strCommand,TRUE);

	if(handleParam < 0)
		return;

	if(IDOK != dlg.DoModal())
		return;

	BOOL bRet = dlg.GetData(handleParam, strCommand);
	if(! bRet)
		return;

	HRESULT hr = g_pEngineManager->ExecuteCommand(strCommand.c_str());
	if (FAILED(hr))
	{
		MessageBox(_T("运行命令失败，可能是命令解释错误"));
	}
}

void KSceneEditorView::OnModifySoundBall(IEKG3DRepresentObject *pObj)
{
	TCHAR strFileName[MAX_PATH];
	KSceneEditorSoundBallDialog Dlg;
	IEKG3DRepresentObjectSoundBall *pSoundBall = dynamic_cast<IEKG3DRepresentObjectSoundBall*>(pObj);
	KGLOG_PROCESS_ERROR(pSoundBall);

	pObj->GetName(strFileName, MAX_PATH);
	pSoundBall->Stop();
	Dlg.m_strDefaultOpen = strFileName;
	Dlg.DoModal();
	pSoundBall->Start();
Exit0:
	;
}

void KSceneEditorView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView)
{
	// TODO: Add your specialized code here and/or call the base class
	KSceneEditorDoc* pDoc = (KSceneEditorDoc*) GetDocument();
	KGLOG_PROCESS_ERROR(pDoc);

	IEKG3DScene* pScene = pDoc->GetScene();
	KG_PROCESS_ERROR(pScene);

	g_GetEngineManager()->SetSoundFocusScene(pScene);

Exit0:
	CBaseView::OnActivateView(bActivate, pActivateView, pDeactiveView);
}

void KSceneEditorView::OnShortcutkey()
{
	if(!m_lpDialogKey)
	{
		m_lpDialogKey = new KGShortcutKeyDlg();
		KGLOG_PROCESS_ERROR(m_lpDialogKey);

		m_lpDialogKey->Create(KGShortcutKeyDlg::IDD);
	}
	m_lpDialogKey->FillKeyList();
	m_lpDialogKey->ShowWindow(SW_SHOW);
Exit0:
	;
}

