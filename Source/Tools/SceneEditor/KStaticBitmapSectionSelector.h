#pragma once

class KStaticBitmapSectionSelector
{
#pragma pack(push,1)
	struct TGAHeader 
	{
		unsigned char   Id ;
		unsigned char   ColourMapType ;
		unsigned char   ImageType ;
		unsigned short  ColourMapOrigin ;
		unsigned short  ColourMapLength ;
		unsigned char   ColourMapDepth ;
		unsigned short  x_origin ;
		unsigned short  y_origin ;
		unsigned short  width ;
		unsigned short  height ;
		unsigned char   bitsperpixel ;
		unsigned char   imagedescriptor ;
	};
#pragma pack(pop)

	struct BitmapBuffer
	{
		BitmapBuffer();
		~BitmapBuffer();
		BYTE* pData;
		DWORD dwSize;
	};

public:
	KStaticBitmapSectionSelector(void);
	~KStaticBitmapSectionSelector(void);
	void SetStaticControl(CStatic* pStatic);
	void SetSection(int nX, int nY);
	void UpdateControl();
	void OnSelect(POINT* pSelection, 
		int nNumSelection, 
		BOOL bClear);
	void SetBitmapData(BYTE* pData, DWORD dwSize);
	void GetSelectSection(int** ppSelected, int& nNum);
	HRESULT LoadTga(LPCSTR strFileName,BOOL bAlpha = TRUE);
	void SetSelected(BYTE* pSelected, int nNum);
private:
	void BlendSrcSelectionMask(int nX, int nY);
    void ConvertToIndex(const POINT& pt, int& nX, int& nY);
	void GetStaticControlDimension(int& nWidth, int& nHeight);
	void GetRectByIndex(int nX, int nY, RECT& rect);
	DWORD    m_dwSelection;
	CBitmap  m_Bitmap;
	int      m_nSectionInX;
	int      m_nSectionInY;
	BOOL     m_bTgaLoaded;
	CStatic* m_pStaticControl;
	BitmapBuffer m_BitmapBuffer;
	BitmapBuffer m_BitmapOrg;
	std::vector<BOOL> m_SelectedSection;
};
