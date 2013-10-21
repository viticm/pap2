////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DRenderGlobalState.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-8-28 14:51:57
//  Comment     : g_GetRenderGlobalState().GetLightParam()就可以得到光的参数
// 更改的接口是分离的，必须那个类有权限才能更改
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DRENDERGLOBALSTATE_H_
#define _INCLUDE_KG3DRENDERGLOBALSTATE_H_

////////////////////////////////////////////////////////////////////////////////
#include "KG3DTypes.h"

class KG3DTimeClock;
class KG3DEngineManager;
struct KG3DRenderGlobalState 
{
	virtual const KG3DLightParam& GetSunLightParam() const = 0;
	virtual const KG3DLightParam& GetPlayerLightParam() const = 0;

	virtual ~KG3DRenderGlobalState() = 0{}
};

extern const KG3DRenderGlobalState& g_GetRenderGlobalState();

//////////////////////////////////////////////////////////////////////////
namespace KG3D_ENVIRONMENT_EFFECT
{
	class KG3DEnvEffect;
}
struct KG3DRenderGlobalStateModifier 
{
	virtual void SetSunLightParam(const KG3DLightParam& LightParam) = 0;
	virtual void SetPlayerLightParam(const KG3DLightParam& LightParam) = 0;

	virtual ~KG3DRenderGlobalStateModifier() = 0{}
};

extern KG3DRenderGlobalStateModifier& g_GetRenderGlobalStateModifier();//用于检查权限，以后用CommonObject来检查

#endif //_INCLUDE_KG3DRENDERGLOBALSTATE_H_
