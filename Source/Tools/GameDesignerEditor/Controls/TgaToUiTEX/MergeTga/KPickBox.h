////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KPickBox.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-20 17:07:05
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KPICKBOX_H_
#define _INCLUDE_KPICKBOX_H_

////////////////////////////////////////////////////////////////////////////////

class KTgaBlock;
class Gdiplus::Graphics;
class Gdiplus::Bitmap;
class Gdiplus::Pen;

class KPickBox
{
protected:
    KPickBox();
    ~KPickBox();

public:
    static int Init();
    static int Exit();
    static KPickBox &GetSelf();

    int Clear();

    int Pick(int nIndex);
    int Drop();
    int GetPickBlockIndex();

    int PaintWindow(HWND hWnd, Gdiplus::Graphics *pGraphics);

    int CreatePen();
    int DeletePen();

    int CreateBitMap();
    int DeleteBitMap();

    int CalculatePickBlockPos(HWND hWnd);

    int GetLastDrawRect(RECT &rcRetRect);

public:
protected:
private:
    static KPickBox      *ms_pSelf;
    int                   m_nTgaBlockIndex;
    Gdiplus::Bitmap      *m_pBitMap;
    Gdiplus::Pen         *m_pPen;
    RECT                 m_rcLastDrawRect;

};



#endif //_INCLUDE_KPICKBOX_H_
