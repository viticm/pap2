#ifndef _KITEM_CONTAINER_H_
#define _KITEM_CONTAINER_H_

struct KBaseItem
{
	int nType;
	int nIndex;
	int nRandomSeed;
	int nGenerateTime;
	DWORD dwPlayerID;		//所属的玩家的ID
};

template <class T>
class KItemContainer
{
public:
	DWORD AddItem(T* pItem);
	BOOL DelItem(DWORD dwItemID);
	T*	 GetFirstItem();
	
	template<class TFunc>
	BOOL TraverseItem(int nStartIndex, int nEndIndex, TFunc Func);

	template<class TFunc>
	BOOL SearchItem(char* pszItemName, TFunc Func);

private:

};

#endif	//_KITEM_CONTAINER_H_