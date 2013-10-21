//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	cdct.c
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//			MPEG audio decoder, dct and window
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include <memory.h>
#include "KMp3FdctClass.h"
#include "KMp3WinClass.h"
#include "KMp3WinBClass.h"
#include "KMp3SbtClass.h"


KMp3Sbt::KMp3Sbt()
{
	vb_ptr = 0;
	vb2_ptr = 0;
	memset(vbuf, 0, sizeof(vbuf));
	memset(vbuf2, 0, sizeof(vbuf2));

	first_pass = 1;
}

KMp3Sbt::~KMp3Sbt()
{

}

//---------------------------------------------------------------------------
void KMp3Sbt::gencoef()		/* gen coef for N=32 (31 coefs) */
{
	int p, n, i, k;
	double t, pi;
	float *coef32;

	coef32 = fdct.dct_coef_addr();

	pi = 4.0 * atan(1.0);
	n = 16;
	k = 0;
	for (i = 0; i < 5; i++, n = n / 2)
	{

		for (p = 0; p < n; p++, k++)
		{
			t = (pi / (4 * n)) * (2 * p + 1);
			coef32[k] = (float) (0.50 / cos(t));
		}
	}
}
//---------------------------------------------------------------------------
void KMp3Sbt::sbt_init()
{
	int i;

	if (first_pass)
	{
		gencoef();
		first_pass = 0;
	}

	/* clear window vbuf */
	for (i = 0; i < 512; i++)
	{
		vbuf[i] = 0.0F;
		vbuf2[i] = 0.0F;
	}
	vb2_ptr = vb_ptr = 0;
}

//---------------------------------------------------------------------------
//	Layer III
//---------------------------------------------------------------------------
void KMp3Sbt::sbt_mono_L3(float *sample, short *pcm, int ch)
{
	int i;

	ch = 0;
	for (i = 0; i < 18; i++)
	{
		fdct.fdct32(sample, vbuf + vb_ptr);
		mp3win.window(vbuf, vb_ptr, pcm);
		sample += 32;
		vb_ptr = (vb_ptr - 32) & 511;
		pcm += 32;
	}
}
//---------------------------------------------------------------------------
void KMp3Sbt::sbt_dual_L3(float *sample, short *pcm, int ch)
{
	int i;

	if (ch == 0)
	{
		for (i = 0; i < 18; i++)
		{
			fdct.fdct32(sample, vbuf + vb_ptr);
			mp3win.window_dual(vbuf, vb_ptr, pcm);
			sample += 32;
			vb_ptr = (vb_ptr - 32) & 511;
			pcm += 64;
		}
	}
	else
	{
		for (i = 0; i < 18; i++)
		{
			fdct.fdct32(sample, vbuf2 + vb2_ptr);
			mp3win.window_dual(vbuf2, vb2_ptr, pcm + 1);
			sample += 32;
			vb2_ptr = (vb2_ptr - 32) & 511;
			pcm += 64;
		}
	}
}
//---------------------------------------------------------------------------
/*---------------- 16 pt sbt's  -------------------------------*/
//---------------------------------------------------------------------------
void KMp3Sbt::sbt16_mono_L3(float *sample, short *pcm, int ch)
{
	int i;

	ch = 0;
	for (i = 0; i < 18; i++)
	{
		fdct.fdct16(sample, vbuf + vb_ptr);
		mp3win.window16(vbuf, vb_ptr, pcm);
		sample += 32;
		vb_ptr = (vb_ptr - 16) & 255;
		pcm += 16;
	}


}
//---------------------------------------------------------------------------
void KMp3Sbt::sbt16_dual_L3(float *sample, short *pcm, int ch)
{
	int i;


	if (ch == 0)
	{
		for (i = 0; i < 18; i++)
		{
			fdct.fdct16(sample, vbuf + vb_ptr);
			mp3win.window16_dual(vbuf, vb_ptr, pcm);
			sample += 32;
			vb_ptr = (vb_ptr - 16) & 255;
			pcm += 32;
		}
	}
	else
	{
		for (i = 0; i < 18; i++)
		{
			fdct.fdct16(sample, vbuf2 + vb2_ptr);
			mp3win.window16_dual(vbuf2, vb2_ptr, pcm + 1);
			sample += 32;
			vb2_ptr = (vb2_ptr - 16) & 255;
			pcm += 32;
		}
	}
}
//---------------------------------------------------------------------------
/*---------------- 8 pt sbt's  -------------------------------*/
//---------------------------------------------------------------------------
void KMp3Sbt::sbt8_mono_L3(float *sample, short *pcm, int ch)
{
	int i;

	ch = 0;
	for (i = 0; i < 18; i++)
	{
		fdct.fdct8(sample, vbuf + vb_ptr);
		mp3win.window8(vbuf, vb_ptr, pcm);
		sample += 32;
		vb_ptr = (vb_ptr - 8) & 127;
		pcm += 8;
	}
}
//---------------------------------------------------------------------------
void KMp3Sbt::sbt8_dual_L3(float *sample, short *pcm, int ch)
{
	int i;

	if (ch == 0)
	{
		for (i = 0; i < 18; i++)
		{
			fdct.fdct8(sample, vbuf + vb_ptr);
			mp3win.window8_dual(vbuf, vb_ptr, pcm);
			sample += 32;
			vb_ptr = (vb_ptr - 8) & 127;
			pcm += 16;
		}
	}
	else
	{
		for (i = 0; i < 18; i++)
		{
			fdct.fdct8(sample, vbuf2 + vb2_ptr);
			mp3win.window8_dual(vbuf2, vb2_ptr, pcm + 1);
			sample += 32;
			vb2_ptr = (vb2_ptr - 8) & 127;
			pcm += 16;
		}
	}
}
//---------------------------------------------------------------------------
/*------- 8 bit output ---------------------------------------*/
//---------------------------------------------------------------------------
void KMp3Sbt::sbtB_mono_L3(float *sample, unsigned char *pcm, int ch)
{
	int i;

	ch = 0;
	for (i = 0; i < 18; i++)
	{
		fdct.fdct32(sample, vbuf + vb_ptr);
		mp3winB.windowB(vbuf, vb_ptr, pcm);
		sample += 32;
		vb_ptr = (vb_ptr - 32) & 511;
		pcm += 32;
	}
}
//---------------------------------------------------------------------------
void KMp3Sbt::sbtB_dual_L3(float *sample, unsigned char *pcm, int ch)
{
	int i;

	if (ch == 0)
	{
		for (i = 0; i < 18; i++)
		{
			fdct.fdct32(sample, vbuf + vb_ptr);
			mp3winB.windowB_dual(vbuf, vb_ptr, pcm);
			sample += 32;
			vb_ptr = (vb_ptr - 32) & 511;
			pcm += 64;
		}
	}
	else
	{
		for (i = 0; i < 18; i++)
		{
			fdct.fdct32(sample, vbuf2 + vb2_ptr);
			mp3winB.windowB_dual(vbuf2, vb2_ptr, pcm + 1);
			sample += 32;
			vb2_ptr = (vb2_ptr - 32) & 511;
			pcm += 64;
		}
	}
}
//---------------------------------------------------------------------------
/*---------------- 16 pt sbtB's  -------------------------------*/
//---------------------------------------------------------------------------
void KMp3Sbt::sbtB16_mono_L3(float *sample, unsigned char *pcm, int ch)
{
	int i;

	ch = 0;
	for (i = 0; i < 18; i++)
	{
		fdct.fdct16(sample, vbuf + vb_ptr);
		mp3winB.windowB16(vbuf, vb_ptr, pcm);
		sample += 32;
		vb_ptr = (vb_ptr - 16) & 255;
		pcm += 16;
	}
}
//---------------------------------------------------------------------------
void KMp3Sbt::sbtB16_dual_L3(float *sample, unsigned char *pcm, int ch)
{
	int i;

	if (ch == 0)
	{
		for (i = 0; i < 18; i++)
		{
			fdct.fdct16(sample, vbuf + vb_ptr);
			mp3winB.windowB16_dual(vbuf, vb_ptr, pcm);
			sample += 32;
			vb_ptr = (vb_ptr - 16) & 255;
			pcm += 32;
		}
	}
	else
	{
		for (i = 0; i < 18; i++)
		{
			fdct.fdct16(sample, vbuf2 + vb2_ptr);
			mp3winB.windowB16_dual(vbuf2, vb2_ptr, pcm + 1);
			sample += 32;
			vb2_ptr = (vb2_ptr - 16) & 255;
			pcm += 32;
		}
	}
}
//---------------------------------------------------------------------------
/*---------------- 8 pt sbtB's  -------------------------------*/
//---------------------------------------------------------------------------
void KMp3Sbt::sbtB8_mono_L3(float *sample, unsigned char *pcm, int ch)
{
	int i;

	ch = 0;
	for (i = 0; i < 18; i++)
	{
		fdct.fdct8(sample, vbuf + vb_ptr);
		mp3winB.windowB8(vbuf, vb_ptr, pcm);
		sample += 32;
		vb_ptr = (vb_ptr - 8) & 127;
		pcm += 8;
	}
}
//---------------------------------------------------------------------------
void KMp3Sbt::sbtB8_dual_L3(float *sample, unsigned char *pcm, int ch)
{
	int i;

	if (ch == 0)
	{
		for (i = 0; i < 18; i++)
		{
			fdct.fdct8(sample, vbuf + vb_ptr);
			mp3winB.windowB8_dual(vbuf, vb_ptr, pcm);
			sample += 32;
			vb_ptr = (vb_ptr - 8) & 127;
			pcm += 16;
		}
	}
	else
	{
		for (i = 0; i < 18; i++)
		{
			fdct.fdct8(sample, vbuf2 + vb2_ptr);
			mp3winB.windowB8_dual(vbuf2, vb2_ptr, pcm + 1);
			sample += 32;
			vb2_ptr = (vb2_ptr - 8) & 127;
			pcm += 16;
		}
	}
}

//------------------------------------------------------------------------
