//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGTestDifferLoad.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-6-2 10:00
//  Comment     :   用新旧加载方式加载材质资源的测试用例类
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGTestDifferLoad_H__
#define _KGTestDifferLoad_H__

#include "KGMaterialTestCase.h"
#include "../OldMaterialLoad/IKGLoadController.h"

class KGTestDifferLoad :
	public KGMaterialTestCase
{
public:
	KGTestDifferLoad();
	virtual ~KGTestDifferLoad();

public:
	virtual int	 Init(IKG3DEngineManager*		pEngineMgr,
					  IKG3DAutoTestInterface*	pTestInterface,
					  const char				cszClientPath[],
					  const char				cszResultPath[]);	
	virtual void RunTest(const char cszFileName[]);
	virtual void UnInit();

private:
	int  LoadOldEngineDLL();
	void OneMtlByDifferLoadingFunc(const char cszFileName[]);
	int  CompareDifferVersionMtlBaseData(IEKG3DMaterial* pSourceMaterial);
	int  CompareDifferVersionMtlSubset(IEKG3DMaterial* pSourceMaterial);

private:
	HMODULE m_hLoadCtrlDLL;
	IKGLoadController* m_pOldLoadCtrl;

};
#endif