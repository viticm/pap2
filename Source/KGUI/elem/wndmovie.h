#ifndef UI_ELEM_K_WND_MOVIE_H
#define UI_ELEM_K_WND_MOVIE_H

#include "KG3DEngine/KG3DInterface.h"
#include "LVideo.h"
#include "./wndwindow.h"

namespace UI
{

class KWndMovie : public KWndWindow
{
public:
	KWndMovie();
	virtual ~KWndMovie();

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
	static int Init();
	static void Exit();

	virtual int CanCastTo(LPCSTR szType) const;
	virtual LPCSTR GetWndType() const;

	bool Play(LPCSTR szVideoFile);
	void Stop();
	void SetVolume(unsigned int uVolume);
	bool IsFinished() const;

	void UpdateMovieViewport();

protected:
	virtual void OnBreathe();
	virtual void OnPaint();
	virtual void OnSize();
	virtual void OnScale(float fScaleX, float fScaleY);
	virtual void OnMove();

private:
	void DrawMovie();

	static void DrawMovieCallback(BYTE* pBits, INT nPitch, UINT uWidth, UINT uHeight, D3DFORMAT Format, void* pUserdata);

private:
	HKLVIDEO m_hVideo;

	KG3DUIImage2 m_Image;
};

}   // namespace UI

#endif // UI_ELEM_K_WND_MOVIE_H

