#include "stdafx.h"
#include "KG3DModel.h"
#include "kg3dmodelst.h"
#include "kg3dboneset.h"
#include "kg3dbip.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DBoneSet::KG3DBoneSet()
{
}

KG3DBoneSet::~KG3DBoneSet()
{
	m_WeightTable.clear();
}

//HRESULT KG3DBoneSet::LoadFromFile(IFile* pFile)
//{
//	HRESULT hr = E_FAIL;
//	//if(pFile)
//	//{
//	//	BWTableHeader Header;
//	//	pFile->Read(&Header, sizeof(BWTableHeader));
//	//	if(Header.dwMask == m_dwID)//Is Bone weight table sector
//	//	{
//	//		m_WeightTable.clear();
//	//		BoneInfoItem bii;
//	//		TCHAR* pChildNames = new TCHAR[MAX_BONE_NAME * MAX_NUM_CHILD_BONE];				
//	//		for(DWORD i = 0; i < Header.dwNumBones; i++)
//	//		{
//	//			pFile->Read(&bii, sizeof(BoneInfoItem));
//	//			pFile->Read(pChildNames, sizeof(TCHAR) * MAX_BONE_NAME * bii.nNumChild);
//	//			BoneInfo bi(bii.strParentBoneName, bii.fWeight, pChildNames, bii.nNumChild);
//	//			m_WeightTable[bii.strBoneName] = bi;
//	//		}
//	//		SAFE_DELETE_ARRAY(pChildNames);
//	//		hr = S_OK;
//	//	}
//	//}
//	return hr;
//}

DWORD KG3DBoneSet::GetNumBones()
{
	return static_cast<DWORD>(m_WeightTable.size());
}

HRESULT KG3DBoneSet::CreateModel(KG3DModel* pModel)
{
	HRESULT hr = E_FAIL;
	KG3DMesh*     pMesh     = NULL;
	int nNumBones = 0;
	int nBoneIndex = 0;
	int nNumChildBones = 0;
	int nChildBoneIndex = 0;
	int nChildBoneID = 0;
	
	m_WeightTable.clear();
	m_MapTable.clear();

	KG_PROCESS_ERROR(pModel);

	pMesh = pModel->GetMesh();
	KG_PROCESS_ERROR(NULL != pMesh);

	nNumBones = static_cast<int>(pMesh->m_dwNumBone);
	
	m_WeightTable.resize(nNumBones);

	for (nBoneIndex = 0; nBoneIndex < nNumBones; nBoneIndex++)
	{
		BoneInfoItem& Item = m_WeightTable[nBoneIndex];
		Item.strName = pMesh->m_pBoneInfo[nBoneIndex].strBoneName;
		Item.nParentIndex = static_cast<int>(pMesh->m_pBoneInfo[nBoneIndex].dwParentIndex);
		Item.nBoneIndex = nBoneIndex;
		
		nNumChildBones = static_cast<int>(pMesh->m_pBoneInfo[nBoneIndex].dwNumChild);
		for (nChildBoneIndex = 0; 
			 nChildBoneIndex < nNumChildBones;
			 nChildBoneIndex++)
		{
			nChildBoneID = static_cast<int>(pMesh->m_pBoneInfo[nBoneIndex].dwChildIndex[nChildBoneIndex]);
			Item.ChildIndices.push_back(nChildBoneID);
		}
		m_MapTable[Item.strName] = nBoneIndex;
	}
Exit0:
	//SAFE_RELEASE(pMesh);//原来是带引用计数的，现在不带
	return hr;
}

HRESULT KG3DBoneSet::Create(KG3DModel *pModel)
{
    HRESULT hr = E_FAIL;
    switch (pModel->GetType())
    {
    case MESHTYPE_DEFAULT:
        hr = CreateModel(pModel);
        break;
    case MESHTYPE_MODELST:
        hr = CreateModelST(pModel);
        break;
    }

    //Exit0:
    return hr;
}

HRESULT KG3DBoneSet::CreateModelST(KG3DModel* pModel)
{
	HRESULT hr = E_FAIL;
	const KG3DBip* pBip = NULL;

	int nNumBones = 0;
	int nBoneIndex = 0;
	int nNumChildBones = 0;
	int nChildBoneIndex = 0;
	int nChildBoneID = 0;
	KG3DModelST *pModelST = static_cast<KG3DModelST*>(pModel);

	m_WeightTable.clear();
	m_MapTable.clear();

	KG_PROCESS_ERROR(pModelST);

	pBip = pModelST->GetBip();
	KG_PROCESS_ERROR(pBip);
	
	nNumBones = pBip->GetNumBones();
	
	m_WeightTable.resize(nNumBones);

	for (nBoneIndex = 0; nBoneIndex < nNumBones; nBoneIndex++)
	{
		BoneInfoItem& Item = m_WeightTable[nBoneIndex];
		const TCHAR* strBoneName = pBip->GetBoneName(nBoneIndex);
		if (strBoneName)
		{
			Item.strName = strBoneName;
		}
		else
		{
			Item.strName = "Unknown";
		}

		Item.nParentIndex = pBip->GetParentIndex(nBoneIndex);
		Item.nBoneIndex = nBoneIndex;
		
		nNumChildBones = pBip->GetNumChildren(nBoneIndex);
		for (nChildBoneIndex = 0; 
			 nChildBoneIndex < nNumChildBones;
			 nChildBoneIndex++)
		{
			nChildBoneID = pBip->GetChildIndex(nBoneIndex, nChildBoneIndex);
			Item.ChildIndices.push_back(nChildBoneID);
		}
		m_MapTable[Item.strName] = nBoneIndex;
	}
	hr = S_OK;
Exit0:
	return hr;
}

float KG3DBoneSet::GetBoneWeight(int nBoneIndex)
{
	float fReturn = 0.0f;
	
	if (nBoneIndex >= static_cast<int>(m_WeightTable.size()))
		goto Exit0;

	fReturn = m_WeightTable[nBoneIndex].fWeight;
Exit0:
	return fReturn;
}

float KG3DBoneSet::GetBoneWeight(const TCHAR* strBoneName)
{
	float fReturn = 0.0f;
	MapTable_it i = m_MapTable.find(strBoneName);
	if (i == m_MapTable.end())
		goto Exit0;
	
	//fReturn = GetBoneWeight(i->second);
	
Exit0:
	return fReturn;
}

void KG3DBoneSet::Clone(const KG3DBoneSet& SrcBoneSet)
{
	if (&SrcBoneSet == this)
		goto Exit0;

Exit0:
	return;
}

void KG3DBoneSet::SetBoneWeight(int nBoneIndex, float fWeight, BOOL bInherit)
{
	int nNumBones = static_cast<int>(m_WeightTable.size());
	int nIndex = 0;
	int nNumChildren = 0;
	if (nBoneIndex >= nNumBones ||
		nBoneIndex < 0 )
		goto Exit0;
	
	BoneInfoItem& Item = m_WeightTable[nBoneIndex];
	//Item.fWeight = fWeight;
	if (bInherit)
	{
		nNumChildren = static_cast<int>(Item.ChildIndices.size());
		for (nIndex = 0; nIndex < nNumChildren; nIndex++)
		{
			SetBoneWeight(Item.ChildIndices[nIndex], fWeight, bInherit);
		}
	}
Exit0:
	return;
}

void KG3DBoneSet::SetBoneWeight(const TCHAR* strBoneName, float fWeight, BOOL bInherit)
{
	MapTable_it i = m_MapTable.find(strBoneName);
	if (i == m_MapTable.end())
		goto Exit0;
	SetBoneWeight(i->second, fWeight, bInherit);
Exit0:
	return;
}

void KG3DBoneSet::GetChildInfo(int nIndex, const std::vector<int>** ppChildren)
{
	int nNumItem = static_cast<int>(m_WeightTable.size());
	*ppChildren = NULL;
	
	if (nIndex >= nNumItem)
		goto Exit0;
	
	BoneInfoItem& Item = m_WeightTable[nIndex];
	
	*ppChildren = &(Item.ChildIndices);
Exit0:
	return;
}

void KG3DBoneSet::GetChildInfo(const TCHAR* strBoneName, const std::vector<int>** ppChildren)
{
	MapTable_it i = m_MapTable.find(strBoneName);
	
	if (i == m_MapTable.end())
		goto Exit0;
	
	GetChildInfo(i->second, ppChildren);

Exit0:
	return;
}
HRESULT KG3DBoneSet::LoadFromFile(const TCHAR *strFileName)
{
    HRESULT hr = E_FAIL;
    KG_PROCESS_ERROR(strFileName);


    hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DBoneSet::SaveToFile(const TCHAR *strFileName)
{
    HRESULT hr = E_FAIL;

    hr = S_OK;
    //Exit0:
    return hr;
}



void KG3DBoneSet::SetCollector(KG3DAnimationUpdateExtraInfoCollector *pCollector)
{
    //	if (m_pCollector)
    //	{
    //		//m_pCollector->Clear();
    //		m_pCollector = NULL;
    //	}
    //	
    //	KG_PROCESS_ERROR(pCollector);
    //	pCollector->SetSize(m_WeightTable.size());
    //
    //	for (DWORD i = 0; i < m_WeightTable.size(); i++)
    //	{
    //		pCollector->SetStore(i, static_cast<void*>(&m_WeightTable[i].rts));
    //	}
    //
    //Exit0:
    //	return;
}

void KG3DBoneSet::GetBoneMatrix(int nIndex, D3DXMATRIX& matResult)
{

    static D3DXMATRIX matIdentity;
    static BOOL bInited = FALSE;
    if (!bInited)
    {
        D3DXMatrixIdentity(&matIdentity);
        bInited = TRUE;
    }

//#ifdef _DEBUG
//    KG_PROCESS_ERROR(nIndex < static_cast<int>(m_WeightTable.size()));
//#endif

    //RTS2Matrix(matResult, m_WeightTable[nIndex].rts, m_WeightTable[nIndex].dwRTSFlag);


    //Should be changed.....
    //if (m_WeightTable[nIndex].fWeight != 0.0f)
 //   {
        //matResult *= m_WeightTable[nIndex].fWeight;
 //   }
    //else
 //   {
        //	matResult = matIdentity;
  //  }
//Exit0:
	return;
}

KG3DBoneSet::BoneInfoItem::BoneInfoItem()
{
    //dwRTSFlag = NORMAL_RTS;
}
