#ifndef _KGMLIST_H_
#define _KGMLIST_H_

#ifdef WIN32
#include <mbstring.h>
#endif
#include <string>

class KGMList
{
public:
    BOOL Load();
    BOOL CheckPermission(const char cszAccount[]);

private:
    typedef struct
    {
	    bool operator()(const string& X, const string& Y) const
	    {
	        #ifdef WIN32
		    return (_mbsicmp((const unsigned char*)X.c_str(), (const unsigned char*)Y.c_str()) < 0);
		    #else   // linux
		    return (strcasecmp(X.c_str(), Y.c_str()) < 0);
		    #endif
	    }
    } KAccountLess;

    typedef std::set<std::string, KAccountLess> KGM_TABLE;
    KGM_TABLE m_AccountTable;
};

#endif


