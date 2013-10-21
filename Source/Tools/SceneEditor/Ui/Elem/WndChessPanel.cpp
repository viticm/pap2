/*
 *  File:      WndChessPanel.cpp
 *  Dest:      ÆåÅÌÃæ°å
 *  Creator:   flying
 *  CopyRight: KingSoft
 *  Date:      2003-8-22
 */
#pragma warning(disable:4243)

#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "../elem/wnds.h"
#include "../Elem/WndMessage.h"
#include "../../../core/src/CoreObjGenreDef.h"
#include "../../../core/src/coreshell.h"
#include "../../TextCtrlCmd/TextCtrlCmd.h"
#include "../../../Represent/iRepresent/iRepresentShell.h"
#include "../../../Engine/src/Text.h"
// flying add these include files.
#include "../../../Represent/iRepresent/KRepresentUnit.h"
#include "../Elem/MouseHover.h"
#include "../Elem/WndWindow.h"
#include "WndChessPanel.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;
extern KMouseOver	g_MouseOver;

KWndChessPanel* KWndChessPanel::m_pSelf = NULL;

// Constuctor
KWndChessPanel::KWndChessPanel()
{
}

KWndChessPanel::~KWndChessPanel()
{
}

KWndChessPanel* KWndChessPanel::OpenWindow(bool bShow)
{
	if (m_pSelf == NULL)
	{
		m_pSelf = new KWndChessPanel;
		if (m_pSelf)
			m_pSelf->Initialize();
	}

	if (m_pSelf)
	{
		if (bShow)
		{
			Wnd_SetCapture(m_pSelf);
			m_pSelf->Show();
			m_pSelf->BringToTop();
		}
		else
			m_pSelf->Hide();
	}
	return m_pSelf;
}

void KWndChessPanel::CloseWindow(bool bDestroy)
{
	if (m_pSelf)
	{
		Wnd_ReleaseCapture();
		m_pSelf->Hide();
		if (bDestroy)
		{
			m_pSelf->Destroy();
			m_pSelf = NULL;
		}
	}
}

void KWndChessPanel::Clear()
{
}

KWndChessPanel* KWndChessPanel::GetIfVisible()
{
	if (m_pSelf && m_pSelf->IsVisible())
		return m_pSelf;
	else
		return NULL;
}

// Draw the grid out
void KWndChessPanel::PaintWindow()
{
	// draw the grid.
	int nRow = 0;
	int nCol = 0;
	int nNum = m_nCount % m_nRowCount;

	nCol = m_nCount > m_nRowCount ? m_nRowCount : m_nCount;
	nRow = nNum ? m_nCount / m_nRowCount : m_nCount / m_nRowCount - 1;

	for (int i = 0; i <= nRow; i++)
	{
		KRULine line;
		line.Color.Color_dw = m_nBorderColor;
		line.oPosition.nX = m_Left;
		line.oPosition.nY = m_Top + (i) * m_nBtnHeight;
		line.oEndPos.nX = m_Left + m_Width;
		line.oEndPos.nY = m_Top + (i) * m_nBtnHeight;
		line.oEndPos.nZ = 1;
		g_pRepresentShell->DrawPrimitives(1, &line, RU_T_LINE, true);
	}
	for (int j = 0; j <= nCol; j++)
	{
		KRULine line;
		line.Color.Color_dw = m_nBorderColor;
		line.oPosition.nX = m_Left + (j) * m_nBtnWidth;
		line.oPosition.nY = m_Top;
		line.oEndPos.nX = line.oPosition.nX;
		if (nNum > 0)
		{
			line.oEndPos.nY = j < nNum ?
				m_Top + (nRow + 1) * m_nBtnHeight :
				m_Top + (nRow) * m_nBtnHeight;
		}
		else
		{
			line.oEndPos.nY = m_Top + (nRow + 1) * m_nBtnHeight;
		}
		line.oEndPos.nZ = 1;
		g_pRepresentShell->DrawPrimitives(1, &line, RU_T_LINE, true);
	}

}

int KWndChessPanel::PtInWindow(int x, int y)
{
	int nResult = false;
	nResult = true;
	return nResult;
}

int KWndChessPanel::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nResult = false;
	switch (uMsg)
	{
	// No warning..
	case 0:
		break;
	default:
		break;
	}
	nResult = true;
	return nResult;
}

void KWndChessPanel::Initialize()
{
}

int	KWndChessPanel::Init(KIniFile* pIniFile, const char* pSection)
{
	int nResult = false;
	nResult = true;
	return nResult;
}