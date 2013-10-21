////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSceneEditorDialogMovie.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2007-1-31 10:37:16
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESCENEEDITORDIALOGMOVIE_H_
#define _INCLUDE_KSCENESCENEEDITORDIALOGMOVIE_H_

class IEKG3DSceneSceneEditor;
class KSceneSceneEditorDialogMovie : public CDialog
{
	DECLARE_DYNAMIC(KSceneSceneEditorDialogMovie)

public :
	KSceneSceneEditorDialogMovie(CWnd* pParent = NULL);   
	virtual ~KSceneSceneEditorDialogMovie();

	enum { IDD = IDD_SCENEEDITOR_MOVIE };

    void SetSceneEditor(IEKG3DSceneSceneEditor* pSceneEdit) {
        m_lpSceneSceneEditor = pSceneEdit;
    }
    BOOL OnInitDialog();

private :
    IEKG3DSceneSceneEditor*  m_lpSceneSceneEditor;
    BOOL  m_bRecord;
    BOOL  m_bPlay;
    BOOL  m_bRecAndPlay;

    CComboBox m_ImageType;
    CComboBox m_FiterType;

    DWORD     m_dwEditorStateSave;
    
protected :
	virtual void DoDataExchange(CDataExchange* pDX);   
    void OnOK(){};
    void OnCancel(){};
    
	DECLARE_MESSAGE_MAP()
    afx_msg void OnBnClickedCheckRecord();
public:
    afx_msg void OnBnClickedPlayani();
};


#endif //_INCLUDE_KSCENESCENEEDITORDIALOGMOVIE_H_
