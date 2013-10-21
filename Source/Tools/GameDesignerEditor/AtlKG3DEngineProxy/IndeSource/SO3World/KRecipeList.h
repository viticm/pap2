#ifndef _KRECIPE_LIST_
#define _KRECIPE_LIST_

#include <vector>

#include "Global.h"

class KPlayer;
class KRecipeLearningInfo;

#pragma	pack(1)
struct KRecipeDBItem_V0 
{
	BYTE byCraftID;
	BYTE byRecipeData[(OLD_MAX_RECIPE_ID + CHAR_BIT - 1) / CHAR_BIT];
};

struct KRecipeDBItem 
{
	BYTE byCraftID;
	BYTE byRecipeData[(MAX_RECIPE_ID + CHAR_BIT - 1) / CHAR_BIT];
};

struct KRecipeDBData
{
    BYTE            byCount;
    KRecipeDBItem   RecipeDBItem[0];
};
#pragma pack()

class KRecipeList
{
public:
	BOOL    Init(KPlayer* pPlayer);
	void    UnInit();

    BOOL    Load(BYTE* pbyData, size_t uDataLen);
    BOOL    Save(size_t* puUsedSize, BYTE* pbyBuffer, size_t uBufferSize);

	BOOL	CanLearnRecipe(DWORD dwCraftID, DWORD dwRecipeID, KRecipeLearningInfo* pLearningInfo);
	BOOL	LearnRecipe(DWORD dwCraftID, DWORD dwRecipeID);
	BOOL	ForgetRecipe(DWORD dwCraftID);
	BOOL	IsRecipeLearned(DWORD dwCraftID, DWORD dwRecipeID);
	DWORD	GetRecipeCountByCraftID(DWORD dwCraftID);
	BOOL	GetRecipeList(DWORD dwCraftID, std::vector<DWORD>& vecRecipeIDList);
	
private:
	KPlayer*    m_pPlayer;
	BYTE        m_byRecipeState[MAX_CRAFT_COUNT][MAX_RECIPE_ID];
};

#endif //_KRECIPE_LIST_
