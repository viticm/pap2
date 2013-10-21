#include "header.h"
#include "KG3DRelinkTable.h"
#include "KG3DBiped.h"

KG3DRelinkTable::NodeInfo::~NodeInfo()
{
	Children.clear();
}

KG3DRelinkTable::KG3DRelinkTable(void)
{
}

KG3DRelinkTable::~KG3DRelinkTable(void)
{
	CleanUp();
}

HRESULT KG3DRelinkTable::LoadFromFile(LPCSTR strFileName)
{
	HRESULT hr = E_NOTIMPL;
	
	return hr;
}

HRESULT KG3DRelinkTable::LoadFromFile(FILE* fpFile)
{
	const size_t LineLen = (MAX_BONE_NAME_LENGTH + 1) * 4;
	int nNumItem = 0;
	TCHAR strBuffer[LineLen];
	if (!fpFile)
		return E_FAIL;
	
	fgets(strBuffer, LineLen, fpFile);
	sscanf(strBuffer, "%d", &nNumItem);
	if (!nNumItem)
		return S_OK;
		
	for(int i = 0; i < nNumItem; i++)
	{
		fgets(strBuffer, LineLen, fpFile);
		size_t uLen = strlen(strBuffer);
		if (strBuffer[uLen - 1] == 0x0a)
			strBuffer[uLen - 1] = '\0';
		ReadInData Data;
		int j = FillReadIn(strBuffer, Data);
		if (j)
		{
			m_ReadIn.push_back(Data);
		}
	}

	return S_OK;
}

INode* KG3DRelinkTable::GetParentNode(INode* pNode)
{
	INode* pReturn = NULL;
	RelinkTable::iterator i = m_RelinkTable.find(pNode);
	if (i != m_RelinkTable.end())
	{
		NodeInfo& Info = i->second;
		pReturn = Info.Parent;
	}
	else
	{
		pReturn = pNode->GetParentNode();
	}
	return pReturn;
}

void KG3DRelinkTable::SetLink(INode* pChild, INode* pParent)
{
	int nReturn = m_RelinkTable.count(pChild);
	if (!pChild)
		return;
	//if (!pParent)
	//	return;

	RelinkTable::iterator i = m_RelinkTable.find(pChild);
	RelinkTable::iterator j = m_RelinkTable.find(pParent);

	if (i == m_RelinkTable.end())
	{
		//Child node not in the relink list
		InsertNode(pChild);
	}
	//Child node is in the relink list
	NodeInfo& Info = m_RelinkTable[pChild];
	if (Info.Parent != pParent)
	{
		//Relink old Parent node
		RelinkTable::iterator itParent = m_RelinkTable.find(Info.Parent);
		if (itParent != m_RelinkTable.end())
		{
			//Node's Parent is inside list
			NodeInfo& ParentInfo = itParent->second;
			std::set<INode*>::iterator itChild = ParentInfo.Children.find(pChild);
			if (itChild != ParentInfo.Children.end())
			{
				ParentInfo.Children.erase(itChild);
			}
		}
		else
		{
			//Node's Parent is not inside the list
			//insert it to the list, and eliminate current child node
			InsertNode(Info.Parent);
			NodeInfo& ParentInfo = m_RelinkTable[Info.Parent];
			std::set<INode*>::iterator k = ParentInfo.Children.find(pChild);
			if (k != ParentInfo.Children.end())
			{
				ParentInfo.Children.erase(k);
			}
		}
	}
	Info.Parent = pParent;

	if (j == m_RelinkTable.end())
	{
		//parent node not in the relink list
		InsertNode(pParent);
		NodeInfo& ParentInfo = m_RelinkTable[pParent];
		std::set<INode*>::iterator k = ParentInfo.Children.find(pChild);
		assert(k == ParentInfo.Children.end());
		ParentInfo.Children.insert(pChild);
	}
	else
	{
		NodeInfo& ParentInfo = j->second;
		ParentInfo.Children.insert(pChild);
	}
}

void KG3DRelinkTable::CleanUp()
{
	m_RelinkTable.clear();
	m_ReadIn.clear();
	m_UnfoundLink.clear();
}

int KG3DRelinkTable::FillReadIn(TCHAR* strInput, ReadInData& Data)
{
	int nReturn = 0;
	TCHAR* strEnd = strInput + strlen(strInput);
	TCHAR* strFind = strstr(strInput, "  ");
	if (strFind)
	{
		strFind[0] = '\0';
		Data.strChild = strInput;
		strFind[0] = ' ';
		while(strFind[0] == ' ' && strFind != strEnd)
			strFind++;
		if (strFind != strEnd)
		{
			Data.strParent = strFind;
			nReturn = 1;
		}
	}
	return nReturn;
}

int KG3DRelinkTable::AjustWithSkeleton(KG3DBiped* pSk)
{
	m_UnfoundLink.clear();
	m_RelinkTable.clear();

	for(size_t i = 0; i < m_ReadIn.size(); i++)
	{
		INode* pChild = pSk->FindBone(m_ReadIn[i].strChild.c_str());
		INode* pParent = pSk->FindBone(m_ReadIn[i].strParent.c_str());
		//Both has been found
		UnfoundInfo Info = { i, RELINK_UNFOUND_NONE };
		if (!pChild)
		{
			Info.Type = RELINK_UNFOUND_CHILD;
		}
		
		if (!pParent)
		{
			Info.Type = RELINK_UNFOUND_PARENT;
		}

		if (Info.Type == RELINK_UNFOUND_NONE)
		{
			SetLink(pChild, pParent);
		}
		else
		{
			m_UnfoundLink.push_back(Info);
		}
	}
	return static_cast<int>(m_UnfoundLink.size());
}

int KG3DRelinkTable::GetNumChildren(INode* pNode)
{
	int nReturn = -1;
	RelinkTable::iterator i = m_RelinkTable.find(pNode);
	if (i != m_RelinkTable.end())
	{
		NodeInfo& Info = i->second;
		nReturn = static_cast<int>(Info.Children.size());
	}
	else
	{
		nReturn = pNode->NumberOfChildren();
	}
	return nReturn;
}

INode* KG3DRelinkTable::GetChildNode(INode* pNode, int i)
{
	RelinkTable::iterator it = m_RelinkTable.find(pNode);
	if (it != m_RelinkTable.end())
	{
		NodeInfo& Info = it->second;
		if (i >= static_cast<int>(Info.Children.size()))
			return NULL;

		std::set<INode*>::iterator j = Info.Children.begin();
		advance(j, i);
		return (*j);
	}
	else
	{
		return pNode->GetChildNode(i);
	}
}

void KG3DRelinkTable::InsertNode(INode* pNode)
{
	if (!pNode)
		return;

	NodeInfo Info;
	Info.Parent = GetParentNode(pNode);
	int nNumChildren = GetNumChildren(pNode);
	for(int i = 0; i < nNumChildren; i++)
	{
		INode* pChild = GetChildNode(pNode, i);
		assert(pChild);
		Info.Children.insert(pChild);
	}
	NodeInfo& InfoDest = m_RelinkTable[pNode];
	CopyNodeInfo(InfoDest, Info);
}


void KG3DRelinkTable::CopyNodeInfo(NodeInfo& Dest, const NodeInfo& Src)
{
	Dest.Parent = Src.Parent;
	std::set<INode*>::const_iterator i = Src.Children.begin();
	while(i != Src.Children.end())
	{
		Dest.Children.insert(*i);
		i++;
	}
}


void KG3DRelinkTable::EraseLink(INode* pNode)
{
	std::set<INode*>::iterator k;
	RelinkTable::iterator i = m_RelinkTable.find(pNode);
	if (i != m_RelinkTable.end())
	{
		//Node is in the relink table
		NodeInfo& Info = i->second;
		RelinkTable::iterator j = m_RelinkTable.find(Info.Parent);
		if (j == m_RelinkTable.end())
		{
			//If parent node is not inside a relink table
			//add it
			InsertNode(Info.Parent);
		}
		NodeInfo& ParentInfo = m_RelinkTable[Info.Parent];
		k = ParentInfo.Children.find(pNode);
		assert(k != ParentInfo.Children.end());
		ParentInfo.Children.erase(k);
		m_RelinkTable.erase(i);
	}
	else
	{
		//If node is not inside the relink table
		INode* pParent = GetParentNode(pNode);
		RelinkTable::iterator it = m_RelinkTable.find(pParent);
		if (it == m_RelinkTable.end())
		{
			InsertNode(pParent);
		}
		NodeInfo& ParentInfo = m_RelinkTable[pParent];
		k = ParentInfo.Children.find(pNode);
		assert(k != ParentInfo.Children.end());
		ParentInfo.Children.erase(k);
	}
}