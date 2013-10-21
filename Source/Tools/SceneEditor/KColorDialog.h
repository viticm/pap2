#pragma once
#include "resource.h"
#include <d3d9.h>
#include <bitset>

// KColorPickerDialog dialog


/************************************************************************/
/*
WM_COLORPOINTER_CHANGE 消息，WPARAM = COLOR， LPARAM = 发出消息的CWnd本身的hWnd
*/
/************************************************************************/
#define WM_COLORPOINTER_CHANGE WM_USER + 1
#define WM_COLORDIALOG_CLOSE WM_USER + 2
class KColorPickerButton;
class KColorPickerDialog : public CDialog
{
	DECLARE_DYNAMIC(KColorPickerDialog)
	friend KColorPickerButton;
public:
	//KColorPickerDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KColorPickerDialog();

// Dialog Data
	enum { IDD = IDD_COLOR_PICKER };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	enum enumChannel
	{
		ENUM_CHANNEL_BEGIN,
		ENUM_CHANNEL_RED   = 0x00ff0000,
		ENUM_CHANNEL_H = ENUM_CHANNEL_RED,
		ENUM_CHANNEL_GREEN = 0x0000ff00,
		ENUM_CHANNEL_S = ENUM_CHANNEL_GREEN,
		ENUM_CHANNEL_BLUE  = 0x000000ff,
		ENUM_CHANNEL_B = ENUM_CHANNEL_BLUE,
		ENUM_CHANNEL_END,
		ENUM_CHANNEL_FORCE_DWORD = 0xffffffff,
	};
	enum
	{
		IDC_CHANNEL_BEGIN = 0,
		IDC_CHANNEL_RED = 16,
		IDC_CHANNEL_GREEN = 8,
		IDC_CHANNEL_BLUE = 0,
		IDC_CHANNEL_ALPHA = 24,
	};
	enum
	{
		em_part_right_vernier = 1 << 0, //右边游标
		em_part_center_color_pallete = 1 << 1,
		em_part_alpha_vernier = 1 << 3,
	};

	enum
	{
		em_color_pallete_size = 256,
		em_vernier_pallete_width = 1,
	};
private:
	//布局参数
	static const POINT s_ptVernier;//游标的大小
	static const INT s_nSpan;//游标到颜色框的距离
	static const RECT s_rectColor1;//左边颜色框的位置
	static const RECT s_rectColor2;//右边颜色框的位置
	//static const INT s_nWidth2;//右边颜色框的宽度
	static const RECT s_rectVernierRgn;//游标的响应区域
	//static const RECT m_rectAlpha;//Alpha条的位置
	//static const RECT m_rectAlphaRgn;//Alpha条的相应区域
	static const RECT m_rectClearRgn[3];//清除框的位置,分别是右边滑动条左边，右边和Alpha的清除框
	static const RECT m_rectSelectColor;//当前选择颜色框位置
	static const RECT m_rectOriginColor;//初始颜色框位置
	static const RECT m_rectSelAndOrg;	//选择和初始颜色框加起来
	//////////////////////////////////////////////////////////////////////////
	//输入控件
	//CButton m_ctlChannel[3];
	//CEdit m_ctlEditChannel[4];
	//CStatic m_ctlAlphaStatic;
	//////////////////////////////////////////////////////////////////////////
	//bool m_bUseColorValue;

	DWORD m_dwWhichPartIsCursorIn;
	//BOOL m_bVernierSelect;//是否选中了右边的颜色框的游标
	BOOL m_bAlphaEnable;//是否要修改Alpha值
	//BOOL m_bColor1Select;//是否在左边颜色框中按下了鼠标
	//BOOL m_bAlphaSelect;//是否选中了Alpha的游标

	//////////////////////////////////////////////////////////////////////////
	//辅助参数
	//CBitmap* m_pbmpColor1;//左边颜色框对应的位图
	CBitmap	m_bmpLeft;	//左边大颜色框绘制所用的位图

	//CBitmap* m_pbmpColor2;//右边颜色框对应的位图
	CBitmap m_bmpRight;	//右边颜色条用的位图
	
	//CBitmap* m_pBmpAlpha;//Alpha对应的位图
	//CBitmap m_bmpAlpha;
	
	const DWORD m_dwColor;//核心颜色，只能用SetColor来改
	DWORD m_dwColorOld;	//没有SetColor之前的颜色

	D3DCOLORVALUE* m_pColorValueBinded;	//外部绑定的ColorValue
	DWORD* m_pBindedColor;//输入的颜色的地址
	CSliderCtrl m_SliderAlpha;
	//DWORD* m_pColorBuffer1;
	//DWORD* m_pColorBuffer2;
	//DWORD* m_pAlphaBuffer;

	//BOOL m_bAlphaMoved;
	//BOOL m_bVernierMoved;

	
	//int m_nGridSize;	
	//BYTE m_byChannel[4];
	FLOAT m_HSV[3];//值域是360,1,1
	BOOL m_bHSVModel;
	enumChannel m_enumLockChannel;//当前锁定的通道(在右边颜色框才能修改的通道)
	//////////////////////////////////////////////////////////////////////////

	
	DWORD m_dwColorOrg;//初始颜色的值
	HWND	m_hNotifyHWND;
public:
	void Init(DWORD* pColor, LPCTSTR strCaption, CWnd* pNotify = NULL ,BOOL bAlphaEnable = TRUE);
	void Init(D3DCOLORVALUE* pColor , LPCTSTR strCaption, CWnd* pNotify = NULL, BOOL bAlphaEnable = TRUE);
	DWORD GetCurrentColor() { return m_dwColor; }
	BOOL IsAlphaEnable()	{return m_bAlphaEnable;}
    HWND GetHwnd(){return m_hNotifyHWND;}
	VOID SetAlphaEnable(BOOL bRet);
	VOID UnInit(void);///用于外部强制复位；给父窗口发送一个Close消息

	KColorPickerDialog();

protected:
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedRadioH();
	afx_msg void OnBnClickedRadioS();
	afx_msg void OnBnClickedRadioB();
	afx_msg void OnBnClickedRadioRed();
	afx_msg void OnBnClickedRadioGreen();
	afx_msg void OnBnClickedRadioBlue();
	afx_msg void OnEnChangeEditH();
	afx_msg void OnEnChangeEditS();
	afx_msg void OnEnChangeEditV();
	afx_msg void OnEnChangeEditR();
	afx_msg void OnEnChangeEditG();
	afx_msg void OnEnChangeEditB();
	afx_msg void OnEnChangeEditA();
	afx_msg void OnEnChangeEditARGB();
	afx_msg void OnEnChangeEditARGBHEX();
	afx_msg void OnEnKillfocusEditArgb();
	afx_msg void OnEnKillfocusEditArgbHEX();
	
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
	virtual void PostNcDestroy();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnDestroy();
private:
	VOID SetColor(D3DCOLOR color, BOOL bModifyAlpha);
	VOID SetColor(FLOAT fH, FLOAT fS, FLOAT fV);

	void SetColorCore( D3DCOLOR color );
	//virtual BOOL OnInit();//初始化控件
	void DrawColor1(CDC* pDc,BYTE byRed, BYTE byGreen, BYTE byBlue, const RECT& rect);//显示左边的颜色框, RGB模式
	void DrawColor2(CDC* pDc, BYTE byRed, BYTE byGreen , BYTE byBlue);//显示右边的颜色框， RGB模式
	void DrawColor1HSV(CDC* pDc, float h, float s, float v);
	void DrawColor2HSV(CDC* pDc, float h, float s, float v);
	//void DrawAlpha(CDC* pDc);
	void DrawSelectColor(CDC* pDc);//显示当前选择颜色
	void DrawOriginColor(CDC* pDc);//显示初始颜色
	virtual void ClearVernier(CDC* pDc);//清除游标
	virtual void DrawVernier(CDC* pDc);//画右边的游标
	static void RGB2HSV(DWORD rgb, float& h, float& s, float& v);
	static DWORD HSV2RGB(float h, float s, float v);//输出的Alpha位一定是0
	//VOID UpdateBindedColor();
    VOID Init(DWORD dwColor, LPCTSTR strCaption, CWnd* pNotify, BOOL bAlphaEnable);
	VOID _Init(DWORD dwColor, LPCTSTR strCaption, CWnd* pNotify, BOOL bAlphaEnable);
	//void SetColorPointer(D3DCOLORVALUE* pNewValue, LPCSTR strCaption, CWnd* pNotify = NULL, BOOL bAlpha = TRUE);

	void DrawAll(CDC* pDC);	
	//BOOL m_bCreated;
	//BOOL m_bUpdateBmp1;
	//BOOL m_bUpdateBmp2;
	VOID UpdateBitmaps();

	void UpdateBitmapLeft();
	void UpdateBitmapRight();

	VOID UnBindAllColorPointers();	
};

extern KColorPickerDialog g_ColorPicker;
extern KColorPickerDialog* GetColorPickerDialog();

//<CTH Added>
/*
<@remark 封装能够自动根据KColorPickerDialog的内容更改按钮颜色的按钮>
//
1.在resource.h中声明IDD_COLOR_PICKER
2.在任何窗口放个按钮，把Owner-Draw属性设为true
3.子类化这个按钮为KColorPickerButton。可以用向导做。也可以就自行在Dialog头文件
的控件声明中声明某个按钮为KColorPickerButton，然后在DDX上加上DDX_Control
4.要使用外部颜色初始化，可以在Dialog的OnInitDialog中使用SetBKColor方法，然后要取的时候用GetBKColor取得，也可以用BindColor和UnBindColor来绑定一个颜色。绑定的颜色会在按钮变的时候自动变。
//可以用ON_BN_DOUBLECLICKED来响应单击事件，ON_BN_CLICKED被内部截取了。

*/

class KColorPickerButton :
	public CButton
{
	DECLARE_DYNAMIC(KColorPickerButton)
public:
	KColorPickerButton(void);
	~KColorPickerButton(void);
private:
	//D3DCOLOR m_d3dc_ColorToChange;//用于给Dialog用 // Last modify by Chen Tianhong: 2007-12-10 14:43:18//去掉，用m_d3dc_bkColor代替
	enum
	{
		EM_IS_ALPHA_ENABLE = 0,
		//EM_IS_COLOR_CHANGED = 1,
		EM_IS_COLOR_PANEL_POPPING_DISABLE = 1,
	};
	D3DCOLOR m_d3dc_bkColor;	//用于按钮绘制
	//用于BindColor时，实时显示颜色
	D3DCOLOR* m_pd3dc_BindedColor;
	D3DCOLORVALUE* m_pd3dc_BindedColor_v;
protected:
	std::tstring	m_tczColorPickerDialogCaption;//颜色选择器的标签
	std::bitset<8> m_bitset;	//状态寄存
public:
	void SetBKColor( D3DCOLOR CR_Color );
	D3DCOLOR GetBkColor( void ){return m_d3dc_bkColor;}
	//绑定颜色之后，每当自身颜色改变的时候也会去改变修改的颜色
	HRESULT BindColor( D3DCOLOR* ColorToBind );
    HRESULT BindColor(D3DCOLORVALUE* pColor);

	HRESULT UnBindColor( void );

	void	SetColorPickerDialogCaption( LPCTSTR szCaption );
	void	SetAlphaEnable( BOOL bIsEnable );
	BOOL	IsAlphaEnable(){return m_bitset.test(EM_IS_ALPHA_ENABLE); }
	
	VOID	EnableColorPanelPopping(BOOL bEnable);//如果允许就自动Pop出ColorPanel
	BOOL	IsColorPanelPoppingEnabled(){return !m_bitset.test(EM_IS_COLOR_PANEL_POPPING_DISABLE);}
protected:
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS/*lpDrawItemStruct*/);
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked();
	//afx_msg BOOL OnBnClicked( NMHDR * pNotifyStruct, LRESULT* result );
};
//</CTH Added>