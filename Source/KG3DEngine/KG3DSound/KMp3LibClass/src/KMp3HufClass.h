//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Huf.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#ifndef KMp3HufClass_H
#define KMp3HufClass_H
//---------------------------------------------------------------------------

typedef int (* INTA2)[2];
typedef int (* INTA4)[4];

class KMp3Huf
{
public:
	void unpack_huff(BITDAT &bitdat, INTA2 xy, int n, int ntable);
	int  unpack_huff_quad(BITDAT &bitdat, INTA4 vwxy, int n, int nbits, int ntable);

private:
	unsigned int bitget_lb(BITDAT &bitdat, int n);
	unsigned int bitget2(BITDAT &bitdat, int n);
};

//---------------------------------------------------------------------------
#endif // KMp3HufClass_H
