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

#define _OUT_PUT_K_MP3_LIB_
#include "KMp3LibClass.h"
#undef _OUT_PUT_K_MP3_LIB_

KMp3::KMp3()
{
	m_pDecoder = new KL3AudioDecoder;
}

KMp3::~KMp3()
{
	if (m_pDecoder)
	{
		delete m_pDecoder;
		m_pDecoder = NULL;
	}
}

int KMp3::DecodeHead(unsigned char *buf, MPEG_HEAD* head)
{
	if (m_pDecoder)
		return m_pDecoder->DecodeHead(buf, head);
	return 0;
}

int KMp3::Init(MPEG_HEAD * h,
		 int framebytes_arg,
		 int reduction_code,
		 int transform_code,
		 int convert_code,
		 int freq_limit)
{
	if (m_pDecoder)
		return m_pDecoder->Init(h, 
			framebytes_arg, 
			reduction_code, 
			transform_code, 
			convert_code, 
			freq_limit);
	return false;
}

IN_OUT KMp3::DecodeFrame(unsigned char *bs, unsigned char *pcm)
{
	if (m_pDecoder)
		return m_pDecoder->DecodeFrame(bs, pcm);
	IN_OUT faild;
	faild.in_bytes = 0;
	faild.out_bytes = 0;
	return IN_OUT(faild);
}

void KMp3::DecodeInfo(DEC_INFO *info)
{
	if (m_pDecoder)
		m_pDecoder->DecodeInfo(info);
}

