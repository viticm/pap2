#pragma once
#include "resource.h"
#include <d3d9.h>
#include <bitset>

// KColorPickerDialog dialog
#define GETRED(SRC)   ((SRC&0x00ff0000)>>16)
#define GETGREEN(SRC) ((SRC&0x0000ff00)>> 8)
#define GETBLUE(SRC)  (SRC&0x000000ff)
#define GETALPHA(SRC) (SRC>>24)

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

private:
	//////////////////////////////////////////////////////////////////////////
	//输入控件
	CButton m_ctlChannel[3];
	CEdit m_ctlEditChannel[4];
	CStatic m_ctlAlphaStatic;
	BOOL m_bAutoUpdate;
	//////////////////////////////////////////////////////////////////////////
	bool m_bUseColorValue;
	virtual BOOL OnInit();//初始化控件
	void DrawColor1(CDC* pDc,BYTE byRed, BYTE byGreen, BYTE byBlue, const RECT& rect, BOOL bUpdateBmp = TRUE);//显示左边的颜色框, RGB模式
	void DrawColor2(CDC* pDc, BYTE byRed, BYTE byGreen , BYTE byBlue, BOOL bUpdateBmp = TRUE);//显示右边的颜色框， RGB模式
	void DrawColor1HSV(CDC* pDc, float h, float s, float v, BOOL bUpdateBmp = TRUE);
	void DrawColor2HSV(CDC* pDc, float h, float s, float v, BOOL bUpdateBmp = TRUE);
	void DrawAlpha(CDC* pDc);
	void DrawSelectColor(CDC* pDc);//显示当前选择颜色
	void DrawOriginColor(CDC* pDc);//显示初始颜色
	virtual void ClearVernier(CDC* pDc);//清除游标
	virtual void DrawVernier(CDC* pDc);//画右边的游标

	BOOL m_bVernierSelect;//是否选中了右边的颜色框的游标
	BOOL m_bAlphaEnable;//是否要修改Alpha值
	BOOL m_bColor1Select;//是否在左边颜色框中按下了鼠标
	BOOL m_bAlphaSelect;//是否选中了Ａｌｐｈａ的游标

	//////////////////////////////////////////////////////////////////////////
	//布局参数
	static int m_nWidth2;//右边颜色框的宽度
	RECT m_rectClearRgn[3];//清除框的位置
	RECT m_rectOriginColor;//初始颜色框位置
	RECT m_rectSelectColor;//当前选择颜色框位置
	RECT m_rectAlpha;//Alpha条的位置
	RECT m_rectAlphaRgn;//Alpha条的相应区域
	static const POINT m_ptVernier;//游标的大小
	static const RECT m_rectVernierRgn;//游标的响应区域
	static const int m_nSpan;//游标到颜色框的距离
	static const RECT m_rectColor1;//左边颜色框的位置
	static const RECT m_rectColor2;//右边颜色框的位置
	//////////////////////////////////////////////////////////////////////////


	//////////////////////////////////////////////////////////////////////////
	//辅助参数
	CBitmap* m_pbmpColor1;//左边颜色框对应的位图
	CBitmap* m_pbmpColor2;//右边颜色框对应的位图
	CBitmap* m_pBmpAlpha;//Alpha对应的位图

	DWORD m_dwColor;//用来中转的颜色
	D3DCOLORVALUE* m_pColorValue;

	DWORD* m_pColorBuffer1;
	DWORD* m_pColorBuffer2;
	DWORD* m_pAlphaBuffer;

	BOOL m_bAlphaMoved;
	BOOL m_bVernierMoved;

	RECT m_rectSelAndOrg;
	int m_nGridSize;	
	BYTE m_byChannel[4];
	float m_HSV[3];
	BOOL m_bHSVModel;
	enumChannel m_enumLockChannel;//当前锁定的通道(在右边颜色框才能修改的通道)
	//////////////////////////////////////////////////////////////////////////
public:
	DWORD* m_pColor;//输入的颜色的地址
	DWORD m_dwColorOrg;//初始颜色的值
	CWnd* m_pNotify;
public:
	void Init(DWORD* pColor, LPCSTR strCaption, CWnd* pNotify = NULL ,BOOL bAlphaEnable = TRUE);
	void Init(D3DCOLORVALUE* pColor , LPCSTR strCaption, CWnd* pNotify = NULL, BOOL bAlphaEnable = TRUE);
	KColorPickerDialog();
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
protected:
	virtual void PostNcDestroy();
	void RGB2HSV(DWORD rgb, float& h, float& s, float& v);
	DWORD HSV2RGB(float h, float s, float v);
	virtual void OnOK();
public:
	afx_msg void OnBnClickedRadioH();
	afx_msg void OnBnClickedRadioS();
	afx_msg void OnBnClickedRadioB();
	afx_msg void OnBnClickedRadioRed();
	afx_msg void OnBnClickedRadioGreen();
	afx_msg void OnBnClickedRadioBlue();
	//void SetColorPointer(D3DCOLORVALUE* pNewValue, LPCSTR strCaption, CWnd* pNotify = NULL, BOOL bAlpha = TRUE);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
	VOID UnInit(void);///用于外部强制复位；给父窗口发送一个Close消息
	BOOL m_bCreated;
	BOOL m_bUpdateBmp1;
	BOOL m_bUpdateBmp2;
public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	DWORD GetCurrentColor() { return *m_pColor; }
};

extern KColorPickerDialog g_ColorPicker;
extern KColorPickerDialog* GetColorPickerDialog();

//<CTH Added><@remark 封装能够自动根据KColorPickerDialog的内容更改按钮颜色的按钮>
//只需要在h文件的控件声明中声明某个按钮为这种类，然后打开按钮的Owner-Draw属性
//就可以得到自绘制的特性
//要使用外部颜色初始化，可以在Dialog的OnInitDialog中使用SetBKColor方法，然后要取的时候用GetBKColor取得，也可以用BindColor和UnBindColor来绑定一个颜色

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
	};
	D3DCOLOR m_d3dc_bkColor;	//用于按钮绘制
	//用于BindColor时，实时显示颜色
	D3DCOLOR* m_pd3dc_BindedColor;
	D3DCOLORVALUE* m_pd3dc_BindedColor_v;
protected:
	std::string	m_tczColorPickerDialogCaption;//颜色选择器的标签
	std::bitset<8> m_bitset;	//状态寄存
public:
	void SetBKColor( D3DCOLOR CR_Color );
	D3DCOLOR GetBkColor( void ){return m_d3dc_bkColor;}
	//绑定颜色之后，每当自身颜色改变的时候也会去改变修改的颜色
	HRESULT BindColor( D3DCOLOR* ColorToBind )
	{
		UnBindColor();

		m_pd3dc_BindedColor =  ColorToBind;
		m_d3dc_bkColor      = *ColorToBind;
		return S_OK;
	}
	HRESULT BindColor(D3DCOLORVALUE* pColor)
	{
		UnBindColor();

		m_pd3dc_BindedColor_v = pColor;
		m_d3dc_bkColor = D3DCOLOR_COLORVALUE(pColor->r, pColor->g, pColor->b, pColor->a);
		return S_OK;
	}

	HRESULT UnBindColor( void )
	{
		m_pd3dc_BindedColor = NULL;
		m_pd3dc_BindedColor_v = NULL;
		return S_OK;
	}

	void	SetColorPickerDialogCaption( LPCSTR szCaption ){m_tczColorPickerDialogCaption = szCaption ? szCaption :_T("");}
	void	SetAlphaEnable( BOOL bIsEnable ){m_bitset.set(EM_IS_ALPHA_ENABLE, bIsEnable?true : false);}
	BOOL	IsAlphaEnable(){return m_bitset.test(EM_IS_ALPHA_ENABLE)?TRUE:FALSE; }
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS/*lpDrawItemStruct*/);
protected:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
public:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClicked();
};
//</CTH Added>