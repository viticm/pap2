////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KSceneHelperFuncs.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2008-4-21 9:41:19
/*  Comment     : 在SSE暴露的接口中，不再支持直接用不同的函数生成Obj。
无论创建什么类型的RepresentObject，都是一个BuildKG3DRepresentObject。参数都
包含在结构体里面，这样子增减参数很方便，能够实现批量的不同种类的RepresentObject
的读取，接口维护会更加容易（因为具体的行为是生成类自己处理的）。
为了方便使用，在外部再重新包装成多参数的函数。这样子好像多此一举，但是事实上是
降低了引擎和编辑器之间的耦合性的，KG3DSceneSceneEditor中可以不维护这些生成OBJ的
行为。而外部使用和维护依然很方便。
*/
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KSCENEHELPERFUNCS_H_
#define _INCLUDE_KSCENEHELPERFUNCS_H_

interface IEKG3DEngineManager;
interface IEKG3DRepresentObject;

////////////////////////////////////////////////////////////////////////////////
//这种方法集合到接口中只会让接口更加复杂，分离成一个辅助函数+内部一个Factory，能让接口简化很多
IEKG3DRepresentObject* BuildTrafficPoint(IEKG3DEngineManager* pManager, DWORD dwSetID, DWORD dwType, LPCTSTR lpName);
IEKG3DRepresentObject* BuildWayPoint(IEKG3DEngineManager* pManager, DWORD dwSetID, LPCTSTR lpName);
IEKG3DRepresentObject* BuildRefreshPoint(IEKG3DEngineManager* pManager, DWORD dwType, DWORD dwSetID, LPCTSTR lpName);
IEKG3DRepresentObject* BuildRepresentDoodad(IEKG3DEngineManager* pManager, DWORD dwDoodadTempleteID,
								   LPCTSTR pDoodadName,
								   LPCTSTR pMeshName,
								   LPCTSTR pMtlName,
								   LPCTSTR pAniName,
								   DWORD dwReliveID );
IEKG3DRepresentObject* BuildRepresentNPC(IEKG3DEngineManager* pManager
								, DWORD dwNPCTempleteID
								, LPCTSTR pNPCName
								, LPCTSTR pMdlName
								, LPCTSTR pAniName
								, LPCTSTR pFaceMesh
								, LPCTSTR pFaceMtl
								, int nLevel
								, int nType
								//, int nAIType
								, float fScale
								, DWORD dwRepresentID
								, float nAlertRange
								, float nAttackRange
								, DWORD dwReliveID
								, DWORD dwRandomID
								, DWORD dwAISet);

#endif //_INCLUDE_KSCENEHELPERFUNCS_H_
