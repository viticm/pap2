#pragma once
#include "afxwin.h"
//#include "KG3DStructs.h"

// KModelEditorPlayerToolsDialog dialog
interface IEKG3DSceneModelEditor;
class IEKG3DModel;
class IEKG3DModelST;
interface IEKG3DSceneModelEditor;//by dengzhihui Refactor 2006年11月24日

class KModelEditorPlayerToolsDialog : public CDialog
{


	DECLARE_DYNAMIC(KModelEditorPlayerToolsDialog)

	IEKG3DSceneModelEditor* m_lpModelEditor;
public:
	void SetModelEditor(IEKG3DSceneModelEditor* pEditor)
	{
		m_lpModelEditor = pEditor;
	}

	vector<string>m_vecHead;
	vector<string>m_vecFace;
	vector<string>m_vecBody;
	vector<string>m_vecHand;
	vector<string>m_vecLeg;
	vector<string>m_vecBelt;
	vector<string>m_vecMDL;

	DWORD m_dwTotalPartCount;
	DWORD m_dwCurrentCount;

	/*IEKG3DModelST* m_lpModelST;
	IEKG3DModel* m_lpModelHead;
	IEKG3DModel* m_lpModelFace;
	IEKG3DModel* m_lpModelBody;
	IEKG3DModel* m_lpModelHand;
	IEKG3DModel* m_lpModelLeg;
	IEKG3DModel* m_lpModelBelt;*/

	//string m_szModelBaseDir;
	////IEKG3DModel* m_lpModelSplit;//分界面模型
	////vector<Sphere>m_vecSplitSphere;
	//struct SphereVertexIndex
	//{
	//	vector<int>vecIndex;
	//};
	//vector<SphereVertexIndex>m_vecStandVertexIndex;

	//IEKG3DModel* m_lpModelStand;//标准顶点位置和法线的模型
	vector<VFormat::FACES_NORMAL_TEXTURE1>m_vecStandVertex;

	void FillAllCombo();
	void FillCombo(CComboBox* pComBo,vector<string>* pvecName);

	KModelEditorPlayerToolsDialog(CWnd* pParent = NULL);   // standard constructor
	virtual ~KModelEditorPlayerToolsDialog();

// Dialog Data
	enum { IDD = IDD_PLAYERTOOLS };

protected:
	//HRESULT WedVertex();
	//HRESULT WedVertexForModel(IEKG3DModel* pModel,VFormat::FACES_NORMAL_TEXTURE1* pVer,D3DXMATRIX* pmat);


	//HRESULT CreateSphereVertexIndex(IEKG3DModel* pModle);
	//HRESULT CreateSphereVertexIndexFromModel(VFormat::FACES_NORMAL_TEXTURE1* pVer,int nNumVertex);

	//HRESULT CreateSpheresFromModel(IEKG3DModel* pModle);
	void ChangeModel(int nPos,CComboBox* pCombox,vector<string>*plist);

	//void SetModel(IEKG3DModel** ppModel,LPCSTR pModelName);

	//void LoadPartFromIni(LPSTR pFileName);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedLoadall();
public:
	CComboBox m_Combo_Head;
public:
	CComboBox m_Combo_Face;
public:
	CComboBox m_Combo_Body;
public:
	CComboBox m_Combo_Hand;
public:
	CComboBox m_Combo_Leg;
public:
	CComboBox m_Combo_Belt;
public:
	afx_msg void OnCbnSelchangeComboHead();
public:
	afx_msg void OnCbnSelchangeComboFace();
public:
	afx_msg void OnCbnSelchangeComboBody();
public:
	afx_msg void OnCbnSelchangeComboHand();
public:
	afx_msg void OnCbnSelchangeComboLeg1();
public:
	afx_msg void OnCbnSelchangeComboBelt();
public:
	afx_msg void OnBnClickedButtonSep();
public:
	afx_msg void OnBnClickedButtonStand();
public:
	afx_msg void OnBnClickedButtonWed();
public:
	afx_msg void OnBnClickedLoadmdl();
public:
	afx_msg void OnBnClickedLoadani();
	afx_msg void OnBnClickedLoadall2();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedButtonH();
	afx_msg void OnBnClickedButtonM();
	afx_msg void OnBnClickedButtonL();
};
