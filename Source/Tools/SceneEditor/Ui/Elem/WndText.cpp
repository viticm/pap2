/*****************************************************************************************
//	界面窗口体系结构--文本窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-23
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "../Elem/WndMessage.h"
#include "WndText.h"
#include "../../Public/Text.h"
#include <string>


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
KWndText::KWndText()
{
	m_nFontSize = 12;
	m_pText = NULL;
	m_nTextLen   = 0;
	m_nBuffLen   = 0;
	m_nTopLine   = 0;
	m_nLineCount = 0;
	m_BorderColor = 0;
}

//--------------------------------------------------------------------------
//	功能：设置文本文字
//--------------------------------------------------------------------------
void KWndText::SetText(const char* pText, int nLen/*= -1*/)
{
	if (m_pText && g_pRepresentShell)
	{
		if (pText && pText[0])
		{
			if (nLen < 0)
				nLen = strlen(pText);
			if (nLen > m_nBuffLen)
				nLen = TSplitString(pText, m_nBuffLen - 1, true);
			memcpy(m_pText, pText, nLen);
			m_pText[nLen] = 0;
			m_nTextLen = nLen;
			if (m_Style & WNDTEXT_ES_MULTILINE)
			{
				int nLineLen;
				m_nLineCount = TGetEncodedTextLineCount(
					m_pText, m_nTextLen, (m_Width * 2) / m_nFontSize, nLineLen, m_nFontSize);
			}
		}
		else
		{
			m_pText[0] = 0;
			m_nTextLen = 0;
			m_nLineCount = 0;
		}
		m_nTopLine = 0;
	}
}


//--------------------------------------------------------------------------
//	功能：设置文字大小
//--------------------------------------------------------------------------
void KWndText::SetFontSize(int nSize)
{
	m_nFontSize = nSize;
	if (m_Style & WNDTEXT_ES_MULTILINE)
	{
		int nLineLen;
		m_nLineCount = TGetEncodedTextLineCount(
			m_pText, m_nTextLen, (m_Width * 2) / m_nFontSize, nLineLen, m_nFontSize);
		if(m_nTopLine > m_nLineCount)
		{
			m_nTopLine = m_nLineCount;
		}
	}
}


//--------------------------------------------------------------------------
//	功能：获取字符串内容
//--------------------------------------------------------------------------
int KWndText::GetText(char* pBuffer, int nSize)
{
	int nRet = 0;
	if (m_pText && pBuffer)
	{
		if (m_nTextLen + 1 <= nSize)
		{
			memcpy(pBuffer, m_pText, m_nTextLen + 1);
			nRet = m_nTextLen;
		}
		else
			nRet = m_nTextLen + 1;
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：获取字体大小
//--------------------------------------------------------------------------
int KWndText::GetFontSize()
{
	return m_nFontSize;
}

//--------------------------------------------------------------------------
//	功能：set the index of the first visible line int the text control wnd
//--------------------------------------------------------------------------
int	KWndText::SetTopLine(int nTopLine)
{
	if (nTopLine >= 0 && nTopLine < m_nLineCount)
		m_nTopLine = nTopLine;
	return 0;
}

//--------------------------------------------------------------------------
//	功能：设置文本缓冲区指针
//--------------------------------------------------------------------------
void KWndText::SetTextPtr(char* pText, int nMaxLen)
{
	m_pText = pText;
	m_nBuffLen = nMaxLen;
	m_nTextLen = 0;
	m_nTopLine = 0;
	m_nLineCount = 0;
}

void KWndText::Clone(KWndText* pCopy)
{
	if (pCopy)
	{
		KWndWindow::Clone(pCopy);
		pCopy->m_nFontSize = m_nFontSize;
		pCopy->m_TextColor = m_TextColor;
		pCopy->m_BorderColor = m_BorderColor;
	}
}

//--------------------------------------------------------------------------
//	功能：初始化
//--------------------------------------------------------------------------
int KWndText::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndWindow::Init(pIniFile, pSection))
	{
		pIniFile->GetInteger(pSection, "Font", 12, &m_nFontSize);
		//====读取文字对齐方式====
		m_Style &= ~WNDTEXT_ES_ALIGN_FILTER;
		int	nValue;
		pIniFile->GetInteger(pSection, "HAlign", 0, &nValue);
		if (nValue == 1)
			m_Style |= WNDTEXT_ES_HALIGN_CENTRE;
		else if (nValue == 2)
			m_Style |= WNDTEXT_ES_HALIGN_RIGHT;
		pIniFile->GetInteger(pSection, "VAlign", 0, &nValue);
		if (nValue == 1)
			m_Style |= WNDTEXT_ES_VALIGN_CENTRE;
		else if (nValue == 2)
			m_Style |= WNDTEXT_ES_VALIGN_BOTTOM;

		//===读取文字====
		char	Buff[256];
		pIniFile->GetString(pSection, "Color", "0,0,0", Buff, 16);
		m_TextColor = GetColor(Buff);
		pIniFile->GetString(pSection, "BorderColor", "0,0,0", Buff, 16);
		m_BorderColor = GetColor(Buff);
		if (m_pText && m_pText[0] == 0)
		{
			if (pIniFile->GetString(pSection, "Text", "", Buff, sizeof(Buff)))
			{
				strncpy(m_pText, Buff, m_nBuffLen - 1);
				m_pText[m_nBuffLen - 1] = 0;
				m_nTextLen = strlen(m_pText);
			}
		}
		pIniFile->GetInteger(pSection, "MultiLine", 0, &nValue);
		if (nValue)
		{
			m_Style |= WNDTEXT_ES_MULTILINE;
			if (g_pRepresentShell)
			{
				int nLineLen;
				m_nLineCount = TGetEncodedTextLineCount(
					m_pText, m_nTextLen, (m_Width * 2) / m_nFontSize, nLineLen, m_nFontSize);
			}
			else
				m_nLineCount = 0;
			if (m_nTopLine >= m_nLineCount)
				m_nTopLine = 0;
		}
		else
		{
			m_Style &= ~ WNDTEXT_ES_MULTILINE;
			m_nLineCount = 0;
			m_nTopLine = 0;
		}

		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：绘制窗口
//--------------------------------------------------------------------------
void KWndText::PaintWindow()
{
	KWndWindow::PaintWindow();
	if (m_pText == NULL || m_pText[0] == 0 || g_pRepresentShell == NULL)
		return;
	KOutputTextParam	Param;
	Param.nX = m_nAbsoluteLeft;
	Param.nY = m_nAbsoluteTop;
	Param.nZ = TEXT_IN_SINGLE_PLANE_COORD;

	if ((m_Style & WNDTEXT_ES_MULTILINE) == 0)
	{
		switch(m_Style & WNDTEXT_ES_HALIGN_FILTER)
		{
		case WNDTEXT_ES_HALIGN_CENTRE:// 居中
			Param.nX += (m_Width - m_nTextLen * m_nFontSize / 2) / 2;
			if (Param.nX < m_nAbsoluteLeft)
				Param.nX = m_nAbsoluteLeft;
			break;
		case WNDTEXT_ES_HALIGN_RIGHT:// 居右
			Param.nX += m_Width - m_nTextLen * m_nFontSize / 2;
			if (Param.nX < m_nAbsoluteLeft)
				Param.nX = m_nAbsoluteLeft;
			break;
		}
		switch(m_Style & WNDTEXT_ES_VALIGN_FILTER)
		{
		case WNDTEXT_ES_VALIGN_CENTRE:
			Param.nY += (m_Height - m_nFontSize - 1) / 2;
			break;
		case WNDTEXT_ES_VALIGN_BOTTOM:
			Param.nY += m_Height - m_nFontSize - 1;
			break;
		}
		Param.nNumLine = 1;
	}
	else	//多行
		Param.nNumLine = m_Height / (m_nFontSize + 1);

	Param.nSkipLine = m_nTopLine;
	Param.Color = m_TextColor;
	Param.BorderColor = m_BorderColor;
	g_pRepresentShell->OutputRichText(m_nFontSize, &Param, m_pText, m_nTextLen, m_Width);
}

//--------------------------------------------------------------------------
//	功能：设置文本文字颜色
//--------------------------------------------------------------------------
void KWndText::SetTextColor(unsigned int Color)
{
	m_TextColor = Color;
}

//--------------------------------------------------------------------------
//	功能：设置数值文字
//--------------------------------------------------------------------------
void KWndText::SetIntText(int nNumber, char Separator)
{
	char	Buff[16];
	int		nLen;
	if (m_pText)
	{
		if (Separator)
			nLen = sprintf(Buff, "%d%c", nNumber, Separator);
		else
			nLen = sprintf(Buff, "%d", nNumber);
		SetText(Buff, nLen);
	}
}

//--------------------------------------------------------------------------
//	功能：设置数值文字
//--------------------------------------------------------------------------
void KWndText::Set2IntText(int nNumber1, int nNumber2, char Separator)
{
	char	Buff[32];
	int		nLen;
	if (m_pText)
	{
		nLen = sprintf(Buff, "%d%c%d", nNumber1, Separator, nNumber2);
		SetText(Buff, nLen);
	}
}

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndText32::KWndText32()
{
	m_Text[0] = m_Text[31] = 0;
	SetTextPtr(m_Text, 31);
}

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndText80::KWndText80()
{
	m_Text[0] = m_Text[79] = 0;
	SetTextPtr(m_Text, 79);
}

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndText256::KWndText256()
{
	m_Text[0] = m_Text[255] = 0;
	SetTextPtr(m_Text, 255);
}

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndText512::KWndText512()
{
	m_Text[0] = m_Text[511] = 0;
	SetTextPtr(m_Text, 511);
}


//根据简单格式化字符串处理数字，这个格式比较怪异，先参阅一下默认串
void KWndText::SetFormatText(int nNumber, const char *szFormat)
{
	char szBuff[64];
	FormatText(nNumber, szFormat, szBuff);
	SetText(szBuff);
}


//根据简单格式化字符串处理数字的实施方法
char* KWndText::FormatText(int nNumber, const char* szFormat, char* szBuff)
{
	if(szFormat && szBuff)
	{
		int nNumberPos, nFormatPos;
		char szNumber[32];
		std::basic_string<char> szConverted;

		//初始化转换缓冲
		szConverted.empty();
		szBuff[0] = 0;

		//首先把数字变为10进制字符串
		itoa(nNumber, szNumber, 10);
		nNumberPos = strlen(szNumber) - 1;
		nFormatPos = strlen(szFormat) - 1;

		//从最右边开始处理，如果不是关键字符，就插入转换缓冲
		while(nFormatPos > -1 && nNumberPos > -1)
		{

			if(szFormat[nFormatPos] != 'x' && szFormat[nFormatPos] != 'X')
			{
				szConverted.insert(0, szFormat[nFormatPos]);
				nFormatPos --;
			}
			else //是关键字符，就把szNumber的最后一个数字根据规则拷贝进转换缓冲
			{
				//'x'是不忽略'0'处理
				if(szFormat[nFormatPos] == 'x')
				{
					szConverted.insert(0, szNumber[nNumberPos]);
				}
				//'X'是有限制忽略'0'处理
				else
				{
					//'0'就进行处理
					if(szNumber[nNumberPos] == '0')
					{
						//如果前面一个数字不为'0'，则一定显示，符合读音规则
						if(szNumber[nNumberPos + 1] != '0')
						{
							szConverted.insert(0, szNumber[nNumberPos]);
						}
						else
						{
							//先找出这段的剩余长度
							int nLess = nFormatPos;
							while(nLess > 0)
							{
								if(szFormat[nLess] != 'x' && szFormat[nLess] != 'X')
									break;
								nLess--;
							}
							nLess = nFormatPos - nLess;

							//在根据这段的剩余长度判断数字后面是否都为0
							int nbZero = TRUE, nNumberLess = nNumberPos;
							for(int i = 0;i < nLess;i++)
							{
								if(nNumberLess < 0)
									break;
								if(szNumber[nNumberLess] != '0')
								{
									nbZero = FALSE;
									break;
								}
								nNumberLess --;
							}

							//如果不是都为'0'，那就显示这个'0'
							if(!nbZero)
							{
								szConverted.insert(0, szNumber[nNumberPos]);
							}
						}
					}
					//不为0就直接拷贝
					else
					{
						szConverted.insert(0, szNumber[nNumberPos]);
					}
				}
				//退步处理
				nFormatPos --;
				nNumberPos --;
			}
		//////////////////////////////////////////////////////
		}
		strcpy(szBuff, szConverted.c_str());
	}
	return szBuff;
}
