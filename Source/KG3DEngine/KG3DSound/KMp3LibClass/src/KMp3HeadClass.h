//---------------------------------------------------------------------------
// Sword3 Engine (c) 1999-2000 by Kingsoft
//
// File:	KMp3Head.h
// Desc:	MP3 Decoder originally Copyright (C) 1995-1997 Xing Technology
//			Corp.  http://www.xingtech.com
//---------------------------------------------------------------------------
#ifndef KMp3HeadClass_H
#define KMp3HeadClass_H
//---------------------------------------------------------------------------

class KL3AudioDecodeHead
{
public:
	int L3audio_decode_head(unsigned char *buf, MPEG_HEAD* head);
protected:
private:
};

//---------------------------------------------------------------------------
#endif // KMp3HeadClass_H
