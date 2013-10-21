#include "StdAfx.h"
#include "Terrain/KG3DTerrainRiver.h"
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

KG3DTerrainRiver::KG3DTerrainRiver(void)
{
	m_fWidth = 2000;
	m_scName = "新建河流";
	m_fDepth = 800;
	m_nSegmentLength =100;
	m_fExtendWidth = 1000.0f;
}
KG3DTerrainRiver::~KG3DTerrainRiver(void)
{
}
HRESULT KG3DTerrainRiver::AddOnePassage(KG3DPassage** ppPassage,int nPos)
{
	HRESULT hr = E_FAIL;
	list<KG3DPassage*>::iterator iPos = m_listPassage.begin();
	KG3DTerrainRiverPassage* pPassage  = new KG3DTerrainRiverPassage();
	KGLOG_PROCESS_ERROR(pPassage);

	(*ppPassage) = pPassage;

	for(int i=0 ;i< nPos; i++)
		++iPos;
	m_listPassage.insert(iPos,pPassage);
	hr = S_OK;
Exit0:
	return hr;
}
HRESULT KG3DTerrainRiver::AddOnePassage(KG3DPassage** ppPassage)
{
	HRESULT hr = E_FAIL;
	KG3DTerrainRiverPassage* pPassage  = new KG3DTerrainRiverPassage();
	KGLOG_PROCESS_ERROR(pPassage);

	(*ppPassage) = pPassage;
	m_listPassage.push_back(pPassage);
	hr = S_OK;
Exit0:
	return hr;
}

void KG3DTerrainRiver::SetDepth(float fDepth)
{
	m_fDepth = fDepth;
}

float KG3DTerrainRiver::GetDepth()
{
	return m_fDepth;
}

HRESULT KG3DTerrainRiver::SaveToFile(const char cszFileName[])
{
	HRESULT hr =E_FAIL;
	unsigned long uSize = 0;
	_RiverHead  RiverHead;
	DWORD PosStart =0;
	IFile* pFile=NULL;
	D3DXVECTOR3* pNodePos = NULL;
	list<KG3DRepresentObjectNode*>::iterator iNode = m_listNode.begin();
	pFile = g_OpenFile(cszFileName,false,true);
	KG_PROCESS_ERROR(pFile);
	PosStart = pFile->Tell();
	RiverHead.dwNumOfNode = (DWORD)m_listNode.size();
	RiverHead.dwNumOfPassage = (DWORD)m_listPassage.size();

	RiverHead.fBendModulus = m_fBendModulus;
	RiverHead.fDepth = m_fDepth;
	RiverHead.fWidth = m_fWidth;
	RiverHead.dwSegmentLength = m_nSegmentLength;
	RiverHead.dwNumParentBlock = (DWORD)m_vecParentTerrainBlock.size();
	strncpy(RiverHead.scName,m_scName.c_str(),sizeof(TCHAR)*32);
	RiverHead.dwParentTerrainBlock  = PosStart + sizeof(_RiverHead); 
	RiverHead.dwNodePosBlock = PosStart + sizeof(_RiverHead) + sizeof(TerrainBlockInfo) * RiverHead.dwNumParentBlock;
	RiverHead.dwPassageBlock = PosStart + sizeof(_RiverHead) + sizeof(TerrainBlockInfo) * RiverHead.dwNumParentBlock + sizeof(D3DXVECTOR3)*RiverHead.dwNumOfNode;
	
	DWORD K = 0;
	pNodePos = new D3DXVECTOR3[RiverHead.dwNumOfNode];
	
	while(iNode != m_listNode.end())
	{
		(*iNode)->GetTranslation(&pNodePos[K]);
		K++;
		++iNode;
	}

	uSize = pFile->Write(&RiverHead, sizeof(_RiverHead));
	KG_PROCESS_ERROR(uSize == sizeof(_RiverHead));

	for (int nT= 0; nT < (int)RiverHead.dwNumParentBlock; nT++)
	{
		TerrainBlockInfo TBI = m_vecParentTerrainBlock[nT];
		pFile->Write(&TBI,sizeof(TerrainBlockInfo));
	}

	uSize = pFile->Write(pNodePos,sizeof(D3DXVECTOR3)*RiverHead.dwNumOfNode);
	KG_PROCESS_ERROR(uSize == sizeof(D3DXVECTOR3)*RiverHead.dwNumOfNode);
	{
		list<KG3DPassage*>::iterator i = m_listPassage.begin();
		while ( i != m_listPassage.end())
		{
			KG3DTerrainRiverPassage* pPassage = static_cast<KG3DTerrainRiverPassage*>(*i); 
			pPassage->WriteDataToFile(pFile);
			++i;
		}
	}
	
	hr = S_OK;
Exit0:
	SAFE_DELETE_ARRAY(pNodePos);
	KG_COM_RELEASE(pFile);
	return hr;
}

HRESULT KG3DTerrainRiver::LoadFromFile(const char cszFileName[])
{
	HRESULT hr = E_FAIL;
	int nRetCode = false;	
	_RiverHead RiverHead;
	unsigned long uSize;
	D3DXVECTOR3* pNodePos=NULL;
	IFile* pFile = NULL;
	
	pFile = g_OpenFile(cszFileName);
	KG_PROCESS_ERROR(pFile);
	
	uSize = pFile->Read(&RiverHead,sizeof(_RiverHead));
	KG_PROCESS_ERROR(uSize == sizeof(_RiverHead));

	m_nSegmentLength= RiverHead.dwSegmentLength;
	/*m_ParentTerrainBlock.nLevel = RiverHead.nLevel;
	m_ParentTerrainBlock.Index.x = RiverHead.nIndexX;
	m_ParentTerrainBlock.Index.y = RiverHead.nIndexZ;*/

	m_scName        = RiverHead.scName;
	m_fBendModulus  = RiverHead.fBendModulus;
	m_fDepth        = RiverHead.fDepth;
	m_fWidth        = RiverHead.fWidth;

	if(!g_bClient)
	{
		nRetCode = pFile->Seek(RiverHead.dwParentTerrainBlock, SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode != -1);
		for (int nT= 0; nT < (int)RiverHead.dwNumParentBlock; nT++)
		{
			TerrainBlockInfo TBI;
			pFile->Read(&TBI,sizeof(TerrainBlockInfo));
			m_vecParentTerrainBlock.push_back(TBI);
		}

		nRetCode = pFile->Seek(RiverHead.dwNodePosBlock, SEEK_SET);
		KGLOG_PROCESS_ERROR(nRetCode != -1);

		pNodePos = new D3DXVECTOR3[RiverHead.dwNumOfNode];
		KGLOG_PROCESS_ERROR(pNodePos);

		uSize = pFile->Read(pNodePos,sizeof(D3DXVECTOR3)*RiverHead.dwNumOfNode);
		KG_PROCESS_ERROR(uSize == sizeof(D3DXVECTOR3)*RiverHead.dwNumOfNode);

		LoadNode(RiverHead.dwNumOfNode,pNodePos);
	}
	nRetCode = pFile->Seek(RiverHead.dwPassageBlock, SEEK_SET);
	KGLOG_PROCESS_ERROR(nRetCode != -1);
	{
	m_BBox.Clear();
	float fLengthSum = 0;
	list<KG3DRepresentObjectNode*>::iterator iNode = m_listNode.begin();
	for (DWORD i = 0;i < RiverHead.dwNumOfPassage;i++)
	{
		KG3DTerrainRiverPassage* pPassage = new KG3DTerrainRiverPassage();
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
	//ComputePassages();
	//CreateRiver(pTerrainEx);
	hr = S_OK;
Exit0:
	KG_COM_RELEASE(pFile);
	SAFE_DELETE_ARRAY(pNodePos);
	return hr;
}

HRESULT KG3DTerrainRiver::LoadNode(DWORD dwNumOfNode,D3DXVECTOR3* pNodePos)
{
	HRESULT hr = E_FAIL;
	for(DWORD i = 0; i< dwNumOfNode; i++)
	{
		KG3DRepresentObjectNode* pNode = NULL;
		//NewOneNode(&pNode);		
		KG3DSceneSceneEditor* pEditor= NULL;
		KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
		KG_PROCESS_ERROR(pScene);
		pEditor = static_cast<KG3DSceneSceneEditor*>(pScene);
		pEditor->AddNodeToScene(&pNode,&(pNodePos[i]),&D3DXVECTOR3(m_fNodeSize,m_fNodeSize,m_fNodeSize));
		/*pNode->SetTranslation(&(pNodePos[i]));
		pNode->SetScaling(&D3DXVECTOR3(m_fNodeSize,m_fNodeSize,m_fNodeSize));*/
		pNode->SetNodeType(NODETYPE_RIVER);
		pNode->SetPointer((PVOID)this);
		AddOneNode(pNode,pNodePos[i]);
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DTerrainRiver::RefreshHeight(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr,BOOL bToTerrain)
{
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	{
		D3DXVECTOR2 A(m_ExtendBBox.A.x,m_ExtendBBox.A.z);
		D3DXVECTOR2 C(m_ExtendBBox.B.x,m_ExtendBBox.B.z);
		g_cTerrainBlockModifier.FillHeightMap(A,C,pOutDoorSpaceMgr);

		list<KG3DPassage*>::iterator i = m_listPassage.begin();
		list<KG3DPassage*>::iterator iend = m_listPassage.end();
		while (i!=iend)
		{
			KG3DTerrainRiverPassage* pPassage = static_cast<KG3DTerrainRiverPassage*>(*i);
			pPassage->UpdateTerrainHeight(this,pOutDoorSpaceMgr);
			++i;
		}
		{
			int nHeighMap_X = g_cTerrainBlockModifier.GetHegihtMapStartIndexX(BLOCK_TYPE_REGION) * TERRIANREGIONCELL;
			int nHeighMap_Z = g_cTerrainBlockModifier.GetHegihtMapStartIndexZ(BLOCK_TYPE_REGION) * TERRIANREGIONCELL;
			int nXStart = (int)(m_ExtendBBox.A.x/TERRIANCELLWIDTH) - nHeighMap_X;
			int nXEnd   = (int)(m_ExtendBBox.B.x/TERRIANCELLWIDTH) - nHeighMap_X;
			int nZStart = (int)(m_ExtendBBox.A.z/TERRIANCELLWIDTH)+1 - nHeighMap_Z;
			int nZEnd   = (int)(m_ExtendBBox.B.z/TERRIANCELLWIDTH)+1 - nHeighMap_Z;
			
			KG3DHeightMap* pHeightMap = g_cTerrainBlockModifier.GetHeightMap(); 
			KG_PROCESS_ERROR(pHeightMap);
			nXStart = max(nXStart,0);
			nXEnd   = min(nXEnd,pHeightMap->GetMapWidth()-1);
			nZStart = max(nZStart,0);
			nZEnd   = min(nZEnd,pHeightMap->GetMapHeight()-1);
			pHeightMap->ComputeNormal(nXStart,nXEnd,nZStart,nZEnd); // 计算法向量
		}
		g_cTerrainBlockModifier.UpdateTerrainBlockHeight(pOutDoorSpaceMgr);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}
HRESULT KG3DTerrainRiver::ComputeAlpha(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	D3DXVECTOR2 A,C;
	m_ExtendBBox.GetXZRect(A,C);
	g_cTerrainBlockModifier.FillHeightMap(A,C,pOutDoorSpaceMgr);

	list<KG3DPassage*>::iterator i = m_listPassage.begin();
	list<KG3DPassage*>::iterator iend = m_listPassage.end();
	while (i!=iend)
	{
		KG3DTerrainRiverPassage* pPassage = static_cast<KG3DTerrainRiverPassage*>(*i);
		pPassage->UpdatAlpha(pOutDoorSpaceMgr);
		++i;
	}
	return S_OK;
}


HRESULT KG3DTerrainRiverPassage::UpdateVertex(KG3DSerpentine* pSerpentine)
{
	HRESULT hr = E_FAIL;
	float Pi =D3DX_PI *0.5f;
	VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1 * pVer = NULL;
	KG3DTerrainRiver* pRiver = static_cast<KG3DTerrainRiver*>(pSerpentine);
	KG_PROCESS_ERROR(pRiver);
	KG_PROCESS_ERROR(m_lpVB);
	hr = m_lpVB->Lock( 0, 0, (void**)&pVer, 0 );
	KG_COM_PROCESS_ERROR(hr);
	
	KG3DPassage* pPassage = NULL;
	_ASSERT(m_uNumVertex == (UINT)m_vecPoint.size());
	for (size_t t=0;t<m_vecPoint.size();t++)
	{
		int X = m_vecPoint[t].x;
		int Z = m_vecPoint[t].y;

		pVer[t].Normal = D3DXVECTOR3(0,1,0);
		pVer[t].p.x = X * TERRIANCELLWIDTH;
		pVer[t].p.z = Z * TERRIANCELLWIDTH;
		float fHeight,V,fLeftDis,fLength;

		float fWidth = pRiver->m_fWidth *0.5f;
		float fDepth = pRiver->m_fDepth;
		if(pRiver->IsPositionInside(&pPassage,pVer[t].p.x,pVer[t].p.z))
		{
			if(pPassage->GetHeightAndUV(fHeight,fLeftDis,fLength,pVer[t].p.x,pVer[t].p.z))
			{
				V =fLeftDis /fWidth;//* 0.0025f;// fWidth;
				pVer[t].p.y = fHeight - 100;
				pVer[t].tu1 = fLength * 0.001f;
				pVer[t].tv1 = V;// 0.5f + Left / (pRoad->m_fWidth *0.70f * pRoad->m_fEdgeModulus);
				pVer[t].Normal = D3DXVECTOR3(0,1,0);
				float K,L ;
				L = fabs(fLeftDis);
				fWidth = fWidth - 250.0f;
				if( L> fWidth)
					L = fWidth;
				K = L /fWidth *Pi;
			
				if (fLength <=200 || (fLength >= pRiver->m_fLength -200))
					K = Pi;
		
			//	(*pHeight) = fHeight - (fDepth * 0.8f * cosf(K));
				float HDis = fHeight - (fDepth * 0.8f * cosf(K)) - pVer[t].p.y;
				pVer[t].diffuse = ComputerColor(HDis);
			}
			else
			{
				pVer[t].diffuse = 0xFFFFFFFF;
				pVer[t].tu1 = 0;
				pVer[t].tv1 = 0;
			}
		}
	}
	
	hr = m_lpVB->Unlock();
	KG_COM_PROCESS_ERROR(hr);
	
Exit0:
	return hr;
}
	
HRESULT KG3DTerrainRiverPassage::UpdateTerrainHeight(KG3DTerrainRiver* pRiver,KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr )
{	
	HRESULT hr =E_FAIL;
	float Pi = D3DX_PI * 0.5f;
	VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1 * pVer = NULL;
	KG3DPassage* pPassage = NULL;
	KG3DHeightMap* pHeightMap = NULL;
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	KG_PROCESS_ERROR(pRiver);
	KG_PROCESS_ERROR(m_lpVB);
	hr = m_lpVB->Lock( 0, 0, (void**)&pVer, 0 );
	KG_COM_PROCESS_ERROR(hr);
	
	pHeightMap = g_cTerrainBlockModifier.GetHeightMap();
	KG_PROCESS_ERROR(pHeightMap);

	int nHeighMap_X = g_cTerrainBlockModifier.GetHegihtMapStartIndexX(BLOCK_TYPE_REGION) * TERRIANREGIONCELL;
	int nHeighMap_Z = g_cTerrainBlockModifier.GetHegihtMapStartIndexZ(BLOCK_TYPE_REGION) * TERRIANREGIONCELL;

	int nMapWidth = pHeightMap->GetMapWidth();
	int nMapHeight= pHeightMap->GetMapHeight();

	for (size_t t=0;t<m_vecPoint.size();t++)
	{
		int X = m_vecPoint[t].x - nHeighMap_X;
		int Z = m_vecPoint[t].y - nHeighMap_Z;
		//ASSERT( X>=0);
		//ASSERT( Z>=0);
		float fHeight,fLeftDis,fLength;

		float fWidth = pRiver->m_fWidth *0.5f;
		float fDepth = pRiver->m_fDepth;
		if(pRiver->IsPositionInside(&pPassage,pVer[t].p.x,pVer[t].p.z))
		{
			if(pPassage->GetHeightAndUV(fHeight,fLeftDis,fLength,pVer[t].p.x,pVer[t].p.z))
			{
				if (X >= nMapWidth)
			    	X = nMapWidth - 1;
				if (Z >= nMapHeight)
					Z = nMapHeight - 1;
				float* pHeight = pHeightMap->GetVertexPointer(X,Z);

				float K,L ;
				L = fabs(fLeftDis);
				fWidth = fWidth - 250.0f;
				if (fWidth <= 0)
					fWidth = 250;
				if( L> fWidth)
					L = fWidth;
				K = L /fWidth *Pi;

				if (fLength <=200 || (fLength >= pRiver->m_fLength -200))
					K = Pi;
				float fTemp = (fDepth * 0.8f * cosf(K));
				(*pHeight) = fHeight - fTemp;

				float HDis = fHeight - fTemp - pVer[t].p.y;
					pVer[t].diffuse = ComputerColor(HDis);
			}
		}
	}

	hr = m_lpVB->Unlock();
	KG_COM_PROCESS_ERROR(hr);

	for (size_t t=0;t<m_vecTerrainPoint.size();t++)
	{
		int X = m_vecTerrainPoint[t].x - nHeighMap_X;
		int Z = m_vecTerrainPoint[t].y - nHeighMap_Z;
		float fx = m_vecTerrainPoint[t].x * TERRIANCELLWIDTH;
		float fz = m_vecTerrainPoint[t].y * TERRIANCELLWIDTH;
		
		if(pRiver->IsPositionInsideExtend(&pPassage,fx,fz))
		{
			float fHeight,fLeftDis,fLength;
			float fWidth = pRiver->m_fWidth * 0.5f;
			TinyLine* pLine = NULL;
			if(pPassage->IsInside(&pLine,fx,fz))
			{
				D3DXVECTOR2 VA;
				VA.x = fx - pLine->vPositionA[0].x;
				VA.y = fz - pLine->vPositionA[0].z;
				float dotL = D3DXVec2Dot(&pLine->vBANormal2D,&VA);//弯曲的时候，其实接近一个平行四边形

				fLength = dotL + pLine->fLengthStart;
				D3DXVECTOR3 vPos = pLine->vPositionA[0] + dotL * pLine->vBANormal;

				float dotLeft =D3DXVec2Dot(&VA,&pLine->vLeft2D);
				fHeight = vPos.y;
				fLeftDis = fabs(dotLeft) - fWidth;
				if(fLeftDis > pRiver->m_fExtendWidth)
					fLeftDis = pRiver->m_fExtendWidth;
				if( fLeftDis > 0)
				{
					if (X >= nMapWidth)
						X = nMapWidth - 1;
					if (Z >= nMapHeight)
						Z = nMapHeight - 1;
					float* pHeight = pHeightMap->GetVertexPointer(X,Z);
					float a = fLeftDis / pRiver->m_fExtendWidth * Pi;
					//float a =sqrt(fHeight *fHeight *  (1 - b*b ));
					*pHeight = max(*pHeight,fHeight * cosf(a));
				}
			}
		}
		
		
	}
	hr = S_OK;
Exit0:
	return hr;
}

D3DXCOLOR KG3DTerrainRiverPassage::ComputerColor(float Height)
{
	if (Height>=0)
		return 0x33FFFFFF;
	else
	{
		DWORD Alpha = DWORD(min(-Height*0.1f+50.0f,140.0f));
		return (Alpha<<24 | 0x00FFFFFF);
	}
}

HRESULT KG3DTerrainRiverPassage::UpdatAlpha(KG3DSceneOutDoorSpaceMgr* pOutDoorSpaceMgr)
{
	HRESULT hr = E_FAIL;
	VFormat::FACES_NORMAL_DIFFUSE_TEXTURE1 * pVer = NULL;
	KG_PROCESS_ERROR(pOutDoorSpaceMgr);
	KG_PROCESS_ERROR(m_lpVB);
	hr = m_lpVB->Lock( 0, 0, (void**)&pVer, 0 );
	KG_COM_PROCESS_ERROR(hr);
	for (UINT t=0;t<m_uNumVertex;t++)
	{
		float fHeight = pOutDoorSpaceMgr->GetGroundHeight(pVer[t].p.x,pVer[t].p.z);
		float HDis = fHeight - pVer[t].p.y;
		pVer[t].diffuse = ComputerColor(HDis);
				
	}
	hr = m_lpVB->Unlock();
	KG_COM_PROCESS_ERROR(hr);
	hr = S_OK;
Exit0:
	return hr;
}

//河流管理器
KG3DTerrainRiverMgr::KG3DTerrainRiverMgr()
{
}
KG3DTerrainRiverMgr::KG3DTerrainRiverMgr(KG3DSceneOutDoorSpaceMgr* pOutDoorMgr)
{
	m_lpOutDoorMgr=pOutDoorMgr;
}
KG3DTerrainRiverMgr::~KG3DTerrainRiverMgr()
{
}

HRESULT KG3DTerrainRiverMgr::DeleteOne(int nIndex)
{
	BOOL bUpdate = FALSE;
	int K = 0;
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	while (i!=m_listSerpentines.end())
	{
		KG3DTerrainRiver* pRiver = static_cast<KG3DTerrainRiver*>(*i);
		if(K==nIndex)
		{
			if(m_lpCurrentSerpentine == pRiver)
				m_lpCurrentSerpentine = NULL;
			m_mapNames.erase(pRiver->m_uHandle);
			m_lpOutDoorMgr->EraseRiver(pRiver);
			SAFE_RELEASE(pRiver);
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
			m_lpOutDoorMgr->GetSceneDataInterface()->Request_ModifyTerrainInformation(m_uHandle,Terrain_Info_RiverMgr,dwLength,pMemFile->GetBuffer());
			g_cMemoryFilePool.FreeResource(pMemFile);
			pMemFile = NULL;
		}
	}
	return S_OK;
}

HRESULT KG3DTerrainRiverMgr::NewOne(KG3DSerpentine** ppSerpentine)
{
	int N = 1;
	UINT uHandle= GetHandle();
	KG3DTerrainRiver* pNewRiver = new KG3DTerrainRiver();
	KG_PROCESS_ERROR(pNewRiver);
	while(FindSerpentineByName(pNewRiver->m_scName.c_str()))
	{
		ASSERT(N<100000);
		char str[32];
		sprintf(str,"新建河流%d",N);
		pNewRiver->m_scName =  str;
		N++;
	}
	m_mapNames.insert(std::pair<UINT,std::string>(uHandle,pNewRiver->m_scName));
	m_listSerpentines.push_back(pNewRiver);
	pNewRiver->m_uHandle = uHandle;
	(*ppSerpentine) = pNewRiver;
	//pNewRiver->LoadRoadTexture("data\\public\\defaultscene\\Road_10_b.dds");	
	{
		if (m_lpOutDoorMgr && m_lpOutDoorMgr->GetSceneDataInterface() )
		{
			DWORD dwLength =0;
			KG3DMemoryFile* pMemFile = g_cMemoryFilePool.RequestResource();
			if(pMemFile)
			{
				SaveToInformationBuffer(dwLength,pMemFile);
				m_lpOutDoorMgr->GetSceneDataInterface()->Request_ModifyTerrainInformation(m_uHandle,Terrain_Info_RiverMgr,dwLength,pMemFile->GetBuffer());
				g_cMemoryFilePool.FreeResource(pMemFile);
				pMemFile = NULL;
			}
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

float  KG3DTerrainRiverMgr::GetCurrentRiverDepth()
{
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	
	return static_cast<KG3DTerrainRiver*>(m_lpCurrentSerpentine)->GetDepth();
Exit0:
	return 0;
}
void  KG3DTerrainRiverMgr::SetCurrentRiverDepth(float fDepth)
{
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	static_cast<KG3DTerrainRiver*>(m_lpCurrentSerpentine)->SetDepth(fDepth);
Exit0:
	return ;
}

HRESULT KG3DTerrainRiverMgr::SaveToFile(LPCSTR strFileName) //保存河流信息到文件
{
	HRESULT hr = E_FAIL;
	TCHAR RiverDir[256];
	TCHAR RelRiverDir[MAX_PATH];
	TCHAR fullRiverDir[MAX_PATH];
	TCHAR fname[256];
	list<KG3DSerpentine*>::iterator i = m_listSerpentines.begin();
	KG_PROCESS_ERROR(strFileName);

	wsprintf(RiverDir, "%sRiver", strFileName);
	g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, RiverDir, RelRiverDir);
	wsprintf(fullRiverDir,"%s%s\\",g_Def_WorkDirectory,RelRiverDir);

	DeleteFileFromDirectory(fullRiverDir,"*.River");
	mkdir(RiverDir);

	while(i != m_listSerpentines.end())
	{
		KG3DSerpentine* pRiver = *i;
		wsprintf(fname, "%s\\%s.River", RiverDir,pRiver->m_scName.c_str());
		pRiver->SaveToFile(fname);
		++i;
	}
	hr = S_OK;
Exit0:
	return hr;
}

HRESULT KG3DTerrainRiverMgr::AddNodeToCurSerpentine(KG3DRepresentObjectNode* pNode,D3DXVECTOR3& vPos)
{
	KG_PROCESS_ERROR(m_lpOutDoorMgr);
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG3DTerrainRiver* pRiver = static_cast<KG3DTerrainRiver*>(m_lpCurrentSerpentine);
	pRiver->AddOneNode(pNode,vPos);

	pNode->SetPointer((PVOID)m_lpCurrentSerpentine);

	pRiver->ComputePassages(pNode);
	pRiver->CreatePassages(pNode);
	
	m_lpOutDoorMgr->InputRiver(pRiver);
	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DTerrainRiverMgr::LoadByHandle(UINT uHandle,KG3DTerrainRiver** ppOutRiver)
{
	*ppOutRiver = NULL;
	KG3DTerrainRiver* pRiver = NULL;
	pRiver = static_cast<KG3DTerrainRiver*>(GetSerpentineByHandle(uHandle));
	if(pRiver)
	{
		*ppOutRiver = pRiver;
		return S_OK;
	}
	map<UINT,std::string>::iterator iFind = m_mapNames.find(uHandle);
	if(iFind == m_mapNames.end())
		return E_FAIL;
	std::string strSerpentineName = iFind->second;
	CHAR szFileName[MAX_PATH];
	wsprintf(szFileName,"%s%s.River",m_strFilePath.c_str(),strSerpentineName.c_str());

	if(!g_IsFileExist(szFileName))
		return E_FAIL;

	pRiver = new KG3DTerrainRiver;
	if(FAILED(pRiver->LoadFromFile(szFileName)))
	{
		SAFE_RELEASE(pRiver);
		return E_FAIL;
	}
	m_listSerpentines.push_back(pRiver);
	pRiver->m_uHandle = uHandle;
	*ppOutRiver = pRiver;
	return S_OK;

}

HRESULT KG3DTerrainRiverMgr::ComputeCurrentRiverAlpha()
{
	KG_PROCESS_ERROR(m_lpOutDoorMgr);
	KG_PROCESS_ERROR(m_lpCurrentSerpentine);
	KG3DTerrainRiver* pRiver = static_cast<KG3DTerrainRiver*>(m_lpCurrentSerpentine);
	pRiver->ComputeAlpha(m_lpOutDoorMgr);
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DTerrainRiverMgr::SetCurSerName(const TCHAR* strName)
{
	HRESULT hr = KG3DSerpentineMgr::SetCurSerName(strName);
	if (SUCCEEDED(hr) && m_lpOutDoorMgr && m_lpOutDoorMgr->GetSceneDataInterface() )
	{
		DWORD dwLength =0;
		KG3DMemoryFile* pMemFile = g_cMemoryFilePool.RequestResource();
		if(pMemFile)
		{
			SaveToInformationBuffer(dwLength,pMemFile);
			m_lpOutDoorMgr->GetSceneDataInterface()->Request_ModifyTerrainInformation(m_uHandle,Terrain_Info_RiverMgr,dwLength,pMemFile->GetBuffer());

			g_cMemoryFilePool.FreeResource(pMemFile);
			pMemFile = NULL;
		}
	}
	return hr;
}