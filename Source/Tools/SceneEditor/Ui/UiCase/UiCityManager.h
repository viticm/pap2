/*******************************************************************************
File        : UiCityManager.h
Creator     : Fyt(Fan Zhanpeng)
create data : 02-11-2004(mm-dd-yyyy)
Description : 帮会城市管理界面
            : |显示实现|-|显示逻辑|-|数据逻辑|-|数据构成|    结构的试验田
********************************************************************************/

#if !defined(AFX_UICITYMANAGER_H__571581A9_0256_4AF3_97BC_CF4F18DEFA47__INCLUDED_)
#define AFX_UICITYMANAGER_H__571581A9_0256_4AF3_97BC_CF4F18DEFA47__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../elem/wndscrollbar.h"
#include "../elem/wndbutton.h"
#include "../elem/wndtext.h"
#include "../elem/wndedit.h"

class KUiCityManager : public KWndImage
{
public:
	KUiCityManager();
	virtual ~KUiCityManager();

	static        KUiCityManager* OpenWindow();		//打开窗口
	static        KUiCityManager* GetIfVisible();	//如果窗口正被显示，则返回实例指针
	static void   CloseWindow(bool bDestory = TRUE);//关闭窗口，同时可以选则是否删除对象实例
	static void   LoadScheme(const char* pScheme);	//载入界面方案


private: //界面标准的内部处理方法
	void          Initialize();						//初始化界面

	//处理界面消息的处理方法
	virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);


private: //标准界面数据定义
	static KUiCityManager* ms_pSelf;


private: //数据构成
	char			m_szTongName[32];
	char			m_szMasterName[32];
	int				m_nTaxRate;				//原始税率
	int				m_nPriceRate;			//原始物价指数
	int				m_nCurrentTaxRate;		//当前设定的税率
	int				m_nCurrentPriceRate;	//当前设定的物价指数

	void			PrepareData();			//准备基础数据

public:  //数据构成部分的接口
	HRESULT UpdateCurrentCityData(char *pszTongName, char *pszMasterName, int nTaxRate, int nPriceRate);


private: //数据逻辑
	int				m_nMaxTax, m_nMinTax;
	int				m_nMaxPrice, m_nMinPrice;

	void			MinTax();
	void			MaxTax();
	void			MinPrice();
	void			MaxPrice();

	void			AdjustPrice(int nCount);
	void			AdjustTax(int nCount);


public:  //数据逻辑部分的接口

	//更新城市设置的参数
	HRESULT UpdateCurrentCityParam(int nMaxTax, int nMinTax, int nMaxPrice, int nMinPrice);


private: //显示逻辑

	//不能够更改设置的界面主题模式
	void			TheCANNOTCHANGE_UI();
	//可以更改设置的界面主题模式
	void			TheCANCHANGE_UI();
	///更新显示
	void			UpdateView();

	//是否接受更改的打开方式
	bool			m_bIsCanChange;


private: //显示实现(D控件自己都做了)

private: //操作挂接
	KWndText32		m_textTongName;
	KWndText32		m_textMasterName;

	KWndEdit32		m_editTax;
	KWndEdit32		m_editPrice;

	KWndButton		m_BtnMinTax, m_BtnMaxTax;
	KWndButton		m_BtnMinPrice, m_BtnMaxPrice;

	KWndButton		m_BtnIncTax, m_BtnDecTax;
	KWndButton		m_BtnIncPrice, m_BtnDecPrice;

	KWndButton		m_BtnClose;
	KWndButton		m_BtnConfirm;
	KWndButton		m_BtnCancel;
	KWndButton		m_BtnHistory;
};

#endif // !defined(AFX_UICITYMANAGER_H__571581A9_0256_4AF3_97BC_CF4F18DEFA47__INCLUDED_)
