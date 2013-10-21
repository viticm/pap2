// -------------------------------------------------------------------------
//	文件名		：	交易确认框
//	创建者		：	Wooy(Wu yue)
//	创建时间	：	2003-5-28
// -------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/AutoLocateWnd.h"
#include "../Elem/WndMessage.h"
#include "../Elem/Wnds.h"
#include "../UiBase.h"
#include "UiTradeConfirmWnd.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/coreshell.h"
#include "UiShop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

#define	SCHEME_INI 	"买卖确认.ini"
#define	MAX_BUY_COUNT	60

KUiTradeConfirm* KUiTradeConfirm::m_pSelf = NULL;

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiTradeConfirm* KUiTradeConfirm::OpenWindow(KUiObjAtContRegion* pObj,
			KUiItemBuySelInfo* pPriceInfo, TRADE_CONFIRM_ACTION eAction)
{
	if (pObj == NULL || pPriceInfo == NULL)
		return NULL;
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiTradeConfirm;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		m_pSelf->m_BuyImg.Hide();
		m_pSelf->m_SaleImg.Hide();
		m_pSelf->m_RepairImg.Hide();
		m_pSelf->m_BuyCount.Hide();
		m_pSelf->m_BtnIncrease.Hide();
		m_pSelf->m_BtnDecrease.Hide();
		m_pSelf->m_ItemInfo = *pObj;
		m_pSelf->m_PriceInfo = *pPriceInfo;
		m_pSelf->m_Money.SetIntText(pPriceInfo->nPrice);

		if (eAction == TCA_SALE)
		{
			m_pSelf->m_SaleImg.Show();
			m_pSelf->m_OkBtn.Enable(true);
			m_pSelf->m_Money.SetTextColor(m_pSelf->m_uNormalPriceColor);
		}
		else
		{
			if (eAction == TCA_BUY)
			{
				m_pSelf->m_BuyImg.Show();
				if (m_pSelf->m_ItemInfo.eContainer != UOC_STALL_OTHER)
				{
					m_pSelf->m_BuyCount.SetIntText(1);
					m_pSelf->m_BuyCount.Show();
					m_pSelf->m_BtnIncrease.Show();
					m_pSelf->m_BtnDecrease.Show();
				}
			}
			else
				m_pSelf->m_RepairImg.Show();
			int nHoldMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
			m_pSelf->m_OkBtn.Enable(nHoldMoney >= pPriceInfo->nPrice);
			m_pSelf->m_Money.SetTextColor((nHoldMoney >= pPriceInfo->nPrice) ?
				m_pSelf->m_uNormalPriceColor : m_pSelf->m_uNotEnoughMoneyPriceColor);
		}
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

KUiTradeConfirm::KUiTradeConfirm()
{
	m_PriceInfo.szItemName[0] = 0;
	m_PriceInfo.nPrice = 0;
	m_ItemInfo.Obj.uGenre = CGOG_NOTHING;
}

//如果窗口正被显示，则返回实例指针
KUiTradeConfirm* KUiTradeConfirm::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：关闭销毁窗口
//--------------------------------------------------------------------------
void KUiTradeConfirm::CloseWindow(bool bDestroy)
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


//显示窗口
void KUiTradeConfirm::Show()
{
	m_ItemName.SetText(m_PriceInfo.szItemName);
	int Left, Top;
	ALW_GetWndPosition(Left, Top, m_Width, m_Height);
	SetPosition(Left, Top);
	KWndImage::Show();
	Wnd_SetExclusive((KWndWindow*)this);
}

//隐藏窗口
void KUiTradeConfirm::Hide()
{
	Wnd_ReleaseExclusive((KWndWindow*)this);
	KWndImage::Hide();
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KUiTradeConfirm::Initialize()
{
	AddChild(&m_ItemName);
	AddChild(&m_Money);
	AddChild(&m_BuyImg);
	AddChild(&m_SaleImg);
	AddChild(&m_RepairImg);
	AddChild(&m_OkBtn);
	AddChild(&m_CancelBtn);
	AddChild(&m_BuyCount);
	AddChild(&m_BtnIncrease);
	AddChild(&m_BtnDecrease);

	Wnd_AddWindow(this, WL_TOPMOST);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	return true;
}

//--------------------------------------------------------------------------
//	功能：载入窗口的界面方案
//--------------------------------------------------------------------------
void KUiTradeConfirm::LoadScheme(const char* pScheme)
{
	char		Buff[128];
	KIniFile	Ini;
	sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI);
	if (Ini.Load(Buff))
	{
		Init(&Ini, "Main");
		m_ItemName.Init(&Ini, "ItemName");

		m_Money.Init(&Ini, "Price");
		Ini.GetString("Price", "Color", "", Buff, sizeof(Buff));
		m_uNormalPriceColor = GetColor(Buff);
		Ini.GetString("Price", "CantBuyColor", "", Buff, sizeof(Buff));
		m_uNotEnoughMoneyPriceColor = GetColor(Buff);

		m_BuyImg.Init(&Ini, "BuyImg");
		m_SaleImg.Init(&Ini, "SaleImg");
		m_RepairImg.Init(&Ini, "RepairImg");
		m_OkBtn.Init(&Ini, "OkBtn");
		m_CancelBtn.Init(&Ini, "CancelBtn");
		m_BuyCount.Init(&Ini, "BuyNumber");
		m_BtnIncrease.Init(&Ini, "Increase");
		m_BtnDecrease.Init(&Ini, "Decrease");
		if (m_BuyImg.IsVisible())
			OnCountInputChanged();//为了重设m_Money内容
		else
			m_Money.SetIntText(m_PriceInfo.nPrice);
	}
}

//--------------------------------------------------------------------------
//	功能：窗口消息函数
//--------------------------------------------------------------------------
int KUiTradeConfirm::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_OkBtn)
		{
			if (m_BuyImg.IsVisible())
				OnBuy();
			else if (m_SaleImg.IsVisible())
				OnSale();
			else
				OnRepair();
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_CancelBtn)
			OnCancel();
		else if(uParam == (unsigned int)&m_BtnIncrease)
		{
			OnModifyCount(TRUE);
		}
		else if(uParam == (unsigned int)&m_BtnDecrease)
		{
			OnModifyCount(FALSE);
		}
		break;
	case WND_N_EDIT_CHANGE:
		OnCountInputChanged();
		break;
	case WM_KEYDOWN:
		if (uParam == VK_RETURN)
		{
			if (m_SaleImg.IsVisible())
			{
				OnSale();
			}
			else if (m_BuyImg.IsVisible())
			{
				if (m_BuyImg.IsDisable())
					OnCancel();
				else
					OnBuy();
			}
			else if (m_RepairImg.IsDisable())
			{
				OnCancel();
			}
			else
			{
				OnRepair();
			}
			nRet = 1;
		}
		else if (uParam == VK_ESCAPE)
		{
			OnCancel();
			nRet = 1;
		}
		break;

	case WND_N_BUTTON_HOLD:
		if(uParam == (unsigned int)&m_BtnIncrease)
			OnModifyCount(true);
		else if(uParam == (unsigned int)&m_BtnDecrease)
			OnModifyCount(false);
		break;

	default:
		nRet = KWndImage::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}

void KUiTradeConfirm::OnCountInputChanged()
{
	int nNewCount;
	int nCount = m_BuyCount.GetIntNumber();
	int nHoldMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	if (m_PriceInfo.nPrice <= 0)
	{
		nNewCount = 0;
	}
	else
	{
		if (nCount * m_PriceInfo.nPrice > nHoldMoney)
			nNewCount = nHoldMoney / m_PriceInfo.nPrice;
		else
			nNewCount = nCount;
		if (nNewCount < 0)
			nNewCount = 0;
	}
	if (nNewCount != nCount)
	{
		m_BuyCount.SetIntText(nNewCount);
	}
	m_Money.SetIntText(nNewCount * m_PriceInfo.nPrice);
}


void KUiTradeConfirm::OnModifyCount(bool bAdd)
{
	if (m_PriceInfo.nPrice <= 0)
		return;
	int nCount = m_BuyCount.GetIntNumber();
	int nHoldMoney = g_pCoreShell->GetGameData(GDI_PLAYER_HOLD_MONEY, 0, 0);
	if (bAdd)
	{
		if (nCount < MAX_BUY_COUNT &&
			(nCount + 1) * m_PriceInfo.nPrice <= nHoldMoney)
		{
			m_BuyCount.SetIntText(nCount + 1);
		}
	}
	else if (nCount > 1)
	{
		m_BuyCount.SetIntText(nCount - 1);
	}
}

//--------------------------------------------------------------------------
//	功能：响应点击买按钮
//--------------------------------------------------------------------------
void KUiTradeConfirm::OnBuy()
{
	//从摊上买物品 [wxb 2003-11-20]
	if (g_pCoreShell && (&m_ItemInfo)->eContainer == UOC_STALL_OTHER)
		g_pCoreShell->OperationRequest(GOI_STALL_BUY_ITEM, m_ItemInfo.Obj.uId, 0);
	else
	{
		KUiShop *pShop;
		KObjForBuy Obj;
		Obj.nCount = m_BuyCount.GetIntNumber();
		if (Obj.nCount > 0 && (pShop = KUiShop::GetIfVisible()))
		{
			memcpy(&Obj, &m_ItemInfo, sizeof(m_ItemInfo));
			Obj.previous = Obj.next = 0;
			pShop->PushBuyAttempt(&Obj);
		}
	}
	CloseWindow(false);
}

//--------------------------------------------------------------------------
//	功能：响应点击卖按钮
//--------------------------------------------------------------------------
void KUiTradeConfirm::OnSale()
{
	if (g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_TRADE_NPC_SELL,
			(unsigned int)(&m_ItemInfo), 0);
	}
	CloseWindow(false);
}

//--------------------------------------------------------------------------
//	功能：响应点修理按钮
//--------------------------------------------------------------------------
void KUiTradeConfirm::OnRepair()
{
	if (g_pCoreShell)
	{
		g_pCoreShell->OperationRequest(GOI_TRADE_NPC_REPAIR,
			(unsigned int)(&m_ItemInfo), 0);
	}
	CloseWindow(false);
}


void KUiTradeConfirm::OnCancel()
{
	CloseWindow(false);
}
