//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGTestStressSaveAndLoad.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-6-2 10:00
//  Comment     :   反复验证加载和保存的材质接口
//
//////////////////////////////////////////////////////////////////////////////////////

#include "kgmaterialtestcase.h"
#include "KGTestSaveAndLoad.h"

class KGTestStressSaveAndLoad :
	public KGMaterialTestCase
{
public:
	KGTestStressSaveAndLoad();
	virtual ~KGTestStressSaveAndLoad();

	virtual int	 Init(IKG3DEngineManager*		pEngineMgr,
					  IKG3DAutoTestInterface*	pTestInterface,
					  const char				cszClientPath[],
					  const char				cszResultPath[]);	
	virtual void RunTest(const char cszFileName[]);
	virtual void UnInit();

	void SetMaxRunTimes(int nTimes){nMaxRunTimes = nTimes;}
	void SetMaxRunFiles(int nFiles){nMaxRunFiles = nFiles;}

private:
	KGTestSaveAndLoad TestSaveAndLoad;
	int nMaxRunTimes;
	int nMaxRunFiles;
	int nCurrentFiles;

};
