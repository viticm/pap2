#include "StdAfx.h"
#include "KG3DAnimationTagContainer.h"
#include "KG3DAnimationSFXTag.h"
#include "KG3DAnimationSoundTag.h"
#include "KG3DAnimationMotionTag.h"
#include "KG3DClipTable.h"
#include "KG3DClip.h"
#include "KG3DEngineManager.h"
#include "KG3DClipTools.h"
#include "kg3dmodelst.h"
#include "KG3DMultiThreadLoader.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

KG3DAnimationTagContainer::KG3DAnimationTagContainer()
{
    m_pModel = NULL;
    m_uMotionTagIndex = INDEX_INVALIDATE;
    m_pClip = NULL;
    m_bModifyFlag = FALSE;
	m_IsLoaded = FALSE;
	m_ulRefCount = 1;
	memset(m_szAnimationName, 0, MAX_PATH);

	m_bInMultiThreadLoadList = FALSE;
}

KG3DAnimationTagContainer::~KG3DAnimationTagContainer()
{
	Clear();
}

HRESULT KG3DAnimationTagContainer::FrameMove(IKG3DAnimationController* pController)
{
    HRESULT hr = E_FAIL;
    KG_PROCESS_ERROR(pController);

	if (g_bClient)
	{
		KG_PROCESS_ERROR(m_pClip && m_pClip->IsLoad());
	}

    KG3DAnimationController* p3dController = static_cast<KG3DAnimationController*>(pController);
    for (size_t i = 0; i < m_vecTags.size(); i++)
    {
        assert(m_vecTags[i].pTag);
		D3DXVECTOR3 vPos(.0f, 0.f, 0.f);
		D3DXVECTOR3 *pvPos = &vPos;
		KG3DModel *pModel = static_cast<KG3DModel*>(pController->GetModel());
		if (pModel)
		{
			vPos.x = pModel->m_matWorld._41;
			vPos.y = pModel->m_matWorld._42;
			vPos.z = pModel->m_matWorld._43;
			vPos /= 100.0f;
			pvPos = &vPos;
		}
        m_vecTags[i].pTag->Apply(p3dController, pvPos);
    }
    hr = S_OK;
Exit0:
    return hr;
}


IKG3DAnimationTag* KG3DAnimationTagContainer::GetNewInstance(enuTagType Type)
{
    IKG3DAnimationTag* pReturn = NULL;
    switch(Type)
    {
    case TAGTYPE_SOUND:
        {
            pReturn = new KG3DAnimationSoundTag;
        }
        break;
    case TAGTYPE_SFX:
        {
            pReturn = new KG3DAnimationSFXTag;
        }
        break;
    case TAGTYPE_MOTION:
        {
            pReturn = new KG3DAnimationMotionTag;
        }
        break;
    default:
        _ASSERTE(0);
    }
    KGLOG_PROCESS_ERROR(pReturn);

Exit0:
    return pReturn;
}


HRESULT KG3DAnimationTagContainer::_Load(LPCSTR strFileName)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    IFile* pFile = NULL;
    FileHeader Header;
    AnimationTagBlockHeader BlockHeader;
    unsigned long uRetCode = 0;
    IKG3DAnimationTag* pNewTag = NULL;
	Clear();

	KG_PROCESS_ERROR(strFileName);

    pFile = g_OpenFile(strFileName);
    KGLOG_PROCESS_ERROR(pFile);
    
    uRetCode = pFile->Read(&Header, sizeof(FileHeader));
    KGLOG_PROCESS_ERROR(uRetCode == sizeof(FileHeader));

    KG_PROCESS_ERROR(Header.dwMask == FileHeader::s_dwMask);

    strncpy(m_szAnimationName, Header.strAnimationFileName, _countof(m_szAnimationName) - 1);
    m_szAnimationName[_countof(m_szAnimationName) - 1] = '\0';

    hrRetCode = g_cClipTable.LoadResourceFromFile(m_szAnimationName, 0, 0, &m_pClip);
    KGLOG_COM_PROCESS_ERROR(hrRetCode);

    switch(Header.dwVersion)
    {
    case FileHeader::s_dwCurrentVersion:
        {
            for (DWORD i = 0; i < Header.dwNumBlock; i++)
            {
                Item NewItem;

                uRetCode = pFile->Read(&BlockHeader, sizeof(AnimationTagBlockHeader));
                KGLOG_PROCESS_ERROR(uRetCode == sizeof(AnimationTagBlockHeader));

                if (BlockHeader.dwNumKeyFrames == 0)
                {
                    hrRetCode = SkipBlock(&BlockHeader, pFile);
                    KGLOG_COM_PROCESS_ERROR(hrRetCode);

                    continue;
                }

                pNewTag = GetNewInstance(static_cast<enuTagType>(BlockHeader.dwType));
                KG_PROCESS_ERROR(pNewTag);

                hrRetCode = pNewTag->LoadFromFile(pFile, BlockHeader.dwVersion, BlockHeader.dwNumKeyFrames);
                KG_COM_PROCESS_ERROR(hrRetCode);

                pNewTag->SetParentModel(m_pModel);
                NewItem.pTag = pNewTag;
                NewItem.Type = static_cast<enuTagType>(BlockHeader.dwType);

                m_vecTags.push_back(NewItem);
                pNewTag = NULL;
            }
        }
        break;
    default:
        _ASSERTE(0);
    }

    m_IsLoaded = TRUE;

    hrResult = S_OK;
Exit0:
    SAFE_DELETE(pNewTag);
    KG_COM_RELEASE(pFile);

    return hrResult;
}

HRESULT KG3DAnimationTagContainer::Load(LPCSTR strFileName)
{
	HRESULT hr = E_FAIL;
	DWORD dwStartTime = timeGetTime();
	//DWORD dwLoadOption = g_bClient ? MLO_ANI_MULTI : 0;
	m_scFileName = strFileName;

	/*if(dwLoadOption == MLO_ANI_MULTI)
	{
		g_cMultiThreadLoader.TryMultiThreadLoad_Tag(this);
		hr = S_OK;
	}
	else*/
	{
		hr = _Load(strFileName);
	}

	DWORD dwCost = timeGetTime() - dwStartTime;
	if(g_cEngineOption.bEnableTimeOptimizeLog && dwCost > 10)
	{
		KGLogPrintf(KGLOG_INFO,"TimeOptimize %d KG3DAnimationTagContainer::Load cost %s", dwCost,strFileName);
	}
	return hr;
}

HRESULT KG3DAnimationTagContainer::Save(LPCSTR strFileName)
{
	//Safely save to file backup before overwrite the old file and delete the back after save successfully.
	HRESULT hResult     = E_FAIL;
	HRESULT hRetCode = E_FAIL;

	TCHAR strBackupFileName[MAX_PATH];
	BOOL bSucceeded = TRUE;
	BOOL bNeedBackup = PathFileExists(strFileName);
	BOOL bExists = bNeedBackup;
	DWORD dwIndex = 0;
	while (bExists)
	{
		sprintf_s(strBackupFileName, 
			MAX_PATH,
			"%s.bak%d", 
			strFileName, 
			dwIndex);

		bExists = PathFileExists(strBackupFileName);
		dwIndex++;
	}

	if (bNeedBackup)
	{
		bSucceeded = CopyFile(strFileName, strBackupFileName, TRUE);
		if (!bSucceeded)
		{
			KGLogPrintf(KGLOG_ERR, "Backup SFX file %s failed.", strFileName);
		}
	}

	hRetCode = _Save(strFileName);
	KG_COM_PROCESS_ERROR(hRetCode);


	hResult = S_OK;
Exit0:
	if (SUCCEEDED(hResult) && bSucceeded)
	{
		//Delete backup when succeed.
		DeleteFile(strBackupFileName);
	}
	return hResult;
}

HRESULT KG3DAnimationTagContainer::_Save(LPCSTR strFileName)
{
    HRESULT hRetCode = E_FAIL;
	HRESULT hr = E_FAIL;
    FILE *fpFile = NULL;
    size_t uRetCode = 0;
    BYTE* pDataBlock = NULL;
    FileHeader Header;
	DWORD dwMaxFrame = 10000;
	KG3DClip *pClip = NULL;
    KG_PROCESS_ERROR(strFileName);
    fpFile = fopen(strFileName, "wb");
    KG_PROCESS_ERROR(fpFile);
    
    Header.dwNumBlock = (DWORD)m_vecTags.size();
    hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, m_szAnimationName, Header.strAnimationFileName);
    KG_COM_PROCESS_ERROR(hr);

    uRetCode = fwrite(&Header, sizeof(FileHeader), 1, fpFile);
    KG_PROCESS_ERROR(uRetCode == 1);

	hr = g_cClipTable.LoadResourceFromFile(m_szAnimationName, 0, 0, &pClip);
	KG_COM_PROCESS_ERROR(hr);

	pClip->GetNumFrames(&dwMaxFrame);

    for (size_t i = 0; i < m_vecTags.size(); i++)
    {
        IEKG3DAnimationTag* pTag = dynamic_cast<IEKG3DAnimationTag*>(m_vecTags[i].pTag);
        if (pTag)
        {
            DWORD dwSize = 0;
            hr = pTag->SaveToMemory(&pDataBlock, &dwSize,dwMaxFrame);
            KG_COM_PROCESS_ERROR(hr);
            
            uRetCode = fwrite(pDataBlock, dwSize, 1, fpFile);
            KG_PROCESS_ERROR(uRetCode == 1);

            SAFE_DELETE_ARRAY(pDataBlock);
        }
    }
    m_bModifyFlag = FALSE;
    hRetCode = S_OK;
Exit0:
    if (fpFile)
    {
        fclose(fpFile);
        fpFile = NULL;
    }
    SAFE_DELETE_ARRAY(pDataBlock);
	SAFE_RELEASE(pClip);
    return hRetCode;
}

HRESULT KG3DAnimationTagContainer::Start(float fSpeed, 
										 int   nOffsetTime, 
										 DWORD dwType, 
										 enuAnimationControllerPriority Priority)
{
    HRESULT hr = E_FAIL;
    HRESULT hRetCode = E_FAIL;
    IKG3DAnimationController* pController = NULL;

    KG_PROCESS_ERROR(m_pModel);
    KG_PROCESS_ERROR(m_szAnimationName[0] != '\0');

	if(!m_pClip)
	{
		hr = g_cClipTable.LoadResourceFromFile(m_szAnimationName, 0, 0, &m_pClip);
		KG_COM_PROCESS_ERROR(hr);
	}
    switch(dwType)
    {
    case ENU_ANIMATIONPLAY_ADDCURRENT_ONCE:
    case ENU_ANIMATIONPLAY_ADDCURRENT:
        {
            KG3DAnimationMotionTag* pMotionTag = dynamic_cast<KG3DAnimationMotionTag*>(GetTag(TAGTYPE_MOTION));
            if (pMotionTag)
            {
                UINT uStartFrame = pMotionTag->GetMotionStart();
                nOffsetTime = m_pClip->ConvertFrameToTime(uStartFrame);
            }
        }
        break;
    }
    hr = m_pModel->Helper_PlayAnimationNormal(dwType, 
		m_szAnimationName, 
		fSpeed, 
        nOffsetTime, 
		NULL, 
		Priority,
		MPAT_TAGGED,
		this,m_pClip);
    KGLOG_COM_PROCESS_ERROR(hr);

    pController = m_pModel->GetAnimationController(Priority);
    KG_PROCESS_ERROR(pController);

    pController->AddAnimationControllerUpdateNotifier(this);
    for (size_t i = 0; i < m_vecTags.size(); i++)
    {
        IKG3DAnimationTag* pTag = m_vecTags[i].pTag;
        if (pTag)
        {
            pTag->SetAnimationSpeed(fSpeed);
            pTag->Reset();
        }
    }
    hRetCode = S_OK;
Exit0:
    return hRetCode;
}


enuTagRunState KG3DAnimationTagContainer::GetState()
{
	enuTagRunState ContainerState = TAG_STATE_FINISHED;
	for (size_t i = 0; i < m_vecTags.size(); i++)
	{
		enuTagRunState TagState = m_vecTags[i].pTag->GetState();
		if (TagState == TAG_STATE_RUNNING)
		{
			ContainerState = TAG_STATE_RUNNING;
			break;
		}
	}
	return ContainerState;
}

void KG3DAnimationTagContainer::Clear()
{
	OnExit();

	SAFE_RELEASE(m_pClip);
    for (size_t i = 0; i < m_vecTags.size(); i++)
    {
        SAFE_DELETE(m_vecTags[i].pTag)
    }

	m_vecTags.clear();
    m_szAnimationName[0] = '\0';
    m_bModifyFlag = FALSE;
	m_IsLoaded = FALSE;
}

void KG3DAnimationTagContainer::OnExit()
{
    IKG3DAnimationController* pController = NULL;
    
    KG_PROCESS_ERROR(m_pModel);
    
	pController = m_pModel->GetAnimationController(ANICTL_PRIMARY);
    KG_PROCESS_ERROR(pController);

	pController->RemoveAnimationControllerUpdateNotifier(this);
  
    for (size_t i = 0; i < m_vecTags.size(); i++)
    {
        m_vecTags[i].pTag->OnMotionChange();
    }
Exit0:
    return;
}

IEKG3DAnimationTag* KG3DAnimationTagContainer::GetTag(enuTagType Type)
{
    IEKG3DAnimationTag *pTag = NULL;
    for (size_t i = 0; i < m_vecTags.size(); i++)
    {
        if (m_vecTags[i].Type == Type)
        {
            pTag = dynamic_cast<IEKG3DAnimationTag*>(m_vecTags[i].pTag);
            break;
        }
    }
    if (!pTag)
    {
        Item NewItem;
        NewItem.Type = Type;
        NewItem.pTag = GetNewInstance(Type);
        KGLOG_PROCESS_ERROR(NewItem.pTag);
        
        m_vecTags.push_back(NewItem);
        pTag = dynamic_cast<IEKG3DAnimationTag*>(NewItem.pTag);
        NewItem.pTag->SetParentModel(m_pModel);
        
    }
Exit0:
    return pTag;
}

HRESULT KG3DAnimationTagContainer::SetAnimation(LPCSTR strFileName)
{
    HRESULT hrResult = E_FAIL;

    KG_PROCESS_ERROR(strFileName);
    
    SAFE_RELEASE(m_pClip);

    strncpy(m_szAnimationName, strFileName, _countof(m_szAnimationName) - 1);
    m_szAnimationName[_countof(m_szAnimationName) - 1] = '\0';

    hrResult = S_OK;
Exit0:
    return hrResult;
}

KG3DAnimationTagContainer::FileHeader::FileHeader()
{
    dwMask = s_dwMask;
    dwVersion = s_dwCurrentVersion;
    *strAnimationFileName = '\0';
    dwNumBlock = 0;
}

LPCSTR KG3DAnimationTagContainer::GetAnimationFilePath()
{
    return m_szAnimationName;
}

BOOL KG3DAnimationTagContainer::GetModifyState()
{
    return m_bModifyFlag;
}

void KG3DAnimationTagContainer::SetModifyState()
{
    m_bModifyFlag = TRUE;
	m_IsLoaded = TRUE;
}


IKG3DModel* KG3DAnimationTagContainer::GetAttachedModel()
{
    return static_cast<IKG3DModel*>(m_pModel);
}

void KG3DAnimationTagContainer::Reset()
{
    for (size_t i = 0; i < m_vecTags.size(); i++)
    {
        if (m_vecTags[i].pTag)
        {
            m_vecTags[i].pTag->Reset();
        }
    }
}

HRESULT KG3DAnimationTagContainer::GetCurrenetAnimation(IEKG3DClip** ppClip)
{
    HRESULT hr = E_FAIL;
    KG_PROCESS_ERROR(ppClip);
    *ppClip = m_pClip;
    hr = S_OK;
Exit0:
    return hr;
}

HRESULT KG3DAnimationTagContainer::AttachToModel(IKG3DModel* pModel)
{ 
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;

    KGLOG_PROCESS_ERROR(pModel);

    m_pModel = static_cast<KG3DModel*>(pModel);

    if (m_pClip && m_pClip->IsLoad())
    {
        unsigned uType = m_pModel->GetType();
        switch (uType)
        {
        case MESHTYPE_DEFAULT:
            {
                hrRetCode = GetClipTool().CheckClipByMesh(m_pClip, m_pModel->GetMesh());
                KGLOG_COM_PROCESS_ERROR(hrRetCode);		
            }
            break;
        case MESHTYPE_MODELST:
            {
                KG3DModelST *pModelST = dynamic_cast<KG3DModelST*>(m_pModel);
                KGLOG_PROCESS_ERROR(pModelST);

                KG3DBip *pBip = pModelST->GetBip();
                hrRetCode = GetClipTool().CheckClipByBip(m_pClip, pBip);
                KGLOG_COM_PROCESS_ERROR(hrRetCode);
            }
            break;
        }
    }

	for (size_t i = 0; i < m_vecTags.size(); i++)
	{
		m_vecTags[i].pTag->SetParentModel(pModel);
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}

BOOL KG3DAnimationTagContainer::IsLoaded()
{
	if(m_pClip)
		return m_IsLoaded && m_pClip->IsLoad();
	else
		return m_IsLoaded;
}


HRESULT KG3DAnimationTagContainer::ExportTani(LPCSTR strFileName,DWORD dwMaxFrames)
{
	HRESULT hRetCode = E_FAIL;
	HRESULT hr = E_FAIL;
	FILE *fpFile = NULL;
	size_t uRetCode = 0;
	BYTE* pDataBlock = NULL;
	FileHeader Header;
	
	TCHAR path_buffer[256];
	TCHAR drive[256];
	TCHAR dir[256];
	TCHAR fname[256];
	TCHAR ext[256];
	TCHAR taniName[256];
	TCHAR aniName[256];

	long FilePos = 0;
	KG_PROCESS_ERROR(strFileName);

	wsprintf(path_buffer,"%s",strFileName);
	_splitpath( path_buffer, drive, dir, fname, ext );
	wsprintf(taniName,"%s%s%s.tani",drive,dir,fname);
	wsprintf(aniName,"%s%s%s.ani",drive,dir,fname);

	fpFile = fopen(taniName, "wb");
	KG_PROCESS_ERROR(fpFile);

	Header.dwNumBlock = (DWORD)m_vecTags.size();
	hr = g_pFileFinder->GetRelativePath(g_Def_WorkDirectory, aniName, Header.strAnimationFileName);
	KG_COM_PROCESS_ERROR(hr);
	
	FilePos = ftell(fpFile);
	uRetCode = fwrite(&Header, sizeof(FileHeader), 1, fpFile);
	KG_PROCESS_ERROR(uRetCode == 1);

	for (size_t i = 0; i < m_vecTags.size(); i++)
	{
		IEKG3DAnimationTag* pTag = dynamic_cast<IEKG3DAnimationTag*>(m_vecTags[i].pTag);
		if (pTag)
		{
			DWORD dwSize = 0;
			hr = pTag->SaveToMemory(&pDataBlock, &dwSize,dwMaxFrames);
			KG_COM_PROCESS_ERROR(hr);

			uRetCode = fwrite(pDataBlock, dwSize, 1, fpFile);
			KG_PROCESS_ERROR(uRetCode == 1);

			SAFE_DELETE_ARRAY(pDataBlock);
		}
	}
	m_bModifyFlag = FALSE;
	hRetCode = S_OK;
Exit0:
	if (fpFile)
	{
		fclose(fpFile);
		fpFile = NULL;
	}
	SAFE_DELETE_ARRAY(pDataBlock);
	return hRetCode;
}

HRESULT KG3DAnimationTagContainer::Clone(KG3DAnimationTagContainer* pSrc)
{
	HRESULT hr = E_FAIL;

	ASSERT(m_pClip==NULL);

    memcpy(m_szAnimationName, pSrc->m_szAnimationName, _countof(m_szAnimationName) - 1);
    m_szAnimationName[_countof(m_szAnimationName) - 1] = '\0';

	m_pClip            = pSrc->m_pClip;
	m_pModel           = pSrc->m_pModel;
	m_uMotionTagIndex  = pSrc->m_uMotionTagIndex;
	m_bModifyFlag      = pSrc->m_bModifyFlag;
	//m_IsLoaded         = pSrc->m_IsLoaded;

	if(m_pClip)
		m_pClip->AddRef();

	ASSERT(m_vecTags.size()==0);

	for (size_t i=0;i<pSrc->m_vecTags.size();i++)
	{
		Item& SrcItem = pSrc->m_vecTags[i];
		IKG3DAnimationTag* pNewTag = NULL;
		Item NewItem;

		pNewTag = GetNewInstance(SrcItem.Type);
		KG_PROCESS_ERROR(pNewTag);

		hr = pNewTag->Clone(SrcItem.pTag);
		KG_COM_PROCESS_ERROR(hr);

		pNewTag->SetParentModel(m_pModel);
		NewItem.pTag = pNewTag;
		NewItem.Type = SrcItem.Type;
		m_vecTags.push_back(NewItem);
	}

	m_IsLoaded = TRUE;
Exit0:
	return S_OK;
}


KG3DAnimationTagContainerTable g_tagConTable;

KG3DAnimationTagContainerTable::KG3DAnimationTagContainerTable()
{
}

KG3DAnimationTagContainerTable::~KG3DAnimationTagContainerTable()
{
	ASSERT(m_mapTagCache.empty());
}

HRESULT KG3DAnimationTagContainerTable::Init()
{
	return S_OK;
}

HRESULT KG3DAnimationTagContainerTable::UnInit()
{
	TAG_MAP::iterator it = m_mapTagCache.begin();
	TAG_MAP::iterator iend = m_mapTagCache.end();
	while (it!=iend)
	{
		KG3DAnimationTagContainer *pTag = it->second;
        VERIFY_RELEASE(pTag);
		++it;
	}
	m_mapTagCache.clear();

	return S_OK;
}

KG3DAnimationTagContainer* KG3DAnimationTagContainerTable::LoadTag(LPCSTR strFileName)
{
    HRESULT hrResult = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    DWORD dwID = 0;
    KG3DAnimationTagContainer* pHoldTag = NULL;
    KG3DAnimationTagContainer* pNewTag = NULL;
    TAG_MAP::iterator it;
    TAG_MAP::_Pairib Pair;

	pNewTag = new KG3DAnimationTagContainer();
	KGLOG_PROCESS_ERROR(pNewTag);

    dwID = g_FileNameHash(strFileName);

	it = m_mapTagCache.find(dwID);
	if (it == m_mapTagCache.end())
	{
		pHoldTag = new KG3DAnimationTagContainer();
		KGLOG_PROCESS_ERROR(pHoldTag);

		hrRetCode = pHoldTag->Load(strFileName);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);

        Pair = m_mapTagCache.insert(make_pair(dwID, pHoldTag));
        KGLOG_PROCESS_ERROR(Pair.second);

        Pair.first->second->AddRef();
	}
	else
	{
		pHoldTag = it->second;
        pHoldTag->AddRef();
	}

    if (pHoldTag->IsLoaded())
    {
        hrRetCode = pNewTag->Clone(pHoldTag);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }
    else
    {
        hrRetCode = pNewTag->Load(strFileName);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
    }

    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
        KG_COM_RELEASE(pNewTag);
    KG_COM_RELEASE(pHoldTag);
	return pNewTag;
}

ULONG KG3DAnimationTagContainer::AddRef()
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}

ULONG KG3DAnimationTagContainer::Release()
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
        delete this; 
    return uNewRefCount;
}

HRESULT KG3DAnimationTagContainer::LoadFromFileMultiThread()
{
	return _Load(m_scFileName.c_str());
}

DWORD KG3DAnimationTagContainer::GetSourceReference(TagSourceInfo *pBuffer, DWORD dwSize)
{
	DWORD dwResult = 0;
	DWORD dwOffset = 0;
	for (size_t i = 0; i < m_vecTags.size(); i++)
	{
		Item& CurItem = m_vecTags[i];
		IEKG3DAnimationTag *pTag = dynamic_cast<IEKG3DAnimationTag*>(CurItem.pTag);
		if (pTag)
		{
			DWORD dwCount = pTag->GetSourceReference(pBuffer + dwOffset, dwSize - dwOffset);
			KG_PROCESS_ERROR(dwCount != IEKG3DAnimationTag::INVALIDATESOURCEREFCOUNT);
			
			dwOffset += dwCount;
		}
	}
	
	dwResult = dwOffset;
Exit0:
	return dwResult;
}

HRESULT KG3DAnimationTagContainer::SkipBlock(AnimationTagBlockHeader *pBlockHeader, IFile *piFile)
{
    HRESULT hrResult = E_FAIL;
    unsigned long ulRetCode = 0;
    
    KG_ASSERT_EXIT(pBlockHeader);
    KG_ASSERT_EXIT(piFile);

    switch (pBlockHeader->dwType)
    {
    case TAGTYPE_MOTION:
        if (pBlockHeader->dwVersion == 1)
        {
            DWORD dwMotionTagFrameArr[2];
            ulRetCode = piFile->Read(dwMotionTagFrameArr, sizeof(DWORD) * 2);
            KG_PROCESS_ERROR(ulRetCode == sizeof(DWORD) * 2);
        }
        break;
    case TAGTYPE_SFX:
        break;
    case TAGTYPE_SOUND:
        break;
    default:
        KG_ASSERT_EXIT(false && "Invalid Tag Type");
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}