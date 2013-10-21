/*****************************************************************************************
//	界面窗口体系结构--切换显示状态时带移动控制的图形的窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-12-17
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "WndShowAnimate.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

int	KWndShowAnimate::ms_nMoveSpeed = 10;
int	KWndShowAnimate::ms_nFullRange = 100;
unsigned int KWndShowAnimate::ms_uMoveTimeInterval = 50;

KWndShowAnimate::KWndShowAnimate()
{
	m_nCurrentSpeed = 0;
	m_uMoveLastTime = 0;
}

void KWndShowAnimate::Clone(KWndShowAnimate* pCopy)
{
	if (pCopy)
	{
		KWndMovingImage::Clone(pCopy);
		pCopy->m_AppearRange = m_AppearRange;
		pCopy->m_DisappearRange = m_DisappearRange;
	}
}

//初始化
int KWndShowAnimate::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndMovingImage::Init(pIniFile, pSection))
	{
		m_DisappearRange = m_MoveRange;
		m_AppearRange.cx = m_oFixPos.x;
		m_AppearRange.cy = m_oFixPos.y;
		pIniFile->GetInteger2(pSection, "StartPos", (int*)&m_AppearRange.cx, (int*)&m_AppearRange.cy);
		m_AppearRange.cx -= m_oFixPos.x;
		m_AppearRange.cy -= m_oFixPos.y;

		m_Style &= ~WND_S_VISIBLE;

		return true;
	}
	return false;
}

void KWndShowAnimate::Show()
{
	if ((m_Style & WND_S_VISIBLE) == 0 ||
		m_nCurrentSpeed > 0)
	{
		KWndMovingImage::Show();
		if (m_AppearRange.cx || m_AppearRange.cy)
		{
			m_nCurrentSpeed = -ms_nMoveSpeed;
			m_MoveRange = m_AppearRange;
			SetMoveValue(100, 100);
		}
		else
        {
			SetPosition(m_oFixPos.x, m_oFixPos.y);
            ShowCompleted();
        }
	}
    else
    {
        ShowCompleted();
    }
	m_uMoveLastTime = IR_GetCurrentTime();
}

void KWndShowAnimate::Hide()
{
	if ((m_Style & WND_S_VISIBLE) &&
		m_nCurrentSpeed <= 0)
	{
		if (m_DisappearRange.cx || m_DisappearRange.cy)
		{
			m_nCurrentSpeed = ms_nMoveSpeed;
			m_MoveRange = m_DisappearRange;
			SetMoveValue(0, 100);
		}
		else
		{
			KWndMovingImage::Hide();
			SetPosition(m_oFixPos.x, m_oFixPos.y);
		}
	}
	m_uMoveLastTime = IR_GetCurrentTime();
}

void KWndShowAnimate::SetMoveSpeed(int nMoveSpeed, int nFullRange)
{
	ms_nFullRange = (nFullRange >= 1) ? nFullRange : 1;
	if (nMoveSpeed <= ms_nFullRange || nMoveSpeed >= 1)
		ms_nMoveSpeed = nMoveSpeed;
	else
		ms_nMoveSpeed = 1;
}

void KWndShowAnimate::SetInterval(unsigned int uInterval)
{
	if (uInterval >= 10)
		ms_uMoveTimeInterval = uInterval;
	else
		ms_uMoveTimeInterval = 10;
}

void KWndShowAnimate::ShowCompleted()
{

}


//窗体绘制
void KWndShowAnimate::PaintWindow()
{
	if (m_nCurrentSpeed)
	{
		while(IR_IsTimePassed(ms_uMoveTimeInterval, m_uMoveLastTime))
		{
			int nValue = m_nCurrentValue + m_nCurrentSpeed;
			if (m_nCurrentSpeed > 0 && nValue >= ms_nFullRange)
			{
				nValue = ms_nFullRange;
				m_nCurrentSpeed = 0;
				KWndMovingImage::Hide();
				SetMoveValue(nValue, ms_nFullRange);
                ShowCompleted();
				break;
			}
			else if (m_nCurrentSpeed < 0 && nValue <= 0)
			{
				m_nCurrentSpeed = 0;
				nValue = 0;
				SetMoveValue(nValue, ms_nFullRange);
                ShowCompleted();
				break;
			}
			else
				SetMoveValue(nValue, ms_nFullRange);
		}
	}
	KWndMovingImage::PaintWindow();
}