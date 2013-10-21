////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DBaseCoordImp.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-11-27 14:20:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DBaseCoordImp.h"
#include "KG3DFrameDrawer.h"
#include "KG3DMeshHelpers.h"
#include "IEKG3DSceneOutputWnd.h"
#include "KG3DGraphicsStructEx.h"

#include "IEEditorTypes.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

D3DXVECTOR3 KG3DBaseCoordImp::RefineInterPoint( const RepresentData& rpData, const D3DXVECTOR3& vInter, const ResultData& retData )
{
	KG3DCOORD IntersectedCoord = retData.IntersectedCoord;
	if (IntersectedCoord >= KG3DCOORD_FIRST_AXIS && IntersectedCoord < KG3DCOORD_FIRST_AXIS + KG3DCOORD_AXIS_COUNT)
	{
		//投射到轴上得到移动距离
		const D3DXVECTOR3& Axis = D3DXVec3GetNormalOfPlane(IntersectedCoord - KG3DCOORD_FIRST_AXIS);

		//retData.EndPos = retData.StartPos + Axis * D3DXVec3Dot(&vDistance, &Axis);
		return ProjectPointToRay(vInter, rpData.vecPos, Axis);
	}
	else if(IntersectedCoord >= KG3DCOORD_FIRST_PLANE && IntersectedCoord < KG3DCOORD_FIRST_PLANE + KG3DCOORD_PLANE_COUNT)
	{
		const D3DXVECTOR3& vPlaneNormal = D3DXVec3GetNormalOfPlane(IntersectedCoord - KG3DCOORD_FIRST_PLANE);
		return ProjectPointToPlane(vInter, rpData.vecPos, vPlaneNormal);
	}
	else if (IntersectedCoord == KG3DCOORD_INTEGRATION)
	{
		return vInter;
	}
	_ASSERTE(NULL && _T("IntersectedCoord错误"));
	return D3DXVECTOR3(0,0,0);
}
DWORD KG3DBaseCoordImp::StateTransforming::CheckInput(KG3DBaseCoordImp& instance)
{
	unsigned uCode = instance.m_messageCur.m_uMessage;
	if(uCode == KM_STATE_CHANGE)
	{
		DWORD_PTR NewState = instance.m_messageCur.m_dwWParam;
		if (NewState != instance.GetActivationState())
			return EM_EDITOR_TOOL_STATE_NOTATIVATED;
	}
	else if(uCode == KM_EXECUTE_ACTION)
	{
		do
		{
			KG3DMessageActionExecutionStruct* p = reinterpret_cast<KG3DMessageActionExecutionStruct*>(instance.m_messageCur.m_dwWParam);
			_ASSERTE(NULL != p);
			if(p->nActionID != EXEACTION_LEFE_KEY_DOWN )
				break;

			if (p->nExecState != 1)//鼠标左键弹起
				return EM_EDITOR_TOOL_STATE_ATIVATED;

			IEKG3DSceneOutputWnd* pOutputWnd = reinterpret_cast<IEKG3DSceneOutputWnd*>(instance.m_messageCur.m_dwLParam);
			if (NULL == pOutputWnd)
				break;

			pOutputWnd->GetPickRay(&instance.m_InputData.vMouseRaySrc, &instance.m_InputData.vMouseRayNormalizedDir, NULL);

			_ASSERTE(instance.m_ResultData.IntersectedCoord >= KG3DCOORD_FIRST_AXIS 
				&& instance.m_ResultData.IntersectedCoord <= KG3DCOORD_INTEGRATION);

			D3DXVECTOR3 vInter;
			BOOL bRet = KG3DBaseCoordImp::IntersectPlaneAndFindPoint(instance.m_ResultData.IntersectedCoord
				, instance.m_RepresentData.vecPos, instance.m_InputData.vMouseRaySrc
				, instance.m_InputData.vMouseRayNormalizedDir, vInter);
			if (! bRet)
				break;

			instance.m_ResultData.EndPos = RefineInterPoint(instance.m_RepresentData, vInter, instance.m_ResultData);
		}while(false);
	}
	
	return EM_EDITOR_TOOL_STATE_TRANSFORMING;
}

DWORD KG3DBaseCoordImp::StateTransforming::GetStateID()
{
	return EM_EDITOR_TOOL_STATE_TRANSFORMING;
}
DWORD KG3DBaseCoordImp::StateActivated::CheckInput(KG3DBaseCoordImp& instance)
{
	KG3DMessage& Message = instance.m_messageCur;
	if(Message.m_uMessage == KM_STATE_CHANGE)
	{
		DWORD_PTR NewState = Message.m_dwWParam;
		if (NewState != instance.GetActivationState())
			return EM_EDITOR_TOOL_STATE_NOTATIVATED;
	}
	else if(Message.m_uMessage == KM_EXECUTE_ACTION)
	{
		KG3DMessageActionExecutionStruct* p = reinterpret_cast<KG3DMessageActionExecutionStruct*>(Message.m_dwWParam);
		_ASSERTE(NULL != p);
		switch(p->nActionID)
		{
		case EXEACTION_LEFE_KEY_DOWN://鼠标左键按下的话，找到并记录碰撞点和碰撞面，如果有就开始旋转
			{
				if(p->nExecState != 1)
					break;

				IEKG3DSceneOutputWnd* pOutputWnd = reinterpret_cast<IEKG3DSceneOutputWnd*>(Message.m_dwLParam);
				if (NULL == pOutputWnd)
					break;

				pOutputWnd->GetPickRay(&instance.m_InputData.vMouseRaySrc, &instance.m_InputData.vMouseRayNormalizedDir, NULL);

				D3DXVECTOR3 vInter(0,0,0);
				KG3DCOORD	IntersectedCoord = KG3DCOORD_INVALID;
			
				BOOL bRet = KG3DBaseCoordImp::IntersectMeshAndFindPoint(instance.m_RepresentData, instance.m_InputData
					, vInter, IntersectedCoord);
				if (! bRet)
					break;
				
				instance.m_ResultData.IntersectedCoord = IntersectedCoord;
				D3DXVECTOR3 vStartPos = RefineInterPoint(instance.m_RepresentData, vInter, instance.m_ResultData);

				instance.m_ResultData.StartPos = vStartPos;
				instance.m_ResultData.EndPos = vStartPos;

				return EM_EDITOR_TOOL_STATE_TRANSFORMING;
			}
			break;
		case EXEACTION_MOUSE_MOVE:	//鼠标移动的时候也尝试找碰撞面，并高亮碰撞面
			{
				IEKG3DSceneOutputWnd* pOutputWnd = reinterpret_cast<IEKG3DSceneOutputWnd*>(Message.m_dwLParam);
				if (NULL == pOutputWnd)
					break;

				pOutputWnd->GetPickRay(&instance.m_InputData.vMouseRaySrc, &instance.m_InputData.vMouseRayNormalizedDir, NULL);

				D3DXVECTOR3 vInter(0,0,0);
				KG3DCOORD	IntersectedCoord = KG3DCOORD_INVALID;
				BOOL bRet = KG3DBaseCoordImp::IntersectMeshAndFindPoint(instance.m_RepresentData, instance.m_InputData
					, vInter, IntersectedCoord);
				if (! bRet)
					break;

				instance.m_ResultData.IntersectedCoord = IntersectedCoord;
				return EM_EDITOR_TOOL_STATE_ATIVATED;
			}
			break;
		default:
			break;
		}

		instance.m_ResultData.IntersectedCoord = KG3DCOORD_INVALID;
	}

	return EM_EDITOR_TOOL_STATE_ATIVATED;
}

DWORD KG3DBaseCoordImp::StateActivated::GetStateID()
{
	return EM_EDITOR_TOOL_STATE_ATIVATED;
}
DWORD KG3DBaseCoordImp::StateNotActivated::CheckInput( KG3DBaseCoordImp& instance )
{
	KG3DMessage& Message = instance.m_messageCur;

	if(Message.m_uMessage == KM_STATE_CHANGE)
	{
		DWORD_PTR NewState = Message.m_dwWParam;
		if (NewState == instance.GetActivationState())
			return EM_EDITOR_TOOL_STATE_ATIVATED;
	}

	return EM_EDITOR_TOOL_STATE_NOTATIVATED;
}

DWORD KG3DBaseCoordImp::StateNotActivated::GetStateID()
{
	return EM_EDITOR_TOOL_STATE_NOTATIVATED;
}
KG3DBaseCoordImp::KG3DBaseCoordImp(KG3DMessageBroadcasterGroup& Sc)
:m_dwMode(KG3DCOORD_WORLD)
,m_pStateMachine(NULL)
{
	HRESULT hr = Sc.RegisterListener(KM_EXECUTE_ACTION, *this);
	_ASSERTE(SUCCEEDED(hr));
	KG_USE_ARGUMENT(hr);
	hr = Sc.RegisterListener(KM_STATE_CHANGE, *this);
	_ASSERTE(SUCCEEDED(hr));
	hr = Sc.RegisterListener(KM_MOUSE_MOUSE, *this);
	_ASSERTE(SUCCEEDED(hr));

	//bool bRet = g_GetSystemEventListener().RegisterReceiver(this);
	//_ASSERTE(bRet);
}
KGStateMachine::BuildStruct KG3DBaseCoordImp::GetStateMachineStateDesc()
{
	static StateNotActivated s0;
	static StateActivated s1;
	static StateTransforming s2;
	static KGStateMachineState* states[] = 
	{
		&s0,
		&s1,
		&s2,
	};
	KGStateMachine::BuildStruct bs;
	bs.pStates = states;
	bs.uStateCount = _countof(states);
	bs.dwInitStateID = EM_EDITOR_TOOL_STATE_NOTATIVATED;
	return bs;
}
KG3DBaseCoordImp::~KG3DBaseCoordImp()
{
	UnInit();
}
HRESULT STDMETHODCALLTYPE KG3DBaseCoordImp::Init()
{
	KGStateMachine::BuildStruct bs = GetStateMachineStateDesc();
	m_pStateMachine = KGStateMachine::Build(NULL, reinterpret_cast<DWORD_PTR>(&bs), NULL);
	KG_PROCESS_ERROR(NULL != m_pStateMachine);

#if defined(DEBUG) | defined(_DEBUG)
	for (size_t i = 0; i < _countof(m_RepresentData.MeshForAxis); ++i)
	{
		LPD3DXMESH p = m_RepresentData.MeshForAxis[i];
		if(p)
		{
			_ASSERTE((p->GetOptions() & D3DXMESH_MANAGED) && _T("必须是Managed型的Mesh"));
		}
	}
	for (size_t i = 0; i < _countof(m_RepresentData.MeshForPlane); ++i)
	{
		LPD3DXMESH p = m_RepresentData.MeshForPlane[i];
		if(p)
		{
			_ASSERTE((p->GetOptions() & D3DXMESH_MANAGED) && _T("必须是Managed型的Mesh"));
		}
	}
	{
		LPD3DXMESH p = m_RepresentData.MeshForIntegration;
		if(p)
		{
			_ASSERTE((p->GetOptions() & D3DXMESH_MANAGED) && _T("必须是Managed型的Mesh"));
		}
	}
#endif
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT STDMETHODCALLTYPE KG3DBaseCoordImp::UnInit()
{
	m_RepresentData.ReleaseMeshes();
    SAFE_RELEASE(m_pStateMachine);
	return S_OK;
}

HRESULT STDMETHODCALLTYPE KG3DBaseCoordImp::FrameMove()
{
	return S_OK;
}
VOID KG3DBaseCoordImp::RenderIntersectMesh(RepresentData& RpData, const D3DXMATRIX& matView, const D3DXMATRIX& matProj)
{
	//检查其它Drawer是否正确
#if 0
	g_CheckFrameDrawers(RpData.vecPos, GetSelectorRingRadius(RpData));
#endif

	GraphicsStruct::RenderState rs1(D3DRS_AMBIENT, 0xffffffff);
	GraphicsStruct::RenderState rs2(D3DRS_ZENABLE, D3DZB_FALSE);

	GraphicsStruct::RenderState rs3(D3DRS_ALPHABLENDENABLE, TRUE);
	GraphicsStruct::RenderState rs4(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	GraphicsStruct::RenderState rs5(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	GraphicsStruct::RenderState rs6(D3DRS_CULLMODE, D3DCULL_NONE);
	GraphicsStruct::RenderState rs7(D3DRS_SPECULARENABLE, TRUE);
	GraphicsStruct::RenderState rs8(D3DRS_FILLMODE, D3DFILL_SOLID);
	GraphicsStruct::RenderState rs9(D3DRS_SHADEMODE, D3DSHADE_GOURAUD);


	GraphicsStruct::SaveMatrix SaveWorld(D3DTS_WORLD);
	GraphicsStruct::SaveMatrix SaveView(D3DTS_VIEW);
	GraphicsStruct::SaveMatrix SaveProj(D3DTS_PROJECTION);

	g_pd3dDevice->SetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	GraphicsStruct::TextureStageState	tssColorOp0(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	GraphicsStruct::TextureStageState	tssColorOp1(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	GraphicsStruct::TextureStageState	tssColorArg0(0, D3DTSS_COLORARG1, D3DTA_TFACTOR);
	GraphicsStruct::TextureStageState	tssAlphaOp0(0,D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
	GraphicsStruct::TextureStageState	tssAlphaArg1(0, D3DTSS_ALPHAARG1, D3DTA_TFACTOR);
	g_pd3dDevice->SetRenderState(D3DRS_TEXTUREFACTOR, 0xffffffff);

	D3DXMATRIX matUse;
	{
		D3DXMATRIX vMatScaling;
		FLOAT fScale = GetSelectorRingRadius(RpData) / em_mesh_range;
		D3DXMatrixScaling(&vMatScaling, fScale, fScale, fScale);
		D3DXMATRIX vMatTrans;
		D3DXMatrixTranslation(&vMatTrans, RpData.vecPos.x, RpData.vecPos.y, RpData.vecPos.z);
		matUse = vMatScaling * vMatTrans;
	}
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matUse);

	_ASSERTE(_countof(RpData.MeshForAxis) == _countof(RpData.MeshForPlane));
	for (size_t i = 0; i < _countof(RpData.MeshForAxis); ++i)
	{
		if(NULL != RpData.MeshForAxis[i])
			RpData.MeshForAxis[i]->DrawSubset(0);

		if(NULL != RpData.MeshForPlane[i])
			RpData.MeshForPlane[i]->DrawSubset(0);
	}
	if(NULL != RpData.MeshForIntegration)
		RpData.MeshForIntegration->DrawSubset(0);
}

BOOL KG3DBaseCoordImp::IntersectMeshAndFindPoint(const RepresentData& RpData, const InputData& IpData
														 , D3DXVECTOR3& vInter, KG3DCOORD& IntersectedCoord )
{
	//把碰撞线转换到Mesh的Local空间,然后求出交点

	D3DXVECTOR3 vSrcLocal;
	D3DXVECTOR3 vDirLocal;	

	D3DXMATRIX matUse;
	{
		D3DXMATRIX vMatScaling;
		{
			FLOAT fRadius = GetSelectorRingRadius(RpData);
			FLOAT fScale = fRadius / em_mesh_range;
			D3DXMatrixScaling(&vMatScaling, fScale,fScale,fScale);
		}

		D3DXMATRIX vMatTrans;
		{
			D3DXVECTOR3 vMeshPos = RpData.vecPos;
			D3DXMatrixTranslation(&vMatTrans, vMeshPos.x, vMeshPos.y, vMeshPos.z);
		}
		matUse = vMatScaling * vMatTrans;
		D3DXMATRIX vMatInv;
		D3DXMATRIX* pRet = D3DXMatrixInverse(&vMatInv, NULL, &matUse);
		if(NULL == pRet)
			return FALSE;

		D3DXVec3TransformCoord(&vSrcLocal, &IpData.vMouseRaySrc, &vMatInv);
		D3DXVec3TransformNormal(&vDirLocal, &IpData.vMouseRayNormalizedDir, &vMatInv);
		D3DXVec3Normalize(&vDirLocal, &vDirLocal);
	}

	FLOAT distRet = 0;
	BOOL bRet = FALSE;


	for(size_t i = 0; i < _countof(RpData.MeshForPlane); ++i)//对面的碰撞是优先的
	{
		if(NULL == RpData.MeshForPlane[i])
			continue;

		HRESULT hr = D3DXIntersect(RpData.MeshForPlane[i], &vSrcLocal, &vDirLocal
			, &bRet, NULL, NULL, NULL, &distRet, NULL, NULL);
		if(FAILED(hr) || !bRet)
			continue;

		D3DXVECTOR3 vRet = vSrcLocal + vDirLocal * distRet;
		D3DXVec3TransformCoord(&vInter, &vRet, &matUse);


		//如果交点距离比中心距离大，即在后面半圆上，根据Filter来决定是否要结果
		if(RpData.bFilterInter != em_filter_none)
		{
			FLOAT distCamToInter = D3DXVec3Length(&(RpData.CamPos - vInter));
			FLOAT distCamToCenter = D3DXVec3Length(&(RpData.CamPos - RpData.vecPos));

			if (RpData.bFilterInter == em_filter_front)
			{
				if(distCamToInter < distCamToCenter)
					continue;
			}
			else 
			{
				if(distCamToInter > distCamToCenter)
					continue;
			}				
		}

		IntersectedCoord = KG3DCOORD(KG3DCOORD_FIRST_PLANE + i);

		return TRUE;
	}

	//对中间的碰撞面第二优先
	do
	{
		if(NULL == RpData.MeshForIntegration)
			break;

		HRESULT hr = D3DXIntersect(RpData.MeshForIntegration, &vSrcLocal, &vDirLocal
			, &bRet, NULL, NULL, NULL, &distRet, NULL, NULL);
		if(FAILED(hr) || !bRet)
			break;

		D3DXVECTOR3 vRet = vSrcLocal + vDirLocal * distRet;
		D3DXVec3TransformCoord(&vInter, &vRet, &matUse);

		IntersectedCoord = KG3DCOORD_INTEGRATION;

		return TRUE;

	}while(false);

	for (size_t i = 0; i < _countof(RpData.MeshForAxis); ++i)
	{
		if(NULL == RpData.MeshForAxis[i])
			break;

		HRESULT hr = D3DXIntersect(RpData.MeshForAxis[i], &vSrcLocal, &vDirLocal
			, &bRet, NULL, NULL, NULL, &distRet, NULL, NULL);
		if(FAILED(hr) || !bRet)
			continue;

		D3DXVECTOR3 vRet = vSrcLocal + vDirLocal * distRet;
		D3DXVec3TransformCoord(&vInter, &vRet, &matUse);

		IntersectedCoord = KG3DCOORD(KG3DCOORD_FIRST_AXIS + i);

		return TRUE;
	}
	return FALSE;
}
BOOL KG3DBaseCoordImp::IntersectPlaneAndFindPoint( KG3DCOORD WhichCoord
														  , const D3DXVECTOR3& Center
														  , const D3DXVECTOR3& vSrc
														  , const D3DXVECTOR3& vDir
														  , D3DXVECTOR3& vInter )
{

	_ASSERTE(WhichCoord >= KG3DCOORD_FIRST_AXIS && WhichCoord <= KG3DCOORD_INTEGRATION);

	D3DXPLANE planeUse;
	if(WhichCoord >= KG3DCOORD_FIRST_AXIS && WhichCoord < KG3DCOORD_FIRST_AXIS + KG3DCOORD_AXIS_COUNT)
	{
		//如果选中的是线,那么找过这条线和发射线垂直的面，然后求交点
		const D3DXVECTOR3& vAxis = D3DXVec3GetNormalOfPlane(WhichCoord - KG3DCOORD_FIRST_AXIS);
		D3DXVECTOR3 vUp;
		D3DXVec3Cross(&vUp, &vAxis, &vDir);

		D3DXVECTOR3 vNormalOfBestPlane;
		D3DXVec3Cross(&vNormalOfBestPlane, &vUp, &vAxis);

		D3DXVec3Normalize(&vNormalOfBestPlane, &vNormalOfBestPlane);

		D3DXPlaneFromPointNormal(&planeUse, &Center, &vNormalOfBestPlane);
	}
	else if(WhichCoord >= KG3DCOORD_FIRST_PLANE && WhichCoord < KG3DCOORD_FIRST_PLANE + KG3DCOORD_PLANE_COUNT)
	{
		const D3DXVECTOR3& NormalUse = D3DXVec3GetNormalOfPlane(WhichCoord - KG3DCOORD_FIRST_PLANE);
		D3DXPlaneFromPointNormal(&planeUse, &Center, &NormalUse);
	}
	else if (WhichCoord == KG3DCOORD_INTEGRATION)
	{
		const D3DXPLANE planeIntergration = GetPlaneIntergration();
		const D3DXVECTOR3& NormalUse = (const D3DXVECTOR3&)planeIntergration;

		D3DXPlaneFromPointNormal(&planeUse, &Center, &NormalUse);
	}
	else
	{
		return FALSE;
	}

	const FLOAT someLargeLength = 100000.f;
	return NULL != D3DXPlaneIntersectLine(&vInter, &planeUse, &vSrc, &(vSrc + vDir * someLargeLength));
}
void __stdcall KG3DBaseCoordImp::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	UINT uCode =Message.m_uMessage;
#if defined(DEBUG) | defined(_DEBUG)
	if(uCode == KM_CAM_POS)
	{
		//测试时，用于设置摄像机的代码
		D3DXVECTOR3* pCamPos = reinterpret_cast<D3DXVECTOR3*>(Message.m_dwWParam);
		_ASSERTE(NULL != pCamPos);
		m_RepresentData.CamPos = *pCamPos;
	}
#endif
	{
		if(uCode != KM_STATE_CHANGE && uCode != KM_EXECUTE_ACTION)
			return;

		this->m_messageCur = Message;
		_ASSERTE(NULL != m_pStateMachine);
		m_pStateMachine->CheckInput(reinterpret_cast<DWORD_PTR>(this));
	}
}

HRESULT KG3DBaseCoordImp::SetPosition( const D3DXVECTOR3& Pos )
{
	m_RepresentData.vecPos = Pos;
	return S_OK;
}

HRESULT KG3DBaseCoordImp::Zoom( FLOAT zoomFactor )
{
	m_RepresentData.zoomFactor = zoomFactor;
	return S_OK;
}

ULONG KG3DBaseCoordImp::GetCurStateID()
{
	_ASSERTE(NULL != m_pStateMachine);
	return m_pStateMachine->GetCurState().GetStateID();
}

FLOAT KG3DBaseCoordImp::GetSelectorRingRadius(const RepresentData& data)
{
	//x,y,z,1乘以投射矩阵之后会变成x * _11 / z, y * _22 /z, _33 + _43/z, 1
	FLOAT CenterZ = D3DXVec3Length(&(data.vecPos - data.CamPos));
	if (MT_FLOAT_EQUAL(CenterZ, 0))
	{
		CenterZ = 1;	//镜头离得太近的时候会发生这种情况
	}
	return CenterZ * data.zoomFactor / 8;// 半径默认是屏幕1/8
}

const D3DXPLANE& KG3DBaseCoordImp::GetPlaneIntergration()
{
	struct PlaneIntergrationMaker 
	{
		D3DXPLANE plane;
		PlaneIntergrationMaker()
		{
			D3DXVECTOR3 vNormal(1,1,1);
			D3DXVec3Normalize(&vNormal, &vNormal);
			FLOAT fNear = (FLOAT)em_mesh_range / 4;
			D3DXPlaneFromPointNormal(&plane, &D3DXVECTOR3(fNear, 0,0), &vNormal);
		}
	};
	static PlaneIntergrationMaker aInstance;
	return aInstance.plane;
}


KG3DBaseCoordImp::RepresentData::RepresentData()
{
	ZeroMemory(this, sizeof(*this));
	zoomFactor = 1;
}

VOID KG3DBaseCoordImp::RepresentData::ReleaseMeshes()
{
	for(size_t i = 0; i < _countof(MeshForAxis); ++i)
		SAFE_RELEASE(MeshForAxis[i]);
	for(size_t i = 0; i < _countof(MeshForPlane); ++i)
		SAFE_RELEASE(MeshForPlane[i]);
	SAFE_RELEASE(MeshForIntegration);
}

KG3DBaseCoordImp::RepresentData::~RepresentData()
{
	ReleaseMeshes();
}
