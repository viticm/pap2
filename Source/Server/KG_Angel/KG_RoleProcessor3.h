////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_RoleProcessor3.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2005-09-22 14:50:36
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG_ROLEPROCESSOR3_H_
#define _INCLUDE_KG_ROLEPROCESSOR3_H_

////////////////////////////////////////////////////////////////////////////////


#include <string>
#include <map>
#include "KG_Angel.h"
#include "Engine/KG_InterlockedVariable.h"
#include "Engine/KMutex.h"
#include "KG_RoleCreator.h"

struct IKG_Buffer;

class KG_RoleProcessor3 : public IKG_RoleProcessor
{
public:
    KG_RoleProcessor3();

    ////////////////////////////////////////////////////////////////////////////
    //                 interface function                                     //
    ////////////////////////////////////////////////////////////////////////////
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
        long lCount = KG_InterlockedDecrement((long *)&m_ulRefCount);
        ASSERT(lCount >= 0);

        if (lCount == 0)
        {
            delete this;
        }
        return lCount;
    }

    virtual int Init(void *pvContent);

    virtual int OnRoleLoadFromDatabase(
        unsigned uRoleDataSize, void *pvRoleData, int *pnContinueFlag
    );

    virtual int OnRoleSaveToDatabase(
        unsigned uRoleDataSize, void *pvRoleData, int *pnContinueFlag
    );

    virtual int SetStatisticData(
        KG_STATISTIC_DATA_TYPE eDataType, IKG_Buffer *pStatistic
    );

    ////////////////////////////////////////////////////////////////////////////
    // @brief : Create a new role
    // @remark : interface function, see more information in KG_Angel.h
    virtual int CreateRole(
        unsigned uParamBufferSize, void *pvCreateParam,
        unsigned *puRoleDataBufferSize, void *pvRoleData,
        unsigned *puNameSize, void **ppvName,
        unsigned *puAccountSize, void **ppvAccount,
        unsigned *puRoleListItemSize, void *pvRoleListItem,
        unsigned uRoleID
    );

    virtual int UnInit(void *pvContent);

private:
    ~KG_RoleProcessor3();

private:
    typedef std::map<std::string, int>       ROLE_NAME_AND_WORLD_STAT_MAP;
    ROLE_NAME_AND_WORLD_STAT_MAP m_RoleNameAndWorldStatMap;
    KMutex                       m_RoleNameAndWorldStatMapLock;
    KG_RoleCreator m_RoleCreator;
    IKG_RoleInfo   *m_piRoleInfo;
    unsigned long m_ulRefCount;
};



#endif //_INCLUDE_KG_ROLEPROCESSOR3_H_
