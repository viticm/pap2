#pragma once
//#include "xmesh.h"
#include "MeshWinBase.h"

namespace ModelUI
{
#define  ITEM_NOTING        0
#define  ITEM_MOUSEON       1
#define  ITEM_CLICK         2
#define  ITEM_DBCLICK       3
#define  ITEM_MOUSELEAVE    4

#define  ITEMSTYLE_TEXT			 0
#define  ITEMSTYLE_IMAGE		 1
#define  ITEMSTYLE_TEXTIMAGE     2
#define  ITEMSTYLE_MATERIAL      3
#define  ITEMSTYLE_COLOR         4


//class KModelUIItem :
//	public KModelUIBase
//{
//public:
//	D3DCOLORVALUE  m_Color;
//
//	DWORD    m_dwValue;
//
//	DWORD    m_dwStrLength;
//	DWORD    m_dwNumChar;
//	DWORD    m_IconTextID;
//	DWORD    m_dwLastClickTime;
//	BOOL     m_bSelected;
//
//	TCHAR    m_szString[256];
//	TCHAR    m_lpShowStr[256];
//
//	BOOL     m_bDrawBorder;
//
//public:
//	HRESULT SetColor(D3DCOLORVALUE Color);
//	HRESULT SetMaterial(KModel::LPMATERIAL pMat);
//	HRESULT SetIcon(LPSTR pIconFile);
//	
//	KModelUIItem(void);
//	virtual ~KModelUIItem(void);
//
//	HRESULT Create(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style);
//	HRESULT CreateAbsolute(LPFRECT pRect,LPSTR StyleFile,BOOL State,DWORD Style);
//	HRESULT Render(void);
//	HRESULT SetText(LPSTR pSrcText);
//	HRESULT RefreshText(void);
//	HRESULT ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);
//	int     Init(IIniFile* pIniFile, const char* pSection);
//};
};
