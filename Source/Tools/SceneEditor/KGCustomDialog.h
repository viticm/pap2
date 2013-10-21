////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGCustomDialog.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2006-4-13 20:08:45
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGCustomDialog_H_
#define _INCLUDE_KGCustomDialog_H_

#include "Resource.h"
#include "Engine.h"

#define CTRL_NUM  8

enum BASE_DATA_TYPE
{
	TYPE_MULTI_STRING,
	TYPE_SINGL_STRING,
	TYPE_INTEGER,
	TYPE_BOOL_EX,
	TYPE_COLOR,

	TYPE_UNKNOW
};

struct CustomCtrl
{
	CString			m_szPropertyName;
	CString         m_szCtrlName;
	CWnd*           m_pWndCtrl;
	BASE_DATA_TYPE  m_eCtrlType;
	CString         m_szValue;
	CStatic			m_staticCaption;
	CSpinButtonCtrl m_spinCtrl;

	CustomCtrl();

	int Init(
		CString m_szPropertyName, CString szCtrlName, BASE_DATA_TYPE eCtrlType, 
		const INT pnCoor[], CWnd* pParent, int nID
	);
};

class KGCustomDialog : public CDialog
{
	DECLARE_DYNAMIC(KGCustomDialog)

public :
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	BASE_DATA_TYPE GetType(const TCHAR szType[]);

public :
	KGCustomDialog(IIniFile* pIniFile, TCHAR pcszSectionName[], CWnd* pParent = NULL);
	virtual ~KGCustomDialog(void);
	enum { IDD = IDD_DIALOG_CUSTOM_INTERFACE };

private :
	// Data
	IIniFile*  m_pIniFile;
	TCHAR      m_szSectionName[64];

	// Ctrl
	CustomCtrl m_ctrlArray[CTRL_NUM];
	

public :
	virtual void DoDataExchange(CDataExchange* pDX);  
	afx_msg void OnBnClickedOk();

	DECLARE_MESSAGE_MAP()
};

#endif //_INCLUDE_KGCustomDialog_H_
