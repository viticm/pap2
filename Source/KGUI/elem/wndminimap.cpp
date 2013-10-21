////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : wndminimap.cpp
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-9-15 10:14:54
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "./wndminimap.h"
#include "./wndmessage.h"
#include "../common/tblock.h"
#include "../script/kscriptmgr.h"
#include "../KGUI.h"

////////////////////////////////////////////////////////////////////////////////

namespace UI
{

	void* KWndMinimap::operator new(size_t nSize, std::nothrow_t const&) throw()
	{
		ASSERT(nSize == sizeof(KWndMinimap));
		return KBlockPool::GetSelf().Alloc(sizeof(KWndMinimap)); 
	}

	void KWndMinimap::operator delete(void* pData, std::nothrow_t const&) throw()
	{ 
		KBlockPool::GetSelf().Free(pData, sizeof(KWndMinimap));
	}

	void* KWndMinimap::operator new(size_t nSize)
	{ 
		ASSERT(nSize == sizeof(KWndMinimap));
		return KBlockPool::GetSelf().Alloc(sizeof(KWndMinimap)); 
	}

	void KWndMinimap::operator delete(void* pData) throw()
	{ 
		KBlockPool::GetSelf().Free(pData, sizeof(KWndMinimap));
	}

	void* KWndMinimap::operator new[](std::size_t nSize, std::nothrow_t const&) throw()
	{
		return malloc(nSize); 
	}

	void KWndMinimap::operator delete[](void* pData, std::nothrow_t const&) throw()
	{
		free(pData);
	}

	void* KWndMinimap::operator new[](std::size_t nSize) throw(std::bad_alloc)
	{
		return malloc(nSize);
	}

	void KWndMinimap::operator delete[](void* pData) throw()
	{
		free(pData);
	}

	void* KWndMinimap::operator new(size_t, void*) throw()
	{
		ASSERT(0); 
		return NULL;
	}

	void KWndMinimap::operator delete(void*, void*) throw()
	{
		ASSERT(0);
	}

	void* KWndMinimap::operator new[](std::size_t, void*) throw()
	{
		ASSERT(0);
		return NULL;
	}

	void KWndMinimap::operator delete[](void*, void*) throw()
	{
		ASSERT(0);
	}

#ifdef KGUI_LUA_LOADER
    int KWndMinimap::Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex)
    {
        int nResult  = false;
        int nRetCode = false;

        KWndWindow *pWnd = NULL;

        KGLOG_PROCESS_ERROR(ppObj);

        pWnd = new(std::nothrow) KWndMinimap;
        KGLOG_PROCESS_ERROR(pWnd);

        if (pLua)
        {
            nRetCode = pWnd->Init(pLua, nTableIndex);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        *ppObj = pWnd;
    // Exit1:
        nResult = true;
    Exit0:
        if (!nResult)
        {
            KG_DELETE(pWnd);
        }
        return nResult;
    }
#endif

	KWndMinimap::KWndMinimap()
		: m_dwMinimapType(MINIMAP_RECTANGLE)
		, m_fSceneToMinimapScale(1.0f)
		, m_fEachBlockWidth(128.0f)
		, m_fWndToMinimapScale(1.0f)
		, m_dwDefaultImageID(0)
		, m_dwSharpImageID(0)
		, m_nImageInfoIndex(-1)
		, m_bDataUnFormated(true)
		, m_bMouseInMinimap(false)
		, m_bLastOverObj(FALSE)
		, m_bLastOverSelf(FALSE)
		, m_fSendX(0.0f)
		, m_fSendY(0.0f)
	{ 
		m_szMinimapPath[0] = '\0';
		m_szPointFile[0] = '\0';

		m_Self.vPos = D3DXVECTOR2(0.0f, 0.0f);
		m_Self.nFrame = -1;
		m_Self.vMin = D3DXVECTOR2(-10000.0f, -10000.0f);
		m_Self.vMax = D3DXVECTOR2(-10000.0f, -10000.0f);
		memset(&m_SharpImage, 0, sizeof(m_SharpImage));
	}

	KWndMinimap::~KWndMinimap()
	{
		for (KOBJECTINFOMAP::iterator it = m_Obj.begin(); it != m_Obj.end(); ++it)
		{
			if ((*it).second.nType != OBJECT_ANIMATE)
				continue;
			g_pUI->m_AnimateImageMgr.UnloadFile((*it).second.nAnimateIndex);
		}

		if (m_nImageInfoIndex != -1)
		{
			g_pUI->m_ImageInfoMgr.UnloadImageInfo(m_nImageInfoIndex);
			m_nImageInfoIndex = -1;
		}

		for (KMINIMAPBLOCKTOIMAGE::iterator it = m_Minimap.begin(); it != m_Minimap.end(); ++it)
		{
			if ((*it).second.dwImageID != m_dwDefaultImageID)
			{
				g_pUI->m_p3DUI->UnloadImage((*it).second.dwImageID);
			}
		}

		g_pUI->m_p3DUI->UnloadImage(m_dwDefaultImageID);
		g_pUI->m_p3DUI->UnloadImage(m_dwSharpImageID);
	}

#ifdef KGUI_LUA_LOADER
    int KWndMinimap::Init(Lua_State *pLua, int nTableIndex)
    {
    	int nResult  = false;
    	int nRetCode = false;

        DWORD   dwResourceID     = 0;
        HRESULT hRetCode         = NULL;
        int     nMapType         = 0;
        char    szDefTexturePath[MAX_PATH];
        char    szShpTexturePath[MAX_PATH];

        KLuaTableField aParamTable[] = 
        {
            KLUA_TABLE_FIELD("type",            emVAR_TYPE_INT,     0,  &nMapType,        0),
            KLUA_TABLE_FIELD("image",           emVAR_TYPE_STRING,  "", m_szPointFile,         _countof(m_szPointFile)),
            KLUA_TABLE_FIELD("selfframe",       emVAR_TYPE_INT,     0,  &m_Self.nFrame,   0),
            KLUA_TABLE_FIELD("defaulttexture",  emVAR_TYPE_STRING,  "", szDefTexturePath, _countof(szDefTexturePath)),
            KLUA_TABLE_FIELD("sharptexture",    emVAR_TYPE_STRING,  "", szShpTexturePath, _countof(szShpTexturePath)),
        };

        KGLOG_PROCESS_ERROR(pLua);

        nRetCode = KWndWindow::Init(pLua, nTableIndex);
        KGLOG_PROCESS_ERROR(nRetCode);

        for (int i = 0; i < _countof(aParamTable); i++)
        {
            nRetCode = g_LuaGetTableField(pLua, nTableIndex, &aParamTable[i]);
            KGLOG_PROCESS_ERROR(nRetCode);
        }

        switch (nMapType)
        {
        case MINIMAP_RECTANGLE:
        case MINIMAP_CIRCLE:
            m_dwMinimapType = nMapType;
            break;
        }

        if (m_szPointFile[0])
        {
            ASSERT(m_nImageInfoIndex == -1 && "ÓÐÌùÍ¼ÉÐÎ´ÊÍ·Å");
            FormatFilePath(m_szPointFile);

			m_nImageInfoIndex = g_pUI->m_ImageInfoMgr.LoadImageInfo(m_szPointFile);
			KGLOG_PROCESS_ERROR(m_nImageInfoIndex >= 0);
        }

        if (szDefTexturePath[0])
        {
            FormatFilePath(szDefTexturePath);

            hRetCode = g_pUI->m_p3DUI->LoadImage(szDefTexturePath, &dwResourceID);
            if (SUCCEEDED(hRetCode))
            {
                m_dwDefaultImageID = dwResourceID;
            }
        }

        if (szShpTexturePath[0])
        {
            FormatFilePath(szShpTexturePath);

            hRetCode = g_pUI->m_p3DUI->LoadImage(szShpTexturePath, &dwResourceID);
            if (SUCCEEDED(hRetCode))
            {
                m_dwSharpImageID = dwResourceID;
            }
        }
    // Exit1:
    	nResult = true;
    Exit0:
    	return nResult;
    }
#endif

	int	KWndMinimap::PtInWindow(int x, int y)
	{
		switch (m_dwMinimapType)
		{
		case MINIMAP_CIRCLE:
			if (KWndWindow::PtInWindow(x, y))
			{
				float fR = m_fWidth / 2.0f;
				float fRH = m_fHeight / 2.0f;
				float fDX = m_fAbsX + fR - x;
				float fDY = m_fAbsY + fRH - y;
				if (fRH < fR)
					fR = fRH;
				if (fDX * fDX + fDY * fDY <= fR * fR)
					return true;
				return false;
			}
			return false;
		default:
			return KWndWindow::PtInWindow(x, y);
		}
	}

	LRESULT	KWndMinimap::WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		if (KWndWindow::WndProc(uMsg, wParam, lParam))
			return TRUE;

		switch(uMsg)
		{
		case WM_LBUTTONDOWN:
			if (m_fSceneToMinimapScale > FLT_MIN)
			{
				D3DXVECTOR2 vPos;
				vPos.x = (float)GET_X_LPARAM(lParam);
				vPos.y = (float)GET_Y_LPARAM(lParam);
				WndPosToMinimapPos(vPos);
				m_fSendX = vPos.x / m_fSceneToMinimapScale;
				m_fSendY = vPos.y / m_fSceneToMinimapScale;

				g_pUI->m_Script.FireWndEvent(this, "OnMinimapSendInfo");
			}
			return true;
			break;
		case WND_M_MOUSE_ENTER:
			m_bMouseInMinimap = true;
			return TRUE;
			break;
		case WND_M_MOUSE_LEAVE:
			m_bMouseInMinimap = false;
			return TRUE;
			break;
		default:
			break;
		}

		return FALSE;
	}

	void KWndMinimap::OnPaint()
	{
		if (m_bDataUnFormated)
			FormatDrawData();

		if (m_dwMinimapType == MINIMAP_CIRCLE)
		{
			g_pUI->m_p3DUI->BeginStencil();
			g_pUI->m_p3DUI->DrawMultiImage(&m_SharpImage, 1);
			g_pUI->m_p3DUI->SetStencilFunc();
		}
		
		for (KMINIMAPBLOCKTOIMAGE::iterator it = m_Minimap.begin(); it != m_Minimap.end(); ++it)
			g_pUI->m_p3DUI->DrawMultiImage(&((*it).second), 1);

		if (m_aArrow.size())
			g_pUI->m_p3DUI->DrawMultiImageRotate(&(m_aArrow[0]), m_aArrow.size());

		for (KOBJECTINFOMAP::iterator it = m_Obj.begin(); it != m_Obj.end(); ++it)
		{
			if (it->second.nType == OBJECT_ANIMATE)
			{
				KOBJECTINFO &Obj = it->second;
				g_pUI->m_AnimateImageMgr.Draw(Obj.nAnimateIndex, Obj.vMin.x, Obj.vMin.y, 
					Obj.vMax.x - Obj.vMin.x, Obj.vMax.y - Obj.vMin.y, m_dwAlpha
				);
			}
		}

		if (m_aImage.size())
			g_pUI->m_p3DUI->DrawMultiImage(&(m_aImage[0]), m_aImage.size());

		KWndWindow::OnPaint();

		if (m_dwMinimapType == MINIMAP_CIRCLE)
		{
			g_pUI->m_p3DUI->EndStencil();
		}
	}

	void KWndMinimap::OnBreathe()
	{
		KOBJECTINFOMAP::iterator it = m_Obj.begin();
		while (it != m_Obj.end())
		{
			KOBJECTINFO &Obj = it->second;
			if (Obj.nLeftCount != -1)
			{
				Obj.nLeftCount--;
				if (Obj.nLeftCount < 0)
				{
					if (Obj.nType == OBJECT_ANIMATE)
					{
						g_pUI->m_AnimateImageMgr.UnloadFile(Obj.nAnimateIndex);
					}
					it = m_Obj.erase(it);
					continue;
				}
			}
			++it;
		}

		BOOL bOverObj = FALSE;
		BOOL bOverSelf = FALSE;
		KOBJECTID ObjID;
		int nX, nY;
		if (!m_bMouseInMinimap)
			goto Exit0;
		g_pUI->m_Pointer.GetCursorPos(nX, nY);
		if (!PtInWindow(nX, nY))
			goto Exit0;
		for (KOBJECTINFOMAP::reverse_iterator it = m_Obj.rbegin(); it != m_Obj.rend(); ++it)
		{
			KOBJECTINFO &Obj = it->second;
			if (nX > Obj.vMin.x && nX < Obj.vMax.x && nY > Obj.vMin.y && nY < Obj.vMax.y)
			{
				bOverObj = TRUE;
				ObjID.dwType = it->first.dwType;
				ObjID.dwID = it->first.dwID;
				break;
			}
		}
		if (!bOverObj && 
			(nX > m_Self.vMin.x && nX < m_Self.vMax.x && nY > m_Self.vMin.y && nY < m_Self.vMax.y))
		{
			bOverSelf = TRUE;
		}

Exit0:
		if (m_bLastOverSelf)
		{
			if (bOverSelf)
				return;
			g_pUI->m_Script.FireWndEvent(this, "OnMinimapMouseLeaveSelf");
			m_bLastOverSelf = FALSE;
		}

		if (m_bLastOverObj)
		{
			if (bOverObj &&
				m_LastMouseOverObj.dwType == ObjID.dwType &&
				m_LastMouseOverObj.dwID == ObjID.dwID
			)
				return;
			g_pUI->m_Script.FireWndEvent(this, "OnMinimapMouseLeaveObj");
			m_bLastOverObj = FALSE;
		}

		if (bOverSelf)
		{
			g_pUI->m_Script.FireWndEvent(this, "OnMinimapMouseEnterSelf");
			m_bLastOverSelf = TRUE;
			return;
		}

		if (bOverObj)
		{
			m_LastMouseOverObj.dwType = ObjID.dwType;
			m_LastMouseOverObj.dwID = ObjID.dwID;
			g_pUI->m_Script.FireWndEvent(this, "OnMinimapMouseEnterObj");
			m_bLastOverObj =TRUE;
			return;
		}
	}

	void KWndMinimap::FormatDrawData()
	{
		KG_PROCESS_ERROR(m_szMinimapPath[0] != '\0');
		KG_PROCESS_ERROR(m_nImageInfoIndex >= 0);

		FormatMapDrawData();
		
		m_aArrow.clear();
		m_aImage.clear();

		FormatSelfDrawData();

		for (KOBJECTINFOMAP::iterator it = m_Obj.begin(); it != m_Obj.end(); ++it)
		{
			switch (it->second.nType)
			{
			case OBJECT_POINT:
				FormatStaticPoint(it->second);
				break;
			case OBJECT_ANIMATE:
				FormatAnimatePoint(it->second);
				break;
			case OBJECT_ARROW:
				FormatArrowPoint(it->second);
				break;
			}
		}

		m_SharpImage.v2Min = D3DXVECTOR2(m_fAbsX, m_fAbsY);
		m_SharpImage.v2Max = D3DXVECTOR2(m_fAbsX + m_fWidth, m_fAbsY + m_fHeight);
		m_SharpImage.dwImageID = m_dwSharpImageID;
		m_SharpImage.crDiffuse = 0xFFFFFFFF;
		m_SharpImage.v2UVMin = D3DXVECTOR2(0.0f, 0.0f);
		m_SharpImage.v2UVMax = D3DXVECTOR2(1.0f, 1.0f);
Exit0:
		m_bDataUnFormated = false;
		return;
	}

	void KWndMinimap::FormatMapDrawData()
	{
		ASSERT(m_fEachBlockWidth);

		float fTotalScale = m_fWndToMinimapScale / g_pUI->m_WndStation.GetUIScale();
		float fW = m_fWidth * fTotalScale;
		float fH = m_fHeight * fTotalScale;
		float fLeft = m_Self.vPos.x - fW / 2.0f;
		float fRight = fLeft + fW;
		float fTop = m_Self.vPos.y + fH / 2.0f;
		float fBottom = fTop - fH;

		int nLayer = m_Self.nLayer;

		int nLeft = (int)floor(fLeft / m_fEachBlockWidth);
		int nRight = (int)floor(fRight / m_fEachBlockWidth);
		int nTop = (int)floor(fTop / m_fEachBlockWidth);
		int nBottom = (int)floor(fBottom / m_fEachBlockWidth);

		for (KMINIMAPBLOCKTOIMAGE::iterator it = m_Minimap.begin(); it != m_Minimap.end(); NULL)
		{
			const KMINIMAPBLOCKINFO &rBlock = it->first;
			if (rBlock.nLayer != nLayer || 
				rBlock.nX < nLeft || rBlock.nX > nRight || 
				rBlock.nY < nBottom || rBlock.nY > nTop)
			{
				if (it->second.dwImageID != m_dwDefaultImageID)
					g_pUI->m_p3DUI->UnloadImage(it->second.dwImageID);
				it = m_Minimap.erase(it);
			}
			else
			{
				++it;
			}
		}

		KMINIMAPBLOCKINFO Block;
		KG3DUIImage2 Image;
		Image.crDiffuse = m_dwAlpha << 24;

		for (int i = nLeft; i <= nRight; ++i)
		{
			for (int j = nBottom; j <= nTop; ++j)
			{
				Block.nLayer = nLayer;
				Block.nX = i;
				Block.nY = j;

				//----------------ÌùÍ¼×ø±ê---------------------------
				if (i == nLeft)
					Image.v2UVMin.x = fLeft - i * m_fEachBlockWidth;
				else
					Image.v2UVMin.x = 0.0f;

				if (i == nRight)
					Image.v2UVMax.x = fRight - i * m_fEachBlockWidth;
				else
					Image.v2UVMax.x = m_fEachBlockWidth;

				if (j == nTop)
					Image.v2UVMin.y = (j + 1) * m_fEachBlockWidth - fTop;
				else
					Image.v2UVMin.y = 0.0f;

				if (j == nBottom)
					Image.v2UVMax.y = (j + 1) * m_fEachBlockWidth - fBottom;
				else
					Image.v2UVMax.y = m_fEachBlockWidth;

				//----------------ÆÁÄ»×ø±ê-----------------------------
				Image.v2Min.x = i * m_fEachBlockWidth + Image.v2UVMin.x;
				Image.v2Min.y = (j + 1) * m_fEachBlockWidth - Image.v2UVMin.y;
				Image.v2Max.x = i * m_fEachBlockWidth + Image.v2UVMax.x;
				Image.v2Max.y = (j + 1) * m_fEachBlockWidth - Image.v2UVMax.y;
				MinimapPosToWndPos(Image.v2Min);
				MinimapPosToWndPos(Image.v2Max);

				Image.v2UVMin /= m_fEachBlockWidth;
				Image.v2UVMax /= m_fEachBlockWidth;

				KMINIMAPBLOCKTOIMAGE::iterator it = m_Minimap.find(Block);
				if (it != m_Minimap.end())
				{
					KG3DUIImage2 &rImage = (*it).second;
					rImage.v2UVMin = Image.v2UVMin;
					rImage.v2UVMax = Image.v2UVMax;
					rImage.v2Min = Image.v2Min;
					rImage.v2Max = Image.v2Max;
				}
				else
				{
					sprintf(m_szMapFile, "%s%d_%d_%d.dds", m_szMinimapPath, nLayer, j, i);

					DWORD dwID = m_dwDefaultImageID;
					if (g_IsFileExist(m_szMapFile))
					{
						g_pUI->m_p3DUI->LoadImage(m_szMapFile, &dwID);
					}
					else
					{
						sprintf(m_szMapFile, "%s%d_%d_%d.tga", m_szMinimapPath, nLayer, j, i);
						if (g_IsFileExist(m_szMapFile))
							g_pUI->m_p3DUI->LoadImage(m_szMapFile, &dwID);
					}
					Image.dwImageID = dwID;
					m_Minimap.insert(make_pair(Block, Image));
				}
			}
		}

	}

	void KWndMinimap::FormatSelfDrawData()
	{
		KG3DUIImageRotate2 Image;
		SIZE sizeImage;
		KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(m_nImageInfoIndex);
		KG_PROCESS_ERROR(pImageInfo);
		KG_PROCESS_ERROR(m_Self.nFrame >= 0 && m_Self.nFrame < pImageInfo->GetFrameCount());

		sizeImage = pImageInfo->GetImageSize();

		RECT const& rcSrc = pImageInfo->GetImageRect(m_Self.nFrame);

		float fUIScale = g_pUI->m_WndStation.GetUIScale();
		float fHalfWidth = (rcSrc.right - rcSrc.left) * fUIScale / 2.0f;
		float fHalfHeight = (rcSrc.bottom - rcSrc.top) * fUIScale / 2.0f;
		float fCenterX = m_fAbsX + m_fWidth / 2.0f;
		float fCenterY = m_fAbsY + m_fHeight / 2.0f;

		Image.fRotationCW = -m_Self.fDirection;
		Image.dwImageID = pImageInfo->GetImageID(m_Self.nFrame);
		Image.crDiffuse = m_dwAlpha << 24;
		Image.v2Min = D3DXVECTOR2(fCenterX - fHalfWidth, fCenterY - fHalfHeight);
		Image.v2Max = D3DXVECTOR2(fCenterX + fHalfWidth, fCenterY + fHalfHeight);
		Image.v2UVMin = D3DXVECTOR2((float)(rcSrc.left) / (float)sizeImage.cx, (float)(rcSrc.top) / (float)sizeImage.cy);
		Image.v2UVMax = D3DXVECTOR2((float)(rcSrc.right) / (float)sizeImage.cx, (float)(rcSrc.bottom) / (float)sizeImage.cy);

		m_aArrow.push_back(Image);

		m_Self.vMin = Image.v2Min;
		m_Self.vMax = Image.v2Max;
Exit0:
		return;
	}

	void KWndMinimap::FormatArrowPoint(KOBJECTINFO &Obj)
	{
		KG3DUIImageRotate2 Image;
		int nFrame = Obj.nFrame;
		int nFrameArrow = Obj.nArrowFrame;
		D3DXVECTOR2 vPlayer = Obj.vPos;
		KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(m_nImageInfoIndex);
		KG_PROCESS_ERROR(pImageInfo);

		KG_PROCESS_ERROR(nFrame >= 0 && nFrame < pImageInfo->GetFrameCount());
		KG_PROCESS_ERROR(nFrameArrow >= 0 && nFrameArrow < pImageInfo->GetFrameCount());

		Image.dwImageID = pImageInfo->GetImageID(nFrame);
		Image.crDiffuse = m_dwAlpha << 24;

		float fCenterX = m_fAbsX + m_fWidth / 2.0f;
		float fCenterY = m_fAbsY + m_fHeight / 2.0f;

		RECT const& rcSrc = pImageInfo->GetImageRect(nFrame);
		RECT const& rcSrcArrow = pImageInfo->GetImageRect(nFrameArrow);
		SIZE const& sizeImage = pImageInfo->GetImageSize();

		float fUIScale = g_pUI->m_WndStation.GetUIScale();

		float fHalfWidth = fUIScale * (rcSrc.right - rcSrc.left) / 2;
		float fHalfWidthArrow = fUIScale * (rcSrcArrow.right - rcSrcArrow.left) / 2;

		float fW = m_fWidth / 2 - fHalfWidth;
		float fH = m_fHeight / 2 - fHalfWidth;
		float fR = fW;
		if (fH < fW)
			fR = fH;
		KG_PROCESS_ERROR(fR > FLT_MIN);
		float fWArrow = m_fWidth / 2 - fHalfWidthArrow;
		float fHArrow = m_fHeight / 2 - fHalfWidthArrow;
		float fRArrow = fWArrow;
		if (fHArrow < fWArrow)
			fRArrow = fHArrow;

		MinimapPosToWndPos(vPlayer);
		int nOutOfRange = false;

		float fX = vPlayer.x - fCenterX;
		float fY = vPlayer.y - fCenterY;

		switch (m_dwMinimapType)
		{
		case MINIMAP_CIRCLE:
			{
				float fDis = fX * fX + fY * fY;
				if (fDis > fR * fR)
				{
					nOutOfRange = true;
					Image.fRotationCW = atan2(fY, fX);
					float fROrg = sqrt(fDis);
					vPlayer.x = fCenterX + fRArrow * fX / fROrg;
					vPlayer.y = fCenterY + fRArrow * fY / fROrg;
				}
			}
			break;
		default:
			if (fX < -fW || fX > fW || fY < -fH || fY > fH )
			{
				nOutOfRange = true;
				float fArg = atan2(fY, fX);
				Image.fRotationCW = fArg;
				
				float fArg1 = atan2(fH, fW);
				if (fArg >= - D3DX_PI + fArg1 && fArg < -fArg1)
				{
					vPlayer.x = -fHArrow * fX / fY; 
					vPlayer.y = -fHArrow;
				}
				else if (fArg >= -fArg1 && fArg < fArg1)
				{
					vPlayer.x = fWArrow;
					vPlayer.y = fWArrow * fY / fX;
				}
				else if (fArg >= fArg1 && fArg < D3DX_PI - fArg1)
				{
					vPlayer.x = fHArrow * fX / fY;
					vPlayer.y = fHArrow;
				}
				else
				{
					vPlayer.x = -fWArrow;
					vPlayer.y = -fWArrow *fY / fX;
				}

				vPlayer.x += fCenterX;
				vPlayer.y += fCenterY;
			}
			break;
		}

		if (nOutOfRange)
		{
			Image.v2Min = D3DXVECTOR2(vPlayer.x - fHalfWidthArrow, vPlayer.y - fHalfWidthArrow);
			Image.v2Max = D3DXVECTOR2(vPlayer.x + fHalfWidthArrow, vPlayer.y + fHalfWidthArrow);
			Image.v2UVMin = D3DXVECTOR2((float)(rcSrcArrow.left) / (float)sizeImage.cx, (float)(rcSrcArrow.top) / (float)sizeImage.cy);
			Image.v2UVMax = D3DXVECTOR2((float)(rcSrcArrow.right) / (float)sizeImage.cx, (float)(rcSrcArrow.bottom) / (float)sizeImage.cy);

			m_aArrow.push_back(Image);

			Obj.vMin = Image.v2Min;
			Obj.vMax = Image.v2Max;
		}
		else
		{
			Image.v2Min = D3DXVECTOR2(vPlayer.x - fHalfWidth, vPlayer.y - fHalfWidth);
			Image.v2Max = D3DXVECTOR2(vPlayer.x + fHalfWidth, vPlayer.y + fHalfWidth);
			Image.v2UVMin = D3DXVECTOR2((float)(rcSrc.left) / (float)sizeImage.cx, (float)(rcSrc.top) / (float)sizeImage.cy);
			Image.v2UVMax = D3DXVECTOR2((float)(rcSrc.right) / (float)sizeImage.cx, (float)(rcSrc.bottom) / (float)sizeImage.cy);

			m_aImage.push_back(Image);

			Obj.vMin = Image.v2Min;
			Obj.vMax = Image.v2Max;
		}

Exit0:
		return;
	}

	void KWndMinimap::FormatStaticPoint(KOBJECTINFO &Obj)
	{
		KG3DUIImage2 Image;
		SIZE sizeImage;
		D3DXVECTOR2 vPlayer = Obj.vPos;
		KImageInfo* pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(m_nImageInfoIndex);
		KG_PROCESS_ERROR(pImageInfo);
		KG_PROCESS_ERROR(Obj.nFrame >= 0 && Obj.nFrame < pImageInfo->GetFrameCount());

		RECT const& rcSrc = pImageInfo->GetImageRect(Obj.nFrame);
		sizeImage = pImageInfo->GetImageSize();

		Image.dwImageID = pImageInfo->GetImageID(Obj.nFrame);
		Image.crDiffuse = m_dwAlpha << 24;
		Image.v2UVMin = D3DXVECTOR2((float)(rcSrc.left) / (float)sizeImage.cx, (float)(rcSrc.top) / (float)sizeImage.cy);
		Image.v2UVMax = D3DXVECTOR2((float)(rcSrc.right) / (float)sizeImage.cx, (float)(rcSrc.bottom) / (float)sizeImage.cy);

		float fUIScale = g_pUI->m_WndStation.GetUIScale();
		float fHalfWidth = fUIScale * (rcSrc.right - rcSrc.left) / 2;
		float fW = m_fWidth / 2 - fHalfWidth;
		float fH = m_fHeight / 2 - fHalfWidth;
		float fR = fW;
		if (fH < fW)
			fR = fH;
		KG_PROCESS_ERROR(fR > FLT_MIN);

		float fCenterX = m_fAbsX + m_fWidth / 2.0f;
		float fCenterY = m_fAbsY + m_fHeight / 2.0f;

		MinimapPosToWndPos(vPlayer);
		int nOutOfRange = false;

		float fX = vPlayer.x - fCenterX;
		float fY = vPlayer.y - fCenterY;

		switch (m_dwMinimapType)
		{
		case MINIMAP_CIRCLE:
			if (fX * fX + fY * fY > fR * fR)
			{
				Obj.vMin = D3DXVECTOR2(-10000.0f, -10000.0f);
				Obj.vMax = D3DXVECTOR2(-10000.0f, -10000.0f);
				goto Exit0;
			}
			break;
		default:
			if (fX < -fW || fX > fW || fY < -fH || fY > fH )
			{
				Obj.vMin = D3DXVECTOR2(-10000.0f, -10000.0f);
				Obj.vMax = D3DXVECTOR2(-10000.0f, -10000.0f);
				goto Exit0;
			}
			break;
		}

		Image.v2Min = D3DXVECTOR2(vPlayer.x - fHalfWidth, vPlayer.y - fHalfWidth);
		Image.v2Max = D3DXVECTOR2(vPlayer.x + fHalfWidth, vPlayer.y + fHalfWidth);
		m_aImage.push_back(Image);

		Obj.vMin = Image.v2Min;
		Obj.vMax = Image.v2Max;

Exit0:
		return;
	}

	void KWndMinimap::FormatAnimatePoint(KOBJECTINFO &Obj)
	{
		int nResult = false;
		int nRetCode = false;
		int nImageIndex = 0;
		int nTextureFrame = 0;
		float fUIScale = g_pUI->m_WndStation.GetUIScale();
		KImageInfo* pImageInfo = NULL;
		SIZE size = { 0, 0 };
		D3DXVECTOR2 vPos = Obj.vPos;

		nImageIndex = g_pUI->m_AnimateImageMgr.GetImageInfoIndex(Obj.nAnimateIndex);
		KG_PROCESS_ERROR(nImageIndex >= 0);

		nTextureFrame = g_pUI->m_AnimateImageMgr.GetFrameIndex(Obj.nAnimateIndex);
		KG_PROCESS_ERROR(nTextureFrame >= 0);

		pImageInfo = g_pUI->m_ImageInfoMgr.GetImageInfo(nImageIndex);
		KG_PROCESS_ERROR(pImageInfo);

		size = pImageInfo->GetFrameSize(nTextureFrame);	

		float fHalfWidth = fUIScale * size.cx / 2.0f;
		float fCenterX = m_fAbsX + m_fWidth / 2.0f;
		float fCenterY = m_fAbsY + m_fHeight / 2.0f;
		MinimapPosToWndPos(vPos);

		float fX = vPos.x - fCenterX;
		float fY = vPos.y - fCenterY;
		float fW = m_fWidth / 2 - fHalfWidth;
		float fH = m_fHeight / 2 - fHalfWidth;
		float fR = fW;
		if (fH < fW)
			fR = fH;
		KG_PROCESS_ERROR(fR > FLT_MIN);

		switch (m_dwMinimapType)
		{
		case MINIMAP_CIRCLE:
			{
				float fDis = fX * fX + fY * fY;
				if (fDis > fR * fR)
				{
					float fROrg = sqrt(fDis);
					vPos.x = fCenterX + fR * fX / fROrg;
					vPos.y = fCenterY + fR * fY / fROrg;
				}
			}
			break;
		default:
			if (fX < -fW || fX > fW || fY < -fH || fY > fH )
			{
				float fArg = atan2(fY, fX);
				float fArg1 = atan2(fH, fW);
				if (fArg >= - D3DX_PI + fArg1 && fArg < -fArg1)
				{
					vPos.x = -fH * fX / fY; 
					vPos.y = -fH;
				}
				else if (fArg >= -fArg1 && fArg < fArg1)
				{
					vPos.x = fW;
					vPos.y = fW * fY / fX;
				}
				else if (fArg >= fArg1 && fArg < D3DX_PI - fArg1)
				{
					vPos.x = fH * fX / fY;
					vPos.y = fH;
				}
				else
				{
					vPos.x = -fW;
					vPos.y = -fW * fY / fX;
				}

				vPos.x += fCenterX;
				vPos.y += fCenterY;
			}
			break;
		}
		Obj.vMin = D3DXVECTOR2(vPos.x - fHalfWidth, vPos.y - fHalfWidth);
		Obj.vMax = D3DXVECTOR2(vPos.x + fHalfWidth, vPos.y + fHalfWidth);

		nResult = true;
Exit0:
		return;
	}

	inline void KWndMinimap::MinimapPosToWndPos(D3DXVECTOR2 &vPos)
	{
		float fTotalScale = m_fWndToMinimapScale / g_pUI->m_WndStation.GetUIScale();
		vPos.x = m_fAbsX + m_fWidth / 2 + (vPos.x - m_Self.vPos.x) / fTotalScale;
		vPos.y = m_fAbsY + m_fHeight / 2 + (m_Self.vPos.y - vPos.y) / fTotalScale;
	}

	inline void KWndMinimap::WndPosToMinimapPos(D3DXVECTOR2 &vPos)
	{
		float fTotalScale = m_fWndToMinimapScale / g_pUI->m_WndStation.GetUIScale();
		vPos.x = m_Self.vPos.x + (vPos.x - m_fAbsX - m_fWidth / 2) * fTotalScale;
		vPos.y = m_Self.vPos.y + (m_fAbsY + m_fHeight / 2 - vPos.y) * fTotalScale;
	}

	int KWndMinimap::CanCastTo(LPCSTR szType) const
	{
		return szType == "WndMinimap" || szType == "WndWindow";
	}

	int KWndMinimap::SetMapPath(LPCSTR pcszPath)
	{
		int nResult = false;
		IIniFile *pIni = NULL;
		char szInfo[MAX_PATH];

		KG_PROCESS_ERROR(pcszPath);

		Clear();

		_snprintf(m_szMinimapPath, _countof(m_szMinimapPath), "\\%s%s\\", pcszPath, "minimap");
		m_szMinimapPath[_countof(m_szMinimapPath) - 1] = '\0';

		_snprintf(szInfo, _countof(szInfo), "%s%s", m_szMinimapPath, "config.ini");
		szInfo[_countof(szInfo) - 1] = '\0';

		pIni = g_OpenIniFile(szInfo);
		KG_PROCESS_ERROR(pIni);

		pIni->GetFloat("config", "scale", 1.0f, &m_fSceneToMinimapScale);
		pIni->GetFloat("config", "width", 64.0f, &m_fEachBlockWidth);

		nResult = true;
Exit0:
		KG_COM_RELEASE(pIni);
		return nResult;
	}

	int KWndMinimap::SetScale(float fScale)
	{
		if (fScale < 0.1f)
			fScale = 0.1f;
		if (fScale > 10.0f)
			fScale = 10.0f;
		m_fWndToMinimapScale = fScale;

		m_bDataUnFormated = true;
		return true;
	}

	float KWndMinimap::GetScale()
	{
		return m_fWndToMinimapScale;
	}

	void KWndMinimap::UpdataStaticPoint(DWORD dwType, DWORD dwID, int nFrame, float fX, float fY, int nLeftCount)
	{
		KOBJECTID ObjID(dwType, dwID);
		KOBJECTINFOMAP::iterator it = m_Obj.find(ObjID);
		if (it == m_Obj.end())
		{
			KOBJECTINFO ObjInfo;
			ObjInfo.nType = OBJECT_POINT;
			ObjInfo.nFrame = nFrame;
			ObjInfo.vMin = D3DXVECTOR2(-10000.0f, -10000.0f);
			ObjInfo.vMax = D3DXVECTOR2(-10000.0f, -10000.0f);
			ObjInfo.vPos = D3DXVECTOR2(fX * m_fSceneToMinimapScale, fY * m_fSceneToMinimapScale);
			ObjInfo.nLeftCount = nLeftCount;
			m_Obj.insert(make_pair(ObjID, ObjInfo));
			m_bDataUnFormated = true;
			return;
		}

		KOBJECTINFO &ObjInfo = (*it).second;
		ObjInfo.nFrame = nFrame;
		ObjInfo.vPos = D3DXVECTOR2(fX * m_fSceneToMinimapScale, fY * m_fSceneToMinimapScale);
		ObjInfo.nLeftCount = nLeftCount;
		m_bDataUnFormated = true;
		return;
	}

	void KWndMinimap::UpdataAnimatePoint(DWORD dwType, DWORD dwID, int nGroup, float fX, float fY, int nLeftCount)
	{
		KOBJECTID ObjID(dwType, dwID);
		KOBJECTINFOMAP::iterator it = m_Obj.find(ObjID);
		if (it == m_Obj.end())
		{
            int nAnimateIndex = g_pUI->m_AnimateImageMgr.LoadFile(m_szPointFile);
			if (nAnimateIndex >= 0)
			{
				g_pUI->m_AnimateImageMgr.SetGroup(nAnimateIndex, nGroup);
				g_pUI->m_AnimateImageMgr.SetLoopCount(nAnimateIndex, -1);

				KOBJECTINFO ObjInfo;

				ObjInfo.nAnimateIndex = nAnimateIndex;
				ObjInfo.nType = OBJECT_ANIMATE;
				ObjInfo.vMin = D3DXVECTOR2(-10000.0f, -10000.0f);
				ObjInfo.vMax = D3DXVECTOR2(-10000.0f, -10000.0f);
				ObjInfo.vPos = D3DXVECTOR2(fX * m_fSceneToMinimapScale, fY * m_fSceneToMinimapScale);
				ObjInfo.nLeftCount = nLeftCount;

				m_Obj.insert(make_pair(ObjID, ObjInfo));
				m_bDataUnFormated = true;
			}
		}
		else
		{
			KOBJECTINFO &ObjInfo = (*it).second;
			ObjInfo.vPos = D3DXVECTOR2(fX * m_fSceneToMinimapScale, fY * m_fSceneToMinimapScale);
			ObjInfo.nLeftCount = nLeftCount;
			m_bDataUnFormated = true;
		}
	}


	void KWndMinimap::UpdataArrowPoint(DWORD dwType, DWORD dwID, int nFrame, int nArrawFrame, float fX, float fY, int nLeftCount)
	{
		KOBJECTID ObjID(dwType, dwID);
		KOBJECTINFOMAP::iterator it = m_Obj.find(ObjID);
		if (it == m_Obj.end())
		{
			KOBJECTINFO ObjInfo;
			ObjInfo.nType = OBJECT_ARROW;
			ObjInfo.nFrame = nFrame;
			ObjInfo.nArrowFrame = nArrawFrame;
			ObjInfo.vMin = D3DXVECTOR2(-10000.0f, -10000.0f);
			ObjInfo.vMax = D3DXVECTOR2(-10000.0f, -10000.0f);
			ObjInfo.vPos = D3DXVECTOR2(fX * m_fSceneToMinimapScale, fY * m_fSceneToMinimapScale);
			ObjInfo.nLeftCount = nLeftCount;
			m_Obj.insert(make_pair(ObjID, ObjInfo));
			m_bDataUnFormated = true;
			return;
		}

		KOBJECTINFO &ObjInfo = (*it).second;
		ObjInfo.nFrame = nFrame;
		ObjInfo.nArrowFrame = nArrawFrame;
		ObjInfo.vPos = D3DXVECTOR2(fX * m_fSceneToMinimapScale, fY * m_fSceneToMinimapScale);
		ObjInfo.nLeftCount = nLeftCount;
		m_bDataUnFormated = true;
		return;
	}


	void KWndMinimap::UpdataSelfPos(int nLayer, float fX, float fY, float fDirection)
	{
		m_Self.nLayer = nLayer;
		m_Self.fDirection = (float)(2 * D3DX_PI * fDirection / 255);

		m_Self.vPos.x = fX * m_fSceneToMinimapScale;
		m_Self.vPos.y = fY * m_fSceneToMinimapScale;

		m_bDataUnFormated = true;
	}

	void KWndMinimap::RemovePoint(DWORD dwType, DWORD dwID)
	{
		KOBJECTID ObjID(dwType, dwID);

		KOBJECTINFOMAP::iterator it = m_Obj.find(ObjID);

		if (it != m_Obj.end())
		{
			if ((*it).second.nType == OBJECT_ANIMATE)
				g_pUI->m_AnimateImageMgr.UnloadFile((*it).second.nAnimateIndex);
			m_Obj.erase(it);
			m_bDataUnFormated = true;
		}
	}

	void KWndMinimap::RemovePoint(DWORD dwType)
	{
		KOBJECTINFOMAP::iterator it = m_Obj.begin();
		while(it != m_Obj.end())
		{
			if((*it).first.dwType == dwType)
			{
				if ((*it).second.nType == OBJECT_ANIMATE)
					g_pUI->m_AnimateImageMgr.UnloadFile((*it).second.nAnimateIndex);
				it = m_Obj.erase(it);
				m_bDataUnFormated = true;
			}
			else
			{
				++it;
			}
		}
	}


	void KWndMinimap::Clear()
	{
		for (KMINIMAPBLOCKTOIMAGE::iterator it = m_Minimap.begin(); it != m_Minimap.end(); ++it)
		{
			if (it->second.dwImageID != m_dwDefaultImageID)
			{
				g_pUI->m_p3DUI->UnloadImage((*it).second.dwImageID);
			}
		}

		for (KOBJECTINFOMAP::iterator it = m_Obj.begin(); it != m_Obj.end(); ++it)
		{
			if (it->second.nType == OBJECT_ANIMATE)
			{
				g_pUI->m_AnimateImageMgr.UnloadFile((*it).second.nAnimateIndex);
			}
		}

		m_Minimap.clear();

		m_Obj.clear();

		m_szMinimapPath[0] = '\0';

		m_bDataUnFormated = true;
	}

	LPCSTR KWndMinimap::GetWndType() const
	{
		return "WndMinimap";
	}

	void KWndMinimap::GetOverObj(DWORD &dwType, DWORD &dwID)
	{
		dwType = m_LastMouseOverObj.dwType;
		dwID = m_LastMouseOverObj.dwID;
	}

	void KWndMinimap::GetSendPos(float &fX, float &fY)
	{
		fX = m_fSendX;
		fY = m_fSendY;
	}

	void KWndMinimap::SetMinimapType(int nType)
	{
		switch (nType)
		{
		case MINIMAP_RECTANGLE:
		case MINIMAP_CIRCLE:
			m_dwMinimapType = nType;
			break;
		}
	}

	void KWndMinimap::LoadPointImageFile(LPCSTR szFile)
	{
		KGLOG_PROCESS_ERROR(szFile);
		KG_PROCESS_ERROR(szFile[0] != '\0');

		if (m_nImageInfoIndex != -1)
			g_pUI->m_ImageInfoMgr.UnloadImageInfo(m_nImageInfoIndex);

		m_nImageInfoIndex = g_pUI->m_ImageInfoMgr.LoadImageInfo(szFile);
		KGLOG_PROCESS_ERROR(m_nImageInfoIndex >= 0);

		strcpy(m_szPointFile, szFile);
		return;
Exit0:
		m_szPointFile[0] = '\0';
		return;
	}

	void KWndMinimap::LoadDefaultImageFile(LPCSTR szFile)
	{
		HRESULT hr = E_FAIL;
		DWORD dwID = 0;

		KGLOG_PROCESS_ERROR(szFile);
		KG_PROCESS_ERROR(szFile[0] != '\0');

		hr = g_pUI->m_p3DUI->LoadImage(szFile, &dwID);
		KGLOG_COM_PROCESS_ERROR(hr);

		m_dwDefaultImageID = dwID;

		return;
Exit0:
		return;
	}

	void KWndMinimap::LoadSharpImageFile(LPCSTR szFile)
	{
		HRESULT hr = E_FAIL;
		DWORD dwID = 0;

		KGLOG_PROCESS_ERROR(szFile);
		KG_PROCESS_ERROR(szFile[0] != '\0');

        hr = g_pUI->m_p3DUI->LoadImage(szFile, &dwID);
		KGLOG_COM_PROCESS_ERROR(hr);

		m_dwSharpImageID = dwID;

		return;
Exit0:
		return;
	}

	void KWndMinimap::SetSelfImageFrame(int nFrame)
	{
		m_Self.nFrame = nFrame;
	}
}
