////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DCullerTop.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-14 10:19:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DCULLERTOP_H_
#define _INCLUDE_KG3DCULLERTOP_H_

#include "KG3DCuller.h"
class KG3DCameraOrthogonalData;
////////////////////////////////////////////////////////////////////////////////
class KG3DCullerTopOrthogonal : KG3DCuller
{
public:
	virtual BOOL Cull(KG3DRepresentObject& Obj);
	KG3DCullerTopOrthogonal(const KG3DCameraOrthogonalData& Data, const D3DXVECTOR3& CamPos);
#if defined(DEBUG) | defined(_DEBUG)
protected:
#else
private:
#endif
	AABBOX	m_Box;
};

#endif //_INCLUDE_KG3DCULLERTOP_H_
