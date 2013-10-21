#ifndef _KTARGET_H_ 
#define _KTARGET_H_

#include "Global.h"
#include "Luna.h"

class KCharacter;
class KNpc;
class KPlayer;
class KDoodad;
class KSceneObject;
class KItem;

struct KTargetData
{
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

		struct  
		{
			DWORD			dwBoxIndex;
			DWORD			dwX;
		} m_ItemPos;
    };
};

class KTarget : public KTargetData
{
public:
    KTarget();
    ~KTarget();

	void SetTargetData(KTargetData const& TargetData);
	void GetTargetData(KTargetData& TargetData);

    TARGET_TYPE GetTargetType();
	DWORD GetTargetID();

    BOOL SetTarget(KCharacter* pCharacter);    
    BOOL SetTarget(KDoodad* pDoodad);
    BOOL SetTarget(KItem* pItem);
    BOOL SetTarget(int nX, int nY, int nZ);
	BOOL SetTarget(KPlayer* pPlayer, KItem* pItem);
	BOOL SetTarget(KPlayer* pPlayer, DWORD dwBoxIndex, DWORD dwX);
    BOOL SetTarget(TARGET_TYPE eType, DWORD dwID);

    BOOL GetTarget(KCharacter** ppCharacter);
    BOOL GetTarget(KNpc** ppNpc);    
    BOOL GetTarget(KPlayer** ppPlayer);
    BOOL GetTarget(KDoodad** ppDoodad);
    BOOL GetTarget(KSceneObject** ppSceneObject);
    BOOL GetTarget(KItem** ppItem);
    BOOL GetTarget(int* pnX, int* pnY, int* pnZ);
	BOOL GetTarget(KPlayer** ppPlayer, KItem** ppItem);
	BOOL GetTarget(KPlayer** ppPlayer, DWORD* pdwBoxIndex, DWORD* pdwX);

    BOOL ClearTarget();

    DECLARE_LUA_CLASS(KTarget);

    int LuaGetTargetType(Lua_State* L);
    int LuaGetPlayer(Lua_State* L);
    int LuaGetNpc(Lua_State* L);
    int LuaGetDoodad(Lua_State* L);
    int LuaGetItem(Lua_State* L);
    int LuaGetCoordinates(Lua_State* L);
};

#endif // _KTARGET_H_
