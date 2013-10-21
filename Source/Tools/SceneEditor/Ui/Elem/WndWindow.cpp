/*****************************************************************************************
//	剑侠引擎，界面窗口体系结构的最基本窗口对象
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-9
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "../Elem/WndMessage.h"
#include "WndWindow.h"
#include "shlwapi.h"
#include "Wnds.h"
#include "MouseHover.h"

//#include "../../Public/iRepresentShell.h"
//#include "../../Public/KRepresentUnit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

//extern iRepresentShell*	g_pRepresentShell;


int WND_SHOW_DEBUG_FRAME_TEXT = false;

#define	MOSE_HOVER_TIME_VALVE		1000

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndWindow::KWndWindow()
{
#ifdef _DEBUG
	m_Caption[0]	= 0;
	m_Caption[31]	= 0;
#endif
	m_Left			= 0;
	m_Top			= 0;
	m_Width			= 0;
	m_Height		= 0;
	m_nAbsoluteLeft = 0;
	m_nAbsoluteTop	= 0;

	m_bMoving = false;
	m_nLastMouseHoldPosX = m_nLastMouseHoldPosY = 0;

	m_pPreviousWnd	= NULL;
	m_pNextWnd		= NULL;
	m_pFirstChild	= NULL;
	m_pParentWnd	= NULL;
	m_Style			= WND_S_VISIBLE;

	for(int i = 0;i < enumWLI_NUM;i++)
	{
		m_nParam[i] = -1;
	}
}

//--------------------------------------------------------------------------
//	功能：分构函数
//--------------------------------------------------------------------------
KWndWindow::~KWndWindow()
{
	LeaveAlone();
	Wnd_OnWindowDelete(this);
	g_MouseOver.OnWndClosed((void*)this);
}

//--------------------------------------------------------------------------
//	功能：把窗口移动到最前面
//--------------------------------------------------------------------------
void KWndWindow::BringToTop()
{
	if (m_pNextWnd)
	{
		KWndWindow*	pLast = m_pNextWnd;
		while(pLast->m_pNextWnd)
			pLast = pLast->m_pNextWnd;

		pLast->m_pNextWnd = this;
		m_pNextWnd->m_pPreviousWnd = m_pPreviousWnd;
		if (m_pPreviousWnd)
			m_pPreviousWnd->m_pNextWnd = m_pNextWnd;
		else if (m_pParentWnd && m_pParentWnd->m_pFirstChild == this)
			m_pParentWnd->m_pFirstChild = m_pNextWnd;
		m_pNextWnd = NULL;
		m_pPreviousWnd = pLast;
	}
}

//--------------------------------------------------------------------------
//	功能：获取窗口位置
//--------------------------------------------------------------------------
void KWndWindow::GetPosition(int* pLeft, int* pTop)
{
	if (pLeft)
		*pLeft = m_Left;
	if (pTop)
		*pTop = m_Top;
}

//--------------------------------------------------------------------------
//	功能：设置窗口位置（左上角相对于父窗口左上角位置的位移）
//--------------------------------------------------------------------------
void KWndWindow::SetPosition(int nLeft, int nTop)
{
	AbsoluteMove(nLeft - m_Left, nTop - m_Top);
	m_Left = nLeft;
	m_Top  = nTop;
}

//--------------------------------------------------------------------------
//	功能：获取窗口位置，绝对坐标
//--------------------------------------------------------------------------
void KWndWindow::GetAbsolutePos(int* pLeft, int* pTop)
{
	if (pLeft)
		*pLeft = m_nAbsoluteLeft;
	if (pTop)
		*pTop = m_nAbsoluteTop;
}

//--------------------------------------------------------------------------
//	功能：获取窗口大小
//--------------------------------------------------------------------------
void KWndWindow::GetSize(int* pWidth, int* pHeight)
{
	if (pWidth)
		*pWidth = m_Width;
	if (pHeight)
		*pHeight = m_Height;
}

//--------------------------------------------------------------------------
//	功能：设置窗口大小
//--------------------------------------------------------------------------
void KWndWindow::SetSize(int nWidth, int nHeight)
{
	if (nWidth < 0)
		nWidth = 0;
	if (nHeight < 0)
		nHeight = 0;
	int nDX = nWidth - m_Width;
	int nDY = nHeight - m_Height;
	if (nDX == 0 && nDY == 0)
		return;
	m_Width = nWidth;
	m_Height = nHeight;

	KWndWindow* pChild = m_pFirstChild;
	while(pChild)
	{
		int	x, y, w, h;
		pChild->GetPosition(&x, &y);
		pChild->GetSize(&w, &h);
		if (nDX)
		{
			if (pChild->m_Style & WND_S_SIZE_WITH_R_EDGE)
				pChild->SetSize(w + nDX, h);
			if (pChild->m_Style & WND_S_MOVE_WITH_R_EDGE)
				pChild->SetPosition(x + nDX, y);
		}
		if (nDY)
		{
			if (pChild->m_Style & WND_S_SIZE_WITH_B_EDGE)
				pChild->SetSize(w, h + nDY);
			if (pChild->m_Style & WND_S_MOVE_WITH_B_EDGE)
				pChild->SetPosition(x, y + nDY);
		}
		pChild = pChild->m_pNextWnd;
	};

	//to be check.!!!!!
	//一些窗口类可能会有根据窗口大小计算出一些变量保存下来供后继运算使用，
	//目前没有提供大小改变的通知消息，可能那些窗口的行为会有未知的结果。
	//但是目前情况来看那样的窗口一般不会被调用SetSize。
}

//--------------------------------------------------------------------------
//	功能：绝对坐标的调整
//--------------------------------------------------------------------------
void KWndWindow::AbsoluteMove(int dx, int dy)
{
	m_nAbsoluteLeft += dx;
	m_nAbsoluteTop  += dy;
	KWndWindow* pChild = m_pFirstChild;
	while(pChild)
	{
		pChild->AbsoluteMove(dx, dy);
		pChild = pChild->m_pNextWnd;
	}
}

//--------------------------------------------------------------------------
//	功能：显示窗口
//--------------------------------------------------------------------------
void KWndWindow::Show()
{
	m_Style |= WND_S_VISIBLE;

//	if (m_pFirstChild)
//    {
//        KWndWindow*	pChild = m_pFirstChild;
//        while (pChild)
//        {
//            pChild->Show();
//
//            pChild = pChild->m_pNextWnd;
//        }
//    }
}


//--------------------------------------------------------------------------
//	功能：隐藏窗口
//--------------------------------------------------------------------------
void KWndWindow::Hide()
{
	m_Style &= ~WND_S_VISIBLE;

	g_MouseOver.OnWndClosed((void*)this);
	KWndWindow* pFocus = Wnd_GetFocusWnd();
	while(pFocus)
	{
		if (pFocus != this)
			pFocus = pFocus->m_pParentWnd;
		else
		{
			Wnd_SetFocusWnd(NULL);
			break;
		}
	}
	Wnd_ReleaseExclusive(this);

//	if (m_pFirstChild)
//    {
//        KWndWindow*	pChild = m_pFirstChild;
//        while (pChild)
//        {
//            pChild->Hide();
//
//            pChild = pChild->m_pNextWnd;
//        }
//    }
}


//--------------------------------------------------------------------------
//	功能：判断窗口是否被显示
//--------------------------------------------------------------------------
int KWndWindow::IsVisible()
{
	return (m_Style & WND_S_VISIBLE);
}

//--------------------------------------------------------------------------
//	功能：禁止或者允许使窗口被操作
//--------------------------------------------------------------------------
void KWndWindow::Enable(int bEnable)
{
	if (bEnable)
		m_Style &= ~WND_S_DISABLE;
	else
		m_Style |= WND_S_DISABLE;
}

void KWndWindow::Destroy()
{
	m_Style |= WND_S_TOBEDESTROY;
}

void KWndWindow::Clone(KWndWindow* pCopy)
{
	if (pCopy)
	{
		pCopy->m_Width	= m_Width;
		pCopy->m_Height = m_Height;
		pCopy->m_Style = m_Style;
		pCopy->SetPosition(m_Left, m_Top);
	}
}

//--------------------------------------------------------------------------
//	功能：初始化窗口
//--------------------------------------------------------------------------
int KWndWindow::Init(KIniFile* pIniFile, const char* pSection)
{
	int		nValue1, nValue2;

	if (pIniFile && pSection)
	{
#ifdef _DEBUG
		strncpy(m_Caption, pSection, 32);
#endif
		pIniFile->GetInteger(pSection, "Width", 0, &m_Width);
		pIniFile->GetInteger(pSection, "Height",0, &m_Height);
		pIniFile->GetInteger(pSection, "Left",  0, &nValue1);
		pIniFile->GetInteger(pSection, "Top",   0, &nValue2);
		SetPosition(nValue1, nValue2);
		pIniFile->GetInteger(pSection, "Disable",  0, &nValue1);
		pIniFile->GetInteger(pSection, "Moveable", 0, &nValue2);
		if (nValue1)
			m_Style |= WND_S_DISABLE;
		else
			m_Style &= ~WND_S_DISABLE;
		if (nValue2)
			m_Style |= WND_S_MOVEALBE;
		else
			m_Style &= ~WND_S_MOVEALBE;

		nValue1 = nValue2 = 0;
		pIniFile->GetInteger2(pSection, "FollowMove", &nValue1, &nValue2);
		if (nValue1)
			m_Style |= WND_S_MOVE_WITH_R_EDGE;
		else
			m_Style &= ~WND_S_MOVE_WITH_R_EDGE;
		if (nValue2)
			m_Style |= WND_S_MOVE_WITH_B_EDGE;
		else
			m_Style &= ~WND_S_MOVE_WITH_B_EDGE;

		nValue1 = nValue2 = 0;
		pIniFile->GetInteger2(pSection, "FollowSize", &nValue1, &nValue2);
		if (nValue1)
			m_Style |= WND_S_SIZE_WITH_R_EDGE;
		else
			m_Style &= ~WND_S_SIZE_WITH_R_EDGE;
		if (nValue2)
			m_Style |= WND_S_SIZE_WITH_B_EDGE;
		else
			m_Style &= ~WND_S_SIZE_WITH_B_EDGE;
		pIniFile->GetInteger(pSection, "DummyWnd", 0, &nValue1);
		if (nValue1 == 0)
			m_Style &= ~WND_S_SIZE_WITH_ALL_CHILD;
		else
			m_Style |= WND_S_SIZE_WITH_ALL_CHILD;

		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：判断一个点是否落在窗口内,传入的是绝对坐标
//--------------------------------------------------------------------------
int KWndWindow::PtInWindow(int x, int y)
{
	int nRet = 0;
	if (m_Style & WND_S_VISIBLE)
	{
		if ((m_Style & WND_S_SIZE_WITH_ALL_CHILD) == 0)
		{
			nRet = (x >= m_nAbsoluteLeft && y >= m_nAbsoluteTop &&
				x < m_nAbsoluteLeft + m_Width && y < m_nAbsoluteTop + m_Height);
		}
		else
		{
			KWndWindow* pChild = m_pFirstChild;
			while(pChild)
			{
				if (pChild->PtInWindow(x, y))
				{
					nRet = 1;
					break;
				}
				pChild = pChild->m_pNextWnd;
			};
		}
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：绘制窗口（包括子窗口与后继兄弟窗口）
//--------------------------------------------------------------------------
void KWndWindow::Paint()
{
	if (m_Style & WND_S_VISIBLE)
	{
		PaintWindow();
		if (m_pFirstChild)
			m_pFirstChild->Paint();
	}
	if (m_pNextWnd)
		m_pNextWnd->Paint();
}

//--------------------------------------------------------------------------
//	功能：让窗口活动
//--------------------------------------------------------------------------
void KWndWindow::LetMeBreathe()
{
	if (m_pNextWnd)
		m_pNextWnd->LetMeBreathe();

	if (m_Style & WND_S_TOBEDESTROY)
		delete this;
	else if ((m_Style & (WND_S_VISIBLE | WND_S_DISABLE)) == WND_S_VISIBLE)
		Breathe();
}

//--------------------------------------------------------------------------
//	功能：添加子窗口
//--------------------------------------------------------------------------
void KWndWindow::AddChild(KWndWindow* pChild)
{
	if (pChild)
	{
		pChild->SplitSmaleFamily();

		//调整绝对坐标
		pChild->AbsoluteMove(m_nAbsoluteLeft + pChild->m_Left - pChild->m_nAbsoluteLeft,
			m_nAbsoluteTop + pChild->m_Top - pChild->m_nAbsoluteTop);

		//调整级连关系
		pChild->m_pParentWnd = this;
		if (m_pFirstChild == NULL)
			m_pFirstChild = pChild;
		else
		{
			KWndWindow* pBrother = m_pFirstChild;
			while(pBrother->m_pNextWnd)
				pBrother = pBrother->m_pNextWnd;
			pBrother->m_pNextWnd = pChild;
			pChild->m_pPreviousWnd = pBrother;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：添加子窗口
//--------------------------------------------------------------------------
void KWndWindow::AddBrother(KWndWindow* pBrother)
{
	if (pBrother)
	{
		pBrother->SplitSmaleFamily();

		//调整绝对坐标
		if (m_pParentWnd)
		{
			pBrother->AbsoluteMove(m_pParentWnd->m_nAbsoluteLeft + pBrother->m_Left - pBrother->m_nAbsoluteLeft,
				m_pParentWnd->m_nAbsoluteTop + pBrother->m_Top - pBrother->m_nAbsoluteTop);
		}

		//调整级连关系
		pBrother->m_pParentWnd = m_pParentWnd;
		if (m_pNextWnd == NULL)
		{
			m_pNextWnd = pBrother;
			pBrother->m_pPreviousWnd = this;
		}
		else
		{
			KWndWindow* pWnd = m_pNextWnd;
			while(pWnd->m_pNextWnd)
				pWnd = pWnd->m_pNextWnd;
			pWnd->m_pNextWnd = pBrother;
			pBrother->m_pPreviousWnd = pWnd;
		}
	}
}

//--------------------------------------------------------------------------
//	功能：世间再无窗在我左右，一无牵连
//--------------------------------------------------------------------------
void KWndWindow::LeaveAlone()
{
	SplitSmaleFamily();
	while (m_pFirstChild)
	{
		m_pFirstChild->m_pParentWnd = NULL;
		m_pFirstChild = m_pFirstChild->m_pNextWnd;
	}
}

//--------------------------------------------------------------------------
//	功能：（抛父弃兄唯留子，成立小家庭），把自己（及子窗口）从窗口树里面里面分离出来
//--------------------------------------------------------------------------
void KWndWindow::SplitSmaleFamily()
{
	if (IsVisible())
	{
		KWndWindow::Hide();
		KWndWindow::Show();
	}
	if (m_pPreviousWnd)
		m_pPreviousWnd->m_pNextWnd = m_pNextWnd;
	else if (m_pParentWnd)
		m_pParentWnd->m_pFirstChild = m_pNextWnd;
	if (m_pNextWnd)
		m_pNextWnd->m_pPreviousWnd = m_pPreviousWnd;
	m_pPreviousWnd = NULL;
	m_pNextWnd     = NULL;
	m_pParentWnd   = NULL;
}

//--------------------------------------------------------------------------
//	功能：窗口函数（处理消息）
//--------------------------------------------------------------------------
int KWndWindow::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int	nRet = 0;
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		OnLBtnDown(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
		nRet = 1;
		break;
	case WM_RBUTTONUP:
		if (m_pParentWnd)
		{
			m_pParentWnd->WndProc(WND_M_POPUPMENU, (unsigned int)(KWndWindow*)this, nParam);
			nRet = 1;
		}
		break;
	case WM_MOUSEMOVE:
		if (m_bMoving)
		{
			OnMoveWnd();
			nRet = 1;
		}
		break;
	case WM_LBUTTONUP:
		if (m_bMoving)
		{
			Wnd_ReleaseCapture();
			m_bMoving = false;
			if (m_pParentWnd)
			{
				m_pParentWnd->WndProc(WND_N_CHILD_MOVE, (unsigned int)(KWndWindow*)this, 0);
			}
		}
		nRet = 1;
		break;
	case WND_M_CAPTURECHANGED:
		m_bMoving = false;
		nRet = 1;
		break;
	}
	return nRet;
}

//--------------------------------------------------------------------------
//	功能：响应鼠标左键按下的操作
//--------------------------------------------------------------------------
void KWndWindow::OnLBtnDown(int x, int y)
{
	if (m_Style & WND_S_MOVEALBE)
	{
		m_bMoving = TRUE;
		m_nLastMouseHoldPosX = x;
		m_nLastMouseHoldPosY = y;
		Wnd_SetCapture(this);
	}
}

//--------------------------------------------------------------------------
//	功能：响应鼠标左键按下移动的操作
//--------------------------------------------------------------------------
void KWndWindow::OnMoveWnd()
{
	int x, y;
	Wnd_GetCursorPos(&x, &y);
	if (x != m_nLastMouseHoldPosX || y != m_nLastMouseHoldPosY)
	{
		x -= m_nLastMouseHoldPosX;
		y -= m_nLastMouseHoldPosY;
		SetPosition(x + m_Left, y + m_Top);
		m_nLastMouseHoldPosX += x;
		m_nLastMouseHoldPosY += y;

		x = (((short)x) & 0xffff) | ((((short)y) & 0xffff) << 16);
		if (x && m_pParentWnd)
		{
			m_pParentWnd->WndProc(WND_N_CHILD_MOVE, (unsigned int)(KWndWindow*)this, x);
		}
	}
}

//--------------------------------------------------------------------------
//	功能：得到处于指定坐标位置的最上层窗口，传入的坐标为绝对坐标
//--------------------------------------------------------------------------
KWndWindow* KWndWindow::TopChildFromPoint(int x, int y)
{
	KWndWindow* pLastMatch = NULL;
	if (PtInWindow(x, y))
	{
		pLastMatch = this;
		KWndWindow*	pWnd = m_pFirstChild;
		while(pWnd)
		{
			//一系列同级的兄弟窗口，要从最上面（链表最末端的）开始判断
			while(pWnd->m_pNextWnd)
				pWnd = pWnd->m_pNextWnd;
			while(pWnd)
			{
				if (pWnd->PtInWindow(x, y) && !pWnd->IsDisable())
				{
					pLastMatch = pWnd;
					pWnd = pLastMatch->m_pFirstChild;
					break;
				}
				pWnd = pWnd->m_pPreviousWnd;
			}
		}
	}
	return pLastMatch;
}

//--------------------------------------------------------------------------
//	功能：获得最顶层的父窗口
//--------------------------------------------------------------------------
KWndWindow* KWndWindow::GetOwner()
{
	KWndWindow* pWnd = this;
	while(pWnd->m_pParentWnd)
		pWnd = pWnd->m_pParentWnd;
	return pWnd;
}

//--------------------------------------------------------------------------
//	功能：设置窗口标题
//--------------------------------------------------------------------------
#ifdef _DEBUG
void KWndWindow::SetCaption(char* pszCaption)
{
	if (pszCaption)
		strncpy(m_Caption, pszCaption, 31);
}
#endif

void KWndWindow::PaintDebugInfo()
{
	char	szInfo[128];
	szInfo[0] = 0;
	int nInfoLen = 0;

	int nOld = WND_SHOW_DEBUG_FRAME_TEXT;
	WND_SHOW_DEBUG_FRAME_TEXT = true;
	KWndWindow::PaintWindow();
	WND_SHOW_DEBUG_FRAME_TEXT = nOld;

#ifdef _DEBUG
	sprintf(szInfo, "Name:%s, Pos:%d,%d,Size:%d,%d", m_Caption,
		m_nAbsoluteLeft, m_nAbsoluteTop, m_Width, m_Height);
	nInfoLen = strlen(szInfo);
#else
	sprintf(szInfo, "Pos:%d,%d,Size:%d,%d", m_nAbsoluteLeft, m_nAbsoluteTop, m_Width, m_Height);
	nInfoLen = strlen(szInfo);
#endif
	if (m_Style & WND_S_SIZE_WITH_ALL_CHILD)
	{
		RECT	rc;
		GetAllChildLayoutRect(&rc);
		sprintf(&szInfo[nInfoLen], ",Rect:%d,%d-%d,%d", rc.left, rc.top,
			rc.right, rc.bottom);
		nInfoLen = strlen(szInfo);
	}
	int x, y, w, h;
	Wnd_GetCursorPos(&x, &y);
	Wnd_GetScreenSize(w, h);
	if (x + nInfoLen * 6  + 24 > w)
		x = w - nInfoLen * 6 - 24;
	if (y + 25 > h)
		y = h - 13;
	else
		y += 12;
	//g_pRepresentShell->OutputText(12, szInfo, nInfoLen, x, y, 0xFFFF0000,
	//	0, TEXT_IN_SINGLE_PLANE_COORD, 0xffffffff);
}


//--------------------------------------------------------------------------
//	功能：窗体绘制
//--------------------------------------------------------------------------
void KWndWindow::PaintWindow()
{
	if (m_bMoving)
		OnMoveWnd();

	/*if (g_pRepresentShell && WND_SHOW_DEBUG_FRAME_TEXT)
	{
		KRULine		Line[4];

		for(int i = 0; i < 4; i++)
		{
			Line[i].Color.Color_dw = 0xff0000ff;
			Line[i].oPosition.nX = Line[i].oEndPos.nX = m_nAbsoluteLeft;
			Line[i].oPosition.nY = Line[i].oEndPos.nY = m_nAbsoluteTop;
		}

		Line[0].oEndPos.nX += m_Width;
		Line[1].oPosition.nY += m_Height;
		Line[1].oEndPos.nX += m_Width;
		Line[1].oEndPos.nY += m_Height;
		Line[2].oEndPos.nY += m_Height;
		Line[3].oPosition.nX += m_Width;
		Line[3].oEndPos.nX += m_Width;
		Line[3].oEndPos.nY += m_Height;

		g_pRepresentShell->DrawPrimitives(4, Line, RU_T_LINE, true);

		if (m_Style & WND_S_SIZE_WITH_ALL_CHILD)
		{
			for(int i = 0; i < 4; i++)
				Line[i].Color.Color_dw = 0xffff0000;
			RECT	rc;
			GetAllChildLayoutRect(&rc);
			Line[0].oPosition.nX = Line[1].oPosition.nX = rc.left;
			Line[0].oEndPos.nX = Line[1].oEndPos.nX = rc.right;
			Line[0].oPosition.nY = Line[0].oEndPos.nY = rc.top;
			Line[1].oPosition.nY = Line[1].oEndPos.nY = rc.bottom;

			Line[0].oPosition.nY = Line[1].oPosition.nY = rc.top;
			Line[0].oEndPos.nY = Line[1].oEndPos.nY = rc.bottom;
			Line[0].oPosition.nX = Line[0].oEndPos.nX = rc.left;
			Line[1].oPosition.nX = Line[1].oEndPos.nX = rc.right;

			g_pRepresentShell->DrawPrimitives(4, Line, RU_T_LINE, true);
		}

#ifdef _DEBUG
		g_pRepresentShell->OutputText(12, m_Caption, -1, m_nAbsoluteLeft, m_nAbsoluteTop, 0xFFFFFFFF);
#endif
	}*/
}

//--------------------------------------------------------------------------
//	功能：把字符串表示的颜色信息转为数值表示
//--------------------------------------------------------------------------
unsigned int GetColor(LPCTSTR pString)
{
	if (pString == NULL)
		return 0;

	unsigned int Color = 0xFF000000;

	char Buf[16] = "";
	int  i = 0;
	int  n = 0;
	while (pString[i] != ',')
	{
		if (pString[i] == 0 || n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}

	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 16);
	n = 0;
	i++;
	while (pString[i] != ',')
	{
		if (pString[i] == 0 || n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	Buf[n] = 0;
	Color += ((atoi(Buf) & 0xFF) << 8);
	n = 0;
	i++;
	while (pString[i] != 0)
	{
		if (n >= 15)
			return Color;
		Buf[n++] = pString[i++];
	}
	Buf[n] = 0;
	Color += (atoi(Buf) & 0xFF);
	return Color;
}

const char* GetColorString(unsigned int nColor)
{
	static char szColor[12];
	KRColor c;
	c.Color_dw = nColor;
	sprintf(szColor, "%d,%d,%d", c.Color_b.r, c.Color_b.g, c.Color_b.b);
	szColor[11] = 0;

	return szColor;
}

//使鼠标指针以移动到悬浮在此窗口中的位置上
void KWndWindow::SetCursorAbove()
{
	Wnd_SetCursorPos(m_nAbsoluteLeft + m_Width / 2,
		m_nAbsoluteTop + m_Height / 2);
}

//取得包含所有子窗口分布区域的最小区域
void KWndWindow::GetAllChildLayoutRect(RECT* pRect)
{
	if (pRect == NULL)
		return;
	if (m_pFirstChild == NULL)
	{
		pRect->left = pRect->right = m_nAbsoluteLeft;
		pRect->top = pRect->bottom = m_nAbsoluteTop;
	}
	else
	{
		m_pFirstChild->GetAbsolutePos((int*)&pRect->left, (int*)&pRect->top);
		pRect->right = pRect->left;
		pRect->bottom = pRect->top;

		KWndWindow* pChild = m_pFirstChild;
		do
		{
			RECT	rc;
			pChild->GetAllChildLayoutRect(&rc);
			int		x, y, w, h;
			pChild->GetAbsolutePos(&x, &y);
			pChild->GetSize(&w, &h);

			if (rc.left > x)
				rc.left = x;
			if (rc.top > y)
				rc.top = y;
			if (rc.right < x + w)
				rc.right = x + w;
			if (rc.bottom < y + h)
				rc.bottom = y + h;

			if (pRect->left > rc.left)
				pRect->left = rc.left;
			if (pRect->top > rc.top)
				pRect->top = rc.top;
			if (pRect->right < rc.right)
				pRect->right = rc.right;
			if (pRect->bottom < rc.bottom)
				pRect->bottom = rc.bottom;
		}while(pChild = pChild->m_pNextWnd);
	}
}

//设置一个窗口参数
int KWndWindow::SetWindowLong(enumWINDOW_LONG_INDEX eIndex, int nValue)
{
	if(eIndex >= 0 && eIndex < enumWLI_NUM)
	{
		m_nParam[eIndex] = nValue;
		return nValue;
	}
	return -1;
}

//获取窗口参数
int KWndWindow::GetWindowLong(enumWINDOW_LONG_INDEX eIndex)
{
	if(eIndex >= 0 && eIndex < enumWLI_NUM)
	{
		return m_nParam[eIndex];
	}
	return -1;
}
