////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DShaderCollocation.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-7-17 16:08:22
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdAfx.h"
#include "KG3DShaderCollocation.h"
#include "KG3DDefaultShaderGroup.h"
#include "KG3DSceneCheckers.h"
#include "KG3DShaderRCBridge.h"
#include "KG3DShaderManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////
//#define MY_MAKESHADERIDSTRING(ID)	KG3DShaderRCBridge::GetStringFromID(ID, strBuffer)

#define USE_RESOURCE_SHADER	0	//新的引擎使用资源dll加载shader，在相关文件在各个分支完全提交并检查之前，暂时还是用文件，之后可以一下子就切换过去

#if	USE_RESOURCE_SHADER == 1
#define MAKESHADERSTRING(path)	KG3DShaderManager::GetResourceString(path)
#else
#define MAKESHADERSTRING(path)	path
#endif

VOID KG3DShaderCollocation::Collocate()
{
	//int* p = new int;
#if USE_RESOURCE_SHADER == 1
	KG3DShaderRCBridge::InitLibrary();
#endif

	//配置默认的Shader,请按照其枚举值的顺序填写下面的数组
	DWORD dwDefaultShaderCompileFlag = 0;

	//注意D3DXMACRO_ARRAY里面那个指针，D3DXMACRO_ARRAY直接记录到DefaultShaderGroup里面。所以那个指针要么是NULL，要么
	//保证其指向正确的内粗。而既然DefaultShaderGroup是static的，所以最好指向的也是静态的内存，否则改代码，支持一个Buffer
	D3DXMACRO_ARRAY macroDefault = D3DXMACRO_ARRAY::GetEmptyMacroArray();//这个Macro是全部shader都会用的，所以可以设一些全局的定义，不过因为如果这里设了，在引擎外编译shader的时候不容易检查正确性，所以最好还是不要这么干

#ifdef DEBUG_VS
	dwDefaultShaderCompileFlag |= D3DXSHADER_FORCE_VS_SOFTWARE_NOOPT;
	dwDefaultShaderCompileFlag |= D3DXSHADER_DEBUG ;//| D3DXSHADER_SKIPOPTIMIZATION;
#endif
#ifdef DEBUG_PS
	dwDefaultShaderCompileFlag |= D3DXSHADER_FORCE_PS_SOFTWARE_NOOPT;
	dwDefaultShaderCompileFlag |= D3DXSHADER_DEBUG ;//| D3DXSHADER_SKIPOPTIMIZATION;
#endif

	//2009-8-5 下面改用资源ID作为文件路径，shaderManager会载入文件路径
	KG3DDefaultShaderRegistrationArg Args[] = 
	{
		{
			DEFST_LIBRARY,
			MAKESHADERSTRING(_T("Data\\public\\shader\\Library.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\Library.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\Library.fx")),
			0,
			macroDefault, NULL, dwDefaultShaderCompileFlag, 
		},//<Shared Pool>
		{
			DEFST_SKINMESH,
			MAKESHADERSTRING(_T("Data\\public\\shader\\shaderskin_2.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\shaderskin_2.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\shaderskin_3.fx")),
			sizeof(KG3D_DEFST_SKINMESH_ParamHandle),
			macroDefault, NULL, dwDefaultShaderCompileFlag,
		},
		{
			DEFST_POST_RENDER,
			MAKESHADERSTRING(_T("Data\\public\\shader\\PostRenderFinalPass.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\PostRenderFinalPass.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\PostRenderFinalPass.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag, 
		},//<PostRenderShadowMove>
		{
			DEFST_NEWLIGHT,
			MAKESHADERSTRING(_T("Data\\public\\shader\\new light mode.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\new light mode.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\new light mode v3.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_NORMAL_NOSKIN,
			MAKESHADERSTRING(_T("Data\\public\\shader\\Normal map without skin.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\Normal map without skin.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\Normal map without skin.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_MODEL_SHADOW,
			MAKESHADERSTRING(_T("Data\\public\\shader\\ModelShadow.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\ModelShadow.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\ModelShadow.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_LIGHT_MAP,
			MAKESHADERSTRING(_T("Data\\public\\shader\\lightmap.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\lightmap.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\lightmap.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_CLOUD,
			MAKESHADERSTRING(_T("Data\\public\\shader\\cloud.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\cloud.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\cloud.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_NEWLIGHT_DETAIL,
			MAKESHADERSTRING(_T("Data\\public\\shader\\new light mode_detail_2_0.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\new light mode_detail_2_0.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\new light mode_detail.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_TERRAIN,
			MAKESHADERSTRING(_T("Data\\public\\shader\\TerrainRender_2_0.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\TerrainRender_2_0.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\TerrainRender.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_TERRAIN_NORMALMAP,
			MAKESHADERSTRING(_T("Data\\public\\shader\\TerrainRender_normalmap.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\TerrainRender_normalmap.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\TerrainRender_normalmap.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_GRASS,
			MAKESHADERSTRING(_T("Data\\public\\shader\\TerrainGrass.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\TerrainGrass.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\TerrainGrass.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_LENSFLARE,
			MAKESHADERSTRING(_T("Data\\public\\shader\\Lensflare.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\Lensflare.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\Lensflare.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_TERRAIN_ROAD,
			MAKESHADERSTRING(_T("Data\\public\\shader\\Road_2_0.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\Road_2_0.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\Road.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_SFX_BOX_PROJ,
			MAKESHADERSTRING(_T("Data\\public\\shader\\box_proj_effect.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\box_proj_effect.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\box_proj_effect.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_BILLBOARD,
			MAKESHADERSTRING(_T("Data\\public\\shader\\shaderbillboard.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\shaderbillboard.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\shaderbillboard.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_HIGH_GRASS,
			MAKESHADERSTRING(_T("Data\\public\\shader\\Grass.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\Grass.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\Grass.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_SPEED_TREE,
			MAKESHADERSTRING(_T("Data\\public\\shader\\SpeedTree.fx")),	
			MAKESHADERSTRING(_T("Data\\public\\shader\\SpeedTree.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\SpeedTree v3.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_WATER,
			MAKESHADERSTRING(_T("Data\\public\\shader\\water.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\water.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\water.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_SKYBOX,
			MAKESHADERSTRING(_T("Data\\public\\shader\\SkyBox.fx")),	
			MAKESHADERSTRING(_T("Data\\public\\shader\\SkyBox.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\SkyBox.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_FARMOUNTAIN,
			MAKESHADERSTRING(_T("Data\\public\\shader\\FarMountain.fx")),	
			MAKESHADERSTRING(_T("Data\\public\\shader\\FarMountain.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\FarMountain.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_LIGHT_MAP_BAKE_DIRECTIONLIGHT,
			MAKESHADERSTRING(_T("Data\\public\\shader\\lightmapbakedirection.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\lightmapbakedirection.fx")),
			MAKESHADERSTRING(_T("Data\\public\\shader\\lightmapbakedirection.fx")),
			0,
			macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_SCALEOUTPUT,
				MAKESHADERSTRING(_T("Data\\public\\shader\\scaleoutput.fx")),
				MAKESHADERSTRING(_T("Data\\public\\shader\\scaleoutput.fx")),
				MAKESHADERSTRING(_T("Data\\public\\shader\\scaleoutput.fx")),
				0,
				macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_NORMAL_OUTPUT,
				MAKESHADERSTRING(_T("Data\\public\\shader\\NormalOutput.fx")),
				MAKESHADERSTRING(_T("Data\\public\\shader\\NormalOutput.fx")),
				MAKESHADERSTRING(_T("Data\\public\\shader\\NormalOutput.fx")),
				0,
				macroDefault, NULL,dwDefaultShaderCompileFlag,
		},	
		{
			DEFST_BILLBOARDCLOUD,
				MAKESHADERSTRING(_T("Data\\public\\shader\\BillboardCloud.fx")),
				MAKESHADERSTRING(_T("Data\\public\\shader\\BillboardCloud.fx")),
				MAKESHADERSTRING(_T("Data\\public\\shader\\BillboardCloud.fx")),
				0,
				macroDefault, NULL,dwDefaultShaderCompileFlag,
		},	
		{
			DEFST_LIGHT_MAP_BAKE_POINTLIGHT,
				MAKESHADERSTRING(_T("Data\\public\\shader\\lightmapbakepoint.fx")),
				MAKESHADERSTRING(_T("Data\\public\\shader\\lightmapbakepoint.fx")),
				MAKESHADERSTRING(_T("Data\\public\\shader\\lightmapbakepoint.fx")),
				0,
				macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
		{
			DEFST_LINEARDEPTHOUTPUT,
				MAKESHADERSTRING(_T("Data\\public\\shader\\LinearDepthoutput.fx")),
				MAKESHADERSTRING(_T("Data\\public\\shader\\LinearDepthoutput.fx")),
				MAKESHADERSTRING(_T("Data\\public\\shader\\LinearDepthoutput.fx")),
				0,
				macroDefault, NULL,dwDefaultShaderCompileFlag,
		},		
		{
			DEFST_LIGHT_MAP_BAKE_ENVLIGHT,
				MAKESHADERSTRING(_T("Data\\public\\shader\\lightmapbakeEnv.fx")),
				MAKESHADERSTRING(_T("Data\\public\\shader\\lightmapbakeEnv.fx")),
				MAKESHADERSTRING(_T("Data\\public\\shader\\lightmapbakeEnv.fx")),
				0,
				macroDefault, NULL,dwDefaultShaderCompileFlag,
		},
			//新的Shader加在这里，方法很简单，加一个DEFST_XX的值，然后填这个结构体就了，注意按enum的顺序填，不然会Assert
	};

	KG3DDefaultShaderGroup& ShaderGroup = g_GetDefaultShaderGroup();
	HRESULT hr = ShaderGroup.Init(Args, _countof(Args));
	_ASSERTE(SUCCEEDED(hr));

	/*添加NormalChecker。这些文件没有实际在引擎中使用，但是里面对几个重要的带NormalChecker功能的shader带宏定义进行了检查，是
	NormalChecker功能正常的重要保证
	*/
	KG3DNormalChekerShaderArg NormalCheckerArg[] = 
	{
		{
			DEFST_NEWLIGHT
			, MAKESHADERSTRING(_T("Data\\public\\shader\\new light mode_checker.fx")),
		},
		{
			DEFST_SKINMESH
			,MAKESHADERSTRING(_T("Data\\public\\shader\\shaderskin_2_checker.fx")),
		},
		{
			DEFST_NEWLIGHT_DETAIL
			, MAKESHADERSTRING(_T("Data\\public\\shader\\new light mode_detail_checker.fx")),
		},
		{
			DEFST_TERRAIN
			, MAKESHADERSTRING(_T("Data\\public\\shader\\TerrainRender_Checker.fx")),
		},	
	};

	hr = g_GetSceneCheckers().InitNormalCheckerShaders(NormalCheckerArg, _countof(NormalCheckerArg));

	(void)hr;
}

VOID KG3DShaderCollocation::CheckShaders()
{
	_ASSERTE(NULL != g_pd3dDevice);

	KG3DShaderHolder typeShaderHolder = g_GetDefaultShaderGroup().GetDefaultShader(DEFST_LIBRARY);
	//检查NormalChecker和Shader类型库。如果里面没有下面Cases里面指定的变量或者大小不一致，可以肯定任何的渲染问题都有可能出现了。
	{
		KG_PROCESS_ERROR(Private::IsShaderHolderInited(typeShaderHolder));

		struct CheckCase 
		{
			LPCTSTR strParamName;
			size_t	uSize;
		};

		//当更改Shader中的参数的时候，请相应检查struct大小的一一对应
		CheckCase checkCases[] = 
		{
			{_T("sunLight"), sizeof(KG3DLightParam)},
			{_T("playerLight"), sizeof(KG3DLightParam)},
			{_T("fogParam"), sizeof(KG3DFogParamShader)},
			{_T("materialCur"), sizeof(D3DMATERIAL9)},
			{_T("materialEx"), sizeof(KG3DShaderParamMaterialEx)},
			{_T("shadowMapParam"), sizeof(KG3DShaderParamShadowMap)},
			{_T("detailParam"), sizeof(KG3DShaderParamDetail)},
		};

		LPD3DXEFFECT pEffect = typeShaderHolder.GetEffect();
		_ASSERTE(NULL != pEffect);		

		for(size_t i = 0; i < _countof(checkCases); ++i) 
		{
			D3DXPARAMETER_DESC Desc;
			HRESULT hr = pEffect->GetParameterDesc(checkCases[i].strParamName, &Desc);
			if(FAILED(hr))
			{
				_ASSERTE("shader类型库检查失败");
				KGLogPrintf(KGLOG_WARNING, _T("Shader类型库以下变量错误或不存在，将导致其它shader编译失败：%s"), checkCases[i].strParamName);
			}
			KG_COM_PROCESS_ERROR(hr);

			BOOL bSizeMatch = (Desc.Bytes == checkCases[i].uSize);
			if(! bSizeMatch)
			{
				_ASSERTE("shader类型库检查失败");
				KGLogPrintf(KGLOG_WARNING, _T("Shader类型库以下变量大小和引擎中的struct大小不对应：%s"), checkCases[i].strParamName);
			}

			KG_PROCESS_ERROR(bSizeMatch);

		}while(false);


	}
	{
		_ASSERTE(! KG3DShaderRCBridge::GetShaderLibraryInterface()->IsLibraryBuildedInUnicode() && _T("shaderLibrary应该不是用Unicode编译的才对，这样才和引擎的CodeSet匹配"));
	}
	return;
Exit0:
	{
		KG3DShaderBuildParams buildParams;
		g_GetShaderManager().GetShaderBuildParams(typeShaderHolder, &buildParams);
		KGLogPrintf(KGLOG_WARNING, _T("Shader类型库不正确，将可能导致其它Shader编译失败：%s"), buildParams.strFilePathOrResourceIDString);
	}
}

VOID KG3DShaderCollocation::UnCollocate()
{
	KG3DShaderRCBridge::UninitLibrary();//即使没有Init也要调用，防止有静态资源的问题
}
