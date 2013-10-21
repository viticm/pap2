////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KTgaManager.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-19 14:00:10
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <algorithm>
#include "KTga.h"
#include "KTgaManager.h"
#include "UiTex.h"
#include "KWndNameList.h"
#include "KShow.h"
#include "KAnimateManager.h"
#include "KPickBox.h"
#include "KSaveManager.h"

////////////////////////////////////////////////////////////////////////////////

#define FRAME_DISTANCE 1

static LPCTSTR gs_pszSortTypeName[] = 
{
    _T("(按名称排序)"),
    _T("(按宽排序)"),
    _T("(按高排序)"),
    _T("(按体积排序)"),
    _T("(按存储先后排序)"),
    _T("(按拼图宽排序)"),
    _T("(按拼图高排序)"),
    _T("(按拼图面积排序)"),
};

LPCTSTR g_GetSortTypeName(int nSortType)
{
    if (nSortType >= SORT_TYPE_BEGINE && nSortType < SORT_TYPE_END)
        return gs_pszSortTypeName[nSortType];
    return NULL;
}


//判断两个矩形是否相交，输入矩形left <= right, top <= bottom 相交返回非零
/*
static inline int g_IntersectRect(const RECT &rcSrc1,  const RECT &rcSrc2)
{
    if (
        rcSrc2.left >= rcSrc1.right ||
        rcSrc2.right <= rcSrc1.left ||
        rcSrc2.top >= rcSrc1.bottom ||
        rcSrc2.bottom <= rcSrc1.top
    )
        return false;
    return true;
}
*/
/*
static inline int g_IntersectRect(const RECT &rcSrc1,  const RECT &rcSrc2)
{
	if (
		rcSrc2.left > rcSrc1.right + FRAME_DISTANCE ||
		rcSrc2.right < rcSrc1.left - FRAME_DISTANCE ||
		rcSrc2.top > rcSrc1.bottom + FRAME_DISTANCE ||
		rcSrc2.bottom < rcSrc1.top - FRAME_DISTANCE
		)
		return false;
	return true;
}
*/

static inline int g_IntersectRect(const RECT &rcSrc1,  const RECT &rcSrc2)
{
	if (
		rcSrc2.left - FRAME_DISTANCE >= rcSrc1.right + FRAME_DISTANCE ||
		rcSrc2.right + FRAME_DISTANCE <= rcSrc1.left - FRAME_DISTANCE ||
		rcSrc2.top - FRAME_DISTANCE >= rcSrc1.bottom + FRAME_DISTANCE ||
		rcSrc2.bottom + FRAME_DISTANCE <= rcSrc1.top - FRAME_DISTANCE
		)
		return false;
	return true;
}


static inline int g_IsTwoRectNeighbors(const RECT &rcSrc1, const RECT &rcSrc2)
{
    RECT rcEnlarg1;
    rcEnlarg1.left = rcSrc1.left - MAX_NEIGHBOR_DISTANCE;
    rcEnlarg1.top = rcSrc1.top - MAX_NEIGHBOR_DISTANCE;
    rcEnlarg1.right = rcSrc1.right + MAX_NEIGHBOR_DISTANCE;
    rcEnlarg1.bottom = rcSrc1.bottom + MAX_NEIGHBOR_DISTANCE;

    if (g_IntersectRect(rcEnlarg1, rcSrc2))
        return true;
    return false;
}

//获得正确的可以放置图片的位置
static inline int g_GetCorrectPosition(int x)
{
	return x;// ((x + 1) / 2) * 2;
}

KTgaManager *KTgaManager::ms_pSelf = NULL;

KTgaManager::KTgaManager()
{
    m_nSaveHeight = 0;
    m_nSaveWidth = 0;
}

KTgaManager::~KTgaManager()
{
}

int KTgaManager::Init()
{
    int nResult = false;

    ASSERT(!ms_pSelf);
    ms_pSelf = new KTgaManager;
    KG_PROCESS_ERROR(ms_pSelf);

    nResult = true;
Exit0:
    if (!nResult)
        KG_DELETE(ms_pSelf);
    return nResult;
}

int KTgaManager::Exit()
{
    if (ms_pSelf) 
        ms_pSelf->Clear();
    KG_DELETE(ms_pSelf);

    return true;
}

KTgaManager &KTgaManager::GetSelf()
{
    ASSERT(ms_pSelf);
    return *ms_pSelf;
}

int KTgaManager::Clear()
{
    for (KTgaBlockArray::iterator it = m_aTgaBlock.begin(); it != m_aTgaBlock.end(); ++it)
    {
        ASSERT(*it);
        KG_COM_RELEASE(*it);
    }
    m_aTgaBlock.clear();

    ClearSaveData();

    UpDataListView();

    return true;
}

int KTgaManager::IsTgaExist(LPCTSTR pcszName)
{
    if (!pcszName)
        return false;

    for (KTgaBlockArray::iterator it = m_aTgaBlock.begin(); it != m_aTgaBlock.end(); ++it) 
    {
        ASSERT(*it);
        TCHAR szName[MAX_PATH];
        (*it)->GetName(szName, sizeof(szName) / sizeof(TCHAR));
        if (_tcsicmp(szName, pcszName) == 0)
            return true;
    }

    return false;
}

KTgaBlock *KTgaManager::Get(int nIndex)
{
    if (nIndex >= 0 && nIndex < (int)m_aTgaBlock.size())
        return (*(m_aTgaBlock.begin() + nIndex));
    return NULL;
}

KTgaBlock *KTgaManager::Get(LPCTSTR pcszName)
{
    if (!pcszName)
        return NULL;
    for (KTgaBlockArray::iterator it = m_aTgaBlock.begin(); it != m_aTgaBlock.end(); ++it)
    {
        if (_tcsicmp(pcszName, (*it)->GetName()) == 0)
            return (*it);
    }

    return NULL;
}

int KTgaManager::GetIndexByUnPosedIndex(int nUnPosedIndex)
{
    int nIndex = -1;
    int nCount = (int)m_aTgaBlock.size();
    if (nUnPosedIndex >= 0)
    {
        for (int i = 0; i < nCount; ++i) 
        {
            if (!(m_aTgaBlock[i]->IsPosHasBeenSeted()))
            {
                if (--nUnPosedIndex < 0)
                {
                    nIndex = i;
                    break;
                }
            }
        }
    }
    return nIndex;
}

int KTgaManager::GetIndexByPosedIndex(int nPosedIndex)
{
    int nIndex = -1;
    
    if (nPosedIndex >= 0)
    {
        int nCount = (int)m_aTgaBlock.size();
        for (int i = 0; i < nCount; ++i) 
        {
            if (m_aTgaBlock[i]->IsPosHasBeenSeted())
            {
                if (--nPosedIndex < 0)
                {
                    nIndex = i;
                    break;
                }
            }
        }
    }
    return nIndex;
}

int KTgaManager::GetPosedBlockIndex(int x, int y)
{
     int nIndex = -1;
     int nCount = (int)m_aTgaBlock.size();
     for (int i = 0; i < nCount; ++i)
     {
         if (
             m_aTgaBlock[i]->IsPosHasBeenSeted() &&
             m_aTgaBlock[i]->PtInBlock(x, y)
         )
         {
            nIndex = i;
            break;
         }
    }
    return nIndex;
}

int KTgaManager::GetIndex(LPCTSTR pcszName)
{
    int nIndex = -1;
    TCHAR szName[MAX_PATH];
    int nCount = (int)m_aTgaBlock.size();
    for (int i = 0; i < nCount; ++i)
    {
        m_aTgaBlock[i]->GetName(szName, sizeof(szName) / sizeof(TCHAR));

        if (_tcsicmp(szName, pcszName) == 0)
        {
            nIndex = i;
            break;
        }
    }

    return nIndex;
}

int KTgaManager::Append(KTgaBlock *pTgaBlock)
{
    int nResult = false;
    KG_PROCESS_ERROR(pTgaBlock);
    m_aTgaBlock.push_back(pTgaBlock);

    UpDataListView();

    nResult = true;
Exit0:
    return nResult;
}

int KTgaManager::Remove(KTgaBlock *pTgaBlock)
{
    int nResult = false;
    
    KTgaBlockArray::iterator it = 
        std::find(m_aTgaBlock.begin(), m_aTgaBlock.end(), pTgaBlock);
    KG_PROCESS_ERROR(it != m_aTgaBlock.end());
    m_aTgaBlock.erase(it);

    UpDataListView();

    nResult = true;
Exit0:
    return nResult;
}

int KTgaManager::Sort(int nSortType)
{
    KPickBox::GetSelf().Clear();

    switch (nSortType) 
    {
    case SORT_BY_NAME:
        std::sort(m_aTgaBlock.begin(), m_aTgaBlock.end(), &KTgaManager::SortByNameFunction);
        break;
    case SORT_BY_WIDTH:
        std::sort(m_aTgaBlock.begin(), m_aTgaBlock.end(), &KTgaManager::SortByWidthFunction);
        break;
    case SORT_BY_HEIGHT:
        std::sort(m_aTgaBlock.begin(), m_aTgaBlock.end(), &KTgaManager::SortByHeightFunction);
        break;
    case SORT_BY_SIZE:
        std::sort(m_aTgaBlock.begin(), m_aTgaBlock.end(), &KTgaManager::SortBySizeFunction);
    	break;
    case SORT_TO_SAVE:
        std::sort(m_aTgaBlock.begin(), m_aTgaBlock.end(), &KTgaManager::SortToSaveFunction);
        break;
    case SORT_TO_AUTO_MAKE_WIDTH:
        std::sort(m_aTgaBlock.begin(), m_aTgaBlock.end(), &KTgaManager::SortToAutoMakeupWidth);
        break;
    case SORT_TO_AUTO_MAKE_HEIGHT:
        std::sort(m_aTgaBlock.begin(), m_aTgaBlock.end(), &KTgaManager::SortToAutoMakeupHeight);
        break;
    case SORT_TO_AUTO_MAKE_SIZE:
        std::sort(m_aTgaBlock.begin(), m_aTgaBlock.end(), &KTgaManager::SortToAutoMakeupSize);
        break;
    default:
        break;
    }
    
    UpDataListView();

    return true;
}

bool KTgaManager::SortByNameFunction(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2)
{
    int nRetCode = false;
    ASSERT(pTgaBlock1);
    ASSERT(pTgaBlock2);

    TCHAR szNameBlock1[MAX_TGA_BLOCK_NAME_LEN];
    TCHAR szNameBlock2[MAX_TGA_BLOCK_NAME_LEN];
    pTgaBlock1->GetName(szNameBlock1, sizeof(szNameBlock1) / sizeof(TCHAR));
    pTgaBlock2->GetName(szNameBlock2, sizeof(szNameBlock2) / sizeof(TCHAR));
    nRetCode = _tcsicmp(szNameBlock1, szNameBlock2);
    if (nRetCode > 0)
        return true;
    return false;
    
}

bool KTgaManager::SortByWidthFunction(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2)
{
    ASSERT(pTgaBlock1);
    ASSERT(pTgaBlock2);

    if (pTgaBlock1->GetWidth() > pTgaBlock2->GetWidth())
        return true;
    return false;
}

bool KTgaManager::SortByHeightFunction(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2)
{
    ASSERT(pTgaBlock1);
    ASSERT(pTgaBlock2);

    if (pTgaBlock1->GetHeight() > pTgaBlock2->GetHeight())
        return true;
    return false;
}

bool KTgaManager::SortBySizeFunction(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2)
{
    ASSERT(pTgaBlock1);
    ASSERT(pTgaBlock2);

    if (pTgaBlock1->GetBufferSize() > pTgaBlock2->GetBufferSize())
        return true;
    return false;
}

bool KTgaManager::SortToSaveFunction(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2)
{
    ASSERT(pTgaBlock1);
    ASSERT(pTgaBlock2);
    
    if (pTgaBlock1->IsIndexChangeable())
        return false;

    if (pTgaBlock2->IsIndexChangeable())
        return true;

    if (pTgaBlock1->GetOriginalIndex() < pTgaBlock2->GetOriginalIndex())
        return true;

    return false;
}

bool KTgaManager::SortToAutoMakeupWidth(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2)
{
    ASSERT(pTgaBlock1);
    ASSERT(pTgaBlock2);


    if (pTgaBlock1->IsPosHasBeenSeted())
	{
		if (pTgaBlock2->IsPosHasBeenSeted())
			return false;
        return true;
	}

    if (pTgaBlock2->IsPosHasBeenSeted())
        return false;

    if (pTgaBlock1->GetWidth() > pTgaBlock2->GetWidth())
        return true;

    return false;
}

bool KTgaManager::SortToAutoMakeupHeight(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2)
{
    ASSERT(pTgaBlock1);
    ASSERT(pTgaBlock2);


    if (pTgaBlock1->IsPosHasBeenSeted())
	{
		if (pTgaBlock2->IsPosHasBeenSeted())
			return false;
		return true;
	}

    if (pTgaBlock2->IsPosHasBeenSeted())
        return false;

    if (pTgaBlock1->GetHeight() > pTgaBlock2->GetHeight())
        return true;

    return false;
}

bool KTgaManager::SortToAutoMakeupSize(KTgaBlock *pTgaBlock1, KTgaBlock *pTgaBlock2)
{
    ASSERT(pTgaBlock1);
    ASSERT(pTgaBlock2);

    if (pTgaBlock1->IsPosHasBeenSeted())
	{
		if (pTgaBlock2->IsPosHasBeenSeted())
			return false;
		return true;
	}

    if (pTgaBlock2->IsPosHasBeenSeted())
        return false;

    if (pTgaBlock1->GetWidth() * pTgaBlock1->GetHeight() >
        pTgaBlock2->GetWidth() * pTgaBlock2->GetHeight()
    )
        return true;

    return false;
}

int KTgaManager::UpDataListView()
{
    LPCTSTR pcszClip = NULL;
    LPCTSTR pcszTemp = NULL;
    TCHAR szName[MAX_PATH];
	KWndNameList::GetSelf().Clear();
    
    for (KTgaBlockArray::iterator it = m_aTgaBlock.begin(); it != m_aTgaBlock.end(); ++it) 
    {
        ASSERT(*it);
        if ((*it)->IsPosHasBeenSeted())
            continue;

        (*it)->GetName(szName, sizeof(szName) / sizeof(TCHAR));
        pcszClip = _tcsrchr(szName, _T('\\'));
        if (!pcszClip)
            pcszClip = szName;
        pcszTemp = _tcsrchr(pcszClip, _T('/'));
        if (pcszTemp)
            pcszClip = pcszTemp;
        KWndNameList::GetSelf().Append(++pcszClip);
    }
    
    return true;
}

int KTgaManager::UnMakeAll()
{
    for (KTgaBlockArray::iterator it = m_aTgaBlock.begin(); it != m_aTgaBlock.end(); ++it)
    {
        ASSERT(*it);
        (*it)->ClearPosHasBeenSeted();
    }

    UpDataListView();
    UpDataShow();

    return true;
}

int KTgaManager::UpDataShow()
{
    RECT rcTemp;
    KTgaBlock *pBlock = NULL;
    KShow::GetSelf().Clear();
    for (KTgaBlockArray::iterator it = m_aTgaBlock.begin(); it != m_aTgaBlock.end(); ++it) 
    {
        pBlock = (*it);
        ASSERT(pBlock);
        if (pBlock->IsPosHasBeenSeted())
        {
            rcTemp.left = pBlock->GetLeft();
            rcTemp.top = pBlock->GetTop();
            rcTemp.right = pBlock->GetWidth();
            rcTemp.bottom = pBlock->GetHeight();
            KShow::GetSelf().AppendBuffer(pBlock);
            KShow::GetSelf().AppendShowRect(rcTemp);
        }
    }
    KShow::GetSelf().CreateBitMap(); 

    return true;
} 

int KTgaManager::DrawImage()
{
	struct _VS
	{
		D3DXVECTOR4 p;
		float u;
		float v;
	};

	LPDIRECT3DVERTEXBUFFER9 pVS = NULL;
	HRESULT hr = E_FAIL;
	int nDeltaX = 0;
	int nDeltaY = 0;
	LPDIRECT3DDEVICE9 pDevice = KShow::GetSelf().GetDevice();
	KG_PROCESS_ERROR(pDevice);

	nDeltaX = KShow::GetSelf().GetShowStartX();
	nDeltaY = KShow::GetSelf().GetShowStartY();

	hr = pDevice->CreateVertexBuffer(6 * sizeof(_VS), 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &pVS, NULL);
	KG_COM_PROCESS_ERROR(hr);

	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);

	for (KTgaBlockArray::iterator it = m_aTgaBlock.begin(); it != m_aTgaBlock.end(); ++it) 
	{
		KTgaBlock * pBlock = (*it);
		ASSERT(pBlock);
		LPDIRECT3DTEXTURE9 pTexture = pBlock->GetTexture();
		_VS* data = NULL;
		if (pBlock->IsPosHasBeenSeted() && pTexture && SUCCEEDED(pVS->Lock(0, 0, (void**)&data, 0)))
		{
			data[0].p = D3DXVECTOR4(pBlock->GetLeft() - nDeltaX, pBlock->GetTop() - nDeltaY, 0.f, 1.f);
			data[0].u = 0.f;
			data[0].v = 0.f;
			data[1].p = D3DXVECTOR4(data[0].p.x + pBlock->GetWidth(), data[0].p.y, 0.f, 1.f);
			data[1].u = 1.f;
			data[1].v = 0.f;
			data[2].p = D3DXVECTOR4(data[1].p.x, data[0].p.y + pBlock->GetHeight(), 0.f, 1.f);
			data[2].u = 1.f;
			data[2].v = 1.f;
			data[3].p = data[0].p;
			data[3].u = 0.f;
			data[3].v = 0.f;
			data[4].p = data[2].p;
			data[4].u = 1.f;
			data[4].v = 1.f;
			data[5].p = D3DXVECTOR4(data[0].p.x, data[2].p.y, 0.f, 1.f);
			data[5].u = 0.f;
			data[5].v = 1.f;

			pVS->Unlock();

			pDevice->SetStreamSource(0, pVS, 0, sizeof(_VS));
			pDevice->SetTexture(0, pTexture);
			pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
		}
	}

Exit0:
	KG_COM_RELEASE(pVS);
	return true;
}

int KTgaManager::DrawLine(DWORD dwColor)
{
	struct _VS
	{
		D3DXVECTOR4 p;
		DWORD dwDiffuse;
	};

	LPDIRECT3DVERTEXBUFFER9 pVS = NULL;
	HRESULT hr = E_FAIL;
	int nDeltaX = 0;
	int nDeltaY = 0;
	LPDIRECT3DDEVICE9 pDevice = KShow::GetSelf().GetDevice();
	KG_PROCESS_ERROR(pDevice);

	nDeltaX = KShow::GetSelf().GetShowStartX();
	nDeltaY = KShow::GetSelf().GetShowStartY();

	hr = pDevice->CreateVertexBuffer(8 * sizeof(_VS), 0, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &pVS, NULL);
	KG_COM_PROCESS_ERROR(hr);

	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pDevice->SetTexture(0, NULL);

	for (KTgaBlockArray::iterator it = m_aTgaBlock.begin(); it != m_aTgaBlock.end(); ++it) 
	{
		KTgaBlock * pBlock = (*it);
		ASSERT(pBlock);
		LPDIRECT3DTEXTURE9 pTexture = pBlock->GetTexture();
		_VS* data = NULL;
		if (pBlock->IsPosHasBeenSeted() && pTexture && SUCCEEDED(pVS->Lock(0, 0, (void**)&data, 0)))
		{
			data[0].p = D3DXVECTOR4(pBlock->GetLeft() - nDeltaX, pBlock->GetTop() - nDeltaY, 0.f, 1.f);
			data[0].dwDiffuse = dwColor;
			data[1].p = D3DXVECTOR4(data[0].p.x + pBlock->GetWidth(), data[0].p.y, 0.f, 1.f);
			data[1].dwDiffuse = dwColor;

			data[2].p = data[1].p;
			data[2].dwDiffuse = dwColor;
			data[3].p = D3DXVECTOR4(data[1].p.x, data[0].p.y + pBlock->GetHeight(), 0.f, 1.f);
			data[3].dwDiffuse = dwColor;

			data[4].p = data[3].p;
			data[4].dwDiffuse = dwColor;
			data[5].p = D3DXVECTOR4(data[0].p.x, data[3].p.y, 0.f, 1.f);
			data[5].dwDiffuse = dwColor;

			data[6].p = data[5].p;
			data[6].dwDiffuse = dwColor;
			data[7].p = data[0].p;
			data[7].dwDiffuse = dwColor;

			pVS->Unlock();

			pDevice->SetStreamSource(0, pVS, 0, sizeof(_VS));
			pDevice->DrawPrimitive(D3DPT_LINELIST, 0, 4);
		}
	}

Exit0:
	KG_COM_RELEASE(pVS);
	return true;
}

int KTgaManager::DrawImage(int nIndex)
{
	struct _VS
	{
		D3DXVECTOR4 p;
		float u;
		float v;
	};

	LPDIRECT3DVERTEXBUFFER9 pVS = NULL;
	HRESULT hr = E_FAIL;
	int nDeltaX = 0;
	int nDeltaY = 0;
	KTgaBlock * pBlock = NULL;
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	_VS* data = NULL;
	LPDIRECT3DDEVICE9 pDevice = KShow::GetSelf().GetDevice();
	KG_PROCESS_ERROR(pDevice);

	KG_PROCESS_ERROR(nIndex >= 0 && nIndex < m_aTgaBlock.size());

	pBlock = m_aTgaBlock[nIndex];
	ASSERT(pBlock);

	pTexture = pBlock->GetTexture();
	KG_PROCESS_ERROR(pTexture);

	nDeltaX = KShow::GetSelf().GetShowStartX();
	nDeltaY = KShow::GetSelf().GetShowStartY();

	hr = pDevice->CreateVertexBuffer(6 * sizeof(_VS), 0, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_MANAGED, &pVS, NULL);
	KG_COM_PROCESS_ERROR(hr);

	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);

	if (SUCCEEDED(pVS->Lock(0, 0, (void**)&data, 0)))
	{
		data[0].p = D3DXVECTOR4(pBlock->GetLeft() - nDeltaX, pBlock->GetTop() - nDeltaY, 0.f, 1.f);
		data[0].u = 0.f;
		data[0].v = 0.f;
		data[1].p = D3DXVECTOR4(data[0].p.x + pBlock->GetWidth(), data[0].p.y, 0.f, 1.f);
		data[1].u = 1.f;
		data[1].v = 0.f;
		data[2].p = D3DXVECTOR4(data[1].p.x, data[0].p.y + pBlock->GetHeight(), 0.f, 1.f);
		data[2].u = 1.f;
		data[2].v = 1.f;
		data[3].p = data[0].p;
		data[3].u = 0.f;
		data[3].v = 0.f;
		data[4].p = data[2].p;
		data[4].u = 1.f;
		data[4].v = 1.f;
		data[5].p = D3DXVECTOR4(data[0].p.x, data[2].p.y, 0.f, 1.f);
		data[5].u = 0.f;
		data[5].v = 1.f;

		pVS->Unlock();

		pDevice->SetStreamSource(0, pVS, 0, sizeof(_VS));
		pDevice->SetTexture(0, pTexture);
		pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 2);
	}

Exit0:
	KG_COM_RELEASE(pVS);
	return true;
}

int KTgaManager::DrawLine(int nIndex, DWORD dwColor)
{
	struct _VS
	{
		D3DXVECTOR4 p;
		DWORD dwDiffuse;
	};

	LPDIRECT3DVERTEXBUFFER9 pVS = NULL;
	HRESULT hr = E_FAIL;
	int nDeltaX = 0;
	int nDeltaY = 0;
	KTgaBlock * pBlock = NULL;
	LPDIRECT3DTEXTURE9 pTexture = NULL;
	_VS* data = NULL;
	LPDIRECT3DDEVICE9 pDevice = KShow::GetSelf().GetDevice();
	KG_PROCESS_ERROR(pDevice);

	KG_PROCESS_ERROR(nIndex >= 0 && nIndex < m_aTgaBlock.size());

	pBlock = m_aTgaBlock[nIndex];
	ASSERT(pBlock);

	pTexture = pBlock->GetTexture();
	KG_PROCESS_ERROR(pTexture);

	nDeltaX = KShow::GetSelf().GetShowStartX();
	nDeltaY = KShow::GetSelf().GetShowStartY();

	hr = pDevice->CreateVertexBuffer(8 * sizeof(_VS), 0, D3DFVF_XYZRHW | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, &pVS, NULL);
	KG_COM_PROCESS_ERROR(hr);

	pDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	pDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	pDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_DIFFUSE);
	pDevice->SetTexture(0, NULL);

	if (SUCCEEDED(pVS->Lock(0, 0, (void**)&data, 0)))
	{
		data[0].p = D3DXVECTOR4(pBlock->GetLeft() - nDeltaX, pBlock->GetTop() - nDeltaY, 0.f, 1.f);
		data[0].dwDiffuse = dwColor;
		data[1].p = D3DXVECTOR4(data[0].p.x + pBlock->GetWidth(), data[0].p.y, 0.f, 1.f);
		data[1].dwDiffuse = dwColor;

		data[2].p = data[1].p;
		data[2].dwDiffuse = dwColor;
		data[3].p = D3DXVECTOR4(data[1].p.x, data[0].p.y + pBlock->GetHeight(), 0.f, 1.f);
		data[3].dwDiffuse = dwColor;

		data[4].p = data[3].p;
		data[4].dwDiffuse = dwColor;
		data[5].p = D3DXVECTOR4(data[0].p.x, data[3].p.y, 0.f, 1.f);
		data[5].dwDiffuse = dwColor;

		data[6].p = data[5].p;
		data[6].dwDiffuse = dwColor;
		data[7].p = data[0].p;
		data[7].dwDiffuse = dwColor;

		pVS->Unlock();

		pDevice->SetStreamSource(0, pVS, 0, sizeof(_VS));
		pDevice->DrawPrimitive(D3DPT_LINELIST, 0, 4);
	}

Exit0:
	KG_COM_RELEASE(pVS);
	return true;
}

int KTgaManager::TryDropBlock(int nIndex)
{
    int nResult = false;
    int nRetCode = false;
    int i = 0;
    int nCount = (int)m_aTgaBlock.size();
    RECT rcSelf;
    RECT rcSeted;
    KG_PROCESS_ERROR(nIndex >= 0);
    KG_PROCESS_ERROR(nIndex < nCount);

    rcSelf.left = g_GetCorrectPosition(m_aTgaBlock[nIndex]->GetLeft());
    rcSelf.right = rcSelf.left + m_aTgaBlock[nIndex]->GetWidth();
    rcSelf.top = g_GetCorrectPosition(m_aTgaBlock[nIndex]->GetTop());
    rcSelf.bottom = rcSelf.top + m_aTgaBlock[nIndex]->GetHeight();

    for (i = 0; i < nCount; ++i) 
    {
        if (i == nIndex)
            continue;
        if (!m_aTgaBlock[i]->IsPosHasBeenSeted())
            continue;

        rcSeted.left = m_aTgaBlock[i]->GetLeft();
        rcSeted.right = rcSeted.left + m_aTgaBlock[i]->GetWidth();
        rcSeted.top = m_aTgaBlock[i]->GetTop();
        rcSeted.bottom = rcSeted.top + m_aTgaBlock[i]->GetHeight();

        nRetCode = g_IntersectRect(rcSelf, rcSeted);
        KG_PROCESS_ERROR(!nRetCode);
    }

    m_aTgaBlock[nIndex]->SetPosHasBeenSeted();

    nResult = true;
Exit0:
    return nResult;
}

int KTgaManager::CalculateBlockPos(int nIndex, POINT &ptWanted)
{
    int nResult = false;
    int nRetCode = false;
    RECT rcSlef;
    RECT rcPosed;
    int i = 0;
    int nCount = (int)m_aTgaBlock.size();
    int nLastMinDistaceX = MAX_ADSORB_DISTANCE;
    int nLastMinDistaceY = MAX_ADSORB_DISTANCE;
    int nDistance = 0;

    KG_PROCESS_ERROR(nIndex >= 0);
    KG_PROCESS_ERROR(nIndex < nCount);

    rcSlef.left = ptWanted.x;
    rcSlef.top = ptWanted.y;
    rcSlef.right = rcSlef.left + m_aTgaBlock[nIndex]->GetWidth();
    rcSlef.bottom = rcSlef.top + m_aTgaBlock[nIndex]->GetHeight();

    for (i = 0; i < nCount; ++i) 
    {
        if (i == nIndex)
            continue;
        if (!m_aTgaBlock[i]->IsPosHasBeenSeted())
            continue;

        rcPosed.left = m_aTgaBlock[i]->GetLeft();
        rcPosed.right = rcPosed.left + m_aTgaBlock[i]->GetWidth();
        rcPosed.top = m_aTgaBlock[i]->GetTop();
        rcPosed.bottom = rcPosed.top + m_aTgaBlock[i]->GetHeight();

        if (!g_IsTwoRectNeighbors(rcSlef, rcPosed))
            continue;

        //pos x

        nDistance = abs(rcPosed.left - rcSlef.right); 
        if (nDistance <= nLastMinDistaceX)
        {
            ptWanted.x = rcPosed.left - FRAME_DISTANCE * 2 - (rcSlef.right - rcSlef.left);
            nLastMinDistaceX = nDistance;
        }

        //add
        nDistance = abs(rcPosed.left - rcSlef.left); 
        if (nDistance <= nLastMinDistaceX)
        {
            ptWanted.x = rcPosed.left;
            nLastMinDistaceX = nDistance;
        }

        nDistance = abs(rcPosed.right - rcSlef.left); 
        if (nDistance <= nLastMinDistaceX)
        {
            ptWanted.x = rcPosed.right + FRAME_DISTANCE * 2;
            nLastMinDistaceX = nDistance;
        }

        //add
        nDistance = abs(rcPosed.right - rcSlef.right); 
        if (nDistance <= nLastMinDistaceX)
        {
            ptWanted.x = rcPosed.right - (rcSlef.right - rcSlef.left);
            nLastMinDistaceX = nDistance;
        }

        //pos y

        nDistance = abs(rcPosed.top - rcSlef.bottom);
        if (nDistance <= nLastMinDistaceY)
        {
            ptWanted.y = rcPosed.top - FRAME_DISTANCE * 2 - (rcSlef.bottom - rcSlef.top);
            nLastMinDistaceY = nDistance;
        }

        //add
        nDistance = abs(rcPosed.top - rcSlef.top);
        if (nDistance <= nLastMinDistaceY)
        {
            ptWanted.y = rcPosed.top;
            nLastMinDistaceY = nDistance;
        }

        nDistance = abs(rcPosed.bottom - rcSlef.top);
        if (nDistance <= nLastMinDistaceY)
        {
            ptWanted.y = rcPosed.bottom + FRAME_DISTANCE * 2;
            nLastMinDistaceY = nDistance;
        }

        //add
        nDistance = abs(rcPosed.bottom - rcSlef.bottom);
        if (nDistance <= nLastMinDistaceY)
        {
            ptWanted.y = rcPosed.bottom - (rcSlef.bottom - rcSlef.top);
            nLastMinDistaceY = nDistance;
        }
    }

    nDistance = abs(KShow::GetSelf().GetDesiredWidth() - rcSlef.right);
    if (nDistance < nLastMinDistaceX)
    {
        ptWanted.x = KShow::GetSelf().GetDesiredWidth() - (rcSlef.right - rcSlef.left);
        nLastMinDistaceX = nDistance;
    }

    nDistance = abs(KShow::GetSelf().GetDesiredHeight() - rcSlef.bottom);
    if (nDistance < nLastMinDistaceY)
    {
        ptWanted.y = KShow::GetSelf().GetDesiredHeight() - (rcSlef.bottom - rcSlef.top);
        nLastMinDistaceY = nDistance;
    }

	if (ptWanted.x + (rcSlef.right - rcSlef.left) > SHOW_BUFFER_WIDTH - nLastMinDistaceX)
		ptWanted.x = SHOW_BUFFER_WIDTH - (rcSlef.right - rcSlef.left);

    if (ptWanted.x < nLastMinDistaceX)
        ptWanted.x = 0;

	if (ptWanted.y + (rcSlef.bottom - rcSlef.top) > SHOW_BUFFER_HEIGHT - nLastMinDistaceY)
		ptWanted.y = SHOW_BUFFER_HEIGHT - (rcSlef.bottom - rcSlef.top);

    if (ptWanted.y < nLastMinDistaceY)
        ptWanted.y = 0;

    m_aTgaBlock[nIndex]->SetLeft(ptWanted.x);
    m_aTgaBlock[nIndex]->SetTop(ptWanted.y);

    nResult = true;
Exit0:
    return nResult;
}

int KTgaManager::IsTgaPosSeted(LPCTSTR pcszName)
{
    int nResult = false;
    int nRetCode = false;
    TCHAR szName[MAX_PATH];
    KG_PROCESS_ERROR(pcszName);

    for (KTgaBlockArray::iterator it = m_aTgaBlock.begin(); it != m_aTgaBlock.end(); ++it) 
    {
        ASSERT(*it);
        if (!((*it)->IsPosHasBeenSeted()))
            continue;

        (*it)->GetName(szName, sizeof(szName) / sizeof(TCHAR));
        nRetCode = _tcsicmp(pcszName, szName); 
        KG_PROCESS_SUCCESS(nRetCode == 0);
    }

    KG_PROCESS_ERROR(true);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KTgaManager::IsFrameShoudBeSaved(LPCTSTR pcszName)
{
    int nResult = false;
    int nRetCode = false;
    TCHAR szName[MAX_PATH];
    KTgaBlock *pBlock = NULL;
    KG_PROCESS_ERROR(pcszName);

    for (KTgaBlockArray::iterator it = m_aTgaBlock.begin(); it != m_aTgaBlock.end(); ++it) 
    {
        pBlock = (*it);
        ASSERT(pBlock);

        pBlock->GetName(szName, sizeof(szName) / sizeof(TCHAR));
        nRetCode = _tcsicmp(pcszName, szName); 
        if (!nRetCode)
            continue;

        //KG_PROCESS_SUCCESS(!(pBlock->IsIndexChangeable()));
        KG_PROCESS_SUCCESS(pBlock->IsPosHasBeenSeted());
    }

    KG_PROCESS_ERROR(true);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KTgaManager::GetSaveBlockCount()
{
    return (int)m_aSaveData.size();
}

int KTgaManager::GetSaveArea(RECT &rcSave)
{
    KTgaBlock *pBlock = NULL;
    rcSave.left = 1024;
    rcSave.top = 1024;
    rcSave.right = 0;
    rcSave.bottom = 0;

    for (KTgaBlockArray::iterator it = m_aTgaBlock.begin(); it != m_aTgaBlock.end(); ++it) 
    {
        pBlock = (*it);
        ASSERT(pBlock);
        if (!(pBlock->IsPosHasBeenSeted()))
            continue;

        int nLeft = pBlock->GetLeft();
        if (nLeft <  rcSave.left)
            rcSave.left = nLeft;
        int nRight = nLeft + pBlock->GetWidth();
        if (nRight > rcSave.right)
            rcSave.right = nRight;
        int nTop = pBlock->GetTop();
        if (nTop < rcSave.top)
            rcSave.top = nTop;
        int nBottom = nTop + pBlock->GetHeight();
        if (nBottom > rcSave.bottom)
            rcSave.bottom = nBottom;
    }

    if (rcSave.bottom < rcSave.top)
    {
        rcSave.top = 0;
        rcSave.bottom = 0;
    }

    if (rcSave.right < rcSave.left)
    {
        rcSave.left = 0;
        rcSave.right = 0;
    }

    rcSave.right -= rcSave.left;
    rcSave.bottom -= rcSave.top;

    return true;
}

int KTgaManager::ClearSaveData()
{
    m_SaveTga.Release();
    m_aSaveData.clear();
    m_nSaveWidth = 0;
    m_nSaveHeight = 0;
    return true;
}

int KTgaManager::FormateSaveData()
{
    int nResult = false;
    int nRetCode = false;
    KTgaBlock *pBlock = NULL;
    KTGABLOCKSAVEDATA SaveData;
    int nCurrentSaveIndex = 0;
    int nSaveTgaWidth = 0;
    int nSaveTgaHeight = 0;
    RECT rcSaveBuffer;

    ClearSaveData();

    Sort(SORT_TO_SAVE);

    GetSaveArea(rcSaveBuffer);

    nSaveTgaWidth = rcSaveBuffer.right;
    nSaveTgaHeight = rcSaveBuffer.bottom;
    KSaveManager::GetSelf().GetCorrectWidthAndHeight(nSaveTgaWidth, nSaveTgaHeight);
    m_SaveTga.Create(nSaveTgaWidth, nSaveTgaHeight);
    
    for (KTgaBlockArray::iterator it = m_aTgaBlock.begin(); it != m_aTgaBlock.end(); ++it)
    {
        pBlock = (*it);
        ASSERT(pBlock);
        if (!(pBlock->IsIndexChangeable()))
        {
            int nIndex = pBlock->GetOriginalIndex();
            ASSERT(nIndex >= nCurrentSaveIndex);
            for (; nCurrentSaveIndex < nIndex;  ++nCurrentSaveIndex) 
            {
                 SaveData.szName[0] = _T('\0');
                 SaveData.nLeft = 0;
                 SaveData.nTop = 0;
                 SaveData.nWidth = 0;
                 SaveData.nHeight = 0;
                 m_aSaveData.push_back(SaveData);
            }

            if (pBlock->IsPosHasBeenSeted())
            {
                pBlock->GetName(SaveData.szName, sizeof(SaveData.szName) / sizeof(TCHAR));
                SaveData.nLeft = pBlock->GetLeft() - rcSaveBuffer.left;
                ASSERT(SaveData.nLeft >= 0);
                SaveData.nTop = pBlock->GetTop() - rcSaveBuffer.top;
                ASSERT(SaveData.nTop >= 0);
                SaveData.nWidth = pBlock->GetWidth();
                SaveData.nHeight = pBlock->GetHeight();
                m_aSaveData.push_back(SaveData);
                nRetCode = AppendSaveTgaData(pBlock, SaveData.nLeft, SaveData.nTop);
                KG_PROCESS_ERROR(nRetCode);
            }
            else
            {
                 SaveData.szName[0] = _T('\0');
                 SaveData.nLeft = 0;
                 SaveData.nTop = 0;
                 SaveData.nWidth = 0;
                 SaveData.nHeight = 0;
                 m_aSaveData.push_back(SaveData);
            }
            ++nCurrentSaveIndex;
            continue;
        }

        if (pBlock->IsPosHasBeenSeted())
        {
            pBlock->GetName(SaveData.szName, sizeof(SaveData.szName) / sizeof(TCHAR));
            SaveData.nLeft = pBlock->GetLeft() - rcSaveBuffer.left;
            ASSERT(SaveData.nLeft >= 0);
            SaveData.nTop = pBlock->GetTop() - rcSaveBuffer.top;
            ASSERT(SaveData.nTop >= 0);
            SaveData.nWidth = pBlock->GetWidth();
            SaveData.nHeight = pBlock->GetHeight();
            m_aSaveData.push_back(SaveData);
            nRetCode = AppendSaveTgaData(pBlock, SaveData.nLeft, SaveData.nTop);
            KG_PROCESS_ERROR(nRetCode);
            ++nCurrentSaveIndex;
        }
    }

    m_nSaveWidth = rcSaveBuffer.right;
    m_nSaveHeight = rcSaveBuffer.bottom;

    nResult = true;
Exit0:
    return true;
}

int KTgaManager::GetSaveIndex(LPCTSTR pcszName)
{
    int nIndex = -1;
    int nCount = (int)m_aSaveData.size();

    KG_PROCESS_ERROR(pcszName);
    KG_PROCESS_ERROR(pcszName[0] != _T('\0'));

    for (int i = 0; i < nCount; ++i) 
    {
        if (_tcsicmp(m_aSaveData[i].szName, pcszName) == 0)
        {
            nIndex = i;
            break;
        }
    }

Exit0:
    return nIndex;
}

int KTgaManager::GetSaveBufferWidth()
{
    return m_nSaveWidth;
}

int KTgaManager::GetSaveBufferHeight()
{
    return m_nSaveHeight;
}

int KTgaManager::Save(FILE *fpFile)
{
    int nResult = false;
    size_t uWrite = 0; 
    int nCount = (int)m_aSaveData.size();
    KG_PROCESS_ERROR(fpFile);

    for (int i = 0; i < nCount; ++i) 
    {
        int nInfo = 0;
        uWrite = fwrite(&nInfo, sizeof(int), 1, fpFile);
        KG_PROCESS_ERROR(uWrite == 1);
        uWrite = fwrite(&(m_aSaveData[i].nLeft), sizeof(int), 1, fpFile);
        KG_PROCESS_ERROR(uWrite == 1);
        uWrite = fwrite(&(m_aSaveData[i].nTop), sizeof(int), 1, fpFile);
        KG_PROCESS_ERROR(uWrite == 1);
        uWrite = fwrite(&(m_aSaveData[i].nWidth), sizeof(int), 1, fpFile);
        KG_PROCESS_ERROR(uWrite == 1);
        uWrite = fwrite(&(m_aSaveData[i].nHeight), sizeof(int), 1, fpFile);
        KG_PROCESS_ERROR(uWrite == 1);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KTgaManager::ExportText(FILE *fpFile)
{
    int nResult = false;
    int nNotSavedIndex = 0;
    size_t uWrite = 0; 
    int nTotalCount = (int)m_aTgaBlock.size();
    int nCount = (int)m_aSaveData.size();
    KG_PROCESS_ERROR(fpFile);

    fprintf(fpFile, "%s \t: %d\n", "Total frame count", nTotalCount);
    fprintf(fpFile, "%s \t: %d\n", "Save  frame count", nCount);

    fprintf(fpFile, "%s:\n", "Save frames are");
    for (int i = 0; i < nCount; ++i) 
    {
        fprintf(fpFile, "%s%d \t: %d\t%d\t%d\t%d \t%s:\t%s\n", "Save frame ", i, 
            m_aSaveData[i].nLeft, m_aSaveData[i].nTop, 
            m_aSaveData[i].nWidth, m_aSaveData[i].nHeight,
            "Name", m_aSaveData[i].szName
        );
    }

    fprintf(fpFile, "%s:\n", "Not save frames are");
    for (int i = 0; i < nTotalCount; ++i) 
    {
        if (m_aTgaBlock[i]->IsPosHasBeenSeted())
            continue;
        fprintf(fpFile, "%s%d \t: %s:\t%s\n", 
            "Not Save frame ", nNotSavedIndex, 
            "Name", m_aTgaBlock[i]->GetName()
        );
        ++nNotSavedIndex;
    }

    nResult = true;
Exit0:
    return nResult;
}

int KTgaManager::ExportNewText(FILE *fpFile)
{
	int nResult = false;
	int nNotSavedIndex = 0;
	int nTotalCount = (int)m_aTgaBlock.size();
	int nCount = (int)m_aSaveData.size();
	size_t uWrite = 0; 

	KG_PROCESS_ERROR(fpFile);

	fprintf(fpFile,"%s\t%s\t%s\t%s\t%s\t%s\n","Farme","Left","Top","Width","High","File");
	for (int i = 0; i < nCount; ++i) 
	{
		fprintf(fpFile, "%d\t%d\t%d\t%d\t%d\t%s\n",i, 
			m_aSaveData[i].nLeft, m_aSaveData[i].nTop, 
			m_aSaveData[i].nWidth, m_aSaveData[i].nHeight,
			m_aSaveData[i].szName
			);
	}

	nResult = true;
Exit0:
	return nResult;
}

int KTgaManager::AppendSaveTgaData(KTgaBlock *pBlock, int nLeft, int nTop)
{
    int nResult = false;
    BYTE *pbyDestStart = NULL;
    BYTE *pbyDest = NULL;
    BYTE *pbySorce = NULL;
    int nDestWidth = 0;
    int nDestHeight = 0;
    int nSorceWidth = 0;
    int nSorceHeight = 0;

    KG_PROCESS_ERROR(pBlock);
    nSorceWidth = pBlock->GetWidth();
    nSorceHeight = pBlock->GetHeight();
    pbySorce = pBlock->GetBufferPtr();
    KG_PROCESS_ERROR(pbySorce);

    pbyDestStart = (BYTE *)m_SaveTga.GetDataBufferPointer();
    KG_PROCESS_ERROR(pbyDestStart);
    nDestWidth = m_SaveTga.GetWidth();
    nDestHeight = m_SaveTga.GetHeight();

    ASSERT(nLeft + nSorceWidth <= nDestWidth);
    ASSERT(nTop + nSorceHeight <= nDestHeight);

	if (nTop > 0)
	{
		pbyDest = pbyDestStart + ((nTop - 1) * nDestWidth + nLeft) * 4;
		memcpy(pbyDest, pbySorce, nSorceWidth * 4);
		if (nLeft > 0)
		{
			pbyDest -= 4;
			memcpy(pbyDest, pbySorce, 4);
			pbyDest += 4;
		}
		if (nLeft + nSorceWidth < nDestWidth)
		{
			pbyDest += 4 * nSorceWidth;
			memcpy(pbyDest, pbySorce + 4 * (nSorceWidth - 1), 4);
		}
	}
    for (int i = 0; i < nSorceHeight; ++i) 
    {
        pbyDest = pbyDestStart + ((i + nTop) * nDestWidth + nLeft) * 4;
        memcpy(pbyDest, pbySorce, nSorceWidth * 4);
		if (nLeft > 0)
		{
			pbyDest -= 4;
			memcpy(pbyDest, pbySorce, 4);
			pbyDest += 4;
		}
        pbySorce += (nSorceWidth * 4);
		if (nLeft + nSorceWidth < nDestWidth)
		{
			pbyDest += 4 * nSorceWidth;
			memcpy(pbyDest, pbySorce - 4, 4);
		}
    }
	if (nTop + nSorceHeight < nDestHeight)
	{
		pbySorce -= (nSorceWidth * 4);
		pbyDest = pbyDestStart + ((nSorceHeight + nTop) * nDestWidth + nLeft) * 4;
		memcpy(pbyDest, pbySorce, nSorceWidth * 4);
		if (nLeft > 0)
		{
			pbyDest -= 4;
			memcpy(pbyDest, pbySorce, 4);
			pbyDest += 4;
		}
		if (nLeft + nSorceWidth < nDestWidth)
		{
			pbyDest += 4 * nSorceWidth;
			memcpy(pbyDest, pbySorce + 4 * (nSorceWidth - 1), 4);
		}
	}

    nResult = true;
Exit0:
    return nResult;
}

KTga32 &KTgaManager::GetSaveTga()
{
    return m_SaveTga;
}

int KTgaManager::AutoMakeup()//图片自动排序函数
{
    int nResult = false;
    int nRetCode = false;

    nRetCode = AutoMakeupWidthDanger();
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = AutoMakeupHeightDanger();
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = AutoMakeupNothingDanger();
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    return nResult;

}

int KTgaManager::AutoMakeupWidthDanger()
{
    int nResult = false;
    KTgaBlock *pBlockSelf = NULL;
    KTgaBlock *pBlockPosed = NULL;
    RECT rcSelf;
    RECT rcPosed;
    int nSelfWidth = 0;
    int nSelfHeight = 0;
    int nCount = (int)m_aTgaBlock.size();
    int nDesiredWidth = KShow::GetSelf().GetDesiredWidth();
    int nDesiredHeight = KShow::GetSelf().GetDesiredHeight();

    Sort(SORT_TO_AUTO_MAKE_WIDTH);
    for (int nIndex = 0; nIndex < nCount; ++nIndex)
    {
        pBlockSelf = m_aTgaBlock[nIndex];
        ASSERT(pBlockSelf);

        if (pBlockSelf->IsPosHasBeenSeted())
            continue;

        nSelfWidth = pBlockSelf->GetWidth();
        if (nSelfWidth < (nDesiredWidth / 2))
            break;
        KG_PROCESS_ERROR(nSelfWidth <= nDesiredWidth);

        nSelfHeight = pBlockSelf->GetHeight();
        KG_PROCESS_ERROR(nSelfHeight <= nDesiredHeight);
        
        pBlockSelf->SetLeft(0);
        for (int i = 0; i <= nDesiredHeight - nSelfHeight; ++i)
        {
			i = g_GetCorrectPosition(i);
            pBlockSelf->SetTop(i);
            for (int nIndexSeted = 0; nIndexSeted < nCount; ++nIndexSeted)
            {
                pBlockPosed = m_aTgaBlock[nIndexSeted];
                ASSERT(pBlockPosed);
                if (!pBlockPosed->IsPosHasBeenSeted())
                {
                    pBlockSelf->SetPosHasBeenSeted();
                    break;
                }

                pBlockSelf->GetByRect(rcSelf);
                pBlockPosed->GetByRect(rcPosed);                
                if (!g_IntersectRect(rcSelf, rcPosed))
                    continue;

                if (rcPosed.right + FRAME_DISTANCE * 2 + nSelfWidth > nDesiredWidth)
                {
                    pBlockSelf->SetLeft(0);
                    break;
                }

                pBlockSelf->SetLeft(g_GetCorrectPosition(rcPosed.right + FRAME_DISTANCE * 2));
                nIndexSeted = -1;
            }

            if (pBlockSelf->IsPosHasBeenSeted())
                break;
        }
        KG_PROCESS_ERROR(pBlockSelf->IsPosHasBeenSeted());
    }

    nResult = true;
Exit0:
    return nResult;
}

int KTgaManager::AutoMakeupHeightDanger()
{
    int nResult = false;
    KTgaBlock *pBlockSelf = NULL;
    KTgaBlock *pBlockPosed = NULL;
    RECT rcSelf;
    RECT rcPosed;
    int nSelfWidth = 0;
    int nSelfHeight = 0;
    int nCount = (int)m_aTgaBlock.size();
    int nDesiredWidth = KShow::GetSelf().GetDesiredWidth();
    int nDesiredHeight = KShow::GetSelf().GetDesiredHeight();

    Sort(SORT_TO_AUTO_MAKE_HEIGHT);
    for (int nIndex = 0; nIndex < nCount; ++nIndex)
    {
        pBlockSelf = m_aTgaBlock[nIndex];
        ASSERT(pBlockSelf);

        if (pBlockSelf->IsPosHasBeenSeted())
            continue;

        nSelfWidth = pBlockSelf->GetWidth();
        KG_PROCESS_ERROR(nSelfWidth <= nDesiredWidth);

        nSelfHeight = pBlockSelf->GetHeight();
        if (nSelfHeight < (nDesiredHeight / 2))
            break;
        KG_PROCESS_ERROR(nSelfHeight <= nDesiredHeight);
        
        pBlockSelf->SetTop(0);
        for (int i = nDesiredWidth - nSelfWidth; i >= 0; --i)
        {
			i = g_GetCorrectPosition(i);
            pBlockSelf->SetLeft(i);
            for (int nIndexSeted = 0; nIndexSeted < nCount; ++nIndexSeted)
            {
                pBlockPosed = m_aTgaBlock[nIndexSeted];
                ASSERT(pBlockPosed);
                if (!pBlockPosed->IsPosHasBeenSeted())
                {
                    pBlockSelf->SetPosHasBeenSeted();
                    break;
                }
                pBlockSelf->GetByRect(rcSelf);
                pBlockPosed->GetByRect(rcPosed);                
                if (!g_IntersectRect(rcSelf, rcPosed))
                    continue;

                if (rcPosed.bottom + FRAME_DISTANCE * 2 + nSelfHeight > nDesiredHeight)
                {
                    pBlockSelf->SetTop(0);
					KG_PROCESS_ERROR(pBlockSelf->IsPosHasBeenSeted());
                    break;
                }

                pBlockSelf->SetTop(g_GetCorrectPosition(rcPosed.bottom + FRAME_DISTANCE * 2));
                nIndexSeted = -1;
            }

            if (pBlockSelf->IsPosHasBeenSeted())
                break;
        }
        KG_PROCESS_ERROR(pBlockSelf->IsPosHasBeenSeted());
    }

    nResult = true;
Exit0:
    return nResult;
}

int KTgaManager::AutoMakeupNothingDanger()
{
    int nResult = false;
    KTgaBlock *pBlockSelf = NULL;
    KTgaBlock *pBlockPosed = NULL;
    RECT rcSelf;
    RECT rcPosed;
    int nSelfWidth = 0;
    int nSelfHeight = 0;
    int nCount = (int)m_aTgaBlock.size();
    int nDesiredWidth = KShow::GetSelf().GetDesiredWidth();
    int nDesiredHeight = KShow::GetSelf().GetDesiredHeight();

    Sort(SORT_TO_AUTO_MAKE_SIZE);
    for (int nIndex = 0; nIndex < nCount; ++nIndex)
    {
        pBlockSelf = m_aTgaBlock[nIndex];
        ASSERT(pBlockSelf);

        nSelfWidth = pBlockSelf->GetWidth();
        nSelfHeight = pBlockSelf->GetHeight();

        if (pBlockSelf->IsPosHasBeenSeted())
            continue;

        pBlockSelf->SetLeft(0);
        for (int i = 0; i <= nDesiredHeight - nSelfHeight; ++i)
        {
			i = g_GetCorrectPosition(i);
            pBlockSelf->SetTop(i);
            //pBlockSelf->GetByRect(rcSelf);
            for (int nIndexSeted = 0; nIndexSeted < nCount; ++nIndexSeted)
            {
                pBlockPosed = m_aTgaBlock[nIndexSeted];
                ASSERT(pBlockPosed);
                if (!pBlockPosed->IsPosHasBeenSeted())
                {
                    pBlockSelf->SetPosHasBeenSeted();
                    break;
                }

                pBlockSelf->GetByRect(rcSelf);
                pBlockPosed->GetByRect(rcPosed);                
                if (!g_IntersectRect(rcSelf, rcPosed))
                    continue;

                if (rcPosed.right + FRAME_DISTANCE * 2 + nSelfWidth > nDesiredWidth)
                {
                    pBlockSelf->SetLeft(0);
                    break;
                }

                pBlockSelf->SetLeft(g_GetCorrectPosition(rcPosed.right + FRAME_DISTANCE * 2));
                nIndexSeted = -1;
            }

            if (pBlockSelf->IsPosHasBeenSeted())
                break;
        }
        KG_PROCESS_ERROR(pBlockSelf->IsPosHasBeenSeted());
    }

    nResult = true;
Exit0:
    return nResult;
}

int KTgaManager::ExportAllTga(LPCTSTR pcszPath)
{
    int nResult = false;
    int nRetCode = false;
    TCHAR szFileName[MAX_PATH];
    TCHAR szBlockName[MAX_PATH];
    LPTSTR pszClip = NULL;
    LPTSTR pszTemp = NULL;
    KTga32 ExportTga;
    KG_PROCESS_ERROR(pcszPath);
    KG_PROCESS_ERROR(pcszPath[0] != _T('\0'));

    for (KTgaBlockArray::iterator it = m_aTgaBlock.begin(); it != m_aTgaBlock.end(); ++it)
    {
        ASSERT(*it);
        (*it)->GetName(szBlockName, sizeof(szBlockName) / sizeof(TCHAR));
        pszClip = _tcsrchr(szBlockName, _T('\\'));
        if (pszClip)
            ++pszClip;
        else
            pszClip = szBlockName;
        pszTemp = _tcsrchr(szBlockName, _T('/'));
        if (pszTemp)
            pszClip = pszTemp + 1;

        pszTemp = _tcschr(pszClip, _T(':'));
        if (pszTemp)
        {
            *pszTemp = _T('@');
            pszTemp = _tcschr(pszClip, _T('.'));
            while (pszTemp)
            {
                *pszTemp = _T('@');
                pszTemp = _tcschr(pszTemp, _T('.'));
            }   
            _sntprintf(szFileName, sizeof(szFileName) / sizeof(TCHAR), _T("%s%s%s%s"), pcszPath, _T("\\"), pszClip, _T(".tga"));
        }
        else
            _sntprintf(szFileName, sizeof(szFileName) / sizeof(TCHAR), _T("%s%s%s"), pcszPath, _T("\\"), pszClip);
        szFileName[sizeof(szFileName) / sizeof(TCHAR) - 1]= _T('\0');

        ExportTga.Release();
        nRetCode = ExportTga.Create((*it)->GetWidth(), (*it)->GetHeight());
        KG_PROCESS_ERROR(nRetCode);
        memcpy(ExportTga.GetDataBufferPointer(), (*it)->GetBufferPtr(), (*it)->GetBufferSize());
        nRetCode = ExportTga.SaveDataBufferToTgaFile(szFileName);
        KG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    ExportTga.Release();
    return nResult;
}

int KTgaManager::ExportTga(LPCTSTR pcszPath, int nIndex)
{
    int nResult = false;
    int nRetCode = false;
    KTgaBlock *pBlock = NULL;
    TCHAR szFileName[MAX_PATH];
    TCHAR szBlockName[MAX_PATH];
    LPTSTR pszClip = NULL;
    LPTSTR pszTemp = NULL;
    KTga32 ExportTga;
    KG_PROCESS_ERROR(pcszPath);
    KG_PROCESS_ERROR(pcszPath[0] != _T('\0'));
    KG_PROCESS_ERROR(nIndex >= 0 && nIndex < (int)m_aTgaBlock.size());

    pBlock = m_aTgaBlock[nIndex];
    ASSERT(pBlock);
    pBlock->GetName(szBlockName, sizeof(szBlockName) / sizeof(TCHAR));
    pszClip = _tcsrchr(szBlockName, _T('\\'));
    if (pszClip)
        ++pszClip;
    else
        pszClip = szBlockName;
    pszTemp = _tcsrchr(szBlockName, _T('/'));
    if (pszTemp)
        pszClip = pszTemp + 1;

    pszTemp = _tcschr(pszClip, _T(':'));
    if (pszTemp)
    {
        *pszTemp = _T('@');
        pszTemp = _tcschr(pszClip, _T('.'));
        while (pszTemp)
        {
            *pszTemp = _T('@');
            pszTemp = _tcschr(pszTemp, _T('.'));
        }   
        _sntprintf(szFileName, sizeof(szFileName) / sizeof(TCHAR), _T("%s%s%s%s"), pcszPath, _T("\\"), pszClip, _T(".tga"));
    }
    else
        _sntprintf(szFileName, sizeof(szFileName) / sizeof(TCHAR), _T("%s%s%s"), pcszPath, _T("\\"), pszClip);
    szFileName[sizeof(szFileName) / sizeof(TCHAR) - 1]= _T('\0');

    nRetCode = ExportTga.Create(pBlock->GetWidth(), pBlock->GetHeight());
    KG_PROCESS_ERROR(nRetCode);
    memcpy(ExportTga.GetDataBufferPointer(), pBlock->GetBufferPtr(), pBlock->GetBufferSize());
    nRetCode = ExportTga.SaveDataBufferToTgaFile(szFileName);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    ExportTga.Release();
    return nResult;
}

int KTgaManager::ExportCur(LPCTSTR pcszPath, int nIndex)
{
	int nResult = false;
	int nRetCode = false;
	KTgaBlock *pBlock = NULL;
	TCHAR szFileName[MAX_PATH];
	TCHAR szBlockName[MAX_PATH];
	LPTSTR pszClip = NULL;
	LPTSTR pszTemp = NULL;
	KTga32 ExportTga;
	KG_PROCESS_ERROR(pcszPath);
	KG_PROCESS_ERROR(pcszPath[0] != _T('\0'));
	KG_PROCESS_ERROR(nIndex >= 0 && nIndex < (int)m_aTgaBlock.size());

	pBlock = m_aTgaBlock[nIndex];
	ASSERT(pBlock);
	pBlock->GetName(szBlockName, sizeof(szBlockName) / sizeof(TCHAR));
	pszClip = _tcsrchr(szBlockName, _T('\\'));
	if (pszClip)
		++pszClip;
	else
		pszClip = szBlockName;
	pszTemp = _tcsrchr(szBlockName, _T('/'));
	if (pszTemp)
		pszClip = pszTemp + 1;

	pszTemp = _tcschr(pszClip, _T(':'));
	if (pszTemp)
	{
		*pszTemp = _T('@');
		pszTemp = _tcschr(pszClip, _T('.'));
		while (pszTemp)
		{
			*pszTemp = _T('@');
			pszTemp = _tcschr(pszTemp, _T('.'));
		}   
		_sntprintf(szFileName, sizeof(szFileName) / sizeof(TCHAR), _T("%s%s%s%s"), pcszPath, _T("\\"), pszClip, _T(".Cur"));
	}
	else
	{
		_sntprintf(szFileName, sizeof(szFileName) / sizeof(TCHAR), _T("%s%s%s"), pcszPath, _T("\\"), pszClip);
		pszClip = _tcsrchr(szFileName, _T('.'));
		if (pszClip)
			_tcscpy(pszClip, _T(".Cur"));
		else
			_tcscat(szFileName, _T(".Cur"));
	}
	szFileName[sizeof(szFileName) / sizeof(TCHAR) - 1]= _T('\0');

	nRetCode = ExportTga.Create(pBlock->GetWidth(), pBlock->GetHeight());
	KG_PROCESS_ERROR(nRetCode);
	memcpy(ExportTga.GetDataBufferPointer(), pBlock->GetBufferPtr(), pBlock->GetBufferSize());
	nRetCode = ExportTga.SaveDataBufferToCurFile(szFileName);
	KG_PROCESS_ERROR(nRetCode);

	nResult = true;
Exit0:
	ExportTga.Release();
	return nResult;
}


int KTgaManager::RemoveThePosedBlock()
{
    int nCount = (int)m_aTgaBlock.size();

    for (int i = 0; i < nCount; ++i)
    {
        ASSERT(m_aTgaBlock[i]);
        if (!(m_aTgaBlock[i]->IsPosHasBeenSeted()))
            continue;
        m_aTgaBlock.erase(m_aTgaBlock.begin() + i);

        --i;
        nCount = (int)m_aTgaBlock.size();
    }

    UpDataListView();
    UpDataShow();

    return true;
}

int KTgaManager::Remove(int nIndex)
{
    int nResult = false;

    KG_PROCESS_ERROR(nIndex >= 0 && nIndex < (int)m_aTgaBlock.size());

    m_aTgaBlock.erase(m_aTgaBlock.begin() + nIndex);

    UpDataListView();

    nResult = true;
Exit0:
    return nResult;
}

int KTgaManager::Replace(int nReplacedIndex, int nReplaceIndex)
{
    int nResult = false;
    KTgaBlock *pBlockReplaced = NULL;
    KTgaBlock *pBlockReplace = NULL;
    KAnimateBlock *pAnimate = NULL;
    TCHAR szNameReplace[MAX_PATH];
    TCHAR szNameReplaced[MAX_PATH];
    int nCount = (int)m_aTgaBlock.size();

    KG_PROCESS_ERROR(nReplaceIndex != nReplacedIndex);
    KG_PROCESS_ERROR(nReplacedIndex >= 0 && nReplacedIndex < nCount);
    KG_PROCESS_ERROR(nReplaceIndex >= 0 && nReplaceIndex <nCount);
    pBlockReplaced = m_aTgaBlock[nReplacedIndex];
    ASSERT(pBlockReplaced);
    pBlockReplace = m_aTgaBlock[nReplaceIndex];
    ASSERT(pBlockReplace);
    
    pBlockReplaced->SetWidth(pBlockReplace->GetWidth());
    pBlockReplaced->SetHeight(pBlockReplace->GetHeight());
    pBlockReplaced->SetBuffer(pBlockReplace->GetBufferPtr(), pBlockReplace->GetBufferSize());

    pBlockReplace->GetName(szNameReplace, sizeof(szNameReplace) / sizeof(TCHAR));
    pBlockReplaced->GetName(szNameReplaced, sizeof(szNameReplaced) / sizeof(TCHAR));

    pAnimate = KAnimateManager::GetSelf().GetParent(szNameReplace);
    if (pAnimate)
	{
        pAnimate->RemoveFrame(szNameReplace);
		if (!pAnimate->GetFrameCount())
			KAnimateManager::GetSelf().Remove(pAnimate);
	}
    
    pBlockReplaced->SetName(szNameReplace);
    pAnimate = KAnimateManager::GetSelf().GetParent(szNameReplaced);
    if (pAnimate)
        pAnimate->ReplaceName(szNameReplaced, szNameReplace);

    KPickBox::GetSelf().Pick(nReplacedIndex);

    Remove(nReplaceIndex);

    UpDataListView();
    UpDataShow();

    nResult = true;
Exit0:
    return nResult;
}