//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGBaseResourceTestCase.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-6-2 10:00
//  Comment     :   资源测试用例基类
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGBaseResourceTestCase_H__
#define _KGBaseResourceTestCase_H__

#include "KGTestCommon.h"
#include "KG3DInterface.h"
#include "KG3DAutoTestInterface.h"

class KGBaseResourceTestCase
{		
public:
	KGBaseResourceTestCase();
	virtual ~KGBaseResourceTestCase();
	virtual int  Init(IKG3DEngineManager*	  pEngineMgr,
					  IKG3DAutoTestInterface* pTestInterface,
					  const char			  cszClientPath[],
					  const char			  cszResultPath[]) = 0;	
 	virtual void RunTest(const char cszFileName[]) = 0;
 	virtual void UnInit() = 0;

protected:
	int	OuputResult();
	int	InitResultLog(const char cszCaseName[]);

protected:
	IKG3DEngineManager*		 m_pEngineMgr;
	IKG3DAutoTestInterface*  m_pTestInterface;
	vector<string> m_vecResult;
	FILE* m_pResultFile;
	char m_szResultPath[MAX_PATH];
	char m_szClientPath[MAX_PATH];
};
#endif