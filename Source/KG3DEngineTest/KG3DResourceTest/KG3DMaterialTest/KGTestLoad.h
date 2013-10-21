//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGTestLoad.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-6-2 10:00
//  Comment     :   检查材质资源的加载测试用例类
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGTestLoad_H__
#define _KGTestLoad_H__
#include "KGMaterialTestCase.h"

class KGTestLoad :
	public KGMaterialTestCase
{
public:
	KGTestLoad(void);
	virtual ~KGTestLoad(void);

public:
	virtual int  Init(IKG3DEngineManager*		pEngineMgr,
					  IKG3DAutoTestInterface*	pTestInterface,
					  const char				cszClientPath[],
					  const char				cszResultPath[]);	
	virtual void RunTest(const char cszFileName[]);
	virtual void UnInit();
};
#endif
