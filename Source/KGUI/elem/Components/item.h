////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : item.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-9 16:46:29
//  Comment     : 
//                Edited by Chen Jie 2006-12-30
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_ITEM_H_
#define _INCLUDE_ITEM_H_

#ifdef KGUI_LUA_LOADER
#include "../uiobject.h"
#endif

namespace UI
{
#define UI_FLOAT_MIN	0.1f
#define UI_FLOAT_LARGE(x, y) ((x) - (y) > UI_FLOAT_MIN)	
#define UI_FLOAT_SMALL(x, y) ((y) - (x) > UI_FLOAT_MIN)
#define UI_FLOAT_EQUAL(x, y) (abs((x) - (y)) <= UI_FLOAT_MIN)
#define UI_FLOAT_LARGE_OR_EQUAL(x, y) ((x) - (y) >= -UI_FLOAT_MIN)	
#define UI_FLOAT_SMALL_OR_EQUAL(x, y) ((y) - (x) >= -UI_FLOAT_MIN)

#define ITEM_TYPE_NAME_MAX_SIZE 16
#define ITEM_NAME_MAX_SIZE 32
#define ITEM_MAX_DECODE_TEXT_LEN 2048
#define ITEM_LINK_INFO_MAX_SIZE 32

//ItemNull占据低4位。
#define ITEM_SHOW				0x00000001
#define ITEM_DATA_FORMATED		0x00000002
#define ITEM_NEVER_SCALED		0x00000004
#define LOCK_SHOW_AND_HIDE		0x00000008
#define ITEM_INT_POS			0x00000010	//整形坐标

enum ITEM_POSITION
{
    POSITION_BEGINE = 0,
    POSITION_BY_CUSTOM = POSITION_BEGINE,
    POSITION_LEFT_BOTTOM,     
    POSITION_LEFT_TOP,  
    POSITION_LEFT_CENTER,
    POSITION_TOP_LEFT,        
    POSITION_TOP_RIGHT,
    POSITION_TOP_CENTER,
    POSITION_RIGHT_TOP,       
    POSITION_RIGHT_BOTTOM,
    POSITION_RIGHT_CENTER,
    POSITION_BOTTOM_LEFT,     
    POSITION_BOTTOM_RIGHT,   
    POSITION_BOTTOM_CENTER,
    POSITION_END,
};

enum ITEM_TYPE
{
    ITEM_BEGINE = 0,
    ITEM_NULL = ITEM_BEGINE,
    ITEM_TEXT,
    ITEM_IMAGE,
    ITEM_SHADOW,
    ITEM_ANIAMTE,
    ITEM_SCENE,
	ITEM_BOX,
    ITEM_HANDLE,
    ITEM_TREE_LEAFE,
    ITEM_UNKNOWN,
    ITEM_COUNT = ITEM_UNKNOWN,
};

int g_GetWndItemType(LPCSTR pcszTypeName);
LPCSTR g_GetWndItemName(int nItemType);

////////////////////////////////////////////////////////////////////////////////

class KItemHandle;
class KWndWindow;

struct KItemNullData 
{
    WCHAR wszName[ITEM_NAME_MAX_SIZE];
    WCHAR wszLinkInfo[ITEM_LINK_INFO_MAX_SIZE];
    KWndWindow *pEventOwner;
    DWORD dwEventID;
    
	int	  nIntPos;
    int   nAutoSize;
	int   nDisableScale;
	int   nLockShowAndHide;
	int   nScript;
    int   nLink;

	int   nTipIndex;
	int	  nAreaTestIndex;
    int   nPosType;
	int	  nAlpha;
    float fRelX;
    float fRelY;
    float fWidth;
    float fHeight;
	float fScale;

    //  Modified by Chen Jie, 2006-12-30: Replace fUserData with an union, makes reuse more type safer.
    union
    {
	    double      fUserData;  //Should be named lfUserData ?
        LONGLONG    llUserData;
        DWORD       dwUserData[2];
    };

    KItemNullData(KWndWindow *pEOwner)
    {
		nIntPos = false;
        wszName[0] = L'\0';
        wszLinkInfo[0] = L'\0';
        pEventOwner = pEOwner;
        dwEventID = 0;
        nAutoSize = true;
        nPosType = POSITION_BY_CUSTOM;
		nTipIndex = -1;
		nAreaTestIndex = -1;
		nDisableScale = false;
		nLockShowAndHide = false;
		nScript = false;
        nLink = false;
        fRelX = 0.0f;
        fRelY = 0.0f;
        fWidth = 0.0f;
        fHeight = 0.0f;
		nAlpha = 255;
		fUserData = 0;
		fScale = 1.0f;
    };
};

class KItemNull 
#ifdef KGUI_LUA_LOADER
	: public IKUIObject
#endif
{
protected:
    KItemNull();
    virtual ~KItemNull();

public:
	void* operator new(size_t nSize, std::nothrow_t const&) throw();
	void operator delete(void* pData, std::nothrow_t const&) throw();

	void* operator new(size_t nSize);
	void operator delete(void* pData) throw();

protected:
	void* operator new[](std::size_t, std::nothrow_t const&) throw();
	void operator delete[](void*, std::nothrow_t const&) throw();

	void* operator new[](std::size_t) throw(std::bad_alloc);
	void operator delete[](void*) throw();

	void* operator new(size_t, void*) throw();
	void operator delete(void*, void*) throw();

	void* operator new[](std::size_t, void*) throw();
	void operator delete[](void*, void*) throw();

#ifdef KGUI_LUA_LOADER
public:
    static int  Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex);
    virtual int Init(Lua_State *pLua, int nTableIndex);
    virtual int AddChild(IKUIObject *pChild) { return false; }  // 不能添加子控件，只有Handle可以
#endif

public:
    static KItemNull *Create(KItemNullData &ItemNullData);
    virtual void    Release();
    virtual int     Init(KItemNullData &ItemNullData);
	void			OnInitEnd(KItemNullData &ItemNullData);

#ifndef DISABLE_UI_EDIT
    virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

    virtual void    Draw();
    virtual DWORD   GetItemType();
    virtual int     IsItemCanChangeTo(DWORD dwType);

    virtual int     PtInItem(float fAbsX, float fAbsY);

    virtual int     SetRelPos(float fX, float fY);
    int             GetRelPos(float &fX, float &fY);

    virtual int     SetAbsPos(float fX, float fY);
    int             GetAbsPos(float &fX, float &fY);

    virtual int     AbsMove(float fDeltaX, float fDeltaY);
    virtual int     Scale(float fScaleX, float fScaleY);

    virtual int     UpdateLevel(int nLevel);

	int			Hide();
	int			Show();

	virtual int	SetAlpha(int nAlpha);
	int	        GetAlpha();

	int		    EnableScale(int nEnable);
	int		    LockShowAndHide(int nLock);
	int		    IsShowOrHideLocked();

    int         SetSize(float fWidth, float fHeight);
    int         GetSize(float &fWidth, float &fHeight);

    int         SetPosType(DWORD dwPosType);
    int         GetPosType(DWORD &dwPosType);

	int         SetTipIndex(int nIndex);
	int         GetTipIndex();
	int		    ShowTip();

    int         SetName(LPCSTR szItemName);
    LPCSTR      GetName() const;

    int         IsVisible();	//只检查自己
	int			IsVisibleInWnd();	//检查到wnd

    int		    SetUserData(double fUserData);
	double      GetUserData();
    int         SetUserDataLONGLONG(LONGLONG llUserData);
    int         GetUserDataLONGLONG(LONGLONG *pllRetUserData);
    int         SetUserDataDWORD(DWORD dwUserData1, DWORD dwUserData2);
    int         GetUserDataDWORD(DWORD *pdwRetUserData1, DWORD *pdwRetUserData2);
	int         SetUserDataInt(int nUserData1, int nUserData2);
	int         GetUserDataInt(int *pnRetUserData1, int *pnRetUserData2);

	int		    SetParent(KItemHandle *pHandle);
	KItemHandle *GetParent();
	KWndWindow	*GetWndOwner();

	void	    SetLuaRef(int nRet);
	int		    GetLuaRef();

	DWORD		GetEvent();
	void		RegisterEvent(DWORD dwEvent);
	void		ClearEvent();

	void		NotifyDraw();

	int			SetAreaTestFile(LPCSTR pcszFile);

	void		SetIntPos(int nIntPos);
	int			IsIntPos();

    int         IsLink();
    LPCSTR      GetLinkInfo() const;
    int         SetLinkInfo(const char szLinkInfo[]);
public:
    static  DWORD       ms_dwItemType;

protected:
    DWORD   m_dwStyle;    
    DWORD   m_dwPosType;    
    float   m_fRelX;
    float   m_fRelY;
    float   m_fAbsX;
    float   m_fAbsY;
    float   m_fWidth;
    float   m_fHeight;

	int		m_nTipIndex; //提示索引
	int		m_nAreaIndex;

	DWORD	m_dwAlpha;	//Alpha值

	DWORD	m_dwEvent; //挂的事件

    //  Modified by Chen Jie, 2006-12-30: Replace fUserData with an union, makes reuse more type safer.
    union
    {
	    double      m_fUserData;  //Should be named lfUserData ?
        LONGLONG    m_llUserData;
        DWORD       m_dwUserData[2];
		int			m_nUserData[2];
    };

	KItemHandle *m_pParent;

	int m_nLuaRef;	//lua里面的引用
    int m_nLevel;
    
    int  m_nLink; //是否是链接
    char m_szLinkInfo[ITEM_LINK_INFO_MAX_SIZE];

    char   m_szItemName[ITEM_NAME_MAX_SIZE];
};

////////////////////////////////////////////////////////////////////////////////

template<typename TItemType> 
	TItemType* g_ItemCast(KItemNull* pItem)
{
	if (!pItem)
		return NULL;

    if (pItem->IsItemCanChangeTo(TItemType::ms_dwItemType))
        return (TItemType *)(pItem);

	return NULL;
}

inline void	KItemNull::SetLuaRef(int nRet)
{
	m_nLuaRef = nRet;
}

inline int KItemNull::GetLuaRef()
{
	return m_nLuaRef;
}

inline KItemHandle *KItemNull::GetParent()
{
	return m_pParent;
}

inline DWORD KItemNull::GetEvent()
{
	return m_dwEvent;
}

} //namespce UI

#endif //_INCLUDE_ITEM_H_
