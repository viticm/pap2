////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG_RoleInfo3.h
//  Version     : 1.0
//  Creator     : Wu Caizhong
//  Create Date : 2005-09-22 14:50:15
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG_ROLEINFO3_H_
#define _INCLUDE_KG_ROLEINFO3_H_

#include "KG_Angel.h"
#include "KGError.h"
#include "Engine/KG_InterlockedVariable.h"


////////////////////////////////////////////////////////////////////////////////
class KG_RoleInfo3 : public IKG_RoleInfo
{
public:
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

    ////////////////////////////////////////////////////////////////////////////
    // @brief : Check role data.
    // @remark : interface function, see more information in KG_Angel.h
    virtual int CheckRoleData(
        unsigned uBufferSize, 
        void *pvRoleData,
        KG_ROLE_DATA_ERROR_PROCESS *pErrorProcess
    );

    ////////////////////////////////////////////////////////////////////////////
    // @brief : Get role name of role
    // @remark : interface function, see more information in KG_Angel.h
    virtual int GetRoleName(
        unsigned uBufferSize,   void *pvRoleData, 
        unsigned *puRoleNameSize, void **ppvRoleName
    );
    
    ////////////////////////////////////////////////////////////////////////////
    // @brief : Get account name of role
    // @remark : interface function, see more information in KG_Angel.h
    virtual int GetAccountName(
        unsigned uBufferSize, void *pvRoleData, 
        unsigned *puAccountNameSize, void **ppvAccountName
    );

    //////////////////////////////////////////////////////////////////////////
    //@brief : Get role ID
    // @remark : interface function, see more information in KG_Angel.h
    virtual int GetRoleID(
        unsigned uBufferSize,
        void *pvRoleData,
        unsigned *puRetRoleIDSize, 
        void **ppvRetRoleID
    );


    ////////////////////////////////////////////////////////////////////////////
    // @brief : Get account's role list item's information
    // @remark : interface function, see more information in KG_Angel.h
    virtual int GetRoleListItem(
        unsigned uBufferSize, void *pvRoleData, 
        unsigned *puRoleListItemSize, void *pvRoleListItem
    );

    ////////////////////////////////////////////////////////////////////////////
    // @brief : Get max size of account's role list item
    virtual unsigned GetRoleListItemMaxSize();

    ////////////////////////////////////////////////////////////////////////////
    // @brief : Get role list information from a set of role list item
    // @remark : interface function, see more information in KG_Angel.h
    virtual int GetRoleList(
        unsigned uBufferSize, void *pvRoleListItems,
        unsigned *puRoleListBufferSize, void *pvRoleList        
    );    

    ////////////////////////////////////////////////////////////////////////////////
    // @brief : Get max buffer size for account info
    virtual unsigned GetRoleListMaxSize();

    ////////////////////////////////////////////////////////////////////////////
    // @brief : Get role data size
    // @remark : interface function, see more information in KG_Angel.h
    virtual int GetRoleDataSize(
        unsigned uBufferSize, void *pvRoleData, unsigned *puRoleDataSize
    );
    
   ////////////////////////////////////////////////////////////////////////////
   // @brief : Copy name of role to a buffer 
   // @remark : interface function, see more information in KG_Angel.h
    virtual int CopyRoleName(
        unsigned uBufferSize, void *pvRoleData, 
        unsigned uRoleNameBufferSize, char *pszRoleName
    );

    ////////////////////////////////////////////////////////////////////////////
    // @brief : Copy account name of role to a buffer
    // @remark : interface function, see more information in KG_Angel.h
    virtual int CopyAccountName(
        unsigned uBufferSize, void *pvRoleData, 
        unsigned uAccountNameBufferSize, char *pszAccountName
    );

    ////////////////////////////////////////////////////////////////////////////
    // @remark : interface function, see more information in KG_Angel.h
    virtual int SetRoleName(
        unsigned uBufferSize, void *pvRoleData, const char cszNewName[]
    );

    ////////////////////////////////////////////////////////////////////////////
    // @remark : interface function, see more information in KG_Angel.h
    virtual int SetAccountName(
        unsigned uBufferSize, void *pvRoleData, const char cszNewName[]
    );

    //////////////////////////////////////////////////////////////////////////
    //@brief : Set role ID
    // @remark : interface function, see more information in KG_Angel.h
    virtual int SetRoleID(unsigned uBufferSize, void *pvRoleData, unsigned uRoleID);

    ////////////////////////////////////////////////////////////////////////////
    virtual int ModifyRoleData(
        unsigned uOrgRoleDataSize, void *pvOrgRoleData,
        unsigned uNewRoleDataBufferSize, 
        unsigned *puNewRoleDataSize, void *pvNewRoleData,
        int *nModifiedFlag
    );
    ////////////////////////////////////////////////////////////////////////////
public:
    KG_RoleInfo3(void);
private:
    ~KG_RoleInfo3(void);         // just enable new operator to create
                                 // and Release() to destroy.
    unsigned long m_ulRefCount;
};



#endif //_INCLUDE_KG_ROLEINFO3_H_
