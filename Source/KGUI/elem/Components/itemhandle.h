////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemhandle.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-9 17:18:20
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_ITEMHANDLE_H_
#define _INCLUDE_ITEMHANDLE_H_

////////////////////////////////////////////////////////////////////////////////

#include "./item.h"
#include "./itemtext.h"
#include "./itemimage.h"
#include "./itemscene.h"
#include "./itemshadow.h"
#include "./itemanimate.h"
#include "./itembox.h"


////////////////////////////////////////////////////////////////////////////////

namespace UI
{

#define ITEM_HANDLE_FORMAT_POS_WHEN_APPEND		0x00010000
#define ITEM_HANDLE_AJUST_RICH_TEXT_SHOW_H		0x00020000
#define ITEM_HANDLE_AUOT_ADJUST_SIZE_BY_CHILD	0x00040000
#define ITEM_HANDLE_USE_STENCIL					0x00080000 //使用精确剪裁，不支持欠套
#define ITEM_HANDLE_DRAW_STENCIL_BEGIN			0x00100000 //设置过精确剪裁了
#define ITEM_HANDLE_STENCIL_DISABLE_EXTENT		0x00200000
#define ITEM_HANDLEL_CONTROL_SHOW				0x00400000 //控制子控件的显示隐藏		

class KWndWindow;
class KItemTreeLeaf;

enum {ITEM_TREE_PATH_LEN = 256};
enum ITEM_HANDLE_STYLE
{
    ITEM_HANDLE_STYLE_BEGINE = 0,
    ITEM_HANDLE_CUSTOM = ITEM_HANDLE_STYLE_BEGINE,
    ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_CHANGELESS,
    ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BETWEEN_MIN_MAX,
    ITEM_HANDLE_AUTO_NEW_LINE_ROW_HEIGHT_BY_MAX_ITEM,
    ITEM_HANDLE_CHAT_ITEM_HANDLE,
    ITEM_HANDLE_TREE_ITEM_HANDLE,
	ITEM_HANDLE_LEFT_RIGHT_ALIGN_BY_TABLE,
    ITEM_HANDEL_STYLE_END,
};

struct KItemHandleData : KItemNullData
{
    int nHandleType;
    int nFirstItemPosType;
	int nPixelScroll;
	int nControlShow;
    float fRowSpacing;
    float fRowHeight;
    float fMaxRowHeight;
    KItemHandleData(KWndWindow *pEOwner) : KItemNullData(pEOwner)
    {
        nHandleType = ITEM_HANDLE_CUSTOM;
        nFirstItemPosType = POSITION_LEFT_TOP;
        fRowSpacing = 0.0f;
        fRowHeight = 0.0f;
        fMaxRowHeight = 0.0f;
		nPixelScroll = false;
		nControlShow = false;
    };
};

enum 
{
    INSERT_BEFORE = 0,
    INSERT_BEHIND = 1,
};


class KItemHandle : public KItemNull
{
public:
    typedef std::vector<KItemNull *> KItemArray;

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
    virtual int AddChild(IKUIObject *pChild);
#endif

public:
    static KItemHandle *Create(KItemHandleData &ItemHandleData);
    virtual void Release();
    virtual int Init(KItemHandleData &ItemHandleData);

#ifndef DISABLE_UI_EDIT
    virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
    int     PtInMeOrMyChild(float fAbsX, float fAbsY);
#endif

    virtual void Draw();
    virtual DWORD GetItemType();
    virtual int IsItemCanChangeTo(DWORD dwType);
    virtual int SetAbsPos(float fAbsX, float fAbsY);
    virtual int AbsMove(float fDeltaX, float fDeltaY);
    virtual int Scale(float fScaleX, float fScaleY);

	virtual int SetAlpha(int nAlpha);

    virtual int UpdateLevel(int nLevel);

    int     SetSizeByAllItemSize();
    int     SetItemStartRelPos(float fRelX, float fRelY);
	int		GetItemStartRelPos(float &fRelX, float &fRelY);
    virtual int FormatAllItemPos();

    int     GetItemCount();

    int     GetAllItemSize(float &fWidth, float &fHeight);

    int     SetHandleStyle(DWORD dwHandleStyle);
    int     SetMinRowHeight(float fHeight);
    int     SetMaxRowHeight(float fHeight);
    int     SetRowHeight(float fHeight);
    int     SetRowSpacing(float fSpacing);
	float	GetRowSpacing();

    int     AppendItem(KItemNull *pItem);
    int     InsertItem(KItemNull *pItem, KItemNull *pItemReference, int nBeforeOrBehind);
	int     InsertItem(int nPos, KItemNull *pItem);
    int     Remove(LPCSTR szItemTreePath);
    int     Remove(KItemNull *pItem);
    int     Clear();

    KItemNull* GetItemByTreePath(LPCSTR szTreePath, KItemHandle** ppHandle = NULL, LPCSTR szDelimit = "/\\");
    KItemNull* GetItemByIndex(int nIndex);

    KItemHandle* GetItemHandle(KItemNull *pItemNull);

    int     GetItemIndex(KItemNull *pItemNull);
	int		GetItemTreePath(KItemNull* pItemNull, LPSTR szOutTreePath, int nOutTreePathLen);

	int		AdjustItemShowInfo();

	int		SetItemNewIndex(int nOriginalIndex, int nNewIndex);
	int		GetVisibleItemCount() const;

	int		PushAllEventItem(KItemArray &arItemArray);
	int		PushAllDrawItem(KItemArray &arItemArray);

	int		EnableFormatWhenAppend(int nFlag);

	int		RemoveItemUntilNewLine();

	int		SetAjustShowRichTextType(int nType);

	KWndWindow *GetOwner();
	int SetOwner(KWndWindow *pOwner);

	int ExchangeIndex(int nFirst, int nLast);

	void Sort();

	void SetDisableExtent(int nDisable);
	int IsDisableExtent();

	void SetControlShow(int nControl);
	int IsControlShow();

	int IsInStencialArea(float fX, float fY);
protected:
    KItemHandle();
    virtual ~KItemHandle();

private:
    KItemNull* FindItem(LPCSTR szItemName);

    int     FormatAllItemPosByCustom();
    int     FormatAllItemPosByAutoNewLineRowHeightChangeless();
    int     FormatAllItemPosByAutoNewLineRowHeightBetweenMinMax();
    int     FormatAllItemPosByAutoNewLineRowHeightByMaxItem();
    int     FormatAllItemPosByChatItemHandle();
    int     FormatAllItemPosByTreeItemHandle();
	int     FormatAllItemPosByLeftRightAlignByTable();

	int		FormatLastItemPos();
	int     FormatLastItemPosByCustom(int nCount);
	int     FormatLastItemPosByAutoNewLineRowHeightChangeless(int nCount);
	int		FormatLastItemPosByAutoNewLineRowHeightBetweenMinMax(int nCount);
	int		FormatLastItemPosByAutoNewLineRowHeightByMaxItem(int nCount);
	int     FormatLastItemPosByChatItemHandle(int nCount);
	int     FormatLastItemPosByTreeItemHandle(int nCount);
	int     FormatLastItemPosByLeftRightAlignByTable(int nCount);

	inline int		CorrectAllItemRect(float fX, float fY, float fW, float fH);

	int		AdjustRowItemEndAbsY(KItemArray::iterator itBegin, KItemArray::iterator itEnd, float fOldY, float fNewY);

	int		IsHandleRichTextHandle() const;

	int		NeedExtent();
public:
    static  DWORD   ms_dwItemType;
protected:
private:
    DWORD           m_dwHandleType;
    DWORD           m_dwFirstItemPosType;
    float           m_fMinRowHeight;
    float           m_fMaxRowHeight;
    float           m_fRowSpacing;

    float           m_fItemStartRelY;   //Item起始y轴相对位移
    float           m_fItemStartRelX;   //Item起始x轴相对位移
    float           m_fAllItemHeight;   //所有item的总高度
    float           m_fAllItemWidth;    //所有item的总宽度

	KWndWindow		*m_pWndOwner;

    KItemArray      m_aItem;
};

} //namespace UI


#endif //_INCLUDE_ITEMHANDLE_H_
