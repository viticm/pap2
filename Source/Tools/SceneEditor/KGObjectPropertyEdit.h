////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGObjectPropertyEdit.h
//  Version     : 1.0
//  Author      : ZhaoXinyu (Chiyer)
//  CopyRight   : Kingsoft Corporation All rights Reserved
//  History     : 2005-9-30 14:59:58
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _H_KGOBJECTPROPERTYEDIT_
#define _H_KGOBJECTPROPERTYEDIT_
#pragma once

//#include "KG3DEngineManager.h"
#include "IKG3DObjectPropertyModifior.h"
#include "KGListCtrl.h"
  
enum PROPERTY_EDIT_TYPE
{
	PROPERTY_EDIT_TYPE_OBJECT,
	PROPERTY_EDIT_TYPE_NPC,
	PROPERTY_EDIT_TYPE_PROP,
	PROPERTY_EDIT_TYPE_MAGIC,
	PROPERTY_EDIT_TYPE_EFFECT,
	PROPERTY_EDIT_TYPE_SKILL,

	PROPERTY_EDIT_TYPE_NPC_ANI,
	PROPERTY_EDIT_TYPE_PLAYER_ANI,

	PROPERTY_EDIT_TYPE_EQUIP,

	PROPERTY_EDIT_TYPE_TASE
};

/*
enum ITEM_VILUE_TYPE
{
    VALUE_FILE_PATH,
    VALUE_STRING,
    VALUE_INT,
    VALUE_FLOAT,
    VALUE_BOOL,
    VALUE_COLOR,
	VALUE_TITLE,
	VALUE_SUBTITLE,
    VALUE_TYPE,
	VALUE_UNKNOW
};
*/
class KGObjectPropertyEdit : public CDialog
{
	DECLARE_DYNAMIC(KGObjectPropertyEdit)

public:
	/*
	KGObjectPropertyEdit(
        CString szName, CString szValue, DWORD valueTpye,
        unsigned nCurIndex, CString szCurItem,
        IKG3DObjectPropertyModifior* lpPropertyModifior,
		CString szIniFileName,
		CString szSection,
		PROPERTY_EDIT_TYPE eEditType,
        CWnd* pParent = NULL
    );   
	*/
	KGObjectPropertyEdit(
		IIniFile* pIniFile, CString szSectionName, CString szKeyName,
		CString szPlate, CString szValue, int uItem, 
		KGListCtrl* pList, DWORD dwValueType, CString szDirectory,
		PROPERTY_EDIT_TYPE eEditType, CWnd* pParent = NULL
	);
	virtual ~KGObjectPropertyEdit();

	enum { IDD = IDD_OBJECT_PROPERTY_EDIT };

protected:
	BOOL UpdateObjectData();
	BOOL UpdateNpcAniData();
	virtual void DoDataExchange(CDataExchange* pDX);   
	DECLARE_MESSAGE_MAP()

public:
    IKG3DObjectPropertyModifior* m_lpPropertyModifior;

    CButton         m_buttonView;
    CEdit           m_editNew;
    CEdit           m_editOld;
    CComboBox       m_comboBool;
    CString         m_szComboBool;
    CComboBox       m_comboType;
    CString         m_szComboType;
    CString         m_szPropertyName;
    CString         m_szPropertyValueOld;
    CString         m_szPropertyValueNew;
    //ITEM_VILUE_TYPE m_enumValueType;
    unsigned        m_nCurIndex;
    CString         m_szCurItem;

	//CString         m_szIniFileName;
	IIniFile*       m_pIniFile;
	CString         m_szSectionName;
	CString         m_szKeyName;
	CString         m_szDirectory;
	int             m_nItem;
	KGListCtrl*     m_pList;

	PROPERTY_EDIT_TYPE m_eEditType;

    afx_msg void OnBnClickedOk();
    virtual BOOL OnInitDialog();
    afx_msg void OnBnClickedView();
};

#endif //_H_KGOBJECTPROPERTYEDIT_