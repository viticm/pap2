////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCollocator.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-10 17:58:52
//  Comment     : 配置器。配置类之间的关系，避免硬耦合。向各个类注册构造器。
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"

#include "KG3DCollocator.h"
#include "IEKG3DCollocator.h"

#include "KG3DScene.h"
//#include "KG3DPointLight.h"
#include "KG3DCommonObjectBuilder.h"
#include "KG3DSceneObjectPlacementTool.h"
#include "KG3DSceneSelectionToolImp.h"
#include "KG3DCommonObject.h"
#include "KG3DRepresentObjectDummy.h"
#include "KG3DRepresentObjectTable.h"
#include "KG3DSceneTopSnapTool.h"
//#include "KG3DDefaultShaderGroup.h"
#include "KG3DSceneObjectTransformTool.h"
#include "KG3DSceneCheckers.h"
//#include "KG3DShaderManager.h"
#include "KG3DEnvEffect.h"
#include "KG3DRegionInfoManager.h"
#include "KG3DTerrainBlock.h"
#include "KG3DShaderCollocation.h"
#include "KGIniFileHelpers.h"
#include "KG3DEngineManager.h"
#include "KG3DSceneSceneEditor.h"
#include "KG3DEnvironment.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//对外是const的，对内的话，用特殊的办法改
const KG3DGlobalSwitch g_Switch;

KG3DGlobalSwitch::KG3DGlobalSwitch()
:bNewSelection(FALSE)
,bSelectableRefactorFinished(FALSE)
{

}
//<GLOBAL_SWITCH>
#define REGISTER_GLOBAL_FILTER(filter)	s_NameCt[_T(#filter)] = offsetof(KG3DGlobalSwitch, filter)

const BOOL* KG3DGlobalSwitch::GetSwitchByName(const KG3DGlobalSwitch& switchIn, LPCTSTR strName )
{
	typedef std::map<std::tstring, size_t>	TypeGlobalFilterNameCt;	//记录偏移
	static TypeGlobalFilterNameCt	s_NameCt;
	static bool s_bIsNameCtInited = false;
	if (! s_bIsNameCtInited)
	{
		REGISTER_GLOBAL_FILTER(bNewSelection);
		REGISTER_GLOBAL_FILTER(bFloorChecker);

		//新的Switch添加到下面
		s_bIsNameCtInited = true;
	}

	_ASSERTE(0 != s_NameCt.size());
	
	_ASSERTE(NULL != strName);
	if (NULL == strName)
	{
		return NULL;
	}
	std::tstring strTemp(strName);
	TypeGlobalFilterNameCt::iterator it = s_NameCt.find(strTemp);
	if (it != s_NameCt.end())
	{
		size_t uOffset= it->second;
		_ASSERTE(1 == sizeof(char));
		const BOOL* p = (const BOOL*)((const char*)(&switchIn) + uOffset);
		_ASSERTE((size_t*)p < (size_t*)(&switchIn)+(sizeof(switchIn)/sizeof(char)));
		return p;
	}
	return NULL;
}


////////////////////////////////////////////////////////////////////////////////

VOID KG3DCollocator::ToggleGlobalSwitch( const BOOL& filter, BOOL bEnable)
{
	const_cast<BOOL&>(filter) = bEnable;
}

const BOOL& KG3DCollocator::GetSwitchByName(LPCTSTR name, BOOL* pSucceeded)
{
	const BOOL* pRet = KG3DGlobalSwitch::GetSwitchByName(g_Switch, name);
	if (NULL != pRet)
	{
		if (NULL != pSucceeded)
		{
			*pSucceeded = TRUE;
		}
		return *pRet;
	}

	if (NULL != pSucceeded)
	{
		*pSucceeded = FALSE;
	}

	static BOOL bNone;
	return bNone;
}
//</GLOBAL_SWITCH>

void KG3DCollocator::CollocateBeforeInit()
{	
	CheckRelationships();
	RegisterTypes();
	__try
	{
		ReadSettings();
	}
	__except(EXCEPTION_EXECUTE_HANDLER)	//注意，这里是SEH异常，不是C++异常处理机制，这个是用来捕获IO异常的。这个函数调用的很早，比较敏感，不能让IO妨碍其运行
	{
		_ASSERTE(NULL && _T("为什么出现IO异常，要仔细检查"));
		KGLogPrintf(KGLOG_ERR, _T("异常被捕获"));
	}

	KG3DShaderCollocation::Collocate();
}
	
void KG3DCollocator::RegisterTypes()
{
	//配置创建器
	{
		KG3DCommonObjectBuilder& Builder = KG3DScene::GetCommonObjectBuilder();
		/*Builder.RegisterType(KG3DTYPE_ENVEFF_POINTLIGHT_TABLE
		, &KG3DPointLightTable::Build, &KG3DPointLightTable::Save);*/

		Builder.RegisterType(KG3DTYPE_SCENE_OBJECT_PLACEMENT_TOOL
			, &KG3DSceneObjectPlacementTool::Build, NULL);

		Builder.RegisterType(KG3DTYPE_SCENE_TOP_SNAP_TOOL, &KG3DSceneTopSnapTool::Build, NULL);
		//<SELECTION_TOOL>
#if 1
		Builder.RegisterType(KG3DTYPE_SCENE_SELECTION_TOOL, &KG3DSceneSelectionToolSolid::Build, NULL);
		Builder.RegisterType(KG3DTYPE_ROTATION_TOOL, &BuildKG3DRotationTool, NULL);
		Builder.RegisterType(KG3DTYPE_TRANSLATION_TOOL, &BuildKG3DTranslationTool, NULL);
		Builder.RegisterType(KG3DTYPE_SCALING_TOOL, &BuildKG3DScalingTool, NULL);
#endif
		//</SELECTION_TOOL>

	}	

	//配置动态天气
	{
		///<KG3DEnvEffect SubClass Register Point>
		KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffectClassLibrary& classLib = KG3D_ENVIRONMENT_EFFECT::g_GetEnvEffectClassLibrary();
		KG3DEnvEffectClassProperty properties[] = 
		{
			{KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM, KG3DTYPE_ENVEFF_INVALID_EFFECT, false},
			{KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX, KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM, true},
			{KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN, KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM, true},
			{KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD, KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM, true},
			{KG3DTYPE_ENVEFF_FOG, KG3DTYPE_ENVEFF_INVALID_EFFECT, true},
			{KG3DTYPE_ENVEFF_LENSFLARE_SUN, KG3DTYPE_ENVEFF_INVALID_EFFECT, true},
			{KG3DTYPE_ENVEFF_LENSFLARE_MOON, KG3DTYPE_ENVEFF_INVALID_EFFECT, true},
			{KG3DTYPE_ENVEFF_LIGHTSET, KG3DTYPE_ENVEFF_INVALID_EFFECT, true},
			{KG3DTYPE_ENVEFF_LIGHT, KG3DTYPE_ENVEFF_LIGHTSET, true},
			{KG3DTYPE_ENVEFF_RAIN, KG3DTYPE_ENVEFF_INVALID_EFFECT, false},
			{KG3DTYPE_ENVEFF_SNOW, KG3DTYPE_ENVEFF_INVALID_EFFECT, false},
			{KG3DTYPE_ENVEFF_WIND, KG3DTYPE_ENVEFF_INVALID_EFFECT, false},
			//{KG3DTYPE_ENVEFF_STARRY_NIGHT, KG3DTYPE_ENVEFF_INVALID_EFFECT, true},
			{KG3DTYPE_ENVEFF_DYNAMIC_WEATHER, KG3DTYPE_ENVEFF_INVALID_EFFECT, false},
			{KG3DTYPE_ENVEFF_DUMMY, KG3DTYPE_ENVEFF_INVALID_EFFECT, false},
		};

		BOOL bRet = FALSE;
		for (size_t i = 0; i < _countof(properties); ++i)
		{
			bRet = classLib.RegisterType(properties[i]);
			_ASSERTE(bRet);
		}
	}
}
void KG3DCollocator::ReadSettings()
{
	IIniFile* pSettings = NULL;
	IIniFile* pDefaultEnv = NULL;

	{
		LPCTSTR strDefaultSettinsPath = _T("data/public/3DEngineSettings.ini");
		pSettings = g_OpenIniFile(strDefaultSettinsPath);
		if(NULL == pSettings)
		{
			KGLogPrintf(KGLOG_ERR, _T("文件读取失败：%s"), strDefaultSettinsPath);
			goto Exit0;
		}

		TCHAR strEnvEffDefault[MAX_PATH] = _T("");
		bool bRet = KGCH::LoadINIFileData(pSettings, _T("DefaultFiles"), _T("EnvEffectDefault"), _T("")
			, strEnvEffDefault, _countof(strEnvEffDefault));
		KGLOG_PROCESS_ERROR(bRet);

		SAFE_RELEASE(pSettings);

		_ASSERTE(0 != _tcslen(strEnvEffDefault));

		pDefaultEnv = g_OpenIniFile(strEnvEffDefault);
		if(NULL == pDefaultEnv)
		{
			KGLogPrintf(KGLOG_ERR, _T("文件读取失败：%s"), strEnvEffDefault);
			goto Exit0;
		}

		KG3D_ENVIRONMENT_EFFECT::KG3DEnvEffLight::LoadDefaultSunAndPlayerLight(pDefaultEnv);
	}
Exit0:
	SAFE_RELEASE(pSettings);
	SAFE_RELEASE(pDefaultEnv);
	return;
}
void KG3DCollocator::CollocateAfterInit(LPDIRECT3DDEVICE9 pDevice)
{
	HRESULT hr = g_GetObjectTable().RegisterType(REPRESENTOBJECT_DUMMY, &KG3DRepresentObjectDummy::Build);
	VERIFY(SUCCEEDED(hr));

	CheckResources(pDevice);

	hr = KG3DTerrainConverMap::InitUniqueDefault();
	if(FAILED(hr))
	{
		KGLogPrintf(KGLOG_ERR, _T("默认的ConverMap分配失败，可能导致部分Scene渲染不正确"));
	}
	return;
}
void KG3DCollocator::UnCollocateBeforeUnInit( LPDIRECT3DDEVICE9 pDevice )
{
	if (NULL != pDevice)//这个有可能为NULL，因为可能创建设备就失败了
	{
		
	}
	KG3DTerrainConverMap::UnInitUniqueDefault();

	KG3DShaderCollocation::UnCollocate();	//主动析构
}
namespace
{
	LPCTSTR GetSaveDesc(DWORD dwType)
	{
		TypeSaveOptionDescArrayRef opDescArray = g_GetSaveOptionDesc();
		for (size_t i = 0; i < _countof(opDescArray); ++i)
		{
			if (dwType == opDescArray[i].dwOp)
			{
				return opDescArray[i].strDesc;
			}
		}
		static TCHAR strUnkownType[] = _T("未知类型");
		return strUnkownType;
	}
};

//这个函数很重要，搞错了保存路径就错了，很有可能造成文件丢失，所以下面有对应的测试。如果改这个函数，一定要仔细改对应的测试
HRESULT KG3DCollocator::GetSavePath( DWORD dwType, const KGCH::StrPathSplited& scenePathSplited	//dwType是EM_SAVE_OP_ENVIRONMENT等
									, KGCH::StrPathSplited* pRetPathSplited		//可以为NULL。返回分裂的路径，得到盘符和路径都很方便
									, LPTSTR pathRet, size_t uPathBufferSize )	//可以为NULL
{
	{
		errno_t errCode = 0;
		KGCH::StrPathSplited returnPath = scenePathSplited;
		_ASSERTE(0 == _tcsicmp(scenePathSplited.ext, _T(".map")) && _T("源路径应该是map文件"));
		switch(dwType)
		{
		case EM_SAVE_OP_TERRAIN_AND_ENTITY:
			{
				//如果是地形和Entity，那么存的就是map文件
			}
			break;
		case EM_SAVE_OP_ENVIRONMENT:
            {
                errCode = _tcscat_s(returnPath.fileName, _T("_Setting"));
                KG_PROCESS_ERROR(0 == errCode);

                extern KG3DEngineManager g_cEngineManager;
                KG3DSceneSceneEditor* pEditor = g_cEngineManager.GetMainScene();
                if (pEditor)
                {
                    DWORD dwType = pEditor->GetEnvironment().GetSettingType();
                    if (dwType == 1)
                    {
                        errCode = _tcscat_s(returnPath.fileName, _T("Advanced"));
                        KG_PROCESS_ERROR(0 == errCode);
                    }
                }
                errCode = _tcscpy_s(returnPath.ext, _T(".ini"));
                KG_PROCESS_ERROR(0 == errCode);
            }
            break;
		case EM_SAVE_OP_SCENE_SETTINGS:
			{
				errCode = _tcscat_s(returnPath.fileName, _T("_Setting"));
				KG_PROCESS_ERROR(0 == errCode);
				errCode = _tcscpy_s(returnPath.ext, _T(".ini"));
				KG_PROCESS_ERROR(0 == errCode);
			}
			break;
		case EM_SAVE_OP_CAMERA_ANIMATION:
			{
				errCode = _tcscpy_s(returnPath.ext, _T(".CameraAni"));
				KG_PROCESS_ERROR(0 == errCode);
			}
			break;
		case EM_SAVE_OP_REGION_INFO:
			{
				size_t uDirLen = _tcslen(scenePathSplited.dir);
				KG_PROCESS_ERROR(uDirLen > 0);
				int nRet = 0;
				if (KGCH::IsDirSpliter(scenePathSplited.dir[uDirLen - 1]))
				{
					nRet = _stprintf_s(returnPath.dir, _T("%s%sRegionInfo/"), scenePathSplited.dir, scenePathSplited.fileName);
				}
				else
				{
					nRet = _stprintf_s(returnPath.dir, _T("%s/%sRegionInfo/"), scenePathSplited.dir, scenePathSplited.fileName);
				}
				
				KG_PROCESS_ERROR(0 != nRet);
				LPCTSTR defaultRegionInfoFileName = KG3DRegionDWInfoManager::GetDefaultMapName();//带文件名和后缀，要分裂一下
				KGCH::StrPathSplited defaultRegionInfoFileNameSplited;
				BOOL bRetTemp = KGCH::StrPathSplit(defaultRegionInfoFileName, defaultRegionInfoFileNameSplited
											, KGCH::em_path_split_filename | KGCH::em_path_split_ext);
				KG_PROCESS_ERROR(bRetTemp);
				errCode = _tcscpy_s(returnPath.fileName, defaultRegionInfoFileNameSplited.fileName);
				KG_PROCESS_ERROR(0 == errCode);
				errCode = _tcscpy_s(returnPath.ext, defaultRegionInfoFileNameSplited.ext);
				KG_PROCESS_ERROR(0 == errCode);
			}
			break;
        case EM_SAVE_OP_EFFECTPARAMS:
            {
                errCode = _tcscat_s(returnPath.fileName, _T("_PostRender"));
                KG_PROCESS_ERROR(0 == errCode);

                extern KG3DEngineManager g_cEngineManager;
                KG3DSceneSceneEditor* pEditor = g_cEngineManager.GetMainScene();
                if (pEditor)
                {
                    DWORD dwType = pEditor->GetEnvironment().GetSettingType();
                    if (dwType == 1)
                    {
                        errCode = _tcscat_s(returnPath.fileName, _T("_ADV"));
                        KG_PROCESS_ERROR(0 == errCode);
                    }
                    else
                    {
                        errCode = _tcscat_s(returnPath.fileName, _T("_LOW"));
                        KG_PROCESS_ERROR(0 == errCode);
                    }
                }
                else
                {
                    errCode = _tcscat_s(returnPath.fileName, _T("_LOW"));
                    KG_PROCESS_ERROR(0 == errCode);
                }

                errCode = _tcscpy_s(returnPath.ext, _T(".ini"));
                KG_PROCESS_ERROR(0 == errCode);
            }
            break;
		default:
			break;
		}

		if (NULL != pRetPathSplited)
		{
			*pRetPathSplited = returnPath;
		}
		if (NULL != pathRet)
		{
			BOOL bRet = returnPath.GetCatedPath(pathRet, uPathBufferSize);
			KG_PROCESS_ERROR(bRet);
			pathRet[uPathBufferSize - 1] = _T('\0');
		}
		return S_OK;
	}
Exit0:
#if defined(DEBUG) | defined(_DEBUG)
	if(! TDD::IsTDDWorking())
#endif	
	{
		LPCTSTR strSaveOpDesc = GetSaveDesc(dwType);
		_ASSERTE(NULL != strSaveOpDesc);
		KGLogPrintf(KGLOG_ERR, _T("取文件路径失败，可能会引发加载对应的文件失败或者保存对应的文件失败：%s"), strSaveOpDesc);
	}
	return E_FAIL;
}

struct KG3DCollocatorInstance : public IEKG3DCollocator 
{
	virtual HRESULT GetSavePath(DWORD dwType, LPCTSTR scenePath
		, KG_CUSTOM_HELPERS::StrPathSplited* pRetPathSplited	//可以为NULL。返回分裂的路径，得到盘符和路径都很方便
		, LPTSTR pathRet, size_t uPathBufferSize)
	{
		KGCH::StrPathSplited pathSplited;
		KG_PROCESS_ERROR(NULL != scenePath && NULL != pathRet);
		{
			BOOL bRet = KGCH::StrPathSplit(scenePath, pathSplited);
			KG_PROCESS_ERROR(bRet);

			return KG3DCollocator::GetSavePath(dwType, pathSplited, pRetPathSplited, pathRet, uPathBufferSize);
		}
Exit0:
		return E_FAIL;
	}
};

IEKG3DCollocator* KG3DCollocator::GetAnotherInterface()
{
	static KG3DCollocatorInstance stInstance;
	return &stInstance;
}

void KG3DCollocator::CheckResources(LPDIRECT3DDEVICE9 pDevice)
{
	KG3DShaderCollocation::CheckShaders();

	//检查Base库里面的函数的行为。Base库里面有些函数没有检查Buffer，
	{
		TCHAR fullPath[MAX_PATH] = _T("");
		g_GetRootPath(fullPath);
		_ASSERTE(0 != _tcscmp(fullPath, _T("")));
	}

	_ASSERTE(typeid(TCHAR) == typeid(CHAR) && _T("引擎应该是非Unicode编译的，否则太多的函数的兼容性没法检查"));
}

void KG3DCollocator::CheckRelationships()
{
	//标志的相关性,非常重要，最好看清楚再改。标志顺序错了程序有崩溃的可能性
	{
		_ASSERTE(KM_D3DDEVICE_CREATE >= KM_D3DDEVICE_BEGIN && KM_D3DDEVICE_CREATE < KM_D3DDEVICE_END);
		_ASSERTE(KM_D3DDEVICE_RELEASE >= KM_D3DDEVICE_BEGIN && KM_D3DDEVICE_RELEASE < KM_D3DDEVICE_END);
		_ASSERTE(KM_D3DDEVICE_LOST >= KM_D3DDEVICE_BEGIN && KM_D3DDEVICE_LOST < KM_D3DDEVICE_END);
		_ASSERTE(KM_D3DDEVICE_RESET >= KM_D3DDEVICE_BEGIN && KM_D3DDEVICE_RESET < KM_D3DDEVICE_END);


		_ASSERTE(EXEACTION_LEFE_KEY_DOWN + 1 == EXEACTION_SELECTION_ADD);
		_ASSERTE(EXEACTION_SELECTION_ADD + 1 == EXEACTION_SELECTION_SUTRACT);
	}

	//静态强转检查
	_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(KG3DCommonObject, KG3DSceneObjectPlacementTool);
	_KG3D_DEBUG_SUPPERSUB_POINTER_EQUAL(KG3DCommonObject, KG3DSceneSelectionToolSolid);

	//检查
	{

	}
}



#ifdef KG_ENABLE_TDD
#if !defined(DEBUG) & !defined(_DEBUG)
NULL	//记得关闭KG_ENABLE_TDD，暂时才忠在改东西，在库代码里面不要打开// Last modify by Chen TianHong:2008-12-3 9:48:03
#endif
#endif


#ifdef KG_ENABLE_TDD
KG_TEST_BEGIN(KG3DCollocator)
{
	//测试斜杠分隔的情况
	{
		TCHAR scenePath[] = _T("d:\\program files/scene\\scene.map");

		KG_CUSTOM_HELPERS::StrPathSplited splitedPath;
		KG_CUSTOM_HELPERS::StrPathSplit(scenePath, splitedPath);

		TCHAR retBuffer[MAX_PATH];

		TCHAR sceneEnvPath[] = _T("d:\\program files/scene\\scene_Setting.ini");
		KG_CUSTOM_HELPERS::StrPathSplited splitedPathRet;
		KG3DCollocator::GetSavePath(EM_SAVE_OP_ENVIRONMENT, splitedPath, &splitedPathRet, retBuffer, _countof(retBuffer));
		_ASSERTE(0 == _tcscmp(retBuffer, sceneEnvPath));
		_ASSERTE(0 == _tcscmp(splitedPath.dir, splitedPathRet.dir));

		HRESULT hr = KG3DCollocator::GetSavePath(EM_SAVE_OP_ENVIRONMENT, splitedPath, NULL, NULL, 0);
		_ASSERTE(SUCCEEDED(hr));
	}
	//测试其它
	{
		//全路径下
		{
			TCHAR scenePath[] = _T("d:/program files/scene/scene.map");

			KG_CUSTOM_HELPERS::StrPathSplited splitedPath;
			KG_CUSTOM_HELPERS::StrPathSplit(scenePath, splitedPath);

			TCHAR retBuffer[MAX_PATH];

			TCHAR sceneEnvPath[] = _T("d:/program files/scene/scene_Setting.ini");
			KG3DCollocator::GetSavePath(EM_SAVE_OP_ENVIRONMENT, splitedPath, NULL, retBuffer, _countof(retBuffer));
			_ASSERTE(0 == _tcscmp(retBuffer, sceneEnvPath));

			TCHAR sceneSettingPath[] = _T("d:/program files/scene/scene_Setting.ini");
			KG3DCollocator::GetSavePath(EM_SAVE_OP_SCENE_SETTINGS, splitedPath, NULL, retBuffer, _countof(retBuffer));
			_ASSERTE(0 == _tcscmp(retBuffer, sceneSettingPath));

			TCHAR strCamAniPath[] = _T("d:/program files/scene/scene.CameraAni");
			KG3DCollocator::GetSavePath(EM_SAVE_OP_CAMERA_ANIMATION, splitedPath, NULL, retBuffer, _countof(retBuffer));
			_ASSERTE(0 == _tcscmp(retBuffer, strCamAniPath));

			TCHAR strRegionInfoPath[] = _T("d:/program files/scene/sceneRegionInfo/DWSplit.bmp");
			KG3DCollocator::GetSavePath(EM_SAVE_OP_REGION_INFO, splitedPath, NULL, retBuffer, _countof(retBuffer));
			_ASSERTE(0 == _tcscmp(retBuffer, strRegionInfoPath));
		}
		//相对路径下
		{
			TCHAR scenePath[] = _T("d:/program files/scene/scene.map");

			KGCH::StrPathSplited splitedPath;
			KGCH::StrPathSplit(scenePath, splitedPath, KGCH::em_path_split_all_without_driver);

			TCHAR retBuffer[MAX_PATH];

			TCHAR sceneEnvPath[] = _T("program files/scene/scene_Setting.ini");
			KG3DCollocator::GetSavePath(EM_SAVE_OP_ENVIRONMENT, splitedPath, NULL, retBuffer, _countof(retBuffer));
			_ASSERTE(0 == _tcscmp(retBuffer, sceneEnvPath));

			TCHAR sceneSettingPath[] = _T("program files/scene/scene_Setting.ini");
			KG3DCollocator::GetSavePath(EM_SAVE_OP_SCENE_SETTINGS, splitedPath, NULL, retBuffer, _countof(retBuffer));
			_ASSERTE(0 == _tcscmp(retBuffer, sceneSettingPath));

			TCHAR strCamAniPath[] = _T("program files/scene/scene.CameraAni");
			KG3DCollocator::GetSavePath(EM_SAVE_OP_CAMERA_ANIMATION, splitedPath, NULL, retBuffer, _countof(retBuffer));
			_ASSERTE(0 == _tcscmp(retBuffer, strCamAniPath));

			TCHAR strRegionInfoPath[] = _T("program files/scene/sceneRegionInfo/DWSplit.bmp");
			KG3DCollocator::GetSavePath(EM_SAVE_OP_REGION_INFO, splitedPath, NULL, retBuffer, _countof(retBuffer));
			_ASSERTE(0 == _tcscmp(retBuffer, strRegionInfoPath));
		}
	}
	//SaveOp
	{
		DWORD dwOp = EM_SAVE_OP_TERRAIN_AND_ENTITY
			| EM_SAVE_OP_SCENE_SETTINGS 
			| EM_SAVE_OP_CAMERA_ANIMATION 
			| EM_SAVE_OP_REGION_INFO
			| EM_SAVE_OP_ENVIRONMENT
            | EM_SAVE_OP_EFFECTPARAMS
//			| EM_SAVE_OP_ROAD
//			| EM_SAVE_OP_RIVER
			;
		_ASSERTE(EM_SAVE_OP_ALL == dwOp && _T("如果不等，检查是不是加了新的Op，并改上面的算式"));

		DWORD dwAll = EM_SAVE_OP_ALL;
		UINT uOpCount = 0;
		for (; 0 != dwAll; ++uOpCount)
		{
			dwAll = dwAll >> 1;
		}
		_ASSERTE(EM_SAVE_OP_COUNT == uOpCount && _T("EM_SAVE_OP_COUNT必须计算正确，不然保存面板上少了点东西可大事"));
	}
}
KG_TEST_END(KG3DCollocator)
#endif
