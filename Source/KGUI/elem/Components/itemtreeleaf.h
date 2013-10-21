////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemtreeleaf.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-29 13:41:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_ITEMTREELEAF_H_
#define _INCLUDE_ITEMTREELEAF_H_

////////////////////////////////////////////////////////////////////////////////

#include "./itemhandle.h"

////////////////////////////////////////////////////////////////////////////////

namespace UI
{
//ItemHandleÕ¼¾Ýµ×6Î»¡£
#define ITEM_TREE_LEAFE_EXPAND				0x01000000
#define ITEM_TREE_LEAFE_NODE				0x02000000
#define ITEM_TREE_LEAFE_SHOW_LINE			0x04000000
#define ITEM_TREE_LEAFE_NODE_FORMATED		0x08000000
#define ITEM_TREE_LEAFE_ALWAYS_NODE			0x10000000

struct  KItemTreeLeafData : KItemHandleData
{
    char szImageName[MAX_PATH];
    DWORD dwLineColor;
    int nShowLine;
    int nExpandIconFrame;
    int nCollapseIconFrame;
    int nIndent;
    float fEachIndentWidth;
    float fIconWidth;
    float fIconHeight;
	int nAutoIconSize;
	int nAlwaysNode;

    KItemTreeLeafData(KWndWindow *pEOwner) : KItemHandleData(pEOwner)
    {
        szImageName[0] = '\0';
        nShowLine = true;
        nExpandIconFrame = -1;
        nCollapseIconFrame = -1;
        nIndent = 0;
        fEachIndentWidth = 0.0f;
        dwLineColor = 0;
        fIconWidth = 0.0f;
        fIconHeight = 0.0f;
		nAutoIconSize = true;
		nAlwaysNode = false;
    };
};


class KItemTreeLeaf : public KItemHandle
{
    typedef std::vector<KG3DUILine2> KTreeLeafLinkLineArray;

protected:
    KItemTreeLeaf();
    virtual ~KItemTreeLeaf();

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
#endif

public:
    static KItemTreeLeaf *Create(KItemTreeLeafData &ItemTreeLeafData);
    virtual void Release();
    virtual int Init(KItemTreeLeafData &ItemTreeLeafData);


#ifndef DISABLE_UI_EDIT
    virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
#endif

    virtual void Draw();
    virtual DWORD GetItemType();
    virtual int IsItemCanChangeTo(DWORD dwType);
    virtual int SetAbsPos(float fAbsX, float fAbsY);
    virtual int AbsMove(float fDeltaX, float fDeltaY);
    virtual int Scale(float fScaleX, float fScaleY);

    virtual int FormatAllItemPos();

    int AppendLineLinkToItem();
    int AppendLineLinkToPrev();
    int AppendLineLinkToNext();
    int AppendLineLinkPrevAndNext(int nIndent);
    int ClearAllLine();

    int GetIndent();
    int SetIndent(int nIndent);

    int SetEachIndentWidth(float fWidth);
    float GetEachIndentWidth();

    int IsExpand();
    int Expand();
    int Collapse();

    int IsNode();
    int SetNode();
    int ClearNode();

    int SetShowIndex(int nIndex);
    int GetShowIndex();

    int SetNodeIconSize(float fWidth, float fHeight);

    int SetIconImage(LPCSTR szImageName, int nExpandFrame, int nCollapseFrame);
    int AdjustNodeIconPos();
    int PtInIcon(float fAbsX, float fAbsY);
    int AutoSetIconSize();

private:
	int FormateIconImage();

public:
    static DWORD ms_dwItemType;

private:
    int m_nShowIndex;
    int m_nIndent;
    float m_fEachIndentWidth;

    float m_fIconWidth;
    float m_fIconHeight;

	KG3DUIImage2 m_Image;
	int m_nImageInfoIndex;
    int m_nExpandIconFrame;
    int m_nCollapseIconFrame;

    DWORD m_dwLineColor;
    KTreeLeafLinkLineArray m_aLine;    
};



} //namespace UI


#endif //_INCLUDE_ITEMTREELEAF_H_
