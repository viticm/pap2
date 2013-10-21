#ifndef FONT_TYPES_H
#define FONT_TYPES_H


UINT const KG3DFONT_NUMS_INDEX = 1024 * 6;
UINT const KG3DFONT_NUMS_VERTEX = 1024;
UINT const KG3DFONT_NUMS_TLVERTEX = 1024;

UINT const KG3DFONT_FVF_VERTEX = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;
UINT const KG3DFONT_FVF_TLVERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1;

UINT  const  KG3DFONT_DEFAULT_PADDING	= 3;	// 最大支持 2x2 的衬边

struct FontVertex
{
	FLOAT	x, y, z;
	UINT	color;
	float	u, v;
};

struct FontTLVertex
{
	FLOAT	x, y, z, w;
	UINT	color;
	float	u, v;
};


#endif // FONT_TYPES_H
