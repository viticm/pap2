#ifndef _KRECIPE_H_
#define _KRECIPE_H_

#include <map>
#include "Global.h"
#include "KPlayer.h"
#include "Luna.h"

struct KRecipeBase
{
	ALL_CRAFT_TYPE  eCraftType;
	DWORD	        dwProfessionID;
	DWORD	        dwRequireProfessionLevel;
	DWORD	        dwToolItemType;
	DWORD	        dwToolItemIndex;
	int		        nEquipmentType;
	DWORD	        dwProfessionExp;
	int		        nPrepareFrame;
    DWORD           dwScriptID;

    virtual const char* GetName();
	CRAFT_RESULT_CODE	CheckCast(
        KPlayer* pCaster, KTarget& Target, 
        DWORD dwProfessionIDExt = 0, DWORD dwRequireProfessionExtLevel = 0 
    );

	DECLARE_LUA_STRUCT_ENUM(CraftType, eCraftType);
	DECLARE_LUA_STRUCT_DWORD(ProfessionID, dwProfessionID);
	DECLARE_LUA_STRUCT_DWORD(RequireProfessionLevel, dwRequireProfessionLevel);
	DECLARE_LUA_STRUCT_DWORD(ToolItemType, dwToolItemType);
	DECLARE_LUA_STRUCT_DWORD(ToolItemIndex, dwToolItemIndex);
	DECLARE_LUA_STRUCT_INTEGER(EquipmentType, nEquipmentType);
	DECLARE_LUA_STRUCT_DWORD(Exp, dwProfessionExp);
	DECLARE_LUA_STRUCT_INTEGER(PrepareFrame, nPrepareFrame);
};

//合成技能用的配方
struct KCraftRecipe : public KRecipeBase
{
	DWORD dwID;
	char szName[MAX_RECIPE_NAME_LEN];
	char szBelong[MAX_BELONG_NAME_LEN];

    int	  nStamina;  // 精力
	DWORD dwRequireBranchID;
	DWORD dwRequireDoodadID;

	DWORD dwRequireItemType[MAX_MATERIAL];
	DWORD dwRequireItemIndex[MAX_MATERIAL];
	DWORD dwRequireItemCount[MAX_MATERIAL];
    
	DWORD dwCreateItemType[MAX_RESULTANT];
	DWORD dwCreateItemIndex[MAX_RESULTANT];
	DWORD dwCreateItemMinCount[MAX_RESULTANT];
	DWORD dwCreateItemMaxCount[MAX_RESULTANT];
	DWORD dwCreateItemProbability[MAX_RESULTANT];

	BOOL bResultOnLevel;		//结果是否受等级浮动的影响

	CRAFT_RESULT_CODE CanCast(KPlayer* pCaster, KTarget& Target);
	virtual const char* GetName();

	DECLARE_LUA_CLASS(KCraftRecipe);
    DECLARE_LUA_STRUCT_INTEGER(Stamina, nStamina);

	DECLARE_LUA_STRUCT_STRING(Name, MAX_RECIPE_NAME_LEN, szName);
	DECLARE_LUA_STRUCT_STRING(Belong, MAX_BELONG_NAME_LEN, szBelong);
	DECLARE_LUA_STRUCT_BOOL(ResultOnLevel, bResultOnLevel);

	DECLARE_LUA_STRUCT_DWORD(RequireBranchID, dwRequireBranchID);
	DECLARE_LUA_STRUCT_DWORD(RequireDoodadID, dwRequireDoodadID);
	
	DECLARE_LUA_STRUCT_DWORD(RequireItemType1, dwRequireItemType[0]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex1, dwRequireItemIndex[0]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount1, dwRequireItemCount[0]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType2, dwRequireItemType[1]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex2, dwRequireItemIndex[1]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount2, dwRequireItemCount[1]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType3, dwRequireItemType[2]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex3, dwRequireItemIndex[2]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount3, dwRequireItemCount[2]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType4, dwRequireItemType[3]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex4, dwRequireItemIndex[3]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount4, dwRequireItemCount[3]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType5, dwRequireItemType[4]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex5, dwRequireItemIndex[4]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount5, dwRequireItemCount[4]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType6, dwRequireItemType[5]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex6, dwRequireItemIndex[5]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount6, dwRequireItemCount[5]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType7, dwRequireItemType[6]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex7, dwRequireItemIndex[6]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount7, dwRequireItemCount[6]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType8, dwRequireItemType[7]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex8, dwRequireItemIndex[7]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount8, dwRequireItemCount[7]);

	DECLARE_LUA_STRUCT_DWORD(CreateItemType1, dwCreateItemType[0]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemIndex1, dwCreateItemIndex[0]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMinCount1, dwCreateItemMinCount[0]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMaxCount1, dwCreateItemMaxCount[0]);

	DECLARE_LUA_STRUCT_DWORD(CreateItemType2, dwCreateItemType[1]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemIndex2, dwCreateItemIndex[1]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMinCount2, dwCreateItemMinCount[1]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMaxCount2, dwCreateItemMaxCount[1]);

	DECLARE_LUA_STRUCT_DWORD(CreateItemType3, dwCreateItemType[2]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemIndex3, dwCreateItemIndex[2]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMinCount3, dwCreateItemMinCount[2]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMaxCount3, dwCreateItemMaxCount[2]);

	DECLARE_LUA_STRUCT_DWORD(CreateItemType4, dwCreateItemType[3]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemIndex4, dwCreateItemIndex[3]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMinCount4, dwCreateItemMinCount[3]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMaxCount4, dwCreateItemMaxCount[3]);

	DECLARE_LUA_STRUCT_DWORD(CreateItemType5, dwCreateItemType[4]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemIndex5, dwCreateItemIndex[4]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMinCount5, dwCreateItemMinCount[4]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMaxCount5, dwCreateItemMaxCount[4]);

	DECLARE_LUA_STRUCT_DWORD(CreateItemType6, dwCreateItemType[5]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemIndex6, dwCreateItemIndex[5]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMinCount6, dwCreateItemMinCount[5]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMaxCount6, dwCreateItemMaxCount[5]);

	DECLARE_LUA_STRUCT_DWORD(CreateItemType7, dwCreateItemType[6]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemIndex7, dwCreateItemIndex[6]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMinCount7, dwCreateItemMinCount[6]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMaxCount7, dwCreateItemMaxCount[6]);

	DECLARE_LUA_STRUCT_DWORD(CreateItemType8, dwCreateItemType[7]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemIndex8, dwCreateItemIndex[7]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMinCount8, dwCreateItemMinCount[7]);
	DECLARE_LUA_STRUCT_DWORD(CreateItemMaxCount8, dwCreateItemMaxCount[7]);
};

//采集技能用的采集物集合
struct KCraftCollection : public KRecipeBase
{
    int   nThew; // 体力
	DWORD dwDoodadTemplateID;

	CRAFT_RESULT_CODE CanCast(KPlayer* pCaster, KTarget& Target);

	DECLARE_LUA_CLASS(KCraftCollection);
    DECLARE_LUA_STRUCT_INTEGER(Thew, nThew);
	DECLARE_LUA_STRUCT_DWORD(DoodadTemplateID, dwDoodadTemplateID);
};

//读书技能用的书本
struct KCraftRead : public KRecipeBase
{
	DWORD dwID;		//书的ID,用道具的耐久实现
	DWORD dwSubID;	//书的SubID,表示一本书中的某一卷

    int   nStamina;

    int   nPlayerExp;                   // 增加玩家经验
	DWORD dwExtendProfessionID1;		// 额外的技能ID
	DWORD dwExtendExp1;					// 增加一个额外的技能的经验

    DWORD dwExtendProfessionID2;        // 额外的技能ID
    DWORD dwExtendExp2;					// 增加一个额外的技能的经验

    DWORD dwCreateItemTab;
    DWORD dwCreateItemIndex;
    int   nStackNum;

    DWORD dwBuffID;
    DWORD dwBuffLevel;

    int nTrain;

	char szName[MAX_RECIPE_NAME_LEN];   // 书名

	CRAFT_RESULT_CODE CanCast(KPlayer* pCaster, KTarget& Target);
	virtual const char* GetName();

    DECLARE_LUA_CLASS(KCraftRead);
    DECLARE_LUA_STRUCT_DWORD(ID, dwID);
    DECLARE_LUA_STRUCT_DWORD(SubID, dwSubID);
    DECLARE_LUA_STRUCT_INTEGER(Stamina, nStamina);
    DECLARE_LUA_STRUCT_INTEGER(PlayerExp, nPlayerExp);
    DECLARE_LUA_STRUCT_DWORD(ExtendProfessionID1, dwExtendProfessionID1);
    DECLARE_LUA_STRUCT_DWORD(ExtendExp1, dwExtendExp1);
    DECLARE_LUA_STRUCT_DWORD(ExtendProfessionID2, dwExtendProfessionID2);
    DECLARE_LUA_STRUCT_DWORD(ExtendExp2, dwExtendExp2);
    DECLARE_LUA_STRUCT_DWORD(CreateItemTab, dwCreateItemTab);
    DECLARE_LUA_STRUCT_DWORD(CreateItemIndex, dwCreateItemIndex);
    DECLARE_LUA_STRUCT_INTEGER(StackNum, nStackNum);
    DECLARE_LUA_STRUCT_DWORD(BuffID, dwBuffID);
    DECLARE_LUA_STRUCT_DWORD(BuffLevel, dwBuffLevel);

    DECLARE_LUA_STRUCT_INTEGER(Train, nTrain);
    DECLARE_LUA_STRUCT_STRING(Name, MAX_RECIPE_NAME_LEN, szName);
};

//副魔技能
struct KCraftEnchant : public KRecipeBase
{
    int nStamina;
	DWORD dwID;
	char szName[MAX_RECIPE_NAME_LEN];
	char szBelong[MAX_BELONG_NAME_LEN];
	DWORD dwEnchantID; // 对应表Enchant.tab中的id

	DWORD dwRequireBranchID;
	DWORD dwRequireDoodadID;

	DWORD dwRequireItemType[MAX_MATERIAL];
	DWORD dwRequireItemIndex[MAX_MATERIAL];
	DWORD dwRequireItemCount[MAX_MATERIAL];

	CRAFT_RESULT_CODE CanCast(KPlayer* pCaster, KTarget& Target);
	virtual const char* GetName();

	DECLARE_LUA_CLASS(KCraftEnchant);
    DECLARE_LUA_STRUCT_INTEGER(Stamina, nStamina);
	DECLARE_LUA_STRUCT_STRING(Name, MAX_RECIPE_NAME_LEN, szName);
	DECLARE_LUA_STRUCT_STRING(Belong, MAX_BELONG_NAME_LEN, szBelong);
	DECLARE_LUA_STRUCT_DWORD(EnchantID, dwEnchantID);

	DECLARE_LUA_STRUCT_DWORD(RequireBranchID, dwRequireBranchID);
	DECLARE_LUA_STRUCT_DWORD(RequireDoodadID, dwRequireDoodadID);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType1, dwRequireItemType[0]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex1, dwRequireItemIndex[0]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount1, dwRequireItemCount[0]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType2, dwRequireItemType[1]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex2, dwRequireItemIndex[1]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount2, dwRequireItemCount[1]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType3, dwRequireItemType[2]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex3, dwRequireItemIndex[2]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount3, dwRequireItemCount[2]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType4, dwRequireItemType[3]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex4, dwRequireItemIndex[3]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount4, dwRequireItemCount[3]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType5, dwRequireItemType[4]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex5, dwRequireItemIndex[4]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount5, dwRequireItemCount[4]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType6, dwRequireItemType[5]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex6, dwRequireItemIndex[5]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount6, dwRequireItemCount[5]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType7, dwRequireItemType[6]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex7, dwRequireItemIndex[6]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount7, dwRequireItemCount[6]);

	DECLARE_LUA_STRUCT_DWORD(RequireItemType8, dwRequireItemType[7]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemIndex8, dwRequireItemIndex[7]);
	DECLARE_LUA_STRUCT_DWORD(RequireItemCount8, dwRequireItemCount[7]);
};

#define MAX_COPY_MATERIAL 4

// 抄录
struct KCraftCopy : public KRecipeBase
{
    DWORD dwID;		//书的ID,用道具的耐久实现
	DWORD dwSubID;	//书的SubID,表示一本书中的某一卷

	char szName[MAX_RECIPE_NAME_LEN];

    int	  nThew;

    DWORD dwCoolDownID;
    DWORD dwDoodadTemplateID;

	DWORD dwRequireItemType[MAX_COPY_MATERIAL];
	DWORD dwRequireItemIndex[MAX_COPY_MATERIAL];
	DWORD dwRequireItemCount[MAX_COPY_MATERIAL];
    
    int nRequirePlayerLevel;

    DWORD dwCreateItemType;
	DWORD dwCreateItemIndex;
	int   nStackNum;

    DWORD	dwProfessionIDExt;
	DWORD	dwRequireProfessionLevelExt;

	CRAFT_RESULT_CODE CanCast(KPlayer* pCaster, KTarget& Target);
	virtual const char* GetName();

    DECLARE_LUA_CLASS(KCraftCopy);
    DECLARE_LUA_STRUCT_INTEGER(Thew, nThew);
    
    DECLARE_LUA_STRUCT_DWORD(CoolDownID, dwCoolDownID);

    DECLARE_LUA_STRUCT_DWORD(RequireItemType1, dwRequireItemType[0]);
    DECLARE_LUA_STRUCT_DWORD(RequireItemIndex1, dwRequireItemIndex[0]);
    DECLARE_LUA_STRUCT_DWORD(RequireItemCount1, dwRequireItemCount[0]);

    DECLARE_LUA_STRUCT_DWORD(RequireItemType2, dwRequireItemType[1]);
    DECLARE_LUA_STRUCT_DWORD(RequireItemIndex2, dwRequireItemIndex[1]);
    DECLARE_LUA_STRUCT_DWORD(RequireItemCount2, dwRequireItemCount[1]);

    DECLARE_LUA_STRUCT_DWORD(RequireItemType3, dwRequireItemType[2]);
    DECLARE_LUA_STRUCT_DWORD(RequireItemIndex3, dwRequireItemIndex[2]);
    DECLARE_LUA_STRUCT_DWORD(RequireItemCount3, dwRequireItemCount[2]);

    DECLARE_LUA_STRUCT_DWORD(RequireItemType4, dwRequireItemType[3]);
    DECLARE_LUA_STRUCT_DWORD(RequireItemIndex4, dwRequireItemIndex[3]);
    DECLARE_LUA_STRUCT_DWORD(RequireItemCount4, dwRequireItemCount[3]);
    
    DECLARE_LUA_STRUCT_INTEGER(RequirePlayerLevel, nRequirePlayerLevel);

    DECLARE_LUA_STRUCT_DWORD(CreateItemType, dwCreateItemType);
    DECLARE_LUA_STRUCT_DWORD(CreateItemIndex, dwCreateItemIndex);

    DECLARE_LUA_STRUCT_DWORD(ProfessionIDExt, dwProfessionIDExt);
	DECLARE_LUA_STRUCT_DWORD(RequireProfessionLevelExt, dwRequireProfessionLevelExt);
};

template<class T>
class KRecipe
{
public:
	BOOL Init(const char* szFileName);
	BOOL UnInit();

	T* GetRecipe(DWORD dwID);
	BOOL LoadLine(ITabFile* piTabFile, int nLine, T& Recipe, const T& DefaultRecipe);
    
    DWORD GetRecipeID(DWORD dwDoodadTemplateID);
private:
	typedef std::map<DWORD, T> RECIPE_MAP;
	RECIPE_MAP m_mapRecipe;
};

template<class T>
BOOL KRecipe<T>::UnInit()
{
	m_mapRecipe.clear();
	return true;
}

template<>
BOOL KRecipe<KCraftRecipe>::LoadLine(ITabFile* piTabFile, int nLine, KCraftRecipe& Recipe, const KCraftRecipe& DefaultRecipe);

template<>
BOOL KRecipe<KCraftCollection>::LoadLine(ITabFile* piTabFile, int nLine, KCraftCollection& Recipe, const KCraftCollection& DefaultRecipe);

template<>
BOOL KRecipe<KCraftRead>::LoadLine(ITabFile* piTabFile, int nLine, KCraftRead& Recipe, const KCraftRead& DefaultRecipe);

template<>
BOOL KRecipe<KCraftEnchant>::LoadLine(ITabFile* piTabFile, int nLine, KCraftEnchant& Recipe, const KCraftEnchant& DefaultRecipe);

template<>
BOOL KRecipe<KCraftCopy>::LoadLine(ITabFile* piTabFile, int nLine, KCraftCopy& Copy, const KCraftCopy& DefaultCopy);


int BookID2RecipeID(DWORD dwBookID, DWORD dwSubID);
BOOL RecipeID2BookID(DWORD dwRecipeID, DWORD& dwBookID, DWORD& dwSubID);
#endif

