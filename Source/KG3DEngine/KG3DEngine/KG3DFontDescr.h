////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DFontDescr.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-02-13 16:43:32
//  Comment     : font def
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DFONTDESCR_H_
#define _INCLUDE_KG3DFONTDESCR_H_


UINT const KG3DFONT_DEFAULT_POINT = 16;
UINT const KG3DFONT_DEFAULT_PADDING = 2;	// 最大支持 2x2 的衬边

// font glyph
// 参考教育部 1998 年出版品之常用字统计资料库，
// 此资料库统计 1579771 个字词，其中常用字共有 5063 个。
// 不过，出现率最高的前 80% 只有 641 个字，
// 考虑到前瞻性，变态性( 有些国家的语言变态的说) 等等，
// 这里使用 1024 作为最大缓冲大小。

UINT const KG3DFONT_GLYPH_NUMS = 1024;
UINT const KG3DFONT_GLYPH_LIMIT	= 0xFFFF;


struct KG3DFontTLVertex
{
	enum { FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1 };

	FLOAT x, y, z;
	D3DCOLOR diffuse;
	float u, v;
};


#endif //_INCLUDE_KG3DFONTDESCR_H_