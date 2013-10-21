#ifndef _KTARGET_H_ 
#define _KTARGET_H_

#include "Global.h"

class KCharacter;
class KNpc;
class KPlayer;
class KDoodad;
class KSceneObject;
class KItem;

class KTarget
{
public:

    KTarget();
    ~KTarget();

    TARGET_TYPE GetTargetType();

    BOOL SetTarget(KCharacter* pCharacter);    
    BOOL SetTarget(KDoodad* pDoodad);
    BOOL SetTarget(KItem* pItem);
    BOOL SetTarget(int nX, int nY, int nZ);
    BOOL SetTarget(TARGET_TYPE eType, DWORD dwID);

    BOOL GetTarget(KCharacter** ppCharacter);
    BOOL GetTarget(KNpc** ppNpc);    
    BOOL GetTarget(KPlayer** ppPlayer);
    BOOL GetTarget(KDoodad** ppDoodad);
    BOOL GetTarget(KSceneObject** ppSceneObject);
    BOOL GetTarget(KItem** ppItem);
    BOOL GetTarget(int* pnX, int* pnY, int* pnZ);

    BOOL ClearTarget();
private:
	TARGET_TYPE		m_eTargetType;
	DWORD			m_dwID;
    union
    {
        struct
        {
            int		        nX;
	        int		        nY;
            int             nZ;
        } m_Coordination;
        KPlayer*        m_pPlayer;
        KNpc*           m_pNpc;
	    KDoodad*        m_pDoodad;
        KItem*          m_pItem;
    };
};

#endif // _KTARGET_H_