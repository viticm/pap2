#include "KGMapResSearch.h"
#include "KG3DSceneDataInterface.h"
#include "IEKG3DScene.h"
#include "IEEditor.h"
#include "KG3DMemoryFile.h"
#include "KG3DSceneDataInterface.h"
#include "KG3DModelTable.h"
#include "KG3DModelST.h"

extern char g_szApplicationPath[MAX_PATH];

//参照KG3DRepresentObjectPVS
#define PVS_FILE_VERSION_1      100 
#define PVS_FILE_VERSION        200 
#define PVS_PORTAL_NAME_MAX_LEN 32
#define PVS_SUOBJ_MAX_MDL_NUM   32

const int EM_BUFFER_SIZE = 1024 * 1024 * 4;		//参照KG3DMemoryFile
const int MAX_LIST_COUNT = 100;

const char STR_UI_STATIC_RESLIST[]				= "资源名称列表(.txt文件，每行记录一个资源)";
const char STR_UI_STATIC_RESNAME[]				= "资源文件名(如: *.mesh)";
const char STR_UI_STATIC_SEARCHTYPE[]			= "搜索的资源类型(如：.mesh|.pvs)";
const char STR_UI_STATIC_SEARCHRESULT[]			= "搜索结果";
const char STR_UI_LISTVIEW_HEADLINE[][MAX_PATH] = {"资源名称", "地图名称", "资源坐标"};
const char STR_UI_RADIO_SEARCHMULTI[]			= "搜索多个资源";
const char STR_UI_RADIO_SEARCHSINGLE[]			= "搜索单个资源";
const char STR_UI_BTN_SEARCH[]					= "搜索资源";
const char STR_UI_BTN_OPEN[]					= "打开场景";


KGMapResSearch::KGMapResSearch(void)
{
	m_ResPosList.clear();
	m_MapResList.clear();
	m_ResTypeList.clear();

	m_nCurrentSelection = -1;
	m_nCurrentID		= 0;
	m_nListViewLine		= 0;
	m_bNeedFindRes		= true;
	m_szClientPath[0]		= '\0';
	m_szSearchResName[0]	= '\0';
	m_szLastSearchType[0]	= '\0';

	m_pSearchResult		= NULL;
	m_pEngineMgr		= NULL;
	m_hInstance			= NULL;
	m_hListViewResult	= NULL;
	m_hDlg				= NULL;
}

KGMapResSearch::~KGMapResSearch(void)
{
	UnInit();
}

int KGMapResSearch::Init(HINSTANCE hInstance, IKG3DEngineManager* pEngineMgr, const char cszClientPath[])
{

	int nResult  = false;
	int nRetCode = false;

	KG_ASSERT_EXIT(hInstance);
	KG_ASSERT_EXIT(pEngineMgr);
	KG_ASSERT_EXIT(cszClientPath);
	KGLOG_PROCESS_ERROR(cszClientPath[0] != '\0');

	m_hInstance = hInstance;
	m_pEngineMgr = pEngineMgr;
	nRetCode = strncpy_s(m_szClientPath, sizeof(m_szClientPath), cszClientPath, strlen(cszClientPath));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nResult = true;
Exit0:
	return nResult;
}


void KGMapResSearch::UnInit()
{
	int nResult  = false;
	int nRetCode = false;
	SourceResMap::iterator iterSourceRes;
	ResInfoMap::iterator iterResInfo;

	for (iterSourceRes = m_MapResList.begin();
		iterSourceRes != m_MapResList.end();
		++iterSourceRes)
	{
		for (iterResInfo = iterSourceRes->second.begin();
			iterResInfo != iterSourceRes->second.end();
			++iterResInfo)
		{
			SAFE_DELETE(iterResInfo->second);
		}
		iterSourceRes->second.clear();
	}
	m_ResPosList.clear();
	m_MapResList.clear();
	m_ResTypeList.clear();

	m_pSearchResult = NULL;
	m_pEngineMgr	= NULL;
	m_hInstance		= NULL;
	m_hListViewResult		= NULL;
	m_hDlg			= NULL;

}

int KGMapResSearch::InitUI(HWND hDlg)
{
	int nResult  = false;
	int nRetCode = false;
	int nBorder		= 20;		//控件与窗口边框的距离
	int nHeight     = nBorder;	//控件与窗口上边距的距离
	int nCtlHeight	= 30;		//控件高度
	int nCtlWidth	= 0;		//控件宽度
	HWND hWnd = NULL;
	RECT rect;

	KG_ASSERT_EXIT(hDlg);
	m_hDlg = hDlg;
	nRetCode = ::GetClientRect(m_hDlg, &rect);
	KGLOG_PROCESS_ERROR(nRetCode);
	InitCommonControls();

	//单选控件：多个资源查询
	nCtlWidth = rect.right - nBorder * 2;
	m_hRadioBtnMultiSearch = CreateWindow("button", STR_UI_RADIO_SEARCHMULTI, WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		nBorder, nHeight, nCtlWidth, nCtlHeight,
		hDlg, (HMENU)IDC_RADIO_MULTI, m_hInstance, NULL);
	KGLOG_PROCESS_ERROR(m_hRadioBtnMultiSearch);

	//静态文件控件：资源文件名
	nHeight += nCtlHeight;
	m_hStaticMultiResText = CreateWindow("STATIC", STR_UI_STATIC_RESLIST, WS_CHILD|WS_VISIBLE|SS_CENTERIMAGE,
		nBorder, nHeight, nCtlWidth, nCtlHeight,
		hDlg, (HMENU)IDC_STATIC_MULTIRESSEARCH, m_hInstance, NULL);
	KGLOG_PROCESS_ERROR(m_hStaticMultiResText);
	EnableWindow(m_hStaticMultiResText, false);

	//文本框控件：输入资源文件名
	nHeight += nCtlHeight;
	m_hEditMultiResText = CreateWindowEx(WS_EX_STATICEDGE, "edit", "",	WS_VISIBLE|WS_CHILD|WS_BORDER,
		nBorder, nHeight, nCtlWidth, nCtlHeight,
		m_hDlg, (HMENU)IDC_EDIT_MULTIRESSEARCH, m_hInstance, NULL);
	KGLOG_PROCESS_ERROR(m_hEditMultiResText);
	EnableWindow(m_hEditMultiResText, false);

	//单选控件：单个资源查询
	nHeight += nCtlHeight;
	m_hRadioBtnSingleSearch = CreateWindow("button", STR_UI_RADIO_SEARCHSINGLE, WS_CHILD | WS_VISIBLE | BS_RADIOBUTTON,
		nBorder, nHeight, nCtlWidth, nCtlHeight,
		hDlg, (HMENU)IDC_RADIO_SINGLE, m_hInstance, NULL);
	KGLOG_PROCESS_ERROR(m_hRadioBtnSingleSearch);
	::SendMessage(m_hRadioBtnSingleSearch, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);

	//静态文件控件：资源文件名
	nHeight += nCtlHeight;
	m_hStaticResName = CreateWindow("STATIC", STR_UI_STATIC_RESNAME, WS_CHILD|WS_VISIBLE|SS_CENTERIMAGE,
		nBorder, nHeight, nCtlWidth, nCtlHeight,
		hDlg, (HMENU)IDC_STATIC_SEARCHTEXT, m_hInstance, NULL);
	KGLOG_PROCESS_ERROR(m_hStaticResName);

	//文本框控件：输入资源文件名
	nHeight += nCtlHeight;
	m_hEditResName = CreateWindowEx(WS_EX_STATICEDGE, "edit", "",	WS_VISIBLE|WS_CHILD|WS_BORDER,
		nBorder, nHeight, nCtlWidth, nCtlHeight,
		m_hDlg, (HMENU)IDC_EDIT_SEARCHTEXT, m_hInstance, NULL);
	KGLOG_PROCESS_ERROR(m_hEditResName);

	//静态文件控件：搜索的资源类型(如：.mesh|.pvs)
	nHeight += nCtlHeight;
	m_hStaticResType = CreateWindow("STATIC", STR_UI_STATIC_SEARCHTYPE, WS_CHILD|WS_VISIBLE|SS_CENTERIMAGE ,
		nBorder, nHeight, nCtlWidth, nCtlHeight,
		hDlg, (HMENU)IDC_STATIC_TYPE, m_hInstance, NULL);
	KGLOG_PROCESS_ERROR(m_hStaticResType);

	//文本框控件：输入搜索的资源类型
	nHeight += nCtlHeight;
	m_hEditResType = CreateWindowEx(WS_EX_STATICEDGE, "edit", "",
		WS_VISIBLE|WS_CHILD|WS_BORDER,
		nBorder, nHeight, nCtlWidth, nCtlHeight,
		m_hDlg, (HMENU)IDC_EDIT_TYPETEXT, m_hInstance, NULL);
	KGLOG_PROCESS_ERROR(m_hEditResType);

	//静态文件控件：搜索结果
	nHeight += nCtlHeight;
	m_hStaticResult = CreateWindow("STATIC", STR_UI_STATIC_SEARCHRESULT, WS_CHILD|WS_VISIBLE|SS_CENTERIMAGE ,
		nBorder, nHeight, nCtlWidth, nCtlHeight,
		hDlg, (HMENU)IDC_STATIC_RESULT, m_hInstance, NULL);
	KGLOG_PROCESS_ERROR(m_hStaticResult);

	{//ListView控件
		DWORD dwStyle = 0;
		LV_COLUMN lvColumn;
		dwStyle = WS_TABSTOP | WS_CHILD | WS_BORDER | WS_VISIBLE |
			LVS_AUTOARRANGE | LVS_REPORT | LVS_OWNERDATA;
		nHeight += nCtlHeight;

		m_hListViewResult = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "", dwStyle,
			nBorder, nHeight, nCtlWidth, nCtlHeight * 5,
			hDlg, (HMENU)IDC_LISTVIEW, m_hInstance, NULL);
		KGLOG_PROCESS_ERROR(m_hListViewResult);

		lvColumn.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
		lvColumn.fmt = LVCFMT_LEFT;
		lvColumn.cx = nCtlWidth / _countof(STR_UI_LISTVIEW_HEADLINE) - 2;
		for (int i = 0; i < _countof(STR_UI_LISTVIEW_HEADLINE); i++)
		{
			lvColumn.pszText = (LPSTR)STR_UI_LISTVIEW_HEADLINE[i];
			ListView_InsertColumn(m_hListViewResult, i, &lvColumn);
		}
	}

	//按钮控件：搜索资源
	nHeight += nCtlHeight * 6;
	nCtlWidth = (rect.right - nBorder * 2) / 3;
	hWnd = CreateWindow ("button", STR_UI_BTN_SEARCH, WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,
		nBorder + nCtlWidth, nHeight, nCtlWidth, nCtlHeight,
		m_hDlg, (HMENU)IDC_BUTTON_SEARCH, m_hInstance,	NULL);
	KGLOG_PROCESS_ERROR(hWnd);
	SetFocus(hWnd);

	//按钮控件：打开场景
// 	hWnd = CreateWindow ("button", STR_UI_BTN_OPEN, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
// 		nBorder + nCtlWidth * 2, nBorder + nCtlHeight * 12, nCtlWidth, nCtlHeight,
// 		m_hDlg, (HMENU)IDC_BUTTON_OPENMAP, m_hInstance,	NULL);
// 	KGLOG_PROCESS_ERROR(hWnd);
	
	nResult = true;
Exit0:
	return nResult;
}

int KGMapResSearch::ListViewUpdate(HWND hWnd, LPARAM lParam)
{
	KG_PROCESS_SUCCESS(m_pSearchResult == NULL);

	LPNMHDR  lpnmh = (LPNMHDR)lParam;
	switch(lpnmh->code)
	{
	case LVN_GETDISPINFO:
		{
			LV_DISPINFO *lpdi = (LV_DISPINFO*)lParam;
			char szString[MAX_PATH];

			switch(lpdi->item.iSubItem)
			{
			case 0:
				if(lpdi->item.mask & LVIF_TEXT)
				{
					KG_PROCESS_SUCCESS(m_pSearchResult->MapNameList.size() <= (size_t)lpdi->item.iItem);
					_snprintf_s(szString, _countof(szString), _TRUNCATE,
						"%s", m_pSearchResult->MapNameList[lpdi->item.iItem].c_str());
					strncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,
					szString, _TRUNCATE);
				}
				//图标
				if(lpdi->item.mask & LVIF_IMAGE)
				{
					lpdi->item.iImage = 0;
				}
				break;
			case 1:
				if(lpdi->item.mask & LVIF_TEXT)
				{
					KG_PROCESS_SUCCESS(m_pSearchResult->PositionList.size() <= (size_t)lpdi->item.iItem);
					_snprintf_s(szString, _countof(szString), _TRUNCATE,
						"%.2f, %.2f, %.2f",
						m_pSearchResult->PositionList[lpdi->item.iItem].x,
						m_pSearchResult->PositionList[lpdi->item.iItem].y,
						m_pSearchResult->PositionList[lpdi->item.iItem].z);
					strncpy_s(lpdi->item.pszText, lpdi->item.cchTextMax,
						szString, _TRUNCATE);
				}
				break;
			default:
				_ASSERTE(false);
			}
		}
		break;
	}

Exit1:
	return true;
}

int KGMapResSearch::Search()
{
	int nResult  = false;
	int nRetCode = false;

	if(IsDlgButtonChecked(m_hDlg, IDC_RADIO_MULTI))
	{
		nRetCode = MultiSearchAnalyseInput(); 
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = FindResInAllMap();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = OutputMultiSearchResult();
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		nRetCode = SingleSearchAnalyseInput(); 
		KGLOG_PROCESS_ERROR(nRetCode);
		if (m_bNeedFindRes)
		{
			nRetCode = FindResInAllMap();
			KGLOG_PROCESS_ERROR(nRetCode);
			m_bNeedFindRes = false;
		}
		nRetCode = SearchResInList(m_szSearchResName, &m_pSearchResult);
		KGLOG_PROCESS_ERROR(nRetCode);
		if (m_pSearchResult)
		{
			m_nListViewLine = (int)m_pSearchResult->MapNameList.size();
			::MessageBox(m_hDlg, "查找结果已输出到搜索结果列表", "查找完毕", MB_ICONINFORMATION);	
		}
		else
		{
			m_nListViewLine = 1;
			::MessageBox(m_hDlg, "在地图场景中搜索不到指定资源", "提示", MB_ICONINFORMATION);			
		}
	}

	nResult = true;
Exit0:
	ListView_DeleteAllItems(m_hListViewResult);
	ListView_SetItemCount(m_hListViewResult, m_nListViewLine);
	return nResult;
}

bool KGMapResSearch::SingleSearchAnalyseInput()
{
	bool bResult  = false;
	int nRetCode = false;
	char szCurrentSearchType[MAX_PATH] = {0};
	char szExt[MAX_PATH]		= {0};
	char* pszResult = NULL;
	
	//判断输入的资源名称和资源搜索类型不为空
	nRetCode = ::GetDlgItemText(m_hDlg, IDC_EDIT_SEARCHTEXT, m_szSearchResName, sizeof(m_szSearchResName));
	if (m_szSearchResName[0] == '\0')
	{
		::MessageBox(m_hDlg, "请输入正确的搜索资源名称", "提示", MB_ICONINFORMATION);
		KG_PROCESS_ERROR(false);
	}
	nRetCode = ::GetDlgItemText(m_hDlg, IDC_EDIT_TYPETEXT, szCurrentSearchType, sizeof(szCurrentSearchType));
	if (szCurrentSearchType[0] == '\0')
	{
		::MessageBox(m_hDlg, "请输入正确的搜索资源类型", "提示", MB_ICONINFORMATION);
		KG_PROCESS_ERROR(false);
	}

	//判断所填的资源名称是否存在于搜索的资源类型范围里
	nRetCode = _splitpath_s(m_szSearchResName, NULL, 0, NULL, 0, NULL, 0, szExt, sizeof(szExt));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _strlwr_s(szExt, sizeof(szExt));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _strlwr_s(szCurrentSearchType, sizeof(szCurrentSearchType));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	pszResult = strstr(szCurrentSearchType, szExt);
	if (pszResult == NULL)
	{
		::MessageBox(m_hDlg, "搜索的资源和搜索的类型没有对应，请重新确认", "提示", MB_ICONINFORMATION);
		KG_PROCESS_ERROR(false);
	}

	if (strncmp(szCurrentSearchType, m_szLastSearchType, sizeof(szCurrentSearchType)) != 0)
	{
		char szSep[] = "|";
		char* pszToken = NULL;

		nRetCode = strncpy_s(
			m_szLastSearchType, sizeof(m_szLastSearchType),
			szCurrentSearchType, strlen(szCurrentSearchType));
		KGLOG_PROCESS_ERROR(nRetCode == 0);

		pszToken = strtok(szCurrentSearchType, szSep);   
		while(pszToken != NULL)   
		{
			m_ResTypeList.insert(pszToken);   
			pszToken = strtok(NULL, szSep);   
		}

		nRetCode = AddRelationType();
		KGLOG_PROCESS_ERROR(nRetCode);
		
		m_bNeedFindRes = true;
	}

	bResult = true;
Exit0:
	return bResult;
}

int KGMapResSearch::AddRelationType()
{
	int nRetCode = false;
	int nResult	 = false;
	set<string>::iterator iter;
	//如果搜索的类型中包含.mesh，则还要搜索以下资源类型中包含的.mesh
	iter = find(m_ResTypeList.begin(), m_ResTypeList.end(), ".mesh");
	if (iter != m_ResTypeList.end())
	{
		m_ResTypeList.insert(".pvs");
		m_ResTypeList.insert(".modelset");
		m_ResTypeList.insert(".mdl");
		m_ResTypeList.insert(".stree");
		m_ResTypeList.insert(".sfx");
	}
	//如果搜索的类型中包含.mtl，则还要搜索以下资源类型中包含的.mtl
	iter = find(m_ResTypeList.begin(), m_ResTypeList.end(), ".mtl");
	if (iter != m_ResTypeList.end())
	{
		m_ResTypeList.insert(".pvs");
		m_ResTypeList.insert(".modelset");
		m_ResTypeList.insert(".mdl");
		m_ResTypeList.insert(".stree");
		m_ResTypeList.insert(".sfx");
	}
	//如果搜索的类型中包含.ani，则还要搜索以下资源类型中包含的.ani
	iter = find(m_ResTypeList.begin(), m_ResTypeList.end(), ".ani");
	if (iter != m_ResTypeList.end())
	{
		m_ResTypeList.insert(".modelset");
		m_ResTypeList.insert(".sfx");
	}
	nResult = true;
Exit0:
	return nResult;
}

int KGMapResSearch::FindResInAllMap()
{
	int nRetCode = false;
	int nResult	 = false;				
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	char szAllFile[MAX_PATH] = {0};
	char szMapName[MAX_PATH] = {0};
	WIN32_FIND_DATA FindFileData;

	nRetCode = _snprintf_s(szAllFile,
		sizeof(szAllFile),
		sizeof(szAllFile) - 1,
		"%s%s",
		m_szClientPath,
		"data\\source\\maps\\*.*");
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	hFindFile = ::FindFirstFile(szAllFile, &FindFileData);
	KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);
	while (true)
	{
		//过滤"."、".."、".svn"
		if (FindFileData.cFileName[0] != '.')
		{
			if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{

				nRetCode = _snprintf_s(szMapName,
					sizeof(szMapName),
					sizeof(szMapName) - 1,
					"%sdata\\source\\maps\\%s\\%s.map",
					m_szClientPath,
					FindFileData.cFileName,
					FindFileData.cFileName
					);
				KGLOG_PROCESS_ERROR(nRetCode > 0);
				nRetCode = g_IsFileExist(szMapName);
				if (nRetCode)
				{
					string strName = FindFileData.cFileName;
					KGLogPrintf(KGLOG_INFO, "Find MapRes In %s", szMapName);
					nRetCode = AnalyseOneResourceInfoInOldMap(szMapName);
					KGLOG_PROCESS_ERROR(nRetCode);
				}

			}
		}
		nRetCode = ::FindNextFile(hFindFile, &FindFileData);
		if (!nRetCode)
		{
			nRetCode = ::GetLastError();
			KG_PROCESS_SUCCESS(nRetCode == ERROR_NO_MORE_FILES);
			KGLOG_PROCESS_ERROR(false);
		}
	}

Exit1:
	nResult = true;
Exit0:
	if (!nResult)
	{
		::MessageBox(m_hDlg, "在所有地图场景中查找指定类型的资源失败", "提示", MB_ICONINFORMATION);
		m_pSearchResult = NULL;
		m_nListViewLine = 0;
	}
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile);
		hFindFile = INVALID_HANDLE_VALUE;
	}
	return nResult;
}


int KGMapResSearch::AnalyseOneResourceInfoInOldMap(const char cszMapName[])
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	IEKG3DSceneSceneEditor*		pIEEditor     = NULL;
	IEKG3DSceneOutDoorSpaceMgr* pIEOutDoorMgr = NULL;
	IEKG3DScene*				pScene		  = NULL;

	KG_ASSERT_EXIT(cszMapName);
	KGLOG_PROCESS_ERROR(cszMapName[0] != '\0');

#if defined(DEBUG) | defined(_DEBUG)
	KG3D_GAME_INIT_CONTEXT Context;
	m_pEngineMgr->InitGameContext(&Context);
#endif
	hrRetCode = m_pEngineMgr->NewOneScene(SCENETYPE_SCENEEDITOR, (IKG3DScene**)&pScene);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	hrRetCode = pScene->GetSceneSceneEditor(&pIEEditor);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	pIEOutDoorMgr = pIEEditor->GetIEKG3DSceneOutDoorSpaceMgr();
	KGLOG_PROCESS_ERROR(pIEOutDoorMgr);

	hrRetCode = pIEOutDoorMgr->LoadFromFile(cszMapName, 0, 0);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	m_strCurrentMapName = cszMapName;
	
	nRetCode = AnalyseEntityInfoInOldMap(pIEOutDoorMgr);
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	if (pScene)
	{
		hrRetCode = m_pEngineMgr->RemoveScene(pScene);
		KGLOG_COM_CHECK_ERROR(hrRetCode);
		pScene = NULL;
	}
	return nResult;
}

int KGMapResSearch::AnalyseEntityInfoInOldMap(IEKG3DSceneOutDoorSpaceMgr* pIEOutDoorMgr)
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	size_t uPos	 = 0;
	vector<UINT> vecEntityHandle;
	KG3DMemoryFile* pMemoryFile = NULL;

	KG_ASSERT_EXIT(pIEOutDoorMgr);

 	nRetCode = FindResPosInMapFile(m_strCurrentMapName.c_str());
 	KGLOG_PROCESS_ERROR(nRetCode);

	pMemoryFile = new KG3DMemoryFile();
	KGLOG_PROCESS_ERROR(pMemoryFile);
	hrRetCode = pIEOutDoorMgr->GetAllEntityInfoHandle(&vecEntityHandle);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	for (UINT i = 0; i < vecEntityHandle.size(); i++)
	{
		DWORD dwType	   = 0;
		DWORD dwLength	   = 0;
		DWORD dwModelCount = 0;
		DWORD dwVersion	   = 0;
		ResPosMap::iterator iter;

 		iter = m_ResPosList.find(vecEntityHandle[i]);
 		KG_ASSERT_EXIT(iter != m_ResPosList.end());
 		m_nCurrentID = vecEntityHandle[i];

		hrRetCode = pIEOutDoorMgr->GetEntityInfoformation(
			m_nCurrentID,
			&dwType,
			&dwLength,
			pMemoryFile->GetBuffer(),
			EM_BUFFER_SIZE);
		KGLOG_COM_PROCESS_ERROR(hrRetCode || i != 0);
		if (FAILED(hrRetCode))
		{
			KGLogPrintf(KGLOG_ERR, "It must be something wrong with %d.EntityInfo", vecEntityHandle[i]);
			continue;
		}

 		switch (dwType)
 		{
 		case REPRESENTOBJECT_DEFAULT:
 			{
 				//参照KG3DRepresentObject::LoadFromInformationBuffer
 				pMemoryFile->read(&dwModelCount, sizeof(DWORD));
 				pMemoryFile->read(&dwVersion, sizeof(DWORD));
 
 				for (DWORD i = 0; i < dwModelCount; i++)
 				{
 					char szMeshName[MAX_PATH] = {0};
 					char szMtlName [MAX_PATH] = {0};
 					char szAniName [MAX_PATH] = {0};
 
 					pMemoryFile->read(szMeshName, sizeof(char) * MAX_PATH);//Mesh
 					_ASSERTE(szMeshName[0] != '\0');
 					InsertUsedResource(szMeshName);
 					pMemoryFile->read(szMtlName, sizeof(char) * MAX_PATH);//Mtl
 					InsertUsedResource(szMtlName);
 					pMemoryFile->read(szAniName, sizeof(char) * MAX_PATH);//Ani
					InsertUsedResource(szAniName);
 
 					uPos = pMemoryFile->GetPos();
 					uPos += sizeof(D3DXVECTOR3) +		//m_vScaling
 						sizeof(D3DXVECTOR3) +			//m_vScalingCenter
 						sizeof(D3DXQUATERNION) +		//m_vScalingRotation
 						sizeof(D3DXQUATERNION) +		//m_vRotation
 						sizeof(D3DXVECTOR3) +			//m_vRotaionCenter
 						sizeof(D3DXVECTOR3);			//m_vTranslation
 					pMemoryFile->SetPos(uPos);
 
 					{
 						TypeInfo* pTypeInfo				  = NULL;
 						IEKG3DEngineManager* pIEEngineMgr = NULL;
 						IEKG3DModelTable* pIEModelTable   = NULL;
 						KG3DModelTable* pModelTable       = NULL;
 
 						pIEEngineMgr = dynamic_cast<IEKG3DEngineManager*>(m_pEngineMgr);
 						hrRetCode = pIEEngineMgr->GetIEKG3DModelTable(&pIEModelTable);
 						KGLOG_COM_PROCESS_ERROR(hrRetCode);
 						pModelTable= dynamic_cast<KG3DModelTable*>(pIEModelTable);
 						hrRetCode = pModelTable->GetTypeInfoByFileName(&pTypeInfo, szMeshName);
 						if (FAILED(hrRetCode))
 						{
 							KGLogPrintf(KGLOG_ERR, "GetTypeInfoByFileName Faild! MeshName: %s", szMeshName);
 							continue;
 						}
 						if (pTypeInfo && pTypeInfo->dwType == MESHTYPE_POINTLIGHT)
 						{
 							D3DCOLORVALUE colorPointLight;
 							pMemoryFile->read(&colorPointLight, sizeof(colorPointLight));
 						}
 					}
 				}
 				if(dwVersion >= 1)
 				{
 					DWORD dwNum = 0;
 					int nTypeSource = 0;
 					int nIndex		= 0;
 					char szSourceName[MAX_PATH] = {0};
 
 					pMemoryFile->read(&dwNum, sizeof(DWORD));
 					for(DWORD i = 0; i < dwNum; i++)
 					{
 						pMemoryFile->read(&nTypeSource, sizeof(int));
 						pMemoryFile->read(&nIndex, sizeof(int));
 						pMemoryFile->read(szSourceName, sizeof(char) * MAX_PATH);
 						InsertUsedResource(szSourceName);
 					}
 				}
 			}
 			break;
 		case REPRESENTOBJECT_SET:
 			{
 				//参照KG3DRepresentObjectSet::LoadFromInformationBuffer
 				int nCurrentModel = 0;
 				DWORD dwVersion = 0;
 				char szSourceName[MAX_PATH]		= {0};
 				char szSourcePathName[MAX_PATH] = {0};
 				pMemoryFile->read(&nCurrentModel, sizeof(int));
 				pMemoryFile->read(&dwVersion, sizeof(DWORD));
 				pMemoryFile->read(szSourceName, sizeof(char) * MAX_PATH);
 				InsertUsedResource(szSourceName);
 			}
 			break;
 		case REPRESENTOBJECT_SOUNDBALL:
 			{
 				//参照KG3DRepresentObjectSoundBall::LoadFromInformationBuffer
 				char szSoundName[MAX_PATH] = {0};
 				DWORD dwVersion = 0;
 
 				pMemoryFile->read(&dwVersion, sizeof(DWORD));
 				pMemoryFile->read(szSoundName, sizeof(char) * MAX_PATH);
 				InsertUsedResource(szSoundName);
 			}
 			break;
 		case REPRESENTOBJECT_PVS:
 			{
 				//参照KG3DRepresentObjectPVS::LoadFromInformationBuffer
 				char szName[MAX_PATH] = {0};
 				pMemoryFile->read(szName, sizeof(char) * MAX_PATH);
 				InsertUsedResource(szName);
 			}
 			break;
 		default:
 			_ASSERTE(false);
 			break;
 		}
		pMemoryFile->Reset();
	}

	nResult = true;
Exit0:
	if (pMemoryFile)
	{
		pMemoryFile->UnInit();
		SAFE_DELETE(pMemoryFile);
		pMemoryFile = NULL;
	}
	return nResult;
}

int KGMapResSearch::SearchResInList(const char cszResName[], ResInfo** ppResInfo)
{
	int nResult  = false;
	int nRetCode = false;
	char szExt[MAX_PATH]		= {0};
	char szLowResName[MAX_PATH] = {0};
	ResInfoMap::iterator iter;

	KG_ASSERT_EXIT(cszResName);
	KGLOG_PROCESS_ERROR(cszResName[0] != '\0');

	nRetCode = strncpy_s(szLowResName, sizeof(szLowResName), cszResName, strlen(cszResName));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

// 	nRetCode = _mbslwr_s((unsigned char*)szLowResName, sizeof(szLowResName));//筞
// 	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = _strlwr_s(szLowResName, sizeof(szLowResName));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _splitpath_s(szLowResName, NULL, 0, NULL, 0, NULL, 0, szExt, sizeof(szExt));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	iter = m_MapResList[szExt].find(szLowResName);
	if(iter != m_MapResList[szExt].end())
	{
		*ppResInfo = m_MapResList[szExt][szLowResName];
		m_nListViewLine = (int)(*ppResInfo)->MapNameList.size();
	}
	else
	{
		*ppResInfo = NULL;
		m_nListViewLine = 1;
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGMapResSearch::InsertUsedResource(const char cszResourceName[])
{
	int nRetCode = false;
	int nResult  = false;
	char szFullName[MAX_PATH] = {0};
	char szName[MAX_PATH] = {0};
	char szExt[MAX_PATH]  = {0};
	set<string>::iterator iterResType;
	ResInfoMap::iterator iterResList;
	ResInfo* pResInfo = NULL;

	KG_ASSERT_EXIT(cszResourceName);
	KG_PROCESS_SUCCESS(cszResourceName[0] == '\0');

	//判断该资源是否为指定要搜索的资源
	nRetCode = _splitpath_s(cszResourceName, NULL, 0, NULL, 0, szName, sizeof(szName), szExt, sizeof(szExt));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _mbslwr_s((unsigned char*)szName, sizeof(szName));//筞
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _strlwr_s(szExt, sizeof(szExt));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	iterResType = find(m_ResTypeList.begin(), m_ResTypeList.end(), szExt);
	KG_PROCESS_SUCCESS(iterResType == m_ResTypeList.end());

	//先保存该资源再对资源做进一步解析
	nRetCode = _snprintf_s(szFullName,
		sizeof(szFullName),
		sizeof(szFullName) - 1,
		"%s%s",
		szName,
		szExt);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

 	iterResList = m_MapResList[szExt].find(szFullName);
 	if (iterResList == m_MapResList[szExt].end())
 	{
 		pResInfo = new ResInfo();
 		KGLOG_PROCESS_ERROR(pResInfo);
 		m_MapResList[szExt].insert(make_pair(szFullName, pResInfo));
 	}
	for (size_t i = 0; i < m_ResPosList[m_nCurrentID].size(); i++)
	{
		m_MapResList[szExt][szFullName]->MapNameList.push_back(m_strCurrentMapName);
		m_MapResList[szExt][szFullName]->PositionList.push_back(m_ResPosList[m_nCurrentID][i]);
	}
 	
	//进一步分析复杂类型的资源
	if (strncmp(szExt, ".modelset", sizeof(".modelset")) == 0)
	{
		nRetCode = FindResInModelset(cszResourceName);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	if (strncmp(szExt, ".pvs", sizeof(".pvs")) == 0)
	{
		nRetCode = FindResInPVS(cszResourceName);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	if (strncmp(szExt, ".stree", sizeof(".stree")) == 0)
	{
		nRetCode = FindResInSTree(cszResourceName);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	if (strncmp(szExt, ".mdl", sizeof(".mdl")) == 0)
	{
		nRetCode = FindResInMDL(cszResourceName);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	if (strncmp(szExt, ".sfx", sizeof(".sfx")) == 0)
	{
		nRetCode = FindResInSFX(cszResourceName);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

Exit1:
	nResult = true;
Exit0:
	if (!nResult)
	{
		KGLogPrintf(KGLOG_DEBUG, "InsertUsedResource %s faild", cszResourceName);
	}
	return nResult;
}

int KGMapResSearch::FindResPosInMapFile(const char cszMapName[])
{
	struct MapFileHeader		//参照KG3DSceneDataInterfaceCore
	{
		DWORD dwMask;
		DWORD dwVersion;
		UINT  uCurrentEntityID;
		UINT  uCurrentEntityInformationID;
		UINT  uCurrentTerrainInformationID;
		DWORD dwNumEntity;
		DWORD dwNumEntityInformation;
		DWORD dwNumTerrainInformation;
		DWORD dwNumSceneBlock[4];
		DWORD dwExtend[128];
	};
	int nResult  = false;
	int nRetCode = false;
	IFile* pFile = NULL;
	size_t uSize = 0;
	DWORD dwSize = 0;
	DWORD dwNum  = 0;
	MapFileHeader vHeader;

	KG_ASSERT_EXIT(cszMapName);
	KGLOG_PROCESS_ERROR(cszMapName[0] != '\0');

	m_ResPosList.clear();

	//参照KG3DSceneDataInterfaceCore::LoadFromFile
	pFile = g_OpenFile(cszMapName);
	KGLOG_PROCESS_ERROR(pFile);

	dwSize = sizeof(MapFileHeader);
	ZeroMemory(&vHeader, dwSize);

	uSize = pFile->Read(&vHeader, dwSize);
	KGLOG_PROCESS_ERROR(uSize == dwSize);

	if(vHeader.dwVersion >= MAP_FILE_VERSION_PACKREGION)//新地图格式
	{
		//新地图格式.map没有保存EntityInfo和TerrainInfo的信息
	}
	else
	{
		//参照KG3DSceneDataInterfaceCore::LoadEntityInfomationFromFile
		dwNum = 0;
		pFile->Read(&dwNum, sizeof(DWORD));
		for (DWORD i = 0; i < dwNum; i++)
		{
			UINT uHandle = 0;
			pFile->Read(&uHandle, sizeof(UINT));
		}
		//参照KG3DSceneDataInterfaceCore::LoadTerrainInfomationFromFile
		dwNum = 0;
		pFile->Read(&dwNum, sizeof(DWORD));
		for (DWORD i = 0; i < dwNum; i++)
		{
			UINT uHandle = 0;
			pFile->Read(&uHandle, sizeof(UINT));
		}
	}


	//参照KG3DSceneDataInterfaceCore::LoadSceneBlockFromFile
	for (int s = 3; s >= 0; s--)
	{
		dwNum = 0;
		pFile->Read(&dwNum, sizeof(DWORD));
		for (DWORD i = 0; i < dwNum; i++)
		{
			POINT Index;
			pFile->Read(&Index, sizeof(POINT));
		}
	}
	//参照KG3DSceneDataInterfaceCore::LoadEntityFromFile
	dwNum = 0;
	pFile->Read(&dwNum, sizeof(DWORD));
	for (DWORD i = 0; i< dwNum; i++)
	{
		UINT uHandle = 0;
		UINT uEntityInformationHandle = 0;
		UINT uParent = INVALID_OBJSHELL_HANDLE;
		KG3DEntityTransformation vTrans;
		vector<D3DXVECTOR3> PosList;
		ResPosMap::iterator iter;

		pFile->Read(&uHandle                 , sizeof(UINT));
 		pFile->Read(&uEntityInformationHandle, sizeof(UINT));
 
 		if (vHeader.dwVersion > MAP_FILE_VERSION_00)
 		{
 			pFile->Read(&vTrans, sizeof(KG3DEntityTransformation));
 			//获取模型在场景中的位置
			iter = m_ResPosList.find(uEntityInformationHandle);
			if (iter == m_ResPosList.end())
			{
				PosList.clear();
				PosList.push_back(vTrans.m_Translation);
				m_ResPosList.insert(make_pair(uEntityInformationHandle, PosList));
			}
			else
			{
				m_ResPosList[uEntityInformationHandle].push_back(vTrans.m_Translation);
			}
 		}
 
		if (vHeader.dwVersion > MAP_FILE_VERSION_00)
		{
 			pFile->Read(&uParent, sizeof(uParent));
		}
	}

	nResult = true;
Exit0:
	if (pFile)
	{
		pFile->Release();
		pFile = NULL;
	}
	return nResult;

}

int KGMapResSearch::OpenMap()
{
	int nRetCode = false;
	int nResult  = false;
	int nSelectedIndex = 0;

	nSelectedIndex = ListView_GetSelectionMark(m_hListViewResult);
	KGLOG_PROCESS_ERROR(nSelectedIndex != -1);
	KG_PROCESS_SUCCESS(m_pSearchResult->PositionList.size() <= (size_t)nSelectedIndex);
	m_pSearchResult->MapNameList[nSelectedIndex];//map name, full path

	//open map

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KGMapResSearch::CopyResult(WPARAM wParam)
{
	int nRetCode = false;
	int nResult  = false;
	char szCopyText[MAX_PATH] = {0};

	switch(LOWORD(wParam))
	{
	case ID_POPMENU_COPYPOS:
		{
			KG_PROCESS_SUCCESS(m_pSearchResult->PositionList.size() <= (size_t)m_nCurrentSelection);

			nRetCode = _snprintf_s(szCopyText, _countof(szCopyText), _TRUNCATE,
				"%.2f, %.2f, %.2f",
				m_pSearchResult->PositionList[m_nCurrentSelection].x,
				m_pSearchResult->PositionList[m_nCurrentSelection].y,
				m_pSearchResult->PositionList[m_nCurrentSelection].z);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
		}
		break;
	case ID_POPMENU_COPYMAP:
		{
			KG_PROCESS_SUCCESS(m_pSearchResult->MapNameList.size() <= (size_t)m_nCurrentSelection);

			nRetCode = _snprintf_s(szCopyText, _countof(szCopyText), _TRUNCATE,
				"%s",
				m_pSearchResult->MapNameList[m_nCurrentSelection].c_str());
			KGLOG_PROCESS_ERROR(nRetCode > 0);
		}
		break;
	default:
		_ASSERTE(false);
	}

	if (::OpenClipboard(NULL)) 
	{ 
		HGLOBAL hClipbuffer = NULL; 
		TCHAR* pBuffer		= NULL; 

		nRetCode = ::EmptyClipboard();
		KGLOG_PROCESS_ERROR(nRetCode);

		hClipbuffer = ::GlobalAlloc(GMEM_DDESHARE, strlen(szCopyText) + 1); 
		KGLOG_PROCESS_ERROR(hClipbuffer);

		pBuffer = (char*)::GlobalLock(hClipbuffer); 
		KGLOG_PROCESS_ERROR(pBuffer);

		strcpy(pBuffer, LPCSTR(szCopyText)); 
		::GlobalUnlock(hClipbuffer); 
		::SetClipboardData(CF_TEXT, hClipbuffer); 
		::CloseClipboard(); 
	}
	ListView_SetSelectionMark(m_hListViewResult, -1);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KGMapResSearch::CatchListViewMessage(WPARAM wParam, LPARAM lParam)
{	
	int nRetCode = false;
	int nResult  = false;
	int nSelectedIndex = 0;

	LPNMHDR  lpnmh = (LPNMHDR) lParam;
	// 	KG_PROCESS_SUCCESS(m_pSearchResult == NULL);
	// 	m_nCurrentSelection = ListView_GetSelectionMark(m_hListViewResult);
	// 	KG_PROCESS_SUCCESS(nSelectedIndex == -1 || m_pSearchResult->PositionList.size() <= nSelectedIndex);

	switch(lpnmh->code)
	{
	case NM_RCLICK:
		{
			nSelectedIndex = ListView_GetSelectionMark(m_hListViewResult);
			KG_PROCESS_SUCCESS(nSelectedIndex == -1);

			HWND  hwndListView = (HWND)wParam;
			HMENU hMenuLoad = NULL;
			HMENU hMenu = NULL;
			POINT point;

			hMenuLoad = ::LoadMenu(m_hInstance, MAKEINTRESOURCE(IDR_POPMENU_COPY));
			KGLOG_PROCESS_ERROR(hMenuLoad);

			hMenu = ::GetSubMenu(hMenuLoad, 0);
			KGLOG_PROCESS_ERROR(hMenu);

			nRetCode = ::GetCursorPos(&point);
			KGLOG_PROCESS_ERROR(nRetCode);

			nRetCode = ::SetForegroundWindow(m_hDlg);
			KGLOG_PROCESS_ERROR(nRetCode);
			nRetCode = ::TrackPopupMenu(hMenu,
				TPM_LEFTALIGN | TPM_RIGHTBUTTON,
				point.x,
				point.y,
				0,
				m_hDlg,
				NULL);
			KGLOG_PROCESS_ERROR(nRetCode);
			m_nCurrentSelection = nSelectedIndex;
		}
		break;
	default:
		break;
	}
	ListView_SetSelectionMark(m_hListViewResult, -1);

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KGMapResSearch::FindResInPVS( const char cszResourceName[] )
{
	//参照KG3DRepresentObjectPVS::LoadPvsFile
	int nRetCode = false;
	int nResult  = false;
	HRESULT hrRetCode = E_FAIL;
	IFile* pFile = NULL;
	DWORD dwVersion = PVS_FILE_VERSION;
	char szResName[MAX_PATH]		= {0};
	long lOffset  = 0;
	size_t objNum = 0;

	KG_ASSERT_EXIT(cszResourceName);
	KGLOG_PROCESS_ERROR(cszResourceName[0] != '\0');

	pFile = g_OpenFile(cszResourceName);
	KGLOG_PROCESS_ERROR(pFile);

	/* read version */
	pFile->Read(&dwVersion, sizeof(dwVersion));

	/* load border, the frist model of the pvs object is it's ouside border
	and the second model is it's inside border */
	pFile->Read(szResName, sizeof(szResName));
	InsertUsedResource(szResName);

	pFile->Read(szResName, sizeof(szResName));
	InsertUsedResource(szResName);

	/* load inside objects, the object may be is pvs object too */
	pFile->Read(&objNum, sizeof(objNum));

	for (size_t i = 0; i < objNum; i++)
	{
		DWORD dwObjType = 0;

		pFile->Read(&dwObjType, sizeof(dwObjType));

		switch (dwObjType)
		{
		case REPRESENTOBJECT_PVS :
		case REPRESENTOBJECT_SET :
			{
				pFile->Read(szResName, sizeof(szResName));
				InsertUsedResource(szResName);
			}
			break;
		case REPRESENTOBJECT_DEFAULT :
			{
				/* is normal object */
				DWORD dwCount = 0;
				DWORD dwColor = 0;
				DWORD dwType  = 0;
				char szAnins[MAX_PATH] = {0};

				pFile->Read(&dwCount, sizeof(dwCount));

				for (DWORD i = 0; i < dwCount; i++)
				{
					pFile->Read(szResName, sizeof(szResName));
					InsertUsedResource(szResName);
					if (dwVersion > PVS_FILE_VERSION_1)
					{
						pFile->Read(szAnins, sizeof(szAnins));
						InsertUsedResource(szAnins);
					}
					TypeInfo* pInfo				  = NULL;
					IEKG3DEngineManager* pIEEngineMgr = NULL;
					IEKG3DModelTable* pIEModelTable   = NULL;
					KG3DModelTable* pModelTable       = NULL;

					pIEEngineMgr = (IEKG3DEngineManager*)m_pEngineMgr;
					hrRetCode = pIEEngineMgr->GetIEKG3DModelTable(&pIEModelTable);
					KGLOG_COM_PROCESS_ERROR(hrRetCode);
					pModelTable= (KG3DModelTable*)pIEModelTable;
					hrRetCode = pModelTable->GetTypeInfoByFileName(&pInfo, szResName);

					if (pInfo)
						dwType = pInfo->dwType;
					else
						dwType = MESHTYPE_DEFAULT;

					switch (dwType)
					{
					case MESHTYPE_POINTLIGHT :
						{
							pFile->Read(&dwColor, sizeof(dwColor));
						}
						break;
					default :
						break;
					}
				}
			}
			break;
		default :
			_ASSERTE(false);
			break;
		}
		lOffset = sizeof(D3DXVECTOR3) +		//ScalingCenter
			sizeof(D3DXQUATERNION) +		//ScalingRotation
			sizeof(D3DXVECTOR3) +			//Scaling
			sizeof(D3DXVECTOR3) +			//RotationCenter
			sizeof(D3DXQUATERNION) +		//Rotation
			sizeof(D3DXVECTOR3) +			//Translation
			sizeof(AABBOX);					//aabox
		pFile->Seek(lOffset, SEEK_CUR);
		lOffset = 0;
	}
	pFile->Close();
	SAFE_RELEASE(pFile);

	nResult = true;
Exit0:
	if (pFile)
	{
		pFile->Close();
		SAFE_RELEASE(pFile);
	}
	if (!nResult)
	{
		KGLogPrintf(KGLOG_ERR, "Find Res In PVS %s failed.\n", cszResourceName);
	}
	return nResult;
}

int KGMapResSearch::FindResInSTree( const char cszResourceName[] )
{
	//参照KG3DModelSpeedTree::_LoadFromFile
	int nRetCode = false;
	int nResult  = false;
	IFile* pFile = NULL;
	DWORD dwMark = 0;
	char szSourcePathName[MAX_PATH] = {0};
	char szTreeMeshName[MAX_PATH]   = {0};
	char szTreeMtlName [MAX_PATH]   = {0};

	char szPathName[MAX_PATH]	= {0};
	char szDriver[MAX_PATH]		= {0};
	char szPath[MAX_PATH]		= {0};
	char szFileName[MAX_PATH]	= {0};
	char szExt[MAX_PATH]		= {0};

	KG_ASSERT_EXIT(cszResourceName);
	KGLOG_PROCESS_ERROR(cszResourceName[0] != '\0');

	pFile = g_OpenFile(cszResourceName);
	KGLOG_PROCESS_ERROR(pFile);

	pFile->Read(&dwMark, sizeof(DWORD));

	//Mesh Name
	pFile->Read(szTreeMeshName, sizeof(char) * MAX_PATH);
	nRetCode = _splitpath_s(
		cszResourceName, 
		szDriver, 
		sizeof(szDriver),
		szPath, 
		sizeof(szPath),
		NULL,
		0,
		NULL,
		0
		);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _splitpath_s(
		szTreeMeshName, 
		NULL, 
		0,
		NULL, 
		0, 
		szFileName, 
		sizeof(szFileName),
		szExt, 
		sizeof(szExt)
		);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _snprintf_s(
		szPathName, 
		sizeof(szPathName),
		sizeof(szPathName) - 1, 
		"%s%s", 
		szDriver, 
		szPath
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	nRetCode = _snprintf_s(
		szTreeMeshName, 
		sizeof(szTreeMeshName),
		sizeof(szTreeMeshName) - 1,
		"%s%s%s", 
		szPathName,
		szFileName,
		szExt
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	InsertUsedResource(szTreeMeshName);

	//Mtl Name
	pFile->Read(szTreeMtlName, sizeof(char) * MAX_PATH);
	nRetCode = _splitpath_s(
		szTreeMtlName, 
		NULL, 
		0,
		NULL, 
		0,
		szFileName,
		sizeof(szFileName),
		szExt,
		sizeof(szExt)
		);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _snprintf_s(
		szTreeMtlName, 
		sizeof(szTreeMtlName),
		sizeof(szTreeMtlName) - 1,
		"%s%s%s",
		szPathName,
		szFileName, 
		szExt
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	InsertUsedResource(szTreeMtlName);

	//Mtl Name
	nRetCode = _splitpath_s(
		cszResourceName, 
		NULL,
		0,
		NULL,
		0,
		szFileName, 
		sizeof(szFileName),
		NULL,
		0
		);
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _snprintf_s(
		szTreeMtlName, 
		sizeof(szTreeMtlName),
		sizeof(szTreeMtlName) - 1,
		"%s%s.Mtl",
		szPathName, 
		szFileName
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	InsertUsedResource(szTreeMtlName);

	nResult = true;
Exit0:
	if (pFile)
	{
		pFile->Close();
		SAFE_RELEASE(pFile);
	}
	if (!nResult)
	{
		KGLogPrintf(KGLOG_ERR, "Find Res In STree %s failed.\n", cszResourceName);
	}
	return nResult;
}

int KGMapResSearch::FindResInModelset( const char cszResourceName[] )
{
	//参照KG3DRepresentObjectSet::LoadFromIniFile
	int nRetCode = false;
	int nResult  = false;
	IIniFile* pIniFile = NULL;

	KG_ASSERT_EXIT(cszResourceName);
	KGLOG_PROCESS_ERROR(cszResourceName[0] != '\0');

	pIniFile = g_OpenIniFile(cszResourceName);
	KGLOG_PROCESS_ERROR(pIniFile);

	int nCount = 0;
	pIniFile->GetInteger("MAIN", "Count", 0, &nCount);

	for (int i = 0; i < nCount; i++)
	{
		char szSecName[MAX_PATH]  = {0};
		char szMeshName[MAX_PATH] = {0};
		char szMtlName[MAX_PATH]  = {0};

		nRetCode = _snprintf_s(szSecName, sizeof(szSecName), sizeof(szSecName) - 1, "Model%d", i);
		KGLOG_PROCESS_ERROR(nRetCode> 0);
		pIniFile->GetString(szSecName, "MeshName", "", szMeshName, sizeof(szMeshName));
		InsertUsedResource(szMeshName);
		pIniFile->GetString(szSecName, "MtlName", "", szMtlName, sizeof(szMtlName));
		InsertUsedResource(szMtlName);
	}

	nResult = true;
Exit0:
	if (pIniFile)
	{	
		pIniFile->Release();
		pIniFile = NULL;
	}
	if (!nResult)
	{
		KGLogPrintf(KGLOG_ERR, "Find Res In ModelSet %s failed.\n", cszResourceName);
	}
	return nResult;
}

int KGMapResSearch::FindResInMDL(const char cszResourceName[])
{
	//参照KG3DModelST::LoadMDLContent
	int nResult = false;
	char szFilePath[MAX_PATH] = {0};
	IFile* pFile = NULL;
	unsigned long uSize = 0;
	unsigned long uFileSize = 0;
	char* pBuffer = NULL;
	std::stringstream ss;
	std::string strBuffer;
	std::string strFilePath;
	MDLFileContent Content;

	KG_ASSERT_EXIT(cszResourceName);
	KGLOG_PROCESS_ERROR(cszResourceName[0] != '\0');

	Content.dwNumModels = 0;
	pFile = g_OpenFile(cszResourceName);
	KG_PROCESS_ERROR(pFile);

	uFileSize = pFile->Size();

	pBuffer = (char*)malloc(uFileSize + 1);
	KG_PROCESS_ERROR(pBuffer);

	uSize = pFile->Read(pBuffer, uFileSize);
	KG_PROCESS_ERROR(uSize == uFileSize);

	pBuffer[uSize] = '\0'; // TODO : text 文件没有使用'\0'作结束，不能作为字符串处理，特别麻烦，建议使用binary

	ss << pBuffer;

	std::getline(ss, strBuffer);
	strBuffer.erase(strBuffer.find_last_not_of("\n\r") + 1);
	KG_PROCESS_ERROR(!strBuffer.empty());

	g_ExtractFilePath(szFilePath, cszResourceName);

	strFilePath = szFilePath;
	strFilePath += "\\";

	if (strBuffer[0] == '\\')
	{
		Content.strBipFile = strBuffer;
	}
	else
	{
		Content.strBipFile = std::string(strFilePath + strBuffer);
	}
	InsertUsedResource(Content.strBipFile.c_str());

	while (std::getline(ss, strBuffer))
	{
		strBuffer.erase(strBuffer.find_last_not_of("\n\r") + 1);
		if (strBuffer.empty())
			break;

		std::stringstream ssLine(strBuffer);
		std::string strMesh, strMtl;
		ssLine >> strMesh >> strMtl;

		if (strMtl.size())
		{
			Content.strMaterialFile[Content.dwNumModels] = strMtl;
			InsertUsedResource(strMtl.c_str());
		}
		

		if (strMesh.size())
		{
			if (strMesh[0] == '\\')
			{
				Content.strMeshFile[Content.dwNumModels] = strMesh;
			}
			else
			{
				Content.strMeshFile[Content.dwNumModels] = strFilePath + strMesh;
			}
			InsertUsedResource(Content.strMeshFile[Content.dwNumModels].c_str());
			Content.dwNumModels++;
		}
	}
	nResult = true;
Exit0:
	SAFE_FREE(pBuffer);
	KG_COM_RELEASE(pFile);

	if (!nResult)
	{
		KGLogPrintf(KGLOG_ERR, "Find Res In MDL %s failed.\n", cszResourceName);
	}
	return nResult;
}

int KGMapResSearch::FindResInSFX(const char cszResourceName[])
{
	//参照KG3DSFX::_LoadFromFile
	int nResult = false;
	HRESULT hRetCode = E_FAIL;

	DWORD            dwSize   = 0;
	SFX_FILE_HEADER* pHeader  = NULL;
	DWORD            fileLen  = 0;
	BYTE*            pBuffer  = NULL;
	DWORD            dwCurPos = 0;
	DWORD            i        = 0;
	IFile*           pFile    = NULL;

	KG_ASSERT_EXIT(cszResourceName);
	KGLOG_PROCESS_ERROR(cszResourceName[0] != '\0');

	pFile = g_OpenFile(cszResourceName);
	KGLOG_PROCESS_ERROR(pFile);

	fileLen = pFile->Size();
	ASSERT(fileLen > 0);
	KGLOG_PROCESS_ERROR(fileLen);

	pBuffer = new BYTE[fileLen];
	KGLOG_PROCESS_ERROR(pBuffer);

	dwSize = pFile->Read(pBuffer, fileLen);
	KGLOG_PROCESS_ERROR(dwSize == fileLen);

	pHeader = reinterpret_cast<SFX_FILE_HEADER*>(pBuffer);
	KGLOG_PROCESS_ERROR(pHeader->dwID == SFX_ID);
	KGLOG_PROCESS_ERROR(pHeader->dwFileLength == fileLen);
	KGLOG_PROCESS_ERROR(LOWORD(pHeader->dwVersion) >= 2);


	dwCurPos += sizeof(SFX_FILE_HEADER);
	for (i = 0; i < pHeader->dwElementNum; ++i, dwCurPos += sizeof(SFX_ELEMENT))
	{
		SFX_ELEMENT *pElem = reinterpret_cast<SFX_ELEMENT*>(&pBuffer[dwCurPos]);
		ASSERT(dwCurPos < fileLen);

		KGLOG_PROCESS_ERROR(
			(pElem->dwElementID >= SFX_FEID_GENERAL_INFORMATION) &&
			(pElem->dwElementID < SFX_FEID_SIZE)
			);

		ASSERT(pElem->dwElementOffset <= fileLen);
		/*
		pElem->dwElementID对应于
		KG3DSFX::ProcessBlockFunction KG3DSFX::ms_pfnProcessBlock[SFX_FEID_SIZE]
		中的ReadModelBlock函数指针的索引7，此函数读取mesh,mtl,ani的信息
		*/
		if (pElem->dwElementID == 7)
		{
			//参照KG3DSFX::ReadModelBlock
			SFX_MODEL_BLOCK* pBlock = (SFX_MODEL_BLOCK*)&pBuffer[pElem->dwElementOffset];
			InsertUsedResource((const char*)pBlock->byMeshFileName);
			InsertUsedResource((const char*)pBlock->byMaterialFileName);
			InsertUsedResource((const char*)pBlock->byAnimationFileName);
		}
	}

	nResult = true;
Exit0:
	SAFE_DELETE_ARRAY(pBuffer);
	KG_COM_RELEASE(pFile);
	if (!nResult)
	{
		KGLogPrintf(KGLOG_ERR, "Find Res In SFX %s failed.\n", cszResourceName);
	}
	return nResult;
}

int KGMapResSearch::EnableMultiSearch()
{
	int nResult = false;
	HRESULT hRetCode = E_FAIL;
	::SendMessage(m_hRadioBtnMultiSearch, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
	::SendMessage(m_hRadioBtnSingleSearch, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0); 
	EnableWindow(m_hStaticMultiResText, true);
	EnableWindow(m_hEditMultiResText, true);
	EnableWindow(m_hStaticResName, false);
	EnableWindow(m_hStaticResType, false);
	EnableWindow(m_hStaticResult, false);
	EnableWindow(m_hEditResName, false);
	EnableWindow(m_hEditResType, false);
	EnableWindow(m_hListViewResult, false);

	nResult = true;
Exit0:
	return nResult;
}

int KGMapResSearch::EnableSingleSearch()
{
	int nResult = false;
	HRESULT hRetCode = E_FAIL;

	::SendMessage(m_hRadioBtnSingleSearch, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
	::SendMessage(m_hRadioBtnMultiSearch, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0); 
	EnableWindow(m_hStaticMultiResText, false);
	EnableWindow(m_hEditMultiResText, false);
	EnableWindow(m_hStaticResName, true);
	EnableWindow(m_hStaticResType, true);
	EnableWindow(m_hStaticResult, true);
	EnableWindow(m_hEditResName, true);
	EnableWindow(m_hEditResType, true);
	EnableWindow(m_hListViewResult, true);

	nResult = true;
Exit0:
	return nResult;
}

int KGMapResSearch::ReadResList(const char cszListFileName[])
{
	int nResult  = false;
	int nRetCode = false;
	int nExtLength = 0;
	IFile* pIFile		 = NULL;
	char*  pszFileBuffer = NULL;
	char*  pszFileName   = NULL;
	char szSep[] = "\n";
	char szListFileName[MAX_PATH];
	char szResFullName[MAX_PATH];
	char szResName[MAX_PATH];
	char szResExt[MAX_PATH];
	char szSearchName[MAX_PATH];

	m_vecSearchResNameList.clear();
	nRetCode = _snprintf_s(
		szListFileName,
		sizeof(szListFileName),
		sizeof(szListFileName) - 1,
		"%s%s",
		g_szApplicationPath,
		cszListFileName);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	pIFile = g_OpenFile(szListFileName);
	KGLOG_PROCESS_ERROR(pIFile);

	pszFileBuffer = (char*)pIFile->GetBuffer();
	KGLOG_PROCESS_ERROR(pszFileBuffer);

	pszFileName = strtok(pszFileBuffer, "\n ");   
	while(pszFileName != NULL)   
	{
		int i = 1;
		nRetCode = ::_splitpath_s(pszFileName, NULL, 0, NULL, 0, szResName, sizeof(szResName), szResExt, sizeof(szResExt));
		KGLOG_PROCESS_ERROR(nRetCode == 0);
		if (szResExt[0] != '\0')
		{
			nRetCode = _strlwr_s(szResExt, sizeof(szResExt));
			KGLOG_PROCESS_ERROR(nRetCode == 0);
			while(szResExt[i] != '\0')
			{
				char a = szResExt[i];
				if (szResExt[i] < 'a' || szResExt[i] > 'z')
				{
					szResExt[i] = '\0';
					break;
				}
				i++;
			}
			nRetCode = _snprintf_s(szSearchName, sizeof(szSearchName), sizeof(szSearchName) - 1, "%s%s", szResName, szResExt);
			KGLOG_PROCESS_ERROR(nRetCode> 0);

			m_vecSearchResNameList.push_back(szSearchName);
			m_ResTypeList.insert(szResExt);	
		}
		pszFileName = strtok(NULL, szSep);   
	}

	nRetCode = AddRelationType();
	KGLOG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	SAFE_RELEASE(pIFile);
	return nResult;
}

bool KGMapResSearch::MultiSearchAnalyseInput()
{
	bool bResult  = false;
	int nRetCode = false;
	char szSearchListFile[MAX_PATH];

	//判断输入的搜索资源列表文件名称不为空
	nRetCode = ::GetDlgItemText(m_hDlg, IDC_EDIT_MULTIRESSEARCH, szSearchListFile, sizeof(szSearchListFile));
	if (szSearchListFile[0] == '\0')
	{
		::MessageBox(m_hDlg, "请输入正确的搜索资源列表文件名称", "提示", MB_ICONINFORMATION);
		KG_PROCESS_ERROR(false);
	}

	nRetCode = ReadResList(szSearchListFile);
	KGLOG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	return bResult;
}

int KGMapResSearch::OutputMultiSearchResult()
{
	int nRetCode = false;
	int nResult  = false;
	IFile* pIFile = NULL;
	ResInfo* pResInfo = NULL;
	char szOutputFile[MAX_PATH];
	char szWriteText[MAX_PATH * 2];
	char szInfo[MAX_PATH * 2];
	struct tm tmNow;
	time_t tmtNow = 0;

	tmtNow = time(NULL);
	localtime_r(&tmtNow, &tmNow);
	nRetCode = _snprintf_s(
		szOutputFile,
		sizeof(szOutputFile),
		sizeof(szOutputFile) - 1,
		"%sMultiResSearchResult_%d_%2.2d_%2.2d_%2.2d_%2.2d_%2.2d.txt", 
		g_szApplicationPath,
		tmNow.tm_year + 1900,
		tmNow.tm_mon + 1,
		tmNow.tm_mday,
		tmNow.tm_hour,
		tmNow.tm_min,
		tmNow.tm_sec
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	
	pIFile = g_CreateFile(szOutputFile);
	KGLOG_PROCESS_ERROR(pIFile);

	for(size_t i = 0; i < m_vecSearchResNameList.size(); i++)
	{
		nRetCode = SearchResInList(m_vecSearchResNameList[i].c_str(), &pResInfo);
		KGLOG_PROCESS_ERROR(nRetCode);
		if (pResInfo)
		{
			nRetCode = _snprintf_s(
				szWriteText,
				sizeof(szWriteText),
				sizeof(szWriteText) - 1,
				"\r\n【Resource Name】%s\r\n"
				"【Used Maps】\r\n", 
				m_vecSearchResNameList[i].c_str()
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			pIFile->Write(szWriteText, strlen(szWriteText));

			for(size_t j = 0; j < pResInfo->MapNameList.size(); j++)
			{
				nRetCode = _snprintf_s(
					szWriteText,
					sizeof(szWriteText),
					sizeof(szWriteText) - 1,
					"\t%s\t(%.2f, %.2f, %.2f)\r\n", 
					pResInfo->MapNameList[j].c_str(),
					pResInfo->PositionList[j].x,
					pResInfo->PositionList[j].y,
					pResInfo->PositionList[j].z
					);
				KGLOG_PROCESS_ERROR(nRetCode > 0);
				pIFile->Write(szWriteText, strlen(szWriteText));
			}
		}
	}
	nRetCode = _snprintf_s(
		szInfo,
		sizeof(szInfo),
		sizeof(szInfo) - 1,
		"查找结果已经输出到文件：\n%s", 
		szOutputFile
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	::MessageBox(m_hDlg, szInfo, "查找完毕", MB_ICONINFORMATION);			

	nResult = true;
Exit0:
	SAFE_RELEASE(pIFile);
	return nResult;
}