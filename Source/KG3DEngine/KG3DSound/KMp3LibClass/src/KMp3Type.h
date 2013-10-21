//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Type.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//			Layer III structures
//			*** Layer III is 32 bit only          ***
//			*** Layer III code assumes 32 bit int ***
//---------------------------------------------------------------------------
#ifndef KMp3Type_H
#define KMp3Type_H
//---------------------------------------------------------------------------
// mpeg audio header
typedef struct
{
	int sync;			// 0xFFF=mpeg1.0 & 2.0  0xFFE=mpeg2.5
	int ver;			// 1=mpeg1.0 0=mpeg2.0
	int layer;			// 1=LayerIII 2=LayerII 3=LayerI
	int prot;			// error protection  0=yes, 1=no
	int br_index;		// bitrate index
	int sr_index;		// sampling rate
	int pad;			// padding
	int extension;		// extension
	int mode;			// 0=stereo 1=joint stereo 2=dual channel 3=single channel
	int mode_ext;		// 0=MPG_MD_LR_LR 1=MPG_MD_LR_I 2=MPG_MD_MS_LR 3=MPG_MD_MS_I
	int copyright;		// 0=no 1=yes
	int original;		// 0=no 1=yes
	int emphasis;		// 0=none 1=50/15 microsecs 2=reserved 3=CCITT J 17
}
MPEG_HEAD;
//---------------------------------------------------------------------------
// mp3 decode info
typedef struct
{
	int channels;		// channels = 1,2
	int outvalues;		// out value
	int samprate;		// sample rate
	int bits;			// bits per sample = 8,16,24,32
	int framebytes;		// frame size in bytes
}
DEC_INFO;
//---------------------------------------------------------------------------
// portable mpeg audio decoder, decoder functions
typedef struct
{
	int in_bytes;
	int out_bytes;
}
IN_OUT;
//---------------------------------------------------------------------------
// bit dat
typedef struct
{
	unsigned int bitbuf;
	int bits;
	unsigned char *bs_ptr;
	unsigned char *bs_ptr0;
	unsigned char *bs_ptr_end;	// optional for overrun test
}
BITDAT;
//---------------------------------------------------------------------------
// gr
typedef struct
{
	int part2_3_length;
	int big_values;
	int global_gain;
	int scalefac_compress;
	int window_switching_flag;
	int block_type;
	int mixed_block_flag;
	int table_select[3];
	int subblock_gain[3];
	int region0_count;
	int region1_count;
	int preflag;
	int scalefac_scale;
	int count1table_select;
}
GR;
//---------------------------------------------------------------------------
// side info
typedef struct
{
	int mode;
	int mode_ext;
	int main_data_begin;	// beginning, not end, my spec wrong
	int private_bits;
	int scfsi[2];			// 4 bit flags [ch]
	GR gr[2][2];			// [gran][ch]
}
SIDE_INFO;
//---------------------------------------------------------------------------
// scale factors 
// check dimensions - need 21 long, 3*12 short
// plus extra for implicit sf=0 above highest cb
typedef struct
{
	int l[23];			/* [cb] */
	int s[3][13];		/* [window][cb] */
}
SCALEFACT;
//---------------------------------------------------------------------------
// cb info
typedef struct
{
	int cbtype;			/* long=0 short=1 */
	int cbmax;			/* max crit band */
	int cbs0;			/* short band start index 0 3 12 (12=no shorts */
	int ncbl;			/* number long cb's 0 8 21 */
	int cbmax_s[3];		/* cbmax by individual short blocks */
}
CB_INFO;
//---------------------------------------------------------------------------
// scale factor infor for MPEG2 intensity stereo
typedef struct
{
	int nr[3];
	int slen[3];
	int intensity_scale;
}
IS_SF_INFO;
//---------------------------------------------------------------------------
#endif // KMp3Type_H
