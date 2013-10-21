////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : itemtext.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2005-11-9 16:57:18
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_ITEMTEXT_H_
#define _INCLUDE_ITEMTEXT_H_

////////////////////////////////////////////////////////////////////////////////

#include "KG3DEngine/KG3DInterface.h"
#include "./item.h"
#include "../../common/tblock.h"

////////////////////////////////////////////////////////////////////////////////

namespace UI
{

//KItemNull占据了低4位。
#define ITEM_TEXT_HALIGN_CENTER				0x00010000
#define ITEM_TEXT_HALIGN_RIGHT				0x00020000

#define ITEM_TEXT_VALIGN_CENTER				0x00040000
#define ITEM_TEXT_VALIGN_BOTTOM				0x00080000

#define ITEM_TEXT_HALIGN_FILTER (ITEM_TEXT_HALIGN_CENTER | ITEM_TEXT_HALIGN_RIGHT)
#define ITEM_TEXT_VALIGN_FILTER (ITEM_TEXT_VALIGN_CENTER | ITEM_TEXT_VALIGN_BOTTOM)

#define ITEM_TEXT_CENTER_EACH_ROW			0x00100000
#define ITEM_TEXT_MULTILINE					0x00200000
#define ITEM_TEXT_FORMATE_BY_OTHER			0x00400000
#define ITEM_TEXT_NOT_RICH_TEXT				0x00800000
#define ITEM_TEXT_AUTO_SIZE					0x01000000
#define ITEM_TEXT_SELECTED					0x02000000	//文字被选中
#define ITEM_TEXT_AUTO_ETC					0x04000000	//文字显示不完自动补齐省略号
#define ITEM_TEXT_SHOW_ALL					0x08000000	//不管大小限制，总是显示完整

#define ITEM_TEXT_AS_ML_LESS_ADJ			0x10000000
#define ITEM_TEXT_AS_ML_NO_LIMIT			0x20000000

enum
{
	ITEM_TEXT_CONTROL_KEY_NONE		= 0,
	ITEM_TEXT_CONTROL_KEY_NEW_LINE	= 1,
	ITEM_TEXT_CONTROL_KEY_TAB		= 2,
};

struct KItemTextData : KItemNullData
{
    int nFontScheme;
    int nMultiLine;
    int nHAlign;
    int nVAlign;
	int nCenterEachLine;
    float fRowSpacing;
	float fFontSpacing;
	int nRichText;
	int nAutoEtc;
	int nText;
	int nShowAll;
	DWORD dwColor;
	int nAutoAdj;

    KItemTextData(KWndWindow *pEOwner) : KItemNullData(pEOwner)
    {
        nFontScheme = 0;
        nMultiLine = 0;
        nHAlign = 0;
        nVAlign = 0;
        fRowSpacing = 0.0f;
		fFontSpacing = 0.0f;
		nCenterEachLine = 0;
		nRichText = true;
		nShowAll = true;
		nAutoEtc = false;
		dwColor = 0;
		nAutoAdj = false;
		nText = false;
    }
};

inline int g_GetNextNewLineKeyW(LPCWSTR wszText, int &nControlKeyPos)
{
	if (wszText == NULL)
		return ITEM_TEXT_CONTROL_KEY_NONE;

	LPCWSTR wszSearch = wszText;

	while (wszSearch && *wszSearch != L'\0') 
	{
		if (*wszSearch == L'\n')
		{
			nControlKeyPos = static_cast<int>(wszSearch - wszText);
			return ITEM_TEXT_CONTROL_KEY_NEW_LINE;
		}
		wszSearch = ::CharNextW(wszSearch);
	}
	return ITEM_TEXT_CONTROL_KEY_NONE;
}

inline int g_IsCharShouldPosedInTheEnd(WCHAR wszChar)
{
	switch(wszChar)
	{
	case L'>':
	case L'》':
	case L')':
	case L'）':
	case L'}':
	case L'｝':
	case L']':
	case L'］':
	case L'】':
	case L'’':
	case L'”':
	case L',':
	case L'，':
	case L'.':
	case L'。':
	case L'!':
	case L'！':
	case L'?':
	case L'？':
	case L'%':
	case L'％':
	case L'…':
	case L'—':
	case L'、':
		return TRUE;
	default:
		break;
	}
	return FALSE;
}

class KItemText : public KItemNull
{
	typedef std::vector<KG3DUIText2> KUITextArray;

protected:
    KItemText();
    virtual ~KItemText();

public:
	void* operator new(size_t nSize, std::nothrow_t const&) throw();
	void operator delete(void* pData, std::nothrow_t const&) throw();

	void* operator new(size_t nSize);
	void operator delete(void* pData) throw();

protected:
	void* operator new[](std::size_t, std::nothrow_t const&) throw();
	void operator delete[](void*, std::nothrow_t const&) throw();

	void* operator new[](std::size_t) throw(std::bad_alloc);
	void operator delete[](void*) throw();

	void* operator new(size_t, void*) throw();
	void operator delete(void*, void*) throw();

	void* operator new[](std::size_t, void*) throw();
	void operator delete[](void*, void*) throw();

#ifdef KGUI_LUA_LOADER
public:
    static int  Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex);
    virtual int Init(Lua_State *pLua, int nTableIndex);
#endif

public:
    static KItemText *Create(KItemTextData &ItemTextData);
    virtual void Release();
    virtual int Init(KItemTextData &ItemTextData);

    virtual void Draw();
    virtual DWORD GetItemType();
    virtual int IsItemCanChangeTo(DWORD dwType);

    virtual int PtInItem(float fAbsX, float fAbsY);

    virtual int SetRelPos(float fRelX, float fRelY);
    virtual int SetAbsPos(float fAbsX, float fAbsY);
    virtual int AbsMove(float fDeltaX, float fDeltaY);
	virtual int Scale(float fScaleX, float fScaleY);

	virtual int	SetAlpha(int nAlpha);

    int     SetFontScheme(int nFontScheme);
    int     GetFontScheme();

    int     SetText(LPCWSTR wszText);
    LPCWSTR GetText() const;
    size_t  GetTextLength();

    int     SetVAlign(int nAlignType);
    int     GetVAlign();
    int     SetHAlign(int nAlignType);
    int     GetHAlign();

	int		SetCenterEachLine(int nCenterEachLine);
	int		IsCenterEachLine();

    int     SetRowSpacing(float fSpacing);
    float   GetRowSpacing();

	int		SetFontSpacing(float fSpacing);
	float	GetFontSpacing();

    int     SetMultiLine(int nMultiLine);
    int     IsMultiLine();

	int		SetAutoEtc(int nFlag);
	int		IsAutoEtc();

    int     AutoSize();
    
	int     FormatTextForDraw();

	int		ClearDrawTextData();
	int		AppendDrawTextData(D3DXVECTOR2 vMin, D3DXVECTOR2 vMax, LPCWSTR wszSrc, unsigned uLen);

	int		GetSplitPos(LPCWSTR wszSrc, float &fPos, int &nType) const;

	float	GetSingleTextHeight() const;
	float	GetTableTextWidth() const;

	int		SetRichText(int nRichText);
	int		IsRichText();

	int		AdjustDrawDataEndAbsY(float fOldY, float fNewY);
	int		AdjustShowTextV(float fAbsTop, float fAbsBottom);
	int		AdjustShowTextH(float fAbsLeft, float fAbsRight);
	int		AdjustShowAll();

	int		GetLastDrawBlockInfo(float &fEndX, float &fEndY, LPCWSTR* pwszEndControlKey);

	int		IsAutoSize();

	int		SetSelected(int nSelected);
	int		SetSelBgColor(DWORD dwColor);

	int		SetFontScale(float fScale);
	float	GetFontScale();

	int		SetFontID(DWORD dwID);
	int		SetFontColor(DWORD dwColor);
	int		SetFontBorder(WORD dwSize, DWORD dwColor);
	int		SetFontShadow(WORD dwSize, DWORD dwColor);

	DWORD	GetFontID();
	DWORD	GetFontColor();
	int		GetFontBoder(WORD &dwSize, DWORD &dwColor);
	int		GetFontProjection(WORD &dwSize, DWORD &dwColor);

	int		GetTextExtent(int nTextLength, float& fWidth, float& fHeight) const;
	int		GetTextPosExtent(float& fWidth, int &nTextLength) const;

private:
    int     FormatSingleLineHorizontal();
	int     FormatSingleLineVertical();
    int     FormatMultiLineHorizontal();
	int     FormatMultiLineVertical();

public:
    static  DWORD ms_dwItemType;

private:      
    float       m_fRowSpacing;	//行距
	float		m_fFontSpacing;	//字间距
    int         m_nFontScheme;  //字体索引号

	DWORD		m_dwFontID;		//字体ID
	DWORD		m_dwFontColor;	//字体颜色
	DWORD		m_dwBorderColor;	//字体边框颜色
	DWORD		m_dwShadowColor;	//字体投影颜色
	BYTE		m_byBorder;	//边框大小
	BYTE		m_byShadow;	//投影大小
	BYTE		m_byPading[2];

	float		m_fFontScale;	//字体缩放系数

    std::wstring m_strText;          //Text数据
    KUITextArray m_aDrawText;   //用于绘制的Text数据

	int			m_nFirstShowIndex;
	int			m_nShowCount;

	DWORD		m_dwSelectBgColor;
};

} //namespace UI

#endif //_INCLUDE_ITEMTEXT_H_
