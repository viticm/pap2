#include "EnvironmentConstruct.h"

class TestShortPath : public EnvironmentConstruct 
{
public:
    TestShortPath(void);
    ~TestShortPath(void);

    BOOL AddOpenNode(int nOpenNodeArray[], const int cnNodeCount);
    void ClearOpenNodeList();

    // function to be test
    BOOL ShortestPath(
        int *pnRetMiniCost, int nFromNodeID, int nTargetCity, int nExcludeNodeID,
        const KOPEN_ROUTE_NODE_VECTOER* cpOpenNodeList, KReputation* pReputation
    );

    // Test function
    BOOL Test(
        int nFromNodeID, int nTargetCity, int nExcludeNodeID,
        int nOpenNodeArray[], int nOpenNodeCount, KReputation* pReputation, 
        int nDesireRetValue, int nDesireRetMiniCost
    );

    BOOL TestAll();

private:
    KOPEN_ROUTE_NODE_VECTOER m_OpenNodeVector;

};
