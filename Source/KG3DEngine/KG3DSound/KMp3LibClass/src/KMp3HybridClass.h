//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Hybrid.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#ifndef KMp3HybridClass_H
#define KMp3HybridClass_H
//---------------------------------------------------------------------------

#include "KMp3MdctClass.h"

typedef float ARRAY36[36];

class KMp3Hybrid
{
public:
	KMp3Hybrid();
	ARRAY36* hwin_init_addr();
	int  hybrid(int &band_limit_nsb, float xin[], float xprev[], float y[18][32], int btype, int nlong, int ntot, int nprev);
	int  hybrid_sum(float xin[], float xin_left[], float y[18][32], int btype, int nlong, int ntot);
	void sum_f_bands(float a[], float b[], int n);
	void FreqInvert(float y[18][32], int n);

public:
	KMp3Mdct mp3Mdct;
private:
	float win[4][36]; /*-- windows by block type --*/

};

//---------------------------------------------------------------------------
#endif // KMp3HybridClass_H
