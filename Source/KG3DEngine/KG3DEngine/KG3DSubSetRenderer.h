////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSubSetRenderer.h
//  Version     : 1.0
//  Creator     : Yang Lin
//  Create Date : 2008-3-28 17:35:50
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSUBSETRENDERER_H_
#define _INCLUDE_KG3DSUBSETRENDERER_H_
#include "KG3DMaterial.h"
#include "KG3DTemplates.h"
#include "KG3DGraphicsTool.h"
////////////////////////////////////////////////////////////////////////////////
class KG3DRepresentObject;
class KG3DModelSpeedTreeRender;
class KG3DSFX;
class KG3DRepresentObjectSoundBall;
class KG3DSceneRegion;
class KG3DRepresentObjectAntiPortal;
class KG3DSceneSection;
class KG3DSFXSelectTexture;
class KG3DTerrainRender;
class KG3DSceneEntityContainer;
class KG3DMesh;
struct KG3DTerrainConverMap;
struct KG3DTerrainBlockGrassShareVB_High;
struct KG3D_PointLight_Info;
class KG3DRenderState;


struct KG3DModelSubsetInfo 
{
	KG3DModel* pModel;
	DWORD dwSubsetID;
	DWORD dwFaceCount;//面数
	BOOL  bPutInBuffer;//是否被合并绘制

	LPD3DXMESH  pXMeshToRender;
	D3DXMATRIX* pMatWorld;

	KG3DModelSubsetInfo();
	HRESULT UnInit();
};

struct KG3DSubMaterialGroup
{
    std::vector<KG3DModelSubsetInfo *> m_vecSubsetMaterialSet;
	KG3DTexture*   m_lpKG3DTexture;
	DWORD   m_dwAlphaType;//1 none,2 alpha test,4 alpha Blend
	BOOL    m_bNeedRefresh;
	KG3DTerrainBlockGrassShareVB_High* m_lpShareVB;

	HRESULT Optimize(BOOL bUnit);

	HRESULT Clear(TKG3DResourcePool<KG3DModelSubsetInfo>*pInfoPool);
	HRESULT PrintfProfile();
	HRESULT Render(BOOL bForcePass0);

    HRESULT RenderDepthOnlyBegin();
	HRESULT RenderDepthOnly();
    HRESULT RenderDepthOnlyEnd();
    HRESULT RenderColorOnlyBegin(BOOL bForceAlphaTest);
	HRESULT RenderColorOnly(LPD3DXEFFECT pEffect,BOOL bDetail);
    HRESULT RenderColorOnlyEnd();

	HRESULT RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX);
	HRESULT Init();
	HRESULT UnInit();

	HRESULT BeginPass(KG3DModelSubsetInfo &Info,BOOL bForcePass0);

	HRESULT GetShareGrassVB(list<KG3DModelSubsetInfo*>&listSubsetToBuffer,DWORD dwNumFace);
	LPDIRECT3DVERTEXBUFFER9 GetShareGrassVBuffer(DWORD& dwFaces);
	
	KG3DSubMaterialGroup();
	~KG3DSubMaterialGroup();
private:
	static HRESULT SetNewLightShaderConst(LPD3DXEFFECT pShader, KG3DModelSubsetInfo& Info,
		DWORD dwShaderType);

    KG3DRenderState *m_pRenderState;
};

struct KG3DSubsetRender
{
	std::map<DWORD,KG3DSubMaterialGroup*>m_mapMaterialGroup;
	KG3DDefaultShaderType m_ShaderType;

	HRESULT Init();
	HRESULT UnInit(TKG3DResourcePool<KG3DSubMaterialGroup>*pPool,TKG3DResourcePool<KG3DModelSubsetInfo>*pInfoPool);
	HRESULT Clear(TKG3DResourcePool<KG3DModelSubsetInfo>*pInfoPool);
	HRESULT Optimize();

	HRESULT InputMeshSubset(
		DWORD dwSubsetKey,
		KG3DModelSubsetInfo& newsubset,
		TKG3DResourcePool<KG3DSubMaterialGroup>*pPool,
		TKG3DResourcePool<KG3DModelSubsetInfo>*pInfoPool);

	HRESULT RemoveMeshSubset(DWORD dwSubsetKey,KG3DModelSubsetInfo* pSubset,TKG3DResourcePool<KG3DSubMaterialGroup>*pPool,TKG3DResourcePool<KG3DModelSubsetInfo>*pInfoPool);

	HRESULT RenderForColorOnly(DWORD dwAlphaType,BOOL bForceAlphaTest,KG3DTerrainConverMap* pConverMap);
	HRESULT RenderDepthOnly(DWORD dwAlphaType);
	HRESULT Render();

	HRESULT RenderReflect(float fWaterHeight,D3DXPLANE& WaterPlane,D3DXPLANE& WaterPlaneFX);

	KG3DSubsetRender();
	~KG3DSubsetRender();

};

extern HRESULT g_SetPointLighting(LPD3DXEFFECT pShader,KG3D_PointLight_Info* pInfo, DWORD dwMaxPointLightNum = MAX_POINTLIGHT, bool bCommit = true);

class KG3DSceneOutputWnd;
class KG3DTerrainConverMap;
extern VOID g_SetShaderSharedParams(KG3DScene& sceneCur, KG3DSceneOutputWnd& outWnd, KG3DFogParamShader* pfogUnderWater, KG3DTerrainConverMap* pConverMap);//如果有pfogUnderWater，那么使用fogUnderWater的值作为fog的参数，这个只有在水下才那么干。pConverMap可以为NULL，如果不是场景的话
extern VOID g_SetShaderSharedParamsDefault(const D3DXVECTOR3& camPos);	//给特殊的Scene用的
extern VOID g_SetShadowParams(LPD3DXEFFECT pShader,KG3DSceneOutputWnd& outWnd,BOOL bForceDisable = FALSE);

//extern void g_SetShadowInfo(LPD3DXEFFECT pShader );//<ShadowParamMoveToShared>
#endif //_INCLUDE_KG3DSUBSETRENDERER_H_


