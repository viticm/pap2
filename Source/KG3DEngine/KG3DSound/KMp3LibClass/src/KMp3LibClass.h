//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	Mp3lib.h
// Desc:	MP3 Decoder
//---------------------------------------------------------------------------
#ifndef Mp3libClass_H
#define Mp3libClass_H
//---------------------------------------------------------------------------

#ifndef _OUT_PUT_K_MP3_LIB_
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

#endif

class KL3AudioDecoder;
class KMp3
{
public:
	KMp3();
	~KMp3();
public:
	int DecodeHead(unsigned char *buf, MPEG_HEAD* head);
	int Init(MPEG_HEAD * h,
		int framebytes_arg,
		int reduction_code,
		int transform_code,
		int convert_code,
		int freq_limit);
	IN_OUT DecodeFrame(unsigned char *bs, unsigned char *pcm);
	void DecodeInfo(DEC_INFO *info);

protected:
private:
	KL3AudioDecoder *m_pDecoder;
};


//---------------------------------------------------------------------------
#endif // Mp3libClass_H
