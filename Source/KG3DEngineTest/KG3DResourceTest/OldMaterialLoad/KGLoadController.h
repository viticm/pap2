//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGLoadController.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-6-10 10:00
//  Comment     :   封装加载旧的引擎Dll，提供获得加载后的材质数据的接口抽象类的实现类
//					加载低版本的引擎dll，调用旧版本的加载接口加载材质资源
//					代码和SVN版本标签：t_jx3_built_1-0-8-1428
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGLoadController_H__
#define _KGLoadController_H__

#include "stdafx.h"
#include "IEOther.h"
#include "IEKG3DMaterial.h"
#include "IKGLoadController.h"

class KGLoadController : public IKGLoadController
{
public:
	KGLoadController();
	~KGLoadController();

	virtual int   Init(const char cszClientPath[]);
	virtual void  UnInit();
	virtual int   Reset();
	virtual int   LoadMaterialByOldFunc(const char cszFileName[]);
	virtual DWORD GetNumMaterials();
	virtual int   GetSubset(OldMaterialSubset** ppOldMaterialSubset, const UINT cuIndex);
	virtual int   GetBaseData(BaseMtlData** ppBaseMtlData);

private:
	int  SetMaterialData();
	void CleanVecSubset();
	//格式化目录
	int  FormatPath(char* pszPath, const size_t cuSize);

private:
	HMODULE m_hEngineDLL;
	IEKG3DEngineManager* m_pEngineMgr;
	IEKG3DMaterial*		 m_pMaterial;
	DWORD m_dwNumMaterials;
	vector<OldMaterialSubset> m_vecMTLSubset;
	BaseMtlData m_BaseMtlData;
};

extern KGLoadController g_cKGLoadController;

#endif