#if !defined(AFX_KDLG_PROPERTYPAGE_H__8CAA101B_DC0E_4C79_B7BB_382A23105B36__INCLUDED_)
#define AFX_KDLG_PROPERTYPAGE_H__8CAA101B_DC0E_4C79_B7BB_382A23105B36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KDlg_PropertyPage.h : header file
//
#include "SFXBase.h"

namespace PropertyBase
{
/////////////////////////////////////////////////////////////////////////////
// KDlg_PropertyPage dialog
#define GROUP_START  20000
#define SCORLLBAR    19990
#define CONTROL_START 25000


class KDlg_PropertyPage;

class _ProItemBase
{ 
public:
	DWORD    m_Type;
	CString  m_Name;
	CDialog* m_lpDailog;
public:
	_ProItemBase()
	{
		m_Type = 0;
		m_Name = "";
		m_lpDailog = NULL;
	}
	~_ProItemBase()
	{
		SAFE_DELETE(m_lpDailog);
	}

	virtual int MoveWindow(int Left,int Top,int Width)
	{
		if(m_lpDailog)
		{
			CRect RectSrc;
			m_lpDailog->GetWindowRect(&RectSrc);
			CRect RectDest;
			RectDest.left  = Left+2;
			RectDest.right = Left+2+RectSrc.Width();
			RectDest.top    = Top;
			RectDest.bottom = Top + RectSrc.Height();

			m_lpDailog->MoveWindow(RectDest);

			return Top + RectSrc.Height();
		}
		else
		{
			return Top + 30;
		}
	}

	virtual void ShowWindow(BOOL bShow)
	{
		if (m_lpDailog)
		{
			m_lpDailog->ShowWindow(bShow);
		}
	}
};

class _ProItemInput_Int : public _ProItemBase
{
public:

	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_Bool : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_Float : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_Enum : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

//////////////////////////////////////////////////////////////////////////
class _ProItemInput_TimePoint:public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_TimePoint_Color:public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};
//////////////////////////////////////////////////////////////////////////

class _ProItemInput_String : public _ProItemBase
{ 
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_Color : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_MtlList : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_MtlTex : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_MtlOption : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_MtlState : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_MtlEffect : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_Ani : public _ProItemBase
{ 
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_SFXNew : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_Terrain_Ground_Texture : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_SceneObjectTable : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_Particle : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class _ProItemInput_Position : public _ProItemBase
{
public:
	int Create(int Top,int Width,PropertyBase::_VarProperty* pPro,KDlg_PropertyPage* pParent);
};

class KDlg_PropertyPage : public CDialog
{ 
// Construction
public:
	UINT GetNewControlID();
	void RecalcLayout();
	void SetPropertyPage(PropertyBase::_PropertyPage* pPage);

	KDlg_PropertyPage(CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlg_PropertyPage();

// Dialog Data
	//{{AFX_DATA(KDlg_PropertyPage)
	enum { IDD = IDD_PROPAGE };

	//}}AFX_DATA
	CScrollBar	m_Scroll;
	BOOL        m_bScrollBarCreated;
	BOOL        m_bEnableScroll;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(KDlg_PropertyPage)
	public:
	virtual void OnFinalRelease();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	//}}AFX_VIRTUAL

// Implementation
public:
	struct _Group
	{
		string    Name;
		CStatic*  pStatic;//Group的边框
		CButton*  pButton;//Group上方的按钮，用来展开或者合上Group
		int       m_Height;//Group高度
		int       m_Top;
		int       m_Width;
		BOOL      m_bExtracted;//是否展开

		vector<_ProItemBase*>m_vecItem;

		void ShowWindow(IN int nCmdShow)
		{
			for(DWORD i=0;i<m_vecItem.size();i++)
			{
				if(m_vecItem[i]->m_lpDailog)
				{
					m_vecItem[i]->m_lpDailog->ShowWindow(nCmdShow);
				}
			}
		}
		void EnableWindow(BOOL bEnable)
		{
			for(DWORD i=0;i<m_vecItem.size();i++)
			{
				if(m_vecItem[i]->m_lpDailog)
				{
					m_vecItem[i]->m_lpDailog->EnableWindow(bEnable);
				}
			}
		}
		HRESULT CreateItems(PropertyBase::_PropertyGroup* pGroup,KDlg_PropertyPage* pBase)
		{
			int m_Top = 0;
			for(DWORD i=0;i<pGroup->PropertyVector.size();i++)
			{
				PropertyBase::_VarProperty* pPro = pGroup->PropertyVector[i];
				switch(pPro->Type)
				{
				//////////////////////////////////////////////////////////////////////////
				
				case PropertyBase::_VarProperty::VAR_TIMEPOINT:
					{
						_ProItemInput_TimePoint* pTimePoint = new _ProItemInput_TimePoint();
						pTimePoint->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pTimePoint);
						break;
					}
				case PropertyBase::_VarProperty::VAR_TIMEPOINT_COLOR:
					{
						_ProItemInput_TimePoint_Color* pTimePointColor = new _ProItemInput_TimePoint_Color();
						pTimePointColor->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pTimePointColor);
						break;
					}
				//////////////////////////////////////////////////////////////////////////
				case PropertyBase::_VarProperty::VAR_INT:
					{
						_ProItemInput_Int* pIntInput = new _ProItemInput_Int();
						pIntInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pIntInput);
						break;
					}
				case PropertyBase::_VarProperty::VAR_BOOL:
					{
						_ProItemInput_Bool* pBoolInput = new _ProItemInput_Bool();
						pBoolInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pBoolInput);
						break;
					}
				case PropertyBase::_VarProperty::VAR_FLOAT:
					{
						_ProItemInput_Float* pInput = new _ProItemInput_Float();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}
				case PropertyBase::_VarProperty::VAR_STRING:
					{
						_ProItemInput_String* pInput = new _ProItemInput_String();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}
				case PropertyBase::_VarProperty::VAR_STLSTRING:
					{
						_ProItemInput_String* pInput = new _ProItemInput_String();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}		
				case PropertyBase::_VarProperty::VAR_ENUM:
					{
						_ProItemInput_Enum* pInput = new _ProItemInput_Enum();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}	
				case PropertyBase::_VarProperty::VAR_COLOR:
				case PropertyBase::_VarProperty::VAR_COLORVALUE:
					{
						_ProItemInput_Color* pInput = new _ProItemInput_Color();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}	
				case PropertyBase::_VarProperty::VAR_MTLLIST:
					{
						_ProItemInput_MtlList* pInput = new _ProItemInput_MtlList();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}	
				case PropertyBase::_VarProperty::VAR_MTLTEX:
					{
						_ProItemInput_MtlTex* pInput = new _ProItemInput_MtlTex();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}
				case PropertyBase::_VarProperty::VAR_MTLOPTOIN:
					{
						_ProItemInput_MtlOption* pInput = new _ProItemInput_MtlOption();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}			
				case PropertyBase::_VarProperty::VAR_MTLSTATE:
					{
						_ProItemInput_MtlState* pInput = new _ProItemInput_MtlState();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}	
				case PropertyBase::_VarProperty::VAR_MTLEFFECT:
					{
						_ProItemInput_MtlEffect* pInput = new _ProItemInput_MtlEffect();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}	
				case PropertyBase::_VarProperty::VAR_ANILIST:
					{
						_ProItemInput_Ani* pInput = new _ProItemInput_Ani();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}	

				case PropertyBase::_VarProperty::VAR_MODELEDITORSFXNEW:
					{
						_ProItemInput_SFXNew* pInput = new _ProItemInput_SFXNew();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}	
				case PropertyBase::_VarProperty::VAR_SCENEEDITOROBJECT:
					{
						_ProItemInput_SceneObjectTable* pInput = new _ProItemInput_SceneObjectTable();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}	
				case PropertyBase::_VarProperty::VAR_POSITION:
					{
						_ProItemInput_Position* pInput = new _ProItemInput_Position();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}
				case PropertyBase::_VarProperty::VAR_PARTICLE:
					{
						_ProItemInput_Particle* pInput = new _ProItemInput_Particle();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}			
				case PropertyBase::_VarProperty::VAR_TERRAIN_GROUND_TEXTURE:
					{
						_ProItemInput_Terrain_Ground_Texture* pInput = new _ProItemInput_Terrain_Ground_Texture();
						pInput->Create(m_Top,200,pPro,pBase);
						m_vecItem.push_back(pInput);
						break;
					}		
				}
				
			}
			return S_OK;
		}
		

		_Group()
		{
			pStatic = NULL;
			pButton = NULL;
			m_Height = 30;
			m_bExtracted = TRUE;
		}

		HRESULT Create(int Index,PropertyBase::_PropertyGroup* pGroup,KDlg_PropertyPage* pBase)
		{
			if(pGroup)
			{
				Name = pGroup->scGroupName;

				TCHAR Str[256];
				wsprintf(Str,"+%s",Name.c_str());

				int Height = 150;
				CRect Rect(3,10 + Height * Index,250,Height * (Index+1));

				pStatic = new CStatic();
				pStatic->Create(Str,
					SS_BLACKFRAME|SS_LEFT,Rect,pBase,GROUP_START+Index);
				pStatic->ShowWindow(TRUE);

				CRect Rect2(3,10 + Height * Index,250,Height * Index + 30);
				pButton = new CButton;
				pButton->Create(Str,
					BS_PUSHLIKE|BS_CHECKBOX,Rect2,pBase,GROUP_START+Index);
				pButton->ShowWindow(TRUE);

				m_Height = 100;
				m_bExtracted = TRUE;
			}
			return S_OK;
		}

		_Group(int Index,PropertyBase::_PropertyGroup* pGroup,KDlg_PropertyPage* pBase)
		{
			Create(Index,pGroup,pBase);
		}

		HRESULT Clear()
		{
			for(DWORD i=0;i<m_vecItem.size();i++)
			{
				_ProItemBase* pPro = m_vecItem[i];
				SAFE_DELETE(pPro);
			}
			m_vecItem.clear();

			SAFE_DELETE(pButton);
			SAFE_DELETE(pStatic);

			return S_OK;
		}

		void SetExtracted(BOOL Ex)
		{
			m_bExtracted =  Ex;
			pButton->SetCheck(m_bExtracted);
		}

		int OnMove(int Top,KDlg_PropertyPage* pBase)
		{
			m_bExtracted = pButton->GetCheck();

			TCHAR Str[256];
			if(m_bExtracted)
			{
				m_Height = 200;
				wsprintf(Str,"-%s",Name.c_str());
			}
			else
			{
				m_Height = 20;
				wsprintf(Str,"+%s",Name.c_str());
			}

			int End = Top + m_Height;

			CRect BaseRect;
			pBase->GetWindowRect(&BaseRect);

			int Left = 2;
			int Right = BaseRect.Width() - 4;
			if(pBase->m_bEnableScroll)
				Right -= 10;

			m_Width = Right - Left;
			
			

			DWORD K = (20<<16)|(Right-Left);
			CRect m_RectB(Left,Top,Right,Top+20);
			pButton->PostMessage(WM_SIZE,0,K);
			pButton->MoveWindow(&m_RectB);
			pButton->SetWindowText(Str);


			int Item_Top = Top + 30 + BaseRect.top;
			for(DWORD i=0;i<m_vecItem.size();i++)
			{
				Item_Top = m_vecItem[i]->MoveWindow(BaseRect.left+2,Item_Top,m_Width);
				m_vecItem[i]->ShowWindow(m_bExtracted);
			}	

			End = Item_Top - BaseRect.top;
			if(!m_bExtracted)
			{
				End = Top + 20;
			}
			CRect m_Rect(Left,Top,Right,End);
			K = ((End - Top)<<16)|(Right-Left);
			pStatic->PostMessage(WM_SIZE,0,K);
			pStatic->MoveWindow(&m_Rect);

			return End;
		}
	};
	PropertyBase::_PropertyPage* m_lpProperPage;
	vector<_Group>m_vecGroup;

public:
	UINT  m_uControlID;

	int m_Top;
	int m_TotalHeight;//所有group高度的总和
	int m_PageHeight;


	//CStatic* m_pStatic;
	// Generated message map functions
	//{{AFX_MSG(KDlg_PropertyPage)
	virtual BOOL OnInitDialog();
	afx_msg void OnMove(int x, int y);
	afx_msg void OnClose();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KDLG_PROPERTYPAGE_H__8CAA101B_DC0E_4C79_B7BB_382A23105B36__INCLUDED_)
