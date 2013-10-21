//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	Mp3lib.h
// Desc:	MP3 Decoder
//---------------------------------------------------------------------------
#ifndef Mp3lib_H
#define Mp3lib_H
//---------------------------------------------------------------------------
#ifdef __cplusplus
extern "C" {
#endif 
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
int		mp3_decode_head(unsigned char *buf, MPEG_HEAD* head);
int		mp3_decode_init(MPEG_HEAD *head,
						int framebytes_arg,
						int reduction_code,
						int transform_code,
						int convert_code,
						int freq_limit);
void	mp3_decode_info(DEC_INFO *info);
IN_OUT	mp3_decode_frame(unsigned char *mp3, unsigned char *pcm);
//---------------------------------------------------------------------------
#ifdef __cplusplus
}
#endif 
//---------------------------------------------------------------------------
#endif // Mp3lib_H
