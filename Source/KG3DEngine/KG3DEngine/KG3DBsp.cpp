#include "StdAfx.h"
#include "KG3DBsp.h"
#include "KG3DGraphicsTool.h"
#include "KG3DMesh.h"
#include "KG3DModel.h"
#include "MathTool.h"
#include "KG3DFileLoader.h"

const DWORD FILE_MASK = 'BSPF';
const DWORD FILE_VERSION = 0;

KG3DBsp::KG3DBsp(void)
{
    m_pHead = NULL;

    m_lpVertexs = NULL;
	m_lpTriangles = NULL;
	m_lpNodes = NULL;

    m_lpRootNode = NULL;
	m_lpRootNodeEx = NULL;
    m_piFileData = NULL;
}

KG3DBsp::~KG3DBsp(void)
{
	UnInit();
}

HRESULT KG3DBsp::UnInit()
{
    m_lpRootNodeEx = NULL;
	m_lpRootNode = NULL;

    if (m_piFileData)
    {
        // if load from memory, the following just point to buffer of m_piFileData
        m_lpNodes     = NULL;
        m_lpTriangles = NULL;
        m_lpVertexs   = NULL;
        m_pHead       = NULL;
        KG_COM_RELEASE(m_piFileData);
    }
    else
    {
        // if not load from memory, the following will be new, so need delete
        SAFE_DELETE_ARRAY(m_lpNodes);
	    SAFE_DELETE_ARRAY(m_lpTriangles);
        SAFE_DELETE_ARRAY(m_lpVertexs);
        SAFE_DELETE(m_pHead);
    }

	for (size_t i=0;i<m_vecRefBspNode.size();i++)
	{
		SAFE_DELETE(m_vecRefBspNode[i]);
	}
	m_vecRefBspNode.clear();

	return S_OK;
}

HRESULT KG3DBsp::CreateFromMesh(DWORD dwNumVertex,DWORD dwNumFace,D3DXVECTOR3* pVertex,DWORD* pFaceIndex)
{
	HRESULT hr = E_FAIL;
	vector<DWORD>vecTriangles;
	AABBOX bbox;

	ASSERT(!m_pHead);

    m_pHead = new _FILE_HEAD;
    KGLOG_PROCESS_ERROR(m_pHead);

	m_pHead->dwNumVertex = dwNumVertex;
	m_pHead->dwTriangles = dwNumFace;
    m_pHead->dwFileMask  = FILE_MASK;
    m_pHead->dwVersion   = FILE_VERSION;

	m_lpVertexs = new D3DXVECTOR3[m_pHead->dwNumVertex];
	KGLOG_PROCESS_ERROR(m_lpVertexs);

	memcpy(m_lpVertexs,pVertex,sizeof(D3DXVECTOR3)*m_pHead->dwNumVertex);

	bbox.AddPosition(m_lpVertexs,m_pHead->dwNumVertex);
	m_pHead->fRadius = bbox.GetDiameter();

	m_lpTriangles = new Triangle[m_pHead->dwTriangles];
	KGLOG_PROCESS_ERROR(m_lpTriangles);

	for (DWORD i=0;i<m_pHead->dwTriangles;i++)
	{
		Triangle* pTrian = &m_lpTriangles[i];
		pTrian->A = pFaceIndex[i*3  ];
		pTrian->B = pFaceIndex[i*3+1];
		pTrian->C = pFaceIndex[i*3+2];

		D3DXPLANE plane;
		D3DXPlaneFromPoints(&plane,&m_lpVertexs[pTrian->A],&m_lpVertexs[pTrian->B],&m_lpVertexs[pTrian->C]);

		pTrian->dwUserData = 0;
		pTrian->Normal = D3DXVECTOR3(plane.a,plane.b,plane.c);
	}

	for (DWORD i=0;i<m_pHead->dwTriangles;i++)
	{
		vecTriangles.push_back(i);
	}

	hr = CreateBspTree(vecTriangles,&m_lpRootNodeEx);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = ConvertToSimpleFormat();
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
Exit0:
	if(FAILED(hr))
	{
		UnInit();
	}

	
	return hr;
}

BOOL KG3DBsp::PlaneRelateVertex(D3DXPLANE* pPlane,D3DXVECTOR3* pVertex)
{
	return pPlane->a * pVertex->x + pPlane->b * pVertex->y + pPlane->c * pVertex->z + pPlane->d >= 0;
}

int KG3DBsp::PlaneRelateTriangle(D3DXPLANE* pPlane,Triangle* pTriangle)
{
	BOOL bA = PlaneRelateVertex(pPlane,&m_lpVertexs[pTriangle->A]);
	BOOL bB = PlaneRelateVertex(pPlane,&m_lpVertexs[pTriangle->B]);
	if(bA == bB)
	{
		BOOL bC = PlaneRelateVertex(pPlane,&m_lpVertexs[pTriangle->C]);
		if(bA == bC)
		{
			if(bA)
				return 1;//全在上面
			else
				return 0;//全在下面
		}
		else
		{
			return -1;
		}
	}
	else
	{
		return -1;//相交
	}
}

int KG3DBsp::IsPlaneFit(D3DXPLANE* pPlane,vector<DWORD>*pvecTriangles,PlaneInfo* pInfo)
{
	int nRef = -1;
	vector<DWORD>::iterator i = pvecTriangles->begin();
	vector<DWORD>::iterator iend = pvecTriangles->end();

	while (i!=iend)
	{
		DWORD dwTriangleIndex = *i;
		Triangle* pTrian = &m_lpTriangles[dwTriangleIndex];

		int K = PlaneRelateTriangle(pPlane,pTrian);

		if(K == -1)
		{
			pInfo->nRef++;
			nRef++;
		}
		else if(K==1)
		{
			pInfo->nUp++;
		}
		else
		{
			pInfo->nDown++;
		}
		i++;
	}

	return nRef;
}

HRESULT KG3DBsp::InputTranglesToNode(BspNodeEx* pNode,vector<DWORD>vecTriangles)
{
	vector<DWORD>vecUp;
	vector<DWORD>vecDown;

	if(vecTriangles.size()<10)
	{
		pNode->vecTriangles = vecTriangles;
		return S_OK;
	}

	for (size_t i=0;i<vecTriangles.size();i++)
	{
		DWORD dwTriangleIndex = vecTriangles[i];
		Triangle* pTrian = &m_lpTriangles[dwTriangleIndex];

		int K = PlaneRelateTriangle(&pNode->plane,pTrian);

		if(K == -1)
		{
			pNode->vecTriangles.push_back(dwTriangleIndex);
		}
		else if(K==1)
		{
			vecUp.push_back(dwTriangleIndex);
		}
		else
		{
			vecDown.push_back(dwTriangleIndex);
		}
	}

	if(vecUp.size())
	{
		if(FAILED(CreateBspTree(vecUp,&pNode->pSubNode[0])))
		{
			for (size_t s=0;s<vecUp.size();s++)
			{
				pNode->vecTriangles.push_back( vecUp[s] );
			}
		}
	}
	if(vecDown.size())
	{
		if(FAILED(CreateBspTree(vecDown,&pNode->pSubNode[1])))
		{
			for (size_t s=0;s<vecDown.size();s++)
			{
				pNode->vecTriangles.push_back( vecDown[s] );
			}
		}
	}
	return S_OK;
}

HRESULT KG3DBsp::CreateBspTree(vector<DWORD>vecTriangles,BspNodeEx** ppNode)
{
	vector<PlaneInfo>vecplanes;
	PlaneInfo vMinPlane;
	int nMinKey = 10000000;

	for (DWORD i=0;i<vecTriangles.size();i++)
	{
		DWORD dwTrangleIndex = vecTriangles[i];
		Triangle* pTrian = &m_lpTriangles[dwTrangleIndex];
		
		D3DXPLANE plane;

		PlaneInfo vPlaneInfo;
		ZeroMemory(&vPlaneInfo,sizeof(PlaneInfo));
		
		D3DXPlaneFromPoints(&plane,&m_lpVertexs[pTrian->A],&m_lpVertexs[pTrian->B],&m_lpVertexs[pTrian->C]);

		vPlaneInfo.MinPlane = plane;

		int nRef = IsPlaneFit(&plane,&vecTriangles,&vPlaneInfo);
		if(nRef >=0)
		{
			vPlaneInfo.Key = nRef + abs(vPlaneInfo.nUp - vPlaneInfo.nDown);
			vecplanes.push_back(vPlaneInfo);
		}
	}

	//////////////////////////////////////////////////////////////////////////
	for (size_t s=0;s<vecplanes.size();s++)
	{
		PlaneInfo& info = vecplanes[s];
		if(info.Key <= nMinKey)
		{
			vMinPlane = info;
			nMinKey = info.Key;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	KG_PROCESS_ERROR(nMinKey < 10000000);
	{
		BspNodeEx* pNewNode = RequesetNodeEx();
		KGLOG_PROCESS_ERROR(pNewNode);

		(*ppNode) = pNewNode;
		pNewNode->plane = vMinPlane.MinPlane;
		InputTranglesToNode(pNewNode,vecTriangles);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

KG3DBsp::BspNodeEx* KG3DBsp::RequesetNodeEx()
{
	BspNodeEx* pNewNode =  new BspNodeEx;
	KGLOG_PROCESS_ERROR(pNewNode);

	pNewNode->pSubNode[0] = NULL;
	pNewNode->pSubNode[1] = NULL;

	m_vecRefBspNode.push_back(pNewNode);
Exit0:
	return pNewNode;
}

HRESULT KG3DBsp::ConvertToSimpleFormatNode(BspNodeSimple* pDest,BspNodeEx* pSrc,DWORD& dwCurrentNode,DWORD& dwCurrentTriangle,Triangle* pNewTriangle)
{
	pDest->nSubNodeID[0] = -1;
	pDest->nSubNodeID[1] = -1;

	pDest->plane = pSrc->plane;
	pDest->nNumTriangles = (int)pSrc->vecTriangles.size();
	pDest->nTriangleStart = dwCurrentTriangle;
	
	for (size_t i=0;i<pSrc->vecTriangles.size();i++)
	{
		DWORD dwIndex = pSrc->vecTriangles[i];
		pNewTriangle[dwCurrentTriangle] = m_lpTriangles[dwIndex];
		dwCurrentTriangle++;
	}

	if(pSrc->pSubNode[0])
	{
		pDest->nSubNodeID[0] = (int)dwCurrentNode;
		BspNodeSimple* pNode = RequesetNode(dwCurrentNode);
		KGLOG_CHECK_ERROR(pNode);
		ConvertToSimpleFormatNode(pNode,pSrc->pSubNode[0],dwCurrentNode,dwCurrentTriangle,pNewTriangle);
	}

	if(pSrc->pSubNode[1])
	{
		pDest->nSubNodeID[1] = (int)dwCurrentNode;
		BspNodeSimple* pNode = RequesetNode(dwCurrentNode);
		KGLOG_CHECK_ERROR(pNode);
		ConvertToSimpleFormatNode(pNode,pSrc->pSubNode[1],dwCurrentNode,dwCurrentTriangle,pNewTriangle);
	}

	return S_OK;
}

HRESULT KG3DBsp::ConvertToSimpleFormat()
{
	DWORD dwCurrentNode = 0;
	DWORD dwCurrentTriangle = 0;

	Triangle* lpTriangles = NULL;

	SAFE_DELETE_ARRAY(m_lpNodes);

	m_pHead->dwNumNodes = (DWORD)m_vecRefBspNode.size();
	
	m_lpNodes = new BspNodeSimple[m_pHead->dwNumNodes];
	KGLOG_PROCESS_ERROR(m_lpNodes);

	ZeroMemory(m_lpNodes,sizeof(BspNodeSimple)*m_pHead->dwNumNodes);

	lpTriangles = new Triangle[m_pHead->dwTriangles];
	KGLOG_PROCESS_ERROR(lpTriangles);

	ZeroMemory(lpTriangles,sizeof(Triangle)*m_pHead->dwTriangles);

	m_lpRootNode = RequesetNode(dwCurrentNode);
	KGLOG_PROCESS_ERROR(m_lpRootNode);

	ConvertToSimpleFormatNode(m_lpRootNode,m_lpRootNodeEx,dwCurrentNode,dwCurrentTriangle,lpTriangles);

	memcpy(m_lpTriangles,lpTriangles,sizeof(Triangle)*m_pHead->dwTriangles);

	SAFE_DELETE_ARRAY(lpTriangles);
	return S_OK;
Exit0:
	SAFE_DELETE_ARRAY(m_lpNodes);
	SAFE_DELETE_ARRAY(lpTriangles);
	return E_FAIL;
}

KG3DBsp::BspNodeSimple* KG3DBsp::RequesetNode(DWORD& dwCurrentNode)
{
	KGLOG_PROCESS_ERROR(m_lpNodes);
	BspNodeSimple* pNode = &m_lpNodes[dwCurrentNode];
	dwCurrentNode++;

	return pNode;
Exit0:
	return NULL;
}

namespace {

    inline bool IsVertexInTriangle(const D3DXVECTOR3* v,
        const D3DXVECTOR3* a,
        const D3DXVECTOR3* b,
        const D3DXVECTOR3* c,
        const D3DXVECTOR3* n
        )
    {
        D3DXVECTOR3 vCross;

        D3DXVec3Cross(&vCross, &(*b - *a), &(*v - *a));
        D3DXVec3Normalize(&vCross, &vCross);

        if (D3DXVec3Dot(&vCross, n) < -0.5f)
            return false;

        D3DXVec3Cross(&vCross, &(*c - *b), &(*v - *b));
        D3DXVec3Normalize(&vCross, &vCross);

        if (D3DXVec3Dot(&vCross, n) < -0.5f)
            return false;

        D3DXVec3Cross(&vCross, &(*a - *c), &(*v - *c));
        D3DXVec3Normalize(&vCross, &vCross);

        if (D3DXVec3Dot(&vCross, n) < -0.5f)
            return false;

        return true;
    }
}

bool KG3DBsp::IsTriangeSphereIntersect(const KG3DBsp::Triangle& tri, const D3DXVECTOR3& vCenter, float fRadius)
{
    return IsSphereCrossTriangle(vCenter, fRadius, m_lpVertexs[tri.A], m_lpVertexs[tri.B], m_lpVertexs[tri.C]);
}

HRESULT KG3DBsp::GetFacesIntersectSphere(const D3DXVECTOR3& vCenter, float fRadius)
{
    if (!m_lpRootNode || !m_lpVertexs || !m_lpTriangles)
        return E_FAIL;

    DWORD* pTriIndies = new DWORD[m_pHead->dwTriangles];
    DWORD  dwTriNum = GetFacesIntersectSphere(m_lpRootNode, pTriIndies, vCenter, fRadius);

    for (DWORD i = 0; i < dwTriNum; ++i)
    {
        D3DXVECTOR3 v[] = { 
            m_lpVertexs[m_lpTriangles[pTriIndies[i]].A],
            m_lpVertexs[m_lpTriangles[pTriIndies[i]].B],
            m_lpVertexs[m_lpTriangles[pTriIndies[i]].C]
        };
        
        g_cGraphicsTool.DrawLine(&v[0], &v[1], 0xffffffff, 0xffffffff);
        g_cGraphicsTool.DrawLine(&v[1], &v[2], 0xffffffff, 0xffffffff);
        g_cGraphicsTool.DrawLine(&v[2], &v[0], 0xffffffff, 0xffffffff);
    }

    SAFE_DELETE_ARRAY(pTriIndies);
    return S_OK;
}

DWORD KG3DBsp::GetFacesIntersectSphere(BspNodeSimple* pNode, DWORD* pTris, const D3DXVECTOR3& vCenter, float fRadius)
{
    if (!pNode || !pTris)
        return 0;

    DWORD dwTriNum = 0;

    for (int i = 0, t = pNode->nTriangleStart; i < pNode->nNumTriangles; ++i, ++t)
        if (IsTriangeSphereIntersect(m_lpTriangles[t], vCenter, fRadius))
            pTris[dwTriNum++] = t;

    if (pNode->nSubNodeID[0] >= 0 || pNode->nSubNodeID[1] >= 0)
    {
        float fDot = D3DXPlaneDotCoord(&pNode->plane, &vCenter);

        bool bCheckPost = false;
        bool bCheckNegt = false;

        if (fabs(fDot) < fRadius)
        {
            if (pNode->nSubNodeID[0] >= 0)
                bCheckPost = true;

            if (pNode->nSubNodeID[1] >= 0)
                bCheckNegt = true;
        }
        else
        {
            if (fDot >= 0)
            {
                if (pNode->nSubNodeID[0] >= 0)
                     bCheckPost = true;
            }
            else
            {
                if (pNode->nSubNodeID[1] >= 0)
                     bCheckNegt = true;
            }
        }

        if (bCheckPost)
            dwTriNum += GetFacesIntersectSphere(&m_lpNodes[pNode->nSubNodeID[0]], pTris + dwTriNum, vCenter, fRadius);

        if (bCheckNegt)
            dwTriNum += GetFacesIntersectSphere(&m_lpNodes[pNode->nSubNodeID[1]], pTris + dwTriNum, vCenter, fRadius);
    }

    return dwTriNum;
}

DWORD KG3DBsp::GetFacesIntersectBox(BspNodeSimple* pNode, TriData* tris, const D3DXVECTOR3& boxHalfSize, const D3DXMATRIX& m)
{
    if (!pNode || !tris)
        return 0;

    DWORD dwTriNum = 0;

    for (int i = 0, t = pNode->nTriangleStart; i < pNode->nNumTriangles; ++i, ++t)
    {
        tris[dwTriNum][0] = m_lpVertexs[m_lpTriangles[t].A];
        tris[dwTriNum][1] = m_lpVertexs[m_lpTriangles[t].B];
        tris[dwTriNum][2] = m_lpVertexs[m_lpTriangles[t].C];
        D3DXVec3TransformCoordArray(tris[dwTriNum], sizeof(D3DXVECTOR3), tris[dwTriNum], sizeof(D3DXVECTOR3), &m, 3);
        if (IsTriangleBoxOverlap(D3DXVECTOR3(0.f, 0.f, 0.f), boxHalfSize, tris[dwTriNum]))
            ++dwTriNum;
    }

    if (pNode->nSubNodeID[0] >= 0 || pNode->nSubNodeID[1] >= 0)
    {
        D3DXPLANE plane;
        D3DXPlaneTransform(&plane, &pNode->plane, &m);

        D3DXVECTOR3 box[] = {
            D3DXVECTOR3(-boxHalfSize.x, 0.f, 0.f) + D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) + D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
            D3DXVECTOR3( boxHalfSize.x, 0.f, 0.f) + D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) + D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
            D3DXVECTOR3( boxHalfSize.x, 0.f, 0.f) + D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) - D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
            D3DXVECTOR3(-boxHalfSize.x, 0.f, 0.f) + D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) - D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
            D3DXVECTOR3(-boxHalfSize.x, 0.f, 0.f) - D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) + D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
            D3DXVECTOR3( boxHalfSize.x, 0.f, 0.f) - D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) + D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
            D3DXVECTOR3( boxHalfSize.x, 0.f, 0.f) - D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) - D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
            D3DXVECTOR3(-boxHalfSize.x, 0.f, 0.f) - D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) - D3DXVECTOR3(0.f, 0.f, boxHalfSize.z)
        };

        int nTick = 0;

        for (int i = 0; i < 8; ++i)
        {
            if (D3DXPlaneDotCoord(&plane, &box[i]) > 0)
                ++nTick;
            else
                --nTick;
        }

        bool bCheckPost = false;
        bool bCheckNegt = false;

        if (nTick != -8)
            if (pNode->nSubNodeID[0] >= 0)
                bCheckPost = true;

        if (nTick != 8)
            if (pNode->nSubNodeID[1] >= 0)
                bCheckNegt = true;

        if (bCheckPost)
            dwTriNum += GetFacesIntersectBox(&m_lpNodes[pNode->nSubNodeID[0]], tris + dwTriNum, boxHalfSize, m);

        if (bCheckNegt)
            dwTriNum += GetFacesIntersectBox(&m_lpNodes[pNode->nSubNodeID[1]], tris + dwTriNum, boxHalfSize, m);
    }

    return dwTriNum;
}

HRESULT KG3DBsp::GetFacesIntersectBox(const D3DXMATRIX& matBoxSpace, const D3DXVECTOR3& hs, find_face_callback ffc, void* param)
{
    if (!ffc)
        return E_FAIL;

    if (!m_lpRootNode || !m_lpVertexs || !m_lpTriangles)
        return E_FAIL;

    TriData* tris = new TriData[m_pHead->dwTriangles];
    DWORD  dwTriNum = GetFacesIntersectBox(m_lpRootNode, tris, hs, matBoxSpace);

    for (DWORD i = 0; i < dwTriNum; ++i)
    {
        bool bContinue = ffc(tris[i][0], tris[i][1], tris[i][2], hs, param);
        if (!bContinue)
            break;
    }

    SAFE_DELETE_ARRAY(tris);
    return S_OK;
}

BOOL KG3DBsp::GetRayIntersect(DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3 Origin,D3DXVECTOR3 Direction)
{
	if(m_lpRootNode)
	{
		D3DXVECTOR3 vEnd = Origin + Direction * m_pHead->fRadius;
		return GetRayIntersectNode(m_lpRootNode,dwCount,pInterPoint,dwMaxNum,Origin,Direction,vEnd);
	}
	else
	{
		return FALSE;
	}
}

BOOL KG3DBsp::GetRayIntersectNode(BspNodeSimple* pNode,DWORD& dwCount,D3DXVECTOR3* pInterPoint,DWORD dwMaxNum,D3DXVECTOR3& vOrigin,D3DXVECTOR3& vDirection,D3DXVECTOR3& vEnd)
{
	BOOL bFinded = FALSE;
	for (int i=0;i<pNode->nNumTriangles;i++)
	{
		int nIndex = pNode->nTriangleStart + i;

		Triangle* pTriangle = &m_lpTriangles[nIndex];
		
		float distance = 0;
	
		float U = 0;
		float V = 0;
		BOOL bInter = D3DXIntersectTri(&m_lpVertexs[pTriangle->A],&m_lpVertexs[pTriangle->B],&m_lpVertexs[pTriangle->C],
			&vOrigin,&vDirection,&U,&V,&distance);

		if(bInter)
		{
			bFinded = TRUE;

			if(dwCount<dwMaxNum)
			{
				pInterPoint[dwCount] = vOrigin + vDirection * distance;
				dwCount++;
			}
			else
			{
				return bFinded;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL bR_Start = PlaneRelateVertex(&pNode->plane,&vOrigin);
	BOOL bR_End   = PlaneRelateVertex(&pNode->plane,&vEnd   );

	if(bR_Start != bR_End)
	{
		if(pNode->nSubNodeID[0]>=0)
		{
			BspNodeSimple* pSubNode = &m_lpNodes[pNode->nSubNodeID[0]];
			bFinded |= GetRayIntersectNode(pSubNode,dwCount,pInterPoint,dwMaxNum,vOrigin,vDirection,vEnd);
		}

		if(pNode->nSubNodeID[1]>=0)
		{
			BspNodeSimple* pSubNode = &m_lpNodes[pNode->nSubNodeID[1]];
			bFinded |= GetRayIntersectNode(pSubNode,dwCount,pInterPoint,dwMaxNum,vOrigin,vDirection,vEnd);
		}
	}
	else
	{
		if (bR_Start)
		{
			if(pNode->nSubNodeID[0]>=0)
			{
				BspNodeSimple* pSubNode = &m_lpNodes[pNode->nSubNodeID[0]];
				bFinded |= GetRayIntersectNode(pSubNode,dwCount,pInterPoint,dwMaxNum,vOrigin,vDirection,vEnd);
			}
		}
		else
		{
			if(pNode->nSubNodeID[1]>=0)
			{
				BspNodeSimple* pSubNode = &m_lpNodes[pNode->nSubNodeID[1]];
				bFinded |= GetRayIntersectNode(pSubNode,dwCount,pInterPoint,dwMaxNum,vOrigin,vDirection,vEnd);
			}
		}
	}

	return bFinded;
}

BOOL KG3DBsp::RayIntersection( D3DXVECTOR3& vSrc, D3DXVECTOR3& vNormalizedDir, FLOAT* pfRet,D3DXVECTOR3* pFaceNormal
							   , KG3DIntersectionFilter* pFilter /*= NULL*/)
{
	static float fDist[1024];
	static D3DXVECTOR3 pNormal[1024];

	DWORD dwHitCount = 0;
	DWORD dwTestCount = 0;
	if(m_lpRootNode)
	{
		D3DXVECTOR3 vEnd = vSrc + vNormalizedDir * m_pHead->fRadius;
		
		BOOL bFind = RayIntersectionNode(m_lpRootNode,vSrc,vNormalizedDir,vEnd,pFilter,fDist,pNormal,dwHitCount,1024,dwTestCount);

		if(bFind)
		{
			float fMinHeight = 10000000;
			D3DXVECTOR3 outNormal;
			for (DWORD i=0;i<dwHitCount;i++)
			{
				if(fDist[i] > 0 && fMinHeight >= fDist[i])
				{
					fMinHeight = fDist[i];
					outNormal = pNormal[i];
				}
			}

			if(fMinHeight > 0)
			{
				(*pfRet) = fMinHeight;
				(*pFaceNormal) = outNormal;
				return TRUE;
			}
		}
	}

	return FALSE;
}

BOOL KG3DBsp::RayIntersectionNode(BspNodeSimple* pNode,D3DXVECTOR3& vSrc, D3DXVECTOR3& vNormalizedDir,D3DXVECTOR3& vEnd,
								  KG3DIntersectionFilter* pFilter,float* pDistance,D3DXVECTOR3* pFaceNormal,DWORD& dwHitCount,DWORD dwMaxHit,DWORD& dwTestCount)
{
	
	BOOL bFinded = FALSE;
	for (int i=0;i<pNode->nNumTriangles;i++)
	{
		int nIndex = pNode->nTriangleStart + i;

		Triangle* pTriangle = &m_lpTriangles[nIndex];

		float distance = 0;

		//BOOL bInter = IsLineIntersectTriangle(vSrc,vNormalizedDir,m_lpVertexs[pTriangle->A],m_lpVertexs[pTriangle->B],m_lpVertexs[pTriangle->C],pTriangle->Normal,TRUE,TRUE,distance);
		float U = 0;
		float V = 0;
		BOOL bInter = D3DXIntersectTri(&m_lpVertexs[pTriangle->A],&m_lpVertexs[pTriangle->B],&m_lpVertexs[pTriangle->C],
			&vSrc,&vNormalizedDir,&U,&V,&distance);
		dwTestCount++;

		if(bInter)
		{
			bFinded = TRUE;
			if(dwHitCount < dwMaxHit)
			{
				pDistance[dwHitCount] = distance;
				if(pFaceNormal)
				{
					pFaceNormal[dwHitCount] = pTriangle->Normal;
				}
				dwHitCount++;
			}
			else
			{
				return bFinded;
			}
		}
	}
	//////////////////////////////////////////////////////////////////////////
	BOOL bR_Start = PlaneRelateVertex(&pNode->plane,&vSrc);
	BOOL bR_End   = PlaneRelateVertex(&pNode->plane,&vEnd);

	if(bR_Start != bR_End)
	{
		if(pNode->nSubNodeID[0]>=0)
		{
			BspNodeSimple* pSubNode = &m_lpNodes[pNode->nSubNodeID[0]];
			bFinded |= RayIntersectionNode(pSubNode,vSrc,vNormalizedDir,vEnd,pFilter,pDistance,pFaceNormal,dwHitCount,dwMaxHit,dwTestCount);
		}

		if(pNode->nSubNodeID[1]>=0)
		{
			BspNodeSimple* pSubNode = &m_lpNodes[pNode->nSubNodeID[1]];
			bFinded |= RayIntersectionNode(pSubNode,vSrc,vNormalizedDir,vEnd,pFilter,pDistance,pFaceNormal,dwHitCount,dwMaxHit,dwTestCount);
		}
	}
	else
	{
		if (bR_Start)
		{
			if(pNode->nSubNodeID[0]>=0)
			{
				BspNodeSimple* pSubNode = &m_lpNodes[pNode->nSubNodeID[0]];
				bFinded |= RayIntersectionNode(pSubNode,vSrc,vNormalizedDir,vEnd,pFilter,pDistance,pFaceNormal,dwHitCount,dwMaxHit,dwTestCount);
			}
		}
		else
		{
			if(pNode->nSubNodeID[1]>=0)
			{
				BspNodeSimple* pSubNode = &m_lpNodes[pNode->nSubNodeID[1]];
				bFinded |= RayIntersectionNode(pSubNode,vSrc,vNormalizedDir,vEnd,pFilter,pDistance,pFaceNormal,dwHitCount,dwMaxHit,dwTestCount);
			}
		}
	}

	return bFinded;
}

HRESULT KG3DBsp::LoadFromMemory(IKG_Buffer *piFileData)
{
    HRESULT hrResult  = E_FAIL;
    int nRetCode = false;
    KG3DBufferReader BufferReader;

    KG_ASSERT_EXIT(piFileData);

    m_piFileData = piFileData;
    m_piFileData->AddRef();

    nRetCode = BufferReader.SetBuffer(m_piFileData);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = BufferReader.ReferenceRead((void **)&m_pHead, sizeof(_FILE_HEAD));
    KGLOG_PROCESS_ERROR(nRetCode);

    KGLOG_PROCESS_ERROR(m_pHead->dwFileMask == FILE_MASK);
    KGLOG_PROCESS_ERROR(m_pHead->dwVersion == FILE_VERSION);

    KGLOG_PROCESS_ERROR(m_pHead->dwNumVertex);
    KGLOG_PROCESS_ERROR(m_pHead->dwTriangles);
    KGLOG_PROCESS_ERROR(m_pHead->dwNumNodes);

    nRetCode = BufferReader.ReferenceRead((void **)&m_lpVertexs  ,sizeof(D3DXVECTOR3  )*m_pHead->dwNumVertex);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = BufferReader.ReferenceRead((void **)&m_lpTriangles,sizeof(Triangle     )*m_pHead->dwTriangles);
    KGLOG_PROCESS_ERROR(nRetCode);
    nRetCode = BufferReader.ReferenceRead((void **)&m_lpNodes    ,sizeof(BspNodeSimple)*m_pHead->dwNumNodes );
    KGLOG_PROCESS_ERROR(nRetCode);

    m_lpRootNode = &m_lpNodes[0];
    //////////////////////////////////////////////////////////////////////////
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        m_lpRootNode  = NULL;
        m_lpNodes     = NULL;
        m_lpTriangles = NULL;
        m_lpVertexs   = NULL;
        m_pHead       = NULL;
        KG_COM_RELEASE(m_piFileData);
    };
    return hrResult;
}
HRESULT KG3DBsp::GetMeshInfo(DWORD *pdwRetVertexCount, DWORD *pdwRetFaceCount)
{
    HRESULT hrResult = E_FAIL;

    KG_ASSERT_EXIT(pdwRetVertexCount);
    KG_ASSERT_EXIT(pdwRetFaceCount);
    
    KG_ASSERT_EXIT(m_pHead);
    *pdwRetVertexCount = m_pHead->dwNumVertex;
    *pdwRetFaceCount   = m_pHead->dwTriangles;

    hrResult = S_OK;
Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DBsp::SaveToFile(LPSTR pFileName)
{
    HRESULT hrResult = E_FAIL;
	IFile* pFile =  g_CreateFile(pFileName);
	KGLOG_PROCESS_ERROR(pFile);

    pFile->Write(m_pHead,       sizeof(*m_pHead));
	pFile->Write(m_lpVertexs,   sizeof(D3DXVECTOR3  )*m_pHead->dwNumVertex);
	pFile->Write(m_lpTriangles, sizeof(Triangle     )*m_pHead->dwTriangles);
	pFile->Write(m_lpNodes,     sizeof(BspNodeSimple)*m_pHead->dwNumNodes );

	hrResult = S_OK;
Exit0:
    KG_COM_RELEASE(pFile);
	return hrResult;
}

void KG3DBsp::Render(KG3DModel* pModel)
{
    if (!pModel)
        return;

    D3DXMATRIX matWorldSave;

    g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
    g_pd3dDevice->SetTransform(D3DTS_WORLD, &pModel->m_matWorld);

   if (m_lpRootNode)
       m_lpRootNode->Render(this, 0);


    // test

   /* static D3DXVECTOR3 boxHalfSize = D3DXVECTOR3(1000.f, 1000.f, 1000.f);

    static DWORD s = GetTickCount();

    if (GetTickCount() - s > 1000)
    {
        boxHalfSize.x = rand() % 1000;
        boxHalfSize.y = rand() % 1000;
        boxHalfSize.z = rand() % 1000;
        s = GetTickCount();
    }

    D3DXVECTOR3 box[] = {
        D3DXVECTOR3(-boxHalfSize.x, 0.f, 0.f) + D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) + D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
        D3DXVECTOR3( boxHalfSize.x, 0.f, 0.f) + D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) + D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
        D3DXVECTOR3( boxHalfSize.x, 0.f, 0.f) + D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) - D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
        D3DXVECTOR3(-boxHalfSize.x, 0.f, 0.f) + D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) - D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
        D3DXVECTOR3(-boxHalfSize.x, 0.f, 0.f) - D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) + D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
        D3DXVECTOR3( boxHalfSize.x, 0.f, 0.f) - D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) + D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
        D3DXVECTOR3( boxHalfSize.x, 0.f, 0.f) - D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) - D3DXVECTOR3(0.f, 0.f, boxHalfSize.z),
        D3DXVECTOR3(-boxHalfSize.x, 0.f, 0.f) - D3DXVECTOR3(0.f, boxHalfSize.y, 0.f) - D3DXVECTOR3(0.f, 0.f, boxHalfSize.z)
    };
    
    g_cGraphicsTool.DrawOBBox(box, 0xff0000ff);

    GetFacesIntersectBox(box);*/

    g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);
}

void KG3DBsp::BspNodeSimple::Render(KG3DBsp* pBsp, int nLevel)
{
    if (!pBsp)
        return;

    static int test = 0;

    if (nLevel > test)
        return;

    int i = nTriangleStart;

    while (i < nNumTriangles)
    {
        KG3DBsp::Triangle& tri = pBsp->m_lpTriangles[i];

        g_cGraphicsTool.DrawLine(&pBsp->m_lpVertexs[tri.A], &pBsp->m_lpVertexs[tri.B], 0xffffffff, 0xffffffff);
        g_cGraphicsTool.DrawLine(&pBsp->m_lpVertexs[tri.B], &pBsp->m_lpVertexs[tri.C], 0xffffffff, 0xffffffff);
        g_cGraphicsTool.DrawLine(&pBsp->m_lpVertexs[tri.C], &pBsp->m_lpVertexs[tri.A], 0xffffffff, 0xffffffff);

        ++i;
    }

}

void KG3DBspRender::Render()
{
    for (list<KG3DModel*>::iterator i = m_listModel.begin(); i != m_listModel.end(); ++i)
    {
        KG3DModel* pModel = *i;

        if (pModel && pModel->GetMesh() && pModel->GetMesh()->GetBsp())
            pModel->GetMesh()->GetBsp()->Render(pModel);
    }
}