//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Msis.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#ifndef KMp3MsisClass_H
#define KMp3MsisClass_H
//---------------------------------------------------------------------------
typedef float ARRAY2[2];
typedef float ARRAY8_2[8][2];
typedef float ARRAY2_64_2[2][64][2];
typedef float (* ARRAY1152)[1152];

class KMp3Msis
{
public:
	KMp3Msis();
	//---------------------------------------------------------------------------
	ARRAY2 *alias_init_addr();
	ARRAY8_2 *msis_init_addr();
	ARRAY2_64_2 *msis_init_addr_MPEG2();
	//---------------------------------------------------------------------------
	void antialias(float x[], int n);
	void ms_process(ARRAY1152 x, int n);
	void is_process_MPEG1(int nBand[2][22], int sfBandIndex[2][22], ARRAY1152 x, /* intensity stereo */
		SCALEFACT * sf,
		CB_INFO cb_info[2], /* [ch] */
		int nsamp,
		int ms_mode);
	void is_process_MPEG2(int nBand[2][22], int sfBandIndex[2][22], ARRAY1152 x, /* intensity stereo */
		SCALEFACT * sf,
		CB_INFO cb_info[2], /* [ch] */
		IS_SF_INFO * is_sf_info,
		int nsamp,
		int ms_mode);

protected:
private:
	float csa[8][2];	/* antialias */

	/* intensity stereo */
	float lr[2][8][2];	/* [ms_mode 0/1][sf][left/right]  */
	float lr2[2][2][64][2];

};

//---------------------------------------------------------------------------
#endif // KMp3MsisClass_H
