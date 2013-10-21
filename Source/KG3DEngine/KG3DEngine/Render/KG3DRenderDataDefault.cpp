////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRenderDataDefault.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-14 15:50:02
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DRenderDataDefault.h"


////////////////////////////////////////////////////////////////////////////////


HRESULT KG3DRenderDataDefault::AddObj( KG3DRepresentObject& Obj, const KG3DRenderer& Renderer )
{
	return E_FAIL;
}

HRESULT KG3DRenderDataDefault::AddModel( KG3DModel& Model, const KG3DMaterial& Material, const KG3DRenderer& Renderer )
{
	return E_FAIL;
}

HRESULT KG3DRenderDataDefault::FrameMove()
{
	return E_FAIL;
}

KG3DRenderDataDefault::~KG3DRenderDataDefault()
{
	/*for (TypeModelCt::iterator it = m_ModelCt.begin(); it != m_ModelCt.end(); ++it)
	{
		TypeModelGroup& ModelGroup = it->second;
		KG_CUSTOM_HELPERS::TContainerRelease(ModelGroup);
	}
	m_ModelCt.clear();*/
}