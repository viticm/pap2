#include "StdAfx.h"
#include "kg3dscenemodeleditor.h"
#include "KG3DGraphicsTool.h"
#include "KG3DModel.h"
#include "KG3DModelST.h"
#include "kg3dmodeltable.h"
#include "kg3dmeshtable.h"
#include "KG3DInstanceBillboard.h"
#include "KG3DInstanceHighDefGrass.h"
#include "KG3DSceneModelEditorPlayerTools.h"
#include "KG3DModelSpeedTree.h"
#include "PhysiscSystem/KG3DPhysiscScene.h"
#include "PhysiscSystem/KG3DPhysiscManager.h"
#include "KG3DEngineManager.h"
#include "..\ViMD3DLib\include\ViMD3DLib.h"				//ViM
#include "KG3DSFX.h"
#include "KG3DSceneCameraMovement.h"
#include "KG3DTextureTable.h"
#include "KG3DAnimationTag.h"
#include "KG3DAnimationTagContainer.h"
#include "..\..\..\Include\IKG3DSoundShell.h"
#include "KG3DBillboardCloudCreater.h"
#include "KG3DEnvironment.h"
#include "KG3DResourcePacker.h"
#include "recplayer.h"
#include "KG3DCaption.h"
#include "KG3DLocalMoveCoord.h"
#include "KG3DLocalRotationCoord.h"
#include "KG3DLocalScalCoord.h"
#include "kg3dSkinModelRender.h"
#include "kg3dstaticmodelrender.h"

#include "KG3DSceneFunctions.h"//KG3DSelectionBBoxGetter

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern DWORD g_dwNumViews;
extern BOOL g_bUseVIM3D;
float g_fBoneWeight = 0.0f;
BOOL g_bOnlyShowPitchOn = FALSE;
extern BOOL g_bDesign;

float g_fModelRenderTwiceCullDistance;
LPCTSTR KG3DSceneModelEditor::s_strIsoSelGroup = _T("IsoSelGroup");
LPCSTR KG3DSceneModelEditor::s_strSeletableBack = _T("SeletableBack");
LPCTSTR KG3DSceneModelEditor::s_strSelBackUp = _T("SelBack");
static BOOL s_bUseLOD = FALSE;
extern vector<D3DXVECTOR3>g_vecModelEditorPoints;

KG3DSceneModelEditor::KG3DSceneModelEditor(void)
:m_lpBillboardCloudCreater2(NULL)
{
	m_SpeedTreeModifier.SetModelEditor(this);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MINFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, (D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
	g_pd3dDevice->SetSamplerState(0, D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);

	m_lpPlayerTools = NULL;
	m_lpPlayerTools = new KG3DSceneModelEditorPlayerTools;
	m_bIsolated = FALSE;
	m_bEnableSelection = TRUE;

	m_lpBillboardCloudCreater = NULL;
	m_lpBillboardCloudCreater2 = NULL;
	m_pFloorTexture   = NULL;
	m_pTagContainer = NULL;
	m_bDrawGroundGrid = TRUE;
}

KG3DSceneModelEditor::~KG3DSceneModelEditor(void)
{
	SAFE_RELEASE(m_pTagContainer);
	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i] && m_Models[i]->IsShowBones())
			m_Models[i]->SetBoneMeshAttachScene(NULL);
		SAFE_RELEASE(m_Models[i]);
	}

	vector<KG3DSFX*>::iterator itor = m_VecpSFXMeteor.begin();
	while (itor != m_VecpSFXMeteor.end())
	{
		(*itor)->Release();
		itor++;
	}
	m_VecpSFXMeteor.clear();

	SAFE_DELETE(m_lpPlayerTools);
	SAFE_DELETE(m_lpBillboardCloudCreater);
	SAFE_DELETE(m_lpBillboardCloudCreater2);
	SAFE_RELEASE(m_pFloorTexture);
}

void KG3DSceneModelEditor::ClearModel()
{
	KG3DSceneEntityList::iterator it = m_listRenderEntity.begin();
	while (it != m_listRenderEntity.end())
	{
		KG3DSceneEntity& CurEntity = *it;
		if (CurEntity.GetType() == SCENEENTITY_MODEL)
		{
			it = m_listRenderEntity.erase(it);
			continue;
		}
		it++;
	}

	/*it = m_listSelectedEntity.begin();
	while (it != m_listSelectedEntity.end())
	{
		KG3DSceneEntity& CurEntity = *it;
		if (CurEntity.GetType() == SCENEENTITY_MODEL)
		{
			it = m_listSelectedEntity.erase(it);
			continue;
		}
		it++;
	}*/

	KG3DSceneEntityList delModelList;
	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for(proxy.Begin(); !proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity& Entity = proxy.GetCur();
		if (Entity.GetType() == SCENEENTITY_MODEL)
		{
			delModelList.push_back(Entity);
		}
	}

	KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
	for (KG3DSceneEntityList::iterator it = delModelList.begin(); it != delModelList.end(); ++it)
	{
		KG3DSceneEntity& Entity = *it;
		selTool.SetSelected(Entity, FALSE);
	}

	/*it = m_listSelectableEntity.begin();
	while (it != m_listSelectableEntity.end())
	{
		KG3DSceneEntity& CurEntity = *it;
		if (CurEntity.GetType() == SCENEENTITY_MODEL)
		{
			it = m_listSelectableEntity.erase(it);
			continue;
		}
		it++;
	}*/

	struct SelectableVisitor : public KG3DSceneEntityVisitor
	{
		KG3DSceneEntityList m_list;
		virtual HRESULT Accept(KG3DSceneEntity& entity)
		{
			if(entity.GetType() == SCENEENTITY_MODEL)
				m_list.push_back(entity);

			return S_OK;
		}
	};
	SelectableVisitor selectableVisitor;
	selTool.TraverseSelectable(selectableVisitor);

	for (KG3DSceneEntityList::iterator it = selectableVisitor.m_list.begin(); it != selectableVisitor.m_list.end(); ++it)
	{
		KG3DSceneEntity& entity = *it;
		//selTool.SetSelectable(entity, FALSE);//<Selectable Refactor>
		this->InsertEntitySelectable(entity, FALSE);
	}
}

HRESULT KG3DSceneModelEditor::RenderForEachWindow(KG3DSceneOutputWnd& wndCur)
{
	KG3DSceneOutputWnd* pWnd = &wndCur;
	HRESULT hr = E_FAIL;
	switch(pWnd->GetType())
	{
	case OUTPUTWND_MODELEDITOR_MAIN:
		{
			if(g_bUseVIM3D)
			{
				if(!pWnd->m_bUseVIM3D)
					pWnd->InitV3D();

				pWnd->GetCamera().SetCamera();

				LPDIRECT3DSURFACE9 pOrigRenderTarget = 0;									// ViM
				if (FAILED( g_pd3dDevice->GetRenderTarget(0, &pOrigRenderTarget)))			// ViM
					return E_FAIL;																	// ViM


				float K = (float)(sinf(timeGetTime()*0.001F)*0.5+0.5);
				ViMD3Dutil_GetCamFocalTranslation(pWnd->m_fCamFixpoint,
					pWnd->m_fCamTranslation,
					pWnd->GetCamera().GetPerspectiveData().fFovy,
					pWnd->GetCamera().GetPerspectiveData().fAspect,
					pWnd->GetCamera().GetPerspectiveData().fzNear,
					pWnd->GetCamera().GetPerspectiveData().fzFar,
					K*1000+1000,
					1280,
					g_dwNumViews);

				for (DWORD i=0;i<g_dwNumViews;i++)
				{

					if (FAILED(g_pd3dDevice->SetRenderTarget(0, pWnd->m_pRenderTargets[i])))	// ViM
                    {
                        pOrigRenderTarget->Release();
						return E_FAIL;				
                    }// ViM

					//////////////////////////////////////////////////////////////////////////
					D3DXMATRIXA16 matProj = pWnd->GetCamera().GetProjectionMatrix();

					ViMD3Dutil_MatrixPerspectiveFovLH(&matProj,
						pWnd->GetCamera().GetPerspectiveData().fFovy,
						pWnd->GetCamera().GetPerspectiveData().fAspect,
						pWnd->GetCamera().GetPerspectiveData().fzNear,
						pWnd->GetCamera().GetPerspectiveData().fzFar,		// ViM
						pWnd->m_fCamTranslation,
						pWnd->m_fCamFixpoint,
						i,
						g_dwNumViews);							    // ViM
					//matProj._41 += i*10.1F;
					/*ViMD3Dutil_MatrixModify(&matProj,
					pWnd->m_fCamTranslation,
					pWnd->m_fCamFixpoint,
					i,
					g_dwNumViews);*/

					g_pd3dDevice->SetTransform( D3DTS_PROJECTION, &matProj );

					hr = RenderMainWindow(wndCur);

				}

				if (FAILED( g_pd3dDevice->SetRenderTarget(0, pOrigRenderTarget)))	
                {                                                                               // ViM
                    pOrigRenderTarget->Release();
                    return E_FAIL;																// ViM
                }
                pOrigRenderTarget->Release();											// ViM
			}
			else
			{
				hr = RenderMainWindow(wndCur);
			}
		}
		break;

	case OUTPUTWND_MODELEDITOR_PREVIEW:
		{
			hr = RenderPreviewWindow(pWnd);
		}
		break;

	case OUTPUTWND_MODELEDITOR_PREVIEW_NPC_PORTRAIT:
		{
			hr = RenderPreviewWindowNpcPortrait(pWnd);
		}
		break;

	default:
		break;
	}
	return hr;
}

//以下2个用于策划工具的预览渲染
HRESULT KG3DSceneModelEditor::RenderPreviewWindowNpcPortrait(KG3DSceneOutputWnd* pWnd)
{
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	RenderWindowBegin(pWnd,TRUE);

	g_cGraphicsTool.ApplyLight(*this);

	for_each(m_listRenderEntity.begin(), m_listRenderEntity.end(), mem_fun_ref(&KG3DSceneEntity::Render));

	RenderWindowEnd(pWnd);
	return S_OK;
}

HRESULT KG3DSceneModelEditor::RenderPreviewWindow(KG3DSceneOutputWnd* pWnd)
{
	g_pd3dDevice->SetRenderState(D3DRS_FOGENABLE, FALSE);
	RenderWindowBegin(pWnd,TRUE);

	if (GetEditing()) // 模型是否旋转
	{
		float CurYaw = g_cGraphicsTool.GetNowTime() * D3DX_PI * 0.00035f;
		D3DXQUATERNION qtn;
		D3DXQuaternionRotationYawPitchRoll(&qtn, CurYaw, 0, 0);
		if (m_Models.size() > 0)
			m_Models[0]->SetRotation(&qtn);
	}

	g_cGraphicsTool.DrawGroundGrid(100, 0xff464646);
	g_cGraphicsTool.ApplyLight(*this);

	for_each(m_listRenderEntity.begin(), m_listRenderEntity.end(), mem_fun_ref(&KG3DSceneEntity::Render));
	
	if (m_Models.size() > 1)
	{
		KG3DModel* pModelSelector = m_Models[1];
		if (pModelSelector->GetType() == MESHTYPE_SFX)
		{
			KG3DSFX* pSFX = dynamic_cast<KG3DSFX*>(pModelSelector);
			pSFX->RenderFootprint();
		}
	}

	g_cCaption2D.Render();

	RenderWindowEnd(pWnd);

	return S_OK;
}

HRESULT KG3DSceneModelEditor::RenderMainWindow(KG3DSceneOutputWnd& wndCur)
{
	KG3DSceneOutputWnd* pWnd = &wndCur;
	g_cGraphicsTool.SetCurrentConverMap(&KG3DTerrainConverMap::GetUniqueDefault());

    DWORD dwCurrTime = timeGetTime();

	GraphicsStruct::RenderStateAlpha Alpha(0x20,TRUE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
	g_pd3dDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	RenderWindowBegin(pWnd,TRUE);

	if (m_bDrawGroundGrid)
	{
		g_cGraphicsTool.DrawGroundGrid(100, 0xff464646);
	}

	/*float fPercentage = abs(sin(timeGetTime() / 1000.0f));
	if (m_Models.size() > 0)
		m_Models[0]->SetPercentage(fPercentage);*/

	g_cGraphicsTool.ApplyLight(*this);

	_ASSERTE(NULL != pWnd);
	g_SetShaderSharedParams(*this, wndCur, NULL, &KG3DTerrainConverMap::GetUniqueDefault());

	//////////////////////////////////////////////////////////////////////////
	// 策划编辑器相关 plz give me a break
	if (g_bDesign)
	{
		if (GetEditing()) // 模型旋转
		{
			float CurYaw = g_cGraphicsTool.GetNowTime() * D3DX_PI * 0.00035f;
			D3DXQUATERNION qtn;
			D3DXQuaternionRotationYawPitchRoll(&qtn, CurYaw, 0, 0);
			if (m_Models.size() > 0)
				m_Models[0]->SetRotation(&qtn);
		}
		if (m_Models.size() > 1) // 特殊处理脚底框
		{
			KG3DModel* pModelSelector = m_Models[1];
			if (pModelSelector->GetType() == MESHTYPE_SFX)
			{
				KG3DSFX* pSFX = dynamic_cast<KG3DSFX*>(pModelSelector);
				pSFX->RenderFootprint();
			}
		}
	}


	g_cCaption2D.Render();

//<BillboardCloudV1>
	KG3DModel* pModel = NULL;
	if (m_Models.size() > 0)
		pModel = m_Models[0];
	if (pModel && pModel->HasBillBoardCloud())
	{
		D3DXMATRIX matOffset;
		D3DXMatrixIdentity(&matOffset);
		matOffset._41 -= (pModel->m_BBox.GetLength() + 100.f);
		pModel->RenderBillboardCloud_Begin();
		pModel->RenderBillboardCloud(&matOffset);
		pModel->RenderBillboardCloud_End();

		if (pModel->GetType() == MESHTYPE_SPEEDTREE) 
		{
			KG3DModelSpeedTree* pTree = static_cast<KG3DModelSpeedTree*>(pModel);
			if (pTree->m_lpModelTree && pTree->m_lpModelTree->HasBillBoardCloud())
			{
				pTree->m_lpModelTree->RenderBillboardCloud_Begin();
				pTree->m_lpModelTree->RenderBillboardCloud(&matOffset);
				pTree->m_lpModelTree->RenderBillboardCloud_End();
			}
		}
	}

    int nEnableCPUSkin = g_cEngineOption.bCpuSkin;
    if (nEnableCPUSkin)
    {
        for (size_t i = 0; i < m_Models.size(); ++i)
        {
            KG3DModel* pModel = m_Models[i];
            ASSERT(pModel);
            if (pModel->IsShowBones())
            {
                nEnableCPUSkin = false;
                break;
            }
        }  
    }
	
    if(nEnableCPUSkin)
	{
		m_EntityMgr.SetVisibleEntity(&m_Models);
		
		m_EntityMgr.m_lpSKinModelRender->FrameMove(m_EntityMgr.m_lpPointLightRender);
		m_EntityMgr.m_lpStaticModelRender->FrameMove(m_EntityMgr.m_lpPointLightRender);
        {
            GraphicsStruct::RenderStateAlpha Alpha(0xFE,TRUE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
            m_EntityMgr.m_lpSKinModelRender->Render(TRUE);
            m_EntityMgr.m_lpStaticModelRender->Render();
        }
		m_EntityMgr.RenderForTranslucence();
	}
    else
    {
        for (KG3DSceneEntityList::iterator it = m_listRenderEntity.begin(); it != m_listRenderEntity.end(); ++it)
        {
            KG3DSceneEntity& Entity = *it;
            Entity.Render();
        }
    }
//<BillboardCloudV1>

    D3DXVECTOR3 vA = D3DXVECTOR3(-2000.0f, -1.0f,  2000.0f);
    D3DXVECTOR3 vB = D3DXVECTOR3( 2000.0f, -1.0f,  2000.0f);
    D3DXVECTOR3 vC = D3DXVECTOR3( 2000.0f, -1.0f, -2000.0f);
    D3DXVECTOR3 vD = D3DXVECTOR3(-2000.0f, -1.0f, -2000.0f);
    
    D3DXVECTOR2 vMoveDir = D3DXVECTOR2(0.0f, -1.0f);
    D3DXVECTOR2 vMoveLen =
        vMoveDir * -m_fFloorMoveSpeed / 4.0f * static_cast<float>(dwCurrTime - m_dwPrevTime) / 1000.0f;
    
    m_vUVLT += vMoveLen;
    m_vUVRB += vMoveLen;
    
    if (m_pFloorTexture)
    {
       // DWORD dwTexID  = m_pFloorTexture->GetID();
        g_cGraphicsTool.DrawRect(
            &vA, 
            &vB, 
            &vD,
            0xFFFFFFFF, 
            m_pFloorTexture->GetTexture(),
            m_vUVRB.x,
            m_vUVRB.y,
            m_vUVLT.x,
            m_vUVLT.y
            );
    }

	g_cGraphicsTool.DrawCircle(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,1,0),5000 ,0xFFFF0000);
	g_cGraphicsTool.DrawCircle(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,1,0),10000,0xFFFF00FF);
	g_cGraphicsTool.DrawCircle(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,1,0),20000,0xFFFFFF00);
	g_cGraphicsTool.DrawCircle(D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,1,0),40000,0xFF0000FF);


	g_cGraphicsTool.SetCurrentConverMap(NULL);

	if(m_lpPlayerTools)
		m_lpPlayerTools->Render();

	m_SpeedTreeModifier.Leaves.Render(0, NULL);

//<BillboardCloudV1>
	if(m_lpBillboardCloudCreater)
	{
		m_lpBillboardCloudCreater->Render();
	}
//</BillboardCloudV1>
	D3DXMATRIX Proj,View;
	Proj = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetProjectionMatrix();
	View = m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetViewMatrix();

	g_RenderRenderTwiceInfo();
    m_EntityMgr.RenderSpecialAlphaModel();
	RenderSelectedEntityMark(this->GetEditState(),m_SceneWndManager->GetCurOutputWnd()->m_Viewport,Proj,View,TRUE);

	//<BillboardCloud>
	if (NULL != m_lpBillboardCloudCreater2)
	{
		IEKG3DModel* pModel = NULL;
		HRESULT hr = GetCurModel(&pModel);
		if (SUCCEEDED(hr))
		{
			D3DXMATRIX TempMatrix;
			pModel->GetTransformationMatrix(&TempMatrix);
			
			D3DXVECTOR3 a,b;
			pModel->GetBBox(a,b);

			D3DXMATRIX TransMat;
			D3DXMatrixTranslation(&TransMat, abs(a.x - b.x) * 1.1f, 0, 0);
			TempMatrix *= TransMat;

			m_lpBillboardCloudCreater2->SetLastBCTransformation(TempMatrix);
		}
		m_lpBillboardCloudCreater2->Render();
	}
	//</BillboardCloud>
	if (pWnd->IsEnablePostRender())
	{
		GraphicsStruct::RenderState RSFog(D3DRS_FOGENABLE,FALSE);
		GraphicsStruct::RenderState ZWrite(D3DRS_ZWRITEENABLE, FALSE);

		pWnd->RenderPostEffect(this);
	}

	g_cGraphicsTool.SetCurrentConverMap(NULL);
    if (g_cEngineManager.GetRenderOption(RENDER_INFO))
    {
        g_cGraphicsTool.DrawAllDebugTextsAtLast();//绘制所有的Debug文字
    }
    else
    {
        g_cGraphicsTool.FlushDebugTexts();
    }

	for (size_t i=0;i<g_vecModelEditorPoints.size();i++)
	{
		D3DXVECTOR3& vPos = g_vecModelEditorPoints[i];
		g_cGraphicsTool.DrawPoint(vPos,0xFFFF0000);
	}

	RenderWindowEnd(pWnd);

	m_EntityMgr.OnRenderEnd();

    m_dwPrevTime = dwCurrTime;
	return S_OK;
}

HRESULT KG3DSceneModelEditor::AddModel(IEKG3DModel* piModel)
{

	ASSERT(0);
    HRESULT hResult = E_FAIL;
    KGLOG_PROCESS_ERROR(piModel);
    KG3DModel *pModel = dynamic_cast<KG3DModel *>(piModel);

    {
        KG3DSceneEntity Entity(SCENEENTITY_MODEL,pModel);
	    m_listRenderEntity.push_back(Entity);
    	
	    if (pModel->GetType() != MESHTYPE_SPEEDTREE)
	    {
			KG3DSceneSelectionTool& selTool = this->GetSelectionTool();

			//selTool.SetSelectable(Entity, TRUE);//<Selectable Refactor>
			this->IEInsertEntitySelectable(SCENEENTITY_MODEL, pModel, TRUE);
			selTool.SetSelected(Entity, TRUE);
	    }

	    pModel->SetEditMode(TRUE, this);
    }
    hResult = S_OK;
Exit0:
	return hResult;
}


HRESULT KG3DSceneModelEditor::OpenModel(
    LPCSTR strMeshFile,
    LPCSTR strMaterialFile,
    LPCSTR strAniamtionFile,
    BOOL bAddMode,
    IEKG3DModel** ppModel
)
{
	KG3DModel* pModel = NULL;
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	hResult = g_cModelTable.LoadResourceFromFile(strMeshFile, 0, 0, (IKG3DResourceBase **)&pModel);
	KG_COM_PROCESS_ERROR(hResult);

	//ASSERT(pModel->IsResourceReady());

	hResult = OpenModel(pModel, bAddMode);
	KG_COM_PROCESS_ERROR(hResult);

	
	pModel->SetClientAdd(TRUE);

	pModel->Release();
	if (ppModel)
	{
		*ppModel = pModel;
	}

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}
///chentianhong 2006-08-14，覆盖KG3DScene的同名方法，以达到可以选取的目的
HRESULT KG3DSceneModelEditor::AddRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable /*Default = TURE*/)
{
	if(NULL == pEntity)//不能允许里面没有东西的Entity，否则每个函数都要判断一下，很多地方会当
		return E_FAIL;
	if (!uClass)
	{
		KG3DSceneEntity Entity(SCENEENTITY_MODEL,pEntity);
		m_listRenderEntity.push_back(Entity);
		if ( TRUE == bSelectable )
		{
			KG3DSceneSelectionTool& selTool = this->GetSelectionTool();

			//selTool.SetSelectable(Entity, TRUE);//<Selectable Refactor>
			this->IEInsertEntitySelectable(SCENEENTITY_MODEL, pEntity, TRUE);
			selTool.SetSelected(Entity, TRUE);
		}		
	}
	else
	{
		KG3DSceneEntity Entity(SCENEENTITY_OBJECT, pEntity);
		m_listRenderEntity.push_back(Entity);
	}	
	return S_OK;
//Exit0:
	return E_FAIL;
}
///End chentianhong
///chentianhong 2006-08-21,为了兼容，覆盖KG3DScene的同名方法。
HRESULT KG3DSceneModelEditor::AddRenderEntity(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer)
{
	if(NULL == pvEntityPointer)//不能允许里面没有东西的Entity，否则每个函数都要判断一下，很多地方会当
		return E_FAIL;
	return KG3DSceneEditorBase::AddRenderEntity(eEntityType, pvEntityPointer);
}

HRESULT KG3DSceneModelEditor::AddRenderEntity( const KG3DSceneEntity& Entity, 
											  BOOL bIsSelectable /*= TRUE*/, 
											  BOOL bIsSelected /*= FALSE*/)
{
	return KG3DSceneEditorBase::AddRenderEntity(Entity, bIsSelectable, bIsSelected);
}
///End chentianhong
KG3DSelectionBBoxGetter* KG3DSceneModelEditor::BuildCustomBBoxGetter( KG3DSceneModelEditor& editor, KG3DSelectionBBoxGetter* pDefaultBBoxGetter )
{
	struct CustomBBoxGetter : public KG3DSelectionBBoxGetter 
	{
		KG3DSceneModelEditor* m_pModelEditor;
		KG3DSelectionBBoxGetter* m_pDefaultBBoxGetter;

		CustomBBoxGetter(KG3DSceneModelEditor& editor, KG3DSelectionBBoxGetter* pDefaultBBoxGetter)
			:m_pModelEditor(&editor)
			,m_pDefaultBBoxGetter(pDefaultBBoxGetter)
		{
			_ASSERTE(NULL != pDefaultBBoxGetter);
		}
		~CustomBBoxGetter()
		{
			SAFE_DELETE(m_pDefaultBBoxGetter);
		}
		virtual HRESULT GetBBox(KG3DSceneSelectionTool& selTool, AABBOX& boxRet)
		{
			_ASSERTE(NULL != m_pModelEditor && NULL != m_pDefaultBBoxGetter);
			if (0 == selTool.GetSelectionCount())
			{
				KG_PROCESS_ERROR(m_pModelEditor->m_Models[0]);

				D3DXVECTOR3 vec3Box[2];
				AABBOX Box;

				boxRet.Clear();

				m_pModelEditor->m_Models[0]->GetBBox(vec3Box[0], vec3Box[1]);
				boxRet.AddPosition(vec3Box, 2);

				return S_OK;
			}
			else
			{
				return m_pDefaultBBoxGetter->GetBBox(selTool, boxRet);
			}
Exit0:
			return E_FAIL;
		}
	};
	return new CustomBBoxGetter(editor, pDefaultBBoxGetter);
}


HRESULT KG3DSceneModelEditor::Init()
{
    HRESULT hr = E_FAIL;

    TCHAR strOutputInfo[MAX_PATH*2];
	strOutputInfo[0] = '\0';
	IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();
	hr = KG3DSceneEditorBase::Init();
	KG_COM_PROCESS_ERROR(hr);

	m_Models.clear();

	//Add default light here
	GetEnvironment().LoadMapSetting(NULL, NULL);

	SetAmbient(0xff999999);
    m_nShowFloor = true;

    //
    m_pFloorTexture   = NULL;
    m_fFloorMoveSpeed = 0.0f;
    m_dwPrevTime      = timeGetTime();  

    m_vUVLT.x = 0.0f;
    m_vUVLT.y = 0.0f;
    m_vUVRB.x = 10.0f;
    m_vUVRB.y = 10.0f;
    //

    ///<默认 FOG> 
	{
		KG3DFogParam fogParam;
		fogParam.bEnable  = TRUE;
		fogParam.bUseTableFog = FALSE;
		fogParam.nFogMode = D3DFOG_LINEAR;

		fogParam.vColor = D3DXCOLOR(1,1,1,1);
		//fogParam.vStartEndDensityNull = D3DXVECTOR4(1000.0f, 8000.0f, 0.5f, 0);
		fogParam.fStart = 1000.f;
		fogParam.fEnd = 8000.f;
		fogParam.fDensityDevidedBy_endMinusStart = 0.5f / (fogParam.fEnd - fogParam.fStart);

		g_cGraphicsTool.SetFogParam(*this, fogParam);
	}
	///<默认Light>
	{
		KG3DLightParam lightParam = g_cGraphicsTool.GetSunLightParam(*this);
		lightParam.sceneAmbient.r 
			= lightParam.sceneAmbient.g 
			= lightParam.sceneAmbient.b 
			= 0.5f;
		lightParam.sceneAmbient.a = 1;
		lightParam.sky_light.r = 0.0f;
		lightParam.sky_light.g = 0.0f;
		lightParam.sky_light.b = 0.0f;
		lightParam.sky_light.a = 1.0f;
		lightParam.com_light.r = 0.0f;
		lightParam.com_light.g = 0.0f;
		lightParam.com_light.b = 0.0f;
		lightParam.com_light.a = 1.0f;
		lightParam.diffuse.r = 0.0f;
		lightParam.diffuse.g = 0.0f;
		lightParam.diffuse.b = 0.0f;
		lightParam.diffuse.a = 1.0f;
		lightParam.specular.r = 0.0f;
		lightParam.specular.g = 0.0f;
		lightParam.specular.b = 0.0f;
		lightParam.specular.a = 1.0f;
		g_cGraphicsTool.SetSunLightParam(*this, lightParam);
		g_cGraphicsTool.SetPlayerLightParam(*this, lightParam);
		
	}
	
	if (pSoundShell)
		pSoundShell->SetListenerPosition(&D3DXVECTOR3(0.0f, 0.0f, 0.0f), &D3DXVECTOR3(0.f, 0.f, .0f));

	hr = this->GetSelectionTool().AllocSelGroup(s_strIsoSelGroup);
	_ASSERTE(SUCCEEDED(hr));
	hr = this->GetSelectionTool().AllocSelGroup(s_strSeletableBack);
	_ASSERTE(SUCCEEDED(hr));
	hr = this->GetSelectionTool().AllocSelGroup(s_strSelBackUp);
	_ASSERTE(SUCCEEDED(hr));

	//取选择Box的方式不一样，要替换掉
	{
		KG3DSelectionBBoxGetter* pDefBBoxGetter = &KG3DSceneEditorBase::GetSelectionBBoxGetter();
		KG3DSelectionBBoxGetter* p = BuildCustomBBoxGetter(*this, pDefBBoxGetter);
		if (NULL != p)
		{
			KG3DSelectionBBoxGetter* pOldGetter = NULL;
			HRESULT hrTemp = KG3DSceneEditorBase::SetSelectionBBoxGetter(*p, &pOldGetter);
			_ASSERTE(SUCCEEDED(hrTemp));
			KG_USE_ARGUMENT(hrTemp);
			_ASSERTE(pDefBBoxGetter == pOldGetter);	//保证了生命周期不会搅乱，现在由CustomBBoxGetter接管了生命周期
		}
	}

	if (pSoundShell)
	{
		pSoundShell->Unlock();
	}

	hr = S_OK;
Exit0:
    return hr;
}
HRESULT KG3DSceneModelEditor::UnInit()
{
	SAFE_RELEASE(m_pTagContainer);
	size_t uSize = m_Models.size();
	for (size_t i = 0; i < uSize; i++)
	{
		RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_MODEL, m_Models[i]));
		SAFE_RELEASE(m_Models[i]);
	}
	for (size_t i = 0; i < m_vecModelsForSockets.size(); i++)
	{
		SAFE_RELEASE(m_vecModelsForSockets[i]);
	}

	for (size_t i = 0; i < m_vecModelLoaded.size(); i++)
	{
		SAFE_RELEASE(m_vecModelLoaded[i]);
	}
	SAFE_RELEASE(m_pFloorTexture);

	//m_listSelectableEntityBack.clear();
	this->GetSelectionTool().ClearSelGroup(s_strSeletableBack);

	//m_listSelectedEntityBack.clear();
	this->GetSelectionTool().ClearSelGroup(s_strSelBackUp);

	return KG3DSceneEditorBase::UnInit();
}

/*
HRESULT KG3DSceneModelEditor::GetCurMesh(KG3DModel** ppModel)
{
	HRESULT hr = E_FAIL;
	return hr;
}*/


void KG3DSceneModelEditor::MsgOnKeyDown(WPARAM wParam, LPARAM lParam)
{
	/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	while(i!=m_listSelectedEntity.end())
	{
		KG3DSceneEntity Entity = *i;
		Entity.ProcessMessage(WM_KEYDOWN,wParam,lParam);
		i++;
	}*/

	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for (proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();
		Entity.ProcessMessage(WM_KEYDOWN, wParam, lParam);
	}

	KG3DSceneEditorBase::MsgOnKeyDown(wParam,lParam);
}

void KG3DSceneModelEditor::MsgOnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
	while(i!=m_listSelectedEntity.end())
	{
		KG3DSceneEntity Entity = *i;
		Entity.ProcessMessage(WM_LBUTTONUP,wParam,lParam);
		i++;
	}*/
	TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
	for (proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
	{
		KG3DSceneEntity Entity = proxy.GetCur();
		Entity.ProcessMessage(WM_KEYUP, wParam, lParam);
	}

	KG3DSceneEditorBase::MsgOnLButtonUp(wParam,lParam);
}


HRESULT KG3DSceneModelEditor::LoadModelForSockets(const TCHAR* strFileName)
{
	HRESULT hr = E_FAIL;
	KG3DModel* pModel = NULL;

	KG_PROCESS_ERROR(strFileName);

	hr = g_cModelTable.LoadResourceFromFile(strFileName, 0, MLO_MULTI|MLO_MODEL_LOD, (IKG3DResourceBase**)&pModel);
	KGLOG_COM_PROCESS_ERROR(hr);

	//pModel->PlayAnimation(ENU_ANIMATIONPLAY_CIRCLE, "data\\source\\player\\M2\\M2H奔跑01.ani", 1.0f, 0, NULL, NULL);

	m_vecModelsForSockets.push_back(pModel);
Exit0:
	return hr;
}

HRESULT KG3DSceneModelEditor::LoadModel(LPCSTR strFileName, 
										IKG3DModel **ppModel)
{
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(ppModel);

	hRetCode = g_cModelTable.LoadResourceFromFile(strFileName, 
		0, 
		MLO_MODEL_LOD,
		(IKG3DResourceBase**)ppModel);
	KG_COM_PROCESS_ERROR(hRetCode);
	
	m_vecModelLoaded.push_back(*ppModel);

	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

void KG3DSceneModelEditor::OnCommandLeafEditMode(enumCommandType eCommandType)
{
	D3DXVECTOR3 Pos(0,0,0);
	D3DXQUATERNION Rot(0,1,0,0);
		
	switch(this->GetEditState())
	{
	case SCENESTATE_MOVE:
		{
			switch(eCommandType)
			{
			case ctUp:
				Pos = D3DXVECTOR3(0,0,5);
				break;	
			case ctDown:
				Pos = D3DXVECTOR3(0,0,-5);
				break;	
			case ctLeft:
				Pos = D3DXVECTOR3(-5,0,0);
				break;	
			case ctRight:
				Pos = D3DXVECTOR3(5,0,0);
				break;
			}
			m_SpeedTreeModifier.Leaves.OnMove(Pos);
		}
		break;
	case SCENESTATE_ROTATE:
		{
			/*switch(eCommandType)
			{
			case ctUp:
				D3DXQuaternionRotationYawPitchRoll(&Rot, 0,           D3DX_PI/36,  0);
				RotateDirect(Rot, &D3DXVECTOR3(0, D3DX_PI / 36, 0));
				break;	
			case ctDown:
				D3DXQuaternionRotationYawPitchRoll(&Rot, 0,           -D3DX_PI/36, 0);
				RotateDirect(Rot, &D3DXVECTOR3(0,-D3DX_PI / 36, 0));
				break;	
			case ctLeft:
				D3DXQuaternionRotationYawPitchRoll(&Rot, D3DX_PI/36,  0,           0);
				RotateDirect(Rot, &D3DXVECTOR3(D3DX_PI / 36, 0, 0));
				break;	
			case ctRight:
				D3DXQuaternionRotationYawPitchRoll(&Rot, -D3DX_PI/36, 0,           0);
				RotateDirect(Rot, &D3DXVECTOR3(-D3DX_PI / 36, 0, 0));
				break;	
			case ctPageUp:
				D3DXQuaternionRotationYawPitchRoll(&Rot,0,0,D3DX_PI/36);
				RotateDirect(Rot, &D3DXVECTOR3(0, 0, D3DX_PI / 36));
				break;	
			case ctPageDown:
				D3DXQuaternionRotationYawPitchRoll(&Rot,0,0,-D3DX_PI/36);
				RotateDirect(Rot, &D3DXVECTOR3(0, 0, -D3DX_PI / 36));
				break;
			case ctEnd:
				D3DXQuaternionRotationYawPitchRoll(&Rot, 0,           0,           D3DX_PI/36);
				RotateDirect(Rot, &D3DXVECTOR3(0, 0, D3DX_PI / 36));
				break;
			case ctHome:
				D3DXQuaternionRotationYawPitchRoll(&Rot, 0,           0,           -D3DX_PI/36);
				RotateDirect(Rot, &D3DXVECTOR3(0, 0, -D3DX_PI / 36));
				break;
			}*/
			return KG3DSceneEditorBase::OnCommand(eCommandType);	//上面整段都是和KG3DSceneEditorBase的重复的，让基类处理就好了
		}
		break;
	case SCENESTATE_SCALE:
		{
			switch(eCommandType)
			{
			case ctUp:
				Pos = D3DXVECTOR3(1.1f, 1.1f, 1.1f);
				break;
			case ctDown:
				Pos = D3DXVECTOR3(1.0f / 1.1f, 1.0f / 1.1f, 1.0f / 1.1f);
				break;	
			}
			m_SpeedTreeModifier.Leaves.OnScale(Pos);
		}
		break;
	default:
		{
			switch(eCommandType)
			{
			case ctAddKeyFrame:
                {
                    IEKG3DSceneCameraAnimation *piCameraAnimation = NULL;
                    m_pCameraMovement->GetCurCamAnimation(&piCameraAnimation);
                    piCameraAnimation->AddKeyFrame();
                }
                break;
			}
		}
		break;
	}

}

void KG3DSceneModelEditor::OnCommand(enumCommandType eCommandType)
{
	if (m_SpeedTreeModifier.IsActive())
	{
		OnCommandLeafEditMode(eCommandType);
	}
	else
	{
		KG3DSceneEditorBase::OnCommand(eCommandType);
	}
}

void KG3DSceneModelEditor::SetEditLeafMode(BOOL bMode)
{
	m_bEditLeaves = bMode; 
}

HRESULT KG3DSceneModelEditor::SetSpeedTreeUIFunction(pFunction pCallBack, 
												  void* pDlg)
{
	m_SpeedTreeModifier.pfUpdateUI = pCallBack;
	m_SpeedTreeModifier.pEditorDlg = pDlg;
    return S_OK;
}

HRESULT KG3DSceneModelEditor::FrameMove()
{
	HRESULT hr = E_FAIL;
	D3DXVECTOR3 vLookAtPos(.0f, .0f, .0f);
	D3DXVECTOR3 vCameraPos(.0f, .0f, .0f);
	KG3DModel* pModel = NULL;
	KG3DModelSpeedTree* pSpeedTree = NULL;
	IKG3DSoundShell *pSoundShell = g_cEngineManager.GetSound();
	
	IKG3DCamera* pCamera = GetCurrentCamera();
	if (NULL != pCamera && NULL != m_lpPhysicsScene)
	{
		pCamera->GetLookAtPosition(&vLookAtPos);
		pCamera->GetPosition(&vCameraPos);
	}
	// if (!g_bDesign)
	g_dwRenderCount++;

	g_cGraphicsTool.SetCurScene(this);
    //
    for (size_t i = 0; i < m_Models.size(); i++)
    {
        KG3DModel *pModel = m_Models[i];
		if (pModel)
		{
			if (s_bUseLOD)
			{
				D3DXVECTOR3 vec3Pos(pModel->m_matWorld._41, 
					pModel->m_matWorld._42,
					pModel->m_matWorld._43);
				float fDistance = D3DXVec3Length(&(vec3Pos - vCameraPos));
				if (fDistance > g_fModelRenderTwiceCullDistance)
				{
					pModel->SetLOD(RENDERTWICE_CULL_LODLEVEL - 0.1f);
				}
				else
				{
					pModel->SetLOD(RENDERTWICE_CULL_LODLEVEL);
				}
			}
			else
			{
				pModel->SetLOD(RENDERTWICE_CULL_LODLEVEL);
			}
			pModel->SetClientAdd(TRUE);

			 if(pModel->IsShowBones()) 
				pModel->UpdateBoneMesh(this);
			 else
			 {
				 switch(pModel->GetType())
				 {
				 case MESHTYPE_MODELST:
					 reinterpret_cast<KG3DModelST*>(pModel)->UpdateBoneBBox(this);
					 break;
				 case MESHTYPE_DEFAULT:
					 pModel->UpdateBoneBBox(this);
					 break;
				 case MESHTYPE_SPEEDTREE:
					 break;
				 case MESHTYPE_SFX:
					 break;
				 default:
				ASSERT(0);
					 break;
				 }

			 }
		}
		
    }
    //
    g_KeyState[VK_UP] = 0;
    g_KeyState[VK_DOWN] = 0; 
    g_KeyState[VK_LEFT] = 0;
    g_KeyState[VK_RIGHT] = 0;
    hr = KG3DSceneEditorBase::FrameMove();
	KG_PROCESS_ERROR(m_listRenderEntity.size());
	KG_PROCESS_ERROR(m_listRenderEntity.begin()->GetType() == SCENEENTITY_MODEL);
	
	pModel = reinterpret_cast<KG3DModel*>(m_listRenderEntity.begin()->m_pPointer);
	KG_PROCESS_ERROR(pModel);

	pSpeedTree = static_cast<KG3DModelSpeedTree*>(pModel);
	KG_PROCESS_ERROR(pSpeedTree);

	//////////////////////////////////////////////////////////////////////////
	//physisc process date output
	if(m_lpPhysicsScene)
    {
        if (pCamera)
        {
            m_lpPhysicsScene->FrameMove((DWORD)g_cEngineManager.GetTimeControl().GetTinyTime(), vCameraPos, vLookAtPos);
        }
    }

	if (NULL != m_lpBillboardCloudCreater2)
		m_lpBillboardCloudCreater2->FrameMove();

	if (pSoundShell && g_cEngineManager.GetSoundFocusScene() == this)
	{
		D3DXVECTOR3 vPos, vec3Front, vec3Up(0.0f, 1.0f, 0.0f);
		GetCurrentCamera()->GetPosition(&vPos);
		GetCurrentCamera()->GetFront(&vec3Front);
		vPos /= 100.0f;
		D3DXVECTOR3 vVel = D3DXVECTOR3(0.f, 0.f, 0.f);
		pSoundShell->SetListenerPosition(&vPos, &vVel);
		pSoundShell->SetOrientation(&vec3Front, &vec3Up);
	}


	hr = S_OK;
Exit0:
	return hr;
}

KG3DSceneModelEditor::KG3DSpeedTreeModifier::KG3DSpeedTreeModifier()
{
	pfUpdateUI = NULL;
	pEditorDlg = NULL;
	pSpeedTree = NULL;
}

//代码移动到OnReceiveMessage
//HRESULT KG3DSceneModelEditor::SelectEnd(D3DVIEWPORT9 &Viewport,
//										D3DXMATRIX &matProjection,
//										D3DXMATRIX &matView)
//{
//	DWORD dwSelectionOperation = GetIncreaseMode();
//    if (dwSelectionOperation == EM_SELECTION_INCREASE_MODE_DEFAULT)
//	{
//		m_SpeedTreeModifier.Leaves.Clear(this);
//	}
//    //m_dwSelectionOperation = EM_SELECTION_INCREASE_MODE_DEFAULT;
//	SetIncreaseMode(EM_SELECTION_INCREASE_MODE_DEFAULT);
//    HRESULT hr = KG3DSceneSelectBase::SelectEnd(Viewport,
//		matProjection,
//		matView);
//
//    KG_PROCESS_ERROR(m_Models.size());
//	if (m_Models[0]->GetType() == MESHTYPE_SPEEDTREE)
//	{
//		KG3DModelSpeedTree* pSpdTree = static_cast<KG3DModelSpeedTree*>(m_Models[0]);
//
//		TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
//		for (proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
//		{
//			KG3DSceneEntity& entity = proxy.GetCur();
//			if (entity.GetType() == SCENEENTITY_SPEEDTREELEAF)
//			{
//				if (dwSelectionOperation == EM_SELECTION_INCREASE_MODE_DEFAULT || dwSelectionOperation == EM_SELECTION_INCREASE_MODE_ADD)
//				{
//					m_SpeedTreeModifier.SelectLeaf(reinterpret_cast<Leaf*>(entity.m_pPointer));
//				}
//				else
//				{
//					m_SpeedTreeModifier.UnSelectLeaf(reinterpret_cast<Leaf*>(entity.m_pPointer));
//				}
//			}
//		}
//
//		if (m_SpeedTreeModifier.pfUpdateUI)
//		{
//			m_SpeedTreeModifier.pfUpdateUI(pSpdTree, m_SpeedTreeModifier.pEditorDlg);
//		}
//	}
//
//	{
//		KG3DSceneEntity Entity = m_SpeedTreeModifier.ConvertToEntity();
//		if (m_SpeedTreeModifier.IsActive())
//		{
//			//RemoveSelectedEntity(Entity.GetType(), Entity.m_pPointer);//不用先Remove，里面会检查是否重复的
//			//AddSelectedEntity(Entity);
//
//			this->GetSelectionTool().SetSelected(Entity, TRUE);
//		}
//		else
//		{
//			//RemoveSelectedEntity(Entity.GetType(), Entity.m_pPointer);
//			this->GetSelectionTool().SetSelected(Entity, FALSE);
//		}
//	}
//Exit0:
//	return hr;
//}

HRESULT KG3DSceneModelEditor::GetCurModel(IEKG3DModel **ppiRetModel)
{
    HRESULT hrResult = E_FAIL;

    KG_PROCESS_ERROR(NULL != ppiRetModel);
    KG_PROCESS_ERROR(m_Models.size() > 0);
	
	KG_PROCESS_ERROR(NULL != m_Models[0]);
    *ppiRetModel = m_Models[0];
 
    hrResult = S_OK;
Exit0:
    return hrResult;
}


IDirect3DSurface9*  KG3DSceneModelEditor::GetBillBoardTexture(DWORD dwHeight, DWORD dwWidth)
{
    if (m_Models.size() <= 0)
        return NULL;

    KG3DCamera* pCamera = g_cGraphicsTool.GetCamera();
    KG3DModel* pModel = m_Models[0];

    static IDirect3DSurface9* pSurface = NULL;
    static DWORD sHeight = dwHeight;
    static DWORD sWidth  = dwWidth;

    if (pSurface == NULL || sHeight != dwHeight || sWidth != dwWidth)
    {
        g_pd3dDevice->CreateRenderTarget(sWidth,
                                        sHeight,
                                        D3DFMT_A8R8G8B8,
                                        D3DMULTISAMPLE_NONE,
                                        0,
                                        FALSE,
                                        &pSurface,
                                        NULL
                                        );
        if (pSurface == NULL)
            return NULL;
    }

    if (pSurface)
    {
        
        IDirect3DSurface9* pTargetSave = NULL;
        g_pd3dDevice->GetRenderTarget(0, &pTargetSave);
        if (!pTargetSave)
        {
            SAFE_RELEASE(pSurface);
            return NULL;
        }

        pTargetSave->Release();

        g_pd3dDevice->SetRenderTarget(0, pSurface);
        
        D3DXVECTOR3 vMin;
        D3DXVECTOR3 vMax;
        D3DXVECTOR3 vCet;
        D3DXVECTOR3 vPos;
        D3DXVECTOR3 vCetSave;
        D3DXVECTOR3 vPosSave;

        pModel->GetBBox(vMin, vMax);

        BBox Box(vMin, vMax);

        vCet = Box.GetCenter();
        vPos = vCet;
        vPos.z -= 1000.f;
        
        pCamera->Capture();
        pCamera->GetPosition(&vPosSave);
        pCamera->GetLookAtPosition(&vCetSave);
        
        /*pCamera->SetPosition(vPos);
        pCamera->SetUpDirection(D3DXVECTOR3(0.f, 1.f, 0.f));
        pCamera->SetLookAtPosition(vCet);*/

		pCamera->SetPositionLookAtUp(&vPos, &vCet, &D3DXVECTOR3(0,1,0));
		pCamera->SetOrthogonal(Box.GetWidth(), vCet.y * 2.f, 1.f, 10000.f);

        g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00000000, 1.f, 0);
        pModel->Render(0, NULL);

        /*pCamera->SetPosition(vPosSave);
        pCamera->SetLookAtPosition(vCetSave);*/
		pCamera->SetPositionLookAtUp(&vPosSave, &vCetSave, NULL);

        pCamera->RestoreCapture();
        g_pd3dDevice->SetRenderTarget(0, pTargetSave);

        return pSurface;

    }

	return NULL;
}

HRESULT KG3DSceneModelEditor::OpenModel(IEKG3DModel* piModel, BOOL bAddMode)
{
	HRESULT hr = E_FAIL;
    KG3DModel *pModel = NULL;
	vector<IKG3DModel*> vecpModel;
    KG_PROCESS_ERROR(piModel);

    pModel = dynamic_cast<KG3DModel *>(piModel);
    KG_PROCESS_ERROR(pModel);

	if (!bAddMode)
	{
		KG3DSceneSelectionTool& selTool = this->GetSelectionTool();
		for (size_t i = 0; i < m_Models.size(); i++)
		{
			if (m_Models[i])
			{
				m_Models[i]->SetEditMode(FALSE, this);
				RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_MODEL, m_Models[i]));
				selTool.ClearSelection();
				//selTool.ClearSelectable();
			}
            SAFE_RELEASE(m_Models[i]);
		}
		m_Models.clear();
		
	}
	
	m_Models.push_back(pModel);
	pModel->AddRef();
	{
		RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_MODEL, pModel));
		AddRenderEntity(KG3DSceneEntity(SCENEENTITY_MODEL, pModel), FALSE, FALSE);
	}
	pModel->SetEditMode(TRUE, this);

	/*if (!g_bDesign)
	{
		pModel->SetCaption(3, "测试内容");
		pModel->SetCaptionColor(3, D3DXCOLOR(.0f, 1.f, .0f, 1.f));
		pModel->SetCaption(2, "<势力范围>");
		pModel->SetCaptionColor(2, D3DXCOLOR(.0f, 1.f, .0f, 1.f));
		pModel->SetPercentageStyle(D3DXCOLOR(1.0f, .0f, .0f, 1.0f));
		pModel->SetCaptionFloorHeight(250.0f);
	}*/

	hr = S_OK;
Exit0:
	return hr;
}


KG3DSceneModelEditor::KG3DSpeedTreeModifier* KG3DSceneModelEditor::GetSpeedTreeModifier()
{
	return &m_SpeedTreeModifier;
}


BOOL KG3DSceneModelEditor::KG3DSpeedTreeModifier::SelectLeaf(IELeaf* piLeaf)
{
	BOOL bReturn = FALSE;
	KG_PROCESS_ERROR(piLeaf);

    Leaf *pLeaf = NULL;
    pLeaf = static_cast<Leaf *>(piLeaf);

	//pModelEditor->RemoveSelectedEntity(SCENEENTITY_SPEEDTREELEAF, pLeaf);
	pModelEditor->RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, pLeaf));

	bReturn = Leaves.AddLeaf(pLeaf);
Exit0:
	return bReturn;
}

HRESULT KG3DSceneModelEditor::RemoveModel(IEKG3DModel* pModel)
{
    HRESULT hrResult = E_FAIL;
	KG_PROCESS_SUCCESS(!pModel);
	
	//RemoveSelectedEntity(SCENEENTITY_MODEL, pModel);
	//RemoveSelectableEntity(KG3DSceneEntity(SCENEENTITY_MODEL, pModel));
	RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_MODEL, pModel));

	for (size_t i = 0; i < m_Models.size(); i++)
	{
		if (m_Models[i] == pModel)
		{
			SAFE_RELEASE(pModel);
			std::vector<KG3DModel*>::iterator it = m_Models.begin();
			advance(it, i);
			m_Models.erase(it);
			break;
		}
	}
Exit1:
    hrResult = S_OK;
	return hrResult;
}

BOOL KG3DSceneModelEditor::KG3DSpeedTreeModifier::UnSelectLeaf(IELeaf* piLeaf)
{

	KG_PROCESS_ERROR(piLeaf);
	
    Leaf *pLeaf = NULL;
    pLeaf = static_cast<Leaf *>(piLeaf);
	pModelEditor->RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, pLeaf));
	pModelEditor->AddRenderEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, pLeaf), TRUE, FALSE);
    //pModelEditor->RemoveSelectedEntity(SCENEENTITY_SPEEDTREELEAF, pLeaf);
	Leaves.RemoveLeaf(pLeaf);
Exit0:
	return TRUE;
}

void KG3DSceneModelEditor::KG3DSpeedTreeModifier::DeleteLeaf(IELeaf* piLeaf)
{
	KG3DModelSpeedTree* pTree = NULL;
	KG_PROCESS_ERROR(pModelEditor);
	KG_PROCESS_ERROR(pModelEditor->m_Models[0]);
	KG_PROCESS_ERROR(pModelEditor->m_Models[0]->GetType() == MESHTYPE_SPEEDTREE);
    
    Leaf *pLeaf = NULL;
    pLeaf = static_cast<Leaf *>(piLeaf);

	pTree = static_cast<KG3DModelSpeedTree*>(pModelEditor->m_Models[0]);
	
	Leaves.RemoveLeaf(pLeaf);

   // pModelEditor->RemoveSelectedEntity(SCENEENTITY_SPEEDTREELEAF, (void*)pLeaf);
	//pModelEditor->RemoveSelectableEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, pLeaf));
	pModelEditor->RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, pLeaf));
	
	pTree->DeleteLeaf(pLeaf);
Exit0:
	return;
}

void KG3DSceneModelEditor::KG3DSpeedTreeModifier::AddLeaf(int nBoneIndex)
{
	KG3DModelSpeedTree* pTree = NULL;
	Leaf *pNewLeaf = NULL;

	KG_PROCESS_ERROR(pModelEditor);
	KG_PROCESS_ERROR(pModelEditor->m_Models[0]);
	KG_PROCESS_ERROR(pModelEditor->m_Models[0]->GetType() == MESHTYPE_SPEEDTREE);

	{
		pTree = static_cast<KG3DModelSpeedTree*>(pModelEditor->m_Models[0]);
		pNewLeaf = pTree->Add1Leaf(nBoneIndex);
		KG_PROCESS_ERROR(pNewLeaf);

		Leaves.AddLeaf(pNewLeaf);

		//KG3DSceneSelectionTool& selTool = pModelEditor->GetSelectionTool();
		//KG3DSceneEntity Entity(SCENEENTITY_SPEEDTREELEAF, pNewLeaf);//<Selectable Refactor>
		//selTool.SetSelectable(Entity, TRUE);//内部会检查重复性，所以不需要先Remove
		pModelEditor->IEInsertEntitySelectable(SCENEENTITY_SPEEDTREELEAF, pNewLeaf, TRUE);

		if (pEditorDlg)
		{
			pfUpdateUI(pTree, pEditorDlg);
		}
	}
Exit0:
	return;
}


void KG3DSceneModelEditor::KG3DSpeedTreeModifier::DeleteSelectedLeaf()
{
	KG3DModelSpeedTree* pTree = NULL;
	Leaf* pLeaf = Leaves.GetFirstLeaf();
	while (pLeaf)
	{
		DeleteLeaf(pLeaf);
		pLeaf = Leaves.GetFirstLeaf();
	}

	KG_PROCESS_ERROR(pModelEditor);
	KG_PROCESS_ERROR(pModelEditor->m_Models[0]);
	KG_PROCESS_ERROR(pModelEditor->m_Models[0]->GetType() == MESHTYPE_SPEEDTREE);
	pTree = static_cast<KG3DModelSpeedTree*>(pModelEditor->m_Models[0]);
	if (pEditorDlg)
	{
		pfUpdateUI(pTree, pEditorDlg);
	}
Exit0:
	return;
}

BOOL KG3DSceneModelEditor::KG3DSpeedTreeModifier::IsActive()
{
	if (Leaves.GetFirstLeaf())
		return TRUE;
	return FALSE;
}

KG3DSceneEntity KG3DSceneModelEditor::KG3DSpeedTreeModifier::ConvertToEntity()
{
	return KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAFGROUP, (void*)&Leaves);
}

DWORD KG3DSceneModelEditor::KG3DSpeedTreeModifier::GetScaleMode()
{
	return Leaves.GetScaleMode();
}

void KG3DSceneModelEditor::KG3DSpeedTreeModifier::SetScaleMode(DWORD dwScaleMode)
{
	Leaves.SetScaleMode(dwScaleMode);
}

//HRESULT KG3DSceneModelEditor::OnToggleStateScaling()//<OnToggleStateScaling Refactor>//额外的代码移动到HandleEngineMessages，SceneSelectBase会监视SetEditState，在内部改变状态
//{
//	//this->GetCoordScale().AttachEntity(this, m_listSelectedEntity);
//
//	DWORD dwCurScaleMode = m_SpeedTreeModifier.GetScaleMode();
//	m_SpeedTreeModifier.SetScaleMode((dwCurScaleMode + 1) % KG3DSpeedTreeModifier::ENU_SCALE_COUNT);
//	m_bScaling = FALSE;
//	return S_OK; 
//}

void KG3DSceneModelEditor::KG3DSpeedTreeModifier::UnSelectAll()
{
	KG_PROCESS_ERROR(pModelEditor);
	Leaves.Clear(pModelEditor);	
Exit0:
	return;
}

//HRESULT KG3DSceneModelEditor::GetSelectedEntityCenter(D3DXVECTOR3* pPos)
//{
//	HRESULT hr = E_FAIL;
//	if (!m_listSelectedEntity.size())
//	{
//		float HeightMin = 1000000;
//		AABBOX Box;
//		D3DXVECTOR3 vec3Box[2];
//
//		KG_PROCESS_ERROR(m_Models.size());
//		KG_PROCESS_ERROR(m_Models[0]);
//
//		m_SelectionCenter = D3DXVECTOR3(0,0,0);
//		m_Models[0]->GetBBox(vec3Box[0], vec3Box[1]);
//		Box.AddPosition(vec3Box, 2);
//		m_SelectionCenter += Box.GetCenter();
//		HeightMin = min(HeightMin,Box.A.y);
//		m_SelectionCenter.y = HeightMin;
//		
//		if(pPos)
//		{
//			(*pPos) = m_SelectionCenter;
//		}
//	}
//	else
//	{
//		KG3DSceneSelectBase::GetSelectedEntityCenter(pPos);
//	}
//	hr = S_OK;
//Exit0:
//	return hr;
//}


//HRESULT KG3DSceneModelEditor::GetSelectedEntityBBox(AABBOX& SelectedEntityBox)//代码移动到GetSelectionBBoxGetter
//{
//	if (0 == this->GetSelectionTool().GetSelectionCount())
//	{
//		D3DXVECTOR3 vec3Box[2];
//		AABBOX Box;
//		SelectedEntityBox.Clear();
//		KG_PROCESS_ERROR(m_Models[0]);
//		
//		m_Models[0]->GetBBox(vec3Box[0], vec3Box[1]);
//		SelectedEntityBox.AddPosition(vec3Box, 2);
//	}
//	else
//	{
//		KG3DSceneSelectBase::GetSelectedEntityBBox(SelectedEntityBox);
//	}
//Exit0:
//	return S_OK;
//}

HRESULT KG3DSceneModelEditor::SetFloorSlipSpeed(float fSpeed)
{
    m_fFloorMoveSpeed = fSpeed;
    return S_OK;
}

HRESULT KG3DSceneModelEditor::SetFloorTexture(LPCTSTR szFilePath)
{
    SAFE_RELEASE(m_pFloorTexture);
    HRESULT hr = g_cTextureTable.LoadResourceFromFile(
        szFilePath, 0, TEXOP_COMMON, (IKG3DResourceBase**)&m_pFloorTexture
    );
    return hr;
}
void KG3DSceneModelEditor::IsolateCurrentSelection()
{
	m_bIsolated = !m_bIsolated;
	if (m_bIsolated)
	{
		this->GetSelectionTool().ClearSelectable();

        m_SpeedTreeModifier.SetAllLeavesSelectable(*this);
		m_SpeedTreeModifier.UnSelectAll();
		
		//m_lstIsolated = m_listSelectableEntity;
		this->GetSelectionTool().CopySelectableToSelGroup(s_strIsoSelGroup);
	}
	else
	{
		m_SpeedTreeModifier.SetAllLeafSelectableFlag(TRUE);
		//m_lstIsolated.clear();

		this->GetSelectionTool().ClearSelGroup(s_strIsoSelGroup);
	}
}

BOOL KG3DSceneModelEditor::GetIsolateMode()
{
	return m_bIsolated;
}

void KG3DSceneModelEditor::KG3DSpeedTreeModifier::SetAllLeafRenderFlag(BOOL bFlag)
{
	KG_PROCESS_ERROR(pModelEditor);
	KG_PROCESS_ERROR(pSpeedTree);
	if (bFlag)
	{
		for (size_t i = 0; i < pSpeedTree->m_lpLeafsData->m_ListLeaf.size(); i++)
		{
			Leaf *pLeaf = pSpeedTree->m_lpLeafsData->m_ListLeaf[i];
			pModelEditor->RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, (void*)pLeaf));
			pModelEditor->AddRenderEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, (void*)pLeaf));
		}
	}
	else
	{
		for (size_t i = 0; i < pSpeedTree->m_lpLeafsData->m_ListLeaf.size(); i++)
		{
			Leaf *pLeaf = pSpeedTree->m_lpLeafsData->m_ListLeaf[i];
			pModelEditor->RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, (void*)pLeaf));
		}
	}
Exit0:
	return;
}

void KG3DSceneModelEditor::KG3DSpeedTreeModifier::SetAllLeafSelectableFlag(BOOL bFlag)
{
	//KG3DSceneSelectionTool& selTool = pModelEditor->GetSelectionTool();
	KG_PROCESS_ERROR(pModelEditor);
	KG_PROCESS_ERROR(pSpeedTree);
	if (bFlag)
	{
		for (size_t i = 0; i < pSpeedTree->m_lpLeafsData->m_ListLeaf.size(); i++)
		{
			Leaf *pLeaf = pSpeedTree->m_lpLeafsData->m_ListLeaf[i];
			
			//KG3DSceneEntity Entity(SCENEENTITY_SPEEDTREELEAF, pLeaf);//<Selectable Refactor>
			//selTool.SetSelectable(Entity, TRUE);//不用先Remove，内部会做重复性检查的
			pModelEditor->IEInsertEntitySelectable(SCENEENTITY_SPEEDTREELEAF, pLeaf, TRUE);
		}
	}
	else
	{
		for (size_t i = 0; i < pSpeedTree->m_lpLeafsData->m_ListLeaf.size(); i++)
		{
			Leaf *pLeaf = pSpeedTree->m_lpLeafsData->m_ListLeaf[i];
			
			/*KG3DSceneEntity Entity(SCENEENTITY_SPEEDTREELEAF, pLeaf);
			selTool.SetSelectable(Entity, FALSE);*///<Selectable Refactor>
			pModelEditor->IEInsertEntitySelectable(SCENEENTITY_SPEEDTREELEAF, pLeaf, FALSE);
		}
	}
Exit0:
	return;
}

void KG3DSceneModelEditor::KG3DSpeedTreeModifier::SetAllLeafSelectedFlag(BOOL bFlag)
{
	KG_PROCESS_ERROR(pModelEditor);
	KG_PROCESS_ERROR(pSpeedTree);
	if (bFlag)
	{
		for (size_t i = 0; i < pSpeedTree->m_lpLeafsData->m_ListLeaf.size(); i++)
		{
			Leaf *pLeaf = pSpeedTree->m_lpLeafsData->m_ListLeaf[i];
			
			//pModelEditor->RemoveSelectedEntity(SCENEENTITY_SPEEDTREELEAF, (void*)pLeaf);//不用先Remove，里面会检查重复
			//pModelEditor->AddSelectedEntity(SCENEENTITY_SPEEDTREELEAF, (void*)pLeaf);

			KG3DSceneEntity entity(SCENEENTITY_SPEEDTREELEAF, (void*)pLeaf);
			pModelEditor->GetSelectionTool().SetSelected(entity, TRUE);
		}
	}
	else
	{
		for (size_t i = 0; i < pSpeedTree->m_lpLeafsData->m_ListLeaf.size(); i++)
		{
			Leaf *pLeaf = pSpeedTree->m_lpLeafsData->m_ListLeaf[i];
			//pModelEditor->RemoveSelectedEntity(SCENEENTITY_SPEEDTREELEAF, (void*)pLeaf);
			pModelEditor->GetSelectionTool().SetSelected(KG3DSceneEntity(SCENEENTITY_SPEEDTREELEAF, (void*)pLeaf), FALSE);
		}
	}
Exit0:
	return;
}

void KG3DSceneModelEditor::KG3DSpeedTreeModifier::SetSpeedTree(KG3DModelSpeedTree* pSpeedTree)
{
	this->pSpeedTree = pSpeedTree;
}


void KG3DSceneModelEditor::RestoreIsolated()
{
	//m_listSelectableEntity = m_lstIsolated;

	this->GetSelectionTool().CopySelGroupToSelectable(s_strIsoSelGroup);
}

//(by dengzhihui Refactor 2006年11月23日
HRESULT KG3DSceneModelEditor::GetSpeedTreeModifier(IEKG3DSpeedTreeModifier** pModifier)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pModifier);

    *pModifier = &m_SpeedTreeModifier;

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DSceneModelEditor::GetScene(IEKG3DScene** ppScene)
{
	_ASSERTE(ppScene);

	if (ppScene)
	{
		*ppScene = static_cast<IEKG3DScene*>(this);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneModelEditor::GetSceneEditorBase(IEKG3DSceneEditorBase** ppSceneBase)
{
	_ASSERTE(ppSceneBase);

	if (ppSceneBase)
	{
		*ppSceneBase = static_cast<IEKG3DSceneEditorBase*>(this);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneModelEditor::GetSceneSelector(IEKG3DSceneSelectBase** ppSelector)
{
	_ASSERTE(ppSelector);
	if (ppSelector)
	{
		*ppSelector = static_cast<IEKG3DSceneSelectBase*>(this);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneModelEditor::GetSocketCount(INT* nCount)
{
	if (nCount)
	{
		*nCount = (INT)m_vecModelsForSockets.size();
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneModelEditor::GetSocketName(INT nIndex, LPTSTR lpName, INT nLen)
{
	_ASSERTE(lpName && nLen > 0);
	_ASSERTE(nLen > (INT)m_vecModelsForSockets[nIndex]->m_scName.length());

	if (nLen > (INT)m_vecModelsForSockets[nIndex]->m_scName.length())
	{
		lstrcpy(lpName, m_vecModelsForSockets[nIndex]->m_scName.c_str());
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DSceneModelEditor::GetModelForSockets(INT nIndex, IEKG3DModel** ppModel)
{
	*ppModel = m_vecModelsForSockets[nIndex];

	return S_OK;
}

HRESULT KG3DSceneModelEditor::ProcessEntity(BOOL bAdd)
{
	KG3DSceneEntity Entity = GetSpeedTreeModifier()->ConvertToEntity();
	if (bAdd)
	{
		//AddSelectedEntity(Entity);
		this->GetSelectionTool().SetSelected(Entity, TRUE);
	}
	else
	{
		//RemoveSelectedEntity(Entity.GetType(), Entity.m_pPointer);
		this->GetSelectionTool().SetSelected(Entity, FALSE);
	}
	return S_OK;
}

HRESULT KG3DSceneModelEditor::MoveSelectEntityToBack()
{
	//m_listSelectedEntityBack = m_listSelectedEntity;
	this->GetSelectionTool().CopyCurSelectionToSelGroup(s_strSelBackUp);

	//m_listSelectableEntityBack = m_listSelectableEntity;
	this->GetSelectionTool().CopySelectableToSelGroup(s_strSeletableBack);

	//m_listSelectedEntity.clear();
	this->GetSelectionTool().ClearSelection();

	this->GetSelectionTool().ClearSelectable();
	return S_OK;
}

HRESULT KG3DSceneModelEditor::CopyBackToSelectedEntity()
{
	//m_listSelectedEntity = m_listSelectedEntityBack;
	this->GetSelectionTool().CopySelGroupToCurSelection(s_strSelBackUp);

	//m_listSelectableEntity = m_listSelectableEntityBack;
	this->GetSelectionTool().CopySelGroupToSelectable(s_strSeletableBack);
	return S_OK;
}


HRESULT KG3DSceneModelEditor::PushEntityToSelected(SCENEENTITYTYPE eEntityType, PVOID pvEntityPointer)
{
    KG3DSceneEntity Entity(eEntityType, pvEntityPointer);

	//this->GetSelectionTool().SetSelectable(Entity, TRUE);//<Selectable Refactor>
	this->IEInsertEntitySelectable(eEntityType, pvEntityPointer, TRUE);

	this->GetSelectionTool().SetSelected(Entity, TRUE);
	return S_OK;
}
HRESULT KG3DSceneModelEditor::KG3DSpeedTreeModifier::SetAllLeavesSelectable(KG3DSceneEditorBase& editBae)
{
	return Leaves.SetAllLeavesSelectable(editBae);
}
//KG3DSceneEntityList KG3DSceneModelEditor::KG3DSpeedTreeModifier::ConvertLeavesToEntityList()
//{
//    return Leaves.ConvertLeavesToEntityList();
//}
//)

HRESULT KG3DSceneModelEditor::GetHeight(D3DXVECTOR3* pOut,D3DXVECTOR3* pIn)
{
	*pOut = *pIn;
	return S_OK;
}

HRESULT KG3DSceneModelEditor::LoadSFXMeteor(LPCSTR pFileName,LPCSTR pSocketName,IEKG3DModel* piModel)
{	
	HRESULT hr = E_FAIL;
	KG3DModel* pModel =NULL;
	KG3DSFX* pSFXMeteor=NULL;
	BindExtraInfo ExtInfo;

	KG_PROCESS_ERROR(piModel);
	KG_PROCESS_ERROR(pFileName);
	KG_PROCESS_ERROR(pSocketName);

	piModel->FindSocket(pSocketName,&ExtInfo);
	KG_PROCESS_ERROR(ExtInfo.pModel);
	pModel = ExtInfo.pModel;

	hr = g_cModelTable.LoadResourceFromFile(pFileName, 0, MLO_MULTI, (IKG3DResourceBase**)&pSFXMeteor);
	KG_COM_PROCESS_ERROR(hr);
	m_VecpSFXMeteor.push_back(pSFXMeteor);
	hr = pSFXMeteor->BindToSocket(pModel,pSocketName);
	
	return hr;
Exit0:
	return E_FAIL;
}

void KG3DSceneModelEditor::ReleaseSFXMeteor()
{
	vector<KG3DSFX*>::iterator i = m_VecpSFXMeteor.begin();
	while (i != m_VecpSFXMeteor.end())
	{
		SAFE_RELEASE(*i);
		i++;
	}
	m_VecpSFXMeteor.clear();
}

void KG3DSceneModelEditor::SetfBoneWeight(float& fWeight)
{
	g_fBoneWeight = fWeight;
}

void KG3DSceneModelEditor::SetbOnlyShowPitchOn(BOOL val)
{
	g_bOnlyShowPitchOn = val;
}

BOOL KG3DSceneModelEditor::GetbOnlyShowPitchOn()
{
	return g_bOnlyShowPitchOn;
}

IEKG3DBillBoardCloudCreater* KG3DSceneModelEditor::GetBillBoardCloudCreater()
{
//<BillboardCloudV1>
	if (!m_lpBillboardCloudCreater)
	{
		m_lpBillboardCloudCreater = new KG3DBillboardCloudCreater;
		if(!m_lpBillboardCloudCreater)
			return NULL;
		m_lpBillboardCloudCreater->SetModelEditor(this);
	}
	return m_lpBillboardCloudCreater;
//</BillboardCloudV1>
}

IEKG3DBillboardCloudCreater2* KG3DSceneModelEditor::GetBillboardCloudCreater2()
{
	if(NULL == m_lpBillboardCloudCreater2)
	{
		//<BillboardCloud>//暂时屏蔽掉 Last modify by Chen TianHong:2008-11-6 14:50:40
		//m_lpBillboardCloudCreater2 = KG3DBillboardCloudCreater2::Build(NULL, NULL, NULL);
		//</BillboardCloud>
	}
	return m_lpBillboardCloudCreater2;
}

HRESULT KG3DSceneModelEditor::PlayerNormalTool_LoadStandModel(LPCSTR pFileName)
{
	HRESULT hr = E_FAIL;
	_ASSERTE(pFileName);
	if (pFileName && m_lpPlayerTools)
	{
		KG3DModel* pModel = NULL;
		hr = g_cModelTable.LoadResourceFromFile(pFileName,0,0/*MLO_TEXTURE_MULTI*/,(IKG3DResourceBase**)&pModel);
		if(SUCCEEDED(hr))
		{
			RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_MODEL, pModel));
			AddRenderEntity(KG3DSceneEntity(SCENEENTITY_MODEL, pModel), TRUE, FALSE);
			
			KG3DModel* pStandModel = m_lpPlayerTools->GetStandModel();
			if(pStandModel)
				RemoveRenderEntity(KG3DSceneEntity(SCENEENTITY_MODEL, pStandModel));
			hr = m_lpPlayerTools->SetStandModel(pModel);	
		}
	}

	return hr;
}

HRESULT KG3DSceneModelEditor::PlayerNormalTool_AddMDL(LPCSTR pFileName)
{
	_ASSERTE(pFileName);
	if (pFileName)
	{
		return m_lpPlayerTools->LoadModifyModelST((LPSTR)pFileName);
	}

	return E_FAIL;
}

HRESULT KG3DSceneModelEditor::PlayerNormalTool_AddModelFromIni(LPCSTR pFileName)
{
	_ASSERTE(pFileName);
	if (pFileName)
	{
		return m_lpPlayerTools->LoadPartFromIni((LPSTR)pFileName);
	}

	return E_FAIL;
}

HRESULT KG3DSceneModelEditor::PlayerNormalTool_ProcessAllModel()
{
	return S_OK;
}

HRESULT KG3DSceneModelEditor::PlayerNormalTool_SetCurrentModelPart(int nPart,int nIndex)
{
	return m_lpPlayerTools->SetCurrentModelPart(nPart,nIndex);
}

HRESULT KG3DSceneModelEditor::PlayerNormalTool_GetModelPartVector(int nIndex,vector<string>* pvec)
{
	return m_lpPlayerTools->GetModelPartVector(nIndex,pvec);
}

HRESULT KG3DSceneModelEditor::AddOutputWindow(LPCTSTR szName, HWND hWnd, DWORD dwType, DWORD_PTR dwOption, int *pOutputWindowID)
{
	HRESULT hr = E_FAIL;
	hr = KG3DSceneEditorBase::AddOutputWindow(szName,hWnd,dwType,dwOption,pOutputWindowID);

	if(dwType==OUTPUTWND_MODELEDITOR_MAIN)
	{
		KG3DSceneOutputWnd *pWnd = static_cast<KG3DSceneOutputWnd *>(m_SceneWndManager->GetOutputWindow(*pOutputWindowID));
		if(pWnd)
		{	
			RECT r = pWnd->m_Rect;
			DWORD dwWidth = r.right - r.left;
			DWORD dwHeight =  r.bottom - r.top;
			pWnd->ResetDevice(TRUE, dwWidth, dwHeight);	

			pWnd->EnablePostEffect(POST_EFFECT_SOCKWARE, TRUE);
			pWnd->EnablePostEffect(POST_EFFECT_MOTIONBLUE, FALSE);

			pWnd->EnablePostEffect(POST_EFFECT_SSAO, FALSE);
			pWnd->EnablePostEffect(POST_EFFECT_FOG, FALSE);
			pWnd->EnablePostEffect(POST_EFFECT_DETAIL, FALSE);		
			pWnd->EnablePostEffect(POST_EFFECT_HSI , FALSE);
            pWnd->EnablePostEffect(POST_EFFECT_CURVE_CMYK, FALSE);
			pWnd->EnablePostEffect(POST_EFFECT_GODRAY, FALSE);
			pWnd->EnablePostEffect(POST_EFFECT_AOE , FALSE);
			pWnd->EnablePostEffect(POST_EFFECT_BLOOM, FALSE);
			pWnd->EnablePostEffect(POST_EFFECT_SHADOW, FALSE);

			//pWnd->SetShockWavePower(g_eEngineOption.fShockWavePower);
			m_SceneWndManager->SetPrimaryWindow(*pOutputWindowID);

		}///if(pWnd)
	}
	return S_OK;
}

HRESULT KG3DSceneModelEditor::ShockWaveRender()
{
	// Here render all shock wave effect
	//return KG3DScene::ShockWaveRender();
	KG3DRenderState RS;
	// if (SUCCEEDED(pWnd->ShockWaveRenderTargetSet()))
	{
		RS.SetRenderState(D3DRS_ZENABLE, TRUE);
		KG3DSceneEntityList::iterator i = m_listPostRenderEntity.begin();
		while (i != m_listPostRenderEntity.end())
		{
			i->FrameMove();
			i->Render();
			++i;
		}

	}
	return S_OK;
}

HRESULT KG3DSceneModelEditor::RemoveRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable)
{
	HRESULT hr = E_FAIL;
	hr = KG3DSceneEditorBase::RemoveRenderEntity(pEntity,uClass,bSelectable);
	KG_PROCESS_ERROR(SUCCEEDED(hr));
	KG3DModel *pModel = static_cast<KG3DModel*>(pEntity);
	KG_PROCESS_ERROR(pModel);
	{
		KG3DSceneEntity Entity(SCENEENTITY_MODEL,pModel);
		if (pModel->m_nIsSockWave)
		{
			m_listPostRenderEntity.remove(Entity);
		}
		hr = S_OK;
	}
Exit0:
	return hr;
}


HRESULT KG3DSceneModelEditor::RemoveRenderEntity( KG3DSceneEntity Entity, BOOL bEitherRemoveInSelectedList /*= TRUE*/ )
{
	return KG3DSceneEditorBase::RemoveRenderEntity(Entity,bEitherRemoveInSelectedList);
}

HRESULT KG3DSceneModelEditor::GetAllModels(vector<IEKG3DModel*>* pvecModels)
{
	HRESULT hr = E_FAIL;
	KGLOG_PROCESS_ERROR(pvecModels);

	pvecModels->clear();

	if(g_cEngineOption.bCpuSkin)
	{
		m_EntityMgr.GetAllModels(pvecModels);
	}
	else
	{
		for (size_t i = 0; i < m_Models.size(); i++)
		{
			KG3DModel* pModel = m_Models[i];
			if (pModel)
			{
				if(pModel->GetType() != MESHTYPE_MODELST)
					pvecModels->push_back(m_Models[i]);

				GetAllModelsForModel(pvecModels,m_Models[i]);
			}
		
		}
	}

	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSceneModelEditor::GetAllModelsForModel(vector<IEKG3DModel*>* pvecModels,KG3DModel* pModel)
{
	HRESULT hRetCode = S_OK;
	int nCount = pModel->GetNumModel();
	LPCTSTR pszName = NULL;

	int i = 0;
	for(i = 0; i < nCount; i++)
	{
		IEKG3DModel* pieSubModel = NULL;
		IKG3DModel* pSubModel = NULL;

		hRetCode = pModel->GetModel(i, &pSubModel);
		KGLOG_COM_PROCESS_ERROR(hRetCode);

		pieSubModel = dynamic_cast<IEKG3DModel*>(pSubModel);
		KG_PROCESS_ERROR(pieSubModel);

		pvecModels->push_back(pieSubModel);
	}

	{
		IEKG3DModel* pieSubModel = NULL;
		hRetCode = pModel->GetFirstChildModel(&pieSubModel);
		if(pieSubModel)
		{
			hRetCode = pieSubModel->GetName(&pszName);
			KGLOG_COM_PROCESS_ERROR(hRetCode);

			i++;

			if(SUCCEEDED(hRetCode))
			{
				hRetCode = pModel->GetNextChildModel(&pieSubModel);
				KG_PROCESS_ERROR(pieSubModel);

				hRetCode = pieSubModel->GetName(&pszName);
				KGLOG_COM_PROCESS_ERROR(hRetCode);

				pvecModels->push_back(pieSubModel);

				i++;
			}
		}
	}

Exit0:
	return S_OK;
}



void KG3DSceneModelEditor::MsgOnMouseWheel(WPARAM wParam, LPARAM lParam)
{
	int k = 0;
	k |=wParam;
	float S = 1.0F;
	if (k>0)
	{
		S = -0.9F;
		m_fWheel -= 0.05f;
	}
	else
	{
		S = 1.1F;
		m_fWheel += 0.05f;
	}

	if(m_SceneWndManager->GetCurOutputWnd())
	{
		D3DXVECTOR3 PosMove(0,0,0);
		D3DXVECTOR3 Pos,LookAt;
		m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetPosition(&Pos);
		m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetLookAtPosition(&LookAt);

		D3DXVECTOR3 N = Pos - LookAt;
		float R = D3DXVec3Length(&N);
		D3DXVec3Normalize(&N,&N);

		float H = 1;
		if(GetAsyncKeyState(VK_LCONTROL) & 0x8000)
			H = 30;
		Pos = Pos +  N * S * R * 0.1F;
		m_SceneWndManager->GetCurOutputWnd()->GetCamera().SetPosition(Pos);
	}
}

void KG3DSceneModelEditor::HandleEngineMessages( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	if (Message.m_uMessage == KM_STATE_CHANGE)
	{
		DWORD_PTR newState = Message.m_dwWParam;
		//DWORD_PTR oldState = Message.m_dwLParam;
		//if (newState != oldState )
		{
			if (newState == SCENESTATE_SCALE)	//这样就等于OnToggleStateScaling的调用时机了
			{
				DWORD dwCurScaleMode = m_SpeedTreeModifier.GetScaleMode();
				m_SpeedTreeModifier.SetScaleMode((dwCurScaleMode + 1) % KG3DSpeedTreeModifier::ENU_SCALE_COUNT);
			}
		}
	}

	KG3DSceneSelectBase::HandleEngineMessages(Message, pSender);
}

HRESULT KG3DSceneModelEditor::GetSelectionToolInterface( IEKG3DSceneSelectionTool** ppTool )
{
	return KG3DScene::GetSelectionToolInterface(ppTool);
}

void __stdcall KG3DSceneModelEditor::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	if (Message.m_uMessage == KM_SELECT && Message.m_dwLParam == EM_EVENT_STATE_END)
	{
		if(m_Models.size() > 0)
		{
			DWORD dwIncreaseMode = this->GetIncreaseMode();
			if (m_Models[0]->GetType() == MESHTYPE_SPEEDTREE)
			{
				KG3DModelSpeedTree* pSpdTree = static_cast<KG3DModelSpeedTree*>(m_Models[0]);

                TypeEntityAccessProxy& proxy = this->GetSelectionTool().GetSelectionAccessProxy();
                std::vector<KG3DSceneEntity> vecEntities;
                for (proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
                {
                    KG3DSceneEntity& entityCur = proxy.GetCur();
                    vecEntities.push_back(entityCur);
                }


				if (dwIncreaseMode == EM_SELECTION_INCREASE_MODE_DEFAULT)
				{
					m_SpeedTreeModifier.UnSelectAll();
				}

                for (size_t i = 0; i < vecEntities.size(); ++i)
                {
                    KG3DSceneEntity& entity = vecEntities[i];
                    if (entity.GetType() == SCENEENTITY_SPEEDTREELEAF)
                    {
                        m_SpeedTreeModifier.SelectLeaf(reinterpret_cast<Leaf*>(entity.m_pPointer));
                    }
                }

				if (m_SpeedTreeModifier.pfUpdateUI)
				{
					m_SpeedTreeModifier.pfUpdateUI(pSpdTree, m_SpeedTreeModifier.pEditorDlg);
				}
			}

			{
				KG3DSceneEntity Entity = m_SpeedTreeModifier.ConvertToEntity();
				if (m_SpeedTreeModifier.IsActive())
				{
					//RemoveSelectedEntity(Entity.GetType(), Entity.m_pPointer);//不用先Remove，里面会检查是否重复的
					//AddSelectedEntity(Entity);

					this->GetSelectionTool().SetSelected(Entity, TRUE);
				}
				else
				{
					//RemoveSelectedEntity(Entity.GetType(), Entity.m_pPointer);
					this->GetSelectionTool().SetSelected(Entity, FALSE);
				}
			}

		}//if(m_Models.size() > 0)
	}//if (Message.m_uMessage == KM_SELECT && Message.m_dwLParam == EM_EVENT_STATE_END)
}
HRESULT KG3DSceneModelEditor::ZoomToObj()
{	
	KG3DSceneEntityList::iterator i = m_listRenderEntity.begin();
	if(i == m_listRenderEntity.end())
		return E_FAIL;
	KG3DSceneEntity* pEntity = &(*i);

	D3DXVECTOR3 Pos;
	D3DXVECTOR3 Eye,Lookat;

	m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetPosition(&Eye);
	m_SceneWndManager->GetCurOutputWnd()->GetCamera().GetLookAtPosition(&Lookat);

	AABBOX Box;
	 pEntity->GetBBox(&Box);
	Pos = Box.GetCenter();
	float R =  D3DXVec3Length(&(Box.A - Box.B));

	D3DXVECTOR3 N = Eye - Lookat;
	D3DXVec3Normalize(&N,&N);
	Lookat = Pos;
	Eye = Pos + N * R;

	KG3DCamera& Cam = m_SceneWndManager->GetCurOutputWnd()->GetCamera();
	Cam.SetPositionLookAtUp(&Eye, &Lookat, NULL);
	return S_OK;
}



VOID KG3DSceneModelEditor::ZoomToSelection()
{
	ZoomToObj();
}

HRESULT KG3DSceneModelEditor::GenerateBspForMesh(LPSTR pFileName)
{
	HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
	KG3DMesh* pMesh = NULL;

	hrRetCode = g_cMeshTable.LoadResourceFromFile(pFileName,0,0,(IKG3DResourceBase**)&pMesh);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

    hrRetCode = pMesh->CreateBspFile();
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrResult = S_OK;
Exit0:
	SAFE_RELEASE(pMesh);
	return hrResult;
}

HRESULT KG3DSceneModelEditor::GenerateLodForMesh(LPSTR pFileName,int& nMediumFace,int& nLowFace,float fLowRation,float fMediumRation)
{
	HRESULT hr = E_FAIL;
	KG3DMesh* pMesh = NULL;
	int nMaxFace = 0;
	int nMinFace = 0;

	TCHAR szName[MAX_PATH];
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];

	TCHAR szWorkingDir[MAX_PATH];
	g_cEngineManager.GetDefWorkDirectory(szWorkingDir,MAX_PATH);

	wsprintf(path_buffer,"%s",pFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );


	hr = g_cMeshTable.LoadResourceFromFile(pFileName,0,0,(IKG3DResourceBase**)&pMesh);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = pMesh->CreateProgressMesh();
	KGLOG_COM_PROCESS_ERROR(hr);

	pMesh->GetProgressMeshMaxFaces(&nMaxFace);
	pMesh->GetProgressMeshMinFaces(&nMinFace);
	//////////////////////////////////////////////////////////////////////////

	wsprintf(szName,"%s%s%s%s_l%s",szWorkingDir,drive,dir,fname,ext);
	nLowFace = (int)(nMinFace + (nMaxFace - nMinFace) * fLowRation);
	pMesh->SetNumFaceLOD(nLowFace);
	hr = pMesh->SavePrograssMeshAsLowMesh(szName);
	KGLOG_COM_PROCESS_ERROR(hr);
	//////////////////////////////////////////////////////////////////////////

	wsprintf(szName,"%s%s%s%s_m%s",szWorkingDir,drive,dir,fname,ext);
	nMediumFace = (int)(nMinFace + (nMaxFace - nMinFace) * fMediumRation);
	pMesh->SetNumFaceLOD(nMediumFace);
	hr = pMesh->SavePrograssMeshAsLowMesh(szName);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:

	SAFE_RELEASE(pMesh);
	return hr;
}

IEKG3DAnimationTagContainer* KG3DSceneModelEditor::GetTagContainer()
{
	if (!m_pTagContainer)
	{
		m_pTagContainer = new KG3DAnimationTagContainer;
		_ASSERTE(m_pTagContainer);
	}
	return m_pTagContainer;
}

void KG3DSceneModelEditor::EnableLOD(BOOL bEnable)
{
	s_bUseLOD = bEnable;
}

HRESULT KG3DSceneModelEditor::AddPostRenderEntity(IKG3DResourceBase *pEntity, unsigned uClass, BOOL bSelectable)
{
	KG3DSceneEntity Entity(SCENEENTITY_MODEL,pEntity);
	m_listPostRenderEntity.push_back(Entity);
	return S_OK;
}

BOOL KG3DSceneModelEditor::IsDrawGound()
{
	return m_bDrawGroundGrid;
}

void KG3DSceneModelEditor::SetDrawGound(BOOL bDraw)
{
	m_bDrawGroundGrid = bDraw;
}

HRESULT KG3DSceneModelEditor::ClearSelection()
{
	HRESULT hResult = KG3DSceneSelectBase::ClearSelection();
	m_SpeedTreeModifier.UnSelectAll();
	return hResult;
}


HRESULT KG3DSceneModelEditor::SetEditState(DWORD dwState)
{
	KG3DSceneEditorBase::SetEditState(dwState);
	GetMessageSender(KM_STATE_CHANGE).PostMessage(KG3DMessage(KM_STATE_CHANGE, dwState, 0));
	return S_OK;
}