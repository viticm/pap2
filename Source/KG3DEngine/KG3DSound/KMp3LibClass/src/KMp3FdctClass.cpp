//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	cdct.c
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//			MPEG audio decoder, dct
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <memory.h>
#include "KMp3FdctClass.h"

#ifdef ASM_X86
extern void fdct32_asm(float*a, float*b);
extern void fdct32_dual_asm(float*a, float*b);
#endif


KMp3Fdct::KMp3Fdct()
{
	memset(coef32, 0, sizeof(coef32));
}

//---------------------------------------------------------------------------
// Name:	dct_coef_addr
// Func:
//---------------------------------------------------------------------------
float *KMp3Fdct::dct_coef_addr()
{
	return coef32;
}
//---------------------------------------------------------------------------
// Name:	forward_bf
// Func:
//---------------------------------------------------------------------------
void KMp3Fdct::forward_bf(int m, int n, float x[], float f[], float coef[])
{
	int i, j, n2;
	int p, q, p0, k;

	p0 = 0;
	n2 = n >> 1;
	for (i = 0; i < m; i++, p0 += n)
	{
		k = 0;
		p = p0;
		q = p + n - 1;
		for (j = 0; j < n2; j++, p++, q--, k++)
		{
			f[p] = x[p] + x[q];
			f[n2 + p] = coef[k] * (x[p] - x[q]);
		}
	}
}
//---------------------------------------------------------------------------
// Name:	back_bf
// Func:
//---------------------------------------------------------------------------
void KMp3Fdct::back_bf(int m, int n, float x[], float f[])
{
	int i, j, n2, n21;
	int p, q, p0;

	p0 = 0;
	n2 = n >> 1;
	n21 = n2 - 1;
	for (i = 0; i < m; i++, p0 += n)
	{
		p = p0;
		q = p0;
		for (j = 0; j < n2; j++, p += 2, q++)
			f[p] = x[q];
		p = p0 + 1;
		for (j = 0; j < n21; j++, p += 2, q++)
			f[p] = x[q] + x[q + 1];
		f[p] = x[q];
	}
}
//---------------------------------------------------------------------------
// Name:	fdct32
// Func:	32 pt fdct
//---------------------------------------------------------------------------
void KMp3Fdct::fdct32(float x[], float c[])
{
	float *src = x;

#ifdef ASM_X86
	fdct32_asm(src, c);
#else

	/* ping pong buffers */
	float a[32];
	float b[32];
	int p, q;

	/* special first stage */
	for (p = 0, q = 31; p < 16; p++, q--)
	{
		a[p] = src[p] + src[q];
		a[16 + p] = coef32[p] * (src[p] - src[q]);
	}
	forward_bf(2, 16, a, b, coef32 + 16);
	forward_bf(4, 8, b, a, coef32 + 16 + 8);
	forward_bf(8, 4, a, b, coef32 + 16 + 8 + 4);
	forward_bf(16, 2, b, a, coef32 + 16 + 8 + 4 + 2);
	back_bf(8, 4, a, b);
	back_bf(4, 8, b, a);
	back_bf(2, 16, a, b);
	back_bf(1, 32, b, c);
#endif
}
//---------------------------------------------------------------------------
// Name:	fdct16
// Func:	16 pt fdct
//---------------------------------------------------------------------------
void KMp3Fdct::fdct16(float x[], float c[])
{
	float a[16];			/* ping pong buffers */
	float b[16];
	int p, q;

	/* special first stage (drop highest sb) */
	a[0] = x[0];
	a[8] = coef32[16] * x[0];
	for (p = 1, q = 14; p < 8; p++, q--)
	{
		a[p] = x[p] + x[q];
		a[8 + p] = coef32[16 + p] * (x[p] - x[q]);
	}
	forward_bf(2, 8, a, b, coef32 + 16 + 8);
	forward_bf(4, 4, b, a, coef32 + 16 + 8 + 4);
	forward_bf(8, 2, a, b, coef32 + 16 + 8 + 4 + 2);
	back_bf(4, 4, b, a);
	back_bf(2, 8, a, b);
	back_bf(1, 16, b, c);
}
//---------------------------------------------------------------------------
// Name:	fdct8
// Func:	8 pt fdct
//---------------------------------------------------------------------------
void KMp3Fdct::fdct8(float x[], float c[])
{
	float a[8];			/* ping pong buffers */
	float b[8];
	int p, q;

	/* special first stage  */

	b[0] = x[0] + x[7];
	b[4] = coef32[16 + 8] * (x[0] - x[7]);
	for (p = 1, q = 6; p < 4; p++, q--)
	{
		b[p] = x[p] + x[q];
		b[4 + p] = coef32[16 + 8 + p] * (x[p] - x[q]);
	}

	forward_bf(2, 4, b, a, coef32 + 16 + 8 + 4);
	forward_bf(4, 2, a, b, coef32 + 16 + 8 + 4 + 2);
	back_bf(2, 4, b, a);
	back_bf(1, 8, a, c);
}
//---------------------------------------------------------------------------
