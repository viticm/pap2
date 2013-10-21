#include "stdafx.h"
#include "./kpointer.h"
#include "../KGUI.h"

namespace UI
{

	KPointer::KPointer()
		: m_nCurrentCursor(0)
		, m_bVisible(true)
	{
		for (int nIndex = CURSOR_BEGIN; nIndex != CURSOR_END; ++nIndex)
			m_aCursor[nIndex] = NULL;

        m_ImageArrow.dwImageID = 0;
		m_szArrowPath[0] = '\0';
	}


	KPointer::~KPointer()
	{
	}


	bool KPointer::Init()
	{
		BOOL bRetCode = FALSE;

		bRetCode = LoadAllCursor();
		KGLOG_PROCESS_ERROR(bRetCode);

		return true;
Exit0:
		Exit();
		return false;
	}


	void KPointer::Exit()
	{
		UnloadAllCursor();

		if (m_szArrowPath[0] != '\0')
		{
			g_pUI->m_ImageInfoMgr.UnloadImageInfo(m_szArrowPath);
		}
	}


	bool KPointer::ShowCursor(bool bShow)
	{
		if (m_bVisible == bShow)
			return true;

		m_bVisible = bShow;

		::ShowCursor(bShow);
		return true;
	}


	bool KPointer::IsCursorVisible() const
	{
		return m_bVisible;
	}


	bool KPointer::SetCursorPos(int nX, int nY)
	{
		POINT pt;

		pt.x = (LONG)nX;
		pt.y = (LONG)nY;

		::ClientToScreen(g_pUI->m_hWnd, &pt);
		::SetCursorPos(pt.x, pt.y);

		return true;
	}

	bool KPointer::GetCursorPos(int& nX, int& nY) const
	{
		POINT pt;

		::GetCursorPos(&pt);
		::ScreenToClient(g_pUI->m_hWnd, &pt);

        nX = pt.x;
		nY = pt.y;

        return true;
	}


	int	KPointer::SwitchCursor(int nIndex, int nIconID /*= -1*/)
	{
		if (nIndex >= CURSOR_BEGIN && nIndex < CURSOR_END)
		{
			if (nIconID >= 0)
				LoadCursor(nIndex, nIconID);

			m_nCurrentCursor = nIndex;
			if (m_aCursor[m_nCurrentCursor])
			{
				::SetCursor(m_aCursor[m_nCurrentCursor]);
			}
			else
			{
				::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
			}
		}
		else
		{
			m_nCurrentCursor = CURSOR_WINDOWS_DEAULT;
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
		}
		return m_nCurrentCursor;
	}


	int	KPointer::GetCurrentCursorIndex() const
	{
		return m_nCurrentCursor;
	}


	void KPointer::RestoreCursor()
	{
#if 0
		ATLTRACE("KPointer::RestoreCursor()\n");
#endif
		SwitchCursor(m_nCurrentCursor);
	}


	void KPointer::RestoreToWindowDefaultCursor()
	{
#if 0
		ATLTRACE("KPointer::RestoreToWindowDefaultCursor()\n");
#endif
		::SetCursor(NULL);
	}


	bool KPointer::IsExistCursor(int nIndex) const
	{
		if (nIndex < CURSOR_BEGIN || nIndex >= CURSOR_END)
			return false;
		if (m_aCursor[nIndex] == NULL)
			return false;
		return true;
	}


	bool KPointer::LoadCursor(int nIndex, LPCSTR szCursorFile)
	{
		BOOL bRetCode = FALSE;
		HCURSOR hLast = NULL;
		HCURSOR hNew = NULL;

		ASSERT(szCursorFile);

		bRetCode = nIndex >= CURSOR_BEGIN && nIndex < CURSOR_END;
		KGLOG_PROCESS_ERROR(bRetCode);

		hLast = m_aCursor[nIndex];

		bRetCode = KUiLoadCursor(szCursorFile, hNew);
		KG_PROCESS_ERROR(bRetCode);

		m_aCursor[nIndex] = hNew;

		if (m_nCurrentCursor == nIndex)
			::SetCursor(m_aCursor[m_nCurrentCursor]);

		if (hLast)
			::DestroyCursor(hLast);

		return true;
Exit0:
		if (hNew)
			::DestroyCursor(hNew);
		KGLogPrintf(KGLOG_ERR, "KGUI file %s\n", szCursorFile);
		return false;
	}

	bool KPointer::LoadCursor(int nIndex, int nIconID)
	{
		static BYTE pbyDataBuffer[HAND_OBJECT_CURSOR_WIDTH * HAND_OBJECT_CURSOR_HEIGHT * 4];
		static BYTE pbyXORDataBuffer[HAND_OBJECT_CURSOR_WIDTH * HAND_OBJECT_CURSOR_HEIGHT / 8];

		LPCSTR pcszImage = NULL;
		int nRetCode = false;
		int nLoaded = false;
		int nFrame = -1;
		bool bResult = false;
		KG3DUIImage2 aImage[2];
		HCURSOR hLast = NULL;
		HCURSOR hNew = NULL;
		KIconModel const* pIconModel = NULL;
		KImageInfo* pImageInfo = NULL;
		RECT rcSrc;
		SIZE sizeImage;

		memset(pbyXORDataBuffer, 0, sizeof(pbyXORDataBuffer));

		KG_PROCESS_ERROR(nIndex >= CURSOR_BEGIN && nIndex < CURSOR_END);
		KG_PROCESS_ERROR(nIconID >= 0);

		pIconModel = g_pUI->GetIconModel((DWORD)nIconID);
		KGLOG_PROCESS_ERROR(pIconModel);

		nRetCode = g_pUI->m_ImageInfoMgr.LoadImageInfo(GetIconFullPath(pIconModel->szFileName));
		KG_PROCESS_ERROR(nRetCode >= 0);

		nLoaded = true;

		pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(GetIconFullPath(pIconModel->szFileName));
		KG_PROCESS_ERROR(pImageInfo);
		KG_PROCESS_ERROR(pIconModel->dwFrame < (DWORD)pImageInfo->GetFrameCount());

		rcSrc = pImageInfo->GetImageRect(pIconModel->dwFrame);
		sizeImage = pImageInfo->GetImageSize();

		aImage[0].crDiffuse = 0xFF000000;
		aImage[0].dwImageID = pImageInfo->GetImageID(pIconModel->dwFrame);
		aImage[0].v2Min = D3DXVECTOR2(0.0f, 0.0f);
		aImage[0].v2Max = D3DXVECTOR2(HAND_OBJECT_CURSOR_WIDTH, HAND_OBJECT_CURSOR_HEIGHT);
		aImage[0].v2UVMin = D3DXVECTOR2((float)(rcSrc.left) / (float)sizeImage.cx, (float)(rcSrc.top) / (float)sizeImage.cy);
		aImage[0].v2UVMax = D3DXVECTOR2((float)(rcSrc.right) / (float)sizeImage.cx, (float)(rcSrc.bottom) / (float)sizeImage.cy);

		aImage[1] = m_ImageArrow;
		
		g_pUI->m_p3DUI->DrawMultiImageToBuffer(aImage, _countof(aImage),
			HAND_OBJECT_CURSOR_WIDTH, HAND_OBJECT_CURSOR_HEIGHT, 
			pbyDataBuffer, sizeof(pbyDataBuffer), FALSE);

		for (int i = 0; i < HAND_OBJECT_CURSOR_WIDTH * HAND_OBJECT_CURSOR_HEIGHT; ++i)
		{
			if (pbyDataBuffer[i * 4 + 3] == 0)
			{
				pbyXORDataBuffer[i / 8] |= (1 << (7 - (i % 8)));
			}
		}

		hLast = m_aCursor[nIndex];

		//hNew = CreateCursor(g_pUI->m_hInst, 0, 0, 32, 32, pbyDataBuffer, pbyXORDataBuffer);
		//KGLOG_PROCESS_ERROR(hNew);
		nRetCode = KUiLoadCursor(pbyDataBuffer, sizeof(pbyDataBuffer), pbyXORDataBuffer, sizeof(pbyXORDataBuffer), hNew);
			KGLOG_PROCESS_ERROR(nRetCode);

		m_aCursor[nIndex] = hNew;

		if (m_nCurrentCursor == nIndex)
			::SetCursor(m_aCursor[m_nCurrentCursor]);

		if (hLast)
			::DestroyCursor(hLast);

		bResult = true;
Exit0:
		if (!bResult && hNew)
			::DestroyCursor(hNew);
		if (nLoaded && pIconModel->szFileName)
			g_pUI->m_ImageInfoMgr.UnloadImageInfo(GetIconFullPath(pIconModel->szFileName));
		return bResult;
	}

	bool KPointer::UnloadCursor(int nIndex)
	{
		BOOL bRetCode = FALSE;

		bRetCode = nIndex >= CURSOR_BEGIN && nIndex < CURSOR_END;
		KGLOG_PROCESS_ERROR(bRetCode);

		if (m_nCurrentCursor == nIndex)
			::SetCursor(::LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));

		::DestroyCursor(m_aCursor[nIndex]);
		m_aCursor[nIndex] = NULL;
		return true;
Exit0:
		return false;
	}


	int KPointer::LoadAllCursor()
	{
		int nRetCode = false;
        bool bResult = false;
		IIniFile* pIni = NULL;
		char szBuffer[MAX_PATH];
		char szKeyName[32];
		int nFrame = 0;
		LPCSTR szFilePathCursorList = NULL;

		szBuffer[0] = '\0';
		szKeyName[0] = '\0';

		szFilePathCursorList = g_pUI->m_FilePathMgr.GetFilePath("CursorList");
		KGLOG_PROCESS_ERROR(szFilePathCursorList);

		pIni = g_OpenIniFile(szFilePathCursorList);
		KGLOG_PROCESS_ERROR(pIni);

		for (int nIndex = CURSOR_BEGIN; nIndex != CURSOR_END; ++nIndex)
		{
			_itot(nIndex, szKeyName, 10);

			pIni->GetString("CursorList", szKeyName, "", szBuffer, _countof(szBuffer));

			FormatFilePath(szBuffer);
			if (szBuffer[0] != '\0')
				LoadCursor(nIndex, szBuffer);
		}

		pIni->GetString("CursorList", "ArrowImg", "", m_szArrowPath, _countof(m_szArrowPath));
		FormatFilePath(m_szArrowPath);
		pIni->GetInteger("CursorList", "ArrowFrame", 0, &nFrame);
		if (m_szArrowPath[0] != '\0' && nFrame >= 0)
		{
			if (g_pUI->m_ImageInfoMgr.LoadImageInfo(m_szArrowPath) >= 0)
			{
				KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(m_szArrowPath);
				if (pImageInfo && nFrame < pImageInfo->GetFrameCount())
				{
					RECT const& rcSrc = pImageInfo->GetImageRect(nFrame);
					SIZE const& sizeImage = pImageInfo->GetImageSize();

					m_ImageArrow.crDiffuse = 0xFF000000;
					m_ImageArrow.dwImageID = pImageInfo->GetImageID(nFrame);
					m_ImageArrow.v2Min = D3DXVECTOR2(0.0f, 0.0f);
					m_ImageArrow.v2Max = D3DXVECTOR2((float)(rcSrc.right - rcSrc.left), (float)(rcSrc.bottom - rcSrc.top));
					m_ImageArrow.v2UVMin = D3DXVECTOR2((float)(rcSrc.left) / (float)sizeImage.cx, (float)(rcSrc.top) / (float)sizeImage.cy);
					m_ImageArrow.v2UVMax = D3DXVECTOR2((float)(rcSrc.right) / (float)sizeImage.cx, (float)(rcSrc.bottom) / (float)sizeImage.cy);
				}
			}
			else
			{
				m_szArrowPath[0] = '\0';
			}

		}

		bResult = true;
Exit0:
        KG_COM_RELEASE(pIni);
		return bResult;
	}


	bool KPointer::UnloadAllCursor()
	{
		bool bRet = false;
		for (int nIndex = CURSOR_BEGIN; nIndex != CURSOR_END; ++nIndex)
			bRet |= UnloadCursor(nIndex);
		return bRet;
	}

	int KPointer::KUiLoadCursor(const char* pszImgFile, HCURSOR &rhRetCursor)
	{
		int nResult = false;
		int nRetCode = false;
		char szTempPath[MAX_PATH];
		char szTempFileName[MAX_PATH];
		unsigned char TempBuffer[4096];
		int nProcessSize = 0;
		szTempPath[0] = '\0';
		szTempFileName[0] = '\0';

		IFile*		SrcCursorFile = NULL;
		int			nSrcFileSize = 0;

		IFile*      DstCursorFile = NULL;

		rhRetCursor = NULL;

		KGLOG_PROCESS_ERROR(pszImgFile);

		nRetCode = ::GetTempPathA(MAX_PATH, szTempPath);
		KGLOG_PROCESS_ERROR(nRetCode);

		nRetCode = ::GetTempFileNameA(szTempPath, "KSG_", 0, szTempFileName);
		KGLOG_PROCESS_ERROR(nRetCode);

		DstCursorFile = g_CreateFile(szTempFileName);
		KGLOG_PROCESS_ERROR(DstCursorFile);

		SrcCursorFile = g_OpenFile(pszImgFile);
		KGLOG_PROCESS_ERROR(SrcCursorFile);

		nSrcFileSize = SrcCursorFile->Size();

		while (nSrcFileSize > 0)
		{
			nProcessSize = SrcCursorFile->Read(TempBuffer, 4096);
			if (nProcessSize == 0)
				break;

			DstCursorFile->Write(TempBuffer, nProcessSize);

			nSrcFileSize -= nProcessSize;
		}

		if (SrcCursorFile)
		{
			SrcCursorFile->Close();
			SrcCursorFile->Release();
			SrcCursorFile = NULL;
		}

		if (DstCursorFile)
		{
			DstCursorFile->Close();
			DstCursorFile->Release();
			DstCursorFile = NULL;
		}

		rhRetCursor = ::LoadCursorFromFileA(szTempFileName);
		KGLOG_PROCESS_ERROR(rhRetCursor);

		nResult = true;
Exit0:

		if (SrcCursorFile)
		{
			SrcCursorFile->Close();
			SrcCursorFile->Release();
			SrcCursorFile = NULL;
		}

		if (DstCursorFile)
		{
			DstCursorFile->Close();
			DstCursorFile->Release();
			DstCursorFile = NULL;
		}

		if (szTempFileName[0])
		{
			::DeleteFile(szTempFileName);
			szTempFileName[0] = '\0';
		}

        if (!nResult)
        {
            KGLogPrintf(KGLOG_ERR, "[UI] KUiLoadCursor(%s)\n", pszImgFile);
        }
		return nResult;
	}

	int KPointer::KUiLoadCursor(BYTE *pbyData, size_t uDataLen, BYTE *pbyXOR, size_t uXORLen, HCURSOR &rhRetCursor)
	{
		int nResult = false;
		int nRetCode = false;
		IFile *fpCur = NULL;
		char szTempPath[MAX_PATH + 32];
		szTempPath[0] = '\0';

		KGLOG_PROCESS_ERROR(pbyData);
		KGLOG_PROCESS_ERROR(pbyXOR);

		nRetCode = ::GetTempPath(MAX_PATH, szTempPath);
		KGLOG_PROCESS_ERROR(nRetCode);
		szTempPath[MAX_PATH - 1] = '\0';
		strcat(szTempPath, "KSG_JX3_CURSOR_TEMP.CUR");
		szTempPath[_countof(szTempPath) - 1] = '\0';

		fpCur = g_CreateFile(szTempPath);
		if (!fpCur)
			KGLogPrintf(KGLOG_ERR, "KPointer::KUiLoadCursor, Failed to create temp cursor file:%s\n", szTempPath);
		KGLOG_PROCESS_ERROR(fpCur);

		fpCur->Write(&m_CurFileHeader, sizeof(KUSERSINGLEFRAMECURFILEHEADER)); //文件头
		fpCur->Write(pbyData, (unsigned long)uDataLen);	//图像数据
		fpCur->Write(pbyXOR, (unsigned long)uXORLen);	//异或数据

		if(fpCur)
		{
			fpCur->Close();
			fpCur->Release();
			fpCur = NULL;
		}

		rhRetCursor = ::LoadCursorFromFileA(szTempPath);
		if (!rhRetCursor)
		{
			DWORD dwError = GetLastError();
			LPVOID lpMsgBuf = NULL;
			::FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER | 
				FORMAT_MESSAGE_FROM_SYSTEM | 
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dwError ,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(LPTSTR) &lpMsgBuf,
				0,
				NULL 
			);
			KGLogPrintf(KGLOG_ERR, (const char *)lpMsgBuf);
			KGLogPrintf(KGLOG_ERR, "file name is:%s\n", szTempPath);
			if (::PathFileExistsA(szTempPath))
				KGLogPrintf(KGLOG_ERR, " use PathFileExistsA to detect the file is exist!\n");
			else
				KGLogPrintf(KGLOG_ERR, " use PathFileExistsA to detect the file is not exist!\n");
			::LocalFree(lpMsgBuf);
		}
		KGLOG_PROCESS_ERROR(rhRetCursor);

		nResult = true;
	Exit0:
		if(fpCur)
		{
			fpCur->Close();
			fpCur->Release();
			fpCur = NULL;
		}

//		if (szTempPath[0] != '\0')
//			::DeleteFile(szTempPath);

		return nResult;
	}


}

