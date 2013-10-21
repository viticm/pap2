#include "stdafx.h"
#include "./kanimateimagemgr.h"
#include "./kimageinfo.h"
#include "../KGUI.h"


namespace UI
{

	KAnimateImageMgr::KAnimateImageMgr()
	{
	}

	KAnimateImageMgr::~KAnimateImageMgr()
	{
	}

	bool KAnimateImageMgr::Init()
	{
		return true;
	}

	void KAnimateImageMgr::Exit()
	{
	}

	bool KAnimateImageMgr::Activate()
	{
		bool bRetCode = true;

		DWORD dwElapsed = KG_GetTickCount();

		int nCount = GetCount();
		for (int nIndex = 0; nIndex < nCount; ++nIndex)
		{
			KAnimateImage* pAnimateImage = m_aAnimateImage.GetAt(nIndex);
			if (pAnimateImage)
			{
				pAnimateImage->Activate(dwElapsed);
			}
		}

		return bRetCode;
	}

	int KAnimateImageMgr::LoadFile(LPCSTR szFilePath)
	{
		BOOL bRetCode = FALSE;
		int nIndex = -1;
		int nImageInfoIndex = -1;
		KAnimateImage AnimateImage;

        nImageInfoIndex = g_pUI->m_ImageInfoMgr.LoadImageInfo(szFilePath);
		KGLOG_PROCESS_ERROR(nImageInfoIndex != -1);

		AnimateImage.SetImageInfoIndex(nImageInfoIndex);

		return m_aAnimateImage.Append(AnimateImage);
Exit0:
		if (szFilePath && szFilePath[0])
	        KGLogPrintf(KGLOG_ERR, "KGUI KAnimateImageMgr::LoadFile(%s)\n", szFilePath);
		return -1;
	}

	void KAnimateImageMgr::UnloadFile(int nIndex)
	{
		BOOL bRetCode = FALSE;
		int nImageInfoIndex = -1;

		nImageInfoIndex = GetImageInfoIndex(nIndex);
		KGLOG_PROCESS_ERROR(nImageInfoIndex >= 0);

		bRetCode = m_aAnimateImage.Remove(nIndex);
		KGLOG_PROCESS_ERROR(bRetCode);

		bRetCode = g_pUI->m_ImageInfoMgr.UnloadImageInfo(nImageInfoIndex);
		KGLOG_PROCESS_ERROR(bRetCode);

		return;
Exit0:
		return;
	}

	int	KAnimateImageMgr::GetCount() const
	{
		return static_cast<int>(m_aAnimateImage.GetTotalCount());
	}

	void KAnimateImageMgr::SetGroup(int nIndex, int nGroup)
	{
		bool bRetCode = false;
		int nImageInfoIndex = -1;
		int nFrameCount = 0;
		KAnimateImage* pAnimateImage = NULL;
		KImageInfo const* pImageInfo = NULL;

		pAnimateImage = m_aAnimateImage.GetAt(nIndex);
		KGLOG_PROCESS_ERROR(pAnimateImage);

		pAnimateImage->SetGroup(nGroup);

		nImageInfoIndex = pAnimateImage->GetImageInfoIndex();
		KGLOG_PROCESS_ERROR(nImageInfoIndex != -1);
		
		pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(nImageInfoIndex);
		KGLOG_PROCESS_ERROR(pImageInfo);

		nFrameCount = pImageInfo->GetFrameCount(nGroup);
		KGLOG_PROCESS_ERROR(nFrameCount >= 0);

		pAnimateImage->SetTotalFrame(nFrameCount);

		if (pAnimateImage->GetFrame() >= nFrameCount)
			pAnimateImage->SetFrame(nFrameCount - 1);

		return;
Exit0:
		return;
	}

	void KAnimateImageMgr::SetFrame(int nIndex, int nFrame)
	{
		KAnimateImage* pAnimateImage = m_aAnimateImage.GetAt(nIndex);
		ASSERT(pAnimateImage);
		pAnimateImage->SetFrame(nFrame);
	}

	void KAnimateImageMgr::SetTotalFrame(int nIndex, int nTotalFrame)
	{
		KAnimateImage* pAnimateImage = m_aAnimateImage.GetAt(nIndex);
		ASSERT(pAnimateImage);
		if (nTotalFrame < 0)
			nTotalFrame = 0;
		pAnimateImage->SetTotalFrame(nTotalFrame);
	}

	void KAnimateImageMgr::SetInterval(int nIndex, int nInterval)
	{
		KAnimateImage* pAnimateImage = m_aAnimateImage.GetAt(nIndex);
		ASSERT(pAnimateImage);
		pAnimateImage->SetInterval(nInterval);
	}

	void KAnimateImageMgr::SetLoopCount(int nIndex, int nLoopCount)
	{
		KAnimateImage* pAnimateImage = m_aAnimateImage.GetAt(nIndex);
		ASSERT(pAnimateImage);
		if (nLoopCount < 0)
			nLoopCount = -1;
		pAnimateImage->SetLoopCount(nLoopCount);
	}

	int KAnimateImageMgr::GetImageInfoIndex(int nIndex) const
	{
		KAnimateImage const* pAnimateImage = NULL;

		pAnimateImage = m_aAnimateImage.GetAt(nIndex);
		KGLOG_PROCESS_ERROR(pAnimateImage);

		return pAnimateImage->GetImageInfoIndex();
Exit0:
		KGLogPrintf(KGLOG_ERR, "KGUI KAnimateImageMgr::GetImageInfoIndex(%d)\n", nIndex);
		return -1;
	}

    int KAnimateImageMgr::GetFrameIndex(int nIndex) const
    {
		int nImageInfoIndex = 0;
		int nGroup = 0;
		int nFrame = 0;
		KAnimateImage const* pAnimateImage = NULL;
		KImageInfo const* pImageInfo = NULL;

		pAnimateImage = m_aAnimateImage.GetAt(nIndex);
		KGLOG_PROCESS_ERROR(pAnimateImage);

        nImageInfoIndex = pAnimateImage->GetImageInfoIndex();
		nGroup = pAnimateImage->GetGroup();
		nFrame = pAnimateImage->GetFrame();

		pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(nImageInfoIndex);
		KGLOG_PROCESS_ERROR(pImageInfo);

        return pImageInfo->GetFrameIndex(nGroup, nFrame);
Exit0:
		return -1;
    }

	bool KAnimateImageMgr::IsFinished(int nIndex) const
	{
		KAnimateImage const* pAnimateImage = m_aAnimateImage.GetAt(nIndex);
		if (pAnimateImage)
			return pAnimateImage->IsFinished() != 0 ? true : false;
		return true;
	}

	bool KAnimateImageMgr::Draw(int nIndex, float fX, float fY, float fWidth, float fHeight, DWORD dwAlpha) const
	{
		int nImageInfoIndex = 0;
		int nGroup = 0;
		int nFrame = 0;
		RECT rcSrc;
		SIZE sizeSrcImage;
		KG3DUIImage2 Image;
		KAnimateImage const* pAnimateImage = NULL;
		KImageInfo const* pImageInfo = NULL;
		
		pAnimateImage = m_aAnimateImage.GetAt(nIndex);
		KG_PROCESS_ERROR(pAnimateImage);

		nImageInfoIndex = pAnimateImage->GetImageInfoIndex();
		nGroup = pAnimateImage->GetGroup();
		nFrame = pAnimateImage->GetFrame();

		// 通过图片名字查找图片信息
		pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(nImageInfoIndex);
		KG_PROCESS_ERROR(pImageInfo);

		// OPTIMIZE:
		rcSrc = pImageInfo->GetImageRect(nGroup, nFrame);
		sizeSrcImage = pImageInfo->GetImageSize();

		ASSERT(fWidth >= 0.0f && fHeight >= 0.0f);

		Image.dwImageID = pImageInfo->GetImageID(nGroup, nFrame);
		Image.v2Min = D3DXVECTOR2(fX, fY);
		Image.v2Max = D3DXVECTOR2(fX + fWidth, fY + fHeight);
		Image.v2UVMin = D3DXVECTOR2((float)rcSrc.left / sizeSrcImage.cx, (float)rcSrc.top / sizeSrcImage.cy);
		Image.v2UVMax = D3DXVECTOR2((float)rcSrc.right / sizeSrcImage.cx, (float)rcSrc.bottom / sizeSrcImage.cy);
		Image.crDiffuse = dwAlpha << 24; //0xFF000000;

		// 画图
		g_pUI->m_p3DUI->DrawMultiImage(&Image, 1);
		return true;
Exit0:
		return false;
	}

}
