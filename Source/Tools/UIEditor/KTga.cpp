#include "Stdafx.h"
#include "KTga.h"

KTga32::KTga32()
{
    m_pDataBuffer = NULL;
    m_szTgaFireName[0] = 0;
}

KTga32::~KTga32()
{

}

int KTga32::CreateFromFile(char *pTgaFileName, BOOL bCreateDataBuffer)
{
    int nResult = false;
    FILE   *pTgaFile = NULL;
    size_t dwReadIn = 0;

    KG_PROCESS_ERROR(pTgaFileName);
    KG_PROCESS_ERROR(pTgaFileName[0]);

    ZeroMemory(&m_TgaFileHeader, sizeof(m_TgaFileHeader));
    strcpy(m_szTgaFireName, pTgaFileName);

    pTgaFile = fopen(pTgaFileName, "rb");  
    KG_PROCESS_ERROR(pTgaFile);

    dwReadIn = fread(&m_TgaFileHeader, sizeof(m_TgaFileHeader), 1, pTgaFile);
    KG_PROCESS_ERROR(dwReadIn);

    fclose(pTgaFile);
    pTgaFile = NULL;

    if (bCreateDataBuffer == TRUE) 
    {
        Create(m_TgaFileHeader.Width, m_TgaFileHeader.Height, 32);
    }
    KG_PROCESS_ERROR(m_TgaFileHeader.ImageType == 2);
    KG_PROCESS_ERROR(m_TgaFileHeader.PixelDep == 32);
    nResult = true;

Exit0:
    if (!nResult) 
    {
        if (pTgaFile) 
        {
            fclose(pTgaFile);
            pTgaFile = NULL;
        }
    }
    return nResult;

}

int KTga32::ReadARowToMemery(void * pTarget, int nRow)
{
    int nResult = false;
    FILE *pTgaFile = NULL;
    int nRowSize = 0;
    size_t dwReadIn = 0;

    KG_PROCESS_ERROR(pTarget);
    if (m_TgaFileHeader.PixelDep != 32 || m_TgaFileHeader.ImageType != 2) 
    {
        int i;
        nRowSize = m_TgaFileHeader.Width * sizeof(DWORD);
        for (i = 0; i < nRowSize; i++) 
        {
            ((BYTE *)pTarget)[i] = 255;
        }

    }
    KG_PROCESS_ERROR(m_TgaFileHeader.PixelDep == 32);
    KG_PROCESS_ERROR(m_TgaFileHeader.ImageType == 2);

    nRowSize = m_TgaFileHeader.Width * sizeof(DWORD);

    pTgaFile = fopen(m_szTgaFireName, "rb");
    KG_PROCESS_ERROR(pTgaFile);

    fseek(pTgaFile, 
        sizeof(m_TgaFileHeader) + m_TgaFileHeader.IDLength + m_TgaFileHeader.Width * (m_TgaFileHeader.Height - 1 - nRow) * sizeof(DWORD), 
        SEEK_CUR);

    dwReadIn = fread(pTarget, nRowSize, 1, pTgaFile);
    KG_PROCESS_ERROR(dwReadIn);

    fclose(pTgaFile);
    pTgaFile = NULL;


    nResult =true;
Exit0:
    if (!nResult) 
    {
        if (pTgaFile) 
        {
            fclose(pTgaFile);
            pTgaFile = NULL;
        }
    }

    return nResult; 
}

int KTga32::ReadAllDataToMemery(void * pTarget, int x, int y, int nWidth)
{
    int nResult = false;
    int i = 0;

    KG_PROCESS_ERROR(pTarget);
    for (i = 0; i < m_TgaFileHeader.Height; i++) 
    {
        ReadARowToMemery((BYTE *)pTarget + (nWidth * (y + i) + x) * sizeof(DWORD), i);
    }

    nResult =true;
Exit0:
    return nResult; 
}

int KTga32::ReadAllDataToSelfMemery()
{
    int nResult = false;
    int i = 0;

    KG_PROCESS_ERROR(m_pDataBuffer);

    for (i = 0; i < m_TgaFileHeader.Height; i++) 
    {
        ReadARowToMemery((BYTE *)m_pDataBuffer + m_TgaFileHeader.Width *  i * sizeof(DWORD), i);
    }

    nResult =true;
Exit0:
    return nResult; 
}

int	KTga32::Premultiplied()
{
	int nResult = false;
	int i = 0;
	int j = 0;
	BYTE * pData;

	KG_PROCESS_ERROR(m_pDataBuffer);

	pData = (BYTE *)m_pDataBuffer;

	for (i = 0; i < m_TgaFileHeader.Height; i++) 
	{
		for (j = 0; j < m_TgaFileHeader.Width; ++i)
		{
			BYTE * pPix = pData + i * m_TgaFileHeader.Width * 4 + j * 4;
			DWORD dwAlpha = *(pPix + 3);
			*pPix = (BYTE)((DWORD)(*pPix * dwAlpha) / 255);
			*(pPix + 1) = (BYTE)((DWORD)(*(pPix + 1) * dwAlpha) / 255);
			*(pPix + 2) = (BYTE)((DWORD)(*(pPix + 2) * dwAlpha) / 255);
		}
	}

	nResult =true;
Exit0:
	return nResult; 
}

int KTga32::Create(int nWidth, int nHeight, int nDepth)
{
    int nResult = false;

    m_pDataBuffer = new BYTE[nHeight * nWidth * nDepth / 8];
    KG_PROCESS_ERROR(m_pDataBuffer);
    ZeroMemory(m_pDataBuffer, nHeight * nWidth * nDepth / 8);

    ZeroMemory(&m_TgaFileHeader, sizeof(m_TgaFileHeader));
    m_TgaFileHeader.Width     = nWidth;
    m_TgaFileHeader.Height    = nHeight;
    m_TgaFileHeader.PixelDep  = nDepth;
    m_TgaFileHeader.ImageType = 2;
    m_TgaFileHeader.Desc      = 8;

    nResult =true;
Exit0:
    if (!nResult) 
    {
        delete[] m_pDataBuffer;
        m_pDataBuffer = NULL;
    }    
    return nResult;


}

int KTga32::Release()
{
    if (m_pDataBuffer) {
        delete[] m_pDataBuffer;
        m_pDataBuffer = NULL;
    }
    return true;
}

int KTga32::SaveDataBufferToTgaFile(char* pTgaFileName)
{
    int nResult = false;
    FILE *pTgaFile = NULL;
    size_t dwWrite = 0;
    int i = 0;


    KG_PROCESS_ERROR(pTgaFileName);
    KG_PROCESS_ERROR(pTgaFileName[0]);

    pTgaFile = fopen(pTgaFileName, "wb");
    KG_PROCESS_ERROR(pTgaFile);

    dwWrite = fwrite(&m_TgaFileHeader, sizeof(m_TgaFileHeader), 1, pTgaFile);
    KG_PROCESS_ERROR(dwWrite);

    KG_PROCESS_ERROR(m_pDataBuffer);
    for (i = 0; i < m_TgaFileHeader.Height; i++) 
    {
        dwWrite = fwrite((BYTE *)m_pDataBuffer + (m_TgaFileHeader.Height - 1 - i) * m_TgaFileHeader.Width * sizeof(DWORD),
            m_TgaFileHeader.Width * sizeof(DWORD), 1, pTgaFile);
        KG_PROCESS_ERROR(dwWrite);
    }

    fclose(pTgaFile);
    pTgaFile = NULL;

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (pTgaFile) 
        {
            fclose(pTgaFile);
            pTgaFile = NULL;
        }
    }
    return nResult;
}

int KTga32::SaveDataBufferToBmpFile(char *pBmpFileName)
{
    int nResult = false;
    FILE *pBmpFile = NULL;
    size_t dwWrite = 0;
    int i = 0;
    DWORD dwEnd;
    BITMAPINFOHEADER bmpInfoHeader;

    BITMAPFILEHEADER bmpHeader;


    KG_PROCESS_ERROR(pBmpFileName);
    KG_PROCESS_ERROR(pBmpFileName[0]);

    ZeroMemory(&bmpHeader, sizeof(BITMAPFILEHEADER));
    ZeroMemory(&bmpInfoHeader, sizeof(BITMAPINFOHEADER));

    bmpHeader.bfType = 19778;
    bmpHeader.bfReserved1 = 0;
    bmpHeader.bfReserved2 = 0;
    bmpHeader.bfSize = sizeof(BITMAPFILEHEADER) +sizeof(BITMAPINFOHEADER) + m_TgaFileHeader.Height * m_TgaFileHeader.Width * sizeof(DWORD) + 2;
    bmpHeader.bfOffBits =  sizeof(BITMAPFILEHEADER) +sizeof(BITMAPINFOHEADER);

    bmpInfoHeader.biBitCount = 32;
    bmpInfoHeader.biClrImportant = 0;
    bmpInfoHeader.biClrUsed = 0;
    bmpInfoHeader.biCompression = 0;
    bmpInfoHeader.biHeight = m_TgaFileHeader.Height;
    bmpInfoHeader.biPlanes = 1;
    bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmpInfoHeader.biSizeImage = m_TgaFileHeader.Height * m_TgaFileHeader.Width * sizeof(DWORD) + 2;
    bmpInfoHeader.biWidth = m_TgaFileHeader.Width;
    bmpInfoHeader.biXPelsPerMeter = 2834;
    bmpInfoHeader.biYPelsPerMeter = 2834;

    pBmpFile = fopen(pBmpFileName, "wb");
    KG_PROCESS_ERROR(pBmpFile);

    dwWrite = fwrite(&bmpHeader, sizeof(BITMAPFILEHEADER), 1, pBmpFile);
    KG_PROCESS_ERROR(dwWrite);
    dwWrite = fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, pBmpFile);
    KG_PROCESS_ERROR(dwWrite);

    KG_PROCESS_ERROR(m_pDataBuffer);
    for (i = 0; i < m_TgaFileHeader.Height; i++) 
    {
        dwWrite = fwrite((BYTE *)m_pDataBuffer + (m_TgaFileHeader.Height - 1 - i) * m_TgaFileHeader.Width * sizeof(DWORD),
            m_TgaFileHeader.Width * sizeof(DWORD), 1, pBmpFile);
        KG_PROCESS_ERROR(dwWrite);
    }
    
    dwEnd = 0;
    fwrite(&dwEnd, sizeof(dwEnd), 1, pBmpFile);
    fclose(pBmpFile);
    pBmpFile = NULL;

    nResult = true;
Exit0:
    if (!nResult) 
    {
        if (pBmpFile) 
        {
            fclose(pBmpFile);
            pBmpFile = NULL;
        }
    }
    return nResult;
}
