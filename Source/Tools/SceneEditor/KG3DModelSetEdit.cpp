#include "stdafx.h"
#include "KG3DModelSetEdit.h"
#include "Engine/KGLog.h"
#include "IEEditor.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DModelSetEdit::KG3DModelSetEdit(void)
{
}

KG3DModelSetEdit::~KG3DModelSetEdit(void)
{
}


void KG3DModelSetEdit::FilltModelSetList(CComboBox& ListBox, IEKG3DModel* pModel)
{
	HRESULT hRetCode = E_FAIL;
    TCHAR strFileName[MAX_PATH];
	int nCount = 0;
	IKG3DModel* pSubModel = NULL;
    LPCTSTR pszName = NULL;

    KG_PROCESS_ERROR(pModel);

    ListBox.ResetContent();
   
	nCount = pModel->GetNumModel();
	
	int i = 0;
	for(i = 0; i < nCount; i++)
	{
        IEKG3DModel* pieSubModel = NULL;

		hRetCode = pModel->GetModel(i, &pSubModel);
		KGLOG_COM_PROCESS_ERROR(hRetCode);

		pieSubModel = dynamic_cast<IEKG3DModel*>(pSubModel);
		if (pieSubModel)
		{
			hRetCode = pieSubModel->GetName(&pszName);
			KGLOG_COM_PROCESS_ERROR(hRetCode);

			_splitpath(pszName, NULL, NULL, strFileName, NULL);
			int nIndex = ListBox.AddString(strFileName);
			ListBox.SetItemDataPtr(nIndex, pieSubModel);
		}
	}

	{
		IEKG3DModel* pieSubModel = NULL;
		hRetCode = pModel->GetFirstChildModel(&pieSubModel);
		while (pieSubModel)
		{
			hRetCode = pieSubModel->GetName(&pszName);
			KGLOG_COM_PROCESS_ERROR(hRetCode);

			_splitpath(pszName, NULL, NULL, strFileName, NULL);
			int nIndex = ListBox.AddString(strFileName);
			ListBox.SetItemDataPtr(nIndex, pieSubModel);

            pieSubModel = NULL;
            hRetCode = pModel->GetNextChildModel(&pieSubModel);
            KG_COM_PROCESS_ERROR(hRetCode);
		}
	}

Exit0:
	return;
}


void KG3DModelSetEdit::FilltModelSetList(CComboBox& ListBox,vector<IEKG3DModel*>*pvecModel)
{
	HRESULT hRetCode = E_FAIL;
	TCHAR strFileName[MAX_PATH];
	int nCount = 0;
	IKG3DModel* pSubModel = NULL;
	LPCTSTR pszName = NULL;

	KG_PROCESS_ERROR(pvecModel);

	ListBox.ResetContent();

	for(size_t i = 0; i < pvecModel->size(); i++)
	{
		IEKG3DModel* pieSubModel = (*pvecModel)[i];

		KG_PROCESS_ERROR(pieSubModel);

		hRetCode = pieSubModel->GetName(&pszName);
		KGLOG_COM_PROCESS_ERROR(hRetCode);

		_splitpath(pszName, NULL, NULL, strFileName, NULL);
		ListBox.InsertString(static_cast<int>(i), strFileName);
		ListBox.SetItemDataPtr(static_cast<int>(i), pieSubModel);
	}

Exit0:
	return;
}
void KG3DModelSetEdit::FillBoneList(CTreeCtrl& TreeCtrl, IEKG3DModel* piModelSet)
{

}
