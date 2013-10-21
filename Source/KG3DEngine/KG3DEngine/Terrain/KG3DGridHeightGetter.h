////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DGridHeightGetter.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-9-17 14:56:19
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DGRIDHEIGHTGETTER_H_
#define _INCLUDE_KG3DGRIDHEIGHTGETTER_H_

////////////////////////////////////////////////////////////////////////////////
struct KG3DGridVertexHeightGetter 
{
	virtual D3DXVECTOR4 GetBlockRect() = 0;
	virtual ULONG		GetBlockCellCount() = 0;	//如果是16个Cell的话，应该每边17个点才对
	virtual FLOAT		GetVertexHeight(int nX,int nZ) = 0;
	virtual ~KG3DGridVertexHeightGetter() = 0{}
};

FLOAT KG3DGridHeightGetterFunc(FLOAT fX, FLOAT fZ, KG3DGridVertexHeightGetter& GH);

#endif //_INCLUDE_KG3DGRIDHEIGHTGETTER_H_
