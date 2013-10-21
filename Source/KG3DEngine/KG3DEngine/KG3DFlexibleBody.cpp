//////////////////////////////////////////////////////////////////////////////////
////
////  FileName    : KG3DFlexibleBody.cpp
////  Version     : 1.0
////  Creator     : Wu Caizhong
////  Create Date : 2008/12/24 15:12:14
////  Comment     : 
////
//////////////////////////////////////////////////////////////////////////////////
//
#include "StdAfx.h"
#include "KG3DFlexibleBody.h"
#include "KG3DModel.h"
#include "./PhysiscSystem/KG3DPhysiscScene.h"

////////////////////////////////////////////////////////////////////////////////
bool _IsFirstFlexiBoneName(LPCTSTR szName)
{
    if (_tcslen(szName) < 5)
        return false;

    if (
        (szName[0] != 'F' && szName[0] != 'f') ||
        (szName[1] != 'B' && szName[1] != 'b') ||
        (szName[2] != 'R' && szName[2] != 'r') 
    )
        return false;

    if ((szName[3] == '_') ||  (szName[4] == '_'))
    {
        return true;
    }

    return false;
}

bool _IsSubFlexiBoneName(LPCTSTR szName)
{
    if (_tcslen(szName) < 5)
        return false;

    if (
        (szName[0] != 'F' && szName[0] != 'f') ||
        (szName[1] != 'B' && szName[1] != 'b') 
    )
        return false;

    if ((szName[2] == '_') || (szName[3] == '_'))
    {
        return true;
    }

    return false;
}

////////////////////////////////////////////////////////////////////////////////
class KG3DFlexibleBody : public IKG3DFlexibleBody
{
public:
    // for IUnknown
    virtual ULONG STDMETHODCALLTYPE AddRef();
    virtual ULONG STDMETHODCALLTYPE Release();
    virtual HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject);

    // for IKG3DFlexibleBody
    virtual HRESULT AddToPhysicsScene(KG3DPhysiscScene *pPhysicsScene);
    virtual HRESULT RemoveFromPhysicsScene();
    virtual HRESULT Update();

public:
    KG3DFlexibleBody();
    ~KG3DFlexibleBody();
    
    // maybe return S_OK, E_FAIL, KG3D_E_NO_FLEXIBLE_BODY_DATA
    //HRESULT Init(KG3DModel *pModel);
	HRESULT InitFromMesh(KG3DMesh *pMesh);

    HRESULT UnInit();

	HRESULT CloneFromMesh(KG3DFlexibleBody* pDest,KG3DModel* pModel);

private:
    //HRESULT _AppendBoneOfChain(DWORD dwFirstBoneIndex);
	HRESULT _AppendBoneOfChainFromMesh(DWORD dwFirstBoneIndex);

private:
    ULONG m_ulRefCount;
    KG3DModel *m_pModel;
	KG3DMesh  *m_pMesh;
    HANDLE m_hPhysiceFlexibleBody;
    KG3DPhysiscScene *m_pPhysicsScene;

    ////////////////////////////////////////////////////////////////////////////
    struct FLEXIBLE_NORMAL_BONE_EX : public FLEXIBLE_NORMAL_BONE
    {
        DWORD       dwIndex;
    };
    std::vector<FLEXIBLE_NORMAL_BONE_EX> m_vecNormalBone;

    struct FLEXIBLE_DRIVER_BONE_EX : public FLEXIBLE_DRIVER_BONE
    {
        DWORD       dwIndex;
    };
    std::vector<FLEXIBLE_DRIVER_BONE_EX> m_vecDriverBone;
};


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
KG3DFlexibleBody::KG3DFlexibleBody() :
    m_ulRefCount(1),
    m_pModel(NULL),
    m_pMesh(NULL), 
	m_hPhysiceFlexibleBody(NULL),
    m_pPhysicsScene(NULL)
{
}

KG3DFlexibleBody::~KG3DFlexibleBody()
{
    ASSERT(!m_pModel);
    ASSERT(m_vecDriverBone.empty());
    ASSERT(!m_pPhysicsScene);
    ASSERT(!m_hPhysiceFlexibleBody);
}
////////////////////////////////////////////////////////////////////////////
//                 interface function                                     //
////////////////////////////////////////////////////////////////////////////
ULONG KG3DFlexibleBody::AddRef(void)
{
    return (ULONG)KG_InterlockedIncrement((long *)&m_ulRefCount);
}
ULONG KG3DFlexibleBody::Release(void)
{
    ULONG uNewRefCount = (ULONG)KG_InterlockedDecrement((long *)&m_ulRefCount);
    if (uNewRefCount == 0)
    {
        UnInit();
        delete this; 
    }
    return uNewRefCount;
}
HRESULT KG3DFlexibleBody::QueryInterface(REFIID riid, void **ppvObject)
{
    KG_USE_ARGUMENT(riid);
    KG_USE_ARGUMENT(ppvObject);
    return KG_E_FAIL;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DFlexibleBody::AddToPhysicsScene(KG3DPhysiscScene *pPhysicsScene)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    std::vector<FLEXIBLE_DRIVER_BONE *> vecDriverBonePtr;
    std::vector<FLEXIBLE_NORMAL_BONE *> vecNormalBonePtr;
    std::vector<FLEXIBLE_DRIVER_BONE_EX>::iterator itDriverBone;
    std::vector<FLEXIBLE_DRIVER_BONE_EX>::iterator itDriverBoneEnd;
    std::vector<FLEXIBLE_NORMAL_BONE_EX>::iterator itNormalBone;
    std::vector<FLEXIBLE_NORMAL_BONE_EX>::iterator itNormalBoneEnd;

    KG_ASSERT_EXIT(pPhysicsScene);
    KG_ASSERT_EXIT(!m_pPhysicsScene && "Already in physics scene");

    vecDriverBonePtr.reserve(m_vecDriverBone.size());
    itDriverBoneEnd = m_vecDriverBone.end();
    for (itDriverBone = m_vecDriverBone.begin(); itDriverBone != itDriverBoneEnd; ++itDriverBone)
    {
        vecDriverBonePtr.push_back(&(*itDriverBone));
    }

    vecNormalBonePtr.reserve(m_vecNormalBone.size());
    itNormalBoneEnd = m_vecNormalBone.end();
    for (itNormalBone = m_vecNormalBone.begin(); itNormalBone != itNormalBoneEnd; ++itNormalBone)
    {
        vecNormalBonePtr.push_back(&(*itNormalBone));
    }

    hrRetCode = pPhysicsScene->CreateFlexibleBody(
        (unsigned)vecDriverBonePtr.size(), &(vecDriverBonePtr[0]),
        (unsigned)vecNormalBonePtr.size(), &(vecNormalBonePtr[0]),
        &m_hPhysiceFlexibleBody        
    );
    KGLOG_COM_PROCESS_ERROR(hrRetCode);
    
    m_pPhysicsScene = pPhysicsScene;

    hrResult = S_OK;
Exit0:
    return hrResult;
}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DFlexibleBody::RemoveFromPhysicsScene()
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;

    KG_ASSERT_EXIT(m_pPhysicsScene && "Not in physics scene");
    
    if (m_hPhysiceFlexibleBody)
    {
        hrRetCode = m_pPhysicsScene->ReleaseFlexibleBody(m_hPhysiceFlexibleBody);
        KGLOG_COM_PROCESS_ERROR(hrRetCode);
        m_hPhysiceFlexibleBody = NULL;
    }

    m_pPhysicsScene = NULL;
    
    hrRetCode = 
    hrResult = S_OK;
Exit0:
    return hrResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DFlexibleBody::Update()
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    FLEXIBLE_DRIVER_BONE_EX *pDriverBoneEx = NULL;
    FLEXIBLE_NORMAL_BONE_EX *pNormalBoneEx = NULL;
    unsigned i = 0;
    unsigned uBoneCount = 0;

    KG_ASSERT_EXIT(m_pModel);
	if(m_pPhysicsScene)
	{

		// -------------------- input --------------------
		uBoneCount = (unsigned)m_vecDriverBone.size();
		ASSERT(uBoneCount > 0);
		pDriverBoneEx = &(m_vecDriverBone[0]);
		for (i = 0; i < uBoneCount; ++i)
		{
			hrRetCode = m_pModel->GetBoneMatrix(pDriverBoneEx->dwIndex, &(pDriverBoneEx->WorldPose));
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			++pDriverBoneEx;
		}

		hrRetCode = m_pPhysicsScene->UpdateFlexibleBody(m_hPhysiceFlexibleBody);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);

		// -------------------- output --------------------
		D3DXMATRIX matWorldInv;
		m_pModel->GetMatrixWorldInv(matWorldInv);

		uBoneCount = (unsigned)m_vecNormalBone.size();
		ASSERT(uBoneCount > 0);
		pNormalBoneEx = &(m_vecNormalBone[0]);
		for (i = 0; i < uBoneCount; ++i)
		{
			hrRetCode = m_pModel->SetBoneMatrix(pNormalBoneEx->dwIndex, &(pNormalBoneEx->WorldPose),matWorldInv);
			KGLOG_COM_PROCESS_ERROR(hrRetCode);

			++pNormalBoneEx;
		}
	}
	else
	{
		uBoneCount = (unsigned)m_vecDriverBone.size();
		ASSERT(uBoneCount > 0);
		pDriverBoneEx = &(m_vecDriverBone[0]);
		for (i = 0; i < uBoneCount; ++i)
		{
			m_pModel->UpdataBoneFamily(pDriverBoneEx->dwIndex);

			++pDriverBoneEx;
		}
	}

    hrResult = S_OK;
Exit0:
    return hrResult;
}

////////////////////////////////////////////////////////////////////////////////
//HRESULT KG3DFlexibleBody::Init(KG3DModel *pModel)
//{
//    HRESULT hrResult  = E_FAIL;
//    HRESULT hrRetCode = E_FAIL;
//    int     nRetCode = 0;
//    unsigned i = 0;
//    unsigned j = 0;
//    FLEXIBLE_DRIVER_BONE_EX  *pDriverBone = NULL;
//    FLEXIBLE_BONE_CHAIN BoneChain;
//    DWORD dwBoneCount = 0;
//    DWORD dwParentIndex = 0;
//    DWORD dwChildCount  = 0;
//    const DWORD *pdwChildIndexs = NULL;
//    const char *pcszBoneName = NULL;
//    
//    KGLOG_PROCESS_ERROR(pModel);
//    ASSERT(!m_pModel);
//    m_pModel = pModel;
//    //m_pModel->AddRef(); // don't add parent object's reference count, 
//                          // if do it, resource can't be release, reference to email from wucaizhong@kingsoft.com at 2009/02/21
//
//    hrRetCode = m_pModel->GetNumBones((int *)&dwBoneCount);
//    KGLOG_COM_PROCESS_ERROR(hrRetCode);
//
//    for (i = 0; i < dwBoneCount; ++i)
//    {
//        // find first bone of a flexible body
//        pcszBoneName = m_pModel->GetBoneName(i);
//        ASSERT(pcszBoneName);
//
//        nRetCode = _IsFirstFlexiBoneName(pcszBoneName);
//        if (!nRetCode)
//            continue;
//
//        hrRetCode = m_pModel->GetBoneFamily(i, &dwParentIndex, &dwChildCount, &pdwChildIndexs);
//        ASSERT(SUCCEEDED(hrRetCode));
//        if (dwParentIndex >= dwBoneCount)
//        {
//            KGLogPrintf(KGLOG_ERR, "[FlexibleBody] (dwParentIndex >= dwBoneCount) Bone %s Model:%s", pcszBoneName, m_pModel->m_scName.c_str());
//            KG_PROCESS_ERROR(false);
//        }
//
//        // found exist or new the first flexible bone's driver bone
//        for (j = 0; j < m_vecDriverBone.size(); ++j)
//        {
//            pDriverBone  = &(m_vecDriverBone[j]);
//            if (pDriverBone->dwIndex == dwParentIndex)
//            {
//                break;
//            }
//        }
//        if (j == m_vecDriverBone.size())
//        {
//            pcszBoneName = m_pModel->GetBoneName(dwParentIndex);
//            ASSERT(pcszBoneName);
//
//            //#ifdef _DEBUG
//                nRetCode = _IsFirstFlexiBoneName(pcszBoneName);
//                if (nRetCode)
//                {
//                    KGLogPrintf(KGLOG_ERR, "[FlexibleBody] Bone name error(%s), Model:%s", pcszBoneName, m_pModel->m_scName.c_str());
//                    //ASSERT(!nRetCode);
//                    KG_PROCESS_ERROR(false);
//                }
//                nRetCode = _IsSubFlexiBoneName(pcszBoneName);
//                if (nRetCode)
//                {
//                    KGLogPrintf(KGLOG_ERR, "[FlexibleBody] Bone name error(%s), Model:%s", pcszBoneName, m_pModel->m_scName.c_str());
//                    //ASSERT(!nRetCode);
//                    KG_PROCESS_ERROR(false);
//               }
//            //#endif
//
//            m_vecDriverBone.resize(j + 1);
//            pDriverBone  = &(m_vecDriverBone[j]);
//            
//            pDriverBone->pcszName = pcszBoneName;
//            hrRetCode = m_pModel->GetBoneMatrixOrg(dwParentIndex, &(pDriverBone->InitPose));
//            ASSERT(SUCCEEDED(hrRetCode));
//            pDriverBone->WorldPose = pDriverBone->InitPose;
//            pDriverBone->dwIndex   = dwParentIndex;
//        }
//        ASSERT(pDriverBone);
//
//        // build bone chain
//        BoneChain.uFirstBoneIndex = (unsigned)m_vecNormalBone.size();
//        
//        hrRetCode = _AppendBoneOfChain(i);
//        KGLOG_COM_PROCESS_ERROR(hrRetCode);
//        
//        BoneChain.uBoneCount = (unsigned)m_vecNormalBone.size() - BoneChain.uFirstBoneIndex;
//        pDriverBone->vecBoneChain.push_back(BoneChain);
//    }
//
//    KG_PROCESS_ERROR_RET_COM_CODE(m_vecDriverBone.size() > 0, KG3D_E_NO_FLEXIBLE_BODY_DATA);
//    ASSERT(m_vecNormalBone.size() > 0);
//
//    hrResult = S_OK;
//Exit0:
//    if (FAILED(hrResult))
//    {
//        m_vecNormalBone.clear();
//        m_vecDriverBone.clear();
//        //KG_COM_RELEASE(m_pModel);
//        m_pModel = NULL;
//    }
//    return hrResult;
//
//}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DFlexibleBody::InitFromMesh(KG3DMesh *pMesh)
{
	HRESULT hrResult  = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	int     nRetCode = 0;
	unsigned i = 0;
	unsigned j = 0;
	FLEXIBLE_DRIVER_BONE_EX  *pDriverBone = NULL;
	FLEXIBLE_BONE_CHAIN BoneChain;
	DWORD dwBoneCount = 0;
	DWORD dwParentIndex = 0;
	DWORD dwChildCount  = 0;
	const DWORD *pdwChildIndexs = NULL;
	const char *pcszBoneName = NULL;

	KGLOG_PROCESS_ERROR(pMesh);
	ASSERT(!m_pMesh);
	m_pMesh = pMesh;
	//m_pModel->AddRef(); // don't add parent object's reference count, 
	// if do it, resource can't be release, reference to email from wucaizhong@kingsoft.com at 2009/02/21

	hrRetCode = m_pMesh->GetNumBones((int *)&dwBoneCount);
	KGLOG_COM_PROCESS_ERROR(hrRetCode);

	for (i = 0; i < dwBoneCount; ++i)
	{
		// find first bone of a flexible body
		pcszBoneName = m_pMesh->GetBoneName(i);
		ASSERT(pcszBoneName);

		nRetCode = _IsFirstFlexiBoneName(pcszBoneName);
		if (!nRetCode)
			continue;

		hrRetCode = m_pMesh->GetBoneFamily(i, &dwParentIndex, &dwChildCount, &pdwChildIndexs);
		ASSERT(SUCCEEDED(hrRetCode));
		if (dwParentIndex >= dwBoneCount)
		{
			KGLogPrintf(KGLOG_ERR, "[FlexibleBody] (dwParentIndex >= dwBoneCount) Bone %s Model:%s", pcszBoneName, m_pModel->m_scName.c_str());
			KG_PROCESS_ERROR(false);
		}

		// found exist or new the first flexible bone's driver bone
		for (j = 0; j < m_vecDriverBone.size(); ++j)
		{
			pDriverBone  = &(m_vecDriverBone[j]);
			if (pDriverBone->dwIndex == dwParentIndex)
			{
				break;
			}
		}
		if (j == m_vecDriverBone.size())
		{
			pcszBoneName = m_pMesh->GetBoneName(dwParentIndex);
			ASSERT(pcszBoneName);

			//#ifdef _DEBUG
			nRetCode = _IsFirstFlexiBoneName(pcszBoneName);
			if (nRetCode)
			{
				KGLogPrintf(KGLOG_ERR, "[FlexibleBody] Bone name error(%s), Model:%s", pcszBoneName, m_pModel->m_scName.c_str());
				//ASSERT(!nRetCode);
				KG_PROCESS_ERROR(false);
			}
			nRetCode = _IsSubFlexiBoneName(pcszBoneName);
			if (nRetCode)
			{
				KGLogPrintf(KGLOG_ERR, "[FlexibleBody] Bone name error(%s), Model:%s", pcszBoneName, m_pModel->m_scName.c_str());
				//ASSERT(!nRetCode);
				KG_PROCESS_ERROR(false);
			}
			//#endif

			m_vecDriverBone.resize(j + 1);
			pDriverBone  = &(m_vecDriverBone[j]);

			pDriverBone->pcszName = pcszBoneName;
			pDriverBone->InitPose = *m_pMesh->GetBoneMatrix(dwParentIndex);

			ASSERT(SUCCEEDED(hrRetCode));
			pDriverBone->WorldPose = pDriverBone->InitPose;
			pDriverBone->dwIndex   = dwParentIndex;
		}
		ASSERT(pDriverBone);

		// build bone chain
		BoneChain.uFirstBoneIndex = (unsigned)m_vecNormalBone.size();

		hrRetCode = _AppendBoneOfChainFromMesh(i);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);

		BoneChain.uBoneCount = (unsigned)m_vecNormalBone.size() - BoneChain.uFirstBoneIndex;
		pDriverBone->vecBoneChain.push_back(BoneChain);
	}

	KG_PROCESS_ERROR_RET_COM_CODE(m_vecDriverBone.size() > 0, KG3D_E_NO_FLEXIBLE_BODY_DATA);
	ASSERT(m_vecNormalBone.size() > 0);

	hrResult = S_OK;
Exit0:
	if (FAILED(hrResult))
	{
		m_vecNormalBone.clear();
		m_vecDriverBone.clear();
		m_pMesh = NULL;
	}
	return hrResult;

}
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DFlexibleBody::UnInit()
{
    m_vecDriverBone.clear();
    m_vecNormalBone.clear();
    
    ASSERT(!m_pPhysicsScene);
    ASSERT(!m_hPhysiceFlexibleBody);

    //KG_COM_RELEASE(m_pModel);
    m_pModel = NULL;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
//HRESULT KG3DFlexibleBody::_AppendBoneOfChain(DWORD dwFirstBoneIndex)
//{
//    HRESULT hrResult = E_FAIL;
//    HRESULT hrRetCode = E_FAIL;
//    int nRetCode = false;
//    DWORD dwCurrentIndex = 0;
//    FLEXIBLE_NORMAL_BONE_EX *pCurrentBone = NULL;
//    DWORD dwParentIndex = 0;
//    DWORD dwChildCount  = 0;
//    const DWORD *pdwChildIndexs = NULL;
//    const char *pcszBoneName = NULL;
//
//    ASSERT(m_pModel);
//
//    dwCurrentIndex = dwFirstBoneIndex;
//    while (true)
//    {
//        pcszBoneName = m_pModel->GetBoneName(dwCurrentIndex);
//        KGLOG_PROCESS_ERROR(pcszBoneName);
//#ifdef _DEBUG
//        nRetCode = _IsFirstFlexiBoneName(pcszBoneName);
//        if (!nRetCode)
//        {
//            nRetCode = _IsSubFlexiBoneName(pcszBoneName);
//            ASSERT(nRetCode);
//        }
//#endif // _DEBUG
//
//        hrRetCode = m_pModel->GetBoneFamily(
//            dwCurrentIndex, &dwParentIndex, &dwChildCount, &pdwChildIndexs
//        );
//        KGLOG_COM_PROCESS_ERROR(hrRetCode);
//
//        m_vecNormalBone.resize(m_vecNormalBone.size() + 1);
//        pCurrentBone = &(m_vecNormalBone[m_vecNormalBone.size() - 1]);
//        pCurrentBone->pcszName = pcszBoneName;
//        hrRetCode = m_pModel->GetBoneMatrixOrg(dwCurrentIndex, &(pCurrentBone->InitPose));
//        ASSERT(SUCCEEDED(hrRetCode));
//        pCurrentBone->WorldPose = pCurrentBone->InitPose;
//        pCurrentBone->fLength = 10.0f;
//        pCurrentBone->dwIndex = dwCurrentIndex;
//
//        if (dwCurrentIndex != dwFirstBoneIndex)
//        {
//            FLEXIBLE_NORMAL_BONE_EX *pParentBone = pCurrentBone - 1; // do not use value of pCurrentBone in last loop, m_vecNormalBone.resize maybe move memory
//            D3DXVECTOR3 vCur   = D3DXVECTOR3(
//                pCurrentBone->InitPose._41, 
//                pCurrentBone->InitPose._42,
//                pCurrentBone->InitPose._43
//            );
//            D3DXVECTOR3 vParent = D3DXVECTOR3(
//                pParentBone->InitPose._41, 
//                pParentBone->InitPose._42, 
//                pParentBone->InitPose._43
//            );
//            
//            pParentBone->fLength = D3DXVec3Length(&(vParent - vCur));
//        }
//
//        if (dwChildCount == 0)
//            break;
//
//        ASSERT(dwChildCount == 1);
//        dwCurrentIndex = *pdwChildIndexs;
//    }
//
//    hrResult = S_OK;
//Exit0:
//    return hrResult;
//}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DFlexibleBody::_AppendBoneOfChainFromMesh(DWORD dwFirstBoneIndex)
{
	HRESULT hrResult = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	int nRetCode = false;
	DWORD dwCurrentIndex = 0;
	FLEXIBLE_NORMAL_BONE_EX *pCurrentBone = NULL;
	DWORD dwParentIndex = 0;
	DWORD dwChildCount  = 0;
	const DWORD *pdwChildIndexs = NULL;
	const char *pcszBoneName = NULL;

	ASSERT(m_pMesh);

	dwCurrentIndex = dwFirstBoneIndex;
	while (true)
	{
		pcszBoneName = m_pMesh->GetBoneName(dwCurrentIndex);
		KGLOG_PROCESS_ERROR(pcszBoneName);
#ifdef _DEBUG
		nRetCode = _IsFirstFlexiBoneName(pcszBoneName);
		if (!nRetCode)
		{
			nRetCode = _IsSubFlexiBoneName(pcszBoneName);
			ASSERT(nRetCode);
		}
#endif // _DEBUG

		hrRetCode = m_pMesh->GetBoneFamily(
			dwCurrentIndex, &dwParentIndex, &dwChildCount, &pdwChildIndexs
			);
		KGLOG_COM_PROCESS_ERROR(hrRetCode);

		m_vecNormalBone.resize(m_vecNormalBone.size() + 1);
		pCurrentBone = &(m_vecNormalBone[m_vecNormalBone.size() - 1]);
		pCurrentBone->pcszName = pcszBoneName;
		
		pCurrentBone->InitPose = *m_pMesh->GetBoneMatrix(dwCurrentIndex);

		pCurrentBone->WorldPose = pCurrentBone->InitPose;
		pCurrentBone->fLength = 10.0f;
		pCurrentBone->dwIndex = dwCurrentIndex;

		if (dwCurrentIndex != dwFirstBoneIndex)
		{
			FLEXIBLE_NORMAL_BONE_EX *pParentBone = pCurrentBone - 1; // do not use value of pCurrentBone in last loop, m_vecNormalBone.resize maybe move memory
			D3DXVECTOR3 vCur   = D3DXVECTOR3(
				pCurrentBone->InitPose._41, 
				pCurrentBone->InitPose._42,
				pCurrentBone->InitPose._43
				);
			D3DXVECTOR3 vParent = D3DXVECTOR3(
				pParentBone->InitPose._41, 
				pParentBone->InitPose._42, 
				pParentBone->InitPose._43
				);

			pParentBone->fLength = D3DXVec3Length(&(vParent - vCur));
		}

		if (dwChildCount == 0)
			break;

		ASSERT(dwChildCount == 1);
		dwCurrentIndex = *pdwChildIndexs;
	}

	hrResult = S_OK;
Exit0:
	return hrResult;
}


////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DCreateFlexibleBodyFromModel(
    KG3DModel *pModel, IKG3DFlexibleBody **ppiRetFlexibleBody
)
{
    HRESULT hrResult  = E_FAIL;
    HRESULT hrRetCode = E_FAIL;
    KG3DFlexibleBody *pFlexibleBody = NULL;
    int nFlexibleBodyInitFlag = false;
	KG3DMesh* pMesh = NULL;

    KGLOG_PROCESS_ERROR(pModel);
    KGLOG_PROCESS_ERROR(ppiRetFlexibleBody);
	pMesh = pModel->GetMesh();
	KGLOG_PROCESS_ERROR(pMesh);

	if(!pMesh->m_lpFlexibleBody)
	{
		hrResult = KG3D_E_NO_FLEXIBLE_BODY_DATA;
	}
	KG_PROCESS_ERROR(pMesh->m_lpFlexibleBody);

	KG3DFlexibleBody* pSrc = static_cast<KG3DFlexibleBody*>(pMesh->m_lpFlexibleBody);

    pFlexibleBody = new KG3DFlexibleBody();
    KGLOG_PROCESS_ERROR(pFlexibleBody);

    hrRetCode = pSrc->CloneFromMesh(pFlexibleBody,pModel);

    /*if (hrRetCode != KG3D_E_NO_FLEXIBLE_BODY_DATA)
        KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
    else
        KG_COM_PROC_ERR_RET_ERR(hrRetCode);*/
    
    nFlexibleBodyInitFlag = true;

    *ppiRetFlexibleBody = pFlexibleBody;
    hrResult = S_OK;
Exit0:
    if (FAILED(hrResult))
    {
        if (nFlexibleBodyInitFlag)
        {
            pFlexibleBody->UnInit();
            nFlexibleBodyInitFlag = false;
        }
        KG_DELETE(pFlexibleBody);
    }
    return hrResult;
}

HRESULT KG3DCreateFlexibleBodyFromMesh(
							   KG3DMesh *pMesh, IKG3DFlexibleBody **ppiRetFlexibleBody
							   )
{
	HRESULT hrResult  = E_FAIL;
	HRESULT hrRetCode = E_FAIL;
	KG3DFlexibleBody *pFlexibleBody = NULL;
	int nFlexibleBodyInitFlag = false;

	KGLOG_PROCESS_ERROR(pMesh);
	KGLOG_PROCESS_ERROR(ppiRetFlexibleBody);

	pFlexibleBody = new KG3DFlexibleBody();
	KGLOG_PROCESS_ERROR(pFlexibleBody);

	hrRetCode = pFlexibleBody->InitFromMesh(pMesh);
	if (hrRetCode != KG3D_E_NO_FLEXIBLE_BODY_DATA)
		KGLOG_COM_PROC_ERR_RET_ERR(hrRetCode);
	else
		KG_COM_PROC_ERR_RET_ERR(hrRetCode);

	nFlexibleBodyInitFlag = true;

	*ppiRetFlexibleBody = pFlexibleBody;
	hrResult = S_OK;
Exit0:
	if (FAILED(hrResult))
	{
		if (nFlexibleBodyInitFlag)
		{
			pFlexibleBody->UnInit();
			nFlexibleBodyInitFlag = false;
		}
		KG_DELETE(pFlexibleBody);
	}
	return hrResult;
}

////////////////////////////////////////////////////////////////////////////////
HRESULT KG3DIsFlexibleBodyBone(const char cszBoneName[], int *pnRetFlexibleBoneFlag)
{
    HRESULT hrResult = E_FAIL;

    KGLOG_PROCESS_ERROR(cszBoneName);
    KGLOG_PROCESS_ERROR(pnRetFlexibleBoneFlag);

    *pnRetFlexibleBoneFlag = _IsFirstFlexiBoneName(cszBoneName);
    if (!*pnRetFlexibleBoneFlag)
    {
        *pnRetFlexibleBoneFlag = _IsSubFlexiBoneName(cszBoneName);
    }

    hrResult = S_OK;
Exit0:
    return hrResult;
}

HRESULT KG3DFlexibleBody::CloneFromMesh(KG3DFlexibleBody* pDest,KG3DModel* pModel)
{
	pDest->m_pModel = pModel;
	pDest->m_vecDriverBone = m_vecDriverBone;
	pDest->m_vecNormalBone = m_vecNormalBone;
	return S_OK;
}
