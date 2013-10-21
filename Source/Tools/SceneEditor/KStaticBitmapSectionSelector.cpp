#include "stdafx.h"
#include "KGLog.h"
#include "KStaticBitmapSectionSelector.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


KStaticBitmapSectionSelector::BitmapBuffer::BitmapBuffer()
{
	dwSize = 0;
	pData = NULL;
}

KStaticBitmapSectionSelector::BitmapBuffer::~BitmapBuffer()
{
	dwSize = 0;
	SAFE_DELETE_ARRAY(pData);
}

KStaticBitmapSectionSelector::KStaticBitmapSectionSelector(void)
{
	m_pStaticControl = NULL;
	m_nSectionInX = 1;
	m_nSectionInY = 1;
	m_dwSelection = 0xcf808080;
	m_bTgaLoaded = FALSE;
}

KStaticBitmapSectionSelector::~KStaticBitmapSectionSelector(void)
{
}

void KStaticBitmapSectionSelector::GetStaticControlDimension(int& nWidth, int& nHeight)
{
	RECT rect;
	KG_PROCESS_ERROR(m_pStaticControl);

	m_pStaticControl->GetClientRect(&rect);
	nWidth = abs(rect.right - rect.left);
	nHeight = abs(rect.bottom - rect.top);

Exit0:
	return;
}

void KStaticBitmapSectionSelector::SetStaticControl(CStatic* pStatic)
{
	CDC* pDc = NULL;
	int nWidth = 0;
	int nHeight = 0;

	KG_PROCESS_ERROR(pStatic);
	m_pStaticControl = pStatic;

	GetStaticControlDimension(nWidth, nHeight);

	pDc = m_pStaticControl->GetDC();
	KG_PROCESS_ERROR(pDc);


	m_Bitmap.DeleteObject();
	m_Bitmap.CreateCompatibleBitmap(pDc, nWidth, nHeight);

	m_BitmapBuffer.~BitmapBuffer();
	m_BitmapBuffer.pData = new BYTE[nWidth * nHeight * 4];
	m_BitmapBuffer.dwSize = nWidth * nHeight * 4;

	m_BitmapOrg.~BitmapBuffer();
	m_BitmapOrg.pData = new BYTE[nWidth * nHeight * 4];
	m_BitmapOrg.dwSize = nWidth * nHeight * 4;


Exit0:

	if (pDc)
	{
		m_pStaticControl->ReleaseDC(pDc);
		pDc = NULL;
	}

	return;
}

void KStaticBitmapSectionSelector::SetSection(int nX, int nY)
{
	m_nSectionInX = nX;
	m_nSectionInY = nY;

	m_SelectedSection.resize(nX * nY);
//Exit0:
	return;
}

void KStaticBitmapSectionSelector::UpdateControl()
{
	KG_PROCESS_ERROR(m_pStaticControl);

	m_Bitmap.SetBitmapBits(m_BitmapBuffer.dwSize, m_BitmapBuffer.pData);
	m_pStaticControl->SetBitmap(m_Bitmap);

Exit0:
	return;
}

void KStaticBitmapSectionSelector::OnSelect(POINT* pSelection,
											int nNumSelection,
											BOOL bClear)
{
	int nIndexX = 0;
	int nIndexY = 0;

	KG_PROCESS_ERROR(m_bTgaLoaded);

	if (bClear)
	{
		for (size_t i = 0; i < m_SelectedSection.size(); i++)
		{
			m_SelectedSection[i] = FALSE;
		}

	}

	memcpy(m_BitmapBuffer.pData, m_BitmapOrg.pData, sizeof(BYTE) * m_BitmapOrg.dwSize);
	for (int i = 0; i < nNumSelection; i++)
	{
		ConvertToIndex(pSelection[i], nIndexX, nIndexY);
		if (nIndexX < m_nSectionInX && nIndexY < m_nSectionInY)
		{
			m_SelectedSection[nIndexY * m_nSectionInX + nIndexX] = TRUE;
		}
	}

	for (int i = 0; i < m_nSectionInY; i++)
	{
		for (int j = 0; j < m_nSectionInX; j++)
		{
			if (m_SelectedSection[i * m_nSectionInX + j])
			{
				BlendSrcSelectionMask(j, i);
			}
		}
	}
	UpdateControl();

Exit0:
	return;
}

void KStaticBitmapSectionSelector::BlendSrcSelectionMask(int nX, int nY)
{
	RECT rect;
	int nWidth   = 0;
	int nHeight  = 0;
	float fDestAlpha = ((m_dwSelection & 0xff000000) >> 24) / 255.0f;
	float fSrcAlpha = 1.0f - fDestAlpha;
	BYTE byRed   = static_cast<BYTE>(((m_dwSelection & 0x00ff0000) >> 16) * fDestAlpha);
	BYTE byGreen = static_cast<BYTE>(((m_dwSelection & 0x0000ff00) >> 8) * fDestAlpha);
	BYTE byBlue  = static_cast<BYTE>((m_dwSelection & 0x000000ff) * fDestAlpha);

	KG_PROCESS_ERROR(m_BitmapOrg.pData);
	KG_PROCESS_ERROR(m_BitmapBuffer.pData);
	KG_PROCESS_ERROR(m_BitmapBuffer.dwSize == m_BitmapOrg.dwSize);

	GetStaticControlDimension(nWidth, nHeight);

	GetRectByIndex(nX, nY, rect);

	for (int i = rect.top; i < rect.bottom; i++)
	{
		for (int j = rect.left; j < rect.right; j++)
		{
			m_BitmapBuffer.pData[(i * nWidth + j) * 4] = m_BitmapOrg.pData[(i * nWidth + j) * 4];
			m_BitmapBuffer.pData[(i * nWidth + j) * 4 + 1] = static_cast<BYTE>(m_BitmapOrg.pData[(i * nWidth + j) * 4 + 1] * fSrcAlpha + byRed);
			m_BitmapBuffer.pData[(i * nWidth + j) * 4 + 2] = static_cast<BYTE>(m_BitmapOrg.pData[(i * nWidth + j) * 4 + 2] * fSrcAlpha + byGreen);
			m_BitmapBuffer.pData[(i * nWidth + j) * 4 + 3] = static_cast<BYTE>(m_BitmapOrg.pData[(i * nWidth + j) * 4 + 3] * fSrcAlpha + byBlue);
		}
	}
Exit0:
	return;
}

void KStaticBitmapSectionSelector::GetRectByIndex(int nX, int nY, RECT& rect)
{
	int nWidth = 0;
	int nHeight = 0;
	float fStrideX = 0.0f;
	float fStrideY = 0.0f;

	GetStaticControlDimension(nWidth, nHeight);

	memset(&rect, 0, sizeof(RECT));
	fStrideX = static_cast<float>(nWidth / m_nSectionInX);
	fStrideY = static_cast<float>(nHeight / m_nSectionInY);

	rect.left = static_cast<int>(nX * fStrideX);
	rect.right = static_cast<int>((nX + 1) * fStrideX);
	rect.top = static_cast<int>(nY * fStrideY);
	rect.bottom = static_cast<int>((nY + 1) * fStrideY);
}

void KStaticBitmapSectionSelector::ConvertToIndex(const POINT& pt, int& nX, int& nY)
{
	int nWidth = 0;
	int nHeight = 0;
	float fStrideX = 0.0f;
	float fStrideY = 0.0f;

	nX = 0;
	nY = 0;

	KG_PROCESS_ERROR(m_nSectionInX != 0);
	KG_PROCESS_ERROR(m_nSectionInY != 0);
	KG_PROCESS_ERROR(m_pStaticControl);

	GetStaticControlDimension(nWidth, nHeight);

	fStrideX = static_cast<float>(nWidth) / m_nSectionInX;
	fStrideY = static_cast<float>(nHeight) / m_nSectionInY;

	nY = static_cast<int>(pt.y / fStrideY);
	nX = static_cast<int>(pt.x / fStrideX);

Exit0:
	return;
}

void KStaticBitmapSectionSelector::SetBitmapData(BYTE* pData, DWORD dwSize)
{
	m_BitmapOrg.~BitmapBuffer();
	KG_PROCESS_ERROR(dwSize);


	m_BitmapOrg.pData = new BYTE[dwSize];
	KG_PROCESS_ERROR(m_BitmapOrg.pData);

	m_BitmapOrg.dwSize = dwSize;
	memcpy(m_BitmapOrg.pData, pData, dwSize * sizeof(BYTE));
	memcpy(m_BitmapBuffer.pData, pData, dwSize * sizeof(BYTE));

Exit0:
	return;
}

void KStaticBitmapSectionSelector::GetSelectSection(int** ppSelected, int& nNum)
{
	nNum = 0;
	KG_PROCESS_ERROR(ppSelected);

	SAFE_DELETE_ARRAY(*ppSelected);

	*ppSelected = new int[m_SelectedSection.size()];
	KG_PROCESS_ERROR(*ppSelected);

	for (size_t i = 0; i < m_SelectedSection.size(); i++)
	{
		if (m_SelectedSection[i])
		{
			(*ppSelected)[nNum] = static_cast<int>(i);
			nNum++;
		}
	}
Exit0:
	return;
}

HRESULT KStaticBitmapSectionSelector::LoadTga(LPCSTR strFileName,BOOL bAlpha /*= TRUE*/)
{
	HRESULT hr = E_FAIL;
	IFile* fpFile = NULL;
	unsigned long uReturn = 0;
	TGAHeader tgaHeader;
	BYTE* pData = NULL;
	BYTE* pBitmapBuffer = NULL;
	unsigned long uDataSize = 0;
	float fStepX = 0;
	float fStepY = 0;
	RECT rectBitmap;
	int nX = 0;
	int nY = 0;

	KG_PROCESS_ERROR(strFileName);
	KG_PROCESS_ERROR(m_pStaticControl);

	fpFile = g_OpenFile(strFileName);//fpFile = fopen(strFileName, "rb");
	KG_PROCESS_ERROR(fpFile);

	uReturn =fpFile->Read(&tgaHeader, sizeof(TGAHeader));//uReturn = fread(&tgaHeader, sizeof(TGAHeader), 1, fpFile);
	KG_PROCESS_ERROR(uReturn == sizeof(TGAHeader));
	KG_PROCESS_ERROR(tgaHeader.bitsperpixel == 32);
	uDataSize = tgaHeader.bitsperpixel * tgaHeader.height * tgaHeader.width / 8;
	KG_PROCESS_ERROR(uDataSize);
	pData = new BYTE[uDataSize];
	KG_PROCESS_ERROR(pData);


	size_t iasdfasdf = sizeof(TGAHeader);

	fpFile->Seek(tgaHeader.Id + sizeof(TGAHeader), SEEK_SET);//fseek(fpFile, tgaHeader.Id + sizeof(TGAHeader), SEEK_SET);
	uReturn=fpFile->Read(pData, uDataSize);//uReturn = fread(pData, uDataSize, 1, fpFile);
	KG_PROCESS_ERROR(uReturn == uDataSize);

	m_pStaticControl->GetClientRect(&rectBitmap);
	nX = abs(rectBitmap.right - rectBitmap.left);
	nY = abs(rectBitmap.top - rectBitmap.bottom);


	fStepY = tgaHeader.height / static_cast<float>(nY);
	fStepX = tgaHeader.width / static_cast<float>(nX);

	KG_PROCESS_ERROR(m_BitmapOrg.pData);
	KG_PROCESS_ERROR(m_BitmapOrg.dwSize);

	for (int i = 0; i < nY; i++)
	{
		int nY1 = static_cast<int>(i * fStepY);
		for (int j = 0; j < nX; j++)
		{
			int nX1 = static_cast<int>(j * fStepX);
			m_BitmapOrg.pData[((nY - i - 1) * nX + j) * 4] = pData[(nY1 * tgaHeader.width + nX1) * 4];
			m_BitmapOrg.pData[((nY - i - 1) * nX + j) * 4 + 1] = pData[(nY1 * tgaHeader.width + nX1) * 4 + 1];
			m_BitmapOrg.pData[((nY - i - 1) * nX + j) * 4 + 2] = pData[(nY1 * tgaHeader.width + nX1) * 4 + 2];
			if (bAlpha)
				m_BitmapOrg.pData[((nY - i - 1) * nX + j) * 4 + 3] = pData[(nY1 * tgaHeader.width + nX1) * 4 + 3];
			else
				m_BitmapOrg.pData[((nY - i - 1) * nX + j) * 4 + 3] = 255;
		}
	}

	memcpy(m_BitmapBuffer.pData, m_BitmapOrg.pData, m_BitmapOrg.dwSize);
	m_bTgaLoaded = TRUE;
	hr = S_OK;
Exit0:
	SAFE_DELETE_ARRAY(pData);

	SAFE_RELEASE(fpFile);
	
	return hr;
}

void KStaticBitmapSectionSelector::SetSelected(BYTE* pSelected, int nNum)
{
	int nSize = (int)m_SelectedSection.size();
	if(nSize<=0)
		return ;
	for(int n = 0;n< nSize;n++ )
	{
		m_SelectedSection[n] = FALSE;
	}
	for (int i = 0; i< nNum;i++)
	{
		int nIndex = pSelected[i];
		if(nIndex <nSize)
			m_SelectedSection[nIndex] = TRUE;
	}
	memcpy(m_BitmapBuffer.pData, m_BitmapOrg.pData, sizeof(BYTE) * m_BitmapOrg.dwSize);
	for (int i = 0; i < m_nSectionInY; i++)
	{
		for (int j = 0; j < m_nSectionInX; j++)
		{
			int nIndex = i * m_nSectionInX + j;
			if ((nIndex < nSize) && m_SelectedSection[nIndex])
			{
				BlendSrcSelectionMask(j, i);
			}
		}
	}
	UpdateControl();
}