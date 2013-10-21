/*****************************************************************************************
//	界面窗口体系结构--切换显示状态时带移动控制的图形的窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-17
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "WndShadow.h"
#include "../../Public/iRepresentShell.h"
#include "../../Public/KRepresentUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;

void KWndShadow::Clone(KWndShadow* pCopy)
{
	if (pCopy)
	{
		KWndWindow::Clone(pCopy);
		pCopy->m_uColor = m_uColor;
	}
}

//初始化
int KWndShadow::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndWindow::Init(pIniFile, pSection))
	{
		char	Buff[16];
		pIniFile->GetString(pSection, "ShadowColor", "", Buff, sizeof(Buff));
		m_uColor = (GetColor(Buff) & 0xffffff);
		int nAlpha;
		pIniFile->GetInteger(pSection, "ShadowColorAlpha", 128, &nAlpha);
		if (nAlpha > 255 || nAlpha < 0)
			m_uColor |= 0x16000000;
		else
			m_uColor |= (((255 - nAlpha) << 21) & 0xff000000);
		return true;
	}
	return false;
}

//窗体绘制
void KWndShadow::PaintWindow()
{
	KWndWindow::PaintWindow();
	if (m_uColor && g_pRepresentShell)
	{
		KRUShadow	Shadow;
		Shadow.oPosition.nX = m_nAbsoluteLeft;
		Shadow.oPosition.nY = m_nAbsoluteTop;
		Shadow.oEndPos.nX = m_nAbsoluteLeft + m_Width;
		Shadow.oEndPos.nY = m_nAbsoluteTop + m_Height;
		Shadow.Color.Color_dw = m_uColor;
		g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
	}
}

//判断一个点是否在窗口范围内,传入的是绝对坐标
int KWndShadow::PtInWindow(int x, int y)
{
	if (m_bExactWnd == FALSE)
		return 0;
	return KWndWindow::PtInWindow(x, y);
}

void KWndShadow::SetColor(unsigned int uColor)
{
	m_uColor = uColor;
}