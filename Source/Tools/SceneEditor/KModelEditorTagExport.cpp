// KModelEditorTagExport.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KModelEditorTagExport.h"
#include "IEKG3DAnimationTag.h"
#include "IEKG3DClip.h"
#include "IEKG3DClipTable.h"
// KModelEditorTagExport dialog

IMPLEMENT_DYNAMIC(KModelEditorTagExport, CDialog)

KModelEditorTagExport::KModelEditorTagExport(CWnd* pParent /*=NULL*/)
	: CDialog(KModelEditorTagExport::IDD, pParent)
{
	m_pContainer = NULL;
	m_strTaniName = "";
	m_eTagState = TAG_Player;
}

KModelEditorTagExport::~KModelEditorTagExport()
{
}

void KModelEditorTagExport::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_ANI, m_ListBox_Animation);
	DDX_Control(pDX, IDC_COMBO1, m_comboSource);
	DDX_Control(pDX, IDC_COMBO2, m_comboTarget);
	DDX_Control(pDX, IDC_LIST_SOURCE,m_ListBox_Source);
	DDX_Control(pDX, IDC_LIST_TARGET,m_ListBox_Target);
}


BEGIN_MESSAGE_MAP(KModelEditorTagExport, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_OPENFILE, &KModelEditorTagExport::OnBnClickedButtonOpenfile)
	ON_BN_CLICKED(IDC_BUTTON_EXPORT_TANI, &KModelEditorTagExport::OnBnClickedButtonExportTani)
	ON_BN_CLICKED(IDC_RADIO1, &KModelEditorTagExport::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &KModelEditorTagExport::OnBnClickedRadio2)
	ON_CBN_SELCHANGE(IDC_COMBO1, &KModelEditorTagExport::OnCbnSelchangeCombo1)
	ON_BN_CLICKED(IDC_BATCH_EXPORT, &KModelEditorTagExport::OnBnClickedBatchExport)
	ON_CBN_SELCHANGE(IDC_COMBO2, &KModelEditorTagExport::OnCbnSelchangeCombo2)
END_MESSAGE_MAP()


// KModelEditorTagExport message handlers
BOOL KModelEditorTagExport::OnInitDialog()
{
	BOOL bReturn = CDialog::OnInitDialog();
	((CButton*)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	((CButton*)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
	RefreshDirectory();
	return bReturn;
}
void KModelEditorTagExport::SetAnimationTagContainer(IEKG3DAnimationTagContainer* pContainer)
{
	m_pContainer = pContainer;
}
void KModelEditorTagExport::SetTaniName(CString strTaniName)
{
	m_strTaniName = strTaniName;
}
void KModelEditorTagExport::OnBnClickedButtonOpenfile()
{
	// TODO: Add your control notification handler code here
	TCHAR strFileNameBuffer[MAX_PATH * 10] = "";
	TCHAR strDefaultPath[MAX_PATH];

	LPCSTR strFilter = "Animation Tag Files (*.ani)|*.ani||";
	CFileDialog DlgLoad(TRUE, 
		".ani", 
		"", 
		OFN_HIDEREADONLY | OFN_ALLOWMULTISELECT, 
		strFilter, 
		this);

	sprintf(strDefaultPath, "%sData\\source\\player\\", g_szDefWorkDirectory);

	DlgLoad.m_ofn.lpstrFile = strFileNameBuffer;
	DlgLoad.m_ofn.nMaxFile = sizeof(strFileNameBuffer);
	DlgLoad.m_ofn.lpstrInitialDir = strDefaultPath;
	KG_PROCESS_SUCCESS(DlgLoad.DoModal() == IDCANCEL);


	POSITION FilePos = DlgLoad.GetStartPosition();
	
	while(FilePos)
	{
		char strFileName[MAX_PATH];
		CString strPathName = DlgLoad.GetNextPathName(FilePos);
		m_vecAinName.push_back(strPathName);
		_splitpath(strPathName, NULL, NULL, strFileName, NULL);
		m_ListBox_Animation.AddString(strFileName);
	}
Exit1:
	
	;
}

void KModelEditorTagExport::OnBnClickedButtonExportTani()
{
	HRESULT hr = E_FAIL;
	IEKG3DClipTable *piClipTable = NULL;
	//IEKG3DClip* pCurClip = NULL;
	DWORD dwCurNumFrames = 0;
	vector<CString> vecError;
	KG_PROCESS_ERROR(m_pContainer);
	KG_PROCESS_ERROR(m_ListBox_Animation.GetCount()>0);
	
	hr = g_pEngineManager->GetIEKG3DClipTable(&piClipTable);
	KG_COM_PROCESS_ERROR(hr);

	KG_PROCESS_ERROR(MessageBox("原有的同名tani文件将被覆盖掉，确定要执行？","提示",MB_OKCANCEL) == IDOK);
	
	for(DWORD i=0;i< m_vecAinName.size();i++)
	{
		IEKG3DClip* pClip = NULL;
		piClipTable->LoadResourceFromFileInEditor(m_vecAinName[i], 0, 0, (IEKG3DClip **)&pClip);
		if(pClip)
		{
			DWORD dwNumFrames = 0;
			pClip->GetNumFrames(&dwNumFrames);
			hr = m_pContainer->ExportTani(m_vecAinName[i],dwNumFrames);
			if(FAILED(hr))
				vecError.push_back(m_vecAinName[i]);

			hr = m_pContainer->Load(m_strTaniName);
			KG_COM_PROCESS_ERROR(hr);
		}
		SAFE_RELEASE(pClip);
	}
	if (vecError.size()> 0)
	{
		std::string strText = "以下文件导出tani失败\n";
		for (DWORD i = 0; i<vecError.size();i++)
		{
			strText += vecError[i];
			strText += "\n";
		}
		MessageBox(strText.c_str());
	}
	else
		MessageBox("恭喜，导出完成。可以继续你的工作了！！！");
Exit0:
	;
}

void KModelEditorTagExport::RefreshDirectory()
{
	switch(m_eTagState)
	{
	case TAG_Player:
		{
			m_strPath.Format("%sData\\source\\player\\", g_szDefWorkDirectory);
		}
		break;
	case TAG_NPC:
		{
			m_strPath.Format("%sData\\source\\NPC_source\\", g_szDefWorkDirectory);
		}
	    break;
	}
	m_comboTarget.ResetContent();
	m_comboSource.ResetContent();
	m_ListBox_Source.ResetContent();
	m_ListBox_Target.ResetContent();
	CFileFind Find;
	TCHAR strFind[MAX_PATH];
	sprintf_s(strFind,
		"%s*.*",
		m_strPath);
	BOOL bReturn = Find.FindFile(strFind);
	while (bReturn)
	{
		bReturn = Find.FindNextFile();
		if (Find.IsDirectory())
		{
			if (Find.GetFileName() != "." && Find.GetFileName() != ".." &&Find.GetFileName().MakeLower() != ".svn")
			{
				m_comboSource.AddString(Find.GetFileName());
				m_comboTarget.AddString(Find.GetFileName());
			}
		}
	}
	
	
}
void KModelEditorTagExport::OnBnClickedRadio1()
{
	// TODO: Add your control notification handler code here
	m_eTagState = TAG_Player;
	RefreshDirectory();
}

void KModelEditorTagExport::OnBnClickedRadio2()
{
	// TODO: Add your control notification handler code here
	m_eTagState = TAG_NPC;
	RefreshDirectory();
}

void KModelEditorTagExport::OnCbnSelchangeCombo1()
{
	// TODO: Add your control notification handler code here
	CFileFind Find;
	TCHAR strFind[MAX_PATH];
	
	m_ListBox_Source.ResetContent();
	int nCurSel = m_comboSource.GetCurSel();

	KG_PROCESS_ERROR(nCurSel != CB_ERR);
	m_comboSource.GetLBText(nCurSel, m_strSource);
	sprintf_s(strFind,"%s%s\\动作\\*.tani",m_strPath,m_strSource);
	BOOL bReturn = Find.FindFile(strFind);
	while (bReturn)
	{
		bReturn = Find.FindNextFile();
		if (!Find.IsDirectory())
		{
			if (Find.GetFileName() != "." && Find.GetFileName() != ".." &&Find.GetFileName().MakeLower() != ".svn")
			{
				char strName[MAX_PATH];
				CString strFileName = Find.GetFileName();
				_splitpath(strFileName, NULL, NULL, strName, NULL);
				m_ListBox_Source.AddString(strName);			
			}
		}
	}
Exit0:
	;
}

void KModelEditorTagExport::OnBnClickedBatchExport()
{
	HRESULT hr = E_FAIL;
	IEKG3DClipTable *piClipTable = NULL;
	DWORD dwCurNumFrames = 0;
	vector<CString> vecError;
	CString strText;
	TCHAR strSouceTani[MAX_PATH];
	TCHAR strTargetAni[MAX_PATH];
	DWORD dwCount = m_ListBox_Source.GetCount();
	KG_PROCESS_ERROR(m_pContainer);
	KG_PROCESS_ERROR(dwCount>0);

	hr = g_pEngineManager->GetIEKG3DClipTable(&piClipTable);
	KG_COM_PROCESS_ERROR(hr);

	KG_PROCESS_ERROR(MessageBox("如果原有的同名tani文件存在将会被覆盖掉，确定要执行？","提示",MB_OKCANCEL) == IDOK);

	for(DWORD i=0;i< dwCount;i++)
	{
		IEKG3DClip* pClip = NULL;

		m_ListBox_Source.GetText(i,strText);
		sprintf_s(strSouceTani,"%s%s\\动作\\%s.tani",m_strPath,m_strSource,strText);

		int nIndex = strText.Find(m_strSource);
		if(nIndex != 0)
			continue;
		CString strTargetName;
		strTargetName.Format("%s%s", m_strTarget,strText.Right(strText.GetLength() - m_strSource.GetLength()));
		sprintf_s(strTargetAni,"%s%s\\动作\\%s.ani",m_strPath,m_strTarget,strTargetName);

		piClipTable->LoadResourceFromFileInEditor(strTargetAni, 0, 0, (IEKG3DClip **)&pClip);
		if(pClip)
		{
			HRESULT hr =E_FAIL;
			if(SUCCEEDED(m_pContainer->Load(strSouceTani)))
			{
				DWORD dwNumFrames = 0;
				pClip->GetNumFrames(&dwNumFrames);
				hr = m_pContainer->ExportTani(strTargetAni,dwNumFrames);
			}
			if(FAILED(hr))
				vecError.push_back(strTargetAni);			
		}
		SAFE_RELEASE(pClip);
	}
	m_pContainer->Load(m_strTaniName);
	if (vecError.size()> 0)
	{
		std::string strText = "以下文件导出tani失败\n";
		for (DWORD i = 0; i<vecError.size();i++)
		{
			strText += vecError[i];
			strText += "\n";
		}
		
		MessageBox(strText.c_str());
	}
	else
		MessageBox("恭喜，导出完成。可以继续你的工作了！！！");
Exit0:
	;
}

void KModelEditorTagExport::OnCbnSelchangeCombo2()
{
	// TODO: Add your control notification handler code here
	CFileFind Find;
	TCHAR strFind[MAX_PATH];
	m_ListBox_Target.ResetContent();
	int nCurSel = m_comboTarget.GetCurSel();
	KG_PROCESS_ERROR(nCurSel != CB_ERR);
	m_comboTarget.GetLBText(nCurSel, m_strTarget);

	sprintf_s(strFind,"%s%s\\动作\\*.ani",m_strPath,m_strTarget);
	BOOL bReturn = Find.FindFile(strFind);
	while (bReturn)
	{
		bReturn = Find.FindNextFile();
		if (!Find.IsDirectory())
		{
			if (Find.GetFileName() != "." && Find.GetFileName() != ".." &&Find.GetFileName().MakeLower() != ".svn")
			{
				char strName[MAX_PATH];
				CString strFileName = Find.GetFileName();
				_splitpath(strFileName, NULL, NULL, strName, NULL);
				m_ListBox_Target.AddString(strName);			
			}
		}
	}
Exit0:
	;
}
