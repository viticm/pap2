#pragma once
#include "kg3dexportnodeselector.h"

class KG3DMainCharactor : public KG3DExportNodeSelector
{
	enum 
	{
		MAX_SUFFIX_LEN = 32,
		MAX_OBJNAME_LEN    = 256,
	};

	struct LinkBreakerInfo
	{
		INode* pChild;
		INode* pParent;
	};

public:
	KG3DMainCharactor(Interface* pIP);
	~KG3DMainCharactor(void);
	
	HRESULT LoadConfig(const TCHAR* strFileName);
	virtual void CleanUp();
	HRESULT SaveToFile(const TCHAR* strFileName);
	void SetMaxFile(const TCHAR* strMaxFileName);
	void GetBipFilePath(TCHAR* strOutput);
	void GetMdlFilePath(TCHAR* strOutput);
	void GetPartFilePath(int nPartIndex, TCHAR* strOutput);
	void GetAnimationFilePath(TCHAR* strOutput);
	void GetTexturePath(TCHAR* strOutput);
	void GetAnimationDescName(TCHAR* strOutput);

	virtual void BeginExport(LPSTR path);
	virtual void EndExport();
	

	//Break links indicated in config file
	void BreakLink();
	
	//Restore links indicated in config file for max file
	void RestoreLink();

private:
	virtual void GetExportNode();
	virtual void GetExportNode1();
	void GetMainFilePath(const TCHAR* strMaxFileName, TCHAR* strOutput);
	void GetPartFilePath(const TCHAR* strMaxFileName, TCHAR* strOutput);
	void GetAllNode();
	void Helper_GetAllNode(INode* pNode);

	void Helper_BreakLink(INode* pNode);

	std::vector<std::vector<INode*>> m_BindBones;
	std::vector<std::string> m_Suffix;
	std::vector<std::string> m_LinkBreaker;
	std::vector<LinkBreakerInfo> m_LinkInfoSaved;
	std::vector<INode*> m_NodeList;

	TCHAR m_strPartFile[MAX_PATH];
	TCHAR m_strMainFile[MAX_PATH];
	TCHAR m_strMaxFile[MAX_PATH];
	TCHAR m_strSrcTexturFile[MAX_PATH];
};
