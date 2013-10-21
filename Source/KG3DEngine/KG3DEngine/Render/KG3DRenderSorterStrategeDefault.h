////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRenderSorterStrategeDefault.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-14 16:24:36
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DRENDERSORTERSTRATEGEDEFAULT_H_
#define _INCLUDE_KG3DRENDERSORTERSTRATEGEDEFAULT_H_

#include "KG3DRenderSorterStratege.h"
////////////////////////////////////////////////////////////////////////////////
class KG3DRepresentObject;
class KG3DRenderData;
struct KG3DRenderSorterStrategeDefault : public KG3DRenderSorterStratege
{
	virtual HRESULT PutIntoRenderData(KG3DRepresentObject& Obj, KG3DRenderData& Data);
};

#endif //_INCLUDE_KG3DRENDERSORTERSTRATEGEDEFAULT_H_
