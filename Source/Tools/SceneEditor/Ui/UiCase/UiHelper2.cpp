/*
 * File:     KUiHelper.cpp
 * Desc:     详细帮助窗口
 * Author:   flying
 * Creation: 2003/7/16
 */
//////////////////////////////////////////////////////////////////////////
//                     帮助界面文件名字的历史来历
// 为什么详细帮助界面叫做Helper2而由详细帮助界面激活的图片帮助界面叫做
// Helper呢？这是有原因的。什么原因呢？最初的时候传说需要一个类似Diablo
// 按H弹出的关于按钮的图片帮助。这也是最初所理解的帮助系统，而且在那时
// 实现，名字自然叫做Helper了。之后需要一个索引类型的帮助，而且把原先所
// 要求的帮助系统转移到那里去激活，于是在开发时间上在图片帮助之后的索引
// 帮助就叫做Helper2了。口合  口合
//////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
#include "KWin32.h"
#include "KIniFile.h"
#include "../Elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../Elem/WndMessageListBox.h"
#include "../UiBase.h"
#include "UiHelper2.h"
#include "UiHelper.h"
#include "../UiSoundSetting.h"
#include "../../../core/src/gamedatadef.h"
#include "../../../core/src/CoreShell.h"
#include "../ShortcutKey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;
extern iCoreShell* g_pCoreShell;
//extern IInlinePicEngineSink* g_pIInlinePicSink;
//extern KMouseOver	g_MouseOver;
KUiHelper2* KUiHelper2::m_pSelf = NULL;

#define		SCHEME_INI_HELP2	"详细帮助界面.ini"
#define		DETAIL_HELP_LIST	"详细帮助项目.ini"

//--------------------------------------------------------------------------
//	功能：如果窗口正被显示，则返回实例指针
//--------------------------------------------------------------------------
KUiHelper2* KUiHelper2::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	return NULL;
}

KUiHelper2::KUiHelper2()
{
	m_nIndexCount = 0;
}

KUiHelper2::~KUiHelper2()
{
	Clear();
}

//--------------------------------------------------------------------------
//	功能：打开窗口，返回唯一的一个类对象实例
//--------------------------------------------------------------------------
KUiHelper2* KUiHelper2::OpenWindow(bool bShow)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KUiHelper2;
		if (m_pSelf)
			m_pSelf->Initialize();
	}
	if (m_pSelf)
	{
		UiSoundPlay(UI_SI_WND_OPENCLOSE);
		m_pSelf->BringToTop();
		m_pSelf->Show();
	}
	return m_pSelf;
}

//初始化
void KUiHelper2::Initialize()
{
	AddChild(&m_BtnClose);
	AddChild(&m_BtnPic);
	AddChild(&m_BtnKeyboard);
	AddChild(&m_BtnWuxing);
	AddChild(&m_IndexList);
	AddChild(&m_IndexScroll);
	AddChild(&m_MessageList);
	m_IndexList.SetScrollbar(&m_IndexScroll);
	AddChild(&m_MessageScroll);
	m_MessageList.SetScrollbar(&m_MessageScroll);

	char Scheme[256];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	LoadScheme(Scheme);

	Wnd_AddWindow(this);
}

//--------------------------------------------------------------------------
//	功能：关闭窗口，同时可以选则是否删除对象实例
//--------------------------------------------------------------------------
void KUiHelper2::CloseWindow(bool bDestory)
{
	if (m_pSelf)
	{
		m_pSelf->Hide();
		if (bDestory)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

void KUiHelper2::LoadScheme(const char* pScheme)
{
	if (m_pSelf)
	{
		char		Buff[128];
		KIniFile	Ini;
		sprintf(Buff, "%s\\%s", pScheme, SCHEME_INI_HELP2);
		if (Ini.Load(Buff))
		{
			m_pSelf->Init(&Ini, "Main");
			m_pSelf->m_BtnClose.Init(&Ini, "BtnClose");
			m_pSelf->m_BtnPic.Init(&Ini, "BtnPic");
			m_pSelf->m_BtnKeyboard.Init(&Ini,"BtnKeyboard");
			m_pSelf->m_BtnWuxing.Init(&Ini,"BtnWuxing");
			m_pSelf->m_IndexList.Init(&Ini, "IndexList");
			m_pSelf->m_IndexScroll.Init(&Ini, "IndexScroll");
			m_pSelf->m_MessageScroll.Init(&Ini, "MessageScroll");
			m_pSelf->m_MessageList.Init(&Ini, "MessageList");
			m_pSelf->LoadIndexList(pScheme);
		}
	}
}

//窗口函数
int KUiHelper2::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nRet = 0;
	switch(uMsg)
	{
	// A button is clicked.
	case WND_N_BUTTON_CLICK:
		if (uParam == (unsigned int)(KWndWindow*)&m_BtnClose)
			CloseWindow(false);
		else if (uParam == (unsigned int)(KWndWindow*)&m_BtnPic)
		{
			CloseWindow(false);
			KUiHelper::SelectInterfacePic();
			KUiHelper::OpenWindow(true);
		}
		else if (uParam == (unsigned int)(KWndWindow*)&m_BtnKeyboard)
		{
			CloseWindow(false);
			KUiHelper::SelectKeyboardPic();
			KUiHelper::OpenWindow(true);
		}
		else if(uParam == (unsigned int)(KWndWindow*)&m_BtnWuxing)
		{
			CloseWindow(false);
			KUiHelper::SelectWuxingPic();
			KUiHelper::OpenWindow(true);
		}
		break;
	// scrollbar's position changed.
	case WND_N_SCORLLBAR_POS_CHANGED:
		if (uParam == (unsigned int)(KWndWindow*)&m_IndexScroll)
			m_IndexList.SetTopItemIndex(nParam);
		else if (uParam == (unsigned int)(KWndWindow*)&m_MessageScroll)
			m_MessageList.SetFirstShowLine(nParam);
		break;
	// list's click
	case WND_N_LIST_ITEM_SEL:
		if (uParam == (unsigned int)(KWndWindow*)&m_IndexList)
		{
			int nIndex = nParam;
			if (nIndex < 0)
				nIndex = 0;
			// Get the content of "nParam" index.
			UpdateData(nIndex);
		}
		break;
	case WM_KEYDOWN:
		{
			int nCurIdx = 0;
			int nTotal = 0;
//			if (nParam == (unsigned int)(KWndWindow*)&m_IndexList)
//			{
				nCurIdx = m_IndexList.GetCurSel();
				nTotal = m_IndexList.GetCount();
				switch (uParam)
				{
				case VK_UP:
					if (nCurIdx == 0)
						nCurIdx = nTotal;
					nCurIdx = nCurIdx - 1;
					m_IndexList.SetCurSel(nCurIdx);
					m_IndexScroll.SetScrollPos(m_IndexList.GetTopItemIndex());
					break;
				case VK_DOWN:
					if (nCurIdx == nTotal - 1)
						nCurIdx = -1;
					nCurIdx = nCurIdx + 1;
					m_IndexList.SetCurSel(nCurIdx);
					m_IndexScroll.SetScrollPos(m_IndexList.GetTopItemIndex());
					break;
				default:
					break;
				}
//			}
//			else if (nParam == (unsigned int)(KWndWindow*)&m_MessageList)
//			{
//				nCurIdx = m_MessageList.GetCurSel();
//				nTotal = m_IndexList.GetCount();
//				switch (uParam)
//				{
//				case VK_UP:
//					if (nCurIdx == 0)
//						nCurIdx = nTotal;
//					nCurIdx = nCurIdx - 1;
//					m_MessageList.SetCurSel(nCurIdx);
//					m_MessageScroll.SetScrollPos(m_MessageList.GetFirstShowLine());
//					break;
//				case VK_DOWN:
//					if (nCurIdx == nTotal - 1)
//						nCurIdx = -1;
//					nCurIdx = nCurIdx + 1;
//					m_MessageList.SetCurSel(nCurIdx);
//					m_MessageScroll.SetScrollPos(m_MessageList.GetFirstShowLine());
//					break;
//				case VK_ESCAPE:
//					CloseWindow(false);
//					break;
//				default:
//					break;
//				}
//			}
		}
		break;
	default:
		nRet = KWndShowAnimate::WndProc(uMsg, uParam, nParam);
	}
	return nRet;
}


void KUiHelper2::Clear()
{
	m_IndexList.SetContent(0, 0, 0, 0);
	m_nIndexCount = 0;
}

void KUiHelper2::LoadIndexList(const char* pScheme)
{
	KIniFile	ListIni;
	int nCount;
	char szInifile[128] = "";

	sprintf(szInifile, "%s\\%s", pScheme, DETAIL_HELP_LIST);
	if (!ListIni.Load(szInifile))
		return;
	ListIni.GetInteger("List", "Count", 0, &nCount);
	// haha, just for fun.
	// be caught by freeway.. :( use "i" instead of "__"
	for (int i = 0; i < nCount; i++)
	{
		char szName[32] = "";
		char szIni[32] = "";
		char szIniSection[32] = "";

		sprintf(szName, "%d_Name", i);
		sprintf(szIni, "%d_Ini", i);
		sprintf(szIniSection, "%d_IniSection", i);
		ListIni.GetString("List", szName, "", m_IndexData[i].szName, 128);
		ListIni.GetString("List", szIni, "", m_IndexData[i].szIni, 128);
		ListIni.GetString("List", szIniSection, "", m_IndexData[i].szIniSection, 128);

		m_IndexList.SetContent((unsigned char *)&m_IndexData[0].szName,
			nCount, sizeof(m_IndexData[i]),
			(char *)&m_IndexData[i].szName - (char *)&m_IndexData[i].szName);
		m_nIndexCount = i + 1;
	}
}

void KUiHelper2::UpdateData(int nIndex)
{
	if (strlen(m_IndexData[nIndex].szIni) == 0)
	{
		KUiHelper::OpenWindow(true);
		return;
	}
	KIniFile Ini;
	char Scheme[256];
	char szIniFile[128];
	g_UiBase.GetCurSchemePath(Scheme, 256);
	sprintf(szIniFile, "%s\\%s", Scheme, m_IndexData[nIndex].szIni);

	if (Ini.Load(szIniFile))
	{
		m_MessageList.SetFirstShowLine(0);
        m_MessageScroll.SetScrollPos(0);
		MsgListBox_LoadContent(&m_MessageList, &Ini,
			m_IndexData[nIndex].szIniSection);
	}
}