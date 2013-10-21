////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KLogicObstacleGenerator.cpp
//  Version     : 1.0
//  Creator     : Zhao Chunfeng
//  Create Date : 2007-02-28 18:01:31
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IEKG3DTerrain.h"
#include "IEKG3DScene.h"
#include "KScene.h"
#include "KRegion.h"
#include "KLogicObstacleGenerator.h"
#include "IEEditor.h"
#include "../ProgressForm.h"

extern CProgressForm g_ProgressDlg;
extern CStatic g_ProgressName;
extern CProgressCtrl g_ProgressProgress;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static int         g_nDynamicObstacleGroupIndex = INVALID_OBSTACLE_GROUP;
static float       g_TrianglePos[3][3];
static KRegion*    g_pCurrentRegion = NULL;

int TryOctree(
    float fWorldX, float fWorldY, float fWorldZ,
    int nCellLenX, int nCellLenY, int nCellLenZ
);

int g_nCellCount = 0;
int g_nCellCountTotal = 0;

KLogicObstacleGenerator::KLogicObstacleGenerator()
{
    m_pi3DScene     = NULL;
    m_pLogicScene   = NULL;
    m_pLogicRegion  = NULL;
//  m_pProgressBar  = NULL;
}
BOOL KLogicObstacleGenerator::GenerateSceneLogicObstacleData(IEKG3DScene* pi3DScene, KScene* pLogicScene,BOOL bOnlyAppointCell,	BOOL bClearCell)
{
    BOOL        bResult             = false;
    int         nRetCode            = false;
	int nX_Start,nZ_Start,nX_Width,nZ_Width;
	int nX_LogicRegionStart = 0,nY_LogicRegionStart = 0,nX_LogicRegionEnd=0,nY_LogicRegionEnd=0;
	int         nCellCountX         = 0;
    int         nCellCountY         = 0;
    int         nRegionCountX       = 0;
    int         nRegionCountY       = 0;
    KRegion*    pLogicRegion        = NULL;
    int         nTotalRegion        = 0;
    int         nCompleteRegion     = 0;
    char        szMessage[64];

    m_pi3DScene     = pi3DScene;
    m_pLogicScene   = pLogicScene;

    // 注意,这里GetTerrainWidth取到的是网格顶点数,定点间距为1米
    // 比如GetTerrainWidth取到129,则表示横向有129个顶点,也就是128米
	pi3DScene->GetLogicalSceneSize(&nX_Start,&nZ_Start,&nX_Width,&nZ_Width);

	/*nRetCode = pi3DTerrain->GetTerrainWidth(&nTerrainPointCountX);
	KGLOG_COM_PROCESS_ERROR(nRetCode);
	nRetCode = pi3DTerrain->GetTerrainHeight(&nTerrainPointCountY);
	KGLOG_COM_PROCESS_ERROR(nRetCode);*/

    KGLOG_PROCESS_ERROR(nX_Width > 0);
    KGLOG_PROCESS_ERROR(nZ_Width > 0);

	if(!bOnlyAppointCell)
	{
		nCellCountX = nX_Width * 100  / LOGICAL_CELL_CM_LENGTH;
		nCellCountY = nZ_Width * 100  / LOGICAL_CELL_CM_LENGTH;

		KGLOG_PROCESS_ERROR(nCellCountX > 0);
		KGLOG_PROCESS_ERROR(nCellCountY > 0);

		nRegionCountX = (nCellCountX - 1) / REGION_GRID_WIDTH + 1;
		nRegionCountY = (nCellCountY - 1) / REGION_GRID_HEIGHT + 1;

		nRetCode = pLogicScene->Resize(nRegionCountX, nRegionCountY, eResizeSceneAlignLeftBottom);
		KGLOG_PROCESS_ERROR(nRetCode);
	}
	else
	{
		pi3DScene->GetAppointLogicalRegion(nX_LogicRegionStart,nY_LogicRegionStart,nRegionCountX,nRegionCountY);
	
		KGLOG_PROCESS_ERROR(nRegionCountX > 0);
		KGLOG_PROCESS_ERROR(nRegionCountY > 0);
	}
	nX_LogicRegionEnd = nX_LogicRegionStart + nRegionCountX;
	nY_LogicRegionEnd = nY_LogicRegionStart + nRegionCountY;

	g_ProgressProgress.SetRange32(0, 100);
	g_ProgressDlg.ShowWindow(SW_NORMAL);

    nTotalRegion = nRegionCountX * nRegionCountY;
	g_nCellCountTotal = 0;
    // 遍历每个Region
    for (int nY = nY_LogicRegionStart; nY < nY_LogicRegionEnd; nY++)
    {
        for (int nX = nX_LogicRegionStart; nX < nX_LogicRegionEnd; nX++)
        {
            pLogicRegion = pLogicScene->GetRegion(nX, nY);
            KGLOG_PROCESS_ERROR(pLogicRegion);

            nRetCode = GenerateRegionLogicObstacleData( pi3DScene, pLogicRegion,bClearCell,nX_Start * 100.0F,nZ_Start*100.0F);
            KGLOG_PROCESS_ERROR(nRetCode);

            nCompleteRegion++;

            snprintf(szMessage, sizeof(szMessage), "Region: %d / %d", nCompleteRegion, nTotalRegion);
            szMessage[sizeof(szMessage) - 1] = '\0';

			g_ProgressName.SetWindowText(szMessage);
			g_ProgressProgress.SetPos(nCompleteRegion * 100 / nTotalRegion);
        }
    }

	g_ProgressName.SetWindowText("正在进行障碍预计算 ... ...");
	g_ProgressProgress.SetPos(100);

	pLogicScene->PreprocessCalculateData();
	{
		TCHAR szMessage[MAX_PATH];
		wsprintf(szMessage,"Calculate logic data complete! \n Total %d Cells",g_nCellCountTotal);
		MessageBox(/*AfxGetMainWnd()->m_hWnd*/NULL, szMessage, "", MB_OK);
		KGLogPrintf(KGLOG_ERR, "Total %d Cells",g_nCellCountTotal);
	}

	g_ProgressDlg.ShowWindow(SW_HIDE);

    bResult = true;
Exit0:
    //SAFE_DELETE(m_pProgressBar);
    if (!bResult)
    {
        pLogicScene->DeleteAllObstacle();
    }
    return bResult;
}

BOOL KLogicObstacleGenerator::GenerateRegionLogicObstacleData(
    IEKG3DScene* pi3DScene, KRegion* pLogicRegion,
	BOOL bClearCell,float fSceneX_Start,float fSceneZ_Start
)
{
    BOOL bResult    = false;
    BOOL bRetCode   = false;

    m_pi3DScene     = pi3DScene;
    m_pLogicRegion  = pLogicRegion;

	bRetCode = pLogicRegion->DeleteAllCell();
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = GenerateRegionTerrainObstacle(fSceneX_Start,fSceneZ_Start);
	KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = GenerateRegionEntityObstacle(fSceneX_Start,fSceneZ_Start);
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = pLogicRegion->MergeObstacleData();
    KGLOG_PROCESS_ERROR(bRetCode);

    bRetCode = GenerateRegionWaterObstacle(fSceneX_Start,fSceneZ_Start);
    KGLOG_PROCESS_ERROR(bRetCode);

	bRetCode = ProcessRegionObstacleWithPoly(bClearCell,fSceneX_Start,fSceneZ_Start);
	KGLOG_PROCESS_ERROR(bRetCode);

    bResult = true;
Exit0:
    if (!bResult)
    {
        pLogicRegion->DeleteAllCell();
    }
    return bResult;
}

BOOL KLogicObstacleGenerator::IsCellInPoly(float CenterX,float CenterZ,KCell* pCell,IEKG3DRepresentPoly* pPoly,BOOL bGroundCell)
{
	float H = pCell->m_wHighLayer * ALTITUDE_UNIT + TERRAIN_MIN_HEIGHT;

	if((bGroundCell)||(pCell->m_wLowLayer==0))
	{
		D3DXVECTOR3 vPosH(CenterX,H + 5.0F,CenterZ);//稍微升高一些，确保能进入包围合
		return pPoly->IsPointIn(vPosH,TRUE);
	}
	else
	{
		float L = pCell->m_wLowLayer * ALTITUDE_UNIT + TERRAIN_MIN_HEIGHT;

		D3DXVECTOR3 vPosH(CenterX,H,CenterZ);
		D3DXVECTOR3 vPosL(CenterX,L,CenterZ);

		if((pPoly->IsPointIn(vPosH,TRUE))||(pPoly->IsPointIn(vPosL,TRUE)))
			return TRUE;

	}
	return FALSE;
}

extern DWORD GetIDFromFileName(LPCTSTR lpstrFileName,BOOL bFullName);
extern bool SetScriptID(KRegion* pRegion, KCell* pCell, DWORD dwNewScriptID);

//遍历所有的POLY，指定的CELL如果在任何一个POLY中，则设置上该POLY对应的逻辑障碍属性
HRESULT KLogicObstacleGenerator::ProcessCellObstacleWithPolys(int nPoly,IEKG3DRepresentPoly* PolyArray[],KCell* pCell,float fCenterX,float fCenterY,BOOL bGroundCell,BOOL bClearCell)
{
	KCell::KBaseInfo& BaseCell = pCell->m_BaseInfo;

	/*if(bClearCell)
	{
		BaseCell.dwScriptIndex = 0;
		BaseCell.dwBlockCharacter = 0;
		BaseCell.dwPutObj = 0;
		BaseCell.dwRest = 0;
		BaseCell.dwStall = 0;
		BaseCell.dwIndoor = 0;
		BaseCell.dwRest = 0;
		BaseCell.dwRideHorse = 0;
	}*/

	for(int i=0;i<nPoly;i++)
	{
		IEKG3DRepresentPoly* pPoly = PolyArray[i];


		DWORD dwScriptID = GetIDFromFileName(pPoly->GetScriptFileName(),FALSE);

		if(IsCellInPoly(fCenterX,fCenterY,pCell,pPoly,bGroundCell))
		{
			DWORD dwModifyState = pPoly->GetModifyState();

			BaseCell.dwBlockCharacter = dwModifyState&MODIFY_BLOCKCHARACTER ? 1 : 0;
			BaseCell.dwPutObj = dwModifyState&MODIFY_PUTOBJECT ? 1 : 0;
			BaseCell.dwRest = dwModifyState&MODIFY_SAFE ? 1 : 0;
			BaseCell.dwStall = dwModifyState&MODIFY_STALL ? 1 : 0;

			BaseCell.dwIndoor = dwModifyState&MODIFY_INDOOR ? 1 : 0;
			BaseCell.dwRest = dwModifyState&MODIFY_REST ? 1 : 0;
			BaseCell.dwRideHorse = dwModifyState&MODIFY_RIDEHORSE ? 1 : 0;

			SetScriptID(m_pLogicRegion,pCell,dwScriptID);
		}
	}

	return S_OK;
}

//遍历指定的REGION中所有的CELL，并对每个CELL作逻辑障碍设置处理
BOOL KLogicObstacleGenerator::ProcessRegionObstacleWithPoly(BOOL bClearCell,float fSceneX_Start,float fSceneZ_Start)
{
	IEKG3DSceneSceneEditor* pEditor = NULL;
	m_pi3DScene->GetSceneSceneEditor(&pEditor);

	static IEKG3DRepresentPoly* PolyArray[MAX_PATH];
////////////////////////////////////////////////////////////////////////////////////
	if(bClearCell)//清除所有逻辑障碍 add by huangjinshou 2008-8-1
	{
		for(int nY = 0; nY < REGION_GRID_HEIGHT; nY++)
		{
			for(int nX = 0; nX < REGION_GRID_WIDTH; nX++)
			{
				KCell* pCell = m_pLogicRegion->GetLowestObstacle(nX, nY);
				while (pCell)
				{
					KCell::KBaseInfo& BaseCell = pCell->m_BaseInfo;
					BaseCell.dwScriptIndex = 0;
					BaseCell.dwBlockCharacter = 0;
					BaseCell.dwPutObj = 0;
					BaseCell.dwRest = 0;
					BaseCell.dwStall = 0;
					BaseCell.dwIndoor = 0;
					BaseCell.dwRest = 0;
					BaseCell.dwRideHorse = 0;
					pCell = pCell->m_pNext;
				}
			}
		}
	}
/////////////////////////////////////////////////////////////////
	float fXStart = fSceneX_Start + LOGICAL_CELL_CM_LENGTH * REGION_GRID_WIDTH * m_pLogicRegion->m_nRegionX;
	float fYStart = fSceneZ_Start + LOGICAL_CELL_CM_LENGTH * REGION_GRID_HEIGHT * m_pLogicRegion->m_nRegionY;

	D3DXVECTOR2 A(fXStart,fYStart);
	D3DXVECTOR2 C = A + D3DXVECTOR2(LOGICAL_CELL_CM_LENGTH * REGION_GRID_WIDTH,LOGICAL_CELL_CM_LENGTH * REGION_GRID_HEIGHT);

	int nCount = pEditor->GetLogicalPolyIntersectRect(PolyArray,MAX_PATH,A,C);
	
	if(!nCount)
		return TRUE;

	for(int nY = 0; nY < REGION_GRID_HEIGHT; nY++)
	{
		for(int nX = 0; nX < REGION_GRID_WIDTH; nX++)
		{
			float fCenterX          = fXStart + nX * LOGICAL_CELL_CM_LENGTH + LOGICAL_CELL_CM_LENGTH / 2.0F;
			float fCenterY          = fYStart + nY * LOGICAL_CELL_CM_LENGTH + LOGICAL_CELL_CM_LENGTH / 2.0F;
			
			KCell* pCell = m_pLogicRegion->GetLowestObstacle(nX, nY);

			BOOL bLow = TRUE;
			while (pCell)
			{
				ProcessCellObstacleWithPolys(nCount,PolyArray,pCell,fCenterX,fCenterY,bLow,bClearCell);

				pCell = pCell->m_pNext;
				bLow = FALSE;
			}
		}
	}

	return TRUE;
}

BOOL KLogicObstacleGenerator::GenerateRegionTerrainObstacle(float fSceneX_Start,float fSceneZ_Start)
{
    BOOL    bResult     = false;
    BOOL    bRetCode    = false;
    float   fXStart     = 0.0f;
    float   fYStart     = 0.0f;
	D3DXVECTOR3 vOut;

	fXStart = fSceneX_Start + LOGICAL_CELL_CM_LENGTH * REGION_GRID_WIDTH * m_pLogicRegion->m_nRegionX;
	fYStart = fSceneZ_Start + LOGICAL_CELL_CM_LENGTH * REGION_GRID_HEIGHT * m_pLogicRegion->m_nRegionY;

	for(int nY = 0; nY < REGION_GRID_HEIGHT; nY++)
	{
		for(int nX = 0; nX < REGION_GRID_WIDTH; nX++)
		{
            float fCenterX          = fXStart + nX * LOGICAL_CELL_CM_LENGTH + LOGICAL_CELL_CM_LENGTH / 2.0F;
            float fCenterY          = fYStart + nY * LOGICAL_CELL_CM_LENGTH + LOGICAL_CELL_CM_LENGTH / 2.0F;
			float fCenterHeight     = 0.0f; 
            float fLowestPoint      = 0.0f;
            float fCellHeight       = TERRAIN_MIN_HEIGHT;
            //float fPointHeight      = 0.0f;
            float fPointX           = 0.0f;
            float fPointY           = 0.0f;
			int   nLowestDirection  = 0;
            
            // 注意,Cell的逻辑高度是取Cell4个角的最高高度(也就是地表在Cell的最高高度)

            // 左下
            fPointX = fXStart + nX * LOGICAL_CELL_CM_LENGTH;
            fPointY = fYStart + nY * LOGICAL_CELL_CM_LENGTH;

			m_pi3DScene->GetHeight(&vOut,&D3DXVECTOR3(fPointX, 0,fPointY));
			//m_pi3DTerrain->GetGroundHeight(fPointHeight, fPointX, fPointY);
            if ( vOut.y > fCellHeight)
            {
                fCellHeight = vOut.y;
            }
            
            // 右下
            fPointX = fXStart + nX * LOGICAL_CELL_CM_LENGTH + LOGICAL_CELL_CM_LENGTH;
            fPointY = fYStart + nY * LOGICAL_CELL_CM_LENGTH;
			m_pi3DScene->GetHeight(&vOut,&D3DXVECTOR3(fPointX, 0,fPointY));
            if ( vOut.y > fCellHeight)
            {
                fCellHeight = vOut.y;
            }

            // 右上
            fPointX = fXStart + nX * LOGICAL_CELL_CM_LENGTH + LOGICAL_CELL_CM_LENGTH;
            fPointY = fYStart + nY * LOGICAL_CELL_CM_LENGTH + LOGICAL_CELL_CM_LENGTH;
			m_pi3DScene->GetHeight(&vOut,&D3DXVECTOR3(fPointX, 0,fPointY));
            if ( vOut.y > fCellHeight)
            {
                fCellHeight = vOut.y;
            }

            // 左上
            fPointX = fXStart + nX * LOGICAL_CELL_CM_LENGTH;
            fPointY = fYStart + nY * LOGICAL_CELL_CM_LENGTH + LOGICAL_CELL_CM_LENGTH;
			m_pi3DScene->GetHeight(&vOut,&D3DXVECTOR3(fPointX, 0,fPointY));
            if ( vOut.y > fCellHeight)
            {
                fCellHeight = vOut.y;
            }

            // 获取Cell中心点的高度
			m_pi3DScene->GetHeight(&vOut,&D3DXVECTOR3(fCenterX, 0,fCenterY));
			fCenterHeight = vOut.y;
            fLowestPoint  = vOut.y;

            for (int nDirection = 0; nDirection < DIRECTION_COUNT; nDirection += DIRECTION_COUNT / 8)
            {
                fPointX         = fCenterX + LOGICAL_CELL_CM_LENGTH / 2.0f * cosf(2 * D3DX_PI * nDirection / DIRECTION_COUNT);
                fPointY         = fCenterY + LOGICAL_CELL_CM_LENGTH / 2.0f * sinf(2 * D3DX_PI * nDirection / DIRECTION_COUNT);
                
				m_pi3DScene->GetHeight(&vOut,&D3DXVECTOR3(fPointX, 0,fPointY));

                if (vOut.y <= fLowestPoint)
                {
                    fLowestPoint        = vOut.y;
                    nLowestDirection    = nDirection;
                }
            }

            int     nCenterH        = (int)((fCenterHeight - TERRAIN_MIN_HEIGHT) / ALTITUDE_UNIT);
            int     nLowestH        = (int)((fLowestPoint - TERRAIN_MIN_HEIGHT) / ALTITUDE_UNIT);
            int     nDeltaHPoint    = (nCenterH - nLowestH) * POINT_PER_ALTITUDE;
            float   fSlopeAngle     = atanf((float)nDeltaHPoint / (float)XYPOINT_TO_ZPOINT(CELL_LENGTH / 2));
            int     nSlopeAngle     = (int)(fSlopeAngle * DIRECTION_COUNT / (2 * D3DX_PI));
            int     nCellH          = (int)((fCellHeight - TERRAIN_MIN_HEIGHT) / ALTITUDE_UNIT);

			KCell Cell;

            if (TERRAIN_MIN_HEIGHT + nCellH * ALTITUDE_UNIT < fCellHeight)
            {
                nCellH++;
            }

            ASSERT(nSlopeAngle >= 0 && nSlopeAngle < DIRECTION_COUNT / 4);

            m_pLogicRegion->GetBaseCellInfo(nX, nY, &Cell);

		    Cell.SetGradientDirection(nLowestDirection);
            Cell.SetGradientDegree(nSlopeAngle);
            Cell.SetBarrierDirection(nLowestDirection);

            Cell.m_BaseInfo.dwFaceUp                = 1;
			Cell.m_wLowLayer                        = 0;
			Cell.m_wHighLayer                       = (WORD)nCellH;

            m_pLogicRegion->SetBaseCellInfo(nX, nY, Cell);
		}
	}

    bResult = true;
//Exit0:
    return bResult;
}



BOOL GetCellCount(KCell* pCell)
{
	g_nCellCount++;
	return TRUE;
}

BOOL KLogicObstacleGenerator::GenerateRegionEntityObstacle(float fSceneX_Start,float fSceneZ_Start)
{
    BOOL    bResult     = false;
    HRESULT hRetCode    = E_FAIL;

    g_pCurrentRegion = m_pLogicRegion;

    hRetCode = m_pi3DScene->TraverseEntityMeshForGenerateLogicObstacle(
        fSceneX_Start, fSceneZ_Start, m_pLogicRegion->m_nRegionX, m_pLogicRegion->m_nRegionY,
        TraverseEntityMeshCallback
    );
    KGLOG_COM_PROCESS_ERROR(hRetCode);

	{
		g_nCellCount = 0;
		g_pCurrentRegion->TraverseCell(GetCellCount);

		g_nCellCountTotal += g_nCellCount;
		KGLogPrintf(KGLOG_ERR, "Region %d %d has %d Cells !\n", m_pLogicRegion->m_nRegionX, m_pLogicRegion->m_nRegionY,g_nCellCount);
	}


    bResult = true;
Exit0:
    return bResult;
}

BOOL KLogicObstacleGenerator::TraverseEntityMeshCallback(
    float fXStart, float fYStart, float fZStart, 
    LPD3DXMESH piMesh, int nNumSubset,BOOL bSubsetNoCell[],
    const D3DXMATRIX& crTransformationMatrix,
    int nDynamicObstacleGroupIndex,LPSTR pMeshName
)
{
    BOOL            bResult             = false;
    HRESULT         hRetCode            = E_FAIL;
    D3DXVECTOR3*    pPos                = NULL;
    BYTE*           pVertices           = NULL;
    DWORD*          pdwIndices          = NULL;
	WORD*           pwIndices           = NULL;
	DWORD*          pdwAttrib           = NULL;

    DWORD           dwVerticesStride    = 0;
    DWORD           dwNumFaces          = 0;
    DWORD           dwNumVertices       = 0;
	BOOL            b32Bit              = FALSE;

    if (piMesh->GetNumVertices() > USHRT_MAX)
    {
        hRetCode = piMesh->LockIndexBuffer(0, (void**)&pdwIndices);
        KGLOG_COM_PROCESS_ERROR(hRetCode);        
        b32Bit = true;
    }
    else
    {
        hRetCode = piMesh->LockIndexBuffer(0, (void**)&pwIndices);
        KGLOG_COM_PROCESS_ERROR(hRetCode);
        b32Bit = false;
    }

    hRetCode = piMesh->LockVertexBuffer(0, (void**)&pVertices);
    KGLOG_COM_PROCESS_ERROR(hRetCode);
	
	hRetCode = piMesh->LockAttributeBuffer(0, &pdwAttrib);
	KGLOG_COM_PROCESS_ERROR(hRetCode);

    dwVerticesStride    = piMesh->GetNumBytesPerVertex();
    dwNumFaces          = piMesh->GetNumFaces();
    dwNumVertices       = piMesh->GetNumVertices();

    g_nDynamicObstacleGroupIndex = nDynamicObstacleGroupIndex;

    pPos = new D3DXVECTOR3[dwNumVertices];
    KGLOG_PROCESS_ERROR(pPos);

    for (DWORD l = 0; l < dwNumVertices; l++)
    {
        D3DXVECTOR3& vec3Pos = *reinterpret_cast<D3DXVECTOR3*>(pVertices + dwVerticesStride *l);
        D3DXVec3TransformCoord(&pPos[l], &vec3Pos, &crTransformationMatrix);
    }		

    for (DWORD l = 0; l < dwNumFaces; l++)
    {
		DWORD dwSubsetID = pdwAttrib[l];

        if (dwSubsetID >= (DWORD)nNumSubset)
        {
            KGLogPrintf(KGLOG_ERR, "Mesh error %s : dwSubsetID(%u) >= nNumSubset(%u) !\n", pMeshName,dwSubsetID, nNumSubset);
            continue;
        }

	    if(bSubsetNoCell[dwSubsetID])
	    {
            // 这个三角形不需要生成Cell
            continue;
	    }

		if (b32Bit)
		{
			g_TrianglePos[0][0] = pPos[pdwIndices[l * 3]].x;
			g_TrianglePos[0][1] = pPos[pdwIndices[l * 3]].y;
			g_TrianglePos[0][2] = pPos[pdwIndices[l * 3]].z;

			g_TrianglePos[1][0] = pPos[pdwIndices[l * 3 + 1]].x;
			g_TrianglePos[1][1] = pPos[pdwIndices[l * 3 + 1]].y;
			g_TrianglePos[1][2] = pPos[pdwIndices[l * 3 + 1]].z;

			g_TrianglePos[2][0] = pPos[pdwIndices[l * 3 + 2]].x;
			g_TrianglePos[2][1] = pPos[pdwIndices[l * 3 + 2]].y;
			g_TrianglePos[2][2] = pPos[pdwIndices[l * 3 + 2]].z;
		}
		else
		{
			g_TrianglePos[0][0] = pPos[pwIndices[l * 3]].x;
			g_TrianglePos[0][1] = pPos[pwIndices[l * 3]].y;
			g_TrianglePos[0][2] = pPos[pwIndices[l * 3]].z;

			g_TrianglePos[1][0] = pPos[pwIndices[l * 3 + 1]].x;
			g_TrianglePos[1][1] = pPos[pwIndices[l * 3 + 1]].y;
			g_TrianglePos[1][2] = pPos[pwIndices[l * 3 + 1]].z;

			g_TrianglePos[2][0] = pPos[pwIndices[l * 3 + 2]].x;
			g_TrianglePos[2][1] = pPos[pwIndices[l * 3 + 2]].y;
			g_TrianglePos[2][2] = pPos[pwIndices[l * 3 + 2]].z;
		}

        TryOctree(
            fXStart, fYStart, fZStart,
            REGION_GRID_WIDTH, MAX_Z_ALTITUDE, REGION_GRID_HEIGHT
        );
    }

    bResult = true;
Exit0:
    if (piMesh)
	{	
		piMesh->UnlockIndexBuffer();
		piMesh->UnlockVertexBuffer();
		piMesh->UnlockAttributeBuffer();
	}
    KG_DELETE_ARRAY(pPos);
    return bResult;
}


BOOL KLogicObstacleGenerator::GenerateRegionWaterObstacle(float fSceneX_Start,float fSceneZ_Start)
{
    BOOL bResult = false;

    float   fXStart     = 0.0f;
    float   fYStart     = 0.0f;

	fXStart = fSceneX_Start + LOGICAL_CELL_CM_LENGTH * REGION_GRID_WIDTH * m_pLogicRegion->m_nRegionX;
	fYStart = fSceneZ_Start + LOGICAL_CELL_CM_LENGTH * REGION_GRID_HEIGHT * m_pLogicRegion->m_nRegionY;

	for(int nY = 0; nY < REGION_GRID_HEIGHT; nY++)
	{
		for(int nX = 0; nX < REGION_GRID_WIDTH; nX++)
		{
            float fPosX         = fXStart + nX * LOGICAL_CELL_CM_LENGTH + LOGICAL_CELL_CM_LENGTH / 2.0F;
            float fPosY         = fYStart + nY * LOGICAL_CELL_CM_LENGTH + LOGICAL_CELL_CM_LENGTH / 2.0F;
			//float fGround       = 0.0f; 
            int   nWaterHeight  = 0;

			D3DXVECTOR3 Out;
			m_pi3DScene->GetWaterHeight(&Out,&D3DXVECTOR3(fPosX,0,fPosY));
			nWaterHeight = (int)((Out.y - TERRAIN_MIN_HEIGHT) / ALTITUDE_UNIT);
			m_pLogicRegion->SetWaterHeight(nX, nY, (WORD)nWaterHeight);
		}
	}

    bResult = true;
//Exit0:
    return bResult;
}


/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
#define X 0
#define Y 1
#define Z 2

#define CROSS(dest,v1,v2) \
    dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
    dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
    dest[2]=v1[0]*v2[1]-v1[1]*v2[0]; 

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])

#define SUB(dest,v1,v2) \
    dest[0]=v1[0]-v2[0]; \
    dest[1]=v1[1]-v2[1]; \
    dest[2]=v1[2]-v2[2]; 

#define FINDMINMAX(x0,x1,x2,min,max) \
    min = max = x0;   \
    if(x1<min) min=x1;\
    if(x1>max) max=x1;\
    if(x2<min) min=x2;\
    if(x2>max) max=x2;

int planeBoxOverlap(float normal[3],float d, float maxbox[3])
{
    int q;
    float vmin[3],vmax[3];
    for(q=X;q<=Z;q++)
    {
        if(normal[q]>0.0f)
        {
            vmin[q]=-maxbox[q];
            vmax[q]=maxbox[q];
        }
        else
        {
            vmin[q]=maxbox[q];
            vmax[q]=-maxbox[q];
        }
    }
    if(DOT(normal,vmin)+d>0.0f) return 0;
    if(DOT(normal,vmax)+d>=0.0f) return 1;

    return 0;
}


/*======================== X-tests ========================*/
#define AXISTEST_X01(a, b, fa, fb)			   \
    p0 = a*v0[Y] - b*v0[Z];			       	   \
    p2 = a*v2[Y] - b*v2[Z];			       	   \
    if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
    rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_X2(a, b, fa, fb)			   \
    p0 = a*v0[Y] - b*v0[Z];			           \
    p1 = a*v1[Y] - b*v1[Z];			       	   \
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[Y] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

/*======================== Y-tests ========================*/
#define AXISTEST_Y02(a, b, fa, fb)			   \
    p0 = -a*v0[X] + b*v0[Z];		      	   \
    p2 = -a*v2[X] + b*v2[Z];	       	       	   \
    if(p0<p2) {min=p0; max=p2;} else {min=p2; max=p0;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Y1(a, b, fa, fb)			   \
    p0 = -a*v0[X] + b*v0[Z];		      	   \
    p1 = -a*v1[X] + b*v1[Z];	     	       	   \
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Z];   \
    if(min>rad || max<-rad) return 0;

/*======================== Z-tests ========================*/

#define AXISTEST_Z12(a, b, fa, fb)			   \
    p1 = a*v1[X] - b*v1[Y];			           \
    p2 = a*v2[X] - b*v2[Y];			       	   \
    if(p2<p1) {min=p2; max=p1;} else {min=p1; max=p2;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
    if(min>rad || max<-rad) return 0;

#define AXISTEST_Z0(a, b, fa, fb)			   \
    p0 = a*v0[X] - b*v0[Y];				   \
    p1 = a*v1[X] - b*v1[Y];			           \
    if(p0<p1) {min=p0; max=p1;} else {min=p1; max=p0;} \
    rad = fa * boxhalfsize[X] + fb * boxhalfsize[Y];   \
    if(min>rad || max<-rad) return 0;

int triBoxOverlap(float boxcenter[3],float boxhalfsize[3],float triverts[3][3])
{

    /*    use separating axis theorem to test overlap between triangle and box */
    /*    need to test for overlap in these directions: */
    /*    1) the {x,y,z}-directions (actually, since we use the AABB of the triangle */
    /*       we do not even need to test these) */
    /*    2) normal of the triangle */
    /*    3) crossproduct(edge from tri, {x,y,z}-directin) */
    /*       this gives 3x3=9 more tests */
    float v0[3],v1[3],v2[3];
    //float axis[3];
    float min,max,d,p0,p1,p2,rad,fex,fey,fez;  
    float normal[3],e0[3],e1[3],e2[3];

    /* This is the fastest branch on Sun */
    /* move everything so that the boxcenter is in (0,0,0) */
    SUB(v0,triverts[0],boxcenter);
    SUB(v1,triverts[1],boxcenter);
    SUB(v2,triverts[2],boxcenter);

    /* compute triangle edges */
    SUB(e0,v1,v0);      /* tri edge 0 */
    SUB(e1,v2,v1);      /* tri edge 1 */
    SUB(e2,v0,v2);      /* tri edge 2 */

    /* Bullet 3:  */
    /*  test the 9 tests first (this was faster) */
    fex = fabs(e0[X]);
    fey = fabs(e0[Y]);
    fez = fabs(e0[Z]);
    AXISTEST_X01(e0[Z], e0[Y], fez, fey);
    AXISTEST_Y02(e0[Z], e0[X], fez, fex);
    AXISTEST_Z12(e0[Y], e0[X], fey, fex);

    fex = fabs(e1[X]);
    fey = fabs(e1[Y]);
    fez = fabs(e1[Z]);
    AXISTEST_X01(e1[Z], e1[Y], fez, fey);
    AXISTEST_Y02(e1[Z], e1[X], fez, fex);
    AXISTEST_Z0(e1[Y], e1[X], fey, fex);

    fex = fabs(e2[X]);
    fey = fabs(e2[Y]);
    fez = fabs(e2[Z]);
    AXISTEST_X2(e2[Z], e2[Y], fez, fey);
    AXISTEST_Y1(e2[Z], e2[X], fez, fex);
    AXISTEST_Z12(e2[Y], e2[X], fey, fex);

    /* Bullet 1: */
    /*  first test overlap in the {x,y,z}-directions */
    /*  find min, max of the triangle each direction, and test for overlap in */
    /*  that direction -- this is equivalent to testing a minimal AABB around */
    /*  the triangle against the AABB */

    /* test in X-direction */
    FINDMINMAX(v0[X],v1[X],v2[X],min,max);
    if(min>boxhalfsize[X] || max<-boxhalfsize[X]) return 0;

    /* test in Y-direction */
    FINDMINMAX(v0[Y],v1[Y],v2[Y],min,max);
    if(min>boxhalfsize[Y] || max<-boxhalfsize[Y]) return 0;

    /* test in Z-direction */
    FINDMINMAX(v0[Z],v1[Z],v2[Z],min,max);
    if(min>boxhalfsize[Z] || max<-boxhalfsize[Z]) return 0;

    /* Bullet 2: */
    /*  test if the box intersects the plane of the triangle */
    /*  compute plane equation of triangle: normal*x+d=0 */
    CROSS(normal,e0,e1);
    d=-DOT(normal,v0);  /* plane eq: normal.x+d=0 */
    if(!planeBoxOverlap(normal,d,boxhalfsize)) return 0;

    return 1;   /* box and triangle overlaps */
}


// 计算点在向量(P1->P2)的左边还是右边, 返回值大于0在左边,小于0在右边
inline float CheckVectorSide(float fX1, float fY1, float fX2, float fY2, float fX, float fY)
{
    return (fX - fX1) * (fY - fY2) - (fX - fX2) * (fY - fY1);    
}

// 检查Cell的点是否在面的投影三角形内部
inline BOOL CheckPointInProjectionTriangle(float fPosX, float fPosZ)
{
    BOOL bResult = false;

    float fSideA = CheckVectorSide(
        g_TrianglePos[0][0], g_TrianglePos[0][2],
        g_TrianglePos[1][0], g_TrianglePos[1][2],
        fPosX, fPosZ
    );

    float fSideB = CheckVectorSide(
        g_TrianglePos[1][0], g_TrianglePos[1][2],
        g_TrianglePos[2][0], g_TrianglePos[2][2],
        fPosX, fPosZ
    );

    float fSideC = CheckVectorSide(
        g_TrianglePos[2][0], g_TrianglePos[2][2],
        g_TrianglePos[0][0], g_TrianglePos[0][2],
        fPosX, fPosZ
    );

    if (
        (fSideA >= 0.0f && fSideB >= 0.0f && fSideC >= 0.0f) || 
        (fSideA <= 0.0f && fSideB <= 0.0f && fSideC <= 0.0f)
    )
    {
        bResult = true;
    }

    return bResult;
}

int TryOctree(
    float fWorldX, float fWorldY, float fWorldZ,
    int nCellLenX, int nCellLenY, int nCellLenZ
)
{     

    if (nCellLenX == 0 || nCellLenY == 0 || nCellLenZ == 0)
        return 0;

    float center[3] = {
        fWorldX + nCellLenX * LOGICAL_CELL_CM_LENGTH * 0.5f,
        fWorldY + nCellLenY * ALTITUDE_UNIT * 0.5f,
        fWorldZ + nCellLenZ * LOGICAL_CELL_CM_LENGTH * 0.5f
    };

    float boxhalfsize[3] = {    
        nCellLenX * LOGICAL_CELL_CM_LENGTH * 0.5f,
        nCellLenY * ALTITUDE_UNIT * 0.5f,
        nCellLenZ * LOGICAL_CELL_CM_LENGTH * 0.5f
    };

    int ret = triBoxOverlap(center, boxhalfsize, g_TrianglePos);
    if (ret == 0)
        return 0;

    if (nCellLenX == 1 && nCellLenY == 1 && nCellLenZ == 1)
    {
		D3DXVECTOR3 p1(g_TrianglePos[0][0], g_TrianglePos[0][1], g_TrianglePos[0][2]);
		D3DXVECTOR3 p2(g_TrianglePos[1][0], g_TrianglePos[1][1], g_TrianglePos[1][2]);
		D3DXVECTOR3 p3(g_TrianglePos[2][0], g_TrianglePos[2][1], g_TrianglePos[2][2]);
		D3DXPLANE	plane;
        KCell       Cell;

		memset(&Cell, 0, sizeof(Cell));

		D3DXPlaneFromPoints(&plane, &p1, &p3, &p2);

        float fProjectionRadius = sqrt(plane.a * plane.a + plane.c * plane.c);

		if (fProjectionRadius > 0.01f)
		{
            float fProjectioAngel           = acosf(plane.a / fProjectionRadius);

            int   nProjectioLogicDirection  = (int)(fProjectioAngel * DIRECTION_COUNT / (2 * D3DX_PI));
            float fNormalVerticalAngel      = atanf(plane.b / fProjectionRadius);
            int   nNormalVerticalDirection  = (int)(fNormalVerticalAngel * DIRECTION_COUNT / (2 * D3DX_PI));
            int   nSlopeVerticalDirection   = DIRECTION_COUNT / 4 - nNormalVerticalDirection;
            int   nBarrierDirection         = 0;
            DWORD dwFaceUpFlag              = 0;

            if (plane.c < 0)
            {
                nProjectioLogicDirection = DIRECTION_COUNT - nProjectioLogicDirection;
            }

            nProjectioLogicDirection = nProjectioLogicDirection % DIRECTION_COUNT;

            ASSERT(nProjectioLogicDirection >= 0 && nProjectioLogicDirection < DIRECTION_COUNT);

            // 障碍的法线方向是只需要在0到180度范围内取值即可
            nBarrierDirection = nProjectioLogicDirection;
            if (nBarrierDirection >= DIRECTION_COUNT / 2)
            {
                nBarrierDirection -= DIRECTION_COUNT / 2;
            }
            ASSERT(nBarrierDirection >= 0 && nBarrierDirection < DIRECTION_COUNT / 2);

            if (nSlopeVerticalDirection > DIRECTION_COUNT / 4)
            {
                nSlopeVerticalDirection = DIRECTION_COUNT / 2 - nSlopeVerticalDirection;
            }

            if (nSlopeVerticalDirection == DIRECTION_COUNT / 4)
            {
                nSlopeVerticalDirection -= 1;
            }

            ASSERT(nSlopeVerticalDirection >= 0 && nSlopeVerticalDirection < DIRECTION_COUNT / 4);

            dwFaceUpFlag = (nNormalVerticalDirection > 0);

            if (dwFaceUpFlag)
            {
                dwFaceUpFlag = CheckPointInProjectionTriangle(
                    fWorldX + nCellLenX * LOGICAL_CELL_CM_LENGTH * 0.5f, 
                    fWorldZ + nCellLenZ * LOGICAL_CELL_CM_LENGTH * 0.5f 
                );
            }

            if (plane.b < -0.001f) // 法线向下的面不要打滑。
            {
                nSlopeVerticalDirection = 0;
            }

            Cell.m_BaseInfo.dwFaceUp            = dwFaceUpFlag;

            Cell.SetGradientDegree(nSlopeVerticalDirection);
            Cell.SetGradientDirection(nProjectioLogicDirection);
            Cell.SetBarrierDirection(nBarrierDirection);
		}
		else
		{
            Cell.m_BaseInfo.dwFaceUp            = (plane.b > 0);
            Cell.SetGradientDegree(0);
            Cell.SetGradientDirection(0);
            Cell.SetBarrierDirection(0);        
        }

        int nCellX      = (int)(fWorldX / LOGICAL_CELL_CM_LENGTH);
        int nCellY      = (int)(fWorldZ / LOGICAL_CELL_CM_LENGTH);

        Cell.m_wLowLayer  = (WORD)((fWorldY - TERRAIN_MIN_HEIGHT) / ALTITUDE_UNIT);
        Cell.m_wHighLayer = Cell.m_wLowLayer + 1;

        g_pCurrentRegion->AddObstacle(
            nCellX % REGION_GRID_WIDTH, nCellY % REGION_GRID_HEIGHT, 
            Cell, g_nDynamicObstacleGroupIndex
        );

        return 1;
    }

    TryOctree(
        fWorldX,        fWorldY,        fWorldZ,
        nCellLenX / 2,  nCellLenY / 2,  nCellLenZ / 2
    );

    TryOctree(
        fWorldX,        fWorldY,        fWorldZ + LOGICAL_CELL_CM_LENGTH * (nCellLenZ / 2),
        nCellLenX / 2,  nCellLenY / 2,  nCellLenZ - nCellLenZ / 2
    );

    TryOctree(
        fWorldX + LOGICAL_CELL_CM_LENGTH * (nCellLenX / 2),     fWorldY,        fWorldZ,
        nCellLenX - nCellLenX / 2,                              nCellLenY / 2,  nCellLenZ / 2
    );

    TryOctree(
        fWorldX + LOGICAL_CELL_CM_LENGTH * (nCellLenX / 2),     fWorldY,        fWorldZ + LOGICAL_CELL_CM_LENGTH * (nCellLenZ / 2),
        nCellLenX - nCellLenX / 2,                              nCellLenY / 2,  nCellLenZ - nCellLenZ / 2
    );

    TryOctree(
        fWorldX,        fWorldY + ALTITUDE_UNIT * (nCellLenY / 2),    fWorldZ,
        nCellLenX / 2,  nCellLenY - nCellLenY / 2,                    nCellLenZ / 2
    );

    TryOctree(
        fWorldX,        fWorldY + ALTITUDE_UNIT * (nCellLenY / 2),    fWorldZ + LOGICAL_CELL_CM_LENGTH * (nCellLenZ / 2),
        nCellLenX / 2,  nCellLenY - nCellLenY / 2,                    nCellLenZ - nCellLenZ / 2
    );

    TryOctree(
        fWorldX + LOGICAL_CELL_CM_LENGTH * (nCellLenX / 2),     fWorldY + ALTITUDE_UNIT * (nCellLenY / 2),    fWorldZ,
        nCellLenX - nCellLenX / 2,                              nCellLenY - nCellLenY / 2,                    nCellLenZ / 2
    );

    TryOctree(
        fWorldX + LOGICAL_CELL_CM_LENGTH * (nCellLenX / 2),     fWorldY + ALTITUDE_UNIT * (nCellLenY / 2),    fWorldZ + LOGICAL_CELL_CM_LENGTH * (nCellLenZ / 2),
        nCellLenX - nCellLenX / 2,                              nCellLenY - nCellLenY / 2,                    nCellLenZ - nCellLenZ / 2
    );

    return 1;
}
