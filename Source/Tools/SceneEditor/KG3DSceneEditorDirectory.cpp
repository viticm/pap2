#include "stdafx.h"
#include "KG3DSceneEditorDirectory.h"
//#include "kg3denginemanager.h"
#include "Engine.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////


KG3DSceneEditorDirectory g_cEditorDirectory;
extern TCHAR g_szDefWorkDirectory[256];

KG3DSceneEditorDirectory::KG3DSceneEditorDirectory(void)
{
}

KG3DSceneEditorDirectory::~KG3DSceneEditorDirectory(void)
{
}

HRESULT KG3DSceneEditorDirectory::AddDirectory(LPSTR pTitle,LPSTR pDirName)
{
	EntityDirectory dir;
	dir.name = pDirName;
	dir.title = pTitle;
	m_vecDirectorys.push_back(dir);
	return S_OK;
}

HRESULT KG3DSceneEditorDirectory::DeleteDirectory(int nIndex)
{
	if(nIndex<0)
		return E_FAIL;
	if(nIndex>=(int)m_vecDirectorys.size())
		return E_FAIL;

	int S = 0;
	for(vector<EntityDirectory>::iterator i=m_vecDirectorys.begin();i!=m_vecDirectorys.end();i++)
	{
		if(S==nIndex)
		{
			m_vecDirectorys.erase(i);
			return S_OK;
		}
		S++;
	}
	return S_OK;
}


HRESULT KG3DSceneEditorDirectory::GetDirectory(int nIndex,LPSTR pTitle,LPSTR pDirName)
{
	if(nIndex<0)
		return E_FAIL;
	if(nIndex>=(int)m_vecDirectorys.size())
		return E_FAIL;
	if(pDirName)
		wsprintf(pDirName,"%s",m_vecDirectorys[nIndex].name);
	if(pTitle)
		wsprintf(pTitle,"%s",m_vecDirectorys[nIndex].title);
	return S_OK;
}

int KG3DSceneEditorDirectory::GetNumofDirectory()
{
	return (int)m_vecDirectorys.size();
}



HRESULT KG3DSceneEditorDirectory::Save()
{
    HRESULT hrResult = E_FAIL;
    IIniFile *pIniFile = NULL;
	TCHAR szName[256];

	wsprintf(szName,"%sEditorDirectory.ini",g_szDefWorkDirectory);

    pIniFile = g_OpenIniFile(szName);
    KGLOG_PROCESS_ERROR(pIniFile);

	for(int i=0;i<(int)m_vecDirectorys.size();i++)
	{
		TCHAR SecName[256];
		wsprintf(SecName,"Directory%d",i);

		TCHAR Title[256];
		wsprintf(Title,"%s",m_vecDirectorys[i].title);
		TCHAR DirName[256];
		wsprintf(DirName,"%s",m_vecDirectorys[i].name);

		pIniFile->WriteString(SecName,"Title",Title);
		pIniFile->WriteString(SecName,"Name",DirName);
	}

	pIniFile->Save(szName);

    hrResult = S_OK;
Exit0:
	KG_COM_RELEASE(pIniFile);

	return hrResult;
}

HRESULT KG3DSceneEditorDirectory::Load()
{
    HRESULT hrResult = E_FAIL;
    IIniFile *pIniFile = NULL;
    TCHAR szName[256];

    wsprintf(szName,"%sEditorDirectory.ini",g_szDefWorkDirectory);

    pIniFile = g_OpenIniFile(szName);
    KGLOG_PROCESS_ERROR(pIniFile);

	BOOL bRun =  TRUE;
	int nIndex = 0;
	while(bRun)
	{
		TCHAR SecName[256];
		wsprintf(SecName,"Directory%d",nIndex);

		if(pIniFile->IsSectionExist(SecName))
		{
			TCHAR Title[256];
			TCHAR DirName[256];

			pIniFile->GetString(SecName,"Title","No Title",Title,256);
			pIniFile->GetString(SecName,"Name","",DirName,256);

			if(_strcmpi(DirName,"")!=0)
			{
				AddDirectory(Title, DirName);
			}
		}
		else
			bRun = FALSE;
		nIndex++;
	}

    hrResult = S_OK;
Exit0:
	KG_COM_RELEASE(pIniFile);

	return S_OK;
}
