//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Scale.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#ifndef KMp3ScaleClass_H
#define KMp3ScaleClass_H
//---------------------------------------------------------------------------

class KL3AudioDecoder;
class KMp3Scale
{
public:
	void unpack_sf_sub_MPEG1(KL3AudioDecoder &Decoder, SCALEFACT * scalefac, GR * gr,
		int scfsi,	/* bit flag */
		int igr);
	void unpack_sf_sub_MPEG2(KL3AudioDecoder &Decoder, SCALEFACT sf[],	/* return intensity scale */
		GR * grdat,
		int is_and_ch, IS_SF_INFO * is_sf_info);
protected:
private:

};

//---------------------------------------------------------------------------
#endif // KMp3ScaleClass_H
