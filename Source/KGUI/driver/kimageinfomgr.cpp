#include "stdafx.h"
#include "./kimageinfomgr.h"

namespace UI
{

	KImageInfoMgr::KImageInfoMgr()
	{
	}


	KImageInfoMgr::~KImageInfoMgr()
	{
		ASSERT(m_aFilePathToImageInfoIndex.empty());
		ASSERT(m_aImageInfo.GetUsedCount() == 0);
	}


	bool KImageInfoMgr::Init()
	{
		return true;
	}


	void KImageInfoMgr::Exit()
	{
	}


	KImageInfo* KImageInfoMgr::GetImageInfo(LPCSTR szFilePath)
	{
		int nIndex = GetImageInfoIndex(szFilePath);
		if (nIndex == -1)
			return NULL;
		return GetImageInfo(nIndex);
	}

	
	int	KImageInfoMgr::GetImageInfoIndex(LPCSTR szFilePath) const
	{
		BOOL bRetCode = FALSE;
		KFilePathToImageInfoIndex::const_iterator it;

		bRetCode = szFilePath != NULL && szFilePath[0] != '\0';
		KGLOG_PROCESS_ERROR(bRetCode);

		if (*szFilePath == '\\')
			szFilePath++;

		it = m_aFilePathToImageInfoIndex.find(szFilePath);
		KG_PROCESS_ERROR(it != m_aFilePathToImageInfoIndex.end());

		return (*it).second;
Exit0:
		return -1;
	}


	KImageInfo* KImageInfoMgr::GetImageInfo(int nIndex)
	{
		return m_aImageInfo.GetAt(nIndex);
	}


	bool KImageInfoMgr::UnloadImageInfo(int nIndex)
	{
		BOOL bRetCode = FALSE;
		KImageInfo* pInfo = NULL;

		pInfo = GetImageInfo(nIndex);
		KGLOG_PROCESS_ERROR(pInfo);

		if (m_aImageInfo.GetRefCount(nIndex) == 1)
		{
			pInfo->Unload();

			bRetCode = m_aImageInfo.Remove(nIndex);
			KGLOG_CHECK_ERROR(bRetCode); 

			KFilePathToImageInfoIndex::iterator it = m_aFilePathToImageInfoIndex.begin();
			KFilePathToImageInfoIndex::iterator ie = m_aFilePathToImageInfoIndex.end();
			for (; it != ie; ++it)
			{
				if ((*it).second == nIndex)
				{
					m_aFilePathToImageInfoIndex.erase(it);
					break;
				}
			}
		}
		else
		{
			bRetCode = !m_aImageInfo.Remove(nIndex);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
		return true;
Exit0:
		KGLogPrintf(KGLOG_ERR, "KGUI KImageInfoMgr::UnloadImageInfo(%d)\tRefCnt=%d\n", nIndex, m_aImageInfo.GetRefCount(nIndex));
		return false;
	}

	int KImageInfoMgr::LoadImageInfo(LPCSTR szFilePath)
	{
		BOOL bRetCode = FALSE;
		int nIndex = -1;

		bRetCode = szFilePath != NULL && szFilePath[0] != '\0';
		KGLOG_PROCESS_ERROR(bRetCode);

        if (szFilePath[0] == '\\')
            szFilePath++;

		nIndex = GetImageInfoIndex(szFilePath);
		if (nIndex != -1)
		{	
			// Exist
			bRetCode = m_aImageInfo.AddRefCount(nIndex);
			KGLOG_PROCESS_ERROR(bRetCode);
		}
		else
		{
			KImageInfo ImageInfo;
			bRetCode = ImageInfo.LoadFromFile(szFilePath);
			KGLOG_PROCESS_ERROR(bRetCode);

			nIndex = m_aImageInfo.Append(ImageInfo);
			if (nIndex == -1)
			{
				ImageInfo.Unload();
				KGLogPrintf(KGLOG_ERR, "KGUI KImageInfoMgr::LoadImageInfo(%s) m_aImageInfo.Append() failed", szFilePath);
			}
			else
			{
				m_aFilePathToImageInfoIndex.insert(make_pair(std::string(szFilePath), nIndex));
			}
		}

        return nIndex;
Exit0:
		KGLogPrintf(KGLOG_ERR, "KGUI KImageInfoMgr::LoadImageInfo(%s)", szFilePath);
		return -1;
	}


	bool KImageInfoMgr::UnloadImageInfo(LPCSTR szFilePath)
	{
		BOOL bRetCode = FALSE;
		int nIndex = -1;
		KFilePathToImageInfoIndex::iterator it;

		bRetCode = szFilePath && szFilePath[0] != '\0';
		KGLOG_PROCESS_ERROR(bRetCode);

		if (*szFilePath == '\\')
			szFilePath++;

		it = m_aFilePathToImageInfoIndex.find(szFilePath);
		KGLOG_PROCESS_ERROR(it != m_aFilePathToImageInfoIndex.end());

		nIndex = (*it).second;
		KGLOG_PROCESS_ERROR(nIndex != -1);

		bRetCode = UnloadImageInfo(nIndex);
		KGLOG_PROCESS_ERROR(bRetCode);

		return true;
Exit0:
		KGLogPrintf(KGLOG_ERR, "KGUI KImageInfoMgr::UnloadImageInfo(%s)\n", szFilePath);
		return false;
	}

	LPCSTR KImageInfoMgr::GetImagePath(int nIndex) const
	{
		if (nIndex < 0)
			return NULL;

		KFilePathToImageInfoIndex::const_iterator it = m_aFilePathToImageInfoIndex.begin();
		KFilePathToImageInfoIndex::const_iterator ie = m_aFilePathToImageInfoIndex.end();
		for (; it != ie; ++it)
		{
			if ((*it).second != nIndex)
				continue;
			return (*it).first.c_str();
		}
		return NULL;
	}

}

