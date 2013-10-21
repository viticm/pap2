////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KShow.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-21 10:25:48
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSHOW_H_
#define _INCLUDE_KSHOW_H_

////////////////////////////////////////////////////////////////////////////////

#include <vector>

////////////////////////////////////////////////////////////////////////////////


#define SHOW_BUFFER_WIDTH   1024
#define SHOW_BUFFER_HEIGHT  1024

class Gdiplus::Graphics;
class Gdiplus::Bitmap;
class Gdiplus::Pen;
class KTgaBlock;

class KShow
{
    typedef std::vector<RECT> KShowRectArray;
protected:
    KShow();
    ~KShow();

public:
    static int Init(HWND hWnd);
    static int Exit();
    static KShow &GetSelf();

    int CreateBuffer();
    int DeleteBuffer();

    int Clear();
    int AppendBuffer(KTgaBlock *pBlock);
	int AppendBuffer(KTgaBlock *pBlock, RECT &Rect);
    int AppendShowRect(RECT &Rect);

    int PaintWindow(HWND hWnd, Gdiplus::Graphics *pGraphics);
	int PaintWindow(HWND hWnd);

    int CreatePen();
    int DeletePen();

    int CreateBitMap();
    int DeleteBitMap();

    int SetNeedReDraw();
    int ClearNeedReDraw();

    int GetShowStartX();
    int GetShowStartY();

    int SetShowStartX(int x);
    int SetShowStartY(int y);

    int GetDesiredWidth();
    int GetDesiredHeight();

    int SetDesiredWidth(int nWidth);
    int SetDesiredHeight(int nHeight);

	LPDIRECT3DDEVICE9 GetDevice();

protected:
	int InitDirect3D(HWND hWnd);
	int	UnInitDirect3D();

	int DrawDesiredRect();

private:
    static KShow *ms_pSelf;
    BYTE                 *m_pbyData;
    Gdiplus::Bitmap      *m_pBitMap;
    Gdiplus::Pen         *m_pPen;
    int                   m_nDrawDataHasBeenChged;
    KShowRectArray m_aRect;

    int m_nShowStartX;
    int m_nShowStartY;

    int m_nDesiredWidth;
    int m_nDesiredHeight;

	LPDIRECT3D9 m_pD3D;
	LPDIRECT3DDEVICE9 m_p3DDevice;
};



#endif //_INCLUDE_KSHOW_H_
