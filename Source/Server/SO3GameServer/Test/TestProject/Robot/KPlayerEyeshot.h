#ifndef _KPlayerEyeshot_H_
#define _KPlayerEyeshot_H_

#include "KBaseObjectSet.h"
#include "KMath.h"

class KPlayer;
class KNpc;
class KDoodad;
class KSceneObject;
class KMonster;
class KRegion;

class KPlayerEyeshot
{
public:
    KPlayerEyeshot();
    ~KPlayerEyeshot();

    int Init(KMonster *pOwner);
    int UnInit();
    int Activate();

    int IsVisible(KSceneObject *pObject);
    int IsVisible(KRegion *pRegion);

    KPlayer *NewPlayer(DWORD dwID);
    int      DeletePlayer(KPlayer *pPlayer);
    KPlayer *GetPlayer(DWORD dwID);

    KNpc    *NewNpc(DWORD dwID);
    int      DeleteNpc(KNpc *pNpc);
    KNpc    *GetNpc(DWORD dwID);

    KDoodad *NewDoodad(DWORD dwID);
    int      DeleteDoodad(KDoodad *pDoodad);
    KDoodad *GetDoodad(DWORD dwID);

    KNpc *GetNearNpc();

    int DeleteAllObject();

private:
    KBaseObjectSet<KPlayer>				m_PlayerSet;
    KBaseObjectSet<KNpc>				m_NpcSet;
    KBaseObjectSet<KDoodad>				m_DoodadSet;

    KMonster *m_pOwner;

private:
    template <class T>
    struct KActivateObjectFunc
    {
        int operator ()(DWORD dwID, T *pObject)
        {
            pObject->Activate();
            return TRUE;
        }
    };

    template <class T>
    struct KDeleteObjectFunc
    {
        KDeleteObjectFunc(KBaseObjectSet<T> *pObjSet) :
                        pObjectSet(pObjSet)
        {
        }
        int operator ()(DWORD dwID, T *pObject)
        {
            int nRetCode = FALSE;
            if (pObject && pObjectSet)
            {
                nRetCode = pObject->UnInit();
                KGLOG_CHECK_ERROR(nRetCode);

                pObjectSet->Delete(pObject);
            }
            return TRUE;
        }
        KBaseObjectSet<T> *pObjectSet;
    };

    template <class T>
    struct KGetObjectFunc
    {
        KGetObjectFunc(KBaseObjectSet<T> *pObjSet) : 
                pTarget(NULL), pObjectSet(pObjSet), nIndex(0), nCurrentIndex(0)
        {
            if (pObjectSet)
            {
                nIndex = g_Random(pObjectSet->GetUsedSize());
            }
        }

        int operator ()(DWORD dwID, T *pObject)
        {
            ++nCurrentIndex;
            if (nCurrentIndex >= nIndex && pObject)
            {
                pTarget = pObject;
                return FALSE;
            }

            return TRUE;
        }

        T *pTarget;
        KBaseObjectSet<T> *pObjectSet;
        int nIndex;
        int nCurrentIndex;
    };
};

#endif //_KPlayerEyeshot_H_