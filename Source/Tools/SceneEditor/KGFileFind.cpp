#include "StdAfx.h"
#include "KGFileFind.h"
#include "KDlgLogicSaveProgress.h"
#include "KDlgAnimationTagBatchTool.h"
#include "KDlgResourceList.h"
#include "iekg3dsfx.h"

KGAnimationTagBatchTool g_AnimationTagBatchTool;

static const TCHAR *s_strType[] = {
	".tani", //TYPE_TANI
	".sfx",  //TYPE_SFX
	".wav",  //TYPE_WAV
};


#define TAGTYPE_INVALIDATE 0xffffffff

static const DWORD s_Type2TagType[] = 
{
	TAGTYPE_INVALIDATE, //TYPE_TANI
	TAGTYPE_SFX, //TYPE_SFX
	TAGTYPE_SOUND, //TYPE_WAV
};

static const TCHAR *s_Keywords[] = {".tani"};
static const TCHAR *s_Ingnores[] = {"svn" };
static const DWORD s_dwTANITYPE = TAGTYPE_MOTION + 1;
KGFileFind::KGFileFind(void)
{
}

KGFileFind::~KGFileFind(void)
{
}


HRESULT KGFileFind::Search(LPCSTR strStartPath, 
			LPCSTR *ppKeywords, 
			DWORD dwNumKeywords,
			LPCSTR *ppIngoreList,
			DWORD dwNumIgnoreList,
			BOOL bIncludeSubDir)
{
	HRESULT hResult  = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	KG_PROCESS_ERROR(strStartPath);
	KG_PROCESS_ERROR(ppKeywords);
	KG_PROCESS_ERROR(dwNumKeywords);

	m_strResult.clear();
	
	hRetCode = Helper_Search(strStartPath,
		ppKeywords,
		dwNumKeywords,
		ppIngoreList,
		dwNumIgnoreList,
		bIncludeSubDir);
	KG_COM_PROCESS_ERROR(hRetCode);
	
	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KGFileFind::Helper_Search(LPCSTR strStartPath, 
								  LPCSTR *ppKeywords, 
								  DWORD dwNumKeywords,
								  LPCSTR *ppIngoreList,
								  DWORD dwNumIgnoreList,
								  BOOL bIncludeSubDir)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	CFileFind FileFind;
	TCHAR strStringToFile[MAX_PATH];

	sprintf_s(strStringToFile, 
		MAX_PATH,
		"%s\\*.*",
		strStartPath);
	BOOL bFind = FileFind.FindFile(strStringToFile);
	while (bFind)
	{
		BOOL bIgnore = FALSE;
		bFind = FileFind.FindNextFile();
		CString strFileName = FileFind.GetFileName();
		CString strFileNameLower = strFileName;
		strFileNameLower.MakeLower();
		for (DWORD i = 0; i < dwNumIgnoreList; i++)
		{
			if (ppIngoreList[i])
			{
				if (strstr(strFileNameLower, ppIngoreList[i]))
				{
					bIgnore = TRUE;
					break;
				}
			}
		}

		if (bIgnore)
			continue;

		if (FileFind.IsDirectory() && bIncludeSubDir)
		{
			if (strFileNameLower == "." ||
				strFileNameLower == "..")
				continue;
			sprintf_s(strStringToFile,
				MAX_PATH,
				"%s\\%s",
				strStartPath,
				strFileName);
			hRetCode = Helper_Search(strStringToFile,
				ppKeywords,
				dwNumKeywords,
				ppIngoreList,
				dwNumIgnoreList,
				bIncludeSubDir);	
			KG_COM_PROCESS_ERROR(hRetCode);
		}
		else
		{
			for (DWORD i = 0; i < dwNumKeywords; i++)
			{
				if (ppKeywords[i])
				{
					if (strstr(strFileNameLower, ppKeywords[i]))
					{
						TCHAR strFilePath[MAX_PATH];
						sprintf_s(strFilePath,
							MAX_PATH,
							"%s\\%s",
							strStartPath,
							strFileName);
						m_strResult.push_back(strFilePath);
					}
				}
			}
		}
	}
	hResult = S_OK;
Exit0:
	FileFind.Close();
	return hResult;
}

KGAnimationTagBatchTool::KGAnimationTagBatchTool()
{
	m_pProgressDlg = NULL;
}

KGAnimationTagBatchTool::~KGAnimationTagBatchTool()
{

}


void KGAnimationTagBatchTool::ProcessTagInfo(LPCSTR strFilePath, 
											 TagSourceInfo *pInfo, 
											 DWORD dwNumSource)
{
	DWORD dwHash = g_FileNameHash(strFilePath);
	SrcToDest S2D;
	IKG3DResourceBase *pResouce = NULL;
	S2D.strSrc = strFilePath;
	for (DWORD i = 0; i < sizeof(s_strType) / sizeof(const TCHAR *); i++)
	{
		if (strstr(strFilePath, s_strType[i]))
		{
			S2D.dwType = i;
			break;
		}
	}
	_ASSERTE(S2D.dwType != TYPE_COUNT);

	TCHAR strFile[MAX_PATH];
	TCHAR strExt[MAX_PATH];
	_splitpath_s(S2D.strSrc.c_str(),
		NULL,
		0,
		NULL,
		0,
		strFile,
		MAX_PATH,
		strExt,
		MAX_PATH);
	sprintf_s(S2D.strSrcName,
		MAX_PATH,
		"%s%s",
		strFile,
		strExt);
	S2D.vecDest.resize(dwNumSource);
	for (DWORD i = 0; i < dwNumSource; i++)
	{
		S2D.vecDest[i] = pInfo[i];
	}
	m_TagToSource[dwHash] = S2D;
	for (size_t j = 0; j < S2D.vecDest.size(); j++)
	{
		TagSourceInfo &Source = S2D.vecDest[j];
		DWORD dwSourceHash = g_FileNameHash(Source.strFilePath.c_str());
		SrcToDest &D2S = m_SourceToTag[dwSourceHash];
		D2S.strSrc = Source.strFilePath;
		_splitpath_s(D2S.strSrc.c_str(),
			NULL,
			0,
			NULL,
			0,
			strFile,
			MAX_PATH,
			strExt,
			MAX_PATH);
		sprintf_s(D2S.strSrcName,
			MAX_PATH,
			"%s%s",
			strFile,
			strExt);
		TagSourceInfo Dest;
		Dest.dwFrame = Source.dwFrame;
		Dest.dwIndex = Source.dwIndex;
		Dest.strFilePath = S2D.strSrc;
		strcpy_s(Dest.strFileName, MAX_PATH, S2D.strSrcName);
		Dest.dwHash = dwHash;
		Dest.dwType = s_dwTANITYPE;
		D2S.vecDest.push_back(Dest);

		if (Source.dwType == TAGTYPE_SFX && 
			!(m_ProcessedSFX.count(dwSourceHash)))
		{
			//需要处理特效使用的资源
			HRESULT hRetCode = E_FAIL;
			const static DWORD s_dwMaxTag = 100;
			IEKG3DSFX *pSfx = NULL;
			TagSourceInfo TagInfoSFX[s_dwMaxTag];
			m_ProcessedSFX.insert(dwSourceHash);
			hRetCode = g_GetModelManager()->LoadResourceFromFile(Source.strFilePath.c_str(),
				0,
				0,
				&pResouce);
			KG_COM_PROCESS_ERROR(hRetCode);

			pSfx = dynamic_cast<IEKG3DSFX*>(pResouce);
			KG_PROCESS_ERROR(pSfx);

			DWORD dwRetCode = pSfx->GetSourceReference(TagInfoSFX, s_dwMaxTag);
			if (dwRetCode)
			{
				ProcessTagInfo(Source.strFilePath.c_str(), 
					TagInfoSFX,
					dwRetCode);
			}
			SAFE_RELEASE(pResouce);
		}
	}
Exit0:
	SAFE_RELEASE(pResouce);
}

HRESULT KGAnimationTagBatchTool::Init(LPCSTR strTagPath, 
									  IEKG3DAnimationTagContainer *pTagContainer, 
									  BOOL bClear)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;
	KGFileFind FileFind;
	const static DWORD s_dwMaxTag = 100;
	TagSourceInfo TagInfo[s_dwMaxTag];
	TCHAR strOutputInfo[MAX_PATH << 1];
	if (bClear)
	{
		m_SourceToTag.clear();
		m_TagToSource.clear();
		m_ProcessedSFX.clear();
	}

	KG_PROCESS_ERROR(strTagPath);
	KG_PROCESS_ERROR(pTagContainer);

	if (m_pProgressDlg)
	{
		sprintf_s(strOutputInfo, 
			MAX_PATH,
			"%s搜索中...",
			strTagPath);
		m_pProgressDlg->m_staticModel.SetWindowText(strOutputInfo);
	}
	hRetCode = FileFind.Search(strTagPath,
		s_Keywords,
		sizeof(s_Keywords) / sizeof(const TCHAR*),
		s_Ingnores,
		sizeof(s_Ingnores) / sizeof(const TCHAR*),
		TRUE);
	KG_COM_PROCESS_ERROR(hRetCode);

	if (m_pProgressDlg)
	{
		sprintf_s(strOutputInfo, 
			MAX_PATH,
			"%s分析中...",
			strTagPath);
		m_pProgressDlg->m_staticModel.SetWindowText(strOutputInfo);
	}

	for (size_t i = 0; i < FileFind.m_strResult.size(); i++)
	{
		if (m_pProgressDlg)
		{
			TCHAR strOutputInfo[MAX_PATH];
			sprintf_s(strOutputInfo,
				MAX_PATH,
				"%d/%d",
				i,
				FileFind.m_strResult.size());
			m_pProgressDlg->m_staticFace.SetWindowText(strOutputInfo);
			m_pProgressDlg->m_progressModel.SetPos(50 + static_cast<int>(i * 50.0f / FileFind.m_strResult.size()));
		}
		hResult = pTagContainer->Load(FileFind.m_strResult[i].c_str());
		if (FAILED(hResult))
		{
			KGLogPrintf(KGLOG_ERR, "TagBatchTool load tag %s failed.\n", FileFind.m_strResult[i].c_str());
			continue;
		}
		DWORD dwNumSource = pTagContainer->GetSourceReference(TagInfo, s_dwMaxTag);
		if (!dwNumSource)
			continue;

		ProcessTagInfo(FileFind.m_strResult[i].c_str(), 
			TagInfo,
			dwNumSource);
	}

	hResult = S_OK;
Exit0:
	return hResult;
}

std::vector<KGAnimationTagBatchTool::SrcToDest> KGAnimationTagBatchTool::GetUsedTagFile(std::vector<std::string>& strSrcFiles)
{
	std::vector<SrcToDest> vecReturn;
	for (size_t i = 0; i < strSrcFiles.size(); i++)
	{
		DWORD dwHash = g_FileNameHash(strSrcFiles[i].c_str());
		std::map<DWORD, SrcToDest>::iterator itFind = m_SourceToTag.find(dwHash);
		if (itFind != m_SourceToTag.end())
		{
			vecReturn.push_back(itFind->second);
		}
	}
	return vecReturn;
}

void KGAnimationTagBatchTool::FillListBox(CListBox *pListBox,
										  DWORD dwType)
{
	std::map<DWORD, SrcToDest>::iterator it, itEnd;
	KG_PROCESS_ERROR(pListBox);

	pListBox->ResetContent();

	std::map<DWORD, SrcToDest> *Lists[] = {&m_TagToSource, &m_SourceToTag};
	KG_PROCESS_ERROR(dwType < sizeof(Lists) / sizeof(std::map<DWORD, SrcToDest>*));
	
	std::map<DWORD, SrcToDest> *pList = Lists[dwType];
	_ASSERTE(pList);

	it = pList->begin();
	itEnd = pList->end();

	while (it != itEnd)
	{
		int nIndex = pListBox->AddString(it->second.strSrcName);
		pListBox->SetItemData(nIndex, it->first);
		it++;
	}

Exit0:
	;
}

HRESULT KGAnimationTagBatchTool::GetUsedTagFile(DWORD dwHash, SrcToDest &Info)
{
	HRESULT hResult = E_FAIL;
	std::map<DWORD, SrcToDest>::iterator itFind = m_SourceToTag.find(dwHash);
	KG_PROCESS_ERROR(itFind != m_SourceToTag.end());

	Info = itFind->second;
	
	hResult = S_OK;
Exit0:
	return hResult;
}


CString KGAnimationTagBatchTool::GetFullPath(DWORD dwType, DWORD dwHash)
{
	CString strReturn;
	std::map<DWORD, SrcToDest>::iterator itFind;
	std::map<DWORD, SrcToDest> *Lists[] = {&m_TagToSource, &m_SourceToTag};
	KG_PROCESS_ERROR(dwType < sizeof(Lists) / sizeof(std::map<DWORD, SrcToDest>*));
	
	std::map<DWORD, SrcToDest> *pList = Lists[dwType];
	_ASSERTE(pList);


	itFind = pList->find(dwHash);
	KG_PROCESS_ERROR(itFind != pList->end());

	strReturn = itFind->second.strSrc.c_str();
Exit0:
	return strReturn;
}

CString KGAnimationTagBatchTool::GetFileName(DWORD dwType, DWORD dwHash)
{
	CString strReturn;
	std::map<DWORD, SrcToDest>::iterator itFind;
	std::map<DWORD, SrcToDest> *Lists[] = {&m_TagToSource, &m_SourceToTag};
	KG_PROCESS_ERROR(dwType < sizeof(Lists) / sizeof(std::map<DWORD, SrcToDest>*));

	std::map<DWORD, SrcToDest> *pList = Lists[dwType];
	_ASSERTE(pList);


	itFind = pList->find(dwHash);
	KG_PROCESS_ERROR(itFind != pList->end());

	strReturn = itFind->second.strSrcName;
Exit0:
	return strReturn;
}

void KGAnimationTagBatchTool::OnNewMotionTag(long nFrame,IEKG3DAnimationTagContainer *pContainer )
{// add by chenganlai
	UINT uRetCode = IEIR_FAILED;
	UINT uID = ((nFrame << 16) | FRAMEINDEX_NEW);
	MotionTagDataInfo Data;
	TCHAR strOutputInfo[MAX_PATH];
	strcpy_s(Data.strTagName, TAG_NAME_MAX_LENGTH, "New Motion Tag");
	IEKG3DAnimationTag* pTag = pContainer->GetTag(TAGTYPE_MOTION);
	KG_PROCESS_ERROR(pTag);
	Data.Type = MTT_USERDEFINE;


	sprintf_s(Data.strTagName, 256, "User Define Tag @ %d", nFrame);
	uRetCode = pTag->ModifyTag(uID, static_cast<void*>(&Data));
Exit0:
	if (uRetCode == IEIR_FAILED)
	{
		sprintf_s(strOutputInfo, 
			MAX_PATH,
			"New frame @ %d failed.",
			nFrame);
	}
	else
	{
		pContainer->SetModifyState();
	}
}

HRESULT KGAnimationTagBatchTool::Get_Model_Editor(OUT IEKG3DSceneModelEditor** ppEditor) 
{// add by chenganlai
	HRESULT hResult = E_FAIL;
	CMDIFrameWnd *pMainFrame = (CMDIFrameWnd*)AfxGetMainWnd();
	KSceneModelEditorFrameWnd *pModelFrame = (KSceneModelEditorFrameWnd*)pMainFrame->GetActiveFrame();
	KSceneModelEditorDoc* pDoc = dynamic_cast<KSceneModelEditorDoc*>(pModelFrame->GetActiveDocument());
	KG_PROCESS_ERROR(pDoc);
	IEKG3DScene* pScene = pDoc->GetScene();
	KG_PROCESS_ERROR(pScene);
	IEKG3DSceneModelEditor* pEditor = NULL;
	pScene->GetSceneModelEditor(&pEditor); 
	KG_PROCESS_ERROR(pEditor);
	*ppEditor = pEditor;
	hResult = S_OK;
Exit0:
	return hResult;
}

HRESULT KGAnimationTagBatchTool::MergeMetro(MotionTagBlockMeteor *&pMetro,MotionTagDataRuntime *pMotionTagData,int *nIndex)
{// add by chenganlai
	HRESULT hResult = E_FAIL;
	HRESULT hRet = E_FAIL;
	*nIndex = -1;
	for (DWORD i = 0 ; i < pMotionTagData->Blocks.size(); i ++)
	{
		DWORD dwType = *(static_cast<DWORD*>(pMotionTagData->Blocks[i]));
		if(dwType == MTBT_METEOR)
		{
			*nIndex = i;
			// 合并 两个MotionTagBlockMeteor
			MotionTagBlockMeteor *pCurrent = static_cast<MotionTagBlockMeteor*>(pMotionTagData->Blocks[i]);
			for(int j = 0 ; j < pCurrent->dwNumFlag ; j ++)
			{
				BOOL bAdd = TRUE;
				for(int z = 0 ; z < pMetro->dwNumFlag ; z ++)
				{
					if(strcmp(pCurrent->strIndicator[j],pMetro->strIndicator[z]) == 0)
					{
						bAdd = FALSE;
						break;
					}
				}
				if(bAdd)
				{
					strcpy_s(pMetro->strIndicator[pMetro->dwNumFlag],pCurrent->strIndicator[j]);
					pMetro->dwFlags[pMetro->dwNumFlag] = pCurrent->dwFlags[j];
					pMetro->dwNumFlag ++; 			
				}
			}
			break;	

		}
	}
	if(*nIndex == -1)
		*nIndex = pMotionTagData->Blocks.size();
	hResult = S_OK;
Exit0:
	return hResult;
}
HRESULT KGAnimationTagBatchTool::ModifyMetor(IEKG3DAnimationTagContainer *pContainer, 
											 map<DWORD,MetorSelInfo> MetroSelect,
											 UINT uFrame)
{// add by chenganlai  
	HRESULT hResult = E_FAIL;
	HRESULT hRet = E_FAIL;
	IEKG3DAnimationTag *pTag = NULL;
	IEKG3DAnimationMotionTag *pMotionTag = NULL;
	MotionTagDataRuntime *pMotionTagData = NULL;
	void *pNewTagBlock = NULL;
	MotionTagBlockMeteor *pMeteor = NULL;
	map<DWORD ,MetorSelInfo>::iterator iter;
	DWORD dwCount = 0;
	DWORD dwID = 0;
	//IEKG3DSceneModelEditor* pEditor = NULL;
	//IEKG3DModel* pCurModel = NULL;
	int nIndex = 0;
	pTag = pContainer->GetTag(static_cast<enuTagType>(TAGTYPE_MOTION));
	KG_PROCESS_ERROR(pTag);
	pMotionTag = static_cast<IEKG3DAnimationMotionTag *>(pTag);
	KG_PROCESS_ERROR(pMotionTag);
	pNewTagBlock = pMotionTag->GetNewBlock(MTBT_METEOR);
	KG_PROCESS_ERROR(pNewTagBlock);
	pMeteor = static_cast<MotionTagBlockMeteor*>(pNewTagBlock);
	KG_PROCESS_ERROR(pMeteor);
	// hRet = Get_Model_Editor(&pEditor);
	//KG_COM_PROCESS_ERROR(hRet);
	// hRet = pEditor->GetCurModel(&pCurModel);
	// KG_COM_PROCESS_ERROR(hRet);
	for(iter = MetroSelect.begin(); iter != MetroSelect.end() ; iter ++)
	{
		MetorSelInfo &metorSelInfo = iter->second;
		pMeteor->dwFlags[pMeteor->dwNumFlag] = metorSelInfo.bIsMetorOn;
		strcpy_s(pMeteor->strIndicator[pMeteor->dwNumFlag],MAX_BONE_NAME_LENGTH,metorSelInfo.pSocketName);
		pMeteor->dwNumFlag ++;
	}
	pMotionTag->GetKeyFrame(uFrame,pMotionTagData);
	if(!pMotionTagData)
	{
		OnNewMotionTag(uFrame,pContainer);
		pMotionTag->GetKeyFrame(uFrame,pMotionTagData);
	}
	MergeMetro(pMeteor,pMotionTagData,&nIndex);
	dwID = GETID(uFrame,nIndex);
	pMotionTag->ModifyTag(dwID,static_cast<void*>(pMeteor));
	hResult = S_OK;
Exit0:
	return hResult;
}
HRESULT KGAnimationTagBatchTool::ApplyProcess(DWORD dwProcessType,
											  IEKG3DAnimationTagContainer *pContainer,
											  DWORD dwType,
											  DWORD dwHash, 
											  LPCSTR strOld, 
											  LPCSTR strNew, 
											  LPCSTR strSuffix,
											  void *pExtraInfo,
											  KDlgResourceList *pdlgResourceList)
{
	HRESULT hResult = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	BOOL bLoad = FALSE;
	IEKG3DSFX *pSfx = NULL;
	IKG3DResourceBase *pResource = NULL;

	std::map<DWORD, SrcToDest>::iterator itFind;
	KG_PROCESS_ERROR(strOld);
	KG_PROCESS_ERROR(strNew);
	KG_PROCESS_ERROR(pContainer);

	itFind = m_TagToSource.find(dwHash);
	KG_PROCESS_ERROR(itFind != m_TagToSource.end());

	if (dwProcessType == KDlgAnimationTagBatchTool::PROCESS_GENERATELIST)
	{
		//生成列表
		KG_PROCESS_ERROR(pdlgResourceList);
		HTREEITEM hRoot = pdlgResourceList->AddBrach(itFind->second.strSrc.c_str());

		for (size_t i = 0; i < itFind->second.vecDest.size(); i++)
		{
			TagSourceInfo &Info = itFind->second.vecDest[i];
			pdlgResourceList->AddLeaf(hRoot, 
				Info.strFilePath.c_str(),
				Info.dwFrame,
				Info.dwIndex,
				Info.dwType);
		}
	}
	else
	{
		BOOL bNewName = FALSE;
		TCHAR strNewName[MAX_PATH];
		TCHAR strDriver[MAX_PATH];
		TCHAR strPath[MAX_PATH];
		TCHAR strFile[MAX_PATH];
		TCHAR strExt[MAX_PATH];
		_splitpath_s(itFind->second.strSrc.c_str(),
			strDriver,
			MAX_PATH,
			strPath,
			MAX_PATH,
			strFile,
			MAX_PATH,
			strExt,
			MAX_PATH);
		sprintf_s(strNewName,
			MAX_PATH,
			"%s%s%s%s%s", 
			strDriver,
			strPath,
			strFile,
			strSuffix,
			strExt);
		DWORD dwNewNameHash = g_FileNameHash(strNewName);
		if (!m_TagToSource.count(dwNewNameHash))
		{
			bNewName = TRUE;
		}

		switch (dwType)
		{
		case TYPE_SFX:
			{
				switch (dwProcessType)
				{
				case KDlgAnimationTagBatchTool::PROCESS_ADD:
					{
						DWORD dwType = TYPE_COUNT;
						TCHAR strLower[MAX_PATH];
						IEKG3DAnimationTag* pTag = NULL;
						if (!bLoad)
						{
							hRetCode = g_GetModelManager()->LoadResourceFromFile(itFind->second.strSrc.c_str(),
								0,
								0,
								&pResource);
							KG_COM_PROCESS_ERROR(hRetCode);

							pSfx = dynamic_cast<IEKG3DSFX*>(pResource);
							KG_PROCESS_ERROR(pSfx);
							bLoad = TRUE;
						}

						//检查Type
						strcpy_s(strLower, MAX_PATH, strNew);
						_strlwr_s(strLower, MAX_PATH);
						for (size_t i = 0; i < sizeof(s_strType) / sizeof(const TCHAR*); i++)
						{
							if (strstr(strLower, s_strType[i]))
							{
								dwType = static_cast<DWORD>(i);
								break;
							}
						}

						if (dwType < TYPE_COUNT && dwType >= TYPE_SFX)
						{
							UINT uFrame = static_cast<UINT>(strtod(strOld, '\0'));
							//如果不是已知的类型, 不做任何处理，目前只能增加SFX和wav两种
							hRetCode = pSfx->AddResource(uFrame, strNew, pExtraInfo);
						}
					}
					break;
				case KDlgAnimationTagBatchTool::PROCESS_DELETEALLSOUND:
					{
						IEKG3DAnimationTag* pTag = NULL;
						if (!bLoad)
						{
							hRetCode = g_GetModelManager()->LoadResourceFromFile(itFind->second.strSrc.c_str(),
								0,
								0,
								&pResource);
							KG_COM_PROCESS_ERROR(hRetCode);

							pSfx = dynamic_cast<IEKG3DSFX*>(pResource);
							KG_PROCESS_ERROR(pSfx);
							bLoad = TRUE;
						}

						hRetCode = pSfx->DeleteAllSound();
					}
					break;
				default:
					{
						for (size_t i = 0; i < itFind->second.vecDest.size(); i++)
						{
							TagSourceInfo &Info = itFind->second.vecDest[i];
							if (!strcmpi(strOld, Info.strFilePath.c_str()))
							{
								if (!bLoad)
								{
									hRetCode = g_GetModelManager()->LoadResourceFromFile(itFind->second.strSrc.c_str(),
										0,
										0,
										&pResource);
									KG_COM_PROCESS_ERROR(hRetCode);

									pSfx = dynamic_cast<IEKG3DSFX*>(pResource);
									KG_PROCESS_ERROR(pSfx);
									bLoad = TRUE;
								}

								switch(dwProcessType)
								{
								case KDlgAnimationTagBatchTool::PROCESS_CHANGESOURCE:
									{
										hRetCode = pSfx->ChangeResource(Info.dwFrame, Info.dwIndex, strNew);
									}
									break;
								case KDlgAnimationTagBatchTool::PROCESS_DELETE:
									{
										hRetCode = pSfx->DeleteResource(Info.dwFrame, Info.dwIndex);
									}
									break;
								case KDlgAnimationTagBatchTool::PROCESS_CHANGEPLAYTYPE:
									hRetCode = pSfx->ChangePlayType(Info.dwFrame, Info.dwIndex, pExtraInfo);
									break;
								default:
									_ASSERTE(0);
								}
								KG_COM_PROCESS_ERROR(hRetCode);
							}
						}
					}
				}
				if (bLoad)
				{
					const static DWORD s_dwMaxTag = 100;
					TagSourceInfo TagInfo[s_dwMaxTag];

					hRetCode = pResource->SaveToFile(strNewName, 0);
					KG_COM_PROCESS_ERROR(hRetCode);

					if (!bNewName)
					{
						ClearTagInfo(dwNewNameHash);
					}

					DWORD dwNumSource = pSfx->GetSourceReference(TagInfo, s_dwMaxTag);
					ProcessTagInfo(strNewName, 
						TagInfo,
						dwNumSource);
				}
			}
			break;
		case TYPE_TANI:
			{
				switch(dwProcessType)
				{
				case KDlgAnimationTagBatchTool::PROCESS_SOUNDVERSION3:// add by chenganlai
					{
						bLoad = TRUE;
						UINT uId = 0;
						IEKG3DAnimationTag *pTag = NULL;
						IEKG3DAnimationSoundTag *pSoundTag = NULL;
						UINT uFrame = static_cast<UINT>(strtod(strOld, '\0'));
						hRetCode = pContainer->Load(itFind->second.strSrc.c_str());
						KG_COM_PROCESS_ERROR(hRetCode);
						SoundDataInfoVersion3 SoundData = *(static_cast<SoundDataInfoVersion3 *>(pExtraInfo));
						pTag = pContainer->GetTag(static_cast<enuTagType>(TAGTYPE_SOUND));
						KG_PROCESS_ERROR(pTag);
						pSoundTag = static_cast<IEKG3DAnimationSoundTag *>(pTag);
						KG_PROCESS_ERROR(pSoundTag);
						uId = GETID(uFrame,3);
						pSoundTag->ModifyTag(uId,static_cast<void*>(&SoundData));
					}
					break;
				case KDlgAnimationTagBatchTool::PROCESS_INSERTMETOR:// add by chenganlai
					{
						map<DWORD,MetorSelInfo> MetroSelect;
						UINT uFrame = static_cast<UINT>(strtod(strOld, '\0'));
						hRetCode = pContainer->Load(itFind->second.strSrc.c_str());
						KG_COM_PROCESS_ERROR(hRetCode);
						bLoad = TRUE;
						MetroSelect = *(static_cast<map<DWORD,MetorSelInfo> *>(pExtraInfo));

						hRetCode = ModifyMetor(pContainer,MetroSelect,uFrame);

						KG_COM_PROCESS_ERROR(hRetCode);
					}                    
					break;
				case KDlgAnimationTagBatchTool::PROCESS_ADD:
					{
						DWORD dwType = TYPE_COUNT;
						TCHAR strLower[MAX_PATH];
						IEKG3DAnimationTag* pTag = NULL;
						if (!bLoad)
						{
							hRetCode = pContainer->Load(itFind->second.strSrc.c_str());
							KG_COM_PROCESS_ERROR(hRetCode);

							bLoad = TRUE;
						}

						//检查Type
						strcpy_s(strLower, MAX_PATH, strNew);
						_strlwr_s(strLower, MAX_PATH);
						for (size_t i = 0; i < sizeof(s_strType) / sizeof(const TCHAR*); i++)
						{
							if (strstr(strLower, s_strType[i]))
							{
								dwType = static_cast<DWORD>(i);
								break;
							}
						}

						if (dwType < TYPE_COUNT && dwType >= TYPE_SFX)
						{
							UINT uFrame = static_cast<UINT>(strtod(strOld, '\0'));
							//如果不是已知的类型, 不做任何处理，目前只能增加SFX和wav两种
							pTag = pContainer->GetTag(static_cast<enuTagType>(s_Type2TagType[dwType]));
							KG_PROCESS_ERROR(pTag);

							pTag->AddResource(uFrame, strNew, pExtraInfo);
						}
					}
					break;
				case KDlgAnimationTagBatchTool::PROCESS_DELETEALLSOUND:
					{
						IEKG3DAnimationTag* pTag = NULL;
						if (!bLoad)
						{
							hRetCode = pContainer->Load(itFind->second.strSrc.c_str());
							KG_COM_PROCESS_ERROR(hRetCode);

							bLoad = TRUE;
						}

						//如果不是已知的类型, 不做任何处理，目前只能增加SFX和wav两种
						pTag = pContainer->GetTag(static_cast<enuTagType>(TAGTYPE_SOUND));
						KG_PROCESS_ERROR(pTag);

						pTag->DeleteAllSound();
					}
					break;
				default:

					{
						for (size_t i = 0; i < itFind->second.vecDest.size(); i++)
						{
							TagSourceInfo &Info = itFind->second.vecDest[i];
							if (!strcmpi(strOld, Info.strFilePath.c_str()))
							{
								if (!bLoad)
								{
									hRetCode = pContainer->Load(itFind->second.strSrc.c_str());
									KG_COM_PROCESS_ERROR(hRetCode);

									bLoad = TRUE;
								}

								IEKG3DAnimationTag* pTag = pContainer->GetTag(static_cast<enuTagType>(Info.dwType));
								KG_PROCESS_ERROR(pTag);

								switch(dwProcessType)
								{
								case KDlgAnimationTagBatchTool::PROCESS_CHANGESOURCE:
									{
										hRetCode = pTag->ChangeResource(Info.dwFrame, Info.dwIndex, strNew);
									}
									break;
								case KDlgAnimationTagBatchTool::PROCESS_DELETE:
									{
										hRetCode = pTag->DeleteResource(Info.dwFrame, Info.dwIndex);
									}
									break;
								case KDlgAnimationTagBatchTool::PROCESS_CHANGEPLAYTYPE:
									{
										hRetCode = pTag->ChangePlayType(Info.dwFrame, Info.dwIndex, pExtraInfo);
									}
									break;
								default:
									_ASSERTE(0);
								}
								KG_COM_PROCESS_ERROR(hRetCode);
							}
						}
					}
				}
				if (bLoad)
				{
					const static DWORD s_dwMaxTag = 100;
					TagSourceInfo TagInfo[s_dwMaxTag];

					hRetCode = pContainer->Save(strNewName);
					KG_COM_PROCESS_ERROR(hRetCode);

					if (!bNewName)
					{
						ClearTagInfo(dwNewNameHash);
					}

					DWORD dwNumSource = pContainer->GetSourceReference(TagInfo, s_dwMaxTag);
					ProcessTagInfo(strNewName, 
						TagInfo,
						dwNumSource);
				}
			}
			break;
		}
	}
	
	hResult = S_OK;
Exit0:
	SAFE_RELEASE(pResource);
	return hResult;
}

DWORD KGAnimationTagBatchTool::GetType(DWORD dwType, DWORD dwHash)
{
	DWORD dwReturn = 0;
	std::map<DWORD, SrcToDest>::iterator itFind;
	std::map<DWORD, SrcToDest> *Lists[] = {&m_TagToSource, &m_SourceToTag};
	KG_PROCESS_ERROR(dwType < sizeof(Lists) / sizeof(std::map<DWORD, SrcToDest>*));

	std::map<DWORD, SrcToDest> *pList = Lists[dwType];
	_ASSERTE(pList);


	itFind = pList->find(dwHash);
	KG_PROCESS_ERROR(itFind != pList->end());

	dwReturn = itFind->second.dwType;
Exit0:
	return dwReturn;
	
}

void KGAnimationTagBatchTool::ClearTagInfo(DWORD dwHash)
{
	std::map<DWORD, SrcToDest>::iterator it = m_TagToSource.find(dwHash);
	KG_PROCESS_ERROR(it != m_TagToSource.end());

	SrcToDest& S2D = it->second;
	for (size_t i = 0; i < S2D.vecDest.size(); i++)
	{
		TagSourceInfo &Info = S2D.vecDest[i];
		_ASSERTE(m_SourceToTag.count(Info.dwHash));

		SrcToDest &D2S = m_SourceToTag[Info.dwHash];
		std::vector<TagSourceInfo>::iterator it1 = D2S.vecDest.begin();
		while (it1 != D2S.vecDest.end())
		{
			if (it1->dwHash == dwHash &&
				it1->dwFrame == Info.dwFrame &&
				it1->dwIndex == Info.dwIndex)
			{
				it1 = D2S.vecDest.erase(it1);
			}
			else
			{
				it1++;
			}
		}

		if (!D2S.vecDest.size())
		{
			m_SourceToTag.erase(Info.dwHash);
		}
	}

	m_TagToSource.erase(it);
Exit0:
	;
}