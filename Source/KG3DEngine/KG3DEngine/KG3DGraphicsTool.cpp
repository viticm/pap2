#include "StdAfx.h"
#include "KG3DGraphicsTool.h"
#include "KG3DGraphiceEngine.h"
#include "KG3DFrustum.h"
#include "KG3DTexture.h"
#include "KG3DTextureTable.h"
#include "KG3DScene.h"

//#include "..\..\..\include\SO3World\KCell.h"	//<SO3World Refactor>
//#include "..\..\..\include\SO3World\KRegion.h"
//#include "..\..\..\include\SO3World\KScene.h"

#include "KG3DTexture.h"
#include "KGLog.h"
#include "KG3DModelST.h"
#include "KG3DBip.h"
#include "KG3DRenderState.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DRegionInfoManager.h"
#include "KG3DEnvironment.h"
#include "KG3DEnvEffFog.h"
#include "KG3DEnvEffLightSet.h"
#include "KG3DSceneBlock.h"
#include "KG3DDefaultShaderGroup.h"
#include "KG3DShaderHolder.h"
#include "KG3DRenderTargetTool.h"

#include "MathTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#ifndef max
#define max(a,b)            (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b)            (((a) < (b)) ? (a) : (b))
#endif

KG3DGraphicsTool g_cGraphicsTool;

const char* g_strErrorTexture = "Data\\public\\errortexture.tga";

int KG3DGraphicsTool::m_nMaxNumBone;

#define	MAX_LOGIC_TERRIAN_CELL_IN_REGION 4096
#define	MAX_LOGIC_ITEM_CELL_IN_REGION 4096 * 16


extern BOOL g_bEnableScaleOutput;//是否使用缩放输出，即渲染使用1024然后拉伸输出到屏幕
extern int  g_nScaleOutputSize;
BOOL g_bScaleOutputSmooth = TRUE;

BOOL g_Get2DLineXbyY(D3DXVECTOR2& PointStart, D3DXVECTOR2& PointEnd, float fY, float &fX)
{
	BOOL bRet = FALSE;

	float fMax, fMin;
	fMin = min(PointStart.x, PointEnd.x);
	fMax = max(PointStart.x, PointEnd.x);

	KG_PROCESS_ERROR(fabs(PointStart.y - PointEnd.y) > FLT_EPSILON);

	fX = (PointStart.x - PointEnd.x) * fY / (PointStart.y - PointEnd.y);

	KG_PROCESS_ERROR(fX > fMin && fX < fMax);

	bRet = TRUE;

Exit0:
	return bRet;
}

BOOL g_Get2DLineYbyX(D3DXVECTOR2& PointStart, D3DXVECTOR2& PointEnd, float fX, float &fY)
{
	BOOL bRet = FALSE;

	float fMax, fMin;
	fMin = min(PointStart.y, PointEnd.y);
	fMax = max(PointStart.y, PointEnd.y);

	KG_PROCESS_ERROR(fabs(PointStart.x - PointEnd.x) > FLT_EPSILON);

	fY = (PointStart.y - PointEnd.y) * fX / (PointStart.x - PointEnd.x);

	KG_PROCESS_ERROR(fY > fMin && fY < fMax);

	bRet = TRUE;

Exit0:
	return bRet;
}

BOOL g_IsPointInPolygon(D3DXVECTOR2 Point, D3DXVECTOR2* pPolyPoint, DWORD dwPolySize)
{
	BOOL bRet = FALSE;
	DWORD i = 0;
	int nFlag[2];

	KGLOG_PROCESS_ERROR(pPolyPoint);
	KGLOG_PROCESS_ERROR(dwPolySize > 2);

	D3DXVECTOR2* pVector = new D3DXVECTOR2[dwPolySize + 1];

	for (i = 0; i < dwPolySize; i++)
	{
		pVector[i] = pPolyPoint[i] - Point;
	}

	pVector[dwPolySize] = pVector[0];

	nFlag[1] = 0;

	for (i = 0; i < dwPolySize; i++)
	{
		float fResult = pVector[i].x * pVector[i + 1].y - pVector[i + 1].x * pVector[i].y;

		nFlag[0] = (fResult > FLT_EPSILON)?1:((fResult < -FLT_EPSILON)?-1:0);

		if (nFlag[0])
		{
			if (!nFlag[1])
			{
				nFlag[1] = nFlag[0];
			}
			KG_PROCESS_ERROR(nFlag[0] == nFlag[1]);
		}
	}
	bRet = TRUE;
Exit0:
	return bRet;
}

KG3DGraphicsTool::KG3DGraphicsTool(void)
:m_pCamera(NULL)
{
    m_pShareDynamicVertexBuffer512 = NULL;
	m_lpLine = NULL;
	m_lpHermiteSpline = NULL;
	m_lpScreenRect = NULL;
	m_lpBox = NULL;
    m_pMeshCube = NULL;
	m_pMeshSphere = NULL;
	m_pMeshCylinder = NULL;

	m_pCurScene = NULL;
	m_lpLogicalTerrian = NULL;
	m_lpLogicalTerrianFace = NULL;
	m_lpLogicalItem = NULL;
	m_lpLogicalItemFace = NULL;
	m_lpd3dFont = NULL;
    m_lpd3dFont2 = NULL;
	m_lpRect = NULL;
	m_fRectSize = 64.0f;
	m_lpSRect[0] = m_lpSRect[1] = NULL;
    m_lpD3DLine = NULL;
    m_lpScreenRectXYZW = NULL;

	m_uRenderTargetTexWidth = 0;
	m_uRenderTargetTexHeight = 0;

    m_dwPrevTime = timeGetTime();
	
	m_nMaxNumBone = 0;

	/*_ASSERTE(DEFST_COUNT == _countof(m_ppDefaultShaders));
	memset(m_ppDefaultShaders, 0, _countof(m_ppDefaultShaders)*sizeof(LPD3DXEFFECT));*/


	m_lpTexture = NULL;

	m_lpGrassDeclaration = NULL;
	m_lpShareDepthSurface512 = NULL;
	m_lpShareColorSurface512 = NULL;
	m_lpCurrentConverMap = NULL;
	//m_lpCurrentShadowMap = NULL;

	m_lpTextureNoise3D = NULL;
	m_lpTextureSkin = NULL;

	m_lpRenderTarget0 = NULL;
	m_lpRenderTarget1 = NULL;
	m_lpRenderTargetSave = NULL;

	m_lpRenderTargetToolForScaleOutPut = NULL;

}



double KG3DGraphicsTool::CubicSplineIntl(double x[], double y[], int n, int k, double t, double s[5])
{ 
    int kk,m,l;
    double u[5],p,q;
    s[4]=0.0; s[0]=0.0; s[1]=0.0; s[2]=0.0; s[3]=0.0;
    if (n<1) return s[4];
    if (n==1) { s[0]=y[0]; s[4]=y[0]; return s[4];}
    if (n==2)
    { 
        s[0]=y[0]; s[1]=(y[1]-y[0])/(x[1]-x[0]);
        if (k<0)
            s[4]=(y[0]*(t-x[1])-y[1]*(t-x[0]))/(x[0]-x[1]);
        return s[4];
    }
    if (k<0)
    { 
        if (t<=x[1]) kk=0;
        else if (t>=x[n-1]) kk=n-2;
        else
        { 
            kk=1; m=n;
            while (((kk-m)!=1)&&((kk-m)!=-1))
            { 
                l=(kk+m)/2;
                if (t<x[l-1]) m=l;
                else kk=l;
            }
            kk=kk-1;
        }
    }
    else kk=k;
    if (kk>=n-1) kk=n-2;
    u[2]=(y[kk+1]-y[kk])/(x[kk+1]-x[kk]);
    if (n==3)
    { 
        if (kk==0)
        { u[3]=(y[2]-y[1])/(x[2]-x[1]);
        u[4]=2.0*u[3]-u[2];
        u[1]=2.0*u[2]-u[3];
        u[0]=2.0*u[1]-u[2];
        }
        else
        {
            u[1]=(y[1]-y[0])/(x[1]-x[0]);
            u[0]=2.0*u[1]-u[2];
            u[3]=2.0*u[2]-u[1];
            u[4]=2.0*u[3]-u[2];
        }
    }
    else
    { 
        if (kk<=1)
        { 
            u[3]=(y[kk+2]-y[kk+1])/(x[kk+2]-x[kk+1]);
            if (kk==1)
            { 
                u[1]=(y[1]-y[0])/(x[1]-x[0]);
                u[0]=2.0*u[1]-u[2];
                if (n==4) u[4]=2.0*u[3]-u[2];
                else u[4]=(y[4]-y[3])/(x[4]-x[3]);
            }
            else
            { 
                u[1]=2.0*u[2]-u[3];
                u[0]=2.0*u[1]-u[2];
                u[4]=(y[3]-y[2])/(x[3]-x[2]);
            }
        }
        else if (kk>=(n-3))
        { 
            u[1]=(y[kk]-y[kk-1])/(x[kk]-x[kk-1]);
            if (kk==(n-3))
            { 
                u[3]=(y[n-1]-y[n-2])/(x[n-1]-x[n-2]);
                u[4]=2.0*u[3]-u[2];
                if (n==4) u[0]=2.0*u[1]-u[2];
                else u[0]=(y[kk-1]-y[kk-2])/(x[kk-1]-x[kk-2]);
            }
            else
            { 
                u[3]=2.0*u[2]-u[1];
                u[4]=2.0*u[3]-u[2];
                u[0]=(y[kk-1]-y[kk-2])/(x[kk-1]-x[kk-2]);
            }
        }
        else
        { 
            u[1]=(y[kk]-y[kk-1])/(x[kk]-x[kk-1]);
            u[0]=(y[kk-1]-y[kk-2])/(x[kk-1]-x[kk-2]);
            u[3]=(y[kk+2]-y[kk+1])/(x[kk+2]-x[kk+1]);
            u[4]=(y[kk+3]-y[kk+2])/(x[kk+3]-x[kk+2]);
        }
    }

    s[0]=fabs(u[3]-u[2]);
    s[1]=fabs(u[0]-u[1]);

    if ((s[0]+1.0==1.0)&&(s[1]+1.0==1.0))
        p=(u[1]+u[2])/2.0;
    else p=(s[0]*u[1]+s[1]*u[2])/(s[0]+s[1]);

    s[0]=fabs(u[3]-u[4]);
    s[1]=fabs(u[2]-u[1]);

    if ((s[0]+1.0==1.0)&&(s[1]+1.0==1.0))
        q=(u[2]+u[3])/2.0;
    else q=(s[0]*u[2]+s[1]*u[3])/(s[0]+s[1]);

    s[0]=y[kk];
    s[1]=p;
    s[3]=x[kk+1]-x[kk];
    s[2]=(3.0*u[2]-2.0*p-q)/s[3];
    s[3]=(q+p-2.0*u[2])/(s[3]*s[3]);

    if (k<0)
    {
        p=t-x[kk];
        s[4]=s[0]+s[1]*p+s[2]*p*p+s[3]*p*p*p;
    }

    return s[4];
}

void KG3DGraphicsTool::LoadAllShaders()
{
	g_GetDefaultShaderGroup().LoadAllShaders();
}

LPD3DXEFFECT KG3DGraphicsTool::GetDefaultShader(KG3DDefaultShaderType Type)
{
	LPD3DXEFFECT p = g_GetDefaultShaderGroup().GetDefaultShader(Type).GetEffect();
	//p->AddRef();//其实这样子很危险，万一用这个函数的人g_cGraphicsTool.GetDefaultShader(XX)，前面忘记赋值，那么
	//这个引用计数就白加了，100%泄漏
	//要不这样更隐蔽一些，有个函数，Func(LPD3DXEFFECT p);里面事实上没有用到这个p。
	//那么Func(g_cGraphicsTool.GetDefaultShader(XX))也泄漏。
	//有空我会把全部用到这个的东东改成用ShaderManager。
	if (g_cGraphicsEngine.GetD3DCaps().VertexShaderVersion == D3DVS_VERSION(1, 1))
	{
		m_nMaxNumBone = MAX_NUM_BONE_PER_SET_SHADER_1;
	}
	else
	{
		m_nMaxNumBone = MAX_NUM_BONE_PER_SET_SHADER_2;      
	}
	return p;	
}

LPVOID KG3DGraphicsTool::GetDefaultShaderParamHandleBuffer(KG3DDefaultShaderType Type, size_t *puRetBufferSize)
{
    return g_GetDefaultShaderGroup().GetDefaultShader(Type).GetEffectParamHandleBuffer(puRetBufferSize);
}


void KG3DGraphicsTool::RefreshShader()
{
	g_GetDefaultShaderGroup().ReloadShaders();
}

KG3DGraphicsTool::~KG3DGraphicsTool(void)
{
	UnInit();
}

HRESULT KG3DGraphicsTool::Init()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hr = E_FAIL;

	SAFE_RELEASE(m_lpShareDepthSurface512);
	SAFE_RELEASE(m_lpShareColorSurface512);

    hr = g_pd3dDevice->CreateDepthStencilSurface(512, 512, D3DFMT_D24S8,D3DMULTISAMPLE_NONE, 0, TRUE, &m_lpShareDepthSurface512, NULL);
	if(FAILED(hr))
	{
		hr = g_pd3dDevice->CreateDepthStencilSurface(512, 512, D3DFMT_D16,D3DMULTISAMPLE_NONE, 0, TRUE, &m_lpShareDepthSurface512, NULL);
	}
    KGLOG_COM_CHECK_ERROR(hr);

    hr = g_pd3dDevice->CreateRenderTarget(512, 512, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 0, FALSE, &m_lpShareColorSurface512, NULL);
    KGLOG_COM_CHECK_ERROR(hr);

    hrResult = S_OK;

//Exit0:
    if (FAILED(hrResult))
    {
        SAFE_RELEASE(m_lpShareColorSurface512);
        SAFE_RELEASE(m_lpShareDepthSurface512);
    }
    return hrResult;
}

HRESULT KG3DGraphicsTool::UnInit()
{
    HRESULT hrResult = E_FAIL;

	SAFE_RELEASE(m_lpShareDepthSurface512);
	SAFE_RELEASE(m_lpShareColorSurface512);

	SAFE_RELEASE(m_lpRect);
	SAFE_RELEASE(m_lpLogicalTerrian);
	SAFE_RELEASE(m_lpLogicalTerrianFace);
	SAFE_RELEASE(m_lpLogicalItem);
	SAFE_RELEASE(m_lpLogicalItemFace);
	SAFE_RELEASE(m_lpd3dFont);
    SAFE_RELEASE(m_lpd3dFont2);
	SAFE_RELEASE(m_lpLine);
    SAFE_RELEASE(m_pShareDynamicVertexBuffer512);
	SAFE_RELEASE(m_lpHermiteSpline);
	SAFE_RELEASE(m_lpScreenRect);
	SAFE_RELEASE(m_lpBox);
	SAFE_RELEASE(m_lpSRect[0]);
	SAFE_RELEASE(m_lpSRect[1]);

	SAFE_RELEASE(m_pMeshCube);
	SAFE_RELEASE(m_pMeshSphere);
	SAFE_RELEASE(m_pMeshCylinder);
    SAFE_RELEASE(m_lpD3DLine);
	SAFE_RELEASE(m_lpTexture);

	SAFE_RELEASE(m_lpTextureNoise3D);
	SAFE_RELEASE(m_lpTextureSkin);

    SAFE_RELEASE(m_lpScreenRectXYZW);

	/*for (DWORD i = 0; i < DEFST_COUNT; i++)
	{
		SAFE_RELEASE(m_ppDefaultShaders[i]);
	}*/
	m_nMaxNumBone = 0;

	SAFE_RELEASE(m_lpGrassDeclaration);
	SAFE_RELEASE(m_lpRenderTargetSave);
	SAFE_RELEASE(m_lpRenderTarget0);
	SAFE_RELEASE(m_lpRenderTarget1);

	SAFE_DELETE(m_lpRenderTargetToolForScaleOutPut);

    hrResult = S_OK;

    return hrResult;
}

HRESULT KG3DGraphicsTool::CreateChessboardTexture(UINT  uWidth, 
                                                  UINT  uHeight, 
                                                  DWORD dwColor, 
                                                  LPDIRECT3DTEXTURE9* ppTexture
                                                  )
{
    if ((int)uWidth < 1 || (int)uHeight < 1 || !ppTexture)
        return E_FAIL;

    //uWidth  /= 10;
    //uHeight /= 10;

    if (uWidth < 1)
        uWidth = 1;

    if (uHeight < 1)
        uHeight = 1;

    ASSERT(ppTexture);
    ASSERT(*ppTexture == NULL);

    if (FAILED(D3DXCreateTexture(g_pd3dDevice, uWidth, uHeight, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, ppTexture)))
        return E_FAIL;

    IDirect3DSurface9* pSurface = NULL;
    (*ppTexture)->GetSurfaceLevel(0, &pSurface);
    
    ASSERT(pSurface);

    D3DLOCKED_RECT LockRect;
    if (SUCCEEDED(pSurface->LockRect(&LockRect, 0, 0)))
    {
        BYTE* pBuffer = reinterpret_cast<BYTE*>(LockRect.pBits);

        for (UINT u = 0; u < uHeight; ++u)
        {
            DWORD* pColor = reinterpret_cast<DWORD*>(pBuffer + u * LockRect.Pitch);

            for (UINT w = 0; w < uWidth; ++w)
            {
                if (u % 2)
                {
                    if (w % 2)
                        pColor[w] = ( dwColor) | 0xff000000;
                    else
                        pColor[w] = (~dwColor) | 0xff000000;
                }
                else
                {
                    if (w % 2)
                        pColor[w] = (~dwColor) | 0xff000000;
                    else
                        pColor[w] = ( dwColor) | 0xff000000;
                }
            }
        }

        pSurface->UnlockRect();
    }

    SAFE_RELEASE(pSurface);
    return S_OK;
}


HRESULT KG3DGraphicsTool::DrawCircle(const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, FLOAT fRadius, DWORD color)
{
    D3DXVECTOR3 vTangent;
    D3DXVECTOR3 vTemp = D3DXVECTOR3(vNormal.x + 100.f, vNormal.y + 100.f, vNormal.z + 100.f);
    D3DXVec3Cross(&vTangent, &vNormal, &vTemp);
    D3DXVec3Normalize(&vTangent, &vTangent);

    vTangent *= fRadius;

    KG3DRenderState R;

    struct _Vertex 
    {
        D3DXVECTOR3 p;
        DWORD       c;
    };

    if ( m_pShareDynamicVertexBuffer512 == NULL)
    {
        if( FAILED( g_pd3dDevice->CreateVertexBuffer( 512 * sizeof(  _Vertex ),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ|D3DFVF_DIFFUSE,
            D3DPOOL_DEFAULT, &m_pShareDynamicVertexBuffer512,NULL ) ) )
        {
            return E_FAIL;
        }
    }

    _Vertex* v  ;
    if( FAILED( m_pShareDynamicVertexBuffer512->Lock(0, 0, (void**)&v, D3DLOCK_DISCARD) ) )
        return E_FAIL;
    
    int count = 0;
    for (float angel = 0.f; angel < D3DX_PI * 2 && count < 500; angel += 0.1f, count++)
    {
        D3DXVECTOR3 vPoint;
        D3DXMATRIX  matRot;
        D3DXMatrixRotationAxis(&matRot, &vNormal, angel);
        D3DXVec3TransformNormal(&vPoint, &vTangent, &matRot);
        v[count].p = vCenter + vPoint;
        v[count].c = color;
    }
    v[count].p = v[0].p;
    v[count].c = color;
    m_pShareDynamicVertexBuffer512->Unlock();

    R.SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    R.SetRenderState( D3DRS_LIGHTING, FALSE );	
    g_pd3dDevice->SetTexture( 0,NULL);

    g_pd3dDevice->SetStreamSource( 0, m_pShareDynamicVertexBuffer512,0, sizeof(_Vertex ) );
    g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE );
    g_pd3dDevice->DrawPrimitive  ( D3DPT_LINESTRIP , 0 ,count);

    return S_OK;
}



HRESULT KG3DGraphicsTool::DrawFan(const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vNormal, FLOAT fRadius, const D3DXVECTOR3& vBegin, FLOAT fAngle, DWORD color )
{
    D3DXVECTOR3 vBegPoint = vBegin - vCenter;;
    D3DXVec3Normalize(&vBegPoint, &vBegPoint);
    vBegPoint *= fRadius;

    KG3DRenderState R;

    struct _Vertex 
    {
        D3DXVECTOR3 p;
        DWORD       c;
    };

    if ( m_pShareDynamicVertexBuffer512 == NULL)
    {
        if( FAILED( g_pd3dDevice->CreateVertexBuffer( 512 * sizeof(  _Vertex ),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ|D3DFVF_DIFFUSE,
            D3DPOOL_DEFAULT, &m_pShareDynamicVertexBuffer512,NULL ) ) )
        {
            return E_FAIL;
        }
    }

    _Vertex* v  ;
    if( FAILED( m_pShareDynamicVertexBuffer512->Lock(0, 0, (void**)&v, D3DLOCK_DISCARD) ) )
        return E_FAIL;

    int count = 0;
    v[count].p = vCenter;
    v[count].c = color;
    ++count;
    if (fAngle > 0)
    {
        for (float angel = 0; angel < fAngle && count < 125; angel += 0.1f, count++)
        {
            D3DXVECTOR3 vPoint;
            D3DXMATRIX  matRot;
            D3DXMatrixRotationAxis(&matRot, &vNormal, angel);
            D3DXVec3TransformNormal(&vPoint, &vBegPoint, &matRot);
            v[count].p = vCenter + vPoint;
            v[count].c = color;
        }
    }
    else 
    {
        for (float angel = 0; angel > fAngle && count < 500; angel -= 0.1f, count++)
        {
            D3DXVECTOR3 vPoint;
            D3DXMATRIX  matRot;
            D3DXMatrixRotationAxis(&matRot, &vNormal, angel);
            D3DXVec3TransformNormal(&vPoint, &vBegPoint, &matRot);
            v[count].p = vCenter + vPoint;
            v[count].c = color;
        }
    }
    v[count].p = v[0].p;
    v[count].c = color;
    m_pShareDynamicVertexBuffer512->Unlock();

    R.SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);
    R.SetRenderState( D3DRS_LIGHTING, FALSE );	
    g_pd3dDevice->SetTexture( 0,NULL);

    g_pd3dDevice->SetStreamSource( 0, m_pShareDynamicVertexBuffer512,0, sizeof(_Vertex ) );
    g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE );
    g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLEFAN , 0 ,count);

    return S_OK;
}

HRESULT KG3DGraphicsTool::DrawLine(D3DXVECTOR3 *pPoint0, D3DXVECTOR3 *pPoint1, DWORD dwColor0, DWORD dwColor1)
{
	HRESULT hr = S_OK;

	GraphicsStruct::RenderState Light(D3DRS_LIGHTING,FALSE);
	g_pd3dDevice->SetTexture(0,NULL);
	
	struct LineVertex
	{
		D3DXVECTOR3 posi;
		D3DCOLOR    color;
	}  A , B;

	A.posi   = *pPoint0 ;A.color  = dwColor0;
	B.posi   = *pPoint1 ;B.color  = dwColor1;

	if( m_lpLine == NULL)
	{
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 2*sizeof(  LineVertex ),
			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_DEFAULT, &m_lpLine,NULL ) ) )
		{
			return E_FAIL;
		}
	}

	LineVertex* pL = NULL ;

	if( FAILED( m_lpLine->Lock(0, 0, (void**)&pL, D3DLOCK_DISCARD) ) )
		return E_FAIL;

	if(!pL)
		return E_FAIL;

	pL[0] = A;pL[1] = B;
	m_lpLine->Unlock();
    g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	
	g_pd3dDevice->SetTexture( 0,NULL);
    DWORD dwColorSource = 0;
    g_pd3dDevice->GetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, &dwColorSource);
    g_pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_COLOR1);

	hr = g_pd3dDevice->SetStreamSource( 0, m_lpLine,0, sizeof(LineVertex ) );
	hr = g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE );
	hr = g_pd3dDevice->DrawPrimitive  ( D3DPT_LINELIST , 0 ,1   );

    g_pd3dDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, dwColorSource);
    hr = g_pd3dDevice->SetStreamSource( 0, NULL,0, 0 );
	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawHermiteSpline(D3DXVECTOR3 *pPoint0,D3DXVECTOR3 *pPoint0Tan,
											D3DXVECTOR3 *pPoint1,D3DXVECTOR3 *pPoint1Tan,
											DWORD dwColor0, DWORD dwColor1,DWORD dwNumCount)
{
	HRESULT hr = S_OK;

	GraphicsStruct::RenderState Light(D3DRS_LIGHTING,FALSE);

	g_pd3dDevice->SetTexture(0,NULL);

	struct HermiteVertex
	{
		D3DXVECTOR3 posi;
		D3DCOLOR    color;
	};

	if( m_lpHermiteSpline == NULL)
	{
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 128*2*sizeof(  HermiteVertex ),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpHermiteSpline,NULL ) ) )
		{
			return E_FAIL;
		}
	}
	HermiteVertex* pL  ;
	if( FAILED( m_lpHermiteSpline->Lock( 0, 0, (void**)&pL, 0 ) ) )
		return E_FAIL;

	DWORD Count = min(dwNumCount,127);
	float T = 1.0F/Count;
	for(DWORD i=0;i<=Count;i++)
	{
		float S = i*T;
		D3DXVec3Hermite(&pL[i].posi,
			pPoint0,
			pPoint0Tan,
			pPoint1,
			pPoint1Tan,
			S);
		if(i%2==0)
			pL[i].color = dwColor0;
		else
			pL[i].color = dwColor1;
	}

	m_lpHermiteSpline->Unlock();
	g_pd3dDevice->SetTexture( 0,NULL);

	hr = g_pd3dDevice->SetStreamSource( 0, m_lpHermiteSpline,0, sizeof(HermiteVertex ) );
	hr = g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE );
	hr = g_pd3dDevice->DrawPrimitive  ( D3DPT_LINESTRIP , 0 ,Count);

	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawArc(D3DXVECTOR3& vCenter,float R,float fAngleStart,float fAngleEnd,float fHeightInc,DWORD dwColor,DWORD dwNumCount)
{
	HRESULT hr = S_OK;

	GraphicsStruct::RenderState Light(D3DRS_LIGHTING,FALSE);

	g_pd3dDevice->SetTexture(0,NULL);

	struct ArcVertex
	{
		D3DXVECTOR3 posi;
		D3DCOLOR    color;
	};

	if( m_lpHermiteSpline == NULL)
	{
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 128*2*sizeof(  ArcVertex ),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpHermiteSpline,NULL ) ) )
		{
			return E_FAIL;
		}
	}
	ArcVertex* pL  ;
	if( FAILED( m_lpHermiteSpline->Lock( 0, 0, (void**)&pL, 0 ) ) )
		return E_FAIL;

	DWORD Count = min(dwNumCount,127);
	float T = 1.0F/Count;
	for(DWORD i=0;i<=Count;i++)
	{
		float S = i*T;
		float Angle = fAngleStart + (fAngleEnd - fAngleStart) * S;

		D3DXVECTOR3 P(cosf(Angle)*R,0/*fHeightInc * S*/,sinf(Angle)*R);
		pL[i].posi = vCenter + P;
		pL[i].color = dwColor;
	}

	m_lpHermiteSpline->Unlock();
	g_pd3dDevice->SetTexture( 0,NULL);

	hr = g_pd3dDevice->SetStreamSource( 0, m_lpHermiteSpline,0, sizeof(ArcVertex ) );
	hr = g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE );
	hr = g_pd3dDevice->DrawPrimitive  ( D3DPT_LINESTRIP , 0 ,Count);

	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawTriangle(D3DXVECTOR3* pPos,D3DXVECTOR3* pNormal,D3DXVECTOR2* pUV)
{
	if( m_lpRect == NULL)
	{
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof( VERTEXWATER ),
			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, FVFWATER,
			D3DPOOL_DEFAULT, &m_lpRect,NULL ) ) )
		{
			return E_FAIL;
		}
	}

	VERTEXWATER * pVer = NULL;
	if(FAILED( m_lpRect->Lock( 0, 4*sizeof( VERTEXWATER ), (void**)&pVer, D3DLOCK_DISCARD )))
		return E_FAIL;

	for (int i=0;i<3;i++)
	{	
		pVer[i].p = pPos[i];
		pVer[i].c = 0xFFFFFFFF;
		pVer[i].n = pNormal[i];
		pVer[i].tu = pUV[i].x;
		pVer[i].tv = pUV[i].y;
	}
	m_lpRect->Unlock();

	g_pd3dDevice->SetStreamSource( 0, m_lpRect,0, sizeof( VERTEXWATER ) );
	g_pd3dDevice->SetFVF( FVFWATER );
	g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLELIST , 0 , 1);

	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawTrapezia(const D3DXVECTOR3& a, 
                                   const D3DXVECTOR3& b, 
                                   const D3DXVECTOR3& c, 
                                   const D3DXVECTOR3& d, 
                                   const DWORD dwColor
                                   )
{
    if( m_lpRect == NULL)
    {
        if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof( VERTEXWATER ),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, FVFWATER,
            D3DPOOL_DEFAULT, &m_lpRect,NULL ) ) )
        {
            return E_FAIL;
        }
    }

    KG3DRenderState State;

    VERTEXWATER * pVer = NULL;
    if(FAILED( m_lpRect->Lock( 0, 4*sizeof( VERTEXWATER ), (void**)&pVer, D3DLOCK_DISCARD )))
        return E_FAIL;


    D3DXVECTOR3 Normal(0,1,0);

    pVer[0].p = a;pVer[0].c = dwColor;pVer[0].n = Normal;
    pVer[0].tu = 0.f;
    pVer[0].tv = 0.f;
    pVer[1].p = b;pVer[1].c = dwColor;pVer[1].n = Normal;
    pVer[1].tu = 0.f;
    pVer[1].tv = 0.f;
    pVer[2].p = d;pVer[2].c = dwColor;pVer[2].n = Normal;
    pVer[2].tu = 0.f;
    pVer[2].tv = 0.f;
    pVer[3].p = c;pVer[3].c = dwColor;pVer[3].n = Normal;
    pVer[3].tu = 0.f;
    pVer[3].tv = 0.f;

    m_lpRect->Unlock();

    GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);

    g_pd3dDevice->SetStreamSource( 0, m_lpRect,0, sizeof( VERTEXWATER ) );
    g_pd3dDevice->SetFVF( FVFWATER );
    g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLESTRIP , 0 , 2);

    return S_OK;
}

HRESULT KG3DGraphicsTool::DrawRect(D3DXVECTOR3* pUpperLeft,
                                   D3DXVECTOR3* pUpperRight,
                                   D3DXVECTOR3* pLowerLeft, 
                                   D3DCOLOR Diffuse, 
                                   IDirect3DTexture9* pTexture,
                                   FLOAT fCoordDU /* = 1 */, 
                                   FLOAT fCoordDV /* = 1 */,
                                   FLOAT fCoordAU /* = 0 */,
                                   FLOAT fCoordAV /* = 0  */
                                   )
{
    if( m_lpRect == NULL)
    {
        if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof( VERTEXWATER ),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, FVFWATER,
            D3DPOOL_DEFAULT, &m_lpRect,NULL ) ) )
        {
            return E_FAIL;
        }
    }

    KG3DRenderState State;

    D3DXVECTOR3 A,B,C,D;
    A = *pUpperLeft;
    B = *pUpperRight;
    D = *pLowerLeft;
    C = B + D - A;

    VERTEXWATER * pVer = NULL;
    if(FAILED( m_lpRect->Lock( 0, 4*sizeof( VERTEXWATER ), (void**)&pVer, D3DLOCK_DISCARD )))
        return E_FAIL;

    D3DXVECTOR2 uv0 = D3DXVECTOR2(static_cast<float>(fCoordAU), static_cast<float>(fCoordAV));
    D3DXVECTOR2 uv1 = D3DXVECTOR2(static_cast<float>(fCoordDU), static_cast<float>(fCoordAV));
    D3DXVECTOR2 uv2 = D3DXVECTOR2(static_cast<float>(fCoordAU), static_cast<float>(fCoordDV));
    D3DXVECTOR2 uv3 = D3DXVECTOR2(static_cast<float>(fCoordDU), static_cast<float>(fCoordDV));

	D3DXVECTOR3 Normal(0,1,0);

    pVer[0].p = A;pVer[0].c = Diffuse;pVer[0].n = Normal;
    pVer[0].tu = uv0.x;
    pVer[0].tv = uv0.y;
    pVer[1].p = B;pVer[1].c = Diffuse;pVer[1].n = Normal;
    pVer[1].tu = uv1.x;
    pVer[1].tv = uv1.y;
    pVer[2].p = D;pVer[2].c = Diffuse;pVer[2].n = Normal;
    pVer[2].tu = uv2.x;
    pVer[2].tv = uv2.y;
    pVer[3].p = C;pVer[3].c = Diffuse;pVer[3].n = Normal;
    pVer[3].tu = uv3.x;
    pVer[3].tv = uv3.y;

    m_lpRect->Unlock();

    State.SetRenderState( D3DRS_LIGHTING, FALSE );	

    if(pTexture == NULL)
    {
        /*g_pd3dDevice->SetTexture(0,NULL);
        g_pd3dDevice->SetTexture(1,NULL);
        g_pd3dDevice->SetTexture(2,NULL);*/
		GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);

		g_pd3dDevice->SetStreamSource( 0, m_lpRect,0, sizeof( VERTEXWATER ) );
		g_pd3dDevice->SetFVF( FVFWATER );
		g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLESTRIP , 0 , 2);

    }
    else
    {
        g_pd3dDevice->SetTexture(0, pTexture);
        g_pd3dDevice->SetTexture(1,NULL);
        g_pd3dDevice->SetTexture(2,NULL);
   
		GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);


	
		//g_pd3dDevice->SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU);
		State.SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
		State.SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
		g_pd3dDevice->SetTexture( 1,NULL);
		State.SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);
		State.SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

		State.SetSamplerState(0,D3DSAMP_MAGFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMagFilter);
		State.SetSamplerState(0,D3DSAMP_MINFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMinFilter);
		State.SetSamplerState(0,D3DSAMP_MIPFILTER,(D3DTEXTUREFILTERTYPE)g_cEngineOption.nSampMipFilter);
		State.SetSamplerState(0,D3DSAMP_MAXANISOTROPY ,g_cEngineOption.dwMaxAnisotropy);
		/*g_pd3dDevice->SetSamplerState(0,D3DSAMP_MIPMAPLODBIAS, 2);*/

		g_pd3dDevice->SetStreamSource( 0, m_lpRect,0, sizeof( VERTEXWATER ) );
		g_pd3dDevice->SetFVF( FVFWATER );
		g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLESTRIP , 0 , 2);
	}

    return S_OK;
}

HRESULT KG3DGraphicsTool::DrawText(FLOAT sx, FLOAT sy,FLOAT scx, FLOAT scy,DWORD dwColor,WORD FontHeight,
								   TCHAR* strFont, TCHAR* strText, DWORD dwFlags)
{
	//LPTEXTBUFFER pTextBuffer = GetTextBuffer(strText,FontHeight,strFont);

	//D3DXVECTOR2 A,C;
	//A = D3DXVECTOR2(sx,sy);
	//C = D3DXVECTOR2(sx+pTextBuffer->Width*scx,sy+pTextBuffer->Height*scy);



	//DrawScreenRect(&A,&C,0,1,1,dwColor,pTextBuffer->TextureID);
	////DrawScreenRect(&A,&C,0,1,1,dwColor,0);
	//g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, TRUE );

	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawGroundGrid(float Weight, DWORD dwColor/* = 0xffff0000*/)
{
	int N = 20;
	float Len = Weight;
	int i = 0;
	for(i = -N; i <= N; i++)
	{
		D3DXVECTOR3 A = D3DXVECTOR3(i*Len,0, N*Len);
		D3DXVECTOR3 B = D3DXVECTOR3(i*Len,0,-N*Len);

		DrawLine(&A,&B, dwColor, dwColor); 
	}
	for(i = -N;i <= N; i++)
	{
		D3DXVECTOR3 A = D3DXVECTOR3( N*Len,0,i*Len);
		D3DXVECTOR3 B = D3DXVECTOR3(-N*Len,0,i*Len);

		DrawLine(&A,&B, dwColor, dwColor); 
	}
	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawScreenRect(const D3DXVECTOR2* pA,
										 const D3DXVECTOR2* pC,
										 float Z,
										 float ScaleU,
										 float ScaleV,
										 D3DCOLOR Diffuse,
										 DWORD TextureID)
{
    KG3DRenderState RenderState;
	if( m_lpScreenRect == NULL)
	{
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 6*sizeof( _ScreenPoint ),
			0,FVF_SCREEN,
			D3DPOOL_MANAGED, &m_lpScreenRect,NULL ) ) )
		{
			return E_FAIL;
		}
	}

	D3DXVECTOR2 A,B,C,D;
	A = *pA;
	C = *pC;
	B.x = C.x;B.y = A.y;
	D.x = A.x;D.y = C.y;

	_ScreenPoint * pVer = NULL;
	if( FAILED( m_lpScreenRect->Lock( 0, 0, (void**)&pVer, 0 ) ) )
		return E_FAIL;
	pVer[0].x = B.x;pVer[0].y = B.y;pVer[0].z = Z;pVer[0].rhw = 1;pVer[0].color = Diffuse;pVer[0].tu = 1*ScaleU;pVer[0].tv = 0;
	pVer[1].x = A.x;pVer[1].y = A.y;pVer[1].z = Z;pVer[1].rhw = 1;pVer[1].color = Diffuse;pVer[1].tu = 0;pVer[1].tv = 0;
	pVer[2].x = D.x;pVer[2].y = D.y;pVer[2].z = Z;pVer[2].rhw = 1;pVer[2].color = Diffuse;pVer[2].tu = 0;pVer[2].tv = 1*ScaleV;

	pVer[3].x = B.x;pVer[3].y = B.y;pVer[3].z = Z;pVer[3].rhw = 1;pVer[3].color = Diffuse;pVer[3].tu = 1*ScaleU;pVer[3].tv = 0;
	pVer[4].x = D.x;pVer[4].y = D.y;pVer[4].z = Z;pVer[4].rhw = 1;pVer[4].color = Diffuse;pVer[4].tu = 0;pVer[4].tv = 1*ScaleV;
	pVer[5].x = C.x;pVer[5].y = C.y;pVer[5].z = Z;pVer[5].rhw = 1;pVer[5].color = Diffuse;pVer[5].tu = 1*ScaleU;pVer[5].tv = 1*ScaleV;

	m_lpScreenRect->Unlock();
	RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	if(TextureID==0)
	{
		g_pd3dDevice->SetTexture(0,NULL);
		g_pd3dDevice->SetTexture(1,NULL);
		g_pd3dDevice->SetTexture(2,NULL);
	}
	else
	{
		if (FAILED(g_cTextureTable.SetTexture(0,TextureID)))
		{
			g_pd3dDevice->SetTexture(0, NULL);
		}
		
		g_pd3dDevice->SetTexture(1,NULL);
		g_pd3dDevice->SetTexture(2,NULL);
	}

	if ((Diffuse & 0xff000000) != 0xff000000)
	{
		RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		RenderState.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	RenderState.SetRenderState( D3DRS_LIGHTING, FALSE );	
	RenderState.SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU);
	/*RenderState.SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
	RenderState.SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);*/
	//RenderState.SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );	
	g_pd3dDevice->SetStreamSource( 0, m_lpScreenRect,0, sizeof( _ScreenPoint ) );
	g_pd3dDevice->SetFVF( FVF_SCREEN );
	g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLELIST , 0 ,2 );
	return S_OK;
}


HRESULT KG3DGraphicsTool::DrawScreenRectUVTrans(const D3DXVECTOR2* pA,
										 const D3DXVECTOR2* pC,
										 float Z,
										 D3DXVECTOR2 vUVScale,
										 D3DXVECTOR2 vUVTrans,
										 D3DCOLOR Diffuse,
										 KG3DTexture* pTexture)
{
	KG3DRenderState RenderState;
	if( m_lpScreenRect == NULL)
	{
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 6*sizeof( _ScreenPoint ),
			0,FVF_SCREEN,
			D3DPOOL_MANAGED, &m_lpScreenRect,NULL ) ) )
		{
			return E_FAIL;
		}
	}

	D3DXVECTOR2 A,B,C,D;
	A = *pA;
	C = *pC;
	B.x = C.x;B.y = A.y;
	D.x = A.x;D.y = C.y;

	_ScreenPoint * pVer = NULL;
	if( FAILED( m_lpScreenRect->Lock( 0, 0, (void**)&pVer, 0 ) ) )
		return E_FAIL;

	float u0 = vUVTrans.x;
	float v0 = vUVTrans.y;
	float u1 = u0 + vUVScale.x;
	float v1 = v0 + vUVScale.y;

	pVer[0].x = B.x;pVer[0].y = B.y;pVer[0].z = Z;pVer[0].rhw = 1;pVer[0].color = Diffuse;pVer[0].tu = u1;pVer[0].tv = v0;
	pVer[1].x = A.x;pVer[1].y = A.y;pVer[1].z = Z;pVer[1].rhw = 1;pVer[1].color = Diffuse;pVer[1].tu = u0;pVer[1].tv = v0;
	pVer[2].x = D.x;pVer[2].y = D.y;pVer[2].z = Z;pVer[2].rhw = 1;pVer[2].color = Diffuse;pVer[2].tu = u0;pVer[2].tv = v1;

	pVer[3].x = B.x;pVer[3].y = B.y;pVer[3].z = Z;pVer[3].rhw = 1;pVer[3].color = Diffuse;pVer[3].tu = u1;pVer[3].tv = v0;
	pVer[4].x = D.x;pVer[4].y = D.y;pVer[4].z = Z;pVer[4].rhw = 1;pVer[4].color = Diffuse;pVer[4].tu = u0;pVer[4].tv = v1;
	pVer[5].x = C.x;pVer[5].y = C.y;pVer[5].z = Z;pVer[5].rhw = 1;pVer[5].color = Diffuse;pVer[5].tu = u1;pVer[5].tv = v1;

	m_lpScreenRect->Unlock();
	RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	if(!pTexture)
	{
		g_pd3dDevice->SetTexture(0,NULL);
		g_pd3dDevice->SetTexture(1,NULL);
		g_pd3dDevice->SetTexture(2,NULL);
	}
	else
	{
		pTexture->SetTexture(g_pd3dDevice,0);

		g_pd3dDevice->SetTexture(1,NULL);
		g_pd3dDevice->SetTexture(2,NULL);
	}

	if ((Diffuse & 0xff000000) != 0xff000000)
	{
		RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		RenderState.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	RenderState.SetRenderState( D3DRS_LIGHTING, FALSE );	
	RenderState.SetTextureStageState(0,D3DTSS_TEXCOORDINDEX,D3DTSS_TCI_PASSTHRU);

	g_pd3dDevice->SetStreamSource( 0, m_lpScreenRect,0, sizeof( _ScreenPoint ) );
	g_pd3dDevice->SetFVF( FVF_SCREEN );
	g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLELIST , 0 ,2 );
	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawScreenRect(const D3DXVECTOR2 *pPointA, 
										 const D3DXVECTOR2 *pPointC,
										 float fZ, 
										 DWORD dwColor,
										 DWORD dwTextureID)
{
	HRESULT hr = S_OK;
	KG3DRenderState RenderState;
	if( m_lpScreenRect == NULL)
	{
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 6*sizeof( _ScreenPoint ),
			0,FVF_SCREEN,
			D3DPOOL_MANAGED, &m_lpScreenRect,NULL ) ) )
		{
			return E_FAIL;
		}
	}

	D3DXVECTOR2 A,B,C,D;
	A = (*pPointA);
	C = (*pPointC);
	B.x = C.x;B.y = A.y;
	D.x = A.x;D.y = C.y;

	_ScreenPoint * pVer = NULL;
	if( FAILED( m_lpScreenRect->Lock( 0, 0, (void**)&pVer, 0 ) ) )
		return E_FAIL;
	pVer[0].x = B.x;pVer[0].y = B.y;pVer[0].z = fZ;pVer[0].rhw = 1;pVer[0].color = dwColor;pVer[0].tu = 1;pVer[0].tv = 0;
	pVer[1].x = A.x;pVer[1].y = A.y;pVer[1].z = fZ;pVer[1].rhw = 1;pVer[1].color = dwColor;pVer[1].tu = 0;pVer[1].tv = 0;
	pVer[2].x = D.x;pVer[2].y = D.y;pVer[2].z = fZ;pVer[2].rhw = 1;pVer[2].color = dwColor;pVer[2].tu = 0;pVer[2].tv = 1;

	pVer[3].x = B.x;pVer[3].y = B.y;pVer[3].z = fZ;pVer[3].rhw = 1;pVer[3].color = dwColor;pVer[3].tu = 1;pVer[3].tv = 0;
	pVer[4].x = D.x;pVer[4].y = D.y;pVer[4].z = fZ;pVer[4].rhw = 1;pVer[4].color = dwColor;pVer[4].tu = 0;pVer[4].tv = 1;
	pVer[5].x = C.x;pVer[5].y = C.y;pVer[5].z = fZ;pVer[5].rhw = 1;pVer[5].color = dwColor;pVer[5].tu = 1;pVer[5].tv = 1;

	m_lpScreenRect->Unlock();

	LPDIRECT3DBASETEXTURE9 pTextureSave = NULL;
	KG3DTexture* pTexture = NULL;
	g_pd3dDevice->GetTexture(0, &pTextureSave);
	if(pTextureSave)
		pTextureSave->Release();
	hr = g_cTextureTable.GetResource(&pTexture, dwTextureID);
	if (FAILED(hr))
	{
		g_pd3dDevice->SetTexture(0, NULL);
	}
	else
	{
		g_pd3dDevice->SetTexture(0, pTexture->GetTexture());
		if (pTexture)
		{
			pTexture->Release();
		}
	}
	
	RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);
	RenderState.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	if ((dwColor & 0xff000000) != 0xff000000)
	{
		RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		RenderState.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	hr = g_pd3dDevice->SetStreamSource( 0, m_lpScreenRect,0, sizeof( _ScreenPoint ) );
	hr = g_pd3dDevice->SetFVF( FVF_SCREEN );
	hr = g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLELIST , 0 ,2 );
	
	g_pd3dDevice->SetTexture(0, pTextureSave);
	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawScreenRect(const D3DXVECTOR2 *pPointA, 
					   const D3DXVECTOR2 *pPointC,
					   float fZ, 
					   DWORD dwColor,
					   KG3DTexture* pTexture)
{
	if (!pTexture)
	{
		return E_FAIL;
	}
	return DrawScreenRect(pPointA, pPointC, fZ, dwColor, pTexture->GetTexture());
}

HRESULT KG3DGraphicsTool::DrawScreenRectNoChange(const D3DXVECTOR2 *pPointA, 
										 const D3DXVECTOR2 *pPointC,
										 float fZ, 
										 DWORD dwColor)
{
	HRESULT hr = S_OK;
	KG3DRenderState RenderState;
	if( m_lpScreenRect == NULL)
	{
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 6*sizeof( _ScreenPoint ),
			0,FVF_SCREEN,
			D3DPOOL_MANAGED, &m_lpScreenRect,NULL ) ) )
		{
			return E_FAIL;
		}
	}

	D3DXVECTOR2 A,B,C,D;
	A = (*pPointA);
	C = (*pPointC);
	B.x = C.x;B.y = A.y;
	D.x = A.x;D.y = C.y;

	_ScreenPoint * pVer = NULL;
	if( FAILED( m_lpScreenRect->Lock( 0, 0, (void**)&pVer, 0 ) ) )
		return E_FAIL;
	pVer[0].x = B.x;pVer[0].y = B.y;pVer[0].z = fZ;pVer[0].rhw = 1;pVer[0].color = dwColor;pVer[0].tu = 1;pVer[0].tv = 0;
	pVer[1].x = A.x;pVer[1].y = A.y;pVer[1].z = fZ;pVer[1].rhw = 1;pVer[1].color = dwColor;pVer[1].tu = 0;pVer[1].tv = 0;
	pVer[2].x = D.x;pVer[2].y = D.y;pVer[2].z = fZ;pVer[2].rhw = 1;pVer[2].color = dwColor;pVer[2].tu = 0;pVer[2].tv = 1;

	pVer[3].x = B.x;pVer[3].y = B.y;pVer[3].z = fZ;pVer[3].rhw = 1;pVer[3].color = dwColor;pVer[3].tu = 1;pVer[3].tv = 0;
	pVer[4].x = D.x;pVer[4].y = D.y;pVer[4].z = fZ;pVer[4].rhw = 1;pVer[4].color = dwColor;pVer[4].tu = 0;pVer[4].tv = 1;
	pVer[5].x = C.x;pVer[5].y = C.y;pVer[5].z = fZ;pVer[5].rhw = 1;pVer[5].color = dwColor;pVer[5].tu = 1;pVer[5].tv = 1;

	m_lpScreenRect->Unlock();

	RenderState.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RenderState.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	RenderState.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);
	RenderState.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	RenderState.SetRenderState(D3DRS_FOGENABLE, FALSE);
	//if ((dwColor & 0xff000000) != 0xff000000)
	{
		RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		RenderState.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	hr = g_pd3dDevice->SetStreamSource( 0, m_lpScreenRect,0, sizeof( _ScreenPoint ) );
	hr = g_pd3dDevice->SetFVF( FVF_SCREEN );
	hr = g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLELIST , 0 ,2 );

	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawScreenRect(const D3DXVECTOR2 *pPointA, 
					   const D3DXVECTOR2 *pPointC,
					   float fZ, 
					   DWORD dwColor,
					   LPDIRECT3DTEXTURE9 pTexture)
{
	HRESULT hr = S_OK;
	KG3DRenderState RenderState;
	if( m_lpScreenRect == NULL)
	{
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 6*sizeof( _ScreenPoint ),
			0,FVF_SCREEN,
			D3DPOOL_MANAGED, &m_lpScreenRect,NULL ) ) )
		{
			return E_FAIL;
		}
	}

	D3DXVECTOR2 A,B,C,D;
	A = (*pPointA);
	C = (*pPointC);
	B.x = C.x;B.y = A.y;
	D.x = A.x;D.y = C.y;

	_ScreenPoint * pVer = NULL;
	if( FAILED( m_lpScreenRect->Lock( 0, 0, (void**)&pVer, 0 ) ) )
		return E_FAIL;
	pVer[0].x = B.x;pVer[0].y = B.y;pVer[0].z = fZ;pVer[0].rhw = 1;pVer[0].color = dwColor;pVer[0].tu = 1;pVer[0].tv = 0;
	pVer[1].x = A.x;pVer[1].y = A.y;pVer[1].z = fZ;pVer[1].rhw = 1;pVer[1].color = dwColor;pVer[1].tu = 0;pVer[1].tv = 0;
	pVer[2].x = D.x;pVer[2].y = D.y;pVer[2].z = fZ;pVer[2].rhw = 1;pVer[2].color = dwColor;pVer[2].tu = 0;pVer[2].tv = 1;

	pVer[3].x = B.x;pVer[3].y = B.y;pVer[3].z = fZ;pVer[3].rhw = 1;pVer[3].color = dwColor;pVer[3].tu = 1;pVer[3].tv = 0;
	pVer[4].x = D.x;pVer[4].y = D.y;pVer[4].z = fZ;pVer[4].rhw = 1;pVer[4].color = dwColor;pVer[4].tu = 0;pVer[4].tv = 1;
	pVer[5].x = C.x;pVer[5].y = C.y;pVer[5].z = fZ;pVer[5].rhw = 1;pVer[5].color = dwColor;pVer[5].tu = 1;pVer[5].tv = 1;

	m_lpScreenRect->Unlock();

	LPDIRECT3DBASETEXTURE9 pTextureSave = NULL;
	g_pd3dDevice->GetTexture(0, &pTextureSave);
	if(pTextureSave)
		pTextureSave->Release();
	if (!pTexture)
	{
		g_pd3dDevice->SetTexture(0, NULL);
	}
	else
	{
		g_pd3dDevice->SetTexture(0, pTexture);
	}
	g_pd3dDevice->SetTexture(1, NULL);

	RenderState.SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	RenderState.SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_CURRENT);
	RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	RenderState.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);
	RenderState.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	RenderState.SetRenderState(D3DRS_FOGENABLE, FALSE);
	//if ((dwColor & 0xff000000) != 0xff000000)
	{
		RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		RenderState.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	hr = g_pd3dDevice->SetStreamSource( 0, m_lpScreenRect,0, sizeof( _ScreenPoint ) );
	hr = g_pd3dDevice->SetFVF( FVF_SCREEN );
	hr = g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLELIST , 0 ,2 );

	g_pd3dDevice->SetTexture(0, pTextureSave);
	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawScreenRect(const D3DXVECTOR2 *pPointA, 
										 const D3DXVECTOR2 *pPointC,
										 float fZ, 
										 DWORD dwColor)
{
	HRESULT hr = S_OK;
	KG3DRenderState RenderState;
	if( m_lpScreenRect == NULL)
	{
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 6*sizeof( _ScreenPoint ),
			0, FVF_SCREEN,
			D3DPOOL_MANAGED, &m_lpScreenRect,NULL ) ) )
		{
			return E_FAIL;
		}
	}

	D3DXVECTOR2 A,B,C,D;
	A = (*pPointA);
	C = (*pPointC);
	B.x = C.x;B.y = A.y;
	D.x = A.x;D.y = C.y;

	_ScreenPoint * pVer = NULL;
	if( FAILED( m_lpScreenRect->Lock( 0, 0, (void**)&pVer, 0 ) ) )
		return E_FAIL;
	pVer[0].x = B.x;pVer[0].y = B.y;pVer[0].z = fZ;pVer[0].rhw = 1;pVer[0].color = dwColor;pVer[0].tu = 1;pVer[0].tv = 0;
	pVer[1].x = A.x;pVer[1].y = A.y;pVer[1].z = fZ;pVer[1].rhw = 1;pVer[1].color = dwColor;pVer[1].tu = 0;pVer[1].tv = 0;
	pVer[2].x = D.x;pVer[2].y = D.y;pVer[2].z = fZ;pVer[2].rhw = 1;pVer[2].color = dwColor;pVer[2].tu = 0;pVer[2].tv = 1;

	pVer[3].x = B.x;pVer[3].y = B.y;pVer[3].z = fZ;pVer[3].rhw = 1;pVer[3].color = dwColor;pVer[3].tu = 1;pVer[3].tv = 0;
	pVer[4].x = D.x;pVer[4].y = D.y;pVer[4].z = fZ;pVer[4].rhw = 1;pVer[4].color = dwColor;pVer[4].tu = 0;pVer[4].tv = 1;
	pVer[5].x = C.x;pVer[5].y = C.y;pVer[5].z = fZ;pVer[5].rhw = 1;pVer[5].color = dwColor;pVer[5].tu = 1;pVer[5].tv = 1;

	m_lpScreenRect->Unlock();

  	RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);
	RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	RenderState.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	if ((dwColor & 0xff000000) != 0xff000000)
	{
		RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		RenderState.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
    g_pd3dDevice->SetTexture(0,NULL);
	hr = g_pd3dDevice->SetStreamSource( 0, m_lpScreenRect,0, sizeof( _ScreenPoint ) );
	hr = g_pd3dDevice->SetFVF( FVF_SCREEN );
	hr = g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLELIST , 0 ,2 );
	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawFullViewPortRect(DWORD color)
{
    if (m_lpScreenRectXYZW == NULL)
    {
        if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof( _ScreenXYZW ),
            D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, _ScreenXYZW::FVF,
            D3DPOOL_DEFAULT, &m_lpScreenRectXYZW,NULL ) ) )
        {
            return E_FAIL;
        }
    }

    D3DVIEWPORT9 viewPort;
    g_pd3dDevice->GetViewport(&viewPort);

    _ScreenXYZW* pBuffer = NULL;

    if( FAILED( m_lpScreenRectXYZW->Lock( 0, 0, (void**)&pBuffer, D3DLOCK_DISCARD ) ) )
        return E_FAIL;
    
    pBuffer[0].pos = D3DXVECTOR4(-1.f, 1.f, 0.f, 1.f);
    pBuffer[0].color = color;
    pBuffer[0].uv = D3DXVECTOR2(0.f, 0.f);

    pBuffer[1].pos = D3DXVECTOR4(1.f, 1.f, 0.f, 1.f);
    pBuffer[1].color = color;
    pBuffer[1].uv = D3DXVECTOR2(1.f, 0.f);

    pBuffer[2].pos = D3DXVECTOR4(-1.f, -1.f, 0.f, 1.f);
    pBuffer[2].color = color;
    pBuffer[2].uv = D3DXVECTOR2(0.f, 1.f);

    pBuffer[3].pos = D3DXVECTOR4(1.f, -1.f, 0.f, 1.f);
    pBuffer[3].color = color;
    pBuffer[3].uv = D3DXVECTOR2(1.f, 1.f);

    m_lpScreenRectXYZW->Unlock();

    g_pd3dDevice->SetStreamSource( 0, m_lpScreenRectXYZW,0, sizeof( _ScreenXYZW ) );
    g_pd3dDevice->SetFVF(_ScreenXYZW::FVF);
    g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	
	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawScreenRect(const D3DXVECTOR2 *pPointA,
										 const D3DXVECTOR2 *pPointB,
										 const D3DXVECTOR2 *pPointC,
										 const D3DXVECTOR2 *pPointD,
										 float fZ, 
										 DWORD dwColor)
{
	HRESULT hr = S_OK;
	KG3DRenderState RenderState;

	if( m_lpScreenRect == NULL)
	{
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 6*sizeof( _ScreenPoint ),
			0,FVF_SCREEN,
			D3DPOOL_MANAGED, &m_lpScreenRect,NULL ) ) )
		{
			return E_FAIL;
		}
	}

	D3DXVECTOR2 A,B,C,D;
	A = (*pPointA);
	B = (*pPointB);
	C = (*pPointC);
	D = (*pPointD);

	_ScreenPoint * pVer = NULL;
	if( FAILED( m_lpScreenRect->Lock( 0, 0, (void**)&pVer, 0 ) ) )
		return E_FAIL;
	pVer[0].x = B.x;pVer[0].y = B.y;pVer[0].z = fZ;pVer[0].rhw = 1;pVer[0].color = dwColor;pVer[0].tu = 1;pVer[0].tv = 0;
	pVer[1].x = A.x;pVer[1].y = A.y;pVer[1].z = fZ;pVer[1].rhw = 1;pVer[1].color = dwColor;pVer[1].tu = 0;pVer[1].tv = 0;
	pVer[2].x = D.x;pVer[2].y = D.y;pVer[2].z = fZ;pVer[2].rhw = 1;pVer[2].color = dwColor;pVer[2].tu = 0;pVer[2].tv = 1;

	pVer[3].x = B.x;pVer[3].y = B.y;pVer[3].z = fZ;pVer[3].rhw = 1;pVer[3].color = dwColor;pVer[3].tu = 1;pVer[3].tv = 0;
	pVer[4].x = D.x;pVer[4].y = D.y;pVer[4].z = fZ;pVer[4].rhw = 1;pVer[4].color = dwColor;pVer[4].tu = 0;pVer[4].tv = 1;
	pVer[5].x = C.x;pVer[5].y = C.y;pVer[5].z = fZ;pVer[5].rhw = 1;pVer[5].color = dwColor;pVer[5].tu = 1;pVer[5].tv = 1;

	m_lpScreenRect->Unlock();

	RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);
	RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	RenderState.SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	if ((dwColor & 0xff000000) != 0xff000000)
	{
		RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
		RenderState.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA );
		RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	}
	hr = g_pd3dDevice->SetStreamSource( 0, m_lpScreenRect,0, sizeof( _ScreenPoint ) );
	hr = g_pd3dDevice->SetFVF( FVF_SCREEN );
	hr = g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST , 0 ,2);

	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawScreenLine(D3DXVECTOR2 *pPointA,D3DXVECTOR2 *pPointB,float fZ, DWORD dwColor1,DWORD dwColor2)
{
	HRESULT hr = S_OK;
	DWORD dwLightEnable = FALSE;

	if( m_lpScreenRect == NULL)
	{
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 6*sizeof( _ScreenPoint ),
			0,FVF_SCREEN,
			D3DPOOL_MANAGED, &m_lpScreenRect,NULL ) ) )
		{
			return E_FAIL;
		}
	}

	D3DXVECTOR2 A,B;
	A = (*pPointA);
	B = (*pPointB);

	_ScreenPoint * pVer = NULL;
	if( FAILED( m_lpScreenRect->Lock( 0, 0, (void**)&pVer, 0 ) ) )
		return E_FAIL;
	pVer[0].x = A.x;pVer[0].y = A.y;pVer[0].z = fZ;pVer[0].rhw = 1;pVer[0].color = dwColor1;pVer[0].tu = 1;pVer[0].tv = 0;
	pVer[1].x = B.x;pVer[1].y = B.y;pVer[1].z = fZ;pVer[1].rhw = 1;pVer[1].color = dwColor2;pVer[1].tu = 0;pVer[1].tv = 0;

	m_lpScreenRect->Unlock();

	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING, &dwLightEnable);
	GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);

	hr = g_pd3dDevice->SetStreamSource( 0, m_lpScreenRect,0, sizeof( _ScreenPoint ) );
	hr = g_pd3dDevice->SetFVF( FVF_SCREEN );
	hr = g_pd3dDevice->DrawPrimitive  ( D3DPT_LINELIST , 0 ,1 );

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, dwLightEnable);
	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawFrustum(const KG3DFrustum* pFrustum, D3DCOLOR dwColor, DWORD dwPlaneFilter)
{
    // enum FrustumPlane {NEARPLANE = 0,FARPLANE,LEFTPLANE,RIGHTPLANE,TOPPLANE,BOTTOMPLANE};

    if (dwPlaneFilter & 1)
        g_cGraphicsTool.DrawTrapezia(pFrustum->GetPoint(NEAR_LEFT_TOP), pFrustum->GetPoint(NEAR_RIGHT_TOP),
            pFrustum->GetPoint(NEAR_RIGHT_BOTTOM), pFrustum->GetPoint(NEAR_LEFT_BOTTOM), 0xAA000000);

    if (dwPlaneFilter & (1 << 2))
        g_cGraphicsTool.DrawTrapezia(pFrustum->GetPoint(FAR_LEFT_TOP), pFrustum->GetPoint(FAR_RIGHT_TOP),
            pFrustum->GetPoint(FAR_RIGHT_BOTTOM), pFrustum->GetPoint(FAR_LEFT_BOTTOM), 0xAA000000);

    if (dwPlaneFilter & (1 << 3))
        g_cGraphicsTool.DrawTrapezia(pFrustum->GetPoint(FAR_LEFT_TOP), pFrustum->GetPoint(FAR_RIGHT_TOP),
            pFrustum->GetPoint(NEAR_RIGHT_TOP), pFrustum->GetPoint(NEAR_LEFT_TOP), 0xAA000000);
    
    if (dwPlaneFilter & (1 << 4))
        g_cGraphicsTool.DrawTrapezia(pFrustum->GetPoint(FAR_LEFT_BOTTOM), pFrustum->GetPoint(FAR_RIGHT_BOTTOM),
            pFrustum->GetPoint(NEAR_RIGHT_BOTTOM), pFrustum->GetPoint(NEAR_LEFT_BOTTOM), 0xAA000000);

    if (dwPlaneFilter & (1 << 5))
        g_cGraphicsTool.DrawTrapezia(pFrustum->GetPoint(FAR_LEFT_BOTTOM), pFrustum->GetPoint(FAR_LEFT_TOP),
            pFrustum->GetPoint(NEAR_LEFT_TOP), pFrustum->GetPoint(NEAR_LEFT_BOTTOM), 0xAA000000);

    if (dwPlaneFilter & (1 << 6))
        g_cGraphicsTool.DrawTrapezia(pFrustum->GetPoint(FAR_RIGHT_BOTTOM), pFrustum->GetPoint(FAR_RIGHT_TOP),
            pFrustum->GetPoint(NEAR_RIGHT_TOP), pFrustum->GetPoint(NEAR_RIGHT_BOTTOM), 0xAA000000);

	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawAABBox(const AABBOX* pBox,D3DCOLOR dwColor , D3DXMATRIX* pMat,BOOL bZEnable)
{
	D3DXMATRIX   matWorldSave;
	//D3DXMATRIX   matWorld,matscale;
	D3DXVECTOR3  BoxPos[8];
	DWORD  dwZEnable;
	pBox->Extract(BoxPos);

	if (NULL != pMat)
	{
		for (int i =0 ; i < 8 ; i++)
		{
			D3DXVec3TransformCoord(&BoxPos[i],&BoxPos[i],pMat);
		}
	}
	
	//g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
	g_pd3dDevice->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, bZEnable);
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, pMat);
	DrawOBBox(BoxPos,dwColor);
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, dwZEnable);
	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawAABBox(const AABBOX* pBox,D3DCOLOR dwColor)
{

	D3DXVECTOR3  BoxPos[8];
	pBox->Extract(BoxPos);

	DrawOBBox(BoxPos,dwColor);
	return S_OK;
}


HRESULT KG3DGraphicsTool::DrawBoxAtPos( D3DXVECTOR3& vPos, FLOAT fSize, D3DCOLOR color, BOOL bEnableZ /*= TRUE*/)
{
	DWORD dwZEnable;
	g_pd3dDevice->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, bEnableZ ? D3DZB_TRUE : D3DZB_FALSE);

	AABBOX Box(vPos - D3DXVECTOR3(fSize,fSize,fSize), vPos + D3DXVECTOR3(fSize,fSize,fSize));
	HRESULT hr = g_cGraphicsTool.DrawAABBox(&Box, color);
	
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, dwZEnable);
	return hr;
}

HRESULT KG3DGraphicsTool::DrawOBBox(D3DXVECTOR3* BoxPos,D3DCOLOR dwColor)
{
	HRESULT hr = S_OK;
	
    KG3DRenderState R;
	R.SetRenderState(D3DRS_FOGENABLE,FALSE);
	R.SetRenderState( D3DRS_LIGHTING, FALSE );	
	
    g_pd3dDevice->SetTexture(0,NULL);

	struct OBBoxVertex
	{
		D3DXVECTOR3 posi;
		D3DCOLOR    color;
	};

	if( m_lpBox == NULL)
	{
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 24*sizeof(  OBBoxVertex ),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpBox,NULL ) ) )
		{
			return E_FAIL;
		}
	}
	OBBoxVertex* pL  ;
	if( FAILED( m_lpBox->Lock( 0, 0, (void**)&pL, 0 ) ) )
		return E_FAIL;

	pL[0].posi = BoxPos[BOX_A1];pL[1].posi = BoxPos[BOX_B1];pL[0].color=pL[1].color=dwColor;
	pL[2].posi = BoxPos[BOX_B1];pL[3].posi = BoxPos[BOX_C1];pL[2].color=pL[3].color=dwColor;
	pL[4].posi = BoxPos[BOX_C1];pL[5].posi = BoxPos[BOX_D1];pL[4].color=pL[5].color=dwColor;
	pL[6].posi = BoxPos[BOX_D1];pL[7].posi = BoxPos[BOX_A1];pL[6].color=pL[7].color=dwColor;

	pL[8].posi   = BoxPos[BOX_A2];pL[9].posi   = BoxPos[BOX_B2];pL[8].color  =pL[9].color=dwColor;
	pL[10].posi = BoxPos[BOX_B2];pL[11].posi = BoxPos[BOX_C2];pL[10].color=pL[11].color=dwColor;
	pL[12].posi = BoxPos[BOX_C2];pL[13].posi = BoxPos[BOX_D2];pL[12].color=pL[13].color=dwColor;
	pL[14].posi = BoxPos[BOX_D2];pL[15].posi = BoxPos[BOX_A2];pL[14].color=pL[15].color=dwColor;

	pL[16].posi = BoxPos[BOX_A2];pL[17].posi = BoxPos[BOX_A1];pL[16].color=pL[17].color=dwColor;
	pL[18].posi = BoxPos[BOX_B2];pL[19].posi = BoxPos[BOX_B1];pL[18].color=pL[19].color=dwColor;
	pL[20].posi = BoxPos[BOX_C2];pL[21].posi = BoxPos[BOX_C1];pL[20].color=pL[21].color=dwColor;
	pL[22].posi = BoxPos[BOX_D2];pL[23].posi = BoxPos[BOX_D1];pL[22].color=pL[23].color=dwColor;

	m_lpBox->Unlock();

	hr = g_pd3dDevice->SetStreamSource( 0, m_lpBox,0, sizeof(OBBoxVertex ) );
	hr = g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE );
	hr = g_pd3dDevice->DrawPrimitive  ( D3DPT_LINELIST , 0 ,12   );
    hr = g_pd3dDevice->SetStreamSource(0, NULL, 0, 0);

	return S_OK;
}

HRESULT KG3DGraphicsTool::SetCamera( KG3DCamera *pCamera )
{
    //ASSERT( pCamera );
    m_pCamera = pCamera;
    return S_OK;
}

KG3DCamera* KG3DGraphicsTool::GetCamera()
{
 //   ASSERT( m_pCamera );
    return m_pCamera;
}

KG3DCamera* KG3DGraphicsTool::GetPrimaryScenePrimaryCamera() const
{
    if (!g_cEngineManager.GetMainScene())
        return NULL;

    KG3DSceneOutputWnd* pWnd = g_cEngineManager.GetMainScene()->KG3DScene::GetPrimaryWindow();

    if (!pWnd)
        return NULL;

    return &pWnd->GetCamera();
}

DWORD KG3DGraphicsTool::GetNowTime()
{
    return g_cEngineManager.GetTimeControl().GetNowTime();

	/*if(m_pCurScene)
	{
		return m_pCurScene->m_TimeControl.GetNowTime();
	}
	else
	{
		return timeGetTime();
	}*/
}

float KG3DGraphicsTool::GetTinyTime()
{
    return g_cEngineManager.GetTimeControl().GetTinyTime();
	//if(m_pCurScene)
	//{
	//	return m_pCurScene->m_TimeControl.GetTinyTime();
	//}
	//else
	//{
	//	return 1.0F;
	//}
}

DWORD KG3DGraphicsTool::GetTinyTimeOriginal()
{
    return g_cEngineManager.GetTimeControl().GetTinyTimeOriginal();
  /*  if(m_pCurScene)
    {
        return m_pCurScene->m_TimeControl.GetTinyTimeOriginal();
    }
    else
    {
        return 1;
    }*/
}

KG3DRegionInfoManager* KG3DGraphicsTool::GetSceneRegionDWInfoManager()
{
	if (m_pCurScene)
	{
		return m_pCurScene->GetRegionDWInfoManager();
	}
	return NULL;
}
KG3DGPSBase* KG3DGraphicsTool::GetSceneCurGPS()
{
	if (m_pCurScene)
	{
		return m_pCurScene->GetCurGameGPS();
	}
	return NULL;
}
HRESULT KG3DGraphicsTool::DemandFullControlOfFog(KG3DScene& whichScene)
{
	return whichScene.GetEnvironment().GetFog().DemandFullControl();
}

HRESULT KG3DGraphicsTool::ReleaseFullControlOfFog(KG3DScene& whichScene)
{
	return whichScene.GetEnvironment().GetFog().ReleaseFullControl();
}

///</CTH>
HRESULT KG3DGraphicsTool::GetCurCameraData(D3DXVECTOR3* pPos, D3DXVECTOR3* pLookAt)
{
	HRESULT hr = E_FAIL;
	KG3DCamera* pCam = this->GetCamera();
	KG_PROCESS_ERROR(pCam);
	{
		if (NULL != pPos)
		{
			*pPos = pCam->GetPosition();
		}

		if(NULL != pLookAt)
		{
			hr = pCam->GetLookAtPosition(pLookAt);
			KG_COM_PROCESS_ERROR(hr);
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DGraphicsTool::SetCurScene(KG3DScene* pScene)
{
	m_pCurScene = pScene;
	return S_OK;
}
IEKG3DScene* KG3DGraphicsTool::GetScene()
{
	return m_pCurScene;
}
KG3DScene* KG3DGraphicsTool::GetCurScene()
{
	return m_pCurScene;
}
// 填格子
#if 0	//<SO3World Refactor>
HRESULT KG3DGraphicsTool::FillLogicalCellsInfo(
	D3DXVECTOR3 A,int X,int Z,int& nCellCount, KCell* pCell,IELogicalVertex* pLLine, IELogicalVertex* pLFace, BOOL bTerrian,
	BOOL bItem, BOOL bSelected, int nMaxCount)
{
	if(nCellCount >= nMaxCount)
		return E_FAIL;

	float xx = A.x + X * LOGICAL_CELL_CM_LENGTH;
	float zz = A.z + Z * LOGICAL_CELL_CM_LENGTH;
	float y0 = A.y + pCell->m_wLowLayer * (float)ALTITUDE_UNIT + TERRAIN_MIN_HEIGHT;
	y0 = max(-200,y0);
	float y1 = A.y + pCell->m_wHighLayer * (float)ALTITUDE_UNIT+ TERRAIN_MIN_HEIGHT + 1;
	DWORD c3 = 0xFFFFFFFF;
	DWORD c1 = 0xA0FF00FF;
	DWORD c2 = 0xA0FF0000;

	AABBOX Box;
	Box.AddPosition(D3DXVECTOR3(xx,y0,zz));
	Box.AddPosition(D3DXVECTOR3(xx + LOGICAL_CELL_CM_LENGTH, y1, zz + LOGICAL_CELL_CM_LENGTH));
	D3DXVECTOR3 BoxPos[8];
	Box.Extract(BoxPos);

	if (bTerrian)
	{
		// Line
		int nCellCountBase = nCellCount*8;
		pLLine[nCellCountBase+0].posi = BoxPos[BOX_A2];pLLine[nCellCountBase+1].posi = BoxPos[BOX_A1];pLLine[nCellCountBase+0].color=c1;pLLine[nCellCountBase+1].color=c3;
		pLLine[nCellCountBase+2].posi = BoxPos[BOX_B2];pLLine[nCellCountBase+3].posi = BoxPos[BOX_B1];pLLine[nCellCountBase+2].color=c1;pLLine[nCellCountBase+3].color=c3;
		pLLine[nCellCountBase+4].posi = BoxPos[BOX_C2];pLLine[nCellCountBase+5].posi = BoxPos[BOX_C1];pLLine[nCellCountBase+4].color=c1;pLLine[nCellCountBase+5].color=c3;
		pLLine[nCellCountBase+6].posi = BoxPos[BOX_D2];pLLine[nCellCountBase+7].posi = BoxPos[BOX_D1];pLLine[nCellCountBase+6].color=c1;pLLine[nCellCountBase+7].color=c3;

		int nCellCount12 = nCellCount * 12; 
		// Face
		pLFace[nCellCount12 + 0].posi = BoxPos[BOX_A1];
		pLFace[nCellCount12 + 1].posi = BoxPos[BOX_C1];
		pLFace[nCellCount12 + 2].posi = BoxPos[BOX_B1];

		pLFace[nCellCount12 + 3].posi = BoxPos[BOX_A1];
		pLFace[nCellCount12 + 4].posi = BoxPos[BOX_D1];
		pLFace[nCellCount12 + 5].posi = BoxPos[BOX_C1];

		pLFace[nCellCount12 + 6].posi = BoxPos[BOX_A2];
		pLFace[nCellCount12 + 7].posi = BoxPos[BOX_C2];
		pLFace[nCellCount12 + 8].posi = BoxPos[BOX_B2];

		pLFace[nCellCount12 + 9].posi = BoxPos[BOX_A2];
		pLFace[nCellCount12 + 10].posi = BoxPos[BOX_D2];
		pLFace[nCellCount12 + 11].posi = BoxPos[BOX_C2];

		pLFace[nCellCount12 + 0].color = c1;
		pLFace[nCellCount12 + 1].color = c1;
		pLFace[nCellCount12 + 2].color = c1;
		pLFace[nCellCount12 + 3].color = c1;
		pLFace[nCellCount12 + 4].color = c1;
		pLFace[nCellCount12 + 5].color = c1;
		pLFace[nCellCount12 + 6].color = c2;
		pLFace[nCellCount12 + 7].color = c2;
		pLFace[nCellCount12 + 8].color = c2;
		pLFace[nCellCount12 + 9].color = c2;
		pLFace[nCellCount12 + 10].color = c2;
		pLFace[nCellCount12 + 11].color = c2;

		nCellCount++;
	}
	if (bItem && pCell->m_pNext)
	{
		FillLogicalCellsInfo(
			A, X, Z, nCellCount, pCell->m_pNext, pLLine, pLFace, TRUE, TRUE, FALSE, nMaxCount);
	}
	return S_OK;
}
#endif
#if 1
extern const KG3DLogicalConstances& g_GetLogicalConstants();
HRESULT KG3DGraphicsTool::DrawLogicalCells(
	IELogicalSceneConnection* pLogicalScene,int nIndex_X,int nIndex_Z,int nXStart,int nZStart,
	int nLogicalStartX,int nLogicalStartZ,
	BOOL bTerrian, BOOL bItem, BOOL bSelected)
{
	D3DXVECTOR3 vStart;
	HRESULT hr = E_FAIL;
	DWORD Light =0;

	const KG3DLogicalConstances& lgC = g_GetLogicalConstants();

	GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);

	KGLOG_PROCESS_ERROR(pLogicalScene);

	int XX_SceneRegion = nIndex_X - nXStart;
	int ZZ_SceneRegion = nIndex_Z - nZStart;

	int XX = (XX_SceneRegion*16 - nLogicalStartX)/16;
	int ZZ = (ZZ_SceneRegion*16 - nLogicalStartZ)/16;

	if(XX<0 || ZZ<0)
		return S_OK;

	if((XX%2==1)||(ZZ%2==1))
		return S_OK;

	DWORD dwLogicalIndexX = (DWORD)(XX /2);
	DWORD dwLogicalIndexZ = (DWORD)(ZZ /2);

	KRegion* pLogicalRegion = pLogicalScene->GetRegion(dwLogicalIndexX,dwLogicalIndexZ);
	if(!pLogicalRegion)
		return E_FAIL;

	
	vStart = D3DXVECTOR3((FLOAT)dwLogicalIndexX * SCENEREGIONWIDTH * 2.0f, 0, (FLOAT)dwLogicalIndexZ * SCENEREGIONWIDTH*2.0F) 
        + D3DXVECTOR3((FLOAT)nLogicalStartX * 100.0f, 0, (FLOAT)nLogicalStartZ * 100.0f);

	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING,&Light);
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );	
	g_pd3dDevice->SetTexture(0,NULL);

	IELogicalVertex* pL1 = NULL;
	IELogicalVertex* pL2 = NULL;
	IELogicalVertex* pL3 = NULL;
	IELogicalVertex* pL4 = NULL;

	if( m_lpLogicalTerrian == NULL)
	{
		if( FAILED( hr = g_pd3dDevice->CreateVertexBuffer( MAX_LOGIC_TERRIAN_CELL_IN_REGION * 8 * sizeof(IELogicalVertex),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpLogicalTerrian,NULL ) ) )
		{
			return E_FAIL;
		}
	}

	if( m_lpLogicalTerrianFace == NULL)
	{
		if( FAILED( hr = g_pd3dDevice->CreateVertexBuffer( MAX_LOGIC_TERRIAN_CELL_IN_REGION * 12 * sizeof(IELogicalVertex),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpLogicalTerrianFace, NULL ) ) )
		{
			return E_FAIL;
		}
	}

	if (m_lpLogicalItem == NULL)
	{
		if( FAILED( hr = g_pd3dDevice->CreateVertexBuffer( MAX_LOGIC_ITEM_CELL_IN_REGION * 8 * sizeof(IELogicalVertex),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpLogicalItem, NULL ) ) )
		{
			return E_FAIL;
		}
	}

	if (m_lpLogicalItemFace == NULL)
	{
		if( FAILED( hr = g_pd3dDevice->CreateVertexBuffer( MAX_LOGIC_ITEM_CELL_IN_REGION * 12 * sizeof(IELogicalVertex),
			0, D3DFVF_XYZ|D3DFVF_DIFFUSE,
			D3DPOOL_MANAGED, &m_lpLogicalItemFace, NULL ) ) )
		{
			return E_FAIL;
		}
	}

	if( FAILED( m_lpLogicalTerrian->Lock( 0, 0, (void**)&pL1, 0 ) ) )
		return E_FAIL;

	if (FAILED(m_lpLogicalTerrianFace->Lock(0, 0, (void**)&pL2, 0)))
	{
		m_lpLogicalTerrian->Unlock();
		return E_FAIL;
	}

	if (FAILED(m_lpLogicalItem->Lock(0, 0, (void**)&pL3, 0)))
	{
		m_lpLogicalTerrian->Unlock();
		m_lpLogicalTerrianFace->Unlock();
		return E_FAIL;
	}

	if (FAILED(m_lpLogicalItemFace->Lock(0, 0, (void**)&pL4, 0)))
	{
		m_lpLogicalTerrian->Unlock();
		m_lpLogicalTerrianFace->Unlock();
		m_lpLogicalItem->Unlock();
		return E_FAIL;
	}

	int nCount1 = 0;
	int nCount2 = 0;
	for (int z=0;z<lgC.regionGridHeight;z++)
	{
		for (int x=0;x<lgC.regionGridWidth;x++)
		{
			if (nCount1 > MAX_LOGIC_TERRIAN_CELL_IN_REGION || nCount2 > MAX_LOGIC_ITEM_CELL_IN_REGION)
				break;

			if(bTerrian)
			{
                KCell* pCell = pLogicalScene->GetCellOfRegion(pLogicalRegion, (INT)lgC.regionGridWidth * z + x);//&pLogicalRegion->m_pCells[REGION_GRID_WIDTH * z + x];

				hr = pLogicalScene->FillLogicalCellsInfo(
					vStart,x,z,nCount1,pCell,
					pL1, pL2,	TRUE, FALSE, bSelected, MAX_LOGIC_TERRIAN_CELL_IN_REGION);
				if (FAILED(hr))
					break;
			}

			if (bItem)
			{
                KCell* pCell = pLogicalScene->GetCellOfRegion(pLogicalRegion, (INT)lgC.regionGridWidth * z + x);//&pLogicalRegion->m_pCells[REGION_GRID_WIDTH * z + x];

				hr = pLogicalScene->FillLogicalCellsInfo(
					vStart, x, z, nCount2, pCell,
					pL3, pL4, FALSE, TRUE, bSelected, MAX_LOGIC_ITEM_CELL_IN_REGION);
			}
		}
	}
	
 	m_lpLogicalItemFace->Unlock();
 	m_lpLogicalItem->Unlock();
 	m_lpLogicalTerrianFace->Unlock();
 	m_lpLogicalTerrian->Unlock();

	if (nCount1)
	{
		GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
		hr = g_pd3dDevice->SetStreamSource( 0, m_lpLogicalTerrian, 0, sizeof(IELogicalVertex ) );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST , 0 ,4 * nCount1);
		KGLOG_COM_CHECK_ERROR(hr);

		hr = g_pd3dDevice->SetStreamSource( 0, m_lpLogicalTerrianFace, 0, sizeof(IELogicalVertex ) );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0 ,4 * nCount1);
		KGLOG_COM_CHECK_ERROR(hr);
	}

	if (nCount2)
	{
		GraphicsStruct::RenderStateAlpha Alpha(0x20,FALSE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
		hr = g_pd3dDevice->SetStreamSource( 0, m_lpLogicalItem, 0, sizeof(IELogicalVertex ) );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE);
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->DrawPrimitive( D3DPT_LINELIST , 0, 4 * nCount2);
		KGLOG_COM_CHECK_ERROR(hr);

		hr = g_pd3dDevice->SetStreamSource( 0, m_lpLogicalItemFace, 0, sizeof(IELogicalVertex ) );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->SetFVF( D3DFVF_XYZ |D3DFVF_DIFFUSE );
		KGLOG_COM_CHECK_ERROR(hr);
		hr = g_pd3dDevice->DrawPrimitive( D3DPT_TRIANGLELIST, 0 ,4 * nCount2);
		KGLOG_COM_CHECK_ERROR(hr);
	}

	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, Light );	

	return S_OK;
Exit0:
	return E_FAIL;
}

#endif

HRESULT KG3DGraphicsTool::DrawScreenText3D(D3DXVECTOR3& vPos,LPSTR pName,D3DXCOLOR vColor)
{
	D3DVIEWPORT9 viewport;
	D3DXMATRIX matProj,matView,matWorld;
	g_pd3dDevice->GetViewport(&viewport);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matProj);
	g_pd3dDevice->GetTransform(D3DTS_VIEW,&matView);
	g_pd3dDevice->GetTransform(D3DTS_WORLD,&matWorld);


	D3DXVECTOR3 vCenter = vPos;
	D3DXVECTOR3 vScreen;
	D3DXVec3Project(&vScreen,&vCenter,&viewport,&matProj,&matView,&matWorld);

	if(vScreen.z >= 0 && vScreen.z <= 1)
	{
		RECT destRect1;
		SetRect(&destRect1, (int)vScreen.x, (int)vScreen.y, 0, 80);

		LPD3DXFONT pFont = g_cGraphicsTool.GetFont(9);
		if (pFont)
		{
			pFont->DrawTextA(NULL, pName, -1, &destRect1, 
				DT_NOCLIP, vColor);
		}
	}

	return S_OK;
}

HRESULT	KG3DGraphicsTool::DrawSRect(const int Index,const LPDIRECT3DTEXTURE9 Texture)
{
	HRESULT hr = E_FAIL;
	if(Index > 1)
		return S_FALSE;

	if(m_lpSRect[0] == NULL)
	{
		hr = InitVertexBuffer();
		if (FAILED(hr))
		{
			return hr;
		}
	}

	D3DXMATRIX matSaveView,matSaveProj,matView,matProj;
	D3DVIEWPORT9 ViewPort;
	DWORD dwLightEnable;
	D3DXVECTOR3 eye,look,up;

	g_pd3dDevice->GetViewport(&ViewPort);
	g_pd3dDevice->GetTransform(D3DTS_VIEW,&matSaveView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION,&matSaveProj);

//	D3DXMatrixIdentity(&matSaveView);
	eye = D3DXVECTOR3(ViewPort.Width / 2.0f,ViewPort.Height / 2.0f,0.0f);
	look = eye;
	look.z += 1.0f;
	up = D3DXVECTOR3(0.0f,1.0f,0.0f);
	D3DXMatrixLookAtLH(&matView,&eye,&look,&up);
	D3DXMatrixOrthoLH(&matProj,static_cast<float>(ViewPort.Width),static_cast<float>(ViewPort.Height),0.0f,1.0f);

	g_pd3dDevice->SetTransform(D3DTS_VIEW,&matView);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&matProj);

	g_pd3dDevice->GetRenderState(D3DRS_LIGHTING, &dwLightEnable);
//	GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);
	g_pd3dDevice->SetRenderState( D3DRS_LIGHTING, FALSE );
//	g_pd3dDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
	if(Texture != NULL)
		g_pd3dDevice->SetTexture(0,Texture);
	g_pd3dDevice->SetStreamSource( 0, m_lpSRect[Index],0, sizeof( VFormat::_Faces_Texture1 ) );
	g_pd3dDevice->SetFVF( D3DFVF_Faces_Texture1 );
	g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLELIST , 0 ,2 );

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, dwLightEnable);
	g_pd3dDevice->SetTransform(D3DTS_VIEW,&matSaveView);
	g_pd3dDevice->SetTransform(D3DTS_PROJECTION,&matSaveProj);

	return S_OK;
}
HRESULT KG3DGraphicsTool::InitVertexBuffer()
{
	int i;

	SAFE_RELEASE(m_lpSRect[0]);
	SAFE_RELEASE(m_lpSRect[1]);
	if( FAILED( g_pd3dDevice->CreateVertexBuffer(6 * sizeof(VFormat::_Faces_Texture1),
		0, D3DFVF_Faces_Texture1,D3DPOOL_MANAGED, &(m_lpSRect[0]),NULL ) ) )
	{
		return E_FAIL;
	}
	if( FAILED( g_pd3dDevice->CreateVertexBuffer(6 * sizeof(VFormat::_Faces_Texture1),
		0, D3DFVF_Faces_Texture1,D3DPOOL_MANAGED, &(m_lpSRect[1]),NULL ) ) )
	{
		return E_FAIL;
	}
	
	VFormat::_Faces_Texture1 *pvb;

	for(i = 0; i < 2; ++i)
	{
		m_lpSRect[i]->Lock(0,6 * sizeof(VFormat::_Faces_Texture1),(void **)&pvb,0);

		pvb[0].p = D3DXVECTOR3(i * m_fRectSize,m_fRectSize,0.0f);			pvb[0].tu1 = 0.0f;		pvb[0].tv1 = 0.0f;
		pvb[1].p = D3DXVECTOR3((i + 1) * m_fRectSize,m_fRectSize,0.0f);		pvb[1].tu1 = 1.0f;		pvb[1].tv1 = 0.0f;
		pvb[2].p = D3DXVECTOR3((i + 1) * m_fRectSize,0.0f,0.0f);			pvb[2].tu1 = 1.0f;		pvb[2].tv1 = 1.0f;

		pvb[3].p = D3DXVECTOR3((i + 1) * m_fRectSize,0.0f,0.0f);			pvb[3].tu1 = 1.0f;		pvb[3].tv1 = 1.0f;
		pvb[4].p = D3DXVECTOR3(i * m_fRectSize,0.0f,0.0f);					pvb[4].tu1 = 0.0f;		pvb[4].tv1 = 1.0f;
		pvb[5].p = D3DXVECTOR3(i * m_fRectSize,m_fRectSize,0.0f);			pvb[5].tu1 = 0.0f;		pvb[5].tv1 = 0.0f;

		m_lpSRect[i]->Unlock();
	}

	return S_OK;
	
}


HRESULT KG3DGraphicsTool::DrawCube(
     const D3DXVECTOR3 position, const D3DCOLORVALUE color, const float side_len
     )
{
    HRESULT hResult  = E_FAIL;
    HRESULT hRetCode = E_FAIL;

    D3DMATERIAL9 material;
    D3DXMATRIX   matWorldSave;
    D3DXMATRIX   matWorld;
    DWORD        dwZEnable;

    if (!m_pMeshCube)
    {
       hRetCode = D3DXCreateBox(
            g_pd3dDevice,
            side_len,
            side_len,
            side_len,
            &m_pMeshCube,
            NULL
            );
       KG_COM_PROCESS_ERROR(hRetCode);
    }

    ZeroMemory(&material, sizeof(material));

    material.Diffuse = color;
    material.Ambient = color;

    D3DXMatrixIdentity(&matWorld);
    matWorld._41 = position.x;
    matWorld._42 = position.y;
    matWorld._43 = position.z;

    g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
    g_pd3dDevice->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, FALSE);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
    g_pd3dDevice->SetMaterial(&material);
    m_pMeshCube->DrawSubset(0);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);
    g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, dwZEnable);

    hResult = S_OK;
Exit0:
    return hResult;
}

HRESULT KG3DGraphicsTool::DrawSphere(D3DXMATRIX* pMat, const D3DCOLORVALUE color, const float radius )
{
	return DrawSphereEx(pMat,color,radius,FALSE);
}

HRESULT KG3DGraphicsTool::DrawSphereEx(D3DXMATRIX* pMat, const D3DCOLORVALUE color, const float radius,BOOL bZEnable)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	D3DMATERIAL9 material;
	D3DXMATRIX   matWorldSave;
	D3DXMATRIX   matWorld;
	D3DXMATRIX	 matScale;
	DWORD        dwZEnable;

	if (!m_pMeshSphere)
	{
		hRetCode = D3DXCreateSphere(
			g_pd3dDevice, 
			1.0f, 
			16, 
			16, 
			&m_pMeshSphere, 
			NULL
			);
		KG_COM_PROCESS_ERROR(hRetCode);
	}

	ZeroMemory(&material, sizeof(material));

	material.Diffuse = color;
	material.Ambient = color;

	D3DXMatrixScaling(&matScale, radius, radius, radius);
	
	if (pMat)
	{
		matWorld = matScale * (*pMat);
	}
	else
	{
		D3DXMatrixIdentity(&matWorld);
	}

	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
	g_pd3dDevice->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, bZEnable);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->SetMaterial(&material);
	m_pMeshSphere->DrawSubset(0);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, dwZEnable);

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DGraphicsTool::DrawCylinder(D3DXMATRIX* pMat, const D3DCOLORVALUE color,
									   const float radius,
									   const float height
									   )
{
	return DrawCylinderEx(pMat,color,radius,height,FALSE);
}

HRESULT KG3DGraphicsTool::DrawCylinderEx(D3DXMATRIX* pMat, const D3DCOLORVALUE color,
									 const float radius,
									 const float height,
									 BOOL bZEnable
									 )
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	D3DMATERIAL9 material;
	D3DXMATRIX   matWorldSave;
	D3DXMATRIX   matWorld,matscale,matrot,matTrans;
	DWORD        dwZEnable;

	if (!m_pMeshCylinder)
	{
		hRetCode = D3DXCreateCylinder(
			g_pd3dDevice,
			1.0f,
			1.0f,
			1.0f,
			16,
			1,
			&m_pMeshCylinder,
			NULL
			);
		KG_COM_PROCESS_ERROR(hRetCode);
	}

	ZeroMemory(&material, sizeof(material));

	material.Diffuse = color;
	material.Ambient = color;

	D3DXMatrixScaling(&matscale,radius,height,radius);
	D3DXMatrixRotationX(&matrot,D3DX_PI*0.5F);
    D3DXMatrixTranslation(&matTrans, 0.f, height * 0.5f, 0.f);
    

	matWorld =matrot * matscale * (*pMat);

	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
	g_pd3dDevice->GetRenderState(D3DRS_ZENABLE, &dwZEnable);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, bZEnable);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
	g_pd3dDevice->SetMaterial(&material);
	m_pMeshCylinder->DrawSubset(0);
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);
	g_pd3dDevice->SetRenderState(D3DRS_ZENABLE, dwZEnable);

	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KG3DGraphicsTool::DrawPoint(D3DXVECTOR3 O,D3DXCOLOR dwColor)
{
	D3DXVECTOR3 X(10,0,0);
	D3DXVECTOR3 Y(0,10,0);
	D3DXVECTOR3 Z(0,0,10);
	DrawLine(&(O-X),&(O+X),dwColor,dwColor);
	DrawLine(&(O-Y),&(O+Y),dwColor,dwColor);
	DrawLine(&(O-Z),&(O+Z),dwColor,dwColor);
	return S_OK;
}


void KG3DGraphicsTool::DrawFocusRect(const D3DVIEWPORT9& ViewPort, FLOAT weight, DWORD color)
{
    if (!m_lpD3DLine)
        D3DXCreateLine(g_pd3dDevice, &m_lpD3DLine);
   
    if (!m_lpD3DLine)
        return;

    static D3DXVECTOR2 vs[5];

    vs[0] = D3DXVECTOR2(static_cast<FLOAT>(ViewPort.X), static_cast<FLOAT>(ViewPort.Y));
	vs[1] = D3DXVECTOR2(static_cast<FLOAT>(ViewPort.X + ViewPort.Width), static_cast<FLOAT>(ViewPort.Y));
    vs[2] = D3DXVECTOR2(static_cast<FLOAT>(ViewPort.X + ViewPort.Width), static_cast<FLOAT>(ViewPort.Y + ViewPort.Height));
    vs[3] = D3DXVECTOR2(static_cast<FLOAT>(ViewPort.X),  static_cast<FLOAT>(ViewPort.Y + ViewPort.Height));
    vs[4] = D3DXVECTOR2(static_cast<FLOAT>(ViewPort.X), static_cast<FLOAT>(ViewPort.Y));

    m_lpD3DLine->SetWidth(weight);
    m_lpD3DLine->Begin();
    m_lpD3DLine->Draw(vs, 5, color);
    m_lpD3DLine->End();
}




HRESULT KG3DGraphicsTool::DrawCapsule(D3DXMATRIX* pMat, const D3DCOLORVALUE color,
									   const float radius,
									   const float height
									   )
{
	D3DXMATRIX mat,matxx;
	D3DXMatrixTranslation(&mat,0,height/2,0);
	matxx = mat * (*pMat);
	DrawSphere(&matxx,color,radius);

	D3DXMatrixTranslation(&mat,0,-height/2,0);
	matxx = mat * (*pMat);
	DrawSphere(&matxx,color,radius);

	DrawCylinder(pMat,color,radius,height);
	return S_OK;
}

HRESULT KG3DGraphicsTool::DrawLeftHandCoordinate()
{
    HRESULT hResult  = E_FAIL;

    float fFovy;
    float fAspect;
    float fzNear;
    float fzFar;


    D3DXMATRIX   matViewProj;
    D3DXVECTOR3  vLeftNook;
    D3DXVECTOR3  vRight;
    D3DXVECTOR3  vFront;
    D3DXVECTOR3  vUp;
    D3DXVECTOR3  vPosition;
    D3DVIEWPORT9 viewPort;  

    float fVel;
    float fHor;

    KG3DCamera* pCurrCamera = GetCamera();
    KG_PROCESS_ERROR(pCurrCamera);

    vFront = pCurrCamera->GetCameraFront();
    D3DXVec3Normalize(&vFront, &vFront); 

    vRight = pCurrCamera->GetCameraRight();
    D3DXVec3Normalize(&vFront, &vFront);

    vUp = pCurrCamera->GetCameraUp();
    D3DXVec3Normalize(&vUp, &vUp);

    pCurrCamera->GetPosition(&vPosition);
    pCurrCamera->GetGlobalPerspective(
        &fFovy, &fAspect, &fzNear, &fzFar
        );

    matViewProj = pCurrCamera->GetViewMatrix() * pCurrCamera->GetProjectionMatrix();
    g_pd3dDevice->GetViewport(&viewPort);

    fVel = tanf(fFovy / 2) * fzNear - 0.25f;
    fHor = fAspect * fVel;

    vLeftNook = vPosition + vFront * (fzNear + 0.5f) - vRight * fHor - vUp * fVel; 

    {
        D3DXVECTOR3 vY = vLeftNook + D3DXVECTOR3(0.0f, 0.5f, 0.0f);
        D3DXVECTOR3 vZ = vLeftNook + D3DXVECTOR3(0.0f, 0.0f, 0.5f);
        D3DXVECTOR3 vX = vLeftNook + D3DXVECTOR3(0.5f, 0.0f, 0.0f);

        DrawLine(&vLeftNook, &vY, 0xFFFF0000, 0xFFFF0000);
        DrawLine(&vLeftNook, &vZ, 0xFF00FF00, 0xFF00FF00);
        DrawLine(&vLeftNook, &vX, 0xFF0000FF, 0xFF0000FF);

        D3DXVECTOR3 vX2D;
        D3DXVECTOR3 vY2D;
        D3DXVECTOR3 vZ2D;

        vY += D3DXVECTOR3(0.0f,   0.02f,  0.0f);
        vZ += D3DXVECTOR3(0.0f,   0.005f, 0.005f);
        vX += D3DXVECTOR3(0.005f, 0.005f, 0.0f);

        D3DXVec3TransformCoord(&vX2D, &vX, &matViewProj);
        D3DXVec3TransformCoord(&vY2D, &vY, &matViewProj);
        D3DXVec3TransformCoord(&vZ2D, &vZ, &matViewProj);


        RECT destRect1;

        INT X = (INT)(viewPort.Width  * (vX2D.x + 1.0f) / 2); 
        INT Y = (INT)(viewPort.Height * (1.0f - vX2D.y) / 2); 

        SetRect(&destRect1, X, Y, 0, 0);
        GetFont(10)->DrawText(
            NULL, 
            "x",
            -1,
            &destRect1,
            DT_NOCLIP, 
            0xFF0000FF
            );

        X = (INT)(viewPort.Width  * (vY2D.x + 1.0f) / 2); 
        Y = (INT)(viewPort.Height * (1.0f - vY2D.y) / 2); 

        SetRect(&destRect1, X, Y, 0, 0);
        GetFont(10)->DrawText(
            NULL, 
            "y",
            -1,
            &destRect1,
            DT_NOCLIP, 
            0xFFFF0000
            );

        X = (INT)(viewPort.Width  * (vZ2D.x + 1.0f) / 2); 
        Y = (INT)(viewPort.Height * (1.0f - vZ2D.y) / 2); 

        SetRect(&destRect1, X, Y, 0, 0);
        GetFont(10)->DrawText(
            NULL, 
            "z",
            -1,
            &destRect1,
            DT_NOCLIP, 
            0xFF00FF00
            );
    }




    hResult = S_OK;
Exit0:
    return hResult;
}
//<Font>
HRESULT KG3DGraphicsTool::InitFont(int size)
{
	if (this->m_StateContainer.test(emFONT_UNAVAILABLE))
	{
		return E_FAIL;	//无法创建就不一直尝试创建了
	}
    SAFE_RELEASE(m_lpd3dFont);
    //create d3dfont
    HDC hDC;
    HRESULT hr;
    int nHeight;
    int nPointSize = size;
    hDC = GetDC( NULL );
    nHeight = -( MulDiv( nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 ) );

    ReleaseDC( NULL, hDC );

    hr = D3DXCreateFont( g_pd3dDevice, nHeight, 0, FW_NORMAL, 0, FALSE, 
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
        DEFAULT_PITCH | FF_DONTCARE, TEXT("宋体"), 
        &m_lpd3dFont );
	if (FAILED(hr))
	{
		this->m_StateContainer.set(emFONT_UNAVAILABLE);
	}
    return hr;
}

LPD3DXFONT KG3DGraphicsTool::GetFont(int size /* = 9 */)
{
    if(!m_lpd3dFont)
	{
		InitFont(size);	
	}
    return m_lpd3dFont;
}

HRESULT KG3DGraphicsTool::InitFont2(int size)
{
    SAFE_RELEASE(m_lpd3dFont2);
    //create d3dfont
    HDC hDC;
    HRESULT hr;
    int nHeight;
    int nPointSize = size;
    hDC = GetDC( NULL );
    nHeight = -( MulDiv( nPointSize, GetDeviceCaps(hDC, LOGPIXELSY), 72 ) );

    ReleaseDC( NULL, hDC );

    hr = D3DXCreateFont( g_pd3dDevice, nHeight, 0, FW_NORMAL, 0, FALSE, 
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, 
        DEFAULT_PITCH | FF_DONTCARE, TEXT("Fixedsys"), 
        &m_lpd3dFont2 );
    return hr;
}

LPD3DXFONT KG3DGraphicsTool::GetFont2(int size /* = 18 */)
{
    if(!m_lpd3dFont2)
        InitFont2(size);
    return m_lpd3dFont2;
}
//</Font>
HRESULT KG3DGraphicsTool::RecordMovie(LPCTSTR szFilePath, int nIndex)
{
    LPDIRECT3DSURFACE9 lpBackBufface = NULL;
    D3DVIEWPORT9 ViewPort;
    RECT Rect;

    TCHAR szFileName[MAX_PATH];
    sprintf(szFileName, TEXT("%s%d.jpg"), szFilePath, nIndex);

    g_pd3dDevice->GetViewport(&ViewPort);
    g_pd3dDevice->GetRenderTarget(0, &lpBackBufface);

    Rect.left   = ViewPort.X;
    Rect.top    = ViewPort.Y;
    Rect.right  = ViewPort.X + ViewPort.Width;
    Rect.bottom = ViewPort.Y + ViewPort.Height;

    if (lpBackBufface)
    {
        D3DXSaveSurfaceToFile(
            szFileName,
            D3DXIFF_JPG,
            lpBackBufface,
            NULL,
            &Rect);
        lpBackBufface->Release();
    }
    else
        return E_FAIL;
    return S_OK;
}

HRESULT KG3DGraphicsTool::DrawVideoInfo(int x, int y, DWORD color /* = 0xFF0000FF */)
{

    VideoMemoryInfo Info = g_GetUI().GetVideoMemeryInfo();

    D3DVIEWPORT9 ViewPort;
    g_pd3dDevice->GetViewport(&ViewPort);

    TCHAR szInfo[1024];

    RECT rect;
    rect.left  = ViewPort.X + x;
    rect.right = rect.left + ViewPort.Width;
    rect.top   = ViewPort.Y + y;
    rect.bottom = ViewPort.Y + ViewPort.Height;

    sprintf(
        szInfo,
        TEXT( 
        "=======================================================\n"
        "             Video Memory Information\n"
        "  Local Video  : Total %.2fM  Free %.2fM \n"
        "  NonLocal     : Total %.2fM  Free %.2fM \n"
        "  3D Texture   : Total %.2fM  Free %.2fM \n"
        "  3D Rendering : Total %.2fM  Free %.2fM \n"
        "=======================================================\n"
        ),
        (float)Info.dwLocalTatal / (1024.f * 1024.f),
        (float)Info.dwLocalFree / (1024.f * 1024.f),
        (float)Info.dwNonLocalTatal / (1024.f * 1024.f),
        (float)Info.dwNonLocalFree / (1024.f * 1024.f),
        (float)Info.dwTextureTatal / (1024.f * 1024.f),
        (float)Info.dwTexturnFree / (1024.f * 1024.f),
        (float)Info.dwD3DTotal / (1024.f * 1024.f),
        (float)Info.dwD3DFree / (1024.f * 1024.f)
        );

    GetFont2()->DrawText( NULL, szInfo, -1, &rect, DT_NOCLIP, color);
    return S_OK;

}

inline static bool CheckEqual(float x, float y) 
{
	if (abs(x - y) < 0.0001) 
		return true; 
	else return false;
}

inline static bool  CheckEqualVec2(const D3DXVECTOR2& a, const D3DXVECTOR2& b)
{
	if (CheckEqual(a.x, b.x) && CheckEqual(a.y, b.y)) 
		return true; 
	else return false; 
}

inline static bool CheckInBoundary(float fValue, float fMin, float fMax)
{
	if (fValue <= fMax && fValue >= fMin)
		return true;
	return false;
}

bool BBox::IsFaceIntersect(const D3DXVECTOR3& vA, 
						   const D3DXVECTOR3& vB,
						   const D3DXVECTOR3& vC
						   )
{
	float fMinX = min(min(vA.x, vB.x), vC.x);
	float fMaxX = max(max(vA.x, vB.x), vC.x);
	
	if (fMinX > max(A.x, B.x) || fMaxX < min(A.x, B.x))
		return false;

	float fMinY = min(min(vA.y, vB.y), vC.y);
	float fMaxY = max(max(vA.y, vB.y), vC.y);

	if (fMinY > max(A.y, B.y) || fMaxY < min(A.y, B.y))
		return false;

	float fMinZ = min(min(vA.z, vB.z), vC.z);
	float fMaxZ = max(max(vA.z, vB.z), vC.z);
	if (fMinZ > max(A.z, B.z) || fMaxZ < min(A.z, B.z))
		return false;

	if (IsPointInside(vA))
		return true;

	if (IsPointInside(vB))
		return true;

	if (IsPointInside(vC))
		return true;

	
	D3DXPLANE plane;
	D3DXPlaneFromPoints(&plane, &vA, &vB, &vC);


	float fHigherY = max(A.y, B.y);
	float fLowerY = min(A.y, B.y);

	float fNearZ = min(A.z, B.z);
	float fFarZ = max(A.z, B.z);

	float fLeftX = min(A.x, B.x);
	float fRightX = max(A.x, B.x);

	float fAngle = abs(D3DXVec3Dot(&D3DXVECTOR3(plane.a, plane.b, plane.c),
		&D3DXVECTOR3(0, 1.0f, 0)));

	if (!CheckEqual(fAngle, 1.0f))//not a horizontal plane
	{
		//top 
		float fX1 = (-plane.b * fHigherY - plane.c * fNearZ - plane.d) / plane.a;
		if (CheckInBoundary(fX1, fLeftX, fRightX))
			return true;

		float fX2 = (-plane.b * fHigherY - plane.c * fFarZ - plane.d) / plane.a;
		if (CheckInBoundary(fX2, fLeftX, fRightX))
			return true;
		
		float fZ1 = (-plane.b * fHigherY - plane.a * fLeftX - plane.d) / plane.c;
		if (CheckInBoundary(fZ1, fNearZ, fFarZ))
			return true;

		float fZ2 = (-plane.b * fHigherY - plane.a * fRightX - plane.d) / plane.c;
		if (CheckInBoundary(fZ2, fNearZ, fFarZ))
			return true;

		//bottom
		fX1 = (-plane.b * fLowerY - plane.c * fNearZ - plane.d) / plane.a;
		if (CheckInBoundary(fX1, fLeftX, fRightX))
			return true;

		fX2 = (-plane.b * fLowerY - plane.c * fFarZ - plane.d) / plane.a;
		if (CheckInBoundary(fX2, fLeftX, fRightX))
			return true;

		fZ1 = (-plane.b * fLowerY - plane.a * fLeftX - plane.d) / plane.c;
		if (CheckInBoundary(fZ1, fNearZ, fFarZ))
			return true;

		fZ2 = (-plane.b * fLowerY - plane.a * fRightX - plane.d) / plane.c;
		if (CheckInBoundary(fZ2, fNearZ, fFarZ))
			return true;
	}

	fAngle = abs(D3DXVec3Dot(&D3DXVECTOR3(plane.a, plane.b, plane.c),
		&D3DXVECTOR3(1.0f, 0.0f, 0.0f)));

	if (!CheckEqual(fAngle, 1.0f))//not a horizontal plane
	{
		//Left
		float fY1 = (-plane.a * fLeftX - plane.c * fNearZ - plane.d) / plane.b;
		if (CheckInBoundary(fY1, fLowerY, fHigherY))
			return true;

		float fY2 = (-plane.a * fLeftX - plane.c * fFarZ - plane.d) / plane.b;
		if (CheckInBoundary(fY1, fLowerY, fHigherY))
			return true;

		float fZ1 = (-plane.a * fLeftX - plane.b * fLowerY - plane.d) / plane.c;
		if (CheckInBoundary(fZ1, fNearZ, fFarZ))
			return true;

		float fZ2 = (-plane.a * fLeftX - plane.b * fHigherY - plane.d) / plane.c;
		if (CheckInBoundary(fZ1, fNearZ, fFarZ))
			return true;

		//right
		fY1 = (-plane.a * fRightX - plane.c * fNearZ - plane.d) / plane.b;
		if (CheckInBoundary(fY1, fLowerY, fHigherY))
			return true;

		fY2 = (-plane.a * fRightX - plane.c * fFarZ - plane.d) / plane.b;
		if (CheckInBoundary(fY1, fLowerY, fHigherY))
			return true;

		fZ1 = (-plane.a * fRightX - plane.b * fLowerY - plane.d) / plane.c;
		if (CheckInBoundary(fZ1, fNearZ, fFarZ))
			return true;

		fZ2 = (-plane.a * fRightX - plane.b * fHigherY - plane.d) / plane.c;
		if (CheckInBoundary(fZ1, fNearZ, fFarZ))
			return true;
	}


	fAngle = abs(D3DXVec3Dot(&D3DXVECTOR3(plane.a, plane.b, plane.c),
		&D3DXVECTOR3(0.0f, 0.0f, 1.0f)));

	if (!CheckEqual(fAngle, 1.0f))//not a horizontal plane
	{
		//front
		float fY1 = (-plane.c * fNearZ - plane.a * fLeftX - plane.d) / plane.b;
		if (CheckInBoundary(fY1, fLowerY, fHigherY))
			return true;

		float fY2 = (-plane.c * fNearZ - plane.a * fRightX - plane.d) / plane.b;
		if (CheckInBoundary(fY1, fLowerY, fHigherY))
			return true;

		float fX1 = (-plane.c * fNearZ - plane.b * fLowerY - plane.d) / plane.a;
		if (CheckInBoundary(fX1, fLeftX, fRightX))
			return true;

		float fX2 = (-plane.c * fNearZ - plane.b * fHigherY - plane.d) / plane.a;
		if (CheckInBoundary(fX1, fLeftX, fRightX))
			return true;

		//back
		fY1 = (-plane.c * fFarZ - plane.a * fLeftX - plane.d) / plane.b;
		if (CheckInBoundary(fY1, fLowerY, fHigherY))
			return true;

		fY2 = (-plane.c * fFarZ - plane.a * fRightX - plane.d) / plane.b;
		if (CheckInBoundary(fY1, fLowerY, fHigherY))
			return true;

		fX1 = (-plane.c * fFarZ - plane.b * fLowerY - plane.d) / plane.a;
		if (CheckInBoundary(fX1, fLeftX, fRightX))
			return true;

		fX2 = (-plane.c * fFarZ - plane.b * fHigherY - plane.d) / plane.a;
		if (CheckInBoundary(fX1, fLeftX, fRightX))
			return true;
	}
	return false;
}

extern void AlignValDown(float& fVal, float Align);
extern void AlignValUp(float& fVal, float Align);

void KG3DGraphicsTool::RenderMeshLogicalBBox(LogicCellBBox* pBox, int nCellInX, int nCellInY, int nCellInZ)
{
	if (!pBox)
		return;
	VFormat::_Faces_Diffuse_Texture0 Vertices[8];
	short Indices[24] = {
		0, 1, 
		1, 2,
		2, 3, 
		3, 0, 
		
		4, 5,
		5, 6,
		6, 7,
		7, 4,

		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	g_pd3dDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	g_pd3dDevice->SetTexture(0, NULL);
	GraphicsStruct::RenderState AlphaTestEnable(D3DRS_ALPHATESTENABLE, TRUE);
	g_pd3dDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

	for (int i = 0; i < nCellInX; ++i)
    {
        for (int j = 0; j < nCellInY; ++j)
        {
            for (int k = 0; k < nCellInZ; ++k)
            {
                int CellPos = nCellInX * nCellInZ * j + i * nCellInZ + k;
                Vertices[0].p = pBox[CellPos].Box.A;
                Vertices[1].p = D3DXVECTOR3(pBox[CellPos].Box.A.x, pBox[CellPos].Box.A.y, pBox[CellPos].Box.B.z);
                Vertices[2].p = D3DXVECTOR3(pBox[CellPos].Box.B.x, pBox[CellPos].Box.A.y, pBox[CellPos].Box.B.z);
                Vertices[3].p = D3DXVECTOR3(pBox[CellPos].Box.B.x, pBox[CellPos].Box.A.y, pBox[CellPos].Box.A.z);

                Vertices[4].p = D3DXVECTOR3(pBox[CellPos].Box.A.x, pBox[CellPos].Box.B.y, pBox[CellPos].Box.A.z);
                Vertices[5].p = D3DXVECTOR3(pBox[CellPos].Box.A.x, pBox[CellPos].Box.B.y, pBox[CellPos].Box.B.z);
                Vertices[6].p = pBox[CellPos].Box.B;
                Vertices[7].p = D3DXVECTOR3(pBox[CellPos].Box.B.x, pBox[CellPos].Box.B.y, pBox[CellPos].Box.A.z);

                switch(pBox[CellPos].dwType)
                {
                case 1:
                    for (int jj = 0; jj < 8; jj++)
                    {
                        Vertices[jj].diffuse = 0xffff0000;
                    }
                    break;
                case 0:
                    for (int jj = 0; jj < 8; jj++)
                    {
                        Vertices[jj].diffuse = 0xff0000ff;
                    }
                    break;
                case 3:
                    for (int jj = 0; j < 8; jj++)
                    {
                        Vertices[jj].diffuse = 0xffffff00;
                    }
                    break;
                default:
                    continue;
                }

                g_pd3dDevice->DrawIndexedPrimitiveUP(D3DPT_LINELIST, 
                    0, 
                    8,
                    12,
                    Indices,
                    D3DFMT_INDEX16,
                    Vertices,
                    sizeof(VFormat::_Faces_Diffuse_Texture0));

            }
        }
    }
}





enum enuSpecialTextureID
{
	STID_INVALIDATE   = 0xffffffff,
	STID_ERROR        = 0xfffffffe,
	STID_NULL         = 0xfffffffd,
	STID_RENDERTARGET = 0xfffffffc,
};


HRESULT KG3DGraphicsTool::DrawAxis(const D3DXMATRIX& Mat, float fLength)
{
    D3DXVECTOR3 vec3Axis[4] = { D3DXVECTOR3(0.0f, 0.0f, 0.0f),
        D3DXVECTOR3(1.0f, 0.0f, 0.0f), 
        D3DXVECTOR3(0.0f, 1.0f, 0.0f), 
        D3DXVECTOR3(0.0f, 0.0f, 1.0f)};
    D3DXVec3TransformCoord(&vec3Axis[0], &vec3Axis[0], &Mat);
    for (int i = 1; i < 4; i++)
    {
        D3DXVec3TransformNormal(&vec3Axis[i], &vec3Axis[i], &Mat);
    }
   for (int i = 1; i < 4; i++)
    {
        DWORD dwColor = 0xff000000 | ((0x000000ff) << (i - 1) * 8);
        DrawLine(&vec3Axis[0], &(vec3Axis[i] * fLength + vec3Axis[0]), dwColor, dwColor);
    }
    DrawLine(&(D3DXVECTOR3(400, 0, 0)), &(D3DXVECTOR3(400, 0, 400)), 0xff0000ff, 0xff0000ff);
    return S_OK;    
}


enuModelPlayAnimationType KG3DGraphicsTool::GetAnimationTypeByFileName(LPCSTR strFileName)
{
    TCHAR strExt[MAX_PATH];
    enuModelPlayAnimationType ReturnType = MPAT_INVALIDATE;
#ifdef _DEBUG
    KG_PROCESS_ERROR(strFileName);
#endif
    _splitpath_s(strFileName,
        NULL,
        0,
        NULL,
        0,
        NULL,
        0,
        strExt,
        MAX_PATH);
    if (!stricmp(strExt, ".ani"))
        ReturnType = MPAT_NORMAL;
    else if (!stricmp(strExt, ".tani"))
        ReturnType = MPAT_TAGGED;
#ifdef _DEBUG
	Exit0:
#endif
    return ReturnType;
}

HRESULT KG3DGraphicsTool::Render3DInfomation(LPCTSTR pName,D3DXVECTOR3 vPos,D3DVIEWPORT9& viewport,D3DXMATRIX& matView,D3DXMATRIX& matProj ,D3DXMATRIX& matWorld)
{
	D3DXVECTOR3 vScreen;
	D3DXVec3Project(&vScreen,&vPos,&viewport,&matProj,&matView,&matWorld);

	RECT destRect1;
	SetRect(&destRect1, (int)vScreen.x, (int)vScreen.y, 0, 80);

	LPD3DXFONT pFont = g_cGraphicsTool.GetFont();
	if (pFont)
	{
		pFont->DrawTextA(NULL, pName, -1, &destRect1, 
			DT_NOCLIP, D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f));
	}

	return S_OK;
}

HRESULT KG3DGraphicsTool::OnLostDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

	if (m_lpd3dFont)
    {
		hr = m_lpd3dFont->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    if (m_lpd3dFont2)
    {
        hr = m_lpd3dFont2->OnLostDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    SAFE_RELEASE(m_pShareDynamicVertexBuffer512);
    SAFE_RELEASE(m_lpLine);
	SAFE_RELEASE(m_lpTexture);
    SAFE_RELEASE(m_lpRect);

	SAFE_RELEASE(m_lpShareDepthSurface512);
	SAFE_RELEASE(m_lpShareColorSurface512);

    SAFE_RELEASE(m_lpScreenRectXYZW);


	SAFE_RELEASE(m_lpRenderTargetSave);
	SAFE_RELEASE(m_lpRenderTarget0);
	SAFE_RELEASE(m_lpRenderTarget1);

	SAFE_DELETE(m_lpRenderTargetToolForScaleOutPut);

	hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DGraphicsTool::OnResetDevice()
{
    HRESULT hrResult = E_FAIL;
    HRESULT hr = E_FAIL;

	ASSERT(m_lpShareDepthSurface512 == NULL);

    hr = g_pd3dDevice->CreateDepthStencilSurface(512, 512, D3DFMT_D24S8,D3DMULTISAMPLE_NONE, 0, TRUE, &m_lpShareDepthSurface512, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

	ASSERT(m_lpShareColorSurface512 == NULL);

    hr = g_pd3dDevice->CreateRenderTarget(512, 512, D3DFMT_A8R8G8B8, D3DMULTISAMPLE_NONE, 0, FALSE, &m_lpShareColorSurface512, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

	if (m_lpd3dFont)
    {
		hr = m_lpd3dFont->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    if (m_lpd3dFont2)
    {
        hr = m_lpd3dFont2->OnResetDevice();
        KGLOG_COM_PROCESS_ERROR(hr);
    }

    hrResult = S_OK;

Exit0:
    if (FAILED(hrResult))
    {
        SAFE_RELEASE(m_lpShareColorSurface512);
        SAFE_RELEASE(m_lpShareDepthSurface512);
    }
    return hrResult;
}

/*
fTimeStay如果设的话，有两种情况，如果DebugText不存在会自动添加，否则会覆盖。
当最终输出后，发现当前时间-设定这个输出的时间 > fTimeStay的话，这个输出会被删除
这样就可以用于在一段时间内输出一个文字
所以这个函数如果fTimeStay设的很大的话，就可以恒定输出。而只要保存返回的Handle，下次用在fTimeStay
的话，就可以直接改上次要求输出的Text，这样就可以在屏幕上恒定输出位置。
*/
HRESULT KG3DGraphicsTool::DrawDebugText(LPCTSTR lpText, INT nTextLength /*= -1*/, DWORD* pdwHandle/* = NULL*/, D3DCOLOR Color/* = 0xff000000*/, DWORD dwTimeStay/* = 0*/, INT nLine/* = -1*/, UINT nWhichFont/* = 0*/ )
{
    static DWORD s_dwNextHandle = 0;
    DEBUG_TEXT debugText;

 	KG_PROCESS_ERROR(lpText && nTextLength >= -1 && nLine >= -1);

	if (m_vecDebugText.size() > 30)
	{
		_KG3D_DEBUG_REPORT("KG3DGraphicsTool::DrawDebugText：太多Text了，要检查");
		goto Exit0;
	}
	
    debugText.Color        = Color;
    debugText.dwRemoveTime = this->GetNowTime() + dwTimeStay;
	if (nTextLength == -1)
		debugText.stText = lpText;
	else
		debugText.stText.assign(lpText, nTextLength);


    if (pdwHandle) // 修改指定位置的文字
    {
        for (size_t i = 0; i < m_vecDebugText.size(); ++i)
        {
            if (m_vecDebugText[i].dwHandle == *pdwHandle)
            {
                debugText.dwHandle  = *pdwHandle;
                m_vecDebugText[i]   = debugText;
                KG_PROCESS_SUCCESS(true);
            }
        }
    }

    if (nLine == -1 || (size_t)nLine >= m_vecDebugText.size())//追加
	{
        debugText.dwHandle = s_dwNextHandle++;
		m_vecDebugText.push_back(debugText);
	}
	else //在特定行插入
	{
		int nLineCount = 0;
		for (DEBUG_TEXT_VECTOR::iterator it = m_vecDebugText.begin(); it != m_vecDebugText.end(); ++it)
		{
			if (nLineCount == nLine)
			{
                debugText.dwHandle = s_dwNextHandle++;
                m_vecDebugText.push_back(debugText);
				break;
			}
            nLineCount++;
		}
	}

Exit1:
    if (pdwHandle)
        *pdwHandle = debugText.dwHandle;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DGraphicsTool::DrawAllDebugTextsAtLast()
{
    INT nFontHeight = font1_size;
    RECT RectForDraw = {0, 125, 0, 0};
    DEBUG_TEXT_VECTOR::iterator it;
	DWORD dwNowTime = this->GetNowTime();

	KG_PROCESS_SUCCESS(m_vecDebugText.empty());
	
    if (!m_lpd3dFont)
	{
		InitFont(font1_size);
	}
	KG_PROCESS_ERROR(m_lpd3dFont);

	it = m_vecDebugText.begin();
    while (it != m_vecDebugText.end())
	{
		DEBUG_TEXT& debugText = *it;
		std::tstring& stText = debugText.stText;
		m_lpd3dFont->DrawText(NULL, stText.c_str(), (INT)stText.length(), &RectForDraw, DT_NOCLIP, debugText.Color);
		RectForDraw.top += (nFontHeight+10);
		
		if (debugText.dwRemoveTime <= dwNowTime)
		{
			it = m_vecDebugText.erase(it);
		}
		else
		{
			++it;
		}
	}		
Exit1:	
	return S_OK;
Exit0:	
	return E_FAIL;
}

HRESULT KG3DGraphicsTool::FlushDebugTexts()
{
    DEBUG_TEXT_VECTOR::iterator it;
	DWORD dwNowTime = this->GetNowTime();
	
    KG_PROCESS_SUCCESS(m_vecDebugText.empty());

	it = m_vecDebugText.begin();
    while (it != m_vecDebugText.end())
	{
		if (it->dwRemoveTime <= dwNowTime)
		{
			it = m_vecDebugText.erase(it);
		}
		else
		{
			++it;
		}
	}
Exit1:
	return S_OK;
}

HRESULT KG3DGraphicsTool::DeleteDebugText(DWORD dwHandle)
{
    for (DEBUG_TEXT_VECTOR::iterator it = m_vecDebugText.begin(); it != m_vecDebugText.end(); ++it)
    {
        if (it->dwHandle == dwHandle)
        {
            m_vecDebugText.erase(it);
            return S_OK;
        }
    }
	return E_FAIL;
}

KG3DCamera* KG3DGraphicsTool::GetSceneMainCamera()
{
	KG_PROCESS_ERROR(m_pCurScene);
	{
		KG3DSceneOutputWnd* pMainWnd = m_pCurScene->GetPrimaryWindow();
		if (pMainWnd)
		{
			return &pMainWnd->GetCamera();
		}
	}
Exit0:
	return NULL;
}

HRESULT KG3DGraphicsTool::GetSceneMainCamera( IEKG3DCamera** pRet )
{
	if (pRet)
	{
		KG3DCamera* pCam = this->GetSceneMainCamera();
		if (pCam)
		{
			*pRet = pCam;
			return S_OK;
		}
	}
	return E_FAIL;
}

HRESULT KG3DGraphicsTool::DrawPainScreenRect()
{
	return E_FAIL;//暂时不写
}

HRESULT KG3DGraphicsTool::DrawBackGroudTexture(LPDIRECT3DTEXTURE9 pTexture)
{
	HRESULT hr = E_FAIL;
	RECT rect;
	struct BackGroundTexVertex
	{
		float x, y, z,rhw; //顶点坐标
		float u,v;  //顶点的纹理坐标 
	};
	IEKG3DSceneOutputWnd* pWnd = NULL;
	if(!m_pCurScene)
		return E_FAIL;
	m_pCurScene->GetCurOutputWnd(&pWnd);
	if(!pWnd)
		return E_FAIL;
	pWnd->GetRect(&rect);
	float fWidth = (float)(rect.right - rect.left);
	float fHeight =(float)( rect.bottom - rect.top);
	BackGroundTexVertex vertex[] = {
		{0.0f,0.0f,0.0f,1.0f,0.0f,0.0f}, 
		{fWidth,0.0f,0.0f,1.0f,1.0f,0.0f}, 
		{0.0f,fHeight,0.0f,1.0f,0.0f,1.0f},
		{fWidth,fHeight,0.0f,1.0f,1.0f,1.0f}
	};


	if( m_lpTexture == NULL)
	{
		//创建顶点缓冲区
		if( FAILED( g_pd3dDevice->CreateVertexBuffer( 4*sizeof(  BackGroundTexVertex ),
			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW|D3DFVF_TEX1,
			D3DPOOL_DEFAULT, &m_lpTexture,NULL ) ) )
		{
			return E_FAIL;
		}
	}
	BYTE* pVertexData;
	//取得顶点缓冲区的指针
	if( FAILED( m_lpTexture->Lock(0, 0, (void**)&pVertexData, D3DLOCK_DISCARD) ) )
		return E_FAIL;
	//写入顶点数据到顶点缓冲区
	memcpy(pVertexData, vertex, sizeof(BackGroundTexVertex)*4);
	m_lpTexture->Unlock();
	KG3DRenderState RenderState;
	RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);

	g_pd3dDevice->SetTexture(0,pTexture);

	hr = g_pd3dDevice->SetStreamSource( 0, m_lpTexture,0, sizeof(BackGroundTexVertex ) );
	hr = g_pd3dDevice->SetFVF( D3DFVF_XYZRHW|D3DFVF_TEX1 );
	hr = g_pd3dDevice->DrawPrimitive  ( D3DPT_TRIANGLESTRIP,0, 2  );

	RenderState.SetRenderState(D3DRS_LIGHTING, TRUE);

	return S_OK;
}

HRESULT	KG3DGraphicsTool::SetFogParam(KG3DScene& Scene, const KG3DFogParam& fogParam)
{
	return Scene.GetEnvironment().GetFog().SetFogParam(fogParam);	
}

KG3DFogParam KG3DGraphicsTool::GetFogParam( KG3DScene& Scene )
{
	return Scene.GetEnvironment().GetFog().GetFogParam();
}
HRESULT KG3DGraphicsTool::SetFogDistancePercent( KG3DScene& Scene, FLOAT fPercent )
{
	return Scene.GetEnvironment().GetFog().SetDistancePercent(fPercent);
}

ULONG KG3DGraphicsTool::IsFogUnderFullControl(KG3DScene& Scene)
{
	return Scene.GetEnvironment().GetFog().IsInFullControl();
}
KG3DLight& KG3DGraphicsTool::GetSunLight( KG3DScene& Scene )
{
	return Scene.GetEnvironment().GetLightSet().GetSunLight();
}
const KG3DLightParam& KG3DGraphicsTool::GetSunLightParam( KG3DScene& Scene )
{
	return GetSunLight(Scene).GetParam();
}
KG3DLight& KG3DGraphicsTool::GetPlayerLight( KG3DScene& Scene )
{
	return Scene.GetEnvironment().GetLightSet().GetPlayerLight();
}
const KG3DLightParam&  KG3DGraphicsTool::GetPlayerLightParam( KG3DScene& Scene )
{
	return GetPlayerLight(Scene).GetParam();
}
KG3DLightSet& KG3DGraphicsTool::GetLightSet( KG3DScene& Scene )
{
	return Scene.GetEnvironment().GetLightSet();
}

HRESULT KG3DGraphicsTool::ApplyLight( KG3DScene& Scene )
{
	return Scene.GetEnvironment().GetLightSet().Apply();
}
HRESULT KG3DGraphicsTool::SetSunLightParam( KG3DScene& Scene, const KG3DLightParam& lightParam )
{
	return Scene.GetEnvironment().GetLightSet().GetSunLight().SetParam(lightParam);
}

HRESULT KG3DGraphicsTool::SetPlayerLightParam( KG3DScene& Scene, const KG3DLightParam& lightParam )
{
	return Scene.GetEnvironment().GetLightSet().GetPlayerLight().SetParam(lightParam);
}

HRESULT KG3DD3DXLoadMeshFromX(LPCSTR pFilename, 
                              DWORD Options, 
                              LPDIRECT3DDEVICE9 pD3DDevice, 
                              LPD3DXBUFFER *ppAdjacency,
                              LPD3DXBUFFER *ppMaterials, 
                              LPD3DXBUFFER *ppEffectInstances, 
                              DWORD *pNumMaterials,
                              LPD3DXMESH *ppMesh)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IFile* pFile = NULL;
    UINT uDataLength = 0;
    LPBYTE pDataBuffer = NULL;

    ASSERT(pD3DDevice);

    KGLOG_PROCESS_ERROR(pFilename);

    pFile = g_OpenFile(pFilename);
    KGLOG_PROCESS_ERROR(pFile);

    uDataLength = (UINT)pFile->Size();

    pDataBuffer = (LPBYTE)malloc(uDataLength);
    KGLOG_PROCESS_ERROR(pDataBuffer);

    nRetCode = pFile->Read(pDataBuffer, uDataLength) == uDataLength;
    KGLOG_PROCESS_ERROR(nRetCode);

    hr = D3DXLoadMeshFromXInMemory(pDataBuffer, uDataLength, Options, pD3DDevice, ppAdjacency, ppMaterials, ppEffectInstances, pNumMaterials,ppMesh);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    SAFE_FREE(pDataBuffer);
    KG_COM_RELEASE(pFile);

    if (FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_ERR, "KG3DD3DXLoadMeshFromX(%s)\n", pFilename);
    }
    return hrResult;
}


HRESULT KG3DD3DXCreateEffectFromFile(LPDIRECT3DDEVICE9 pDevice,
                                     LPCTSTR pSrcFile,
                                     const D3DXMACRO *pDefines,
                                     LPD3DXINCLUDE pInclude,
                                     DWORD Flags,
                                     LPD3DXEFFECTPOOL pPool,
                                     LPD3DXEFFECT *ppEffect,
                                     LPD3DXBUFFER *ppCompilationErrors)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IFile* pFile = NULL;
    UINT uDataLength = 0;
    LPBYTE pDataBuffer = NULL;

    ASSERT(pDevice);

    KGLOG_PROCESS_ERROR(pSrcFile);

    pFile = g_OpenFile(pSrcFile);
    KGLOG_PROCESS_ERROR(pFile);

    uDataLength = (UINT)pFile->Size();

    pDataBuffer = (LPBYTE)malloc(uDataLength);
    KGLOG_PROCESS_ERROR(pDataBuffer);

    nRetCode = (pFile->Read(pDataBuffer, uDataLength) == uDataLength);
    KGLOG_PROCESS_ERROR(nRetCode);

    hr = D3DXCreateEffect(pDevice, pDataBuffer, uDataLength, pDefines, pInclude, Flags, pPool, ppEffect, ppCompilationErrors);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    SAFE_FREE(pDataBuffer);
    KG_COM_RELEASE(pFile);

    if (FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_ERR, "KG3DD3DXCreateEffectFromFile(%s)\n", pSrcFile);
    }
    return hrResult;
}


HRESULT KG3DD3DXCreateTextureFromFile(LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, LPDIRECT3DTEXTURE9* ppTexture)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IFile* pFile = NULL;
    UINT uDataLength = 0;
    LPBYTE pDataBuffer = NULL;
	char szFile[MAX_PATH];

    ASSERT(pDevice);

    KGLOG_PROCESS_ERROR(pSrcFile);

	strcpy(szFile, pSrcFile);

	if (!g_IsFileExist(szFile))
	{
		size_t nLen = strlen(szFile);
		KGLOG_PROCESS_ERROR(nLen > 3);

		szFile[nLen - 3] = 0;
		strcat(szFile, "dds");
	}

    pFile = g_OpenFile(szFile);
    KGLOG_PROCESS_ERROR(pFile);

    uDataLength = (UINT)pFile->Size();

    pDataBuffer = (LPBYTE)malloc(uDataLength);
    KGLOG_PROCESS_ERROR(pDataBuffer);

    nRetCode = pFile->Read(pDataBuffer, uDataLength) == uDataLength;
    KGLOG_PROCESS_ERROR(nRetCode);

    hr = D3DXCreateTextureFromFileInMemory(pDevice, pDataBuffer, uDataLength, ppTexture);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    SAFE_FREE(pDataBuffer);
    KG_COM_RELEASE(pFile);

    if (FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_ERR, "KG3DD3DXCreateTextureFromFile(%s)\n", szFile);
    }
    return hrResult;
}


HRESULT KG3DD3DXGetImageInfoFromFile(LPCSTR pSrcFile, D3DXIMAGE_INFO *pSrcInfo)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IFile* pFile = NULL;
    UINT uDataLength = 0;
    LPBYTE pDataBuffer = NULL;

    KGLOG_PROCESS_ERROR(pSrcFile);

    pFile = g_OpenFile(pSrcFile);
    KGLOG_PROCESS_ERROR(pFile);

    uDataLength = (UINT)pFile->Size();

    pDataBuffer = (LPBYTE)malloc(uDataLength);
    KGLOG_PROCESS_ERROR(pDataBuffer);

    nRetCode = pFile->Read(pDataBuffer, uDataLength) == uDataLength;
    KGLOG_PROCESS_ERROR(nRetCode);

    hr = D3DXGetImageInfoFromFileInMemory(pDataBuffer, uDataLength, pSrcInfo);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    SAFE_FREE(pDataBuffer);
    KG_COM_RELEASE(pFile);

    //if (FAILED(hrResult))
    //   KGLogPrintf(KGLOG_ERR, "KG3DD3DXGetImageInfoFromFile(%s)\n", pSrcFile);
    return hrResult;
}


HRESULT KG3DD3DXCreateCubeTextureFromFile(LPDIRECT3DDEVICE9 pDevice, LPCSTR pSrcFile, LPDIRECT3DCUBETEXTURE9* ppCubeTexture)
{
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    IFile* pFile = NULL;
    UINT uDataLength = 0;
    LPBYTE pDataBuffer = NULL;

    ASSERT(pDevice);

    KGLOG_PROCESS_ERROR(pSrcFile);

    pFile = g_OpenFile(pSrcFile);
    KGLOG_PROCESS_ERROR(pFile);

    uDataLength = (UINT)pFile->Size();

    pDataBuffer = (LPBYTE)malloc(uDataLength);
    KGLOG_PROCESS_ERROR(pDataBuffer);

    nRetCode = pFile->Read(pDataBuffer, uDataLength) == uDataLength;
    KGLOG_PROCESS_ERROR(nRetCode);

    hr = D3DXCreateCubeTextureFromFileInMemory(pDevice, pDataBuffer, uDataLength, ppCubeTexture);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    SAFE_FREE(pDataBuffer);
    KG_COM_RELEASE(pFile);

    if (FAILED(hrResult))
    {
        KGLogPrintf(KGLOG_ERR, "KG3DD3DXCreateCubeTextureFromFile(%s)\n", pSrcFile);
    }
    return hrResult;
}

LPDIRECT3DVERTEXDECLARATION9 KG3DGraphicsTool::GetGrassVertexDec()
{
	HRESULT hr = E_FAIL;
	if(!m_lpGrassDeclaration)
	{
		D3DVERTEXELEMENT9 Declaration2[] = 
		{
			{0, 0 , D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
			{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL  , 0},
			{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
			D3DDECL_END()
		};
		hr = g_pd3dDevice->CreateVertexDeclaration(Declaration2, &m_lpGrassDeclaration);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	return m_lpGrassDeclaration;
Exit0:
	return NULL;
}

LPDIRECT3DSURFACE9 KG3DGraphicsTool::GetShareDepthSurface512()
{
    return m_lpShareDepthSurface512;
}

bool KG3DGraphicsTool::IsCurrRender2MainWnd()
{
    KG3DScene* pScene = GetCurScene();

    if (!pScene)
        return false;

    KG3DSceneOutputWnd* pWnd = NULL;

    pScene->GetCurRenderWnd((IEKG3DSceneOutputWnd**)&pWnd);

    if (!pWnd)
        return false;

    return pWnd->m_nID == OUTPUTWND_SCENEEDITOR_MAIN;

    
}

LPDIRECT3DSURFACE9 KG3DGraphicsTool::GetShareColorSurface512()
{
	return m_lpShareColorSurface512;
}

KG3DTerrainConverMap* KG3DGraphicsTool::GetCurrentConverMap()
{
	return m_lpCurrentConverMap;
}

HRESULT KG3DGraphicsTool::SetCurrentConverMap(KG3DTerrainConverMap* pMap)
{
	m_lpCurrentConverMap = pMap;
	return S_OK;
}

//KG3DSceneShadowMap* KG3DGraphicsTool::GetCurrentShadowMap()
//{
//	return m_lpCurrentShadowMap;
//}
//
//HRESULT KG3DGraphicsTool::SetCurrentShadowMap(KG3DSceneShadowMap* pMap)
//{
//	m_lpCurrentShadowMap = pMap;
//	return S_OK;
//}

LPDIRECT3DVOLUMETEXTURE9 KG3DGraphicsTool::GetShareNoiseTexture()
{
	if(!m_lpTextureNoise3D)
	{
		D3DXCreateVolumeTextureFromFile(g_pd3dDevice,"Data\\Public\\noiseL8_32x32x32.dds",&m_lpTextureNoise3D);
	}

	return m_lpTextureNoise3D;
}

LPDIRECT3DTEXTURE9 KG3DGraphicsTool::GetSkinTexture()
{
	if(!m_lpTextureSkin)
	{
		g_cTextureTable.LoadResourceFromFile("Data\\public\\Skin.tga",0,0,(IKG3DResourceBase**)&m_lpTextureSkin);
	}

	if(m_lpTextureSkin)
	{
		return m_lpTextureSkin->GetTexture();
	}

	return NULL;
}
HRESULT KG3DGraphicsTool::SetRenderTarget(LPDIRECT3DSURFACE9 pSurface0,LPDIRECT3DSURFACE9 pSurface1)
{
	_ASSERTE((NULL != pSurface0 && NULL != pSurface1) 
		|| (NULL == pSurface0 && NULL == pSurface1));//必须一起设，按照代码的理解是这样的。这里别随便改，最好整个组探讨一下，不然一改就出事。

	SaveDefaultRenderTarget();//这句话没有地方放，N多地方都在修改Surface。只能默认这个地方RenderTarget是正确的。

	//严格遵循Com的入参出参规范，不容易出错
	SAFE_RELEASE(m_lpRenderTarget0);
	m_lpRenderTarget0 = pSurface0;
	if(NULL != pSurface0)
		pSurface0->AddRef();
	
	SAFE_RELEASE(m_lpRenderTarget1);
	m_lpRenderTarget1 = pSurface1;
	if(NULL != pSurface1)
		pSurface1->AddRef();
	return S_OK;
}

HRESULT KG3DGraphicsTool::EnableRenderTarget(int nRenderTarget)
{
    D3DVIEWPORT9 ViewPort;
    g_pd3dDevice->GetViewport(&ViewPort);

	_ASSERTE(0 == nRenderTarget || 1 == nRenderTarget);
	{
		if(nRenderTarget==0)
		{
			KG_PROCESS_ERROR(NULL != m_lpRenderTarget0);
			g_pd3dDevice->SetRenderTarget(0,m_lpRenderTarget0);
		}
		else if(nRenderTarget==1)
		{
			KG_PROCESS_ERROR(NULL != m_lpRenderTarget1);
			g_pd3dDevice->SetRenderTarget(0,m_lpRenderTarget1);
		}
        g_pd3dDevice->SetViewport(&ViewPort);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DGraphicsTool::DisableRenderTarget()
{
	if(NULL != m_lpRenderTargetSave)
	{
		g_pd3dDevice->SetRenderTarget(0,m_lpRenderTargetSave);
	}
	return S_OK;
}

HRESULT KG3DGraphicsTool::SaveDefaultRenderTarget()
{
	SAFE_RELEASE(m_lpRenderTargetSave);
	g_pd3dDevice->GetRenderTarget(0, &m_lpRenderTargetSave);
	return S_OK;
}

KG3DLightParam KG3DGraphicsTool::GetDefaultLightParam()
{
	return KG3DLight::GetDefaultLightParam();
}

KG3DFogParam KG3DGraphicsTool::GetDefaultFogParam()
{
	return KG3DFog::GetDefaultFogParam();
}

KG3DRenderTargetTool* KG3DGraphicsTool::GetScaleOutputRenderTool()
{
	HRESULT hr = E_FAIL;

	if(!m_lpRenderTargetToolForScaleOutPut)
	{
		m_lpRenderTargetToolForScaleOutPut = new KG3DRenderTargetTool;
		KGLOG_PROCESS_ERROR(m_lpRenderTargetToolForScaleOutPut);

		hr = m_lpRenderTargetToolForScaleOutPut->Init(g_nScaleOutputSize,TRUE,D3DFMT_A8R8G8B8);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

Exit0:
	return m_lpRenderTargetToolForScaleOutPut;
}

extern int JITTER_SIZE;
extern int JITTER_SAMPLES;

HRESULT KG3DGraphicsTool::InitJitter(LPDIRECT3DVOLUMETEXTURE9* lppJitterTexture)
{
	HRESULT hr = E_FAIL;
	HRESULT hrResult = E_FAIL;
	D3DLOCKED_BOX lb;
	unsigned char *data = NULL;
	BOOL bLocked = FALSE;

	LPDIRECT3DVOLUMETEXTURE9 m_lpJitterTexture = NULL;

	SAFE_RELEASE(m_lpJitterTexture);
	ASSERT(m_lpJitterTexture == NULL);

	hr = g_pd3dDevice->CreateVolumeTexture(JITTER_SIZE, JITTER_SIZE, JITTER_SAMPLES*JITTER_SAMPLES / 2, 1, 
		D3DUSAGE_DYNAMIC, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &m_lpJitterTexture, NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = m_lpJitterTexture->LockBox(0, &lb, NULL, 0);
	KGLOG_COM_PROCESS_ERROR(hr);

	bLocked = TRUE;

	data = (unsigned char *)lb.pBits;

	for (int i = 0; i<JITTER_SIZE; i++)
	{
		for (int j = 0; j<JITTER_SIZE; j++)
		{
			float rot_offset = ((float)rand() / RAND_MAX - 1) * 2.0f * D3DX_PI;

			for (int k = 0; k<JITTER_SAMPLES*JITTER_SAMPLES/2; k++)
			{
				int x, y;
				float v[4];

				x = k % (JITTER_SAMPLES / 2);
				y = (JITTER_SAMPLES - 1) - k / (JITTER_SAMPLES / 2);

				v[0] = (float)(x * 2 + 0.5f) / JITTER_SAMPLES;
				v[1] = (float)(y + 0.5f) / JITTER_SAMPLES;
				v[2] = (float)(x * 2 + 1 + 0.5f) / JITTER_SAMPLES;
				v[3] = v[1];

				// jitter
				v[0] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;
				v[1] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;
				v[2] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;
				v[3] += ((float)rand() * 2 / RAND_MAX - 1) / JITTER_SAMPLES;

				// warp to disk
				float d[4];
				d[0] = sqrtf(v[1]) * cosf(2.0f * D3DX_PI * v[0] + rot_offset);
				d[1] = sqrtf(v[1]) * sinf(2.0f * D3DX_PI * v[0] + rot_offset);
				d[2] = sqrtf(v[3]) * cosf(2.0f * D3DX_PI * v[2] + rot_offset);
				d[3] = sqrtf(v[3]) * sinf(2.0f * D3DX_PI * v[2] + rot_offset);

				d[0] = (d[0] + 1.0f) / 2.0f;
				data[k*lb.SlicePitch + j*lb.RowPitch + i*4 + 0] = (unsigned char)(d[0] * 255);
				d[1] = (d[1] + 1.0f) / 2.0f;
				data[k*lb.SlicePitch + j*lb.RowPitch + i*4 + 1] = (unsigned char)(d[1] * 255);
				d[2] = (d[2] + 1.0f) / 2.0f;
				data[k*lb.SlicePitch + j*lb.RowPitch + i*4 + 2] = (unsigned char)(d[2] * 255);
				d[3] = (d[3] + 1.0f) / 2.0f;
				data[k*lb.SlicePitch + j*lb.RowPitch + i*4 + 3] = (unsigned char)(d[3] * 255);
			}
		}
	}

	hrResult = S_OK;
	(*lppJitterTexture) = m_lpJitterTexture;
Exit0:
	if (bLocked)
	{
		hr = m_lpJitterTexture->UnlockBox(0);
		KGLOG_COM_CHECK_ERROR(hr);
	}
	if (FAILED(hrResult))
	{
		VERIFY_RELEASE(m_lpJitterTexture);
	}
	return hrResult;
}