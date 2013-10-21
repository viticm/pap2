#include "stdafx.h"
#include "SceneEditor.h"
#include "KGCameraSettingDlg.h"
#include "IEKG3DScene.h"
#include "IEKG3DSceneOutputWnd.h"
//#include "KG3DGraphicsTool.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNAMIC(KGCameraSettingDlg, CPropertyPage)

KGCameraSettingDlg::KGCameraSettingDlg()
	: CPropertyPage(KGCameraSettingDlg::IDD)
	, m_pCameraStation(0)
    , m_nChaseRate(0)
{
	m_pCameraStation = NULL;
}

KGCameraSettingDlg::~KGCameraSettingDlg()
{
}

void KGCameraSettingDlg::DoDataExchange(CDataExchange* pDX)
{
    CPropertyPage::DoDataExchange(pDX);
    DDX_Control(pDX, IDC_CHECK0, m_comboNoUse);
    DDX_Control(pDX, IDC_CHECK1, m_comboAways);
    DDX_Control(pDX, IDC_CHECK2, m_comboAptit);
    DDX_Control(pDX, IDC_CHECK4, m_combObstrt);
    DDX_Slider(pDX, IDC_CHASE_RATE, m_nChaseRate);
    DDX_Slider(pDX, IDC_SK,     m_nflexCoefficient);
    DDX_Slider(pDX, IDC_DK,     m_nDampCoefficient);
    DDX_Slider(pDX, IDC_MIN_AN, m_nMinAngelVel);
    DDX_Slider(pDX, IDC_MAX_AN, m_nMaxAngelVel);
    DDX_Slider(pDX, IDC_HOR,    m_nAngelRateHor);
    DDX_Slider(pDX, IDC_VEL,    m_nAngelRateVel);
    DDX_Slider(pDX, IDC_MAX_LEN, m_nMaxDistance);
    DDX_Slider(pDX, IDC_WEEL,   m_nMouseWeel);

    /*DDX_Text(pDX, IDC_EDIT_1, m_nMaxDistance);
    DDX_Text(pDX, IDC_EDIT_2, m_nMinDistance);
    DDX_Text(pDX, IDC_EDIT_3, m_nAcceleration);
    DDX_Text(pDX, IDC_EDIT_4, m_nBeginSpeed);*/
}

BEGIN_MESSAGE_MAP(KGCameraSettingDlg, CPropertyPage)
    ON_BN_CLICKED(IDC_CHECK0, &KGCameraSettingDlg::OnBnClickedCheck0)
	ON_BN_CLICKED(IDC_CHECK2, &KGCameraSettingDlg::OnBnClickedCheck2)
	ON_BN_CLICKED(IDC_CHECK1, &KGCameraSettingDlg::OnBnClickedCheck1)
    ON_WM_HSCROLL()
END_MESSAGE_MAP()

//(by dengzhihui Refactor 2006年11月6日
class CStationHeler
{
	IEKG3DCameraStation* m_pInteface;
public:
	void Init(IEKG3DCameraStation* pReal)
	{
		_ASSERTE(pReal);
		m_pInteface = pReal;
	}

	void IniByConfigFile(LPCTSTR pcszFileName)
	{
		HRESULT hr = m_pInteface->IniByConfigFile(pcszFileName);
	}

	int GetChaseType()
	{
		int ChaseType;
		m_pInteface->GetChaseType(&ChaseType);
		return ChaseType;
	}

	float GetChaseRate()
	{
		float ChaseRate;
		m_pInteface->GetChaseRate(&ChaseRate);
		return ChaseRate;
	}

	BOOL GetObstructdAvert()
	{
		int nObstructdAvert;
		m_pInteface->GetObstructdAvert(&nObstructdAvert);
		return !!nObstructdAvert;
	}

	float GetFlexCoefficient()
	{
		float FlexCoefficientl;
		m_pInteface->GetFlexCoefficient(&FlexCoefficientl);
		return FlexCoefficientl;
	}

	float GetDampCoefficient()
	{
		float DampCoefficient;
		m_pInteface->GetDampCoefficient(&DampCoefficient);
		return DampCoefficient;
	}

	float GetMaxAngelVel()
	{
		float MaxAngelVel;
		m_pInteface->GetMaxAngelVel(&MaxAngelVel);
		return MaxAngelVel;
	}

	float GetMinAngelVel()
	{
		float MinAngelVel;
		m_pInteface->GetMinAngelVel(&MinAngelVel);
		return MinAngelVel;
	}

	float GetAngelRateVel()
	{
		float AngelRateVel;
		m_pInteface->GetAngelRateVel(&AngelRateVel);
		return AngelRateVel;
	}

	float GetAngelRateHor()
	{
		float AngelRateHor;
		m_pInteface->GetAngelRateHor(&AngelRateHor);
		return AngelRateHor;
	}

	float GetMaxDistance()
	{
		float MaxDistance;
		m_pInteface->GetMaxDistance(&MaxDistance);
		return MaxDistance;
	}
    
    float GetDisZoomRate()
    {
        float fDisZoomRate = 0.0f;
        m_pInteface->GetDisZoomRate(&fDisZoomRate);
        return fDisZoomRate;
    }

};
//)

BOOL KGCameraSettingDlg::OnInitDialog()
{
	int nResult  = false;
	int nRetCode = false;
    HRESULT hRetCode = E_FAIL;
    IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;

	IEKG3DScene*	   lpCurScene   = NULL;

	nRetCode = CPropertyPage::OnInitDialog();
	KG_PROCESS_ERROR(nRetCode);

	lpCurScene   = g_pGraphicsTool->GetScene();
	KG_PROCESS_ERROR(lpCurScene);

    hRetCode = lpCurScene->GetCurOutputWnd(&piSceneCurOutputWnd);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
	KG_PROCESS_ERROR(piSceneCurOutputWnd);

	//(by dengzhihui Refactor 2006年11月6日
	hRetCode = piSceneCurOutputWnd->GetCameraStation(&m_pCameraStation);
	KG_COM_PROCESS_ERROR(hRetCode);
  
	KG_PROCESS_ERROR(m_pCameraStation);

	CStationHeler Station; 
	Station.Init(m_pCameraStation); 

	if (Station.GetChaseType() == 1)    
		m_comboAways.SetCheck(TRUE);
	else if (Station.GetChaseType() == 2)
		m_comboAptit.SetCheck(TRUE);
    else
        m_comboNoUse.SetCheck(TRUE);

    if (Station.GetObstructdAvert())
        m_combObstrt.SetCheck(TRUE);

    m_nChaseRate = static_cast<int>(Station.GetChaseRate() / 15.0f * 100);
	m_nflexCoefficient = static_cast<int>(Station.GetFlexCoefficient() * 10);
	m_nDampCoefficient = static_cast<int>(Station.GetDampCoefficient() * 10);

    m_nMaxAngelVel     = static_cast<int>(Station.GetMaxAngelVel() * 200 / D3DX_PI);
    m_nMinAngelVel     = static_cast<int>(Station.GetMinAngelVel() * 200 / D3DX_PI + 100);

    m_nAngelRateHor    = static_cast<int>(Station.GetAngelRateHor() * 25000 / D3DX_PI);
    m_nAngelRateVel    = static_cast<int>(Station.GetAngelRateVel() * 25000 / D3DX_PI);

    m_nMaxDistance     = static_cast<int>(Station.GetMaxDistance() / 50);
    m_nMouseWeel       = static_cast<int>(Station.GetDisZoomRate() / 10);
	//)

    float K = m_nflexCoefficient / 10.0F;
    float D = sqrtf(K) * 2;
    if (D == m_nDampCoefficient / 10)
        GetDlgItem(IDC_EDIT)->SetWindowText(_T("系统性质：临界阻尼"));
    else if (D < m_nDampCoefficient / 10)
        GetDlgItem(IDC_EDIT)->SetWindowText(_T("系统性质：强阻尼"));
    else
        GetDlgItem(IDC_EDIT)->SetWindowText(_T("系统性质：弱阻尼"));

	UpdateData(FALSE);

	nResult = true;
Exit0:
	return nResult;
}

void KGCameraSettingDlg::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    UpdateData();

    if (
        (static_cast<CWnd*>(pScrollBar) == GetDlgItem(IDC_SK)) ||
        (static_cast<CWnd*>(pScrollBar) == GetDlgItem(IDC_DK))
    )
    {
        float K = m_nflexCoefficient / 10.0F;
        float D = sqrtf(K) * 2;
        if (D == m_nDampCoefficient / 10)
            GetDlgItem(IDC_EDIT)->SetWindowText(_T("系统性质：临界阻尼"));
        else if (D < m_nDampCoefficient / 10)
            GetDlgItem(IDC_EDIT)->SetWindowText(_T("系统性质：强阻尼"));
        else
            GetDlgItem(IDC_EDIT)->SetWindowText(_T("系统性质：弱阻尼"));

    }
    return CPropertyPage::OnHScroll(nSBCode, nPos, pScrollBar);
}

int KGCameraSettingDlg::Update()
{
	int nResult = false;
	KG_PROCESS_ERROR(m_pCameraStation);

	UpdateData();

	//(by dengzhihui Refactor 2006年11月6日
	if (m_comboAways.GetCheck())
		m_pCameraStation->SetChaseType(1);
	else if (m_comboAptit.GetCheck())
		m_pCameraStation->SetChaseType(2);
    else
        m_pCameraStation->SetChaseType(0);

    if (m_combObstrt.GetCheck())
        m_pCameraStation->SetObstructdAvert(true);
    else
        m_pCameraStation->SetObstructdAvert(false);

    m_pCameraStation->SetChaseRate(static_cast<float>(m_nChaseRate) / 100 * 15.0f);
    m_pCameraStation->SetFlexCoefficient(static_cast<float>(m_nflexCoefficient) / 10);
    m_pCameraStation->SetDampCoefficient(static_cast<float>(m_nDampCoefficient) / 10);

    m_pCameraStation->SetMaxAngelVel(static_cast<float>(m_nMaxAngelVel) * D3DX_PI / 200);
    m_pCameraStation->SetMinAngelVel((static_cast<float>(m_nMinAngelVel) - 100) * D3DX_PI / 200);

    m_pCameraStation->SetAngelRateHor(static_cast<float>(m_nAngelRateHor) * D3DX_PI / 25000);
    m_pCameraStation->SetAngelRateVel(static_cast<float>(m_nAngelRateVel) * D3DX_PI / 25000);

    m_pCameraStation->SetMaxDistance(static_cast<float>(m_nMaxDistance * 50));
    m_pCameraStation->SetDisZoomRate(static_cast<float>(m_nMouseWeel * 10));
	/*m_pCameraStation->fFlexCoefficient  = static_cast<float>(m_nAcceleration);
	m_pCameraStation->fMaxDistance   = static_cast<float>(m_nMaxDistance);
	m_pCameraStation->fMinDistance   = static_cast<float>(m_nMinDistance);*/

    m_pCameraStation->SaveToConfigFile(EDITOR_CONFIG_FILE);
	//)
	nResult = true;
Exit0:
	return nResult;
}

void KGCameraSettingDlg::OnBnClickedCheck0()
{
    if (m_comboNoUse.GetCheck())
    {
        m_comboAways.SetCheck(FALSE);
        m_comboAptit.SetCheck(FALSE);
    }
}

void KGCameraSettingDlg::OnBnClickedCheck2()
{
    if (m_comboAptit.GetCheck())
    {
        m_comboAways.SetCheck(FALSE);
        m_comboNoUse.SetCheck(FALSE);
    }
}

void KGCameraSettingDlg::OnBnClickedCheck1()
{
    if (m_comboAways.GetCheck())
    {
        m_comboAptit.SetCheck(FALSE);
        m_comboNoUse.SetCheck(FALSE);
    }
}
