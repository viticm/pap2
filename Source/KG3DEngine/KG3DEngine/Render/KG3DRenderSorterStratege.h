////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRenderSorterStratege.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-14 15:35:09
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DRENDERSORTERSTRATEGE_H_
#define _INCLUDE_KG3DRENDERSORTERSTRATEGE_H_

class KG3DRenderData;
class KG3DModel;
class KG3DRepresentObject;
////////////////////////////////////////////////////////////////////////////////
struct KG3DRenderSorterStratege 
{
	virtual HRESULT PutIntoRenderData(KG3DRepresentObject& Obj, KG3DRenderData& Data) = 0;
	virtual ~KG3DRenderSorterStratege() = 0{}
};

//struct KG3DRenderSorterModelStratege 
//{
//	virtual HRESULT PutIntoRenderData(KG3DModel& Model, KG3DRenderData& Data) = 0;
//	virtual ~KG3DRenderSorterModelStratege() = 0{}
//};

#endif //_INCLUDE_KG3DRENDERSORTERSTRATEGE_H_
