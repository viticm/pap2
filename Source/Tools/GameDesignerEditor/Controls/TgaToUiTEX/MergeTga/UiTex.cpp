////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : UiTex.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-12-17 13:49:20
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <algorithm>
#include "KTga.h"
#include "UiTex.h"
#include "KTgaManager.h"
#include "KAnimateManager.h"
#include "KWndAnimateList.h"
#include "KIconManager.h"
////////////////////////////////////////////////////////////////////////////////

KTgaBlock::KTgaBlock()
{
    m_dwStyle = 0x00000000;
    m_szName[0] = _T('\0');
    m_nLeft = 0;
    m_nTop = 0;
    m_nWidth = 0;
    m_nHeight = 0;

    m_nOriginalIndex = 0;
    m_pbyBuffer = NULL;
    m_nBufferSize = 0;
}

KTgaBlock::~KTgaBlock()
{
}

KTgaBlock *KTgaBlock::CreateFromFile(LPCTSTR pcszTgaFileName)
{
    KTgaBlock *pTgaBlock = NULL;
    int nResult = false;
    int nRetCode = false;
    LPCTSTR pcszClip = NULL;
    KTga32 Tga;

    KG_PROCESS_ERROR(pcszTgaFileName);
    KG_PROCESS_ERROR(pcszTgaFileName[0] != _T('\0'));
    
    nRetCode = Tga.CreateFromFile((char *)pcszTgaFileName, true);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = Tga.ReadAllDataToSelfMemory();
    KG_PROCESS_ERROR(nRetCode);

    pTgaBlock = new KTgaBlock;
    KG_PROCESS_ERROR(pTgaBlock);

    nRetCode = pTgaBlock->SetName(pcszTgaFileName);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = pTgaBlock->SetLeft(0);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = pTgaBlock->SetTop(0);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = pTgaBlock->SetWidth(Tga.GetWidth());
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = pTgaBlock->SetHeight(Tga.GetHeight());
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = pTgaBlock->EnableChangeIndex();
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = pTgaBlock->SetOriginalIndex(0);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = pTgaBlock->ClearPosHasBeenSeted();
    KG_PROCESS_ERROR(nRetCode);
    
    nRetCode = pTgaBlock->SetBuffer(
        (BYTE *)Tga.GetDataBufferPointer(), Tga.GetWidth() * Tga.GetHeight() * Tga.GetDepth() / 8
    );
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
        KG_COM_RELEASE(pTgaBlock);
    return pTgaBlock;
}

KTgaBlock *KTgaBlock::CreateFromBuffer(
BYTE *pbyBuffer, int nBufferSize, int nWidth, int nHeight, LPCTSTR pcszName
)
{
    KTgaBlock *pTgaBlock = NULL;
    int nResult = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(pbyBuffer);
    KG_PROCESS_ERROR(nBufferSize > 0);
    KG_PROCESS_ERROR(pcszName);

    pTgaBlock = new KTgaBlock;
    KG_PROCESS_ERROR(pTgaBlock);

    nRetCode = pTgaBlock->SetName(pcszName);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = pTgaBlock->SetWidth(nWidth);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = pTgaBlock->SetHeight(nHeight);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = pTgaBlock->SetBuffer(pbyBuffer, nBufferSize);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
        KG_COM_RELEASE(pTgaBlock);
    return pTgaBlock;
}

KTgaBlock *KTgaBlock::CreateFromBuffer( 
    BYTE *pbyBuffer, int nBufferSize, int nWidth, int nHeight, 
    int nOriginalLeft, int nOriginalTop, int nIsIndexChangeable, int nOriginalIndex, 
    int nPosSeted, LPCTSTR pcszName 
)
{
    KTgaBlock *pTgaBlock = NULL;
    int nResult = false;
    int nRetCode = false;

    KG_PROCESS_ERROR(pbyBuffer);
    KG_PROCESS_ERROR(nBufferSize > 0);
    KG_PROCESS_ERROR(pcszName);

    pTgaBlock = new KTgaBlock;
    KG_PROCESS_ERROR(pTgaBlock);

    nRetCode = pTgaBlock->SetName(pcszName);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = pTgaBlock->SetLeft(nOriginalLeft);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = pTgaBlock->SetTop(nOriginalTop);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = pTgaBlock->SetWidth(nWidth);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = pTgaBlock->SetHeight(nHeight);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = pTgaBlock->DisableChangeIndex();
    KG_PROCESS_ERROR(nRetCode);
    if (nIsIndexChangeable)
    {
        nRetCode = pTgaBlock->EnableChangeIndex();
        KG_PROCESS_ERROR(nRetCode);
    }

    nRetCode = pTgaBlock->SetOriginalIndex(nOriginalIndex);
    KG_PROCESS_ERROR(nRetCode);

    nRetCode = pTgaBlock->SetPosHasBeenSeted();
    KG_PROCESS_ERROR(nRetCode);
    if (!nPosSeted)
    {
        nRetCode =pTgaBlock->ClearPosHasBeenSeted();
        KG_PROCESS_ERROR(nRetCode);
    }
    
    nRetCode = pTgaBlock->SetBuffer(pbyBuffer, nBufferSize);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
        KG_COM_RELEASE(pTgaBlock);
    return pTgaBlock;
}

void KTgaBlock::Release()
{
    KG_DELETE_ARRAY(m_pbyBuffer);
    delete this;
}

int KTgaBlock::SetName(LPCTSTR pcszName)
{
    int nResult = false;

    KG_PROCESS_ERROR(pcszName);

    _tcsncpy(m_szName, pcszName, sizeof(m_szName) / sizeof(TCHAR));
    m_szName[sizeof(m_szName) / sizeof(TCHAR) - 1] = _T('\0');

    nResult = true;
Exit0:
    return nResult;
}

int KTgaBlock::EnableChangeIndex()
{
    m_dwStyle &= ~KEEP_ORIGINAL_INDEX;
    return true;
}

int KTgaBlock::DisableChangeIndex()
{
    m_dwStyle |= KEEP_ORIGINAL_INDEX;
    return true;
}

int KTgaBlock::SetPosHasBeenSeted()
{
    m_dwStyle |= HASE_BEEEN_SET_POS;
    return true;
}

int KTgaBlock::ClearPosHasBeenSeted()
{
    m_dwStyle &= ~HASE_BEEEN_SET_POS;
    return true;
}

int KTgaBlock::SetLeft(int nLeft)
{
    m_nLeft = nLeft;
    return true;
}

int KTgaBlock::SetTop(int nTop)
{
    m_nTop = nTop;
    return true;
}

int KTgaBlock::SetWidth(int nWidth)
{
    m_nWidth = nWidth;
    return true;
}

int KTgaBlock::SetHeight(int nHeight)
{
    m_nHeight = nHeight;
    return true;
}

int KTgaBlock::SetOriginalIndex(int nIndex)
{
    m_nOriginalIndex = nIndex;
    return true;
}

int KTgaBlock::SetBuffer(BYTE *pbyBuffer, int nBufferSize)
{
    int nResult = false;

    KG_PROCESS_ERROR(pbyBuffer);
    KG_PROCESS_ERROR(nBufferSize > 0);

    KG_DELETE_ARRAY(m_pbyBuffer);
    m_pbyBuffer = new BYTE[nBufferSize];
    KG_PROCESS_ERROR(m_pbyBuffer);

    memcpy(m_pbyBuffer, pbyBuffer, nBufferSize);

    m_nBufferSize = nBufferSize;

    nResult = true;
Exit0:
    if (!nResult)
        KG_DELETE_ARRAY(m_pbyBuffer);
    return nResult;
}

int KTgaBlock::SetInAnimateGroup()
{
    m_dwStyle |= HAS_AREADY_ANIMATE_GROUP;
    return true;
}

int KTgaBlock::ClearInAnimateGroup()
{
    m_dwStyle &= ~HAS_AREADY_ANIMATE_GROUP;
    return true;
}

int KTgaBlock::SetByRect(RECT &rcSet)
{
    m_nLeft = rcSet.left;
    m_nTop = rcSet.right;
    m_nWidth = rcSet.right - rcSet.left;
    m_nHeight = rcSet.bottom - rcSet.top;
    return true;
}

int KTgaBlock::GetByRect(RECT &rcGet)
{
    rcGet.left = m_nLeft;
    rcGet.top = m_nTop;
    rcGet.right = m_nLeft + m_nWidth;
    rcGet.bottom = m_nTop + m_nHeight;
    return true;
}

int KTgaBlock::IsAlreadyOnAnimateGroup()
{
    return (m_dwStyle & HAS_AREADY_ANIMATE_GROUP) == HAS_AREADY_ANIMATE_GROUP;
}

int KTgaBlock::GetName(LPTSTR pszNameBuffer, int nNameBufferSize)
{
    int nResult = false;

    KG_PROCESS_ERROR(pszNameBuffer);
    KG_PROCESS_ERROR(nNameBufferSize > 0);

    _tcsncpy(pszNameBuffer, m_szName, nNameBufferSize);
    pszNameBuffer[nNameBufferSize - 1] = _T('\0');

    nResult = true;
Exit0:
    return nResult;
}

LPCTSTR KTgaBlock::GetName()
{
    return m_szName;
}

int KTgaBlock::IsIndexChangeable()
{
    return (m_dwStyle & KEEP_ORIGINAL_INDEX) != KEEP_ORIGINAL_INDEX;
}

int KTgaBlock::IsPosHasBeenSeted()
{
    return (m_dwStyle & HASE_BEEEN_SET_POS) ==  HASE_BEEEN_SET_POS;
}

int KTgaBlock::GetLeft()
{
    return m_nLeft;
}

int KTgaBlock::GetTop()
{
    return m_nTop;
}

int KTgaBlock::GetWidth()
{
    return m_nWidth;
}

int KTgaBlock::GetHeight()
{
    return m_nHeight;
}

int KTgaBlock::GetOriginalIndex()
{
    return m_nOriginalIndex;
}

BYTE *KTgaBlock::GetBufferPtr()
{
    return m_pbyBuffer;
}

int KTgaBlock::GetBufferSize()
{
    return m_nBufferSize;
}

int KTgaBlock::PtInBlock(int nX, int nY)
{
    if (
        nX >= m_nLeft &&
        nX < m_nLeft + m_nWidth &&
        nY >= m_nTop &&
        nY < m_nTop + m_nHeight
    )
        return true;

    return false;
}

KIconBlock::KIconBlock()
{
	m_pTgaBlock = NULL;
	m_szUITexName[0] = _T('\0');
    m_szFrameName[0] = _T('\0');
	m_nFrame = 0;
	m_nWidth = 0;
	m_nHeight = 0;
    m_pBitmap = NULL;
}

KIconBlock::~KIconBlock()
{
}

KIconBlock *KIconBlock::CreateFromBuffer(int nIconID, LPCTSTR pcszFileName, int nFrame)
{
	int nResult = false;
	int nRetCode = false;
	KIconBlock *pIconBlock = NULL;

	pIconBlock = new KIconBlock;

	KG_PROCESS_ERROR(pcszFileName);

	nResult = true;
Exit0:
	if (!nResult)
		KG_COM_RELEASE(pIconBlock);
	return pIconBlock;
}

KIconBlock *KIconBlock::CreateFromBuffer(
	BYTE *pbyBuffer, int nBufferSize, int nId, int nFrame,
    int nWidth, int nHeight, LPCTSTR pcszName, LPCTSTR pcszFrameName
)
{
	int nResult = false;
	int	nRetCode = false;

	int nNewLeft = 0;
	int nNewTop = 0;

	KIconBlock *pIconBlock = NULL;
	KTgaBlock *pTgaBlock = NULL;

	pIconBlock = new KIconBlock;
	KG_PROCESS_ERROR(pIconBlock);

	pTgaBlock = KTgaBlock::CreateFromBuffer(
        pbyBuffer,  nBufferSize,  nWidth,  nHeight, pcszName
	);
	KG_PROCESS_ERROR(pTgaBlock);
	nRetCode = pIconBlock->SetTgaBlock(pTgaBlock);
	KG_PROCESS_ERROR(nRetCode);
    nRetCode = pIconBlock->SetName(pcszName);
    KG_PROCESS_ERROR(nRetCode);
    nRetCode = pIconBlock->SetFrameName(pcszFrameName);
    KG_PROCESS_ERROR(nRetCode);
	nRetCode = pIconBlock->SetFile(nId);
	KG_PROCESS_ERROR(nRetCode);

	nRetCode = pIconBlock->SetFrame(nFrame);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = pIconBlock->SetWidth(nWidth);
	KG_PROCESS_ERROR(nRetCode);
	nRetCode = pIconBlock->SetHeight(nHeight);
	KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
	if (!nResult)
	{
		KG_COM_RELEASE(pIconBlock);
		KG_COM_RELEASE(pTgaBlock);
	}
	return pIconBlock;
}

int KIconBlock::SetFile(int nFile)
{
	m_nFile = nFile;
	return true;
}

int KIconBlock::SetFrame(int nFrame)
{
	m_nFrame = nFrame;
	return true;
}

int KIconBlock::SetName(LPCTSTR pcszUITexName)
{
	_tcsncpy(m_szUITexName, pcszUITexName, sizeof(m_szUITexName)/sizeof(TCHAR));
    m_szUITexName[sizeof(m_szUITexName)/ sizeof(m_szUITexName[0]) - 1] = _T('\0');
	return true;
}

int KIconBlock::SetFrameName(LPCTSTR pcszFrameName)
{
    _tcsncpy(m_szFrameName, pcszFrameName, sizeof(m_szFrameName)/sizeof(TCHAR));
    m_szFrameName[sizeof(m_szFrameName)/ sizeof(m_szFrameName[0]) - 1] = _T('\0');
    return true;
}

int KIconBlock::SetTgaBlock(KTgaBlock *pTgaBlock)
{
    ASSERT(pTgaBlock);
	m_pTgaBlock = pTgaBlock;
	return true;
}

int KIconBlock::SetWidth(int nWidth)
{
	m_nWidth = nWidth;
	return true;
}

int KIconBlock::SetHeight(int nHeight)
{
	m_nHeight = nHeight;
	return true;
}

int KIconBlock::SetBitmap(Gdiplus::Bitmap *pBitmap)
{
    m_pBitmap = pBitmap;
    return true;
}

KTgaBlock *KIconBlock::GetTgaBlock()
{
	return m_pTgaBlock;
}

int KIconBlock::GetWidth()
{	
	return m_nWidth;
}

int KIconBlock::GetHeight()
{
	return m_nHeight;
}

int KIconBlock::GetFrame()
{
	return m_nFrame;
}

int KIconBlock::GetFile()
{
	return m_nFile;
}

Gdiplus::Bitmap *KIconBlock::GetBitmap()
{
    return m_pBitmap;
}

int KIconBlock::GetName(OUT LPTSTR pszUITexName, int nUITexNameBufferSize)
{
	int nResult = false;

	KG_PROCESS_ERROR(pszUITexName);
	KG_PROCESS_ERROR(nUITexNameBufferSize > 0);

	_tcsncpy(pszUITexName, m_szUITexName, nUITexNameBufferSize);
	pszUITexName[nUITexNameBufferSize - 1] = _T('\0');

	nResult = true;
Exit0:
	return nResult;
}

int KIconBlock::GetFrameName(OUT LPTSTR pszFrameName, int nFrameNameBufferSize)
{
    int nResult = false;

    KG_PROCESS_ERROR(pszFrameName);
    KG_PROCESS_ERROR(nFrameNameBufferSize > 0);

    _tcsncpy(pszFrameName, m_szFrameName, nFrameNameBufferSize);
    pszFrameName[nFrameNameBufferSize - 1] = _T('\0');

    nResult = true;
Exit0:
    return nResult;
}

void KIconBlock::Release()
{
	delete this;
}

KAnimateBlock::KAnimateBlock()
{
    m_dwStyle = 0x00000000;
    m_szName[0] = _T('\0');
    m_nOriginalIndex = -1;
}

KAnimateBlock::~KAnimateBlock()
{
}

KAnimateBlock *KAnimateBlock::Create( 
    LPCTSTR pcszName, int nIsIndexChangeable, int nOriginalIndex 
)
{
    int nResult = false;
    int nRetCode = false;
    KAnimateBlock *pBlock = NULL;

    pBlock = new KAnimateBlock;
    KG_PROCESS_ERROR(pBlock);

    nRetCode = pBlock->SetOriginalIndex(nOriginalIndex);
    KG_PROCESS_ERROR(nRetCode);

    if (nIsIndexChangeable)
        pBlock->EnableChangeIndex();
    else
        pBlock->DisableChangeIndex();

    nRetCode = pBlock->SetName(pcszName);
    KG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
        KG_DELETE(pBlock);
    return pBlock;
}

void KAnimateBlock::Release()
{
    delete this;
}

int KAnimateBlock::AppendFrame(LPCTSTR pcszName)
{
    int nResult = false;

    KG_PROCESS_ERROR(pcszName);
    KG_PROCESS_ERROR(pcszName[0] != _T('\0'));

    m_aFrame.push_back(pcszName);

    nResult = true;
Exit0:
    return nResult;
}

int KAnimateBlock::InsertFrame(LPCTSTR pcszName)
{
    int nResult = false;
    int nRetCode = false;
    int nFrameIndexSelf = -1;
    int nFrameIndexBrother = -1;;
    LPCTSTR pcszClip = NULL;
    KG_PROCESS_ERROR(pcszName);
    KG_PROCESS_ERROR(pcszName[0] != _T('\0'));

    pcszClip = _tcsrchr(pcszName, _T('_'));
    if (pcszClip)
    {
        ++pcszClip;
        nFrameIndexSelf = _ttoi(pcszClip);
    }

    for (KAnimateFrameArray::iterator it = m_aFrame.begin(); it != m_aFrame.end(); ++it) 
    {
        pcszClip = _tcsrchr((*it).c_str(), _T('_'));
        if (!pcszClip)
            continue;
        ++pcszClip;
        nFrameIndexBrother = _ttoi(pcszClip);
        if (nFrameIndexBrother < nFrameIndexSelf)
            continue;

        m_aFrame.insert(it, pcszName);
        KG_PROCESS_SUCCESS(1);
    }

    AppendFrame(pcszName);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KAnimateBlock::InsertFrame(int nIndex, LPCTSTR pcszName)
{
    int nResult = false;

    KG_PROCESS_ERROR(pcszName);
    KG_PROCESS_ERROR(pcszName[0] != _T('\0'));

    if (nIndex < 0)
        nIndex = 0;
    if (nIndex > (int)m_aFrame.size())
        nIndex = (int)m_aFrame.size();

    m_aFrame.insert(m_aFrame.begin() + nIndex, pcszName);

    nResult = true;
Exit0:
    return nResult;
}

int KAnimateBlock::RemoveFrame(LPCTSTR pcszName)
{
    int nResult = false;
    int nRetCode = false;
    KG_PROCESS_ERROR(pcszName);
    KG_PROCESS_ERROR(pcszName[0] != _T('\0'));

    for (KAnimateFrameArray::iterator it = m_aFrame.begin(); it != m_aFrame.end(); ++it) 
    {
        nRetCode = _tcsicmp(pcszName, (*it).c_str());
        if (nResult)
            continue;
        m_aFrame.erase(it);
        KG_PROCESS_SUCCESS(1);
    }

    KG_PROCESS_ERROR(false);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KAnimateBlock::RemoveFrame(int nIndex)
{
    if (nIndex >= 0 && nIndex < (int)m_aFrame.size())
    {
        m_aFrame.erase(m_aFrame.begin() + nIndex);
        return true;
    }

    return false;
}

int KAnimateBlock::Clear()
{
    m_aFrame.clear();
    return true;
}

int KAnimateBlock::MoveUp(int nIndex)
{
    int nResult = false;
    TCHAR szName[MAX_PATH];
    KG_PROCESS_ERROR(nIndex > 0);
    KG_PROCESS_ERROR(nIndex < (int)m_aFrame.size());

    _tcsncpy(szName, (*(m_aFrame.begin() + nIndex)).c_str(), sizeof(szName) / sizeof(TCHAR));
    szName[sizeof(szName) / sizeof(TCHAR) - 1] = _T('\0');
    
    *(m_aFrame.begin() + nIndex) = *(m_aFrame.begin() + nIndex - 1);
    *(m_aFrame.begin() + nIndex - 1) = szName;

    nResult = true;
Exit0:
    return nResult;
}

int KAnimateBlock::MoveDown(int nIndex)
{
    int nResult = false;
    TCHAR szName[MAX_PATH];
    KG_PROCESS_ERROR(nIndex >= 0);
    KG_PROCESS_ERROR(nIndex < (int)m_aFrame.size() - 1);

    _tcsncpy(szName, (*(m_aFrame.begin() + nIndex)).c_str(), sizeof(szName) / sizeof(TCHAR));
    szName[sizeof(szName) / sizeof(TCHAR) - 1] = _T('\0');
    
    *(m_aFrame.begin() + nIndex) = *(m_aFrame.begin() + nIndex + 1);
    *(m_aFrame.begin() + nIndex + 1) = szName;

    nResult = true;
Exit0:
    return nResult;
}

int KAnimateBlock::GetAnimateFramCount()
{
    return (int)m_aFrame.size();
}

int KAnimateBlock::SetName(LPCTSTR pcszName)
{
    int nResult = false;

    KG_PROCESS_ERROR(pcszName);
    KG_PROCESS_ERROR(pcszName[0] != _T('\0'));

    _tcsncpy(m_szName, pcszName, sizeof(m_szName) / sizeof(TCHAR));
    m_szName[sizeof(m_szName) / sizeof(TCHAR) - 1] = _T('\0');

    nResult = true;
Exit0:
    return true;
}

int KAnimateBlock::GetName(OUT LPTSTR pszName, int nNameBufferSize)
{
    int nResult = false;

    KG_PROCESS_ERROR(pszName);
    KG_PROCESS_ERROR(nNameBufferSize > 0);

    _tcsncpy(pszName, m_szName, nNameBufferSize);
    pszName[nNameBufferSize - 1] = _T('\0');

    nResult = true;
Exit0:
    return nResult;
}

LPCTSTR KAnimateBlock::GetName()
{
    return m_szName;
}

int KAnimateBlock::SetOriginalIndex(int nIndex)
{
    m_nOriginalIndex = nIndex;
    return true;
}

int KAnimateBlock::GetOriginalIndex()
{
    return m_nOriginalIndex;
}

int KAnimateBlock::EnableChangeIndex()
{
    m_dwStyle &= ~KEEP_ORIGINAL_INDEX;
    return true;
}

int KAnimateBlock::DisableChangeIndex()
{
    m_dwStyle |= KEEP_ORIGINAL_INDEX;
    return true;
}

int KAnimateBlock::IsIndexChangeable()
{
    return (m_dwStyle & KEEP_ORIGINAL_INDEX) != KEEP_ORIGINAL_INDEX;
}

int KAnimateBlock::LookFromNameShouldFrameMyson(LPCTSTR pcszFrameName)
{
    int nResult = false;
    int nRetCode = false;
    LPCTSTR pcszClip = NULL;
    KG_PROCESS_ERROR(pcszFrameName);
    KG_PROCESS_ERROR(pcszFrameName[0] != _T('\0'));

    nRetCode = _tcsnicmp(pcszFrameName, m_szName, _tcslen(m_szName));
    KG_PROCESS_ERROR(!nRetCode);
    
    pcszClip = pcszFrameName + _tcslen(m_szName);
    KG_PROCESS_ERROR(*pcszClip == _T('_'));

    nResult = true;
Exit0:
    return nResult;
}

int KAnimateBlock::IsFrameMySon(LPCTSTR pcszFrameName)
{
    int nResult = false;
    int nRetCode = false;
    LPCTSTR pcszClip = NULL;
    KG_PROCESS_ERROR(pcszFrameName);
    KG_PROCESS_ERROR(pcszFrameName[0] != _T('\0'));

    for (KAnimateFrameArray::iterator it = m_aFrame.begin(); it != m_aFrame.end(); ++it)
    {
        nRetCode = _tcsicmp(pcszFrameName, (*it).c_str());
        KG_PROCESS_SUCCESS(nRetCode == 0);
    }
    
    KG_PROCESS_ERROR(false);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KAnimateBlock::IsMeShouldBeSave()
{
    int nResult = false;
    int nRetCode = false;
    KG_PROCESS_SUCCESS(!IsIndexChangeable());

    for (KAnimateFrameArray::iterator it = m_aFrame.begin(); it != m_aFrame.end(); ++it) 
    {
        nRetCode = KTgaManager::GetSelf().IsFrameShoudBeSaved((*it).c_str());
        KG_PROCESS_SUCCESS(nRetCode);
    }

    KG_PROCESS_ERROR(false);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KAnimateBlock::FormateSaveData()
{
    int nResult = false;
    int nRetCode = false;
    int nCount = 0;
    std::vector<int> anSaveData;
    
    for (KAnimateFrameArray::iterator it = m_aFrame.begin(); it != m_aFrame.end(); ++it) 
    {
        int nFrame = KTgaManager::GetSelf().GetSaveIndex((*it).c_str());
        if (nFrame == -1)
            continue;
        anSaveData.push_back(nFrame);
    }

    nCount = (int)anSaveData.size();
    KG_PROCESS_ERROR(nCount);

    KAnimateManager::GetSelf().AppendSaveData(nCount);
    for (int i = 0; i < nCount; ++i) 
    {
        KAnimateManager::GetSelf().AppendSaveData(anSaveData[i]);
    }

    nResult = true;
Exit0:
    return nResult;
}

int KAnimateBlock::ReplaceName(LPCTSTR pcszReplaced, LPCTSTR pcszReplace)
{
    int nResult = false;
    int nRetCode = false;
    KG_PROCESS_ERROR(pcszReplaced);
    KG_PROCESS_ERROR(pcszReplace);

    for (KAnimateFrameArray::iterator it = m_aFrame.begin(); it != m_aFrame.end(); ++it) 
    {
        nRetCode = _tcsicmp(pcszReplaced, (*it).c_str());
        if (nResult)
            continue;
        (*it) = pcszReplace;
        KG_PROCESS_SUCCESS(true);
    }
    
    KG_PROCESS_ERROR(false);

Exit1:
    nResult = true;
Exit0:
    return nResult;
}

int KAnimateBlock::UpdataListView()
{
    LPCTSTR pcszClip = NULL;
    LPCTSTR pcszTemp = NULL;
    KWndAnimateList::GetSelf().ClearAnimateFrame();

    for (KAnimateFrameArray::iterator it = m_aFrame.begin(); it != m_aFrame.end(); ++it)
    {
        pcszClip = _tcsrchr((*it).c_str(), _T('\\'));
        if (pcszClip)
            pcszClip++;
        else
            pcszClip = (*it).c_str();
        pcszTemp = _tcsrchr(pcszClip, _T('/'));
        if (pcszTemp)
            pcszClip = pcszTemp + 1;
        KWndAnimateList::GetSelf().AppendAnimateFrame(pcszClip);
    }
    
    return true;
}

LPCTSTR KAnimateBlock::Get(int nIndex)
{
    if (nIndex >= 0 && nIndex < (int)m_aFrame.size())
        return ((*(m_aFrame.begin() + nIndex)).c_str());
    return NULL;
}

int KAnimateBlock::GetFrameCount()
{
    return (int)m_aFrame.size();
}
