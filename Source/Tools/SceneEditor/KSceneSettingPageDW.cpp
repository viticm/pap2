// KSceneSettingPageDW.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneSettingPageDW.h"
#include "IEKG3DRegionInfoManager.h"
#include "KSceneCommonParamDialog.h"
#include "KSceneSceneHelpers.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
// KSceneSettingPageDW dialog

IMPLEMENT_DYNAMIC(KSceneSettingPageDW, CPropertyPage)

KSceneSettingPageDW::KSceneSettingPageDW()
	: KSceneSettingPageBase(KSceneSettingPageDW::IDD)
	, m_nTexID(-1)
	, m_nOutputWndID(-1)
	//, m_RegionList(stTempHelper)
	, m_RegionList(TypeContainerGetter()
			, TypeElemGetter(&IEKG3DRegionInfoManager::GetDiffArrayCount, &IEKG3DRegionInfoManager::GetDiffArray)
			, TypeNameGetter())
	, m_bShowRegionMap(FALSE)
	, m_pLoadParamStructBuffer(NULL)
	, m_bNeedShowRegionMap(FALSE)
	
{
	m_nFlag = 0;
}

KSceneSettingPageDW::~KSceneSettingPageDW()
{
	SAFE_DELETE_ARRAY(m_pLoadParamStructBuffer);
	//SAFE_DELETE(m_pCWndKeyTrack);
}

void KSceneSettingPageDW::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_ST_OUTPUT_WND, m_stOutputWnd);
	DDX_Control(pDX, IDC_LT_SS_ELEMENT, m_RegionList);
	DDX_Control(pDX, IDC_CK_SHOW_REGION_SPLIT_MAP, m_ckShowRegionMap);
	DDX_Control(pDX, IDC_SS_ED_TEXPATH, m_edTexPath);
}
BEGIN_MESSAGE_MAP(KSceneSettingPageDW, CPropertyPage)
	ON_BN_CLICKED(IDC_SS_BN_READ_REGION_SPLIT_MAP, &KSceneSettingPageDW::OnBnClickedSsBnReadRegionSplitMap)
	ON_BN_CLICKED(IDC_CK_SHOW_REGION_SPLIT_MAP, &KSceneSettingPageDW::OnBnClickedCkShowRegionSplitMap)
	ON_BN_CLICKED(IDC_SS_BN_READ_REGION_SPLIT_MAP_AGAIN, &KSceneSettingPageDW::OnBnClickedSsBnReadRegionSplitMapAgain)
	ON_BN_CLICKED(IDC_BUTTON1, &KSceneSettingPageDW::OnBnClickedButtonKeyTrack)
END_MESSAGE_MAP()

int KSceneSettingPageDW::Update()
{
	return 1;
}

HRESULT KSceneSettingPageDW::ShowRegionMap(BOOL bShow)
{
	IEKG3DRegionInfoManager* pRI = this->GetCurRegionInformer();
	KG_PROCESS_ERROR(pRI);
	{
		if (bShow)
		{
			BOOL bRet = FALSE;
			BOOL bExist = FALSE;
			INT  nID = -1;
			HRESULT hr = pRI->GetRegionTexState(&bExist, &nID, NULL, 0);
			if (SUCCEEDED(hr) && bExist)
			{
				_KG3D_DEBUG_FAIL_REPORT(nID >= 0);
				bRet = AddOutputTexWnd(nID);
			}
			else
			{
				DWORD dwTexID = 0;
				HRESULT hr = pRI->GenerateRegionTex(0, &dwTexID);
				if (SUCCEEDED(hr))
				{
					bRet= AddOutputTexWnd(dwTexID);
				}
			}
			if (!bRet)
			{
				MessageBox("添加贴图输出失败");
			}
			KSH::SynACButtonWithABool(m_ckShowRegionMap, TRUE, TRUE);
			m_bNeedShowRegionMap = TRUE;
		}
		else
		{
			this->RemoveOutputTexWnd();
			KSH::SynACButtonWithABool(m_ckShowRegionMap, FALSE, FALSE);
			m_bNeedShowRegionMap = FALSE;
		}
	}
	return S_OK;
Exit0:
	return E_FAIL;
}

// KSceneSettingPageDW message handlers

HRESULT KSceneSettingPageDW::SynFromInterfaceToInner(DWORD_PTR dwOption/* = 0*/ )
{
	return S_OK;
}
HRESULT KSceneSettingPageDW::SynFromInnerToInterface(DWORD_PTR dwOption /*= 0*/ )
{
	m_RegionList.Reload();
	ShowRegionMap(m_bNeedShowRegionMap);
	IEKG3DRegionInfoManager* pRIM = this->GetCurRegionInformer();
	if (pRIM)
	{
		CHAR cPathBuffer[MAX_PATH];
		ZeroMemory(cPathBuffer, sizeof(CHAR)*_countof(cPathBuffer));
		HRESULT hr = pRIM->GetPath(cPathBuffer, _countof(cPathBuffer));
		_ASSERTE(SUCCEEDED(hr));
		cPathBuffer[_countof(cPathBuffer) - 1] = '\0';///无论如何都加上结束符号
		this->m_edTexPath.SetWindowText(cPathBuffer);			
	}
	return S_OK;
}
KG3DREGION_INFORMER_LOADFILE_PARAM* KSceneSettingPageDW::GetLoadParam(IEKG3DRegionInfoManager* pRIM)
{
	KG3DREGION_INFORMER_LOADFILE_PARAM	TempParam;
	KSceneCommonParamDialog TempParamDlg;
	HRESULT hr = E_FAIL;
	if (pRIM)
	{
		hr = pRIM->GetDefaultParam(&TempParam);
		_ASSERTE(SUCCEEDED(hr));
	}	
	INT nIndex1 = TempParamDlg.AddAnParam(_T("像素容差阈值"), TempParam.nPixelTolerance,TRUE);
	INT nIndex2 = TempParamDlg.AddAnParam(_T("采样次数(1-3)"), TempParam.nSampleTimes,TRUE);
	INT nIndex3 = TempParamDlg.AddAnParam(_T("蚕食像素碎片"), TempParam.nEliminateIsoDataCount,TRUE);
	INT nIndex4 = TempParamDlg.AddAnParam(_T("色调分离"), TempParam.nTotalValue,TRUE);
	KG_PROCESS_ERROR(nIndex1 >= 0 && nIndex2 >= 0);
	if (TempParamDlg.DoModal() == IDOK)
	{
		bool bHasOnceParamGetWrong = false;
		BOOL bRet = TempParamDlg.GetData(nIndex1, TempParam.nPixelTolerance);
		if(!bRet)				
			bHasOnceParamGetWrong = TRUE;
		bRet = TempParamDlg.GetData(nIndex2, TempParam.nSampleTimes);
		if (!bRet)				
			bHasOnceParamGetWrong = TRUE;
		bRet = TempParamDlg.GetData(nIndex3, TempParam.nEliminateIsoDataCount);
		if (!bRet)
			bHasOnceParamGetWrong = TRUE;
		bRet = TempParamDlg.GetData(nIndex4, TempParam.nTotalValue);
		if (!bRet)
			bHasOnceParamGetWrong = TRUE;
		if (bHasOnceParamGetWrong)
		{
			MessageBox(_T("参数不正确，采用默认参数载入"), _T("提示"), MB_OK | MB_ICONINFORMATION);
			if (pRIM)
			{
				hr = pRIM->GetDefaultParam(&TempParam);
				_ASSERTE(SUCCEEDED(hr));
			}
		}
		TempParam.dwRetTextureID = 0;
		TempParam.m_bNeedPreviewTex = TRUE;///这个。。。以后要不要做成能请求贴图。
		*m_pLoadParam = TempParam;
		return KGCommonBufferGetPtr(m_pLoadParam);
	}
Exit0:
	return NULL;
}
CString KSceneSettingPageDW::GetLoadPath()
{
	char szFilter[] ="Texture Files(*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS)|*.bmp;*.jpg;*.png;*.tga;*.Spr;*.DDS||";
	CFileDialog dlg(TRUE, NULL, NULL, OFN_FILEMUSTEXIST | OFN_HIDEREADONLY , szFilter);
	if (dlg.DoModal() == IDOK)
	{
		return dlg.GetPathName();
	}
	return _T("");
}

void KSceneSettingPageDW::OnBnClickedSsBnReadRegionSplitMap()
{
	// TODO: Add your control notification handler code here
	HRESULT hr = E_FAIL;

	IEKG3DRegionInfoManager* pRM = this->GetCurRegionInformer();
	KG_PROCESS_ERROR(pRM);
	{
		CString csLoadPath = this->GetLoadPath();
		KG_PROCESS_ERROR(csLoadPath != _T(""));
		KG3DREGION_INFORMER_LOADFILE_PARAM* pLoadParam = this->GetLoadParam(pRM);
		KG_PROCESS_ERROR(pLoadParam);
		ReloadRegionMap(csLoadPath, pLoadParam, sizeof(KG3DREGION_INFORMER_LOADFILE_PARAM));
		return;
	}
Exit0:
	MessageBox(_T("贴图读取失败"),_T("提示"), MB_ICONINFORMATION | MB_OK);
	return;
}

void KSceneSettingPageDW::OnBnClickedCkShowRegionSplitMap()
{
	// TODO: Add your control notification handler code here
	BOOL bTemp = FALSE;
	KSH::SynACButtonWithABool(m_ckShowRegionMap, bTemp, FALSE);
	if ((bTemp&& !m_bNeedShowRegionMap) || (!bTemp && m_bNeedShowRegionMap ))
	{
		ShowRegionMap(bTemp);
	}
}

VOID KSceneSettingPageDW::RemoveOutputTexWnd()
{
	IEKG3DScene* pScene = KSH::GetEngineInterface<KG3DTYPE_SCENE>();
	if (pScene)
	{
		HRESULT hr = pScene->RemoveOutputWindow(m_nOutputWndID);
		if (SUCCEEDED(hr))
		{
			m_nOutputWndID = -1;
			m_bShowRegionMap = FALSE;
			KSH::SynACButtonWithABool(m_ckShowRegionMap, m_bShowRegionMap, TRUE);
			this->Invalidate();
		}
	}
}

BOOL KSceneSettingPageDW::AddOutputTexWnd( INT nTexID )
{
	if (m_nOutputWndID >= 0)
	{
		RemoveOutputTexWnd();
	}
	KG_PROCESS_ERROR(nTexID != KG3DREGION_INFORMER_LOADFILE_PARAM::INVALID_MAX_TEXTURE_ID);
	{
		IEKG3DScene* pScene = KSH::GetEngineInterface<KG3DTYPE_SCENE>();
		KG_PROCESS_ERROR(pScene);
		{
			INT nTempRetWinID = -1;
			OUTPUTWND_TEXTURE_PARAM TempParam;
			TempParam.dwTextureID = nTexID;
			HRESULT hr = pScene->AddOutputWindow(_T("Region分块贴图"), m_stOutputWnd.GetSafeHwnd(), OUTPUTWND_SCENEEDITOR_TEXTUREVIEW, (DWORD_PTR)(&TempParam), &nTempRetWinID);
			KG_PROCESS_ERROR(SUCCEEDED(hr) && nTempRetWinID >= 0);
			m_nOutputWndID = nTempRetWinID;
			m_nTexID = nTexID;
			m_bShowRegionMap = TRUE;
		}
	}
	return TRUE;
Exit0:
	MessageBox("添加贴图输出窗口失败");
	return FALSE;
}
IEKG3DRegionInfoManager* KSceneSettingPageDW::GetCurRegionInformer( DWORD_PTR* pdwHandle )
{
	return KSH::GetEngineInterface<KG3DTYPE_REGION_INFO_MANAGER>();
}

BOOL KSceneSettingPageDW::OnInitDialog()
{
	CPropertyPage::OnInitDialog();


	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

BOOL KSceneSettingPageDW::OnKillActive()
{
	// TODO: Add your specialized code here and/or call the base class
	RemoveOutputTexWnd();
	return CPropertyPage::OnKillActive();
}

BOOL KSceneSettingPageDW::OnSetActive()
{
	// TODO: Add your specialized code here and/or call the base class

	m_CWndKeyTrack.SetAssociateHWnd(::GetParent(this->GetSafeHwnd()));

	SynFromInnerToInterface();
	return CPropertyPage::OnSetActive();
}

void KSceneSettingPageDW::OnBnClickedSsBnReadRegionSplitMapAgain()
{
	// TODO: Add your control notification handler code here
	ReloadRegionMap(m_csRegionMapPath, m_pLoadParamStructBuffer, sizeof(KG3DREGION_INFORMER_LOADFILE_PARAM));
}

void KSceneSettingPageDW::ReloadRegionMap( const CString& csTempString, VOID* pParam, INT nParamSize )
{
	_ASSERTE(nParamSize == sizeof(KG3DREGION_INFORMER_LOADFILE_PARAM));
	IEKG3DRegionInfoManager* pRI = KSH::GetEngineInterface<KG3DTYPE_REGION_INFO_MANAGER>();
	KG_PROCESS_ERROR(pRI && pParam);
	{
		KG3DREGION_INFORMER_LOADFILE_PARAM TempParam = *(KG3DREGION_INFORMER_LOADFILE_PARAM*)pParam;
		HRESULT hr = pRI->IELoadFromFile((LPCTSTR)csTempString, 0, reinterpret_cast<DWORD_PTR>(&TempParam));
		KG_COM_PROCESS_ERROR(hr);
		{
			///保存载入参数
			if (!m_pLoadParamStructBuffer)
			{
				m_pLoadParamStructBuffer = new CHAR[sizeof(KG3DREGION_INFORMER_LOADFILE_PARAM)];
			}
			if (m_pLoadParamStructBuffer && (VOID*)pParam != (VOID*)m_pLoadParamStructBuffer)
			{
				memcpy_s(m_pLoadParamStructBuffer, sizeof(KG3DREGION_INFORMER_LOADFILE_PARAM)
					, &TempParam, sizeof(KG3DREGION_INFORMER_LOADFILE_PARAM));
			}
			if (&csTempString != &m_csRegionMapPath)
			{
				m_csRegionMapPath = csTempString;
			}
			
		}
		SynFromInnerToInterface();
	}
Exit0:
	return;
}
void KSceneSettingPageDW::OnBnClickedButtonKeyTrack()
{
	// TODO: Add your control notification handler code here
	//显示或者隐藏关键帧编辑	
	if (! m_CWndKeyTrack.GetSafeHwnd())
	{
		BOOL bRet = m_CWndKeyTrack.Create(KSceneSettingDialogDWGCKeyTrack::IDD, NULL);
		if (! bRet)
		{
			MessageBox("关键帧编辑窗口创建失败");
		}
	}
	if (m_CWndKeyTrack.GetSafeHwnd())
	{
		if (NULL == KSH::GetEngineInterfaceEx<IEKG3DEnvEffDynamicWeather>() )
		{
			MessageBox("动态天气不存在，先创建DynamicWeather");
			return;
		}
		if (m_CWndKeyTrack.IsWindowVisible())
		{
			m_CWndKeyTrack.ShowWindow(SW_HIDE);
		}
		else
		{
			CRect Rect;
			m_CWndKeyTrack.GetWindowRect(&Rect);

			//CRect ThisRect;
			CRect ThisRectScreen;
			//this->GetClientRect(&ThisRect);
			this->GetWindowRect(&ThisRectScreen);

			CRect Temp;
			Temp.left = ThisRectScreen.left;
			//如果屏幕上面还有地方放，就放上面，否则放下面
			Temp.top = ThisRectScreen.top < Rect.Height() + 20 ? ThisRectScreen.bottom + 20 : ThisRectScreen.top - Rect.Height() - 20;
			Temp.right = Temp.left + Rect.Width();
			Temp.bottom = Temp.top + Rect.Height();
			
			m_CWndKeyTrack.ShowWindow(SW_SHOW);
			m_CWndKeyTrack.MoveWindow(&Temp, TRUE);
		}
	}
}
