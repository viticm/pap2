#ifndef _KMAINTABCTRL_H_
#define _KMAINTABCTRL_H_

// KMainTabCtrl
#include "KClientSetting.h"
#include "KClientEnvironment.h"

#define PAGE_MAX 32

class KMainTabCtrl : public CTabCtrl
{
	DECLARE_DYNAMIC(KMainTabCtrl)

public:
	KMainTabCtrl();
	virtual ~KMainTabCtrl();

public:
	int Init();
	void ConfigSave();

protected:
	DECLARE_MESSAGE_MAP()

private:
	void _SetRectangle();

private:
	KClientEnvironment m_ClientInfo;
	KClientSetting   m_ConfigPage;

private:
	int m_nCurrent;
	
public:
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};

#endif