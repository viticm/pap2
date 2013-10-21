#ifndef KRLPROFILE_H
#define KRLPROFILE_H

enum KRLPROFILE
{
    KRLPROFILE_MAIN,
    KRLPROFILE_SKILL_EFFECT_RESULT,
    KRLPROFILE_MISSILE,
    KRLPROFILE_CHARACTER,
    KRLPROFILE_RIDES,
    KRLPROFILE_DOODAD,
    KRLPROFILE_CHARACTER_GC,
    KRLPROFILE_RIDES_GC,
    KRLPROFILE_DOODAD_GC,
    KRLPROFILE_CAMERA,
    KRLPROFILE_CHARACTER_CAMERA,
    KRLPROFILE_CHARACTER_ALPHA,
    KRLPROFILE_TRACK_CAMERA,
    KRLPROFILE_CURSOR,
    KRLPROFILE_TARGET,
    KRLPROFILE_MODEL_GC,
    KRLPROFILE_REGION_INFO,
    KRLPROFILE_OPTIMIZATION,
    KRLPROFILE_RESOURCE_LIMIT,

    KRLPROFILE_COUNT,
};

struct KRLProfileItem
{
    LPCTSTR szName;
    int nCount;
    double fTotalTime;
    double fStartTime;
};

struct KRLProfile
{
    BOOL bEnable;
    DWORD dwPrintCount;
    KRLProfileItem Items[KRLPROFILE_COUNT];
};

int InitProfile(KRLProfile* pRLProfile);
int UnInitProfile(KRLProfile* pRLProfile);
int ResetProfile(KRLProfile* pRLProfile);
int PrintProfile(KRLProfile* pRLProfile);

int StartProfile(KRLProfile* pRLProfile, KRLPROFILE nIndex);
int StopProfile(KRLProfile* pRLProfile, KRLPROFILE nIndex);

#endif // KRLPROFILE_H

