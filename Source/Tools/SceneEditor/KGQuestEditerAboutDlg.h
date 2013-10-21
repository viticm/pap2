#pragma once


#define  QUEST_HELP_FILE "settings\\QuestHelp.txt"

class KGQuestEditerAboutDlg : public CDialog
{
	DECLARE_DYNAMIC(KGQuestEditerAboutDlg)

public:
	KGQuestEditerAboutDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~KGQuestEditerAboutDlg();
	enum { IDD = IDD_QUEST_EDIT_ABOUT };
	virtual BOOL OnInitDialog();
public:
	CString m_szText;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
};
