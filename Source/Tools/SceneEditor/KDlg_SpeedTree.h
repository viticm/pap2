#if !defined(AFX_KDLG_SPEEDTREE_H__0C8EDF68_EF79_421D_9DF7_5386FFD4AAA1__INCLUDED_)
#define AFX_KDLG_SPEEDTREE_H__0C8EDF68_EF79_421D_9DF7_5386FFD4AAA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_SpeedTree.h : header file
//
//#include "KSpeedTree.h"
/////////////////////////////////////////////////////////////////////////////
// KDlg_SpeedTree dialog

void DecToHex(unsigned int DecNum, string& sHexNum)
{
	vector<char> vChars;

	if(DecNum == 0)
	{
		sHexNum = "00";
	}
	else
	{
		unsigned int PosNum = 0;
		while(DecNum > 0)
		{
			++PosNum;
			switch(DecNum & 0x0000000F)
			{
			case 0:
				vChars.push_back('0');
				break;
			case 1:
				vChars.push_back('1');
				break;
			case 2:
				vChars.push_back('2');
				break;
			case 3:
				vChars.push_back('3');
				break;
			case 4:
				vChars.push_back('4');
				break;
			case 5:
				vChars.push_back('5');
				break;
			case 6:
				vChars.push_back('6');
				break;
			case 7:
				vChars.push_back('7');
				break;
			case 8:
				vChars.push_back('8');
				break;
			case 9:
				vChars.push_back('9');
				break;
			case 10:
				vChars.push_back('A');
				break;
			case 11:
				vChars.push_back('B');
				break;
			case 12:
				vChars.push_back('C');
				break;
			case 13:
				vChars.push_back('D');
				break;
			case 14:
				vChars.push_back('E');
				break;
			case 15:
				vChars.push_back('F');
				break;
			}
			DecNum >>= 4;
		}
		if(PosNum % 2 != 0)
		{
			vChars.push_back('0');
		}

		size_t NumOfHexPos = vChars.size();

		sHexNum.clear();
		for(size_t i = 0; i < NumOfHexPos; ++i)
		{
			sHexNum += vChars[NumOfHexPos - i - 1];
		}
	}
}

unsigned int HexToDec(const string& HexNum)
{
	if(HexNum.empty())
	{
		return 0;
	}

	unsigned int NumOfHexPos = (unsigned)HexNum.size();
	unsigned int i = 0;
	bool bZero = true;
	for(i = 0; i < NumOfHexPos; ++i)
	{
		if(HexNum.at(i) != '0')
		{
			bZero = false;
			break;
		}
	}
	if(bZero)
	{
		return 0;
	}

	unsigned int Ret = 0;

	for(i = 0; i < NumOfHexPos && i < 8; ++i)
	{
		char c = HexNum.at(i);
		switch(c)
		{
		case '0':
			Ret += 0;
			break;
		case '1':
			Ret += 1;
			break;
		case '2':
			Ret += 2;
			break;
		case '3':
			Ret += 3;
			break;
		case '4':
			Ret += 4;
			break;
		case '5':
			Ret += 5;
			break;
		case '6':
			Ret += 6;
			break;
		case '7':
			Ret += 7;
			break;
		case '8':
			Ret += 8;
			break;
		case '9':
			Ret += 9;
			break;
		case 'A':
		case 'a':
			Ret += 10;
			break;
		case 'B':
		case 'b':
			Ret += 11;
			break;
		case 'C':
		case 'c':
			Ret += 12;
			break;
		case 'D':
		case 'd':
			Ret += 13;
			break;
		case 'E':
		case 'e':
			Ret += 14;
			break;
		case 'F':
		case 'f':
			Ret += 15;
			break;
		}
		Ret <<= 4;
	}

	return Ret;
}

class KDlg_SpeedTree : public CPropertyPage
{
// Construction
public:
	KDlg_SpeedTree(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(KDlg_SpeedTree)
	enum { IDD = IDD_DIALOG_SPEEDTREE };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_SpeedTree)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

protected:
	float m_Swing;
	float m_MinVel;
	float m_MaxVel;
	float m_MinWidth;
	float m_MaxWidth;
	float m_MinHeight;
	float m_MaxHeight;
	int m_SegNum;
	float m_Lx, m_Ly, m_Lz;
	D3DCOLOR m_LightColor;
	D3DCOLOR m_AmbientColor;
	float m_AlphaFactor;
	string m_TexFilename;
	string m_TexFile;
	IDirect3DTexture9* m_pTex;
	bool m_bNeedReleaseTex;
	bool m_bReady;

public:
	//KSpeedTree* m_pST;

	void Create();
	void Show();

	//void SetSpeedTree(KSpeedTree* pST);
	void Apply();

	void LightingChanged();

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(KDlg_SpeedTree)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	afx_msg void OnClose();
	afx_msg void OnOKClicked();
	afx_msg void OnCancelClicked();
	afx_msg void OnSaveClicked();
	
	afx_msg void OnSwingChanged();
	afx_msg void OnMinVelChanged();
	afx_msg void OnMaxVelChanged();
	afx_msg void OnMaxWidthChanged();
	afx_msg void OnMaxHeightChanged();
	afx_msg void OnSegNumChanged();
	afx_msg void OnLXChanged();
	afx_msg void OnLYChanged();
	afx_msg void OnLZChanged();
	afx_msg void OnLightColorChanged();
	afx_msg void OnAmbientColorChanged();
	afx_msg void OnLightColorTableClicked();
	afx_msg void OnAmbientColorTableClicked();
	afx_msg void OnTexFilenameChanged();
	afx_msg void OnTexFilenameButtonClicked();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnResetClicked();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_SPEEDTREE_H__0C8EDF68_EF79_421D_9DF7_5386FFD4AAA1__INCLUDED_)
