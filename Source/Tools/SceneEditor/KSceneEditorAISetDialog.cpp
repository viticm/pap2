// KSceneEditorAISetDialog.cpp : implementation file
//

#include "stdafx.h"
#include "SceneEditor.h"
#include "KSceneEditorAISetDialog.h"
#include "KSceneEditorDocLogical.h"
#include "KSceneCommonParamDialog.h"
#include "IEEDITOR.h"

// KSceneEditorAISetDialog dialog

IMPLEMENT_DYNAMIC(KSceneEditorAISetDialog, CDialog)

KSceneEditorAISetDialog::KSceneEditorAISetDialog(CWnd* pParent /*=NULL*/)
	: CDialog(KSceneEditorAISetDialog::IDD, pParent)
	, m_szName(_T(""))
{
	m_lpSceneSceneEditor = NULL;
	m_pDocLogical = NULL;
	m_dwSelectedID = 0;
	m_dwCurrentNpcTempleteID = 0;
	m_pCurrentNpc = NULL;
}

KSceneEditorAISetDialog::~KSceneEditorAISetDialog()
{
}

void KSceneEditorAISetDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST1, m_List);
	DDX_Text(pDX, IDC_EDIT_NAME, m_szName);
}


BEGIN_MESSAGE_MAP(KSceneEditorAISetDialog, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_EDIT, &KSceneEditorAISetDialog::OnBnClickedButtonEdit)
	ON_BN_CLICKED(IDC_BUTTON_NEW, &KSceneEditorAISetDialog::OnBnClickedButtonNew)
	ON_BN_CLICKED(IDC_BUTTON_DELETE, &KSceneEditorAISetDialog::OnBnClickedButtonDelete)
	ON_LBN_SELCHANGE(IDC_LIST1, &KSceneEditorAISetDialog::OnLbnSelchangeList1)
	ON_BN_CLICKED(IDC_BUTTON_CLEARSELECTION, &KSceneEditorAISetDialog::OnBnClickedButtonClearselection)
	ON_EN_CHANGE(IDC_EDIT_NAME, &KSceneEditorAISetDialog::OnEnChangeEditName)
	ON_BN_CLICKED(IDC_BUTTON_NEWTEMPLETE, &KSceneEditorAISetDialog::OnBnClickedButtonNewtemplete)
END_MESSAGE_MAP()


// KSceneEditorAISetDialog message handlers
void KSceneEditorAISetDialog::ApplyModiyToNpcList(BOOL bModifyToAll)
{
	ApplyModiyToNpcList(m_OldAIParameterSet,m_NewAIParameterSet,m_TemplateAIParameterSet, bModifyToAll);
}

void KSceneEditorAISetDialog::ApplyModiyToNpcList(KSceneEditorDocLogical::AIParameterSet&  OldAIParameterSet,
KSceneEditorDocLogical::AIParameterSet&  NewAIParameterSet,
KSceneEditorDocLogical::AIParameterSet&  TemplateAIParameterSet,
BOOL bModifyToAll)
{
	if (bModifyToAll)
	{
		list<KSceneEditorDocLogical::NPCInfo*>::iterator i = m_listNpcToModify.begin();
		while (i!=m_listNpcToModify.end())
		{
			KSceneEditorDocLogical::NPCInfo* pNpcData = *i;

			for (size_t s=0;s<NewAIParameterSet.vecParameters.size();s++)
			{
				KSceneEditorDocLogical::AIParameter& Par_old = OldAIParameterSet.vecParameters[s];
				KSceneEditorDocLogical::AIParameter& Par_new = NewAIParameterSet.vecParameters[s];

				if(Par_new.strValue != Par_old.strValue)
				{
					pNpcData->m_cAIParameterSet.vecParameters[s].strValue = Par_new.strValue;
				}
			}

			pNpcData->dwNpcAISet = m_pDocLogical->FindAIParameterSet(pNpcData->nTempleteID,pNpcData->m_cAIParameterSet);

			if(pNpcData->pPointer)
			{
				IEKG3DRepresentNPC* pRepNPC = m_lpSceneSceneEditor->GetIEKG3DRepresentNPC(pNpcData->pPointer);
				if(pRepNPC)
					pRepNPC->SetAISetID(pNpcData->dwNpcAISet);
			}
			i++;
		}
	}
	else
	{
		for (size_t s=0;s<NewAIParameterSet.vecParameters.size();s++)
		{
			KSceneEditorDocLogical::AIParameter& Par_old = OldAIParameterSet.vecParameters[s];
			KSceneEditorDocLogical::AIParameter& Par_new = NewAIParameterSet.vecParameters[s];

			if(Par_new.strValue != Par_old.strValue)
			{
				m_pCurrentNpc->m_cAIParameterSet.vecParameters[s].strValue = Par_new.strValue;
			}
		}

		m_pCurrentNpc->dwNpcAISet = m_pDocLogical->FindAIParameterSet(m_pCurrentNpc->nTempleteID,m_pCurrentNpc->m_cAIParameterSet);

		if(m_pCurrentNpc->pPointer)
		{
			IEKG3DRepresentNPC* pRepNPC = m_lpSceneSceneEditor->GetIEKG3DRepresentNPC(m_pCurrentNpc->pPointer);
			if(pRepNPC)
				pRepNPC->SetAISetID(m_pCurrentNpc->dwNpcAISet);
		}
	}
}

void KSceneEditorAISetDialog::OnBnClickedButtonEdit()
{
    KSceneCommonParamDialog                 ParamDlg;
    int                                     nRetParamIndex[30];

    if(!m_pDocLogical)
		return;

	if(!m_pCurrentNpc)
        return;

    m_OldAIParameterSet = m_pCurrentNpc->m_cAIParameterSet;
    m_NewAIParameterSet = m_OldAIParameterSet;

    m_pDocLogical->GetNpcDefauleAIParameterSetByTempleteID(m_pCurrentNpc->nTempleteID, &m_TemplateAIParameterSet);

	m_pDocLogical->UpdataOneNpcAIParameters(m_pCurrentNpc,m_pDocLogical->m_bUseNpcWayPointSetPointer);

	for (size_t i = 0; i < m_NewAIParameterSet.vecParameters.size(); i++)
	{
		KSceneEditorDocLogical::AIParameter& para = m_NewAIParameterSet.vecParameters[i];

		BOOL bEnableEdit = TRUE;
		if(_strcmpi(para.strName.c_str(),"PatrolPathID")==0)
			bEnableEdit = FALSE;
		nRetParamIndex[i] = ParamDlg.AddAnParam(para.strName.c_str(), para.strValue,bEnableEdit);
	}

	if (IDOK != ParamDlg.DoModal())
        return;

    for (size_t i = 0; i < m_NewAIParameterSet.vecParameters.size(); i++)
    {
        KSceneEditorDocLogical::AIParameter& para = m_NewAIParameterSet.vecParameters[i];

        ParamDlg.GetData(nRetParamIndex[i], para.strValue);
    }

    if (m_NewAIParameterSet.IsEqual(m_OldAIParameterSet))//检测是否进行过变更
        return;
	else
	{
		;
	}

	m_pCurrentNpc->m_cAIParameterSet = m_NewAIParameterSet;
	m_pCurrentNpc->dwNpcAISet = m_pDocLogical->FindAIParameterSet(m_pCurrentNpc->nTempleteID,m_NewAIParameterSet);
	
	// TODO: Add your control notification handler code here
    return;
}

void KSceneEditorAISetDialog::OnBnClickedButtonNew()
{
	if(!m_pDocLogical)
		return;
	DWORD dwID;
	m_pDocLogical->NewOneAISet(dwID,"New AI Set");
	FillList();
}

void KSceneEditorAISetDialog::OnBnClickedButtonDelete()
{
	if(!m_pDocLogical)
		return;

	if(m_dwSelectedID)
	{
		m_pDocLogical->DeleteAISet(m_dwSelectedID);
		m_dwSelectedID = 0;
	}
	FillList();
}

void KSceneEditorAISetDialog::FillList()
{
	{
		int nCount = m_List.GetCount();
		for (int i=0;i<nCount;i++)
		{
			m_List.DeleteString(0);
		}
	}

	if(!m_pDocLogical)
		return;

	map<DWORD,KSceneEditorDocLogical::AIParameterSet>::iterator i = m_pDocLogical->m_mapNpcAISceneParameterSet.begin();
	map<DWORD,KSceneEditorDocLogical::AIParameterSet>::iterator iend = m_pDocLogical->m_mapNpcAISceneParameterSet.end();
	while (i!=iend)
	{
		DWORD dwID = i->first;
		KSceneEditorDocLogical::AIParameterSet& AIset = i->second;

		TCHAR szName[MAX_PATH];
		wsprintf(szName,"%d %s",dwID,AIset.szName.c_str());

		int nIndex = m_List.AddString(szName);
		m_List.SetItemData(nIndex,dwID);

		i++;
	}
}

BOOL KSceneEditorAISetDialog::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  Add extra initialization here
	FillList();

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

void KSceneEditorAISetDialog::OnLbnSelchangeList1()
{
	int nIndex = m_List.GetCurSel();
	if(nIndex!=-1)
	{
		m_dwSelectedID = (DWORD)m_List.GetItemData(nIndex);

		if(m_dwSelectedID)
		{
			KSceneEditorDocLogical::AIParameterSet aiset;
			m_pDocLogical->GetAISet(m_dwSelectedID,aiset);

			m_szName.Format("%s",aiset.szName.c_str());
			UpdateData(FALSE);
		}
	}
}

void KSceneEditorAISetDialog::OnBnClickedButtonClearselection()
{
	m_dwSelectedID = 0;
}

void KSceneEditorAISetDialog::OnEnChangeEditName()
{
	UpdateData(TRUE);

	if(!m_pDocLogical)
		return;

	if(m_dwSelectedID)
	{
		KSceneEditorDocLogical::AIParameterSet aiset;
		m_pDocLogical->GetAISet(m_dwSelectedID,aiset);

		aiset.szName = m_szName;

		m_pDocLogical->SetAISet(m_dwSelectedID,aiset);

		FillList();
	}
}

void KSceneEditorAISetDialog::OnBnClickedButtonNewtemplete()
{
	if(!m_pDocLogical)
		return;
	if(!m_pCurrentNpc)
		return;

	m_pCurrentNpc->dwNpcAISet = 0;
	m_pDocLogical->UpdataOneNpcAIParameters(m_pCurrentNpc,FALSE);

	/*DWORD dwID;
	m_pDocLogical->NewOneAISet(dwID,"NpcTempeleteDefauleValue");

	KSceneEditorDocLogical::AIParameterSet aiset;
	m_pDocLogical->GetAISet(m_dwSelectedID,aiset);

	m_pDocLogical->GetNpcDefauleAIParameterSetByTempleteID(m_dwCurrentNpcTempleteID,aiset);

	m_pDocLogical->SetAISet(m_dwSelectedID,aiset);

	m_dwSelectedID = dwID;*/
	FillList();

}
