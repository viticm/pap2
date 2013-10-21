/*
 * File:     UiWorldMap.cpp
 * Desc:     世界地图
 * Author:   flying
 * Creation: 2003/7/22
 */
//-----------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
//#include "../ShortcutKey.h"
#include "UiWorldMap.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../../../core/src/CoreShell.h"
#include "../../../core/src/GameDataDef.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*		g_pCoreShell;

KUiWorldmap* KUiWorldmap::m_pSelf = NULL;

#define		SCHEME_INI_WORLD		"小地图_世界地图版.ini"
#define		WORLD_MAP_INFO_FILE		"\\Settings\\MapList.ini"

void MapToggleStatus();

KUiWorldmap* KUiWorldmap::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiWorldmap;
		if (m_pSelf)
			m_pSelf->Initialize();
	}

	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData();
		m_pSelf->Show();
		m_pSelf->BringToTop();
		Wnd_SetExclusive(m_pSelf);
	}
	return m_pSelf;
}

void KUiWorldmap::CloseWindow()
{
	if (m_pSelf)
	{
		Wnd_ReleaseExclusive(m_pSelf);
		m_pSelf->Destroy();
		m_pSelf = NULL;
		MapToggleStatus();
	}
}

KUiWorldmap* KUiWorldmap::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}

//初始化
void KUiWorldmap::Initialize()
{
	AddChild(&m_Sign);

	char szBuffer[128];
	g_UiBase.GetCurSchemePath(szBuffer, sizeof(szBuffer));
	strcat(szBuffer, "\\"SCHEME_INI_WORLD);
	KIniFile	Ini;
	if (Ini.Load(szBuffer))
	{
		Init(&Ini, "WorldMap");
		m_Sign.Init(&Ini, "Sign");
	}

	Wnd_AddWindow(this, WL_TOPMOST);
	return;
}

int KUiWorldmap::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;

	switch(uMsg)
	{
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_KEYDOWN:
		CloseWindow();
		nResult = true;
		break;
	default:
		nResult = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nResult;
}

void KUiWorldmap::UpdateData()
{
	m_Sign.Hide();

	if (g_pCoreShell)
	{
		KIniFile	Ini;
		if (Ini.Load(WORLD_MAP_INFO_FILE))
		{
			//取得世界地图的图形文件明
			char	szBuffer[128];
			if (Ini.GetString("List", "WorldMapImage", "", szBuffer, sizeof(szBuffer)))
			{
				SetImage(ISI_T_BITMAP16, szBuffer, true);

				int nAreaX = -1, nAreaY = 0;
				KUiSceneTimeInfo Info;
				g_pCoreShell->SceneMapOperation(GSMOI_SCENE_TIME_INFO, (unsigned int)&Info, 0);
				sprintf(szBuffer, "%d_MapPos", Info.nSceneId);
				Ini.GetInteger2("List", szBuffer, &nAreaX, &nAreaY);
				if (nAreaX != -1)
				{
					int nWidth, nHeight;
					m_Sign.GetSize(&nWidth,  &nHeight);
					m_Sign.SetPosition(nAreaX - nWidth / 2,
						nAreaY - nHeight / 2);
					m_Sign.Show();
				}
			}
		}
	}
}

//活动函数
void KUiWorldmap::Breathe()
{
	if (m_Sign.IsVisible())
		m_Sign.NextFrame();
}