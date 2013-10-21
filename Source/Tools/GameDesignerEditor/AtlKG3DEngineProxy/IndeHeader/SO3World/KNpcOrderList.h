#ifndef _KNPC_ORDER_LIST_H_
#define _KNPC_ORDER_LIST_H_

// -------------------------------------------------------------------------------
// ----------------------- NPC 阵型表 ---------------------------

#include <vector>
#include <map>

// 阵型数据里面的坐标(极坐标)都是阵列成员相对于"王"的坐标,单位: (点, 360/256度)
struct KORDER_NODE 
{
    int nRadius;
    int nAngel;
};

typedef std::vector<KORDER_NODE> KORDER;

class KNpcOrderManager
{
public:
    BOOL Init();
    void UnInit();

    const KORDER* GetOrder(DWORD dwID);

private:
    typedef std::map<DWORD, KORDER> KORDER_LIST;

    KORDER_LIST m_OrderList;

    BOOL LoadOrder(KORDER& rOrder, const char cszFileName[], float fZoom);
};

#endif
