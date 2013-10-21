////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRenderDataDefault.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-14 15:50:05
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DRENDERDATADEFAULT_H_
#define _INCLUDE_KG3DRENDERDATADEFAULT_H_
#include "KG3DRenderData.h"
////////////////////////////////////////////////////////////////////////////////
class KG3DRenderDataDefault : KG3DRenderData
{
public:
	virtual HRESULT AddObj(KG3DRepresentObject& Obj, const KG3DRenderer& Renderer);
	virtual HRESULT AddModel(KG3DModel& Model, const KG3DMaterial& Material, const KG3DRenderer& Renderer);
	virtual HRESULT FrameMove();
	~KG3DRenderDataDefault();
private:
	typedef std::vector<KG3DModel*>	TypeModelGroup;
	typedef std::map<DWORD, TypeModelGroup> TypeModelCt;
	TypeModelCt	m_ModelCt;
};

#endif //_INCLUDE_KG3DRENDERDATADEFAULT_H_
