#include "stdafx.h"
#include "kg3dbonesetex.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DBoneSetEx::KG3DBoneSetEx(void)
{
}

KG3DBoneSetEx::~KG3DBoneSetEx(void)
{
	m_WeightTable.clear();
}


HRESULT KG3DBoneSetEx::SetBoneWeight(const BONE_ID& BoneID, float fWeight, DWORD dwOption)
{
	HRESULT hr = E_FAIL;
	//m_WeightTable[BoneID].fWeight = fWeight;
	//list<BONE_ID>::iterator it = m_WeightTable[BoneID].ChildBones.begin();
	//while(it!=m_WeightTable[BoneID].ChildBones.end())
	//{
	//	//目前的做法是， 如果指定的骨骼有子骨骼，那么将所有的子骨骼的权重都乘上指定的权重
	//	SetBoneWeight(*it, fWeight, 0);
	//	it++;
	//}
	return hr;
}

HRESULT KG3DBoneSetEx::SetBonesWeight(BONE_ID* pBoneID, float fWeight, DWORD dwNumBones)
{
	HRESULT hr = E_FAIL;
	/*for(DWORD i = 0; i < dwNumBones; i++)
	{
		m_WeightTable[pBoneID[i]].fWeight = fWeight;
	}*/
	return hr;
}

//////////////////////////////////////////////////////////////////////////
//Desc: 本函数用来把所有骨骼的权重都设置为制定的值
void KG3DBoneSetEx::NormalizeTable(float fWeight)
{
	/*stdext::hash_map<BONE_ID, BoneInfo>::iterator it = m_WeightTable.begin();
	while (it != m_WeightTable.end())
	{
		it->second.fWeight = fWeight;
		it++;
	}*/
}

HRESULT KG3DBoneSetEx::AddBone(const BONE_ID& BoneID, const BONE_ID& ParentBoneID /* =""*/, float fWeight /*= 1.0f*/)
{
	HRESULT hr = E_FAIL;
	/*if (!m_WeightTable.count(BoneID))
	{
		BoneInfo bf;
		bf.ParentBone = ParentBoneID;
		bf.fWeight = fWeight;
		m_WeightTable[BoneID] = bf;
		hr = S_OK;
	}*/
	return hr;
}

void KG3DBoneSetEx::RemoveBone(const BONE_ID& BoneID, DWORD dwOption, BOOL bRemoveChild /*= TRUE*/)
{
	//if (BoneID != string("") && m_WeightTable.count(BoneID))
	//{
	//	if(bRemoveChild)
	//	{
	//		for(list<BONE_ID>::iterator i = m_WeightTable[BoneID].ChildBones.begin(); i != m_WeightTable[BoneID].ChildBones.end(); i++)
	//		{
	//			RemoveBone(*i, 0);
	//		}
	//	}
	//	else
	//	{
	//		//如果不删除子骨骼,那么就把他们连接到要删除的骨骼的父骨骼上
	//		BONE_ID NewParent = m_WeightTable[BoneID].ParentBone;
	//		for(list<BONE_ID>::iterator i = m_WeightTable[BoneID].ChildBones.begin(); i != m_WeightTable[BoneID].ChildBones.end(); i++)
	//		{
	//			m_WeightTable[*i].ParentBone = NewParent;
	//		}
	//	}
	//	m_WeightTable.erase(BoneID);
	//}
}

HRESULT KG3DBoneSetEx::SaveToFile(FILE* pFile)
{
	HRESULT hr = E_FAIL;
	/*if( pFile )
	{
		BWTableHeader Header = { m_dwID, 0, static_cast<DWORD>(m_WeightTable.size()) };
		TCHAR* pChildNames = new TCHAR[MAX_BONE_NAME * MAX_NUM_CHILD_BONE];
		int k = 0;
		memset(&(Header.dwExtendBlock), 0, sizeof(DWORD) * 10);
		fwrite(&Header, sizeof(BWTableHeader), 1, pFile);
		for(stdext::hash_map<BONE_ID, BoneInfo>::iterator i = m_WeightTable.begin();
			i != m_WeightTable.end(); i++)
		{
			k = 0;
			BoneInfoItem bii;
			strcpy(bii.strBoneName, i->first.c_str());
			strcpy(bii.strParentBoneName, i->second.ParentBone.c_str());
			bii.fWeight = i->second.fWeight;
			bii.nNumChild = static_cast<int>(i->second.ChildBones.size());
			for(list<BONE_ID>::iterator j = i->second.ChildBones.begin();
				j != i->second.ChildBones.end(); j++, k++)
			{
				strcpy( pChildNames + MAX_BONE_NAME * k, j->c_str() );
			}
			fwrite(&bii, sizeof(BoneInfoItem), 1, pFile);
			fwrite(&pChildNames, sizeof(TCHAR), MAX_BONE_NAME * bii.nNumChild, pFile);
		}
		SAFE_DELETE_ARRAY(pChildNames);
		hr = S_OK;
	}*/
	return hr;
}

//////////////////////////////////////////////////////////////////////////
//Desc: 本函数用来检查多个BoneWeightTable的权重和是否是1
HRESULT KG3DBoneSetEx::CheckSum(KG3DBoneSetEx* pTables, DWORD dwNumTables, BONE_ID* pErrorBones, DWORD& dwNumErrBones)
{
	/*if (!pTables || dwNumTables<= 0)
		return E_FAIL;
	std::set<BONE_ID> BoneChecked;
	DWORD dwMaxNumBone = 0;
	dwNumErrBones = 0;
	stdext::hash_map<BONE_ID, BoneInfo>::iterator it = m_WeightTable.begin();
	float fWeight1;
	while (it != m_WeightTable.end())
	{
		float fWeight = it->second.fWeight;
		BoneChecked.insert(it->first);
		for(DWORD i = 0; i < dwNumTables; i++)
		{
			if (SUCCEEDED(pTables[i].GetBoneWeight(it->first, fWeight1))) 
				fWeight += fWeight1;
		}
		if(abs(fWeight - 1.0f) > 0.00001f)
		{
			if(pErrorBones)
				pErrorBones[dwNumErrBones++] = it->first;
			hr = E_FAIL;
		}
		it++;
	}
	for(DWORD i = 0; i < dwNumTables; i++)
		dwMaxNumBone = dwMaxNumBone > pTables[i].GetNumBones()? dwMaxNumBone : pTables[i].GetNumBones();
	BONE_ID* pBones = new BONE_ID[dwMaxNumBone];
	for(DWORD i = 0; i < dwNumTables; i++)
	{
		pTables[i].GetAllBones(pBones);
		for(DWORD j = 0; j < pTables[i].GetNumBones(); j++)
		{
			if( !BoneChecked.count(pBones[j]) )
			{
				BoneChecked.insert(pBones[j]);
				float fWeight;
				pTables[i].GetBoneWeight(pBones[j], fWeight);
				for(DWORD k = i + 1; k < dwNumTables; k++)
				{
					float fWeight1;
					if(SUCCEEDED( pTables[k].GetBoneWeight(pBones[j], fWeight1)))
					{
						fWeight += fWeight1;
					}
				}
				if(abs(fWeight - 1.0f) >0.00001f)
				{
					if(pErrorBones)
						pErrorBones[dwNumErrBones++] = it->first;
					hr = E_FAIL;
				}
			}
		}
	}*/
	return S_OK;
}

void KG3DBoneSetEx::GetAllBones(BONE_ID* pBoneID)
{
	/*if(pBoneID)
	{
		DWORD dwIndex = 0;
		stdext::hash_map<BONE_ID, BoneInfo>::iterator it = m_WeightTable.begin();
		while (it != m_WeightTable.end())
		{
			pBoneID[dwIndex] = it->first;
			it++;
		}
	}*/
}

//Desc: 本函数用于对指定的骨骼信息， 自动生成对应的权重表，并将所有权重设置为制定的值
HRESULT KG3DBoneSetEx::GenerateSekeletonFromMesh(const KG3DMesh* pMesh, float fDefaultWeight/* = 1.0f*/)
{
	HRESULT hr = E_FAIL;
	/*m_WeightTable.clear();
	if(pMesh && pMesh->m_pBoneInfo)
	{
		for(DWORD i = 0; i < pMesh->m_dwNumBone; i++ )
		{
			KG3DMesh::_BoneInfo& BoneInfo = pMesh->m_pBoneInfo[i];
			AddBone(BoneInfo.scName, BoneInfo.scParentName, fDefaultWeight);
		}
		hr = S_OK;
	}*/
	return hr;
}

void KG3DBoneSetEx::SetBoneWeightWithChild(const BONE_ID& BoneID, float fWeight)
{
	/*m_WeightTable[BoneID].fWeight = fWeight;
	for(list<BONE_ID>::iterator i = m_WeightTable[BoneID].ChildBones.begin();
		i != m_WeightTable[BoneID].ChildBones.end(); i++)
	{
		SetBoneWeightWithChild(*i, fWeight * m_WeightTable[*i].fWeight);
	}*/
}

void KG3DBoneSetEx::AttachToBone(const BONE_ID& BoneSrc, const BONE_ID& BoneDest)
{
	/*if(m_WeightTable.count(BoneSrc) && m_WeightTable.count(BoneDest))
	{
		m_WeightTable[BoneSrc].ParentBone = BoneDest;
		m_WeightTable[BoneDest].ChildBones.push_back(BoneSrc);
	}*/
}

void KG3DBoneSetEx::Detach(const BONE_ID& Bone)
{
	//if(m_WeightTable.count(Bone))
	//{
	//	list<BONE_ID>& ChildBones = m_WeightTable[m_WeightTable[Bone].ParentBone].ChildBones;
	//	list<BONE_ID>::iterator it = find(ChildBones.begin(), ChildBones.end(), Bone);
	//	if ( it != ChildBones.end() )
	//	{
	//		ChildBones.erase(it);
	//	}
	//	AttachToBone(Bone, string(""));//Root骨骼认为是""
	//}
}