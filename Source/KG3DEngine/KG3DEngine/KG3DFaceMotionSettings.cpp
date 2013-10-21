#include "StdAfx.h"
#include "KG3DFaceMotionSettings.h"
#include "kg3dmodel.h"
#include "kg3danimationmotiontag.h"
#include "KG3DGraphicsTool.h"

KG3DFaceMotionSettings   g_FaceMotionSettings;
KG3DDefaultFaceMotionMgr g_DefaultFaceMotionMgr;
static TCHAR *s_strConfigFileName = "\\data\\public\\TagEditor\\FaceMotionSettings.ini";
static size_t s_MaxIngnoreFileSize = 500;

KG3DFaceMotionSettings::KG3DFaceMotionSettings()
{

}

KG3DFaceMotionSettings::~KG3DFaceMotionSettings()
{

}


HRESULT KG3DFaceMotionSettings::Init()
{
	HRESULT hRetCode = E_FAIL;
	IIniFile *pFile = NULL;
	m_FaceMotion.clear();
	m_DefaultMotion.clear();
	int nNumFaceMotion = 0;
	int nNumDefaultMotion = 0;
	
	pFile = g_OpenIniFile(s_strConfigFileName);
	KG_PROCESS_ERROR(pFile);

	pFile->GetInteger("Header", "NumFaceMotion", 0, &nNumFaceMotion);
	for (int i = 0; i < nNumFaceMotion; i++)
	{
		TCHAR strSection[MAX_PATH];
		FaceMotionInfo Info;
		sprintf_s(strSection, 
			MAX_PATH,
			"Motion%d",
			i);
		pFile->GetString(strSection, "Name", "", Info.strName, MAX_PATH);
		pFile->GetString(strSection, "Suffix", "", Info.strSuffix, MAX_PATH);
		m_FaceMotion.push_back(Info);
	}

	pFile->GetInteger("Header", "NumDefault", 0, &nNumDefaultMotion);
	for (int i = 0; i < nNumDefaultMotion; i++)
	{
		TCHAR strDefaultType[MAX_PATH];
		TCHAR strDefaultRation[MAX_PATH];
		TCHAR strDefaultSpan[MAX_PATH];
		DefaultFaceMotion Info;
		sprintf_s(strDefaultType,
			MAX_PATH,
			"DefaultSuffix%d",
			i);
		sprintf_s(strDefaultRation,
			MAX_PATH,
			"DefaultRation%d",
			i);
		sprintf_s(strDefaultSpan,
			MAX_PATH,
			"DefaultSpan%d",
			i);

		pFile->GetString("DefaultMotion", strDefaultType, "", Info.strSuffix, MAX_PATH);
		pFile->GetFloat("DefaultMotion", strDefaultRation, 0.0f, &Info.fPlayRate);
		pFile->GetInteger("DefaultMotion", strDefaultSpan, 5000, &Info.nSpan);
		m_DefaultMotion.push_back(Info);
	}

	pFile->GetInteger("Header", "PathDepth", 0, &m_nPathDepth);
	pFile->GetString("Header", "FaceMotionModifyPath", "", m_strFaceMotionModifyPath, MAX_PATH);
	hRetCode = S_OK;
Exit0:
    KG_COM_RELEASE(pFile);
	return hRetCode;
}

DWORD KG3DFaceMotionSettings::GetFaceMotionType(LPCSTR strTypeName)
{
	DWORD dwReturn = s_dwInvalidateType;
	KG_PROCESS_ERROR(strTypeName);

	for (DWORD i = 0; i < static_cast<DWORD>(m_FaceMotion.size()); i++)
	{
		if (!strcmp(m_FaceMotion[i].strName, strTypeName))
		{
			dwReturn = i;
			break;
		}
	}
Exit0:
	return dwReturn;
}

HRESULT KG3DFaceMotionSettings::ApplyDefaultMotion(DefaultFaceMotionMgrItem &FaceMotionItem)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hResult  = E_FAIL;
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];
	TCHAR strMotionFilePath[MAX_PATH];
	DWORD dwMotionIndex = 0;
	KG_PROCESS_ERROR(FaceMotionItem.pModel);
	//如果正在播放标签触发的表情就跳过默认表情
	KG_PROCESS_SUCCESS(FaceMotionItem.pModel->GetAnimationController(ANICTL_PRIMARY)->GetEventHandler() == g_pLifeRangeHandler);

	KG_PROCESS_SUCCESS(!m_DefaultMotion.size());
	//dwMotionIndex = pModel->GetMotionTypeBySceneInfo();根据场景信息来获得需要的表情类型， 暂时没有
	dwMotionIndex = 0;
	DWORD dwCurTime = g_cGraphicsTool.GetNowTime();
	int nTimeSpan = static_cast<int>(dwCurTime - FaceMotionItem.dwLastCheck);
	KG_PROCESS_ERROR(nTimeSpan > m_DefaultMotion[dwMotionIndex].nSpan);
	
	//如果检查时间到了
	FaceMotionItem.dwLastCheck = dwCurTime;
	KG_PROCESS_ERROR(m_DefaultMotion[dwMotionIndex].fPlayRate > (rand() % 100));
	
	//通过了播放几率测试
	_splitpath_s(FaceMotionItem.pModel->m_scName.c_str(),
		strDriver,
		MAX_PATH,
		strPath,
		MAX_PATH,
		strFile,
		MAX_PATH,
		NULL,
		0);
	GetFaceMotionPathFromFacePath(strPath);
	sprintf_s(strMotionFilePath, 
		MAX_PATH,
		"%s%s%s%s%s.ani",
		strDriver,
		strPath,
		GetFaceMotionModifyPath(),
		strFile,
		m_DefaultMotion[dwMotionIndex].strSuffix);

	DWORD dwHash = g_FileNameHash(strMotionFilePath);

	if (m_IgnoreFile.find(dwHash) != m_IgnoreFile.end())
		goto Exit1;

    if (!g_IsFileExist(strMotionFilePath))
	{
		m_IgnoreFile.insert(dwHash);
		goto Exit1;
	}

	hResult = FaceMotionItem.pModel->PlayAnimation(ENU_ANIMATIONPLAY_ONCE, 
		strMotionFilePath,
		1.0f,
		0,
		NULL,
		NULL,
		ANICTL_PRIMARY);
    KG_COM_PROCESS_ERROR(hResult);
	
Exit1:
	SAFE_RELEASE(FaceMotionItem.pModel);
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

HRESULT KG3DFaceMotionSettings::GetFaceMotion(DWORD dwType,
											  LPCSTR strFaceMeshName,
											  LPSTR strFaceMotionAnimationName)
{
	HRESULT hRetCode = E_FAIL;
	TCHAR strDriver[MAX_PATH];
	TCHAR strPath[MAX_PATH];
	TCHAR strFile[MAX_PATH];

	KG_PROCESS_ERROR(strFaceMeshName);
	KG_PROCESS_ERROR(strFaceMotionAnimationName);
	KG_PROCESS_ERROR(dwType < m_FaceMotion.size());

	_splitpath_s(strFaceMeshName,
		strDriver,
		MAX_PATH,
		strPath,
		MAX_PATH,
		strFile,
		MAX_PATH,
		NULL,
		0);

	GetFaceMotionPathFromFacePath(strPath);
	sprintf_s(strFaceMotionAnimationName, 
		MAX_PATH,
		"%s%s%s%s%s.ani",
		strDriver,
		strPath,
		m_strFaceMotionModifyPath,
		strFile,
		m_FaceMotion[dwType].strSuffix);
	hRetCode = S_OK;
Exit0:
	return hRetCode;
}

LPCSTR KG3DFaceMotionSettings::GetFaceMotionTypeName(DWORD dwIndex)
{
	LPCSTR strReturn = NULL;
	KG_PROCESS_ERROR(dwIndex < m_FaceMotion.size());

	strReturn = m_FaceMotion[dwIndex].strName;
Exit0:
	return strReturn;
}

LPCSTR KG3DFaceMotionSettings::GetFaceMotionModifyPath()
{
	return m_strFaceMotionModifyPath;
}

LPCSTR KG3DFaceMotionSettings::GetFaceMotionSuffixName(DWORD dwIndex)
{
	LPCSTR strReturn = NULL;
	KG_PROCESS_ERROR(dwIndex < m_FaceMotion.size());

	strReturn = m_FaceMotion[dwIndex].strSuffix;
Exit0:
	return strReturn;
}

DWORD KG3DFaceMotionSettings::GetNumFaceMotion()
{
	return static_cast<DWORD>(m_FaceMotion.size()); 
}



KG3DDefaultFaceMotionMgr::KG3DDefaultFaceMotionMgr() :
	m_pCurrentFaceModelList(&m_FaceModels),
	m_pBackFaceModelList(&m_FaceModelsBack)
{

}

KG3DDefaultFaceMotionMgr::~KG3DDefaultFaceMotionMgr()
{
	
}

HRESULT KG3DDefaultFaceMotionMgr::UnInit()
{
	HRESULT hrResult = E_FAIL;
    size_t uSize = 0;

	KG_PROCESS_ERROR(m_pCurrentFaceModelList);

    uSize = m_pCurrentFaceModelList->size();
	for (size_t i = 0; i < uSize; i++)
	{
		SAFE_RELEASE((*m_pCurrentFaceModelList)[i].pModel);
	}

    m_pCurrentFaceModelList->clear();

    hrResult = S_OK;
Exit0:
	return hrResult;
}

void KG3DDefaultFaceMotionMgr::SetDefaultFaceMotion(KG3DModel *pModel)
{
	DefaultFaceMotionMgrItem Item;
	KG3DAnimationController *pController = NULL;
	KG_PROCESS_ERROR(pModel);
	
	pController = static_cast<KG3DAnimationController*>(pModel->GetAnimationController(ANICTL_PRIMARY));
	KG_PROCESS_ERROR(pController);
	
	if (pController->m_bOnFinish)
	{
		_ASSERTE(m_pCurrentFaceModelList);
		Item.pModel = pModel;
		Item.pModel->AddRef();
		Item.dwLastCheck = g_cGraphicsTool.GetNowTime() - pController->GetPlayTime();
		m_pCurrentFaceModelList->push_back(Item);
	}
	pModel->GetAnimationController(ANICTL_PRIMARY)->RegistereEventHandler(g_pFaceMotionDefaultHandler);
Exit0:
	;
}

void KG3DDefaultFaceMotionMgr::FrameMove()
{
	HRESULT hResult = E_FAIL;
	_ASSERTE(m_pBackFaceModelList);
	_ASSERTE(m_pCurrentFaceModelList);

	m_pBackFaceModelList->clear();

	g_FaceMotionSettings.RefreshIgnoreFile();

	for (size_t i = 0; i < m_pCurrentFaceModelList->size(); i++)
	{
		hResult = g_FaceMotionSettings.ApplyDefaultMotion((*m_pCurrentFaceModelList)[i]);
		if (FAILED(hResult))
		{
			//没有播放成功的放入到列表中下个循环继续检测
			m_pBackFaceModelList->push_back((*m_pCurrentFaceModelList)[i]);
		}
	}
	std::vector<DefaultFaceMotionMgrItem> *pTemp = m_pCurrentFaceModelList;
	m_pCurrentFaceModelList = m_pBackFaceModelList;
	m_pBackFaceModelList = pTemp;
}

void KG3DFaceMotionSettings::GetFaceMotionPathFromFacePath(LPSTR strFacePath)
{
	for (int i = 0; i < m_nPathDepth; i++)
	{
		TCHAR *pFind = strrchr(strFacePath, '\\');
		if (pFind)
		{
			*pFind = '\0';
		}
	}
}

void KG3DFaceMotionSettings::RefreshIgnoreFile()
{
	if (m_IgnoreFile.size() > 500)
	{
		m_IgnoreFile.clear();
	}
}