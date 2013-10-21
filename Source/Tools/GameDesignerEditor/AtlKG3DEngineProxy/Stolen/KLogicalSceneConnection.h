#pragma once

#include "KG3DToolsImpl.h"
#include "SO3GlobalDef.h"
#include "IEEditor.h"
#include "SO3World/KScene.h"
//#include "../IndeSO3World/Header/KScene.h"

//[Ô­À´ÔÚSceneEditorDoc.h,cpp
class KLogicalSceneConnection : public IELogicalSceneConnection
{
public:
	KScene* m_lpLogicalScene;
	_KG3DLogicalRegionInfo* m_lpLogicalRegionInfo;
	virtual HRESULT BindLogicalScene(KScene* pScene)
	{
		m_lpLogicalScene = pScene;
		return S_OK;
	}
	virtual BOOL CheckObstacle(int nXCell, int nYCell, int nLayer);
	virtual KRegion* GetRegion(int nRegionX, int nRegionY);
	virtual HRESULT FillLogicalCellsInfo(
		D3DXVECTOR3 A,int X,int Z,int& nCellCount,KCell* pCell,IELogicalVertex* pLLine, IELogicalVertex* pLFace, 
		BOOL bTerrian,BOOL bItem, BOOL bSelected, int nMaxCount); 
	virtual KCell* GetCellOfRegion(KRegion* pRegion, INT nIndex);
	virtual FLOAT GetLogicalHeight(const D3DXVECTOR3& vLogicalCoord, size_t* pOrderOfCell);
	virtual KCell* GetCurOrLowerObstacle(const KRegion& regionCur, int nXCell, int nYCell, int nZ, size_t* pOrderOfCell);
	virtual KCell* GetNextCell(const KCell* pCell, BOOL bIgnoreDynamicSwitch);
	virtual KCell* GetLowestObstacle(const KRegion& regionCur, int nXCell, int nYCell);
	virtual POINT LogicalToLogicalRegionIndex( FLOAT fLogicalX, FLOAT fLogicalY );

	KLogicalSceneConnection()
	{
		m_lpLogicalScene = NULL;
		m_lpLogicalRegionInfo = NULL;
	}

	~KLogicalSceneConnection()
	{
		m_lpLogicalScene = NULL;
		SAFE_DELETE_ARRAY(m_lpLogicalRegionInfo);
	}
	HRESULT CreateLogicalRegionInfo();

};
//]