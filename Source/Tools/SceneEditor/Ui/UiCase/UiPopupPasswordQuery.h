/*******************************************************************************
File        : UiPopupPasswordQuery.h
Creator     : Fyt(Fan Zhanpeng)
create data : 08-30-2003(mm-dd-yyyy)
Description : 询问密码窗口
********************************************************************************/

#if !defined(AFX_UIPOPUPPASSWORDQUERY_H__136E1BB1_F7B4_425B_927A_E7F805EE7E71__INCLUDED_)
#define AFX_UIPOPUPPASSWORDQUERY_H__136E1BB1_F7B4_425B_927A_E7F805EE7E71__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/*
                    .-.
          .-._    _.../   `,    _.-.
          |   `'-'    \     \_'`   |
          \            '.__,/ `\_.--,
           /                '._/     |
          /                    '.    /
         ;   _                  _'--;
      '--|- (_)       __       (_) -|--'
      .--|-          (__)          -|--.
       .-\-                        -/-.
      '   '.                      .'   `
            '-._              _.-'
                `""--....--""`

*/

#include "../elem/wndbutton.h"
#include "../elem/wndimage.h"
#include "../elem/wndedit.h"

class KUiPopupPasswordQuery : KWndImage
{
public:
	KUiPopupPasswordQuery();
	virtual ~KUiPopupPasswordQuery();

	static        KUiPopupPasswordQuery* OpenWindow(    //打开窗口
                                    KWndWindow *pCaller, unsigned int uParam);

	static        KUiPopupPasswordQuery* GetIfVisible();//如果窗口正被显示，则返回实例指针
	static void   CloseWindow(bool bDestory = FALSE);   //关闭窗口，同时可以选则是否删除对象实例
	static void   LoadScheme(const char* pScheme);      //载入界面方案

private:
	static KUiPopupPasswordQuery* ms_pSelf;

private:
	void          Initialize();                         //初始化

	virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
	void          OnConfirm();

private:
	KWndEdit32             m_Password;                  //密码输入的编辑控件
	KWndButton             m_Confirm;                   //确认按钮
	KWndWindow            *m_pCaller;                   //调用这个窗口的窗口的指针

private:
	unsigned int           m_uParam;                    //别的窗口调用这个窗口时传入
	                                                    //的参数，通常用来识别返回结
	                                                    //果时的处理方法
};

#endif // !defined(AFX_UIPOPUPPASSWORDQUERY_H__136E1BB1_F7B4_425B_927A_E7F805EE7E71__INCLUDED_)
