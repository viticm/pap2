//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KG3DEngineHelper.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-10-10 10:00
//  Comment     :   3D引擎接口调用的操作类
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DEngineHelper_H__
#define _KG3DEngineHelper_H__

#include "KGTestCommon.h"
#include "KGFileManager.h"

#include "KG3DInterface.h"
#include "KG3DSceneDataInterface.h"
#include "IEKG3DTerrain.h"

class KG3DEngineHelper
{
public:
	KG3DEngineHelper(void);
	~KG3DEngineHelper(void);

	int  Init(const char cszClientPath[]);
	int  UnInit();

	//创建默认的地图场景
	int  CreateMap(IKG3DScene** pOutScene, const char cszResourcePath[], int nModelToCreate);
	//加载地图场景信息，不加载模型数据
	int  LoadMapInfo(const char cszMapName[], IKG3DScene** pOutScene);
	//加载整个地图场景数据，包括模型数据
	int  LoadMapData(const char cszMapName[], IKG3DScene** pOutScene);
	//保存地图场景
	int  SaveMap(const char cszMapName[], IKG3DScene* pOutScene);
	//释放场景资源
	int  RemoveScene(IKG3DScene* pScene);

	//引擎的加载和卸载
	int  LoadEngineDLL();
	int  UnLoadEngineDLL();

private:
	HMODULE m_hEngineDLL;
	IKG3DEngineManager* m_pEngineMgr;
	char m_szClientPath[MAX_PATH];
	vector<string> m_vecResNameList;			//加入新建场景的模型名称列表
};

#endif