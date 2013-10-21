////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DUITexture.cpp
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-08-28 16:43:32
//  Comment     : font def
//
////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "./KG3DUITexture.h"
#include "KG3DEngineManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//#define SLICE_DEBUG

namespace
{
    UINT const SLICE_PADDING = 3;

    template < typename TYPE >
    inline TYPE GeneratePadding( TYPE In )
    {
        return ( In + SLICE_PADDING );
    }

    inline int GeneratePowerOf2(int in)
    {
        in -= 1;

        in |= in >> 16;
        in |= in >> 8;
        in |= in >> 4;
        in |= in >> 2;
        in |= in >> 1;

        return in + 1;
    }

    VOID WINAPI GenerateColorFill(D3DXVECTOR4* pOut, const D3DXVECTOR2* pTexCoord, const D3DXVECTOR2* pTexelSize, LPVOID pData)
    {
        ASSERT(pOut);
        *pOut = D3DXVECTOR4(0.0f, 0.0f, 0.0f, 0.0f);
    }

    inline UINT GetImageType(UINT uIndex)
    {
        return (uIndex & 0xFF000000) >> 24;
    }

    inline UINT MakeSliceIndex(UINT uMainIndex, UINT uSubIndex)
    {
        ASSERT(uMainIndex < 0x00FF);
        ASSERT(uSubIndex < 0xFFFF);

        return (KG3DUITexture::IMAGE_TYPE_SLICE << 24) | (uMainIndex << 16) | uSubIndex;
    }

    inline UINT GetSliceMainIndex(UINT uIndex)
    {
        return (uIndex & 0x00FF0000) >> 16;
    }

    inline UINT GetSliceSubIndex(UINT uIndex)
    {
        return uIndex & 0x0000FFFF;
    }

    inline UINT MakeNormalIndex(UINT uIndex)
    {
        ASSERT(uIndex < 0x00FFFFFF);

        return (KG3DUITexture::IMAGE_TYPE_NORMAL << 24) | uIndex;
    }

    inline UINT GetNormalIndex(UINT uIndex)
    {
        return uIndex & 0x00FFFFFF;
    }

}


KG3DUITexture::KG3DUITexture( )
: m_pD3DDevice(NULL)
, m_TextureFormat(D3DFMT_UNKNOWN)
, m_TextureByte(0)
, m_uTextureWidth(512)
, m_uTextureHeight(512)
{
}

KG3DUITexture::~KG3DUITexture()
{
    ASSERT(m_pD3DDevice == NULL);
}


HRESULT KG3DUITexture::Initialize(LPDIRECT3DDEVICE9 pD3DDevice)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(pD3DDevice);

    m_pD3DDevice = pD3DDevice;
    m_pD3DDevice->AddRef();

    hr = CheckD3DTextureCaps(pD3DDevice);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = CheckD3DTextureFormat(pD3DDevice);
    KGLOG_COM_PROCESS_ERROR(hr);

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        SAFE_RELEASE(m_pD3DDevice);
    }
    return hrResult;
}

HRESULT KG3DUITexture::CleanUp()
{
    for (size_t nIndex = 0; nIndex < m_aNormalImage.size(); ++nIndex)
    {
        KNormalImage& Image = m_aNormalImage[nIndex];

        SAFE_RELEASE(Image.pTexture);
    }

    m_aNormalImage.clear();

    for (size_t nIndex = 0; nIndex < m_aSliceImage.size(); ++nIndex)
    {
        KSliceImage& Image = m_aSliceImage[nIndex];

        SAFE_RELEASE(Image.pTexture);
    }

    m_aSliceImage.clear();

    SAFE_RELEASE(m_pD3DDevice);

    return S_OK;
}

HRESULT KG3DUITexture::OnLostDevice()
{
    for (size_t nIndex = 0; nIndex < m_aNormalImage.size(); ++nIndex)
    {
        KNormalImage& Image = m_aNormalImage[nIndex];

        if (Image.dwFlags & KG3DUI_IMAGE_FLAGS_POOL_DEFAULT)
        {
            SAFE_RELEASE(Image.pTexture);
        }
    }

    for (size_t nIndex = 0; nIndex < m_aSliceImage.size(); ++nIndex)
    {
        KSliceImage& Image = m_aSliceImage[nIndex];

        if (Image.dwFlags & KG3DUI_IMAGE_FLAGS_POOL_DEFAULT)
        {
            SAFE_RELEASE(Image.pTexture);
        }
    }

    return S_OK;
}

HRESULT KG3DUITexture::OnResetDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    LPDIRECT3DTEXTURE9 pTexture = NULL;

    for (size_t nIndex = 0; nIndex < m_aSliceImage.size(); ++nIndex)
    {
        KSliceImage& Image = m_aSliceImage[nIndex];

        if (Image.dwFlags & KG3DUI_IMAGE_FLAGS_POOL_DEFAULT)
        {
            SAFE_RELEASE(pTexture);

            pTexture = CreateTexture(m_uTextureWidth, m_uTextureHeight, Image.dwFlags);
            KGLOG_PROCESS_ERROR(pTexture);

            hr = D3DXFillTexture(pTexture, GenerateColorFill, NULL);
            KGLOG_COM_PROCESS_ERROR(hr);

            Image.pTexture = pTexture;
            pTexture->AddRef();
        }
    }

    for (size_t nIndex = 0; nIndex < m_aNormalImage.size(); ++nIndex)
    {
        KNormalImage& Image = m_aNormalImage[nIndex];

        if (Image.dwFlags & KG3DUI_IMAGE_FLAGS_POOL_DEFAULT)
        {
            SAFE_RELEASE(pTexture);

            pTexture = CreateTexture(Image.uWidth, Image.uHeight, Image.dwFlags);
            KGLOG_PROCESS_ERROR(pTexture);

            hr = D3DXFillTexture(pTexture, GenerateColorFill, NULL);
            KGLOG_COM_PROCESS_ERROR(hr);

            Image.pTexture = pTexture;
            pTexture->AddRef();
        }
    }

    hrResult = S_OK;
Exit0:
    SAFE_RELEASE(pTexture);
    return hrResult;
}

void KG3DUITexture::ReleaseNormalImage(UINT uIndex)
{
    int nResult = false;
    UINT uNormalIndex = 0;
    KNormalImage* pImage = NULL;

    uNormalIndex = GetNormalIndex(uIndex);
    KGLOG_PROCESS_ERROR(uNormalIndex < (UINT)m_aNormalImage.size());

    pImage = &m_aNormalImage[uNormalIndex];

#ifdef SLICE_DEBUG
    if (pImage->pTexture)
    {	
        HRESULT hrRetCode = E_FAIL;
        TCHAR szBuffer[MAX_PATH] = { 0 };

        _sntprintf(szBuffer, _countof(szBuffer), _T("Normal_%02d.tga"), uNormalIndex);
        szBuffer[sizeof(szBuffer)/sizeof(szBuffer[0]) - 1] = _T('\0'); 

        hrRetCode = D3DXSaveTextureToFile(szBuffer, D3DXIFF_TGA, pImage->pTexture, NULL);
        KGLOG_COM_PEROCESS_ERROR(hrRetCode);
    }
#endif

    pImage->uWidth = 0;
    pImage->uHeight = 0;
    pImage->dwFlags = 0;

    nResult = true;
Exit0:
    if (pImage)
    {
        SAFE_RELEASE(pImage->pTexture);
    }
}

void KG3DUITexture::ReleaseSliceImage(UINT uIndex)
{
    UINT uMainIndex = 0;
    UINT uSubIndex = 0;
    KSliceImage* pImage = NULL;

    uMainIndex = GetSliceMainIndex(uIndex);
    KGLOG_PROCESS_ERROR(uMainIndex < (UINT)m_aSliceImage.size());

    pImage = &m_aSliceImage[uMainIndex];

    uSubIndex = GetSliceSubIndex(uIndex);
    KGLOG_PROCESS_ERROR(uSubIndex < (UINT)pImage->aUsed.size());

    ASSERT(pImage->aUsed[uSubIndex]);

    pImage->aUsed[uSubIndex] = false;

Exit0:
    return;
}

void KG3DUITexture::ReleaseImage(DWORD dwImageID)
{
    switch (GetImageType(dwImageID))
    {
    case IMAGE_TYPE_NONE:
        break;
    case IMAGE_TYPE_NORMAL:
        ReleaseNormalImage(dwImageID);
        break;
    case IMAGE_TYPE_SLICE:
        ReleaseSliceImage(dwImageID);
        break;
    default:
        ASSERT(0);
        break;
    };
}

UINT KG3DUITexture::GetFreeNormalImageIndex(UINT uWidth, UINT uHeight)
{
    UINT uIndex = 0;
    UINT uSize = (UINT)m_aNormalImage.size();
    for (; uIndex < uSize; ++uIndex)
    {
        KNormalImage& Image = m_aNormalImage[uIndex];
        if (Image.pTexture == NULL)
        {
            break;
        }
    }

    if (uIndex == uSize)
        m_aNormalImage.resize(uSize + 1);

    KNormalImage& Image = m_aNormalImage[uIndex];
    Image.pTexture = NULL;
    Image.uWidth = uWidth;
    Image.uHeight = uHeight;
    return MakeNormalIndex(uIndex);
}


UINT KG3DUITexture::GetFreeSliceImageIndex(UINT uWidth, UINT uHeight)
{
    UINT uMainSize = (UINT)m_aSliceImage.size();
    for (UINT uMainIndex = 0; uMainIndex < uMainSize; ++uMainIndex)
    {
        KSliceImage& Image = m_aSliceImage[uMainIndex];
        if (Image.uSubWidth == uWidth && Image.uSubHeight == uHeight)
        {
            UINT uSubImageSize = (UINT)Image.aUsed.size();
            for (UINT uSubIndex = 0; uSubIndex < uSubImageSize; ++uSubIndex)
            {
                if (!Image.aUsed[uSubIndex])
                {
                    Image.aUsed[uSubIndex] = true;
                    return MakeSliceIndex(uMainIndex, uSubIndex);
                }
            }
        }
    }

    m_aSliceImage.resize(uMainSize + 1);
    KSliceImage& Image = m_aSliceImage[uMainSize];

    Image.pTexture = NULL;
    Image.uSubWidth = uWidth;
    Image.uSubHeight = uHeight;

    UINT uSubImageSize = (m_uTextureWidth / GeneratePadding(uWidth)) * (m_uTextureHeight / GeneratePadding(uHeight));

    Image.aUsed.resize(uSubImageSize);
    for (UINT uSubIndex = 1; uSubIndex < uSubImageSize; ++uSubIndex)
        Image.aUsed[uSubIndex] = false;
    Image.aUsed[0] = true;

    return MakeSliceIndex(uMainSize, 0);
}


int KG3DUITexture::CreateNormalImage(UINT uWidth, UINT uHeight, LPDWORD pdwImageID, DWORD dwFlags)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    UINT uIndex = ERROR_IMAGE_INDEX;
    UINT uNormalIndex = 0;
    KNormalImage* pImage = NULL;
    LPDIRECT3DTEXTURE9 pTexture = NULL;

    uIndex = GetFreeNormalImageIndex(uWidth, uHeight);

    uNormalIndex = GetNormalIndex(uIndex);
    KGLOG_PROCESS_ERROR(uNormalIndex < (UINT)m_aNormalImage.size());

    pImage = &m_aNormalImage[uNormalIndex];

    ASSERT(pImage->pTexture == NULL);

    pTexture = CreateTexture(uWidth, uHeight, dwFlags);
    KGLOG_PROCESS_ERROR(pTexture);

    hr = D3DXFillTexture(pTexture, GenerateColorFill, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    pImage->dwFlags = dwFlags;
    pImage->pTexture = pTexture;
    pImage->pTexture->AddRef();

    *pdwImageID = uIndex;

    nResult = true;
Exit0:
    SAFE_RELEASE(pTexture);

    if (!nResult)
    {
        if (uIndex != ERROR_IMAGE_INDEX)
        {
            ReleaseNormalImage(uIndex);
        }
    }
    return nResult;
}

int KG3DUITexture::CreateSliceImage(UINT uWidth, UINT uHeight, LPDWORD pdwImageID, DWORD dwFlags)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    UINT uIndex = ERROR_IMAGE_INDEX;
    UINT uMainIndex = 0;
    KSliceImage* pImage = NULL;
    LPDIRECT3DTEXTURE9 pTexture = NULL;

    uIndex = GetFreeSliceImageIndex(uWidth, uHeight);

    uMainIndex = GetSliceMainIndex(uIndex);
    KGLOG_PROCESS_ERROR(uMainIndex < (UINT)m_aSliceImage.size());

    pImage = &m_aSliceImage[uMainIndex];

    if (pImage->pTexture == NULL)
    {
        pTexture = CreateTexture(m_uTextureWidth, m_uTextureHeight, dwFlags);
        KGLOG_PROCESS_ERROR(pTexture);

        hr = D3DXFillTexture(pTexture, GenerateColorFill, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        pImage->dwFlags = dwFlags;
        pImage->pTexture = pTexture;
        pImage->pTexture->AddRef();
    }

    *pdwImageID = uIndex;

    nResult = true;
Exit0:
    SAFE_RELEASE(pTexture);

    if (!nResult)
    {
        if (uIndex != ERROR_IMAGE_INDEX)
        {
            ReleaseSliceImage(uIndex);
        }
    }
    return nResult;
}

int KG3DUITexture::CreateImage(UINT uWidth, UINT uHeight, LPDWORD pdwImageID, DWORD dwFlags)
{
    int nRetCode = false;
    int nResult = false;
    SIZE sizeImage;

    sizeImage.cx = uWidth;
    sizeImage.cy = uHeight;

    KSliceSizeArray::iterator it = std::find(m_aSliceSize.begin(), m_aSliceSize.end(), sizeImage);
    if (it == m_aSliceSize.end())
    {
        nRetCode = CreateNormalImage(uWidth, uHeight, pdwImageID, dwFlags);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = CreateSliceImage(uWidth, uHeight, pdwImageID, dwFlags);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    if (!nResult)
    {
        *pdwImageID = ERROR_IMAGE_INDEX;
    }
    return nResult;
}

int KG3DUITexture::CreateSliceImageFromFile(LPDIRECT3DTEXTURE9 pTexture, KG3DUIImageInfo ImageInfo, LPDWORD pdwImageID)
{
    int nRetCode = false;
    int nResult = false;
    HRESULT hr = E_FAIL;
    RECT rcDst;
    RECT rcSrc;
    UINT uIndex = ERROR_IMAGE_INDEX;
    UINT uMainIndex = 0;
    KSliceImage* pImage = NULL;
    LPDIRECT3DTEXTURE9 pSliceImageTexture = NULL;
    BOOL bCreateTexture = FALSE;

    KGLOG_PROCESS_ERROR(pTexture);
    KGLOG_PROCESS_ERROR(pdwImageID);

    uIndex = GetFreeSliceImageIndex(ImageInfo.uWidth, ImageInfo.uHeight);

    uMainIndex = GetSliceMainIndex(uIndex);
    KGLOG_PROCESS_ERROR(uMainIndex < (UINT)m_aSliceImage.size());

    pImage = &m_aSliceImage[uMainIndex];

    if (pImage->pTexture == NULL)
    {
        pImage->dwFlags = 0;

        pSliceImageTexture = CreateTexture(m_uTextureWidth, m_uTextureHeight, pImage->dwFlags);
        KGLOG_PROCESS_ERROR(pSliceImageTexture);

        bCreateTexture = TRUE;

        hr = D3DXFillTexture(pSliceImageTexture, GenerateColorFill, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        pImage->pTexture = pSliceImageTexture;
        pImage->pTexture->AddRef();
    }

    ASSERT(pImage->pTexture);

    rcDst = GetSliceImageRect(uIndex);

    rcSrc.left = 0;
    rcSrc.top = 0;
    rcSrc.right = rcDst.right - rcDst.left;
    rcSrc.bottom = rcDst.bottom - rcDst.top;

    nRetCode = Stretch(pImage->pTexture, pTexture, rcDst, rcSrc);
    KGLOG_PROCESS_ERROR(nRetCode);

    *pdwImageID = (DWORD)uIndex;

    nResult = true;
Exit0:
    SAFE_RELEASE(pSliceImageTexture);

    if (!nResult)
    {
        if (uIndex != ERROR_IMAGE_INDEX)
            ReleaseSliceImage(uIndex);

        if (pdwImageID)
            *pdwImageID = ERROR_IMAGE_INDEX;

        if (bCreateTexture)
            SAFE_RELEASE(pImage->pTexture);
    }
    return nResult;
}

int KG3DUITexture::CreateNormalImageFromFile(LPDIRECT3DTEXTURE9 pTexture, KG3DUIImageInfo ImageInfo, LPDWORD pdwImageID)
{
    int nResult = false;
    UINT uIndex = ERROR_IMAGE_INDEX;
    UINT uNormalIndex = 0;
    KNormalImage* pImage = NULL;

    KGLOG_PROCESS_ERROR(pTexture);
    KGLOG_PROCESS_ERROR(pdwImageID);

    uIndex = GetFreeNormalImageIndex(ImageInfo.uWidth, ImageInfo.uHeight);

    uNormalIndex = GetNormalIndex(uIndex); 
    KGLOG_PROCESS_ERROR(uNormalIndex < (UINT)m_aNormalImage.size());

    pImage = &m_aNormalImage[uNormalIndex];

    ASSERT(pImage->pTexture == NULL);

    pImage->pTexture = pTexture;
    pImage->pTexture->AddRef();

    *pdwImageID = (DWORD)uIndex;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (uIndex != ERROR_IMAGE_INDEX)
            ReleaseNormalImage(uIndex);

        if (pdwImageID)
            *pdwImageID = ERROR_IMAGE_INDEX;
    }
    return nResult;
}

int KG3DUITexture::CreateImageFromFile(LPCTSTR szFilePath, LPDWORD pdwImageID)
{
    int nRetCode = false;
    int nResult = false;
    KG3DUIImageInfo ImageInfo;
    LPDIRECT3DTEXTURE9 pTexture = NULL;
    BOOL bSlice = FALSE;

    KGLOG_PROCESS_ERROR(pdwImageID);

    pTexture = LoadTexture(szFilePath, ImageInfo, bSlice);
    KGLOG_PROCESS_ERROR(pTexture);

    if (bSlice)
    {
        nRetCode = CreateSliceImageFromFile(pTexture, ImageInfo, pdwImageID);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = CreateNormalImageFromFile(pTexture, ImageInfo, pdwImageID);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    SAFE_RELEASE(pTexture);
    if (!nResult)
    {
        if (pdwImageID)
        {
            *pdwImageID = ERROR_IMAGE_INDEX;
        }

    }
    return nResult;
}

RECT KG3DUITexture::GetSliceImageRect(UINT uIndex) const
{
    int nResult = false;
    UINT uMainIndex = 0;
    UINT uSubIndex = 0;
    KSliceImage const* pImage = NULL;
    UINT uWidth = 0;
    UINT uHeight = 0;
    UINT uPadWidth = 0;
    UINT uPadHeight = 0;
    UINT uColumnCount = 0;
    UINT uColumn = 0;
    UINT uRow = 0;
    LONG lLeft = 0;
    LONG lTop = 0;
    RECT rcImage = { 0 };

    uMainIndex = GetSliceMainIndex(uIndex);
    KGLOG_PROCESS_ERROR(uMainIndex < (UINT)m_aSliceImage.size());

    pImage = &m_aSliceImage[uMainIndex];

    uSubIndex = GetSliceSubIndex(uIndex);

    uWidth = pImage->uSubWidth;
    uHeight = pImage->uSubHeight;
    uPadWidth = GeneratePadding(uWidth);
    uPadHeight = GeneratePadding(uHeight);

    uColumnCount = m_uTextureWidth / uPadWidth;
    uColumn = uSubIndex % uColumnCount;
    uRow = uSubIndex / uColumnCount;

    lLeft = uColumn * uPadWidth;
    lTop = uRow * uPadHeight;

    rcImage.left = lLeft;
    rcImage.top = lTop;
    rcImage.right = lLeft + uWidth;
    rcImage.bottom = lTop + uHeight;

    nResult = true;
Exit0:
    if (!nResult)
    {
        memset(&rcImage, 0, sizeof(rcImage));
    }
    return rcImage;
}

UINT KG3DUITexture::DuplicateNormalImage(UINT uIndex, DWORD dwFlags)
{
    int nRetCode = false;
    int nResult = false;
    DWORD dwNewIndex = ERROR_IMAGE_INDEX;
    UINT uNormalIndex = 0;
    UINT uNewNormalIndex = 0;
    KNormalImage* pImage = NULL;
    KNormalImage* pNewImage = NULL;
    RECT rcImage = { 0 };

    uNormalIndex = GetNormalIndex(uIndex);
    KGLOG_PROCESS_ERROR(uNormalIndex < (UINT)m_aNormalImage.size());

    pImage = &m_aNormalImage[uNormalIndex];

    nRetCode = CreateImage(pImage->uWidth, pImage->uHeight, &dwNewIndex, dwFlags);
    KGLOG_PROCESS_ERROR(nRetCode);

    uNewNormalIndex = GetNormalIndex(dwNewIndex);
    KGLOG_PROCESS_ERROR(uNewNormalIndex < (UINT)m_aNormalImage.size());

    pNewImage = &m_aNormalImage[uNewNormalIndex];

    rcImage.left = 0;
    rcImage.top = 0;
    rcImage.right = pNewImage->uWidth;
    rcImage.bottom = pNewImage->uHeight;

    ASSERT(pImage->pTexture);
    ASSERT(pNewImage->pTexture);

    nRetCode = Stretch(pNewImage->pTexture, pImage->pTexture, rcImage, rcImage);
    KGLOG_PROCESS_ERROR(nRetCode);

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (dwNewIndex != ERROR_IMAGE_INDEX)
        {
            ReleaseImage(dwNewIndex);
            dwNewIndex = ERROR_IMAGE_INDEX;
        }
    }
    return dwNewIndex;
}


UINT KG3DUITexture::DuplicateSliceImage(UINT uIndex, DWORD dwFlags)
{
    int nRetCode = false;
    int nResult = false;
    DWORD dwNewIndex = ERROR_IMAGE_INDEX;
    UINT uSliceMainIndex = 0;
    UINT uNewSliceMainIndex = 0;
    KSliceImage* pImage = NULL;
    KSliceImage* pNewImage = NULL;
    RECT rcSrc;
    RECT rcDst;

    uSliceMainIndex = GetSliceMainIndex(uIndex);
    KGLOG_PROCESS_ERROR(uSliceMainIndex < (UINT)m_aSliceImage.size());

    pImage = &m_aSliceImage[uSliceMainIndex];

    nRetCode = CreateImage(pImage->uSubWidth, pImage->uSubHeight, &dwNewIndex, dwFlags);
    KGLOG_PROCESS_ERROR(nRetCode);

    uNewSliceMainIndex = GetSliceMainIndex(dwNewIndex);
    KGLOG_PROCESS_ERROR(uNewSliceMainIndex < (UINT)m_aSliceImage.size());

    pNewImage = &m_aSliceImage[uNewSliceMainIndex];

    ASSERT(pImage->pTexture);
    ASSERT(pNewImage->pTexture);

    rcSrc = GetSliceImageRect(uIndex);
    rcDst = GetSliceImageRect(dwNewIndex);

    nRetCode = Stretch(pNewImage->pTexture, pImage->pTexture, rcDst, rcSrc);
    KGLOG_PROCESS_ERROR(nRetCode);

    pNewImage->dwFlags = dwFlags;

    nResult = true;
Exit0:
    if (!nResult)
    {
        if (dwNewIndex != ERROR_IMAGE_INDEX)
        {
            ReleaseImage(dwNewIndex);
            dwNewIndex = ERROR_IMAGE_INDEX;
        }	
    }
    return dwNewIndex;
}


int KG3DUITexture::DuplicateImage(DWORD dwImageID, DWORD dwFlags, LPDWORD pdwNewImageID)
{
    int nResult = false;

    KGLOG_PROCESS_ERROR(pdwNewImageID);

    switch (GetImageType(dwImageID))
    {
    case IMAGE_TYPE_NONE:
        *pdwNewImageID = ERROR_IMAGE_INDEX;
        ASSERT(0);
        break;
    case IMAGE_TYPE_NORMAL:
        *pdwNewImageID = DuplicateNormalImage(dwImageID, dwFlags);
        break;
    case IMAGE_TYPE_SLICE:
        *pdwNewImageID = DuplicateSliceImage(dwImageID, dwFlags);
        break;
    default:
        *pdwNewImageID = ERROR_IMAGE_INDEX;
        ASSERT(0);
        break;
    };

    KGLOG_PROCESS_ERROR(GetImageType(*pdwNewImageID) != IMAGE_TYPE_NONE);

    nResult = true;
Exit0:
    return nResult;
}

int KG3DUITexture::GrayImage(DWORD dwImageID)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    LPDIRECT3DSURFACE9 pSrcSurface = NULL;
    LPDIRECT3DSURFACE9 pDstSurface = NULL;
    LPDIRECT3DTEXTURE9 pSrcTexture = NULL;
    KG3DUIImageInfo SrcInfo;
    KG3DUIImageRect SrcRect;
    D3DLOCKED_RECT rcLock;
    D3DSURFACE_DESC DescSrc;
    RECT rcSrc;
    RECT rcDst;

    ASSERT(m_pD3DDevice);

    pSrcTexture = GetImageTexture(dwImageID, SrcRect, NULL);
    KG_PROCESS_ERROR(pSrcTexture);

    GetImageInfo(dwImageID, SrcInfo);

    hr = pSrcTexture->GetSurfaceLevel(0, &pSrcSurface);
    KGLOG_COM_PROCESS_ERROR(hr);

    ASSERT(pSrcSurface);

    hr = pSrcSurface->GetDesc(&DescSrc);
    KGLOG_COM_PROCESS_ERROR(hr);

    KGLOG_PROCESS_ERROR(m_TextureFormat == D3DFMT_A8R8G8B8 || m_TextureFormat == D3DFMT_A4R4G4B4);

    rcSrc.left = SrcInfo.uLeft;
    rcSrc.right = SrcInfo.uLeft + SrcInfo.uWidth;
    rcSrc.top = SrcInfo.uTop;
    rcSrc.bottom = SrcInfo.uTop + SrcInfo.uHeight;

    rcDst.left = 0;
    rcDst.right = SrcInfo.uWidth;
    rcDst.top = 0;
    rcDst.bottom = SrcInfo.uHeight;

    if (DescSrc.Pool == D3DPOOL_DEFAULT)
    {
        hr = m_pD3DDevice->CreateOffscreenPlainSurface(DescSrc.Width, DescSrc.Height, m_TextureFormat, D3DPOOL_SYSTEMMEM, &pDstSurface, NULL);
        KGLOG_COM_PROCESS_ERROR(hr);

        ASSERT(pDstSurface);

        hr = m_pD3DDevice->GetRenderTargetData(pSrcSurface, pDstSurface);
        KGLOG_COM_PROCESS_ERROR(hr);

        if (SUCCEEDED(pDstSurface->LockRect(&rcLock, NULL, 0)))
        {
            ASSERT(rcLock.pBits);

            if (m_TextureByte == 4)
            {
                ASSERT(m_TextureFormat == D3DFMT_A8R8G8B8);

                BYTE* pData = (BYTE*)(rcLock.pBits);
                for (UINT uY = 0; uY < SrcInfo.uHeight; ++uY)
                {
                    for (UINT uX = 0; uX < SrcInfo.uWidth * 4; uX += 4)
                    {
                        // R = G = B = R * 0.299 + G * 0.587 + B * 0.144
                        // R = G = B = (R * 3 + G * 6 + B) / 10
                        // INT nGray = (int)((float)pData[uX + 1] * 0.299f + (float)pData[uX + 2] * 0.587f + (float)pData[uX + 3] * 0.144f);
                        int nGray = ((int)pData[uX + 1] * 3 + (int)pData[uX + 2] * 6 + (int)pData[uX + 3]) / 10;
                        pData[uX] = pData[uX + 1] = pData[uX + 2] = nGray > 0xFF ? 0xFF : (BYTE)nGray;
                    }
                    pData += rcLock.Pitch;
                }
            }
            else
            {
                ASSERT(m_TextureFormat == D3DFMT_A4R4G4B4);

                BYTE* pData = (BYTE*)(rcLock.pBits);
                for (UINT uY = 0; uY < SrcInfo.uHeight; ++uY)
                {
                    for (UINT uX = 0; uX < SrcInfo.uWidth * 2; uX += 2)
                    {
                        // R = G = B = R * 0.299 + G * 0.587 + B * 0.144
                        // R = G = B = (R * 3 + G * 6 + B) / 10
                        // INT nGray = (int)((float)pData[uX + 1] * 0.299f + (float)pData[uX + 2] * 0.587f + (float)pData[uX + 3] * 0.144f);

                        int nR = pData[uX] & 0x0F;
                        int nG = (pData[uX + 1] & 0xF0) >> 4;
                        int nB = (pData[uX + 1] & 0x0F);
                        int nGray = (nR * 3 + nG * 6 + nB) / 10;

                        if (nGray > 0x0F)
                            nGray = 0x0F;

                        pData[uX] = (BYTE)nGray;
                        pData[uX + 1] = (BYTE)((nGray << 4) | nGray);
                    }
                    pData += rcLock.Pitch;
                }
            }

            pDstSurface->UnlockRect();
        }

        hr = m_pD3DDevice->UpdateSurface(pDstSurface, &rcDst, pSrcSurface, (POINT*)&rcSrc);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else
    {
        if (SUCCEEDED(pSrcSurface->LockRect(&rcLock, NULL, 0)))
        {
            ASSERT(rcLock.pBits);
            ASSERT(m_TextureFormat == D3DFMT_A8R8G8B8);

            BYTE* pData = (BYTE*)(rcLock.pBits);
            for (UINT uY = 0; uY < SrcInfo.uHeight; ++uY)
            {
                for (UINT uX = 0; uX < SrcInfo.uWidth * 4; uX += 4)
                {
                    // R = G = B = R * 0.299 + G * 0.587 + B * 0.144
                    // R = G = B = (R * 3 + G * 6 + B) / 10
                    // INT nGray = (int)((float)pData[uX + 1] * 0.299f + (float)pData[uX + 2] * 0.587f + (float)pData[uX + 3] * 0.144f);
                    INT nGray = ((int)pData[uX + 1] * 3 + (int)pData[uX + 2] * 6 + (int)pData[uX + 3]) / 10;
                    pData[uX + 0] = pData[uX + 1] = pData[uX + 2] = nGray > 0xFF ? 0xFF : (BYTE)nGray;
                }
                pData += rcLock.Pitch;
            }

            pSrcSurface->UnlockRect();
        }
    }

    nResult = true;
Exit0:
    SAFE_RELEASE(pSrcSurface);
    SAFE_RELEASE(pDstSurface);
    return nResult;
}


int KG3DUITexture::DrawUserdataToImage(DWORD dwImageID, KG3DUIDrawUserdata DrawUserdata, void* pUserdata)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    LPDIRECT3DTEXTURE9 pTexture = NULL;
    KG3DUIImageRect ImageRect;
    KG3DUIImageInfo Info;
    D3DLOCKED_RECT LockedRect;
    D3DSURFACE_DESC Desc;
    RECT rect;
    BOOL bLocked = FALSE;

    ASSERT(m_pD3DDevice);

    pTexture = GetImageTexture(dwImageID, ImageRect, NULL);
    KG_PROCESS_ERROR(pTexture);

    GetImageInfo(dwImageID, Info);

    rect.left = Info.uLeft;
    rect.right = Info.uLeft + Info.uWidth;
    rect.top = Info.uTop;
    rect.bottom = Info.uTop + Info.uHeight;

    hr = pTexture->GetLevelDesc(0, &Desc);
    KGLOG_COM_PROCESS_ERROR(hr);

    KGLOG_PROCESS_ERROR(Desc.Pool == D3DPOOL_MANAGED || Desc.Pool == D3DPOOL_SYSTEMMEM);

    hr = pTexture->LockRect(0, &LockedRect, &rect, 0);
    KGLOG_COM_PROCESS_ERROR(hr);

    bLocked = TRUE;

    DrawUserdata((PBYTE)LockedRect.pBits, LockedRect.Pitch, Info.uWidth, Info.uHeight, m_TextureFormat, pUserdata);

    nResult = true;
Exit0:
    if (bLocked)
    {
        hr = pTexture->UnlockRect(0);
        KGLOG_COM_CHECK_ERROR(hr);
    }

    return nResult;
}


int KG3DUITexture::GetImageData(DWORD dwImageID, UINT uWidth, UINT uHeight, BYTE* pData, UINT uLength, BOOL bUpToDown)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    LPDIRECT3DSURFACE9 pSrcSurface = NULL;
    LPDIRECT3DSURFACE9 pDstSurface = NULL;
    LPDIRECT3DTEXTURE9 pSrcTexture = NULL;
    KG3DUIImageInfo SrcInfo;
    KG3DUIImageRect SrcRect;
    D3DLOCKED_RECT rcLock;
    RECT rcSrc;
    RECT rcDst;
    BOOL bLockDstSurface = FALSE;

    ASSERT(pData);
    ASSERT(m_pD3DDevice);
    ASSERT(uWidth * uHeight * m_TextureByte == uLength);

    pSrcTexture = GetImageTexture(dwImageID, SrcRect, NULL);
    KG_PROCESS_ERROR(pSrcTexture);

    GetImageInfo(dwImageID, SrcInfo);

    KGLOG_PROCESS_ERROR(uWidth == SrcInfo.uWidth);
    KGLOG_PROCESS_ERROR(uHeight == SrcInfo.uHeight);

    hr = pSrcTexture->GetSurfaceLevel(0, &pSrcSurface);
    KGLOG_COM_PROCESS_ERROR(hr);

    ASSERT(pSrcSurface);

    hr = m_pD3DDevice->CreateOffscreenPlainSurface(uWidth, uHeight, m_TextureFormat, D3DPOOL_DEFAULT, &pDstSurface, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    ASSERT(pDstSurface);

    rcSrc.left = SrcInfo.uLeft;
    rcSrc.right = SrcInfo.uLeft + SrcInfo.uWidth;
    rcSrc.top = SrcInfo.uTop;
    rcSrc.bottom = SrcInfo.uTop + SrcInfo.uHeight;

    rcDst.left = 0;
    rcDst.right = SrcInfo.uWidth;
    rcDst.top = 0;
    rcDst.bottom = SrcInfo.uHeight;

    hr = m_pD3DDevice->UpdateSurface(pSrcSurface, &rcSrc, pDstSurface, (POINT*)&rcDst);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = pDstSurface->LockRect(&rcLock, NULL, D3DLOCK_READONLY);
    KGLOG_COM_PROCESS_ERROR(hr);
    KGLOG_PROCESS_ERROR(rcLock.pBits);

    bLockDstSurface = TRUE;

    if (bUpToDown)
    {
        BYTE* pDataSrc = (BYTE*)(rcLock.pBits);
        for (UINT uY = 0; uY < SrcInfo.uHeight; ++uY)
        {
            memcpy(pData, pDataSrc, rcLock.Pitch);
            pDataSrc += rcLock.Pitch;
            pData += rcLock.Pitch;
        }
    }
    else
    {
        BYTE* pDataSrc = (BYTE*)(rcLock.pBits);
        pData += uLength;
        for (UINT uY = 0; uY < SrcInfo.uHeight; ++uY)
        {
            pData -= rcLock.Pitch;
            memcpy(pData, pDataSrc, rcLock.Pitch);
            pDataSrc += rcLock.Pitch;
        }
    }

    nResult = true;
Exit0:
    if (bLockDstSurface)
    {
        hr = pDstSurface->UnlockRect();
        KGLOG_COM_CHECK_ERROR(hr);
    }

    SAFE_RELEASE(pSrcSurface);
    SAFE_RELEASE(pDstSurface);
    return nResult;
}


UINT KG3DUITexture::GetMemoryUsage() const
{
    UINT uTotalSize = 0;

    for (size_t nIndex = 0; nIndex < m_aNormalImage.size(); ++nIndex)
    {
        KNormalImage const& Image = m_aNormalImage[nIndex];
        if (Image.pTexture == NULL)
            continue;
        uTotalSize += (UINT)(Image.uWidth * Image.uHeight * m_TextureByte);
    }

    for (size_t nIndex = 0; nIndex < m_aSliceImage.size(); ++nIndex)
    {
        KSliceImage const& Image = m_aSliceImage[nIndex];
        if (Image.pTexture == NULL)
            continue;
        uTotalSize += (UINT)(m_uTextureWidth * m_uTextureHeight * m_TextureByte);
    }

    return uTotalSize;
}

UINT KG3DUITexture::GetTextureUsage() const
{
    return (UINT)(m_aNormalImage.size() + m_aSliceImage.size());
}


UINT KG3DUITexture::GetSliceUsage() const
{
    size_t uTotalSize = 0;

    for (size_t nIndex = 0; nIndex < m_aSliceImage.size(); ++nIndex)
    {
        KSliceImage const& Image = m_aSliceImage[nIndex];
        if (Image.pTexture == NULL)
            continue;

        size_t uSubImageMemory = (size_t)(GeneratePadding(Image.uSubWidth) * GeneratePadding(Image.uSubHeight) * m_TextureByte);

        for (size_t nUsed = 0; nUsed < Image.aUsed.size(); ++nUsed)
        {
            if (!Image.aUsed[nUsed])
                continue;
            uTotalSize += uSubImageMemory;
        }
    }

    return (UINT)uTotalSize;
}


void KG3DUITexture::GetImageInfo(DWORD dwImageID, KG3DUIImageInfo& Info)
{
    switch (GetImageType(dwImageID))
    {
    case IMAGE_TYPE_NONE:
        Info.uLeft = 0;
        Info.uTop = 0;
        Info.uWidth = 0;
        Info.uHeight = 0;
        break;
    case IMAGE_TYPE_NORMAL:
        if (GetNormalIndex(dwImageID) < (UINT)m_aNormalImage.size())
        {
            KNormalImage& Image = m_aNormalImage[GetNormalIndex(dwImageID)];
            Info.uLeft = 0;
            Info.uTop = 0;
            Info.uWidth = Image.uWidth;
            Info.uHeight = Image.uHeight;
        }
        break;
    case IMAGE_TYPE_SLICE:
        if (GetSliceMainIndex(dwImageID) < (UINT)m_aSliceImage.size())
        {
            RECT rcImage = GetSliceImageRect(dwImageID);
            Info.uLeft = rcImage.left;
            Info.uTop = rcImage.top;
            Info.uWidth = rcImage.right - rcImage.left;
            Info.uHeight = rcImage.bottom - rcImage.top;
        }
        break;
    default:
        Info.uLeft = 0;
        Info.uTop = 0;
        Info.uWidth = 0;
        Info.uHeight = 0;

        ASSERT(0);
        break;
    };
}


LPDIRECT3DTEXTURE9 KG3DUITexture::GetImageTexture(DWORD dwImageID, KG3DUIImageRect& Rect, D3DFORMAT* pFormat)
{
    LPDIRECT3DTEXTURE9 pTexture = NULL;

    switch (GetImageType(dwImageID))
    {
    case IMAGE_TYPE_NONE:
        Rect.left = 0.0f;
        Rect.top = 0.0f;
        Rect.width = 0.0f;
        Rect.height = 0.0f;
        break;
    case IMAGE_TYPE_NORMAL:
        if (GetNormalIndex(dwImageID) < m_aNormalImage.size())
        {
            KNormalImage& Image = m_aNormalImage[GetNormalIndex(dwImageID)];

            pTexture = Image.pTexture;

            Rect.left = 0.0f;
            Rect.top = 0.0f;
            Rect.width = 1.0f;
            Rect.height = 1.0f;
        }
        break;
    case IMAGE_TYPE_SLICE:
        if (GetSliceMainIndex(dwImageID) < m_aSliceImage.size())
        {
            KSliceImage& Image = m_aSliceImage[GetSliceMainIndex(dwImageID)];

            RECT rcImage = GetSliceImageRect(dwImageID);

            pTexture = Image.pTexture;

            Rect.left = (float)rcImage.left / m_uTextureWidth;
            Rect.top = (float)rcImage.top / m_uTextureHeight;
            Rect.width = (float)rcImage.right / m_uTextureWidth - Rect.left;
            Rect.height = (float)rcImage.bottom / m_uTextureHeight - Rect.top;
        }
        break;
    default:
        Rect.left = 0.0f;
        Rect.top = 0.0f;
        Rect.width = 0.0f;
        Rect.height = 0.0f;

        ASSERT(0);
        break;
    };

    if (pFormat)
        *pFormat = m_TextureFormat;
    return pTexture;
}

HRESULT KG3DUITexture::CheckD3DTextureCaps(LPDIRECT3DDEVICE9 pD3DDevice)
{
    static UINT const TEX_USAGE = 1024 * 1024 * 16;

    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    D3DCAPS9 d3dCaps;

    ASSERT(m_uTextureWidth > 0);
    ASSERT(m_uTextureHeight > 0);

    hr = m_pD3DDevice->GetDeviceCaps(&d3dCaps);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_uTextureWidth = d3dCaps.MaxTextureWidth;
    m_uTextureHeight = d3dCaps.MaxTextureHeight;

    if (m_pD3DDevice->GetAvailableTextureMem() >= TEX_USAGE)
    {
        m_uTextureWidth = min((UINT)512, m_uTextureWidth);
        m_uTextureHeight = min((UINT)512, m_uTextureHeight);
    }
    else
    {
        m_uTextureWidth = min((UINT)256, m_uTextureWidth);
        m_uTextureHeight = min((UINT)256, m_uTextureHeight);
    }

    KG_ASSERT_EXIT(m_uTextureHeight);

    if (d3dCaps.MaxTextureAspectRatio > 0 && d3dCaps.MaxTextureAspectRatio < (m_uTextureWidth / m_uTextureHeight))
    {
        m_uTextureHeight = GeneratePowerOf2(m_uTextureWidth / d3dCaps.MaxTextureAspectRatio);

        KG_ASSERT_EXIT(m_uTextureHeight);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DUITexture::CheckD3DTextureFormat(LPDIRECT3DDEVICE9 pD3DDevice)
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;
    D3DFORMAT d3dFmt[] = { D3DFMT_A4R4G4B4, D3DFMT_A8R8G8B8 };
    INT d3dBits[] = { 2, 4 };

    // get d3d9 & display mode
    D3DDISPLAYMODE AdapterFormat;
    LPDIRECT3D9 pD3D9 = NULL;

    hr = pD3DDevice->GetDirect3D(&pD3D9);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = pD3DDevice->GetDisplayMode(0, &AdapterFormat);
    KGLOG_COM_PROCESS_ERROR(hr);

    m_TextureFormat = d3dFmt[0];
    m_TextureByte = d3dBits[0];

    for (int i = _countof(d3dFmt) - 1; i >= 0 ; --i)
    {
        if (SUCCEEDED(pD3D9->CheckDeviceFormat(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, AdapterFormat.Format, 0, D3DRTYPE_TEXTURE, d3dFmt[i])))
        {
            m_TextureFormat = d3dFmt[i];
            m_TextureByte = d3dBits[i];
            break;
        }
    }

    hrResult = S_OK;
Exit0:
    SAFE_RELEASE(pD3D9);
    return hrResult;
}


LPDIRECT3DTEXTURE9 KG3DUITexture::CreateTexture(UINT uWidth, UINT uHeight, DWORD dwFlags)
{
    HRESULT hr = E_FAIL;
    LPDIRECT3DTEXTURE9 pTexture = NULL;
    D3DPOOL Pool = D3DPOOL_MANAGED;
    DWORD dwUsage = 0;
    D3DFORMAT TextureFormat = m_TextureFormat;

    if (dwFlags & KG3DUI_IMAGE_FLAGS_POOL_DEFAULT)
    {
        dwUsage = D3DUSAGE_RENDERTARGET;
        Pool = D3DPOOL_DEFAULT;
    }

    if (dwFlags & KG3DUI_IMAGE_FLAGS_HDC)
    {
        TextureFormat = D3DFMT_X8R8G8B8;
    }

    hr = D3DXCreateTexture(m_pD3DDevice, uWidth, uHeight, 1, dwUsage, TextureFormat, Pool, &pTexture);
    KGLOG_COM_PROCESS_ERROR(hr);

Exit0:
    if (FAILED(hr))
    {
        if (hr == D3DERR_OUTOFVIDEOMEMORY)
            KGLogPrintf(KGLOG_ERR, "CreateTexture(%d, %d) out of video memory\n", uWidth, uHeight);
        else
            KGLogPrintf(KGLOG_ERR, "CreateTexture(%d, %d) %u %u\n", uWidth, uHeight, ::GetLastError(), hr);
    }
    return pTexture;
}


void KG3DUITexture::RegisterSliceSize(UINT uWidth, UINT uHeight)
{
    SIZE sizeImage = { uWidth, uHeight };

    ASSERT(std::find(m_aSliceSize.begin(), m_aSliceSize.end(), sizeImage) == m_aSliceSize.end());

    m_aSliceSize.push_back(sizeImage);
}


LPDIRECT3DTEXTURE9 KG3DUITexture::LoadTexture(LPCTSTR szFilePath, KG3DUIImageInfo& Info, BOOL& bSlice)
{
    BOOL bRetCode = FALSE;
    HRESULT hr = E_FAIL;
    LPDIRECT3DTEXTURE9 pTexture = NULL;
    TCHAR szTempFileName[MAX_PATH];
    DWORD dwSize = 0;
    DWORD dwSizeReaded = 0;
    IFile* pFile = NULL;
    BYTE* pBuffer = NULL;
    D3DXIMAGE_INFO ImageInfo;
    SIZE sizeImage;

    KGLOG_PROCESS_ERROR(m_pD3DDevice);

    ASSERT(szFilePath && szFilePath[0] != _T('\0'));

    _tcsncpy(szTempFileName, szFilePath, _countof(szTempFileName)); 

    {
        int nFileNameLength = (int)_tcslen(szTempFileName);
        if (nFileNameLength > 3 && _tcsicmp(szTempFileName + nFileNameLength - 3 - 1, _T(".tga")) == 0)
        {
            szTempFileName[nFileNameLength - 3] = _T('\0');
            _tcscat(szTempFileName, _T("dds"));

            if (!g_IsFileExist(szTempFileName))
            {
                szTempFileName[nFileNameLength - 3] = _T('\0');
                _tcscat(szTempFileName, _T("tga"));
            }
        }

        for (int nIndex = 0; nIndex < nFileNameLength; ++nIndex)
        {
            if (szTempFileName[nIndex] == _T('/'))
            {
                szTempFileName[nIndex] = _T('\\');
            }
        }
    }

    szFilePath = szTempFileName;


    bRetCode = g_IsFileExist(szFilePath);
    KGLOG_PROCESS_ERROR(bRetCode);

    pFile = g_OpenFile(szFilePath);
    KGLOG_PROCESS_ERROR(pFile);

    dwSize = pFile->Size();
    KGLOG_PROCESS_ERROR(dwSize > 0);

    pBuffer = (PBYTE)malloc(dwSize);
    KGLOG_PROCESS_ERROR(pBuffer);

    dwSizeReaded = pFile->Read(pBuffer, dwSize);
    KGLOG_PROCESS_ERROR(dwSizeReaded == dwSize);

    hr = D3DXGetImageInfoFromFileInMemory(pBuffer, dwSize, &ImageInfo);
    KGLOG_COM_PROCESS_ERROR(hr);

    sizeImage.cx = ImageInfo.Width;
    sizeImage.cy = ImageInfo.Height;

    bSlice = std::find(m_aSliceSize.begin(), m_aSliceSize.end(), sizeImage) != m_aSliceSize.end();

    hr = D3DXCreateTextureFromFileInMemoryEx(m_pD3DDevice, pBuffer, dwSize, ImageInfo.Width, ImageInfo.Height,
        1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, D3DX_DEFAULT, D3DX_DEFAULT, 0, NULL, NULL, &pTexture);
    KGLOG_COM_PROCESS_ERROR(hr);

    Info.uLeft = 0;
    Info.uTop = 0;
    Info.uWidth = ImageInfo.Width;
    Info.uHeight = ImageInfo.Height;
Exit0:

    KG_COM_RELEASE(pFile);
    free(pBuffer);

    if (pTexture == NULL)
    {
        KGLogPrintf(KGLOG_ERR, "KG3DUITexture::LoadTexture(%s, %d, %d)\n", szFilePath, Info.uWidth, Info.uHeight);

        Info.uWidth = 0;
        Info.uHeight = 0;
    }

    return pTexture;
}


int KG3DUITexture::StretchVRamToRam(LPDIRECT3DSURFACE9 pDstSurface, LPDIRECT3DSURFACE9 pSrcSurface, 
                                    RECT rcDst, RECT rcSrc, D3DSURFACE_DESC DescDst, D3DSURFACE_DESC DescSrc)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    LPDIRECT3DSURFACE9 pSrcOffscreenSurface = NULL;
    D3DLOCKED_RECT LockedRectDst;
    D3DLOCKED_RECT LockedRectSrc;
    BOOL bLockedDst = FALSE;
    BOOL bLockedSrc = FALSE;
    int nDstWidth = rcDst.right - rcDst.left;
    int nDstHeight = rcDst.bottom - rcDst.top;
    int nSrcWidth = rcSrc.right - rcSrc.left;
    int nSrcHeight = rcSrc.bottom - rcSrc.top;
    PBYTE pDstData = NULL;
    PBYTE pSrcData = NULL;

    ASSERT(pDstSurface);
    ASSERT(pSrcSurface);
    ASSERT(nSrcWidth > 0);
    ASSERT(nSrcHeight > 0);

    KGLOG_PROCESS_ERROR(nSrcWidth == nDstWidth);
    KGLOG_PROCESS_ERROR(nSrcHeight == nDstHeight);

    hr = pDstSurface->LockRect(&LockedRectDst, &rcDst, 0);
    KGLOG_COM_PROCESS_ERROR(hr);

    bLockedDst = TRUE;

    hr = m_pD3DDevice->CreateOffscreenPlainSurface(DescSrc.Width, DescSrc.Height, m_TextureFormat, D3DPOOL_SYSTEMMEM, &pSrcOffscreenSurface, NULL);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = m_pD3DDevice->GetRenderTargetData(pSrcSurface, pSrcOffscreenSurface);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = pSrcOffscreenSurface->LockRect(&LockedRectSrc, &rcSrc, D3DLOCK_READONLY);
    KGLOG_COM_PROCESS_ERROR(hr);

    bLockedSrc = TRUE;

    pDstData = (PBYTE)LockedRectDst.pBits;
    pSrcData = (PBYTE)LockedRectSrc.pBits;

    for (int nHeight = 0; nHeight < nDstHeight; ++nHeight)
    {
        memcpy(pDstData, pSrcData, m_TextureByte * nDstWidth);

        pDstData += LockedRectDst.Pitch;
        pSrcData += LockedRectSrc.Pitch;
    }

    nResult = true;
Exit0:
    if (bLockedSrc)
    {
        hr = pSrcOffscreenSurface->UnlockRect();
        KGLOG_COM_CHECK_ERROR(hr);
    }
    SAFE_RELEASE(pSrcOffscreenSurface);

    if (bLockedDst)
    {
        hr = pDstSurface->UnlockRect();
        KGLOG_COM_CHECK_ERROR(hr);
    }
    return nResult;
}

int KG3DUITexture::StretchRamToRam(LPDIRECT3DSURFACE9 pDstSurface, LPDIRECT3DSURFACE9 pSrcSurface, 
                                   RECT rcDst, RECT rcSrc, D3DSURFACE_DESC DescDst, D3DSURFACE_DESC DescSrc)
{
    int nResult = false;
    HRESULT hr = E_FAIL;
    D3DLOCKED_RECT LockedRectDst;
    D3DLOCKED_RECT LockedRectSrc;
    BOOL bLockedDst = FALSE;
    BOOL bLockedSrc = FALSE;
    int nDstWidth = rcDst.right - rcDst.left;
    int nDstHeight = rcDst.bottom - rcDst.top;
    int nSrcWidth = rcSrc.right - rcSrc.left;
    int nSrcHeight = rcSrc.bottom - rcSrc.top;
    PBYTE pDstData = NULL;
    PBYTE pSrcData = NULL;

    ASSERT(nSrcWidth > 0);
    ASSERT(nSrcHeight > 0);

    KGLOG_PROCESS_ERROR(nSrcWidth == nDstWidth);
    KGLOG_PROCESS_ERROR(nSrcHeight == nDstHeight);

    hr = pDstSurface->LockRect(&LockedRectDst, &rcDst, 0);
    KGLOG_COM_PROCESS_ERROR(hr);

    bLockedDst = TRUE;

    hr = pSrcSurface->LockRect(&LockedRectSrc, &rcSrc, D3DLOCK_READONLY);
    KGLOG_COM_PROCESS_ERROR(hr);

    bLockedSrc = TRUE;

    pDstData = (PBYTE)LockedRectDst.pBits;
    pSrcData = (PBYTE)LockedRectSrc.pBits;

    for (int nHeight = 0; nHeight < nDstHeight; ++nHeight)
    {
        memcpy(pDstData, pSrcData, m_TextureByte * nDstWidth);

        pDstData += LockedRectDst.Pitch;
        pSrcData += LockedRectSrc.Pitch;
    }

    nResult = true;
Exit0:
    if (bLockedSrc)
    {
        hr = pSrcSurface->UnlockRect();
        KGLOG_COM_CHECK_ERROR(hr);
    }
    if (bLockedDst)
    {
        hr = pDstSurface->UnlockRect();
        KGLOG_COM_CHECK_ERROR(hr);
    }
    return nResult;
}

int KG3DUITexture::Stretch(LPDIRECT3DTEXTURE9 pDstTexture, LPDIRECT3DTEXTURE9 pSrcTexture, RECT rcDst, RECT rcSrc)
{
    int nResult = false;
    int nRetCode = false;
    HRESULT hr = E_FAIL;
    LPDIRECT3DSURFACE9 pSrcSurface = NULL;
    LPDIRECT3DSURFACE9 pDstSurface = NULL;
    D3DSURFACE_DESC DescDst;
    D3DSURFACE_DESC DescSrc;

    ASSERT(m_pD3DDevice);

    KG_PROCESS_ERROR(pDstTexture);
    KG_PROCESS_ERROR(pSrcTexture);

    hr = pDstTexture->GetLevelDesc(0, &DescDst);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = pSrcTexture->GetLevelDesc(0, &DescSrc);
    KGLOG_COM_PROCESS_ERROR(hr);

    KGLOG_PROCESS_ERROR(DescSrc.Format == DescDst.Format);

    hr = pSrcTexture->GetSurfaceLevel(0, &pSrcSurface);
    KGLOG_COM_PROCESS_ERROR(hr);

    hr = pDstTexture->GetSurfaceLevel(0, &pDstSurface);
    KGLOG_COM_PROCESS_ERROR(hr);

    if (DescSrc.Pool == D3DPOOL_DEFAULT && DescDst.Pool == D3DPOOL_DEFAULT)
    {
        hr = m_pD3DDevice->StretchRect(pSrcSurface, &rcSrc, pDstSurface, &rcDst, D3DTEXF_NONE);
        KGLOG_COM_PROCESS_ERROR(hr);
    }
    else if (DescSrc.Pool == D3DPOOL_DEFAULT && (DescSrc.Usage & D3DUSAGE_RENDERTARGET) != 0)
    {
        nRetCode = StretchVRamToRam(pDstSurface, pSrcSurface, rcDst, rcSrc, DescDst, DescSrc);
        KGLOG_PROCESS_ERROR(nRetCode);
    }
    else
    {
        nRetCode = StretchRamToRam(pDstSurface, pSrcSurface, rcDst, rcSrc, DescDst, DescSrc);
        KGLOG_PROCESS_ERROR(nRetCode);
    }

    nResult = true;
Exit0:
    SAFE_RELEASE(pSrcSurface);
    SAFE_RELEASE(pDstSurface);
    return nResult;
}


