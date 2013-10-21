#include "stdafx.h"
#include "./kimageinfo.h"
#include "../KGUI.h"

namespace UI
{

	KImageInfo::KImageInfo()
	{
		m_sizeImage.cx = 0;
		m_sizeImage.cy = 0;
	}

	KImageInfo::~KImageInfo()
	{
	}

	bool KImageInfo::LoadFromFile(LPCSTR szFileName)
	{
		BOOL bRetCode = FALSE;

		bRetCode = LoadUITexFile(szFileName);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = LoadUIAlphaFile(szFileName);
		KGLOG_CHECK_ERROR(bRetCode);

		return true;
Exit0:
		return false;
	}


	bool KImageInfo::LoadUITexFile(LPCSTR szFileName)
	{
        int nRetCode    = false;
        int nFilePathID = 0;
		int nFileNameLength = 0;
        char szFilePath[MAX_PATH];
        DWORD dwImageID = 0;
        LPSTR pszTail = NULL;
        HRESULT hr = E_FAIL;
        UITEXFILEHEADER FileHeader;
        size_t dwReadIn = 0;
		KSmartFile File;

        KGLOG_PROCESS_ERROR(szFileName);
        KGLOG_PROCESS_ERROR(szFileName[0] != '\0');

		nFileNameLength = (int)strlen(szFileName);

		nRetCode = nFileNameLength > 5;
		KG_PROCESS_ERROR(nRetCode);

		nRetCode = stricmp(szFileName + nFileNameLength - 5 - 1, ".uitex") == 0;
		KG_PROCESS_ERROR(nRetCode);
		
        if (szFileName[0] != '\\')
        {
            szFilePath[0] = '\\';
            szFilePath[1] = '\0';
        }
        else
        {
            szFilePath[0] = '\0';
        }
        strcat(szFilePath, szFileName);
        szFilePath[_countof(szFilePath) - 1] = '\0';

        File = g_OpenFile(szFilePath);
        KGLOG_PROCESS_ERROR(File);

        dwReadIn = File->Read(&FileHeader, sizeof(UITEXFILEHEADER));
        KGLOG_PROCESS_ERROR(dwReadIn == sizeof(UITEXFILEHEADER));

        pszTail = strrchr(szFilePath, '\\');
        ASSERT(pszTail);
        ++pszTail;

		pszTail[0] = '\0';
		FormatFilePath(FileHeader.szTgaFileName);
		KGLOG_PROCESS_ERROR(strlen(szFilePath) + strlen(FileHeader.szTgaFileName) < _countof(szFilePath));
		strcat(szFilePath, FileHeader.szTgaFileName);

        hr = g_pUI->m_p3DUI->LoadImage(szFilePath, &dwImageID);
        KGLOG_COM_PROCESS_ERROR(hr);

		m_STexFileInfo.SetImageID(dwImageID);

		nFilePathID = g_pUI->m_FilePathMgr.AddFilePath(szFilePath);
		m_STexFileInfo.SetFilePathID(nFilePathID);

        m_sizeImage.cx = FileHeader.nWidth;
		KGLOG_PROCESS_ERROR(m_sizeImage.cx > 0);

        m_sizeImage.cy = FileHeader.nHeight;
		KGLOG_PROCESS_ERROR(m_sizeImage.cy > 0);

        if (FileHeader.nFrameCount > 0)
        {
			nRetCode = m_aFrame.Alloc(FileHeader.nFrameCount);
            KGLOG_PROCESS_ERROR(nRetCode);

			for (int nIndex = 0; nIndex < m_aFrame.GetSize(); ++nIndex)
			{
				int nFrame = 0;
				POINT ptLeftTop = { 0, 0 };
				SIZE sizeFrame = { 0, 0 };
                UITEXFRAMEDATASTRUCTURE FrameData;

                dwReadIn = File->Read(&FrameData, sizeof(UITEXFRAMEDATASTRUCTURE));
                KGLOG_PROCESS_ERROR(dwReadIn == sizeof(UITEXFRAMEDATASTRUCTURE));

                nFrame = FrameData.dwInfo;
                ptLeftTop.x = FrameData.nLeft;
                ptLeftTop.y = FrameData.nTop;
                sizeFrame.cx = FrameData.nWidth;
                sizeFrame.cy = FrameData.nHeight;

				nRetCode = m_aFrame[nIndex].Init(ptLeftTop, sizeFrame, static_cast<WORD>(nFrame));
				KGLOG_PROCESS_ERROR(nRetCode);
			}
        }

        if (FileHeader.nAnimateCount > 0)
        {
			nRetCode = m_aFrameGroup.Alloc(FileHeader.nAnimateCount);
			KGLOG_PROCESS_ERROR(nRetCode);

			for (int nGroupIndex = 0; nGroupIndex < FileHeader.nAnimateCount; ++nGroupIndex)
			{
				int nFrameCount = 0;
                ASSERT(sizeof(int) == 4);
                dwReadIn = File->Read(&nFrameCount, sizeof(int));
                KGLOG_PROCESS_ERROR(dwReadIn == sizeof(int));

				if (nFrameCount > 0)
				{
					m_aFrameGroup[nGroupIndex].SetFrameCount(nFrameCount);

					for (int nGroupFrameIndex = 0; nGroupFrameIndex < nFrameCount; ++nGroupFrameIndex)
					{
						int nFrameIndex = -1;
						ASSERT(sizeof(int) == 4);
                        dwReadIn = File->Read(&nFrameIndex, sizeof(int));
                        KGLOG_PROCESS_ERROR(dwReadIn == sizeof(int));

						m_aFrameGroup[nGroupIndex].SetFrameIndex(nGroupFrameIndex, nFrameIndex);
					}
				}
			}
        }

        return true;
Exit0:
		if (dwImageID != 0)
		{
			g_pUI->m_p3DUI->UnloadImage(dwImageID);
			m_STexFileInfo.SetImageID(0);
		}
		KGLogPrintf(KGLOG_ERR, "KGUI KImageInfo::LoadUITexFile(%s)\n", szFileName);
        return false;
	}

	bool KImageInfo::LoadUIAlphaFile(LPCSTR szFileName)
	{
        int nRetCode = false;
		int nFileNameLength = 0;
		int nLength = 0;
        long nBufferBegin = 0;
        long nBufferEnd = 0;
        short nWidth = 0;
		short nHeight = 0;
        size_t dwReadIn = 0;
		char szFilePath[MAX_PATH];
		LPSTR szTail = NULL;
		BYTE* pBuffer = NULL;
		KSmartFile File;

        KGLOG_PROCESS_ERROR(szFileName);
        KGLOG_PROCESS_ERROR(szFileName[0] != '\0');
		
		nFileNameLength = (int)strlen(szFileName);
		
		nRetCode = nFileNameLength > 5;
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = stricmp(szFileName + nFileNameLength - 5 - 1, ".uitex") == 0;
		KGLOG_PROCESS_ERROR(nRetCode);

		// 加载UIAlphaC
		szFilePath[_countof(szFilePath) - 1] = '\0';
		strncpy(szFilePath, szFileName, _countof(szFilePath));
		ASSERT(szFilePath[_countof(szFilePath) - 1] == '\0');
        szFilePath[_countof(szFilePath) - 1] = '\0';

		// 替换UITex为UIAlphaC
		szTail = strrchr(szFilePath, '.') + 1;

		ASSERT(szTail);
		szTail[0] = '\0';

		KGLOG_PROCESS_ERROR(sizeof(szFilePath) - (szTail - szFilePath) > sizeof("uialphac") / sizeof(char));
		strcat(szFilePath, "uialphac");

		nRetCode = !g_IsFileExist(szFilePath);
		KG_PROCESS_SUCCESS(nRetCode);

        File = g_OpenFile(szFilePath);
        KG_PROCESS_ERROR(File);

        ASSERT(sizeof(short) == 2);
        dwReadIn =  File->Read(&nWidth, sizeof(short));

		nRetCode = dwReadIn == sizeof(short);
        KGLOG_PROCESS_ERROR(nRetCode);

        ASSERT(sizeof(short) == 2);
        dwReadIn =  File->Read(&nHeight, sizeof(short));

		nRetCode = dwReadIn == sizeof(short);
        KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = nWidth == m_sizeImage.cx;
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = nHeight == m_sizeImage.cy;
		KGLOG_PROCESS_ERROR(nRetCode);

		m_Alpha.SetSize(nWidth, nHeight);

		nRetCode = m_Alpha.GetBuffer(&pBuffer, &nLength);
		KGLOG_PROCESS_ERROR(nRetCode);

        nBufferBegin = File->Tell();
		KGLOG_PROCESS_ERROR(nBufferBegin != -1);

		nRetCode = File->Seek(0, SEEK_END);
        KGLOG_PROCESS_ERROR(nRetCode != -1);

        nBufferEnd = File->Tell();
		KGLOG_PROCESS_ERROR(nBufferEnd != -1);

        File->Seek(nBufferBegin, SEEK_SET);

		nRetCode = nLength == nBufferEnd - nBufferBegin;
        KGLOG_PROCESS_ERROR(nRetCode);

		ASSERT(sizeof(BYTE) == 1);

        dwReadIn =  File->Read(pBuffer, sizeof(BYTE) * (nBufferEnd - nBufferBegin));

		nRetCode = dwReadIn == sizeof(BYTE) * (nBufferEnd - nBufferBegin);
        KGLOG_PROCESS_ERROR(nRetCode);

Exit1:
		return true;
Exit0:
        return false;
	}

	void KImageInfo::Unload()
	{
		g_pUI->m_p3DUI->UnloadImage(m_STexFileInfo.GetImageID());
	}

	SIZE KImageInfo::GetImageSize() const
	{
		return m_sizeImage;
	}

	SIZE KImageInfo::GetFrameSize(int nFrame) const
	{
		static SIZE const s_sizeNull = { 0, 0 };

		if (nFrame < 0 || nFrame >= m_aFrame.GetSize())
			return s_sizeNull;

		return m_aFrame[nFrame].GetFrameSize();
	}

	int KImageInfo::GetFrameCount() const
	{
		return m_aFrame.GetSize();
	}

	DWORD KImageInfo::GetImageID(int nFrame) const
	{
		if (nFrame < 0 || nFrame >= m_aFrame.GetSize())
			return 0;

		// TODO : 目前uitex只使用一个stex,一个stex有多个tga组成,分多帧
		return m_STexFileInfo.GetImageID();
	}

	RECT KImageInfo::GetImageRect(int nFrame) const
	{
		static RECT const s_rcNull = { 0, 0, 0, 0 };

		if (nFrame < 0 || nFrame >= m_aFrame.GetSize())
			return s_rcNull;

		POINT ptFrame = m_aFrame[nFrame].GetFrameLeftTop();
		SIZE sizeFrame = m_aFrame[nFrame].GetFrameSize();

		RECT rcFrame = { 0, 0, 0, 0 };

		rcFrame.left = ptFrame.x;
		rcFrame.right = ptFrame.x + sizeFrame.cx;
		rcFrame.top = ptFrame.y;
		rcFrame.bottom = ptFrame.y + sizeFrame.cy;

		return rcFrame;
	}

	int KImageInfo::GetImageFrame(int nFrame) const
	{
		if (nFrame < 0 || nFrame >= m_aFrame.GetSize())
			return -1;
		return m_aFrame[nFrame].GetImageFrame();
	}

	bool KImageInfo::IsImagePixelAlpha(int nFrame, int nX, int nY) const
	{
		if (nX < 0 || nY < 0)
			return false;

		if (nFrame < 0 || nFrame >= m_aFrame.GetSize())
			return false;

		SIZE sizeFrame = m_aFrame[nFrame].GetFrameSize();

		if (nX >= sizeFrame.cx || nY >= sizeFrame.cy)
			return false;

		POINT ptFrame = m_aFrame[nFrame].GetFrameLeftTop();

		if (!m_Alpha.IsAlpha(ptFrame.x + nX, ptFrame.y + nY))
			return false;

		return true;
	}

	int	KImageInfo::GetFrameGroupCount() const
	{
		return m_aFrameGroup.GetSize();
	}

	int	KImageInfo::GetFrameCount(int nGroup) const
	{
		if (nGroup < 0 || nGroup >= m_aFrameGroup.GetSize())
			return -1;
		return m_aFrameGroup[nGroup].GetFrameCount();
	}

	SIZE KImageInfo::GetFrameSize(int nGroup, int nGroupFrame) const
	{
		return GetFrameSize(GetFrameIndex(nGroup, nGroupFrame));
	}

	DWORD KImageInfo::GetImageID(int nGroup, int nGroupFrame) const
	{
		return GetImageID(GetFrameIndex(nGroup, nGroupFrame));
	}

	RECT KImageInfo::GetImageRect(int nGroup, int nGroupFrame) const
	{
		return GetImageRect(GetFrameIndex(nGroup, nGroupFrame));
	}

	int	KImageInfo::GetImageFrame(int nGroup, int nGroupFrame) const
	{
		return GetImageFrame(GetFrameIndex(nGroup, nGroupFrame));
	}

	bool KImageInfo::IsImagePixelAlpha(int nGroup, int nGroupFrame, int nX, int nY) const
	{
		return IsImagePixelAlpha(GetFrameIndex(nGroup, nGroupFrame), nX, nY);
	}

	int KImageInfo::GetFrameIndex(int nGroup, int nGroupFrame) const
	{
		int const nGroupFrameCount = GetFrameCount(nGroup);

		if (nGroupFrame < 0 || nGroupFrameCount < 0 || nGroupFrame >= nGroupFrameCount)
			return -1;

		return m_aFrameGroup[nGroup].GetFrameIndex(nGroupFrame);
	}

	bool KImageInfo::operator==(KImageInfo const& rhs) const
	{
		return m_STexFileInfo == rhs.m_STexFileInfo;
	}

}
