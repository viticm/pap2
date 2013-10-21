////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemanimate.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-9 17:11:57
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_ITEMANIMATE_H_
#define _INCLUDE_ITEMANIMATE_H_

////////////////////////////////////////////////////////////////////////////////

#include "./item.h"

////////////////////////////////////////////////////////////////////////////////

namespace UI
{

struct KItemAnimateData : public KItemNullData
{
    char szImagePath[MAX_PATH];
    int nGroup;
    int nLoopCount;

	KItemAnimateData(KWndWindow *pEOwner) : KItemNullData(pEOwner)
    {
        szImagePath[0] = '\0';
        nGroup = 0;
        nLoopCount = -1;
    };
};

class KItemAnimate : public KItemNull
{
protected:
    KItemAnimate();
    virtual ~KItemAnimate();

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
    static KItemAnimate *Create(KItemAnimateData &ItemAnimateData);
    virtual void Release();
    virtual int Init(KItemAnimateData &ItemAnimateData);

    virtual void	Draw();
    virtual DWORD	GetItemType();
    virtual int     IsItemCanChangeTo(DWORD dwType);

    int SetAnimate(LPCSTR szImageName, int nAnimateGroup, int nLoopCount);
    int AutoSize();
    int SetImagePath(LPCSTR szImagePath);
    int SetGroup(int nGroup);
    int SetLoopCount(int nLoopCount);
    void Replay();
public:
    static  DWORD ms_dwItemType;
protected:
private:
    int     m_nAnimateIndex;
};

}//namespace UI

#endif //_INCLUDE_ITEMANIMATE_H_
