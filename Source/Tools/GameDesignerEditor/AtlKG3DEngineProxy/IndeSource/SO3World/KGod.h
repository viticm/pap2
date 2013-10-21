
#ifndef _KGOD_H_
#define _KGOD_H_

#if defined(_SERVER)
#include <map>
#include <list>
#include "Global.h"

class KCharacter;
class KNpc;
class KPlayer;
class KDoodad;
class IKG_Buffer;
class KTarget;
class KRegion;
class KItem;
class KQuest;
class KShop;
class KSkill;
class KScene;


struct KGod
{
    BOOL    bConnected;
    KScene* pScene;
};

#endif	//_SERVER
#endif	
