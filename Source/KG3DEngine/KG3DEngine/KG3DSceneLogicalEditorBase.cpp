#include "StdAfx.h"
#include "KG3DSceneLogicalEditorBase.h"
#include "Klist.h"
#include "../include/SO3World/KCell.h"
#include "../include/SO3World/KRegion.h"
#include "../Include/SO3World/KScene.h"
#include "Terrain/KG3DTerrainEx.h"

#ifdef _DEBUG
#define new DEBUG_NEW_3DENGINE
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define TODWORD(baseinfo) (*(DWORD*)baseinfo)
const static short ScriptIDCount = 0xf;

KG3DSceneLogicalEditorBase::CellInfo::CellInfo(const CellInfo& src)
{
	//使用默认的COPY构造函数
	*this = src;
}

KG3DSceneLogicalEditorBase::CellInfo::CellInfo()
{
	vPosition = D3DXVECTOR3(0,0,0);
	fTop = 0;
	pCell = NULL;
	bSelected = FALSE;
	pRegion = NULL;
	byX = 0;
	byY = 0;
}

KG3DSceneLogicalEditorBase::CellInfo::CellInfo(bool abEditGroundCells, Terrain::KG3DTerrain* pTerrain,int nX,int nZ,KCell* pCellInput, KRegion* argpRegion, BYTE argbyX, BYTE argbyY)
{
	pCell = pCellInput;
	//vPostion 保存逻辑格式，左下角的点坐标
	vPosition.x = pTerrain->m_BBox.A.x + nX * LOGICAL_CELL_CM_LENGTH; //nX 整个地图上X方向上的逻辑序号, vPoition.x = 从地图左下角起计算的坐标值
	vPosition.z = pTerrain->m_BBox.A.z + nZ * LOGICAL_CELL_CM_LENGTH;

	pTerrain->GetGroundHeight(H[0], vPosition.x, vPosition.z);
	pTerrain->GetGroundHeight(H[1], vPosition.x + LOGICAL_CELL_CM_LENGTH, vPosition.z);
	pTerrain->GetGroundHeight(H[2], vPosition.x + LOGICAL_CELL_CM_LENGTH, vPosition.z + LOGICAL_CELL_CM_LENGTH);
	pTerrain->GetGroundHeight(H[3], vPosition.x, vPosition.z + LOGICAL_CELL_CM_LENGTH);
	
	vPosition.y = max(H[0] - 50, pCell->m_wLowLayer  * ALTITUDE_UNIT + TERRAIN_MIN_HEIGHT);

	fTop       = pCell->m_wHighLayer * ALTITUDE_UNIT + TERRAIN_MIN_HEIGHT; 
	bSelected = FALSE;
	BOOL bGourndCell = abEditGroundCells;
	if (abEditGroundCells)
	{
		//如果此logic cell高度 <= 当前地形格最高点，则认为没有与物件合并过，主要考虑斜坡情况
		bGourndCell = fTop <= max(H[0], max(H[1], max(H[2], H[3])));
	}
	if (!bGourndCell)
	{
		H[0] = H[1] = H[2] = H[3] = fTop; //为解决非地表物件显示不出逻辑格子问题
	}

	vBBox.Clear();
	vBBox.AddPosition(vPosition);
	D3DXVECTOR3 Top = vPosition + D3DXVECTOR3(LOGICAL_CELL_CM_LENGTH,0,LOGICAL_CELL_CM_LENGTH);
	Top.y = fTop;
	vBBox.AddPosition(Top);
	pRegion = argpRegion;
	byX = argbyX;
	byY = argbyY;
}

BOOL KG3DSceneLogicalEditorBase::CellInfo::IsRayIntersect(float& fDistance,D3DXVECTOR3& CameraPos,D3DXVECTOR3& CameraDirct)
{
	D3DXVECTOR3 BoxPos[8];
	vBBox.Extract(BoxPos);

	D3DXVECTOR3 Inter(0,0,0);

	if(IsLineIntersectBox(&Inter,&CameraPos,&(CameraPos+CameraDirct*10000),BoxPos))
	{
		D3DXVECTOR3 V = Inter - CameraPos;
		float Len = D3DXVec3Length(&V);
		
		fDistance = Len;
		return TRUE;
	}
	return FALSE;
}

KG3DSceneLogicalEditorBase::KG3DSceneLogicalEditorBase(void) 
	: m_d3dclrCurrColor(0x80FF8080), m_dwCurrentScriptID(0), m_nBrushWidth(5), m_nDisplayWidth(1), m_nCellHorzHeight(1), m_eBrushType(cbRectangle)
{
	m_pLogicalScene = NULL;
	m_pCellVB = NULL;
	m_pCellIB = NULL;
	m_bEditGroundCells = TRUE;
	m_bEditOverGroundCells = TRUE;
	m_nCurRegionX = -1;
	m_nCurRegionZ = -1;
	m_dwLogicalCellModifyState = MODIFY_NONE;
	m_dwLogicalCellShowState = MODIFY_NONE;
	m_bAddCellInfo = TRUE;
}

KG3DSceneLogicalEditorBase::~KG3DSceneLogicalEditorBase(void)
{
	UnInit();
}

HRESULT KG3DSceneLogicalEditorBase::Init()
{
	SAFE_RELEASE(m_pCellVB);
	SAFE_RELEASE(m_pCellIB);
	m_pLogicalScene = NULL;

	m_listLogicalCellSelectableGround.clear();
	m_listLogicalCellSelectableOverGround.clear();
	m_listLogicalCellSelected.clear();
	m_listLogicalRegionEditing.clear();

	CreateBuffersForRenderCell();
	return S_OK;
}

HRESULT KG3DSceneLogicalEditorBase::UnInit()
{
	SAFE_RELEASE(m_pCellVB);
	SAFE_RELEASE(m_pCellIB);

	m_pLogicalScene = NULL;
	m_listLogicalCellSelectableGround.clear();
	m_listLogicalCellSelectableOverGround.clear();
	m_listLogicalCellSelected.clear();
	m_listLogicalRegionEditing.clear();
	m_nCurRegionX = -1;
	m_nCurRegionZ = -1;
	return S_OK;
}

HRESULT KG3DSceneLogicalEditorBase::SetLogicalScene(KScene* pLogicalScene)
{
	m_pLogicalScene = pLogicalScene;
	return S_OK;
}

KScene* KG3DSceneLogicalEditorBase::GetLogicalScene()
{
	return m_pLogicalScene;
}

HRESULT KG3DSceneLogicalEditorBase::SetModifyState(eCellModifyState eState,BOOL bModify)
{
	if(bModify)
	{
		m_dwLogicalCellModifyState |= eState;
	}
	else
	{
		m_dwLogicalCellModifyState &= (~eState);
	}
	return S_OK;
}

BOOL KG3DSceneLogicalEditorBase::GetModifyState(eCellModifyState eState)
{
	return(m_dwLogicalCellModifyState&eState);
}

DWORD GetIDFromFileName(LPCTSTR lpstrFileName)
{
	DWORD dwRet = 0;
	TCHAR szFileName[MAX_PATH + 1];
	if (!lpstrFileName ||strlen(lpstrFileName) == 0)
	{
		dwRet = 0;
	}
	else if (g_GetFilePathFromFullPath(szFileName, lpstrFileName))
	{
		dwRet = g_FileNameHash(szFileName);
	}
	else
	{
		KGLOG_PROCESS_ERROR(0); //不能失败，否则脚本设为空
		dwRet = 0;
	}
Exit0:
	;
	return dwRet;
}

//将文件名转为ID
void KG3DSceneLogicalEditorBase::SetCurrentScriptFileName(LPCTSTR lpstrFileName)
{
	m_dwCurrentScriptID = GetIDFromFileName(lpstrFileName);
}

HRESULT KG3DSceneLogicalEditorBase::SetShowState(eCellModifyState eState,BOOL bModify)
{
	if(bModify)
	{
		m_dwLogicalCellShowState |= eState;
	}
	else
	{
		m_dwLogicalCellShowState &= (~eState);
	}
	//(by dengzhihui 2006年7月26日 15:55:31
	//RecalcShowCell();
	//)
	return S_OK;
}

BOOL KG3DSceneLogicalEditorBase::GetShowState(eCellModifyState eState)
{
	return(m_dwLogicalCellShowState&eState);
}

void KG3DSceneLogicalEditorBase::GetMapSize(int* pMapWidth, int* pMapHeight)
{
	int& nWidth = *pMapWidth;
	for(; nWidth < MAX_REGION_WIDTH; nWidth ++)
	{
		KRegion* pRegion = m_pLogicalScene->GetRegion(nWidth, 0);
		if (!pRegion) break;
	}
	int& nHeight = *pMapHeight;
	for (; nHeight < MAX_REGION_HEIGHT; nHeight ++)
	{
		KRegion* pRegion = m_pLogicalScene->GetRegion(0, nHeight);
		if (!pRegion) break;
	}	
}

HRESULT KG3DSceneLogicalEditorBase::UpdateEditingCells(Terrain::KG3DTerrainEx* pTerrain,D3DXVECTOR3 CameraPos)//更新编辑的逻辑格子
{
	D3DXVECTOR3 InterPos;

	KGLOG_PROCESS_ERROR(m_pLogicalScene);
	KGLOG_PROCESS_ERROR(pTerrain);

	pTerrain->GetIntersectionPoint(InterPos);
	{
		int nXcm = (int)(InterPos.x - pTerrain->m_BBox.A.x);
		int nZcm = (int)(InterPos.z - pTerrain->m_BBox.A.z);
		int nRegionX = nXcm/LOGICAL_CELL_CM_LENGTH/REGION_GRID_WIDTH;
		int nRegionZ = nZcm/LOGICAL_CELL_CM_LENGTH/REGION_GRID_HEIGHT;

		if((nRegionX==m_nCurRegionX)&&(nRegionZ==m_nCurRegionZ))
			return S_OK;

		m_nCurRegionX = nRegionX;
		m_nCurRegionZ = nRegionZ;

		// 如果比整张地图还大，则不需要更新显示区域
		//先拿到整张地图的大小
		int nMapWidth = 0;
		int nMapHeight = 0;
		GetMapSize(&nMapWidth, &nMapHeight);
		if (m_nXStart <= 0 && m_nXEnd >= nMapWidth 
			&& m_nZStart <= 0 && m_nZEnd >= nMapHeight)
			return S_OK;

		int nRegionXStart = max(nRegionX - (m_nDisplayWidth - 1), 0);
		int nRegionXEnd   = nRegionX + (m_nDisplayWidth - 1);
		int nRegionZStart = max(nRegionZ - (m_nDisplayWidth - 1), 0);
		int nRegionZEnd   = nRegionZ + (m_nDisplayWidth - 1);
		/*
		//先完成 1 个 Region 的功能，再作优化
		int nRegionXStart = nRegionX;
		int nRegionXEnd   = nRegionX;
		int nRegionZStart = nRegionZ;
		int nRegionZEnd   = nRegionZ;
		*/

		m_nXStart = nRegionXStart;
		m_nXEnd   = nRegionXEnd;
		m_nZStart = nRegionZStart;
		m_nZEnd   = nRegionZEnd; 

		m_listLogicalRegionEditing.clear();
		m_listLogicalCellSelectableGround.clear();
		m_listLogicalCellSelectableOverGround.clear();
		m_listLogicalCellSelected.clear();


		for(int z=nRegionZStart;z<=nRegionZEnd;z++)
		{
			for (int x=nRegionXStart;x<=nRegionXEnd;x++)
			{
				KRegion* pRegion = m_pLogicalScene->GetRegion(x,z);
				if(pRegion)
				{
					m_listLogicalRegionEditing.push_back(pRegion);
				}
			}
		}

	}
	//////////////////////////////////////////////////////////////////////////
	//Get Selectable Cell
	{
		list<KRegion*>::iterator i = m_listLogicalRegionEditing.begin();
		while (i!=m_listLogicalRegionEditing.end())
		{
			KRegion* pRegion = *i;
			for(BYTE Z=0;Z<REGION_GRID_HEIGHT;Z++)
			{
				for(BYTE X=0;X<REGION_GRID_WIDTH;X++)
				{
					KCell* pCell = &pRegion->m_Cells[X][Z];

					if(m_bEditGroundCells)
					{
						CellInfo cellinfo(true, pTerrain,
							X+pRegion->m_nRegionX*REGION_GRID_WIDTH,    //m_dwRegionX 在当前逻辑Region中的x序号
							Z+pRegion->m_nRegionY*REGION_GRID_HEIGHT,
							pCell, pRegion, X, Z);


						m_listLogicalCellSelectableGround.push_back(cellinfo);
					}
					if(m_bEditOverGroundCells)
					{
						pCell = pCell->m_pNext;
						while (pCell)
						{
							CellInfo cellinfo(false, pTerrain,
								X+pRegion->m_nRegionX*REGION_GRID_WIDTH,
								Z+pRegion->m_nRegionY*REGION_GRID_HEIGHT,
								pCell, pRegion, X, Z);


							m_listLogicalCellSelectableOverGround.push_back(cellinfo);

							pCell = pCell->m_pNext;
						}
					}
				}
			}
			i++;
		}
	}
	//(by dengzhihui 2006年7月26日 15:55:31
	RecalcShowCell();
	//)

	return S_OK;
Exit0:
	return E_FAIL;
}

//(by dengzhihui 2006年7月27日 9:08:57
void KG3DSceneLogicalEditorBase::RecalcShowCell()
{
	m_listLogicalCellCurrMarked.clear();
	m_listLogicalCellDiffMarked.clear();
	GetShowCellList(&m_listLogicalCellCurrMarked, &m_listLogicalCellDiffMarked, m_listLogicalCellSelectableGround);
	GetShowCellList(&m_listLogicalCellCurrMarked, &m_listLogicalCellDiffMarked, m_listLogicalCellSelectableOverGround);
}

void KG3DSceneLogicalEditorBase::GetCurrentModifyMask(DWORD dwCurrentState, DWORD* pDes)
{
	KCell::KBaseInfo stCellNullMask;
	KCell::KBaseInfo& stCellCurrModifyMask = *(KCell::KBaseInfo*)pDes;
	memset(&stCellNullMask, 0x0, sizeof(KCell::KBaseInfo));
	
	CopyMemory(&stCellCurrModifyMask, &stCellNullMask, sizeof(KCell::KBaseInfo));
	if(dwCurrentState&MODIFY_BLOCKCHARACTER)
		stCellCurrModifyMask.m_bBlockCharacter = m_bAddCellInfo;
	if(dwCurrentState&MODIFY_PUTOBJECT)
		stCellCurrModifyMask.m_bPutObj = m_bAddCellInfo;
	if(dwCurrentState&MODIFY_SAFE)
		stCellCurrModifyMask.m_bRest = m_bAddCellInfo;
	if(dwCurrentState&MODIFY_STALL)
		stCellCurrModifyMask.m_bStall = m_bAddCellInfo;

	if(dwCurrentState&MODIFY_INDOOR)
		stCellCurrModifyMask.m_bIndoor = m_bAddCellInfo;
	if(dwCurrentState&MODIFY_REST)
		stCellCurrModifyMask.m_bRest = m_bAddCellInfo;
	if(dwCurrentState&MODIFY_RIDEHORSE)
		stCellCurrModifyMask.m_bRideHorse = m_bAddCellInfo;
	//@
	if(dwCurrentState&MODIFY_SCRIPTFILE)
		stCellCurrModifyMask.m_ScriptIndex = 0;//dwScriptID;
}

//scriptid位置0，其他位置 1
inline DWORD GetScriptIDMask()
{
	DWORD dwRet = 0xffffffff;
	KCell::KBaseInfo& stCellReservedMask = *(KCell::KBaseInfo*)&dwRet;
	stCellReservedMask.m_ScriptIndex = 0;
	return dwRet;
}
//保留位置0，其他位置 1
DWORD GetReserveMask()
{
	DWORD dwRet = 0xffffffff;
	KCell::KBaseInfo& stCellReservedMask = *(KCell::KBaseInfo*)&dwRet;
	stCellReservedMask.m_NoObstacleRange = 0;
	stCellReservedMask.m_FaceUp	= 0;
	stCellReservedMask.m_FaceDown	= 0;
	stCellReservedMask.m_Reserved	= 0;
	stCellReservedMask.m_SlipDirection = 0; //8 为默认滑动方向,滑动方向暂不处理
	stCellReservedMask.m_PassWidth = 0;
	stCellReservedMask.m_SlipVelocity = 0;	
	return dwRet;
}

inline void FindScriptIDbyRegion(KRegion* pRegion, DWORD dwOldScriptID, DWORD dwNewScriptID, 
							DWORD* pdwFindOldIndex, DWORD* pdwFindNewIndex)
{
	if (dwOldScriptID == 0 && pdwFindOldIndex)
		*pdwFindOldIndex = 0;
	if (dwNewScriptID == 0 && pdwFindNewIndex)
		*pdwFindNewIndex = 0;
	if (dwNewScriptID == 0 && dwOldScriptID == 0)
		return;
	for(int nSciprtIndex = 0; nSciprtIndex <= ScriptIDCount; nSciprtIndex++)
	{
		//索引ID在每个REGION中是不同的, 需要重新放进mask中一次
		//将当前region对应的 oldmask 与 newmask 作好设置
		if (dwOldScriptID != 0 && pdwFindOldIndex && pRegion->GetScriptIDByIndex(nSciprtIndex) == dwOldScriptID)					
			*pdwFindOldIndex = nSciprtIndex;
		else if (dwNewScriptID != 0 && pdwFindNewIndex && pRegion->GetScriptIDByIndex(nSciprtIndex) == dwNewScriptID )
			*pdwFindNewIndex = nSciprtIndex;
	}
}

void KG3DSceneLogicalEditorBase::DeleteCurrentCellInfo()//long lLogicInfo, LPCTSTR lpstrScriptFileName)
{
	//if (lLogicInfo == 0)
	//	return;
	DWORD dwCurrentMask = 0;
	GetCurrentModifyMask(m_dwLogicalCellShowState, &dwCurrentMask);
	KCell::KBaseInfo& stCellCurrModifyMask = *(KCell::KBaseInfo*)&dwCurrentMask;
	DWORD dwReservedMask = GetReserveMask();
	DWORD dwSetInReservedMask = ~dwReservedMask;
	//遍历全地图所有的cell，将与lLogicInfo相同置的cell内容全部清空
	DWORD* pBaseInfo;
	for(int i = 0; i < MAX_REGION_WIDTH; i ++)
		for (int j = 0; j < MAX_REGION_HEIGHT; j ++)
		{
			KRegion* pRegion = m_pLogicalScene->GetRegion(i, j);
			if (!pRegion) continue;
			DWORD dwCurrRegionScriptIndex = 0;
			FindScriptIDbyRegion(pRegion, m_dwCurrentScriptID, 0, &dwCurrRegionScriptIndex, NULL);
			if (dwCurrRegionScriptIndex == 0 && m_dwCurrentScriptID != 0) //本region中没有要找的script设置
				continue;
			((KCell::KBaseInfo*)&dwCurrentMask)->m_ScriptIndex = dwCurrRegionScriptIndex;
			if (pRegion)
			{
				for(int Z=0;Z<REGION_GRID_HEIGHT;Z++)
					for(int X=0;X<REGION_GRID_WIDTH;X++)
					{
						KCell* pCell = &pRegion->m_Cells[X][Z];
						while (pCell)
						{	//这里先把baseinfo转为32字节的dword,再直接作位处理会比较快
							pBaseInfo = (DWORD*)&(pCell->m_BaseInfo);
							if((*pBaseInfo & dwReservedMask) == dwCurrentMask)
							{
								*pBaseInfo &= dwSetInReservedMask; //除保留位，其他位置0
							}
							pCell = pCell->m_pNext;
						}
					}
			}
			else
				continue;
		}
	//将当前编辑标志清空
	m_dwCurrentScriptID = 0;
	m_dwLogicalCellShowState = 0;
	m_dwLogicalCellModifyState = 0;
}

inline void SetScriptID(KRegion* pRegion, KCell* pCell, DWORD dwNewScriptID)
{
	if (!pRegion->SetScriptID(pCell, dwNewScriptID)) //脚本空间溢出
	{
		if (!pRegion->RecycleScriptIndex())
		{
			KGLOG_PROCESS_ERROR(0); //脚本空间溢出
		}
		else if(!pRegion->SetScriptID(pCell, dwNewScriptID))//再次尝试执行
		{
			KGLOG_PROCESS_ERROR(0); //脚本空间溢出
		}
	}
Exit0:
	;
}


void KG3DSceneLogicalEditorBase::ModifyCurrentCellInfo(long lOldInfo, long lNewInfo, LPCTSTR OldScriptFileName, LPCTSTR NewScriptFileName)
{
	KGLOG_PROCESS_ERROR(m_pLogicalScene);

	if (lOldInfo == 0 && (!OldScriptFileName || strlen(OldScriptFileName) == 0))
		return;
	//遍历全地图所有的cell，将与oldinfo相同相同设置的cell, 转为NewInfo
	DWORD dwOldMask = 0;
	DWORD dwNewMask = 0;
	GetCurrentModifyMask(lOldInfo, &dwOldMask);
	GetCurrentModifyMask(lNewInfo, &dwNewMask);
	DWORD dwOldScriptIndex = 0;
	DWORD dwNewScriptIndex = 0;
	
	DWORD dwReservedMask = GetReserveMask();
	dwNewMask |= (~dwReservedMask);  //将保留位置设为1
	//遍历全地图所有的cell
	short nRegionIndexCache[ScriptIDCount];
	DWORD dwOldScriptID = GetIDFromFileName(OldScriptFileName);
	DWORD dwNewScriptID = GetIDFromFileName(NewScriptFileName);
	//DWORD dwScriptIDMask = GetScriptIDMask();
	DWORD* pBaseInfo;
	for(int i = 0; i < MAX_REGION_WIDTH; i ++)
		for (int j = 0; j < MAX_REGION_HEIGHT; j ++)
		{
			KRegion* pRegion = m_pLogicalScene->GetRegion(i, j);			
			if (pRegion)
			{	//查找当前REGION中对应 newinf 与 oldinfo 的 index
				dwOldScriptIndex = 0;
				dwNewScriptIndex = 0;
				FindScriptIDbyRegion(pRegion, dwOldScriptID, dwNewScriptID, &dwOldScriptIndex, &dwNewScriptIndex);
				if (dwOldScriptIndex == 0 && dwOldScriptID != 0) //本region中没有要找的script设置
					continue;
				((KCell::KBaseInfo*)&dwOldMask)->m_ScriptIndex = dwOldScriptIndex;
				((KCell::KBaseInfo*)&dwNewMask)->m_ScriptIndex = dwNewScriptIndex;
				for(int Z=0;Z<REGION_GRID_HEIGHT;Z++)
					for(int X=0;X<REGION_GRID_WIDTH;X++)
					{
						KCell* pCell = &pRegion->m_Cells[X][Z];
						while (pCell)
						{	
							pBaseInfo = (DWORD*)&(pCell->m_BaseInfo);
							
							if((*pBaseInfo & dwReservedMask) == dwOldMask //除保留位及脚本位，其他相同
								//&& (dwOldScriptID == 0 || dwOldScriptIndex == pCell->m_BaseInfo.m_ScriptIndex) //如果欲设置trap中没有脚本 或者 当前CELL符合 欲设置trap中的脚本index相同
								)  
							{								
								//保留位保持原有内容，其他位设为NewInfo中内容
								*pBaseInfo &= dwNewMask;  //先清除与newinfo不同的内容，保留位不动
								*pBaseInfo |= dwNewMask;  //先加回newinfo中有，而pBaseInfo中没有的内容
								
								SetScriptID(pRegion, pCell, dwNewScriptID);
							}
							pCell = pCell->m_pNext;
						}
					}
			}
			else
				continue; //地图不一定是方形的
		}
Exit0:
		;
}

//)
//修改当前光标处的内容
HRESULT KG3DSceneLogicalEditorBase::PaintSelectedLogicalCell(BOOL bAddCellInfo, BOOL bForceDel)
{
	defShowListCellInfo::iterator i = m_listLogicalCellSelected.begin();
	
	bAddCellInfo = bForceDel ? FALSE : bAddCellInfo;

	DWORD dwOldScriptIndex = 0;
	DWORD dwOldScriptID = 0;
	DWORD dwOldMask = 0;
	DWORD dwReservedMask = GetReserveMask();
	GetCurrentModifyMask(m_dwLogicalCellModifyState, &dwOldMask);
	while (i!=m_listLogicalCellSelected.end())
	{
		CellInfo*& pcellInfo = *i;
		KCell::KBaseInfo& BaseCell = pcellInfo->pCell->m_BaseInfo;

		if (!bForceDel && !bAddCellInfo) //如果用户强制删除bForceDel=true，则不管是否当前选中的TRAP，全部清空
		{
			FindScriptIDbyRegion(pcellInfo->pRegion, dwOldScriptID, 0, &dwOldScriptIndex, NULL);
			if (dwOldScriptIndex == 0 && dwOldScriptID != 0) //本region中没有要找的script设置
				continue;
			((KCell::KBaseInfo*)&dwOldMask)->m_ScriptIndex = dwOldScriptIndex;
			
			DWORD dwBaseInfo = *(DWORD*)(&pcellInfo->pCell->m_BaseInfo);
			if((dwBaseInfo & dwReservedMask) != dwOldMask) //不是活动的Trap
			{
				i++;
				continue;
			}
		}

		BaseCell.m_bBlockCharacter = m_dwLogicalCellModifyState&MODIFY_BLOCKCHARACTER ? bAddCellInfo : 0;
		BaseCell.m_bPutObj = m_dwLogicalCellModifyState&MODIFY_PUTOBJECT ? bAddCellInfo : 0;
		BaseCell.m_bRest = m_dwLogicalCellModifyState&MODIFY_SAFE ? bAddCellInfo : 0;
		BaseCell.m_bStall = m_dwLogicalCellModifyState&MODIFY_STALL ? bAddCellInfo : 0;
		
		BaseCell.m_bIndoor = m_dwLogicalCellModifyState&MODIFY_INDOOR ? bAddCellInfo : 0;
		BaseCell.m_bRest = m_dwLogicalCellModifyState&MODIFY_REST ? bAddCellInfo : 0;
		BaseCell.m_bRideHorse = m_dwLogicalCellModifyState&MODIFY_RIDEHORSE ? bAddCellInfo : 0;

		//@
		//BaseCell.m_ScriptIndex = m_dwLogicalCellModifyState&MODIFY_SCRIPTFILE ? m_dwCurrentScriptID : 0;
		if (bAddCellInfo)
			SetScriptID(pcellInfo->pRegion, pcellInfo->pCell, m_dwCurrentScriptID);
		else
			SetScriptID(pcellInfo->pRegion, pcellInfo->pCell, 0);
		
		if (m_dwLogicalCellModifyState != MODIFY_NONE || m_dwCurrentScriptID)
		{
			if (bForceDel)
			{
				m_listLogicalCellCurrMarked.remove(pcellInfo);
				m_listLogicalCellDiffMarked.remove(pcellInfo);
			}
			else
			{
				if(bAddCellInfo)
				{
					//(by dengzhihui 2006年7月26日 16:10:36
						m_listLogicalCellCurrMarked.remove(pcellInfo);//避免重复加入cellinfo对象
						m_listLogicalCellDiffMarked.remove(pcellInfo);//同时覆盖其他颜色内容
						m_listLogicalCellCurrMarked.push_back(pcellInfo);
					//)
				}
				else
				{
					//(by dengzhihui 2006年7月26日 16:10:36
					m_listLogicalCellCurrMarked.remove(pcellInfo);
					//)
				}
			}
		}
		i++;
	}
	return S_OK;
}

HRESULT KG3DSceneLogicalEditorBase::RenderLogicalCellList(defShowListCellInfo* pList,DWORD dwColorInput)
{
	VFormat::_Faces_Diffuse_Texture1* pV = NULL;
	defShowListCellInfo::iterator i;
	int nCount = 0;
	HRESULT hr = S_OK;

	KGLOG_PROCESS_ERROR(m_pCellVB);

	hr = m_pCellVB->Lock(0,0,(void**)&pV,0);
	KGLOG_COM_PROCESS_ERROR(hr);

	g_pd3dDevice->SetTexture(0,NULL);

	i = pList->begin();
	while (i!=pList->end())
	{
		CellInfo*& pcellinfo = *i;

		float HOff = -pcellinfo->vPosition.y + (10 * m_nCellHorzHeight - 1) + 1;
		//float H = pcellinfo->fTop + HOff;

		DWORD dwColor = dwColorInput;
		int nCount8 = nCount * 8;
		//位于下面的面
		pV[nCount8+BOX_A1].diffuse = dwColor;
		pV[nCount8+BOX_A1].p = pcellinfo->vPosition;
		pV[nCount8+BOX_A1].tu1 = 0;
		pV[nCount8+BOX_A1].tv1 = 0;

		pV[nCount8+BOX_B1].diffuse = dwColor;
		pV[nCount8+BOX_B1].p = pcellinfo->vPosition + D3DXVECTOR3(LOGICAL_CELL_CM_LENGTH,0,0);
		pV[nCount8+BOX_B1].tu1 = 1;
		pV[nCount8+BOX_B1].tv1 = 0;

		pV[nCount8+BOX_C1].diffuse = dwColor;
		pV[nCount8+BOX_C1].p = pcellinfo->vPosition + D3DXVECTOR3(LOGICAL_CELL_CM_LENGTH,0,LOGICAL_CELL_CM_LENGTH);
		pV[nCount8+BOX_C1].tu1 = 1;
		pV[nCount8+BOX_C1].tv1 = 1;

		pV[nCount8+BOX_D1].diffuse = dwColor;
		pV[nCount8+BOX_D1].p = pcellinfo->vPosition + D3DXVECTOR3(0,0,LOGICAL_CELL_CM_LENGTH);
		pV[nCount8+BOX_D1].tu1 = 0;
		pV[nCount8+BOX_D1].tv1 = 1;

		//位于上面的面
		//左下角点
		pV[nCount8+BOX_A2].diffuse = dwColor;
		pV[nCount8+BOX_A2].p = pcellinfo->vPosition + D3DXVECTOR3(0,pcellinfo->H[0] + HOff,0);
		pV[nCount8+BOX_A2].tu1 = 0;
		pV[nCount8+BOX_A2].tv1 = 0;
		//右下角
		pV[nCount8+BOX_B2].diffuse = dwColor;
		pV[nCount8+BOX_B2].p = pcellinfo->vPosition + D3DXVECTOR3(LOGICAL_CELL_CM_LENGTH,
			pcellinfo->H[1] + HOff, 0);
		pV[nCount8+BOX_B2].tu1 = 1;
		pV[nCount8+BOX_B2].tv1 = 0;
		//右上角
		pV[nCount8+BOX_C2].diffuse = dwColor;
		pV[nCount8+BOX_C2].p = pcellinfo->vPosition + D3DXVECTOR3(LOGICAL_CELL_CM_LENGTH,
			pcellinfo->H[2] + HOff, LOGICAL_CELL_CM_LENGTH);
		pV[nCount8+BOX_C2].tu1 = 1;
		pV[nCount8+BOX_C2].tv1 = 1;
		//左上角
		pV[nCount8+BOX_D2].diffuse = dwColor;
		pV[nCount8+BOX_D2].p = pcellinfo->vPosition + D3DXVECTOR3(0,
			pcellinfo->H[3] + HOff,LOGICAL_CELL_CM_LENGTH);
		pV[nCount8+BOX_D2].tu1 = 0;
		pV[nCount8+BOX_D2].tv1 = 1;

		i++;
		nCount++;

		if(nCount>=MAXCELLSRENDEREACHTIME)
		{
			hr = m_pCellVB->Unlock();
			KGLOG_COM_PROCESS_ERROR(hr);

			g_pd3dDevice->SetIndices(m_pCellIB);
			g_pd3dDevice->SetStreamSource(0,m_pCellVB,0,sizeof(VFormat::_Faces_Diffuse_Texture1));
			g_pd3dDevice->SetFVF(D3DFVF_Faces_Diffuse_Texture1);

			g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,MAXCELLSRENDEREACHTIME*8,	0,nCount*12);

			nCount = 0;

			hr = m_pCellVB->Lock(0,0,(void**)&pV,0);
			KGLOG_COM_PROCESS_ERROR(hr);

		}
	}

	hr = m_pCellVB->Unlock();
	KGLOG_COM_PROCESS_ERROR(hr);

	g_pd3dDevice->SetIndices(m_pCellIB);
	g_pd3dDevice->SetStreamSource(0,m_pCellVB,0,sizeof(VFormat::_Faces_Diffuse_Texture1));
	g_pd3dDevice->SetFVF(D3DFVF_Faces_Diffuse_Texture1);

	g_pd3dDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,0,0,MAXCELLSRENDEREACHTIME*8,	0,nCount*12);

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneLogicalEditorBase::GetShowCellList(defShowListCellInfo* pListCurrShow, defShowListCellInfo* pListDiffShow, const list<CellInfo>& listInput)
{
	KCell::KBaseInfo stCellNullMask, stCellCurrModifyMask;
	memset(&stCellNullMask, 0x0, sizeof(KCell::KBaseInfo));

	//处于编辑状态的集合转换为可比较的 Mask;
	GetCurrentModifyMask(m_dwLogicalCellShowState, (DWORD*)&stCellCurrModifyMask);

	BOOL bModifyNull = TODWORD(&stCellNullMask) == TODWORD(&stCellCurrModifyMask) && (m_dwCurrentScriptID == 0);
	DWORD dwReservedMask = GetReserveMask();

	list<CellInfo>::const_iterator i = listInput.begin();
	KRegion* preRegion = NULL;
	DWORD dwPreScriptIndex = 0;
	DWORD dwCurrRegionScriptIndex = 0;
	DWORD dwScriptIDMask = GetScriptIDMask();
	while (i!=listInput.end() && !bModifyNull)
	{
		const CellInfo& cellinfo = *i;
		KCell::KBaseInfo* pBaseInfo = &cellinfo.pCell->m_BaseInfo;
				
		//如果当前CELL非空
		if ((TODWORD(pBaseInfo) & dwReservedMask))
		{
			if( (TODWORD(&stCellCurrModifyMask) == (TODWORD(pBaseInfo) & dwReservedMask & dwScriptIDMask))) //除保留位及脚本位，其他相同
			{
				//还要判断是否ScriptIndex是否相同
				if (m_dwCurrentScriptID == 0 && pBaseInfo->m_ScriptIndex == 0) 
					pListCurrShow->push_back((CellInfo*)&cellinfo); //没有设置scriptindex，则直接判定为当前集合
				else
				{
					//先转换欲比较的 sciprtindex 为本region实际的 scriptidx
					if (preRegion == cellinfo.pRegion) //cache
						dwCurrRegionScriptIndex = dwPreScriptIndex;
					else
					{
						DWORD dwFindID = 0;
						for (int i = 0; i < ScriptIDCount; i++)
						{
							dwFindID = cellinfo.pRegion->GetScriptIDByIndex(i);
							if (m_dwCurrentScriptID == dwFindID)
								dwCurrRegionScriptIndex = i;
						}
					}
					//dwCurrRegionScriptIndex是转换为当前region的scriptindex, 是否与当前CELL的scriptID相同
					if (dwCurrRegionScriptIndex == pBaseInfo->m_ScriptIndex)
						pListCurrShow->push_back((CellInfo*)&cellinfo);
					else
						pListDiffShow->push_back((CellInfo*)&cellinfo);
				}
			}
			else //如果不是当前集合，且格式中有设过逻辑信息，则设为其他颜色
				pListDiffShow->push_back((CellInfo*)&cellinfo);
		}
		i++;
	}
	return S_OK;
}

//(by dengzhihui 2006年8月3日 10:47:49
void KG3DSceneLogicalEditorBase::SetCurrentEditColor(COLORREF clr)
{	
	m_d3dclrCurrColor = ColorValue(Colorref2value(clr, 0x88));
}
//)

HRESULT KG3DSceneLogicalEditorBase::RenderLogicalCell()
{
	GraphicsStruct::RenderStateCullMode cullmode(D3DCULL_NONE);
	GraphicsStruct::RenderState Light(D3DRS_LIGHTING,FALSE);
	GraphicsStruct::RenderStateAlpha Alpha(0x20,TRUE,TRUE,D3DBLEND_SRCALPHA,D3DBLEND_INVSRCALPHA);

	//(by dengzhihui 2006年7月26日 16:02:15 --此处代码已经转移到RecalcShowCell，不需要每次都计算
	//list<CellInfo>listLogicalCellMarked;
	//m_listLogicalCellCurrMarked.clear();
	//GetShowCellList(&m_listLogicalCellCurrMarked,&m_listLogicalCellSelectableGround);
	//GetShowCellList(&m_listLogicalCellCurrMarked,&m_listLogicalCellSelectableOverGround);
	//)

	RenderLogicalCellList(&m_listLogicalCellDiffMarked,0x80203040); //非当前集合的颜色
	RenderLogicalCellList(&m_listLogicalCellCurrMarked,m_d3dclrCurrColor); //当前选中集合的颜色
	RenderLogicalCellList(&m_listLogicalCellSelected,0x88880099);   //光标所在位置的高亮色
	return S_OK;
}


KG3DSceneLogicalEditorBase::CellInfo* KG3DSceneLogicalEditorBase::GetFirstLogicalCellIntersectRay(D3DXVECTOR3& CameraPos,D3DXVECTOR3& CameraDirct)
{
	CellInfo* pcellinfo = NULL;

	float fDistance = 1000000;
	list<CellInfo>::iterator i = m_listLogicalCellSelectableOverGround.begin();
	while (i!=m_listLogicalCellSelectableOverGround.end())
	{
		CellInfo& cell = *i;
		cell.bSelected = FALSE;
		float Len = 1000000;
		if(cell.IsRayIntersect(Len,CameraPos,CameraDirct))
		{
			if(Len<fDistance)
			{
				fDistance = Len;
				pcellinfo = &cell;
			}
		}
		i++;
	}
	return pcellinfo;
}

HRESULT KG3DSceneLogicalEditorBase::SelectLogicalCells(Terrain::KG3DTerrainEx* pTerrain,D3DXVECTOR3& CameraPos,D3DXVECTOR3& CameraDirct)
{
	KGLOG_PROCESS_ERROR(m_pLogicalScene);

	m_listLogicalCellSelected.clear();

	CellInfo* pCellInfo = GetFirstLogicalCellIntersectRay(CameraPos,CameraDirct);

	float Width = (m_eBrushType == cbCircle) ? m_nBrushWidth * float(2000) : m_nBrushWidth * 25;//10000;

	if(pCellInfo)//选中了空中的格子
	{
		//if (m_nBrushWidth == 1) //特殊处理，只有一个格子
		{
			//m_listLogicalCellSelected.push_back(pCellInfo);
		}
		//else
		{		
			list<CellInfo>::iterator i = m_listLogicalCellSelectableOverGround.begin();
			while (i!=m_listLogicalCellSelectableOverGround.end()) //遍历所有的空中格子
			{
				CellInfo& cellinfo = *i;

				float x = cellinfo.vPosition.x - pCellInfo->vPosition.x;
				float z = cellinfo.vPosition.z - pCellInfo->vPosition.z;
				int y = cellinfo.pCell->m_wLowLayer - pCellInfo->pCell->m_wLowLayer;

				if( (m_eBrushType == cbCircle && (x*x+z*z<=Width)&&(abs(y)<=2)) ||
					(m_eBrushType == cbRectangle && abs(x) <= Width && abs(z) <= Width && (abs(y)<=2))
					)
				{
					m_listLogicalCellSelected.push_back(&cellinfo);
				}			

				i++;
			}
		}
		//m_listLogicalCellSelected.push_back(*pCellInfo);
	}
	else//检查地面上的格子
	{
		D3DXVECTOR3 Inter;
		pTerrain->GetIntersectionPoint(Inter);

		list<CellInfo>::iterator i = m_listLogicalCellSelectableGround.begin();
		while (i!=m_listLogicalCellSelectableGround.end())
		{
			CellInfo& cellinfo = *i;

			float x = cellinfo.vPosition.x - Inter.x;
			float z = cellinfo.vPosition.z - Inter.z;
			if( (m_eBrushType == cbCircle && (x*x+z*z<=Width)) ||
				(m_eBrushType == cbRectangle && abs(x) <= Width && abs(z) <= Width)
				)
			{
				m_listLogicalCellSelected.push_back(&cellinfo);
			}
			i++;
		}
	}

	return S_OK;
Exit0:
	return E_FAIL;
}

HRESULT KG3DSceneLogicalEditorBase::CreateBuffersForRenderCell()
{
	HRESULT hr = S_OK;
	DWORD dwNumVertex,dwNumIndex;
	WORD* pIndex = NULL;

	SAFE_RELEASE(m_pCellVB);
	SAFE_RELEASE(m_pCellIB);

	dwNumVertex = MAXCELLSRENDEREACHTIME * 8;
	dwNumIndex  = MAXCELLSRENDEREACHTIME * 6 * 2 * 3;

	hr = g_pd3dDevice->CreateVertexBuffer(dwNumVertex * sizeof(VFormat::_Faces_Diffuse_Texture1),
		0,D3DFVF_Faces_Diffuse_Texture1,
		D3DPOOL_MANAGED, 
		&m_pCellVB, NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

	hr = g_pd3dDevice->CreateIndexBuffer(dwNumIndex*sizeof(DWORD),
		0,D3DFMT_INDEX16,D3DPOOL_MANAGED,
		&m_pCellIB,NULL);
	KGLOG_COM_PROCESS_ERROR(hr);

	//////////////////////////////////////////////////////////////////////////
	//FillIndexBuffer
	hr = m_pCellIB->Lock(0,0,(void**)&pIndex,0);
	KGLOG_COM_PROCESS_ERROR(hr);

	for(int i=0;i<MAXCELLSRENDEREACHTIME;i++)
	{
		pIndex[i*36   ] = (WORD)(i*8+BOX_A2);//A2B2C2D2
		pIndex[i*36+1 ] = (WORD)(i*8+BOX_B2);
		pIndex[i*36+2 ] = (WORD)(i*8+BOX_C2);
		pIndex[i*36+3 ] = (WORD)(i*8+BOX_A2);
		pIndex[i*36+4 ] = (WORD)(i*8+BOX_C2);
		pIndex[i*36+5 ] = (WORD)(i*8+BOX_D2);

		pIndex[i*36+6]  = (WORD)(i*8+BOX_A1);//A1D1D2A2
		pIndex[i*36+7 ] = (WORD)(i*8+BOX_D1);
		pIndex[i*36+8 ] = (WORD)(i*8+BOX_D2);
		pIndex[i*36+9 ] = (WORD)(i*8+BOX_A1);
		pIndex[i*36+10] = (WORD)(i*8+BOX_D2);
		pIndex[i*36+11] = (WORD)(i*8+BOX_A2);

		pIndex[i*36+12] = (WORD)(i*8+BOX_D1);//D1C1C2D2
		pIndex[i*36+13] = (WORD)(i*8+BOX_C1);
		pIndex[i*36+14] = (WORD)(i*8+BOX_C2);
		pIndex[i*36+15] = (WORD)(i*8+BOX_D1);
		pIndex[i*36+16] = (WORD)(i*8+BOX_C2);
		pIndex[i*36+17] = (WORD)(i*8+BOX_D2);

		pIndex[i*36+18] = (WORD)(i*8+BOX_B1);//B1C1C2B2
		pIndex[i*36+19] = (WORD)(i*8+BOX_C1);
		pIndex[i*36+20] = (WORD)(i*8+BOX_C2);
		pIndex[i*36+21] = (WORD)(i*8+BOX_B1);
		pIndex[i*36+22] = (WORD)(i*8+BOX_C2);
		pIndex[i*36+23] = (WORD)(i*8+BOX_B2);

		pIndex[i*36+24] = (WORD)(i*8+BOX_A1);//A1B1B2A2
		pIndex[i*36+25] = (WORD)(i*8+BOX_B1);
		pIndex[i*36+26] = (WORD)(i*8+BOX_B2);
		pIndex[i*36+27] = (WORD)(i*8+BOX_A1);
		pIndex[i*36+28] = (WORD)(i*8+BOX_B2);
		pIndex[i*36+29] = (WORD)(i*8+BOX_A2);

		pIndex[i*36+30] = (WORD)(i*8+BOX_A1);//A1B1C1D1
		pIndex[i*36+31] = (WORD)(i*8+BOX_B1);
		pIndex[i*36+32] = (WORD)(i*8+BOX_C1);
		pIndex[i*36+33] = (WORD)(i*8+BOX_A1);
		pIndex[i*36+34] = (WORD)(i*8+BOX_C1);
		pIndex[i*36+35] = (WORD)(i*8+BOX_D1);
	}
	hr = m_pCellIB->Unlock();
	KGLOG_COM_PROCESS_ERROR(hr);

	return S_OK;
Exit0:
	SAFE_RELEASE(m_pCellVB);
	SAFE_RELEASE(m_pCellIB);
	return E_FAIL;
}
//(by dengzhihui 2006年8月9日 12:06:45
void KG3DSceneLogicalEditorBase::SetBrushInfo(int nWidth, eCellBrushShape eBrushType)
{
	if (nWidth > 0 && nWidth < 100)
		m_nBrushWidth = nWidth;
	switch(eBrushType)
	{
	case cbRectangle:
	case cbCircle:
			m_eBrushType = eBrushType;
			break;
	default:
		m_eBrushType = cbRectangle;
	}
}

void KG3DSceneLogicalEditorBase::SetDisplayParam(int nDisplayWidth, int nCellHorzHeight)
{
	if (nDisplayWidth > 0 && nDisplayWidth < 100 && m_nDisplayWidth != nDisplayWidth)
	{
		m_nDisplayWidth = nDisplayWidth;
		m_nXStart = 0; m_nXEnd = 0;
		m_nZStart = 0; m_nZEnd = 0;
		//defShowListCellInfo::iterator i = m_listLogicalCellSelected.begin();
		//if (i!=m_listLogicalCellSelected.end())
		//	UpdateEditingCells(i->pRegion->gett)
	}

	if (nCellHorzHeight > 0 && nCellHorzHeight < 100 && m_nCellHorzHeight != nCellHorzHeight)
		m_nCellHorzHeight = nCellHorzHeight;
}

//)