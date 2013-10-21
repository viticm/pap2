#pragma once
#include "afxcmn.h"
#include "ConfigDefine.h"
#include "KIniOperator.h"

// KPageOfConfig dialog


class KClientSetting : public CDialog
{
	DECLARE_DYNAMIC(KClientSetting)

public:
	KClientSetting(CWnd* pParent = NULL);   // standard constructor
	virtual ~KClientSetting();

// Dialog Data
	enum { IDD = IDD_CONFIG_DLG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	void Save();
private:
	//OPTION_GROUP m_eCurrentOption;
	DIALOG_ITEM_VALUE_GROUP m_Option;
	KIniOperator m_IniFile;
	int          m_nInited;
	
	CSliderCtrl m_SliderCammeraDistance;
	CSliderCtrl m_SliderWaterDetail;
	CSliderCtrl m_SliderTerrainDetail;
	CSliderCtrl m_SliderTextureScale;

private:
	void _ChangeConfig(OPTION_GROUP eOptionID);
	void _ChangeCtrlStatus();
	void _GetItemValue();
	int  _InitResolution();
	int  _InitSlider();

public:
	int Init();
	afx_msg void OnBnClickedRadioLow();
	afx_msg void OnBnClickedRadioMID();
	afx_msg void OnBnClickedRadioHIGH();
	afx_msg void OnBnClickedRadioCUSTOM();
	afx_msg BOOL PreTranslateMessage(MSG* pMsg);
    afx_msg void OnBnClickedCheckFullScreen();
    afx_msg void OnBnClickedCheckExclusive();
};
