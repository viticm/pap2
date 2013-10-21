// SceneEditor.cpp : Implementation of CSceneEditor
#include "stdafx.h"
#include "SceneEditor.h"
#include "IEEditor.h"
#include "IEKG3DScene.h"
#include "IEOther.h"
#include "KGSTLWrappers.h"
#include "KG3DSceneSelectBase.h"
#include "IEKG3DRegionInfoManager.h"
#include "kg3denginetypes.h"
#include "KG3DGraphicsTool.h"
#include "assert.h"


extern IEKG3DSceneSceneEditor* g_pSceneEditor;
extern IEKG3DScene* g_pIEScene;
extern INT g_iSceneOutputWindowID;
extern IEKG3DSceneEditorBase* g_pEditorBase;
extern IEKG3DEngineManager* g_pEngineManager;
extern IEKG3DSceneLogicalEditorBase* g_pLogicEditorBase;
extern KG3DGraphicsTool g_cGraphicsTool;
extern IEKG3DScene* g_pNpcScene;
extern IEKG3DScene* g_pDoodadScene;
// CSceneEditor


STDMETHODIMP CSceneEditor::AddWindowing(BSTR szName, LONG ihWnd, LONG iWindowType, LONG* windowid, ULONG editState)
{
	g_pIEScene->AddOutputWindow(BSTR_TO_STRING(szName), (HWND)ihWnd, iWindowType, 0, (int*)windowid);
	if (iWindowType == OUTPUTWND_SCENEEDITOR_MAIN)
		g_iSceneOutputWindowID = *windowid;
	g_pSceneEditor->SetEditState(editState);
	return S_OK;
}
STDMETHODIMP CSceneEditor::UpdateTopPosAsMainPos()
{
	D3DXVECTOR3 CamMainPos = g_pSceneEditor->GetCameraMainPos();
	g_pSceneEditor->SetCameraTopPos(CamMainPos.x, CamMainPos.z);
	return S_OK;
}
STDMETHODIMP CSceneEditor::RemoveWindowing(LONG iWindowID)
{
	g_pSceneEditor->RemoveOutputWindow(iWindowID);
	return S_OK;
}

STDMETHODIMP CSceneEditor::GetCameraPosLookat(AtlVector3* pos, AtlVector3* lookat)
{
	g_pSceneEditor->GetCameraPosLookat(pos, lookat);
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetCameraPosLookat(AtlVector3* pos, AtlVector3* lookat)
{
	g_pSceneEditor->SetCameraPosLookat(pos, lookat);
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetCameraLocation(FLOAT camerax, FLOAT cameray, FLOAT cameraz, FLOAT lookatx, FLOAT lookaty, FLOAT lookatz)
{
	g_pSceneEditor->SetCameraLocation(camerax, cameray, cameraz, lookatx, lookaty, lookatz);
	return S_OK;
}
STDMETHODIMP CSceneEditor::GetSelectedEntityCount(LONG* count)
{
	//g_pSceneEditor->GetSelectedEntityCount((INT*)count);
	IEKG3DSceneSelectionTool* pSelectionTool = NULL;
	g_pSceneEditor->GetSelectionToolInterface(&pSelectionTool);
	*count = pSelectionTool->GetSelectionCount();

	return S_OK;
}
STDMETHODIMP CSceneEditor::SetFullScreen(BOOL bFullScreen)
{
	g_pEngineManager->SetFullScreen(bFullScreen);
	if (bFullScreen)
		g_pEngineManager->SetMaxViewScene(g_pIEScene);
	return S_OK;
}
STDMETHODIMP CSceneEditor::TogglePMode()
{
	if (g_pEditorBase->GetEditing())
		g_pEditorBase->SetEditing(0);
	else
		g_pEditorBase->SetEditing(1);

	//if (!g_pEditorBase->GetEditing())
	//	g_pSceneEditor->SetNPCPosition()
	return S_OK;
}
STDMETHODIMP CSceneEditor::IsPMode(BOOL* isPMode)
{
	*isPMode = g_pEditorBase->GetEditing() ? FALSE : TRUE;
	return S_OK;
}
STDMETHODIMP CSceneEditor::OnResizeWindow()
{
	g_pIEScene->OnResizeWindow(NULL);
	return S_OK;
}
STDMETHODIMP CSceneEditor::RotateSelectedObject(BSTR* newRotation)
{
	TCHAR szBuf[512];
	g_pSceneEditor->RotateSelectedObject(szBuf);
	*newRotation = STRING_TO_BSTR(szBuf);
	return S_OK;
}

STDMETHODIMP CSceneEditor::SetCameraSpeed(FLOAT fPercent)
{
	g_pSceneEditor->SetCameraSpeed(fPercent);
	return S_OK;
}
STDMETHODIMP CSceneEditor::NpcPlayNextAction()
{
	g_pSceneEditor->NpcPlayNextAction();
	return S_OK;
}
STDMETHODIMP CSceneEditor::NpcPlayPreviousAction()
{
	g_pSceneEditor->NpcPlayPreviousAction();
	return S_OK;
}

STDMETHODIMP CSceneEditor::UpdateSelectableEntityList(LONG doCount)
{
	g_pSceneEditor->UpdateSelectableEntityList(doCount);
	return S_OK;
}
STDMETHODIMP CSceneEditor::ClearSelectedEntityList()
{
	g_pSceneEditor->ClearSelectedEntityList();
	return S_OK;
}
STDMETHODIMP CSceneEditor::AddSelectedRepresentObject(LONG representObjPtr, LONG bAdd)
{
	LONG* rptr = (LONG*)representObjPtr;
	IEKG3DRepresentObject* pro = (IEKG3DRepresentObject*)rptr;
	g_pSceneEditor->SetSelected(pro, bAdd != 0 ? TRUE : FALSE);
	return S_OK;
}

STDMETHODIMP CSceneEditor::SetFirstSelectedKing()
{
	//KG3DSceneSelectBase* pselectbase = dynamic_cast<KG3DSceneSelectBase*>(g_pSceneEditor);
	//KG3DSceneEntityList::iterator it = pselectbase->m_listSelectedEntity.begin();
	//pselectbase->m_SelectedKingEntity = *it;
	return S_OK;
}
STDMETHODIMP CSceneEditor::DropSelectedEntity()
{
	g_pSceneEditor->OnCommand(ctDropObject);
	return S_OK;
}
STDMETHODIMP CSceneEditor::ZoomToObj()
{
	g_pEditorBase->ExecAction(EXEACTION_ZOOM_TO_OBJECT, 1, 0, 0);
	return S_OK;
}
STDMETHODIMP CSceneEditor::GetEditorSceneSize(LONG* width, LONG* height)
{
	g_pSceneEditor->GetSceneRect(width, height);
	return S_OK;
}

STDMETHODIMP CSceneEditor::SetObjectSelectMask(DWORD nMask)
{
	g_pSceneEditor->SetObjectSelectMaskDesign(nMask);
	return S_OK;
}

KG3DSceneEntity& KG3DSceneEntity::operator=(const KG3DSceneEntity& Other)
{
	const KG3DSceneEntityExecutor* pExe = Other.m_pExecutor;
	_ASSERTE(NULL != pExe);
	pExe->CopyEntity(Other, *this);

	_ASSERTE(NULL != m_pPointer);	//下面的很多函数根本就没有判断Pointer，所以构建的时候就应该默认是有值的
	//当然也可以每个函数对m_pPointer判空，但是这样比在构造函数中判空麻烦多了
	return *this;
}

STDMETHODIMP CSceneEditor::BeginAddPolyVertex()
{
	g_pSceneEditor->BeginAddPolyVertex();
	return S_OK;
}

STDMETHODIMP CSceneEditor::EndAddPolyVertex(ULONG dwType)
{
	g_pSceneEditor->EndAddPolyVertex(dwType);
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetPolyHeight(FLOAT fHeight)
{
	g_pSceneEditor->SetPolyHeight(fHeight);
	return S_OK;
}
STDMETHODIMP CSceneEditor::EnableRenderPoly(LONG bEnable)
{
	g_pSceneEditor->EnableRenderPoly(bEnable);
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetFocus()
{
	g_pEngineManager->SetFocusScene((IKG3DScene*)g_pIEScene);
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetRenderBlockEdge(BOOL bRenderEdge)
{
	g_pSceneEditor->SetRenderBlockEdge(bRenderEdge);
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetCameraDis(FLOAT fDis)
{
	g_pSceneEditor->SetZFar(fDis);
	return S_OK;
}

STDMETHODIMP CSceneEditor::GetCameraTopScale(FLOAT* scale)
{
	*scale = g_pSceneEditor->GetCameraTopScale();
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetCameraTopScale(FLOAT scale)
{
	g_pSceneEditor->SetCameraTopScale(scale);
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetCameraTopPos(FLOAT x, FLOAT z)
{
	g_pSceneEditor->SetCameraTopPos(x, z);
	return S_OK;
}
STDMETHODIMP CSceneEditor::GetCameraTopPos(AtlVector3* pos)
{
	D3DXVECTOR3 vPos = g_pSceneEditor->GetCameraTopPos();
	pos->x = vPos.x; pos->y = vPos.y; pos->z = vPos.z;
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetNPCPosition()
{
	g_pSceneEditor->SetNPCPosition(TRUE, TRUE, NULL);
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetCurrentOutputWindow(LONG nWindowID)
{
	g_pIEScene->SetCurrentOutputWindow(nWindowID);
	return S_OK;
}
STDMETHODIMP CSceneEditor::GetEditState(LONG* nState)
{
	g_pSceneEditor->GetEditState((DWORD*)nState);
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetEditState(LONG nState)
{
	g_pSceneEditor->SetEditState(nState);
	return S_OK;
}
STDMETHODIMP CSceneEditor::EnableShowAppointLogicalRegion(LONG bEnable)
{
	g_pSceneEditor->EnableShowAppointLogicalRegion(bEnable != 0);
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetAppointLogicalRegion(LONG xStart, LONG zStart, LONG xCount, LONG zCount)
{
	g_pIEScene->SetAppointLogicalRegion(xStart, zStart, xCount, zCount);
	return S_OK;
}
STDMETHODIMP CSceneEditor::GetAppointLogicalRegion(LONG* xStart, LONG* zStart, LONG* xCount, LONG* zCount)
{
	int xs, zs, xc, zc;
	g_pIEScene->GetAppointLogicalRegion(xs, zs, xc, zc);
	*xStart = xs; *zStart = zs; *xCount = xc; *zCount = zc;
	return S_OK;
}
STDMETHODIMP CSceneEditor::GetSetLogicDisplayParams(LONG bGet, LONG* bShowCell, LONG* bShowTerrainCell, LONG* bShowItemCell, LONG* bShowSelectedCell)
{
	IEKG3DSceneLogicalEditorBase* pLogicalEditor = NULL;
	g_pSceneEditor->GetIELogicScene(&pLogicalEditor);
	if (bGet)
	{
		*bShowCell = pLogicalEditor->GetShowLogicalCell();
		*bShowTerrainCell = pLogicalEditor->GetShowLogicalTerrenCell();
		*bShowItemCell = pLogicalEditor->GetShowLogicalItemCell();
		*bShowSelectedCell = pLogicalEditor->GetShowLogicalSelectedCell();
	}
	else
	{
		pLogicalEditor->SetShowLogicalCell(*bShowCell != 0);
		pLogicalEditor->SetShowLogicalTerrenCell(*bShowTerrainCell != 0);
		pLogicalEditor->SetShowLogicalItemCell(*bShowItemCell != 0);
		pLogicalEditor->SetShowLogicalSelectedCell(*bShowSelectedCell != 0);
	}
	return S_OK;
}
STDMETHODIMP CSceneEditor::ClearLogicModifyState()
{
	g_pLogicEditorBase->SetCurrentScriptFileName(NULL);
	g_pLogicEditorBase->SetModifyState(DWORD_FORCE, FALSE);
	g_pLogicEditorBase->SetShowState(DWORD_FORCE, FALSE);
	g_pLogicEditorBase->RecalcShowCell();
	return S_OK;
}

static DWORD dwBitInfo[][2] = {
	{MODIFY_BLOCKCHARACTER, 1}, //障碍
	{MODIFY_PUTOBJECT, 2},		//可摆放物件
	{MODIFY_STALL, 4},			//可摆摊
	{MODIFY_INDOOR, 8},			//室内区域
	{MODIFY_REST, 16},			//休息区域
	{MODIFY_RIDEHORSE, 32},		//下马区域
	{MODIFY_SCRIPTFILE, 64},	//索引文件ID
	{MODIFY_NONE, 0},			//结束
};

STDMETHODIMP CSceneEditor::SetLogicModifyState(LONG lstate, BSTR scriptName)
{
	g_pLogicEditorBase->SetCurrentScriptFileName(BSTR_TO_STRING(scriptName));
	for(int i = 0; dwBitInfo[i][0] != MODIFY_NONE; i++)
	{
		g_pLogicEditorBase->SetModifyState(eCellModifyState(dwBitInfo[i][0]),
			lstate & dwBitInfo[i][1]);
		g_pLogicEditorBase->SetShowState(eCellModifyState(dwBitInfo[i][0]),
			lstate & dwBitInfo[i][1]);
	}

	g_pLogicEditorBase->RecalcShowCell();
	return S_OK;
}
STDMETHODIMP CSceneEditor::TransToEngineFormat(LONG inMask, LONG* outMask)
{
	DWORD dwRet = 0;
	for(int i = 0; dwBitInfo[i][0] != MODIFY_NONE; i++)
	{
		dwRet |= inMask & dwBitInfo[i][1] ? dwBitInfo[i][0] : 0;
	}
	*outMask = dwRet;
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetLogicCurrentColor(LONG color)
{
	g_pLogicEditorBase->SetCurrentEditColor(color);
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetLogicBrushParams(LONG brushwidth, LONG brushtype, LONG displaywidth, LONG brushheight)
{
	g_pLogicEditorBase->SetBrushInfo(brushwidth, (eCellBrushShape)brushtype);
	g_pLogicEditorBase->SetDisplayParam(displaywidth, brushheight);
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetCurrentEditBrushIndex(LONG nIndex)
{
	g_pSceneEditor->SetCurrentEditBrushIndex(nIndex);
	return S_OK;
}
STDMETHODIMP CSceneEditor::DeleteCurrentCellInfo()
{
	g_pLogicEditorBase->DeleteCurrentCellInfo();
	g_pLogicEditorBase->RecalcShowCell();
	return S_OK;
}
STDMETHODIMP CSceneEditor::DeleteSelectedEntity()
{
	g_pSceneEditor->DeleteSelectedEntity();
	return S_OK;
}
STDMETHODIMP CSceneEditor::ModifyCurrentCellInfo(LONG oldInfo, LONG newInfo, BSTR oldScript, BSTR newScript)
{
	g_pLogicEditorBase->ModifyCurrentCellInfo(oldInfo, newInfo, BSTR_TO_STRING(oldScript), BSTR_TO_STRING(newScript));
	return S_OK;
}
STDMETHODIMP CSceneEditor::BuildPolyAndSetInHand(LONG VergeNum, FLOAT Radius, FLOAT Height, FLOAT Degree)
{
	g_pSceneEditor->BuildPolyAndSetInHand(VergeNum, Radius, Height, Degree);
	return S_OK;
}
STDMETHODIMP CSceneEditor::SetSnapToolScale(FLOAT fScale)
{
	g_pSceneEditor->SetSnapToolScale(fScale);
	return S_OK;
}

STDMETHODIMP CSceneEditor::SetSpeedTreeLeafScale(FLOAT fScale)
{
	g_pEngineManager->SetSpeedTreeLeafScale(fScale);
	return S_OK;
}

STDMETHODIMP CSceneEditor::RenderSceneSnap(BSTR fileName)
{
	ASSERT(g_pDoodadScene && g_pNpcScene);

	g_pEngineManager->ManipulateSceneList(g_pDoodadScene, FALSE);
	g_pEngineManager->ManipulateSceneList(g_pNpcScene, FALSE);

	g_pSceneEditor->RenderSceneSnap(BSTR_TO_STRING(fileName));
	
	g_pEngineManager->ManipulateSceneList(g_pNpcScene, TRUE);
	g_pEngineManager->ManipulateSceneList(g_pDoodadScene, TRUE);

	return S_OK;
}

STDMETHODIMP CSceneEditor::SetRenderTreeFlag(LONG bFlag)
{
	g_pEngineManager->SetRenderTreeFlag(bFlag != 0);
	return S_OK;
}


STDMETHODIMP CSceneEditor::AddRegionInfoManager(BSTR fileName, LONG nPixelTolerance, LONG nSampleTimes, LONG nEliminateIsoDataCount, LONG nTotalValue, ULONG uHowManyPixelsInOneRegion, ULONG* pdwHandle)
{
	KG3DREGION_INFORMER_LOADFILE_PARAM param;
	param.nPixelTolerance = nPixelTolerance;
	param.nSampleTimes = nSampleTimes;
	param.nEliminateIsoDataCount = nEliminateIsoDataCount;
	param.nTotalValue = nTotalValue;
	param.uHowManyPixelsInOneRegion = uHowManyPixelsInOneRegion;

	IEKG3DRegionInfoManagerTable* pTable = KSH::GetEngineCombinativeInterface<KG3DTYPE_RESOURCE_TABLE_TYPE, KG3DTYPE_REGION_INFO_MANAGER>();
	DWORD dwHandle = NULL;
	HRESULT hr = pTable->Add(BSTR_TO_STRING(fileName), 0, 0, (DWORD_PTR)&param, NULL, NULL, &dwHandle);
	
	if (pdwHandle != NULL)
		*pdwHandle = dwHandle;

	return S_OK;
}

STDMETHODIMP CSceneEditor::ShowRegionInfoMap(LONG ihWnd, ULONG dwHandle, LONG bShow)
{
	IEKG3DRegionInfoManagerTable* pTable = 
		KSH::GetEngineCombinativeInterface<KG3DTYPE_RESOURCE_TABLE_TYPE, KG3DTYPE_REGION_INFO_MANAGER>();
	IEKG3DRegionInfoManager* pRIM = NULL;
	HRESULT hr = pTable->GetRIM(_kg_to_smaller_size_cast<DWORD>(dwHandle), &pRIM, NULL);

	IEKG3DScene* pScene = KSH::GetEngineInterface<KG3DTYPE_SCENE>();
	assert(pScene != NULL);

	if (m_nRegionWinID >= 0)
	{
		hr = pScene->RemoveOutputWindow(m_nRegionWinID);
		m_nRegionWinID = -1;
	}

	if (bShow != 0)
	{
		BOOL bExist = FALSE;
		INT  nID = -1;
		hr = pRIM->GetRegionTexState(&bExist, &nID, NULL, 0);
		if (!SUCCEEDED(hr) || !bExist)
		{
			DWORD dwID = 0;
			hr = pRIM->GenerateRegionTex(0, &dwID);
			nID = dwID;
		}
		INT nTempRetWinID = -1;
		OUTPUTWND_TEXTURE_PARAM TempParam;
		TempParam.dwTextureID = nID;
		HRESULT hr = pScene->AddOutputWindow(_T("Region分块贴图"), (HWND)ihWnd, OUTPUTWND_SCENEEDITOR_TEXTUREVIEW, (DWORD_PTR)(&TempParam), &nTempRetWinID);
		m_nRegionWinID = nTempRetWinID;
		assert(nTempRetWinID > 0);
	}

	return S_OK;
}

STDMETHODIMP CSceneEditor::DelRegionInfoManager(ULONG dwHandle)
{
	IEKG3DRegionInfoManagerTable* pTable = KSH::GetEngineCombinativeInterface<
		KG3DTYPE_RESOURCE_TABLE_TYPE, KG3DTYPE_REGION_INFO_MANAGER>();
	HRESULT hr = pTable->Del(dwHandle);
	return S_OK;
}

STDMETHODIMP CSceneEditor::EnableRegionMask(ULONG dwHandle, LONG nEnable)
{
	IEKG3DScene* pScene = KSH::GetEngineInterfaceEx<IEKG3DScene>();
	
	if (nEnable == 0)
	{
		pScene->SetSceneSettingData(scene_setting_enable_render_terrain_mask, false);
	}
	else
	{
		IEKG3DRegionInfoManagerTable* pTable = 
			KSH::GetEngineCombinativeInterface<KG3DTYPE_RESOURCE_TABLE_TYPE, KG3DTYPE_REGION_INFO_MANAGER>();
		IEKG3DRegionInfoManager* pRIM = NULL;
		HRESULT hr = pTable->GetRIM(_kg_to_smaller_size_cast<DWORD>(dwHandle), &pRIM, NULL);
		assert(SUCCEEDED(hr) && pRIM);
		BOOL bExistTex = FALSE; 
		INT ID = 0;
		hr = pRIM->GetRegionTexState(&bExistTex, &ID, NULL, 0);
		if (! bExistTex)
		{
			pRIM->GenerateRegionTex(0, (DWORD*)&ID);
		}
		pScene->SetSceneSettingData(scene_setting_enable_render_terrain_mask, ID);
		
	}
	return S_OK;
}

STDMETHODIMP CSceneEditor::GetRegionMapInfo(ULONG dwHandle, ULONG* puHeight, ULONG* puWidth, ULONG* puPixelPerRegion, ULONG* puRegionCount, ULONG* puMemorySize)
{
	IEKG3DRegionInfoManagerTable* pTable = 
		KSH::GetEngineCombinativeInterface<KG3DTYPE_RESOURCE_TABLE_TYPE, KG3DTYPE_REGION_INFO_MANAGER>();
	IEKG3DRegionInfoManager* pRIM = NULL;
	HRESULT hr = pTable->GetRIM(_kg_to_smaller_size_cast<DWORD>(dwHandle), &pRIM, NULL);
	assert(SUCCEEDED(hr) && pRIM);
	INT nRegionCount = 0;
	hr = pRIM->GetDiffArrayCount(&nRegionCount);
	if (SUCCEEDED(hr))
	{
		*puRegionCount = nRegionCount;
	}
	KG3DREGION_INFORMER_STATE state;
	hr = pRIM->IEGetState(&state);
	if (SUCCEEDED(hr))
	{
		*puHeight = state.uInnerTexHeight;
		*puWidth = state.uInnerTexWidth;
		*puPixelPerRegion = state.uHowManyPixelsInOneRegion;
		*puMemorySize = state.uSizeInMemory;
	}

	return S_OK;
}

STDMETHODIMP CSceneEditor::GetRegionDiffValue(ULONG dwHandle, ULONG uIndex, LONG* pnValue)
{
	if (uIndex == 0)
	{
		if (m_pdwRegionDiffValue)
		{
			delete [] m_pdwRegionDiffValue;
		}
		IEKG3DRegionInfoManagerTable* pTable = 
			KSH::GetEngineCombinativeInterface<KG3DTYPE_RESOURCE_TABLE_TYPE, KG3DTYPE_REGION_INFO_MANAGER>();
		IEKG3DRegionInfoManager* pRIM = NULL;
		HRESULT hr = pTable->GetRIM(_kg_to_smaller_size_cast<DWORD>(dwHandle), &pRIM, NULL);
		assert(SUCCEEDED(hr) && pRIM);
		INT nRegionCount = 0;
		hr = pRIM->GetDiffArrayCount(&nRegionCount);
		if (SUCCEEDED(hr))
		{
			m_nRegionDiffValueCount = nRegionCount;
			m_pdwRegionDiffValue = new DWORD[nRegionCount];
			hr = pRIM->GetDiffArray(m_pdwRegionDiffValue, nRegionCount);
			*pnValue = m_pdwRegionDiffValue[uIndex];
		}
		else
		{
			*pnValue = -1;
		}
	}
	else if (uIndex >= m_nRegionDiffValueCount)
	{
		delete [] m_pdwRegionDiffValue;
		m_pdwRegionDiffValue = NULL;
		m_nRegionDiffValueCount = INT_MAX;
		*pnValue = -1;
	}
	else
	{
		*pnValue = m_pdwRegionDiffValue[uIndex];
	}
	return S_OK;
}

STDMETHODIMP CSceneEditor::SaveRegionMap(BSTR filePath, ULONG dwHandle)
{
	IEKG3DRegionInfoManagerTable* pTable = 
		KSH::GetEngineCombinativeInterface<KG3DTYPE_RESOURCE_TABLE_TYPE, KG3DTYPE_REGION_INFO_MANAGER>();
	IEKG3DRegionInfoManager* pRIM = NULL;
	HRESULT hr = pTable->GetRIM(_kg_to_smaller_size_cast<DWORD>(dwHandle), &pRIM, NULL);
	pRIM->Save(BSTR_TO_STRING(filePath), 0);
	return S_OK;
}
