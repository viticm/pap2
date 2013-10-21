////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KAnimateManager.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-22 14:14:11
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <algorithm>
#include "KTgaManager.h"
#include "KAnimateManager.h"
#include "UiTex.h"
#include "KWndAnimateList.h"


////////////////////////////////////////////////////////////////////////////////

KAnimateManager *KAnimateManager::ms_pSelf = NULL;

KAnimateManager::KAnimateManager()
{
    m_nSaveAnimateCount = 0;
}

KAnimateManager::~KAnimateManager()
{
}

int KAnimateManager::Init()
{
    int nResult = false;

    ASSERT(!ms_pSelf);
    ms_pSelf = new KAnimateManager;
    KG_PROCESS_ERROR(ms_pSelf);

    nResult = true;
Exit0:
    if (!nResult)
        KG_DELETE(ms_pSelf);
    return nResult;
}

int KAnimateManager::Exit()
{
    if (ms_pSelf)
        ms_pSelf->Clear();
    KG_DELETE(ms_pSelf);
    return true;
}

KAnimateManager &KAnimateManager::GetSelf()
{
    ASSERT(ms_pSelf);
    return *ms_pSelf;
}

int KAnimateManager::Append(KAnimateBlock *pAnimate)
{
    int nResult = false;

    KG_PROCESS_ERROR(pAnimate);

    m_aAnimate.push_back(pAnimate);

    UpdataListView();

    nResult = true;
Exit0:
    return nResult;
}

int KAnimateManager::Remove(KAnimateBlock *pAnimate)
{
    int nResult = false;
    
    KAnimateArray::iterator it = 
        std::find(m_aAnimate.begin(), m_aAnimate.end(), pAnimate);
    KG_PROCESS_ERROR(it != m_aAnimate.end());
    (*it)->Release();
    m_aAnimate.erase(it);

    UpdataListView();

    nResult = true;
Exit0:
    return nResult;
}

int KAnimateManager::Remove(int nIndex)
{
    if (nIndex >= 0 && nIndex < (int)m_aAnimate.size())
    {
        (*(m_aAnimate.begin() + nIndex))->Release();
        m_aAnimate.erase(m_aAnimate.begin() + nIndex);
        UpdataListView();
        return true;
    }

    return false;
}

int KAnimateManager::Clear()
{
    for (KAnimateArray::iterator it = m_aAnimate.begin(); it != m_aAnimate.end(); ++it) 
    {
        ASSERT(*it);
        (*it)->Release();
        (*it) = NULL;
    }
    m_aAnimate.clear();
    ClearSaveData();

    UpdataListView();

    return true;
}

int KAnimateManager::IsFrameAlreadyHasBrother(LPCTSTR pcszFramName)
{
    int nResult = false;

    KG_PROCESS_ERROR(pcszFramName);
    KG_PROCESS_ERROR(pcszFramName[0] != _T('\0'));

    for (KAnimateArray::iterator it = m_aAnimate.begin(); it != m_aAnimate.end(); ++it) 
    {
        KG_PROCESS_SUCCESS((*it)->LookFromNameShouldFrameMyson(pcszFramName));
    }

    KG_PROCESS_ERROR(false);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KAnimateManager::IsFrameAleadyInAnyAnimateGroup(LPCTSTR pcszName)
{
    int nResult = false;

    KG_PROCESS_ERROR(pcszName);

    for (KAnimateArray::iterator it = m_aAnimate.begin(); it != m_aAnimate.end(); ++it) 
    {
        KG_PROCESS_SUCCESS((*it)->IsFrameMySon(pcszName));
    }

    KG_PROCESS_ERROR(false);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

KAnimateBlock * KAnimateManager::GetParent(LPCTSTR pcszFramName)
{
    KAnimateBlock *pParent = NULL;

    KG_PROCESS_ERROR(pcszFramName);
    KG_PROCESS_ERROR(pcszFramName[0] != _T('\0'));

    for (KAnimateArray::iterator it = m_aAnimate.begin(); it != m_aAnimate.end(); ++it) 
    {
        if ((*it)->IsFrameMySon(pcszFramName))
        {
            pParent = (*it);
            break;
        }
    }

Exit0:
    return pParent;
}

KAnimateBlock* KAnimateManager::GetParentJustFromName(LPCTSTR pcszFramName)
{
	KAnimateBlock *pParent = NULL;

	KG_PROCESS_ERROR(pcszFramName);
	KG_PROCESS_ERROR(pcszFramName[0] != _T('\0'));

	for (KAnimateArray::iterator it = m_aAnimate.begin(); it != m_aAnimate.end(); ++it) 
	{
		if ((*it)->LookFromNameShouldFrameMyson(pcszFramName))
		{
			pParent = (*it);
			break;
		}
	}

Exit0:
	return pParent;
}

int KAnimateManager::GetSaveAnimateCount()
{
    return m_nSaveAnimateCount;
}

int KAnimateManager::ClearSaveData()
{
    m_aSaveAnimateData.clear();
    m_nSaveAnimateCount = 0;
    return true;
}

int KAnimateManager::FormateSaveData()
{
    int nRetCode = false;
    KAnimateBlock *pAnimate = NULL;
    int nCurrentSaveIndex = 0;
    ClearSaveData();

    Sort(SORT_TO_SAVE);

    for (KAnimateArray::iterator it = m_aAnimate.begin(); it != m_aAnimate.end(); ++it) 
    {
        pAnimate = (*it);
        ASSERT(*it);
        if (!(pAnimate->IsIndexChangeable()))
        {
            int nIndex = pAnimate->GetOriginalIndex();
            ASSERT(nIndex >= nCurrentSaveIndex);
            for (; nCurrentSaveIndex < nIndex;  ++nCurrentSaveIndex) 
            {
                 AppendSaveData(0); //表示有动画组被删掉了
            }
            nRetCode = pAnimate->FormateSaveData();
            if (!nRetCode)
                AppendSaveData(0);  //这组动画为空
            ++nCurrentSaveIndex;
            continue;
        }

        nRetCode = pAnimate->FormateSaveData();
        if (nRetCode)
            ++nCurrentSaveIndex;
    }

    m_nSaveAnimateCount = nCurrentSaveIndex;
    return true;
}

int KAnimateManager::AppendSaveData(int nData)
{
    m_aSaveAnimateData.push_back(nData);
    return true;
}

bool KAnimateManager::SortToSaveFunction(KAnimateBlock *pAnimate1, KAnimateBlock *pAnimate2)
{
    ASSERT(pAnimate1);
    ASSERT(pAnimate2);
    
    if (pAnimate1->IsIndexChangeable())
        return false;

    if (pAnimate2->IsIndexChangeable())
        return true;

    if (pAnimate1->GetOriginalIndex() < pAnimate2->GetOriginalIndex())
        return true;
    
    return false;
}

int KAnimateManager::Sort(int nSortType)
{
    switch(nSortType) 
    {
    case SORT_TO_SAVE:
        std::sort(m_aAnimate.begin(), m_aAnimate.end(), &KAnimateManager::SortToSaveFunction);
        break;
    case SORT_BY_HEIGHT:
    	break;
    }

    UpdataListView();
    return true;
}

int KAnimateManager::GetSaveAnimateBufferSize()
{
    return (int)m_aSaveAnimateData.size();
}

int KAnimateManager::Save(FILE *fpFile)
{
    int nResult = false;
    size_t uWrite = 0;
    int nCount = (int)m_aSaveAnimateData.size();
    KG_PROCESS_ERROR(fpFile);
    KG_PROCESS_SUCCESS(nCount == 0);

    uWrite = fwrite(&(m_aSaveAnimateData[0]), sizeof(int), nCount, fpFile);
    KG_PROCESS_ERROR(uWrite == nCount);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KAnimateManager::ExportText(FILE *fpFile)
{
    int nResult = false;
    int nCount = (int)m_aAnimate.size();
    int nDataIndex = 0;
    KG_PROCESS_ERROR(fpFile);

    fprintf(fpFile, "%s \t: %d\n", "Total animate count", nCount);
    fprintf(fpFile, "%s \t: %d\n", "Save  animate count", m_nSaveAnimateCount);

    fprintf(fpFile, "%s \t:\n", "Save animate data are(Frame are Save frame index)");
    for (int i = 0; i < m_nSaveAnimateCount; ++i)
    {
        fprintf(fpFile, "%s %d \t: %s\t: %d\n%s \t:", "Animate", i, "Frame count", m_aSaveAnimateData[nDataIndex], "Frame are");
        for (int j = 0; j < m_aSaveAnimateData[nDataIndex]; ++j)
        {
            fprintf(fpFile, "%d \t", m_aSaveAnimateData[nDataIndex + j + 1]);
        }
        nDataIndex += (m_aSaveAnimateData[nDataIndex] + 1);
        fprintf(fpFile, "\n");
    }

    ASSERT(nDataIndex == (int)m_aSaveAnimateData.size());

    nResult = true;
Exit0:
    return nResult;
}

int KAnimateManager::UpdataListView()
{
    LPCTSTR pcszClip = NULL;
    LPCTSTR pcszTemp = NULL;
    TCHAR szName[MAX_PATH];
    KWndAnimateList::GetSelf().ClearAnimateGroup();

    for (KAnimateArray::iterator it = m_aAnimate.begin(); it != m_aAnimate.end(); ++it) 
    {
        ASSERT(*it);
        (*it)->GetName(szName, sizeof(szName) / sizeof(TCHAR));
        pcszClip = _tcsrchr(szName, _T('\\'));
        if (pcszClip)
            pcszClip++;
        else
            pcszClip = szName;
        pcszTemp = _tcsrchr(pcszClip, _T('/'));
        if (pcszTemp)
            pcszClip = pcszTemp + 1;
        KWndAnimateList::GetSelf().AppendAnimateGroup(pcszClip);
    }

    return true;
}

KAnimateBlock *KAnimateManager::Get(int nIndex)
{
    if (nIndex >= 0 && nIndex < (int)m_aAnimate.size())
        return (*(m_aAnimate.begin() + nIndex));
    return NULL;
}

KAnimateBlock *KAnimateManager::Get(LPCTSTR pcszAnimateName)
{
    if (!pcszAnimateName)
        return NULL;
    if (pcszAnimateName[0] == _T('\0'))
        return NULL;

    for (KAnimateArray::iterator it = m_aAnimate.begin(); it != m_aAnimate.end(); ++it)
    {
        if (_tcsicmp(pcszAnimateName, (*it)->GetName()) == 0)
            return (*it);
    }

    return NULL;
}

