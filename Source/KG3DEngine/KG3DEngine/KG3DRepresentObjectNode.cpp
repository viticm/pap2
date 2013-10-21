#include "StdAfx.h"
#include "KG3DRepresentObjectNode.h"
#include "KG3DTerrainRoad.h"
#include "KG3DGraphicsTool.h"
#include "Terrain/KG3DTerrainRiver.h"
#include "KG3DRepresentObjectWall.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DRepresentObjectNode::KG3DRepresentObjectNode(void)
{
	//m_lpTerrainRoad = NULL;
	//m_lpTerrainRoadNode = NULL;
	m_fNodeRadius = 10;
	m_pPointer = NULL;
	m_dwNodeType = NODETYPE_NONE;

	m_lpWall = NULL;
	m_nWallIndex = 0;
	m_nPointIndex = 0;
}

KG3DRepresentObjectNode::~KG3DRepresentObjectNode(void)
{
	//m_lpTerrainRoad = NULL;
	//m_lpTerrainRoadNode = NULL;
}

HRESULT KG3DRepresentObjectNode::SetTranslation(const D3DXVECTOR3* pValue)
{
	KG3DTransformation::SetTranslation(pValue);
	ComputeBBox();
	if(!m_pPointer)
		return E_FAIL;
	if(m_dwNodeType == NODETYPE_ROAD)
	{
		static_cast<KG3DTerrainRoad*>(m_pPointer)->ComputePassages(this);	
	}
	else if (m_dwNodeType == NODETYPE_RIVER)
	{
		static_cast<KG3DTerrainRiver*>(m_pPointer)->ComputePassages(this);
	}
	return S_OK;
}

HRESULT KG3DRepresentObjectNode::SetScaling(const D3DXVECTOR3 *pValue)
{
	KG3DTransformation::SetScaling(pValue);
	ComputeBBox();
	return S_OK;
}

HRESULT KG3DRepresentObjectNode::OnDeleteFromScene(KG3DSceneSceneEditor* pEditor)
{
	if(!m_pPointer)
		return E_FAIL;

	if(m_dwNodeType == NODETYPE_ROAD)
	{
		static_cast<KG3DTerrainRoad*>(m_pPointer)->DeleteNode(this);	
		//static_cast<KG3DTerrainRoad*>(m_pPointer)->ComputeRoadParts();	
		//static_cast<KG3DTerrainRoad*>(m_pPointer)->CreateRoad();	
		
	}
	else if (m_dwNodeType == NODETYPE_RIVER)
	{
		static_cast<KG3DTerrainRiver*>(m_pPointer)->DeleteNode(this);	
	//	static_cast<KG3DTerrainRiver*>(m_pPointer)->ComputePassages();
	//	static_cast<KG3DTerrainRiver*>(m_pPointer)->CreateRiver();
	}
	m_pPointer = NULL;
	return S_OK;
}

HRESULT KG3DRepresentObjectNode::Render(DWORD dwOption)
{
	g_pd3dDevice->SetTexture(0,NULL);

	D3DCOLOR color2 = 0xff3080cc;
	D3DCOLORVALUE color;
	color.a = 1.0f;color.r=0.0f;color.g = 0.0f;color.b = 0.0f;
	
	switch(m_dwNodeType)
	{
	case NODETYPE_ROAD: 
			color.a = 1.0f;color.r=0.5f;color.g = 0.5f;color.b = 0.5f;
			color2 = 0xff5533cc;
			break;
	case NODETYPE_RIVER:
			color.a = 1.0f;color.r=0.2f;color.g = 0.5f;color.b = 0.8f;
			color2 = 0xff3080cc;
			break;
	}
	
	//GraphicsStruct::RenderStateAlpha Alpha(0x20,TRUE,TRUE,D3DBLEND_ZERO,D3DBLEND_SRCCOLOR);
	GraphicsStruct::RenderState Fog(D3DRS_FOGENABLE, FALSE);
	GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_CCW);
	g_cGraphicsTool.DrawSphereEx(&m_Matrix,color,m_fNodeRadius*0.3f,TRUE);
	g_cGraphicsTool.DrawAABBox(&m_BBox,color2);


	return S_OK;
}

BOOL KG3DRepresentObjectNode::IsRayIntersect(D3DXVECTOR3&Inter,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	D3DXVECTOR3 vInter;
//	float K = PointDistanceLine(&vInter,&Origin,&(Origin+Direction));

	//if(K < m_fNodeRadius * m_Scaling.y)
	//{
	//	Inter = vInter;
	//	return TRUE;
	//}
	if (m_BBox.IsRayIntersect3D(vInter,Origin,Direction,NULL,NULL))
	{
		Inter = vInter;
		return TRUE;
	}
	return FALSE;
}

HRESULT KG3DRepresentObjectNode::ComputeBBox()
{

	D3DXVECTOR3 V(m_fNodeRadius * m_Scaling.y,0,m_fNodeRadius * m_Scaling.y);
	D3DXVECTOR3 V1(m_fNodeRadius * m_Scaling.y,m_fNodeRadius * m_Scaling.y,m_fNodeRadius * m_Scaling.y);

	m_BBox.A = m_Translation - V;
	m_BBox.B = m_Translation + V1;
	return S_OK;
}

HRESULT KG3DRepresentObjectNode::ComputeDynamicBBox()
{
	return ComputeBBox();
}

HRESULT KG3DRepresentObjectNode::Refresh()
{
	if(m_pPointer && m_dwNodeType == NODETYPE_ROAD)
	{
		static_cast<KG3DTerrainRoad*>(m_pPointer)->ComputePassages();
		static_cast<KG3DTerrainRoad*>(m_pPointer)->Create();//CreatePassages(this);
	}
	else if (m_pPointer&& m_dwNodeType == NODETYPE_RIVER)
	{
		static_cast<KG3DTerrainRiver*>(m_pPointer)->ComputePassages();
		static_cast<KG3DTerrainRiver*>(m_pPointer)->Create();//CreatePassages(this);
	}
	else if(m_dwNodeType == NODETYPE_WALLKEYPOINT)
	{
		if(m_lpWall)
		{
			m_lpWall->ModifyWallKeyPoint(m_lpWall,m_nWallIndex,m_nPointIndex,m_Translation);
		}
	}
	return S_OK;
}

void KG3DRepresentObjectNode::SetNodeType(DWORD dwType)
{
	m_dwNodeType = dwType;
}
DWORD  KG3DRepresentObjectNode::GetNodeType()
{
	return m_dwNodeType;
}
HRESULT KG3DRepresentObjectNode::SetPointer(PVOID pPointer)
{
	m_pPointer = pPointer;
	return S_OK;
}

HRESULT KG3DRepresentObjectNode::GetPointer(PVOID* ppPointer)
{
	HRESULT hr= E_FAIL;
	KG_PROCESS_ERROR(ppPointer);
	*ppPointer = m_pPointer;
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DRepresentObjectNode::SetWall(KG3DRepresentObjectWall* pWall,int nWallIndex,int nPointIndex)
{
	m_lpWall = pWall;
	m_nWallIndex = nWallIndex;
	m_nPointIndex = nPointIndex;
	return S_OK;
}

HRESULT KG3DRepresentObjectNode::GetWall(KG3DRepresentObjectWall*& pWall,int& nWallIndex,int& nPointIndex)
{
	pWall = m_lpWall;
	nWallIndex  = m_nWallIndex;
	nPointIndex = m_nPointIndex;
	return S_OK;
}

HRESULT KG3DRepresentObjectNode::Copy(KG3DRepresentObject* pSrcObject)
{
	KG3DRepresentObjectNode* pSrc = static_cast<KG3DRepresentObjectNode*>(pSrcObject);

	m_fNodeRadius = pSrc->m_fNodeRadius;
	m_dwNodeType = pSrc->m_dwNodeType;
	m_pPointer = pSrc->m_pPointer;

	m_lpWall = pSrc->m_lpWall;
	m_nWallIndex = pSrc->m_nWallIndex;
	m_nPointIndex = pSrc->m_nPointIndex;

	return KG3DRepresentObject::Copy(pSrcObject);
}
