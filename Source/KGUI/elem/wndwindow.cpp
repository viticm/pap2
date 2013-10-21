/*****************************************************************************************
//	剑侠引擎，界面窗口体系结构的最基本窗口对象
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-9
*****************************************************************************************/
#include "stdafx.h"
#include <shlwapi.h>
#include "./WndMessage.h"
#include "./wndwindow.h"
#include "./wndframe.h"
#include "./Components/decoder.h"
#include "./Components/itemhandle.h"
#include "./Components/item.h"
#include "./Components/itemeventmgr.h"
#include "./tipcenter.h"
#include "../common/ksmallobjectpool.h"
#include "../config/kdebugconfig.h"
#include "../driver/kcolor.h"
#include "../driver/kareatest.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"

#ifndef DISABLE_UI_EDIT
#include "./Components/itemeditmodemgr.h"  
#endif


namespace
{
    KSmallObjectPool g_Allocator("KWndWindow", sizeof(UI::KWndWindow));
}

namespace UI
{
    static bool CalcRelativePosition(float& fX, float& fY, int nRelSide, float fRelX, float fRelY, float fRelCX, float fRelCY)
    {
        fX = 0.0f;
        fY = 0.0f;

        switch (nRelSide)
        {
        case WND_SIDE_TOP:
            fY = fRelY;
            break;
        case WND_SIDE_BOTTOM:
            fY = fRelY + fRelCY;
            break;
        case WND_SIDE_LEFT:
            fX = fRelX;
            break;
        case WND_SIDE_RIGHT:
            fX = fRelX + fRelCX;
            break;
        case WND_SIDE_TOPLEFT:
            fX = fRelX;
            fY = fRelY;
            break;
        case WND_SIDE_TOPRIGHT:
            fX = fRelX + fRelCX;
            fY = fRelY;
            break;
        case WND_SIDE_BOTTOMLEFT:
            fX = fRelX;
            fY = fRelY + fRelCY;
            break;
        case WND_SIDE_BOTTOMRIGHT:
            fX = fRelX + fRelCX;
            fY = fRelY + fRelCY;
            break;
        case WND_SIDE_CENTER:
            fX = fRelX + fRelCX / 2.0f;
            fY = fRelY + fRelCY / 2.0f;
            break;
        case WND_SIDE_LEFTCENTER:
            fX = fRelX;
            fY = fRelY + fRelCY / 2.0f;
            break;
        case WND_SIDE_RIGHTCENTER:
            fX = fRelX + fRelCX;
            fY = fRelY + fRelCY / 2.0f;
            break;
        case WND_SIDE_TOPCENTER:
            fX = fRelX + fRelCX / 2.0f;
            fY = fRelY;
            break;
        case WND_SIDE_BOTTOMCENTER:
            fX = fRelX + fRelCX / 2.0f;
            fY = fRelY + fRelCY;
            break;
        default:
            return false;
        }

        return true;
    }

#ifdef KGUI_LUA_LOADER
    int KWndWindow::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
    {
    	int nResult  = false;
    	int nRetCode = false;
    
        KWndWindow *pWnd = NULL;

        KGLOG_PROCESS_ERROR(ppObj);

        pWnd = new(std::nothrow) KWndWindow;
        KGLOG_PROCESS_ERROR(pWnd);

        if (pLua)
        {
            nRetCode = pWnd->Init(pLua, nTableIndex);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        *ppObj = pWnd;
    // Exit1:
    	nResult = true;
    Exit0:
        if (!nResult)
        {
            KG_DELETE(pWnd);
        }
    	return nResult;
    }

    void KWndWindow::Release()
    {
        delete this;
    }
#endif


	KWndWindow::KWndWindow()
		: m_pEventMgr(NULL)
        , m_pItemHandle(NULL)
        , m_fRelX(0.0f)
        , m_fRelY(0.0f)
        , m_fAbsX(0.0f)
        , m_fAbsY(0.0f)
        , m_fWidth(0.0f)
        , m_fHeight(0.0f)
        , m_dwStyle(WND_S_VISIBLE | WND_S_DRAW_LIST_CHANGED)
        , m_dwAlpha(255)
        , m_pPrev(NULL)
        , m_pNext(NULL)
        , m_pFirstChild(NULL)
        , m_pParent(NULL)
        , m_nTipIndex(INVALID_TIP_INDEX)
        , m_nAreaIndex(-1)
        , m_nLuaRef(LUA_NOREF)
        , m_nLevel(0)
	{
        m_szName[0] = '\0';
    }

    //--------------------------------------------------------------------------
    //	功能：分构函数
    //--------------------------------------------------------------------------
    KWndWindow::~KWndWindow()
    {
    }

    void* KWndWindow::operator new(size_t /* nSize */, std::nothrow_t const&) throw()
    { 
        return g_Allocator.Alloc(); 
    }

    void KWndWindow::operator delete(void* pData, std::nothrow_t const&) throw()
    { 
        g_Allocator.Free(pData); 
    }

    void* KWndWindow::operator new(size_t /* nSize */)
    { 
        return g_Allocator.Alloc(); 
    }

    void KWndWindow::operator delete(void* pData) throw()
    { 
        g_Allocator.Free(pData); 
    }

#ifdef KGUI_LUA_LOADER
    // 窗口的初始化，所需的参数在pLua脚本环境中
    int KWndWindow::Init(Lua_State *pLua, int nTableIndex)
    {
    	int nResult  = false;
    	int nRetCode = false;
    
        float fLeft         = .0f;
        float fTop          = .0f;
        int   nDisableFlag  = false;
        int   nMoveableFlag = false;
        int   nDummyFlag    = false;
        int   nPenetrable   = false;
        int   nMouseHover   = false;
        int   nDisBringTop  = false;
        int   nShowTipType  = 0;
        int   nTipRichText  = 0;
        char  szTip[KD_NORMAL_BUFFER_SIZE];
        char  szFilePath[MAX_PATH];

        KLuaTableField aParamTable[] = 
        {
            KLUA_TABLE_FIELD("name",                emVAR_TYPE_STRING,  "",     m_szName,       _countof(m_szName)),
            KLUA_TABLE_FIELD("width",               emVAR_TYPE_FLOAT,   0,      &m_fWidth,      0),
            KLUA_TABLE_FIELD("height",              emVAR_TYPE_FLOAT,   0,      &m_fHeight,     0),
            KLUA_TABLE_FIELD("left",                emVAR_TYPE_FLOAT,   0,      &fLeft,         0),
            KLUA_TABLE_FIELD("top",                 emVAR_TYPE_FLOAT,   0,      &fTop,          0),
            KLUA_TABLE_FIELD("disable",             emVAR_TYPE_INT,     false,  &nDisableFlag,  0),
            KLUA_TABLE_FIELD("moveable",            emVAR_TYPE_INT,     false,  &nMoveableFlag, 0),
            // FollowMove字段删除
            // FollowSize字段删除
            KLUA_TABLE_FIELD("dummy",               emVAR_TYPE_INT,     false,  &nDummyFlag,    0),
            KLUA_TABLE_FIELD("penetrable",          emVAR_TYPE_INT,     false,  &nPenetrable,   0),
            KLUA_TABLE_FIELD("HoldMouseHover",      emVAR_TYPE_INT,     false,  &nMouseHover,   0),
            KLUA_TABLE_FIELD("DisableTop",          emVAR_TYPE_INT,     false,  &nDisBringTop,  0),
            KLUA_TABLE_FIELD("tip",                 emVAR_TYPE_STRING,  "",     szTip,          _countof(szTip)),
            KLUA_TABLE_FIELD("ShowTipType",         emVAR_TYPE_INT,     0,      &nShowTipType,  0),
            KLUA_TABLE_FIELD("TipRichText",         emVAR_TYPE_INT,     false,  &nTipRichText,  0),
            KLUA_TABLE_FIELD("alpha",               emVAR_TYPE_INT,     255,    &m_dwAlpha,     0),
            KLUA_TABLE_FIELD("AreaFile",            emVAR_TYPE_STRING,  "",     szFilePath,     _countof(szFilePath)),
        };

        KGLOG_PROCESS_ERROR(pLua);

        m_emObjType = em_OBJ_TYPE_WND;      // 指定类型

        for (int i = 0; i < _countof(aParamTable); i++)
        {
            nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        SetRelPos(fLeft, fTop);

        g_SetBitFlag(nDisableFlag, WND_S_DISABLE, m_dwStyle);
        g_SetBitFlag(nMoveableFlag, WND_S_MOVEABLE, m_dwStyle);
        g_SetBitFlag(nDummyFlag, WND_S_SIZE_WITH_ALL_CHILD, m_dwStyle);
        g_SetBitFlag(nPenetrable, WND_S_MOUSE_PENETRABLE, m_dwStyle);
        g_SetBitFlag(nMouseHover, WND_S_SELF_HOLD_MOUSE_HOVER, m_dwStyle);
        g_SetBitFlag(nDisBringTop, WND_S_DISABLE_BRING_TO_TOP, m_dwStyle);

        if (szTip[0])
        {
            int nStrIndex = g_pUI->m_GlobalStrValuableMgr.GetIndexAt(szTip);
            m_nTipIndex = KTipCenter::GetSelf().Append(nStrIndex, nShowTipType, nTipRichText);
        }

        if (szFilePath[0])
        {
            FormatFilePath(szFilePath);

            m_nAreaIndex = g_AreaTester.GetID(szFilePath);
        }
        
    // Exit1:
    	nResult = true;
    Exit0:
        if (!nResult)
        {
            KGLogPrintf(KGLOG_ERR, "KGUI %s init failed!\n", m_szName);
        }
    	return nResult;
    }

    int KWndWindow::AddChild(UI::IKUIObject *pChild)
    {
    	int nResult  = false;
    	int nRetCode = false;

        KGLOG_PROCESS_ERROR(pChild);

        switch(pChild->m_emObjType)
        {
        case em_OBJ_TYPE_HANDLE:
            {
                float        fHandleRelX = .0f;
                float        fHandleRelY = .0f;
                KItemHandle *pHandle     = (KItemHandle *)pChild;

                nRetCode = SetItemHandle(pHandle);
                KGLOG_PROCESS_ERROR(nRetCode);

                nRetCode = pHandle->GetRelPos(fHandleRelX, fHandleRelY);
                KGLOG_PROCESS_ERROR(nRetCode);

                nRetCode = pHandle->SetAbsPos(m_fAbsX + fHandleRelX, m_fAbsY + fHandleRelY);
                KGLOG_PROCESS_ERROR(nRetCode);
                
                UpdateDrawList();
                UpdateEvent();
            }
            break;

        case em_OBJ_TYPE_WND:
        case em_OBJ_TYPE_FRAME:
            nRetCode = AddChild((KWndWindow *)pChild);
            KGLOG_PROCESS_ERROR(nRetCode);
            break;
        default:
            ASSERT(false && "类型错误");
            KGLOG_PROCESS_ERROR(false);
            break;
        }
    // Exit1:
    	nResult = true;
    Exit0:
    	return nResult;
    }
#endif

	void KWndWindow::SetTip(int nTipIndex, int nShowTipType, int nRichText)
	{
		if (m_nTipIndex != INVALID_TIP_INDEX)
			KTipCenter::GetSelf().Remove(m_nTipIndex);
		m_nTipIndex = KTipCenter::GetSelf().Append(nTipIndex, nShowTipType, nRichText);
	}


	void KWndWindow::SetAreaTestFile(LPCSTR szFilePath)
	{
		m_nAreaIndex = g_AreaTester.GetID(szFilePath);
	}


	int KWndWindow::CreateItemHandle(IIniFile* pIni, LPCSTR pszSectionName)
    {
        int nResult = false;
        int nRetCode = false;
        char szHandle[WND_SECTION_NAME_LEN];
        char szBuffer[WND_SECTION_NAME_LEN];

        szHandle[0] = '\0';
        szBuffer[0] = '\0';

        ASSERT(pIni);
        ASSERT(pszSectionName);
        ASSERT(pszSectionName[0] != '\0');

        while (pIni->GetNextSection(szHandle, szBuffer))
        {
            strncpy(szHandle, szBuffer, _countof(szHandle));
            szHandle[_countof(szHandle) - 1] = '\0';
            pIni->GetString(szHandle, "._WndType", "", szBuffer, _countof(szBuffer));
            if (stricmp(szBuffer, "Handle") != 0)
                continue;
            pIni->GetString(szHandle, "._Parent", "", szBuffer, _countof(szBuffer));
            if (stricmp(szBuffer, pszSectionName) != 0)
                continue;

            float fRelWidth, fRelHeight;
            float fScale = g_pUI->m_WndStation.GetUIScale();
            KWndWindow *pParent = this;
            while(pParent)
            {
                if (pParent->GetWndType() == "WndTexture")
                {
                    fScale = 1.0f;
                    break;
                }
                pParent = pParent->GetParent();
            }
            KItemHandle *pHandle = NULL;
            nRetCode = KUiComponentsDecoder::GetSelf().CreateItemHandle(
                &pHandle, pIni, szHandle, this, fScale);
            SetItemHandle(pHandle);
            KG_PROCESS_ERROR(nRetCode);
            ASSERT(m_pItemHandle);
            m_pItemHandle->GetRelPos(fRelWidth, fRelHeight);
            m_pItemHandle->SetAbsPos(m_fAbsX + fRelWidth, m_fAbsY + fRelHeight);

            break;
        }

        nResult = true;
Exit0:
        if (!nResult)
            SAFE_RELEASE(m_pItemHandle);
        return nResult;
    }


    //--------------------------------------------------------------------------
    //	功能：把窗口移动到最前面
    //--------------------------------------------------------------------------
    void KWndWindow::BringToTop()
    {
        if (IsStyle(WND_S_DISABLE_BRING_TO_TOP))
            return;
        if (!m_pNext)
            return;
        KWndWindow*	pLast = m_pNext;
        while(pLast->m_pNext)
            pLast = pLast->m_pNext;

        pLast->m_pNext = this;
        m_pNext->m_pPrev = m_pPrev;
        if (m_pPrev)
            m_pPrev->m_pNext = m_pNext;
        else if (m_pParent && m_pParent->m_pFirstChild == this)
            m_pParent->m_pFirstChild = m_pNext;
        m_pNext = NULL;
        m_pPrev = pLast;

    }

	float KWndWindow::GetRelX() const
	{
		return m_fRelX;
	}

	float KWndWindow::GetRelY() const
	{
		return m_fRelY;
	}

	float KWndWindow::GetAbsX() const
	{
		return m_fAbsX;
	}

	float KWndWindow::GetAbsY() const
	{
		return m_fAbsY;
	}

    void KWndWindow::SetRelPos(float fX, float fY)
    {
        AbsMove(fX - m_fRelX, fY - m_fRelY);

        m_fRelX = fX;
        m_fRelY = fY;
    }

    void KWndWindow::SetAbsPos(float fX, float fY)
    {
		float fOffsetX = fX - m_fAbsX;
		float fOffsetY = fY - m_fAbsY;

		AbsMove(fOffsetX, fOffsetY);

		m_fRelX += fOffsetX;
		m_fRelY += fOffsetY;
	}

    void KWndWindow::GetSize(float* pWidth, float* pHeight) const
    {
        if (pWidth)
            *pWidth = m_fWidth;
        if (pHeight)
            *pHeight = m_fHeight;
    }

    //--------------------------------------------------------------------------
    //	功能：设置窗口大小
    //--------------------------------------------------------------------------
    void KWndWindow::SetSize(float fWidth, float fHeight)
    {
        if (fWidth < 0)
            fWidth = 0;
        if (fHeight < 0)
            fHeight = 0;
        float fDX = fWidth - m_fWidth;
        float fDY = fHeight - m_fHeight;

        if (abs(fDX) <= FLT_EPSILON && abs(fDY) <= FLT_EPSILON)
            return;

        m_fWidth = fWidth;
        m_fHeight = fHeight;

        OnSize();

        //to be check.!!!!!
        //一些窗口类可能会有根据窗口大小计算出一些变量保存下来供后继运算使用，
        //目前没有提供大小改变的通知消息，可能那些窗口的行为会有未知的结果。
        //但是目前情况来看那样的窗口一般不会被调用SetSize。
    }


    //--------------------------------------------------------------------------
    //	功能：绝对坐标的调整
    //--------------------------------------------------------------------------
    void KWndWindow::AbsMove(float fXOffset, float fYOffset)
    {
        if (abs(fXOffset) <= FLT_EPSILON && abs(fYOffset) <= FLT_EPSILON)
            return;

        m_fAbsX += fXOffset;
        m_fAbsY += fYOffset;

        OnMove();

        for (KWndWindow* pWnd = GetFirstChild(); pWnd; pWnd = pWnd->GetNext())
            pWnd->AbsMove(fXOffset, fYOffset);

        if (m_pItemHandle)
            m_pItemHandle->AbsMove(fXOffset, fYOffset);
    }


    void KWndWindow::Show()
    {
		SetStyle(WND_S_VISIBLE, TRUE);

        OnShow();
    }


    void KWndWindow::Hide()
    {
        if (IsVisible())
		{
			SetStyle(WND_S_VISIBLE, FALSE);

			OnHide();

			g_pUI->m_WndStation.SetBlurWindow(this);
		}
    }

    void KWndWindow::OnShow()
    {
    }

    void KWndWindow::OnHide()
    {
    }

    void KWndWindow::OnSize()
    {
    }

    void KWndWindow::OnScale(float fScaleX, float fScaleY)
    {
        SetSize(m_fWidth * fScaleX, m_fHeight * fScaleY);
    }


    //--------------------------------------------------------------------------
    //	功能：判断窗口是否被显示
    //--------------------------------------------------------------------------
    BOOL KWndWindow::IsVisible() const
    {
        return IsStyle(WND_S_VISIBLE);
    }

    BOOL KWndWindow::IsDisable() const 
    { 
        return IsStyle(WND_S_DISABLE); 
    }

    //--------------------------------------------------------------------------
    //	功能：禁止或者允许使窗口被操作
    //--------------------------------------------------------------------------
    void KWndWindow::Enable(int bEnable)
    {
		SetStyle(WND_S_DISABLE, !bEnable);
    }

    void KWndWindow::Destroy(int nDestroyMyBrother)
    {

        if (m_pFirstChild)
            m_pFirstChild->Destroy(WND_DESTROY_MY_BROTHER);
        if (m_pNext && nDestroyMyBrother == WND_DESTROY_MY_BROTHER)
            m_pNext->Destroy(WND_DESTROY_MY_BROTHER);

		OnDestroy();
    }

    inline BOOL KWndWindow::PtInThisWindow(float x, float y) const
    {	
        return x >= m_fAbsX && y >= m_fAbsY && x < m_fAbsX + m_fWidth && y < m_fAbsY + m_fHeight;
    }


    //--------------------------------------------------------------------------
    //	功能：判断一个点是否落在窗口内,传入的是绝对坐标
    //--------------------------------------------------------------------------

    int KWndWindow::PtInWindow(int x, int y)
    {
#ifndef DISABLE_UI_EDIT
        if (KDebugConfig::ms_aBoolFlag[KDebugConfig::BOOL_EDIT_MODE])
        {
            if (!IsVisible())
                return false;

            if (PtInThisWindow((float)x, (float)y))
                return true;

            for (KWndWindow *pChild = m_pFirstChild; pChild; pChild = pChild->m_pNext)
            {
                if (pChild->PtInWindow(x, y))
                    return true;
            }
            if (m_pItemHandle)
                return m_pItemHandle->PtInMeOrMyChild(float(x), float(y));
            return false;
        }
#endif // #ifndef DISABLE_UI_EDIT

        if (!IsStyle(WND_S_VISIBLE))
            return false;
        if (IsStyle(WND_S_MOUSE_PENETRABLE))
            return false;
        if (IsStyle(WND_S_SELF_HOLD_MOUSE_HOVER) &&
            g_pUI->m_WndStation.GetMessage() == WM_MOUSEHOVER &&
            PtInThisWindow((float)x, (float)y))
        {
            return true;
        }

        if (IsStyle(WND_S_SIZE_WITH_ALL_CHILD))
        {
            for (KWndWindow *pChild = m_pFirstChild; pChild; pChild = pChild->m_pNext)
            {
                if (pChild->PtInWindow(x, y))
                    return true;
            }
            EventCheck();
            if (m_pEventMgr && m_pEventMgr->PtInEventItem((float)x, (float)y))
                return true;
            return false;
        }

        if (m_nAreaIndex != -1)
        {
            if (UI_FLOAT_LARGE(m_fWidth, 0.0f) && UI_FLOAT_LARGE(m_fHeight, 0.0f))
                return g_AreaTester.PtInArea(m_nAreaIndex, (x - m_fAbsX) / m_fWidth,  (y - m_fAbsY) / m_fHeight);
            return false;
        }

        return PtInThisWindow((float)x, (float)y);
    }


    //--------------------------------------------------------------------------
    //	功能：绘制窗口（包括子窗口与后继兄弟窗口）
    //--------------------------------------------------------------------------
    void KWndWindow::Paint()
    {
        if (IsVisible())
            OnPaint();
        if (m_pNext)
            m_pNext->Paint();
    }

    //--------------------------------------------------------------------------
    //	功能：让窗口活动
    //--------------------------------------------------------------------------
    void KWndWindow::Breathe()
    {
        if (m_pNext)
            m_pNext->Breathe();

		if (IsDisable())
			return;

		if (IsVisible() || IsStyle(WND_S_BREAHTE_WHEN_HIDE))
		{
			OnBreathe();
			if (m_pFirstChild) 
				m_pFirstChild->Breathe();
		}
    }

    //--------------------------------------------------------------------------
    //	功能：添加子窗口
    //--------------------------------------------------------------------------
    int KWndWindow::AddChild(KWndWindow* pChild)
    {
        int nRetCode = false;
        int nResult = false;

        KG_PROCESS_ERROR(pChild);

        pChild->SplitSmallFamily();

        //调整绝对坐标
        pChild->AbsMove(
            m_fAbsX + pChild->m_fRelX - pChild->m_fAbsX,
            m_fAbsY + pChild->m_fRelY - pChild->m_fAbsY);

        //调整级连关系
        pChild->m_pParent = this;
        if (!m_pFirstChild)
        {
            m_pFirstChild = pChild;
        }
        else
        {
            KWndWindow* pBrother = m_pFirstChild;
            while(pBrother->m_pNext)
                pBrother = pBrother->m_pNext;
            pBrother->m_pNext = pChild;
            pChild->m_pPrev = pBrother;
        }

        nRetCode = pChild->UpdateLevel(m_nLevel + 1);
        KGLOG_PROCESS_ERROR(nRetCode);

        nResult = true;
Exit0:
        return nResult;
    }

    //--------------------------------------------------------------------------
    //	功能：添加子窗口
    //--------------------------------------------------------------------------
    int KWndWindow::AddBrother(KWndWindow* pBrother, int nJustBehindMe /*= false*/)
    {
        int nRetCode = false;
        int nResult = false;
        KWndWindow* pWnd = m_pNext;

        KG_PROCESS_ERROR(pBrother);
        KG_PROCESS_ERROR(pBrother != this);

        pBrother->SplitSmallFamily();

        //调整绝对坐标
        if (m_pParent)
        {
            float const fX = m_pParent->m_fAbsX + pBrother->m_fRelX - pBrother->m_fAbsX;
            float const fY = m_pParent->m_fAbsY + pBrother->m_fRelY - pBrother->m_fAbsY;

            pBrother->AbsMove(fX, fY);
        }

        //调整级连关系
        pBrother->m_pParent = m_pParent;
        if (m_pNext == NULL)
        {
            m_pNext = pBrother;
            pBrother->m_pPrev = this;
        } 
        else if (nJustBehindMe)
        {
            pBrother->m_pPrev = this;
            pBrother->m_pNext = m_pNext;
            m_pNext->m_pPrev = pBrother;
            m_pNext = pBrother;
        }
        else
        {
            while (pWnd->m_pNext)
                pWnd = pWnd->m_pNext;

            pWnd->m_pNext = pBrother;
            pBrother->m_pPrev = pWnd;
        }

        nRetCode = pBrother->UpdateLevel(m_nLevel);
        KGLOG_PROCESS_ERROR(nRetCode);

        nResult = true;
Exit0:
        return nResult;
    }

    //--------------------------------------------------------------------------
    //	功能：世间再无窗在我左右，一无牵连
    //--------------------------------------------------------------------------
    void KWndWindow::LeaveAlone()
    {
        SplitSmallFamily();
        while (m_pFirstChild)
        {
            m_pFirstChild->m_pParent = NULL;
            m_pFirstChild = m_pFirstChild->m_pNext;
        }
    }

    //--------------------------------------------------------------------------
    //	功能：（抛父弃兄唯留子，成立小家庭），把自己（及子窗口）从窗口树里面里面分离出来
    //--------------------------------------------------------------------------
    void KWndWindow::SplitSmallFamily()
    {
        if (m_pPrev)
            m_pPrev->m_pNext = m_pNext;
        else if (m_pParent)
            m_pParent->m_pFirstChild = m_pNext;
        if (m_pNext)
            m_pNext->m_pPrev = m_pPrev;
        m_pPrev = NULL;
        m_pNext = NULL;
        m_pParent = NULL;
    }

    inline void KWndWindow::EventCheck()
    {
        if (IsStyle(WND_S_EVENT_LIST_CHANGED))
        {
            if (m_pEventMgr)
            {
                if(!m_pEventMgr->ReBuild(this))
                {
                    SAFE_RELEASE(m_pEventMgr);
                }
            }
            else
            {
                m_pEventMgr = KItemEventMgr::Create(this);
            }
            SetStyle(WND_S_EVENT_LIST_CHANGED, FALSE);
        }
    }

    //--------------------------------------------------------------------------
    //	功能：窗口函数（处理消息）
    //--------------------------------------------------------------------------
    LRESULT KWndWindow::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        int nResult = FALSE;

#ifndef DISABLE_UI_EDIT
        if (KDebugConfig::ms_aBoolFlag[UI::KDebugConfig::BOOL_EDIT_MODE])
            EditModeProc(uMsg, wParam, lParam);
#endif

        EventCheck();
        if (m_pEventMgr && m_pEventMgr->EventItemProc(uMsg, wParam, lParam))
            return TRUE;

        switch (uMsg)
        {
        case WND_M_MOUSE_ENTER:
            if (m_nTipIndex >= 0)
                KTipCenter::GetSelf().ShowTip(m_nTipIndex, m_fAbsX, m_fAbsY, m_fWidth, m_fHeight);
            g_pUI->m_Script.FireWndEvent(this, "OnMouseEnter");
            break;
		case WND_M_REFRESH_TIP:
			g_pUI->m_Script.FireWndEvent(this, "OnRefreshTip");
			break;
        case WND_M_MOUSE_LEAVE:
            if (m_nTipIndex >= 0)
                KTipCenter::GetSelf().HideTip();
            g_pUI->m_Script.FireWndEvent(this, "OnMouseLeave");
            break;
        case WM_MOUSEHOVER:
            g_pUI->m_Script.FireWndEvent(this, "OnMouseHover", &nResult);
			return nResult;
        case WM_LBUTTONDOWN:
        	g_pUI->m_Script.FireWndEvent(this, "OnLButtonDown", &nResult);
            return nResult;
        case WM_RBUTTONDOWN:
            g_pUI->m_Script.FireWndEvent(this, "OnRButtonDown", &nResult);
            return nResult;
        case WM_LBUTTONUP:
        	g_pUI->m_Script.FireWndEvent(this, "OnLButtonUp", &nResult);
            return nResult;
        case WM_RBUTTONUP:
        	g_pUI->m_Script.FireWndEvent(this, "OnRButtonUp", &nResult);
            return nResult;
		case WM_MOUSEWHEEL:
			g_pUI->m_Script.FireWndEvent(this, "OnMouseWheel", &nResult);
			return nResult;
        }

        return FALSE;
    }

#ifndef DISABLE_UI_EDIT
    LRESULT KWndWindow::EditModeProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
    {
        if (uMsg == WM_MOUSEMOVE || uMsg == WM_LBUTTONDOWN || uMsg == WM_LBUTTONUP)
        {
            static float fDragStartX = 0.0f;
            static float fDragStartY = 0.0f;
            static int nIsDragMe = false;
            static int nIsResizeMe = false;
            LRESULT lDragRet = false;

            if ((uMsg == WM_LBUTTONDOWN || (uMsg == WM_MOUSEMOVE && !(wParam & MK_LBUTTON))) && m_pItemHandle)
            {
                lDragRet = m_pItemHandle->WndProc(uMsg, wParam, lParam);
                if (!lDragRet)
                    KItemEditModeMgr::GetSelf().ClearCurrentEditItem();
            }
            else
            {
                lDragRet = KItemEditModeMgr::GetSelf().WndProc(uMsg, wParam, lParam);
            }

            if (!lDragRet)
            {
                switch(uMsg)
                {
                case WM_LBUTTONDOWN:
                    if (KDebugConfig::ms_aBoolFlag[UI::KDebugConfig::BOOL_SELECT_WINDOW_IN_CLIENT])
                    {
                        g_pUI->m_EditorMgr.UIEditorOperation(PLUGIN_UI_EDITOR_SEL_WND, (WPARAM)m_szName, 0);
                    }
                    fDragStartX = (float)GET_X_LPARAM(lParam);
                    fDragStartY = (float)GET_Y_LPARAM(lParam);
                    g_pUI->m_WndStation.SetCapture(this);
                    nIsDragMe = true;
                    nIsResizeMe = false;
                    if (
                        (fDragStartX > m_fAbsX + m_fWidth - 2 * KDebugConfig::ms_nResizeWindowHandleNodeHalfSize) && 
                        (fDragStartY > m_fAbsY + m_fHeight - 2 * KDebugConfig::ms_nResizeWindowHandleNodeHalfSize)
                        )
                    {
                        nIsResizeMe = true;
                        nIsDragMe = false;
                    }
                    break;
                case WM_LBUTTONUP:
                    if (g_pUI->m_WndStation.GetCapture() == this)
                        g_pUI->m_WndStation.ReleaseCapture();
                    nIsDragMe = false;
                    nIsResizeMe = false;
                    break;
                case WM_MOUSEMOVE:
                    if ((wParam & MK_LBUTTON) && (nIsDragMe || nIsResizeMe))
                    {
                        float fPosX = (float)GET_X_LPARAM(lParam);
                        float fPosY = (float)GET_Y_LPARAM(lParam);
                        if (nIsDragMe)
                            AbsMove(fPosX - fDragStartX, fPosY - fDragStartY);
                        else
                            SetSize(m_fWidth + (fPosX - fDragStartX), m_fHeight + (fPosY - fDragStartY));
                        fDragStartX = fPosX;
                        fDragStartY = fPosY;
                    }
                    else
                    {
                        static WCHAR wszInfo[MAX_PATH] = { 0 };

                        _snwprintf(wszInfo, _countof(wszInfo), L"名字: %s\n   x=%d, y=%d, w=%d, h=%d\n", 
							ATL::CA2W(m_szName, GetCodePage()), 
                            (int)m_fRelX, (int)m_fRelY, (int)m_fWidth, (int)m_fHeight);
                        g_pUI->m_EditorMgr.UIEditorOperation(PLUGIN_UI_EDITOR_SHOW_INFO, (WPARAM)wszInfo, 0);
                    }
                    break;
                }
            }
        }
        return FALSE;
    }

#endif //ifndef DISABLE_UI_EDIT

    int KWndWindow::Scale(float fScaleX, float fScaleY)
    {
		ASSERT(fScaleX > 0.0f && fScaleY > 0.0f);
        if (GetWndType() == "WndTexture")
            return true;

        for (KWndWindow* pChild = m_pFirstChild; pChild; pChild = pChild->m_pNext)
        {
            pChild->SetRelPos(pChild->GetRelX() * fScaleX, pChild->GetRelY() * fScaleY);
            pChild->Scale(fScaleX, fScaleY);
        }

        if (m_pItemHandle)
        {
            float fRelX = 0.0f;
            float fRelY = 0.0f;

            m_pItemHandle->Scale(fScaleX, fScaleY);
            m_pItemHandle->GetRelPos(fRelX, fRelY);
            m_pItemHandle->SetAbsPos(m_fAbsX + fRelX, m_fAbsY + fRelY);
        }

        OnScale(fScaleX, fScaleY);

        return true;
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
            while (pWnd)
            {
                //一系列同级的兄弟窗口，要从最上面（链表最末端的）开始判断
                while (pWnd->m_pNext)
                    pWnd = pWnd->m_pNext;
                while (pWnd)
                {
                    if (pWnd->PtInWindow(x, y))
                    {
#ifndef DISABLE_UI_EDIT
                        if (KDebugConfig::ms_aBoolFlag[UI::KDebugConfig::BOOL_RESIZE_WND_AND_ITEM_MODE])
                        {
                            pLastMatch = pWnd;
                            pWnd = pLastMatch->m_pFirstChild;
                            break;
                        }
#endif // #ifndef DISABLE_UI_EDIT

                        if (pWnd->IsDisable())
                        {
                            pWnd = NULL;
                            break;
                        }

                        pLastMatch = pWnd;
                        pWnd = pLastMatch->m_pFirstChild;
                        break;
                    }
                    pWnd = pWnd->m_pPrev;
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
        while(pWnd->m_pParent)
            pWnd = pWnd->m_pParent;
        return pWnd;
    }

    void KWndWindow::PaintDebugInfo()
    {
    }

    inline void KWndWindow::DrawCheck()
    {
        if (IsStyle(WND_S_DRAW_LIST_CHANGED))
        {
            m_aDrawItem.clear();

            if(m_pItemHandle) 
                m_pItemHandle->PushAllDrawItem(m_aDrawItem);

            SetStyle(WND_S_DRAW_LIST_CHANGED, FALSE);
        }
    }

    //--------------------------------------------------------------------------
    //	功能：窗体绘制
    //--------------------------------------------------------------------------
    void KWndWindow::OnPaint()
    {
#ifndef DISABLE_UI_EDIT
        if (KDebugConfig::ms_aBoolFlag[UI::KDebugConfig::BOOL_SHOW_FRAME])
        {
            float const fNodeHalfSize = KDebugConfig::ms_aBoolFlag[UI::KDebugConfig::BOOL_RESIZE_WND_AND_ITEM_MODE] 
            ? static_cast<float>(KDebugConfig::ms_nResizeWindowHandleNodeHalfSize) 
                : 0.0f;

            KG3DUIRect2 aRect[2];

            float fLeft = m_fAbsX;
            float fTop = m_fAbsY;
            float fRight = m_fAbsX + m_fWidth;
            float fBottom = m_fAbsY + m_fHeight;

            DWORD dwColor = g_pUI->m_WndStation.GetFocusWindow() == this ? 0xFFFFFF00 : 0xFFFFFFFF;

            if (IsStyle(WND_S_SIZE_WITH_ALL_CHILD))
            {
                dwColor = 0xFFFF0000;

                GetAllChildLayoutRect(&fLeft, &fTop, &fRight, &fBottom);
            }

            for (int nIndex = 0; nIndex < _countof(aRect); ++nIndex)
                aRect[nIndex].crDiffuse = dwColor;

            fLeft -= 1.0f;
            fTop -= 1.0f;

            // 窗口包围框
            aRect[0].v2Min.x = fLeft;
            aRect[0].v2Min.y = fTop;
            aRect[0].v2Max.x = fRight;
            aRect[0].v2Max.y = fBottom;

            // 右下角
            aRect[1].v2Min.x = fRight - fNodeHalfSize * 2;
            aRect[1].v2Min.y = fBottom - fNodeHalfSize * 2;
            aRect[1].v2Max.x = fRight;
            aRect[1].v2Max.y = fBottom;
            aRect[1].crDiffuse = 0xFFFF0000;

            g_pUI->m_p3DUI->DrawMultiRect(&aRect[0], _countof(aRect));
        }

        if (KDebugConfig::ms_aBoolFlag[UI::KDebugConfig::BOOL_SHOW_NAME])
        {
            KG3DUIText2 text;

            text.v2Min = D3DXVECTOR2(m_fAbsX + m_fWidth / 2.0f, m_fAbsY + m_fHeight / 2.0f);
            text.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
            text.crFont = 0xFFFFFFFF;
            text.fScale = 1.0f;
            text.crBorder = 0x00000000;
            text.crShadow = 0x00000000;
            text.dwFontID = 0;
            text.fSpacing = 0.0f;
            text.byBorder = 0;
            text.byShadow = 0;
            text.nTextLength = -1;

            ATL::CA2W wszSectionName(m_szName, GetCodePage());
            _snwprintf(text.wszText, _countof(text.wszText), L"%s\n", wszSectionName);

            text.wszText[_countof(text.wszText) - 1] = L'\0';
            text.byAlpha = 0xFF;

            g_pUI->m_p3DUI->DrawMultiText(&text, 1);
        }
#endif

        DrawCheck();

        KItemList::iterator it = m_aDrawItem.begin();
        KItemList::iterator ie = m_aDrawItem.end();
        for (; it != ie; ++it)
        {
            KItemNull* pItem = *it;
            pItem->Draw();
        }

        if (m_pFirstChild)
        {
            m_pFirstChild->Paint();
        }
    }

    //使鼠标指针以移动到悬浮在此窗口中的位置上
    void KWndWindow::SetCursorAbove()
    {
        g_pUI->m_Pointer.SetCursorPos(
            static_cast<int>(m_fAbsX + m_fWidth / 2.0f),
            static_cast<int>(m_fAbsY + m_fHeight / 2.0f));
    }

    //取得包含所有子窗口分布区域的最小区域
    void KWndWindow::GetAllChildLayoutRect(float *pfLeft, float* pfTop, float* pfRight, float* pfBottom) const
    {
        if (m_pFirstChild == NULL)
        {
            if (pfLeft)
                *pfLeft = m_fAbsX;
            if (pfRight)
                *pfRight = m_fAbsX;
            if (pfTop)
                *pfTop = m_fAbsY;
            if (pfBottom)
                *pfBottom = m_fAbsY;
        }
        else
        {
            if (pfLeft)
                *pfLeft = FLT_MAX;
            if (pfTop)
                *pfTop = FLT_MAX;
            if (pfRight)
                *pfRight = FLT_MIN;
            if (pfBottom)
                *pfBottom = FLT_MIN;

            for (KWndWindow* pChild = m_pFirstChild; pChild; pChild = pChild->m_pNext)
            {
                float fChildLeft = 0.0f;
                float fChildTop = 0.0f;
                float fChildRight = 0.0f;
                float fChildBottom = 0.0f;

                pChild->GetAllChildLayoutRect(
                    &fChildLeft, &fChildTop, &fChildRight, &fChildBottom);

                float fX = pChild->GetAbsX();
                float fY = pChild->GetAbsY();
                float fWidth = 0.0f;
                float fHeight = 0.0f;

                pChild->GetSize(&fWidth, &fHeight);

                if (fChildLeft > fX)
                    fChildLeft = fX;
                if (fChildTop > fY)
                    fChildTop = fY;
                if (fChildRight < fX + fWidth)
                    fChildRight = fX + fWidth;
                if (fChildBottom < fY + fHeight)
                    fChildBottom = fY + fHeight;

                if (pfLeft && *pfLeft > fChildLeft)
                    *pfLeft = fChildLeft;

                if (pfTop && *pfTop > fChildTop)
                    *pfTop = fChildTop;

                if (pfRight && *pfRight < fChildRight)
                    *pfRight = fChildRight;

                if (pfBottom && *pfBottom < fChildBottom)
                    *pfBottom = fChildBottom;
            }
        }
    }

    int KWndWindow::CanCastTo(LPCSTR szType) const
    {
		return szType == "WndWindow";
    }

    void KWndWindow::OnBreathe()
    {
    }

    void KWndWindow::SetStyle(DWORD dwStyle, BOOL bEnable)
    {
		if (bEnable)
			m_dwStyle |= dwStyle;
		else
			m_dwStyle &= ~dwStyle;
    }

    void KWndWindow::OnDestroy()
    {
		if (IsStyle(WND_S_TOBEDESTROY))
			return;
		SetStyle(WND_S_TOBEDESTROY, TRUE);

		Hide();
		KWndFrame* pFrame = (KWndFrame*)CastWndWindow(this, "WndFrame");
		if (pFrame)
			pFrame->FireDestroy();

        g_pUI->m_WndStation.SetBlurWindow(this);
        if (m_nTipIndex >= 0)
            KTipCenter::GetSelf().Remove(m_nTipIndex);

        g_pUI->m_Script.UnRefUIObject(m_nLuaRef);
        m_nLuaRef = LUA_NOREF;

		SAFE_RELEASE(m_pEventMgr);
		SAFE_RELEASE(m_pItemHandle);

		LeaveAlone();
		g_pUI->m_WndStation.DestoryWindow(this);
    }


    int KWndWindow::ReleaseItemHandle()
    {
        SAFE_RELEASE(m_pItemHandle);
        return true;
    }


    int KWndWindow::SetItemHandle(KItemHandle *pItemHandle)
    {
        int nResult = false;

        KG_PROCESS_ERROR(pItemHandle);
        //如果m_pItemhandle存在，请先释放。否则会造成内存泄漏
        ASSERT(!m_pItemHandle);

        m_pItemHandle = pItemHandle;
        m_pItemHandle->SetOwner(this);

        nResult = true;
Exit0:
        return nResult;
    }

    int	KWndWindow::SetAlpha(int nAlpha)
    {
        for (KWndWindow *pWnd = m_pFirstChild; pWnd != NULL; pWnd = pWnd->GetNext())
            pWnd->SetAlpha(nAlpha);
        if (m_pItemHandle)
            m_pItemHandle->SetAlpha(nAlpha);

        SetSelfAlpha(nAlpha);

        return true;
    }

    int	KWndWindow::SetSelfAlpha(int nAlpha)
    {
        if (nAlpha > 255)
            nAlpha = 255;
        if (nAlpha < 0)
            nAlpha = 0;
        m_dwAlpha = (DWORD)nAlpha;

        return true;
    }

    int KWndWindow::GetAlpha()
    {
        return (int)m_dwAlpha;
    }

    void KWndWindow::OnMove()
    {
    }

    LPCSTR KWndWindow::GetWndType() const
    {
        return "WndWindow";
    }

    void KWndWindow::OnItemRelease(KItemNull *pItem)
    {
        UpdateEvent();

        if (m_pEventMgr)
            m_pEventMgr->ForceLeave(pItem);
    }

    bool KWndWindow::SetPoint(float fX, float fY)
    {
        SetRelPos(fX, fY);
        return true;
    }


    bool KWndWindow::SetPoint(int nSide, float fX, float fY, KWndWindow const* pRelWnd, int nRelSide, float fRelOffsetX, float fRelOffsetY)
    {
        bool bRetCode = false;

        float fAdjustX = 0.0f;
        float fAdjustY = 0.0f;
        float fWndX = 0.0f;
        float fWndY = 0.0f;
        float fWndCX = 0.0f;
        float fWndCY = 0.0f;
        float fRelAdjustX = 0.0f;
        float fRelAdjustY = 0.0f;
        float fRelWndX = 0.0f;
        float fRelWndY = 0.0f;
        float fRelWndCX = 0.0f;
        float fRelWndCY = 0.0f;

        // Calc relative window offset
        if (pRelWnd)
        {
			fRelWndX = pRelWnd->GetAbsX();
            fRelWndY = pRelWnd->GetAbsY();

            pRelWnd->GetSize(&fRelWndCX, &fRelWndCY);
        }
        else	// Relative to root
        {
            fRelWndCX = g_pUI->m_WndStation.GetClientWidth();
            fRelWndCY = g_pUI->m_WndStation.GetClientHeight();
        }

        bRetCode = CalcRelativePosition(fRelAdjustX, fRelAdjustY, nRelSide, fRelWndX, fRelWndY, fRelWndCX, fRelWndCY);
        KGLOG_PROCESS_ERROR(bRetCode);


        // Calc this frame offset
        GetSize(&fWndCX, &fWndCY);

        bRetCode = CalcRelativePosition(fAdjustX, fAdjustY, nSide, 0.0f, 0.0f, fWndCX, fWndCY);
        KGLOG_PROCESS_ERROR(bRetCode);

        SetRelPos(fX - fAdjustX + fRelAdjustX + fRelOffsetX, fY - fAdjustY + fRelAdjustY + fRelOffsetY);
        return true;
Exit0:
        return false;
    }

    int	KWndWindow::GetSide(LPCSTR szSide)
    {
        LPCSTR s_szSides[UI::WND_SIDE_END] = 
        {
            "TOP",
            "BOTTOM",
            "LEFT",
            "RIGHT",
            "TOPLEFT",
            "TOPRIGHT",
            "BOTTOMLEFT",
            "BOTTOMRIGHT",
            "CENTER",
            "LEFTCENTER",
            "RIGHTCENTER",
            "TOPCENTER",
            "BOTTOMCENTER",
        };

        BOOL bRetCode = FALSE;

        ASSERT(szSide);

        bRetCode = szSide[0] != '\0';
        KGLOG_PROCESS_ERROR(bRetCode);

        for (int nIndex = WND_SIDE_BEGIN; nIndex != WND_SIDE_END; ++nIndex)
        {
            if (stricmp(s_szSides[nIndex], szSide) == 0)
            {
                return nIndex;
            }
        }

Exit0:
        return -1;
    }

	void KWndWindow::SetName(LPCSTR szName)
	{
		KGLOG_PROCESS_ERROR(szName);
		KGLOG_PROCESS_ERROR(strlen(szName) < _countof(m_szName));

		strncpy(m_szName, szName, _countof(m_szName));
		return;
Exit0:
		m_szName[0] = '\0';
		return;
	}

	LPCSTR KWndWindow::GetName() const
	{ 
		return m_szName; 
	}

    int KWndWindow::UpdateLevel(int nLevel)
    {
        int nRetCode = false;
        int nResult = false;

        KG_PROCESS_SUCCESS(m_nLevel == nLevel);

        m_nLevel = nLevel;

        if (m_pItemHandle)
        {
            nRetCode = m_pItemHandle->UpdateLevel(0);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        if (m_pNext)
        {
            nRetCode = m_pNext->UpdateLevel(nLevel);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        if (m_pFirstChild)
        {
            nRetCode = m_pFirstChild->UpdateLevel(nLevel + 1);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

Exit1:
        nResult = true;
Exit0:
        return nResult;
    }

    void* CastWndWindow(KWndWindow* pWnd, LPCSTR szType)
    {
        if (pWnd == NULL)
            return NULL;

        if (pWnd->CanCastTo(szType))
            return pWnd;

        return NULL;
    }

} // namespace UI
