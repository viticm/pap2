#include "StdAfx.h"
#include "IEKG3DMaterial.h"
#include "KG3DMaterialSetter.h"
//#include "SceneEditorCommon.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DMaterialSetter::KG3DMaterialSetter(void)
{
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();
    pManager->GetNewIEKG3DMaterial(&m_pMaterial);
}

KG3DMaterialSetter::~KG3DMaterialSetter(void)
{
    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

	m_pMaterial->UnInit();

    pManager->DestroyIEKG3DMaterial(m_pMaterial);
}

void KG3DMaterialSetter::SetMaterial(const MaterialOption& Option)
{
    DWORD dwNumMaterial = 0;
    m_pMaterial->GetMaterialCount(&dwNumMaterial);
	for (DWORD i = 0; i < dwNumMaterial; i++)
	{
		SetMaterial(i, Option);
	}
}

void KG3DMaterialSetter::SetMaterial(unsigned int uIndex,
									 const MaterialOption& Option)
{
    DWORD dwNumMaterial = 0;
    DWORD dwOption = 0;
    m_pMaterial->GetMaterialCount(&dwNumMaterial);
	if ((DWORD)uIndex > dwNumMaterial)
		return;

	m_pMaterial->SetSubMaterialColorRef2Value(
        (DWORD)uIndex,
        Option.crColors[BUTTON_AMBIENT],
        Option.crColors[BUTTON_DIFFUSE],
        Option.crColors[BUTTON_EMISSIVE],
        Option.crColors[BUTTON_SPECULAR]
    );

	m_pMaterial->GetSubMaterialOption((DWORD)uIndex, &dwOption);

    if (Option.bEnableZ)
	{
		dwOption |= MATERIAL_OPTION_ZBUFFER_TRUE;
		dwOption &= (~MATERIAL_OPTION_ZBUFFER_FALSE);
	}
	else
	{
		dwOption |= MATERIAL_OPTION_ZBUFFER_FALSE;
		dwOption &= (~MATERIAL_OPTION_ZBUFFER_TRUE);
	}

	if (Option.bHide)
	{
		dwOption |= MATERIAL_OPTION_HIDE;
	}
	else
	{
		dwOption &= (~MATERIAL_OPTION_HIDE);
	}

	if (Option.bLighting)
	{
		dwOption |= MATERIAL_OPTION_LIGHTING;
	}
	else
	{
		dwOption &= (~MATERIAL_OPTION_LIGHTING);
	}

	if (Option.bHighLight)
	{
		dwOption |= MATERIAL_OPTION_SPECULARENABLE;
	}
	else
	{
		dwOption &= ~MATERIAL_OPTION_SPECULARENABLE;
	}

	if (Option.dwCullMode)
	{
		dwOption |= MATERIAL_OPTION_CULL_CW;
		dwOption &= ~MATERIAL_OPTION_CULL_NONE;
	}
	else
	{
		dwOption &= ~MATERIAL_OPTION_CULL_CW;
		dwOption |= MATERIAL_OPTION_CULL_NONE;
	}
    m_pMaterial->SetSubMaterialOption((DWORD)uIndex, dwOption);

	_MtlAlphaOperation* pAlphaOp = NULL;
    m_pMaterial->GetSubMaterialItemAlphaOp((DWORD)uIndex, &pAlphaOp);

	if (pAlphaOp)
	{
		memcpy(pAlphaOp, Option.pAlphaOperation, sizeof(_MtlAlphaOperation));
	}
}

void KG3DMaterialSetter::SaveToFile(BOOL bBackup,
									CListCtrl* pListBox)
{
	HRESULT hr = E_FAIL;

	TCHAR strFileName[MAX_PATH];
	TCHAR strFileNameBak[MAX_PATH];
	TCHAR strOutputInfo[MAX_PATH << 1];
    LPCTSTR pszName = NULL;
    m_pMaterial->GetName(&pszName);
	strcpy(strFileName, pszName);
	sprintf(strFileNameBak, "%s.bak", strFileName);
	if (bBackup)
	{
		BOOL bReturn = CopyFile(strFileName, strFileNameBak, FALSE);
		if (pListBox)
		{
			if (!bReturn)
			{
				sprintf(strOutputInfo, "保存备份文件%s失败.", strFileNameBak);
				pListBox->InsertItem(0, strOutputInfo);
			}
			else
			{
				sprintf(strOutputInfo, "保存备份文件%s成功.", strFileNameBak);
				pListBox->InsertItem(0, strOutputInfo);
			}
		}
	}
	hr = m_pMaterial->SaveToFile(strFileName, 0);
	if (SUCCEEDED(hr))
	{
		if (pListBox)
		{
			sprintf(strOutputInfo, "保存文件%s成功.", strFileName);
			//pListBox->InsertItem(0, NULL);//strOutputInfo);
			pListBox->InsertItem(0, strOutputInfo);
			int nNum = pListBox->GetItemCount();
			int i = 0;
		}
	}
	else
	{
		if (pListBox)
		{
			sprintf(strOutputInfo, "保存文件%s失败.", strFileName);
			pListBox->InsertItem(0, strOutputInfo);
		}
	}
}

HRESULT KG3DMaterialSetter::LoadMaterail(const TCHAR* strFileName)
{
	HRESULT hr = E_FAIL;
	if (!strFileName)
		return hr;

	m_pMaterial->UnInit();

	hr = m_pMaterial->LoadFromFile(strFileName, 0, 0);
	return hr;
}

void KG3DMaterialSetter::SetByOption(const TCHAR* strFileName,
									 const MaterialOption& Option,
									 BOOL bBackup,
									 CListCtrl* pListBox)
{
	HRESULT hr = E_FAIL;
	TCHAR strOutputInfo[512];
	hr = LoadMaterail(strFileName);
	if (FAILED(hr))
	{
		sprintf(strOutputInfo, "读取文件%s失败.", strFileName);
		pListBox->InsertItem(0, strOutputInfo);
		return;
	}
	/*else
	{
		pListBox->InsertString(0, strFileName);
	}*/

	SetMaterial(Option);

	SaveToFile(bBackup, pListBox);

}