#pragma once
#include "KG3DInterface.h"
#include "afxwin.h"
#include "ieeditor.h"
// KDlgPlayerCheckTool dialog

class KDlgPlayerCheckTool : public CDialog
{
	//enum enuPlayPart
	//{
	//	PART_HEAD,
	//	PART_BODY,
	//	PART_LEG,//ignore
	//	PART_HAND,//ignore
	//	PART_BELT,//ignore
	//	PART_FACE,
	//	PART_COUNT,
	//};

	enum enuModelType
	{
		Model_Mesh,
		Model_MDL,
		Model_Ani,
		Model_Mtl,
	};

	struct AutoBind
	{
		std::string strDest;
		DWORD dwPartIndex;
		IKG3DModel *pCurrent;
	};

	DECLARE_DYNAMIC(KDlgPlayerCheckTool)

public:
    /*enum enuPatternModelID
    {
        PM_F_NORMAL,
        PM_F_SMALL,
        PM_F_TALL,
        PM_M_NORMAL,
        PM_M_SMALL,
        PM_M_TALL,
        PM_M_COUNT,
        PM_M_FORCE_DWORD = 0xffffffff,
    };*/
	const static DWORD INVALIDATEPATTERN = 0xffffffff;
	
    KDlgPlayerCheckTool(CWnd* pParent = NULL);   // standard constructor
	virtual ~KDlgPlayerCheckTool();
    void SetScene(IEKG3DScene* pScene)
    {
        IEKG3DModel* pModel = NULL;
        IEKG3DSceneModelEditor* pModelEditor = NULL;
        m_pScene = pScene;
        KG_PROCESS_ERROR(pScene);

        pScene->GetSceneModelEditor(&pModelEditor);
        KG_PROCESS_ERROR(pModelEditor);

        pModelEditor->GetCurModel(&pModel);
        KG_PROCESS_ERROR(pModel);

        pScene->RemoveRenderEntity(SCENEENTITY_MODEL, (void*)pModel);
Exit0:
    ;
    }

// Dialog Data
	enum { IDD = IDD_DIALOG_PLAYERCHECK ,
	NUM_SOCKET = 9,
	MODELTYPE_COUNT = 4};

protected:
	void SetCurModelLod(float fLod);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
private:
	BOOL ApplyAutoBind(int nPartIndex, 
		IKG3DModel *pNewModel);
    void SetModel(DWORD ID);
    IEKG3DScene* m_pScene;
	void UpdatePath();
	IKG3DModel* m_pModel;
    IKG3DModel* m_pStillModel;
	std::vector<IKG3DModel*> m_ItemModels;
	TCHAR m_strModelPath[MAX_PATH];
	TCHAR m_strAnimationPath[MAX_PATH];
	TCHAR m_strStartPath[MAX_PATH];
	std::vector<std::vector<std::string>> m_PartPath;
	std::vector<std::vector<std::string>> m_MtlPath;
	std::vector<std::string> m_MotionPath;
	std::vector<std::string> m_ItemPath;
	static std::vector<std::string> m_strSuffix;
	static std::vector<std::string> m_strSockets;
	static std::vector<std::string> m_PatternPath;
	static std::vector<std::string> m_StandardPath;
	std::vector<int> m_CurrentSelection;
	static std::vector<AutoBind> m_AutoBind;
	static int m_nHeadIndex;
	static int m_nBodyIndex;
	static int m_nFaceIndex;
	static int m_nHatIndex;
	static int m_nHandIndex;
	//static TCHAR* m_strSuffix[PART_COUNT];
	//static TCHAR* m_strSockets[NUM_SOCKET];
	//static TCHAR* s_PatternPath[PM_M_COUNT];
	static TCHAR* m_ModelType[MODELTYPE_COUNT];
	const static TCHAR* m_strConfigFilePath;
	//const static TCHAR* m_strConfigSection;
	const static TCHAR* m_strDirectorySection;
	const static TCHAR* m_strPlayerTypeSection;
	const static TCHAR* m_strPartSection;
	const static TCHAR* m_strSocketSection;
	const static TCHAR* m_strAutoBindSection;

	CListBox m_listPart;
	CListBox m_listItem;
	CComboBox m_comboMotion;
	afx_msg void OnLbnSelchangeListPart();
	afx_msg void OnLbnSelchangeListItem();
	afx_msg void OnBnClickedButtonSaveAs();
private:
	CComboBox m_comboPart;
private:
	CComboBox m_comboSocket;
	afx_msg void OnCbnSelchangeComboPart();
	afx_msg void OnCbnSelchangeComboSet();
	void RefreshPath();
	virtual void OnOK();
	virtual void OnCancel();
	void SaveConfig();
	void LoadConfig();
	virtual BOOL OnInitDialog();

private:
	CComboBox m_comboSet;
	CString m_strSetPath;
	CString m_strItemPath;
	afx_msg void OnBnClickedButtonRefresh();
	void FindModelInDirectory(LPCSTR strStartDir, 
		BOOL bIncludeSubDir,
		enuModelType Type,
		const TCHAR* strKeyWord,
		std::vector<std::string>& vecStore);
	afx_msg void OnCbnSelchangeComboMotion();
	void OnRefreshMotion();
	void OnRefreshItem();
	float m_fFrameLength;
	BOOL m_bFrameLengthChanged;
	afx_msg void OnBnClickedButtonClear();
    std::vector<IKG3DModel*> m_PatternModels;
    std::vector<IKG3DModel*> m_vecStillModel;
    HRESULT LoadPreset();
    void ClearPatternModel()
    {
        for (size_t i = 0; i < m_PatternModels.size(); i++)
        {
            SAFE_RELEASE(m_PatternModels[i]);
            SAFE_RELEASE(m_vecStillModel[i]);
        }
        m_PatternModels.clear();
    }
private:
	void PostNcDestroy();
	CComboBox m_comboMtl;
	//afx_msg void OnCbnSelchangeCombo1();
	CListBox m_listMtl;
public:
	static std::vector<std::string> m_PatternPathName;
	//static TCHAR* s_PatternPathName[PM_M_COUNT];
	afx_msg void OnLbnSelchangeListMtl();
//Normal¥¶¿Ì
private:
	afx_msg void OnBnClickedRadioWireframe();
	afx_msg void OnBnClickedRadioSolid();
	afx_msg void OnBnClickedVertexJointing();
	struct KSphere
	{
		D3DXVECTOR3 vPos;
		D3DXVECTOR3 vNormal;
		BOOL IsPositionIn(D3DXVECTOR3& vP);
	};
	//enuPatternModelID m_enCurModelID;
	DWORD m_dwCurModelID;
	vector<KSphere> m_vecSphereFace;
	vector<KSphere> m_vecSphereHead;
	vector<KSphere> m_vecSphereNeck;
	vector<KSphere> m_vecSphereHand;
	void UpdateStandardData();
	HRESULT EnableWireframeMode(BOOL bEnable);
	HRESULT LoadStandardDataFromIni(const TCHAR* strFileName);
	HRESULT ChangeJointNormal(IEKG3DModel* pModel,vector<KSphere> &vecSphere,BOOL bChangePos, int nIndex);
	void OnChangeModelJointNormal();
public:
	afx_msg void OnBnClickedCheck1();
	BOOL m_bChangePos;
	afx_msg void OnBnClickedButtonH();
	afx_msg void OnBnClickedButtonM();
	afx_msg void OnBnClickedButtonL();

private:
    int m_nHorsePlayerTypeIndex;
    CString m_strHorsePath;
public:
	afx_msg void OnBnClickedButtonClearpart();
	afx_msg void OnBnClickedButtonSaveas2();
};
