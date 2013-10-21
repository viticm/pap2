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
#include "UiStatus.h"
#include "UiPlayerBar.h"
#include "../ShortcutKey.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/gamedatadef.h"
#include "../UiBase.h"
#include "UiTradeConfirmWnd.h"
#include "UiPortrait.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI		"玩家装备与人物状态.ini"

KUiStatus* KUiStatus::m_pSelf = NULL;

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
KUiStatus* KUiStatus::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiStatus* KUiStatus::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiStatus;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiStatus::CloseWindow(bool bDestroy)
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
void KUiStatus::Initialize()
{
	AddChild(&m_Agname);
	AddChild(&m_Name);
	AddChild(&m_Title);

	AddChild(&m_Money);

	AddChild(&m_Life);
	AddChild(&m_Mana);
	AddChild(&m_Stamina);
	AddChild(&m_Angry);
	AddChild(&m_Experience);

	AddChild(&m_RemainPoint);
	AddChild(&m_Strength);
	AddChild(&m_Dexterity);
	AddChild(&m_Vitality);
	AddChild(&m_Energy);

	AddChild(&m_AddStrength);
	AddChild(&m_AddDexterity);
	AddChild(&m_AddVitality);
	AddChild(&m_AddEnergy);

	AddChild(&m_LeftDamage);
	AddChild(&m_RightDamage);
	AddChild(&m_Attack);
	AddChild(&m_Defence);
	AddChild(&m_MoveSpeed);
	AddChild(&m_AttackSpeed);

	AddChild(&m_PhyDef);
	AddChild(&m_CoolDef);
	AddChild(&m_LightDef);
	AddChild(&m_FireDef);
	AddChild(&m_PoisonDef);
	AddChild(&m_Level);
	AddChild(&m_StatusDesc);

	AddChild(&m_ClickHereSelectPortrait);
	AddChild(&m_PKValue);
	AddChild(&m_Face);

	for (int i = 0; i < _ITEM_COUNT; i ++)
	{
		m_EquipBox[i].SetObjectGenre(CGOG_ITEM);
		AddChild(&m_EquipBox[i]);
		m_EquipBox[i].SetContainerId((int)UOC_EQUIPTMENT);
		m_EquipBox[i].SetWindowLong(enumWLI_ID, i);
		m_EquipBox[i].SetWindowLong(enumWLI_USERDATA, i);
	}

	AddChild(&m_OpenItemPad);
	AddChild(&m_Close);

	Wnd_AddWindow(this);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiStatus::LoadScheme(const char* pScheme)
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
void KUiStatus::LoadScheme(class KIniFile* pIni)
{
	if (g_pCoreShell->GetGameData(GDI_PLAYER_IS_MALE, 0, 0))
		Init(pIni, "Male");
	else
		Init(pIni, "Female");

	m_Face    .Init(pIni, "Face");
	m_Agname  .Init(pIni, "Agname");
	m_Name    .Init(pIni, "Name");
	m_Title   .Init(pIni, "Title");

	m_Money  .Init(pIni, "Money");

	m_Life		.Init(pIni, "Life");
	m_Mana		.Init(pIni, "Mana");
	m_Stamina	.Init(pIni, "Stamina");
	m_Angry		.Init(pIni, "Angry");
	m_Experience.Init(pIni, "Exp");

	m_RemainPoint.Init(pIni, "RemainPoint");
	m_Strength   .Init(pIni, "Strength");
	m_Dexterity  .Init(pIni, "Dexterity");
	m_Vitality   .Init(pIni, "Vitality");
	m_Energy     .Init(pIni, "Energy");

	m_AddStrength .Init(pIni, "AddStrength");
	m_AddDexterity.Init(pIni, "AddDexterity");
	m_AddVitality .Init(pIni, "AddVitality");
	m_AddEnergy   .Init(pIni, "AddEnergy");

	m_LeftDamage .Init(pIni, "LeftDamage");
	m_RightDamage.Init(pIni, "RightDamage");
	m_Attack     .Init(pIni, "Attack");
	m_Defence    .Init(pIni, "Defense");
	m_MoveSpeed  .Init(pIni, "MoveSpeed");
	m_AttackSpeed.Init(pIni, "AttackSpeed");

	m_PhyDef	 .Init(pIni, "ResistPhy");
	m_CoolDef    .Init(pIni, "ResistCold");
	m_LightDef   .Init(pIni, "ResistLighting");
	m_FireDef    .Init(pIni, "ResistFire");
	m_PoisonDef  .Init(pIni, "ResistPoison");

	m_Level		 .Init(pIni, "Level");
	m_StatusDesc .Init(pIni, "Status");
	m_Close	 .Init(pIni, "Close");

	m_OpenItemPad.Init(pIni, "Item");

	m_PKValue.Init(pIni, "PKValue");
	m_ClickHereSelectPortrait.Init(pIni, "ClickHere");

	for (int i = 0; i < _ITEM_COUNT; i ++)
	{
		m_EquipBox[i].Init(pIni, CtrlItemMap[i].pIniSection);
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KUiStatus::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_Close)
			Hide();
		else if (uParam == (unsigned int)(KWndWindow*)&m_OpenItemPad)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_ITEMS);
		else if (uParam == (unsigned int)(KWndWindow*)&m_AddStrength)
		{
			if(m_nRemainPoint > 0)
				UseRemainPoint(UIPA_STRENGTH);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_AddDexterity)
		{
			if(m_nRemainPoint > 0)
				UseRemainPoint(UIPA_DEXTERITY);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_AddVitality)
		{
			if(m_nRemainPoint > 0)
				UseRemainPoint(UIPA_VITALITY);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_AddEnergy)
		{
			if(m_nRemainPoint > 0)
				UseRemainPoint(UIPA_ENERGY);
		}
		else if(uParam == (unsigned int)&m_Face)
		{
			if(KUiPortrait::GetIfVisible())
			{
				KUiPortrait::CloseWindow();
			}
			else
			{
				KUiPortrait::OpenWindow();
			}
		}
		break;
	case WND_N_ITEM_PICKDROP:
		if (g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM))
			OnEquiptChanged((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		else if(g_UiBase.IsOperationEnable(UIS_O_REPARE_ITEM))
			OnRepairItem((ITEM_PICKDROP_PLACE*)uParam);
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：升级某项属性
//--------------------------------------------------------------------------
void KUiStatus::UseRemainPoint(UI_PLAYER_ATTRIBUTE type)
{
	g_pCoreShell->OperationRequest(GOI_TONE_UP_ATTRIBUTE, type, 0);
	m_nRemainPoint --;
	m_RemainPoint.SetIntText(m_nRemainPoint);
	m_AddStrength.Enable(m_nRemainPoint);
	m_AddDexterity.Enable(m_nRemainPoint);
	m_AddVitality.Enable(m_nRemainPoint);
	m_AddEnergy.Enable(m_nRemainPoint);
}

//--------------------------------------------------------------------------
//	功能：更新基本数据（人名等不易变数据）
//--------------------------------------------------------------------------
void KUiStatus::UpdateBaseData()
{
	char szBuf[128];
	KUiPlayerBaseInfo	Info;

	memset(&Info, 0, sizeof(KUiPlayerBaseInfo));
	memset(szBuf, 0, sizeof(szBuf));
	g_pCoreShell->GetGameData(GDI_PLAYER_BASE_INFO, (int)&Info, 0);
	m_Agname.SetText(Info.Agname);
	m_Name  .SetText(Info.Name);
	m_Title .SetText(Info.Title);

	int nPortraitCount = KUiPortrait::GetPortraitCount();
	if(Info.nPortrait > 0 && Info.nPortrait <= nPortraitCount)
	{
		m_Face	.SetImage(ISI_T_SPR, KUiPortrait::GetPortraitImagePath(Info.nPortrait, szBuf));
		m_ClickHereSelectPortrait.Hide();
	}
	else
	{
		m_ClickHereSelectPortrait.Show();
	}
}

void KUiStatus::UpdateRuntimeInfo(KUiPlayerRuntimeInfo* pInfo)
{
	if (pInfo)
	{
		m_Life		.Set2IntText(pInfo->nLife, pInfo->nLifeFull, '/');
		m_Mana		.Set2IntText(pInfo->nMana, pInfo->nManaFull, '/');
		m_Stamina	.Set2IntText(pInfo->nStamina, pInfo->nStaminaFull, '/');
		m_Angry		.Set2IntText(pInfo->nAngry, pInfo->nAngryFull, '/');
		m_Experience.Set2IntText(pInfo->nExperience, pInfo->nExperienceFull, '/');
		m_PKValue   .SetIntText(pInfo->cPKValue);
		//Info.byAction & PA_RIDE
	}
}

//--------------------------------------------------------------------------
//	功能：更新数据
//--------------------------------------------------------------------------
void KUiStatus::UpdateData()
{
	UpdateAllEquips();
	UpdateBaseData();
}

void KUiStatus::UpdateAllEquips()
{
	KUiObjAtRegion	Equips[_ITEM_COUNT];
	int nCount = g_pCoreShell->GetGameData(GDI_EQUIPMENT, (unsigned int)&Equips, 0);
	int	i;
	for (i = 0; i < _ITEM_COUNT; i++)
		m_EquipBox[i].Celar();
	for (i = 0; i < nCount; i++)
	{
		if (Equips[i].Obj.uGenre != CGOG_NOTHING)
			UpdateEquip(&Equips[i], true);
	}
}

void KUiStatus::UpdateRuntimeAttribute(KUiPlayerAttribute* pInfo)
{
	if (pInfo)
	{
		m_Money.SetIntText(pInfo->nMoney);

		m_nRemainPoint = pInfo->nBARemainPoint;
		m_AddStrength.Enable(m_nRemainPoint);
		m_AddDexterity.Enable(m_nRemainPoint);
		m_AddVitality.Enable(m_nRemainPoint);
		m_AddEnergy.Enable(m_nRemainPoint);
		m_RemainPoint.SetIntText(pInfo->nBARemainPoint);
		m_Strength   .SetIntText(pInfo->nStrength);
		m_Dexterity  .SetIntText(pInfo->nDexterity);
		m_Vitality   .SetIntText(pInfo->nVitality);
		m_Energy     .SetIntText(pInfo->nEnergy);

		m_LeftDamage.Set2IntText(pInfo->nKillMIN,pInfo->nKillMAX,'/');
		m_RightDamage.Set2IntText(pInfo->nRightKillMin, pInfo->nRightKillMax, '/');
		char	szValue[32];
		if (pInfo->nAttack != 0x7fffffff)
		{
			if (pInfo->nRightAttack != 0x7fffffff)
				m_Attack.Set2IntText(pInfo->nAttack, pInfo->nRightAttack, '/');
			else
			{
				sprintf(szValue, "%d/-", pInfo->nAttack);
				m_Attack.SetText(szValue);
			}
		}
		else if (pInfo->nRightAttack != 0x7fffffff)
		{
			sprintf(szValue, "-/%d", pInfo->nRightAttack);
			m_Attack.SetText(szValue);
		}
		else
		{
			m_Attack.SetText("-/-");
		}
		m_Defence.SetIntText(pInfo->nDefence);
		m_MoveSpeed.SetIntText(pInfo->nMoveSpeed);

		m_AttackSpeed.Set2IntText(pInfo->nAttackSpeed, pInfo->nCastSpeed, '/');

		char	TextInfo[32];
		sprintf(TextInfo, "%d%%", pInfo->nPhyDef);
		m_PhyDef	.SetText(TextInfo);
		sprintf(TextInfo, "%d%%", pInfo->nCoolDef);
		m_CoolDef  .SetText(TextInfo);
		sprintf(TextInfo, "%d%%", pInfo->nLightDef);
		m_LightDef .SetText(TextInfo);
		sprintf(TextInfo, "%d%%", pInfo->nFireDef);
		m_FireDef  .SetText(TextInfo);
		sprintf(TextInfo, "%d%%", pInfo->nPoisonDef);
		m_PoisonDef.SetText(TextInfo);

		m_Level.SetIntText(pInfo->nLevel);			//等级
		m_StatusDesc.SetText(pInfo->StatusDesc);
	}
}

//--------------------------------------------------------------------------
//	功能：响应界面操作引起装备的改变
//--------------------------------------------------------------------------
void KUiStatus::OnEquiptChanged(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	KUiObjAtContRegion	Drop, Pick;
	KUiDraggedObject	Obj;
	KWndWindow*			pWnd = NULL;

	DWORD	dwStatus = g_UiBase.GetStatus();
	if (pPickPos)
	{
		//_ASSERT(pPickPos->pWnd);
		((KWndObjectBox*)(pPickPos->pWnd))->GetObject(Obj);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = 0;
		Pick.eContainer = UOC_EQUIPTMENT;
		pWnd = pPickPos->pWnd;

		if (dwStatus & UIS_S_TRADE_REPAIR)
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR,
				(unsigned int)(&Pick), 0);
			return;
		}
		else if (dwStatus & UIS_S_TRADE_SALE)
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL,
				(unsigned int)(&Pick), 0);
			return;
		}
		else if (dwStatus & UIS_S_TRADE_NPC)
		{

			return;
		}
		else if (dwStatus & UIS_S_TRADE_BUY)
			return;
	}
	else if (pDropPos)
	{
		pWnd = pDropPos->pWnd;
	}
	else
		return;

	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.h = 0;
		Drop.eContainer = UOC_EQUIPTMENT;
	}

	for (int i = 0; i < _ITEM_COUNT; i++)
	{
		if (pWnd == (KWndWindow*)&m_EquipBox[i])
		{
			Drop.Region.v = Pick.Region.v = CtrlItemMap[i].nPosition;
			break;
		}
	}
	//_ASSERT(i < _ITEM_COUNT);

	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}

//--------------------------------------------------------------------------
//	功能：装备变化更新
//--------------------------------------------------------------------------
void KUiStatus::UpdateEquip(KUiObjAtRegion* pEquip, int bAdd)
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


//--------------------------------------------------------------------------
//	功能：修理装备
//--------------------------------------------------------------------------
void KUiStatus::OnRepairItem(ITEM_PICKDROP_PLACE* pPickPos)
{
	KUiDraggedObject	ObjDragging;
	KUiObjAtContRegion	Obj;

	if (pPickPos && g_pCoreShell)
	{
	    ((KWndObjectBox*)(pPickPos->pWnd))->GetObject(ObjDragging);

		Obj.Obj.uGenre = ObjDragging.uGenre;
	    Obj.Obj.uId = ObjDragging.uId;
	    Obj.Region.h = ObjDragging.DataX;
	    Obj.Region.v = ObjDragging.DataY;
	    Obj.Region.Width  = ObjDragging.DataW;
	    Obj.Region.Height = ObjDragging.DataH;
	    Obj.eContainer = UOC_EQUIPTMENT;

		KUiItemBuySelInfo	Price = { 0 };
	    if (g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE,
		    (unsigned int)(&Obj), (int)(&Price)))
	    {
    		KUiTradeConfirm::OpenWindow(&Obj, &Price, TCA_REPAIR);
    	}
	}
}
