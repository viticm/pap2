/* 
 * File:     UiStallSetAdv.h
 * Desc:     摆摊广告
 * Author:   flying
 * Creation: 2003/10/23
 */
#if !defined _UISTALLSETADV_HEADER
#define _UISTALLSETADV_HEADER

#include "../Elem/WndEdit.h"

// "Edit" with 2048 byte buffer
class KAdvEdit : public KWndEdit
{
private:
	char			m_Text[200];
public:
	KAdvEdit();
};

class KUiStallSetAdv : private KWndImage
{
public:
	//打开窗口，返回唯一的一个类对象实例
	static KUiStallSetAdv* OpenWindow(bool bShow, bool bCallBack = false);
	//关闭窗口，同时可以选则是否删除对象实例
	static void				CloseWindow(bool bDestroy);
	static KUiStallSetAdv*	GetIfVisible();
	//载入界面方案
	void					LoadScheme(const char* pScheme);
private:
	static KUiStallSetAdv* m_pSelf;
	KUiStallSetAdv();
	virtual ~KUiStallSetAdv();
	void Initialize();
	int WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

	void OnOK();
	void OnCancel();

	bool		m_bCallBack;
	KAdvEdit	AdvEditor;
	KWndButton	m_BtnOk;
	KWndButton	m_BtnCancel;
};

#endif
