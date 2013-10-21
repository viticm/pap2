////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : MathToolEx.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-5-22 11:46:10
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_MATHTOOLEX_H_
#define _INCLUDE_MATHTOOLEX_H_
#include "MathTool.h"
////////////////////////////////////////////////////////////////////////////////
struct KG3DPoly
{
	std::vector<D3DXVECTOR3>m_vecVertex;
	float m_fHeight;

	AABBOX m_BBox;

	BOOL IsPointIn(D3DXVECTOR3 vPos,BOOL bTestHeight);

	HRESULT Clear();
	HRESULT AddVertex(D3DXVECTOR3& vPos);
	HRESULT AddVertex(std::list<D3DXVECTOR3>*plistPos);
	HRESULT SetHeight(float fH);
	HRESULT ComputeBBox();
};


#endif //_INCLUDE_MATHTOOLEX_H_
