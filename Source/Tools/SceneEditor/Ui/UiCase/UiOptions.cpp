// -------------------------------------------------------------------------
//	文件名		：	UiOptions.cpp
//	创建者		：	吕桂华
//	创建时间	：	2002-9-11 15:05:15
//	功能描述	：
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../Elem/wnds.h"
#include "uioptions.h"
#include "UiInit.h"
#include "../ShortcutKey.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiBase.h"
#include "../Elem/PopupMenu.h"
#include "../UiSoundSetting.h"
#include "../UiChatPhrase.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*	g_pCoreShell;

#define SCHEME_INI_OPTION		"选项.ini"
#define OPTIONS_SAVE_SECTION	"Options"

#define	DEFAULT_SETTING_SET		"\\Ui\\autoexec.lua"
#define	LEGEND_SETTING_SET		"\\Ui\\船鳍配置.lua"

const char* ls_ToggleOptionName[OPTION_INDEX_COUNT] =
{
	"DynamicLight",
	"Weather",
	"Perspective",
};

KUiOptions* KUiOptions::m_pSelf = NULL;

extern int	g_bRepresent3;
extern int	g_bScreen;

KUiOptions::KUiOptions()
{
	m_pReturn = NULL;
	m_nToggleItemCount = 0;
	m_pSkinMenu = NULL;
	m_nBrightness = 50;
	m_nSoundValue = 100;
	m_nMusicValue = 100;
	m_nShortcutSet = 0;
	m_nFirstControlableIndex = 0;
	m_nToggleBtnValidCount = 0;
	m_nToggleItemCount = 0;
	m_nStatusEnableFrame = m_nStatusDisableFrame = m_nStatusInvalidFrame;
	for (int i = 0; i < OPTION_INDEX_COUNT; i++)
	{
		m_ToggleItemList[i].szName[0] = 0;
	}
	m_ToggleItemList[OPTION_I_DYNALIGHT].bInvalid = !g_bRepresent3;
	m_ToggleItemList[OPTION_I_WEATHER].bInvalid = false;
	m_ToggleItemList[OPTION_I_PERSPECTIVE].bInvalid = !g_bRepresent3;
}

void KUiOptions::CancelMenu()
{
	if (m_pSkinMenu)
	{
		KPopupMenu::Cancel();
		free(m_pSkinMenu);
		m_pSkinMenu = NULL;
	}
}

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiOptions* KUiOptions::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiOptions* KUiOptions::OpenWindow(KWndWindow* pReturn)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiOptions;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
	 	m_pSelf->m_pReturn = pReturn;
		LoadSetting(true, false);
		UiSoundPlay(UI_SI_POPUP_OUTGAME_WND);
		m_pSelf->Show();
		m_pSelf->BringToTop();
	}

	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口
//--------------------------------------------------------------------------
void KUiOptions::CloseWindow()
{
	if (m_pSelf)
	{
		m_pSelf->CancelMenu();
		m_pSelf->StoreSetting();
		if (m_pSelf->m_pReturn)
			m_pSelf->m_pReturn->Show();
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

// -------------------------------------------------------------------------
// 功能	: 初始化
// -------------------------------------------------------------------------
void KUiOptions::Initialize()
{
	AddChild(&m_CloseBtn);
	//AddChild(&m_SkinBtn);
	AddChild(&m_BrightnessScroll);
	AddChild(&m_BGMValue);
	AddChild(&m_SoundValue);
	AddChild(&m_ShortcutSetView);
	AddChild(&m_Scroll);
	for (int i = 0; i < MAX_TOGGLE_BTN_COUNT; i++)
	{
		AddChild(&m_ToggleBtn[i]);
		AddChild(&m_StatusImage[i]);
	}

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	m_pSelf->LoadScheme(Scheme);

	// 注册本窗口
	Wnd_AddWindow(this);
}

// -------------------------------------------------------------------------
// 功能	: 载入界面方案
// -------------------------------------------------------------------------
void KUiOptions::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_OPTION);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->LoadScheme(&Ini);
	}
}

//载入界面方案
void KUiOptions::LoadScheme(KIniFile* pIni)
{
//	assert(pIni);
	char szBuff[32];

	Init(pIni, "Main");	// 窗口背景数据
	m_CloseBtn.Init(pIni, "CloseBtn");
	m_ShortcutKeyBtn.Init(pIni, "ShortcutKeyBtn");
//	m_SkinBtn.Init(pIni, "SkinBtn");

	m_BrightnessScroll.Init(pIni, "Brightness");
	m_BGMValue.Init(pIni, "Music");
	m_SoundValue.Init(pIni, "Sound");
	m_ShortcutSetView.Init(pIni, "ShortcutSet");
	m_Scroll.Init(pIni, "Scroll");

	m_StatusImage[0].Init(pIni, "ToggleStatus");
	pIni->GetInteger("ToggleStatus", "NotCheckFrame", 0, &m_nStatusDisableFrame);
	pIni->GetInteger("ToggleStatus", "CheckFrame",    0, &m_nStatusEnableFrame);
	pIni->GetInteger("ToggleStatus", "DisableFrame",  0, &m_nStatusInvalidFrame);

	int	nIntervalH, nIntervalV, nCountPerLine;
	m_ToggleBtn[0].Init(pIni, "ToggleBtn");
	pIni->GetInteger2("ToggleBtn", "Interval", &nIntervalH, &nIntervalV);
	pIni->GetInteger("ToggleBtn", "CountPerLine", 1, &nCountPerLine);
	int	i, nBtnX, nBtnY, nImgX, nImgY;
	m_ToggleBtn[0].GetPosition(&nBtnX, &nBtnY);
	m_StatusImage[0].GetPosition(&nImgX, &nImgY);
	if (nCountPerLine < 1)
		nCountPerLine = 1;
	for (i = 1; i < MAX_TOGGLE_BTN_COUNT; i++)
	{
		int dx = (i % nCountPerLine) * nIntervalH;
		int dy = (i / nCountPerLine) * nIntervalV;
		m_ToggleBtn[0].Clone(&m_ToggleBtn[i]);
		m_ToggleBtn[i].SetPosition(nBtnX + dx,	nBtnY + dy);
		m_StatusImage[0].Clone(&m_StatusImage[i]);
		m_StatusImage[i].SetPosition(nImgX + dx, nImgY + dy);
	}

	for (i = 0; i < OPTION_INDEX_COUNT; i++)
	{
		char	szKey[16];
		pIni->GetString("ToggleOptionsName", itoa(i, szKey, 10),
			"", m_ToggleItemList[i].szName, sizeof(m_ToggleItemList[i].szName));
	}


	pIni->GetString("Settings", "EnableColor", "", szBuff, sizeof(szBuff));
	m_uEnableTextColor = GetColor(szBuff);

	pIni->GetString("Settings", "DisableColor", "", szBuff, sizeof(szBuff));
	m_uDisableTextColor = GetColor(szBuff);

	m_nFirstControlableIndex = 0;
	m_nToggleBtnValidCount = (OPTION_INDEX_COUNT <= MAX_TOGGLE_BTN_COUNT) ? OPTION_INDEX_COUNT : MAX_TOGGLE_BTN_COUNT;
	UpdateAllToggleBtn();
	UpdateAllStatusImg();
}

// -------------------------------------------------------------------------
// 功能	: 窗口函数
// -------------------------------------------------------------------------
int	 KUiOptions::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;

	switch(uMsg)
	{
	case WND_N_SCORLLBAR_POS_CHANGED:
		OnScrollBarPosChanged((KWndWindow*)uParam, nParam);
		break;
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
			CloseWindow();
		//else if (uParam == (unsigned int)(KWndWindow*)&m_SkinBtn)
		//	PopupSkinMenu();
		else if (uParam == (unsigned int)(KWndWindow*)&m_ShortcutSetView)
		{
			int nX, nY, nWidth, nHeight;
			m_ShortcutSetView.GetPosition(&nX, &nY);
			m_ShortcutSetView.GetSize(&nWidth, &nHeight);
			nX += nWidth + m_nAbsoluteLeft;
			nY += nHeight + m_nAbsoluteTop;
			PopupSeleteSetMenu(nX, nY);
		}
		else
		{
			for (int i = 0; i < m_nToggleBtnValidCount; i++)
			{
				if (uParam == (unsigned int)(KWndWindow*)&m_ToggleBtn[i])
				{
					ToggleOption(m_nFirstControlableIndex + i);
				}
			}
		}
		break;
	case WM_KEYDOWN:
		if (uParam  == VK_ESCAPE)
			CloseWindow();
		nRet = true;
		break;
	case WND_M_MENUITEM_SELECTED:
		if (uParam == (unsigned int)(KWndWindow*)this)
		{
			if (HIWORD(nParam) == 1)
			{
				if (GET_X_LPARAM(nParam) >= 0)
				{
					m_nShortcutSet = GET_X_LPARAM(nParam);
					UpdateSettingSet(m_nShortcutSet, false);
				}
				KPopupMenu::Cancel();
			}
			else
			{
			    CancelMenu();
			}
			//if (m_pSkinMenu && GET_X_LPARAM(nParam) >= 0 && GET_X_LPARAM(nParam) < m_pSkinMenu->nNumItem)
			//{
			//	if (g_UiBase.LoadScheme(m_pSkinMenu->Items[GET_X_LPARAM(nParam)].szData))
			//		m_SkinBtn.SetLabel(m_pSkinMenu->Items[GET_X_LPARAM(nParam)].szData);
			//}
		}
		break;
	default:
		nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

void KUiOptions::UpdateSettingSet(int eSet, bool bOnlyUpdateUi)
{
//这部分是根据CheckBox来做的
/*	if (eSet < SETTING_DEFAULT || eSet > SETTING_LEGEND)
		return;
	m_eSettingSet = eSet;
	m_IDefault.CheckButton(m_eSettingSet == SETTING_DEFAULT);
//	m_IMu.CheckButton(m_eSettingSet== SETTING_MU);
	m_ILegend.CheckButton(m_eSettingSet == SETTING_LEGEND);
	if (bOnlyUpdateUi == false)
	{
		KShortcutKeyCentre::ClearScript();
		const char* pszFileName;
//		if (m_eSettingSet== SETTING_MU)
//			pszFileName = MU_SETTING_SET;
		if (m_eSettingSet== SETTING_LEGEND)
			pszFileName = LEGEND_SETTING_SET;
		else
			pszFileName = DEFAULT_SETTING_SET;
		KShortcutKeyCentre::LoadScript((char*)pszFileName);
	}*/

	//这部分是按照按钮和弹出式菜单来做的，更新按钮和载入新配置
	KIniFile*	pSetting = g_UiBase.GetCommConfigFile();
	if (pSetting)
	{
		int		nCount;
		char	szKey[16], szBuff[128];

		pSetting->GetInteger("ShortcutSet", "Count", 0, &nCount);
		if (eSet < 0 || eSet >= nCount)
			eSet = 0;
		m_nShortcutSet = eSet;

		sprintf(szKey, "%d_Name", eSet);
		pSetting->GetString("ShortcutSet", szKey, "", szBuff, sizeof(szBuff));
		m_ShortcutSetView.SetLabel(szBuff);

		if (bOnlyUpdateUi == false)
		{
			sprintf(szKey, "%d_File", eSet);
			pSetting->GetString("ShortcutSet", szKey, "", szBuff, sizeof(szBuff));
			if (szBuff[0])
			{
				KShortcutKeyCentre::ClearScript();
				KShortcutKeyCentre::LoadScript(szBuff);
			}
		}
		g_UiBase.CloseCommConfigFile();
	}
}

// -------------------------------------------------------------------------
// 功能	: 弹出选择配置方案的菜单
// -------------------------------------------------------------------------
void KUiOptions::PopupSeleteSetMenu(int nX, int nY)
{
	KIniFile*	pSetting = g_UiBase.GetCommConfigFile();
	if (pSetting)
	{
		char	szKey[16];
		int		nCount, i;
		pSetting->GetInteger("ShortcutSet", "Count", 0, &nCount);
		if (nCount > 0)
		{
		    KPopupMenuData* pMenuData = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nCount));
			if (pMenuData)
			{
				KPopupMenu::InitMenuData(pMenuData, nCount);
				for (i = 0;i < nCount;i++)
				{
					sprintf(szKey, "%d_Name", i);
					pSetting->GetString("ShortcutSet", szKey, "", pMenuData->Items[i].szData, sizeof(pMenuData->Items[i].szData));
					pMenuData->Items[i].uDataLen = strlen(pMenuData->Items[i].szData);
				}
				pMenuData->nX = nX;
				pMenuData->nY = nY;
				KPopupMenu::Popup(pMenuData, (KWndWindow*)this, 1);
			}
    	}
		g_UiBase.CloseCommConfigFile();
	}
}


/*void KUiOptions::PopupSkinMenu()
{
	if (m_pSkinMenu)
		return;

	int nCount = g_UiBase.SchemeCount();
	if (nCount <= 0)
		return;
	m_pSkinMenu = (KPopupMenuData*)malloc(MENU_DATA_SIZE(nCount));
	if (m_pSkinMenu == NULL)
		return;

	KPopupMenu::InitMenuData(m_pSkinMenu, nCount);
	for (int i = 0; i < nCount; i++)
	{
		g_UiBase.GetScheme(i, m_pSkinMenu->Items[i].szData, NULL);
		m_pSkinMenu->Items[i].uDataLen = strlen(m_pSkinMenu->Items[i].szData);
	}
	int x, y;
	m_SkinBtn.GetPosition(&x, &y);
	m_pSkinMenu->nX = x + m_nAbsoluteLeft;
	m_pSkinMenu->nY = y + m_nAbsoluteTop;
	KPopupMenu::Popup(m_pSkinMenu, (KWndWindow*)this, 0);
}*/

// -------------------------------------------------------------------------
// 功能	: 响应滚动条被拖动
// -------------------------------------------------------------------------
void KUiOptions::OnScrollBarPosChanged(KWndWindow* pWnd, int nPos)
{
	if (pWnd == (KWndWindow*)&m_BGMValue)
		SetMusicValue(nPos);
	else if (pWnd == (KWndWindow*)&m_SoundValue)
		SetSoundValue(nPos);
	else if(pWnd == (KWndWindow*)&m_BrightnessScroll)
		SetBrightness(nPos);
	else if (pWnd == (KWndWindow*)&m_Scroll)
	{
		if (m_nToggleItemCount < m_nToggleItemCount &&
			nPos != m_nFirstControlableIndex &&
			nPos < m_nToggleItemCount && nPos >= 0)
		{
			m_nFirstControlableIndex = nPos;
			m_nToggleBtnValidCount = m_nToggleItemCount - m_nFirstControlableIndex;
			if (m_nToggleBtnValidCount > MAX_TOGGLE_BTN_COUNT)
				m_nToggleBtnValidCount = MAX_TOGGLE_BTN_COUNT;
			UpdateAllToggleBtn();
			UpdateAllStatusImg();
		}
	}
}

void KUiOptions::UpdateAllToggleBtn()
{
	int	i = 0;
	for (; i < m_nToggleBtnValidCount; i++)
	{
		m_ToggleBtn[i].Show();
		m_StatusImage[i].Show();
		int nIndex = m_nFirstControlableIndex + i;
		m_ToggleBtn[i].SetLabel(m_ToggleItemList[nIndex].szName);
		if (m_ToggleItemList[nIndex].bInvalid == false)
		{
			m_ToggleBtn[i].Enable(true);
			m_ToggleBtn[i].SetLabelColor(m_ToggleItemList[nIndex].bEnable ?
					m_uEnableTextColor : m_uDisableTextColor);
		}
		else
		{
			m_ToggleBtn[i].Enable(false);
			m_ToggleBtn[i].SetLabelColor(m_uInvalidTextColor);
		}
	}
	for (; i < MAX_TOGGLE_BTN_COUNT; i++)
	{
		m_ToggleBtn[i].Hide();
		m_StatusImage[i].Hide();
	}
}

void KUiOptions::UpdateAllStatusImg()
{
	int nFrame;
	for (int i = 0; i < m_nToggleBtnValidCount; i++)
	{
		if (m_ToggleItemList[m_nFirstControlableIndex + i].bInvalid == false)
		{
			nFrame = m_ToggleItemList[m_nFirstControlableIndex + i].bEnable ?
				m_nStatusEnableFrame : m_nStatusDisableFrame;
		}
		else
		{
			nFrame = m_nStatusInvalidFrame;
		}
		m_StatusImage[i].SetFrame(nFrame);
	}
}

// -------------------------------------------------------------------------
// 功能	: 更新数据
// -------------------------------------------------------------------------
void KUiOptions::LoadSetting(bool bReload, bool bUpdateOption)
{
	int	nBrightness  = 50;
	int	nSoundValue  = 100;
	int	nMusicValue  = 100;
	int nSettingSet  = 0;
	int i;

	int bOptionsEnable[OPTION_INDEX_COUNT] =
	{
		true, true, true
	};

	if (bReload == false && m_pSelf)
	{
		nBrightness  = m_pSelf->m_nBrightness;
		nSoundValue  = m_pSelf->m_nSoundValue;
		nMusicValue  = m_pSelf->m_nMusicValue;
		nSettingSet  = m_pSelf->m_nShortcutSet;

		for (i = 0; i < OPTION_INDEX_COUNT; i++)
			bOptionsEnable[i] = m_pSelf->m_ToggleItemList[i].bEnable;
	}
	else
	{
		KIniFile* pSetting = g_UiBase.GetCommSettingFile();
		if (pSetting)
		{
			pSetting->GetInteger(OPTIONS_SAVE_SECTION, "Brightness", 50, &nBrightness);
			pSetting->GetInteger(OPTIONS_SAVE_SECTION, "MusicValue", 100, &nMusicValue);
			pSetting->GetInteger(OPTIONS_SAVE_SECTION, "SoundValue", 100, &nSoundValue);
			pSetting->GetInteger(OPTIONS_SAVE_SECTION, "ShortcutSet", 0, &nSettingSet);
			for (i = 0; i < OPTION_INDEX_COUNT; i++)
				pSetting->GetInteger(OPTIONS_SAVE_SECTION, ls_ToggleOptionName[i], true, &bOptionsEnable[i]);
			g_UiBase.CloseCommSettingFile(false);
		}
	}
	if (g_bRepresent3 == false)
	{
		bOptionsEnable[OPTION_I_DYNALIGHT] = false;
		bOptionsEnable[OPTION_I_PERSPECTIVE] = false;
	}

	if (bUpdateOption && g_pCoreShell)
	{
		if (g_bRepresent3)
		{
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_PERSPECTIVE, bOptionsEnable[OPTION_I_PERSPECTIVE]);
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DYNALIGHT, bOptionsEnable[OPTION_I_DYNALIGHT]);
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_BRIGHTNESS, nBrightness);
		}

		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_MUSIC_VALUE, nMusicValue);
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SOUND_VALUE, nSoundValue);
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_WEATHER, bOptionsEnable[OPTION_I_WEATHER]);
	}

	if (m_pSelf)
	{
		if (bReload)
		{
			for (i = 0; i < OPTION_INDEX_COUNT; i++)
				m_pSelf->m_ToggleItemList[i].bEnable = bOptionsEnable[i];
			m_pSelf->m_nBrightness = nBrightness;
			m_pSelf->m_nSoundValue = nSoundValue;
			m_pSelf->m_nMusicValue = nMusicValue;
			m_pSelf->m_nShortcutSet = nSettingSet;
			m_pSelf->m_BrightnessScroll.SetScrollPos(nBrightness);
			m_pSelf->m_BGMValue.SetScrollPos(nMusicValue);
			m_pSelf->m_SoundValue.SetScrollPos(nSoundValue);
			m_pSelf->UpdateSettingSet(nSettingSet, true);
			m_pSelf->UpdateAllToggleBtn();
			m_pSelf->UpdateAllStatusImg();
		}

		m_pSelf->m_BrightnessScroll.Enable(g_bRepresent3 && g_bScreen);
	}
}

void KUiOptions::StoreSetting()
{
	KIniFile* pSetting = NULL;
	pSetting = g_UiBase.GetCommSettingFile();

	if (pSetting)
	{
		for (int i = 0; i < OPTION_INDEX_COUNT; i++)
		{
			pSetting->WriteInteger(OPTIONS_SAVE_SECTION, ls_ToggleOptionName[i],
				m_ToggleItemList[i].bEnable);
		}
		pSetting->WriteInteger(OPTIONS_SAVE_SECTION, "Brightness", m_nBrightness);
		pSetting->WriteInteger(OPTIONS_SAVE_SECTION, "MusicValue", m_nMusicValue);
		pSetting->WriteInteger(OPTIONS_SAVE_SECTION, "SoundValue", m_nSoundValue);
		pSetting->WriteInteger(OPTIONS_SAVE_SECTION, "ShortcutSet", m_nShortcutSet);
	}
	g_UiBase.CloseCommSettingFile(true);
}

//切换开关型选项
void KUiOptions::ToggleOption(int nIndex)
{
	if (nIndex < OPTION_I_START || nIndex >= OPTION_INDEX_COUNT)
		return;
	if (m_ToggleItemList[nIndex].bInvalid)
		return;
	bool bEnable = m_ToggleItemList[nIndex].bEnable = !m_ToggleItemList[nIndex].bEnable;
	switch(nIndex)
	{
	case OPTION_I_DYNALIGHT:	//动态光影
		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_DYNALIGHT, bEnable);
		break;
	case OPTION_I_WEATHER:		//天气开关
		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_WEATHER, bEnable);
		break;
	case OPTION_I_PERSPECTIVE:	//透视模式
		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_PERSPECTIVE, bEnable);
		break;
	}
	UpdateAllStatusImg();
	UpdateAllToggleBtn();
}

void KUiOptions::SetMusicValue(int n)
{
	if (m_nMusicValue != n)
	{
		m_nMusicValue = n;
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_MUSIC_VALUE, m_nMusicValue);
	}
}

void KUiOptions::SetSoundValue(int n)
{
	if (m_nSoundValue != n)
	{
		m_nSoundValue = n;
		g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_SOUND_VALUE, m_nSoundValue);
	}
}

void KUiOptions::SetBrightness(int n)
{
	if (g_bRepresent3)
	{
		if (m_nBrightness != n)
		{
			m_nBrightness = n;
			g_pCoreShell->OperationRequest(GOI_OPTION_SETTING, OPTION_BRIGHTNESS, m_nBrightness);
		}
	}
}