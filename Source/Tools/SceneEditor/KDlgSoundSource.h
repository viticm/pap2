#pragma once
#include "afxcmn.h"
#include "kgtreectrl.h"


// KDlgSoundSource dialog

enum
{
	IMAGE_ERROR =		-1,
	IMAGE_DIR =			0,
	IMAGE_WAV =         1
};
class KDlgSoundTag;
class KDlgSoundSource : public CDialog
{
	DECLARE_DYNAMIC(KDlgSoundSource)

public:
	KDlgSoundSource(CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlgSoundSource();

	// Dialog Data
	enum { IDD = IDD_DIALOG_SOUNDSOURCE  };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	HRESULT FillTreeWithDir(LPSTR Dir,LPSTR pFileType,HTREEITEM hStart);
	int GetItemImage(LPSTR extName);
public:
	HRESULT FillTree();
	KGTreeCtrl m_Tree;
	virtual BOOL OnInitDialog();
	afx_msg void OnNMClickTreeSoundsource(NMHDR *pNMHDR, LRESULT *pResult);
	CImageList  m_imgList;
	LPSTR       m_pszTreeTouch; //传出的文件名
	TCHAR       m_szTreeTouch[MAX_PATH];// 暂存左键点中的文件
protected:
	void GetItemFilePath(HTREEITEM hItem);
	BOOL m_bIsWav;
public:
	afx_msg void OnBnClickedButtonRefresh();
	afx_msg void OnBnClickedButtonOk();
	HRESULT SetTreeTouch(LPSTR pszTreeTouch);
	afx_msg void OnClose();
};
