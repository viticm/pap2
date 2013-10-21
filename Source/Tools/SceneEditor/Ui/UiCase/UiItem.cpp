// -------------------------------------------------------------------------
//	文件名		：	UiItem.cpp
//	Author		：	吕桂华, Wooy(Wu yue)
//	创建时间	：	2002-9-16 11:26:43
//	功能描述	：
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "uitrade.h"
#include "uiitem.h"
#include "UiGetMoney.h"
#include "UiStoreBox.h"
#include "UiTradeConfirmWnd.h"
#include "UiShop.h"
#include "UiStall.h"
#include "UiStallMarkPrice.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiBase.h"
#include "../ShortcutKey.h"
#include "../UiSoundSetting.h"
#include "UiStallSetAdv.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI_ITEM	"随身物品.ini"

KUiItem* KUiItem::m_pSelf = NULL;

enum WAIT_OTHER_WND_OPER_PARAM
{
	UIITEM_WAIT_GETMONEY,
};

KUiItem::KUiItem()
{
	m_nStatusHandle = 0;
}

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiItem* KUiItem::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiItem* KUiItem::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiItem;
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
void KUiItem::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		KUiShop::CancelTrade();
		KUiStallMarkPrice::CloseWindow(true);
		g_UiBase.ReleaseStatus(m_pSelf->m_nStatusHandle);
		if (bDestroy == false)
			m_pSelf->Hide();
		else
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

// -------------------------------------------------------------------------
// 功能	: 初始化
// -------------------------------------------------------------------------
void KUiItem::Initialize()
{
	AddChild(&m_Money);
	AddChild(&m_GetMoneyBtn);
	AddChild(&m_CloseBtn);
	AddChild(&m_ItemBox);
	AddChild(&m_OpenStatusPadBtn);
	AddChild(&m_MarkPriceBtn);
	AddChild(&m_MakeAdvBtn);
	AddChild(&m_MakeStallBtn);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_ItemBox.SetContainerId((int)UOC_ITEM_TAKE_WITH);
	m_nMoney = 0;
	Wnd_AddWindow(this);
}

//活动函数
void KUiItem::Breathe()
{
	m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	//m_Money.SetIntText(m_nMoney);
	m_Money.SetFormatText(m_nMoney);
}

void KUiItem::OnNpcTradeMode(bool bTrue)
{
	if (m_pSelf)
		m_pSelf->m_ItemBox.EnablePickPut(!bTrue);
}

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
void KUiItem::UpdateData()
{
	m_ItemBox.Clear();

	m_nMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	//m_Money.SetIntText(m_nMoney);
	m_Money.SetFormatText(m_nMoney);

	KUiObjAtRegion* pObjs = NULL;
	int nCount = g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, 0, 0);
	if (nCount == 0)
		return;

	int nStallState = g_pCoreShell->GetGameData(GDI_IS_STALL, -1, -1);
	m_MakeStallBtn.CheckButton(nStallState == 1);
	m_ItemBox.EnablePickPut(!nStallState);
	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_TAKEN_WITH, (unsigned int)pObjs, nCount);//单线程执行，nCount值不变
		for (int i = 0; i < nCount; i++)
		{
			KUiDraggedObject no;
			no.uGenre = pObjs[i].Obj.uGenre;
			no.uId = pObjs[i].Obj.uId;
			no.DataX = pObjs[i].Region.h;
			no.DataY = pObjs[i].Region.v;
			no.DataW = pObjs[i].Region.Width;
			no.DataH = pObjs[i].Region.Height;
			m_ItemBox.AddObject(&no, 1);
		}
		free(pObjs);
		pObjs = NULL;
	}
}

// -------------------------------------------------------------------------
// 功能	: 物品变化更新
// -------------------------------------------------------------------------
void KUiItem::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		KUiDraggedObject Obj;
		Obj.uGenre = pItem->Obj.uGenre;
		Obj.uId = pItem->Obj.uId;
		Obj.DataX = pItem->Region.h;
		Obj.DataY = pItem->Region.v;
		Obj.DataW = pItem->Region.Width;
		Obj.DataH = pItem->Region.Height;
		if (bAdd)
		{
			m_ItemBox.AddObject(&Obj, 1);
			KUiShop *pShop = KUiShop::GetIfVisible();
			if(pShop)
			{
				pShop->ABuyFinish();
			}
		}
		else
			m_ItemBox.RemoveObject(&Obj);
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
	}
	else
		UpdateData();
}

// -------------------------------------------------------------------------
// 功能	: 载入界面方案
// -------------------------------------------------------------------------
void KUiItem::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_ITEM);
	if (m_pSelf && Ini.Load(Buff))
	{
		m_pSelf->Init(&Ini, "Main");
		m_pSelf->m_Money.Init(&Ini, "Money");
		m_pSelf->m_GetMoneyBtn.Init(&Ini, "GetMoneyBtn");
		m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");
		m_pSelf->m_MarkPriceBtn.Init(&Ini, "MarkPriceBtn");
		m_pSelf->m_ItemBox.Init(&Ini, "ItemBox");
		m_pSelf->m_OpenStatusPadBtn.Init(&Ini, "OpenStatus");
		m_pSelf->m_MakeAdvBtn.Init(&Ini, "MakeAdvBtn");
		m_pSelf->m_MakeStallBtn.Init(&Ini, "MakeStallBtn");
		m_pSelf->m_ItemBox.EnableTracePutPos(true);
	}
}

void KUiItem::OnClickItem(KUiDraggedObject* pItem, bool bDoImmed)
{
	if (pItem == NULL || g_pCoreShell == NULL)
		return;
	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre = pItem->uGenre;
	Obj.Obj.uId = pItem->uId;
	Obj.Region.h = pItem->DataX;
	Obj.Region.v = pItem->DataY;
	Obj.Region.Width  = pItem->DataW;
	Obj.Region.Height = pItem->DataH;
	Obj.eContainer = UOC_ITEM_TAKE_WITH;

	if (bDoImmed == false)
	{
		KUiItemBuySelInfo	Price = { 0 };
		if (g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE,
			(unsigned int)(&Obj), (int)(&Price)))
		{
			KUiTradeConfirm::OpenWindow(&Obj, &Price, TCA_SALE);
		}
	}
	else
	{
		DWORD dwStatus = g_UiBase.GetStatus();
		if (dwStatus & UIS_S_TRADE_SALE || dwStatus & UIS_S_TRADE_NPC)
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL,
				(unsigned int)(&Obj), 0);
		}
		else if (g_UiBase.IsOperationEnable(UIS_O_USE_ITEM))
		{
			g_pCoreShell->OperationRequest(GOI_USE_ITEM,
				(unsigned int)(&Obj), UOC_ITEM_TAKE_WITH);
		}
	}
}

void KUiItem::OnRepairItem(KUiDraggedObject* pItem)
{
	if (pItem == NULL || g_pCoreShell == NULL)
		return;
	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre = pItem->uGenre;
	Obj.Obj.uId = pItem->uId;
	Obj.Region.h = pItem->DataX;
	Obj.Region.v = pItem->DataY;
	Obj.Region.Width  = pItem->DataW;
	Obj.Region.Height = pItem->DataH;
	Obj.eContainer = UOC_ITEM_TAKE_WITH;

	KUiItemBuySelInfo	Price = { 0 };
	if (g_pCoreShell->GetGameData(GDI_REPAIR_ITEM_PRICE,
		(unsigned int)(&Obj), (int)(&Price)))
	{
		KUiTradeConfirm::OpenWindow(&Obj, &Price, TCA_REPAIR);
	}
}

// -------------------------------------------------------------------------
// 功能	: 窗口函数
// -------------------------------------------------------------------------
int KUiItem::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_LEFT_CLICK_ITEM:
		if (g_UiBase.GetStatus() & UIS_S_TRADE_SALE)
			OnClickItem((KUiDraggedObject*)uParam, true);
		else if (g_UiBase.GetStatus() & UIS_S_TRADE_NPC)
			OnClickItem((KUiDraggedObject*)uParam, false);
		else if (g_UiBase.GetStatus() & UIS_S_TRADE_REPAIR)
			OnRepairItem((KUiDraggedObject*)uParam);
		else if (g_UiBase.GetStatus() & UIS_S_STALL_SETPRICE)
			OnMarkPrice((KUiDraggedObject*)uParam);
		break;
	case WND_N_RIGHT_CLICK_ITEM:
		OnClickItem((KUiDraggedObject*)uParam, true);
		break;
	case WND_N_ITEM_PICKDROP:
		OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		break;
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
		{
			KUiStallMarkPrice::CloseWindow(true);
			KUiStall::CloseWindow(TRUE);
			KUiShop::CloseWindow();
			g_UiBase.ReleaseStatus(m_nStatusHandle);
			m_ItemBox.EnablePickPut(true);
			m_MarkPriceBtn.CheckButton(false);
			Hide();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_OpenStatusPadBtn)
			KShortcutKeyCentre::ExcuteScript(SCK_SHORTCUT_STATUS);
		else if (uParam == (unsigned int)(KWndWindow*)&m_GetMoneyBtn)
		{
			if (KUiStoreBox::GetIfVisible())
				KUiGetMoney::OpenWindow(0, m_nMoney, this, UIITEM_WAIT_GETMONEY, &m_Money);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_MarkPriceBtn)
		{
#ifndef _SHUTDOWN_STALL
			if (g_UiBase.GetStatus() & UIS_S_STALL_SETPRICE)
			{
				KUiStallMarkPrice::CloseWindow(true);
				g_UiBase.ReleaseStatus(m_nStatusHandle);
				m_ItemBox.EnablePickPut(true);
				m_MarkPriceBtn.CheckButton(false);
			}
			else if (g_pCoreShell->GetGameData(GDI_IS_STALL, -1, -1) == 0 && !g_UiBase.GetStatus())
			{
				g_UiBase.SetStatus(UIS_S_STALL_SETPRICE, m_nStatusHandle);
				m_MarkPriceBtn.CheckButton(true);
				m_ItemBox.EnablePickPut(false);
			}
#endif
		}
		//摆摊:如果点击输入广告按钮
		else if(uParam == (unsigned int)(KWndWindow*)&m_MakeAdvBtn)
		{
#ifndef _SHUTDOWN_STALL
			int nStallState = g_pCoreShell->GetGameData(GDI_IS_STALL, -1, -1);
			if (nStallState == 0 && (!g_UiBase.GetStatus() || UIS_S_STALL_SETPRICE & g_UiBase.GetStatus()))
				KUiStallSetAdv::OpenWindow(true);
#endif
		}
		//摆摊:如果点击开始摆摊按钮
		else if (uParam == (unsigned int)(KWndWindow*)&m_MakeStallBtn)
		{
#ifndef _SHUTDOWN_STALL
			int nStallState = g_pCoreShell->GetGameData(GDI_IS_STALL, -1, -1);
			m_MakeStallBtn.CheckButton(FALSE);
			if ((nStallState == 1 || nStallState == 0) &&
				KUiStallSetAdv::GetIfVisible() == NULL &&
				((!g_UiBase.GetStatus() || UIS_S_STALL_SETPRICE & g_UiBase.GetStatus())) &&
				!(KUiStoreBox::GetIfVisible()) && !(KUiShop::GetIfVisible()))
			{
				m_MakeStallBtn.CheckButton(nStallState == 1);
				if (g_UiBase.GetStatus() & UIS_S_STALL_SETPRICE)
				{
					KUiStallMarkPrice::CloseWindow(true);
					g_UiBase.ReleaseStatus(m_nStatusHandle);
					m_ItemBox.EnablePickPut(true);
					m_MarkPriceBtn.CheckButton(false);
				}
				g_pCoreShell->OperationRequest(GOI_SWITCH_STALL, 0, 0);
			}
#endif
		}
		break;
	case WND_M_OTHER_WORK_RESULT:
		if (uParam == UIITEM_WAIT_GETMONEY)
			OnGetMoney(nParam);
		break;
	case WM_KEYDOWN:
		if (uParam == VK_ESCAPE)
		{
			if (g_UiBase.GetStatus() & UIS_S_STALL_SETPRICE)
			{
				m_MarkPriceBtn.CheckButton(FALSE);
				g_UiBase.ReleaseStatus(m_nStatusHandle);
				m_ItemBox.EnablePickPut(true);
			}
		}
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiItem::OnGetMoney(int nMoney)
{
	if (nMoney > 0 && KUiStoreBox::GetIfVisible())
	{
		g_pCoreShell->OperationRequest(GOI_MONEY_INOUT_STORE_BOX,
			true, nMoney);
	}
}

void KUiItem::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	if (!g_UiBase.IsOperationEnable(UIS_O_MOVE_ITEM) &&
		!g_UiBase.IsOperationEnable(UIS_O_TRADE_ITEM))
		return;
	KUiObjAtContRegion	Pick, Drop;
	KUiDraggedObject	Obj;

	DWORD dwStatus = g_UiBase.GetStatus();
	if (pPickPos)
	{
		_ASSERT(pPickPos->pWnd);
		((KWndObjectMatrix*)(pPickPos->pWnd))->GetObject(
			Obj, pPickPos->h, pPickPos->v);
		Pick.Obj.uGenre = Obj.uGenre;
		Pick.Obj.uId = Obj.uId;
		Pick.Region.Width = Obj.DataW;
		Pick.Region.Height = Obj.DataH;
		Pick.Region.h = Obj.DataX;
		Pick.Region.v = Obj.DataY;
		Pick.eContainer = UOC_ITEM_TAKE_WITH;

		if (dwStatus & UIS_S_TRADE_SALE)
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL,
				(unsigned int)(&Pick), 0);
			return;
		}
		else if (dwStatus & UIS_S_TRADE_REPAIR)
		{
			g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR,
				(unsigned int)(&Pick), 0);
			return;
		}
		else if (dwStatus & UIS_S_TRADE_BUY)
			return;
	}

	if (pDropPos)
	{
		Wnd_GetDragObj(&Obj);
		Drop.Obj.uGenre = Obj.uGenre;
		Drop.Obj.uId = Obj.uId;
		Drop.Region.Width = Obj.DataW;
		Drop.Region.Height = Obj.DataH;
		Drop.Region.h = pDropPos->h;
		Drop.Region.v = pDropPos->v;
		Drop.eContainer = UOC_ITEM_TAKE_WITH;
	}

	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}

void KUiItem::OnMarkPrice(KUiDraggedObject* pItem)
{
	if (pItem == NULL || g_pCoreShell == NULL)
		return;
	KUiObjAtContRegion	Obj;
	Obj.Obj.uGenre = pItem->uGenre;
	Obj.Obj.uId = pItem->uId;
	Obj.Region.h = pItem->DataX;
	Obj.Region.v = pItem->DataY;
	Obj.Region.Width  = pItem->DataW;
	Obj.Region.Height = pItem->DataH;
	Obj.eContainer = UOC_ITEM_TAKE_WITH;

	KUiItemBuySelInfo	Price = { 0 };
	g_pCoreShell->GetGameData(GDI_STALL_ITEM_PRICE,	(unsigned int)(&Obj), (int)(&Price));
	if (Price.nPrice < 0)
		Price.nPrice = 0;
	KUiStallMarkPrice::SetObjectName(Price.szItemName);
	KUiStallMarkPrice::OpenWindow(Price.nPrice, 99999999, Obj.Obj.uId,
								  UIITEM_WAIT_GETMONEY, &m_Money);
	return;
}

void KUiItem::UpdateStallState(bool bIsStall)
{
	m_ItemBox.EnablePickPut(!bIsStall);
	m_MakeStallBtn.CheckButton(bIsStall);
}

