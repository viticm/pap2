////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DModelShadowRendererSolid.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2007-9-17 15:35:13
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DMODELSHADOWRENDERER_H_
#define _INCLUDE_KG3DMODELSHADOWRENDERER_H_

#include "KG3DCommonObject.h"
////////////////////////////////////////////////////////////////////////////////
class KG3DTexture;
struct KG3DModelShadowRendererEffectParams;
class KG3DModel;

struct KG3DModelShadowRenderer : public KG3DCommonObjectSimple
{
	virtual HRESULT Render(KGCH::TFrontAccessProxy<KG3DModel*>& SortedModelsIt) = 0;	//保证同时只有一个Scene在用，不然会出错，现在StrentchRect的操作异步
	STDMETHOD(FrameMove)(THIS)PURE;
	virtual ULONG STDMETHODCALLTYPE Release( void)PURE;
	virtual HRESULT InputShadowDummy(const D3DXVECTOR3& Pos, FLOAT fScaleFactor) = 0;//外部的，需要绘制脚底影子的，从这里Input。必须在渲染过程中Input，然后在Render中会被Flush掉。因为ModelShadowRender是Singleton，所以要在但个渲染过程中保持对其独占，即在一个Render中Input并Render。当然如果不用这个函数，就没有这个问题。
	virtual ULONG	SynchronizedStrenchRect(BOOL bSynchronized) = 0;	//返回旧的状态
	virtual ~KG3DModelShadowRenderer() = 0{}

	static bool IsModelShadowNeedToBeRender(KG3DModel& modelRef);
};

struct KG3DModelShadowRendererDummy : public KG3DModelShadowRenderer
{
	STDMETHOD_(ULONG, GetType)(){return 0;}
	virtual HRESULT Render(KGCH::TFrontAccessProxy<KG3DModel*>& SortedModelsIt){return E_FAIL;}
	STDMETHOD(FrameMove)(THIS){return E_FAIL;}
	virtual ULONG STDMETHODCALLTYPE Release( void){return 0;}
	virtual HRESULT InputShadowDummy(const D3DXVECTOR3& Pos, FLOAT fScaleFactor){return E_FAIL;}
	virtual ULONG	SynchronizedStrenchRect(BOOL bSynchronized){return TRUE;}
};

KG3DModelShadowRenderer& g_GetModelShadowRenderer();//需要时会自动构建
VOID	g_ModelShadowRendererRelease();


#endif //_INCLUDE_KG3DMODELSHADOWRENDERER_H_
