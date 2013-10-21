/*****************************************************************************************
//	界面--储物箱界面
//	Copyright : Kingsoft 2003
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2003-4-21
*****************************************************************************************/
#include "KWin32.h"
#include "KIniFile.h"
#include "../elem/wnds.h"
#include "UiStoreBox.h"
#include "UiGetMoney.h"
#include "UiItem.h"
#include "../../../core/src/coreshell.h"
#include "../../../core/src/GameDataDef.h"
#include "../UiSoundSetting.h"
#include "../UiBase.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*		g_pCoreShell;

#define SCHEME_INI_ITEM	"储物箱.ini"

KUiStoreBox* KUiStoreBox::m_pSelf = NULL;

enum WAIT_OTHER_WND_OPER_PARAM
{
	UISTOREBOX_WAIT_GETMONEY,
};

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiStoreBox* KUiStoreBox::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiStoreBox* KUiStoreBox::OpenWindow()
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiStoreBox;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		if (KUiItem::GetIfVisible() == NULL)
			KUiItem::OpenWindow();
		else
			UiSoundPlay(UI_SI_WND_OPENCLOSE);

		m_pSelf->UpdateData();
		m_pSelf->BringToTop();
		m_pSelf->Show();
		g_UiBase.SetStatus(UIS_S_STORE_BOX, m_pSelf->m_nStatusHandle);
	}
	return m_pSelf;
}

//--------------------------------------------------------------------------
//	功能：关闭窗口
//--------------------------------------------------------------------------
void KUiStoreBox::CloseWindow()
{
	if (m_pSelf)
	{
		g_UiBase.ReleaseStatus(m_pSelf->m_nStatusHandle);
		m_pSelf->Destroy();
		m_pSelf = NULL;
	}
}

// -------------------------------------------------------------------------
// 功能	: 初始化
// -------------------------------------------------------------------------
void KUiStoreBox::Initialize()
{
	AddChild(&m_Money);
	AddChild(&m_GetMoneyBtn);
	AddChild(&m_CloseBtn);
	AddChild(&m_ItemBox);

	m_ItemBox.SetContainerId((int)UOC_STORE_BOX);
	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	m_nMoney = 0;
	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
void KUiStoreBox::UpdateData()
{
	m_ItemBox.Clear();
	m_nMoney = 0;
	m_Money.SetText("0");

	KUiObjAtRegion* pObjs = NULL;

	int nCount = g_pCoreShell->GetGameData(GDI_ITEM_IN_STORE_BOX, 0, 0);
	if (nCount == 0)
		return;

	if (pObjs = (KUiObjAtRegion*)malloc(sizeof(KUiObjAtRegion) * nCount))
	{
		g_pCoreShell->GetGameData(GDI_ITEM_IN_STORE_BOX, (unsigned int)pObjs, nCount);//单线程执行，nCount值不变
		for (int i = 0; i < nCount; i++)
			UpdateItem(&pObjs[i], true);
		free(pObjs);
		pObjs = NULL;
	}
}

// -------------------------------------------------------------------------
// 功能	: 物品变化更新
// -------------------------------------------------------------------------
void KUiStoreBox::UpdateItem(KUiObjAtRegion* pItem, int bAdd)
{
	if (pItem)
	{
		UiSoundPlay(UI_SI_PICKPUT_ITEM);
		if (pItem->Obj.uGenre != CGOG_MONEY)
		{
			KUiDraggedObject Obj;
			Obj.uGenre = pItem->Obj.uGenre;
			Obj.uId = pItem->Obj.uId;
			Obj.DataX = pItem->Region.h;
			Obj.DataY = pItem->Region.v;
			Obj.DataW = pItem->Region.Width;
			Obj.DataH = pItem->Region.Height;
			if (bAdd)
				m_ItemBox.AddObject(&Obj, 1);
			else
				m_ItemBox.RemoveObject(&Obj);
		}
		else
		{
			m_nMoney = pItem->Obj.uId;
			//m_Money.SetIntText(m_nMoney);
			m_Money.SetFormatText(m_nMoney);
		}
	}
	else
		UpdateData();
}

// -------------------------------------------------------------------------
// 功能	: 载入界面方案
// -------------------------------------------------------------------------
void KUiStoreBox::LoadScheme(const char* pScheme)
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
		m_pSelf->m_ItemBox.Init(&Ini, "ItemBox");
		m_pSelf->m_ItemBox.EnableTracePutPos(true);
	}
}

// -------------------------------------------------------------------------
// 功能	: 窗口函数
// -------------------------------------------------------------------------
int KUiStoreBox::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	switch(uMsg)
	{
	case WND_N_ITEM_PICKDROP:
		OnItemPickDrop((ITEM_PICKDROP_PLACE*)uParam, (ITEM_PICKDROP_PLACE*)nParam);
		break;
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_CloseBtn)
			CloseWindow();
		else if (uParam == (unsigned int)(KWndWindow*)&m_GetMoneyBtn)
		{
			KUiGetMoney::OpenWindow(0, m_nMoney, this, UISTOREBOX_WAIT_GETMONEY, &m_Money);
		}
		break;
	case WND_M_OTHER_WORK_RESULT:
		if (uParam == UISTOREBOX_WAIT_GETMONEY)
			OnGetMoney(nParam);
		break;
	default:
		return KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

void KUiStoreBox::OnGetMoney(int nMoney)
{
	if (nMoney > 0)
	{
		g_pCoreShell->OperationRequest(GOI_MONEY_INOUT_STORE_BOX,
			false, nMoney);
	}
}

void KUiStoreBox::OnItemPickDrop(ITEM_PICKDROP_PLACE* pPickPos, ITEM_PICKDROP_PLACE* pDropPos)
{
	if (g_UiBase.GetStatus() & ~UIS_S_STORE_BOX)
		return;
	KUiObjAtContRegion	Pick, Drop;
	KUiDraggedObject	Obj;

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
		Pick.eContainer = UOC_STORE_BOX;
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
		Drop.eContainer = UOC_STORE_BOX;
	}

	g_pCoreShell->OperationRequest(GOI_SWITCH_OBJECT,
		pPickPos ? (unsigned int)&Pick : 0,
		pDropPos ? (int)&Drop : 0);
}