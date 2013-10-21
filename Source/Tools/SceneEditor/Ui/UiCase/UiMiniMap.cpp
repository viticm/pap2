/*****************************************************************************************
//	界面--小地图
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-21
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/Wnds.h"
#include "../Elem/WndMessage.h"
#include "UiMiniMap.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"
#include "UiWorldMap.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/gamedatadef.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../Elem/TextPic.h"
#include <time.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell*			g_pCoreShell;
extern IInlinePicEngineSink *g_pIInlinePicSink;


#define	SCHEME_INI_SMALL	"小地图_小.ini"
#define	SCHEME_INI_BIG		"小地图_浏览版.ini"
#define SCHEME_INI_MINIMAP_NOTHING "小地图之虚无缥缈.ini"

#define MINIMAP_TRIFFIC_FILE "\\settings\\MapTraffic.ini"

//地图拖动后还原的时间(秒)
#define MAP_RECOVER_TIME		5000
#define	NOT_DRAGING_MAP			-1
#define	WAIT_TO_BE_SET_BACK		-2
#define	IS_DRAGING_MAP			(m_nCtrlMode & enumMCM_SCROLL)
#define	IS_WAIT_TO_SET_BACK		(m_nCtrlMode & enumMCM_WAITING_TO_SET_BACK)

//交通相关
#define TRAFFIC_POINT_RADIUS	20

const char*	s_ModeName[MINIMAP_M_COUNT] =
{
	"标准小地图",
	"标准大地图",
	"世界大地图",
	" 关闭地图 ",
};

KUiMiniMap*			KUiMiniMap::ms_pSelf = NULL;
BOOL                KUiMiniMap::m_bIsShowMapDetail = TRUE;
int					KUiMiniMap::m_nRealMapIndex = 0;
int					KUiMiniMap::m_nCurrentMapIndex = 0;
int					KUiMiniMap::m_nbInTrafficMode = 0;

static MINIMAP_MODE	s_eMapMode = MINIMAP_M_NONE;
static MINIMAP_MODE	s_eMapOldMode = MINIMAP_M_BRIEF_PIC;

KUiMiniMap::KUiMiniMap()
{
	m_OldPos.x = NOT_DRAGING_MAP;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiMiniMap* KUiMiniMap::OpenWindow()
{
	if (ms_pSelf == NULL)
	{
		ms_pSelf = new KUiMiniMap;
		if (ms_pSelf)
			ms_pSelf->Initialize();
	}
	if (ms_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		ms_pSelf->Show();
		ms_pSelf->BringToTop();
		ms_pSelf->LoadTrafficData();
		ms_pSelf->m_nDefaultCursor = Wnd_GetCurrentCursor();
	}
	return ms_pSelf;
}

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiMiniMap* KUiMiniMap::GetIfVisible()
{
	if (ms_pSelf && ms_pSelf->IsVisible())
		return ms_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiMiniMap::Initialize()
{
	AddChild(&m_SceneNameBG);
	AddChild(&m_Shadow);
	AddChild(&m_SceneName);
	AddChild(&m_SwitchBtn);
	AddChild(&m_WorldMapBtn);
	AddChild(&m_ScenePos);
	AddChild(&m_BtnFlag);
	m_Style &= ~WND_S_VISIBLE;
	m_FlagPos.x = m_FlagPos.y = -1;

	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_NORMAL);
}

//--------------------------------------------------------------------------
//	功能：关闭窗口
//--------------------------------------------------------------------------
void KUiMiniMap::CloseWindow()
{
	if (ms_pSelf)
	{
		ms_pSelf->StopScrollMap();
		ms_pSelf->MapMoveBack();
		if (g_pCoreShell)
			g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING, SCENE_PLACE_MAP_ELEM_NONE, 0);
		ms_pSelf->Destroy();
		ms_pSelf = NULL;
	}
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiMiniMap::LoadScheme(const char* pScheme)
{
	if (ms_pSelf)
	{
		char		szBuf[128];
		KIniFile	Ini;
		if(m_bIsShowMapDetail)
		{
			sprintf(szBuf, "%s\\%s", pScheme,
				s_eMapMode == MINIMAP_M_BRIEF_PIC ? SCHEME_INI_SMALL : SCHEME_INI_BIG);
		}
		else
		{
			sprintf(szBuf, "%s\\%s", pScheme, SCHEME_INI_MINIMAP_NOTHING);
		}
		if (Ini.Load(szBuf))
		{
			ms_pSelf->LoadScheme(&Ini);
		}
	}
}

void KUiMiniMap::LoadScheme(KIniFile* pIni)
{
	if (pIni == NULL)
		return;
	Init(pIni, "MiniMap");
	m_Shadow.Init(pIni, "NameShadow");
	m_SceneName.Init(pIni, "SceneName");
	m_SceneNameBG.Init(pIni, "SceneName");
	m_ScenePos.Init(pIni, "ScenePos");
	m_SwitchBtn.Init(pIni, "SwitchBtn");
	m_WorldMapBtn.Init(pIni, "WorldMapBtn");
	m_BtnFlag.Init(pIni, "BtnFlag");

	m_SceneNameBG.SetText(" ");
	pIni->GetInteger("MapRect", "Left",   0, (int*)&m_MapPos.x);
	pIni->GetInteger("MapRect", "Top",    0, (int*)&m_MapPos.y);
	pIni->GetInteger("MapRect", "Width",  0, (int*)&m_MapSize.cx);
	pIni->GetInteger("MapRect", "Height", 0, (int*)&m_MapSize.cy);

	IR_InitUiImageRef(m_FlagImage);
	pIni->GetString("BtnFlag", "FlagImage", "", m_FlagImage.szImage, sizeof(m_FlagImage.szImage));
	m_FlagImage.bRenderFlag = RUIMAGE_RENDER_FLAG_REF_SPOT;
	m_FlagImage.nType = ISI_T_SPR;
	m_FlagImage.nFlipTime  = 0;
	m_FlagImage.nInterval  = 0;
	m_FlagImage.nNumFrames = 0;
	m_FlagImage.Color.Color_b.r = 0;
	m_FlagImage.Color.Color_b.g = 0;
	m_FlagImage.Color.Color_b.b = 0;
	m_FlagImage.Color.Color_b.a = 255;

	if (IsVisible() && g_pCoreShell)
	{
		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING,
			SCENE_PLACE_MAP_ELEM_PIC | SCENE_PLACE_MAP_ELEM_CHARACTER | SCENE_PLACE_MAP_ELEM_PARTNER,
			(m_MapSize.cx | (m_MapSize.cy << 16)));
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiMiniMap::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_SwitchBtn)
			MapSetMode(s_eMapMode == MINIMAP_M_BRIEF_PIC ? MINIMAP_M_BRIEF_PIC_BROWSE : MINIMAP_M_BRIEF_PIC);
		else if (uParam == (unsigned int)(KWndWindow*)&m_WorldMapBtn)
			MapSetMode(MINIMAP_M_WORLD_MAP);
		else if(uParam == (unsigned int)&m_BtnFlag)
		{
			m_FlagPos.x = m_FlagPos.y = -1;
			if(m_nCtrlMode & enumMCM_FLAG)
			{
				m_nCtrlMode &= ~enumMCM_FLAG;
			}
			else
			{
				m_nCtrlMode |= enumMCM_FLAG;
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			if(m_nCtrlMode & enumMCM_FLAG)
			{
				SetFlag(GET_X_LPARAM(nParam) - m_nAbsoluteLeft, GET_Y_LPARAM(nParam) - m_nAbsoluteTop);
				m_nCtrlMode &= ~enumMCM_FLAG;
			}
			else if(!DoTransfer())
			{
				Wnd_TransmitInputToGameSpace(uMsg, uParam, nParam);
			}
		}
		break;

	case WM_LBUTTONUP:
	case WM_LBUTTONDBLCLK:
		Wnd_TransmitInputToGameSpace(uMsg, uParam, nParam);
		break;
	case WND_N_CHILD_MOVE:
		if (uParam == (unsigned int)(KWndWindow*)&m_SceneName ||
			uParam == (unsigned int)(KWndWindow*)&m_ScenePos ||
			uParam == (unsigned int)(KWndWindow*)&m_SceneNameBG ||
			uParam == (unsigned int)(KWndWindow*)&m_Shadow)
		{
			KWndWindow* pWnd = (KWndWindow*)uParam;
			int nMoveOffsetX = GET_X_LPARAM(nParam);
			int nMoveOffsetY = GET_Y_LPARAM(nParam);

			int x, y;
			pWnd->GetPosition(&x, &y);
			x -= nMoveOffsetX;
			y -= nMoveOffsetY;
			pWnd->SetPosition(x, y);	//按钮跟着窗口变化，所以要先变回原样
			GetPosition(&x, &y);
			x += nMoveOffsetX;
			y += nMoveOffsetY;
			SetPosition(x, y);
		}
		break;
	case WM_RBUTTONDOWN:
		Wnd_SetCapture(this);
		Wnd_GetCursorPos((int *)&m_OldPos.x, (int *)&m_OldPos.y);
		m_nCtrlMode |= enumMCM_SCROLL;
		m_nCtrlMode &= ~enumMCM_WAITING_TO_SET_BACK;
		break;
	case WM_MOUSEHOVER:
	case WM_MOUSEMOVE:
		if(IS_DRAGING_MAP && (uParam & MK_RBUTTON))
		{
			int nCursorX, nCursorY;
			Wnd_GetCursorPos(&nCursorX, &nCursorY);
			m_nOffsetX = m_OldPos.x - nCursorX;
			m_nOffsetY = m_OldPos.y - nCursorY;
			m_OldPos.x   =  nCursorX;
			m_OldPos.y   =  nCursorY;
			MapScroll(uParam & MK_CONTROL);
		}
		else if(!HandleCursor(GET_X_LPARAM(nParam) - m_nAbsoluteLeft, GET_Y_LPARAM(nParam) - m_nAbsoluteTop))
		{
			Wnd_TransmitInputToGameSpace(uMsg, uParam, nParam);
		}
		break;
	case WM_RBUTTONUP:
		if (IS_DRAGING_MAP && ((uParam & MK_CONTROL) == 0) && g_pCoreShell)
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, 0);
		StopScrollMap();
		break;

	case WND_M_MOUSE_ENTER:
		m_nDefaultCursor = Wnd_GetCurrentCursor();
		break;

	default:
		nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	}
	return nRet;
}

//停止拖动地图
void KUiMiniMap::StopScrollMap()
{
	if (IS_DRAGING_MAP)
	{
		m_uLastScrollTime = IR_GetCurrentTime();
		Wnd_ReleaseCapture();
		m_nCtrlMode &= ~enumMCM_SCROLL;
		m_nCtrlMode |= enumMCM_WAITING_TO_SET_BACK;
	}
}

//地图还原原始偏移
void KUiMiniMap::MapMoveBack()
{
	m_nCtrlMode &= ~enumMCM_SCROLL;
	m_nCtrlMode &= ~enumMCM_WAITING_TO_SET_BACK;
	m_nOffsetX = 0;
	m_nOffsetY = 0;
	if (g_pCoreShell && m_nRealMapIndex == m_nCurrentMapIndex)
	{
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, 0);
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, 0, 0);
	}
}

//绘制窗口
void KUiMiniMap::PaintWindow()
{
	KWndImage::PaintWindow();
	if (g_pCoreShell)
	{
		int nX = m_MapPos.x + m_nAbsoluteLeft;
		int nY = m_MapPos.y + m_nAbsoluteTop;
		g_pCoreShell->SceneMapOperation(GSMOI_PAINT_SCENE_MAP, nX, nY);

		if (g_pRepresentShell)
		{
			KRURect	rect;
			rect.Color.Color_dw = 0xff000000;
			rect.oEndPos.nX = rect.oPosition.nX = nX - 1;
			rect.oEndPos.nY = rect.oPosition.nY = nY - 1;
			rect.oEndPos.nZ = rect.oPosition.nZ = 0;
			rect.oEndPos.nX += m_MapSize.cx;
			rect.oEndPos.nY += m_MapSize.cy;
			g_pRepresentShell->DrawPrimitives(1, &rect, RU_T_RECT, true);

			if(m_nCtrlMode & enumMCM_FLAG)
			{
				int nCursorX, nCursorY;
				Wnd_GetCursorPos(&nCursorX, &nCursorY);
				if(KWndWindow::PtInWindow(nCursorX, nCursorY))
				{
					m_FlagImage.oPosition.nX = nCursorX;
					m_FlagImage.oPosition.nY = nCursorY;
					g_pRepresentShell->DrawPrimitives(1, &m_FlagImage, RU_T_IMAGE, TRUE);
				}
			}
			else
			{
				if(m_FlagPos.x != -1 && m_FlagPos.y != -1)
				{
					KSceneMapInfo LittleMapInfo;
					g_pCoreShell->SceneMapOperation(GSMOI_SCENE_LITTLE_MAP_INFO, (unsigned int)&LittleMapInfo, 0);

					if(abs(m_FlagPos.x - LittleMapInfo.nOrigFocusH) < m_MapSize.cx / 2 &&
						abs(m_FlagPos.y - LittleMapInfo.nOrigFocusV) < m_MapSize.cy / 2)
					{
						int nFlagX, nFlagY;
						nFlagX = m_FlagPos.x - LittleMapInfo.nOrigFocusH + m_MapSize.cx / 2;
						nFlagY = m_FlagPos.y - LittleMapInfo.nOrigFocusV + m_MapSize.cy / 2;
						if(nFlagX + 14 < m_MapSize.cx && nFlagY + 14 < m_MapSize.cy)
						{
							m_FlagImage.oPosition.nX = nFlagX + m_nAbsoluteLeft;
							m_FlagImage.oPosition.nY = nFlagY + m_nAbsoluteTop;
							g_pRepresentShell->DrawPrimitives(1, &m_FlagImage, RU_T_IMAGE, TRUE);
						}
					}
				}
			}
		}
	}
}

void KUiMiniMap::Breathe()
{
	if (IS_WAIT_TO_SET_BACK && g_pCoreShell &&
		IR_IsTimePassed(MAP_RECOVER_TIME, m_uLastScrollTime))
	{
		MapMoveBack();
	}
}

//活动函数
void KUiMiniMap::UpdateSceneTimeInfo(KUiSceneTimeInfo* pInfo)
{
	if (ms_pSelf && pInfo)
	{
		ms_pSelf->m_SceneName.SetText(pInfo->szSceneName);
		if(!(ms_pSelf->m_nCtrlMode & (int)enumMCM_FLAG))
		{
			ms_pSelf->m_ScenePos.Set2IntText(pInfo->nScenePos0 / 8, pInfo->nScenePos1 / 8, '/');
		}
	}
}

//显示窗口
void KUiMiniMap::Show()
{
	KWndImage::Show();
	if (g_pCoreShell)
	{
		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING,
			SCENE_PLACE_MAP_ELEM_PIC | SCENE_PLACE_MAP_ELEM_CHARACTER | SCENE_PLACE_MAP_ELEM_PARTNER,
			(ms_pSelf->m_MapSize.cx | (ms_pSelf->m_MapSize.cy << 16)));
	}
}

//隐藏窗口
void KUiMiniMap::Hide()
{
	KWndImage::Hide();
	if (g_pCoreShell)
		g_pCoreShell->SceneMapOperation(GSMOI_IS_SCENE_MAP_SHOWING, SCENE_PLACE_MAP_ELEM_NONE, 0);
}

//地图卷动
void KUiMiniMap::MapScroll(int nbScrollScene)
{
	KSceneMapInfo MapInfo, LittleMapInfo;
    if(g_pCoreShell && g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_INFO, (unsigned int)&MapInfo, 0) &&
	   g_pCoreShell->SceneMapOperation(GSMOI_SCENE_LITTLE_MAP_INFO, (unsigned int)&LittleMapInfo, 0))
	{
		int nX, nY, nXInterval, nYInterval;

		if(m_nbInTrafficMode)
		{
			nX = (LittleMapInfo.nOrigFocusH + m_nOffsetX) * MapInfo.nScallH;
			nY = (LittleMapInfo.nOrigFocusV + m_nOffsetY) * MapInfo.nScallV;

			LittleMapInfo.nFocusMinH *= MapInfo.nScallH;
			LittleMapInfo.nFocusMinV *= MapInfo.nScallV;
//			LittleMapInfo.nFocusMaxH = MapInfo.nFocusMaxH / MapInfo.nScallH;
//			LittleMapInfo.nFocusMaxV = MapInfo.nFocusMaxV / MapInfo.nScallV;

			if (nX < LittleMapInfo.nFocusMinH)
				nX = LittleMapInfo.nFocusMinH;
//			else if (nX > LittleMapInfo.nFocusMaxH)
//				nX = LittleMapInfo.nFocusMaxH;
			if (nY < LittleMapInfo.nFocusMinV)
				nY = LittleMapInfo.nFocusMinV;
//			else if (nY > LittleMapInfo.nFocusMaxV)
//				nY = LittleMapInfo.nFocusMaxV;
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_LITTLE_MAP_FOCUS, (unsigned int)nX, nY);
		}
		else
		{
			nX = MapInfo.nOrigFocusH + MapInfo.nFocusOffsetH + MapInfo.nScallH * m_nOffsetX;
			nY = MapInfo.nOrigFocusV + MapInfo.nFocusOffsetV + MapInfo.nScallV * m_nOffsetY;

			if (nX < MapInfo.nFocusMinH)
				nX = MapInfo.nFocusMinH;
			else if (nX > MapInfo.nFocusMaxH)
				nX = MapInfo.nFocusMaxH;
			if (nY < MapInfo.nFocusMinV)
				nY = MapInfo.nFocusMinV;
			else if (nY > MapInfo.nFocusMaxV)
				nY = MapInfo.nFocusMaxV;

			nXInterval = nX - MapInfo.nOrigFocusH;
			nYInterval = nY - MapInfo.nOrigFocusV;

			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, nbScrollScene);
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, (unsigned int)nXInterval, nYInterval);
		}
	}
}


//设置小地图的焦点
void KUiMiniMap::SetMapFocus(int nX, int nY)
{
	KSceneMapInfo MapInfo, LittleMapInfo;
    if(g_pCoreShell && g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_INFO, (unsigned int)&MapInfo, 0) &&
	   g_pCoreShell->SceneMapOperation(GSMOI_SCENE_LITTLE_MAP_INFO, (unsigned int)&LittleMapInfo, 0))
	{
		int nIntervalX = (nX + LittleMapInfo.nFocusMinH) * MapInfo.nScallH;
		int nIntervalY = (nY + LittleMapInfo.nFocusMinV) * MapInfo.nScallV;

		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_FOLLOW_WITH_MAP, 0, 0);
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, 0, 0);
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_LITTLE_MAP_FOCUS, (unsigned int)nIntervalX, nIntervalY);
	}
}


void MapToggleStatus()
{
	if (s_eMapMode == MINIMAP_M_NONE || s_eMapMode == MINIMAP_M_WORLD_MAP)
		MapSetMode(s_eMapOldMode);
	else
		MapSetMode(MINIMAP_M_NONE);
}


//设置该大地图显不显示详细小地图
void KUiMiniMap::SetIsShowMapDetail(int nbIsDetail)
{
	if(nbIsDetail)
	{
		m_bIsShowMapDetail = TRUE;
	}
	else
	{
		m_bIsShowMapDetail = FALSE;
	}
	char Scheme[128];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
}


//设置显示内容模式
void MapSetMode(MINIMAP_MODE eMode)
{
	if (eMode == s_eMapMode &&
		(eMode < MINIMAP_M_NONE || eMode > MINIMAP_M_WORLD_MAP))
		return;

	if (s_eMapMode != MINIMAP_M_WORLD_MAP)
		s_eMapOldMode = s_eMapMode;
	switch(s_eMapMode = eMode)
	{
	case MINIMAP_M_NONE:			//不显示地图
		KUiMiniMap::CloseWindow();
		KUiWorldmap::CloseWindow();
		break;
	case MINIMAP_M_BRIEF_PIC:		//缩略图
	case MINIMAP_M_BRIEF_PIC_BROWSE://缩略图浏览/放大模式
		KUiWorldmap::CloseWindow();
		if (KUiMiniMap::GetIfVisible() == NULL)
			KUiMiniMap::OpenWindow();
		else
		{
			char Scheme[128];
			g_UiBase.GetCurSchemePath(Scheme, 256);
			KUiMiniMap::LoadScheme(Scheme);
		}
		break;
	case MINIMAP_M_WORLD_MAP:		//世界地图
		KUiMiniMap::CloseWindow();
		KUiWorldmap::OpenWindow();
		break;
	}
}


//功能：小地图区域子窗口左上角为0点
//      把当前的鼠标光标坐标转换为在整张小地图中的象素坐标
  /*__(@_
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
int KUiMiniMap::PosToPhotoPos(int *pX, int *pY)
{
	KSceneMapInfo MapInfo;
    if(g_pCoreShell && g_pCoreShell->SceneMapOperation(GSMOI_SCENE_LITTLE_MAP_INFO, (unsigned int)&MapInfo, 0))
	{
		*pX = MapInfo.nOrigFocusH - MapInfo.nFocusMinH - m_MapSize.cx / 2 + *pX;
		*pY = MapInfo.nOrigFocusV - MapInfo.nFocusMinV - m_MapSize.cy / 2 + *pY;
		return 1;
	}
	return 0;
}


  /*__(@_          功能：对小地图施展移形换影大法
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
int	KUiMiniMap::DoTransfer()
{
	if(m_pTrafficNow && g_pCoreShell)
	{
		m_nCurrentMapIndex = m_pTrafficNow->TargetIndex;
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_CHANGE_MAP, 0, m_nCurrentMapIndex);
		SetMapFocus(m_pTrafficNow->TargetPoint.x, m_pTrafficNow->TargetPoint.y);
		LoadTrafficData();
		m_nbInTrafficMode = TRUE;
		return 1;
	}
	return 0;
}


  /*__(@_            功能：处理鼠标移动和HOVER
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
int	KUiMiniMap::HandleCursor(int nX, int nY)
{
	GetCursorPosOnMap(&nX, &nY);
	PosToPhotoPos(&nX, &nY);
	KMinimapTraffic *pNode = m_Traffic.Begin();
	int nCount = m_Traffic.GetCount();
	int nFlag = 0;

	for(int i = 0;i < nCount;i++)
	{
		if(pNode->TrafficPoint.x - TRAFFIC_POINT_RADIUS < nX &&
		   pNode->TrafficPoint.x + TRAFFIC_POINT_RADIUS > nX &&
		   pNode->TrafficPoint.y - TRAFFIC_POINT_RADIUS < nY &&
		   pNode->TrafficPoint.y + TRAFFIC_POINT_RADIUS > nY)
		{
			nFlag = 1;
			break;
		}
		pNode = m_Traffic.NextItem();
	}
	if((m_nCtrlMode & enumMCM_FLAG) && g_pCoreShell)
	{
		KSceneMapInfo LittleMapInfo;
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_LITTLE_MAP_INFO, (unsigned int)&LittleMapInfo, 0);
		m_ScenePos.Set2IntText((nX + LittleMapInfo.nFocusMinH) / 16, (nY + LittleMapInfo.nFocusMinV) / 16, '/');
	}
	if(nFlag)
	{
		m_pTrafficNow = pNode;
		Wnd_SwitchCursor(CURSOR_PICKABLE);
		return 1;
	}
	else
	{
		m_pTrafficNow = NULL;
		Wnd_SwitchCursor(m_nDefaultCursor);
		return 0;
	}
}


  /*__(@_功能：通过小地图窗口坐标计算以小地图区域子窗口左上角为0点的坐标
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/               值为象素，参数为in out
      (*/
int	KUiMiniMap::GetCursorPosOnMap(int *pX, int *pY)
{
	*pX -= m_MapPos.x;
	*pY -= m_MapPos.y;
	return 1;
}


  /*__(@_  功能：在掌管天地五行推断值的天书上边,拿到意志所在地域的五行推断值
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/                  (载入交通数据)
      (*/
void KUiMiniMap::LoadTrafficData()
{
	KIniFile Ini;

	if(Ini.Load(MINIMAP_TRIFFIC_FILE))
	{
		char szKey[32];
		char szSection[16], szBuf[16];
		KMinimapTraffic Traffic;
		int  nHereX, nHereY, nThereX, nThereY, nThereIndex;

		ReleaseTrafficData();
		itoa(m_nCurrentMapIndex, szSection, 10);
		for(int i = 1;;i++)
		{
			itoa(i, szBuf, 10);
			sprintf(szKey, "%s_Point", szBuf);
			if(!Ini.GetInteger2(szSection, szKey, &nHereX, &nHereY))
			{
				break;
			}
			sprintf(szKey, "%s_TargetPoint", szBuf);
			Ini.GetInteger2(szSection, szKey, &nThereX, &nThereY);
			sprintf(szKey, "%s_Index", szBuf);
			Ini.GetInteger(szSection, szKey, 0, &nThereIndex);
			Traffic.TrafficPoint.x = nHereX;
			Traffic.TrafficPoint.y = nHereY;
			Traffic.TargetPoint.x  = nThereX;
			Traffic.TargetPoint.y  = nThereY;
			Traffic.TargetIndex    = nThereIndex;
			m_Traffic.AddItem(Traffic);
		}
	}
}


  /*__(@_                 功能：重置交通数据
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiMiniMap::ReleaseTrafficData()
{
	m_Traffic.Clear();
	m_pTrafficNow = NULL;
}


  /*__(@_ 功能：设置当前角色的真身所在的地域的大五行推断值(编号)
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiMiniMap::HandleMapInfo(int nIndex)
{
	m_nRealMapIndex = g_pCoreShell->SceneMapOperation(GSMOI_GET_SCENE_ID, 0, 0);
	m_nCurrentMapIndex = nIndex;
	if(ms_pSelf)
	{
		ms_pSelf->LoadTrafficData();
		ms_pSelf->m_FlagPos.x = ms_pSelf->m_FlagPos.y = -1;
	}
}


  /*__(@_         功能：把小地图设回正确的地图和坐标
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiMiniMap::RestoreMapPos()
{
	if(m_nbInTrafficMode)
	{
		m_nbInTrafficMode = FALSE;
		m_nCurrentMapIndex = m_nRealMapIndex;
		if(g_pCoreShell)
		{
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_CHANGE_MAP, 0, -1);
			g_pCoreShell->SceneMapOperation(GSMOI_SCENE_MAP_FOCUS_OFFSET, 0, 0);
		}
		if(ms_pSelf)
			ms_pSelf->LoadTrafficData();
	}
}


  /*__(@_		           功能：设置小旗
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiMiniMap::SetFlag(int nX, int nY)
{
	if(g_pCoreShell)
	{
		KSceneMapInfo LittleMapInfo;
		g_pCoreShell->SceneMapOperation(GSMOI_SCENE_LITTLE_MAP_INFO, (unsigned int)&LittleMapInfo, 0);

		GetCursorPosOnMap(&nX, &nY);
		PosToPhotoPos(&nX, &nY);

		m_FlagPos.x = nX + LittleMapInfo.nFocusMinH;
		m_FlagPos.y = nY + LittleMapInfo.nFocusMinV;
	}
}
