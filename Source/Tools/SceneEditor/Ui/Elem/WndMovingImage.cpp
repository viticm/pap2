/*****************************************************************************************
//	界面窗口体系结构--带移动控制的显示图形的窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-10
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "WndMovingImage.h"

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
KWndMovingImage::KWndMovingImage()
{
	m_nCurrentValue = 0;
	m_nFullValue = 1;
	m_MoveRange.cx = 0;
	m_MoveRange.cy = 0;
}

void KWndMovingImage::Clone(KWndMovingImage* pCopy)
{
	if (pCopy)
	{
		KWndImage::Clone(pCopy);
		pCopy->m_oFixPos	= m_oFixPos;
		pCopy->m_MoveRange	= m_MoveRange;
		SetMoveValue(m_nCurrentValue, m_nFullValue);
	}
}

//--------------------------------------------------------------------------
//	功能：初始化窗口
//--------------------------------------------------------------------------
int KWndMovingImage::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndImage::Init(pIniFile, pSection))
	{
		m_MoveRange.cx = m_oFixPos.x = m_Left;
		m_MoveRange.cy = m_oFixPos.y = m_Top;
		pIniFile->GetInteger2(pSection, "EndPos", (int*)&m_MoveRange.cx, (int*)&m_MoveRange.cy);
		m_MoveRange.cx -= m_oFixPos.x;
		m_MoveRange.cy -= m_oFixPos.y;
		SetMoveValue(m_nCurrentValue, m_nFullValue);
		return true;
	}
	return false;
}

void KWndMovingImage::SetMoveValue(int nCurrentValue, int nFullValue)
{
	m_nFullValue = nFullValue ? nFullValue : 1;
	m_nCurrentValue = nCurrentValue;
	int	x, y;
	if (m_MoveRange.cx)
		 x = (m_MoveRange.cx * m_nCurrentValue) / m_nFullValue + m_oFixPos.x;
	else
		x = m_oFixPos.x;
	if (m_MoveRange.cy)
		 y = (m_MoveRange.cy * m_nCurrentValue) / m_nFullValue + m_oFixPos.y;
	else
		y = m_oFixPos.y;
	SetPosition(x, y);
}