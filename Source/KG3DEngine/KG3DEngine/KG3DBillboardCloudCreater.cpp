#include "StdAfx.h"
#include "KG3DBillboardCloudCreater.h"
#include "KG3DMesh.h"
#include "KG3DGraphicsTool.h"
#include "IEEditor.h"
#include "KG3DTextureTable.h"
#include "KG3DModelSpeedTree.h"
#include "IEKG3DTexture.h"
#include "KG3DBillboardCloud.h"
#include "KG3DGraphicsStructEx.h"
#include "KG3DFrameDrawer.h"
#include "KG3DSystemEventListener.h"
#include "KG3DRenderTargetTool.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace {

	void OffsetRect(FRECT& rect, float x, float y)
	{
		rect.Left += x;
		rect.Bottom += y;
	}

	void MoveRect(FRECT& rect, float x, float y)
	{
		OffsetRect(rect, x - rect.Left, y - rect.Bottom);
	}

	bool CheckPos(const vector<KG3DBillboardCloudCreater::BillboardPlane*>& vBillBoards)
	{
		const FRECT& rect = vBillBoards.back()->vRectUV;
		for (size_t i = 0; i < vBillBoards.size() - 1; ++i)
			if (vBillBoards[i]->vRectUV.IsIntersect(rect))
				return false;
		return true;
	}
}

float KG3DBillboardCloudCreater::CalcRange(const vector<KG3DBillboardCloudCreater::BillboardPlane*>& vBillBoards)
{
	float max_x = FLT_MIN;
	float max_y = FLT_MIN;

	float min_x = FLT_MAX;
	float min_y = FLT_MAX;

	for (size_t i = 0; i < vBillBoards.size(); ++i)
	{
		KG3DBillboardCloudCreater::_Rect rect = {
			vBillBoards[i]->vRectUV.Left, 
			vBillBoards[i]->vRectUV.Bottom,
			vBillBoards[i]->vRectUV.Left + vBillBoards[i]->vRectUV.Width,
			vBillBoards[i]->vRectUV.Bottom + vBillBoards[i]->vRectUV.Height
		};

		if (rect.left < min_x)
			min_x = rect.left;

		if (rect.right > max_x)
			max_x = rect.right;

		if (rect.top < min_y)
			min_y = rect.top;

		if (rect.bottom > max_y)
			max_y = rect.bottom;
	}

	return max(max_x - min_x, max_y - min_y);
}



KG3DBillboardCloudCreater::KG3DBillboardCloudCreater(void)
{
	m_lpTextureColor = NULL;
	m_lpTextureNormal = NULL;
	m_lpModelEdtior = NULL;
	m_fPrecisionTemp = 0;
	m_fPrecision = 0.05F;
	m_dwTextureWidth = 512;

	m_lpModel = NULL;
	m_bTwitterPlane = TRUE;
	m_dwState = EM_BILLBOARD_CREATE_STOP;
	m_fTexInteval = 1.f;
	m_dwTexNumber = 0;

	ZeroMemory(m_apTexArray, _countof(m_apTexArray) * sizeof(KG3DTexture*));

	m_vFronts[0] = D3DXVECTOR3( 0.f,  0.f,  1.f);
	m_vFronts[1] = D3DXVECTOR3( 1.f,  0.f,  0.f);
	m_vFronts[2] = D3DXVECTOR3( 0.71f,  0.71f,  0.f);
	m_vFronts[3] = D3DXVECTOR3( -0.71f,  0.71f,  0.f);
	m_vFronts[4] = D3DXVECTOR3( 0.f,  0.71f,  0.71f);
	m_vFronts[5] = D3DXVECTOR3( 0.f,  0.71f, -0.71f);

	m_lpRenderTargetTool = NULL;

	m_pBillboardCloud = NULL;
}

KG3DBillboardCloudCreater::~KG3DBillboardCloudCreater(void)
{
	SAFE_DELETE(m_lpRenderTargetTool);

	SAFE_RELEASE(m_lpTextureColor);
	SAFE_RELEASE(m_lpTextureNormal);

	m_lpModelEdtior = NULL;
	m_lpModel = NULL;

	SAFE_RELEASE(m_pBillboardCloud);
}

float KG3DBillboardCloudCreater::TriangleArea(Triangle* pTriangle)
{
	D3DXVECTOR3 AB,BC,CA;
	AB = pTriangle->vVertexPos[1] - pTriangle->vVertexPos[0];
	BC = pTriangle->vVertexPos[2] - pTriangle->vVertexPos[1];
	CA = pTriangle->vVertexPos[0] - pTriangle->vVertexPos[2];

	float a = D3DXVec3Length(&AB);
	float b = D3DXVec3Length(&BC);
	float c = D3DXVec3Length(&CA);

	float p = 0.5F * (a + b + c);

	pTriangle->fArea = sqrt(p*(p - a)*(p - b)*(p - c));
	return pTriangle->fArea;
}

void KG3DBillboardCloudCreater::StartBillBoardCreating()
{
	m_vecBillboardPlanes.clear();
	m_dwState = EM_BILLBOARD_CREATE_INPUT_MESH;
}

void KG3DBillboardCloudCreater::StopBillBoardCreating()
{
	m_vecTrianglesCopy.clear();
	m_dwState = EM_BILLBOARD_CREATE_STOP;
}

void KG3DBillboardCloudCreater::PipelineBillBoardCreating()
{
	switch (m_dwState)
	{
	case EM_BILLBOARD_CREATE_INPUT_MESH :
		{
			/*
			if (!m_lpModelEdtior || !m_lpModelEdtior->m_Models.size() || !m_lpModelEdtior->m_Models[0])
			{
			m_dwState = EM_BILLBOARD_CREATE_STOP;
			break;
			}*/
			IEKG3DModel* pIEModel= NULL;
			if (NULL == m_lpModelEdtior)
				m_dwState = EM_BILLBOARD_CREATE_STOP;

			HRESULT hr = m_lpModelEdtior->GetCurModel(&pIEModel);

			if(FAILED(hr))
				break;

			m_lpModel = dynamic_cast<KG3DModel*>(pIEModel);
			_ASSERTE(NULL != m_lpModel);

			if (FAILED(InputModel(m_lpModel)))
			{
				m_dwState = EM_BILLBOARD_CREATE_STOP;
				break;
			}

			m_dwState = EM_BILLBOARD_CREATE_PLANE;

			if (m_lpTextureColor)
			{
				IDirect3DSurface9* pSurface = NULL;
				m_lpTextureColor->GetSurfaceLevel(0, &pSurface);
				if (pSurface)
					g_pd3dDevice->ColorFill(pSurface, NULL, 0xffffffff);
				SAFE_RELEASE(pSurface);
			}
			if (m_lpTextureNormal)
			{
				IDirect3DSurface9* pSurface = NULL;
				m_lpTextureNormal->GetSurfaceLevel(0, &pSurface);
				if (pSurface)
					g_pd3dDevice->ColorFill(pSurface, NULL, 0xffffffff);
				SAFE_RELEASE(pSurface);
			}
			m_vecTrianglesCopy = m_vecTriangles;
		}
		break;
	case EM_BILLBOARD_CREATE_PLANE :
		{
			BillboardPlane newBillboard;
			if (SUCCEEDED(FindBillboardPlane(newBillboard,&m_vecTrianglesCopy, m_fPrecisionTemp)))
			{
				newBillboard.ComputeAxis();
				newBillboard.ComputeBBox();

				if (newBillboard.vBBox.GetLength() * newBillboard.vBBox.GetHeight() >= m_fPrecisionTemp * m_fPrecisionTemp)
					m_vecBillboardPlanes.push_back(newBillboard);
			}
			else
			{
				ProcessBillBoardUV();
				m_dwState = EM_BILLBOARD_CREATE_CALC_UV;
			}
		}
		break;

	case EM_BILLBOARD_CREATE_CALC_UV :
		{
			if (!CalcBillBoardUV())
			{
				if (fabs(m_fTexInteval) < FLT_EPSILON)
				{
					float fTexIneval = CalcRange(m_vecBillboardTried) * 0.02f;

					m_listBillboardToTry.clear();
					m_vecBillboardTried.clear();
					m_listMovePos.clear();
					m_listMovePos.push_back(_Point(0, 0));

					for (size_t i = 0; i < m_vecBillboardPlanes.size(); i++)
						m_listBillboardToTry.push_back(&m_vecBillboardPlanes[i]);

					m_fTexInteval = fTexIneval;
				}
				else
					m_dwState = EM_BILLBOARD_RENDER_TEXTURE;
			}
		}
		break;

	case EM_BILLBOARD_RENDER_TEXTURE :
		{
			ComputeTexture();
			m_dwState = EM_BILLBOARD_CREATE_STOP;
		}
		break;

	default :
		break;
	}

}


HRESULT KG3DBillboardCloudCreater::FindBillboardPlane(BillboardPlane& newPlane,vector<Triangle>* pvecTrianlges,float e)
{
	float fMaxArea = 0;
	int N = (int)pvecTrianlges->size();

	if (N <= 0)
		return E_FAIL;

	for (int i = 0; i < N; i++)
	{
		//////////////////////////////////////////////////////////////////////////
		//随机挑选一个三角形，并根据这个三角形创建公告板，增添一些随机偏转的角度

		int nIndex = 0;

		if (m_bTwitterPlane)
			nIndex = rand()%N;
		else
			nIndex = i;

		Triangle& vTriangle = (*pvecTrianlges)[nIndex];

		D3DXVECTOR3 vBillboardPos[3];

		if (m_bTwitterPlane)
		{
			for (int j=0;j<3;j++)
			{
				float d  = e * ((rand()%100)*0.02F - 1.0F) * 0.01F;
				vBillboardPos[j] = vTriangle.vVertexPos[j] + d * vTriangle.vTriangleNormal;
			}
		}
		else
		{
			for (int j=0;j<3;j++)
				vBillboardPos[j] = vTriangle.vVertexPos[j];
		}

		D3DXVECTOR3 vBillCenter = (vBillboardPos[0]+vBillboardPos[1]+vBillboardPos[2])/3.0F;

		//////////////////////////////////////////////////////////////////////////
		//创建一个新的公告板
		BillboardPlane vBillboard;
		vBillboard.fArea = 0;
		vBillboard.vecReferenceTriangle.clear();

		D3DXPlaneFromPoints(&vBillboard.vPlane,
			&vBillboardPos[0],
			&vBillboardPos[1],
			&vBillboardPos[2]);

		vBillboard.vPlaneNormal.x = vBillboard.vPlane.a;
		vBillboard.vPlaneNormal.y = vBillboard.vPlane.b;
		vBillboard.vPlaneNormal.z = vBillboard.vPlane.c;

		D3DXPlaneIntersectLine(&vBillboard.vCenter,&vBillboard.vPlane,&vBillCenter,&(vBillCenter+vBillboard.vPlaneNormal*10000));

		//////////////////////////////////////////////////////////////////////////
		//将每一个三角形投影到公告板平面上，如果满足条件就加入公告板

		for (int K = 0;K < N; K++)
		{
			Triangle& vTri = (*pvecTrianlges)[K];
			D3DXVECTOR3 v[3];

			BOOL bNear = TRUE;
			for (int j=0;j<3;j++)
			{
				D3DXVECTOR3 A = vTri.vVertexPos[j];
				D3DXVECTOR3 B = A + vBillboard.vPlaneNormal*100000;
				D3DXVECTOR3 O;

				D3DXVECTOR3* pOut = D3DXPlaneIntersectLine(&O,&vBillboard.vPlane,&A,&B);
				if(pOut)
				{
					float test0 = abs(D3DXVec3Dot(&vBillboard.vPlaneNormal,&(O - A)));
					//float test1 = D3DXVec3Length(&(O-vBillboard.vCenter)) * 0.001F;

					if( test0 >= e)
						bNear = FALSE;
				}
			}

			if(bNear)
			{
				vBillboard.fArea += vTri.fArea * abs( D3DXVec3Dot ( &vBillboard.vPlaneNormal, &vTri.vTriangleNormal ));
				vBillboard.vecReferenceTriangle.push_back(vTri);
			}
		}

		//////////////////////////////////////////////////////////////////////////
		if (fMaxArea < vBillboard.fArea)
		{
			fMaxArea = vBillboard.fArea;
			newPlane = vBillboard;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	//获得最大引用的公告板后，将涉及的三角形从序列中删除
	for (size_t t = 0;t<newPlane.vecReferenceTriangle.size();t++)
	{
		int nIndex = newPlane.vecReferenceTriangle[t].nIndex;

		vector<Triangle>::iterator i = pvecTrianlges->begin();
		vector<Triangle>::iterator iend = pvecTrianlges->end();
		while (i!=iend)
		{
			Triangle& trian = *i;
			if (trian.nIndex == nIndex)
			{
				pvecTrianlges->erase(i);
				break;
			}
			else
			{
				i++;
			}
		}
	}
	return S_OK;
}

HRESULT KG3DBillboardCloudCreater::InputModel(KG3DModel* pModel)
{
	ASSERT(pModel);

	m_vecBillboardPlanes.clear();
	m_vecTriangles.clear();

	m_vModelOrigin = pModel->m_BBox.GetCenter();
	m_fPrecisionTemp = pModel->m_BBox.GetDiameter() * m_fPrecision;
	m_vTransWhenRenderBillboards = D3DXVECTOR3(pModel->m_BBox.GetLength() + 100.f ,0,0);

	switch (pModel->GetType())
	{
	case MESHTYPE_SPEEDTREE :
		{
			KG3DModelSpeedTree* pTree = dynamic_cast<KG3DModelSpeedTree*>(m_lpModel);
			if (!pTree || !pTree->m_lpModelTree || !pTree->m_lpModelTree->m_lpMaterial || !pTree->m_lpMaterial)
				return E_FAIL;

			KG3DMaterial::KG3DMaterialSubset* pSubset = NULL;
			KG3DTexture* pTexture = NULL;
			m_dwTexNumber = 0;

			pSubset  = pTree->m_lpMaterial->GetSubMaterial(0);
			pTexture = NULL;

			KGLOG_PROCESS_ERROR(pSubset);

			pTexture = pSubset->m_lpTextures[0];

			m_apTexArray[m_dwTexNumber++] = pTexture;

			return InputSpeedTree(pTree);
		}
		break;
	default :
		{
			if (!m_lpModel || !m_lpModel->m_lpMaterial)
				return E_FAIL;

			KG3DMaterial::KG3DMaterialSubset* pSubset = NULL;
			KG3DTexture* pTexture = NULL;
			m_dwTexNumber = 0;

			for (; m_dwTexNumber < m_lpModel->m_lpMaterial->m_dwNumMaterials; ++m_dwTexNumber)
			{
				pSubset  = m_lpModel->m_lpMaterial->GetSubMaterial(m_dwTexNumber);
				pTexture = NULL;

				KGLOG_PROCESS_ERROR(pSubset);
				
				pTexture = pSubset->m_lpTextures[0];
				m_apTexArray[m_dwTexNumber] = pTexture;
			}

			return InputMesh(m_lpModel->GetMesh());
		}
		break;
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DBillboardCloudCreater::OnLostDevice()
{
	SAFE_RELEASE(m_lpTextureColor);
	SAFE_RELEASE(m_lpTextureNormal);
	SAFE_DELETE(m_lpRenderTargetTool);

    return S_OK;
}

HRESULT KG3DBillboardCloudCreater::OnResetDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    hr = CreateTextures();
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    return hrResult;
}

namespace {
	inline float GetRandom(float fMin, float fMax)
	{
		float fRandNum = (float)rand() / RAND_MAX;
		return (fMin + (fMax - fMin) * fRandNum);
	}
};

KG3DBillboardCloudCreater::_VextorOffset::_VextorOffset(float fSize, float ua, float ub, float uc, float ud,
														float va, float vb, float vc, float vd )
{
	/* 
	* position
	*/

	vOffsets[0][0] = D3DXVECTOR3(-fSize, -fSize, 0.f);
	vOffsets[0][1] = D3DXVECTOR3( fSize, -fSize, 0.f);
	vOffsets[0][2] = D3DXVECTOR3(-fSize,  fSize, 0.f);

	vOffsets[1][0] = D3DXVECTOR3( fSize, -fSize, 0.f);
	vOffsets[1][1] = D3DXVECTOR3( fSize,  fSize, 0.f);
	vOffsets[1][2] = D3DXVECTOR3(-fSize,  fSize, 0.f);

	vOffsets[2][0] = D3DXVECTOR3(0.f, -fSize, -fSize);
	vOffsets[2][1] = D3DXVECTOR3(0.f,  fSize, -fSize);
	vOffsets[2][2] = D3DXVECTOR3(0.f, -fSize,  fSize);

	vOffsets[3][0] = D3DXVECTOR3(0.f, fSize, -fSize);
	vOffsets[3][1] = D3DXVECTOR3(0.f,  fSize,  fSize);
	vOffsets[3][2] = D3DXVECTOR3(0.f, -fSize,  fSize);

	vOffsets[4][0] = D3DXVECTOR3(-fSize, 0.f, -fSize);
	vOffsets[4][1] = D3DXVECTOR3( fSize, 0.f, -fSize);
	vOffsets[4][2] = D3DXVECTOR3(-fSize, 0.f,  fSize);

	vOffsets[5][0] = D3DXVECTOR3( fSize, 0.f, -fSize);
	vOffsets[5][1] = D3DXVECTOR3( fSize, 0.f,  fSize);
	vOffsets[5][2] = D3DXVECTOR3(-fSize, 0.f,  fSize);


	/* 
	* uv
	*/

	vUVs[0][0] = D3DXVECTOR2(ud, vd);
	vUVs[0][1] = D3DXVECTOR2(uc, vc);
	vUVs[0][2] = D3DXVECTOR2(ub, vb);

	vUVs[1][0] = D3DXVECTOR2(uc, vc);
	vUVs[1][1] = D3DXVECTOR2(ua, va);
	vUVs[1][2] = D3DXVECTOR2(ub, vb);

	vUVs[2][0] = D3DXVECTOR2(ud, vd);
	vUVs[2][1] = D3DXVECTOR2(uc, vc);
	vUVs[2][2] = D3DXVECTOR2(ub, vb);

	vUVs[3][0] = D3DXVECTOR2(uc, vc);
	vUVs[3][1] = D3DXVECTOR2(ua, va);
	vUVs[3][2] = D3DXVECTOR2(ub, vb);

	vUVs[4][0] = D3DXVECTOR2(ud, vd);
	vUVs[4][1] = D3DXVECTOR2(uc, vc);
	vUVs[4][2] = D3DXVECTOR2(ub, vb);

	vUVs[5][0] = D3DXVECTOR2(uc, vc);
	vUVs[5][1] = D3DXVECTOR2(ua, va);
	vUVs[5][2] = D3DXVECTOR2(ub, vb);


	/* 
	* normal
	*/

	vNormal[0] = D3DXVECTOR3( 0.f,  0.f, -1.f);
	vNormal[1] = D3DXVECTOR3( 0.f,  0.f,  1.f);
	vNormal[2] = D3DXVECTOR3( 1.f,  0.f,  0.f);
	vNormal[3] = D3DXVECTOR3( -1.f, 0.f,  0.f);
	vNormal[4] = D3DXVECTOR3( 0.f,  1.f,  0.f);
	vNormal[5] = D3DXVECTOR3( 0.f, -1.f,  0.f);
}

HRESULT KG3DBillboardCloudCreater::InputSpeedTree(KG3DModelSpeedTree* pSTree)
{
	ASSERT(pSTree);

	/* if (FAILED(InputMesh(dynamic_cast<KG3DMesh*>(pSTree->m_lpModelTree->m_lpMesh))))
	return E_FAIL;*/

	if (!pSTree->m_lpLeafsData)
		return E_FAIL;

	for (LEAFIter_t i = pSTree->m_lpLeafsData->m_ListLeaf.begin(); 
		i != pSTree->m_lpLeafsData->m_ListLeaf.end(); ++i)
	{
		LPLEAF leaf = *i;

		float fSize = leaf->Size;

		float tuA = leaf->TextureUV % 2 * 0.5f;
		float tuB = tuA + 0.5f;
		float tuC = tuA;
		float tuD = tuA + 0.5f;

		float tvA = leaf->TextureUV / 2 * 0.5f;
		float tvB = tvA;
		float tvC = tvA + 0.5f;
		float tvD = tvA + 0.5f;

		int cnt = sizeof(m_vFronts) / sizeof(m_vFronts[0]);
		for (int s = 0; s < cnt * 2; ++s)
		{
			Triangle vTriangle;
			vTriangle.nIndex = static_cast<int>(m_vecTriangles.size());

			D3DXMATRIX mat;
			D3DXMatrixLookAtLH(&mat, &D3DXVECTOR3(0.f, 0.f, 0.f), &m_vFronts[s / 2], &D3DXVECTOR3(0.f, 1.f, 0.f));
			D3DXMatrixInverse(&mat, NULL, &mat);

			D3DXVECTOR3 vLeft  = D3DXVECTOR3(mat._11, mat._12, mat._13) * fSize;
			D3DXVECTOR3 vUp    = D3DXVECTOR3(mat._21, mat._22, mat._23) * fSize;
			D3DXVECTOR3 vFront = D3DXVECTOR3(mat._31, mat._32, mat._33);

			if (!(s % 2))
			{
				vTriangle.vVertexPos[0] = leaf->OriginPosition - vLeft - vUp;
				vTriangle.vVertexPos[1] = leaf->OriginPosition - vLeft + vUp;
				vTriangle.vVertexPos[2] = leaf->OriginPosition + vLeft - vUp;

				vTriangle.vVertexUV1[0] = D3DXVECTOR2(tuD, tvD);
				vTriangle.vVertexUV1[1] = D3DXVECTOR2(tuC, tvC);
				vTriangle.vVertexUV1[2] = D3DXVECTOR2(tuB, tvB);
			}
			else
			{
				vTriangle.vVertexPos[0] = leaf->OriginPosition + vLeft - vUp;
				vTriangle.vVertexPos[1] = leaf->OriginPosition + vLeft + vUp;
				vTriangle.vVertexPos[2] = leaf->OriginPosition - vLeft - vUp;

				vTriangle.vVertexUV1[0] = D3DXVECTOR2(tuC, tvC);
				vTriangle.vVertexUV1[1] = D3DXVECTOR2(tuA, tvA);
				vTriangle.vVertexUV1[2] = D3DXVECTOR2(tuB, tvB);
			}

			vTriangle.vVertexNormal[0]  = m_vFronts[s / 2];
			vTriangle.vVertexNormal[1]  = m_vFronts[s / 2];
			vTriangle.vVertexNormal[2]  = m_vFronts[s / 2];

			vTriangle.m_dwSubSet = m_dwTexNumber - 1;

			// check triangle 

			if ((vTriangle.vVertexPos[0] == vTriangle.vVertexPos[1]) ||
				(vTriangle.vVertexPos[0] == vTriangle.vVertexPos[2]) ||
				(vTriangle.vVertexPos[1] == vTriangle.vVertexPos[2]))
				continue;

			D3DXVECTOR3 v1 = vTriangle.vVertexPos[1] - vTriangle.vVertexPos[0];
			D3DXVECTOR3 v2 = vTriangle.vVertexPos[2] - vTriangle.vVertexPos[0];

			D3DXVec3Normalize(&v1, &v1);
			D3DXVec3Normalize(&v2, &v2);

			float fDot = D3DXVec3Dot(&v1, &v2);

			if ((fabs(fDot - 1.f) < FLT_EPSILON) || (fabs(fDot + 1.f) < FLT_EPSILON))
				continue;


			D3DXPLANE plane;
			D3DXPlaneFromPoints(&plane,
				&vTriangle.vVertexPos[0],
				&vTriangle.vVertexPos[1],
				&vTriangle.vVertexPos[2]);
			vTriangle.vTriangleNormal = D3DXVECTOR3(plane.a,plane.b,plane.c);

			TriangleArea(&vTriangle);

			m_vecTriangles.push_back(vTriangle);
		}

	}

	return S_OK;
}

HRESULT KG3DBillboardCloudCreater::InputMesh(KG3DMesh* pMesh)	//把Mesh里面的顶点分解出来放到m_vecTriangles里面
{
	HRESULT hr = S_OK;
	KGLOG_PROCESS_ERROR(pMesh);
	KGLOG_PROCESS_ERROR(pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]);	

	DWORD dwFVF = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->GetFVF();
	//DWORD dwNumVertices = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->GetNumVertices();
	DWORD dwNumFaces = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->GetNumFaces();

	BOOL bHasTex2 = FALSE;
	if (dwFVF == (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1))
	{
		bHasTex2 = FALSE;
	}
	else if (dwFVF == (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2))
	{
		bHasTex2 = TRUE;
	}
	else
	{
		//不能处理的Mesh
		_KG3D_DEBUG_OUTPUT0(_T("KG3DBillboardCloudCreater，无法识别的Mesh类型"));
		goto Exit0;
	}

	{
		//VFormat::FACES_NORMAL_TEXTURE1 * pVers = NULL;
		VOID* pVertexBuffer = NULL;
		WORD* pIndexBuffer = NULL;
		DWORD* pSubSetIDBuffer = NULL;

		hr = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->LockVertexBuffer(0, &pVertexBuffer);
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		hr = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->LockIndexBuffer (0, (void**) &pIndexBuffer);
		if (FAILED(hr))
		{
			pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockVertexBuffer();
		}
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		hr = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->LockAttributeBuffer(0,&pSubSetIDBuffer);
		if (FAILED(hr))
		{
			pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockVertexBuffer();
			pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockIndexBuffer();
		}
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		for(DWORD i = 0; i < dwNumFaces; i++)
		{
			Triangle vTriangle;
			vTriangle.nIndex = i;
			for (int j=0;j<3;j++)
			{
				int vertexIndex = pIndexBuffer[i*3+j];

				if (bHasTex2)
				{
					VFormat::FACES_NORMAL_TEXTURE2* pVer = reinterpret_cast<VFormat::FACES_NORMAL_TEXTURE2*>(pVertexBuffer);
					pVer += vertexIndex;
					vTriangle.vVertexPos[j] = pVer->p;
					vTriangle.vVertexNormal[j] = pVer->Normal;
					vTriangle.vVertexUV1[j].x = pVer->tu1;
					vTriangle.vVertexUV1[j].y = pVer->tv1;
					vTriangle.vVertexUV2[j].x = pVer->tu2;
					vTriangle.vVertexUV2[j].y = pVer->tv2;
				}
				else
				{
					VFormat::FACES_NORMAL_TEXTURE1* pVer = reinterpret_cast<VFormat::FACES_NORMAL_TEXTURE1*>(pVertexBuffer);
					pVer += vertexIndex;
					vTriangle.vVertexPos[j] = pVer->p;
					vTriangle.vVertexNormal[j] = pVer->Normal;
					vTriangle.vVertexUV1[j].x = pVer->tu1;
					vTriangle.vVertexUV1[j].y = pVer->tv1;
				}
			}
			vTriangle.m_dwSubSet = pSubSetIDBuffer[i];


			// 排除面积为0的点

			/*
			if ((vTriangle.vVertexPos[0] == vTriangle.vVertexPos[1]) ||
			(vTriangle.vVertexPos[0] == vTriangle.vVertexPos[2]) ||
			(vTriangle.vVertexPos[1] == vTriangle.vVertexPos[2]))
			continue;*/
			if (MT_VECTOR_EQUAL(vTriangle.vVertexPos[0], vTriangle.vVertexPos[1], FLT_EPSILON)
				|| MT_VECTOR_EQUAL(vTriangle.vVertexPos[0], vTriangle.vVertexPos[2], FLT_EPSILON)
				|| MT_VECTOR_EQUAL(vTriangle.vVertexPos[1], vTriangle.vVertexPos[2], FLT_EPSILON))
				continue;	//用MT_VECTOR_EQUAL宏，D3DXVECTOR9的==操作符是直接的==,这样是不对的


			D3DXVECTOR3 v1 = vTriangle.vVertexPos[1] - vTriangle.vVertexPos[0];
			D3DXVECTOR3 v2 = vTriangle.vVertexPos[2] - vTriangle.vVertexPos[0];

			D3DXVec3Normalize(&v1, &v1);
			D3DXVec3Normalize(&v2, &v2);

			float fDot = D3DXVec3Dot(&v1, &v2);

			if ((fabs(fDot - 1.f) < FLT_EPSILON) || (fabs(fDot + 1.f) < FLT_EPSILON))
				continue;


			D3DXPLANE plane;
			D3DXPlaneFromPoints(&plane,
				&vTriangle.vVertexPos[0],
				&vTriangle.vVertexPos[1],
				&vTriangle.vVertexPos[2]);
			vTriangle.vTriangleNormal = D3DXVECTOR3(plane.a,plane.b,plane.c);

			TriangleArea(&vTriangle);

			m_vecTriangles.push_back(vTriangle);
		}

		hr = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockVertexBuffer();
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		hr = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockIndexBuffer();
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

		hr = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL]->UnlockAttributeBuffer();
		KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}
//
//HRESULT KG3DBillboardCloudCreater::CreateBillBoardCloudForModel()
//{
//	HRESULT hr = S_OK;
//	KGLOG_PROCESS_ERROR(m_lpModelEdtior);
//
//	KGLOG_PROCESS_ERROR(m_lpModelEdtior->m_Models.size());
//
//	KG3DModel* pModel = m_lpModelEdtior->m_Models[0];
//	KGLOG_PROCESS_ERROR(pModel);
//	m_lpModel = pModel;
//
//	KGLOG_PROCESS_ERROR(pModel->m_lpMesh);
//	KG3DMesh* pMesh = static_cast<KG3DMesh*>(pModel->m_lpMesh);
//	KGLOG_PROCESS_ERROR(pMesh);
//
//  /*  if (m_lpTexture)
//    {
//        IDirect3DSurface9* pSurface = NULL;
//        m_lpTexture->GetSurfaceLevel(0, &pSurface);
//        if (pSurface)
//        {
//            g_pd3dDevice->ColorFill(pSurface, NULL, 0xffa0a0a0);
//            pSurface->Release();
//        }
//    }*/
//
//
//	hr = InputMesh(pMesh);
//	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
//
//	hr = CreateBillboards();
//	KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);
//
//	return S_OK;
//Exit0:
//	return E_FAIL;
//}

HRESULT KG3DBillboardCloudCreater::BillboardPlane::ComputeAxis()
{
	//D3DXVECTOR3 vO;
	//D3DXVECTOR3 A = D3DXVECTOR3(0,0,0);//pCreater->m_vOrigin;
	//D3DXVECTOR3 B = A + vPlaneNormal*10000;
	//D3DXPlaneIntersectLine(&vO,&vPlane,&A,&B);

	D3DXMATRIX mat;
	D3DXVECTOR3 vAt = vCenter + vPlaneNormal*10000;
	D3DXMatrixLookAtRH(&mat,&vCenter,&vAt,&D3DXVECTOR3(0.00001F,0.9999997F,0));

	vAxisX = D3DXVECTOR3(mat._11,mat._21,mat._31);
	vAxisY = D3DXVECTOR3(mat._12,mat._22,mat._32);

	return S_OK;
}

HRESULT KG3DBillboardCloudCreater::BillboardPlane::ComputeBBox()
{
	vBBox.Clear();

	for (size_t i=0;i<vecReferenceTriangle.size();i++)
	{
		Triangle& Trian = vecReferenceTriangle[i];

		for (size_t j=0;j<3;j++)
		{
			D3DXVECTOR3 vO;
			D3DXVECTOR3 A = Trian.vVertexPos[j];
			D3DXVECTOR3 B = A + vPlaneNormal * 10000;
			D3DXPlaneIntersectLine(&vO,&vPlane,&A,&B);

			float X = D3DXVec3Dot(&vAxisX,&(vO-vCenter));
			float Y = D3DXVec3Dot(&vAxisY,&(vO-vCenter));
			float Z = D3DXVec3Dot(&vPlaneNormal,&(A-vCenter));

			vBBox.AddPosition(D3DXVECTOR3(X,Y,Z));
		}

	}

	return S_OK;
}

bool SortComp(KG3DBillboardCloudCreater::BillboardPlane pElem1, KG3DBillboardCloudCreater::BillboardPlane pElem2)
{
	return max(pElem1.vBBox.GetLength(),pElem1.vBBox.GetHeight()) > max(pElem2.vBBox.GetLength(),pElem2.vBBox.GetHeight()) ;
}

HRESULT KG3DBillboardCloudCreater::Render()
{
	PipelineBillBoardCreating();

	if (!m_lpModel)
		return S_OK;

	GraphicsStruct::RenderStateAlpha Alpha(0x10,TRUE,FALSE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);
	GraphicsStruct::RenderState State( D3DRS_LIGHTING, FALSE );
	GraphicsStruct::RenderState  alphaFun(D3DRS_ALPHAFUNC,D3DCMP_GREATEREQUAL);

	//把贴图画出来
	{
		D3DXVECTOR3 A,B,C,D;
		D3DXVECTOR3 vTrans = D3DXVECTOR3(-m_lpModel->m_BBox.GetLength() * 0.5f, 0.f, -m_lpModel->m_BBox.GetWidth() + 100.f);

		A = vTrans + D3DXVECTOR3(0              ,0,0);
		B = vTrans + D3DXVECTOR3(m_fTextureWidth,0,0);
		C = vTrans + D3DXVECTOR3(m_fTextureWidth,0,-m_fTextureWidth);
		D = vTrans + D3DXVECTOR3(0              ,0,-m_fTextureWidth);

		g_cGraphicsTool.DrawLine(&A,&B,0xFFFF0000,0xFFFF0000);
		g_cGraphicsTool.DrawLine(&B,&C,0xFFFF0000,0xFFFF0000);
		g_cGraphicsTool.DrawLine(&C,&D,0xFFFF0000,0xFFFF0000);
		g_cGraphicsTool.DrawLine(&D,&A,0xFFFF0000,0xFFFF0000);

		g_cGraphicsTool.DrawRect(&A,&B,&D,0xFFFFFFFF,m_lpTextureNormal);
	}
	for (size_t i=0;i<m_vecBillboardPlanes.size();i++)
	{
		BillboardPlane& bill = m_vecBillboardPlanes[i];

		D3DXVECTOR3 A,B,C,D;

		A = bill.vCenter + m_vTransWhenRenderBillboards + bill.vAxisX * bill.vBBox.A.x + bill.vAxisY * bill.vBBox.A.y;
		B = bill.vCenter + m_vTransWhenRenderBillboards + bill.vAxisX * bill.vBBox.B.x + bill.vAxisY * bill.vBBox.A.y;
		C = bill.vCenter + m_vTransWhenRenderBillboards + bill.vAxisX * bill.vBBox.B.x + bill.vAxisY * bill.vBBox.B.y;
		D = bill.vCenter + m_vTransWhenRenderBillboards + bill.vAxisX * bill.vBBox.A.x + bill.vAxisY * bill.vBBox.B.y;

		float U1,U2,V1,V2;
		U1 = bill.vRectUV.Left / m_fTextureWidth;
		V1 = bill.vRectUV.Bottom / m_fTextureWidth;
		U2 = (bill.vRectUV.Left + bill.vRectUV.Width) / m_fTextureWidth;
		V2 = (bill.vRectUV.Bottom + bill.vRectUV.Height) / m_fTextureWidth;

		//g_pd3dDevice->SetTexture(0,NULL);

		g_cGraphicsTool.DrawRect(&A,&B,&D,0xFFFFFFFF,m_lpTextureNormal,U1,V1,U2,V2);
		

		D3DXVECTOR3 vTrans = D3DXVECTOR3(-m_lpModel->m_BBox.GetLength() * 0.5f, 0.f, -m_lpModel->m_BBox.GetWidth() + 100.f);

		float U = bill.vBBox.B.x - bill.vBBox.A.x;
		float V = bill.vBBox.B.y - bill.vBBox.A.y;

		A = vTrans + D3DXVECTOR3(bill.vRectUV.Left, 0.f, -bill.vRectUV.Bottom);
		B = A + D3DXVECTOR3(U,0,0);
		C = A + D3DXVECTOR3(U,0,-V);
		D = A + D3DXVECTOR3(0,0,-V);

		g_cGraphicsTool.DrawLine(&A,&B,0xFF00FF00,0xFF00FF00);
		g_cGraphicsTool.DrawLine(&B,&C,0xFF00FF00,0xFF00FF00);
		g_cGraphicsTool.DrawLine(&C,&D,0xFF00FF00,0xFF00FF00);
		g_cGraphicsTool.DrawLine(&D,&A,0xFF00FF00,0xFF00FF00);
	}

	return S_OK;
}

bool KG3DBillboardCloudCreater::CalcBillBoardUV()	
{
	if (m_listBillboardToTry.size() <= 0)
		return false;


	BillboardPlane* pBill = m_listBillboardToTry.front();
	m_listBillboardToTry.pop_front();
	m_vecBillboardTried.push_back(pBill);

	FRECT& rect = pBill->vRectUV;

	_Point Pos = _Point(-1, -1);
	float  fMinRange = FLT_MAX;

	for (list<_Point>::iterator i = m_listMovePos.begin(); i != m_listMovePos.end(); ++i)
	{
		const _Point& MovPos = (*i);
		MoveRect(rect, MovPos._x + m_fTexInteval, MovPos._y + m_fTexInteval);  

		if (!CheckPos(m_vecBillboardTried))
			continue;

		float fRange = CalcRange(m_vecBillboardTried);
		if (fRange < fMinRange)
		{
			fMinRange = fRange;
			Pos = MovPos;
		}
	}

	MoveRect(rect, Pos._x + m_fTexInteval, Pos._y + m_fTexInteval);
	_Rect _rect = { rect.Left, rect.Bottom, rect.Left + rect.Width, rect.Bottom + rect.Height};

	m_listMovePos.remove(Pos);
	m_listMovePos.push_back(_Point(_rect.right, _rect.top));
	m_listMovePos.push_back(_Point(_rect.left, _rect.bottom));
	m_listMovePos.push_back(_Point(_rect.right, _rect.bottom));

	m_fTextureWidth = fMinRange;

	return true;
}

HRESULT KG3DBillboardCloudCreater::ProcessBillBoardUV()
{
	std::sort(m_vecBillboardPlanes.begin(), m_vecBillboardPlanes.end(), SortComp);

	m_listBillboardToTry.clear();
	m_vecBillboardTried.clear();
	m_listMovePos.clear();
	m_listMovePos.push_back(_Point(0, 0));

	for (size_t i = 0; i < m_vecBillboardPlanes.size(); i++)
	{
		BillboardPlane& bill = m_vecBillboardPlanes[i];

		bill.vRectUV.Left   = m_fTextureWidth;
		bill.vRectUV.Bottom = m_fTextureWidth;
		bill.vRectUV.Width  = bill.vBBox.GetLength();
		bill.vRectUV.Height = bill.vBBox.GetHeight();

		m_listBillboardToTry.push_back(&m_vecBillboardPlanes[i]);
	}

	m_fTexInteval = 0.f;

	return S_OK;
}
//
//HRESULT KG3DBillboardCloudCreater::TryMovingBillBoardUV()
//{
//	HRESULT hr = S_OK;
//	if ((m_listBillboardToTry.size()>0) && m_bTryMovingBillBoardUV)
//	{
//		BillboardPlane* pBill = *m_listBillboardToTry.begin();
//		hr = TryMovingOneBillBoardUV(pBill);
//		if(SUCCEEDED(hr))
//		{
//			;
//		}
//		else
//		{
//			m_vecBillboardTried.push_back(pBill);
//			m_listBillboardToTry.pop_front();
//		}
//	}
//	else
//	{
//		m_bTryMovingBillBoardUV = FALSE;
//	}
//
//	return S_OK;
//}
//
//HRESULT KG3DBillboardCloudCreater::TryMovingOneBillBoardUV(BillboardPlane* pPlane)
//{
//	float K = m_fTextureWidth * 0.02F;
//
//	HRESULT hMoveX = TryMovingOneBillBoardUVDirection(pPlane,-2*K,0);
//	HRESULT hMoveY = TryMovingOneBillBoardUVDirection(pPlane, 0,-K);
//
//	if(FAILED(hMoveX) && FAILED(hMoveY))
//	{
//		D3DXVECTOR2 A,C;
//		pPlane->vRectUV.ToPosition(&A,&C);
//		if(C.y > m_fTextureWidth)
//			pPlane->vRectUV.Left += m_fTextureWidth;
//		else if (C.x > m_fTextureWidth)
//		{
//			HRESULT hMoveUp = TryMovingOneBillBoardUVDirection(pPlane,0,2*K);
//			if(SUCCEEDED(hMoveUp))
//			{
//				float a = rand()%100 * 0.01F + 0.5F;
//				float b = rand()%100 * 0.01F + 0.5F;
//				pPlane->vRectUV.Bottom += a * m_fTextureWidth ;
//				pPlane->vRectUV.Left +=  b * m_fTextureWidth ;
//			}//这里只是发现有空间可用，就随机移动一下位置在尝试移动
//		}
//		else
//			return E_FAIL;
//	}
//	return S_OK;
//}
//
//HRESULT KG3DBillboardCloudCreater::TryMovingOneBillBoardUVDirection(BillboardPlane* pPlane,float X,float Y)
//{
//	BOOL bIntersected = FALSE;
//
//	FRECT rectsave = pPlane->vRectUV;
//
//	pPlane->vRectUV.Left += X;
//	pPlane->vRectUV.Bottom += Y;
//
//	if (pPlane->vRectUV.Left <=0)
//	{
//		bIntersected = TRUE;
//		pPlane->vRectUV.Left = 0;
//	}
//
//	if (pPlane->vRectUV.Bottom <=0)
//	{
//		bIntersected = TRUE;
//		pPlane->vRectUV.Bottom = 0;
//	}
//
//	if(bIntersected)
//	{
//		pPlane->vRectUV = rectsave;
//		return E_FAIL;
//	}
//
//	for (size_t i=0;i<m_vecBillboardTried.size();i++)
//	{
//		BillboardPlane* psrcbill = m_vecBillboardTried[i];
//
//		if (pPlane->vRectUV.IsIntersect(psrcbill->vRectUV))
//		{
//			bIntersected = TRUE;
//			pPlane->vRectUV = rectsave;
//			return E_FAIL;
//		}
//	}
//
//	return S_OK;
//}

HRESULT KG3DBillboardCloudCreater::ComputeUV()
{
	ProcessBillBoardUV();
	return S_OK;
}

HRESULT KG3DBillboardCloudCreater::CreateTextures()
{
	HRESULT hr = S_OK;
	SAFE_RELEASE(m_lpTextureColor);
	SAFE_RELEASE(m_lpTextureNormal);

	m_lpRenderTargetTool = new KG3DRenderTargetTool;
	KGLOG_PROCESS_ERROR(m_lpRenderTargetTool);

	hr = m_lpRenderTargetTool->Init(m_dwTextureWidth,TRUE,D3DFMT_A8R8G8B8);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->CreateTexture( m_dwTextureWidth, m_dwTextureWidth,1, 0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		&m_lpTextureColor,NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->CreateTexture( m_dwTextureWidth, m_dwTextureWidth,1, 0,
		D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED,
		&m_lpTextureNormal,NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DBillboardCloudCreater::RenderTextureForBillBoard(BillboardPlane* pPlane,BOOL bClear,LPD3DXEFFECT pShader,LPDIRECT3DTEXTURE9 pOutputTexture)
{
	HRESULT hr = S_OK;

	KG3DRenderState& RenderStateMgr = g_GetRenderStateMgr();

	KGLOG_PROCESS_ERROR(m_lpRenderTargetTool);

	m_lpRenderTargetTool->BeginNewRenderTarget(NULL,NULL,NULL);
	//////////////////////////////////////////////////////////////////////////
	{
		D3DVIEWPORT9 Viewport;
		if(bClear)
		{
			Viewport.X = 0;
			Viewport.Y = 0;
			Viewport.Width = m_dwTextureWidth;
			Viewport.Height = m_dwTextureWidth;
			g_pd3dDevice->SetViewport(&Viewport);
			hr = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
				0x00000000, 1.0f, 0 );

		}
		Viewport.X = (DWORD)(pPlane->vRectUV.Left / m_fTextureWidth * m_dwTextureWidth);
		Viewport.Y = (DWORD)(pPlane->vRectUV.Bottom / m_fTextureWidth * m_dwTextureWidth);
		Viewport.Width = (DWORD)(pPlane->vRectUV.Width / m_fTextureWidth * m_dwTextureWidth);
		Viewport.Height = (DWORD)(pPlane->vRectUV.Height / m_fTextureWidth * m_dwTextureWidth);


		Viewport.MinZ =0;Viewport.MaxZ =1;

		g_pd3dDevice->SetViewport(&Viewport);
		//////////////////////////////////////////////////////////////////////////
		D3DXVECTOR3 A,B,C,D,Center,Eye;

		A = pPlane->vCenter + pPlane->vAxisX * pPlane->vBBox.A.x + pPlane->vAxisY * pPlane->vBBox.A.y;
		B = pPlane->vCenter + pPlane->vAxisX * pPlane->vBBox.B.x + pPlane->vAxisY * pPlane->vBBox.A.y;
		C = pPlane->vCenter + pPlane->vAxisX * pPlane->vBBox.B.x + pPlane->vAxisY * pPlane->vBBox.B.y;
		D = pPlane->vCenter + pPlane->vAxisX * pPlane->vBBox.A.x + pPlane->vAxisY * pPlane->vBBox.B.y;
		Center = 0.5F*(A + C);
		Eye = Center - pPlane->vPlaneNormal * m_fTextureWidth;
		KG3DCamera vCamera;

		vCamera.SetPositionLookAtUp(&Eye, &Center,&pPlane->vAxisY);
		vCamera.SetOrthogonal(pPlane->vBBox.GetLength(),pPlane->vBBox.GetHeight(),10,m_fTextureWidth * 2);

		//////////////////////////////////////////////////////////////////////////
		g_pd3dDevice->BeginScene();
		hr = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER|D3DCLEAR_STENCIL, 
			0x00000000, 1.0f, 0 );

		RenderStateMgr.SetRenderState(D3DRS_SPECULARENABLE, FALSE);
		//RenderStateMgr.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
		//RenderStateMgr.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_MODULATE);

		GraphicsStruct::RenderStateCullMode CullMode(D3DCULL_NONE);
		GraphicsStruct::RenderState State(D3DRS_LIGHTING, FALSE);

		GraphicsStruct::TextureStageState tss0a1(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
		GraphicsStruct::TextureStageState tss0aop(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
		GraphicsStruct::TextureStageState tss0AlphaA1(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
		GraphicsStruct::TextureStageState tss0AlphaOP(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);

		if(m_lpModel /*&& m_lpModel->m_lpMaterial*/)
		{
			{
				for (size_t i = 0;i<pPlane->vecReferenceTriangle.size();i++)
				{
					Triangle& vTrian = pPlane->vecReferenceTriangle[i];


					g_pd3dDevice->SetTexture(0,NULL);
					g_pd3dDevice->SetTexture(1,NULL);


					if (m_apTexArray[vTrian.m_dwSubSet])
					{
						m_apTexArray[vTrian.m_dwSubSet]->SetTexture(g_pd3dDevice, 0);
						if (!m_apTexArray[vTrian.m_dwSubSet]->GetTexture())
						{
							m_apTexArray[vTrian.m_dwSubSet]->ForceReload();
							m_apTexArray[vTrian.m_dwSubSet]->UpdateTexture();
						}
					}

					if(pShader)
					{
						D3DXMATRIX mat = vCamera.GetViewMatrix() * vCamera.GetProjectionMatrix();
						pShader->SetMatrix("WorldViewProj",&mat);
						pShader->CommitChanges();
					}

					g_cGraphicsTool.DrawTriangle(vTrian.vVertexPos,vTrian.vVertexNormal,vTrian.vVertexUV1);
				}
			}
		}

		g_pd3dDevice->EndScene();
	}
	//////////////////////////////////////////////////////////////////////////
	m_lpRenderTargetTool->EndNewRenderTarget();

	m_lpRenderTargetTool->ExportRenderData(pOutputTexture,TRUE);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DBillboardCloudCreater::ComputeTexture()
{
	CreateTextures();

	//color
	for (size_t i=0;i<m_vecBillboardPlanes.size();i++)
	{
		RenderTextureForBillBoard(&m_vecBillboardPlanes[i],i==0,NULL,m_lpTextureColor);
	}

	//normal
	LPD3DXEFFECT pShader = g_cGraphicsTool.GetDefaultShader(DEFST_NORMAL_OUTPUT);
	if(pShader)
	{
		UINT uPass = 0;

		pShader->SetTechnique("tec0");

		pShader->Begin(&uPass,0);
		pShader->BeginPass(0);

		for (size_t i=0;i<m_vecBillboardPlanes.size();i++)
		{
			RenderTextureForBillBoard(&m_vecBillboardPlanes[i],i==0,pShader,m_lpTextureNormal);
		}

		pShader->EndPass();
		pShader->End();
	}

	return S_OK;
}

HRESULT KG3DBillboardCloudCreater::SaveBillboardCloud()
{
	if (!m_lpModel)
		return E_FAIL;

	/*
	* make file path
	*/


	TCHAR   szFileName[MAX_PATH];
	TCHAR   szBCFileName[MAX_PATH];
	{
		szFileName[0] = _T('\0');
		LPCTSTR szModeName = NULL;
		/*
		m_lpModel->GetFullName(&szModeName);
		wsprintf(szFileName, TEXT("%s%s"), szModeName, TEXT(".bbc"));*/
		m_lpModel->GetName(&szModeName);
		KG_CUSTOM_HELPERS::StrReplaceExtension(szModeName, _T("bbc"), szFileName);
	}

	wsprintf(szBCFileName,"%s",szFileName);

	FILE* pFile = fopen(szFileName, "wb");

	if (!pFile)
		return E_FAIL;

	/*
	* fill billboard clound vertex datas
	*/

	VFormat::_Faces_Normal_Texture1* pVertexs = new VFormat::_Faces_Normal_Texture1[m_vecBillboardPlanes.size() * 6];

	if (!pVertexs)
    {
        fclose(pFile);
		return E_FAIL;
    }

	size_t dwPlaneCount = m_vecBillboardPlanes.size();

	for (size_t i = 0; i < dwPlaneCount; ++i)
	{
		BillboardPlane& bill = m_vecBillboardPlanes[i];

		D3DXVECTOR3 A;
		D3DXVECTOR3 B;
		D3DXVECTOR3 C;
		D3DXVECTOR3 D;

		A = bill.vCenter + bill.vAxisX * bill.vBBox.A.x + bill.vAxisY * bill.vBBox.A.y;
		B = bill.vCenter + bill.vAxisX * bill.vBBox.B.x + bill.vAxisY * bill.vBBox.A.y;
		C = bill.vCenter + bill.vAxisX * bill.vBBox.B.x + bill.vAxisY * bill.vBBox.B.y;
		D = bill.vCenter + bill.vAxisX * bill.vBBox.A.x + bill.vAxisY * bill.vBBox.B.y;

		float U1;
		float U2;
		float V1;
		float V2;

		U1 = (bill.vRectUV.Left + bill.vRectUV.Width) / m_fTextureWidth;
		V1 = (bill.vRectUV.Bottom + bill.vRectUV.Height) / m_fTextureWidth;
		U2 = (bill.vRectUV.Left / m_fTextureWidth);
		V2 = (bill.vRectUV.Bottom / m_fTextureWidth);


		pVertexs[i * 6 + 0].Normal = bill.vPlaneNormal;
		pVertexs[i * 6 + 1].Normal = bill.vPlaneNormal;
		pVertexs[i * 6 + 2].Normal = bill.vPlaneNormal;

		pVertexs[i * 6 + 3].Normal = bill.vPlaneNormal;
		pVertexs[i * 6 + 4].Normal = bill.vPlaneNormal;
		pVertexs[i * 6 + 5].Normal = bill.vPlaneNormal;

		pVertexs[i * 6 + 0].p = A;
		pVertexs[i * 6 + 1].p = B;
		pVertexs[i * 6 + 2].p = D;

		pVertexs[i * 6 + 3].p = B;
		pVertexs[i * 6 + 4].p = D;
		pVertexs[i * 6 + 5].p = C;

		pVertexs[i * 6 + 0].tu1 = U1;
		pVertexs[i * 6 + 0].tv1 = V1;
		pVertexs[i * 6 + 1].tu1 = U2;
		pVertexs[i * 6 + 1].tv1 = V1;
		pVertexs[i * 6 + 2].tu1 = U1;
		pVertexs[i * 6 + 2].tv1 = V2;

		pVertexs[i * 6 + 3].tu1 = U2;
		pVertexs[i * 6 + 3].tv1 = V1;
		pVertexs[i * 6 + 4].tu1 = U1;
		pVertexs[i * 6 + 4].tv1 = V2;
		pVertexs[i * 6 + 5].tu1 = U2;
		pVertexs[i * 6 + 5].tv1 = V2;
	}


	/*
	* save vertex buffer datas
	*/

	fwrite(&dwPlaneCount, sizeof(dwPlaneCount), 1, pFile);
	fwrite(pVertexs, sizeof(VFormat::_Faces_Normal_Texture1), dwPlaneCount * 6, pFile);

	fclose(pFile);

	{
		TCHAR szTextureColorName[MAX_PATH];
		TCHAR szTextureNormalName[MAX_PATH];
		szTextureColorName[0] = '\0';
		szTextureNormalName[0] = '\0';

		LPCTSTR szModelName = NULL;
		m_lpModel->GetName(&szModelName);

		KG_CUSTOM_HELPERS::StrReplaceExtension(szModelName, _T("C.png"), szTextureColorName);
		D3DXSaveTextureToFile(szTextureColorName, D3DXIFF_PNG, m_lpTextureColor, NULL);
		
		KG_CUSTOM_HELPERS::StrReplaceExtension(szModelName, _T("N.png"), szTextureNormalName);
		D3DXSaveTextureToFile(szTextureNormalName, D3DXIFF_PNG, m_lpTextureNormal, NULL);
	}

	SAFE_DELETE_ARRAY(pVertexs);

	SAFE_RELEASE(m_pBillboardCloud);

	m_pBillboardCloud = KG3DBillboardCloud::Build(szBCFileName,*m_lpModel->GetMaterial(),0);
	if(m_pBillboardCloud)
	{
		m_pBillboardCloud->AddRef();
		m_lpModel->SetBillboardCloud(m_pBillboardCloud);
	}

	return S_OK;
}
/************************************************************************/
/* 
KG3DBillboardCloudCreater2Solid
*/
/************************************************************************/

class KG3DBillboardCloudCreater2Solid : public KG3DBillboardCloudCreater2, public KG3DMessageSingleSenderReceiver
{
public:
	virtual HRESULT BuildForModel(IEKG3DModel* pModel);	//入口

	virtual HRESULT FrameMove();
	virtual HRESULT Render();

	virtual VOID	SetLastBCTransformation(const D3DXMATRIX& mat);//预览的时候才有用

	virtual HRESULT SetThresholdStruct(const KG3DBillboardCloudCreaterThresholdStruct* pIn);
	virtual HRESULT GetThresholdStruct(KG3DBillboardCloudCreaterThresholdStruct* pOut);
	virtual HRESULT UpdateShader();
	virtual ULONG GetOption();
	virtual HRESULT SetOption(DWORD Option);
	virtual HRESULT	StepCommand();


	virtual void __stdcall OnReceiveMessage(const KG3DMessage& Message , IKG3DMessageSenderBase* pSender);

	KG3DBillboardCloudCreater2Solid(KG3DSystemEventListener& Lst);
	~KG3DBillboardCloudCreater2Solid();
#if defined(DEBUG) | defined(_DEBUG)
protected:	//这个是为了单元测试做的，没有必要就不要在派生类里面搅合下面这些函数和变量
#else
private:
#endif
	struct ThresholdStruct : public KG3DBillboardCloudCreaterThresholdStruct
	{
		ThresholdStruct();
		BOOL IsLegal()const
		{
			return m_fMaxNormalBias > -FLT_EPSILON 
				&& m_fMaxNormalDistanceBiasPercent > -FLT_EPSILON
				&& m_fMaxDistanceToPoint > -FLT_EPSILON
				&& m_fMinAreaInfluenceAtNormalCorrection > -FLT_EPSILON 
				&& m_fMinAreaAccepted > -FLT_EPSILON;
		}
	};

	struct Triangle 
	{
		D3DXVECTOR3	m_vNormal; //不是点的Normal是面的Normal
		UINT		m_VertexIndices[3];	//对应3个点的坐标
		FLOAT		m_fArea;
		D3DXVECTOR3	m_vCenter;
	};
	typedef std::list<Triangle> TypeTriangleContainer;

	struct CollapsedFace
	{
		D3DXVECTOR3 m_vNormal;
		D3DXVECTOR3	m_vCenter;
		D3DXVECTOR3	m_vRight;
		D3DXVECTOR3	m_vDown;
		FLOAT		m_fHalfWidth;
		FLOAT		m_fHalfHeight;
		FLOAT		GetArea()const {return m_fHalfWidth * m_fHalfHeight * 4;}
		FLOAT		GetMinEdge()const {return min(m_fHalfWidth, m_fHalfHeight) *2;}

		D3DXVECTOR3	GetUpperLeft()const{return m_vCenter - m_vRight * m_fHalfWidth - m_vDown * m_fHalfHeight;}
		D3DXVECTOR3	GetUpperRight()const{return m_vCenter + m_vRight * m_fHalfWidth - m_vDown * m_fHalfHeight;}
		D3DXVECTOR3	GetLowerLeft()const{return m_vCenter - m_vRight * m_fHalfWidth + m_vDown * m_fHalfHeight;}
		D3DXVECTOR3	GetLowerRight()const{return m_vCenter + m_vRight * m_fHalfWidth + m_vDown * m_fHalfHeight;}
		//std::set<UINT>	m_VertexIndices;
	};
	typedef std::list<CollapsedFace> TypeFaceContainer;

	struct FaceUV 
	{
		BOOL bHasRotatedToLeft90Degree;
		FLOAT x, y, Width, Height;

		VOID Init(){ZeroMemory(this, sizeof(FaceUV));}//不写成构造函数了，不然默认的比较函数啥都要自己写
		D3DXVECTOR2	GetUpperLeft()const{return D3DXVECTOR2(x, y);}
		D3DXVECTOR2	GetUpperRight()const{return D3DXVECTOR2(x + Width, y);}
		D3DXVECTOR2	GetLowerLeft()const{return D3DXVECTOR2(x, y + Height);}
		D3DXVECTOR2	GetLowerRight()const{return D3DXVECTOR2(x + Width, y + Height);}

	};
	typedef std::list<FaceUV> TypeFaceUVContainer;	

	struct VertexBufferDesc 
	{
		CHAR*	m_lpBuffer;
		size_t	m_uSizeOfVertex;
		VertexBufferDesc(CHAR* pBuffer, size_t Stride):m_lpBuffer(pBuffer), m_uSizeOfVertex(Stride)
		{
			_ASSERTE(Stride != 0);
		}
		VOID Clear()
		{
			SAFE_DELETE_ARRAY(m_lpBuffer);
			m_uSizeOfVertex = 0;
		}
	};
	struct IndexBufferDesc 
	{
		LPVOID	m_lpBuffer;
		BOOL	m_b32bit;
		size_t	m_Size;
		IndexBufferDesc(CHAR* pBuffer, BOOL b32bit, size_t Size)
			:m_lpBuffer(pBuffer)
			, m_b32bit(b32bit)
			, m_Size(Size)
		{

		}
		VOID	Clear()
		{
			SAFE_DELETE_ARRAY(m_lpBuffer);
		}
		DWORD	GetNumFaces()
		{
			size_t Size = m_Size / (m_b32bit ? 4 : 2);
			return _kg_to_smaller_size_cast<DWORD>(Size/3);
		}
	};
	//成员////////////////////////////////////////////////////////////////////////
	ThresholdStruct	m_threshould;
	TypeTriangleContainer	m_listTriangle;
	TypeFaceUVContainer m_listFaceUV;
	TypeFaceContainer	m_listFaces;

	KG3DModel*	m_lpModel;
	KG3DBillboardCloud* m_pBillboardCloud;	//预览用

	KG3DTexture*	m_lpTexture;
	KG3DTexture*	m_lpDepthTexture;
	LPDIRECT3DVERTEXBUFFER9	m_lpVertexBuffer;
	VertexBufferDesc	m_VBDesc;
	IndexBufferDesc		m_IBDesc;

	D3DXMATRIX		m_matModelWorld;
	AABBOX			m_ModelBox;

	//<ShowProcess>
	DWORD			m_dwOption;
	TypeFaceContainer	m_listFaceToShow;
	//</ShowProcess>

	std::list<KGCommand*>	m_listCommands;	//Commander模式
	//////////////////////////////////////////////////////////////////////////
	HRESULT CreateResouce(IEKG3DModel* pModel, std::tstring* pErrorStr);
	VOID	Clear();
	HRESULT ConstructBillboard();

	HRESULT RenderToTexture();	

	static HRESULT MeshToTriangle(VertexBufferDesc VBDesc, IndexBufferDesc IBDesc
		, TypeTriangleContainer& TriCt , ThresholdStruct& threshold );

	static VOID CollapseTriangleToFaces(TypeTriangleContainer& TriCt, TypeFaceContainer& FaceCt
		, VertexBufferDesc VBDesc, const ThresholdStruct& threshold);
	static VOID CollapseOnTriangleToFace(TypeTriangleContainer& TriCt, TypeFaceContainer& FaceCt
		, VertexBufferDesc VBDesc, const ThresholdStruct& threshold);

	static HRESULT FillVertexBuffer(TypeFaceContainer& FaceCt, TypeFaceUVContainer& FaceUVCt
		, LPDIRECT3DVERTEXBUFFER9 lpBuffer, const AABBOX& Box);

	static void UpdateFaceCenterAndSize(CollapsedFace &Face, const D3DXVECTOR3& Point);
	static BOOL InitTriangle(Triangle& triangle, VertexBufferDesc& VBDesc, UINT Indices[3]);
	static VOID InitFace(CollapsedFace& Face, const Triangle& Tri, VertexBufferDesc& VertexBuffer);
	static BOOL CanCollapseTriToFace(const CollapsedFace& Face, const Triangle& SecondTri, VertexBufferDesc& VertexBuffer, const ThresholdStruct& threshold);

	static const D3DXVECTOR3& GetVertexFromBuffer(UINT Index, VertexBufferDesc& VertexBuffer);
	static UINT	GetIndexFromBuffer(size_t iIndex, IndexBufferDesc& IndexBuffer);
	static VOID UpdateFace(CollapsedFace& Face, const Triangle& Tri
		, VertexBufferDesc& VertexBuffer
		, const ThresholdStruct& threshold);
	static VOID	CalculateUV(TypeFaceContainer& FaceCt, TypeFaceUVContainer& FaceUVCt);	
	static VOID PutFaceIntoList(TypeFaceContainer& FaceCt, const CollapsedFace& Face);

	static VOID IsFaceLegal(const CollapsedFace& Face);
	static void CalculateRight( CollapsedFace &Face );
	BOOL	IsCollapsingProcessShow(){return KG3D_BILLBOARD_CLOUD_CT_OP_SHOW_PROCESS&m_dwOption;}
	static BOOL IsFaceLookingOut(const CollapsedFace& Face, const D3DXVECTOR3& ModelCenter);
	//<UV>
	enum
	{
		max_uv_disperse = 5,
	};

	static FLOAT CalTotalArea(TypeFaceContainer& FaceCt);
	FLOAT ReCalTotalArea();
	static FLOAT EdgeSizeDisperse(FLOAT edgePercent, FLOAT totalPercent);
	static FLOAT CalTotalPercent(size_t MaxUVDisperse);

	struct TextureUVSpace 
	{
		std::list<D3DXVECTOR4> m_Space;

		TextureUVSpace()
		{
			m_Space.push_back(D3DXVECTOR4(0,0,1,1));
		}
	};

	static BOOL FillTextureUVSpace(TextureUVSpace& Space, FaceUV& UV);
	//</UV>

};

KG3DBillboardCloudCreater2Solid::KG3DBillboardCloudCreater2Solid(KG3DSystemEventListener& Lst)
:m_pBillboardCloud(NULL)
,m_lpTexture(NULL)
,m_lpDepthTexture(NULL)
,m_lpVertexBuffer(NULL)
,m_dwOption(0)
,m_lpModel(NULL)
,m_VBDesc((CHAR*)NULL, 1)
,m_IBDesc((CHAR*)NULL, FALSE, 1)
{
	BOOL bRet = Lst.RegisterReceiver(this);
	VERIFY(bRet);
}

KG3DBillboardCloudCreater2Solid::~KG3DBillboardCloudCreater2Solid()
{
	Clear();
}

VOID KG3DBillboardCloudCreater2Solid::Clear()
{
	SAFE_RELEASE(m_lpModel);
	m_listTriangle.clear();
	m_listFaces.clear();
	m_listFaceUV.clear();
	m_listFaceToShow.clear();
	SAFE_RELEASE(m_pBillboardCloud);
	SAFE_RELEASE(m_lpTexture);
	SAFE_RELEASE(m_lpDepthTexture);
	SAFE_RELEASE(m_lpVertexBuffer);
	m_VBDesc.Clear();
	m_IBDesc.Clear();
	m_listCommands.clear();
}

HRESULT KG3DBillboardCloudCreater2Solid::CreateResouce( IEKG3DModel* pIEModel, std::tstring* pErrorStr)
{	
	//分解出Mesh
	LPD3DXMESH lpD3DXMesh = NULL;
	KG3DModel* pModel = dynamic_cast<KG3DModel*>(pIEModel);
	KG_PROCESS_ERROR(NULL != pModel);

	switch (pModel->GetType())
	{
	case MESHTYPE_SPEEDTREE :
		{
			goto Exit0;
		}
		break;
	default :
		{
			KG3DMesh* pMesh = pModel->GetMesh();
			KG_PROCESS_ERROR(NULL != pMesh);

			lpD3DXMesh = pMesh->m_ppMeshes[KG3DMesh::SMBT_NORMAL];
		}
		break;
	}

	KG_PROCESS_ERROR(NULL != lpD3DXMesh);

	{
		//不能处理的Mesh
		DWORD dwFVF = lpD3DXMesh->GetFVF();

		if (dwFVF != (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1) && dwFVF != (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX2))
		{
			if(NULL != pErrorStr)
				*pErrorStr += _T("Mesh顶点格式不匹配");
			goto Exit0;
		}

		Clear();


		//复制一份VB
		HRESULT hr = E_FAIL;
		{
			DWORD dwNumVertices = lpD3DXMesh->GetNumVertices();
			DWORD dwNumBytePerVertices = lpD3DXMesh->GetNumBytesPerVertex();
			size_t VertexBufferSize = static_cast<size_t>(dwNumBytePerVertices*dwNumVertices);

			LPVOID pVerticesData = NULL;

			_ASSERTE(NULL == m_VBDesc.m_lpBuffer);
			m_VBDesc.m_lpBuffer = new CHAR[VertexBufferSize];
			KG_PROCESS_ERROR(NULL != m_VBDesc.m_lpBuffer);
			m_VBDesc.m_uSizeOfVertex = dwNumBytePerVertices;
			_ASSERTE(m_VBDesc.m_uSizeOfVertex != 0);
			_ASSERTE(m_VBDesc.m_uSizeOfVertex == sizeof(FLOAT)*(3+3+2)
				|| m_VBDesc.m_uSizeOfVertex == sizeof(FLOAT)*(3+3+2+2));

			hr = lpD3DXMesh->LockVertexBuffer(0, &pVerticesData);
			KG_COM_PROCESS_ERROR(hr);
			errno_t err = memcpy_s(m_VBDesc.m_lpBuffer, VertexBufferSize, pVerticesData, VertexBufferSize);
			(void)err;
			lpD3DXMesh->UnlockVertexBuffer();
			_ASSERTE(err == 0);
		}

		//复制一份IB
		{
			BOOL bUse32BitVB = lpD3DXMesh->GetOptions()&D3DXMESH_32BIT;

			_ASSERTE(NULL == m_IBDesc.m_lpBuffer);
			size_t SizePerIndex = bUse32BitVB ? 4 : 2;
			size_t IBSize = lpD3DXMesh->GetNumFaces() * 3 * SizePerIndex;

			LPVOID pIndexData = NULL;

			m_IBDesc.m_lpBuffer = new CHAR[IBSize];
			m_IBDesc.m_b32bit = bUse32BitVB;
			m_IBDesc.m_Size = IBSize;

			hr = lpD3DXMesh->LockIndexBuffer(0, &pIndexData);
			KG_COM_PROCESS_ERROR(hr);
			errno_t err = memcpy_s(m_IBDesc.m_lpBuffer, IBSize, pIndexData, IBSize);
			lpD3DXMesh->UnlockIndexBuffer();
			_ASSERTE(err == 0);
			(void)err;
		}



		//创建VB和Texture
		UINT VertexBufferSize = (UINT)(lpD3DXMesh->GetNumFaces() * 6 * KG3DBillboardCloud::VERTEX_SIZE); 
		hr = g_pd3dDevice->CreateVertexBuffer(VertexBufferSize, 0, KG3DBillboardCloud::CUR_FVF
			, D3DPOOL_MANAGED, &m_lpVertexBuffer, NULL);
		KG_COM_PROCESS_ERROR(hr);

		if(NULL == m_lpTexture)
		{
			hr = g_cTextureTable.CreateTexture( KG3DBillboardCloud::TEXTURE_WIDTH, KG3DBillboardCloud::TEXTURE_HEIGHT
				, (D3DFORMAT)KG3DBillboardCloud::TEXTURE_FORMAT
				,TEXTYPE_RENDER_TARGET, &m_lpTexture);
			KGLOG_PROCESS_ERROR(SUCCEEDED(hr) && _T("RenderTarget创建失败"));
		}

		if(NULL == m_lpDepthTexture)
		{
			hr  = g_cTextureTable.CreateTexture(KG3DBillboardCloud::TEXTURE_WIDTH, KG3DBillboardCloud::TEXTURE_HEIGHT
				, D3DFMT_D24S8
				, TEXTYPE_DEPTH
				, &m_lpDepthTexture);
			KGLOG_PROCESS_ERROR(SUCCEEDED(hr) && _T("DepthStencil创建失败"));
		}

		//记录Model
		_ASSERTE(NULL == m_lpModel);
		m_lpModel = pModel;
		m_lpModel->AddRef();

		D3DXVECTOR3 vVecter(0,0,0);
		pModel->GetTranslation(&vVecter);
		D3DXMatrixTranslation(&m_matModelWorld, vVecter.x, vVecter.y, vVecter.z);
		m_ModelBox = pModel->GetBBox();
	}

	return S_OK;
Exit0:
	Clear();
	return E_FAIL;

}

BOOL KG3DBillboardCloudCreater2Solid::InitTriangle( Triangle& triangle, VertexBufferDesc& VBDesc, UINT Indices[3])//注意，其实这里是传数组指针，不是复制数组
{
	memcpy_s(triangle.m_VertexIndices, sizeof(UINT)*3, Indices, sizeof(UINT)*3);

	const D3DXVECTOR3* Vertices[3] = {
		&GetVertexFromBuffer(Indices[0], VBDesc),
		&GetVertexFromBuffer(Indices[1], VBDesc),
		&GetVertexFromBuffer(Indices[2], VBDesc),
	};

	D3DXPLANE	TempPlane;
	D3DXPLANE* p = D3DXPlaneFromPoints(&TempPlane, Vertices[0], Vertices[1], Vertices[2]);
	if(NULL == p)
		return FALSE;

	D3DXPlaneNormalize(&TempPlane, &TempPlane);

	triangle.m_vNormal = *reinterpret_cast<D3DXVECTOR3*>(&TempPlane);
	triangle.m_fArea = TriangleArea(*Vertices[0],*Vertices[1],*Vertices[2]);

	FLOAT xMin = Vertices[0]->x, yMin = Vertices[0]->y, zMin = Vertices[0]->z;
	FLOAT xMax = Vertices[0]->x, yMax = Vertices[0]->y, zMax = Vertices[0]->z;

	for (size_t i = 0; i < 3; ++i)
	{
		FLOAT x = Vertices[i]->x;
		FLOAT y = Vertices[i]->y;
		FLOAT z = Vertices[i]->z;
		if(x < xMin)
			xMin = x;
		if(x > xMax)
			xMax = x;
		if(y < yMin)
			yMin = y;
		if(y > yMax)
			yMax = y;
		if(z < zMin)
			zMin = z;
		if(z > zMax)
			zMax = z;
	}
	D3DXVECTOR3 vCenter = D3DXVECTOR3((xMax+xMin)/2, (yMax+yMin)/2, (zMax+zMin)/2);
	//投射到面上去
	triangle.m_vCenter = vCenter - (D3DXVec3Dot(&vCenter, &triangle.m_vNormal) + TempPlane.d)* triangle.m_vNormal;	

#if defined(DEBUG) | defined(_DEBUG)//Center应该在面上
	FLOAT PlaneRet = D3DXVec3Dot(&triangle.m_vCenter, (D3DXVECTOR3*)&TempPlane) + TempPlane.d;
	_ASSERTE(MT_FLOAT_EQUAL_BIAS(PlaneRet, 0, 0.01f));
#endif

	return TRUE;
}


//最大的插到前面
VOID	KG3DBillboardCloudCreater2Solid::PutFaceIntoList(TypeFaceContainer& FaceCt, const CollapsedFace& Face)
{
	typedef std::list<CollapsedFace> TypeList;
	TypeList& list = FaceCt;
	FLOAT fMaxHalfEdge = max(Face.m_fHalfHeight, Face.m_fHalfWidth);
	_ASSERTE(fMaxHalfEdge > 0);
	for (TypeList::iterator it = list.begin(); it != list.end(); ++it)
	{
		CollapsedFace& FaceCur = *it;
		FLOAT fMaxHalfEdgeCur = max(FaceCur.m_fHalfHeight, FaceCur.m_fHalfWidth);
		if(fMaxHalfEdge > fMaxHalfEdgeCur)
		{
			if(it == list.begin())
				list.push_front(Face);
			else
			{
				list.insert(it, Face);
			}
			return;
		}
	}
#if defined(DEBUG) | defined(_DEBUG)
	if(! list.empty())
	{
		CollapsedFace& FaceBack = list.back();
		_ASSERTE(fMaxHalfEdge <= max(FaceBack.m_fHalfHeight, FaceBack.m_fHalfWidth) + FLT_EPSILON);
	}
#endif
	list.push_back(Face);
	return;
}

BOOL KG3DBillboardCloudCreater2Solid::CanCollapseTriToFace(const CollapsedFace& Face
													  , const Triangle& SecondTri
													  , VertexBufferDesc& VertexBuffer
													  , const ThresholdStruct& threshold)
{
	if(SecondTri.m_fArea < FLT_EPSILON)
		return FALSE;

	if (Face.GetArea() < SecondTri.m_fArea)
		return FALSE;

	//如果Normal偏离太多，拒绝
	FLOAT fAngleGap = D3DXVec3Dot(&Face.m_vNormal, &SecondTri.m_vNormal);
	fAngleGap = abs(fAngleGap);//反向也应该能够合并
	if(fAngleGap < threshold.m_fMaxNormalBias)
		return FALSE;

	D3DXVECTOR3 FacePoints[4] = {Face.GetUpperLeft(), Face.GetUpperRight()
		, Face.GetLowerLeft(), Face.GetLowerRight()};
	
	FLOAT fPointGapGate = Face.GetMinEdge()* threshold.m_fMaxDistanceToPoint;
	for (size_t i = 0; i < 3; ++i)
	{
		//把三角型点投射到Normal上，和面距离是否在阈值之内
		const D3DXVECTOR3& Point = GetVertexFromBuffer(SecondTri.m_VertexIndices[i], VertexBuffer);
		D3DXVECTOR3 vGap = Point - Face.m_vCenter;
		FLOAT PointDistance = D3DXVec3Dot(&vGap, &Face.m_vNormal);//这就是点到面距离，带方向
		BOOL bGapAtNormalDirectionCheck = (abs(PointDistance)/Face.GetMinEdge()) < threshold.m_fMaxNormalDistanceBiasPercent;

		//看看和点的距离是否在阈值之内
		BOOL bGapPointCheck = FALSE;
		if(D3DXVec3Length(&ProjectVectorToDirection(vGap, Face.m_vRight)) < Face.m_fHalfWidth 
			&& D3DXVec3Length(&ProjectVectorToDirection(vGap, Face.m_vDown)) < Face.m_fHalfHeight)
			bGapPointCheck = TRUE;
		else
		{
			for (size_t i = 0; i < _countof(FacePoints); ++i)
			{
				if (D3DXVec3Length(&(FacePoints[i] - Point)) < fPointGapGate)
				{
					bGapPointCheck = TRUE;
					break;
				}
			}
		}

		

		if(bGapAtNormalDirectionCheck && bGapPointCheck)
		{
			return TRUE;
		}
	}
	return FALSE;
}

VOID KG3DBillboardCloudCreater2Solid::UpdateFace( CollapsedFace& Face
											, const Triangle& Tri
											, VertexBufferDesc& VertexBuffer
											, const ThresholdStruct& threshold )
{
	FLOAT fFaceArea = Face.GetArea();
	_ASSERTE(! (fFaceArea < FLT_EPSILON || Tri.m_fArea < FLT_EPSILON));
	if (Tri.m_fArea / fFaceArea > threshold.m_fMinAreaInfluenceAtNormalCorrection)
	{
		//按照面积权重重新分配Normal
		D3DXVECTOR3 vNewNormal = Face.m_vNormal * fFaceArea + Tri.m_vNormal * Tri.m_fArea;//用面积作为权值来影响Normal

		D3DXVec3Normalize(&vNewNormal, &vNewNormal);
		Face.m_vNormal = vNewNormal;

		//重新计算两个轴向，重新投射到平面上，并单位化
		CalculateRight(Face);

		D3DXVec3Cross(&Face.m_vDown, &Face.m_vNormal, &Face.m_vRight);
	}

	//投射三角形的点，看看需不需要增大Face的面积
	for(size_t i = 0; i < 3; i++)
	{
		const D3DXVECTOR3& Point = GetVertexFromBuffer(Tri.m_VertexIndices[i], VertexBuffer);	

		UpdateFaceCenterAndSize(Face, Point);//这里Face的Size变了，但下面的还是用旧的Area

	}

	//沿着Normal移动Face，让它更接近合并的三角
	FLOAT fDistanceProj = D3DXVec3Length(&ProjectVectorToDirection(Tri.m_vCenter - Face.m_vCenter, Face.m_vNormal));
	Face.m_vCenter += Face.m_vNormal * (fDistanceProj * (Tri.m_fArea / fFaceArea));

#if defined(DEBUG) | defined(_DEBUG)
	IsFaceLegal(Face);
#endif
}

VOID KG3DBillboardCloudCreater2Solid::InitFace( CollapsedFace& Face, const Triangle& Tri, VertexBufferDesc& VertexBuffer )
{
	Face.m_vNormal = Tri.m_vNormal;

	//得到三角形中心作为Face原点
	Face.m_vCenter =  Tri.m_vCenter;

	{
		CalculateRight(Face);
		D3DXVec3Cross(&Face.m_vDown, &Face.m_vNormal, &Face.m_vRight);
	}

	//投射到两个轴上，撑大Size，
	Face.m_fHalfWidth = 0;
	Face.m_fHalfHeight = 0;

	for (size_t i = 0; i < 3; ++i)
	{
		const D3DXVECTOR3& Point = GetVertexFromBuffer(Tri.m_VertexIndices[i], VertexBuffer);
		UpdateFaceCenterAndSize(Face, Point);
	}
#if defined(DEBUG) | defined(_DEBUG)
	IsFaceLegal(Face);
#endif
}


void KG3DBillboardCloudCreater2Solid::CalculateRight( CollapsedFace &Face )
{
	/*
	求出面方程 ax + by + cz = d; 其中abc是面Normal的三个分量。
	随便设一个x，求出z，再单位化就可以得到Right了，为了让Right规则，设定Right的y等于0，即尽量让Right
	是一个xz平面上的向量
	*/
	if(abs(Face.m_vNormal.z) < FLT_EPSILON)
	{
		Face.m_vRight = D3DXVECTOR3(0, 0, Face.m_vNormal.x > 0 ? 1.f : -1.f);//为了让Down向量向下，要考虑叉乘结果
	}
	else
	{
		//因为Right设定为y等于0，所以在xz平面上，d为任何值都可以，为了方便，设为0(如果d不是0，到时候要重新投射到面上)
		//FLOAT fD = D3DXVec3Dot(&Face.m_vNormal, &Face.m_vCenter);
		D3DXVECTOR3 vPointOnTheRightDir(Face.m_vNormal.z > 0 ? -1.f : 1.f, 0, 0);		

		//从ax + by + cz = d知道z = (d- ax - by)/c， 以为y是0，所以z = (d - ax)/c
		vPointOnTheRightDir.z = (0 - Face.m_vNormal.x * vPointOnTheRightDir.x) / Face.m_vNormal.z;

		D3DXVec3Normalize(&Face.m_vRight, &vPointOnTheRightDir);
	}
}

void KG3DBillboardCloudCreater2Solid::UpdateFaceCenterAndSize(CollapsedFace &Face
														 , const D3DXVECTOR3& Point)
{
	D3DXVECTOR3 vCenter = Face.m_vCenter;//下面会改Center，所以这里要先保存一下
	D3DXVECTOR3 vPointToCenter = Point - vCenter;

	{
		D3DXVECTOR3 ProjectedRight = ProjectVectorToDirection(vPointToCenter, Face.m_vRight);
		_ASSERTE(Face.m_fHalfWidth > -FLT_EPSILON);

		FLOAT ProjectedRightLength = D3DXVec3Dot(&ProjectedRight, &Face.m_vRight);//有向
		if(abs(ProjectedRightLength) > Face.m_fHalfWidth)
		{
			FLOAT WidthMovement = (abs(ProjectedRightLength) - Face.m_fHalfWidth)/2;
			if(ProjectedRightLength > 0)
				Face.m_vCenter += Face.m_vRight * WidthMovement;
			else
				Face.m_vCenter -= Face.m_vRight * WidthMovement;
			Face.m_fHalfWidth += abs(WidthMovement);	
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		D3DXVECTOR3 ProjectedDown = ProjectVectorToDirection(vPointToCenter, Face.m_vDown);
		_ASSERTE(Face.m_fHalfHeight > -FLT_EPSILON);

		FLOAT ProjectedDownLength = D3DXVec3Dot(&ProjectedDown, &Face.m_vDown);
		if(abs(ProjectedDownLength) > Face.m_fHalfHeight)
		{
			FLOAT HeightMovement = (abs(ProjectedDownLength) - Face.m_fHalfHeight)/2;
			if(ProjectedDownLength > 0)
				Face.m_vCenter += Face.m_vDown * HeightMovement;
			else
				Face.m_vCenter -= Face.m_vDown * HeightMovement;
			Face.m_fHalfHeight += abs(HeightMovement);
		}
	}
}

const D3DXVECTOR3& KG3DBillboardCloudCreater2Solid::GetVertexFromBuffer( UINT Index, VertexBufferDesc& VertexBuffer )
{
	return *reinterpret_cast<D3DXVECTOR3*>(VertexBuffer.m_lpBuffer + Index * VertexBuffer.m_uSizeOfVertex);
}

UINT KG3DBillboardCloudCreater2Solid::GetIndexFromBuffer( size_t iIndex, IndexBufferDesc& IndexBuffer )
{
	//为了16位和32位的IB兼容，写成这样子
	return IndexBuffer.m_b32bit 
		? ((DWORD*)IndexBuffer.m_lpBuffer)[iIndex] 
	: ((WORD*)IndexBuffer.m_lpBuffer)[iIndex];
}

HRESULT KG3DBillboardCloudCreater2Solid::BuildForModel( IEKG3DModel* pIEModel )
{
	HRESULT hr = CreateResouce(pIEModel, NULL);
	KG_COM_PROCESS_ERROR(hr);
	{
		struct TypeBCCommand : public KGCommand 
		{
			KG3DBillboardCloudCreater2Solid* m_pBC;
			virtual HRESULT Execute(){return Execute(*m_pBC);}
			virtual HRESULT Execute(KG3DBillboardCloudCreater2Solid& BC) = 0;
		};

		_ASSERTE(m_listCommands.empty());
		
		typedef KG3DBillboardCloudCreater2Solid TBC;

		//1.在Mesh中分解出三角
		struct Command1 : public TypeBCCommand 
		{
			virtual HRESULT Execute(KG3DBillboardCloudCreater2Solid& BC)
			{
				_ASSERTE(NULL != BC.m_VBDesc.m_lpBuffer && NULL != BC.m_IBDesc.m_lpBuffer);
				return TBC::MeshToTriangle(BC.m_VBDesc, BC.m_IBDesc
					, BC.m_listTriangle, BC.m_threshould);
			}
		};
		static Command1 c1;
		c1.m_pBC = this;
		m_listCommands.push_back(&c1);

		
		//2.塌陷能够合并的三角成为四方面
		struct Command2 : public TypeBCCommand 
		{
			virtual HRESULT Execute(KG3DBillboardCloudCreater2Solid& BC)
			{
				TBC::CollapseOnTriangleToFace(BC.m_listTriangle, BC.m_listFaces, BC.m_VBDesc, BC.m_threshould);
				if(! BC.m_listTriangle.empty())
				{
					_ASSERTE(BC.m_listCommands.front() != this);
					BC.m_listCommands.push_front(this);
				}
				return S_OK;
			}
		};
		static Command2 c2;
		c2.m_pBC = this;
		m_listCommands.push_back(&c2);

		//3.用贪婪算法把UV打包到贴图空间中，可能会再删除一些面
		struct Command3 : public TypeBCCommand 
		{
			virtual HRESULT Execute(KG3DBillboardCloudCreater2Solid& BC)
			{
				TBC::CalculateUV(BC.m_listFaces, BC.m_listFaceUV);
				return S_OK;
			}
		};
		static Command3 c3;
		c3.m_pBC = this;
		m_listCommands.push_back(&c3);
		
		
		//4.填顶点缓冲，渲染贴图，创建Billboard
		struct Command4 : public TypeBCCommand
		{
			virtual HRESULT Execute(KG3DBillboardCloudCreater2Solid& BC)
			{
				HRESULT hr = TBC::FillVertexBuffer(BC.m_listFaces, BC.m_listFaceUV
								, BC.m_lpVertexBuffer, BC.m_ModelBox);
				KG_COM_PROCESS_ERROR(hr);

				_ASSERTE(NULL != BC.m_lpModel);
				hr = BC.RenderToTexture();
				KG_COM_PROCESS_ERROR(hr);

				hr = BC.ConstructBillboard();
				KG_COM_PROCESS_ERROR(hr);

				return S_OK;
Exit0:
				return E_FAIL;
			}
		};
		static Command4 c4;
		c4.m_pBC = this;
		m_listCommands.push_back(&c4);

		if (! this->IsCollapsingProcessShow())
		{
			while (! m_listCommands.empty())
			{
				KGCommand* p = m_listCommands.front();
				m_listCommands.pop_front();
				HRESULT hr = p->Execute();
				if (FAILED(hr))
				{
					m_listCommands.clear();
					Clear();
				}
			}
		}
	}

	return S_OK;
Exit0:	
	Clear();
	return E_FAIL;
}

HRESULT KG3DBillboardCloudCreater2Solid::RenderToTexture()
{
	//Texture.ColorFill(NULL, 0xffff0000);
	KG3DCamera Cam;

	_ASSERTE(NULL != m_lpModel);
	FLOAT fDiameterOfModel = m_lpModel->GetBBox().GetDiameter();

	GraphicsStruct::RenderTargetAndDepthStencilSave rtSace(0);

	_ASSERTE(NULL != m_lpModel);
	KG3DModel& Model = *m_lpModel;

	KG_PROCESS_ERROR(NULL != m_lpTexture && NULL != m_lpDepthTexture);
	{
		m_lpTexture->ColorFill(NULL, 0x00000000);

		m_lpTexture->SetAsRenderTarget(g_pd3dDevice, 0);
		m_lpDepthTexture->SetAsDelphStencil(g_pd3dDevice);

		g_pd3dDevice->BeginScene();

		_ASSERTE(m_listFaceUV.size() == m_listFaces.size());

		D3DXVECTOR3 vModelCenter = m_ModelBox.GetCenter();

		TypeFaceUVContainer::iterator itUV = m_listFaceUV.begin();
		for (TypeFaceContainer::iterator it = m_listFaces.begin(); it != m_listFaces.end(); ++it, ++itUV)
		{
			CollapsedFace& Face = *it;
			FaceUV& FaceUV = *itUV;

			enum
			{
				em_cam_bias = 10,	//主要向后退一下摄像机是因为View矩阵在ZN值很小的时候，ZBuffer性能很差
			};

			BOOL bIsFaceLookingOut = IsFaceLookingOut(Face, vModelCenter);
			//D3DXVECTOR3 vCamPos = bIsFaceLookingOut ? 
			//	Face.m_vCenter + Face.m_vNormal * em_cam_bias * 1.1f//fDiameterOfModel*2;
			//	: Face.m_vCenter - Face.m_vNormal *em_cam_bias * 1.1f;

			D3DXVECTOR3 vCamPos = bIsFaceLookingOut ? Face.m_vCenter + Face.m_vNormal * fDiameterOfModel*2
				: Face.m_vCenter - Face.m_vNormal * fDiameterOfModel * 2;					

			D3DXVECTOR3 vCamUP = FaceUV.bHasRotatedToLeft90Degree ? -Face.m_vRight : -Face.m_vDown;

			Cam.SetPositionLookAtUp(&vCamPos, &Face.m_vCenter, &vCamUP);

			FLOAT fZFar = D3DXVec3Length(&(Face.m_vNormal * fDiameterOfModel * 3));

			if(! FaceUV.bHasRotatedToLeft90Degree)
				Cam.SetOrthogonal(Face.m_fHalfWidth*2, Face.m_fHalfHeight*2, em_cam_bias, fZFar);
			else
				Cam.SetOrthogonal(Face.m_fHalfHeight*2, Face.m_fHalfWidth*2, em_cam_bias, fZFar);
			Cam.SetCamera();

			D3DVIEWPORT9 ViewPort;
			//FaceUV.bHasRotatedToLeft90Degree
			_ASSERTE(FaceUV.Width >= FaceUV.Height);
			ViewPort.Width = (DWORD)(FaceUV.Width * KG3DBillboardCloud::TEXTURE_WIDTH);
			ViewPort.Height = (DWORD)(FaceUV.Height * KG3DBillboardCloud::TEXTURE_HEIGHT);
			ViewPort.X = (DWORD)(FaceUV.x * KG3DBillboardCloud::TEXTURE_WIDTH);
			ViewPort.Y = (DWORD)(FaceUV.y * KG3DBillboardCloud::TEXTURE_HEIGHT);
			ViewPort.MinZ = 0;
			ViewPort.MaxZ = 1;
			g_pd3dDevice->SetViewport(&ViewPort);

			Model.Render(0, NULL);

			//<Line>
			D3DXVECTOR2 v1((FLOAT)ViewPort.X,(FLOAT)ViewPort.Y);
			D3DXVECTOR2 v2((FLOAT)(ViewPort.X + ViewPort.Width),(FLOAT)ViewPort.Y);
			D3DXVECTOR2 v3((FLOAT)ViewPort.X, (FLOAT)(ViewPort.Y + ViewPort.Height));
			D3DXVECTOR2 v4((FLOAT)(ViewPort.X + ViewPort.Width),(FLOAT)(ViewPort.Y + ViewPort.Height));

			g_cGraphicsTool.DrawScreenLine(&v1, &v2, 0, 0xff00ff00, 0xff00ff00);
			g_cGraphicsTool.DrawScreenLine(&v1, &v3, 0, 0xff00ff00, 0xff00ff00);
			g_cGraphicsTool.DrawScreenLine(&v2, &v4, 0, 0xff00ff00, 0xff00ff00);
			g_cGraphicsTool.DrawScreenLine(&v3, &v4, 0, 0xff00ff00, 0xff00ff00);
			//</Line>
		}
		g_pd3dDevice->EndScene();

		return S_OK;
	}
Exit0:
	return E_FAIL;
}

HRESULT KG3DBillboardCloudCreater2Solid::FillVertexBuffer(TypeFaceContainer& FaceCt, TypeFaceUVContainer& FaceUVCt
													 , LPDIRECT3DVERTEXBUFFER9 lpBuffer, const AABBOX& Box)
{
	_ASSERTE(! FaceCt.empty());

	KG3DBillboardCloud::TypeVertexStruct* pBuffer = NULL;
	UINT uSizeToLock = (UINT)(FaceCt.size() * 6 * KG3DBillboardCloud::VERTEX_SIZE);
	HRESULT hr = lpBuffer->Lock(0, uSizeToLock, (VOID**)&pBuffer, 0);
	KG_COM_PROCESS_ERROR(hr);

	{
		D3DXVECTOR3 vModelCenter = Box.GetCenter();

		KG3DBillboardCloud::TypeVertexStruct* pCurBufferPos = pBuffer;

		_ASSERTE(FaceCt.size() == FaceUVCt.size());
		TypeFaceContainer::iterator itFace = FaceCt.begin();
		std::list<FaceUV>::iterator itUV = FaceUVCt.begin();

		for ( ;itFace != FaceCt.end(); ++itFace, ++itUV, pCurBufferPos += 6)
		{
			CollapsedFace& Face = *itFace;
			FaceUV& FaceUV = *itUV;

			//如果面向里面，那么拍贴图的时候，摄像机在法线反向，需要翻转贴图
			BOOL bIsFaceLookingOut = IsFaceLookingOut(Face, vModelCenter);

			if (! FaceUV.bHasRotatedToLeft90Degree)
			{
				KG3DBillboardCloud::TypeVertexStruct& p0 = pCurBufferPos[0];
				p0.p = Face.GetUpperLeft();
				p0.uv = FaceUV.GetUpperLeft();

				KG3DBillboardCloud::TypeVertexStruct& p1 = pCurBufferPos[1];
				p1.p = Face.GetUpperRight();
				p1.uv = FaceUV.GetUpperRight();

				KG3DBillboardCloud::TypeVertexStruct& p2 = pCurBufferPos[2];
				p2.p = Face.GetLowerRight();
				p2.uv = FaceUV.GetLowerRight();

				KG3DBillboardCloud::TypeVertexStruct& p3 = pCurBufferPos[3];
				p3.p = Face.GetLowerLeft();
				p3.uv = FaceUV.GetLowerLeft();
			}
			else	//如果需要转90度，从UV左下开始顺时针
			{
				KG3DBillboardCloud::TypeVertexStruct& p0 = pCurBufferPos[0];
				p0.p = Face.GetUpperLeft();
				p0.uv = FaceUV.GetLowerLeft();

				KG3DBillboardCloud::TypeVertexStruct& p1 = pCurBufferPos[1];
				p1.p = Face.GetUpperRight();
				p1.uv = FaceUV.GetUpperLeft();

				KG3DBillboardCloud::TypeVertexStruct& p2 = pCurBufferPos[2];
				p2.p = Face.GetLowerRight();
				p2.uv = FaceUV.GetUpperRight();

				KG3DBillboardCloud::TypeVertexStruct& p3 = pCurBufferPos[3];
				p3.p = Face.GetLowerLeft();
				p3.uv = FaceUV.GetLowerRight();
			}

			if (! bIsFaceLookingOut)
			{
				std::swap(pCurBufferPos[0].uv, pCurBufferPos[1].uv);
				std::swap(pCurBufferPos[2].uv, pCurBufferPos[3].uv);
			}

			pCurBufferPos[4] = pCurBufferPos[0];
			pCurBufferPos[5] = pCurBufferPos[2];
			//这样面片按顺时针，从左上开始，点分别是0,1,2,3。绘制的时候，按0,1,2;3,0,2就可以形成两个左手优先三角了
		}
	}

	lpBuffer->Unlock();
	return S_OK;
Exit0:
	return E_FAIL;
}

FLOAT KG3DBillboardCloudCreater2Solid::CalTotalPercent(size_t MaxUVDisperse)
{
	//int nTotalDispersePercent = 0;
	//for (size_t i = 0; i < MaxUVDisperse; --i)
	//{
	//	nTotalDispersePercent += (1 << i);
	//}
	//if(nTotalDispersePercent % 2 != 0)
	//	nTotalDispersePercent += 1;	//对于3D程序来说，因为有MipMap，所以不能2分的话总是会有点麻烦

	//return (FLOAT)nTotalDispersePercent;

	//算法相当于上面，就是如果分4个离散等级，那么结果是1+2+4+8，然后再变成2的倍数，但结果可以简单成这样：
	return (FLOAT)(1 << MaxUVDisperse);
}

VOID KG3DBillboardCloudCreater2Solid::CalculateUV(TypeFaceContainer& FaceCt, TypeFaceUVContainer& FaceUVCt)
{
	if(FaceCt.empty())
		return;

	_ASSERTE(FaceUVCt.empty());

	FLOAT fEdgeSizeEstimated = sqrt(CalTotalArea(FaceCt));
	static FLOAT fTotalDispersePercents = CalTotalPercent(max_uv_disperse);
	static FLOAT fMinDispersedPercent = 1 / fTotalDispersePercents;
	//把所有的长宽离散化，重新计算总的面积
	for (std::list<CollapsedFace>::iterator it = FaceCt.begin(); it != FaceCt.end();)
	{
		CollapsedFace& Face = *it;

		FaceUV UVofTheFace;
		UVofTheFace.Init();

		//如果高比宽大，那么旋转90度，让横向大些
		bool bRotate90Degree = Face.m_fHalfHeight > Face.m_fHalfWidth;
		UVofTheFace.bHasRotatedToLeft90Degree = bRotate90Degree;

		//把长宽的比例离散化
		{
			FLOAT WidthPercent = bRotate90Degree ? Face.m_fHalfHeight * 2 : Face.m_fHalfWidth *2;
			WidthPercent /= fEdgeSizeEstimated;
			UVofTheFace.Width = EdgeSizeDisperse(WidthPercent, fTotalDispersePercents);
		}

		{
			FLOAT HeightPercent = bRotate90Degree ? Face.m_fHalfWidth * 2 : Face.m_fHalfHeight *2;
			HeightPercent /= fEdgeSizeEstimated;
			UVofTheFace.Height = EdgeSizeDisperse(HeightPercent, fTotalDispersePercents);
		}

		if (UVofTheFace.Width < fMinDispersedPercent || UVofTheFace.Height < fMinDispersedPercent)
		{
			it = FaceCt.erase(it);
		}
		else
		{
			FaceUVCt.push_back(UVofTheFace);
			++it;
		}
	}

	//fEdgeSizeEstimated = sqrt(ReCalTotalArea());	
	TextureUVSpace	LeftSpace;
	_ASSERTE(FaceCt.size() == FaceUVCt.size());

	for(std::list<FaceUV>::iterator itUV = FaceUVCt.begin(); itUV != FaceUVCt.end();++itUV)
	{
		FaceUV&		CurUV = *itUV;
		//CurUV.Width /= fEdgeSizeEstimated;
		//CurUV.Height /= fEdgeSizeEstimated;
		FillTextureUVSpace(LeftSpace, CurUV);					
	}
}

BOOL KG3DBillboardCloudCreater2Solid::FillTextureUVSpace( TextureUVSpace& Space, FaceUV& UV )
{
	if(Space.m_Space.size() == 0)
		return FALSE;

	if(UV.Width > 1)
		UV.Width = 1;
	if(UV.Height > 1)
		UV.Height = 1;
	_ASSERTE(UV.Width > 0 && UV.Height > 0);
	_ASSERTE(UV.Width + FLT_EPSILON > UV.Height);//保证宽比较大，塞起来好塞，不用判断
	_ASSERTE(Space.m_Space.size() <= max_uv_disperse + 1);

	D3DXVECTOR4& FirstSpace = Space.m_Space.front();

	if(FirstSpace.z > UV.Width + FLT_EPSILON)	//如果大于，那么分裂
	{
		D3DXVECTOR4 vSpaceToPush;
		vSpaceToPush.x = FirstSpace.x + UV.Width;
		vSpaceToPush.y = FirstSpace.y;
		vSpaceToPush.z = FirstSpace.z - UV.Width;
		vSpaceToPush.w = FirstSpace.w;

		FirstSpace.z = UV.Width;

		std::list<D3DXVECTOR4>::iterator itSecond = Space.m_Space.begin();
		itSecond++;
		Space.m_Space.insert(itSecond, vSpaceToPush);			
	}

	UV.Width = FirstSpace.z;	//z较小或者等于的情况，都塞进去

	if (FirstSpace.w > UV.Height + FLT_EPSILON)
	{
		UV.x = FirstSpace.x;
		UV.y = FirstSpace.y;
		FirstSpace.y += UV.Height;
		FirstSpace.w -= UV.Height;
		return TRUE;
	}

	UV.x = FirstSpace.x;
	UV.y = FirstSpace.y;
	UV.Height = FirstSpace.w;

	Space.m_Space.erase(Space.m_Space.begin());
	return TRUE;

}
FLOAT KG3DBillboardCloudCreater2Solid::CalTotalArea(TypeFaceContainer& FaceCt)
{
	FLOAT fRet = 0;
	for (std::list<CollapsedFace>::iterator it = FaceCt.begin(); it != FaceCt.end(); ++it)
	{
		CollapsedFace& Face = *it;
		fRet += Face.GetArea();
	}
	return fRet;
}

FLOAT KG3DBillboardCloudCreater2Solid::EdgeSizeDisperse(FLOAT edgePercent, FLOAT totalPercent )
{
	//FLOAT edgePercent = edgeSize / edgeSizeEstimate;
	//INT nPercent = (INT)(edgePercent * totalPercent);
	//for (size_t i = 0; i < max_uv_disperse+1; i++)	//这里加1的用意是允许全贴图大小的分块
	//{
	//	INT GapToBigger = (1 << (i+1)) - nPercent;
	//	INT GapToSmaller = nPercent - (1 << i);

	//	if (GapToBigger > GapToSmaller && GapToBigger > 0)
	//	{
	//		return ((1 << i)) / totalPercent * edgeSizeEstimate;
	//	}
	//}
	//if(nPercent < 1)
	//	return 1 / totalPercent * edgeSizeEstimate;

	//_ASSERTE(nPercent > (1 << (max_uv_disperse-1)));
	//return ((1 << max_uv_disperse)) / totalPercent * edgeSizeEstimate; 
	_ASSERTE(edgePercent > -FLT_EPSILON);
	INT edgePercentINT = FourCullFiveAdd(edgePercent * totalPercent);//四舍5入到一个整数

	if(edgePercentINT < 1)
		return 0;

	if(edgePercentINT >= (INT)totalPercent)
		return 1;

	_ASSERTE(edgePercentINT != 0);
	int MaxPos = -1;
	INT edgePercentEnum = edgePercentINT;
	while(edgePercentEnum)
	{
		edgePercentEnum >>= 1;
		MaxPos += 1;
	}

	if(MaxPos == 0)
		return 1 / totalPercent;

	if(edgePercentINT & (1 << (MaxPos-1)))
		return (FLOAT)(1 << (MaxPos+1))/totalPercent;
	return (1 << MaxPos)/totalPercent;
}

FLOAT KG3DBillboardCloudCreater2Solid::ReCalTotalArea()
{
	FLOAT fArea = 0;
	for (std::list<FaceUV>::iterator it = m_listFaceUV.begin(); it != m_listFaceUV.end(); ++it)
	{
		FaceUV& uv = *it;
		fArea += uv.Width * uv.Height;
	}
	return fArea;
}

HRESULT KG3DBillboardCloudCreater2Solid::StepCommand()
{
	if(this->IsCollapsingProcessShow() && ! m_listCommands.empty())
	{
		KGCommand* p = m_listCommands.front();
		m_listCommands.pop_front();
		HRESULT hr = p->Execute();
		if (FAILED(hr))
		{
			m_listCommands.clear();
			Clear();
		}
	}
	return S_OK;
}

HRESULT KG3DBillboardCloudCreater2Solid::FrameMove()
{
	
	return S_OK;
}

HRESULT KG3DBillboardCloudCreater2Solid::Render()
{
	if (! this->IsCollapsingProcessShow())
	{
		if (NULL != m_pBillboardCloud)
		{
			HRESULT hr = m_pBillboardCloud->RenderBegin2();
			if (SUCCEEDED(hr))
			{
				m_pBillboardCloud->Render2();

				m_pBillboardCloud->RenderEnd2();
			}

		}
	}
	else	//需要演示塌陷的话，一个个Face显示
	{
		//static DWORD s_dwLastTime = g_cGraphicsTool.GetNowTime();

		//DWORD dwNowTime = g_cGraphicsTool.GetNowTime();
		//if (dwNowTime - s_dwLastTime > 500)
		//{
		//	s_dwLastTime = dwNowTime;
		//	if(! m_listFaces.empty())
		//	{
		//		m_listFaceToShow.push_back(m_listFaces.front());
		//		m_listFaces.pop_front();
		//	}
		//}//if (dwNowTime - s_dwLastTime > 500)	
		//g_GetFrameDrawerRect().EnableFrameFace(TRUE);
		//g_GetFrameDrawerRect().EnableFrameLine(TRUE);
		//g_GetFrameDrawerRect().SetLineColor(0xff00ff00);
		//g_GetFrameDrawerRect().SetFaceColor(0x3300ff00);
		//HRESULT hr = g_GetFrameDrawerRect().Begin();
		//if (SUCCEEDED(hr))
		//{
		//	const D3DXMATRIX* pmat = NULL != m_pBillboardCloud ? &m_pBillboardCloud->GetTransform() : NULL; 
		//	for (TypeFaceContainer::iterator it = m_listFaceToShow.begin(); it != m_listFaceToShow.end(); ++it)
		//	{
		//		CollapsedFace& Face = *it;
		//		D3DXVECTOR3 Buffer[3];
		//		Buffer[0] = Face.m_vCenter - Face.m_vRight * Face.m_fHalfWidth 
		//			- Face.m_vDown * Face.m_fHalfHeight;
		//		Buffer[1] = Face.m_vCenter + Face.m_vRight * Face.m_fHalfWidth 
		//			- Face.m_vDown * Face.m_fHalfHeight;
		//		Buffer[2] = Face.m_vCenter + Face.m_vRight * Face.m_fHalfWidth 
		//			+ Face.m_vDown * Face.m_fHalfHeight;

		//		g_GetFrameDrawerRect().Draw(Buffer, _countof(Buffer), pmat);
		//	}
		//	g_GetFrameDrawerRect().End();
		//}

		StepCommand();
		if (NULL != m_VBDesc.m_lpBuffer)
		{
			D3DXMATRIX TransMat;
			D3DXMatrixTranslation(&TransMat, m_ModelBox.GetDiameter() * 1.1f, 0, 0);
			D3DXMATRIX FrameMat = m_matModelWorld;
			FrameMat *= TransMat;

			const D3DXMATRIX* pmat = NULL != m_pBillboardCloud ? &m_pBillboardCloud->GetTransform() : &FrameMat;
			
			KG3DFrameDrawer& DrawerTri = g_GetFrameDrawerTri();
			DrawerTri.SetParameters(TRUE, TRUE, 0x330000ff, 0xff0000ff);
			if (SUCCEEDED(DrawerTri.Begin(pmat)))
			{
				for (TypeTriangleContainer::iterator it = m_listTriangle.begin(); it != m_listTriangle.end(); ++it)
				{
					Triangle& tri = *it;
					
					const D3DXVECTOR3& UpperLeft = GetVertexFromBuffer(tri.m_VertexIndices[0], m_VBDesc);
					const D3DXVECTOR3& UpperRight = GetVertexFromBuffer(tri.m_VertexIndices[1], m_VBDesc);
					const D3DXVECTOR3& LowerRight = GetVertexFromBuffer(tri.m_VertexIndices[2], m_VBDesc);
					DrawerTri.Draw(UpperLeft, UpperRight, LowerRight);
				}
				DrawerTri.End();
			}
			
			//////////////////////////////////////////////////////////////////////////

			KG3DFrameDrawer& DrawerRect = g_GetFrameDrawerRect();
			DrawerRect.SetParameters(TRUE, TRUE, 0x3300ff00, 0xff00ff00);

			if(SUCCEEDED(DrawerRect.Begin(pmat)))
			{
				for (TypeFaceContainer::iterator it = m_listFaces.begin(); it != m_listFaces.end(); ++it)
				{
					CollapsedFace& Face = *it;
					D3DXVECTOR3 Buffer[3];
					Buffer[0] = Face.m_vCenter - Face.m_vRight * Face.m_fHalfWidth 
						- Face.m_vDown * Face.m_fHalfHeight;
					Buffer[1] = Face.m_vCenter + Face.m_vRight * Face.m_fHalfWidth 
						- Face.m_vDown * Face.m_fHalfHeight;
					Buffer[2] = Face.m_vCenter + Face.m_vRight * Face.m_fHalfWidth 
						+ Face.m_vDown * Face.m_fHalfHeight;

					DrawerRect.Draw(Buffer[0],Buffer[1],Buffer[2]);
				}
				DrawerRect.End();
			}
		}
	}
	//把贴图也渲染出来
	if (NULL != m_lpTexture)
	{
		GraphicsStruct::SaveMatrix matSave(D3DTS_WORLD);
		D3DXMATRIX mat = m_matModelWorld;
		mat._43 -= m_ModelBox.GetDiameter()* 1.10f;	//向前拉一点点

		FLOAT fDiameter = m_ModelBox.GetDiameter();
		D3DXVECTOR3 v1(0,0,0);
		D3DXVECTOR3 v2(fDiameter, 0, 0);
		D3DXVECTOR3 v3(0,0, -fDiameter * (FLOAT)KG3DBillboardCloud::TEXTURE_HEIGHT / (FLOAT)KG3DBillboardCloud::TEXTURE_WIDTH);
		g_cGraphicsTool.DrawRect(&v1,&v2,&v3, 0xffffffff, m_lpTexture->GetTexture());
	}
	return S_OK;
}

VOID KG3DBillboardCloudCreater2Solid::SetLastBCTransformation( const D3DXMATRIX& mat )
{
	if(NULL != m_pBillboardCloud)
		m_pBillboardCloud->SetTransform(mat);
}

VOID KG3DBillboardCloudCreater2Solid::IsFaceLegal( const CollapsedFace& Face )
{
	FLOAT fDown = D3DXVec3Length(&Face.m_vDown);
	_ASSERTE(MT_FLOAT_EQUAL_BIAS(fDown, 1, 0.001f));
	(void)fDown;

	FLOAT fRight = D3DXVec3Length(&Face.m_vRight);
	_ASSERTE(MT_FLOAT_EQUAL_BIAS(fRight, 1, 0.001f));
	KG_USE_ARGUMENT(fRight);

	FLOAT fNormal = D3DXVec3Length(&Face.m_vNormal);
	_ASSERTE(MT_FLOAT_EQUAL_BIAS(fNormal, 1, 0.001f));
	(void)fNormal;

	D3DXVECTOR3 vTemp;
	D3DXVec3Cross(&vTemp, &Face.m_vRight, &Face.m_vDown);

	_ASSERTE(MT_VECTOR_EQUAL(Face.m_vNormal, vTemp, 0.01));

	FLOAT fDot = D3DXVec3Dot(&Face.m_vDown, &Face.m_vNormal);
	_ASSERTE(MT_FLOAT_EQUAL_BIAS(fDot, 0, 0.001f));
	fDot = D3DXVec3Dot(&Face.m_vRight, &Face.m_vNormal);
	_ASSERTE(MT_FLOAT_EQUAL_BIAS(fDot, 0, 0.001f));
}

HRESULT KG3DBillboardCloudCreater2Solid::SetThresholdStruct( const KG3DBillboardCloudCreaterThresholdStruct* pIn )
{
	if(NULL == pIn)
		return E_INVALIDARG;
	_KG3D_IS_SUPPERSUB_POINTER_EQUAL(KG3DBillboardCloudCreaterThresholdStruct, ThresholdStruct);
	const ThresholdStruct* p = static_cast<const ThresholdStruct*>(pIn);
	if(! p->IsLegal())
		return E_FAIL;
	_ASSERTE(sizeof(ThresholdStruct) >= sizeof(KG3DBillboardCloudCreaterThresholdStruct));
	m_threshould = *p;
	return S_OK;
}

HRESULT KG3DBillboardCloudCreater2Solid::GetThresholdStruct( KG3DBillboardCloudCreaterThresholdStruct* pOut )
{
	if(NULL == pOut)
		return E_INVALIDARG;
	*pOut = m_threshould;
	return S_OK;
}

HRESULT KG3DBillboardCloudCreater2Solid::UpdateShader()
{
	return S_OK;
}

ULONG KG3DBillboardCloudCreater2Solid::GetOption()
{
	return m_dwOption;
}

HRESULT KG3DBillboardCloudCreater2Solid::SetOption( DWORD Option )
{
	m_dwOption = Option;
	return S_OK;
}

VOID KG3DBillboardCloudCreater2Solid::CollapseTriangleToFaces(TypeTriangleContainer& TriCt, TypeFaceContainer& FaceCt
															, VertexBufferDesc VBDesc, const ThresholdStruct& threshold )
{
	_ASSERTE(TriCt.size() < 2 || TriCt.front().m_fArea >= TriCt.back().m_fArea);

	while(! TriCt.empty())
	{
#if defined(DEBUG) | defined(_DEBUG)
		size_t CurTriCtSize = TriCt.size();
#endif		
		CollapseOnTriangleToFace(TriCt, FaceCt, VBDesc, threshold);
		_ASSERTE(CurTriCtSize > TriCt.size());
	}
}

VOID KG3DBillboardCloudCreater2Solid::CollapseOnTriangleToFace( TypeTriangleContainer& TriCt, TypeFaceContainer& FaceCt , VertexBufferDesc VBDesc, const ThresholdStruct& threshold )
{
	//贪婪算法，每次取出最大的三角形，能和多少三角形合并
	Triangle& CurTri = TriCt.front();

	//排除过小的面
	if (CurTri.m_fArea < FLT_EPSILON)	
	{
		TriCt.pop_front();
		return;
	}

	//加入新的面
	CollapsedFace NewFace;
	InitFace(NewFace, CurTri, VBDesc);

	//再遍历没有塌陷的，如果能塌陷就移动到塌陷列表
	TypeTriangleContainer::iterator itOther = TriCt.begin();
	++itOther;
	while(itOther != TriCt.end())
	{
		Triangle& OtherTri = *itOther;
		if(CanCollapseTriToFace(NewFace, OtherTri, VBDesc, threshold))
		{
			UpdateFace(NewFace, OtherTri, VBDesc, threshold);
			itOther = TriCt.erase(itOther);
		}
		else
		{
			++itOther;
		}
	}

	TriCt.pop_front();//Triangle放入到Face之后就可以删除了

	PutFaceIntoList(FaceCt, NewFace);
	return;
}

BOOL KG3DBillboardCloudCreater2Solid::IsFaceLookingOut( const CollapsedFace& Face, const D3DXVECTOR3& ModelCenter )
{
	return 0 > D3DXVec3Dot(&Face.m_vNormal, &(ModelCenter - Face.m_vCenter));
}

HRESULT KG3DBillboardCloudCreater2Solid::MeshToTriangle(VertexBufferDesc VBDesc
												   , IndexBufferDesc IBDesc
												   , TypeTriangleContainer& TriCt
												   , ThresholdStruct& threshold )
{
	DWORD dwNumFaces = IBDesc.GetNumFaces();
	for (size_t i = 0; i < dwNumFaces; ++i)
	{
		UINT	Indices[3];
		for (size_t j = 0; j < 3; j++)
			Indices[j] = GetIndexFromBuffer(i * 3 + j, IBDesc);

		Triangle CurTri;
		BOOL bRet = InitTriangle(CurTri, VBDesc, Indices);
		if(! bRet)
			continue;

		if(CurTri.m_fArea < threshold.m_fMinAreaAccepted)
			continue;

		TriCt.push_back(CurTri);
	}	

	//把面积最大的三角形向上移动
	struct TriangleSorter
	{
		bool operator()(const Triangle& _Left, const Triangle& _Right) const
		{	
			return _Left.m_fArea > _Right.m_fArea;
		}
	};
	TriCt.sort(TriangleSorter());

	return S_OK;
}

HRESULT KG3DBillboardCloudCreater2Solid::ConstructBillboard()
{
	KG3DBillboardCloudBuildStruct BS;
	BS.m_pBuffer = m_lpVertexBuffer;
	BS.m_pTexture = m_lpTexture;

	_ASSERTE(NULL != m_lpModel);
	KG3DMaterial* pMaterial = m_lpModel->GetMaterial();
	KG_PROCESS_ERROR(NULL != pMaterial);

	{
#if defined(DEBUG) | defined(_DEBUG)
		ULONG uRefTexture = KGGetRef(m_lpTexture);
		ULONG uRefVB = KGGetRef(m_lpVertexBuffer);
#endif
		KG3DBillboardCloud* pCloud = KG3DBillboardCloud::Build(NULL, *pMaterial
			, reinterpret_cast<DWORD_PTR>(&BS));//里面会AddRef
		KG_PROCESS_ERROR(NULL != pCloud);
		//确认引用计数有增加
		_ASSERTE(KGGetRef(m_lpTexture) == uRefTexture + 1 && KGGetRef(m_lpVertexBuffer) == uRefVB + 1);

		m_lpModel->SetBillboardCloud(pCloud);

		_ASSERTE(NULL == m_pBillboardCloud);
		m_pBillboardCloud = pCloud;
		m_pBillboardCloud->AddRef();
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

void KG3DBillboardCloudCreater2Solid::OnReceiveMessage( const KG3DMessage& Message , IKG3DMessageSenderBase* pSender )
{
	switch(Message.m_uMessage)
	{
	case KM_D3DDEVICE_LOST:
		{
			m_lpTexture->OnLostDevice();
			m_lpDepthTexture->OnLostDevice();
		}
		break;
	case KM_D3DDEVICE_RESET:
		{
			m_lpTexture->OnResetDevice();
			m_lpDepthTexture->OnResetDevice();
		}
	    break;
	default:
	    break;
	}
}


//
//bool KG3DBillboardCloudCreater2Solid::ColomnLeftSpace::IsSpaceEnough( FLOAT maxEdge, FLOAT minEdge )
//{
//	_ASSERTE(MT_FLOAT_EQUAL(maxEdge, fWidth) || MT_FLOAT_EQUAL(maxEdge, fWidth/2));
//}
//
//size_t KG3DBillboardCloudCreater2Solid::ColomnLeftSpace::CalColumn( FLOAT fPercent, ColomnLeftSpace[], size_t Count )
//{
//	for (size_t i = 0; i < Count; ++i)
//	{
//		if(fPercent > ColomnLeftSpace[i+1].fWidth && fPercent < ColomnLeftSpace[i].fWidth + FLT_EPSILON)
//			return i;
//	}
//	return 0;
//}
KG3DBillboardCloudCreater2Solid::ThresholdStruct::ThresholdStruct()
{
	m_fMaxNormalBias = (cos(D3DX_PI*30/180))
		,m_fMaxNormalDistanceBiasPercent = (0.2f)
		,m_fMaxDistanceToPoint = (0.1f)
		,m_fMinAreaInfluenceAtNormalCorrection = (1/20)
		,m_fMinAreaAccepted = (FLT_EPSILON*2);
}
#if defined(KG_ENABLE_TDD)
KG_TEST_BEGIN(KG3DBillboardCloudCreater)
{
	static KG3DSystemEventListenerDummy DummyLst;
	struct TestBCC : public KG3DBillboardCloudCreater2Solid
	{
		TestBCC():KG3DBillboardCloudCreater2Solid(DummyLst){}
		using KG3DBillboardCloudCreater2Solid::TypeTriangleContainer;
		using KG3DBillboardCloudCreater2Solid::TypeFaceContainer;
		using KG3DBillboardCloudCreater2Solid::Triangle;
		using KG3DBillboardCloudCreater2Solid::CollapsedFace;
		using KG3DBillboardCloudCreater2Solid::ThresholdStruct;
		//////////////////////////////////////////////////////////////////////////
		using KG3DBillboardCloudCreater2Solid::UpdateFaceCenterAndSize;
		using KG3DBillboardCloudCreater2Solid::CalculateRight;
		using KG3DBillboardCloudCreater2Solid::InitFace;
		using KG3DBillboardCloudCreater2Solid::InitTriangle;
		using KG3DBillboardCloudCreater2Solid::CanCollapseTriToFace;
		using KG3DBillboardCloudCreater2Solid::UpdateFace;
		//////////////////////////////////////////////////////////////////////////
		using KG3DBillboardCloudCreater2Solid::VertexBufferDesc;
		using KG3DBillboardCloudCreater2Solid::IndexBufferDesc;
		using KG3DBillboardCloudCreater2Solid::GetVertexFromBuffer;
		using KG3DBillboardCloudCreater2Solid::GetIndexFromBuffer;
		//////////////////////////////////////////////////////////////////////////
		using KG3DBillboardCloudCreater2Solid::CalTotalPercent;
		using KG3DBillboardCloudCreater2Solid::EdgeSizeDisperse;
		using KG3DBillboardCloudCreater2Solid::TextureUVSpace;
		using KG3DBillboardCloudCreater2Solid::FaceUV;
		using KG3DBillboardCloudCreater2Solid::FillTextureUVSpace;
		using KG3DBillboardCloudCreater2Solid::CollapseTriangleToFaces;

	};

#if 0	//提交的时候应该去掉
	//GetVertexFromBuffer
	{
		struct VertexStruct 
		{
			D3DXVECTOR3 v1;
			D3DXVECTOR3 v2;
		};	
		VertexStruct VBBufferDummy[4];
		VBBufferDummy[0].v1 = D3DXVECTOR3(1,2,3);
		VBBufferDummy[3].v1 = D3DXVECTOR3(0,0,0);
		TestBCC::VertexBufferDesc VBDesc((CHAR*)VBBufferDummy, sizeof(VertexStruct));

		const D3DXVECTOR3& PT0 = TestBCC::GetVertexFromBuffer(0, VBDesc);
		_ASSERTE(MT_VECTOR_EQUAL(PT0, VBBufferDummy[0].v1, FLT_EPSILON));
		const D3DXVECTOR3& PT3 = TestBCC::GetVertexFromBuffer(3, VBDesc);
		_ASSERTE(MT_VECTOR_EQUAL(PT3, VBBufferDummy[3].v1, FLT_EPSILON));
	}
	//GetIndexFromBuffer
	{
		{
			USHORT IBBufferDummy[4] = {1000,125,0,USHRT_MAX};

			TestBCC::IndexBufferDesc IBDesc((CHAR*)IBBufferDummy, FALSE, _countof(IBBufferDummy)*sizeof(USHORT));

			UINT IB0 = TestBCC::GetIndexFromBuffer(0, IBDesc);
			_ASSERTE(IB0 == IBBufferDummy[0]);

			UINT IB3 = TestBCC::GetIndexFromBuffer(3, IBDesc);
			_ASSERTE(IB3 == IBBufferDummy[3]);

		}
		{
			UINT IBBufferDummy[4] = {1000,125,0,UINT_MAX};

			TestBCC::IndexBufferDesc IBDesc((CHAR*)IBBufferDummy, TRUE, _countof(IBBufferDummy)*sizeof(IBBufferDummy));

			UINT IB0 = TestBCC::GetIndexFromBuffer(0, IBDesc);
			_ASSERTE(IB0 == IBBufferDummy[0]);

			UINT IB3 = TestBCC::GetIndexFromBuffer(3, IBDesc);
			_ASSERTE(IB3 == IBBufferDummy[3]);
		}
	}

	//UpdateFaceCenterAndSize
	{
		TestBCC::CollapsedFace Face;
		Face.m_vNormal = D3DXVECTOR3(0,1,0);
		Face.m_vCenter = D3DXVECTOR3(1,-1,1);
		Face.m_vRight = D3DXVECTOR3(1,0,0);
		Face.m_vDown = D3DXVECTOR3(0,0,-1);
		Face.m_fHalfWidth = 0;
		Face.m_fHalfHeight = 0;


		//UpdateFaceCenterAndSize是打平撑开的
		TestBCC::UpdateFaceCenterAndSize(Face, D3DXVECTOR3(2,2,2));
		_ASSERTE(MT_FLOAT_EQUAL(Face.m_fHalfWidth, 0.5f));
		_ASSERTE(MT_FLOAT_EQUAL(Face.m_fHalfHeight, 0.5f));
		_ASSERTE(MT_VECTOR_EQUAL(Face.m_vCenter, D3DXVECTOR3(1.5f, -1, 1.5f), 0.01));

		TestBCC::UpdateFaceCenterAndSize(Face, D3DXVECTOR3(0,0,0));
		_ASSERTE(MT_FLOAT_EQUAL(Face.m_fHalfWidth, 1));
		_ASSERTE(MT_FLOAT_EQUAL(Face.m_fHalfHeight, 1));
		_ASSERTE(MT_VECTOR_EQUAL(Face.m_vCenter, D3DXVECTOR3(1.0f, -1, 1.0f), 0.01));

		TestBCC::UpdateFaceCenterAndSize(Face, D3DXVECTOR3(-2,-2,-2));
		_ASSERTE(MT_FLOAT_EQUAL(Face.m_fHalfWidth, 2));
		_ASSERTE(MT_FLOAT_EQUAL(Face.m_fHalfHeight, 2));
		_ASSERTE(MT_VECTOR_EQUAL(Face.m_vCenter, D3DXVECTOR3(0.0f, -1, 0.0f), 0.01));

	}

	//InitTriangle
	{
		D3DXVECTOR3 VerticesBuffer[3] = {D3DXVECTOR3(0,0,1), D3DXVECTOR3(1,0,0), D3DXVECTOR3(0,0,0)};
		D3DXVECTOR3 VerticesBufferInver[3] = {D3DXVECTOR3(0,0,1), D3DXVECTOR3(0,0,0), D3DXVECTOR3(1,0,0)};
		//const D3DXVECTOR3* Vertices[3] = {VerticesBuffer+0, VerticesBuffer+1, VerticesBuffer+2};
		//const D3DXVECTOR3* VerticesInverse[3] = {VerticesBuffer+0, VerticesBuffer+2, VerticesBuffer+1};

		TestBCC::VertexBufferDesc VBDesc((CHAR*)VerticesBuffer, sizeof(D3DXVECTOR3));
		TestBCC::VertexBufferDesc VBDescInverse((CHAR*)VerticesBufferInver, sizeof(D3DXVECTOR3));

		UINT	Indices[3] = {0,1,2};

		{
			//点的顺序会遵循左手定则
			TestBCC::Triangle triangle;
			BOOL bRet = TestBCC::InitTriangle(triangle, VBDesc, Indices);
			_ASSERTE(bRet);
			_ASSERTE(MT_VECTOR_EQUAL(triangle.m_vNormal, D3DXVECTOR3(0,1,0), 0.01f));
			_ASSERTE(MT_FLOAT_EQUAL(triangle.m_fArea, 0.5f));
			_ASSERTE(MT_VECTOR_EQUAL(triangle.m_vCenter, D3DXVECTOR3(0.5,0,0.5f), 0.01f));

			for(size_t j = 0; j < 3; j++)
			{
				_ASSERTE(triangle.m_VertexIndices[j] == Indices[j]);
			}
		}
		{
			TestBCC::Triangle triangle;
			BOOL bRet = TestBCC::InitTriangle(triangle, VBDescInverse, Indices);
			_ASSERTE(bRet);
			_ASSERTE(MT_VECTOR_EQUAL(triangle.m_vNormal, D3DXVECTOR3(0,-1,0), 0.01f));
		}
	}

	//InitFace
	{
		D3DXVECTOR3 lpVB[] = {D3DXVECTOR3(1,1,0), D3DXVECTOR3(-1,0,0),D3DXVECTOR3(0,-1,0)};
		TestBCC::VertexBufferDesc VBDesc((CHAR*)lpVB, sizeof(D3DXVECTOR3));

		typedef TestBCC::Triangle Triangle;
		Triangle triangle1;

		UINT Indices[3] = {0,1,2};
		TestBCC::InitTriangle(triangle1, VBDesc, Indices);

		typedef TestBCC::CollapsedFace TypeFace;
		TypeFace face;
		TestBCC::InitFace(face, triangle1, VBDesc);

		_ASSERTE(MT_VECTOR_EQUAL(face.m_vNormal, D3DXVECTOR3(0,0,1), 0.01f));
		_ASSERTE(MT_VECTOR_EQUAL(face.m_vCenter, D3DXVECTOR3(0,0,0), 0.01f));
		_ASSERTE(MT_VECTOR_EQUAL(face.m_vRight, D3DXVECTOR3(-1,0,0), 0.01f));
		_ASSERTE(MT_VECTOR_EQUAL(face.m_vDown, D3DXVECTOR3(0,-1,0), 0.01f));
		_ASSERTE(MT_FLOAT_EQUAL_BIAS(face.m_fHalfWidth, 1, 0.001));	
		_ASSERTE(MT_FLOAT_EQUAL_BIAS(face.m_fHalfHeight, 1, 0.001));	
	}

	//CanCollapseTriToFace
	{
		TestBCC::CollapsedFace standardFace;
		standardFace.m_vNormal = D3DXVECTOR3(0,1,0);
		standardFace.m_vCenter = D3DXVECTOR3(0,0,0);
		standardFace.m_vRight = D3DXVECTOR3(1,0,0);
		standardFace.m_vDown = D3DXVECTOR3(0,0,-1);
		standardFace.m_fHalfWidth = 1;
		standardFace.m_fHalfHeight = 1;

		D3DXVECTOR3 lpVB[] = {D3DXVECTOR3(1,0,0), D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,1)};
		TestBCC::VertexBufferDesc VBDesc((CHAR*)lpVB, sizeof(D3DXVECTOR3));

		UINT Indices[3] = {0,1,2};

		TestBCC::ThresholdStruct threshold;

		//Normal偏移过多
		{
			TestBCC::Triangle tri;
			TestBCC::InitTriangle(tri, VBDesc, Indices);
			
			TestBCC::CollapsedFace face = standardFace;
			face.m_vNormal = D3DXVECTOR3(0,0,1);

			BOOL bRet = TestBCC::CanCollapseTriToFace(face, tri, VBDesc, threshold);
			_ASSERTE(! bRet);
		}

		FLOAT fMaxVerticalBias = threshold.m_fMaxNormalDistanceBiasPercent*standardFace.GetMinEdge() + FLT_EPSILON;
		lpVB[0] = D3DXVECTOR3(0,0,fMaxVerticalBias);
		lpVB[1] = D3DXVECTOR3(0,1,fMaxVerticalBias);
		lpVB[2] = D3DXVECTOR3(1,0,fMaxVerticalBias);

		//纵向偏移过大
		{	
			TestBCC::Triangle tri;
			TestBCC::InitTriangle(tri, VBDesc, Indices);
			TestBCC::CollapsedFace face = standardFace;
			face.m_vCenter -= face.m_vNormal * 1000;
			BOOL bRet = TestBCC::CanCollapseTriToFace(standardFace, tri, VBDesc, threshold);
			_ASSERTE(! bRet);
		}

		//横向偏移过大
		{
			lpVB[0] = D3DXVECTOR3(2+1,0,0);
			lpVB[1] = D3DXVECTOR3(1+1,0,0);
			lpVB[2] = D3DXVECTOR3(1+1,0,1);

			TestBCC::Triangle tri;
			TestBCC::InitTriangle(tri, VBDesc, Indices);
			BOOL bRet = TestBCC::CanCollapseTriToFace(standardFace, tri, VBDesc, threshold);
			_ASSERTE(! bRet);

			lpVB[0] = D3DXVECTOR3(0,0,3000);
			lpVB[1] = D3DXVECTOR3(0,0,0);//只要有一个点在里面，就能合并
			lpVB[2] = D3DXVECTOR3(3000,0,0);
			TestBCC::InitTriangle(tri, VBDesc, Indices);
			TestBCC::CollapsedFace face = standardFace;
			face.m_fHalfWidth = 1000;
			face.m_fHalfHeight = 1000;
			tri.m_fArea = face.GetArea();
			bRet = TestBCC::CanCollapseTriToFace(face, tri, VBDesc, threshold);
			_ASSERTE(bRet);

		}

		//所有还原，面积过小
		{
			lpVB[0] = D3DXVECTOR3(1,0,0);
			lpVB[1] = D3DXVECTOR3(0,0,0);
			lpVB[2] = D3DXVECTOR3(0,0,1);

			TestBCC::Triangle tri;
			TestBCC::InitTriangle(tri, VBDesc, Indices);
			tri.m_fArea = threshold.m_fMinAreaInfluenceAtNormalCorrection * standardFace.GetArea();
			tri.m_fArea -= FLT_EPSILON;
			BOOL bRet = TestBCC::CanCollapseTriToFace(standardFace, tri, VBDesc, threshold);
			_ASSERTE(! bRet);

		}

		//所有条件满足
		{
			TestBCC::Triangle tri;
			TestBCC::InitTriangle(tri, VBDesc, Indices);
			TestBCC::CollapsedFace face = standardFace;
			BOOL bRet = TestBCC::CanCollapseTriToFace(face, tri, VBDesc, threshold);
			_ASSERTE(bRet);
		}

		//翻转法线应该也能合并
		{
			TestBCC::Triangle tri;
			TestBCC::InitTriangle(tri, VBDesc, Indices);
			tri.m_vNormal = -tri.m_vNormal;
			TestBCC::CollapsedFace face = standardFace;
			BOOL bRet = TestBCC::CanCollapseTriToFace(face, tri, VBDesc, threshold);
			_ASSERTE(bRet);
		}

		//tri的面积太大，face不能合并比自己面大的tri
		{
			TestBCC::Triangle tri;
			TestBCC::InitTriangle(tri, VBDesc, Indices);
			tri.m_fArea = standardFace.GetArea()+0.1f;
			BOOL bRet = TestBCC::CanCollapseTriToFace(standardFace, tri, VBDesc, threshold);
			_ASSERTE(! bRet);

		}
	}//Endof	CanCollapseTriToFace
	//CalculateRight
	{
		TestBCC::CollapsedFace CaseArray[5];
		D3DXVECTOR3	  RightRetArray[5];
		for (size_t i = 0; i < _countof(CaseArray); ++i)
		{
			CaseArray[i].m_vCenter = D3DXVECTOR3(1,1,1);//Center的位置不重要
		}
		CaseArray[0].m_vNormal = D3DXVECTOR3(FLT_EPSILON,1,0);
		RightRetArray[0] = D3DXVECTOR3(0,0,1);

		CaseArray[1].m_vNormal = D3DXVECTOR3(-FLT_EPSILON,1,0);
		RightRetArray[1] = D3DXVECTOR3(0,0,-1);

		CaseArray[2].m_vNormal = D3DXVECTOR3(0,0,1);
		RightRetArray[2] = D3DXVECTOR3(-1,0,0);

		CaseArray[3].m_vNormal = D3DXVECTOR3(-1,0,0);
		RightRetArray[3] = D3DXVECTOR3(0,0,-1);

		CaseArray[4].m_vNormal = D3DXVECTOR3(0,0,-1);
		RightRetArray[4] = D3DXVECTOR3(1,0,0);

		CaseArray[4].m_vNormal = D3DXVECTOR3(1,0,0);
		RightRetArray[4] = D3DXVECTOR3(0,0,1);

		for (size_t i = 0; i < _countof(CaseArray); ++i)
		{
			TestBCC::CalculateRight(CaseArray[i]);
			_ASSERTE(MT_VECTOR_EQUAL(CaseArray[i].m_vRight, RightRetArray[i], 0.001f));
		}
	}
	//UpdateFace
	{
		D3DXVECTOR3 lpVB[] = {D3DXVECTOR3(1,0,0), D3DXVECTOR3(0,0,0),D3DXVECTOR3(0,0,1),
			D3DXVECTOR3(1,0,0), D3DXVECTOR3(0,0.1f,0),D3DXVECTOR3(0,0,1)};
		TestBCC::VertexBufferDesc VBDesc((CHAR*)lpVB, sizeof(D3DXVECTOR3));

		TestBCC::ThresholdStruct threshold;

		TestBCC::CollapsedFace face;
		{
			TestBCC::Triangle tri;

			UINT Indices[3] = {0,1,2};

			TestBCC::InitTriangle(tri, VBDesc, Indices);


			TestBCC::InitFace(face, tri, VBDesc);
			_ASSERTE(MT_VECTOR_EQUAL(face.m_vNormal, D3DXVECTOR3(0,1,0), 0.01));
			_ASSERTE(MT_VECTOR_EQUAL(face.m_vCenter, D3DXVECTOR3(0.5, 0, 0.5f), 0.01));
			_ASSERTE(MT_VECTOR_EQUAL(face.m_vRight, D3DXVECTOR3(0,0,-1), 0.01));
			_ASSERTE(MT_VECTOR_EQUAL(face.m_vDown, D3DXVECTOR3(-1,0,0), 0.01));
			_ASSERTE(MT_FLOAT_EQUAL(face.m_fHalfHeight, 0.5));
			_ASSERTE(MT_FLOAT_EQUAL(face.m_fHalfWidth, 0.5));


			TestBCC::UpdateFace(face, tri, VBDesc, threshold);//同一个FaceUpdate，应该啥都不变
			_ASSERTE(MT_VECTOR_EQUAL(face.m_vNormal, D3DXVECTOR3(0,1,0), 0.01));
			_ASSERTE(MT_VECTOR_EQUAL(face.m_vCenter, D3DXVECTOR3(0.5, 0, 0.5f), 0.01));
			_ASSERTE(MT_VECTOR_EQUAL(face.m_vRight, D3DXVECTOR3(0,0,-1), 0.01));
			_ASSERTE(MT_FLOAT_EQUAL(face.m_fHalfHeight, 0.5));
			_ASSERTE(MT_FLOAT_EQUAL(face.m_fHalfWidth, 0.5));
		}

		{
			TestBCC::Triangle tri;

			UINT	Indices[3] = {3,4,5};
			TestBCC::InitTriangle(tri, VBDesc, Indices);
			BOOL bRet = TestBCC::CanCollapseTriToFace(face, tri, VBDesc, threshold);
			_ASSERTE(bRet);

			D3DXPLANE plane;
			D3DXPlaneFromPoints(&plane, lpVB + 3, lpVB + 4, lpVB + 5);
			D3DXVECTOR3 vTemp = *(D3DXVECTOR3*)&plane;
			vTemp = (vTemp * 0.5f + face.m_vNormal * 1);
			D3DXVec3Normalize(&vTemp, &vTemp);

			TestBCC::UpdateFace(face, tri, VBDesc, threshold);

			_ASSERTE(MT_VECTOR_EQUAL(face.m_vNormal, vTemp, 0.001));
			_ASSERTE(MT_VECTOR_EQUAL(face.m_vCenter, D3DXVECTOR3(0.425f, 0.005f, 0.425f),0.001f));
			_ASSERTE(MT_VECTOR_EQUAL(face.m_vRight, D3DXVECTOR3(-0.707f,0,0.707f), 0.01));

			_ASSERTE(MT_FLOAT_EQUAL_BIAS(face.m_fHalfHeight, 0.605f, 0.01f));
			_ASSERTE(MT_FLOAT_EQUAL_BIAS(face.m_fHalfWidth, 0.707f, 0.01f));
		}

	}

	//CalTotalPercent
	{
		_ASSERTE(TestBCC::CalTotalPercent(1) == 2);
		_ASSERTE(TestBCC::CalTotalPercent(2) == 4);
		_ASSERTE(TestBCC::CalTotalPercent(3) == 8);
		_ASSERTE(TestBCC::CalTotalPercent(4) == 16);
	}
	//EdgeSizeDisperse
	{
		struct Case 
		{
			FLOAT In;
			FLOAT Out;
		};
		FLOAT fTotalPercent = TestBCC::CalTotalPercent(5);
		_ASSERTE(fTotalPercent == 32);
		Case CaseArray[] = 
		{
			{0, 0.f},
			{1.f/32.f, 1.f/fTotalPercent},
			{2.f/32.f, 2.f/32.f},
			{3.f/32.f, 4.f/32.f},
			{4.f/32.f, 4.f/32.f},
			{16.f/32.f, 16.f/32.f},
			{23.f/32.f, 16.f/32.f},
			{24.f/32.f, 32.f/32.f},
			{33.f/32.f, 32.f/32.f},
		};
		for (size_t i = 0; i < _countof(CaseArray); ++i)
		{
			_ASSERTE(TestBCC::EdgeSizeDisperse(CaseArray[i].In, fTotalPercent) == CaseArray[i].Out);
		}
	}
	//FillTextureUVSpace
	{
		//1
		{
			TestBCC::TextureUVSpace UVSpace;
			_ASSERTE(MT_VECTOR_EQUAL(UVSpace.m_Space.front(), D3DXVECTOR3(0,0,1), FLT_EPSILON));
			_ASSERTE(MT_FLOAT_EQUAL(UVSpace.m_Space.front().w , 1));
			TestBCC::FaceUV SourceArray[] = 
			{
				{FALSE, 0, 0, 1, 0.5f},
				{FALSE, 0, 0.5f, 0.4f, 0.4f},
				{FALSE, 0, 0.9f, 0.2f, 0.1f},
				{FALSE, 0.2f, 0.9f, 0.2f, 0.1f},
				{FALSE, 0.4f, 0.5f, 0.6f, 0.5f}, 
			};
			for (size_t i = 0; i < _countof(SourceArray); ++i)
			{
				TestBCC::FaceUV TempUV = SourceArray[i];
				BOOL bRet = TestBCC::FillTextureUVSpace(UVSpace, TempUV);
				_ASSERTE(bRet);
				_ASSERTE(MT_FLOAT_EQUAL(TempUV.x, SourceArray[i].x));
				_ASSERTE(MT_FLOAT_EQUAL(TempUV.y, SourceArray[i].y));
			}
			_ASSERTE(UVSpace.m_Space.empty());
		}
		//2
		{
			TestBCC::TextureUVSpace UVSpace;
			TestBCC::FaceUV SourceArray[] = 
			{
				{FALSE, 0, 0, 0.5, 0.5f},
				{FALSE, 0, 0.5f, 0.5f, 0.5f},
				{FALSE, 0.5, 0.0f, 0.5f, 0.5f},
				{FALSE, 0.5, 0.5f, 0.5f, 0.5f},
			};
			for (size_t i = 0; i < _countof(SourceArray); ++i)
			{
				TestBCC::FaceUV TempUV = SourceArray[i];
				BOOL bRet = TestBCC::FillTextureUVSpace(UVSpace, TempUV);
				_ASSERTE(bRet);
				_ASSERTE(MT_FLOAT_EQUAL(TempUV.x, SourceArray[i].x));
				_ASSERTE(MT_FLOAT_EQUAL(TempUV.y, SourceArray[i].y));
			}
			_ASSERTE(UVSpace.m_Space.empty());
		}
	}
	//CollapseTriangleToFaces
	{
		D3DXVECTOR3 VB[] = 
		{
			D3DXVECTOR3(0,0,0),
			D3DXVECTOR3(200,0,0),
			D3DXVECTOR3(200,0,200),
			D3DXVECTOR3(0,0,200),

			D3DXVECTOR3(0,200,0),
			D3DXVECTOR3(200,200,0),
			D3DXVECTOR3(200,200,200),
			D3DXVECTOR3(0,200,200),
		};

		typedef UINT TypeTriIndices[3];

		TypeTriIndices IndicesGroup[] = 
		{
			{0,1,3},
			{1,2,3},
			{1,5,6},
			{6,2,1},
			{6,7,3},
			{7,3,2},
			{0,3,7},
			{7,4,0},
			{0,4,1},
			{4,5,1},
			{4,7,6},
			{4,6,5},
		};

		TestBCC::VertexBufferDesc VBDesc((CHAR*)VB, sizeof(D3DXVECTOR3));

		TestBCC::TypeTriangleContainer TriCt;
		TestBCC::TypeFaceContainer	FaceCt;

		for (size_t i = 0; i < _countof(IndicesGroup); ++i)
		{
			TestBCC::Triangle tri;
			BOOL bRet = TestBCC::InitTriangle(tri, VBDesc, IndicesGroup[i]);
			_ASSERTE(bRet);
			TriCt.push_back(tri);
		}

		TestBCC::ThresholdStruct threshold;
		TestBCC::CollapseTriangleToFaces(TriCt, FaceCt, VBDesc, threshold);
		_ASSERTE(FaceCt.size() == 6);
		D3DXVECTOR3 vFirstNormal = FaceCt.front().m_vNormal;
		_ASSERTE(MT_VECTOR_EQUAL(vFirstNormal, D3DXVECTOR3(0,-1,0), FLT_EPSILON));
	}
#endif
}
KG_TEST_END(KG3DBillboardCloudCreater)
#endif


KG3DBillboardCloudCreater2* KG3DBillboardCloudCreater2::Build( LPCTSTR, DWORD_PTR, DWORD_PTR )
{
	return new KG3DBillboardCloudCreater2Solid(g_GetSystemEventListener());
}

int KG3DBillboardCloudCreater::GetTextureWidth()
{
	return m_dwTextureWidth;
}

void KG3DBillboardCloudCreater::SetTextureWidth(int nWidth)
{
	HRESULT hr = E_FAIL;

	if(m_dwTextureWidth != (DWORD)nWidth)
	{
		m_dwTextureWidth = nWidth;

		if(m_lpRenderTargetTool)
		{
			m_lpRenderTargetTool->UnInit();
			
			hr = m_lpRenderTargetTool->Init(m_dwTextureWidth,TRUE,D3DFMT_A8R8G8B8);
			KGLOG_COM_PROCESS_ERROR(hr);

		}

		SAFE_RELEASE(m_lpTextureColor);
		SAFE_RELEASE(m_lpTextureNormal);

		hr = g_pd3dDevice->CreateTexture( m_dwTextureWidth, m_dwTextureWidth,1, 0,
			D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED,
			&m_lpTextureColor,NULL);
		KGLOG_COM_PROCESS_ERROR(hr);

		hr = g_pd3dDevice->CreateTexture( m_dwTextureWidth, m_dwTextureWidth,1, 0,
			D3DFMT_A8R8G8B8,
			D3DPOOL_MANAGED,
			&m_lpTextureNormal,NULL);
		KGLOG_COM_PROCESS_ERROR(hr);
	}

Exit0:
	;
}