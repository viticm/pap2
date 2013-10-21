#include "StdAfx.h"
#include "KG3DRepresentObjectSet.h"
#include "kg3dgraphicstool.h"
#include "KG3DEngineManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

////////////////////////////////////////////////////////////////////////////////

KG3DRepresentObjectSet::KG3DRepresentObjectSet(void)
{
	m_ncurModel = 0;
}

KG3DRepresentObjectSet::~KG3DRepresentObjectSet(void)
{
	m_vecModelInfo.clear();
	KG3DRepresentObject::UnInit();
}

int KG3DRepresentObjectSet::GetNumModelofSet()
{
	return (int)m_vecModelInfo.size();
}

HRESULT KG3DRepresentObjectSet::AddModelInfo(int& nIndex,LPSTR pMeshName,LPSTR pMtlName,LPSTR pAniName)
{
	ModelInfo NewModel;
	NewModel.szMeshName = pMeshName;
	NewModel.szMaterialName = pMtlName;
	NewModel.szAnimationName = pAniName;
	m_vecModelInfo.push_back(NewModel);
	nIndex = (int)(m_vecModelInfo.size() - 1);
	return S_OK;
}

HRESULT KG3DRepresentObjectSet::DeleteModelInfo(int nIndex)
{
	if(nIndex<0)
		return E_FAIL;
	if(nIndex>=(int)m_vecModelInfo.size())
		return E_FAIL;

	vector<ModelInfo>::iterator it = m_vecModelInfo.begin();
	for(int s=0;s<(int)m_vecModelInfo.size();s++,it++)
	{
		if(s==nIndex)
		{
			m_vecModelInfo.erase(it);
			break;
		}
	}
	if(m_ncurModel>=(int)m_vecModelInfo.size())
		m_ncurModel = (int)m_vecModelInfo.size() -1;
	return S_OK;
}

HRESULT KG3DRepresentObjectSet::GetModelInfo(int nIndex,LPSTR pMeshName,LPSTR pMtlName,LPSTR pAniName)
{
	if(nIndex<0)
		return E_FAIL;
	if(nIndex>=(int)m_vecModelInfo.size())
		return E_FAIL;

	ModelInfo& NewModel = m_vecModelInfo[nIndex];
	wsprintf(pMeshName,"%s",NewModel.szMeshName.c_str());
	wsprintf(pMtlName ,"%s",NewModel.szMaterialName.c_str());
	wsprintf(pAniName ,"%s",NewModel.szAnimationName.c_str());

	return S_OK;
}

HRESULT KG3DRepresentObjectSet::SaveToIniFile(LPCTSTR pFileName,DWORD dwOption)
{
	IIniFile* pIniFile = NULL;//g_OpenIniFile(Name);

	IFile* pFile = g_CreateFile(pFileName);
	KG_COM_RELEASE(pFile);
	pIniFile = g_OpenIniFile(pFileName,false,true);
	if(!pIniFile)
		return E_FAIL;

	pIniFile->WriteInteger("MAIN","Count",(int)m_vecModelInfo.size());

	for(int i=0;i<(int)m_vecModelInfo.size();i++)
	{
		TCHAR SecName[256];
		wsprintf(SecName,"Model%d",i);

		TCHAR MeshName[256];
		TCHAR MtlName[256];
		TCHAR AniName[256];
		wsprintf(MeshName,"%s",m_vecModelInfo[i].szMeshName.c_str());
		wsprintf(MtlName,"%s" ,m_vecModelInfo[i].szMaterialName.c_str());
		wsprintf(AniName,"%s" ,m_vecModelInfo[i].szAnimationName.c_str());

		pIniFile->WriteString(SecName,"MeshName",MeshName);
		pIniFile->WriteString(SecName,"MtlName",MtlName);
		pIniFile->WriteString(SecName,"AniName",AniName);
	}

	pIniFile->Save(pFileName);
	pIniFile->Release();

	return S_OK;
}

HRESULT KG3DRepresentObjectSet::GetModelSetIniFileName(LPSTR pFileName)
{
	wsprintf(pFileName,"%s", GetRetPath(m_szSetFileName.c_str()));
	return S_OK;
}

HRESULT KG3DRepresentObjectSet::LoadFromIniFile(LPCTSTR pFileName,DWORD dwOption)
{
	IIniFile* pIniFile = NULL;//g_OpenIniFile(Name);

	if (NULL == pFileName)
	{
		return E_FAIL;
	}

	pIniFile = g_OpenIniFile(pFileName);

	if(!pIniFile)
	{
		KGLogPrintf(KGLOG_ERR,"KG3DRepresentObjectSet::LoadFromIniFile Failed %s",pFileName);
		return E_FAIL;
	}

	int nCount = 0;
	pIniFile->GetInteger("MAIN","Count",0,&nCount);

	for(int i=0;i<nCount;i++)
	{
		TCHAR SecName[256];
		wsprintf(SecName,"Model%d",i);

		TCHAR MeshName[256];
		TCHAR MtlName[256];
		TCHAR AniName[256];

		pIniFile->GetString(SecName,"MeshName","",MeshName,256);
		pIniFile->GetString(SecName,"MtlName","",MtlName,256);
		pIniFile->GetString(SecName,"AniName","",AniName,256);
		
		TCHAR strDrive[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		TCHAR strFile[MAX_PATH];
		TCHAR strExt[MAX_PATH];
		_splitpath(MeshName, strDrive, strPath, strFile, strExt);
		wsprintf(AniName,"%s%s%s.Ani",strDrive,strPath,strFile);
		if(!g_IsFileExist(AniName))
			wsprintf(AniName,"");
		int nIndex = 0;
		AddModelInfo(nIndex,MeshName,MtlName,AniName);

	}

	pIniFile->Release();

	m_szSetFileName = pFileName;

	return S_OK;
}

HRESULT KG3DRepresentObjectSet::SetCurrentModel(int nIndex,BOOL bForceLoad)
{
	HRESULT hr = E_FAIL;

	if(nIndex<0)
		return E_FAIL;
	if(nIndex>=(int)m_vecModelInfo.size())
		return E_FAIL;
	 ModelInfo& Info = m_vecModelInfo[nIndex];

	 m_ncurModel = nIndex;
	 hr = SetModel(0, Info.szMeshName.c_str(), Info.szMaterialName.c_str(), Info.szAnimationName.c_str(),bForceLoad);
	 KGLOG_COM_PROCESS_ERROR_CONSOLE(hr);

	 ComputeBBox();
	 return S_OK;
Exit0:
	 return E_FAIL;
}

int KG3DRepresentObjectSet::GetCurrentModel()
{
	return m_ncurModel;
}

HRESULT KG3DRepresentObjectSet::Copy(KG3DRepresentObject* pSrcObject)
{
	KG3DRepresentObjectSet* pSrcSet = static_cast<KG3DRepresentObjectSet*>(pSrcObject);
	if(!pSrcSet)
		return E_FAIL;

	m_szSetFileName = pSrcSet->m_szSetFileName;
	m_ncurModel = pSrcSet->m_ncurModel;
	m_vecModelInfo = pSrcSet->m_vecModelInfo;

	return KG3DRepresentObject::Copy(pSrcObject);
}

//(by dengzhihui 2006年9月28日 14:28:49

//////////////////////////////////////////////////////////////
// Function name   : KG3DRepresentObjectSet::ChangeModel
// Description     : 改变当前模型
// Return type     : HRESULT 
// Argument        : INT nChangeCode, 1=上一个模型 2=下一个模型 3=随机选择组内模型
// Author          : dengzhihui
//////////////////////////////////////////////////////////////
HRESULT KG3DRepresentObjectSet::ChangeModel(INT nChangeCode)
{
	//***这里调SetCurrentModel时不能用动态加载，不然后导至后面计算模型BBOX大小的值是错误的***//
	switch(nChangeCode)
	{
	case 1: //上一模型
		{
			int newindex = m_ncurModel - 1;
			if(newindex<0)
				newindex = (int)m_vecModelInfo.size() -1;
			SetCurrentModel(newindex,TRUE);
		}
		break;
	case 2: //下一模型
		{
			int newindex = m_ncurModel+1;
			newindex = newindex % (int)m_vecModelInfo.size();
			SetCurrentModel(newindex,TRUE);
		}
		break;
	case 3: //随机模型
		{
			int newindex = rand()%(int)m_vecModelInfo.size();
			SetCurrentModel(newindex,TRUE);
		}
		break;
	}
	return S_OK;
}


HRESULT KG3DRepresentObjectSet::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)//<PROCESS_MESSAGE>
{
	if(message==WM_KEYDOWN)
	{
		if(wParam==VK_PRIOR)
		{
			int newindex = m_ncurModel - 1;
			if(newindex<0)
				newindex = (int)m_vecModelInfo.size() -1;
			SetCurrentModel(newindex,FALSE);
		}
		else if(wParam==VK_NEXT)
		{
			int newindex = m_ncurModel+1;
			newindex = newindex % (int)m_vecModelInfo.size();
			SetCurrentModel(newindex,FALSE);
		}
		else if(wParam==VK_HOME)
		{
			int newindex = rand()%(int)m_vecModelInfo.size();
			SetCurrentModel(newindex,FALSE);
		}
	}
	return S_OK;
}

HRESULT KG3DRepresentObjectSet::RenderSelectMark()
{
	g_cGraphicsTool.DrawAABBox(&m_BBox,0xFFFFFF00);
	return S_OK;
}

HRESULT KG3DRepresentObjectSet::IELoadFromIniFile(LPCTSTR pFileName,DWORD dwOption)
{
    return LoadFromIniFile(pFileName, dwOption);
}

HRESULT KG3DRepresentObjectSet::GetIERepresentObject(IEKG3DRepresentObject** pObj)
{
	_ASSERTE(pObj);
	if (pObj)
	{
		*pObj = static_cast<IEKG3DRepresentObject*>(this);
		return S_OK;
	}
	return E_FAIL;
}

HRESULT KG3DRepresentObjectSet::SaveToInformationBuffer(/*DWORD& dwType,DWORD& dwLength,*/KG3DMemoryFile* pFile)
{	
	//pFile->Reset();
	//dwType = m_dwType;

	CHAR  SetName[MAX_PATH];
	ZeroMemory(SetName,sizeof(SetName));

	int   nCurrentModel = GetCurrentModel();
	DWORD dwVersion = 0;

	pFile->write(&nCurrentModel,sizeof(int));

	pFile->write(&dwVersion,sizeof(DWORD));

	GetModelSetIniFileName(SetName);
	pFile->write(SetName,sizeof(CHAR) * MAX_PATH);

	//dwLength = pFile->GetLength();
	return S_OK;
} 

HRESULT KG3DRepresentObjectSet::LoadFromInformationBuffer(/*DWORD dwType,DWORD dwLength,*/const KG3DMemoryFile* pFile,DWORD dwOption)
{
	//pFile->Reset();

	CHAR  SetName[MAX_PATH];
	int   nCurrentModel = 0;
	DWORD dwVersion = 0;

	BOOL bForceLoad = TRUE;
	if(dwOption & OBJLOAD_MULTITHEAD)
		bForceLoad = FALSE;

	pFile->read(&nCurrentModel,sizeof(int));

	pFile->read(&dwVersion,sizeof(DWORD));

	pFile->read(SetName,sizeof(CHAR) * MAX_PATH);

	LoadFromIniFile(SetName,0);
	
	SetCurrentModel(nCurrentModel,bForceLoad);

	ComputeBBox();
	//m_dwType = dwType;
	return S_OK;
}

HRESULT KG3DRepresentObjectSet::Load( LPCTSTR lpFileName, DWORD_PTR wParam, DWORD_PTR lParam )
{
	DWORD dwOption = _kg_to_smaller_size_cast<DWORD>(wParam);
	BOOL bForceLoad = TRUE;

	HRESULT hr = LoadFromIniFile(lpFileName, dwOption );
	KG_COM_PROCESS_ERROR(hr);

	if(dwOption & OBJLOAD_MULTITHEAD)
		bForceLoad = FALSE;
	hr = this->SetCurrentModel(0,bForceLoad);
	KG_COM_PROCESS_ERROR(hr);

    m_scName = lpFileName;

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DRepresentObjectSet::GetAnotherIEInterface( DWORD dwType, VOID** ppAnotherInterface )
{
	if (dwType == REPRESENTOBJECT_SET && NULL != ppAnotherInterface)
	{
		*ppAnotherInterface = reinterpret_cast<VOID*>(static_cast<IEKG3DRepresentObjectSet*>(this));
		return S_OK;
	}
	return E_FAIL;
}