#pragma once
#include "../../../Include/kg3dengine/kg3dinterface.h"
#include "KG3DStructs.h"
#include "KG3DGraphicsTool.h"

using namespace std;

#define  UNIONTEXTUREWIDTH   512
#define  UNIONTEXTUREHEIGHT  512
#define  TREETEXTUREWIDTH    128
#define  TREETEXTUREHEIGHT   128
#define  MAXNUMTREEUNION     1024 //最大允许合并绘制的树木数量

#define VERTEXFVF (D3DFVF_XYZ|D3DFVF_TEX1)

class KG3DModelSpeedTree;
class KG3DCamera;
struct KG3DTerrainConverMap;

class KG3DModelSpeedTreeRender
{
	friend class KG3DSceneEntityMgr;

public:
    vector<KG3DModelSpeedTree*> m_vecSpeedTreeRenderData;

private:
    BOOL m_bEndablePush;
	vector<KG3DModelSpeedTree*>m_vecSpeedTreeInput;//输入的speedtree

protected:

	HRESULT EffectBegin();
	HRESULT EffectEnd();

public:
    virtual HRESULT OnLostDevice();
    virtual HRESULT OnResetDevice();

	HRESULT Init();
	HRESULT UnInit();

	HRESULT BegainPushSpeedTree();
	HRESULT EndPushSpeedTree();
	HRESULT PushSpeedTree(KG3DModelSpeedTree* pTree);

	HRESULT Render();

	HRESULT PrepareSpeedTreeRenderData();
	HRESULT RenderSpeedTreeLeafsSortWithZpass(BOOL bDepthOnly,BOOL bAlphaBlend,KG3DLight& rLight,KG3DTerrainConverMap* pConverMap,float fTrans);
    HRESULT RenderLowPrecisionTree();

	HRESULT RenderSpeedTreeLeafsSort(KG3DLight& rLight,float fTrans);
	HRESULT PopSpeedTree(KG3DModelSpeedTree* pTree);

	KG3DModelSpeedTreeRender(void);
	~KG3DModelSpeedTreeRender(void);
};
