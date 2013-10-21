////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DBlockTraverseData.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-5-26 11:30:44
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DBLOCKTRAVERSEDATA_H_
#define _INCLUDE_KG3DBLOCKTRAVERSEDATA_H_

////////////////////////////////////////////////////////////////////////////////
extern INT GetSceneBlockIndex(FLOAT In, FLOAT BlockWidth);
struct KG3DBlockTraverseData 
{
	D3DXVECTOR2	m_vStartIndex;
	INT			m_nStep;
	D3DXVECTOR2 m_vIndexStep1;	//浮点的步长，分两份
	D3DXVECTOR2 m_vIndexStep2;

	static KG3DBlockTraverseData Build(const D3DXVECTOR2& vSrc, const D3DXVECTOR2& vDes, FLOAT fSceneBlockWidth);
	friend bool operator==(const KG3DBlockTraverseData& dataA, const KG3DBlockTraverseData& dataB);
	friend bool operator!=(const KG3DBlockTraverseData& dataA, const KG3DBlockTraverseData& dataB);
};

#endif //_INCLUDE_KG3DBLOCKTRAVERSEDATA_H_
