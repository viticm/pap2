////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneSettingPageLighting.cpp
//  Version     : 1.0
//  Creator     : Chiyer (Zhao XinYu)
//  Create Date : 2007-6-27 10:44:44
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////


#include "stdafx.h"
#include "KSceneSettingPageLighting.h"
#include "IEKG3DScene.h"
//#include "IEOther.h"
#include "IEEditor.h"
#include "IEKG3DEnvEffLight.h"
#include "IEKG3DEnvEffLightSet.h"


#define MT_FLOAT_EQUAL_L(F1, F2) ((F1) >= (F2) - FLT_EPSILON && (F1) <= (F2) + FLT_EPSILON)
#define MT_DOUBLE_EQUAL_L(F1, F2) ((F1) >= (F2) - (double)FLT_EPSILON && (F1) <= (F2) + (double)FLT_EPSILON)



extern IEKG3DGraphicsTool* g_pGraphicsTool;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(KSceneSettingPageLighting, CPropertyPage)
KSceneSettingPageLighting::KSceneSettingPageLighting()
	: KSceneSettingPageBase(KSceneSettingPageLighting::IDD)
    , m_pSceneLight(NULL)
    , m_strEvnMap("")
    , m_fHeightAngle(0.f)
	, m_vLightDir(0,-1,0)
    , m_fHeadingAngle(0.f)
	, m_LightList(TypeContainerGetter()
		, TypeElemGetter(&IEKG3DLightSet::Size, &IEKG3DLightSet::Get)
		, TypeNameGetter(_T("Light_"))
		)
	, m_bIsNowLight0Active(false)
{

}

KSceneSettingPageLighting::~KSceneSettingPageLighting()
{
}

void KSceneSettingPageLighting::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LT_SS_ELEMENT, m_LightList);
	DDX_Control(pDX, IDC_BUTTON_DIFF,   m_ColorBnDiffuse);
    DDX_Control(pDX, IDC_BUTTON_AMB,    m_ColorBnAmbient);
	DDX_Control(pDX, IDC_BUTTON_SPEC,	m_ColorBnSpecular);
    
    DDX_Control(pDX, IDC_BUTTON_SKY,     m_ColorBnColor0);
    DDX_Control(pDX, IDC_BUTTON_COM,     m_ColorBnColor1);
	DDX_Control(pDX, IDC_BUTTON1,    m_ColorBnColor2);
    DDX_Control(pDX, IDC_BUTTON_DIFF2,     m_ColorBnColor3);
   
    DDX_Control(pDX, IDC_BUTTON_AMB2,    m_ColorBnSceneAmbient);
    DDX_Text(pDX,    IDC_EDIT_EVNMAP,    m_strEvnMap);
    DDX_Text(pDX,    IDC_EDIT_HEIGHT,    m_fHeightAngle);
    DDX_Text(pDX,    IDC_EDIT_ANGEL,     m_fHeadingAngle);
	
	if (pDX->m_bSaveAndValidate)
	{
		CString csTemp;
		DDX_Text(pDX, IDC_EDIT_DIR, csTemp);

		D3DXVECTOR3 vTemp;
		if(KGCH::TStringToValue(csTemp, vTemp))
		{
			m_vLightDir = vTemp;
		}
	}
	else
	{
		CString csTemp;
		if (KGCH::TValueToString(m_vLightDir, csTemp))
		{
			DDX_Text(pDX, IDC_EDIT_DIR, csTemp);
		}
	}


	DDX_Control(pDX, IDC_CHECK1, m_cbNeedSynAmbient);

}


BEGIN_MESSAGE_MAP(KSceneSettingPageLighting, CPropertyPage)
	ON_WM_SHOWWINDOW()
    ON_BN_CLICKED(IDC_EVNMAP, &KSceneSettingPageLighting::OnBnClickedEvnmap)
	ON_BN_CLICKED(IDC_SS_LIST_BN_ADD, &KSceneSettingPageLighting::OnBnClickedSsListBnAdd)
	ON_BN_CLICKED(IDC_SS_LIST_BN_DEL, &KSceneSettingPageLighting::OnBnClickedSsListBnDel)
	//ON_BN_CLICKED(IDC_SS_BN_APPLY, &KSceneSettingPageLighting::OnBnClickedSsBnUpdate)
	//ON_BN_CLICKED(IDC_SS_BN_CANCEL, &KSceneSettingPageLighting::OnBnClickedSsBnCancel)
	ON_LBN_SELCHANGE(IDC_LT_SS_ELEMENT, &KSceneSettingPageLighting::OnLbnSelchangeLtSsElement)
END_MESSAGE_MAP()

/*
void KSceneSettingPageLighting::UpdateLightDirection()
{
    D3DLIGHT9* pD3DLight = NULL;
    D3DLIGHT9* pMdlLight = NULL;

    if (m_pSceneLight)
        m_pSceneLight->GetD3DLight(&pD3DLight);

    if (pD3DLight)
    {
        pD3DLight->Direction.x = -cos(m_fAngle * D3DX_PI / 180.0f) * cos(m_fHeight * D3DX_PI / 180.0f);
        pD3DLight->Direction.y = -sin(m_fHeight * D3DX_PI / 180.0f);
        pD3DLight->Direction.z = -sin(m_fAngle * D3DX_PI / 180.0f) * cos(m_fHeight * D3DX_PI / 180.0f);

        pD3DLight->Type = D3DLIGHT_DIRECTIONAL;

        pD3DLight->Position.x = 0.0f;
        pD3DLight->Position.y = 400.0f;
        pD3DLight->Position.z = 0.0f;
    }

    if (m_lpLight)
        m_lpLight->GetD3DLight(&pMdlLight);

    if (pMdlLight)
    {
        pMdlLight->Direction.x = -cos(m_fAngle * D3DX_PI / 180.0f) * cos(m_fHeight * D3DX_PI / 180.0f);
        pMdlLight->Direction.y = -sin(m_fHeight * D3DX_PI / 180.0f);
        pMdlLight->Direction.z = -sin(m_fAngle * D3DX_PI / 180.0f) * cos(m_fHeight * D3DX_PI / 180.0f);

        pMdlLight->Type = D3DLIGHT_DIRECTIONAL;

        pMdlLight->Position.x = 0.0f;
        pMdlLight->Position.y = 400.0f;
        pMdlLight->Position.z = 0.0f;
    }

//Exit0:
    return;
}*/


/*
BOOL KSceneSettingPageLighting::PreTranslateMessage(MSG* pMsg)
{
    if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
    {
        if (pMsg->hwnd == GetDlgItem(IDC_EDIT_ANGLE)->GetSafeHwnd() ||
            pMsg->hwnd == GetDlgItem(IDC_EDIT_HEIGHT)->GetSafeHwnd()
           )
        {
            UpdateData();

            m_fAngle  = max(0,   m_fAngle);
            m_fAngle  = min(360, m_fAngle);
            m_fHeight = max(0,   m_fHeight);
            m_fHeight = min(180, m_fHeight);

            UpdateData(FALSE);

            UpdateLightDirection();

            return true;
        }
    }

    return CPropertyPage::PreTranslateMessage(pMsg);
}*/



BOOL KSceneSettingPageLighting::OnSetActive()
{
	//KG_PROCESS_ERROR(TRUE);

	

	m_LightList.Reload();
	//this->SynFromInnerToInterface();
	return CPropertyPage::OnSetActive();
/*
Exit0:
	MessageBox(_T("不能得到正确灯光，错误"));
	return CPropertyPage::OnSetActive();*/

}

void KSceneSettingPageLighting::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CPropertyPage::OnShowWindow(bShow, nStatus);
    
}


void KSceneSettingPageLighting::OnBnClickedEvnmap()
{
    CFileDialog dlg(true);
    if (dlg.DoModal() == IDOK)
    {
        m_strEvnMap = dlg.GetPathName();

        TCHAR szName[MAX_PATH];

        if (g_GetFilePathFromFullPath(szName, m_strEvnMap.GetBuffer()))
        {
            IEKG3DScene* pScene = g_pGraphicsTool->GetScene();

            if (pScene)
            {
                if (SUCCEEDED(pScene->SetEnvMap(szName)))
                    UpdateData(false);
            }
        }
    }
}

HRESULT KSceneSettingPageLighting::SynFromInnerToInterface( DWORD_PTR dwOption /*= 0*/ )
{
	IEKG3DEnvEffLightSet* pLightSet = KSH::GetEngineInterfaceEx<IEKG3DEnvEffLightSet>();
	KG_PROCESS_ERROR(pLightSet);
	{
		KG3DENVEFFLIGHTSET_DESCSTRUCT	LightSetStruct;
		HRESULT hr = pLightSet->GetStruct(&LightSetStruct);
		KG_COM_PROCESS_ERROR(hr);
		m_ColorBnSceneAmbient.SetBKColor(LightSetStruct.m_dwSceneAmbient);

		IEKG3DLight* pLight = m_LightList.GetElemValueByCurSel();
		KG_PROCESS_ERROR(pLight);

		KG3DENVEFFLIGHT_DESCSTRUCT	DescStruct;
		hr = pLight->GetStruct(&DescStruct);
		KG_COM_PROCESS_ERROR(hr);

		m_ColorBnDiffuse.SetBKColor(Value2Color(DescStruct.m_D3DLight.Diffuse));
		m_ColorBnAmbient.SetBKColor(Value2Color(DescStruct.m_D3DLight.Ambient));
		m_ColorBnSpecular.SetBKColor(Value2Color(DescStruct.m_D3DLight.Specular));
		m_ColorBnColor0.SetBKColor((DescStruct.m_ColorArgs[0]));
		m_ColorBnColor1.SetBKColor((DescStruct.m_ColorArgs[1]));
		m_ColorBnColor2.SetBKColor((DescStruct.m_ColorArgs[2]));
		m_ColorBnColor3.SetBKColor((DescStruct.m_ColorArgs[3]));

		D3DVECTOR TempVec;
		TempVec.x = -DescStruct.m_D3DLight.Direction.x;
		TempVec.y = -DescStruct.m_D3DLight.Direction.y;
		TempVec.z = -DescStruct.m_D3DLight.Direction.z;
		this->VecToAngle(TempVec, m_fHeadingAngle, m_fHeightAngle);//加-号的原因是把出射角度变成位置
		m_vLightDir = DescStruct.m_D3DLight.Direction;
		D3DXVec3Normalize(&m_vLightDir, &m_vLightDir);

		IEKG3DScene* pScene = KSH::GetEngineInterfaceEx<IEKG3DScene>();
		if(NULL != pScene)
		{
			//m_strEvnMap = pScene->GetEvnMapName().c_str();

			TCHAR envMap[MAX_PATH] = _T("");
			pScene->GetEnvMap(envMap, _countof(envMap));
			m_strEvnMap = envMap;
		}

		UpdateData(FALSE);
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KSceneSettingPageLighting::SynFromInterfaceToInner( DWORD_PTR dwOption /*= 0*/ )
{
	IEKG3DEnvEffLightSet* pLightSet = KSH::GetEngineInterfaceEx<IEKG3DEnvEffLightSet>();
	KG_PROCESS_ERROR(pLightSet);
	{
		KG3DENVEFFLIGHTSET_DESCSTRUCT	LightSetStruct;
		LightSetStruct.m_dwSceneAmbient = m_ColorBnSceneAmbient.GetBkColor();
		LightSetStruct.m_dwModificationOption = KG3DENVEFFLIGHTSET_MDO_ALL;
		HRESULT hr = pLightSet->RequestModification(&LightSetStruct);
		KG_COM_PROCESS_ERROR(hr);
		
		IEKG3DLight* pLight = m_LightList.GetElemValueByCurSel();
		KG_PROCESS_ERROR(pLight);
		KG3DENVEFFLIGHT_DESCSTRUCT	DescStruct;
		DescStruct.m_D3DLight.Diffuse = Color2Value(m_ColorBnDiffuse.GetBkColor());
		DescStruct.m_D3DLight.Ambient = Color2Value(m_ColorBnAmbient.GetBkColor());
		DescStruct.m_D3DLight.Specular = Color2Value(m_ColorBnSpecular.GetBkColor());
		DescStruct.m_ColorArgs[0] = m_ColorBnColor0.GetBkColor();
		DescStruct.m_ColorArgs[1] = m_ColorBnColor1.GetBkColor();
		DescStruct.m_ColorArgs[2] = m_ColorBnColor2.GetBkColor();
		DescStruct.m_ColorArgs[3] = m_ColorBnColor3.GetBkColor();
		

		UpdateData(TRUE);
		
		_ASSERTE(360.0f * (1-FLT_EPSILON) < 360.0f);
		if (MT_FLOAT_EQUAL_L(m_fHeightAngle, 360.0f))//MFC的宏会降低精度的
		{
			m_fHeightAngle *= (1-FLT_EPSILON);
		}

		if (MT_FLOAT_EQUAL_L(m_fHeadingAngle, 180.0f))
		{
			m_fHeadingAngle *= (1- FLT_EPSILON);
		}

		if(m_fHeightAngle < 0 || m_fHeightAngle > 360.0f - FLT_EPSILON)
		{
			MessageBox(_T("高度角超范围"));
			goto Exit0;
		}
		if (m_fHeadingAngle < 0 || m_fHeadingAngle > 180.0f - FLT_EPSILON)
		{
			MessageBox(_T("角度角超范围"));
		}

		D3DVECTOR TempVec;

		D3DXVec3Normalize((D3DXVECTOR3*)&TempVec, &m_vLightDir);

		//KG_PROCESS_ERROR(this->AngleToVec(m_fHeadingAngle, m_fHeightAngle, TempVec));
		//DescStruct.m_D3DLight.Direction = D3DXVECTOR3(-TempVec.x, -TempVec.y, -TempVec.z); //加-号的原因是把位置变成出射角度
		DescStruct.m_D3DLight.Direction = m_vLightDir;

		DescStruct.m_D3DLight.Type = D3DLIGHT_DIRECTIONAL;

		DescStruct.m_D3DLight.Position.x = 0.0f;
		DescStruct.m_D3DLight.Position.y = 400.0f;
		DescStruct.m_D3DLight.Position.z = 0.0f;
		DescStruct.m_dwModificationOption = KG3DENVEFFLIGHT_MDO_ALL;

		hr = pLight->RequestModification(&DescStruct);
		KG_COM_PROCESS_ERROR(hr);

		//同步灯光角度
		{
			CWnd* pCheckSynDirection = this->GetDlgItem(IDC_CHECK2);
			if(pCheckSynDirection)
			{
				if (BST_CHECKED == (int)::SendMessage(pCheckSynDirection->GetSafeHwnd(), BM_GETCHECK, 0, 0))
				{
					_ASSERTE(pLightSet);

					KG3DENVEFFLIGHT_DESCSTRUCT DirectionStruct = DescStruct;
					DirectionStruct.m_dwModificationOption = KG3DENVEFFLIGHT_MDO_D3DLIGHT_DIRECTION;

					INT nSize = 0;
					pLightSet->Size(&nSize);
					for (INT i = 0; i < nSize; ++i)
					{
						IEKG3DLight* pLightTemp = NULL;
						if (SUCCEEDED(pLightSet->Get(i, &pLightTemp)))
						{
							pLightTemp->RequestModification(&DirectionStruct);
						}
					}

				}				
			}
		}

		TCHAR szName[MAX_PATH];

		if (g_GetFilePathFromFullPath(szName, m_strEvnMap.GetBuffer()))
		{
			IEKG3DScene* pScene = KSH::GetEngineInterfaceEx<IEKG3DScene>();

			if (pScene)
			{
				pScene->SetEnvMap(szName);
			}
		}

	}
	return S_OK;
Exit0:
	return E_FAIL;
}
void KSceneSettingPageLighting::OnBnClickedSsListBnAdd()
{
	// TODO: Add your control notification handler code here
	IEKG3DLightSet* pLightSet = KSH::GetEngineInterfaceEx<IEKG3DLightSet>();
	KG_PROCESS_ERROR(pLightSet);
	pLightSet->Add(NULL, NULL);
	m_LightList.Reload();
Exit0:
	return;
}

void KSceneSettingPageLighting::OnBnClickedSsListBnDel()
{
	// TODO: Add your control notification handler code here
	IEKG3DLightSet* pLightSet = KSH::GetEngineInterfaceEx<IEKG3DLightSet>();
	KG_PROCESS_ERROR(pLightSet);
	pLightSet->Del(m_LightList.GetCurSel());
	m_LightList.Reload();
Exit0:
	return;
}



void KSceneSettingPageLighting::OnLbnSelchangeLtSsElement()
{
	// TODO: Add your control notification handler code here	
	m_bIsNowLight0Active = (m_LightList.GetCurSel() == 0);
	
	SynFromInnerToInterface();
}

/*static*/ BOOL KSceneSettingPageLighting::AngleToVec( float fHeadingAngle, float fHeightAngle, D3DVECTOR& vec )
{
	//角度变换
	/*
	vec.x = -cos(m_fHeadingAngle * D3DX_PI / 180.0f) * cos(m_fHeightAngle * D3DX_PI / 180.0f);
		vec.y = -sin(m_fHeightAngle * D3DX_PI / 180.0f);
		vec.z = -sin(m_fHeadingAngle * D3DX_PI / 180.0f) * cos(m_fHeightAngle * D3DX_PI / 180.0f);*/


	KG_PROCESS_ERROR(fHeadingAngle > -FLT_EPSILON && fHeadingAngle < 180.0f);
	KG_PROCESS_ERROR(fHeightAngle > -FLT_EPSILON && fHeightAngle < 360.0f);

	{
		float x = 1.0f, y = 1.0f, z = 1.0f;

		if(MT_FLOAT_EQUAL_L(fHeadingAngle, 90.0f))
		{
			x = fHeightAngle > 90.0f-FLT_EPSILON && fHeightAngle < 270.0f ? -1.0f : 1.0f;
			z = 0.0f;
		}
		else
		{
			x = 1.0f * tan(fHeadingAngle * D3DX_PI / 180.0f);
			x = abs(x);
			if (fHeightAngle > 90.0f-FLT_EPSILON && fHeightAngle < 270.0f)	//第2，3像限
			{
				x= -x;
				if (fHeadingAngle < 90.0f )
				{
					z = -1.0f;
				}
			}
			else
			{
				if (fHeadingAngle > 90.0f - FLT_EPSILON)
				{
					z = -1.0f;
				}
			}
		}

		if (MT_FLOAT_EQUAL_L(fHeightAngle, 90.0f))
		{
			x = 0;
			y = 1; 
			z = 0;
		}
		else if (MT_FLOAT_EQUAL_L(fHeightAngle, 270.0f))
		{
			x = 0;
			y = -1;
			z = 0;
		}
		else
		{
			y = sqrt(x*x + z*z) * abs(tan(fHeightAngle * D3DX_PI / 180.0f));
			if (fHeightAngle > 180.0f - FLT_EPSILON && fHeightAngle < 360.0f)
			{
				y = -y;
			}
		}

		D3DXVECTOR3 TempVecX3(x,y,z);
		D3DXVec3Normalize(&TempVecX3, &TempVecX3);
		vec.x = TempVecX3.x;
		vec.y = TempVecX3.y;
		vec.z = TempVecX3.z;
	}

	return TRUE;
Exit0:
	return FALSE;
}

/*static*/ BOOL KSceneSettingPageLighting::VecToAngle( const D3DVECTOR& vec, float& fHeadingAngle, float& fHeightAngle)
{
	KG_PROCESS_ERROR((abs(vec.x) > FLT_EPSILON) || (abs(vec.y) > FLT_EPSILON) || (abs(vec.z) > FLT_EPSILON));
	{
		struct DoubleVec 
		{
			double x;
			double y;
			double z;
		};
		DoubleVec vec3Dir;//要用双精度，否则180-FLT_EPSILON这种数会被180蚕食掉精度
		{
			D3DXVECTOR3 vec3DirTemp = vec;
			D3DXVec3Normalize(&vec3DirTemp, &vec3DirTemp);
			vec3Dir.x = vec3DirTemp.x;
			vec3Dir.y = vec3DirTemp.y;
			vec3Dir.z = vec3DirTemp.z;
		}
		double HeadingAngle = 0;
		double HeightAngle = 0;
		/*
		m_fAngle  = atan(vec3Dir.z / vec3Dir.x) * 180 / D3DX_PI;
		m_fHeight = asin(-vec3Dir.y) * 180 / D3DX_PI;*/


		//方向角是和Z正方向的夹角，0-180，第三和第四象限取反向角，方向角正好反映春夏变化时地轴和阳光的夹角
		if (MT_DOUBLE_EQUAL_L(vec3Dir.z, 0.0))
		{
			if (MT_DOUBLE_EQUAL_L(vec3Dir.x, 0.0))
			{
				HeightAngle = vec3Dir.y > 0 ? 90.0 : 270.0;
				HeadingAngle = 0.0;	
				
				goto Exit1;
			}
			else
			{
				HeadingAngle = 90.0;
			}	
		}
		else
		{
			HeadingAngle = atan(vec3Dir.x/vec3Dir.z) * 180 / (double)D3DX_PI;
			
			if (HeadingAngle < 0)
			{
				HeadingAngle += ((double)(180 - (double)(FLT_EPSILON)));	//atan的值域是-90~+90，只有第2和第4象限是负的，都映射都第2象限
			}
		}
		

		//高度角是方向角决定的垂直ZX平面的平面上的与右边0轴的正夹角，0-360，这样从0渐变到360正好相当于太阳转1周
		bool bIsInSection1Or2 = (vec3Dir.z > 0 && vec3Dir.x > -(double)FLT_EPSILON) 
			|| (vec3Dir.z < (double)FLT_EPSILON && vec3Dir.x > 0);
		if (MT_DOUBLE_EQUAL_L(vec3Dir.y, 0.0f))
		{
			HeightAngle =  bIsInSection1Or2 ? 0.0 : 180.0;
		}
		else
		{
			HeightAngle = asin(vec3Dir.y) * 180 / D3DX_PI;	//asin是[-1,1]的函数
			//根据不同的象限，纠正高度角
			if (bIsInSection1Or2)	//在第1，2象限中
			{
				if (HeightAngle < 0)
				{
					HeightAngle = MT_DOUBLE_EQUAL_L(HeightAngle, 0.0) ? 
						((double)(360 - (double)(FLT_EPSILON))) 
						: ((double)(360 - (double)(FLT_EPSILON))) + HeightAngle;
				}
			}
			else
			{
				if (HeightAngle > -(double)FLT_EPSILON)
				{
					HeightAngle  = 180.0 - HeightAngle;
				}
				else
				{
					HeightAngle = 180.0 - HeightAngle;
				}
			}
		}
Exit1:
		fHeadingAngle = static_cast<float>(HeadingAngle);
		fHeightAngle = static_cast<float>(HeightAngle);
		if (MT_FLOAT_EQUAL_L(fHeadingAngle, 180.0f))
		{
			fHeadingAngle *= (1-FLT_EPSILON);
		}
		if (MT_FLOAT_EQUAL_L(fHeightAngle, 360.0f))
		{
			fHeightAngle *= (1-FLT_EPSILON);
		}
		_ASSERTE(fHeadingAngle > -FLT_EPSILON && fHeadingAngle < 180.0f);
		_ASSERTE(fHeightAngle > -FLT_EPSILON && fHeightAngle < 360.0f);
		return TRUE;
	}

	
Exit0:
	return FALSE;
}

BOOL KSceneSettingPageLighting::OnInitDialog()
{
	BOOL bRet =CPropertyPage::OnInitDialog();

	CWnd* p = this->GetDlgItem(IDC_CHECK2);
	if (p)
	{
		::SendMessage(p->GetSafeHwnd(), BM_SETCHECK, BST_CHECKED, 0);
	}

	NeedToSynLightSetAmbientWithSunLightArg3(TRUE);
	return bRet;
}

LRESULT KSceneSettingPageLighting::WindowProc( UINT message, WPARAM wParam, LPARAM lParam )
{
	if (message == WM_COLORPOINTER_CHANGE)
	{
		HWND hFrom = reinterpret_cast<HWND>(lParam);
		if (hFrom == m_ColorBnColor2.GetSafeHwnd())
		{
			if (NeedToSynLightSetAmbientWithSunLightArg3() && m_bIsNowLight0Active)
			{
				m_ColorBnSceneAmbient.SetBKColor(static_cast<D3DCOLOR>(wParam));
			}
		}
		SynFromInterfaceToInner();
	}
	return CPropertyPage::WindowProc(message, wParam, lParam);
}
#if defined(KG_ENABLE_TDD)
static BOOL AngleToVec(float fHeadingAngle, float fHeightAngle, D3DVECTOR& vec)
{
	return KSceneSettingPageLighting::AngleToVec(fHeadingAngle, fHeightAngle, vec);
}
static BOOL VecToAngle(const D3DVECTOR& vec, float& fHeadingAngle, float& fHeightAngle)
{ 
	return KSceneSettingPageLighting::VecToAngle(vec, fHeadingAngle, fHeightAngle);
}

KG_TEST_BEGIN(KSceneSettingPageLighting)
	{
		float fSqure2 = sqrt(2.0f);
		D3DVECTOR vecArray[] = 
		{
			{1,fSqure2,1},
			{1,fSqure2,-1},
			{-1,fSqure2,-1},
			{-1,fSqure2,1},

			{1,-fSqure2,1},
			{1,-fSqure2,-1},
			{-1,-fSqure2,-1},
			{-1,-fSqure2,1},

			{1,0,0},
			{0,1,0},
			{0,0,1},
			{-1,0,0},
			{0,-1,0},
			{0,0,-1},

			{1, 0, 1},
			{1, 0, -1},
			{-1, 0, 1},
			{-1, 0, -1},

			{0, 1, 1},
			{0, -1, 1},
			{0, 1, -1},
			{0, -1, -1},

			{1, 1, 0},
			{-1, 1, 0},
			{1, -1, 0},
			{-1, -1, 0},
		};

		struct AngleStruct 
		{
			float m_fHeadingAngle;
			float m_fHeightAngle;
		};

		AngleStruct AngleArray[] = 
		{
			{45.0f, 45.0f},
			{135.0f, 45.0f},
			{45.0f, 135.0f},
			{135.0f,135.0f},

			{45.0f, 315.0f},
			{135.0f, 315.0f},
			{45.0f, 225.0f},
			{135.0f,225.0f},

			{90.0f, 0.0f},
			{0.0f, 90.0f},
			{0.0f, 0.0f},
			{90.0f, 180.0f},
			{0.0f, 270.0f},
			{0.0f, 180.0f},	

			{45.0f, 0.0f},
			{135.0f, 0.0f},
			{135.0f, 180.0f},
			{45.0f, 180.0f},

			{0.0f, 45.0f},
			{0.0f, 315.0f},
			{0.0f, 135.0f},
			{0.0f, 225.0f},

			{90.0f, 45.0f},
			{90.0f, 135.0f},
			{90.0f, 315.0f},
			{90.0f, 225.0f},
		};

		for(size_t i = 0; i < _countof(vecArray); i++)
		{
			D3DXVECTOR3 vec = vecArray[i];
			D3DXVec3Normalize(&vec, &vec);
			vecArray[i].x = vec.x;
			vecArray[i].y = vec.y;
			vecArray[i].z = vec.z;
		}

#define MT_FLOAT_EQUAL_L2(F1, F2) ((F1) > (F2) - 0.0001 && (F1) < (F2) + 0.0001)
		for (size_t i = 0; i < _countof(vecArray); i++)
		{
			float fHeading = 0, fHeight = 0;
			BOOL bRet = VecToAngle(vecArray[i], fHeading, fHeight);
			_ASSERTE(bRet);
			_ASSERTE(MT_FLOAT_EQUAL_L2(fHeading, AngleArray[i].m_fHeadingAngle));
			_ASSERTE(MT_FLOAT_EQUAL_L2(fHeight, AngleArray[i].m_fHeightAngle));
		}

		for (size_t i = 0; i < _countof(vecArray); i++)
		{
			D3DVECTOR vec;
			BOOL bRet = AngleToVec(AngleArray[i].m_fHeadingAngle, AngleArray[i].m_fHeightAngle, vec);
			_ASSERTE(bRet);
			_ASSERTE(MT_FLOAT_EQUAL_L2(vec.x, vecArray[i].x));
			_ASSERTE(MT_FLOAT_EQUAL_L2(vec.y, vecArray[i].y));
			_ASSERTE(MT_FLOAT_EQUAL_L2(vec.z, vecArray[i].z));
		}

		//_KG3D_DEBUG_REPORT("AngleToVec and VecToAngle RunTime Test All Passed, Congratulations!");
	}
KG_TEST_END(KSceneSettingPageLighting)
#endif
