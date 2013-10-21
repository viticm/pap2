#include "KGTestMapDisuseResource.h"
#include "IEKG3DTerrain.h"
#include "IEEditor.h"
#include "KG3DSceneDataInterface.h"
#include "KG3DMemoryFile.h"
#include "KG3DModelTable.h"
#include "KG3DBBox.h"

//参照KG3DRepresentObjectPVS
#define PVS_FILE_VERSION_1      100 
#define PVS_FILE_VERSION        200 
#define PVS_PORTAL_NAME_MAX_LEN 32
#define PVS_SUOBJ_MAX_MDL_NUM   32


const int EM_BUFFER_SIZE = 1024 * 1024 * 4;			//参照KG3DMemoryFile
const char RESULT_DISUSE_RES[] = "Result_DisuseResource.txt";
const char RESULT_LOST_RES[]   = "Result_LostResource.txt";

//TestSetting.ini
const char TESTSETTING_FILE[]		 = "TestSetting.ini";
const char SECTION_SETTINGS[]		 = "Settings";
const char KEY_CLIENTPATH[]			 = "ClientPath";
const char KEY_TESTMAP_PREFIX[]		 = "TestMap_";
const char KEY_RESOURCETYPE[]		 = "ResourceType";
const char KEY_RESOURCEPATH_PREFIX[] = "SourceResPath_";

KGTestMapDisuseResource::KGTestMapDisuseResource()
{
	m_hEngineDLL = NULL;
	m_pEngineMgr = NULL;
	m_szClientPath[0] = '\0';
	m_mapSourceRes.clear();
	m_setUsedRes.clear();
	m_vecMapList.clear();
	m_vecLODRes.clear();
	m_bFindDisuseMapRes = false;
}

KGTestMapDisuseResource::~KGTestMapDisuseResource()
{
	UnInit();
}

int KGTestMapDisuseResource::Init()
{
	int nRetCode = false;
	int nResult  = false;

	printf("Init Testing...\n");
	KGLogPrintf(KGLOG_INFO, "Init Testing");

	//读取测试设置
	nRetCode = LoadTestSetting();
	KGLOG_PROCESS_ERROR(nRetCode);

	if (m_szClientPath[0] == '\0')
	{
		nRetCode = strncpy_s(m_szClientPath, sizeof(m_szClientPath), g_szApplicationPath, strlen(g_szApplicationPath));
		KGLOG_PROCESS_ERROR(nRetCode == 0);
	}

	nRetCode = KGTestCommon::FormatPath(m_szClientPath, sizeof(m_szClientPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	//初始化资源列表
	if (m_bFindDisuseMapRes)
	{
		KGTestCommon::OutputInfo("Finding Source Resource");
		nRetCode = FindSourceResource();
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	//加载引擎动态库
 	nRetCode = LoadEngineDLL();
 	KGLOG_PROCESS_ERROR(nRetCode);


	nResult = true;
Exit0:
	if (!nResult)
	{
		UnInit();
	}
	return nResult;
}

void KGTestMapDisuseResource::UnInit()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;

	m_mapSourceRes.clear();
	m_setUsedRes.clear();
	m_vecMapList.clear();
	m_vecLODRes.clear();
	if (m_pEngineMgr)
	{
		hrRetCode = m_pEngineMgr->UnInit();
		KGLOG_COM_CHECK_ERROR(hrRetCode);
		m_pEngineMgr = NULL;
	}
	if (m_hEngineDLL)
	{
		nRetCode = FreeLibrary(m_hEngineDLL);
		KGLOG_CHECK_ERROR(nRetCode);
		m_hEngineDLL = NULL;
	}
}

int KGTestMapDisuseResource::RunTest()
{
	int nResult  = false;
	int nRetCode = false;
	char szMapName[MAX_PATH];

	KGTestCommon::OutputInfo("Analyse Maps Used Resource");
	if (m_bFindDisuseMapRes)
	{
		nRetCode = FindUsedMapResource();
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = CheckDisuseResource();
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		for (size_t i = 0; i < m_vecMapList.size(); i++)
		{
			nRetCode = _snprintf_s(
				szMapName,
				sizeof(szMapName),
				sizeof(szMapName) - 1,        
				"%sdata\\source\\maps\\%s\\%s.map",
				m_szClientPath,
				m_vecMapList[i].c_str(),
				m_vecMapList[i].c_str()
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			nRetCode = AnalyseOneMapResourceInfo(szMapName);
			KGLOG_PROCESS_ERROR(nRetCode);			
		}
		nRetCode = CheckLostResource();
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGTestMapDisuseResource::LoadEngineDLL()
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	HWND hWindow = NULL;
	char szEngineDll[MAX_PATH] = {0};

	g_SetRootPath(m_szClientPath);
	::SetCurrentDirectory(m_szClientPath);

	nRetCode = _snprintf_s(
		szEngineDll,
		sizeof(szEngineDll),
		sizeof(szEngineDll) - 1,        
		"%s%s",
		g_szApplicationPath,
		ENGINE_DLL_NAME
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

 	nRetCode = _access(szEngineDll, 0);
 	if (nRetCode != 0)
	{
		nRetCode = _snprintf_s(
			szEngineDll,
			sizeof(szEngineDll),
			sizeof(szEngineDll) - 1,        
			"%s%s",
			m_szClientPath,
			ENGINE_DLL_NAME
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);
	}
	
	m_hEngineDLL = LoadLibrary(szEngineDll);
	KGLOG_PROCESS_ERROR(m_hEngineDLL);

	hWindow = ::GetDesktopWindow();
	KGLOG_PROCESS_ERROR(hWindow);

	typedef HRESULT (*pfnGet3DEngineInterface)(void** ppvEngineManager);
	pfnGet3DEngineInterface Get3DEngineInterface;

	Get3DEngineInterface = (pfnGet3DEngineInterface)GetProcAddress(m_hEngineDLL, "Get3DEngineInterface");
	Get3DEngineInterface((void**)&m_pEngineMgr);
	KGLOG_PROCESS_ERROR(m_pEngineMgr);
	hrRetCode = m_pEngineMgr->Init(0, KG3DENG_CLIENT, hWindow, hWindow);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	nResult = true;
Exit0:
	if (!nResult)
	{
		if (m_pEngineMgr)
		{
			hrRetCode = m_pEngineMgr->UnInit();
			KGLOG_COM_CHECK_ERROR(hrRetCode);
			m_pEngineMgr = NULL;
		}
		if (m_hEngineDLL)
		{
			nRetCode = FreeLibrary(m_hEngineDLL);
			KGLOG_CHECK_ERROR(hrRetCode);
			m_hEngineDLL = NULL;
		}
	}
	return nResult;
}

int KGTestMapDisuseResource::AnalyseOneMapResourceInfo(const char cszMapName[])
{
	int nResult  = false;
	int nRetCode = false;
	HRESULT hrRetCode = E_FAIL;
	IEKG3DSceneSceneEditor*		pIEEditor     = NULL;
	IEKG3DSceneOutDoorSpaceMgr* pIEOutDoorMgr = NULL;
	IEKG3DScene*				pScene		  = NULL;
	char szInfo[MAX_PATH];

	KG_ASSERT_EXIT(cszMapName);
	KGLOG_PROCESS_ERROR(cszMapName[0] != '\0');

	nRetCode = _snprintf_s(szInfo,
		sizeof(szInfo),
		sizeof(szInfo) - 1,
		"Analyse Maps Name: %s",
		cszMapName
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	KGTestCommon::OutputInfo(szInfo);
	
#if defined(DEBUG) | defined(_DEBUG)
	KG3D_GAME_INIT_CONTEXT Context;
 	m_pEngineMgr->InitGameContext(&Context);
	//KGLOG_COM_PROCESS_ERROR(hrRetCode);
#endif
 	hrRetCode = m_pEngineMgr->NewOneScene(SCENETYPE_SCENEEDITOR, (IKG3DScene**)&pScene);
 	KGLOG_COM_PROCESS_ERROR(hrRetCode);
 
 	hrRetCode = pScene->GetSceneSceneEditor(&pIEEditor);
 	KGLOG_COM_PROCESS_ERROR(hrRetCode);
 
 	pIEOutDoorMgr = pIEEditor->GetIEKG3DSceneOutDoorSpaceMgr();
 	KGLOG_PROCESS_ERROR(pIEOutDoorMgr);

 	hrRetCode = pIEOutDoorMgr->LoadFromFile(cszMapName, 0, 0);
 	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	//nRetCode = AnalyseTerrainInfo(pIEOutDoorMgr);
	//KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = AnalyseEntityInfo(pIEOutDoorMgr);
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

int KGTestMapDisuseResource::LoadSourceResourceInfo(const char cszPath[])
{
	int nRetCode = false;
	int nResult	 = false;				
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	char szFileName[MAX_PATH]	  = {0};
	char szAllFile[MAX_PATH]	  = {0};
	char szFullPathFile[MAX_PATH] = {0};
	char szPath[MAX_PATH]		  = {0};
	char szExt[MAX_PATH]		  = {0};
	WIN32_FIND_DATA FindFileData;

	KG_ASSERT_EXIT(cszPath);
	KGLOG_PROCESS_ERROR(cszPath[0] != '\0');

	nRetCode = strncpy_s(szPath, sizeof(szPath), cszPath, strlen(cszPath));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	nRetCode = KGTestCommon::FormatPath(szPath, sizeof(szPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _snprintf_s(szAllFile,
		sizeof(szAllFile),
		sizeof(szAllFile) - 1,
		"%s%s", szPath,
		"*.*");
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	hFindFile = ::FindFirstFile(szAllFile, &FindFileData);
	KGLOG_PROCESS_ERROR(hFindFile != INVALID_HANDLE_VALUE);
	while (true)
	{
		nRetCode = strncpy_s(szFileName,
			sizeof(szFileName),
			FindFileData.cFileName,
			strlen(FindFileData.cFileName)
			);
		KGLOG_PROCESS_ERROR(nRetCode == 0);
		//过滤"."、".."、".svn"
		if (szFileName[0] != '.')
		{
			nRetCode = _snprintf_s(szFullPathFile,
				sizeof(szFullPathFile),
				sizeof(szFullPathFile) - 1,
				"%s%s",
				szPath,
				szFileName
				);
			KGLOG_PROCESS_ERROR(nRetCode > 0);
			if (FindFileData.dwFileAttributes == FILE_ATTRIBUTE_DIRECTORY)
			{
				nRetCode = LoadSourceResourceInfo(szFullPathFile);
				KGLOG_PROCESS_ERROR(nRetCode);
			}
			else
			{
				nRetCode = _splitpath_s(szFileName, NULL, 0, NULL, 0, NULL, 0, szExt, sizeof(szExt));
				KGLOG_PROCESS_ERROR(nRetCode == 0);
				nRetCode = _strlwr_s(szExt, sizeof(szExt));
				KGLOG_PROCESS_ERROR(nRetCode == 0);
				if (m_mapSourceRes.find(szExt) != m_mapSourceRes.end())
				{
					if (strncmp(szExt, ".mesh", sizeof(".mesh")) == 0 ||
						strncmp(szExt, ".mtl", sizeof(".mtl")) == 0)
					{	
						string strName = szFileName;
						nRetCode = (int)strName.find("_L.");
						if (nRetCode != -1)
						{
							m_vecLODRes.push_back(szFullPathFile);
						}
						else
						{
							m_mapSourceRes[szExt].push_back(szFullPathFile);
						}
					}
					else
					{
						m_mapSourceRes[szExt].push_back(szFullPathFile);
					}
					
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
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile);
		hFindFile = INVALID_HANDLE_VALUE;
	}
	return nResult;
}
/*
int KGTestMapDisuseResource::AnalyseTerrainInfo(IEKG3DSceneOutDoorSpaceMgr* pIEOutDoorMgr)
{
	int nResult  = false;
	int nRetCode = false;
 	HRESULT hrRetCode = E_FAIL;
 	DWORD dwType	  = 0;
 	DWORD dwLength	  = 0;
 	char szResourceName[MAX_PATH];
 	KG3DMemoryFile* pMemoryFile = NULL;
 	vector<UINT> vecTerrainHandle;

 	KG_ASSERT_EXIT(pIEOutDoorMgr);
 
	pMemoryFile = new KG3DMemoryFile();
	KGLOG_PROCESS_ERROR(pMemoryFile);
 	hrRetCode = pIEOutDoorMgr->GetAllTerrainInfoHandle(&vecTerrainHandle);
 	KGLOG_COM_PROCESS_ERROR(hrRetCode);
 
 	for (UINT i = 0; i < vecTerrainHandle.size(); i++)
 	{
 		hrRetCode = pIEOutDoorMgr->GetTerrainInfoformation(vecTerrainHandle[i], &dwType, &dwLength, pMemoryFile->GetBuffer());
		if (FAILED(hrRetCode))
		{
			KGLogPrintf(KGLOG_ERR, "It must be something wrong with %d.TerrainInfo", vecTerrainHandle[i]);
			continue;
		}	
 
 		switch (dwType)
 		{
 		case Terrain_Info_Convermap:
 			{
 				pMemoryFile->read(szResourceName, sizeof(char) * MAX_PATH);
 				szResourceName[sizeof(szResourceName) - 1] = '\0';
				if (szResourceName[0] != '\0')
				{
 					FindResource(szResourceName);
				}
 
 				pMemoryFile->read(szResourceName, sizeof(char) * MAX_PATH);
 				szResourceName[sizeof(szResourceName) - 1] = '\0';
 				if (szResourceName[0] != '\0')
 				{
 					FindResource(szResourceName);
				}
 			}
			break;
 		case Terrain_Info_DetailMtlMgr:
 			{
 				DWORD dwVersion = 0;
 				BYTE byteNum				  = 0;
 				BYTE byteIndex				  = 0;
 				BYTE byteCurrentMaterialIndex = 0;
 
  				nRetCode = pMemoryFile->read(&dwVersion, sizeof(DWORD));
 				nRetCode = pMemoryFile->read(&byteNum, sizeof(BYTE));
 				nRetCode = pMemoryFile->read(&byteCurrentMaterialIndex, sizeof(BYTE));
  
  				for (BYTE i = 0;i < byteNum; i++)
  				{
  					BYTE byteMask = 0;
					size_t uPos = 0;

  					pMemoryFile->read(&byteMask,sizeof(BYTE));
  
  					if(byteMask)
  					{
  						DWORD dwVersion = 0;
  						pMemoryFile->read(&dwVersion,sizeof(DWORD));
  						pMemoryFile->read(&byteIndex,sizeof(BYTE));
 						//这里如果g_cEngineOption.bEnableGroundNormalMap = true时，
 						//需调用g_cEngineManager.GetHDFilePath();获得NormalMap资源名称
						//默认g_cEngineOption.bEnableGroundNormalMap = false
  						pMemoryFile->read(&szResourceName,sizeof(char) * MAX_PATH);
 						szResourceName[sizeof(szResourceName) - 1] = '\0';
 						FindResource(szResourceName);

						uPos = pMemoryFile->GetPos();
						uPos += sizeof(BYTE) +			//m_byTexCoordIndex
								sizeof(float) +			//m_fScaleU
								sizeof(float) +			//m_fScaleV
								sizeof(D3DXVECTOR4) +	//m_vUT
								sizeof(D3DXVECTOR4);	//m_vVT

						if(dwVersion >= 1)
						{
							uPos +=	sizeof(BOOL) +	//m_bAssociateGrass
									8 +				//m_byGrassTexIndex
									8; 				//m_byPatternIndex
						}
						if(dwVersion == 2)
						{
							uPos +=	sizeof(float) +		//m_fEnvReflection
									sizeof(float) +		//m_fSpecular
									sizeof(float);		//m_fEmissive
						}
						pMemoryFile->SetPos(uPos);
  					}
  				}
 			}
 			break;
 		case Terrain_Info_GrassSet:
 		case Terrain_Info_StoneSet:
 		case Terrain_Info_FrutexSet:
 			{
 				DWORD dwVersion  = 0;
 				DWORD dwStrideX  = 0;
 				DWORD dwStrideY  = 0;
 				DWORD NumPattern = 0;
 
 				pMemoryFile->read(&dwVersion,sizeof(DWORD));
 				pMemoryFile->read(&dwStrideX,sizeof(DWORD));
 				pMemoryFile->read(&dwStrideY,sizeof(DWORD));
 				pMemoryFile->read(szResourceName, sizeof(char) * MAX_PATH);
 				szResourceName[sizeof(szResourceName) - 1] = '\0';
 				FindResource(szResourceName);
 
 				pMemoryFile->read(&NumPattern,sizeof(DWORD));
 				for (DWORD i = 0; i < NumPattern; i++)
 				{
 					pMemoryFile->read(szResourceName, sizeof(char) * MAX_PATH);
 					szResourceName[sizeof(szResourceName) - 1] = '\0';
 					FindResource(szResourceName);
 				}
 			}
 			break;
 		case Terrain_Info_RoadMgr:
 		case Terrain_Info_RiverMgr:
 			{
 				UINT uHandle = 0;
 				DWORD dwVersion = 0;
 				DWORD dwNum		= 0;
 
 				pMemoryFile->read(&dwVersion, sizeof(DWORD));
 				pMemoryFile->read(&dwNum, sizeof(DWORD));
 				for (DWORD i = 0 ; i < dwNum; i++)
 				{
 					pMemoryFile->read(&uHandle,sizeof(UINT));
 					pMemoryFile->read(&szResourceName, sizeof(char) * 32);
 					szResourceName[sizeof(szResourceName) - 1] = '\0';
 					FindResource(szResourceName);
 				}
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
	}
	return nResult;
}
*/
int KGTestMapDisuseResource::AnalyseEntityInfo(IEKG3DSceneOutDoorSpaceMgr* pIEOutDoorMgr)
{
	int nResult  = false;
	int nRetCode = false;
	size_t uPos	 = 0;
	HRESULT hrRetCode = E_FAIL;
	vector<UINT> vecEntityHandle;
	KG3DMemoryFile* pMemoryFile = NULL;

	KG_ASSERT_EXIT(pIEOutDoorMgr);

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
		hrRetCode = pIEOutDoorMgr->GetEntityInfoformation(vecEntityHandle[i],
														  &dwType,
														  &dwLength,
														  pMemoryFile->GetBuffer(),
														  EM_BUFFER_SIZE);
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

					pMemoryFile->read(szMeshName, MAX_PATH);//Mesh
					_ASSERTE(szMeshName[0] != '\0');
					FindResource(szMeshName, m_setUsedRes);
					pMemoryFile->read(szMtlName, MAX_PATH);//Mtl
					FindResource(szMtlName, m_setUsedRes);
					pMemoryFile->read(szAniName, MAX_PATH);//AniName
					FindResource(szAniName, m_setUsedRes);
					
					uPos = pMemoryFile->GetPos();
					uPos += sizeof(D3DXVECTOR3) +		//m_vScaling
							sizeof(D3DXVECTOR3) +		//m_vScalingCenter
							sizeof(D3DXQUATERNION) +	//m_vScalingRotation
							sizeof(D3DXQUATERNION) +	//m_vRotation
							sizeof(D3DXVECTOR3) +		//m_vRotaionCenter
							sizeof(D3DXVECTOR3);		//m_vTranslation
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
						//release版本GetTypeInfoByFileName会宕机////////////////////////
						hrRetCode = pModelTable->GetTypeInfoByFileName(&pTypeInfo, szMeshName);
						/////////////////////////////////////////////////////
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
						FindResource(szSourceName, m_setUsedRes);
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
				FindResource(szSourceName, m_setUsedRes);
			}
			break;
		case REPRESENTOBJECT_SOUNDBALL:
			{
				//参照KG3DRepresentObjectSoundBall::LoadFromInformationBuffer
				char szSoundName[MAX_PATH] = {0};
				DWORD dwVersion = 0;

				pMemoryFile->read(&dwVersion, sizeof(DWORD));
				pMemoryFile->read(szSoundName, sizeof(char) * MAX_PATH);
				FindResource(szSoundName, m_setUsedRes);
			}
			break;
		case REPRESENTOBJECT_PVS:
			{
				//参照KG3DRepresentObjectPVS::LoadFromInformationBuffer
				char szName[MAX_PATH] = {0};
				pMemoryFile->read(szName, sizeof(char) * MAX_PATH);
				FindResource(szName, m_setUsedRes);
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
	}
	return nResult;
}

int KGTestMapDisuseResource::CheckDisuseResource()
{
	int nResult  = false;
	int nRetCode = false;
	set<string>::iterator iterUsed;
	string strUsedResName;
	string strSourceResName;
	char szExt[MAX_PATH] = {0};

	printf("Checking Disuse Resource...\n");
	KGLogPrintf(KGLOG_INFO, "Checking Disuse Resource");
	for (iterUsed = m_setUsedRes.begin(); iterUsed != m_setUsedRes.end(); ++iterUsed)
	{
 		strUsedResName = *iterUsed;
 		nRetCode = _mbslwr_s((unsigned char*)strUsedResName.c_str(), strUsedResName.length() + 1);
 		KGLOG_PROCESS_ERROR(nRetCode == 0);

		nRetCode = _splitpath_s(strUsedResName.c_str(), NULL, 0, NULL , 0, NULL, 0, szExt, sizeof(szExt));
		KGLOG_PROCESS_ERROR(nRetCode == 0);
		for (UINT i = 0; i < m_mapSourceRes[szExt].size(); i++)
		{
			strSourceResName = m_mapSourceRes[szExt][i];
			if (strSourceResName.empty())
			{
				continue;
			}
			nRetCode = _mbslwr_s((unsigned char*)strSourceResName.c_str(), strSourceResName.length() + 1);
			KGLOG_PROCESS_ERROR(nRetCode == 0);

			nRetCode = (int)strSourceResName.find(strUsedResName);
			if (nRetCode != -1)
			{
				if (strncmp(szExt, ".mesh", sizeof(".mesh")) == 0 ||
					strncmp(szExt, ".mtl", sizeof(".mtl")) == 0)
				{				
					char szLODRes[MAX_PATH] = {0};
					char szDrive[MAX_PATH]	= {0};
					char szDir[MAX_PATH]	= {0};
					char szName[MAX_PATH]	= {0};
					char szExt[MAX_PATH]	= {0};
					string strSourceLODRes;

					nRetCode = _splitpath_s(
						strUsedResName.c_str(),
						szDrive, sizeof(szDrive),
						szDir, sizeof(szDir),
						szName, sizeof(szName),
						szExt, sizeof(szExt));
					KGLOG_PROCESS_ERROR(nRetCode == 0);

					nRetCode = _snprintf_s(
						szLODRes,
						sizeof(szLODRes),
						sizeof(szLODRes) - 1,
						"%s%s%s_l%s",
						szDrive, szDir, szName, szExt);
					KGLOG_PROCESS_ERROR(nRetCode > 0);

					for (size_t i = 0; i < m_vecLODRes.size(); i++)
					{
						strSourceLODRes = m_vecLODRes[i];
						nRetCode = _mbslwr_s((unsigned char*)strSourceLODRes.c_str(), strSourceLODRes.length() + 1);
						KGLOG_PROCESS_ERROR(nRetCode == 0);
						nRetCode = (int)strSourceLODRes.find(szLODRes);
						if (nRetCode != -1)
						{
							m_vecLODRes[i].clear();
							break;
						}
					}
				}

				(*iterUsed).clear();
				m_mapSourceRes[szExt][i].clear();
				break;
			}
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGTestMapDisuseResource::FindUsedMapResource()
{
	int nRetCode = false;
	int nResult	 = false;				
	HANDLE hFindFile = INVALID_HANDLE_VALUE;
	char szAllFile[MAX_PATH];
	char szMapName[MAX_PATH];
	WIN32_FIND_DATA FindFileData;

	nRetCode = _snprintf_s(
		szAllFile,
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
				nRetCode = _access(szMapName, 0);
				if (nRetCode == 0)
				{
					nRetCode = AnalyseOneMapResourceInfo(szMapName);
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
	if (hFindFile != INVALID_HANDLE_VALUE)
	{
		::FindClose(hFindFile);
		hFindFile = INVALID_HANDLE_VALUE;
	}
	return nResult;
}

int KGTestMapDisuseResource::OutputResult()
{
	int nRetCode = false;
	int nResult	 = false;
	char szResultPath[MAX_PATH];
	char szInfo[MAX_PATH * 2];

	nRetCode = KGTestCommon::MakeResultPath(TEST_PROJECT_NAME, szResultPath, sizeof(szResultPath));
	KGLOG_PROCESS_ERROR(nRetCode);

	if (m_bFindDisuseMapRes)
	{
		nRetCode = OutputDisuseRes(szResultPath);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	nRetCode = OutputLostRes(szResultPath);
	KGLOG_PROCESS_ERROR(nRetCode);

	nRetCode = _snprintf_s(szInfo,
		sizeof(szInfo),
		sizeof(szInfo) - 1,
		"检查结果已保存到以下目录：\n%s",
		szResultPath
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	::MessageBox(NULL, szInfo, "检查完成", MB_ICONINFORMATION);		

	nResult = true;
Exit0:
	return nResult;
}

int KGTestMapDisuseResource::FindResource(const char cszResourceName[] , set<string>& setResList)
{
	int nRetCode = false;
	int nResult  = false;
	char szName[MAX_PATH];
	char szExt[MAX_PATH];
	std::pair<std::set<string>::iterator,bool> retInsert;

	KG_ASSERT_EXIT(cszResourceName);
	KG_PROCESS_ERROR(cszResourceName[0] != '\0');

	nRetCode = strncpy_s(szName, sizeof(szName), cszResourceName, strlen(cszResourceName));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	nRetCode = _mbslwr_s((unsigned char*)szName, sizeof(szName));//筞
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	retInsert = setResList.insert(szName);
	KG_PROCESS_SUCCESS(!retInsert.second);		//插入失败表示有重复，不做接下来的解析

	nRetCode = _splitpath_s(szName, NULL, 0, NULL, 0, NULL, 0, szExt, sizeof(szExt));
	KGLOG_PROCESS_ERROR(nRetCode == 0);
	if (strncmp(szExt, ".modelset", sizeof(".modelset")) == 0)
	{
		nRetCode = FindResInModelset(szName, setResList);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	if (strncmp(szExt, ".pvs", sizeof(".pvs")) == 0)
	{
		nRetCode = FindResInPVS(szName, setResList);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

	if (strncmp(szExt, ".stree", sizeof(".stree")) == 0)
	{
		nRetCode = FindResInSTree(szName, setResList);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	if (strncmp(szExt, ".mdl", sizeof(".mdl")) == 0)
	{
		nRetCode = FindResInMDL(cszResourceName, setResList);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	if (strncmp(szExt, ".sfx", sizeof(".sfx")) == 0)
	{
		nRetCode = FindResInSFX(cszResourceName, setResList);
		KGLOG_PROCESS_ERROR(nRetCode);
	}

Exit1:
	nResult = true;
Exit0:
	return nResult;
}

int KGTestMapDisuseResource::FindResInModelset(const char cszResourceName[], set<string>& vecResList)
{
	//参照KG3DRepresentObjectSet::LoadFromIniFile
	int nRetCode = false;
	int nResult  = false;
	IIniFile* pIniFile = NULL;
	char szSourcePathName[MAX_PATH] = {0};

	KG_ASSERT_EXIT(cszResourceName);
	KGLOG_PROCESS_ERROR(cszResourceName[0] != '\0');

	nRetCode = _snprintf_s(szSourcePathName,
		sizeof(szSourcePathName),
		sizeof(szSourcePathName) - 1,
		"%s%s",
		m_szClientPath,
		cszResourceName);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	pIniFile = g_OpenIniFile(szSourcePathName);
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
		FindResource(szMeshName, vecResList);
		pIniFile->GetString(szSecName, "MtlName", "", szMtlName, sizeof(szMtlName));
		FindResource(szMtlName, vecResList);
	}

	nResult = true;
Exit0:
	if (pIniFile)
	{	
		pIniFile->Release();
		pIniFile = NULL;
	}
	return nResult;
}

int KGTestMapDisuseResource::FindResInPVS(const char cszResourceName[], set<string>& setResList)
{
	//参照KG3DRepresentObjectPVS::LoadPvsFile
	int nRetCode = false;
	int nResult  = false;
	HRESULT hrRetCode = E_FAIL;
	IFile* pFile = NULL;
	DWORD dwVersion = PVS_FILE_VERSION;
	char szSourcePathName[MAX_PATH] = {0};
	char szResName[MAX_PATH]		= {0};
	long lOffset  = 0;
	size_t objNum = 0;

	KG_ASSERT_EXIT(cszResourceName);
	KGLOG_PROCESS_ERROR(cszResourceName[0] != '\0');

	nRetCode = _snprintf_s(szSourcePathName,
		sizeof(szSourcePathName),
		sizeof(szSourcePathName) - 1,
		"%s%s",
		m_szClientPath,
		cszResourceName);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	pFile = g_OpenFile(cszResourceName, false, false);
	KGLOG_PROCESS_ERROR(pFile);

	/* read version */
	pFile->Read(&dwVersion, sizeof(dwVersion));

	/* load border, the frist model of the pvs object is it's ouside border
	and the second model is it's inside border */
	pFile->Read(szResName, sizeof(szResName));
	FindResource(szResName, setResList);

	pFile->Read(szResName, sizeof(szResName));
	FindResource(szResName, setResList);

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
				FindResource(szResName, setResList);
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
					FindResource(szResName, setResList);
					if (dwVersion > PVS_FILE_VERSION_1)
					{
						pFile->Read(szAnins, sizeof(szAnins));
						FindResource(szAnins, setResList);
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
	return nResult;
}

int KGTestMapDisuseResource::FindResInSTree(const char cszResourceName[], set<string>& setResList)
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

	nRetCode = _snprintf_s(szSourcePathName,
		sizeof(szSourcePathName),
		sizeof(szSourcePathName) - 1,
		"%s%s",
		m_szClientPath,
		cszResourceName);
	KGLOG_PROCESS_ERROR(nRetCode > 0);
	pFile = g_OpenFile(szSourcePathName);
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
	FindResource(szTreeMeshName, setResList);

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
	FindResource(szTreeMtlName, setResList);
	
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
	FindResource(szTreeMtlName, setResList);

	nResult = true;
Exit0:
	if (pFile)
	{
		pFile->Close();
		SAFE_RELEASE(pFile);
	}
	return nResult;
}

int KGTestMapDisuseResource::FindResInMDL(const char cszResourceName[], set<string>& setResList)
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
	FindResource(Content.strBipFile.c_str(), setResList);

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
		}
		FindResource(Content.strMaterialFile[Content.dwNumModels].c_str(), setResList);

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
			FindResource(Content.strMeshFile[Content.dwNumModels].c_str(), setResList);
			Content.dwNumModels++;
		}
	}
	nResult = true;
Exit0:
	SAFE_FREE(pBuffer);
	KG_COM_RELEASE(pFile);

	if (!nResult && cszResourceName)
	{
		KGLogPrintf(KGLOG_ERR, "Find Res In MDL %s failed.\n", cszResourceName);
	}
	return nResult;
}

int KGTestMapDisuseResource::FindResInSFX(const char cszResourceName[], set<string>& setResList)
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
	KG_PROCESS_ERROR(pFile);

	fileLen = pFile->Size();
	ASSERT(fileLen > 0);
	KG_PROCESS_ERROR(fileLen);

	pBuffer = new BYTE[fileLen];
	KG_PROCESS_ERROR(pBuffer);

	dwSize = pFile->Read(pBuffer, fileLen);
	KG_PROCESS_ERROR(dwSize == fileLen);

	pHeader = reinterpret_cast<SFX_FILE_HEADER*>(pBuffer);
	KG_PROCESS_ERROR(pHeader->dwID == SFX_ID);
	KG_PROCESS_ERROR(pHeader->dwFileLength == fileLen);
	KG_PROCESS_ERROR(LOWORD(pHeader->dwVersion) >= 2);


	dwCurPos += sizeof(SFX_FILE_HEADER);
	for (i = 0; i < pHeader->dwElementNum; ++i, dwCurPos += sizeof(SFX_ELEMENT))
	{
		SFX_ELEMENT *pElem = reinterpret_cast<SFX_ELEMENT*>(&pBuffer[dwCurPos]);
		ASSERT(dwCurPos < fileLen);

		KG_PROCESS_ERROR(
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
			FindResource((const char*)pBlock->byMeshFileName, setResList);
			FindResource((const char*)pBlock->byMaterialFileName, setResList);
			FindResource((const char*)pBlock->byAnimationFileName, setResList);
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGTestMapDisuseResource::LoadTestSetting()
{
	int nResult  = false; 
	int nRetCode = false;
	char szSettingFile[MAX_PATH];
	char szResType[MAX_PATH];

	nRetCode = _snprintf_s(
		szSettingFile, 
		sizeof(szSettingFile),
		sizeof(szSettingFile) - 1,
		"%s%s",
		g_szApplicationPath,
		TESTSETTING_FILE
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	//读取客户端目录，如果不存在，则将会把运行目录设为客户端目录
	::GetPrivateProfileString(
		SECTION_SETTINGS,
		KEY_CLIENTPATH,
		"",
		m_szClientPath,
		sizeof(m_szClientPath),
		szSettingFile);

	for (int i = 1;; i++)
	{
		char szKeyTestMap[MAX_PATH];
		char szMapName[MAX_PATH];
		nRetCode = _snprintf_s(
			szKeyTestMap, 
			sizeof(szKeyTestMap),
			sizeof(szKeyTestMap) - 1,
			"%s%02d",
			KEY_TESTMAP_PREFIX,
			i
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);

		::GetPrivateProfileString(
			SECTION_SETTINGS,
			szKeyTestMap,
			"",
			szMapName,
			sizeof(szMapName),
			szSettingFile);

		if (szMapName[0] == '\0')
		{
			if (i == 1)
			{
				m_bFindDisuseMapRes = true;
			}
			break;
		}
		m_vecMapList.push_back(szMapName);
	}
	//读取检查的资源类型
	::GetPrivateProfileString(
		SECTION_SETTINGS,
		KEY_RESOURCETYPE,
		"",
		szResType,
		sizeof(szResType),
		szSettingFile);

	nRetCode = AnalyseResType(szResType);
	KGLOG_PROCESS_ERROR(nRetCode);
	//读取源始资源目录
	for (int i = 1;; i++)
	{
		char szKeyResPath[MAX_PATH];
		char szResPath[MAX_PATH];
		nRetCode = _snprintf_s(
			szKeyResPath, 
			sizeof(szKeyResPath),
			sizeof(szKeyResPath) - 1,
			"%s%02d",
			KEY_RESOURCEPATH_PREFIX,
			i
			);
		KGLOG_PROCESS_ERROR(nRetCode > 0);

		::GetPrivateProfileString(
			SECTION_SETTINGS,
			szKeyResPath,
			"",
			szResPath,
			sizeof(szResPath),
			szSettingFile);

		KGLOG_PROCESS_ERROR(szResPath[0] != '\0' || i != 1);
		if (szResPath[0] == '\0')
		{
			break;
		}
		m_vecResPath.push_back(szResPath);
	}


	nResult = true;
Exit0:
	return nResult;
}

int KGTestMapDisuseResource::AnalyseResType(const char cszResType[])
{
	int nResult  = false; 
	int nRetCode = false;
	char szSep[] = "|";
	char* pszToken = NULL;
	char szResType[MAX_PATH];

	nRetCode = strncpy_s(
		szResType, sizeof(szResType),
		cszResType, strlen(cszResType));
	KGLOG_PROCESS_ERROR(nRetCode == 0);

	pszToken = strtok(szResType, szSep);   
	while(pszToken != NULL)   
	{
		m_vecResType.push_back(pszToken);   
		pszToken = strtok(NULL, szSep);   
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGTestMapDisuseResource::FindSourceResource()
{
	int nResult  = false; 
	int nRetCode = false;
	char szMapSourcePath[MAX_PATH];
	vector<string> vecResList;

	for (size_t i = 0; i < m_vecResType.size(); i++)
	{
		m_mapSourceRes.insert(make_pair(m_vecResType[i], vecResList));
	}

	for (size_t i = 0; i < m_vecResPath.size(); i++)
	{
		nRetCode = _snprintf_s(szMapSourcePath,
			sizeof(szMapSourcePath),
			sizeof(szMapSourcePath) - 1,
			"%s%s", 
			m_szClientPath,
			m_vecResPath[i].c_str());
		KGLOG_PROCESS_ERROR(nRetCode > 0);
		nRetCode = LoadSourceResourceInfo(szMapSourcePath);
		KGLOG_PROCESS_ERROR(nRetCode);

	}

	nResult = true;
Exit0:
	return nResult;
}

int KGTestMapDisuseResource::CheckLostResource()
{
	int nResult  = false; 
	int nRetCode = false;
	char szResName[MAX_PATH];
	set<string>::iterator iter;

	for (iter = m_setUsedRes.begin(); iter != m_setUsedRes.end(); ++iter)
	{
		nRetCode = _snprintf_s(szResName,
			sizeof(szResName),
			sizeof(szResName) - 1,
			"%s%s", 
			m_szClientPath,
			(*iter).c_str());
		KGLOG_PROCESS_ERROR(nRetCode > 0);
		nRetCode = _access(szResName, 0);
		if (nRetCode == 0)
		{
			(*iter).clear();
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int KGTestMapDisuseResource::OutputDisuseRes(const char cszResultPath[])
{
	int nResult  = false; 
	int nRetCode = false;
	int nTotal   = 0;
	char szResultName[MAX_PATH];
	string strWrite;
	FILE* pResultFile = NULL;
	map<string, vector<string>>::iterator iter;

	nRetCode = _snprintf_s(szResultName,
		sizeof(szResultName),
		sizeof(szResultName) - 1,
		"%s%s",
		cszResultPath,
		RESULT_DISUSE_RES
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	pResultFile = fopen(szResultName, "w");
	KGLOG_PROCESS_ERROR(pResultFile);

	//输出未使用的LOD资源，即某些LOD资源没有其对应的.mesh或.mtl在地图里用到
	strWrite = "【Resource Type】 _L.Mesh | _L.Mtl\n【Resource Name】\n";
	fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);
	for (size_t i = 0; i < m_vecLODRes.size(); i++)
	{
		if (m_vecLODRes[i].empty())
		{
			continue;
		}
		strWrite = "\t" + m_vecLODRes[i] + "\n";
		fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);
		nTotal++;
	}
	strWrite = "【Total Num】 " + KGTestCommon::IntToString(nTotal) + "\n\n";
	fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);

	//输出未使用的地图资源
	for (iter = m_mapSourceRes.begin(); iter != m_mapSourceRes.end(); ++iter)
	{
		nTotal = 0;
		if (iter->second.size() == 0)
		{
			continue;
		}
		strWrite = "【Resource Type】 " + iter->first + "\n【Resource Name】\n";
		fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);
		for (size_t i = 0; i < iter->second.size(); i++)
		{
			if (iter->second[i].empty())
			{
				continue;
			}
			strWrite = "\t" + iter->second[i] + "\n";
			fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);
			nTotal++;
		}
		strWrite = "【Total Num】 " + KGTestCommon::IntToString(nTotal) + "\n\n";
		fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);
	}

	nResult = true;
Exit0:
	if (pResultFile)
	{
		fclose(pResultFile);
		pResultFile = NULL;
	}
	return nResult;
}

int KGTestMapDisuseResource::OutputLostRes(const char cszResultPath[])
{
	int nResult  = false; 
	int nRetCode = false;
	char szResultName[MAX_PATH];
	string strWrite;
	FILE* pResultFile = NULL;
	int nTotal = 0;
	set<string>::iterator iter;

	//输出缺失资源
	nRetCode = _snprintf_s(szResultName,
		sizeof(szResultName),
		sizeof(szResultName) - 1,
		"%s%s",
		cszResultPath,
		RESULT_LOST_RES
		);
	KGLOG_PROCESS_ERROR(nRetCode > 0);

	pResultFile = fopen(szResultName, "w");
	KGLOG_PROCESS_ERROR(pResultFile);

	nTotal = 0;
	strWrite = "【Lost Resource Name】\n";
	fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);
	for (iter = m_setUsedRes.begin(); iter != m_setUsedRes.end(); ++iter)
	{
		if ((*iter).empty())
		{
			continue;
		}
		strWrite = "\t" + *iter + "\n";
		fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);
		nTotal++;
	}
	strWrite = "【Total Num】" + KGTestCommon::IntToString(nTotal) + "\n";
	fwrite(strWrite.c_str(), strWrite.length(), 1, pResultFile);

	nResult = true;
Exit0:
	if (pResultFile)
	{
		fclose(pResultFile);
		pResultFile = NULL;
	}
	return nResult;
}