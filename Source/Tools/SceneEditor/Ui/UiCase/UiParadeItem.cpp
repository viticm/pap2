/*****************************************************************************************
//	界面--状态界面
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-2
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiParadeItem.h"
#include "../ShortcutKey.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "../UiBase.h"
#include "UiPortrait.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI		"看对方玩家装备.ini"
#define WHICH_NUMBER_WHEN_CAN_SEE_OTHERS_PK_VALUE 5

KUiParadeItem* KUiParadeItem::m_pSelf = NULL;

// -------------------------------------------------------------------------
// ---> 建立控件与UIEP_*以及可接纳物品的类型的对应关系
static struct UE_CTRL_MAP
{
	int				nPosition;
	const char*		pIniSection;
}CtrlItemMap[_ITEM_COUNT] =
{
	{ UIEP_HEAD,		"Cap"		},	//装备-帽子
	{ UIEP_HAND,		"Weapon"	},	//装备-武器
	{ UIEP_NECK,		"Necklace"	},	//装备-项链
	{ UIEP_FINESSE,		"Bangle"	},	//装备-手镯
	{ UIEP_BODY,		"Cloth"		},	//装备-衣服
	{ UIEP_WAIST,		"Sash"		},	//装备-腰带
	{ UIEP_FINGER1,		"Ring1"		},	//装备-戒指
	{ UIEP_FINGER2,		"Ring2"		},	//装备-戒指
	{ UIEP_WAIST_DECOR,	"Pendant"	},	//装备-腰缀
	{ UIEP_FOOT,		"Shoes"		},	//装备-鞋子
	{ UIEP_HORSE,		"Horse"		}	//装备-马
};


//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiParadeItem* KUiParadeItem::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiParadeItem* KUiParadeItem::OpenWindow(KUiPlayerItem* pDest)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiParadeItem;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->m_Dest = *pDest;

		char Scheme[256];
		g_UiBase.GetCurSchemePath(Scheme, 256);
		LoadScheme(Scheme);

		m_pSelf->BringToTop();
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData(pDest);

		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiParadeItem::CloseWindow(bool bDestroy)
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

		if (g_pCoreShell)
			g_pCoreShell->OperationRequest(GOI_VIEW_PLAYERITEM_END, 0, 0);
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
void KUiParadeItem::Initialize()
{
	AddChild(&m_Name);
	AddChild(&m_Title);
	AddChild(&m_WorldRank);
	AddChild(&m_MateName);
	AddChild(&m_PKValue);
	AddChild(&m_Face);

	for (int i = 0; i < _ITEM_COUNT; i ++)
	{
		m_EquipBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_EquipBox[i]);
		m_EquipBox[i].SetContainerId((int)UOC_OTHER_EQUIPTMENT);
		m_EquipBox[i].SetWindowLong(KWndWindow::enumWLI_ID, i);
		m_EquipBox[i].SetWindowLong(KWndWindow::enumWLI_USERDATA, i);
	}

	AddChild(&m_Close);

	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiParadeItem::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
		if (Ini.Load(Buff))
			m_pSelf->LoadScheme(&Ini);
	}
}

//载入界面方案
void KUiParadeItem::LoadScheme(class KIniFile* pIni)
{
	if (g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, m_Dest.uId))
		Init(pIni, "Male");
	else
		Init(pIni, "Female");

	m_Face    .Init(pIni, "Face");
	m_Name    .Init(pIni, "Name");
	m_Title   .Init(pIni, "Title");
	m_PKValue .Init(pIni, "PKValue");
	m_MateName.Init(pIni, "MateName");
	m_WorldRank.Init(pIni, "WorldRank");

	m_Close	 .Init(pIni, "Close");

	for (int i = 0; i < _ITEM_COUNT; i ++)
	{
		m_EquipBox[i].Init(pIni, CtrlItemMap[i].pIniSection);
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiParadeItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Close)
			Hide();
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：更新基本数据（人名等不易变数据）
//--------------------------------------------------------------------------
void KUiParadeItem::UpdateBaseData(KUiPlayerItem* pDest)
{
	if (!g_pCoreShell)
		return;

	KUiPlayerBaseInfo	Info;
	int nPK, nPortrait;
	char szBuf[128];

	memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
	g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info, pDest->uId);
	m_Name  .SetText(Info.Name);
	m_Title .SetText(Info.Title);

	nPK = HIWORD(pDest->nData);
	nPortrait = LOWORD(pDest->nData);

	if(nPK >= WHICH_NUMBER_WHEN_CAN_SEE_OTHERS_PK_VALUE)
		m_PKValue.SetIntText(nPK);
	else
		m_PKValue.SetText("?");

	m_Face.SetImage(ISI_T_SPR, KUiPortrait::GetPortraitImagePath(nPortrait, szBuf));
	if(pDest->nParam > 0)
	{
		m_WorldRank.SetIntText(pDest->nParam);
	}
	else
	{
		m_WorldRank.SetText("?");
	}

	if(pDest->szMateName[0])
	{
		m_MateName.SetText(pDest->szMateName);
	}
	else
	{
		m_MateName.SetText("无");
	}
}

//--------------------------------------------------------------------------
//	功能：更新数据
//--------------------------------------------------------------------------
void KUiParadeItem::UpdateData(KUiPlayerItem* pDest)
{
	if (pDest == NULL)
		return;

	UpdateBaseData(pDest);

	UpdateAllEquips(pDest);
}

void KUiParadeItem::UpdateAllEquips(KUiPlayerItem* pDest)
{
	if (!g_pCoreShell)
		return;

	KUiObjAtRegion	Equips[_ITEM_COUNT];
	int nCount = g_pCoreShell->GetGameData(GDI_PARADE_EQUIPMENT, (unsigned int)&Equips, 0);
	int	i;
	for (i = 0; i < _ITEM_COUNT; i++)
		m_EquipBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Equips[i].Obj.uGenre != CGOG_NOTHING)
			UpdateEquip(&Equips[i], true);
	}
}

//--------------------------------------------------------------------------
//	功能：装备变化更新
//--------------------------------------------------------------------------
void KUiParadeItem::UpdateEquip(KUiObjAtRegion* pEquip, int bAdd)
{
	if (pEquip)
	{
		for (int i = 0; i < _ITEM_COUNT; i++)
		{
			if (CtrlItemMap[i].nPosition == pEquip->Region.v)
			{
				if (bAdd)
					m_EquipBox[i].HoldObject(pEquip->Obj.uGenre, pEquip->Obj.uId,
						pEquip->Region.Width, pEquip->Region.Height);
				else
					m_EquipBox[i].HoldObject(CGOG_NOTHING, 0, 0, 0);
				break;
			}
		}
	}
}
