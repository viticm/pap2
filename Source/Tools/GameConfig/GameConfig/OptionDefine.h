#ifndef _OPTION_DEFINE_H_
#define _OPTION_DEFINE_H_

#include <d3d9.h>

struct _RESOLUTION
{
	TCHAR czName[32];
	unsigned uWidth;
	unsigned uHeight;
};

static _RESOLUTION gs_ResolutionPresets[] =
{
	{_T("640 x 480"),  640,  480},
	{_T("720 x 480"),  720,  480},
	{_T("720 x 576"),  720,  576},
	{_T("800 x 600"),  800,  600},
	{_T("1024 x 768"),  1024, 768},
	{_T("1152 x 720"),  1152, 720},
	{_T("1152 x 864"),  1152, 864},
	{_T("1280 x 720"),  1280, 720},
	{_T("1280 x 768"),  1280, 768},
	{_T("1280 x 800"),  1280, 800},
	{_T("1280 x 960"),  1280, 960},
	{_T("1280 x 1024"), 1280, 1024},
	{_T("1360 x 768"), 1360, 768},
	{_T("1440 x 900"),  1440, 900},
	{_T("1600 x 1200"), 1600, 1200},
	{_T("1600 x 1024"), 1600, 1024},
	{_T("1680 x 1050"), 1680, 1050},
	{_T("1920 x 1080"), 1920, 1080},
	{_T("1920 x 1200"), 1920, 1200}
};

struct _MUTISAMPLETYPE
{
	TCHAR czName[32];
	D3DMULTISAMPLE_TYPE eType;
};

static _MUTISAMPLETYPE gs_MutiSampleTypePresets[] =
{
	{_T("无"),      D3DMULTISAMPLE_NONE},
	{_T("低(2X)"),  D3DMULTISAMPLE_2_SAMPLES},
	{_T("中(4X)"),  D3DMULTISAMPLE_4_SAMPLES},
	{_T("高(8X)"),  D3DMULTISAMPLE_8_SAMPLES}
};

struct _TEXTURE_FILTER
{
	TCHAR czName[32];
	D3DTEXTUREFILTERTYPE  eType;
};
static _TEXTURE_FILTER gs_TextureFilterPresets[] =
{
	{_T("最近点采样"),       D3DTEXF_POINT},
	{_T("线性纹理过滤"),     D3DTEXF_LINEAR},
	{_T("各向异性纹理过滤"), D3DTEXF_ANISOTROPIC}
};

struct _MODEL_SHADOW_TYPE
{
	TCHAR czName[10];
	int nValue;
};

static _MODEL_SHADOW_TYPE gs_ShadowOption[] =
{
	{_T("无"), 0},
	{_T("低"), 1},
	{_T("中"), 2},
	{_T("高"), 3}
};

const  unsigned RESOLUTION_PRESETS_COUNT = sizeof(gs_ResolutionPresets) / sizeof(gs_ResolutionPresets[0]);
static _RESOLUTION gs_SupportedResolution[RESOLUTION_PRESETS_COUNT + 1]; 
static  unsigned gs_uSupportedResolutionCount = 0;

const  unsigned MUTISAMPLE_PRESETS_COUNT = sizeof(gs_MutiSampleTypePresets) / sizeof(gs_MutiSampleTypePresets[0]);
static _MUTISAMPLETYPE gs_SupportedMutiSampleType[MUTISAMPLE_PRESETS_COUNT + 1];
static unsigned gs_uSupportedMutiSampleTypeCount = 0;

const  unsigned TEXTURE_FILTER_PRESETS_COUNT = sizeof(gs_TextureFilterPresets) / sizeof(gs_TextureFilterPresets[0]);
// const  unsigned TEXTURE_SCALE_PRESETS_COUNT = sizeof(gs_TextureScalePresets) / sizeof(gs_TextureScalePresets[0]);

const unsigned MODEL_SHADOW_TYPE = sizeof(gs_ShadowOption) / sizeof(gs_ShadowOption[0]);


#endif /* _OPTION_DEFINE_H_ */