/************************************************************************/
/* 地图上的Region						                                */
/* Copyright : Kingsoft 2004										    */
/* Author	 : Zhu Jianqiu												*/
/* History	 :															*/
/*		2004.12.30	Create												*/
/* Comment	 :															*/
/*		封装了地图上一个Region的数据和操作								*/
/************************************************************************/
#ifndef _KREGION_H_
#define	_KREGION_H_

#include "Global.h"
#include "KCell.h"
#include "../../Source/Common/SO3World/Src/KSceneObjNode.h"

#define SCRIPT_COUNT_PER_REGION		16
#define SCRIPT_DATA_SIZE (sizeof(DWORD) * SCRIPT_COUNT_PER_REGION)

class KSceneObject;
class KScene;
class KPlayer;
class KNpc;
class KDoodad;
class KMissile;
class KCharacter;

class KRegion
{
public:
	KRegion();
	~KRegion();

	BOOL Init(KScene* pScene, int nRegionX, int nRegionY);
    void UnInit();

	void Activate(void);

    BOOL LoadTerrain();
private:
    BOOL LoadTerrainBufferV7(const BYTE* pbyData, size_t uDataLen);
    BOOL LoadTerrainBufferV6(const BYTE* pbyData, size_t uDataLen);
    
public:
	BOOL SaveTerrain();
private:
    BOOL CheckUpdate(const char cszFileName[], BYTE* pbyNewData, size_t uNewDataLen);

public:
	// 本Region中的左下角格子，在整个地图上的坐标（单位：格子）
	int					m_nLeftCell;
	int					m_nBottomCell;

	// 本Region中的左下角格子的左下点，在整个地图上的坐标（单位：点）
	int					m_nLeft;
	int					m_nBottom;

	// 本Region在整个地图上的坐标（单位：Region）
	int				    m_nRegionX;
	int				    m_nRegionY;

	KScene*				m_pScene;										
    KCell*              m_pCells;
	DWORD*			    m_pdwScriptList;
    BOOL                m_bRegionView;
	int					m_nActiveFrame;

    int                 m_nInFightPlayerCount;

    void SetActive();

    BOOL IsActive();

	// 添加障碍
	BOOL AddObstacle(int nXCell, int nYCell, KCell* pCell);
	BOOL AddObstacle(int nXCell, int nYCell, const KCell& crCellInfo, int nDynamicObstacleGroupIndex);
	// 删除障碍
	KCell* RemoveObstacle(int nXCell, int nYCell, int nZ);

    BOOL GetBaseCellInfo(int nXCell, int nYCell, KCell* pCell);
    BOOL SetBaseCellInfo(int nXCell, int nYCell, KCell& rCell);

    BOOL MergeObstacleData();

    void MergeCellList(KCell* pHead);
    BOOL ProcessDynamicCell(KCell* pHead, const KDynamicCell* pcCell);

    BOOL SetWaterHeight(int nXCell, int nYCell, WORD wWaterHeight);

	// 获取障碍: 下表面 <= nZ <= 上表面
	KCell* GetObstacle(int nXCell, int nYCell, int nZ);

    KCell* GetLowestObstacle(int nXCell, int nYCell);

    // 获取Z轴往下第一个低于或等于nZ的Cell, nZ: 绝对坐标,单位:点
	KCell* GetLowerObstacle(int nXCell, int nYCell, int nZ);

    // 从pnRelativeZ输出成功后的Z轴相对坐标,如果没有成功,则不会修改*pnRelativeZ
    // 如果遇到目标Cell是障碍而无法进入,则*pnObstacleDirection返回障碍的法线方向
    // 注意,不见得一定能够得到障碍法线方向,所以要在调此函数前将*pnObstacleDirection设为-1
    // 调用后,如果返回Cell为空,并且*pnObstacleDirection在[0, DIRECTION_COUNT]内,则*pnObstacleDirection才表示障碍方向
    KCell* GetAdaptedCellForObj(
        int nXCell, int nYCell, int nFootPosZ, int nClimb, 
        int nHeight, BOOL bIgnoreDynamicSwitch,
        int* pnRelativeZ, int *pnObstacleDirection
    );

    // 注意,上面的函数严格计算高度,而下面这个函数是寻找一个最接近的Cell,并不严格匹配高度

    // 从pnRelativeZ输出成功后的Z轴相对坐标,如果没有成功,则不会修改*pnRelativeZ
    KCell* GetProximalCellForObj(
        int nXCell, int nYCell, int nFootPosZ, int nHeight, 
        BOOL bIgnoreDynamicSwitch, 
        BOOL bIgnoreBlock, 
        int* pnRelativeZ
    );

    // 取得Cell链表中的上一个Cell,内部会动态障碍的开关
    KCell* GetPreCell(int nCellX, int nCellY, KCell* pCell);
    // 取得下一个有效的Cell,内部会考虑动态障碍开关
    KCell* GetNextCell(KCell* pCell, BOOL bIgnoreDynamicSwitch);

	BOOL CheckObstacle(int nXCell, int nYCell, int nLayer);

public:
	DWORD GetScriptIDByIndex(DWORD dwIndex);

	BOOL SetScriptID(KCell* pCell, DWORD dwScriptID);

	int	RecycleScriptIndex();

    BOOL ClearAllScript();
    BOOL ClearScriptSpecial(DWORD dwScriptID);

public:
	BOOL AddNpc(KNpc* pNpc);
	BOOL AddDoodad(KDoodad* pDoodad);
	BOOL AddPlayer(KPlayer* pPlayer);

	BOOL RemoveNpc(KNpc* pNpc);
	BOOL RemoveDoodad(KDoodad* pDoodad);
	BOOL RemovePlayer(KPlayer* pPlayer);

	// 遍历Region中的场景物体
	template <class TFunc> BOOL TraverseNpc(TFunc& Func);
	template <class TFunc> BOOL TraverseDoodad(TFunc& Func);
	template <class TFunc> BOOL TraversePlayer(TFunc& Func);
	// 遍历Region中的Cell
	template <class TFunc> BOOL TraverseCell(TFunc& Func);

	BOOL DeleteAllNpc(void);
	BOOL DeleteAllDoodad(void);
	BOOL DeleteAllPlayer(void);
	BOOL DeleteAllObject(void);
	BOOL DeleteAllCell(void);

public:
	KList			m_NpcList;			
	KList			m_DoodadList;		
	KList			m_PlayerList;		

private:
	enum KTERRAIN_DATA_VERSION
	{
        eTerrainDataVersion7 = 7
	};

	struct KRegionHeader
	{
		int nVersion;
		int nRegionX;
		int nRegionY;
		int nReserved;
	};

	BOOL LoadRegionData(void);

#if defined(_CLIENT)
	KCell   m_Cells[REGION_GRID_WIDTH * REGION_GRID_HEIGHT];	
    DWORD   m_dwScriptList[SCRIPT_COUNT_PER_REGION];
#endif
};

// ------------------------ 遍历Region中的场景物体 ----------------------------------->

template <class TFunc> 
BOOL KRegion::TraverseNpc(TFunc& Func)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

	KSceneObjNode* pNode = (KSceneObjNode*)m_NpcList.GetHead();
	while (pNode != NULL)
	{
		KNpc* pNpc = (KNpc*)(pNode->m_pSceneObject);

		bRetCode = Func(pNpc);
		KG_PROCESS_ERROR(bRetCode);

		pNode = (KSceneObjNode*)pNode->GetNext();
	}

    bResult = true;
Exit0:
	return bResult;
}

template <class TFunc> 
BOOL KRegion::TraverseDoodad(TFunc& Func)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

	KSceneObjNode* pNode = (KSceneObjNode*)m_DoodadList.GetHead();
	while (pNode != NULL)
	{
		KDoodad* pDoodad = (KDoodad*)(pNode->m_pSceneObject);

		bRetCode = Func(pDoodad);
		KG_PROCESS_ERROR(bRetCode);

		pNode = (KSceneObjNode*)pNode->GetNext();
	}

    bResult = true;
Exit0:
	return bResult;
}

template <class TFunc> 
BOOL KRegion::TraversePlayer(TFunc& Func)
{
    BOOL bResult  = false;
    BOOL bRetCode = false;

	KSceneObjNode* pNode = (KSceneObjNode*)m_PlayerList.GetHead();
	while (pNode != NULL)
	{
		KPlayer* pPlayer = (KPlayer*)(pNode->m_pSceneObject);

		bRetCode = Func(pPlayer);
		KG_PROCESS_ERROR(bRetCode);

		pNode = (KSceneObjNode*)pNode->GetNext();
	}

    bResult = true;
Exit0:
	return bResult;
}

template <class TFunc>
BOOL KRegion::TraverseCell(TFunc& Func)
{
	BOOL bResult  = false;
	BOOL bRetCode = false;

    for (int nY = 0; nY < REGION_GRID_HEIGHT; nY++)
    {
        for (int nX = 0; nX < REGION_GRID_WIDTH; nX++)
        {
            KCell* pCell = GetLowestObstacle(nX, nY);
	        while (pCell != NULL)
	        {
		        bRetCode = Func(pCell);
		        KG_PROCESS_ERROR(bRetCode); // 返回false，则不继续遍历

		        pCell = pCell->m_pNext;
	        }
        }
    }

	bResult = true;
Exit0:
	return bResult;
}

// <-------------------------- 遍历Region中的场景物体 -----------------------------------

// 遍历Region中的所有Player,通知"有新Npc"
struct KRegisterNewNpcFunc 
{
    KRegisterNewNpcFunc(KNpc* pNpc) : m_pNpc(pNpc) {};
    BOOL operator () (KPlayer* pPlayer);
    KNpc* m_pNpc;
};

// 遍历Region中的所有Player,通知"有新Doodad"
struct KRegisterNewDoodadFunc
{
    KRegisterNewDoodadFunc(KDoodad* pDoodad) : m_pDoodad(pDoodad) {};
    BOOL operator () (KPlayer* pPlayer);
    KDoodad* m_pDoodad;
};

#endif	//_KREGION_H_

