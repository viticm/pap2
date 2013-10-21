// -------------------------------------------------------------------------
//	文件名		：	UiNewPlayer.h
//	创建者		：	彭建波
//	创建时间	：	2002-9-10 14:25:21
//	功能描述	：	角色新建界面－1
//
// -------------------------------------------------------------------------
#ifndef __UINEWPLAYER_H__
#define __UINEWPLAYER_H__

#include "../Elem/WndImage.h"
#include "../Elem/WndEdit.h"
#include "../Elem/WndButton.h"
#include "../Elem/WndShowAnimate.h"
#include "../Elem/WndShadow.h"
#include "../../Login/Login.h"

class KUiNewPlayer : protected KWndShowAnimate
{
public:
	//----界面面板统一的接口函数----
	static KUiNewPlayer* OpenWindow(int nNativePlaceId);//打开窗口，返回唯一的一个类对象实例
	static void			CloseWindow(bool bDestroy);		//关闭窗口，同时可以选则是否删除对象实例
	static bool			IsValidRoleName(const char* pszName, int nLen);
private:
	KUiNewPlayer();
	~KUiNewPlayer();
	void	Initialize();						//初始化
	void	LoadScheme(const char* pScheme);	//载入界面方案
	int		WndProc(unsigned int uMsg, unsigned int uParam, int nParam);
	void	OnClickButton(KWndWindow* pWnd);	//响应点击按钮
	int		GetInputInfo();
	void	OnOk();								//完成角色选择界面
	void	OnCancel();							//返回“角色选择界面”
	void	SelGender();
private:
	static KUiNewPlayer* m_pSelf;
	void	UpdateProperty();				//更新属性说明
	void	Breathe();
private:
	KWndEdit32	m_Name;						// 姓名
	KWndButton	m_OK;						// 确定
	KWndButton  m_Cancel;					// 取消
	KWndButton	m_Male, m_Female;			// 性别按钮
	
	KWndText256	m_PropertyShow;				// 属性说明
	KWndButton	m_Gold;						// 金
	KWndButton	m_Wood;						// 木
	KWndButton	m_Water;					// 水
	KWndButton	m_Fire;						// 火
	KWndButton	m_Earth;					// 土
	
	KRoleChiefInfo		m_Info;
	char				m_szLoginBg[32];
	char				m_szPlayerImgPrefix[128];
	int					m_bJustClicked;
	struct PROPTYPEINFO
	{
		KWndWindow*			pBtn;
		int					nShowTextLen;
		char				propertyShow[256];		//属性说明
		char				szMaleImg[128];
		char				szFemaleImg[128];
		char				szMaleSound[128];
		char				szFemaleSound[128];
	}					m_propTypeInfoTable[series_num];
};

const char*		PROP_GetTitleString(OBJ_ATTRIBYTE_TYPE eType);

#endif // __UINEWPLAYER_H__