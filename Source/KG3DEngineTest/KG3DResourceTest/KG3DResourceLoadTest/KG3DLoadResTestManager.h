#ifndef _KG3DLoadResTestManager_H__
#define _KG3DLoadResTestManager_H__

#include "KGTestLoadMaterial.h"
#include "KG3DTestLoadMesh.h"

const char TEST_PROJECT_NAME[] = "KGTestLoadRes";

class KG3DLoadResTestManager
{
public:
	KG3DLoadResTestManager(void);
	~KG3DLoadResTestManager(void);

	int  Init(const char cszClientPath[]);
	int  Run(const char cszTestPath[]);
	int  AnalyseLog();
	int  OutputResult();
	void UnInit();

private:
	int  LoadEngineDLL();
	void RefreshConsole();

private:
	KGTestLoadMaterial  m_TestLoadMaterial;
	KG3DTestLoadMesh	m_TestLoadMesh;

	HMODULE m_hEngineDLL;
	IKG3DEngineManager* m_pEngineMgr;
	IKG3DAutoTestInterface* m_pTestInterface;
	
	char m_szClientPath[MAX_PATH];
	int m_nTotalFileNum;
	int m_nMtlFileNum;
	int m_nMeshFileNum;
};

#endif