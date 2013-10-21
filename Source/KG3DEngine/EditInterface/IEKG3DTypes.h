////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : IEKG3DTypes.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-4-2 15:28:42
//  Comment     : 使用IE模板方式接口的步骤：
/*
1.在KG3DTypes中加一个Type
2.在这个文件中ADD一个模板
3.在KG3DEngineManager或者KG3DScene中写出Get
*/
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_IEKG3DTYPES_H_
#define _INCLUDE_IEKG3DTYPES_H_
#include "KG3DTypes.h"


//下面是用于编辑器的接口系统的
interface IEKG3DEnvEffect;

interface IEKG3DMeshSkyBox;
interface IEKG3DMeshFarMountain;
interface IEKG3DCloud;
interface IEKG3DSkySystem;
interface IEKG3DEnvEffFog;
interface IEKG3DMeshLensFlare;
interface IEKG3DEnvEffLight;//即IEKG3DLight
interface IEKG3DEnvEffLightSet;//即IEKG3DLightSet
interface IEKG3DEnvEffDWGC_KeyTable0;
interface IEKG3DEnvEffDynamicWeather;
interface IEKG3DEnvEffStarryNight;

interface IEKG3DEnvEffWind;
interface IEKG3DEnvironment;

interface IEKG3DRegionInfoManager;
interface IEKG3DRegionInfoManagerTable;

interface IEKG3DSceneSceneEditor;

interface IEKG3DSceneCameraMovement;
interface IEKG3DSceneCameraAnimation;

interface IEKG3DCommonContainer;
interface IEKG3DCommonObject;
interface IEKG3DSceneObjectPlacementTool;
interface IEKG3DSceneCheckers;

interface IEKG3DCollocator;

namespace IEKG3DTYPE
{
	///定义两个Dummy类
	struct KG3DNullTypeAbstruct 
	{
		virtual void DoSomeThing() = 0;
	};
	struct KG3DNullType : public KG3DNullTypeAbstruct
	{
		virtual void DoSomeThing(){}
	};

	///特化模板的基础模板
	template<DWORD dwTypeAbstruct, DWORD dwTypeSolid>//前面的是基类，后面的是派生的实际类
	struct TKG3DDWORD_TO_IEINTERFACE	//用两个DWORD，可以指定得到Interface
	{
		enum{bIsAbstract = false};
		typedef KG3DNullType Result;//不特化就无法使用
	};

	//第一个参数为NULL的特化
	template<DWORD dwTypeSolid>
	struct TKG3DDWORD_TO_IEINTERFACE<NULL, dwTypeSolid>
	{
		enum{bIsAbstract = false};
		typedef KG3DNullType Result;//不特化就无法使用
	};

	///用Interface，反过来得到两个DWORD
	template<typename _Interface>
	struct TKG3DIEINTERFACE_TO_DWORD;

	///全特化模板
#define _KG3D_ADD_AN_IETYPE(dwType, IEInterface, IsAbstract)	\
	template<>	\
	struct TKG3DDWORD_TO_IEINTERFACE<NULL, dwType>	\
	{	\
		enum	\
		{	\
			bIsAbstract = IsAbstract,	\
		};	\
		typedef IEInterface Result;	\
	};	\
	template<>	\
	struct TKG3DIEINTERFACE_TO_DWORD<IEInterface>	\
	{	\
		enum	\
		{	\
			bIsAbstract = IsAbstract,	\
		};	\
		enum{_type_dword1 = NULL, _type_dword2 = dwType,};	\
	}

//有的DWORD对应的是相同的接口，不能重复添加TKG3DIEINTERFACE_TO_DWORD
#define _KG3D_ADD_AN_IETYPE_NO_REVERSE(dwType, IEInterface, IsAbstract)	\
	template<>	\
	struct TKG3DDWORD_TO_IEINTERFACE<NULL, dwType>	\
	{	\
		enum	\
		{	\
			bIsAbstract = IsAbstract,	\
		};	\
		typedef IEInterface Result;	\
	}

#define  _KG3D_ADD_AN_IETYPE2(dwTypeTypeBase, dwType, IEInterface)	\
	template<>	\
	struct TKG3DDWORD_TO_IEINTERFACE<dwTypeTypeBase, dwType>	\
	{	\
		enum	\
		{	\
			bIsAbstract = false,	\
		};	\
		typedef IEInterface Result;	\
	};	\
	template<>	\
	struct TKG3DIEINTERFACE_TO_DWORD<IEInterface>	\
	{	\
		enum	\
		{	\
			bIsAbstract = false,	\
		};	\
		enum{_type_dword1 = dwTypeTypeBase, _type_dword2 = dwType,};	\
	}

	_KG3D_ADD_AN_IETYPE(KG3DTYPE_NONE_ABSTRUCT, KG3DNullTypeAbstruct, true);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_NONE, KG3DNullType, false);

	_KG3D_ADD_AN_IETYPE(KG3DTYPE_SCENE, IEKG3DScene, false);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_SCENE_SCENE_EDITOR, IEKG3DSceneSceneEditor, false);

	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENVIRONMENT, IEKG3DEnvironment, false);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENV_EFFECT, IEKG3DEnvEffect, true);///抽象类
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENVEFF_KEY_TABLE, IEKG3DEnvEffDWGC_KeyTable0, true);///抽象类
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENVEFF_SKYSYSTEM_SYSTEM, IEKG3DSkySystem, false);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENVEFF_SKYSYSTEM_SKYBOX, IEKG3DMeshSkyBox, false);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENVEFF_SKYSYSTEM_FARMOUNTAIN, IEKG3DMeshFarMountain, false);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENVEFF_SKYSYSTEM_CLOUD, IEKG3DCloud, false);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENVEFF_FOG, IEKG3DEnvEffFog, false);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENVEFF_LENSFLARE_SUN, IEKG3DMeshLensFlare, false);//注意这个和Moon是同一个接口，不能用TKG3DINTERFACE_TO_DWORD
	_KG3D_ADD_AN_IETYPE_NO_REVERSE(KG3DTYPE_ENVEFF_LENSFLARE_MOON, IEKG3DMeshLensFlare, false);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENVEFF_LIGHTSET, IEKG3DEnvEffLightSet, false);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENVEFF_LIGHT, IEKG3DEnvEffLight, false);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENVEFF_DYNAMIC_WEATHER, IEKG3DEnvEffDynamicWeather, false);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENVEFF_STARRY_NIGHT, IEKG3DEnvEffStarryNight, false);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_ENVEFF_WIND, IEKG3DEnvEffWind, false);

	_KG3D_ADD_AN_IETYPE(KG3DTYPE_REGION_INFO_MANAGER, IEKG3DRegionInfoManager, false);//用这个得到的是动态天气那个RIM
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_CAM_MOVEMENT, IEKG3DSceneCameraMovement, false);
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_CAM_ANIMATION, IEKG3DSceneCameraAnimation, false);

	_KG3D_ADD_AN_IETYPE(KG3DTYPE_SCENE_OBJECT_PLACEMENT_TOOL, IEKG3DSceneObjectPlacementTool, false);

	_KG3D_ADD_AN_IETYPE2(KG3DTYPE_RESOURCE_TABLE_TYPE, KG3DTYPE_REGION_INFO_MANAGER, IEKG3DRegionInfoManagerTable);

	_KG3D_ADD_AN_IETYPE(KG3DTYPE_COMMON_OBJECT_CONTAINER, IEKG3DCommonContainer, true);	//抽象类
	_KG3D_ADD_AN_IETYPE(KG3DTYPE_COMMON_OBJECT, IEKG3DCommonObject, true);	//抽象类

	_KG3D_ADD_AN_IETYPE(KG3DTYPE_SCENE_CHECKERS, IEKG3DSceneCheckers, false);

	_KG3D_ADD_AN_IETYPE(KG3DTYPE_COLLOCATOR, IEKG3DCollocator, false);
	
};


#endif //_INCLUDE_IEKG3DTYPES_H_
