//////////////////////////////////////////////////////////////////////////////////////
// Copyright(c) Kingsoft
//
//  FileName    :   KGMapResSearch.h    
//  Version     :   1.0
//  Creater     :   dinglong
//  Date        :   2009-8-27 10:00
//  Comment     :   地图场景资源搜索工具界面和逻辑处理类
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KGMapResSearch_H__
#define _KGMapResSearch_H__

#include "resource.h"
#include "stdafx.h"
#include "KG3DInterface.h"
#include "IEKG3DTerrain.h"
#include <commctrl.h>

class KGMapResSearch
{
public:
	KGMapResSearch(void);
	~KGMapResSearch(void);

	int  Init(HINSTANCE hInstance, IKG3DEngineManager* pEngineMgr, const char cszClientPath[]);
	void UnInit();

	int  InitUI(HWND hDlg);
	int  ListViewUpdate(HWND hWnd, LPARAM lParam);
	//启用多资源查询
	int  EnableMultiSearch();
	//启用单资源查询
	int  EnableSingleSearch();
	//在资源列表中搜索指定资源信息
	int  Search();
	//打开ListView中选中的地图场景
	int  OpenMap();
	//处理ListView的消息
	int  CatchListViewMessage(WPARAM wParam, LPARAM lParam);
	//复制某条搜索结果记录到剪切版，目前只复制资源坐标
	int  CopyResult(WPARAM wParam);

private:
	//资源详细信息数据结构
	struct ResInfo
	{
		vector<string> MapNameList;				//资源所在的地图名称列表
		vector<D3DXVECTOR3> PositionList;		//对应地图中的资源位置
	};
	//解析用户输入
	bool MultiSearchAnalyseInput();
	bool SingleSearchAnalyseInput();
	//输出多资源查找结果
	int OutputMultiSearchResult();
	//读取要搜索的资源列表（全路径或相对于运行程序目录）
	int  ReadResList(const char cszListFileName[]);
	//关联资源类型的添加
	int  AddRelationType();

	//根据用户输入的搜索资源类型筛选出所有地图场景中使用的资源
	int  FindResInAllMap();
	int  AnalyseOneResourceInfoInOldMap(const char cszMapName[]);
	int  AnalyseEntityInfoInOldMap(IEKG3DSceneOutDoorSpaceMgr* pIEOutDoorMgr);
	int  InsertUsedResource(const char cszResourceName[]);

	//读*.map文件，搜索出地图场景中每个资源坐标位置
	int  FindResPosInMapFile(const char cszMapName[]);

	//在筛选出的资源列表中查找某个资源
	int  SearchResInList(const char cszResName[], ResInfo** ppResInfo);

	//新地图格式的查找/////////////////////////////////////////////////////////
	//int  AnalyseOneResourceInfoInNewMap(const char cszMapName[]);
	//int  AnalyseEntityInfoInNewMap(IEKG3DSceneOutDoorSpaceMgr* pIEOutDoorMgr);
	//////////////////////////////////////////////////////////////////////////

	//辅助分析资源文件
	int  FindResInModelset(const char cszResourceName[]);
	int  FindResInPVS(const char cszResourceName[]);
	int  FindResInSTree(const char cszResourceName[]);
	int  FindResInMDL(const char cszResourceName[]);
	int  FindResInSFX(const char cszResourceName[]);

private:
	typedef map<string, ResInfo*> ResInfoMap;			//string:资源名称	ResInfo*：资源详细信息实例指针
	typedef map<string, ResInfoMap> SourceResMap;		//string:资源类型	ResInfoMap：资源详细信息集合
	typedef map<UINT, vector<D3DXVECTOR3>> ResPosMap;	//UINT:资源的EntityInfoHandle　vector<D3DXVECTOR3>坐标集合

	ResPosMap m_ResPosList;					//资源坐标列表
	SourceResMap m_MapResList;				//地图场景所包含的资源列表
	set<string> m_ResTypeList;				//搜索资源类型列表
	vector<string> m_vecSearchResNameList;	//搜索的资源名称列表
	ResInfo* m_pSearchResult;				//搜索结果

	string m_strCurrentMapName;				//当前正在搜索的地图场景名称
	int m_nCurrentSelection;				//当前选中的ListView记录
	int m_nCurrentID;						//当前正在搜索的资源EntityInfoHandle
	int m_nListViewLine;					//ListView的显示行数
	bool m_bNeedFindRes;					//标记是否需要重新筛选资源
	char m_szClientPath[MAX_PATH];			//客户端目录
	char m_szSearchResName[MAX_PATH];		//搜索的资源名称
	char m_szLastSearchType[MAX_PATH];		//搜索的资源类型

	IKG3DEngineManager* m_pEngineMgr;
	HINSTANCE m_hInstance;
	HWND m_hDlg;
	//控件句柄
	HWND m_hRadioBtnMultiSearch;
	HWND m_hRadioBtnSingleSearch;
	HWND m_hStaticMultiResText;
	HWND m_hEditMultiResText;
	HWND m_hStaticResName;
	HWND m_hStaticResType;
	HWND m_hStaticResult;
	HWND m_hEditResName;
	HWND m_hEditResType;
	HWND m_hListViewResult;
};

#endif