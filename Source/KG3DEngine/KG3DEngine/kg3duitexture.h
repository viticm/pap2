////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DUITexture.h
//  Version     : 1.0
//  Creator     : dengchao
//  Create Date : 2006-08-28 16:43:32
//  Comment     : font def
//
////////////////////////////////////////////////////////////////////////////////


#ifndef _INCLUDE_KG3DUITEXTURE_H_
#define _INCLUDE_KG3DUITEXTURE_H_

#include "../../../Include/KG3DEngine/KG3DInterface.h"

struct KG3DUIImageInfo
{
	UINT uLeft;
	UINT uTop;
	UINT uWidth;
	UINT uHeight;
};

inline bool operator==(SIZE const& lhs, SIZE const& rhs)
{
	return lhs.cx == rhs.cx && lhs.cy == rhs.cy;
}

class KG3DUITexture
{
public:
	enum { ERROR_IMAGE_INDEX = 0x00FFFFFF };

	enum
	{
		IMAGE_TYPE_NONE,
		IMAGE_TYPE_NORMAL,
		IMAGE_TYPE_SLICE,
		IMAGE_TYPE_COUNT,
	};

	struct KNormalImage
	{
		LPDIRECT3DTEXTURE9 pTexture;

		UINT uWidth;
		UINT uHeight;

        DWORD dwFlags;
	};

	typedef std::vector<bool> KUsedSubImage;

	struct KSliceImage
	{
		LPDIRECT3DTEXTURE9 pTexture;

		UINT uSubWidth;
		UINT uSubHeight;

        DWORD dwFlags;

		KUsedSubImage aUsed;
	};

	typedef std::vector<SIZE>					KSliceSizeArray;
	typedef std::vector<KNormalImage>			KNormalImageArray;
	typedef std::vector<KSliceImage>			KSliceImageArray;

public:
	KG3DUITexture();
	~KG3DUITexture();

private:
	KG3DUITexture(KG3DUITexture const&);
	KG3DUITexture& operator=(KG3DUITexture const&);

public:
	HRESULT Initialize(LPDIRECT3DDEVICE9 pD3DDevice);
	HRESULT CleanUp();

	HRESULT OnLostDevice();
	HRESULT OnResetDevice();

	void RegisterSliceSize(UINT uWidth, UINT uHeight);

	int CreateImage(UINT uWidth, UINT uHeight, LPDWORD pdwImageID, DWORD dwFlags);
	int CreateImageFromFile(LPCTSTR szFilePath, LPDWORD pdwImageID);
	int DuplicateImage(DWORD dwImageID, DWORD dwFlags, LPDWORD pdwNewImageID); // 复制后的贴图都是使用Managed Pool
	int GrayImage(DWORD dwImageID);
	void ReleaseImage(DWORD dwImageID);

	UINT GetMemoryUsage() const;
	UINT GetTextureUsage() const;
	UINT GetSliceUsage() const;

	int GetImageData(DWORD dwImageID, UINT uWidth, UINT uHeight, BYTE* pData, UINT uLength, BOOL bUpToDown);
	void GetImageInfo(DWORD dwImageID, KG3DUIImageInfo& Info);

	LPDIRECT3DTEXTURE9 GetImageTexture(DWORD dwImageID, KG3DUIImageRect& Rect, D3DFORMAT* pFormat);

	int DrawUserdataToImage(DWORD dwImageID, KG3DUIDrawUserdata DrawUserdata, void* pUserdata);

private:
	HRESULT CheckD3DTextureCaps(LPDIRECT3DDEVICE9 pD3DDevice);
	HRESULT CheckD3DTextureFormat(LPDIRECT3DDEVICE9 pD3DDevice);

	BOOL Stretch(LPDIRECT3DTEXTURE9 pDstTexture, LPDIRECT3DTEXTURE9 pSrcTexture, RECT rcDst, RECT rcSrc);

	int CreateNormalImage(UINT uWidth, UINT uHeight, LPDWORD pdwImageID, DWORD dwFlags);
	int CreateSliceImage(UINT uWidth, UINT uHeight, LPDWORD pdwImageID, DWORD dwFlags);

	void ReleaseNormalImage(UINT uIndex);
	void ReleaseSliceImage(UINT uIndex);

	UINT DuplicateNormalImage(UINT uIndex, DWORD dwFlags);
	UINT DuplicateSliceImage(UINT uIndex, DWORD dwFlags);

	RECT GetSliceImageRect(UINT uIndex) const;

	UINT GetFreeNormalImageIndex(UINT uWidth, UINT uHeight);
	UINT GetFreeSliceImageIndex(UINT uWidth, UINT uHeight);

	LPDIRECT3DTEXTURE9 LoadTexture(LPCTSTR szFilePath, KG3DUIImageInfo& Info, BOOL& bSlice);
	LPDIRECT3DTEXTURE9 CreateTexture(UINT uWidth, UINT uHeight, DWORD dwFlags);

	int CreateSliceImageFromFile(LPDIRECT3DTEXTURE9 pTexture, KG3DUIImageInfo ImageInfo, LPDWORD pdwImageID);
	int CreateNormalImageFromFile(LPDIRECT3DTEXTURE9 pTexture, KG3DUIImageInfo ImageInfo, LPDWORD pdwImageID);

    int StretchVRamToRam(LPDIRECT3DSURFACE9 pDstSurface, LPDIRECT3DSURFACE9 pSrcSurface, RECT rcDst, RECT rcSrc, D3DSURFACE_DESC DescDst, D3DSURFACE_DESC DescSrc);
    int StretchRamToRam(LPDIRECT3DSURFACE9 pDstSurface, LPDIRECT3DSURFACE9 pSrcSurface, RECT rcDst, RECT rcSrc, D3DSURFACE_DESC DescDst, D3DSURFACE_DESC DescSrc);

private:
	LPDIRECT3DDEVICE9	m_pD3DDevice;
	D3DFORMAT			m_TextureFormat;
	INT					m_TextureByte;
	UINT				m_uTextureWidth;
	UINT				m_uTextureHeight;

	KSliceSizeArray		m_aSliceSize;

	KNormalImageArray	m_aNormalImage;
	KSliceImageArray	m_aSliceImage;
};

#endif // _INCLUDE_KG3DUITEXTURE_H_
