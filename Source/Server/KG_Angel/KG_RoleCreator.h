#ifndef KG_RoleCreatorH
#define	KG_RoleCreatorH

#include <map>
#include <vector>
#include "Protocol/GoddessProtocol.h"
#include "KRoleDBDataDef.h"

class KG_RoleCreator
{
public:

	KG_RoleCreator();
	~KG_RoleCreator();

	int Init();
    int UnInit();
    int GetRoleData(
        KDB_ROLE_GENERATOR_PARAM *pParam, 
        unsigned uRoleDataBufferSize, 
        void *pvRoleData, 
        unsigned *puRoleDataSize,
        unsigned uRoleID
    );
};
#endif