/*****************************************************************************************
//	界面窗口体系结构--容纳游戏对象的窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-9-25
*****************************************************************************************/
#include "../../Public/KWin32.h"
#include "../../Public/KIniFile.h"
#include "../UiBase.h"
#include "../Elem/WndMessage.h"
#include "../elem/wnds.h"
#include "WndObjContainer.h"
#include "../Elem/MouseHover.h"
#include "../../Public/CoreObjGenreDef.h"
#include "../../../core/src/CoreShell.h"
#include "../../Public/GameDataDef.h"
#include "../../Public/iRepresentShell.h"
#include "../../Public/KRepresentUnit.h"
#include "../../Public/Text.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

extern iCoreShell*	g_pCoreShell;
extern iRepresentShell*	g_pRepresentShell;


unsigned int l_BgColors[] =
{
	0x0a001e13,		//IIEP_NORMAL 一般/正常/可用 0,30,19
	0x0a2c0000,		//IIEP_NOT_USEABLE 不可用/不可装配 44,0,0
	0x0a654915,		//IIEP_SPECIAL 特定的不同情况 101，73，21
	0x0a000636,		//mouse over 0,6,54
	0x0a0000ff,		//放下物品的位置的颜色
	0x0a595004,		//为摆摊标过价的物品颜色
};

void WndObjContainerInit(KIniFile* pIni)
{
#define	COLOR_SECTION	"ObjContColor"
	if (pIni)
	{
		char	Buff[16];
		int		nAlpha;
		pIni->GetInteger(COLOR_SECTION, "Alpha", 0, &nAlpha);
		if (nAlpha < 0)
			nAlpha = 0;
		else if (nAlpha > 255)
			nAlpha = 255;
		nAlpha = ((nAlpha << 21) & 0xff000000);
		pIni->GetString(COLOR_SECTION, "NormalColor", "", Buff, sizeof(Buff));
		l_BgColors[0] = (GetColor(Buff) & 0xffffff) | nAlpha;
		pIni->GetString(COLOR_SECTION, "NotUseableColor", "", Buff, sizeof(Buff));
		l_BgColors[1] = (GetColor(Buff) & 0xffffff) | nAlpha;
		pIni->GetString(COLOR_SECTION, "SpecialColor", "", Buff, sizeof(Buff));
		l_BgColors[2] = (GetColor(Buff) & 0xffffff) | nAlpha;
		pIni->GetString(COLOR_SECTION, "MouseOverColor", "", Buff, sizeof(Buff));
		l_BgColors[3] = (GetColor(Buff) & 0xffffff) | nAlpha;
		pIni->GetString(COLOR_SECTION, "PutdownColor", "", Buff, sizeof(Buff));
		l_BgColors[4] = (GetColor(Buff) & 0xffffff) | nAlpha;
		pIni->GetString(COLOR_SECTION, "PriceMarkedColor", "", Buff, sizeof(Buff));
		l_BgColors[5] = (GetColor(Buff) & 0xffffff) | nAlpha;
	}
}

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndObjectBox::KWndObjectBox()
{
	m_uAcceptableGenre = CGOG_NOTHING;
	m_Object.uGenre = CGOG_NOTHING;
	m_Object.DataX = 0;
	m_Object.DataY = 0;
	m_nContainerId = 0;
}

void KWndObjectBox::SetContainerId(int nId)
{
	m_nContainerId = nId;
}

//--------------------------------------------------------------------------
//	功能：设置可以容纳的对象的类型
//--------------------------------------------------------------------------
void KWndObjectBox::SetObjectGenre(unsigned int uGenre)
{
	m_uAcceptableGenre = uGenre;
}

//--------------------------------------------------------------------------
//	功能：清除对象物品
//--------------------------------------------------------------------------
void KWndObjectBox::Celar()
{
	m_Object.uGenre = CGOG_NOTHING;
}

//--------------------------------------------------------------------------
//	功能：获取容纳的对象信息
//--------------------------------------------------------------------------
int KWndObjectBox::GetObject(KUiDraggedObject& Obj) const
{
	if ((Obj.uGenre = m_Object.uGenre) != CGOG_NOTHING)
	{
		Obj = m_Object;
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：设置容纳的对象
//--------------------------------------------------------------------------
void KWndObjectBox::HoldObject(unsigned int uGenre, unsigned int uId, int nDataW, int nDataH)
{
	m_Object.uGenre = uGenre;
	m_Object.uId = uId;
	m_Object.DataW = nDataW;
	m_Object.DataH = nDataH;

	if (g_MouseOver.IsMoseHoverWndObj(this, 0))
	{
		if (m_Object.uGenre != CGOG_NOTHING)
		{
			int x, y;
			Wnd_GetCursorPos(&x, &y);
			SetMouseHoverObjectDesc(this, 0, m_Object.uGenre,
				m_Object.uId, m_nContainerId, x, y);
		}
		else
		{
			g_MouseOver.CancelMouseHoverInfo();
		}
	}
}

//--------------------------------------------------------------------------
//	功能：窗体绘制
//--------------------------------------------------------------------------
void KWndObjectBox::PaintWindow()
{
	KWndWindow::PaintWindow();
	if (m_Object.uGenre != CGOG_NOTHING && g_pRepresentShell)
	{
		KRUShadow	Shadow;
		Shadow.Color.Color_dw = 0;
		if (m_Style & OBJCONT_F_MOUSE_HOVER)
			Shadow.Color.Color_dw = l_BgColors[3];
		else if (m_Style & OBJCONT_S_HAVEOBJBGCOLOR)
		{
			KUiObjAtContRegion	Obj;
			Obj.Obj.uGenre = m_Object.uGenre;
			Obj.Obj.uId = m_Object.uId;
			Obj.Region.h = Obj.Region.v = 0;
			Obj.Region.Width = Obj.Region.Height = 0;
			Obj.nContainer = m_nContainerId;

			ITEM_IN_ENVIRO_PROP eProp = (ITEM_IN_ENVIRO_PROP)g_pCoreShell->
				GetGameData(GDI_ITEM_IN_ENVIRO_PROP, (unsigned int)&Obj, 0);
			if (eProp == IIEP_NORMAL)
				Shadow.Color.Color_dw = l_BgColors[0];
			else if (eProp == IIEP_NOT_USEABLE)
				Shadow.Color.Color_dw = l_BgColors[1];
			else if (eProp == IIEP_SPECIAL)
				Shadow.Color.Color_dw = l_BgColors[2];
		}
		if (Shadow.Color.Color_dw)
		{
			Shadow.oPosition.nX = m_nAbsoluteLeft;
			Shadow.oPosition.nY = m_nAbsoluteTop;
			Shadow.oEndPos.nX = m_nAbsoluteLeft + m_Width;
			Shadow.oEndPos.nY = m_nAbsoluteTop + m_Height;
			g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
		}
		g_pCoreShell->DrawGameObj(m_Object.uGenre, m_Object.uId,
			m_nAbsoluteLeft, m_nAbsoluteTop, m_Width, m_Height, 0);
	}
}

void KWndObjectBox::Clone(KWndObjectBox* pCopy)
{
	if (pCopy)
	{
		KWndWindow::Clone(pCopy);
		pCopy->m_uAcceptableGenre = m_uAcceptableGenre;
	}
}

//初始化
int	KWndObjectBox::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndWindow::Init(pIniFile, pSection))
	{
		int nValue;
		pIniFile->GetInteger(pSection, "EnableClickEmpty", 0, &nValue);
		if (nValue)
			m_Style |= OBJCONT_S_ENABLE_CLICK_EMPTY;
		else
			m_Style &= ~OBJCONT_S_ENABLE_CLICK_EMPTY;

		pIniFile->GetInteger(pSection, "HaveBgColor", 1, &nValue);
		if (nValue)
			m_Style |= OBJCONT_S_HAVEOBJBGCOLOR;
		else
			m_Style &= ~OBJCONT_S_HAVEOBJBGCOLOR;
		return true;
	}
	return false;
}

void KWndObjectBox::EnablePickPut(bool bEnable)
{
	if (bEnable == false)
		m_Style |= OBJCONT_S_DISABLE_PICKPUT;
	else
		m_Style &= ~OBJCONT_S_DISABLE_PICKPUT;
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KWndObjectBox::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nStallState = g_pCoreShell->GetGameData(GDI_IS_STALL, -1, -1);
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		if (m_pParentWnd && !nStallState &&
			(m_Style & OBJCONT_S_DISABLE_PICKPUT) == 0)
		{
			if (Wnd_GetDragObj(NULL))
			{
				DropObject(false);
			}
			else if (m_Object.uGenre != CGOG_NOTHING)
			{
				ITEM_PICKDROP_PLACE	Pick;
				Pick.pWnd = this;
				Pick.h = 0;
				Pick.v = 0;
				m_pParentWnd->WndProc(WND_N_ITEM_PICKDROP,
					(unsigned int)&Pick, NULL);
			}
		}
		break;
	case WM_LBUTTONUP:
		if (m_pParentWnd && ((m_Style & OBJCONT_S_DISABLE_PICKPUT) || nStallState))
		{
			if (m_Object.uGenre != CGOG_NOTHING)
			{
				KUiDraggedObject	Obj;
				Obj = m_Object;
				m_pParentWnd->WndProc(WND_N_LEFT_CLICK_ITEM,
					(unsigned int)&Obj, (int)(KWndWindow*)this);
			}
			else if (m_Style & OBJCONT_S_ENABLE_CLICK_EMPTY)
			{
				m_pParentWnd->WndProc(WND_N_LEFT_CLICK_ITEM,
					NULL, (int)(KWndWindow*)this);
			}
		}
		break;
	case WM_RBUTTONUP:
		if (m_pParentWnd)
		{
			if (m_Object.uGenre != CGOG_NOTHING)
			{
				KUiDraggedObject	Obj;
				Obj = m_Object;
				m_pParentWnd->WndProc(WND_N_RIGHT_CLICK_ITEM,
					(unsigned int)&m_Object, (int)(KWndWindow*)this);
			}
			else if (m_Style & OBJCONT_S_ENABLE_CLICK_EMPTY)
			{
				m_pParentWnd->WndProc(WND_N_LEFT_CLICK_ITEM,
					NULL, (int)(KWndWindow*)this);
			}
		}
		break;
	case WM_MOUSEHOVER:
	case WM_MOUSEMOVE:
		m_Style |= OBJCONT_F_MOUSE_HOVER;
		if (m_Object.uGenre != CGOG_NOTHING && g_MouseOver.IsMoseHoverWndObj(this, 0) == 0)
			SetMouseHoverObjectDesc(this, 0, m_Object.uGenre,
				m_Object.uId, m_nContainerId, GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
		break;
	case WND_M_MOUSE_LEAVE:
		m_Style &= ~OBJCONT_F_MOUSE_HOVER;
		KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	default:
		return KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

//--------------------------------------------------------------------------
//	功能：放置物品
//--------------------------------------------------------------------------
int KWndObjectBox::DropObject(bool bTestOnly)
{
	KUiDraggedObject	DragObj;
	Wnd_GetDragObj(&DragObj);

	if (m_uAcceptableGenre != CGOG_NOTHING)
	{
		if ((m_uAcceptableGenre != DragObj.uGenre) &&
			((m_uAcceptableGenre & 0xFFFF) != (DragObj.uGenre & 0xFFFF)) || ((m_uAcceptableGenre & 0xFFFF0000) != 0))
		return false;
	}
	if (bTestOnly)
		return true;

	ITEM_PICKDROP_PLACE	Pick, Drop;
	Drop.pWnd = this;
	Drop.h = Drop.v = 0;
	if (m_Object.uGenre == CGOG_NOTHING)
		m_pParentWnd->WndProc(WND_N_ITEM_PICKDROP, NULL, (int)&Drop);
	else
	{
		Pick.pWnd = this;
		Pick.h = 0;
		Pick.v = 0;
		m_pParentWnd->WndProc(WND_N_ITEM_PICKDROP, (unsigned int)&Pick, (int)&Drop);
	}
	return true;
}

#define	NO_MATCHED_PUT_POS			-761209
#define	REPLACE_ITEM_POS(iItem)		(-(iItem) - 1)
#define	REPLACE_ITEM_INDEX(iPos)	((-iPos) - 1)

//--------------------------------------------------------------------------
//	功能：构造函数
//--------------------------------------------------------------------------
KWndObjectMatrix::KWndObjectMatrix()
{
	m_nNumUnitHori = 1;
	m_nNUmUnitVert = 1;
	m_nUnitWidth = 1;
	m_nUnitHeight = 1;
	m_nNumObjects = 0;
	m_pObjects = NULL;
	m_nMouseOverObj = -1;
	m_nPutPosX = NO_MATCHED_PUT_POS;
	m_nContainerId = 0;
}

void KWndObjectMatrix::SetContainerId(int nId)
{
	m_nContainerId = nId;
}

KWndObjectMatrix::~KWndObjectMatrix()
{
	Clear();
}

void KWndObjectMatrix::Clone(KWndObjectMatrix* pCopy)
{
	if (pCopy)
	{
		KWndWindow::Clone(pCopy);
		pCopy->m_nNumUnitHori = m_nNumUnitHori;
		pCopy->m_nNUmUnitVert = m_nNUmUnitVert;
		pCopy->m_nUnitWidth  = m_nUnitWidth;
		pCopy->m_nUnitHeight = m_nUnitHeight;
	}
}

// -------------------------------------------------------------------------
//	功能：初始化窗口
// -------------------------------------------------------------------------
int KWndObjectMatrix::Init(KIniFile* pIniFile, const char* pSection)
{
	if (KWndWindow::Init(pIniFile, pSection))
	{
		pIniFile->GetInteger(pSection, "HUnits", 1, &m_nNumUnitHori);
		pIniFile->GetInteger(pSection, "VUnits", 1, &m_nNUmUnitVert);
		if (m_nNumUnitHori < 1)
			m_nNumUnitHori = 1;
		if (m_nNUmUnitVert < 1)
			m_nNUmUnitVert = 1;
		m_nUnitWidth = m_Width / m_nNumUnitHori;
		if (m_nUnitWidth < 1)
			m_nUnitWidth = 1;
		m_nUnitHeight = m_Height / m_nNUmUnitVert;
		if (m_nUnitHeight < 1)
			m_nUnitHeight = 1;
		int nValue;
		pIniFile->GetInteger(pSection, "HaveBgColor", 1, &nValue);
		if (nValue)
			m_Style |= OBJCONT_S_HAVEOBJBGCOLOR;
		else
			m_Style &= ~OBJCONT_S_HAVEOBJBGCOLOR;
		pIniFile->GetInteger(pSection, "AcceptFree", 0, &nValue);
		if (nValue)
			m_Style |= OBJCONT_S_ACCEPT_FREE;
		else
			m_Style &= ~OBJCONT_S_ACCEPT_FREE;
		pIniFile->GetInteger(pSection, "UnitBorder", 0, &m_nUnitBorder);
		if (m_nUnitBorder >= m_nUnitWidth)
			m_nUnitBorder = m_nUnitWidth - 1;
		if (m_nUnitBorder >= m_nUnitHeight)
			m_nUnitBorder = m_nUnitHeight - 1;
		if (m_nUnitBorder < 0)
			m_nUnitBorder = 0;

		m_nPutPosX = NO_MATCHED_PUT_POS;
		return true;
	}
	return false;
}

// -------------------------------------------------------------------------
// 功能	: 窗体绘制
// -------------------------------------------------------------------------
void KWndObjectMatrix::PaintWindow()
{
	KWndWindow::PaintWindow();
	KRUShadow	Shadow;
	for (int i = 0; i < m_nNumObjects; i++)
	{
		KUiDraggedObject* pObj = &m_pObjects[i];
		Shadow.Color.Color_dw = 0;
		if (i == REPLACE_ITEM_INDEX(m_nPutPosX))
			Shadow.Color.Color_dw = l_BgColors[4];
		else if ((m_Style & OBJCONT_F_MOUSE_HOVER) && m_nMouseOverObj == i)
			Shadow.Color.Color_dw = l_BgColors[3];

		//正在标价或者正在摆摊[wxb 2003-11-21]
		else if (g_UiBase.GetStatus() & UIS_S_STALL_SETPRICE ||
			g_pCoreShell->GetGameData(GDI_IS_STALL, -1, -1) == 1)
		{
			KUiObjAtContRegion	Obj;
			Obj.Obj.uGenre = pObj->uGenre;
			Obj.Obj.uId = pObj->uId;
			Obj.Region.h = Obj.Region.v = 0;
			Obj.Region.Width = Obj.Region.Height = 0;
			Obj.nContainer = m_nContainerId;

			KUiItemBuySelInfo info;
			if (g_pCoreShell->GetGameData(GDI_STALL_ITEM_PRICE, (unsigned int)&Obj, (int)&info))
			{
				Shadow.Color.Color_dw = l_BgColors[5];
			}
		}
		else if (m_Style & OBJCONT_S_HAVEOBJBGCOLOR)
		{
			KUiObjAtContRegion	Obj;
			Obj.Obj.uGenre = pObj->uGenre;
			Obj.Obj.uId = pObj->uId;
			Obj.Region.h = Obj.Region.v = 0;
			Obj.Region.Width = Obj.Region.Height = 0;
			Obj.nContainer = m_nContainerId;

			ITEM_IN_ENVIRO_PROP eProp = (ITEM_IN_ENVIRO_PROP)g_pCoreShell->
				GetGameData(GDI_ITEM_IN_ENVIRO_PROP, (unsigned int)&Obj, 0);
			if (eProp == IIEP_NORMAL)
				Shadow.Color.Color_dw = l_BgColors[0];
			else if (eProp == IIEP_NOT_USEABLE)
				Shadow.Color.Color_dw = l_BgColors[1];
			else if (eProp == IIEP_SPECIAL)
				Shadow.Color.Color_dw = l_BgColors[2];
		}

		int width = m_nUnitWidth * pObj->DataW - m_nUnitBorder * 2;
		int height = m_nUnitHeight * pObj->DataH - m_nUnitBorder * 2;
		Shadow.oPosition.nX = m_nAbsoluteLeft + m_nUnitWidth * pObj->DataX + m_nUnitBorder;
		Shadow.oPosition.nY = m_nAbsoluteTop + m_nUnitHeight * pObj->DataY + m_nUnitBorder;
		if (Shadow.Color.Color_dw)
		{
			Shadow.oEndPos.nX = Shadow.oPosition.nX + width;
			Shadow.oEndPos.nY = Shadow.oPosition.nY + height;
			g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
		}

		g_pCoreShell->DrawGameObj(pObj->uGenre, pObj->uId,
			Shadow.oPosition.nX, Shadow.oPosition.nY, width, height, 0);
	}
	if (m_nPutPosX >= 0)
	{
		Shadow.oPosition.nX = m_nAbsoluteLeft + m_nUnitWidth * m_nPutPosX + m_nUnitBorder;
		Shadow.oPosition.nY = m_nAbsoluteTop + m_nUnitHeight * m_nPutPosY + m_nUnitBorder;
		Shadow.oEndPos.nX = Shadow.oPosition.nX + m_nUnitWidth * m_nPutWidth - m_nUnitBorder * 2;
		Shadow.oEndPos.nY = Shadow.oPosition.nY + m_nUnitHeight * m_nPutHeight - m_nUnitBorder * 2;
		Shadow.Color.Color_dw = l_BgColors[4];
		g_pRepresentShell->DrawPrimitives(1, &Shadow, RU_T_SHADOW, true);
	}
}

// -------------------------------------------------------------------------
// 功能	: 增加一个对象物品
// -------------------------------------------------------------------------
int KWndObjectMatrix::AddObject(KUiDraggedObject* pObject, int nCount)
{
	if (pObject && nCount > 0)
	{
		int	i, nValidCount = 0;
		for (i = 0; i < nCount; i++)
		{
			if (pObject[i].uGenre)
				nValidCount++;
		}
		if (nValidCount)
		{
			KUiDraggedObject* pNewList = (KUiDraggedObject*)realloc(m_pObjects, sizeof(KUiDraggedObject) * (m_nNumObjects + nValidCount));
			if (pNewList)
			{
				m_pObjects = pNewList;
				for (i = 0; i < nCount; i++)
				{
					if (pObject[i].uGenre)
					{
						m_pObjects[m_nNumObjects] = pObject[i];
						m_nNumObjects ++;
					}
				}
			}
			m_nPutPosX = NO_MATCHED_PUT_POS;
			return true;
		}
	}
	return false;
}

// -------------------------------------------------------------------------
// 功能	: 减少一个对象物品
// -------------------------------------------------------------------------
int KWndObjectMatrix::RemoveObject(KUiDraggedObject* pObject)
{
	if (pObject)
	{
		m_nPutPosX = NO_MATCHED_PUT_POS;
		m_nMouseOverObj = -1;
		for (int i = 0; i < m_nNumObjects; i++)
		{
			KUiDraggedObject* pHolded = &m_pObjects[i];
			if (pHolded->DataX == pObject->DataX &&
				pHolded->DataY == pObject->DataY)
			{
				if (g_MouseOver.IsMoseHoverWndObj(this, i))
					g_MouseOver.CancelMouseHoverInfo();
				m_nNumObjects --;
				for (; i < m_nNumObjects; i++)
					m_pObjects[i] = m_pObjects[i + 1];
				return true;
			}
		}
	}
	return false;
}

// -------------------------------------------------------------------------
// 功能	: 获取容纳的某个对象信息
//	返回：对象的数目
// -------------------------------------------------------------------------
int	KWndObjectMatrix::GetObject(KUiDraggedObject& Obj, int x, int y) const
{
	for (int i = 0; i < m_nNumObjects; i++)
	{
		KUiDraggedObject*	pHolded = &m_pObjects[i];
		if (x == pHolded->DataX && y == pHolded->DataY)
		{
			Obj = *pHolded;
			return true;
		}
	}
	return false;
}

// -------------------------------------------------------------------------
// 功能	: 获取容纳的对象信息
//	返回：对象的数目
// -------------------------------------------------------------------------
/*int KWndObjectMatrix::GetObjects(KUiGameObject* pObjects, int nCount) const
{
	if (m_nNumObjects <= nCount)
	{
		for (int i = 0; i < m_nNumObjects; i++)
		{
			pObjects[i].uGenre = m_pObjects[i].uGenre;
			pObjects[i].uId = m_pObjects[i].uId;
		}
		return m_nNumObjects;
	}
	return m_nNumObjects;
}*/

//--------------------------------------------------------------------------
//	功能：清除全部的对象物品
//--------------------------------------------------------------------------
void KWndObjectMatrix::Clear()
{
	m_nNumObjects = 0;
	m_nMouseOverObj = -1;
	if (m_pObjects)
	{
		free(m_pObjects);
		m_pObjects = NULL;
	}
}

//--------------------------------------------------------------------------
//	功能：窗口函数
//--------------------------------------------------------------------------
int KWndObjectMatrix::WndProc(unsigned int uMsg, unsigned int uParam, int nParam)
{
	int nStallState = g_pCoreShell->GetGameData(GDI_IS_STALL, -1, -1);
	switch(uMsg)
	{
	case WM_LBUTTONDOWN:
		if (!nStallState && (m_Style & OBJCONT_S_DISABLE_PICKPUT)== 0)
		{
			if (Wnd_GetDragObj(NULL))
				DropObject(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam), false);
			else
				PickUpObjectAt(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
		}
		break;
	case WM_LBUTTONUP:
		if ((nStallState || (m_Style & OBJCONT_S_DISABLE_PICKPUT)) && m_pParentWnd)
		{
			m_nPutPosX = NO_MATCHED_PUT_POS;
			int nObj = GetObjectAt(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
			if (nObj >= 0)
			{
				KUiDraggedObject	Obj;
				Obj = m_pObjects[nObj];
				m_pParentWnd->WndProc(WND_N_LEFT_CLICK_ITEM,
					(unsigned int)&Obj, (int)(KWndWindow*)this);
			}
		}
		break;
	case WM_RBUTTONUP:
		m_nPutPosX = NO_MATCHED_PUT_POS;
		if (m_pParentWnd)
		{
			int nObj = GetObjectAt(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam));
			if (nObj >= 0)
			{
				KUiDraggedObject	Obj;
				Obj = m_pObjects[nObj];
				m_pParentWnd->WndProc(WND_N_RIGHT_CLICK_ITEM,
					(unsigned int)&Obj, (int)(KWndWindow*)this);
			}
		}
		break;
	case WM_MOUSEHOVER:
	case WM_MOUSEMOVE:
		m_Style |= OBJCONT_F_MOUSE_HOVER;
		{
			int	x = GET_X_LPARAM(nParam);
			int y = GET_Y_LPARAM(nParam);
			int nObj = GetObjectAt(x, y);
			m_nMouseOverObj = nObj;
			if (nObj >= 0)
			{
				if (g_MouseOver.IsMoseHoverWndObj(this, nObj) == 0)
				{
					SetMouseHoverObjectDesc(this, nObj, m_pObjects[nObj].uGenre,
						m_pObjects[nObj].uId, m_nContainerId, x, y);
				}
			}
			else
				g_MouseOver.CancelMouseHoverInfo();
			if ((m_Style & OBJCONT_S_TRACE_PUT_POS) && Wnd_GetDragObj(NULL))
			{
				DropObject(GET_X_LPARAM(nParam), GET_Y_LPARAM(nParam), true);
			}
		}
		break;
	case WND_M_MOUSE_LEAVE:
		m_nPutPosX = NO_MATCHED_PUT_POS;
		m_Style &= ~OBJCONT_F_MOUSE_HOVER;
		KWndWindow::WndProc(uMsg, uParam, nParam);
		break;
	default:
		return KWndWindow::WndProc(uMsg, uParam, nParam);
	}
	return 0;
}

//--------------------------------------------------------------------------
//	功能：获得某个位置上的物品
//--------------------------------------------------------------------------
int KWndObjectMatrix::GetObjectAt(int x, int y)
{
	x = (x - m_nAbsoluteLeft) / m_nUnitWidth;
	y = (y - m_nAbsoluteTop) / m_nUnitHeight;
	for (int i = 0; i < m_nNumObjects; i++)
	{
		KUiDraggedObject*	pHolded = &m_pObjects[i];
		if (x < pHolded->DataX || y < pHolded->DataY ||
			x >= pHolded->DataX + pHolded->DataW ||
			y >= pHolded->DataY + pHolded->DataH)
			continue;
		return i;
	}
	return -1;
}

//--------------------------------------------------------------------------
//	功能：捡起某个位置上的对象
//--------------------------------------------------------------------------
int KWndObjectMatrix::PickUpObjectAt(int x, int y)
{
	int nPicked = GetObjectAt(x, y);
	if (nPicked >= 0)
	{
		ITEM_PICKDROP_PLACE	Pick;
		Pick.pWnd = this;
		Pick.h = m_pObjects[nPicked].DataX;
		Pick.v = m_pObjects[nPicked].DataY;
		m_pParentWnd->WndProc(WND_N_ITEM_PICKDROP, (unsigned int)&Pick, NULL);
		return true;
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：放置物品
//--------------------------------------------------------------------------
int KWndObjectMatrix::DropObject(int x, int y, bool bTestOnly)
{
	KUiDraggedObject	DragObj;

	m_nPutPosX = NO_MATCHED_PUT_POS;
	if (m_Style & OBJCONT_S_ACCEPT_FREE)
	{
		if (bTestOnly == false)
			DropObject(x, y, (KUiDraggedObject*)NULL);
		return true;
	}

	Wnd_GetDragObj(&DragObj);
	if (DragObj.DataW > m_nNumUnitHori || DragObj.DataH > m_nNUmUnitVert)
		return false;

	//转换成格子坐标
	RECT	or;
	KUiDraggedObject* pOverlaped = NULL;

	x = (x - m_nAbsoluteLeft) / m_nUnitWidth;
	y = (y - m_nAbsoluteTop) / m_nUnitHeight;
	if ((or.right = x + (DragObj.DataW + 1) / 2) > m_nNumUnitHori)
		or.right = m_nNumUnitHori;
	if ((or.bottom = y + (DragObj.DataH + 1) / 2) > m_nNUmUnitVert)
		or.bottom = m_nNUmUnitVert;
	if (or.right >= DragObj.DataW)
		or.left = or.right - DragObj.DataW;
	else
	{
		or.left = 0;
		or.right = DragObj.DataW;
	}
	if (or.bottom >= DragObj.DataH)
		or.top = or.bottom - DragObj.DataH;
	else
	{
		or.top = 0;
		or.bottom = DragObj.DataH;
	}

	if (TryDropObjAtPos(or, pOverlaped))
	{
		if (bTestOnly == false)
			DropObject(or.left, or.top, pOverlaped);
		else if (pOverlaped)
		{
			m_nPutPosX = REPLACE_ITEM_POS(pOverlaped - m_pObjects);
		}
		else
		{
			m_nPutPosX = or.left;
			m_nPutPosY = or.top;
			m_nPutWidth = DragObj.DataW;
			m_nPutHeight = DragObj.DataH;
		}
		return true;
	}

	if (DragObj.DataW == 1 && DragObj.DataH == 1)
		return false;

	RECT	Try;
	Try.right = x;
	Try.bottom = y;
	if ((Try.left = x - DragObj.DataW + 1) < 0)
		Try.left = 0;
	if ((Try.top = y - DragObj.DataH + 1) < 0)
		Try.top = 0;

	for (or.left = Try.left; or.left <= Try.right; or.left ++)
	{
		or.right = or.left + DragObj.DataW;
		for (or.top = Try.top; or.top <= Try.bottom; or.top ++)
		{
			or.bottom = or.top + DragObj.DataH;
			if (TryDropObjAtPos(or, pOverlaped))
			{
				if (bTestOnly == false)
					DropObject(or.left, or.top, pOverlaped);
				return true;
			}
		}
	}
	return false;
}

//--------------------------------------------------------------------------
//	功能：尝试放置物品
//--------------------------------------------------------------------------
int KWndObjectMatrix::TryDropObjAtPos(const RECT& dor, KUiDraggedObject*& pOverlaped)
{
	pOverlaped = NULL;
	for (int i = 0; i < m_nNumObjects; i++)
	{
		KUiDraggedObject*	pHolded = &m_pObjects[i];
		if (pHolded->DataX >= dor.right || pHolded->DataY >= dor.bottom ||
			pHolded->DataX + pHolded->DataW <= dor.left ||
			pHolded->DataY + pHolded->DataH <= dor.top)
			continue;
		if (pOverlaped)
			break;
		pOverlaped = pHolded;
	}
	return (i == m_nNumObjects);
}

//--------------------------------------------------------------------------
//	功能：放下物品
//--------------------------------------------------------------------------
void KWndObjectMatrix::DropObject(int x, int y, KUiDraggedObject* pToPickUpObj)
{
	ITEM_PICKDROP_PLACE	Drop;
	Drop.pWnd = this;
	Drop.h = x;
	Drop.v = y;

	if (pToPickUpObj)
	{
		ITEM_PICKDROP_PLACE	Pick;
		Pick.pWnd = this;
		Pick.h = pToPickUpObj->DataX;
		Pick.v = pToPickUpObj->DataY;
		m_pParentWnd->WndProc(WND_N_ITEM_PICKDROP, (unsigned int)&Pick, (int)&Drop);
	}
	else
		m_pParentWnd->WndProc(WND_N_ITEM_PICKDROP, NULL, (int)&Drop);
}

void KWndObjectMatrix::EnableTracePutPos(bool bEnable)
{
	if (bEnable)
		m_Style |= OBJCONT_S_TRACE_PUT_POS;
	else
		m_Style &= ~OBJCONT_S_TRACE_PUT_POS;
}

void KWndObjectMatrix::EnablePickPut(bool bEnable)
{
	if (bEnable == false)
		m_Style |= OBJCONT_S_DISABLE_PICKPUT;
	else
		m_Style &= ~OBJCONT_S_DISABLE_PICKPUT;
}

int KWndObjectMatrix::GetGridInfo(int &nWidth, int &nHeight)
{
	nWidth  = m_nUnitWidth;
	nHeight = m_nUnitHeight;
	return m_nUnitBorder;
}
