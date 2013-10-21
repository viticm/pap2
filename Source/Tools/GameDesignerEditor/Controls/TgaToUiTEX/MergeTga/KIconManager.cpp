////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KIconManager.cpp
//  Version     : 1.0
//  Creator     : Lin Jiaqi
//  Create Date : 2005-12-22 14:14:11
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include <algorithm>
#include <vector>
#include "KTgaManager.h"
#include "KAnimateManager.h"
#include "UiTex.h"
#include "KWndAnimateList.h"
#include "KIconManager.h"
#include "KWndFrameList.h"
#include "KIconManager.h"
#include "KShow.h"

////////////////////////////////////////////////////////////////////////////////

KIconManager *KIconManager::ms_pSelf = NULL;

KIconManager::KIconManager()
{ 
    m_aIcon.clear();
    m_hWnd = NULL;
}

KIconManager::~KIconManager()
{
}

int KIconManager::Init()
{
	int nResult = false;

	ASSERT(!ms_pSelf);
	ms_pSelf = new KIconManager;
	KG_PROCESS_ERROR(ms_pSelf);

	nResult = true;
Exit0:
	if (!nResult)
		KG_DELETE(ms_pSelf);
	return nResult;
}

int KIconManager::Exit()
{
	if (ms_pSelf)
		ms_pSelf->Clear();
	KG_DELETE(ms_pSelf);
	return true;
}

int KIconManager::SetHwnd(HWND hWnd)
{
    m_hWnd = hWnd;
    return true;
}

HWND KIconManager::GetHwnd()
{
    return m_hWnd;
}

KIconManager &KIconManager::GetSelf()
{
	ASSERT(ms_pSelf);
	return *ms_pSelf;
}

int KIconManager::Append(KIconBlock *pIcon)
{
	int nResult = false;

	KG_PROCESS_ERROR(pIcon);

	m_aIcon.push_back(pIcon);

	UpDataListView();

	nResult = true;
Exit0:
	return nResult;
}

int KIconManager::Clear()
{
	for (KIconArray::iterator it = m_aIcon.begin(); it != m_aIcon.end(); ++it) 
	{
		ASSERT(*it);
		(*it)->Release(); 
		(*it) = NULL;
	}
	m_aIcon.clear();
	ClearSaveData();
	UpDataListView();
	return true;
}

int KIconManager::UpDataListView()
{
	LPCTSTR pcszRemark = NULL;
	LPCTSTR pcszClip1 = NULL;
    LPCTSTR pcszClip2 = NULL;
	LPTSTR pcszTemp = NULL;
	TCHAR szFrameName[MAX_PATH];
	TCHAR szUITexName[MAX_PATH];
    BYTE *pBuffer = NULL;
	KWndFrameList::GetSelf().ClearFrameGroup();
	int i = 0;
	int nTempFile = 0;
	int nTempFrame = 0;
	int nTempWidth = 0;
	int nTempHeight = 0;
    int nBufferSize = 0;

	for (KIconArray::iterator it = m_aIcon.begin(); it != m_aIcon.end(); ++it) 
	{
		ASSERT(*it); 
		(*it)->GetName(szFrameName, sizeof(szFrameName) / sizeof(szFrameName[0]));
        (*it)->GetFrameName(szUITexName, sizeof(szUITexName) / sizeof(szUITexName[0]));
		nTempFile = (*it)->GetFile();
		nTempFrame = (*it)->GetFrame();
		nTempWidth = (*it)->GetWidth();
		nTempHeight = (*it)->GetHeight();
        pBuffer = (*it)->GetTgaBlock()->GetBufferPtr();
        nBufferSize = (*it)->GetTgaBlock()->GetBufferSize();

        pcszClip1 = _tcsrchr(szUITexName, _T('\\'));
        if (pcszClip1)
            pcszClip1++;
        else
            pcszClip1 = szUITexName;
        pcszClip2 = _tcsrchr(szFrameName, _T(':'));
        if (pcszClip2)
            pcszClip2++;
        else
            pcszClip2 = szFrameName;

		KWndFrameList::GetSelf().AppendFrameGroup(
			pcszClip1, pcszClip2, nTempFile, nTempFrame, nTempWidth, nTempHeight, 
            pcszRemark, pBuffer, nBufferSize
		    );
	}
	return true;
}

int KIconManager::ClearSaveData()
{
	return true;
}

KIconBlock *KIconManager::Get(int nIndex)
{
	if (nIndex >= 0 && nIndex < (int)m_aIcon.size())
		return (*(m_aIcon.begin() + nIndex));
	return NULL;
}

int KIconManager::GetIconCount()
{
    return (int)m_aIcon.size();
}
