#include "StdAfx.h"
#include "KG3DCaption.h"
#include "KG3DCamera.h"
#include "KG3DGraphicsTool.h"
#include "kg3dTexture.h"
#include "KG3DRenderState.h"
#include "kg3dtexturetable.h"

extern TCHAR g_szConfigFilePath[MAX_PATH];

KG3DCaption2D g_cCaption2D;

struct Vertex
{
	D3DXVECTOR3 vec3Pos;
	D3DXVECTOR2 vec2Tex;
	const static DWORD FVF = D3DFVF_XYZ | D3DFVF_TEX1;
};

struct Vertex2D
{
	D3DXVECTOR4 vec4Pos;
	DWORD dwColor;
	D3DXVECTOR2 vec2Tex;
	const static DWORD FVF = D3DFVF_XYZRHW | D3DFVF_TEX1 | D3DFVF_DIFFUSE;
};

#define SWAPWORD(x)		MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x)		MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

//static DWORD s_dwScale = 2;
static DWORD s_dwTextureBufferSize = 1024;// / s_dwScale;
static const DWORD s_dwInvalidateMgr = 0xffffffff;
static D3DXCOLOR crFontColor(1.0f, 1.0f, 1.0f, 1.0f);//字体在buffer上的颜色 一定是白色
const TCHAR *strSection = "Caption";
static int s_nMaxWordCount = 8;//单行的最大字符个数
Vertex Percentage[6];
const float KG3DCaptionTextureManager::s_fOffset = 1.0f / s_dwTextureBufferSize;
static TCHAR s_strDefaultFont[MAX_PATH];//字体加载失败时候使用的默认字体
LPD3DXFONT KG3DCaptionTextureManager::m_pFont = NULL;
HANDLE KG3DCaptionTextureManager::m_hFontHandle = 0;
std::string KG3DCaptionTextureManager::m_strFontName;
std::string KG3DCaptionTextureManager::m_strFontFaceName;
BOOL KG3DCaptionTextureManager::m_bBold;
BOOL KG3DCaptionTextureManager::m_bItalic;
int KG3DCaptionTextureManager::m_nFontSize;
DWORD KG3DCaptionTextureManager::m_dwBlockWidth;
DWORD KG3DCaptionTextureManager::m_dwBlockHeight;
DWORD KG3DCaptionTextureManager::m_dwNumCloumn;
DWORD KG3DCaptionTextureManager::m_dwNumRow;

HRESULT KG3DCaptionTextureManager::GetFontNameFromFile(LPCSTR lpszFilePath, 
													   LPSTR strFaceName,
													   size_t uSize)
{
	HRESULT hRetCode = E_FAIL;
	IFile *pFontFile = NULL;
	TT_OFFSET_TABLE ttOffsetTable;
	TT_TABLE_DIRECTORY tblDir;
	BOOL bFound = FALSE;

	KG_PROCESS_ERROR(lpszFilePath);
	KG_PROCESS_ERROR(strFaceName);

	strFaceName[0] = '\0';

	pFontFile = g_OpenFile(lpszFilePath);
	KG_PROCESS_ERROR(pFontFile);

	pFontFile->Read(&ttOffsetTable, sizeof(TT_OFFSET_TABLE));
	ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
	ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
	ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

	ZeroMemory(&tblDir, sizeof(TT_TABLE_DIRECTORY));

	//check is this is a true type font and the version is 1.0
	KG_PROCESS_ERROR(ttOffsetTable.uMajorVersion == 1 && ttOffsetTable.uMinorVersion == 0);

	for(int i=0; i< ttOffsetTable.uNumOfTables; i++)
	{
		TCHAR strTemp[5];
		pFontFile->Read(&tblDir, sizeof(TT_TABLE_DIRECTORY));
		strncpy_s(strTemp, tblDir.szTag, 4);
		if (!strcmpi(strTemp, "name"))
		{
			bFound = TRUE;
			tblDir.uLength = SWAPLONG(tblDir.uLength);
			tblDir.uOffset = SWAPLONG(tblDir.uOffset);
			break;
		}
	}

	if(bFound)
	{
		pFontFile->Seek(tblDir.uOffset, SEEK_SET);

		TT_NAME_TABLE_HEADER ttNTHeader;
		pFontFile->Read(&ttNTHeader, sizeof(TT_NAME_TABLE_HEADER));
		ttNTHeader.uNRCount = SWAPWORD(ttNTHeader.uNRCount);
		ttNTHeader.uStorageOffset = SWAPWORD(ttNTHeader.uStorageOffset);
		TT_NAME_RECORD ttRecord;
		for(int i=0; i<ttNTHeader.uNRCount; i++)
		{
			pFontFile->Read(&ttRecord, sizeof(TT_NAME_RECORD));
			ttRecord.uNameID = SWAPWORD(ttRecord.uNameID);
			if(ttRecord.uNameID == 1)
			{
				ttRecord.uStringLength = SWAPWORD(ttRecord.uStringLength);
				ttRecord.uStringOffset = SWAPWORD(ttRecord.uStringOffset);
				long lPos = pFontFile->Tell();
				pFontFile->Seek(tblDir.uOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset, SEEK_SET);

				memset(strFaceName, 0, uSize);
				_ASSERTE(ttRecord.uStringLength <= uSize);
				pFontFile->Read(strFaceName, ttRecord.uStringLength);
				if (strFaceName[0] != '\0')
				{
					hRetCode = S_OK;
					break;
				}
				pFontFile->Seek(lPos, SEEK_SET);
			}
		}			
	}
Exit0:
	SAFE_RELEASE(pFontFile);
	return hRetCode;
}

KG3DCaptionTextureManager::KG3DCaptionTextureManager() :
	m_pNextMgr(NULL),
	m_pTexture(NULL),
	m_bNeedClear(TRUE),
	m_dwMgrIndex(s_dwInvalidateMgr)
{

}

void KG3DCaptionTextureManager::UnInit()
{
	if (m_pNextMgr)
	{
		m_pNextMgr->UnInit();
		SAFE_DELETE(m_pNextMgr);
	}

	if (m_dwMgrIndex == 0)
	{
		SAFE_RELEASE(m_pFont);
		if (m_hFontHandle)
		{
			RemoveFontMemResourceEx(m_hFontHandle);
			m_hFontHandle = NULL;
		}
	}
	SAFE_RELEASE(m_pTexture);
}

HRESULT KG3DCaptionTextureManager::CreateFont()
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
	RECT rect;
	BYTE *pBuffer = NULL;
	TCHAR *strTest = "测";
	TCHAR strFaceName[MAX_PATH];
	IFile *pFontFile = NULL;
	hResult = GetFontNameFromFile(m_strFontName.c_str(), 
		strFaceName, 
		MAX_PATH);
	KG_COM_PROCESS_ERROR(hResult);
	
	m_strFontFaceName = strFaceName;

	pFontFile = g_OpenFile(m_strFontName.c_str());
	KG_PROCESS_ERROR(pFontFile);

	long lLength = pFontFile->Tell();
	pFontFile->Seek(0, SEEK_END);
	lLength = pFontFile->Tell() - lLength;

	pBuffer = new BYTE[lLength];
	KG_PROCESS_ERROR(pBuffer);

	pFontFile->Seek(0, SEEK_SET);
	unsigned long uReturn = pFontFile->Read(pBuffer, lLength);
	KG_PROCESS_ERROR(static_cast<unsigned long>(lLength) == uReturn);

	DWORD dwDummy = 0;
	m_hFontHandle = AddFontMemResourceEx((void*)pBuffer, lLength, 0, &dwDummy);
	KG_PROCESS_ERROR(m_hFontHandle);

	HDC hDC = GetDC( NULL );
	int nLogPixelsY = GetDeviceCaps(hDC, LOGPIXELSY);
	ReleaseDC( NULL, hDC );
	int nHeight = -m_nFontSize * nLogPixelsY / 72;
	hResult = D3DXCreateFont(g_pd3dDevice, 
		nHeight, 
		0, 
		m_bBold? FW_BOLD : FW_NORMAL,
		0,
		m_bItalic,
		DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS,
		DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE,
		m_strFontFaceName.c_str(),
		&m_pFont);
	if (FAILED(hResult))
	{
		KGLogPrintf(KGLOG_ERR, "KG3DCaption create font failed, use default font.\n");
		hResult = D3DXCreateFont(g_pd3dDevice, 
			nHeight, 
			0, 
			m_bBold? FW_BOLD : FW_NORMAL,
			0,
			m_bItalic,
			DEFAULT_CHARSET,
			OUT_DEFAULT_PRECIS,
			DEFAULT_QUALITY,
			DEFAULT_PITCH | FF_DONTCARE,
			s_strDefaultFont,
			&m_pFont);
	}
	KG_COM_PROCESS_ERROR(hResult);

	ZeroMemory(&rect, sizeof(RECT));
    m_pFont->DrawTextW(NULL, ATL::CT2W(strTest, 936), -1, &rect, DT_CALCRECT, crFontColor);

	m_dwBlockWidth = (rect.right - rect.left) * s_nMaxWordCount;
	m_dwBlockHeight = (rect.bottom - rect.top);
	m_dwNumCloumn = s_dwTextureBufferSize / m_dwBlockWidth;
	m_dwNumRow = s_dwTextureBufferSize / m_dwBlockHeight;

	hRetCode = S_OK;
Exit0:
	if (FAILED(hRetCode))
	{
		KGLogPrintf(KGLOG_ERR, "KG3DCaption create font failed.\n");
		
		if (m_hFontHandle)
		{
			RemoveFontMemResourceEx(m_hFontHandle);
			m_hFontHandle = NULL;
		}
	}
	SAFE_RELEASE(pFontFile);
	SAFE_DELETE_ARRAY(pBuffer);
	return hRetCode;
}

HRESULT KG3DCaptionTextureManager::Init(LPCSTR strFontFile, int nSize, BOOL bBold, BOOL bItalic, DWORD dwMgrIndex)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;

    ASSERT(m_pTexture == NULL);

	//创建贴图
	hResult = D3DXCreateTexture(g_pd3dDevice, 
		s_dwTextureBufferSize, 
		s_dwTextureBufferSize, 
		1, 
		D3DUSAGE_RENDERTARGET, 
		D3DFMT_A8,//R8G8B8, 
		D3DPOOL_DEFAULT, 
		&m_pTexture);
	if (FAILED(hResult))
	{
		hResult = D3DXCreateTexture(g_pd3dDevice, 
			s_dwTextureBufferSize, 
			s_dwTextureBufferSize, 
			1, 
			D3DUSAGE_RENDERTARGET, 
			D3DFMT_A8R8G8B8, 
			D3DPOOL_DEFAULT, 
			&m_pTexture);
	}
	KG_COM_PROCESS_ERROR(hResult);

	m_nFontSize = nSize;// / s_dwScale;
	m_bBold = bBold;
	m_bItalic = bItalic;
	m_strFontName = strFontFile;

	if (!m_pFont)
	{
		//字体只需要创建一次
		hResult = CreateFont();
		KG_COM_PROCESS_ERROR(hResult);
	}
	
	for (DWORD i = 0; i < m_dwNumCloumn * m_dwNumRow; i++)
	{
		m_FreeBlock.insert(i);
	}
	m_Data.resize(m_dwNumCloumn * m_dwNumRow);
	m_bNeedClear = TRUE;
	m_pNextMgr = NULL;
	m_dwMgrIndex = dwMgrIndex;//Mgr的Index， 单向链表逐个递增
	hRetCode = S_OK;
Exit0:
	if (FAILED(hRetCode))
	{
		SAFE_RELEASE(m_pTexture);
		KGLogPrintf(KGLOG_ERR, "%d, Create caption texture %d failed.", hRetCode, dwMgrIndex);
	}
	return hRetCode;
}

HRESULT KG3DCaptionTextureManager::OnLostDevice()
{
    SAFE_RELEASE(m_pTexture);
	if (m_pNextMgr)
	{
		m_pNextMgr->OnLostDevice();
	}
	if (m_dwMgrIndex == 0 && m_pFont)
	{
		m_pFont->OnLostDevice();
	}
    return S_OK;
}

HRESULT KG3DCaptionTextureManager::OnResetDevice()
{
    HRESULT hRetCode = E_FAIL;
    HRESULT hrResult = E_FAIL;

    _ASSERTE(m_pTexture == NULL);

	//重新创建贴图
	hrResult = D3DXCreateTexture(g_pd3dDevice, 
		s_dwTextureBufferSize, 
		s_dwTextureBufferSize, 
		1, 
		D3DUSAGE_RENDERTARGET, 
		D3DFMT_A8,
		D3DPOOL_DEFAULT, 
		&m_pTexture);
	if (FAILED(hrResult))
	{
		hrResult = D3DXCreateTexture(g_pd3dDevice, 
			s_dwTextureBufferSize, 
			s_dwTextureBufferSize, 
			1, 
			D3DUSAGE_RENDERTARGET, 
			D3DFMT_A8R8G8B8, 
			D3DPOOL_DEFAULT, 
			&m_pTexture);
	}
	KG_COM_PROCESS_ERROR(hrResult);

	//设置所有用到的字符块为需要绘制
	for (DWORD i = 0; i < static_cast<DWORD>(m_Data.size()); i++)
	{
		if (m_Data[i].State == IS_READY)
		{
			m_Data[i].State = IS_PENDING;
			m_DataPending.insert(i);
		}
	}
	
	//设置清除贴图
	m_bNeedClear = TRUE;

	if (m_pNextMgr)
	{
		m_pNextMgr->OnResetDevice();
	}
	
	if (m_dwMgrIndex == 0 && m_pFont)
	{
		m_pFont->OnResetDevice();
	}
    hRetCode = S_OK;
Exit0:
    return hRetCode;
}

DWORD KG3DCaptionTextureManager::FindContentNext(LPCSTR strContent, DWORD& dwIndex)
{
	DWORD dwReturn = CAPTION_INVALIDATE_INDEX;
	if (m_pNextMgr)
	{
		if (m_pNextMgr->m_StringToData.find(strContent) != m_pNextMgr->m_StringToData.end())
		{
			dwIndex = m_pNextMgr->m_dwMgrIndex;
			dwReturn = m_pNextMgr->m_StringToData[strContent];
			std::deque<DWORD>::iterator it = find(m_pNextMgr->m_Released.begin(), m_pNextMgr->m_Released.end(), dwReturn);
			if (it != m_pNextMgr->m_Released.end())
			{
				m_pNextMgr->m_Released.erase(it);
				m_pNextMgr->m_Data[dwReturn].State = IS_PENDING;
				m_pNextMgr->m_DataPending.insert(dwReturn);
			}
			m_pNextMgr->m_Data[dwReturn].nRef++;
		}
		else
		{
			dwReturn = m_pNextMgr->FindContentNext(strContent, dwIndex);
		}
	}
	return dwReturn;
}

DWORD KG3DCaptionTextureManager::Add(LPCSTR strContent, 
									 DWORD& dwMgrIndex)
{
	RECT rect;
	DWORD dwReturn = CAPTION_INVALIDATE_INDEX;
	DWORD dwResult = CAPTION_INVALIDATE_INDEX;
	KG_PROCESS_ERROR(m_pFont);
	dwMgrIndex = m_dwMgrIndex;
	//如果已经有了就直接返回
	if (m_StringToData.find(strContent) != m_StringToData.end())
	{
		dwReturn = m_StringToData[strContent];
		std::deque<DWORD>::iterator it = find(m_Released.begin(), m_Released.end(), dwReturn);
		if (it != m_Released.end())		
		{
			m_Released.erase(it);//从删除队列中移除
			m_Data[dwReturn].State = IS_PENDING;
			m_DataPending.insert(dwReturn);//放入pending队列
		}
		m_Data[dwReturn].nRef++;
		goto Exit1;
	}
	
	dwResult = FindContentNext(strContent, dwMgrIndex);
	if (dwResult != CAPTION_INVALIDATE_INDEX)
	{
		//在下级Mgr里找到了对应内容
		dwReturn = dwResult;
		goto Exit1;
	}
		
	m_pFont->DrawTextA(NULL, strContent, -1, &rect, DT_CALCRECT, crFontColor);

	//获取合适的贴图区域
	if (m_FreeBlock.size())
	{
		//直接使用空闲的
		dwReturn = *m_FreeBlock.begin();
		m_FreeBlock.erase(m_FreeBlock.begin());
	}
	else
	{
		if (m_Released.size())
		{
			//使用应该释放的
			dwReturn = *m_Released.begin();
			if (m_Data[dwReturn].nRef != 0)
			{
				KGLogPrintf(KGLOG_ERR, "Caption 使用了错误的贴图块.\n");
			}
			std::map<std::string, DWORD>::iterator it = m_StringToData.find(m_Data[dwReturn].strContent);
			m_StringToData.erase(it);
			m_Released.pop_front();
		}
		else
		{
			//如果已经完全没有空闲的也没有释放了的, 需要放入到下一个Mgr里面
			if (!m_pNextMgr)
			{
				m_pNextMgr = new KG3DCaptionTextureManager;
				KG_PROCESS_ERROR(m_pNextMgr);

				HRESULT hResult = m_pNextMgr->Init(m_strFontName.c_str(), m_nFontSize, m_bBold, m_bItalic, m_dwMgrIndex + 1);
				KG_COM_PROCESS_ERROR(hResult);
			}
			dwReturn = m_pNextMgr->Add(strContent, dwMgrIndex);
			goto Exit1;
		}
	}

	_ASSERTE(dwReturn != CAPTION_INVALIDATE_INDEX);
	m_StringToData[strContent] = dwReturn;
	ItemInfo &Block = m_Data[dwReturn];
	
	Block.strContent = strContent;
	_ASSERTE(Block.nRef == 0);
	Block.nRef++;
	Block.dwHeight = rect.bottom - rect.top;
	Block.dwWidth = rect.right - rect.left;
	Block.State = IS_PENDING;
	m_DataPending.insert(dwReturn);//放入pending队列
Exit1:
Exit0:
	return dwReturn;
}

void KG3DCaptionTextureManager::Remove(DWORD dwMgrIndex, 
									   DWORD dwBlockIndex)
{
	if (dwMgrIndex > m_dwMgrIndex)
	{
		_ASSERTE(m_pNextMgr);
		m_pNextMgr->Remove(dwMgrIndex, dwBlockIndex);
	}
	else
	{
		KG_PROCESS_ERROR(dwBlockIndex < m_Data.size());
		m_Data[dwBlockIndex].nRef--;
		if (m_Data[dwBlockIndex].nRef <= 0)
		{
			//释放这个块
			_ASSERTE(m_Data[dwBlockIndex].nRef == 0);//防止过度释放
			if (m_Data[dwBlockIndex].nRef < 0)
			{
				KGLogPrintf(KGLOG_ERR, "%s 过度释放.\n", m_Data[dwBlockIndex].strContent);
			}
			m_DataPending.erase(dwBlockIndex);
			m_Released.push_back(dwBlockIndex);
		}
	}
Exit0:
	;
}

float KG3DCaptionTextureManager::GetAspect()
{
	return static_cast<float>(m_dwBlockWidth) / m_dwBlockHeight;
}

HRESULT KG3DCaptionTextureManager::GetUVCoordinate(DWORD dwMgrIndex,
												   DWORD dwBlockIndex, 
												   D3DXVECTOR2 &vA, 
												   D3DXVECTOR2 &vC)
{
	HRESULT hRetCode = E_FAIL;
	if (dwMgrIndex > m_dwMgrIndex)
	{
		KG_PROCESS_ERROR(m_pNextMgr);

		hRetCode = m_pNextMgr->GetUVCoordinate(dwMgrIndex, dwBlockIndex, vA, vC);
		KG_PROCESS_SUCCESS(1);
	}

	KG_PROCESS_ERROR(dwBlockIndex < static_cast<DWORD>(m_Data.size()));
	DWORD dwRow = dwBlockIndex / m_dwNumCloumn;
	DWORD dwColumn = dwBlockIndex % m_dwNumCloumn;
	float fStartX = float(dwColumn * m_dwBlockWidth);
	float fStartY = float(dwRow * m_dwBlockHeight);
	vA.x = fStartX / s_dwTextureBufferSize + s_fOffset;
	vA.y = fStartY / s_dwTextureBufferSize + s_fOffset;
	vC.x = (fStartX + m_dwBlockWidth) / s_dwTextureBufferSize + s_fOffset;
	vC.y = (fStartY + m_dwBlockHeight) / s_dwTextureBufferSize + s_fOffset;
	hRetCode = S_OK;
Exit1:
Exit0:
	return hRetCode;
}

HRESULT KG3DCaptionTextureManager::SetTexture(DWORD dwStage,
											  DWORD dwIndex)
{
	HRESULT hRetCode = E_FAIL;
	if (dwIndex > m_dwMgrIndex)
	{
		KG_PROCESS_ERROR(m_pNextMgr);
		
		hRetCode = m_pNextMgr->SetTexture(dwStage, dwIndex);
	}
	else
	{
		_ASSERTE(!m_bNeedClear);
		hRetCode = g_pd3dDevice->SetTexture(0, m_pTexture);
	}
Exit0:
	return hRetCode;
}

HRESULT KG3DCaptionTextureManager::OnRender(bool bNeedRestore)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
	LPDIRECT3DSURFACE9 pDepthSave = NULL;
	LPDIRECT3DSURFACE9 pRTSave = NULL;
	LPDIRECT3DSURFACE9 pRT = NULL;
	std::set<DWORD>::iterator it = m_DataPending.begin();
	std::set<DWORD>::iterator itEnd = m_DataPending.end();
	D3DVIEWPORT9 vpSave;

	if (bNeedRestore)
	{
		hResult = g_pd3dDevice->GetViewport(&vpSave);
		KG_COM_PROCESS_ERROR(hResult);

		hResult = g_pd3dDevice->GetRenderTarget(0, &pRTSave);
		_ASSERTE(pRTSave);

		hResult = g_pd3dDevice->GetDepthStencilSurface(&pDepthSave);
		_ASSERTE(pDepthSave);
	}

	KG_PROCESS_SUCCESS(!m_DataPending.size());
	KG_PROCESS_ERROR(m_pFont);
	KG_PROCESS_ERROR(m_pTexture);

	hResult = m_pTexture->GetSurfaceLevel(0, &pRT);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = g_pd3dDevice->SetRenderTarget(0, pRT);
	KG_COM_PROCESS_ERROR(hResult);

	hResult = g_pd3dDevice->SetDepthStencilSurface(NULL);
	KG_COM_PROCESS_ERROR(hResult);

	if (m_bNeedClear)
	{
		hResult = g_pd3dDevice->Clear(0, NULL, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L);
	}

	while (it != itEnd)
	{
		DWORD dwBlockIndex = *it;
		ItemInfo &Info = m_Data[dwBlockIndex];
		RECT rect;
		DWORD dwRow = dwBlockIndex / m_dwNumCloumn;
		DWORD dwColumn = dwBlockIndex % m_dwNumCloumn;
		rect.left = dwColumn * m_dwBlockWidth;
		rect.top = dwRow * m_dwBlockHeight;
		rect.right = (dwColumn + 1) * m_dwBlockWidth;
		rect.bottom = (dwRow + 1) * m_dwBlockHeight;
		if (!m_bNeedClear)
		{
			//如果没有清空整个贴图， 需要清除一下要用到的区域
			D3DRECT d3dRect;
			d3dRect.x1 = rect.left;
			d3dRect.x2 = rect.right;
			d3dRect.y1 = rect.top;
			d3dRect.y2 = rect.bottom;
			g_pd3dDevice->Clear(1, &d3dRect, D3DCLEAR_TARGET, 0x00000000, 1.0f, 0L);
		}
		m_pFont->DrawTextW(NULL, ATL::CT2W(Info.strContent.c_str(), 936), -1, &rect, DT_NOCLIP | DT_CENTER, crFontColor);
		Info.State = IS_READY;
		it++;
	}
	
	m_DataPending.clear();
	m_bNeedClear = FALSE;

Exit1:
	if (m_pNextMgr)
	{
		m_pNextMgr->OnRender(false);
	}
	hRetCode = S_OK;
Exit0:
	if (bNeedRestore)
	{
		if (pRTSave)
		{
			hResult = g_pd3dDevice->SetRenderTarget(0, pRTSave);
			pRTSave->Release();
		}
		if (pDepthSave)
		{
			hResult = g_pd3dDevice->SetDepthStencilSurface(pDepthSave);
			pDepthSave->Release();
		}
		g_pd3dDevice->SetViewport(&vpSave);
	}
	SAFE_RELEASE(pRT);
	return hRetCode;
}


KG3DCaption2D::KG3DCaption2D():
m_pTextureMgr(NULL),
m_pFrontTexture(NULL),
m_pBackTexture(NULL),
m_fBackBorderColorScale(0.3f)
{

}

void KG3DCaption2D::UnInit()
{
	m_RenderData.clear();
	//存储血条的信息
	IIniFile *pConfigFile = pConfigFile = g_OpenIniFile(g_szConfigFilePath, false, true);
	int nResult = 0;
	KG_PROCESS_ERROR(pConfigFile);

	if (m_pFrontTexture)
	{
		pConfigFile->WriteString(strSection, "FrontTexture", m_pFrontTexture->m_scName.c_str());
	}
	if (m_pBackTexture)
	{
		pConfigFile->WriteString(strSection, "BackTexture", m_pBackTexture->m_scName.c_str());
	}

	nResult = pConfigFile->WriteFloat(strSection, "PercentageWidth", m_fPercentageWidth);
	nResult = pConfigFile->WriteFloat(strSection, "PercentageHeight", m_fPercentageHeight);
	nResult = pConfigFile->WriteFloat(strSection, "PercentageBorderXWidth", m_fBorderX);
	nResult = pConfigFile->WriteFloat(strSection, "PercentageBorderYWidth", m_fBorderY);
	nResult = pConfigFile->WriteFloat(strSection, "CaptionHeight", m_fCaptionHeight);
	nResult = pConfigFile->WriteInteger(strSection, "MaxCharactorPerLine", s_nMaxWordCount);
	nResult = pConfigFile->WriteFloat(strSection, "BackBorderColorScale", m_fBackBorderColorScale);
	nResult = pConfigFile->WriteInteger(strSection, "BackBorderColor", m_dwBackBorderColor);
	nResult = pConfigFile->WriteString(strSection, "DefaultFont", s_strDefaultFont);
	if (m_pTextureMgr)
	{
		nResult = pConfigFile->WriteString(strSection, "FontFile", m_pTextureMgr->m_strFontName.c_str());
		nResult = pConfigFile->WriteInteger(strSection, "FontSize", m_pTextureMgr->m_nFontSize /** s_dwScale*/);
		nResult = pConfigFile->WriteInteger(strSection, "FontBold", m_pTextureMgr->m_bBold);
		nResult = pConfigFile->WriteInteger(strSection, "FontItalic", m_pTextureMgr->m_bItalic);
	}

	pConfigFile->Save(g_szConfigFilePath);
Exit0:
	if (m_pTextureMgr)
	{
		m_pTextureMgr->UnInit();
		SAFE_DELETE(m_pTextureMgr);
	}
	SAFE_RELEASE(m_pFrontTexture);
	SAFE_RELEASE(m_pBackTexture);
	SAFE_RELEASE(pConfigFile);
}

HRESULT KG3DCaption2D::Init()
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
	TCHAR strFontFile[MAX_PATH];
	int nResult = 0;
	int nFontSize = 0;
	int nBackBorderColor = 0;
	BOOL bBold = FALSE;
	BOOL bItalic = FALSE;
	
	IIniFile *pConfigFile = NULL;
	TCHAR strFrontTexture[MAX_PATH];
	
	float fX = 0.5f;
	float fY = 1.0f;
	D3DXVECTOR2 vec2A(0.0f, 0.0f), vec2B(1.0f, 0.0f), vec2C(1.0f, 1.0f), vec2D(0.0f, 1.0f);
	
	{
		m_Vertex[0].vec3Pos = D3DXVECTOR3(-fX, fY, 0.0f);
		m_Vertex[1].vec3Pos = D3DXVECTOR3( fX, fY, 0.0f);
		m_Vertex[2].vec3Pos = D3DXVECTOR3( fX,       0, 0.0f);
		m_Vertex[3].vec3Pos = m_Vertex[0].vec3Pos;
		m_Vertex[4].vec3Pos = m_Vertex[2].vec3Pos;
		m_Vertex[5].vec3Pos = D3DXVECTOR3(-fX,       0, 0.0f);

		m_Vertex[0].vec2Tex0 = vec2A;
		m_Vertex[1].vec2Tex0 = vec2B;
		m_Vertex[2].vec2Tex0 = vec2C;
		m_Vertex[3].vec2Tex0 = vec2A;
		m_Vertex[4].vec2Tex0 = vec2C;
		m_Vertex[5].vec2Tex0 = vec2D;
	}

	//读取血条的设置
	SAFE_RELEASE(m_pFrontTexture);
	SAFE_RELEASE(m_pBackTexture);
	pConfigFile = g_OpenIniFile(g_szConfigFilePath);
	KG_PROCESS_ERROR(pConfigFile);

	nResult = pConfigFile->GetString(strSection, 
		"FrontTexture", 
		"data\\public\\hitpointspan.tga", 
		strFrontTexture, 
		MAX_PATH);

	if (strFrontTexture[0] == '\0')
	{
		strcpy_s(strFrontTexture, MAX_PATH, "data\\public\\hitpointspan.tga");
	}


	hResult = g_cTextureTable.LoadResourceFromFile(strFrontTexture, 
		0, 
		0, 
		(IKG3DResourceBase**)&m_pFrontTexture);
	KG_COM_PROCESS_ERROR(hResult);

	nResult = pConfigFile->GetString(strSection,
		"BackTexture",
		"data\\public\\hitpointspanback.tga",
		strFrontTexture,
		MAX_PATH);

	if (strFrontTexture[0] == '\0')
	{
		strcpy_s(strFrontTexture, MAX_PATH, "data\\public\\hitpointspanback.tga");
	}

	hResult = g_cTextureTable.LoadResourceFromFile(strFrontTexture, 
		0, 
		0, 
		(IKG3DResourceBase**)&m_pBackTexture);
	KG_COM_PROCESS_ERROR(hResult);
	
	nResult = pConfigFile->GetFloat(strSection, "PercentageWidth", 60.0f, &m_fPercentageWidth);
	if (m_fPercentageWidth == 0.0f)
	{
		m_fPercentageWidth = 60.0f;
	}

	nResult = pConfigFile->GetFloat(strSection, "PercentageHeight", 7.0f, &m_fPercentageHeight);
	if (m_fPercentageHeight == 0.0f)
	{
		m_fPercentageHeight = 7.0f;
	}
	
	nResult = pConfigFile->GetFloat(strSection, "PercentageBorderXWidth", 1.f, &m_fBorderX);
	nResult = pConfigFile->GetFloat(strSection, "PercentageBorderYWidth", .5f, &m_fBorderY);
	nResult = pConfigFile->GetFloat(strSection, "CaptionHeight", 15.0f, &m_fCaptionHeight);
	if (m_fCaptionHeight == 0.0f)
	{
		m_fCaptionHeight = 15.0f;
	}

	nResult = pConfigFile->GetString(strSection, "FontFile", "ui/font/方正黑体_GBK.ttf", strFontFile, MAX_PATH);
	if (strFontFile[0] == '\0')
	{
		strcpy_s(strFontFile, MAX_PATH, "ui/font/方正黑体_GBK.ttf");
	}
	nResult = pConfigFile->GetInteger(strSection, "FontSize", 28, &nFontSize);
	if (nFontSize == 0)
	{
		nFontSize = 28;
	}

	nResult = pConfigFile->GetInteger(strSection, "FontBold", FALSE, &bBold);
	nResult = pConfigFile->GetInteger(strSection, "FontItalic", FALSE, &bItalic);
	nResult = pConfigFile->GetString(strSection, "DefaultFont", "黑体", s_strDefaultFont, MAX_PATH);

	nResult = pConfigFile->GetInteger(strSection, "BackBorderColor", 0xffffffff, &nBackBorderColor);
	m_dwBackBorderColor = static_cast<DWORD>(nBackBorderColor);
	nResult = pConfigFile->GetFloat(strSection, "BackBorderColorScale", 0.3f, &m_fBackBorderColorScale);
	
	//MaxCharactorPerLine一定要在KG3DCaptionTextureManager初始化之前读进来
	nResult = pConfigFile->GetInteger(strSection, "MaxCharactorPerLine", 10, &s_nMaxWordCount);
	if (s_nMaxWordCount == 0)
	{
		s_nMaxWordCount = 10;
	}

	if (!m_pTextureMgr)
	{
		m_pTextureMgr = new KG3DCaptionTextureManager;
	}

	KG_PROCESS_ERROR(m_pTextureMgr);

	hResult = m_pTextureMgr->Init(strFontFile, nFontSize, bBold, bItalic, 0);
	KG_COM_PROCESS_ERROR(hResult);

	hRetCode = S_OK;
Exit0:
	SAFE_RELEASE(pConfigFile);
	return hRetCode;
}

HRESULT KG3DCaption2D::OnLostDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (m_pTextureMgr)
	{
		hr = m_pTextureMgr->OnLostDevice();
		KGLOG_COM_PROCESS_ERROR(hr);
	}

	hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DCaption2D::OnResetDevice()
{
    HRESULT hr = E_FAIL;
    HRESULT hrResult = E_FAIL;

    if (m_pTextureMgr)
	{
		hr = m_pTextureMgr->OnResetDevice();
		KGLOG_COM_PROCESS_ERROR(hr);
	}

    hrResult = S_OK;
Exit0:
    return hrResult;
}

DWORD KG3DCaption2D::Add(DWORD dwID,
						 DWORD dwIndex, 
						 LPCSTR strContent)
{
	DWORD dwFreeIndex = CAPTION_INVALIDATE_INDEX;
	KG_PROCESS_ERROR(m_pTextureMgr);
	KG_PROCESS_ERROR(dwIndex < MAX_NUM_CAPTION);
	KG_PROCESS_ERROR(strContent);

	if (dwID == CAPTION_INVALIDATE_INDEX)
	{
		//如果dwID是CAPTION_INVALIDATE_INDEX表示新生成一个RenderData
		if (m_Free.size())
		{
			//如果有空余的就用空的
			dwFreeIndex = *m_Free.begin();
			m_Free.erase(dwFreeIndex);
		}
		else
		{
			//新生成一个
			m_RenderData.resize(m_RenderData.size() + 1);
			dwFreeIndex = static_cast<DWORD>(m_RenderData.size() - 1);
		}	
	}
	else
	{
		//指定了RenderData的ID
		dwFreeIndex = dwID;
	}

	KG_PROCESS_ERROR(dwFreeIndex != CAPTION_INVALIDATE_INDEX);
	
	m_RenderData[dwFreeIndex].bUsed = TRUE;
	LineContent &Content = m_RenderData[dwFreeIndex].Content[dwIndex];
	if (Content.dwBlockIndex != CAPTION_INVALIDATE_INDEX)
	{
		//如果这个块是有内容的需要清除掉内容
		m_pTextureMgr->Remove(Content.dwMgrIndex, Content.dwBlockIndex);
	}
	Content.dwBlockIndex = m_pTextureMgr->Add(strContent, Content.dwMgrIndex);
	Content.color = crFontColor;
	
Exit0:
	return dwFreeIndex;
}

void KG3DCaption2D::Remove(unsigned int uID)
{
	KG_PROCESS_ERROR(uID < m_RenderData.size());
	for (DWORD i = 0; i < MAX_NUM_CAPTION; i++)
	{
		LineContent &Content = m_RenderData[uID].Content[i];
		if (Content.dwBlockIndex != CAPTION_INVALIDATE_INDEX)
		{
			m_pTextureMgr->Remove(Content.dwMgrIndex, Content.dwBlockIndex);
			Content.dwBlockIndex = CAPTION_INVALIDATE_INDEX;
		}
	}
	m_Free.insert(uID);
	m_RenderData[uID].UnInit();
Exit0:
	;
}

void KG3DCaption2D::SetRenderData(DWORD dwID, 
								  const D3DXVECTOR3 *pPos, 
								  const D3DCOLORVALUE *pColor, 
								  const float *pPercentage,
								  const D3DCOLORVALUE *pPercentageColor)
{
	KG_PROCESS_ERROR(dwID < m_RenderData.size());

	if (pPos)
	{
		//位置
		m_RenderData[dwID].vec3Pos = *pPos;
	}

	if (pColor)
	{
		//颜色
		for (DWORD i = 0; i < MAX_NUM_CAPTION; i++)
		{
			m_RenderData[dwID].Content[i].color = *pColor;
		}
	}

	if (pPercentage)
	{
		//血条信息
		m_RenderData[dwID].fPercentage = *pPercentage;
	}

	if (pPercentageColor)
	{
		//血条颜色
		m_RenderData[dwID].PercentageColor = *pPercentageColor;
	}
Exit0:
	;
}

bool bNew = false;
static int nAlphaRef = 128;
HRESULT KG3DCaption2D::Render()
{
	if (bNew)
	{
		return Render2D();
	}
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;
	
	KG3DRenderState RenderState;
	D3DMATERIAL9 mat;
	D3DXMATRIX matWorld, matScale, matScalePercentage, matTransLocal, matTrans, matView, matWorldSave;
	D3DXVECTOR3 vec3A(-.5f, .5f, .0f), vec3B(.5f, .5f, .0f), vec3C(.5f, -.5f, .0f), vec3D(-.5f, -.5f, .0f);
	D3DXVECTOR2 vec2A(0.0f, 0.0f), vec2B(1.0f, 0.0f), vec2C(1.0f, 1.0f), vec2D(0.0f, 1.0f);
	D3DXVECTOR2 vec2UVA, vec2UVB, vec2UVC, vec2UVD;
	KG_PROCESS_ERROR(m_pTextureMgr);

	//渲染字体贴图
	hResult = m_pTextureMgr->OnRender(true);
	KG_COM_PROCESS_ERROR(hResult);

	ZeroMemory(&mat, sizeof(D3DMATERIAL9));
	mat.Diffuse.a = 1.0f;
	
	g_pd3dDevice->GetTransform(D3DTS_WORLD, &matWorldSave);
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	matView._41 = 0.0f;
	matView._42 = 0.0f;
	matView._43 = 0.f;
	D3DXMatrixInverse(&matView, NULL, &matView);
	
	float fBorderX = m_fBorderX / m_fPercentageWidth;
	float fBorderY = m_fBorderY / m_fPercentageHeight;
	float fRationX = 1 / (1.f - fBorderX);
	float fRationY = 1 / (1.f - fBorderY);
	vec2UVA = D3DXVECTOR2(-0.5f * fRationX + 0.5f, -0.5f * fRationY + 0.5f);
	vec2UVB = D3DXVECTOR2(0.5f * fRationX + 0.5f, -0.5f * fRationY + 0.5f);
	vec2UVC = D3DXVECTOR2(0.5f * fRationX + 0.5f, 0.5f * fRationY + 0.5f);
	vec2UVD = D3DXVECTOR2(-0.5f * fRationX + 0.5f, 0.5f * fRationY + 0.5f);
	
	g_pd3dDevice->SetFVF(Vertex::FVF);
	RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RenderState.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	RenderState.SetRenderState(D3DRS_ALPHAREF, nAlphaRef);
	RenderState.SetRenderState(D3DRS_LIGHTING, TRUE);
	RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	RenderState.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	RenderState.SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetTexture(1, NULL);
	RenderState.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	RenderState.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	float fAspect = m_pTextureMgr->GetAspect();
	float fWidth = fAspect * m_fCaptionHeight;
	D3DXMatrixScaling(&matScale, fWidth, m_fCaptionHeight, 0.0f);
	D3DXMatrixScaling(&matScalePercentage, m_fPercentageWidth, m_fPercentageHeight, 0.0f);
	
	for (size_t i = 0; i < m_RenderData.size(); i++)
	{
		if (!m_RenderData[i].bUsed)
			continue;

		float fStartHeight = m_RenderData[i].m_bVisible[MAX_NUM_CAPTION] ? m_fPercentageHeight : 0.0f;
		D3DXMatrixTranslation(&matTrans, 
			m_RenderData[i].vec3Pos.x, 
			m_RenderData[i].vec3Pos.y, 
			m_RenderData[i].vec3Pos.z);

		
		float fHeight = fStartHeight;
		//渲染头顶文字
		for (DWORD j = 0; j < MAX_NUM_CAPTION; j++)
		{
			if (!m_RenderData[i].m_bVisible[j] || m_RenderData[i].Content[j].dwBlockIndex == CAPTION_INVALIDATE_INDEX)
				continue;
			D3DXVECTOR2 vec2A, vec2C;
			m_pTextureMgr->GetUVCoordinate(m_RenderData[i].Content[j].dwMgrIndex, 
				m_RenderData[i].Content[j].dwBlockIndex, 
				vec2A, 
				vec2C);
			D3DXVECTOR2 vec2B(vec2C.x, vec2A.y), vec2D(vec2A.x, vec2C.y);
			m_Vertex[0].vec2Tex0 = vec2A;
			m_Vertex[1].vec2Tex0 = vec2B;
			m_Vertex[2].vec2Tex0 = vec2C;
			m_Vertex[3].vec2Tex0 = vec2A;
			m_Vertex[4].vec2Tex0 = vec2C;
			m_Vertex[5].vec2Tex0 = vec2D;

			mat.Emissive = m_RenderData[i].Content[j].color;
			D3DXMatrixTranslation(&matTransLocal, 0.0f, fHeight, 0.0f);
			fHeight += m_fCaptionHeight;
			matWorld = matScale * matTransLocal * matView * matTrans;
			g_pd3dDevice->SetMaterial(&mat);
			
			m_pTextureMgr->SetTexture(0, m_RenderData[i].Content[j].dwMgrIndex);
			g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, &m_Vertex, sizeof(Vertex));
		}

		//渲染血条
		if (m_RenderData[i].m_bVisible[MAX_NUM_CAPTION])
		{
			m_Percentage[0].vec3Pos = vec3A;
			m_Percentage[1].vec3Pos = vec3B;
			m_Percentage[2].vec3Pos = vec3C;
			m_Percentage[3].vec3Pos = vec3A;
			m_Percentage[4].vec3Pos = vec3C;
			m_Percentage[5].vec3Pos = vec3D;
			
			m_Percentage[0].vec2Tex0 = vec2UVA;
			m_Percentage[1].vec2Tex0 = vec2UVB;
			m_Percentage[2].vec2Tex0 = vec2UVC;
			m_Percentage[3].vec2Tex0 = vec2UVA;
			m_Percentage[4].vec2Tex0 = vec2UVC;
			m_Percentage[5].vec2Tex0 = vec2UVD;
			RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			mat.Emissive = m_RenderData[i].PercentageColor;
			mat.Emissive.r *= m_fBackBorderColorScale;
			mat.Emissive.g *= m_fBackBorderColorScale;
			mat.Emissive.b *= m_fBackBorderColorScale;
			g_pd3dDevice->SetMaterial(&mat);
			matWorld = matScalePercentage * matView * matTrans;
			g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
			RenderState.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			RenderState.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			RenderState.SetSamplerState(0, D3DSAMP_BORDERCOLOR, m_dwBackBorderColor);
			if (m_pBackTexture)
			{
				g_pd3dDevice->SetTexture(0, m_pBackTexture->GetTexture());
			}
			else
			{
				g_pd3dDevice->SetTexture(0, NULL);
			}
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, &m_Percentage, sizeof(Vertex));
			mat.Emissive = m_RenderData[i].PercentageColor;
			g_pd3dDevice->SetMaterial(&mat);
			
			D3DXVECTOR3 vec3B1((m_RenderData[i].fPercentage - 0.5f) * (1.0f - 2 * fBorderX), .5f - fBorderY, .0f);
			D3DXVECTOR3 vec3C1((m_RenderData[i].fPercentage - 0.5f) * (1.0f - 2 * fBorderX), -.5f + fBorderY, .0f);
			m_Percentage[1].vec3Pos = vec3B1;
			m_Percentage[2].vec3Pos = vec3C1;
			m_Percentage[4].vec3Pos = vec3C1;

			m_Percentage[0].vec3Pos += D3DXVECTOR3(fBorderX, -fBorderY, 0.0f);
			m_Percentage[3].vec3Pos += D3DXVECTOR3(fBorderX, -fBorderY, 0.0f);
			m_Percentage[5].vec3Pos += D3DXVECTOR3(fBorderX, fBorderY, 0.0f);

			m_Percentage[0].vec2Tex0 = vec2A;
			m_Percentage[1].vec2Tex0 = vec2B;
			m_Percentage[2].vec2Tex0 = vec2C;
			m_Percentage[3].vec2Tex0 = vec2A;
			m_Percentage[4].vec2Tex0 = vec2C;
			m_Percentage[5].vec2Tex0 = vec2D;
			RenderState.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			RenderState.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			RenderState.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			if (m_pFrontTexture)
			{
				g_pd3dDevice->SetTexture(0, m_pFrontTexture->GetTexture());
			}
			else
			{
				g_pd3dDevice->SetTexture(0, NULL);
			}
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, &m_Percentage, sizeof(Vertex));
		}
	}
	g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

void KG3DCaption2D::SetVisible(DWORD dwID,
							   DWORD dwIndex, 
							   BOOL bVisible)
{
	KG_PROCESS_ERROR(dwIndex < MAX_NUM_CAPTION + 1);
	KG_PROCESS_ERROR(dwID < m_RenderData.size());

	m_RenderData[dwID].m_bVisible[dwIndex] = bVisible;
Exit0:
	;
}

KG3DCaption2D::RenderData::RenderData()
{
	UnInit();
}

void KG3DCaption2D::RenderData::UnInit()
{
	bUsed = FALSE;
	vec3Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	fPercentage = 1.0f;
	PercentageColor.r = PercentageColor.g = PercentageColor.b = PercentageColor.a = 1.0f;
	for (DWORD i = 0; i < MAX_NUM_CAPTION; i++)
	{
		m_bVisible[i] = TRUE;
		Content[i].UnInit();
	}
	m_bVisible[MAX_NUM_CAPTION] = TRUE;
}


float fRenderZ = 0.0f;
bool bTest = false;
float fTestWidth = 160;


static D3DXVECTOR3 GetScreenPoint(const D3DXVECTOR3 &vec3Pos, 
								  const D3DXMATRIX &matWVP,
								  const D3DVIEWPORT9 &vp)
{
	D3DXVECTOR3 vec3PosResult;
	D3DXVec3TransformCoord(&vec3PosResult, &vec3Pos, &matWVP);
	vec3PosResult.y = vp.Height * (-vec3PosResult.y + 1.0f) / 2.0f  + vp.Y;
	vec3PosResult.x = vp.Width  * (vec3PosResult.x + 1.0f) / 2.0f + vp.X;
	return vec3PosResult;
}

HRESULT KG3DCaption2D::Render2D()
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult = E_FAIL;

	KG3DRenderState RenderState;
	D3DVIEWPORT9 vp;
	D3DMATERIAL9 mat;
	D3DXMATRIX matWorld, matScale, matProj, matWVP, matScalePercentage, matTransLocal, matTrans, matView, matWorldSave;
	D3DXVECTOR3 vec3A(-.5f, .5f, .0f), vec3B(.5f, .5f, .0f), vec3C(.5f, -.5f, .0f), vec3D(-.5f, -.5f, .0f);
	D3DXVECTOR2 vec2A(0.0f, 0.0f), vec2B(1.0f, 0.0f), vec2C(1.0f, 1.0f), vec2D(0.0f, 1.0f);
	D3DXVECTOR2 vec2UVA, vec2UVB, vec2UVC, vec2UVD;
	KG_PROCESS_ERROR(m_pTextureMgr);

	//渲染字体贴图
	hResult = m_pTextureMgr->OnRender(true);
	KG_COM_PROCESS_ERROR(hResult);

	ZeroMemory(&mat, sizeof(D3DMATERIAL9));
	mat.Diffuse.a = 1.0f;

	g_pd3dDevice->GetViewport(&vp);
	g_pd3dDevice->GetTransform(D3DTS_VIEW, &matView);
	g_pd3dDevice->GetTransform(D3DTS_PROJECTION, &matProj);
	matWVP = matView * matProj;
	
	float fBorderX = m_fBorderX / m_fPercentageWidth;
	float fBorderY = m_fBorderY / m_fPercentageHeight;
	float fRationX = 1 / (1.f - fBorderX);
	float fRationY = 1 / (1.f - fBorderY);
	vec2UVA = D3DXVECTOR2(-0.5f * fRationX + 0.5f, -0.5f * fRationY + 0.5f);
	vec2UVB = D3DXVECTOR2(0.5f * fRationX + 0.5f, -0.5f * fRationY + 0.5f);
	vec2UVC = D3DXVECTOR2(0.5f * fRationX + 0.5f, 0.5f * fRationY + 0.5f);
	vec2UVD = D3DXVECTOR2(-0.5f * fRationX + 0.5f, 0.5f * fRationY + 0.5f);

	RenderState.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	RenderState.SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	RenderState.SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	RenderState.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	RenderState.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	RenderState.SetRenderState(D3DRS_ALPHAREF, nAlphaRef);
	RenderState.SetRenderState(D3DRS_LIGHTING, FALSE);
	RenderState.SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	RenderState.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	RenderState.SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pd3dDevice->SetTexture(1, NULL);
	RenderState.SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE);
	RenderState.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL);
	RenderState.SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
	float fAspect = m_pTextureMgr->GetAspect();
	float fWidth = fAspect * m_fCaptionHeight;
	D3DXMatrixScaling(&matScale, fWidth, m_fCaptionHeight, 0.0f);
	D3DXMatrixScaling(&matScalePercentage, m_fPercentageWidth, m_fPercentageHeight, 0.0f);

	int nIndex = 0;
	for (size_t i = 0; i < m_RenderData.size(); i++)
	{
		if (!m_RenderData[i].bUsed)
			continue;

		float fHeight = 0.0f;
		//渲染头顶文字
		for (DWORD j = 0; j < MAX_NUM_CAPTION; j++)
		{
			if (!m_RenderData[i].m_bVisible[j] || 
				m_RenderData[i].Content[j].dwBlockIndex == CAPTION_INVALIDATE_INDEX)
				continue;
			D3DXVECTOR2 vec2A, vec2C;
			m_pTextureMgr->GetUVCoordinate(m_RenderData[i].Content[j].dwMgrIndex, 
				m_RenderData[i].Content[j].dwBlockIndex, 
				vec2A, 
				vec2C);
			D3DXVECTOR2 vec2B(vec2C.x, vec2A.y), vec2D(vec2A.x, vec2C.y);
			

			static float fOffset[10];
			
			D3DXVECTOR3 vec3Center = GetScreenPoint(m_RenderData[i].vec3Pos + D3DXVECTOR3(fOffset[nIndex], fOffset[nIndex], fOffset[nIndex]) , matWVP, vp);
			Vertex2D Vertices[6];
			D3DXVECTOR4 vec4A, vec4B, vec4C, vec4D;
			Vertices[0].vec2Tex = vec2A;
			Vertices[1].vec2Tex = vec2B;
			Vertices[2].vec2Tex = vec2C;
			Vertices[3].vec2Tex = vec2A;
			Vertices[4].vec2Tex = vec2C;
			Vertices[5].vec2Tex = vec2D;

			DWORD dwDiffuse = Value2Color(m_RenderData[i].Content[j].color);
			Vertices[0].dwColor = dwDiffuse;
			Vertices[1].dwColor = dwDiffuse;
			Vertices[2].dwColor = dwDiffuse;
			Vertices[3].dwColor = dwDiffuse;
			Vertices[4].dwColor = dwDiffuse;
			Vertices[5].dwColor = dwDiffuse;

			float fAspect = m_pTextureMgr->GetAspect();
			float fTestHalfHeight = fTestWidth / fAspect / 2.0f;
			float fTestHalfWidth = fTestWidth / 2.0f;
			vec4A = D3DXVECTOR4(-fTestHalfWidth, -fTestHalfHeight + fHeight, 0.0f, 1.0f) + D3DXVECTOR4(vec3Center, 0.0f);
			vec4B = D3DXVECTOR4( fTestHalfWidth, -fTestHalfHeight + fHeight, 0.0f, 1.0f) + D3DXVECTOR4(vec3Center, 0.0f);
			vec4C = D3DXVECTOR4( fTestHalfWidth,  fTestHalfHeight + fHeight, 0.0f, 1.0f) + D3DXVECTOR4(vec3Center, 0.0f);
			vec4D = D3DXVECTOR4(-fTestHalfWidth,  fTestHalfHeight + fHeight, 0.0f, 1.0f) + D3DXVECTOR4(vec3Center, 0.0f);

			fHeight -= fTestHalfHeight * 2.0f;

			Vertices[0].vec4Pos = vec4A;
			Vertices[1].vec4Pos = vec4B;
			Vertices[2].vec4Pos = vec4C;
			Vertices[3].vec4Pos = vec4A;
			Vertices[4].vec4Pos = vec4C;
			Vertices[5].vec4Pos = vec4D;
			
			m_pTextureMgr->SetTexture(0, m_RenderData[i].Content[j].dwMgrIndex);
			g_pd3dDevice->SetFVF(Vertex2D::FVF);
			
			hRetCode = g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, &Vertices, sizeof(Vertex2D));
			KG_COM_PROCESS_ERROR(hRetCode);
		}

		//渲染血条
		/*if (m_RenderData[i].m_bVisible[MAX_NUM_CAPTION])
		{
			m_Percentage[0].vec3Pos = vec3A;
			m_Percentage[1].vec3Pos = vec3B;
			m_Percentage[2].vec3Pos = vec3C;
			m_Percentage[3].vec3Pos = vec3A;
			m_Percentage[4].vec3Pos = vec3C;
			m_Percentage[5].vec3Pos = vec3D;

			m_Percentage[0].vec2Tex0 = vec2UVA;
			m_Percentage[1].vec2Tex0 = vec2UVB;
			m_Percentage[2].vec2Tex0 = vec2UVC;
			m_Percentage[3].vec2Tex0 = vec2UVA;
			m_Percentage[4].vec2Tex0 = vec2UVC;
			m_Percentage[5].vec2Tex0 = vec2UVD;
			RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
			mat.Emissive = m_RenderData[i].PercentageColor;
			mat.Emissive.r *= m_fBackBorderColorScale;
			mat.Emissive.g *= m_fBackBorderColorScale;
			mat.Emissive.b *= m_fBackBorderColorScale;
			g_pd3dDevice->SetMaterial(&mat);
			matWorld = matScalePercentage * matView * matTrans;
			g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorld);
			RenderState.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_BORDER);
			RenderState.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_BORDER);
			RenderState.SetSamplerState(0, D3DSAMP_BORDERCOLOR, m_dwBackBorderColor);
			if (m_pBackTexture)
			{
				g_pd3dDevice->SetTexture(0, m_pBackTexture->GetTexture());
			}
			else
			{
				g_pd3dDevice->SetTexture(0, NULL);
			}
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, &m_Percentage, sizeof(Vertex));
			mat.Emissive = m_RenderData[i].PercentageColor;
			g_pd3dDevice->SetMaterial(&mat);

			D3DXVECTOR3 vec3B1((m_RenderData[i].fPercentage - 0.5f) * (1.0f - 2 * fBorderX), .5f - fBorderY, .0f);
			D3DXVECTOR3 vec3C1((m_RenderData[i].fPercentage - 0.5f) * (1.0f - 2 * fBorderX), -.5f + fBorderY, .0f);
			m_Percentage[1].vec3Pos = vec3B1;
			m_Percentage[2].vec3Pos = vec3C1;
			m_Percentage[4].vec3Pos = vec3C1;

			m_Percentage[0].vec3Pos += D3DXVECTOR3(fBorderX, -fBorderY, 0.0f);
			m_Percentage[3].vec3Pos += D3DXVECTOR3(fBorderX, -fBorderY, 0.0f);
			m_Percentage[5].vec3Pos += D3DXVECTOR3(fBorderX, fBorderY, 0.0f);

			m_Percentage[0].vec2Tex0 = vec2A;
			m_Percentage[1].vec2Tex0 = vec2B;
			m_Percentage[2].vec2Tex0 = vec2C;
			m_Percentage[3].vec2Tex0 = vec2A;
			m_Percentage[4].vec2Tex0 = vec2C;
			m_Percentage[5].vec2Tex0 = vec2D;
			RenderState.SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
			RenderState.SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
			RenderState.SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
			RenderState.SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
			if (m_pFrontTexture)
			{
				g_pd3dDevice->SetTexture(0, m_pFrontTexture->GetTexture());
			}
			else
			{
				g_pd3dDevice->SetTexture(0, NULL);
			}
			g_pd3dDevice->DrawPrimitiveUP(D3DPT_TRIANGLELIST, 2, &m_Percentage, sizeof(Vertex));
		}*/
	}
	//g_pd3dDevice->SetTransform(D3DTS_WORLD, &matWorldSave);
	hRetCode = S_OK;
Exit0: 
	return hRetCode;
}