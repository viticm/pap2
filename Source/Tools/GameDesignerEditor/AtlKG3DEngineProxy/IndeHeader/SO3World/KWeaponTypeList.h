#ifndef _KWEAPONTYPELIST_H_
#define _KWEAPONTYPELIST_H_
/*
武器的分类,按照武器的使用分门别类:
本分类的元素并不是具体的某个武器,而是WEAPON_DETAIL_TYPE.
WEAPON_DETAIL_TYPE 用于说明某个武器是刀枪棍棒...中的哪一种.
而本分类旨在描述技能对武器的分类需求.
比如:
某技能需要第1类武器,1类武器包括: 枪,棍,棒
某技能需要第2类武器,2类武器包括: 刀,剑
*/

#include <map>
#include <vector>

class KWeaponTypeList
{
public:
    BOOL Init();
    void UnInit();

    BOOL CheckType(DWORD dwTypeID, int eDetailType);

    int  GetWeaponGenre(DWORD dwTypeID);

private:
    typedef std::vector<int> KDETAIL_TYPE_TABLE;

    struct KWEAPON_REQUEST_INFO
    {
        int                 eWeaponGenre;     // 表示是近程还是远程 
        KDETAIL_TYPE_TABLE  DetailTypeTable;  // 表示刀,剑,枪等详细类型
    };

    typedef std::map<DWORD, KWEAPON_REQUEST_INFO> KTYPE_LIST;
    KTYPE_LIST m_TypeList;

    BOOL LoadLine(ITabFile* piTabFile, int nLine);
};

#endif

