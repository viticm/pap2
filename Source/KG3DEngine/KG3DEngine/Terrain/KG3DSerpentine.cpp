#include "StdAfx.h"
#include "Terrain/KG3DSerpentine.h"
//#include "Terrain/KG3DTerrainEx.h"
#include "KG3DRepresentObjectNode.h"
#include "KG3DGraphicsTool.h"
#include "../scene/KG3DSceneSceneEditor.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DMemoryFile.h"
#include "KG3DHeightMap.h"
#include "KG3DTerrainBlockModifier.h"
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern BOOL g_bClient;///是否客户端

KG3DSerpentine::KG3DSerpentine(void)
{
	m_fWidth = 2000;
	m_bDrawSelectMark = TRUE;
	m_bDrawExtendLine = FALSE;
	m_scName = "Serpentine";
	m_fBendModulus = 0.8f;
	m_fLength = 0;
	m_fNodeSize = 10;
	m_nSegmentLength =200;
	m_fExtendWidth = 0;
	m_uHandle = 0;
	m_ulRefCount = 0;
	AddRef();
}
KG3DSerpentine::~KG3DSerpentine(void)
{
	//UnInit();
}
void KG3DSerpentine::UnInit()
{
	DeleteAllNode();
	DeleteAllPassage();
	m_uHandle = 0;
}
BOOL KG3DSerpentine::IsInPassageSet(set<KG3DPassage*>* psetPassage,KG3DPassage* pPassage)
{
	set<KG3DPassage*>::iterator i = psetPassage->find(pPassage);
	if(i != psetPassage->end())
		return TRUE;
	return FALSE;
}
static BOOL bB = TRUE;
HRESULT KG3DSerpentine::ComputePassages(KG3DRepresentObjectNode* pCurNode)
{
	HRESULT hr = E_FAIL;
	set<KG3DPassage*> setPassage;
	GetNeedModifyPassageAndNode(pCurNode,&setPassage);

	m_BBox.Clear();
	m_ExtendBBox.Clear();
	{
		KG3DPassage* pLastPassage = NULL;
		int N = 0;
		int NPassage = 0;
		list<KG3DRepresentObjectNode*>::iterator i = m_listNode.begin();
		list<KG3DRepresentObjectNode*>::iterator iend = m_listNode.end();
		while (i!=iend)
		{
			KG3DRepresentObjectNode* pNode = *i;
			list<KG3DRepresentObjectNode*>::iterator j = i;
			++j;
			if(j!=iend)
			{
				KG3DRepresentObjectNode* pNodeNext = *j;

				{
					KG3DPassage* pnewPassage = NULL;
					if (FindPassage(&pnewPassage,pNode,pNodeNext))
					{	
						if(IsInPassageSet(&setPassage,pnewPassage))
						{
							pnewPassage->UnInit();
							//							
							pnewPassage->m_pNodeA = pNode;
							pnewPassage->m_pNodeB = pNodeNext;
							pNode->GetTranslation(&(pnewPassage->m_vPositionA));
							pNodeNext->GetTranslation(&(pnewPassage->m_vPositionB));
							pnewPassage->m_fABDistance = D3DXVec3Length(&(pnewPassage->m_vPositionB - pnewPassage->m_vPositionA));

							pnewPassage->m_vNormalA = pnewPassage->m_vPositionA - pnewPassage->m_vPositionB;
							pnewPassage->m_vNormalB = pnewPassage->m_vPositionB - pnewPassage->m_vPositionA;
							pnewPassage->m_vNormalA.y = 0;
							pnewPassage->m_vNormalB.y = 0;
							D3DXVec3Normalize(&pnewPassage->m_vNormalA,&pnewPassage->m_vNormalA);
							D3DXVec3Normalize(&pnewPassage->m_vNormalB,&pnewPassage->m_vNormalB);

							if(pLastPassage)
							{
								if(NPassage == 0)
								{
									pnewPassage->m_vNormalA = -pLastPassage->m_vNormalB;
								}
								else
								{
									D3DXVECTOR3 normal = (pLastPassage->m_vNormalB - pnewPassage->m_vNormalA) * 0.5;
									D3DXVec3Normalize(&normal,&normal);

									pLastPassage->m_vNormalB = normal;
									pnewPassage->m_vNormalA = -normal;
									if(NPassage == (int)setPassage.size()-1)
									{
										KG3DPassage* pNextPassage = NULL;
										list<KG3DPassage*>::iterator iFind = m_listPassage.begin();
										while(iFind != m_listPassage.end())
										{
											if(pnewPassage == *iFind)
											{
												++iFind;
												if(iFind != m_listPassage.end())
												{
													pNextPassage = *iFind;

												}
												break;
											}
											++iFind;
										}
										if(pNextPassage)
										pnewPassage->m_vNormalB= -pNextPassage->m_vNormalA;
										
									}
								}

							}
							++NPassage;
						}


					}
					else
					{
						hr = AddOnePassage(&pnewPassage,N);
						if(SUCCEEDED(hr))
						{
							setPassage.insert(pnewPassage);
							pnewPassage->m_pNodeA = pNode;
							pnewPassage->m_pNodeB = pNodeNext;
							pNode->GetTranslation(&(pnewPassage->m_vPositionA));//pnewPassage->m_vPositionA = pNode->m_vPosition;
							pNodeNext->GetTranslation(&(pnewPassage->m_vPositionB));//pnewPassage->m_vPositionB = pNodeNext->m_vPosition;
							pnewPassage->m_fABDistance = D3DXVec3Length(&(pnewPassage->m_vPositionB - pnewPassage->m_vPositionA));

							pnewPassage->m_vNormalA = pnewPassage->m_vPositionA - pnewPassage->m_vPositionB;
							pnewPassage->m_vNormalB = pnewPassage->m_vPositionB - pnewPassage->m_vPositionA;
							pnewPassage->m_vNormalA.y = 0;
							pnewPassage->m_vNormalB.y = 0;
							D3DXVec3Normalize(&pnewPassage->m_vNormalA,&pnewPassage->m_vNormalA);
							D3DXVec3Normalize(&pnewPassage->m_vNormalB,&pnewPassage->m_vNormalB);

							if(pLastPassage)
							{
								D3DXVECTOR3 normal = (pLastPassage->m_vNormalB - pnewPassage->m_vNormalA) * 0.5;
								D3DXVec3Normalize(&normal,&normal);

								pLastPassage->m_vNormalB = normal;
								pnewPassage->m_vNormalA = -normal;
							}
						}
					}

					pLastPassage = pnewPassage;
				}				
			}
			N++;
			++i;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	{
		float fLengthSum = 0;
		list<KG3DPassage*>::iterator iPre;
		list<KG3DPassage*>::iterator i = m_listPassage.begin();
		list<KG3DPassage*>::iterator iend = m_listPassage.end();
		while (i!=iend)
		{
			KG3DPassage* pPassage = *i;
			if(IsInPassageSet(&setPassage,pPassage))
			{
				{
					D3DXVECTOR3 left;
					D3DXVec3Cross(&left,&pPassage->m_vNormalA,&D3DXVECTOR3(0,1,0));
					left.y = 0;
					D3DXVec3Normalize(&left,&left);

					pPassage->m_vBorderPosA[0] = pPassage->m_vPositionA - left * m_fWidth * 0.5F;
					pPassage->m_vBorderPosA[1] = pPassage->m_vPositionA + left * m_fWidth * 0.5F;

					pPassage->m_vExtendPosA[0] = pPassage->m_vPositionA - left * (m_fWidth * 0.5F + m_fExtendWidth);
					pPassage->m_vExtendPosA[1] = pPassage->m_vPositionA + left * (m_fWidth * 0.5F + m_fExtendWidth);

				}
				{
					D3DXVECTOR3 left;
					D3DXVec3Cross(&left,&pPassage->m_vNormalB,&D3DXVECTOR3(0,1,0));
					left.y = 0;
					D3DXVec3Normalize(&left,&left);

					pPassage->m_vBorderPosB[0] = pPassage->m_vPositionB + left * m_fWidth * 0.5F;
					pPassage->m_vBorderPosB[1] = pPassage->m_vPositionB - left * m_fWidth * 0.5F;

					pPassage->m_vExtendPosB[0] = pPassage->m_vPositionB + left * (m_fWidth * 0.5F + m_fExtendWidth);
					pPassage->m_vExtendPosB[1] = pPassage->m_vPositionB - left * (m_fWidth * 0.5F + m_fExtendWidth);
				}
			//	pPassage->m_fSerpentineLength[0] = D3DXVec3Length(&(pPassage->m_vSerpentinePositionB[0] - pPassage->m_vSerpentinePositionA[0]));
				//pPassage->m_fSerpentineLength[1] = D3DXVec3Length(&(pPassage->m_vSerpentinePositionB[1] - pPassage->m_vSerpentinePositionA[1]));

				pPassage->m_Bbox.Clear();
				pPassage->m_Bbox.AddPosition(pPassage->m_vBorderPosA,2);
				pPassage->m_Bbox.AddPosition(pPassage->m_vBorderPosB,2);

				pPassage->m_ExtendBBox.Clear();
				pPassage->m_ExtendBBox.AddPosition(pPassage->m_vExtendPosA,2);
				pPassage->m_ExtendBBox.AddPosition(pPassage->m_vExtendPosB,2);

				int num =(int) pPassage->m_fABDistance /m_nSegmentLength;
				if(num < 2)
					num = 2;
				D3DXVECTOR3* pVertex = new D3DXVECTOR3[num];
				pVertex[0] = pPassage->m_vPositionA;

				float K = m_fBendModulus;

				for (int j = 1;j<num; j++)
				{

					float S = (float)j / (float)num ;

					D3DXVec3Hermite(&pVertex[j],
						&pPassage->m_vPositionA,
						&(-pPassage->m_vNormalA*K*pPassage->m_fABDistance) ,
						&pPassage->m_vPositionB,
						&(pPassage->m_vNormalB*K*pPassage->m_fABDistance),
						S);
				}
				pVertex[num - 1] = pPassage->m_vPositionB;
				pPassage->m_Bbox.AddPosition(pVertex,num);
				float SaveLength = fLengthSum;
				pPassage->CreateTinyLines(pVertex,num,fLengthSum,m_fWidth,m_fExtendWidth);
				pPassage->m_fTotalLength = fLengthSum - SaveLength;
				SAFE_DELETE_ARRAY(pVertex);
			}
			else
			{
				fLengthSum += pPassage->m_fTotalLength;
			}
			m_BBox.AddPosition(pPassage->m_Bbox.A);
			m_BBox.AddPosition(pPassage->m_Bbox.B);
			m_ExtendBBox.AddPosition(pPassage->m_ExtendBBox.A);
			m_ExtendBBox.AddPosition(pPassage->m_ExtendBBox.B);
			++i;
		}

		m_fLength = fLengthSum;//计算整条河流的总长度
	}

	return S_OK;
}
HRESULT KG3DSerpentine::GetNeedModifyPassageAndNode(KG3DRepresentObjectNode* pInNode,set<KG3DPassage*>* psetPassageOut)
{
	list<KG3DRepresentObjectNode*>::iterator i = m_listNode.begin();
	list<KG3DRepresentObjectNode*>::iterator iPre,iNext;

	KG_PROCESS_ERROR(pInNode);
	KG_PROCESS_ERROR(psetPassageOut);

	while (i != m_listNode.end())
	{
		if (pInNode == *i)
		{
			iPre = iNext= i;

			FindPassage(psetPassageOut,pInNode);
			if(i == m_listNode.begin())
			{
				++iNext;
				if(iNext == m_listNode.end())
					break;
				//psetNodeOut->m_insert(*iNext);
				FindPassage(psetPassageOut,*iNext);
			}
			else
			{
				iPre--;
				//psetNodeOut->m_insert(*iPre);
				this->FindPassage(psetPassageOut,*iPre);
				++iNext;
				if(iNext == m_listNode.end())
					break;
				//psetNodeOut->m_insert(*iNext);
				FindPassage(psetPassageOut,*iNext);
			}
		}
		++i;
	}
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DSerpentine::ModifyData(set<KG3DPassage*>* psetPassage)
{
	BBox Bbox;
	int cell = 100;
	set<KG3DPassage*>::iterator i = psetPassage->begin();
	while(i!= psetPassage->end())
	{
		KG3DPassage* pPassage = *i;

		Bbox.AddPosition(pPassage->m_ExtendBBox.A);
		Bbox.AddPosition(pPassage->m_ExtendBBox.B);

		++i;
	}
	int nXStart = (int)(Bbox.A.x/cell);
	int nXEnd   = (int)(Bbox.B.x/cell);
	int nZStart = (int)(Bbox.A.z/cell);
	int nZEnd   = (int)(Bbox.B.z/cell);
	nXStart = max(0,nXStart);
	nZStart = max(0,nZStart);
	for (int z=nZStart;z<nZEnd;z++)
	{
		for (int x=nXStart;x<nXEnd;x++)
		{
			if(!IsTerrainCellInside(x,z,0))
				IsTerrainCellInside(x,z,1);
		}
	}
	i = psetPassage->begin();
	while(i!= psetPassage->end())
	{
		KG3DPassage* pPassage = *i;
		pPassage->CreateBuffers();
		pPassage->UpdateVertex(this);
		++i;
	}

	return S_OK;
}
HRESULT KG3DSerpentine::CreatePassages(KG3DRepresentObjectNode* pNode)
{
	set<KG3DPassage*> setPassage;

	GetNeedModifyPassageAndNode(pNode,&setPassage);
	return ModifyData(&setPassage);
}
BOOL KG3DSerpentine::FindPassage(set<KG3DPassage*>* psetPassage,KG3DRepresentObjectNode* pNode)
{
	BOOL bFind = FALSE;
	list<KG3DPassage*>::iterator i = m_listPassage.begin();
	list<KG3DPassage*>::iterator iend = m_listPassage.end();
	while (i != iend)
	{
		KG3DPassage* pPassage = *i;
		if ((pNode == pPassage->m_pNodeA )|| (pNode == pPassage->m_pNodeB)) 
		{
			psetPassage->insert(pPassage);
			bFind = TRUE;
		}
		++i;
	}
	return bFind;
}
BOOL KG3DSerpentine::FindPassage(KG3DPassage** ppOutPassage,KG3DRepresentObjectNode* pNodeA,KG3DRepresentObjectNode* pNodeB)
{
	list<KG3DPassage*>::iterator i = m_listPassage.begin();
	list<KG3DPassage*>::iterator iend = m_listPassage.end();
	while (i != iend)
	{
		KG3DPassage* pPassage = *i;
		if ((pNodeA == pPassage->m_pNodeA && pNodeB == pPassage->m_pNodeB)|| (pNodeB == pPassage->m_pNodeA && pNodeA == pPassage->m_pNodeB)) 
		{
			*ppOutPassage = pPassage;
			return TRUE;
		}
		++i;
	}
	*ppOutPassage = NULL;
	return FALSE;
}
///////////////////////////////////////////////////////////////	
HRESULT KG3DSerpentine::CheckVisablePassage(const KG3DFrustum* pFrustum)
{
	list<KG3DPassage*>::iterator i = m_listPassage.begin();
	while(i != m_listPassage.end())
	{
		KG3DPassage* pPassage = *i;
		if(pPassage && pFrustum->IsAABBoxVisible(pPassage->m_Bbox))
			pPassage->m_bIsVisable = TRUE;
		else
			pPassage->m_bIsVisable = FALSE;
		++i;
	}
	return S_OK;
}
HRESULT KG3DSerpentine::Render()
{
	list<KG3DPassage*>::iterator i = m_listPassage.begin();
	list<KG3DPassage*>::iterator iend = m_listPassage.end();
	while (i!=iend)
	{
		KG3DPassage* pPassage = *i;
		if(pPassage->m_bIsVisable)
			pPassage->Render();
		++i;
	}
	return S_OK;
}

HRESULT KG3DSerpentine::RenderLines()
{
	list<KG3DPassage*>::iterator i = m_listPassage.begin();
	list<KG3DPassage*>::iterator iend = m_listPassage.end();

	while (i!=iend)
	{
		KG3DPassage* pPassage = *i;
		pPassage->RenderTinyLines();
		++i;
	}
	return S_OK;
}

HRESULT KG3DSerpentine::RenderSelectMark()
{
	if ((!m_bDrawSelectMark) && (!m_bDrawExtendLine))
		return S_OK;

	list<KG3DPassage*>::iterator i = m_listPassage.begin();
	list<KG3DPassage*>::iterator iend = m_listPassage.end();
	while (i!=iend)
	{
		KG3DPassage* pPassage = *i;
		if(m_bDrawSelectMark)
			pPassage->RenderSelectMark();
		if(m_bDrawExtendLine)
			pPassage->RenderLineEx();
		++i;
	}
	return S_OK;
}

HRESULT KG3DSerpentine::AddOneNode(KG3DRepresentObjectNode* pNode,D3DXVECTOR3& vPos)
{
	HRESULT hr = E_FAIL;
	KGLOG_PROCESS_ERROR(pNode);
	{			
		KG3DPassage* pPassage = NULL;

		BOOL bFind = IsPositionInside(&pPassage,vPos.x,vPos.z);
		if(bFind)
		{
			BOOL IsInsert = FALSE;
			list<KG3DRepresentObjectNode*>::iterator iNode = m_listNode.begin();
			while(iNode != m_listNode.end())
			{
				if(*iNode == pPassage->m_pNodeB)
				{
					m_listNode.insert(iNode,pNode);
					IsInsert = TRUE;
					DeletePassage(pPassage);
					break;
				}
				IsInsert = FALSE;
				++iNode;
			}
			if(!IsInsert)
			{
				m_listNode.push_back(pNode);
			}
		}
		else
		{
			m_listNode.push_back(pNode);
		}
	}
	pNode->AddRef();
	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DSerpentine::DeleteNode(KG3DRepresentObjectNode* pNode)
{
	set<KG3DPassage*> setPassage;

	FindPassage(&setPassage,pNode);
	set<KG3DPassage*> ::iterator i= setPassage.begin();
	while(i != setPassage.end())
	{
		DeletePassage(*i);
		++i;
	}
	m_listNode.remove(pNode);
	SAFE_RELEASE(pNode);
	return S_OK;	
}

HRESULT KG3DSerpentine::DeleteAllNode()
{
	list<KG3DRepresentObjectNode*>::iterator i = m_listNode.begin();
	while (i!=m_listNode.end())
	{
		KG3DRepresentObjectNode* pNode = *i;
		SAFE_RELEASE(pNode);
		++i;
	}
	m_listNode.clear();
	return S_OK;
}
HRESULT KG3DSerpentine::AddOnePassage(KG3DPassage** ppPassage,int nPos)
{
	/*HRESULT hr = E_FAIL;
	list<KG3DPassage*>::iterator iPos = m_listPassage.begin();
	KG3DPassage* pPassage  = new KG3DPassage();
	KGLOG_PROCESS_ERROR(pPassage);

	(*ppPassage) = pPassage;

	for(int i=0 ;i< nPos; i++)
		iPos++;
	m_listPassage.insert(iPos,pPassage);
	hr = S_OK;
Exit0:*/
	_ASSERT(0);
	return S_OK;
}
HRESULT KG3DSerpentine::AddOnePassage(KG3DPassage** ppPassage)
{
//	HRESULT hr = E_FAIL;
//	KG3DPassage* pPassage  = new KG3DPassage();
//	KGLOG_PROCESS_ERROR(pPassage);
//
//	(*ppPassage) = pPassage;
//	m_listPassage.push_back(pPassage);
//	hr = S_OK;
//Exit0:
	_ASSERT(0);
	return S_OK;
}

HRESULT KG3DSerpentine::DeletePassage(KG3DPassage* pPassage)
{
	m_listPassage.remove(pPassage);
	SAFE_DELETE(pPassage);
	return S_OK;
}

HRESULT KG3DSerpentine::DeleteAllPassage()
{
	list<KG3DPassage*>::iterator i = m_listPassage.begin();
	while (i!=m_listPassage.end())
	{
		KG3DPassage* pPassage = *i;
		SAFE_DELETE(pPassage);
		++i;
	}
	m_listPassage.clear();
	return S_OK;
}
BOOL KG3DSerpentine::GetHeightAndUV(float& fHeight,float& fLeftDis,float& fLength,float fX,float fZ)
{
	list<KG3DPassage*> listPassageIntersected;
	{
		list<KG3DPassage*>::iterator i = m_listPassage.begin();
		list<KG3DPassage*>::iterator iend = m_listPassage.end();
		while (i!=iend)
		{
			KG3DPassage* pPassage = *i;

			if(pPassage->m_Bbox.IsPointInside2D(fX,fZ))
			{
				listPassageIntersected.push_back(pPassage);
			}
			++i;
		}
	}
	//////////////////////////////////////////////////////////////////////////

	{
		list<KG3DPassage*>::iterator i = listPassageIntersected.begin();
		list<KG3DPassage*>::iterator iend = listPassageIntersected.end();
		while (i!=iend)
		{
			KG3DPassage* pPassage = *i;

			for (size_t j=0;j<pPassage->m_vecTinyLine.size();j++)
			{
				KG3DPassage::TinyLine& Line = pPassage->m_vecTinyLine[j];

				if(Line.IsPointInLine(fX,fZ))
				{

					D3DXVECTOR2 VA;
					VA.x = fX - Line.vPositionA[0].x;
					VA.y = fZ - Line.vPositionA[0].z;
					float dotL = D3DXVec2Dot(&Line.vBANormal2D,&VA);//弯曲的时候，其实接近一个平行四边形

					fLength = dotL + Line.fLengthStart;
					D3DXVECTOR3 vPos = Line.vPositionA[0] + dotL * Line.vBANormal;

					float dotLeft =D3DXVec2Dot(&VA,&Line.vLeft2D);
					fLeftDis = dotLeft;// fabs(dotLeft);
					fHeight = vPos.y;
					return TRUE;	
				}
			}
			++i;
		}
	}

	return FALSE;
}

BOOL KG3DSerpentine::IsPositionInside(KG3DPassage** ppOutPassage,float fX,float fZ)//IsPositionInSerpentine
{
	list<KG3DPassage*> listPassageIntersected;
	{
		list<KG3DPassage*>::iterator i = m_listPassage.begin();
		list<KG3DPassage*>::iterator iend = m_listPassage.end();
		while (i!=iend)
		{
			KG3DPassage* pPassage = *i;

			if(pPassage->m_Bbox.IsPointInside2D(fX,fZ))
			{
				listPassageIntersected.push_back(pPassage);
			}
			++i;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DPassage*>::iterator i = listPassageIntersected.begin();
		list<KG3DPassage*>::iterator iend = listPassageIntersected.end();
		while (i!=iend)
		{
			KG3DPassage* pPassage = *i;

			for (size_t j=0;j<pPassage->m_vecTinyLine.size();j++)
			{
				KG3DPassage::TinyLine& Line = pPassage->m_vecTinyLine[j];

				if(Line.IsPointInLine(fX,fZ))
				{
					*ppOutPassage = pPassage;
					return TRUE;	
				}
			}
			++i;
		}
	}

	return FALSE;
}

BOOL KG3DSerpentine::IsPositionInsideExtend(KG3DPassage** ppOutPassage,float fX,float fZ)//IsPositionInside
{
	list<KG3DPassage*> listPassageIntersected;
	{
		list<KG3DPassage*>::iterator i = m_listPassage.begin();
		list<KG3DPassage*>::iterator iend = m_listPassage.end();
		while (i!=iend)
		{
			KG3DPassage* pPassage = *i;

			if(pPassage->m_ExtendBBox.IsPointInside2D(fX,fZ))
			{
				listPassageIntersected.push_back(pPassage);
			}
			++i;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		list<KG3DPassage*>::iterator i = listPassageIntersected.begin();
		list<KG3DPassage*>::iterator iend = listPassageIntersected.end();
		while (i!=iend)
		{
			KG3DPassage* pPassage = *i;

			for (size_t j=0;j<pPassage->m_vecTinyLine.size();j++)
			{
				KG3DPassage::TinyLine& Line = pPassage->m_vecTinyLine[j];

				if(Line.IsPointInside(fX,fZ))
				{
					*ppOutPassage = pPassage;
					return TRUE;	
				}
			}
			++i;
		}
	}

	return FALSE;
}

BOOL KG3DSerpentine::IsTerrainCellInside(int nX,int nZ,DWORD dwOption)
{
	float cell  = 100;
	D3DXVECTOR2 point[4];
	BOOL bFind = FALSE;

	point[0].x = (nX) * cell;     point[0].y = (nZ) * cell;
	point[1].x = (nX + 1) * cell; point[1].y = (nZ) * cell;
	point[2].x = (nX + 1) * cell; point[2].y = (nZ + 1) * cell;
	point[3].x = (nX) * cell;	  point[3].y = (nZ + 1) * cell;
	KG3DPassage* pPassage = NULL;
	int sum = 0;
	if(!dwOption)
	{
		for(int i=0;i< 4;i++)
		{
			if(IsPositionInside(&pPassage,point[i].x,point[i].y))
				sum++;
		}
		if(sum == 4)
		{
			POINT p;
			p.x = nX;
			p.y = nZ;
			pPassage->m_listCell.push_back(p);
			bFind = TRUE;
		}
	}
	else
	{
		for(int i=0;i< 4;i++)
		{
			if(IsPositionInsideExtend(&pPassage,point[i].x,point[i].y))
				sum++;
		}
		if(sum == 4)
		{
			POINT p;
			p.x = nX;
			p.y = nZ;
			pPassage->m_listTerrainCell.push_back(p);
			bFind = TRUE;
		}
	}
	return bFind;
}

HRESULT KG3DSerpentine::ComputePassages()
{
	HRESULT hr = E_FAIL;
	DeleteAllPassage();
	m_BBox.Clear();
	m_ExtendBBox.Clear();
	{
		KG3DPassage* pLastPassage = NULL;
		int K = 0;
		list<KG3DRepresentObjectNode*>::iterator i = m_listNode.begin();
		list<KG3DRepresentObjectNode*>::iterator iend = m_listNode.end();
		while (i!=iend)
		{
			KG3DRepresentObjectNode* pNode = *i;
			list<KG3DRepresentObjectNode*>::iterator j = i;
			++j;
			if(j!=iend)
			{
				KG3DRepresentObjectNode* pNodeNext = *j;

				{
					KG3DPassage* pnewPassage = NULL;
					hr = AddOnePassage(&pnewPassage);
					if(SUCCEEDED(hr))
					{
						pnewPassage->m_pNodeA = pNode;
						pnewPassage->m_pNodeB = pNodeNext;
						pNode->GetTranslation(&(pnewPassage->m_vPositionA));//pnewPassage->m_vPositionA = pNode->m_vPosition;
						pNodeNext->GetTranslation(&(pnewPassage->m_vPositionB));//pnewPassage->m_vPositionB = pNodeNext->m_vPosition;
						pnewPassage->m_fABDistance = D3DXVec3Length(&(pnewPassage->m_vPositionB - pnewPassage->m_vPositionA));

						pnewPassage->m_vNormalA = pnewPassage->m_vPositionA - pnewPassage->m_vPositionB;
						pnewPassage->m_vNormalB = pnewPassage->m_vPositionB - pnewPassage->m_vPositionA;
						pnewPassage->m_vNormalA.y = 0;
						pnewPassage->m_vNormalB.y = 0;
						D3DXVec3Normalize(&pnewPassage->m_vNormalA,&pnewPassage->m_vNormalA);
						D3DXVec3Normalize(&pnewPassage->m_vNormalB,&pnewPassage->m_vNormalB);

						if(pLastPassage)
						{
							D3DXVECTOR3 normal = (pLastPassage->m_vNormalB - pnewPassage->m_vNormalA) * 0.5;
							D3DXVec3Normalize(&normal,&normal);

							pLastPassage->m_vNormalB = normal;
							pnewPassage->m_vNormalA = -normal;
						}
						pLastPassage = pnewPassage;
					}
				}				
			}

			++i;
			K++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	{
		float fLengthSum = 0;
		list<KG3DPassage*>::iterator iPre;
		list<KG3DPassage*>::iterator i = m_listPassage.begin();
		list<KG3DPassage*>::iterator iend = m_listPassage.end();
		while (i!=iend)
		{
			KG3DPassage* pPassage = *i;
			iPre = i;
			{
				D3DXVECTOR3 left;
				D3DXVec3Cross(&left,&pPassage->m_vNormalA,&D3DXVECTOR3(0,1,0));
				left.y = 0;
				D3DXVec3Normalize(&left,&left);

				pPassage->m_vBorderPosA[0] = pPassage->m_vPositionA - left * m_fWidth * 0.5F;
				pPassage->m_vBorderPosA[1] = pPassage->m_vPositionA + left * m_fWidth * 0.5F;

				///////////////////////////////////
				pPassage->m_vExtendPosA[0] = pPassage->m_vPositionA - left * (m_fWidth * 0.5F + m_fExtendWidth);
				pPassage->m_vExtendPosA[1] = pPassage->m_vPositionA + left * (m_fWidth * 0.5F + m_fExtendWidth);

				///////////////////////////////////
			}
			{
				D3DXVECTOR3 left;
				D3DXVec3Cross(&left,&pPassage->m_vNormalB,&D3DXVECTOR3(0,1,0));
				left.y = 0;
				D3DXVec3Normalize(&left,&left);

				pPassage->m_vBorderPosB[0] = pPassage->m_vPositionB + left * m_fWidth * 0.5F;
				pPassage->m_vBorderPosB[1] = pPassage->m_vPositionB - left * m_fWidth * 0.5F;

				///////////////////////////////////
				pPassage->m_vExtendPosB[0] = pPassage->m_vPositionB + left * (m_fWidth * 0.5F + m_fExtendWidth);
				pPassage->m_vExtendPosB[1] = pPassage->m_vPositionB - left * (m_fWidth * 0.5F + m_fExtendWidth);

				///////////////////////////////////
			}
			//pPassage->m_fSerpentineLength[0] = D3DXVec3Length(&(pPassage->m_vSerpentinePositionB[0] - pPassage->m_vSerpentinePositionA[0]));
			//pPassage->m_fSerpentineLength[1] = D3DXVec3Length(&(pPassage->m_vSerpentinePositionB[1] - pPassage->m_vSerpentinePositionA[1]));

			pPassage->m_Bbox.Clear();
			pPassage->m_Bbox.AddPosition(pPassage->m_vBorderPosA,2);
			pPassage->m_Bbox.AddPosition(pPassage->m_vBorderPosB,2);

			//////////////////////////////////////
			pPassage->m_ExtendBBox.Clear();
			pPassage->m_ExtendBBox.AddPosition(pPassage->m_vExtendPosA,2);
			pPassage->m_ExtendBBox.AddPosition(pPassage->m_vExtendPosB,2);
			/////////////////////////////////////
			//float L = D3DXVec3Length(&(pPassage->m_vPositionB - pPassage->m_vPositionA));
			int num =(int) pPassage->m_fABDistance /m_nSegmentLength; 
			if(num < 2)
				num = 2;
			//num++;
			D3DXVECTOR3* pVertex = new D3DXVECTOR3[num];
			//int Index = 1;
			pVertex[0] = pPassage->m_vPositionA;
			//for (float S=0.1F;S<1.0F;S+=0.1F,Index++)
			float K = m_fBendModulus;//BEND_MODULUS;

			for (int j = 1;j<num; j++)
			{

				float S = (float)j / (float)num ;

				D3DXVec3Hermite(&pVertex[j],
					&pPassage->m_vPositionA,
					&(-pPassage->m_vNormalA*K*pPassage->m_fABDistance) ,
					&pPassage->m_vPositionB,
					&(pPassage->m_vNormalB*K*pPassage->m_fABDistance),
					S);
			}
			pVertex[num - 1] = pPassage->m_vPositionB;
			pPassage->m_Bbox.AddPosition(pVertex,num);

			float SaveLength = fLengthSum;
			pPassage->CreateTinyLines(pVertex,num,fLengthSum,m_fWidth,m_fExtendWidth);
			pPassage->m_fTotalLength = fLengthSum - SaveLength;
			m_BBox.AddPosition(pPassage->m_Bbox.A);
			m_BBox.AddPosition(pPassage->m_Bbox.B);

			m_ExtendBBox.AddPosition(pPassage->m_ExtendBBox.A);
			m_ExtendBBox.AddPosition(pPassage->m_ExtendBBox.B);
			//pPassage->m_fLengthSum = fLengthSum;
			//fLengthSum += pPassage->m_fLength;
			SAFE_DELETE_ARRAY(pVertex);
			++i;
		}

		m_fLength = fLengthSum;//计算整条道路的总长度
	}


	return S_OK;
}


HRESULT KG3DSerpentine::InputTerrainCell()
{
	int cell = 100;
	int nXStartEx = (int)(m_ExtendBBox.A.x/cell);
	int nXEndEx  = (int)(m_ExtendBBox.B.x/cell);
	int nZStartEx = (int)(m_ExtendBBox.A.z/cell);
	int nZEndEx   = (int)(m_ExtendBBox.B.z/cell);
	nXStartEx = max(0,nXStartEx);
	nZStartEx = max(0,nZStartEx);
	for (int z=nZStartEx;z<nZEndEx;z++)
	{
		for (int x=nXStartEx;x<nXEndEx;x++)
		{
			if(!IsTerrainCellInside(x,z,0))
				IsTerrainCellInside(x,z,1);	
		}
	}

	return S_OK;
}
void KG3DSerpentine::SetWidth(float fWidth)
{
	m_fWidth = fWidth;
	ComputePassages();
}

float KG3DSerpentine::GetWidth()
{
	return m_fWidth;
}

void KG3DSerpentine::SetDrawSelectMark(BOOL bVal)
{
	m_bDrawSelectMark = bVal;
}

BOOL KG3DSerpentine::GetDrawSelectMark()
{
	return m_bDrawSelectMark;
}
void KG3DSerpentine::SetDrawExtendLine(BOOL bVal)
{
	m_bDrawExtendLine = bVal;
}

BOOL KG3DSerpentine::GetDrawExtendLine()
{
	return m_bDrawExtendLine;
}
float KG3DSerpentine::GetNodeSize()
{
	return m_fNodeSize;
}
HRESULT KG3DSerpentine::SetNodeSize(float nSize)
{
	m_fNodeSize = nSize;
	list<KG3DRepresentObjectNode*>::iterator i = m_listNode.begin();
	while (i != m_listNode.end())
	{
		KG3DRepresentObjectNode* pNode = *i;
		if(pNode)
		{
			pNode->SetScaling(&D3DXVECTOR3(m_fNodeSize,m_fNodeSize,m_fNodeSize));
		}
		++i;
	}
	return S_OK;
}
void KG3DSerpentine::SetSegmentLength(int nLength)
{
	m_nSegmentLength = nLength;
}

int KG3DSerpentine::GetSegmentLength()
{
	return m_nSegmentLength;
}

HRESULT KG3DSerpentine::SaveToFile(const char cszFileName[])
{
	_ASSERT(0);
	return S_OK;
}

HRESULT KG3DSerpentine::LoadFromFile(const char cszFileName[])
{
	_ASSERT(0);
	return S_OK;
}

HRESULT KG3DSerpentine::LoadNode(DWORD dwNumOfNode,D3DXVECTOR3* pNodePos)
{
	_ASSERT(0);
	return S_OK;
}
HRESULT KG3DSerpentine::Create()
{
	InputTerrainCell();
	{
		list<KG3DPassage*>::iterator i = m_listPassage.begin();
		list<KG3DPassage*>::iterator iend = m_listPassage.end();
		while (i!=iend)
		{
			KG3DPassage* pPassage = *i;
			pPassage->CreateBuffers();
			pPassage->UpdateVertex(this);
			++i;
		}
	}
	return S_OK;
}
HRESULT KG3DSerpentine::RefreshHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BOOL bToTerrain)
{
	_ASSERT(0);
	return S_OK;
}

ULONG KG3DSerpentine::AddRef()
{
	return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG KG3DSerpentine::Release()
{
	ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
	if (uNewRefCount == 0)
		OnReferenceZero(); 
	return uNewRefCount;
}
HRESULT KG3DSerpentine::OnReferenceZero()
{
	UnInit();
	delete this;
	return S_OK;
}
////////////////////////////////////////////////////////////////////////////////////////////////////
/////KG3DPassage

KG3DPassage::KG3DPassage()
{
	m_lpIB = NULL;
	m_lpVB = NULL;
	m_pNodeA = NULL;
	m_pNodeB = NULL;

	m_bIsVisable = TRUE;
	//m_dwFrameCount = 0;

	m_fTotalLength = 0; 
	m_fABDistance = 0;
	
	m_uNumVertex = 0;
	m_uNumFaces = 0;
}

KG3DPassage::~KG3DPassage()
{
	UnInit();
}
void KG3DPassage::UnInit()
{
	m_uNumVertex = 0;
	m_uNumFaces = 0;
	m_fTotalLength = 0; 
	m_fABDistance = 0;

	m_pNodeA = NULL;
	m_pNodeB = NULL;

	SAFE_RELEASE(m_lpIB);
	SAFE_RELEASE(m_lpVB);

	m_listTerrainCell.clear();
	m_listCell.clear();
	m_vecPoint.clear();
	m_vecTerrainPoint.clear();
	m_vecTinyLine.clear();
	m_Bbox.Clear();
}
BOOL KG3DPassage::GetVertexPoint(WORD& OutI, int nX,int nZ)
{
	for (WORD i =0;i<(WORD)m_vecPoint.size();i++)
	{
		POINT& p = m_vecPoint[i];
		if ((p.x == nX)&&(p.y == nZ))
		{
			OutI = i;
			return TRUE;
		}
	}
	return FALSE;
}
WORD KG3DPassage::FindVertexPoint(int nX,int nZ)
{
	for (WORD i =0;i<(WORD)m_vecPoint.size();i++)
	{
		POINT& p = m_vecPoint[i];
		if ((p.x == nX)&&(p.y == nZ))
		{
			return i;
		}
	}
	POINT p;
	p.x = nX;
	p.y = nZ;
	m_vecPoint.push_back(p);
	return (WORD)m_vecPoint.size() -1;
}

WORD KG3DPassage::FindVertexTerrainPoint(int nX,int nZ)
{
	for (WORD i =0;i<(WORD)m_vecTerrainPoint.size();i++)
	{
		POINT& p = m_vecTerrainPoint[i];
		if ((p.x == nX)&&(p.y == nZ))
		{
			return i;
		}
	}
	POINT p;
	p.x = nX;
	p.y = nZ;
	m_vecTerrainPoint.push_back(p);
	return (WORD)m_vecTerrainPoint.size() -1;
}

HRESULT KG3DPassage::CreateTinyLines(D3DXVECTOR3* pV,const int numVertex,float& fLengthStart,float fSerpentineWidth,float fExtendWidth)
{
	m_vecTinyLine.clear();

	float fWidth = fSerpentineWidth * 0.5f;
	for (int i=1;i<numVertex;i++)
	{
		TinyLine Line;
		Line.vPositionA[0] = pV[i-1];
		Line.vPositionB[0] = pV[i];

		D3DXVECTOR3 V = Line.vPositionB[0] - Line.vPositionA[0];
		D3DXVec3Cross(&Line.vLeft,&V,&D3DXVECTOR3(0,1,0));

		Line.vLeft2D.x = Line.vLeft.x;
		Line.vLeft2D.y = Line.vLeft.z;
		D3DXVec2Normalize(&Line.vLeft2D,&Line.vLeft2D);
		D3DXVec3Normalize(&Line.vLeft,&Line.vLeft);
		if(i == 1)
		{
			Line.vPositionA[1] = m_vBorderPosA[0];
			Line.vPositionA[2] = m_vBorderPosA[1];
			/////////////////////////////
			Line.vExtendPosA[0] = m_vExtendPosA[0];
			Line.vExtendPosA[1] = m_vExtendPosA[1];
		}
		else
		{
			Line.vPositionA[1] = Line.vPositionA[0] + Line.vLeft * fWidth;
			Line.vPositionA[2] = Line.vPositionA[0] - Line.vLeft * fWidth;

			Line.vExtendPosA[0] = Line.vPositionA[0] + Line.vLeft * (fWidth + fExtendWidth);
			Line.vExtendPosA[1] = Line.vPositionA[0] - Line.vLeft * (fWidth + fExtendWidth);

		}
		Line.vExtendPosB[0] = Line.vPositionB[0] + Line.vLeft * (fWidth + fExtendWidth);
		Line.vExtendPosB[1] = Line.vPositionB[0] - Line.vLeft * (fWidth + fExtendWidth);

		Line.vPositionB[1] = Line.vPositionB[0] + Line.vLeft * fWidth;
		Line.vPositionB[2] = Line.vPositionB[0] - Line.vLeft * fWidth;

		//////////////////////////////////////////////////////
		Line.bBox.AddPosition(Line.vPositionA,3);
		Line.bBox.AddPosition(Line.vPositionB,3);
		this->m_Bbox.AddPosition(Line.bBox.A); 
		this->m_Bbox.AddPosition(Line.bBox.B); 

		Line.ExtendBBox.AddPosition(Line.vExtendPosA,2);
		Line.ExtendBBox.AddPosition(Line.vExtendPosB,2);
		this->m_ExtendBBox.AddPosition(Line.ExtendBBox.A); 
		this->m_ExtendBBox.AddPosition(Line.ExtendBBox.B); 
		//////////////////////////////////////////////////////
		Line.fLength = D3DXVec3Length(&V);
		D3DXVec3Normalize(&Line.vBANormal,&V);

		V.y = 0;
		Line.fLength2D = D3DXVec3Length(&V);

		Line.vBANormal2D.x = V.x;
		Line.vBANormal2D.y = V.z;
		D3DXVec2Normalize(&Line.vBANormal2D,&Line.vBANormal2D);


		Line.fLengthStart = fLengthStart;
		fLengthStart += Line.fLength;

		m_vecTinyLine.push_back(Line);
	}
	int numLine = (int)m_vecTinyLine.size();
	for (int i=0 ; i < numLine ;i++)
	{
		if(i > 0)
		{
			m_vecTinyLine[i].vPositionA[1]  = m_vecTinyLine[i - 1].vPositionB[1];
			m_vecTinyLine[i].vPositionA[2]  = m_vecTinyLine[i - 1].vPositionB[2];

			m_vecTinyLine[i].vExtendPosA[0]  = m_vecTinyLine[i - 1].vExtendPosB[0];
			m_vecTinyLine[i].vExtendPosA[1]  = m_vecTinyLine[i - 1].vExtendPosB[1];
		}
		if( i < numLine - 1)
		{
			m_vecTinyLine[i].vPositionB[1]  = (m_vecTinyLine[i].vPositionB[1] + m_vecTinyLine[i+1].vPositionA[1]) * 0.5f;
			m_vecTinyLine[i].vPositionB[2]  = (m_vecTinyLine[i].vPositionB[2] + m_vecTinyLine[i+1].vPositionA[2]) * 0.5f;

			m_vecTinyLine[i].vExtendPosB[0]  = (m_vecTinyLine[i].vExtendPosB[0] + m_vecTinyLine[i+1].vExtendPosA[0]) * 0.5f;
			m_vecTinyLine[i].vExtendPosB[1]  = (m_vecTinyLine[i].vExtendPosB[1] + m_vecTinyLine[i+1].vExtendPosA[1]) * 0.5f;
		}
		if( i == numLine - 1)
		{
			m_vecTinyLine[i].vPositionB[1]  =m_vBorderPosB[0];
			m_vecTinyLine[i].vPositionB[2]  = m_vBorderPosB[1];

			m_vecTinyLine[i].vExtendPosB[0]  = m_vExtendPosB[0];
			m_vecTinyLine[i].vExtendPosB[1]  = m_vExtendPosB[1];
		}
	}
	return S_OK;
}

BOOL KG3DPassage::TinyLine::IsPointInLine(float fX,float fZ)
{
	const int VECTORNUM = 4 ;
	static D3DXVECTOR2 p[VECTORNUM + 1];
	size_t n = VECTORNUM;
	size_t i;
	int sum;

	//把坐标轴原点移到测试点的位置
	p[0].x = vPositionA[1].x - fX; p[0].y = vPositionA[1].z - fZ;
	p[1].x = vPositionA[2].x - fX; p[1].y = vPositionA[2].z - fZ;
	p[2].x = vPositionB[2].x - fX; p[2].y = vPositionB[2].z - fZ;
	p[3].x = vPositionB[1].x - fX; p[3].y = vPositionB[1].z - fZ;
	p[4] = p[0];
	BOOL bIn = FALSE;
	for (sum = 0,i =0 ; i < n; i++)
	{
		if((abs(p[i].x)<=0.00001F)&&(abs(p[i].y)<=0.00001F))
		{
			bIn = TRUE;
			break;
		}

		if( (int)p[i].x == 0 && (int)p[i].y == 0) return TRUE; //在顶点上
		else if (p[i].y - p[i+1].y != 0 && p[i].y * p[i+1].y < 0) //顶点不在X轴上，且顶点与下一点顶不在X轴同一倾
		{
			float X = p[i].y *(p[i+1].x - p[i].x) / (p[i].y-p[i+1].y) +p[i].x;//计算两顶点所成的线与X轴的交点
			if (X >= 0 && X > min(p[i].x,p[i+1].x) && X < max(p[i].x,p[i+1].x))//交点在X轴正方向上，且在两顶点所成的线段上，计数加1
			{
				sum++;
			}
		}	
	}
	if(bIn)
		return TRUE;
	if((sum%2) == 1)//测试点发出的射线与多边形的边的交点为单数则该点在多边形里
		return TRUE;

	return FALSE;
}

BOOL KG3DPassage::TinyLine::IsPointInside(float fX,float fZ)
{
	const int VECTORNUM = 4 ;
	static D3DXVECTOR2 p[VECTORNUM + 1];
	size_t n = VECTORNUM;
	size_t i;
	int sum;

	//把坐标轴原点移到测试点的位置
	p[0].x = vExtendPosA[0].x - fX; p[0].y = vExtendPosA[0].z - fZ;
	p[1].x = vExtendPosA[1].x - fX; p[1].y = vExtendPosA[1].z - fZ;
	p[2].x = vExtendPosB[1].x - fX; p[2].y = vExtendPosB[1].z - fZ;
	p[3].x = vExtendPosB[0].x - fX; p[3].y = vExtendPosB[0].z - fZ;
	p[4] = p[0];
	BOOL bIn = FALSE;
	for (sum = 0,i =0 ; i < n; i++)
	{
		if((abs(p[i].x)<=0.00001F)&&(abs(p[i].y)<=0.00001F))
		{
			bIn = TRUE;
			break;
		}

		if( (int)p[i].x == 0 && (int)p[i].y == 0) return TRUE; //在顶点上
		else if (p[i].y - p[i+1].y != 0 && p[i].y * p[i+1].y < 0) //顶点不在X轴上，且顶点与下一点顶不在X轴同一倾
		{
			float X = p[i].y *(p[i+1].x - p[i].x) / (p[i].y-p[i+1].y) +p[i].x;//计算两顶点所成的线与X轴的交点
			if (X >= 0 && X > min(p[i].x,p[i+1].x) && X < max(p[i].x,p[i+1].x))//交点在X轴正方向上，且在两顶点所成的线段上，计数加1
			{
				sum++;
			}
		}	
	}
	if(bIn)
		return TRUE;
	if((sum%2) == 1)//测试点发出的射线与多边形的边的交点为单数则该点在多边形里
		return TRUE;

	return FALSE;
}

HRESULT KG3DPassage::RenderTinyLines()
{
	DWORD color;
	for (size_t i=0;i<m_vecTinyLine.size();i++)
	{
		TinyLine& line = m_vecTinyLine[i];
		/*if(i%2 == 0)
		color = 0xFFFFFF00;
		else*/
		color = 0xFF0058FF;

		g_cGraphicsTool.DrawLine(&(line.vPositionA[0] + D3DXVECTOR3(0,10,0)) ,&(line.vPositionB[0] + D3DXVECTOR3(0,10,0)),color,color);
	}
	color = 0xFF04F8FF;
	g_cGraphicsTool.DrawLine(&(m_vPositionA + D3DXVECTOR3(0,10,0)) ,&((m_vPositionA + D3DXVECTOR3(0,10,0))+ m_vNormalB * 500),color,color);

	color = 0xFFFF0000;
	g_cGraphicsTool.DrawLine(&(m_vPositionA + D3DXVECTOR3(0,10,0)) ,&((m_vPositionA + D3DXVECTOR3(0,10,0))+ m_vNormalA * 500),color,color);
	return S_OK;
}
HRESULT KG3DPassage::RenderSelectMark()
{
	DWORD color;
	for (size_t i=0;i<m_vecTinyLine.size();i++)
	{
		TinyLine& line = m_vecTinyLine[i];

		color = 0xFFF0F852;

		g_cGraphicsTool.DrawLine(&(line.vPositionA[1] + D3DXVECTOR3(0,10,0)) ,&(line.vPositionB[1] + D3DXVECTOR3(0,10,0)),color,color);
		g_cGraphicsTool.DrawLine(&(line.vPositionA[2] + D3DXVECTOR3(0,10,0)) ,&(line.vPositionB[2] + D3DXVECTOR3(0,10,0)),color,color);
		g_cGraphicsTool.DrawLine(&(line.vPositionA[1] + D3DXVECTOR3(0,10,0)) ,&(line.vPositionA[2] + D3DXVECTOR3(0,10,0)),color,color);
		g_cGraphicsTool.DrawLine(&(line.vPositionB[1] + D3DXVECTOR3(0,10,0)) ,&(line.vPositionB[2] + D3DXVECTOR3(0,10,0)),color,color);

	}

	return S_OK;
}
HRESULT KG3DPassage::RenderLineEx()
{
	DWORD color;
	for (size_t i=0;i<m_vecTinyLine.size();i++)
	{
		TinyLine& line = m_vecTinyLine[i];

		color = 0xFF00F800;
		g_cGraphicsTool.DrawLine(&(line.vExtendPosA[0] + D3DXVECTOR3(0,5,0)) ,&(line.vExtendPosB[0] + D3DXVECTOR3(0,5,0)),color,color);
		g_cGraphicsTool.DrawLine(&(line.vExtendPosA[1] + D3DXVECTOR3(0,5,0)) ,&(line.vExtendPosB[1] + D3DXVECTOR3(0,5,0)),color,color);
		if(i == 0)
		{
			g_cGraphicsTool.DrawLine(&(line.vExtendPosA[0] + D3DXVECTOR3(0,5,0)) ,&(line.vExtendPosA[1] + D3DXVECTOR3(0,5,0)),color,color);

		}
		else if(i == m_vecTinyLine.size() -1)
		{
			g_cGraphicsTool.DrawLine(&(line.vExtendPosB[0] + D3DXVECTOR3(0,5,0)) ,&(line.vExtendPosB[1] + D3DXVECTOR3(0,5,0)),color,color);
		}
	}
	return S_OK;
}
HRESULT KG3DPassage::UpdateVertex(KG3DSerpentine* pSerpentin)
{
	return S_OK;
}

BOOL KG3DPassage::GetHeightAndUV(float& fHeight,float& fLeftDis,float& fLength,float fX,float fZ)
{
	for (size_t j=0;j<m_vecTinyLine.size();j++)
	{
		TinyLine& Line = m_vecTinyLine[j];

		if(Line.IsPointInLine(fX,fZ))
		{
			D3DXVECTOR2 VA;
			VA.x = fX - Line.vPositionA[0].x;
			VA.y = fZ - Line.vPositionA[0].z;
			float dotL = D3DXVec2Dot(&Line.vBANormal2D,&VA);//弯曲的时候，其实接近一个平行四边形

			fLength = dotL + Line.fLengthStart;
			D3DXVECTOR3 vPos = Line.vPositionA[0] + dotL * Line.vBANormal;

			float dotLeft =D3DXVec2Dot(&VA,&Line.vLeft2D);
			fLeftDis = dotLeft;// fabs(dotLeft);
			fHeight = vPos.y;
			return TRUE;	
		}
	}
	return FALSE;
}

HRESULT KG3DPassage::CreateBuffers()
{
	//GenerateWaterVertex();
	HRESULT hr = E_FAIL;
	SAFE_RELEASE(m_lpIB);
	SAFE_RELEASE(m_lpVB);

	WORD* pIndex = NULL;
	DWORD dwSize = (DWORD)m_listCell.size() * 6;

	hr = g_pd3dDevice->CreateIndexBuffer(sizeof(WORD) * dwSize, 
		0, 
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_lpIB,
		0);
	if(FAILED(hr))
		return E_FAIL;

	hr = m_lpIB->Lock(0,0,(void**)&pIndex,0);
	KG_COM_PROCESS_ERROR(hr);

	//////////////////////////////////////////////////////////////////////////
	{
		m_vecTerrainPoint.clear();
		list<POINT>::iterator it = m_listTerrainCell.begin();
		list<POINT>::iterator itend = m_listTerrainCell.end();
		while (it!=itend)
		{
			POINT& cell = *it;

			FindVertexTerrainPoint(cell.x,cell.y);
			FindVertexTerrainPoint(cell.x+1,cell.y);
			FindVertexTerrainPoint(cell.x+1,cell.y+1);
			FindVertexTerrainPoint(cell.x,cell.y+1);

			++it;
		}
		int K = 0;
		m_vecPoint.clear();
		list<POINT>::iterator i = m_listCell.begin();
		list<POINT>::iterator iend = m_listCell.end();
		while (i!=iend)
		{
			POINT& cell = *i;

			WORD A = FindVertexPoint(cell.x,cell.y);
			WORD B = FindVertexPoint(cell.x+1,cell.y);
			WORD C = FindVertexPoint(cell.x+1,cell.y+1);
			WORD D = FindVertexPoint(cell.x,cell.y+1);

			pIndex[6*K  ] = A;	   pIndex[6*K+3] = B/*A*/;
			pIndex[6*K+1] = B/*C*/;pIndex[6*K+4] = C/*D*/;
			pIndex[6*K+2] = D/*B*/;pIndex[6*K+5] = D/*C*/;			

			++i;
			K++;
		}
		hr = g_pd3dDevice->CreateVertexBuffer( sizeof( VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1) * (UINT) m_vecPoint.size(),
			0 , D3DFVF_FONT3DVERTEX,
			D3DPOOL_MANAGED, &m_lpVB,NULL );
		KG_COM_PROCESS_ERROR(hr);

		m_uNumVertex = (UINT)m_vecPoint.size();
		m_uNumFaces = (UINT)m_listCell.size()*2;
	}
	//////////////////////////////////////////////////////////////////////////

	hr = m_lpIB->Unlock();
	KG_COM_PROCESS_ERROR(hr);

	hr = S_OK;
	return hr;
Exit0:
	m_lpIB->Unlock();
	return hr;
}
HRESULT KG3DPassage::Render()
{
	if(m_lpIB && m_lpVB)
	{
		g_pd3dDevice->SetStreamSource( 0, m_lpVB,0, sizeof( VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1 ) );
		g_pd3dDevice->SetIndices(m_lpIB);
		g_pd3dDevice->SetFVF( D3DFVF_FONT3DVERTEX );
		g_pd3dDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST , 0 ,0,m_uNumVertex,0,m_uNumFaces);
	}
	return S_OK;
}

BOOL KG3DPassage::IsInside(TinyLine** pLine, float fX,float fZ)
{
	for (size_t j=0;j<m_vecTinyLine.size();j++)
	{
		TinyLine& Line = m_vecTinyLine[j];

		if(Line.IsPointInside(fX,fZ))
		{
			*pLine = &Line;
			return TRUE;	
		}
	}
	return FALSE;
}

HRESULT KG3DPassage::WriteDataToFile(IFile* pFile)
{
	HRESULT hr =E_FAIL;
	HRESULT RetCode = E_FAIL;
	BYTE *pVertexData = NULL;
	unsigned long uSize = 0;
	/*unsigned long uSize2Write = 0;*/
	BYTE *pIndices = NULL;
	D3DVERTEXBUFFER_DESC vbDesc;
	D3DINDEXBUFFER_DESC ibDesc;

	DWORD HeadPos , PosEnd; 
	_PassageHead  PassageHead;
	KG_PROCESS_ERROR(m_lpVB);
	KG_PROCESS_ERROR(m_lpIB);
	KG_PROCESS_ERROR(pFile);

	HeadPos = pFile->Tell();

	hr = m_lpVB->GetDesc(&vbDesc);
	KG_COM_PROCESS_ERROR(hr);
	hr = m_lpIB->GetDesc(&ibDesc);
	KG_COM_PROCESS_ERROR(hr);

	PassageHead.dwNumVertex = m_uNumVertex;
	PassageHead.dwNumFaces = m_uNumFaces;
	PassageHead.vPosA = m_vPositionA;
	PassageHead.vPosB = m_vPositionB;
	PassageHead.vNormalA = m_vNormalA;
	PassageHead.vNormalB = m_vNormalB;

	PassageHead.dwFVF = vbDesc.FVF;
	PassageHead.dwVertexBlockSize = vbDesc.Size;
	KG_PROCESS_ERROR(PassageHead.dwVertexBlockSize);
	PassageHead.dwVertexBlock = HeadPos + sizeof(_PassageHead);

	PassageHead.dwIndexBlockSize = ibDesc.Size;
	KG_PROCESS_ERROR(PassageHead.dwIndexBlockSize);
	PassageHead.dwIndexBlock  = HeadPos + sizeof(_PassageHead) + PassageHead.dwVertexBlockSize;

	uSize = pFile->Write(&PassageHead, sizeof(_PassageHead));//fwrite(&PassageHead, sizeof(_PassageHead),1,pFile);
	KG_PROCESS_ERROR(uSize == sizeof(_PassageHead));


	hr = m_lpVB->Lock(0,PassageHead.dwVertexBlockSize,(void**)&pVertexData,0);
	KG_COM_PROCESS_ERROR(hr);

	uSize = pFile->Write(pVertexData, PassageHead.dwVertexBlockSize);//fwrite(pVertexData, sizeof(BYTE),PassageHead.dwVertexBlockSize,pFile);//
	KG_PROCESS_ERROR(uSize == PassageHead.dwVertexBlockSize);

	//Save Index data
	hr = m_lpIB->Lock(0,PassageHead.dwIndexBlockSize,(void**)&pIndices,0);
	KG_COM_PROCESS_ERROR(hr);

	uSize = pFile->Write(pIndices, PassageHead.dwIndexBlockSize);//fwrite(pIndices, sizeof(BYTE),PassageHead.dwIndexBlockSize,pFile);
	KG_PROCESS_ERROR(uSize == PassageHead.dwIndexBlockSize);

	PosEnd =pFile->Tell();

	RetCode = S_OK;
Exit0:
	if(m_lpIB)
		m_lpIB->Unlock();
	if (m_lpVB)
		m_lpVB->Unlock();
	return RetCode;
}

HRESULT KG3DPassage::ReadDataFromFile(IFile* pFile,_PassageData& ReadData)
{
	HRESULT hr = E_FAIL;
	int nRetCode = false;
	unsigned long uSize = 0;
	_PassageHead PassageHead;

	uSize = pFile->Read(&PassageHead, sizeof(_PassageHead));
	KG_PROCESS_ERROR(uSize == sizeof(_PassageHead));

	KG_PROCESS_ERROR(PassageHead.dwVertexBlockSize > 0);
	KG_PROCESS_ERROR(PassageHead.dwIndexBlockSize > 0);

	ReadData.dwNumVertex = PassageHead.dwNumVertex;
	ReadData.dwNumFaces = PassageHead.dwNumFaces;
	ReadData.dwFVF = PassageHead.dwFVF;
	ReadData.dwVBSize = PassageHead.dwVertexBlockSize;
	ReadData.dwIBSize = PassageHead.dwIndexBlockSize;

	m_vPositionA = PassageHead.vPosA;
	m_vPositionB = PassageHead.vPosB;
	m_vNormalA = PassageHead.vNormalA;
	m_vNormalB = PassageHead.vNormalB;

	nRetCode = pFile->Seek(PassageHead.dwVertexBlock, SEEK_SET);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	SAFE_DELETE_ARRAY(ReadData.pVBData);
	ReadData.pVBData = new BYTE[PassageHead.dwVertexBlockSize];

	uSize = pFile->Read(ReadData.pVBData,PassageHead.dwVertexBlockSize);
	KG_PROCESS_ERROR(uSize == PassageHead.dwVertexBlockSize);

	nRetCode = pFile->Seek(PassageHead.dwIndexBlock, SEEK_SET);
	KGLOG_PROCESS_ERROR(nRetCode != -1);

	SAFE_DELETE_ARRAY(ReadData.pIBData);
	ReadData.pIBData = new BYTE[PassageHead.dwIndexBlockSize];

	uSize = pFile->Read(ReadData.pIBData,PassageHead.dwIndexBlockSize);
	KG_PROCESS_ERROR(uSize == PassageHead.dwIndexBlockSize);

	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DPassage::UpdatAlpha(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	return S_OK;
}

HRESULT KG3DPassage::CreateBuffersFromData(_PassageData& ReadData)
{
	HRESULT hr = E_FAIL;
	SAFE_RELEASE(m_lpIB);
	SAFE_RELEASE(m_lpVB);

	BYTE* pIndex = NULL;
	BYTE* pVertex = NULL;
	hr = g_pd3dDevice->CreateIndexBuffer(ReadData.dwIBSize, 0,D3DFMT_INDEX16,D3DPOOL_MANAGED,&m_lpIB,0);
	if(FAILED(hr))
		return E_FAIL;

	hr = m_lpIB->Lock(0,0,(void**)&pIndex,0);
	KG_COM_PROCESS_ERROR(hr);
	memcpy( pIndex, ReadData.pIBData, ReadData.dwIBSize);
	hr = m_lpIB->Unlock();
	KG_COM_PROCESS_ERROR(hr);
	//////////////////////////////////////////////////////////////////////////

	hr = g_pd3dDevice->CreateVertexBuffer( ReadData.dwVBSize,0 , ReadData.dwFVF,D3DPOOL_MANAGED, &m_lpVB,NULL );
	if(FAILED(hr))
		return E_FAIL;

	hr = m_lpVB->Lock(0,0,(void**)&pVertex,0);
	KG_COM_PROCESS_ERROR(hr);
	memcpy( pVertex, ReadData.pVBData, ReadData.dwVBSize);
	hr = m_lpVB->Unlock();
	KG_COM_PROCESS_ERROR(hr);
	//////////////////////////////////////////////////////////////////////////

	m_uNumVertex = (UINT)ReadData.dwNumVertex;
	m_uNumFaces = (UINT)ReadData.dwNumFaces;
	hr = S_OK;
	return hr;
Exit0:
	if (m_lpVB)
		m_lpVB->Unlock();
	if (m_lpIB)
		m_lpIB->Unlock();
	return hr;
}
HRESULT KG3DPassage::BulidPassageData(float& fLengthStart,float fWidth,float fBendModulus,int nSegmentLength,float fExtendWidth)
{
	m_fABDistance = D3DXVec3Length(&(m_vPositionB - m_vPositionA));

	D3DXVECTOR3 left;
	D3DXVec3Cross(&left,&m_vNormalA,&D3DXVECTOR3(0,1,0));
	left.y = 0;
	D3DXVec3Normalize(&left,&left);

	m_vBorderPosA[0] = m_vPositionA - left * fWidth * 0.5F;
	m_vBorderPosA[1] = m_vPositionA + left * fWidth * 0.5F;

	m_vExtendPosA[0] = m_vPositionA - left * (fWidth * 0.5F + fExtendWidth);
	m_vExtendPosA[1] = m_vPositionA + left * (fWidth * 0.5F + fExtendWidth);

	D3DXVec3Cross(&left,&m_vNormalB,&D3DXVECTOR3(0,1,0));
	left.y = 0;
	D3DXVec3Normalize(&left,&left);

	m_vBorderPosB[0] = m_vPositionB + left * fWidth * 0.5F;
	m_vBorderPosB[1] = m_vPositionB - left * fWidth * 0.5F;

	m_vExtendPosB[0] = m_vPositionB + left * (fWidth * 0.5F + fExtendWidth);
	m_vExtendPosB[1] = m_vPositionB - left * (fWidth * 0.5F + fExtendWidth);

	int num =(int)m_fABDistance /nSegmentLength; 
	if(num < 2)
		num = 2;
	//num++;
	D3DXVECTOR3* pVertex = new D3DXVECTOR3[num];
	pVertex[0] = m_vPositionA;

	for (int j = 1;j<num; j++)
	{

		float S = (float)j / (float)num ;

		D3DXVec3Hermite(&pVertex[j],
			&m_vPositionA,
			&(-m_vNormalA * fBendModulus * m_fABDistance) ,
			&m_vPositionB,
			&(m_vNormalB * fBendModulus * m_fABDistance),
			S);
	}
	pVertex[num - 1] = m_vPositionB;
	m_Bbox.AddPosition(pVertex,num);
	float SaveLength = fLengthStart;
	CreateTinyLines(pVertex,num,fLengthStart,fWidth,fExtendWidth);
	this->m_fTotalLength = fLengthStart - SaveLength;
	SAFE_DELETE_ARRAY(pVertex);
	return S_OK;
}
//河流管理器
KG3DSerpentineMgr::KG3DSerpentineMgr()
{
	m_lpCurrentSerpentine = NULL;
	m_lpOutDoorMgr= NULL;
	m_strFilePath = "";
	m_uHandle = 0;
}
KG3DSerpentineMgr::~KG3DSerpentineMgr()
{
	UnInit();
}
void KG3DSerpentineMgr::UnInit()
{
	m_lpCurrentSerpentine = NULL;
	DeleteAll();
}
DWORD KG3DSerpentineMgr::GetCount()
{
	return (DWORD)(m_listSerpentines.size());
}
UINT KG3DSerpentineMgr::GetHandle()
{
	UINT uHandle = 0;
	while (FindSerpentineByHandle(uHandle))
	{
		uHandle++;
		ASSERT(uHandle < 65535);
	}
	return uHandle;
}
KG3DSerpentine* KG3DSerpentineMgr::GetSerpentineByHandle(UINT& uHandle)
{
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	while (i != m_listSerpentines.end())
	{
		KG3DSerpentine* pSerpentine = *i;
		if(uHandle == pSerpentine->m_uHandle)
			return pSerpentine;
		++i;
	}
	return NULL;
}
KG3DSerpentine* KG3DSerpentineMgr::GetSerpentineByName(const TCHAR* strName)
{
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	while (i != m_listSerpentines.begin())
	{
		KG3DSerpentine* pSerpentine = *i;
		if(strcmpi(pSerpentine->m_scName.c_str(),strName) == 0)
			return pSerpentine;
		++i;
	}
	return NULL;
}
BOOL KG3DSerpentineMgr::FindSerpentineByName(const TCHAR* strName)
{
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	while (i != m_listSerpentines.end())
	{
		KG3DSerpentine* pSerpentine = *i;
		if(strcmpi(pSerpentine->m_scName.c_str(),strName) == 0)
			return TRUE;
		++i;
	}
	return FALSE;
}
BOOL KG3DSerpentineMgr::FindSerpentineByHandle(UINT& uHandle)
{
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	while (i != m_listSerpentines.end())
	{
		KG3DSerpentine* pSerpentine = *i;
		if(uHandle == pSerpentine->m_uHandle)
			return TRUE;
		++i;
	}
	return FALSE;
}
HRESULT KG3DSerpentineMgr::AddNewOne()
{
	KG3DSerpentine* pNewSerpentine = NULL;
	return NewOne(&pNewSerpentine);
}

HRESULT KG3DSerpentineMgr::DeleteOne(int nIndex)
{
	_ASSERT(0);
	return S_OK;
}

HRESULT KG3DSerpentineMgr::NewOne(KG3DSerpentine** ppSerpentine)
{
	_ASSERT(0);
	return S_OK;
}

HRESULT KG3DSerpentineMgr::DeleteAll()
{
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	while (i!=m_listSerpentines.end())
	{
		KG3DSerpentine* pSerpentine = *i;
		SAFE_RELEASE(pSerpentine);
		++i;
	}
	m_listSerpentines.clear();
	return S_OK;
}

HRESULT KG3DSerpentineMgr::SetCurSerpentine(int nIndex)
{
	int K = 0; 
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	while (i!=m_listSerpentines.end())
	{
		KG3DSerpentine* pSerpentine = *i;
		if(K==nIndex)
		{
			m_lpCurrentSerpentine = pSerpentine;
			return S_OK;
		}
		++i;
		K++;
	}
	return E_FAIL;
}
KG3DSerpentine* KG3DSerpentineMgr::GetCurSerpentine()
{
	return m_lpCurrentSerpentine;
}

HRESULT KG3DSerpentineMgr::Render()
{
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	list<KG3DSerpentine*>::iterator iend = m_listSerpentines.end();
	while (i!=iend)
	{
		KG3DSerpentine* pSerpentine = *i;
		pSerpentine->RenderLines();
		++i;
	}
	if (m_lpCurrentSerpentine)
		m_lpCurrentSerpentine->RenderSelectMark();

	return S_OK;
}

HRESULT KG3DSerpentineMgr::GetAllName(vector<std::string>& vecName)
{
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	while (i != m_listSerpentines.end())
	{
		KG3DSerpentine* pSerpentine = *i;
		vecName.push_back(pSerpentine->m_scName);
		++i;
	}
	return S_OK;
}
HRESULT KG3DSerpentineMgr::RefreshCurSerpentine()
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	m_lpCurrentSerpentine->ComputePassages();
	m_lpCurrentSerpentine->Create();
	//m_lpCurrentSerpentine->m_RefreshHeight(m_lpOutDoorMgr);
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DSerpentineMgr::RefreshHeight(BOOL bToTerrain)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG_PROCESS_ERROR(m_lpOutDoorMgr);
	m_lpCurrentSerpentine->RefreshHeight(m_lpOutDoorMgr,bToTerrain);
	hr = S_OK;
Exit0:
	return hr;
}

float KG3DSerpentineMgr::GetCurSerWidth()
{
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	return m_lpCurrentSerpentine->GetWidth();
Exit0:
	return 0;
}
void  KG3DSerpentineMgr::SetCurSerWidth(float fWidth)
{
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	m_lpCurrentSerpentine->SetWidth(fWidth);
Exit0:
	return ;
}

void KG3DSerpentineMgr::SetCurSerDrawSelectMark(BOOL bVal)
{
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	m_lpCurrentSerpentine->SetDrawSelectMark(bVal);
Exit0:
	return ;
}
BOOL KG3DSerpentineMgr::GetCurSerDrawSelectMark()
{
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	return m_lpCurrentSerpentine->GetDrawSelectMark();
Exit0:
	return 0;
}
void KG3DSerpentineMgr::SetCurSerDrawExtendLine(BOOL bVal)
{
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	m_lpCurrentSerpentine->SetDrawExtendLine(bVal);
Exit0:
	return ;
}
BOOL KG3DSerpentineMgr::GetCurSerDrawExtendLine()
{
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	return m_lpCurrentSerpentine->GetDrawExtendLine();
Exit0:
	return 0;
}
int KG3DSerpentineMgr::GetSerpentineIndex(KG3DSerpentine* pSerpentine)
{
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	int Index = 0;
	while (i != m_listSerpentines.end())
	{ 
		if(pSerpentine == (*i))
		{
			return Index;
		}	
		Index++;
		++i;
	}
	return -1;
}
HRESULT KG3DSerpentineMgr::SetCurSerModulusK(float fK)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	m_lpCurrentSerpentine->m_fBendModulus = fK;
	m_lpCurrentSerpentine->ComputePassages();
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DSerpentineMgr::GetCurSerModulusK(float& fK)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	fK = m_lpCurrentSerpentine->m_fBendModulus;
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DSerpentineMgr::SetCurSerName(const TCHAR* strName)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG_PROCESS_ERROR(strName);
	m_lpCurrentSerpentine->m_scName = strName;
	m_mapNames[m_lpCurrentSerpentine->m_uHandle] = m_lpCurrentSerpentine->m_scName;

	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DSerpentineMgr::GetCurSerName(std::string& strName)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);

	strName = m_lpCurrentSerpentine->m_scName;
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DSerpentineMgr::SaveToFile(LPCSTR strFileName) //保存河流信息到文件
{
	_ASSERT(0);
	return S_OK;
}

HRESULT KG3DSerpentineMgr::SaveToInformationBuffer(DWORD& dwLength,KG3DMemoryFile* pFile)
{
	pFile->Reset();
	CHAR szName[32];
	DWORD dwVersion = 0;
	pFile->write(&dwVersion,sizeof(DWORD));
	DWORD dwNum = (DWORD)m_mapNames.size();
	pFile->write(&dwNum,sizeof(DWORD));
	map<UINT,std::string>::iterator it = m_mapNames.begin();
	while(it != m_mapNames.end())
	{
		UINT uHandle = it->first;
		std::string strName = it->second;
		wsprintf(szName,"%s",strName.c_str());
		pFile->write(&uHandle,sizeof(UINT));
		pFile->write(szName,sizeof(szName));
		++it;
	}
	dwLength = pFile->GetLength();
	return S_OK;
}
HRESULT KG3DSerpentineMgr::LoadFromInformationBuffer(DWORD& dwLength,KG3DMemoryFile* pFile)
{
	m_mapNames.clear();
	pFile->Reset();
	UINT uHandle = 0;
	CHAR szName[32];
	DWORD dwVersion = 0;
	DWORD dwNum = 0;
	pFile->read(&dwVersion,sizeof(DWORD));
	pFile->read(&dwNum,sizeof(DWORD));
	for (DWORD i=0 ;i<dwNum;i++)
	{
		pFile->read(&uHandle,sizeof(UINT));
		pFile->read(&szName,sizeof(CHAR)*32);
		std::string strName = szName;
		m_mapNames.insert(std::pair<UINT,std::string>(uHandle,strName));
	}
	KGLOG_PROCESS_ERROR( dwLength == pFile->GetPos());
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSerpentineMgr::AddNodeToCurSerpentine(KG3DRepresentObjectNode* pNode,D3DXVECTOR3& vPos)
{
	//KG_PROCESS_ERROR(m_lpOutDoorMgr);
	//KG_PROCESS_ERROR(m_lpCurrentSerpentine);

	//m_lpCurrentSerpentine->AddOneNode(pNode,vPos);

	//pNode->SetPointer((PVOID)m_lpCurrentSerpentine);

	//m_lpCurrentSerpentine->ComputePassages(pNode);
	//m_lpCurrentSerpentine->CreatePassage(pNode);

	//m_lpOutDoorMgr->InputSerpentine(m_lpCurrentSerpentine);
	_ASSERT(0);
	return S_OK;
//Exit0:
//	return E_FAIL;
}
HRESULT KG3DSerpentineMgr::MoveCurSerpentinePre()
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	{
		list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
		if( *i == m_lpCurrentSerpentine)
			return S_OK;
		while (i != m_listSerpentines.end())
		{
			if(*i == m_lpCurrentSerpentine)
			{
				m_listSerpentines.insert(--i,m_lpCurrentSerpentine);
				m_listSerpentines.erase(++i);
				break;
			}
			++i;
		}
	}

	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DSerpentineMgr::MoveCurSerpentineNext()
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	{
		list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
		if( m_listSerpentines.back() == m_lpCurrentSerpentine)
			return S_OK;
		while (i != m_listSerpentines.end())
		{
			if(*i == m_lpCurrentSerpentine)
			{
				KG3DSerpentine* pNextRoad = *(++i);
				i--;
				m_listSerpentines.insert(i,pNextRoad);
				m_listSerpentines.erase(++i);
				break;
			}
			++i;
		}
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DSerpentineMgr::SetCurSerSegmentLength(int nLength)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	m_lpCurrentSerpentine->SetSegmentLength(nLength);
	m_lpCurrentSerpentine->ComputePassages();
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DSerpentineMgr::GetCurSerSegmentLength(int& nLength)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	nLength = m_lpCurrentSerpentine->GetSegmentLength();
	hr = S_OK;
Exit0:
	return hr;
}


BOOL  DeleteFileFromDirectory(char   *DirName,const char* ext)  
{  
	BOOL flag = TRUE;
	HANDLE hFile;
	WIN32_FIND_DATA FindFileData;;
	char tempFileFind[200];  
	char filename[260];  
	sprintf(tempFileFind,"%s\\%s",DirName,ext);   
	hFile = FindFirstFile(tempFileFind,&FindFileData);

	if (hFile != INVALID_HANDLE_VALUE) // 此目录不为空目录
	{
		BOOL   IsFinded = TRUE;
		while(IsFinded)   
		{   
			if(FindFileData.cFileName[0] !=   '.')   
			{   
				strcpy(filename,   DirName);   
				strcat(filename,   FindFileData.cFileName);   
				if(!(FindFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))   
				{ // 删除找到的文件   
					SetFileAttributes(filename,   FILE_ATTRIBUTE_NORMAL);   
					flag   =  DeleteFile(filename);
					if (!flag)
					{
						_ASSERTE(0);
					}
				}   
			}   
			IsFinded   =   FindNextFile(hFile,   &FindFileData);   
		}   
		FindClose(hFile); 
	}

	return TRUE;

}                          
