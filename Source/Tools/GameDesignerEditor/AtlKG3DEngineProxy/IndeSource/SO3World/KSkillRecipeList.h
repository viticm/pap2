#ifndef _KRECIPE_LIST_H_
#define _KRECIPE_LIST_H_

#include <vector>
#include <algorithm>

#define KG_SKILL_RECIPE_DB_LOAD_LOWEST_VER  1
#define KG_SKILL_RECIPE_DB_CURRENT_VER      2

class KPlayer;

// warning C4200: nonstandard extension used : zero-sized array in struct/union
#pragma warning(disable: 4200)

#pragma pack(1)
struct KG_SKILL_RECIPE_LIST_DB_DATA_BASE 
{
    WORD    wVersion;
};

struct KG_SKILL_RECIPE_LIST_DB_DATA_V1 : KG_SKILL_RECIPE_LIST_DB_DATA_BASE
{
    WORD    wCount;
    DWORD   dwRecipeKeyArray[0];
};

struct KG_SKILL_RECIPE_LIST_DB_DATA_V2 : KG_SKILL_RECIPE_LIST_DB_DATA_BASE
{
    WORD    wCount;
    struct _RecipeNode
    {
        DWORD   dwRecipeKey;
        BYTE    byActive;
    } RecipeArray[0];
};

struct KG_ADD_SKILL_RECIPE_NOTIFY 
{
    DWORD       dwRecipeID;
    DWORD       dwRecipeLevel;
};
#pragma pack()

struct KG_SKILL_RECIPE_LIST_NODE 
{
    DWORD   dwRecipeKey;
    BOOL    bActive;
};

class KSkillRecipeList
{
public:
    BOOL Init(KPlayer* pPlayer);
    void UnInit();

    BOOL CanAdd(DWORD dwRecipeKey);
    BOOL Add(DWORD dwRecipeKey);
    BOOL Delete(DWORD dwRecipeKey);
    BOOL Find(DWORD dwRecipeKey);
    int  Count();
    void Clear();

    BOOL Active(DWORD dwRecipeKey);
    BOOL Deactive(DWORD dwRecipeKey);

    BOOL GetRecipeList(
        DWORD dwSkillID, DWORD dwSkillLevel, 
        size_t uMaxListLength, size_t* puArrayCount, size_t* puActiveCount, KG_SKILL_RECIPE_LIST_NODE RecipeList[]
    );

#ifdef _SERVER
    BOOL Load(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen);
    BOOL Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);
#endif

    template <class TFunc>
    BOOL Traverse(TFunc &Func);

private:
    KG_SKILL_RECIPE_LIST_NODE* _Find(DWORD dwRecipeKey);

#ifdef _SERVER
    BOOL _LoadDataV1(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen);
    BOOL _LoadDataV2(size_t* puUsedSize, BYTE* pbyData, size_t uDataLen);
#endif

    typedef std::vector<KG_SKILL_RECIPE_LIST_NODE> KG_SKILL_RECIPE_LIST;
    KPlayer*                m_pPlayer;
    KG_SKILL_RECIPE_LIST    m_SkillRecipeVector;
};

template <class TFunc>
BOOL KSkillRecipeList::Traverse(TFunc &Func)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;
    KG_SKILL_RECIPE_LIST::iterator it;
    KG_SKILL_RECIPE_LIST::iterator itBegin;
    KG_SKILL_RECIPE_LIST::iterator itEnd;

    itBegin = m_SkillRecipeVector.begin();
    itEnd   = m_SkillRecipeVector.end();
    for (it = itBegin; it != itEnd; ++it)
    {
        bRetCode = Func(*it);
        KG_PROCESS_ERROR(bRetCode);
    }

    bResult = true;
Exit0:
    return bResult;
}

#endif
