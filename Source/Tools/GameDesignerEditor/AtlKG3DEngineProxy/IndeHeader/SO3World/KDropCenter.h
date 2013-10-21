#ifndef _KDROP_CENTER_H_
#define _KDROP_CENTER_H_

#ifdef _SERVER

#include <map>
#include <vector>
#include "Global.h"
#include "KDropList.h"

class KPlayer;
class KLootList;

class KDropCenter
{
public:
	BOOL Init(void);
	BOOL UnInit(void);

	BOOL NpcMapDrop(KLootList* pLootList, std::vector<DWORD>& vecLooterList, DWORD dwMapID);
	BOOL NpcClassDrop(KLootList* pLootList, std::vector<DWORD>& vecLooterList, DWORD dwClassID, int nLevel);
	BOOL NpcTemplateDrop(KLootList* pLootList, std::vector<DWORD>& vecLooterList, DWORD dwNpcTemplateID, int nDropListIndex);

	BOOL DoodadClassDrop(KLootList* pLootList, KPlayer* pOpener, DWORD dwClassID, DWORD dwDropIndex, int nLevel);
	BOOL DoodadTemplateDrop(KLootList* pLootList, KPlayer* pOpener, DWORD dwDoodadTemplateID, DWORD dwDropIndex);

	int NpcDropMoney(DWORD dwNpcTemplateID);
	int DoodadDropMoney(DWORD dwDoodadTempalteID);
    
    BOOL NpcDropQuestItem(DWORD dwTemplateID, KPlayer* pDropTarget, KLootList* pLootList, std::vector<DWORD>& vecLooterList);
    BOOL DoodadDropQuestItem(DWORD dwTemplateID, KPlayer* pDropTarget, KLootList* pLootList);
private:
    BOOL NpcTemplateDropInit(void);
    BOOL NpcClassDropInit(void);
    BOOL MapDropInit(void);
    BOOL DoodadTemplateDropInit(void);
    BOOL DoodadClassDropInit(void);
    BOOL MoneyDropInit(void);

private:
	typedef std::map<DWORD, KDropList>		MAP_DWORD_2_DROP_LIST;
	typedef std::map<uint64_t, KDropList>	MAP_INT64_2_DROP_LIST;

	typedef std::map<DWORD, KMoneyDropList>		MAP_DWORD_2_MONEY_DROP_LIST;
	typedef std::map<uint64_t, KMoneyDropList>	MAP_INT64_2_MONEY_DROP_LIST;

	MAP_DWORD_2_DROP_LIST	m_mapNpcTemplateID2DropList[MAX_DROP_PER_NPC];	
	MAP_DWORD_2_DROP_LIST	m_mapMapID2DropList;
	MAP_INT64_2_DROP_LIST	m_mapClassID2DropList;

	MAP_DWORD_2_DROP_LIST	m_mapDoodadTemplateID2DropList[MAX_DROP_PER_DOODAD];
	MAP_INT64_2_DROP_LIST	m_mapDoodadClassID2DropList[MAX_DROP_PER_DOODAD];
	MAP_INT64_2_DROP_LIST	m_mapDoodadClassMapID2DropList;

	MAP_DWORD_2_MONEY_DROP_LIST m_mapNpcTemplateID2MoneyDropList;
	MAP_INT64_2_MONEY_DROP_LIST m_mapNpcClassID2MoneyDropList;

	MAP_DWORD_2_MONEY_DROP_LIST m_mapDoodadTemplateID2MoneyDropList;
	MAP_INT64_2_MONEY_DROP_LIST m_mapDoodadClassID2MoneyDropList;
    
    BOOL GetNeedQuestItemPlayer(
        IN KItem* pItem, IN DROP_DATA& DropData, IN std::vector<DWORD>& vecLooterList, 
        OUT std::vector<DWORD>& vecNeedQuestItemPlayer
    );
    
    BOOL AddItemToLootListFromNpc(
        KLootList* pLootList, KItem* pItem, DROP_DATA& DropData, std::vector<DWORD>& vecLooterList
    );
    
    BOOL AddItemToLootListFromDoodad(
        KLootList* pLootList, KItem* pItem, DROP_DATA& DropData, KPlayer* pOpener
    );

    void DropQuestItemFromNpc(
        DWORD dwQuestID, DWORD dwTemplateID, KPlayer* pDropTarget, KLootList* pLootList, std::vector<DWORD>& vecLooterList
    );

    void DropQuestItemFromDoodad(
        DWORD dwQuestID, DWORD dwTemplateID, KPlayer* pDropTarget, KLootList* pLootList
    );
};
#endif	//_SERVER
#endif	//_KDROP_CENTER_H_
