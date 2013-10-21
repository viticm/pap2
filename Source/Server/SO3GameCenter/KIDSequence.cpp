//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KDBBase.cpp
//  Version     :   1.0
//  Creater     :   Xia Yong
//  Date        :   2009-06-25 
//  Comment     :   序列生成器
//////////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KDBBase.h"
#include "KIDSequence.h"
#include "Engine/KMutex.h"
#include "errmsg.h"
#include "mysqld_error.h"

#define SEQ_TABLE_NAME  "sequence"
#define SEQ_NAME        "seqname"
#define SEQ_CURRENT_ID  "currentid"

static const unsigned long MYSQL5_VERSION_ID = 50000; // MySQL 5 Version ID

#ifdef WIN32
#define strtoull _strtoui64
#endif

class KIDSequence : public IKIDSequence
{
public:
    KIDSequence();
    ~KIDSequence();

    HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
        /* [iid_is][out] */ void **ppvObject
    )
    {
        KG_USE_ARGUMENT(riid);
        KG_USE_ARGUMENT(ppvObject);
        return KG_E_FAIL;
    }

    ULONG STDMETHODCALLTYPE AddRef(void)
    {
        return (unsigned long)KG_InterlockedIncrement((long *)&m_ulRefCount);
    }

    ULONG STDMETHODCALLTYPE Release(void)
    {
        unsigned long ulCount = KG_InterlockedDecrement((long *)&m_ulRefCount);
        if (ulCount == 0)
        {
            Destroy();
            delete this;
        }
        return ulCount;
    }

    BOOL GenerateID(const char cszSequenceName[], int nPrefetch, uint64_t* puRetID);

public:
    BOOL Create(const char cszDBAddr[], const char cszDBName[], const char cszUserName[], const char cszPassword[]);
    void Destroy();
    
private:
    BOOL DoQuery(const char cszSQL[]);

    unsigned long   m_ulRefCount;
    MYSQL*          m_pDBHandle;
    KMutex          m_Mutex;

    struct KID_PREFETCH_INFO
    {
        uint64_t uIDEnd;
        uint64_t uNextID;
    };

    typedef KMemory::KAllocator<std::pair<kstring, KID_PREFETCH_INFO> > KID_PREFETCH_TABLE_ALLOCATOR;
    typedef std::map<kstring, KID_PREFETCH_INFO, std::less<kstring>, KID_PREFETCH_TABLE_ALLOCATOR> KID_PREFETCH_TABLE;
    
    KID_PREFETCH_TABLE m_PrefetchTable;
};

KIDSequence::KIDSequence()
{
    m_ulRefCount    = 1;
    m_pDBHandle     = NULL;
}

KIDSequence::~KIDSequence()
{
    assert(m_pDBHandle == NULL);

    m_PrefetchTable.clear();
}

BOOL KIDSequence::GenerateID(const char cszSequenceName[], int nPrefetch, uint64_t* puRetID)
{
    BOOL                                            bResult     = false;    
    int                                             nRetCode    = false;
    KID_PREFETCH_INFO*                              pPrefetch   = NULL;
    MYSQL_RES*                                      pQueryRes   = NULL;
    uint64_t                                        uStartID    = 0;
    my_ulonglong                                    uRowCount   = 0;
    MYSQL_ROW                                       Row;
    char                                            szQuery[256];
    KID_PREFETCH_TABLE::iterator                    it;
    std::pair<KID_PREFETCH_TABLE::iterator, BOOL>   InsRet;

    assert(cszSequenceName);
    assert(puRetID);
    assert(m_pDBHandle);
    assert(nPrefetch >= 1);

    m_Mutex.Lock();
    
    it = m_PrefetchTable.find(cszSequenceName);
    if (it != m_PrefetchTable.end())
    {
        pPrefetch = &it->second;
    }
    else
    {
        InsRet = m_PrefetchTable.insert(std::make_pair(cszSequenceName, KID_PREFETCH_INFO()));
        KGLOG_PROCESS_ERROR(InsRet.second);
        
        pPrefetch = &InsRet.first->second;

        pPrefetch->uIDEnd   = 0;
        pPrefetch->uNextID  = 0;
    }

    if (pPrefetch->uNextID < pPrefetch->uIDEnd)
        goto Exit1;

    while(true)
    { 
        nRetCode = snprintf(
            szQuery,  
            sizeof(szQuery),
            "select %s from %s where %s = '%s'",
            SEQ_CURRENT_ID, SEQ_TABLE_NAME, SEQ_NAME,
            cszSequenceName
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szQuery));

        nRetCode = DoQuery(szQuery);
        KGLOG_PROCESS_ERROR(nRetCode);

        if (pQueryRes)
        {
            mysql_free_result(pQueryRes);
            pQueryRes = NULL;
        }

        pQueryRes = mysql_store_result(m_pDBHandle);
        KGLOG_PROCESS_ERROR(pQueryRes);

        uRowCount = mysql_num_rows(pQueryRes);
        if (uRowCount == 0)
        {
            nRetCode = snprintf(
                szQuery, sizeof(szQuery),
                "insert into %s (%s, %s) values('%s', 1)",
                SEQ_TABLE_NAME, SEQ_NAME, SEQ_CURRENT_ID,
                cszSequenceName
            );
            KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szQuery));

            nRetCode = DoQuery(szQuery);
            KGLOG_PROCESS_ERROR(nRetCode);

            continue;
        }

        Row = mysql_fetch_row(pQueryRes);
        KGLOG_PROCESS_ERROR(Row);
        
        assert(Row[0]);

        uStartID = (uint64_t)strtoull(Row[0], NULL, 10);
        
        KGLOG_PROCESS_ERROR(uStartID < ULLONG_MAX - nPrefetch);

        nRetCode = snprintf(
            szQuery,
            sizeof(szQuery),
            "update %s set %s = %s + %d where %s = '%s' and %s = %llu",
            SEQ_TABLE_NAME, SEQ_CURRENT_ID, SEQ_CURRENT_ID, nPrefetch,
            SEQ_NAME, cszSequenceName, SEQ_CURRENT_ID, uStartID
        );
        KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szQuery));

        nRetCode = DoQuery(szQuery);
        KGLOG_PROCESS_ERROR(nRetCode);

        nRetCode = (int)mysql_affected_rows(m_pDBHandle);
        if (nRetCode == 1)
            break;
    }
    
    pPrefetch->uIDEnd   = uStartID + nPrefetch;
    pPrefetch->uNextID  = uStartID;

Exit1:
    *puRetID = pPrefetch->uNextID++;

    bResult = true;
Exit0:
    if (pQueryRes)
    {
        mysql_free_result(pQueryRes);
        pQueryRes = NULL;
    }

    m_Mutex.Unlock();
    return bResult;
}

BOOL KIDSequence::Create(const char cszDBAddr[], const char cszDBName[], const char cszUserName[], const char cszPassword[])
{
    BOOL        bResult     = false;
    MYSQL*      pHandle     = NULL;
    int         nRetCode    = false;
    MYSQL_RES*  pQueryRes   = NULL;
    unsigned    uRowCount   = 0;
    char        szQuery[256];

    pHandle = MySQL_Connect(cszDBAddr, cszDBName, cszUserName, cszPassword);
    KGLOG_PROCESS_ERROR(pHandle);
    
    m_pDBHandle = pHandle;
    pHandle = NULL;

    nRetCode = snprintf(
        szQuery,
        sizeof(szQuery), 
        "create table if not exists %s (%s char(64) primary key, %s bigint unsigned)",
        SEQ_TABLE_NAME, SEQ_NAME, SEQ_CURRENT_ID
    );
    KGLOG_PROCESS_ERROR(nRetCode > 0 && nRetCode < (int)sizeof(szQuery));
    
    nRetCode = DoQuery(szQuery);
    KG_PROCESS_ERROR(nRetCode);
    
    bResult = true;
Exit0:
    if (!bResult)
    {
        if (pHandle)
        {
            MySQL_Disconnect(pHandle);
            pHandle = NULL;
        }
    }
    return bResult;
}

void KIDSequence::Destroy()
{
    if (m_pDBHandle)
    {
        MySQL_Disconnect(m_pDBHandle);
        m_pDBHandle = NULL;
    }
    m_PrefetchTable.clear();
}

BOOL KIDSequence::DoQuery(const char cszSQL[])
{
    BOOL     bResult            = false;
    BOOL     bRetCode           = false;
    unsigned ulMySQLErrorCode   = 0;

    assert(cszSQL);
    assert(m_pDBHandle);
    
    bRetCode = MySQL_Query(m_pDBHandle, cszSQL, &ulMySQLErrorCode);
    KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (ulMySQLErrorCode != 0 && ulMySQLErrorCode != ER_DUP_ENTRY) // 主键冲突不用断开链接
    {
        MySQL_Disconnect(m_pDBHandle);
        m_pDBHandle = NULL;
    }
    return bResult;
}

IKIDSequence* CreateIDSequence(
    const char cszDBAddr[], const char cszDBName[], const char cszUserName[], const char cszPassword[]
)
{
    IKIDSequence*   piResult    = NULL;
    KIDSequence*    pIDSequence = NULL;
    BOOL            bRetCode    = false;

    assert(cszDBAddr);
    assert(cszDBName);
    assert(cszUserName);
    assert(cszPassword);

    pIDSequence = new KIDSequence;
    KGLOG_PROCESS_ERROR(pIDSequence);

    bRetCode = pIDSequence->Create(cszDBAddr, cszDBName, cszUserName, cszPassword);
    KGLOG_PROCESS_ERROR(bRetCode);

    piResult = pIDSequence;
Exit0:
    if (!piResult)
    {
        KG_COM_RELEASE(pIDSequence);
    }
    return piResult;
}
