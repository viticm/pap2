//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Win.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#ifndef KMp3WinClass_H
#define KMp3WinClass_H
//---------------------------------------------------------------------------

class KMp3Win
{
public:
	void window(float *vbuf, int vb_ptr, short *pcm);
	void window_dual(float *vbuf, int vb_ptr, short *pcm);
	void window16(float *vbuf, int vb_ptr, short *pcm);
	void window16_dual(float *vbuf, int vb_ptr, short *pcm);
	void window8(float *vbuf, int vb_ptr, short *pcm);
	void window8_dual(float *vbuf, int vb_ptr, short *pcm);
protected:
private:

};


//---------------------------------------------------------------------------
#endif // KMp3WinClass_H
