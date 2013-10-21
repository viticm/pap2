////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KGSFXMessageBox.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2006-9-2 10:41:40
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KGSFXMESSAGEBOX_H_
#define _INCLUDE_KGSFXMESSAGEBOX_H_

DWORD SFXMessageBox(LPSTR szMessage, LPSTR szTitle, DWORD dwType);
enum
{
    SFX_MESSAGE_STUEDE_LOAD,
    SFX_MESSAGE_TIPS
};

enum
{
    SFXMESSAGE_OK,
    SFXMESSAGE_CANCEL,
    SFXMESSAGE_COVER,
    SFXMESSAGE_ADD,
    SFXMESSAGE_YES,
    SFXMESSAGE_NO
};

class KGSFXMessageBox : public CDialog
{
	DECLARE_DYNAMIC(KGSFXMessageBox)

public :
	KGSFXMessageBox(
        LPSTR szMessage,
        LPSTR szTitle,
        DWORD dwType = SFX_MESSAGE_TIPS,
        CWnd* pParent = NULL
    );   
	virtual ~KGSFXMessageBox();
	enum { IDD = IDD_SFX_MESSAGEG_BOX };
    BOOL OnInitDialog();
    DWORD GetResult();

public :
    CString m_strInformation;
    CString m_strTitle;
    DWORD   m_dwType;
    DWORD   m_dwReturn;

protected :
	virtual void DoDataExchange(CDataExchange* pDX);   
    void OnClickBottonA();
    void OnClickBottonB();
    void OnClickBottonC();
	DECLARE_MESSAGE_MAP()
};


#endif //_INCLUDE_KGSFXMESSAGEBOX_H_
