#if !defined(AFX_KSCENEMODELEDITORPROPERTYPAGEMATERIAL_H__D08F0284_A9DF_4E02_9034_93AFEE8D0733__INCLUDED_)
#define AFX_KSCENEMODELEDITORPROPERTYPAGEMATERIAL_H__D08F0284_A9DF_4E02_9034_93AFEE8D0733__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// KSceneModelEditorPropertyPageMaterial.h : header file
//
#include <list>
#include <vector>
#include "KColorButton.h"
//#include "kg3dmaterialex.h"
#include "IEEditor.h"//by dengzhihui Refactor 2006年11月7日
#include "KModelEditorMaterialAlphaOperatorDialog.h"
#include "afxwin.h"
#include "KColorDialog.h"
#include "KModelEditorMaterialDetailDialog.h"
enum enuTimer
{
	ENUM_TIMER_AUTOCHANGE = 1,
};



enum enuDirtyMask
{
	ENUM_DIRTYMASK_DIFFUSE = 1,
	ENUM_DIRTYMASK_AMBIENT = (1<<1),
	ENUM_DIRTYMASK_SPECULAR = (1<<2),
    ENUM_DIRTYMASK_TEXTURE = (1<<3),
	ENUM_DIRTYMASK_EN_LIGHT = (1<<4),
	ENUM_DIRTYMASK_EN_SPECULAR = (1<<5),
	ENUM_DIRTYMASK_EN_CULLNONE = (1<<6),
	ENUM_DIRTYMASK_EN_HIDE = (1<<7),
	ENUM_DIRTYMASK_SHINNESS = (1<<8),
	ENUM_DIRTYMASK_EMISSIVE = (1<<9),
	//ENUM_DIRTYMASK_ALPHA = (1<<10),
	ENUM_DIRTYMASK_COUNT = 10,
	ENUM_DIRTYMASK_FORCE_DWORD = 0xffffffff,
};

/////////////////////////////////////////////////////////////////////////////
// KSceneModelEditorPropertyPageMaterial dialog



static const TCHAR* strZFunc[] = { 
	"D3DCMP_NEVER",
	"D3DCMP_LESS",
	"D3DCMP_EQUAL",
	"D3DCMP_LESSEQUAL",
	"D3DCMP_GREATER",
	"D3DCMP_NOTEQUAL",
	"D3DCMP_GREATEREQUAL",
	"D3DCMP_ALWAYS"
};

class KSceneModelEditorPropertyPageMaterial : public CPropertyPage
{
	DECLARE_DYNCREATE(KSceneModelEditorPropertyPageMaterial)

	enum 
	{
		FX_NONE = 0,
		FX_AMBIENT,
		FX_NORMALMAP_NOSKIN,
	};
// Construction
public:
	enum enuButtonMaps
	{
		ENUM_BUTTON_AMBIENT,
		ENUM_BUTTON_DIFFUSE,
		ENUM_BUTTON_SPECULAR,
		ENUM_BUTTON_EMISSIVE,

		ENUM_BUTTON_COUNT,
		ENUM_BUTTON_FORCE_DWORD = 0xffffffff,
	};

	
	struct ButtonColor
	{
		CBitmap Bitmap;
		COLORREF*  pBuffer;
		size_t BufferSize;
		ButtonColor()
		{
			pBuffer = NULL;
			BufferSize = 0;
		}
		
		~ButtonColor()
		{
			SAFE_DELETE_ARRAY(pBuffer);
			BufferSize = 0;
		}
	};

	COLORREF ValueToColor(D3DCOLORVALUE Value)
	{
		return (0xff000000 | RGB(Value.r * 255, Value.g * 255, Value.b * 255));
	}

	void SetBuffer(COLORREF* pBuffer, COLORREF crColor, size_t Size)
	{
		KG_PROCESS_ERROR(pBuffer);
		KG_PROCESS_ERROR(Size);
		
		crColor = 0xff000000 | RGB((crColor & 0x00ff0000) >> 16, 
			          (crColor & 0x0000ff00) >> 8, 
					  crColor & 0x000000ff);

		for(size_t i = 0; i < Size; i++)
		{
			pBuffer[i] = crColor;
		}

Exit0:
		return;
	}

	void SetCurModel(IEKG3DModel* pCurModel);//by dengzhihui Refactor 2006年11月7日
	KSceneModelEditorPropertyPageMaterial();
	~KSceneModelEditorPropertyPageMaterial();

	KModelEditorMaterialDetailDialog* m_lpDetailDialog;
// Dialog Data
	//{{AFX_DATA(KSceneModelEditorPropertyPageMaterial)
	enum { IDD = IDD_MODELEDITOR_MATERIAL };
	CButton	m_ButtonHide;
	CButton	m_ButtonBlend;
	CButton	m_ButtonZWrite;
	CButton	m_Static_Texture; 
	CButton	m_ButtonTextureTranslation;
	CButton	m_ButtonTextureRemove;
	CButton	m_ButtonTextureOperation;
	CButton	m_ButtonTextureMap;
	CButton	m_ButtonTextureApplySet;
	CButton	m_ButtonTextureAdd;
	CEdit	m_EditDiffuseAlpha;
	CEdit	m_EditPower;
	CListBox	m_ctlTextureList;
	CButton	m_ColorButtonDiffuse;
	CButton	m_ColorButtonSpecular;
	CButton	m_ColorButtonEmissive;
	CButton	m_ColorButtonAmbient;
	CButton	m_ButtonZbuffer;
	CButton	m_ButtonSpecular;
	CButton	m_ButtonLighting;
	CButton	m_Button2Face;
	CButton	m_Static_Subset;
	CListBox m_ctlSubsetList;
	BOOL	m_bShowSelectOnly;
	float	m_fAlpha;
	float	m_fPower;
    float   m_fSpecPower;
    float   m_fEmssPower;
    CEdit	m_EditSpecPower;
    CEdit	m_EditEmssPower;
	BOOL    m_bAutoModify;
	DWORD m_dwAlphaTop;
	DWORD m_dwAlphaBottom;

    KColorPickerButton m_ColorCastBn;
	KColorPickerButton m_ColorCastBn1;
	KColorPickerButton m_ColorCastBn2;
	KColorPickerButton m_ColorCastBn3;
	KColorPickerButton m_ColorCastBn4;
	KColorPickerButton m_ColorCastBn5;
	KColorPickerButton m_ColorCastBn6;
	KColorPickerButton m_ColorCastBn7;

	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(KSceneModelEditorPropertyPageMaterial)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	KSceneModelEditorMaterialAlphaOperatorDialog* m_lpMaterialAlphaDialog;

	void FillListBoxSubset();
	void FillListBoxTexture(int i);
	IEKG3DModel* m_lpCurModel;
	// Generated message map functions
	//{{AFX_MSG(KSceneModelEditorPropertyPageMaterial)
	virtual BOOL OnInitDialog();
	afx_msg void OnButtonButton();
	afx_msg void OnButtonDelete();
	afx_msg void OnButtonDown();
	afx_msg void OnButtonTop();
	afx_msg void OnButtonUnit();
	afx_msg void OnButtonUp();
	afx_msg void OnCheckShowselectonly();
	afx_msg void OnSelchangeListSubset();
	afx_msg void OnButtonSelectall();
	afx_msg void OnButtonSelectinvert();
	afx_msg void OnButtonSelectnone();
	afx_msg void OnCheckZbuffer();
	afx_msg void OnCheckSpecular();
	afx_msg void OnCheckLighting();
	afx_msg void OnCheck2face();
	afx_msg void OnButtonTextureadd();
	afx_msg void OnButtonTextureremove();
	afx_msg void OnDblclkListTextures();
	afx_msg void OnSelchangeListTextures();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonLoad();
	afx_msg void OnBnClickedButtonSave();
	afx_msg void OnBnClickedButtonSaveid();
	afx_msg void OnBnClickedCheckZwrite();
	afx_msg void OnBnClickedCheckHide();
	afx_msg void OnBnClickedButtonAmbient();
	afx_msg void OnBnClickedButtonDiffuse();
	afx_msg void OnBnClickedButtonSpecular();
	afx_msg void OnBnClickedButtonEmissive();
	BOOL PreTranslateMessage(MSG* pMsg);
	//HRESULT AutoFindMapsBySemantics(KG3DMaterial* pMaterial, int nIndex, const TCHAR* strSemantic, TCHAR* strRetFileName);
private:
	IEKG3DMesh* GetReleasedMesh();

	void OnEditKeyReturn(HWND hWnd);
	//HRESULT GetCurSubMaterial(KG3DMaterial::KG3DMaterialSubset** pSubMat, int* pIndex = NULL);
	IEKG3DMaterial* GetMaterialFromModel();

	//这个vector只被AutoFindMapsBySemantics使用，根据去掉AutoFindMapsBySemantics的理由，去掉这个Vector
	//vector<Semantic2MapFinder> m_vecSemantic2MapFinder;
	int m_nBaseMaterial;
	DWORD m_dwDirtyTextureMask;
public:
	afx_msg void OnBnClickedButtonTexturemap();
	afx_msg void OnBnClickedButtonTextureop();
public:
//	afx_msg void OnBnClickedCheckAlpha();
//	afx_msg void OnBnClickedCheckAlphablend();
	void UpdateSubsetUI();
	LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
private:
	ButtonColor m_ButtonMaps[ENUM_BUTTON_COUNT];
	void InitButtonMaps(CButton* pButton, DWORD dwID, COLORREF crColor);
	void UpdateButtonState(DWORD dwButtonID, COLORREF crColor);
	afx_msg void OnCbnSelchangeComboZfun();
	afx_msg void OnCbnSelchangeComboBlendmode();
	afx_msg void OnBnClickedButtonBlendadv();
private:
	CComboBox m_ctlBlendMode;
private:
	CButton m_ctlAutoChange;
	void ResetAlphaWindow(BOOL bShow);
private:
	TCHAR m_strFullPath[8][MAX_PATH];
public:
	afx_msg void OnBnClickedCheckNocell();
	CButton m_ButtonNoCell;
    CButton m_CheckSpec;
    CButton m_CheckEMap;
    afx_msg void OnBnClickedCheckSpec();
    afx_msg void OnBnClickedCheckEmap();
    afx_msg void OnBnClickedFlushTexs();
	afx_msg void OnEnChangeEditTwicealphatop();
	afx_msg void OnBnClickedButtonSaveas();
	afx_msg void OnBnClickedCheckSortassfx();
	BOOL m_bSortAsSFX;
	afx_msg void OnBnClickedButtonDetail();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_KSCENEMODELEDITORPROPERTYPAGEMATERIAL_H__D08F0284_A9DF_4E02_9034_93AFEE8D0733__INCLUDED_)
