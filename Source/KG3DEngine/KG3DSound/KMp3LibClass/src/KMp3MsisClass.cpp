//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Msis
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//			Layer III  
//			antialias, ms and is stereo precessing
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <memory.h>
#include "KMp3Type.h"
#include "KMp3MsisClass.h"
//---------------------------------------------------------------------------

#ifdef ASM_X86
extern void antialias_asm(float x[], int n);
#endif

KMp3Msis::KMp3Msis()
{
	memset(csa, 0, sizeof(csa));
	memset(lr, 0, sizeof(lr));
	memset(lr2, 0, sizeof(lr2));

}

//---------------------------------------------------------------------------
ARRAY2 *KMp3Msis::alias_init_addr()
{
	return csa;
}
//---------------------------------------------------------------------------
ARRAY8_2 *KMp3Msis::msis_init_addr()
{
	return lr;
}
//---------------------------------------------------------------------------
ARRAY2_64_2 *KMp3Msis::msis_init_addr_MPEG2()
{
	return lr2;
}
//---------------------------------------------------------------------------
void KMp3Msis::antialias(float x[], int n)
{
#ifdef ASM_X86
	antialias_asm(x, n);
#else
	int i, k;
	float a, b;

	for (k = 0; k < n; k++)
	{
		for (i = 0; i < 8; i++)
		{
			a = x[17 - i];
			b = x[18 + i];
			x[17 - i] = a * csa[i][0] - b * csa[i][1];
			x[18 + i] = b * csa[i][0] + a * csa[i][1];
		}
		x += 18;
	}
#endif
}
//---------------------------------------------------------------------------
void KMp3Msis::ms_process(ARRAY1152 x, int n)		/* sum-difference stereo */
{
	int i;
	float xl, xr;

	/*-- note: sqrt(2) done scaling by dequant ---*/
	for (i = 0; i < n; i++)
	{
		xl = x[0][i] + x[1][i];
		xr = x[0][i] - x[1][i];
		x[0][i] = xl;
		x[1][i] = xr;
	}
}
//---------------------------------------------------------------------------
void KMp3Msis::is_process_MPEG1(int nBand[2][22], int sfBandIndex[2][22], ARRAY1152 x,	/* intensity stereo */
					  SCALEFACT * sf,
					  CB_INFO cb_info[2],	/* [ch] */
					  int nsamp, int ms_mode)
{
	int i, j, n, cb, w;
	float fl, fr;
	int m;
	int isf;
	float fls[3], frs[3];
	int cb0;

	cb0 = cb_info[1].cbmax;	/* start at end of right */
	i = sfBandIndex[cb_info[1].cbtype][cb0];
	cb0++;
	m = nsamp - i;		/* process to len of left */

	if (cb_info[1].cbtype)
		goto short_blocks;
	/*------------------------*/
	/* long_blocks: */
	for (cb = cb0; cb < 21; cb++)
	{
		isf = sf->l[cb];
		n = nBand[0][cb];
		fl = lr[ms_mode][isf][0];
		fr = lr[ms_mode][isf][1];
		for (j = 0; j < n; j++, i++)
		{
			if (--m < 0)
				goto exit;
			x[1][i] = fr * x[0][i];
			x[0][i] = fl * x[0][i];
		}
	}
	return;

short_blocks:

	for (cb = cb0; cb < 12; cb++)
	{
		for (w = 0; w < 3; w++)
		{
			isf = sf->s[w][cb];
			fls[w] = lr[ms_mode][isf][0];
			frs[w] = lr[ms_mode][isf][1];
		}
		n = nBand[1][cb];
		for (j = 0; j < n; j++)
		{
			m -= 3;
			if (m < 0)
				goto exit;
			x[1][i] = frs[0] * x[0][i];
			x[0][i] = fls[0] * x[0][i];
			x[1][1 + i] = frs[1] * x[0][1 + i];
			x[0][1 + i] = fls[1] * x[0][1 + i];
			x[1][2 + i] = frs[2] * x[0][2 + i];
			x[0][2 + i] = fls[2] * x[0][2 + i];
			i += 3;
		}
	}

exit:
	return;
}
//---------------------------------------------------------------------------
void KMp3Msis::is_process_MPEG2(int nBand[2][22], int sfBandIndex[2][22], ARRAY1152 x,	/* intensity stereo */
					  SCALEFACT * sf,
					  CB_INFO cb_info[2],	/* [ch] */
					  IS_SF_INFO * is_sf_info,
					  int nsamp, int ms_mode)
{
	int i, j, k, n, cb, w;
	float fl, fr;
	int m;
	int isf;
	int il[21];
	int tmp;
	int r;
	ARRAY2 *lr;
	int cb0, cb1;

	lr = lr2[is_sf_info->intensity_scale][ms_mode];

	if (cb_info[1].cbtype)
		goto short_blocks;

	/* long_blocks: */
	cb0 = cb_info[1].cbmax;	/* start at end of right */
	i = sfBandIndex[0][cb0];
	m = nsamp - i;		/* process to len of left */

	/* gen sf info */
	for (k = r = 0; r < 3; r++)
	{
		tmp = (1 << is_sf_info->slen[r]) - 1;
		for (j = 0; j < is_sf_info->nr[r]; j++, k++)
			il[k] = tmp;
	}
	for (cb = cb0 + 1; cb < 21; cb++)
	{
		isf = il[cb] + sf->l[cb];
		fl = lr[isf][0];
		fr = lr[isf][1];
		n = nBand[0][cb];
		for (j = 0; j < n; j++, i++)
		{
			if (--m < 0)
				goto exit;
			x[1][i] = fr * x[0][i];
			x[0][i] = fl * x[0][i];
		}
	}
	return;

short_blocks:

	for (k = r = 0; r < 3; r++)
	{
		tmp = (1 << is_sf_info->slen[r]) - 1;
		for (j = 0; j < is_sf_info->nr[r]; j++, k++)
			il[k] = tmp;
	}

	for (w = 0; w < 3; w++)
	{
		cb0 = cb_info[1].cbmax_s[w];	/* start at end of right */
		i = sfBandIndex[1][cb0] + w;
		cb1 = cb_info[0].cbmax_s[w];	/* process to end of left */

		for (cb = cb0 + 1; cb <= cb1; cb++)
		{
			isf = il[cb] + sf->s[w][cb];
			fl = lr[isf][0];
			fr = lr[isf][1];
			n = nBand[1][cb];
			for (j = 0; j < n; j++)
			{
				x[1][i] = fr * x[0][i];
				x[0][i] = fl * x[0][i];
				i += 3;
			}
		}
	}

exit:
	return;
}
//---------------------------------------------------------------------------
