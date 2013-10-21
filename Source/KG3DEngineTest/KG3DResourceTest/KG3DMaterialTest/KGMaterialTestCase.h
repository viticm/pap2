//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGMaterialTestCase.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-6-2 10:00
//  Comment     :   材质资源测试用例基类
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGMaterialTestCase_H__
#define _KGMaterialTestCase_H__

#include "KGBaseResourceTestCase.h"
#include "KG3DMaterial.h"
#include "KG3DTexture.h"
#include "KG3DMaterialTable.h"

class KGMaterialTestCase :
	public KGBaseResourceTestCase
{
public:
	KGMaterialTestCase();
	virtual ~KGMaterialTestCase();
 
	virtual int  Init(IKG3DEngineManager*	  pEngineMgr,
					  IKG3DAutoTestInterface* pTestInterface,
					  const char			  cszClientPath[],
					  const char			  cszResultPath[]) = 0;	
	virtual void RunTest(const char cszFileName[]) = 0;
	virtual void UnInit() = 0;

protected:
	int  LoadMaterialByNewFunc(const char cszFileName[], IEKG3DMaterial** ppMaterial);
	int  CompareOption(void* pvOption1, void* pvOption2);
	void ReleaseMtl(IEKG3DMaterial* pMaterial);
	
	static bool m_MtlCanBeLoaded;

	KG3DMaterialTable* m_pMtlTable;
	
};
#endif