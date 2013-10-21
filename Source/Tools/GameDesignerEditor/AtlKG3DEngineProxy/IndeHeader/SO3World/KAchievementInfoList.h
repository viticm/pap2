#ifndef _KACHIEVEMENT_INFO_LIST_H_
#define _KACHIEVEMENT_INFO_LIST_H_

struct KACHIEVEMENT_INFO 
{
    int nPoint;
    int nExp;
};

class KAchievementInfoList
{
public:
    KAchievementInfoList();
    ~KAchievementInfoList();

    BOOL Init();
    void UnInit();

    KACHIEVEMENT_INFO* GetInfo(int nAchievementID);

private:
    typedef std::map<int, KACHIEVEMENT_INFO> KINFO_LIST;
    KINFO_LIST m_InfoList;
};
#endif
