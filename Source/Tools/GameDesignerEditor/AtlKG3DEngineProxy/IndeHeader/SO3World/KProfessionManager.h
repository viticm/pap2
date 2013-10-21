#ifndef _KPROFESSION_MANAGER_
#define _KPROFESSION_MANAGER_

#include <vector>
#include <map>

#include "Global.h"

class KProfession;
class KCraft;
class KRecipeBase;
class KRecipeMaster;

class KProfessionManager
{
public:
	KProfessionManager(void);
	~KProfessionManager(void);

	BOOL            Init(void);
	void            UnInit(void);
	KProfession*    GetProfession(DWORD dwProfessionID);
	char*           GetProfessionName(DWORD dwProfessionID);
	DWORD           GetProfessionCount();
	BOOL            LoadProfessions();
	BOOL            IsProfessionExist(DWORD dwProfessionID);
	int             GetLevelProficiency(DWORD dwProfessionID, DWORD dwLevel);
	BOOL            GetCraftIDList(DWORD dwProfessionID, std::vector<DWORD>& vecCraftIDList);
	KCraft*         GetCraft(DWORD dwCraftID);
	BOOL            IsCraftExist(DWORD dwCraftID);
	BOOL            LoadCraft();
	BOOL            IsRecipeExist(DWORD dwCraftID, DWORD dwRecipeID);
	KRecipeBase*    GetRecipe(DWORD dwCraftID, DWORD dwRecipeID);
	BOOL            LoadMasterTable();
	KRecipeMaster*  GetRecipeMaster(DWORD dwMasterID);

private:
	KProfession *m_pProfession;

	typedef std::map<DWORD, KProfession*> PROFESSION_LIST;
	PROFESSION_LIST	m_Professions;

	typedef std::map<DWORD, KCraft> CRAFT_LIST;
	CRAFT_LIST m_Craft;

	typedef std::map<DWORD, KRecipeMaster> RECIPE_MASTER_LIST;
	RECIPE_MASTER_LIST m_RecipeMasterList;
};
#endif //_KPROFESSION_MANAGER_
