//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Quant.cpp
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//			Layer III  dequant
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <string.h>
#include <memory.h>
#include "KMp3Type.h"
#include "KMp3QuantClass.h"
//---------------------------------------------------------------------------
static const int pretab[2][22] =
{
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 2, 2, 3, 3, 3, 2, 0},
};

KMp3Quant::KMp3Quant()
{
	memset(look_global, 0, sizeof(look_global));
	memset(look_scale, 0, sizeof(look_scale));
	memset(look_pow, 0, sizeof(look_pow));

	memset(look_subblock, 0, sizeof(look_subblock));

	memset(re_buf, 0, sizeof(re_buf));

}

//---------------------------------------------------------------------------
float *KMp3Quant::quant_init_global_addr()
{
	return look_global;
}
//---------------------------------------------------------------------------
LS *KMp3Quant::quant_init_scale_addr()
{
	return look_scale;
}
//---------------------------------------------------------------------------
float *KMp3Quant::quant_init_pow_addr()
{
	return look_pow;
}
//---------------------------------------------------------------------------
float *KMp3Quant::quant_init_subblock_addr()
{
	return look_subblock;
}
//---------------------------------------------------------------------------
void KMp3Quant::dequant(int nBand[2][22], float sample[], int *nsamp,
			 SCALEFACT * sf,
			 GR * gr,
			 CB_INFO * cb_info, int ncbl_mixed)
{
	int i, j;
	int cb, n, w;
	float x0, xs;
	float xsb[3];
	double tmp;
	int ncbl;
	int cbs0;
	ARRAY3 *buf;		/* short block reorder */
	int nbands;
	int i0;
	int non_zero;
	int cbmax[3];

	// sample type convert
	SAMPLE* Sample = (SAMPLE*)sample;

	nbands = *nsamp;

	ncbl = 22;			/* long block cb end */
	cbs0 = 12;			/* short block cb start */

	/* ncbl_mixed = 8 or 6  mpeg1 or 2 */
	if (gr->block_type == 2)
	{
		ncbl = 0;
		cbs0 = 0;
		if (gr->mixed_block_flag)
		{
			ncbl = ncbl_mixed;
			cbs0 = 3;
		}
	}

	/* fill in cb_info -- */
	cb_info->cbs0 = cbs0;
	cb_info->ncbl = ncbl;

	cbmax[2] = cbmax[1] = cbmax[0] = 0;
	/* global gain pre-adjusted by 2 if ms_mode, 0 otherwise */
	x0 = look_global[(2 + 4) + gr->global_gain];
	i = 0;
	/*----- long blocks ---*/
	for (cb = 0; cb < ncbl; cb++)
	{
		non_zero = 0;
		xs = x0 * look_scale[gr->scalefac_scale][pretab[gr->preflag][cb]][sf->l[cb]];
		n = nBand[0][cb];
		for (j = 0; j < n; j++, i++)
		{
			if (Sample[i].s == 0)
				Sample[i].x = 0.0F;
			else
			{
				non_zero = 1;
				if ((Sample[i].s >= (-ISMAX)) && (Sample[i].s < ISMAX))
					Sample[i].x = xs * look_pow[ISMAX + Sample[i].s];
				else
				{
					double tmpConst = (double)(1.0/3.0);
					tmp = (double) Sample[i].s;
					Sample[i].x = (float) (xs * tmp * pow(fabs(tmp), tmpConst));
				}
			}
		}
		if (non_zero)
			cbmax[0] = cb;
		if (i >= nbands)
			break;
	}

	cb_info->cbmax = cbmax[0];
	cb_info->cbtype = 0;		// type = long

	if (cbs0 >= 12)
		return;
	/*---------------------------
	block type = 2  short blocks
	----------------------------*/
	cbmax[2] = cbmax[1] = cbmax[0] = cbs0;
	i0 = i;			/* save for reorder */
	buf = re_buf;
	for (w = 0; w < 3; w++)
		xsb[w] = x0 * look_subblock[gr->subblock_gain[w]];
	for (cb = cbs0; cb < 13; cb++)
	{
		n = nBand[1][cb];
		for (w = 0; w < 3; w++)
		{
			non_zero = 0;
			xs = xsb[w] * look_scale[gr->scalefac_scale][0][sf->s[w][cb]];
			for (j = 0; j < n; j++, i++)
			{
				if (Sample[i].s == 0)
					buf[j][w] = 0.0F;
				else
				{
					non_zero = 1;
					if ((Sample[i].s >= (-ISMAX)) && (Sample[i].s < ISMAX))
						buf[j][w] = xs * look_pow[ISMAX + Sample[i].s];
					else
					{
						double tmpConst = (double)(1.0/3.0);
						tmp = (double) Sample[i].s;
						buf[j][w] = (float) (xs * tmp * pow(fabs(tmp), tmpConst));
					}
				}
			}
			if (non_zero)
				cbmax[w] = cb;
		}
		if (i >= nbands)
			break;
		buf += n;
	}

	memmove(&Sample[i0].x, &re_buf[0][0], sizeof(float) * (i - i0));

	*nsamp = i;			/* update nsamp */

	cb_info->cbmax_s[0] = cbmax[0];
	cb_info->cbmax_s[1] = cbmax[1];
	cb_info->cbmax_s[2] = cbmax[2];

	if (cbmax[1] > cbmax[0])
		cbmax[0] = cbmax[1];
	if (cbmax[2] > cbmax[0])
		cbmax[0] = cbmax[2];

	cb_info->cbmax = cbmax[0];
	cb_info->cbtype = 1;		/* type = short */
}
//---------------------------------------------------------------------------
