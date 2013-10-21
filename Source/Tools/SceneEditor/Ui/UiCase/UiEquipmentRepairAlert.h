/*******************************************************************************
File        : UiEquipmentRepairAlert.h
Creator     : Fyt(Fan Zhanpeng)
create data : 10-15-2003(mm-dd-yyyy)
Description : 物品修理警告界面
********************************************************************************/


#if !defined(AFX_EQUIPMENTREPAIRALERT_H__DAAACD58_7AB6_4DCA_8DCA_09765EDB7CA3__INCLUDED_)
#define AFX_EQUIPMENTREPAIRALERT_H__DAAACD58_7AB6_4DCA_8DCA_09765EDB7CA3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../elem/wndimage.h"

class KUiEquipmentRepairAlert : public KWndImage
{
public:
	KUiEquipmentRepairAlert();
	virtual ~KUiEquipmentRepairAlert();

	static        KUiEquipmentRepairAlert* OpenWindow();  //打开窗口
	static        KUiEquipmentRepairAlert* GetIfVisible();//如果窗口正被显示，则返回实例指针
	static void   CloseWindow(bool bDestory = TRUE);      //关闭窗口，同时可以选则是否删除对象实例
	static void   LoadScheme(const char* pScheme);        //载入界面方案
	static        KUiEquipmentRepairAlert* GetIfCreate(); //如果窗体已经被创建，则返回对象实例，否则返回NULL

	virtual void  Show(int nDur, char* szName); //显示窗口

	//设置要修理的那个装备的Tip
	int			  SetTargetItemTip(char *szName, int nbIsAlmostCrash = 0);

private:
	static        KUiEquipmentRepairAlert *ms_pSelf;

private:
	void          Initialize();
	virtual int   WndProc(unsigned int uMsg, unsigned int uParam, int nParam);

private:
	int           m_nYellowAlertSignal;
	int           m_nRedAlertSignal;

	char		  m_szTip[64];
	int			  m_nItemIndex;
};

#endif // !defined(AFX_EQUIPMENTREPAIRALERT_H__DAAACD58_7AB6_4DCA_8DCA_09765EDB7CA3__INCLUDED_)
