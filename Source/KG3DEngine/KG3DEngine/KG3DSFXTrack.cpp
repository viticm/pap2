////////////////////////////////////////////////////////////////////////////////
//
//  Collator : Chiyer (Zhao XinYu)
//  Date     : 2007-2-12 17:14:16
//
////////////////////////////////////////////////////////////////////////////////

#include "StdAfx.h"
#include "KG3DSFXTrack.h"
#include "KG3DSFX.h"
#include "KG3DGraphicsTool.h"

////////////////////////////////////////////////////////////////////////////////
#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static TypeInfo trackTypeInfo = { 
    ".track", SFX_TT_COMMON, TRUE,
};

KG3DSFXTrackTable    g_cTrackTable;

KG3DSFXTrackTable::KG3DSFXTrackTable(void)
{
}

KG3DSFXTrackTable::~KG3DSFXTrackTable(void)
{
	UnInit();
}

HRESULT KG3DSFXTrackTable::Init()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    int nInitResourceManagerBase = false;
    
    hr = TKG3DResourceManagerBase<KG3DSFXTrackData>::Init();
    KGLOG_COM_PROCESS_ERROR(hr);

    nInitResourceManagerBase = true;

    m_pDefaultType = &trackTypeInfo;
    
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (nInitResourceManagerBase)
        {
            hr = TKG3DResourceManagerBase<KG3DSFXTrackData>::UnInit();
            KGLOG_COM_CHECK_ERROR(hr);
        }
    }
    return hrResult;
}

HRESULT KG3DSFXTrackTable::UnInit()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    
    hr = TKG3DResourceManagerBase<KG3DSFXTrackData>::UnInit();
    KGLOG_COM_PROCESS_ERROR(hr);

    m_pDefaultType = NULL;
    
    hrResult = S_OK;
Exit0:
    return hrResult;
}

KG3DSFXTrackData::KG3DSFXTrackData()
{
    m_dwID = 0;
    m_dwType = 0;
    m_ulRefCount = 0;

    m_pKeyFrame = NULL;

    m_dwInterval = 0;
    m_dwFrameNum = 0;
}

KG3DSFXTrackData::~KG3DSFXTrackData(void)
{
    SAFE_DELETE_ARRAY(m_pKeyFrame);
}

ULONG STDMETHODCALLTYPE KG3DSFXTrackData::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG STDMETHODCALLTYPE KG3DSFXTrackData::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        OnReferenceZero(); 
    return uNewRefCount;
}

HRESULT STDMETHODCALLTYPE KG3DSFXTrackData::QueryInterface( 
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppvObject
)
{
    return E_FAIL;
}

HRESULT KG3DSFXTrackData::OnReferenceZero()
{
    g_cTrackTable.ReleaseResource(this);
    return S_OK;
}

HRESULT KG3DSFXTrackData::Init()
{
    return S_OK;
}

HRESULT KG3DSFXTrackData::UnInit()
{
    return S_OK;
}

HRESULT KG3DSFXTrackData::SaveToFile(const char cszFileName[],   unsigned uOption)
{
    return S_OK;
}

unsigned KG3DSFXTrackData::GetType()
{
    return m_dwType;
}

void KG3DSFXTrackData::SetID(DWORD dwID)
{
    m_dwID = dwID;
}

DWORD KG3DSFXTrackData::GetID()
{
    return m_dwID;
}

HRESULT KG3DSFXTrackData::PrepareRender(unsigned uOption)
{
    return E_NOTIMPL;
}

HRESULT KG3DSFXTrackData::PostRender()
{
    return E_NOTIMPL;
}

HRESULT KG3DSFXTrackData::GetDateInformation(
    int *pnMemory,
    int *pnVideoMemory,
    int *pnCompressMem,
    int *pnCompressVideoMem
)
{
    return E_NOTIMPL;
}

unsigned KG3DSFXTrackData::GetLastUseTime()
{
    return 0;
}

unsigned KG3DSFXTrackData::GetUseFrequency()
{
    return 0;
}

HRESULT KG3DSFXTrackData::LoadFromFile(
    const char cszFileName[],
    unsigned uFileNameHash,
    unsigned uOption
)
{
    HRESULT hr = E_FAIL;
    DWORD dwID = 0;
    DWORD dwType = 0;
    DWORD dwFrameNum = 0;
    DWORD dwInterval = 0;
    IFile *pFile = NULL;
    DWORD dwSize = 0;
    
    pFile = g_OpenFile(cszFileName);
    KG_PROCESS_ERROR(pFile);

    dwSize = pFile->Read(&dwID, sizeof(DWORD));
    KG_PROCESS_ERROR(dwSize == sizeof(DWORD));
    KG_PROCESS_ERROR(dwID == MAKEFOURCC('T', 'R', 'A', 'C'));
    
    dwSize = pFile->Read(&dwType, sizeof(DWORD));
    KG_PROCESS_ERROR(dwSize == sizeof(DWORD));
    KG_PROCESS_ERROR(dwType == SFX_TT_COMMON);

    dwSize = pFile->Read(&dwFrameNum, sizeof(DWORD));
    KG_PROCESS_ERROR(dwSize == sizeof(DWORD));
    dwSize = pFile->Read(&dwInterval, sizeof(DWORD));
    KG_PROCESS_ERROR(dwSize == sizeof(DWORD));

    D3DXMATRIX* pKeyFrame = new D3DXMATRIX[dwFrameNum];
    KG_PROCESS_ERROR(pKeyFrame);

    dwSize = pFile->Read(pKeyFrame, sizeof(D3DXMATRIX) * dwFrameNum);
    if (dwSize != sizeof(D3DXMATRIX) * dwFrameNum)
    {
        SAFE_DELETE_ARRAY(pKeyFrame);
        KG_PROCESS_ERROR(false);
    }

    m_pKeyFrame = pKeyFrame;
    m_dwType = dwType;
    m_dwInterval = dwInterval;
    m_dwFrameNum = dwFrameNum;
    m_scName = cszFileName;

    hr = S_OK;

Exit0:
    KG_COM_RELEASE(pFile);
    return hr;
}

KG3DSFXTrack::KG3DSFXTrack(KG3DSFX *pSFX)
{
    ASSERT(pSFX);
    m_pSFX = pSFX;

    m_pData = NULL;
    D3DXMatrixIdentity(&m_matWorld);
}

KG3DSFXTrack::~KG3DSFXTrack()
{
    SAFE_RELEASE(m_pData);
}

void KG3DSFXTrack::SetInterval(DWORD dwInterval)
{
    m_dwInterval = dwInterval;
}

void KG3DSFXTrack::GetTransform(float fCurrentFrame, D3DXMATRIX* pMatrix, float* pTrackFrame /* = NULL */)
{
    ASSERT(m_dwInterval != 0);
    ASSERT(m_pData);
    ASSERT(m_dwFrameNum != 0);

    if (m_dwFrameNum == 0)
    {
        D3DXMatrixIdentity(pMatrix);
    }
    else if (fCurrentFrame <= 0)
    {
        *pMatrix = m_pData->m_pKeyFrame[0];
    }
    else
    {
        DWORD dwSFXInterval = m_pSFX->GetFrameInterval();
        float fTrackFrame = fCurrentFrame * (float)dwSFXInterval / (float)m_dwInterval;

        fTrackFrame -= ((DWORD)fTrackFrame / (m_dwFrameNum - 1) * (m_dwFrameNum - 1));
        int nFrom = (int)fTrackFrame;
        int nTo = nFrom + 1;
        float fFactor = fTrackFrame - (float)nFrom;
        *pMatrix = m_pData->m_pKeyFrame[nTo] * fFactor + (1.0f - fFactor) * m_pData->m_pKeyFrame[nFrom];

        if (pTrackFrame)
            (*pTrackFrame) = fTrackFrame;
    }

    *pMatrix *= m_Matrix;
}

DWORD KG3DSFXTrack::GetInterval() const
{
    return m_dwInterval;
}

DWORD KG3DSFXTrack::GetFrameNum() const
{
    return m_dwFrameNum;
}

HRESULT KG3DSFXTrack::FrameMove()
{
    //if (!m_pSFX)
    //    return E_FAIL;

    //m_matWorld = m_Matrix * (*m_pSFX->GetCurrentWorldMatrix());

    return S_OK;
}

HRESULT KG3DSFXTrack::Render()
{
    ASSERT(m_pData);

    D3DXVECTOR3 pos1, pos2;

    for (int i = 0; i < (int)m_dwFrameNum - 1; ++i)
    {
        pos1 = D3DXVECTOR3(
            m_pData->m_pKeyFrame[i]._41, 
            m_pData->m_pKeyFrame[i]._42, 
            m_pData->m_pKeyFrame[i]._43
        );
        pos2 = D3DXVECTOR3(
            m_pData->m_pKeyFrame[i + 1]._41, 
            m_pData->m_pKeyFrame[i + 1]._42, 
            m_pData->m_pKeyFrame[i + 1]._43
        );
        D3DXVec3TransformCoord(&pos1, &pos1, &m_Matrix);
        D3DXVec3TransformCoord(&pos2, &pos2, &m_Matrix);
        g_cGraphicsTool.DrawLine(&pos1, &pos2, 0xffffffff, 0xffffffff);
    }

    return S_OK;
}

HRESULT KG3DSFXTrack::RenderSelectMark()
{
    return S_OK;
}

void KG3DSFXTrack::GetAABBox(AABBOX *pBox)
{
    ASSERT(m_pData);

    for (int i = 0; i < (int)m_dwFrameNum; ++i)
    {
        D3DXVECTOR3 vPos(
            m_pData->m_pKeyFrame[i]._41, 
            m_pData->m_pKeyFrame[i]._42, 
            m_pData->m_pKeyFrame[i]._43
        );
        D3DXVec3TransformCoord(&vPos, &vPos, &m_Matrix);
        pBox->AddPosition(vPos);
    }
}

HRESULT KG3DSFXTrack::LoadFromFile(const char cszFileName[])
{
    SAFE_RELEASE(m_pData);
	HRESULT hr = g_cTrackTable.LoadResourceFromFile(cszFileName, 0, 0, 
		(IKG3DResourceBase **)&m_pData);
    if (SUCCEEDED(hr))
    {
        m_dwFrameNum = m_pData->m_dwFrameNum;
        m_dwInterval = m_pData->m_dwInterval;
    }

    return hr;
}

std::string KG3DSFXTrack::GetName() const
{
    ASSERT(m_pData);
    return m_pData->m_scName;
}

LPCTSTR KG3DSFXTrack::GetName()
{
	ASSERT(m_pData);
	return m_pData->m_scName.c_str();
}

HRESULT KG3DSFXTrackData::RefreshOption(unsigned uOption)
{
	return S_OK;
}
