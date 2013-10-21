#ifndef _KCRAFT_H_
#define _KCRAFT_H_

#include "Global.h"
#include "KRecipe.h"

#define MAX_CRAFT_NAME_LEN 32

struct KCraftCastState
{
	DWORD dwCraftID;
	DWORD dwRecipeID;
	TARGET_TYPE eTargetType;
	DWORD dwTargetID;
};

class KCraft
{
public:
	KCraft(void);

	BOOL Init(const char* szFileName);
	void UnInit();

	BOOL IsRecipeExist(DWORD dwRecipeID);
	KRecipeBase* GetRecipe(DWORD dwRecipeID);

	CRAFT_RESULT_CODE CanCast(KPlayer* pCaster, DWORD dwRecipeID, DWORD dwSourceItemID, KTarget& Target);
#ifdef _SERVER
	CRAFT_RESULT_CODE Cast(KPlayer* pCaster, DWORD dwRecipeID, DWORD dwSourceItemID, KTarget& Target);
    void CallBeginScriptFunction(KPlayer* pPlayer, DWORD dwRecipeID);
    void CallBreakScriptFunction(KPlayer* pPlayer, DWORD dwRecipeID);
    void CallEndScriptFunction(KPlayer* pPlayer, DWORD dwRecipeID, KItem* pItem = NULL);
#endif //_SERVER
    
    DWORD GetRecipeIDByDoodadTemplateID(DWORD dwDoodadTemplateID);

	DWORD m_dwID;
	DWORD m_dwProfessionID;
	BOOL  m_bHide;
	ALL_CRAFT_TYPE m_eCraftType;
	char m_szName[MAX_CRAFT_NAME_LEN];

protected:
#ifdef _SERVER
	CRAFT_RESULT_CODE Collection(KPlayer* pCaster, KDoodad* pDoodad);		        //采集
	CRAFT_RESULT_CODE Produce(KPlayer* pCaster, DWORD dwRecipeID);			        //合成
	CRAFT_RESULT_CODE MemorizeBook(KPlayer* pCaster, KTarget& rTarget);             //读书,把书本记到脑子里
	CRAFT_RESULT_CODE Enchant(KPlayer* pCaster, DWORD dwRecipeID, KItem* pItem);	//副魔
    CRAFT_RESULT_CODE Copy(KPlayer* pCaster, DWORD dwRecipeID);                     //抄录
#endif //_SERVER
	static int GetModifedExp(int nExp, int nDiffLevel);

private:
	KRecipe<KCraftCollection>   m_CraftCollection;
	KRecipe<KCraftRecipe>       m_CraftRecipe;
	KRecipe<KCraftRead>         m_CraftRead;
	KRecipe<KCraftEnchant>      m_CraftEnchant;
    KRecipe<KCraftCopy>         m_CraftCopy;
};

#endif //_KCRAFT_H_
