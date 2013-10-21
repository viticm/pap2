#pragma once

#include "..\KG3DSFXFileDefine.h"
#include "IEKG3DSfx.h"
#include "IEKG3DSceneSFXEditor.h"

class KGImportSfxDlg : public CDialog
{
	DECLARE_DYNAMIC(KGImportSfxDlg)

    struct SfxEditImportElement {
        SfxImportElement sie;
        bool enable; 
    };
    typedef std::vector<SfxEditImportElement> SfxEditImportInfo;

    struct CheckBoxInfo {
        RECT rect;
        int  num;
    };
    CheckBoxInfo m_aCheckBoxInfo[SFX_FEID_SIZE];
    CButton m_aCheckBoxs[100];
    int     m_nCheckBoxNum;

public:
	KGImportSfxDlg(CWnd* pParent, BYTE* pBuffer, IEKG3DSFX* pCurSfx);   // standard constructor
	virtual ~KGImportSfxDlg();

    SfxEditImportInfo m_SfxImportInfo;
    BYTE* m_pBuffer;
    IEKG3DSFX* m_pCurSfx;

    DWORD m_dwImpInterval;
    DWORD m_dwImpFrameNum;
    DWORD m_dwPrecept;  /* 1 : scanl import sfx 2 : cut import sfx 3 : scanl current sfx*/

// Dialog Data
	enum { IDD = IDD_DIALOG_INPORT_SFX };

    SfxImportInfo GetImportInfo(DWORD& dwImpFrameNum, DWORD& dwImpInterval, DWORD& dwPrecept);
    void UpdateUI();
    BOOL OnInitDialog();
    LPCTSTR GetCheckBoxName(DWORD dwType);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    afx_msg void OnCheckBoxUpdate(UINT CmdId);

	DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnBnClickedScanlImpsfx();
    afx_msg void OnBnClickedCutImpsfx();
    afx_msg void OnBnClickedScanlCursfx();
};
