#include "StdAfx.h"
#include "KG3DFrustum.h"
#include "KG3DGraphicsTool.h"
#include "KG3DRepresentObjectPVS.h"
//#include "MathTool.h"


#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

#define	shuf_01_01	 68	// 01000100
#define	shuf_23_23  238 // 11101110

_declspec (align(16)) const float Zero[] = {0.0f, 0.0f, 0.0f, 0.0f};

extern KG3DGraphicsTool g_cGraphicsTool;

D3DXVECTOR3	KG3DFrustum::m_stVerticesInPPSpace[8] = 
{
	D3DXVECTOR3(-1.0,-1.0,0.0)
	,D3DXVECTOR3(1.0,-1.0,0.0)
	,D3DXVECTOR3(-1.0,1.0,0.0)
	,D3DXVECTOR3(1.0,1.0,0.0)

	,D3DXVECTOR3(-1.0,-1.0,1.0)
	,D3DXVECTOR3(1.0,-1.0,1.0)
	,D3DXVECTOR3(-1.0,1.0,1.0)
	,D3DXVECTOR3(1.0,1.0,1.0)

};

KG3DFrustum::KG3DFrustum()
{	
	DWORD KG3DFrustum::* p = &KG3DFrustum::m_dwState;
	INT nGap = *(INT*)(&p);
	_ASSERTE((char*)this + nGap == (void*)&m_dwState);
	ZeroMemory(this + nGap, sizeof(KG3DFrustum) - nGap);
    //D3DXMatrixIdentity(&m_matVPInv);
}

KG3DFrustum::~KG3DFrustum()
{

}

HRESULT KG3DFrustum::CreateBy2Rays(const D3DXVECTOR3& vSrcA, const D3DXVECTOR3& vDirA,
					  const D3DXVECTOR3& vSrcC, const D3DXVECTOR3& vDirC,float fDistance)
{
	KG_PROCESS_ERROR(fDistance > 0);

	{
		CameraDataStruct CameraData;

		CameraData.m_fZFar = fDistance;

		CameraData.m_vDirection = (vDirA + vDirC) * 0.5f;
		D3DXVec3Normalize(&CameraData.m_vDirection,&CameraData.m_vDirection);

		//Near的计算有点麻烦，vDirA，vDirC，和vSrcA-vSrcC 三条线会形成三角形，用这个三角形可以计算出Near
		//vDirA和vDirC应该已经Normal化
		{
			FLOAT fAngleBetween2Dirs = acos(D3DXVec3Dot(&vDirA, &vDirC));
			FLOAT fLengthOfA2C = D3DXVec3Length(&(vSrcA - vSrcC));
			CameraData.m_fZNear = (fLengthOfA2C / 2) / tan(fAngleBetween2Dirs / 2);
			KG_PROCESS_ERROR(fLengthOfA2C > FLT_EPSILON && _T("两个点叠在一起的时候，计算的视锥巨大"));

			D3DXVECTOR3 vScreenCenter = (vSrcA + vSrcC) * 0.5f;

			CameraData.m_vOrigin =  vScreenCenter - CameraData.m_vDirection * CameraData.m_fZNear;
		}

		m_CameraData = CameraData;

		//计算View

		{
			D3DXVECTOR3 vAt = CameraData.m_vOrigin + CameraData.m_vDirection * CameraData.m_fZFar;
			D3DXMatrixLookAtLH(&m_matView, &CameraData.m_vOrigin, &vAt, &D3DXVECTOR3(0,1,0));
		}

		//计算Proj
		{

			D3DXVECTOR3 vRight = D3DXMatrixGetRightOfView(m_matView);

			FLOAT fNearRightLength = D3DXVec3Length(&ProjectVectorToDirection((vSrcA - vSrcC), vRight));

			D3DXVECTOR3 vUp = D3DXMatrixGetUpOfView(m_matView);

			FLOAT fNearUpLength = D3DXVec3Length(&ProjectVectorToDirection((vSrcA - vSrcC), vUp));

			D3DXMatrixPerspectiveLH(&m_matProj, fNearRightLength, fNearUpLength, CameraData.m_fZNear, CameraData.m_fZFar);
		}

		ComputeFrustum(m_matView, m_matProj);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

D3DXVECTOR3 GetFarPoint(const D3DXVECTOR3& vCameraPos,D3DXVECTOR3 vNearPos,float fZFar)
{
	D3DXVECTOR3 V = vNearPos - vCameraPos;
	D3DXVec3Normalize(&V,&V);
	return (vCameraPos + V * fZFar);
}

//HRESULT KG3DFrustum::CalcDirection()
//{
//    m_CameraData.m_vDirection = 
//        (m_vecViewFrustumPoint[FAR_LEFT_BOTTOM] + m_vecViewFrustumPoint[FAR_RIGHT_BOTTOM]   +
//         m_vecViewFrustumPoint[FAR_LEFT_TOP] + m_vecViewFrustumPoint[FAR_RIGHT_TOP] ) / 4   -   
//        (m_vecViewFrustumPoint[NEAR_LEFT_BOTTOM] + m_vecViewFrustumPoint[NEAR_RIGHT_BOTTOM] +
//         m_vecViewFrustumPoint[NEAR_LEFT_TOP] + m_vecViewFrustumPoint[NEAR_RIGHT_TOP] ) / 4;
//    D3DXVec3Normalize(&m_CameraData.m_vDirection, &m_CameraData.m_vDirection);
//    return S_OK;
//}

namespace {

    struct Rectf { float left; float top; float right; float bottom; };

    inline bool IntersectRect(Rectf& rectc, const Rectf &rect1, const Rectf &rect2)
    {
        rectc.left = max(rect1.left, rect2.left);
        rectc.top  = max(rect1.top, rect2.top);

        rectc.right = min(rect1.right, rect2.right);

        if (rectc.right <= rectc.left) {
            return false;
        }

        rectc.bottom = min(rect1.bottom, rect2.bottom);

        if (rectc.bottom <= rectc.top) {
            return false;
        }

        return true;
    }

    inline void ConverToUVCoord(Rectf& rect) 
    {
        rect.left   = 0.5f + rect.left   * 0.5f;
        rect.right  = 0.5f + rect.right  * 0.5f;
        rect.top    = 0.5f - rect.top    * 0.5f;
        rect.bottom = 0.5f - rect.bottom * 0.5f;
    }

    inline void ConverToPjCoord(Rectf& rect) 
    {
        rect.left   = rect.left   * 2.f - 1.f;
        rect.right  = rect.right  * 2.f - 1.f;
        rect.top    = 1.f - rect.top    * 2.f;
        rect.bottom = 1.f - rect.bottom * 2.f; 
    }

    inline void ConverRect(D3DXVECTOR3 vRect[])
    {
        float min_x = static_cast<float>(INT_MAX);
        float min_y = static_cast<float>(INT_MAX);
        float max_x = static_cast<float>(INT_MIN);
        float max_y = static_cast<float>(INT_MIN);
        float min_z = static_cast<float>(INT_MAX);

        for (int i = 0; i < 4; ++i)
        {
            if (vRect[i].x < min_x)
                min_x = vRect[i].x;

            if (vRect[i].y < min_y)
                min_y = vRect[i].y;

            if (vRect[i].x > max_x)
                max_x = vRect[i].x;

            if (vRect[i].y > max_y)
                max_y = vRect[i].y;

            if (vRect[i].z < min_z)
                min_z = vRect[i].z;
        }

        vRect[0] = D3DXVECTOR3(min_x, max_y, min_z);
        vRect[1] = D3DXVECTOR3(max_x, max_y, min_z);
        vRect[2] = D3DXVECTOR3(max_x, min_y, min_z);
        vRect[3] = D3DXVECTOR3(min_x, min_y, min_z);
    }
};


BOOL KG3DFrustum::PortalVisable(const D3DXVECTOR3 vp[], DWORD dwLookMode) const
{
    if (vp[0].x < -1.f && vp[1].x < -1.f && vp[2].x < -1.f && vp[3].x < -1.f)
        return FALSE;

    if (vp[0].x > 1.f && vp[1].x > 1.f && vp[2].x > 1.f && vp[3].x > 1.f)
        return FALSE;

    if (vp[0].y < -1.f && vp[1].y < -1.f && vp[2].y < -1.f && vp[3].y < -1.f)
        return FALSE;

    if (vp[0].y > 1.f && vp[1].y > 1.f && vp[2].y > 1.f && vp[3].y > 1.f)
        return FALSE;

    if (vp[0].z < 0.f && vp[1].z < 0.f && vp[2].z < 0.f && vp[3].z < 0.f)
        return FALSE;

    if (vp[0].z > 1.f && vp[1].z > 1.f && vp[2].z > 1.f && vp[3].z > 1.f)
        return FALSE;

    D3DXVECTOR2 e1 = D3DXVECTOR2(vp[1].x, vp[1].y) - D3DXVECTOR2(vp[0].x, vp[0].y);
    D3DXVECTOR2 e2 = D3DXVECTOR2(vp[2].x, vp[2].y) - D3DXVECTOR2(vp[1].x, vp[1].y);

    switch (dwLookMode)
    {
    case LOOK_OUT_TO_IN :
        {
            if (D3DXVec2CCW(&e1, &e2) > 0.f)
                return FALSE;
        }
        break;
    case LOOK_IN_TO_OUT :
        {
            if (D3DXVec2CCW(&e1, &e2) < 0.f)
                return FALSE;
        }
        break;
    default :
        return FALSE;
        break;
    }

    return TRUE;
}

BOOL KG3DFrustum::CullByPortal(const KG3DPvsPortal& Portal, DWORD dwLookMode)
{
    D3DXVECTOR3 vPortalPoint[5];

    D3DXVec3TransformCoordArray(vPortalPoint, sizeof(D3DXVECTOR3), Portal.m_vWorldVector, sizeof(D3DXVECTOR3), &m_matViewProj, 4);

    for (int i = 0; i < 4; ++i)
    {
        if (vPortalPoint[i].z > 1.f)
            vPortalPoint[i] *= -1.f;
    } 


#if 0
    D3DXVECTOR4 vPortalTS[4];
    
    for (int i = 0; i < 4; ++i)
        vPortalTS[i] = D3DXVECTOR4(Portal.m_vWorldVector[i].x, Portal.m_vWorldVector[i].y, Portal.m_vWorldVector[i].z, 1.f);
    
    D3DXVec4TransformArray(vPortalTS, sizeof(D3DXVECTOR4), vPortalTS, sizeof(D3DXVECTOR4), &m_matViewProj, 4);
#endif    
    

    if (!PortalVisable(vPortalPoint, dwLookMode))
        return FALSE;

    ConverRect(vPortalPoint);

    Rectf rPortal = {vPortalPoint[0].x, vPortalPoint[0].y, vPortalPoint[2].x, vPortalPoint[2].y};
    Rectf rFrustm = {0.f, 0.f, 1.f, 1.f};
    Rectf rInters;

    ConverToUVCoord(rPortal);

    if (!IntersectRect(rInters, rPortal, rFrustm))
        return FALSE;

    ConverToPjCoord(rInters);

    vPortalPoint[0].x = rInters.left;
    vPortalPoint[0].y = rInters.top;
    vPortalPoint[1].x = rInters.right;
    vPortalPoint[1].y = rInters.top;
    vPortalPoint[2].x = rInters.right;
    vPortalPoint[2].y = rInters.bottom;
    vPortalPoint[3].x = rInters.left;
    vPortalPoint[3].y = rInters.bottom;
    vPortalPoint[4].x = (rInters.left + rInters.right) * 0.5f;
    vPortalPoint[4].y = (rInters.top + rInters.bottom) * 0.5f;
    vPortalPoint[4].z = vPortalPoint[0].z;

	

	//<m_matVPInv Refactor>
    //D3DXVec3TransformCoordArray(vPortalPoint, sizeof(D3DXVECTOR3), vPortalPoint, sizeof(D3DXVECTOR3), &m_matVPInv, 5);

	{
		D3DXMATRIX matVPInv;
		D3DXMatrixInverse(&matVPInv, NULL, &m_matViewProj);
		D3DXVec3TransformCoordArray(vPortalPoint, sizeof(D3DXVECTOR3), vPortalPoint, sizeof(D3DXVECTOR3), &matVPInv, 5);
	}
    
	
	D3DXMatrixLookAtLH(&m_matView, &m_CameraData.m_vOrigin, &vPortalPoint[4], &D3DXVECTOR3(0.f, 1.f, 0.f));
    D3DXMatrixPerspectiveLH(
        &m_matProj,
        D3DXVec3Length(&(vPortalPoint[0] - vPortalPoint[1])),
        D3DXVec3Length(&(vPortalPoint[1] - vPortalPoint[2])), 
        D3DXVec3Length(&(m_CameraData.m_vOrigin - vPortalPoint[4])),
        m_CameraData.m_fZFar
        );
    m_matViewProj = m_matView * m_matProj;
    //D3DXMatrixInverse(&m_matVPInv, NULL,&m_matViewProj);	//<m_matVPInv Refactor>

    m_CameraData.m_vDirection =  D3DXVECTOR3(m_matView._13, m_matView._23, m_matView._33);

	//<m_matVPInv Refactor>
    /*D3DXVec3TransformCoordArray(m_vPoints, sizeof(D3DXVECTOR3),  m_stVerticesInPPSpace
		, sizeof(D3DXVECTOR3), &m_matVPInv, _countof(m_stVerticesInPPSpace));*/

	
	{
		D3DXMATRIX matVPInv;
		D3DXMatrixInverse(&matVPInv, NULL, &m_matViewProj);
		D3DXVec3TransformCoordArray(m_vPoints, sizeof(D3DXVECTOR3),  m_stVerticesInPPSpace
			, sizeof(D3DXVECTOR3), &matVPInv, _countof(m_stVerticesInPPSpace));
	}

	ComputePlanesFromPoints();

    return TRUE;
}

HRESULT KG3DFrustum::CreateByObjectPVS(KG3DCamera& Camera,D3DXVECTOR3 ProtalPos[4])
{
	return CreateByPointGate(Camera, ProtalPos[PORTAL_LEFT_B0TTOM], ProtalPos[PORTAL_RIGHT_B0TTOM]
					, ProtalPos[PORTAL_LEFT_TOP], ProtalPos[PORTAL_RIGHT_TOP]);
}
inline VOID GetNearFarPoint(const D3DXVECTOR3& Origin, const D3DXVECTOR3& GatePos
					   , FLOAT fZNear, FLOAT fZFar
					   , D3DXVECTOR3& RetNear, D3DXVECTOR3& RetFar)
{
	D3DXVECTOR3 vDir = GatePos - Origin;
	D3DXVec3Normalize(&vDir, &vDir);
	RetNear = Origin + vDir * fZNear;
	RetFar = Origin + vDir * fZFar;
}
HRESULT KG3DFrustum::CreateByPointGate( KG3DCamera& Camera, const D3DXVECTOR3& BottomLeft
									   , const D3DXVECTOR3& BottomRight
									   , const D3DXVECTOR3& TopLeft
									   , const D3DXVECTOR3& TopRight )
{
	CameraDataStruct CameraData;
	CameraData.m_vOrigin = Camera.GetPosition();
	m_matView = Camera.GetViewMatrix();
	m_matProj = Camera.GetProjectionMatrix();
	CameraData.m_fZFar = Camera.GetPerspectiveData().fzFar;
	CameraData.m_fZNear = Camera.GetPerspectiveData().fzNear;
	CameraData.m_vDirection = Camera.GetCameraFront();

	HRESULT hr = CreateByGatePrivate(CameraData, BottomLeft, BottomRight, TopLeft, TopRight);
	if (SUCCEEDED(hr) && (&CameraData != &m_CameraData))
	{
		m_CameraData = CameraData;
	}
	return hr;

}

HRESULT KG3DFrustum::CreateByCamera(KG3DCamera& Camera)
{
	m_CameraData.m_vOrigin = Camera.GetPosition();
	m_matView = Camera.GetViewMatrix();
	m_matProj = Camera.GetProjectionMatrix();
	m_CameraData.m_fZFar = Camera.GetPerspectiveData().fzFar;
	m_CameraData.m_fZNear = Camera.GetPerspectiveData().fzNear;
	m_CameraData.m_vDirection = Camera.GetCameraFront();

	ComputeFrustum(m_matView, m_matProj);
	return S_OK;
	
	
}

HRESULT KG3DFrustum::CreateByDevice(LPDIRECT3DDEVICE9 pDevice)
{
	_ASSERTE(NULL != pDevice);
	HRESULT hr = pDevice->GetTransform(D3DTS_PROJECTION,&m_matProj);
	KG_COM_PROCESS_ERROR(hr);
	hr = pDevice->GetTransform(D3DTS_VIEW,&m_matView);
	KG_COM_PROCESS_ERROR(hr);
	
	ComputeFrustum(m_matView, m_matProj);	

	return S_OK;
Exit0:
	return E_FAIL;
}
VOID KG3DFrustum::ComputeFrustum( const D3DXMATRIX& matView, const D3DXMATRIX& matProj )
{
	if(&m_matView != &matView)
		m_matView = matView;
	if(&m_matProj != &matProj)
		m_matProj = matProj;
	m_matViewProj = matView * matProj;

	D3DXMATRIX matVPInv;

	D3DXMATRIX* p = D3DXMatrixInverse(&/*m_matVPInv*/matVPInv, NULL, &m_matViewProj);////<m_matVPInv Refactor>
	if(NULL == p)
		return;

	D3DXVec3TransformCoordArray(m_vPoints, sizeof(D3DXVECTOR3)
		, m_stVerticesInPPSpace
		, sizeof(D3DXVECTOR3)
		, &/*m_matVPInv*/matVPInv	//<m_matVPInv Refactor>
		, _countof(m_stVerticesInPPSpace));

	
	ComputePlanesFromPoints();
}

VOID KG3DFrustum::ComputePlanesFromPoints()
{
	D3DXPlaneFromPoints(&m_Planes[NEARPLANE],&m_vPoints[NEAR_RIGHT_BOTTOM],
		&m_vPoints[NEAR_RIGHT_TOP],&m_vPoints[NEAR_LEFT_TOP]);
	D3DXPlaneFromPoints(&m_Planes[FARPLANE],&m_vPoints[FAR_LEFT_BOTTOM],
		&m_vPoints[FAR_LEFT_TOP],&m_vPoints[FAR_RIGHT_TOP]);
	D3DXPlaneFromPoints(&m_Planes[LEFTPLANE],&m_vPoints[NEAR_LEFT_BOTTOM],
		&m_vPoints[NEAR_LEFT_TOP],&m_vPoints[FAR_LEFT_TOP]);
	D3DXPlaneFromPoints(&m_Planes[RIGHTPLANE],&m_vPoints[FAR_RIGHT_BOTTOM],
		&m_vPoints[FAR_RIGHT_TOP],&m_vPoints[NEAR_RIGHT_TOP]);
	D3DXPlaneFromPoints(&m_Planes[TOPPLANE],&m_vPoints[FAR_LEFT_TOP],
		&m_vPoints[NEAR_LEFT_TOP],&m_vPoints[NEAR_RIGHT_TOP]);
	D3DXPlaneFromPoints(&m_Planes[BOTTOMPLANE],&m_vPoints[NEAR_LEFT_BOTTOM],
		&m_vPoints[FAR_LEFT_BOTTOM],&m_vPoints[FAR_RIGHT_BOTTOM]);
}

CULLSTATE KG3DFrustum::CullObjectInside(const D3DXVECTOR3 *ViewFrustumPoint,const D3DXPLANE *ViewFrustumPlane,
										const D3DXVECTOR3 *ObjectBoundPoint,const D3DXPLANE *ObjectBoundPlane)
{
	for (int i=0;i<6;i++)
	{
		const D3DXPLANE& K = ViewFrustumPlane[i];

		for (int s=0;s<8;s++)
		{
			const D3DXVECTOR3& p = ObjectBoundPoint[s];
			if(K.a * p.x + K.b * p.y + K.c * p.z + K.d<0)
			{
				return CS_OUTSIDE;
			}
		}
	}

	return CS_INSIDE;
}

BOOL KG3DFrustum::IsOBBoxVisibleOptimize(D3DXVECTOR3 BoxPos[])
{
	static D3DXVECTOR3 vPos[8];
	D3DXVec3TransformCoordArray(vPos,sizeof(D3DXVECTOR3),BoxPos,sizeof(D3DXVECTOR3),&m_matViewProj,8);

	BOOL bInside = FALSE;
	for (int i=0;i<8;i++)
	{
		if((vPos[i].z >= 0)&&(vPos[i].z<=1.0F)&&
		   (vPos[i].x >= -1.0F)&&(vPos[i].x<=1.0F))
			bInside = TRUE;
	}
	return bInside;
}

BOOL KG3DFrustum::IsNegativeSide(const D3DXPLANE& plane, const D3DXVECTOR3& vCenter, const D3DXVECTOR3& vHalf)
{
    FLOAT fDist = D3DXPlaneDotCoord(&plane, &vCenter);    
    FLOAT fMaxAbsDist = fabs(plane.a * vHalf.x) + fabs(plane.b * vHalf.y) + fabs(plane.c * vHalf.z);

    if (fDist < -fMaxAbsDist)
        return TRUE;

    return FALSE;
}

BOOL KG3DFrustum::IsOBBoxVisible(const D3DXVECTOR3 BoxPos[]) const
{
 
    D3DXVECTOR3 vCenter = (BoxPos[2] + BoxPos[4]) * 0.5f;
    D3DXVECTOR3 vHalf   = vCenter - BoxPos[4];

    for (int i = 0; i < 6; ++i)
    {
        FLOAT fDist = 
            m_Planes[i].a * vCenter.x +
            m_Planes[i].b * vCenter.y +
            m_Planes[i].c * vCenter.z +
            m_Planes[i].d * 1.f;

        FLOAT fMaxAbsDist =
            fabs(m_Planes[i].a * vHalf.x) + 
            fabs(m_Planes[i].b * vHalf.y) +
            fabs(m_Planes[i].c * vHalf.z);

        if (fDist < -fMaxAbsDist)
            return FALSE;
    }

    return TRUE;    
}

BOOL KG3DFrustum::IsAABBoxVisible(const AABBOX& BBox) const
{
	return Intersect(BBox);
}
INT KG3DFrustum::Intersect( const AABBOX& BBox ) const
{
	D3DXVECTOR3 vCenter = BBox.GetCenter();
	D3DXVECTOR3 vHalf   = vCenter - BBox.A;

	//稍微改一下，改为探测Intersect的情况
	INT Result = INSIDE;
	for (INT i = 0; i < 6; i++) 
	{ 
		const D3DXPLANE& P = m_Planes[i];

		FLOAT fDist = (vCenter.x * P.a) + (vCenter.y * P.b) + (vCenter.z * P.c) + P.d;
		FLOAT fMaxAbsDist = abs(vHalf.x * P.a) + abs(vHalf.y * P.b) + abs(vHalf.z * P.c);

		if (fDist < -fMaxAbsDist) 
			return OUTSIDE;
		if (fDist < fMaxAbsDist)	//这里包含了上面那个判断 
			Result = INTERSECT;
		//不需要else.因为else就是INSIDE
	} 
	return Result;

	//Efficient View Frustum Culling 中有对这个算法的详细解释
}

BOOL KG3DFrustum::IsAABBoxInside(AABBOX& BBox)
{
	static D3DXVECTOR3 BoxPos[8];
	BBox.Extract(BoxPos);

	CULLSTATE state;
	D3DXPLANE planeBoundsWorld[6];
	// Determine planes of the bounding box
	D3DXPlaneFromPoints( &planeBoundsWorld[0], &BoxPos[0], 
		&BoxPos[1], &BoxPos[2] ); // Near
	D3DXPlaneFromPoints( &planeBoundsWorld[1], &BoxPos[6], 
		&BoxPos[7], &BoxPos[5] ); // Far
	D3DXPlaneFromPoints( &planeBoundsWorld[2], &BoxPos[2], 
		&BoxPos[6], &BoxPos[4] ); // Left
	D3DXPlaneFromPoints( &planeBoundsWorld[3], &BoxPos[7], 
		&BoxPos[3], &BoxPos[5] ); // Right
	D3DXPlaneFromPoints( &planeBoundsWorld[4], &BoxPos[2], 
		&BoxPos[3], &BoxPos[6] ); // Top
	D3DXPlaneFromPoints( &planeBoundsWorld[5], &BoxPos[1], 
		&BoxPos[0], &BoxPos[4] ); // Bottom

	state = CullObjectInside(m_vPoints,m_Planes,BoxPos,planeBoundsWorld);

	if (state==GraphicsStruct::CS_INSIDE)
		return TRUE;
	else
		return FALSE;
}
HRESULT KG3DFrustum::ConstructEdges(const D3DXVECTOR3 (&vPoints)[em_point_count], KG3DLine (&edges)[em_edge_count])	//传进来的是个指向数组的引用
{
	edges[0][0] = vPoints[NEAR_LEFT_BOTTOM]; 
	edges[0][1] = vPoints[NEAR_LEFT_TOP]; 

	edges[1][0] = vPoints[NEAR_LEFT_TOP]; 
	edges[1][1] = vPoints[NEAR_RIGHT_TOP]; 

	edges[2][0] = vPoints[NEAR_RIGHT_TOP]; 
	edges[2][1] = vPoints[NEAR_RIGHT_BOTTOM]; 

	edges[3][0] = vPoints[NEAR_RIGHT_BOTTOM]; 
	edges[3][1] = vPoints[NEAR_LEFT_BOTTOM]; 



	edges[4][0] = vPoints[FAR_LEFT_BOTTOM]; 
	edges[4][1] = vPoints[FAR_LEFT_TOP]; 

	edges[5][0] = vPoints[FAR_LEFT_TOP]; 
	edges[5][1] = vPoints[FAR_RIGHT_TOP]; 

	edges[6][0] = vPoints[FAR_RIGHT_TOP]; 
	edges[6][1] = vPoints[FAR_RIGHT_BOTTOM]; 

	edges[7][0] = vPoints[FAR_RIGHT_BOTTOM]; 
	edges[7][1] = vPoints[FAR_LEFT_BOTTOM]; 


	edges[8][0] = vPoints[NEAR_LEFT_BOTTOM]; 
	edges[8][1] = vPoints[FAR_LEFT_BOTTOM]; 

	edges[9][0] = vPoints[NEAR_LEFT_TOP]; 
	edges[9][1] = vPoints[FAR_LEFT_TOP]; 

	edges[10][0] = vPoints[NEAR_RIGHT_BOTTOM]; 
	edges[10][1] = vPoints[FAR_RIGHT_BOTTOM]; 

	edges[11][0] = vPoints[NEAR_RIGHT_TOP]; 
	edges[11][1] = vPoints[FAR_RIGHT_TOP];


	return S_OK;
}

HRESULT KG3DFrustum::DrawFrustumByLine()
{
	int i;

	KG3DLine edges[em_edge_count];
	ConstructEdges(m_vPoints, edges);

	for(i = 0;i < 12;++i)
	{
		g_cGraphicsTool.DrawLine(&edges[i][0],&edges[i][1],
			0xff0000ff,0xff0000ff);
	}

	return S_OK;
}

HRESULT KG3DFrustum::GetFrustumXZRect(D3DXVECTOR2& A,D3DXVECTOR2& C)
{
	return S_OK;
}

const D3DXVECTOR3& KG3DFrustum::GetPoint( INT nIndex )const
{
	_ASSERTE(nIndex >= 0 && nIndex < _countof(m_vPoints));
	return m_vPoints[nIndex];
}

INT KG3DFrustum::GetPointCount() const
{
	return _countof(m_vPoints);
}

const D3DXPLANE& KG3DFrustum::GetPlane( INT nIndex ) const
{
	_ASSERTE(nIndex >= 0 && nIndex < _countof(m_Planes));
	return m_Planes[nIndex];
}

INT KG3DFrustum::GetPlaneCount() const
{
	return _countof(m_Planes);
}

HRESULT KG3DFrustum::CreateByGatePrivate(CameraDataStruct& CameraData
										 , const D3DXVECTOR3& BottomLeft
										 , const D3DXVECTOR3& BottomRight 
										 , const D3DXVECTOR3& TopLeft
										 , const D3DXVECTOR3& TopRight )
{
#if defined(DEBUG) | defined(_DEBUG)
	{
		//保证他们是一个门的形状
		D3DXVECTOR3 vRight1 = BottomRight - BottomLeft;
		D3DXVECTOR3 vRight2 = TopRight - TopLeft;
		D3DXVECTOR3 vUp1 = TopLeft - BottomLeft;
		D3DXVECTOR3 vUp2 = TopRight - BottomRight;
		_ASSERTE(MT_VECTOR_EQUAL(vRight1, vRight2, 1.0f));
		_ASSERTE(MT_VECTOR_EQUAL(vUp1, vUp2, 1.0f));
	}
#endif
	_ASSERTE(CameraData.m_fZFar > CameraData.m_fZNear);
	BOOL bIsPositive = TRUE;

	//检查Camera方向和Portal的位置是否有可能形成新的Frustrum
	FLOAT fNearLineLength = CameraData.m_fZNear;
	FLOAT fFarLineLength = CameraData.m_fZFar;
	{
		D3DXVECTOR3 vCenter = BottomLeft + BottomRight + TopLeft + TopRight;
		vCenter /= 4;
		D3DXVECTOR3 vDirGate = vCenter - CameraData.m_vOrigin;
		FLOAT fDot = D3DXVec3Dot(&vDirGate, &CameraData.m_vDirection);
		KG_PROCESS_ERROR(fDot > 0);

		//计算出近端的线的长度
		D3DXVECTOR3 vDirTopLeft = TopLeft - CameraData.m_vOrigin;
		FLOAT fCosine = D3DXVec3Dot(&vDirGate, &vDirTopLeft) / (D3DXVec3Length(&vDirTopLeft) * D3DXVec3Length(&vDirGate));
		fNearLineLength = D3DXVec3Length(&vDirGate) / fCosine;
		fFarLineLength = CameraData.m_fZFar / fCosine;
	}

	//看看摄像机的朝向在Gate的正向还是负向
	{
		D3DXPLANE Plane;
		D3DXPlaneFromPoints(&Plane, &BottomLeft, &BottomRight, &TopLeft);
		bIsPositive = (Plane.a * CameraData.m_vDirection.x + Plane.b * CameraData.m_vDirection.y + Plane.c * CameraData.m_vDirection.z) > 0;
	}


	if(bIsPositive)
	{
		GetNearFarPoint(CameraData.m_vOrigin, BottomLeft, fNearLineLength, fFarLineLength
			, m_vPoints[NEAR_LEFT_BOTTOM], m_vPoints[FAR_LEFT_BOTTOM]);
		GetNearFarPoint(CameraData.m_vOrigin, BottomRight, fNearLineLength, fFarLineLength
			, m_vPoints[NEAR_RIGHT_BOTTOM], m_vPoints[FAR_RIGHT_BOTTOM]);
		GetNearFarPoint(CameraData.m_vOrigin, TopLeft, fNearLineLength, fFarLineLength
			, m_vPoints[NEAR_LEFT_TOP], m_vPoints[FAR_LEFT_TOP]);
		GetNearFarPoint(CameraData.m_vOrigin, TopRight, fNearLineLength, fFarLineLength
			, m_vPoints[NEAR_RIGHT_TOP], m_vPoints[FAR_RIGHT_TOP]);
	}
	else	//如果在反面应该左右颠倒
	{
		GetNearFarPoint(CameraData.m_vOrigin, BottomRight, fNearLineLength, fFarLineLength
			, m_vPoints[NEAR_LEFT_BOTTOM], m_vPoints[FAR_LEFT_BOTTOM]);
		GetNearFarPoint(CameraData.m_vOrigin, BottomLeft, fNearLineLength, fFarLineLength
			, m_vPoints[NEAR_RIGHT_BOTTOM], m_vPoints[FAR_RIGHT_BOTTOM]);
		GetNearFarPoint(CameraData.m_vOrigin, TopRight, fNearLineLength, fFarLineLength
			, m_vPoints[NEAR_LEFT_TOP], m_vPoints[FAR_LEFT_TOP]);
		GetNearFarPoint(CameraData.m_vOrigin, TopLeft, fNearLineLength, fFarLineLength
			, m_vPoints[NEAR_RIGHT_TOP], m_vPoints[FAR_RIGHT_TOP]);
	}
	ComputePlanesFromPoints();
	return S_OK;
Exit0:
	return E_FAIL;
}

#if defined(KG_ENABLE_TDD)
KG_TEST_CLASS_TEST_BEGIN(KG3DFrustum)
{
	{
		class KG3DFrustumTester: public KG3DFrustum
		{
		public:
			using KG3DFrustum::ComputeFrustum;
			using KG3DFrustum::CreateByGatePrivate;
			using KG3DFrustum::m_matView;
			using KG3DFrustum::m_matProj;
			using KG3DFrustum::m_matViewProj;
			using KG3DFrustum::m_CameraData;
		};
		//验证最基础的ComputeFromFrustum
		{
			KG3DFrustumTester frustum;

			FLOAT Zn = 10.0f, ZFar = 400.0f;
			FLOAT Fovy = D3DX_PI / 4;
			FLOAT fAspect = 800.0f/600.0f;
			D3DXMATRIX matView, matProj;
			D3DXMatrixLookAtLH(&matView, &D3DXVECTOR3(0,0,0), &D3DXVECTOR3(0,0,1), &D3DXVECTOR3(0,1,0));
			D3DXMatrixPerspectiveFovLH(&matProj, Fovy, fAspect, Zn, ZFar);

			frustum.ComputeFrustum(matView, matProj);

			FLOAT fHeightNearHalf = 10 * tan(Fovy / 2);
			FLOAT fWidthNearHalf = fHeightNearHalf * fAspect;
			FLOAT fHeightFarHalf = 400 * tan(Fovy / 2);
			FLOAT fWidthFarHalf = fHeightFarHalf * fAspect;

			D3DXVECTOR3 PointResult[] = 
			{
				D3DXVECTOR3(- fWidthNearHalf, - fHeightNearHalf, Zn)
				,D3DXVECTOR3(fWidthNearHalf, - fHeightNearHalf, Zn)
				,D3DXVECTOR3(- fWidthNearHalf, fHeightNearHalf, Zn)
				,D3DXVECTOR3(fWidthNearHalf, fHeightNearHalf, Zn)

				,D3DXVECTOR3(-fWidthFarHalf, - fHeightFarHalf, ZFar)
				,D3DXVECTOR3(fWidthFarHalf, - fHeightFarHalf, ZFar)
				,D3DXVECTOR3(-fWidthFarHalf, fHeightFarHalf, ZFar)
				,D3DXVECTOR3(fWidthFarHalf, fHeightFarHalf, ZFar)

			};
			for (INT i = 0; i < frustum.GetPointCount(); ++i)
			{
				const D3DXVECTOR3& PT = frustum.GetPoint(i);
				_ASSERTE(MT_VECTOR_EQUAL(PT, PointResult[i], 0.01));
			}

			enum FrustumPlane {NEARPLANE = 0,FARPLANE,LEFTPLANE,RIGHTPLANE,TOPPLANE,BOTTOMPLANE};
			D3DXPLANE PlaneResult[6];
			D3DXPlaneFromPoints(PlaneResult+0, PointResult+0, PointResult+1, PointResult+2);
			D3DXPlaneFromPoints(PlaneResult+1, PointResult+4, PointResult+7, PointResult+5);
			D3DXPlaneFromPoints(PlaneResult+2, PointResult+0, PointResult+2, PointResult+4);
			D3DXPlaneFromPoints(PlaneResult+3, PointResult+1, PointResult+5, PointResult+3);
			D3DXPlaneFromPoints(PlaneResult+4, PointResult+2, PointResult+3, PointResult+6);
			D3DXPlaneFromPoints(PlaneResult+5, PointResult+0, PointResult+4, PointResult+5);


			for (INT i = 0; i < _countof(PlaneResult); ++i)
			{
				const D3DXPLANE& PL = frustum.GetPlane(i);
				_ASSERTE(MT_FLOAT_EQUAL_BIAS(PL.a, PlaneResult[i].a, 0.01f));
				_ASSERTE(MT_FLOAT_EQUAL_BIAS(PL.b, PlaneResult[i].b, 0.01f));
				_ASSERTE(MT_FLOAT_EQUAL_BIAS(PL.c, PlaneResult[i].c, 0.01f));
				_ASSERTE(MT_FLOAT_EQUAL_BIAS(PL.d, PlaneResult[i].d, 0.01f));
			}
		}

		//检查CreateByPointGate
		{
			D3DXVECTOR3 GatePoints[] = 
			{
				D3DXVECTOR3(-1,-1,10),
				D3DXVECTOR3(1,-1,10),
				D3DXVECTOR3(-1,1,10),
				D3DXVECTOR3(1,1,10),
			};
			KG3DFrustumTester frustum;
			FLOAT Zn = 10.0f, ZFar = 400.0f;
			FLOAT Fovy = D3DX_PI / 4;
			FLOAT fAspect = 800.0f/600.0f;

			FLOAT fWidthNearHalf = 1;
			FLOAT fHeightNearHalf = 1;
			FLOAT fWidthFarHalf = ZFar/Zn*1;
			FLOAT fHeightFarHalf = fWidthFarHalf;

			frustum.m_CameraData.m_fZNear = Zn;
			frustum.m_CameraData.m_fZFar = ZFar;
			frustum.m_CameraData.m_vOrigin = D3DXVECTOR3(0,0,0);
			frustum.m_CameraData.m_vDirection = D3DXVECTOR3(0,0,1);
			D3DXMatrixLookAtLH(&frustum.m_matView, &frustum.m_CameraData.m_vOrigin
				, &(frustum.m_CameraData.m_vOrigin + frustum.m_CameraData.m_vDirection), &D3DXVECTOR3(0,1,0));
			D3DXMatrixPerspectiveFovLH(&frustum.m_matProj, Fovy
				, fAspect, frustum.m_CameraData.m_fZNear, frustum.m_CameraData.m_fZFar);
			HRESULT hr = frustum.CreateByGatePrivate(frustum.m_CameraData, GatePoints[0], GatePoints[1], GatePoints[2], GatePoints[3]);
			_ASSERTE(SUCCEEDED(hr));

			D3DXVECTOR3 PointResult[] = 
			{
				D3DXVECTOR3(- fWidthNearHalf, - fHeightNearHalf, Zn)
				,D3DXVECTOR3(fWidthNearHalf, - fHeightNearHalf, Zn)
				,D3DXVECTOR3(- fWidthNearHalf, fHeightNearHalf, Zn)
				,D3DXVECTOR3(fWidthNearHalf, fHeightNearHalf, Zn)

				,D3DXVECTOR3(-fWidthFarHalf, - fHeightFarHalf, ZFar)
				,D3DXVECTOR3(fWidthFarHalf, - fHeightFarHalf, ZFar)
				,D3DXVECTOR3(-fWidthFarHalf, fHeightFarHalf, ZFar)
				,D3DXVECTOR3(fWidthFarHalf, fHeightFarHalf, ZFar)

			};

			for (INT i = 0; i < frustum.GetPointCount(); ++i)
			{
				const D3DXVECTOR3& PT = frustum.GetPoint(i);
				_ASSERTE(MT_VECTOR_EQUAL(PT, PointResult[i], 0.01));
			}
			//把Frustum搞到背面再试一次

			frustum.m_CameraData.m_vDirection = D3DXVECTOR3(0,0,-1);
			hr = frustum.CreateByGatePrivate(frustum.m_CameraData, GatePoints[0], GatePoints[1]
			, GatePoints[2], GatePoints[3]);
			_ASSERTE(FAILED(hr));
		}
	}
	{
		//创建一个面向X正向的椎体，检查创建
		KG3DFrustum frustum;

		FLOAT fNear = 2;
		FLOAT fHalfWidth = 2;
		FLOAT fHalfHeight = 1;

		D3DXVECTOR3 vSrcA(fNear,fHalfHeight,fHalfWidth);
		D3DXVECTOR3 vSrcC(fNear,-fHalfHeight,-fHalfWidth);

		D3DXVECTOR3 vDirA = vSrcA - D3DXVECTOR3(0,0,0);
		D3DXVECTOR3 vDirC = vSrcC - D3DXVECTOR3(0,0,0);

		D3DXVec3Normalize(&vDirA, &vDirA);
		D3DXVec3Normalize(&vDirC, &vDirC);

		FLOAT fTimes = 100;
		FLOAT fFar = fNear * fTimes;
		FLOAT fBias = 0.1f;

		frustum.CreateBy2Rays(vSrcA, vDirA, vSrcC, vDirC, fFar);

		_ASSERTE(MT_FLOAT_EQUAL(frustum.m_CameraData.m_fZFar, fFar));
		_ASSERTE(MT_FLOAT_EQUAL(frustum.m_CameraData.m_fZNear, fNear));
		_ASSERTE(MT_VECTOR_EQUAL(frustum.m_CameraData.m_vDirection, D3DXVECTOR3(1,0,0), fBias));
		_ASSERTE(MT_VECTOR_EQUAL(frustum.m_CameraData.m_vOrigin, D3DXVECTOR3(0,0,0), fBias));

		_ASSERTE(MT_VECTOR_EQUAL(frustum.m_vPoints[NEAR_LEFT_TOP], D3DXVECTOR3(fNear, fHalfHeight, fHalfWidth), fBias));
		_ASSERTE(MT_VECTOR_EQUAL(frustum.m_vPoints[NEAR_LEFT_BOTTOM], D3DXVECTOR3(fNear, -fHalfHeight, fHalfWidth), fBias));
		_ASSERTE(MT_VECTOR_EQUAL(frustum.m_vPoints[NEAR_RIGHT_TOP], D3DXVECTOR3(fNear, fHalfHeight, -fHalfWidth), fBias));
		_ASSERTE(MT_VECTOR_EQUAL(frustum.m_vPoints[NEAR_RIGHT_BOTTOM], D3DXVECTOR3(fNear, -fHalfHeight, -fHalfWidth), fBias));
		_ASSERTE(MT_VECTOR_EQUAL(frustum.m_vPoints[FAR_LEFT_TOP], D3DXVECTOR3(fNear * fTimes, fHalfHeight * fTimes, fHalfWidth * fTimes), fBias));
		_ASSERTE(MT_VECTOR_EQUAL(frustum.m_vPoints[FAR_LEFT_BOTTOM], D3DXVECTOR3(fNear * fTimes, -fHalfHeight * fTimes, fHalfWidth * fTimes), fBias));
		_ASSERTE(MT_VECTOR_EQUAL(frustum.m_vPoints[FAR_RIGHT_TOP], D3DXVECTOR3(fNear * fTimes, fHalfHeight * fTimes, -fHalfWidth * fTimes), fBias));
		_ASSERTE(MT_VECTOR_EQUAL(frustum.m_vPoints[FAR_RIGHT_BOTTOM], D3DXVECTOR3(fNear * fTimes, -fHalfHeight * fTimes, -fHalfWidth * fTimes), fBias));
	}
}
KG_TEST_CLASS_TEST_END(KG3DFrustum)
#endif
