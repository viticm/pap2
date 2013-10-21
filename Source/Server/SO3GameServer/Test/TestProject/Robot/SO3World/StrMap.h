//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   StrMap.h 
//  Version     :   1.0
//  Creater     :   ZhaoChunFeng
//  Date        :   2006-05-20 12:47:29
//  Comment     :   将从配置文件读取的字符串映射到程序中的枚举值
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _STRING_MAP_H_
#define _STRING_MAP_H_

#include <map>
#include <string>


// 映射字符串到指定类型的枚举值

#define MAP_STRING_LOCAL(TYPE, szValue, nValue) \
    do                                          \
    {                                           \
        std::map<std::string, TYPE>::const_iterator it = _g_##TYPE##Map.find(szValue);  \
        if (it == _g_##TYPE##Map.end())                                 \
        {                                                               \
            KGLogPrintf(KGLOG_ERR, "Map string %s[\"%s\"] failed -> %s\n", #TYPE, szValue, __FUNCTION__);     \
            goto Exit0;                                                 \
        }                                                               \
        nValue = it->second;                                            \
    } while (false)

#define MAP_STRING_EXTERN(TYPE, szValue, nValue) \
    do                                           \
    {                                            \
        extern std::map<std::string, TYPE> _g_##TYPE##Map;                  \
        std::map<std::string, TYPE>::const_iterator it = _g_##TYPE##Map.find(szValue);  \
        if (it == _g_##TYPE##Map.end())                                 \
        {                                                               \
            KGLogPrintf(KGLOG_ERR, "Map string %s[\"%s\"] failed -> %s\n", #TYPE, szValue, __FUNCTION__);     \
            goto Exit0;                                                 \
        }                                                               \
        nValue = it->second;                                            \
    } while (false)

#define DECLARE_STRING_MAP_BEGIN(TYPE) \
    static const std::map<std::string, TYPE>::value_type _gs_c##TYPE##MapItems[] = \
    {

#define DECLARE_STRING_MAP_END(TYPE) \
    };                               \
    std::map<std::string, TYPE> _g_##TYPE##Map(          \
        _gs_c##TYPE##MapItems,                          \
        _gs_c##TYPE##MapItems + sizeof(_gs_c##TYPE##MapItems) / sizeof(_gs_c##TYPE##MapItems[0])  \
    );

//make_pair("", 0)                 

        
// 下面两个宏都可用于字符串到枚举的映射，前者允许你换个名字，后者映射的名字和原来的枚举值是一样的

#define REGISTER_STR_TO_VALUE(Name, nValue) make_pair(Name, nValue),

#define REGISTER_MAP_VALUE(nValue) make_pair(#nValue, nValue),

/************** 示例 *********************************

读取Tab文件,中的技能伤害类型，策划填的是字符串，而不是枚举的数字:

enum KSKILL_DAMAGE_TYPE
{
    sdtInvalidDamageType = 0,
    sdtPhysicsDamage,         // 外功伤害
    sdtSolarMagicDamage,      // 阳性内功伤害
    sdtNeutralMagicDamage,    // 中性内功伤害
    sdtLunarMagicDamage,      // 阴性内功伤害
    sdtPoisonDamage,          // 毒性伤害
    sdtDamageTypeTotal
};

// 这段声明可以放在其他的某个cpp里面
// 如果这些值只是在程序载入的时候需要做映射的话，也可以写在载入函数里面，省得长期占着内存不用

BOOL LoadSkillConfig()
{
    BOOL bResult = false;
    // ... ...
    char szValue [32];
    int nDamageType = 0;
    
    DECLARE_STRING_MAP_BEGIN(KSKILL_DAMAGE_TYPE)
        REGISTER_STR_TO_VALUE("PhysicsDamage", sdtPhysicsDamage)    
        REGISTER_STR_TO_VALUE("SolarMagicDamage", sdtSolarMagicDamage)
        REGISTER_MAP_VALUE(sdtPoisonDamage) // 这个和上面的差不多，只是枚举映射到字符串时名字没有变
    DECLARE_STRING_MAP_END(KSKILL_DAMAGE_TYPE)

    piTabFile->GetString("DamageType", 1, szValue, sizeof(szValue));

    MAP_STRING_LOCAL(KSKILL_DAMAGE_TYPE, szValue, nDamageType);

    bResult = true;
Exit0:
    return bResult;
}
*****************************************************/


#endif
