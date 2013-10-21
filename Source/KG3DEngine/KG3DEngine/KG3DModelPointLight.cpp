#include "StdAfx.h"
#include "KG3DModelPointLight.h"
#include "KG3DGraphicstool.h"
#include "KG3DSFX.h"
#include "KG3DModelTable.h"

KG3DSFX* KG3DModelPointLight::m_spSign = NULL;
BOOL g_bShowPointLightSign = FALSE;

HRESULT KG3DModelPointLight::StaticInit()
{
    if (!m_spSign)
        g_cModelTable.LoadResourceFromFile(TEXT("\\data\\public\\SceneEditor\\point_light.Sfx"), 0, MLO_MULTI, (IKG3DResourceBase**)&m_spSign);

    return S_OK;
}

HRESULT KG3DModelPointLight::StaticUnInit()
{
    if (m_spSign)
        SAFE_RELEASE(m_spSign);

    return S_OK;
}

KG3DModelPointLight::KG3DModelPointLight(void)
{
	m_vColor.r = 1.f;
    m_vColor.g = 1.f;
    m_vColor.b = 1.f;
    m_vColor.a = 5.f;

	m_bEnable = TRUE;
	m_lpCubeTexture = NULL;

	m_eType = LIGHTTYPE_POINT;
}

KG3DModelPointLight::~KG3DModelPointLight(void)
{
	SAFE_RELEASE(m_lpCubeTexture);
}

HRESULT KG3DModelPointLight::SetModelColor(DWORD dwSubset, D3DCOLORVALUE color)
{
    m_vColor = color;
    m_vColor.a = m_vColor.a * 50.f;
    return S_OK;
}

HRESULT KG3DModelPointLight::GetModelColor(DWORD dwSubset, D3DCOLORVALUE* color)
{
    (*color) = m_vColor;
    (*color).a = (*color).a / 50.f;
    return S_OK;
}

HRESULT	KG3DModelPointLight::RenderSelectMark(DWORD dwColor)
{
	if(m_eType == LIGHTTYPE_POINT)
	{
		D3DXVECTOR3 center = m_BBox.GetCenter();
		float fRadius = m_BBox.GetDiameter() * 0.5f;
		D3DXVECTOR3 right = g_cGraphicsTool.GetCamera()->GetCameraRight();
		D3DXVECTOR3 up = g_cGraphicsTool.GetCamera()->GetCameraUp();
		D3DXVECTOR3 front = g_cGraphicsTool.GetCamera()->GetCameraFront();

		KG3DRenderState Rs;
		Rs.SetRenderState(D3DRS_ZENABLE, TRUE);

		g_cGraphicsTool.DrawCircle(center, up, fRadius, dwColor);
		g_cGraphicsTool.DrawCircle(center, D3DXVECTOR3(0.f, 1.f, 0.f), fRadius, dwColor);
		g_cGraphicsTool.DrawCircle(center, front, fRadius, dwColor);
		g_cGraphicsTool.DrawCircle(center, right, fRadius, dwColor);

		float s = timeGetTime() * 0.03F;
		D3DXVECTOR3 v(sinf(s),0,cosf(s));
		g_cGraphicsTool.DrawLine(&(center-v*fRadius), &(center+v*fRadius), dwColor, dwColor);
	}
	else if(m_eType == LIGHTTYPE_DIRECTION)
	{
		float Width = m_Scaling.x;
		float Height = m_Scaling.z;

		//g_cGraphicsTool.DrawRect()

	}

    return S_OK;
}

BOOL KG3DModelPointLight::IsResourceReady(BOOL bTestTexture)
{
    return IsLoaded();
}

HRESULT KG3DModelPointLight::LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption)
{
	m_scName = cszFileName;
	m_bMeshPostLoadProcessed = TRUE;
	ComputeBBox();
	return S_OK;
}

HRESULT KG3DModelPointLight::SaveToFile(const char cszFileName[], unsigned uOption)
{
	return S_OK;
}


HRESULT KG3DModelPointLight::Enable(BOOL bEnable)
{
	m_bEnable = bEnable;
	return S_OK;
}

HRESULT KG3DModelPointLight::Render(unsigned uOption, void* pExtInfo)
{
	D3DXVECTOR3 pos = GetPositionWorld();

    if (g_bShowPointLightSign && m_spSign)
    {
        m_spSign->SetTranslation(&pos);
        m_spSign->FrameMove();
        m_spSign->Render(0, 0);
    }
	/*{
		D3DCOLORVALUE c;
		c.a = c.r = c.g = c.b = 1;
		D3DXMATRIX mat;
		D3DXMatrixTranslation(&mat,pos.x,pos.y,pos.z);
		g_cGraphicsTool.DrawSphere(&mat,c,100);
	}*/
	return S_OK;
}

HRESULT KG3DModelPointLight::GetMeshFileName(LPTSTR strFileName)
{
	wsprintf(strFileName,"%s",m_scName.c_str());
	return S_OK;
}

HRESULT KG3DModelPointLight::ComputeBBox()
{
	m_BBoxLocal.Clear();

    m_BBoxLocal.AddPosition(D3DXVECTOR3(-500,-500,-500));
    m_BBoxLocal.AddPosition(D3DXVECTOR3( 500, 500, 500));
	
    return KG3DModel::ComputeBBox();
}

D3DXVECTOR3 KG3DModelPointLight::GetPositionWorld()
{
	D3DXVECTOR3 vPos;
	vPos.x = m_matWorld._41;
	vPos.y = m_matWorld._42;
	vPos.z = m_matWorld._43;
	return vPos;
}

BOOL KG3DModelPointLight::RayIntersection(const D3DXVECTOR3& vSrc , const D3DXVECTOR3& vNormalizedDir, FLOAT* pfRet ,D3DXVECTOR3* pFaceNormal , BOOL bIntersectBoxOnly , KG3DIntersectionFilter* pFilter /* = NULL */)
{
	D3DXVECTOR3 vPos = GetPositionWorld();
	D3DXVECTOR3 vInter;

	BOOL bInter = IsLineIntersectSphere(&vInter,&vPos,100,(D3DXVECTOR3*)&vSrc,&(vSrc+vNormalizedDir*100000));
	if(bInter)
	{
		(*pfRet) = D3DXVec3Length(&(vInter - m_Translation));
	}

	return bInter;
}

HRESULT KG3DModelPointLight::GetExtData(KG3DModelLightExtData& vData)
{
	vData.vColor = m_vColor;
	vData.nType = (int)m_eType;

	return S_OK;
}

HRESULT KG3DModelPointLight::SetExtData(KG3DModelLightExtData& vData)
{
	m_vColor = vData.vColor;
	m_eType = (KG3DLightType)(vData.nType);

	return S_OK;
}