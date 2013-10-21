// KPageOfConfig.cpp : implementation file
//

#include "stdafx.h"
#include "GameConfig.h"
#include "KClientSetting.h"
#include "OptionDefine.h"
#include "kglog.h"

IMPLEMENT_DYNAMIC(KClientSetting, CDialog)

KClientSetting::KClientSetting(CWnd* pParent /*=NULL*/)
	: CDialog(KClientSetting::IDD, pParent)
{
	//m_eCurrentOption = OPTION_GROUP_LOW;
	m_nInited = false;
	memset((void *)&m_Option, 0x0, sizeof(m_Option));
}

KClientSetting::~KClientSetting()
{
}

void KClientSetting::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDER_CAMMERAdISTANCE, m_SliderCammeraDistance);
	DDX_Control(pDX, IDC_SLIDER_WATER_DETAIL, m_SliderWaterDetail);
	DDX_Control(pDX, IDC_SLIDER_TERRAIN_DETAIL, m_SliderTerrainDetail);
	DDX_Control(pDX, IDC_SLIDER_TEXTURE_SCALE, m_SliderTextureScale);
}


BEGIN_MESSAGE_MAP(KClientSetting, CDialog)
	ON_BN_CLICKED(IDC_RADIO_LOW, &KClientSetting::OnBnClickedRadioLow)
	ON_BN_CLICKED(IDC_RADIO_MID, &KClientSetting::OnBnClickedRadioMID)
	ON_BN_CLICKED(IDC_RADIO_HIGH, &KClientSetting::OnBnClickedRadioHIGH)
	ON_BN_CLICKED(IDC_RADIO_CUSTOM, &KClientSetting::OnBnClickedRadioCUSTOM)
    ON_BN_CLICKED(IDC_CHECK_FULL_SCREEN, &KClientSetting::OnBnClickedCheckFullScreen)
    ON_BN_CLICKED(IDC_CHECK_EXCLUSIVE, &KClientSetting::OnBnClickedCheckExclusive)
END_MESSAGE_MAP()


int KClientSetting::Init()
{
    int nRetCode = false;
    int nResult  = false;
    TCHAR szApplicationPath[MAX_PATH];
    TCHAR szConfigFileName[MAX_PATH];
    TCHAR *pchLashSlash = NULL;

    KG_PROCESS_SUCCESS(m_nInited);

    nRetCode = _InitSlider();
    KGLOG_PROCESS_ERROR(nRetCode && "_InitSlider()");

    nRetCode = GetModuleFileName(NULL, szApplicationPath, _countof(szApplicationPath));
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szApplicationPath));

    pchLashSlash = _tcsrchr(szApplicationPath, _T('\\'));
    KGLOG_PROCESS_ERROR(pchLashSlash);
    *(pchLashSlash + 1) = _T('\0');

    nRetCode = _sntprintf(
        szConfigFileName, _countof(szConfigFileName), _T("%s%s"), szApplicationPath, _T("config.ini")
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0);
    KGLOG_PROCESS_ERROR(nRetCode < _countof(szConfigFileName));

    m_IniFile.Init(szConfigFileName);

    nRetCode = _InitResolution();
    KGLOG_PROCESS_ERROR(nRetCode);
    _ChangeConfig(OPTION_GROUP_CUSTOM);

    m_nInited = true;

Exit1:
    nResult = true;
Exit0:
    return nResult;
}


void KClientSetting::OnBnClickedRadioLow()
{
	_ChangeConfig(OPTION_GROUP_LOW);
}
void KClientSetting::OnBnClickedRadioCUSTOM()
{
	_ChangeConfig(OPTION_GROUP_CUSTOM);
}
void KClientSetting::OnBnClickedRadioHIGH()
{
	_ChangeConfig(OPTION_GROUP_HIGH);
}
void KClientSetting::OnBnClickedRadioMID()
{
	_ChangeConfig(OPTION_GROUP_MID);
}

void KClientSetting::_ChangeConfig(OPTION_GROUP eOptionID)
{
	unsigned i = 0;
	int nWidth = 0;
	int nHeight = 0;
	D3DMULTISAMPLE_TYPE  eMultiSampleType   = D3DMULTISAMPLE_NONE;
	D3DTEXTUREFILTERTYPE eTextureFilterType = D3DTEXF_NONE;

	switch (eOptionID)
	{
	case OPTION_GROUP_LOW:
		m_Option.nRadioLow    = true;
		m_Option.nRadioMiddle = false;
		m_Option.nRadioHigh   = false;
		m_Option.nRadioCustom = false;

		eMultiSampleType = D3DMULTISAMPLE_NONE;
		eTextureFilterType = D3DTEXF_LINEAR;

		m_Option.nCammeraDistance = 20000;
		m_Option.nEnableRenderGrass = true;
		m_Option.nEnableWaterRefraction = false;
		m_Option.nEnableWaterReflection = false;
		m_Option.nEnableGodRay = false;
		m_Option.nEnableBloom = false;
		m_Option.nEnableShockWave = false;
		
		m_Option.nGrassAnimation = 0;
		m_Option.nVegetationDensity = 3;
		m_Option.szTerrainDetail[0] = _T('2');
		m_Option.szTerrainDetail[1] = _T('\0');

		m_Option.nFullScreenFlag = false;
		m_Option.nResultionIndex  = 0;
		m_Option.nModelShadowType = 0;  //阴影选项 低配置为0
		m_Option.nWaterDetail     = 0;  //水面精度 低配置为0
		//m_Option.nTerrainDetail   = 0;  //地形精度 低配置为0
		m_Option.nTextureScale    = 1;  //贴图精度 低配置为1
		break;

	case OPTION_GROUP_MID:
		m_Option.nRadioLow    = false;
		m_Option.nRadioMiddle = true;
		m_Option.nRadioHigh   = false;
		m_Option.nRadioCustom = false;

		eMultiSampleType = D3DMULTISAMPLE_2_SAMPLES;
		eTextureFilterType = D3DTEXF_LINEAR;

		m_Option.nCammeraDistance = 40000;
		m_Option.nEnableRenderGrass = true;
		m_Option.nEnableWaterRefraction = true;
		m_Option.nEnableWaterReflection = true;
		m_Option.nEnableGodRay = false;
		m_Option.nEnableBloom = false;
		m_Option.nEnableShockWave = false;
		m_Option.nFullScreenFlag = false;
		m_Option.nResultionIndex  = gs_uSupportedResolutionCount / 2;

		m_Option.nVegetationDensity = 1;
		m_Option.nGrassAnimation = 1;
		m_Option.szTerrainDetail[0] = _T('X');
		m_Option.szTerrainDetail[1] = _T('\0');

		m_Option.nModelShadowType = 1;  //阴影选项 中配置为1
		m_Option.nWaterDetail     = 1;  //水面精度 中配置为1
		//m_Option.nTerrainDetail   = 3;  //地形精度 中配置为3
		m_Option.nTextureScale    = 1;  //贴图精度 中配置为1
		break;
	case OPTION_GROUP_HIGH:
		m_Option.nRadioLow    = false;
		m_Option.nRadioMiddle = false;
		m_Option.nRadioHigh   = true;
		m_Option.nRadioCustom = false;

		eMultiSampleType = D3DMULTISAMPLE_4_SAMPLES;
		eTextureFilterType = D3DTEXF_ANISOTROPIC;

		m_Option.nCammeraDistance = 80000;
		m_Option.nEnableRenderGrass = true;
		m_Option.nEnableWaterRefraction = true;
		m_Option.nEnableWaterReflection = true;
		m_Option.nEnableGodRay = true;
		m_Option.nEnableBloom = true;
		m_Option.nEnableShockWave = true;
		m_Option.nFullScreenFlag = true;
		m_Option.nResultionIndex = gs_uSupportedResolutionCount - 1;

		m_Option.nVegetationDensity = 1;
		m_Option.nGrassAnimation = 1;
		m_Option.szTerrainDetail[0] = _T('X');
		m_Option.szTerrainDetail[1] = _T('\0');

		m_Option.nModelShadowType = 2;  //阴影选项 高配置为2
		m_Option.nWaterDetail     = 4;  //水面精度 高配置为4
		//m_Option.nTerrainDetail   = 3;  //地形精度 高配置为3
		m_Option.nTextureScale    = 1;  //贴图精度 高配置为1
 		break;
	case OPTION_GROUP_CUSTOM:
		m_Option.nRadioLow    = false;
		m_Option.nRadioMiddle = false;
		m_Option.nRadioHigh   = false;
		m_Option.nRadioCustom = true;		

		m_Option.nResultionIndex  = 0;
		m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_MULTISAMPLETYPE, D3DTEXF_LINEAR, (int *)&eMultiSampleType);
		m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_SAMPMIPFILTER, D3DTEXF_LINEAR, (int *)&eTextureFilterType);
		m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_CAMMERAdISTANCE, 40000, &m_Option.nCammeraDistance);
		m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_WATERREFRACTION, TRUE, &m_Option.nEnableWaterRefraction);
		m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_RENDERGRASS, TRUE, &m_Option.nEnableRenderGrass);
		m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_WATERREFLECTION, TRUE, &m_Option.nEnableWaterReflection);
		m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_BGODRAY, TRUE, &m_Option.nEnableGodRay);
		m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_BLOOMENABLE, TRUE, &m_Option.nEnableBloom);
		m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_SHOCKWAVEENABLE, TRUE, &m_Option.nEnableShockWave);
		m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_VEGETATIONDENSITY, 1, &m_Option.nVegetationDensity);

		m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_MODELSHADOWTYPE, 0, &m_Option.nModelShadowType); //阴影选项
		m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_WATERDETAIL,     0, &m_Option.nWaterDetail);     //水面精度
		//m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_TERRAINDETAIL,   0, &m_Option.nTerrainDetail);   //地形精度
		m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_TEXTURESCALE,    1, &m_Option.nTextureScale);    //贴图精度

		m_IniFile.GetInteger(CONFIG_SECTION_SOUND, CONFIG_KEY_SILENCE, FALSE, &m_Option.nSilence);

        m_IniFile.GetInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_3D_VISION, FALSE, &(m_Option.n3DVisionFlag));

		m_IniFile.GetString(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_FORCESHADERMODEL, _T("X"), m_Option.szTerrainDetail, sizeof(m_Option.szTerrainDetail) / sizeof(m_Option.szTerrainDetail[0]));
		m_Option.szTerrainDetail[1] = _T('\0');
		
		m_Option.nGrassAnimation = 1;
			
		m_Option.nResultionIndex = 0;
		m_IniFile.GetInteger(CONFIG_SECTION_MAIN, CONFIG_KEY_CANVASWIDTH, 800, &nWidth);
		m_IniFile.GetInteger(CONFIG_SECTION_MAIN, CONFIG_KEY_CANVASHEIGHT, 600, &nHeight);
	
		for (i = 0; i < gs_uSupportedResolutionCount; ++i)
		{
			if (gs_SupportedResolution[i].uWidth >= nWidth)
			{
				if (gs_SupportedResolution[i].uHeight >= nHeight)
				{
					m_Option.nResultionIndex = i;
					break;
				}
			}
		}
		
        m_IniFile.GetInteger(CONFIG_SECTION_MAIN, CONFIG_KEY_FULLSCREEN,     FALSE, &(m_Option.nFullScreenFlag));
        m_IniFile.GetInteger(CONFIG_SECTION_MAIN, CONFIG_KEY_EXCLUSIVE_MODE, FALSE, &(m_Option.nExclusiveFlag));
        m_IniFile.GetInteger(CONFIG_SECTION_MAIN, CONFIG_KEY_PANAUISION,     FALSE, &(m_Option.nPanauisionFlag));

		break;
	default:
		ASSERT(FALSE);
		break;
	}
 

	m_Option.nMutisampleTypeIndex = 0;
	for (i = 0; i < gs_uSupportedMutiSampleTypeCount; ++i)
	{
		if (eMultiSampleType == gs_SupportedMutiSampleType[i].eType)
		{
			m_Option.nMutisampleTypeIndex = i;
			break;
		}
	}

	m_Option.nTextureFilterIndex = 0;
	for (i = 0; i < TEXTURE_FILTER_PRESETS_COUNT; ++i)
	{
		if (eTextureFilterType == gs_TextureFilterPresets[i].eType)
		{
			m_Option.nTextureFilterIndex = i;
			break;
		}
	}

	_ChangeCtrlStatus();
}

void KClientSetting::_ChangeCtrlStatus()
{
	int nRetCode = false;

	nRetCode = (int)SendDlgItemMessage(IDC_COMBO_RESOLUTION, CB_SETCURSEL, m_Option.nResultionIndex, 0);
	ASSERT(nRetCode != CB_ERR);

	nRetCode = (int)SendDlgItemMessage(IDC_COMBO_MULTISAMPL, CB_SETCURSEL, m_Option.nMutisampleTypeIndex, 0);
	ASSERT(nRetCode != CB_ERR);

	nRetCode = (int)SendDlgItemMessage(IDC_COMBO_SAMPMIPFILTER, CB_SETCURSEL, m_Option.nTextureFilterIndex, 0);
	ASSERT(nRetCode != CB_ERR);

	nRetCode = (int)SendDlgItemMessage(IDC_COMBO_MODEL_SHADOW_TYPE, CB_SETCURSEL, m_Option.nModelShadowType, 0);
	ASSERT(nRetCode != CB_ERR);

	CheckDlgButton(IDC_RADIO_LOW, m_Option.nRadioLow);
	CheckDlgButton(IDC_RADIO_MID, m_Option.nRadioMiddle);
	CheckDlgButton(IDC_RADIO_HIGH, m_Option.nRadioHigh);
	CheckDlgButton(IDC_RADIO_CUSTOM, m_Option.nRadioCustom);

	CheckDlgButton(IDC_CHECK_BLOOMENABLE, m_Option.nEnableBloom);
	CheckDlgButton(IDC_CHECK_GODRAY, m_Option.nEnableGodRay);
	CheckDlgButton(IDC_CHECK_RENDERGRASS, m_Option.nEnableRenderGrass);
	CheckDlgButton(IDC_CHECK_SHOCKWAVEENABLE, m_Option.nEnableShockWave);
	CheckDlgButton(IDC_CHECK_QUIET, m_Option.nSilence);

    CheckDlgButton(IDC_CHECK_3D_VISION, m_Option.n3DVisionFlag);
    
    CheckDlgButton(IDC_CHECK_FULL_SCREEN, m_Option.nFullScreenFlag);
    CheckDlgButton(IDC_CHECK_EXCLUSIVE,   m_Option.nExclusiveFlag);
    CheckDlgButton(IDC_CHECK_PANAUISION,  m_Option.nPanauisionFlag);
    OnBnClickedCheckExclusive();
    OnBnClickedCheckFullScreen();

	m_SliderCammeraDistance.SetPos(m_Option.nCammeraDistance);

	m_SliderWaterDetail.SetPos(m_Option.nWaterDetail);

	m_SliderTerrainDetail.SetPos(m_Option.nTerrainDetail);

	m_SliderTextureScale.SetPos( 5 - m_Option.nTextureScale);
}

BOOL KClientSetting::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_RETURN)
	{
		return TRUE;
	}
	return CDialog::PreTranslateMessage(pMsg); 

}   

void KClientSetting::Save()
{
	_GetItemValue();
	
	if (m_Option.nFullScreenFlag) //当全屏模式时，禁止AA
		gs_SupportedMutiSampleType[m_Option.nMutisampleTypeIndex].eType = D3DMULTISAMPLE_NONE;
	
	m_IniFile.WriteInteger(CONFIG_SECTION_MAIN, CONFIG_KEY_CANVASWIDTH,  gs_SupportedResolution[m_Option.nResultionIndex].uWidth);
	m_IniFile.WriteInteger(CONFIG_SECTION_MAIN, CONFIG_KEY_CANVASHEIGHT, gs_SupportedResolution[m_Option.nResultionIndex].uHeight);
	m_IniFile.WriteInteger(CONFIG_SECTION_MAIN, CONFIG_KEY_FULLSCREEN, m_Option.nFullScreenFlag);
    m_IniFile.WriteInteger(CONFIG_SECTION_MAIN, CONFIG_KEY_EXCLUSIVE_MODE, m_Option.nExclusiveFlag);
    m_IniFile.WriteInteger(CONFIG_SECTION_MAIN, CONFIG_KEY_PANAUISION, m_Option.nPanauisionFlag);

	m_IniFile.WriteInteger(CONFIG_SECTION_ENGINEOPTION, CONFIG_KEY_BENABLEAUTORELOADTEXTURE,  0);  //low and mid and high must be 0
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_GRASSANIMATION,            m_Option.nGrassAnimation);  //low and mid and high must be 0

	m_IniFile.WriteString(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_FORCESHADERMODEL,          m_Option.szTerrainDetail);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_VEGETATIONDENSITY,        m_Option.nVegetationDensity);

	m_IniFile.WriteInteger(CONFIG_SECTION_SOUND, CONFIG_KEY_SILENCE,                      m_Option.nSilence);
	
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_MULTISAMPLETYPE,         gs_SupportedMutiSampleType[m_Option.nMutisampleTypeIndex].eType);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_SAMPMIPFILTER,           gs_TextureFilterPresets[m_Option.nTextureFilterIndex].eType);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_SAMPMINFILETER,          gs_TextureFilterPresets[m_Option.nTextureFilterIndex].eType);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_SAMPMAGFILTER,           gs_TextureFilterPresets[m_Option.nTextureFilterIndex].eType);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_CAMMERAdISTANCE,         m_Option.nCammeraDistance);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_RENDERGRASS,             TRUE);  //low and mid and high must be 1
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_WATERREFRACTION,         m_Option.nEnableWaterRefraction);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_WATERREFLECTION,         m_Option.nEnableWaterReflection);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_WATERREFLECTENVIRONMENT, m_Option.nEnableWaterReflection);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_WATERREFLECTGROUND,      m_Option.nEnableWaterReflection);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_WATERREFLECTNPC,         m_Option.nEnableWaterReflection);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_WATERREFLECTENTITY,      m_Option.nEnableWaterReflection);

	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_WATERREFLECTENTITY,      m_Option.nEnableWaterReflection);	
	
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_MODELSHADOWTYPE,         m_Option.nModelShadowType);  //阴影选项
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_WATERDETAIL,             m_Option.nWaterDetail);      //水面精度
	//m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_TERRAINDETAIL,           m_Option.nTerrainDetail);    //地形精度
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_TEXTURESCALE,            m_Option.nTextureScale);      //贴图精度


	int nPostRender = false;
	if (m_Option.nEnableGodRay || m_Option.nEnableBloom || m_Option.nEnableShockWave || m_Option.nModelShadowType > 1)
	{
		nPostRender = true;
	}

	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_POSTRENDEREFFECT, nPostRender);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_BGODRAY,          m_Option.nEnableGodRay);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_SHOCKWAVEENABLE,  m_Option.nEnableShockWave);
	m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_BLOOMENABLE,      m_Option.nEnableBloom);
    m_IniFile.WriteInteger(CONFIG_SECTION_KG3DENGINE, CONFIG_KEY_3D_VISION,        m_Option.n3DVisionFlag);

}

void KClientSetting::_GetItemValue()
{
	m_Option.nRadioLow = IsDlgButtonChecked(IDC_RADIO_LOW);
	ASSERT(m_Option.nRadioLow != BST_INDETERMINATE);

	m_Option.nRadioMiddle = IsDlgButtonChecked(IDC_RADIO_MID);
	ASSERT(m_Option.nRadioMiddle != BST_INDETERMINATE);

	m_Option.nRadioHigh   = IsDlgButtonChecked(IDC_RADIO_HIGH);
	ASSERT(m_Option.nRadioHigh != BST_INDETERMINATE);

	m_Option.nRadioCustom = IsDlgButtonChecked(IDC_RADIO_CUSTOM);
	ASSERT(m_Option.nRadioCustom != BST_INDETERMINATE);

	m_Option.nResultionIndex = (int)SendDlgItemMessage(IDC_COMBO_RESOLUTION, CB_GETCURSEL, 0, 0);
	ASSERT(m_Option.nResultionIndex != CB_ERR);

	m_Option.nMutisampleTypeIndex = (int)SendDlgItemMessage(IDC_COMBO_MULTISAMPL, CB_GETCURSEL, 0, 0);
	ASSERT(m_Option.nMutisampleTypeIndex != CB_ERR);

	m_Option.nTextureFilterIndex = (int)SendDlgItemMessage(IDC_COMBO_SAMPMIPFILTER, CB_GETCURSEL, 0, 0);
	ASSERT(m_Option.nTextureFilterIndex != CB_ERR);

	//阴影选项
	m_Option.nModelShadowType = (int)SendDlgItemMessage(IDC_COMBO_MODEL_SHADOW_TYPE, CB_GETCURSEL, 0, 0);
	ASSERT(m_Option.nModelShadowType != CB_ERR);

	//水面精度
	m_Option.nWaterDetail = m_SliderWaterDetail.GetPos();
	ASSERT(m_Option.nWaterDetail >= 0);

	//地形精度
	//m_Option.nTerrainDetail = m_SliderTerrainDetail.GetPos();
	//ASSERT(m_Option.nTerrainDetail >= 0);

	//贴图精度
	m_Option.nTextureScale = 5 - m_SliderTextureScale.GetPos();
	ASSERT(m_Option.nTextureScale >= 0);

	m_Option.nCammeraDistance = m_SliderCammeraDistance.GetPos();
	ASSERT(m_Option.nCammeraDistance > 0);

	m_Option.nEnableRenderGrass = IsDlgButtonChecked(IDC_CHECK_RENDERGRASS);
	ASSERT(m_Option.nEnableRenderGrass != BST_INDETERMINATE);

	//m_Option.nEnableWaterRefraction = IsDlgButtonChecked(IDC_CHECK_WATERREFRACTION);
	//ASSERT(m_Option.nEnableWaterRefraction != BST_INDETERMINATE);

	//m_Option.nEnableWaterReflection = IsDlgButtonChecked(IDC_CHECK_WATERREFLECTION);
	//ASSERT(m_Option.nEnableWaterReflection != BST_INDETERMINATE);

	m_Option.nEnableGodRay = IsDlgButtonChecked(IDC_CHECK_GODRAY);
	ASSERT(m_Option.nEnableGodRay != BST_INDETERMINATE);

	m_Option.nEnableBloom = IsDlgButtonChecked(IDC_CHECK_BLOOMENABLE);
	ASSERT(m_Option.nEnableBloom != BST_INDETERMINATE);

	m_Option.nEnableShockWave = IsDlgButtonChecked(IDC_CHECK_SHOCKWAVEENABLE);
	ASSERT(m_Option.nEnableShockWave != BST_INDETERMINATE);

    m_Option.nSilence = IsDlgButtonChecked(IDC_CHECK_QUIET);
    
    m_Option.nFullScreenFlag     = IsDlgButtonChecked(IDC_CHECK_FULL_SCREEN);
    m_Option.nExclusiveFlag  = IsDlgButtonChecked(IDC_CHECK_EXCLUSIVE);
    m_Option.nPanauisionFlag = IsDlgButtonChecked(IDC_CHECK_PANAUISION);

    m_Option.n3DVisionFlag   = IsDlgButtonChecked(IDC_CHECK_3D_VISION);
}

int KClientSetting::_InitResolution()
{
	int nResult = false;
	unsigned i = 0;

	LPDIRECT3D9 pD3D = NULL;
	if (NULL == (pD3D = Direct3DCreate9(D3D_SDK_VERSION)))
	{
		MessageBox(_T("Failed Create the D3D object!."), _T("Failed Create the D3D object!"), 0);
		KGLOG_PROCESS_ERROR(false);
	}

	D3DCAPS9 Caps;
	if (FAILED(pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT,D3DDEVTYPE_HAL,&Caps)))
	{
		MessageBox(_T("Get Caps Error!"), _T("Get Caps Error!"), 0);
		KGLOG_PROCESS_ERROR(false);
	}

	if (Caps.PixelShaderVersion < D3DPS_VERSION(2,0))
	{
		MessageBox(_T("PixelShaderVersion is less than 2.0! Maybe later!"), _T("PixelShaderVersion is less than 2.0!"), 0);
		KGLOG_PROCESS_ERROR(false);
	}

	D3DDISPLAYMODE DisplayMode;
	if ( FAILED( pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &DisplayMode)))
	{
		MessageBox(_T("Failed GetAdapterDisplayMode !"), _T("Failed Create the D3D object!"), 0);
		KGLOG_PROCESS_ERROR(false);
	}

	int nFullScreenResultionFlag = false;
	for (i = 0; i < RESOLUTION_PRESETS_COUNT; ++i)
	{
		if (
			(gs_ResolutionPresets[i].uWidth  <= DisplayMode.Width) &&
			(gs_ResolutionPresets[i].uHeight <= DisplayMode.Height)
			)
		{
			gs_SupportedResolution[gs_uSupportedResolutionCount] = gs_ResolutionPresets[i];
			SendDlgItemMessage(IDC_COMBO_RESOLUTION, CB_INSERTSTRING, gs_uSupportedResolutionCount, (LPARAM)gs_SupportedResolution[gs_uSupportedResolutionCount].czName);
			++gs_uSupportedResolutionCount;
		}
	}

	for (i = 0; i < TEXTURE_FILTER_PRESETS_COUNT; ++i)
	{
		SendDlgItemMessage(IDC_COMBO_SAMPMIPFILTER, CB_INSERTSTRING, i, (LPARAM)gs_TextureFilterPresets[i].czName);
	}

	//初始化 阴影选项

	for (i = 0; i < MODEL_SHADOW_TYPE; ++i)
	{
		SendDlgItemMessage(IDC_COMBO_MODEL_SHADOW_TYPE, CB_INSERTSTRING, i, (LPARAM)gs_ShadowOption[i].czName);
	}

	gs_uSupportedMutiSampleTypeCount = 0;
	for (i = 0; i < MUTISAMPLE_PRESETS_COUNT; i++)
	{
		HRESULT hr;
		hr = pD3D->CheckDeviceMultiSampleType(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, DisplayMode.Format, TRUE, gs_MutiSampleTypePresets[i].eType, NULL);
		if (SUCCEEDED(hr))
		{
			gs_SupportedMutiSampleType[gs_uSupportedMutiSampleTypeCount] = gs_MutiSampleTypePresets[i];
			SendDlgItemMessage(
				IDC_COMBO_MULTISAMPL,
				CB_INSERTSTRING,
				gs_uSupportedMutiSampleTypeCount,
				(LPARAM)gs_SupportedMutiSampleType[gs_uSupportedMutiSampleTypeCount].czName
				);
			++gs_uSupportedMutiSampleTypeCount;
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int KClientSetting::_InitSlider()
{
	//初始化 远景显示
	m_SliderCammeraDistance.SetRange(20000, 80000);
	m_SliderCammeraDistance.SetPos(20000);
	m_SliderCammeraDistance.SetTic((80000 - 20000) / 5);
	m_SliderCammeraDistance.SetTicFreq((80000 - 20000) / 5);

	//初始化 水面精度
	m_SliderWaterDetail.SetRange(0, 4);
	m_SliderWaterDetail.SetPos(0);
	m_SliderWaterDetail.SetTic(1);
	m_SliderWaterDetail.SetTicFreq(1);

	//初始化 地形精度 
	m_SliderTerrainDetail.SetRange(0, 3);
	m_SliderTerrainDetail.SetPos(0);
	m_SliderTerrainDetail.SetTic(1);
	m_SliderTerrainDetail.SetTicFreq(1);

	//初始化 贴图精度
	m_SliderTextureScale.SetRange(1, 4);
	m_SliderTextureScale.SetPos(1);
	m_SliderTextureScale.SetTic(1);
	m_SliderTextureScale.SetTicFreq(1);
	
	return true;
}
void KClientSetting::OnBnClickedCheckFullScreen()
{
    // TODO: Add your control notification handler code here
    if (IsDlgButtonChecked(IDC_CHECK_FULL_SCREEN))
    {
        GetDlgItem(IDC_CHECK_EXCLUSIVE)->EnableWindow(true);

    }
    else
    {
        CheckDlgButton(IDC_CHECK_EXCLUSIVE, false);
        GetDlgItem(IDC_CHECK_EXCLUSIVE)->EnableWindow(false);
    }
    OnBnClickedCheckExclusive();
}

void KClientSetting::OnBnClickedCheckExclusive()
{
    // TODO: Add your control notification handler code here
    if (IsDlgButtonChecked(IDC_CHECK_EXCLUSIVE))
    {
        CheckDlgButton(IDC_CHECK_PANAUISION, false);
        GetDlgItem(IDC_CHECK_PANAUISION)->EnableWindow(false);

        GetDlgItem(IDC_CHECK_3D_VISION)->EnableWindow(true);
    }
    else
    {
        GetDlgItem(IDC_CHECK_PANAUISION)->EnableWindow(true);

        CheckDlgButton(IDC_CHECK_3D_VISION, false);
        GetDlgItem(IDC_CHECK_3D_VISION)->EnableWindow(false);
    }
}
