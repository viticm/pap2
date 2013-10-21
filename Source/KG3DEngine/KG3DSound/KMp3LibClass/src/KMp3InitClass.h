//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Init.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#ifndef KMp3InitClass_H
#define KMp3InitClass_H
//---------------------------------------------------------------------------

#include "KMp3QuantClass.h"
#include "KMp3MsisClass.h"
#include "KMp3HybridClass.h"

typedef float PAIR[2];

class KMp3Init
{
public:
	int L3table_init();
	void hwin_init();
	void imdct_init();
	void msis_init();
	void msis_init_MPEG2();

public:
	KMp3Hybrid mp3Hybrid;
	KMp3Quant mp3Quant;
	KMp3Msis mp3Msis;
protected:
private:
};

//---------------------------------------------------------------------------
#endif // KMp3InitClass_H
