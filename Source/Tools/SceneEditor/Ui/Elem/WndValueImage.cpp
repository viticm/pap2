/*****************************************************************************************
//	界面窗口体系结构--值控制换帧的显示图形的窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-10
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "WndValueImage.h"

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
KWndValueImage::KWndValueImage()
{
	m_nCurrentValue = 0;
	m_nFullValue = 1;
}

void KWndValueImage::Clone(KWndValueImage* pCopy)
{
	if (pCopy)
	{
		KWndImage::Clone(pCopy);
		pCopy->m_nFrameCount = m_nFrameCount;
		pCopy->m_nCurrentValue = m_nCurrentValue;
		pCopy->m_nFullValue = m_nFullValue;
	}
}

//--------------------------------------------------------------------------
//	功能：初始化窗口
//--------------------------------------------------------------------------
int KWndValueImage::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndImage::Init(pIniFile, pSection))
	{
		KImageParam	Param;
		if(g_pRepresentShell &&
			g_pRepresentShell->GetImageParam(m_Image.szImage, &Param, m_Image.nType))
		{
			m_Image.nNumFrames = Param.nNumFrames;
			m_Image.nInterval = Param.nInterval;
			SetValue(m_nCurrentValue, m_nFullValue);
		}
		return true;
	}
	return false;
}

void KWndValueImage::SetValue(int nCurrentValue, int nFullValue)
{
	m_nFullValue = nFullValue ? nFullValue : 1;
	m_nCurrentValue = nCurrentValue;
	if (m_Image.nNumFrames > 1)
	{
		m_Image.nFrame = nCurrentValue * (m_Image.nNumFrames - 1)/ m_nFullValue;
	}
	else
		m_Image.nFrame = 0;
}