//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Head.cpp
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <math.h>
#include "KMp3Type.h"
#include "KMp3HeadClass.h"
//---------------------------------------------------------------------------
static const int mp_br_tableL1[2][16] =
{{0, 32, 48, 56, 64, 80, 96, 112, 128, 144, 160, 176, 192, 224, 256, 0}, /* mpeg2 */
{0, 32, 64, 96, 128, 160, 192, 224, 256, 288, 320, 352, 384, 416, 448, 0}};

static const int mp_br_tableL2[2][16] =
{{0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0},
{0, 32, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 384, 0}};

static const int mp_br_tableL3[2][16] =
{{0, 8, 16, 24, 32, 40, 48, 56, 64, 80, 96, 112, 128, 144, 160, 0}, /* mpeg 2 */
{0, 32, 40, 48, 56, 64, 80, 96, 112, 128, 160, 192, 224, 256, 320, 0}};

static const int mp_sr20_table[2][4] =
{{441, 480, 320, -999}, {882, 960, 640, -999}};
//---------------------------------------------------------------------------
// Name:	L3audio_decode_head
// Func:	get mp3 head info
// Return:	mp3 frame size in bytes
//			return > 0 for success
//			return = 0 for bad mp3 file
//---------------------------------------------------------------------------
int KL3AudioDecodeHead::L3audio_decode_head(unsigned char *buf, MPEG_HEAD * h)
{
	int framebytes = 0;

	// read mp3 header structure	
	h->sync      = (buf[0] << 4) | (buf[1] >> 4);
	h->ver       = (buf[1] & 0x08) >> 3;
	h->layer     = (buf[1] & 0x06) >> 1;
	h->prot      = (buf[1] & 0x01);
	h->br_index  = (buf[2] & 0xF0) >> 4;
	h->sr_index  = (buf[2] & 0x0C) >> 2;
	h->pad       = (buf[2] & 0x02) >> 1;
	h->extension = (buf[2] & 0x01);
	h->mode      = (buf[3] & 0xC0) >> 6;
	h->mode_ext  = (buf[3] & 0x30) >> 4;
	h->copyright = (buf[3] & 0x08) >> 3;
	h->original  = (buf[3] & 0x04) >> 2;
	h->emphasis  = (buf[3] & 0x03);

	// check mpeg sync
	if (h->sync != 0xFFF && h->sync != 0xFFE)
		return 0;

	// check mpeg layer
	if ((h->layer < 1) || (h->layer > 3))
		return 0;

	// check mpeg br index
	if (h->br_index == 0)
		return 0;

	// compute frame bytes for Layer I
	if (h->layer == 3)
	{
		framebytes = 240 * mp_br_tableL1[h->ver][h->br_index]
		/ mp_sr20_table[h->ver][h->sr_index];
		framebytes = 4 * framebytes;
		return framebytes;
	}

	// compute frame bytes for Layer II
	if (h->layer == 2)
	{
		framebytes = 2880 * mp_br_tableL2[h->ver][h->br_index]
		/ mp_sr20_table[h->ver][h->sr_index];
		return framebytes;
	}

	// compute frame bytes for Layer III
	if (h->layer == 1)
	{
		if (h->ver)
		{	// mpeg1
			framebytes = 2880 * mp_br_tableL3[h->ver][h->br_index]
			/ mp_sr20_table[h->ver][h->sr_index];
		}
		else
		{	// mpeg2
			if (h->sync == 0xFFE)
			{	// mpeg2.5
				framebytes = 2880 * mp_br_tableL3[h->ver][h->br_index]
				/ mp_sr20_table[h->ver][h->sr_index];
			}
			else
			{	// mpeg2.0
				framebytes = 1440 * mp_br_tableL3[h->ver][h->br_index]
				/ mp_sr20_table[h->ver][h->sr_index];
			}
		}
	}

	return framebytes;
}
//---------------------------------------------------------------------------
