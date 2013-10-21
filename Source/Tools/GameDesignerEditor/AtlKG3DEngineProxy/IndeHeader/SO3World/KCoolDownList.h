//////////////////////////////////////////////////////////////////////////////////////
//
//  FileName    :   KCoolDownList.h 
//  Version     :   1.0
//  Creater     :   ZhaoChunFeng
//  Date        :   2006-4-21 15:36:23
//  Comment     :   读取Cool down 配置表
//
//////////////////////////////////////////////////////////////////////////////////////

#ifndef _KCOOL_DOWN_LIST_H_
#define _KCOOL_DOWN_LIST_H_

#include <map>

#define MAX_COOL_DOWN_COUNT 512

class KCoolDownList
{
public:
    BOOL Init();
    void UnInit();

    int GetCoolDownValue(DWORD dwCoolDownID); // 返回帧数

private:
    typedef std::map<DWORD, int> KCOOL_DOWN_MAP;
    KCOOL_DOWN_MAP m_CoolDownTable;
};
#endif	//_KCOOL_DOWN_LIST_H_

