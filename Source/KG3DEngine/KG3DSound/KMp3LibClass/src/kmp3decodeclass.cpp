//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Decode.cpp
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//			Decode Layer III
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <memory.h>
#include <string.h>
#include "KMp3Type.h"
#include "KMp3InitClass.h"
#include "KMp3MsisClass.h"
#include "KMp3QuantClass.h"
#include "KMp3HybridClass.h"
#include "KMp3HufClass.h"
#include "KMp3ScaleClass.h"
#include "KMp3SbtClass.h"
#include "KMp3DecodeClass.h"

static const int mp_sr20_table[2][4] =
{{441, 480, 320, -999}, {882, 960, 640, -999}};

static const int mp_br_tableL3[2][16] =
{{0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0},	/* mpeg 2 */
{0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0}};


KL3AudioDecoder::KL3AudioDecoder()
{
	memset(nBand, 0, sizeof(nBand));
	memset(sfBandIndex, 0, sizeof(sfBandIndex));

	memset(&bitdat, 0, sizeof(bitdat));
	memset(&decinfo, 0, sizeof(decinfo));

	band_limit_nsb = 32;

	mpeg25_flag = 0;
	iframe = 0;

	band_limit = (576);
	band_limit12 = (576);	// limit for sf band 12
	band_limit21 = (576);	// limit for sf band 21

	nsb_limit = 32;
	gain_adjust = 0;	/* adjust gain e.g. cvt to mono */
	id = 0;
	ncbl_mixed = 0;		/* number of long cb's in mixed block 8 or 6 */
	sr_index = 0;
	outvalues = 0;
	outbytes = 0;
	half_outbytes = 0;
	framebytes = 0;
	padframebytes = 0;
	crcbytes = 0;
	pad = 0;
	stereo_flag = 0;
	nchan = 0;
	ms_mode = 0;
	is_mode = 0;
	zero_level_pcm = 0;

	/* cb_info[igr][ch], compute by dequant, used by joint */
	memset(cb_info, 0, sizeof(cb_info));
	memset(&is_sf_info, 0, sizeof(is_sf_info));

	memset(buf, 0, sizeof(buf));
	buf_ptr0 = 0;
	buf_ptr1 = 0;
	main_pos_bit = 0;

	memset(&side_info, 0, sizeof(side_info));

	memset(sf, 0, 2 * 2 * sizeof(sf));

	memset(nsamp, 0, sizeof(nsamp));

	//---------------------------------------------------------------------------
	// sample union of int
	memset(sample, 0, sizeof(sample));

	// hybrid out, sbt in
	memset(yout, 0, sizeof(yout));

	m_igr = 0;

/*
	band_limit_nsb = 32;

	band_limit = (576);
	band_limit12 = (576);	// limit for sf band 12
	band_limit21 = (576);	// limit for sf band 21

	nsb_limit = 32;
	gain_adjust = 0;
	zero_level_pcm = 0;

	buf_ptr0 = 0;
	buf_ptr1 = 0;

	m_igr = 0;
*/

	Xform = &KL3AudioDecoder::Xform_dual;
	xform_table[0] = &KL3AudioDecoder::Xform_mono,
	xform_table[1] = &KL3AudioDecoder::Xform_dual,
	xform_table[2] = &KL3AudioDecoder::Xform_dual_mono,
	xform_table[3] = &KL3AudioDecoder::Xform_mono,
	xform_table[4] = &KL3AudioDecoder::Xform_dual_right,

	sbt_L3 = &KMp3Sbt::sbt_dual_L3;

	sbt_table[0][0][0] = (SBT_FUNCTION)&KMp3Sbt::sbt_mono_L3;
	sbt_table[0][0][1] = (SBT_FUNCTION)&KMp3Sbt::sbt_dual_L3;

	sbt_table[0][1][0] = (SBT_FUNCTION)&KMp3Sbt::sbt16_mono_L3;
	sbt_table[0][1][1] = (SBT_FUNCTION)&KMp3Sbt::sbt16_dual_L3;

	sbt_table[0][2][0] = (SBT_FUNCTION)&KMp3Sbt::sbt8_mono_L3;
	sbt_table[0][2][1] = (SBT_FUNCTION)&KMp3Sbt::sbt8_dual_L3;

	sbt_table[1][0][0] = (SBT_FUNCTION)&KMp3Sbt::sbtB_mono_L3;
	sbt_table[1][0][1] = (SBT_FUNCTION)&KMp3Sbt::sbtB_dual_L3;

	sbt_table[1][1][0] = (SBT_FUNCTION)&KMp3Sbt::sbtB16_mono_L3;
	sbt_table[1][1][1] = (SBT_FUNCTION)&KMp3Sbt::sbtB16_dual_L3;

	sbt_table[1][2][0] = (SBT_FUNCTION)&KMp3Sbt::sbtB8_mono_L3;
	sbt_table[1][2][1] = (SBT_FUNCTION)&KMp3Sbt::sbtB8_dual_L3;

	decode_function = &KL3AudioDecoder::L3audio_decode_MPEG1;

	
}

KL3AudioDecoder::~KL3AudioDecoder()
{

}

void KL3AudioDecoder::bitget_init(unsigned char *buf)
{
	bitdat.bs_ptr0 = bitdat.bs_ptr = buf;
	bitdat.bits = 0;
	bitdat.bitbuf = 0;
}

//---------------------------------------------------------------------------
/*------------- initialize bit getter -------------*/
void KL3AudioDecoder::bitget_init_end(unsigned char *buf_end)
{
	bitdat.bs_ptr_end = buf_end;
}

//---------------------------------------------------------------------------
/*------------- get n bits from bitstream -------------*/
int KL3AudioDecoder::bitget_bits_used()
{
	int n;			/* compute bits used from last init call */

	n = ((bitdat.bs_ptr - bitdat.bs_ptr0) << 3) - bitdat.bits;
	return n;
}

//---------------------------------------------------------------------------
/*------------- get n bits from bitstream -------------*/
unsigned int KL3AudioDecoder::bitget(int n)
{
	unsigned int x;

	if (bitdat.bits < n)
	{				/* refill bit buf if necessary */
		while (bitdat.bits <= 24)
		{
			bitdat.bitbuf = (bitdat.bitbuf << 8) | *bitdat.bs_ptr++;
			bitdat.bits += 8;
		}
	}
	bitdat.bits -= n;
	x = bitdat.bitbuf >> bitdat.bits;
	bitdat.bitbuf -= x << bitdat.bits;
	return x;
}

//---------------------------------------------------------------------------
void KL3AudioDecoder::Xform_mono(void *pcm, int igr)
{
	int igr_prev, n1, n2;

	/*--- hybrid + sbt ---*/
	n1 = n2 = nsamp[igr][0];	/* total number bands */
	if (side_info.gr[igr][0].block_type == 2)
	{				/* long bands */
		n1 = 0;
		if (side_info.gr[igr][0].mixed_block_flag)
			n1 = sfBandIndex[0][ncbl_mixed - 1];
	}
	if (n1 > band_limit)
		n1 = band_limit;
	if (n2 > band_limit)
		n2 = band_limit;
	igr_prev = igr ^ 1;

	nsamp[igr][0] = mp3Init.mp3Hybrid.hybrid(band_limit_nsb, sample[0][igr], sample[0][igr_prev],
		yout, side_info.gr[igr][0].block_type, n1, n2, nsamp[igr_prev][0]);
	mp3Init.mp3Hybrid.FreqInvert(yout, nsamp[igr][0]);
	(sbt.*sbt_L3)(yout[0], (short*)pcm, 0);

}
//---------------------------------------------------------------------------
void KL3AudioDecoder::Xform_dual_right(void *pcm, int igr)
{
	int igr_prev, n1, n2;

	/*--- hybrid + sbt ---*/
	n1 = n2 = nsamp[igr][1];	/* total number bands */
	if (side_info.gr[igr][1].block_type == 2)
	{				/* long bands */
		n1 = 0;
		if (side_info.gr[igr][1].mixed_block_flag)
			n1 = sfBandIndex[0][ncbl_mixed - 1];
	}
	if (n1 > band_limit)
		n1 = band_limit;
	if (n2 > band_limit)
		n2 = band_limit;
	igr_prev = igr ^ 1;
	nsamp[igr][1] = mp3Init.mp3Hybrid.hybrid(band_limit_nsb, sample[1][igr], sample[1][igr_prev],
		yout, side_info.gr[igr][1].block_type, n1, n2, nsamp[igr_prev][1]);
	mp3Init.mp3Hybrid.FreqInvert(yout, nsamp[igr][1]);
	(sbt.*sbt_L3)(yout[0], (short*)pcm, 0);

}
//---------------------------------------------------------------------------
void KL3AudioDecoder::Xform_dual(void *pcm, int igr)
{
	int ch;
	int igr_prev, n1, n2;

	/*--- hybrid + sbt ---*/
	igr_prev = igr ^ 1;
	for (ch = 0; ch < nchan; ch++)
	{
		n1 = n2 = nsamp[igr][ch];	/* total number bands */
		if (side_info.gr[igr][ch].block_type == 2)
		{				/* long bands */
			n1 = 0;
			if (side_info.gr[igr][ch].mixed_block_flag)
				n1 = sfBandIndex[0][ncbl_mixed - 1];
		}
		if (n1 > band_limit)
			n1 = band_limit;
		if (n2 > band_limit)
			n2 = band_limit;
		nsamp[igr][ch] = mp3Init.mp3Hybrid.hybrid(band_limit_nsb, sample[ch][igr], sample[ch][igr_prev],
			yout, side_info.gr[igr][ch].block_type, n1, n2, nsamp[igr_prev][ch]);
		mp3Init.mp3Hybrid.FreqInvert(yout, nsamp[igr][ch]);
		(sbt.*sbt_L3)(yout[0], (short*)pcm, ch);
	}

}
//---------------------------------------------------------------------------
void KL3AudioDecoder::Xform_dual_mono(void *pcm, int igr)
{
	int igr_prev, n1, n2, n3;

	/*--- hybrid + sbt ---*/
	igr_prev = igr ^ 1;
	if ((side_info.gr[igr][0].block_type == side_info.gr[igr][1].block_type)
		&& (side_info.gr[igr][0].mixed_block_flag == 0)
		&& (side_info.gr[igr][1].mixed_block_flag == 0))
	{

		n2 = nsamp[igr][0];	/* total number bands max of L R */
		if (n2 < nsamp[igr][1])
			n2 = nsamp[igr][1];
		if (n2 > band_limit)
			n2 = band_limit;
		n1 = n2;			/* n1 = number long bands */
		if (side_info.gr[igr][0].block_type == 2)
			n1 = 0;
		mp3Init.mp3Hybrid.sum_f_bands(sample[0][igr], sample[1][igr], n2);
		n3 = nsamp[igr][0] = mp3Init.mp3Hybrid.hybrid(band_limit_nsb, sample[0][igr], sample[0][igr_prev],
			yout, side_info.gr[igr][0].block_type, n1, n2, nsamp[igr_prev][0]);
	}
	else
	{				/* transform and then sum (not tested - never happens in test) */
		/*-- left chan --*/
		n1 = n2 = nsamp[igr][0];	/* total number bands */
		if (side_info.gr[igr][0].block_type == 2)
		{
			n1 = 0;		/* long bands */
			if (side_info.gr[igr][0].mixed_block_flag)
				n1 = sfBandIndex[0][ncbl_mixed - 1];
		}
		n3 = nsamp[igr][0] = mp3Init.mp3Hybrid.hybrid(band_limit_nsb, sample[0][igr], sample[0][igr_prev],
			yout, side_info.gr[igr][0].block_type, n1, n2, nsamp[igr_prev][0]);
		/*-- right chan --*/
		n1 = n2 = nsamp[igr][1];	/* total number bands */
		if (side_info.gr[igr][1].block_type == 2)
		{
			n1 = 0;		/* long bands */
			if (side_info.gr[igr][1].mixed_block_flag)
				n1 = sfBandIndex[0][ncbl_mixed - 1];
		}
		nsamp[igr][1] = mp3Init.mp3Hybrid.hybrid_sum(sample[1][igr], sample[0][igr],
			yout, side_info.gr[igr][1].block_type, n1, n2);
		if (n3 < nsamp[igr][1])
			n1 = nsamp[igr][1];
	}

	/*--------*/
	mp3Init.mp3Hybrid.FreqInvert(yout, n3);
	(sbt.*sbt_L3)(yout[0], (short*)pcm, 0);

}
//---------------------------------------------------------------------------
int KL3AudioDecoder::unpack_side_MPEG1()
{
	int prot;
	int br_index;
	int igr, ch;
	int side_bytes;

	/* decode partial header plus initial side info */
	/* at entry bit getter points at id, sync skipped by caller */

	id = bitget(1);		/* id */
	bitget(2);			/* skip layer */
	prot = bitget(1);		/* bitget prot bit */
	br_index = bitget(4);
	sr_index = bitget(2);
	pad = bitget(1);
	bitget(1);			/* skip to mode */
	side_info.mode = bitget(2);	/* mode */
	side_info.mode_ext = bitget(2);	/* mode ext */

	if (side_info.mode != 1)
		side_info.mode_ext = 0;

	/* adjust global gain in ms mode to avoid having to mult by 1/sqrt(2) */
	ms_mode = side_info.mode_ext >> 1;
	is_mode = side_info.mode_ext & 1;


	crcbytes = 0;
	if (prot)
		bitget(4);		/* skip to data */
	else
	{
		bitget(20);		/* skip crc */
		crcbytes = 2;
	}

	if (br_index > 0)		/* framebytes fixed for free format */
	{
		framebytes =
			2880 * mp_br_tableL3[id][br_index] / mp_sr20_table[id][sr_index];
	}

	side_info.main_data_begin = bitget(9);
	if (side_info.mode == 3)
	{
		side_info.private_bits = bitget(5);
		nchan = 1;
		stereo_flag = 0;
		side_bytes = (4 + 17);
		/*-- with header --*/
	}
	else
	{
		side_info.private_bits = bitget(3);
		nchan = 2;
		stereo_flag = 1;
		side_bytes = (4 + 32);
		/*-- with header --*/
	}
	for (ch = 0; ch < nchan; ch++)
		side_info.scfsi[ch] = bitget(4);
	/* this always 0 (both igr) for short blocks */

	for (igr = 0; igr < 2; igr++)
	{
		for (ch = 0; ch < nchan; ch++)
		{
			side_info.gr[igr][ch].part2_3_length = bitget(12);
			side_info.gr[igr][ch].big_values = bitget(9);
			side_info.gr[igr][ch].global_gain = bitget(8) + gain_adjust;
			if (ms_mode)
				side_info.gr[igr][ch].global_gain -= 2;
			side_info.gr[igr][ch].scalefac_compress = bitget(4);
			side_info.gr[igr][ch].window_switching_flag = bitget(1);
			if (side_info.gr[igr][ch].window_switching_flag)
			{
				side_info.gr[igr][ch].block_type = bitget(2);
				side_info.gr[igr][ch].mixed_block_flag = bitget(1);
				side_info.gr[igr][ch].table_select[0] = bitget(5);
				side_info.gr[igr][ch].table_select[1] = bitget(5);
				side_info.gr[igr][ch].subblock_gain[0] = bitget(3);
				side_info.gr[igr][ch].subblock_gain[1] = bitget(3);
				side_info.gr[igr][ch].subblock_gain[2] = bitget(3);
				/* region count set in terms of long block cb's/bands */
				/* r1 set so r0+r1+1 = 21 (lookup produces 576 bands ) */
				/* if(window_switching_flag) always 36 samples in region0 */
				side_info.gr[igr][ch].region0_count = (8 - 1);	/* 36 samples */
				side_info.gr[igr][ch].region1_count = 20 - (8 - 1);
			}
			else
			{
				side_info.gr[igr][ch].mixed_block_flag = 0;
				side_info.gr[igr][ch].block_type = 0;
				side_info.gr[igr][ch].table_select[0] = bitget(5);
				side_info.gr[igr][ch].table_select[1] = bitget(5);
				side_info.gr[igr][ch].table_select[2] = bitget(5);
				side_info.gr[igr][ch].region0_count = bitget(4);
				side_info.gr[igr][ch].region1_count = bitget(3);
			}
			side_info.gr[igr][ch].preflag = bitget(1);
			side_info.gr[igr][ch].scalefac_scale = bitget(1);
			side_info.gr[igr][ch].count1table_select = bitget(1);
		}
	}

	/* return  bytes in header + side info */
	return side_bytes;
}
//---------------------------------------------------------------------------
int KL3AudioDecoder::unpack_side_MPEG2(int igr)
{
	int prot;
	int br_index;
	int ch;
	int side_bytes;

	/* decode partial header plus initial side info */
	/* at entry bit getter points at id, sync skipped by caller */

	id = bitget(1);		/* id */
	bitget(2);			/* skip layer */
	prot = bitget(1);		/* bitget prot bit */
	br_index = bitget(4);
	sr_index = bitget(2);
	pad = bitget(1);
	bitget(1);			/* skip to mode */
	side_info.mode = bitget(2);	/* mode */
	side_info.mode_ext = bitget(2);	/* mode ext */

	if (side_info.mode != 1)
		side_info.mode_ext = 0;

	/* adjust global gain in ms mode to avoid having to mult by 1/sqrt(2) */
	ms_mode = side_info.mode_ext >> 1;
	is_mode = side_info.mode_ext & 1;

	crcbytes = 0;
	if (prot)
		bitget(4);		/* skip to data */
	else
	{
		bitget(20);		/* skip crc */
		crcbytes = 2;
	}

	if (br_index > 0)
	{				/* framebytes fixed for free format */
		if (mpeg25_flag == 0)
		{
			framebytes =
				1440 * mp_br_tableL3[id][br_index] / mp_sr20_table[id][sr_index];
		}
		else
		{
			framebytes =
				2880 * mp_br_tableL3[id][br_index] / mp_sr20_table[id][sr_index];
			//if( sr_index == 2 ) return 0;  // fail mpeg25 8khz
		}
	}
	side_info.main_data_begin = bitget(8);
	if (side_info.mode == 3)
	{
		side_info.private_bits = bitget(1);
		nchan = 1;
		stereo_flag = 0;
		side_bytes = (4 + 9);
		/*-- with header --*/
	}
	else
	{
		side_info.private_bits = bitget(2);
		nchan = 2;
		stereo_flag = 1;
		side_bytes = (4 + 17);
		/*-- with header --*/
	}
	side_info.scfsi[1] = side_info.scfsi[0] = 0;


	for (ch = 0; ch < nchan; ch++)
	{
		side_info.gr[igr][ch].part2_3_length = bitget(12);
		side_info.gr[igr][ch].big_values = bitget(9);
		side_info.gr[igr][ch].global_gain = bitget(8) + gain_adjust;
		if (ms_mode)
			side_info.gr[igr][ch].global_gain -= 2;
		side_info.gr[igr][ch].scalefac_compress = bitget(9);
		side_info.gr[igr][ch].window_switching_flag = bitget(1);
		if (side_info.gr[igr][ch].window_switching_flag)
		{
			side_info.gr[igr][ch].block_type = bitget(2);
			side_info.gr[igr][ch].mixed_block_flag = bitget(1);
			side_info.gr[igr][ch].table_select[0] = bitget(5);
			side_info.gr[igr][ch].table_select[1] = bitget(5);
			side_info.gr[igr][ch].subblock_gain[0] = bitget(3);
			side_info.gr[igr][ch].subblock_gain[1] = bitget(3);
			side_info.gr[igr][ch].subblock_gain[2] = bitget(3);
			/* region count set in terms of long block cb's/bands  */
			/* r1 set so r0+r1+1 = 21 (lookup produces 576 bands ) */
			/* bt=1 or 3       54 samples */
			/* bt=2 mixed=0    36 samples */
			/* bt=2 mixed=1    54 (8 long sf) samples? or maybe 36 */
			/* region0 discussion says 54 but this would mix long */
			/* and short in region0 if scale factors switch */
			/* at band 36 (6 long scale factors) */
			if ((side_info.gr[igr][ch].block_type == 2))
			{
				side_info.gr[igr][ch].region0_count = (6 - 1);	/* 36 samples */
				side_info.gr[igr][ch].region1_count = 20 - (6 - 1);
			}
			else
			{	/* long block type 1 or 3 */
				side_info.gr[igr][ch].region0_count = (8 - 1);	/* 54 samples */
				side_info.gr[igr][ch].region1_count = 20 - (8 - 1);
			}
		}
		else
		{
			side_info.gr[igr][ch].mixed_block_flag = 0;
			side_info.gr[igr][ch].block_type = 0;
			side_info.gr[igr][ch].table_select[0] = bitget(5);
			side_info.gr[igr][ch].table_select[1] = bitget(5);
			side_info.gr[igr][ch].table_select[2] = bitget(5);
			side_info.gr[igr][ch].region0_count = bitget(4);
			side_info.gr[igr][ch].region1_count = bitget(3);
		}
		side_info.gr[igr][ch].preflag = 0;
		side_info.gr[igr][ch].scalefac_scale = bitget(1);
		side_info.gr[igr][ch].count1table_select = bitget(1);
	}

	/* return  bytes in header + side info */
	return side_bytes;
}
//---------------------------------------------------------------------------
void KL3AudioDecoder::unpack_main(unsigned char *pcm, int igr)
{
	int ch;
	int bit0;
	int n1, n2, n3, n4, nn2, nn3;
	int nn4;
	int qbits;
	int m0;


	for (ch = 0; ch < nchan; ch++)
	{
		bitget_init(buf + (main_pos_bit >> 3));
		bit0 = (main_pos_bit & 7);
		if (bit0)
			bitget(bit0);
		main_pos_bit += side_info.gr[igr][ch].part2_3_length;
		bitget_init_end(buf + ((main_pos_bit + 39) >> 3));

		/*-- scale factors --*/
		if (id)
		{
			mp3Scale.unpack_sf_sub_MPEG1(*this, &sf[igr][ch],
				&side_info.gr[igr][ch], side_info.scfsi[ch], igr);
		}
		else
		{
			mp3Scale.unpack_sf_sub_MPEG2(*this, &sf[igr][ch],
				&side_info.gr[igr][ch], is_mode & ch, &is_sf_info);
		}

		/*--- huff data ---*/
		n1 = sfBandIndex[0][side_info.gr[igr][ch].region0_count];
		n2 = sfBandIndex[0][side_info.gr[igr][ch].region0_count
			+ side_info.gr[igr][ch].region1_count + 1];
		n3 = side_info.gr[igr][ch].big_values;
		n3 = n3 + n3;

		if (n3 > band_limit)
			n3 = band_limit;
		if (n2 > n3)
			n2 = n3;
		if (n1 > n3)
			n1 = n3;
		nn3 = n3 - n2;
		nn2 = n2 - n1;

		mp3Huf.unpack_huff(bitdat, (INTA2)(sample[ch][igr] + 0 ),  n1, side_info.gr[igr][ch].table_select[0]);
		mp3Huf.unpack_huff(bitdat, (INTA2)(sample[ch][igr] + n1), nn2, side_info.gr[igr][ch].table_select[1]);
		mp3Huf.unpack_huff(bitdat, (INTA2)(sample[ch][igr] + n2), nn3, side_info.gr[igr][ch].table_select[2]);

		qbits = side_info.gr[igr][ch].part2_3_length - (bitget_bits_used() - bit0);

		nn4 = mp3Huf.unpack_huff_quad(bitdat, (INTA4)(sample[ch][igr] + n3), band_limit - n3, qbits,
			side_info.gr[igr][ch].count1table_select);

		n4 = n3 + nn4;
		nsamp[igr][ch] = n4;

		//limit n4 or allow deqaunt to sf band 22
		if (side_info.gr[igr][ch].block_type == 2)
		{
			n4 = (n4 < band_limit12)? n4: band_limit12;
		}
		else
		{
			n4 = (n4 < band_limit21)? n4: band_limit21;
		}

		if (n4 < 576)
		{
			memset(sample[ch][igr] + n4, 0, sizeof(float) * (576 - n4));
		}

		if (bitdat.bs_ptr > bitdat.bs_ptr_end)
		{	// bad data overrun
			memset(sample[ch][igr], 0, sizeof(float) * (576));
		}
	}

	/*--- dequant ---*/
	for (ch = 0; ch < nchan; ch++)
	{
		mp3Init.mp3Quant.dequant(nBand, sample[ch][igr],
			&nsamp[igr][ch],	/* nsamp updated for shorts */
			&sf[igr][ch], &side_info.gr[igr][ch],
			&cb_info[igr][ch], ncbl_mixed);
	}

	/*--- ms stereo processing  ---*/
	if (ms_mode)
	{
		if (is_mode == 0)
		{	/* process to longer of left/right */
			m0 = nsamp[igr][0];
			if (m0 < nsamp[igr][1])
				m0 = nsamp[igr][1];
		}
		else
		{	/* process to last cb in right */
			m0 = sfBandIndex[cb_info[igr][1].cbtype][cb_info[igr][1].cbmax];
		}
		mp3Init.mp3Msis.ms_process((ARRAY1152)sample[0][igr], m0);
	}

	/*--- is stereo processing  ---*/
	if (is_mode)
	{
		if (id)
		{
			mp3Init.mp3Msis.is_process_MPEG1(nBand, sfBandIndex, (ARRAY1152)sample[0][igr], &sf[igr][1],
				cb_info[igr], nsamp[igr][0], ms_mode);
		}
		else
		{
			mp3Init.mp3Msis.is_process_MPEG2(nBand, sfBandIndex, (ARRAY1152)sample[0][igr], &sf[igr][1],
				cb_info[igr], &is_sf_info,
				nsamp[igr][0], ms_mode);
		}
	}

	/*-- adjust ms and is modes to max of left/right */
	if (side_info.mode_ext)
	{
		if (nsamp[igr][0] < nsamp[igr][1])
			nsamp[igr][0] = nsamp[igr][1];
		else
			nsamp[igr][1] = nsamp[igr][0];
	}

	/*--- antialias ---*/
	for (ch = 0; ch < nchan; ch++)
	{
		if (cb_info[igr][ch].ncbl == 0)
			continue;		/* have no long blocks */
		if (side_info.gr[igr][ch].mixed_block_flag)
			n1 = 1;		/* 1 -> 36 samples */
		else
			n1 = (nsamp[igr][ch] + 7) / 18;
		if (n1 > 31)
			n1 = 31;
		mp3Init.mp3Msis.antialias(sample[ch][igr], n1);
		n1 = 18 * n1 + 8;		/* update number of samples */
		if (n1 > nsamp[igr][ch])
			nsamp[igr][ch] = n1;
	}



	/*--- hybrid + sbt ---*/
	(this->*Xform)(pcm, igr);


	/*-- done --*/
}
//---------------------------------------------------------------------------
IN_OUT KL3AudioDecoder::L3audio_decode_MPEG1(unsigned char *bs, unsigned char *pcm)
{
	int sync;
	IN_OUT in_out;
	int side_bytes;
	int nbytes;

	iframe++;

	bitget_init(bs);		/* initialize bit getter */

	/* test sync */
	in_out.in_bytes = 0;		/* assume fail */
	in_out.out_bytes = 0;
	sync = bitget(12);

	if (sync != 0xFFF)
		return in_out;		/* sync fail */

	/*-- unpack side info --*/
	side_bytes = unpack_side_MPEG1();
	padframebytes = framebytes + pad;
	in_out.in_bytes = padframebytes;

	/*-- load main data and update buf pointer --*/
	/*------------------------------------------- 
	if start point < 0, must just cycle decoder 
	if jumping into middle of stream, 
	w---------------------------------------------*/
	buf_ptr0 = buf_ptr1 - side_info.main_data_begin;	/* decode start point */
	if (buf_ptr1 > BUF_TRIGGER)
	{				/* shift buffer */
		memmove(buf, buf + buf_ptr0, side_info.main_data_begin);
		buf_ptr0 = 0;
		buf_ptr1 = side_info.main_data_begin;
	}
	nbytes = padframebytes - side_bytes - crcbytes;

	// RAK: This is no bueno. :-(
	if (nbytes < 0 || nbytes > NBUF)
	{
		in_out.in_bytes = 0;
		return in_out;
	}

	memmove(buf + buf_ptr1, bs + side_bytes + crcbytes, nbytes);
	buf_ptr1 += nbytes;

	if (buf_ptr0 >= 0)
	{
		main_pos_bit = buf_ptr0 << 3;
		unpack_main(pcm, 0);
		unpack_main(pcm + half_outbytes, 1);
		in_out.out_bytes = outbytes;
	}
	else
	{
		memset(pcm, zero_level_pcm, outbytes);	/* fill out skipped frames */
		in_out.out_bytes = outbytes;
	}

	return in_out;
}
//---------------------------------------------------------------------------
IN_OUT KL3AudioDecoder::L3audio_decode_MPEG2(unsigned char *bs, unsigned char *pcm)
{
	int sync;
	IN_OUT in_out;
	int side_bytes;
	int nbytes;

	iframe++;

	bitget_init(bs);		/* initialize bit getter */
	/* test sync */
	in_out.in_bytes = 0;		/* assume fail */
	in_out.out_bytes = 0;
	sync = bitget(12);

	// if( sync != 0xFFF ) return in_out;       /* sync fail */

	mpeg25_flag = 0;
	if (sync != 0xFFF)
	{
		mpeg25_flag = 1;		/* mpeg 2.5 sync */
		if (sync != 0xFFE)
			return in_out;		/* sync fail */
	}
	/*-----------*/

	/*-- unpack side info --*/
	side_bytes = unpack_side_MPEG2(m_igr);
	padframebytes = framebytes + pad;
	in_out.in_bytes = padframebytes;

	buf_ptr0 = buf_ptr1 - side_info.main_data_begin;	/* decode start point */
	if (buf_ptr1 > BUF_TRIGGER)
	{				/* shift buffer */
		memmove(buf, buf + buf_ptr0, side_info.main_data_begin);
		buf_ptr0 = 0;
		buf_ptr1 = side_info.main_data_begin;
	}
	nbytes = padframebytes - side_bytes - crcbytes;
	// RAK: This is no bueno. :-(
	if (nbytes < 0 || nbytes > NBUF)
	{
		in_out.in_bytes = 0;
		return in_out;
	}
	memmove(buf + buf_ptr1, bs + side_bytes + crcbytes, nbytes);
	buf_ptr1 += nbytes;
	/*-----------------------*/

	if (buf_ptr0 >= 0)
	{
		main_pos_bit = buf_ptr0 << 3;
		unpack_main(pcm, m_igr);
		in_out.out_bytes = outbytes;
	}
	else
	{
		memset(pcm, zero_level_pcm, outbytes);	/* fill out skipped frames */
		in_out.out_bytes = outbytes;
		// iframe--;  in_out.out_bytes = 0; return in_out;// test test */
	}

	m_igr = m_igr ^ 1;
	return in_out;
}

//---------------------------------------------------------------------------
static int const sr_table[8] =
{22050, 24000, 16000, 1, 44100, 48000, 32000, 1};

static const struct
{
	int l[23];
	int s[14];
}

sfBandIndexTable[3][3] =
{
	/* mpeg-2 */
	{
		{
			{
				0, 6, 12, 18, 24, 30, 36, 44, 54, 66, 80, 96, 116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576
			}
			,
			{
				0, 4, 8, 12, 18, 24, 32, 42, 56, 74, 100, 132, 174, 192
			}
		}
		,
		{
			{
				0, 6, 12, 18, 24, 30, 36, 44, 54, 66, 80, 96, 114, 136, 162, 194, 232, 278, 332, 394, 464, 540, 576
			}
			,
			{
				0, 4, 8, 12, 18, 26, 36, 48, 62, 80, 104, 136, 180, 192
			}
		}
		,
		{
			{
				0, 6, 12, 18, 24, 30, 36, 44, 54, 66, 80, 96, 116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576
			}
			,
			{
				0, 4, 8, 12, 18, 26, 36, 48, 62, 80, 104, 134, 174, 192
			}
		}
		,
	}
	,
		/* mpeg-1 */
	{
		{
			{
				0, 4, 8, 12, 16, 20, 24, 30, 36, 44, 52, 62, 74, 90, 110, 134, 162, 196, 238, 288, 342, 418, 576
			}
			,
			{
				0, 4, 8, 12, 16, 22, 30, 40, 52, 66, 84, 106, 136, 192
			}
		}
		,
		{
			{
				0, 4, 8, 12, 16, 20, 24, 30, 36, 42, 50, 60, 72, 88, 106, 128, 156, 190, 230, 276, 330, 384, 576
			}
			,
			{
				0, 4, 8, 12, 16, 22, 28, 38, 50, 64, 80, 100, 126, 192
			}
		}
		,
		{
			{
				0, 4, 8, 12, 16, 20, 24, 30, 36, 44, 54, 66, 82, 102, 126, 156, 194, 240, 296, 364, 448, 550, 576
			}
			,
			{
				0, 4, 8, 12, 16, 22, 30, 42, 58, 78, 104, 138, 180, 192
			}
		}
	}
	,
		/* mpeg-2.5, 11 & 12 KHz seem ok, 8 ok */
	{
		{
			{
				0, 6, 12, 18, 24, 30, 36, 44, 54, 66, 80, 96, 116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576
			}
			,
			{
				0, 4, 8, 12, 18, 26, 36, 48, 62, 80, 104, 134, 174, 192
			}
		}
		,
		{
			{
				0, 6, 12, 18, 24, 30, 36, 44, 54, 66, 80, 96, 116, 140, 168, 200, 238, 284, 336, 396, 464, 522, 576
			}
			,
			{
				0, 4, 8, 12, 18, 26, 36, 48, 62, 80, 104, 134, 174, 192
			}
		}
		,
			// this 8khz table, and only 8khz, from mpeg123)
		{
			{
				0, 12, 24, 36, 48, 60, 72, 88, 108, 132, 160, 192, 232, 280, 336, 400, 476, 566, 568, 570, 572, 574, 576
			}
			,
			{
				0, 8, 16, 24, 36, 52, 72, 96, 124, 160, 162, 164, 166, 192
			}
		}
		,
	}
	,
};

int KL3AudioDecoder::DecodeHead(unsigned char *buf, MPEG_HEAD* head)
{
	return mp3Head.L3audio_decode_head(buf, head);
}

//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// Func:	init mp3 decoder 
// Return:	0 for fail
//			1 for success
//---------------------------------------------------------------------------
int KL3AudioDecoder::Init(MPEG_HEAD * h,
						int framebytes_arg,
						int reduction_code,
						int transform_code,	// not used for compatible
						int convert_code,
						int freq_limit)
{
	int i, k;
	int samprate;
	int limit;
	int bit_code;
	int out_chans;

	buf_ptr0 = 0;
	buf_ptr1 = 0;

	// layer III only
	if (h->layer != 1)
		return 0;			

	// version 1=mpeg-1 0=mpeg-2
	if (h->ver)
		ncbl_mixed = 8;	/* mpeg-1 */
	else
		ncbl_mixed = 6;	/* mpeg-2 */

	framebytes = framebytes_arg;

	bit_code = 0;
	if (convert_code & 8)
		bit_code = 1;
	convert_code = convert_code & 3;	/* higher bits used by dec8 freq cvt */

	if (reduction_code < 0)
		reduction_code = 0;
	if (reduction_code > 2)
		reduction_code = 2;

	if (freq_limit < 1000)
		freq_limit = 1000;

	samprate = sr_table[4 * h->ver + h->sr_index];

	if (h->sync == 0xFFE)
		samprate = samprate / 2;	// mpeg 2.5 

	// compute nsb_limit 
	nsb_limit = (freq_limit * 64L + samprate / 2) / samprate;

	// caller limit
	limit = (32 >> reduction_code);
	if (limit > 8)
		limit--;
	if (nsb_limit > limit)
		nsb_limit = limit;
	limit = 18 * nsb_limit;

	k = h->ver;

	if (h->sync == 0xFFE)
		k = 2;	// mpeg 2.5 

	if (k == 1)
	{
		band_limit12 = 3 * sfBandIndexTable[k][h->sr_index].s[13];
		band_limit = band_limit21 = sfBandIndexTable[k][h->sr_index].l[22];
	}
	else
	{
		band_limit12 = 3 * sfBandIndexTable[k][h->sr_index].s[12];
		band_limit = band_limit21 = sfBandIndexTable[k][h->sr_index].l[21];
	}

	band_limit += 8;	/* allow for antialias */
	if (band_limit > limit)
		band_limit = limit;

	if (band_limit21 > band_limit)
		band_limit21 = band_limit;
	if (band_limit12 > band_limit)
		band_limit12 = band_limit;

	band_limit_nsb = (band_limit + 17) / 18;	/* limit nsb's rounded up */

	gain_adjust = 0;	/* adjust gain e.g. cvt to mono sum channel */
	if ((h->mode != 3) && (convert_code == 1))
		gain_adjust = -4;

	outvalues = 1152 >> reduction_code;
	if (h->ver == 0)
		outvalues /= 2;

	out_chans = 2;
	if (h->mode == 3)
		out_chans = 1;
	if (convert_code)
		out_chans = 1;

	sbt_L3 = sbt_table[bit_code][reduction_code][out_chans - 1];
	k = 1 + convert_code;
	if (h->mode == 3)
		k = 0;
	Xform = xform_table[k];

	outvalues *= out_chans;

	if (bit_code)
		outbytes = outvalues;
	else
		outbytes = sizeof(short) * outvalues;

	if (bit_code)
		zero_level_pcm = 128;	/* 8 bit output */
	else
		zero_level_pcm = 0;

	// setup decode info
	decinfo.channels = out_chans;
	decinfo.outvalues = outvalues;
	decinfo.samprate = samprate >> reduction_code;
	if (bit_code)
		decinfo.bits = 8;
	else
		decinfo.bits = sizeof(short) * 8;
	decinfo.framebytes = framebytes;

	half_outbytes = outbytes / 2;

	// init band tables
	k = h->ver;
	if (h->sync == 0xFFE)
		k = 2;	// mpeg 2.5 

	for (i = 0; i < 22; i++)
		sfBandIndex[0][i] = sfBandIndexTable[k][h->sr_index].l[i + 1];

	for (i = 0; i < 13; i++)
		sfBandIndex[1][i] = 3 * sfBandIndexTable[k][h->sr_index].s[i + 1];

	for (i = 0; i < 22; i++)
		nBand[0][i] = sfBandIndexTable[k][h->sr_index].l[i + 1]
	- sfBandIndexTable[k][h->sr_index].l[i];

	for (i = 0; i < 13; i++)
		nBand[1][i] = sfBandIndexTable[k][h->sr_index].s[i + 1]
	- sfBandIndexTable[k][h->sr_index].s[i];

	// init tables
	mp3Init.L3table_init();

	// init ms and is stereo modes 
	mp3Init.msis_init();

	// init sbt 
	sbt.sbt_init();

	// clear buffers
	memset(yout, 0, sizeof(yout));
	memset(sample, 0, sizeof(sample));

	// set decode function
	if (h->ver == 1)
		decode_function = &KL3AudioDecoder::L3audio_decode_MPEG1;
	else
		decode_function = &KL3AudioDecoder::L3audio_decode_MPEG2;

	return 1;
}
//---------------------------------------------------------------------------
void KL3AudioDecoder::DecodeInfo(DEC_INFO *info)
{
	*info = decinfo;
}
//---------------------------------------------------------------------------
IN_OUT KL3AudioDecoder::DecodeFrame(unsigned char *bs, unsigned char *pcm)
{
	return (this->*decode_function)(bs, pcm);
}
//---------------------------------------------------------------------------
