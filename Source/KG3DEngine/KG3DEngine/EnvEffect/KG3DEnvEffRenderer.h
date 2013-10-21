////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DEnvEffRenderer.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-3-31 15:07:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DENVEFFRENDERER_H_
#define _INCLUDE_KG3DENVEFFRENDERER_H_

class KG3DEnvironment;
////////////////////////////////////////////////////////////////////////////////
namespace KG3D_ENVIRONMENT_EFFECT
{
	class KG3DEnvEffRenderer 
	{
	public:
		HRESULT Render(KG3DEnvironment& Env, DWORD dwFilter);
		HRESULT PostRender(KG3DEnvironment& Env);//其它都渲染完了之后进行渲染
        HRESULT RenderSun(KG3DEnvironment& Env);
        const D3DXVECTOR4* GetSunPosition(KG3DEnvironment& Env);
	};
};


#endif //_INCLUDE_KG3DENVEFFRENDERER_H_
