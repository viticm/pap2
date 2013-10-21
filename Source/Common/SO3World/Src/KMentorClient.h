#ifndef _MENTOR_CLIENT_H_
#define _MENTOR_CLIENT_H_

#include <map>
#include "Engine/KMemory.h"
#include "KMentorDef.h"

#ifdef _CLIENT
class KMentorClient
{
public:
    BOOL Update(DWORD dwMentor, DWORD dwApprentice, const KMentorRecordBase& crInfo, int* nnMentorValueChange);
    BOOL DeleteRecord(DWORD dwMentor, DWORD dwApprentice);
    void Clear();
    
    BOOL IsMyMentor(DWORD dwPlayerID);
    BOOL IsMyApprentice(DWORD dwPlayerID);

    BOOL IsApprentice();
    BOOL IsMentor();

private:
    typedef KMemory::KAllocator<std::pair<DWORD, KMentorRecordBase> > KMentorTableAlloctor;
    typedef std::map<DWORD, KMentorRecordBase, std::less<DWORD>, KMentorTableAlloctor> KMentorTable;
    KMentorTable m_MentorTable;
    
    typedef KMemory::KAllocator<std::pair<DWORD, KMentorRecordBase> > KApprenticeTableAlloctor;
    typedef std::map<DWORD, KMentorRecordBase, std::less<DWORD>, KApprenticeTableAlloctor> KApprenticeTable;
    KApprenticeTable m_ApprenticeTable;
};
#endif // _CLIENT

#endif // _MENTOR_CLIENT_H_


