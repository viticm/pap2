//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3DecodeClass.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#ifndef KMp3DecodeClass_H
#define KMp3DecodeClass_H
//---------------------------------------------------------------------------
#include "KMp3SbtClass.h"
#include "KMp3InitClass.h"
#include "KMp3ScaleClass.h"
#include "KMp3HufClass.h"
#include "KMp3HeadClass.h"

class KL3AudioDecoder
{
public:
	KL3AudioDecoder();
	virtual ~KL3AudioDecoder();

	int DecodeHead(unsigned char *buf, MPEG_HEAD* head);
	int Init(MPEG_HEAD * h,
		int framebytes_arg,
		int reduction_code,
		int transform_code,
		int convert_code,
		int freq_limit);
	IN_OUT DecodeFrame(unsigned char *bs, unsigned char *pcm);
	void DecodeInfo(DEC_INFO *info);

	unsigned int bitget(int n);

private:
	void bitget_init(unsigned char *buf);
	void bitget_init_end(unsigned char *buf_end);
	int bitget_bits_used();
	int unpack_side_MPEG1();
	int unpack_side_MPEG2(int igr);
	void unpack_main(unsigned char *pcm, int igr);

	//---------------------------------------------------------------------------
	IN_OUT L3audio_decode_MPEG1(unsigned char *bs, unsigned char *pcm);
	IN_OUT L3audio_decode_MPEG2(unsigned char *bs, unsigned char *pcm);
	void Xform_mono(void *pcm, int igr);
	void Xform_dual(void *pcm, int igr);
	void Xform_dual_mono(void *pcm, int igr);
	void Xform_dual_right(void *pcm, int igr);

	typedef void (KL3AudioDecoder::*XFORM_FUNCTION) (void *pcm, int igr);

	XFORM_FUNCTION Xform;
	XFORM_FUNCTION xform_table[5];

	typedef void (KMp3Sbt::*SBT_FUNCTION) (float *sample, short *pcm, int ch);
	SBT_FUNCTION sbt_L3;

	SBT_FUNCTION sbt_table[2][3][2];

	typedef IN_OUT(KL3AudioDecoder::*DECODE_FUNCTION) (unsigned char *bs, unsigned char *pcm);
	DECODE_FUNCTION decode_function;


	//---------------------------------------------------------------------------
	/*-- global band tables */
	/*-- short portion is 3*x !! --*/
	int nBand[2][22];		/* [long/short][cb] */
	int sfBandIndex[2][22];		/* [long/short][cb] */
	//---------------------------------------------------------------------------
	/* get bits from bitstream in endian independent way */
	BITDAT bitdat;			/* global for inline use by Huff */
	//---------------------------------------------------------------------------
	DEC_INFO decinfo;	/* decode info */
	int band_limit_nsb;	/* global for hybrid */
	//---------------------------------------------------------------------------
	int mpeg25_flag;
	int iframe;

	int band_limit;
	int band_limit12;	// limit for sf band 12
	int band_limit21;	// limit for sf band 21

	int nsb_limit;
	int gain_adjust;	/* adjust gain e.g. cvt to mono */
	int id;
	int ncbl_mixed;		/* number of long cb's in mixed block 8 or 6 */
	int sr_index;
	int outvalues;
	int outbytes;
	int half_outbytes;
	int framebytes;
	int padframebytes;
	int crcbytes;
	int pad;
	int stereo_flag;
	int nchan;
	int ms_mode;
	int is_mode;
	unsigned int zero_level_pcm;

	/* cb_info[igr][ch], compute by dequant, used by joint */
	CB_INFO cb_info[2][2];
	IS_SF_INFO is_sf_info;	/* MPEG-2 intensity stereo */

	//---------------------------------------------------------------------------
	/* main data bit buffer */
#define NBUF (8*1024)
#define BUF_TRIGGER (NBUF-1500)
	unsigned char buf[NBUF];
	int buf_ptr0;
	int buf_ptr1;
	int main_pos_bit;

	SIDE_INFO side_info;

	SCALEFACT sf[2][2];	/* [gr][ch] */

	int nsamp[2][2];		/* must start = 0, for nsamp[igr_prev] */

	//---------------------------------------------------------------------------
	// sample union of int
	float sample[2][2][576];

	// hybrid out, sbt in
	float yout[18][32];

	KMp3Sbt sbt;

	int m_igr;

	KMp3Init mp3Init;

	KMp3Scale mp3Scale;

	KMp3Huf mp3Huf;

	KL3AudioDecodeHead mp3Head;
};

//---------------------------------------------------------------------------
#endif // KMp3DecodeClass_H
