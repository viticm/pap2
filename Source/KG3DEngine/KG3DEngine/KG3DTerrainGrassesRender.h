#pragma once
class KG3DTerrainBlockGrass;
class KG3DScene;

class KG3DTerrainConverMap;

#include "KG3DGraphicsTool.h"
#include "KG3DShaderHolder.h"
class KG3DTerrainGrassesRender
{
public:
	HRESULT Init();
	HRESULT UnInit();

	HRESULT OnLostDevice();
	HRESULT OnResetDevice();

	//HRESULT InitShader();

	HRESULT InputGrassBlock(KG3DTerrainBlockGrass* pGrass);
	HRESULT InputStoneBlock(KG3DTerrainBlockGrass* pStone);
	HRESULT InputFrutexBlock(KG3DTerrainBlockGrass* pFrutex);
	HRESULT RemoveGrassBlock(KG3DTerrainBlockGrass* pGrass);
	HRESULT RemoveStoneBlock(KG3DTerrainBlockGrass* pStone);
	HRESULT RemoveFrutexBlock(KG3DTerrainBlockGrass* pFrutex);

	HRESULT RenderGrassDepthOnly(BOOL bShader,BOOL bGrass,BOOL bStone,BOOL bFrutex);
	HRESULT RenderGrassColorOnly(KG3DTerrainConverMap* pConverMap,BOOL bGrass,BOOL bStone,BOOL bFrutex);
	HRESULT RenderGrassColoraAlphaBlendOnly(KG3DTerrainConverMap* pConverMap,BOOL bGrass,BOOL bStone,BOOL bFrutex);

	KG3DTerrainGrassesRender(void);
	~KG3DTerrainGrassesRender(void);

	HRESULT FrameMove();

	DWORD GetVisibleGrassBlockCount();
    DWORD m_dwLastAddRenderCount;
private:
	HRESULT InputGrassBlock(KG3DTerrainBlockGrass* pGrass,set<KG3DTerrainBlockGrass *> *pSet);
	HRESULT EmptyGrassBlockSet(set<KG3DTerrainBlockGrass *> *pSet, vector<KG3DTerrainBlockGrass *> *pRendBlocks);
	HRESULT FrameMoveGrassBlockSet(set<KG3DTerrainBlockGrass *> *pSet, vector<KG3DTerrainBlockGrass *> *pRendBlocks);

	HRESULT EmptyRenderList();
	HRESULT RemoveGrassBlock(KG3DTerrainBlockGrass* pGrass,set<KG3DTerrainBlockGrass *>* pSet);

    set<KG3DTerrainBlockGrass *> m_setTerrainGrassCache;
    set<KG3DTerrainBlockGrass *> m_setTerrainStoneCache;
    set<KG3DTerrainBlockGrass *> m_setTerrainFrutexCache;

    vector<KG3DTerrainBlockGrass *> m_vecGrassRenderBlocks;
    vector<KG3DTerrainBlockGrass *> m_vecStoneRenderBlocks;
    vector<KG3DTerrainBlockGrass *> m_vecFrutexRenderBlocks;


//    LPD3DXEFFECT m_lpEffect;
	//KG3DShaderHolder			m_GrassShader;
    LPDIRECT3DVERTEXDECLARATION9 m_lpDeclaration;
};
