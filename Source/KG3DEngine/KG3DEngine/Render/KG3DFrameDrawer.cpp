////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFrameDrawerSolid.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-9-4 10:21:37
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DFrameDrawer.h"
#include "KG3DShaderManager.h"
#include "KG3DDeviceHelpers.h"

////////////////////////////////////////////////////////////////////////////////

namespace
{
	//集中放置Shader的常量
	LPCTSTR strFrameShaderPathSimple = _T("data\\public\\shader\\SimpleColor.fx");
	LPCTSTR strFrameShaderPathRect = _T("data\\public\\shader\\SimpleColor.fx");
	LPCTSTR strFrameShaderPathTri = _T("data\\public\\shader\\SimpleColor.fx");
	LPCTSTR strFrameShaderPathFan = _T("data\\public\\shader\\FanFrame.fx");
	LPCTSTR strFrameShaderPathBox = _T("data\\public\\shader\\BoxFrame.fx");

	const LPTSTR tcsDefaultTechName = _T("DefaultTech");
	const UINT	 uPassCount = 2;
	//用于FrameDrawer的Shader的规定
	struct ShaderRuleChecker 
	{
		static BOOL Check(LPD3DXEFFECT p)
		{
			D3DXHANDLE hTech = p->GetTechniqueByName(tcsDefaultTechName);
			D3DXHANDLE hPass = p->GetPass(hTech, uPassCount - 1);

			return hTech && hPass;
		}
	};
};

class KG3DFrameDrawerBasicSolid : public KG3DFrameDrawerBasic
{
public:
	virtual HRESULT Begin(const D3DXMATRIX* pWorld, const D3DXMATRIX* pView = NULL, const D3DXMATRIX* pProj = NULL, BOOL UseDefaultStates = TRUE);
	virtual HRESULT End();
	virtual HRESULT SetColor(const D3DCOLORVALUE& color);
	virtual HRESULT SetTransform(const D3DXMATRIX* pWorld, const D3DXMATRIX* pView, const D3DXMATRIX* pPerspective);
public:
	KG3DFrameDrawerBasicSolid(LPCTSTR EffectFileName);
	~KG3DFrameDrawerBasicSolid();

	KG3DShaderHolder& GetShaderHolder(){return m_ShaderHolder;}
	
	void SetFVF(DWORD fvf){m_fvf = fvf;}
	BOOL HasBegin(){return m_bHasBegin;}
private:
	VOID LoadEffect(LPCTSTR pFileName);

	KG3DShaderHolder m_ShaderHolder;

	DWORD	m_fvf;

	D3DXHANDLE	m_hColor;
	D3DXHANDLE	m_hWorld;
	D3DXHANDLE	m_hViewProj;

	D3DXMATRIX	m_matView;
	D3DXMATRIX	m_matProj;
	
	BOOL m_bHasBegin;
};

KG3DFrameDrawerBasicSolid::KG3DFrameDrawerBasicSolid( LPCTSTR EffectFileName )
:m_bHasBegin(FALSE)
,m_hWorld(NULL)
,m_hViewProj(NULL)
,m_hColor(NULL)
,m_fvf(D3DFVF_XYZ)
{
	D3DXMatrixIdentity(&m_matView);
	D3DXMatrixIdentity(&m_matProj);
	_ASSERTE(NULL != EffectFileName);
	try
	{
		LoadEffect(EffectFileName);
	}
	catch (...)
	{
		_ASSERTE(NULL && _T("不允许抛出任何异常"));		
	}

}

KG3DFrameDrawerBasicSolid::~KG3DFrameDrawerBasicSolid()
{

}
HRESULT KG3DFrameDrawerBasicSolid::Begin( const D3DXMATRIX* pWorld, const D3DXMATRIX* pView /*= NULL*/
										, const D3DXMATRIX* pProj /*= NULL*/, BOOL UseDefaultStates /*= TRUE*/ )
{
	_ASSERTE(! m_bHasBegin);

	g_pd3dDevice->SetFVF(m_fvf);

	LPD3DXEFFECT p = m_ShaderHolder.GetEffect();
	if(NULL == p)
		return E_FAIL;

	//Begin的时候一切设一个默认参数
	{
		if(NULL == pView)
			g_pd3dDevice->GetTransform(D3DTS_VIEW, &m_matView);
		else
			m_matView = *pView;
		if(NULL == pProj)
			g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &m_matProj);
		else
			m_matProj = *pProj;

		p->SetMatrix(m_hViewProj, &(m_matView * m_matProj));
		p->SetMatrix(m_hWorld, (NULL == pWorld) ? &D3DXMatrixGetIdentity() : pWorld);

		const D3DCOLORVALUE defaultColor = {1,1,1,1};
		p->SetFloatArray(m_hColor, (FLOAT*)&defaultColor, 4);
		p->CommitChanges();
	}

	HRESULT hr = p->Begin(NULL, 0);
	if(SUCCEEDED(hr))
	{
		UINT PassToBegin = UseDefaultStates ? 0 : 1;
		HRESULT hr = p->BeginPass(PassToBegin);
		hr;
		m_bHasBegin = TRUE;
	}			
	return m_bHasBegin ? S_OK : E_FAIL;
}
HRESULT KG3DFrameDrawerBasicSolid::End()
{
	_ASSERTE(m_bHasBegin);

	_ASSERTE(NULL != m_ShaderHolder.GetEffect());
	LPD3DXEFFECT p = m_ShaderHolder.GetEffect();
	p->EndPass();
	p->End();
	m_bHasBegin = FALSE;

	return S_OK;
}


VOID KG3DFrameDrawerBasicSolid::LoadEffect(LPCTSTR EffectFileName)
{
	m_ShaderHolder = g_GetShaderManager().Create(g_pd3dDevice, EffectFileName);
	{
		LPD3DXEFFECT pEffect = m_ShaderHolder.GetEffect();
		if (NULL != pEffect)
		{
			m_hColor = pEffect->GetParameterByName(NULL, "Color");
			m_hWorld = pEffect->GetParameterByName(NULL, "World");
			m_hViewProj = pEffect->GetParameterByName(NULL, "ViewProj");

			BOOL IsValid = ShaderRuleChecker::Check(pEffect);
			if(! IsValid)
			{
				_KG3D_DEBUG_OUTPUT1("以下Effect不符合规定：%s", EffectFileName);
				KGLogPrintf(KGLOG_ERR, "以下Effect不符合规定：%s", EffectFileName);
			}
		}
	}
}

HRESULT KG3DFrameDrawerBasicSolid::SetTransform(const D3DXMATRIX* pWorld, const D3DXMATRIX* pView, const D3DXMATRIX* pPerspective )
{
	LPD3DXEFFECT p = m_ShaderHolder.GetEffect();
	_ASSERTE(NULL != p);

	if(NULL != pView || NULL != pPerspective)
	{
		if(NULL != pView)
			m_matView = *pView;

		if(NULL != pPerspective)
			m_matProj = *pPerspective;

		p->SetMatrix(m_hViewProj, &(m_matView * m_matProj));
	}

	if(NULL != pWorld)
		p->SetMatrix(m_hWorld, pWorld);

	p->CommitChanges();

	return S_OK;
}

HRESULT KG3DFrameDrawerBasicSolid::SetColor( const D3DCOLORVALUE& color )
{
	LPD3DXEFFECT p = m_ShaderHolder.GetEffect();
	_ASSERTE(NULL != p);
	HRESULT hr = p->SetFloatArray(m_hColor, (FLOAT*)&color, 4);
	if (SUCCEEDED(hr))
	{
		p->CommitChanges();
	}
	return hr;
}
//////////////////////////////////////////////////////////////////////////
class KG3DFrameDrawerSolid : public KG3DFrameDrawerBasicSolid, public KG3DFrameDrawer
{
public:
	virtual HRESULT Begin(const D3DXMATRIX* pWorld = NULL, const D3DXMATRIX* pView = NULL, const D3DXMATRIX* pProj = NULL, BOOL UseDefaultStates = TRUE)
	{
		return KG3DFrameDrawerBasicSolid::Begin(pWorld, pView, pProj, UseDefaultStates);
	}
	virtual HRESULT End()
	{
		return KG3DFrameDrawerBasicSolid::End();
	}
	virtual HRESULT SetTransform(const D3DXMATRIX* pWorld, const D3DXMATRIX* pView, const D3DXMATRIX* pPerspective) 
	{
		return KG3DFrameDrawerBasicSolid::SetTransform(pWorld, pView, pPerspective);
	}
	virtual HRESULT SetColor(const D3DCOLORVALUE& color);

	virtual HRESULT Draw(const D3DXVECTOR3& UpperLeft, const D3DXVECTOR3& UpperRight, const D3DXVECTOR3& LowerRight){return E_FAIL;}
	
	virtual VOID	SetLineColor(D3DCOLOR color);
	virtual VOID	SetFrameColor(D3DCOLOR color);
	virtual VOID	SetParameters(BOOL bEnableFace, BOOL bEnableLine, D3DCOLOR FaceColor, D3DCOLOR LineColor);
public:
	KG3DFrameDrawerSolid(LPCTSTR EffectFileName);
	~KG3DFrameDrawerSolid();
	
	struct RenderParams
	{
		D3DCOLORVALUE	m_LineColor;
		D3DCOLORVALUE	m_FrameColor;
		BOOL m_bEnableFrameLine;
		BOOL m_bEnableFrameFace;
		RenderParams(){ZeroMemory(this, sizeof(*this)); m_bEnableFrameFace = TRUE;}
	};
	RenderParams& GetParamters(){return m_Params;}
	
private:
	RenderParams	m_Params;
	
};

KG3DFrameDrawerSolid::KG3DFrameDrawerSolid( LPCTSTR EffectFileName )
:KG3DFrameDrawerBasicSolid(EffectFileName)
{
	SetParameters(TRUE, FALSE, 0xffffffff, 0xffffffff);
}

KG3DFrameDrawerSolid::~KG3DFrameDrawerSolid()
{

}
VOID KG3DFrameDrawerSolid::SetParameters( BOOL bEnableFace, BOOL bEnableLine, D3DCOLOR FaceColor, D3DCOLOR LineColor )
{
	m_Params.m_bEnableFrameFace = bEnableFace;
	if(bEnableFace)
		m_Params.m_FrameColor = Color2Value(FaceColor);
	m_Params.m_bEnableFrameLine = bEnableLine;
	if(bEnableLine)
		m_Params.m_LineColor = Color2Value(LineColor);
}
VOID KG3DFrameDrawerSolid::SetLineColor( D3DCOLOR color )
{
	m_Params.m_LineColor = Color2Value(color);
}

VOID KG3DFrameDrawerSolid::SetFrameColor( D3DCOLOR color )
{
	m_Params.m_FrameColor = Color2Value(color);
}

HRESULT KG3DFrameDrawerSolid::SetColor( const D3DCOLORVALUE& color )
{
	m_Params.m_FrameColor = color;
	m_Params.m_LineColor = color;
	return KG3DFrameDrawerBasicSolid::SetColor(color);
}
//////////////////////////////////////////////////////////////////////////
class KG3DFrameDrawerRectSolid : public KG3DFrameDrawerSolid
{
public:
	virtual HRESULT Draw(const D3DXVECTOR3& UpperLeft, const D3DXVECTOR3& UpperRight, const D3DXVECTOR3& LowerRight);
	KG3DFrameDrawerRectSolid();
};
KG3DFrameDrawerRectSolid::KG3DFrameDrawerRectSolid()
:KG3DFrameDrawerSolid(strFrameShaderPathRect)
{

}
HRESULT KG3DFrameDrawerRectSolid::Draw(const D3DXVECTOR3& UpperLeft, const D3DXVECTOR3& UpperRight, const D3DXVECTOR3& LowerRight)
{
	_ASSERTE(HasBegin());

	D3DXVECTOR3 Buffer[5];
	Buffer[0] = UpperLeft;
	Buffer[1] = UpperRight;
	Buffer[2] = LowerRight;
	Buffer[3] = UpperLeft - UpperRight + LowerRight;

	RenderParams& RP = this->GetParamters();

	if(RP.m_bEnableFrameLine)//不画线的时候不需要第五个点
		Buffer[4] = Buffer[0];

	if(RP.m_bEnableFrameLine)
	{
		KG3DFrameDrawerBasicSolid::SetColor(RP.m_LineColor);	//这个用的是基类的SetColor注意了
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 4, Buffer, sizeof(FLOAT)*3);
	}
	if(RP.m_bEnableFrameFace)
	{
		KG3DFrameDrawerBasicSolid::SetColor(RP.m_FrameColor);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 2, Buffer, sizeof(FLOAT)*3);
	}
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
class KG3DFrameDrawerTriSolid : public KG3DFrameDrawerSolid
{
public:
	virtual HRESULT Draw(const D3DXVECTOR3& UpperLeft, const D3DXVECTOR3& UpperRight, const D3DXVECTOR3& LowerRight);
	KG3DFrameDrawerTriSolid();
};

KG3DFrameDrawerTriSolid::KG3DFrameDrawerTriSolid()
:KG3DFrameDrawerSolid(strFrameShaderPathTri)
{

}
HRESULT KG3DFrameDrawerTriSolid::Draw(const D3DXVECTOR3& UpperLeft, const D3DXVECTOR3& UpperRight, const D3DXVECTOR3& LowerRight)
{
	_ASSERTE(HasBegin());

	D3DXVECTOR3 Buffer[4];
	Buffer[0] = UpperLeft;
	Buffer[1] = UpperRight;
	Buffer[2] = LowerRight;

	RenderParams& RP = this->GetParamters();

	if(RP.m_bEnableFrameLine)//不画线的时候不需要第五个点
		Buffer[3] = Buffer[0];

	if(RP.m_bEnableFrameLine)
	{
		KG3DFrameDrawerBasicSolid::SetColor(RP.m_LineColor);	//这个用的是基类的SetColor注意了
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_LINESTRIP, 3, Buffer, sizeof(FLOAT)*3);
	}

	if(RP.m_bEnableFrameFace)
	{
		KG3DFrameDrawerBasicSolid::SetColor(RP.m_FrameColor);
		g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLEFAN, 1, Buffer, sizeof(FLOAT)*3);
	}
	return S_OK;
}
//////////////////////////////////////////////////////////////////////////
class KG3DFrameDrawerFanSolid : public KG3DFrameDrawerFan	//用聚合的方式完成这个类。因为扇形是特殊的圆锥扇形（高为0），而圆是特殊的扇形，所以都可以一起实现
{
public:
	virtual HRESULT Begin(const D3DXMATRIX* pWorld = NULL, const D3DXMATRIX* pView = NULL
							, const D3DXMATRIX* pProj = NULL, BOOL UseDefaultStates = TRUE);
	virtual HRESULT End(){return m_DrawerSolid.End();}
	virtual HRESULT SetTransform(const D3DXMATRIX* pWorld, const D3DXMATRIX* pView, const D3DXMATRIX* pPerspective)
	{
		return m_DrawerSolid.SetTransform(pWorld, pView,pPerspective);
	}
	virtual HRESULT SetColor(const D3DCOLORVALUE& color){return m_DrawerSolid.SetColor(color);}

	virtual VOID SetFrameColor(D3DCOLOR color){m_DrawerSolid.SetFrameColor(color);}
	virtual VOID SetLineColor(D3DCOLOR color){m_DrawerSolid.SetLineColor(color);}
	virtual VOID SetParameters(BOOL bEnableFace, BOOL bEnableLine, D3DCOLOR FaceColor, D3DCOLOR LineColor)
	{
		return m_DrawerSolid.SetParameters(bEnableFace, bEnableLine, FaceColor, LineColor);
	}

	virtual HRESULT Draw(const D3DXVECTOR3& UpperLeft, const D3DXVECTOR3& UpperRight, const D3DXVECTOR3& LowerRight){return E_FAIL;}
	
	
	virtual HRESULT DrawCone(const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, FLOAT fBottomRadius, FLOAT fHeight);
	virtual HRESULT DrawFan(const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, const D3DXVECTOR3& Begin, FLOAT fAngle);
	virtual HRESULT DrawCircle(const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, FLOAT fAngle);

	
	KG3DFrameDrawerFanSolid();
	~KG3DFrameDrawerFanSolid();
private:
	HRESULT DrawCoreFunc(const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, const D3DXVECTOR3& Begin, FLOAT fHeight, FLOAT fAngle);
	LPDIRECT3DVERTEXBUFFER9	m_pVB;
	struct TypeVertex 
	{
		FLOAT		radiusPercent;
		FLOAT		anglePercent;
		FLOAT		useless;
	};
	enum{em_fvf = D3DFVF_XYZ};
	enum
	{
		em_segment_in_circle = 64,//越多越圆
		em_point_count = 1 + em_segment_in_circle + 1,//中心点还需要一个点，而每段的段尾还需要一个点
	};
	KG3DFrameDrawerSolid	m_DrawerSolid;
	D3DXHANDLE		m_hNormalCenterBegin;
	D3DXHANDLE		m_hAngle;
	D3DXHANDLE		m_hConeHeight;
};

KG3DFrameDrawerFanSolid::KG3DFrameDrawerFanSolid()
	:m_DrawerSolid(strFrameShaderPathFan)
	,m_pVB(NULL)
	,m_hNormalCenterBegin(NULL)
	,m_hAngle(NULL)
{
	try
	{
		_ASSERTE(NULL != g_pd3dDevice);
		UINT uVBLength = sizeof(TypeVertex) * em_point_count;
		HRESULT hr = g_pd3dDevice->CreateVertexBuffer(uVBLength, 0, em_fvf, D3DPOOL_MANAGED, &m_pVB, NULL);

		if (SUCCEEDED(hr))
		{
			TypeVertex* pVertices = NULL;
			hr = m_pVB->Lock(0, uVBLength, (LPVOID*)&pVertices, 0);
			if (SUCCEEDED(hr))
			{
				{
					TypeVertex Temp = {0, 0, 0};//中心点
					pVertices[0] = Temp;
				}
				UINT circlePointBeginIndex = 1;
				for (UINT i = circlePointBeginIndex; i < 1 + em_segment_in_circle + 1; ++i)
				{
					TypeVertex Temp = {1, (float)(i - circlePointBeginIndex)/(float)em_segment_in_circle, 0};
					pVertices[i] = Temp;
				}

				m_pVB->Unlock();
			}
		}

		LPD3DXEFFECT p = m_DrawerSolid.GetShaderHolder().GetEffect();
		_ASSERTE(NULL != p);

		m_hNormalCenterBegin = p->GetParameterByName(NULL, "NormalCenterBegin");
		m_hAngle = p->GetParameterByName(NULL, "Angle");
		m_hConeHeight = p->GetParameterByName(NULL, "ConeHeight");
	}
	catch (...)
	{
		_ASSERTE(NULL && _T("不能在这里抛出异常"));		
	}
}

HRESULT KG3DFrameDrawerFanSolid::DrawCoreFunc( const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, const D3DXVECTOR3& Begin, FLOAT fHeight, FLOAT fAngle )
{
	KG_PROCESS_ERROR(NULL != m_pVB);
	{
		LPD3DXEFFECT pEffect = m_DrawerSolid.GetShaderHolder().GetEffect();
		_ASSERTE(NULL != pEffect);

		D3DXVECTOR3 Array[] = {vNormal, vCenter, Begin};
		HRESULT hr = pEffect->SetFloatArray(m_hNormalCenterBegin, reinterpret_cast<const FLOAT*>(Array), _countof(Array) * 3);
		{
			FLOAT fAngleUse = fAngle;
			MT_LIMIT_TO_RANGE(fAngleUse, 2 * D3DX_PI, -2 * D3DX_PI);
			hr = pEffect->SetFloat(m_hAngle, fAngleUse);
		}
		hr = pEffect->SetFloat(m_hConeHeight, fHeight);

		KG3DFrameDrawerSolid::RenderParams& RP = m_DrawerSolid.GetParamters();
		if (RP.m_bEnableFrameLine)
		{
			m_DrawerSolid.KG3DFrameDrawerBasicSolid::SetColor(RP.m_LineColor);
			hr = g_pd3dDevice->DrawPrimitive(D3DPT_LINESTRIP, 1, em_segment_in_circle);
		}

		if (RP.m_bEnableFrameFace)
		{
			m_DrawerSolid.KG3DFrameDrawerBasicSolid::SetColor(RP.m_FrameColor);
			hr = g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, em_segment_in_circle);
		}

		return S_OK;
	}
Exit0:
	return E_FAIL;
}
HRESULT KG3DFrameDrawerFanSolid::DrawCone(const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, FLOAT fBottomRadius, FLOAT fHeight)
{
	/*
	因为是圆，所以Begin随便在哪都可以。那么直接找到和Normal垂直的一个矢量，然后单位化，再乘以半径就可以得到Begin点了
	*/
	D3DXVECTOR3 vBegin;
	BOOL IsNormalVertical = abs(vNormal.y - 1) < FLT_EPSILON;
	if(IsNormalVertical)
	{
		vBegin = fBottomRadius * D3DXVECTOR3(1,0,0) + vCenter;
	}
	else
	{
		D3DXVECTOR3 vTemp(-vNormal.z, 0, vNormal.x);	//这个矢量和Normal点乘的话，得到0，证明它一定会和Normal垂直
		D3DXVec3Normalize(&vTemp, &vTemp);
		vBegin = fBottomRadius * vTemp + vCenter;
	}
	return DrawCoreFunc(vCenter, vNormal, vBegin, fHeight, 2* D3DX_PI);
}
HRESULT KG3DFrameDrawerFanSolid::DrawFan(const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, const D3DXVECTOR3& Begin, FLOAT fAngle)
{
	return DrawCoreFunc(vCenter, vNormal, Begin, 0, fAngle);
}

HRESULT KG3DFrameDrawerFanSolid::DrawCircle(const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, FLOAT Radius)
{
	return DrawCone(vCenter, vNormal, Radius, 0);	//圆是特殊的圆锥
}

KG3DFrameDrawerFanSolid::~KG3DFrameDrawerFanSolid()
{
	SAFE_RELEASE(m_pVB);
}


HRESULT KG3DFrameDrawerFanSolid::Begin( const D3DXMATRIX* pWorld /*= NULL */, const D3DXMATRIX* pView /*= NULL */, const D3DXMATRIX* pProj /*= NULL */, BOOL UseDefaultStates /*= TRUE*/ )
{
	if(NULL == m_pVB)
		return E_FAIL;
	m_DrawerSolid.SetFVF(em_fvf);
	g_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(TypeVertex));
	return m_DrawerSolid.Begin(pWorld, pView, pProj, UseDefaultStates);
}


//////////////////////////////////////////////////////////////////////////
class KG3DFrameDrawerBoxSolid : public KG3DFrameDrawerBox
{
public:
	virtual HRESULT Begin(const D3DXMATRIX* pWorld = NULL, const D3DXMATRIX* pView = NULL
		, const D3DXMATRIX* pProj = NULL, BOOL UseDefaultStates = TRUE);

	virtual HRESULT End(){return m_DrawerSolid.End();}
	virtual HRESULT SetTransform(const D3DXMATRIX* pWorld, const D3DXMATRIX* pView, const D3DXMATRIX* pPerspective)
	{
		return m_DrawerSolid.SetTransform(pWorld, pView, pPerspective);
	}
	virtual HRESULT SetColor(const D3DCOLORVALUE& color){return m_DrawerSolid.SetColor(color);}

	virtual VOID	SetLineColor(D3DCOLOR color){return m_DrawerSolid.SetLineColor(color);}
	virtual VOID	SetFrameColor(D3DCOLOR color){return m_DrawerSolid.SetFrameColor(color);}
	virtual VOID	SetParameters(BOOL bEnableFace, BOOL bEnableLine, D3DCOLOR FaceColor, D3DCOLOR LineColor)
	{
		return m_DrawerSolid.SetParameters(bEnableFace, bEnableLine, FaceColor, LineColor);
	}
	virtual HRESULT Draw(const D3DXVECTOR3& UpperLeft, const D3DXVECTOR3& UpperRight, const D3DXVECTOR3& LowerRight){return E_FAIL;}


	virtual HRESULT SetAxis(const D3DXVECTOR3& vX, const D3DXVECTOR3& vZ);
	virtual HRESULT SetRange(const D3DXVECTOR3& vRangeXYZ);
	virtual HRESULT DrawBox(const D3DXVECTOR3& Center);
	

	KG3DFrameDrawerBoxSolid();
	~KG3DFrameDrawerBoxSolid();
private:
	enum{
		em_fvf = D3DFVF_XYZ,
		em_point_count = 8,
		em_ib_face_count = 22,
		//em_ib_face_count = 48,
		em_ib_line_count = 18,

		em_ib_count = em_ib_face_count + em_ib_line_count,
		em_face_count = em_ib_face_count - 2,	//用TriangleStrip
		//em_face_count = em_ib_face_count / 3,	//用TriangleList
		em_line_count = em_ib_line_count - 1,	//用LineStrip
	};
	KG3DFrameDrawerSolid m_DrawerSolid;
	LPDIRECT3DVERTEXBUFFER9 m_pVB;
	LPDIRECT3DINDEXBUFFER9 m_pIB;
	D3DXHANDLE m_hXVecZVec;
	D3DXHANDLE m_hCenter;
	D3DXHANDLE m_hLengthXYZ;
};

HRESULT KG3DFrameDrawerBoxSolid::Begin( const D3DXMATRIX* pWorld /*= NULL*/, const D3DXMATRIX* pView /*= NULL */, const D3DXMATRIX* pProj /*= NULL*/, BOOL UseDefaultStates /*= TRUE*/ )
{
	if(NULL == m_pVB)
		return E_FAIL;
	m_DrawerSolid.SetFVF(D3DFVF_XYZ);
	g_pd3dDevice->SetStreamSource(0, m_pVB, 0, sizeof(D3DXVECTOR3));
	g_pd3dDevice->SetIndices(m_pIB);
	return m_DrawerSolid.Begin(pWorld, pView, pProj);
}

KG3DFrameDrawerBoxSolid::KG3DFrameDrawerBoxSolid()
	:m_DrawerSolid(strFrameShaderPathBox)
	,m_pVB(NULL)
	,m_hXVecZVec(NULL)
	,m_hCenter(NULL)
	,m_hLengthXYZ(NULL)
{
	try
	{
		_ASSERTE(NULL != g_pd3dDevice);
		HRESULT hr = E_FAIL;

		{
			D3DVECTOR Buffer [] = {
				{-0.5f, -0.5f, -0.5f},	//围着一圈建立顶点
				{-0.5f, 0.5f, -0.5f},
				{0.5f, -0.5f, -0.5f},
				{0.5f, 0.5f, -0.5f},

				{0.5f, -0.5f, 0.5f},
				{0.5f, 0.5f, 0.5f},
				{-0.5f, -0.5f, 0.5f},
				{-0.5f, 0.5f, 0.5f},
			};

			hr = D3DXDeviceCreateVertexBufferByData(Buffer, sizeof(D3DVECTOR), _countof(Buffer)
				, sizeof(D3DXVECTOR3), 0, em_fvf, D3DPOOL_MANAGED, g_pd3dDevice, &m_pVB, NULL);
		}

		{
			struct  IBBuffer 
			{
				WORD FaceBuffer[em_ib_face_count];
				WORD LineBuffer[em_ib_line_count];
			};

			WORD FaceBuffer[] = {4,6,2,0,3,1,3,3,2,3,4,5,6,7,0,1,1,7,3,5};
			//WORD FaceBuffer[] = {0,1,3,	0,3,2,	2,3,5,	2,5,4,	4,5,7,	4,7,6,	6,7,1,	6,1,0,	1,7,5,	1,5,3,	0,2,4,	0,4,6};
 			WORD LineBuffer[] = {0,1,3,2,0,6,4,5,7,6,0,1,7,5,3,2,4,};

			IBBuffer bufferTemp;
			_ASSERTE(_countof(FaceBuffer) <= _countof(bufferTemp.FaceBuffer) && _countof(LineBuffer) < _countof(bufferTemp.LineBuffer));

			//把两个Buffer连起来，最后没有填的部分填成最后一个值，画的时候会被忽略掉
			
			memcpy_s(bufferTemp.FaceBuffer, sizeof(bufferTemp.FaceBuffer), FaceBuffer, sizeof(FaceBuffer));
			memcpy_s(bufferTemp.LineBuffer, sizeof(bufferTemp.LineBuffer), LineBuffer, sizeof(LineBuffer));

			for (size_t i = _countof(FaceBuffer); i < _countof(bufferTemp.FaceBuffer); ++i)
			{
				bufferTemp.FaceBuffer[i] = bufferTemp.FaceBuffer[_countof(FaceBuffer) - 1];
			}
			for (size_t i = _countof(LineBuffer); i < _countof(bufferTemp.LineBuffer); ++i)
			{
				bufferTemp.LineBuffer[i] = bufferTemp.LineBuffer[_countof(LineBuffer) - 1];
			}

			hr = D3DXDeviceCreateIndexBufferByData(&bufferTemp, em_ib_count, 0, FALSE, D3DPOOL_MANAGED, g_pd3dDevice, &m_pIB, NULL);
		}
		
		{
			LPD3DXEFFECT p = m_DrawerSolid.GetShaderHolder().GetEffect();
			_ASSERTE(NULL != p);

			m_hCenter = p->GetParameterByName(NULL, "Center");
			m_hLengthXYZ = p->GetParameterByName(NULL, "LengthXYZ");
			m_hXVecZVec = p->GetParameterByName(NULL, "XVecZVec");
		}
		return;
	}
	catch (...)
	{
		_ASSERTE(NULL && _T("不能在这里抛出异常"));		
	}
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
}

KG3DFrameDrawerBoxSolid::~KG3DFrameDrawerBoxSolid()
{
	SAFE_RELEASE(m_pVB);
	SAFE_RELEASE(m_pIB);
}

HRESULT KG3DFrameDrawerBoxSolid::SetAxis( const D3DXVECTOR3& vX, const D3DXVECTOR3& vZ )
{
	LPD3DXEFFECT p = m_DrawerSolid.GetShaderHolder().GetEffect();
	_ASSERTE(NULL != p);
	D3DXVECTOR3 XVecZVec[2] = {vX, vZ};
	return p->SetFloatArray(m_hXVecZVec, (FLOAT*)XVecZVec, sizeof(XVecZVec)/sizeof(FLOAT));
}

HRESULT KG3DFrameDrawerBoxSolid::SetRange(const D3DXVECTOR3& vRangeXYZ)
{
	LPD3DXEFFECT p = m_DrawerSolid.GetShaderHolder().GetEffect();
	_ASSERTE(NULL != p);
	return p->SetFloatArray(m_hLengthXYZ, (const FLOAT*)vRangeXYZ, sizeof(D3DXVECTOR3)/sizeof(FLOAT));
}

HRESULT KG3DFrameDrawerBoxSolid::DrawBox( const D3DXVECTOR3& Center )
{
	LPD3DXEFFECT p = m_DrawerSolid.GetShaderHolder().GetEffect();
	_ASSERTE(NULL != p);
	p->SetFloatArray(m_hCenter, (const FLOAT*)Center, sizeof(D3DXVECTOR3)/sizeof(FLOAT));

	KG3DFrameDrawerSolid::RenderParams& params = m_DrawerSolid.GetParamters();
	if (params.m_bEnableFrameFace)
	{
		m_DrawerSolid.KG3DFrameDrawerBasicSolid::SetColor(params.m_FrameColor);
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, em_point_count, 0, em_face_count);
	}
	if (params.m_bEnableFrameLine)
	{
		m_DrawerSolid.KG3DFrameDrawerBasicSolid::SetColor(params.m_LineColor);
		g_pd3dDevice->DrawIndexedPrimitive(D3DPT_LINESTRIP, 0, 0, em_point_count, em_ib_face_count, em_line_count);
	}
	
	return S_OK;
}
extern KG3DFrameDrawer& g_GetFrameDrawerRect()
{
	static KG3DFrameDrawerRectSolid stDrawer;
	return stDrawer;
}
extern KG3DFrameDrawer& g_GetFrameDrawerTri()
{
	static KG3DFrameDrawerTriSolid stDrawer;
	return stDrawer;
}

extern KG3DFrameDrawerFan& g_GetFrameDrawerFan()
{
	static KG3DFrameDrawerFanSolid stDrawer;
	return stDrawer;
}


struct KG3DFrameDrawerSimple : public KG3DFrameDrawerBasicSolid
{
	KG3DFrameDrawerSimple():KG3DFrameDrawerBasicSolid(strFrameShaderPathSimple){}
};
extern KG3DFrameDrawerBasic& g_GetFrameDrawerBasic()
{
	static KG3DFrameDrawerSimple stInstance;
	return stInstance;
}

extern KG3DFrameDrawerCone& g_GetFrameDrawerCone()
{
	return g_GetFrameDrawerFan();
}

extern KG3DFrameDrawerBox& g_GetFrameDrawerBox()
{
	static KG3DFrameDrawerBoxSolid instance;
	return instance;
}

extern VOID g_CheckFrameDrawers( const D3DXVECTOR3& vPos, FLOAT fRange )
{
#if 1
	{
		KG3DFrameDrawer& Drawer = g_GetFrameDrawerRect();
		Drawer.SetParameters(TRUE, TRUE, 0x500ff000, 0xffff0000);
		if(SUCCEEDED(Drawer.Begin()))
		{			
			Drawer.Draw(vPos + D3DXVECTOR3(-fRange, 0, fRange)
				, vPos + D3DXVECTOR3(fRange, 0, fRange), 
				vPos + D3DXVECTOR3(fRange, 0, -fRange));
			Drawer.End();
		}
	}
#endif
#if 1
	{
		KG3DFrameDrawer& Drawer = g_GetFrameDrawerTri();
		Drawer.SetParameters(TRUE, TRUE, 0x50000000, 0xff000000);
		if (SUCCEEDED(Drawer.Begin()))
		{
			Drawer.Draw(vPos + D3DXVECTOR3(-fRange, 100, fRange)
				, vPos + D3DXVECTOR3(fRange, 100, fRange), 
				vPos + D3DXVECTOR3(fRange, 100, -fRange));
			Drawer.End();
		}
	}
#endif
#if 1
	{
		KG3DFrameDrawerBox& Drawer = g_GetFrameDrawerBox();
		Drawer.SetParameters(TRUE, TRUE, 0x500000ff, 0xff000000);
		Drawer.SetAxis(D3DXVECTOR3(1,0,0), D3DXVECTOR3(0,0,1));
		Drawer.SetRange(D3DXVECTOR3(fRange,fRange,fRange));
		if (SUCCEEDED(Drawer.Begin()))
		{
			Drawer.DrawBox(vPos);
			Drawer.End();
		}
	}
#endif
}