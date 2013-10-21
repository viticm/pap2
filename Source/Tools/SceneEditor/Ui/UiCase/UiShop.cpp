// -------------------------------------------------------------------------
//	文件名		：	UiTrade.h
//	创建者		：	Wooy(Wu yue)
//	创建时间	：	2002-12-21
//	功能描述	：	npc交易系统界面
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "UiShop.h"
#include "UiItem.h"
#include "UiStatus.h"
#include "UiTradeConfirmWnd.h"
#include "../../../core/src/CoreObjGenreDef.h"
#include "../../../core/src/coreshell.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"
#include "UiInformation.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI		"npc买卖界面.ini"

KUiShop* KUiShop::m_pSelf = NULL;

KUiShop::KUiShop()
{
	m_pObjsList = NULL;
	m_nObjCount = 0;
	m_nPageCount = 0;
	m_nStatusHandle = 0;
}

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiShop* KUiShop::GetIfVisible()
{
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiShop* KUiShop::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiShop;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		if(KUiStatus::GetIfVisible() == NULL)
			KUiStatus::OpenWindow();
		g_UiBase.SetStatus(UIS_S_TRADE_NPC, m_pSelf->m_nStatusHandle);
		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();

		if (KUiItem::GetIfVisible() == NULL)
			KUiItem::OpenWindow();
		else
			UiSoundPlay(UI_SI_WND_OPENCLOSE);
		KUiItem::OnNpcTradeMode(true);
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiShop::CloseWindow()
{
	if (m_pSelf)
	{
		KUiItem::OnNpcTradeMode(false);
		KUiTradeConfirm::CloseWindow(true);
		g_UiBase.ReleaseStatus(m_pSelf->m_nStatusHandle);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

//初始化
void KUiShop::Initialize()
{
	AddChild(&m_ItemsBox);
	AddChild(&m_BuyBtn);
	AddChild(&m_SellBtn);
	AddChild(&m_RepairBtn);
	AddChild(&m_PreBtn);
	AddChild(&m_NextBtn);
	AddChild(&m_CloseBtn);

	m_ItemsBox.SetContainerId((int)UOC_NPC_SHOP);
	Wnd_AddWindow(this, WL_TOPMOST);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);
}

//载入界面方案
void KUiShop::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\"SCHEME_INI, pScheme);
		if (Ini.Load(Buff))
		{
			m_pSelf->KWndShowAnimate::Init(&Ini, "Main");
			m_pSelf->m_ItemsBox.Init(&Ini, "ItemBox");
			m_pSelf->m_BuyBtn.Init(&Ini, "BuyBtn");
			m_pSelf->m_SellBtn.Init(&Ini, "SellBtn");
			m_pSelf->m_RepairBtn.Init(&Ini, "RepairBtn");
			m_pSelf->m_PreBtn.Init(&Ini, "LeftBtn");
			m_pSelf->m_NextBtn.Init(&Ini,"RightBtn");
			m_pSelf->m_CloseBtn.Init(&Ini, "CloseBtn");

			m_pSelf->m_ItemsBox.EnablePickPut(false);
		}
	}
}

void KUiShop::CancelTrade()
{
	CloseWindow();
}

//窗口函数
int	KUiShop::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch (uMsg)
	{
	case WND_N_BUTTON_CLICK:
		OnClickButton((KWndButton*)(KWndWindow*)uParam, nParam);
		break;
	case WND_N_LEFT_CLICK_ITEM:
		{
			if(g_UiBase.GetStatus() & UIS_S_TRADE_BUY || g_UiBase.GetStatus() & UIS_S_TRADE_NPC)
				OnBuyItem((KUiDraggedObject*)uParam, (g_UiBase.GetStatus() & UIS_S_TRADE_BUY) > 0);
		}
		break;
	case WND_N_RIGHT_CLICK_ITEM:
		if (nParam == (int)(KWndWindow*)&m_ItemsBox)
		{
			if(g_UiBase.GetStatus() & UIS_S_TRADE_BUY || g_UiBase.GetStatus() & UIS_S_TRADE_NPC)
				OnBuyItem((KUiDraggedObject*)uParam, true);
		}
		break;
	case WM_KEYDOWN:
		if (uParam == VK_ESCAPE)
		{
			if (g_UiBase.GetStatus() & UIS_S_TRADE_NPC)
			{
				CloseWindow();
			}
			else
			{
				m_BuyBtn.CheckButton(false);
				m_SellBtn.CheckButton(false);
				m_RepairBtn.CheckButton(false);
				g_UiBase.SetStatus(UIS_S_TRADE_NPC, m_nStatusHandle);
			}
		}
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiShop::OnBuyItem(KUiDraggedObject* pItem, bool bDoImmed)
{
	if (pItem == NULL || g_pCoreShell == NULL)
		return;

	KObjForBuy	Obj;
	Obj.Obj.uGenre = pItem->uGenre;
	Obj.Obj.uId = pItem->uId;
	Obj.Region.h = pItem->DataX;
	Obj.Region.v = pItem->DataY;
	Obj.Region.Width  = pItem->DataW;
	Obj.Region.Height = pItem->DataH;
	Obj.eContainer = UOC_NPC_SHOP;
	Obj.nCount = 1;
	Obj.previous = Obj.next = NULL;

	if (bDoImmed == false)
	{
		KUiItemBuySelInfo	Price = { 0 };
		if (g_pCoreShell->GetGameData(GDI_TRADE_ITEM_PRICE,
			(unsigned int)(&Obj), (int)(&Price)))
		{
			KUiTradeConfirm::OpenWindow((KUiObjAtContRegion *)&Obj, &Price, TCA_BUY);
		}
	}
	else
	{
		PushBuyAttempt(&Obj);
	}
}

void KUiShop::OnClickButton(KWndButton* pWnd, int bCheck)
{
	if (Wnd_GetDragObj(NULL))
		return;
	if (pWnd == &m_BuyBtn)
	{
		if (bCheck)
		{
			m_SellBtn.CheckButton(false);
			m_RepairBtn.CheckButton(false);
			g_UiBase.SetStatus(UIS_S_TRADE_BUY, m_nStatusHandle);
		}
		else
			g_UiBase.SetStatus(UIS_S_TRADE_NPC, m_nStatusHandle);
	}
	else if (pWnd == &m_SellBtn)
	{
		if (bCheck)
		{
			m_BuyBtn.CheckButton(false);
			m_RepairBtn.CheckButton(false);
			g_UiBase.SetStatus(UIS_S_TRADE_SALE, m_nStatusHandle);
		}
		else
			g_UiBase.SetStatus(UIS_S_TRADE_NPC, m_nStatusHandle);
	}
	else if (pWnd == &m_RepairBtn)
	{
		if (bCheck)
		{
			m_BuyBtn.CheckButton(false);
			m_SellBtn.CheckButton(false);
			g_UiBase.SetStatus(UIS_S_TRADE_REPAIR, m_nStatusHandle);
		}
		else
			g_UiBase.SetStatus(UIS_S_TRADE_NPC, m_nStatusHandle);
	}
	else if (pWnd == &m_PreBtn)
		SetPage(m_nCurrentPage - 1);
	else if(pWnd == &m_NextBtn)
		SetPage(m_nCurrentPage + 1);
	else if (pWnd == &m_CloseBtn)
		CloseWindow();
}

void KUiShop::SetPage(int nIndex)
{
	if (nIndex >= 0 && nIndex < m_nPageCount && m_pObjsList)
	{
		m_ItemsBox.Clear();
		for (int i = 0; i < m_nObjCount; i++)
		{
			if (m_pObjsList[i].nContainer == nIndex)
				m_ItemsBox.AddObject((KUiDraggedObject*)&m_pObjsList[i], 1);
		}
		m_nCurrentPage = nIndex;
	}
}

void KUiShop::UpdateData()
{
	Clear();
	m_nObjCount = g_pCoreShell->GetGameData(GDI_TRADE_NPC_ITEM, 0, 0);
	if (m_nObjCount == 0)
		return;

	if (m_pObjsList = (KUiObjAtContRegion*)malloc(sizeof(KUiObjAtContRegion) * m_nObjCount))
	{
		g_pCoreShell->GetGameData(GDI_TRADE_NPC_ITEM, (unsigned int)m_pObjsList, m_nObjCount);//单线程执行，nCount值不变
		m_nPageCount = m_pObjsList[m_nObjCount - 1].nContainer + 1;
		SetPage(0);
		m_PreBtn.Enable(m_nPageCount > 1);
		m_NextBtn.Enable(m_nPageCount > 1);
	}
	else
		m_nObjCount = 0;
}

void KUiShop::Clear()
{
	m_nObjCount  = 0;
	m_nPageCount = 0;
	if (m_pObjsList)
	{
		free(m_pObjsList);
		m_pObjsList = NULL;
	}
}


  /*__(@_         功能：把一次购买的意图入队
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiShop::PushBuyAttempt(KObjForBuy *pObj)
{
	if(!m_BuyList.GetCount())
	{
		Buy(pObj);
	}
	if(pObj->nCount > 0)
	{
		m_BuyList.End();
		m_BuyList.AddItem(*pObj);
	}
}


  /*__(@_      功能：购买一个物品，并且把等待数量减一
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiShop::Buy(KObjForBuy *pObj)
{
	if(g_pCoreShell)
	{
		pObj->nCount --;
		g_pCoreShell->OperationRequest(GOI_TRADE_NPC_BUY,
			                           (unsigned int)(pObj), 0);
	}
}


  /*__(@_  功能：当买进一个物品之后，外部调用这个通知Shop界面
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiShop::ABuyFinish()
{
	KObjForBuy *pObj = m_BuyList.Begin();
	if(pObj)
	{
		Buy(pObj);
		if(pObj->nCount < 1)
		{
			m_BuyList.RemoveItem();
		}
	}
}


  /*__(@_              功能：清空掉购买队列
 /     ) \______________________________________________________
(_)@8@8{}<______________________________________________________>
       )_/
      (*/
void KUiShop::ClearBuyList()
{
	m_BuyList.Clear();
}
