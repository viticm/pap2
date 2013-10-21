//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3FdctClass.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#ifndef KMp3Fdct_H
#define KMp3Fdct_H
//---------------------------------------------------------------------------

class KMp3Fdct
{
public:
	KMp3Fdct();
	float *dct_coef_addr();
	void fdct32(float *, float *);
	void fdct16(float *, float *);
	void fdct8(float *, float *);

protected:
private:
	void forward_bf(int m, int n, float x[], float f[], float coef[]);
	void back_bf(int m, int n, float x[], float f[]);

private:
	float coef32[31];	/* 32 pt dct coefs */
};

//---------------------------------------------------------------------------
#endif // KMp3Fdct_H
