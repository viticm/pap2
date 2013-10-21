////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingPageLighting.h
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2007-6-27 10:43:44
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENESETTINGPAGELIGHTING_H_
#define _INCLUDE_KSCENESETTINGPAGELIGHTING_H_

#include "KColorDialog.h"
#include "KSceneCustomListContrl.h"
#include "KSceneSettingPageBase.h"

typedef struct IEKG3DEnvEffLight IEKG3DLight;
typedef struct IEKG3DEnvEffLightSet IEKG3DLightSet;
class KSceneSettingPageLighting : public KSceneSettingPageBase
{
	DECLARE_DYNAMIC(KSceneSettingPageLighting)
private:
	typedef KSH::COMMON_CTRL::GetContainerByInterfaceName<IEKG3DLightSet>	TypeContainerGetter;
	typedef KSH::COMMON_CTRL::GetElemDynamic<IEKG3DLightSet, IEKG3DLight*>	TypeElemGetter;
	typedef KSH::COMMON_CTRL::GetNameByDefault<IEKG3DLightSet, IEKG3DLight*> TypeNameGetter;
	typedef KSH::COMMON_CTRL::KSceneCustomListContrl<
		IEKG3DLightSet
		, IEKG3DLight*
		, TypeContainerGetter
		, TypeElemGetter
		, TypeNameGetter
	>	TypeLightList;
	TypeLightList m_LightList;


	CButton m_cbNeedSynAmbient;
	BOOL NeedToSynLightSetAmbientWithSunLightArg3()
	{
		return m_cbNeedSynAmbient.GetCheck() == BST_CHECKED;		
	}
	VOID NeedToSynLightSetAmbientWithSunLightArg3(BOOL bNeeded)
	{
		m_cbNeedSynAmbient.SetCheck(bNeeded ? BST_CHECKED : BST_UNCHECKED);
		m_cbNeedSynAmbient.Invalidate();
	}

public :
	KSceneSettingPageLighting();
	virtual ~KSceneSettingPageLighting();

	enum { IDD = IDD_SCENESETTING_LIGHT };
private:
    IEKG3DLight*        m_lpLight;
    IEKG3DLight*        m_pSceneLight;

	KColorPickerButton  m_ColorBnDiffuse;	// KColorPickerButton  m_ColorBnSceneDiffuse;
    KColorPickerButton  m_ColorBnAmbient;	//m_ColorBnPlayerDiffuse
	KColorPickerButton	m_ColorBnSpecular;

	KColorPickerButton  m_ColorBnColor0;	//m_ColorBnPlayerSkyLigt
	KColorPickerButton  m_ColorBnColor1;	//m_ColorBnPlayerComLigt
    KColorPickerButton  m_ColorBnColor2;	//m_ColorBnPlayerSpecure
    KColorPickerButton  m_ColorBnColor3;	//m_ColorBnPlayerAmbient

    
    KColorPickerButton  m_ColorBnSceneAmbient;//m_ColorBnSceneAmbient

    CString             m_strEvnMap;

    float               m_fHeightAngle;	//灯光高度角
    float               m_fHeadingAngle;	//灯光Heading角

	D3DXVECTOR3			m_vLightDir;

	bool				m_bIsNowLight0Active;

    void UpdateLightDirection();
public:
	static BOOL AngleToVec(float fHeadingAngle, float fHeightAngle, D3DVECTOR& vec);
	static BOOL VecToAngle(const D3DVECTOR& vec, float& fHeadingAngle, float& fHeightAngle);

public :
	virtual void DoDataExchange(CDataExchange* pDX);    
    //BOOL PreTranslateMessage(MSG* pMsg);
	
    DECLARE_MESSAGE_MAP()
	
	afx_msg BOOL OnInitDialog();
    afx_msg BOOL OnSetActive();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
    afx_msg void OnBnClickedEvnmap();

	HRESULT SynFromInnerToInterface(DWORD_PTR dwOption = 0);
	HRESULT SynFromInterfaceToInner(DWORD_PTR dwOption = 0);
	afx_msg void OnBnClickedSsListBnAdd();
	afx_msg void OnBnClickedSsListBnDel();
	afx_msg void OnLbnSelchangeLtSsElement();
	afx_msg LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};

#endif //_INCLUDE_KSCENESETTINGPAGELIGHTING_H_
