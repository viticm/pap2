#include "stdafx.h"
#include "KG3DAnimationContainer.h"
#include "IEKG3DAnimationTable.h"
#include "IEKG3DClip.h"
#include "IEKG3DClipTable.h"
#include "Engine/KGLog.h"
//#include "SceneEditorCommon.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DAnimationContainer::KG3DAnimationContainer(void)
{
}

KG3DAnimationContainer::~KG3DAnimationContainer(void)
{
	CleanUp();
}


HRESULT KG3DAnimationContainer::ReleaseAnimation(unsigned int uIndex)
{
	HRESULT hr = E_FAIL;
	std::vector<IEKG3DClip*>::iterator i = m_Clips.begin();
	KG_PROCESS_ERROR(uIndex >= m_Clips.size());

	advance(i, uIndex);
	SAFE_RELEASE(m_Clips[uIndex]);
	m_Clips.erase(i);
Exit0:
	return hr;
}

HRESULT KG3DAnimationContainer::ReleaseAnimation(IEKG3DClip* pClip)
{
	HRESULT hr = E_FAIL;
	std::vector<IEKG3DClip*>::iterator i = m_Clips.begin();

	KG_PROCESS_ERROR(pClip);

	while (i != m_Clips.end())
	{
		if (*i == pClip)
		{
			SAFE_RELEASE(*i);
			m_Clips.erase(i);
			break;
		}
		i++;
	}
Exit0:
	return hr;

}

void KG3DAnimationContainer::CleanUp()
{
	for(size_t i = 0; i < m_Clips.size(); i++)
	{
		SAFE_RELEASE(m_Clips[i]);
	}
	m_Clips.clear();
}

HRESULT KG3DAnimationContainer::LoadAnimationFromFile(const TCHAR* strFileName, DWORD dwOption)
{
	HRESULT hr = E_FAIL;
	//IEKG3DAnimation* pAnimation = NULL;
    IEKG3DClipTable *pClipTable = NULL;
	IEKG3DClip* pClip = NULL;

    IEKG3DEngineManager *pManager = g_GetEngineIEInterface();

    hr = pManager->GetIEKG3DClipTable(&pClipTable);
    KGLOG_COM_PROCESS_ERROR(hr);

	hr = pClipTable->LoadResourceFromFileInEditor(strFileName, 0, dwOption, (IEKG3DClip **)(&pClip));
	KGLOG_COM_PROCESS_ERROR(hr);
	m_Clips.push_back(pClip);
Exit0:
	return hr;
}

void KG3DAnimationContainer::FillAnimationList(CListBox& ListBox, enuFillListBoxType Type)
{
	ListBox.ResetContent();
	LPCTSTR pszName = NULL;
	for(int i = 0; i < static_cast<int>(m_Clips.size()); i++)
	{

		switch(Type)
		{
		case ENUM_FILL_WITH_ANINAME:
			{
                m_Clips[i]->GetName(&pszName);
				ListBox.InsertString(i, pszName);
				ListBox.SetItemData(i, reinterpret_cast<DWORD_PTR>(m_Clips[i]));
				break;
			}
		case ENUM_FILL_WITH_FILENAME:
			{
                m_Clips[i]->GetPathName(&pszName);
				ListBox.InsertString(i, pszName);
				ListBox.SetItemData(i, reinterpret_cast<DWORD_PTR>(m_Clips[i]));
				break;
			}
		}
	}
}

void KG3DAnimationContainer::FillAnimationComboBox(CComboBox& ComboBox)
{
	ComboBox.ResetContent();
    LPCTSTR pszAniName = NULL;
	for (int i = 0; i < static_cast<int>(m_Clips.size()); i++)
	{
        m_Clips[i]->GetName(&pszAniName);
		ComboBox.InsertString(i, pszAniName);
		ComboBox.SetItemData(i, reinterpret_cast<DWORD_PTR>(m_Clips[i]));
	}
}


HRESULT KG3DAnimationContainer::GetAnimation(UINT uIndex, IEKG3DClip** ppClip)
{
    HRESULT hr = E_FAIL;
    KG_PROCESS_ERROR(uIndex < m_Clips.size());
    KG_PROCESS_ERROR(ppClip);
    
    *ppClip = m_Clips[uIndex];
    hr = S_OK;
Exit0:
    return hr;
}