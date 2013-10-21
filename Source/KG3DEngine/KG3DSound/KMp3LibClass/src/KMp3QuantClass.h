//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Quant.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#ifndef KMp3QuantClass_H
#define KMp3QuantClass_H
//---------------------------------------------------------------------------
typedef float LS[4][32];

class KMp3Quant
{
public:
	KMp3Quant();

	float*		quant_init_global_addr();
	LS*			quant_init_scale_addr();
	float*		quant_init_pow_addr();
	float*		quant_init_subblock_addr();
	void		dequant(int nBand[2][22], float sample[],
		int * nsamp,
		SCALEFACT * sf,
		GR * gr,
		CB_INFO * cb_info,
		int ncbl_mixed);

protected:
private:

	/* 8 bit plus 2 lookup x = pow(2.0, 0.25*(global_gain-210)) */
	/* two extra slots to do 1/sqrt(2) scaling for ms */
	/* 4 extra slots to do 1/2 scaling for cvt to mono */
	float look_global[256 + 2 + 4];

	/*-------- scaling lookup
	x = pow(2.0, -0.5*(1+scalefact_scale)*scalefac + preemp)
	look_scale[scalefact_scale][preemp][scalefac]
	-----------------------*/
	float look_scale[2][4][32];
	typedef float LS[4][32];

	/*--- iSample**(4/3) lookup, -32<=i<=31 ---*/
#define ISMAX 32
	float look_pow[2 * ISMAX];

	/*-- pow(2.0, -0.25*8.0*subblock_gain) --*/
	float look_subblock[8];

	/*-- reorder buffer ---*/
	float re_buf[192][3];
	typedef float ARRAY3[3];

	//---------------------------------------------------------------------------
	typedef union
	{
		int s;
		float x;
	}
	SAMPLE;

};


//---------------------------------------------------------------------------
#endif // KMp3QuantClass_H
