////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : wndedit.h
//  Version     : 1.0
//  Creator     : Hu Changyin
//  Create Date : 2006-7-18 10:43:13
//  Comment     : 
//                Edited by Chen Jie 2006-12-30
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_WNDEDIT_H_
#define _INCLUDE_WNDEDIT_H_

////////////////////////////////////////////////////////////////////////////////
#include "./wndwindow.h"
#include "./Components/itemtext.h"
#include "../driver/kcolor.h"

#define	WNDEDIT_ES_FILTER				0x000000FF
#define	WNDEDIT_ES_ENGLISH_AVAILABLE	0x00000001		//可以输入西文字符，符号、标点等
#define	WNDEDIT_ES_IME_AVAILABLE		0x00000002		//可以输入ime字符（中文字符）等
#define	WNDEDIT_ES_CHAR_TYPE_FILTER		(WNDEDIT_ES_ENGLISH_AVAILABLE | WNDEDIT_ES_IME_AVAILABLE)
#define	WNDEDIT_ES_MASK_CHARACTER		0x00000010		//显示时以特殊符号替换要显示的字符
#define	WNDEDIT_ES_MULTI_LINE			0x00000020		//多行编辑
//#define	WNDEDIT_ES_WIN32_WND			0x0040		//采用Win32窗口
#define	WNDEDIT_F_HAVE_FOCUS			0x00000100		//为当前拥有输入焦点的窗口
#define	WNDEDIT_F_SHOW_CARET			0x00000200		//显示光标
#define	WNDEDIT_F_SHOW_DATA_UNFORMATED	0x00000400		//显示数据没有经格式化
#define	WNDEDIT_F_SEL_STARTED			0x00000800		//正在进行选择
#define	WNDEDIT_F_ENABLE_ANIMATE		0x00001000		//允许控制符转换为表情符号
#define WNDEDIT_F_STR_TRANSFORMED       0x00002000      //已从显示数据转换回字符串
#define WNDEDIT_F_CURSOR_MOVED          0x00004000      //光标的位置发生了变化
#define WNDEDIT_F_DRAW_DATA_CHANGED     0x00008000      //绘制列表发生了变化
#define WNDEDIT_F_LIMIT_MULTIBYTE		0x00010000      //输入的长度的控制使用MultiByte为计算单位

#define K_INPUT_CURSOR				    L"|"
#define K_INPUT_MASK                    L"*"
#define K_INPUT_MASK_CHAR               L'*'
#define K_INPUT_CURSOR_NAME			    "_Cursor"
#define K_INPUT_MASK_NAME               "_Mask"
#define K_INPUT_TEXT_NAME               "_Text"
#define K_INPUT_TERM_NAME               "_Term"     //TERMINATOR, 结束标志
#define K_INPUT_OBJ_NAME				"_Obj"

namespace UI
{

class KWndEdit : public KWndWindow
{
private:
    typedef std::vector<KItemNull *> KItemArray;

#ifdef KGUI_LUA_LOADER
public:
    static int  Create(IKUIObject **ppObj, Lua_State *pLua, int nTableIndex);
    virtual int Init(Lua_State *pLua, int nTableIndex);
#endif

public:
	virtual void	SetSize(float fWidth, float fHeight);					//设置窗口大小                        
	virtual LRESULT WndProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual void    OnPaint();

	virtual void    AbsMove(float fXOffset, float fYOffset);

	virtual void    Enable(int nEnable);

    int             SetText(LPCWSTR wszText);
	LPCWSTR         GetText();
	int             GetPassword(char szDestMD5String[64]);
	int             GetTextLength(BOOL bMultiByte);
	int             ClearText();

    int             InsertObject(LPCSTR szTypeName, LPCWSTR wszShowText, int nUserData);
    int             GetTextLuaStruct(Lua_State* L);

	void            SetFontScheme(int nFontScheme);
	void			SetFontColor(DWORD dwColor);
	int             GetFontScheme();

	int             SelectAll();
	int				CancelSelect();
	int             SetType(int nType);	//设置输入框类型 0数字/1英文/2中英文

	int				SetLimit(int nLimit);
	int				GetLimit();

	void			SetLimitMultiByte(int nMultiByte);
	int				IsLimitMultiByte();

	int				IsPassword();
	virtual int     CanCastTo(LPCSTR szType) const;
    virtual LPCSTR	GetWndType() const;

	int				AppendString(LPCWSTR wszString);
    int             Backspace();

	BOOL IsStatus(DWORD dwStatus) const;
	void SetStatus(DWORD dwStatus, BOOL bEnable);

	void SetFontSpacing(float fSpacing);
	void SetRowSpacing(float fSpacing);
	void SetFocusBgColor(DWORD dwColor);
    void SetSelectBgColor(DWORD dwColor);
	void SetSelectFontScheme(int nFontScheme);
	void SetCaretFontScheme(int nFontScheme);

	int CreateCursorHandle();
	int CreateTextHandle();

	BOOL SetCurSel(int nStart, int nEnd, BOOL bMultiByte);
protected:
	virtual void    OnBreathe();
	virtual void    OnScale(float fScaleX, float fScaleY);

	LRESULT	        OnLButtonDown(float fX, float fY);
	LRESULT	        OnLButtonUp(float fX, float fY);
	LRESULT	        OnRButtonDown(float fX, float fY);
	LRESULT         OnRButtonUp(float fX, float fY);
	LRESULT         OnMouseMove(WPARAM wParam, float fX, float fY);

	LRESULT         OnMouseWheel(WPARAM wParam);

	LRESULT         OnLButtonDblClk();

	LRESULT         OnChar(WPARAM wParam);
	LRESULT         OnImeChar(WPARAM wParam);

	LRESULT         OnKeyDown(WPARAM wKey, LPARAM lModifier);
	LRESULT         OnSysKeyDown(WPARAM wParam, LPARAM lParam);

	LRESULT         OnKeyDownSpecial(WPARAM wKey);

	LRESULT         OnSetFocus();
	LRESULT         OnKillFocus();

private:
	int             CopyString();
	int             PasteString();
	int             DeleteString();

	int				GetTextRowHeight(int nIndex, float &fRowHeight);
	int				IsTextRowVisible(int nIndex);
	int             GetTextPos(float fX, float fY, DWORD *pdwRetPos, int *pnAfterFlag);
 	int             FormatShowData();
	int             LetCaretShow();

	void            CorrectImePos();

    int             IsInSelectRegion(DWORD dwPos);
    void            UpdateSelRegion();
	void            SetSelStartPos(DWORD dwPos, int nAfterFlag);
	void            SetSelEndPos(DWORD dwPos, int nAfterFlag);
	void            SetCaretPos(DWORD dwPos, int nAfterFlag);
    DWORD           GetCaretPos();

    float           GetFontHeight(int nFontScheme);

	int             IgnoreKey(WPARAM wKey);
	int             HoldUpKey(WPARAM wKey);

    void            MoveCaretCharPrev();
    void            MoveCaretCharNext();
	void            MoveCaretWordPrev();
	void            MoveCaretWordNext();
	void            MoveCaretRowPrev();
	void            MoveCaretRowNext();
	void            MoveCaretPagePrev();
	void            MoveCaretPageNext();
	void            MoveCaretRowHome();
	void            MoveCaretRowEnd();

	int             AppendChar(WCHAR wcChar);
	int             AppendFilteredChar(WCHAR wcChar);

	void			Remove(KItemNull *pItem);

private:
	DWORD           m_dwStatus;

    std::wstring    m_strText;

	float           m_fFontSpacing;         //字间距
	float           m_fRowSpacing;			//行间距

    DWORD           m_dwSelStartPos;        //选择区域起始点
    DWORD           m_dwSelEndPos;          //选择区域结束点

	DWORD           m_dwCaretPos;           //插入符的位置
    int             m_nCaretAfterFlag;      //光标靠字符前显示还是靠字符后显示
	DWORD           m_dwCaretTickCount;     //控制插入符显示的时钟

	int				m_nTextLimit;          //允许用户输入的最大长度, -1表示不限制

	DWORD           m_dwFocusBgColor;       //有焦点时的背景颜色
    DWORD           m_dwSelectBgColor;         //选中部分的背景色

	int	            m_nFontScheme;          //平时的字体
	int             m_nSelectFontScheme;      //选中时的字体
	int             m_nCaretFontScheme;     //光标用的字体

	DWORD			m_dwFontColor;			//平时的字体颜色

    KItemText*      m_pCursor;              //用于存放光标
	KItemHandle*    m_pTextHandle;	        //用于存放显示内容的Handle

	KItemArray      m_aDraw;

public:
	KWndEdit();
	virtual	~KWndEdit();

	void* operator new(size_t nSize, std::nothrow_t const&) throw();
	void  operator delete(void* pData, std::nothrow_t const&) throw();

	void* operator new(size_t nSize);
	void  operator delete(void* pData) throw();

protected:
	void* operator new[](std::size_t, std::nothrow_t const&) throw();
	void  operator delete[](void*, std::nothrow_t const&) throw();

	void* operator new[](std::size_t) throw(std::bad_alloc);
	void  operator delete[](void*) throw();

	void* operator new(size_t, void*) throw();
	void  operator delete(void*, void*) throw();

	void* operator new[](std::size_t, void*) throw();
	void  operator delete[](void*, void*) throw();
};


inline BOOL KWndEdit::IsStatus(DWORD dwStatus) const
{
	return (m_dwStatus & dwStatus) != 0; 
}

}

#endif //_INCLUDE_WNDEDIT_H_
