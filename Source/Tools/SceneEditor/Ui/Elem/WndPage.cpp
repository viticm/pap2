/*****************************************************************************************
//	界面窗口体系结构--页面窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-8-9
------------------------------------------------------------------------------------------
    类是于M$ Win32里面的 PropertiePage
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "../Elem/WndMessage.h"
#include "WndButton.h"
#include "WndPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
//		当点击页面切换按钮或在页面空白区时需要将消息转发给PageSet处理
//--------------------------------------------------------------------------
int KWndPage::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if ((uMsg == WM_LBUTTONDOWN || uMsg == WM_MOUSEMOVE ||
		uMsg == WM_LBUTTONUP || uMsg == WND_N_BUTTON_CLICK) && m_pParentWnd)
	{
		return m_pParentWnd->WndProc(uMsg, uParam, nParam);
	}
	return KWndImage::WndProc(uMsg, uParam, nParam);
}

//------------------------------------------------------------------------------------------
//		多个页面集合窗口
//------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndPageSet::KWndPageSet()
{
	m_pPageBtnPairList = NULL;
	m_nNumPage = 0;
	m_nAcitvePage = -1;
}

//--------------------------------------------------------------------------
//	功能：析构函数
//--------------------------------------------------------------------------
KWndPageSet::~KWndPageSet()
{
	if (m_pPageBtnPairList)
	{
		free(m_pPageBtnPairList);
		m_pPageBtnPairList = NULL;
	}
	m_nNumPage = 0;
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KWndPageSet::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (uMsg != WND_N_BUTTON_CLICK)
		return KWndImage::WndProc(uMsg, uParam, nParam);
	OnPageBtnClick((KWndWindow*)uParam);
	return 0;
}

//--------------------------------------------------------------------------
//	功能：添加页面
//--------------------------------------------------------------------------
bool KWndPageSet::AddPage(KWndPage* pPage, KWndButton* pPageBtn)
{
	if (pPage && pPageBtn)
	{
		AddChild(pPage);
		KWndPageBtnPair* pNewList = (KWndPageBtnPair*)realloc(m_pPageBtnPairList, sizeof(KWndPageBtnPair) * (m_nNumPage + 1));
		if (pNewList)
		{
			m_pPageBtnPairList = pNewList;
			m_pPageBtnPairList[m_nNumPage].pPage = pPage;
			m_pPageBtnPairList[m_nNumPage].pPageBtn = pPageBtn;
			m_nNumPage++;
			if ((m_nNumPage) == 1)	//只有这个刚加入的页面
				ActivePage(0);
			else
			{
				pPage->Hide();
				int	nActivePage = m_nAcitvePage;
				m_nAcitvePage = -1;
				ActivePage(nActivePage);
			}
			return true;
		}
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：激活指定的页面
//--------------------------------------------------------------------------
bool KWndPageSet::ActivePage(int nPageIndex)
{
	if (nPageIndex >= 0 && nPageIndex < m_nNumPage && nPageIndex != m_nAcitvePage)
	{
		if (m_nAcitvePage >= 0)
			m_pPageBtnPairList[m_nAcitvePage].pPage->Hide();
		m_nAcitvePage = nPageIndex;
		m_pPageBtnPairList[m_nAcitvePage].pPage->Show();
		for (int i = 0; i < m_nNumPage; i++)
		{
			m_pPageBtnPairList[i].pPageBtn->SplitSmaleFamily();
			AddChild(m_pPageBtnPairList[i].pPageBtn);
			m_pPageBtnPairList[i].pPageBtn->CheckButton(i == nPageIndex);
		}
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：响应按下切换页面按钮的操作
//--------------------------------------------------------------------------
void KWndPageSet::OnPageBtnClick(KWndWindow* pBtn)
{
	for (int i = 0; i < m_nNumPage; i++)
	{
		if (m_pPageBtnPairList[i].pPageBtn == pBtn)
		{
			if (i != m_nAcitvePage)
				ActivePage(i);
			for (i = 0; i < m_nNumPage; i++)
				m_pPageBtnPairList[i].pPageBtn->CheckButton(m_pPageBtnPairList[i].pPageBtn == pBtn);
			break;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：激活指定的页面
//--------------------------------------------------------------------------
KWndPage* KWndPageSet::GetActivePage()
{
	if (m_nAcitvePage >= 0)
		return m_pPageBtnPairList[m_nAcitvePage].pPage;
	else
		return NULL;
}