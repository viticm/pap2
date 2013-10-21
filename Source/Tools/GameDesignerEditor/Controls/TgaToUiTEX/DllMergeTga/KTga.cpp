
#include "stdafx.h"
#include "KTga.h"
#include <iostream>

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
    //直接从一个32位或24位tga图像创建该文件数据。只支持未压缩的tga图像。
    //参数：pTgaFileName -- 文件名。
    //      bCreateDataBuffer -- 是否创建数据区，默认为不创建（false）。
    //数据区是32位数据（r,g,b,a）--按行从上到下存储。
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

    if (bCreateDataBuffer) 
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

int KTga32::ReadARowToMemory(void * pTarget, int nRow)
{
    //从文件中读取一行到数据区。
    //参数：pTarget  ---  目标数据区地址。
    //      nRow     ---  行数，从上到下，从0开始。
    //该行数不负责检测数据区益处问题。
    int nResult = false;
    FILE *pTgaFile = NULL;
    int nRowSize = 0;
    size_t dwReadIn = 0;
    int i =  0;

    KG_PROCESS_ERROR(pTarget);
    if (m_TgaFileHeader.ImageType != 2) 
    {
        nRowSize = m_TgaFileHeader.Width * sizeof(DWORD);
        for (i = 0; i < nRowSize; i++) 
        {
            ((BYTE *)pTarget)[i] = 255;
        }

    }
    KG_PROCESS_ERROR(m_TgaFileHeader.ImageType == 2);
    KG_PROCESS_ERROR(m_TgaFileHeader.PixelDep == 32 || m_TgaFileHeader.PixelDep == 24);

    switch(m_TgaFileHeader.PixelDep) 
    {
    case 32:
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
    	break;
    case 24:
        pTgaFile = fopen(m_szTgaFireName, "rb");
        KG_PROCESS_ERROR(pTgaFile);
        fseek(pTgaFile, 
            sizeof(m_TgaFileHeader) + m_TgaFileHeader.IDLength + m_TgaFileHeader.Width * (m_TgaFileHeader.Height - 1 - nRow) * 3, 
            SEEK_CUR);
        for (i = 0; i < m_TgaFileHeader.Width; i++) 
        {
            dwReadIn = fread((BYTE *)pTarget + i * 4, 3, 1, pTgaFile);
            *((BYTE *)pTarget + i * 4 + 3) = 255;
            KG_PROCESS_ERROR(dwReadIn);
        }       
        fclose(pTgaFile);
        pTgaFile = NULL;
        break;
    default:
        break;
    }

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

int KTga32::ReadAllDataToMemory(void * pTarget, int x, int y, int nWidth)
{
    //将该文件的所有数据读入内存数据区中。
    //参数：pTarget ----- 目标数据区地址。
    //      x,y     ----- 读取到目标数据区中的行，列数。
    //      nWidth  ----- 目标数据区的宽度。
    //本函数不负责检查数据区溢出，也不负责边缘剪裁，当x + 自身的长度 > nWidth 或其他不正常情况下会出现错误。
    int nResult = false;
    int i = 0;

    KG_PROCESS_ERROR(pTarget);
    for (i = 0; i < m_TgaFileHeader.Height; i++) 
    {
        ReadARowToMemory((BYTE *)pTarget + (nWidth * (y + i) + x) * sizeof(DWORD), i);
    }

    nResult =true;
Exit0:
    return nResult; 
}

int KTga32::ReadAllDataToSelfMemory()
{
    //将该文件的数据读入到自己的数据区中。
    //只用于从文件创建，并创建了数据区的情况。
    int nResult = false;
    int i = 0;

    KG_PROCESS_ERROR(m_pDataBuffer);

    for (i = 0; i < m_TgaFileHeader.Height; i++) 
    {
        ReadARowToMemory((BYTE *)m_pDataBuffer + m_TgaFileHeader.Width *  i * sizeof(DWORD), i);
    }

    nResult =true;
Exit0:
    return nResult; 
}

int KTga32::Create(int nWidth, int nHeight, int nDepth)
{
    //直接创建数据区。
    //nWidth ---- 数据区长度。
    //nHeight --- 数据区高度。
    //nDepth  --- 数据区深度。只支持 32。
    //创建的数据区大小为nWidth * nHeight * nDepth / 8 。
    int nResult = false;

    KG_PROCESS_ERROR(nWidth > 0);
    KG_PROCESS_ERROR(nHeight > 0);
    KG_PROCESS_ERROR(nDepth == 32);

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
        if (m_pDataBuffer) 
        {
            delete[] m_pDataBuffer;
            m_pDataBuffer = NULL;
        }
    }    
    return nResult;
}

int KTga32::Release()
{
    //如果创建了数据区，释放自身数据缓冲区。
    if (m_pDataBuffer) {
        delete[] m_pDataBuffer;
        m_pDataBuffer = NULL;
    }
    return true;
}

int KTga32::SaveDataBufferToTgaFile(char* pTgaFileName)
{
    //把数据区的数据保存到一个tga文件中，该文件为32位未经压缩的tga文件。
    //参数：pTgaFileName ------ 要保存的文件名“*.tga”。
    //只用于创建了数据区的情况。
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
    //把数据区的数据保存到一个bmp文件中，该文件为32位未经压缩的bmp文件。
    //参数：pTgaFileName ------ 要保存的文件名“*.bmp”。
    //只用于创建了数据区的情况。

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

int	KTga32::SaveDataBufferToCurFile(char *pCurFileName)
{
	typedef struct _tagCurFileHeader
	{
		WORD cdtype;
		WORD cdcount;
	} tagCurFileHeader;
	typedef struct _tagCurDirectoryHeader
	{
		byte bwidth;
		byte bheight;
		byte bcolorcount;
		byte breserved;

		WORD wxhotspot;
		WORD wyhotspot;

		DWORD lbytesinres;
		DWORD dwimageoffset;
	} tagCurDirectoryHeader;



	FILE *pCurFile = NULL;
	int nResult = false;
	size_t dwWrite = 0;
	BYTE *pbyXORBuffer = NULL;
	WORD cdreserved = 0;

	tagCurFileHeader curFileHeader;
	tagCurDirectoryHeader curDirectoryHeader;
	BITMAPINFOHEADER bmpInfoHeader;

	KG_PROCESS_ERROR(pCurFileName);
	KG_PROCESS_ERROR(pCurFileName[0]);

	KG_PROCESS_ERROR(m_TgaFileHeader.Width == m_TgaFileHeader.Height);
	switch (m_TgaFileHeader.Width)
	{
	case 16:
	case 32:
	case 48:
	case 64:
		break;
	default:
		KG_PROCESS_ERROR(false);
		break;
	}

	pCurFile =  fopen(pCurFileName, "wb");
	KG_PROCESS_ERROR(pCurFile);
	
	dwWrite = fwrite(&cdreserved, sizeof(WORD), 1, pCurFile);
	KG_PROCESS_ERROR(dwWrite);
	
	curFileHeader.cdtype = 2;
	curFileHeader.cdcount = 1;
	dwWrite = fwrite(&curFileHeader, sizeof(tagCurFileHeader), 1, pCurFile);
	KG_PROCESS_ERROR(dwWrite);
	
	curDirectoryHeader.bwidth = (byte)(m_TgaFileHeader.Width);
	curDirectoryHeader.bheight = (byte)(m_TgaFileHeader.Height);
	curDirectoryHeader.bcolorcount = 0;
	curDirectoryHeader.wxhotspot = 0;
	curDirectoryHeader.wyhotspot = 0;
	curDirectoryHeader.lbytesinres =  m_TgaFileHeader.Width * m_TgaFileHeader.Height * sizeof(DWORD) + 
		(m_TgaFileHeader.Width * m_TgaFileHeader.Height) / 8;
	curDirectoryHeader.dwimageoffset = 22;
	dwWrite = fwrite(&curDirectoryHeader, sizeof(tagCurDirectoryHeader), 1, pCurFile);
	KG_PROCESS_ERROR(dwWrite);

	bmpInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmpInfoHeader.biWidth = m_TgaFileHeader.Width;
	bmpInfoHeader.biHeight = m_TgaFileHeader.Height * 2;
	bmpInfoHeader.biPlanes = 1;
	bmpInfoHeader.biBitCount = 32;
	bmpInfoHeader.biCompression = 0;
	bmpInfoHeader.biSizeImage = m_TgaFileHeader.Width * m_TgaFileHeader.Height * sizeof(DWORD);
	bmpInfoHeader.biXPelsPerMeter = 0;
	bmpInfoHeader.biYPelsPerMeter = 0;
	bmpInfoHeader.biClrUsed = 0;
	bmpInfoHeader.biClrImportant = 0;
	dwWrite = fwrite(&bmpInfoHeader, sizeof(BITMAPINFOHEADER), 1, pCurFile);
	KG_PROCESS_ERROR(dwWrite);

	//接下来是这一页的数据，从最后面一行到最上面一行。
	KG_PROCESS_ERROR(m_pDataBuffer);
	for (int i = 0; i < m_TgaFileHeader.Height; i++) 
	{
		dwWrite = fwrite((BYTE *)m_pDataBuffer + 
			(m_TgaFileHeader.Height - 1 - i) * m_TgaFileHeader.Width * sizeof(DWORD),
			m_TgaFileHeader.Width * sizeof(DWORD), 1, pCurFile);
		KG_PROCESS_ERROR(dwWrite);
	}

	//依次是个像素的Alpha为零的话为1， 否则为0， 从最后面一行到最上面一行。
	
	pbyXORBuffer = new BYTE[m_TgaFileHeader.Width * m_TgaFileHeader.Height / 8];
	KG_PROCESS_ERROR(pbyXORBuffer);
	ZeroMemory(pbyXORBuffer, m_TgaFileHeader.Width * m_TgaFileHeader.Height / 8);

	for (int i = 0; i < m_TgaFileHeader.Height; ++i)
	{
		for (int j = 0; j < m_TgaFileHeader.Width; ++j)
		{
			BYTE *pAlpha = (BYTE *)m_pDataBuffer + 
				((m_TgaFileHeader.Height - 1 - i) * m_TgaFileHeader.Width + j) * sizeof(DWORD) + 3;
			BYTE *pCode = pbyXORBuffer + (i * m_TgaFileHeader.Width + j) / 8;
			*pCode += ((*pAlpha == 0) ? (1 << ((i * m_TgaFileHeader.Width + j) % 8)) : 0); 
		}
	}

	dwWrite = fwrite(pbyXORBuffer, m_TgaFileHeader.Width * m_TgaFileHeader.Height / 8, 1, pCurFile);
	KG_PROCESS_ERROR(dwWrite);

	nResult = true;
Exit0:
	if (pbyXORBuffer)
	{
		delete[] pbyXORBuffer;
		pbyXORBuffer = NULL;
	}
	if (pCurFile)
	{
		fclose(pCurFile);
		pCurFile = NULL;
	}
	return nResult;
}


int KTga32::ProduceAlphaCodeToFile(char *pAlphaDataFileName, char *pAlphaCodeFileName)
{
    //生成Alpha数据及掩码文件。 
    //参数pAlphaDataFileName---要生成的Alpha数据文件名。
    //参数pAlphaCodeFileName---要生成的掩码文件名。

    int nResult = FALSE;
    FILE *fpAlphaFile = NULL;
    size_t dwWrite = 0;
    int i = 0;
    int nDataLen = 0;
    BYTE *pAlpha = NULL;
    int nCodeSize = 0;
    BYTE *pCodeBuffer = NULL;
    BYTE *pCode = NULL;

    KG_PROCESS_ERROR(pAlphaDataFileName && pAlphaDataFileName[0]);
    KG_PROCESS_ERROR(pAlphaCodeFileName && pAlphaCodeFileName[0]);
    


    fpAlphaFile = fopen(pAlphaDataFileName, "wb");
    KG_PROCESS_ERROR(fpAlphaFile);

    dwWrite = fwrite(&m_TgaFileHeader.Width, sizeof(WORD), 1, fpAlphaFile);
    KG_PROCESS_ERROR(dwWrite);    
    dwWrite = fwrite(&m_TgaFileHeader.Height, sizeof(WORD), 1, fpAlphaFile);
    KG_PROCESS_ERROR(dwWrite);

    nDataLen = m_TgaFileHeader.Height * m_TgaFileHeader.Width;
    for (i = 0; i < nDataLen; i ++) 
    {
        pAlpha = (BYTE *)m_pDataBuffer + i * sizeof(DWORD) + 3;
        dwWrite = fwrite(pAlpha, sizeof(BYTE), 1, fpAlphaFile);
        KG_PROCESS_ERROR(dwWrite);  
    }
    fclose(fpAlphaFile);
    fpAlphaFile = NULL;

    nCodeSize = nDataLen / 8  + (((nDataLen % 8) == 0) ? 0 : 1);
    pCodeBuffer = new BYTE[nCodeSize];
    KG_PROCESS_ERROR(pCodeBuffer);
    ZeroMemory(pCodeBuffer, nCodeSize);
    for (i = 0; i < nDataLen; i ++) 
    {
        pAlpha = (BYTE *)m_pDataBuffer + i * sizeof(DWORD) + 3;
        pCode = pCodeBuffer + i / 8;
        //*pCode += ((*pAlpha == 0) ? 0 : (1 << (7 - i % 8))); 
        *pCode += ((*pAlpha == 0) ? 0 : (1 << (i % 8))); 
    }

    fpAlphaFile = fopen(pAlphaCodeFileName, "wb");
    KG_PROCESS_ERROR(fpAlphaFile);
    dwWrite = fwrite(&m_TgaFileHeader.Width, sizeof(WORD), 1, fpAlphaFile);
    KG_PROCESS_ERROR(dwWrite);    
    dwWrite = fwrite(&m_TgaFileHeader.Height, sizeof(WORD), 1, fpAlphaFile);
    KG_PROCESS_ERROR(dwWrite);
    dwWrite = fwrite(pCodeBuffer, nCodeSize, 1, fpAlphaFile);
    KG_PROCESS_ERROR(dwWrite);
    fclose(fpAlphaFile);
    fpAlphaFile = NULL;


    nResult = TRUE;
Exit0:
    if (fpAlphaFile) 
    {
        fclose(fpAlphaFile);
        fpAlphaFile = NULL;
    }
    if (pCodeBuffer) 
    {
        delete[] pCodeBuffer;
        pCodeBuffer = NULL;
    }
    return nResult;

}

int KTga32::CopyRectBufferTo(BYTE *pDest, int nDestBufferSize, int x, int y, int nWidth, int nHeight)
{
    int nResult = false;
    BYTE *pNow = NULL;
    KG_PROCESS_ERROR(pDest);
    KG_PROCESS_ERROR(m_pDataBuffer);
    KG_PROCESS_ERROR(x + nWidth <= GetWidth());
    KG_PROCESS_ERROR(y + nHeight <= GetHeight());
    KG_PROCESS_ERROR(nDestBufferSize >= nWidth * nHeight * 4);

    for (int i = 0; i < nHeight; ++i) 
    {
        pNow = (BYTE *)m_pDataBuffer + ((y + i) * m_TgaFileHeader.Width + x) * 4;
        memcpy(pDest, pNow, nWidth * 4);
        pDest += nWidth * 4;
    }

    nResult = true;
Exit0:
    return nResult;
}


