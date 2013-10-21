//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KBitmap16.h
// Date:	2000.08.08
// Code:	Daniel Wang
// Desc:	Header File
//---------------------------------------------------------------------------
#ifndef KBitmap16_H
#define KBitmap16_H
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
typedef struct {
	char		Id[4];	// "BM16"
	int			Width;
	int			Height;
	int			RGBMask;
} KBMPHEADER16;
//---------------------------------------------------------------------------
class KBitmap16
{
protected:
	void*		m_pBitmap;
	int			m_nWidth;
	int			m_nHeight;
public:
	KBitmap16();
	BOOL		Init(int nWidth, int nHeight);
	void		Free();
	BOOL		Load(LPSTR lpFileName);
	BOOL		Save(LPSTR lpFileName);
	void		Draw(int nX, int nY);
	void		DrawLum(int nX, int nY, int nLum);
	void		Clear();
	void		PutPixel(int nX, int nY, WORD wColor);
	WORD		GetPixel(int nX, int nY);
	LPWORD		GetPointer(int nX, int nY);

public:
	void*		GetBitmap()	{ return m_pBitmap; };
	int			GetWidth()	{ return m_nWidth; };
	int			GetHeight()	{ return m_nHeight; };
};
//---------------------------------------------------------------------------
#endif
