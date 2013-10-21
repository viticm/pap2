#include "header.h"
#include "KG3DSK.h"

KG3DSK::KG3DSK(void)
{
}

KG3DSK::~KG3DSK(void)
{
	m_SK.clear();
	m_BlackListBones.clear();
}


int KG3DSK::FindBone(const TCHAR* strBoneName)
{
	int nReturn = -1;
	for(size_t i = 0; i < m_SK.size(); i++)
	{
		if(m_SK[i].dwIndex != 0xffffffff && 
		   !strcmp(strBoneName, m_SK[i].strBoneName))
		{
			nReturn = i;
			break;
		}
	}
	return nReturn;
}

void KG3DSK::FillChildInfo(TCHAR** ppInput, BoneInfoData& Data)
{
	int nIndex = -1;
	for (int i = 0; i < Data.dwNumChildBones; i++)
	{
		nIndex = FindBone(ppInput[i]);
		Data.ChildIndices[i] = nIndex;
		if (nIndex != -1)
		{
			m_SK[nIndex].dwParentIndex = Data.dwIndex;
		}
	}
}


int KG3DSK::GetBoneInfoFromString(const TCHAR* strInput, BoneInfoData& Data, TCHAR** ppChildBones)
{
	int nReturn = 0;
	int i = 0;
	int nLen = strlen(strInput);
	int nLast = 0;

	Data.dwNumChildBones = 0;

	if (!nLen)
		return -1;

	TCHAR* strBuffer = new TCHAR[nLen + 1];
	strcpy(strBuffer, strInput);
	strBuffer[nLen -1] = '\0';
	
	//Get Bone Name
	while (i < nLen)
	{
		if (strBuffer[i] == ' ' && strBuffer[i + 1] == ' ')
		{
			strBuffer[i] = '\0';
			//If bone is inside blacklist, do not insert and return directly
			if (m_BlackList.CheckBoneInList(strBuffer))
				nReturn = -1;
			strcpy(Data.strBoneName, strBuffer);
			strBuffer[i] = ' ';
			break;
		}
		i++;
	}
	
	//No child bones at all
	if (i == nLen)
	{
		if (m_BlackList.CheckBoneInList(strBuffer))
			nReturn = -1;
		strcpy(Data.strBoneName, strBuffer);
		return nReturn;
	}

	if (ppChildBones)
	{
		//Get children bones names
		
		while(i < nLen)
		{
			while(strBuffer[i] == ' ') i++;
			nLast = i;
			while( (strBuffer[i] != ' ' || strBuffer[i + 1] != ' ') && 
				   (i < nLen)) 
				i++;
			if (i < nLen)
			{
				strBuffer[i] = '\0';
				//if child bone is in black list, do not insert info
				strcpy(ppChildBones[Data.dwNumChildBones], strBuffer + nLast);
				Data.dwNumChildBones++;
				strBuffer[i] = ' ';
			}
			if (i == nLen)
			{
				strcpy(ppChildBones[Data.dwNumChildBones], strBuffer + nLast);
				Data.dwNumChildBones++;
			}
			i++;
		}
	}
	return nReturn;
}

TCHAR** KG3DSK::GetChildBoneNameArray()
{
	TCHAR** ppArray = new TCHAR*[MAX_NUM_CHILD_BONE];
	for(int i = 0; i < MAX_NUM_CHILD_BONE; i++)
	{
		ppArray[i] = new TCHAR[MAX_BONE_NAME_LENGTH];
	}
	return ppArray;
}

void KG3DSK::DeleteBoneNameArray(TCHAR** ppArray)
{
	for(int i = 0; i < MAX_NUM_CHILD_BONE; i++)
	{
		delete[] ppArray[i];
	}
	delete[] ppArray;
}

HRESULT KG3DSK::GetStandardSkeletonFromFile(const TCHAR* strFileName)
{
	FILE* fpFile = fopen(strFileName, "r");
	int nNumBones = 0;
	TCHAR strNumBones[256];
	TCHAR* strLineIn = NULL;
	std::vector<TCHAR**> ChildNames;
	
	static int nLineSize = (MAX_BONE_NAME_LENGTH + 1) * MAX_NUM_CHILD_BONE;
	if (!fpFile)
		return E_FAIL;
	
	//clear old data
	m_BlackListBones.clear();
	m_BlackList.GetBlackList(fpFile);
	

	fgets(strNumBones, 256, fpFile);
	sscanf(strNumBones, "%d", &nNumBones);
	if (!nNumBones)
		return E_FAIL;
	
	int nBufferSize = nLineSize * nNumBones;
	strLineIn = new TCHAR[nBufferSize];
	m_SK.resize(nNumBones);

	for (int i = 0; i < nNumBones; i++)
	{
		TCHAR** pChildren = GetChildBoneNameArray();
		m_SK[i].dwIndex = static_cast<DWORD>(i);
		ChildNames.push_back(pChildren);
		fgets(strLineIn + (nLineSize * i), nLineSize, fpFile);
		int n = GetBoneInfoFromString(strLineIn + (nLineSize * i), m_SK[i], pChildren);
		if (n == -1)
		{
			m_BlackListBones.push_back(i);
		}
	}

	for (int i = 0; i < nNumBones; i++)
	{
		FillChildInfo(ChildNames[i], m_SK[i]);
	}
	
	for (size_t j = 0; j < ChildNames.size(); j++)
	{
		DeleteBoneNameArray(ChildNames[j]);
	}
	ChildNames.clear();
	EliminateBlackBone();
	return S_OK;
}

void KG3DSK::EliminateBlackBone()
{
	//Must loop from back to forth
	for (int i = static_cast<int>(m_BlackListBones.size()) - 1; i >= 0; i--)
	{
		int j = m_BlackListBones[i];
		RelinkChildToParent(j);
	}
}

void KG3DSK::ClearUnusedBones()
{
	//do nothing
	//Deleted bones is identified by index 0xffffffff
}

void KG3DSK::RelinkChildToParent(int nIndex)
{
	//Cannot kill base bone
	DWORD k = 0;
	assert(nIndex);
	BoneInfoData& BoneToDel = m_SK[nIndex];
	BoneInfoData& Parent = m_SK[BoneToDel.dwParentIndex];
	assert(BoneToDel.dwIndex != BoneToDel.dwParentIndex);
	
	for (k = 0; k < Parent.dwNumChildBones; k++)
	{
		if (Parent.ChildIndices[k] == nIndex)
			break;
	}
	
	for (DWORD i = 0; i < BoneToDel.dwNumChildBones; i++)
	{
		BoneInfoData& Child = m_SK[BoneToDel.ChildIndices[i]];
		Child.dwParentIndex = Parent.dwIndex;
		Parent.ChildIndices[Parent.dwNumChildBones] = Child.dwIndex;
		assert(Parent.dwNumChildBones < MAX_NUM_CHILD_BONE);
		Parent.dwNumChildBones++;
	}

	//Delete bone index in parent bone's child indices
	Parent.ChildIndices[k] = Parent.ChildIndices[Parent.dwNumChildBones - 1];
	Parent.dwNumChildBones--;
	BoneToDel.dwIndex = 0xffffffff;
}

BOOL KG3DSK::CheckSKValidation(BOOL bWriteLog)
{
	FILE* fpFile = fopen("d:\\log.txt", "w");
	BOOL bReturn = TRUE;
	
	if (!fpFile)
		bWriteLog = FALSE;

	for (size_t i = 0; i < m_SK.size(); i++)
	{
		for (int j = 0; j < m_SK[i].dwNumChildBones; j++)
		{
			int k = m_SK[i].ChildIndices[j];
			if (m_SK[k].dwParentIndex != m_SK[i].dwIndex)
			{
				bReturn = FALSE;
				if (bWriteLog)
				{
					fprintf(fpFile, 
						    "Bone %s child bone %s link error.", 
							m_SK[i].strBoneName, 
							m_SK[j].strBoneName);
				}
			}
		}
	}
	fclose(fpFile);
	return bReturn;
}

void KG3DSK::OutputToFile(const TCHAR* strFileName)
{
	FILE* fpFile = fopen(strFileName, "w");
	int nNumDeprecated = 0;
	if (!fpFile)
		return;
	fprintf(fpFile, "Total num of bones: %d.\n", m_SK.size());
	for (size_t i = 0; i < m_SK.size(); i++)
	{
		BoneInfoData& Bone = m_SK[i];
		if (Bone.dwIndex == 0xffffffff)
		{
			fprintf(fpFile, "====Deprecated====");
			nNumDeprecated++;
		}
		fprintf(fpFile, "%s", Bone.strBoneName);
		
		for (DWORD j = 0; j < Bone.dwNumChildBones; j++)
		{
			fprintf(fpFile, "  %s", m_SK[Bone.ChildIndices[j]].strBoneName);
		}
		fprintf(fpFile, "\n");
	}

	fprintf(fpFile, "Num of deprecated: %d", nNumDeprecated); 
	fclose(fpFile);
}