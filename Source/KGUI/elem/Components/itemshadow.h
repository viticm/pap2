////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemshadow.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-15 15:57:15
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_ITEMSHADOW_H_
#define _INCLUDE_ITEMSHADOW_H_

#include "KG3DEngine/KG3DInterface.h"
#include "./item.h"
#include "../../driver/kcolor.h"

////////////////////////////////////////////////////////////////////////////////


namespace UI
{
struct KItemShadowData : KItemNullData
{
    DWORD dwShadowColor;

    KItemShadowData(KWndWindow *pEOwner) : KItemNullData(pEOwner)
    {
        dwShadowColor = 0x00000000;
    }
};

#define SHADOW_TRIANGLE_FAN 0x00010000

class KItemShadow : public KItemNull
{
public:
protected:

protected:
    KItemShadow();
    virtual ~KItemShadow();

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
    static KItemShadow *Create(KItemShadowData &ItemShadowData);
    virtual void Release();
    virtual int Init(KItemShadowData &ItemShadowData);

	virtual int Scale(float fScaleX, float fScaleY);
    
    virtual void Draw();
    virtual DWORD GetItemType();
    virtual int IsItemCanChangeTo(DWORD dwType);

	virtual int	SetAlpha(int nAlpha);
    
	int SetTriangleFan(int nTriangleFan);
	int IsTriangleFan();
	int AppendTriangleFanPoint(float fX, float fY, DWORD dwClolor);
	int ClearTriangleFanPoint();

	int SetShadowColor(DWORD dwColor);
	DWORD GetshadowColor();
public:
    static  DWORD ms_dwItemType;
private:
    KG3DUIRect2 m_ShadowRect;
	typedef std::vector<KG3DUIVertex2> VertexArray;
	VertexArray m_OrgVertex;
};

}

#endif //_INCLUDE_ITEMSHADOW_H_


