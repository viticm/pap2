#pragma once
#include <vector>
//#include <string>

using namespace std;

class KG3DSceneEditorDirectory
{
private:
	struct EntityDirectory
	{
		CString title;
		CString name;
	};
	vector<EntityDirectory>m_vecDirectorys;
public:
	KG3DSceneEditorDirectory(void);
public:
	virtual ~KG3DSceneEditorDirectory(void);
public:
	HRESULT DeleteDirectory(int nIndex);
	HRESULT AddDirectory(LPSTR pTitle,LPSTR pDirName);
	HRESULT GetDirectory(int nIndex,LPSTR pTitle,LPSTR pDirName);
	int     GetNumofDirectory();

	HRESULT Save();
	HRESULT Load();
};

extern KG3DSceneEditorDirectory g_cEditorDirectory;
