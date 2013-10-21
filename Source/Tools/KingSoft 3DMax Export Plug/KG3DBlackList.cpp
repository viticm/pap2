#include "header.h"
#include "KG3DBlackList.h"

KG3DBlackList::KG3DBlackList(void)
{
}

KG3DBlackList::~KG3DBlackList(void)
{
	CleanUp();
}

void KG3DBlackList::CleanUp()
{
	m_BlackList.clear();
}

void KG3DBlackList::GetBlackList(FILE* fpFile)
{
	TCHAR strInput[MAX_BONE_NAME_LENGTH + 1];
	int nNumItem = 0;
	m_BlackList.clear();

	fgets(strInput, MAX_BONE_NAME_LENGTH + 1, fpFile);
	sscanf(strInput, "%d", &nNumItem);
	
	for (int i = 0; i < nNumItem; i++)
	{
		fgets(strInput, MAX_BONE_NAME_LENGTH + 1, fpFile);
		size_t nLen = strlen(strInput);
		if (strInput[nLen - 1] == 0x0a)
		{
			//eliminate newline character at the end of the line
			strInput[nLen - 1] = '\0';
		}
		m_BlackList.push_back(strInput);
	}
}

BOOL KG3DBlackList::CheckBoneInList(const TCHAR* strBoneName)
{
	BOOL bReturn = FALSE;
	TCHAR strName[MAX_BONE_NAME_LENGTH];
	
	for (size_t i = 0; i < m_BlackList.size(); i++)
	{
		strcpy(strName, m_BlackList[i].c_str());
		size_t uLen = strlen(strName);
		//case like *Nub
		if (strName[0] == '*')
		{
			size_t uSrcLen = strlen(strBoneName);
			if (!strcmp(strName + 1, strBoneName + uSrcLen - uLen + 1))
			{
				//In black List;
				bReturn = TRUE;
				break;
			}
		}
		else if (strName[uLen - 1] == '*')
		{
			//case like S_*
			strName[uLen - 1] = '\0';
			const TCHAR* strFound = strstr(strBoneName, strName);
			if (strFound && strFound == strBoneName)
			{
				bReturn = TRUE;
				break;
			}
		}
		else
		{
			if (!strcmp(strBoneName, strName))
			{
				bReturn = TRUE;
				break;
			}
		}
	}

	return bReturn;
}