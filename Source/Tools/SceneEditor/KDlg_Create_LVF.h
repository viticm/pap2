#pragma once


// KDlg_Create_LVF dialog
extern void DecToHex(unsigned int DecNum, string& sHexNum);
extern unsigned int HexToDec(const string& HexNum);

class KDlg_Create_LVF : public CDialog
{
	DECLARE_DYNAMIC(KDlg_Create_LVF)

public:
	KDlg_Create_LVF(CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlg_Create_LVF();

// Dialog Data
	enum { IDD = IDD_CREATE_LVF };

private:
	string m_Path;
	string m_Filename;
	unsigned int m_NumOfFPs;

public:
	void GetPath(string& sPath) const { sPath = m_Path; }
	void GetFilename(string& sFilename) const { sFilename = m_Filename; }
	unsigned int GetNumOfFPs() const { return m_NumOfFPs; }

	//KVolumeFog* GetVolumeFog(KModelTable* pKMT);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	
	afx_msg void OnCreateClicked();
	afx_msg void OnCancelClicked();
	afx_msg void OnClose();

	afx_msg void OnPathChanged();
	afx_msg void OnBrowseClicked();

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFPNumChanged();
};
