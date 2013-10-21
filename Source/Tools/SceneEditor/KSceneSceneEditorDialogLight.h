////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSceneEditorDialogLight.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-3-3 16:04:08
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESCENEEDITORDIALOGLIGHT_H_
#define _INCLUDE_KSCENESCENEEDITORDIALOGLIGHT_H_
#include "KSceneCustomListContrl.h"
#include "KG3DMessenger.h"
////////////////////////////////////////////////////////////////////////////////

// KSceneSceneEditorDialogLight dialog
interface IEKG3DCommonContainer;
interface IEKG3DRepresentObject;
class KSceneSceneEditorDialogLight : public CDialog, public KG3DMessageSingleSenderReceiver
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogLight)

public:
	KSceneSceneEditorDialogLight(CWnd* pParent = NULL);   // standard constructor
	virtual ~KSceneSceneEditorDialogLight();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_LIGHT };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

//////////////////////////////////////////////////////////////////////////
public:
	virtual VOID __stdcall OnReceiveMessage(const KG3DMessage& Message , IKG3DMessageSenderBase* pSender );
private:
	class List : public CListBox 
	{
	public:
		HRESULT Reload();
	};
	List m_ListBox;
private:
	IEKG3DCommonContainer* GetLightTable();
	
public:
	afx_msg void OnBnClickedBnAdd();
	afx_msg void OnBnClickedBnDel();
	afx_msg void OnBnClickedBnClear();
	afx_msg void OnListSelChange();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);

};

#endif //_INCLUDE_KSCENESCENEEDITORDIALOGLIGHT_H_