/*****************************************************************************************
//	界面--聊天窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-27
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiSkills.h"
#include "UiSysMsgCentre.h"
#include "../../../core/src/coreshell.h"
#include "../UiBase.h"
#include "../UiSoundSetting.h"

#include "../../../Represent/iRepresent/iRepresentShell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;

extern iCoreShell*		g_pCoreShell;

#define 	SCHEME_INI_SHEET			"技能主窗口.ini"
#define 	SCHEME_INI_LIVE				"生活技能分页.ini"
#define 	SCHEME_INI_FIGHT			"战斗技能分页.ini"
#define 	SCHEME_INI_FIGHT_SUB_PAGE	"战斗技能细分页.ini"
#define		SET_NEW_SKILL_TO_IMMED_SKILL_LEVEL_RANGE	9
#define		AUTO_SET_IMMED_SKILL_MSG_ID					"24"

KUiFightSkillSubPage::KUiFightSkillSubPage()
{
	m_nRemainSkillPoint = 0;
//	m_nSubPagIndex = 0;
}

//初始化
void KUiFightSkillSubPage::Initialize(/*int nSubPageIndex*/)
{
	for (int i = 0; i < FIGHT_SKILL_COUNT_PER_PAGE; i ++)
	{
		AddChild(&m_FightSkills[i]);
		m_FightSkills[i].Celar();
		m_FightSkills[i].SetContainerId((int)UOC_SKILL_LIST);
	}
//	m_nSubPagIndex = nSubPageIndex;
}

//载入界面方案
void KUiFightSkillSubPage::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\"SCHEME_INI_FIGHT_SUB_PAGE, pScheme);
	if (Ini.Load(Buff))
	{
		KWndPage::Init(&Ini, "Main");
		for (int i = 0; i < FIGHT_SKILL_COUNT_PER_PAGE; i++)
		{
			sprintf(Buff, "Skill_%d", i);
			m_FightSkills[i].Init(&Ini, Buff);
			m_FightSkills[i].EnablePickPut(false);
		}

		Ini.GetInteger("SkillText", "Font", 12, &m_SkillTextParam.nFont);
		Ini.GetInteger2("SkillText", "Offset",
			(int*)&m_SkillTextParam.Offset.cx, (int*)&m_SkillTextParam.Offset.cy);
		Ini.GetString("SkillText", "Color", "", Buff, 16);
		m_SkillTextParam.uColorNormal = GetColor(Buff);
		Ini.GetString("SkillText", "ColorAddon", "50,50,255", Buff, 16);
		m_SkillTextParam.uColorAddon = GetColor(Buff);
		Ini.GetString("SkillText", "ColorCutdown", "255,50,50", Buff, 16);
		m_SkillTextParam.uColorCutdown = GetColor(Buff);
	}
}

//更新升级点数
void KUiFightSkillSubPage::UpdateRemainPoint(int nPoint)
{
	m_nRemainSkillPoint = nPoint;
}

//更新升级点数
void KUiFightSkillSubPage::UpdateSkill(KUiSkillData* pSkill, int nIndex)
{
	_ASSERT(pSkill && nIndex >= 0 && nIndex < FIGHT_SKILL_COUNT_PER_PAGE);
	m_FightSkills[nIndex].HoldObject(pSkill->uGenre, pSkill->uId, pSkill->nLevel, pSkill->nData);
	if(pSkill->nData > pSkill->nLevel)
	{
		m_uColors[nIndex] = m_SkillTextParam.uColorAddon;
	}
	else if(pSkill->nData < pSkill->nLevel)
	{
		m_uColors[nIndex] = m_SkillTextParam.uColorCutdown;
	}
	else
	{
		m_uColors[nIndex] = m_SkillTextParam.uColorNormal;
	}
}

//更新数据
void KUiFightSkillSubPage::UpdateData(KUiSkillData* pSkills)
{
	_ASSERT(pSkills);
	for (int i = 0; i < FIGHT_SKILL_COUNT_PER_PAGE; i++)
	{
		m_FightSkills[i].HoldObject(pSkills[i].uGenre, pSkills[i].uId, pSkills[i].nLevel, pSkills[i].nData);
		if(pSkills[i].nData > pSkills[i].nLevel)
		{
			m_uColors[i] = m_SkillTextParam.uColorAddon;
		}
		else if(pSkills[i].nData < pSkills[i].nLevel)
		{
			m_uColors[i] = m_SkillTextParam.uColorCutdown;
		}
		else
		{
			m_uColors[i] = m_SkillTextParam.uColorNormal;
		}
	}
}

//窗口函数
int	KUiFightSkillSubPage::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_LEFT_CLICK_ITEM && uParam && m_nRemainSkillPoint)
	{
		KUiDraggedObject* pObj = (KUiDraggedObject*)uParam;
		if (pObj->uGenre != CGOG_NOTHING)
		{
			m_nRemainSkillPoint --;	// 使用技能点数
			g_pCoreShell->OperationRequest(GOI_TONE_UP_SKILL, CGOG_SKILL_FIGHT, pObj->uId);
		}
		return 0;
	}

	return KWndPage::WndProc(uMsg, uParam, nParam);
}

//绘制窗口
void KUiFightSkillSubPage::PaintWindow()
{
	KWndPage::PaintWindow();
	for (int i = 0; i < FIGHT_SKILL_COUNT_PER_PAGE; i++)
	{
		KUiDraggedObject	Obj;
		m_FightSkills[i].GetObject(Obj);
		if (Obj.uGenre != CGOG_NOTHING)
		{
			int nLeft, nTop, nWidth, nHeight;
			m_FightSkills[i].GetAbsolutePos(&nLeft, &nTop);
			m_FightSkills[i].GetSize(&nWidth, &nHeight);
			g_pCoreShell->DrawGameObj(CGOG_SKILL_LIVE, Obj.uId, nLeft, nTop, nWidth, nHeight, Obj.DataW);
			if (Obj.DataW)
			{
				char	szLevel[8];
				int		nLen;
				itoa(Obj.DataH, szLevel, 10);
				nLen = strlen(szLevel);
				nLeft += m_SkillTextParam.Offset.cx;
				nTop += m_SkillTextParam.Offset.cy;
				nLeft += (nWidth - nLen * m_SkillTextParam.nFont / 2) / 2;
				g_pRepresentShell->OutputText(m_SkillTextParam.nFont, szLevel, nLen, nLeft, nTop,
					m_uColors[i], 0);
			}
		}
	}
}

//初始化
void KUiFightSkillSub::Initialize()
{
	for (int i = 0; i < FIGHT_SKILL_SUB_PAGE_COUNT; i++)
	{
		m_SubPages[i].Initialize();
		AddPage(&m_SubPages[i], &m_SubPageBtn[i]);
	}
}

//载入界面方案
void KUiFightSkillSub::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\"SCHEME_INI_FIGHT, pScheme);
	if (Ini.Load(Buff))
	{
		KWndPageSet::Init(&Ini, "Main");
		m_oFixPos.x = 0;
		m_oFixPos.y = 0;
		SetPosition(0, 0);
		for (int i = 0; i < FIGHT_SKILL_SUB_PAGE_COUNT; i++)
		{
			m_SubPages[i].LoadScheme(pScheme);
			sprintf(Buff, "SubPageBtn_%d", i);
			m_SubPageBtn[i].Init(&Ini, Buff);
		}
		Show();
	}
}

//更新升级点数
void KUiFightSkillSub::UpdateRemainPoint(int nPoint)
{
	for (int i = 0; i < FIGHT_SKILL_SUB_PAGE_COUNT; i++)
		m_SubPages[i].UpdateRemainPoint(nPoint);
}

//更新技能
void KUiFightSkillSub::UpdateSkill(KUiSkillData* pSkill, int nIndex)
{
	_ASSERT(pSkill);
	int nPage = nIndex / FIGHT_SKILL_COUNT_PER_PAGE;
	nIndex = nIndex % FIGHT_SKILL_COUNT_PER_PAGE;
	_ASSERT(nPage >= 0 && nPage < FIGHT_SKILL_SUB_PAGE_COUNT);
	m_SubPages[nPage].UpdateSkill(pSkill, nIndex);
}

//更新数据
void KUiFightSkillSub::UpdateData()
{
	KUiSkillData	Skills[FIGHT_SKILL_COUNT];
	g_pCoreShell->GetGameData(GDI_FIGHT_SKILLS, (unsigned int)Skills, 0);
	for (int i = 0; i < FIGHT_SKILL_SUB_PAGE_COUNT; i++)
		m_SubPages[i].UpdateData(&Skills[i * FIGHT_SKILL_COUNT_PER_PAGE]);
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiFightSkill::Initialize()
{
	m_InternalPad.Initialize();
	AddChild(&m_InternalPad);
	AddChild(&m_RemainSkillPoint);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiFightSkill::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\"SCHEME_INI_FIGHT, pScheme);
	if (Ini.Load(Buff))
	{
		KWndPage::Init(&Ini, "Main");
		m_InternalPad.LoadScheme(pScheme);
		m_RemainSkillPoint.Init(&Ini, "RemainPoint");
	}
}

//--------------------------------------------------------------------------
//	功能：更新数据
//--------------------------------------------------------------------------
void KUiFightSkill::UpdateData()
{
	int nRemainSkillPoint = g_pCoreShell->GetGameData(GDI_FIGHT_SKILL_POINT, 0, 0);
	m_InternalPad.UpdateRemainPoint(nRemainSkillPoint);
	m_RemainSkillPoint.SetIntText(nRemainSkillPoint);
	m_InternalPad.UpdateData();
}

//--------------------------------------------------------------------------
//	功能：更新技能
//--------------------------------------------------------------------------
void KUiFightSkill::UpdateSkill(KUiSkillData* pSkill, int nIndex)
{
	m_InternalPad.UpdateSkill(pSkill, nIndex);
}

//--------------------------------------------------------------------------
//	功能：更新升级点数
//--------------------------------------------------------------------------
void KUiFightSkill::UpdateRemainPoint(int nPoint)
{
	m_RemainSkillPoint.SetIntText(nPoint);
	m_InternalPad.UpdateRemainPoint(nPoint);

}

KUiLiveSkill::KUiLiveSkill()
{
	m_nRemainSkillPoint = 0;
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiLiveSkill::Initialize()
{
	AddChild(&m_RemainSkillPoint);

	for (int i = 0; i < LIVE_SKILL_COUNT; i++)
	{
		m_LiveSkill[i].Celar();
		AddChild(&m_LiveSkill[i]);
		m_LiveSkill[i].SetContainerId((int)UOC_SKILL_LIST);
	}
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiLiveSkill::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_LIVE);
	if (Ini.Load(Buff))
	{
		KWndImage::Init(&Ini, "Main");
		m_RemainSkillPoint.	Init(&Ini, "RemainPoint");

		for (int i = 0; i < LIVE_SKILL_COUNT; i++)
		{
			sprintf(Buff, "Skill_%d", i);
			m_LiveSkill[i].Init(&Ini, Buff);
			m_LiveSkill[i].EnablePickPut(false);
		}

		Ini.GetInteger("SkillText", "Font", 12, &m_SkillTextParam.nFont);
		Ini.GetInteger2("SkillText", "Offset",
			(int*)&m_SkillTextParam.Offset.cx, (int*)&m_SkillTextParam.Offset.cy);
		Ini.GetString("SkillText", "Color", "", Buff, 16);
		m_SkillTextParam.uColorNormal = GetColor(Buff);
		Ini.GetString("SkillText", "ColorAddon", "50,50,255", Buff, 16);
		m_SkillTextParam.uColorAddon = GetColor(Buff);
		Ini.GetString("SkillText", "ColorCutdown", "255,50,50", Buff, 16);
		m_SkillTextParam.uColorCutdown = GetColor(Buff);
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiLiveSkill::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_LEFT_CLICK_ITEM && uParam && m_nRemainSkillPoint)
	{
		KUiDraggedObject* pObj = (KUiDraggedObject*)uParam;
		if (pObj->uGenre != CGOG_NOTHING)
		{
			m_nRemainSkillPoint--;	// 使用技能点数
			g_pCoreShell->OperationRequest(GOI_TONE_UP_SKILL, CGOG_SKILL_LIVE, pObj->uId);
		}
		return 0;
	}
	return KWndPage::WndProc(uMsg, uParam, nParam);
}

//--------------------------------------------------------------------------
//	功能：绘制窗口
//--------------------------------------------------------------------------
void KUiLiveSkill::PaintWindow()
{
	KWndPage::PaintWindow();
	for (int i = 0; i < LIVE_SKILL_COUNT; i++)
	{
		KUiDraggedObject	Obj;
		m_LiveSkill[i].GetObject(Obj);
		if (Obj.uGenre != CGOG_NOTHING)
		{
			int nLeft, nTop, nWidth, nHeight;
			m_LiveSkill[i].GetAbsolutePos(&nLeft, &nTop);
			m_LiveSkill[i].GetSize(&nWidth, &nHeight);
			g_pCoreShell->DrawGameObj(CGOG_SKILL_LIVE, Obj.uId, nLeft, nTop, nWidth, nHeight, Obj.DataW);
			if (Obj.DataW)
			{
				char	szLevel[8];
				int		nLen;
				itoa(Obj.DataW, szLevel, 10);
				nLen = strlen(szLevel);
				nLeft += m_SkillTextParam.Offset.cx;
				nTop += m_SkillTextParam.Offset.cy;
				nLeft += (nWidth - nLen * m_SkillTextParam.nFont / 2) / 2;
				g_pRepresentShell->OutputText(m_SkillTextParam.nFont, szLevel, nLen, nLeft, nTop,
					m_uColors[i], 0);
			}
		}
	}
}

//--------------------------------------------------------------------------
//	功能：更新技能公共数据
//--------------------------------------------------------------------------
void KUiLiveSkill::UpdateBaseData()
{
	//从游戏世界获取生活技能数据
	KUiPlayerLiveSkillBase	Info;
	g_pCoreShell->GetGameData(GDI_LIVE_SKILL_BASE, (unsigned int)&Info, 0);
	m_RemainSkillPoint.SetIntText(m_nRemainSkillPoint = Info.nRemainPoint);
}

//--------------------------------------------------------------------------
//	功能：更新技能
//--------------------------------------------------------------------------
void KUiLiveSkill::UpdateSkill(KUiSkillData* pSkill, int nIndex)
{
	if (pSkill && nIndex >= 0 && nIndex < LIVE_SKILL_COUNT)
	{
		m_LiveSkill[nIndex].HoldObject(pSkill->uGenre, pSkill->uId, pSkill->nLevel, pSkill->nData);
		if(pSkill->nData > pSkill->nLevel)
		{
			m_uColors[nIndex] = m_SkillTextParam.uColorAddon;
		}
		else if(pSkill->nData < pSkill->nLevel)
		{
			m_uColors[nIndex] = m_SkillTextParam.uColorCutdown;
		}
		else
		{
			m_uColors[nIndex] = m_SkillTextParam.uColorNormal;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：更新数据
//--------------------------------------------------------------------------
void KUiLiveSkill::UpdateData()
{
	UpdateBaseData();
	KUiSkillData	Skills[LIVE_SKILL_COUNT];
	g_pCoreShell->GetGameData(GDI_LIVE_SKILLS, (unsigned int)Skills, 0);
	for (int i = 0; i < LIVE_SKILL_COUNT; i++)
	{
		m_LiveSkill[i].HoldObject(Skills[i].uGenre, Skills[i].uId, Skills[i].nLevel, Skills[i].nData);
		if(Skills[i].nData > Skills[i].nLevel)
		{
			m_uColors[i] = m_SkillTextParam.uColorAddon;
		}
		else if(Skills[i].nData < Skills[i].nLevel)
		{
			m_uColors[i] = m_SkillTextParam.uColorCutdown;
		}
		else
		{
			m_uColors[i] = m_SkillTextParam.uColorNormal;
		}
	}
}

KUiSkills* KUiSkills::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiSkills* KUiSkills::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiSkills;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->m_FightSkillPad.UpdateData();
		m_pSelf->m_LiveSkillPad.UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiSkills* KUiSkills::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiSkills::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		if (bDestroy == false)
			m_pSelf->Hide();
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiSkills::Initialize()
{
	m_FightSkillPad.Initialize();
	AddPage(&m_FightSkillPad, &m_FightSkillPadBtn);
	m_LiveSkillPad.Initialize();
	AddPage(&m_LiveSkillPad, &m_LiveSkillPadBtn);

	AddChild(&m_Close);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiSkills::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_SHEET);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_FightSkillPadBtn.Init(&Ini, "FightBtn");
		m_pSelf->m_LiveSkillPadBtn .Init(&Ini, "LiveBtn");
		m_pSelf->m_Close           .Init(&Ini, "CloseBtn");

		m_pSelf->m_LiveSkillPad.LoadScheme(pScheme);
		m_pSelf->m_FightSkillPad.LoadScheme(pScheme);
	}
}

//--------------------------------------------------------------------------
//	功能：更新技能
//--------------------------------------------------------------------------
void KUiSkills::UpdateSkill(KUiSkillData* pSkill, int nIndex)
{
	if (pSkill)
	{
		if (m_pSelf)
		{
			if (pSkill->uGenre == CGOG_SKILL_LIVE)
				m_pSelf->m_LiveSkillPad.UpdateSkill(pSkill, nIndex);
			else if (pSkill->uGenre == CGOG_SKILL_FIGHT)
				m_pSelf->m_FightSkillPad.UpdateSkill(pSkill, nIndex);
		}
		if (g_pCoreShell)
		{
			KUiPlayerAttribute	Info;
			memset(&Info, 0, sizeof(KUiPlayerAttribute));
			g_pCoreShell->GetGameData(GDI_PLAYER_RT_ATTRIBUTE, (unsigned int)&Info, 0);
			if (Info.nLevel <= SET_NEW_SKILL_TO_IMMED_SKILL_LEVEL_RANGE)
			{
				g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL, (unsigned int)pSkill, 1);
				KSystemMessage	Msg;
				Msg.byConfirmType = SMCT_NONE;
				Msg.byParamSize = 0;
				Msg.byPriority = 0;
				Msg.eType = SMT_NORMAL;
				Msg.uReservedForUi = 0;

				KIniFile*	pIni = g_UiBase.GetCommConfigFile();
				if (pIni)
				{
					if (pIni->GetString("InfoString", AUTO_SET_IMMED_SKILL_MSG_ID,
						"", Msg.szMessage, sizeof(Msg.szMessage)))
					{
						KUiSysMsgCentre::AMessageArrival(&Msg, NULL);
					}
					g_UiBase.CloseCommConfigFile();
				}
			}
		}
	}
}

//--------------------------------------------------------------------------
//	功能：更新战斗技能升级点数
//--------------------------------------------------------------------------
void KUiSkills::UpdateFightRemainPoint(int nPoint)
{
	m_FightSkillPad.UpdateRemainPoint(nPoint);
}

//--------------------------------------------------------------------------
//	功能：更新生活技能公共数据
//--------------------------------------------------------------------------
void KUiSkills::UpdateLiveBaseData()
{
	m_LiveSkillPad.UpdateBaseData();
}

//窗口函数
int KUiSkills::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg == WND_N_BUTTON_CLICK && (KWndWindow*)uParam == (KWndWindow*)&m_Close)
	{
		Hide();	// 关闭装备框
		return 0;
	}
	return KWndPageSet::WndProc(uMsg, uParam, nParam);
}