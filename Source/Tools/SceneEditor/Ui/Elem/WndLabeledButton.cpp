/*****************************************************************************************
//	界面窗口体系结构--带文字按键窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-10-4
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "../Elem/WndMessage.h"
#include "WndLabeledButton.h"
#include "Wnds.h"
#include "../../Public//Text.h"
#include "../../Public/iRepresentShell.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iRepresentShell*	g_pRepresentShell;

//===========================================
//	简单的带文字按钮窗口。
//	文字单行，居中处理，文字中不包含控制符。
//===========================================

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndLabeledButton::KWndLabeledButton()
{
	m_Label[0]  = 0;
	m_Label[31] = 0;
	m_nLabelLen = 0;
	m_nFontSize = 16;
	m_nLabelYOffset = 0;
}

void KWndLabeledButton::Clone(KWndLabeledButton* pCopy)
{
	if (pCopy)
	{
		KWndButton::Clone(pCopy);
		pCopy->m_nFontSize = m_nFontSize;
		pCopy->m_FontColor = m_FontColor;
		pCopy->m_OverColor = m_OverColor;
		pCopy->m_nLabelYOffset = m_nLabelYOffset;
		pCopy->m_FontBorderColor = m_FontBorderColor;
		pCopy->m_OverBorderColor = m_OverBorderColor;
		strcpy(pCopy->m_Label, m_Label);
	}
}

//--------------------------------------------------------------------------
//	功能：初始化窗口
//--------------------------------------------------------------------------
int KWndLabeledButton::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndButton::Init(pIniFile, pSection))
	{
		pIniFile->GetInteger(pSection, "Font", 16, &m_nFontSize);
		pIniFile->GetInteger(pSection, "LabelYOffset", 0, &m_nLabelYOffset);
		if (m_nFontSize < 12)
			m_nFontSize = 16;
		//===读取文字====
		char	Buff[32];
		pIniFile->GetString(pSection, "Color", "", Buff, sizeof(Buff));
		m_FontColor = GetColor(Buff);
		pIniFile->GetString(pSection, "BorderColor", "", Buff, sizeof(Buff));
		m_FontBorderColor = GetColor(Buff);
		pIniFile->GetString(pSection, "OverColor", "", Buff, sizeof(Buff));
		m_OverColor = GetColor(Buff);
		pIniFile->GetString(pSection, "OverBorderColor", "", Buff, sizeof(Buff));
		m_OverBorderColor = GetColor(Buff);

		pIniFile->GetString(pSection, "Label", "", Buff, sizeof(Buff));
		SetLabel(Buff);
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：设置按钮标题文字
//--------------------------------------------------------------------------
void KWndLabeledButton::SetLabel(const char* pLabel)
{
	if (pLabel)
	{
		m_nLabelLen = strlen(pLabel);
		if (m_nLabelLen > 31)
			m_nLabelLen = 31;
		memcpy(m_Label, pLabel, m_nLabelLen);
		m_Label[m_nLabelLen] = 0;
	}
}

int KWndLabeledButton::GetLabel(char* pLabel, int nSize)
{
	int nRet = 0;
	if (pLabel)
	{
		if (m_nLabelLen < nSize)
		{
			memcpy(pLabel, m_Label, m_nLabelLen + 1);
			nRet = m_nLabelLen;
		}
		else
		{
			pLabel[0] = 0;
			nRet = 0;
		}
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：设置按钮标题文字颜色
//--------------------------------------------------------------------------
void KWndLabeledButton::SetLabelColor(unsigned int Color)
{
	m_FontColor = Color;
}

//--------------------------------------------------------------------------
//	功能：窗体绘制
//--------------------------------------------------------------------------
void KWndLabeledButton::PaintWindow()
{
	KWndButton::PaintWindow();
	if (g_pRepresentShell)
	{
		char	Buffer[32];
		int nMaxLen = m_Width * 2 / m_nFontSize;
		const char* pShowString = TGetLimitLenString(m_Label, -1, Buffer, nMaxLen);
		if (pShowString)
		{
			int nLen = strlen(pShowString);
			unsigned int uColor = m_FontColor;
			unsigned int uBorderColor = m_FontBorderColor;
			if (m_Flag & WNDBTN_F_OVER)
			{
				uColor = m_OverColor;
				uBorderColor = m_OverBorderColor;
			}
			g_pRepresentShell->OutputText(m_nFontSize, pShowString, nLen,
				m_nAbsoluteLeft + (m_Width - nLen * m_nFontSize / 2) / 2,
				m_nAbsoluteTop + m_nLabelYOffset,
				uColor , 0,
				TEXT_IN_SINGLE_PLANE_COORD, uBorderColor);
		}
	}
}

//=====================================================
//	复杂的带文字按钮窗口。
//	文字可以为单行，也可以为多行，单行时候可以指定
//	水平以及垂直的对齐方式。文字中可以包含控制符。
//=====================================================

//--------------------------------------------------------------------------
//	功能：初始化窗口
//--------------------------------------------------------------------------
int KWndRichLabeledButton::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndButton::Init(pIniFile, pSection))
	{
		m_Label.Init(pIniFile, pSection);
		m_Label.SetPosition(m_nAbsoluteLeft, m_nAbsoluteTop);
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：设置按钮标题文字
//--------------------------------------------------------------------------
void KWndRichLabeledButton::SetLabel(const char* pLabel, int nLen)
{
	m_Label.SetText(pLabel, nLen);
}

//--------------------------------------------------------------------------
//	功能：设置按钮标题文字颜色
//--------------------------------------------------------------------------
void KWndRichLabeledButton::SetLabelColor(unsigned int Color)
{
	m_Label.SetTextColor(Color);
}

//--------------------------------------------------------------------------
//	功能：窗体绘制
//--------------------------------------------------------------------------
void KWndRichLabeledButton::PaintWindow()
{
	KWndButton::PaintWindow();
	m_Label.SetPosition(m_nAbsoluteLeft, m_nAbsoluteTop);
	m_Label.PaintWindow();
}
