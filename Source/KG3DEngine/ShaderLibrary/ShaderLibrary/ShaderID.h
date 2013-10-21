////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : ShaderID.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-8-5 10:42:27
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_SHADERID_H_
#define _INCLUDE_SHADERID_H_

////////////////////////////////////////////////////////////////////////////////


#define SHADER_RC_MIN_VALUE 113


#define SHADER_RC_AMBIENT_ONLY	113
#define SHADER_RC_AOE	114
#define SHADER_RC_BILLBOARDCLOUD	115
#define SHADER_RC_BLOOM	116
#define SHADER_RC_BOXFRAME	117
#define SHADER_RC_BOX_PROJ_EFFECT	118
#define SHADER_RC_BRUSH	119
#define SHADER_RC_CLOUD	120
#define SHADER_RC_COMMONPARAMS	121
#define SHADER_RC_DETAIL	122
#define SHADER_RC_DETAILS	123
#define SHADER_RC_DOF	124
#define SHADER_RC_FANFRAME	125
#define SHADER_RC_FARMOUNTAIN	126
#define SHADER_RC_GRASS	127
#define SHADER_RC_GRAY	128
#define SHADER_RC_HDR	129
#define SHADER_RC_LENSFLARE	130
#define SHADER_RC_LIBRARY	131
#define SHADER_RC_LIGHTMAP	132
#define SHADER_RC_LIGHTMAPBAKEDIRECTION	133
#define SHADER_RC_LIGHTMAPBAKEENV	134
#define SHADER_RC_LIGHTMAPBAKEPOINT	135
#define SHADER_RC_LIGHTMAPBAKE_INDIRECTION	136
#define SHADER_RC_LINEARDEPTHOUTPUT	137
#define SHADER_RC_MODELSHADOW	138
#define SHADER_RC_MULTITEXTURE	139
#define SHADER_RC_MULTITEXTURESKIN	140
#define SHADER_RC_MULTITEXTURESKIN_NORMALMAP	141
#define SHADER_RC_MULTITEXTURE_NORMALMAP	142
#define SHADER_RC_NEW_LIGHT_MODE_V3	143
#define SHADER_RC_NEW_LIGHT_MODE	144
#define SHADER_RC_NEW_LIGHT_MODE_CHECKER	145
#define SHADER_RC_NEW_LIGHT_MODE_DETAIL	146
#define SHADER_RC_NEW_LIGHT_MODE_DETAIL_2_0	147
#define SHADER_RC_NEW_LIGHT_MODE_DETAIL_CHECKER	148
#define SHADER_RC_NORMAL_MAP_WITHOUT_SKIN_BUT_WITH_EDGE	149
#define SHADER_RC_NORMAL_MAP_WITHOUT_SKIN	150
#define SHADER_RC_NORMALOUTPUT	151
#define SHADER_RC_OCEAN_SURFACE	152
#define SHADER_RC_POINTLIGHT	153
#define SHADER_RC_POSTRENDERFINALPASS	154
#define SHADER_RC_RAIN	155
#define SHADER_RC_RAIN	156
#define SHADER_RC_RAINENVIRONMENT	157
#define SHADER_RC_ROAD	158
#define SHADER_RC_ROAD_2_0	159
#define SHADER_RC_SCALEOUTPUT	160
#define SHADER_RC_SFX_EDITOR	161
#define SHADER_RC_SHADERBILLBOARD	162
#define SHADER_RC_SHADERDEFINITIONS	163
#define SHADER_RC_SHADERSKIN	164
#define SHADER_RC_SHADERSKIN_2	165
#define SHADER_RC_SHADERSKIN_2_CHECKER	166
#define SHADER_RC_SHADERSKIN_3	167
#define SHADER_RC_SHADERTYPEDEFINITIONS	168
#define SHADER_RC_SHADOWMAP	169
#define SHADER_RC_SIMPLECOLOR	170
#define SHADER_RC_SKYBOX	171
#define SHADER_RC_SPEEDTREE_V3	172
#define SHADER_RC_SPEEDTREE	173
#define SHADER_RC_SPEEDTREE_NORMALMAP	174
#define SHADER_RC_TERRAINGRASS_V3	175
#define SHADER_RC_TERRAINGRASS	176
#define SHADER_RC_TERRAINRENDER	177
#define SHADER_RC_TERRAINRENDER_2_0	178
#define SHADER_RC_TERRAINRENDER_CHECKER	179
#define SHADER_RC_TERRAINRENDER_NORMALMAP	180
#define SHADER_RC_TERRAINSHADOW	181
#define SHADER_RC_TEXFRAME	182
#define SHADER_RC_WATER	183
#define SHADER_RC_ZPOSTRENDER	184
//<InsertPoint>

#define SHADER_RC_TYPE				SHADER
#define SHADER_RC_TYPE_STRING		"SHADER"

#ifdef _IN_CPP_FILE_
struct  shaderRC
{
	DWORD uID;
	//LPCSTR	strResourceType;	//useless
	LPCSTR	strFilePath;	
};
#endif

#ifdef _IN_CPP_FILE_
static shaderRC g_shaderRCArray[] = 
{	
{SHADER_RC_AMBIENT_ONLY	,	"../../KG3DEngine/shaders/ambient only.fx"},
{SHADER_RC_AOE	,	"../../KG3DEngine/shaders/aoe.fx"},
{SHADER_RC_BILLBOARDCLOUD	,	"../../KG3DEngine/shaders/billboardcloud.fx"},
{SHADER_RC_BLOOM	,	"../../KG3DEngine/shaders/bloom.fx"},
{SHADER_RC_BOXFRAME	,	"../../KG3DEngine/shaders/boxframe.fx"},
{SHADER_RC_BOX_PROJ_EFFECT	,	"../../KG3DEngine/shaders/box_proj_effect.fx"},
{SHADER_RC_BRUSH	,	"../../KG3DEngine/shaders/brush.fx"},
{SHADER_RC_CLOUD	,	"../../KG3DEngine/shaders/cloud.fx"},
{SHADER_RC_COMMONPARAMS	,	"../../KG3DEngine/shaders/commonparams.h"},
{SHADER_RC_DETAIL	,	"../../KG3DEngine/shaders/detail.fx"},
{SHADER_RC_DETAILS	,	"../../KG3DEngine/shaders/details.h"},
{SHADER_RC_DOF	,	"../../KG3DEngine/shaders/dof.fx"},
{SHADER_RC_FANFRAME	,	"../../KG3DEngine/shaders/fanframe.fx"},
{SHADER_RC_FARMOUNTAIN	,	"../../KG3DEngine/shaders/farmountain.fx"},
{SHADER_RC_GRASS	,	"../../KG3DEngine/shaders/grass.fx"},
{SHADER_RC_GRAY	,	"../../KG3DEngine/shaders/gray.fx"},
{SHADER_RC_HDR	,	"../../KG3DEngine/shaders/hdr.fx"},
{SHADER_RC_LENSFLARE	,	"../../KG3DEngine/shaders/lensflare.fx"},
{SHADER_RC_LIBRARY	,	"../../KG3DEngine/shaders/library.fx"},
{SHADER_RC_LIGHTMAP	,	"../../KG3DEngine/shaders/lightmap.fx"},
{SHADER_RC_LIGHTMAPBAKEDIRECTION	,	"../../KG3DEngine/shaders/lightmapbakedirection.fx"},
{SHADER_RC_LIGHTMAPBAKEENV	,	"../../KG3DEngine/shaders/lightmapbakeenv.fx"},
{SHADER_RC_LIGHTMAPBAKEPOINT	,	"../../KG3DEngine/shaders/lightmapbakepoint.fx"},
{SHADER_RC_LIGHTMAPBAKE_INDIRECTION	,	"../../KG3DEngine/shaders/lightmapbake_indirection.fx"},
{SHADER_RC_LINEARDEPTHOUTPUT	,	"../../KG3DEngine/shaders/lineardepthoutput.fx"},
{SHADER_RC_MODELSHADOW	,	"../../KG3DEngine/shaders/modelshadow.fx"},
{SHADER_RC_MULTITEXTURE	,	"../../KG3DEngine/shaders/multitexture.fx"},
{SHADER_RC_MULTITEXTURESKIN	,	"../../KG3DEngine/shaders/multitextureskin.fx"},
{SHADER_RC_MULTITEXTURESKIN_NORMALMAP	,	"../../KG3DEngine/shaders/multitextureskin_normalmap.fx"},
{SHADER_RC_MULTITEXTURE_NORMALMAP	,	"../../KG3DEngine/shaders/multitexture_normalmap.fx"},
{SHADER_RC_NEW_LIGHT_MODE_V3	,	"../../KG3DEngine/shaders/new light mode v3.fx"},
{SHADER_RC_NEW_LIGHT_MODE	,	"../../KG3DEngine/shaders/new light mode.fx"},
{SHADER_RC_NEW_LIGHT_MODE_CHECKER	,	"../../KG3DEngine/shaders/new light mode_checker.fx"},
{SHADER_RC_NEW_LIGHT_MODE_DETAIL	,	"../../KG3DEngine/shaders/new light mode_detail.fx"},
{SHADER_RC_NEW_LIGHT_MODE_DETAIL_2_0	,	"../../KG3DEngine/shaders/new light mode_detail_2_0.fx"},
{SHADER_RC_NEW_LIGHT_MODE_DETAIL_CHECKER	,	"../../KG3DEngine/shaders/new light mode_detail_checker.fx"},
{SHADER_RC_NORMAL_MAP_WITHOUT_SKIN_BUT_WITH_EDGE	,	"../../KG3DEngine/shaders/normal map without skin but with edge.fx"},
{SHADER_RC_NORMAL_MAP_WITHOUT_SKIN	,	"../../KG3DEngine/shaders/normal map without skin.fx"},
{SHADER_RC_NORMALOUTPUT	,	"../../KG3DEngine/shaders/normaloutput.fx"},
{SHADER_RC_OCEAN_SURFACE	,	"../../KG3DEngine/shaders/ocean_surface.fx"},
{SHADER_RC_POINTLIGHT	,	"../../KG3DEngine/shaders/pointlight.h"},
{SHADER_RC_POSTRENDERFINALPASS	,	"../../KG3DEngine/shaders/postrenderfinalpass.fx"},
{SHADER_RC_RAIN	,	"../../KG3DEngine/shaders/rain.fx"},
{SHADER_RC_RAIN	,	"../../KG3DEngine/shaders/rain.h"},
{SHADER_RC_RAINENVIRONMENT	,	"../../KG3DEngine/shaders/rainenvironment.h"},
{SHADER_RC_ROAD	,	"../../KG3DEngine/shaders/road.fx"},
{SHADER_RC_ROAD_2_0	,	"../../KG3DEngine/shaders/road_2_0.fx"},
{SHADER_RC_SCALEOUTPUT	,	"../../KG3DEngine/shaders/scaleoutput.fx"},
{SHADER_RC_SFX_EDITOR	,	"../../KG3DEngine/shaders/sfx_editor.fx"},
{SHADER_RC_SHADERBILLBOARD	,	"../../KG3DEngine/shaders/shaderbillboard.fx"},
{SHADER_RC_SHADERDEFINITIONS	,	"../../KG3DEngine/shaders/shaderdefinitions.h"},
{SHADER_RC_SHADERSKIN	,	"../../KG3DEngine/shaders/shaderskin.fx"},
{SHADER_RC_SHADERSKIN_2	,	"../../KG3DEngine/shaders/shaderskin_2.fx"},
{SHADER_RC_SHADERSKIN_2_CHECKER	,	"../../KG3DEngine/shaders/shaderskin_2_checker.fx"},
{SHADER_RC_SHADERSKIN_3	,	"../../KG3DEngine/shaders/shaderskin_3.fx"},
{SHADER_RC_SHADERTYPEDEFINITIONS	,	"../../KG3DEngine/shaders/shadertypedefinitions.h"},
{SHADER_RC_SHADOWMAP	,	"../../KG3DEngine/shaders/shadowmap.h"},
{SHADER_RC_SIMPLECOLOR	,	"../../KG3DEngine/shaders/simplecolor.fx"},
{SHADER_RC_SKYBOX	,	"../../KG3DEngine/shaders/skybox.fx"},
{SHADER_RC_SPEEDTREE_V3	,	"../../KG3DEngine/shaders/speedtree v3.fx"},
{SHADER_RC_SPEEDTREE	,	"../../KG3DEngine/shaders/speedtree.fx"},
{SHADER_RC_SPEEDTREE_NORMALMAP	,	"../../KG3DEngine/shaders/speedtree_normalmap.fx"},
{SHADER_RC_TERRAINGRASS_V3	,	"../../KG3DEngine/shaders/terraingrass v3.fx"},
{SHADER_RC_TERRAINGRASS	,	"../../KG3DEngine/shaders/terraingrass.fx"},
{SHADER_RC_TERRAINRENDER	,	"../../KG3DEngine/shaders/terrainrender.fx"},
{SHADER_RC_TERRAINRENDER_2_0	,	"../../KG3DEngine/shaders/terrainrender_2_0.fx"},
{SHADER_RC_TERRAINRENDER_CHECKER	,	"../../KG3DEngine/shaders/terrainrender_checker.fx"},
{SHADER_RC_TERRAINRENDER_NORMALMAP	,	"../../KG3DEngine/shaders/terrainrender_normalmap.fx"},
{SHADER_RC_TERRAINSHADOW	,	"../../KG3DEngine/shaders/terrainshadow.fx"},
{SHADER_RC_TEXFRAME	,	"../../KG3DEngine/shaders/texframe.fx"},
{SHADER_RC_WATER	,	"../../KG3DEngine/shaders/water.fx"},
{SHADER_RC_ZPOSTRENDER	,	"../../KG3DEngine/shaders/zpostrender.fx"},
//<InsertPoint>
	{0, NULL},
};
#endif

#endif //_INCLUDE_SHADERID_H_