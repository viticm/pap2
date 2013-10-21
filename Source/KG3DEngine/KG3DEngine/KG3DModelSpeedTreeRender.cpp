#include "StdAfx.h"
#include "MathTool.h"
#include ".\kg3dmodelspeedtreerender.h"
#include "kg3dgraphicstool.h"
#include "kg3denginemanager.h"
#include "KG3DModelSpeedTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
extern BOOL g_bRenderTree;

KG3DModelSpeedTreeRender::KG3DModelSpeedTreeRender(void)
{
    m_bEndablePush = FALSE;
}

KG3DModelSpeedTreeRender::~KG3DModelSpeedTreeRender(void)
{
	ASSERT(m_vecSpeedTreeRenderData.empty());
	ASSERT(m_vecSpeedTreeInput.empty());
}

HRESULT KG3DModelSpeedTreeRender::Init()
{
	return S_OK;
}

HRESULT KG3DModelSpeedTreeRender::UnInit()
{
    m_vecSpeedTreeRenderData.clear();

    for (size_t i = 0; i < m_vecSpeedTreeInput.size(); ++i)
    {
        SAFE_RELEASE(m_vecSpeedTreeInput[i]);
    }

    m_vecSpeedTreeInput.clear();

    m_bEndablePush = FALSE;

    return S_OK;
}

HRESULT KG3DModelSpeedTreeRender::EffectBegin()
{
	return S_OK;
}

HRESULT KG3DModelSpeedTreeRender::EffectEnd()
{
	return S_OK;
}

HRESULT KG3DModelSpeedTreeRender::BegainPushSpeedTree()
{
    m_vecSpeedTreeRenderData.clear();
    for (size_t i = 0; i < m_vecSpeedTreeInput.size(); ++i)
    {
        SAFE_RELEASE(m_vecSpeedTreeInput[i]);
    } 
    m_vecSpeedTreeInput.clear();
    m_bEndablePush = TRUE;

	return S_OK;
}

HRESULT KG3DModelSpeedTreeRender::EndPushSpeedTree()
{
	m_bEndablePush = FALSE;
	return S_OK;
}

HRESULT KG3DModelSpeedTreeRender::PushSpeedTree(KG3DModelSpeedTree* pTree)
{
    HRESULT hrResult = E_FAIL;

    KG_PROCESS_ERROR(m_bEndablePush);
    KG_PROCESS_ERROR(pTree);
    
    pTree->AddRef();
	m_vecSpeedTreeInput.push_back(pTree);
    
    hrResult = S_OK;
Exit0:
	return hrResult;
}

HRESULT KG3DModelSpeedTreeRender::PopSpeedTree(KG3DModelSpeedTree* pTree)
{
	vector<KG3DModelSpeedTree*>::iterator i = m_vecSpeedTreeInput.begin();//输入的speedtree
	vector<KG3DModelSpeedTree*>::iterator iend = m_vecSpeedTreeInput.end();//输入的speedtree

    while (i!=iend)
    {
        KG3DModelSpeedTree* pSpeedTree = *i;
        if(pSpeedTree == pTree)
        {
            m_vecSpeedTreeInput.erase(i);
            SAFE_RELEASE(pSpeedTree);
            break;
        }
        i++;
    }

    i = m_vecSpeedTreeRenderData.begin();
    iend = m_vecSpeedTreeRenderData.end();
    for (NULL; i != iend; ++i)
    {
        KG3DModelSpeedTree *pSpeedTree = *i;
        if (pSpeedTree == pTree)
        {
            m_vecSpeedTreeRenderData.erase(i);
            break;
        }
    }
    
	return S_OK;
}


HRESULT KG3DModelSpeedTreeRender::OnLostDevice()
{
    return S_OK;
}

HRESULT KG3DModelSpeedTreeRender::OnResetDevice()
{
    return S_OK;
}

HRESULT KG3DModelSpeedTreeRender::Render()
{
	if (!g_bRenderTree)
		return S_OK;

	KG3DScene* pScene = g_cGraphicsTool.GetCurScene();
	if(pScene)
	{
		KG3DLight& vLight = g_cGraphicsTool.GetSunLight(*pScene);
		RenderSpeedTreeLeafsSort(vLight,0);
	}

	return S_OK;
}

bool SortComp(KG3DModelSpeedTree* pTree1,KG3DModelSpeedTree* pTree2)
{
	return pTree1->m_uFileHashID < pTree2->m_uFileHashID;
}

HRESULT KG3DModelSpeedTreeRender::PrepareSpeedTreeRenderData()
{
    KG3DModelSpeedTree::InitBillBoardTree();

    m_vecSpeedTreeRenderData.clear();

	for (size_t i =0;i<m_vecSpeedTreeInput.size();i++)
	{
		KG3DModelSpeedTree* pTree = m_vecSpeedTreeInput[i];

        if (g_cEngineOption.bUseSpeedTreeLod && pTree->IsFaraway())
        {
            KG3DModelSpeedTree::AddBillBoardTree(pTree);
        }
        else 
        {
            m_vecSpeedTreeRenderData.push_back(pTree);
        }
	}

    sort(m_vecSpeedTreeRenderData.begin(), m_vecSpeedTreeRenderData.end(), SortComp);

	return S_OK;
}

HRESULT KG3DModelSpeedTreeRender::RenderSpeedTreeLeafsSortWithZpass(BOOL bDepthOnly,BOOL bAlphaBlend,KG3DLight& rLight,KG3DTerrainConverMap* pConverMap,float fTrans)
{
	//GraphicsStruct::SamplerState sa( 0, D3DSAMP_MIPMAPLODBIAS, FtoDW(-1.0f));
	HRESULT hrRetCode = E_FAIL;
	if (!g_bRenderTree)
		return S_OK;

    if (m_vecSpeedTreeRenderData.empty())
        return S_OK;

	int nPass = 1;
	if (bDepthOnly)
	{
		nPass = 1;
	}
	else
	{
		if(bAlphaBlend)
			nPass = 3;
		else
			nPass = 2;
    }

    KG3DModelSpeedTree *pSpeedTree = m_vecSpeedTreeRenderData[0];
    UINT uPreFileHash = 0;
    BOOL bApplyMtlSuccess = FALSE;
    pSpeedTree->RenderLeafsBegin(nPass,rLight,pConverMap,bDepthOnly,fTrans);

    for (size_t i = 0; i < m_vecSpeedTreeRenderData.size(); ++i)
    {
        pSpeedTree = m_vecSpeedTreeRenderData[i];
        if (i == 0 || pSpeedTree->m_uFileHashID != uPreFileHash)
        {
            hrRetCode = pSpeedTree->ApplyLeafsMaterial();
            bApplyMtlSuccess = SUCCEEDED(hrRetCode);
            uPreFileHash = pSpeedTree->m_uFileHashID;
        }

        if (bApplyMtlSuccess) // 材质设置成功才渲染了
        {
            pSpeedTree->RenderLeafsWithoutEffectAndMtlChange();
        }
    }

    pSpeedTree->RenderLeafsEnd();
 
	return S_OK;
}


HRESULT KG3DModelSpeedTreeRender::RenderSpeedTreeLeafsSort(KG3DLight& rLight,float fTrans)
{
    HRESULT hrRetCode = E_FAIL;

	if (!g_bRenderTree)
		return S_OK;

    if (m_vecSpeedTreeRenderData.empty())
        return S_OK;

    KG3DModelSpeedTree *pSpeedTree = m_vecSpeedTreeRenderData[0];
    UINT uPreFileHash = 0;
    BOOL bApplyMtlSuccess = FALSE;
    pSpeedTree->RenderLeafsBegin(0, rLight, NULL, FALSE,fTrans);
    for (size_t i = 0; i < m_vecSpeedTreeRenderData.size(); ++i)
    {
        pSpeedTree = m_vecSpeedTreeRenderData[i];
        if (i == 0 || pSpeedTree->m_uFileHashID != uPreFileHash)
        {
            hrRetCode = pSpeedTree->ApplyLeafsMaterial();
            bApplyMtlSuccess = SUCCEEDED(hrRetCode);
            uPreFileHash = pSpeedTree->m_uFileHashID;
        }

        if (bApplyMtlSuccess) // 材质设置成功才渲染了
        {
            pSpeedTree->RenderLeafsWithoutEffectAndMtlChange();
        }
    }
    pSpeedTree->RenderLeafsEnd();
   
	return S_OK;
}

HRESULT KG3DModelSpeedTreeRender::RenderLowPrecisionTree()
{
    if (!g_bRenderTree)
        return S_OK;

    KG3DModelSpeedTree::RenderBillboardTree();

    return S_OK;
}
