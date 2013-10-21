//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KEntryList.h 
//  Version     :   1.0
//  Creater     :   Zhao ChunFeng
//  Date        :   2007-03-16 10:18:48
//  Comment     :   
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KENTRY_LIST_H_ 
#define _KENTRY_LIST_H_

struct KMapEntry 
{
    int nPosX;
    int nPosY;
    int nPosZ;
    int nDirection;
};

class KEntryList
{
public:
    KEntryList();

    BOOL Init();
    void UnInit();

    const KMapEntry* GetMapEntry(DWORD dwMapID, int nIndex);

private:
    typedef std::map<__int64, KMapEntry> KMAP_ENTRY_LIST;
    KMAP_ENTRY_LIST m_EntryList;
};

#endif

