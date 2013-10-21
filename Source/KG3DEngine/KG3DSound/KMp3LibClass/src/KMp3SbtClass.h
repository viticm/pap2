//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3SbtClass.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#ifndef KMp3SbtClass_H
#define KMp3SbtClass_H
//---------------------------------------------------------------------------

#include "KMp3FdctClass.h"
#include "KMp3WinClass.h"
#include "KMp3WinBClass.h"

class KMp3Sbt
{
public:
	KMp3Sbt();
	~KMp3Sbt();

	void sbt_init();

	void sbt_mono_L3(float *sample, signed short *pcm, int ch);
	void sbt_dual_L3(float *sample, signed short *pcm, int ch);
	void sbt16_mono_L3(float *sample, signed short *pcm, int ch);
	void sbt16_dual_L3(float *sample, signed short *pcm, int ch);
	void sbt8_mono_L3(float *sample, signed short *pcm, int ch);
	void sbt8_dual_L3(float *sample, signed short *pcm, int ch);

	void sbtB_mono_L3(float *sample, unsigned char *pcm, int ch);
	void sbtB_dual_L3(float *sample, unsigned char *pcm, int ch);
	void sbtB16_mono_L3(float *sample, unsigned char *pcm, int ch);
	void sbtB16_dual_L3(float *sample, unsigned char *pcm, int ch);
	void sbtB8_mono_L3(float *sample, unsigned char *pcm, int ch);
	void sbtB8_dual_L3(float *sample, unsigned char *pcm, int ch);

private:
	void gencoef();

private:
	signed int vb_ptr;
	signed int vb2_ptr;
	float vbuf[512];
	float vbuf2[512];

	int first_pass;

	KMp3Fdct fdct;
	KMp3Win mp3win;
	KMp3WinB mp3winB;
};

//-----------------------------------------------------------------------
#endif // KMp3SbtClass_H
