////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSelRepresentNPCDlg.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2007-1-22 16:06:47
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSELREPRESENTNPCDLG_H_
#define _INCLUDE_KGSELREPRESENTNPCDLG_H_

class KGSelRepresentNPCDlg : public CDialog
{
	DECLARE_DYNAMIC(KGSelRepresentNPCDlg)

public :
	KGSelRepresentNPCDlg(CWnd* pParent = NULL);   
	virtual ~KGSelRepresentNPCDlg();
	enum { IDD = IDD_DLG_SEL_PRESENT_NPC };

    int GetRenderWndID();

private :
    CListBox    m_listNpcRepresent;
    int         m_nRenderWndId;

protected :
    BOOL OnInitDialog();
    void OnTimer(UINT_PTR nIDEvent);
    void OnOK();
    void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    
	DECLARE_MESSAGE_MAP()
};


#endif //_INCLUDE_KGSELREPRESENTNPCDLG_H_
