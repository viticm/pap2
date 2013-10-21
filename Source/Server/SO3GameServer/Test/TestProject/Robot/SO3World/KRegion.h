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

#define SCRIPT_COUNT_PER_REGION		16

class KSceneObject;
class KScene;
class KPlayer;
class KNpc;
class KDoodad;
class KMissile;
class KCharacter;
class KObjAlloctor;

class KRegion
{
public:
	KRegion();
	~KRegion();

	BOOL Init(KScene* pScene, int nRegionX, int nRegionY);
    void UnInit();

    BOOL ClientLoad();

private:

    BOOL LoadTerrainData(const char cszFileName[], KObjAlloctor& rObjectAlloctor);
    BOOL LoadObjectsData(const char cszFileName[], KObjAlloctor& rObjectAlloctor);

private:

public:
	//本Region中的左下角格子，在整个地图上的坐标（单位：格子）
	int					m_nLeftCell;
	int					m_nBottomCell;

	//本Region中的左下角格子的左下点，在整个地图上的坐标（单位：点）
	int					m_nLeft;
	int					m_nBottom;

	//本Region在整个地图上的坐标（单位：Region）
	int				    m_nRegionX;
	int				    m_nRegionY;

	KScene*				m_pScene;										//指向所属的Scene
	KRegion*			m_pConnectRegion[CONNECTED_REGION_TOTAL];		//相邻的地图指针
	KCell				m_Cells[REGION_GRID_WIDTH][REGION_GRID_HEIGHT];	//格子
	DWORD				m_dwScriptList[SCRIPT_COUNT_PER_REGION];

	int					m_nActive;										//玩家个数

	//添加障碍
	BOOL AddObstacle(int nXCell, int nYCell, KCell* pCell);
	BOOL AddObstacle(int nXCell, int nYCell, KCell::KBaseInfo* pBaseInfo, WORD wLowLayer, WORD wHighLayer);
	// 删除障碍
	KCell* RemoveObstacle(int nXCell, int nYCell, int nZ);

    BOOL GetBaseCellInfo(int nXCell, int nYCell, KCell* pCell);
    BOOL SetBaseCellInfo(int nXCell, int nYCell, KCell& rCell);

    BOOL MergeObstacleData();

    BOOL SetWaterHeight(int nXCell, int nYCell, WORD wWaterHeight);

	// 获取障碍: 下表面 <= nZ <= 上表面
	KCell* GetObstacle(int nXCell, int nYCell, int nZ);

    KCell* GetLowestObstacle(int nXCell, int nYCell);
    KCell* GetTopmostObstacle(int nXCell, int nYCell);

    // 获取Z轴往下第一个低于或等于nZ的Cell, nZ: 绝对坐标,单位:点
	KCell* GetLowerObstacle(int nXCell, int nYCell, int nZ);

    // 从pnRelativeZ输出成功后的Z轴相对坐标,如果没有成功,则不会修改*pnRelativeZ
    // 如果遇到目标Cell是障碍而无法进入,则*pnObstacleDirection返回障碍的法线方向
    // 注意,不见得一定能够得到障碍法线方向,所以要在调此函数前将*pnObstacleDirection设为-1
    // 调用后,如果返回Cell为空,并且*pnObstacleDirection在[0, DIRECTION_COUNT]内,则*pnObstacleDirection才表示障碍方向
    KCell* GetAdaptedCellForObj(
        int nXCell, int nYCell, int nFootPosZ, int nClimb, int nHeight, 
        int* pnRelativeZ, int *pnObstacleDirection
    );
    // 从pnRelativeZ输出成功后的Z轴相对坐标,如果没有成功,则不会修改*pnRelativeZ
    KCell* GetProximalCellForObj(int nXCell, int nYCell, int nFootPosZ, int nHeight, int* pnRelativeZ);

	BOOL CheckObstacle(int nXCell, int nYCell, int nLayer);

	DWORD GetScriptID(int nXCell, int nYCell);
	DWORD GetScriptIDByIndex(DWORD dwIndex);
	BOOL SetScriptID(KCell* pCell, DWORD dwScriptID);
	BOOL SetScriptName(int nXCell, int nYCell, char* pszScriptName);

//内部接口函数
public:

	int	RecycleScriptIndex();

	//传入的参数不清零,只会累加
	BOOL GetNpcCount(int* pnServerCount, int* pnClientCount);
	//传入的参数不清零,只会累加
	BOOL GetDoodadCount(int* pnServerCount, int* pnClientCount);

	inline BOOL	IsActive() {return m_nActive;};

	BOOL AddNpc(KNpc* pNpc);
	BOOL AddDoodad(KDoodad* pDoodad);
	BOOL AddPlayer(KPlayer* pPlayer);

	BOOL RemoveNpc(KNpc* pNpc);
	BOOL RemoveDoodad(KDoodad* pDoodad);
	BOOL RemovePlayer(KPlayer* pPlayer);

	BOOL DeleteAllCell(void);

//存储相关
private:
	enum MAP_FILE_VERSION
	{
		mfvVersion1_0_0 = 1,
		mfvVersion1_0_1,		// 坐标修正
		mfvVersion1_0_2,		// ScriptID存储格式更改
		mfvVersion1_0_3,		// 在存储结构中加入ID,以支持Client only的对象
		mfvCurrentVersion,
	};

	struct KRegionHeader
	{
		int nVersion;
		int nRegionX;
		int nRegionY;
		int nReserved;
	};

	struct KObjHeader
	{
		int nVersion;
		int nRegionX;
		int nRegionY;
		DWORD dwNpcTotal;
		DWORD dwDoodadTotal;
		int nReserved;
	};

	BOOL LoadRegionData(void);

#if defined(_CLIENT)
	BOOL LoadNpcDoodad(void);
#endif	//_CLIENT


};
#endif	//_KREGION_H_

