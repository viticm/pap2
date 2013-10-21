//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGTestMapDisuseResource.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-7-03 10:00
//  Comment     :   1. 通过加载KG3DEngine_AutoTest.dll，调用加载.map的接口，加载.EntityInfo和.TerrainInfo文件。 
//					2. 从.EntityInfo和.TerrainInfo文件中分析出引用的资源文件名，
//					   并在data/source/map_source的资源列表里进行搜索，留下没有被引用的资源文件名。 
//					3. 输出没有被引用的资源文件名列表。
//					4. 输出缺失的资源文件名列表。
//
//////////////////////////////////////////////////////////////////////////////////////
#ifndef _KGTestMapDisuseResource_H__
#define _KGTestMapDisuseResource_H__

#include "KGTestCommon.h"
#include "KG3DInterface.h"
#include "IEKG3DTerrain.h"

const char TEST_PROJECT_NAME[] = "KG3DMapsResourceTest";

class KGTestMapDisuseResource
{
public:
	KGTestMapDisuseResource();
	~KGTestMapDisuseResource();
	
public:
	int  Init();	
	int  RunTest();
	int  OutputResult();
	void UnInit();

private:
	int  LoadEngineDLL();
	int  LoadTestSetting();
	int  AnalyseResType(const char cszResType[]);
	int  FindSourceResource();
	//查找指定的现有资源(.sfx .SoundBall .ModelSet .Mesh .Mtl .pvs）
	int  LoadSourceResourceInfo(const char cszPath[]);
	///////////////////////////////////////////////////////////////////////////
	//查找场景文件(.map)中引用的指定资源
	int  FindUsedMapResource();
	int  AnalyseOneMapResourceInfo(const char cszMapName[]);
	int  AnalyseEntityInfo(IEKG3DSceneOutDoorSpaceMgr* pIEOutDoorMgr);
	//int  AnalyseTerrainInfo(IEKG3DSceneOutDoorSpaceMgr* pIEOutDoorMgr);
	//////////////////////////////////////////////////////////////////////////
	//对比现有资源和场景已使用的资源，找出未使用的资源
	int  CheckDisuseResource();
	int  CheckLostResource();

	//辅助分析资源文件
	int  FindResource(const char cszResourceName[], set<string>& setResList);
	int  FindResInModelset(const char cszResourceName[], set<string>& setResList);
	int  FindResInPVS(const char cszResourceName[], set<string>& setResList);
	int  FindResInSTree(const char cszResourceName[], set<string>& setResList);
	int  FindResInMDL(const char cszResourceName[], set<string>& setResList);
	int  FindResInSFX(const char cszResourceName[], set<string>& setResList);

	//输出
	int  OutputDisuseRes(const char cszResultPath[]);
	int  OutputLostRes(const char cszResultPath[]);

private:
	HMODULE m_hEngineDLL;
	IKG3DEngineManager* m_pEngineMgr;
	char m_szClientPath[MAX_PATH];
	map<string, vector<string>> m_mapSourceRes;		//本地资源所在目录下所有资源集合 string:资源类型（扩展名） vector<string>:资源文件名集合
	set<string> m_setUsedRes;						//所有场景使用的资源
	vector<string> m_vecLODRes;						//LOD资源，如*_L.Mesh *_L.Mtl
	vector<string> m_vecMapList;					//检查的地图名称，如果配置文件没配置，则为MapList.tab文件中记录的地图名称
	vector<string> m_vecResType;					//检查的资源类型
	vector<string> m_vecResPath;					//本地资源所在目录
	bool m_bFindDisuseMapRes;						//是否查找废弃地图资源
};
#endif