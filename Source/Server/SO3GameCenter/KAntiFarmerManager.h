#ifndef _KANTI_FARMER_H_
#define _KANTI_FARMER_H_

#include <map>

class KRole;

class KAntiFarmerManager
{
public:
    void Clear();

    void Activate();

    BOOL AddFarmerRecord(const char cszAccount[], time_t nPunishEndTime);
    BOOL CheckFarmerLimit(const char cszAccount[]);

    int GetFarmerCount();
    
private:
	struct KNameLess
	{
		bool operator()(const kstring& crA, const kstring& crB) const
		{
            const char* pszA = crA.c_str();
            const char* pszB = crB.c_str();

			return STR_CASE_CMP(pszA, pszB) < 0;
		}
	};

    typedef std::map<kstring, time_t, KNameLess, KMemory::KAllocator<std::pair<kstring, time_t> > >KAntiFarmerTable;
    KAntiFarmerTable m_AntiFarmerTable;
};

#endif // _KANTI_FARMER_H_
