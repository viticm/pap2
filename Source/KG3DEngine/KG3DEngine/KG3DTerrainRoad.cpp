#include "StdAfx.h"
#include "KG3DTerrainRoad.h"
#include "KG3DGraphicsTool.h"
#include "IEKG3DTerrain.h"
#include "KG3DTexture.h"
#include "KG3DTextureTable.h"
#include "../scene/KG3DSceneSceneEditor.h"
#include "KG3DRepresentObjectNode.h"
#include "KG3DSFXProjectionTextureMgr.h"
#include "KG3DSceneOutDoorSpaceMgr.h"
#include "KG3DHeightMap.h"
#include "KG3DMemoryFile.h"
#include "KG3DTerrainBlockModifier.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern TKG3DResourcePool<KG3D_PointLight_Info>g_cPointLightInfoPool;
extern BOOL g_bOpenSelectTexture;
extern BOOL g_bClient;///是否客户端
#define HeightOffset 3.0f


KG3DTerrainRoad::KG3DTerrainRoad(void)
{
	m_fWidth = 1000;
	m_lpTexture = NULL;
	m_scName = "新建道路";
	m_fTexDensity = 0.001f;      //贴图密度
	m_fBlendLength = 1000;      //端点的透明宽度
	m_fEdgeModulus = 0.75f;
	m_nID = 0;
	m_lpPointLightInfo = NULL;
}

KG3DTerrainRoad::~KG3DTerrainRoad(void)
{
	//UnInit();
}
void KG3DTerrainRoad::UnInit()
{
	KG3DSerpentine::UnInit();
	if(m_lpPointLightInfo)
	{
		g_cPointLightInfoPool.FreeResource(m_lpPointLightInfo);
		m_lpPointLightInfo = NULL;
	}
	SAFE_RELEASE(m_lpTexture);
}
HRESULT KG3DTerrainRoad::AddOnePassage(KG3DPassage** ppPassage,int nPos)
{
	HRESULT hr = E_FAIL;
	list<KG3DPassage*>::iterator iPos = m_listPassage.begin();
	KG3DTerrainRoadPassage* pPassage  = new KG3DTerrainRoadPassage();
	KGLOG_PROCESS_ERROR(pPassage);

	(*ppPassage) = pPassage;

	for(int i=0 ;i< nPos; i++)
		++iPos;
	m_listPassage.insert(iPos,pPassage);
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DTerrainRoad::AddOnePassage(KG3DPassage** ppPassage)
{
	HRESULT hr = E_FAIL;
	KG3DTerrainRoadPassage* pPassage  = new KG3DTerrainRoadPassage();
	KGLOG_PROCESS_ERROR(pPassage);

	(*ppPassage) = pPassage;
	m_listPassage.push_back(pPassage);
	hr = S_OK;
Exit0:
	return hr;
}

BOOL KG3DTerrainRoad::CheckIncurvate(KG3DTerrainRoadPassage::TinyLine& L1,KG3DTerrainRoadPassage::TinyLine& L2)
{
	if((L1.vLeft2D == L2.vLeft2D)||(L1.vLeft2D == -L2.vLeft2D))
		return FALSE;
	
	D3DXVECTOR2 P1(L1.vPositionA[0].x,L1.vPositionA[0].z);
	D3DXVECTOR2 P2(L2.vPositionA[0].x,L2.vPositionA[0].z);

	float K1 = L1.vLeft2D.y/L1.vLeft2D.x;
	float K2 = L2.vLeft2D.y/L2.vLeft2D.x;
	D3DXVECTOR2 Inter;
	
	Inter.x = (P2.y - P1.y +  K1 * P1.x -  K2 * P2.x)/(K1 - K2);
	Inter.y = K1 * (Inter.x - P1.x) + P1.y;
	
	D3DXVECTOR2 dir = Inter - P1;
	if( dir.x * L2.vLeft2D.x >= 0 && dir.y * L2.vLeft2D.y >= 0)
		return TRUE;

	return FALSE;
}

HRESULT KG3DTerrainRoad::LoadRoadTexture(LPCTSTR pFileName)
{
	SAFE_RELEASE(m_lpTexture);
	g_cTextureTable.LoadResourceFromFile(pFileName,0,0,(IKG3DResourceBase**)&m_lpTexture);
	if(m_lpTexture)
	{
		LPCSTR strReturn = NULL;
		m_lpTexture->GetName(&strReturn);
		m_scTextureName = strReturn;//dir;
	}
	return S_OK;
}
HRESULT KG3DTerrainRoad::SetPointLightInfo(KG3D_PointLight_Info& vInfo)
{
	if (g_bEnablePointLight)
	{
		if(vInfo.m_nNumPointLights)
		{
			if(!m_lpPointLightInfo)
			{
				m_lpPointLightInfo = g_cPointLightInfoPool.RequestResource();
			}
			KGLOG_PROCESS_ERROR(m_lpPointLightInfo);

			(*m_lpPointLightInfo) = vInfo;
		}
		else
		{
			if(m_lpPointLightInfo)
			{
				g_cPointLightInfoPool.FreeResource(m_lpPointLightInfo);
				m_lpPointLightInfo = NULL;
			}
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DTerrainRoad::SaveToFile(const char cszFileName[])
{
   // int nRetCode = false;
	HRESULT hr =E_FAIL;
	_RoadHead  RoadHead;
	DWORD PosStart,PosEnd;
	
	IFile* pFile = NULL;
	RoadHead.dwNumOfNode = (DWORD)m_listNode.size();
	RoadHead.dwNumOfPassage = (DWORD)m_listPassage.size();
	D3DXVECTOR3* pNodePos = new D3DXVECTOR3[RoadHead.dwNumOfNode];
	list<KG3DRepresentObjectNode*>::iterator iNode; 
	DWORD k = 0;
	pFile = g_OpenFile(cszFileName,false,true);
	KG_PROCESS_ERROR(pFile);
	PosStart = pFile->Tell();//ftell(pFile);

	iNode = m_listNode.begin();
	while(iNode != m_listNode.end())
	{
		(*iNode)->GetTranslation(&pNodePos[k]);//pNodePos[i] = (*iNode)->vPosition;
		k++;
		++iNode;
	}
	RoadHead.fBendModulus = m_fBendModulus;
	RoadHead.fBlendLength = m_fBlendLength;
	RoadHead.fEdgeModulus = m_fEdgeModulus;
	RoadHead.fWidth   = m_fWidth;
	RoadHead.fTexDensity  = m_fTexDensity;
	RoadHead.nID          = m_nID;
	RoadHead.fNodeSize    = m_fNodeSize;
	RoadHead.dwSegmentLength= m_nSegmentLength;
	RoadHead.dwNumParentBlock = (DWORD)m_vecParentTerrainBlock.size();
	RoadHead.dwParentTerrainBlock  = PosStart + sizeof(_RoadHead); 
	RoadHead.dwNodePosBlock = PosStart + sizeof(_RoadHead) + sizeof(TerrainBlockInfo) * RoadHead.dwNumParentBlock ;
	RoadHead.dwPassageBlock  = PosStart + sizeof(_RoadHead) + sizeof(TerrainBlockInfo) * RoadHead.dwNumParentBlock + sizeof(D3DXVECTOR3)*RoadHead.dwNumOfNode;
	strncpy(RoadHead.scName,m_scName.c_str(),sizeof(TCHAR)*32);
	strncpy(RoadHead.scTextureName,m_scTextureName.c_str(),sizeof(TCHAR)*MAX_PATH);

	pFile->Write(&RoadHead, sizeof(_RoadHead));

	for (int nT= 0; nT < (int)RoadHead.dwNumParentBlock; nT++)
	{
		TerrainBlockInfo TBI = m_vecParentTerrainBlock[nT];
		pFile->Write(&TBI,sizeof(TerrainBlockInfo));
	}
	
	pFile->Write(pNodePos,sizeof(D3DXVECTOR3)*RoadHead.dwNumOfNode);
		
	{
		list<KG3DPassage*>::iterator i = m_listPassage.begin();
		while ( i != m_listPassage.end())
		{
			KG3DTerrainRoadPassage* pPassage = static_cast<KG3DTerrainRoadPassage*>(*i); 
			pPassage->WriteDataToFile(pFile);
			++i;
		}
	}
	
	PosEnd = pFile->Tell();//ftell(pFile);
	hr = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);
	SAFE_DELETE_ARRAY(pNodePos);	
	return hr;
}

HRESULT KG3DTerrainRoad::LoadFromFile(const char cszFileName[])
{
	//return E_FAIL;
    HRESULT hr = E_FAIL;	
	int nRetCode = false;
	TCHAR FileName[MAX_PATH]; 
	_RoadHead RoadHead;
	DWORD nSize;
	D3DXVECTOR3* pNodePos = NULL;
	IFile* pFile = NULL;
	pFile = g_OpenFile(cszFileName);
	KG_PROCESS_ERROR(pFile);

	nSize = pFile->Read(&RoadHead,sizeof(_RoadHead));//fread(&RoadData,sizeof(_RoadData),1,pFile);
	
	m_scName        = RoadHead.scName;
	m_scTextureName = RoadHead.scTextureName;
	m_fBendModulus  = RoadHead.fBendModulus;
	m_fEdgeModulus  = RoadHead.fEdgeModulus;
	m_fTexDensity   = RoadHead.fTexDensity;
	m_fBlendLength  = RoadHead.fBlendLength;
	m_fWidth        = RoadHead.fWidth;
	m_nID           = RoadHead.nID;
	m_fNodeSize     = RoadHead.fNodeSize;
	m_nSegmentLength= RoadHead.dwSegmentLength;
	wsprintf(FileName,"%s", m_scTextureName.c_str());
	LoadRoadTexture(FileName);
	if (!g_bClient)
	{
		nRetCode = pFile->Seek(RoadHead.dwParentTerrainBlock, SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode != -1);
		for (int nT= 0; nT < (int)RoadHead.dwNumParentBlock; nT++)
		{
			TerrainBlockInfo TBI;
			pFile->Read(&TBI,sizeof(TerrainBlockInfo));
			m_vecParentTerrainBlock.push_back(TBI);
		}

		nRetCode = pFile->Seek(RoadHead.dwNodePosBlock, SEEK_SET);//fseek(pFile,RoadData.dwNodePosBlock, SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode != -1);
		pNodePos = new D3DXVECTOR3[RoadHead.dwNumOfNode];
		KGLOG_PROCESS_ERROR(pNodePos);
		nSize = pFile->Read(pNodePos,sizeof(D3DXVECTOR3)*RoadHead.dwNumOfNode);//fread(RoadData.pNodePos,sizeof(D3DXVECTOR3),RoadData.dwNumOfNode,pFile);
		LoadNode(RoadHead.dwNumOfNode,pNodePos);
	}

	nRetCode = pFile->Seek(RoadHead.dwPassageBlock, SEEK_SET);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	{
	
	list<KG3DRepresentObjectNode*>::iterator iNode = m_listNode.begin();
	
	float fLengthSum = 0;
	m_BBox.Clear();
	for (DWORD i = 0;i < RoadHead.dwNumOfPassage;i++)
	{
		KG3DTerrainRoadPassage* pPassage = new KG3DTerrainRoadPassage();
		if(pPassage)
		{
			_PassageData ReadData;
			if(SUCCEEDED(pPassage->ReadDataFromFile(pFile,ReadData)))
			{
				pPassage->CreateBuffersFromData(ReadData);
				pPassage->BulidPassageData(fLengthSum,m_fWidth,m_fBendModulus,m_nSegmentLength,m_fExtendWidth);
				m_listPassage.push_back(pPassage);
				m_BBox.AddPosition(pPassage->m_Bbox.A);
				m_BBox.AddPosition(pPassage->m_Bbox.B);
				m_ExtendBBox.AddPosition(pPassage->m_ExtendBBox.A);
				m_ExtendBBox.AddPosition(pPassage->m_ExtendBBox.B);

				if(iNode!=m_listNode.end())
				{
					pPassage->m_pNodeA = *iNode;
					++iNode;
					if(iNode!=m_listNode.end())
						pPassage->m_pNodeB = *iNode;
				}
			}
			else
			{
				SAFE_DELETE(pPassage);
			}
		}
		
	}
	this->m_fLength = fLengthSum;
	}
	hr = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);
	SAFE_DELETE_ARRAY(pNodePos);
	return hr;
}
HRESULT KG3DTerrainRoad::LoadNode(DWORD dwNumOfNode,D3DXVECTOR3* pNodePos)
{
	HRESULT hr = E_FAIL;
	for(DWORD i = 0; i< dwNumOfNode; i++)
	{
		KG3DRepresentObjectNode* pNode = NULL;		
		KG3DSceneSceneEditor* pEditor= NULL;
		KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
		KG_PROCESS_ERROR(pScene);
		pEditor = static_cast<KG3DSceneSceneEditor*>(pScene);
		pEditor->AddNodeToScene(&pNode,&(pNodePos[i]),&D3DXVECTOR3(m_fNodeSize,m_fNodeSize,m_fNodeSize));
		/*pNode->SetTranslation(&(pNodePos[i]));
		pNode->SetScaling(&D3DXVECTOR3(m_fNodeSize,m_fNodeSize,m_fNodeSize));*/
		pNode->SetNodeType(NODETYPE_ROAD);
		pNode->SetPointer((PVOID)this);
		AddOneNode(pNode,pNodePos[i]);
	}
	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DTerrainRoad::RefreshHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BOOL bToTerrain)
{
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	{	
		if(bToTerrain)
		{
			D3DXVECTOR2 A,C;
			m_BBox.GetXZRect(A,C);
			g_cTerrainBlockModifier.FillHeightMap(A,C,pOutDoorSpaceMgr);
			//////////////////////////////////////////////////////////////////////////
			{
				list<KG3DPassage*>::iterator i = m_listPassage.begin();
				list<KG3DPassage*>::iterator iend = m_listPassage.end();
				while (i!=iend)
				{
					KG3DTerrainRoadPassage* pPassage = static_cast<KG3DTerrainRoadPassage*>(*i);
					pPassage->UpdateTerrainHeight(pOutDoorSpaceMgr);
					++i;
				}
			}
			/*pOutDoorSpaceMgr->UpdateTerrainHeight(A,C);*/
			g_cTerrainBlockModifier.UpdateTerrainBlockHeight(pOutDoorSpaceMgr);
		}
		else
		{
			list<KG3DPassage*>::iterator i = m_listPassage.begin();
			list<KG3DPassage*>::iterator iend = m_listPassage.end();
			while (i!=iend)
			{
				KG3DTerrainRoadPassage* pPassage = static_cast<KG3DTerrainRoadPassage*>(*i);
				pPassage->UpdateRoadHeight(pOutDoorSpaceMgr);
				++i;
			}
		}
		return S_OK;
	}
	
Exit0:
	return E_FAIL;
}

////////////////////////////////////////////////////////////////////////////////////////////////////
/////KG3DTerrainRoadPassage
HRESULT KG3DTerrainRoadPassage::UpdateVertex(KG3DSerpentine* pSerpentine)
{
	D3DXVECTOR3 BA = m_vPositionB - m_vPositionA;
	D3DXVECTOR3 BA_Normal;
	D3DXVec3Normalize(&BA_Normal,&BA);
	KG3DTerrainRoad* pRoad = static_cast<KG3DTerrainRoad*>(pSerpentine);
	//////////////////////////////////////////////////////////////////////////
	HRESULT hr = E_FAIL;
	VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1 * pVer = NULL;

	KGLOG_PROCESS_ERROR(m_lpVB);

	hr = m_lpVB->Lock( 0, 0, (void**)&pVer, 0 );
	KG_COM_PROCESS_ERROR(hr);
	_ASSERT(m_uNumVertex == (UINT)m_vecPoint.size());
	for (UINT t=0;t<m_uNumVertex;t++)
	{
		int X = m_vecPoint[t].x;
		int Z = m_vecPoint[t].y;

		pVer[t].p.x = X * TERRIANCELLWIDTH;
		pVer[t].p.z = Z * TERRIANCELLWIDTH;
		pVer[t].Normal = D3DXVECTOR3(0,1,0);
		pVer[t].diffuse = 0x00FFFFFF;
		pVer[t].tu1 = 0; //0.001F;
		pVer[t].tv1 = 0;

		float fHeight = 0;
		float Left;
		float Dis = 0;	
		//KG3DPassage* pPassage = NULL;
	
		if(pRoad->GetHeightAndUV(fHeight,Left,Dis,pVer[t].p.x,pVer[t].p.z))
		{
			float V = 0.5f + Left /(pRoad->m_fWidth *0.70f * pRoad->m_fEdgeModulus);
			pVer[t].Normal = D3DXVECTOR3(0,1,0);
			pVer[t].p.y = fHeight + HeightOffset;
			pVer[t].tu1 = Dis * pRoad->m_fTexDensity; //0.001F;
			pVer[t].tv1 = V;//0.5f + Left / (pRoad->m_fWidth *0.70f * pRoad->m_fEdgeModulus);
			pVer[t].diffuse = 0xFFFFFFFF;
		}
		//else
		//{
		//	pVer[t].diffuse = 0x00FFFFFF;
		//	pVer[t].tu1 = 0; //0.001F;
		//	pVer[t].tv1 = 0;
		//}
		
		
		float BlendLength = pRoad->m_fBlendLength;//1000;

		if(Dis<BlendLength)
		{
			DWORD dwAlpha = (DWORD)(1-(BlendLength - Dis)/BlendLength*255);
			pVer[t].diffuse = (dwAlpha<<24) | 0x00FFFFFF;		
		}
		else if (Dis > pRoad->m_fLength - BlendLength)
		{
			float fVal =(pRoad->m_fLength > Dis) ? (pRoad->m_fLength - Dis) : (Dis - pRoad->m_fLength);
			DWORD dwAlpha = (DWORD)((fVal)/BlendLength*255);
			pVer[t].diffuse = (dwAlpha<<24) | 0x00FFFFFF;
		}
		//int a = 0;
		//a = 1;
	}

	hr = m_lpVB->Unlock();
	KG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}


KG3DTerrainRoadPassage::KG3DTerrainRoadPassage()
{

}

KG3DTerrainRoadPassage::~KG3DTerrainRoadPassage()
{
	
}

HRESULT KG3DTerrainRoadPassage::UpdateTerrainHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	HRESULT hr = E_FAIL;
	int nXStart = (int)(m_Bbox.A.x/TERRIANCELLWIDTH);
	int nXEnd   = (int)(m_Bbox.B.x/TERRIANCELLWIDTH);
	int nZStart = (int)(m_Bbox.A.z/TERRIANCELLWIDTH)+1;
	int nZEnd   = (int)(m_Bbox.B.z/TERRIANCELLWIDTH)+1;

	VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1 * pVer = NULL; 

	KG3DHeightMap* pHeightMap = g_cTerrainBlockModifier.GetHeightMap();
	KG_PROCESS_ERROR(pHeightMap);

	int nHeighMap_X = g_cTerrainBlockModifier.GetHegihtMapStartIndexX(BLOCK_TYPE_REGION) * TERRIANREGIONCELL;
	int nHeighMap_Z = g_cTerrainBlockModifier.GetHegihtMapStartIndexZ(BLOCK_TYPE_REGION) * TERRIANREGIONCELL;

	nXStart = max(0,nXStart - nHeighMap_X);
	nZStart = max(0,nZStart - nHeighMap_Z);
	nXEnd   = min(pHeightMap->GetMapWidth()-1 ,nXEnd - nHeighMap_X);
	nZEnd   = min(pHeightMap->GetMapHeight()-1 ,nZEnd - nHeighMap_Z);

	hr = m_lpVB->Lock( 0, 0, (void**)&pVer, 0 );
	KG_COM_PROCESS_ERROR(hr);

	for(UINT nIndex = 0 ;nIndex < this->m_uNumVertex;nIndex++)
	{
		int X =(int) (pVer[nIndex].p.x / TERRIANCELLWIDTH) - nHeighMap_X;
		int Z =(int) (pVer[nIndex].p.z / TERRIANCELLWIDTH) - nHeighMap_Z;
		if(X>=0 && X<pHeightMap->GetMapWidth() && Z >=0 && Z <pHeightMap->GetMapHeight())
		{
			float* pHeight = pHeightMap->GetVertexPointer(X,Z);
			(*pHeight) = pVer[nIndex].p.y - HeightOffset;//稍微往下移动一点，避免现实上的交错	
		}
	}
	pHeightMap->ComputeNormal(nXStart,nXEnd,nZStart,nZEnd); // 计算法向量
	
	hr = m_lpVB->Unlock();
Exit0:
	return hr;
}
HRESULT KG3DTerrainRoadPassage::UpdateRoadHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	HRESULT hr = E_FAIL;

	VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1 * pVer = NULL; 
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	hr = m_lpVB->Lock( 0, 0, (void**)&pVer, 0 );
	KG_COM_PROCESS_ERROR(hr);
	
	for(UINT nIndex = 0 ;nIndex < this->m_uNumVertex;nIndex++)
	{
		float fHeight = pOutDoorSpaceMgr->GetGroundHeight(pVer[nIndex].p.x,pVer[nIndex].p.z);
		pVer[nIndex].p.y = fHeight + HeightOffset ;//稍微往上移动一点，避免与地面重合交错
		pOutDoorSpaceMgr->GetRegionNormal(pVer[nIndex].Normal,pVer[nIndex].p.x,pVer[nIndex].p.z);
	}

	hr = m_lpVB->Unlock();
Exit0:
	return hr;
}

LPDIRECT3DTEXTURE9 KG3DTerrainRoadPassage::GetTexture()
{
	if(m_lpTexture)
		return m_lpTexture->GetTexture();
	return NULL;
}




///////道路管理器//////////////////////////
KG3DTerrainRoadMgr::KG3DTerrainRoadMgr()
{
	
}
KG3DTerrainRoadMgr::KG3DTerrainRoadMgr(KG3DSceneOutDoorSpaceMgr* pOutDoorMgr)
{
	m_lpOutDoorMgr=pOutDoorMgr;
}
KG3DTerrainRoadMgr::~KG3DTerrainRoadMgr()
{
	UnInit();
}

int KG3DTerrainRoadMgr::GetID()
{
	int nID = 0;
	while (FindRoadByID(nID))
	{
		nID++;
		ASSERT(nID < 65535);
	}
	return nID;
}
KG3DTerrainRoad* KG3DTerrainRoadMgr::GetRoadByID(int& nID)
{
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	while (i != m_listSerpentines.end())
	{
		KG3DTerrainRoad* pRoad = static_cast<KG3DTerrainRoad*>(*i);
		if(nID == pRoad->m_nID)
			return pRoad;
		++i;
	}
	return NULL;
}

BOOL KG3DTerrainRoadMgr::FindRoadByID(int& nID)
{
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	while (i != m_listSerpentines.end())
	{
		KG3DTerrainRoad* pRoad = static_cast<KG3DTerrainRoad*>(*i);
		if(nID == pRoad->m_nID)
			return TRUE;
		++i;
	}
	return FALSE;
}

HRESULT KG3DTerrainRoadMgr::DeleteOne(int nIndex)
{
	int K = 0;
	BOOL bUpdate = FALSE;
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	while (i != m_listSerpentines.end())
	{
		KG3DTerrainRoad* pRoad = static_cast<KG3DTerrainRoad*>(*i);
		if(K==nIndex)
		{
			if(m_lpCurrentSerpentine == pRoad)
				m_lpCurrentSerpentine = NULL;
			m_mapNames.erase(pRoad->m_uHandle);
			m_lpOutDoorMgr->EraseRoad(pRoad);
			SAFE_RELEASE(pRoad);
			m_listSerpentines.erase(i);
			bUpdate = TRUE;
			break;
		}
		++i;
		K++;
	}
	if (bUpdate && m_lpOutDoorMgr && m_lpOutDoorMgr->GetSceneDataInterface() )
	{
		DWORD dwLength =0;
		KG3DMemoryFile* pMemFile = g_cMemoryFilePool.RequestResource();
		if(pMemFile)
		{
			SaveToInformationBuffer(dwLength,pMemFile);
			m_lpOutDoorMgr->GetSceneDataInterface()->Request_ModifyTerrainInformation(m_uHandle,Terrain_Info_RoadMgr,dwLength,pMemFile->GetBuffer());

			g_cMemoryFilePool.FreeResource(pMemFile);
			pMemFile = NULL;
		}
	}
	return S_OK;

}

HRESULT KG3DTerrainRoadMgr::NewOne(KG3DSerpentine** ppSerpentine)
{
	int N = 1;
	UINT uHandle= GetHandle();
	KG3DTerrainRoad* pNewRoad = new KG3DTerrainRoad();
	KG_PROCESS_ERROR(pNewRoad);
	while(FindSerpentineByName(pNewRoad->m_scName.c_str()))
	{
		ASSERT(N<100000);
		char str[32];
		sprintf(str,"新建道路%d",N);
		pNewRoad->m_scName =  str;
		N++;
	}
	m_mapNames.insert(std::pair<UINT,std::string>(uHandle,pNewRoad->m_scName));
	m_listSerpentines.push_back(pNewRoad);

	(*ppSerpentine) = pNewRoad;
	pNewRoad->m_uHandle = uHandle;
	pNewRoad->m_nID = GetID();//默认ID与Handle相等
	pNewRoad->LoadRoadTexture("data\\public\\defaultscene\\Road_10_b.dds");	
	{
		if (m_lpOutDoorMgr && m_lpOutDoorMgr->GetSceneDataInterface() )
		{
			DWORD dwLength =0;
			KG3DMemoryFile* pMemFile = g_cMemoryFilePool.RequestResource();
			if(pMemFile)
			{
				SaveToInformationBuffer(dwLength,pMemFile);
				m_lpOutDoorMgr->GetSceneDataInterface()->Request_ModifyTerrainInformation(m_uHandle,Terrain_Info_RoadMgr,dwLength,pMemFile->GetBuffer());
				g_cMemoryFilePool.FreeResource(pMemFile);
				pMemFile = NULL;
			}
		}
	}
	
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainRoadMgr::SetCurrentRoadBlendLength(float fBlendLength)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG3DTerrainRoad* pRoad = static_cast<KG3DTerrainRoad*>(m_lpCurrentSerpentine);
	pRoad->m_fBlendLength = fBlendLength;
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DTerrainRoadMgr::GetCurrentRoadBlendLength(float& fBlendLength)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG3DTerrainRoad* pRoad = static_cast<KG3DTerrainRoad*>(m_lpCurrentSerpentine);
	fBlendLength = pRoad->m_fBlendLength;
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DTerrainRoadMgr::SetCurrentRoadTexDensity(float fTexDensity)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG3DTerrainRoad* pRoad = static_cast<KG3DTerrainRoad*>(m_lpCurrentSerpentine);
	pRoad->m_fTexDensity = fTexDensity;
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DTerrainRoadMgr::GetCurrentRoadTexDensity(float& fTexDensity)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG3DTerrainRoad* pRoad = static_cast<KG3DTerrainRoad*>(m_lpCurrentSerpentine);
	fTexDensity = pRoad->m_fTexDensity;
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DTerrainRoadMgr::SetCurrentRoadEdgeModulus(float fEdgeModulus)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG3DTerrainRoad* pRoad = static_cast<KG3DTerrainRoad*>(m_lpCurrentSerpentine);
	pRoad->m_fEdgeModulus = fEdgeModulus;
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DTerrainRoadMgr::GetCurrentRoadEdgeModulus(float& fEdgeModulus)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG3DTerrainRoad* pRoad = static_cast<KG3DTerrainRoad*>(m_lpCurrentSerpentine);
	fEdgeModulus = pRoad->m_fEdgeModulus;
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DTerrainRoadMgr::ChangeCurrentRoadTexture(const TCHAR* strName)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG3DTerrainRoad* pRoad = static_cast<KG3DTerrainRoad*>(m_lpCurrentSerpentine);
	hr = pRoad->LoadRoadTexture(strName);
Exit0:
	return hr;
}

HRESULT KG3DTerrainRoadMgr::GetCurrentRoadID(int& nID)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG3DTerrainRoad* pRoad = static_cast<KG3DTerrainRoad*>(m_lpCurrentSerpentine);
	nID = pRoad->m_nID;
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DTerrainRoadMgr::SetCurrentRoadID(int& nID)
{
	HRESULT hr = E_FAIL;
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG3DTerrainRoad* pCurRoad = static_cast<KG3DTerrainRoad*>(m_lpCurrentSerpentine);
	KG3DTerrainRoad* pRoad = GetRoadByID(nID);
	if (pRoad)
	{
		pRoad->m_nID = pCurRoad->m_nID;
	}
	pCurRoad->m_nID = nID;
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DTerrainRoadMgr::SetCurSerName(const TCHAR* strName)
{
	HRESULT hr = KG3DSerpentineMgr::SetCurSerName(strName);
	if (SUCCEEDED(hr) && m_lpOutDoorMgr && m_lpOutDoorMgr->GetSceneDataInterface() )
	{
		DWORD dwLength =0;
		KG3DMemoryFile* pMemFile = g_cMemoryFilePool.RequestResource();
		if(pMemFile)
		{
			SaveToInformationBuffer(dwLength,pMemFile);
			m_lpOutDoorMgr->GetSceneDataInterface()->Request_ModifyTerrainInformation(m_uHandle,Terrain_Info_RoadMgr,dwLength,pMemFile->GetBuffer());

			g_cMemoryFilePool.FreeResource(pMemFile);
			pMemFile = NULL;
		}
	}
	return hr;
}

HRESULT KG3DTerrainRoadMgr::SaveToFile(LPCSTR strFileName) //保存道路信息
{
	HRESULT hr = E_FAIL;
	TCHAR RoadDir[256];
	TCHAR RelRoadDir[MAX_PATH];
	TCHAR fullRoadDir[MAX_PATH];
	TCHAR fname[256];
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	KG_PROCESS_ERROR(strFileName);
	
	wsprintf(RoadDir, "%sRoad", strFileName);
	 g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, RoadDir, RelRoadDir);
	wsprintf(fullRoadDir,"%s%s\\",g_Def_WorkDirectory,RelRoadDir);
	
	DeleteFileFromDirectory(fullRoadDir,"*.Road");
	mkdir(RoadDir);

	while(i != m_listSerpentines.end())
	{
		KG3DSerpentine* pRoad = *i;
		wsprintf(fname, "%s\\%s.Road", RoadDir,pRoad->m_scName.c_str());
		pRoad->SaveToFile(fname);
		++i;
	}
	hr = S_OK;
Exit0:
	return hr;
}


HRESULT KG3DTerrainRoadMgr::AddNodeToCurSerpentine(KG3DRepresentObjectNode* pNode,D3DXVECTOR3& vPos)
{
	KG_PROCESS_ERROR(m_lpOutDoorMgr);
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG3DTerrainRoad* pRoad = static_cast<KG3DTerrainRoad*>(m_lpCurrentSerpentine);
	pRoad->AddOneNode(pNode,vPos);

	pNode->SetPointer((PVOID)m_lpCurrentSerpentine);

	pRoad->ComputePassages(pNode);
	pRoad->CreatePassages(pNode);

	m_lpOutDoorMgr->InputRoad(pRoad);
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainRoadMgr::LoadByHandle(UINT uHandle,KG3DTerrainRoad** ppOutRoad)
{
	KG3DTerrainRoad* pRoad = NULL;
	pRoad = static_cast<KG3DTerrainRoad*>(GetSerpentineByHandle(uHandle));
	if(pRoad)
	{
		*ppOutRoad = pRoad;
		return S_OK;
	}
	map<UINT,std::string>::iterator iFind = m_mapNames.find(uHandle);
	if(iFind == m_mapNames.end())
		return E_FAIL;
	std::string strRoadName = iFind->second;
	CHAR szFileName[MAX_PATH];
	wsprintf(szFileName,"%s%s.Road",m_strFilePath.c_str(),strRoadName.c_str());

	if(!g_IsFileExist(szFileName))
		return E_FAIL;

	pRoad = new KG3DTerrainRoad;
	if(FAILED(pRoad->LoadFromFile(szFileName)))
	{
		SAFE_RELEASE(pRoad);
		return E_FAIL;
	}
	m_listSerpentines.push_back(pRoad);
	pRoad->m_uHandle = uHandle;
	*ppOutRoad = pRoad;
	return S_OK;
}