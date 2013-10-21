#pragma once
#ifndef _INCLUDE_KSCENELOGICCELLBUF_H_
#define _INCLUDE_KSCENELOGICCELLBUF_H_

#include "kg3dinterface.h"
#include "../EditInterface/IEEditor.h"

//region的逻辑cell的显示数据缓存
class KG3DLogicRegionCellBuf
{
private:

   BOOL m_bAvailable;

   //这个buf对应的LogicRegion
   int m_nLogicX;
   int m_nLogicZ;

   LPDIRECT3DVERTEXBUFFER9 m_lpLogicalTerrian;
   LPDIRECT3DVERTEXBUFFER9 m_lpLogicalTerrianFace;
   LPDIRECT3DVERTEXBUFFER9 m_lpLogicalItem;
   LPDIRECT3DVERTEXBUFFER9 m_lpLogicalItemFace;

   int m_nCount1;
   int m_nCount2;

public:
   
   KG3DLogicRegionCellBuf();
   
   //是否有效的CellBuf，失效的情况是鼠标抓取的范围不在地图之内
   BOOL IsAvailable();

   //从LogicRegion提取数据
   HRESULT FetchCellDataFromLogicRegion(IELogicalSceneConnection* pLogicalScene, int nRegionX, int nRegionZ);

   //绘制逻辑cell
   void DrawLogicCell(BOOL bTerrian, BOOL bItem, BOOL bSelected);

   virtual ~KG3DLogicRegionCellBuf();
};

#endif