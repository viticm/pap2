//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGTestSaveAndLoad.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-6-2 10:00
//  Comment     :   验证加载和保存材质资源的测试用例类
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGTestSaveAndLoad_H__
#define _KGTestSaveAndLoad_H__
#include "KGMaterialTestCase.h"

class KGTestSaveAndLoad :
	public KGMaterialTestCase
{
public:
	KGTestSaveAndLoad();
	KGTestSaveAndLoad(const char szCaseName);
	virtual ~KGTestSaveAndLoad();

public:
	virtual int	 Init(IKG3DEngineManager*		pEngineMgr,
					  IKG3DAutoTestInterface*	pTestInterface,
					  const char				cszClientPath[],
					  const char				cszResultPath[]);	
	virtual void RunTest(const char cszFileName[]);
	virtual void UnInit();

	int  SetCaseName(const char szCaseName[]);

private:
	void OneSaveMtlByNewLoadingFunc(const char cszFileName[]);
	int  CompareNewMtlData(IEKG3DMaterial* pSavedMaterial, IEKG3DMaterial* pSourceMaterial);

	char m_szCaseName[MAX_PATH];
};
#endif