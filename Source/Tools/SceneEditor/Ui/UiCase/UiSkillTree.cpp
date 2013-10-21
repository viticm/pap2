// -------------------------------------------------------------------------
//	文件名		：	UiSkillTree.cpp
//	创建者		：	吕桂华
//	创建时间	：	2002-9-16 9:47:24
//	功能描述	：
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "../Elem/MouseHover.h"
#include "uiskilltree.h"
#include "../UiBase.h"
#include "../ShortcutKey.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/CoreObjGenreDef.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;
extern iRepresentShell*	g_pRepresentShell;

#define SCHEME_INI						"技能选择树.ini"
#define SKILLTREE_SHORTCUT_SKILL_SAVE	"ShortSkill"
#define	IS_LEFT_SKILL					nLevel

KUiSkillTree*	KUiSkillTree::m_pSelf = NULL;
KUiSkillData	KUiSkillTree::ms_ShortcutSkills[SKILLTREE_SHORTCUT_SKILL_COUNT];

KUiSkillTree::KUiSkillTree()
{
	m_nNumSkills = 0;
	m_bLeft = true;
	m_nFont = 12;
}

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiSkillTree* KUiSkillTree::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiSkillTree* KUiSkillTree::OpenWindow(int bLeft)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiSkillTree;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		Wnd_SetCapture(m_pSelf);
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->m_bLeft = !!bLeft;
		m_pSelf->UpdateData();
		m_pSelf->Show();
		m_pSelf->BringToTop();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiSkillTree::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

//隐藏窗口
void KUiSkillTree::Hide()
{
	Wnd_ReleaseCapture();
	KWndWindow::Hide();
}

void KUiSkillTree::LoadConfig(KIniFile* pIni)
{
	memset(&ms_ShortcutSkills, 0, sizeof(ms_ShortcutSkills));
	if (pIni == NULL)
		return;

	int		i, j;
	bool	bEncounter = false;
	char szShortcutName[32];
	for (i = 0; i < SKILLTREE_SHORTCUT_SKILL_COUNT; i++)
	{
		sprintf(szShortcutName, "ShortcutSkill_%d", i);
		pIni->GetStruct(SKILLTREE_SHORTCUT_SKILL_SAVE, szShortcutName,
			&ms_ShortcutSkills[i], sizeof(KUiSkillData));

		if (ms_ShortcutSkills[i].uGenre != CGOG_NOTHING)
		{
			ms_ShortcutSkills[i].IS_LEFT_SKILL = !!ms_ShortcutSkills[i].IS_LEFT_SKILL;
			for (j = 0; j < i; j++)
			{
				if (ms_ShortcutSkills[i].uId == ms_ShortcutSkills[j].uId &&
					ms_ShortcutSkills[i].uGenre == ms_ShortcutSkills[j].uGenre &&
					ms_ShortcutSkills[i].IS_LEFT_SKILL == ms_ShortcutSkills[j].IS_LEFT_SKILL)
				{
					ms_ShortcutSkills[i].uGenre = CGOG_NOTHING;
					ms_ShortcutSkills[i].uId = 0;
				}
			}
			bEncounter = true;
		}
	}

	bool bLeft = false;
	do
	{
		bLeft = !bLeft;

		KUiSkillData	Skills[SKILLTREE_MAX_SKILL_COUNT];
		int nNum = g_pCoreShell->GetGameData(
			bLeft ? GDI_LEFT_ENABLE_SKILLS : GDI_RIGHT_ENABLE_SKILLS, (unsigned int)&Skills, 0);

		if (bEncounter)
		{
			for (i = 0; i < SKILLTREE_SHORTCUT_SKILL_COUNT; i++)
			{
				if (ms_ShortcutSkills[i].uGenre == CGOG_NOTHING ||
					(!bLeft) != (!ms_ShortcutSkills[i].IS_LEFT_SKILL))
					continue;
				for (j = 0; j < nNum; j++)
				{
					if (ms_ShortcutSkills[i].uId == Skills[j].uId &&
						ms_ShortcutSkills[i].uGenre == Skills[j].uGenre)
					{
						break;
					}
				}
				if (j >= nNum)
					ms_ShortcutSkills[i].uGenre = CGOG_NOTHING;
			}
		}

		KUiGameObject	Skill;
		memset(&Skill, 0, sizeof(Skill));
		pIni->GetStruct("Player", bLeft ? "LeftSkill" : "RightSkill",
			&Skill, sizeof(KUiGameObject));
		if (Skill.uGenre != CGOG_NOTHING && g_pCoreShell)
		{
			for (i = 0; i < nNum; i++)
			{
				if (Skills[i].uId == Skill.uId &&
					Skills[i].uGenre == Skill.uGenre)
				{
					g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL,
						(unsigned int)&Skill, bLeft ? 0 : 1);
					break;
				}
			}
		}
	}while(bLeft);
}

//保存自定义配置
void KUiSkillTree::SaveConfig(KIniFile* pIni)
{
	if (pIni == NULL)
		return;
	char szShortcutName[32];
	for (int i = 0; i < SKILLTREE_SHORTCUT_SKILL_COUNT; i++)
	{
		sprintf(szShortcutName, "ShortcutSkill_%d", i);
		if (ms_ShortcutSkills[i].uGenre != CGOG_NOTHING)
		{
			pIni->WriteStruct(SKILLTREE_SHORTCUT_SKILL_SAVE, szShortcutName,
				&ms_ShortcutSkills[i], sizeof(KUiSkillData));
		}
		else
		{
			pIni->EraseKey(SKILLTREE_SHORTCUT_SKILL_SAVE, szShortcutName);
		}
	}

	if(g_pCoreShell)
	{
		//记录玩家左右键技能
	    KUiPlayerImmedItemSkill Skill;
		g_pCoreShell->GetGameData(GDI_PLAYER_IMMED_ITEMSKILL, (unsigned int)&Skill, 0);

		if (Skill.IMmediaSkill[0].uGenre != CGOG_NOTHING)
			pIni->WriteStruct("Player", "LeftSkill", &Skill.IMmediaSkill[0], sizeof(KUiGameObject));
		else
			pIni->EraseKey("Player", "LeftSkill");
		if (Skill.IMmediaSkill[1].uGenre != CGOG_NOTHING)
			pIni->WriteStruct("Player", "RightSkill", &Skill.IMmediaSkill[1], sizeof(KUiGameObject));
		else
			pIni->EraseKey("Player", "RightSkill");
	}
}

// -------------------------------------------------------------------------
// 功能	: 初始化
// -------------------------------------------------------------------------
void KUiSkillTree::Initialize()
{
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this, WL_TOPMOST);
}

void KUiSkillTree::UpdateWndRect()
{
	int	nNumRow = 0, nNumColu = 0, nMaxColu = 0;
	int	nCurRowIndex = -1;
	for (int i = 0; i < m_nNumSkills; i++)
	{
		if (m_Skills[i].nLevel == nCurRowIndex)
			nNumColu ++;
		else
		{
			nNumRow++;
			if (nNumColu > nMaxColu)
				nMaxColu = nNumColu;
			nNumColu = 1;
			nCurRowIndex = m_Skills[i].nLevel;
		}
	}
	if (nNumColu > nMaxColu)
		nMaxColu = nNumColu;

	int nWidth = m_nWidthPerSkill * nMaxColu;
	int nHeight = m_nHeightPerSkill * nNumRow;

	int nLeft, nTop;
//	if (m_bLeft)
//	{
//		nLeft = m_nLeftLeft;
//		nTop = m_nLeftTop + m_nHeightPerSkill - nHeight;
//	}
//	else
	{
		nLeft = m_nRightLeft + m_nWidthPerSkill - nWidth;
		nTop = m_nRightTop + m_nHeightPerSkill - nHeight;
	}
	SetPosition(nLeft, nTop);
	SetSize(nWidth, nHeight);
}

// -------------------------------------------------------------------------
// 功能	: 更新数据
// -------------------------------------------------------------------------
void KUiSkillTree::UpdateData()
{
	m_nNumSkills = g_pCoreShell->GetGameData(
		m_bLeft ? GDI_LEFT_ENABLE_SKILLS : GDI_RIGHT_ENABLE_SKILLS, (unsigned int)&m_Skills, 0);

	for (int i = 0; i < SKILLTREE_SHORTCUT_SKILL_COUNT; i++)
	{
		if (ms_ShortcutSkills[i].uGenre == CGOG_NOTHING ||
			(!m_bLeft) != (!ms_ShortcutSkills[i].IS_LEFT_SKILL))
			continue;
		for (int j = 0; j < m_nNumSkills; j++)
		{
			if (ms_ShortcutSkills[i].uId == m_Skills[j].uId &&
				ms_ShortcutSkills[i].uGenre == m_Skills[j].uGenre)
			{
				break;
			}
		}
		if (j >= m_nNumSkills)
			ms_ShortcutSkills[i].uGenre = CGOG_NOTHING;
	}

	UpdateWndRect();
}

int	KUiSkillTree::PtInWindow(int x, int y)
{
	if (KWndWindow::PtInWindow(x, y))
		return (GetSkillAtPos(x, y) >= 0);
	return 0;
}

//得到指定位置的技能的索引，返回小于0的值表示指定的位置上没有技能
int	KUiSkillTree::GetSkillAtPos(int x, int y)
{
	int nMaxColu = m_Width / m_nWidthPerSkill;
	int nRow = (y - m_nAbsoluteTop) / m_nHeightPerSkill;
	int nColu, nIndex;

//	if (m_bLeft)
//		nColu = (x - m_nAbsoluteLeft) / m_nWidthPerSkill;
//	else
		nColu = nMaxColu - 1 - (x - m_nAbsoluteLeft) / m_nWidthPerSkill;

	nIndex = 0;
	while (nIndex < m_nNumSkills && m_Skills[nIndex].nLevel < nRow)
		nIndex ++;

	int	nNumColu = 0;
	while(nIndex < m_nNumSkills && m_Skills[nIndex].nLevel == nRow)
	{
		if (nNumColu == nColu)
			return nIndex;
		nIndex++;
		nNumColu ++;
	}

	return -1;
}

// -------------------------------------------------------------------------
// 功能		: 载入界面方案
// -------------------------------------------------------------------------
void KUiSkillTree::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;

	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (m_pSelf && Ini.Load(Buff))
	{
		Ini.GetInteger2("Main", "LeftBtnPos", &m_pSelf->m_nLeftLeft, &m_pSelf->m_nLeftTop);
		Ini.GetInteger2("Main", "RightBtnPos", &m_pSelf->m_nRightLeft, &m_pSelf->m_nRightTop);
		Ini.GetInteger2("Main", "BtnSize", &m_pSelf->m_nWidthPerSkill, &m_pSelf->m_nHeightPerSkill);
		if (m_pSelf->m_nWidthPerSkill < 0)
			m_pSelf->m_nWidthPerSkill = 1;
		if (m_pSelf->m_nHeightPerSkill <= 0)
			m_pSelf->m_nHeightPerSkill = 1;
		Ini.GetInteger("Main", "KeyFont", 12, &m_pSelf->m_nFont);
		Ini.GetString("Main", "KeyColor", "", Buff, sizeof(Buff));
		m_pSelf->m_uColor = GetColor(Buff);
		m_pSelf->UpdateWndRect();
	}
}

// -------------------------------------------------------------------------
// 函数		: KUiSkillTree::WndProc
// 功能		: 窗口函数
// -------------------------------------------------------------------------
int KUiSkillTree::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int x, y, nIndex;
	int	nRet = 0;
	switch(uMsg)
	{
	case WM_LBUTTONUP:
		x = GET_X_LPARAM(nParam);
		y = GET_Y_LPARAM(nParam);
		if ((nIndex = GetSkillAtPos(x, y)) >= 0)
		{
			KUiGameObject	Skill;
			Skill.uGenre = m_Skills[nIndex].uGenre;
			Skill.uId = m_Skills[nIndex].uId;
			g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL,
				(unsigned int)&Skill, m_bLeft ? 0 : 1);
		}
		Hide();
		break;
	case WM_KEYDOWN:
		if (uParam == VK_ESCAPE)
		{
			Hide();
			nRet = 1;
		}
	case WM_MOUSEHOVER:
		{
			int	x = GET_X_LPARAM(nParam);
			int y = GET_Y_LPARAM(nParam);
			int nSkill = GetSkillAtPos(x, y);
			if (nSkill >= 0)
			{
				if (g_MouseOver.IsMoseHoverWndObj(this, nSkill) == 0)
				{
					SetMouseHoverObjectDesc(this, nSkill, m_Skills[nSkill].uGenre,
						m_Skills[nSkill].uId, UOC_SKILL_TREE, x, y);
				}
			}
			else
				g_MouseOver.CancelMouseHoverInfo();
		}
		break;
	case WM_RBUTTONUP:
		Hide();
		break;
	default:
		nRet = KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiSkillTree::HandleShortcutKey(int nIndex)
{
	if (nIndex < 0 || nIndex >= SKILLTREE_SHORTCUT_SKILL_COUNT)
		return;
	if (GetIfVisible())
	{
		int	x, y;
		Wnd_GetCursorPos(&x, &y);
		int nSkill = m_pSelf->GetSkillAtPos(x, y);
		if (nSkill < 0)
			return;

		ms_ShortcutSkills[nIndex].uGenre = m_pSelf->m_Skills[nSkill].uGenre;
		ms_ShortcutSkills[nIndex].uId = m_pSelf->m_Skills[nSkill].uId;
		ms_ShortcutSkills[nIndex].IS_LEFT_SKILL = m_pSelf->m_bLeft;
		for (int i = 0; i < SKILLTREE_SHORTCUT_SKILL_COUNT; i++)
		{
			if (i != nIndex)
			{
				if (ms_ShortcutSkills[nIndex].uId == ms_ShortcutSkills[i].uId &&
					ms_ShortcutSkills[nIndex].IS_LEFT_SKILL == ms_ShortcutSkills[i].IS_LEFT_SKILL &&
					ms_ShortcutSkills[nIndex].uGenre == ms_ShortcutSkills[i].uGenre)
				{
					ms_ShortcutSkills[i].uGenre = CGOG_NOTHING;
					ms_ShortcutSkills[i].uId = 0;
				}
			}
		}
	}
	else if (ms_ShortcutSkills[nIndex].uGenre != CGOG_NOTHING)
	{
		g_pCoreShell->OperationRequest(GOI_SET_IMMDIA_SKILL,
			(unsigned int)&ms_ShortcutSkills[nIndex],
			ms_ShortcutSkills[nIndex].IS_LEFT_SKILL ? 0 : 1);
	}
}

void KUiSkillTree::DirectHandleShortcutKey(int nIndex)
{
	if (nIndex < 0 || nIndex >= SKILLTREE_SHORTCUT_SKILL_COUNT)
		return;

	int	x, y;
	Wnd_GetCursorPos(&x, &y);
	if (GetIfVisible())
	{
		int nSkill = m_pSelf->GetSkillAtPos(x, y);
		if (nSkill < 0)
			return;

		ms_ShortcutSkills[nIndex].uGenre = m_pSelf->m_Skills[nSkill].uGenre;
		ms_ShortcutSkills[nIndex].uId = m_pSelf->m_Skills[nSkill].uId;
		ms_ShortcutSkills[nIndex].IS_LEFT_SKILL = m_pSelf->m_bLeft;
		for (int i = 0; i < SKILLTREE_SHORTCUT_SKILL_COUNT; i++)
		{
			if (i != nIndex)
			{
				if (ms_ShortcutSkills[nIndex].uId == ms_ShortcutSkills[i].uId &&
					ms_ShortcutSkills[nIndex].IS_LEFT_SKILL == ms_ShortcutSkills[i].IS_LEFT_SKILL &&
					ms_ShortcutSkills[nIndex].uGenre == ms_ShortcutSkills[i].uGenre)
				{
					ms_ShortcutSkills[i].uGenre = CGOG_NOTHING;
					ms_ShortcutSkills[i].uId = 0;
				}
			}
		}
	}
	else if (ms_ShortcutSkills[nIndex].uGenre != CGOG_NOTHING)
	{
		g_pCoreShell->UseSkill(x, y, ms_ShortcutSkills[nIndex].uId);
	}
}

// -------------------------------------------------------------------------
// 功能	: 窗体绘制
// -------------------------------------------------------------------------
void KUiSkillTree::PaintWindow()
{
	if (g_pRepresentShell == NULL)
		return;
	KWndWindow::PaintWindow();
	int	nCurRowIndex = -1;
	int	nLeft, nTop = m_nAbsoluteTop - m_nHeightPerSkill;
	int nDX = -m_nWidthPerSkill; //m_bLeft ? m_nWidthPerSkill : (-m_nWidthPerSkill);

	for (int i = 0; i < m_nNumSkills; i++)
	{
		if (m_Skills[i].nLevel == nCurRowIndex)
		{
			nLeft += nDX;
		}
		else
		{
			nTop += m_nHeightPerSkill;
//			if (m_bLeft)
//				nLeft = m_nAbsoluteLeft;
//			else
				nLeft = m_nAbsoluteLeft + m_Width - m_nWidthPerSkill;
			nCurRowIndex = m_Skills[i].nLevel;
		}
		g_pCoreShell->DrawGameObj(m_Skills[i].uGenre, m_Skills[i].uId,
			nLeft, nTop, m_nWidthPerSkill, m_nHeightPerSkill, 0);


		for (int j = 0; j < SKILLTREE_SHORTCUT_SKILL_COUNT; j++)
		{
			if (m_bLeft == (unsigned)ms_ShortcutSkills[j].IS_LEFT_SKILL &&
				m_Skills[i].uGenre == ms_ShortcutSkills[j].uGenre &&
				m_Skills[i].uId == ms_ShortcutSkills[j].uId)
			{
				char szScript[64];
				sprintf(szScript, SCK_SHORTCUTSKILL_FORMAT, j);
				int nIndexC = KShortcutKeyCentre::FindCommandByScript(szScript);
				char* pszKey = NULL;
				if (nIndexC >= 0)
				{
					pszKey = (char*)KShortcutKeyCentre::GetKeyName(KShortcutKeyCentre::GetCommandKey(nIndexC));
				}
				else
				{
					sprintf(szScript, SCK_DIRECTSHORTCUTSKILL_FORMAT, j);
					nIndexC = KShortcutKeyCentre::FindCommandByScript(szScript);
					if (nIndexC >= 0)
					{
						pszKey = (char*)KShortcutKeyCentre::GetKeyName(KShortcutKeyCentre::GetCommandKey(nIndexC));
					}
				}

				if (pszKey)
				{
					g_pRepresentShell->OutputText(m_nFont, pszKey, KRF_ZERO_END,
						nLeft + m_nWidthPerSkill - m_nFont, nTop + m_nHeightPerSkill - m_nFont - 1, m_uColor);
					break;
				}
			}
		}
	}
}
