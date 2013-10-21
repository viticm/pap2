#include "StdAfx.h"
#include "KG3DRepresentObjectWall.h"
#include "KG3DMesh.h"
#include "KG3DMeshTable.h"
#include "KG3DGraphicsTool.h"
#include "KG3DModelTable.h"
#include "KG3DTerrainBlockGrass.h"

extern int g_nGrassVertexBufferFillPerFrame;
extern int g_nLoadCount;


KG3DWallDefineTable g_cWallDefineTable;
//////////////////////////////////////////////////////////////////////////

KG3DWallDefine::KG3DWallDefine()
{
	m_ulRefCount = 0;
}

KG3DWallDefine::~KG3DWallDefine()
{
	;
}

ULONG KG3DWallDefine::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

HRESULT KG3DWallDefine::WallElement::GetDataFromFile()
{
	KG3DMesh* pMesh = NULL;
	if(_strcmpi( szModelFile.c_str(),"")!=0)
	{
		g_cMeshTable.LoadResourceFromFile(szModelFile.c_str(),0,0,(IKG3DResourceBase**)&pMesh);
	}

	if(pMesh)
	{
		bbox = pMesh->m_BBox;
		D3DXVECTOR3 center = bbox.GetCenter();

		vConnectionPointA.x = bbox.A.x;
		vConnectionPointA.y = 0;
		vConnectionPointA.z = 0;

		vConnectionPointB.x = bbox.B.x;
		vConnectionPointB.y = 0;
		vConnectionPointB.z = 0;

		D3DXVECTOR3 V = vConnectionPointB - vConnectionPointA;
		fLength = D3DXVec3Length(&V);
	}
	else
	{
		vConnectionPointA = D3DXVECTOR3(-100,0,0);
		vConnectionPointB = D3DXVECTOR3( 100,0,0);

		fLength = 200;
		bbox.Clear();
		bbox.AddPosition(D3DXVECTOR3(-100,0,-50));
		bbox.AddPosition(D3DXVECTOR3( 100,0, 50));
	}

	SAFE_RELEASE(pMesh);

	return S_OK;
}

ULONG KG3DWallDefine::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        g_cWallDefineTable.DeleteWallDefine(this);
    return uNewRefCount;
}

HRESULT KG3DWallDefine::LoadFromFile(LPSTR pFileName)
{

	HRESULT hr = E_FAIL;

	IIniFile* pFile = NULL;
	m_vecWallElement.clear();
	int nCount = 0;

	pFile = g_OpenIniFile(pFileName);
	KGLOG_PROCESS_ERROR(pFile);

	pFile->GetInteger("MAIN","NumElement",0,&nCount);

	for (int i=0;i<nCount;i++)
	{
		WallElement element;
		TCHAR szSectionName[MAX_PATH];
		wsprintf(szSectionName,"Element%d",i);
		int ntype;
		pFile->GetInteger(szSectionName,"eType",0,&ntype);
		element.eType = (KG3DWALLELEMENTTYPE)ntype;

		pFile->GetMultiFloat(szSectionName,"vConnectionPointA",(float*)&element.vConnectionPointA,3);
		pFile->GetMultiFloat(szSectionName,"vConnectionPointB",(float*)&element.vConnectionPointB,3);

		D3DXVECTOR3 A,B;
		pFile->GetMultiFloat(szSectionName,"bboxA",(float*)&A,3);
		pFile->GetMultiFloat(szSectionName,"bboxB",(float*)&B,3);
		element.bbox.Clear();
		element.bbox.AddPosition(A);
		element.bbox.AddPosition(B);

		pFile->GetFloat(szSectionName,"fLength",0,&element.fLength);
		TCHAR szModelName[MAX_PATH];
		pFile->GetString(szSectionName,"ModelName","",szModelName,MAX_PATH);
		element.szModelFile = szModelName;
		element.nIndex = i;

		m_vecWallElement.push_back(element);
	}

	hr =  S_OK;
Exit0:
	KG_COM_RELEASE(pFile);
	return hr;
}

HRESULT KG3DWallDefine::SaveToFile(LPSTR pFileName)
{
	HRESULT hr = E_FAIL;

	IIniFile* pFile = NULL;
	pFile = g_OpenIniFile(pFileName,true,true);
	KGLOG_PROCESS_ERROR(pFile);

	pFile->WriteInteger("MAIN","NumElement",(int)m_vecWallElement.size());

	for (int i=0;i<(int)m_vecWallElement.size();i++)
	{
		WallElement& element = m_vecWallElement[i];
		TCHAR szSectionName[MAX_PATH];
		wsprintf(szSectionName,"Element%d",i);

		pFile->WriteInteger(szSectionName,"eType",element.eType);
		pFile->WriteMultiFloat(szSectionName,"vConnectionPointA",(float*)&element.vConnectionPointA,3);
		pFile->WriteMultiFloat(szSectionName,"vConnectionPointB",(float*)&element.vConnectionPointB,3);
		pFile->WriteMultiFloat(szSectionName,"bboxA",(float*)&element.bbox.A,3);
		pFile->WriteMultiFloat(szSectionName,"bboxB",(float*)&element.bbox.B,3);
		pFile->WriteFloat(szSectionName,"fLength",element.fLength);
		pFile->WriteString(szSectionName,"ModelName",element.szModelFile.c_str());
	}

	pFile->Save(pFileName);

	hr =  S_OK;
Exit0:
	KG_COM_RELEASE(pFile);
	return hr;
}

int KG3DWallDefine::GetNumElement()
{
	return (int)m_vecWallElement.size();
}

HRESULT KG3DWallDefine::GetElement(int nIndex,KG3DWALLELEMENTTYPE* pType,D3DXVECTOR3* pA,D3DXVECTOR3* pB,LPSTR pFileName)
{
	KGLOG_PROCESS_ERROR(nIndex>=0 && nIndex<(int)m_vecWallElement.size());
	WallElement& element = m_vecWallElement[nIndex];

	(*pType) = element.eType;
	(*pA)    = element.vConnectionPointA;
	(*pB)    = element.vConnectionPointB;
	wsprintf(pFileName,"%s",element.szModelFile.c_str());

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DWallDefine::SetElement(int nIndex,KG3DWALLELEMENTTYPE eType,D3DXVECTOR3 vA,D3DXVECTOR3 vB,LPSTR pFileName,BOOL bComputeFromMesh)
{
	KGLOG_PROCESS_ERROR(nIndex>=0 && nIndex<(int)m_vecWallElement.size());
	WallElement& element = m_vecWallElement[nIndex];

	element.eType     = eType;
	element.vConnectionPointA = vA;
	element.vConnectionPointB = vB;

	element.szModelFile = pFileName;
	if(bComputeFromMesh)
	{
		element.GetDataFromFile();
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DWallDefine::NewOneElement(int* pIndex)
{
	WallElement element;
	element.eType = WALLELEMENT_WALL;
	element.fLength = 200;
	element.vConnectionPointA = D3DXVECTOR3(-100,0,0);
	element.vConnectionPointB = D3DXVECTOR3( 100,0,0);
	element.nIndex = (int)m_vecWallElement.size();

	m_vecWallElement.push_back(element);
	(*pIndex) = element.nIndex;

	return S_OK;
}

HRESULT KG3DWallDefine::DeleteElement(int nIndex)
{
	KGLOG_PROCESS_ERROR(nIndex>=0 && nIndex<(int)m_vecWallElement.size());
	{
		int K = 0;
		vector<WallElement>::iterator i = m_vecWallElement.begin();
		vector<WallElement>::iterator iend = m_vecWallElement.end();
		while (i!=iend)
		{
			if(K == nIndex)
			{
				m_vecWallElement.erase(i);
				return S_OK;
			}
			i++;
			K++;
		}
	}

Exit0:

	return E_FAIL;
}

HRESULT KG3DWallDefine::FindElement(vector<WallElement*>*pvecElement,KG3DWALLELEMENTTYPE eType)
{
	KGLOG_PROCESS_ERROR(pvecElement);
	pvecElement->clear();

	for (size_t i=0;i<m_vecWallElement.size();i++)
	{
		WallElement* pEle = &m_vecWallElement[i];
		if(pEle->eType == eType)
		{
			pvecElement->push_back(pEle);
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;

}

//////////////////////////////////////////////////////////////////////////
KG3DWallDefineTable::KG3DWallDefineTable()
{
	;
}

KG3DWallDefineTable::~KG3DWallDefineTable()
{
	UnInit();
}

HRESULT KG3DWallDefineTable::Init()
{
	return S_OK;
}

HRESULT KG3DWallDefineTable::UnInit()
{
	m_mapWallDefines.clear();

	list<KG3DWallDefine*>::iterator i = m_listWallDefines.begin();
	list<KG3DWallDefine*>::iterator iend = m_listWallDefines.end();
	while (i!=iend)
	{
		KG3DWallDefine* pDefine = *i;
		SAFE_DELETE(pDefine);

		i++;
	}
	m_listWallDefines.clear();
	return S_OK;
}

HRESULT KG3DWallDefineTable::LoadWallDefineFromFile(LPSTR pFileName,KG3DWallDefine** ppDefine)
{
	map<string,KG3DWallDefine*>::iterator i = m_mapWallDefines.find(pFileName);
	if (i == m_mapWallDefines.end())
	{
		KG3DWallDefine* pNewDef = new KG3DWallDefine;
		KGLOG_PROCESS_ERROR(pNewDef);

		pNewDef->AddRef();
		m_listWallDefines.push_back(pNewDef);
		m_mapWallDefines[pFileName] = pNewDef;
		(*ppDefine) = pNewDef;

		pNewDef->LoadFromFile(pFileName);
	}
	else
	{
		KG3DWallDefine* pDef = i->second;
		pDef->AddRef();

		(*ppDefine) = pDef;
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DWallDefineTable::DeleteWallDefine(KG3DWallDefine* pDefine)
{
	map<string,KG3DWallDefine*>::iterator i = m_mapWallDefines.begin();
	map<string,KG3DWallDefine*>::iterator iend = m_mapWallDefines.end();
	while (i!=iend)
	{
		KG3DWallDefine* pDef = i->second;
		if(pDef == pDefine)
		{
			i = m_mapWallDefines.erase(i);
		}
		else
		{
			i++;
		}
	}
	//////////////////////////////////////////////////////////////////////////
	m_listWallDefines.remove(pDefine);

	SAFE_DELETE(pDefine);

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
KG3DRepresentObjectWall::KG3DRepresentObjectWall(void)
{
	m_lpWallDefine = NULL;

	m_bOrthogonality = FALSE;//正交模式，墙体间相互垂直
	m_bLockXZ = FALSE;//统一水平高度
	m_bAutoPitch = FALSE;//自动倾斜墙体
	m_lpShareVB = NULL;
	m_dwWallModelTotalFaces = 0;
	m_bNeedRefreshVB = FALSE;
	m_lpDeclaration = NULL;
}

KG3DRepresentObjectWall::~KG3DRepresentObjectWall(void)
{
	UnInit();
}

HRESULT KG3DRepresentObjectWall::Init()
{
	HRESULT hr = S_OK;

	D3DVERTEXELEMENT9 Declaration2[] = 
	{
		{0, 0 , D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL  , 0},
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		D3DDECL_END()
	};
	hr = g_pd3dDevice->CreateVertexDeclaration(Declaration2, &m_lpDeclaration);
	KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
	return KG3DRepresentObject::Init();
}

HRESULT KG3DRepresentObjectWall::UnInit()
{
	m_bNeedRefreshVB = FALSE;
	SAFE_RELEASE(m_lpDeclaration);
	m_lpShareVB = NULL;

	SAFE_RELEASE(m_lpWallDefine);
	{
		for (size_t i=0;i<m_vecWall.size();i++)
		{
			Wall* pWall = m_vecWall[i];
			SAFE_DELETE(pWall);
		}
		m_vecWall.clear();
	}

	return KG3DRepresentObject::UnInit();
}

KG3DRepresentObjectWall::Wall::Wall()
{
	;
}

KG3DRepresentObjectWall::Wall::~Wall()
{
	for (size_t i=0;i<m_vecKeyPoints.size();i++)
	{
		KeyPoint* pPoint = m_vecKeyPoints[i];
		SAFE_DELETE(pPoint);
	}
	m_vecKeyPoints.clear();
	ClearLines();
}

HRESULT KG3DRepresentObjectWall::Wall::ClearLines()
{
	for (size_t i=0;i<m_vecLines.size();i++)
	{
		Line* pLine = m_vecLines[i];
		SAFE_DELETE(pLine);
	}
	m_vecLines.clear();
	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT KG3DRepresentObjectWall::Load(LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam)//这个用于加载附属的模型或者别的东西
{
	HRESULT hr = S_OK;
	m_szWallDefineFile = lpFileName;
	SAFE_RELEASE(m_lpWallDefine);

	hr = g_cWallDefineTable.LoadWallDefineFromFile((LPSTR)lpFileName,&m_lpWallDefine);
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectWall::Render(DWORD dwOption)
{
	D3DXVECTOR3 X = m_Translation + D3DXVECTOR3(0,1000,0);
	g_cGraphicsTool.DrawLine(&m_Translation,&X,0xFFFFFFFF,0xFF000000);

	//if(1)
	//{
	//	DWORD dwFace = 0;
	//	LPDIRECT3DVERTEXBUFFER9 pVB = GetShareVBuffer(dwFace);

	//	if(pVB && dwFace)
	//	{
	//		g_pd3dDevice->SetVertexDeclaration(m_lpDeclaration);
	//		g_pd3dDevice->SetStreamSource(0,pVB,0,sizeof(KG3DTerrainGrassVertex));
	//		g_pd3dDevice->DrawPrimitive(D3DPT_TRIANGLELIST,0,dwFace);
	//	}
	//}

    vector<KG3DModel*>vecModel;
    for (size_t i=0;i<m_vecWall.size();i++)
    {
        m_vecWall[i]->GetModels(m_dwWallModelTotalFaces,&vecModel);
    }
    for (size_t i = 0; i < vecModel.size(); ++i)
    {
        vecModel[i]->Render(0 , NULL);
    }

	return S_OK;
}

HRESULT KG3DRepresentObjectWall::ComputeBBox()
{
	m_BBox.Clear();

	D3DXVECTOR3 A = m_Translation + D3DXVECTOR3(-1000,-1000,-1000);
	D3DXVECTOR3 B = m_Translation + D3DXVECTOR3( 1000, 1000, 1000);
	m_BBox.AddPosition(A);
	m_BBox.AddPosition(B);

	return S_OK;
}

HRESULT KG3DRepresentObjectWall::Copy(KG3DRepresentObject* pSrcObject)
{
	KG3DRepresentObjectWall* pSrc = static_cast<KG3DRepresentObjectWall*>(pSrcObject);

	m_lpShareVB = NULL;
	m_szWallDefineFile = pSrc->m_szWallDefineFile;
	SAFE_RELEASE(m_lpWallDefine);
	m_lpWallDefine = pSrc->m_lpWallDefine;
	m_lpWallDefine->AddRef();

	KG3DRepresentObject::Copy(pSrcObject);
	ComputeBBox();

	return S_OK;
}

//////////////////////////////////////////////////////////////////////////
HRESULT KG3DRepresentObjectWall::SetOption(BOOL bOrthogonality,BOOL bLockXZ,BOOL bAutoPitch)
{
	m_bOrthogonality = bOrthogonality;
	m_bLockXZ = bLockXZ;
	m_bAutoPitch = bAutoPitch;

	return S_OK;
}

HRESULT KG3DRepresentObjectWall::GetOption(BOOL& bOrthogonality,BOOL& bLockXZ,BOOL& bAutoPitch)
{
	bOrthogonality = m_bOrthogonality;
	bLockXZ = m_bLockXZ;
	bAutoPitch = m_bAutoPitch;
	return S_OK;
}

int KG3DRepresentObjectWall::GetNumWall()
{
	return (int)m_vecWall.size();
}

HRESULT KG3DRepresentObjectWall::GetWall(int nID,int& nPoint,int& nLine)
{
	KGLOG_PROCESS_ERROR(nID>=0 && nID<(int)m_vecWall.size());
	Wall* pwall = m_vecWall[nID];

	nPoint = (int)pwall->m_vecKeyPoints.size();
	nLine  = (int)pwall->m_vecLines.size();

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectWall::GetWallLine(int nID,int nIndex,BOOL& bStartEnd, BOOL& bDoor)
{
	KGLOG_PROCESS_ERROR(nID>=0 && nID<(int)m_vecWall.size());
	Wall* pwall = m_vecWall[nID];
	KGLOG_PROCESS_ERROR(nIndex>=0 && nIndex<(int)pwall->m_vecLines.size());
	Line* pline = pwall->m_vecLines[nIndex];

	bStartEnd = pline->m_bStartEnd;
	bDoor     = pline->m_bDoor;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectWall::SetWallLine(int nID,int nIndex,BOOL  bStartEnd, BOOL  bDoor)
{
	KGLOG_PROCESS_ERROR(nID>=0 && nID<(int)m_vecWall.size());
	Wall* pwall = m_vecWall[nID];
	KGLOG_PROCESS_ERROR(nIndex>=0 && nIndex<(int)pwall->m_vecLines.size());
	Line* pline = pwall->m_vecLines[nIndex];

	pline->m_bStartEnd = bStartEnd;
	pline->m_bDoor     =	bDoor; 

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectWall::NewOneWall(int& nID)
{
	Wall* pnewwall= new Wall;
	m_vecWall.push_back(pnewwall);
	nID = (int)m_vecWall.size() - 1;
	return S_OK;
}

HRESULT KG3DRepresentObjectWall::DeleteOneWall(int nID)
{
	KGLOG_PROCESS_ERROR(nID>=0 && nID<(int)m_vecWall.size());

	{
		int K = 0;
		vector<Wall*>::iterator i = m_vecWall.begin();
		vector<Wall*>::iterator iend = m_vecWall.end();
		while (i!=iend)
		{
			if(K == nID)
			{
				Wall* pWall = *i;
				SAFE_DELETE(pWall);
				m_vecWall.erase(i);
				return S_OK;
			}
			i++;
			K++;
		}
	}

Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectWall::Fix()
{
	return S_OK;
}

HRESULT KG3DRepresentObjectWall::ModifyWallKeyPoint(KG3DRepresentObjectWall* pObjectWall,int nWallID,int nIndex,D3DXVECTOR3 vPosition)
{
	KGLOG_PROCESS_ERROR(nWallID>=0 && nWallID<(int)m_vecWall.size());
	Wall* pwall = m_vecWall[nWallID];
	BOOL bNewLine = FALSE;

	while(nIndex>=(int)pwall->m_vecKeyPoints.size())
	{
		KeyPoint* pNewPoint = new KeyPoint();
		pwall->m_vecKeyPoints.push_back(pNewPoint);
		bNewLine = TRUE;
	}

	KeyPoint* pPoint = pwall->m_vecKeyPoints[nIndex];

	pPoint->m_vPosition = vPosition;

	pPoint->ComputeModel(pObjectWall,pwall);

	if(bNewLine)
	{

		if(pwall->m_vecKeyPoints.size()>=2)
		{
			pwall->RefreshLines(this);
		}
	}

	if(nIndex == 0)
	{
		pwall->UpdateLine(0,pObjectWall);
	}
	else if(nIndex < (int)pwall->m_vecKeyPoints.size() - 1)
	{
		pwall->UpdateLine(nIndex - 1,pObjectWall);
		pwall->UpdateLine(nIndex    ,pObjectWall);
	}
	else
	{
		pwall->UpdateLine(nIndex - 1,pObjectWall);
	}

	m_bNeedRefreshVB = TRUE;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectWall::Wall::RenderLine()
{
	for (size_t i=0;i<m_vecKeyPoints.size();i++)
	{
		KeyPoint* pPoint = m_vecKeyPoints[i];
		if (pPoint->m_lpModel)
		{
			pPoint->m_lpModel->FrameMove(0,NULL);
			pPoint->m_lpModel->Render(0,NULL);
		}
	}

	for (size_t i=0;i<m_vecLines.size();i++)
	{
		Line* pline = m_vecLines[i];

		for (size_t s=0;s<pline->vecLineSection.size();s++)
		{
			LineSection* pSection = pline->vecLineSection[s];

			if(pSection->pModel)
			{
				pSection->pModel->FrameMove(0,NULL);

				pSection->pModel->Render(0,NULL);
			}
		}
	}

	return S_OK;
}

HRESULT KG3DRepresentObjectWall::Wall::RefreshLines(KG3DRepresentObjectWall* pWall)
{
	KGLOG_PROCESS_ERROR(m_vecKeyPoints.size()>=2);

	if((int)m_vecLines.size() < (int)m_vecKeyPoints.size() - 1)
	{
		int nA = (int)m_vecLines.size();
		int nB = nA + 1;
		Line*  pnewline = new Line;
		pnewline->m_nIndexKeyPointA = nA;
		pnewline->m_nIndexKeyPointB = nB;

		m_vecLines.push_back(pnewline);

		UpdateLine( nA ,pWall);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectWall::Wall::UpdateLine(int nIndex,KG3DRepresentObjectWall* pWall)
{
	KGLOG_PROCESS_ERROR(nIndex>=0 && nIndex<(int)m_vecLines.size());

	{
		Line* pline = m_vecLines[nIndex];

		D3DXVECTOR3 vA = m_vecKeyPoints[pline->m_nIndexKeyPointA]->m_vPosition;
		D3DXVECTOR3 vB = m_vecKeyPoints[pline->m_nIndexKeyPointB]->m_vPosition;
		D3DXVECTOR3 V = vB - vA;
		pline->m_fLength = D3DXVec3Length(&V);
		V.y = 0;
		pline->m_fLengthXZ = D3DXVec3Length(&V);

		pline->ComputeLineSection(pWall,this);
	}


	return S_OK;

Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectWall::Wall::GetModels(DWORD& dwFaceCount,vector<KG3DModel*>*pvecModel)
{
	KGLOG_PROCESS_ERROR(pvecModel);

	for (size_t i=0;i<m_vecKeyPoints.size();i++)
	{
		KeyPoint* pPoint = m_vecKeyPoints[i];
		if (pPoint->m_lpModel)
		{
			pPoint->m_lpModel->FrameMove(0,NULL);
			
			dwFaceCount += pPoint->m_lpModel->GetFaceNum();
			pvecModel->push_back(pPoint->m_lpModel);
		}
	}

	for (size_t i=0;i<m_vecLines.size();i++)
	{
		Line* pline = m_vecLines[i];

		for (size_t s=0;s<pline->vecLineSection.size();s++)
		{
			LineSection* pSection = pline->vecLineSection[s];

			if(pSection->pModel)
			{
				pSection->pModel->FrameMove(0,NULL);

				dwFaceCount += pSection->pModel->GetFaceNum();
				pvecModel->push_back(pSection->pModel);
			}
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}


KG3DRepresentObjectWall::Line::~Line()
{
	for (size_t i=0;i<vecLineSection.size();i++)
	{
		LineSection* pSection = vecLineSection[i];
		SAFE_DELETE(pSection);
	}
	vecLineSection.clear();
}

HRESULT KG3DRepresentObjectWall::Line::ComputeLineSection(KG3DRepresentObjectWall* pWall,Wall* pWallSection)
{
	vector<KG3DWallDefine::WallElement*>vecElement;	
	int nCount = 0;
	D3DXVECTOR3 vA,vB,V;
	
	KGLOG_PROCESS_ERROR(pWall);
	KGLOG_PROCESS_ERROR(pWall->m_lpWallDefine);
	{
		for (size_t i=0;i<vecLineSection.size();i++)
		{
			LineSection* pSection = vecLineSection[i];
			SAFE_DELETE(pSection);
		}
		vecLineSection.clear();
	}
	pWall->m_lpWallDefine->FindElement(&vecElement,WALLELEMENT_WALL);
	nCount = (int)vecElement.size();

	vA = pWallSection->m_vecKeyPoints[m_nIndexKeyPointA]->m_vPosition;
	vB = pWallSection->m_vecKeyPoints[m_nIndexKeyPointB]->m_vPosition;
	V  = vB - vA;
	D3DXVec3Normalize(&V,&V);

	KGLOG_PROCESS_ERROR(nCount);
	{
		float TotalLength = 0;
		while (TotalLength < m_fLength)
		{
			int n = rand() % nCount;

			KG3DWallDefine::WallElement* pElement = vecElement[n];
			KGLOG_PROCESS_ERROR(pElement);

			LineSection* pSection = new LineSection();
			pSection->nElementIndex = pElement->nIndex;
			pSection->vA.x = TotalLength;
			TotalLength += pElement->fLength;
			pSection->vB.x = TotalLength;

			vecLineSection.push_back(pSection);
		}

		float fScale = m_fLength / TotalLength;

		for (size_t i=0;i<vecLineSection.size();i++)
		{
			LineSection* pSection = vecLineSection[i];
			float fStart = pSection->vA.x * fScale;
			float fEnd   = pSection->vB.x * fScale;

			pSection->vA = vA + fStart * V;
			pSection->vB = vA + fEnd   * V;

			pSection->ComputeModel(pWall,pWallSection);
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

KG3DRepresentObjectWall::LineSection::LineSection()
{
	vA = D3DXVECTOR3(0,0,0);;
	vB = D3DXVECTOR3(0,0,0);
	pModel = NULL;
	D3DXMatrixIdentity(&mat);
	nElementIndex = -1;
}

KG3DRepresentObjectWall::LineSection::~LineSection()
{
	SAFE_RELEASE(pModel);
}

HRESULT KG3DRepresentObjectWall::LineSection::ComputeModel(KG3DRepresentObjectWall* pWall,Wall* pWallSection)
{
	KGLOG_PROCESS_ERROR(nElementIndex>=0);
	KGLOG_PROCESS_ERROR(pWall);
	KGLOG_PROCESS_ERROR(pWallSection);
	KGLOG_PROCESS_ERROR(pWall->m_lpWallDefine);

	SAFE_RELEASE(pModel);

	{
		KG3DWallDefine::WallElement* pElement= & pWall->m_lpWallDefine->m_vecWallElement[nElementIndex];

		float LenA = D3DXVec3Length(&pElement->vConnectionPointA);
		float LenB = D3DXVec3Length(&pElement->vConnectionPointB);
		float LenAll = LenA + LenB;

		D3DXVECTOR3 V = vB - vA;
		float LenSection = D3DXVec3Length(&V);
		D3DXVec3Normalize(&V,&V);

		float fScale = LenSection / LenAll;
		D3DXVECTOR3 vPosition = vA + LenA * fScale * V;

		HRESULT hr = g_cModelTable.LoadResourceFromFile(pElement->szModelFile.c_str(),0,0,(IKG3DResourceBase**)&pModel);
		KGLOG_COM_PROCESS_ERROR(hr);

		D3DXVECTOR2 v2D = D3DXVECTOR2(V.x,V.z);
		D3DXVec2Normalize(&v2D,&v2D);
		float fAngle = ::GetCircleAngle(v2D,D3DXVECTOR2(0,0));
		D3DXQUATERNION Rot;
		D3DXQuaternionRotationYawPitchRoll(&Rot,D3DX_PI * -0.5F + fAngle,0,0);

		if(pModel)
		{
			pModel->SetParentObjMatrix(NULL);
			pModel->SetTranslation(&vPosition);
			pModel->SetScaling(&D3DXVECTOR3(fScale,1,1));
			pModel->SetRotation(&Rot);
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectWall::KeyPoint::ComputeModel(KG3DRepresentObjectWall* pWall,Wall* pWallSection)
{
	KGLOG_PROCESS_ERROR(pWall);
	KGLOG_PROCESS_ERROR(pWallSection);
	KGLOG_PROCESS_ERROR(pWall->m_lpWallDefine);

	SAFE_RELEASE(m_lpModel);

	{
		vector<KG3DWallDefine::WallElement*>vecElement;	

		pWall->m_lpWallDefine->FindElement(&vecElement,WALLELEMENT_CORNER_ALL);
		int nCount = (int)vecElement.size();
		KGLOG_PROCESS_ERROR(nCount);

		int n = rand() % nCount;

		KG3DWallDefine::WallElement* pElement = vecElement[n];
		KGLOG_PROCESS_ERROR(pElement);

		HRESULT hr = g_cModelTable.LoadResourceFromFile(pElement->szModelFile.c_str(),0,0,(IKG3DResourceBase**)&m_lpModel);
		KGLOG_COM_PROCESS_ERROR(hr);

		if(m_lpModel)
		{
			m_lpModel->SetParentObjMatrix(NULL);
			m_lpModel->SetTranslation(&m_vPosition);
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectWall::CreateToShareVB()
{
	KGLOG_PROCESS_ERROR(m_lpShareVB);
	{
		m_dwWallModelTotalFaces = 0;

		vector<KG3DModel*>vecModel;
		for (size_t i=0;i<m_vecWall.size();i++)
		{
			m_vecWall[i]->GetModels(m_dwWallModelTotalFaces,&vecModel);
		}

		KG_PROCESS_ERROR(m_dwWallModelTotalFaces);
		m_lpShareVB->CreateFromModels(m_dwWallModelTotalFaces,&vecModel,this);
	}

	return S_OK;
Exit0:
	return E_FAIL;
}


HRESULT KG3DRepresentObjectWall::GetShareVB()
{
	//HRESULT hr = S_OK;
//	BOOL bNeedRefresh = FALSE;
	/*if(!m_lpShareVB)
	{
		g_cTerrainBlockGrassShareVBHighPool.GetOneShareResource(&m_lpShareVB,this);
		bNeedRefresh = TRUE;
	}
	else if(m_lpShareVB->pUser!=this)
	{
		g_cTerrainBlockGrassShareVBHighPool.GetOneShareResource(&m_lpShareVB,this);
		bNeedRefresh = TRUE;
	}

	if(bNeedRefresh||m_bNeedRefreshVB)
	{
		if(g_nGrassVertexBufferFillPerFrame <= 1 )
		{
			HRESULT hr = CreateToShareVB();
			KG_COM_PROCESS_ERROR(hr);

			m_bNeedRefreshVB = FALSE;
			g_nLoadCount ++;
			g_nGrassVertexBufferFillPerFrame++;
		}
		else
		{
			m_bNeedRefreshVB = TRUE;
		}
	}*/

	return E_FAIL;
}

LPDIRECT3DVERTEXBUFFER9 KG3DRepresentObjectWall::GetShareVBuffer(DWORD& dwFaces)
{
	HRESULT hr = S_OK;

	hr = GetShareVB();
	KG_COM_PROCESS_ERROR(hr);

	dwFaces = m_lpShareVB->m_dwUsedFaces;
	return m_lpShareVB->m_pVB;
Exit0:
	dwFaces = 0;
	return NULL;
}