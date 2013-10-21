////////////////////////////////////////////////////////////////////////////////
//
//  FileName    : KG3DSceneFunctions.h
//  Version     : 1.0
//  Creator     : Chen Tianhong
//  Create Date : 2009-1-4 16:37:11
//  Comment     : 
//
////////////////////////////////////////////////////////////////////////////////

#ifndef _INCLUDE_KG3DSCENEFUNCTIONS_H_
#define _INCLUDE_KG3DSCENEFUNCTIONS_H_

////////////////////////////////////////////////////////////////////////////////
class KG3DSceneSelectionTool;
class KG3DScene;
class KG3DMessageSenderBase;
class KG3DRepresentNPC;

struct KG3DSelectionBBoxGetter 
{
	virtual HRESULT GetBBox(KG3DSceneSelectionTool& selTool, AABBOX& boxRet) = 0;
	virtual ~KG3DSelectionBBoxGetter() = 0{}
};
struct IELogicalSceneConnection;
namespace KG3D_SCENE_FUNCTIONS
{
	HRESULT GetSelectionCenter(KG3DSceneSelectionTool& selTool, D3DXVECTOR3& vCenterRet);	//当没有选择项目的时候会失败,这个使用Box来得到中心的

	KG3DSelectionBBoxGetter* BuildDefaultSelectionBoxGetter();

	HRESULT SetSelectionSFXScaleSize(KG3DSceneSelectionTool& selTool, FLOAT fSize);

	HRESULT FrameMoveSceneRunner(KG3DRepresentNPC& npcToMove, KG3DScene& sceneCur);

	//下面三个函数不能直接使用
	HRESULT SelectionTranslationEntity(KG3DSceneSelectionTool& selTool, const D3DXVECTOR3& Move);
	HRESULT SelectionRotationEntity(KG3DSceneSelectionTool& selTool, const D3DXMATRIX& matRotation);
	HRESULT SelectionScaleEntity(KG3DSceneSelectionTool& selTool, const D3DXVECTOR3& vScaling, KG3DCOORD dwMode);

	VOID SceneToLogicalDirect(const D3DXVECTOR3& vSceneCoord,INT *pX, INT *pY, INT *pZ );//场景坐标到逻辑坐标，直接转换
	VOID LogicalToSceneDirect(const D3DXVECTOR3& vLogic, D3DXVECTOR3& vScene);	//逻辑坐标到场景坐标，直接转换
	POINT  LogicalToLogicalRegionIndex(FLOAT fLogical);	//逻辑坐标到逻辑RegionIndex索引号

	FLOAT GetLogicalHeight(IELogicalSceneConnection& logicalCnt, const D3DXVECTOR3& vLogicalCoord, size_t* pOrderOfCell);	//由逻辑坐标得到逻辑高度，失败返回-FLT_MAX。另外如果传入pOrderOfCell，可以知道当前在哪个Cell那里。这个Cell要不比当前高度低，要不包含当前高度
	
};

namespace KSF = KG3D_SCENE_FUNCTIONS;



#endif //_INCLUDE_KG3DSCENEFUNCTIONS_H_
