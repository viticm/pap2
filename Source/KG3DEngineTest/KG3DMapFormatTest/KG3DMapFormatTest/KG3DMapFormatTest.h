//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KG3DMapFormatTest.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-9-14 10:00
//  Comment     :   地图场景新格式的测试。
//					主要实现了随机创建、随机加载、保存新旧格式地图场景的功能。
//					通过测试接口获取的各种地图场景数据并进行数据比对验证，保证新旧地图在创建、加载、保存的结果正确
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KG3DMapFormatTest_H__
#define _KG3DMapFormatTest_H__

#include "KGFileManager.h"
#include "KG3DEngineHelper.h"

#include "..\KGOldEngineController\IKG3DEngineController.h"

const char TEST_PROJECT_NAME[] = "KG3DMapFormatTest";

class KG3DMapFormatTest
{
public:
	KG3DMapFormatTest(void);
	~KG3DMapFormatTest(void);

	int  Init(const char cszClientPath[]);
	int  Run();
	void UnInit();

private:
	//测试用例/////////////////////////////////////////////////////////////////
	//验证新旧引擎加载旧格式地图场景的数据
	int  ValidateLoadOldMapByDifferEngine(const char cszOldMapName[]);
	//验证保存新建的地图场景(包括新旧格式)
	int  ValidateSaveCreatedMap(int nPackFileSign);
	//验证重存已存在的地图场景(包括新旧格式)
	int  ValidateSaveExistMap(int nPackFileSign, const char cszMapName[]);
	//验证旧格式地图转换为新格式地图(包括转与不转)
	int  ValidateOldFormatTransformNew(const char cszOldMapName[], bool bTransformNew);
	//////////////////////////////////////////////////////////////////////////

	
	//地图场景资源数据比较(新旧引擎加载的资源比较)
	int  CompareDifferEngineMapData(IKG3DScene* pNewEngineScene, IKG3DEngineController* pOldEngineController);
	int  CompareDifferEngineEntityInfo(IEKG3DSceneOutDoorSpaceMgr* pNewEngineOutDoorMgr, IKG3DEngineController* pOldEngineController);
	int  CompareDifferEngineTerrainInfo(IEKG3DSceneOutDoorSpaceMgr* pNewEngineOutDoorMgr, IKG3DEngineController* pOldEngineController);

	//地图场景资源数据比较
	int  CompareMapData(IKG3DScene* pScene_1, IKG3DScene* pScene_2);
	int  CompareEntityInfo(IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_1, IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_2);
	int  CompareTerrainInfo(IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_1, IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_2);
	int  CompareSceneBlockInfo(IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_1, IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_2);
	int  CompareEntityData(IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_1, IEKG3DSceneOutDoorSpaceMgr* pOutDoorMgr_2);
	int  CompareDataBuffer(PVOID pData_1, PVOID pData_2, DWORD dwLenth);
	int  CompareMapFiles(const char cszMapPath_1[], const char cszMapPath_2[]);

	//新格式文件的验证(验证是否保存为map.TerrainInfo和map.EntityInfo)
	int  CheckNewFormatFile(const char cszMapName[]);
	int  CheckMapVersion(const char cszMapName[], DWORD dwCheckVersion);
	int  LoadMapList();
	int  LoadCaseSetting();
	int  OutputResult();
	int  CleanSaveMap(const char cszMapName[]);

	//创建线程查找MessageBox窗口发送消息模拟按下Yes按钮
	//由于用例：ValidateOldFormatTransformNew中将旧格式地图场景保存为新格式时，会有MessageBox的提示需要确认
	int  CreateWindowFinderThread();
	static void FinderRun(void *pvArg);
	int DestroyThread();

private:
	struct CaseSetting 
	{
		int nTestingAllMap;				//1：测试所有地图 0：随机抽取某个地图测试
		int nMaxModelToCreate;			//添加到场景中的最大模型数量
		int nMinModelToCreate;			//添加到场景中的最小模型数量
		int nMaxOldMapID;				//MapList.tab中旧地图名称对应的最大ID号
		char szResourcePath[MAX_PATH];	//添加到场景中的模型来源
	};
	CaseSetting m_CaseSetting;
	struct CaseInfo
	{
		string strCaseName;
		string strCaseInfo;
		string strCaseResult;
	};
	
	char m_szConfigFullName[MAX_PATH];
	char m_szClientPath[MAX_PATH];
	char m_szTestMapFullName[MAX_PATH];
	vector<string> m_vecOldMapList;				//旧场景名称列表
	vector<string> m_vecNewMapList;				//新场景名称列表
	vector<CaseInfo> m_vecTestResult;				//测试用例运行结果
	bool m_bKMemoryInited;						//内存管理初始化状态
	bool m_bNeedToDelKey;						//测试结束是否需要删除config.ini里的nPackFile项
	KG3DEngineHelper m_EngineHelper;			//3D引擎接口操作封装
	KThread* m_pWorkThread;
	static bool m_bTransformNew;				//是否转换成新格式地图保存
};

#endif