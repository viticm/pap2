////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemsence.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-22 17:07:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_ITEMSCENE_H_
#define _INCLUDE_ITEMSCENE_H_

////////////////////////////////////////////////////////////////////////////////

#include "./item.h"
#include "KG3DEngine/KG3DInterface.h"

////////////////////////////////////////////////////////////////////////////////


namespace UI
{

struct KItemSceneData : KItemNullData
{
	int nEnableFrameMove;
	int nEnableRenderTerrain;
	int nEnableRenderSkyBox;
	int nEnableAlpha;

    KItemSceneData(KWndWindow *pEOwner) : KItemNullData(pEOwner)
    {
        nEnableFrameMove = false;
		nEnableRenderTerrain = true;
		nEnableRenderSkyBox = true;
		nEnableAlpha = true;
    };
};


class KItemScene : public KItemNull
{
protected:
    KItemScene();
    virtual ~KItemScene();

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
    static KItemScene *Create(KItemSceneData &ItemSceneData);
    virtual void Release();
    virtual int Init(KItemSceneData &ItemSceneData);

    virtual void Draw();
    virtual DWORD GetItemType();
    virtual int IsItemCanChangeTo(DWORD dwType);

	void	SetScene(IKG3DScene* p3DScene);
	void	EnableFrameMove(int nEnable);
	void	EnableRenderTerrain(int nEnable);
	void	EnableRenderSkyBox(int nEnable);
	void	EnableAlpha(int nEnable);

public:
    static  DWORD ms_dwItemType;

private:
    KG3DUIScene2 m_Scene;
};




}//namespace UI





#endif //_INCLUDE_ITEMSCENE_H_
