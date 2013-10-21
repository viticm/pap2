////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_OptionDialog.cpp
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2008-8-8 14:52:16
//  Comment     :
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG_OptionDialog.h"
#include <commctrl.h>
#include <cuda_runtime_api.h> 
#include <cuda_d3d9_interop.h>
#include "KG_Benchmark.h"

struct _RESOLUTION
{
    char czName[32];
    unsigned uWidth;
    unsigned uHeight;
    int nFullScreen;
};
static _RESOLUTION gs_ResolutionPresets[] =
{
    //{"800  x 600",  800,  600 , 0},
    //{"1024 x 768",  1024, 768 , 0},
    {"1280 x 1024", 1280, 1024, 0},
    {"1440 x 900",  1440, 900 , 0},
    {"1600 x 1200", 1600, 1200, 0},
    {"1680 x 1050", 1680, 1050, 0},
    {"1600 x 1024", 1600, 1024, 0},
    {"1920 x 1200", 1920, 1200, 0},
	{"2560 x 1600", 2560, 1600, 0}
};
const unsigned RESOLUTION_PRESETS_COUNT = sizeof(gs_ResolutionPresets) / sizeof(gs_ResolutionPresets[0]);
static _RESOLUTION gs_SupportedResolution[RESOLUTION_PRESETS_COUNT + 1]; // one for user's current resolution not in presets
static unsigned gs_uSupportedResolutionCount = 0;

struct _MUTISAMPLETYPE
{
	char czName[32];
	D3DMULTISAMPLE_TYPE eType;
};
static _MUTISAMPLETYPE gs_MutiSampleTypePresets[] =
{
	{"OFF",      D3DMULTISAMPLE_NONE},
	//{"低(2X)",  D3DMULTISAMPLE_2_SAMPLES},
	{"ON(4X)",  D3DMULTISAMPLE_4_SAMPLES},
	//{"高(8X)",  D3DMULTISAMPLE_8_SAMPLES}
};
const unsigned MUTISAMPLE_PRESETS_COUNT = sizeof(gs_MutiSampleTypePresets) / sizeof(gs_MutiSampleTypePresets[0]);
static _MUTISAMPLETYPE gs_SupportedMutiSampleType[MUTISAMPLE_PRESETS_COUNT + 1];
static unsigned gs_uSupportedMutiSampleTypeCount = 0;

struct _TEXTURE_FILTER
{
    char czName[32];
    D3DTEXTUREFILTERTYPE  eType;

};
static _TEXTURE_FILTER gs_TextureFilterPresets[] =
{
    {"最近点采样",       D3DTEXF_POINT},
    {"线性纹理过滤",     D3DTEXF_LINEAR},
    {"各向异性纹理过滤", D3DTEXF_ANISOTROPIC}
};
const unsigned TEXTURE_FILTER_PRESETS_COUNT = sizeof(gs_TextureFilterPresets) / sizeof(gs_TextureFilterPresets[0]);

struct _SHADOW_TYPE
{
	char cszName[32];
	int nType;
};
static _SHADOW_TYPE gs_ShadowTypePresets[] =
{
	{"Low",   1},
	{"Middle",2},
	{"High",  3}
};
const unsigned SHADOW_TYPE_PRESETS_COUNT = sizeof(gs_ShadowTypePresets) / sizeof(gs_ShadowTypePresets[0]);

struct _POSTEFFECT_TYPE
{
	char cszName[32];
	BOOL nType;
};
static _POSTEFFECT_TYPE gs_PostEffectType[]=
{
	{"Off",FALSE},
	{"On", TRUE}
};
const unsigned POSTEFFECT_TYPE_PRESETS_COUNT = sizeof(gs_PostEffectType) / sizeof(gs_PostEffectType[0]);





////////////////////////////////////////////////////////////////////////////////
struct DIALOG_ITEM_VALUE_GROUP
{
    //int nPresetOptionGroupLow;
    //int nPresetOptionGroupMiddle;
    //int nPresetOptionGroupHigh;
    //int nPresetOptionGroupCustom;

    //int nResultionIndex;
    //int nMutisampleTypeIndex;
    //int nTextureFilterIndex;
    //int nEnableRenderGrass;
    //int nEnableWaterRefraction;
    //int nEnableWaterReflection;
    //int nEnableGodRay;
    //int nEnableBloom;
    //int nEnableShockWave;
    //int nEnableSCM;
    //float fCammeraDistance;
	int nResultionIndex;
	int nMutisampleTypeIndex;
	int nShadowTypeIndex;
	int nPostEffectIndex;
	int nEnableCudaTest; //0 disenable 1 enable 2 grayed
	BOOL bEnableNormalTest;
};

////////////////////////////////////////////////////////////////////////////////
int _GetDlgItemValue(HWND hDlg, DIALOG_ITEM_VALUE_GROUP *pOption)
{
    ASSERT(pOption);

    //pOption->nPresetOptionGroupLow    = IsDlgButtonChecked(hDlg, IDC_PRESET_OPTION_GROUP_LOW);
    //ASSERT(pOption->nPresetOptionGroupLow != BST_INDETERMINATE);

    //pOption->nPresetOptionGroupMiddle = IsDlgButtonChecked(hDlg, IDC_PRESET_OPTION_GROUP_MIDDLE);
    //ASSERT(pOption->nPresetOptionGroupMiddle != BST_INDETERMINATE);

    //pOption->nPresetOptionGroupHigh   = IsDlgButtonChecked(hDlg, IDC_PRESET_OPTION_GROUP_HIGH);
    //ASSERT(pOption->nPresetOptionGroupHigh != BST_INDETERMINATE);

    //pOption->nPresetOptionGroupCustom = IsDlgButtonChecked(hDlg, IDC_PRESET_OPTION_GROUP_CUSTOM);
    //ASSERT(pOption->nPresetOptionGroupCustom != BST_INDETERMINATE);

    pOption->nResultionIndex = (int)SendDlgItemMessage(hDlg, IDC_RESULTION, CB_GETCURSEL, 0, 0);
    ASSERT(pOption->nResultionIndex != CB_ERR);

    pOption->nMutisampleTypeIndex = (int)SendDlgItemMessage(hDlg, IDC_MSAA, CB_GETCURSEL, 0, 0);
    ASSERT(pOption->nMutisampleTypeIndex != CB_ERR);

	pOption->nPostEffectIndex = (int)SendDlgItemMessage(hDlg, IDC_POSTEFFECT, CB_GETCURSEL, 0, 0);
	ASSERT(pOption->nPostEffectIndex != CB_ERR);

	pOption->nShadowTypeIndex = (int)SendDlgItemMessage(hDlg, IDC_SHADOW, CB_GETCURSEL, 0, 0);
	ASSERT(pOption->nShadowTypeIndex != CB_ERR);

	pOption->bEnableNormalTest = (BST_CHECKED == SendDlgItemMessage(hDlg, IDC_NORMALTEST, BM_GETCHECK,0,0));

	pOption->nEnableCudaTest = 0;
	HWND hCheckCudaTest = GetDlgItem(hDlg,IDC_CUDATEST);
	if (IsWindowEnabled(hCheckCudaTest))
	{
		if (BST_CHECKED == SendDlgItemMessage(hDlg, IDC_CUDATEST, BM_GETCHECK,0,0))
		{
			pOption->nEnableCudaTest = 1;
		}
		else
		{
			pOption->nEnableCudaTest = 0;
		}
	}
	else
	{
		pOption->nEnableCudaTest = 2;
	}

    //pOption->nTextureFilterIndex = (int)SendDlgItemMessage(hDlg, IDC_TEXTURE_FILTER, CB_GETCURSEL, 0, 0);
    //ASSERT(pOption->nTextureFilterIndex != CB_ERR);

    //pOption->fCammeraDistance = (float)SendDlgItemMessage(hDlg, IDC_CAMMERA_DISTANCE, TBM_GETPOS, 0, 0);
    //ASSERT(pOption->fCammeraDistance > 0);

    //pOption->nEnableRenderGrass = IsDlgButtonChecked(hDlg, IDC_ENABLE_RENDER_GRASS);
    //ASSERT(pOption->nEnableRenderGrass != BST_INDETERMINATE);

    //pOption->nEnableWaterRefraction = IsDlgButtonChecked(hDlg, IDC_ENABLE_WATER_REFRACTION);
    //ASSERT(pOption->nEnableWaterRefraction != BST_INDETERMINATE);

    //pOption->nEnableWaterReflection = IsDlgButtonChecked(hDlg, IDC_ENABLE_WATER_REFLECTION);
    //ASSERT(pOption->nEnableWaterReflection != BST_INDETERMINATE);

    //pOption->nEnableGodRay = IsDlgButtonChecked(hDlg, IDC_ENABLE_GODRAY);
    //ASSERT(pOption->nEnableGodRay != BST_INDETERMINATE);

    //pOption->nEnableBloom = IsDlgButtonChecked(hDlg, IDC_ENABLE_BLOOM);
    //ASSERT(pOption->nEnableBloom != BST_INDETERMINATE);

    //pOption->nEnableShockWave = IsDlgButtonChecked(hDlg, IDC_ENABLE_SHOCKWAVE);
    //ASSERT(pOption->nEnableShockWave != BST_INDETERMINATE);

    //pOption->nEnableSCM = IsDlgButtonChecked(hDlg, IDC_ENABLE_CSM);
    //ASSERT(pOption->nEnableSCM != BST_INDETERMINATE);

    return true;
}

////////////////////////////////////////////////////////////////////////////////
int _SetDlgItemValue(HWND hDlg, const DIALOG_ITEM_VALUE_GROUP &crOption)
{
    int nRetCode = 0;

    //nRetCode = CheckDlgButton(hDlg, IDC_PRESET_OPTION_GROUP_LOW, crOption.nPresetOptionGroupLow);
    //ASSERT(nRetCode);
    //nRetCode = CheckDlgButton(hDlg, IDC_PRESET_OPTION_GROUP_MIDDLE, crOption.nPresetOptionGroupMiddle);
    //ASSERT(nRetCode);
    //nRetCode = CheckDlgButton(hDlg, IDC_PRESET_OPTION_GROUP_HIGH, crOption.nPresetOptionGroupHigh);
    //ASSERT(nRetCode);
    //nRetCode = CheckDlgButton(hDlg, IDC_PRESET_OPTION_GROUP_CUSTOM, crOption.nPresetOptionGroupCustom);
    //ASSERT(nRetCode);

    nRetCode = (int)SendDlgItemMessage(hDlg, IDC_RESULTION, CB_SETCURSEL, crOption.nResultionIndex, 0);
    ASSERT(nRetCode != CB_ERR);

    nRetCode = (int)SendDlgItemMessage(hDlg, IDC_MSAA, CB_SETCURSEL, crOption.nMutisampleTypeIndex, 0);
    ASSERT(nRetCode != CB_ERR);

    nRetCode = (int)SendDlgItemMessage(hDlg, IDC_POSTEFFECT, CB_SETCURSEL, crOption.nPostEffectIndex, 0);
    ASSERT(nRetCode != CB_ERR);

	nRetCode = (int)SendDlgItemMessage(hDlg, IDC_SHADOW, CB_SETCURSEL, crOption.nShadowTypeIndex, 0);
	ASSERT(nRetCode != CB_ERR);

	if (crOption.bEnableNormalTest)
	{
		nRetCode = (int)SendDlgItemMessage(hDlg, IDC_NORMALTEST, BM_SETCHECK, BST_CHECKED, 0);
	}
	else
	{
		nRetCode = (int)SendDlgItemMessage(hDlg, IDC_NORMALTEST, BM_SETCHECK, BST_UNCHECKED, 0);
	}

	if (crOption.nEnableCudaTest == 2)
	{
		HWND hCheckCudaTest = GetDlgItem(hDlg,IDC_CUDATEST);
		EnableWindow(hCheckCudaTest,FALSE);
	}
	else if (crOption.nEnableCudaTest == 1)
	{
		nRetCode = (int)SendDlgItemMessage(hDlg, IDC_CUDATEST, BM_SETCHECK, BST_CHECKED, 0);
	}
	else if (crOption.nEnableCudaTest == 0)
	{
		nRetCode = (int)SendDlgItemMessage(hDlg, IDC_CUDATEST, BM_SETCHECK, BST_UNCHECKED, 0);
	}



  
    //nRetCode = (int)SendDlgItemMessage(hDlg, IDC_CAMMERA_DISTANCE, TBM_SETPOS, TRUE, (LONG)crOption.fCammeraDistance);
    //ASSERT(nRetCode); // No return value for this message

    //nRetCode = CheckDlgButton(hDlg, IDC_ENABLE_RENDER_GRASS, crOption.nEnableRenderGrass);
    //ASSERT(nRetCode);
    //nRetCode = CheckDlgButton(hDlg, IDC_ENABLE_WATER_REFRACTION, crOption.nEnableWaterRefraction);
    //ASSERT(nRetCode);
    //nRetCode = CheckDlgButton(hDlg, IDC_ENABLE_WATER_REFLECTION, crOption.nEnableWaterReflection);
    //ASSERT(nRetCode);
    //nRetCode = CheckDlgButton(hDlg, IDC_ENABLE_GODRAY, crOption.nEnableGodRay);
    //ASSERT(nRetCode);
    //nRetCode = CheckDlgButton(hDlg, IDC_ENABLE_BLOOM, crOption.nEnableBloom);
    //ASSERT(nRetCode);
    //nRetCode = CheckDlgButton(hDlg, IDC_ENABLE_SHOCKWAVE, crOption.nEnableShockWave);
    //ASSERT(nRetCode);
    //nRetCode = CheckDlgButton(hDlg, IDC_ENABLE_CSM, crOption.nEnableSCM);
    //ASSERT(nRetCode);

    return true;
}
////////////////////////////////////////////////////////////////////////////////
int _ApplayPresetOption(HWND hDlg/*, int nPresetOptionGroupID*/)
{
    int nResult = false;
    DIALOG_ITEM_VALUE_GROUP Option = { 0 };
    KSmartIniFile pFile;
    const char CONFIG_FILE_NAME[] = "config.ini";
    unsigned i = 0;
    D3DMULTISAMPLE_TYPE  eMultiSampleType   = D3DMULTISAMPLE_NONE;
	BOOL bPostEffect = FALSE;
	int nShadow = 1;
    //D3DTEXTUREFILTERTYPE eTextureFilterType = D3DTEXF_NONE;
    //int nTextureScale = 1;

    //switch (nPresetOptionGroupID)
    //{
    //case IDC_PRESET_OPTION_GROUP_LOW:
    //    Option.nPresetOptionGroupLow    = true;
    //    Option.nPresetOptionGroupMiddle = false;
    //    Option.nPresetOptionGroupHigh   = false;
    //    Option.nPresetOptionGroupCustom = false;

    //    //Option.nResultionIndex  = 0;
    //    //Option.nMutisampleTypeIndex = 0;
    //    eMultiSampleType = D3DMULTISAMPLE_NONE;
    //    //Option.nSampFilterIndex = 0;
    //    eTextureFilterType = D3DTEXF_LINEAR;
    //    //Option.nTextureScaleIndex = 0;
    //    nTextureScale = 4;

    //    Option.fCammeraDistance = 20000.0;
    //    Option.nEnableRenderGrass = false;
    //    Option.nEnableWaterRefraction = false;
    //    Option.nEnableWaterReflection = false;
    //    Option.nEnableGodRay = false;
    //    Option.nEnableBloom = false;
    //    Option.nEnableShockWave = false;
    //    Option.nEnableSCM = false;
    //	break;

    //case IDC_PRESET_OPTION_GROUP_MIDDLE:
    //    Option.nPresetOptionGroupLow    = false;
    //    Option.nPresetOptionGroupMiddle = true;
    //    Option.nPresetOptionGroupHigh   = false;
    //    Option.nPresetOptionGroupCustom = false;

    //    //Option.nResultionIndex  = 0;
    //    //Option.nMutisampleTypeIndex = 0;
    //    eMultiSampleType = D3DMULTISAMPLE_NONE;//D3DMULTISAMPLE_2_SAMPLES;
    //    //Option.nSampFilterIndex = 0;
    //    eTextureFilterType = D3DTEXF_LINEAR;
    //    //Option.nTextureScaleIndex = 0;
    //    nTextureScale = 2;

    //    Option.fCammeraDistance = 40000.0;
    //    Option.nEnableRenderGrass = true;
    //    Option.nEnableWaterRefraction = true;
    //    Option.nEnableWaterReflection = true;
    //    Option.nEnableGodRay = false;
    //    Option.nEnableBloom = false;
    //    Option.nEnableShockWave = false;
    //    Option.nEnableSCM = false;
    //    break;

    //case IDC_PRESET_OPTION_GROUP_HIGH:
    //    Option.nPresetOptionGroupLow    = false;
    //    Option.nPresetOptionGroupMiddle = false;
    //    Option.nPresetOptionGroupHigh   = true;
    //    Option.nPresetOptionGroupCustom = false;

    //    //Option.nResultionIndex  = 0;
    //    //Option.nMutisampleTypeIndex = 0;
    //    eMultiSampleType = D3DMULTISAMPLE_NONE;//D3DMULTISAMPLE_8_SAMPLES;
    //    //Option.nSampFilterIndex = 0;
    //    eTextureFilterType = D3DTEXF_ANISOTROPIC;
    //    //Option.nTextureScaleIndex = 0;
    //    nTextureScale = 1;

    //    Option.fCammeraDistance = 80000.0;
    //    Option.nEnableRenderGrass = true;
    //    Option.nEnableWaterRefraction = true;
    //    Option.nEnableWaterReflection = true;
    //    Option.nEnableGodRay = true;
    //    Option.nEnableBloom = true;
    //    Option.nEnableShockWave = true;
    //    Option.nEnableSCM = true;
    //    break;

    //case IDC_PRESET_OPTION_GROUP_CUSTOM:
    //    Option.nPresetOptionGroupLow    = false;
    //    Option.nPresetOptionGroupMiddle = false;
    //    Option.nPresetOptionGroupHigh   = false;
    //    Option.nPresetOptionGroupCustom = true;


    //    pFile = g_OpenIniFile(CONFIG_FILE_NAME);
    //    KGLOG_PROCESS_ERROR(pFile);
    //    //Option.nResultionIndex  = 0;
    //    pFile->GetInteger("KG3DENGINE", "MultiSampleType", D3DMULTISAMPLE_NONE, (int *)&eMultiSampleType);
    //    pFile->GetInteger("KG3DENGINE", "SampMipFilter", D3DTEXF_LINEAR, (int *)&eTextureFilterType);
    //    pFile->GetInteger("KG3DENGINE", "TextureScale", 2, &nTextureScale);
    //    pFile->GetFloat("KG3DENGINE", "CammeraDistance", 40000.0F, &Option.fCammeraDistance);
    //    pFile->GetInteger("KG3DENGINE", "WaterRefraction", TRUE, &Option.nEnableWaterRefraction);
    //    pFile->GetInteger("KG3DENGINE", "RenderGrass", TRUE, &Option.nEnableRenderGrass);
    //    pFile->GetInteger("KG3DENGINE", "WaterReflection", TRUE, &Option.nEnableWaterReflection);
    //    pFile->GetInteger("KG3DENGINE", "bGodRay", TRUE, &Option.nEnableGodRay);
    //    pFile->GetInteger("KG3DENGINE", "BloomEnable", TRUE, &Option.nEnableBloom);
    //    pFile->GetInteger("KG3DENGINE", "ShockWaveEnable", TRUE, &Option.nEnableShockWave);
    //    pFile->GetInteger("KG3DENGINE", "ModelShadowType", TRUE, &Option.nEnableSCM);
    //    break;

    //default:
    //    ASSERT(false);
    //    break;
    //}

    // always set to full screen by default
    Option.nResultionIndex  = 0;
    for (i = 0; i < gs_uSupportedResolutionCount; ++i)
    {
        if (gs_SupportedResolution[i].nFullScreen)
        {
            Option.nResultionIndex = i;
            break;
        }
    }
    Option.nMutisampleTypeIndex = 0;
    for (i = 0; i < gs_uSupportedMutiSampleTypeCount; ++i)
    {
        if (eMultiSampleType == gs_SupportedMutiSampleType[i].eType)
        {
            Option.nMutisampleTypeIndex = i;
            break;
        }
    }

	Option.nShadowTypeIndex = 0;
	for (i = 0; i < SHADOW_TYPE_PRESETS_COUNT; ++i)
	{
	    if (nShadow == gs_ShadowTypePresets[i].nType)
	    {
	        Option.nShadowTypeIndex = i;
	        break;
	    }
	}

	Option.nPostEffectIndex = 0;
	for (i = 0; i < POSTEFFECT_TYPE_PRESETS_COUNT; ++i)
	{
		if (bPostEffect == gs_PostEffectType[i].nType)
		{
			Option.nPostEffectIndex = i;
			break;
		}
	}

    //Option.nTextureFilterIndex = 0;
    //for (i = 0; i < TEXTURE_FILTER_PRESETS_COUNT; ++i)
    //{
    //    if (eTextureFilterType == gs_TextureFilterPresets[i].eType)
    //    {
    //        Option.nTextureFilterIndex = i;
    //        break;
    //    }
    //}
	{
		LPDIRECT3D9 pD3D = Direct3DCreate9( D3D_SDK_VERSION );
		D3DADAPTER_IDENTIFIER9 Identifier;
		HRESULT hr;
		hr = pD3D->GetAdapterIdentifier(0, 0, &Identifier);
		// check CUDA support
		int nDevice = 0;
		cudaD3D9GetDevice(&nDevice, Identifier.DeviceName);
		if (cudaGetLastError() != 0)
		{
			Option.nEnableCudaTest = 2;
		}
		else
		{
			Option.nEnableCudaTest = 1;
		}
		Option.bEnableNormalTest = TRUE;
		SAFE_RELEASE(pD3D);
	}
    _SetDlgItemValue(hDlg, Option);

    nResult = true;
Exit0:
    return nResult;
}

////////////////////////////////////////////////////////////////////////////////
static HRESULT _InitDialog(HWND hDlg)
{
    HRESULT hResult = E_FAIL;
    unsigned i = 0;
    DIALOG_ITEM_VALUE_GROUP Option = { 0 };

	LPDIRECT3D9 pD3D = NULL;
	if (NULL == (pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
	{
		MessageBox(hDlg,"Failed Create the D3D object!.","Failed Create the D3D object!",0);
		return E_FAIL;
	}
	D3DCAPS9 Caps;
    if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&Caps)))
	{
		MessageBox(hDlg,"Get Caps Error!","Get Caps Error!",0);
		return E_FAIL;
	}
	if (Caps.PixelShaderVersion < D3DPS_VERSION(2,0))
	{
		MessageBox(hDlg,"PixelShaderVersion is less than 2.0! Maybe later!","PixelShaderVersion is less than 2.0!",0);
		return E_FAIL;
	}

	D3DDISPLAYMODE DisplayMode;
	if ( FAILED( pD3D->GetAdapterDisplayMode( D3DADAPTER_DEFAULT, &DisplayMode ) ) )
	{
		MessageBox(hDlg,"Failed GetAdapterDisplayMode !","Failed Create the D3D object!",0);
		return E_FAIL;
	}

	int nFullScreenResultionFlag = false;
    for (i = 0; i < RESOLUTION_PRESETS_COUNT; ++i)
    {
        if (
            //(gs_ResolutionPresets[i].uWidth  <= DisplayMode.Width) &&
            //(gs_ResolutionPresets[i].uHeight <= DisplayMode.Height)
			TRUE
        )
        {
            gs_SupportedResolution[gs_uSupportedResolutionCount] = gs_ResolutionPresets[i];
            if (
                (gs_ResolutionPresets[i].uWidth  == DisplayMode.Width) &&
                (gs_ResolutionPresets[i].uHeight == DisplayMode.Height)
            )
            {
                nFullScreenResultionFlag = true;
                gs_SupportedResolution[gs_uSupportedResolutionCount].nFullScreen = 1;
            }
            SendDlgItemMessage(hDlg, IDC_RESULTION, CB_INSERTSTRING, gs_uSupportedResolutionCount, (LPARAM)gs_SupportedResolution[gs_uSupportedResolutionCount].czName);
            ++gs_uSupportedResolutionCount;
        }
    }

    if (!nFullScreenResultionFlag)
    {
        _RESOLUTION *pResolution = &(gs_SupportedResolution[gs_uSupportedResolutionCount]);
	    sprintf(pResolution->czName, "%d X %d", DisplayMode.Width, DisplayMode.Height);
        pResolution->uWidth  = DisplayMode.Width;
        pResolution->uHeight = DisplayMode.Height;
        pResolution->nFullScreen = 1;
        SendDlgItemMessage(hDlg, IDC_RESULTION, CB_INSERTSTRING, gs_uSupportedResolutionCount, (LPARAM)gs_SupportedResolution[gs_uSupportedResolutionCount].czName);
        Option.nResultionIndex = gs_uSupportedResolutionCount;
        ++gs_uSupportedResolutionCount;
    }

    //for (i = 0; i < TEXTURE_FILTER_PRESETS_COUNT; ++i)
    //{
    //    SendDlgItemMessage(hDlg,IDC_TEXTURE_FILTER, CB_INSERTSTRING, i, (LPARAM)gs_TextureFilterPresets[i].czName);
    //}

	gs_uSupportedMutiSampleTypeCount = 0;
	for (i = 0; i < MUTISAMPLE_PRESETS_COUNT; i++)
	{
		HRESULT hr;
		hr = pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, DisplayMode.Format, FALSE, gs_MutiSampleTypePresets[i].eType, NULL);
		if (SUCCEEDED(hr))
		{
			gs_SupportedMutiSampleType[gs_uSupportedMutiSampleTypeCount] = gs_MutiSampleTypePresets[i];
			SendDlgItemMessage(
                hDlg,
                IDC_MSAA,
                CB_INSERTSTRING,
                gs_uSupportedMutiSampleTypeCount,
                (LPARAM)gs_SupportedMutiSampleType[gs_uSupportedMutiSampleTypeCount].czName
            );
			++gs_uSupportedMutiSampleTypeCount;
		}
	}

	for (i = 0; i < SHADOW_TYPE_PRESETS_COUNT; ++i)
	{
	    SendDlgItemMessage(hDlg,IDC_SHADOW, CB_INSERTSTRING, i, (LPARAM)gs_ShadowTypePresets[i].cszName);
	}

	for (i = 0; i < POSTEFFECT_TYPE_PRESETS_COUNT; ++i)
	{
		SendDlgItemMessage(hDlg,IDC_POSTEFFECT, CB_INSERTSTRING, i, (LPARAM)gs_PostEffectType[i].cszName);
	}	

    //SendDlgItemMessage(hDlg, IDC_CAMMERA_DISTANCE, TBM_SETRANGEMIN, TRUE, (LONG)20000);
    //SendDlgItemMessage(hDlg, IDC_CAMMERA_DISTANCE, TBM_SETRANGEMAX, TRUE, (LONG)80000);

    _ApplayPresetOption(hDlg);
	HBITMAP hBitmap; 
	hBitmap =(HBITMAP)LoadImage(NULL, 
		"DebugFiles/BenchMark/JX3.bmp", 
		IMAGE_BITMAP,0,0, 
		LR_LOADFROMFILE);
	SendDlgItemMessage(hDlg, IDC_IMAGE, STM_SETIMAGE, (WPARAM)IMAGE_BITMAP, (LPARAM)hBitmap);

    hResult = S_OK;
//Exit0:
	SAFE_RELEASE(pD3D);
	return hResult;
}

////////////////////////////////////////////////////////////////////////////////
static HRESULT _SaveIniFromDialog(HWND hDlg)
{
    HRESULT hResult = E_FAIL;
	KSmartIniFile pFile ;
	const char CONFIG_FILE_NAME[] = "config.ini";
    DIALOG_ITEM_VALUE_GROUP Option = { 0 };

    _GetDlgItemValue(hDlg, &Option);
    
	pFile = g_OpenIniFile(CONFIG_FILE_NAME);
	KGLOG_PROCESS_ERROR(pFile);

	pFile->WriteInteger("Main", "CanvasWidth",  gs_SupportedResolution[Option.nResultionIndex].uWidth);
	pFile->WriteInteger("Main", "CanvasHeight", gs_SupportedResolution[Option.nResultionIndex].uHeight);
    pFile->WriteInteger("Main", "FullScreen",   1/*gs_SupportedResolution[Option.nResultionIndex].nFullScreen*/);
	pFile->WriteInteger("Main", "ExclusiveMode",   0/*gs_SupportedResolution[Option.nResultionIndex].nFullScreen*/);


    pFile->WriteInteger("KG3DENGINE", "MultiSampleType", gs_SupportedMutiSampleType[Option.nMutisampleTypeIndex].eType);

	if (gs_PostEffectType[Option.nPostEffectIndex].nType || gs_ShadowTypePresets[Option.nShadowTypeIndex].nType > 1)
		pFile->WriteInteger("KG3DENGINE", "PostRenderEffect", 1);
	else
		pFile->WriteInteger("KG3DENGINE", "PostRenderEffect", 0);

	pFile->WriteInteger("KG3DENGINE", "ModelShadowType", gs_ShadowTypePresets[Option.nShadowTypeIndex].nType);

	if (gs_PostEffectType[Option.nPostEffectIndex].nType)
	{
		 pFile->WriteInteger("KG3DENGINE", "bGodRay",         1);
		 pFile->WriteInteger("KG3DENGINE", "ShockWaveEnable",         1);
		 pFile->WriteInteger("KG3DENGINE", "bHDR",         1);
		 pFile->WriteInteger("KG3DENGINE", "bDOF",         1);
	}
	else
	{
		pFile->WriteInteger("KG3DENGINE", "bGodRay",         0);
		pFile->WriteInteger("KG3DENGINE", "ShockWaveEnable",         0);
		pFile->WriteInteger("KG3DENGINE", "bHDR",         0);
		pFile->WriteInteger("KG3DENGINE", "bDOF",         0);
	}
	pFile->WriteInteger("KG3DENGINE", "MaxAnisotropy", 16);
    pFile->WriteFloat("KG3DENGINE", "CammeraDistance", 80000.000000);
    pFile->WriteInteger("KG3DENGINE", "RenderGrass", 1);
    pFile->WriteInteger("KG3DENGINE", "WaterRefraction", 1);
	pFile->WriteInteger("KG3DENGINE", "WaterReflection",         1);
	pFile->WriteInteger("KG3DENGINE", "WaterReflectEnvironment", 1);
	pFile->WriteInteger("KG3DENGINE", "WaterReflectGround",      1);
	pFile->WriteInteger("KG3DENGINE", "WaterReflectNPC",         1);
	pFile->WriteInteger("KG3DENGINE", "WaterReflectEntity",      1);
	pFile->WriteInteger("KG3DENGINE", "TextureScale",      1);

	pFile->WriteInteger("KG3DENGINE", "GrassAnimation",      1);
	pFile->WriteInteger("KG3DENGINE", "VegetationDensity",      1);
	pFile->WriteInteger("KG3DENGINE", "VSync",      0);
	pFile->WriteInteger("KG3DENGINE", "WaterDetail",      4);
	pFile->WriteInteger("KG3DENGINE", "EnablePhysics",      1);
	pFile->WriteInteger("ENGINEOPTION", "b3DVision",      1);
	pFile->Save(CONFIG_FILE_NAME);
	g_benchMarkOption.bPlayCudaTest = (Option.nEnableCudaTest == 1);
	g_benchMarkOption.bPlayNoramlTest = Option.bEnableNormalTest;
    hResult = S_OK;
Exit0:
	return hResult;
}

BOOL CALLBACK OptionDlgProc(HWND hDlg, UINT message,WPARAM wParam, LPARAM lParam)
{
    KG_USE_ARGUMENT(lParam);

	switch (message)
	{
	case WM_INITDIALOG:
        {
            return SUCCEEDED(_InitDialog(hDlg));//这个必须检查错误，如果起不来，不能硬起，必须关闭程序，不然下面引发device错误
        }
	case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case IDCANCEL:
                EndDialog(hDlg, FALSE);
                return TRUE;

            case IDOK :
                if (FAILED(_SaveIniFromDialog(hDlg)))
                {
                    MessageBox(hDlg, "Config Save failed!", "Error", MB_OK | MB_ICONSTOP);
                    EndDialog(hDlg, FALSE);
                }
                else
                {
                    EndDialog(hDlg, TRUE) ;
                }
                return TRUE ;

                //case IDC_PRESET_OPTION_GROUP_LOW:
                //case IDC_PRESET_OPTION_GROUP_MIDDLE:
                //case IDC_PRESET_OPTION_GROUP_HIGH:
                //case IDC_PRESET_OPTION_GROUP_CUSTOM:
                //    if (HIWORD (wParam) == BN_CLICKED)
                //        _ApplayPresetOption(hDlg, LOWORD(wParam));
                //    return TRUE;

            }
        }
		break ;
	}
	return  FALSE;
}

////////////////////////////////////////////////////////////////////////////////
int ShowOptionDialog()
{
    int nResult  = false;
    int nRetCode = false;
    HINSTANCE hInstance;
    INITCOMMONCONTROLSEX InitCtrls;

    hInstance = (HINSTANCE)::GetModuleHandle(NULL);
    KGLOG_PROCESS_ERROR(hInstance);

    // InitCommonControlsEx() is required on Windows XP if an application
    // manifest specifies use of ComCtl32.dll version 6 or later to enable
    // visual styles.  Otherwise, any window creation will fail.
    InitCtrls.dwSize = sizeof(InitCtrls);
    // Set this to include all the common control classes you want to use
    // in your application.
    InitCtrls.dwICC = ICC_WIN95_CLASSES;
    nRetCode = InitCommonControlsEx(&InitCtrls);
    KGLOG_PROCESS_ERROR(nRetCode);

    nRetCode = (int)DialogBox(hInstance,MAKEINTRESOURCE(IDD_OPTIONNEW),NULL, OptionDlgProc);
    KGLOG_PROCESS_ERROR(nRetCode == TRUE);

    nResult = true;
Exit0:
    return nResult;
}
