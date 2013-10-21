//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Mdct.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#ifndef KMp3MdctClass_H
#define KMp3MdctClass_H
//---------------------------------------------------------------------------
typedef struct
{
	float *w;
	float *w2;
	void *coef;
}
IMDCT_INIT_BLOCK;

class KMp3Mdct
{
public:
	KMp3Mdct();

	IMDCT_INIT_BLOCK *imdct_init_addr_18();
	IMDCT_INIT_BLOCK *imdct_init_addr_6();
	void imdct18(float f[18]);
	void imdct6_3(float f[]);
protected:
private:
	// 18 point xform
	float mdct18w[18];
	float mdct18w2[9];
	float coef[9][4];

	float mdct6_3v[6];
	float mdct6_3v2[3];
	float coef87;

	IMDCT_INIT_BLOCK imdct_info_18;
	IMDCT_INIT_BLOCK imdct_info_6;

};

//---------------------------------------------------------------------------
#endif // KMp3MdctClass_H
