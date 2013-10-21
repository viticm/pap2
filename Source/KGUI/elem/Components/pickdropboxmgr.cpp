////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : pickdropboxmgr.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-2-23 17:13:55
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "./pickdropboxmgr.h"
#include "./itembox.h"
#include "../../KGUI.h"

////////////////////////////////////////////////////////////////////////////////

namespace UI
{

KPickDropBoxMgr *KPickDropBoxMgr::ms_pSelf = NULL;

KPickDropBoxMgr::KPickDropBoxMgr()
{
	m_szImageFileName[0] = '\0';
	m_szAnimateFileName[0] = '\0';
	m_nSparkingAnimateGroup = -1;
	m_nInUseAnimateGroup = -1;
	m_nStaringAnimateGroup = -1;
	m_nImageIndex = -1;
	memset(m_aImage, 0, sizeof(m_aImage)); 
}

KPickDropBoxMgr::~KPickDropBoxMgr()
{

}

bool KPickDropBoxMgr::Init()
{
	bool bResult = false;
	int nRetCode = false;
	ASSERT(!ms_pSelf);
	ms_pSelf = new(std::nothrow) KPickDropBoxMgr;
	KGLOG_PROCESS_ERROR(ms_pSelf);

	nRetCode = ms_pSelf->LoadSource();
	KGLOG_PROCESS_ERROR(nRetCode);

	bResult = true;
Exit0:
	if (!bResult)
		Exit();
	return bResult;
}

void KPickDropBoxMgr::Exit()
{
	if (ms_pSelf)
		ms_pSelf->UnloadSource();
	delete ms_pSelf;
}

KPickDropBoxMgr &KPickDropBoxMgr::GetSelf()
{
	ASSERT(ms_pSelf);
	return *ms_pSelf;
}

int KPickDropBoxMgr::LoadSource()
{
	int nResult = false;
	int nRetCode = false;
	int nValue = 0;
	
	LPCSTR pcszConfigPath = NULL;
	IIniFile *pIni = NULL;
	
	pcszConfigPath = g_pUI->m_FilePathMgr.GetFilePath("PickDropBoxSetting");
	KGLOG_PROCESS_ERROR(pcszConfigPath);

	pIni = g_OpenIniFile(pcszConfigPath);
	KGLOG_PROCESS_ERROR(pIni);

	pIni->GetString("Setting", "ImagePath", "", m_szImageFileName, _countof(m_szImageFileName));
	FormatFilePath(m_szImageFileName);
	pIni->GetString("Setting", "AnimatePath", "", m_szAnimateFileName, _countof(m_szAnimateFileName));
	FormatFilePath(m_szAnimateFileName);	

	m_nImageIndex = g_pUI->m_ImageInfoMgr.LoadImageInfo(m_szImageFileName);
	KGLOG_PROCESS_ERROR(m_nImageIndex >= 0);

	pIni->GetInteger("Setting", "DisableFrame", -1, &nValue);
	FillImageData(DISABLE_IMAGE, nValue);

	pIni->GetInteger("Setting", "UnEquipableFrame", -1, &nValue);
	FillImageData(UNEQUIPABLE_IMAGE, nValue);

	pIni->GetInteger("Setting", "CoolDownFrame", -1, &nValue);
	FillImageData(COOLDOWN_IMAGE, nValue);
	FillCollingImageData();

	pIni->GetInteger("Setting", "SelectedFrame", -1, &nValue);
	FillImageData(SELECTED_IMAGE, nValue);

	pIni->GetInteger("Setting", "PressedFrame", -1, &nValue);
	FillImageData(PRESSED_IMAGE, nValue);

	pIni->GetInteger("Setting", "NoObjectMouseOverFrame", -1, &nValue);
	FillImageData(NO_OBJECT_MOUSEOVER_IMAGE, nValue);

	pIni->GetInteger("Setting", "MouseOverFrame", -1, &nValue);
	FillImageData(MOUSEOVER_IMAGE, nValue);

	pIni->GetInteger("Setting", "SparkingGroup", -1, &m_nSparkingAnimateGroup);	
	pIni->GetInteger("Setting", "InUseGroup", -1, &m_nInUseAnimateGroup);	
	pIni->GetInteger("Setting", "StaringGroup", -1, &m_nStaringAnimateGroup);	
	

	nResult = true;
Exit0:
	SAFE_RELEASE(pIni);
	return nResult;
}

int KPickDropBoxMgr::FillCollingImageData()
{
	int nResult = false;
	KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(m_nImageIndex);
	KG_PROCESS_ERROR(pImageInfo);

	m_CoolingImage.v2Min = m_aImage[COOLDOWN_IMAGE].v2Min;
	m_CoolingImage.v2Max = m_aImage[COOLDOWN_IMAGE].v2Max;
	m_CoolingImage.v2UVMin = m_aImage[COOLDOWN_IMAGE].v2UVMin;
	m_CoolingImage.v2UVMax = m_aImage[COOLDOWN_IMAGE].v2UVMax;
	m_CoolingImage.dwImageID = m_aImage[COOLDOWN_IMAGE].dwImageID;
	m_CoolingImage.crDiffuse = 0xFF000000;
	m_CoolingImage.dwType = KG3DUI_PART_TYPE_TIMER;

	nResult = true;
Exit0:
	return nResult;
}

int KPickDropBoxMgr::FillImageData(int nIndex, int nFrame)
{
	int nResult = false;
	int nRetCode = false;
	SIZE sizeImage;

	ASSERT(nIndex >= PICKDROPBOX_IMAGE_BEGIN && nIndex < PICKDROPBOX_IMAGE_COUNT);
	KG_PROCESS_ERROR(m_nImageIndex >= 0);

	KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(m_nImageIndex);
	KG_PROCESS_ERROR(pImageInfo);
	KG_PROCESS_ERROR(nFrame >= 0 && nFrame < pImageInfo->GetFrameCount());

	RECT const& rcSrc = pImageInfo->GetImageRect(nFrame);
	sizeImage = pImageInfo->GetImageSize();

	m_aImage[nIndex].v2UVMin = D3DXVECTOR2((float)(rcSrc.left) / (float)sizeImage.cx, (float)(rcSrc.top) / (float)sizeImage.cy);
	m_aImage[nIndex].v2UVMax = D3DXVECTOR2((float)(rcSrc.right) / (float)sizeImage.cx, (float)(rcSrc.bottom) / (float)sizeImage.cy);
	m_aImage[nIndex].dwImageID = pImageInfo->GetImageID(nFrame);
	m_aImage[nIndex].crDiffuse = 0xFF000000;

	nResult = true;
Exit0:
	return nResult;
}

int KPickDropBoxMgr::UnloadSource()
{
	if (m_nImageIndex != -1)
	{
		g_pUI->m_ImageInfoMgr.UnloadImageInfo(m_nImageIndex);
		m_nImageIndex = -1;
	}
	return true;
}

void KPickDropBoxMgr::DrawBoxAddOnLayer(
	float fX, float fY, float fWidth, float fHeight, DWORD dwBoxStatus, float fCoolingPercentage
)
{
	if (dwBoxStatus & ITEM_BOX_OBJECT_DISABLE)
	{
		m_aImage[DISABLE_IMAGE].v2Min = D3DXVECTOR2(fX, fY);
		m_aImage[DISABLE_IMAGE].v2Max = D3DXVECTOR2(fX + fWidth, fY + fHeight);
		g_pUI->m_p3DUI->DrawMultiImage(&m_aImage[DISABLE_IMAGE], 1);
	}
	if (dwBoxStatus & ITEM_BOX_OBJECT_UNEQUIPABLE)
	{
		m_aImage[UNEQUIPABLE_IMAGE].v2Min = D3DXVECTOR2(fX, fY);
		m_aImage[UNEQUIPABLE_IMAGE].v2Max = D3DXVECTOR2(fX + fWidth, fY + fHeight);
		g_pUI->m_p3DUI->DrawMultiImage(&m_aImage[UNEQUIPABLE_IMAGE], 1);
	}
	if (dwBoxStatus & ITEM_BOX_OBJECT_SELECTED)
	{
		m_aImage[SELECTED_IMAGE].v2Min = D3DXVECTOR2(fX, fY);
		m_aImage[SELECTED_IMAGE].v2Max = D3DXVECTOR2(fX + fWidth, fY + fHeight);
		g_pUI->m_p3DUI->DrawMultiImage(&m_aImage[SELECTED_IMAGE], 1);
	}
	if (dwBoxStatus & ITEM_BOX_OBJECT_COOLINGDOWN)
	{	
		m_CoolingImage.v2Min = D3DXVECTOR2(fX, fY);
		m_CoolingImage.v2Max = D3DXVECTOR2(fX + fWidth, fY + fHeight);
		m_CoolingImage.fRatio = fCoolingPercentage;

		g_pUI->m_p3DUI->DrawMultiImagePart(&m_CoolingImage, 1);
	}

	if (dwBoxStatus & ITEM_BOX_OBJECT_PRESSED)
	{
		m_aImage[PRESSED_IMAGE].v2Min = D3DXVECTOR2(fX, fY);
		m_aImage[PRESSED_IMAGE].v2Max = D3DXVECTOR2(fX + fWidth, fY + fHeight);
		g_pUI->m_p3DUI->DrawMultiImage(&m_aImage[PRESSED_IMAGE], 1);
	}
	else if (dwBoxStatus & ITEM_BOX_OBJECT_MOUSEOVER)
	{
		m_aImage[MOUSEOVER_IMAGE].v2Min = D3DXVECTOR2(fX, fY);
		m_aImage[MOUSEOVER_IMAGE].v2Max = D3DXVECTOR2(fX + fWidth, fY + fHeight);
		g_pUI->m_p3DUI->DrawMultiImage(&m_aImage[MOUSEOVER_IMAGE], 1);
	}

}

void KPickDropBoxMgr::DrawBoxMouseOverLayer(float fX, float fY, float fWidth, float fHeight)
{
	m_aImage[NO_OBJECT_MOUSEOVER_IMAGE].v2Min = D3DXVECTOR2(fX, fY);
	m_aImage[NO_OBJECT_MOUSEOVER_IMAGE].v2Max = D3DXVECTOR2(fX + fWidth, fY + fHeight);
	g_pUI->m_p3DUI->DrawMultiImage(&m_aImage[NO_OBJECT_MOUSEOVER_IMAGE], 1);
}

int KPickDropBoxMgr::AppendSparkingAnimate()
{
	int nAnimateIndex = -1;

    nAnimateIndex = g_pUI->m_AnimateImageMgr.LoadFile(m_szAnimateFileName);
	KGLOG_PROCESS_ERROR(nAnimateIndex >= 0);

	g_pUI->m_AnimateImageMgr.SetGroup(nAnimateIndex, m_nSparkingAnimateGroup);
	g_pUI->m_AnimateImageMgr.SetFrame(nAnimateIndex, 0);
	g_pUI->m_AnimateImageMgr.SetLoopCount(nAnimateIndex, 1);

Exit0:
	return nAnimateIndex;
}

int KPickDropBoxMgr::RemoveSparkingAnimate(int nAnimateIndex)
{
	g_pUI->m_AnimateImageMgr.UnloadFile(nAnimateIndex);
	return true;
}

int KPickDropBoxMgr::AppendInUseAnimate()
{
	int nAnimateIndex = -1;

	nAnimateIndex = g_pUI->m_AnimateImageMgr.LoadFile(m_szAnimateFileName);
	KGLOG_PROCESS_ERROR(nAnimateIndex >= 0);

	g_pUI->m_AnimateImageMgr.SetGroup(nAnimateIndex, m_nInUseAnimateGroup);
	g_pUI->m_AnimateImageMgr.SetFrame(nAnimateIndex, 0);
	g_pUI->m_AnimateImageMgr.SetLoopCount(nAnimateIndex, -1);

Exit0:
	return nAnimateIndex;
}

int KPickDropBoxMgr::RemoveInUseAnimate(int nAnimateIndex)
{
	g_pUI->m_AnimateImageMgr.UnloadFile(nAnimateIndex);
	return true;
}

int KPickDropBoxMgr::AppendStaringAnimate()
{
	int nAnimateIndex = -1;

	nAnimateIndex = g_pUI->m_AnimateImageMgr.LoadFile(m_szAnimateFileName);
	KGLOG_PROCESS_ERROR(nAnimateIndex >= 0);

	g_pUI->m_AnimateImageMgr.SetGroup(nAnimateIndex, m_nStaringAnimateGroup);
	g_pUI->m_AnimateImageMgr.SetFrame(nAnimateIndex, 0);
	g_pUI->m_AnimateImageMgr.SetLoopCount(nAnimateIndex, -1);

Exit0:
	return nAnimateIndex;
}

int KPickDropBoxMgr::RemoveStaringAnimate(int nAnimateIndex)
{
	g_pUI->m_AnimateImageMgr.UnloadFile(nAnimateIndex);
	return true;
}


}
