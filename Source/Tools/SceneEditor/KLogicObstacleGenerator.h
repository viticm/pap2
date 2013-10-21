////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KLogicObstacleGenerator.h
//  Version     : 1.0
//  Creator     : Zhao Chunfeng
//  Create Date : 2007-02-28 18:01:31
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _KLOGIC_OBSTACLE_GENERATOR_H_
#define _KLOGIC_OBSTACLE_GENERATOR_H_

#include "KDlgLogicSaveProgress.h"
struct IEKG3DRepresentPoly;

class KLogicObstacleGenerator
{
public:
    KLogicObstacleGenerator();
    BOOL GenerateSceneLogicObstacleData(IEKG3DScene* pi3DScene, KScene* pLogicScene,BOOL bOnlyAppointCell,BOOL bClearCell);

    BOOL GenerateRegionLogicObstacleData(
        IEKG3DScene* pi3DScene, KRegion* pLogicRegion,
		BOOL bClearCell,float fSceneX_Start,float fSceneZ_Start
    );

private:
	BOOL IsCellInPoly(float CenterX,float CenterZ,KCell* pCell,IEKG3DRepresentPoly* pPoly,BOOL bGroundCell);

	BOOL ProcessRegionObstacleWithPoly(BOOL bClearCell,float fSceneX_Start,float fSceneZ_Start);//和场景中的逻辑多边形进行检查，批量生成障碍信息

    BOOL GenerateRegionTerrainObstacle(float fSceneX_Start,float fSceneZ_Start);
    BOOL GenerateRegionEntityObstacle(float fSceneX_Start,float fSceneZ_Start);

    static BOOL TraverseEntityMeshCallback(
        float fXStart, float fYStart, float fZStart, 
        LPD3DXMESH piMesh, int nNumSubset,BOOL bSubsetNoCell[],
        const D3DXMATRIX& crTransformationMatrix,
        int nDynamicObstacleGroupIndex,LPSTR pMeshName
    );

    BOOL GenerateRegionWaterObstacle(float fSceneX_Start,float fSceneZ_Start);

	HRESULT ProcessCellObstacleWithPolys(int nPoly,IEKG3DRepresentPoly* PolyArray[],
		KCell* pCell,float fCenterX,float fCenterY,BOOL bGroundCell,BOOL bClearCell);


private:
    IEKG3DScene*            m_pi3DScene;
    KScene*                 m_pLogicScene;
    KRegion*                m_pLogicRegion;
    KDlgLogicSaveProgress*  m_pProgressBar;
};

#endif
