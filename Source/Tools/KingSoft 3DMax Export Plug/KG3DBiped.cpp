#include "header.h"
#include "kg3dsk.h"
#include "KG3DBiped.h"
#include "KG3DRTSAnimation.h"


KG3DBiped::KG3DBiped(void)
{
	m_bStandardBipeLoaded = FALSE;
	m_ip = NULL;
}

KG3DBiped::~KG3DBiped(void)
{
	m_Skeleton.clear();
	m_BaseBones.clear();
}


BoneInfoData::BoneInfoData()
{
	memset(ChildIndices, -1, sizeof(DWORD) * MAX_NUM_CHILD_BONE);
	memset(strBoneName, 0, sizeof(TCHAR) * MAX_BONE_NAME_LENGTH);
	dwParentIndex = 0;
	dwNumChildBones = 0;
}

void KG3DBiped::BoneInfo::FillBoneInfo(INode *pNode, KG3DRelinkTable *pRelinkTable)
{
	INode* pParentNode = NULL;
	int nIndex = 0;
	assert(pNode);

	if(pRelinkTable)
	{
		pParentNode = pRelinkTable->GetParentNode(pNode);
	}
	else
	{
		pParentNode = pNode->GetParentNode();
	}
}



INode* KG3DBiped::FindBone(const TCHAR* strInBoneName)
{
	INode* pReturn = NULL;
	for (size_t i = 0; i < m_Skeleton.size(); i++)
	{
		TSTR strBoneName = m_Skeleton[i]->NodeName();
		if (!strcmp(strBoneName, strInBoneName))
		{
			pReturn = m_Skeleton[i];
			break;
		}
	}
	return pReturn;
}




HRESULT KG3DBiped::SaveSkeletonToFile(const TCHAR* strFileName)
{
	HRESULT hr = E_NOTIMPL;
	
	return hr;
}


HRESULT KG3DBiped::GenerateSkeletonFromNode(INode* pNode, BOOL bBipOnly)
{
	HRESULT hr         = E_FAIL;
	INode* pRoot       = m_ip->GetRootNode();
	INode* pParentNode = pNode;
	//m_Skeleton.clear();
	TSTR strBoneName = pNode->NodeName();
	//Already in.
	if (!strcmp(strBoneName, "Bone14"))
	{
		int nDebug = 0;
	}
	if (FindBone(strBoneName))
		return S_OK;

	while(pParentNode != pRoot)
	{
		pNode = pParentNode;
		strBoneName = pNode->NodeName();
		pParentNode = pNode->GetParentNode();
	};
	
	/*if (pNode == pRoot)
		return E_FAIL;*/

	m_Skeleton.push_back(pNode);
	m_BaseBones.push_back(m_Skeleton.size() - 1);
	
	int nNumChildren = pNode->NumberOfChildren();
	int nLastInsert = 1;
	while(nNumChildren)
	{
		size_t uNumBones = m_Skeleton.size();
		nNumChildren = 0;
		for (int i = nLastInsert; i > 0; i--)
		{
			nNumChildren += InsertChildBone(m_Skeleton[uNumBones - i]);
		}
		nLastInsert = nNumChildren;
	}

	
	return S_OK;
}



int KG3DBiped::InsertChildBone(INode* pNode)
{
	INode* pChild = NULL;
	int nNumChildren = pNode->NumberOfChildren();
	
	for (int i = 0; i < nNumChildren; i++)
	{
		pChild = pNode->GetChildNode(i);
		m_Skeleton.push_back(pChild);
	}
	return nNumChildren;
}

void KG3DBiped::OutputBipedText(const TCHAR* strFilename)
{
	FILE* fpFile = fopen(strFilename, "w");
	size_t uNumBones = m_Skeleton.size();
	if (!fpFile)
		return;

	fprintf(fpFile, "%d\n", uNumBones);
	for (size_t i = 0;  i < uNumBones; i++)
	{
		INode* pCurNode = m_Skeleton[i];
		TSTR strBoneName = pCurNode->NodeName();
		fprintf(fpFile, "%s", strBoneName);
		int nNumChildren = GetNumChildren(pCurNode);
		for(int j = 0; j < nNumChildren; j++)
		{
			TSTR strChildName = GetChildNode(pCurNode, j)->NodeName();
			fprintf(fpFile, "  %s", strChildName);
		}
		fprintf(fpFile, "\n");
	}

	fprintf(fpFile, "%d\n", m_BaseBones.size());
	for (size_t j = 0; j < m_BaseBones.size(); j++)
	{
		fprintf(fpFile, "%d\n", m_BaseBones[j]);
	}
	fclose(fpFile);
}


HRESULT KG3DBiped::LoadConfigFile(const TCHAR* strFileName)
{
	HRESULT hr = E_FAIL;
	FILE* fpConfig = fopen(strFileName, "r");
	
	m_RelinkTable.CleanUp();
	m_BlackList.CleanUp();

	if (!fpConfig)
		return E_FAIL;

	
	//read blacklist
	m_BlackList.GetBlackList(fpConfig);
	
	//read relink table
	m_RelinkTable.LoadFromFile(fpConfig);
	fclose(fpConfig);

	m_RelinkTable.AjustWithSkeleton(this);
	ApplyBlackList();
	
	for(size_t i = 0; i < m_BaseBones.size(); i++)
	{
		m_RelinkTable.SetLink(m_Skeleton[m_BaseBones[i]], NULL);
	}

	return S_OK;
}


INode* KG3DBiped::GetParentNode(INode* pNode)
{
	return m_RelinkTable.GetParentNode(pNode);
}

int KG3DBiped::GetNumChildren(INode* pNode)
{
	return m_RelinkTable.GetNumChildren(pNode);
}

INode* KG3DBiped::GetChildNode(INode* pNode, int i)
{
	return m_RelinkTable.GetChildNode(pNode, i);
}

void KG3DBiped::ApplyBlackList()
{
	for (size_t i = 0; i < m_Skeleton.size(); i++)
	{
		INode* pCurNode = m_Skeleton[i];
		TSTR strBoneName = pCurNode->NodeName();
		if (m_BlackList.CheckBoneInList(strBoneName))
		{
			EraseBone(i);
			i--;
		}
	}
}
int KG3DBiped::GetBoneIndex(const INode* pNode)
{
	int nReturn = -1;
	for (size_t i = 0; i < m_Skeleton.size(); i++)
	{
		if (pNode == m_Skeleton[i])
		{
			nReturn = static_cast<int>(i);
			break;
		}
	}
	return nReturn;
}

void KG3DBiped::EraseBone(int nIndex)
{
	INode* pNode = m_Skeleton[nIndex];
	INode* pParent = GetParentNode(pNode);
	int nNumChildren = GetNumChildren(pNode);
	vector<INode*> pChildren;
	vector<int> ChildrenIndex;
	for (int i = 0; i < nNumChildren; i++)
	{
		INode* pChild = GetChildNode(pNode, i);
		pChildren.push_back(pChild);
	}
	for (size_t i = 0; i < pChildren.size(); i++)
	{
		m_RelinkTable.SetLink(pChildren[i], pParent);
	}
	m_RelinkTable.EraseLink(pNode);
	
	//Fix base bone links
	for (int i = 0; i < m_BaseBones.size(); i++)
	{
		if(m_BaseBones[i] > nIndex)
		{
			m_BaseBones[i]--;
		}
		else if(m_BaseBones[i] == nIndex)
		{
			int nTemp = m_BaseBones[m_BaseBones.size() - 1];
			m_BaseBones[m_BaseBones.size() - 1] = nIndex;
			if (nTemp > nIndex)
			{
				nTemp--;
			}
			m_BaseBones[i] = nTemp;
			m_BaseBones.pop_back();
			
		}
	}
	std::vector<INode*>::iterator j = m_Skeleton.begin();
	advance(j, nIndex);
	m_Skeleton.erase(j);
	
	if (!m_ip)
		return;
	if (m_ip->GetRootNode() == pParent)
	{
		for (size_t i = 0 ; i < pChildren.size(); i++)
		{
			int nIndex = GetBoneIndex(pChildren[i]);
			if (nIndex != -1)
			{
				ChildrenIndex.push_back(nIndex);
			}
		}

		//需要增加这些骨骼为根骨骼
		for (size_t i = 0; i < ChildrenIndex.size(); i++)
		{
			m_BaseBones.push_back(ChildrenIndex[i]);
		}
	}
	
}

int KG3DBiped::GetNumNodes()
{
	return static_cast<int>(m_Skeleton.size());
}

INode* KG3DBiped::GetNode(unsigned int nIndex)
{
	return m_Skeleton[nIndex];
}

void* KG3DBiped::GetExtraInfo()
{
	return static_cast<void*>(&m_RelinkTable);
}


HRESULT KG3DBiped::ExportRTSAnimation(const TCHAR* strFileName)
{
	HRESULT hr = E_FAIL;
	Interval AniRange = m_ip->GetAnimRange();
	TimeValue FrameStep = GetTicksPerFrame() * GetExportOption()->m_nKeyFrameStep;
	int nNumFrame = AniRange.Duration() / FrameStep + 1;
	TimeValue CurrentTime = AniRange.Start();
	KG3DRTSAnimation RTSExporter;
	RTSExporter.Attach(this);
	for(int i = 0; i < nNumFrame; i++)
	{
		RTSExporter.GetFrameData(CurrentTime);
		CurrentTime += FrameStep;
	}

	hr = RTSExporter.SaveToFile(strFileName);
	return hr;
}


HRESULT KG3DBiped::GenerateSkeletonFromMesh(CMesh* pMesh)
{
	HRESULT hr = E_FAIL;
	
	if (!pMesh)
		return E_FAIL;

	m_Skeleton.clear();
	m_BaseBones.clear();
	m_RelinkTable.CleanUp();

	for(DWORD i = 0; i < pMesh->m_dNumBone; i++)
	{
		CMesh::_BoneInfo& Bone = pMesh->m_lpBoneInfo[i];
		if (!strcmp(Bone.szParentName, "Scene Root"))
		{
			m_BaseBones.push_back(i);
		}
		INode* pNode = m_ip->GetINodeByName(Bone.szName);
		assert(pNode);
		m_Skeleton.push_back(pNode);

		INode* pNodeParent = pNode->GetParentNode();
		if (pNode == m_ip->GetRootNode())
			continue;

		TSTR strParentName = pNodeParent->NodeName();
		//If parent node in mesh is not equal to node parent, 
		//set relink table.
		if (strcmp(strParentName, Bone.szParentName))
		{
			pNodeParent = m_ip->GetINodeByName(Bone.szParentName);
			m_RelinkTable.SetLink(pNode, pNodeParent);
		}
	}
	return S_OK;
}
