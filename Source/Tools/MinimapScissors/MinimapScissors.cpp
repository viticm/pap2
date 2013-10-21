#include <windows.h>
#include <gdiplus.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>
#include <assert.h>
#include <atlbase.h>
#include <atltrace.h>
#include <atlconv.h>

using namespace Gdiplus;

#define FILL_FLOORS 2
#define KG_PROCESS_ERROR(Condition) \
    do  \
    {   \
        if (!(Condition))   \
        goto Exit0;     \
    } while (false)
#define SAFE_DELETE(p) \
    do \
    {  \
        if (p) \
        { \
            delete[] p;   \
            p = NULL;    \
        }\
    } while (false)

enum AXIS
{
    AXIS_BEGIN = 0,
    AXIS_INVALID  = AXIS_BEGIN,
    AXIS_VERTICAL,
    AXIS_HORIZONTAL,
    AXIS_VERTICAL_HORIZONTAL,
    AXIS_COUNT,
};

BITMAPINFO bmpInfo;
BYTE * pbyDefaultData = NULL;

int FlipMap(Bitmap *pBitmap, AXIS emAxis, Bitmap **ppRetBitmap);

void InitBmpInfo(int nWidth)
{
	bmpInfo.bmiHeader.biBitCount = 32;
	bmpInfo.bmiHeader.biClrImportant = 0;
	bmpInfo.bmiHeader.biClrUsed = 0;
	bmpInfo.bmiHeader.biCompression = 0;
	bmpInfo.bmiHeader.biHeight = 0 - nWidth;   
	bmpInfo.bmiHeader.biPlanes = 1;
	bmpInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfo.bmiHeader.biSizeImage = nWidth * nWidth * sizeof(DWORD);
	bmpInfo.bmiHeader.biWidth = nWidth;
	bmpInfo.bmiHeader.biXPelsPerMeter = 2834;
	bmpInfo.bmiHeader.biYPelsPerMeter = 2834;
	bmpInfo.bmiColors[0].rgbBlue = 255;
	bmpInfo.bmiColors[0].rgbGreen = 255;
	bmpInfo.bmiColors[0].rgbRed = 255;
	bmpInfo.bmiColors[0].rgbReserved = 0;
}

int InitByDefaultData(int nWidth)
{
	if (!pbyDefaultData)
		pbyDefaultData = (BYTE *) new DWORD[nWidth * nWidth];
	if (!pbyDefaultData)
		return false;
	UINT *pStart = (UINT*)pbyDefaultData;
	UINT *pEnd = pStart + nWidth * nWidth;
	while (pStart != pEnd)
	{
		*pStart = 0xFF000000;
		pStart++;
	}
	return true;
}

int InitScissors(int nWidth)
{
	InitBmpInfo(nWidth);
	return InitByDefaultData(nWidth);
}

int GetEncoderClsid(const WCHAR* format, CLSID* pClsid)
{
	UINT  num = 0;          // number of image encoders
	UINT  size = 0;         // size of the image encoder array in bytes

	ImageCodecInfo* pImageCodecInfo = NULL;

	GetImageEncodersSize(&num, &size);
	if(size == 0)
		return -1;  // Failure

	pImageCodecInfo = (ImageCodecInfo*)(malloc(size));
	if(pImageCodecInfo == NULL)
		return -1;  // Failure

	GetImageEncoders(num, size, pImageCodecInfo);

	for(UINT j = 0; j < num; ++j)
	{
		if( wcscmp(pImageCodecInfo[j].MimeType, format) == 0 )
		{
			*pClsid = pImageCodecInfo[j].Clsid;
			free(pImageCodecInfo);
			return j;  // Success
		}    
	}

	free(pImageCodecInfo);
	return -1;  // Failure
}

int IsBmpDataNull(Bitmap* pBmp)
{
	if (!pBmp)
		return true;

	BitmapData* pBmpData = new BitmapData;
	if (!pBmpData)
		return true;
	UINT w = pBmp->GetWidth();
	UINT h = pBmp->GetHeight();
	Rect rcOrg(0, 0, w, h);
	pBmp->LockBits(&rcOrg, ImageLockModeRead, PixelFormat32bppARGB, pBmpData);
	UINT* pixels = (UINT*)pBmpData->Scan0;

	assert(pBmpData->Stride / sizeof(DWORD) == w);

	for(UINT row = 0; row < h; ++row)
	{
		for(UINT col = 0; col < w; ++col, ++pixels)
		{
			if ((*pixels) & 0x00FFFFFF)
			{
				pBmp->UnlockBits(pBmpData);
				return false;
			}
		}
	}

	pBmp->UnlockBits(pBmpData);
	return true;
}

void SaveMinimapBlock(char * szOutDir, int nLayer, UINT uX, UINT uY, Bitmap* pBmp)
{
	char szPath[MAX_PATH];
	if (szOutDir[0])
	{
		char cLast = szOutDir[strlen(szOutDir) - 1];
		if ( cLast == '\\' || cLast == '/')
			sprintf(szPath, "%s%d_%d_%d.tga", szOutDir, nLayer, uY, uX);
		else
			sprintf(szPath, "%s\\%d_%d_%d.tga", szOutDir, nLayer, uY, uX);
	}
	else
	{
		sprintf(szPath, "%d_%d_%d.tga", nLayer, uY, uX);
	}
	

	if (pBmp && !IsBmpDataNull(pBmp))
	{
		BitmapData bitmapData;
		Rect rcOrg(0, 0, pBmp->GetWidth(), pBmp->GetHeight());
		pBmp->LockBits(&rcOrg, ImageLockModeRead, PixelFormat24bppRGB, &bitmapData);

		FILE * pTgaFile = fopen(szPath, "wb");
		if (pTgaFile)
		{
			struct TGAFILEHEADER
			{
				BYTE		IDLength;					// ID length
				BYTE		ColorMapType;				// Color map type
				BYTE		ImageType;					// Image type
				BYTE		ColorMapSpec[5];			// Color map specification
				WORD		X,Y;						// Image specification
				WORD		Width,Height;
				BYTE		PixelDep;
				BYTE		Desc;
			} TgaFileHeader;

			ZeroMemory(&TgaFileHeader, sizeof(TgaFileHeader));
			TgaFileHeader.Width     = bitmapData.Width;
			TgaFileHeader.Height    = bitmapData.Height;
			TgaFileHeader.PixelDep  = 24;
			if (bitmapData.PixelFormat == PixelFormat32bppARGB)
				TgaFileHeader.PixelDep = 32;
			TgaFileHeader.ImageType = 2;
			TgaFileHeader.Desc      = 8;

			fwrite(&TgaFileHeader, sizeof(TgaFileHeader), 1, pTgaFile);
            
			for (int i = 0; i < pBmp->GetHeight(); i++) 
			{
				fwrite((BYTE *)bitmapData.Scan0 + (pBmp->GetHeight() - i - 1) * bitmapData.Stride ,  bitmapData.Width * TgaFileHeader.PixelDep / 8, 1, pTgaFile);
			}
			fclose(pTgaFile);
			pTgaFile = NULL;
		}
		pBmp->UnlockBits(&bitmapData);
	}
}

int FillMap(Bitmap *pSrcBitmap, Rect *pSrcRect, Bitmap *pDesBitmap, Rect *pDesRect, AXIS emAxis)
{
    int nResult  = false;
    int nRetCode = false;

    BitmapData *pBmpData = NULL; 
    Bitmap *pCpyBitmap = NULL;
    Bitmap *pFlipCpyBitmap = NULL;
    Rect    rect;

    KG_PROCESS_ERROR(pSrcBitmap);
    KG_PROCESS_ERROR(pSrcRect);
    KG_PROCESS_ERROR(pDesBitmap);
    KG_PROCESS_ERROR(pDesRect);
    KG_PROCESS_ERROR(emAxis >= AXIS_BEGIN && emAxis < AXIS_COUNT);

    if (emAxis != AXIS_INVALID)
    {
        pCpyBitmap = pSrcBitmap->Clone(*pSrcRect, PixelFormat32bppRGB);
        KG_PROCESS_ERROR(pCpyBitmap);
        
        nRetCode = FlipMap(pCpyBitmap, emAxis, &pFlipCpyBitmap);
        KG_PROCESS_ERROR(nRetCode);
        KG_PROCESS_ERROR(pFlipCpyBitmap);

        pBmpData = new BitmapData;
        KG_PROCESS_ERROR(pBmpData);

        rect.X = 0;
        rect.Y = 0;
        rect.Width  = pFlipCpyBitmap->GetWidth();
        rect.Height = pFlipCpyBitmap->GetHeight();
        pFlipCpyBitmap->LockBits(&rect, ImageLockModeRead, PixelFormat24bppRGB, pBmpData);
    }
    else
    {
        pBmpData = new BitmapData;
        KG_PROCESS_ERROR(pBmpData);
        pSrcBitmap->LockBits(pSrcRect, ImageLockModeRead, PixelFormat24bppRGB, pBmpData);
    }

    pDesBitmap->LockBits(
        pDesRect, 
        ImageLockModeWrite|ImageLockModeUserInputBuf, 
        PixelFormat24bppRGB,
        pBmpData
    );

    pDesBitmap->UnlockBits(pBmpData);
    pSrcBitmap->UnlockBits(pBmpData);

// Exit1:
    nResult = true;
Exit0:
    SAFE_DELETE(pBmpData);
    SAFE_DELETE(pCpyBitmap);
    SAFE_DELETE(pFlipCpyBitmap);
    return nResult;
}

int GetCompletionBitmap(Bitmap *pMinimap, int nSize, Bitmap **ppRetBitmap)
{
    int nResult  = false;
    int nRetCode = false;
    
    int nWidth      = 0;
    int nHeight     = 0;
    int nDesWidth   = 0;
    int nDesHeight  = 0;
    int nFillSize   = 0;
    int nTopDist    = 0;
    int nLeftDist   = 0;
    int nRightDist  = 0;
    int nBottomDist = 0;

    Bitmap *pNewBitmap = NULL;
    Rect    SrcRect;
    Rect    DesRect;

    KG_PROCESS_ERROR(pMinimap);
    KG_PROCESS_ERROR(ppRetBitmap);
    KG_PROCESS_ERROR(nSize > 0);

    nWidth  = pMinimap->GetWidth();
    nHeight = pMinimap->GetHeight();
    
    nFillSize = FILL_FLOORS * nSize; 

    KG_PROCESS_ERROR(nWidth >= nFillSize && nHeight >= nFillSize);

    nDesWidth  = nFillSize + nWidth + nFillSize + (nSize - nWidth % nSize) % nSize;
    nDesHeight = nFillSize + nHeight + nFillSize + (nSize - nHeight % nSize) % nSize;
    
    nTopDist    = nFillSize;
    nLeftDist   = nFillSize;
    nRightDist  = nDesWidth - nWidth - nFillSize;
    nBottomDist = nDesHeight - nHeight - nFillSize;

    pNewBitmap = new Bitmap(nDesWidth, nDesHeight, PixelFormat32bppRGB);
    KG_PROCESS_ERROR(pNewBitmap);

    // Middle
    SrcRect.X = 0;
    SrcRect.Y = 0;
    SrcRect.Width  = DesRect.Width  = nWidth;
    SrcRect.Height = DesRect.Height = nHeight;

    DesRect.X = nLeftDist;
    DesRect.Y = nBottomDist;

    nRetCode = FillMap(pMinimap, &SrcRect, pNewBitmap, &DesRect, AXIS_INVALID);
    KG_PROCESS_ERROR(nRetCode);

    // BOTTOMLEFT
    SrcRect.X = 0;
    SrcRect.Y = 0;
    SrcRect.Width = nLeftDist;
    SrcRect.Height = nBottomDist;
    DesRect = SrcRect;

    nRetCode = FillMap(pMinimap, &SrcRect, pNewBitmap, &DesRect, AXIS_VERTICAL_HORIZONTAL);
    KG_PROCESS_ERROR(nRetCode);

    // BOTTOMRIGHT
    SrcRect.X = nWidth - nRightDist;
    SrcRect.Y = 0;
    SrcRect.Width  = DesRect.Width  = nRightDist;
    SrcRect.Height = DesRect.Height = nBottomDist;

    DesRect.X = nDesWidth - nRightDist;
    DesRect.Y = 0;

    nRetCode = FillMap(pMinimap, &SrcRect, pNewBitmap, &DesRect, AXIS_VERTICAL_HORIZONTAL);
    KG_PROCESS_ERROR(nRetCode);

    // TOPLEFT
    SrcRect.X = 0;
    SrcRect.Y = nHeight - nTopDist;
    SrcRect.Width  = DesRect.Width  = nLeftDist;
    SrcRect.Height = DesRect.Height = nTopDist;

    DesRect.X = 0;
    DesRect.Y = nDesHeight - nFillSize;

    nRetCode = FillMap(pMinimap, &SrcRect, pNewBitmap, &DesRect, AXIS_VERTICAL_HORIZONTAL);
    KG_PROCESS_ERROR(nRetCode);

    // TOPRIGHT
    SrcRect.X = nWidth  - nRightDist;
    SrcRect.Y = nHeight - nTopDist;
    SrcRect.Width  = DesRect.Width  = nRightDist;
    SrcRect.Height = DesRect.Height = nTopDist;

    DesRect.X = nDesWidth  - nRightDist;
    DesRect.Y = nDesHeight - nTopDist;

    nRetCode = FillMap(pMinimap, &SrcRect, pNewBitmap, &DesRect, AXIS_VERTICAL_HORIZONTAL);
    KG_PROCESS_ERROR(nRetCode);


    // LEFT
    SrcRect.X = 0;
    SrcRect.Y = 0;
    SrcRect.Width  = DesRect.Width  = nLeftDist;
    SrcRect.Height = DesRect.Height = nHeight;

    DesRect.X = 0;
    DesRect.Y = nBottomDist;

    nRetCode = FillMap(pMinimap, &SrcRect, pNewBitmap, &DesRect, AXIS_VERTICAL);
    KG_PROCESS_ERROR(nRetCode);

    // BOTTOM
    SrcRect.X = 0;
    SrcRect.Y = 0;
    SrcRect.Width  = DesRect.Width  = nWidth;
    SrcRect.Height = DesRect.Height = nBottomDist;

    DesRect.X = nLeftDist;
    DesRect.Y = 0;

    nRetCode = FillMap(pMinimap, &SrcRect, pNewBitmap, &DesRect, AXIS_HORIZONTAL);
    KG_PROCESS_ERROR(nRetCode);

    // RIGHT
    SrcRect.X = nWidth - nRightDist;
    SrcRect.Y = 0;
    SrcRect.Width  = DesRect.Width  = nRightDist;
    SrcRect.Height = DesRect.Height = nHeight;

    DesRect.X = nDesWidth - nRightDist;
    DesRect.Y = nBottomDist;

    nRetCode = FillMap(pMinimap, &SrcRect, pNewBitmap, &DesRect, AXIS_VERTICAL);
    KG_PROCESS_ERROR(nRetCode);

    // TOP
    SrcRect.X = 0;
    SrcRect.Y = nHeight - nTopDist;
    SrcRect.Width  = DesRect.Width  = nWidth;
    SrcRect.Height = DesRect.Height = nTopDist;

    DesRect.X = nLeftDist;
    DesRect.Y = nDesHeight - nTopDist;

    nRetCode = FillMap(pMinimap, &SrcRect, pNewBitmap, &DesRect, AXIS_HORIZONTAL);
    KG_PROCESS_ERROR(nRetCode);

// Exit1:
	nResult = true;
    *ppRetBitmap = pNewBitmap;
Exit0:
	return nResult;

}

int FlipMap(Bitmap *pBitmap, AXIS emAxis, Bitmap **ppRetBitmap)
{
    int nResult  = false;
    int nRetCode = false;

    int nHeight  = 0;
    int nWidth   = 0;
    Bitmap *pNewBitmap = NULL;
    BitmapData bitmapData1;
    BitmapData bitmapData2;
    Rect rcOrg; 

    KG_PROCESS_ERROR(pBitmap);
    KG_PROCESS_ERROR(emAxis >= AXIS_BEGIN && emAxis < AXIS_COUNT);
    KG_PROCESS_ERROR(pBitmap);

    nHeight = pBitmap->GetHeight();
    nWidth = pBitmap->GetWidth();

    rcOrg.X = 0;
    rcOrg.Y = 0;
    rcOrg.Width = nWidth;
    rcOrg.Height = nHeight;

    pNewBitmap = new Bitmap(nWidth, nHeight, PixelFormat32bppRGB);
    pBitmap->LockBits(&rcOrg, ImageLockModeRead, PixelFormat24bppRGB, &bitmapData1);
    pNewBitmap->LockBits(&rcOrg, ImageLockModeWrite|ImageLockModeUserInputBuf, PixelFormat24bppRGB, &bitmapData2);
    
    bitmapData2.Scan0 = new BYTE[nHeight * bitmapData1.Stride];
    bitmapData2.Stride = bitmapData1.Stride;

    BYTE *pInData  = (BYTE *)bitmapData1.Scan0;
    BYTE *pOutData = (BYTE *)bitmapData2.Scan0;

    if (emAxis == AXIS_VERTICAL)
    {
        for (int i = 0; i < nWidth; i++)
        {
            for (int j = 0; j < nHeight; j++)
            {
                BYTE *pIn  = pInData + (nWidth - i - 1) * 3 + j * bitmapData1.Stride;
                BYTE *pOut = pOutData + i * 3 + j * bitmapData2.Stride;

                pOut[0] = pIn[0];
                pOut[1] = pIn[1];
                pOut[2] = pIn[2];
            }
        }
    }
    else if (emAxis == AXIS_HORIZONTAL)
    {
        for (int i = 0; i < nHeight; i++)
        {
            BYTE *pIn  = pInData + i * bitmapData1.Stride;
            BYTE *pOut = pOutData + (nHeight - i - 1) * bitmapData2.Stride;
            memcpy(pOut, pIn, bitmapData2.Stride);
        }
    }
    else if (emAxis == AXIS_VERTICAL_HORIZONTAL)
    {
        BYTE *byPix = new BYTE[nHeight * bitmapData1.Stride];
        
        for (int i = 0; i < nWidth; i++)
        {
            for (int j = 0; j < nHeight; j++)
            {
                BYTE *pIn  = pInData + (nWidth - i - 1) * 3 + j * bitmapData1.Stride;
                BYTE *pOut = byPix + i * 3 + j * bitmapData1.Stride;

                pOut[0] = pIn[0];
                pOut[1] = pIn[1];
                pOut[2] = pIn[2];
            }
        }

        for (int i = 0; i < nHeight; i++)
        {
            BYTE *pIn  = byPix + i * bitmapData1.Stride;
            BYTE *pOut = pOutData + (nHeight - i - 1) * bitmapData2.Stride;
            memcpy(pOut, pIn, bitmapData2.Stride);
        }
    }

    pBitmap->UnlockBits(&bitmapData1);
    pNewBitmap->UnlockBits(&bitmapData2);

// Exit1:
    nResult = true;
    *ppRetBitmap = pNewBitmap;
Exit0:
    if (bitmapData2.Scan0)
    {
        delete[] bitmapData2.Scan0;
        bitmapData2.Scan0 = NULL;
    }
    return nResult;
}

void SaveMinimapSideBlock(char * szOutDir, int nLayer, int nWidth, UINT uX, UINT uY, Bitmap* pBmp)
{
	if (pBmp)
	{
		Bitmap* pNewBmp = Bitmap::FromBITMAPINFO(&bmpInfo, pbyDefaultData);
		if (pNewBmp)
		{
			BitmapData* pBmpData = new BitmapData;
			if (pBmpData)
			{
				InitByDefaultData(nWidth);

				Rect rcOrg(0, 0, pBmp->GetWidth(), pBmp->GetHeight());
				pBmp->LockBits(&rcOrg, ImageLockModeRead, PixelFormat32bppARGB, pBmpData);
				Rect rc(0, nWidth - pBmp->GetHeight(), pBmp->GetWidth(), pBmp->GetHeight());
				pNewBmp->LockBits(&rc, ImageLockModeWrite|ImageLockModeUserInputBuf, PixelFormat32bppARGB, pBmpData);
				
				pNewBmp->UnlockBits(pBmpData);
				pBmp->UnlockBits(pBmpData);

				delete pBmpData;
				pBmpData = NULL;
			}

			SaveMinimapBlock(szOutDir, nLayer, uX, uY, pNewBmp);
			delete pNewBmp;
			pNewBmp = NULL;
		}
	}
}

void ScissorMinimap(char * szFile, char * szOutDir, int nLayer, int nWidth, BOOL bFillEdge)
{
    int nRetCode = false;
	printf("%s", "处理小地图文件");
	printf("%s", szFile);

	Bitmap* pMinimap = new Bitmap(ATL::CA2W(szFile));
	if (pMinimap)
	{
        if (bFillEdge)
        {
            if ((FILL_FLOORS * nWidth) > pMinimap->GetWidth() || (FILL_FLOORS * nWidth) > pMinimap->GetHeight())
                return;

            Bitmap *pNewBit2 = NULL;
            nRetCode = GetCompletionBitmap(pMinimap, nWidth, &pNewBit2);
            if (!nRetCode || !pNewBit2)
                return;

            int nBitHeight = pNewBit2->GetHeight();
            int nRow = pNewBit2->GetWidth() / nWidth;
            int nColomn = nBitHeight / nWidth;
            int nRowID = - FILL_FLOORS;
            int nColomnID = - FILL_FLOORS;

            for (int i = 0; i < nRow; i++)
            {
                int nID = nColomnID;
                for (int j = 0; j < nColomn; j++)
                {
                    Rect rc(i * nWidth, nBitHeight - (j + 1) * nWidth, nWidth, nWidth);
                    Bitmap* pBlock = pNewBit2->Clone(rc, PixelFormat32bppARGB);
                    if (pBlock)
                    {
                        printf(".");
                        Sleep(20);
                        SaveMinimapBlock(szOutDir, nLayer, nRowID, nID, pBlock);
                        delete pBlock;
                        pBlock = NULL;
                    }				
                    nID += 1;
                }
                nRowID += 1;
            }
            SAFE_DELETE(pNewBit2);
            printf("[OK]!\n");
            return;
        }

		UINT uW = pMinimap->GetWidth();
		UINT uH = pMinimap->GetHeight();
        UINT uX = uW / nWidth;
        UINT uY = uH / nWidth;
        
		for (UINT i = 0; i < uX; ++i)
		{
			for (UINT j = 0; j < uY; ++j)
			{
				Rect rc(i * nWidth, uH - (j + 1) * nWidth, nWidth, nWidth);
				Bitmap* pBlock = pMinimap->Clone(rc, PixelFormat32bppARGB);
				if (pBlock)
				{
					printf(".");
					Sleep(20);
					SaveMinimapBlock(szOutDir, nLayer, i, j, pBlock);
					delete pBlock;
					pBlock = NULL;
				}				
			}
		}

		UINT uLeftW = uW % nWidth;
		if (uLeftW)
		{
			for (UINT j = 0; j < uY; ++j)
			{
				Rect rc(uX * nWidth, uH - (j + 1) * nWidth, uLeftW, nWidth);
				Bitmap* pBlock = pMinimap->Clone(rc, PixelFormat32bppARGB);
				if (pBlock)
				{
					printf(".");
					Sleep(20);
					SaveMinimapSideBlock(szOutDir, nLayer, nWidth, uX, j, pBlock);
					delete pBlock;
					pBlock = NULL;
				}
			}
		}
    
		UINT uLeftH = uH % nWidth;
		if (uLeftH)
		{
			for (UINT i = 0; i < uX; ++i)
			{
				Rect rc(i * nWidth, 0, nWidth, uLeftH);
				Bitmap* pBlock = pMinimap->Clone(rc, PixelFormat32bppARGB);
				if (pBlock)
				{
					printf(".");
					Sleep(20);
					SaveMinimapSideBlock(szOutDir, nLayer, nWidth, i, uY, pBlock);
					delete pBlock;
					pBlock = NULL;
				}
			}
		}

		if (uLeftW && uLeftH)
		{
			Rect rc(uX * nWidth, 0, uLeftW, uLeftH);
			Bitmap* pBlock = pMinimap->Clone(rc, PixelFormat32bppARGB);
			if (pBlock)
			{
				printf(".");
				Sleep(20);
				SaveMinimapSideBlock(szOutDir, nLayer, nWidth, uX, uY, pBlock);
				delete pBlock;
				pBlock = NULL;
			}
		}
	}
	printf("[OK]!\n");
}

void ScissorMinimap(int nWidth)
{
	printf("%s!\n", "小地图剪刀手正在作业");
	printf("%s!\n", "可能需要几分钟的时间,请耐心等待");
	printf("\n");
	WIN32_FIND_DATAA FindFileData;
	HANDLE hFile = ::FindFirstFileA("*.bmp", &FindFileData);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		int nLayer = atoi(FindFileData.cFileName);
		ScissorMinimap(FindFileData.cFileName, "", nLayer, nWidth, true);
		while(::FindNextFileA(hFile, &FindFileData)) 
		{
			int nLayer = atoi(FindFileData.cFileName);
			ScissorMinimap(FindFileData.cFileName, "", nLayer, nWidth, true);
		}
		FindClose(hFile);
	}
	else
	{
		printf("%s!\n", "该文件夹找不到小地图文件(*.bmp)");
	}
}

/*
INT main()
{
	printf("%s\n", "-----------------------------------------------------");
	printf("%s!\n", "你好，欢迎使用小地图剪刀手");
	printf("%s:\n", "注意事项");
	printf("%s!\n", "    使用前,请把我放入小地图所在的文件夹");
	printf("%s!\n", "    小地图名称应该为0.bmp,1.bmp....  0,1为小地图层数");
	printf("%s:(%d,%d)!\n", "    剪裁后的每块小地图的大小为", BLOCK_WIDTH, BLOCK_WIDTH);
	printf("%s\n", "如有疑问, 或者不能正常工作请与作者联系");
	printf("%s\n", "MSN: huchangyin@kingsoft.com");
	printf("%s\n", "-----------------------------------------------------");
	printf("\n");
	printf("\n");
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	InitScissors();

	ScissorMinimap();

	GdiplusShutdown(gdiplusToken);

	getch();

	return 0;
}
*/

int main(int argc, char * argv[])
{
	char szFile[MAX_PATH];
	szFile[0] = '\0';
	char szOutDir[MAX_PATH];
	szOutDir[0] = '\0';
	int nLayer = 0;
	int nWidth = 128;
	int nStartX = 0;
	int nStartY = 0;
	BOOL bUserInput = TRUE;
    int  nFillEdge = true;
	printf("%s\n", "-----------------------------------------------------");
	printf("%s!\n", "你好，欢迎使用小地图剪刀手");
	printf("%s\n", "如有疑问, 或者不能正常工作请与作者联系");
	printf("%s\n", "MSN: huchangyin@kingsoft.com");
	printf("%s\n", "-----------------------------------------------------");
	printf("\n");

	if (argc > 1)
	{
		strcpy(szFile, argv[1]);
		if (argc > 2)
		{
			strcpy(szOutDir, argv[2]);
			if (argc > 3)
			{
				nLayer = atoi(argv[3]);
				if (argc > 4)
				{
					nWidth = atoi(argv[4]);
					if (argc > 6)
					{
						nStartX = atoi(argv[5]);
						nStartY = atoi(argv[6]);
                        if (argc > 7)
                        {
                            nFillEdge = atoi(argv[7]);
                        }
					}
				}
			}

		}
		bUserInput = FALSE;
	}

	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	if (bUserInput)
	{
		while (true)
		{
			printf("%s:\n", "自动裁图：Y");
			printf("\t%s:\n", "把待裁剪的图命名为*.bmp（*表示小地图的层级，因该是一个数字，默认为0）");
			printf("\t%s:\n", "小地图剪刀手会自动帮你裁图并默认补边");
			printf("\t%s:(%d,%d)!\n", "剪裁后的每块小地图的大小为", nWidth, nWidth);
			printf("%s:\n", "手动裁图：N");
			printf("\t%s:\n", "手动裁图会让你输入裁图的参数");
			printf("\t%s:\n", "你可以自己指定裁图的文件，输出目录，层级，生成图的大小，是否补边（是为非0，默认为非0）等");
			char szInput[MAX_PATH];
			szInput[0] = '\0';
			while (stricmp(szInput, "y") && stricmp(szInput, "n") && stricmp(szInput, "yes") && stricmp(szInput, "no"))
			{
				printf("%s:", "自动裁图（Y/N）");
				scanf("%s", szInput);
			}

			BOOL bAuto = FALSE;
			if (stricmp(szInput, "y") == 0 || stricmp(szInput, "yes") == 0 )
				bAuto = true;
			if (!bAuto)
			{
				printf("%s:", "请输入裁图文件：");
				scanf("%s", szFile);
				printf("%s:", "请输入输出目录：");
				scanf("%s", szOutDir);
				printf("%s:", "请输入层级：");
				scanf("%d", &nLayer);
				printf("%s:", "请输入裁图的大小：");
				scanf("%d", &nWidth);
                printf("%s:", "请输入是否补边：");
                scanf("%d", &nFillEdge);
			}

			if (InitScissors(nWidth))
			{
				if (bAuto)
					ScissorMinimap(nWidth);
				else
					ScissorMinimap(szFile, szOutDir, nLayer, nWidth, nFillEdge);
			}

			szInput[0] = '\0';
			while (stricmp(szInput, "y") && stricmp(szInput, "n") && stricmp(szInput, "yes") && stricmp(szInput, "no"))
			{
				printf("%s:", "继续裁图（Y/N）");
				scanf("%s", szInput);
			}
			if (stricmp(szInput, "n") == 0 || stricmp(szInput, "no") == 0)
				break;
		}
	}
	else
	{
		if (InitScissors(nWidth))
		{
			ScissorMinimap(szFile, szOutDir, nLayer, nWidth, nFillEdge);
		}
	}

	GdiplusShutdown(gdiplusToken);

	if (pbyDefaultData)
	{
		delete[] pbyDefaultData;
		pbyDefaultData = NULL;
	}

	return 0;
}
