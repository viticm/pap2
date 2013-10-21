////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEngineTypes.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-7-23 15:10:47
//  Comment     : 凡是需要在引擎内部共享又不能放到接口的东东就放这里 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DENGINETYPES_H_
#define _INCLUDE_KG3DENGINETYPES_H_

////////////////////////////////////////////////////////////////////////////////
enum KG3DDefaultShaderType
{
	DEFST_LIBRARY = 0,	//尽量不要改它们之间的顺序
	DEFST_SKINMESH,
	DEFST_POST_RENDER,
	DEFST_NEWLIGHT,
	DEFST_NORMAL_NOSKIN,
	DEFST_MODEL_SHADOW,
	DEFST_LIGHT_MAP,		//计算物件贴图用的Shader
	DEFST_CLOUD,
	DEFST_NEWLIGHT_DETAIL,
	DEFST_TERRAIN,
	DEFST_TERRAIN_NORMALMAP,
	DEFST_GRASS,
	DEFST_LENSFLARE,
	DEFST_TERRAIN_ROAD,
    DEFST_SFX_BOX_PROJ,
	DEFST_BILLBOARD,
	DEFST_HIGH_GRASS,
	DEFST_SPEED_TREE,
	DEFST_WATER,
	DEFST_SKYBOX,
	DEFST_FARMOUNTAIN,
	DEFST_LIGHT_MAP_BAKE_DIRECTIONLIGHT,//光照图烘焙用SHADER
	DEFST_SCALEOUTPUT,//缩放输出
	DEFST_NORMAL_OUTPUT,//输出模型的NORMAL，模型空间的 
	DEFST_BILLBOARDCLOUD,//公告板云的绘制SHADER
	DEFST_LIGHT_MAP_BAKE_POINTLIGHT,//光照图烘焙用SHADER
	DEFST_LINEARDEPTHOUTPUT,//线性深度转化
	DEFST_LIGHT_MAP_BAKE_ENVLIGHT,//光照图烘焙用SHADER
	DEFST_COUNT,			//加新的Shader的时候，在这个前面添加
	DEFST_FORCE_DWORD = 0xffffffff,
};

#endif //_INCLUDE_KG3DENGINETYPES_H_
