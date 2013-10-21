#pragma once
#include "KG3DCamera.h"
#include "KG3DFrustum.h"

class KG3DSceneEntityMgrShadow;
class KG3DCamera;
class KG3DSceneSceneEditor;
class KG3DSceneEntityMgr;
class KG3DSceneShadowMap;
class KG3DRepresentObjectPVS;
class KG3DModelPointLight;
class KG3DScenePvsEditor;
class KG3DRenderTargetTool;

struct KG3DShadowMapLevel
{
	D3DXMATRIX m_matVP;
	DWORD m_dwShadowmapSize;
	KG3DCamera  m_vCameraLight;

	float      m_fSize;
	int        nIndex;
	BOOL m_bNeedColorSurface;
	BOOL m_bUseINTZ;


	KG3DSceneEntityMgrShadow* m_lpShadowMapEntityMgr;

	LPDIRECT3DTEXTURE9 m_lpSMShadowMap;
	LPDIRECT3DSURFACE9 m_lpSMShadowMapSurface;

	LPDIRECT3DTEXTURE9 m_lpSMColorMap;
	LPDIRECT3DSURFACE9 m_lpSMColorSurface; 

	HRESULT Init(KG3DSceneShadowMap* pShadow,int nLevel,KG3DSceneEntityMgr* pSrcMgr);
	HRESULT Init(DWORD dwSize);

	HRESULT UnInit();

	HRESULT OnLostDevice();
	HRESULT OnResetDevice();

	HRESULT ProcessShadowMap(KG3DCamera* pCameraLight,D3DXVECTOR3& vFocusPos,KG3DSceneSceneEditor* pEditor,KG3DSceneShadowMap* pShadow,LPDIRECT3DSURFACE9 pColorSur);

	HRESULT ProcessShadowMapWithPVS(KG3DRepresentObjectPVS* pPVS);

	KG3DShadowMapLevel();
	~KG3DShadowMapLevel();
};

class KG3DSceneShadowMap
{


public:
	HRESULT SetParametersForShader(LPD3DXEFFECT pShader);

	HRESULT Init(DWORD dwShadowMapSize ,KG3DSceneEntityMgr* pEntityMgr);
	HRESULT UnInit();

    HRESULT OnLostDevice();
    HRESULT OnResetDevice();

	KG3DSceneShadowMap(void);
	~KG3DSceneShadowMap(void);

public:
    BOOL m_bInit;

    D3DXVECTOR3 m_vPositionLast;

	KG3DShadowMapLevel m_ShadowLevel[3];
};


class KG3DPointLightShadow
{
public:
	KG3DShadowMapLevel m_vShadowMap;
	LPDIRECT3DSURFACE9 m_lpDepth;


	HRESULT Init(DWORD dwSize);

	HRESULT RenderForCubeShadowmap();
	HRESULT ComputeCubeMapForPointLight(KG3DModelPointLight* pLight,KG3DScenePvsEditor* pScene,KG3DRenderTargetTool* pTool);

	HRESULT ExportToCubeSurface(KG3DRenderTargetTool* pTool,LPDIRECT3DSURFACE9 pSurface,BOOL bOutputDepth);

	HRESULT ComputeCubeMapForPosition(D3DXVECTOR3 vPosition,KG3DScenePvsEditor* pScene,
		LPDIRECT3DCUBETEXTURE9 pCubeColor,LPDIRECT3DCUBETEXTURE9 pCubeDepth,
		KG3DRenderTargetTool* pToolColor ,KG3DRenderTargetTool*  pToolDepth);

	KG3DPointLightShadow();
	~KG3DPointLightShadow();
};