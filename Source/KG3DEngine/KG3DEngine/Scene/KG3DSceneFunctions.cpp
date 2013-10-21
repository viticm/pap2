////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneFunctions.cpp
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-1-4 16:36:39
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "KG3DSceneFunctions.h"

#include "KG3DSceneSelectionTool.h"
#include "KG3DRepresentObject.h"
#include "KG3DIntersection.h"
#include "KG3DSceneBlock.h"
#include "KG3DScene.h"

#include "KG3DRepresentNPC.h"//FrameMoveSceneRunner

//#include "../../../include/SO3World/KRegion.h"//<SO3World Refactor>
#include "IEEditor.h"//IELogicalSceneConnection

////////////////////////////////////////////////////////////////////////////////
extern const KG3DLogicalConstances& g_GetLogicalConstants();
namespace KG3D_SCENE_FUNCTIONS
{
	HRESULT GetSelectionCenter(KG3DSceneSelectionTool& selTool, D3DXVECTOR3& vCenterRet )	//这个是新旧选择器都用的
	{
		ULONG uCount = selTool.GetSelectionCount();
		KG_PROCESS_ERROR(uCount != 0);

		{
			//新旧代码都有，所以要注意对Entity和Obj都写一下
			struct ObjAndEntityVisitor : public KG3DRepresentObjectVisitor, public KG3DSceneEntityVisitor
			{
				D3DXVECTOR3	m_vCenterSum;
				UINT		m_visitedCount;
				ObjAndEntityVisitor():m_vCenterSum(0,0,0), m_visitedCount(0){}
				HRESULT Accept(KG3DRepresentObject& obj)
				{
					m_vCenterSum += obj.GetWorldPos();
					++m_visitedCount;
					return S_OK;
				}
				HRESULT Accept(KG3DSceneEntity& entity)
				{
					AABBOX box;
					HRESULT hr = entity.GetBBox(&box);
					if (SUCCEEDED(hr))
					{
						m_vCenterSum += box.GetCenter();
						++m_visitedCount;
					}
					return S_OK;
				}
			};

			ObjAndEntityVisitor visitorTemp;
			selTool.TraverseSelected(static_cast<KG3DRepresentObjectVisitor&>(visitorTemp));
			selTool.TraverseSelected(static_cast<KG3DSceneEntityVisitor&>(visitorTemp));

			vCenterRet = visitorTemp.m_vCenterSum / static_cast<FLOAT>(visitorTemp.m_visitedCount);
		}

		return S_OK;
Exit0:
		return E_FAIL;
	}
	HRESULT SelectionScaleEntity( KG3DSceneSelectionTool& selTool, const D3DXVECTOR3& vScaling, KG3DCOORD dwMode )//这个是只旧选择器都用的
	{
		if (dwMode == KG3DCOORD_WORLD || selTool.GetSelectionCount() > 1)
		{
			/*for (std::list<KG3DSceneEntity>::iterator i = listEntitys.begin();
			i != listEntitys.end(); ++i)*/

			TypeEntityAccessProxy& proxy = selTool.GetSelectionAccessProxy();
			for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
			{
				D3DXVECTOR3 vTrans;
				D3DXMATRIX  mTrans;
				D3DXMATRIX  mTransInv;
				AABBOX box;
				D3DXMATRIX mat;
				D3DXVECTOR3 vScal = vScaling;

				KG3DSceneEntity& entity = proxy.GetCur();
				
				///////如果是DOODAD就不进行缩放////////////////////////////////////////////////////////////////
				if((entity.GetType() ==  SCENEENTITY_OBJECT) && entity.m_pPointer) 
				{
					KG3DRepresentObject* pObject = static_cast<KG3DRepresentObject*>(entity.m_pPointer);
					if(pObject->GetType() == REPRESENTOBJECT_DOODAD)
						continue;
				}
				//////////////////////////////////////////////////////////////////////////////////////////

				D3DXMATRIX matWorld = entity.GetWorldMatrix();
				D3DXMATRIX matLocal = entity.GetMatrix();
				D3DXMATRIX matParentInv;
				D3DXMatrixIdentity(&matParentInv);

				if (matWorld != matLocal)
				{
					D3DXMATRIX matParent;
					D3DXMATRIX matLocalInv;
					D3DXMatrixInverse(&matLocalInv, NULL, &matLocal);
					matParent = matLocalInv * matWorld;
					D3DXMatrixInverse(&matParentInv, NULL, &matParent);
				}

				entity.GetBBox(&box);

				vScal = D3DXVECTOR3(1.f, 1.f, 1.f) + vScal / box.GetDiameter();
				D3DXMatrixScaling(&mat, vScal.x, vScal.y, vScal.z);

				vTrans.x = matWorld._41;
				vTrans.y = matWorld._42;
				vTrans.z = matWorld._43;

				D3DXMatrixTranslation(&mTrans, vTrans.x, vTrans.y, vTrans.z);
				D3DXMatrixTranslation(&mTransInv, -vTrans.x, -vTrans.y, -vTrans.z);

				mat = matWorld * mTransInv * mat * mTrans * matParentInv;

				entity.SetLocalMatrix(mat);
				entity.UpdateTransformation();

				entity.FrameMove();
			}
		}
		else
		{
			TypeEntityAccessProxy& proxy = selTool.GetSelectionAccessProxy();
			proxy.Begin();

			KG3DSceneEntity Entity = proxy.GetCur();

			D3DXMATRIX matWorld = Entity.GetWorldMatrix();
			D3DXMATRIX matLocal = Entity.GetMatrix();
			D3DXMATRIX matParentInv;
			D3DXMatrixIdentity(&matParentInv);

			D3DXVECTOR3 vScal = vScaling;
			AABBOX box;
			D3DXMATRIX mat;

			Entity.GetBBox(&box);

			if (matWorld != matLocal)
			{
				D3DXMATRIX matParent;
				D3DXMATRIX matLocalInv;
				D3DXMatrixInverse(&matLocalInv, NULL, &matLocal);
				matParent = matLocalInv * matWorld;
				D3DXMatrixInverse(&matParentInv, NULL, &matParent);
			}

			D3DXMATRIX mWorldInv;
			D3DXMatrixInverse(&mWorldInv, NULL, &matWorld);
			float fLength = D3DXVec3Length(&vScal);
			D3DXVec3TransformNormal(&vScal, &vScal, &mWorldInv);
			D3DXVec3Normalize(&vScal, &vScal);
			vScal *= fLength;
			vScal = D3DXVECTOR3(1.f, 1.f, 1.f) + vScal / box.GetDiameter();
			D3DXMATRIX mScal;
			D3DXMatrixScaling(&mScal, vScal.x, vScal.y, vScal.z);
			mat = mScal * matWorld * matParentInv;

			Entity.SetLocalMatrix(mat);
			Entity.UpdateTransformation();

			Entity.FrameMove();
		}
		return S_OK;
	}

	HRESULT SelectionRotationEntity( KG3DSceneSelectionTool& selTool, const D3DXMATRIX& matRotation )//这个是只旧选择器都用的
	{
		/*for (std::list<KG3DSceneEntity>::iterator i = listEntitys.begin();
		i != listEntitys.end(); ++i)*/
		TypeEntityAccessProxy& proxy = selTool.GetSelectionAccessProxy();
		for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
		{
			KG3DSceneEntity& entity = proxy.GetCur();
			D3DXMATRIX matWorld = entity.GetWorldMatrix();
			D3DXMATRIX matLocal = entity.GetMatrix();
			D3DXMATRIX matParentInv;
			D3DXMatrixIdentity(&matParentInv);

			D3DXMATRIX mat = matRotation;

			if (matWorld != matLocal)
			{
				D3DXMATRIX matParent;
				D3DXMATRIX matLocalInv;
				D3DXMatrixInverse(&matLocalInv, NULL, &matLocal);
				matParent = matLocalInv * matWorld;
				D3DXMatrixInverse(&matParentInv, NULL, &matParent);
			}

			D3DXVECTOR3 vTrans;
			D3DXMATRIX  mTrans;
			D3DXMATRIX  mTransInv;

			vTrans.x = matWorld._41;
			vTrans.y = matWorld._42;
			vTrans.z = matWorld._43;

			D3DXMatrixTranslation(&mTrans, vTrans.x, vTrans.y, vTrans.z);
			D3DXMatrixTranslation(&mTransInv, -vTrans.x, -vTrans.y, -vTrans.z);

			mat = matWorld * mTransInv * mat * mTrans * matParentInv;

			entity.SetLocalMatrix(mat);
			entity.UpdateTransformation();

			entity.FrameMove();
		}
		return S_OK;
	}

	HRESULT SelectionTranslationEntity(KG3DSceneSelectionTool& selTool, const D3DXVECTOR3& Move)//这个是只旧选择器都用的
	{
		/*for (std::list<KG3DSceneEntity>::iterator i = listEntitys.begin();
		i != listEntitys.end(); ++i)*/
		TypeEntityAccessProxy& proxy = selTool.GetSelectionAccessProxy();
		for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
		{
			KG3DSceneEntity& entity = proxy.GetCur();

			D3DXMATRIX matWorld = entity.GetWorldMatrix();
			D3DXMATRIX matLocal = entity.GetMatrix();
			D3DXVECTOR3 vMove = Move;

			if (matWorld != matLocal)
			{
				D3DXMATRIX matParent;
				D3DXMATRIX matLocalInv;
				D3DXMATRIX matParentInv;

				D3DXMatrixInverse(&matLocalInv, NULL, &matLocal);
				matParent = matLocalInv * matWorld;
				D3DXMatrixInverse(&matParentInv, NULL, &matParent);
				D3DXVec3TransformNormal(&vMove, &vMove, &matParentInv);
			}

			D3DXVECTOR3 vTrans;
			entity.GetTranslation(&vTrans);
			vTrans += vMove;
			entity.SetTranslation(&vTrans);
			entity.UpdateTransformation();
			entity.FrameMove();
		}
		return S_OK;
	}

	KG3DSelectionBBoxGetter* BuildDefaultSelectionBoxGetter()
	{
		struct KG3DSelectionBBoxGetterSolid : public KG3DSelectionBBoxGetter
		{
			virtual HRESULT GetBBox(KG3DSceneSelectionTool& selTool, AABBOX& boxRet)
			{
				ULONG uCount = selTool.GetSelectionCount();
				KG_PROCESS_ERROR(uCount != 0);

				{
					//新旧代码都有，所以要注意对Entity和Obj都写一下
					struct ObjAndEntityVisitor : public KG3DRepresentObjectVisitor, public KG3DSceneEntityVisitor
					{
						AABBOX boxAll;
						ObjAndEntityVisitor(){ boxAll.Clear(); }
						HRESULT Accept(KG3DRepresentObject& obj)
						{
							const AABBOX& box = obj.GetBBox();
							boxAll.AddPosition(box.A);
							boxAll.AddPosition(box.B);
							return S_OK;
						}
						HRESULT Accept(KG3DSceneEntity& entity)
						{
							AABBOX box;
							HRESULT hr = entity.GetBBox(&box);
							if (SUCCEEDED(hr))
							{
								boxAll.AddPosition(box.A);
								boxAll.AddPosition(box.B);
							}
							return S_OK;
						}
					};

					ObjAndEntityVisitor visitorTemp;

					selTool.TraverseSelected(static_cast<KG3DRepresentObjectVisitor&>(visitorTemp));
					selTool.TraverseSelected(static_cast<KG3DSceneEntityVisitor&>(visitorTemp));

					boxRet = visitorTemp.boxAll;

					return S_OK;
				}
Exit0:
				return E_FAIL;
			}
		};

		return new KG3DSelectionBBoxGetterSolid;
	}

	HRESULT SetSelectionSFXScaleSize( KG3DSceneSelectionTool& selTool, FLOAT fSize )
	{
		/*KG3DSceneEntityList::iterator i = m_listSelectedEntity.begin();
		while (i != m_listSelectedEntity.end())*/

		TypeEntityAccessProxy& proxy = selTool.GetSelectionAccessProxy();
		for(proxy.Begin(); ! proxy.IsEnd(); proxy.StepForward())
		{
			KG3DSceneEntity &Entity = proxy.GetCur();
			float fValue;
			Entity.GetScaleSize(&fValue);
			fValue *= fSize;
			Entity.SetScaleSize(fValue);
			//i++;
		}
		return S_OK;
	}
	
	void SetNPCRotation(KG3DRepresentNPC &npcToMove, float s_fPerFrameRunAngle, KG3DScene &sceneCur, float s_fPerFrameJumpAngle, float s_fPerFrameStandAngle )
	{
		{
			static D3DXVECTOR3 s_vec3LastPos = npcToMove.m_vecCurPosition;
			static D3DXQUATERNION s_LastDir(0.0f, 1.0f, 0.0f, 0.0f);
			float fAngle = .0f;
			static float s_fLastAngle = .0f;
			float fSlerpWeight = s_fPerFrameRunAngle;
			D3DXVECTOR3 vec3Axis;
			D3DXVECTOR3 vec3Dir, vec3NewDir;

			D3DXVec3Normalize(&vec3Dir, &npcToMove.m_vecDirection);
			D3DXQUATERNION quatRotation, quatRotationOrg, quatDir;
			D3DXQuaternionRotationYawPitchRoll(&quatRotationOrg, npcToMove.m_fAngleHor, 0.0f, 0.0f);

			D3DXVECTOR3 vec3SampleB = npcToMove.m_vecCurPosition + D3DXVECTOR3(.0f, 10.0f, .0f);
			D3DXVECTOR3 vec3FloorB = vec3SampleB;
			//sceneCur.GetFloor(&vec3FloorB, &vec3SampleB);
			sceneCur.GetFloor(&vec3SampleB, &vec3FloorB.y);

			if (abs(vec3FloorB.y - npcToMove.m_vecCurPosition.y) > 1.0f)
			{
				fSlerpWeight = s_fPerFrameJumpAngle;
				D3DXVECTOR3 vec3Up(.0f, 1.f, .0f);
				D3DXVec3Cross(&vec3Axis, &npcToMove.m_vecDirection, &vec3Up);
				fAngle = 0.0f;
			}
			else
			{
				D3DXVECTOR3 vec3SampleA = npcToMove.m_vecCurPosition + vec3Dir * .1f + D3DXVECTOR3(0, 10.0f, 0);

				//sceneCur.GetFloor(&vec3FloorA, &vec3SampleA);
				D3DXVECTOR3 vec3FloorA = vec3SampleA;
				sceneCur.GetFloor(&vec3SampleA, &vec3FloorA.y);

				vec3NewDir = vec3FloorA - npcToMove.m_vecCurPosition;
				D3DXVec3Normalize(&vec3NewDir, &vec3NewDir);
				D3DXVec3Cross(&vec3Axis, &npcToMove.m_vecDirection, &vec3NewDir);
				D3DXVec3Normalize(&vec3Axis, &vec3Axis);
				FLOAT fDotRet = D3DXVec3Dot(&vec3Dir, &vec3NewDir);
				MT_LIMIT_TO_RANGE(fDotRet, 1.f, 0.f);
				fAngle = acos(fDotRet);
				if (s_vec3LastPos == npcToMove.m_vecCurPosition && 
					abs(fAngle - s_fLastAngle) < 0.00001 )
				{
					fSlerpWeight = s_fPerFrameStandAngle;
				}
			}
			D3DXQuaternionRotationAxis(&quatDir, &vec3Axis, fAngle);
			D3DXQuaternionSlerp(&quatDir, &s_LastDir, &quatDir, fSlerpWeight);
			if (IS_FLOAT_VALID(quatDir.x) && IS_FLOAT_VALID(quatDir.y) && IS_FLOAT_VALID(quatDir.z))//这里有溢出现象，先这么纠正着
			{
				s_LastDir = quatDir;
				quatRotation = quatRotationOrg * quatDir;
				npcToMove.m_vNewDir = vec3NewDir;
				npcToMove.m_vAxis = vec3Axis;
				s_vec3LastPos = npcToMove.m_vecCurPosition;
				s_fLastAngle = fAngle;
				npcToMove.SetRotation(&quatRotation);
			}
			else
			{
				//_ASSERTE(NULL && _T("数值溢出"));
			}
		}
	}

	HRESULT FrameMoveSceneRunner(KG3DRepresentNPC& npcToMove, KG3DScene& sceneCur )
	{
		static float s_fPerFrameStandAngle = 0.1f;
		static float s_fPerFrameRunAngle = 0.02f;
		static float s_fPerFrameJumpAngle = 0.2f;

		float fDelayTime    = 0.0f;

		D3DXVECTOR3 vecPosition;
		D3DXVECTOR3 vecUpSpeed;

		static POINT scMousePos;
		POINT mousePos;

		GetCursorPos(&mousePos);

		fDelayTime = g_cGraphicsTool.GetTinyTime() / 1000.0f;
		KG_PROCESS_ERROR(fDelayTime != 0.0f);

		if (npcToMove.m_dwNpcType == NPC_TYPE_PROTAGONIST)
		{
			npcToMove.m_dwMovementType = (DWORD)NPC_MOVEMENT_TYPE_STAND;
			if (npcToMove.m_eNPCState & KG3DRepresentNPC::nmsFoward)
				npcToMove.GoAhead(npcToMove.m_fMoveSpeed * fDelayTime, sceneCur);
			if (npcToMove.m_eNPCState & KG3DRepresentNPC::nmsBack)
				npcToMove.BackOff(npcToMove.m_fMoveSpeed * fDelayTime, sceneCur);
			{
				if (npcToMove.m_eNPCState & KG3DRepresentNPC::nmsTurnLeft)
					npcToMove.TurnLef(2.5f * fDelayTime, false);
				else if (npcToMove.m_eNPCState & KG3DRepresentNPC::nmsMoveLeft)
					npcToMove.MoveLef(npcToMove.m_fMoveSpeed * fDelayTime, sceneCur);
				else if (npcToMove.m_eNPCState & KG3DRepresentNPC::nmsTurnBodyLeft)
					npcToMove.TurnLef(1.5f * fDelayTime, true);
			}
			{
				if (npcToMove.m_eNPCState & KG3DRepresentNPC::nmsTurnRight)
					npcToMove.TurnRig(2.5f * fDelayTime, false);
				else if (npcToMove.m_eNPCState & KG3DRepresentNPC::nmsMoveRight)
					npcToMove.MoveRig(npcToMove.m_fMoveSpeed * fDelayTime, sceneCur);
				else if (npcToMove.m_eNPCState & KG3DRepresentNPC::nmsTurnBodyRight)
					npcToMove.TurnRig(1.5f * fDelayTime, true);
			}
			if (npcToMove.m_eNPCState & KG3DRepresentNPC::nmsJump)
				npcToMove.Jump(2500.f);
			if ((npcToMove.m_eNPCState & KG3DRepresentNPC::nmsCamareTurn) && 
				(scMousePos.x != mousePos.x))
			{
				HRESULT hRetCode = E_FAIL;
				IEKG3DSceneOutputWnd *piSceneCurOutputWnd = NULL;
				hRetCode = sceneCur.GetCurOutputWnd(&piSceneCurOutputWnd);
				KGLOG_COM_PROCESS_ERROR(hRetCode);
				if(piSceneCurOutputWnd)
				{
					if (::GetFocus() == static_cast<KG3DSceneOutputWnd *>(piSceneCurOutputWnd)->m_hWnd)
					{
						float fAngel = npcToMove.m_fAngelRateHor * (mousePos.x - scMousePos.x);
						npcToMove.TurnRig(fAngel);
					}
				}
			}
			if (npcToMove.m_eNPCState & KG3DRepresentNPC::nmsSpeedUp)		
			{
				npcToMove.m_fMoveSpeed += 50.0f;
				npcToMove.m_fMoveSpeed = min(10000.0f, npcToMove.m_fMoveSpeed);
			}
			if (npcToMove.m_eNPCState & KG3DRepresentNPC::nmsSpeedDown)
			{
				npcToMove.m_fMoveSpeed -= 50.0f;
				npcToMove.m_fMoveSpeed = max(100.0f, npcToMove.m_fMoveSpeed);
			}

		}

		if (npcToMove.m_vecPrePosition != npcToMove.m_vecCurPosition)
		{
			vecPosition = npcToMove.m_vecCurPosition;
			npcToMove.m_fUpSpeed  = npcToMove.m_fUpSpeed - 3000.0f * fDelayTime;
			vecUpSpeed  = D3DXVECTOR3(0.0f, npcToMove.m_fUpSpeed * fDelayTime, 0.0f);
			vecPosition += vecUpSpeed;
			npcToMove.SetCurPosition(vecPosition, sceneCur, true);
		}

		SetNPCRotation(npcToMove, s_fPerFrameRunAngle, sceneCur, s_fPerFrameJumpAngle, s_fPerFrameStandAngle);



		//if (m_pAperture)
		//	m_pAperture->FrameMove();
		//Exit1:
		scMousePos = mousePos;
Exit0:
		for (DWORD i = 0; i < npcToMove.GetModelCount(); i++)
		{
			KG3DModel* pModel = npcToMove.GetModel(i);
			if(NULL != pModel)
			{
				pModel->GetRenderEnviromentParamenter(&sceneCur);
			}
		}
		return npcToMove.KG3DRepresentObject::FrameMove();	
	}

	
	VOID SceneToLogicalDirect(const D3DXVECTOR3& vSceneCoord,INT *pX, INT *pY, INT *pZ )
	{
		//注意两者的坐标的顺序是不一样的。
		const INT nLogicalXStart = 0;
		const INT nLogicalZStart = 0;
		/*if (pX)
			*pX = (int)((vSceneCoord.x - nLogicalXStart * 100) * CELL_LENGTH / LOGICAL_CELL_CM_LENGTH);
		if (pY)
			*pY = (int)((vSceneCoord.z - nLogicalZStart * 100) * CELL_LENGTH / LOGICAL_CELL_CM_LENGTH);
		if (pZ)
			*pZ = (int)((vSceneCoord.y - TERRAIN_MIN_HEIGHT) * POINT_PER_ALTITUDE / ALTITUDE_UNIT);*/

		const KG3DLogicalConstances& lgC = g_GetLogicalConstants();
		_ASSERTE(0 != lgC.logicalCellCmLength);
		if (pX)
			*pX = (int)((vSceneCoord.x - nLogicalXStart * 100) * lgC.cellLength / lgC.logicalCellCmLength);
		if (pY)
			*pY = (int)((vSceneCoord.z - nLogicalZStart * 100) * lgC.cellLength / lgC.logicalCellCmLength);
		if (pZ)
			*pZ = (int)((vSceneCoord.y - lgC.terrainMinHeight) * lgC.pointPerAltitude / lgC.altitudeUnit);
	}

	VOID LogicalToSceneDirect( const D3DXVECTOR3& vLogic, D3DXVECTOR3& vScene )
	{
		//注意两者的坐标的顺序是不一样的。
		const INT nLogicalXStart = 0;
		const INT nLogicalZStart = 0;

		const KG3DLogicalConstances& lgC = g_GetLogicalConstants();
		_ASSERTE(0 != lgC.cellLength);
		/*vScene.x = vLogic.x * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH + nLogicalXStart * 100;
		vScene.z = vLogic.y * LOGICAL_CELL_CM_LENGTH / CELL_LENGTH + nLogicalZStart * 100;
		vScene.y = vLogic.z / POINT_PER_ALTITUDE * ALTITUDE_UNIT + TERRAIN_MIN_HEIGHT;*/
		vScene.x = vLogic.x * lgC.logicalCellCmLength / lgC.cellLength + nLogicalXStart * 100;
		vScene.z = vLogic.y * lgC.logicalCellCmLength / lgC.cellLength + nLogicalZStart * 100;
		vScene.y = vLogic.z / lgC.pointPerAltitude * lgC.altitudeUnit+ lgC.terrainMinHeight;
	}

	

	FLOAT GetLogicalHeight( IELogicalSceneConnection& logicalCnt, const D3DXVECTOR3& vLogicalCoord, size_t* pOrderOfCell)
	{
		/*POINT regionIndex = LogicalToLogicalRegionIndex(vLogicalCoord.x, vLogicalCoord.y);
		KRegion* pRegion = logicalCnt.GetRegion(regionIndex.x, regionIndex.y);
		KG_PROCESS_ERROR(NULL != pRegion);
		{
			INT nCellIndexLocalX = (INT)(vLogicalCoord.x / CELL_LENGTH) % REGION_GRID_WIDTH;
			INT nCellIndexLocalY = (INT)(vLogicalCoord.y / CELL_LENGTH) % REGION_GRID_HEIGHT;
			std::pair<KCell*, size_t> pairCell_Order = GetCurOrLowerObstacle(*pRegion, nCellIndexLocalX, nCellIndexLocalY, (INT)vLogicalCoord.z);
			KCell* pCell = pairCell_Order.first;
			KG_PROCESS_ERROR(NULL != pCell);

			if (NULL != pOrderOfCell)
			{
				*pOrderOfCell = pairCell_Order.second;
			}

			return (FLOAT)(pCell->m_wHighLayer * POINT_PER_ALTITUDE);
		}
Exit0:
		return -FLT_MAX;*/	//代码全部移动到IELogicalSceneConnection

		return logicalCnt.GetLogicalHeight(vLogicalCoord, pOrderOfCell);
	}
};

