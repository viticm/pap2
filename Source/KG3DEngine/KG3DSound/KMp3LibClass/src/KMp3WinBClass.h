//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3WinB.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#ifndef KMp3WinBClass_H
#define KMp3WinBClass_H
//---------------------------------------------------------------------------

class KMp3WinB
{
public:
	void windowB(float *vbuf, int vb_ptr, unsigned char *pcm);
	void windowB_dual(float *vbuf, int vb_ptr, unsigned char *pcm);
	void windowB16(float *vbuf, int vb_ptr, unsigned char *pcm);
	void windowB16_dual(float *vbuf, int vb_ptr, unsigned char *pcm);
	void windowB8(float *vbuf, int vb_ptr, unsigned char *pcm);
	void windowB8_dual(float *vbuf, int vb_ptr, unsigned char *pcm);
protected:
private:
};

//---------------------------------------------------------------------------
#endif // KMp3WinBClass_H
