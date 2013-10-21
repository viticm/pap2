#pragma once


// KMovieOptionDialog dialog

class KMovieOptionDialog : public CDialog
{
	DECLARE_DYNAMIC(KMovieOptionDialog)

public:
	KMovieOptionDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KMovieOptionDialog();

// Dialog Data
	enum { IDD = IDD_SCENEEDITOR_MOVIE };
	
	BOOL OnInitDialog();

private:
	BOOL  m_bRecord;
	BOOL  m_bPlay;
	BOOL  m_bRecAndPlay;

	CComboBox m_ImageType;
	CComboBox m_FiterType;

	DWORD     m_dwEditorStateSave;
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
};
