#include "stdafx.h"
#include "./wndpageset.h"
#include "./wndmessage.h"
#include "./wndcheckbox.h"
#include "../common/ksmallobjectpool.h"
#include "../KGUI.h"

namespace
{
	KSmallObjectPool	g_Allocator("KWndPageSet", sizeof(UI::KWndPageSet));
}

namespace UI
{

#ifdef KGUI_LUA_LOADER
int KWndPageSet::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
{
    int nResult  = false;
    int nRetCode = false;

    KWndWindow *pWnd = NULL;

    KGLOG_PROCESS_ERROR(ppObj);

    pWnd = new(std::nothrow) KWndPageSet;
    KGLOG_PROCESS_ERROR(pWnd);

    if (pLua)
    {
        nRetCode = pWnd->Init(pLua, nTableIndex);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    *ppObj = pWnd;
    // Exit1:
    nResult = true;
Exit0:
    if (!nResult)
    {
        KG_DELETE(pWnd);
    }
    return nResult;
}
#endif

//------------------------------------------------------------------------------------------
//		多个页面集合窗口
//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndPageSet::KWndPageSet()
: m_nAcitvePage(-1)
, m_nLastAcitvePage(-1)
{
}

//--------------------------------------------------------------------------
//	功能：析构函数
//--------------------------------------------------------------------------
KWndPageSet::~KWndPageSet()
{
}

void* KWndPageSet::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
{ 
	return g_Allocator.Alloc(); 
}

void KWndPageSet::operator delete(void* pData, std::nothrow_t const&) throw()
{ 
	g_Allocator.Free(pData); 
}

void* KWndPageSet::operator new(size_t /* nSize */)
{ 
	return g_Allocator.Alloc(); 
}

void KWndPageSet::operator delete(void* pData) throw()
{ 
	g_Allocator.Free(pData); 
}

#ifdef KGUI_LUA_LOADER
int KWndPageSet::Init(Lua_State *pLua, int nTableIndex)
{
	int nResult  = false;
	int nRetCode = false;

    int           nTopIndex  = 0;
    int           nPageCount = 0;
    KWndWindow   *pPage      = NULL;
    KWndCheckBox *pCheckBox  = NULL;

    char szPage[WND_SECTION_NAME_LEN];
    char szCheckBox[WND_SECTION_NAME_LEN];

    KLuaTableField aParamTable[] = 
    {
        KLUA_TABLE_FIELD("PageCount", emVAR_TYPE_INT, 0, &nPageCount, 0),
    };

    KGLOG_PROCESS_ERROR(pLua);

    nTopIndex = lua_gettop(pLua);

    nRetCode = KWndWindow::Init(pLua, nTableIndex);
    KG_PROCESS_ERROR(nRetCode);

    for (int i = 0; i < _countof(aParamTable); i++)
    {
        nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    for (int nPage = 0; nPage < nPageCount; nPage++)
    {
        nRetCode = _snprintf(szPage, _countof(szPage), "Page_%d", nPage);
        szPage[_countof(szPage) - 1] = '\0';
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = _snprintf(szCheckBox, _countof(szCheckBox), "CheckBox_%d", nPage);
        szCheckBox[_countof(szCheckBox) - 1] = '\0';
        KGLOG_PROCESS_ERROR(nRetCode);

        lua_getfield(pLua, nTableIndex, szPage);
        nRetCode = lua_isnil(pLua, -1);
        KGLOG_PROCESS_ERROR(!nRetCode);
        nRetCode = g_pUI->m_LuaWndBuilder.CreateFrame(pLua, -1, (IKUIObject **)&pPage);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(pPage);
        lua_pop(pLua, 1);

        lua_getfield(pLua, nTableIndex, szCheckBox);
        nRetCode = lua_isnil(pLua, -1);
        KGLOG_PROCESS_ERROR(!nRetCode);
        nRetCode = g_pUI->m_LuaWndBuilder.CreateFrame(pLua, -1, (IKUIObject **)&pCheckBox);
        KGLOG_PROCESS_ERROR(nRetCode);
        KGLOG_PROCESS_ERROR(pCheckBox);
        lua_pop(pLua, 1);

        nRetCode = AddPage(pPage, pCheckBox);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

// Exit1:
	nResult = true;
Exit0:
    if (!nResult)
    {
        SAFE_RELEASE(pPage);
        SAFE_RELEASE(pCheckBox);
    }
    lua_settop(pLua, nTopIndex);
	return nResult;
}
#endif


//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
LRESULT KWndPageSet::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg != WND_N_BUTTON_CLICK)
		return KWndWindow::WndProc(uMsg, wParam, lParam);
	OnPageBtnClick((KWndWindow*)wParam);
	return TRUE;
}

//--------------------------------------------------------------------------
//	功能：添加页面
//--------------------------------------------------------------------------
int KWndPageSet::AddPage(KWndWindow* pWndPage, KWndCheckBox* pWndCheckBox)
{
	int nResult = false;
	KWndPageSet *pWndOriginalParent = NULL;
	KWNDPAGECHECKBOXPAIR WndPageCheckBox;

	KG_PROCESS_ERROR(pWndPage);
	KG_PROCESS_ERROR(pWndCheckBox);

	pWndOriginalParent = (KWndPageSet*)CastWndWindow(pWndPage->GetParent(), "WndPageSet");
	if (pWndOriginalParent)
		pWndOriginalParent->RemovePage(pWndPage);

	pWndOriginalParent = (KWndPageSet*)CastWndWindow(pWndCheckBox->GetParent(), "WndPageSet");
	if (pWndOriginalParent)
		pWndOriginalParent->RemovePage(pWndCheckBox);

	AddChild(pWndPage);
	AddChild(pWndCheckBox);
	KGLOG_PROCESS_ERROR(!IsPageExist(pWndPage, pWndCheckBox));

	WndPageCheckBox.pWndPage = pWndPage;
	WndPageCheckBox.pWndCheckBox = pWndCheckBox;

	m_aPageCheckBox.push_back(WndPageCheckBox);

	if ((int)(m_aPageCheckBox.size()) == 1)	//只有这个刚加入的页面
		ActivePage(0);
	else
	{
		pWndPage->Hide();				
		int	nActivePage = m_nAcitvePage;
		m_nAcitvePage = -1;
		ActivePage(nActivePage);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KWndPageSet::IsPageExist(KWndWindow* pWndPage, KWndCheckBox *pWndCheckBox)
{
	KWndPageCheckBoxArray::iterator it = m_aPageCheckBox.begin();
	KWndPageCheckBoxArray::iterator ie = m_aPageCheckBox.end();
	for (; it != ie; ++it)
	{
		if ((*it).pWndPage == pWndPage && (*it).pWndCheckBox == pWndCheckBox)
		{
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：激活指定的页面
//--------------------------------------------------------------------------
int KWndPageSet::ActivePage(int nPageIndex)
{
	int nResult = false;

	KG_PROCESS_ERROR(nPageIndex >= 0);
	KG_PROCESS_ERROR(nPageIndex < (int)(m_aPageCheckBox.size()));

	//需要这一页可以使用
	KG_PROCESS_ERROR(m_aPageCheckBox[nPageIndex].pWndCheckBox->IsCheckBoxActive());

	m_nLastAcitvePage = m_nAcitvePage;

	if (m_nAcitvePage >= 0 && m_nAcitvePage < (int)(m_aPageCheckBox.size()))
		m_aPageCheckBox[m_nAcitvePage].pWndPage->Hide();

	m_nAcitvePage = nPageIndex;
	m_aPageCheckBox[m_nAcitvePage].pWndPage->Show();

	{
		KWndPageCheckBoxArray::iterator it = m_aPageCheckBox.begin();
		KWndPageCheckBoxArray::iterator ie = m_aPageCheckBox.end();
		for (; it != ie; ++it)
		{
			(*it).pWndCheckBox->Check(it == (m_aPageCheckBox.begin() + m_nAcitvePage));
		}
	}

	g_pUI->m_Script.FireWndEvent(this, "OnActivePage");

	nResult = true;
Exit0:
	return nResult;
}

int KWndPageSet::ActivePage(KWndWindow* pWndPage)
{
	int nResult = false;
	int nIndex = GetPageIndex(pWndPage);
	KG_PROCESS_ERROR(nIndex != -1);

	ActivePage(nIndex);

	nResult = true;
Exit0:
	return nResult;
}

int KWndPageSet::ActivePage(KWndCheckBox *pWndCheckBox)
{
	int nResult = false;
	int nIndex = GetPageIndex(pWndCheckBox);
	KG_PROCESS_ERROR(nIndex != -1);

	ActivePage(nIndex);

	nResult = true;
Exit0:
	return nResult;
}

int KWndPageSet::GetPageIndex(KWndWindow* pWndPage)
{
	KWndPageCheckBoxArray::iterator it = m_aPageCheckBox.begin();
	KWndPageCheckBoxArray::iterator ie = m_aPageCheckBox.end();
	for (; it != ie; ++it)
	{
		if ((*it).pWndPage != pWndPage)
			continue;
		return (int)std::distance(m_aPageCheckBox.begin(), it);
	}

	return -1;
}


int KWndPageSet::GetPageIndex(KWndCheckBox *pWndCheckBox)
{
	KWndPageCheckBoxArray::iterator it = m_aPageCheckBox.begin();
	KWndPageCheckBoxArray::iterator ie = m_aPageCheckBox.end();
	for (; it != ie; ++it)
	{
		if ((*it).pWndCheckBox != pWndCheckBox)
			continue;
		return (int)std::distance(m_aPageCheckBox.begin(), it);
	}

	return -1;
}

void KWndPageSet::OnShow()
{
    KWndWindow::OnShow();

    if (m_nAcitvePage >= 0 && m_nAcitvePage < (int)(m_aPageCheckBox.size())) 
    {
		m_aPageCheckBox[m_nAcitvePage].pWndPage->Show();

		KWndPageCheckBoxArray::iterator it = m_aPageCheckBox.begin();
		KWndPageCheckBoxArray::iterator ie = m_aPageCheckBox.end();
		for (; it != ie; ++it)
		{
			(*it).pWndCheckBox->Show();
			(*it).pWndCheckBox->Check( it == (m_aPageCheckBox.begin() + m_nAcitvePage));
		}
    }
}

void KWndPageSet::OnHide()
{
	KWndPageCheckBoxArray::iterator it = m_aPageCheckBox.begin();
	KWndPageCheckBoxArray::iterator ie = m_aPageCheckBox.end();
	for (; it != ie; ++it)
	{
		(*it).pWndCheckBox->Hide();
		(*it).pWndPage->Hide();
	}

    KWndWindow::OnHide();
}

void KWndPageSet::OnPaint()
{
	KWndWindow::OnPaint();
}

int KWndPageSet::RemovePage(int nPageIndex)
{
	int nResult = false;

	KG_PROCESS_ERROR(nPageIndex >= 0);
	KG_PROCESS_ERROR(nPageIndex < (int)(m_aPageCheckBox.size()));

	m_aPageCheckBox.erase(m_aPageCheckBox.begin() + nPageIndex);

	if (m_nLastAcitvePage > nPageIndex)
		m_nLastAcitvePage--;
	else if (m_nLastAcitvePage == nPageIndex)
		m_nLastAcitvePage = -1;

	if (m_nAcitvePage > nPageIndex)
	{
		m_nAcitvePage--;
	}
	else if (m_nAcitvePage == nPageIndex)
	{
		int nSize = (int)m_aPageCheckBox.size();
		if (nSize)
		{
			if (m_nAcitvePage >= nSize)
				ActivePage(nSize - 1);
			else
				ActivePage(m_nAcitvePage);
		}
		else
		{
			m_nAcitvePage = -1;
		}
	}

	nResult = true;
Exit0:
	return nResult;
}

int KWndPageSet::RemovePage(KWndCheckBox *pWndCheckBox)
{
	KWndPageCheckBoxArray::iterator it = m_aPageCheckBox.begin();
	KWndPageCheckBoxArray::iterator ie = m_aPageCheckBox.end();
	for (; it != ie; ++it)
	{
		if ((*it).pWndCheckBox == pWndCheckBox)
		{
			return RemovePage((int)(it - m_aPageCheckBox.begin()));
		}
	}
	return false;
}

int KWndPageSet::RemovePage(KWndWindow *pWndPage)
{
	KWndPageCheckBoxArray::iterator it = m_aPageCheckBox.begin();
	KWndPageCheckBoxArray::iterator ie = m_aPageCheckBox.end();
	for (; it != ie; ++it)
	{
		if ((*it).pWndPage == pWndPage)
		{
			return RemovePage((int)(it - m_aPageCheckBox.begin()));
		}
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：响应按下切换页面按钮的操作
//--------------------------------------------------------------------------
void KWndPageSet::OnPageBtnClick(KWndWindow* pCheckBox)
{
	KWndPageCheckBoxArray::iterator it = m_aPageCheckBox.begin();
	KWndPageCheckBoxArray::iterator ie = m_aPageCheckBox.end();
	for (; it != ie; ++it)
	{
		if ((*it).pWndCheckBox == pCheckBox)
		{
			ActivePage((int)(std::distance(m_aPageCheckBox.begin(), it)));
			break;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：激活指定的页面
//--------------------------------------------------------------------------
KWndWindow* KWndPageSet::GetActivePage()
{
	if (m_nAcitvePage >= 0 && m_nAcitvePage < (int)(m_aPageCheckBox.size()))
		return m_aPageCheckBox[m_nAcitvePage].pWndPage;
	return NULL;
}

KWndCheckBox* KWndPageSet::GetActiveCheckBox()
{
	if (m_nAcitvePage >= 0 && m_nAcitvePage < (int)(m_aPageCheckBox.size()))
		return m_aPageCheckBox[m_nAcitvePage].pWndCheckBox;
	return NULL;
}

int KWndPageSet::CanCastTo(LPCSTR szType) const
{
	return szType == "WndPageSet" || szType == "WndWindow";
}


LPCSTR KWndPageSet::GetWndType() const
{
	return "WndPageSet";
}

}

