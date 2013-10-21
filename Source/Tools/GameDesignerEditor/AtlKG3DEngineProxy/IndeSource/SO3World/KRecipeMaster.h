#ifndef _KRECIPE_MASTER_
#define _KRECIPE_MASTER_

#include <map>
#include <vector>
#include "Global.h"

struct KRecipeLearningInfo 
{
	DWORD	dwCraftID;
	DWORD	dwRecipeID;
	DWORD   dwProfessionID;
	int		nProfessionLevel;
	DWORD	dwBranchID;
	DWORD   dwReputationID;
	int     nReputationLevel;
	int     nPrice;
};

class KRecipeMaster
{
public:
	KRecipeMaster(void);
	~KRecipeMaster(void);

	BOOL Init(DWORD dwMasterID, char szFileName[]);
	void UnInit();
	BOOL LoadLine(int nLine, ITabFile* piTabFile, KRecipeLearningInfo& DefaultLearningInfo, KRecipeLearningInfo& LearnInfo);
	void GetLearnInfoList(std::vector<KRecipeLearningInfo>& vecLearnInfoList);

	KRecipeLearningInfo* GetLearnInfo(DWORD dwCraftID, DWORD dwRecipeID);

private:
	typedef std::map<int64_t, KRecipeLearningInfo> KRECIPE_MASTER;
	KRECIPE_MASTER m_mapRecipeMaster;

	DWORD m_dwMasterID;
};

#endif //_KRECIPE_MASTER_
