/*****************************************************************************************
//	界面窗口体系结构--图形窗口
//	Copyright : Kingsoft 2002
//	Author	:   Wooy(Wu yue)
//	CreateTime:	2002-7-25
------------------------------------------------------------------------------------------
显示(spr)图形的窗口。
*****************************************************************************************/
#ifndef UI_ELEM_K_WND_SCENE_H
#define UI_ELEM_K_WND_SCENE_H

#include "./wndwindow.h"
#include "KG3DEngine/KG3DInterface.h"

namespace UI
{

class KWndScene : public KWndWindow
{
public:
	KWndScene();
	virtual ~KWndScene();

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
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);

	virtual int	CanCastTo(LPCSTR szType) const;
	virtual LPCSTR GetWndType() const;

	void SetScene(IKG3DScene* p3DScene);

	void EnableFrameMove(bool bEnable);
	void EnableRenderTerrain(bool bEnable);
	void EnableRenderSkyBox(bool bEnable);
	void EnableAlpha(bool bEnable);
	void UpdateSceneViewport();

protected:
	virtual void OnPaint();
    virtual void OnBreathe();
	virtual void OnSize();
	virtual void OnScale(float fScaleX, float fScaleY);
	virtual void OnMove();

private:
    KG3DUIScene2 m_Scene;
};

}   // namespace UI

#endif // UI_ELEM_K_WND_SCENE_H

