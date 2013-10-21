//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KG3DEngineController.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-9-15 10:00
//  Comment     :   封装加载旧的引擎Dll，并提供相应的控制接口，获取相应的数据
//					使用的代码和SVN版本标签：t_jx3_built_1-0-8-1548
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KGLoadController_H__
#define _KGLoadController_H__

#include "stdafx.h"
#include "IKG3DEngineController.h"
#include "KG3DInterface.h"
#include "IEKG3DScene.h"
#include "IEKG3DTerrain.h"

class KG3DEngineController : public IKG3DEngineController
{
public:
	KG3DEngineController(void);
	~KG3DEngineController(void);

	int  Init(const char cszClientPath[], const char cszDLLName[]);
	void UnInit();

	int  LoadMapInfo(const char cszMapName[]);
	int  RemoveMap();
	HRESULT  GetAllTerrainInfoHandle(vector<UINT>* vecHandle);
	HRESULT  GetAllEntityInfoHandle(vector<UINT>* vecHandle);
	HRESULT  GetEntityInfoformation(UINT uEntityInfoHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData,DWORD dwMaxLen);
	HRESULT  GetTerrainInfoformation(UINT uEntityInfoHandle,DWORD* pdwDataType,DWORD* pdwLength,PVOID pData);

private:
	//格式化目录
	int  FormatPath(char* pszPath, const size_t cuSize);

private:
	HMODULE m_hEngineDLL;
	IKG3DEngineManager* m_pEngineMgr;
	IKG3DScene* m_pScene;
	IEKG3DSceneOutDoorSpaceMgr* m_pIEOutDoorMgr;
};
#endif