////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KEnchantLib.h
//  Version     : 1.0
//  Creator     : Xia Yong
//  Create Date : 2008-03-05 15:42:00
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KENCHANTLIB_H_
#define _KENCHANTLIB_H_

#include <map>

struct KAttribute;
struct KENCHANT 
{
    KAttribute* pAttribute;
	int         nRepresentIndex;
	int         nRepresentID;
    int         nTime; // 附魔时间,单位: 秒; <=0表示永久附魔
	int         nDestItemSubType;
};

class KEnchantLib
{
public:
    KEnchantLib();
    ~KEnchantLib();

    BOOL Init();
    void UnInit();

    KENCHANT* GetEnchantInfo(int nEnchantID);

private:
    typedef std::map<int, KENCHANT> KENCHANT_MAP;
    KENCHANT_MAP m_Enchant;

private:
    KAttribute* LoadLineAttribute(ITabFile* piFile, int nLine);
};

#endif
