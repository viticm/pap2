#ifndef UI_ELEM_K_WND_TEXTURE_H
#define UI_ELEM_K_WND_TEXTURE_H


#include "./wndwindow.h"


class IKG3DModel;

namespace UI
{

class KWndTexture : public KWndWindow
{
public:
	KWndTexture();
	virtual ~KWndTexture();

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
#endif

public:
	virtual int	CanCastTo(LPCSTR szType) const;
	virtual LPCSTR GetWndType() const;

	bool SetTexture(IKG3DModel* p3DModel);

protected:
	virtual void OnPaint();

private:
	static void RenderCallback(DWORD dwTextureID, void* pUserData);
};

}   // namespace UI

#endif // UI_ELEM_K_WND_TEXTURE_H

