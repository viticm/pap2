// -------------------------------------------------------------------------
//	文件名		：	WndToolBar.cpp
//	创建者		：	万里
//	创建时间	：	2003-7-23 21:56:43
//	功能描述	：
//
// -------------------------------------------------------------------------

#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "../Elem/WndMessage.h"
#include "WndToolBar.h"
#include "Wnds.h"
#include "ComWindow.h"
#include "../../Public/iRepresentShell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndToolBar::KWndToolBar()
{
	m_nButtonNum = 0;
	m_pButtons = NULL;
}

KWndToolBar::~KWndToolBar()
{
	FreeAllWindow();
}

void KWndToolBar::Clone(KWndToolBar* pCopy)
{
	if (pCopy)
	{
		KWndImage::Clone(pCopy);
		if (m_nButtonNum && m_pButtons)
		{
		}
		else
		{
			pCopy->FreeAllWindow();
		}
	}
}

//--------------------------------------------------------------------------
//	功能：初始化窗口
//--------------------------------------------------------------------------
int KWndToolBar::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndImage::Init(pIniFile, pSection))
	{
		int n = 0;
		char szName[32];
		char szTitle[32];
		while(1)
		{
			sprintf(szName, "Button%d", n);
			if (pIniFile->GetString(pSection, szName, "", szTitle, 32))
			{
				AddOneWindow(pIniFile, szTitle);
			}
			else
				break;
			n++;
		}
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KWndToolBar::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	if (!KWndImage::WndProc(uMsg, uParam, nParam))
	{
		if (m_pParentWnd)
			return m_pParentWnd->WndProc(uMsg, uParam, nParam);
	}

	return 0;
}

//窗体绘制
void KWndToolBar::PaintWindow()
{
	KWndImage::PaintWindow();
}

void KWndToolBar::AddOneWindow(KIniFile* pIniFile, const char* pSection)
{
	m_pButtons = (KButtonInfo*)realloc(m_pButtons, sizeof(KButtonInfo) * (m_nButtonNum + 1));
	if (m_pButtons && m_nButtonNum >= 0)
	{
		char szType[32];
		if (pIniFile->GetString(pSection, "ClassType", "", szType, 32))
		{
			strncpy(m_pButtons[m_nButtonNum].szTitle, pSection, 32);
			m_pButtons[m_nButtonNum].m_pButton = gGetComClassFactory()->CreateComObject(szType);
			if (m_pButtons[m_nButtonNum].m_pButton)
			{
				m_pButtons[m_nButtonNum].m_pButton->Init(pIniFile, pSection);
				AddChild(m_pButtons[m_nButtonNum].m_pButton);
				m_nButtonNum++;
			}
		}
	}
}

void KWndToolBar::FreeAllWindow()
{
	if (m_pButtons && m_nButtonNum)
	{
		for (int n = 0; n < m_nButtonNum; n++)
		{
			if (m_pButtons[n].m_pButton)
			{
				delete m_pButtons[n].m_pButton;
			}
		}
		free(m_pButtons);
		m_pButtons = NULL;
		m_nButtonNum = 0;
	}
}

void KWndToolBar::UpdateData()
{
	for (int n = 0; n < m_nButtonNum; n++)
	{
		if (m_pButtons[n].m_pButton)
		{
			m_pButtons[n].m_pButton->UpdateData();
		}
	}
}