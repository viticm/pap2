
////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFlashTexture.h
//  Version     : 1.0
//  Creator     : Chiyer
//  Create Date : 2007-9-20
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KGFLASH_H_
#define _KGFLASH_H_

#include <atlbase.h>
#include <atlwin.h>
#include "flash.h"
#include "KG3DTexture.h"

interface IDirect3DTexture9;
interface IDirect3DDevice9;
extern CComModule _Module;

class KG3DFlashTexture : public KG3DTexture
{

public :
    HRESULT Initialize(LPDIRECT3DDEVICE9 pD3DDevice, int nWidth, int nHeight);
    HRESULT LoadFromFile(const char cszFileName[], unsigned uFileNameHash, unsigned uOption);
    HRESULT LoadFromFileEx(const char cszFileName[], unsigned uFileNameHash, unsigned uOption, DWORD_PTR WParam, DWORD_PTR LParam);
    long    DrawFrame();
    BYTE*   GetBitmapBuffer();
    void    Play();
    void    Stop();
    void    Loop();
    void    Once();
    void    UpdateBuffer();

    KG3DFlashTexture();
    virtual ~KG3DFlashTexture();

    virtual operator IDirect3DTexture9* ()     { UpdateBuffer(); return m_pD3DTexture; }
    virtual IDirect3DTexture9* GetTexture()    { UpdateBuffer(); return m_pD3DTexture; }

    virtual void HandelClick(POINT point) {

        if (!m_pAxWindow)
            return;

        CWindow WndFlash = m_pAxWindow->GetWindow(GW_CHILD);
        
        if (!WndFlash.IsWindow())
            return;

        WndFlash.SendMessage(WM_MOUSEMOVE,   0, MAKELPARAM(point.x, point.y));
        WndFlash.SendMessage(WM_LBUTTONDOWN,   0, MAKELPARAM(point.x, point.y));
        WndFlash.SendMessage(WM_LBUTTONUP,   0, MAKELPARAM(point.x, point.y));
    }

private :
    IShockwaveFlash* m_pShockwaveFlash;
    CAxWindow      * m_pAxWindow;
    IUnknown       * m_pIUnknow;
    IViewObjectEx  * m_pView;
    BYTE           * m_pBuffer;

    HDC              m_CompatibleDc;
    HBITMAP          m_hBitmap;
    void           * m_pFrameBuffer;

    long             m_lLastFrame;

    IDirect3DTexture9* m_pD3DTexture;
    ULONG              m_uRef;
    IDirect3DDevice9 * m_pD3DDevice;

    int                m_nWidth;
    int                m_nHeight;

    int                m_bPlaying;

    HWND               m_hParentWnd;

    CLSID const& GetClsid()
    {
        static CLSID const clsid
            = { 0xd27cdb6e, 0xae6d, 0x11cf, { 0x96, 0xb8, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 } };
        return clsid;
    }

    IID const& GetIid()
    {
        static IID const iid
            = { 0xD27CDB6C, 0xAE6D, 0x11CF, { 0x96, 0xB8, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 } };
        return iid;
    }
};

#endif