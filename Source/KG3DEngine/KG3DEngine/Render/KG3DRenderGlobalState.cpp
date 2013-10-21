////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRenderGlobalState.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-28 14:51:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DRenderGlobalState.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

class KG3DRenderGlobalStateSolid : public KG3DRenderGlobalState, public KG3DRenderGlobalStateModifier 
{
public:
	KG3DRenderGlobalStateSolid();

	virtual const KG3DLightParam& GetSunLightParam() const
	{
		return m_sunLightParam;
	}
	virtual const KG3DLightParam& GetPlayerLightParam() const 
	{
		return m_playerLightParam;
	}
	virtual void SetSunLightParam(const KG3DLightParam& LightParam)
	{
		m_sunLightParam = LightParam;
	}
	virtual void SetPlayerLightParam(const KG3DLightParam& LightParam)
	{
		m_playerLightParam = LightParam;
	}
private:
	KG3DLightParam m_sunLightParam;
	KG3DLightParam m_playerLightParam;
};


KG3DRenderGlobalStateSolid::KG3DRenderGlobalStateSolid()
{	
	
}



static KG3DRenderGlobalStateSolid SolidState;
extern const KG3DRenderGlobalState& g_GetRenderGlobalState()
{
	return SolidState;
}

#if defined(DEBUG) | defined(_DEBUG)
#include "KG3DEnvEffLight.h"
#endif

extern KG3DRenderGlobalStateModifier& g_GetRenderGlobalStateModifier()
{
	return SolidState;
}