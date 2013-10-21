#ifndef _CONFIG_DEFINE_H_
#define _CONFIG_DEFINE_H_


//***********************config.ini*************************
CONST TCHAR CONFIG_FILE_NAME[]                    = _T("config.ini");

//**************config.ini*****SECTION**********************
CONST TCHAR CONFIG_SECTION_KG3DENGINE[]           = _T("KG3DENGINE");
CONST TCHAR CONFIG_SECTION_ENGINEOPTION[]         = _T("ENGINEOPTION");
CONST TCHAR CONFIG_SECTION_MAIN[]                 = _T("Main");
CONST TCHAR CONFIG_SECTION_SOUND[]                = _T("SoundSetting");

//**********config.ini****SECTION****KG3DENGIN***KEY********
CONST TCHAR CONFIG_KEY_MULTISAMPLETYPE[]          = _T("MultiSampleType");
CONST TCHAR CONFIG_KEY_SAMPMIPFILTER[]            = _T("SampMipFilter");
CONST TCHAR CONFIG_KEY_CAMMERAdISTANCE[]          = _T("CammeraDistance");
CONST TCHAR CONFIG_KEY_WATERREFRACTION[]          = _T("WaterRefraction");
CONST TCHAR CONFIG_KEY_RENDERGRASS[]              = _T("RenderGrass"); //low and mid and high must be 1
CONST TCHAR CONFIG_KEY_BGODRAY[]                  = _T("bGodRay");
CONST TCHAR CONFIG_KEY_BLOOMENABLE[]              = _T("BloomEnable");
CONST TCHAR CONFIG_KEY_SHOCKWAVEENABLE[]          = _T("ShockWaveEnable");
CONST TCHAR CONFIG_KEY_MODELSHADOWTYPE[]          = _T("ModelShadowType");
CONST TCHAR CONFIG_KEY_SAMPMINFILETER[]           = _T("SampMinFilter");
CONST TCHAR CONFIG_KEY_SAMPMAGFILTER[]            = _T("SampMagFilter");
CONST TCHAR CONFIG_KEY_TEXTURESCALE[]             = _T("TextureScale");
CONST TCHAR CONFIG_KEY_WATERREFLECTION[]          = _T("WaterReflection");
CONST TCHAR CONFIG_KEY_WATERREFLECTENVIRONMENT[]  = _T("WaterReflectEnvironment");
CONST TCHAR CONFIG_KEY_WATERREFLECTGROUND[]       = _T("WaterReflectGround");
CONST TCHAR CONFIG_KEY_WATERREFLECTNPC[]          = _T("WaterReflectNPC");
CONST TCHAR CONFIG_KEY_WATERREFLECTENTITY[]       = _T("WaterReflectEntity");
CONST TCHAR CONFIG_KEY_POSTRENDEREFFECT[]         = _T("PostRenderEffect");
CONST TCHAR CONFIG_KEY_ENABLESHADOWMAP[]          = _T("EnableShadowMap");
CONST TCHAR CONFIG_KEY_BENABLEAUTORELOADTEXTURE[] = _T("bEnableAutoReLoadTexture"); //low and mid and high must be 0
CONST TCHAR CONFIG_KEY_GRASSANIMATION[]           = _T("GrassAnimation");  //low must be 0
CONST TCHAR CONFIG_KEY_FORCESHADERMODEL[]         = _T("ForceShaderModel"); //low must be 2,mid and high is X
CONST TCHAR CONFIG_KEY_VEGETATIONDENSITY[]        = _T("VegetationDensity"); //LOW must be 3, mid and high is 1;

CONST TCHAR CONFIG_KEY_WATERDETAIL[]              = _T("WaterDetail"); //水面精度
CONST TCHAR CONFIG_KEY_TERRAINDETAIL[]            = _T("TerrainDetail"); //地形精度

CONST TCHAR CONFIG_KEY_3D_VISION[]                = _T("b3DVision");    // 3D眼镜模式

//**********config.ini****SECTION****MAIN***KEY********
CONST TCHAR CONFIG_KEY_CANVASWIDTH[]              = _T("CanvasWidth");
CONST TCHAR CONFIG_KEY_CANVASHEIGHT[]             = _T("CanvasHeight");
CONST TCHAR CONFIG_KEY_FULLSCREEN[]               = _T("FullScreen");
CONST TCHAR CONFIG_KEY_PANAUISION[]               = _T("Panauision");
CONST TCHAR CONFIG_KEY_EXCLUSIVE_MODE[]           = _T("ExclusiveMode");

//**********config.ini****SECTION****SOUND***KEY********
CONST TCHAR CONFIG_KEY_SILENCE[]                  = _T("Silence");
//**********config.ini***struct*************************

enum OPTION_GROUP
{
	OPTION_GROUP_LOW,
	OPTION_GROUP_MID,
	OPTION_GROUP_HIGH,
	OPTION_GROUP_CUSTOM
};

typedef struct _tagDIALOG_ITEM_VALUE_GROUP
{
 	int nRadioLow;
	int nRadioMiddle;
 	int nRadioHigh;
 	int nRadioCustom;

	int nResultionIndex;
	int nMutisampleTypeIndex;
	int nTextureFilterIndex;
	int nEnableRenderGrass;
	int nEnableWaterRefraction;
	int nEnableWaterReflection;
	int nEnableGodRay;
	int nEnableBloom;
	int nEnableShockWave;
	int nCammeraDistance;
	int nSilence;

	int nFullScreenFlag;
    int nExclusiveFlag;
    int nPanauisionFlag;
    int n3DVisionFlag;

	int nModelShadowType; //阴影选项
	int nWaterDetail;     //水面精度
	int nTerrainDetail;   //地形精度
	int nTextureScale;    //贴图精度

	int nGrassAnimation;
	int nVegetationDensity;
	TCHAR szTerrainDetail[2];

}DIALOG_ITEM_VALUE_GROUP, *PDIALOG_ITEM_VALUE_GROUP;

#endif  /*_CONFIG_DEFINE_H_*/
