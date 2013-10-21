////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRenderData.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-14 11:33:44
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DRENDERDATA_H_
#define _INCLUDE_KG3DRENDERDATA_H_

////////////////////////////////////////////////////////////////////////////////
class KG3DRepresentObject;
class KG3DRenderer;
class KG3DModel;
class KG3DMaterial;

struct KG3DRenderData	//Add都会加引用计数
{
	virtual HRESULT AddObj(KG3DRepresentObject& Obj, const KG3DRenderer& Renderer) = 0;
	virtual HRESULT AddModel(KG3DModel& Model, const KG3DMaterial& Materail, const KG3DRenderer& Renderer) = 0;
	virtual HRESULT FrameMove() = 0;
	virtual ~KG3DRenderData() = 0{}
};

#endif //_INCLUDE_KG3DRENDERDATA_H_
