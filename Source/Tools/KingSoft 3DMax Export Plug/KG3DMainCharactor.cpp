#include "header.h"
#include "KG3DMainCharactor.h"
#include <direct.h>
#include <set>
#include <string>

KG3DMainCharactor::KG3DMainCharactor(Interface* pIP) : KG3DExportNodeSelector(pIP)
{
	memset(m_strMainFile, 0, sizeof(TCHAR) * MAX_PATH);
	memset(m_strPartFile, 0, sizeof(TCHAR) * MAX_PATH);
	memset(m_strMaxFile, 0, sizeof(TCHAR) * MAX_PATH);
}

KG3DMainCharactor::~KG3DMainCharactor(void)
{
}

void KG3DMainCharactor::GetExportNode()
{
	TCHAR strPartName[MAX_OBJNAME_LEN];
	TCHAR strFileName[MAX_PATH];
	TCHAR strErrorInfo[MAX_PATH * 4];
	BOOL bError = FALSE;

	strFileName[0] = '\0';

	memset(strErrorInfo, 0, sizeof(TCHAR) * (MAX_PATH * 4));
	sprintf(strErrorInfo, "The parts below not found:\n");

	if (!m_ip)
		return;

	m_Parts.clear();

	TSTR strMaxFile = m_ip->GetCurFilePath();
	SetMaxFile(strMaxFile);

	int nLast = strlen(m_strMainFile) - 1;
	while (nLast >= 0)
	{
		if (m_strMainFile[nLast] == '\\')
			break;
		nLast--;
	}
	
	if (nLast >= 0)
	{
		strcpy(strFileName, m_strMainFile + nLast + 1);
	}
	
	//Here we find the mesh need to be exported.
	for(size_t i = 0; i < m_Suffix.size(); i++)
	{
		sprintf(strPartName, "%s%s", strFileName, m_Suffix[i].c_str());
		INode* pNode = m_ip->GetINodeByName(strPartName);
		if (!pNode)
		{
			//Error here
			bError = TRUE;
			strcat(strErrorInfo, strPartName);
			strcat(strErrorInfo, "\n");
		}
		else
		{
			m_Parts.push_back(pNode);
		}
	}

	if (bError)
	{
		//Error happened, output info.
		MessageBox(m_ip->GetMAXHWnd(), strErrorInfo, "Error", MB_OK);
	}
}

void KG3DMainCharactor::GetExportNode1()
{
	TCHAR strFileName[MAX_PATH];
	TCHAR strErrorInfo[MAX_PATH * 4];
	BOOL bError = FALSE;

	strFileName[0] = '\0';

	memset(strErrorInfo, 0, sizeof(TCHAR) * (MAX_PATH * 4));
	sprintf(strErrorInfo, "The parts below not found:\n");

	if (!m_ip)
		return;

	m_Parts.clear();

	TSTR strMaxFile = m_ip->GetCurFilePath();
	SetMaxFile(strMaxFile);

	int nLast = strlen(m_strMainFile) - 1;
	while (nLast >= 0)
	{
		if (m_strMainFile[nLast] == '\\')
			break;
		nLast--;
	}

	if (nLast >= 0)
	{
		strcpy(strFileName, m_strMainFile + nLast + 1);
	}

	std::set<std::string> setSelList;
	setSelList.clear();
	int nSelNodeCount = m_ip->GetSelNodeCount();
	for (int i = 0;i < nSelNodeCount;i++)
	{
		INode* pNode = m_ip->GetSelNode(i);
		if (pNode)
		{
			char strMainName[MAX_PATH];
			memset(strMainName,'\0',MAX_PATH);
			TSTR strName = pNode->NodeName();
			TCHAR* strSubStr = strrchr(strName,'_');
			if (strSubStr)
			{
				int nMainNameSize = strlen(strName) - strlen(strSubStr);
				if (nMainNameSize > 0)
				{
					strncpy(strMainName,strName,nMainNameSize);
					string strTemp = strMainName;
					setSelList.insert(strTemp);
				}
			}
		}
	}

	GetAllNode();
	//Here we find the mesh need to be exported.
	std::set<string>::iterator it = setSelList.begin();
	while (it != setSelList.end())
	{
		for (size_t i = 0; i < m_NodeList.size(); i++)
		{
			for (size_t j = 0; j < m_Suffix.size(); j++)
			{
				ObjectState ObjState = m_NodeList[i]->EvalWorldState(0);
				if (!ObjState.obj || 
					ObjState.obj->SuperClassID() != GEOMOBJECT_CLASS_ID) 
					continue;
				TSTR strNodeName = m_NodeList[i]->NodeName();
				TCHAR* strFind = strstr(strNodeName, m_Suffix[j].c_str());
				TCHAR* strFind2 = strstr(strNodeName,it->c_str());
				if (strFind && strFind2  && strFind[m_Suffix[j].size()] == '\0')//is a suffix
				{
					m_Parts.push_back(m_NodeList[i]);
					break;
				}
			}
		}
		it++;
	}

	if (bError)
	{
		//Error happened, output info.
		MessageBox(m_ip->GetMAXHWnd(), strErrorInfo, "Error", MB_OK);
	}
}

HRESULT KG3DMainCharactor::LoadConfig(const TCHAR* strFileName)
{
	TCHAR strBuffer[MAX_SUFFIX_LEN];
	if (!strFileName)
		return E_FAIL;

	FILE* fpFile = fopen(strFileName, "r");
	if (!fpFile)
		return E_FAIL;
	
	while(fgets(strBuffer, MAX_SUFFIX_LEN, fpFile))
	{
		if (strBuffer[strlen(strBuffer) - 1] == 0x0a)
			strBuffer[strlen(strBuffer) - 1] = '\0';
		std::string strSuffix = strBuffer;
		m_Suffix.push_back(strSuffix);
	}
	fclose(fpFile);
	return S_OK;
}

void KG3DMainCharactor::CleanUp()
{
	for(size_t i = 0; i < m_BindBones.size(); i++)
	{
		m_BindBones[i].clear();
	}
	m_BindBones.clear();
	m_Suffix.clear();
	m_LinkBreaker.clear();
	m_LinkInfoSaved.clear();
	m_NodeList.clear();
	KG3DExportNodeSelector::CleanUp();
}

HRESULT KG3DMainCharactor::SaveToFile(const TCHAR* strFileName)
{
	HRESULT hr = E_FAIL;
	TCHAR strSaveFileName[MAX_PATH];
	FILE* fpFile = fopen(strFileName, "w");
	if (!fpFile)
		return E_FAIL;

	GetBipFilePath(strSaveFileName);

	_splitpath(strFileName, NULL, NULL, strSaveFileName, NULL);

	//bip file name
	fprintf(fpFile, "%s.txt\n", strSaveFileName);

	for (size_t i = 0; i < m_Parts.size(); i++)
	{
		//GetPartFilePath(i, strSaveFileName);
		TSTR strNode = m_Parts[i]->NodeName();
		if (strstr(strNode, "_face"))
			continue;
		//mesh file name
		fprintf(fpFile, "%s.mesh\n", strNode);
	}
	fclose(fpFile);
	return hr;
}

void KG3DMainCharactor::GetAnimationFilePath(TCHAR* strOutput)
{
	assert(strOutput);
	sprintf(strOutput, "%s%s.ani", m_strPartFile, m_strMaxFile);
}

void KG3DMainCharactor::GetAnimationDescName(TCHAR* strOutput)
{
	assert(strOutput);
	strcpy(strOutput, m_strMaxFile);
}

void KG3DMainCharactor::GetBipFilePath(TCHAR* strOutput)
{
	assert(strOutput);
	sprintf(strOutput, "%s.txt", m_strMainFile);
}

void KG3DMainCharactor::GetMdlFilePath(TCHAR* strOutput)
{
	assert(strOutput);
	sprintf(strOutput, "%s.mdl", m_strMainFile);
}

void KG3DMainCharactor::GetPartFilePath(int nPartIndex, TCHAR* strOutput)
{
	assert(nPartIndex < static_cast<int>(m_Parts.size()));
	assert(strOutput);
	INode* pNode = m_Parts[nPartIndex];
	TSTR strParName = pNode->NodeName();
	sprintf(strOutput, "%s%s", m_strPartFile, strParName);
}

void KG3DMainCharactor::SetMaxFile(const TCHAR* strMaxFileName)
{
	assert(strMaxFileName);
	GetMainFilePath(strMaxFileName, m_strMainFile);
	GetPartFilePath(strMaxFileName, m_strPartFile);
}

void KG3DMainCharactor::GetMainFilePath(const TCHAR* strMaxFileName, TCHAR* strOutput)
{
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];
	TCHAR strExt[MAX_PATH];
	TCHAR strExportDir[MAX_PATH];
	
	_splitpath(strMaxFileName, strDriver, strPath, strFile, strExt);
	int nLast = strlen(strPath) - 1;
	if (strPath[nLast] == '\\')
	{
		nLast--;
	}
	sprintf(m_strSrcTexturFile, "%s%sTexture\\", strDriver, strPath);
	while(nLast >= 0)
	{
		if (strPath[nLast] == '\\')
			break;
		nLast--;
	}

	if (nLast >= 0)
	{
		sprintf(strOutput, "%s%sExport\\%s", strDriver, strPath, strPath + nLast + 1);
	}
	else
	{
		sprintf(strOutput, "%sExport\\%s", strDriver, strFile);
	}
	
	if (strOutput[strlen(strOutput) - 1] == '\\')
	{
		strOutput[strlen(strOutput) - 1] = '\0';
	}

	sprintf(strExportDir, "%s%sExport\\", strDriver, strPath);
	_mkdir(strExportDir);
}

void KG3DMainCharactor::GetPartFilePath(const TCHAR* strMaxFileName, TCHAR* strOutput)
{
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];
	TCHAR strExt[MAX_PATH];

	
	_splitpath(strMaxFileName, strDriver, strPath, strFile, strExt);
	sprintf(strOutput, "%s%sExport\\", strDriver, strPath);
	strcpy(m_strMaxFile, strFile);
}

void KG3DMainCharactor::BreakLink()
{
	m_LinkInfoSaved.clear();
	INode* pRoot = m_ip->GetRootNode();
	int nNumChildren = pRoot->NumberOfChildren();
	for (int i = 0; i < nNumChildren; i++)
	{
		INode* pChild = pRoot->GetChildNode(i);
		Helper_BreakLink(pChild);
	}
}

void KG3DMainCharactor::RestoreLink()
{
	for (size_t i = 0; i < m_LinkInfoSaved.size(); i++)
	{
		LinkBreakerInfo& Info = m_LinkInfoSaved[i];
		Info.pParent->AttachChild(Info.pChild);
	}
	m_LinkInfoSaved.clear();
}

void KG3DMainCharactor::Helper_BreakLink(INode* pNode)
{
	TCHAR* strSocketFind = NULL;
	LinkBreakerInfo BreakInfo;
	if (!pNode)
		return;
	
	TSTR strNodeName = pNode->NodeName();
	strSocketFind = strstr(strNodeName, "S_");
	if (strSocketFind && 
		strSocketFind == strNodeName)//Make sure S_ is at the beginning of node's name
	{
		ObjectState os = pNode->EvalWorldState(GetExportOption()->m_nStaticFrame); 

		if (os.obj) 
		{
			if (HELPER_CLASS_ID == os.obj->SuperClassID())
			{
				BreakInfo.pParent = pNode;
				size_t uLastDetached = m_LinkInfoSaved.size();
				int nNumChildren = pNode->NumberOfChildren();
				for (int i = 0; i < nNumChildren; i++)
				{
					BreakInfo.pChild = pNode->GetChildNode(i);
					m_LinkInfoSaved.push_back(BreakInfo);
				}

				for (size_t j = uLastDetached; j < m_LinkInfoSaved.size(); j++)
				{
					LinkBreakerInfo& Info = m_LinkInfoSaved[j];
					Info.pChild->Detach(GetExportOption()->m_nStaticFrame, 1);//Keep pos
				}
				return;
			}
		}
	}

	strSocketFind = strstr(strNodeName, "B_");
	if (strSocketFind &&
		strSocketFind == strNodeName)
	{
		size_t uLastDetached = m_LinkInfoSaved.size();
		int nNumChildren = pNode->NumberOfChildren();
		for (int i = 0; i < nNumChildren; i++)
		{
			INode* pChild = pNode->GetChildNode(i);
			TSTR strChildName = pChild->NodeName();
			strSocketFind = strstr(strChildName, "S_");
			if (!strSocketFind ||
				strSocketFind != strChildName)
			{
				BreakInfo.pParent = pNode;
				BreakInfo.pChild = pChild;
				m_LinkInfoSaved.push_back(BreakInfo);
			}
		}

		for (size_t j = uLastDetached; j < m_LinkInfoSaved.size(); j++)
		{
			LinkBreakerInfo& Info = m_LinkInfoSaved[j];
			Info.pChild->Detach(GetExportOption()->m_nStaticFrame, 1);
		}
	}
	
	int nNumChildren = pNode->NumberOfChildren();
	for (int i = 0; i < nNumChildren; i++)
	{
		INode* pChildNode = pNode->GetChildNode(i);
		Helper_BreakLink(pChildNode);
	}
}


void KG3DMainCharactor::BeginExport(LPSTR path)
{
	HRESULT hr = E_FAIL;
	TSTR filename;
	filename = path;
	filename += "\\";
	filename += "main.txt";
	hr = LoadConfig(filename);
	if (FAILED(hr))
	{
		MessageBox(m_ip->GetMAXHWnd(), "Load main character config file failed.", filename, MB_OK);
		return;
	}

	GetExportNode1();
	//BreakLink();	
}

void KG3DMainCharactor::EndExport()
{
	RestoreLink();
}

void KG3DMainCharactor::GetTexturePath(TCHAR* strOutput)
{
	sprintf(strOutput, "%sTexture\\", m_strPartFile);
} 

void KG3DMainCharactor::GetAllNode()
{
	if (!m_ip)
		return;

	m_NodeList.clear();
	INode* pNode = m_ip->GetRootNode();
	
	if (pNode)
	{
		Helper_GetAllNode(pNode);
	}
}

void KG3DMainCharactor::Helper_GetAllNode(INode* pNode)
{
	if (!pNode)
		return;

	m_NodeList.push_back(pNode);
	for (int i = 0; i < pNode->NumberOfChildren(); i++)
	{
		INode* pChildren = pNode->GetChildNode(i);
		Helper_GetAllNode(pChildren);
	}
}